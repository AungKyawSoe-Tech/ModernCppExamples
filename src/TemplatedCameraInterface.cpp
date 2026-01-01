#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <chrono>

// ===================================================================
// TEMPLATED CAMERA INTERFACE FOR MULTIPLE PIXEL TYPES
// ===================================================================
// Demonstrates C++ templates for camera interfacing where pixel data
// can be various types: uint8_t, uint16_t, float, double
// ===================================================================

// ===================================================================
// 1. BASIC IMAGE CLASS (TEMPLATED BY PIXEL TYPE)
// ===================================================================

template<typename PixelType>
class Image {
private:
    size_t width;
    size_t height;
    std::vector<PixelType> pixels;
    
public:
    Image(size_t w, size_t h) 
        : width(w), height(h), pixels(w * h) {}
    
    Image(size_t w, size_t h, PixelType initial_value) 
        : width(w), height(h), pixels(w * h, initial_value) {}
    
    // Accessors
    size_t get_width() const { return width; }
    size_t get_height() const { return height; }
    size_t get_size() const { return pixels.size(); }
    
    // Pixel access
    PixelType& at(size_t x, size_t y) {
        return pixels[y * width + x];
    }
    
    const PixelType& at(size_t x, size_t y) const {
        return pixels[y * width + x];
    }
    
    // Raw data access (for camera hardware interface)
    PixelType* data() { return pixels.data(); }
    const PixelType* data() const { return pixels.data(); }
    
    // Memory size in bytes
    size_t memory_bytes() const {
        return pixels.size() * sizeof(PixelType);
    }
    
    // Fill with value
    void fill(PixelType value) {
        std::fill(pixels.begin(), pixels.end(), value);
    }
};

// ===================================================================
// 2. CAMERA INTERFACE (TEMPLATED)
// ===================================================================

template<typename PixelType>
class Camera {
private:
    size_t width;
    size_t height;
    std::string camera_name;
    
public:
    Camera(const std::string& name, size_t w, size_t h)
        : camera_name(name), width(w), height(h) {}
    
    virtual ~Camera() = default;
    
    // Pure virtual: capture image
    virtual Image<PixelType> capture() = 0;
    
    // Configuration
    size_t get_width() const { return width; }
    size_t get_height() const { return height; }
    const std::string& get_name() const { return camera_name; }
    
    // Get pixel type information
    static constexpr size_t bits_per_pixel() {
        return sizeof(PixelType) * 8;
    }
    
    static constexpr bool is_floating_point() {
        return std::is_floating_point_v<PixelType>;
    }
    
    static constexpr bool is_integer() {
        return std::is_integral_v<PixelType>;
    }
};

// ===================================================================
// 3. SIMULATED CAMERA IMPLEMENTATIONS
// ===================================================================

// 8-bit grayscale camera (most common)
class Camera8bit : public Camera<uint8_t> {
public:
    Camera8bit(const std::string& name, size_t w, size_t h)
        : Camera<uint8_t>(name, w, h) {}
    
    Image<uint8_t> capture() override {
        Image<uint8_t> img(get_width(), get_height());
        
        // Simulate captured gradient pattern
        for (size_t y = 0; y < get_height(); ++y) {
            for (size_t x = 0; x < get_width(); ++x) {
                uint8_t value = static_cast<uint8_t>(
                    (x * 255.0 / get_width()) * 0.5 + 
                    (y * 255.0 / get_height()) * 0.5
                );
                img.at(x, y) = value;
            }
        }
        return img;
    }
};

// 16-bit camera (scientific/medical imaging)
class Camera16bit : public Camera<uint16_t> {
public:
    Camera16bit(const std::string& name, size_t w, size_t h)
        : Camera<uint16_t>(name, w, h) {}
    
    Image<uint16_t> capture() override {
        Image<uint16_t> img(get_width(), get_height());
        
        // Simulate high dynamic range data
        for (size_t y = 0; y < get_height(); ++y) {
            for (size_t x = 0; x < get_width(); ++x) {
                uint16_t value = static_cast<uint16_t>(
                    (x * 65535.0 / get_width()) * 0.3 + 
                    (y * 65535.0 / get_height()) * 0.7
                );
                img.at(x, y) = value;
            }
        }
        return img;
    }
};

