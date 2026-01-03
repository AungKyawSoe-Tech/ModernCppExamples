# Modern C++ Examples - Comprehensive Reference

> **A comprehensive collection of Modern C++ examples covering C++11 through C++23, design patterns, embedded systems, real-time programming, and safety-critical development.**

---

## 📝 **Document Information**

**Version:** 1.0.0  
**Last Updated:** January 3, 2026  
**Authours:**
- **Aung K. Soe** - Primary Developer
- **Claude Sonnet 4.5** - AI Assistant & Documentation Contributor

**Repository:** https://github.com/AungKyawSoe-Tech/ModernCppExamples

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
git clone https://github.com/AungKyawSoe-Tech/ModernCppExamples.git
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
git clone https://github.com/AungKyawSoe-Tech/ModernCppExamples.git
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

Below is a complete alphabetical listing of all C++ example files in this repository. 
Each entry links to the source file with a brief description of its contents.

### A

1. **[AdvancedExceptionHandling.cpp](src/AdvancedExceptionHandling.cpp)**
   - AdvancedExceptionHandling.cpp
   - Namespaces: std
   - Classes: DatabaseException, TracedException, StackTrace

2. **[ARMInstructionSets.cpp](src/ARMInstructionSets.cpp)**
   - compiled as Thumb
   - Functions: explain_alignment_issues, explain_interworking, explain_instruction_sets

3. **[AsioAndModernCppConcurrency.cpp](src/AsioAndModernCppConcurrency.cpp)**
   - Note: This example demonstrates ASIO concepts using standard C++ features.
   - Namespaces: std
   - Classes: SimulatedAsioTimer, HybridServer, EventLoop

4. **[AsioMultipleContexts.cpp](src/AsioMultipleContexts.cpp)**
   - ...
   - Namespaces: SimulatedAsio, std
   - Classes: io_context, executor_work_guard, steady_timer


### B

5. **[BinarySearch.cpp](src/BinarySearch.cpp)**
   - Modern C++ example demonstrating various features
   - Classes: Solution
   - Functions: main


### C

6. **[CameraModule.cppm](src/CameraModule.cppm)**
   - ===================================================================
   - Classes: ImageProcessor, Image, Camera

7. **[ConceptsExamples.cpp](src/ConceptsExamples.cpp)**
   - ===================================================================
   - Classes: Rectangle, Point2D, Circle
   - Functions: print_sum, example_basic_concept, example_standard_concepts

8. **[ConfigLoaderAndChecker.cpp](src/ConfigLoaderAndChecker.cpp)**
   - ConfigLoaderAndChecker.cpp
   - Namespaces: std, fs
   - Classes: ConfigManager, ConfigMonitor

9. **[Cpp11Examples.cpp](src/Cpp11Examples.cpp)**
   - ===================================================================
   - Classes: Color, Traffic, MoveableResource
   - Functions: example_auto, foo

10. **[Cpp14Examples.cpp](src/Cpp14Examples.cpp)**
   - ===================================================================
   - Namespaces: std
   - Functions: example_generic_lambdas, example_lambda_capture_initializers, example_binary_literals

11. **[Cpp17Concurrency.cpp](src/Cpp17Concurrency.cpp)**
   - ===================================================================
   - Namespaces: std
   - Classes: AsyncFileProcessor, CancellableFuture, ThreadPool

12. **[Cpp17Examples.cpp](src/Cpp17Examples.cpp)**
   - ===================================================================
   - Namespaces: fs, A
   - Functions: example_template_argument_deduction, example_structured_bindings, sum_fold

13. **[Cpp20Examples.cpp](src/Cpp20Examples.cpp)**
   - ===================================================================
   - Classes: Color
   - Functions: example_concepts, example_designated_initializers, example_three_way_comparison

14. **[Cpp23Examples.cpp](src/Cpp23Examples.cpp)**
   - ===================================================================
   - Classes: GPIOPin, MemoryRegister, SensorError
   - Functions: demonstrate_expected, demonstrate_byteswap

