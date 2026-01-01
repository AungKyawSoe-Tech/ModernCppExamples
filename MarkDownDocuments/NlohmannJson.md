# Nlohmann JSON Library for Modern C++

## What is Nlohmann JSON?

Nlohmann JSON (also known as JSON for Modern C++) is a header-only C++11 JSON library with an intuitive API similar to Python dictionaries or JavaScript objects. It's one of the most popular JSON libraries in the C++ ecosystem.

## Why Use Nlohmann JSON?

### Advantages
- ✅ **Intuitive API**: `json["key"] = value` - as simple as Python
- ✅ **Header-only**: No compilation required, just include the header
- ✅ **STL integration**: Works seamlessly with vectors, maps, etc.
- ✅ **Type-safe**: Automatic type conversions with strong typing
- ✅ **Modern C++**: Uses C++11/14/17/20 features
- ✅ **Feature-rich**: JSON Pointer, Merge Patch, binary formats
- ✅ **Well-tested**: 100% code coverage, continuous integration

### Use Cases
- **Configuration files**: Easy parsing and generation
- **REST APIs**: JSON serialization for web services
- **Data interchange**: Human-readable data format
- **Logging**: Structured log messages
- **Testing**: Easy test data creation

## Installation

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install nlohmann-json3-dev
```

### vcpkg
```bash
vcpkg install nlohmann-json
```

### CMake
```cmake
find_package(nlohmann_json REQUIRED)
target_link_libraries(YourTarget PRIVATE nlohmann_json::nlohmann_json)
```

### Single Header
Download `json.hpp` from [GitHub releases](https://github.com/nlohmann/json/releases) and include it:
```cpp
#include "json.hpp"
```

## Quick Start

```cpp
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

int main() {
    // Create JSON object
    json j = {
        {"name", "Alice"},
        {"age", 30},
        {"city", "San Francisco"}
    };
    
    // Pretty print
    std::cout << j.dump(2) << "\n";
    
    // Access values
    std::string name = j["name"];
    int age = j["age"];
    
    // Parse from string
    json parsed = json::parse(R"({"status": "ok"})");
    
    return 0;
}
```

## Basic Features

### Creating JSON

```cpp
// From initializer list
json j = {
    {"pi", 3.141},
    {"happy", true},
    {"name", "Niels"},
    {"nothing", nullptr},
    {"answer", {{"everything", 42}}},
    {"list", {1, 0, 2}},
    {"object", {{"currency", "USD"}, {"value", 42.99}}}
};

// Empty object/array
json empty_obj = json::object();
json empty_arr = json::array();

// From STL containers
std::vector<int> vec = {1, 2, 3};
json j_vec = vec;

std::map<std::string, int> map = {{"one", 1}, {"two", 2}};
json j_map = map;
```

### Accessing Values

```cpp
// Dictionary-style access
std::string name = j["name"];
int age = j["age"];

// Type-safe access with get<T>()
std::string name = j["name"].get<std::string>();
int age = j.value("age", 0);  // Default if missing

// Check if key exists
if (j.contains("name")) {
    // ...
}

// Array access
json arr = {1, 2, 3};
int first = arr[0];
int size = arr.size();
```

### Type Checking

```cpp
if (j["age"].is_number()) { /* ... */ }
if (j["name"].is_string()) { /* ... */ }
if (j["active"].is_boolean()) { /* ... */ }
if (j["data"].is_object()) { /* ... */ }
if (j["items"].is_array()) { /* ... */ }
if (j["missing"].is_null()) { /* ... */ }
```

## Advanced Features

### Custom Struct Serialization

```cpp
struct Person {
    std::string name;
    int age;
    std::string city;
};

// Method 1: Non-intrusive (outside class)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Person, name, age, city)

// Method 2: Intrusive (inside class)
struct Employee {
    std::string name;
    int id;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Employee, name, id)
};

// Usage
Person p{"Alice", 30, "SF"};
json j = p;  // Automatic conversion

Person p2 = j.get<Person>();  // Back to struct
```

### JSON Pointer (RFC 6901)

Navigate nested structures:

```cpp
json data = {
    {"user", {
        {"name", "Alice"},
        {"address", {
            {"city", "SF"},
            {"zip", "94102"}
        }}
    }}
};

// Access with JSON Pointer
std::string city = data["/user/address/city"_json_pointer];

// Check if path exists
if (data.contains("/user/address/country"_json_pointer)) {
    // ...
}
```

### JSON Merge Patch (RFC 7386)

Update JSON documents:

```cpp
json original = {
    {"name", "Alice"},
    {"age", 30},
    {"city", "SF"}
};

json patch = {
    {"age", 31},        // Update
    {"city", nullptr},  // Delete
    {"country", "USA"}  // Add
};

original.merge_patch(patch);
// Result: {"name": "Alice", "age": 31, "country": "USA"}
```

### Binary Formats

Nlohmann JSON supports multiple binary formats:

```cpp
json data = {{"temp", 23.5}, {"humidity", 65}};

