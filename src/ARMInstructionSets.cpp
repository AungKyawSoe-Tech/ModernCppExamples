#include <iostream>
#include <cstdint>
#include <cstring>
#include <array>

// ===================================================================
// ARM INSTRUCTION SETS: ARM, THUMB, THUMB-2
// ===================================================================
// This example discusses instruction set modes, alignment issues,
// and compiler switches for ARM Cortex-M processors.
//
// IMPORTANT: This is a DEMONSTRATION for understanding concepts.
// The actual crashes occur on ARM hardware, not x86/x64.
// Compiler flags discussed apply to ARM cross-compilers (arm-none-eabi-gcc).
// ===================================================================

// ===================================================================
// 1. ARM INSTRUCTION SET OVERVIEW
// ===================================================================

void explain_instruction_sets() {
    std::cout << "\n=== 1. ARM INSTRUCTION SETS OVERVIEW ===" << std::endl;
    
    std::cout << "\n📚 THREE INSTRUCTION SET MODES:" << std::endl;
    
    std::cout << "\n1. ARM (A32) - 32-bit instructions:" << std::endl;
    std::cout << "   • Original ARM instruction set" << std::endl;
    std::cout << "   • Fixed 32-bit instruction size" << std::endl;
    std::cout << "   • Most powerful, but code size larger" << std::endl;
    std::cout << "   • Used in: ARM7, ARM9, ARM11, Cortex-A" << std::endl;
    std::cout << "   • Example instruction: ADD r0, r1, r2  (32 bits)" << std::endl;
    
    std::cout << "\n2. THUMB (T16) - 16-bit instructions:" << std::endl;
    std::cout << "   • Compact 16-bit instruction subset" << std::endl;
    std::cout << "   • ~30% smaller code size vs ARM" << std::endl;
    std::cout << "   • Limited register access (r0-r7)" << std::endl;
    std::cout << "   • Used in: ARM7TDMI, ARM9, early Cortex-M0" << std::endl;
    std::cout << "   • Example: ADD r0, r1  (16 bits, 2-operand only)" << std::endl;
    
    std::cout << "\n3. THUMB-2 (T32) - Mixed 16/32-bit:" << std::endl;
    std::cout << "   • Mix of 16-bit and 32-bit instructions" << std::endl;
    std::cout << "   • Best of both worlds: compact + powerful" << std::endl;
    std::cout << "   • Full register access, conditional execution" << std::endl;
    std::cout << "   • Used in: Cortex-M3, M4, M7, M33 (modern MCUs)" << std::endl;
    std::cout << "   • Example: ADD r0, r1, r2 (16 or 32 bits as needed)" << std::endl;
    
    std::cout << "\n🎯 MODERN CORTEX-M PROCESSORS:" << std::endl;
    std::cout << "   • Cortex-M0/M0+: Thumb + subset of Thumb-2" << std::endl;
    std::cout << "   • Cortex-M3/M4/M7: Thumb-2 only (no ARM mode)" << std::endl;
    std::cout << "   • Cortex-M33/M55: Thumb-2 + DSP + optional FPU" << std::endl;
}

// ===================================================================
// 2. ALIGNMENT ISSUES - THE MAIN CAUSE OF CRASHES
// ===================================================================

// Simulated aligned data structure
struct __attribute__((aligned(4))) AlignedData {
    uint32_t value1;
    uint32_t value2;
    uint32_t value3;
};

// Potentially misaligned data
struct UnalignedData {
    uint8_t byte;      // 1 byte
    uint32_t value;    // Should be 4-byte aligned, but might not be!
    uint16_t halfword; // 2 bytes
};

