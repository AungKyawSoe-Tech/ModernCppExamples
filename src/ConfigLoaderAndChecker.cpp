// ConfigLoaderAndChecker.cpp
// Demonstrates dynamic configuration loading and monitoring using std::unordered_multimap
//
// FEATURES:
// 1. Load key-value pairs from JSON config file into unordered_multimap
// 2. Monitor config.json for changes (file modification time)
// 3. Automatically reload and update configuration on changes
// 4. Handle new keys, modified values, and deleted keys
// 5. Thread-safe configuration access
// 6. JSON validation and error handling
// 7. Real-time configuration updates without restart

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using namespace std::chrono;
using namespace std::chrono_literals;

// ============================================================================
// SECTION 1: Using nlohmann/json library for JSON parsing
// ============================================================================

using json = nlohmann::json;

// ============================================================================
// SECTION 2: Thread-Safe Configuration Manager with unordered_multimap
// ============================================================================

class ConfigManager {
private:
    std::unordered_multimap<std::string, std::string> config_data_;
    std::unordered_multimap<std::string, std::string> previous_config_data_;
    mutable std::mutex mutex_;
    fs::file_time_type last_write_time_;
    std::string config_file_path_;
    std::atomic<int> version_{0};
    
public:
    explicit ConfigManager(const std::string& config_file)
        : config_file_path_(config_file) {
        
        if (fs::exists(config_file_path_)) {
            last_write_time_ = fs::last_write_time(config_file_path_);
        }
    }
    
    // Load configuration from file
    bool load() {
        try {
            std::ifstream file(config_file_path_);
            if (!file.is_open()) {
                std::cerr << "❌ Cannot open config file: " << config_file_path_ << "\n";
                return false;
            }
            
            // Parse JSON using nlohmann/json
            json parsed_json;
            file >> parsed_json;
            file.close();
            
            // Update configuration
            std::lock_guard<std::mutex> lock(mutex_);
            
            // Save previous config for change detection
            previous_config_data_ = config_data_;
            config_data_.clear();
            
            // Support both flat objects and nested structures
            for (auto& [key, value] : parsed_json.items()) {
                // Convert value to string representation
                std::string value_str;
                if (value.is_string()) {
                    value_str = value.get<std::string>();
                } else if (value.is_number()) {
                    value_str = value.dump();
                } else if (value.is_boolean()) {
                    value_str = value.get<bool>() ? "true" : "false";
                } else if (value.is_array()) {
                    // For arrays, insert multiple entries (true multimap usage)
                    for (const auto& item : value) {
                        if (item.is_string()) {
                            config_data_.insert({key, item.get<std::string>()});
                        } else {
                            config_data_.insert({key, item.dump()});
                        }
                    }
                    continue; // Skip the insert below
                } else {
                    value_str = value.dump();
                }
                
                config_data_.insert({key, value_str});
            }
            
            version_++;
            
            std::cout << "✓ Loaded " << config_data_.size() 
                      << " configuration entries (version " << version_ << ")\n";
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "❌ Failed to load config: " << e.what() << "\n";
            return false;
        }
    }
    
    // Reload if file has changed
    bool reload_if_changed() {
        if (!fs::exists(config_file_path_)) {
            std::cerr << "⚠️  Config file not found: " << config_file_path_ << "\n";
            return false;
        }
        
        auto current_write_time = fs::last_write_time(config_file_path_);
        
        if (current_write_time != last_write_time_) {
            std::cout << "\n🔄 Config file modified, reloading...\n";
            last_write_time_ = current_write_time;
            bool loaded = load();
            if (loaded) {
                print_changes();
            }
            return loaded;
        }
        
        return false;
    }
    