// Float camera (normalized values 0.0-1.0)
class CameraFloat : public Camera<float> {
public:
    CameraFloat(const std::string& name, size_t w, size_t h)
        : Camera<float>(name, w, h) {}
    
    Image<float> capture() override {
        Image<float> img(get_width(), get_height());
        
        // Simulate normalized data with some pattern
        for (size_t y = 0; y < get_height(); ++y) {
            for (size_t x = 0; x < get_width(); ++x) {
                float value = 
                    0.5f + 0.5f * std::sin(x * 0.1f) * std::cos(y * 0.1f);
                img.at(x, y) = value;
            }
        }
        return img;
    }
};

// Double precision camera (research/astronomy)
class CameraDouble : public Camera<double> {
public:
    CameraDouble(const std::string& name, size_t w, size_t h)
        : Camera<double>(name, w, h) {}
    
    Image<double> capture() override {
        Image<double> img(get_width(), get_height());
        
        // Simulate high-precision data
        for (size_t y = 0; y < get_height(); ++y) {
            for (size_t x = 0; x < get_width(); ++x) {
                double value = 
                    std::sin(x * 0.05) * std::cos(y * 0.05) + 
                    std::exp(-((x - get_width()/2.0) * (x - get_width()/2.0) + 
                               (y - get_height()/2.0) * (y - get_height()/2.0)) / 1000.0);
                img.at(x, y) = value;
            }
        }
        return img;
    }
};

// ===================================================================
// 4. IMAGE PROCESSING ALGORITHMS (TEMPLATED)
// ===================================================================

template<typename PixelType>
class ImageProcessor {
public:
    // Calculate average pixel value
    static double calculate_mean(const Image<PixelType>& img) {
        double sum = 0.0;
        const PixelType* data = img.data();
        size_t size = img.get_size();
        
        for (size_t i = 0; i < size; ++i) {
            sum += static_cast<double>(data[i]);
        }
        
        return sum / size;
    }
    
    // Find min and max pixel values
    static std::pair<PixelType, PixelType> find_min_max(const Image<PixelType>& img) {
        const PixelType* data = img.data();
        size_t size = img.get_size();
        
        PixelType min_val = data[0];
        PixelType max_val = data[0];
        
        for (size_t i = 1; i < size; ++i) {
            if (data[i] < min_val) min_val = data[i];
            if (data[i] > max_val) max_val = data[i];
        }
        
        return {min_val, max_val};
    }
    
    // Scale pixel values
    static Image<PixelType> scale(const Image<PixelType>& img, double factor) {
        Image<PixelType> result(img.get_width(), img.get_height());
        
        for (size_t y = 0; y < img.get_height(); ++y) {
            for (size_t x = 0; x < img.get_width(); ++x) {
                double scaled = static_cast<double>(img.at(x, y)) * factor;
                result.at(x, y) = static_cast<PixelType>(scaled);
            }
        }
        
        return result;
    }
    
    // Threshold operation
    static Image<PixelType> threshold(const Image<PixelType>& img, PixelType threshold_value) {
        Image<PixelType> result(img.get_width(), img.get_height());
        
        for (size_t y = 0; y < img.get_height(); ++y) {
            for (size_t x = 0; x < img.get_width(); ++x) {
                result.at(x, y) = (img.at(x, y) >= threshold_value) ? 
                    threshold_value : PixelType(0);
            }
        }
        
        return result;
    }
};

// ===================================================================
// 5. TYPE CONVERSION BETWEEN PIXEL TYPES
// ===================================================================

template<typename DestType, typename SrcType>
Image<DestType> convert_image(const Image<SrcType>& src) {
    Image<DestType> dest(src.get_width(), src.get_height());
    
    // Find source range for proper scaling
    auto [min_val, max_val] = ImageProcessor<SrcType>::find_min_max(src);
    double src_range = static_cast<double>(max_val) - static_cast<double>(min_val);
    
    // Determine destination range
    double dest_min, dest_max;
    if constexpr (std::is_floating_point_v<DestType>) {
        dest_min = 0.0;
        dest_max = 1.0;
    } else if constexpr (std::is_same_v<DestType, uint8_t>) {
        dest_min = 0.0;
        dest_max = 255.0;
    } else if constexpr (std::is_same_v<DestType, uint16_t>) {
        dest_min = 0.0;
        dest_max = 65535.0;
    } else {
        dest_min = 0.0;
        dest_max = 1.0;
    }
    
    double dest_range = dest_max - dest_min;
    
    // Convert with proper scaling
    for (size_t y = 0; y < src.get_height(); ++y) {
        for (size_t x = 0; x < src.get_width(); ++x) {
            double normalized = (static_cast<double>(src.at(x, y)) - static_cast<double>(min_val)) / src_range;
            double scaled = normalized * dest_range + dest_min;
            dest.at(x, y) = static_cast<DestType>(scaled);
        }
    }
    
    return dest;
}