15. **[CppWrappingCLibrary.cpp](src/CppWrappingCLibrary.cpp)**
   - ===================================================================
   - Classes: Socket, TcpServer, SocketError
   - Functions: demonstrate_raii, demonstrate_nodiscard, demonstrate_noexcept

16. **[CreatingCApiFromCpp.cpp](src/CreatingCApiFromCpp.cpp)**
   - Create a new image
   - Namespaces: image_processing
   - Classes: Image

17. **[CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp)**
   - CRTP vs Virtual Functions: Static vs Dynamic Polymorphism
   - Namespaces: static_functions, virtual_functions
   - Classes: Dog, AnimalBase, Base


### D

18. **[DependencyInjection.cpp](src/DependencyInjection.cpp)**
   - ===================================================================
   - Classes: ConsoleLogger, ILogger, FileLogger
   - Functions: example_traditional_di, example_constructor_injection, example_interface_injection

19. **[DiamondProblem.cpp](src/DiamondProblem.cpp)**
   - DiamondProblem.cpp
   - Namespaces: virtual_inheritance_solution, diamond_problem
   - Classes: InputDevice, OutputDevice, Device


### E

20. **[EigenSensorFusion.cpp](src/EigenSensorFusion.cpp)**
   - EXPECTED OUTPUT:
   - Namespaces: Eigen
   - Classes: ComplementaryFilter, ParticleFilter, KalmanFilter

21. **[EmbeddedSystemsAvoid.cpp](src/EmbeddedSystemsAvoid.cpp)**
   - ===================================================================
   - Classes: BaseSensor, TempSensor, SensorBase
   - Functions: demonstrate_vector_problems, demonstrate_string_problems, demonstrate_heap_fragmentation

22. **[EmbeddedSystemsProgramming.cpp](src/EmbeddedSystemsProgramming.cpp)**
   - ===================================================================
   - Namespaces: BitOps
   - Classes: MemoryPool, PinMode, CircularBuffer

23. **[ErrorHandling.cpp](src/ErrorHandling.cpp)**
   - ===================================================================
   - Classes: runtime_error, CameraException, CameraNotConnectedException
   - Functions: demonstrate_custom_exceptions, demonstrate_basic_exception_handling, demonstrate_exception_hierarchy

24. **[ErrorHandlingStroustrup.cpp](src/ErrorHandlingStroustrup.cpp)**
   - ===================================================================
   - Classes: FileHandler_GOOD, FileHandler_BAD, ConfigParser
   - Functions: demonstrate_return_values, demonstrate_exceptions, demonstrate_termination

25. **[EventDrivenProgramming_Inheritance.cpp](src/EventDrivenProgramming_Inheritance.cpp)**
   - ===================================================================
   - Classes: IObserver, ConsoleObserver, CountingObserver
   - Functions: example_inheritance_observer, example_listener_pattern, example_event_hierarchy

26. **[EventDrivenProgramming_Lambdas.cpp](src/EventDrivenProgramming_Lambdas.cpp)**
   - ===================================================================
   - Classes: Subject, Signal, Button
   - Functions: example_variant_dispatcher, example_signal_slot, example_lambda_observer

27. **[ExceptionWithSourceLocation.cpp](src/ExceptionWithSourceLocation.cpp)**
   - ===================================================================
   - Classes: DetailedException, SourceLocationException, BaseLocationException
   - Functions: demonstrate_detailed_location, demonstrate_basic_location, some_function_that_fails


### F

28. **[FindCountOfCommonNodes.cpp](src/FindCountOfCommonNodes.cpp)**
   - Modern C++ example demonstrating various features
   - Namespaces: std
   - Classes: Solution

29. **[FindFirstCommonNode.cpp](src/FindFirstCommonNode.cpp)**
   - Modern C++ example demonstrating various features
   - Namespaces: std
   - Classes: Solution

