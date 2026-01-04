// ===================================================================
// THREAD-SAFETY AND RE-ENTRANCY FOR EMBEDDED/FIRMWARE SYSTEMS
// ===================================================================
// Comprehensive guide to writing safe concurrent code for:
//   • Bare-metal embedded systems (no OS)
//   • RTOS-based firmware
//   • Interrupt Service Routines (ISRs)
//   • Real-time systems
//   • Memory-constrained environments
//
// KEY CONCEPTS:
//   • Re-entrant: Function can be interrupted and safely called again
//   • Thread-safe: Function safe for concurrent access from multiple threads
//   • ISR-safe: Safe to call from interrupt context
//   • Atomic: Operation completes without interruption
//
// Build: g++ -std=c++17 -O2 -pthread -o EmbeddedThreadSafety EmbeddedThreadSafetyReentrancy.cpp
// ===================================================================

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <cstdint>
#include <chrono>
#include <mutex>
#include <array>

using namespace std;

// ===================================================================
// PART 1: RE-ENTRANT vs THREAD-SAFE vs ISR-SAFE
// ===================================================================

// ❌ NOT RE-ENTRANT: Uses static variable
int not_reentrant_counter() {
    static int count = 0;  // PROBLEM: Shared state
    count++;
    // If interrupted here and called again, count is corrupted!
    return count;
}

// ✅ RE-ENTRANT: No shared state, uses only local variables
int reentrant_counter(int& count) {
    count++;  // Operates on caller's variable
    return count;
}

// ❌ NOT ISR-SAFE: Uses mutex (cannot be used in ISR)
class NotIsrSafe {
    mutex mtx;
    int value = 0;
    
public:
    void increment() {
        lock_guard<mutex> lock(mtx);  // PROBLEM: Mutex in ISR = deadlock!
        value++;
    }
};

// ✅ ISR-SAFE: Uses atomic operations
class IsrSafe {
    atomic<int> value{0};
    
public:
    void increment() {
        value.fetch_add(1, memory_order_relaxed);  // Safe for ISR
    }
    
    int get() const {
        return value.load(memory_order_relaxed);
    }
};

void demo_reentrant_vs_threadsafe() {
    cout << "\n=== RE-ENTRANT vs THREAD-SAFE vs ISR-SAFE ===\n\n";
    
    cout << "📋 DEFINITIONS:\n";
    cout << "• RE-ENTRANT:  Function can be interrupted and called again safely\n";
    cout << "               - Uses only local variables or parameters\n";
    cout << "               - No shared state (static, global)\n";
    cout << "               - No locks (they're not re-entrant!)\n\n";
    
    cout << "• THREAD-SAFE: Function safe for concurrent access from threads\n";
    cout << "               - Uses locks, atomics, or other synchronization\n";
    cout << "               - May use shared state (protected)\n\n";
    
    cout << "• ISR-SAFE:    Function safe to call from interrupt context\n";
    cout << "               - Must be re-entrant\n";
    cout << "               - Cannot use mutexes or blocking calls\n";
    cout << "               - Only atomics or disable interrupts\n\n";
    
    cout << "🔄 COMPARISON TABLE:\n";
    cout << "┌─────────────────────────┬────────────┬─────────────┬──────────┐\n";
    cout << "│ Code Type               │ Re-entrant │ Thread-safe │ ISR-safe │\n";
    cout << "├─────────────────────────┼────────────┼─────────────┼──────────┤\n";
    cout << "│ Local vars only         │     ✓      │      ✓      │    ✓     │\n";
    cout << "│ Static/global vars      │     ❌     │      ❌     │    ❌    │\n";
    cout << "│ Static + mutex          │     ❌     │      ✓      │    ❌    │\n";
    cout << "│ Static + atomic         │     ❌     │      ✓      │    ✓     │\n";
    cout << "│ Atomic operations       │     ✓      │      ✓      │    ✓     │\n";
    cout << "│ Disable interrupts      │     ✓      │      ✓      │    ✓     │\n";
    cout << "└─────────────────────────┴────────────┴─────────────┴──────────┘\n";
}

