// ===================================================================
// SYSTEM INTERACTION, FILESYSTEM, STRING_VIEW, AND REGEX
// ===================================================================
// This example demonstrates modern C++ for system programming:
//
// TOPICS COVERED:
// 1. Executing external commands (popen, system)
// 2. std::filesystem for file operations (C++17)
// 3. std::string_view for efficient parsing (C++17)
// 4. std::regex for pattern matching (C++11)
// 5. Parsing Linux tool output (ps, df, lsof, etc.)
// 6. Running bash and Python scripts from C++
// 7. File streams (ifstream, ofstream, fstream)
// 8. String streams (ostringstream, istringstream)
// 9. Stream manipulators and formatting
// 10. Binary I/O and stream state handling
// 11. Error handling with std::error_code
// 12. Thread synchronization (mutex, lock_guard, unique_lock)
// 13. File locking for concurrent access
// 14. Best practices for system integration
// 15. Security considerations
// 16. Cross-platform considerations
// ===================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <regex>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <array>
#include <memory>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace fs = std::filesystem;

// ===================================================================
// SECTION 1: BASIC COMMAND EXECUTION
// ===================================================================

// Execute command and capture output
std::string execute_command(std::string_view command) {
    std::array<char, 128> buffer;
    std::string result;
    
    // popen is safer than system() - doesn't invoke shell if not needed
    std::unique_ptr<FILE, decltype(&pclose)> pipe(
        popen(command.data(), "r"), pclose);
    
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    return result;
}

// Execute command with return code
struct CommandResult {
    std::string output;
    int return_code;
    bool success;
};

CommandResult execute_command_with_status(std::string_view command) {
    CommandResult result;
    
    std::array<char, 256> buffer;
    std::string output;
    
    FILE* pipe = popen(command.data(), "r");
    if (!pipe) {
        return {.output = "", .return_code = -1, .success = false};
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        output += buffer.data();
    }
    
    int status = pclose(pipe);
    result.output = output;
    result.return_code = WEXITSTATUS(status);
    result.success = (result.return_code == 0);
    
    return result;
}

void demonstrate_basic_execution() {
    std::cout << "\n=== 1. BASIC COMMAND EXECUTION ===" << std::endl;
    
    // Simple command
    std::cout << "\n1.1 Current directory:" << std::endl;
    std::string pwd = execute_command("pwd");
    std::cout << "   " << pwd;
    
    // Command with pipes
    std::cout << "1.2 Count files in current directory:" << std::endl;
    std::string count = execute_command("ls -1 | wc -l");
    std::cout << "   Files: " << count;
    
    // With status check
    std::cout << "1.3 Execute with status check:" << std::endl;
    auto result = execute_command_with_status("echo 'Hello from shell'");
    std::cout << "   Output: " << result.output;
    std::cout << "   Return code: " << result.return_code << std::endl;
    std::cout << "   Success: " << std::boolalpha << result.success << std::endl;
    
    std::cout << "\n💡 KEY POINTS:" << std::endl;
    std::cout << "   • Use popen() to capture output" << std::endl;
    std::cout << "   • Check return codes for errors" << std::endl;
    std::cout << "   • pclose() is critical - use RAII" << std::endl;
}

// ===================================================================
// SECTION 2: std::string_view FOR EFFICIENT PARSING
// ===================================================================

// Parse process info using string_view (no copies!)
struct ProcessInfo {
    std::string pid;
    std::string user;
    std::string cpu;
    std::string mem;
    std::string command;
};

std::vector<std::string_view> split_string_view(std::string_view str, char delimiter) {
    std::vector<std::string_view> tokens;
    size_t start = 0;
    
    while (start < str.size()) {
        // Skip leading delimiters
        while (start < str.size() && str[start] == delimiter) {
            ++start;
        }
        
        if (start >= str.size()) break;
        
        // Find end of token
        size_t end = str.find(delimiter, start);
        if (end == std::string_view::npos) {
            end = str.size();
        }
        
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    
    return tokens;
}

// Efficient parsing with string_view - no memory allocations for substrings!
std::vector<ProcessInfo> parse_ps_output(std::string_view output) {
    std::vector<ProcessInfo> processes;
    
    size_t line_start = 0;
    size_t line_end = output.find('\n');
    
    // Skip header line
    if (line_end != std::string_view::npos) {
        line_start = line_end + 1;
    }
    
    while (line_start < output.size()) {
        line_end = output.find('\n', line_start);
        if (line_end == std::string_view::npos) {
            line_end = output.size();
        }
        
        std::string_view line = output.substr(line_start, line_end - line_start);
        if (line.empty()) {
            line_start = line_end + 1;
            continue;
        }
        
        auto tokens = split_string_view(line, ' ');
        if (tokens.size() >= 5) {
            ProcessInfo info;
            info.pid = std::string(tokens[0]);
            info.user = std::string(tokens[1]);
            info.cpu = std::string(tokens[2]);
            info.mem = std::string(tokens[3]);
            
            // Command is rest of line - find position of 5th token in original output
            size_t token4_pos = line.find(tokens[4]);
            if (token4_pos != std::string_view::npos) {
                size_t cmd_offset = line_start + token4_pos;
                info.command = std::string(output.substr(cmd_offset, line_end - cmd_offset));
            }
            
            processes.push_back(info);
        }
        
        line_start = line_end + 1;
    }
    
    return processes;
}

void demonstrate_string_view_parsing() {
    std::cout << "\n=== 2. std::string_view PARSING ===" << std::endl;
    
    // Get process list
    std::string ps_output = execute_command("ps aux | head -20");
    
    std::cout << "\n2.1 Parsing ps output with string_view (zero-copy):" << std::endl;
    auto processes = parse_ps_output(ps_output);
    
    std::cout << "   Found " << processes.size() << " processes" << std::endl;
    std::cout << "\n   Top 5 by CPU:" << std::endl;
    
    // Sort by CPU
    std::sort(processes.begin(), processes.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  return std::stod(a.cpu) > std::stod(b.cpu);
              });
    
    for (size_t i = 0; i < std::min(size_t(5), processes.size()); i++) {
        std::cout << "   " << processes[i].pid << " "
                  << processes[i].cpu << "% CPU "
                  << processes[i].command.substr(0, 50) << std::endl;
    }
    
    std::cout << "\n💡 string_view benefits:" << std::endl;
    std::cout << "   • No memory allocations during parsing" << std::endl;
    std::cout << "   • Fast substring operations" << std::endl;
    std::cout << "   • Perfect for tokenizing large outputs" << std::endl;
}

// ===================================================================
// SECTION 3: std::regex FOR PATTERN MATCHING
// ===================================================================

// Parse network interface info with regex
struct NetworkInterface {
    std::string name;
    std::string ip_address;
    std::string netmask;
    std::string status;
};

std::vector<NetworkInterface> parse_ifconfig_with_regex(std::string_view output) {
    std::vector<NetworkInterface> interfaces;
    
    // Regex patterns
    std::regex iface_pattern(R"(^(\w+):\s+flags=.+)");
    std::regex inet_pattern(R"(inet\s+(\d+\.\d+\.\d+\.\d+)\s+netmask\s+(\S+))");
    std::regex status_pattern(R"(status:\s+(\w+))");
    
    std::string output_str(output);
    std::istringstream iss(output_str);
    std::string line;
    
    NetworkInterface current_iface;
    bool has_current = false;
    
    while (std::getline(iss, line)) {
        std::smatch match;
        
        // Check for interface name
        if (std::regex_search(line, match, iface_pattern)) {
            // Save previous interface
            if (has_current && !current_iface.ip_address.empty()) {
                interfaces.push_back(current_iface);
            }
            
            // Start new interface
            current_iface = NetworkInterface{};
            current_iface.name = match[1].str();
            has_current = true;
        }
        // Check for IP address
        else if (has_current && std::regex_search(line, match, inet_pattern)) {
            current_iface.ip_address = match[1].str();
            current_iface.netmask = match[2].str();
        }
        // Check for status
        else if (has_current && std::regex_search(line, match, status_pattern)) {
            current_iface.status = match[1].str();
        }
    }
    
    // Don't forget last interface
    if (has_current && !current_iface.ip_address.empty()) {
        interfaces.push_back(current_iface);
    }
    
    return interfaces;
}

// Parse log files with regex
struct LogEntry {
    std::string timestamp;
    std::string level;
    std::string message;
};

std::vector<LogEntry> parse_log_with_regex(std::string_view log_content) {
    std::vector<LogEntry> entries;
    
    // Match: [2024-01-15 10:30:45] ERROR: Something went wrong
    std::regex log_pattern(R"(\[(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})\]\s+(\w+):\s+(.+))");
    
    std::string log_str(log_content);
    std::istringstream iss(log_str);
    std::string line;
    
    while (std::getline(iss, line)) {
        std::smatch match;
        if (std::regex_search(line, match, log_pattern)) {
            entries.push_back({
                .timestamp = match[1].str(),
                .level = match[2].str(),
                .message = match[3].str()
            });
        }
    }
    
    return entries;
}

