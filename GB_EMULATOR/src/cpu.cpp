#include "emu.h"
#include "cpu.h"
#include "timer.h"
#include "cart.h"

namespace {

    RegisterType registerTypeTable[] = {
        RT_B,
        RT_C,
        RT_D,
        RT_E,
        RT_H,
        RT_L,
        RT_HL,
        RT_A
    };

    static const char* registerNameTable[] = {
        "<NONE>",
        "A",
        "F",
        "B",
        "C",
        "D",
        "E",
        "H",
        "L",
        "AF",
        "BC",
        "DE",
        "HL",
        "SP",
        "PC"
    };

    RegisterType decodeReg(u8 reg) {
        if (reg > 0b111) {
            return RT_NONE;
        }
        return registerTypeTable[reg];
    }

    u16 reverse(u16 n) {
        return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
    }

    bool is16Bit(RegisterType rt) {
        return rt >= RT_AF;
    }

#if CPU_DEBUG
    static char DEBUG_msg[1024] = { 0 };
    static int msgSize = 0;
#endif
};

void Cpu::initialize()
{
    _context._regs._pc = 0x100;
    _context._regs._sp = 0xFFFE;
    *((short*)&_context._regs._a) = 0xB001; 
    *((short*)&_context._regs._b) = 0x1300;
    *((short*)&_context._regs._d) = 0xD800;
    *((short*)&_context._regs._h) = 0x4D01;
    _context._ieRegister = 0;
    _context._intFlags = 0;
    _context._intMasterEnabled = false;
    _context._enablingIme = false;
    EmuGet()->getTimer()->getTimerContext()._div = 0xABCC;
    initializeProcessors();
}

void Cpu::initializeProcessors()
{
    _processors[IN_NONE] = &Cpu::procNone;
    _processors[IN_NOP] = &Cpu::procNop;
    _processors[IN_LD] = &Cpu::procLd;
    _processors[IN_LDH] = &Cpu::procLdh;
    _processors[IN_JP] = &Cpu::procJp;
    _processors[IN_DI] = &Cpu::procDi;
    _processors[IN_POP] = &Cpu::procPop;
    _processors[IN_PUSH] = &Cpu::procPush;
    _processors[IN_JR] = &Cpu::procJr;
    _processors[IN_CALL] = &Cpu::procCall;
    _processors[IN_RET] = &Cpu::procRet;
    _processors[IN_RST] = &Cpu::procRst;
    _processors[IN_DEC] = &Cpu::procDec;
    _processors[IN_INC] = &Cpu::procInc;
    _processors[IN_ADD] = &Cpu::procAdd;
    _processors[IN_ADC] = &Cpu::procAdc;
    _processors[IN_SUB] = &Cpu::procSub;
    _processors[IN_SBC] = &Cpu::procSbc;
    _processors[IN_AND] = &Cpu::procAnd;
    _processors[IN_XOR] = &Cpu::procXor;
    _processors[IN_OR] = &Cpu::procOr;
    _processors[IN_CP] = &Cpu::procCp;
    _processors[IN_CB] = &Cpu::procCb;
    _processors[IN_RRCA] = &Cpu::procRrca;
    _processors[IN_RLCA] = &Cpu::procRlca;
    _processors[IN_RRA] = &Cpu::procRra;
    _processors[IN_RLA] = &Cpu::procRla;
    _processors[IN_STOP] = &Cpu::procStop;
    _processors[IN_HALT] = &Cpu::procHalt;
    _processors[IN_DAA] = &Cpu::procDaa;
    _processors[IN_CPL] = &Cpu::procCpl;
    _processors[IN_SCF] = &Cpu::procScf;
    _processors[IN_CCF] = &Cpu::procCcf;
    _processors[IN_EI] = &Cpu::procEi;
    _processors[IN_RETI] = &Cpu::procReti;
}

void Cpu::execute() {
    IN_PROC proc = getProcessorByInstructionType(_context._curInst->_instType);

    if (!proc) {
        NO_IMPL
    }

    (this->*proc)(&_context);
}

void Cpu::DEBUG_update()
{
    if (EmuGet()->getCart()->busRead(0xFF02) == 0x81) {
        char c = EmuGet()->getCart()->busRead(0xFF01);

        DEBUG_msg[msgSize++] = c;

        EmuGet()->getCart()->busWrite(0xFF02, 0);
    }
}

void Cpu::DEBUG_print()
{
    if (DEBUG_msg[0]) {
        printf("DBG: %s\n", DEBUG_msg);
    }
}

IN_PROC Cpu::getProcessorByInstructionType(InstructionType& instType)
{
    return _processors[instType];
}

