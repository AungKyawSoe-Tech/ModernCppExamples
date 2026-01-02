// ===================================================================
// CREATING A C API FROM C++ CODE
// ===================================================================
// This example demonstrates how to expose C++ functionality through
// a C-compatible API. This is the REVERSE direction from the previous
// example - we're wrapping C++ in C, not C in C++.
//
// USE CASES:
// - Creating libraries usable from C code
// - Plugin systems with C interfaces
// - FFI (Foreign Function Interface) for Python, Rust, etc.
// - Legacy code integration
// - Stable ABI across compiler versions
//
// TOPICS COVERED:
// 1. Opaque pointers (pimpl idiom for C API)
// 2. extern "C" for C linkage
// 3. Exception handling across C boundary
// 4. C++ features hidden behind C API
// 5. Memory management strategies
// 6. Header guards and C/C++ compatibility
// ===================================================================

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <cstdint>

// ===================================================================
// SECTION 1: C++ IMPLEMENTATION (INTERNAL)
// ===================================================================
// This is the actual C++ code with modern features that we'll expose
// through a C API

namespace image_processing {

class Image {
private:
    size_t width_;
    size_t height_;
    std::vector<uint8_t> pixels_;
    
public:
    Image(size_t width, size_t height) 
        : width_(width), height_(height), pixels_(width * height, 0) {
        std::cout << "✓ C++ Image created (" << width_ << "x" << height_ << ")" << std::endl;
    }
    
    ~Image() {
        std::cout << "✓ C++ Image destroyed" << std::endl;
    }
    
    // C++ features: const correctness, exceptions
    size_t width() const noexcept { return width_; }
    size_t height() const noexcept { return height_; }
    
    uint8_t& at(size_t x, size_t y) {
        if (x >= width_ || y >= height_) {
            throw std::out_of_range("Pixel coordinates out of range");
        }
        return pixels_[y * width_ + x];
    }
    
    const uint8_t& at(size_t x, size_t y) const {
        if (x >= width_ || y >= height_) {
            throw std::out_of_range("Pixel coordinates out of range");
        }
        return pixels_[y * width_ + x];
    }
    
    // Modern C++ algorithm
    void invert() {
        std::transform(pixels_.begin(), pixels_.end(), pixels_.begin(),
                      [](uint8_t pixel) { return 255 - pixel; });
        std::cout << "✓ Image inverted (C++ algorithm)" << std::endl;
    }
    
    void fill(uint8_t value) {
        std::fill(pixels_.begin(), pixels_.end(), value);
        std::cout << "✓ Image filled with value " << static_cast<int>(value) << std::endl;
    }
    
    void apply_threshold(uint8_t threshold) {
        for (auto& pixel : pixels_) {
            pixel = (pixel >= threshold) ? 255 : 0;
        }
        std::cout << "✓ Threshold applied at " << static_cast<int>(threshold) << std::endl;
    }
    
    // Get raw data
    const uint8_t* data() const noexcept { return pixels_.data(); }
    size_t size() const noexcept { return pixels_.size(); }
};

} // namespace image_processing

// ===================================================================
// SECTION 2: C API HEADER (WHAT C CODE SEES)
// ===================================================================
// This is what would go in a .h file that C programs include

