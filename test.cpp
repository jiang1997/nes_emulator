#include "CPU.cpp"
#include <cassert>

#include <cstdio>
#include <iostream>

CPU c1;

void test_0xA9_LDA_immidiate_load_data() {

    std::printf("-------test_0xA9_LDA_immidiate_load_data-------\n");
    c1.load_and_run({0xa9, 0x05, 0x00});
    // std::printf("%d\n", c1.ctx.register_a);
    assert(c1.ctx.register_a == 0x05);
    assert((c1.ctx.status & 0b00000010) == 0b00);
    assert((c1.ctx.status & 0b10000000) == 0);
}

void test_0xA9_LDA_zero_flag() {
    std::printf("-------test_0xA9_LDA_zero_flag-------\n");
    c1.load_and_run({0xa9, 0x00, 0x00});
    assert((c1.ctx.status & 0b00000010) == 0b10);
}

void test_0xAA_TAX_move_a_to_x() {
    c1.reset();
    c1.load({0xaa, 0x00});
    c1.ctx.register_a = 10;
    std::printf("-------test_0xAA_TAX_move_a_to_x-------\n");
    c1.run();
    assert(c1.ctx.register_x == 10);
}

void test_5_ops_working_together() {
    std::printf("-------test_5_ops_working_together-------\n");
    c1.load_and_run({0xa9, 0xc0, 0xaa, 0xe8, 0x00});
    assert(c1.ctx.register_x == 0xc1);
}

void test_INX_ouerflow() {
    
    c1.load({0xe8, 0xe8, 0x00});
    c1.reset();
    c1.ctx.register_x = 0xFF;
    
    std::printf("-------test_INX_ouerflow-------\n");

    c1.run();
    assert(c1.ctx.register_x == 1);
}

void test_BBC() {
    
    c1.load({0xa2, 0x08, 0xca, 0x8e, 0x00, 0x02, 0xe0, 0x03, 0xd0, 0xf8, 0x8e, 0x01, 0x02, 0x00});
    c1.reset();
    // c1.run();
    for(int i = 0; c1.ctx.terminal != true; ++i) {
        printf("status: 0x%x\n", c1.ctx.status);
        
        c1.step();
        if(c1.ctx.program_counter == 0x8000 + 8) {
            assert(c1.ctx.mem[0x0200] == c1.ctx.register_x);
        }
    }
}

int main() {


    // test_0xA9_LDA_immidiate_load_data();
    // test_0xA9_LDA_zero_flag();
    // test_0xAA_TAX_move_a_to_x();

    // test_5_ops_working_together();
    // test_INX_ouerflow();
    test_BBC();
    return 0;
}