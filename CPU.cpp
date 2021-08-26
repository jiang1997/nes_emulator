#include <vector>
#include <unordered_map>

#include <cstdint>
#include <cstdio>

#include <cstring>

// status register 
#define INDEX_OF_N 7
#define INDEX_OF_V 6
#define INDEX_OF_B 4
#define INDEX_OF_D 3
#define INDEX_OF_I 2
#define INDEX_OF_Z 1
#define INDEX_OF_C 0


class Context {
public:
    uint8_t& register_a;
    uint8_t& register_x;
    uint8_t& register_y;
    uint8_t& status;
    uint16_t program_counter;

    bool terminal;

    uint8_t mem[0xFFFF + 8];

    Context(): register_a(mem[0xFFFF + 0]), register_x(mem[0xFFFF + 1]), register_y(mem[0xFFFF + 2]), status(mem[0xFFFF + 3]) {
        // register_a = mem[0xFFFF + 0];
        // register_x = mem[0xFFFF + 1];
        // register_y = mem[0xFFFF + 2];
        // status = mem[0xFFFF + 3];
        program_counter = 0;

        terminal = false;

    }

};


class Instruction {
public:
    virtual void proceed(Context& ctx) {
        std:printf("from parent\n");
    }
    
    virtual void callbacck(Context& ctx) {

    }

    void update_zero_and_negative_flags(Context& ctx, uint8_t value) {
        if(value == 0) {
            ctx.status = ctx.status | 0b00000010;
        } else {
            ctx.status = ctx.status & 0b11111101;
        }
        
        if((value & 0b10000000) != 0) {
            ctx.status = ctx.status | 0b10000000;
        } else {
            ctx.status = ctx.status & 0b01111111;
        }
    }

    void set_bit(uint8_t& data, uint8_t i) {
        data = data | ((uint8_t)1 << i);
    }

    void unset_bit(uint8_t& data, uint8_t i) {
        data = data & ~((uint8_t)1 << i);
    }
};

class AddressingMode {
public:
    static uint8_t mem_read(uint8_t* mem, uint16_t addr) {
        return mem[addr];
    }

    static void mem_write(uint8_t* mem, uint16_t addr, uint8_t data) {
        mem[addr] = data;
    }

    static uint16_t mem_read_u16(uint8_t* mem, uint16_t pos) {
        uint16_t lo = mem_read(mem, pos);
        uint16_t hi = mem_read(mem, pos + 1);
        return (hi << 8) | (uint8_t)lo;
    }

    static void mem_write_u16(uint8_t* mem, uint16_t pos, uint16_t data) {
        uint8_t hi = data >> 8;
        uint8_t lo = data & 0xFF;
        mem_write(mem, pos, lo);
        mem_write(mem, pos + 1, hi);
    }
};

class Immediate: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        uint16_t addr = ctx.program_counter;
        ctx.program_counter += 1;
        return addr;
    }
};

class ZeroPage: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        uint8_t addr = mem_read(ctx.mem, ctx.program_counter);
        ctx.program_counter += 1;
        return addr;
    }
};

class ZeroPage_X: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        uint8_t pos = mem_read(ctx.mem, ctx.program_counter);
        uint8_t addr = pos + ctx.register_x;
        ctx.program_counter += 1;
        return addr;
    }
};

class ZeroPage_Y: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        uint8_t pos = mem_read(ctx.mem, ctx.program_counter);
        uint8_t addr = pos + ctx.register_y;
        ctx.program_counter += 1;
        return addr;
    }
};

class Absolute: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        uint16_t addr = mem_read_u16(ctx.mem, ctx.program_counter);
        ctx.program_counter += 2;
        return addr;
    }
};

class Absolute_X: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        uint16_t base = mem_read_u16(ctx.mem, ctx.program_counter);
        uint16_t addr = base + ctx.register_x;
        ctx.program_counter += 2;
        return addr;
    }
};

class Absolute_Y: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        uint16_t base = mem_read_u16(ctx.mem, ctx.program_counter);
        uint16_t addr = base + ctx.register_y;
        ctx.program_counter += 2;
        return addr;
    }
};

class Indirect_X: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        uint8_t base = mem_read(ctx.mem, ctx.program_counter);
        uint8_t ptr = base + ctx.register_x;
        // uint16_t lo = mem_read(ctx.mem, ptr);
        // uint16_t hi = mem_read(ctx.mem, ptr + 1);
        uint16_t addr = mem_read_u16(ctx.mem, ptr);
        ctx.program_counter += 1;
        // return (hi << 8) | (uint8_t)lo;
        return addr;
    }
};

class Indirect_Y: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        uint8_t base = mem_read(ctx.mem, ctx.program_counter);

        // uint16_t lo = mem_read(ctx.mem, base);
        // uint16_t hi = mem_read(ctx.mem, base + 1);
        // uint16_t deref_base = (hi << 8) | (uint8_t)lo;
        uint16_t deref_base = mem_read_u16(ctx.mem, base);
        uint16_t deref = deref_base + ctx.register_y;

        ctx.program_counter += 1;
        return deref;
    }
};

