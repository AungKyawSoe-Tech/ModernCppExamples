// NVIIdiomTemplateMethod.cpp
// Demonstrates the Non-Virtual Interface (NVI) Idiom and Template Method Pattern
// Based on Herb Sutter's "Virtuality" guidelines
//
// KEY CONCEPTS:
// 1. Public non-virtual methods define the interface (Template Method)
// 2. Private/protected virtual methods are customization points
// 3. Base class controls the algorithm flow
// 4. Derived classes customize specific steps
//
// HERB SUTTER'S GUIDELINES:
// Guideline #1: Prefer to make interfaces non-virtual (Template Method)
// Guideline #2: Prefer to make virtual functions private
// Guideline #3: Only if derived needs base implementation, make virtual protected
// Guideline #4: Destructor should be public+virtual OR protected+non-virtual
//
// WHEN TO USE NVI:
// ✓ Need invariant algorithm with customizable steps
// ✓ Want to enforce pre/post conditions
// ✓ Need to guarantee certain code always runs
// ✓ Want separation of interface from implementation
//
// WHEN TO AVOID NVI:
// ✗ Simple interfaces with no invariants
// ✗ Performance-critical code (tiny extra overhead)
// ✗ When derived classes need full control
// ✗ Pure abstract interfaces (use public virtual)

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <stdexcept>

using namespace std::chrono_literals;

// ============================================================================
// SECTION 1: Basic NVI Idiom - Template Method Pattern
// ============================================================================

namespace basic_nvi {

class DataProcessor {
public:
    // PUBLIC NON-VIRTUAL: This is the Template Method
    // Defines the algorithm structure that CANNOT be changed by derived classes
    void process() {
        std::cout << "  [DataProcessor::process] Algorithm started\n";
        
        // Invariant: Always validate before processing
        if (!validate()) {
            std::cout << "  ✗ Validation failed, aborting\n";
            return;
        }
        
        // Call customization points (private virtuals)
        load();
        transform();
        save();
        
        std::cout << "  [DataProcessor::process] Algorithm completed\n";
    }
    
    virtual ~DataProcessor() = default;

private:
    // PRIVATE VIRTUAL: Customization points that derived classes override
    // These are the "steps" of the algorithm
    
    virtual bool validate() {
        std::cout << "    [Base] Default validation\n";
        return true;  // Base default: always valid
    }
    
    virtual void load() = 0;      // Pure virtual - must override
    virtual void transform() = 0;  // Pure virtual - must override
    virtual void save() = 0;       // Pure virtual - must override
};

class CSVProcessor : public DataProcessor {
private:
    // Override private virtuals to provide CSV-specific behavior
    void load() override {
        std::cout << "    [CSV] Loading CSV file...\n";
    }
    
    void transform() override {
        std::cout << "    [CSV] Transforming CSV data...\n";
    }
    
    void save() override {
        std::cout << "    [CSV] Saving CSV results...\n";
    }
};

class JSONProcessor : public DataProcessor {
private:
    bool validate() override {
        std::cout << "    [JSON] Custom JSON validation\n";
        return true;
    }
    
    void load() override {
        std::cout << "    [JSON] Loading JSON file...\n";
    }
    
    void transform() override {
        std::cout << "    [JSON] Transforming JSON data...\n";
    }
    
    void save() override {
        std::cout << "    [JSON] Saving JSON results...\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 1: Basic NVI Idiom - Template Method ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. Processing CSV data:\n";
    CSVProcessor csv;
    csv.process();  // Client calls public non-virtual method
    
    std::cout << "\n2. Processing JSON data:\n";
    JSONProcessor json;
    json.process();
    
    std::cout << "\n✓ Benefits:\n";
    std::cout << "  • Base class controls algorithm flow (validation always runs)\n";
    std::cout << "  • Derived classes customize only the steps they need\n";
    std::cout << "  • Cannot accidentally skip validation or change algorithm\n";
    std::cout << "  • Clear separation: public interface vs private implementation\n";
}

} // namespace basic_nvi

// ============================================================================
// SECTION 2: NVI with Pre/Post Conditions - Real Power
// ============================================================================

namespace preconditions_example {

class DatabaseConnection {
    bool connected_ = false;
    int transaction_count_ = 0;

public:
    // PUBLIC NON-VIRTUAL: Template Method with pre/post conditions
    void executeQuery(const std::string& query) {
        std::cout << "  [executeQuery] Starting query execution\n";
        
        // PRE-CONDITION: Must be connected
        if (!connected_) {
            std::cout << "  ✗ Error: Not connected to database!\n";
            return;
        }
        
        std::cout << "  [Pre] Acquiring lock...\n";
        std::cout << "  [Pre] Starting transaction " << ++transaction_count_ << "\n";
        
        // Call customization point
        try {
            doExecuteQuery(query);
            
            // POST-CONDITION: Commit transaction
            std::cout << "  [Post] Committing transaction\n";
            std::cout << "  [Post] Releasing lock\n";
            
        } catch (const std::exception& e) {
            // POST-CONDITION: Rollback on error
            std::cout << "  [Post] Rolling back transaction due to error\n";
            std::cout << "  [Post] Releasing lock\n";
            throw;
        }
        
        std::cout << "  [executeQuery] Query completed successfully\n";
    }
    
