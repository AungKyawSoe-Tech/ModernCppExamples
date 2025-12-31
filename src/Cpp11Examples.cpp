#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <tuple>
#include <array>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <numeric>
#include <type_traits>

// ===================================================================
// C++11 COMPREHENSIVE EXAMPLES
// ===================================================================

// ===================================================================
// 1. AUTO TYPE DEDUCTION
// ===================================================================
void example_auto() {
    std::cout << "\n=== 1. AUTO TYPE DEDUCTION ===" << std::endl;
    
    auto a = 42;              // int
    auto b = 3.14;            // double
    auto c = "hello";         // const char*
    auto d = std::string("world");  // std::string
    
    std::vector<int> vec = {1, 2, 3};
    auto it = vec.begin();    // std::vector<int>::iterator
    
    std::cout << "auto deduced types: int, double, const char*, std::string" << std::endl;
}

// ===================================================================
// 2. NULLPTR
// ===================================================================
void foo(int) { std::cout << "Called foo(int)" << std::endl; }
void foo(int*) { std::cout << "Called foo(int*)" << std::endl; }

void example_nullptr() {
    std::cout << "\n=== 2. NULLPTR ===" << std::endl;
    
    int* ptr = nullptr;
    foo(nullptr);  // Calls foo(int*)
    std::cout << "nullptr is " << (ptr == nullptr ? "null" : "not null") << std::endl;
}