// ===================================================================
// PART 2: INTERRUPT SERVICE ROUTINE (ISR) PATTERNS
// ===================================================================

// Simulating hardware registers (volatile for memory-mapped I/O)
struct HardwareRegisters {
    volatile uint32_t status_reg;
    volatile uint32_t data_reg;
    volatile uint32_t control_reg;
};

// Global hardware registers (memory-mapped I/O)
volatile HardwareRegisters* const UART_REGS = reinterpret_cast<volatile HardwareRegisters*>(0x40000000);

// ❌ WRONG: ISR with non-atomic operations
namespace WrongISR {
    int rx_buffer[128];
    int rx_head = 0;  // PROBLEM: Not atomic, race condition!
    int rx_tail = 0;
    
    void uart_isr() {
        // ❌ WRONG: Non-atomic access to shared variables
        rx_buffer[rx_head] = 0; // UART_REGS->data_reg;
        rx_head = (rx_head + 1) % 128;
        // If main code accesses rx_head here, race condition!
    }
}

// ✅ CORRECT: ISR with atomic operations
namespace CorrectISR {
    array<uint8_t, 128> rx_buffer;
    atomic<uint32_t> rx_head{0};
    atomic<uint32_t> rx_tail{0};
    
    // ISR: Runs in interrupt context
    void uart_rx_isr() {
        // ✅ Atomic operations safe for ISR
        uint32_t next_head = (rx_head.load(memory_order_relaxed) + 1) % 128;
        
        // Check if buffer not full
        if (next_head != rx_tail.load(memory_order_acquire)) {
            rx_buffer[rx_head.load(memory_order_relaxed)] = 0; // UART_REGS->data_reg;
            rx_head.store(next_head, memory_order_release);
        }
    }
    
    // Main code: Runs in normal context
    bool read_byte(uint8_t& byte) {
        uint32_t current_tail = rx_tail.load(memory_order_relaxed);
        
        // Check if data available
        if (current_tail == rx_head.load(memory_order_acquire)) {
            return false;  // Buffer empty
        }
        
        byte = rx_buffer[current_tail];
        rx_tail.store((current_tail + 1) % 128, memory_order_release);
        return true;
    }
}

void demo_isr_patterns() {
    cout << "\n=== INTERRUPT SERVICE ROUTINE (ISR) PATTERNS ===\n\n";
    
    cout << "🎯 ISR REQUIREMENTS:\n";
    cout << "1. Fast execution (microseconds, not milliseconds)\n";
    cout << "2. No blocking operations (no malloc, no mutex, no I/O)\n";
    cout << "3. Minimal processing (defer to main loop if possible)\n";
    cout << "4. Use atomics or critical sections for shared data\n";
    cout << "5. Volatile keyword for hardware registers\n\n";
    
    cout << "✅ ISR-SAFE TECHNIQUES:\n";
    cout << "• std::atomic<T> with memory_order_relaxed (fastest)\n";
    cout << "• Lock-free circular buffers (producer-consumer)\n";
    cout << "• Set flags for main loop to process\n";
    cout << "• Critical sections (disable interrupts briefly)\n\n";
    
    cout << "❌ NEVER IN ISR:\n";
    cout << "• malloc/new (non-deterministic, can fail)\n";
    cout << "• mutex/lock_guard (blocking, can deadlock)\n";
    cout << "• printf/cout (I/O, slow, uses locks)\n";
    cout << "• Complex algorithms (keep ISR short!)\n\n";
    
    cout << "📊 TYPICAL ISR FLOW:\n";
    cout << "1. Read hardware register (volatile)\n";
    cout << "2. Store data in lock-free buffer (atomic)\n";
    cout << "3. Set flag for main loop (atomic)\n";
    cout << "4. Clear interrupt flag (volatile)\n";
    cout << "5. Return immediately\n";
}

// ===================================================================
// PART 3: CRITICAL SECTIONS FOR EMBEDDED SYSTEMS
// ===================================================================

