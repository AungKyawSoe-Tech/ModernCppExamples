// ===================================================================
// SOLID PRINCIPLES IN C++
// ===================================================================
// A comprehensive demonstration of the five SOLID design principles
// for object-oriented programming in Modern C++
//
// S - Single Responsibility Principle
// O - Open-Closed Principle
// L - Liskov Substitution Principle
// I - Interface Segregation Principle
// D - Dependency Inversion Principle
//
// Build: g++ -std=c++20 -Wall -Wextra -O2 -o SOLIDPrinciples SOLIDPrinciples.cpp
// ===================================================================

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <cmath>
#include <stdexcept>

// ===================================================================
// PRINCIPLE 1: SINGLE RESPONSIBILITY PRINCIPLE (SRP)
// ===================================================================
// A class should have only ONE reason to change
// Each class should do ONE thing and do it well

namespace single_responsibility {

// ❌ BAD: Class with multiple responsibilities
class BadEmployee {
private:
    std::string name_;
    double salary_;
    
public:
    BadEmployee(const std::string& name, double salary) 
        : name_(name), salary_(salary) {}
    
    // Responsibility 1: Employee data management
    std::string getName() const { return name_; }
    double getSalary() const { return salary_; }
    
    // Responsibility 2: Salary calculation (WRONG!)
    double calculateTax() const {
        return salary_ * 0.25;
    }
    
    // Responsibility 3: Database operations (WRONG!)
    void saveToDatabase() {
        std::cout << "   [BAD] Saving " << name_ << " to database...\n";
        // Database logic here...
    }
    
    // Responsibility 4: Reporting (WRONG!)
    void generatePayslip() {
        std::cout << "   [BAD] Generating payslip for " << name_ << "\n";
        // Report generation logic...
    }
};

// ✅ GOOD: Separate classes for separate responsibilities
class Employee {
private:
    std::string name_;
    double salary_;
    
public:
    Employee(const std::string& name, double salary) 
        : name_(name), salary_(salary) {}
    
    std::string getName() const { return name_; }
    double getSalary() const { return salary_; }
};

// Responsibility 2: Tax calculation
class TaxCalculator {
public:
    double calculateTax(const Employee& emp) const {
        return emp.getSalary() * 0.25;
    }
};

// Responsibility 3: Database operations
class EmployeeRepository {
public:
    void save(const Employee& emp) {
        std::cout << "   [GOOD] Saving " << emp.getName() << " to database...\n";
        // Database logic here...
    }
};

// Responsibility 4: Reporting
class PayslipGenerator {
public:
    void generate(const Employee& emp) {
        std::cout << "   [GOOD] Generating payslip for " << emp.getName() << "\n";
        // Report generation logic...
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "PRINCIPLE 1: SINGLE RESPONSIBILITY PRINCIPLE (SRP)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 DEFINITION:\n";
    std::cout << "   A class should have only ONE reason to change.\n";
    std::cout << "   Each class should do ONE thing and do it well.\n\n";
    
    std::cout << "❌ BAD Example (Multiple Responsibilities):\n";
    BadEmployee badEmp("John Doe", 50000);
    badEmp.saveToDatabase();
    badEmp.generatePayslip();
    std::cout << "   Problem: Employee class handles data, tax, DB, and reports!\n\n";
    
    std::cout << "✅ GOOD Example (Single Responsibility):\n";
    Employee goodEmp("Jane Smith", 60000);
    TaxCalculator taxCalc;
    EmployeeRepository repo;
    PayslipGenerator payslip;
    
    repo.save(goodEmp);
    payslip.generate(goodEmp);
    std::cout << "   Tax: $" << taxCalc.calculateTax(goodEmp) << "\n\n";
    
    std::cout << "💡 BENEFITS:\n";
    std::cout << "   • Easier to understand and maintain\n";
    std::cout << "   • Changes to tax logic don't affect database code\n";
    std::cout << "   • Classes are more reusable and testable\n";
    std::cout << "   • Better separation of concerns\n";
}

} // namespace single_responsibility

// ===================================================================
// PRINCIPLE 2: OPEN-CLOSED PRINCIPLE (OCP)
// ===================================================================
// Open for extension, closed for modification
// You should be able to add new functionality without changing existing code

namespace open_closed {

// ❌ BAD: Need to modify existing code for each new shape
class BadShapeCalculator {
public:
    double calculateArea(const std::string& shapeType, double dimension) {
        if (shapeType == "circle") {
            return 3.14159 * dimension * dimension;
        } else if (shapeType == "square") {
            return dimension * dimension;
        } 
        // Need to modify this function for each new shape! (BAD)
        return 0.0;
    }
};

// ✅ GOOD: Use polymorphism - open for extension, closed for modification
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};

class Circle : public Shape {
private:
    double radius_;
    
public:
    explicit Circle(double radius) : radius_(radius) {}
    