#ifdef __cplusplus
extern "C" {
#endif

// Opaque handle - C code sees this as incomplete type
// This hides the C++ implementation details
typedef struct ImageHandle* ImageHandle_t;

// Error codes (C-compatible enum)
typedef enum {
    IMAGE_SUCCESS = 0,
    IMAGE_ERROR_INVALID_HANDLE = -1,
    IMAGE_ERROR_INVALID_DIMENSIONS = -2,
    IMAGE_ERROR_OUT_OF_RANGE = -3,
    IMAGE_ERROR_OUT_OF_MEMORY = -4,
    IMAGE_ERROR_UNKNOWN = -99
} ImageError;

// C API functions
// Note: All return error codes, use output parameters for data

/**
 * Create a new image
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @param out_handle Output parameter for image handle
 * @return Error code
 */
ImageError image_create(size_t width, size_t height, ImageHandle_t* out_handle);

/**
 * Destroy an image and free resources
 * @param handle Image handle
 * @return Error code
 */
ImageError image_destroy(ImageHandle_t handle);

/**
 * Get image dimensions
 * @param handle Image handle
 * @param out_width Output parameter for width
 * @param out_height Output parameter for height
 * @return Error code
 */
ImageError image_get_dimensions(ImageHandle_t handle, size_t* out_width, size_t* out_height);

/**
 * Set pixel value
 * @param handle Image handle
 * @param x X coordinate
 * @param y Y coordinate
 * @param value Pixel value (0-255)
 * @return Error code
 */
ImageError image_set_pixel(ImageHandle_t handle, size_t x, size_t y, uint8_t value);

/**
 * Get pixel value
 * @param handle Image handle
 * @param x X coordinate
 * @param y Y coordinate
 * @param out_value Output parameter for pixel value
 * @return Error code
 */
ImageError image_get_pixel(ImageHandle_t handle, size_t x, size_t y, uint8_t* out_value);

/**
 * Invert all pixel values
 * @param handle Image handle
 * @return Error code
 */
ImageError image_invert(ImageHandle_t handle);

/**
 * Fill image with a value
 * @param handle Image handle
 * @param value Fill value (0-255)
 * @return Error code
 */
ImageError image_fill(ImageHandle_t handle, uint8_t value);

/**
 * Apply threshold to image
 * @param handle Image handle
 * @param threshold Threshold value (0-255)
 * @return Error code
 */
ImageError image_apply_threshold(ImageHandle_t handle, uint8_t threshold);

/**
 * Get error message for error code
 * @param error Error code
 * @return Human-readable error message (static string)
 */
const char* image_error_string(ImageError error);

#ifdef __cplusplus
}
#endif

// ===================================================================
// SECTION 3: C API IMPLEMENTATION (BRIDGE LAYER)
// ===================================================================
// This bridges between C and C++ code

// Helper function to catch C++ exceptions and convert to error codes
// NOTE: Must be OUTSIDE extern "C" block since it's a template
template<typename Func>
ImageError safe_call(Func&& func) noexcept {
    try {
        func();
        return IMAGE_SUCCESS;
    } catch (const std::out_of_range& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return IMAGE_ERROR_OUT_OF_RANGE;
    } catch (const std::bad_alloc& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return IMAGE_ERROR_OUT_OF_MEMORY;
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return IMAGE_ERROR_UNKNOWN;
    } catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
        return IMAGE_ERROR_UNKNOWN;
    }
}

// The actual implementation uses extern "C"
extern "C" {

ImageError image_create(size_t width, size_t height, ImageHandle_t* out_handle) {
    if (!out_handle) {
        return IMAGE_ERROR_INVALID_HANDLE;
    }
    
    if (width == 0 || height == 0) {
        return IMAGE_ERROR_INVALID_DIMENSIONS;
    }
    
    return safe_call([&]() {
        // Create C++ object and cast to opaque handle
        auto* img = new image_processing::Image(width, height);
        *out_handle = reinterpret_cast<ImageHandle_t>(img);
    });
}

ImageError image_destroy(ImageHandle_t handle) {
    if (!handle) {
        return IMAGE_ERROR_INVALID_HANDLE;
    }
    
    return safe_call([&]() {
        // Cast back to C++ object and delete
        auto* img = reinterpret_cast<image_processing::Image*>(handle);
        delete img;
    });
}

ImageError image_get_dimensions(ImageHandle_t handle, size_t* out_width, size_t* out_height) {
    if (!handle || !out_width || !out_height) {
        return IMAGE_ERROR_INVALID_HANDLE;
    }
    
    return safe_call([&]() {
        auto* img = reinterpret_cast<image_processing::Image*>(handle);
        *out_width = img->width();
        *out_height = img->height();
    });
}

ImageError image_set_pixel(ImageHandle_t handle, size_t x, size_t y, uint8_t value) {
    if (!handle) {
        return IMAGE_ERROR_INVALID_HANDLE;
    }
    
    return safe_call([&]() {
        auto* img = reinterpret_cast<image_processing::Image*>(handle);
        img->at(x, y) = value;
    });
}

ImageError image_get_pixel(ImageHandle_t handle, size_t x, size_t y, uint8_t* out_value) {
    if (!handle || !out_value) {
        return IMAGE_ERROR_INVALID_HANDLE;
    }
    
    return safe_call([&]() {
        auto* img = reinterpret_cast<image_processing::Image*>(handle);
        *out_value = img->at(x, y);
    });
}

ImageError image_invert(ImageHandle_t handle) {
    if (!handle) {
        return IMAGE_ERROR_INVALID_HANDLE;
    }
    
    return safe_call([&]() {
        auto* img = reinterpret_cast<image_processing::Image*>(handle);
        img->invert();
    });
}

ImageError image_fill(ImageHandle_t handle, uint8_t value) {
    if (!handle) {
        return IMAGE_ERROR_INVALID_HANDLE;
    }
    
    return safe_call([&]() {
        auto* img = reinterpret_cast<image_processing::Image*>(handle);
        img->fill(value);
    });
}

ImageError image_apply_threshold(ImageHandle_t handle, uint8_t threshold) {
    if (!handle) {
        return IMAGE_ERROR_INVALID_HANDLE;
    }
    
    return safe_call([&]() {
        auto* img = reinterpret_cast<image_processing::Image*>(handle);
        img->apply_threshold(threshold);
    });
}

const char* image_error_string(ImageError error) {
    switch (error) {
        case IMAGE_SUCCESS: return "Success";
        case IMAGE_ERROR_INVALID_HANDLE: return "Invalid handle";
        case IMAGE_ERROR_INVALID_DIMENSIONS: return "Invalid dimensions";
        case IMAGE_ERROR_OUT_OF_RANGE: return "Coordinates out of range";
        case IMAGE_ERROR_OUT_OF_MEMORY: return "Out of memory";
        case IMAGE_ERROR_UNKNOWN: return "Unknown error";
        default: return "Invalid error code";
    }
}

} // extern "C"

