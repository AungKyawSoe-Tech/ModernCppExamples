// ===================================================================
// WRAPPING C LIBRARIES IN MODERN C++
// ===================================================================
// This example demonstrates best practices for wrapping C libraries
// (TCP/UDP sockets) in modern C++ with:
// - RAII for automatic resource management
// - noexcept for exception safety
// - [[nodiscard]] for preventing ignored errors
// - extern "C" for C library linkage
// - Strong types and error handling
//
// TOPICS COVERED:
// 1. C library integration (POSIX sockets)
// 2. RAII wrappers for C resources
// 3. noexcept specifications (safe and unsafe uses)
// 4. [[nodiscard]] attribute
// 5. std::optional and std::expected for error handling
// 6. Strong typing over raw C types
// ===================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include <system_error>
#include <cstring>
#include <vector>
#include <memory>
#include <chrono>

// Platform-specific socket headers
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using socket_t = SOCKET;
    constexpr socket_t INVALID_SOCKET_VALUE = INVALID_SOCKET;
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <netdb.h>
    using socket_t = int;
    constexpr socket_t INVALID_SOCKET_VALUE = -1;
    #define CLOSE_SOCKET close
#endif

// ===================================================================
// SECTION 1: C LIBRARY FUNCTIONS (extern "C" linkage)
// ===================================================================
// These are C functions - they use C calling conventions and linkage
// extern "C" prevents C++ name mangling

extern "C" {
    // Example: Custom C utility function we might have in a C library
    int c_validate_port(int port) {
        return (port > 0 && port <= 65535) ? 1 : 0;
    }
    
    // Note: socket(), bind(), listen(), etc. are already extern "C"
    // from system headers, so we don't redeclare them
}

// ===================================================================
// SECTION 2: ERROR HANDLING TYPES
// ===================================================================

// Modern C++ error type using std::optional
enum class SocketError {
    Success,
    InvalidSocket,
    BindFailed,
    ListenFailed,
    ConnectFailed,
    SendFailed,
    ReceiveFailed,
    AcceptFailed,
    SocketOptionFailed,
    AddressResolutionFailed,
    TimeoutExpired,
    WouldBlock
};

// Convert error code to string
[[nodiscard]] constexpr std::string_view error_to_string(SocketError error) noexcept {
    switch (error) {
        case SocketError::Success: return "Success";
        case SocketError::InvalidSocket: return "Invalid socket";
        case SocketError::BindFailed: return "Bind failed";
        case SocketError::ListenFailed: return "Listen failed";
        case SocketError::ConnectFailed: return "Connect failed";
        case SocketError::SendFailed: return "Send failed";
        case SocketError::ReceiveFailed: return "Receive failed";
        case SocketError::AcceptFailed: return "Accept failed";
        case SocketError::SocketOptionFailed: return "Socket option failed";
        case SocketError::AddressResolutionFailed: return "Address resolution failed";
        case SocketError::TimeoutExpired: return "Timeout expired";
        case SocketError::WouldBlock: return "Operation would block";
        default: return "Unknown error";
    }
}

// Result type for operations that can fail
template<typename T>
using Result = std::optional<T>;

// ===================================================================
// SECTION 3: RAII SOCKET WRAPPER
// ===================================================================
// Modern C++ wrapper that automatically manages socket lifecycle

class Socket {
private:
    socket_t fd_;
    bool is_valid_;
    
public:
    // Constructor - creates socket
    // noexcept(false) because it may throw
    Socket(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0) 
        : fd_(::socket(domain, type, protocol)), 
          is_valid_(fd_ != INVALID_SOCKET_VALUE) {
        
        if (!is_valid_) {
            std::cerr << "Socket creation failed" << std::endl;
        } else {
            std::cout << "✓ Socket created (fd=" << fd_ << ")" << std::endl;
        }
    }
    
    // Tag type for from_fd constructor
    struct from_fd_t {};
    static constexpr from_fd_t from_fd{};
    
    // Construct from existing socket (for accept())
    Socket(from_fd_t, socket_t fd) noexcept 
        : fd_(fd), is_valid_(fd != INVALID_SOCKET_VALUE) {}
    
    // Destructor - RAII automatically closes socket
    // noexcept because destructors should not throw
    ~Socket() noexcept {
        if (is_valid_ && fd_ != INVALID_SOCKET_VALUE) {
            std::cout << "✓ Socket closing (fd=" << fd_ << ")" << std::endl;
            CLOSE_SOCKET(fd_);
        }
    }
    
