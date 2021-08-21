#include <vector>
#include <unordered_map>

#include <cstdint>
#include <cstdio>

class CPU {
public:
    uint8_t register_a;
    uint8_t status;
    uint16_t program_counter;

    std::unordered_map<uint8_t, void (*)()> opcodeMap;

    CPU() {
        register_a = 0;
        status = 0;
        program_counter = 0;
    }

    void register_opencode(uint8_t opcode, void (*fn)()) {
        opcodeMap.emplace(opcode, fn);
    }

    void interpret(std::vector<uint8_t> progream);

};

void  CPU::interpret(std::vector<uint8_t> progream) {

}

void f1() {
    std::printf("Hello World");
}

int main() {
    CPU c1;
    c1.register_opencode(12, f1);
    // c1.opcodeMap[12]();
    return 0;
}