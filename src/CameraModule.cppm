// ===================================================================
// C++20 MODULE: CAMERA INTERFACE WITH TEMPLATES
// ===================================================================
// Demonstrates C++20 modules with export/import
// Also includes concepts for type constraints
// ===================================================================

module;

// Global module fragment - for #include directives
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <utility>
#include <concepts>

export module camera;

// ===================================================================
// C++20 CONCEPTS FOR PIXEL TYPES
// ===================================================================

export template<typename T>
concept PixelType = std::is_arithmetic_v<T> && (
    std::is_same_v<T, uint8_t> ||
    std::is_same_v<T, uint16_t> ||
    std::is_same_v<T, uint32_t> ||
    std::is_same_v<T, float> ||
    std::is_same_v<T, double>
);

export template<typename T>
concept IntegerPixel = PixelType<T> && std::is_integral_v<T>;

export template<typename T>
concept FloatingPixel = PixelType<T> && std::is_floating_point_v<T>;

// ===================================================================
// IMAGE CLASS (EXPORTED)
// ===================================================================

export template<PixelType T>
class Image {
private:
    size_t width;
    size_t height;
    std::vector<T> pixels;
    
public:
    Image(size_t w, size_t h) 
        : width(w), height(h), pixels(w * h) {}
    
    Image(size_t w, size_t h, T initial_value) 
        : width(w), height(h), pixels(w * h, initial_value) {}
    
    // Accessors
    [[nodiscard]] size_t get_width() const noexcept { return width; }
    [[nodiscard]] size_t get_height() const noexcept { return height; }
    [[nodiscard]] size_t get_size() const noexcept { return pixels.size(); }
    
    // Pixel access
    T& at(size_t x, size_t y) {
        return pixels[y * width + x];
    }
    
    const T& at(size_t x, size_t y) const {
        return pixels[y * width + x];
    }
    
    // Raw data access
    [[nodiscard]] T* data() noexcept { return pixels.data(); }
    [[nodiscard]] const T* data() const noexcept { return pixels.data(); }
    
    // Memory size
    [[nodiscard]] size_t memory_bytes() const noexcept {
        return pixels.size() * sizeof(T);
    }
    
    // Fill with value
    void fill(T value) {
        std::fill(pixels.begin(), pixels.end(), value);
    }
    
    // C++20: Three-way comparison
    auto operator<=>(const Image&) const = default;
};

// ===================================================================
// CAMERA INTERFACE (EXPORTED)
// ===================================================================

export template<PixelType T>
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
    virtual Image<T> capture() = 0;
    
    // Configuration
    [[nodiscard]] size_t get_width() const noexcept { return width; }
    [[nodiscard]] size_t get_height() const noexcept { return height; }
    [[nodiscard]] const std::string& get_name() const noexcept { return camera_name; }
    
    // Get pixel type information
    [[nodiscard]] static constexpr size_t bits_per_pixel() noexcept {
        return sizeof(T) * 8;
    }
    
    [[nodiscard]] static constexpr bool is_floating_point() noexcept {
        return std::is_floating_point_v<T>;
    }
    
    [[nodiscard]] static constexpr bool is_integer() noexcept {
        return std::is_integral_v<T>;
    }
};

// ===================================================================
// IMAGE PROCESSOR (EXPORTED)
// ===================================================================

export template<PixelType T>
class ImageProcessor {
public:
    // Calculate average pixel value
    [[nodiscard]] static double calculate_mean(const Image<T>& img) {
        double sum = 0.0;
        const T* data = img.data();
        size_t size = img.get_size();
        
        for (size_t i = 0; i < size; ++i) {
            sum += static_cast<double>(data[i]);
        }
        
        return sum / size;
    }
    
    // Find min and max pixel values
    [[nodiscard]] static std::pair<T, T> find_min_max(const Image<T>& img) {
        const T* data = img.data();
        size_t size = img.get_size();
        
        T min_val = data[0];
        T max_val = data[0];
        
        for (size_t i = 1; i < size; ++i) {
            if (data[i] < min_val) min_val = data[i];
            if (data[i] > max_val) max_val = data[i];
        }
        
        return {min_val, max_val};
    }
    
