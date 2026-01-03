/*
 * VIRTUAL FUNCTIONS IN TEMPLATES: THE CODE BLOAT PROBLEM
 * 
 * This file demonstrates WHY you should AVOID virtual functions in class templates
 * and shows the severe code bloat that results from naive templatization.
 * 
 * KEY PRINCIPLE: Virtual functions in templates are instantiated EVERY TIME,
 * even if never called, causing exponential code bloat.
 * 
 * References:
 * - C++ Core Guidelines T.80: Do not naively templatize a class hierarchy
 * - C++ Core Guidelines T.5: Combine generic and OO techniques judiciously
 * - C++ Core Guidelines T.84: Use a non-template core implementation
 */

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

// ============================================================================
// SECTION 1: THE PROBLEM - WHY VIRTUAL FUNCTIONS CAUSE BLOAT
// ============================================================================

namespace the_problem {

/*
 * THE CORE ISSUE:
 * 
 * In a class template:
 * - NON-VIRTUAL functions: Only instantiated if explicitly called (lazy)
 * - VIRTUAL functions: ALWAYS instantiated when class is instantiated (eager)
 * 
 * WHY?
 * Virtual functions MUST be in the vtable, and the vtable is constructed
 * at compile-time during template instantiation. Therefore, ALL virtual
 * functions must be compiled, even if your code never calls them.
 * 
 * This is the root cause of code bloat in template hierarchies.
 */

template<typename T>
class DemoInstantiation {
public:
    // VIRTUAL: Compiled when class is instantiated
    virtual void always_compiled() {
        std::cout << "Virtual: always compiled for " << typeid(T).name() << "\n";
    }
    
    virtual void never_called_but_still_compiled() {
        std::cout << "Virtual: wasting space for " << typeid(T).name() << "\n";
    }
    
    virtual ~DemoInstantiation() = default;
    
    // NON-VIRTUAL: Only compiled if actually called
    void only_if_used() {
        std::cout << "Non-virtual: only compiled if called for " << typeid(T).name() << "\n";
    }
    
    void also_only_if_used() {
        std::cout << "Non-virtual: lazy instantiation for " << typeid(T).name() << "\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "THE PROBLEM: Virtual Functions Cause Template Bloat\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Creating DemoInstantiation<int>:\n\n";
    DemoInstantiation<int> demo;
    
    std::cout << "What gets compiled?\n";
    std::cout << "   ✅ always_compiled() - YES (virtual, in vtable)\n";
    std::cout << "   ✅ never_called_but_still_compiled() - YES (virtual, in vtable)\n";
    std::cout << "   ❌ only_if_used() - NO (not called yet)\n";
    std::cout << "   ❌ also_only_if_used() - NO (not called yet)\n\n";
    
    demo.always_compiled();
    
    std::cout << "\n💥 THE BLOAT:\n";
    std::cout << "   • never_called_but_still_compiled() is compiled\n";
    std::cout << "   • It's in the binary taking up space\n";
    std::cout << "   • But we never call it!\n";
    std::cout << "   • This is WASTED CODE\n\n";
    
    std::cout << "Now imagine:\n";
    std::cout << "   • 10 virtual functions\n";
    std::cout << "   • 10 template instantiations\n";
    std::cout << "   • = 100 compiled functions\n";
    std::cout << "   • If you only use 2-3 per type = 70-80 wasted functions!\n";
}

} // namespace the_problem

// ============================================================================
// SECTION 2: THE BAD EXAMPLE - C++ Core Guidelines T.80
// ============================================================================

namespace bad_example_t80 {

/*
 * ❌ BAD DESIGN: Template with virtual functions
 * 
 * This is the EXACT example from C++ Core Guidelines T.80 showing
 * what NOT to do when designing template hierarchies.
 */

template<typename T>
struct Container {         // an interface
    virtual T* get(int i) = 0;
    virtual T* first() = 0;
    virtual T* next() = 0;
    virtual void sort() = 0;
    virtual ~Container() = default;
};

template<typename T>
class Vector : public Container<T> {
private:
    std::vector<T> data_;
    typename std::vector<T>::iterator current_;
    
public:
    Vector() : current_(data_.begin()) {}
    
    T* get(int i) override {
        return (i >= 0 && i < static_cast<int>(data_.size())) ? &data_[i] : nullptr;
    }
    
    T* first() override {
        current_ = data_.begin();
        return current_ != data_.end() ? &(*current_) : nullptr;
    }
    
