#include <map>

#include "instructions.h"
#include "cpu.h"

namespace {
    Instruction g_instructions[0x100];

    static const char* g_instructionTable[] = {
    "<NONE>",
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RLCA",
    "ADD",
    "RRCA",
    "STOP",
    "RLA",
    "JR",
    "RRA",
    "DAA",
    "CPL",
    "SCF",
    "CCF",
    "HALT",
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "XOR",
    "OR",
    "CP",
    "POP",
    "JP",
    "PUSH",
    "RET",
    "CB",
    "CALL",
    "RETI",
    "LDH",
    "JPHL",
    "DI",
    "EI",
    "RST",
    "IN_ERR",
    "IN_RLC",
    "IN_RRC",
    "IN_RL",
    "IN_RR",
    "IN_SLA",
    "IN_SRA",
    "IN_SWAP",
    "IN_SRL",
    "IN_BIT",
    "IN_RES",
    "IN_SET"
    };
} // namespace

Instruction::Instruction()
{
    initializeInstructions();
}

void initializeInstructions()
{
    g_instructions[0x00]._instType = IN_NOP;
    g_instructions[0x00]._addrMode = AM_IMP;

    g_instructions[0x05]._instType = IN_DEC;
    g_instructions[0x05]._addrMode = AM_R;
    g_instructions[0x05]._regType0 = RT_B;

    g_instructions[0x0E]._instType = IN_LD;
    g_instructions[0x0E]._addrMode = AM_R_D8;
    g_instructions[0x0E]._regType0 = RT_C;

    g_instructions[0xAF]._instType = IN_XOR;
    g_instructions[0xAF]._addrMode = AM_R;
    g_instructions[0xAF]._regType0 = RT_A;

    g_instructions[0xC3]._instType = IN_JP;
    g_instructions[0xC3]._addrMode = AM_D16;

    g_instructions[0xF3]._instType = IN_DI;
}

const char* getInstructionName(InstructionType t)
{
    return g_instructionTable[t];
}

Instruction* getInstructionByOpcode(u8 opcode)
{
    return &g_instructions[opcode];
}