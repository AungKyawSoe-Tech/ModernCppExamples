#include <iostream>
#include <memory>
#include <vector>
#include <string>

// ===================================================================
// OBJECT SLICING AND SMART POINTERS (C++11/14)
// ===================================================================

// Base class
class Shape {
protected:
    std::string name;
    
public:
    Shape(const std::string& n) : name(n) {
        std::cout << "Shape constructor: " << name << std::endl;
    }
    
    virtual ~Shape() {
        std::cout << "Shape destructor: " << name << std::endl;
    }
    
    virtual void draw() const {
        std::cout << "Drawing Shape: " << name << std::endl;
    }
    
    virtual double area() const {
        return 0.0;
    }
    
    virtual void info() const {
        std::cout << "Shape: " << name << std::endl;
    }
};

// Derived class: Circle
class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(const std::string& n, double r) : Shape(n), radius(r) {
        std::cout << "Circle constructor: " << name << " (radius=" << radius << ")" << std::endl;
    }
    
    ~Circle() override {
        std::cout << "Circle destructor: " << name << std::endl;
    }
    
    void draw() const override {
        std::cout << "Drawing Circle: " << name << " with radius " << radius << std::endl;
    }
    
    double area() const override {
        return 3.14159 * radius * radius;
    }
    
    void info() const override {
        std::cout << "Circle: " << name << ", radius=" << radius << ", area=" << area() << std::endl;
    }
};

// Derived class: Rectangle
class Rectangle : public Shape {
private:
    double width;
    double height;
    
public:
    Rectangle(const std::string& n, double w, double h) 
        : Shape(n), width(w), height(h) {
        std::cout << "Rectangle constructor: " << name 
                  << " (width=" << width << ", height=" << height << ")" << std::endl;
    }
    
    ~Rectangle() override {
        std::cout << "Rectangle destructor: " << name << std::endl;
    }
    
    void draw() const override {
        std::cout << "Drawing Rectangle: " << name 
                  << " (" << width << "x" << height << ")" << std::endl;
    }
    
    double area() const override {
        return width * height;
    }
    
    void info() const override {
        std::cout << "Rectangle: " << name << ", " << width << "x" << height 
                  << ", area=" << area() << std::endl;
    }
};

// ===================================================================
// 1. CLASSIC OBJECT SLICING (WITHOUT SMART POINTERS)
// ===================================================================

void example_classic_slicing() {
    std::cout << "\n=== 1. CLASSIC OBJECT SLICING ===" << std::endl;
    std::cout << "Problem: Assigning derived object to base object by value\n" << std::endl;
    
    Circle circle("MyCircle", 5.0);
    Shape shape = circle;  // ❌ SLICING! Circle parts are lost
    
    std::cout << "\nCalling draw() on sliced object:" << std::endl;
    shape.draw();  // Calls Shape::draw(), not Circle::draw()!
    
    std::cout << "\nArea of sliced object: " << shape.area() << std::endl;  // Returns 0.0, not circle area!
    
    std::cout << "\n❌ The Circle-specific data (radius) was sliced off!" << std::endl;
}

// ===================================================================
// 2. OBJECT SLICING WITH SHARED_PTR - WRONG WAY
// ===================================================================

void example_shared_ptr_slicing_wrong() {
    std::cout << "\n=== 2. OBJECT SLICING WITH SHARED_PTR (WRONG) ===" << std::endl;
    std::cout << "Problem: Creating shared_ptr to base from derived object by value\n" << std::endl;
    
    Circle circle("SlicedCircle", 7.0);
    
    // ❌ WRONG: This creates a shared_ptr<Shape> pointing to a SLICED copy
    std::shared_ptr<Shape> shape_ptr = std::make_shared<Shape>(circle);
    
    std::cout << "\nCalling draw() through shared_ptr:" << std::endl;
    shape_ptr->draw();  // Calls Shape::draw(), not Circle::draw()!
    
    std::cout << "\nArea: " << shape_ptr->area() << std::endl;  // Returns 0.0!
    
    std::cout << "\n❌ Object was sliced when passed by value to make_shared<Shape>!" << std::endl;
}

// ===================================================================
// 3. CONTAINER OF SHARED_PTR - WRONG WAY (SLICING)
// ===================================================================

