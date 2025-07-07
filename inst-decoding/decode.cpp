#include <fstream>
#include <string>
#include <iostream>
#include <bitset>
#include <sstream>
#include <exception>

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

// effective address calculator
//         MOD 00           MOD 01          MOD 10
// RM 000  BX+SI            +8               +16
// RM 001  BX+DI            +8               +16
// RM 010  BP+SI            +8               +16
// RM 011  BP+DI            +8               +16
// RM 100  SI               +8               +16
// RM 101  DI               +8               +16
// RM 110  DIRECT (BP) + 8  +8               +16
// RM 111  BX               +8               +16

// register address calculator
// REG      W=0     W=1
// 000      AL      AX
// 001      CL      CX
// 010      DL      DX
// 011      BL      BX
// 100      AH      SP
// 101      CH      BP
// 110      DH      SI
// 111      BH      DI

// instruction encoding
// MOV = Move
// Reg/memory to/from reg
// 100010dw|mod reg rm|displo|disphi
//           2   3   3
std::string getEffectiveAddressOperandDecoded(u8 rm, u8 mod, std::ifstream &file)
{
    std::stringstream ss;

    switch (rm)
    {
    case 0b000:
        ss << "[bx + si";
        break;
    case 0b001:
        ss << "[bx + di";
        break;
    case 0b010:
        ss << "[bp + si";
        break;
    case 0b011:
        ss << "[bp + di";
        break;
    case 0b100:
        ss << "[si";
        break;
    case 0b101:
        ss << "[di";
        break;
    case 0b110:
        ss << "[bp";
        break;
    case 0b111:
        ss << "[bx";
        break;
    }

    if (mod == 0b00)
    {
        if (rm == 0b110) // special case
        {
            // throw the extra byte away
            file.get();
        }

        ss << "]";
    }
    else if (mod == 0b01)
    {
        u8 value = file.get();
        ss << " + " << (int)value << "]";
    }
    else if (mod == 0b10)
    {
        uint16_t value;
        file.read(reinterpret_cast<char *>(&value), sizeof(value));
        ss << " + " << (int)value << "]";
    }
    else
    {
        throw std::exception();
    }

    return ss.str();
}

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
    return "";
}

void regMemToFromReg(u8 byte1, std::ifstream &file, std::fstream &out)
{
    u8 byte2 = file.get();
    if (byte2 == (u8)-1)
    {
        // error
        return;
    }

    // byte 1 masks
    const u8 DBIT_MASK = 0b00000010; // direction
    const u8 WBIT_MASK = 0b00000001; // 1: word (16 bit), 0: byte (8 bit)

    // byte 2 masks
    const u8 MOD_MASK = 0b11000000;
    const u8 REG_MASK = 0b00111000;
    const u8 RM_MASK = 0b00000111;

    // valid MODs (Modes)
    const u8 MOD_MEMORY_MODE_NO_DISPLACEMENT = 0b00000000;
    const u8 MOD_MEMORY_MODE_8_DISPLACEMENT = 0b01000000;
    const u8 MOD_MEMORY_MODE_16_DISPLACEMENT = 0b10000000;
    const u8 MOD_REGISTER_MODE = 0b00000011;

    const bool wbit = (byte1 & WBIT_MASK) == WBIT_MASK;
    const bool dbit = (byte1 & DBIT_MASK) == DBIT_MASK;

    // process byte2
    const u8 modVal = (byte2 & MOD_MASK) >> 6;
    const u8 regVal = (byte2 & REG_MASK) >> 3;
    const u8 rmVal = byte2 & RM_MASK;

    // only supporting register mode
    if (modVal == MOD_REGISTER_MODE)
    {
        out << getRegisterOperandDecoded(rmVal, wbit) << ", ";
        out << getRegisterOperandDecoded(regVal, wbit) << "\r\n";
    }
    else
    {
        if (!dbit)
        {
            out << getEffectiveAddressOperandDecoded(rmVal, modVal, file) << ", ";
            out << getRegisterOperandDecoded(regVal, wbit) << "\r\n";
        }
        else
        {
            out << getRegisterOperandDecoded(regVal, wbit) << ", ";
            out << getEffectiveAddressOperandDecoded(rmVal, modVal, file) << "\r\n";
        }
    }
}

