#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>

// ===================================================================
// DEPENDENCY INJECTION IN C++ - NO PRIVATE INHERITANCE NEEDED
// ===================================================================

// ===================================================================
// 1. TRADITIONAL DI - PUBLIC INHERITANCE FOR INTERFACES
// ===================================================================

// Abstract interface (use PUBLIC inheritance)
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
};

// Concrete implementations
class ConsoleLogger : public ILogger {  // ✓ PUBLIC inheritance
public:
    void log(const std::string& message) override {
        std::cout << "[CONSOLE] " << message << std::endl;
    }
};

class FileLogger : public ILogger {  // ✓ PUBLIC inheritance
private:
    std::string filename;
    
public:
    FileLogger(const std::string& file) : filename(file) {}
    
    void log(const std::string& message) override {
        std::cout << "[FILE:" << filename << "] " << message << std::endl;
    }
};

// Service that depends on ILogger - uses COMPOSITION, not inheritance
class UserService {
private:
    std::shared_ptr<ILogger> logger;  // ✓ Has-a relationship via composition
    
public:
    // Constructor injection
    UserService(std::shared_ptr<ILogger> log) : logger(std::move(log)) {}
    
    void createUser(const std::string& username) {
        logger->log("Creating user: " + username);
        // User creation logic...
        logger->log("User created successfully: " + username);
    }
    
    void deleteUser(const std::string& username) {
        logger->log("Deleting user: " + username);
        // User deletion logic...
        logger->log("User deleted: " + username);
    }
};

void example_traditional_di() {
    std::cout << "\n=== 1. TRADITIONAL DI - PUBLIC INHERITANCE + COMPOSITION ===" << std::endl;
    std::cout << "Pattern: Interface (public) + Composition (has-a)\n" << std::endl;
    
    // Inject ConsoleLogger
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    UserService service1(consoleLogger);
    service1.createUser("Alice");
    
    std::cout << std::endl;
    
    // Inject FileLogger - same service, different dependency
    auto fileLogger = std::make_shared<FileLogger>("users.log");
    UserService service2(fileLogger);
    service2.createUser("Bob");
    
    std::cout << "\n✓ DEPENDENCY INJECTION USES:" << std::endl;
    std::cout << "   • PUBLIC inheritance for interfaces (ILogger)" << std::endl;
    std::cout << "   • COMPOSITION for dependencies (has-a ILogger)" << std::endl;
    std::cout << "   • NO private inheritance needed!" << std::endl;
}

// ===================================================================
// 2. CONSTRUCTOR INJECTION (MOST COMMON)
// ===================================================================

class IDatabase {
public:
    virtual ~IDatabase() = default;
    virtual void save(const std::string& data) = 0;
    virtual std::string load(const std::string& key) = 0;
};

class MockDatabase : public IDatabase {
public:
    void save(const std::string& data) override {
        std::cout << "  [MockDB] Saving: " << data << std::endl;
    }
    
    std::string load(const std::string& key) override {
        return "mock_data_for_" + key;
    }
};

class PostgresDatabase : public IDatabase {
public:
    void save(const std::string& data) override {
        std::cout << "  [PostgreSQL] Saving to database: " << data << std::endl;
    }
    
    std::string load(const std::string& key) override {
        return "postgres_data_for_" + key;
    }
};

// Repository with multiple dependencies injected via constructor
class OrderRepository {
private:
    std::shared_ptr<IDatabase> database;  // Composition
    std::shared_ptr<ILogger> logger;      // Composition
    
public:
    // Constructor injection of multiple dependencies
    OrderRepository(std::shared_ptr<IDatabase> db, std::shared_ptr<ILogger> log)
        : database(std::move(db)), logger(std::move(log)) {}
    
    void saveOrder(const std::string& orderId) {
        logger->log("Saving order: " + orderId);
        database->save("Order:" + orderId);
        logger->log("Order saved successfully");
    }
    
    void loadOrder(const std::string& orderId) {
        logger->log("Loading order: " + orderId);
        auto data = database->load(orderId);
        std::cout << "  Loaded: " << data << std::endl;
        logger->log("Order loaded successfully");
    }
};

void example_constructor_injection() {
    std::cout << "\n=== 2. CONSTRUCTOR INJECTION (MULTIPLE DEPENDENCIES) ===" << std::endl;
    std::cout << "Pattern: Inject all dependencies via constructor\n" << std::endl;
    
    auto logger = std::make_shared<ConsoleLogger>();
    auto database = std::make_shared<PostgresDatabase>();
    
    OrderRepository repo(database, logger);
    repo.saveOrder("ORD-12345");
    
    std::cout << std::endl;
    repo.loadOrder("ORD-12345");
    
    std::cout << "\n✓ BENEFITS:" << std::endl;
    std::cout << "   • Dependencies are explicit and immutable" << std::endl;
    std::cout << "   • Easy to test (inject mocks)" << std::endl;
    std::cout << "   • No private inheritance - uses composition" << std::endl;
}

