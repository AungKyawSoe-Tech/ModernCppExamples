#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <future>
#include <thread>

// ===================================================================
// 1. NO CAPTURE []
// ===================================================================
void example_no_capture() {
    std::cout << "\n=== 1. NO CAPTURE [] ===" << std::endl;
    
    int global_counter = 100;  // Global-like variable
    
    // Lambda can only access its parameters and static/global variables
    auto lambda = []() {
        static int static_var = 42;
        std::cout << "Static variable: " << static_var++ << std::endl;
        return static_var;
    };
    
    std::cout << "Call 1: " << lambda() << std::endl;  // 43
    std::cout << "Call 2: " << lambda() << std::endl;  // 44
}

// ===================================================================
// 2. CAPTURE ALL BY VALUE [=]
// ===================================================================
void example_capture_all_by_value() {
    std::cout << "\n=== 2. CAPTURE ALL BY VALUE [=] ===" << std::endl;
    
    int x = 10;
    int y = 20;
    int z = 30;  // Not referenced, so not captured
    
    auto lambda = [=]() {  // Captures x and y by value
        std::cout << "x = " << x << ", y = " << y << std::endl;
        // Cannot modify: x and y are const copies
        // x = 100;  // ERROR!
        return x + y;
    };
    
    x = 100;  // Change original
    std::cout << "Lambda result: " << lambda() << std::endl;  // Still 30 (10+20)
    std::cout << "Original x after: " << x << std::endl;      // 100
}

// ===================================================================
// 3. CAPTURE ALL BY REFERENCE [&]
// ===================================================================
void example_capture_all_by_reference() {
    std::cout << "\n=== 3. CAPTURE ALL BY REFERENCE [&] ===" << std::endl;
    
    int x = 10;
    int y = 20;
    
    auto lambda = [&]() {  // Captures x and y by reference
        std::cout << "Before modify: x = " << x << ", y = " << y << std::endl;
        x = 100;  // Modifies original
        y = 200;  // Modifies original
        return x + y;
    };
    
    std::cout << "Before lambda: x=" << x << ", y=" << y << std::endl;  // 10, 20
    std::cout << "Lambda result: " << lambda() << std::endl;            // 300
    std::cout << "After lambda: x=" << x << ", y=" << y << std::endl;   // 100, 200
}

// ===================================================================
// 4. CAPTURE SPECIFIC BY VALUE [var]
// ===================================================================
void example_specific_capture_by_value() {
    std::cout << "\n=== 4. CAPTURE SPECIFIC BY VALUE [var] ===" << std::endl;
    
    int a = 1, b = 2, c = 3;
    
    auto lambda = [a, c]() {  // Only captures a and c by value
        std::cout << "a = " << a << ", c = " << c << std::endl;
        // b is not accessible here
        return a + c;
    };
    
    a = 100;  // Change original
    std::cout << "Lambda result: " << lambda() << std::endl;  // 4 (1+3)
    std::cout << "Original a after: " << a << std::endl;      // 100
}

// ===================================================================
// 5. CAPTURE SPECIFIC BY REFERENCE [&var]
// ===================================================================
void example_specific_capture_by_reference() {
    std::cout << "\n=== 5. CAPTURE SPECIFIC BY REFERENCE [&var] ===" << std::endl;
    
    int a = 1, b = 2, c = 3;
    
    auto lambda = [&a, &c]() {  // Only captures a and c by reference
        std::cout << "Before modify: a = " << a << ", c = " << c << std::endl;
        a = 10;  // Modifies original a
        c = 30;  // Modifies original c
        return a + c;
    };
    
    std::cout << "Before lambda: a=" << a << ", c=" << c << std::endl;  // 1, 3
    std::cout << "Lambda result: " << lambda() << std::endl;            // 40
    std::cout << "After lambda: a=" << a << ", c=" << c << std::endl;   // 10, 30
}