    void connect() {
        std::cout << "  [Connection] Connected to database\n";
        connected_ = true;
    }
    
    virtual ~DatabaseConnection() = default;

private:
    // PRIVATE VIRTUAL: Derived classes provide query logic
    virtual void doExecuteQuery(const std::string& query) = 0;
};

class MySQLConnection : public DatabaseConnection {
private:
    void doExecuteQuery(const std::string& query) override {
        std::cout << "    [MySQL] Executing: " << query << "\n";
        std::cout << "    [MySQL] Query executed successfully\n";
    }
};

class PostgreSQLConnection : public DatabaseConnection {
private:
    void doExecuteQuery(const std::string& query) override {
        std::cout << "    [PostgreSQL] Executing: " << query << "\n";
        if (query.find("DROP") != std::string::npos) {
            throw std::runtime_error("DROP statements not allowed!");
        }
        std::cout << "    [PostgreSQL] Query executed successfully\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 2: NVI with Pre/Post Conditions ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. MySQL query execution:\n";
    MySQLConnection mysql;
    mysql.connect();
    mysql.executeQuery("SELECT * FROM users");
    
    std::cout << "\n2. PostgreSQL with error handling:\n";
    PostgreSQLConnection postgres;
    postgres.connect();
    try {
        postgres.executeQuery("DROP TABLE users");
    } catch (const std::exception& e) {
        std::cout << "  ✓ Caught exception: " << e.what() << "\n";
        std::cout << "  ✓ Transaction rolled back automatically!\n";
    }
    
    std::cout << "\n3. Attempting query without connection:\n";
    MySQLConnection mysql2;
    mysql2.executeQuery("SELECT * FROM users");
    
    std::cout << "\n✓ NVI ensures:\n";
    std::cout << "  • Pre-conditions always checked (connection, locks)\n";
    std::cout << "  • Post-conditions always executed (commit, rollback, cleanup)\n";
    std::cout << "  • Derived classes cannot bypass these guarantees!\n";
}

} // namespace preconditions_example

// ============================================================================
// SECTION 3: Protected Virtual - When Derived Needs Base Implementation
// ============================================================================

namespace protected_virtual_example {

class Logger {
public:
    // PUBLIC NON-VIRTUAL: Template Method
    void log(const std::string& message) {
        std::cout << "  [log] Preparing log entry\n";
        addTimestamp();
        writeLog(message);
        flush();
    }
    
    virtual ~Logger() = default;

protected:
    // PROTECTED VIRTUAL: Derived classes may want to call base implementation
    virtual void writeLog(const std::string& message) {
        std::cout << "    [Base Logger] " << message << "\n";
    }

private:
    void addTimestamp() {
        std::cout << "    [Timestamp] 2026-01-02 10:30:45\n";
    }
    
