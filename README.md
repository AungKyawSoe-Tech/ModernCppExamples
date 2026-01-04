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
   - Classes: StackTrace, DatabaseException, TracedException

2. **[ARMInstructionSets.cpp](src/ARMInstructionSets.cpp)**
   - compiled as Thumb
   - Functions: explain_interworking, explain_alignment_issues, explain_instruction_sets

3. **[AsioAndModernCppConcurrency.cpp](src/AsioAndModernCppConcurrency.cpp)**
   - Note: This example demonstrates ASIO concepts using standard C++ features.
   - Namespaces: std
   - Classes: HybridServer, SimulatedAsioTimer, EventLoop

4. **[AsioMultipleContexts.cpp](src/AsioMultipleContexts.cpp)**
   - ...
   - Namespaces: std, SimulatedAsio
   - Classes: io_context, steady_timer, executor_work_guard


### B

5. **[BinarySearch.cpp](src/BinarySearch.cpp)**
   - Modern C++ example demonstrating various features
   - Classes: Solution
   - Functions: main


### C

6. **[CameraModule.cppm](src/CameraModule.cppm)**
   - ===================================================================
   - Classes: Image, ImageProcessor, Camera

7. **[ConceptsExamples.cpp](src/ConceptsExamples.cpp)**
   - ===================================================================
   - Classes: Rectangle, Point2D, Circle
   - Functions: example_basic_concept, example_standard_concepts, print_sum

8. **[ConfigLoaderAndChecker.cpp](src/ConfigLoaderAndChecker.cpp)**
   - ConfigLoaderAndChecker.cpp
   - Namespaces: fs, std
   - Classes: ConfigManager, ConfigMonitor

9. **[Cpp11Examples.cpp](src/Cpp11Examples.cpp)**
   - ===================================================================
   - Classes: Traffic, MoveableResource, Color
   - Functions: example_auto, foo

10. **[Cpp14Examples.cpp](src/Cpp14Examples.cpp)**
   - ===================================================================
   - Namespaces: std
   - Functions: example_binary_literals, example_lambda_capture_initializers, example_generic_lambdas

11. **[Cpp17Concurrency.cpp](src/Cpp17Concurrency.cpp)**
   - ===================================================================
   - Namespaces: std
   - Classes: ThreadPool, CancellableFuture, AsyncFileProcessor

12. **[Cpp17Examples.cpp](src/Cpp17Examples.cpp)**
   - ===================================================================
   - Namespaces: fs, A
   - Functions: example_template_argument_deduction, sum_fold, example_structured_bindings

13. **[Cpp20Examples.cpp](src/Cpp20Examples.cpp)**
   - ===================================================================
   - Classes: Color
   - Functions: example_designated_initializers, example_three_way_comparison, example_concepts

14. **[Cpp23Examples.cpp](src/Cpp23Examples.cpp)**
   - ===================================================================
   - Classes: MemoryRegister, SensorError, GPIOPin
   - Functions: demonstrate_byteswap, demonstrate_expected

15. **[CppWrappingCLibrary.cpp](src/CppWrappingCLibrary.cpp)**
   - ===================================================================
   - Classes: SocketError, Socket, TcpServer
   - Functions: demonstrate_raii, demonstrate_noexcept, demonstrate_nodiscard

16. **[CreatingCApiFromCpp.cpp](src/CreatingCApiFromCpp.cpp)**
   - Create a new image
   - Namespaces: image_processing
   - Classes: Image

17. **[CRTPvsVirtualFunctions.cpp](src/CRTPvsVirtualFunctions.cpp)**
   - CRTP vs Virtual Functions: Static vs Dynamic Polymorphism
   - Namespaces: static_functions, virtual_functions
   - Classes: Dog, Base, AnimalBase


### D

18. **[DependencyInjection.cpp](src/DependencyInjection.cpp)**
   - ===================================================================
   - Classes: FileLogger, ILogger, ConsoleLogger
   - Functions: example_traditional_di, example_constructor_injection, example_interface_injection

19. **[DiamondProblem.cpp](src/DiamondProblem.cpp)**
   - DiamondProblem.cpp
   - Namespaces: diamond_problem, virtual_inheritance_solution
   - Classes: Device, OutputDevice, InputDevice


### E

20. **[EigenSensorFusion.cpp](src/EigenSensorFusion.cpp)**
   - EXPECTED OUTPUT:
   - Namespaces: Eigen
   - Classes: ParticleFilter, KalmanFilter, ComplementaryFilter

