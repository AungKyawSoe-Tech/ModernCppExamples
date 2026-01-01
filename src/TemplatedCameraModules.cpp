// ===================================================================
// C++20 MODULES DEMONSTRATION: IMPORT AND USE CAMERA MODULE
// ===================================================================
// This file demonstrates:
// 1. Importing C++20 modules with "import"
// 2. Using exported templates, classes, and concepts
// 3. C++20 features: concepts, requires clauses, [[nodiscard]]
// ===================================================================

#include <iostream>
#include <iomanip>
#include <chrono>

// C++20 MODULE IMPORT
import camera;

// ===================================================================
// DEMONSTRATION FUNCTIONS
// ===================================================================

void demonstrate_concepts() {
    std::cout << "\n=== C++20 CONCEPTS DEMONSTRATION ===" << std::endl;
    
    std::cout << "\nPixelType concept checks:" << std::endl;
    std::cout << "  uint8_t is PixelType: " << PixelType<uint8_t> << std::endl;
    std::cout << "  uint16_t is PixelType: " << PixelType<uint16_t> << std::endl;
    std::cout << "  float is PixelType: " << PixelType<float> << std::endl;
    std::cout << "  double is PixelType: " << PixelType<double> << std::endl;
    std::cout << "  int is PixelType: " << PixelType<int> << " (not in allowed list)" << std::endl;
    
    std::cout << "\nIntegerPixel concept checks:" << std::endl;
    std::cout << "  uint8_t is IntegerPixel: " << IntegerPixel<uint8_t> << std::endl;
    std::cout << "  float is IntegerPixel: " << IntegerPixel<float> << std::endl;
    
    std::cout << "\nFloatingPixel concept checks:" << std::endl;
    std::cout << "  float is FloatingPixel: " << FloatingPixel<float> << std::endl;
    std::cout << "  uint8_t is FloatingPixel: " << FloatingPixel<uint8_t> << std::endl;
}

template<PixelType T>
void display_camera_info(const Camera<T>& camera) {
    std::cout << "\n  Camera: " << camera.get_name() << std::endl;
    std::cout << "  Resolution: " << camera.get_width() << "x" << camera.get_height() << std::endl;
    std::cout << "  Bits per pixel: " << Camera<T>::bits_per_pixel() << std::endl;
    std::cout << "  Floating point: " << (Camera<T>::is_floating_point() ? "Yes" : "No") << std::endl;
    
    size_t bytes = camera.get_width() * camera.get_height() * sizeof(T);
    std::cout << "  Memory per frame: " << bytes << " bytes";
    if (bytes >= 1024 * 1024) {
        std::cout << " (" << std::fixed << std::setprecision(2) 
                  << bytes / (1024.0 * 1024.0) << " MB)";
    } else if (bytes >= 1024) {
        std::cout << " (" << std::fixed << std::setprecision(2) 
                  << bytes / 1024.0 << " KB)";
    }
    std::cout << std::endl;
}

template<PixelType T>
void demonstrate_camera_capture(CameraHandler<T>& handler) {
    const Camera<T>* cam = handler.get_camera();
    
    std::cout << "\n  Capturing image..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    Image<T> img = handler.capture();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "  Capture time: " << duration.count() << " µs" << std::endl;
    
    std::cout << "\n  Processing image..." << std::endl;
    double mean = ImageProcessor<T>::calculate_mean(img);
    auto [min_val, max_val] = ImageProcessor<T>::find_min_max(img);
    
    std::cout << "  Mean value: " << mean << std::endl;
    std::cout << "  Min value: " << static_cast<double>(min_val) << std::endl;
    std::cout << "  Max value: " << static_cast<double>(max_val) << std::endl;
    
    std::cout << "\n  Sample pixels:" << std::endl;
    std::cout << "    Top-left (0,0): " << static_cast<double>(img.at(0, 0)) << std::endl;
    std::cout << "    Center (" << img.get_width()/2 << "," << img.get_height()/2 << "): " 
              << static_cast<double>(img.at(img.get_width()/2, img.get_height()/2)) << std::endl;
}

void demonstrate_8bit_camera() {
    std::cout << "\n=== 1. 8-BIT CAMERA (uint8_t) ===" << std::endl;
    
    CameraHandler<uint8_t> handler(
        std::make_unique<Camera8bit>("Webcam HD", 640, 480)
    );
    
    display_camera_info(*handler.get_camera());
    demonstrate_camera_capture(handler);
}