void explain_alignment_issues() {
    std::cout << "\n=== 2. ALIGNMENT ISSUES (MAIN CRASH CAUSE) ===" << std::endl;
    
    std::cout << "\n⚠️ ARM ALIGNMENT REQUIREMENTS:" << std::endl;
    std::cout << "   • uint8_t  (byte):      1-byte aligned (any address)" << std::endl;
    std::cout << "   • uint16_t (halfword):  2-byte aligned (address % 2 == 0)" << std::endl;
    std::cout << "   • uint32_t (word):      4-byte aligned (address % 4 == 0)" << std::endl;
    std::cout << "   • uint64_t (doubleword): 8-byte aligned (address % 8 == 0)" << std::endl;
    
    std::cout << "\n❌ WHAT HAPPENS ON MISALIGNED ACCESS:" << std::endl;
    std::cout << "   • Cortex-M0/M0+: HARDFAULT (crash)" << std::endl;
    std::cout << "   • Cortex-M3/M4/M7: May work BUT slower (extra cycles)" << std::endl;
    std::cout << "   • Depends on UNALIGN_TRP bit in CCR register" << std::endl;
    
    // Demonstrate alignment
    std::cout << "\n📊 ALIGNMENT EXAMPLES:" << std::endl;
    
    AlignedData aligned;
    std::cout << "  AlignedData size:      " << sizeof(AlignedData) << " bytes" << std::endl;
    std::cout << "  Address of value1:     " << reinterpret_cast<uintptr_t>(&aligned.value1) 
              << " (mod 4 = " << (reinterpret_cast<uintptr_t>(&aligned.value1) % 4) << ")" << std::endl;
    
    UnalignedData unaligned;
    std::cout << "\n  UnalignedData size:    " << sizeof(UnalignedData) << " bytes" << std::endl;
    std::cout << "  Address of byte:       " << reinterpret_cast<uintptr_t>(&unaligned.byte) << std::endl;
    std::cout << "  Address of value:      " << reinterpret_cast<uintptr_t>(&unaligned.value) 
              << " (mod 4 = " << (reinterpret_cast<uintptr_t>(&unaligned.value) % 4) << ")" << std::endl;
    
    std::cout << "\n⚠️ PROBLEM: If unaligned.value is at address 0x20000001:" << std::endl;
    std::cout << "   • ARM instruction: LDR r0, [r1]  (load word from unaligned address)" << std::endl;
    std::cout << "   • Result: HARDFAULT on Cortex-M0, silent corruption on some others" << std::endl;
}

// ===================================================================
// 3. INTERWORKING ISSUES - STATE SWITCHING
// ===================================================================

void explain_interworking() {
    std::cout << "\n=== 3. INTERWORKING ISSUES (STATE SWITCHING) ===" << std::endl;
    
    std::cout << "\n🔄 HOW PROCESSOR KNOWS WHICH MODE:" << std::endl;
    std::cout << "   • Function address LSB (Least Significant Bit):" << std::endl;
    std::cout << "     - LSB = 0: ARM mode (A32 instructions)" << std::endl;
    std::cout << "     - LSB = 1: Thumb/Thumb-2 mode (T16/T32 instructions)" << std::endl;
    std::cout << "   • BX/BLX instructions switch modes automatically" << std::endl;
    
    std::cout << "\n❌ CRASH SCENARIOS:" << std::endl;
    std::cout << "\n  1. WRONG FUNCTION ADDRESS:" << std::endl;
    std::cout << "     // C code" << std::endl;
    std::cout << "     void my_function() { /* compiled as Thumb */ }" << std::endl;
    std::cout << "     " << std::endl;
    std::cout << "     // Assembly calling without setting LSB" << std::endl;
    std::cout << "     LDR r0, =my_function  ; Address without LSB=1" << std::endl;
    std::cout << "     BLX r0                 ; ❌ Tries to execute as ARM!" << std::endl;
    std::cout << "     " << std::endl;
    std::cout << "     ✓ CORRECT:" << std::endl;
    std::cout << "     LDR r0, =my_function+1  ; Set LSB to indicate Thumb" << std::endl;
    std::cout << "     BLX r0                   ; ✓ Correctly switches to Thumb" << std::endl;
    
    std::cout << "\n  2. INTERRUPT SERVICE ROUTINES (ISR):" << std::endl;
    std::cout << "     • Some ARM7/ARM9 enter exceptions in ARM mode" << std::endl;
    std::cout << "     • If ISR compiled as Thumb: CRASH!" << std::endl;
    std::cout << "     • Solution: Force ISR to ARM mode or ensure processor supports Thumb ISRs" << std::endl;
    
    std::cout << "\n  3. ASSEMBLY CODE MISMATCH:" << std::endl;
    std::cout << "     // File: startup.s (ARM mode)" << std::endl;
    std::cout << "     .arm" << std::endl;
    std::cout << "     Reset_Handler:" << std::endl;
    std::cout << "         LDR sp, =_estack" << std::endl;
    std::cout << "         BL main           ; ❌ Calls main as ARM!" << std::endl;
    std::cout << "     " << std::endl;
    std::cout << "     ✓ CORRECT:" << std::endl;
    std::cout << "     .thumb              ; Switch to Thumb mode" << std::endl;
    std::cout << "     Reset_Handler:" << std::endl;
    std::cout << "         LDR sp, =_estack" << std::endl;
    std::cout << "         BLX main          ; ✓ Properly switches to main's mode" << std::endl;
}