    // Scale pixel values
    [[nodiscard]] static Image<T> scale(const Image<T>& img, double factor) {
        Image<T> result(img.get_width(), img.get_height());
        
        for (size_t y = 0; y < img.get_height(); ++y) {
            for (size_t x = 0; x < img.get_width(); ++x) {
                double scaled = static_cast<double>(img.at(x, y)) * factor;
                result.at(x, y) = static_cast<T>(scaled);
            }
        }
        
        return result;
    }
    
    // Threshold operation - using concept constraint
    [[nodiscard]] static Image<T> threshold(const Image<T>& img, T threshold_value) 
        requires IntegerPixel<T>
    {
        Image<T> result(img.get_width(), img.get_height());
        
        for (size_t y = 0; y < img.get_height(); ++y) {
            for (size_t x = 0; x < img.get_width(); ++x) {
                result.at(x, y) = (img.at(x, y) >= threshold_value) ? 
                    threshold_value : T(0);
            }
        }
        
        return result;
    }
    
    // Normalize operation - only for floating point images
    [[nodiscard]] static Image<T> normalize(const Image<T>& img)
        requires FloatingPixel<T>
    {
        auto [min_val, max_val] = find_min_max(img);
        T range = max_val - min_val;
        
        if (range == T(0)) return img;
        
        Image<T> result(img.get_width(), img.get_height());
        
        for (size_t y = 0; y < img.get_height(); ++y) {
            for (size_t x = 0; x < img.get_width(); ++x) {
                result.at(x, y) = (img.at(x, y) - min_val) / range;
            }
        }
        
        return result;
    }
};

// ===================================================================
// TYPE CONVERSION (EXPORTED)
// ===================================================================

export template<PixelType DestType, PixelType SrcType>
[[nodiscard]] Image<DestType> convert_image(const Image<SrcType>& src) {
    Image<DestType> dest(src.get_width(), src.get_height());
    
    // Find source range
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
    if (src_range == 0.0) {
        dest.fill(static_cast<DestType>(dest_min));
        return dest;
    }
    
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
// CAMERA IMPLEMENTATIONS (EXPORTED)
// ===================================================================

export class Camera8bit : public Camera<uint8_t> {
public:
    Camera8bit(const std::string& name, size_t w, size_t h)
        : Camera<uint8_t>(name, w, h) {}
    
    Image<uint8_t> capture() override {
        Image<uint8_t> img(get_width(), get_height());
        
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

export class Camera16bit : public Camera<uint16_t> {
public:
    Camera16bit(const std::string& name, size_t w, size_t h)
        : Camera<uint16_t>(name, w, h) {}
    
    Image<uint16_t> capture() override {
        Image<uint16_t> img(get_width(), get_height());
        
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

export class CameraFloat : public Camera<float> {
public:
    CameraFloat(const std::string& name, size_t w, size_t h)
        : Camera<float>(name, w, h) {}
    
    Image<float> capture() override {
        Image<float> img(get_width(), get_height());
        
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

export class CameraDouble : public Camera<double> {
public:
    CameraDouble(const std::string& name, size_t w, size_t h)
        : Camera<double>(name, w, h) {}
    
    Image<double> capture() override {
        Image<double> img(get_width(), get_height());
        
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
// CAMERA HANDLER (EXPORTED)
// ===================================================================

export template<PixelType T>
class CameraHandler {
private:
    std::unique_ptr<Camera<T>> camera;
    
public:
    CameraHandler(std::unique_ptr<Camera<T>> cam)
        : camera(std::move(cam)) {}
    
    [[nodiscard]] const Camera<T>* get_camera() const noexcept {
        return camera.get();
    }
    
    [[nodiscard]] Image<T> capture() {
        return camera->capture();
    }
    
    [[nodiscard]] static constexpr size_t pixel_size() noexcept {
        return sizeof(T);
    }
    
    [[nodiscard]] static constexpr size_t bits_per_pixel() noexcept {
        return sizeof(T) * 8;
    }
};