21. **[EmbeddedSystemsAvoid.cpp](src/EmbeddedSystemsAvoid.cpp)**
   - ===================================================================
   - Classes: BaseSensor, SensorBase, TempSensor
   - Functions: demonstrate_vector_problems, demonstrate_string_problems, demonstrate_heap_fragmentation

22. **[EmbeddedSystemsProgramming.cpp](src/EmbeddedSystemsProgramming.cpp)**
   - ===================================================================
   - Namespaces: BitOps
   - Classes: MemoryPool, PinMode, CircularBuffer

23. **[EmbeddedThreadSafetyReentrancy.cpp](src/EmbeddedThreadSafetyReentrancy.cpp)**
   - Thread-safety and re-entrancy for embedded/firmware systems
   - ISR-safe code, lock-free circular buffers, critical sections, atomic operations
   - Re-entrant vs thread-safe vs ISR-safe patterns, interrupt service routines
   - Practical UART driver with zero-overhead design

24. **[ErrorHandling.cpp](src/ErrorHandling.cpp)**
   - ===================================================================
   - Classes: CameraException, runtime_error, CameraNotConnectedException
   - Functions: demonstrate_basic_exception_handling, demonstrate_custom_exceptions, demonstrate_exception_hierarchy

24. **[ErrorHandlingStroustrup.cpp](src/ErrorHandlingStroustrup.cpp)**
   - ===================================================================
   - Classes: ConfigParser, FileHandler_GOOD, FileHandler_BAD
   - Functions: demonstrate_exceptions, demonstrate_termination, demonstrate_return_values

25. **[EventDrivenProgramming_Inheritance.cpp](src/EventDrivenProgramming_Inheritance.cpp)**
   - ===================================================================
   - Classes: CountingObserver, IObserver, ConsoleObserver
   - Functions: example_event_hierarchy, example_inheritance_observer, example_listener_pattern

26. **[EventDrivenProgramming_Lambdas.cpp](src/EventDrivenProgramming_Lambdas.cpp)**
   - ===================================================================
   - Classes: Subject, Signal, Button
   - Functions: example_lambda_observer, example_signal_slot, example_variant_dispatcher

27. **[ExceptionWithSourceLocation.cpp](src/ExceptionWithSourceLocation.cpp)**
   - ===================================================================
   - Classes: DetailedException, BaseLocationException, SourceLocationException
   - Functions: some_function_that_fails, demonstrate_detailed_location, demonstrate_basic_location


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
   - Namespaces: fragile_example, composition_solution
   - Classes: Counter_V2, LoggingCounter_V1, Counter_V1

33. **[FunctionalSafetyISO26262.cpp](src/FunctionalSafetyISO26262.cpp)**
   - FunctionalSafetyISO26262.cpp
   - Classes: SafetyState, ASILLevel, SafetyResult
   - Functions: demonstrate_redundancy_voting, demonstrate_safe_data_types, demonstrate_asil_levels

34. **[FuturePromiseAsync.cpp](src/FuturePromiseAsync.cpp)**
   - FuturePromiseAsync.cpp
   - Namespaces: std, async_examples
   - Classes: RequestProcessor, SimpleThreadPool


### G

35. **[GenericLambdas.cpp](src/GenericLambdas.cpp)**
   - ===================================================================
   - Functions: example_generic_lambda_containers, example_generic_lambda_operations, example_basic_generic_lambda


### I

36. **[InheritanceTypes.cpp](src/InheritanceTypes.cpp)**
   - ===================================================================
   - Classes: Timer, Dog, Animal
   - Functions: example_public_inheritance, example_private_inheritance, example_protected_inheritance

37. **[InsertAndDeleteNodes.cpp](src/InsertAndDeleteNodes.cpp)**
   - PrintListNode(pInHead);
   - Namespaces: std
   - Classes: Solution


### L

38. **[LambdaCaptures.cpp](src/LambdaCaptures.cpp)**
   - ===================================================================
   - Classes: ExampleClass2, Button, ExampleClass
   - Functions: example_capture_all_by_value, example_capture_all_by_reference, example_no_capture


### M

39. **[MISRACppDemo.cpp](src/MISRACppDemo.cpp)**
   - ===================================================================
   - Namespaces: expressions_and_operators, types_and_declarations
   - Classes: GoodBase, Resource, BadBase