30. **[FindMaxNoOfConsecutiveOnesFromIntArray.cpp](src/FindMaxNoOfConsecutiveOnesFromIntArray.cpp)**
   - Instructions:
   - Functions: main

31. **[FindMToLastElement.cpp](src/FindMToLastElement.cpp)**
   - Modern C++ example demonstrating various features
   - Namespaces: std
   - Classes: Solution

32. **[FragileBaseClass.cpp](src/FragileBaseClass.cpp)**
   - FragileBaseClass.cpp
   - Namespaces: composition_solution, fragile_example
   - Classes: Counter_V1, LoggingCounter_V1, Counter_V2

33. **[FunctionalSafetyISO26262.cpp](src/FunctionalSafetyISO26262.cpp)**
   - FunctionalSafetyISO26262.cpp
   - Classes: SafetyResult, SafetyState, ASILLevel
   - Functions: demonstrate_safe_data_types, demonstrate_redundancy_voting, demonstrate_asil_levels

34. **[FuturePromiseAsync.cpp](src/FuturePromiseAsync.cpp)**
   - FuturePromiseAsync.cpp
   - Namespaces: std, async_examples
   - Classes: RequestProcessor, SimpleThreadPool


### G

35. **[GenericLambdas.cpp](src/GenericLambdas.cpp)**
   - ===================================================================
   - Functions: example_basic_generic_lambda, example_generic_lambda_operations, example_generic_lambda_containers


### I

36. **[InheritanceTypes.cpp](src/InheritanceTypes.cpp)**
   - ===================================================================
   - Classes: Animal, Dog, Timer
   - Functions: example_public_inheritance, example_private_inheritance, example_protected_inheritance

37. **[InsertAndDeleteNodes.cpp](src/InsertAndDeleteNodes.cpp)**
   - PrintListNode(pInHead);
   - Namespaces: std
   - Classes: Solution


### L

38. **[LambdaCaptures.cpp](src/LambdaCaptures.cpp)**
   - ===================================================================
   - Classes: ExampleClass, ExampleClass2, Button
   - Functions: example_capture_all_by_reference, example_no_capture, example_capture_all_by_value


### M

39. **[MISRACppDemo.cpp](src/MISRACppDemo.cpp)**
   - ===================================================================
   - Namespaces: expressions_and_operators, types_and_declarations
   - Classes: Resource, GoodBase, BadBase

40. **[MoveSemantics.cpp](src/MoveSemantics.cpp)**
   - ===================================================================
   - Classes: MoveOnlyType, LargeObject, Resource
   - Functions: demonstrate_copy_vs_move, demonstrate_std_move, demonstrate_lvalue_rvalue

41. **[MoveSematicsPerfectForwarding.cpp](src/MoveSematicsPerfectForwarding.cpp)**
   - ===================================================================
   - Namespaces: lvalues_rvalues, move_semantics
   - Classes: Resource, Widget

42. **[MultiThreadedMicroservices.cpp](src/MultiThreadedMicroservices.cpp)**
   - MultiThreadedMicroservices.cpp
   - Namespaces: std
   - Classes: ThreadType, ThreadContext, Logger


### N

42. **[NlohmannJsonExample.cpp](src/NlohmannJsonExample.cpp)**
   - ===================================================================
   - Functions: example_basic_json, example_arrays, example_custom_types

42. **[NoexceptBestPractices.cpp](src/NoexceptBestPractices.cpp)**
   - ===================================================================
   - Namespaces: always_use_noexcept, what_is_noexcept
   - Classes: Resource, Swappable, MoveableResource

42. **[NVIIdiomTemplateMethod.cpp](src/NVIIdiomTemplateMethod.cpp)**
   - NVIIdiomTemplateMethod.cpp
   - Namespaces: std, basic_nvi
   - Classes: CSVProcessor, JSONProcessor, DataProcessor


### O

42. **[ObjectSlicingCpp20.cpp](src/ObjectSlicingCpp20.cpp)**
   - ===================================================================
   - Classes: Shape, Rectangle, Circle
   - Functions: draw_shape, process_shape, example_concepts_prevent_slicing

