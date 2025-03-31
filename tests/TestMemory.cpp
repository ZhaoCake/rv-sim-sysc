#include <systemc>
#include <iostream>
#include "../src/memory/Memory.h"

using namespace sc_core;
using namespace riscv;

// Test bench for memory module
class TestBench : public sc_module {
public:
    // ROM interface signals
    sc_signal<bool> rom_read;
    sc_signal<Word> rom_address;
    sc_signal<Word> rom_data;
    
    // RAM interface signals
    sc_signal<bool> ram_read;
    sc_signal<bool> ram_write;
    sc_signal<Word> ram_address;
    sc_signal<Word> ram_write_data;
    sc_signal<Word> ram_read_data;
    
    // Memory instance
    Memory *memory;

    SC_HAS_PROCESS(TestBench);
    TestBench(sc_module_name name) : sc_module(name) {
        // Instantiate memory
        memory = new Memory("memory");
        
        // Connect ROM interface
        memory->rom_read(rom_read);
        memory->rom_address(rom_address);
        memory->rom_data(rom_data);
        
        // Connect RAM interface
        memory->ram_read(ram_read);
        memory->ram_write(ram_write);
        memory->ram_address(ram_address);
        memory->ram_write_data(ram_write_data);
        memory->ram_read_data(ram_read_data);
        
        // Register test process
        SC_THREAD(test_process);
    }
    
    ~TestBench() {
        delete memory;
    }
    
    void test_process() {
        std::cout << "Starting memory test..." << std::endl;
        
        // Initialize signals
        rom_read.write(false);
        ram_read.write(false);
        ram_write.write(false);
        
        wait(10, SC_NS);
        
        // Test 1: Write word to RAM
        std::cout << "Test 1: Writing 0xABCD1234 to address 0x100" << std::endl;
        ram_write.write(true);
        ram_read.write(false);
        ram_address.write(0x100);
        ram_write_data.write(0xABCD1234);
        
        wait(10, SC_NS);
        
        ram_write.write(false);
        wait(10, SC_NS);
        
        // Test 2: Read word from RAM
        std::cout << "Test 2: Reading from address 0x100" << std::endl;
        ram_read.write(true);
        ram_write.write(false);
        ram_address.write(0x100);
        
        wait(10, SC_NS);
        
        std::cout << "Read value: 0x" << std::hex << ram_read_data.read() << std::dec << std::endl;
        ram_read.write(false);
        wait(10, SC_NS);
        
        // Test 3: Write byte manually through word write (LSU would normally do this)
        std::cout << "Test 3: Writing byte 0xEF to address 0x104 using read-modify-write" << std::endl;
        
        // First read the word
        ram_read.write(true);
        ram_address.write(0x104 & ~0x3);  // Align to word boundary
        wait(10, SC_NS);
        
        Word original_word = ram_read_data.read();
        ram_read.write(false);
        wait(5, SC_NS);
        
        // Modify the specific byte
        uint8_t byte_offset = 0x104 & 0x3;
        Word mask = ~(0xFF << (byte_offset * 8));
        Word new_word = (original_word & mask) | (static_cast<Word>(0xEF) << (byte_offset * 8));
        
        // Write back
        ram_write.write(true);
        ram_address.write(0x104 & ~0x3);
        ram_write_data.write(new_word);
        wait(10, SC_NS);
        ram_write.write(false);
        wait(10, SC_NS);
        
        // Test 4: Read word and extract byte manually (LSU would do this)
        std::cout << "Test 4: Reading word from address 0x104 aligned and extracting byte" << std::endl;
        ram_read.write(true);
        ram_address.write(0x104 & ~0x3);
        
        wait(10, SC_NS);
        
        Word read_word = ram_read_data.read();
        uint8_t extracted_byte = (read_word >> (byte_offset * 8)) & 0xFF;
        
        std::cout << "Read word value: 0x" << std::hex << read_word << std::dec << std::endl;
        std::cout << "Extracted byte value: 0x" << std::hex << static_cast<int>(extracted_byte) << std::dec << std::endl;
        ram_read.write(false);
        wait(10, SC_NS);
        
        // Test 5: Write to ROM via RAM interface and read via ROM interface
        std::cout << "Test 5: Writing to ROM area and reading via ROM interface" << std::endl;
        ram_write.write(true);
        ram_read.write(false);
        ram_address.write(0x0);  // ROM area
        ram_write_data.write(0x12345678);
        
        wait(10, SC_NS);
        
        ram_write.write(false);
        wait(10, SC_NS);
        
        // Read via ROM interface
        rom_read.write(true);
        rom_address.write(0x0);
        
        wait(10, SC_NS);
        
        std::cout << "Read from ROM interface: 0x" << std::hex << rom_data.read() << std::dec << std::endl;
        rom_read.write(false);
        
        wait(10, SC_NS);
        
        std::cout << "Memory test completed." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    TestBench tb("test_bench");
    
    std::cout << "Simulation starting..." << std::endl;
    sc_start();
    std::cout << "Simulation finished at " << sc_time_stamp() << std::endl;
    
    return 0;
}