    // Get value by key (returns first match for multimap)
    std::string get(const std::string& key, const std::string& default_value = "") const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = config_data_.find(key);
        if (it != config_data_.end()) {
            return it->second;
        }
        return default_value;
    }
    
    // Get all values for a key (multimap can have multiple values per key)
    std::vector<std::string> get_all(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> values;
        
        auto range = config_data_.equal_range(key);
        for (auto it = range.first; it != range.second; ++it) {
            values.push_back(it->second);
        }
        
        return values;
    }
    
    // Check if key exists
    bool has_key(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_data_.find(key) != config_data_.end();
    }
    
    // Get all keys
    std::vector<std::string> get_all_keys() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> keys;
        
        for (const auto& [key, value] : config_data_) {
            keys.push_back(key);
        }
        
        return keys;
    }
    
    // Get configuration count
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_data_.size();
    }
    
    // Display all configuration
    void display() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
        std::cout << "║  Current Configuration (Version " << version_ << ")";
        std::cout << std::string(20 - std::to_string(version_.load()).length(), ' ') << "║\n";
        std::cout << "╠════════════════════════════════════════════════════════╣\n";
        
        if (config_data_.empty()) {
            std::cout << "║  (empty)                                               ║\n";
        } else {
            for (const auto& [key, value] : config_data_) {
                std::string line = "║  " + key + ": " + value;
                line += std::string(58 - line.length(), ' ') + "║";
                std::cout << line << "\n";
            }
        }
        
        std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    }
    
    int get_version() const {
        return version_.load();
    }
    
    std::string get_file_path() const {
        return config_file_path_;
    }
    
    void print_changes() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::cout << "\n📝 Configuration changes detected:\n";
        
        // Track unique keys
        std::set<std::string> all_keys;
        for (const auto& [key, _] : previous_config_data_) all_keys.insert(key);
        for (const auto& [key, _] : config_data_) all_keys.insert(key);
        
        for (const auto& key : all_keys) {
            auto prev_range = previous_config_data_.equal_range(key);
            auto curr_range = config_data_.equal_range(key);
            
            std::vector<std::string> prev_values;
            for (auto it = prev_range.first; it != prev_range.second; ++it) {
                prev_values.push_back(it->second);
            }
            
            std::vector<std::string> curr_values;
            for (auto it = curr_range.first; it != curr_range.second; ++it) {
                curr_values.push_back(it->second);
            }
            
            if (prev_values.empty() && !curr_values.empty()) {
                std::cout << "  ➕ ADDED to unordered_multimap: '" << key << "' = ";
                for (size_t i = 0; i < curr_values.size(); ++i) {
                    std::cout << "\"" << curr_values[i] << "\"";
                    if (i < curr_values.size() - 1) std::cout << ", ";
                }
                if (curr_values.size() > 1) {
                    std::cout << " (" << curr_values.size() << " entries)";
                }
                std::cout << "\n";
            } else if (!prev_values.empty() && curr_values.empty()) {
                std::cout << "  ➖ REMOVED from unordered_multimap: '" << key << "' (was: ";
                for (size_t i = 0; i < prev_values.size(); ++i) {
                    std::cout << "\"" << prev_values[i] << "\"";
                    if (i < prev_values.size() - 1) std::cout << ", ";
                }
                std::cout << ")\n";
            } else if (prev_values != curr_values) {
                std::cout << "  🔄 MODIFIED in unordered_multimap: '" << key << "'\n";
                std::cout << "     Old: ";
                for (size_t i = 0; i < prev_values.size(); ++i) {
                    std::cout << "\"" << prev_values[i] << "\"";
                    if (i < prev_values.size() - 1) std::cout << ", ";
                }
                std::cout << "\n     New: ";
                for (size_t i = 0; i < curr_values.size(); ++i) {
                    std::cout << "\"" << curr_values[i] << "\"";
                    if (i < curr_values.size() - 1) std::cout << ", ";
                }
                std::cout << "\n";
            }
        }
        std::cout << "\n";
    }
};

// ============================================================================
// SECTION 3: Configuration Monitor (Background Thread)
// ============================================================================