// ===================================================================
// 4. COMPILER SWITCHES AND FLAGS
// ===================================================================

void explain_compiler_switches() {
    std::cout << "\n=== 4. COMPILER SWITCHES FOR ARM ===" << std::endl;
    
    std::cout << "\n🔧 ARM-NONE-EABI-GCC FLAGS:" << std::endl;
    
    std::cout << "\n1. INSTRUCTION SET MODE:" << std::endl;
    std::cout << "   -marm" << std::endl;
    std::cout << "     • Generate ARM (A32) instructions" << std::endl;
    std::cout << "     • Larger code size, faster execution" << std::endl;
    std::cout << "     • Use for: ARM7, ARM9, Cortex-A" << std::endl;
    std::cout << "     • Example: arm-none-eabi-gcc -marm -mcpu=arm7tdmi main.c" << std::endl;
    
    std::cout << "\n   -mthumb" << std::endl;
    std::cout << "     • Generate Thumb/Thumb-2 instructions" << std::endl;
    std::cout << "     • Smaller code size, good performance" << std::endl;
    std::cout << "     • DEFAULT for Cortex-M (they only support Thumb-2)" << std::endl;
    std::cout << "     • Example: arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 main.c" << std::endl;
    
    std::cout << "\n2. CPU/ARCHITECTURE:" << std::endl;
    std::cout << "   -mcpu=cortex-m0      # Cortex-M0 (Thumb subset)" << std::endl;
    std::cout << "   -mcpu=cortex-m3      # Cortex-M3 (Thumb-2)" << std::endl;
    std::cout << "   -mcpu=cortex-m4      # Cortex-M4 (Thumb-2 + DSP)" << std::endl;
    std::cout << "   -mcpu=cortex-m7      # Cortex-M7 (Thumb-2 + DSP + FPU)" << std::endl;
    
    std::cout << "\n3. ALIGNMENT OPTIONS:" << std::endl;
    std::cout << "   -mno-unaligned-access" << std::endl;
    std::cout << "     • Disable unaligned memory access" << std::endl;
    std::cout << "     • Compiler generates multi-instruction sequences" << std::endl;
    std::cout << "     • Slower but safer on Cortex-M0" << std::endl;
    std::cout << "     • Example: arm-none-eabi-gcc -mthumb -mno-unaligned-access" << std::endl;
    
    std::cout << "\n   -munaligned-access (default on M3/M4/M7)" << std::endl;
    std::cout << "     • Allow unaligned memory access" << std::endl;
    std::cout << "     • Hardware handles it (with performance penalty)" << std::endl;
    std::cout << "     • Use for: Cortex-M3 and above" << std::endl;
    
    std::cout << "\n4. INTERWORKING:" << std::endl;
    std::cout << "   -mthumb-interwork" << std::endl;
    std::cout << "     • Generate code that can call between ARM and Thumb" << std::endl;
    std::cout << "     • Required for mixed ARM/Thumb projects" << std::endl;
    std::cout << "     • Not needed for Cortex-M (Thumb-2 only)" << std::endl;
}

// ===================================================================
// 5. PRACTICAL EXAMPLE: ALIGNMENT CRASH SIMULATION
// ===================================================================

