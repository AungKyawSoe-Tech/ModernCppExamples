// ===================================================================
// NLOHMANN JSON LIBRARY EXAMPLE
// ===================================================================
// This example demonstrates the nlohmann/json library - a modern C++
// JSON library that provides an intuitive API similar to Python/JavaScript.
//
// TOPICS COVERED:
// 1. Basic JSON parsing and serialization
// 2. Working with JSON objects and arrays
// 3. Type conversions and safety
// 4. Custom struct serialization
// 5. JSON Merge Patch (RFC 7386)
// 6. JSON Pointer (RFC 6901)
// 7. CBOR/MessagePack/BSON/UBJSON support
// 8. Performance and embedded systems considerations
//
// WHAT IS NLOHMANN JSON?
// - Header-only C++ JSON library
// - Intuitive API similar to Python dictionaries
// - No external dependencies
// - Supports C++11 and later
// - MIT licensed, widely adopted
//
// WHY USE NLOHMANN JSON?
// ✅ Easy to use: json["key"] = value
// ✅ Header-only: Just #include <nlohmann/json.hpp>
// ✅ STL-like: Integrates seamlessly with C++ containers
// ✅ Type-safe: Strong typing with automatic conversions
// ✅ Standards-compliant: RFC 7159, RFC 6901, RFC 7386
// ✅ Modern C++: Uses C++11/14/17/20 features
//
// INSTALLATION:
// - Ubuntu: sudo apt-get install nlohmann-json3-dev
// - vcpkg: vcpkg install nlohmann-json
// - CMake: find_package(nlohmann_json REQUIRED)
// - Single header: Download json.hpp from GitHub
//
// ===================================================================

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

// For convenience
using json = nlohmann::json;

// ===================================================================
// EXAMPLE 1: BASIC JSON CREATION AND PARSING
// ===================================================================

void example_basic_json() {
    std::cout << "=== Example 1: Basic JSON Creation and Parsing ===\n";
    
    // Create JSON from initializer list (most readable)
    json j = {
        {"name", "Alice"},
        {"age", 30},
        {"city", "San Francisco"},
        {"active", true}
    };
    
    std::cout << "Created JSON:\n" << j.dump(2) << "\n\n";
    
    // Access values (Python-like syntax)
    std::cout << "Name: " << j["name"] << "\n";
    std::cout << "Age: " << j["age"] << "\n";
    
    // Type-safe access with .get<T>()
    std::string name = j["name"].get<std::string>();
    int age = j["age"].get<int>();
    std::cout << "Type-safe: " << name << " is " << age << " years old\n";
    
    // Parse from string
    std::string json_str = R"({"device": "sensor_001", "temperature": 23.5})";
    json parsed = json::parse(json_str);
    std::cout << "\nParsed JSON:\n" << parsed.dump(2) << "\n";
    
    // Check if key exists
    if (parsed.contains("temperature")) {
        std::cout << "Temperature: " << parsed["temperature"] << "°C\n";
    }
    
    std::cout << "\n✅ Easy syntax like Python/JavaScript\n";
    std::cout << "✅ Automatic type conversions\n";
    std::cout << "✅ contains() for safe key checking\n\n";
}

// ===================================================================
// EXAMPLE 2: WORKING WITH ARRAYS
// ===================================================================

