#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <span>
#include <ranges>
#include <concepts>

// ===================================================================
// PREVENTING OBJECT SLICING WITH C++20 FEATURES
// ===================================================================

// Base class
class Shape {
protected:
    std::string name;
    
public:
    Shape(const std::string& n) : name(n) {}
    virtual ~Shape() = default;
    
    virtual void draw() const {
        std::cout << "Drawing Shape: " << name << std::endl;
    }
    
    virtual double area() const { return 0.0; }
    
    virtual std::string type() const { return "Shape"; }
};

class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(const std::string& n, double r) : Shape(n), radius(r) {}
    
    void draw() const override {
        std::cout << "Drawing Circle: " << name << " (radius=" << radius << ")" << std::endl;
    }
    
    double area() const override {
        return 3.14159 * radius * radius;
    }
    
    std::string type() const override { return "Circle"; }
};

class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(const std::string& n, double w, double h) 
        : Shape(n), width(w), height(h) {}
    
    void draw() const override {
        std::cout << "Drawing Rectangle: " << name << " (" << width << "x" << height << ")" << std::endl;
    }
    
    double area() const override {
        return width * height;
    }
    
    std::string type() const override { return "Rectangle"; }
};

// ===================================================================
// 1. C++20 CONCEPTS - ENFORCE POINTER TYPES AT COMPILE TIME
// ===================================================================

// Concept: Must be a pointer or smart pointer to a type derived from Shape
template<typename T>
concept ShapePointer = requires(T t) {
    { *t };  // Must be dereferenceable
    requires std::derived_from<std::remove_reference_t<decltype(*t)>, Shape>;
};

// Concept: Must be a smart pointer
template<typename T>
concept SmartPointer = requires(T t) {
    typename T::element_type;  // Must have element_type (shared_ptr/unique_ptr trait)
    { t.get() } -> std::same_as<typename T::element_type*>;
    { *t } -> std::same_as<typename T::element_type&>;
};

// Function that ONLY accepts pointers/smart pointers - prevents slicing at compile time
template<ShapePointer T>
void draw_shape(const T& shape_ptr) {
    shape_ptr->draw();
}

// Function that ONLY accepts smart pointers specifically
template<SmartPointer T>
    requires std::derived_from<typename T::element_type, Shape>
void process_shape(const T& shape_ptr) {
    std::cout << "Type: " << shape_ptr->type() << ", Area: " << shape_ptr->area() << std::endl;
}

void example_concepts_prevent_slicing() {
    std::cout << "\n=== 1. C++20 CONCEPTS - COMPILE-TIME PREVENTION ===" << std::endl;
    std::cout << "Solution: Use concepts to enforce pointer usage at compile time\n" << std::endl;
    
    auto circle = std::make_shared<Circle>("ConceptCircle", 5.0);
    auto rect = std::make_unique<Rectangle>("ConceptRect", 4.0, 6.0);
    
    std::cout << "✓ These calls compile (using pointers):" << std::endl;
    draw_shape(circle);           // ✓ Works with shared_ptr
    draw_shape(rect);             // ✓ Works with unique_ptr
    
    Circle* raw_circle = circle.get();
    draw_shape(raw_circle);       // ✓ Works with raw pointer
    
    process_shape(circle);        // ✓ Works with smart pointers
    process_shape(rect);          // ✓ Works with smart pointers
    
    std::cout << "\n✓ Concepts prevent slicing at compile time!" << std::endl;
    std::cout << "   (Uncommenting the line below would cause a compile error)" << std::endl;
    
    // Circle circle_obj("DirectCircle", 3.0);
    // draw_shape(circle_obj);    // ❌ Compile error! Not a pointer type
}

// ===================================================================
// 2. C++20 RANGES - WORK WITH REFERENCES, NOT COPIES
// ===================================================================