// This simulates what happens on ARM hardware
void demonstrate_alignment_crash() {
    std::cout << "\n=== 5. ALIGNMENT CRASH SIMULATION ===" << std::endl;
    
    std::cout << "\n⚠️ NOTE: On x86/x64, unaligned access works (slower)" << std::endl;
    std::cout << "   On ARM Cortex-M0: Would cause HARDFAULT" << std::endl;
    
    // Create a buffer with known alignment
    alignas(4) uint8_t buffer[16];
    
    // Write data at aligned address (safe)
    std::cout << "\n✓ ALIGNED ACCESS (address % 4 == 0):" << std::endl;
    uint32_t* aligned_ptr = reinterpret_cast<uint32_t*>(&buffer[0]);
    *aligned_ptr = 0x12345678;
    std::cout << "   Address: " << reinterpret_cast<uintptr_t>(aligned_ptr) 
              << " (aligned)" << std::endl;
    std::cout << "   Value written: 0x" << std::hex << *aligned_ptr << std::dec << std::endl;
    std::cout << "   Result: ✓ SUCCESS (no crash on ARM)" << std::endl;
    
    // Try unaligned access (dangerous on ARM!)
    std::cout << "\n❌ UNALIGNED ACCESS (address % 4 != 0):" << std::endl;
    uint32_t* unaligned_ptr = reinterpret_cast<uint32_t*>(&buffer[1]);
    std::cout << "   Address: " << reinterpret_cast<uintptr_t>(unaligned_ptr) 
              << " (unaligned, offset by 1)" << std::endl;
    
    // On x86/x64 this works, on ARM Cortex-M0 it would HARDFAULT
    std::cout << "   Attempting to write uint32_t at unaligned address..." << std::endl;
    *unaligned_ptr = 0xAABBCCDD;  // Works on x86, HARDFAULT on Cortex-M0!
    std::cout << "   Result on x86: ✓ Works (slow)" << std::endl;
    std::cout << "   Result on ARM Cortex-M0: ❌ HARDFAULT! (crash)" << std::endl;
    std::cout << "   Result on ARM Cortex-M4: ⚠️ Works but slow (multiple bus cycles)" << std::endl;
}

// ===================================================================
// 6. SOLUTION: PORTABLE UNALIGNED ACCESS
// ===================================================================

// ✓ SAFE: Portable unaligned read (works on all platforms)
uint32_t read_uint32_unaligned(const uint8_t* ptr) {
    uint32_t value;
    std::memcpy(&value, ptr, sizeof(value));  // Compiler optimizes this
    return value;
}

// ✓ SAFE: Portable unaligned write
void write_uint32_unaligned(uint8_t* ptr, uint32_t value) {
    std::memcpy(ptr, &value, sizeof(value));
}

// ❌ UNSAFE: Direct cast (crashes on unaligned ARM)
uint32_t read_uint32_unsafe(const uint8_t* ptr) {
    return *reinterpret_cast<const uint32_t*>(ptr);  // HARDFAULT if unaligned!
}

void demonstrate_portable_unaligned() {
    std::cout << "\n=== 6. PORTABLE UNALIGNED ACCESS ===" << std::endl;
    
    alignas(4) uint8_t buffer[16] = {};
    
    std::cout << "\n✓ SAFE METHOD (using memcpy):" << std::endl;
    std::cout << "   uint32_t read_uint32_unaligned(const uint8_t* ptr) {" << std::endl;
    std::cout << "       uint32_t value;" << std::endl;
    std::cout << "       std::memcpy(&value, ptr, sizeof(value));" << std::endl;
    std::cout << "       return value;" << std::endl;
    std::cout << "   }" << std::endl;
    
    write_uint32_unaligned(&buffer[1], 0x12345678);
    uint32_t safe_value = read_uint32_unaligned(&buffer[1]);
    std::cout << "\n   Written to unaligned address using memcpy" << std::endl;
    std::cout << "   Value: 0x" << std::hex << safe_value << std::dec << std::endl;
    std::cout << "   ✓ Works on ALL platforms (x86, ARM, RISC-V)" << std::endl;
    std::cout << "   ✓ Compiler optimizes memcpy to efficient code" << std::endl;
    
    std::cout << "\n❌ UNSAFE METHOD (direct cast):" << std::endl;
    std::cout << "   uint32_t value = *reinterpret_cast<uint32_t*>(ptr);" << std::endl;
    std::cout << "   ❌ HARDFAULT on ARM Cortex-M0 if unaligned!" << std::endl;
}

