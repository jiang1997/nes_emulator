#include "CPU.cpp"
#include <cassert>

#include <cstdio>


CPU c1;

void test_0xA9_LDA_immidiate_load_data() {
    c1.ctx = Context();
    c1.ctx.mem = {0xa9, 0x05, 0x00};
    std::printf("-------test_0xA9_LDA_immidiate_load_data-------\n");
    c1.run();
    assert(c1.ctx.register_a == 0x05);
    assert((c1.ctx.status & 0b00000010) == 0b00);
    assert((c1.ctx.status & 0b10000000) == 0);
}

void test_0xA9_LDA_zero_flag() {
    c1.ctx = Context();
    c1.ctx.mem = {0xa9, 0x00, 0x00};
    std::printf("-------test_0xA9_LDA_zero_flag-------\n");
    c1.run();
    assert((c1.ctx.status & 0b00000010) == 0b10);
}

void test_0xAA_TAX_move_a_to_x() {
    c1.ctx = Context();
    c1.ctx.mem = {0xaa, 0x00};
    std::printf("-------test_0xAA_TAX_move_a_to_x-------\n");
    c1.ctx.register_a = 10;
    c1.run();
    assert(c1.ctx.register_x == 10);
}

void test_5_ops_working_together() {
    c1.ctx = Context();
    c1.ctx.mem = {0xa9, 0xc0, 0xaa, 0xe8, 0x00};
    std::printf("-------test_5_ops_working_together-------\n");
    c1.run();
    assert(c1.ctx.register_x == 0xc1);
}

void test_INX_ouerflow() {
    c1.ctx = Context();
    c1.ctx.mem = {0xe8, 0xe8, 0x00};
    
    c1.ctx.register_x = 0xFF;
    
    std::printf("-------test_INX_ouerflow-------\n");

    c1.run();
    assert(c1.ctx.register_x == 1);
}

int main() {


    test_0xA9_LDA_immidiate_load_data();
    test_0xA9_LDA_zero_flag();
    test_0xAA_TAX_move_a_to_x();

    test_5_ops_working_together();
    test_INX_ouerflow();
    
    return 0;
}