    // Delete copy operations - sockets are unique resources
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    
    // Move operations - transfer ownership
    Socket(Socket&& other) noexcept 
        : fd_(other.fd_), is_valid_(other.is_valid_) {
        other.fd_ = INVALID_SOCKET_VALUE;
        other.is_valid_ = false;
    }
    
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            if (is_valid_) {
                CLOSE_SOCKET(fd_);
            }
            fd_ = other.fd_;
            is_valid_ = other.is_valid_;
            other.fd_ = INVALID_SOCKET_VALUE;
            other.is_valid_ = false;
        }
        return *this;
    }
    
    // Check if socket is valid
    // [[nodiscard]] - result should not be ignored!
    [[nodiscard]] bool is_valid() const noexcept {
        return is_valid_;
    }
    
    // Get raw socket descriptor
    // [[nodiscard]] - caller needs this value!
    [[nodiscard]] socket_t get() const noexcept {
        return fd_;
    }
    
    // Bind to address
    // [[nodiscard]] - error must be checked!
    [[nodiscard]] SocketError bind(const std::string& address, uint16_t port) noexcept {
        if (!is_valid_) {
            return SocketError::InvalidSocket;
        }
        
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        
        if (address.empty() || address == "0.0.0.0") {
            addr.sin_addr.s_addr = INADDR_ANY;
        } else {
            addr.sin_addr.s_addr = inet_addr(address.c_str());
        }
        
        if (::bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            return SocketError::BindFailed;
        }
        
        std::cout << "✓ Socket bound to " << address << ":" << port << std::endl;
        return SocketError::Success;
    }
    
    // Listen for connections
    // [[nodiscard]] - error must be checked!
    [[nodiscard]] SocketError listen(int backlog = 10) noexcept {
        if (!is_valid_) {
            return SocketError::InvalidSocket;
        }
        
        if (::listen(fd_, backlog) < 0) {
            return SocketError::ListenFailed;
        }
        
        std::cout << "✓ Socket listening (backlog=" << backlog << ")" << std::endl;
        return SocketError::Success;
    }
    
    // Accept connection - returns new Socket
    // [[nodiscard]] - must handle the result!
    [[nodiscard]] Result<Socket> accept() noexcept {
        if (!is_valid_) {
            return std::nullopt;
        }
        
        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);
        
        socket_t client_fd = ::accept(fd_, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
        
        if (client_fd == INVALID_SOCKET_VALUE) {
            return std::nullopt;
        }
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "✓ Connection accepted from " << client_ip 
                  << ":" << ntohs(client_addr.sin_port) << std::endl;
        
        return Socket(Socket::from_fd, client_fd);
    }
    
    // Connect to remote address
    // [[nodiscard]] - error must be checked!
    [[nodiscard]] SocketError connect(const std::string& address, uint16_t port) noexcept {
        if (!is_valid_) {
            return SocketError::InvalidSocket;
        }
        
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(address.c_str());
        
        if (::connect(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            return SocketError::ConnectFailed;
        }
        
        std::cout << "✓ Connected to " << address << ":" << port << std::endl;
        return SocketError::Success;
    }
    
    // Send data
    // [[nodiscard]] - must check if send succeeded!
    [[nodiscard]] Result<size_t> send(const std::string& data) noexcept {
        if (!is_valid_) {
            return std::nullopt;
        }
        
        ssize_t sent = ::send(fd_, data.c_str(), data.length(), 0);
        
        if (sent < 0) {
            return std::nullopt;
        }
        
        std::cout << "✓ Sent " << sent << " bytes" << std::endl;
        return static_cast<size_t>(sent);
    }
    
    // Receive data
    // [[nodiscard]] - must handle received data!
    [[nodiscard]] Result<std::string> receive(size_t max_length = 4096) noexcept {
        if (!is_valid_) {
            return std::nullopt;
        }
        
        std::vector<char> buffer(max_length);
        ssize_t received = ::recv(fd_, buffer.data(), buffer.size(), 0);
        
        if (received <= 0) {
            return std::nullopt;
        }
        
        std::cout << "✓ Received " << received << " bytes" << std::endl;
        return std::string(buffer.data(), received);
    }
    
    // Set socket option
    // [[nodiscard]] - error must be checked!
    [[nodiscard]] SocketError set_reuse_address(bool enable) noexcept {
        if (!is_valid_) {
            return SocketError::InvalidSocket;
        }
        
        int opt = enable ? 1 : 0;
        if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, 
                      reinterpret_cast<const char*>(&opt), sizeof(opt)) < 0) {
            return SocketError::SocketOptionFailed;
        }
        
        std::cout << "✓ SO_REUSEADDR set to " << (enable ? "true" : "false") << std::endl;
        return SocketError::Success;
    }
};