void demonstrate_regex_parsing() {
    std::cout << "\n=== 3. std::regex PATTERN MATCHING ===" << std::endl;
    
    // Network interface parsing
    std::cout << "\n3.1 Parsing network interfaces with regex:" << std::endl;
    std::string ifconfig_output = execute_command("ifconfig 2>/dev/null || ip addr 2>/dev/null || echo 'No network tools available'");
    
    auto interfaces = parse_ifconfig_with_regex(ifconfig_output);
    for (const auto& iface : interfaces) {
        std::cout << "   Interface: " << iface.name << std::endl;
        std::cout << "      IP: " << iface.ip_address << std::endl;
        std::cout << "      Netmask: " << iface.netmask << std::endl;
        if (!iface.status.empty()) {
            std::cout << "      Status: " << iface.status << std::endl;
        }
    }
    
    // Create sample log for parsing
    std::cout << "\n3.2 Parsing log files with regex:" << std::endl;
    std::string sample_log = 
        "[2024-01-15 10:30:45] INFO: Application started\n"
        "[2024-01-15 10:30:46] DEBUG: Loading configuration\n"
        "[2024-01-15 10:30:47] ERROR: Database connection failed\n"
        "[2024-01-15 10:30:48] WARN: Retrying connection\n"
        "[2024-01-15 10:30:50] INFO: Connection established\n";
    
    auto log_entries = parse_log_with_regex(sample_log);
    
    std::cout << "   Found " << log_entries.size() << " log entries:" << std::endl;
    for (const auto& entry : log_entries) {
        std::cout << "   [" << entry.timestamp << "] " 
                  << entry.level << ": " << entry.message << std::endl;
    }
    
    // Filter errors
    std::cout << "\n   Errors only:" << std::endl;
    for (const auto& entry : log_entries) {
        if (entry.level == "ERROR") {
            std::cout << "   ⚠️  " << entry.message << std::endl;
        }
    }
    
    std::cout << "\n💡 regex use cases:" << std::endl;
    std::cout << "   • Structured text parsing (logs, config)" << std::endl;
    std::cout << "   • Validation (emails, IPs, dates)" << std::endl;
    std::cout << "   • Extraction from unstructured output" << std::endl;
}

// ===================================================================
// SECTION 4: std::filesystem OPERATIONS
// ===================================================================

void demonstrate_filesystem() {
    std::cout << "\n=== 4. std::filesystem OPERATIONS ===" << std::endl;
    
    std::error_code ec;
    
    // Current path
    std::cout << "\n4.1 Current path:" << std::endl;
    fs::path current = fs::current_path(ec);
    std::cout << "   " << current << std::endl;
    
    // Create temporary test directory
    fs::path test_dir = fs::temp_directory_path(ec) / "cpp_test_dir";
    std::cout << "\n4.2 Creating test directory:" << std::endl;
    std::cout << "   " << test_dir << std::endl;
    
    if (fs::create_directories(test_dir, ec)) {
        std::cout << "   ✅ Created" << std::endl;
    } else {
        std::cout << "   ℹ️  Already exists or error: " << ec.message() << std::endl;
    }
    
    // Create test files
    std::cout << "\n4.3 Creating test files:" << std::endl;
    for (int i = 0; i < 3; i++) {
        fs::path file = test_dir / ("test_file_" + std::to_string(i) + ".txt");
        std::ofstream ofs(file);
        ofs << "Test content " << i << "\n";
        ofs << "Line 2 of file " << i << "\n";
        std::cout << "   Created: " << file.filename() << std::endl;
    }
    
    // Iterate directory
    std::cout << "\n4.4 Directory contents:" << std::endl;
    for (const auto& entry : fs::directory_iterator(test_dir, ec)) {
        std::cout << "   " << entry.path().filename() 
                  << " (" << fs::file_size(entry, ec) << " bytes)" << std::endl;
    }
    
    // File info
    std::cout << "\n4.5 File properties:" << std::endl;
    auto first_file = test_dir / "test_file_0.txt";
    if (fs::exists(first_file, ec)) {
        std::cout << "   Size: " << fs::file_size(first_file, ec) << " bytes" << std::endl;
        std::cout << "   Is regular file: " << fs::is_regular_file(first_file, ec) << std::endl;
        std::cout << "   Is directory: " << fs::is_directory(first_file, ec) << std::endl;
        
        auto ftime = fs::last_write_time(first_file, ec);
        std::cout << "   Last modified: " << std::chrono::system_clock::to_time_t(
            std::chrono::file_clock::to_sys(ftime)) << " (unix timestamp)" << std::endl;
    }
    
    // Path operations
    std::cout << "\n4.6 Path operations:" << std::endl;
    fs::path example = "/home/user/documents/file.txt";
    std::cout << "   Full path: " << example << std::endl;
    std::cout << "   Filename: " << example.filename() << std::endl;
    std::cout << "   Stem: " << example.stem() << std::endl;
    std::cout << "   Extension: " << example.extension() << std::endl;
    std::cout << "   Parent: " << example.parent_path() << std::endl;
    
    // Cleanup
    std::cout << "\n4.7 Cleanup:" << std::endl;
    std::uintmax_t removed = fs::remove_all(test_dir, ec);
    std::cout << "   Removed " << removed << " files/directories" << std::endl;
    
    std::cout << "\n💡 filesystem features:" << std::endl;
    std::cout << "   • Cross-platform path handling" << std::endl;
    std::cout << "   • Iterator for directory traversal" << std::endl;
    std::cout << "   • Error codes for exception-free errors" << std::endl;
}

// ===================================================================
// SECTION 5: RUNNING BASH SCRIPTS
// ===================================================================

void create_test_bash_script(const fs::path& script_path) {
    std::ofstream script(script_path);
    script << "#!/bin/bash\n";
    script << "# Test bash script called from C++\n\n";
    script << "echo \"Script started with $# arguments\"\n";
    script << "echo \"Arguments: $@\"\n\n";
    script << "# Process arguments\n";
    script << "for arg in \"$@\"; do\n";
    script << "    echo \"  - Processing: $arg\"\n";
    script << "done\n\n";
    script << "# Return some data\n";
    script << "echo \"Hostname: $(hostname)\"\n";
    script << "echo \"User: $(whoami)\"\n";
    script << "echo \"Date: $(date +%Y-%m-%d)\"\n";
    script << "\n";
    script << "exit 0\n";
    
    fs::permissions(script_path, 
                   fs::perms::owner_all | fs::perms::group_read | fs::perms::others_read,
                   fs::perm_options::add);
}

void demonstrate_bash_scripts() {
    std::cout << "\n=== 5. RUNNING BASH SCRIPTS ===" << std::endl;
    
    std::error_code ec;
    fs::path temp_dir = fs::temp_directory_path(ec);
    fs::path script_path = temp_dir / "test_script.sh";
    
    std::cout << "\n5.1 Creating bash script:" << std::endl;
    std::cout << "   " << script_path << std::endl;
    
    create_test_bash_script(script_path);
    
    // Execute script with arguments
    std::cout << "\n5.2 Executing script with arguments:" << std::endl;
    std::string command = "bash " + script_path.string() + " arg1 arg2 arg3";
    std::string output = execute_command(command);
    std::cout << output;
    
    // Parse script output with string_view
    std::cout << "\n5.3 Parsing script output:" << std::endl;
    std::regex hostname_pattern(R"(Hostname:\s+(\S+))");
    std::regex user_pattern(R"(User:\s+(\S+))");
    std::regex date_pattern(R"(Date:\s+(\S+))");
    
    std::smatch match;
    std::string output_str(output);
    
    if (std::regex_search(output_str, match, hostname_pattern)) {
        std::cout << "   Hostname: " << match[1].str() << std::endl;
    }
    if (std::regex_search(output_str, match, user_pattern)) {
        std::cout << "   User: " << match[1].str() << std::endl;
    }
    if (std::regex_search(output_str, match, date_pattern)) {
        std::cout << "   Date: " << match[1].str() << std::endl;
    }
    
    // Cleanup
    fs::remove(script_path, ec);
    
    std::cout << "\n💡 Script execution:" << std::endl;
    std::cout << "   • Use bash explicitly for portability" << std::endl;
    std::cout << "   • Pass arguments safely (avoid injection)" << std::endl;
    std::cout << "   • Parse structured output with regex" << std::endl;
}

// ===================================================================
// SECTION 6: RUNNING PYTHON SCRIPTS
// ===================================================================

void create_test_python_script(const fs::path& script_path) {
    std::ofstream script(script_path);
    script << "#!/usr/bin/env python3\n";
    script << "import sys\n";
    script << "import json\n";
    script << "import platform\n\n";
    script << "def main():\n";
    script << "    # Get system info\n";
    script << "    info = {\n";
    script << "        'python_version': sys.version.split()[0],\n";
    script << "        'platform': platform.system(),\n";
    script << "        'architecture': platform.machine(),\n";
    script << "        'arguments': sys.argv[1:]\n";
    script << "    }\n";
    script << "    \n";
    script << "    # Output as JSON for easy parsing\n";
    script << "    print(json.dumps(info, indent=2))\n\n";
    script << "if __name__ == '__main__':\n";
    script << "    main()\n";
    
    fs::permissions(script_path, 
                   fs::perms::owner_all | fs::perms::group_read | fs::perms::others_read,
                   fs::perm_options::add);
}

void demonstrate_python_scripts() {
    std::cout << "\n=== 6. RUNNING PYTHON SCRIPTS ===" << std::endl;
    
    std::error_code ec;
    fs::path temp_dir = fs::temp_directory_path(ec);
    fs::path script_path = temp_dir / "test_script.py";
    
    std::cout << "\n6.1 Creating Python script:" << std::endl;
    std::cout << "   " << script_path << std::endl;
    
    create_test_python_script(script_path);
    
    // Execute Python script
    std::cout << "\n6.2 Executing Python script:" << std::endl;
    std::string command = "python3 " + script_path.string() + " data1 data2";
    auto result = execute_command_with_status(command);
    
    if (result.success) {
        std::cout << "   ✅ Script executed successfully" << std::endl;
        std::cout << "\n6.3 JSON Output:\n" << result.output << std::endl;
        
        // Parse JSON output with regex (simple approach)
        std::cout << "6.4 Extracting data:" << std::endl;
        std::regex version_pattern("\"python_version\"\\s*:\\s*\"([^\"]+)\"");
        std::regex platform_pattern("\"platform\"\\s*:\\s*\"([^\"]+)\"");
        std::regex arch_pattern("\"architecture\"\\s*:\\s*\"([^\"]+)\"");
        
        std::smatch match;
        if (std::regex_search(result.output, match, version_pattern)) {
            std::cout << "   Python version: " << match[1].str() << std::endl;
        }
        if (std::regex_search(result.output, match, platform_pattern)) {
            std::cout << "   Platform: " << match[1].str() << std::endl;
        }
        if (std::regex_search(result.output, match, arch_pattern)) {
            std::cout << "   Architecture: " << match[1].str() << std::endl;
        }
    } else {
        std::cout << "   ❌ Script failed with code: " << result.return_code << std::endl;
    }
    
    // Cleanup
    fs::remove(script_path, ec);
    
    std::cout << "\n💡 Python integration:" << std::endl;
    std::cout << "   • Use JSON for structured data exchange" << std::endl;
    std::cout << "   • Check return codes" << std::endl;
    std::cout << "   • Python for quick prototyping/data processing" << std::endl;
}