40. **[MockInterview.cpp](src/MockInterview.cpp)**
   - ===================================================================
   - Namespaces: std
   - Classes: Data, FileHandler, Parent

41. **[MoveSemantics.cpp](src/MoveSemantics.cpp)**
   - ===================================================================
   - Classes: MoveOnlyType, LargeObject, Resource
   - Functions: demonstrate_lvalue_rvalue, demonstrate_std_move, demonstrate_copy_vs_move

42. **[MoveSematicsPerfectForwarding.cpp](src/MoveSematicsPerfectForwarding.cpp)**
   - ===================================================================
   - Namespaces: lvalues_rvalues, move_semantics
   - Classes: Widget, Resource

43. **[MultiThreadedMicroservices.cpp](src/MultiThreadedMicroservices.cpp)**
   - MultiThreadedMicroservices.cpp
   - Namespaces: std
   - Classes: Logger, ThreadType, ThreadContext


### N

44. **[NlohmannJsonExample.cpp](src/NlohmannJsonExample.cpp)**
   - ===================================================================
   - Functions: example_custom_types, example_arrays, example_basic_json

45. **[NoexceptBestPractices.cpp](src/NoexceptBestPractices.cpp)**
   - ===================================================================
   - Namespaces: what_is_noexcept, always_use_noexcept
   - Classes: Swappable, Resource, MoveableResource

46. **[NVIIdiomTemplateMethod.cpp](src/NVIIdiomTemplateMethod.cpp)**
   - NVIIdiomTemplateMethod.cpp
   - Namespaces: basic_nvi, std
   - Classes: DataProcessor, CSVProcessor, JSONProcessor


### O

47. **[ObjectSlicingCpp20.cpp](src/ObjectSlicingCpp20.cpp)**
   - ===================================================================
   - Classes: Rectangle, Shape, Circle
   - Functions: draw_shape, process_shape, example_concepts_prevent_slicing

48. **[ObjectSlicingSmartPtr.cpp](src/ObjectSlicingSmartPtr.cpp)**
   - ===================================================================
   - Classes: Rectangle, Shape, Circle
   - Functions: example_shared_ptr_slicing_wrong, example_container_slicing_wrong, example_classic_slicing

49. **[OptionalExamples.cpp](src/OptionalExamples.cpp)**
   - ===================================================================
   - Functions: example_value_or, example_optional_return, example_basic_optional


### P

50. **[PerfectForwardingAndRequires.cpp](src/PerfectForwardingAndRequires.cpp)**
   - tests
   - Classes: Serializable, ThreadSafeQueue, Register
   - Functions: call_with_forward, call_by_value, demonstrate_perfect_forwarding

51. **[PimplIdiom.cpp](src/PimplIdiom.cpp)**
   - ===================================================================
   - Namespaces: modern_cpp_pimpl, what_is_pimpl
   - Classes: WidgetWithPimpl, WidgetCpp98, WidgetNoPimpl

52. **[ProtobufExample.cpp](src/ProtobufExample.cpp)**
   - ===================================================================
   - Functions: example_binary_serialization, example_binary_deserialization, example_create_sensor_reading

53. **[Pybind11Example.cpp](src/Pybind11Example.cpp)**
   - PYTHON USAGE EXAMPLES:
   - Namespaces: py
   - Classes: Vector2D, Shape, Circle


### R

54. **[RangesExamples.cpp](src/RangesExamples.cpp)**
   - ===================================================================
   - Namespaces: views, ranges
   - Functions: example_filter_view, example_basic_ranges, example_transform_view

55. **[RealTimeProgramming.cpp](src/RealTimeProgramming.cpp)**
   - RealTimeProgramming.cpp
   - Namespaces: big_o_notation, thread_architecture
   - Classes: SensorSystem, SensorStatus, RealTimeFlagManager

56. **[ResourceLeaks.cpp](src/ResourceLeaks.cpp)**
   - EXPECTED OUTPUT HIGHLIGHTS:
   - Namespaces: OldCpp, ModernCpp
   - Classes: Node, ResourceIntensive, ExpensiveResource

57. **[RestApiExample.cpp](src/RestApiExample.cpp)**
   - ===================================================================
   - Classes: CurlHandle, RestClient, CurlHeaders
   - Functions: example_create_post, example_json_placeholder_api, example_update_post

58. **[ROMability.cpp](src/ROMability.cpp)**
   - ROMability.cpp
   - Namespaces: rom_arrays, const_vs_constexpr
   - Functions: demonstrate, get_runtime_value