// Simulating interrupt enable/disable (platform-specific)
namespace CriticalSection {
    // Platform abstraction for interrupt control
    class InterruptGuard {
        uint32_t saved_state;
        
    public:
        InterruptGuard() {
            // Save current interrupt state and disable
            // saved_state = __get_PRIMASK();
            // __disable_irq();
            saved_state = 0;  // Simulated
        }
        
        ~InterruptGuard() {
            // Restore interrupt state
            // __set_PRIMASK(saved_state);
        }
        
        // Non-copyable, non-movable (RAII)
        InterruptGuard(const InterruptGuard&) = delete;
        InterruptGuard& operator=(const InterruptGuard&) = delete;
    };
    
    // Shared resource accessed by ISR and main code
    struct SensorData {
        uint32_t timestamp;
        int16_t temperature;
        int16_t pressure;
    };
    
    SensorData latest_reading{0, 0, 0};
    
    // ISR: Updates sensor data
    void sensor_isr() {
        // No critical section needed in ISR (it can't be interrupted by itself)
        latest_reading.timestamp = 12345; // Get_system_tick();
        latest_reading.temperature = 25;  // Read_ADC();
        latest_reading.pressure = 1013;   // Read_ADC();
    }
    
    // Main code: Reads sensor data
    SensorData get_sensor_reading() {
        // ✅ CRITICAL SECTION: Prevent ISR from modifying data while we read
        InterruptGuard guard;  // Disables interrupts
        SensorData data = latest_reading;  // Atomic copy
        return data;
        // guard destructor re-enables interrupts
    }
}

void demo_critical_sections() {
    cout << "\n=== CRITICAL SECTIONS (EMBEDDED SYSTEMS) ===\n\n";
    
    cout << "🔒 CRITICAL SECTION = Disable interrupts temporarily\n\n";
    
    cout << "WHEN TO USE:\n";
    cout << "• Accessing multi-word data shared with ISR\n";
    cout << "• Updating complex data structures\n";
    cout << "• When atomics are insufficient (struct, arrays)\n\n";
    
    cout << "⏱️  KEEP CRITICAL SECTIONS SHORT:\n";
    cout << "• Bad:  10ms → Misses interrupts!\n";
    cout << "• Good: <10μs → Minimal interrupt latency\n\n";
    
    cout << "✅ EXAMPLE USAGE:\n";
    cout << "  InterruptGuard guard;  // Disable interrupts\n";
    cout << "  // Access shared data here\n";
    cout << "  // Interrupts automatically re-enabled on scope exit\n\n";
    
    cout << "🎯 ALTERNATIVE: Use atomics when possible\n";
    cout << "• Single int/pointer: std::atomic<T>\n";
    cout << "• Multiple values: Critical section or double-buffering\n";
}

// ===================================================================
// PART 4: LOCK-FREE CIRCULAR BUFFER (SPSC)
// ===================================================================

// Single Producer Single Consumer (SPSC) circular buffer
// ISR-safe, no locks needed
template<typename T, size_t Size>
class LockFreeRingBuffer {
    array<T, Size> buffer;
    atomic<size_t> write_idx{0};
    atomic<size_t> read_idx{0};
    
public:
    // Producer (e.g., ISR)
    bool push(const T& item) {
        size_t current_write = write_idx.load(memory_order_relaxed);
        size_t next_write = (current_write + 1) % Size;
        
        // Check if buffer full
        if (next_write == read_idx.load(memory_order_acquire)) {
            return false;  // Buffer full
        }
        
        buffer[current_write] = item;
        write_idx.store(next_write, memory_order_release);
        return true;
    }
    
    // Consumer (e.g., main loop)
    bool pop(T& item) {
        size_t current_read = read_idx.load(memory_order_relaxed);
        
        // Check if buffer empty
        if (current_read == write_idx.load(memory_order_acquire)) {
            return false;  // Buffer empty
        }
        
        item = buffer[current_read];
        read_idx.store((current_read + 1) % Size, memory_order_release);
        return true;
    }
    