// ===================================================================
// 6. MIXED CAPTURE [=, &var]
// ===================================================================
void example_mixed_capture_value_default() {
    std::cout << "\n=== 6. MIXED CAPTURE [=, &var] ===" << std::endl;
    
    int x = 10, y = 20, z = 30;
    
    auto lambda = [=, &z]() {  // x,y by value, z by reference
        std::cout << "x=" << x << ", y=" << y << ", z=" << z << std::endl;
        // x = 100;  // ERROR: captured by value
        z = 300;     // OK: captured by reference
        return x + y + z;
    };
    
    std::cout << "Before lambda: z=" << z << std::endl;    // 30
    std::cout << "Lambda result: " << lambda() << std::endl;  // 330
    std::cout << "After lambda: z=" << z << std::endl;     // 300
}

// ===================================================================
// 7. MIXED CAPTURE [&, var]
// ===================================================================
void example_mixed_capture_reference_default() {
    std::cout << "\n=== 7. MIXED CAPTURE [&, var] ===" << std::endl;
    
    int x = 10, y = 20, z = 30;
    
    auto lambda = [&, y]() {  // x,z by reference, y by value
        std::cout << "x=" << x << ", y=" << y << ", z=" << z << std::endl;
        x = 100;  // OK: captured by reference
        // y = 200;  // ERROR: captured by value
        z = 300;  // OK: captured by reference
        return x + y + z;
    };
    
    std::cout << "Before lambda: x=" << x << ", y=" << y << ", z=" << z << std::endl;  // 10,20,30
    std::cout << "Lambda result: " << lambda() << std::endl;  // 420
    std::cout << "After lambda: x=" << x << ", y=" << y << ", z=" << z << std::endl;   // 100,20,300
}

// ===================================================================
// 8. CAPTURE THIS POINTER [this]
// ===================================================================
class ExampleClass {
private:
    int member_var = 42;
    std::string name = "TestClass";
    
public:
    void example_capture_this() {
        std::cout << "\n=== 8. CAPTURE THIS POINTER [this] ===" << std::endl;
        
        int local_var = 100;
        
        auto lambda = [this, local_var]() {
            std::cout << "Member var: " << member_var << std::endl;
            std::cout << "Name: " << name << std::endl;
            std::cout << "Local var: " << local_var << std::endl;
            member_var = 999;  // Can modify member through this
            return member_var + local_var;
        };
        
        std::cout << "Before lambda: member_var=" << member_var << std::endl;  // 42
        std::cout << "Lambda result: " << lambda() << std::endl;               // 1099
        std::cout << "After lambda: member_var=" << member_var << std::endl;   // 999
    }
};

// ===================================================================
// 9. CAPTURE *THIS [*this] (C++17)
// ===================================================================
class ExampleClass2 {
private:
    int value = 42;
    
public:
    void example_capture_star_this() {
        std::cout << "\n=== 9. CAPTURE *THIS [*this] (C++17) ===" << std::endl;
        
        std::cout << "Original object value: " << value << std::endl;
        
        // Capture a copy of the entire object
        auto lambda = [*this]() mutable {
            std::cout << "Copied object value: " << value << std::endl;
            value = 100;  // Modifies the copy
            return value;
        };
        
        // Simulate async usage
        auto future = std::async(std::launch::async, lambda);
        
        std::cout << "Original unchanged during async: " << value << std::endl;  // 42
        std::cout << "Async result (copy modified): " << future.get() << std::endl;  // 100
        std::cout << "Original still: " << value << std::endl;  // 42
    }
};

// ===================================================================
// 10. MUTABLE LAMBDA
// ===================================================================
void example_mutable_lambda() {
    std::cout << "\n=== 10. MUTABLE LAMBDA ===" << std::endl;
    
    int x = 10;
    int counter = 0;
    
    // Without mutable - cannot modify captured values
    // auto lambda1 = [x, counter]() {
    //     counter++;  // ERROR
    //     return x + counter;
    // };
    
    // With mutable - can modify captured copies
    auto lambda2 = [x, counter]() mutable {
        std::cout << "Before increment: x=" << x << ", counter=" << counter << std::endl;
        x += 5;      // Can modify captured copy
        counter++;   // Can modify captured copy
        std::cout << "After increment: x=" << x << ", counter=" << counter << std::endl;
        return x + counter;
    };
    
    std::cout << "Before lambda: counter=" << counter << std::endl;  // 0
    std::cout << "Lambda result 1: " << lambda2() << std::endl;      // 16 (15+1)
    std::cout << "Lambda result 2: " << lambda2() << std::endl;      // 22 (20+2)
    std::cout << "Original unchanged: counter=" << counter << std::endl;  // 0
}