    void flush() {
        std::cout << "    [Flush] Log buffer flushed\n";
    }
};

class FileLogger : public Logger {
protected:
    void writeLog(const std::string& message) override {
        // Call base implementation first
        Logger::writeLog(message);
        
        // Add file-specific logging
        std::cout << "    [FileLogger] Writing to file: logs.txt\n";
    }
};

class NetworkLogger : public Logger {
protected:
    void writeLog(const std::string& message) override {
        // Call base implementation
        Logger::writeLog(message);
        
        // Send over network
        std::cout << "    [NetworkLogger] Sending to log server 192.168.1.100\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 3: Protected Virtual (Guideline #3) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. File logger with base implementation:\n";
    FileLogger file_logger;
    file_logger.log("Application started");
    
    std::cout << "\n2. Network logger with base implementation:\n";
    NetworkLogger net_logger;
    net_logger.log("User logged in");
    
    std::cout << "\n✓ Protected virtual when:\n";
    std::cout << "  • Derived classes need to call base implementation\n";
    std::cout << "  • Want to extend, not replace, base behavior\n";
    std::cout << "  • Common pattern: call base, then add specific logic\n";
}

} // namespace protected_virtual_example

// ============================================================================
// SECTION 4: When NOT to Use NVI - Alternatives
// ============================================================================

namespace when_not_to_use_nvi {

// ANTI-PATTERN: NVI overkill for simple interfaces
class BadExample_OverengineeredNVI {
public:
    void getValue() {
        doGetValue();  // Unnecessary indirection!
    }

private:
    virtual void doGetValue() = 0;
};

// BETTER: Simple public virtual for simple interfaces
class GoodExample_SimpleInterface {
public:
    virtual int getValue() const = 0;  // Direct and clear
    virtual ~GoodExample_SimpleInterface() = default;
};

class SimpleImpl : public GoodExample_SimpleInterface {
public:
    int getValue() const override { return 42; }
};

// WHEN TO USE PUBLIC VIRTUAL: Pure abstract interfaces (like Java interfaces)
class IDrawable {
public:
    virtual void draw() = 0;
    virtual void move(int x, int y) = 0;
    virtual ~IDrawable() = default;
};

class Circle : public IDrawable {
public:
    void draw() override {
        std::cout << "    Drawing circle\n";
    }
    
    void move(int x, int y) override {
        std::cout << "    Moving circle to (" << x << ", " << y << ")\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 4: When NOT to Use NVI ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. Simple interface - public virtual is fine:\n";
    SimpleImpl impl;
    std::cout << "    Value: " << impl.getValue() << "\n";
    
    std::cout << "\n2. Pure abstract interface - use public virtual:\n";
    Circle circle;
    circle.draw();
    circle.move(10, 20);
    
    std::cout << "\n✗ DON'T use NVI when:\n";
    std::cout << "  • Simple getter/setter interfaces\n";
    std::cout << "  • No invariants to enforce\n";
    std::cout << "  • No pre/post conditions needed\n";
    std::cout << "  • Pure abstract interfaces (like Java interfaces)\n";
    std::cout << "  • Performance critical (tiny overhead of extra call)\n\n";
    
    std::cout << "✓ DO use NVI when:\n";
    std::cout << "  • Need to enforce pre/post conditions\n";
    std::cout << "  • Have invariant algorithm with customizable steps\n";
    std::cout << "  • Want to guarantee certain code always runs\n";
    std::cout << "  • Need clear separation of interface from implementation\n";
}

} // namespace when_not_to_use_nvi

// ============================================================================
// SECTION 5: Real-World Example - HTTP Request Handler
// ============================================================================

namespace http_handler_example {

class HTTPRequestHandler {
public:
    // PUBLIC NON-VIRTUAL: Template Method for handling HTTP requests
    void handleRequest(const std::string& method, const std::string& path) {
        std::cout << "  [HTTP] Received " << method << " " << path << "\n";
        
        // PRE: Always authenticate
        std::cout << "  [Pre] Authenticating request...\n";
        if (!authenticate()) {
            std::cout << "  ✗ 401 Unauthorized\n";
            return;
        }
        
        // PRE: Always validate input
        std::cout << "  [Pre] Validating input...\n";
        if (!validateInput(method, path)) {
            std::cout << "  ✗ 400 Bad Request\n";
            return;
        }
        
        // PRE: Check rate limiting
        std::cout << "  [Pre] Checking rate limits...\n";
        if (!checkRateLimit()) {
            std::cout << "  ✗ 429 Too Many Requests\n";
            return;
        }
        
        // Call customization point
        try {
            std::cout << "  [Processing] Handling request...\n";
            processRequest(method, path);
            
            // POST: Always log successful requests
            std::cout << "  [Post] Logging successful request\n";
            logAccess(method, path, 200);
            std::cout << "  ✓ 200 OK\n";
            
        } catch (const std::exception& e) {
            // POST: Always log errors
            std::cout << "  [Post] Logging error\n";
            logAccess(method, path, 500);
            std::cout << "  ✗ 500 Internal Server Error: " << e.what() << "\n";
        }
    }
    
    virtual ~HTTPRequestHandler() = default;

private:
    // PRIVATE VIRTUAL: Customization points
    virtual bool authenticate() {
        return true;  // Default: no auth
    }
    
    virtual bool validateInput([[maybe_unused]] const std::string& method, 
                              [[maybe_unused]] const std::string& path) {
        return true;  // Default: always valid
    }
    