// ===================================================================
// SECTION 7: PARSING LINUX TOOL OUTPUT
// ===================================================================

struct DiskInfo {
    std::string filesystem;
    std::string size;
    std::string used;
    std::string available;
    std::string use_percent;
    std::string mounted_on;
};

std::vector<DiskInfo> parse_df_output(std::string_view output) {
    std::vector<DiskInfo> disks;
    
    size_t line_start = 0;
    size_t line_end = output.find('\n');
    
    // Skip header
    if (line_end != std::string_view::npos) {
        line_start = line_end + 1;
    }
    
    while (line_start < output.size()) {
        line_end = output.find('\n', line_start);
        if (line_end == std::string_view::npos) {
            line_end = output.size();
        }
        
        std::string_view line = output.substr(line_start, line_end - line_start);
        if (line.empty()) {
            line_start = line_end + 1;
            continue;
        }
        
        auto tokens = split_string_view(line, ' ');
        if (tokens.size() >= 6) {
            DiskInfo info;
            info.filesystem = std::string(tokens[0]);
            info.size = std::string(tokens[1]);
            info.used = std::string(tokens[2]);
            info.available = std::string(tokens[3]);
            info.use_percent = std::string(tokens[4]);
            info.mounted_on = std::string(tokens[5]);
            disks.push_back(info);
        }
        
        line_start = line_end + 1;
    }
    
    return disks;
}

void demonstrate_linux_tools() {
    std::cout << "\n=== 7. PARSING LINUX TOOL OUTPUT ===" << std::endl;
    
    // df command
    std::cout << "\n7.1 Disk usage (df -h):" << std::endl;
    std::string df_output = execute_command("df -h 2>/dev/null | head -10");
    auto disks = parse_df_output(df_output);
    
    for (const auto& disk : disks) {
        if (disk.filesystem.find("/dev/") == 0) {  // Real devices only
            std::cout << "   " << disk.mounted_on 
                      << ": " << disk.used << "/" << disk.size
                      << " (" << disk.use_percent << ")" << std::endl;
        }
    }
    
    // Memory info
    std::cout << "\n7.2 Memory usage:" << std::endl;
    std::string mem_output = execute_command("free -h | grep Mem");
    std::regex mem_pattern(R"(Mem:\s+(\S+)\s+(\S+)\s+(\S+))");
    std::smatch match;
    if (std::regex_search(mem_output, match, mem_pattern)) {
        std::cout << "   Total: " << match[1].str() << std::endl;
        std::cout << "   Used: " << match[2].str() << std::endl;
        std::cout << "   Free: " << match[3].str() << std::endl;
    }
    
    // CPU info
    std::cout << "\n7.3 CPU information:" << std::endl;
    std::string cpu_output = execute_command("lscpu 2>/dev/null | grep -E 'Model name|CPU\\(s\\)|Thread'");
    
    std::istringstream iss(cpu_output);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty()) {
            std::cout << "   " << line << std::endl;
        }
    }
    
    std::cout << "\n💡 Linux tools:" << std::endl;
    std::cout << "   • Rich ecosystem of system tools" << std::endl;
    std::cout << "   • Standardized output formats" << std::endl;
    std::cout << "   • Combine with grep/awk for filtering" << std::endl;
}

// ===================================================================
// SECTION 8: FILE STREAMS - ifstream, ofstream, fstream
// ===================================================================

void demonstrate_file_streams() {
    std::cout << "\n=== 8. FILE STREAMS (ifstream, ofstream, fstream) ===" << std::endl;
    
    std::error_code ec;
    fs::path temp_dir = fs::temp_directory_path(ec);
    fs::path test_file = temp_dir / "stream_test.txt";
    
    // 8.1 Writing with ofstream
    std::cout << "\n8.1 Writing with ofstream:" << std::endl;
    {
        std::ofstream ofs(test_file);
        if (!ofs) {
            std::cout << "   ❌ Failed to open file for writing" << std::endl;
            return;
        }
        
        ofs << "Line 1: Hello, World!\n";
        ofs << "Line 2: Modern C++ Streams\n";
        ofs << "Line 3: Numbers: " << 42 << ", " << 3.14159 << "\n";
        ofs << "Line 4: Boolean: " << std::boolalpha << true << "\n";
        
        std::cout << "   ✅ Wrote 4 lines to " << test_file.filename() << std::endl;
    }  // File automatically closed when ofs goes out of scope
    
    // 8.2 Reading with ifstream
    std::cout << "\n8.2 Reading with ifstream (line by line):" << std::endl;
    {
        std::ifstream ifs(test_file);
        if (!ifs) {
            std::cout << "   ❌ Failed to open file for reading" << std::endl;
            return;
        }
        
        std::string line;
        int line_num = 1;
        while (std::getline(ifs, line)) {
            std::cout << "   [" << line_num++ << "] " << line << std::endl;
        }
    }
    
    // 8.3 Reading entire file
    std::cout << "\n8.3 Reading entire file at once:" << std::endl;
    {
        std::ifstream ifs(test_file);
        std::string content((std::istreambuf_iterator<char>(ifs)),
                           std::istreambuf_iterator<char>());
        std::cout << "   File size: " << content.size() << " bytes" << std::endl;
        std::cout << "   First 50 chars: " << content.substr(0, 50) << "..." << std::endl;
    }
    
    // 8.4 Appending to file
    std::cout << "\n8.4 Appending with std::ios::app:" << std::endl;
    {
        std::ofstream ofs(test_file, std::ios::app);  // Append mode
        ofs << "Line 5: Appended line\n";
        std::cout << "   ✅ Appended one line" << std::endl;
    }
    
    // 8.5 Random access with fstream
    std::cout << "\n8.5 Random access with fstream:" << std::endl;
    {
        std::fstream fs(test_file, std::ios::in | std::ios::out);
        
        // Seek to position
        fs.seekg(0, std::ios::end);
        std::streampos file_size = fs.tellg();
        std::cout << "   File size: " << file_size << " bytes" << std::endl;
        
        // Go back to start
        fs.seekg(0, std::ios::beg);
        
        // Read first line
        std::string first_line;
        std::getline(fs, first_line);
        std::cout << "   First line: " << first_line << std::endl;
        
        // Current position
        std::cout << "   Current position: " << fs.tellg() << std::endl;
    }
    
    // 8.6 Stream state checking
    std::cout << "\n8.6 Stream state checking:" << std::endl;
    {
        std::ifstream ifs(test_file);
        
        std::cout << "   Initial state:" << std::endl;
        std::cout << "      good(): " << ifs.good() << std::endl;
        std::cout << "      eof():  " << ifs.eof() << std::endl;
        std::cout << "      fail(): " << ifs.fail() << std::endl;
        std::cout << "      bad():  " << ifs.bad() << std::endl;
        
        // Read until EOF
        std::string line;
        while (std::getline(ifs, line)) {
            // Reading...
        }
        
        std::cout << "   After reading to EOF:" << std::endl;
        std::cout << "      eof():  " << ifs.eof() << std::endl;
        std::cout << "      good(): " << ifs.good() << std::endl;
        
        // Clear error state
        ifs.clear();
        std::cout << "   After clear():" << std::endl;
        std::cout << "      good(): " << ifs.good() << std::endl;
    }
    
    // Cleanup
    fs::remove(test_file, ec);
    
    std::cout << "\n💡 File stream tips:" << std::endl;
    std::cout << "   • RAII - files auto-close when stream destroyed" << std::endl;
    std::cout << "   • Check state with good(), fail(), bad(), eof()" << std::endl;
    std::cout << "   • Use seekg()/tellg() for reading position" << std::endl;
    std::cout << "   • Use seekp()/tellp() for writing position" << std::endl;
}

// ===================================================================
// SECTION 9: STRING STREAMS - ostringstream, istringstream
// ===================================================================

struct LogRecord {
    std::string timestamp;
    std::string level;
    int thread_id;
    std::string message;
};

