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

    g_instructions[0x01]._instType = IN_LD;
    g_instructions[0x01]._addrMode = AM_R_D16;
    g_instructions[0x01]._regType0 = RT_BC;

    g_instructions[0x02]._instType = IN_LD;
    g_instructions[0x02]._addrMode = AM_MR_R;
    g_instructions[0x02]._regType0 = RT_BC;
    g_instructions[0x02]._regType1 = RT_A;

    g_instructions[0x03]._instType = IN_INC;
    g_instructions[0x03]._addrMode = AM_R;
    g_instructions[0x03]._regType0 = RT_BC;

    g_instructions[0x04]._instType = IN_INC;
    g_instructions[0x04]._addrMode = AM_R;
    g_instructions[0x04]._regType0 = RT_B;

    g_instructions[0x05]._instType = IN_DEC;
    g_instructions[0x05]._addrMode = AM_R;
    g_instructions[0x05]._regType0 = RT_B;

    g_instructions[0x06]._instType = IN_LD;
    g_instructions[0x06]._addrMode = AM_R_D8;
    g_instructions[0x06]._regType0 = RT_B;

    g_instructions[0x07]._instType = IN_RLCA;

    g_instructions[0x08]._instType = IN_LD;
    g_instructions[0x08]._addrMode = AM_A16_R;
    g_instructions[0x08]._regType0 = RT_NONE;
    g_instructions[0x08]._regType1 = RT_SP;

    g_instructions[0x09]._instType = IN_ADD;
    g_instructions[0x09]._addrMode = AM_R_R;
    g_instructions[0x09]._regType0 = RT_HL;
    g_instructions[0x09]._regType1 = RT_BC;

    g_instructions[0x0A]._instType = IN_LD;
    g_instructions[0x0A]._addrMode = AM_R_MR;
    g_instructions[0x0A]._regType0 = RT_A;
    g_instructions[0x0A]._regType1 = RT_BC;

    g_instructions[0x0B]._instType = IN_DEC;
    g_instructions[0x0B]._addrMode = AM_R;
    g_instructions[0x0B]._regType0 = RT_BC;

    g_instructions[0x0C]._instType = IN_INC;
    g_instructions[0x0C]._addrMode = AM_R;
    g_instructions[0x0C]._regType0 = RT_C;

    g_instructions[0x0D]._instType = IN_DEC;
    g_instructions[0x0D]._addrMode = AM_R;
    g_instructions[0x0D]._regType0 = RT_C;

    g_instructions[0x0E]._instType = IN_LD;
    g_instructions[0x0E]._addrMode = AM_R_D8;
    g_instructions[0x0E]._regType0 = RT_C;

    g_instructions[0x0F]._instType = IN_RRCA;

    // 0x1X
    g_instructions[0x10]._instType = IN_STOP;

    g_instructions[0x11]._instType = IN_LD;
    g_instructions[0x11]._addrMode = AM_R_D16;
    g_instructions[0x11]._regType0 = RT_DE;

    g_instructions[0x12]._instType = IN_LD;
    g_instructions[0x12]._addrMode = AM_MR_R;
    g_instructions[0x12]._regType0 = RT_DE;
    g_instructions[0x12]._regType1 = RT_A;

    g_instructions[0x13]._instType = IN_INC;
    g_instructions[0x13]._addrMode = AM_R;
    g_instructions[0x13]._regType0 = RT_DE;

    g_instructions[0x14]._instType = IN_INC;
    g_instructions[0x14]._addrMode = AM_R;
    g_instructions[0x14]._regType0 = RT_D;

    g_instructions[0x15]._instType = IN_DEC;
    g_instructions[0x15]._addrMode = AM_R;
    g_instructions[0x15]._regType0 = RT_D;

    g_instructions[0x16]._instType = IN_LD;
    g_instructions[0x16]._addrMode = AM_R_D8;
    g_instructions[0x16]._regType0 = RT_D;

    g_instructions[0x17]._instType = IN_RLA;

    g_instructions[0x18]._instType = IN_JR;
    g_instructions[0x18]._addrMode = AM_D8;

    g_instructions[0x19]._instType = IN_ADD;
    g_instructions[0x19]._addrMode = AM_R_R;
    g_instructions[0x19]._regType0 = RT_HL;
    g_instructions[0x19]._regType1 = RT_DE;

    g_instructions[0x1A]._instType = IN_LD;
    g_instructions[0x1A]._addrMode = AM_R_MR;
    g_instructions[0x1A]._regType0 = RT_A;
    g_instructions[0x1A]._regType1 = RT_DE;

    g_instructions[0x1B]._instType = IN_DEC;
    g_instructions[0x1B]._addrMode = AM_R;
    g_instructions[0x1B]._regType0 = RT_DE;

    g_instructions[0x1C]._instType = IN_INC;
    g_instructions[0x1C]._addrMode = AM_R;
    g_instructions[0x1C]._regType0 = RT_E;

    g_instructions[0x1D]._instType = IN_DEC;
    g_instructions[0x1D]._addrMode = AM_R;
    g_instructions[0x1D]._regType0 = RT_E;

    g_instructions[0x1E]._instType = IN_LD;
    g_instructions[0x1E]._addrMode = AM_R_D8;
    g_instructions[0x1E]._regType0 = RT_E;

    g_instructions[0x1F]._instType = IN_RRA;

    //0x2X
    g_instructions[0x20]._instType = IN_JR;
    g_instructions[0x20]._addrMode = AM_D8;
    g_instructions[0x20]._regType0 = RT_NONE;
    g_instructions[0x20]._regType1 = RT_NONE;
    g_instructions[0x20]._conditionType = CT_NZ;

    g_instructions[0x21]._instType = IN_LD;
    g_instructions[0x21]._addrMode = AM_R_D16;
    g_instructions[0x21]._regType0 = RT_HL;

    g_instructions[0x22]._instType = IN_LD;
    g_instructions[0x22]._addrMode = AM_HLI_R;
    g_instructions[0x22]._regType0 = RT_HL;
    g_instructions[0x22]._regType1 = RT_A;

    g_instructions[0x23]._instType = IN_INC;
    g_instructions[0x23]._addrMode = AM_R;
    g_instructions[0x23]._regType0 = RT_HL;

    g_instructions[0x24]._instType = IN_INC;
    g_instructions[0x24]._addrMode = AM_R;
    g_instructions[0x24]._regType0 = RT_H;

    g_instructions[0x25]._instType = IN_DEC;
    g_instructions[0x25]._addrMode = AM_R;
    g_instructions[0x25]._regType0 = RT_H;

    g_instructions[0x26]._instType = IN_LD;
    g_instructions[0x26]._addrMode = AM_R_D8;
    g_instructions[0x26]._regType0 = RT_H;

    g_instructions[0x27]._instType = IN_DAA;

    g_instructions[0x28]._instType = IN_JR;
    g_instructions[0x28]._addrMode = AM_D8;
    g_instructions[0x28]._regType0 = RT_NONE;
    g_instructions[0x28]._regType1 = RT_NONE;
    g_instructions[0x28]._conditionType = CT_Z;

    g_instructions[0x29]._instType = IN_ADD;
    g_instructions[0x29]._addrMode = AM_R_R;
    g_instructions[0x29]._regType0 = RT_HL;
    g_instructions[0x29]._regType1 = RT_HL;

    g_instructions[0x2A]._instType = IN_LD;
    g_instructions[0x2A]._addrMode = AM_R_HLI;
    g_instructions[0x2A]._regType0 = RT_A;
    g_instructions[0x2A]._regType1 = RT_HL;

    g_instructions[0x2B]._instType = IN_DEC;
    g_instructions[0x2B]._addrMode = AM_R;
    g_instructions[0x2B]._regType0 = RT_HL;

    g_instructions[0x2C]._instType = IN_INC;
    g_instructions[0x2C]._addrMode = AM_R;
    g_instructions[0x2C]._regType0 = RT_L;

    g_instructions[0x2D]._instType = IN_DEC;
    g_instructions[0x2D]._addrMode = AM_R;
    g_instructions[0x2D]._regType0 = RT_L;

    g_instructions[0x2E]._instType = IN_LD;
    g_instructions[0x2E]._addrMode = AM_R_D8;
    g_instructions[0x2E]._regType0 = RT_L;

    g_instructions[0x2F]._instType = IN_CPL;

    //0x3X
    g_instructions[0x30]._instType = IN_JR;
    g_instructions[0x30]._addrMode = AM_D8;
    g_instructions[0x30]._regType0 = RT_NONE;
    g_instructions[0x30]._regType1 = RT_NONE;
    g_instructions[0x30]._conditionType = CT_NC;

    g_instructions[0x31]._instType = IN_LD;
    g_instructions[0x31]._addrMode = AM_R_D16;
    g_instructions[0x31]._regType0 = RT_SP;

    g_instructions[0x32]._instType = IN_LD;
    g_instructions[0x32]._addrMode = AM_HLD_R;
    g_instructions[0x32]._regType0 = RT_HL;
    g_instructions[0x32]._regType1 = RT_A;

    g_instructions[0x33]._instType = IN_INC;
    g_instructions[0x33]._addrMode = AM_R;
    g_instructions[0x33]._regType0 = RT_SP;

    g_instructions[0x34]._instType = IN_INC;
    g_instructions[0x34]._addrMode = AM_MR;
    g_instructions[0x34]._regType0 = RT_HL;

    g_instructions[0x35]._instType = IN_DEC;
    g_instructions[0x35]._addrMode = AM_MR;
    g_instructions[0x35]._regType0 = RT_HL;

    g_instructions[0x36]._instType = IN_LD;
    g_instructions[0x36]._addrMode = AM_MR_D8;
    g_instructions[0x36]._regType0 = RT_HL;

    g_instructions[0x37]._instType = IN_SCF;

    g_instructions[0x38]._instType = IN_JR;
    g_instructions[0x38]._addrMode = AM_D8;
    g_instructions[0x38]._regType0 = RT_NONE;
    g_instructions[0x38]._regType1 = RT_NONE;
    g_instructions[0x38]._conditionType = CT_C;

    g_instructions[0x39]._instType = IN_ADD;
    g_instructions[0x39]._addrMode = AM_R_R;
    g_instructions[0x39]._regType0 = RT_HL;
    g_instructions[0x39]._regType1 = RT_SP;

    g_instructions[0x3A]._instType = IN_LD;
    g_instructions[0x3A]._addrMode = AM_R_HLD;
    g_instructions[0x3A]._regType0 = RT_A;
    g_instructions[0x3A]._regType1 = RT_HL;

    g_instructions[0x3B]._instType = IN_DEC;
    g_instructions[0x3B]._addrMode = AM_R;
    g_instructions[0x3B]._regType0 = RT_SP;

    g_instructions[0x3C]._instType = IN_INC;
    g_instructions[0x3C]._addrMode = AM_R;
    g_instructions[0x3C]._regType0 = RT_A;

    g_instructions[0x3D]._instType = IN_DEC;
    g_instructions[0x3D]._addrMode = AM_R;
    g_instructions[0x3D]._regType0 = RT_A;

    g_instructions[0x3E]._instType = IN_LD;
    g_instructions[0x3E]._addrMode = AM_R_D8;
    g_instructions[0x3E]._regType0 = RT_A;

    g_instructions[0x3F]._instType = IN_CCF;

    // 0x4X
    g_instructions[0x40]._instType = IN_LD;
    g_instructions[0x40]._addrMode = AM_R_R;
    g_instructions[0x40]._regType0 = RT_B;
    g_instructions[0x40]._regType1 = RT_B;

    g_instructions[0x41]._instType = IN_LD;
    g_instructions[0x41]._addrMode = AM_R_R;
    g_instructions[0x41]._regType0 = RT_B;
    g_instructions[0x41]._regType1 = RT_C;

    g_instructions[0x42]._instType = IN_LD;
    g_instructions[0x42]._addrMode = AM_R_R;
    g_instructions[0x42]._regType0 = RT_B;
    g_instructions[0x42]._regType1 = RT_D;

    g_instructions[0x43]._instType = IN_LD;
    g_instructions[0x43]._addrMode = AM_R_R;
    g_instructions[0x43]._regType0 = RT_B;
    g_instructions[0x43]._regType1 = RT_E;

    g_instructions[0x44]._instType = IN_LD;
    g_instructions[0x44]._addrMode = AM_R_R;
    g_instructions[0x44]._regType0 = RT_B;
    g_instructions[0x44]._regType1 = RT_H;

    g_instructions[0x45]._instType = IN_LD;
    g_instructions[0x45]._addrMode = AM_R_R;
    g_instructions[0x45]._regType0 = RT_B;
    g_instructions[0x45]._regType1 = RT_L;

    g_instructions[0x46]._instType = IN_LD;
    g_instructions[0x46]._addrMode = AM_R_MR;
    g_instructions[0x46]._regType0 = RT_B;
    g_instructions[0x46]._regType1 = RT_HL;

    g_instructions[0x47]._instType = IN_LD;
    g_instructions[0x47]._addrMode = AM_R_R;
    g_instructions[0x47]._regType0 = RT_B;
    g_instructions[0x47]._regType1 = RT_A;

    g_instructions[0x48]._instType = IN_LD;
    g_instructions[0x48]._addrMode = AM_R_R;
    g_instructions[0x48]._regType0 = RT_C;
    g_instructions[0x48]._regType1 = RT_B;

    g_instructions[0x49]._instType = IN_LD;
    g_instructions[0x49]._addrMode = AM_R_R;
    g_instructions[0x49]._regType0 = RT_C;
    g_instructions[0x49]._regType1 = RT_C;

    g_instructions[0x4A]._instType = IN_LD;
    g_instructions[0x4A]._addrMode = AM_R_R;
    g_instructions[0x4A]._regType0 = RT_C;
    g_instructions[0x4A]._regType1 = RT_D;

    g_instructions[0x4B]._instType = IN_LD;
    g_instructions[0x4B]._addrMode = AM_R_R;
    g_instructions[0x4B]._regType0 = RT_C;
    g_instructions[0x4B]._regType1 = RT_E;

    g_instructions[0x4C]._instType = IN_LD;
    g_instructions[0x4C]._addrMode = AM_R_R;
    g_instructions[0x4C]._regType0 = RT_C;
    g_instructions[0x4C]._regType1 = RT_H;

    g_instructions[0x4D]._instType = IN_LD;
    g_instructions[0x4D]._addrMode = AM_R_R;
    g_instructions[0x4D]._regType0 = RT_C;
    g_instructions[0x4D]._regType1 = RT_L;

    g_instructions[0x4E]._instType = IN_LD;
    g_instructions[0x4E]._addrMode = AM_R_MR;
    g_instructions[0x4E]._regType0 = RT_C;
    g_instructions[0x4E]._regType1 = RT_HL;

    g_instructions[0x4F]._instType = IN_LD;
    g_instructions[0x4F]._addrMode = AM_R_R;
    g_instructions[0x4F]._regType0 = RT_C;
    g_instructions[0x4F]._regType1 = RT_A;

    //0x5X
    g_instructions[0x50]._instType = IN_LD;
    g_instructions[0x50]._addrMode = AM_R_R;
    g_instructions[0x50]._regType0 = RT_D;
    g_instructions[0x50]._regType1 = RT_B;

    g_instructions[0x51]._instType = IN_LD;
    g_instructions[0x51]._addrMode = AM_R_R;
    g_instructions[0x51]._regType0 = RT_D;
    g_instructions[0x51]._regType1 = RT_C;

    g_instructions[0x52]._instType = IN_LD;
    g_instructions[0x52]._addrMode = AM_R_R;
    g_instructions[0x52]._regType0 = RT_D;
    g_instructions[0x52]._regType1 = RT_D;

    g_instructions[0x53]._instType = IN_LD;
    g_instructions[0x53]._addrMode = AM_R_R;
    g_instructions[0x53]._regType0 = RT_D;
    g_instructions[0x53]._regType1 = RT_E;

    g_instructions[0x54]._instType = IN_LD;
    g_instructions[0x54]._addrMode = AM_R_R;
    g_instructions[0x54]._regType0 = RT_D;
    g_instructions[0x54]._regType1 = RT_H;

    g_instructions[0x55]._instType = IN_LD;
    g_instructions[0x55]._addrMode = AM_R_R;
    g_instructions[0x55]._regType0 = RT_D;
    g_instructions[0x55]._regType1 = RT_L;

    g_instructions[0x56]._instType = IN_LD;
    g_instructions[0x56]._addrMode = AM_R_MR;
    g_instructions[0x56]._regType0 = RT_D;
    g_instructions[0x56]._regType1 = RT_HL;

    g_instructions[0x57]._instType = IN_LD;
    g_instructions[0x57]._addrMode = AM_R_R;
    g_instructions[0x57]._regType0 = RT_D;
    g_instructions[0x57]._regType1 = RT_A;

    g_instructions[0x58]._instType = IN_LD;
    g_instructions[0x58]._addrMode = AM_R_R;
    g_instructions[0x58]._regType0 = RT_E;
    g_instructions[0x58]._regType1 = RT_B;

    g_instructions[0x59]._instType = IN_LD;
    g_instructions[0x59]._addrMode = AM_R_R;
    g_instructions[0x59]._regType0 = RT_E;
    g_instructions[0x59]._regType1 = RT_C;

    g_instructions[0x5A]._instType = IN_LD;
    g_instructions[0x5A]._addrMode = AM_R_R;
    g_instructions[0x5A]._regType0 = RT_E;
    g_instructions[0x5A]._regType1 = RT_D;

    g_instructions[0x5B]._instType = IN_LD;
    g_instructions[0x5B]._addrMode = AM_R_R;
    g_instructions[0x5B]._regType0 = RT_E;
    g_instructions[0x5B]._regType1 = RT_E;

    g_instructions[0x5C]._instType = IN_LD;
    g_instructions[0x5C]._addrMode = AM_R_R;
    g_instructions[0x5C]._regType0 = RT_E;
    g_instructions[0x5C]._regType1 = RT_H;

    g_instructions[0x5D]._instType = IN_LD;
    g_instructions[0x5D]._addrMode = AM_R_R;
    g_instructions[0x5D]._regType0 = RT_E;
    g_instructions[0x5D]._regType1 = RT_L;

    g_instructions[0x5E]._instType = IN_LD;
    g_instructions[0x5E]._addrMode = AM_R_MR;
    g_instructions[0x5E]._regType0 = RT_E;
    g_instructions[0x5E]._regType1 = RT_HL;

    g_instructions[0x5F]._instType = IN_LD;
    g_instructions[0x5F]._addrMode = AM_R_R;
    g_instructions[0x5F]._regType0 = RT_E;
    g_instructions[0x5F]._regType1 = RT_A;

    //0x6X
    g_instructions[0x60]._instType = IN_LD;
    g_instructions[0x60]._addrMode = AM_R_R;
    g_instructions[0x60]._regType0 = RT_H;
    g_instructions[0x60]._regType1 = RT_B;

    g_instructions[0x61]._instType = IN_LD;
    g_instructions[0x61]._addrMode = AM_R_R;
    g_instructions[0x61]._regType0 = RT_H;
    g_instructions[0x61]._regType1 = RT_C;

    g_instructions[0x62]._instType = IN_LD;
    g_instructions[0x62]._addrMode = AM_R_R;
    g_instructions[0x62]._regType0 = RT_H;
    g_instructions[0x62]._regType1 = RT_D;

    g_instructions[0x63]._instType = IN_LD;
    g_instructions[0x63]._addrMode = AM_R_R;
    g_instructions[0x63]._regType0 = RT_H;
    g_instructions[0x63]._regType1 = RT_E;

    g_instructions[0x64]._instType = IN_LD;
    g_instructions[0x64]._addrMode = AM_R_R;
    g_instructions[0x64]._regType0 = RT_H;
    g_instructions[0x64]._regType1 = RT_H;

    g_instructions[0x65]._instType = IN_LD;
    g_instructions[0x65]._addrMode = AM_R_R;
    g_instructions[0x65]._regType0 = RT_H;
    g_instructions[0x65]._regType1 = RT_L;

    g_instructions[0x66]._instType = IN_LD;
    g_instructions[0x66]._addrMode = AM_R_MR;
    g_instructions[0x66]._regType0 = RT_H;
    g_instructions[0x66]._regType1 = RT_HL;

    g_instructions[0x67]._instType = IN_LD;
    g_instructions[0x67]._addrMode = AM_R_R;
    g_instructions[0x67]._regType0 = RT_H;
    g_instructions[0x67]._regType1 = RT_A;

    g_instructions[0x68]._instType = IN_LD;
    g_instructions[0x68]._addrMode = AM_R_R;
    g_instructions[0x68]._regType0 = RT_L;
    g_instructions[0x68]._regType1 = RT_B;

    g_instructions[0x69]._instType = IN_LD;
    g_instructions[0x69]._addrMode = AM_R_R;
    g_instructions[0x69]._regType0 = RT_L;
    g_instructions[0x69]._regType1 = RT_C;

    g_instructions[0x6A]._instType = IN_LD;
    g_instructions[0x6A]._addrMode = AM_R_R;
    g_instructions[0x6A]._regType0 = RT_L;
    g_instructions[0x6A]._regType1 = RT_D;

    g_instructions[0x6B]._instType = IN_LD;
    g_instructions[0x6B]._addrMode = AM_R_R;
    g_instructions[0x6B]._regType0 = RT_L;
    g_instructions[0x6B]._regType1 = RT_E;

    g_instructions[0x6C]._instType = IN_LD;
    g_instructions[0x6C]._addrMode = AM_R_R;
    g_instructions[0x6C]._regType0 = RT_L;
    g_instructions[0x6C]._regType1 = RT_H;

    g_instructions[0x6D]._instType = IN_LD;
    g_instructions[0x6D]._addrMode = AM_R_R;
    g_instructions[0x6D]._regType0 = RT_L;
    g_instructions[0x6D]._regType1 = RT_L;

    g_instructions[0x6E]._instType = IN_LD;
    g_instructions[0x6E]._addrMode = AM_R_MR;
    g_instructions[0x6E]._regType0 = RT_L;
    g_instructions[0x6E]._regType1 = RT_HL;

    g_instructions[0x6F]._instType = IN_LD;
    g_instructions[0x6F]._addrMode = AM_R_R;
    g_instructions[0x6F]._regType0 = RT_L;
    g_instructions[0x6F]._regType1 = RT_A;
    
    //0x7X
    g_instructions[0x70]._instType = IN_LD;
    g_instructions[0x70]._addrMode = AM_MR_R;
    g_instructions[0x70]._regType0 = RT_HL;
    g_instructions[0x70]._regType1 = RT_B;

    g_instructions[0x71]._instType = IN_LD;
    g_instructions[0x71]._addrMode = AM_MR_R;
    g_instructions[0x71]._regType0 = RT_HL;
    g_instructions[0x71]._regType1 = RT_C;

    g_instructions[0x72]._instType = IN_LD;
    g_instructions[0x72]._addrMode = AM_MR_R;
    g_instructions[0x72]._regType0 = RT_HL;
    g_instructions[0x72]._regType1 = RT_D;

    g_instructions[0x73]._instType = IN_LD;
    g_instructions[0x73]._addrMode = AM_MR_R;
    g_instructions[0x73]._regType0 = RT_HL;
    g_instructions[0x73]._regType1 = RT_E;

    g_instructions[0x74]._instType = IN_LD;
    g_instructions[0x74]._addrMode = AM_MR_R;
    g_instructions[0x74]._regType0 = RT_HL;
    g_instructions[0x74]._regType1 = RT_H;

    g_instructions[0x75]._instType = IN_LD;
    g_instructions[0x75]._addrMode = AM_MR_R;
    g_instructions[0x75]._regType0 = RT_HL;
    g_instructions[0x75]._regType1 = RT_L;

    g_instructions[0x76]._instType = IN_HALT;

    g_instructions[0x77]._instType = IN_LD;
    g_instructions[0x77]._addrMode = AM_MR_R;
    g_instructions[0x77]._regType0 = RT_HL;
    g_instructions[0x77]._regType1 = RT_A;

    g_instructions[0x78]._instType = IN_LD;
    g_instructions[0x78]._addrMode = AM_R_R;
    g_instructions[0x78]._regType0 = RT_A;
    g_instructions[0x78]._regType1 = RT_B;

    g_instructions[0x79]._instType = IN_LD;
    g_instructions[0x79]._addrMode = AM_R_R;
    g_instructions[0x79]._regType0 = RT_A;
    g_instructions[0x79]._regType1 = RT_C;

    g_instructions[0x7A]._instType = IN_LD;
    g_instructions[0x7A]._addrMode = AM_R_R;
    g_instructions[0x7A]._regType0 = RT_A;
    g_instructions[0x7A]._regType1 = RT_D;

    g_instructions[0x7B]._instType = IN_LD;
    g_instructions[0x7B]._addrMode = AM_R_R;
    g_instructions[0x7B]._regType0 = RT_A;
    g_instructions[0x7B]._regType1 = RT_E;

    g_instructions[0x7C]._instType = IN_LD;
    g_instructions[0x7C]._addrMode = AM_R_R;
    g_instructions[0x7C]._regType0 = RT_A;
    g_instructions[0x7C]._regType1 = RT_H;

    g_instructions[0x7D]._instType = IN_LD;
    g_instructions[0x7D]._addrMode = AM_R_R;
    g_instructions[0x7D]._regType0 = RT_A;
    g_instructions[0x7D]._regType1 = RT_L;

    g_instructions[0x7E]._instType = IN_LD;
    g_instructions[0x7E]._addrMode = AM_R_MR;
    g_instructions[0x7E]._regType0 = RT_A;
    g_instructions[0x7E]._regType1 = RT_HL;

    g_instructions[0x7F]._instType = IN_LD;
    g_instructions[0x7F]._addrMode = AM_R_R;
    g_instructions[0x7F]._regType0 = RT_A;
    g_instructions[0x7F]._regType1 = RT_A;

    g_instructions[0x80]._instType = IN_ADD;
    g_instructions[0x80]._addrMode = AM_R_R;
    g_instructions[0x80]._regType0 = RT_A;
    g_instructions[0x80]._regType1 = RT_B;

    g_instructions[0x81]._instType = IN_ADD;
    g_instructions[0x81]._addrMode = AM_R_R;
    g_instructions[0x81]._regType0 = RT_A;
    g_instructions[0x81]._regType1 = RT_C;

    g_instructions[0x82]._instType = IN_ADD;
    g_instructions[0x82]._addrMode = AM_R_R;
    g_instructions[0x82]._regType0 = RT_A;
    g_instructions[0x82]._regType1 = RT_D;

    g_instructions[0x83]._instType = IN_ADD;
    g_instructions[0x83]._addrMode = AM_R_R;
    g_instructions[0x83]._regType0 = RT_A;
    g_instructions[0x83]._regType1 = RT_E;

    g_instructions[0x84]._instType = IN_ADD;
    g_instructions[0x84]._addrMode = AM_R_R;
    g_instructions[0x84]._regType0 = RT_A;
    g_instructions[0x84]._regType1 = RT_H;

    g_instructions[0x85]._instType = IN_ADD;
    g_instructions[0x85]._addrMode = AM_R_R;
    g_instructions[0x85]._regType0 = RT_A;
    g_instructions[0x85]._regType1 = RT_L;

    g_instructions[0x86]._instType = IN_ADD;
    g_instructions[0x86]._addrMode = AM_R_MR;
    g_instructions[0x86]._regType0 = RT_A;
    g_instructions[0x86]._regType1 = RT_HL;

    g_instructions[0x87]._instType = IN_ADD;
    g_instructions[0x87]._addrMode = AM_R_R;
    g_instructions[0x87]._regType0 = RT_A;
    g_instructions[0x87]._regType1 = RT_A;

    g_instructions[0x88]._instType = IN_ADC;
    g_instructions[0x88]._addrMode = AM_R_R;
    g_instructions[0x88]._regType0 = RT_A;
    g_instructions[0x88]._regType1 = RT_B;

    g_instructions[0x89]._instType = IN_ADC;
    g_instructions[0x89]._addrMode = AM_R_R;
    g_instructions[0x89]._regType0 = RT_A;
    g_instructions[0x89]._regType1 = RT_C;

    g_instructions[0x8A]._instType = IN_ADC;
    g_instructions[0x8A]._addrMode = AM_R_R;
    g_instructions[0x8A]._regType0 = RT_A;
    g_instructions[0x8A]._regType1 = RT_D;

    g_instructions[0x8B]._instType = IN_ADC;
    g_instructions[0x8B]._addrMode = AM_R_R;
    g_instructions[0x8B]._regType0 = RT_A;
    g_instructions[0x8B]._regType1 = RT_E;

    g_instructions[0x8C]._instType = IN_ADC;
    g_instructions[0x8C]._addrMode = AM_R_R;
    g_instructions[0x8C]._regType0 = RT_A;
    g_instructions[0x8C]._regType1 = RT_H;

    g_instructions[0x8D]._instType = IN_ADC;
    g_instructions[0x8D]._addrMode = AM_R_R;
    g_instructions[0x8D]._regType0 = RT_A;
    g_instructions[0x8D]._regType1 = RT_L;

    g_instructions[0x8E]._instType = IN_ADC;
    g_instructions[0x8E]._addrMode = AM_R_MR;
    g_instructions[0x8E]._regType0 = RT_A;
    g_instructions[0x8E]._regType1 = RT_HL;

    g_instructions[0x8F]._instType = IN_ADC;
    g_instructions[0x8F]._addrMode = AM_R_R;
    g_instructions[0x8F]._regType0 = RT_A;
    g_instructions[0x8F]._regType1 = RT_A;

    g_instructions[0x90]._instType = IN_SUB;
    g_instructions[0x90]._addrMode = AM_R_R;
    g_instructions[0x90]._regType0 = RT_A;
    g_instructions[0x90]._regType1 = RT_B;

    g_instructions[0x91]._instType = IN_SUB;
    g_instructions[0x91]._addrMode = AM_R_R;
    g_instructions[0x91]._regType0 = RT_A;
    g_instructions[0x91]._regType1 = RT_C;

    g_instructions[0x92]._instType = IN_SUB;
    g_instructions[0x92]._addrMode = AM_R_R;
    g_instructions[0x92]._regType0 = RT_A;
    g_instructions[0x92]._regType1 = RT_D;

    g_instructions[0x93]._instType = IN_SUB;
    g_instructions[0x93]._addrMode = AM_R_R;
    g_instructions[0x93]._regType0 = RT_A;
    g_instructions[0x93]._regType1 = RT_E;

    g_instructions[0x94]._instType = IN_SUB;
    g_instructions[0x94]._addrMode = AM_R_R;
    g_instructions[0x94]._regType0 = RT_A;
    g_instructions[0x94]._regType1 = RT_H;

    g_instructions[0x95]._instType = IN_SUB;
    g_instructions[0x95]._addrMode = AM_R_R;
    g_instructions[0x95]._regType0 = RT_A;
    g_instructions[0x95]._regType1 = RT_L;

    g_instructions[0x96]._instType = IN_SUB;
    g_instructions[0x96]._addrMode = AM_R_MR;
    g_instructions[0x96]._regType0 = RT_A;
    g_instructions[0x96]._regType1 = RT_HL;

    g_instructions[0x97]._instType = IN_SUB;
    g_instructions[0x97]._addrMode = AM_R_R;
    g_instructions[0x97]._regType0 = RT_A;
    g_instructions[0x97]._regType1 = RT_A;

    g_instructions[0x98]._instType = IN_SBC;
    g_instructions[0x98]._addrMode = AM_R_R;
    g_instructions[0x98]._regType0 = RT_A;
    g_instructions[0x98]._regType1 = RT_B;

    g_instructions[0x99]._instType = IN_SBC;
    g_instructions[0x99]._addrMode = AM_R_R;
    g_instructions[0x99]._regType0 = RT_A;
    g_instructions[0x99]._regType1 = RT_C;

    g_instructions[0x9A]._instType = IN_SBC;
    g_instructions[0x9A]._addrMode = AM_R_R;
    g_instructions[0x9A]._regType0 = RT_A;
    g_instructions[0x9A]._regType1 = RT_D;

    g_instructions[0x9B]._instType = IN_SBC;
    g_instructions[0x9B]._addrMode = AM_R_R;
    g_instructions[0x9B]._regType0 = RT_A;
    g_instructions[0x9B]._regType1 = RT_E;

    g_instructions[0x9C]._instType = IN_SBC;
    g_instructions[0x9C]._addrMode = AM_R_R;
    g_instructions[0x9C]._regType0 = RT_A;
    g_instructions[0x9C]._regType1 = RT_H;

    g_instructions[0x9D]._instType = IN_SBC;
    g_instructions[0x9D]._addrMode = AM_R_R;
    g_instructions[0x9D]._regType0 = RT_A;
    g_instructions[0x9D]._regType1 = RT_L;

    g_instructions[0x9E]._instType = IN_SBC;
    g_instructions[0x9E]._addrMode = AM_R_MR;
    g_instructions[0x9E]._regType0 = RT_A;
    g_instructions[0x9E]._regType1 = RT_HL;

    g_instructions[0x9F]._instType = IN_SBC;
    g_instructions[0x9F]._addrMode = AM_R_R;
    g_instructions[0x9F]._regType0 = RT_A;
    g_instructions[0x9F]._regType1 = RT_A;

    //0xAX
    g_instructions[0xA0]._instType = IN_AND;
    g_instructions[0xA0]._addrMode = AM_R_R;
    g_instructions[0xA0]._regType0 = RT_A;
    g_instructions[0xA0]._regType1 = RT_B;

    g_instructions[0xA1]._instType = IN_AND;
    g_instructions[0xA1]._addrMode = AM_R_R;
    g_instructions[0xA1]._regType0 = RT_A;
    g_instructions[0xA1]._regType1 = RT_C;

    g_instructions[0xA2]._instType = IN_AND;
    g_instructions[0xA2]._addrMode = AM_R_R;
    g_instructions[0xA2]._regType0 = RT_A;
    g_instructions[0xA2]._regType1 = RT_D;

    g_instructions[0xA3]._instType = IN_AND;
    g_instructions[0xA3]._addrMode = AM_R_R;
    g_instructions[0xA3]._regType0 = RT_A;
    g_instructions[0xA3]._regType1 = RT_E;

    g_instructions[0xA4]._instType = IN_AND;
    g_instructions[0xA4]._addrMode = AM_R_R;
    g_instructions[0xA4]._regType0 = RT_A;
    g_instructions[0xA4]._regType1 = RT_H;

    g_instructions[0xA5]._instType = IN_AND;
    g_instructions[0xA5]._addrMode = AM_R_R;
    g_instructions[0xA5]._regType0 = RT_A;
    g_instructions[0xA5]._regType1 = RT_L;

    g_instructions[0xA6]._instType = IN_AND;
    g_instructions[0xA6]._addrMode = AM_R_MR;
    g_instructions[0xA6]._regType0 = RT_A;
    g_instructions[0xA6]._regType1 = RT_HL;

    g_instructions[0xA7]._instType = IN_AND;
    g_instructions[0xA7]._addrMode = AM_R_R;
    g_instructions[0xA7]._regType0 = RT_A;
    g_instructions[0xA7]._regType1 = RT_A;

    g_instructions[0xA8]._instType = IN_XOR;
    g_instructions[0xA8]._addrMode = AM_R_R;
    g_instructions[0xA8]._regType0 = RT_A;
    g_instructions[0xA8]._regType1 = RT_B;

    g_instructions[0xA9]._instType = IN_XOR;
    g_instructions[0xA9]._addrMode = AM_R_R;
    g_instructions[0xA9]._regType0 = RT_A;
    g_instructions[0xA9]._regType1 = RT_C;

    g_instructions[0xAA]._instType = IN_XOR;
    g_instructions[0xAA]._addrMode = AM_R_R;
    g_instructions[0xAA]._regType0 = RT_A;
    g_instructions[0xAA]._regType1 = RT_D;

    g_instructions[0xAB]._instType = IN_XOR;
    g_instructions[0xAB]._addrMode = AM_R_R;
    g_instructions[0xAB]._regType0 = RT_A;
    g_instructions[0xAB]._regType1 = RT_E;

    g_instructions[0xAC]._instType = IN_XOR;
    g_instructions[0xAC]._addrMode = AM_R_R;
    g_instructions[0xAC]._regType0 = RT_A;
    g_instructions[0xAC]._regType1 = RT_H;

    g_instructions[0xAD]._instType = IN_XOR;
    g_instructions[0xAD]._addrMode = AM_R_R;
    g_instructions[0xAD]._regType0 = RT_A;
    g_instructions[0xAD]._regType1 = RT_L;

    g_instructions[0xAE]._instType = IN_XOR;
    g_instructions[0xAE]._addrMode = AM_R_MR;
    g_instructions[0xAE]._regType0 = RT_A;
    g_instructions[0xAE]._regType1 = RT_HL;

    g_instructions[0xAF]._instType = IN_XOR;
    g_instructions[0xAF]._addrMode = AM_R_R;
    g_instructions[0xAF]._regType0 = RT_A;
    g_instructions[0xAF]._regType1 = RT_A;
    
    //0xBX
    g_instructions[0xB0]._instType = IN_OR;
    g_instructions[0xB0]._addrMode = AM_R_R;
    g_instructions[0xB0]._regType0 = RT_A;
    g_instructions[0xB0]._regType1 = RT_B;

    g_instructions[0xB1]._instType = IN_OR;
    g_instructions[0xB1]._addrMode = AM_R_R;
    g_instructions[0xB1]._regType0 = RT_A;
    g_instructions[0xB1]._regType1 = RT_C;

    g_instructions[0xB2]._instType = IN_OR;
    g_instructions[0xB2]._addrMode = AM_R_R;
    g_instructions[0xB2]._regType0 = RT_A;
    g_instructions[0xB2]._regType1 = RT_D;

    g_instructions[0xB3]._instType = IN_OR;
    g_instructions[0xB3]._addrMode = AM_R_R;
    g_instructions[0xB3]._regType0 = RT_A;
    g_instructions[0xB3]._regType1 = RT_E;

    g_instructions[0xB4]._instType = IN_OR;
    g_instructions[0xB4]._addrMode = AM_R_R;
    g_instructions[0xB4]._regType0 = RT_A;
    g_instructions[0xB4]._regType1 = RT_H;

    g_instructions[0xB5]._instType = IN_OR;
    g_instructions[0xB5]._addrMode = AM_R_R;
    g_instructions[0xB5]._regType0 = RT_A;
    g_instructions[0xB5]._regType1 = RT_L;

    g_instructions[0xB6]._instType = IN_OR;
    g_instructions[0xB6]._addrMode = AM_R_MR;
    g_instructions[0xB6]._regType0 = RT_A;
    g_instructions[0xB6]._regType1 = RT_HL;

    g_instructions[0xB7]._instType = IN_OR;
    g_instructions[0xB7]._addrMode = AM_R_R;
    g_instructions[0xB7]._regType0 = RT_A;
    g_instructions[0xB7]._regType1 = RT_A;

    g_instructions[0xB8]._instType = IN_CP;
    g_instructions[0xB8]._addrMode = AM_R_R;
    g_instructions[0xB8]._regType0 = RT_A;
    g_instructions[0xB8]._regType1 = RT_B;

    g_instructions[0xB9]._instType = IN_CP;
    g_instructions[0xB9]._addrMode = AM_R_R;
    g_instructions[0xB9]._regType0 = RT_A;
    g_instructions[0xB9]._regType1 = RT_C;

    g_instructions[0xBA]._instType = IN_CP;
    g_instructions[0xBA]._addrMode = AM_R_R;
    g_instructions[0xBA]._regType0 = RT_A;
    g_instructions[0xBA]._regType1 = RT_D;

    g_instructions[0xBB]._instType = IN_CP;
    g_instructions[0xBB]._addrMode = AM_R_R;
    g_instructions[0xBB]._regType0 = RT_A;
    g_instructions[0xBB]._regType1 = RT_E;

    g_instructions[0xBC]._instType = IN_CP;
    g_instructions[0xBC]._addrMode = AM_R_R;
    g_instructions[0xBC]._regType0 = RT_A;
    g_instructions[0xBC]._regType1 = RT_H;

    g_instructions[0xBD]._instType = IN_CP;
    g_instructions[0xBD]._addrMode = AM_R_R;
    g_instructions[0xBD]._regType0 = RT_A;
    g_instructions[0xBD]._regType1 = RT_L;

    g_instructions[0xBE]._instType = IN_CP;
    g_instructions[0xBE]._addrMode = AM_R_MR;
    g_instructions[0xBE]._regType0 = RT_A;
    g_instructions[0xBE]._regType1 = RT_HL;

    g_instructions[0xBF]._instType = IN_CP;
    g_instructions[0xBF]._addrMode = AM_R_R;
    g_instructions[0xBF]._regType0 = RT_A;
    g_instructions[0xBF]._regType1 = RT_A;

    //0xCX
    g_instructions[0xC0]._instType = IN_RET;
    g_instructions[0xC0]._addrMode = AM_IMP;
    g_instructions[0xC0]._regType0 = RT_NONE;
    g_instructions[0xC0]._regType1 = RT_NONE;
    g_instructions[0xC0]._conditionType = CT_NZ;

    g_instructions[0xC1]._instType = IN_POP;
    g_instructions[0xC1]._addrMode = AM_R;
    g_instructions[0xC1]._regType0 = RT_BC;

    g_instructions[0xC2]._instType = IN_JP;
    g_instructions[0xC2]._addrMode = AM_D16;
    g_instructions[0xC2]._regType0 = RT_NONE;
    g_instructions[0xC2]._regType1 = RT_NONE;
    g_instructions[0xC2]._conditionType = CT_NZ;

    g_instructions[0xC3]._instType = IN_JP;
    g_instructions[0xC3]._addrMode = AM_D16;

    g_instructions[0xC4]._instType = IN_CALL;
    g_instructions[0xC4]._addrMode = AM_D16;
    g_instructions[0xC4]._regType0 = RT_NONE;
    g_instructions[0xC4]._regType1 = RT_NONE;
    g_instructions[0xC4]._conditionType = CT_NZ;

    g_instructions[0xC5]._instType = IN_PUSH;
    g_instructions[0xC5]._addrMode = AM_R;
    g_instructions[0xC5]._regType0 = RT_BC;

    g_instructions[0xC6]._instType = IN_ADD;
    g_instructions[0xC6]._addrMode = AM_R_D8;
    g_instructions[0xC6]._regType0 = RT_A;

    g_instructions[0xC7]._instType = IN_RST;
    g_instructions[0xC7]._addrMode = AM_IMP;
    g_instructions[0xC7]._regType0 = RT_NONE;
    g_instructions[0xC7]._regType1 = RT_NONE;
    g_instructions[0xC7]._conditionType = CT_NONE;
    g_instructions[0xC7]._param = 0x00;

    g_instructions[0xC8]._instType = IN_RET;
    g_instructions[0xC8]._addrMode = AM_IMP;
    g_instructions[0xC8]._regType0 = RT_NONE;
    g_instructions[0xC8]._regType1 = RT_NONE;
    g_instructions[0xC8]._conditionType = CT_Z;

    g_instructions[0xC9]._instType = IN_RET;
    
    g_instructions[0xCA]._instType = IN_JP;
    g_instructions[0xCA]._addrMode = AM_D16;
    g_instructions[0xCA]._regType0 = RT_NONE;
    g_instructions[0xCA]._regType1 = RT_NONE;
    g_instructions[0xCA]._conditionType = CT_Z; // todo : check if this is correct

    g_instructions[0xCB]._instType = IN_CB;
    g_instructions[0xCB]._addrMode = AM_D8;

    g_instructions[0xCC]._instType = IN_CALL;
    g_instructions[0xCC]._addrMode = AM_D16;
    g_instructions[0xCC]._regType0 = RT_NONE;
    g_instructions[0xCC]._regType1 = RT_NONE;
    g_instructions[0xCC]._conditionType = CT_Z;
    
    g_instructions[0xCD]._instType = IN_CALL;
    g_instructions[0xCD]._addrMode = AM_D16;

    g_instructions[0xCE]._instType = IN_ADC;
    g_instructions[0xCE]._addrMode = AM_R_D8;
    g_instructions[0xCE]._regType0 = RT_A;

    g_instructions[0xCF]._instType = IN_RST;
    g_instructions[0xCF]._addrMode = AM_IMP;
    g_instructions[0xCF]._regType0 = RT_NONE;
    g_instructions[0xCF]._regType1 = RT_NONE;
    g_instructions[0xCF]._conditionType = CT_NONE;
    g_instructions[0xCF]._param = 0x08;
    
    //0xDX
    g_instructions[0xD0]._instType = IN_RET;
    g_instructions[0xD0]._addrMode = AM_IMP;
    g_instructions[0xD0]._regType0 = RT_NONE;
    g_instructions[0xD0]._regType1 = RT_NONE;
    g_instructions[0xD0]._conditionType = CT_NC;

    g_instructions[0xD1]._instType = IN_POP;
    g_instructions[0xD1]._addrMode = AM_R;
    g_instructions[0xD1]._regType0 = RT_DE;

    g_instructions[0xD2]._instType = IN_JP;
    g_instructions[0xD2]._addrMode = AM_D16;
    g_instructions[0xD2]._regType0 = RT_NONE;
    g_instructions[0xD2]._regType1 = RT_NONE;
    g_instructions[0xD2]._conditionType = CT_NC;

    g_instructions[0xD4]._instType = IN_CALL;
    g_instructions[0xD4]._addrMode = AM_D16;
    g_instructions[0xD4]._regType0 = RT_NONE;
    g_instructions[0xD4]._regType1 = RT_NONE;
    g_instructions[0xD4]._conditionType = CT_NC;

    g_instructions[0xD5]._instType = IN_PUSH;
    g_instructions[0xD5]._addrMode = AM_R;
    g_instructions[0xD5]._regType0 = RT_DE;

    g_instructions[0xD6]._instType = IN_SUB;
    g_instructions[0xD6]._addrMode = AM_R_D8;
    g_instructions[0xD6]._regType0 = RT_A;

    g_instructions[0xD7]._instType = IN_RST;
    g_instructions[0xD7]._addrMode = AM_IMP;
    g_instructions[0xD7]._regType0 = RT_NONE;
    g_instructions[0xD7]._regType1 = RT_NONE;
    g_instructions[0xD7]._conditionType = CT_NONE;
    g_instructions[0xD7]._param = 0x10;

    g_instructions[0xD8]._instType = IN_RET;
    g_instructions[0xD8]._addrMode = AM_IMP;
    g_instructions[0xD8]._regType0 = RT_NONE;
    g_instructions[0xD8]._regType1 = RT_NONE;
    g_instructions[0xD8]._conditionType = CT_C;

    g_instructions[0xD9]._instType = IN_RETI;

    g_instructions[0xDA]._instType = IN_JP;
    g_instructions[0xDA]._addrMode = AM_D16;
    g_instructions[0xDA]._regType0 = RT_NONE;
    g_instructions[0xDA]._regType1 = RT_NONE;
    g_instructions[0xDA]._conditionType = CT_C;

    g_instructions[0xDC]._instType = IN_CALL;
    g_instructions[0xDC]._addrMode = AM_D16;
    g_instructions[0xDC]._regType0 = RT_NONE;
    g_instructions[0xDC]._regType1 = RT_NONE;
    g_instructions[0xDC]._conditionType = CT_C;

    g_instructions[0xDE]._instType = IN_SBC;
    g_instructions[0xDE]._addrMode = AM_R_D8;
    g_instructions[0xDE]._regType0 = RT_A;

    g_instructions[0xDF]._instType = IN_RST;
    g_instructions[0xDF]._addrMode = AM_IMP;
    g_instructions[0xDF]._regType0 = RT_NONE;
    g_instructions[0xDF]._regType1 = RT_NONE;
    g_instructions[0xDF]._conditionType = CT_NONE;
    g_instructions[0xDF]._param = 0x18;

    //0xEX
    g_instructions[0xE0]._instType = IN_LDH;
    g_instructions[0xE0]._addrMode = AM_A8_R;
    g_instructions[0xE0]._regType0 = RT_NONE;
    g_instructions[0xE0]._regType1 = RT_A;

    g_instructions[0xE1]._instType = IN_POP;
    g_instructions[0xE1]._addrMode = AM_R;
    g_instructions[0xE1]._regType0 = RT_HL;

    g_instructions[0xE2]._instType = IN_LD;
    g_instructions[0xE2]._addrMode = AM_MR_R;
    g_instructions[0xE2]._regType0 = RT_C;
    g_instructions[0xE2]._regType1 = RT_A;

    g_instructions[0xE5]._instType = IN_PUSH;
    g_instructions[0xE5]._addrMode = AM_R;
    g_instructions[0xE5]._regType0 = RT_HL;

    g_instructions[0xE6]._instType = IN_AND;
    g_instructions[0xE6]._addrMode = AM_R_D8;
    g_instructions[0xE6]._regType0 = RT_A;
    
    g_instructions[0xE7]._instType = IN_RST;
    g_instructions[0xE7]._addrMode = AM_IMP;
    g_instructions[0xE7]._regType0 = RT_NONE;
    g_instructions[0xE7]._regType1 = RT_NONE;
    g_instructions[0xE7]._conditionType = CT_NONE;
    g_instructions[0xE7]._param = 0x20;

    g_instructions[0xE8]._instType = IN_ADD;
    g_instructions[0xE8]._addrMode = AM_R_D8;
    g_instructions[0xE8]._regType0 = RT_SP;

    g_instructions[0xE9]._instType = IN_JP;
    g_instructions[0xE9]._addrMode = AM_R;
    g_instructions[0xE9]._regType0 = RT_HL;

    g_instructions[0xEA]._instType = IN_LD;
    g_instructions[0xEA]._addrMode = AM_A16_R;
    g_instructions[0xEA]._regType0 = RT_NONE;
    g_instructions[0xEA]._regType1 = RT_A;

    g_instructions[0xEE]._instType = IN_XOR;
    g_instructions[0xEE]._addrMode = AM_R_D8;
    g_instructions[0xEE]._regType0 = RT_A;

    g_instructions[0xEF]._instType = IN_RST;
    g_instructions[0xEF]._addrMode = AM_IMP;
    g_instructions[0xEF]._regType0 = RT_NONE;
    g_instructions[0xEF]._regType1 = RT_NONE;
    g_instructions[0xEF]._conditionType = CT_NONE;
    g_instructions[0xEF]._param = 0x28;
    
    //0xFX
    g_instructions[0xF0]._instType = IN_LDH;
    g_instructions[0xF0]._addrMode = AM_R_A8;
    g_instructions[0xF0]._regType0 = RT_A;

    g_instructions[0xF1]._instType = IN_POP;
    g_instructions[0xF1]._addrMode = AM_R;
    g_instructions[0xF1]._regType0 = RT_AF;

    g_instructions[0xF2]._instType = IN_LD;
    g_instructions[0xF2]._addrMode = AM_R_MR;
    g_instructions[0xF2]._regType0 = RT_A;
    g_instructions[0xF2]._regType1 = RT_C;

    g_instructions[0xF3]._instType = IN_DI;

    g_instructions[0xF5]._instType = IN_PUSH;
    g_instructions[0xF5]._addrMode = AM_R;
    g_instructions[0xF5]._regType0 = RT_AF;

    g_instructions[0xF6]._instType = IN_OR;
    g_instructions[0xF6]._addrMode = AM_R_D8;
    g_instructions[0xF6]._regType0 = RT_A;

    g_instructions[0xF7]._instType = IN_RST;
    g_instructions[0xF7]._addrMode = AM_IMP;
    g_instructions[0xF7]._regType0 = RT_NONE;
    g_instructions[0xF7]._regType1 = RT_NONE;
    g_instructions[0xF7]._conditionType = CT_NONE;
    g_instructions[0xF7]._param = 0x30;

    g_instructions[0xF8]._instType = IN_LD;
    g_instructions[0xF8]._addrMode = AM_HL_SPR;
    g_instructions[0xF8]._regType0 = RT_HL;
    g_instructions[0xF8]._regType1 = RT_SP;

    g_instructions[0xF9]._instType = IN_LD;
    g_instructions[0xF9]._addrMode = AM_R_R;
    g_instructions[0xF9]._regType0 = RT_SP;
    g_instructions[0xF9]._regType1 = RT_HL;

    g_instructions[0xFA]._instType = IN_LD;
    g_instructions[0xFA]._addrMode = AM_R_A16;
    g_instructions[0xFA]._regType0 = RT_A;

    g_instructions[0xFB]._instType = IN_EI;

    g_instructions[0xFE]._instType = IN_CP;
    g_instructions[0xFE]._addrMode = AM_R_D8;
    g_instructions[0xFE]._regType0 = RT_A;

    g_instructions[0xFF]._instType = IN_RST;
    g_instructions[0xFF]._addrMode = AM_IMP;
    g_instructions[0xFF]._regType0 = RT_NONE;
    g_instructions[0xFF]._regType1 = RT_NONE;
    g_instructions[0xFF]._conditionType = CT_NONE;
    g_instructions[0xFF]._param = 0x38;
}

const char* getInstructionName(InstructionType t)
{
    return g_instructionTable[t];
}

Instruction* getInstructionByOpcode(u8 opcode)
{
    return &g_instructions[opcode];
}