void example_ranges_prevent_slicing() {
    std::cout << "\n=== 2. C++20 RANGES - REFERENCE-BASED OPERATIONS ===" << std::endl;
    std::cout << "Solution: Ranges work with references, preventing copies\n" << std::endl;
    
    std::vector<std::shared_ptr<Shape>> shapes;
    shapes.push_back(std::make_shared<Circle>("RangeCircle1", 3.0));
    shapes.push_back(std::make_shared<Rectangle>("RangeRect1", 5.0, 4.0));
    shapes.push_back(std::make_shared<Circle>("RangeCircle2", 2.5));
    
    std::cout << "Using ranges to filter and transform (no slicing):" << std::endl;
    
    // Filter shapes with area > 20 and draw them
    auto large_shapes = shapes 
        | std::views::filter([](const auto& s) { return s->area() > 20.0; })
        | std::views::transform([](const auto& s) { return s.get(); });
    
    for (const auto* shape : large_shapes) {
        shape->draw();
        std::cout << "  Area: " << shape->area() << std::endl;
    }
    
    std::cout << "\n✓ Ranges operate on references - no slicing occurs!" << std::endl;
}

// ===================================================================
// 3. C++20 SPAN - NON-OWNING VIEWS WITHOUT SLICING
// ===================================================================

// Function taking span of Shape pointers
void draw_all_shapes(std::span<const std::shared_ptr<Shape>> shapes) {
    std::cout << "Drawing " << shapes.size() << " shapes:" << std::endl;
    for (const auto& shape : shapes) {
        shape->draw();
    }
}

// Function taking span of raw pointers
void process_shapes(std::span<const Shape* const> shapes) {
    double total_area = 0.0;
    for (const auto* shape : shapes) {
        total_area += shape->area();
    }
    std::cout << "Total area: " << total_area << std::endl;
}

void example_span_prevent_slicing() {
    std::cout << "\n=== 3. C++20 SPAN - NON-OWNING VIEWS ===" << std::endl;
    std::cout << "Solution: std::span provides views without copying\n" << std::endl;
    
    std::vector<std::shared_ptr<Shape>> shapes;
    shapes.push_back(std::make_shared<Circle>("SpanCircle", 4.0));
    shapes.push_back(std::make_shared<Rectangle>("SpanRect", 3.0, 5.0));
    
    std::cout << "Passing span of smart pointers:" << std::endl;
    draw_all_shapes(shapes);  // ✓ No slicing - span of pointers
    
    std::cout << "\nPassing span of raw pointers:" << std::endl;
    std::vector<const Shape*> raw_ptrs;
    for (const auto& s : shapes) {
        raw_ptrs.push_back(s.get());
    }
    process_shapes(raw_ptrs);  // ✓ No slicing - span of raw pointers
    
    std::cout << "\n✓ std::span is non-owning - operates on existing pointers!" << std::endl;
}

// ===================================================================
// 4. C++20 CONSTRAINTS - DELETE SLICING-PRONE FUNCTIONS
// ===================================================================

template<typename T>
    requires std::derived_from<T, Shape>
class ShapeContainer {
private:
    std::vector<std::unique_ptr<T>> items;
    
public:
    // ✓ CORRECT: Add by pointer (transfers ownership)
    void add(std::unique_ptr<T> shape) {
        items.push_back(std::move(shape));
    }
    
    // ❌ DELETED: Prevent adding by value (would cause slicing)
    void add(const T& shape) = delete;
    void add(T&& shape) = delete;
    
    // ✓ CORRECT: Access via reference
    const T& get(size_t index) const {
        return *items[index];
    }
    
    void draw_all() const {
        for (const auto& item : items) {
            item->draw();
        }
    }
    
    size_t size() const { return items.size(); }
};