// CBOR (Concise Binary Object Representation)
std::vector<uint8_t> cbor = json::to_cbor(data);
json restored = json::from_cbor(cbor);

// MessagePack
std::vector<uint8_t> msgpack = json::to_msgpack(data);

// BSON (MongoDB format)
std::vector<uint8_t> bson = json::to_bson(data);

// UBJSON (Universal Binary JSON)
std::vector<uint8_t> ubjson = json::to_ubjson(data);
```

Binary formats are typically 30-50% smaller than JSON text.

## File I/O

### Writing to File

```cpp
json config = {
    {"version", "1.0.0"},
    {"server", {
        {"host", "localhost"},
        {"port", 8080}
    }}
};

// Write to file
std::ofstream file("config.json");
file << std::setw(2) << config << std::endl;  // Pretty print with indent=2
```

### Reading from File

```cpp
std::ifstream file("config.json");
json config;
file >> config;

std::string host = config["server"]["host"];
int port = config["server"]["port"];
```

## Error Handling

### Exception Handling

```cpp
try {
    json j = json::parse(invalid_string);
} catch (const json::parse_error& e) {
    std::cerr << "Parse error: " << e.what() << "\n";
    std::cerr << "Byte position: " << e.byte << "\n";
}

try {
    int value = j.at("key");  // Throws if key missing
} catch (const json::out_of_range& e) {
    std::cerr << "Key not found\n";
}

try {
    int value = j.get<int>();  // Throws if wrong type
} catch (const json::type_error& e) {
    std::cerr << "Type mismatch\n";
}
```

### Safe Access with Defaults

```cpp
// Returns default if key doesn't exist
std::string name = j.value("name", "Unknown");
int age = j.value("age", 0);
```

### Disable Exceptions (Embedded Systems)

Compile with `-DJSON_NOEXCEPTION` for embedded systems without exception support.

## Performance Tips

### 1. Use References to Avoid Copies

```cpp
// BAD: Creates copies
for (auto item : json_array) { /* ... */ }

// GOOD: Use const reference
for (const auto& item : json_array) { /* ... */ }
```

### 2. Pre-allocate Arrays

```cpp
json arr = json::array();
arr.get_ref<json::array_t&>().reserve(1000);
```

### 3. Use Binary Formats for I/O

```cpp
// 30-50% smaller than text JSON
std::vector<uint8_t> cbor = json::to_cbor(data);
```

### 4. Move Instead of Copy

```cpp
json large_object = create_large_json();
json container;
container["data"] = std::move(large_object);  // Move, don't copy
```

## Comparison with Alternatives

| Feature | nlohmann/json | RapidJSON | jsoncpp | simdjson | Protocol Buffers |
|---------|---------------|-----------|---------|----------|------------------|
| Ease of use | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| Performance | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Header-only | ✅ | ✅ | ❌ | ✅ | ❌ |
| STL integration | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ |
| Binary formats | ✅ | ❌ | ❌ | ❌ | ✅ |
| Schema required | ❌ | ❌ | ❌ | ❌ | ✅ |

## When to Use

### Use nlohmann/json for:
- ✅ Configuration files
- ✅ REST API communication
- ✅ Rapid prototyping
- ✅ When ease of use matters
- ✅ Header-only projects

### Consider alternatives for:
- ⚠️ Extreme performance requirements → RapidJSON or simdjson
- ⚠️ Very large JSON documents → simdjson (streaming)
- ⚠️ Binary size constraints → Protocol Buffers
- ⚠️ Embedded systems → Consider lite alternatives

## Best Practices

### DO:
✅ Use `const auto&` in loops  
✅ Use `value()` for safe access with defaults  
✅ Pre-allocate arrays when size is known  
✅ Use binary formats for network/storage  
✅ Use `contains()` to check for keys  
✅ Handle parse errors with try-catch  
✅ Use NLOHMANN_DEFINE_TYPE for structs  

### DON'T:
❌ Use `auto` (copies) in loops - use `const auto&`  
❌ Access keys without checking existence in production  
❌ Parse untrusted JSON without error handling  
❌ Use JSON for very large datasets (use databases)  
❌ Serialize binary data directly (use Base64)  

## Real-World Example

See [NlohmannJsonExample.cpp](../src/NlohmannJsonExample.cpp) for a complete example demonstrating:
- Basic JSON creation and parsing
- Working with arrays and objects
- Custom struct serialization
- File I/O
- JSON Pointer and Merge Patch
- Binary format support (CBOR, MessagePack, BSON)
- Error handling
- Performance optimization

## Resources

- **GitHub Repository**: https://github.com/nlohmann/json
- **Documentation**: https://json.nlohmann.me/
- **API Reference**: https://json.nlohmann.me/api/basic_json/
- **Examples**: https://github.com/nlohmann/json#examples
- **Integration**: https://json.nlohmann.me/integration/

## Compiler Support

- GCC ≥ 4.9
- Clang ≥ 3.4
- MSVC ≥ 2015
- AppleClang ≥ 7.0

## License

MIT License - free for commercial and open-source use.
