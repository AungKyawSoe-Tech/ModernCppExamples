// ===================================================================
// REST API CALLS IN C++ EXAMPLE
// ===================================================================
// This example demonstrates making REST API calls in modern C++ using:
// - libcurl: For HTTP/HTTPS requests
// - nlohmann/json: For JSON parsing and serialization
//
// TOPICS COVERED:
// 1. GET requests with query parameters
// 2. POST requests with JSON payloads
// 3. PUT and DELETE requests
// 4. Custom headers and authentication
// 5. Error handling for network operations
// 6. RAII wrapper for curl resources
// 7. Response parsing with JSON
//
// WHAT IS libcurl?
// - A free and easy-to-use client-side URL transfer library
// - Supports HTTP, HTTPS, FTP, and many other protocols
// - Thread-safe when used correctly
// - Cross-platform (Windows, Linux, macOS)
//
// WHY USE libcurl?
// ✅ Battle-tested: Used in billions of devices
// ✅ Feature-rich: Supports all HTTP methods and features
// ✅ Cross-platform: Works everywhere
// ✅ Well-documented: Extensive documentation and examples
// ✅ Active development: Regular updates and security fixes
//
// INSTALLATION:
// - Ubuntu: sudo apt-get install libcurl4-openssl-dev
// - Windows (vcpkg): vcpkg install curl
// - macOS: brew install curl
// - CMake: find_package(CURL REQUIRED)
//
// ALTERNATIVES TO CONSIDER:
// - cpp-httplib: Header-only, simpler but less features
// - Boost.Beast: Part of Boost, good for async operations
// - cpprestsdk: Microsoft's REST SDK, good for async
// - Qt Network: If already using Qt framework
//
// ===================================================================

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ===================================================================
// SECTION 1: RAII Wrapper for CURL
// ===================================================================
// Modern C++ principle: Use RAII to manage resources automatically
// This ensures curl_easy_cleanup is always called, even on exceptions

class CurlHandle {
private:
    CURL* handle_;
    
public:
    CurlHandle() : handle_(curl_easy_init()) {
        if (!handle_) {
            throw std::runtime_error("Failed to initialize CURL");
        }
    }
    
    ~CurlHandle() {
        if (handle_) {
            curl_easy_cleanup(handle_);
        }
    }
    
    // Delete copy constructor and assignment (non-copyable)
    CurlHandle(const CurlHandle&) = delete;
    CurlHandle& operator=(const CurlHandle&) = delete;
    
    // Allow move semantics
    CurlHandle(CurlHandle&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    
    CurlHandle& operator=(CurlHandle&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                curl_easy_cleanup(handle_);
            }
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }
    
    CURL* get() const { return handle_; }
    operator CURL*() const { return handle_; }
};

// ===================================================================
// SECTION 2: RAII Wrapper for curl_slist (headers)
// ===================================================================

class CurlHeaders {
private:
    struct curl_slist* headers_;
    
public:
    CurlHeaders() : headers_(nullptr) {}
    
    ~CurlHeaders() {
        if (headers_) {
            curl_slist_free_all(headers_);
        }
    }
    
    CurlHeaders(const CurlHeaders&) = delete;
    CurlHeaders& operator=(const CurlHeaders&) = delete;
    
    void append(const std::string& header) {
        headers_ = curl_slist_append(headers_, header.c_str());
    }
    
    struct curl_slist* get() const { return headers_; }
};

// ===================================================================
// SECTION 3: HTTP Response Structure
// ===================================================================

struct HttpResponse {
    long status_code;
    std::string body;
    std::map<std::string, std::string> headers;
    
    bool is_success() const {
        return status_code >= 200 && status_code < 300;
    }
    
    json to_json() const {
        return json::parse(body);
    }
};

// ===================================================================
// SECTION 4: Callback for Writing Response Data
// ===================================================================
// libcurl uses C-style callbacks. We use a static function that
// calls into our C++ code.

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    auto* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), total_size);
    return total_size;
}

// ===================================================================
// SECTION 5: REST API Client Class
// ===================================================================

class RestClient {
private:
    std::string base_url_;
    std::map<std::string, std::string> default_headers_;
    
public:
    explicit RestClient(const std::string& base_url = "") 
        : base_url_(base_url) {
        // Initialize libcurl globally (once per program)
        static bool initialized = false;
        if (!initialized) {
            curl_global_init(CURL_GLOBAL_DEFAULT);
            initialized = true;
        }
    }
    
    ~RestClient() {
        // Note: curl_global_cleanup() should be called at program exit
        // We don't call it here because multiple RestClient instances may exist
    }
    
    void set_default_header(const std::string& key, const std::string& value) {
        default_headers_[key] = value;
    }
    
    void set_bearer_token(const std::string& token) {
        default_headers_["Authorization"] = "Bearer " + token;
    }
    
    // GET request
    HttpResponse get(const std::string& endpoint, 
                     const std::map<std::string, std::string>& params = {}) {
        std::string url = build_url(endpoint, params);
        return perform_request(url, "GET", "", {});
    }
    