void demonstrate_string_streams() {
    std::cout << "\n=== 9. STRING STREAMS (ostringstream, istringstream) ===" << std::endl;
    
    // 9.1 ostringstream - building strings
    std::cout << "\n9.1 Building strings with ostringstream:" << std::endl;
    {
        std::ostringstream oss;
        oss << "System report:\n";
        oss << "  CPU cores: " << 8 << "\n";
        oss << "  Memory: " << 16.5 << " GB\n";
        oss << "  Load: " << std::fixed << std::setprecision(2) << 0.6789 << "\n";
        
        std::string report = oss.str();
        std::cout << report;
    }
    
    // 9.2 istringstream - parsing strings
    std::cout << "\n9.2 Parsing with istringstream:" << std::endl;
    {
        std::string data = "2024-01-15 ERROR 12345 Database connection failed";
        std::istringstream iss(data);
        
        LogRecord record;
        iss >> record.timestamp >> record.level >> record.thread_id;
        std::getline(iss, record.message);  // Rest is message
        
        // Trim leading space from message
        if (!record.message.empty() && record.message[0] == ' ') {
            record.message = record.message.substr(1);
        }
        
        std::cout << "   Parsed log record:" << std::endl;
        std::cout << "      Timestamp: " << record.timestamp << std::endl;
        std::cout << "      Level: " << record.level << std::endl;
        std::cout << "      Thread: " << record.thread_id << std::endl;
        std::cout << "      Message: " << record.message << std::endl;
    }
    
    // 9.3 Parsing CSV
    std::cout << "\n9.3 Parsing CSV with istringstream:" << std::endl;
    {
        std::string csv = "John,Doe,30,Engineer\nJane,Smith,28,Designer\nBob,Johnson,35,Manager";
        std::istringstream iss(csv);
        std::string line;
        
        while (std::getline(iss, line)) {
            std::istringstream line_stream(line);
            std::string first, last, age, role;
            
            std::getline(line_stream, first, ',');
            std::getline(line_stream, last, ',');
            std::getline(line_stream, age, ',');
            std::getline(line_stream, role, ',');
            
            std::cout << "   " << first << " " << last 
                      << " (" << age << ") - " << role << std::endl;
        }
    }
    
    // 9.4 Number conversion with stringstream
    std::cout << "\n9.4 Type conversions:" << std::endl;
    {
        // String to number
        std::string num_str = "42.5";
        std::istringstream iss(num_str);
        double value;
        iss >> value;
        std::cout << "   String \"" << num_str << "\" -> double: " << value << std::endl;
        
        // Number to string (alternative to std::to_string with formatting)
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << 3.14159265;
        std::cout << "   double 3.14159265 -> string: \"" << oss.str() << "\"" << std::endl;
        
        // Hex conversion
        oss.str("");  // Clear
        oss.clear();  // Clear state
        oss << "0x" << std::hex << 255;
        std::cout << "   int 255 -> hex string: \"" << oss.str() << "\"" << std::endl;
    }
    
    // 9.5 Building complex formatted strings
    std::cout << "\n9.5 Complex string formatting:" << std::endl;
    {
        std::ostringstream oss;
        
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        
        oss << "╔══════════════════════════════════════╗\n";
        oss << "║         SYSTEM STATUS REPORT         ║\n";
        oss << "╠══════════════════════════════════════╣\n";
        oss << "║ Time:     " << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S") << "          ║\n";
        oss << "║ Status:   " << std::left << std::setw(22) << "Running" << "║\n";
        oss << "║ Uptime:   " << std::right << std::setw(10) << "99.9%" << "            ║\n";
        oss << "╚══════════════════════════════════════╝\n";
        
        std::cout << oss.str();
    }
    
    std::cout << "\n💡 String stream benefits:" << std::endl;
    std::cout << "   • Type-safe string building" << std::endl;
    std::cout << "   • Parsing with >> operator" << std::endl;
    std::cout << "   • Full stream manipulator support" << std::endl;
    std::cout << "   • Reusable: clear() + str(\"\") to reset" << std::endl;
}

// ===================================================================
// SECTION 10: STREAM MANIPULATORS AND FORMATTING
// ===================================================================

void demonstrate_stream_manipulators() {
    std::cout << "\n=== 10. STREAM MANIPULATORS ===" << std::endl;
    
    // 10.1 Numeric formatting
    std::cout << "\n10.1 Numeric formatting:" << std::endl;
    {
        double pi = 3.14159265358979;
        int num = 255;
        
        std::cout << "   Default: " << pi << std::endl;
        std::cout << "   Fixed (6 decimals): " << std::fixed << pi << std::endl;
        std::cout << "   Scientific: " << std::scientific << pi << std::endl;
        std::cout << std::defaultfloat;  // Reset to default
        
        std::cout << "   Precision 3: " << std::setprecision(3) << pi << std::endl;
        std::cout << "   Precision 10: " << std::setprecision(10) << pi << std::endl;
        
        std::cout << "   Decimal: " << std::dec << num << std::endl;
        std::cout << "   Hex: 0x" << std::hex << num << std::endl;
        std::cout << "   Octal: 0" << std::oct << num << std::endl;
        std::cout << std::dec;  // Reset to decimal
    }
    
    // 10.2 Width and alignment
    std::cout << "\n10.2 Width and alignment:" << std::endl;
    {
        std::cout << "   |" << std::setw(15) << "Right" << "|" << std::endl;
        std::cout << "   |" << std::left << std::setw(15) << "Left" << "|" << std::endl;
        std::cout << "   |" << std::internal << std::setw(15) << -123 << "|" << std::endl;
        std::cout << std::right;  // Reset
    }
    
    // 10.3 Fill character
    std::cout << "\n10.3 Fill character:" << std::endl;
    {
        std::cout << "   " << std::setfill('*') << std::setw(20) << "Title" << std::setfill(' ') << std::endl;
        std::cout << "   " << std::setfill('0') << std::setw(8) << 42 << std::setfill(' ') << std::endl;
        std::cout << "   " << std::setfill('-') << std::setw(30) << "" << std::setfill(' ') << std::endl;
    }
    
    // 10.4 Boolean formatting
    std::cout << "\n10.4 Boolean formatting:" << std::endl;
    {
        bool flag = true;
        std::cout << "   Default: " << flag << std::endl;
        std::cout << "   Boolalpha: " << std::boolalpha << flag << std::endl;
        std::cout << std::noboolalpha;  // Reset
    }
    
    // 10.5 Sign formatting
    std::cout << "\n10.5 Sign formatting:" << std::endl;
    {
        int pos = 42, neg = -42;
        std::cout << "   Default: " << pos << ", " << neg << std::endl;
        std::cout << "   Showpos: " << std::showpos << pos << ", " << neg << std::endl;
        std::cout << std::noshowpos;  // Reset
    }
    
    // 10.6 Table formatting example
    std::cout << "\n10.6 Table formatting:" << std::endl;
    {
        struct Product {
            std::string name;
            double price;
            int quantity;
        };
        
        std::vector<Product> products = {
            {"Widget", 29.99, 15},
            {"Gadget", 149.50, 8},
            {"Doohickey", 9.95, 42}
        };
        
        std::cout << "   ┌─────────────────┬──────────┬──────────┐" << std::endl;
        std::cout << "   │ " << std::left << std::setw(15) << "Product"
                  << " │ " << std::right << std::setw(8) << "Price"
                  << " │ " << std::setw(8) << "Qty" << " │" << std::endl;
        std::cout << "   ├─────────────────┼──────────┼──────────┤" << std::endl;
        
        for (const auto& p : products) {
            std::cout << "   │ " << std::left << std::setw(15) << p.name
                      << " │ $" << std::right << std::setw(7) << std::fixed 
                      << std::setprecision(2) << p.price
                      << " │ " << std::setw(8) << p.quantity << " │" << std::endl;
        }
        
        std::cout << "   └─────────────────┴──────────┴──────────┘" << std::endl;
    }
    
    std::cout << "\n💡 Manipulator categories:" << std::endl;
    std::cout << "   • Numeric: fixed, scientific, precision, hex, oct, dec" << std::endl;
    std::cout << "   • Alignment: setw, left, right, internal, setfill" << std::endl;
    std::cout << "   • Boolean: boolalpha, noboolalpha" << std::endl;
    std::cout << "   • Sign: showpos, noshowpos, showbase" << std::endl;
}

// ===================================================================
// SECTION 11: BINARY I/O
// ===================================================================

struct BinaryRecord {
    int id;
    double value;
    char name[32];
};

void demonstrate_binary_io() {
    std::cout << "\n=== 11. BINARY I/O ===" << std::endl;
    
    std::error_code ec;
    fs::path temp_dir = fs::temp_directory_path(ec);
    fs::path binary_file = temp_dir / "binary_data.bin";
    
    // 11.1 Writing binary data
    std::cout << "\n11.1 Writing binary data:" << std::endl;
    {
        std::ofstream ofs(binary_file, std::ios::binary);
        
        BinaryRecord records[] = {
            {1, 3.14159, "Record One"},
            {2, 2.71828, "Record Two"},
            {3, 1.41421, "Record Three"}
        };
        
        for (const auto& record : records) {
            ofs.write(reinterpret_cast<const char*>(&record), sizeof(BinaryRecord));
        }
        
        std::cout << "   ✅ Wrote " << sizeof(records) / sizeof(BinaryRecord) 
                  << " binary records (" << sizeof(records) << " bytes)" << std::endl;
    }
    
    // 11.2 Reading binary data
    std::cout << "\n11.2 Reading binary data:" << std::endl;
    {
        std::ifstream ifs(binary_file, std::ios::binary);
        
        // Get file size
        ifs.seekg(0, std::ios::end);
        std::streamsize size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        
        std::cout << "   File size: " << size << " bytes" << std::endl;
        std::cout << "   Records: " << size / sizeof(BinaryRecord) << std::endl;
        
        BinaryRecord record;
        int count = 0;
        while (ifs.read(reinterpret_cast<char*>(&record), sizeof(BinaryRecord))) {
            std::cout << "   Record " << ++count << ": "
                      << "id=" << record.id << ", "
                      << "value=" << record.value << ", "
                      << "name=\"" << record.name << "\"" << std::endl;
        }
    }
    
    // 11.3 Writing POD types
    std::cout << "\n11.3 Writing POD (Plain Old Data) types:" << std::endl;
    {
        fs::path pod_file = temp_dir / "pod_data.bin";
        
        std::ofstream ofs(pod_file, std::ios::binary);
        
        int integer = 42;
        double floating = 3.14159;
        char character = 'X';
        
        ofs.write(reinterpret_cast<const char*>(&integer), sizeof(integer));
        ofs.write(reinterpret_cast<const char*>(&floating), sizeof(floating));
        ofs.write(reinterpret_cast<const char*>(&character), sizeof(character));
        
        ofs.close();
        
        // Read back
        std::ifstream ifs(pod_file, std::ios::binary);
        
        int read_int;
        double read_double;
        char read_char;
        
        ifs.read(reinterpret_cast<char*>(&read_int), sizeof(read_int));
        ifs.read(reinterpret_cast<char*>(&read_double), sizeof(read_double));
        ifs.read(reinterpret_cast<char*>(&read_char), sizeof(read_char));
        
        std::cout << "   Read back: " << read_int << ", " 
                  << read_double << ", " << read_char << std::endl;
        
        fs::remove(pod_file, ec);
    }
    
    // Cleanup
    fs::remove(binary_file, ec);
    
    std::cout << "\n💡 Binary I/O notes:" << std::endl;
    std::cout << "   • Use std::ios::binary flag" << std::endl;
    std::cout << "   • Only for POD types (no pointers, virtual functions)" << std::endl;
    std::cout << "   • Platform-dependent (endianness, padding)" << std::endl;
    std::cout << "   • Use serialization libraries for portability" << std::endl;
}