    T* next() override {
        if (current_ != data_.end()) ++current_;
        return current_ != data_.end() ? &(*current_) : nullptr;
    }
    
    void sort() override {
        std::sort(data_.begin(), data_.end());
    }
    
    void add(const T& item) { data_.push_back(item); }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "BAD EXAMPLE: C++ Core Guidelines T.80\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Code:\n";
    std::cout << "   template<typename T>\n";
    std::cout << "   struct Container {\n";
    std::cout << "       virtual T* get(int i);\n";
    std::cout << "       virtual T* first();\n";
    std::cout << "       virtual T* next();\n";
    std::cout << "       virtual void sort();\n";
    std::cout << "   };\n\n";
    
    std::cout << "Creating two instantiations:\n";
    std::cout << "   Vector<int> vi;\n";
    std::cout << "   Vector<string> vs;\n\n";
    
    Vector<int> vi;
    Vector<std::string> vs;
    
    std::cout << "⚠️  WHAT GETS COMPILED:\n\n";
    
    std::cout << "Vector<int>:\n";
    std::cout << "   • get(int) - compiled\n";
    std::cout << "   • first() - compiled\n";
    std::cout << "   • next() - compiled\n";
    std::cout << "   • sort() - compiled\n";
    std::cout << "   Even if you only use get()!\n\n";
    
    std::cout << "Vector<string>:\n";
    std::cout << "   • get(int) - compiled AGAIN\n";
    std::cout << "   • first() - compiled AGAIN\n";
    std::cout << "   • next() - compiled AGAIN\n";
    std::cout << "   • sort() - compiled AGAIN\n";
    std::cout << "   Complete duplication!\n\n";
    
    std::cout << "📊 CODE BLOAT CALCULATION:\n";
    std::cout << "   • 2 types × 4 virtual functions = 8 functions\n";
    std::cout << "   • 10 types × 10 virtual functions = 100 functions\n";
    std::cout << "   • Average function: 50-100 bytes\n";
    std::cout << "   • 100 functions × 75 bytes = 7.5 KB of bloat\n";
    std::cout << "   • In large projects: MEGABYTES of wasted code\n\n";
    
    std::cout << "❌ WHY THIS IS BAD:\n";
    std::cout << "   1. Binary size increases unnecessarily\n";
    std::cout << "   2. Longer compilation times\n";
    std::cout << "   3. More pressure on instruction cache\n";
    std::cout << "   4. Harder to maintain\n";
    std::cout << "   5. Embedded systems: wastes precious flash memory\n";
}

} // namespace bad_example_t80

// ============================================================================
// SECTION 3: WHY IT HAPPENS - THE VTABLE REQUIREMENT
// ============================================================================

namespace why_it_happens {

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "WHY VIRTUAL FUNCTIONS MUST BE INSTANTIATED\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "🔧 THE MECHANISM:\n\n";
    
    std::cout << "1. VTABLE CONSTRUCTION:\n";
    std::cout << "   • Each class with virtual functions has a vtable\n";
    std::cout << "   • vtable is built at COMPILE-TIME\n";
    std::cout << "   • vtable contains pointers to ALL virtual functions\n";
    std::cout << "   • Therefore, all virtual functions MUST exist\n\n";
    
    std::cout << "2. TEMPLATE INSTANTIATION:\n";
    std::cout << "   • Template<int> creates a new class type\n";
    std::cout << "   • This class needs its own vtable\n";
    std::cout << "   • Compiler must compile ALL virtual functions\n";
    std::cout << "   • Even those never called\n\n";
    
    std::cout << "3. COMPARISON:\n\n";
    
    std::cout << "   Non-virtual function:\n";
    std::cout << "   ✓ Compiler sees call site\n";
    std::cout << "   ✓ \"Oh, this function is used, compile it\"\n";
    std::cout << "   ✓ No call = no compilation (lazy)\n\n";
    
    std::cout << "   Virtual function:\n";
    std::cout << "   ✓ Compiler instantiates class\n";
    std::cout << "   ✓ \"Must build vtable for this class\"\n";
    std::cout << "   ✓ \"Need ALL virtual function addresses\"\n";
    std::cout << "   ✓ ALL compiled regardless of use (eager)\n\n";
    
    std::cout << "📝 STANDARD LIBRARY MISTAKE:\n";
    std::cout << "   • std::locale facets (std::ctype<T>) made this mistake\n";
    std::cout << "   • ~15 virtual functions per facet\n";
    std::cout << "   • Most programs use 2-3 of them\n";
    std::cout << "   • But all 15 are instantiated\n";
    std::cout << "   • Acknowledged in C++ Core Guidelines as historical error\n";
}

} // namespace why_it_happens