    double area() const override {
        return 3.14159 * radius_ * radius_;
    }
    
    std::string name() const override { return "Circle"; }
};

class Square : public Shape {
private:
    double side_;
    
public:
    explicit Square(double side) : side_(side) {}
    
    double area() const override {
        return side_ * side_;
    }
    
    std::string name() const override { return "Square"; }
};

class Triangle : public Shape {
private:
    double base_;
    double height_;
    
public:
    Triangle(double base, double height) : base_(base), height_(height) {}
    
    double area() const override {
        return 0.5 * base_ * height_;
    }
    
    std::string name() const override { return "Triangle"; }
};

// This class is CLOSED for modification, but OPEN for extension
class AreaCalculator {
public:
    double totalArea(const std::vector<std::unique_ptr<Shape>>& shapes) const {
        double total = 0.0;
        for (const auto& shape : shapes) {
            total += shape->area();
        }
        return total;
    }
    
    void printAreas(const std::vector<std::unique_ptr<Shape>>& shapes) const {
        for (const auto& shape : shapes) {
            std::cout << "   " << shape->name() << " area: " 
                      << shape->area() << "\n";
        }
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "PRINCIPLE 2: OPEN-CLOSED PRINCIPLE (OCP)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 DEFINITION:\n";
    std::cout << "   Software entities should be OPEN for extension,\n";
    std::cout << "   but CLOSED for modification.\n\n";
    
    std::cout << "❌ BAD Example:\n";
    BadShapeCalculator badCalc;
    std::cout << "   Circle area: " << badCalc.calculateArea("circle", 5.0) << "\n";
    std::cout << "   Problem: Must modify calculateArea() for each new shape!\n\n";
    
    std::cout << "✅ GOOD Example (Using Polymorphism):\n";
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Square>(4.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 6.0));
    
    AreaCalculator calc;
    calc.printAreas(shapes);
    std::cout << "   Total area: " << calc.totalArea(shapes) << "\n\n";
    
    std::cout << "💡 BENEFITS:\n";
    std::cout << "   • Add new shapes without modifying AreaCalculator\n";
    std::cout << "   • Existing code remains stable and tested\n";
    std::cout << "   • Reduces risk of breaking existing functionality\n";
    std::cout << "   • Promotes use of interfaces and polymorphism\n";
}

} // namespace open_closed

// ===================================================================
// PRINCIPLE 3: LISKOV SUBSTITUTION PRINCIPLE (LSP)
// ===================================================================
// Objects of a superclass should be replaceable with objects of a subclass
// without breaking the application

namespace liskov_substitution {

// ❌ BAD: Violates LSP - Square changes Rectangle's behavior
class BadRectangle {
protected:
    int width_;
    int height_;
    
public:
    virtual ~BadRectangle() = default;  // Virtual destructor for polymorphic class
    virtual void setWidth(int w) { width_ = w; }
    virtual void setHeight(int h) { height_ = h; }
    
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    
    virtual int area() const { return width_ * height_; }
};

class BadSquare : public BadRectangle {
public:
    // Violates LSP: Changes the behavior!
    void setWidth(int w) override {
        width_ = w;
        height_ = w;  // Square forces equal sides
    }
    