class ConfigMonitor {
private:
    ConfigManager& config_manager_;
    std::atomic<bool> running_{false};
    std::thread monitor_thread_;
    std::chrono::milliseconds check_interval_;
    
public:
    ConfigMonitor(ConfigManager& manager, std::chrono::milliseconds interval = 600000ms)
        : config_manager_(manager), check_interval_(interval) {}
    
    ~ConfigMonitor() {
        stop();
    }
    
    void start() {
        if (running_) {
            std::cout << "⚠️  Monitor already running\n";
            return;
        }
        
        running_ = true;
        monitor_thread_ = std::thread(&ConfigMonitor::monitor_loop, this);
        
        std::cout << "✓ Configuration monitor started (checking every " 
                  << check_interval_.count() << "ms)\n";
    }
    
    void stop() {
        if (!running_) return;
        
        running_ = false;
        if (monitor_thread_.joinable()) {
            monitor_thread_.join();
        }
        
        std::cout << "✓ Configuration monitor stopped\n";
    }
    
private:
    void monitor_loop() {
        while (running_) {
            config_manager_.reload_if_changed();
            std::this_thread::sleep_for(check_interval_);
        }
    }
};

// ============================================================================
// SECTION 4: Demonstration Application
// ============================================================================

void demonstrate_basic_loading() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Demo 1: Basic Configuration Loading ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    ConfigManager config("../config.json");
    
    std::cout << "Loading initial configuration...\n";
    if (config.load()) {
        config.display();
        
        // Access individual values
        std::cout << "Accessing specific values:\n";
        std::cout << "  server_host: " << config.get("server_host", "not found") << "\n";
        std::cout << "  server_port: " << config.get("server_port", "not found") << "\n";
        std::cout << "  debug_mode: " << config.get("debug_mode", "not found") << "\n";
        std::cout << "  max_connections: " << config.get("max_connections", "not found") << "\n";
        
        // Check if keys exist
        std::cout << "\nKey existence checks:\n";
        std::cout << "  has 'server_host': " << (config.has_key("server_host") ? "Yes" : "No") << "\n";
        std::cout << "  has 'nonexistent': " << (config.has_key("nonexistent") ? "Yes" : "No") << "\n";
        
        // Get all keys
        std::cout << "\nAll keys: ";
        auto keys = config.get_all_keys();
        for (size_t i = 0; i < keys.size(); ++i) {
            std::cout << keys[i];
            if (i < keys.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
    }
}

void demonstrate_live_monitoring() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Demo 2: Live Configuration Monitoring ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    ConfigManager config("../config.json");
    std::cout << "📂 Reading config from: " << fs::absolute("../config.json") << "\n\n";
    config.load();
    config.display();
    
    // Start monitoring in background - use 10 seconds for demo (production: 10 minutes)
    ConfigMonitor monitor(config, 10000ms);  // Check every 10 seconds for demo
    monitor.start();
    
    // Read demoTime from config (default to 120 seconds)
    int demo_time = 120;
    std::string demo_time_str = config.get("demoTime");
    if (!demo_time_str.empty()) {
        try {
            demo_time = std::stoi(demo_time_str);
            std::cout << "📌 Using demoTime from config: " << demo_time << " seconds\n";
        } catch (...) {
            std::cout << "⚠️  Invalid demoTime in config, using default: 120 seconds\n";
        }
    }
    
    std::cout << "\n📋 Instructions:\n";
    std::cout << "  1. Edit config.json file while this program is running\n";
    std::cout << "  2. Add new key-value pairs: \"new_key\": \"new_value\"\n";
    std::cout << "  3. Modify existing values: \"server_port\": \"9090\"\n";
    std::cout << "  4. Add arrays: \"allowed_ips\": [\"192.168.1.1\", \"10.0.0.1\"]\n";
    std::cout << "  5. Change \"demoTime\": 60 to adjust monitoring duration dynamically\n";
    std::cout << "  6. Save the file and watch for automatic reload!\n";
    std::cout << "\n⏱️  App will run for " << demo_time << " seconds (checks config every 10 seconds)...\n";
    std::cout << "  (In production: use 10 minutes / 600000ms)\n\n";
    
    // Simulate application running and accessing config
    int elapsed = 0;
    while (elapsed < demo_time) {
        std::this_thread::sleep_for(1s);
        elapsed++;
        
        // Periodically show current config version
        if (elapsed % 10 == 0) {
            std::cout << "⏰ " << elapsed << "s - Config version: " << config.get_version() 
                      << " | Entries: " << config.size();
            
            // Check if demoTime has changed
            std::string current_demo_time_str = config.get("demoTime");
            if (!current_demo_time_str.empty()) {
                try {
                    int new_demo_time = std::stoi(current_demo_time_str);
                    if (new_demo_time != demo_time) {
                        std::cout << " | demoTime changed: " << demo_time << "→" << new_demo_time << "s";
                        demo_time = new_demo_time;
                        std::cout << "\n⏰ Runtime adjusted! Will now run until " << demo_time << " seconds\n";
                    } else {
                        std::cout << "\n";
                    }
                } catch (...) {
                    std::cout << "\n";
                }
            } else {
                std::cout << "\n";
            }
        }
    }
    
    monitor.stop();
    
    std::cout << "\nFinal configuration state:\n";
    config.display();
}