class Accumulator: public AddressingMode{
public:
    static uint16_t get_operand_address(Context& ctx) {
        return 0xFFFF + 0;
    }
};


template <class Mode>
class LDA: public Instruction {
public:
    void proceed(Context& ctx) override {
        std::printf("LDA\n");
        uint8_t param = ctx.mem[Mode::get_operand_address(ctx)];
        // ctx.program_counter += 1;
        ctx.register_a = param;

        update_zero_and_negative_flags(ctx, ctx.register_a);
    }
};

template <class Mode>
class AND: public Instruction {
public:
    void proceed(Context& ctx) override {
        std::printf("AND\n");
        uint8_t param = ctx.mem[Mode::get_operand_address(ctx)];
        ctx.register_a &= param;
        update_zero_and_negative_flags(ctx, ctx.register_a);        
    }
};

template <class Mode>
class ASL: public Instruction {
public:
    void proceed(Context& ctx) override {
        std::printf("ASL\n");
        uint8_t addr = Mode::get_operand_address(ctx);
        uint8_t param = Mode::mem_read(ctx.mem, addr);
        Mode::mem_write(ctx.mem, addr, param << 1);


        // set C
        ctx.status = ctx.status | (param & ((uint8_t)1 << INDEX_OF_C));

        // Z
        if(Mode::mem_read(ctx.mem, addr) != 0) {
            set_bit(ctx.status, INDEX_OF_Z);
            // ctx.status = ctx.status & 0b11111101;
        } else {
            unset_bit(ctx.status, INDEX_OF_Z);
            // ctx.status = ctx.status | 0b00000010;
        }

        // set N
        if(Mode::mem_read(ctx.mem, addr) & 0b10000000 != 0) {
            set_bit(ctx.status, INDEX_OF_N);
            // ctx.status = ctx.status | 0b10000000; 
        } else {
            unset_bit(ctx.status, INDEX_OF_N);
        }
        
    }
};

template <class Mode>
class STA: public Instruction {
public:
    void proceed(Context& ctx) override {
        std::printf("STA\n");
        uint16_t addr = Mode::get_operand_address(ctx);
        Mode::mem_write(ctx.mem, addr, ctx.register_a);
    }
};


class TAX_0xAA: public Instruction {
public:
    void proceed(Context& ctx) override {
        std::printf("TAX\n");
        ctx.register_x = ctx.register_a;
        update_zero_and_negative_flags(ctx, ctx.register_x);
    }
};

class INX_0xE8: public Instruction {
    void proceed(Context& ctx) override {
        std::printf("INX\n");
        ctx.register_x += 1;
        update_zero_and_negative_flags(ctx, ctx.register_x);
    }
};

class BRK: public Instruction {
public:
    void proceed(Context& ctx) override {
        std::printf("BRK\n");
        ctx.terminal = true;
    }
};


class CPU {
public:
    Context ctx;
    std::unordered_map<uint8_t, Instruction*> opscodeMap;

    CPU(): ctx() {
        register_opencode(0xA9, new LDA<Immediate>());
        register_opencode(0xA5, new LDA<ZeroPage>());
        register_opencode(0xAD, new LDA<Absolute>());

        register_opencode(0x85, new STA<ZeroPage>());
        register_opencode(0x95, new STA<ZeroPage_X>());

        register_opencode(0x00, new BRK());
        register_opencode(0xAA, new TAX_0xAA());
        register_opencode(0xE8, new INX_0xE8());
    }

    void register_opencode(uint8_t opcode, Instruction* fn) {
        opscodeMap.emplace(opcode, fn);
    }

    void run();
    void load(std::vector<uint8_t>);
    
    
    void load_and_run(std::vector<uint8_t> program) {
        load(program);
        reset();
        run();
    }

    void reset() {
        ctx.register_a = 0;
        ctx.register_x = 0;
        ctx.register_y = 0;
        ctx.status = 0;

        ctx.program_counter = AddressingMode::mem_read_u16(ctx.mem, 0xFFFC);
        ctx.terminal = false;
    }



};

void  CPU::run() {
    while(true) {
        std::printf("program_counter: %d\n", ctx.program_counter);

        uint8_t opcode = ctx.mem[ctx.program_counter];
        ctx.program_counter += 1;

        opscodeMap[opcode]->proceed(ctx);
        if(ctx.terminal) {
            std::printf("terminal\n");
            break;
        }
    }

}

void CPU::load(std::vector<uint8_t> program) {
    // memcpy
    uint16_t offset = 0x8000;
    std::memcpy(ctx.mem + offset, program.data(), program.size());
    AddressingMode::mem_write_u16(ctx.mem, 0xFFFC, 0x8000);
}