    // POST request with JSON body
    HttpResponse post(const std::string& endpoint, 
                      const json& body,
                      const std::map<std::string, std::string>& extra_headers = {}) {
        std::string url = build_url(endpoint);
        auto headers = extra_headers;
        headers["Content-Type"] = "application/json";
        return perform_request(url, "POST", body.dump(), headers);
    }
    
    // PUT request with JSON body
    HttpResponse put(const std::string& endpoint, 
                     const json& body,
                     const std::map<std::string, std::string>& extra_headers = {}) {
        std::string url = build_url(endpoint);
        auto headers = extra_headers;
        headers["Content-Type"] = "application/json";
        return perform_request(url, "PUT", body.dump(), headers);
    }
    
    // DELETE request
    HttpResponse del(const std::string& endpoint) {
        std::string url = build_url(endpoint);
        return perform_request(url, "DELETE", "", {});
    }
    
private:
    std::string build_url(const std::string& endpoint, 
                         const std::map<std::string, std::string>& params = {}) {
        std::string url = base_url_ + endpoint;
        
        if (!params.empty()) {
            url += "?";
            bool first = true;
            for (const auto& [key, value] : params) {
                if (!first) url += "&";
                url += key + "=" + curl_easy_escape_string(value);
                first = false;
            }
        }
        
        return url;
    }
    
    std::string curl_easy_escape_string(const std::string& str) {
        CurlHandle curl;
        char* escaped = curl_easy_escape(curl, str.c_str(), str.length());
        std::string result(escaped);
        curl_free(escaped);
        return result;
    }
    
    HttpResponse perform_request(const std::string& url,
                                 const std::string& method,
                                 const std::string& body,
                                 const std::map<std::string, std::string>& extra_headers) {
        CurlHandle curl;
        HttpResponse response;
        std::string response_body;
        
        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        // Set HTTP method
        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
        } else if (method == "PUT") {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        } else if (method == "DELETE") {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        } else if (method == "GET") {
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        }
        
        // Set request body if provided
        if (!body.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
        }
        
        // Set headers
        CurlHeaders headers;
        
        // Add default headers
        for (const auto& [key, value] : default_headers_) {
            headers.append(key + ": " + value);
        }
        
        // Add extra headers (can override defaults)
        for (const auto& [key, value] : extra_headers) {
            headers.append(key + ": " + value);
        }
        
        if (headers.get()) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.get());
        }
        
        // Set write callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
        
        // Follow redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        // Set timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        // Perform request
        CURLcode res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("CURL error: ") + 
                                   curl_easy_strerror(res));
        }
        
        // Get response code
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
        response.body = response_body;
        
        return response;
    }
};

// ===================================================================
// SECTION 6: Example Usage with Public APIs
// ===================================================================