// ===================================================================
// 11. CAPTURE WITH INITIALIZATION (C++14)
// ===================================================================
void example_capture_with_initialization() {
    std::cout << "\n=== 11. CAPTURE WITH INITIALIZATION (C++14) ===" << std::endl;
    
    int x = 10;
    std::string str = "Hello";
    
    // C++14: Capture with initialization expressions
    auto lambda1 = [value = x * 2, message = str + " World"]() {
        std::cout << "value = " << value << std::endl;      // 20
        std::cout << "message = " << message << std::endl;  // "Hello World"
        return value;
    };
    
    auto lambda2 = [&ref = x]() {  // Capture reference with custom name
        ref = 100;  // Modifies original x
        return ref;
    };
    
    std::cout << "Lambda1 result: " << lambda1() << std::endl;  // 20
    std::cout << "Before lambda2: x=" << x << std::endl;        // 10
    std::cout << "Lambda2 result: " << lambda2() << std::endl;  // 100
    std::cout << "After lambda2: x=" << x << std::endl;         // 100
    
    // Capture unique_ptr
    auto lambda3 = [ptr = std::make_unique<int>(42)]() {
        return *ptr;
    };
    std::cout << "Lambda3 with unique_ptr: " << lambda3() << std::endl;  // 42
}

// ===================================================================
// 12. MULTIPLE SPECIFIC CAPTURES
// ===================================================================
void example_multiple_specific_captures() {
    std::cout << "\n=== 12. MULTIPLE SPECIFIC CAPTURES ===" << std::endl;
    
    int a = 1, b = 2, c = 3, d = 4, e = 5;
    
    auto lambda = [a, &b, c, &d]() {  // Mix of value and reference
        std::cout << "Captured: a=" << a << ", b=" << b 
                  << ", c=" << c << ", d=" << d << std::endl;
        b = 20;  // OK: reference
        d = 40;  // OK: reference
        // a = 10;  // ERROR: value capture
        return a + b + c + d;
    };
    
    std::cout << "Before lambda: a=" << a << ", b=" << b 
              << ", c=" << c << ", d=" << d << std::endl;  // 1,2,3,4
    std::cout << "Lambda result: " << lambda() << std::endl;           // 66 (1+20+3+40)
    std::cout << "After lambda: a=" << a << ", b=" << b 
              << ", c=" << c << ", d=" << d << std::endl;   // 1,20,3,40
}

// ===================================================================
// 13. PRACTICAL EXAMPLE: EVENT HANDLER
// ===================================================================
void example_event_handler() {
    std::cout << "\n=== 13. PRACTICAL EXAMPLE: EVENT HANDLER ===" << std::endl;
    
    class Button {
    private:
        std::vector<std::function<void()>> click_handlers;
        std::string label;
        
    public:
        Button(const std::string& lbl) : label(lbl) {}
        
        void add_handler(std::function<void()> handler) {
            click_handlers.push_back(handler);
        }
        
        void click() {
            std::cout << "Button '" << label << "' clicked!" << std::endl;
            for (auto& handler : click_handlers) {
                handler();
            }
        }
    };
    
    std::string username = "Alice";
    int click_count = 0;
    
    Button button("Submit");
    
    // Capture username by value, click_count by reference
    button.add_handler([username, &click_count]() {
        std::cout << "Hello " << username << "!" << std::endl;
        click_count++;
        std::cout << "Click count: " << click_count << std::endl;
    });
    
    button.click();  // Hello Alice! Click count: 1
    button.click();  // Hello Alice! Click count: 2
    
    username = "Bob";  // Change doesn't affect captured value
    
    button.click();  // Still: Hello Alice! Click count: 3
}