void demonstrate_16bit_camera() {
    std::cout << "\n=== 2. 16-BIT CAMERA (uint16_t) ===" << std::endl;
    
    CameraHandler<uint16_t> handler(
        std::make_unique<Camera16bit>("Scientific CCD", 1024, 1024)
    );
    
    display_camera_info(*handler.get_camera());
    demonstrate_camera_capture(handler);
}

void demonstrate_float_camera() {
    std::cout << "\n=== 3. FLOAT CAMERA (32-bit) ===" << std::endl;
    
    CameraHandler<float> handler(
        std::make_unique<CameraFloat>("ToF Depth Camera", 320, 240)
    );
    
    display_camera_info(*handler.get_camera());
    demonstrate_camera_capture(handler);
}

void demonstrate_double_camera() {
    std::cout << "\n=== 4. DOUBLE CAMERA (64-bit) ===" << std::endl;
    
    CameraHandler<double> handler(
        std::make_unique<CameraDouble>("Telescope CCD", 512, 512)
    );
    
    display_camera_info(*handler.get_camera());
    demonstrate_camera_capture(handler);
}

void demonstrate_type_conversion() {
    std::cout << "\n=== 5. IMAGE TYPE CONVERSION ===" << std::endl;
    
    Camera8bit cam8("Source Camera", 320, 240);
    Image<uint8_t> img8 = cam8.capture();
    
    std::cout << "\nOriginal 8-bit image:" << std::endl;
    std::cout << "  Size: " << img8.memory_bytes() << " bytes" << std::endl;
    auto [min8, max8] = ImageProcessor<uint8_t>::find_min_max(img8);
    std::cout << "  Range: " << (int)min8 << " to " << (int)max8 << std::endl;
    
    std::cout << "\nConverting to 16-bit..." << std::endl;
    Image<uint16_t> img16 = convert_image<uint16_t>(img8);
    std::cout << "  Size: " << img16.memory_bytes() << " bytes" << std::endl;
    auto [min16, max16] = ImageProcessor<uint16_t>::find_min_max(img16);
    std::cout << "  Range: " << min16 << " to " << max16 << std::endl;
    
    std::cout << "\nConverting to float..." << std::endl;
    Image<float> imgf = convert_image<float>(img8);
    std::cout << "  Size: " << imgf.memory_bytes() << " bytes" << std::endl;
    auto [minf, maxf] = ImageProcessor<float>::find_min_max(imgf);
    std::cout << "  Range: " << minf << " to " << maxf << std::endl;
    
    std::cout << "\nConverting to double..." << std::endl;
    Image<double> imgd = convert_image<double>(img8);
    std::cout << "  Size: " << imgd.memory_bytes() << " bytes" << std::endl;
    auto [mind, maxd] = ImageProcessor<double>::find_min_max(imgd);
    std::cout << "  Range: " << mind << " to " << maxd << std::endl;
}