    virtual bool checkRateLimit() {
        return true;  // Default: no limits
    }
    
    virtual void processRequest(const std::string& method, 
                               const std::string& path) = 0;
    
    virtual void logAccess(const std::string& method, 
                          const std::string& path, 
                          int status_code) {
        std::cout << "    [Log] " << method << " " << path 
                  << " - " << status_code << "\n";
    }
};

class UserAPIHandler : public HTTPRequestHandler {
private:
    bool authenticate() override {
        std::cout << "    [Auth] Checking JWT token...\n";
        return true;  // Simplified
    }
    
    bool validateInput(const std::string& method, 
                      const std::string& path) override {
        if (path.find("/api/users") != 0) {
            std::cout << "    [Validation] Invalid path\n";
            return false;
        }
        return true;
    }
    
    void processRequest(const std::string& method, 
                       const std::string& path) override {
        if (method == "GET") {
            std::cout << "    [Handler] Fetching user data from database\n";
            std::cout << "    [Handler] Returning user list\n";
        } else if (method == "POST") {
            std::cout << "    [Handler] Creating new user\n";
            std::cout << "    [Handler] User created successfully\n";
        }
    }
};

class PublicAPIHandler : public HTTPRequestHandler {
private:
    int request_count_ = 0;
    
    bool checkRateLimit() override {
        if (++request_count_ > 3) {
            std::cout << "    [RateLimit] Exceeded rate limit!\n";
            return false;
        }
        return true;
    }
    
    void processRequest([[maybe_unused]] const std::string& method, 
                       const std::string& path) override {
        std::cout << "    [Handler] Processing public API request: " << path << "\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 5: Real-World - HTTP Request Handler ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. User API with authentication:\n";
    UserAPIHandler user_api;
    user_api.handleRequest("GET", "/api/users/123");
    
    std::cout << "\n2. Public API with rate limiting:\n";
    PublicAPIHandler public_api;
    for (int i = 1; i <= 5; ++i) {
        std::cout << "\n  Request " << i << ":\n";
        public_api.handleRequest("GET", "/api/public/data");
        if (i >= 3) {
            break;  // Stop after rate limit
        }
    }
    
    std::cout << "\n✓ NVI guarantees:\n";
    std::cout << "  • Authentication always checked before processing\n";
    std::cout << "  • Input validation cannot be bypassed\n";
    std::cout << "  • Rate limiting enforced consistently\n";
    std::cout << "  • Logging always happens (success or error)\n";
    std::cout << "  • Derived classes customize logic, not security/logging!\n";
}

} // namespace http_handler_example

// ============================================================================
// SECTION 6: Guideline #4 - Destructor Rules
// ============================================================================

namespace destructor_rules {

// CORRECT: Public virtual destructor for polymorphic base class
class PolymorphicBase {
public:
    virtual ~PolymorphicBase() {
        std::cout << "    [PolymorphicBase] Destructor\n";
    }
    
    virtual void doSomething() = 0;
};

class DerivedFromPolymorphic : public PolymorphicBase {
public:
    ~DerivedFromPolymorphic() override {
        std::cout << "    [DerivedFromPolymorphic] Destructor\n";
    }
    
    void doSomething() override {
        std::cout << "    [DerivedFromPolymorphic] Doing something\n";
    }
};

// CORRECT: Protected non-virtual destructor for non-polymorphic base
class NonPolymorphicBase {
protected:
    ~NonPolymorphicBase() {
        std::cout << "    [NonPolymorphicBase] Destructor\n";
    }
    
public:
    void doWork() {
        std::cout << "    [NonPolymorphicBase] Working\n";
    }
};

class DerivedFromNonPolymorphic : public NonPolymorphicBase {
public:
    ~DerivedFromNonPolymorphic() {
        std::cout << "    [DerivedFromNonPolymorphic] Destructor\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 6: Guideline #4 - Destructor Rules ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. Polymorphic base (public virtual destructor):\n";
    {
        PolymorphicBase* ptr = new DerivedFromPolymorphic();
        ptr->doSomething();
        delete ptr;  // ✓ Calls derived destructor first, then base
    }
    
    std::cout << "\n2. Non-polymorphic base (protected non-virtual destructor):\n";
    {
        DerivedFromNonPolymorphic obj;
        obj.doWork();
        // Cannot do: NonPolymorphicBase* ptr = new DerivedFromNonPolymorphic();
        // delete ptr;  // Would be undefined behavior!
        // Protected destructor prevents polymorphic deletion
    }
    
    std::cout << "\n✓ Guideline #4 (Herb Sutter):\n";
    std::cout << "  • Polymorphic base class: public + virtual destructor\n";
    std::cout << "  • Non-polymorphic base class: protected + non-virtual destructor\n";
    std::cout << "  • Protected destructor prevents polymorphic deletion\n";
    std::cout << "  • Virtual destructor allows safe polymorphic deletion\n";
}

} // namespace destructor_rules

// ============================================================================
// SECTION 7: Summary - Complete Guidelines
// ============================================================================

void show_summary() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Herb Sutter's Virtuality Guidelines - Complete Summary ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "GUIDELINE #1: Prefer non-virtual interfaces (Template Method)\n";
    std::cout << "  • Public non-virtual methods define the interface\n";
    std::cout << "  • These are the \"Template Methods\" that orchestrate the algorithm\n";
    std::cout << "  • Benefits: Enforce invariants, pre/post conditions, logging, etc.\n\n";
    