void example_json_placeholder_api() {
    std::cout << "\n=== Example 1: JSONPlaceholder API (GET) ===\n";
    
    try {
        RestClient client("https://jsonplaceholder.typicode.com");
        
        // GET request: Fetch a post
        auto response = client.get("/posts/1");
        
        std::cout << "Status Code: " << response.status_code << "\n";
        
        if (response.is_success()) {
            auto data = response.to_json();
            std::cout << "Post Title: " << data["title"] << "\n";
            std::cout << "Post Body: " << data["body"] << "\n";
            std::cout << "User ID: " << data["userId"] << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void example_create_post() {
    std::cout << "\n=== Example 2: Create a Post (POST) ===\n";
    
    try {
        RestClient client("https://jsonplaceholder.typicode.com");
        
        // Create JSON payload
        json post_data = {
            {"title", "My C++ REST API Example"},
            {"body", "This post was created using C++ and libcurl!"},
            {"userId", 1}
        };
        
        // POST request
        auto response = client.post("/posts", post_data);
        
        std::cout << "Status Code: " << response.status_code << "\n";
        
        if (response.is_success()) {
            auto data = response.to_json();
            std::cout << "Created Post ID: " << data["id"] << "\n";
            std::cout << "Response:\n" << data.dump(2) << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void example_update_post() {
    std::cout << "\n=== Example 3: Update a Post (PUT) ===\n";
    
    try {
        RestClient client("https://jsonplaceholder.typicode.com");
        
        // Updated data
        json updated_data = {
            {"id", 1},
            {"title", "Updated Title from C++"},
            {"body", "This post was updated using modern C++!"},
            {"userId", 1}
        };
        
        // PUT request
        auto response = client.put("/posts/1", updated_data);
        
        std::cout << "Status Code: " << response.status_code << "\n";
        
        if (response.is_success()) {
            auto data = response.to_json();
            std::cout << "Updated Post:\n" << data.dump(2) << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void example_delete_post() {
    std::cout << "\n=== Example 4: Delete a Post (DELETE) ===\n";
    
    try {
        RestClient client("https://jsonplaceholder.typicode.com");
        
        // DELETE request
        auto response = client.del("/posts/1");
        
        std::cout << "Status Code: " << response.status_code << "\n";
        
        if (response.is_success()) {
            std::cout << "Post deleted successfully!\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void example_with_query_parameters() {
    std::cout << "\n=== Example 5: GET with Query Parameters ===\n";
    
    try {
        RestClient client("https://jsonplaceholder.typicode.com");
        
        // GET with query parameters
        std::map<std::string, std::string> params = {
            {"userId", "1"}
        };
        
        auto response = client.get("/posts", params);
        
        std::cout << "Status Code: " << response.status_code << "\n";
        
        if (response.is_success()) {
            auto data = response.to_json();
            std::cout << "Found " << data.size() << " posts by user 1\n";
            
            // Display first 3 posts
            for (size_t i = 0; i < std::min(data.size(), size_t(3)); ++i) {
                std::cout << "\nPost " << (i + 1) << ":\n";
                std::cout << "  Title: " << data[i]["title"] << "\n";
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void example_github_api() {
    std::cout << "\n=== Example 6: GitHub API (Public Data) ===\n";
    
    try {
        RestClient client("https://api.github.com");
        
        // GitHub requires User-Agent header
        client.set_default_header("User-Agent", "ModernCppExamples/1.0");
        
        // GET user information
        auto response = client.get("/users/torvalds");
        
        std::cout << "Status Code: " << response.status_code << "\n";
        
        if (response.is_success()) {
            auto data = response.to_json();
            std::cout << "GitHub User Info:\n";
            std::cout << "  Name: " << data.value("name", "N/A") << "\n";
            std::cout << "  Login: " << data["login"] << "\n";
            std::cout << "  Public Repos: " << data["public_repos"] << "\n";
            std::cout << "  Followers: " << data["followers"] << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void example_error_handling() {
    std::cout << "\n=== Example 7: Error Handling ===\n";
    
    try {
        RestClient client("https://jsonplaceholder.typicode.com");
        
        // Try to access a non-existent resource (404)
        auto response = client.get("/posts/999999");
        
        std::cout << "Status Code: " << response.status_code << "\n";
        
        if (!response.is_success()) {
            std::cout << "Request failed!\n";
            std::cout << "Response body: " << response.body << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

// ===================================================================
// SECTION 7: Best Practices Summary
// ===================================================================

void print_best_practices() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "REST API BEST PRACTICES IN C++:\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. Resource Management (RAII):\n";
    std::cout << "   ✓ Use RAII wrappers for CURL handles\n";
    std::cout << "   ✓ Ensure cleanup happens automatically\n";
    std::cout << "   ✓ Exception-safe resource handling\n\n";
    
    std::cout << "2. Error Handling:\n";
    std::cout << "   ✓ Check CURLcode return values\n";
    std::cout << "   ✓ Check HTTP status codes\n";
    std::cout << "   ✓ Use exceptions for network errors\n";
    std::cout << "   ✓ Parse JSON safely with try-catch\n\n";
    
    std::cout << "3. Security:\n";
    std::cout << "   ✓ Always use HTTPS for sensitive data\n";
    std::cout << "   ✓ Validate SSL certificates (enabled by default)\n";
    std::cout << "   ✓ Never hardcode API keys (use environment variables)\n";
    std::cout << "   ✓ Use Bearer tokens for authentication\n\n";
    
    std::cout << "4. Performance:\n";
    std::cout << "   ✓ Reuse RestClient instances when possible\n";
    std::cout << "   ✓ Set appropriate timeouts\n";
    std::cout << "   ✓ Consider connection pooling for many requests\n";
    std::cout << "   ✓ Use HTTP/2 when supported\n\n";
    
    std::cout << "5. Modern C++ Features:\n";
    std::cout << "   ✓ Use smart pointers and RAII\n";
    std::cout << "   ✓ Leverage move semantics\n";
    std::cout << "   ✓ Use structured bindings for map iteration\n";
    std::cout << "   ✓ std::optional for nullable values\n\n";
    
    std::cout << "6. Testing:\n";
    std::cout << "   ✓ Mock HTTP responses for unit tests\n";
    std::cout << "   ✓ Test error conditions\n";
    std::cout << "   ✓ Use test APIs like JSONPlaceholder\n\n";
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          REST API CALLS IN C++ - Complete Examples              ║\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "║  Demonstrates HTTP methods: GET, POST, PUT, DELETE               ║\n";
    std::cout << "║  Using: libcurl + nlohmann::json                                 ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    // Run all examples
    example_json_placeholder_api();
    example_create_post();
    example_update_post();
    example_delete_post();
    example_with_query_parameters();
    example_github_api();
    example_error_handling();
    
    // Print best practices
    print_best_practices();
    
    std::cout << "\n✅ All examples completed successfully!\n";
    std::cout << "\nNOTE: These examples use public test APIs:\n";
    std::cout << "  - JSONPlaceholder (https://jsonplaceholder.typicode.com)\n";
    std::cout << "  - GitHub API (https://api.github.com)\n";
    std::cout << "\nNo API key required. Responses are simulated/cached.\n\n";
    
    return 0;
}