// ===================================================================
// 14. DANGEROUS EXAMPLE: DANGLING REFERENCE
// ===================================================================
void example_dangling_reference() {
    std::cout << "\n=== 14. DANGEROUS: DANGLING REFERENCE ===" << std::endl;
    
    std::function<void()> dangerous_lambda;
    
    {
        int local_variable = 42;
        
        // CAPTURE BY REFERENCE - DANGER!
        dangerous_lambda = [&local_variable]() {
            std::cout << "Local variable: " << local_variable << std::endl;  // UNDEFINED!
        };
        
        // local_variable will be destroyed when scope ends
    }
    
    std::cout << "WARNING: Calling lambda with dangling reference..." << std::endl;
    // dangerous_lambda();  // UNDEFINED BEHAVIOR - might crash
    
    // SAFE ALTERNATIVE
    std::function<void()> safe_lambda;
    {
        int local_variable = 42;
        safe_lambda = [local_variable]() {  // Capture by value
            std::cout << "Safe: captured value = " << local_variable << std::endl;
        };
    }
    
    safe_lambda();  // Safe! Outputs: 42
}

// ===================================================================
// 15. COMPREHENSIVE EXAMPLE: ALL IN ONE
// ===================================================================
void comprehensive_example() {
    std::cout << "\n=== 15. COMPREHENSIVE EXAMPLE ===" << std::endl;
    
    int x = 1, y = 2, z = 3;
    static int static_var = 100;
    
    std::cout << "Creating multiple lambdas with different captures:" << std::endl;
    
    // 1. No capture
    auto lambda1 = []() { 
        std::cout << "  Lambda1: No capture, static=" << static_var++ << std::endl;
    };
    
    // 2. Capture all by value
    auto lambda2 = [=]() {
        std::cout << "  Lambda2: All by value, x=" << x << ", y=" << y << std::endl;
    };
    
    // 3. Capture all by reference
    auto lambda3 = [&]() {
        std::cout << "  Lambda3: All by ref, before x=" << x;
        x = 10;
        std::cout << ", after x=" << x << std::endl;
    };
    
    // 4. Specific captures
    auto lambda4 = [x, &y]() {
        std::cout << "  Lambda4: x by value=" << x << ", y by ref=" << y << std::endl;
        y = 20;
    };
    
    // 5. Mixed capture
    auto lambda5 = [=, &z]() {
        std::cout << "  Lambda5: x,y by value, z by ref, z=" << z << std::endl;
        z = 30;
    };
    
    // 6. Mutable
    auto lambda6 = [x]() mutable {
        std::cout << "  Lambda6: Mutable, x before=" << x;
        x = 100;  // Modifies copy
        std::cout << ", x after=" << x << std::endl;
    };
    
    // Execute all
    lambda1();
    lambda2();
    lambda3();
    lambda4();
    lambda5();
    lambda6();
    
    std::cout << "\nFinal values: x=" << x << ", y=" << y << ", z=" << z << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "===================================================================" << std::endl;
    std::cout << "COMPLETE C++ LAMBDA CAPTURE EXAMPLES IN ONE FILE" << std::endl;
    std::cout << "===================================================================" << std::endl;
    
    // Run all examples
    example_no_capture();
    example_capture_all_by_value();
    example_capture_all_by_reference();
    example_specific_capture_by_value();
    example_specific_capture_by_reference();
    example_mixed_capture_value_default();
    example_mixed_capture_reference_default();
    
    ExampleClass obj1;
    obj1.example_capture_this();
    
    ExampleClass2 obj2;
    obj2.example_capture_star_this();
    
    example_mutable_lambda();
    example_capture_with_initialization();
    example_multiple_specific_captures();
    example_event_handler();
    example_dangling_reference();
    comprehensive_example();
    
    std::cout << "\n===================================================================" << std::endl;
    std::cout << "ALL EXAMPLES COMPLETED SUCCESSFULLY!" << std::endl;
    std::cout << "===================================================================" << std::endl;
    
    return 0;
}