void example_container_slicing_wrong() {
    std::cout << "\n=== 3. CONTAINER WITH SLICING (WRONG) ===" << std::endl;
    std::cout << "Problem: Storing objects by value in container\n" << std::endl;
    
    std::vector<Shape> shapes;  // ❌ WRONG: Stores Shape objects by value
    
    shapes.push_back(Circle("Circle1", 3.0));      // Sliced!
    shapes.push_back(Rectangle("Rect1", 4.0, 5.0)); // Sliced!
    
    std::cout << "\nDrawing shapes from container:" << std::endl;
    for (const auto& shape : shapes) {
        shape.draw();  // Always calls Shape::draw()!
        std::cout << "  Area: " << shape.area() << std::endl;  // Always returns 0.0!
    }
    
    std::cout << "\n❌ All derived class data was sliced off!" << std::endl;
}

// ===================================================================
// 4. COPYING SMART POINTERS - POTENTIAL SLICING
// ===================================================================

void process_shape_wrong(std::shared_ptr<Shape> shape) {
    // ❌ If we copy the object instead of using the pointer
    Shape copy = *shape;  // SLICING!
    copy.draw();  // Calls Shape::draw(), not the derived version
}

void example_copying_smart_ptr_wrong() {
    std::cout << "\n=== 4. COPYING OBJECT FROM SMART_PTR (WRONG) ===" << std::endl;
    std::cout << "Problem: Dereferencing smart_ptr and copying by value\n" << std::endl;
    
    auto circle = std::make_shared<Circle>("PointerCircle", 6.0);
    
    std::cout << "\nOriginal (via pointer):" << std::endl;
    circle->draw();
    
    std::cout << "\nCopied value from pointer:" << std::endl;
    process_shape_wrong(circle);  // Slicing happens inside function
    
    std::cout << "\n❌ Dereferencing and copying caused slicing!" << std::endl;
}

// ===================================================================
// 5. CORRECT WAY: SHARED_PTR TO DERIVED CLASS
// ===================================================================

void example_shared_ptr_correct() {
    std::cout << "\n=== 5. CORRECT: SHARED_PTR TO DERIVED CLASS ===" << std::endl;
    std::cout << "Solution: Create shared_ptr directly to derived class\n" << std::endl;
    
    // ✓ CORRECT: Create shared_ptr<Circle> directly
    std::shared_ptr<Circle> circle = std::make_shared<Circle>("CorrectCircle", 8.0);
    
    // ✓ Upcast to base class pointer (no slicing!)
    std::shared_ptr<Shape> shape_ptr = circle;
    
    std::cout << "\nCalling draw() through base pointer:" << std::endl;
    shape_ptr->draw();  // ✓ Calls Circle::draw() via polymorphism
    
    std::cout << "\nArea: " << shape_ptr->area() << std::endl;  // ✓ Returns actual circle area
    
    std::cout << "\n✓ Polymorphism works correctly - no slicing!" << std::endl;
}

// ===================================================================
// 6. CORRECT WAY: CONTAINER OF SMART POINTERS
// ===================================================================

void example_container_correct() {
    std::cout << "\n=== 6. CORRECT: CONTAINER OF SMART_PTR ===" << std::endl;
    std::cout << "Solution: Store pointers to base class, not objects\n" << std::endl;
    
    // ✓ CORRECT: Vector of pointers to Shape
    std::vector<std::shared_ptr<Shape>> shapes;
    
    shapes.push_back(std::make_shared<Circle>("Circle2", 4.0));
    shapes.push_back(std::make_shared<Rectangle>("Rect2", 6.0, 3.0));
    shapes.push_back(std::make_shared<Circle>("Circle3", 2.5));
    
    std::cout << "\nDrawing shapes from container:" << std::endl;
    for (const auto& shape : shapes) {
        shape->draw();  // ✓ Calls correct derived class method
        std::cout << "  Area: " << shape->area() << std::endl;  // ✓ Returns actual area
    }
    
    std::cout << "\n✓ Polymorphism works - each object retains its type!" << std::endl;
}

// ===================================================================
// 7. CORRECT WAY: UNIQUE_PTR (C++11/14)
// ===================================================================

void example_unique_ptr_correct() {
    std::cout << "\n=== 7. CORRECT: UNIQUE_PTR FOR OWNERSHIP ===" << std::endl;
    std::cout << "Solution: Use unique_ptr for exclusive ownership\n" << std::endl;
    
    // ✓ CORRECT: unique_ptr to derived class
    std::unique_ptr<Circle> circle = std::make_unique<Circle>("UniqueCircle", 5.5);
    
    // ✓ Move to base class pointer (transfer ownership)
    std::unique_ptr<Shape> shape_ptr = std::move(circle);
    
    std::cout << "\nCalling methods through unique_ptr:" << std::endl;
    shape_ptr->draw();  // ✓ Calls Circle::draw()
    shape_ptr->info();  // ✓ Calls Circle::info()
    
    std::cout << "\n✓ unique_ptr maintains polymorphism without slicing!" << std::endl;
}