59. **[RuleOf3_5_0.cpp](src/RuleOf3_5_0.cpp)**
   - no-op
   - Classes: RuleOfThreeExample, RuleOfZeroExample, RuleOfFiveExample
   - Functions: example_rule_of_zero, example_rule_of_three, example_rule_of_five

60. **[RuntimePolymorphism.cpp](src/RuntimePolymorphism.cpp)**
   - ===================================================================
   - Classes: Rectangle, IShape, Circle
   - Functions: example_virtual_with_default, example_virtual_destructor, example_pure_virtual


### S

61. **[SafetyCriticalSTLContainers.cpp](src/SafetyCriticalSTLContainers.cpp)**
   - ===================================================================
   - Namespaces: containers_to_avoid, container_classification
   - Classes: SafetyPoolAllocator

62. **[SearchAnagramsDictionary.cpp](src/SearchAnagramsDictionary.cpp)**
   - Example program
   - Functions: main

63. **[SinglyLinkedList.cpp](src/SinglyLinkedList.cpp)**
   - pHead
   - Functions: main, print

64. **[SOLIDPrinciples.cpp](src/SOLIDPrinciples.cpp)**
   - ===================================================================
   - Namespaces: open_closed, single_responsibility
   - Classes: TaxCalculator, BadEmployee, Employee

65. **[STLContainersNoHeap.cpp](src/STLContainersNoHeap.cpp)**
   - ===================================================================
   - Classes: PoolAllocator, StackAllocator, FixedVector
   - Functions: example_std_array, example_std_span, process_data

66. **[StopTokenExample.cpp](src/StopTokenExample.cpp)**
   - StopTokenExample.cpp
   - Namespaces: old_vs_new, std
   - Classes: OldStyleThread, RESTService, NewStyleThread

67. **[StructuredBindings.cpp](src/StructuredBindings.cpp)**
   - ===================================================================
   - Functions: example_array_bindings, example_basic_structured_bindings, example_tuple_bindings

68. **[SystemInteractionAndParsing.cpp](src/SystemInteractionAndParsing.cpp)**
   - ===================================================================
   - Namespaces: fs
   - Classes: ThreadSafeLogger, BankAccount, SharedCounter


### T

69. **[TemplatedCameraInterface.cpp](src/TemplatedCameraInterface.cpp)**
   - ===================================================================
   - Classes: Image, Camera8bit, Camera
   - Functions: demonstrate_float_camera, demonstrate_8bit_camera, demonstrate_16bit_camera

70. **[TemplatedCameraModules.cpp](src/TemplatedCameraModules.cpp)**
   - ===================================================================
   - Functions: demonstrate_concepts, display_camera_info, demonstrate_camera_capture

71. **[ThreadPoolExamples.cpp](src/ThreadPoolExamples.cpp)**
   - ThreadPoolExamples.cpp
   - Namespaces: std
   - Classes: BasicThreadPool, ThreadPoolWithFutures, TaskPriority

72. **[TuplesAndStructuredBindings.cpp](src/TuplesAndStructuredBindings.cpp)**
   - ===================================================================
   - Functions: demonstrate_structured_bindings, demonstrate_tie, demonstrate_basic_tuples


### U

73. **[UniversalResourceManager.cpp](src/UniversalResourceManager.cpp)**
   - ===================================================================
   - Namespaces: std
   - Functions: database_example, file_example, explain_the_problem


### V

74. **[VariadicTemplateRecursion.cpp](src/VariadicTemplateRecursion.cpp)**
   - ===================================================================
   - Classes: GPIO_Pin
   - Functions: print_recursive, demonstrate_basic_recursion

75. **[VirtualFunctionsInTemplates.cpp](src/VirtualFunctionsInTemplates.cpp)**
   - VIRTUAL FUNCTIONS IN TEMPLATES: THE CODE BLOAT PROBLEM
   - Namespaces: bad_example_t80, the_problem
   - Classes: DemoInstantiation, Vector, template

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
- **Embedded Systems**: [EmbeddedSystemsProgramming.cpp](src/EmbeddedSystemsProgramming.cpp), [EmbeddedThreadSafetyReentrancy.cpp](src/EmbeddedThreadSafetyReentrancy.cpp), [ROMability.cpp](src/ROMability.cpp), [ARMInstructionSets.cpp](src/ARMInstructionSets.cpp)
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
