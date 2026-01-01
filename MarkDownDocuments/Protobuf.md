# Protocol Buffers (Protobuf) in Modern C++

## What is Protocol Buffers?

Protocol Buffers (protobuf) is Google's language-neutral, platform-neutral, extensible mechanism for serializing structured data. Think of it as XML or JSON, but smaller, faster, and type-safe.

## Why Use Protobuf?

### Advantages
- ✅ **Efficient**: 3-10x smaller than XML, 20-100x faster parsing
- ✅ **Type-safe**: Strong typing with generated code
- ✅ **Versioned**: Schema evolution with backward compatibility
- ✅ **Cross-language**: C++, Python, Java, Go, JavaScript, and more
- ✅ **Validated**: Automatic validation of message structure
- ✅ **Production-proven**: Used by Google, Netflix, Uber, Square

### Use Cases
- **Microservices**: Inter-service communication (gRPC)
- **IoT/Embedded**: Efficient data transmission
- **Mobile apps**: Network protocol serialization
- **Data storage**: Compact binary format
- **Configuration**: Type-safe config files
- **Logging**: Structured log messages

## Installation

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install protobuf-compiler libprotobuf-dev
```

### macOS
```bash
brew install protobuf
```

### Windows
Download from: https://github.com/protocolbuffers/protobuf/releases

Verify installation:
```bash
protoc --version
```

## Quick Start

### 1. Define Your Schema (.proto file)

Create `person.proto`:
```protobuf
syntax = "proto3";

package example;

message Person {
  string name = 1;
  int32 age = 2;
  string email = 3;
}
```

### 2. Generate C++ Code

```bash
protoc --cpp_out=. person.proto
```

This generates:
- `person.pb.h` - Header file with class definitions
- `person.pb.cc` - Implementation file

### 3. Use in C++ Code

```cpp
#include "person.pb.h"
#include <iostream>
#include <fstream>

int main() {
    // Create and populate message
    example::Person person;
    person.set_name("Alice");
    person.set_age(30);
    person.set_email("alice@example.com");
    
    // Serialize to string
    std::string serialized;
    person.SerializeToString(&serialized);
    
    std::cout << "Serialized size: " << serialized.size() << " bytes\n";
    
    // Deserialize
    example::Person person2;
    person2.ParseFromString(serialized);
    
    std::cout << "Name: " << person2.name() << "\n";
    std::cout << "Age: " << person2.age() << "\n";
    std::cout << "Email: " << person2.email() << "\n";
    
    return 0;
}
```

### 4. Compile

```bash
g++ -std=c++17 main.cpp person.pb.cc -lprotobuf -o app
./app
```

## Advanced Features

### Nested Messages
```protobuf
message Address {
  string street = 1;
  string city = 2;
  string country = 3;
}

message Person {
  string name = 1;
  Address address = 2;  // Nested message
}
```

### Repeated Fields (Arrays)
```protobuf
message PhoneBook {
  repeated Person contacts = 1;  // Array of Person
}
```

### Enumerations
```protobuf
enum PhoneType {
  MOBILE = 0;
  HOME = 1;
  WORK = 2;
}

message PhoneNumber {
  string number = 1;
  PhoneType type = 2;
}
```

### Maps
```protobuf
message UserPreferences {
  map<string, string> settings = 1;
}
```

### Oneof (Union Types)
```protobuf
message SensorReading {
  oneof measurement {
    float temperature = 1;
    float pressure = 2;
    float humidity = 3;
  }
}
```

## Performance Optimization

### 1. Arena Allocation
For high-performance scenarios:
```cpp
google::protobuf::Arena arena;
auto* person = google::protobuf::Arena::CreateMessage<Person>(&arena);
person->set_name("Alice");
// All memory freed when arena goes out of scope
```

### 2. Message Reuse
```cpp
Person person;
for (int i = 0; i < 1000; ++i) {
    person.Clear();  // Reuse, don't recreate
    person.set_name("Person " + std::to_string(i));
    // ... serialize and send ...
}
```

### 3. Lite Runtime
For embedded systems, use lite runtime in .proto:
```protobuf
option optimize_for = LITE_RUNTIME;
```
- 50-70% smaller binary
- Removes reflection, text format, JSON
- Still fully functional for serialization

## Embedded Systems

### Fixed-Size Buffers
```cpp
constexpr size_t MAX_SIZE = 256;
uint8_t buffer[MAX_SIZE];