// ===================================================================
// 7. ASSEMBLY DIRECTIVES FOR MIXED CODE
// ===================================================================

void explain_assembly_directives() {
    std::cout << "\n=== 7. ASSEMBLY DIRECTIVES ===" << std::endl;
    
    std::cout << "\n📝 ARM ASSEMBLY DIRECTIVES:" << std::endl;
    
    std::cout << "\n1. SET INSTRUCTION MODE:" << std::endl;
    std::cout << "   .arm              ; Switch to ARM (A32) mode" << std::endl;
    std::cout << "   .thumb            ; Switch to Thumb/Thumb-2 (T16/T32) mode" << std::endl;
    
    std::cout << "\n2. MARK THUMB FUNCTIONS:" << std::endl;
    std::cout << "   .thumb_func       ; Next symbol is a Thumb function" << std::endl;
    std::cout << "   " << std::endl;
    std::cout << "   Example:" << std::endl;
    std::cout << "   .thumb" << std::endl;
    std::cout << "   .thumb_func" << std::endl;
    std::cout << "   my_function:" << std::endl;
    std::cout << "       PUSH {lr}" << std::endl;
    std::cout << "       ; ... function code ..." << std::endl;
    std::cout << "       POP {pc}" << std::endl;
    
    std::cout << "\n3. ALIGNMENT DIRECTIVES:" << std::endl;
    std::cout << "   .align 2          ; Align to 2^2 = 4 bytes (word)" << std::endl;
    std::cout << "   .align 3          ; Align to 2^3 = 8 bytes (doubleword)" << std::endl;
    std::cout << "   " << std::endl;
    std::cout << "   Example:" << std::endl;
    std::cout << "   .align 2          ; Force 4-byte alignment" << std::endl;
    std::cout << "   my_data:" << std::endl;
    std::cout << "       .word 0x12345678" << std::endl;
}

// ===================================================================
// 8. REAL-WORLD EXAMPLE: PROTOCOL PARSING
// ===================================================================

// Common scenario: Parsing network packets with misaligned fields
struct __attribute__((packed)) NetworkPacket {
    uint8_t  header;      // 1 byte
    uint32_t timestamp;   // 4 bytes (might be misaligned!)
    uint16_t length;      // 2 bytes
    uint32_t crc;         // 4 bytes (might be misaligned!)
};

void demonstrate_real_world_scenario() {
    std::cout << "\n=== 8. REAL-WORLD: PROTOCOL PARSING ===" << std::endl;
    
    std::cout << "\n📦 SCENARIO: Parsing network packet on Cortex-M4" << std::endl;
    
    // Simulate receiving a packet
    alignas(4) uint8_t rx_buffer[16] = {
        0xAA,                           // header (1 byte)
        0x78, 0x56, 0x34, 0x12,        // timestamp (4 bytes, at offset 1 = UNALIGNED!)
        0x10, 0x00,                     // length (2 bytes)
        0xDD, 0xCC, 0xBB, 0xAA         // crc (4 bytes)
    };
    
    std::cout << "\n❌ WRONG WAY (direct struct cast):" << std::endl;
    std::cout << "   NetworkPacket* pkt = (NetworkPacket*)rx_buffer;" << std::endl;
    std::cout << "   uint32_t ts = pkt->timestamp;  // ❌ Unaligned access!" << std::endl;
    std::cout << "   " << std::endl;
    std::cout << "   Result: HARDFAULT on Cortex-M0, slow on Cortex-M4" << std::endl;
    
    std::cout << "\n✓ CORRECT WAY (memcpy for unaligned fields):" << std::endl;
    uint8_t header = rx_buffer[0];
    uint32_t timestamp;
    uint16_t length;
    uint32_t crc;
    
    std::memcpy(&timestamp, &rx_buffer[1], sizeof(timestamp));
    std::memcpy(&length, &rx_buffer[5], sizeof(length));
    std::memcpy(&crc, &rx_buffer[7], sizeof(crc));
    
    std::cout << "   Header: 0x" << std::hex << static_cast<int>(header) << std::endl;
    std::cout << "   Timestamp: 0x" << timestamp << std::endl;
    std::cout << "   Length: 0x" << length << std::endl;
    std::cout << "   CRC: 0x" << crc << std::dec << std::endl;
    std::cout << "   ✓ Works on ALL ARM processors!" << std::endl;
    
    std::cout << "\n💡 COMPILER OPTIMIZATIONS:" << std::endl;
    std::cout << "   • With -O2: memcpy() compiles to efficient LDR/STR" << std::endl;
    std::cout << "   • No function call overhead" << std::endl;
    std::cout << "   • Compiler knows about alignment and handles it" << std::endl;
}

