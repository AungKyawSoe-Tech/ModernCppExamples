cmake_minimum_required(VERSION 3.20)
project(ModernCppExamples VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find Eigen3 (optional, for EigenSensorFusion example)
find_package(Eigen3 QUIET)
if(EIGEN3_FOUND)
    message(STATUS "Eigen3 found: ${EIGEN3_VERSION}")
else()
    message(STATUS "Eigen3 not found - EigenSensorFusion will be skipped")
    message(STATUS "Install with: sudo apt-get install libeigen3-dev")
endif()

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# List of executables to build
set(EXECUTABLES
    # Main comprehensive C++ standard examples
    Cpp11Examples
    Cpp14Examples
    Cpp17Examples
    Cpp20Examples
    
    # Special member functions and smart pointer examples
    RuleOf3_5_0
    ObjectSlicingSmartPtr
    ObjectSlicingCpp20
    InheritanceTypes
    DependencyInjection
    PolymorphismRTTI
    AsioAndModernCppConcurrency
    EventDrivenProgramming_Lambdas
    EventDrivenProgramming_Inheritance
    EmbeddedSystemsProgramming
    EmbeddedSystemsAvoid
    TemplatedCameraInterface
    ErrorHandling
    ExceptionWithSourceLocation
    ErrorHandlingStroustrup
    MoveSemantics
    VirtualFunctions
    STLContainersNoHeap
    ARMInstructionSets
    Cpp23Examples
    EigenSensorFusion
    ResourceLeaks
    
    # NOTE: C++20 modules (TemplatedCameraModules) require special compilation:
    # Modules are not yet fully supported by CMake's standard add_executable
    # See MarkDownDocuments/Cpp20Modules.md for manual compilation instructions
    # TemplatedCameraModules  # Requires module support
    
    # Existing algorithm and data structure examples
    BinarySearch
    Cpp17Concurrency
    DeleteNode
    FindCountOfCommonNodes
    FindFirstCommonNode
    FindMaxNoOfConsecutiveOnesFromIntArray
    FindMToLastElement
    LambdaCaptures
    SearchAnagramsDictionary
    SinglyLinkedList
    Tuples
    
    # Feature-specific examples (kept for reference)
    GenericLambdas
    StructuredBindings
    OptionalExamples
    ConceptsExamples
    RangesExamples
)

# Create executable for each source file
foreach(EXECUTABLE ${EXECUTABLES})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${EXECUTABLE}.cpp)
        # Skip EigenSensorFusion if Eigen3 not found
        if(${EXECUTABLE} STREQUAL "EigenSensorFusion" AND NOT EIGEN3_FOUND)
            message(STATUS "Skipping ${EXECUTABLE} (Eigen3 not found)")
            continue()
        endif()
        
        add_executable(${EXECUTABLE} src/${EXECUTABLE}.cpp)
        
        # Set specific C++ standards for different examples
        if(${EXECUTABLE} MATCHES "^(Cpp11Examples)$")
            set_target_properties(${EXECUTABLE} PROPERTIES CXX_STANDARD 11)
        elseif(${EXECUTABLE} MATCHES "^(Cpp14Examples|GenericLambdas)$")
            set_target_properties(${EXECUTABLE} PROPERTIES CXX_STANDARD 14)
        elseif(${EXECUTABLE} MATCHES "^(Cpp17Examples|StructuredBindings|OptionalExamples|Cpp17Concurrency)$")
            set_target_properties(${EXECUTABLE} PROPERTIES CXX_STANDARD 17)
        elseif(${EXECUTABLE} MATCHES "^(Cpp20Examples|ConceptsExamples|RangesExamples)$")
            set_target_properties(${EXECUTABLE} PROPERTIES CXX_STANDARD 20)
        elseif(${EXECUTABLE} MATCHES "^(Cpp23Examples)$")
            set_target_properties(${EXECUTABLE} PROPERTIES CXX_STANDARD 23)
        else()
            # Default to C++20 for other examples
            set_target_properties(${EXECUTABLE} PROPERTIES CXX_STANDARD 20)
        endif()
        
        # Link Eigen3 for EigenSensorFusion
        if(${EXECUTABLE} STREQUAL "EigenSensorFusion" AND EIGEN3_FOUND)
            target_link_libraries(${EXECUTABLE} Eigen3::Eigen)
        endif()
        
        message(STATUS "Added executable: ${EXECUTABLE}")
    else()
        message(WARNING "Source file not found: src/${EXECUTABLE}.cpp")
    endif()
endforeach()

# Print build configuration
message(STATUS "")
message(STATUS "===========================================")
message(STATUS "  Modern C++ Examples Build Configuration")
message(STATUS "===========================================")
message(STATUS "  CMake version: ${CMAKE_VERSION}")
message(STATUS "  C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Default C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "===========================================")
message(STATUS "")