void example_arrays() {
    std::cout << "=== Example 2: JSON Arrays ===\n";
    
    // Create array
    json arr = json::array();
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);
    
    std::cout << "Array: " << arr.dump() << "\n";
    
    // Create from vector
    std::vector<int> vec = {1, 2, 3, 4, 5};
    json j_vec = vec;
    std::cout << "From vector: " << j_vec.dump() << "\n";
    
    // Convert back to vector
    std::vector<int> vec2 = j_vec.get<std::vector<int>>();
    std::cout << "Back to vector: size = " << vec2.size() << "\n";
    
    // Array of objects
    json sensors = json::array();
    for (int i = 0; i < 5; ++i) {
        sensors.push_back({
            {"id", "sensor_" + std::to_string(i)},
            {"value", 20.0 + i * 0.5},
            {"online", i % 2 == 0}
        });
    }
    
    std::cout << "\nSensor array:\n" << sensors.dump(2) << "\n";
    
    // Iterate over array
    std::cout << "\nOnline sensors:\n";
    for (const auto& sensor : sensors) {
        if (sensor["online"].get<bool>()) {
            std::cout << "  " << sensor["id"] << ": " << sensor["value"] << "\n";
        }
    }
    
    std::cout << "\n✅ STL container integration\n";
    std::cout << "✅ Range-based for loops\n";
    std::cout << "✅ Array manipulation like JavaScript\n\n";
}

// ===================================================================
// EXAMPLE 3: CUSTOM STRUCT SERIALIZATION
// ===================================================================

struct SensorReading {
    std::string device_id;
    double temperature;
    double humidity;
    bool online;
};

// Define JSON serialization (method 1: macro)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SensorReading, device_id, temperature, humidity, online)

// Alternative custom struct with intrusive serialization
struct GpsCoordinate {
    double latitude;
    double longitude;
    double altitude;
    
    // Method 2: Member functions (more control)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GpsCoordinate, latitude, longitude, altitude)
};

void example_custom_types() {
    std::cout << "=== Example 3: Custom Struct Serialization ===\n";
    
    // Create struct
    SensorReading reading{
        "sensor_001",
        23.5,
        65.3,
        true
    };
    
    // Automatic conversion to JSON
    json j = reading;
    std::cout << "Struct to JSON:\n" << j.dump(2) << "\n";
    
    // Automatic conversion from JSON
    json j2 = R"({
        "device_id": "sensor_002",
        "temperature": 25.0,
        "humidity": 70.5,
        "online": false
    })"_json;  // User-defined literal
    
    SensorReading reading2 = j2.get<SensorReading>();
    std::cout << "\nJSON to struct:\n";
    std::cout << "  Device: " << reading2.device_id << "\n";
    std::cout << "  Temperature: " << reading2.temperature << "°C\n";
    std::cout << "  Online: " << (reading2.online ? "yes" : "no") << "\n";
    
    // Array of structs
    std::vector<SensorReading> readings = {
        {"sensor_001", 20.0, 60.0, true},
        {"sensor_002", 21.5, 62.0, true},
        {"sensor_003", 19.0, 58.0, false}
    };
    
    json j_array = readings;
    std::cout << "\nVector of structs to JSON:\n" << j_array.dump(2) << "\n";
    
    std::cout << "\n✅ Automatic struct ↔ JSON conversion\n";
    std::cout << "✅ NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE macro\n";
    std::cout << "✅ Works with STL containers\n\n";
}

// ===================================================================
// EXAMPLE 4: FILE I/O
// ===================================================================

void example_file_io() {
    std::cout << "=== Example 4: File I/O ===\n";
    
    // Create configuration JSON
    json config = {
        {"version", "1.0.0"},
        {"server", {
            {"host", "localhost"},
            {"port", 8080},
            {"ssl", true}
        }},
        {"sensors", json::array({
            {{"id", "temp_01"}, {"enabled", true}},
            {{"id", "humid_01"}, {"enabled", true}}
        })}
    };
    
    // Write to file
    std::ofstream file("config.json");
    file << std::setw(2) << config << std::endl;
    file.close();
    std::cout << "✓ Written to config.json\n";
    
    // Read from file
    std::ifstream input("config.json");
    json loaded_config;
    input >> loaded_config;
    input.close();
    
    std::cout << "✓ Loaded from config.json:\n";
    std::cout << "  Version: " << loaded_config["version"] << "\n";
    std::cout << "  Server port: " << loaded_config["server"]["port"] << "\n";
    std::cout << "  Sensors: " << loaded_config["sensors"].size() << "\n";
    
    std::cout << "\n✅ Easy file serialization\n";
    std::cout << "✅ Pretty printing with setw()\n";
    std::cout << "✅ Stream operators << and >>\n\n";
}