// ===================================================================
// 9. COMPILER FLAG EXAMPLES
// ===================================================================

void show_compiler_examples() {
    std::cout << "\n=== 9. PRACTICAL COMPILER COMMAND EXAMPLES ===" << std::endl;
    
    std::cout << "\n📋 TYPICAL MAKEFILE FLAGS:" << std::endl;
    
    std::cout << "\n1. CORTEX-M0 PROJECT (strict alignment):" << std::endl;
    std::cout << "   CFLAGS = -mcpu=cortex-m0 \\" << std::endl;
    std::cout << "            -mthumb \\" << std::endl;
    std::cout << "            -mno-unaligned-access \\" << std::endl;
    std::cout << "            -O2 -Wall" << std::endl;
    
    std::cout << "\n2. CORTEX-M4 PROJECT (with FPU):" << std::endl;
    std::cout << "   CFLAGS = -mcpu=cortex-m4 \\" << std::endl;
    std::cout << "            -mthumb \\" << std::endl;
    std::cout << "            -mfpu=fpv4-sp-d16 \\" << std::endl;
    std::cout << "            -mfloat-abi=hard \\" << std::endl;
    std::cout << "            -munaligned-access \\" << std::endl;
    std::cout << "            -O2 -Wall" << std::endl;
    
    std::cout << "\n3. MIXED ARM/THUMB PROJECT (ARM7):" << std::endl;
    std::cout << "   CFLAGS = -mcpu=arm7tdmi \\" << std::endl;
    std::cout << "            -mthumb \\" << std::endl;
    std::cout << "            -mthumb-interwork \\" << std::endl;
    std::cout << "            -O2 -Wall" << std::endl;
    
    std::cout << "\n4. ASSEMBLY FILE COMPILATION:" << std::endl;
    std::cout << "   # For Thumb-2 code" << std::endl;
    std::cout << "   arm-none-eabi-as -mcpu=cortex-m4 \\" << std::endl;
    std::cout << "                     -mthumb \\" << std::endl;
    std::cout << "                     startup.s -o startup.o" << std::endl;
}

// ===================================================================
// 10. DEBUGGING HARDFAULTS
// ===================================================================

void explain_debugging_hardfaults() {
    std::cout << "\n=== 10. DEBUGGING HARDFAULTS ON ARM ===" << std::endl;
    
    std::cout << "\n🔍 WHEN YOU GET A HARDFAULT:" << std::endl;
    
    std::cout << "\n1. CHECK FAULT STATUS REGISTERS:" << std::endl;
    std::cout << "   • HFSR (HardFault Status Register) at 0xE000ED2C" << std::endl;
    std::cout << "   • CFSR (Configurable Fault Status) at 0xE000ED28" << std::endl;
    std::cout << "   • MMFAR (MemManage Fault Address) at 0xE000ED34" << std::endl;
    std::cout << "   • BFAR (BusFault Address) at 0xE000ED38" << std::endl;
    
    std::cout << "\n2. COMMON FAULT CAUSES:" << std::endl;
    std::cout << "   IBUSERR (bit 0 of CFSR):  Instruction bus error" << std::endl;
    std::cout << "     → Called function at wrong address or wrong mode" << std::endl;
    std::cout << "   " << std::endl;
    std::cout << "   PRECISERR (bit 1 of CFSR): Data bus error" << std::endl;
    std::cout << "     → Unaligned access on Cortex-M0" << std::endl;
    std::cout << "   " << std::endl;
    std::cout << "   IACCVIOL (bit 0 of CFSR): MPU violation" << std::endl;
    std::cout << "     → Tried to execute from non-executable memory" << std::endl;
    
    std::cout << "\n3. DEBUGGER INSPECTION:" << std::endl;
    std::cout << "   (gdb) info registers  # Check PC, LR, SP" << std::endl;
    std::cout << "   (gdb) x/4x $sp        # Stack contents" << std::endl;
    std::cout << "   (gdb) bt              # Backtrace" << std::endl;
    std::cout << "   " << std::endl;
    std::cout << "   Look for:" << std::endl;
    std::cout << "   • PC (Program Counter) - where crash occurred" << std::endl;
    std::cout << "   • LR (Link Register) - return address (LSB = mode)" << std::endl;
    std::cout << "   • Unaligned addresses (check if address % 4 != 0)" << std::endl;
}