    bool empty() const {
        return read_idx.load(memory_order_acquire) == 
               write_idx.load(memory_order_acquire);
    }
    
    size_t size() const {
        size_t w = write_idx.load(memory_order_acquire);
        size_t r = read_idx.load(memory_order_acquire);
        return (w >= r) ? (w - r) : (Size - r + w);
    }
};

void demo_lockfree_buffer() {
    cout << "\n=== LOCK-FREE CIRCULAR BUFFER (ISR-SAFE) ===\n\n";
    
    LockFreeRingBuffer<uint8_t, 16> uart_buffer;
    
    // Simulate ISR pushing data
    cout << "Simulating ISR pushing data...\n";
    for (uint8_t i = 0; i < 10; ++i) {
        if (uart_buffer.push(i)) {
            cout << "  ISR pushed: " << static_cast<int>(i) << "\n";
        }
    }
    
    // Main loop consuming data
    cout << "\nMain loop consuming data...\n";
    uint8_t byte;
    while (uart_buffer.pop(byte)) {
        cout << "  Main read: " << static_cast<int>(byte) << "\n";
    }
    
    cout << "\n✅ LOCK-FREE SPSC BUFFER:\n";
    cout << "• No locks, no mutexes → ISR-safe\n";
    cout << "• Single Producer, Single Consumer only\n";
    cout << "• Uses atomic indices with memory ordering\n";
    cout << "• Fixed size (no dynamic allocation)\n";
    cout << "• O(1) push and pop operations\n\n";
    
    cout << "🎯 MEMORY ORDERING:\n";
    cout << "• memory_order_relaxed: Local operations (fast)\n";
    cout << "• memory_order_acquire: Read operations (sync)\n";
    cout << "• memory_order_release: Write operations (sync)\n";
}

// ===================================================================
// PART 5: VOLATILE KEYWORD IN EMBEDDED SYSTEMS
// ===================================================================

void demo_volatile_keyword() {
    cout << "\n=== VOLATILE KEYWORD IN EMBEDDED SYSTEMS ===\n\n";
    
    cout << "🔧 WHEN TO USE VOLATILE:\n";
    cout << "1. Memory-mapped hardware registers\n";
    cout << "2. Variables modified by ISR (better: use atomic)\n";
    cout << "3. Variables used in signal handlers\n";
    cout << "4. Multi-threaded communication (better: use atomic)\n\n";
    
    cout << "✅ CORRECT USAGE:\n";
    cout << "  volatile uint32_t* const GPIO_OUTPUT = (volatile uint32_t*)0x40020000;\n";
    cout << "  *GPIO_OUTPUT = 0xFF;  // Compiler won't optimize away\n\n";
    
    cout << "  volatile bool isr_flag = false;\n";
    cout << "  void isr() { isr_flag = true; }\n";
    cout << "  while (!isr_flag) { }  // Won't be optimized to infinite loop\n\n";
    
    cout << "❌ VOLATILE IS NOT:\n";
    cout << "• NOT atomic (use std::atomic for thread safety)\n";
    cout << "• NOT a memory barrier (use std::atomic with memory_order)\n";
    cout << "• NOT sufficient for thread synchronization\n\n";
    
    cout << "🎯 MODERN APPROACH:\n";
    cout << "• Hardware registers: volatile (required)\n";
    cout << "• ISR flags: std::atomic<bool> (better than volatile)\n";
    cout << "• Thread communication: std::atomic or mutex\n";
}

// ===================================================================
// PART 6: DOUBLE BUFFERING TECHNIQUE
// ===================================================================

template<typename T>
class DoubleBuffer {
    T buffers[2];
    atomic<int> read_idx{0};
    atomic<int> write_idx{1};
    
public:
    // Producer: Write to write buffer
    T& get_write_buffer() {
        return buffers[write_idx.load(memory_order_relaxed)];
    }
    
    // Producer: Swap buffers (atomic operation)
    void swap_buffers() {
        int old_write = write_idx.load(memory_order_relaxed);
        int old_read = read_idx.load(memory_order_relaxed);
        
        write_idx.store(old_read, memory_order_release);
        read_idx.store(old_write, memory_order_release);
    }
    