// ===================================================================
// 3. RANGE-BASED FOR LOOPS
// ===================================================================
void example_range_for() {
    std::cout << "\n=== 3. RANGE-BASED FOR LOOPS ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout << "Elements: ";
    for (const auto& elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 4. LAMBDA EXPRESSIONS
// ===================================================================
void example_lambdas() {
    std::cout << "\n=== 4. LAMBDA EXPRESSIONS ===" << std::endl;
    
    int x = 10;
    auto add_x = [x](int y) { return x + y; };
    std::cout << "Lambda add_x(5) = " << add_x(5) << std::endl;
    
    auto square = [](int n) { return n * n; };
    std::cout << "Lambda square(7) = " << square(7) << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    int sum = 0;
    std::for_each(vec.begin(), vec.end(), [&sum](int n) { sum += n; });
    std::cout << "Sum using lambda: " << sum << std::endl;
}

// ===================================================================
// 5. RVALUE REFERENCES AND MOVE SEMANTICS
// ===================================================================
class MoveableResource {
private:
    int* data;
    size_t size;
public:
    MoveableResource(size_t s) : size(s) {
        data = new int[size];
        std::cout << "Allocated resource of size " << size << std::endl;
    }
    
    ~MoveableResource() {
        delete[] data;
    }
    
    // Move constructor
    MoveableResource(MoveableResource&& other) noexcept 
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
        std::cout << "Moved resource" << std::endl;
    }
    
    // Move assignment
    MoveableResource& operator=(MoveableResource&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
};

void example_move_semantics() {
    std::cout << "\n=== 5. MOVE SEMANTICS ===" << std::endl;
    
    MoveableResource res1(100);
    MoveableResource res2 = std::move(res1);  // Move constructor
}

// ===================================================================
// 6. VARIADIC TEMPLATES
// ===================================================================
template <typename... Args>
void print_args(Args... args) {
    (void)std::initializer_list<int>{
        (std::cout << args << " ", 0)...
    };
    std::cout << std::endl;
}

template <typename... T>
struct arity {
    static constexpr int value = sizeof...(T);
};

void example_variadic_templates() {
    std::cout << "\n=== 6. VARIADIC TEMPLATES ===" << std::endl;
    
    print_args(1, "hello", 3.14, "world");
    std::cout << "arity<int, double, char>::value = " 
              << arity<int, double, char>::value << std::endl;
}

// ===================================================================
// 7. STRONGLY-TYPED ENUMS
// ===================================================================
enum class Color { Red, Green, Blue };
enum class Traffic { Red, Yellow, Green };  // No conflict!

void example_strongly_typed_enums() {
    std::cout << "\n=== 7. STRONGLY-TYPED ENUMS ===" << std::endl;
    
    Color c = Color::Red;
    Traffic t = Traffic::Red;
    
    // Color and Traffic::Red don't conflict
    std::cout << "Strongly-typed enums prevent naming conflicts" << std::endl;
}

// ===================================================================
// 8. INITIALIZER LISTS
// ===================================================================
int sum_list(const std::initializer_list<int>& list) {
    int total = 0;
    for (auto elem : list) {
        total += elem;
    }
    return total;
}

void example_initializer_lists() {
    std::cout << "\n=== 8. INITIALIZER LISTS ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout << "Vector initialized with {1, 2, 3, 4, 5}" << std::endl;
    std::cout << "sum_list({10, 20, 30}) = " << sum_list({10, 20, 30}) << std::endl;
}

// ===================================================================
// 9. DECLTYPE
// ===================================================================
template <typename X, typename Y>
auto multiply(X x, Y y) -> decltype(x * y) {
    return x * y;
}

void example_decltype() {
    std::cout << "\n=== 9. DECLTYPE ===" << std::endl;
    
    int a = 5;
    decltype(a) b = 10;  // b is int
    
    std::cout << "multiply(3, 4) = " << multiply(3, 4) << std::endl;
    std::cout << "multiply(2.5, 4) = " << multiply(2.5, 4) << std::endl;
}

// ===================================================================
// 10. TYPE ALIASES
// ===================================================================
template <typename T>
using Vec = std::vector<T>;

using String = std::string;

void example_type_aliases() {
    std::cout << "\n=== 10. TYPE ALIASES ===" << std::endl;
    
    Vec<int> numbers = {1, 2, 3};
    String text = "Hello";
    
    std::cout << "Vec<int> and String aliases work" << std::endl;
}

// ===================================================================
// 11. CONSTEXPR
// ===================================================================
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int square(int n) {
    return n * n;
}

void example_constexpr() {
    std::cout << "\n=== 11. CONSTEXPR ===" << std::endl;
    
    constexpr int fact5 = factorial(5);
    constexpr int sq7 = square(7);
    
    std::cout << "constexpr factorial(5) = " << fact5 << std::endl;
    std::cout << "constexpr square(7) = " << sq7 << std::endl;
}

// ===================================================================
// 12. STATIC ASSERTIONS
// ===================================================================
void example_static_assert() {
    std::cout << "\n=== 12. STATIC ASSERTIONS ===" << std::endl;
    
    static_assert(sizeof(int) >= 4, "int must be at least 4 bytes");
    static_assert(sizeof(char) == 1, "char must be 1 byte");
    
    std::cout << "Static assertions passed" << std::endl;
}

// ===================================================================
// 13. DELEGATING CONSTRUCTORS
// ===================================================================
class Rectangle {
private:
    int width, height;
public:
    Rectangle() : Rectangle(0, 0) {}  // Delegate to another constructor
    Rectangle(int w) : Rectangle(w, w) {}  // Delegate
    Rectangle(int w, int h) : width(w), height(h) {
        std::cout << "Rectangle(" << width << ", " << height << ")" << std::endl;
    }
};

void example_delegating_constructors() {
    std::cout << "\n=== 13. DELEGATING CONSTRUCTORS ===" << std::endl;
    
    Rectangle r1;
    Rectangle r2(10);
    Rectangle r3(5, 8);
}

// ===================================================================
// 14. DEFAULT AND DELETE
// ===================================================================
class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

void example_default_delete() {
    std::cout << "\n=== 14. DEFAULT AND DELETE ===" << std::endl;
    
    NonCopyable obj1;
    // NonCopyable obj2 = obj1;  // Error: copy constructor deleted
    
    std::cout << "Non-copyable class works" << std::endl;
}

// ===================================================================
// 15. OVERRIDE AND FINAL
// ===================================================================
class Base {
public:
    virtual void foo() { std::cout << "Base::foo()" << std::endl; }
    virtual ~Base() = default;
};

class Derived final : public Base {
public:
    void foo() override { std::cout << "Derived::foo()" << std::endl; }
};

void example_override_final() {
    std::cout << "\n=== 15. OVERRIDE AND FINAL ===" << std::endl;
    
    std::unique_ptr<Base> ptr(new Derived());
    ptr->foo();
}

// ===================================================================
// 16. SMART POINTERS
// ===================================================================
void example_smart_pointers() {
    std::cout << "\n=== 16. SMART POINTERS ===" << std::endl;
    
    // unique_ptr
    std::unique_ptr<int> uptr(new int(42));
    std::cout << "*uptr = " << *uptr << std::endl;
    
    // shared_ptr
    std::shared_ptr<int> sptr1 = std::make_shared<int>(100);
    std::shared_ptr<int> sptr2 = sptr1;
    std::cout << "shared_ptr count: " << sptr1.use_count() << std::endl;
    
    // weak_ptr
    std::weak_ptr<int> wptr = sptr1;
    std::cout << "weak_ptr valid: " << !wptr.expired() << std::endl;
}

// ===================================================================
// 17. TUPLES
// ===================================================================
void example_tuples() {
    std::cout << "\n=== 17. TUPLES ===" << std::endl;
    
    std::tuple<int, std::string, double> tup{42, "hello", 3.14};
    
    std::cout << "Tuple element 0: " << std::get<0>(tup) << std::endl;
    std::cout << "Tuple element 1: " << std::get<1>(tup) << std::endl;
    std::cout << "Tuple element 2: " << std::get<2>(tup) << std::endl;
    
    int a;
    std::string b;
    double c;
    std::tie(a, b, c) = tup;
    std::cout << "Using tie: " << a << ", " << b << ", " << c << std::endl;
}

// ===================================================================
// 18. STD::ARRAY
// ===================================================================
void example_array() {
    std::cout << "\n=== 18. STD::ARRAY ===" << std::endl;
    
    std::array<int, 5> arr = {1, 2, 3, 4, 5};
    
    std::cout << "Array size: " << arr.size() << std::endl;
    std::cout << "Array elements: ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 19. UNORDERED CONTAINERS
// ===================================================================
void example_unordered_containers() {
    std::cout << "\n=== 19. UNORDERED CONTAINERS ===" << std::endl;
    
    std::unordered_map<std::string, int> map;
    map["one"] = 1;
    map["two"] = 2;
    map["three"] = 3;
    
    std::cout << "Unordered map: ";
    for (const auto& pair : map) {
        std::cout << pair.first << "=" << pair.second << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 20. STD::THREAD
// ===================================================================
void thread_function(int n) {
    std::cout << "Thread " << n << " running" << std::endl;
}

void example_threads() {
    std::cout << "\n=== 20. STD::THREAD ===" << std::endl;
    
    std::thread t1(thread_function, 1);
    std::thread t2(thread_function, 2);
    
    t1.join();
    t2.join();
    
    std::cout << "Threads completed" << std::endl;
}

// ===================================================================
// 21. CHRONO
// ===================================================================
void example_chrono() {
    std::cout << "\n=== 21. STD::CHRONO ===" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Do some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Duration: " << duration.count() << " ms" << std::endl;
}

// ===================================================================
// 22. RAW STRING LITERALS
// ===================================================================
void example_raw_strings() {
    std::cout << "\n=== 22. RAW STRING LITERALS ===" << std::endl;
    
    std::string normal = "Line 1\\nLine 2\\nLine 3";
    std::string raw = R"(Line 1
Line 2
Line 3)";
    
    std::cout << "Raw string with newlines preserved" << std::endl;
}

// ===================================================================
// 23. ATTRIBUTES
// ===================================================================
[[noreturn]] void terminate_program() {
    std::exit(0);
}

void example_attributes() {
    std::cout << "\n=== 23. ATTRIBUTES ===" << std::endl;
    
    std::cout << "[[noreturn]] attribute available" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    C++11 FEATURES COMPREHENSIVE EXAMPLES" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    example_auto();
    example_nullptr();
    example_range_for();
    example_lambdas();
    example_move_semantics();
    example_variadic_templates();
    example_strongly_typed_enums();
    example_initializer_lists();
    example_decltype();
    example_type_aliases();
    example_constexpr();
    example_static_assert();
    example_delegating_constructors();
    example_default_delete();
    example_override_final();
    example_smart_pointers();
    example_tuples();
    example_array();
    example_unordered_containers();
    example_threads();
    example_chrono();
    example_raw_strings();
    example_attributes();
    
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    ALL C++11 EXAMPLES COMPLETED" << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    return 0;
}