// ===================================================================
// 3. INTERFACE INJECTION (STRATEGY PATTERN)
// ===================================================================

class IEmailSender {
public:
    virtual ~IEmailSender() = default;
    virtual void sendEmail(const std::string& to, const std::string& message) = 0;
};

class SmtpEmailSender : public IEmailSender {
public:
    void sendEmail(const std::string& to, const std::string& message) override {
        std::cout << "  [SMTP] Sending email to " << to << ": " << message << std::endl;
    }
};

class MockEmailSender : public IEmailSender {
public:
    void sendEmail(const std::string& to, const std::string& message) override {
        std::cout << "  [MOCK] Would send email to " << to << ": " << message << std::endl;
    }
};

class NotificationService {
private:
    std::shared_ptr<IEmailSender> emailSender;  // Composition
    
public:
    NotificationService(std::shared_ptr<IEmailSender> sender)
        : emailSender(std::move(sender)) {}
    
    // Setter injection (optional, less common)
    void setEmailSender(std::shared_ptr<IEmailSender> sender) {
        emailSender = std::move(sender);
    }
    
    void notifyUser(const std::string& email, const std::string& notification) {
        std::cout << "Sending notification..." << std::endl;
        emailSender->sendEmail(email, notification);
    }
};

void example_interface_injection() {
    std::cout << "\n=== 3. INTERFACE INJECTION (STRATEGY PATTERN) ===" << std::endl;
    std::cout << "Pattern: Inject behavior through interfaces\n" << std::endl;
    
    auto smtpSender = std::make_shared<SmtpEmailSender>();
    NotificationService service(smtpSender);
    
    service.notifyUser("user@example.com", "Your order has shipped!");
    
    std::cout << "\nSwitching to mock sender (setter injection):" << std::endl;
    auto mockSender = std::make_shared<MockEmailSender>();
    service.setEmailSender(mockSender);
    service.notifyUser("user@example.com", "Your order is delivered!");
    
    std::cout << "\n✓ KEY POINTS:" << std::endl;
    std::cout << "   • Strategy pattern uses PUBLIC inheritance" << std::endl;
    std::cout << "   • Can switch implementations at runtime" << std::endl;
    std::cout << "   • No private inheritance involved" << std::endl;
}

// ===================================================================
// 4. TEMPLATE-BASED DI (COMPILE-TIME INJECTION)
// ===================================================================

// No interface needed - duck typing via templates
class ConsoleOutput {
public:
    void write(const std::string& msg) {
        std::cout << "[Console] " << msg << std::endl;
    }
};

class FileOutput {
public:
    void write(const std::string& msg) {
        std::cout << "[File] " << msg << std::endl;
    }
};

// Template-based dependency injection (no inheritance at all!)
template<typename TOutput>
class MessageProcessor {
private:
    TOutput output;  // Composition with template type
    
public:
    MessageProcessor(TOutput out) : output(std::move(out)) {}
    
    void process(const std::string& message) {
        std::cout << "Processing message: " << message << std::endl;
        output.write("Processed: " + message);
    }
};

void example_template_di() {
    std::cout << "\n=== 4. TEMPLATE-BASED DI (COMPILE-TIME) ===" << std::endl;
    std::cout << "Pattern: No inheritance, no interfaces - pure templates\n" << std::endl;
    
    ConsoleOutput console;
    MessageProcessor<ConsoleOutput> processor1(console);
    processor1.process("Hello World");
    
    std::cout << std::endl;
    
    FileOutput file;
    MessageProcessor<FileOutput> processor2(file);
    processor2.process("Template DI");
    
    std::cout << "\n✓ ADVANTAGES:" << std::endl;
    std::cout << "   • Zero runtime overhead (compile-time binding)" << std::endl;
    std::cout << "   • No virtual functions needed" << std::endl;
    std::cout << "   • No inheritance (public OR private) needed!" << std::endl;
    std::cout << "   • Duck typing - if it has write(), it works" << std::endl;
}

// ===================================================================
// 5. DI CONTAINER / SERVICE LOCATOR PATTERN
// ===================================================================

class DIContainer {
private:
    std::shared_ptr<ILogger> logger;
    std::shared_ptr<IDatabase> database;
    std::shared_ptr<IEmailSender> emailSender;
    
public:
    // Register dependencies
    void registerLogger(std::shared_ptr<ILogger> log) {
        logger = std::move(log);
    }
    