    void setHeight(int h) override {
        width_ = h;
        height_ = h;  // Square forces equal sides
    }
};

// ✅ GOOD: Use composition instead of inheritance for Square
class Shape {
public:
    virtual ~Shape() = default;
    virtual int area() const = 0;
    virtual std::string type() const = 0;
};

class Rectangle : public Shape {
private:
    int width_;
    int height_;
    
public:
    Rectangle(int w, int h) : width_(w), height_(h) {}
    
    void setWidth(int w) { width_ = w; }
    void setHeight(int h) { height_ = h; }
    
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    
    int area() const override { return width_ * height_; }
    std::string type() const override { return "Rectangle"; }
};

class Square : public Shape {
private:
    int side_;
    
public:
    explicit Square(int side) : side_(side) {}
    
    void setSide(int s) { side_ = s; }
    int getSide() const { return side_; }
    
    int area() const override { return side_ * side_; }
    std::string type() const override { return "Square"; }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "PRINCIPLE 3: LISKOV SUBSTITUTION PRINCIPLE (LSP)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 DEFINITION:\n";
    std::cout << "   Objects of a superclass should be replaceable with objects\n";
    std::cout << "   of a subclass without breaking the application.\n\n";
    
    std::cout << "❌ BAD Example (LSP Violation):\n";
    BadRectangle* rect = new BadSquare();
    rect->setWidth(5);
    rect->setHeight(4);
    std::cout << "   Expected area: 20 (5x4)\n";
    std::cout << "   Actual area: " << rect->area() << " (4x4)\n";
    std::cout << "   Problem: Square changed Rectangle's behavior!\n";
    delete rect;
    std::cout << "\n";
    
    std::cout << "✅ GOOD Example (LSP Compliant):\n";
    Rectangle goodRect(5, 4);
    std::cout << "   Rectangle (5x4) area: " << goodRect.area() << "\n";
    
    Square goodSquare(4);
    std::cout << "   Square (4x4) area: " << goodSquare.area() << "\n\n";
    
    std::cout << "💡 BENEFITS:\n";
    std::cout << "   • Substitutability is preserved\n";
    std::cout << "   • No unexpected behavior when using derived classes\n";
    std::cout << "   • Polymorphism works correctly\n";
    std::cout << "   • \"IS-A\" relationship is properly maintained\n";
}

} // namespace liskov_substitution

// ===================================================================
// PRINCIPLE 4: INTERFACE SEGREGATION PRINCIPLE (ISP)
// ===================================================================
// Clients should not be forced to depend on interfaces they don't use
// Many specific interfaces are better than one general-purpose interface

namespace interface_segregation {

// ❌ BAD: Fat interface forces implementations to provide unused methods
class BadWorker {
public:
    virtual ~BadWorker() = default;
    virtual void work() = 0;
    virtual void eat() = 0;
    virtual void sleep() = 0;
};

class BadHumanWorker : public BadWorker {
public:
    void work() override {
        std::cout << "   [Human] Working...\n";
    }
    
    void eat() override {
        std::cout << "   [Human] Eating lunch...\n";
    }
    
    void sleep() override {
        std::cout << "   [Human] Sleeping...\n";
    }
};

class BadRobotWorker : public BadWorker {
public:
    void work() override {
        std::cout << "   [Robot] Working 24/7...\n";
    }
    
    // Robots don't eat or sleep! Forced to implement unused methods
    void eat() override {
        // Empty or throw exception (both are bad)
        std::cout << "   [Robot] ERROR: Robots don't eat!\n";
    }
    
