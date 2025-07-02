#include <fstream>
#include <string>
#include <iostream>
#include <bitset>

using u8 = unsigned char;

// reg table
const u8 REG_AX = 0b000; // 0x00
const u8 REG_CX = 0b001; // 0x01
const u8 REG_DX = 0b010; // 0x02
const u8 REG_BX = 0b011; // 0x04
const u8 REG_SP = 0b100; // 0x08
const u8 REG_BP = 0b101; // 0x10
const u8 REG_SI = 0b110; // 0x11
const u8 REG_DI = 0b111; // 0x12

std::string getRegisterOperandDecoded(u8 _3bitRegOperand, bool wbit)
{
    if (!wbit)
    {
        switch (_3bitRegOperand)
        {
        case REG_AX:
            return "al";
        case REG_CX:
            return "cl";
        case REG_DX:
            return "dl";
        case REG_BX:
            return "bl";
        case REG_SP:
            return "ah";
        case REG_BP:
            return "ch";
        case REG_SI:
            return "dh";
        case REG_DI:
            return "bh";
        }
    }
    else
    {
        switch (_3bitRegOperand)
        {
        case REG_AX:
            return "ax";
        case REG_CX:
            return "cx";
        case REG_DX:
            return "dx";
        case REG_BX:
            return "bx";
        case REG_SP:
            return "sp";
        case REG_BP:
            return "bp";
        case REG_SI:
            return "si";
        case REG_DI:
            return "di";
        }
    }
}

int main()
{

    u8 MV_RM_2_R = 0b10001000;
    u8 MV_IM_2_RM = 0b11000110;

    std::string line;
    std::ifstream file("./input/input02", std::ios::in | std::ios::binary);
    std::fstream out("./output/input02_disassembled.asm", std::ios::out);

    out << "bits 16\r\n\r\n";

    // byte 1 masks
    const u8 OPCODE_MASK = 0b11111100;
    const bool DBIT_MASK = 0b00000010; // direction
    const bool WBIT_MASK = 0b00000001; // 1: word (16 bit), 0: byte (8 bit)

    // byte 2 masks
    const u8 MOD_MASK = 0b11000000;
    const u8 REG_MASK = 0b00111000;
    const u8 RM_MASK = 0b00000111;

    const u8 MOD_REGISTER_MODE = 0b11000000;

    int safetyCounter = 0;

    while (true)
    {
        ++safetyCounter;
        if (safetyCounter > 100)
        {
            break;
        }

        u8 byte1 = file.get();
        if (byte1 == (u8)-1)
        {
            break;
        }

        u8 byte2 = file.get();
        if (byte2 == (u8)-1)
        {
            break;
        }

        // process byte1

        if ((OPCODE_MASK & byte1) == MV_RM_2_R)
        {
            out << "mov ";
        }
        const bool wbit = byte1 & WBIT_MASK;
        const bool dbit = byte1 & DBIT_MASK;

        // process byte2

        const u8 modVal = byte2 & MOD_MASK;
        const u8 regVal = (byte2 & REG_MASK) >> 3;
        const u8 rmVal = byte2 & RM_MASK;

        // only supporting register mode
        if (modVal == MOD_REGISTER_MODE)
        {
            out << getRegisterOperandDecoded(rmVal, wbit) << ", ";
            out << getRegisterOperandDecoded(regVal, wbit) << "\r\n";
        }
    }
}