// ===================================================================
// SECTION 4: DEMONSTRATION - USING THE C API
// ===================================================================

void demonstrate_c_api_usage() {
    std::cout << "\n=== USING C API (SIMULATING C CODE) ===" << std::endl;
    std::cout << "Note: This C++ code simulates how C code would use the API\n" << std::endl;
    
    // In real C code, these would be the only includes needed:
    // #include "image_api.h"
    
    ImageHandle_t image = nullptr;
    ImageError err;
    
    // Create image
    std::cout << "1. Creating 10x10 image:" << std::endl;
    err = image_create(10, 10, &image);
    if (err != IMAGE_SUCCESS) {
        std::cout << "   Error: " << image_error_string(err) << std::endl;
        return;
    }
    std::cout << "   ✓ Image created successfully" << std::endl;
    
    // Get dimensions
    size_t width, height;
    err = image_get_dimensions(image, &width, &height);
    if (err == IMAGE_SUCCESS) {
        std::cout << "\n2. Image dimensions: " << width << "x" << height << std::endl;
    }
    
    // Fill with value
    std::cout << "\n3. Filling image with value 128:" << std::endl;
    err = image_fill(image, 128);
    if (err != IMAGE_SUCCESS) {
        std::cout << "   Error: " << image_error_string(err) << std::endl;
    }
    
    // Set some pixels
    std::cout << "\n4. Setting individual pixels:" << std::endl;
    image_set_pixel(image, 0, 0, 255);
    image_set_pixel(image, 1, 1, 200);
    image_set_pixel(image, 2, 2, 150);
    std::cout << "   ✓ Pixels set" << std::endl;
    
    // Get pixel values
    std::cout << "\n5. Reading pixel values:" << std::endl;
    uint8_t value;
    for (int i = 0; i < 3; i++) {
        err = image_get_pixel(image, i, i, &value);
        if (err == IMAGE_SUCCESS) {
            std::cout << "   Pixel(" << i << "," << i << ") = " 
                     << static_cast<int>(value) << std::endl;
        }
    }
    
    // Invert
    std::cout << "\n6. Inverting image:" << std::endl;
    err = image_invert(image);
    if (err != IMAGE_SUCCESS) {
        std::cout << "   Error: " << image_error_string(err) << std::endl;
    }
    
    // Read inverted values
    std::cout << "\n7. Reading inverted pixel values:" << std::endl;
    for (int i = 0; i < 3; i++) {
        err = image_get_pixel(image, i, i, &value);
        if (err == IMAGE_SUCCESS) {
            std::cout << "   Pixel(" << i << "," << i << ") = " 
                     << static_cast<int>(value) << std::endl;
        }
    }
    
    // Apply threshold
    std::cout << "\n8. Applying threshold at 100:" << std::endl;
    err = image_apply_threshold(image, 100);
    
    // Error handling
    std::cout << "\n9. Testing error handling (out of range access):" << std::endl;
    err = image_get_pixel(image, 100, 100, &value);
    if (err != IMAGE_SUCCESS) {
        std::cout << "   ✓ Error caught: " << image_error_string(err) << std::endl;
    }
    
    // Cleanup
    std::cout << "\n10. Destroying image:" << std::endl;
    err = image_destroy(image);
    if (err == IMAGE_SUCCESS) {
        std::cout << "   ✓ Image destroyed successfully" << std::endl;
    }
}