    void sleep() override {
        // Empty or throw exception (both are bad)
        std::cout << "   [Robot] ERROR: Robots don't sleep!\n";
    }
};

// ✅ GOOD: Segregated interfaces - clients use only what they need
class Workable {
public:
    virtual ~Workable() = default;
    virtual void work() = 0;
};

class Eatable {
public:
    virtual ~Eatable() = default;
    virtual void eat() = 0;
};

class Sleepable {
public:
    virtual ~Sleepable() = default;
    virtual void sleep() = 0;
};

// Human implements all interfaces
class HumanWorker : public Workable, public Eatable, public Sleepable {
public:
    void work() override {
        std::cout << "   [Human] Working...\n";
    }
    
    void eat() override {
        std::cout << "   [Human] Eating lunch...\n";
    }
    
    void sleep() override {
        std::cout << "   [Human] Sleeping...\n";
    }
};

// Robot only implements what it needs
class RobotWorker : public Workable {
public:
    void work() override {
        std::cout << "   [Robot] Working 24/7...\n";
    }
    // No eat() or sleep() - not forced to implement them!
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "PRINCIPLE 4: INTERFACE SEGREGATION PRINCIPLE (ISP)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 DEFINITION:\n";
    std::cout << "   Clients should not be forced to depend on interfaces\n";
    std::cout << "   they don't use. Prefer many specific interfaces over\n";
    std::cout << "   one general-purpose interface.\n\n";
    
    std::cout << "❌ BAD Example (Fat Interface):\n";
    BadRobotWorker badRobot;
    badRobot.work();
    badRobot.eat();   // Forced to implement, but doesn't make sense!
    badRobot.sleep(); // Forced to implement, but doesn't make sense!
    std::cout << "\n";
    
    std::cout << "✅ GOOD Example (Segregated Interfaces):\n";
    HumanWorker human;
    human.work();
    human.eat();
    human.sleep();
    std::cout << "\n";
    
    RobotWorker robot;
    robot.work();
    std::cout << "   [Robot] No eat() or sleep() methods - clean interface!\n\n";
    
    std::cout << "💡 BENEFITS:\n";
    std::cout << "   • Classes implement only what they need\n";
    std::cout << "   • No dummy implementations or exceptions\n";
    std::cout << "   • Better decoupling and flexibility\n";
    std::cout << "   • Easier to understand and maintain\n";
}

} // namespace interface_segregation

// ===================================================================
// PRINCIPLE 5: DEPENDENCY INVERSION PRINCIPLE (DIP)
// ===================================================================
// High-level modules should not depend on low-level modules
// Both should depend on abstractions (interfaces)
// Abstractions should not depend on details, details should depend on abstractions

namespace dependency_inversion {

// ❌ BAD: High-level class depends on low-level concrete classes
class BadMySQLDatabase {
public:
    void connect() {
        std::cout << "   [BAD] Connecting to MySQL...\n";
    }
    
    void saveData(const std::string& data) {
        std::cout << "   [BAD] Saving to MySQL: " << data << "\n";
    }
};

class BadUserService {
private:
    BadMySQLDatabase database_;  // Tightly coupled to MySQL!
    
public:
    void saveUser(const std::string& user) {
        database_.connect();
        database_.saveData(user);
        // If we want to switch to PostgreSQL, we must modify this class!
    }
};

// ✅ GOOD: Depend on abstraction, not concrete implementation
class IDatabase {
public:
    virtual ~IDatabase() = default;
    virtual void connect() = 0;
    virtual void saveData(const std::string& data) = 0;
};

// Low-level modules implement the interface
class MySQLDatabase : public IDatabase {
public:
    void connect() override {
        std::cout << "   [GOOD] Connecting to MySQL...\n";
    }
    
    void saveData(const std::string& data) override {
        std::cout << "   [GOOD] Saving to MySQL: " << data << "\n";
    }
};

class PostgreSQLDatabase : public IDatabase {
public:
    void connect() override {
        std::cout << "   [GOOD] Connecting to PostgreSQL...\n";
    }
    
    void saveData(const std::string& data) override {
        std::cout << "   [GOOD] Saving to PostgreSQL: " << data << "\n";
    }
};

class MongoDBDatabase : public IDatabase {
public:
    void connect() override {
        std::cout << "   [GOOD] Connecting to MongoDB...\n";
    }
    