void movRegMemToFromReg(u8 byte1, std::ifstream &file, std::fstream &out)
{
    out << "mov ";
    regMemToFromReg(byte1, file, out);
}

void addRegMemToFromReg(u8 byte1, std::ifstream &file, std::fstream &out)
{
    out << "add ";
    regMemToFromReg(byte1, file, out);
}

void immediateToReg(u8 byte1, std::ifstream &file, std::fstream &out)
{
    const u8 WBIT_MASK = 0b00001000; // 1: word (16 bit), 0: byte (8 bit)
    const u8 REG_MASK = 0b00000111;

    const bool wbit = (byte1 & WBIT_MASK) == WBIT_MASK;
    out << getRegisterOperandDecoded(byte1 & REG_MASK, wbit) << ", ";

    if (wbit)
    {
        uint16_t value;
        file.read(reinterpret_cast<char *>(&value), sizeof(value));
        out << (int)value << "\r\n";
    }
    else
    {
        const u8 value = file.get();
        out << (int)value << "\r\n";
    }
}

void movImmediateToReg(u8 byte1, std::ifstream &file, std::fstream &out)
{
    out << "mov ";
    immediateToReg(byte1, file, out);
}

void addImmediateToReg(u8 byte1, std::ifstream &file, std::fstream &out)
{
    out << "add ";
    immediateToReg(byte1, file, out);
}

void reassembleAndCompare(const std::string &inputBinaryFileName, const std::string &disassembledFileName)
{
    // reassemble the disassembly
    const std::string nasmCommand = std::string{"nasm "} + disassembledFileName;
    int ret = std::system(nasmCommand.c_str());

    if (ret == 0)
    {
        const std::string reassembledFileName = disassembledFileName.substr(0, disassembledFileName.length() - 4);
        const std::string diffCommand = std::string{"diff "} + inputBinaryFileName + std::string{" "} + reassembledFileName;
        int diffResult = std::system(diffCommand.c_str());
        if (diffResult != 0)
        {
            std::cout << "input did not match dissassembly" << std::endl;
        }
        else
        {
            std::cout << "success!" << std::endl;
        }
    }
    else
    {
        std::cerr << "Compilation failed with error code: " << ret << std::endl;
    }
}

int main(int count, char **args)
{
    if (count != 2)
    {
        std::cout << "supply an 8086 assembly file input argument" << std::endl;
        return 1;
    }
    const std::string inputBinaryFileName = args[1];

    // mov register to register OR memory to register
    const u8 MV_REG_MEM_TO_FROM_REG = 0b10001000;
    const u8 REG_MEM_TO_FROM_REG_MASK = 0b11111100;

    // immediate to register OR immediate to memory
    const u8 IM_TO_REG_MEM_MASK = 0b11111110;
    const u8 MV_IM_TO_REG_MEM = 0b11000110;

    // immediate to register
    const u8 MV_IM_TO_REG = 0b10110000;
    const u8 IM_TO_REG_MASK = 0b11110000;

    std::string line;
    std::ifstream file(inputBinaryFileName, std::ios::in | std::ios::binary);
    const std::string disassembledFileName = inputBinaryFileName + std::string{"_disassembled.asm"};
    const std::string reassembledFileName = inputBinaryFileName + std::string{"_disassembled"};
    std::fstream out(inputBinaryFileName + std::string{"_disassembled.asm"}, std::ios::out);

    out << "bits 16\r\n\r\n";

    while (true)
    {
        u8 byte1 = file.get();
        if (byte1 == (u8)-1)
        {
            break;
        }

        // process byte1
        if ((REG_MEM_TO_FROM_REG_MASK & byte1) == MV_REG_MEM_TO_FROM_REG)
        {
            movRegMemToFromReg(byte1, file, out);
        }
        else if ((IM_TO_REG_MASK & byte1) == MV_IM_TO_REG)
        {
            movImmediateToReg(byte1, file, out);
        }
    }
    out.close();

    reassembleAndCompare(inputBinaryFileName, disassembledFileName);
}