bool Cpu::step()
{
    if (!_context._halted) {
        u16 pc = _context._regs._pc;

        fetchInstruction();
        EmuGet()->emuCycles(1);
        fetchData();

#if CPU_DEBUG
        char flags[16];
        sprintf_s(flags, "%c%c%c%c",
            _context._regs._f & (1 << 7) ? 'Z' : '-',
            _context._regs._f & (1 << 6) ? 'N' : '-',
            _context._regs._f & (1 << 5) ? 'H' : '-',
            _context._regs._f & (1 << 4) ? 'C' : '-'
        );

        const u8 instSize = 16;
        char inst[instSize];
        instructionToStr(&_context, inst, instSize);
         
        printf("%llu - %04X: %-12s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X\n",
            EmuGet()->getEmuContext()->_ticks,
            pc, inst, _context._curOpcode,
            EmuGet()->getCart()->busRead(pc + 1), EmuGet()->getCart()->busRead(pc + 2), _context._regs._a, flags, _context._regs._b, _context._regs._c,
            _context._regs._d, _context._regs._e, _context._regs._h, _context._regs._l);
#endif

        if (!_context._curInst) {
            printf("Unknown Instruction! %02X\n", _context._curOpcode);
            exit(-7);
        }

        DEBUG_update();
        DEBUG_print();

        execute();
    }
    else {
        //is halted...
        EmuGet()->emuCycles(1);

        if (_context._intFlags) {
            _context._halted = false;
        }
    }

    if (_context._intMasterEnabled) {
        handleInterrupts(&_context);
        _context._enablingIme = false;
    }

    if (_context._enablingIme) {
        _context._intMasterEnabled = true;
    }

    return true;
}

void Cpu::fetchInstruction()
{
    _context._curOpcode = EmuGet()->getCart()->busRead(_context._regs._pc++);
    _context._curInst = getInstructionByOpcode(_context._curOpcode);
}

void Cpu::fetchData()
{
    _context._memDest = 0;
    _context._destIsMem = false;
    Cart* cart = EmuGet()->getCart();
    if (_context._curInst == nullptr) {
        return;
    }

    switch (_context._curInst->_addrMode) {
    case AM_IMP: 
        return;

    case AM_R:
        _context._fetchedData = readRegister(_context._curInst->_regType0);
        return;

    case AM_R_R:
        _context._fetchedData = readRegister(_context._curInst->_regType1);
        return;

    case AM_R_D8:
        _context._fetchedData = cart->busRead(_context._regs._pc);
        EmuGet()->emuCycles(1);
        _context._regs._pc++;
        return;

    case AM_R_D16:
    case AM_D16: {
        u16 lo = cart->busRead(_context._regs._pc);
        EmuGet()->emuCycles(1);

        u16 hi = cart->busRead(_context._regs._pc + 1);
        EmuGet()->emuCycles(1);

        _context._fetchedData = lo | (hi << 8);

        _context._regs._pc += 2;

        return;
    }
    
    case AM_MR_R:{
        _context._fetchedData = readRegister(_context._curInst->_regType1);
        _context._memDest = readRegister(_context._curInst->_regType0);
        _context._destIsMem = true;

        if (_context._curInst->_regType1 == RT_C) {
            _context._memDest |= 0xFF00;
        }
        return;
    }

    case AM_R_MR: {
        u16 addr = readRegister(_context._curInst->_regType1);

        if (_context._curInst->_regType1 == RT_C) {
            addr |= 0xFF00;
        }

        _context._fetchedData = cart->busRead(addr);
        EmuGet()->emuCycles(1);
        return;
    } 
    
    case AM_R_HLI:
        _context._fetchedData = cart->busRead(readRegister(_context._curInst->_regType1));
        EmuGet()->emuCycles(1);;
        setRegister(RT_HL, readRegister(RT_HL) + 1);
        return;

    case AM_R_HLD:
        _context._fetchedData = cart->busRead(readRegister(_context._curInst->_regType1));
        EmuGet()->emuCycles(1);;
        setRegister(RT_HL, readRegister(RT_HL) - 1);
        return;

    case AM_HLI_R:
        _context._fetchedData = readRegister(_context._curInst->_regType1);
        _context._memDest = readRegister(_context._curInst->_regType0);
        _context._destIsMem = true;
        setRegister(RT_HL, readRegister(RT_HL) + 1);
        return;

    case AM_HLD_R:
        _context._fetchedData = readRegister(_context._curInst->_regType1);
        _context._memDest = readRegister(_context._curInst->_regType0);
        _context._destIsMem = true;
        setRegister(RT_HL, readRegister(RT_HL) - 1);
        return;

    case AM_R_A8:
        _context._fetchedData = cart->busRead(_context._regs._pc);
        EmuGet()->emuCycles(1);
        _context._regs._pc++;
        return;

    case AM_A8_R:
        _context._memDest = cart->busRead(_context._regs._pc) | 0xFF00;
        _context._destIsMem = true;
        EmuGet()->emuCycles(1);
        _context._regs._pc++;
        return;

    case AM_HL_SPR:
        _context._fetchedData = cart->busRead(_context._regs._pc);
        EmuGet()->emuCycles(1);
        _context._regs._pc++;
        return;

    case AM_D8:
        _context._fetchedData = cart->busRead(_context._regs._pc);
        EmuGet()->emuCycles(1);
        _context._regs._pc++;
        return;

    case AM_A16_R:
    case AM_D16_R: {
        u16 lo = cart->busRead(_context._regs._pc);
        EmuGet()->emuCycles(1);

        u16 hi = cart->busRead(_context._regs._pc + 1);
        EmuGet()->emuCycles(1);

        _context._memDest = lo | (hi << 8);
        _context._destIsMem = true;

        _context._regs._pc += 2;
        _context._fetchedData = readRegister(_context._curInst->_regType1);

    } return;

    case AM_MR_D8:
        _context._fetchedData = cart->busRead(_context._regs._pc);
        EmuGet()->emuCycles(1);
        _context._regs._pc++;
        _context._memDest = readRegister(_context._curInst->_regType0);
        _context._destIsMem = true;
        return;

    case AM_MR:
        _context._memDest = readRegister(_context._curInst->_regType0);
        _context._destIsMem = true;
        _context._fetchedData = cart->busRead(readRegister(_context._curInst->_regType0));
        EmuGet()->emuCycles(1);;
        return;

    case AM_R_A16: {
        u16 lo = cart->busRead(_context._regs._pc);
        EmuGet()->emuCycles(1);

        u16 hi = cart->busRead(_context._regs._pc + 1);
        EmuGet()->emuCycles(1);

        u16 addr = lo | (hi << 8);

        _context._regs._pc += 2;
        _context._fetchedData = cart->busRead(addr);
        EmuGet()->emuCycles(1);

        return;
    }

    default:
        printf("Unknown Addressing Mode! %d (%02X)\n", _context._curInst->_addrMode, _context._curOpcode);
        exit(-7);
        return;
    }
}