// ===================================================================
// EXAMPLE 5: JSON POINTER (RFC 6901)
// ===================================================================

void example_json_pointer() {
    std::cout << "=== Example 5: JSON Pointer (RFC 6901) ===\n";
    
    json data = {
        {"user", {
            {"name", "Alice"},
            {"address", {
                {"city", "San Francisco"},
                {"zip", "94102"}
            }},
            {"scores", {85, 92, 78}}
        }}
    };
    
    // Access nested values using JSON Pointer
    std::cout << "Using JSON Pointer:\n";
    std::cout << "  /user/name: " << data["/user/name"_json_pointer] << "\n";
    std::cout << "  /user/address/city: " << data["/user/address/city"_json_pointer] << "\n";
    std::cout << "  /user/scores/0: " << data["/user/scores/0"_json_pointer] << "\n";
    
    // Check if pointer exists
    if (data.contains(json::json_pointer("/user/address/country"))) {
        std::cout << "  Country exists\n";
    } else {
        std::cout << "  Country does not exist\n";
    }
    
    // Modify using pointer
    data["/user/address/country"_json_pointer] = "USA";
    std::cout << "\n✓ Added country: " << data["/user/address/country"_json_pointer] << "\n";
    
    std::cout << "\n✅ Navigate nested structures easily\n";
    std::cout << "✅ RFC 6901 compliant\n";
    std::cout << "✅ Safe access with contains()\n\n";
}

// ===================================================================
// EXAMPLE 6: JSON MERGE PATCH (RFC 7386)
// ===================================================================

void example_merge_patch() {
    std::cout << "=== Example 6: JSON Merge Patch (RFC 7386) ===\n";
    
    json original = {
        {"name", "Alice"},
        {"age", 30},
        {"city", "San Francisco"},
        {"active", true}
    };
    
    std::cout << "Original:\n" << original.dump(2) << "\n";
    
    // Merge patch: update existing fields, add new fields
    json patch = {
        {"age", 31},           // Update
        {"city", nullptr},     // Delete
        {"country", "USA"}     // Add
    };
    
    original.merge_patch(patch);
    
    std::cout << "\nAfter merge patch:\n" << original.dump(2) << "\n";
    
    std::cout << "\n✅ Update multiple fields at once\n";
    std::cout << "✅ null value deletes field\n";
    std::cout << "✅ RFC 7386 compliant\n\n";
}

// ===================================================================
// EXAMPLE 7: BINARY FORMATS (CBOR, MessagePack, BSON)
// ===================================================================

void example_binary_formats() {
    std::cout << "=== Example 7: Binary Formats ===\n";
    
    json data = {
        {"device", "sensor_001"},
        {"temperature", 23.5},
        {"humidity", 65.3},
        {"readings", {20, 21, 22, 23, 24}}
    };
    
    // JSON string
    std::string json_str = data.dump();
    std::cout << "JSON size: " << json_str.size() << " bytes\n";
    
    // CBOR (Concise Binary Object Representation)
    std::vector<uint8_t> cbor = json::to_cbor(data);
    std::cout << "CBOR size: " << cbor.size() << " bytes";
    std::cout << " (" << std::fixed << std::setprecision(1) 
              << (1.0 - (double)cbor.size() / json_str.size()) * 100 << "% smaller)\n";
    
    // MessagePack
    std::vector<uint8_t> msgpack = json::to_msgpack(data);
    std::cout << "MessagePack size: " << msgpack.size() << " bytes";
    std::cout << " (" << (1.0 - (double)msgpack.size() / json_str.size()) * 100 << "% smaller)\n";
    
    // BSON
    std::vector<uint8_t> bson = json::to_bson(data);
    std::cout << "BSON size: " << bson.size() << " bytes";
    std::cout << " (" << (1.0 - (double)bson.size() / json_str.size()) * 100 << "% smaller)\n";
    
    // UBJSON (Universal Binary JSON)
    std::vector<uint8_t> ubjson = json::to_ubjson(data);
    std::cout << "UBJSON size: " << ubjson.size() << " bytes\n";
    
    // Deserialize CBOR
    json restored = json::from_cbor(cbor);
    std::cout << "\n✓ Restored from CBOR:\n" << restored.dump(2) << "\n";
    
    std::cout << "\n✅ Multiple binary format support\n";
    std::cout << "✅ 30-50% smaller than JSON text\n";
    std::cout << "✅ Faster parsing than text JSON\n\n";
}