// ============================================================================
// SOLUTION 1: NON-TEMPLATE BASE CLASS (TYPE ERASURE)
// ============================================================================

namespace solution_type_erasure {

/*
 * ✅ GOOD: Move virtual interface to non-template base
 * 
 * Virtual functions compiled ONCE, not per template instantiation.
 * This is the T.84 guideline: "Use a non-template core implementation"
 */

// Non-template base - compiled ONCE
class ContainerBase {
public:
    virtual ~ContainerBase() = default;
    virtual void* get_impl(int i) = 0;
    virtual void* first_impl() = 0;
    virtual void* next_impl() = 0;
    virtual void sort_impl() = 0;
};

// Template provides type safety
template<typename T>
class Vector : public ContainerBase {
private:
    std::vector<T> data_;
    typename std::vector<T>::iterator current_;
    
public:
    Vector() : current_(data_.begin()) {}
    
    void* get_impl(int i) override {
        return (i >= 0 && i < static_cast<int>(data_.size())) ? &data_[i] : nullptr;
    }
    
    void* first_impl() override {
        current_ = data_.begin();
        return current_ != data_.end() ? static_cast<void*>(&(*current_)) : nullptr;
    }
    
    void* next_impl() override {
        if (current_ != data_.end()) ++current_;
        return current_ != data_.end() ? static_cast<void*>(&(*current_)) : nullptr;
    }
    
    void sort_impl() override {
        std::sort(data_.begin(), data_.end());
    }
    
    // Type-safe wrappers
    T* get(int i) { return static_cast<T*>(get_impl(i)); }
    T* first() { return static_cast<T*>(first_impl()); }
    T* next() { return static_cast<T*>(next_impl()); }
    void sort() { sort_impl(); }
    void add(const T& item) { data_.push_back(item); }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SOLUTION 1: Non-Template Base (Type Erasure)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    Vector<int> vi;
    Vector<std::string> vs;
    
    std::cout << "✅ FIXED DESIGN:\n\n";
    
    std::cout << "ContainerBase (non-template):\n";
    std::cout << "   • Virtual functions compiled ONCE\n";
    std::cout << "   • get_impl(), first_impl(), next_impl(), sort_impl()\n";
    std::cout << "   • Shared across ALL instantiations\n\n";
    
    std::cout << "Vector<T> (template):\n";
    std::cout << "   • Overrides virtual functions (minimal code)\n";
    std::cout << "   • Type-specific logic only\n";
    std::cout << "   • Type-safe wrapper functions\n\n";
    
    std::cout << "📉 BLOAT REDUCTION:\n";
    std::cout << "   • 10 types × 4 virtuals = 4 base + 10 overrides\n";
    std::cout << "   • vs Bad design: 40 separate functions\n";
    std::cout << "   • Reduction: 50-80% less code\n\n";
    
    std::cout << "⚖️  TRADE-OFFS:\n";
    std::cout << "   ✅ Massive code size reduction\n";
    std::cout << "   ✅ Runtime polymorphism preserved\n";
    std::cout << "   ⚠️  Slight runtime overhead (void* casts)\n";
    std::cout << "   ⚠️  Loss of type information in base\n";
}

} // namespace solution_type_erasure

// ============================================================================
// SOLUTION 2: CRTP - NO VIRTUAL FUNCTIONS
// ============================================================================

namespace solution_crtp {

/*
 * ✅ BETTER: CRTP for compile-time polymorphism
 * 
 * No vtable = No forced instantiation = No bloat
 * Zero runtime overhead
 */

template<typename Derived>
class ContainerCRTP {
public:
    auto* get(int i) { return derived()->get_impl(i); }
    auto* first() { return derived()->first_impl(); }
    auto* next() { return derived()->next_impl(); }
    void sort() { derived()->sort_impl(); }
    
private:
    Derived* derived() { return static_cast<Derived*>(this); }
};

template<typename T>
class Vector : public ContainerCRTP<Vector<T>> {
private:
    std::vector<T> data_;
    typename std::vector<T>::iterator current_;
    
public:
    Vector() : current_(data_.begin()) {}
    
    T* get_impl(int i) {
        return (i >= 0 && i < static_cast<int>(data_.size())) ? &data_[i] : nullptr;
    }
    
