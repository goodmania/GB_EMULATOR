#pragma once

#include <map>
#include "common.h"
#include "instructions.h"

#define CPU_DEBUG 0

class Emu;
class Timer;
class Bus;
class Cpu;
struct CpuContext;
typedef void (Cpu::* IN_PROC)(CpuContext*);

enum InterruptType : u8{
    IT_VBLANK =     1 << 0,
    IT_LCD_STAT =   1 << 1,
    IT_TIMER =      1 << 2,
    IT_SERIAL =     1 << 3,
    IT_JOYPAD =     1 << 4,
} ;

struct CpuRegisters {
    u8 _a;
    u8 _f;
    u8 _b;
    u8 _c;
    u8 _d;
    u8 _e;
    u8 _h;
    u8 _l;
    u16 _pc;
    u16 _sp;
};

/**
 * @brief The context of the CPU.
 */
struct CpuContext {
    CpuRegisters _regs; /**< The CPU registers. */

    u16 _fetchedData; /**< The currently fetched data. */
    u16 _memDest; /**< The memory destination. */
    bool _destIsMem; /**< Flag indicating if the destination is memory. */
    u8 _curOpcode; /**< The current opcode. */
    Instruction* _curInst; /**< The current instruction. */

    bool _halted; /**< Flag indicating if the CPU is halted. */
    bool _stepping; /**< Flag indicating if the CPU is in stepping mode. */

    bool _intMasterEnabled; /**< Flag controlling whether the CPU accepts interrupts. If IME is enabled, the CPU accepts interrupts. If IME is disabled, the CPU ignores interrupts. */
    bool _enablingIme; /**< Flag indicating if IME will be enabled after the next instruction. */
    u8 _ieRegister; /**< The interrupt enable register. */
    u8 _intFlags; /**< The interrupt flags register. */
};

class Cpu {

#define CPU_FLAG_Z BIT(_context._regs._f, 7)
#define CPU_FLAG_N BIT(_context._regs._f, 6)
#define CPU_FLAG_H BIT(_context._regs._f, 5)
#define CPU_FLAG_C BIT(_context._regs._f, 4)

public:
    Cpu() = default;
    ~Cpu() = default;

	void initialize();
	bool step();
   
    /**
     * Executes the "NOP" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procNone(CpuContext* ctx); // NOP

    /**
     * Executes the "NOP" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procNop(CpuContext* ctx); // NOP

    /**
     * Executes the "CB" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procCb(CpuContext* ctx); // CB

    /**
     * Executes the "RLCA" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procRlca(CpuContext* ctx); // RLCA

    /**
     * Executes the "RRCA" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procRrca(CpuContext* ctx); // RRCA

    /**
     * Executes the "STOP" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procStop(CpuContext* ctx); // STOP

    /**
     * Executes the "DAA" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procDaa(CpuContext* ctx); // DAA

    /**
     * Executes the "CPL" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procCpl(CpuContext* ctx);

    /**
     * Executes the "SCF" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procScf(CpuContext* ctx);

    /**
     * Executes the "CCF" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procCcf(CpuContext* ctx);

    /**
     * Executes the "HALT" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procHalt(CpuContext* ctx);

    /**
     * Executes the "RRA" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procRra(CpuContext* ctx);

    /**
     * Executes the "RLA" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procRla(CpuContext* ctx);

    /**
     * Executes the "AND" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procAnd(CpuContext* ctx);

    /**
     * Executes the "XOR" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procXor(CpuContext* ctx);

    /**
     * Executes the "OR" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procOr(CpuContext* ctx);

    /**
     * Executes the "CP" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procCp(CpuContext* ctx);

    /**
     * Executes the "DI" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procDi(CpuContext* ctx);

    /**
     * Executes the "EI" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procEi(CpuContext* ctx);

    /**
     * Executes the "LD" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procLd(CpuContext* ctx);

    /**
     * Executes the "LDH" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procLdh(CpuContext* ctx);

    /**
     * Executes the "JP" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procJp(CpuContext* ctx);

    /**
     * Executes the "JR" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procJr(CpuContext* ctx);

    /**
     * Executes the "CALL" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procCall(CpuContext* ctx);

    /**
     * Executes the "RST" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procRst(CpuContext* ctx);

    /**
     * Executes the "RET" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procRet(CpuContext* ctx);

    /**
     * Executes the "RETI" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procReti(CpuContext* ctx);

    /**
     * Executes the "POP" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procPop(CpuContext* ctx);

    /**
     * Executes the "PUSH" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procPush(CpuContext* ctx);

    /**
     * Executes the "INC" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procInc(CpuContext* ctx);

    /**
     * Executes the "DEC" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procDec(CpuContext* ctx);

    /**
     * Executes the "SUB" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procSub(CpuContext* ctx);

    /**
     * Executes the "ADC" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procAdc(CpuContext* ctx);

    /**
     * Executes the "ADD" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procAdd(CpuContext* ctx);

    /**
     * Executes the "SBC" instruction.
     * 
     * @param ctx The CPU context.
     */
    void procSbc(CpuContext* ctx);

    void setFlags(CpuContext* ctx, s8 z, s8 n, s8 h, s8 c);
    bool checkCond(CpuContext* ctx);
    void gotoAddr(CpuContext* ctx, u16 addr, bool pushpc);

    u16 readRegister(RegisterType rt);
    void setRegister(RegisterType rt, u16 val);

    u8 getIeRegister();
    void setIeRegister(u8 n);

    u8 readRegister8(RegisterType rt);
    void setRegister8(RegisterType rt, u8 val);

    u8 getIntFlags();
    void setIntFlags(u8 value);

    void instructionToStr(CpuContext* ctx, char* str, const u8 strSize);

    void stackPush(u8 data);
    void stackPush16(u16 data);
    u8 stackPop();
    u16 stackPop16();

    void requestInterrupt(InterruptType t);
    void handleInterrupts(CpuContext* ctx);

private:
    void interruptHandle(CpuContext* ctx, u16 address);
    bool interruptCheck(CpuContext* ctx, u16 address, InterruptType it);

    void initializeProcessors();
    void fetchInstruction();
    void fetchData();
    void execute();
#if CPU_DEBUG
    void DEBUG_update();
    void DEBUG_print();
#endif
    IN_PROC getProcessorByInstructionType(InstructionType& instType);

    CpuContext _context;
    std::map<InstructionType, IN_PROC> _processors;
};