    // Consumer: Read from read buffer
    const T& get_read_buffer() const {
        return buffers[read_idx.load(memory_order_acquire)];
    }
};

struct SensorFrame {
    uint32_t timestamp;
    array<int16_t, 16> samples;
};

void demo_double_buffering() {
    cout << "\n=== DOUBLE BUFFERING TECHNIQUE ===\n\n";
    
    DoubleBuffer<SensorFrame> sensor_buffers;
    
    cout << "✅ USE CASES:\n";
    cout << "• ISR collects data in one buffer while main processes other\n";
    cout << "• Graphics: Render to back buffer while displaying front buffer\n";
    cout << "• Audio: Fill one buffer while DMA reads from other\n\n";
    
    cout << "🔄 WORKFLOW:\n";
    cout << "1. ISR writes to write_buffer\n";
    cout << "2. When complete, swap buffers atomically\n";
    cout << "3. Main loop processes read_buffer\n";
    cout << "4. Repeat\n\n";
    
    cout << "✅ BENEFITS:\n";
    cout << "• No locks needed (lock-free)\n";
    cout << "• ISR never blocks\n";
    cout << "• Main code always has complete frame\n";
    cout << "• Zero-copy (just swap pointers)\n";
}

// ===================================================================
// PART 7: ATOMIC FLAG PATTERNS
// ===================================================================

void demo_atomic_flags() {
    cout << "\n=== ATOMIC FLAG PATTERNS ===\n\n";
    
    cout << "🚩 COMMON FLAG PATTERNS:\n\n";
    
    cout << "1. ISR NOTIFICATION:\n";
    cout << "   atomic<bool> data_ready{false};\n";
    cout << "   void isr() { data_ready.store(true, memory_order_release); }\n";
    cout << "   void main() {\n";
    cout << "     if (data_ready.load(memory_order_acquire)) {\n";
    cout << "       process_data();\n";
    cout << "       data_ready.store(false, memory_order_release);\n";
    cout << "     }\n";
    cout << "   }\n\n";
    
    cout << "2. ATOMIC COUNTER:\n";
    cout << "   atomic<uint32_t> error_count{0};\n";
    cout << "   void isr() { error_count.fetch_add(1, memory_order_relaxed); }\n\n";
    
    cout << "3. STATE MACHINE:\n";
    cout << "   enum class State { IDLE, BUSY, ERROR };\n";
    cout << "   atomic<State> current_state{State::IDLE};\n";
    cout << "   void isr() { current_state.store(State::BUSY, memory_order_release); }\n\n";
    
    cout << "✅ MEMORY ORDERING GUIDE:\n";
    cout << "• relaxed: Independent operations (counters, stats)\n";
    cout << "• acquire: Reading synchronized data\n";
    cout << "• release: Writing synchronized data\n";
    cout << "• seq_cst: When in doubt (safest, slower)\n";
}

// ===================================================================
// PART 8: COMMON PITFALLS IN EMBEDDED SYSTEMS
// ===================================================================

void demo_common_pitfalls() {
    cout << "\n=== COMMON PITFALLS IN EMBEDDED SYSTEMS ===\n\n";
    
    cout << "❌ PITFALL 1: Printf in ISR\n";
    cout << "   Problem: printf uses locks, can deadlock\n";
    cout << "   Solution: Set flag in ISR, print in main loop\n\n";
    
    cout << "❌ PITFALL 2: malloc/new in ISR\n";
    cout << "   Problem: Non-deterministic, can fail, not re-entrant\n";
    cout << "   Solution: Pre-allocate buffers (static or at startup)\n\n";
    
    cout << "❌ PITFALL 3: Long-running ISR\n";
    cout << "   Problem: Blocks other interrupts, missed events\n";
    cout << "   Solution: Set flag, defer processing to main loop\n\n";
    
    cout << "❌ PITFALL 4: Non-atomic multi-byte access\n";
    cout << "   Problem: ISR reads while main writes → corrupted data\n";
    cout << "   Solution: Use atomic<T>, critical section, or double-buffer\n\n";
    
    cout << "❌ PITFALL 5: Forgetting volatile for MMIO\n";
    cout << "   Problem: Compiler optimizes away register access\n";
    cout << "   Solution: Always use volatile for hardware registers\n\n";
    
    cout << "❌ PITFALL 6: Priority inversion with interrupts\n";
    cout << "   Problem: High-priority ISR waits for low-priority task\n";
    cout << "   Solution: Use priority inheritance or lock-free structures\n\n";
    
    cout << "❌ PITFALL 7: Stack overflow in ISR\n";
    cout << "   Problem: Large local variables in ISR\n";
    cout << "   Solution: Keep ISR stack usage minimal (<100 bytes)\n";
}