void example_delete_slicing_functions() {
    std::cout << "\n=== 4. DELETE SLICING-PRONE FUNCTIONS ===" << std::endl;
    std::cout << "Solution: Use = delete to prevent value-based operations\n" << std::endl;
    
    ShapeContainer<Shape> container;
    
    std::cout << "✓ These operations work (using pointers):" << std::endl;
    container.add(std::make_unique<Circle>("DeleteCircle", 5.0));
    container.add(std::make_unique<Rectangle>("DeleteRect", 4.0, 3.0));
    
    container.draw_all();
    
    std::cout << "\n✓ Deleted functions prevent slicing at compile time!" << std::endl;
    std::cout << "   (Uncommenting the lines below would cause compile errors)" << std::endl;
    
    // Circle circle("ByValueCircle", 3.0);
    // container.add(circle);          // ❌ Compile error! Function deleted
    // container.add(std::move(circle)); // ❌ Compile error! Function deleted
}

// ===================================================================
// 5. C++20 DESIGNATED INITIALIZERS WITH SMART POINTERS
// ===================================================================

struct ShapeConfig {
    std::shared_ptr<Shape> primary;
    std::shared_ptr<Shape> secondary;
    std::string label;
};

void example_designated_initializers() {
    std::cout << "\n=== 5. DESIGNATED INITIALIZERS - SAFE INITIALIZATION ===" << std::endl;
    std::cout << "Solution: Initialize with pointers using designated initializers\n" << std::endl;
    
    // ✓ CORRECT: Designated initializers with smart pointers
    ShapeConfig config {
        .primary = std::make_shared<Circle>("Primary", 6.0),
        .secondary = std::make_shared<Rectangle>("Secondary", 5.0, 4.0),
        .label = "MyShapes"
    };
    
    std::cout << "Configuration: " << config.label << std::endl;
    config.primary->draw();
    config.secondary->draw();
    
    std::cout << "\n✓ Designated initializers ensure proper pointer initialization!" << std::endl;
}

// ===================================================================
// 6. C++20 REQUIRES CLAUSE - POLYMORPHIC CONTAINERS ONLY
// ===================================================================

template<typename Container>
    requires requires(Container c) {
        // Require that container holds pointers to Shape-derived types
        { *c.begin() } -> std::convertible_to<std::shared_ptr<Shape>>;
    }
void draw_container(const Container& shapes) {
    std::cout << "Drawing " << shapes.size() << " shapes from container:" << std::endl;
    for (const auto& shape : shapes) {
        shape->draw();
    }
}

void example_requires_clause() {
    std::cout << "\n=== 6. REQUIRES CLAUSE - CONSTRAIN CONTAINER TYPES ===" << std::endl;
    std::cout << "Solution: Require containers to hold pointers only\n" << std::endl;
    
    std::vector<std::shared_ptr<Shape>> shapes;
    shapes.push_back(std::make_shared<Circle>("RequiresCircle", 4.5));
    shapes.push_back(std::make_shared<Rectangle>("RequiresRect", 6.0, 2.0));
    
    std::cout << "✓ This works (container of shared_ptr):" << std::endl;
    draw_container(shapes);
    
    std::cout << "\n✓ Requires clause ensures polymorphic container usage!" << std::endl;
    std::cout << "   (Uncommenting the lines below would cause a compile error)" << std::endl;
    
    // std::vector<Circle> value_circles;
    // value_circles.push_back(Circle("ValueCircle", 3.0));
    // draw_container(value_circles);  // ❌ Compile error! Container holds values, not pointers
}

// ===================================================================
// 7. COMPARISON: C++11/14 VS C++20 PREVENTION
// ===================================================================