void Cpu::setFlags(CpuContext* ctx, s8 z, s8 n, s8 h, s8 c) {
    if (z != -1) {
        BIT_SET(ctx->_regs._f, 7, z);
    }

    if (n != -1) {
        BIT_SET(ctx->_regs._f, 6, n);
    }

    if (h != -1) {
        BIT_SET(ctx->_regs._f, 5, h);
    }

    if (c != -1) {
        BIT_SET(ctx->_regs._f, 4, c);
    }
}

void Cpu::procNone(CpuContext* ctx)
{
    printf("INVALID INSTRUCTION!\n");
    exit(-7);
}

void Cpu::procNop(CpuContext* ctx)
{

}

void Cpu::procCb(CpuContext* ctx) {
    u16 op = ctx->_fetchedData;
    RegisterType regType = decodeReg(op & 0b111);
    u8 bit = (op >> 3) & 0b111;
    u8 bitOp = (op >> 6) & 0b11;
    u8 regVal = readRegister8(regType);

    EmuGet()->emuCycles(1);

    if (regType == RT_HL) {
        EmuGet()->emuCycles(2);
    }

    switch (bitOp) {
    case 1:
        //BIT
        setFlags(ctx, !(regVal & (1 << bit)), 0, 1, -1);
        return;

    case 2:
        //RST
        regVal &= ~(1 << bit);
        setRegister8(regType, regVal);
        return;

    case 3:
        //SET
        regVal |= (1 << bit);
        setRegister8(regType, regVal);
        return;
    }

    bool flagC = BIT(ctx->_regs._f, 4);

    switch (bit) {
    case 0: {
        //RLC
        bool setC = false;
        u8 result = (regVal << 1) & 0xFF;

        if ((regVal & (1 << 7)) != 0) {
            result |= 1;
            setC = true;
        }

        setRegister8(regType, result);
        setFlags(ctx, result == 0, false, false, setC);
    } return;

    case 1: {
        //RRC
        u8 old = regVal;
        regVal >>= 1;
        regVal |= (old << 7);

        setRegister8(regType, regVal);
        setFlags(ctx, !regVal, false, false, old & 1);
    } return;

    case 2: {
        //RL
        u8 old = regVal;
        regVal <<= 1;
        regVal |= flagC;

        setRegister8(regType, regVal);
        setFlags(ctx, !regVal, false, false, !!(old & 0x80));
    } return;

    case 3: {
        //RR
        u8 old = regVal;
        regVal >>= 1;

        regVal |= (flagC << 7);

        setRegister8(regType, regVal);
        setFlags(ctx, !regVal, false, false, old & 1);
    } return;

    case 4: {
        //SLA
        u8 old = regVal;
        regVal <<= 1;

        setRegister8(regType, regVal);
        setFlags(ctx, !regVal, false, false, !!(old & 0x80));
    } return;

    case 5: {
        //SRA
        u8 u = (int8_t)regVal >> 1;
        setRegister8(regType, u);
        setFlags(ctx, !u, 0, 0, regVal & 1);
    } return;

    case 6: {
        //SWAP
        regVal = ((regVal & 0xF0) >> 4) | ((regVal & 0xF) << 4);
        setRegister8(regType, regVal);
        setFlags(ctx, regVal == 0, false, false, false);
    } return;

    case 7: {
        //SRL
        u8 u = regVal >> 1;
        setRegister8(regType, u);
        setFlags(ctx, !u, 0, 0, regVal & 1);
    } return;
    }

    fprintf(stderr, "ERROR: INVALID CB: %02X", op);
    NO_IMPL
}