// ===================================================================
// EXAMPLE 8: ERROR HANDLING
// ===================================================================

void example_error_handling() {
    std::cout << "=== Example 8: Error Handling ===\n";
    
    // Safe parsing with exception handling
    std::string invalid_json = R"({"name": "Alice", "age": })";
    
    try {
        json j = json::parse(invalid_json);
    } catch (const json::parse_error& e) {
        std::cout << "Parse error caught:\n";
        std::cout << "  Message: " << e.what() << "\n";
        std::cout << "  Exception ID: " << e.id << "\n";
        std::cout << "  Byte position: " << e.byte << "\n";
    }
    
    // Safe access with value()
    json data = {{"name", "Alice"}, {"age", 30}};
    
    std::string name = data.value("name", "Unknown");
    std::string country = data.value("country", "Unknown");  // Default if missing
    
    std::cout << "\n✓ Safe value() with default:\n";
    std::cout << "  Name: " << name << "\n";
    std::cout << "  Country: " << country << " (default)\n";
    
    // Type checking
    if (data["age"].is_number_integer()) {
        int age = data["age"];
        std::cout << "  Age is integer: " << age << "\n";
    }
    
    std::cout << "\n✅ Exception-based error handling\n";
    std::cout << "✅ value() method with defaults\n";
    std::cout << "✅ Type checking with is_xxx()\n\n";
}

// ===================================================================
// EXAMPLE 9: ADVANCED STL CONTAINER CONVERSIONS
// ===================================================================

void example_stl_conversions() {
    std::cout << "=== Example 9: Advanced STL Container Conversions ===\n";
    
    // Map conversions
    std::map<std::string, int> scores = {
        {"Alice", 95},
        {"Bob", 87},
        {"Charlie", 92}
    };
    
    json j_map = scores;
    std::cout << "Map to JSON:\n" << j_map.dump(2) << "\n";
    
    auto restored_map = j_map.get<std::map<std::string, int>>();
    std::cout << "\n✓ Restored map: " << restored_map.size() << " entries\n";
    
    // Nested containers
    std::vector<std::map<std::string, std::vector<int>>> complex = {
        {{"data", {1, 2, 3}}, {"scores", {90, 85, 88}}},
        {{"data", {4, 5, 6}}, {"scores", {92, 87, 91}}}
    };
    
    json j_complex = complex;
    std::cout << "\nNested containers:\n" << j_complex.dump(2) << "\n";
    
    // Set conversions
    std::set<std::string> tags = {"cpp", "json", "modern", "c++17"};
    json j_set = tags;
    std::cout << "\nSet to JSON array: " << j_set.dump() << "\n";
    
    // Tuple conversions
    std::tuple<std::string, int, double> data = {"sensor_001", 42, 23.5};
    json j_tuple = data;
    std::cout << "Tuple to JSON array: " << j_tuple.dump() << "\n";
    
    std::cout << "\n✅ Automatic STL container serialization\n";
    std::cout << "✅ Works with nested containers\n";
    std::cout << "✅ Two-way conversion\n\n";
}