void example_comparison() {
    std::cout << "\n=== 7. C++11/14 VS C++20 PREVENTION COMPARISON ===" << std::endl;
    
    std::cout << "\n📋 C++11/14 APPROACH (Runtime/Developer Discipline):" << std::endl;
    std::cout << "   • Manually use smart pointers everywhere" << std::endl;
    std::cout << "   • Remember to not pass by value" << std::endl;
    std::cout << "   • Runtime errors if you forget" << std::endl;
    std::cout << "   • Code reviews needed to catch mistakes" << std::endl;
    
    std::cout << "\n✨ C++20 APPROACH (Compile-Time Enforcement):" << std::endl;
    std::cout << "   • Concepts enforce pointer usage at compile time" << std::endl;
    std::cout << "   • = delete prevents slicing-prone operations" << std::endl;
    std::cout << "   • Requires clauses constrain templates" << std::endl;
    std::cout << "   • Compiler catches mistakes before runtime!" << std::endl;
    std::cout << "   • std::span provides safe non-owning views" << std::endl;
    std::cout << "   • Ranges work with references by default" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  PREVENTING OBJECT SLICING WITH C++20 FEATURES" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    example_concepts_prevent_slicing();
    example_ranges_prevent_slicing();
    example_span_prevent_slicing();
    example_delete_slicing_functions();
    example_designated_initializers();
    example_requires_clause();
    example_comparison();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  C++20 SLICING PREVENTION SUMMARY" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "\n✨ NEW C++20 FEATURES TO PREVENT SLICING:" << std::endl;
    std::cout << "\n1. 🎯 CONCEPTS:" << std::endl;
    std::cout << "   • Define ShapePointer concept to enforce pointer types" << std::endl;
    std::cout << "   • Compile-time error if trying to pass by value" << std::endl;
    std::cout << "   • Example: template<ShapePointer T> void draw(const T& ptr)" << std::endl;
    
    std::cout << "\n2. 🔍 RANGES:" << std::endl;
    std::cout << "   • Operate on references, not copies" << std::endl;
    std::cout << "   • views::filter and views::transform preserve types" << std::endl;
    std::cout << "   • No accidental slicing during transformations" << std::endl;
    
    std::cout << "\n3. 👁️ STD::SPAN:" << std::endl;
    std::cout << "   • Non-owning view of container" << std::endl;
    std::cout << "   • std::span<shared_ptr<Shape>> - no copies" << std::endl;
    std::cout << "   • Safe passing of pointer collections" << std::endl;
    
    std::cout << "\n4. 🚫 = DELETE:" << std::endl;
    std::cout << "   • Explicitly delete value-based operations" << std::endl;
    std::cout << "   • void add(const T& shape) = delete" << std::endl;
    std::cout << "   • Compiler prevents slicing attempts" << std::endl;
    
    std::cout << "\n5. 📝 REQUIRES CLAUSES:" << std::endl;
    std::cout << "   • Constrain templates to pointer-holding containers" << std::endl;
    std::cout << "   • requires { *c.begin() } -> convertible_to<shared_ptr<Shape>>" << std::endl;
    std::cout << "   • Type-safe generic programming" << std::endl;
    
    std::cout << "\n6. 🎨 DESIGNATED INITIALIZERS:" << std::endl;
    std::cout << "   • Clear initialization syntax" << std::endl;
    std::cout << "   • Config { .shape = make_shared<Circle>(...) }" << std::endl;
    std::cout << "   • Prevents accidental by-value initialization" << std::endl;
    
    std::cout << "\n🎯 KEY ADVANTAGE OF C++20:" << std::endl;
    std::cout << "   • C++11/14: Prevent slicing through discipline (runtime errors)" << std::endl;
    std::cout << "   • C++20: Prevent slicing through type system (compile-time errors)" << std::endl;
    std::cout << "   • Shift from \"remember not to\" → \"impossible to\"" << std::endl;
    
    std::cout << "\n💡 BEST PRACTICE (C++20):" << std::endl;
    std::cout << "   ✓ Use concepts to enforce pointer parameters" << std::endl;
    std::cout << "   ✓ Use requires clauses for container constraints" << std::endl;
    std::cout << "   ✓ Use = delete for slicing-prone operations" << std::endl;
    std::cout << "   ✓ Use std::span for non-owning views" << std::endl;
    std::cout << "   ✓ Use ranges for safe transformations" << std::endl;
    std::cout << "   ✓ Let the compiler catch slicing at compile time!" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