// ===================================================================
// 6. GENERIC CAMERA HANDLER (WORKS WITH ANY PIXEL TYPE)
// ===================================================================

template<typename PixelType>
class CameraHandler {
private:
    std::unique_ptr<Camera<PixelType>> camera;
    
public:
    CameraHandler(std::unique_ptr<Camera<PixelType>> cam)
        : camera(std::move(cam)) {}
    
    void display_camera_info() {
        std::cout << "\n  Camera: " << camera->get_name() << std::endl;
        std::cout << "  Resolution: " << camera->get_width() << "x" << camera->get_height() << std::endl;
        std::cout << "  Pixel type: " << typeid(PixelType).name() << std::endl;
        std::cout << "  Bits per pixel: " << Camera<PixelType>::bits_per_pixel() << std::endl;
        std::cout << "  Floating point: " << (Camera<PixelType>::is_floating_point() ? "Yes" : "No") << std::endl;
        
        // Calculate memory per frame
        size_t bytes = camera->get_width() * camera->get_height() * sizeof(PixelType);
        std::cout << "  Memory per frame: " << bytes << " bytes";
        if (bytes >= 1024 * 1024) {
            std::cout << " (" << bytes / (1024.0 * 1024.0) << " MB)";
        } else if (bytes >= 1024) {
            std::cout << " (" << bytes / 1024.0 << " KB)";
        }
        std::cout << std::endl;
    }
    
    void capture_and_process() {
        std::cout << "\n  Capturing image..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        
        Image<PixelType> img = camera->capture();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "  Capture time: " << duration.count() << " µs" << std::endl;
        
        // Process image
        std::cout << "\n  Processing image..." << std::endl;
        double mean = ImageProcessor<PixelType>::calculate_mean(img);
        auto [min_val, max_val] = ImageProcessor<PixelType>::find_min_max(img);
        
        std::cout << "  Mean value: " << mean << std::endl;
        std::cout << "  Min value: " << static_cast<double>(min_val) << std::endl;
        std::cout << "  Max value: " << static_cast<double>(max_val) << std::endl;
        
        // Sample some pixel values
        std::cout << "\n  Sample pixels:" << std::endl;
        std::cout << "    Top-left (0,0): " << static_cast<double>(img.at(0, 0)) << std::endl;
        std::cout << "    Center (" << img.get_width()/2 << "," << img.get_height()/2 << "): " 
                  << static_cast<double>(img.at(img.get_width()/2, img.get_height()/2)) << std::endl;
        std::cout << "    Bottom-right (" << img.get_width()-1 << "," << img.get_height()-1 << "): " 
                  << static_cast<double>(img.at(img.get_width()-1, img.get_height()-1)) << std::endl;
    }
};

// ===================================================================
// 7. DEMONSTRATION FUNCTIONS
// ===================================================================

void demonstrate_8bit_camera() {
    std::cout << "\n=== 1. 8-BIT GRAYSCALE CAMERA (uint8_t) ===" << std::endl;
    std::cout << "Common in: Webcams, industrial cameras, surveillance" << std::endl;
    
    CameraHandler<uint8_t> handler(
        std::make_unique<Camera8bit>("Webcam HD", 640, 480)
    );
    
    handler.display_camera_info();
    handler.capture_and_process();
}

void demonstrate_16bit_camera() {
    std::cout << "\n=== 2. 16-BIT CAMERA (uint16_t) ===" << std::endl;
    std::cout << "Common in: Scientific imaging, medical X-ray, astronomy" << std::endl;
    
    CameraHandler<uint16_t> handler(
        std::make_unique<Camera16bit>("Scientific CCD", 1024, 1024)
    );
    
    handler.display_camera_info();
    handler.capture_and_process();
}