    void registerDatabase(std::shared_ptr<IDatabase> db) {
        database = std::move(db);
    }
    
    void registerEmailSender(std::shared_ptr<IEmailSender> sender) {
        emailSender = std::move(sender);
    }
    
    // Resolve dependencies
    std::shared_ptr<ILogger> getLogger() { return logger; }
    std::shared_ptr<IDatabase> getDatabase() { return database; }
    std::shared_ptr<IEmailSender> getEmailSender() { return emailSender; }
    
    // Factory method to create services with auto-injected dependencies
    std::shared_ptr<OrderRepository> createOrderRepository() {
        return std::make_shared<OrderRepository>(database, logger);
    }
};

void example_di_container() {
    std::cout << "\n=== 5. DI CONTAINER / SERVICE LOCATOR ===" << std::endl;
    std::cout << "Pattern: Central registry for dependency management\n" << std::endl;
    
    DIContainer container;
    
    // Setup (composition root)
    container.registerLogger(std::make_shared<ConsoleLogger>());
    container.registerDatabase(std::make_shared<MockDatabase>());
    container.registerEmailSender(std::make_shared<MockEmailSender>());
    
    // Resolve dependencies
    auto repo = container.createOrderRepository();
    repo->saveOrder("ORD-99999");
    
    auto emailSender = container.getEmailSender();
    emailSender->sendEmail("admin@example.com", "System started");
    
    std::cout << "\n✓ DI CONTAINER USES:" << std::endl;
    std::cout << "   • Stores dependencies as members (composition)" << std::endl;
    std::cout << "   • Public inheritance for interfaces only" << std::endl;
    std::cout << "   • No private inheritance needed" << std::endl;
}

// ===================================================================
// 6. FUNCTIONAL DI (C++11/14 STYLE)
// ===================================================================

class PaymentService {
private:
    std::function<void(const std::string&)> logFunc;
    std::function<bool(double)> validateFunc;
    
public:
    // Inject functions instead of objects
    PaymentService(
        std::function<void(const std::string&)> logger,
        std::function<bool(double)> validator)
        : logFunc(std::move(logger))
        , validateFunc(std::move(validator)) {}
    
    void processPayment(double amount) {
        logFunc("Processing payment of $" + std::to_string(amount));
        
        if (validateFunc(amount)) {
            logFunc("Payment validated and processed");
        } else {
            logFunc("Payment validation failed");
        }
    }
};

void example_functional_di() {
    std::cout << "\n=== 6. FUNCTIONAL DI (INJECT FUNCTIONS) ===" << std::endl;
    std::cout << "Pattern: Inject std::function instead of interfaces\n" << std::endl;
    
    // Define behaviors as lambdas
    auto logger = [](const std::string& msg) {
        std::cout << "  [Payment Log] " << msg << std::endl;
    };
    
    auto validator = [](double amount) {
        return amount > 0 && amount < 10000;  // Simple validation
    };
    
    PaymentService service(logger, validator);
    service.processPayment(150.50);
    
    std::cout << std::endl;
    service.processPayment(-50.0);  // Invalid
    
    std::cout << "\n✓ FUNCTIONAL DI:" << std::endl;
    std::cout << "   • No interfaces needed" << std::endl;
    std::cout << "   • No inheritance at all (public or private)" << std::endl;
    std::cout << "   • Lightweight and flexible" << std::endl;
    std::cout << "   • Uses std::function and lambdas" << std::endl;
}

// ===================================================================
// 7. WHY NOT PRIVATE INHERITANCE FOR DI?
// ===================================================================

// ❌ WRONG: Using private inheritance for DI (anti-pattern)
class WrongServiceWithPrivateInheritance : private ILogger {
private:
    // This makes ILogger methods private - defeats the purpose of DI!
    
public:
    void doSomething() {
        log("This is confusing!");  // Can only use internally
    }
    
    // ❌ Cannot inject different logger implementations
    // ❌ Cannot swap loggers at runtime
    // ❌ Tightly coupled to ILogger
};

// ✓ CORRECT: Using composition for DI
class CorrectServiceWithComposition {
private:
    std::shared_ptr<ILogger> logger;  // Injected dependency
    
public:
    CorrectServiceWithComposition(std::shared_ptr<ILogger> log)
        : logger(std::move(log)) {}
    
    void doSomething() {
        logger->log("This is the right way!");
    }
    
    // ✓ Can inject any ILogger implementation
    // ✓ Can swap loggers at runtime
    // ✓ Loosely coupled
};