void Cpu::procRlca(CpuContext* ctx) {
    u8 u = ctx->_regs._a;
    bool c = (u >> 7) & 1;
    u = (u << 1) | c;
    ctx->_regs._a = u;

    setFlags(ctx, 0, 0, 0, c);
}

void Cpu::procRrca(CpuContext* ctx) {
    u8 b = ctx->_regs._a & 1;
    ctx->_regs._a >>= 1;
    ctx->_regs._a |= (b << 7);

    setFlags(ctx, 0, 0, 0, b);
}

void Cpu::procStop(CpuContext* ctx) {
    fprintf(stderr, "STOPPING!\n");
}

void Cpu::procDaa(CpuContext* ctx) {
    u8 u = 0;
    u32 fc = 0;

    if (CPU_FLAG_H || (!CPU_FLAG_N && (ctx->_regs._a & 0xF) > 9)) {
        u = 6;
    }

    if (CPU_FLAG_C || (!CPU_FLAG_N && ctx->_regs._a > 0x99)) {
        u |= 0x60;
        fc = 1;
    }

    ctx->_regs._a += CPU_FLAG_N ? -u : u;

    setFlags(ctx, ctx->_regs._a == 0, -1, 0, fc);
}

void Cpu::procCpl(CpuContext* ctx) {
    ctx->_regs._a = ~ctx->_regs._a;
    setFlags(ctx, -1, 1, 1, -1);
}

void Cpu::procScf(CpuContext* ctx) {
    setFlags(ctx, -1, 0, 0, 1);
}

void Cpu::procCcf(CpuContext* ctx) {
    setFlags(ctx, -1, 0, 0, CPU_FLAG_C ^ 1);
}

void Cpu::procHalt(CpuContext* ctx) {
    ctx->_halted = true;
}

void Cpu::procRra(CpuContext* ctx) {
    u8 carry = CPU_FLAG_C;
    u8 newC = ctx->_regs._a & 1;

    ctx->_regs._a >>= 1;
    ctx->_regs._a |= (carry << 7);

    setFlags(ctx, 0, 0, 0, newC);
}

void Cpu::procRla(CpuContext* ctx) {
    u8 u = ctx->_regs._a;
    u8 cf = CPU_FLAG_C;
    u8 c = (u >> 7) & 1;

    ctx->_regs._a = (u << 1) | cf;
    setFlags(ctx, 0, 0, 0, c);
}

void Cpu::procAnd(CpuContext* ctx) {
    ctx->_regs._a &= ctx->_fetchedData;
    setFlags(ctx, ctx->_regs._a == 0, 0, 1, 0);
}

void Cpu::procXor(CpuContext* ctx) {
    ctx->_regs._a ^= ctx->_fetchedData & 0xFF;
    setFlags(ctx, ctx->_regs._a == 0, 0, 0, 0);
}

void Cpu::procOr(CpuContext* ctx) {
    ctx->_regs._a |= ctx->_fetchedData & 0xFF;
    setFlags(ctx, ctx->_regs._a == 0, 0, 0, 0);
}

void Cpu::procCp(CpuContext* ctx) {
    s32 n = (s32)ctx->_regs._a - (s32)ctx->_fetchedData;
    setFlags(ctx, n == 0, 1,
        ((s32)ctx->_regs._a & 0x0F) - ((s32)ctx->_fetchedData & 0x0F) < 0, n < 0);
}

void Cpu::procDi(CpuContext* ctx) {
    ctx->_intMasterEnabled = false;
}

void Cpu::procEi(CpuContext* ctx) {
    ctx->_enablingIme = true;
}

void Cpu::procLd(CpuContext* ctx) {
    if (ctx->_destIsMem) {
        //LD (BC), A for instance...

        if (is16Bit(ctx->_curInst->_regType1)) {
            //if 16 bit register...
            EmuGet()->emuCycles(1);
            EmuGet()->getCart()->busWrite16(ctx->_memDest, ctx->_fetchedData);
        }
        else {
            EmuGet()->getCart()->busWrite(ctx->_memDest, static_cast<u8>(ctx->_fetchedData));
        }

        EmuGet()->emuCycles(1);
        return;
    }

    if (ctx->_curInst->_addrMode == AM_HL_SPR) {
        u8 hflag = (readRegister(ctx->_curInst->_regType1) & 0xF) +
            (ctx->_fetchedData & 0xF) >= 0x10;

        u8 cflag = (readRegister(ctx->_curInst->_regType1) & 0xFF) +
            (ctx->_fetchedData & 0xFF) >= 0x100;

        setFlags(ctx, 0, 0, hflag, cflag);
        setRegister(ctx->_curInst->_regType1,
            readRegister(ctx->_curInst->_regType1) + (int8_t)ctx->_fetchedData);

        return;
    }

    setRegister(ctx->_curInst->_regType1, ctx->_fetchedData);
}

