#include <vector>
#include <unordered_map>

#include <cstdint>
#include <cstdio>

class CONTEXT {
public:
    uint8_t register_a;
    uint8_t register_x;
    uint8_t status;
    uint16_t program_counter;
};


class OPCODE {
public:
    virtual void proceed(CONTEXT& ctx) {
        std:printf("from parent\n");
    }
};


class CPU {
public:
    CONTEXT ctx;
    std::unordered_map<uint8_t, OPCODE*> opcodeMap;

    CPU(): ctx() {
        ctx.register_a = 0;
        ctx.status = 0;
        ctx.program_counter = 0;
    }

    void register_opencode(uint8_t opcode, OPCODE* fn) {
        opcodeMap.emplace(opcode, fn);
    }

    void interpret(std::vector<uint8_t> progream);

};

void  CPU::interpret(std::vector<uint8_t> progream) {

}

class _ll: public OPCODE {
public:
    void proceed(CONTEXT& ctx) override {
        std::printf("Hello\n");
    }
};


int main() {
    CPU c1;
    c1.register_opencode(12, new _ll());
    (*c1.opcodeMap[12]).proceed(c1.ctx);
    return 0;
}