void demonstrate_error_handling() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Demo 3: Error Handling ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    // Test with non-existent file
    std::cout << "1. Loading non-existent file:\n";
    ConfigManager config1("nonexistent.json");
    config1.load();
    
    // Test with invalid JSON
    std::cout << "\n2. Creating and loading invalid JSON:\n";
    std::ofstream bad_file("bad_config.json");
    bad_file << "{ invalid json without quotes }";
    bad_file.close();
    
    ConfigManager config2("bad_config.json");
    config2.load();
    
    // Cleanup
    fs::remove("bad_config.json");
    
    std::cout << "\n✓ Error handling demonstrated\n";
    std::cout << "  • File not found: gracefully handled\n";
    std::cout << "  • Invalid JSON: exception caught and reported\n";
}

void demonstrate_multimap_features() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Demo 4: unordered_multimap Features ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Concept: unordered_multimap allows multiple values per key\n";
    std::cout << "Use case: Configuration with arrays or multiple values\n\n";
    
    std::cout << "Example JSON (if extended):\n";
    std::cout << R"({
  "server": "localhost",
  "allowed_ip": "192.168.1.1",
  "allowed_ip": "192.168.1.2",
  "allowed_ip": "10.0.0.1"
})" << "\n\n";
    
    std::cout << "With unordered_multimap, you can store multiple 'allowed_ip' values\n";
    std::cout << "and retrieve them all using get_all('allowed_ip')\n\n";
    
    std::cout << "Current implementation: Simple key-value pairs\n";
    std::cout << "If duplicate keys in JSON: last one wins (map behavior)\n";
    std::cout << "To enable true multimap: parse JSON arrays into separate entries\n\n";
    
    ConfigManager config("../config.json");
    config.load();
    
    std::cout << "Accessing all values for a key:\n";
    auto values = config.get_all("server_host");
    std::cout << "  'server_host' has " << values.size() << " value(s):\n";
    for (const auto& val : values) {
        std::cout << "    - " << val << "\n";
    }
}

void create_sample_config_if_not_exists() {
    if (!fs::exists("config.json")) {
        std::cout << "📝 Creating sample config.json...\n";
        std::ofstream config_file("config.json");
        config_file << R"({
  "server_host": "localhost",
  "server_port": "8080",
  "debug_mode": "true",
  "max_connections": "100",
  "timeout_seconds": "30",
  "database_url": "mongodb://localhost:27017"
})";
        config_file.close();
        std::cout << "✓ Sample config.json created\n\n";
    }
}