// ===================================================================
// SECTION 5: BEST PRACTICES EXPLANATION
// ===================================================================

void explain_best_practices() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "BEST PRACTICES FOR C API FROM C++ CODE:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n1. OPAQUE HANDLES (PIMPL IDIOM FOR C):" << std::endl;
    std::cout << "   ✓ Hide C++ implementation details" << std::endl;
    std::cout << "   ✓ Use typedef struct Name* Handle_t;" << std::endl;
    std::cout << "   ✓ Cast to/from C++ objects internally" << std::endl;
    std::cout << "   ✓ Stable ABI - C++ changes don't affect C API" << std::endl;
    
    std::cout << "\n2. EXCEPTION HANDLING:" << std::endl;
    std::cout << "   ✓ NEVER let C++ exceptions cross C boundary!" << std::endl;
    std::cout << "   ✓ Catch ALL exceptions in extern \"C\" functions" << std::endl;
    std::cout << "   ✓ Convert exceptions to error codes" << std::endl;
    std::cout << "   ✓ Use try-catch wrappers consistently" << std::endl;
    
    std::cout << "\n3. ERROR HANDLING:" << std::endl;
    std::cout << "   ✓ Return error codes (int or enum)" << std::endl;
    std::cout << "   ✓ Use output parameters for data" << std::endl;
    std::cout << "   ✓ Provide error_to_string() function" << std::endl;
    std::cout << "   ✓ Check for NULL pointers before dereferencing" << std::endl;
    
    std::cout << "\n4. MEMORY MANAGEMENT:" << std::endl;
    std::cout << "   ✓ C code creates/destroys via API functions" << std::endl;
    std::cout << "   ✓ NEVER expose C++ new/delete directly" << std::endl;
    std::cout << "   ✓ Provide create() and destroy() functions" << std::endl;
    std::cout << "   ✓ Match create/destroy across DLL boundaries" << std::endl;
    
    std::cout << "\n5. HEADER GUARDS AND COMPATIBILITY:" << std::endl;
    std::cout << "   ✓ Use #ifdef __cplusplus for extern \"C\" blocks" << std::endl;
    std::cout << "   ✓ Include guards in all headers" << std::endl;
    std::cout << "   ✓ Use C-compatible types (no bool, use int)" << std::endl;
    std::cout << "   ✓ No function overloading in C API" << std::endl;
    
    std::cout << "\n6. C++ FEATURES TO AVOID IN C API:" << std::endl;
    std::cout << "   ❌ Classes (use opaque handles instead)" << std::endl;
    std::cout << "   ❌ Templates (not C-compatible)" << std::endl;
    std::cout << "   ❌ Function overloading (C doesn't support it)" << std::endl;
    std::cout << "   ❌ Default arguments (C doesn't support them)" << std::endl;
    std::cout << "   ❌ References (use pointers)" << std::endl;
    std::cout << "   ❌ bool type (use int, 0 for false, 1 for true)" << std::endl;
    
    std::cout << "\n7. DOCUMENTATION:" << std::endl;
    std::cout << "   ✓ Document ownership semantics clearly" << std::endl;
    std::cout << "   ✓ Specify thread-safety guarantees" << std::endl;
    std::cout << "   ✓ List all possible error codes" << std::endl;
    std::cout << "   ✓ Provide usage examples" << std::endl;
    
    std::cout << "\n8. TESTING:" << std::endl;
    std::cout << "   ✓ Test C API from actual C code" << std::endl;
    std::cout << "   ✓ Verify exception safety" << std::endl;
    std::cout << "   ✓ Test error conditions" << std::endl;
    std::cout << "   ✓ Check for memory leaks" << std::endl;
}