    T* first_impl() {
        current_ = data_.begin();
        return current_ != data_.end() ? &(*current_) : nullptr;
    }
    
    T* next_impl() {
        if (current_ != data_.end()) ++current_;
        return current_ != data_.end() ? &(*current_) : nullptr;
    }
    
    void sort_impl() {
        std::sort(data_.begin(), data_.end());
    }
    
    void add(const T& item) { data_.push_back(item); }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SOLUTION 2: CRTP (Static Polymorphism)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    Vector<int> vi;
    vi.add(5); vi.add(3); vi.add(9);
    
    std::cout << "✅ NO VIRTUAL FUNCTIONS:\n\n";
    
    std::cout << "Benefits:\n";
    std::cout << "   • No vtable\n";
    std::cout << "   • No vptr (8 bytes saved per object)\n";
    std::cout << "   • Functions ONLY compiled if called\n";
    std::cout << "   • Can be fully inlined\n";
    std::cout << "   • Zero runtime overhead\n\n";
    
    vi.sort();
    
    std::cout << "What got compiled:\n";
    std::cout << "   ✓ sort_impl<int> - used\n";
    std::cout << "   ✓ add - used\n";
    std::cout << "   ✗ get_impl - NOT compiled (not used)\n";
    std::cout << "   ✗ first_impl - NOT compiled (not used)\n";
    std::cout << "   ✗ next_impl - NOT compiled (not used)\n\n";
    
    std::cout << "📊 BLOAT ELIMINATION:\n";
    std::cout << "   • Only used functions compiled\n";
    std::cout << "   • Lazy instantiation = minimal code\n";
    std::cout << "   • Perfect for performance-critical code\n\n";
    
    std::cout << "⚖️  TRADE-OFFS:\n";
    std::cout << "   ✅ Zero code bloat\n";
    std::cout << "   ✅ Zero runtime overhead\n";
    std::cout << "   ❌ No runtime polymorphism\n";
    std::cout << "   ❌ Cannot use heterogeneous containers\n";
}

} // namespace solution_crtp

// ============================================================================
// SOLUTION 3: C++20 CONCEPTS
// ============================================================================

namespace solution_concepts {

#if __cplusplus >= 202002L

template<typename T>
concept Container = requires(T c, int i) {
    { c.get(i) };
    { c.first() };
    { c.next() };
    { c.sort() } -> std::same_as<void>;
};

template<typename T>
class Vector {
private:
    std::vector<T> data_;
    typename std::vector<T>::iterator current_;
    
public:
    Vector() : current_(data_.begin()) {}
    
    T* get(int i) {
        return (i >= 0 && i < static_cast<int>(data_.size())) ? &data_[i] : nullptr;
    }
    
    T* first() {
        current_ = data_.begin();
        return current_ != data_.end() ? &(*current_) : nullptr;
    }
    
    T* next() {
        if (current_ != data_.end()) ++current_;
        return current_ != data_.end() ? &(*current_) : nullptr;
    }
    
    void sort() { std::sort(data_.begin(), data_.end()); }
    void add(const T& item) { data_.push_back(item); }
};

template<Container C>
void process(C& cont) {
    cont.sort();
}

#endif

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SOLUTION 3: C++20 Concepts\n";
    std::cout << std::string(70, '=') << "\n\n";
    
#if __cplusplus >= 202002L
    Vector<int> vi;
    vi.add(5);
    process(vi);
    
    std::cout << "✅ NO INHERITANCE, NO VIRTUAL:\n\n";
    
    std::cout << "Concepts provide:\n";
    std::cout << "   • Compile-time interface checking\n";
    std::cout << "   • Duck typing with type safety\n";
    std::cout << "   • No base class needed\n";
    std::cout << "   • Better error messages\n";
    std::cout << "   • Zero runtime overhead\n\n";
    
    std::cout << "📊 BLOAT ELIMINATION:\n";
    std::cout << "   • No virtual functions at all\n";
    std::cout << "   • Only used functions instantiated\n";
    std::cout << "   • Cleanest solution\n";
#else
    std::cout << "⚠️  C++20 concepts not available.\n";
    std::cout << "   Compile with -std=c++20 to see this solution.\n";
#endif
}

} // namespace solution_concepts

// ============================================================================
// GUIDELINES AND RECOMMENDATIONS
// ============================================================================