// ============================================================================
// SECTION 5: Best Practices Guide
// ============================================================================

void show_best_practices() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Configuration Management Best Practices ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✓ DESIGN PATTERNS:\n";
    std::cout << "  1. Singleton pattern for global config access\n";
    std::cout << "  2. Observer pattern for change notifications\n";
    std::cout << "  3. Strategy pattern for different config sources\n";
    std::cout << "  4. Builder pattern for complex configurations\n\n";
    
    std::cout << "✓ THREAD SAFETY:\n";
    std::cout << "  1. Use std::mutex for protecting config access\n";
    std::cout << "  2. std::shared_mutex for read-write locks (C++17)\n";
    std::cout << "  3. std::atomic for version counters\n";
    std::cout << "  4. Lock-free structures for high-performance\n\n";
    
    std::cout << "✓ FILE MONITORING:\n";
    std::cout << "  1. Check file modification time (fs::last_write_time)\n";
    std::cout << "  2. Use inotify (Linux) / ReadDirectoryChangesW (Windows)\n";
    std::cout << "  3. Background thread with periodic checks\n";
    std::cout << "  4. Debounce rapid changes (wait for settle)\n\n";
    
    std::cout << "✓ ERROR HANDLING:\n";
    std::cout << "  1. Validate JSON syntax before parsing\n";
    std::cout << "  2. Keep previous valid config on parse errors\n";
    std::cout << "  3. Log all config changes and errors\n";
    std::cout << "  4. Provide default values for missing keys\n\n";
    
    std::cout << "✓ PERFORMANCE:\n";
    std::cout << "  1. Cache frequently accessed values\n";
    std::cout << "  2. Use unordered_map for O(1) lookups\n";
    std::cout << "  3. Minimize file I/O (check timestamp first)\n";
    std::cout << "  4. Parse JSON incrementally for large files\n\n";
    
    std::cout << "✓ PRODUCTION CONSIDERATIONS:\n";
    std::cout << "  1. Support multiple config sources (file, env, CLI)\n";
    std::cout << "  2. Implement config versioning and rollback\n";
    std::cout << "  3. Add config validation (schemas, constraints)\n";
    std::cout << "  4. Enable hot-reload without service restart\n";
    std::cout << "  5. ✅ Using nlohmann/json library (header-only, modern C++)\n";
    std::cout << "  6. Implement config encryption for sensitive data\n";
    std::cout << "  7. Set appropriate monitoring interval (10 min for production)\n\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Configuration Loader and Live Monitor                         ║\n";
    std::cout << "║  Using std::unordered_multimap + File Watching                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    // Create sample config if it doesn't exist
    create_sample_config_if_not_exists();
    
    // Run demonstrations
    demonstrate_basic_loading();
    demonstrate_multimap_features();
    demonstrate_error_handling();
    
    // Interactive monitoring demo
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "Would you like to run the live monitoring demo? (y/n): ";
    char choice;
    std::cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        demonstrate_live_monitoring();
    } else {
        std::cout << "\nSkipping live monitoring demo.\n";
        std::cout << "To test manually:\n";
        std::cout << "  1. Run this program\n";
        std::cout << "  2. In another terminal: echo '{\"test\": \"value\"}' > config.json\n";
        std::cout << "  3. Watch the program detect changes!\n";
    }
    
    show_best_practices();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All demonstrations completed!\n";
    std::cout << "\nKEY FEATURES DEMONSTRATED:\n";
    std::cout << "  ✓ std::unordered_multimap for config storage\n";
    std::cout << "  ✓ JSON parsing and validation\n";
    std::cout << "  ✓ File modification detection\n";
    std::cout << "  ✓ Automatic config reloading\n";
    std::cout << "  ✓ Thread-safe access with std::mutex\n";
    std::cout << "  ✓ Background monitoring thread\n";
    std::cout << "  ✓ Error handling and recovery\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    return 0;
}