// ===================================================================
// SECTION 4: HIGH-LEVEL TCP SERVER CLASS
// ===================================================================

class TcpServer {
private:
    Socket listen_socket_;
    uint16_t port_;
    bool is_running_;
    
public:
    // Constructor
    // noexcept(false) - may throw if initialization fails critically
    explicit TcpServer(uint16_t port) 
        : listen_socket_(AF_INET, SOCK_STREAM, 0), 
          port_(port),
          is_running_(false) {
        
        if (!listen_socket_.is_valid()) {
            throw std::runtime_error("Failed to create listen socket");
        }
    }
    
    // Start server
    // [[nodiscard]] - must check if server started successfully!
    [[nodiscard]] SocketError start() noexcept {
        // Enable address reuse
        auto err = listen_socket_.set_reuse_address(true);
        if (err != SocketError::Success) {
            return err;
        }
        
        // Bind to port
        err = listen_socket_.bind("0.0.0.0", port_);
        if (err != SocketError::Success) {
            return err;
        }
        
        // Start listening
        err = listen_socket_.listen();
        if (err != SocketError::Success) {
            return err;
        }
        
        is_running_ = true;
        std::cout << "✓ TCP Server started on port " << port_ << std::endl;
        return SocketError::Success;
    }
    
    // Accept one client connection
    // [[nodiscard]] - must handle the client socket!
    [[nodiscard]] Result<Socket> accept_client() noexcept {
        if (!is_running_) {
            return std::nullopt;
        }
        return listen_socket_.accept();
    }
    
    // Check if server is running
    [[nodiscard]] bool is_running() const noexcept {
        return is_running_;
    }
    
    // Stop server
    void stop() noexcept {
        is_running_ = false;
        std::cout << "✓ TCP Server stopped" << std::endl;
    }
};

// ===================================================================
// SECTION 5: HIGH-LEVEL TCP CLIENT CLASS
// ===================================================================

class TcpClient {
private:
    Socket socket_;
    bool is_connected_;
    
public:
    // Constructor
    TcpClient() 
        : socket_(AF_INET, SOCK_STREAM, 0),
          is_connected_(false) {}
    
    // Connect to server
    // [[nodiscard]] - must check connection status!
    [[nodiscard]] SocketError connect(const std::string& host, uint16_t port) noexcept {
        if (!socket_.is_valid()) {
            return SocketError::InvalidSocket;
        }
        
        auto err = socket_.connect(host, port);
        if (err == SocketError::Success) {
            is_connected_ = true;
        }
        return err;
    }
    
    // Send message
    // [[nodiscard]] - must check if send succeeded!
    [[nodiscard]] Result<size_t> send(const std::string& message) noexcept {
        if (!is_connected_) {
            return std::nullopt;
        }
        return socket_.send(message);
    }
    
    // Receive message
    // [[nodiscard]] - must handle received data!
    [[nodiscard]] Result<std::string> receive() noexcept {
        if (!is_connected_) {
            return std::nullopt;
        }
        return socket_.receive();
    }
    
    // Check if connected
    [[nodiscard]] bool is_connected() const noexcept {
        return is_connected_;
    }
};

// ===================================================================
// SECTION 6: DEMONSTRATION OF noexcept USAGE
// ===================================================================