    std::cout << "GUIDELINE #2: Prefer private virtual functions\n";
    std::cout << "  • Private virtual methods are customization points\n";
    std::cout << "  • Derived classes override to provide specific behavior\n";
    std::cout << "  • Cannot be called directly by client code or derived classes\n";
    std::cout << "  • Clearest separation of interface from implementation\n\n";
    
    std::cout << "GUIDELINE #3: Make virtual protected if derived needs base impl\n";
    std::cout << "  • Use protected when derived classes need to call base version\n";
    std::cout << "  • Common pattern: call base, then add derived-specific logic\n";
    std::cout << "  • Still not part of public interface\n\n";
    
    std::cout << "GUIDELINE #4: Destructor rules\n";
    std::cout << "  • Polymorphic base: public AND virtual destructor\n";
    std::cout << "  • Non-polymorphic base: protected AND non-virtual destructor\n";
    std::cout << "  • Public virtual: allows safe polymorphic deletion\n";
    std::cout << "  • Protected non-virtual: prevents polymorphic deletion\n\n";
    
    std::cout << std::string(70, '-') << "\n\n";
    
    std::cout << "WHEN TO USE NVI:\n";
    std::cout << "  ✓ Need to enforce pre/post conditions\n";
    std::cout << "  ✓ Have invariant algorithm with customizable steps\n";
    std::cout << "  ✓ Want to guarantee certain code always runs\n";
    std::cout << "  ✓ Need separation of interface from implementation\n";
    std::cout << "  ✓ Building frameworks or libraries with hooks\n\n";
    
    std::cout << "WHEN NOT TO USE NVI:\n";
    std::cout << "  ✗ Simple interfaces with no invariants\n";
    std::cout << "  ✗ Pure abstract interfaces (use public virtual)\n";
    std::cout << "  ✗ No pre/post conditions needed\n";
    std::cout << "  ✗ Performance-critical inner loops (tiny overhead)\n";
    std::cout << "  ✗ Simple getter/setter methods\n\n";
    
    std::cout << "COMPARISON:\n";
    std::cout << "  Traditional (public virtual):\n";
    std::cout << "    • Simpler for basic polymorphism\n";
    std::cout << "    • No invariant enforcement\n";
    std::cout << "    • Derived classes have full control\n\n";
    
    std::cout << "  NVI Idiom (public non-virtual, private virtual):\n";
    std::cout << "    • Enforces invariants and contracts\n";
    std::cout << "    • Base controls algorithm flow\n";
    std::cout << "    • Clear interface/implementation separation\n";
    std::cout << "    • Better encapsulation\n\n";
    
    std::cout << "REFERENCE:\n";
    std::cout << "  Herb Sutter's \"Virtuality\" article:\n";
    std::cout << "  http://www.gotw.ca/publications/mill18.htm\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Non-Virtual Interface (NVI) Idiom & Template Method Pattern  ║\n";
    std::cout << "║              Herb Sutter's Virtuality Guidelines               ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    // Section 1: Basic NVI
    basic_nvi::demonstrate();
    
    // Section 2: Pre/post conditions
    preconditions_example::demonstrate();
    
    // Section 3: Protected virtual
    protected_virtual_example::demonstrate();
    
    // Section 4: When NOT to use NVI
    when_not_to_use_nvi::demonstrate();
    
    // Section 5: Real-world example
    http_handler_example::demonstrate();
    
    // Section 6: Destructor rules
    destructor_rules::demonstrate();
    
    // Section 7: Complete summary
    show_summary();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All demonstrations completed!\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    return 0;
}