// ===================================================================
// MAIN
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  ARM INSTRUCTION SETS: ARM, THUMB, THUMB-2" << std::endl;
    std::cout << "  Alignment Issues and Compiler Switches" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    explain_instruction_sets();
    explain_alignment_issues();
    explain_interworking();
    explain_compiler_switches();
    demonstrate_alignment_crash();
    demonstrate_portable_unaligned();
    explain_assembly_directives();
    demonstrate_real_world_scenario();
    show_compiler_examples();
    explain_debugging_hardfaults();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  SUMMARY: AVOIDING ARM THUMB-2 CRASHES" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n🎯 KEY TAKEAWAYS:" << std::endl;
    
    std::cout << "\n1. INSTRUCTION SET MODES:" << std::endl;
    std::cout << "   • ARM (A32): 32-bit, powerful, larger code" << std::endl;
    std::cout << "   • Thumb (T16): 16-bit, compact, limited" << std::endl;
    std::cout << "   • Thumb-2 (T32): 16/32-bit mix, best of both" << std::endl;
    std::cout << "   • Cortex-M: Thumb-2 only (no ARM mode)" << std::endl;
    
    std::cout << "\n2. ALIGNMENT REQUIREMENTS:" << std::endl;
    std::cout << "   • uint32_t MUST be 4-byte aligned on Cortex-M0" << std::endl;
    std::cout << "   • Cortex-M3/M4/M7 can handle unaligned (but slower)" << std::endl;
    std::cout << "   • Use std::memcpy() for portable unaligned access" << std::endl;
    
    std::cout << "\n3. INTERWORKING:" << std::endl;
    std::cout << "   • Function address LSB indicates mode (0=ARM, 1=Thumb)" << std::endl;
    std::cout << "   • Use BLX instruction for mode switching" << std::endl;
    std::cout << "   • Mark assembly functions with .thumb_func" << std::endl;
    
    std::cout << "\n4. COMPILER FLAGS:" << std::endl;
    std::cout << "   -mthumb                    # Use Thumb-2 instructions" << std::endl;
    std::cout << "   -mcpu=cortex-m4            # Specify processor" << std::endl;
    std::cout << "   -mno-unaligned-access      # Disable unaligned (M0)" << std::endl;
    std::cout << "   -munaligned-access         # Enable unaligned (M3+)" << std::endl;
    
    std::cout << "\n5. BEST PRACTICES:" << std::endl;
    std::cout << "   ✓ Use std::memcpy() for unaligned access" << std::endl;
    std::cout << "   ✓ Align structs with __attribute__((aligned(4)))" << std::endl;
    std::cout << "   ✓ Use __attribute__((packed)) carefully" << std::endl;
    std::cout << "   ✓ Compile with correct -mcpu flag" << std::endl;
    std::cout << "   ✓ Test on actual hardware (not just simulator)" << std::endl;
    std::cout << "   ✗ Never cast unaligned pointers directly" << std::endl;
    std::cout << "   ✗ Don't mix ARM/Thumb without proper interworking" << std::endl;
    
    std::cout << "\n💡 DEBUGGING TIPS:" << std::endl;
    std::cout << "   1. Enable HardFault handler to print fault registers" << std::endl;
    std::cout << "   2. Check CFSR register to identify fault type" << std::endl;
    std::cout << "   3. Look for unaligned addresses in fault address registers" << std::endl;
    std::cout << "   4. Verify -mcpu matches your actual hardware" << std::endl;
    std::cout << "   5. Check linker script for correct alignment" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
