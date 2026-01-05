# Modern C++ Examples - Comprehensive Index

**Last Updated:** January 3, 2026

This index maps C++ concepts, keywords, features, design patterns, problems, and standards to specific example files in this repository. Use this as a quick reference to find examples for specific topics.

---

## Table of Contents

- [Interview Preparation](#interview-preparation)
- [C++ Standards Features](#c-standards-features)
- [Design Patterns & Idioms](#design-patterns--idioms)
- [Object-Oriented Programming](#object-oriented-programming)
- [Memory Management](#memory-management)
- [Concurrency & Parallelism](#concurrency--parallelism)
- [Real-Time & Embedded Systems](#real-time--embedded-systems)
- [Safety-Critical & Standards](#safety-critical--standards)
- [STL Containers & Algorithms](#stl-containers--algorithms)
- [Template Metaprogramming](#template-metaprogramming)
- [Error Handling](#error-handling)
- [Interoperability](#interoperability)
- [Performance & Optimization](#performance--optimization)
- [Common Problems & Solutions](#common-problems--solutions)
- [Keywords & Language Features](#keywords--language-features)

---

## Interview Preparation

### Complete C++ Interview Guide
- **File:** [MockInterview.cpp](src/MockInterview.cpp)
- **Description:** Comprehensive collection of C++ interview questions with detailed, runnable answers
- **Topics Covered:**
  - **Fundamentals:** Pointers vs references, const correctness, RAII pattern
  - **Memory Management:** Smart pointers (unique_ptr, shared_ptr, weak_ptr), custom deleters, aliasing constructor, enable_shared_from_this, memory alignment, custom allocators
  - **OOP & Design Patterns:** Virtual dispatch mechanics, vtable internals, multiple inheritance, CRTP with mixins, PIMPL idiom, compile-time polymorphism
  - **Templates & Metaprogramming:** SFINAE techniques, enable_if, detection idiom, variadic templates, fold expressions, compile-time computation, constexpr/consteval
  - **Concurrency:** Thread-safe singleton (Meyer's, call_once), producer-consumer pattern, condition variables, atomic operations, memory ordering
  - **Performance:** **Enhanced move semantics with comprehensive 7-part educational guide**, copy elision (RVO/NRVO), cache-friendly design, branch prediction optimization
- **Move Semantics Deep Dive (Educational):**
  - Part 1: Basic string class with move constructor and move assignment operator
  - Part 2: Understanding value categories (lvalues vs rvalues)
  - Part 3: Rule of Five complete implementation with ResourceManager class
  - Part 4: Performance comparison with timing measurements (copy vs move)
  - Part 5: Common pitfalls (moving from const, using moved-from objects, unnecessary std::move on returns, forgetting noexcept)
  - Part 6: Perfect forwarding with universal references and std::forward
  - Part 7: Comprehensive summary with key takeaways and best practices
- **Common Interview Questions:**
  - What's the difference between pointers and references?
  - Explain smart pointers and when to use each type
  - How does virtual dispatch work internally?
  - What is SFINAE and how is it used?
  - How do you implement a thread-safe singleton?
  - What's the difference between RVO and move semantics?
  - Explain memory ordering in atomic operations
  - How does CRTP provide compile-time polymorphism?
  - **How do you implement move semantics? (Complete educational implementation included)**
- **Format:** Each section includes working code examples with detailed explanations and best practices

**[⬆ Back to Top](#table-of-contents)**

---

## C++ Standards Features

### C++11
- **File:** [Cpp11Examples.cpp](src/Cpp11Examples.cpp)
- **Topics:** `auto`, `nullptr`, range-based for, lambda expressions, smart pointers, move semantics, `constexpr`, `static_assert`, uniform initialization, `decltype`, `std::array`, `std::thread`, `std::mutex`

### C++14
- **File:** [Cpp14Examples.cpp](src/Cpp14Examples.cpp)
- **Topics:** Generic lambdas, `auto` return type deduction, binary literals, digit separators, `std::make_unique`, relaxed `constexpr`, variable templates
- **File:** [GenericLambdas.cpp](src/GenericLambdas.cpp)
- **Topics:** Generic lambda expressions with `auto` parameters

### C++17
- **File:** [Cpp17Examples.cpp](src/Cpp17Examples.cpp)
- **Topics:** Structured bindings, `if constexpr`, fold expressions, `std::optional`, `std::variant`, `std::any`, `std::string_view`, parallel algorithms, `std::filesystem`
- **File:** [Cpp17Concurrency.cpp](src/Cpp17Concurrency.cpp)
- **Topics:** Parallel STL algorithms, execution policies
- **File:** [StructuredBindings.cpp](src/StructuredBindings.cpp)
- **Topics:** Structured bindings syntax and use cases
- **File:** [OptionalExamples.cpp](src/OptionalExamples.cpp)
- **Topics:** `std::optional` usage patterns

### C++20
- **File:** [Cpp20Examples.cpp](src/Cpp20Examples.cpp)
- **Topics:** Concepts, ranges, coroutines, modules, three-way comparison (`<=>`), designated initializers, `std::span`, `consteval`, `constinit`
- **File:** [ConceptsExamples.cpp](src/ConceptsExamples.cpp)
- **Topics:** Concepts, constraints, `requires` clauses
- **File:** [RangesExamples.cpp](src/RangesExamples.cpp)
- **Topics:** Ranges library, views, adaptors, pipelines
- **File:** [CameraModule.cppm](src/CameraModule.cppm) / [TemplatedCameraModules.cpp](src/TemplatedCameraModules.cpp)
- **Topics:** C++20 modules

### C++23
- **File:** [Cpp23Examples.cpp](src/Cpp23Examples.cpp)
- **Topics:** `std::expected`, deducing `this`, `if consteval`, multidimensional subscript operator, `std::print`, `std::stacktrace`

**[⬆ Back to Top](#table-of-contents)**

---

## Design Patterns & Idioms

### CRTP (Curiously Recurring Template Pattern)
- **File:** [CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp)
- **Topics:** CRTP vs virtual functions, static polymorphism, performance comparison, vtable overhead

### Pimpl (Pointer to Implementation)
- **File:** [PimplIdiom.cpp](src/PimplIdiom.cpp)
- **Topics:** Pimpl idiom, ABI stability, compilation firewall, real-time concerns, cache locality, FastPimpl alternative

### NVI (Non-Virtual Interface)
- **File:** [NVIIdiomTemplateMethod.cpp](src/NVIIdiomTemplateMethod.cpp)
- **Topics:** Non-virtual interface idiom, template method pattern, Herb Sutter's virtuality guidelines

### RAII (Resource Acquisition Is Initialization)
- **File:** [RuleOf3_5_0.cpp](src/RuleOf3_5_0.cpp)
- **Topics:** Rule of 3/5/0, RAII, copy/move semantics, special member functions
- **File:** [ResourceLeaks.cpp](src/ResourceLeaks.cpp)
- **Topics:** Preventing resource leaks with RAII

### Dependency Injection
- **File:** [DependencyInjection.cpp](src/DependencyInjection.cpp)
- **Topics:** Constructor injection, setter injection, interface injection, composition over inheritance

### SOLID Principles
- **File:** [SOLIDPrinciples.cpp](src/SOLIDPrinciples.cpp)
- **Topics:** Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion

### Factory Pattern
- **File:** [CreatingCApiFromCpp.cpp](src/CreatingCApiFromCpp.cpp)
- **Topics:** Factory functions for C API

**[⬆ Back to Top](#table-of-contents)**

---

## Object-Oriented Programming

### Inheritance
- **File:** [InheritanceTypes.cpp](src/InheritanceTypes.cpp)
- **Topics:** Public, private, protected inheritance, IS-A vs implemented-in-terms-of
- **File:** [DiamondProblem.cpp](src/DiamondProblem.cpp)
- **Topics:** Diamond problem, virtual inheritance, composition alternative
- **File:** [FragileBaseClass.cpp](src/FragileBaseClass.cpp)
- **Topics:** Fragile base class problem, ABI stability, solutions

### Polymorphism
- **File:** [RuntimePolymorphism.cpp](src/RuntimePolymorphism.cpp)
- **Topics:** Virtual functions, pure virtual functions, abstract classes, dynamic polymorphism
- **File:** [VirtualFunctionsInTemplates.cpp](src/VirtualFunctionsInTemplates.cpp)
- **Topics:** Why virtual functions in templates cause code bloat, type erasure, CRTP, concepts as alternatives
- **File:** [CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp)
- **Topics:** Static vs dynamic polymorphism comparison

### Object Slicing
- **File:** [ObjectSlicingCpp20.cpp](src/ObjectSlicingCpp20.cpp)
- **Topics:** Object slicing problem, C++20 solutions
- **File:** [ObjectSlicingSmartPtr.cpp](src/ObjectSlicingSmartPtr.cpp)
- **Topics:** Preventing object slicing with smart pointers

**[⬆ Back to Top](#table-of-contents)**

---

## Memory Management

### Smart Pointers
- **File:** [RuleOf3_5_0.cpp](src/RuleOf3_5_0.cpp)
- **Topics:** `unique_ptr`, `shared_ptr`, `weak_ptr`, ownership semantics
- **File:** [ResourceLeaks.cpp](src/ResourceLeaks.cpp)
- **Topics:** Smart pointers for preventing leaks
- **File:** [ObjectSlicingSmartPtr.cpp](src/ObjectSlicingSmartPtr.cpp)
- **Topics:** Smart pointers with polymorphism

### Universal Resource Management with Custom Deleters
- **File:** [UniversalResourceManager.cpp](src/UniversalResourceManager.cpp)
- **Topics:** Custom deleters for RAII, universal resource management beyond memory
- **Description:** Comprehensive guide to using smart pointers with custom deleters for managing non-memory resources (files, sockets, databases, GPU resources, shared memory, resource pools). Demonstrates 8 real-world examples, 10 common pitfalls with solutions, deleter patterns (lambdas, function objects, stateful deleters), decision tree for choosing unique_ptr vs shared_ptr, and simplified RAII patterns
- **Key Examples:**
  - FILE* management with `fclose()` deleter
  - Database connection lifecycle with RAII
  - Socket/network resource cleanup
  - GPU resource management (buffers, textures)
  - Shared memory mapping/unmapping
  - Resource pool integration with custom deleters
  - Debug allocators with statistics tracking
  - Composite deleters for multi-step cleanup
- **Pitfalls Covered:** Type bloat, size overhead, nullptr checks, exception safety, dangling references, double delete, stack vs heap, performance implications, allocation mismatch, thread safety
- **Design Patterns:** Lambdas, stateful lambdas, function objects, function pointers, templated factory functions, RAII wrapper templates

### Move Semantics
- **File:** [MoveSemantics.cpp](src/MoveSemantics.cpp)
- **Topics:** Rvalue references, move constructors, move assignment, perfect forwarding
- **File:** [MoveSematicsPerfectForwarding.cpp](src/MoveSematicsPerfectForwarding.cpp)
- **Topics:** Move semantics and perfect forwarding patterns
- **File:** [PerfectForwardingAndRequires.cpp](src/PerfectForwardingAndRequires.cpp)
- **Topics:** Perfect forwarding with `std::forward`, requires clauses

### Memory Pools & Allocators
- **File:** [RealTimeProgramming.cpp](src/RealTimeProgramming.cpp)
- **Topics:** Memory pools, custom allocators, pre-allocation strategies
- **File:** [STLContainersNoHeap.cpp](src/STLContainersNoHeap.cpp)
- **Topics:** Custom allocators, stack allocators, pool allocators

### Resource Management
- **File:** [ResourceLeaks.cpp](src/ResourceLeaks.cpp)
- **Topics:** Common resource leak patterns and prevention

**[⬆ Back to Top](#table-of-contents)**

---

## Concurrency & Parallelism

### Threading
- **File:** [AsioAndModernCppConcurrency.cpp](src/AsioAndModernCppConcurrency.cpp)
- **Topics:** ASIO, thread pools, async operations, io_context
- **File:** [AsioMultipleContexts.cpp](src/AsioMultipleContexts.cpp)
- **Topics:** Multiple io_context patterns, thread distribution
- **File:** [ThreadPoolExamples.cpp](src/ThreadPoolExamples.cpp)
- **Topics:** Thread pool implementation patterns

### Parallel Algorithms
- **File:** [Cpp17Concurrency.cpp](src/Cpp17Concurrency.cpp)
- **Topics:** Parallel STL, execution policies (`std::execution::par`)

### Future/Promise/Async
- **File:** [FuturePromiseAsync.cpp](src/FuturePromiseAsync.cpp)
- **Topics:** `std::future`, `std::promise`, `std::async`, `std::packaged_task`, ASIO relationship

### Stop Tokens
- **File:** [StopTokenExample.cpp](src/StopTokenExample.cpp)
- **Topics:** `std::stop_token`, `std::stop_source`, cooperative cancellation (C++20)

### Microservices
- **File:** [MultiThreadedMicroservices.cpp](src/MultiThreadedMicroservices.cpp)
- **Topics:** Multi-threaded microservice architecture

**[⬆ Back to Top](#table-of-contents)**

---

## Real-Time & Embedded Systems

### Real-Time Programming
- **File:** [RealTimeProgramming.cpp](src/RealTimeProgramming.cpp)
- **Topics:** Deterministic behavior, WCET, Big O notation, memory pre-allocation, `std::bitset`, `std::list::splice()`, real-time patterns

### Embedded Systems
- **File:** [EmbeddedSystemsProgramming.cpp](src/EmbeddedSystemsProgramming.cpp)
- **Topics:** Best practices for embedded systems, constexpr, fixed-size containers
- **File:** [EmbeddedThreadSafetyReentrancy.cpp](src/EmbeddedThreadSafetyReentrancy.cpp)
- **Topics:** Thread-safety and re-entrancy for embedded/firmware systems, ISR-safe code, re-entrant functions, atomic operations, lock-free circular buffers, critical sections, volatile keyword, double buffering, ISR patterns, UART driver example
- **Concepts:** Re-entrant vs thread-safe vs ISR-safe, interrupt service routines, memory-mapped I/O, bare-metal programming, RTOS patterns
- **File:** [EmbeddedSystemsAvoid.cpp](src/EmbeddedSystemsAvoid.cpp)
- **Topics:** What to avoid in embedded systems (exceptions, dynamic allocation, iostream)
- **File:** [FunctionalSafetyCppSTL.cpp](src/FunctionalSafetyCppSTL.cpp)
- **Topics:** STL usage on STM32 MCUs vs MPs, functional safety (IEC-61508 SIL-3, ISO 26262 ASIL-D), MISRA/AUTOSAR compliance
- **See Also:** Safety-Critical & Standards section for detailed coverage

### ARM & Architecture
- **File:** [ARMInstructionSets.cpp](src/ARMInstructionSets.cpp)
- **Topics:** ARM instruction sets, NEON, SVE, architecture-specific optimization

### ROM Placement
- **File:** [ROMability.cpp](src/ROMability.cpp)
- **Topics:** `constexpr`, `consteval`, `constinit`, ROM placement strategies

**[⬆ Back to Top](#table-of-contents)**

---

## Safety-Critical & Standards

### Functional Safety and STL Usage on STM32
- **File:** [FunctionalSafetyCppSTL.cpp](src/FunctionalSafetyCppSTL.cpp)
- **Description:** Comprehensive guide to C++ and STL usage across different STM32 platforms and safety levels
- **Topics Covered:**
  - **STM32 MCUs (Cortex-M, no MMU):** Safe STL subset, bounded allocation, memory constraints
  - **STM32MP (Cortex-A with MMU):** Full STL support, Linux-based development
  - **IEC-61508 SIL-3:** Functional safety requirements, prohibited features, compliant patterns
  - **ISO 26262 ASIL-D:** Automotive safety, AUTOSAR C++14 compliance
  - **Comparison Tables:** Feature availability across platforms and safety levels
  - **Practical Examples:** SIL-3 compliant error handling, CRTP for safety-critical systems, ASIL-D brake controller
- **Key Concepts:** MISRA C++, AUTOSAR C++, heap allocation restrictions, static polymorphism, bounded containers, deterministic behavior
- **When to Use:** Understanding STL limitations in embedded/safety-critical contexts, choosing appropriate C++ features for your platform

### MISRA C++
- **File:** [MISRACppDemo.cpp](src/MISRACppDemo.cpp)
- **Topics:** MISRA C++ rules, safety-critical coding guidelines

### ISO 26262 (Automotive)
- **File:** [FunctionalSafetyISO26262.cpp](src/FunctionalSafetyISO26262.cpp)
- **Topics:** ISO 26262 functional safety, ASIL levels, automotive safety

### STL for Safety-Critical
- **File:** [SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp)
- **Topics:** Which STL containers to avoid/use in safety-critical systems, MISRA compliance, AUTOSAR, heap allocation issues

### AUTOSAR
- **File:** [SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp)
- **Topics:** AUTOSAR C++14 guidelines, container usage

**[⬆ Back to Top](#table-of-contents)**

---

## STL Containers & Algorithms

### Container Usage
- **File:** [STLContainersNoHeap.cpp](src/STLContainersNoHeap.cpp)
- **Topics:** Using STL containers without heap, custom allocators, `std::array`, `std::span`
- **File:** [SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp)
- **Topics:** Container safety analysis, forbidden containers (list, map, set, unordered_*)

### std::bitset
- **File:** [RealTimeProgramming.cpp](src/RealTimeProgramming.cpp)
- **Topics:** `std::bitset` vs `std::vector<bool>`, real-time usage

### std::list::splice()
- **File:** [RealTimeProgramming.cpp](src/RealTimeProgramming.cpp)
- **Topics:** O(1) element movement, splice operations, real-time patterns

### Algorithms
- **File:** [BinarySearch.cpp](src/BinarySearch.cpp)
- **Topics:** Binary search algorithms, STL algorithm usage

**[⬆ Back to Top](#table-of-contents)**

---

## Template Metaprogramming

### Templates & Concepts
- **File:** [ConceptsExamples.cpp](src/ConceptsExamples.cpp)
- **Topics:** C++20 concepts, type constraints, `requires` clauses
- **File:** [PerfectForwardingAndRequires.cpp](src/PerfectForwardingAndRequires.cpp)
- **Topics:** Perfect forwarding with concepts

### CRTP (Static Polymorphism)
- **File:** [CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp)
- **Topics:** Curiously Recurring Template Pattern, compile-time polymorphism

### Variadic Templates
- **File:** [VariadicTemplateRecursion.cpp](src/VariadicTemplateRecursion.cpp)
- **Topics:** Variadic templates, template recursion, parameter packs

### Template Interface
- **File:** [TemplatedCameraInterface.cpp](src/TemplatedCameraInterface.cpp)
- **Topics:** Generic templated interfaces

**[⬆ Back to Top](#table-of-contents)**

---

## Error Handling

### Exceptions
- **File:** [ErrorHandling.cpp](src/ErrorHandling.cpp)
- **Topics:** Exception handling patterns, custom exceptions
- **File:** [AdvancedExceptionHandling.cpp](src/AdvancedExceptionHandling.cpp)
- **Topics:** Advanced exception techniques, exception safety guarantees
- **File:** [ExceptionWithSourceLocation.cpp](src/ExceptionWithSourceLocation.cpp)
- **Topics:** `std::source_location` (C++20), exception context
- **File:** [ErrorHandlingStroustrup.cpp](src/ErrorHandlingStroustrup.cpp)
- **Topics:** Bjarne Stroustrup's error handling guidelines

### noexcept
- **File:** [NoexceptBestPractices.cpp](src/NoexceptBestPractices.cpp)
- **Topics:** `noexcept` specifier, move operations, best practices

### Error Codes & Optional
- **File:** [OptionalExamples.cpp](src/OptionalExamples.cpp)
- **Topics:** `std::optional` for error handling
- **File:** [Cpp23Examples.cpp](src/Cpp23Examples.cpp)
- **Topics:** `std::expected` for error handling (C++23)

**[⬆ Back to Top](#table-of-contents)**

---

## Interoperability

### C Interop
- **File:** [CppWrappingCLibrary.cpp](src/CppWrappingCLibrary.cpp)
- **Topics:** Wrapping C libraries in C++, RAII for C resources
- **File:** [CreatingCApiFromCpp.cpp](src/CreatingCApiFromCpp.cpp)
- **Topics:** Creating C API from C++ code, `extern "C"`, opaque pointers

### Python Binding
- **File:** [Pybind11Example.cpp](src/Pybind11Example.cpp)
- **Topics:** pybind11, exposing C++ to Python, automatic binding generation

### Protobuf
- **File:** [ProtobufExample.cpp](src/ProtobufExample.cpp)
- **Topics:** Protocol Buffers, serialization, cross-language data exchange

### JSON
- **File:** [NlohmannJsonExample.cpp](src/NlohmannJsonExample.cpp)
- **Topics:** nlohmann/json library, JSON parsing and serialization

### REST API
- **File:** [RestApiExample.cpp](src/RestApiExample.cpp)
- **Topics:** REST API client, HTTP requests, CURL integration

**[⬆ Back to Top](#table-of-contents)**

---

## Performance & Optimization

### Performance Comparison
- **File:** [CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp)
- **Topics:** CRTP vs virtual functions performance, vtable overhead, benchmarking
- **File:** [PimplIdiom.cpp](src/PimplIdiom.cpp)
- **Topics:** Pimpl performance impact, cache locality, indirection overhead

### Cache Locality
- **File:** [PimplIdiom.cpp](src/PimplIdiom.cpp)
- **Topics:** Cache-friendly design, avoiding pointer indirection
- **File:** [RealTimeProgramming.cpp](src/RealTimeProgramming.cpp)
- **Topics:** Memory layout for real-time systems

### Compile-Time Computation
- **File:** [ROMability.cpp](src/ROMability.cpp)
- **Topics:** `constexpr`, `consteval`, compile-time evaluation

**[⬆ Back to Top](#table-of-contents)**

---

## Common Problems & Solutions

### Fragile Base Class
- **File:** [FragileBaseClass.cpp](src/FragileBaseClass.cpp)
- **Topics:** Fragile base class problem, ABI breaks, composition over inheritance

### Diamond Problem
- **File:** [DiamondProblem.cpp](src/DiamondProblem.cpp)
- **Topics:** Multiple inheritance diamond problem, virtual inheritance solution

### Object Slicing
- **File:** [ObjectSlicingCpp20.cpp](src/ObjectSlicingCpp20.cpp)
- **Topics:** Object slicing, prevention strategies
- **File:** [ObjectSlicingSmartPtr.cpp](src/ObjectSlicingSmartPtr.cpp)
- **Topics:** Smart pointers to prevent slicing

### Resource Leaks
- **File:** [ResourceLeaks.cpp](src/ResourceLeaks.cpp)
- **Topics:** Memory leaks, file handle leaks, RAII solutions

### Virtual Functions in Templates
- **File:** [VirtualFunctionsInTemplates.cpp](src/VirtualFunctionsInTemplates.cpp)
- **Topics:** Code bloat from virtual functions in templates, C++ Core Guidelines T.80, T.83, T.84

**[⬆ Back to Top](#table-of-contents)**

---

## Keywords & Language Features

### Keywords

#### `auto`
- [Cpp11Examples.cpp](src/Cpp11Examples.cpp) - C++11 auto keyword
- [Cpp14Examples.cpp](src/Cpp14Examples.cpp) - C++14 auto return type
- [GenericLambdas.cpp](src/GenericLambdas.cpp) - Generic lambdas with auto

#### `constexpr` / `consteval` / `constinit`
- [Cpp11Examples.cpp](src/Cpp11Examples.cpp) - C++11 constexpr
- [Cpp14Examples.cpp](src/Cpp14Examples.cpp) - C++14 relaxed constexpr
- [Cpp20Examples.cpp](src/Cpp20Examples.cpp) - C++20 consteval, constinit
- [ROMability.cpp](src/ROMability.cpp) - ROM placement with const* keywords

#### `virtual`
- [RuntimePolymorphism.cpp](src/RuntimePolymorphism.cpp) - Virtual functions basics
- [CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp) - Virtual vs CRTP
- [VirtualFunctionsInTemplates.cpp](src/VirtualFunctionsInTemplates.cpp) - Virtual in templates problem
- [NVIIdiomTemplateMethod.cpp](src/NVIIdiomTemplateMethod.cpp) - Virtual in NVI pattern

#### `noexcept`
- [NoexceptBestPractices.cpp](src/NoexceptBestPractices.cpp) - noexcept best practices
- [MoveSemantics.cpp](src/MoveSemantics.cpp) - noexcept move operations

#### `requires` (Concepts)
- [ConceptsExamples.cpp](src/ConceptsExamples.cpp) - Requires clauses
- [PerfectForwardingAndRequires.cpp](src/PerfectForwardingAndRequires.cpp) - Requires with forwarding

#### `final`
- [FragileBaseClass.cpp](src/FragileBaseClass.cpp) - Final keyword usage

#### `override`
- [RuntimePolymorphism.cpp](src/RuntimePolymorphism.cpp) - Override specifier

#### `nullptr`
- [Cpp11Examples.cpp](src/Cpp11Examples.cpp) - nullptr vs NULL

#### `decltype`
- [Cpp11Examples.cpp](src/Cpp11Examples.cpp) - decltype usage
- [PerfectForwardingAndRequires.cpp](src/PerfectForwardingAndRequires.cpp) - decltype with forwarding

**[⬆ Back to Top](#table-of-contents)**

---

## Event-Driven Programming

### Event Systems
- **File:** [EventDrivenProgramming_Lambdas.cpp](src/EventDrivenProgramming_Lambdas.cpp)
- **Topics:** Signals/slots with lambdas, observer pattern
- **File:** [EventDrivenProgramming_Inheritance.cpp](src/EventDrivenProgramming_Inheritance.cpp)
- **Topics:** Event systems with inheritance

**[⬆ Back to Top](#table-of-contents)**

---

## Lambda Expressions

### Lambda Basics
- **File:** [Cpp11Examples.cpp](src/Cpp11Examples.cpp)
- **Topics:** Basic lambda syntax (C++11)
- **File:** [GenericLambdas.cpp](src/GenericLambdas.cpp)
- **Topics:** Generic lambdas with auto parameters (C++14)
- **File:** [LambdaCaptures.cpp](src/LambdaCaptures.cpp)
- **Topics:** Lambda capture modes, mutable lambdas

**[⬆ Back to Top](#table-of-contents)**

---

## Data Structures & Algorithms

### Linked Lists
- **File:** [SinglyLinkedList.cpp](src/SinglyLinkedList.cpp)
- **Topics:** Singly linked list implementation
- **File:** [InsertAndDeleteNodes.cpp](src/InsertAndDeleteNodes.cpp)
- **Topics:** Node insertion and deletion
- **File:** [FindFirstCommonNode.cpp](src/FindFirstCommonNode.cpp)
- **Topics:** Finding common node in linked lists
- **File:** [FindCountOfCommonNodes.cpp](src/FindCountOfCommonNodes.cpp)
- **Topics:** Counting common nodes
- **File:** [FindMToLastElement.cpp](src/FindMToLastElement.cpp)
- **Topics:** Finding m-th to last element

### Search & Sort
- **File:** [BinarySearch.cpp](src/BinarySearch.cpp)
- **Topics:** Binary search implementation
- **File:** [SearchAnagramsDictionary.cpp](src/SearchAnagramsDictionary.cpp)
- **Topics:** Anagram searching in dictionary

### Array Algorithms
- **File:** [FindMaxNoOfConsecutiveOnesFromIntArray.cpp](src/FindMaxNoOfConsecutiveOnesFromIntArray.cpp)
- **Topics:** Maximum consecutive elements

**[⬆ Back to Top](#table-of-contents)**

---

## Tuples & Structured Bindings

### Tuples
- **File:** [TuplesAndStructuredBindings.cpp](src/TuplesAndStructuredBindings.cpp)
- **Topics:** `std::tuple`, tuple operations, structured bindings

### Structured Bindings
- **File:** [StructuredBindings.cpp](src/StructuredBindings.cpp)
- **Topics:** Structured bindings syntax and patterns

**[⬆ Back to Top](#table-of-contents)**

---

## Configuration & Parsing

### Configuration
- **File:** [ConfigLoaderAndChecker.cpp](src/ConfigLoaderAndChecker.cpp)
- **Topics:** Configuration file loading and validation

### System Interaction
- **File:** [SystemInteractionAndParsing.cpp](src/SystemInteractionAndParsing.cpp)
- **Topics:** System calls, command execution, parsing

**[⬆ Back to Top](#table-of-contents)**

---

## Sensor Fusion & Scientific Computing

### Eigen Library
- **File:** [EigenSensorFusion.cpp](src/EigenSensorFusion.cpp)
- **Topics:** Eigen library, Kalman filter, sensor fusion, matrix operations

**[⬆ Back to Top](#table-of-contents)**

---

## Quick Reference Tables

### By Language Version

| C++11 | C++14 | C++17 | C++20 | C++23 |
|-------|-------|-------|-------|-------|
| [Cpp11Examples.cpp](src/Cpp11Examples.cpp) | [Cpp14Examples.cpp](src/Cpp14Examples.cpp) | [Cpp17Examples.cpp](src/Cpp17Examples.cpp) | [Cpp20Examples.cpp](src/Cpp20Examples.cpp) | [Cpp23Examples.cpp](src/Cpp23Examples.cpp) |
| [MoveSemantics.cpp](src/MoveSemantics.cpp) | [GenericLambdas.cpp](src/GenericLambdas.cpp) | [StructuredBindings.cpp](src/StructuredBindings.cpp) | [ConceptsExamples.cpp](src/ConceptsExamples.cpp) | - |
| [LambdaCaptures.cpp](src/LambdaCaptures.cpp) | - | [OptionalExamples.cpp](src/OptionalExamples.cpp) | [RangesExamples.cpp](src/RangesExamples.cpp) | - |
| - | - | [Cpp17Concurrency.cpp](src/Cpp17Concurrency.cpp) | [StopTokenExample.cpp](src/StopTokenExample.cpp) | - |

### By Use Case

| Use Case | Recommended Files |
|----------|------------------|
| **Learning Modern C++** | Start with [Cpp11Examples.cpp](src/Cpp11Examples.cpp), then progress through C++14, 17, 20, 23 examples |
| **Real-Time Systems** | [RealTimeProgramming.cpp](src/RealTimeProgramming.cpp), [SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp), [PimplIdiom.cpp](src/PimplIdiom.cpp) |
| **Embedded Systems** | [EmbeddedSystemsProgramming.cpp](src/EmbeddedSystemsProgramming.cpp), [EmbeddedThreadSafetyReentrancy.cpp](src/EmbeddedThreadSafetyReentrancy.cpp), [EmbeddedSystemsAvoid.cpp](src/EmbeddedSystemsAvoid.cpp), [FunctionalSafetyCppSTL.cpp](src/FunctionalSafetyCppSTL.cpp), [STLContainersNoHeap.cpp](src/STLContainersNoHeap.cpp) |
| **Safety-Critical** | [MISRACppDemo.cpp](src/MISRACppDemo.cpp), [FunctionalSafetyISO26262.cpp](src/FunctionalSafetyISO26262.cpp), [SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp) |
| **Performance** | [CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp), [PimplIdiom.cpp](src/PimplIdiom.cpp), [RealTimeProgramming.cpp](src/RealTimeProgramming.cpp) |
| **OOP Design** | [InheritanceTypes.cpp](src/InheritanceTypes.cpp), [DependencyInjection.cpp](src/DependencyInjection.cpp), [SOLIDPrinciples.cpp](src/SOLIDPrinciples.cpp) |
| **Memory Management** | [RuleOf3_5_0.cpp](src/RuleOf3_5_0.cpp), [MoveSemantics.cpp](src/MoveSemantics.cpp), [ResourceLeaks.cpp](src/ResourceLeaks.cpp) |
| **Concurrency** | [AsioAndModernCppConcurrency.cpp](src/AsioAndModernCppConcurrency.cpp), [FuturePromiseAsync.cpp](src/FuturePromiseAsync.cpp), [ThreadPoolExamples.cpp](src/ThreadPoolExamples.cpp) |

### By Problem You're Trying to Solve

| Problem | Solution Files |
|---------|---------------|
| "My code has memory leaks" | [ResourceLeaks.cpp](src/ResourceLeaks.cpp), [RuleOf3_5_0.cpp](src/RuleOf3_5_0.cpp) |
| "Objects are getting sliced" | [ObjectSlicingCpp20.cpp](src/ObjectSlicingCpp20.cpp), [ObjectSlicingSmartPtr.cpp](src/ObjectSlicingSmartPtr.cpp) |
| "Base class changes break derived classes" | [FragileBaseClass.cpp](src/FragileBaseClass.cpp) |
| "Multiple inheritance is causing problems" | [DiamondProblem.cpp](src/DiamondProblem.cpp) |
| "Virtual functions are too slow" | [CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp), [VirtualFunctionsInTemplates.cpp](src/VirtualFunctionsInTemplates.cpp) |
| "Pimpl hurts performance" | [PimplIdiom.cpp](src/PimplIdiom.cpp) |
| "Need containers without heap" | [STLContainersNoHeap.cpp](src/STLContainersNoHeap.cpp), [SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp) |
| "Timing is non-deterministic" | [RealTimeProgramming.cpp](src/RealTimeProgramming.cpp) |
| "Need C interop" | [CppWrappingCLibrary.cpp](src/CppWrappingCLibrary.cpp), [CreatingCApiFromCpp.cpp](src/CreatingCApiFromCpp.cpp) |

**[⬆ Back to Top](#table-of-contents)**

---

## Additional Resources

### Documentation Files
- [MarkDownDocuments/](MarkDownDocuments/) - Comprehensive markdown documentation
  - CPP11.md, CPP14.md, CPP17.md, CPP20.md, CPP23.md
  - Cpp20Modules.md, Cpp20ModulesQuickRef.md
  - CppCInterop.md
  - EventDrivenProgramming.md
  - MultiThreadedMicroservices.md
  - NlohmannJson.md
  - Protobuf.md
  - Pybind11.md
  - RestApi.md
  - SECURITY.md
  - TemplatedCameraInterface.md
  - VirtualFunctions.md

### Build Scripts
- [scripts/](scripts/) - Build and execution scripts
- [build_modules.ps1](build_modules.ps1) - PowerShell build script

**[⬆ Back to Top](#table-of-contents)**

---

## How to Use This Index

1. **Find by Topic**: Search for a keyword (e.g., "lambda", "virtual", "real-time")
2. **Find by Standard**: Look under C++ Standards Features for version-specific examples
3. **Find by Problem**: Check "Common Problems & Solutions" section
4. **Find by Use Case**: See "By Use Case" table for curated file lists

---

## Contributing

When adding new example files:
1. Update this index with appropriate mappings
2. Add to relevant sections
3. Update quick reference tables
4. Add problem-solution mapping if applicable

---

**Repository Structure:**
```
ModernCppExamples/
├── src/                  # Source files (.cpp, .cppm)
├── MarkDownDocuments/    # Markdown documentation
├── scripts/              # Build scripts
├── proto/                # Protocol buffer definitions
├── CMakeLists.txt        # CMake build configuration
└── INDEX.md              # This file
```

**[⬆ Back to Top](#table-of-contents)**

---

*Last updated: January 3, 2026*