// ===================================================================
// SECTION 12: CUSTOM STREAM OPERATORS
// ===================================================================

struct Point {
    double x, y, z;
};

// Custom output operator
std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "Point(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
}

// Custom input operator
std::istream& operator>>(std::istream& is, Point& p) {
    char paren, comma1, comma2;
    std::string word;
    
    // Expected format: Point(1.0, 2.0, 3.0)
    is >> word >> paren >> p.x >> comma1 >> p.y >> comma2 >> p.z >> paren;
    
    return is;
}

void demonstrate_custom_operators() {
    std::cout << "\n=== 12. CUSTOM STREAM OPERATORS ===" << std::endl;
    
    // 12.1 Custom output
    std::cout << "\n12.1 Custom output operator:" << std::endl;
    {
        Point p1{1.5, 2.7, 3.9};
        std::cout << "   " << p1 << std::endl;
        
        // Works with string streams too
        std::ostringstream oss;
        oss << "Point data: " << p1;
        std::cout << "   Stringified: " << oss.str() << std::endl;
    }
    
    // 12.2 Custom input
    std::cout << "\n12.2 Custom input operator:" << std::endl;
    {
        std::string input = "Point(4.5, 5.6, 6.7)";
        std::istringstream iss(input);
        
        Point p2;
        iss >> p2;
        
        std::cout << "   Parsed from string: " << p2 << std::endl;
    }
    
    // 12.3 Chaining
    std::cout << "\n12.3 Operator chaining:" << std::endl;
    {
        Point p1{1.0, 2.0, 3.0};
        Point p2{4.0, 5.0, 6.0};
        
        std::cout << "   Multiple points: " << p1 << " and " << p2 << std::endl;
    }
    
    std::cout << "\n💡 Custom operator benefits:" << std::endl;
    std::cout << "   • Natural syntax: cout << myObject" << std::endl;
    std::cout << "   • Works with all streams (file, string, cout)" << std::endl;
    std::cout << "   • Chainable for multiple operations" << std::endl;
    std::cout << "   • Consistent with standard library" << std::endl;
}

// ===================================================================
// SECTION 13: MUTEXES AND LOCK_GUARD
// ===================================================================

// Shared resource with thread-safe access
class SharedCounter {
private:
    int counter = 0;
    mutable std::mutex mtx;  // mutable allows locking in const methods
    
public:
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);  // RAII lock
        ++counter;
    }  // Automatic unlock when lock goes out of scope
    
    void add(int value) {
        std::lock_guard<std::mutex> lock(mtx);
        counter += value;
    }
    
    int get() const {
        std::lock_guard<std::mutex> lock(mtx);
        return counter;
    }
};

// File logger with mutex protection
class ThreadSafeLogger {
private:
    std::ofstream log_file;
    std::mutex file_mutex;
    
public:
    ThreadSafeLogger(const std::string& filename) : log_file(filename, std::ios::app) {}
    
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(file_mutex);
        
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        
        log_file << "[" << std::put_time(std::localtime(&now_time_t), "%H:%M:%S") 
                 << "] " << message << std::endl;
    }
    
    ~ThreadSafeLogger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
};