void demonstrate_float_camera() {
    std::cout << "\n=== 3. FLOAT CAMERA (32-bit float) ===" << std::endl;
    std::cout << "Common in: HDR imaging, computational photography, depth sensors" << std::endl;
    
    CameraHandler<float> handler(
        std::make_unique<CameraFloat>("ToF Depth Camera", 320, 240)
    );
    
    handler.display_camera_info();
    handler.capture_and_process();
}

void demonstrate_double_camera() {
    std::cout << "\n=== 4. DOUBLE PRECISION CAMERA (64-bit double) ===" << std::endl;
    std::cout << "Common in: Research, astronomical imaging, spectroscopy" << std::endl;
    
    CameraHandler<double> handler(
        std::make_unique<CameraDouble>("Telescope CCD", 512, 512)
    );
    
    handler.display_camera_info();
    handler.capture_and_process();
}

void demonstrate_type_conversion() {
    std::cout << "\n=== 5. IMAGE TYPE CONVERSION ===" << std::endl;
    
    // Capture 8-bit image
    Camera8bit cam8("Source Camera", 320, 240);
    Image<uint8_t> img8 = cam8.capture();
    
    std::cout << "\nOriginal 8-bit image:" << std::endl;
    std::cout << "  Size: " << img8.memory_bytes() << " bytes" << std::endl;
    auto [min8, max8] = ImageProcessor<uint8_t>::find_min_max(img8);
    std::cout << "  Range: " << (int)min8 << " to " << (int)max8 << std::endl;
    
    // Convert to 16-bit
    std::cout << "\nConverting to 16-bit..." << std::endl;
    Image<uint16_t> img16 = convert_image<uint16_t>(img8);
    std::cout << "  Size: " << img16.memory_bytes() << " bytes" << std::endl;
    auto [min16, max16] = ImageProcessor<uint16_t>::find_min_max(img16);
    std::cout << "  Range: " << min16 << " to " << max16 << std::endl;
    
    // Convert to float
    std::cout << "\nConverting to float..." << std::endl;
    Image<float> imgf = convert_image<float>(img8);
    std::cout << "  Size: " << imgf.memory_bytes() << " bytes" << std::endl;
    auto [minf, maxf] = ImageProcessor<float>::find_min_max(imgf);
    std::cout << "  Range: " << minf << " to " << maxf << std::endl;
    
    // Convert to double
    std::cout << "\nConverting to double..." << std::endl;
    Image<double> imgd = convert_image<double>(img8);
    std::cout << "  Size: " << imgd.memory_bytes() << " bytes" << std::endl;
    auto [mind, maxd] = ImageProcessor<double>::find_min_max(imgd);
    std::cout << "  Range: " << mind << " to " << maxd << std::endl;
}

