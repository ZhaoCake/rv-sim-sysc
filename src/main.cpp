#include <systemc>
#include <iostream>
#include <string>
#include "core/Top.h"

using namespace sc_core;
using namespace riscv;

// Command line options
struct Options {
    std::string program_file;
    uint32_t num_cycles;
    bool dump_registers;
    bool dump_memory;
    std::string memory_dump_file;
    uint32_t memory_dump_start;
    uint32_t memory_dump_size;
};

// Parse command line arguments
Options parse_arguments(int argc, char* argv[]) {
    Options opts;
    
    // Default values
    opts.program_file = "";
    opts.num_cycles = 100;  // Default number of cycles to run
    opts.dump_registers = true;
    opts.dump_memory = false;
    opts.memory_dump_file = "memory_dump.bin";
    opts.memory_dump_start = 0;
    opts.memory_dump_size = 256;
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--program" || arg == "-p") {
            if (i + 1 < argc) {
                opts.program_file = argv[++i];
            } else {
                std::cerr << "Error: --program requires a file path" << std::endl;
                exit(1);
            }
        } else if (arg == "--cycles" || arg == "-c") {
            if (i + 1 < argc) {
                opts.num_cycles = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: --cycles requires a value" << std::endl;
                exit(1);
            }
        } else if (arg == "--no-reg-dump") {
            opts.dump_registers = false;
        } else if (arg == "--dump-memory" || arg == "-m") {
            opts.dump_memory = true;
            
            // Optional memory dump file path
            if (i + 1 < argc && argv[i+1][0] != '-') {
                opts.memory_dump_file = argv[++i];
            }
        } else if (arg == "--mem-start") {
            if (i + 1 < argc) {
                opts.memory_dump_start = std::stoul(argv[++i], nullptr, 0);
            } else {
                std::cerr << "Error: --mem-start requires a value" << std::endl;
                exit(1);
            }
        } else if (arg == "--mem-size") {
            if (i + 1 < argc) {
                opts.memory_dump_size = std::stoul(argv[++i], nullptr, 0);
            } else {
                std::cerr << "Error: --mem-size requires a value" << std::endl;
                exit(1);
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "RISC-V SystemC Simulator" << std::endl;
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --program, -p FILE    Program binary file to load" << std::endl;
            std::cout << "  --cycles, -c NUM      Number of cycles to run (default: 100)" << std::endl;
            std::cout << "  --no-reg-dump         Don't dump register contents after simulation" << std::endl;
            std::cout << "  --dump-memory, -m     Dump memory contents after simulation" << std::endl;
            std::cout << "  --mem-start ADDR      Start address for memory dump (default: 0)" << std::endl;
            std::cout << "  --mem-size SIZE       Size of memory to dump in bytes (default: 256)" << std::endl;
            std::cout << "  --help, -h            Show this help message" << std::endl;
            exit(0);
        } else if (opts.program_file.empty()) {
            // Assume it's the program file if not specified with flag
            opts.program_file = arg;
        } else {
            std::cerr << "Warning: Ignoring unknown argument: " << arg << std::endl;
        }
    }
    
    return opts;
}

int sc_main(int argc, char* argv[]) {
    // Parse command line arguments
    Options opts = parse_arguments(argc, argv);
    
    // Check if program file is provided
    if (opts.program_file.empty()) {
        std::cerr << "Error: No program file specified" << std::endl;
        std::cerr << "Run with --help for usage information" << std::endl;
        return 1;
    }
    
    // Create signals
    sc_signal<bool> clk_sig;
    sc_signal<bool> reset_sig;
    
    // Create processor top module
    Top processor("riscv_processor");
    processor.clk(clk_sig);
    processor.reset(reset_sig);
    
    // Load program
    std::cout << "Loading program from " << opts.program_file << std::endl;
    if (!processor.load_program(opts.program_file)) {
        std::cerr << "Error: Failed to load program" << std::endl;
        return 1;
    }
    
    // Initial reset
    reset_sig.write(true);
    clk_sig.write(false);
    
    sc_start(10, SC_NS);
    
    reset_sig.write(false);
    
    // Run simulation
    std::cout << "Starting simulation for " << opts.num_cycles << " cycles..." << std::endl;
    
    for (uint32_t i = 0; i < opts.num_cycles; i++) {
        // Toggle clock
        clk_sig.write(!clk_sig.read());
        sc_start(5, SC_NS);
        
        clk_sig.write(!clk_sig.read());
        sc_start(5, SC_NS);
    }
    
    std::cout << "Simulation completed after " << opts.num_cycles << " cycles." << std::endl;
    
    // Dump register contents
    if (opts.dump_registers) {
        std::cout << "\nFinal register state:" << std::endl;
        processor.dump_registers();
    }
    
    // Dump memory contents
    if (opts.dump_memory) {
        std::cout << "Dumping memory to " << opts.memory_dump_file << std::endl;
        processor.dump_memory(opts.memory_dump_file, opts.memory_dump_start, opts.memory_dump_size);
    }
    
    return 0;
}