void Cpu::procLdh(CpuContext* ctx) {
    if (ctx->_curInst->_regType1 == RT_A) {
        setRegister(ctx->_curInst->_regType1, EmuGet()->getCart()->busRead(0xFF00 | ctx->_fetchedData));
    }
    else {
        EmuGet()->getCart()->busWrite(ctx->_memDest, ctx->_regs._a);
    }
    EmuGet()->emuCycles(1);
}

bool Cpu::checkCond(CpuContext* ctx) {
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch (ctx->_curInst->_conditionType) {
    case CT_NONE: return true;
    case CT_C: return c;
    case CT_NC: return !c;
    case CT_Z: return z;
    case CT_NZ: return !z;
    }

    return false;
}

void Cpu::gotoAddr(CpuContext* ctx, u16 addr, bool pushpc) {
    if (checkCond(ctx)) {
        if (pushpc) {
            EmuGet()->emuCycles(2);
            stackPush16(ctx->_regs._pc);
        }

        ctx->_regs._pc = addr;
        EmuGet()->emuCycles(1);
    }
}

void Cpu::procJp(CpuContext* ctx) {
    gotoAddr(ctx, ctx->_fetchedData, false);
}

void Cpu::procJr(CpuContext* ctx) {
    int8_t rel = (int8_t)(ctx->_fetchedData & 0xFF);
    u16 addr = ctx->_regs._pc + rel;
    gotoAddr(ctx, addr, false);
}

void Cpu::procCall(CpuContext* ctx) {
    gotoAddr(ctx, ctx->_fetchedData, true);
}

void Cpu::procRst(CpuContext* ctx) {
    gotoAddr(ctx, ctx->_curInst->_param, true);
}

void Cpu::procRet(CpuContext* ctx) {
    if (ctx->_curInst->_conditionType != CT_NONE) {
        EmuGet()->emuCycles(1);
    }

    if (checkCond(ctx)) {
        u16 lo = stackPop();
        EmuGet()->emuCycles(1);
        u16 hi = stackPop();
        EmuGet()->emuCycles(1);

        u16 n = (hi << 8) | lo;
        ctx->_regs._pc = n;

        EmuGet()->emuCycles(1);
    }
}

void Cpu::procReti(CpuContext* ctx) {
    ctx->_intMasterEnabled = true;
    procRet(ctx);
}

void Cpu::procPop(CpuContext* ctx) {
    u16 lo = stackPop();
    EmuGet()->emuCycles(1);
    u16 hi = stackPop();
    EmuGet()->emuCycles(1);

    u16 n = (hi << 8) | lo;

    setRegister(ctx->_curInst->_regType0, n);

    if (ctx->_curInst->_regType0 == RT_AF) {
        setRegister(ctx->_curInst->_regType0, n & 0xFFF0);
    }
}

void Cpu::procPush(CpuContext* ctx) {
    u16 hi = (readRegister(ctx->_curInst->_regType0) >> 8) & 0xFF;
    EmuGet()->emuCycles(1);
    stackPush(hi);

    u16 lo = readRegister(ctx->_curInst->_regType0) & 0xFF;
    EmuGet()->emuCycles(1);
    stackPush(lo);

    EmuGet()->emuCycles(1);
}

void Cpu::procInc(CpuContext* ctx) {
    u16 val = readRegister(ctx->_curInst->_regType0) + 1;

    if (is16Bit(ctx->_curInst->_regType0)) {
        EmuGet()->emuCycles(1);
    }

    if (ctx->_curInst->_regType0 == RT_HL && ctx->_curInst->_addrMode == AM_MR) {
        val = EmuGet()->getCart()->busRead(readRegister(RT_HL)) + 1;
        val &= 0xFF;
        EmuGet()->getCart()->busWrite(readRegister(RT_HL), val);
    }
    else {
        setRegister(ctx->_curInst->_regType0, val);
        val = readRegister(ctx->_curInst->_regType0);
    }

    if ((ctx->_curOpcode & 0x03) == 0x03) {
        return;
    }

    setFlags(ctx, val == 0, 0, (val & 0x0F) == 0, -1);
}

