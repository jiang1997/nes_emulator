#include "CPU.cpp"
#include <cassert>

#include <cstdio>


CPU c1;

void test_0xA9_LDA_immidiate_load_data() {
    c1.ctx = Context();
    c1.ctx.mem = {0xa9, 0x05, 0x00};

    c1.interpret();
    assert(c1.ctx.register_a == 0x05);
    assert((c1.ctx.status & 0b00000010) == 0b00);
    assert((c1.ctx.status & 0b10000000) == 0);
}

void test_0xA9_LDA_zero_flag() {
    c1.ctx = Context();
    c1.ctx.mem = {0xa9, 0x00, 0x00};
    
    c1.interpret();
    assert((c1.ctx.status & 0b00000010) == 0b10);
}

int main() {
    c1.register_opencode(0xA9, new LDA_0xA9());
    c1.register_opencode(0x00, new BRK_0x00());


    test_0xA9_LDA_immidiate_load_data();
    test_0xA9_LDA_zero_flag();
    
    return 0;
}