    void saveData(const std::string& data) override {
        std::cout << "   [GOOD] Saving to MongoDB: " << data << "\n";
    }
};

// High-level module depends on abstraction (IDatabase), not concrete class
class UserService {
private:
    IDatabase& database_;  // Depends on interface, not implementation!
    
public:
    explicit UserService(IDatabase& db) : database_(db) {}
    
    void saveUser(const std::string& user) {
        database_.connect();
        database_.saveData(user);
        // Can switch database without modifying this class!
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "PRINCIPLE 5: DEPENDENCY INVERSION PRINCIPLE (DIP)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 DEFINITION:\n";
    std::cout << "   1. High-level modules should not depend on low-level modules.\n";
    std::cout << "      Both should depend on abstractions.\n";
    std::cout << "   2. Abstractions should not depend on details.\n";
    std::cout << "      Details should depend on abstractions.\n\n";
    
    std::cout << "❌ BAD Example (Tight Coupling):\n";
    BadUserService badService;
    badService.saveUser("John Doe");
    std::cout << "   Problem: UserService is tightly coupled to MySQL!\n\n";
    
    std::cout << "✅ GOOD Example (Dependency Injection):\n";
    
    MySQLDatabase mysql;
    UserService service1(mysql);
    service1.saveUser("Jane Smith");
    std::cout << "\n";
    
    PostgreSQLDatabase postgres;
    UserService service2(postgres);
    service2.saveUser("Bob Johnson");
    std::cout << "\n";
    
    MongoDBDatabase mongo;
    UserService service3(mongo);
    service3.saveUser("Alice Williams");
    std::cout << "\n";
    
    std::cout << "💡 BENEFITS:\n";
    std::cout << "   • Loose coupling between modules\n";
    std::cout << "   • Easy to switch implementations\n";
    std::cout << "   • Better testability (can inject mocks)\n";
    std::cout << "   • Follows \"Program to an interface, not an implementation\"\n";
}

} // namespace dependency_inversion

// ===================================================================
// MAIN - Demonstrate All SOLID Principles
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                      SOLID PRINCIPLES IN C++                     ║\n";
    std::cout << "║              Object-Oriented Design Best Practices               ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    try {
        // S - Single Responsibility Principle
        single_responsibility::demonstrate();
        
        // O - Open-Closed Principle
        open_closed::demonstrate();
        
        // L - Liskov Substitution Principle
        liskov_substitution::demonstrate();
        
        // I - Interface Segregation Principle
        interface_segregation::demonstrate();
        
        // D - Dependency Inversion Principle
        dependency_inversion::demonstrate();
        
        // Summary
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "SUMMARY: SOLID PRINCIPLES\n";
        std::cout << std::string(70, '=') << "\n\n";
        
        std::cout << "S - Single Responsibility Principle:\n";
        std::cout << "    One class, one job, one reason to change\n\n";
        
        std::cout << "O - Open-Closed Principle:\n";
        std::cout << "    Open for extension, closed for modification\n\n";
        
        std::cout << "L - Liskov Substitution Principle:\n";
        std::cout << "    Derived classes must be substitutable for base classes\n\n";
        
        std::cout << "I - Interface Segregation Principle:\n";
        std::cout << "    Many specific interfaces > one general interface\n\n";
        
        std::cout << "D - Dependency Inversion Principle:\n";
        std::cout << "    Depend on abstractions, not concrete implementations\n\n";
        
        std::cout << "💡 APPLYING SOLID:\n";
        std::cout << "   • Leads to more maintainable code\n";
        std::cout << "   • Reduces coupling between components\n";
        std::cout << "   • Makes code more testable\n";
        std::cout << "   • Improves code reusability\n";
        std::cout << "   • Easier to understand and modify\n\n";
        
        std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║               ALL SOLID PRINCIPLES DEMONSTRATED!                 ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