void Cpu::procDec(CpuContext* ctx) {
    u16 val = readRegister(ctx->_curInst->_regType0) - 1;

    if (is16Bit(ctx->_curInst->_regType0)) {
        EmuGet()->emuCycles(1);
    }

    if (ctx->_curInst->_regType0 == RT_HL && ctx->_curInst->_addrMode == AM_MR) {
        val = EmuGet()->getCart()->busRead(readRegister(RT_HL)) - 1;
        EmuGet()->getCart()->busWrite(readRegister(RT_HL), val);
    }
    else {
        setRegister(ctx->_curInst->_regType0, val);
        val = readRegister(ctx->_curInst->_regType0);
    }

    if ((ctx->_curOpcode & 0x0B) == 0x0B) {
        return;
    }

    setFlags(ctx, val == 0, 1, (val & 0x0F) == 0x0F, -1);
}

void Cpu::procSub(CpuContext* ctx) {
    u16 val = readRegister(ctx->_curInst->_regType0) - ctx->_fetchedData;

    int z = val == 0;
    int h = ((int)readRegister(ctx->_curInst->_regType0) & 0xF) - ((int)ctx->_fetchedData & 0xF) < 0;
    int c = ((int)readRegister(ctx->_curInst->_regType0)) - ((int)ctx->_fetchedData) < 0;

    setRegister(ctx->_curInst->_regType0, val);
    setFlags(ctx, z, 1, h, c);
}

void Cpu::procSbc(CpuContext* ctx) {
    u8 val = ctx->_fetchedData + CPU_FLAG_C;

    int z = readRegister(ctx->_curInst->_regType0) - val == 0;

    int h = ((int)readRegister(ctx->_curInst->_regType0) & 0xF)
        - ((int)ctx->_fetchedData & 0xF) - ((int)CPU_FLAG_C) < 0;
    int c = ((int)readRegister(ctx->_curInst->_regType0))
        - ((int)ctx->_fetchedData) - ((int)CPU_FLAG_C) < 0;

    setRegister(ctx->_curInst->_regType0, readRegister(ctx->_curInst->_regType0) - val);
    setFlags(ctx, z, 1, h, c);
}

void Cpu::procAdc(CpuContext* ctx) {
    u16 u = ctx->_fetchedData;
    u16 a = ctx->_regs._a;
    u16 c = CPU_FLAG_C;

    ctx->_regs._a = (a + u + c) & 0xFF;

    setFlags(ctx, ctx->_regs._a == 0, 0,
        (a & 0xF) + (u & 0xF) + c > 0xF,
        a + u + c > 0xFF);
}

void Cpu::procAdd(CpuContext* ctx) {
    u32 val = readRegister(ctx->_curInst->_regType0) + ctx->_fetchedData;

    bool is_16bit = is16Bit(ctx->_curInst->_regType0);

    if (is_16bit) {
        EmuGet()->emuCycles(1);
    }

    if (ctx->_curInst->_regType0 == RT_SP) {
        val = readRegister(ctx->_curInst->_regType0) + (int8_t)ctx->_fetchedData;
    }

    int z = (val & 0xFF) == 0;
    int h = (readRegister(ctx->_curInst->_regType0) & 0xF) + (ctx->_fetchedData & 0xF) >= 0x10;
    int c = (int)(readRegister(ctx->_curInst->_regType0) & 0xFF) + (int)(ctx->_fetchedData & 0xFF) >= 0x100;

    if (is_16bit) {
        z = -1;
        h = (readRegister(ctx->_curInst->_regType0) & 0xFFF) + (ctx->_fetchedData & 0xFFF) >= 0x1000;
        u32 n = ((u32)readRegister(ctx->_curInst->_regType0)) + ((u32)ctx->_fetchedData);
        c = n >= 0x10000;
    }

    if (ctx->_curInst->_regType0 == RT_SP) {
        z = 0;
        h = (readRegister(ctx->_curInst->_regType0) & 0xF) + (ctx->_fetchedData & 0xF) >= 0x10;
        c = (int)(readRegister(ctx->_curInst->_regType0) & 0xFF) + (int)(ctx->_fetchedData & 0xFF) >= 0x100;
    }

    setRegister(ctx->_curInst->_regType0, val & 0xFFFF);
    setFlags(ctx, z, 0, h, c);
}


u16 Cpu::readRegister(RegisterType rt)
{
    switch (rt) {
    case RT_A: return _context._regs._a;
    case RT_F: return _context._regs._f;
    case RT_B: return _context._regs._b;
    case RT_C: return _context._regs._c;
    case RT_D: return _context._regs._d;
    case RT_E: return _context._regs._e;
    case RT_H: return _context._regs._h;
    case RT_L: return _context._regs._l;

    case RT_AF: return reverse(*((u16*)&_context._regs._a));
    case RT_BC: return reverse(*((u16*)&_context._regs._b));
    case RT_DE: return reverse(*((u16*)&_context._regs._d));
    case RT_HL: return reverse(*((u16*)&_context._regs._h));

    case RT_PC: return _context._regs._pc;
    case RT_SP: return _context._regs._sp;
    default: return 0;
    }
}