void demonstrate_processing_algorithms() {
    std::cout << "\n=== 6. IMAGE PROCESSING ALGORITHMS ===" << std::endl;
    
    Camera8bit cam("Processing Camera", 100, 100);
    Image<uint8_t> img = cam.capture();
    
    std::cout << "\nOriginal image statistics:" << std::endl;
    double mean = ImageProcessor<uint8_t>::calculate_mean(img);
    auto [min_val, max_val] = ImageProcessor<uint8_t>::find_min_max(img);
    std::cout << "  Mean: " << mean << std::endl;
    std::cout << "  Min: " << (int)min_val << ", Max: " << (int)max_val << std::endl;
    
    // Scale by 2x
    std::cout << "\nScaling by 2x:" << std::endl;
    Image<uint8_t> scaled = ImageProcessor<uint8_t>::scale(img, 2.0);
    double mean_scaled = ImageProcessor<uint8_t>::calculate_mean(scaled);
    std::cout << "  Mean after scaling: " << mean_scaled << std::endl;
    
    // Threshold
    std::cout << "\nThresholding at 128:" << std::endl;
    Image<uint8_t> thresholded = ImageProcessor<uint8_t>::threshold(img, 128);
    size_t above_threshold = 0;
    for (size_t i = 0; i < thresholded.get_size(); ++i) {
        if (thresholded.data()[i] > 0) ++above_threshold;
    }
    std::cout << "  Pixels above threshold: " << above_threshold 
              << " (" << (100.0 * above_threshold / thresholded.get_size()) << "%)" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  TEMPLATED CAMERA INTERFACE FOR MULTIPLE PIXEL TYPES" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    demonstrate_8bit_camera();
    demonstrate_16bit_camera();
    demonstrate_float_camera();
    demonstrate_double_camera();
    demonstrate_type_conversion();
    demonstrate_processing_algorithms();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  TEMPLATE BENEFITS FOR CAMERA INTERFACING" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n✅ ADVANTAGES OF TEMPLATES:" << std::endl;
    std::cout << "\n1. TYPE SAFETY" << std::endl;
    std::cout << "   • Compile-time type checking" << std::endl;
    std::cout << "   • No runtime casting needed" << std::endl;
    std::cout << "   • Prevents mixing incompatible pixel types" << std::endl;
    
    std::cout << "\n2. ZERO OVERHEAD" << std::endl;
    std::cout << "   • No virtual function calls" << std::endl;
    std::cout << "   • All types resolved at compile time" << std::endl;
    std::cout << "   • Fully optimizable by compiler" << std::endl;
    
    std::cout << "\n3. CODE REUSABILITY" << std::endl;
    std::cout << "   • Write processing algorithms once" << std::endl;
    std::cout << "   • Works with uint8_t, uint16_t, float, double" << std::endl;
    std::cout << "   • Easy to add new pixel types" << std::endl;
    
    std::cout << "\n4. COMPILE-TIME INFORMATION" << std::endl;
    std::cout << "   • sizeof(PixelType) known at compile time" << std::endl;
    std::cout << "   • Type traits (is_floating_point, etc.)" << std::endl;
    std::cout << "   • if constexpr for type-specific code" << std::endl;
    
    std::cout << "\n📊 MEMORY COMPARISON (640x480 image):" << std::endl;
    size_t pixels = 640 * 480;
    std::cout << "  uint8_t:  " << pixels * sizeof(uint8_t) / 1024 << " KB" << std::endl;
    std::cout << "  uint16_t: " << pixels * sizeof(uint16_t) / 1024 << " KB" << std::endl;
    std::cout << "  float:    " << pixels * sizeof(float) / 1024 << " KB" << std::endl;
    std::cout << "  double:   " << pixels * sizeof(double) / 1024 << " KB" << std::endl;
    
    std::cout << "\n💡 REAL-WORLD APPLICATIONS:" << std::endl;
    std::cout << "\n  uint8_t (8-bit):" << std::endl;
    std::cout << "    • Consumer webcams" << std::endl;
    std::cout << "    • Surveillance cameras" << std::endl;
    std::cout << "    • Industrial inspection" << std::endl;
    std::cout << "    • Video streaming" << std::endl;
    
    std::cout << "\n  uint16_t (16-bit):" << std::endl;
    std::cout << "    • Medical X-ray imaging" << std::endl;
    std::cout << "    • Scientific CCD cameras" << std::endl;
    std::cout << "    • Astronomy (star imaging)" << std::endl;
    std::cout << "    • High dynamic range photography" << std::endl;
    
    std::cout << "\n  float (32-bit):" << std::endl;
    std::cout << "    • Time-of-Flight (ToF) depth sensors" << std::endl;
    std::cout << "    • HDR image processing" << std::endl;
    std::cout << "    • Computational photography" << std::endl;
    std::cout << "    • Normalized image data" << std::endl;
    
    std::cout << "\n  double (64-bit):" << std::endl;
    std::cout << "    • Radio telescope data" << std::endl;
    std::cout << "    • Spectroscopy" << std::endl;
    std::cout << "    • High-precision research" << std::endl;
    std::cout << "    • Scientific analysis" << std::endl;
    
    std::cout << "\n🔧 TEMPLATE TECHNIQUES DEMONSTRATED:" << std::endl;
    std::cout << "  • Class templates: Image<T>, Camera<T>" << std::endl;
    std::cout << "  • Function templates: convert_image<DestType, SrcType>" << std::endl;
    std::cout << "  • Type traits: std::is_floating_point_v<T>" << std::endl;
    std::cout << "  • if constexpr: Compile-time conditional code" << std::endl;
    std::cout << "  • Template specialization: Type-specific behavior" << std::endl;
    std::cout << "  • constexpr functions: Compile-time computation" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