void demonstrate_concept_constrained_operations() {
    std::cout << "\n=== 6. CONCEPT-CONSTRAINED OPERATIONS ===" << std::endl;
    
    // Integer pixel operations
    std::cout << "\nInteger pixel operations (uint8_t):" << std::endl;
    Camera8bit cam8("Integer Camera", 100, 100);
    Image<uint8_t> img8 = cam8.capture();
    
    std::cout << "  Original mean: " << ImageProcessor<uint8_t>::calculate_mean(img8) << std::endl;
    
    // Threshold only works with IntegerPixel concept
    Image<uint8_t> thresholded = ImageProcessor<uint8_t>::threshold(img8, 128);
    size_t above = 0;
    for (size_t i = 0; i < thresholded.get_size(); ++i) {
        if (thresholded.data()[i] > 0) ++above;
    }
    std::cout << "  Pixels above threshold: " << above 
              << " (" << (100.0 * above / thresholded.get_size()) << "%)" << std::endl;
    
    // Floating point operations
    std::cout << "\nFloating pixel operations (float):" << std::endl;
    CameraFloat camf("Float Camera", 100, 100);
    Image<float> imgf = camf.capture();
    
    std::cout << "  Original mean: " << ImageProcessor<float>::calculate_mean(imgf) << std::endl;
    
    // Normalize only works with FloatingPixel concept
    Image<float> normalized = ImageProcessor<float>::normalize(imgf);
    std::cout << "  Normalized mean: " << ImageProcessor<float>::calculate_mean(normalized) << std::endl;
    auto [minf, maxf] = ImageProcessor<float>::find_min_max(normalized);
    std::cout << "  Normalized range: " << minf << " to " << maxf << std::endl;
    
    // Note: The following would NOT compile (concept constraint):
    // Image<float> bad = ImageProcessor<float>::threshold(imgf, 0.5f); // ERROR: requires IntegerPixel
    // Image<uint8_t> bad2 = ImageProcessor<uint8_t>::normalize(img8);  // ERROR: requires FloatingPixel
    
    std::cout << "\n  ✅ Concept constraints prevent invalid operations at compile time!" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  C++20 MODULES: TEMPLATED CAMERA INTERFACE" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    demonstrate_concepts();
    demonstrate_8bit_camera();
    demonstrate_16bit_camera();
    demonstrate_float_camera();
    demonstrate_double_camera();
    demonstrate_type_conversion();
    demonstrate_concept_constrained_operations();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  C++20 FEATURES DEMONSTRATED" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n✅ MODULES (import/export):" << std::endl;
    std::cout << "   • export module camera - Module interface declaration" << std::endl;
    std::cout << "   • import camera - Import module in main program" << std::endl;
    std::cout << "   • Exported classes: Image<T>, Camera<T>, ImageProcessor<T>" << std::endl;
    std::cout << "   • Exported functions: convert_image<T1, T2>()" << std::endl;
    std::cout << "   • Exported concepts: PixelType, IntegerPixel, FloatingPixel" << std::endl;
    
    std::cout << "\n✅ CONCEPTS:" << std::endl;
    std::cout << "   • concept PixelType - Constrain allowed pixel types" << std::endl;
    std::cout << "   • concept IntegerPixel - Only integer pixel types" << std::endl;
    std::cout << "   • concept FloatingPixel - Only floating-point pixel types" << std::endl;
    std::cout << "   • requires clauses on functions (threshold, normalize)" << std::endl;
    std::cout << "   • Compile-time type checking prevents invalid operations" << std::endl;
    
    std::cout << "\n✅ OTHER C++20 FEATURES:" << std::endl;
    std::cout << "   • [[nodiscard]] - Warn if return value ignored" << std::endl;
    std::cout << "   • Three-way comparison (operator<=>) - Auto-generated comparisons" << std::endl;
    std::cout << "   • noexcept specifications - Exception safety guarantees" << std::endl;
    
    std::cout << "\n✅ MODULE BENEFITS:" << std::endl;
    std::cout << "   • Faster compilation - No header parsing overhead" << std::endl;
    std::cout << "   • Better encapsulation - Only exported entities visible" << std::endl;
    std::cout << "   • No include guards needed - Modules included once automatically" << std::endl;
    std::cout << "   • Order independent - No macro contamination" << std::endl;
    std::cout << "   • Cleaner code - Explicit export declarations" << std::endl;
    
    std::cout << "\n📚 COMPILATION NOTES:" << std::endl;
    std::cout << "   GCC 11+:" << std::endl;
    std::cout << "     g++ -std=c++20 -fmodules-ts -xc++-system-header iostream" << std::endl;
    std::cout << "     g++ -std=c++20 -fmodules-ts -c CameraModule.cppm" << std::endl;
    std::cout << "     g++ -std=c++20 -fmodules-ts TemplatedCameraModules.cpp CameraModule.o" << std::endl;
    
    std::cout << "\n   MSVC 2022+:" << std::endl;
    std::cout << "     cl /std:c++20 /experimental:module /c CameraModule.cppm" << std::endl;
    std::cout << "     cl /std:c++20 /experimental:module TemplatedCameraModules.cpp CameraModule.obj" << std::endl;
    
    std::cout << "\n   Clang 16+:" << std::endl;
    std::cout << "     clang++ -std=c++20 -fmodules -c CameraModule.cppm" << std::endl;
    std::cout << "     clang++ -std=c++20 -fmodules TemplatedCameraModules.cpp CameraModule.o" << std::endl;
    
    std::cout << "\n⚠️  NOTE: C++20 modules support varies by compiler!" << std::endl;
    std::cout << "   • GCC: Experimental support with -fmodules-ts" << std::endl;
    std::cout << "   • MSVC: Best support with /std:c++20 and /experimental:module" << std::endl;
    std::cout << "   • Clang: Good support in recent versions (16+)" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