void Cpu::setRegister(RegisterType rt, u16 val)
{
    switch (rt) {
    case RT_A: _context._regs._a = val & 0xFF; break;
    case RT_F: _context._regs._f = val & 0xFF; break;
    case RT_B: _context._regs._b = val & 0xFF; break;
    case RT_C: {
        _context._regs._c = val & 0xFF;
    } break;
    case RT_D: _context._regs._d = val & 0xFF; break;
    case RT_E: _context._regs._e = val & 0xFF; break;
    case RT_H: _context._regs._h = val & 0xFF; break;
    case RT_L: _context._regs._l = val & 0xFF; break;

    case RT_AF: *((u16*)&_context._regs._a) = reverse(val); break;
    case RT_BC: *((u16*)&_context._regs._b) = reverse(val); break;
    case RT_DE: *((u16*)&_context._regs._d) = reverse(val); break;
    case RT_HL: {
        *((u16*)&_context._regs._h) = reverse(val);
        break;
    }

    case RT_PC: _context._regs._pc = val; break;
    case RT_SP: _context._regs._sp = val; break;
    case RT_NONE: break;
    }
}

u8 Cpu::getIeRegister()
{
    return _context._ieRegister;
}

void Cpu::setIeRegister(u8 n)
{
    _context._ieRegister = n;
}

u8 Cpu::readRegister8(RegisterType rt)
{
    switch (rt) {
    case RT_A: return _context._regs._a;
    case RT_F: return _context._regs._f;
    case RT_B: return _context._regs._b;
    case RT_C: return _context._regs._c;
    case RT_D: return _context._regs._d;
    case RT_E: return _context._regs._e;
    case RT_H: return _context._regs._h;
    case RT_L: return _context._regs._l;
    case RT_HL: {
        return EmuGet()->getCart()->busRead(readRegister(RT_HL));
    }
    default:
        printf("**ERR INVALID REG8: %d\n", rt);
        NO_IMPL
    }
}

void Cpu::setRegister8(RegisterType rt, u8 val) {
    switch (rt) {
    case RT_A: _context._regs._a = val & 0xFF; break;
    case RT_F: _context._regs._f = val & 0xFF; break;
    case RT_B: _context._regs._b = val & 0xFF; break;
    case RT_C: _context._regs._c = val & 0xFF; break;
    case RT_D: _context._regs._d = val & 0xFF; break;
    case RT_E: _context._regs._e = val & 0xFF; break;
    case RT_H: _context._regs._h = val & 0xFF; break;
    case RT_L: _context._regs._l = val & 0xFF; break;
    case RT_HL: EmuGet()->getCart()->busWrite(readRegister(RT_HL), val); break;
    default:
        printf("**ERR INVALID REG8: %d\n", rt);
        NO_IMPL
    }
}

u8 Cpu::getIntFlags()
{
    return _context._intFlags;
}

void Cpu::setIntFlags(u8 value)
{
    _context._intFlags = value;
}

void Cpu::instructionToStr(CpuContext* ctx, char* str, const u8 strSize)
{
    Instruction* inst = ctx->_curInst;

    snprintf(str, strSize, "%s ", getInstructionName(inst->_instType));

    switch (inst->_addrMode) {
    case AM_IMP:
        return;

    case AM_R_D16:
    case AM_R_A16:
        snprintf(str, strSize, "%s %s,$%04X", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], ctx->_fetchedData);
        return;

    case AM_R:
        snprintf(str, strSize, "%s %s", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0]);
        return;

    case AM_R_R:
        snprintf(str, strSize, "%s %s,%s", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], registerNameTable[inst->_regType1]);
        return;

    case AM_MR_R:
        snprintf(str, strSize, "%s (%s),%s", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], registerNameTable[inst->_regType1]);
        return;

    case AM_MR:
        snprintf(str, strSize, "%s (%s)", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0]);
        return;

    case AM_R_MR:
        snprintf(str, strSize, "%s %s,(%s)", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], registerNameTable[inst->_regType1]);
        return;

    case AM_R_D8:
    case AM_R_A8:
        snprintf(str, strSize, "%s %s,$%02X", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], ctx->_fetchedData & 0xFF);
        return;

    case AM_R_HLI:
        snprintf(str, strSize, "%s %s,(%s+)", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], registerNameTable[inst->_regType1]);
        return;

    case AM_R_HLD:
        snprintf(str, strSize, "%s %s,(%s-)", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], registerNameTable[inst->_regType1]);
        return;

    case AM_HLI_R:
        snprintf(str, strSize, "%s (%s+),%s", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], registerNameTable[inst->_regType1]);
        return;

    case AM_HLD_R:
        snprintf(str, strSize, "%s (%s-),%s", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], registerNameTable[inst->_regType1]);
        return;

    case AM_A8_R:
        snprintf(str, strSize, "%s $%02X,%s", getInstructionName(inst->_instType),
            EmuGet()->getCart()->busRead(ctx->_regs._pc - 1), registerNameTable[inst->_regType1]);
        return;

    case AM_HL_SPR:
        snprintf(str, strSize, "%s (%s),SP+%d", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], ctx->_fetchedData & 0xFF);
        return;

    case AM_D8:
        snprintf(str, strSize, "%s $%02X", getInstructionName(inst->_instType),
            ctx->_fetchedData & 0xFF);
        return;

    case AM_D16:
        snprintf(str, strSize, "%s $%04X", getInstructionName(inst->_instType),
            ctx->_fetchedData);
        return;

    case AM_MR_D8:
        snprintf(str, strSize, "%s (%s),$%02X", getInstructionName(inst->_instType),
            registerNameTable[inst->_regType0], ctx->_fetchedData & 0xFF);
        return;

    case AM_A16_R:
        snprintf(str, strSize, "%s ($%04X),%s", getInstructionName(inst->_instType),
            ctx->_fetchedData, registerNameTable[inst->_regType1]);
        return;

    default:
        fprintf(stderr, "INVALID AM: %d\n", inst->_addrMode);
        NO_IMPL
    }
}