Person person;
person.set_name("Alice");

size_t size = person.ByteSizeLong();
if (size <= MAX_SIZE) {
    person.SerializeToArray(buffer, size);
    // uart_transmit(buffer, size);
}
```

### Zero Dynamic Allocation
```cpp
// Pre-allocate everything
static uint8_t tx_buffer[256];
static uint8_t rx_buffer[256];

// Serialize directly to UART buffer
person.SerializeToArray(tx_buffer, person.ByteSizeLong());
```

## JSON Conversion

Protobuf v3 supports JSON (for debugging/REST APIs):
```cpp
#include <google/protobuf/util/json_util.h>

Person person;
person.set_name("Alice");

// To JSON
std::string json;
google::protobuf::util::MessageToJsonString(person, &json);
std::cout << json << "\n";

// From JSON
std::string json_input = R"({"name":"Bob","age":25})";
Person person2;
google::protobuf::util::JsonStringToMessage(json_input, &person2);
```

## Schema Evolution

Protobuf supports backward and forward compatibility:

### Rules for Compatibility
✅ **Can add new fields** - old code ignores them  
✅ **Can remove optional fields** - new code uses defaults  
✅ **Can rename fields** - only affects code, not wire format  
❌ **Cannot change field numbers** - breaks everything  
❌ **Cannot change field types** - incompatible  

### Example
```protobuf
// Version 1
message User {
  string name = 1;
  int32 age = 2;
}

// Version 2 (backward compatible)
message User {
  string name = 1;
  int32 age = 2;
  string email = 3;      // NEW FIELD - old code ignores
  int32 score = 4;       // NEW FIELD
  // removed 'phone' field 5  // Old field number reserved
}
```

## Comparison with Alternatives

| Feature | Protobuf | JSON | XML | FlatBuffers | Cap'n Proto |
|---------|----------|------|-----|-------------|-------------|
| Binary size | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Parse speed | ⭐⭐⭐⭐ | ⭐⭐ | ⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Human-readable | ⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐ | ⭐ |
| Schema | ⭐⭐⭐⭐⭐ | ⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Ecosystem | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| Zero-copy | ❌ | ❌ | ❌ | ✅ | ✅ |

## CMake Integration

```cmake
find_package(Protobuf REQUIRED)

# Generate C++ code from .proto files
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    proto/sensor_data.proto
)

add_executable(my_app
    src/main.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_link_libraries(my_app
    protobuf::libprotobuf
)

target_include_directories(my_app PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated headers
)
```

## Best Practices

### DO:
✅ Use meaningful field names  
✅ Reserve deleted field numbers  
✅ Use enums for limited value sets  
✅ Group related fields with nested messages  
✅ Use repeated fields for arrays  
✅ Document your .proto files  
✅ Version your schemas  
✅ Use lite runtime for embedded systems  

### DON'T:
❌ Reuse field numbers  
❌ Change field types  
❌ Use required fields (proto2 only)  
❌ Serialize to/from text in production  
❌ Ignore backward compatibility  
❌ Forget to call ShutdownProtobufLibrary()  

## Real-World Example: IoT Sensor Network

See [ProtobufExample.cpp](../src/ProtobufExample.cpp) for a complete example demonstrating:
- Sensor data messages
- Batch processing
- GPS coordinates
- Device configuration
- Status reporting
- Embedded systems integration

## Resources

- **Official Documentation**: https://protobuf.dev/
- **GitHub Repository**: https://github.com/protocolbuffers/protobuf
- **C++ Tutorial**: https://protobuf.dev/getting-started/cpptutorial/
- **Language Guide**: https://protobuf.dev/programming-guides/proto3/
- **Style Guide**: https://protobuf.dev/programming-guides/style/
- **gRPC** (RPC framework using protobuf): https://grpc.io/

## Related Projects

- **gRPC** - High-performance RPC framework
- **Protobuf-c** - C implementation for embedded systems
- **nanopb** - Tiny protobuf for microcontrollers
- **protobuf-net** - .NET implementation
- **swift-protobuf** - Swift implementation

## Alternatives to Consider

- **FlatBuffers** - If you need zero-copy deserialization
- **Cap'n Proto** - Similar to FlatBuffers, different design
- **MessagePack** - If you don't need schemas
- **CBOR** - Compact binary JSON
- **Thrift** - Similar to protobuf, from Apache
