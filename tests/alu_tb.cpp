#include <systemc>
#include <iostream>
#include <iomanip>
#include "../include/constants.h"
#include "../include/types.h"

using namespace riscv;

// 模拟ALU功能的测试版本
SC_MODULE(TestALU) {
    sc_core::sc_in<sc_dt::sc_uint<4>> operation;
    sc_core::sc_in<Word> operand1;
    sc_core::sc_in<Word> operand2;
    sc_core::sc_out<Word> result;
    
    SC_CTOR(TestALU) {
        SC_METHOD(compute);
        sensitive << operation << operand1 << operand2;
    }
    
    void compute() {
        Word op1 = operand1.read();
        Word op2 = operand2.read();
        unsigned int oper = operation.read();
        Word res = 0;
        
        switch (oper) {
            case ALU_ADD:
                res = op1 + op2;
                break;
            case ALU_SUB:
                res = op1 - op2;
                break;
            case ALU_AND:
                res = op1 & op2;
                break;
            case ALU_OR:
                res = op1 | op2;
                break;
            case ALU_XOR:
                res = op1 ^ op2;
                break;
            case ALU_SLL:
                res = op1 << (op2 & 0x1F);
                break;
            case ALU_SRL:
                res = op1 >> (op2 & 0x1F);
                break;
            case ALU_SRA: {
                // 算术右移需要特殊处理
                CppWord cpp_op1 = static_cast<CppWord>(op1.to_uint());
                CppWord cpp_res = static_cast<int32_t>(cpp_op1) >> (op2.to_uint() & 0x1F);
                res = cpp_res;
                break;
            }
            case ALU_SLT: {
                CppWord cpp_op1 = static_cast<CppWord>(op1.to_uint());
                CppWord cpp_op2 = static_cast<CppWord>(op2.to_uint());
                res = (static_cast<int32_t>(cpp_op1) < static_cast<int32_t>(cpp_op2)) ? 1 : 0;
                break;
            }
            case ALU_SLTU:
                res = (op1 < op2) ? 1 : 0;
                break;
            default:
                res = 0;
                break;
        }
        
        result.write(res);
    }
};

// ALU测试台
SC_MODULE(ALUTestbench) {
    sc_core::sc_signal<sc_dt::sc_uint<4>> op_sig;
    sc_core::sc_signal<Word> op1_sig;
    sc_core::sc_signal<Word> op2_sig;
    sc_core::sc_signal<Word> result_sig;
    
    TestALU* alu;
    sc_core::sc_trace_file* tf;
    
    SC_CTOR(ALUTestbench) {
        // 实例化ALU
        alu = new TestALU("alu_instance");
        alu->operation(op_sig);
        alu->operand1(op1_sig);
        alu->operand2(op2_sig);
        alu->result(result_sig);
        
        // 测试线程
        SC_THREAD(stimulus);
        
        // 创建波形跟踪
        tf = sc_core::sc_create_vcd_trace_file("alu_waveform");
        sc_core::sc_trace(tf, op_sig, "operation");
        sc_core::sc_trace(tf, op1_sig, "operand1");
        sc_core::sc_trace(tf, op2_sig, "operand2");
        sc_core::sc_trace(tf, result_sig, "result");
    }
    
    ~ALUTestbench() {
        sc_core::sc_close_vcd_trace_file(tf);
        delete alu;
    }
    
    void stimulus() {
        // 测试用例数组
        struct TestCase {
            unsigned int op;
            uint32_t op1;
            uint32_t op2;
            uint32_t expected;
            const char* op_name;
        };
        
        TestCase tests[] = {
            {ALU_ADD,  10, 20, 30,   "ADD"},
            {ALU_SUB,  30, 15, 15,   "SUB"},
            {ALU_AND,  0xFF, 0x0F, 0x0F, "AND"},
            {ALU_OR,   0xF0, 0x0F, 0xFF, "OR"},
            {ALU_XOR,  0xFF, 0x55, 0xAA, "XOR"},
            {ALU_SLL,  0x01, 4, 0x10, "SLL"},
            {ALU_SRL,  0x10, 2, 0x04, "SRL"},
            {ALU_SRA,  0x80000000, 4, 0xF8000000, "SRA"},
            {ALU_SLT,  10, 20, 1,    "SLT"},
            {ALU_SLTU, 20, 10, 0,    "SLTU"}
        };
        
        std::cout << "Starting ALU tests..." << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << std::setw(6) << "OP" << " | " 
                  << std::setw(10) << "Operand 1" << " | " 
                  << std::setw(10) << "Operand 2" << " | " 
                  << std::setw(10) << "Result" << " | " 
                  << std::setw(10) << "Expected" << " | " 
                  << "Status" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        for (const auto& test : tests) {
            // 设置输入
            op_sig.write(test.op);
            op1_sig.write(test.op1);
            op2_sig.write(test.op2);
            
            // 等待一段时间让计算完成
            wait(1, sc_core::SC_NS);
            
            // 检查结果
            uint32_t actual = result_sig.read().to_uint();
            bool pass = (actual == test.expected);
            
            std::cout << std::setw(6) << test.op_name << " | " 
                      << std::hex << std::setw(10) << test.op1 << " | " 
                      << std::hex << std::setw(10) << test.op2 << " | " 
                      << std::hex << std::setw(10) << actual << " | " 
                      << std::hex << std::setw(10) << test.expected << " | " 
                      << (pass ? "PASS" : "FAIL") << std::endl;
            
            // 等待一下再进行下一个测试
            wait(9, sc_core::SC_NS);
        }
        
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "ALU tests completed!" << std::endl;
        
        sc_core::sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    ALUTestbench tb("alu_testbench");
    
    sc_core::sc_start();
    
    std::cout << "Simulation completed at " << sc_core::sc_time_stamp() << std::endl;
    
    return 0;
}