// ===================================================================
// PART 9: DECISION GUIDE
// ===================================================================

void demo_decision_guide() {
    cout << "\n=== DECISION GUIDE: CHOOSING SYNCHRONIZATION ===\n\n";
    
    cout << "📋 SINGLE INTEGER/BOOLEAN:\n";
    cout << "   → Use std::atomic<T>\n";
    cout << "   Example: atomic<bool> flag, atomic<int> counter\n\n";
    
    cout << "📋 SMALL STRUCT (≤8 bytes on 64-bit):\n";
    cout << "   → Use std::atomic<T> if supported\n";
    cout << "   → Otherwise: Critical section or double-buffer\n\n";
    
    cout << "📋 LARGE STRUCT OR ARRAY:\n";
    cout << "   → Critical section (brief!)\n";
    cout << "   → Or double-buffering (preferred)\n\n";
    
    cout << "📋 CIRCULAR BUFFER (ISR → Main):\n";
    cout << "   → Lock-free SPSC ring buffer\n\n";
    
    cout << "📋 COMPLEX DATA PROCESSING:\n";
    cout << "   → Flag in ISR, process in main loop\n\n";
    
    cout << "📋 RTOS WITH THREADS:\n";
    cout << "   → Mutex for long critical sections\n";
    cout << "   → Semaphore for task notification\n";
    cout << "   → Message queue for data passing\n\n";
    
    cout << "⚡ PERFORMANCE HIERARCHY (fastest → slowest):\n";
    cout << "1. Atomic with relaxed ordering\n";
    cout << "2. Atomic with acquire/release\n";
    cout << "3. Critical section (disable interrupts)\n";
    cout << "4. RTOS mutex/semaphore\n";
    cout << "5. Lock-based structures\n";
}

// ===================================================================
// PART 10: PRACTICAL EXAMPLE - UART DRIVER
// ===================================================================

class UartDriver {
    static constexpr size_t BUFFER_SIZE = 256;
    
    // Lock-free buffers for ISR ↔ main communication
    LockFreeRingBuffer<uint8_t, BUFFER_SIZE> rx_buffer;
    LockFreeRingBuffer<uint8_t, BUFFER_SIZE> tx_buffer;
    
    // Atomic flags
    atomic<bool> tx_in_progress{false};
    atomic<uint32_t> rx_overflow_count{0};
    atomic<uint32_t> tx_complete_count{0};
    
public:
    // Called from ISR: RX interrupt
    void rx_isr() {
        // Read from hardware (volatile access)
        // uint8_t data = UART_REGS->data_reg;
        uint8_t data = 'A';  // Simulated
        
        if (!rx_buffer.push(data)) {
            rx_overflow_count.fetch_add(1, memory_order_relaxed);
        }
    }
    
    // Called from ISR: TX interrupt
    void tx_isr() {
        uint8_t data;
        if (tx_buffer.pop(data)) {
            // Write to hardware (volatile access)
            // UART_REGS->data_reg = data;
        } else {
            tx_in_progress.store(false, memory_order_release);
            tx_complete_count.fetch_add(1, memory_order_relaxed);
        }
    }
    
