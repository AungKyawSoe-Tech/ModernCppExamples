# Modern C++ Examples - Comprehensive Reference

> **A comprehensive collection of Modern C++ examples covering C++11 through C++23, design patterns, embedded systems, real-time programming, and safety-critical development.**

---

## 📝 **Document Information**

**Version:** 1.0.0  
**Last Updated:** January 3, 2026  
**Authors:**
- **Aung K. Soe** - Primary Developer
- **Claude Sonnet 4.5** - AI Assistant & Documentation Contributor

**Repository:** https://github.com/yourusername/ModernCppExamples

---

## 📖 **About This Project**

This repository contains over 70 comprehensive C++ examples demonstrating modern C++ features, design patterns, best practices, and real-world applications. Each example is self-contained, well-documented, and includes practical use cases.

### Key Features:
- ✅ **Complete C++ Standards Coverage**: Examples from C++11 through C++23
- ✅ **Safety-Critical Systems**: MISRA C++, AUTOSAR C++14, ISO 26262, DO-178C
- ✅ **Real-Time Programming**: Deterministic timing, WCET analysis, bounded memory
- ✅ **Design Patterns**: Gang of Four, CRTP, Pimpl, NVI, SOLID principles
- ✅ **Embedded Systems**: ARM instruction sets, ROMability, resource constraints
- ✅ **Modern Concurrency**: C++20 coroutines, thread pools, async/await
- ✅ **Interoperability**: Python bindings (pybind11), C API wrapping, Protocol Buffers
- ✅ **Performance**: Move semantics, perfect forwarding, template metaprogramming

---

## 🚀 **Quick Start**