void example_why_not_private() {
    std::cout << "\n=== 7. WHY NOT PRIVATE INHERITANCE FOR DI? ===" << std::endl;
    
    std::cout << "\n❌ PRIVATE INHERITANCE FOR DI IS WRONG BECAUSE:" << std::endl;
    std::cout << "   • Cannot inject different implementations" << std::endl;
    std::cout << "   • Tightly coupled to specific base class" << std::endl;
    std::cout << "   • Cannot swap dependencies at runtime" << std::endl;
    std::cout << "   • Violates Dependency Inversion Principle" << std::endl;
    std::cout << "   • Makes testing difficult (can't inject mocks)" << std::endl;
    
    std::cout << "\n✓ DI REQUIRES LOOSE COUPLING:" << std::endl;
    std::cout << "   • Use interfaces (abstract base classes)" << std::endl;
    std::cout << "   • Use PUBLIC inheritance for interfaces" << std::endl;
    std::cout << "   • Use COMPOSITION to hold dependencies" << std::endl;
    std::cout << "   • Inject dependencies via constructor/setter" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  DEPENDENCY INJECTION IN C++ - NO PRIVATE INHERITANCE!" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    example_traditional_di();
    example_constructor_injection();
    example_interface_injection();
    example_template_di();
    example_di_container();
    example_functional_di();
    example_why_not_private();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  DEPENDENCY INJECTION SUMMARY" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n🎯 DI PATTERNS IN C++ (NO PRIVATE INHERITANCE):" << std::endl;
    std::cout << "\n1. INTERFACE-BASED DI (MOST COMMON):" << std::endl;
    std::cout << "   • Define abstract interfaces (ILogger, IDatabase, etc.)" << std::endl;
    std::cout << "   • Implementations use PUBLIC inheritance" << std::endl;
    std::cout << "   • Services use COMPOSITION (has-a relationship)" << std::endl;
    std::cout << "   • Inject via constructor or setter" << std::endl;
    
    std::cout << "\n2. TEMPLATE-BASED DI:" << std::endl;
    std::cout << "   • No inheritance at all!" << std::endl;
    std::cout << "   • Compile-time polymorphism via templates" << std::endl;
    std::cout << "   • Duck typing - if it quacks like a duck..." << std::endl;
    std::cout << "   • Zero runtime overhead" << std::endl;
    
    std::cout << "\n3. FUNCTIONAL DI:" << std::endl;
    std::cout << "   • Inject std::function instead of interfaces" << std::endl;
    std::cout << "   • No inheritance needed" << std::endl;
    std::cout << "   • Lightweight and flexible" << std::endl;
    std::cout << "   • Great for simple dependencies" << std::endl;
    
    std::cout << "\n4. DI CONTAINER:" << std::endl;
    std::cout << "   • Central registry for dependencies" << std::endl;
    std::cout << "   • Auto-wiring of complex object graphs" << std::endl;
    std::cout << "   • Uses composition internally" << std::endl;
    std::cout << "   • Similar to Spring (Java) or Autofac (.NET)" << std::endl;
    
    std::cout << "\n❌ NEVER USE PRIVATE INHERITANCE FOR DI:" << std::endl;
    std::cout << "   • Private inheritance = tight coupling" << std::endl;
    std::cout << "   • Cannot inject different implementations" << std::endl;
    std::cout << "   • Cannot swap at runtime" << std::endl;
    std::cout << "   • Defeats the entire purpose of DI!" << std::endl;
    
    std::cout << "\n✓ CORRECT DI APPROACH:" << std::endl;
    std::cout << "   • PUBLIC inheritance for interfaces (polymorphism)" << std::endl;
    std::cout << "   • COMPOSITION for dependencies (has-a)" << std::endl;
    std::cout << "   • Constructor/setter injection" << std::endl;
    std::cout << "   • Inversion of Control (IoC)" << std::endl;
    
    std::cout << "\n💡 KEY PRINCIPLE:" << std::endl;
    std::cout << "   \"Depend on abstractions, not concretions\"" << std::endl;
    std::cout << "   - Dependency Inversion Principle (SOLID)" << std::endl;
    
    std::cout << "\n🔑 WHEN TO USE EACH:" << std::endl;
    std::cout << "   • Interface-based DI: Runtime polymorphism, complex systems" << std::endl;
    std::cout << "   • Template-based DI: Performance-critical code, simple deps" << std::endl;
    std::cout << "   • Functional DI: Simple behaviors, lambdas" << std::endl;
    std::cout << "   • DI Container: Large applications with many dependencies" << std::endl;
    std::cout << "   • Private inheritance: NEVER for DI!" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