    // Called from main: Non-blocking read
    bool read(uint8_t& byte) {
        return rx_buffer.pop(byte);
    }
    
    // Called from main: Non-blocking write
    bool write(uint8_t byte) {
        if (!tx_buffer.push(byte)) {
            return false;  // Buffer full
        }
        
        // Kickstart TX if not already running
        if (!tx_in_progress.exchange(true, memory_order_acquire)) {
            // Trigger TX interrupt
            // UART_REGS->control_reg |= TX_ENABLE;
        }
        return true;
    }
    
    uint32_t get_rx_overflow_count() const {
        return rx_overflow_count.load(memory_order_relaxed);
    }
};

void demo_uart_driver() {
    cout << "\n=== PRACTICAL EXAMPLE: ISR-SAFE UART DRIVER ===\n\n";
    
    UartDriver uart;
    
    cout << "✅ DESIGN FEATURES:\n";
    cout << "• Lock-free ring buffers (ISR-safe)\n";
    cout << "• Non-blocking read/write from main code\n";
    cout << "• Atomic flags for state management\n";
    cout << "• Overflow detection (never loses data silently)\n\n";
    
    cout << "🔄 DATA FLOW:\n";
    cout << "RX: Hardware → ISR → rx_buffer → main code\n";
    cout << "TX: main code → tx_buffer → ISR → Hardware\n\n";
    
    cout << "⚡ KEY PRINCIPLES:\n";
    cout << "1. ISR does minimal work (read/write + buffer)\n";
    cout << "2. Main code processes data at leisure\n";
    cout << "3. No locks needed (lock-free design)\n";
    cout << "4. Bounded memory usage (fixed buffers)\n";
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    cout << "\n================================================================\n";
    cout << "  THREAD-SAFETY & RE-ENTRANCY FOR EMBEDDED/FIRMWARE SYSTEMS\n";
    cout << "================================================================\n";
    
    demo_reentrant_vs_threadsafe();
    demo_isr_patterns();
    demo_critical_sections();
    demo_lockfree_buffer();
    demo_volatile_keyword();
    demo_double_buffering();
    demo_atomic_flags();
    demo_common_pitfalls();
    demo_decision_guide();
    demo_uart_driver();
    
    cout << "\n================================================================\n";
    cout << "  SUMMARY: EMBEDDED SYSTEMS BEST PRACTICES\n";
    cout << "================================================================\n\n";
    
    cout << "🎯 FOR BARE-METAL / ISR CODE:\n";
    cout << "1. Use std::atomic<T> for simple shared variables\n";
    cout << "2. Use lock-free SPSC buffers for data passing\n";
    cout << "3. Keep ISRs short (<10μs if possible)\n";
    cout << "4. Use volatile for memory-mapped I/O only\n";
    cout << "5. Critical sections: Brief and rare\n\n";
    
    cout << "🎯 FOR RTOS / THREADED CODE:\n";
    cout << "1. Mutexes for long critical sections\n";
    cout << "2. Semaphores for task notification\n";
    cout << "3. Message queues for data transfer\n";
    cout << "4. Still use atomics for flags/counters\n\n";
    
    cout << "⚠️  NEVER IN ISR:\n";
    cout << "• malloc/new/delete\n";
    cout << "• mutex/lock_guard\n";
    cout << "• printf/cout\n";
    cout << "• Blocking operations\n";
    cout << "• Long-running algorithms\n\n";
    
    cout << "✅ ALWAYS IN ISR:\n";
    cout << "• Fast execution (microseconds)\n";
    cout << "• Atomic operations\n";
    cout << "• Volatile for hardware access\n";
    cout << "• Defer complex work to main loop\n\n";
    
    cout << "📚 FURTHER READING:\n";
    cout << "• ISO 26262 (Automotive functional safety)\n";
    cout << "• MISRA C++ (Safety-critical guidelines)\n";
    cout << "• ARM Cortex-M interrupt handling\n";
    cout << "• Real-Time Systems by Jane W. S. Liu\n";
    
    cout << "\n================================================================\n\n";
    
    return 0;
}
