#pragma once

#include <map>
#include "common.h"
#include "instructions.h"

#define CPU_DEBUG 1

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

struct CpuContext {
    CpuRegisters _regs;

    //current fetch...
    u16 _fetchedData;
    u16 _memDest;
    bool _destIsMem;
    u8 _curOpcode;
    Instruction* _curInst;

    bool _halted;
    bool _stepping;

    bool _intMasterEnabled;
    bool _enablingIme;
    u8 _ieRegister;
    u8 _intFlags;
};

class Cpu {

#define CPU_FLAG_Z BIT(_context._regs._f, 7)
#define CPU_FLAG_N BIT(_context._regs._f, 6)
#define CPU_FLAG_H BIT(_context._regs._f, 5)
#define CPU_FLAG_C BIT(_context._regs._f, 4)

public:
    Cpu(Bus& bus, Timer& timer, Emu& emu);
    ~Cpu() = default;

	void initialize();
	bool step();
   
    void procNone(CpuContext* ctx);
    void procNop(CpuContext* ctx);
    void procCb(CpuContext* ctx);
    void procRlca(CpuContext* ctx);
    void procRrca(CpuContext* ctx);
    void procStop(CpuContext* ctx);
    void procDaa(CpuContext* ctx);
    void procCpl(CpuContext* ctx);
    void procScf(CpuContext* ctx);
    void procCcf(CpuContext* ctx);
    void procHalt(CpuContext* ctx);
    void procRra(CpuContext* ctx);
    void procRla(CpuContext* ctx);
    void procAnd(CpuContext* ctx);
    void procXor(CpuContext* ctx);
    void procOr(CpuContext* ctx);
    void procCp(CpuContext* ctx);
    void procDi(CpuContext* ctx);
    void procEi(CpuContext* ctx);
    void procLd(CpuContext* ctx);
    void procLdh(CpuContext* ctx);
    void procJp(CpuContext* ctx);
    void procJr(CpuContext* ctx);
    void procCall(CpuContext* ctx);
    void procRst(CpuContext* ctx);
    void procRet(CpuContext* ctx);
    void procReti(CpuContext* ctx);
    void procPop(CpuContext* ctx);
    void procPush(CpuContext* ctx);
    void procInc(CpuContext* ctx);
    void procDec(CpuContext* ctx);
    void procSub(CpuContext* ctx);
    void procAdc(CpuContext* ctx);
    void procAdd(CpuContext* ctx);
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

    void instructionToStr(CpuContext* ctx, char* str);

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
    Timer* _timer;
    Bus* _bus;
    std::map<InstructionType, IN_PROC> _processors;
    Emu* _emu;
};