void demonstrate_mutex_and_lock_guard() {
    std::cout << "\n=== 13. MUTEXES AND LOCK_GUARD ===" << std::endl;
    
    // 13.1 Basic lock_guard usage
    std::cout << "\n13.1 std::lock_guard for automatic RAII locking:" << std::endl;
    {
        SharedCounter counter;
        std::vector<std::thread> threads;
        
        // Launch 10 threads that increment counter
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&counter]() {
                for (int j = 0; j < 100; ++j) {
                    counter.increment();
                }
            });
        }
        
        // Wait for all threads
        for (auto& t : threads) {
            t.join();
        }
        
        std::cout << "   Final counter value: " << counter.get() << std::endl;
        std::cout << "   Expected: 1000" << std::endl;
        std::cout << "   ✅ Thread-safe with lock_guard!" << std::endl;
    }
    
    // 13.2 Thread-safe file logging
    std::cout << "\n13.2 Thread-safe file logging:" << std::endl;
    {
        std::error_code ec;
        fs::path temp_dir = fs::temp_directory_path(ec);
        fs::path log_file = temp_dir / "threadsafe_log.txt";
        
        ThreadSafeLogger logger(log_file.string());
        std::vector<std::thread> threads;
        
        // Multiple threads writing to log
        for (int i = 0; i < 5; ++i) {
            threads.emplace_back([&logger, i]() {
                for (int j = 0; j < 3; ++j) {
                    std::ostringstream oss;
                    oss << "Thread " << i << " message " << j;
                    logger.log(oss.str());
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        std::cout << "   ✅ 15 log entries written safely" << std::endl;
        std::cout << "   Log file: " << log_file.filename() << std::endl;
        
        // Read and display log
        std::ifstream ifs(log_file);
        std::string line;
        int count = 0;
        std::cout << "   First 5 entries:" << std::endl;
        while (std::getline(ifs, line) && count++ < 5) {
            std::cout << "      " << line << std::endl;
        }
        
        fs::remove(log_file, ec);
    }
    
    std::cout << "\n💡 lock_guard benefits:" << std::endl;
    std::cout << "   • RAII - automatic unlock on scope exit" << std::endl;
    std::cout << "   • Exception-safe" << std::endl;
    std::cout << "   • Simple and efficient" << std::endl;
    std::cout << "   • Cannot forget to unlock" << std::endl;
}

// ===================================================================
// SECTION 14: UNIQUE_LOCK AND ADVANCED LOCKING
// ===================================================================

class BankAccount {
private:
    double balance = 0.0;
    mutable std::mutex mtx;
    std::condition_variable cv;
    
public:
    BankAccount(double initial) : balance(initial) {}
    
    bool withdraw(double amount) {
        std::unique_lock<std::mutex> lock(mtx);
        
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }
    
    void deposit(double amount) {
        std::unique_lock<std::mutex> lock(mtx);
        balance += amount;
        cv.notify_all();  // Wake up waiting threads
    }
    
    // Wait until sufficient funds available
    bool wait_and_withdraw(double amount, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Wait with timeout
        if (!cv.wait_for(lock, timeout, [this, amount]() { 
            return balance >= amount; 
        })) {
            return false;  // Timeout
        }
        
        balance -= amount;
        return true;
    }
    
    double get_balance() const {
        std::unique_lock<std::mutex> lock(mtx);
        return balance;
    }
};

// Reader-writer lock example
class SharedData {
private:
    std::map<std::string, int> data;
    mutable std::shared_mutex rw_mutex;
    
public:
    // Multiple readers can access simultaneously
    int read(const std::string& key) const {
        std::shared_lock<std::shared_mutex> lock(rw_mutex);
        auto it = data.find(key);
        return (it != data.end()) ? it->second : 0;
    }
    
    // Only one writer at a time
    void write(const std::string& key, int value) {
        std::unique_lock<std::shared_mutex> lock(rw_mutex);
        data[key] = value;
    }
    
    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(rw_mutex);
        return data.size();
    }
};

void demonstrate_unique_lock() {
    std::cout << "\n=== 14. UNIQUE_LOCK AND ADVANCED LOCKING ===" << std::endl;
    
    // 14.1 unique_lock - more flexible than lock_guard
    std::cout << "\n14.1 std::unique_lock features:" << std::endl;
    {
        std::mutex mtx;
        
        std::cout << "   • Can be locked/unlocked multiple times:" << std::endl;
        {
            std::unique_lock<std::mutex> lock(mtx);
            std::cout << "      Locked" << std::endl;
            
            lock.unlock();  // Explicit unlock
            std::cout << "      Unlocked (doing non-critical work)" << std::endl;
            
            lock.lock();  // Relock
            std::cout << "      Locked again" << std::endl;
        }
        
        std::cout << "   • Deferred locking:" << std::endl;
        {
            std::unique_lock<std::mutex> lock(mtx, std::defer_lock);  // Don't lock yet
            std::cout << "      Created unlocked" << std::endl;
            
            lock.lock();  // Lock when needed
            std::cout << "      Now locked" << std::endl;
        }
    }
    
    // 14.2 Condition variable with unique_lock
    std::cout << "\n14.2 Condition variable (wait with timeout):" << std::endl;
    {
        BankAccount account(100.0);
        
        std::cout << "   Initial balance: $" << account.get_balance() << std::endl;
        
        // Try to withdraw more than available
        std::thread withdrawal_thread([&account]() {
            std::cout << "   Attempting to withdraw $150 (waiting up to 500ms)..." << std::endl;
            bool success = account.wait_and_withdraw(150.0, std::chrono::milliseconds(500));
            
            if (success) {
                std::cout << "   ✅ Withdrawal successful" << std::endl;
            } else {
                std::cout << "   ⏱️  Timeout - insufficient funds" << std::endl;
            }
        });
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        // Deposit money to unblock
        std::cout << "   Depositing $75..." << std::endl;
        account.deposit(75.0);
        
        withdrawal_thread.join();
        std::cout << "   Final balance: $" << account.get_balance() << std::endl;
    }
    
    // 14.3 Shared mutex for reader-writer locks
    std::cout << "\n14.3 std::shared_mutex (reader-writer lock):" << std::endl;
    {
        SharedData shared;
        std::atomic<int> read_count{0};
        std::atomic<int> write_count{0};
        
        std::vector<std::thread> threads;
        
        // 8 readers
        for (int i = 0; i < 8; ++i) {
            threads.emplace_back([&shared, &read_count, i]() {
                for (int j = 0; j < 10; ++j) {
                    shared.read("key" + std::to_string(i % 3));
                    read_count++;
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
            });
        }
        
        // 2 writers
        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&shared, &write_count, i]() {
                for (int j = 0; j < 5; ++j) {
                    shared.write("key" + std::to_string(j), i * 100 + j);
                    write_count++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        std::cout << "   Completed " << read_count << " reads" << std::endl;
        std::cout << "   Completed " << write_count << " writes" << std::endl;
        std::cout << "   Final data size: " << shared.size() << std::endl;
        std::cout << "   ✅ Multiple readers can run concurrently!" << std::endl;
    }
    
    std::cout << "\n💡 unique_lock vs lock_guard:" << std::endl;
    std::cout << "   lock_guard:  Simple, RAII, no unlock() method" << std::endl;
    std::cout << "   unique_lock: Flexible, can unlock/relock, works with condition_variable" << std::endl;
    std::cout << "\n💡 shared_mutex:" << std::endl;
    std::cout << "   • shared_lock:  Multiple readers simultaneously" << std::endl;
    std::cout << "   • unique_lock:  Exclusive writer access" << std::endl;
    std::cout << "   • Perfect for read-heavy workloads" << std::endl;
}

// ===================================================================
// SECTION 15: FILE LOCKING (OS-LEVEL)
// ===================================================================

#ifdef __linux__
#include <sys/file.h>  // For flock
#include <fcntl.h>
#include <unistd.h>

class FileLock {
private:
    int fd = -1;
    std::string filename;
    
public:
    FileLock(const std::string& file) : filename(file) {
        fd = open(filename.c_str(), O_RDWR | O_CREAT, 0666);
        if (fd == -1) {
            throw std::runtime_error("Failed to open file for locking");
        }
    }
    
    bool try_lock() {
        return flock(fd, LOCK_EX | LOCK_NB) == 0;  // Non-blocking exclusive lock
    }
    
    void lock() {
        if (flock(fd, LOCK_EX) != 0) {  // Blocking exclusive lock
            throw std::runtime_error("Failed to acquire lock");
        }
    }
    
    void unlock() {
        flock(fd, LOCK_UN);
    }
    
    ~FileLock() {
        if (fd != -1) {
            unlock();
            close(fd);
        }
    }
};
#endif

void demonstrate_file_locking() {
    std::cout << "\n=== 15. FILE LOCKING (OS-LEVEL) ===" << std::endl;
    
#ifdef __linux__
    std::cout << "\n15.1 Advisory file locking with flock():" << std::endl;
    {
        std::error_code ec;
        fs::path temp_dir = fs::temp_directory_path(ec);
        fs::path lock_file = temp_dir / "test.lock";
        
        try {
            FileLock lock1(lock_file.string());
            
            std::cout << "   Thread 1: Acquiring lock..." << std::endl;
            lock1.lock();
            std::cout << "   Thread 1: ✅ Lock acquired" << std::endl;
            
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Try to acquire same lock from another thread
            std::thread other_thread([&lock_file]() {
                try {
                    FileLock lock2(lock_file.string());
                    std::cout << "   Thread 2: Trying non-blocking lock..." << std::endl;
                    
                    if (lock2.try_lock()) {
                        std::cout << "   Thread 2: ✅ Lock acquired" << std::endl;
                        lock2.unlock();
                    } else {
                        std::cout << "   Thread 2: ⏸️  Lock held by another process" << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cout << "   Thread 2: Error - " << e.what() << std::endl;
                }
            });
            
            other_thread.join();
            
            std::cout << "   Thread 1: Releasing lock..." << std::endl;
            lock1.unlock();
            
        } catch (const std::exception& e) {
            std::cout << "   Error: " << e.what() << std::endl;
        }
        
        fs::remove(lock_file, ec);
    }
    
    std::cout << "\n15.2 Preventing concurrent file access:" << std::endl;
    {
        std::error_code ec;
        fs::path temp_dir = fs::temp_directory_path(ec);
        fs::path shared_file = temp_dir / "shared_data.txt";
        fs::path lock_file_path = temp_dir / "shared_data.txt.lock";
        
        // Writer with lock
        auto writer = [&shared_file, &lock_file_path](int id) {
            try {
                FileLock lock(lock_file_path.string());
                lock.lock();
                
                std::cout << "   Writer " << id << ": Writing..." << std::endl;
                std::ofstream ofs(shared_file, std::ios::app);
                ofs << "Line from writer " << id << "\n";
                ofs.close();
                
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                lock.unlock();
                std::cout << "   Writer " << id << ": Done" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "   Writer " << id << ": Error - " << e.what() << std::endl;
            }
        };
        
        std::thread w1(writer, 1);
        std::thread w2(writer, 2);
        std::thread w3(writer, 3);
        
        w1.join();
        w2.join();
        w3.join();
        
        // Read result
        std::ifstream ifs(shared_file);
        std::string line;
        int line_count = 0;
        while (std::getline(ifs, line)) {
            line_count++;
        }
        
        std::cout << "   ✅ All writers completed safely" << std::endl;
        std::cout << "   Total lines written: " << line_count << std::endl;
        
        fs::remove(shared_file, ec);
        fs::remove(lock_file_path, ec);
    }
    
    std::cout << "\n💡 File locking types:" << std::endl;
    std::cout << "   • Advisory locks: Processes must cooperate (flock)" << std::endl;
    std::cout << "   • Mandatory locks: Enforced by kernel (rare)" << std::endl;
    std::cout << "   • LOCK_EX: Exclusive lock (writer)" << std::endl;
    std::cout << "   • LOCK_SH: Shared lock (reader)" << std::endl;
    std::cout << "   • LOCK_NB: Non-blocking flag" << std::endl;
    
#else
    std::cout << "\n   ℹ️  File locking examples require Linux (flock)" << std::endl;
    std::cout << "   Windows equivalent: LockFileEx()" << std::endl;
    std::cout << "   Cross-platform: Use Boost.Interprocess" << std::endl;
#endif
}

// ===================================================================
// SECTION 16: RACE CONDITIONS - PROBLEM AND SOLUTION
// ===================================================================

// Simulated device hotplug manager - WITHOUT proper synchronization (BROKEN!)
class BrokenDeviceManager {
private:
    std::map<std::string, std::string> devices;  // device_id -> status
    int device_count = 0;
    
public:
    // Called by multiple threads - NO MUTEX!
    void add_device(const std::string& device_id) {
        // RACE CONDITION: Multiple threads can read/write simultaneously
        if (devices.find(device_id) == devices.end()) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));  // Simulate processing
            devices[device_id] = "active";
            device_count++;
        }
    }
    
    void remove_device(const std::string& device_id) {
        // RACE CONDITION: Map modification without synchronization
        if (devices.find(device_id) != devices.end()) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            devices.erase(device_id);
            device_count--;
        }
    }
    
    void process_script_output(const std::string& script_output) {
        // RACE CONDITION: Reading and updating shared state
        std::istringstream iss(script_output);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.find("ADD:") == 0) {
                std::string device = line.substr(4);
                devices[device] = "pending";
            }
        }
    }
    
    int get_count() const {
        return device_count;  // RACE CONDITION: Reading without lock
    }
    
    std::map<std::string, std::string> get_devices() const {
        return devices;  // RACE CONDITION: Copying map without lock
    }
};

// Fixed version - WITH proper synchronization
class SafeDeviceManager {
private:
    std::map<std::string, std::string> devices;
    int device_count = 0;
    mutable std::mutex mtx;  // Protects all shared state
    std::ofstream log_file;
    
public:
    SafeDeviceManager() : log_file("/tmp/device_manager.log", std::ios::app) {}
    
    void add_device(const std::string& device_id) {
        std::lock_guard<std::mutex> lock(mtx);  // FIXED: Automatic locking
        
        if (devices.find(device_id) == devices.end()) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            devices[device_id] = "active";
            device_count++;
            
            if (log_file.is_open()) {
                log_file << "[ADD] " << device_id << std::endl;
            }
        }
    }
    
    void remove_device(const std::string& device_id) {
        std::lock_guard<std::mutex> lock(mtx);  // FIXED: Protected
        
        if (devices.find(device_id) != devices.end()) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            devices.erase(device_id);
            device_count--;
            
            if (log_file.is_open()) {
                log_file << "[REMOVE] " << device_id << std::endl;
            }
        }
    }
    
    void process_script_output(const std::string& script_output) {
        std::lock_guard<std::mutex> lock(mtx);  // FIXED: Protected
        
        std::istringstream iss(script_output);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.find("ADD:") == 0) {
                std::string device = line.substr(4);
                devices[device] = "pending";
            }
        }
    }
    
    int get_count() const {
        std::lock_guard<std::mutex> lock(mtx);  // FIXED: Protected read
        return device_count;
    }
    
    std::map<std::string, std::string> get_devices() const {
        std::lock_guard<std::mutex> lock(mtx);  // FIXED: Protected copy
        return devices;
    }
    
    ~SafeDeviceManager() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
};