// ===================================================================
// 8. FACTORY PATTERN WITH SMART POINTERS (CORRECT)
// ===================================================================

std::unique_ptr<Shape> createShape(const std::string& type, const std::string& name) {
    if (type == "circle") {
        return std::make_unique<Circle>(name, 10.0);
    } else if (type == "rectangle") {
        return std::make_unique<Rectangle>(name, 8.0, 6.0);
    }
    return std::make_unique<Shape>(name);
}

void example_factory_pattern() {
    std::cout << "\n=== 8. FACTORY PATTERN (CORRECT) ===" << std::endl;
    std::cout << "Solution: Return base class pointer to derived objects\n" << std::endl;
    
    auto shape1 = createShape("circle", "FactoryCircle");
    auto shape2 = createShape("rectangle", "FactoryRect");
    
    std::cout << "\nUsing factory-created objects:" << std::endl;
    shape1->draw();
    shape2->draw();
    
    std::cout << "\n✓ Factory pattern prevents slicing by returning pointers!" << std::endl;
}

// ===================================================================
// 9. COMPARISON: WRONG VS RIGHT
// ===================================================================

void example_side_by_side_comparison() {
    std::cout << "\n=== 9. SIDE-BY-SIDE COMPARISON ===" << std::endl;
    
    std::cout << "\n❌ WRONG - Object Slicing:" << std::endl;
    Circle c1("WrongCircle", 5.0);
    Shape s1 = c1;  // Slicing!
    s1.draw();      // Calls Shape::draw()
    std::cout << "   Area: " << s1.area() << " (lost circle data!)" << std::endl;
    
    std::cout << "\n✓ CORRECT - Pointer (no slicing):" << std::endl;
    auto c2 = std::make_shared<Circle>("RightCircle", 5.0);
    std::shared_ptr<Shape> s2 = c2;  // No slicing!
    s2->draw();     // Calls Circle::draw()
    std::cout << "   Area: " << s2->area() << " (preserves circle data!)" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  OBJECT SLICING AND SMART POINTERS (C++11/14)" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    // Wrong ways (demonstrating slicing)
    example_classic_slicing();
    example_shared_ptr_slicing_wrong();
    example_container_slicing_wrong();
    example_copying_smart_ptr_wrong();
    
    // Correct ways (avoiding slicing)
    example_shared_ptr_correct();
    example_container_correct();
    example_unique_ptr_correct();
    example_factory_pattern();
    
    // Comparison
    example_side_by_side_comparison();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  KEY TAKEAWAYS" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "\n❌ CAUSES OF OBJECT SLICING:" << std::endl;
    std::cout << "   1. Assigning derived object to base object by value" << std::endl;
    std::cout << "   2. Passing derived object by value to function expecting base" << std::endl;
    std::cout << "   3. Storing objects by value in containers of base type" << std::endl;
    std::cout << "   4. Creating smart_ptr<Base>(derived_object) - copies by value" << std::endl;
    std::cout << "   5. Dereferencing smart pointer and copying: Base copy = *ptr" << std::endl;
    
    std::cout << "\n✓ HOW TO AVOID SLICING WITH SMART POINTERS:" << std::endl;
    std::cout << "   1. Always create smart_ptr to the actual derived type" << std::endl;
    std::cout << "   2. Use make_shared<DerivedType>() or make_unique<DerivedType>()" << std::endl;
    std::cout << "   3. Store smart_ptr<Base> in containers, not objects" << std::endl;
    std::cout << "   4. Pass smart pointers by reference or const reference" << std::endl;
    std::cout << "   5. Never dereference and copy: always use pointer access (->)" << std::endl;
    
    std::cout << "\n💡 C++11/14 BEST PRACTICES:" << std::endl;
    std::cout << "   • Use std::unique_ptr for exclusive ownership" << std::endl;
    std::cout << "   • Use std::shared_ptr for shared ownership" << std::endl;
    std::cout << "   • Always use make_shared/make_unique (C++14)" << std::endl;
    std::cout << "   • Prefer polymorphism via pointers, not by value" << std::endl;
    std::cout << "   • Virtual destructors are essential for polymorphic classes" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