namespace guidelines {

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "GUIDELINES: AVOID VIRTUAL FUNCTIONS IN TEMPLATES\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "🚫 AVOID THIS:\n\n";
    std::cout << "   template<typename T>\n";
    std::cout << "   class Container {\n";
    std::cout << "       virtual void operation();  // BAD!\n";
    std::cout << "   };\n\n";
    
    std::cout << "❌ PROBLEMS:\n";
    std::cout << "   1. CODE BLOAT: All virtuals instantiated every time\n";
    std::cout << "   2. COMPILATION: Slower, more memory\n";
    std::cout << "   3. BINARY SIZE: Can add megabytes in large projects\n";
    std::cout << "   4. PERFORMANCE: More pressure on I-cache\n";
    std::cout << "   5. MAINTENANCE: Harder to track what's compiled\n\n";
    
    std::cout << "⚠️  WHEN IT'S A PROBLEM:\n";
    std::cout << "   • Many virtual functions (5+)\n";
    std::cout << "   • Many template instantiations (10+)\n";
    std::cout << "   • Large codebase\n";
    std::cout << "   • Embedded systems (limited flash)\n";
    std::cout << "   • Header-only libraries\n\n";
    
    std::cout << "✅ SOLUTIONS (IN ORDER OF PREFERENCE):\n\n";
    
    std::cout << "1. C++20 CONCEPTS (best)\n";
    std::cout << "   • No inheritance, no virtual\n";
    std::cout << "   • Compile-time duck typing\n";
    std::cout << "   • Zero overhead\n\n";
    
    std::cout << "2. CRTP (for static polymorphism)\n";
    std::cout << "   • No virtual functions\n";
    std::cout << "   • Lazy instantiation\n";
    std::cout << "   • Zero runtime overhead\n\n";
    
    std::cout << "3. NON-TEMPLATE BASE (for runtime polymorphism)\n";
    std::cout << "   • Virtual compiled once\n";
    std::cout << "   • Massive bloat reduction\n";
    std::cout << "   • Runtime flexibility preserved\n\n";
    
    std::cout << "✓ WHEN VIRTUAL IN TEMPLATES IS OK:\n";
    std::cout << "   • Very few virtual functions (1-2)\n";
    std::cout << "   • Few instantiations (2-3 types)\n";
    std::cout << "   • Virtual functions actually needed\n";
    std::cout << "   • Code clarity more important than size\n\n";
    
    std::cout << "📚 RELATED GUIDELINES:\n";
    std::cout << "   • T.5: Combine generic and OO judiciously\n";
    std::cout << "   • T.80: Do not naively templatize a hierarchy\n";
    std::cout << "   • T.83: Do not declare member function template virtual\n";
    std::cout << "   • T.84: Use a non-template core implementation\n";
}

} // namespace guidelines

// ============================================================================
// MAIN
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║      VIRTUAL FUNCTIONS IN TEMPLATES: THE CODE BLOAT PROBLEM       ║\n";
    std::cout << "║                                                                    ║\n";
    std::cout << "║          WHY TO AVOID & BETTER ALTERNATIVES                        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    the_problem::demonstrate();
    bad_example_t80::demonstrate();
    why_it_happens::demonstrate();
    solution_type_erasure::demonstrate();
    solution_crtp::demonstrate();
    solution_concepts::demonstrate();
    guidelines::demonstrate();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "KEY TAKEAWAYS\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "❌ THE PROBLEM:\n";
    std::cout << "   • Virtual functions in templates = code bloat\n";
    std::cout << "   • ALL virtuals instantiated for EVERY type\n";
    std::cout << "   • Even if never called\n";
    std::cout << "   • Can add megabytes to binary size\n\n";
    
    std::cout << "🔍 WHY IT HAPPENS:\n";
    std::cout << "   • vtable must be built at compile-time\n";
    std::cout << "   • vtable needs ALL virtual function addresses\n";
    std::cout << "   • Compiler must compile them all\n";
    std::cout << "   • Non-virtual functions use lazy instantiation\n\n";
    
    std::cout << "✅ SOLUTIONS:\n";
    std::cout << "   1. C++20 Concepts - no inheritance needed\n";
    std::cout << "   2. CRTP - compile-time polymorphism\n";
    std::cout << "   3. Non-template base - runtime polymorphism\n\n";
    
    std::cout << "🎯 RECOMMENDATION:\n";
    std::cout << "   AVOID virtual functions in class templates\n";
    std::cout << "   unless you have a compelling reason and few instantiations.\n\n";
    
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║        Follow C++ Core Guidelines T.80 and T.84                   ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    return 0;
}