// ===================================================================
// EXAMPLE 10: SAX PARSING FOR LARGE FILES
// ===================================================================

void example_sax_parsing() {
    std::cout << "=== Example 10: SAX Parsing for Large Files ===\n";
    
    // SAX (Simple API for XML) style parsing - event-driven
    // Useful for large JSON files to avoid loading entire document
    
    struct MySaxHandler {
        bool null() {
            std::cout << "  Event: null\n";
            return true;
        }
        
        bool boolean(bool val) {
            std::cout << "  Event: boolean = " << std::boolalpha << val << "\n";
            return true;
        }
        
        bool number_integer(json::number_integer_t val) {
            std::cout << "  Event: integer = " << val << "\n";
            return true;
        }
        
        bool number_float(json::number_float_t val, const std::string&) {
            std::cout << "  Event: float = " << val << "\n";
            return true;
        }
        
        bool string(std::string& val) {
            std::cout << "  Event: string = \"" << val << "\"\n";
            return true;
        }
        
        bool start_object(std::size_t) {
            std::cout << "  Event: start_object\n";
            return true;
        }
        
        bool end_object() {
            std::cout << "  Event: end_object\n";
            return true;
        }
        
        bool start_array(std::size_t) {
            std::cout << "  Event: start_array\n";
            return true;
        }
        
        bool end_array() {
            std::cout << "  Event: end_array\n";
            return true;
        }
        
        bool key(std::string& val) {
            std::cout << "  Event: key = \"" << val << "\"\n";
            return true;
        }
        
        bool parse_error(std::size_t, const std::string&, const json::exception& ex) {
            std::cout << "  Parse error: " << ex.what() << "\n";
            return false;
        }
    };
    
    std::cout << "\nParsing JSON with SAX events:\n";
    std::string json_str = R"({"name":"Alice","age":30,"scores":[95,87,92]})";
    
    MySaxHandler handler;
    bool result = json::sax_parse(json_str, &handler);
    
    std::cout << "\n✓ SAX parsing " << (result ? "succeeded" : "failed") << "\n";
    
    std::cout << "\n✅ Memory-efficient for large files\n";
    std::cout << "✅ Event-driven processing\n";
    std::cout << "✅ Can stop parsing early\n";
    std::cout << "✅ Useful for streaming data\n\n";
}

// ===================================================================
// EXAMPLE 11: JSON SCHEMA VALIDATION
// ===================================================================