42. **[ObjectSlicingSmartPtr.cpp](src/ObjectSlicingSmartPtr.cpp)**
   - ===================================================================
   - Classes: Shape, Rectangle, Circle
   - Functions: example_shared_ptr_slicing_wrong, example_container_slicing_wrong, example_classic_slicing

42. **[OptionalExamples.cpp](src/OptionalExamples.cpp)**
   - ===================================================================
   - Functions: example_basic_optional, example_optional_return, example_value_or


### P

42. **[PerfectForwardingAndRequires.cpp](src/PerfectForwardingAndRequires.cpp)**
   - tests
   - Classes: ThreadSafeQueue, Serializable, Register
   - Functions: demonstrate_perfect_forwarding, call_with_forward, call_by_value

42. **[PimplIdiom.cpp](src/PimplIdiom.cpp)**
   - ===================================================================
   - Namespaces: what_is_pimpl, modern_cpp_pimpl
   - Classes: WidgetCpp98, WidgetNoPimpl, WidgetWithPimpl

42. **[ProtobufExample.cpp](src/ProtobufExample.cpp)**
   - ===================================================================
   - Functions: example_binary_deserialization, example_binary_serialization, example_create_sensor_reading

42. **[Pybind11Example.cpp](src/Pybind11Example.cpp)**
   - PYTHON USAGE EXAMPLES:
   - Namespaces: py
   - Classes: Vector2D, Shape, Circle


### R

42. **[RangesExamples.cpp](src/RangesExamples.cpp)**
   - ===================================================================
   - Namespaces: views, ranges
   - Functions: example_filter_view, example_basic_ranges, example_transform_view

42. **[RealTimeProgramming.cpp](src/RealTimeProgramming.cpp)**
   - RealTimeProgramming.cpp
   - Namespaces: big_o_notation, thread_architecture
   - Classes: RealTimeFlagManager, SensorSystem, SensorStatus

42. **[ResourceLeaks.cpp](src/ResourceLeaks.cpp)**
   - EXPECTED OUTPUT HIGHLIGHTS:
   - Namespaces: ModernCpp, OldCpp
   - Classes: Node, ExpensiveResource, ResourceIntensive

42. **[RestApiExample.cpp](src/RestApiExample.cpp)**
   - ===================================================================
   - Classes: CurlHandle, CurlHeaders, RestClient
   - Functions: example_create_post, example_update_post, example_json_placeholder_api

42. **[ROMability.cpp](src/ROMability.cpp)**
   - ROMability.cpp
   - Namespaces: const_vs_constexpr, rom_arrays
   - Functions: demonstrate, get_runtime_value

42. **[RuleOf3_5_0.cpp](src/RuleOf3_5_0.cpp)**
   - no-op
   - Classes: RuleOfFiveExample, RuleOfThreeExample, RuleOfZeroExample
   - Functions: example_rule_of_zero, example_rule_of_five, example_rule_of_three

42. **[RuntimePolymorphism.cpp](src/RuntimePolymorphism.cpp)**
   - ===================================================================
   - Classes: IShape, Rectangle, Circle
   - Functions: example_virtual_destructor, example_virtual_with_default, example_pure_virtual


### S

42. **[SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp)**
   - ===================================================================
   - Namespaces: container_classification, containers_to_avoid
   - Classes: SafetyPoolAllocator

42. **[SearchAnagramsDictionary.cpp](src/SearchAnagramsDictionary.cpp)**
   - Example program
   - Functions: main

42. **[SinglyLinkedList.cpp](src/SinglyLinkedList.cpp)**
   - pHead
   - Functions: print, main

42. **[SOLIDPrinciples.cpp](src/SOLIDPrinciples.cpp)**
   - ===================================================================
   - Namespaces: open_closed, single_responsibility
   - Classes: Employee, BadEmployee, TaxCalculator

