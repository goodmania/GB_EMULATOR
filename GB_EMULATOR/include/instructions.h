#pragma once

#include "common.h"

/**
 * @enum AddressingMode
 * @brief Enumerates the different addressing modes for instructions.
 */
enum AddressingMode{
    AM_IMP,     /**< Implied addressing mode */
    AM_R_D16,   /**< to Register from 16-bit immediate addressing mode */
    AM_R_R,     /**< Register to register addressing mode */
    AM_MR_R,    /**< Memory register from register addressing mode */
    AM_R,       /**< Register addressing mode */
    AM_R_D8,    /**< Register from 8-bit immediate addressing mode */
    AM_R_MR,    /**< Register from memory register addressing mode */
    AM_R_HLI,   /**< Register from HL indirect addressing mode */
    AM_R_HLD,   /**< Register from HL indirect decrement addressing mode */
    AM_HLI_R,   /**< HL indirect addressing mode from register */
    AM_HLD_R,   /**< HL indirect decrement addressing mode from register */
    AM_R_A8,    /**< Register from 8-bit immediate address addressing mode */
    AM_A8_R,    /**< 8-bit immediate address from register addressing mode */
    AM_HL_SPR,  /**< HL register from stack pointer addressing mode */
    AM_D16,     /**< 16-bit immediate addressing mode */
    AM_D8,      /**< 8-bit immediate addressing mode */
    AM_D16_R,   /**< 16-bit immediate from register addressing mode */
    AM_MR_D8,   /**< Memory register from 8-bit immediate addressing mode */
    AM_MR,      /**< Memory register addressing mode */
    AM_A16_R,   /**< 16-bit immediate address from register addressing mode */
    AM_R_A16    /**< Register from 16-bit immediate address addressing mode */
};

/**
 * @enum RegisterType
 * @brief Enumerates the different register types.
 */
enum RegisterType : u16 {
    RT_NONE,    /**< No register */
    RT_A,       /**< Register A */
    RT_F,       /**< Register F */
    RT_B,       /**< Register B */
    RT_C,       /**< Register C */
    RT_D,       /**< Register D */
    RT_E,       /**< Register E */
    RT_H,       /**< Register H */
    RT_L,       /**< Register L */
    RT_AF,      /**< Register AF */
    RT_BC,      /**< Register BC */
    RT_DE,      /**< Register DE */
    RT_HL,      /**< Register HL */
    RT_SP,      /**< Stack Pointer */
    RT_PC       /**< Program Counter */
};

/**
 * @enum InstructionType
 * @brief Enumerates the different instruction types.
 */
enum InstructionType{
    IN_NONE,    /**< No instruction */
    IN_NOP,     /**< No operation instruction */
    IN_LD,      /**< Load instruction */
    IN_INC,     /**< Increment instruction */
    IN_DEC,     /**< Decrement instruction */
    IN_RLCA,    /**< Rotate left through carry instruction */
    IN_ADD,     /**< Add instruction */
    IN_RRCA,    /**< Rotate right through carry instruction */
    IN_STOP,    /**< Stop instruction */
    IN_RLA,     /**< Rotate left instruction */
    IN_JR,      /**< Jump relative instruction */
    IN_RRA,     /**< Rotate right instruction */
    IN_DAA,     /**< Decimal adjust accumulator instruction */
    IN_CPL,     /**< Complement accumulator instruction */
    IN_SCF,     /**< Set carry flag instruction */
    IN_CCF,     /**< Complement carry flag instruction */
    IN_HALT,    /**< Halt instruction */
    IN_ADC,     /**< Add with carry instruction */
    IN_SUB,     /**< Subtract instruction */
    IN_SBC,     /**< Subtract with carry instruction */
    IN_AND,     /**< Logical AND instruction */
    IN_XOR,     /**< Logical XOR instruction */
    IN_OR,      /**< Logical OR instruction */
    IN_CP,      /**< Compare instruction */
    IN_POP,     /**< Pop instruction */
    IN_JP,      /**< Jump instruction */
    IN_PUSH,    /**< Push instruction */
    IN_RET,     /**< Return instruction */
    IN_CB,      /**< CB prefix instruction */
    IN_CALL,    /**< Call instruction */
    IN_RETI,    /**< Return from interrupt instruction */
    IN_LDH,     /**< Load high immediate instruction */
    IN_JPHL,    /**< Jump to HL instruction */
    IN_DI,      /**< Disable interrupts instruction */
    IN_EI,      /**< Enable interrupts instruction */
    IN_RST,     /**< Restart instruction */
    IN_ERR,     /**< Error instruction */
    //CB instructions...
    IN_RLC,     /**< Rotate left through carry (CB prefix) instruction */
    IN_RRC,     /**< Rotate right through carry (CB prefix) instruction */
    IN_RL,      /**< Rotate left (CB prefix) instruction */
    IN_RR,      /**< Rotate right (CB prefix) instruction */
    IN_SLA,     /**< Shift left arithmetic (CB prefix) instruction */
    IN_SRA,     /**< Shift right arithmetic (CB prefix) instruction */
    IN_SWAP,    /**< Swap nibbles (CB prefix) instruction */
    IN_SRL,     /**< Shift right logical (CB prefix) instruction */
    IN_BIT,     /**< Test bit (CB prefix) instruction */
    IN_RES,     /**< Reset bit (CB prefix) instruction */
    IN_SET      /**< Set bit (CB prefix) instruction */
};

struct InstructionInfo {
    InstructionType _instType;
    AddressingMode _addrMode;
    RegisterType _regType;
};

enum ConditionType{
    CT_NONE,
    CT_NZ,
    CT_Z,
    CT_NC,
    CT_C,
};

struct Instruction {
    Instruction();
    ~Instruction() = default;

    InstructionType _instType;
    AddressingMode _addrMode;
    RegisterType _regType0;
    RegisterType _regType1;
    ConditionType _conditionType;
    u8 _param;
};

void initializeInstructions();
extern Instruction* getInstructionByOpcode(u8 opcode);
extern const char* getInstructionName(InstructionType t);