void example_json_schema() {
    std::cout << "=== Example 11: JSON Schema-like Validation ===\n";
    
    // Note: nlohmann::json doesn't have built-in schema validation
    // But we can implement basic validation checks
    
    json schema = {
        {"type", "object"},
        {"required", {"name", "age", "email"}},
        {"properties", {
            {"name", {{"type", "string"}}},
            {"age", {{"type", "number"}, {"minimum", 0}, {"maximum", 150}}},
            {"email", {{"type", "string"}}}
        }}
    };
    
    auto validate = [](const json& data, const json& schema) -> bool {
        // Simple validation example
        if (schema["type"] == "object") {
            // Check required fields
            for (const auto& field : schema["required"]) {
                if (!data.contains(field.get<std::string>())) {
                    std::cout << "  ❌ Missing required field: " << field << "\n";
                    return false;
                }
            }
            
            // Check types
            for (const auto& [key, prop_schema] : schema["properties"].items()) {
                if (data.contains(key)) {
                    std::string expected_type = prop_schema["type"];
                    const auto& value = data[key];
                    
                    if (expected_type == "string" && !value.is_string()) {
                        std::cout << "  ❌ Field '" << key << "' should be string\n";
                        return false;
                    }
                    if (expected_type == "number" && !value.is_number()) {
                        std::cout << "  ❌ Field '" << key << "' should be number\n";
                        return false;
                    }
                    
                    // Check number constraints
                    if (value.is_number() && prop_schema.contains("minimum")) {
                        if (value.get<double>() < prop_schema["minimum"].get<double>()) {
                            std::cout << "  ❌ Field '" << key << "' below minimum\n";
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    };
    
    json valid_data = {
        {"name", "Alice"},
        {"age", 30},
        {"email", "alice@example.com"}
    };
    
    json invalid_data = {
        {"name", "Bob"},
        {"age", "thirty"}  // Wrong type
    };
    
    std::cout << "\nValidating correct data:\n";
    if (validate(valid_data, schema)) {
        std::cout << "  ✅ Validation passed\n";
    }
    
    std::cout << "\nValidating incorrect data:\n";
    validate(invalid_data, schema);
    
    std::cout << "\n💡 For full JSON Schema support, use:\n";
    std::cout << "   • nlohmann/json-schema-validator library\n";
    std::cout << "   • valijson library\n\n";
}

// ===================================================================
// EXAMPLE 12: CUSTOM ALLOCATORS FOR EMBEDDED SYSTEMS
// ===================================================================

void example_custom_allocator() {
    std::cout << "=== Example 12: Custom Allocators for Embedded ===\n";
    
    // nlohmann::json supports custom allocators via template parameter
    // Useful for embedded systems with custom memory management
    
    std::cout << "\nDefault allocator usage:\n";
    json j = {{"sensor", "temp_01"}, {"value", 23.5}};
    std::cout << "  Created JSON: " << j.dump() << "\n";
    
    // For embedded systems, you can use:
    // 1. Compile with -DJSON_NOEXCEPTION (no exceptions)
    // 2. Use custom allocator with basic_json template
    // 3. Disable certain features to reduce code size
    
    std::cout << "\n💡 Embedded system optimizations:\n";
    std::cout << "   1. Compile flags:\n";
    std::cout << "      • -DJSON_NOEXCEPTION - Disable exceptions\n";
    std::cout << "      • -DJSON_NO_IO - Disable file I/O\n";
    std::cout << "      • -DJSON_DIAGNOSTICS=0 - Smaller binary\n";
    std::cout << "   2. Use binary formats (CBOR/MessagePack)\n";
    std::cout << "   3. Use SAX parsing for large data\n";
    std::cout << "   4. Pre-allocate with reserve()\n";
    std::cout << "   5. Consider alternatives like ArduinoJson\n\n";
    
    std::cout << "Memory footprint:\n";
    std::cout << "  • Header-only: ~20KB code size\n";
    std::cout << "  • Runtime: depends on JSON size\n";
    std::cout << "  • Stack usage: minimal (recursion depth)\n\n";
}

// ===================================================================
// EXAMPLE 13: PERFORMANCE TIPS
// ===================================================================

void example_performance() {
    std::cout << "=== Example 13: Performance Tips ===\n";
    
    // Tip 1: Use references to avoid copies
    json large_data = {
        {"sensors", json::array()}
    };
    
    for (int i = 0; i < 100; ++i) {
        large_data["sensors"].push_back({
            {"id", i},
            {"value", 20.0 + i * 0.1}
        });
    }
    
    // BAD: Creates copies
    // for (auto sensor : large_data["sensors"]) { ... }
    
    // GOOD: Use const reference
    int count = 0;
    for (const auto& sensor : large_data["sensors"]) {
        if (sensor["value"].get<double>() > 25.0) {
            ++count;
        }
    }
    std::cout << "Found " << count << " sensors > 25.0 (using const ref)\n";
    
    // Tip 2: Reserve array capacity
    json arr = json::array();
    // arr.get_ref<json::array_t&>().reserve(1000);  // Pre-allocate
    
    // Tip 3: Use binary formats for network/storage
    std::vector<uint8_t> cbor = json::to_cbor(large_data);
    std::cout << "Binary format: " << cbor.size() << " bytes\n";
    
    // Tip 4: Disable exceptions for embedded systems
    // Compile with -DJSON_NOEXCEPTION
    
    std::cout << "\nPerformance tips:\n";
    std::cout << "  ✅ Use const auto& in loops\n";
    std::cout << "  ✅ Reserve array capacity\n";
    std::cout << "  ✅ Use binary formats for I/O\n";
    std::cout << "  ✅ Compile with -DJSON_NOEXCEPTION for embedded\n\n";
}

// ===================================================================
// EXAMPLE 14: COMPARISON WITH ALTERNATIVES
// ===================================================================

void comparison_with_alternatives() {
    std::cout << "=== Comparison with Alternatives ===\n\n";
    
    std::cout << "RapidJSON:\n";
    std::cout << "  ✅ Faster parsing (2-3x)\n";
    std::cout << "  ✅ Lower memory usage\n";
    std::cout << "  ❌ More complex API\n";
    std::cout << "  ❌ Manual memory management\n\n";
    
    std::cout << "jsoncpp:\n";
    std::cout << "  ✅ Mature and stable\n";
    std::cout << "  ❌ Older API design\n";
    std::cout << "  ❌ Slower than nlohmann\n";
    std::cout << "  ❌ Requires compilation\n\n";
    
    std::cout << "simdjson:\n";
    std::cout << "  ✅ Extremely fast (SIMD optimized)\n";
    std::cout << "  ✅ Read-only parsing\n";
    std::cout << "  ❌ No JSON creation\n";
    std::cout << "  ❌ Different API paradigm\n\n";
    
    std::cout << "Protocol Buffers:\n";
    std::cout << "  ✅ Smaller binary format\n";
    std::cout << "  ✅ Faster parsing\n";
    std::cout << "  ❌ Requires schema\n";
    std::cout << "  ❌ Code generation needed\n\n";
    
    std::cout << "When to use nlohmann/json:\n";
    std::cout << "  ✅ Need easy-to-use API\n";
    std::cout << "  ✅ Header-only library preferred\n";
    std::cout << "  ✅ Configuration files\n";
    std::cout << "  ✅ REST API communication\n";
    std::cout << "  ✅ Rapid prototyping\n";
    std::cout << "  ⚠️ Not for extreme performance (use RapidJSON/simdjson)\n\n";
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "=========================================================\n";
    std::cout << "NLOHMANN JSON LIBRARY EXAMPLES\n";
    std::cout << "=========================================================\n\n";
    
    example_basic_json();
    example_arrays();
    example_custom_types();
    example_file_io();
    example_json_pointer();
    example_merge_patch();
    example_binary_formats();
    example_error_handling();
    example_stl_conversions();
    example_sax_parsing();
    example_json_schema();
    example_custom_allocator();
    example_performance();
    comparison_with_alternatives();
    
    std::cout << "=========================================================\n";
    std::cout << "INSTALLATION:\n";
    std::cout << "=========================================================\n\n";
    
    std::cout << "Ubuntu/Debian:\n";
    std::cout << "  sudo apt-get install nlohmann-json3-dev\n\n";
    
    std::cout << "vcpkg:\n";
    std::cout << "  vcpkg install nlohmann-json\n\n";
    
    std::cout << "CMake:\n";
    std::cout << "  find_package(nlohmann_json REQUIRED)\n";
    std::cout << "  target_link_libraries(YourTarget nlohmann_json::nlohmann_json)\n\n";
    
    std::cout << "Single header:\n";
    std::cout << "  Download: https://github.com/nlohmann/json/releases\n";
    std::cout << "  Just #include \"json.hpp\"\n\n";
    
    std::cout << "=========================================================\n";
    std::cout << "RESOURCES:\n";
    std::cout << "=========================================================\n";
    std::cout << "GitHub: https://github.com/nlohmann/json\n";
    std::cout << "Documentation: https://json.nlohmann.me/\n";
    std::cout << "API Reference: https://json.nlohmann.me/api/basic_json/\n\n";
    
    return 0;
}