// Simulate hotplug events by calling external scripts
void simulate_hotplug_script(int event_id) {
    // Create a temporary script that simulates device detection
    std::error_code ec;
    fs::path temp_dir = fs::temp_directory_path(ec);
    fs::path script_path = temp_dir / ("hotplug_" + std::to_string(event_id) + ".sh");
    
    std::ofstream script(script_path);
    script << "#!/bin/bash\n";
    script << "echo 'ADD:device_" << event_id << "'\n";
    script << "sleep 0.01\n";
    script << "echo 'STATUS:online'\n";
    script.close();
    
    fs::permissions(script_path, fs::perms::owner_all, fs::perm_options::add, ec);
}

void demonstrate_race_condition() {
    std::cout << "\n=== 16. RACE CONDITIONS - PROBLEM AND SOLUTION ===" << std::endl;
    
    // 16.1 Demonstrate the BROKEN version
    std::cout << "\n16.1 ❌ BROKEN: Race condition without mutex:" << std::endl;
    {
        BrokenDeviceManager broken_mgr;
        std::vector<std::thread> threads;
        
        std::cout << "   Launching 20 threads to add devices..." << std::endl;
        
        // Multiple threads adding same devices
        for (int i = 0; i < 20; ++i) {
            threads.emplace_back([&broken_mgr, i]() {
                for (int j = 0; j < 10; ++j) {
                    std::string device_id = "dev_" + std::to_string(j);
                    broken_mgr.add_device(device_id);
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        int final_count = broken_mgr.get_count();
        auto devices = broken_mgr.get_devices();
        
        std::cout << "   Expected: 10 unique devices" << std::endl;
        std::cout << "   Device count variable: " << final_count << std::endl;
        std::cout << "   Actual devices in map: " << devices.size() << std::endl;
        
        if (final_count != 10 || devices.size() != 10) {
            std::cout << "   ⚠️  RACE CONDITION DETECTED!" << std::endl;
            std::cout << "   ⚠️  Count mismatch due to concurrent access" << std::endl;
        } else {
            std::cout << "   ℹ️  (Race condition may not always show - timing dependent)" << std::endl;
        }
    }
    
    // 16.2 Demonstrate the FIXED version
    std::cout << "\n16.2 ✅ FIXED: With mutex protection:" << std::endl;
    {
        SafeDeviceManager safe_mgr;
        std::vector<std::thread> threads;
        
        std::cout << "   Launching 20 threads to add devices..." << std::endl;
        
        for (int i = 0; i < 20; ++i) {
            threads.emplace_back([&safe_mgr, i]() {
                for (int j = 0; j < 10; ++j) {
                    std::string device_id = "dev_" + std::to_string(j);
                    safe_mgr.add_device(device_id);
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        int final_count = safe_mgr.get_count();
        auto devices = safe_mgr.get_devices();
        
        std::cout << "   Expected: 10 unique devices" << std::endl;
        std::cout << "   Device count: " << final_count << std::endl;
        std::cout << "   Devices in map: " << devices.size() << std::endl;
        
        if (final_count == 10 && devices.size() == 10) {
            std::cout << "   ✅ CORRECT: Mutex prevented race condition!" << std::endl;
        }
    }
    
    // 16.3 Real-world scenario: File updates from multiple sources
    std::cout << "\n16.3 Race condition with file I/O and scripts:" << std::endl;
    {
        std::error_code ec;
        fs::path temp_dir = fs::temp_directory_path(ec);
        fs::path shared_file = temp_dir / "device_registry.txt";
        
        // BROKEN: Multiple threads writing to file without coordination
        std::cout << "   ❌ Without file locking:" << std::endl;
        {
            // Clear file
            std::ofstream(shared_file).close();
            
            std::vector<std::thread> writers;
            
            for (int i = 0; i < 5; ++i) {
                writers.emplace_back([&shared_file, i]() {
                    for (int j = 0; j < 3; ++j) {
                        // RACE CONDITION: Multiple threads writing simultaneously
                        std::ofstream ofs(shared_file, std::ios::app);
                        ofs << "Thread_" << i << "_entry_" << j << "\n";
                        ofs.close();
                        std::this_thread::sleep_for(std::chrono::microseconds(100));
                    }
                });
            }
            
            for (auto& t : writers) {
                t.join();
            }
            
            // Count lines
            std::ifstream ifs(shared_file);
            int line_count = 0;
            std::string line;
            while (std::getline(ifs, line)) {
                if (!line.empty()) line_count++;
            }
            
            std::cout << "      Expected 15 lines, got: " << line_count << std::endl;
            if (line_count != 15) {
                std::cout << "      ⚠️  Some writes may have been lost!" << std::endl;
            }
        }
        
        // FIXED: With mutex protection
        std::cout << "   ✅ With mutex protection:" << std::endl;
        {
            // Clear file
            std::ofstream(shared_file).close();
            
            std::mutex file_mtx;
            std::vector<std::thread> writers;
            
            for (int i = 0; i < 5; ++i) {
                writers.emplace_back([&shared_file, &file_mtx, i]() {
                    for (int j = 0; j < 3; ++j) {
                        std::lock_guard<std::mutex> lock(file_mtx);  // FIXED!
                        std::ofstream ofs(shared_file, std::ios::app);
                        ofs << "Thread_" << i << "_entry_" << j << "\n";
                        ofs.close();
                        std::this_thread::sleep_for(std::chrono::microseconds(100));
                    }
                });
            }
            
            for (auto& t : writers) {
                t.join();
            }
            
            // Count lines
            std::ifstream ifs(shared_file);
            int line_count = 0;
            std::string line;
            while (std::getline(ifs, line)) {
                if (!line.empty()) line_count++;
            }
            
            std::cout << "      Expected 15 lines, got: " << line_count << std::endl;
            if (line_count == 15) {
                std::cout << "      ✅ All writes preserved!" << std::endl;
            }
        }
        
        fs::remove(shared_file, ec);
    }
    
    // 16.4 Simulating hotplug events with script execution
    std::cout << "\n16.4 Hotplug simulation (scripts + threads):" << std::endl;
    {
        SafeDeviceManager mgr;
        std::vector<std::thread> hotplug_threads;
        
        std::cout << "   Simulating 3 concurrent hotplug events..." << std::endl;
        
        for (int i = 0; i < 3; ++i) {
            hotplug_threads.emplace_back([&mgr, i]() {
                // Simulate calling udev script or hotplug handler
                std::string script_output = "ADD:usb_device_" + std::to_string(i) + "\n";
                script_output += "ADD:usb_port_" + std::to_string(i) + "\n";
                
                mgr.process_script_output(script_output);
                
                // Simulate device becoming active
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                mgr.add_device("usb_device_" + std::to_string(i));
                mgr.add_device("usb_port_" + std::to_string(i));
            });
        }
        
        for (auto& t : hotplug_threads) {
            t.join();
        }
        
        auto devices = mgr.get_devices();
        std::cout << "   Total devices registered: " << devices.size() << std::endl;
        std::cout << "   Devices:" << std::endl;
        for (const auto& [id, status] : devices) {
            std::cout << "      " << id << " -> " << status << std::endl;
        }
        std::cout << "   ✅ All hotplug events handled safely" << std::endl;
    }
    
    std::cout << "\n💡 Common race condition scenarios:" << std::endl;
    std::cout << "   1. Multiple threads modifying shared container" << std::endl;
    std::cout << "   2. Check-then-act pattern without atomicity" << std::endl;
    std::cout << "   3. Reading and writing without synchronization" << std::endl;
    std::cout << "   4. File I/O from multiple threads/processes" << std::endl;
    std::cout << "   5. Scripts updating shared resources" << std::endl;
    
    std::cout << "\n💡 Solutions:" << std::endl;
    std::cout << "   • Use std::mutex with lock_guard/unique_lock" << std::endl;
    std::cout << "   • Use std::atomic for simple counters" << std::endl;
    std::cout << "   • Use file locks for inter-process sync" << std::endl;
    std::cout << "   • Protect ALL access to shared data" << std::endl;
    std::cout << "   • Keep critical sections small" << std::endl;
}

// ===================================================================
// SECTION 17: SECURITY CONSIDERATIONS
// ===================================================================

void explain_security_considerations() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "SECURITY CONSIDERATIONS:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n⚠️ CRITICAL SECURITY ISSUES:" << std::endl;
    
    std::cout << "\n1. COMMAND INJECTION:" << std::endl;
    std::cout << "   ❌ BAD:  system(\"ls \" + user_input);" << std::endl;
    std::cout << "   ✅ GOOD: Validate and sanitize input first" << std::endl;
    std::cout << "   ✅ GOOD: Use execve() family for direct execution" << std::endl;
    
    std::cout << "\n2. SHELL METACHARACTERS:" << std::endl;
    std::cout << "   Dangerous: ; | & $ ` \\ \" ' < > ( ) { } [ ] ! #" << std::endl;
    std::cout << "   User input: \"file.txt; rm -rf /\"" << std::endl;
    std::cout << "   Result: DISASTER!" << std::endl;
    
    std::cout << "\n3. PATH TRAVERSAL:" << std::endl;
    std::cout << "   ❌ BAD:  open(\"/data/\" + user_input)" << std::endl;
    std::cout << "   User: \"../../etc/passwd\"" << std::endl;
    std::cout << "   ✅ GOOD: Validate paths, use fs::canonical()" << std::endl;
    
    std::cout << "\n4. ENVIRONMENT VARIABLES:" << std::endl;
    std::cout << "   $PATH, $LD_PRELOAD can be hijacked" << std::endl;
    std::cout << "   ✅ Use absolute paths: /usr/bin/python3" << std::endl;
    std::cout << "   ✅ Don't trust $PATH from external sources" << std::endl;
    
    std::cout << "\n✅ BEST PRACTICES:" << std::endl;
    std::cout << "\n1. INPUT VALIDATION:" << std::endl;
    std::cout << "   • Whitelist allowed characters" << std::endl;
    std::cout << "   • Use regex to validate format" << std::endl;
    std::cout << "   • Reject anything suspicious" << std::endl;
    
    std::cout << "\n2. AVOID system():" << std::endl;
    std::cout << "   • Use popen() for output capture" << std::endl;
    std::cout << "   • Use fork()+execve() for full control" << std::endl;
    std::cout << "   • Never pass user input directly to shell" << std::endl;
    
    std::cout << "\n3. LEAST PRIVILEGE:" << std::endl;
    std::cout << "   • Run with minimum necessary permissions" << std::endl;
    std::cout << "   • Drop privileges after initialization" << std::endl;
    std::cout << "   • Use separate user accounts" << std::endl;
    
    std::cout << "\n4. ERROR HANDLING:" << std::endl;
    std::cout << "   • Check all return codes" << std::endl;
    std::cout << "   • Don't expose system details in errors" << std::endl;
    std::cout << "   • Log security events" << std::endl;
}

// ===================================================================
// SECTION 18: CROSS-PLATFORM CONSIDERATIONS
// ===================================================================

void explain_cross_platform() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "CROSS-PLATFORM CONSIDERATIONS:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n🐧 LINUX vs 🪟 WINDOWS:" << std::endl;
    
    std::cout << "\n1. COMMAND EXECUTION:" << std::endl;
    std::cout << "   Linux:   popen(\"/bin/ls\", \"r\")" << std::endl;
    std::cout << "   Windows: _popen(\"dir\", \"r\")" << std::endl;
    std::cout << "   Windows: CreateProcess() for full control" << std::endl;
    
    std::cout << "\n2. PATH SEPARATORS:" << std::endl;
    std::cout << "   Linux:   /home/user/file.txt" << std::endl;
    std::cout << "   Windows: C:\\Users\\user\\file.txt" << std::endl;
    std::cout << "   ✅ Use fs::path - handles both!" << std::endl;
    
    std::cout << "\n3. LINE ENDINGS:" << std::endl;
    std::cout << "   Linux:   \\n (LF)" << std::endl;
    std::cout << "   Windows: \\r\\n (CRLF)" << std::endl;
    std::cout << "   ✅ Open files in text mode for conversion" << std::endl;
    
    std::cout << "\n4. SYSTEM TOOLS:" << std::endl;
    std::cout << "   Linux:   ps, df, grep, awk, sed" << std::endl;
    std::cout << "   Windows: tasklist, wmic, findstr, PowerShell" << std::endl;
    std::cout << "   ✅ Check platform and use appropriate tools" << std::endl;
    
    std::cout << "\n💡 PORTABLE CODE:" << std::endl;
    std::cout << "\n#ifdef _WIN32" << std::endl;
    std::cout << "    std::string cmd = \"dir\";" << std::endl;
    std::cout << "#else" << std::endl;
    std::cout << "    std::string cmd = \"ls\";" << std::endl;
    std::cout << "#endif" << std::endl;
    
    std::cout << "\nOr use std::filesystem for platform independence!" << std::endl;
}

// ===================================================================
// SECTION 19: BEST PRACTICES SUMMARY
// ===================================================================

void explain_best_practices() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "BEST PRACTICES SUMMARY:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n✅ DO:" << std::endl;
    std::cout << "\n1. Use std::filesystem for file operations" << std::endl;
    std::cout << "   • Cross-platform" << std::endl;
    std::cout << "   • Exception-safe" << std::endl;
    std::cout << "   • Modern and clean API" << std::endl;
    
    std::cout << "\n2. Use std::string_view for parsing" << std::endl;
    std::cout << "   • Zero-copy substring operations" << std::endl;
    std::cout << "   • Perfect for tokenizing large outputs" << std::endl;
    std::cout << "   • No memory allocations" << std::endl;
    
    std::cout << "\n3. Use std::regex for pattern matching" << std::endl;
    std::cout << "   • Structured text parsing" << std::endl;
    std::cout << "   • Validation" << std::endl;
    std::cout << "   • Extraction from unstructured data" << std::endl;
    
    std::cout << "\n4. Check return codes and use error_code" << std::endl;
    std::cout << "   • filesystem operations can fail" << std::endl;
    std::cout << "   • Commands can fail" << std::endl;
    std::cout << "   • Use std::error_code for exception-free errors" << std::endl;
    
    std::cout << "\n5. Use RAII for resource management" << std::endl;
    std::cout << "   • std::unique_ptr with custom deleter for FILE*" << std::endl;
    std::cout << "   • Automatic cleanup on exception" << std::endl;
    
    std::cout << "\n❌ DON'T:" << std::endl;
    std::cout << "\n1. Don't use system() with user input" << std::endl;
    std::cout << "   • Command injection vulnerability" << std::endl;
    std::cout << "   • Use execve() family instead" << std::endl;
    
    std::cout << "\n2. Don't ignore errors" << std::endl;
    std::cout << "   • Check pclose() return value" << std::endl;
    std::cout << "   • Check filesystem operation errors" << std::endl;
    
    std::cout << "\n3. Don't parse with manual string manipulation" << std::endl;
    std::cout << "   • Use string_view for efficiency" << std::endl;
    std::cout << "   • Use regex for complex patterns" << std::endl;
    
    std::cout << "\n4. Don't assume POSIX everywhere" << std::endl;
    std::cout << "   • Windows is different" << std::endl;
    std::cout << "   • Use std::filesystem for portability" << std::endl;
    
    std::cout << "\n💡 GOLDEN RULES:" << std::endl;
    std::cout << "   1. Validate all external input" << std::endl;
    std::cout << "   2. Use modern C++ facilities (filesystem, string_view)" << std::endl;
    std::cout << "   3. Check errors always" << std::endl;
    std::cout << "   4. Think security first!" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║    SYSTEM INTERACTION, FILESYSTEM, STRING_VIEW, AND REGEX       ║\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "║  Modern C++ for system programming and external tool integration║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    try {
        demonstrate_basic_execution();
        demonstrate_string_view_parsing();
        demonstrate_regex_parsing();
        demonstrate_filesystem();
        demonstrate_bash_scripts();
        demonstrate_python_scripts();
        demonstrate_linux_tools();
        demonstrate_file_streams();
        demonstrate_string_streams();
        demonstrate_stream_manipulators();
        demonstrate_binary_io();
        demonstrate_custom_operators();
        demonstrate_mutex_and_lock_guard();
        demonstrate_unique_lock();
        demonstrate_file_locking();
        demonstrate_race_condition();
        explain_security_considerations();
        explain_cross_platform();
        explain_best_practices();
        
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "SUMMARY:\n";
        std::cout << std::string(70, '=') << std::endl;
        
        std::cout << "\n🎯 KEY FEATURES DEMONSTRATED:" << std::endl;
        std::cout << "\n1. COMMAND EXECUTION:" << std::endl;
        std::cout << "   • popen() for output capture" << std::endl;
        std::cout << "   • Return code checking" << std::endl;
        std::cout << "   • RAII for resource management" << std::endl;
        
        std::cout << "\n2. std::string_view (C++17):" << std::endl;
        std::cout << "   • Zero-copy parsing" << std::endl;
        std::cout << "   • Efficient tokenization" << std::endl;
        std::cout << "   • Perfect for large outputs" << std::endl;
        
        std::cout << "\n3. std::regex (C++11):" << std::endl;
        std::cout << "   • Pattern matching" << std::endl;
        std::cout << "   • Log parsing" << std::endl;
        std::cout << "   • Network output parsing" << std::endl;
        
        std::cout << "\n4. std::filesystem (C++17):" << std::endl;
        std::cout << "   • Cross-platform paths" << std::endl;
        std::cout << "   • Directory iteration" << std::endl;
        std::cout << "   • Error code handling" << std::endl;
        
        std::cout << "\n5. STREAM I/O:" << std::endl;
        std::cout << "   • File streams (ifstream, ofstream, fstream)" << std::endl;
        std::cout << "   • String streams (ostringstream, istringstream)" << std::endl;
        std::cout << "   • Stream manipulators (setw, setprecision, etc.)" << std::endl;
        std::cout << "   • Binary I/O for POD types" << std::endl;
        std::cout << "   • Custom stream operators" << std::endl;
        
        std::cout << "\n6. SCRIPT INTEGRATION:" << std::endl;
        std::cout << "   • Bash scripts with arguments" << std::endl;
        std::cout << "   • Python scripts with JSON output" << std::endl;
        std::cout << "   • Structured data exchange" << std::endl;
        
        std::cout << "\n7. THREAD SYNCHRONIZATION:" << std::endl;
        std::cout << "   • std::mutex and std::lock_guard (RAII)" << std::endl;
        std::cout << "   • std::unique_lock (flexible locking)" << std::endl;
        std::cout << "   • std::shared_mutex (reader-writer locks)" << std::endl;
        std::cout << "   • std::condition_variable (wait/notify)" << std::endl;
        std::cout << "   • OS-level file locking (flock)" << std::endl;
        std::cout << "   • Race condition examples and fixes" << std::endl;
        
        std::cout << "\n⚠️ SECURITY REMINDERS:" << std::endl;
        std::cout << "   • Never pass unsanitized user input to shell" << std::endl;
        std::cout << "   • Validate all paths and arguments" << std::endl;
        std::cout << "   • Use absolute paths for commands" << std::endl;
        std::cout << "   • Check return codes always" << std::endl;
        
        std::cout << "\n✅ Modern C++: Safe, efficient system programming!\n" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
