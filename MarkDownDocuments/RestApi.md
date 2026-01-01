# REST API Calls in C++

This document provides a comprehensive guide to making REST API calls in modern C++ using `libcurl` and `nlohmann::json`.

## Overview

The [RestApiExample.cpp](../src/RestApiExample.cpp) demonstrates how to:
- Make HTTP requests (GET, POST, PUT, DELETE)
- Handle JSON request/response bodies
- Manage HTTP headers and authentication
- Implement proper error handling
- Use RAII for resource management

## Dependencies

### Required Libraries

1. **libcurl** - HTTP client library
   ```bash
   # Ubuntu/Debian/WSL
   sudo apt-get update
   sudo apt-get install libcurl4-openssl-dev
   
   # Windows (vcpkg)
   vcpkg install curl
   
   # macOS
   brew install curl
   ```

2. **nlohmann_json** - JSON library
   ```bash
   # Ubuntu/Debian/WSL
   sudo apt-get install nlohmann-json3-dev
   
   # Windows (vcpkg)
   vcpkg install nlohmann-json
   
   # macOS
   brew install nlohmann-json
   ```

## Building

### On WSL/Linux:
```bash
# Navigate to project directory
cd /mnt/c/CoPilot_Cli/ModernCppExamples

# Configure with CMake
cmake -B build

# Build
cmake --build build

# Run the example
./build/bin/RestApiExample
```

### On Windows:
```powershell
# Configure and build
cmake -B build
cmake --build build

# Run the example
.\build\bin\RestApiExample.exe
```

## Key Features

### 1. RAII Resource Management

The example uses RAII wrappers to ensure proper cleanup of CURL resources:

```cpp
class CurlHandle {
    CURL* handle_;
public:
    CurlHandle() : handle_(curl_easy_init()) { }
    ~CurlHandle() { if (handle_) curl_easy_cleanup(handle_); }
    // ... move semantics, no copying
};
```

### 2. Type-Safe REST Client

The `RestClient` class provides a clean, modern C++ interface:

```cpp
RestClient client("https://api.example.com");

// GET request
auto response = client.get("/users/123");

// POST with JSON
json data = {{"name", "John"}, {"age", 30}};
auto response = client.post("/users", data);

// PUT request
auto response = client.put("/users/123", updated_data);

// DELETE request
auto response = client.del("/users/123");
```

### 3. Structured Response Handling

```cpp
struct HttpResponse {
    long status_code;
    std::string body;
    std::map<std::string, std::string> headers;
    
    bool is_success() const;
    json json() const;
};
```

### 4. Authentication Support

```cpp
// Bearer token authentication
client.set_bearer_token("your_token_here");

// Custom headers
client.set_default_header("X-API-Key", "your_api_key");
```

## Examples Included

### Example 1: Simple GET Request
Fetches data from a REST API endpoint.

### Example 2: POST with JSON Payload
Creates a new resource with JSON data.

### Example 3: PUT Request
Updates an existing resource.

### Example 4: DELETE Request
Removes a resource.

### Example 5: Query Parameters
Demonstrates URL encoding and query parameter handling.

### Example 6: GitHub API Integration
Shows real-world API usage with custom headers.

### Example 7: Error Handling
Demonstrates proper error handling for network failures and HTTP errors.

## Best Practices

### Security
- ✅ Always use HTTPS for sensitive data
- ✅ Validate SSL certificates (enabled by default in libcurl)
- ✅ Never hardcode API keys or tokens
- ✅ Store credentials in environment variables
- ✅ Use Bearer tokens for authentication

### Error Handling
- ✅ Check `CURLcode` return values
- ✅ Validate HTTP status codes
- ✅ Use exceptions for unrecoverable errors
- ✅ Parse JSON safely with try-catch blocks

### Resource Management
- ✅ Use RAII wrappers for all C resources
- ✅ Ensure cleanup happens automatically
- ✅ Make classes move-only where appropriate
- ✅ Initialize libcurl globally once per program

### Performance
- ✅ Reuse `RestClient` instances
- ✅ Set appropriate timeouts
- ✅ Consider HTTP/2 for better performance
- ✅ Use connection pooling for many requests

### Modern C++ Features Used
- ✅ RAII and smart pointers
- ✅ Move semantics
- ✅ Structured bindings (C++17)
- ✅ `std::optional` for nullable values
- ✅ `auto` for type deduction
- ✅ Range-based for loops

## Testing APIs

The example uses public test APIs that don't require authentication:

1. **JSONPlaceholder** - https://jsonplaceholder.typicode.com
   - Free fake REST API for testing
   - Supports all HTTP methods
   - Returns realistic JSON responses

2. **GitHub API** - https://api.github.com
   - Public endpoints don't require authentication
   - Rate limited: 60 requests/hour for unauthenticated requests

## Alternative Libraries

While this example uses libcurl, consider these alternatives:

### cpp-httplib
- **Pros**: Header-only, simpler API, modern C++
- **Cons**: Less mature, fewer features than libcurl
- **Use when**: You want simplicity and don't need advanced features

### Boost.Beast
- **Pros**: Part of Boost, excellent async support, HTTP/1.1 and WebSocket
- **Cons**: Requires Boost, more complex API
- **Use when**: You need async operations or WebSocket support

### cpprestsdk (C++ REST SDK)
- **Pros**: Microsoft-backed, good async support, cross-platform
- **Cons**: Heavy dependency, uses PPL/futures
- **Use when**: Building Microsoft-centric applications

### Qt Network
- **Pros**: Integrated with Qt, excellent cross-platform support
- **Cons**: Requires Qt framework
- **Use when**: Already using Qt in your project

## Common Issues and Solutions

### Issue: SSL Certificate Verification Failed
**Solution**: Ensure CA certificates are installed:
```bash
# Ubuntu/Debian
sudo apt-get install ca-certificates

# Update certificates
sudo update-ca-certificates
```

### Issue: Timeout Errors
**Solution**: Increase timeout or check network connectivity:
```cpp
curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);  // 60 seconds
```

### Issue: JSON Parsing Errors
**Solution**: Validate response content type and handle exceptions:
```cpp
try {
    auto data = json::parse(response.body);
} catch (const json::parse_error& e) {
    std::cerr << "JSON parse error: " << e.what() << std::endl;
}
```

### Issue: CURL not found by CMake
**Solution**: Set CURL path manually:
```bash
cmake -B build -DCURL_ROOT=/path/to/curl
```

## Production Considerations

### Threading
- libcurl handles are NOT thread-safe
- Create separate handles per thread
- Use `curl_global_init()` once at startup
- Consider using `CURLSH` for connection sharing

### Memory Management
- The example uses RAII to manage memory automatically
- Avoid manual `new`/`delete` for CURL structures
- Let smart pointers and RAII wrappers handle cleanup

### Logging
Add logging for debugging:
```cpp
// Enable verbose output
curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
```

### Connection Pooling
For high-performance applications:
```cpp
// Reuse connections
curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);
```

## Further Reading

- [libcurl documentation](https://curl.se/libcurl/c/)
- [nlohmann::json documentation](https://json.nlohmann.me/)
- [REST API Best Practices](https://restfulapi.net/)
- [HTTP Status Codes](https://httpstatuses.com/)
- [cURL Tutorial](https://curl.se/docs/tutorial.html)

## License

This example is provided as part of the ModernCppExamples project for educational purposes.