### Prerequisites
- C++20 or later compiler (GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.20 or later
- Optional: Python 3.8+ (for pybind11 examples)
- Optional: Protocol Buffers compiler (for protobuf examples)

### Build Instructions

#### Linux/Mac:
```bash
# Clone the repository
git clone https://github.com/yourusername/ModernCppExamples.git
cd ModernCppExamples

# Build all examples
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run a specific example
./bin/Cpp20Examples
```

#### Windows (PowerShell):
```powershell
# Clone the repository
git clone https://github.com/yourusername/ModernCppExamples.git
cd ModernCppExamples

# Build all examples
mkdir build
cd build
cmake ..
cmake --build . --config Release

# Run a specific example
.\bin\Release\Cpp20Examples.exe
```

---

## 📚 **Navigation & Documentation**

### Detailed Index
For comprehensive topic mapping, concept lookup, and cross-references, see:
- **[INDEX.md](INDEX.md)** - Complete index mapping concepts, keywords, features, and problems to specific examples

### Supporting Documentation
Additional detailed guides are available in the `MarkDownDocuments/` folder:
- [CPP11.md](MarkDownDocuments/CPP11.md) - C++11 features guide
- [CPP14.md](MarkDownDocuments/CPP14.md) - C++14 enhancements
- [CPP17.md](MarkDownDocuments/CPP17.md) - C++17 major features
- [CPP20.md](MarkDownDocuments/CPP20.md) - C++20 revolutionary features
- [CPP23.md](MarkDownDocuments/CPP23.md) - C++23 latest additions
- [EventDrivenProgramming.md](MarkDownDocuments/EventDrivenProgramming.md) - Event-driven architecture
- [MultiThreadedMicroservices.md](MarkDownDocuments/MultiThreadedMicroservices.md) - Microservices patterns
- [Protobuf.md](MarkDownDocuments/Protobuf.md) - Protocol Buffers integration
- [Pybind11.md](MarkDownDocuments/Pybind11.md) - Python bindings
- [SECURITY.md](MarkDownDocuments/SECURITY.md) - Security best practices

### 🤖 Automatic Updates
**When you add new source files**, the documentation automatically updates:
- ✅ **README.md** - Alphabetical listing auto-updates (see [AUTO_UPDATE_GUIDE.md](AUTO_UPDATE_GUIDE.md))
- ⚠️ **INDEX.md** - Template generated for manual topic categorization
- ✅ **PDF** - Automatically includes all new files

**See:** [AUTO_UPDATE_GUIDE.md](AUTO_UPDATE_GUIDE.md) for complete details on adding new examples.

---

## 📋 **Complete Examples - Alphabetical Reference**

Below is a complete alphabetical listing of all 72 C++ example files in this repository. Each entry links to the source file with a brief description of its contents.

### A

1. **[AdvancedExceptionHandling.cpp](src/AdvancedExceptionHandling.cpp)**
   - Advanced exception handling techniques
   - Nested exceptions, exception safety guarantees
   - RAII and scope guards
   - Key Functions: `demonstrate_exception_safety()`, `demonstrate_nested_exceptions()`

2. **[ARMInstructionSets.cpp](src/ARMInstructionSets.cpp)**
   - ARM processor instruction sets and optimization
   - NEON intrinsics, inline assembly
   - Platform-specific code patterns
   - Key Functions: `demonstrate_arm_optimization()`

3. **[AsioAndModernCppConcurrency.cpp](src/AsioAndModernCppConcurrency.cpp)**
   - Boost.Asio with modern C++ concurrency
   - Coroutines with Asio
   - Async I/O patterns
   - Key Functions: `async_tcp_server()`, `coroutine_example()`

4. **[AsioMultipleContexts.cpp](src/AsioMultipleContexts.cpp)**
   - Multiple io_context management
   - Thread pool with Asio
   - Load balancing strategies
   - Key Functions: `multi_context_server()`, `load_balanced_service()`

### B

5. **[BinarySearch.cpp](src/BinarySearch.cpp)**
   - Binary search implementations
   - std::lower_bound, std::upper_bound
   - Custom comparators
   - Key Functions: `binary_search_recursive()`, `binary_search_iterative()`

### C

6. **[CameraModule.cppm](src/CameraModule.cppm)**
   - C++20 modules example
   - Module interface units
   - Export declarations
   - Key Module: `CameraModule`

7. **[ConceptsExamples.cpp](src/ConceptsExamples.cpp)**
   - C++20 concepts and constraints
   - Requires clauses
   - Type constraints
   - Key Concepts: `Numeric`, `Comparable`, `Container`

8. **[ConfigLoaderAndChecker.cpp](src/ConfigLoaderAndChecker.cpp)**
   - Configuration file parsing
   - JSON validation
   - Error reporting
   - Key Functions: `load_config()`, `validate_config()`

9. **[Cpp11Examples.cpp](src/Cpp11Examples.cpp)**
   - C++11 features: auto, nullptr, range-for
   - Lambda expressions
   - Move semantics
   - Key Functions: `demonstrate_cpp11_features()`

10. **[Cpp14Examples.cpp](src/Cpp14Examples.cpp)**
    - C++14 enhancements: generic lambdas
    - Binary literals, digit separators
    - Return type deduction
    - Key Functions: `demonstrate_cpp14_features()`

11. **[Cpp17Concurrency.cpp](src/Cpp17Concurrency.cpp)**
    - C++17 parallel algorithms
    - std::execution policies
    - Parallel STL
    - Key Functions: `parallel_sort()`, `parallel_transform()`

12. **[Cpp17Examples.cpp](src/Cpp17Examples.cpp)**
    - C++17 features: structured bindings
    - std::optional, std::variant, std::any
    - Fold expressions
    - Key Functions: `demonstrate_cpp17_features()`

13. **[Cpp20Examples.cpp](src/Cpp20Examples.cpp)**
    - C++20 features: concepts, ranges, coroutines
    - Three-way comparison (spaceship operator)
    - Calendar and time zones
    - Key Functions: `demonstrate_cpp20_features()`

14. **[Cpp23Examples.cpp](src/Cpp23Examples.cpp)**
    - C++23 latest features: std::expected
    - Multidimensional subscript operator
    - Deducing this
    - Key Functions: `demonstrate_cpp23_features()`

15. **[CppWrappingCLibrary.cpp](src/CppWrappingCLibrary.cpp)**
    - Wrapping C libraries in C++
    - RAII for C resources
    - Type-safe wrappers
    - Key Functions: `wrap_c_library()`, `CFileWrapper`

16. **[CreatingCApiFromCpp.cpp](src/CreatingCApiFromCpp.cpp)**
    - Creating C API from C++ code
    - extern "C" linkage
    - Opaque pointer pattern
    - Key Functions: `create_c_api()`, opaque handle pattern

17. **[CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp)**
    - CRTP (Curiously Recurring Template Pattern)
    - Static polymorphism vs dynamic polymorphism
    - Performance comparison
    - Key Functions: `crtp_example()`, `virtual_example()`

### D

18. **[DependencyInjection.cpp](src/DependencyInjection.cpp)**
    - Dependency injection patterns
    - Constructor injection
    - Interface segregation
    - Key Functions: `demonstrate_dependency_injection()`

19. **[DiamondProblem.cpp](src/DiamondProblem.cpp)**
    - Diamond inheritance problem
    - Virtual inheritance
    - Multiple inheritance solutions
    - Key Functions: `demonstrate_diamond_problem()`

### E

20. **[EigenSensorFusion.cpp](src/EigenSensorFusion.cpp)**
    - Eigen library for sensor fusion
    - Kalman filters
    - Matrix operations
    - Key Functions: `kalman_filter()`, `sensor_fusion()`

21. **[EmbeddedSystemsAvoid.cpp](src/EmbeddedSystemsAvoid.cpp)**
    - What to avoid in embedded systems
    - Exceptions, RTTI, dynamic memory
    - Forbidden features for resource-constrained systems
    - Key Functions: `demonstrate_what_to_avoid()`

22. **[EmbeddedSystemsProgramming.cpp](src/EmbeddedSystemsProgramming.cpp)**
    - Embedded C++ best practices
    - Register access, bit manipulation
    - Interrupt handling
    - Key Functions: `embedded_patterns()`, `register_access()`

23. **[ErrorHandling.cpp](src/ErrorHandling.cpp)**
    - Error handling strategies
    - Error codes vs exceptions
    - std::expected (C++23)
    - Key Functions: `error_code_example()`, `exception_example()`

24. **[ErrorHandlingStroustrup.cpp](src/ErrorHandlingStroustrup.cpp)**
    - Bjarne Stroustrup's error handling guidelines
    - Constructor exceptions
    - Resource management
    - Key Functions: `demonstrate_stroustrup_guidelines()`

25. **[EventDrivenProgramming_Inheritance.cpp](src/EventDrivenProgramming_Inheritance.cpp)**
    - Event-driven programming using inheritance
    - Observer pattern
    - Event dispatch
    - Key Functions: `EventSystem`, `Observer`

26. **[EventDrivenProgramming_Lambdas.cpp](src/EventDrivenProgramming_Lambdas.cpp)**
    - Event-driven programming using lambdas
    - Callback registration
    - Signal/slot mechanism
    - Key Functions: `EventDispatcher`, `on_event()`

27. **[ExceptionWithSourceLocation.cpp](src/ExceptionWithSourceLocation.cpp)**
    - C++20 std::source_location
    - Exception with location info
    - Debug-friendly exceptions
    - Key Functions: `throw_with_location()`

### F

28. **[FindCountOfCommonNodes.cpp](src/FindCountOfCommonNodes.cpp)**
    - Algorithm: count common nodes in linked lists
    - Two-pointer technique
    - Key Functions: `count_common_nodes()`

29. **[FindFirstCommonNode.cpp](src/FindFirstCommonNode.cpp)**
    - Algorithm: find first common node in linked lists
    - Intersection point detection
    - Key Functions: `find_first_common_node()`

30. **[FindMaxNoOfConsecutiveOnesFromIntArray.cpp](src/FindMaxNoOfConsecutiveOnesFromIntArray.cpp)**
    - Algorithm: find maximum consecutive ones
    - Sliding window technique
    - Key Functions: `find_max_consecutive_ones()`

31. **[FindMToLastElement.cpp](src/FindMToLastElement.cpp)**
    - Algorithm: find Mth element from end
    - Two-pointer technique
    - Key Functions: `find_mth_from_last()`

32. **[FragileBaseClass.cpp](src/FragileBaseClass.cpp)**
    - Fragile base class problem
    - Avoiding inheritance pitfalls
    - Composition over inheritance
    - Key Functions: `demonstrate_fragile_base_class()`

33. **[FunctionalSafetyISO26262.cpp](src/FunctionalSafetyISO26262.cpp)**
    - ISO 26262 functional safety
    - ASIL levels
    - Safety mechanisms
    - Key Functions: `demonstrate_iso26262_patterns()`

34. **[FuturePromiseAsync.cpp](src/FuturePromiseAsync.cpp)**
    - std::future, std::promise, std::async
    - Asynchronous programming
    - Task-based parallelism
    - Key Functions: `async_example()`, `promise_example()`

### G

35. **[GenericLambdas.cpp](src/GenericLambdas.cpp)**
    - Generic lambdas (C++14)
    - Auto parameters
    - Template lambdas (C++20)
    - Key Functions: `demonstrate_generic_lambdas()`

### I

36. **[InheritanceTypes.cpp](src/InheritanceTypes.cpp)**
    - Public, protected, private inheritance
    - Interface inheritance vs implementation inheritance
    - Key Functions: `demonstrate_inheritance_types()`

37. **[InsertAndDeleteNodes.cpp](src/InsertAndDeleteNodes.cpp)**
    - Linked list operations: insert/delete
    - Node management
    - Key Functions: `insert_node()`, `delete_node()`

### L

38. **[LambdaCaptures.cpp](src/LambdaCaptures.cpp)**
    - Lambda capture modes
    - Capture by value, by reference, init captures
    - Key Functions: `demonstrate_lambda_captures()`

### M

39. **[MISRACppDemo.cpp](src/MISRACppDemo.cpp)**
    - MISRA C++ guidelines demonstration
    - Safety-critical coding standards
    - Rule compliance examples
    - Key Functions: `demonstrate_misra_rules()`

40. **[MoveSemantics.cpp](src/MoveSemantics.cpp)**
    - Move semantics and rvalue references
    - std::move, perfect forwarding
    - Key Functions: `demonstrate_move_semantics()`

41. **[MoveSematicsPerfectForwarding.cpp](src/MoveSematicsPerfectForwarding.cpp)**
    - Advanced move semantics
    - Perfect forwarding with std::forward
    - Universal references
    - Key Functions: `perfect_forward()`, `make_unique_example()`

42. **[MultiThreadedMicroservices.cpp](src/MultiThreadedMicroservices.cpp)**
    - Multi-threaded microservices architecture
    - Service communication
    - Thread-safe queues
    - Key Functions: `MicroserviceFramework`, `Service`

### N

43. **[NlohmannJsonExample.cpp](src/NlohmannJsonExample.cpp)**
    - nlohmann/json library usage
    - JSON parsing and serialization
    - Key Functions: `parse_json()`, `serialize_json()`

44. **[NoexceptBestPractices.cpp](src/NoexceptBestPractices.cpp)**
    - noexcept specifier best practices
    - Conditional noexcept
    - Move operations and noexcept
    - Key Functions: `demonstrate_noexcept()`

45. **[NVIIdiomTemplateMethod.cpp](src/NVIIdiomTemplateMethod.cpp)**
    - Non-Virtual Interface (NVI) idiom
    - Template Method pattern
    - Key Functions: `NVIBase`, `template_method()`

### O

46. **[ObjectSlicingCpp20.cpp](src/ObjectSlicingCpp20.cpp)**
    - Object slicing problem
    - Solutions using polymorphism
    - Key Functions: `demonstrate_object_slicing()`

47. **[ObjectSlicingSmartPtr.cpp](src/ObjectSlicingSmartPtr.cpp)**
    - Object slicing prevention with smart pointers
    - Polymorphic containers
    - Key Functions: `demonstrate_polymorphic_container()`

48. **[OptionalExamples.cpp](src/OptionalExamples.cpp)**
    - std::optional usage
    - Monadic operations (C++23)
    - Key Functions: `optional_examples()`, `transform_optional()`

### P

49. **[PerfectForwardingAndRequires.cpp](src/PerfectForwardingAndRequires.cpp)**
    - Perfect forwarding with requires clauses
    - Constrained templates
    - Key Functions: `forward_with_constraints()`

50. **[PimplIdiom.cpp](src/PimplIdiom.cpp)**
    - Pointer to Implementation (Pimpl) idiom
    - Compilation firewall
    - ABI stability
    - Performance considerations for real-time systems
    - FastPimpl alternatives
    - Key Functions: `Widget`, `WidgetImpl`, `FastPimplWidget`

51. **[ProtobufExample.cpp](src/ProtobufExample.cpp)**
    - Protocol Buffers integration
    - Serialization/deserialization
    - Key Functions: `serialize_protobuf()`, `deserialize_protobuf()`

52. **[Pybind11Example.cpp](src/Pybind11Example.cpp)**
    - Python bindings with pybind11
    - Exposing C++ to Python
    - Key Functions: `PYBIND11_MODULE()`

### R

53. **[RangesExamples.cpp](src/RangesExamples.cpp)**
    - C++20 Ranges library
    - Views, adaptors, pipelines
    - Key Functions: `demonstrate_ranges()`, `range_pipelines()`

54. **[RealTimeProgramming.cpp](src/RealTimeProgramming.cpp)**
    - Real-time C++ programming
    - Deterministic timing
    - WCET analysis
    - std::list::splice() for O(1) operations
    - Memory pools and bounded execution
    - Key Functions: `demonstrate_realtime_patterns()`, `list_splice_example()`

55. **[ResourceLeaks.cpp](src/ResourceLeaks.cpp)**
    - Resource leak detection and prevention
    - RAII patterns
    - Key Functions: `demonstrate_resource_management()`

56. **[RestApiExample.cpp](src/RestApiExample.cpp)**
    - REST API implementation
    - HTTP client/server
    - Key Functions: `RestApiServer`, `handle_request()`

57. **[ROMability.cpp](src/ROMability.cpp)**
    - ROMable code for embedded systems
    - Const correctness
    - Placement in ROM vs RAM
    - Key Functions: `demonstrate_romability()`

58. **[RuleOf3_5_0.cpp](src/RuleOf3_5_0.cpp)**
    - Rule of Three, Five, and Zero
    - Copy/move constructors and assignment operators
    - Key Functions: `RuleOfThree`, `RuleOfFive`, `RuleOfZero`

59. **[RuntimePolymorphism.cpp](src/RuntimePolymorphism.cpp)**
    - Runtime polymorphism
    - Virtual functions, vtables
    - Key Functions: `demonstrate_polymorphism()`

### S

60. **[SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp)**
    - STL containers in safety-critical systems
    - MISRA C++, AUTOSAR C++14, ISO 26262, DO-178C guidelines
    - Which containers to avoid (std::list, std::map, std::set, unordered_*)
    - Custom allocators for deterministic behavior
    - std::array, std::vector with fixed capacity
    - Key Functions: `demonstrate_safe_containers()`, `custom_allocator_example()`

61. **[SearchAnagramsDictionary.cpp](src/SearchAnagramsDictionary.cpp)**
    - Algorithm: find anagrams in dictionary
    - Hash-based solutions
    - Key Functions: `find_anagrams()`

62. **[SinglyLinkedList.cpp](src/SinglyLinkedList.cpp)**
    - Singly linked list implementation
    - Basic operations: insert, delete, search
    - Key Functions: `SinglyLinkedList::insert()`, `SinglyLinkedList::remove()`

63. **[SOLIDPrinciples.cpp](src/SOLIDPrinciples.cpp)**
    - SOLID principles demonstration
    - Single Responsibility, Open/Closed, Liskov Substitution
    - Interface Segregation, Dependency Inversion
    - Key Functions: `demonstrate_solid_principles()`

64. **[STLContainersNoHeap.cpp](src/STLContainersNoHeap.cpp)**
    - STL containers without heap allocation
    - Fixed-size containers
    - Custom allocators
    - Key Functions: `demonstrate_no_heap_containers()`

65. **[StopTokenExample.cpp](src/StopTokenExample.cpp)**
    - C++20 std::stop_token for cancellation
    - Cooperative cancellation
    - Key Functions: `cancellable_task()`

66. **[StructuredBindings.cpp](src/StructuredBindings.cpp)**
    - C++17 structured bindings
    - Decomposition declarations
    - Key Functions: `demonstrate_structured_bindings()`

67. **[SystemInteractionAndParsing.cpp](src/SystemInteractionAndParsing.cpp)**
    - System interaction: file I/O, command execution
    - Parsing: regex, string operations
    - Key Functions: `execute_command()`, `parse_output()`

### T

68. **[TemplatedCameraInterface.cpp](src/TemplatedCameraInterface.cpp)**
    - Templated camera interface design
    - Policy-based design
    - Key Functions: `CameraInterface<>`, camera policies

69. **[TemplatedCameraModules.cpp](src/TemplatedCameraModules.cpp)**
    - Advanced templated camera modules
    - Compile-time polymorphism
    - Key Functions: `CameraModule<>`, sensor abstractions

70. **[ThreadPoolExamples.cpp](src/ThreadPoolExamples.cpp)**
    - Thread pool implementation
    - Task scheduling
    - Work queues
    - Key Functions: `ThreadPool`, `enqueue_task()`

71. **[TuplesAndStructuredBindings.cpp](src/TuplesAndStructuredBindings.cpp)**
    - std::tuple with structured bindings
    - Tuple manipulation
    - Key Functions: `tuple_examples()`, `structured_binding_examples()`

### V

72. **[VariadicTemplateRecursion.cpp](src/VariadicTemplateRecursion.cpp)**
    - Variadic templates
    - Template parameter pack expansion
    - Recursive templates
    - Key Functions: `variadic_print()`, `sum()`

73. **[VirtualFunctionsInTemplates.cpp](src/VirtualFunctionsInTemplates.cpp)**
    - Virtual functions in template classes
    - Combining static and dynamic polymorphism
    - Key Functions: `TemplateWithVirtual<>`

---

## 🏗️ **Project Structure**

```
ModernCppExamples/
├── src/                      # All C++ source files (.cpp, .cppm)
│   ├── Cpp11Examples.cpp     # C++11 features
│   ├── Cpp20Examples.cpp     # C++20 features
│   ├── RealTimeProgramming.cpp
│   ├── SafetyCriticalSTLContainers.cpp
│   ├── PimplIdiom.cpp
│   └── ... (70+ examples)
│
├── MarkDownDocuments/        # Detailed documentation
│   ├── CPP20.md
│   ├── EventDrivenProgramming.md
│   ├── MultiThreadedMicroservices.md
│   ├── Protobuf.md
│   ├── Pybind11.md
│   ├── SECURITY.md
│   └── ...
│
├── scripts/                  # Build and utility scripts
│   ├── build.sh              # Main build script
│   ├── build_modules.sh      # C++20 modules build
│   ├── build_protobuf.sh     # Protocol Buffers build
│   ├── build_pybind.sh       # Pybind11 build
│   └── run_all.sh            # Run all examples
│
├── proto/                    # Protocol Buffer definitions
│   └── sensor_data.proto
│
├── build/                    # Build output directory
│   └── bin/                  # Compiled executables
│
├── CMakeLists.txt            # CMake configuration
├── INDEX.md                  # Comprehensive topic index
├── README.md                 # This file
└── config.json               # Configuration file
```

---

## 🔍 **Finding Examples by Topic**

For detailed topic mapping, see [INDEX.md](INDEX.md). Quick topic guide:

### By C++ Standard
- **C++11**: [Cpp11Examples.cpp](src/Cpp11Examples.cpp), [LambdaCaptures.cpp](src/LambdaCaptures.cpp), [MoveSemantics.cpp](src/MoveSemantics.cpp)
- **C++14**: [Cpp14Examples.cpp](src/Cpp14Examples.cpp), [GenericLambdas.cpp](src/GenericLambdas.cpp)
- **C++17**: [Cpp17Examples.cpp](src/Cpp17Examples.cpp), [StructuredBindings.cpp](src/StructuredBindings.cpp), [OptionalExamples.cpp](src/OptionalExamples.cpp)
- **C++20**: [Cpp20Examples.cpp](src/Cpp20Examples.cpp), [ConceptsExamples.cpp](src/ConceptsExamples.cpp), [RangesExamples.cpp](src/RangesExamples.cpp), [CameraModule.cppm](src/CameraModule.cppm)
- **C++23**: [Cpp23Examples.cpp](src/Cpp23Examples.cpp)

### By Application Domain
- **Embedded Systems**: [EmbeddedSystemsProgramming.cpp](src/EmbeddedSystemsProgramming.cpp), [ROMability.cpp](src/ROMability.cpp), [ARMInstructionSets.cpp](src/ARMInstructionSets.cpp)
- **Real-Time Systems**: [RealTimeProgramming.cpp](src/RealTimeProgramming.cpp), [STLContainersNoHeap.cpp](src/STLContainersNoHeap.cpp)
- **Safety-Critical**: [SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp), [FunctionalSafetyISO26262.cpp](src/FunctionalSafetyISO26262.cpp), [MISRACppDemo.cpp](src/MISRACppDemo.cpp)
- **Concurrency**: [Cpp17Concurrency.cpp](src/Cpp17Concurrency.cpp), [FuturePromiseAsync.cpp](src/FuturePromiseAsync.cpp), [ThreadPoolExamples.cpp](src/ThreadPoolExamples.cpp)

### By Design Pattern
- **CRTP**: [CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp)
- **Pimpl**: [PimplIdiom.cpp](src/PimplIdiom.cpp)
- **NVI**: [NVIIdiomTemplateMethod.cpp](src/NVIIdiomTemplateMethod.cpp)
- **Dependency Injection**: [DependencyInjection.cpp](src/DependencyInjection.cpp)
- **SOLID**: [SOLIDPrinciples.cpp](src/SOLIDPrinciples.cpp)

---

## 🧪 **Testing & Running Examples**

Each example is a standalone executable. After building:

```bash
cd build/bin

# Run a specific example
./Cpp20Examples

# Run all examples (Linux/Mac)
cd ../..
./scripts/run_all.sh

# Run all examples (Windows PowerShell)
Get-ChildItem .\bin\Release\*.exe | ForEach-Object { & $_.FullName }
```

---

## 📦 **Dependencies**

Most examples have no external dependencies. Optional dependencies for specific examples:

- **Boost.Asio**: [AsioAndModernCppConcurrency.cpp](src/AsioAndModernCppConcurrency.cpp), [AsioMultipleContexts.cpp](src/AsioMultipleContexts.cpp)
- **Eigen**: [EigenSensorFusion.cpp](src/EigenSensorFusion.cpp)
- **nlohmann/json**: [NlohmannJsonExample.cpp](src/NlohmannJsonExample.cpp)
- **Protocol Buffers**: [ProtobufExample.cpp](src/ProtobufExample.cpp)
- **pybind11**: [Pybind11Example.cpp](src/Pybind11Example.cpp)

Build scripts automatically check for these dependencies.

---

## 🤝 **Contributing**

Contributions are welcome! Please follow these guidelines:

1. **Code Style**: Follow the existing code style (Google C++ Style Guide)
2. **Documentation**: Each example should have clear comments explaining the concepts
3. **Compilation**: Ensure code compiles with C++20 standard
4. **Testing**: Test on multiple compilers if possible
5. **Commit Messages**: Use clear, descriptive commit messages

---

## 📄 **License**

This project is licensed under the MIT License. See LICENSE file for details.

---

## 📞 **Contact & Support**

For questions, suggestions, or issues:
- **GitHub Issues**: https://github.com/yourusername/ModernCppExamples/issues
- **Email**: your.email@example.com

---

## 🙏 **Acknowledgments**

Special thanks to:
- The C++ Standards Committee for continuously evolving the language
- Open source contributors: Boost, Eigen, nlohmann/json, pybind11
- The C++ community for valuable feedback and suggestions

---

## 📖 **Additional Resources**

### Official Documentation
- [C++ Reference](https://en.cppreference.com/)
- [ISO C++ Standard](https://isocpp.org/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)

### Safety Standards
- [MISRA C++:2008](https://www.misra.org.uk/)
- [AUTOSAR C++14](https://www.autosar.org/)
- [ISO 26262](https://www.iso.org/standard/68383.html) - Automotive functional safety
- [DO-178C](https://en.wikipedia.org/wiki/DO-178C) - Airborne software safety

### Books
- "Effective Modern C++" by Scott Meyers
- "C++ Concurrency in Action" by Anthony Williams
- "Real-Time C++" by Christopher Kormanyos
- "C++ Move Semantics" by Nicolai Josuttis

---

**Last Updated:** January 3, 2026  
**Version:** 1.0.0  
**Repository:** https://github.com/yourusername/ModernCppExamples