void demonstrate_noexcept() {
    std::cout << "\n=== NOEXCEPT USAGE ===" << std::endl;
    
    std::cout << "\n✅ SAFE noexcept usage:" << std::endl;
    std::cout << "   • Getters that don't throw: is_valid() noexcept" << std::endl;
    std::cout << "   • Destructors: ~Socket() noexcept" << std::endl;
    std::cout << "   • Move operations: Socket(Socket&&) noexcept" << std::endl;
    std::cout << "   • Simple checks: error_to_string() noexcept" << std::endl;
    
    std::cout << "\n⚠️  Operations marked noexcept (but handle errors internally):" << std::endl;
    std::cout << "   • bind() noexcept - returns error code instead of throwing" << std::endl;
    std::cout << "   • send() noexcept - returns std::optional for errors" << std::endl;
    std::cout << "   • receive() noexcept - returns std::optional" << std::endl;
    
    std::cout << "\n❌ NOT noexcept (may throw):" << std::endl;
    std::cout << "   • Constructor if critical initialization fails" << std::endl;
    std::cout << "   • std::string operations (may throw std::bad_alloc)" << std::endl;
    
    std::cout << "\n💡 KEY PRINCIPLE:" << std::endl;
    std::cout << "   Mark noexcept when you guarantee no exceptions will escape" << std::endl;
    std::cout << "   Use error codes/std::optional for expected failures" << std::endl;
}

// ===================================================================
// SECTION 7: DEMONSTRATION OF [[nodiscard]]
// ===================================================================

void demonstrate_nodiscard() {
    std::cout << "\n=== [[nodiscard]] ATTRIBUTE ===" << std::endl;
    
    std::cout << "\n✓ [[nodiscard]] forces checking return values:" << std::endl;
    
    Socket sock(AF_INET, SOCK_STREAM, 0);
    
    // GOOD: Checking the return value
    if (auto err = sock.bind("0.0.0.0", 8080); err != SocketError::Success) {
        std::cout << "   Bind error checked: " << error_to_string(err) << std::endl;
    }
    
    // BAD: Would produce compiler warning (if we uncommented)
    // sock.bind("0.0.0.0", 8080);  // Warning: ignoring return value!
    
    std::cout << "\n💡 BENEFITS:" << std::endl;
    std::cout << "   ✓ Prevents forgetting to check errors" << std::endl;
    std::cout << "   ✓ Compiler warns about ignored return values" << std::endl;
    std::cout << "   ✓ Makes APIs safer to use" << std::endl;
    std::cout << "   ✓ Self-documenting - shows value is important" << std::endl;
}

// ===================================================================
// SECTION 8: RAII BENEFITS DEMONSTRATION
// ===================================================================

void demonstrate_raii() {
    std::cout << "\n=== RAII AUTOMATIC RESOURCE MANAGEMENT ===" << std::endl;
    
    std::cout << "\nCreating socket in scope:" << std::endl;
    {
        Socket sock(AF_INET, SOCK_STREAM, 0);
        auto err = sock.bind("0.0.0.0", 9999);
        if (err == SocketError::Success) {
            std::cout << "   Socket is active (fd=" << sock.get() << ")" << std::endl;
        }
        // No need to manually close - destructor handles it!
    }
    std::cout << "   ✓ Socket automatically closed when out of scope!" << std::endl;
    
    std::cout << "\n💡 RAII GUARANTEES:" << std::endl;
    std::cout << "   ✓ Resource acquired in constructor" << std::endl;
    std::cout << "   ✓ Resource released in destructor" << std::endl;
    std::cout << "   ✓ Exception-safe - cleanup always happens" << std::endl;
    std::cout << "   ✓ No manual cleanup needed" << std::endl;
    std::cout << "   ✓ No resource leaks possible" << std::endl;
}

// ===================================================================
// SECTION 9: EXTERN "C" USAGE
// ===================================================================

void demonstrate_extern_c() {
    std::cout << "\n=== EXTERN \"C\" LINKAGE ===" << std::endl;
    
    std::cout << "\n📚 Purpose: Interface with C libraries" << std::endl;
    std::cout << "   • Prevents C++ name mangling" << std::endl;
    std::cout << "   • Allows C code to call C++ functions" << std::endl;
    std::cout << "   • Enables linking with C libraries" << std::endl;
    
    // Call our C function
    int port = 8080;
    int is_valid = c_validate_port(port);
    std::cout << "\n✓ Called C function: c_validate_port(" << port << ") = " 
              << (is_valid ? "valid" : "invalid") << std::endl;
    
    std::cout << "\n💡 USAGE PATTERNS:" << std::endl;
    std::cout << "   extern \"C\" { ... }    - Wrap C declarations" << std::endl;
    std::cout << "   #ifdef __cplusplus     - Conditional compilation" << std::endl;
    std::cout << "   System headers (socket.h) already use extern \"C\"" << std::endl;
}

// ===================================================================
// SECTION 10: PRACTICAL EXAMPLES
// ===================================================================