42. **[STLContainersNoHeap.cpp](src/STLContainersNoHeap.cpp)**
   - ===================================================================
   - Classes: PoolAllocator, FixedVector, StackAllocator
   - Functions: example_std_array, example_std_span, process_data

42. **[StopTokenExample.cpp](src/StopTokenExample.cpp)**
   - StopTokenExample.cpp
   - Namespaces: std, old_vs_new
   - Classes: RESTService, OldStyleThread, NewStyleThread

42. **[StructuredBindings.cpp](src/StructuredBindings.cpp)**
   - ===================================================================
   - Functions: example_array_bindings, example_tuple_bindings, example_basic_structured_bindings

42. **[SystemInteractionAndParsing.cpp](src/SystemInteractionAndParsing.cpp)**
   - ===================================================================
   - Namespaces: fs
   - Classes: BankAccount, ThreadSafeLogger, SharedCounter


### T

42. **[TemplatedCameraInterface.cpp](src/TemplatedCameraInterface.cpp)**
   - ===================================================================
   - Classes: Camera8bit, Image, Camera
   - Functions: demonstrate_16bit_camera, demonstrate_float_camera, demonstrate_8bit_camera

42. **[TemplatedCameraModules.cpp](src/TemplatedCameraModules.cpp)**
   - ===================================================================
   - Functions: demonstrate_camera_capture, demonstrate_concepts, display_camera_info

42. **[ThreadPoolExamples.cpp](src/ThreadPoolExamples.cpp)**
   - ThreadPoolExamples.cpp
   - Namespaces: std
   - Classes: BasicThreadPool, TaskPriority, ThreadPoolWithFutures

42. **[TuplesAndStructuredBindings.cpp](src/TuplesAndStructuredBindings.cpp)**
   - ===================================================================
   - Functions: demonstrate_structured_bindings, demonstrate_basic_tuples, demonstrate_tie


### V

42. **[VariadicTemplateRecursion.cpp](src/VariadicTemplateRecursion.cpp)**
   - ===================================================================
   - Classes: GPIO_Pin
   - Functions: demonstrate_basic_recursion, print_recursive

42. **[VirtualFunctionsInTemplates.cpp](src/VirtualFunctionsInTemplates.cpp)**
   - VIRTUAL FUNCTIONS IN TEMPLATES: THE CODE BLOAT PROBLEM
   - Namespaces: the_problem, bad_example_t80
   - Classes: template, Vector, DemoInstantiation


### 🎯 **Interview Preparation**

75. **[MockInterview.cpp](src/MockInterview.cpp)**
   - **Complete C++ Interview Questions & Answers** - Comprehensive mock interview preparation guide
   - **Topics Covered:**
     - Section 1: C++ Fundamentals (pointers vs references, const correctness, RAII)
     - Section 2: Memory Management (smart pointers, memory alignment, custom allocators)
     - Section 3: OOP & Design Patterns (virtual dispatch, CRTP, PIMPL)
     - Section 4: Templates & Metaprogramming (SFINAE, variadic templates, compile-time computation)
     - Section 5: Concurrency & Multithreading (singleton, producer-consumer, atomic operations)
     - Section 6: Performance & Optimization (move semantics, RVO/NRVO, copy elision)
   - **Key Demonstrations:** 25+ interview questions with detailed explanations and runnable code
   - **Namespaces:** std
   - **Classes:** FileHandler, Data, Widget, ThreadSafeSingleton, ProducerConsumer, LargeObject, Expensive
   - **Functions:** pointers_vs_references, const_correctness, raii_demo, smart_pointers_advanced, memory_alignment, virtual_dispatch_details, crtp_mixins_demo, sfinae_demo, variadic_demo, compile_time_demo, singleton_demo, producer_consumer_demo, atomic_operations_demo, move_semantics_performance, copy_elision_demo

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
- **GitHub Issues**: https://github.com/AungKyawSoe-Tech/ModernCppExamples/issues
- **Email**: aungksoe.mobile@gmail.com

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
**Repository:** https://github.com/AungKyawSoe-Tech/ModernCppExamples