// stack
/*
    STACK

    SP=0xDFFF

    MEMORY:
    0xDFF7: 00
    0xDFF8: 00
    0xDFF9: 00
    0xDFFA: 00
    0xDFFB: 00
    0xDFFC: 00
    0xDFFD: 00
    0xDFFE: 00
    0xDFFF: 00 <- SP

    PUSH 0x55

    SP-- = 0xDFFE
    MEMORY[0xDFFE] = 0x55

    MEMORY:
    0xDFF7: 00
    0xDFF8: 00
    0xDFF9: 00
    0xDFFA: 00
    0xDFFB: 00
    0xDFFC: 00
    0xDFFD: 00
    0xDFFE: 55 <- SP
    0xDFFF: 00

    PUSH 0x77

    SP-- = 0xDFFD
    MEMORY[0xDFFD] = 0x77

    MEMORY:
    0xDFF7: 00
    0xDFF8: 00
    0xDFF9: 00
    0xDFFA: 00
    0xDFFB: 00
    0xDFFC: 00
    0xDFFD: 77 <- SP
    0xDFFE: 55
    0xDFFF: 00

    val = POP

    val = MEMORY[0xDFFD] = 0x77
    SP++ = 0xDFFE

    MEMORY:
    0xDFF7: 00
    0xDFF8: 00
    0xDFF9: 00
    0xDFFA: 00
    0xDFFB: 00
    0xDFFC: 00
    0xDFFD: 77
    0xDFFE: 55 <- SP
    0xDFFF: 00


    PUSH 0x88

    SP-- = 0xDFFD
    MEMORY[0xDFFD] = 0x88

    MEMORY:
    0xDFF7: 00
    0xDFF8: 00
    0xDFF9: 00
    0xDFFA: 00
    0xDFFB: 00
    0xDFFC: 00
    0xDFFD: 88 <- SP
    0xDFFE: 55
    0xDFFF: 00
*/

void Cpu::stackPush(u8 data) {
    _context._regs._sp--;
    EmuGet()->getCart()->busWrite(_context._regs._sp, data);
}

void Cpu::stackPush16(u16 data) {
    stackPush((data >> 8) & 0xFF);
    stackPush(data & 0xFF);
}

u8 Cpu::stackPop() {
    return EmuGet()->getCart()->busRead(_context._regs._sp++);
}

u16 Cpu::stackPop16() {
    u16 lo = stackPop();
    u16 hi = stackPop();

    return (hi << 8) | lo;
}

// interrupt
void Cpu::interruptHandle(CpuContext* ctx, u16 address) {
     stackPush16(ctx->_regs._pc);
     ctx->_regs._pc = address;
}

bool Cpu::interruptCheck(CpuContext* ctx, u16 address, InterruptType it) {
    if (ctx->_intFlags & it && ctx->_ieRegister & it) {
        interruptHandle(ctx, address);
        ctx->_intFlags &= ~it;
        ctx->_halted = false;
        ctx->_intMasterEnabled = false;

        return true;
    }
    return false;
}

void Cpu::requestInterrupt(InterruptType t)
{
    _context._intFlags |= t;
}

void Cpu::handleInterrupts(CpuContext* ctx)
{
    if (interruptCheck(ctx, 0x40, IT_VBLANK)) {

    }
    else if (interruptCheck(ctx, 0x48, IT_LCD_STAT)) {

    }
    else if (interruptCheck(ctx, 0x50, IT_TIMER)) {

    }
    else if (interruptCheck(ctx, 0x58, IT_SERIAL)) {

    }
    else if (interruptCheck(ctx, 0x60, IT_JOYPAD)) {

    }
}