void example_echo_server() {
    std::cout << "\n=== EXAMPLE: SIMPLE ECHO SERVER ===" << std::endl;
    std::cout << "Simulating echo server workflow...\n" << std::endl;
    
    try {
        TcpServer server(12345);
        
        // Start server
        if (auto err = server.start(); err != SocketError::Success) {
            std::cout << "Failed to start server: " << error_to_string(err) << std::endl;
            return;
        }
        
        std::cout << "Echo server would accept connections and echo messages..." << std::endl;
        std::cout << "(Actual network operations skipped for demo)" << std::endl;
        
        server.stop();
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << "\n✓ Server resources automatically cleaned up by RAII!" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║       WRAPPING C LIBRARIES IN MODERN C++                         ║\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "║  Demonstrates: RAII, noexcept, [[nodiscard]], extern \"C\"        ║\n";
    std::cout << "║  Example: TCP/UDP Socket Wrapper                                ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
#ifdef _WIN32
    // Initialize Winsock on Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }
    std::cout << "\n✓ Winsock initialized (Windows)" << std::endl;
#else
    std::cout << "\n✓ Using POSIX sockets (Linux/Unix)" << std::endl;
#endif
    
    demonstrate_raii();
    demonstrate_noexcept();
    demonstrate_nodiscard();
    demonstrate_extern_c();
    example_echo_server();
    
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "BEST PRACTICES SUMMARY:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n1. RAII (Resource Acquisition Is Initialization):" << std::endl;
    std::cout << "   ✓ Acquire resources in constructor" << std::endl;
    std::cout << "   ✓ Release resources in destructor" << std::endl;
    std::cout << "   ✓ Make classes non-copyable for unique resources" << std::endl;
    std::cout << "   ✓ Implement move semantics for transfer" << std::endl;
    
    std::cout << "\n2. noexcept Usage:" << std::endl;
    std::cout << "   ✓ Mark destructors noexcept (implicit in C++11+)" << std::endl;
    std::cout << "   ✓ Mark move operations noexcept (enables optimizations)" << std::endl;
    std::cout << "   ✓ Mark operations that handle errors via return codes" << std::endl;
    std::cout << "   ✓ DON'T mark operations that may allocate memory" << std::endl;
    
    std::cout << "\n3. [[nodiscard]] Usage:" << std::endl;
    std::cout << "   ✓ Use for error codes that must be checked" << std::endl;
    std::cout << "   ✓ Use for expensive operations (no wasted work)" << std::endl;
    std::cout << "   ✓ Use for functions where ignoring result is likely a bug" << std::endl;
    std::cout << "   ✓ Makes APIs self-documenting and safer" << std::endl;
    
    std::cout << "\n4. extern \"C\" Linkage:" << std::endl;
    std::cout << "   ✓ Wrap C library includes in extern \"C\" blocks" << std::endl;
    std::cout << "   ✓ Use #ifdef __cplusplus for C/C++ compatibility" << std::endl;
    std::cout << "   ✓ Only C-compatible functions can be extern \"C\"" << std::endl;
    std::cout << "   ✓ No overloading, no classes, no templates" << std::endl;
    
    std::cout << "\n5. C/C++ Interop Patterns:" << std::endl;
    std::cout << "   ✓ Create thin C++ wrapper classes (RAII)" << std::endl;
    std::cout << "   ✓ Hide C types behind strong C++ types" << std::endl;
    std::cout << "   ✓ Use std::optional for nullable results" << std::endl;
    std::cout << "   ✓ Use error codes/exceptions instead of C error globals" << std::endl;
    std::cout << "   ✓ Provide modern C++ interfaces (string_view, span, etc.)" << std::endl;
    
    std::cout << "\n6. Modern C++ Features for C Wrappers:" << std::endl;
    std::cout << "   ✓ std::optional<T> for operations that may fail" << std::endl;
    std::cout << "   ✓ std::string_view for non-owning string parameters" << std::endl;
    std::cout << "   ✓ std::span<T> for array views (C++20)" << std::endl;
    std::cout << "   ✓ std::unique_ptr with custom deleter" << std::endl;
    std::cout << "   ✓ enum class for type-safe error codes" << std::endl;
    
    std::cout << "\n✅ All socket resources properly cleaned up by RAII!\n" << std::endl;
    
#ifdef _WIN32
    WSACleanup();
    std::cout << "✓ Winsock cleaned up\n" << std::endl;
#endif
    
    return 0;
}