// ===================================================================
// SECTION 6: COMPARISON WITH PREVIOUS EXAMPLE
// ===================================================================

void compare_approaches() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "COMPARISON: C IN C++ vs C++ IN C:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\nPREVIOUS EXAMPLE (Wrapping C in C++):" << std::endl;
    std::cout << "  Purpose: Use C libraries from C++ code" << std::endl;
    std::cout << "  Technique: RAII wrappers, smart pointers" << std::endl;
    std::cout << "  Example: Socket library → C++ Socket class" << std::endl;
    std::cout << "  Benefits: Modern C++ safety, automatic cleanup" << std::endl;
    
    std::cout << "\nTHIS EXAMPLE (Exposing C++ as C):" << std::endl;
    std::cout << "  Purpose: Make C++ code usable from C" << std::endl;
    std::cout << "  Technique: Opaque handles, extern \"C\"" << std::endl;
    std::cout << "  Example: C++ Image class → C image_* functions" << std::endl;
    std::cout << "  Benefits: C compatibility, stable ABI, FFI-ready" << std::endl;
    
    std::cout << "\nWHEN TO USE EACH APPROACH:" << std::endl;
    std::cout << "\nWrapping C in C++ (Previous):" << std::endl;
    std::cout << "  • You have C libraries to use" << std::endl;
    std::cout << "  • You want modern C++ features" << std::endl;
    std::cout << "  • You want automatic resource management" << std::endl;
    std::cout << "  • Internal project use" << std::endl;
    
    std::cout << "\nExposing C++ as C (This):" << std::endl;
    std::cout << "  • You want to create a C-compatible library" << std::endl;
    std::cout << "  • You need stable ABI across versions" << std::endl;
    std::cout << "  • You want FFI for other languages" << std::endl;
    std::cout << "  • You need to support legacy C code" << std::endl;
    std::cout << "  • Plugin systems with C interfaces" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║        CREATING A C API FROM C++ CODE                           ║\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "║  Demonstrates: Opaque Handles, extern \"C\", Exception Safety     ║\n";
    std::cout << "║  Example: Image Processing Library with C Interface            ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    demonstrate_c_api_usage();
    explain_best_practices();
    compare_approaches();
    
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "SUMMARY:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n✅ KEY TECHNIQUES DEMONSTRATED:" << std::endl;
    std::cout << "   1. Opaque handles for hiding C++ implementation" << std::endl;
    std::cout << "   2. extern \"C\" for C-compatible linkage" << std::endl;
    std::cout << "   3. Exception safety across C/C++ boundary" << std::endl;
    std::cout << "   4. Error code-based error handling" << std::endl;
    std::cout << "   5. Output parameters instead of return values" << std::endl;
    std::cout << "   6. Proper memory management (create/destroy)" << std::endl;
    
    std::cout << "\n✅ REAL-WORLD APPLICATIONS:" << std::endl;
    std::cout << "   • Game engine C APIs (Unity, Unreal plugins)" << std::endl;
    std::cout << "   • Database drivers (SQLite, PostgreSQL)" << std::endl;
    std::cout << "   • Graphics libraries (Vulkan, OpenGL wrappers)" << std::endl;
    std::cout << "   • Compression libraries (zlib, bzip2 style)" << std::endl;
    std::cout << "   • Python/Ruby/Lua bindings via C API" << std::endl;
    
    std::cout << "\n✅ BENEFITS:" << std::endl;
    std::cout << "   • C++ power with C compatibility" << std::endl;
    std::cout << "   • Stable ABI (no name mangling issues)" << std::endl;
    std::cout << "   • Can be used from ANY language with C FFI" << std::endl;
    std::cout << "   • Hide implementation details completely" << std::endl;
    std::cout << "   • Version changes don't break binary compatibility" << std::endl;
    
    std::cout << "\n✅ All resources properly managed!\n" << std::endl;
    
    return 0;
}
