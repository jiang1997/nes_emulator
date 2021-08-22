#include <vector>
#include <unordered_map>

#include <cstdint>
#include <cstdio>

class Context {
public:
    uint8_t register_a;
    uint8_t register_x;
    uint8_t status;
    uint16_t program_counter;

    bool terminal;

    std::vector<uint8_t> mem;

    Context() {
        register_a = 0;
        status = 0;
        program_counter = 0;

        terminal = false;

        mem = std::vector<uint8_t>(0xFFFF);
    }
};


class Instruction {
public:
    virtual void proceed(Context& ctx) {
        std:printf("from parent\n");
    }
    
    virtual void callbacck(Context& ctx) {

    }
};


class CPU {
public:
    Context ctx;
    std::unordered_map<uint8_t, Instruction*> opscodeMap;

    CPU(): ctx() {
        //
    }

    void register_opencode(uint8_t opscode, Instruction* fn) {
        opscodeMap.emplace(opscode, fn);
    }

    void interpret();

};

void  CPU::interpret() {
    ctx.program_counter = 0;

    while(true) {
        std::printf("program_counter: %d\n", ctx.program_counter);

        uint8_t opscode = ctx.mem[ctx.program_counter];
        ctx.program_counter += 1;

        opscodeMap[opscode]->proceed(ctx);
        if(ctx.terminal) {
            std::printf("terminal\n");
            break;
        }
    }

}

class LDA_0xA9: public Instruction {
public:
    void proceed(Context& ctx) override {
        std::printf("LDA_0xA9\n");
        uint8_t param = ctx.mem[ctx.program_counter];
        ctx.program_counter += 1;
        ctx.register_a = param;

        if(ctx.register_a == 0) {
            ctx.status = ctx.status | 0b00000010;
        } else {
            ctx.status = ctx.status & 0b11111101;
        }
        
        if((ctx.register_a & 0b10000000) != 0) {
            ctx.status = ctx.status | 0b10000000;
        } else {
            ctx.status = ctx.status & 0b01111111;
        }
    }
};

class BRK_0x00: public Instruction {
public:
    void proceed(Context& ctx) override {
        std::printf("BRK_0x00\n");
        ctx.terminal = true;
    }
};


