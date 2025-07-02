#include <fstream>
#include <string>
#include <iostream>
#include <bitset>

int main()
{
    using u8 = unsigned char;

    u8 MV_RM_2_R = 0b10001000;
    u8 MV_IM_2_RM = 0b11000110;

    std::string line;
    std::ifstream file("./input/input01", std::ios::in | std::ios::binary);
    std::fstream out("./input/out01.asm", std::ios::out);

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

    // reg table w=0
    const u8 REG_AL = 0;
    const u8 REG_CL = 0b0010000;
    const u8 REG_DL = 0b0100000;
    const u8 REG_BL = 0b0110000;
    const u8 REG_AH = 0b1000000;
    const u8 REG_CH = 0b1010000;
    const u8 REG_DH = 0b1100000;
    const u8 REG_BH = 0b1110000;

    // reg table w=1
    const u8 REG_AX = REG_AL;
    const u8 REG_CX = REG_CL;
    const u8 REG_DX = REG_DL;
    const u8 REG_BX = REG_BL;
    const u8 REG_SP = REG_AH;
    const u8 REG_BP = REG_CH;
    const u8 REG_SI = REG_DH;
    const u8 REG_DI = REG_BH;

    while (u8 byte1 = file.get())
    {
        if (byte1 == -1)
        {
            break;
        }

        if ((OPCODE_MASK & byte1) == MV_RM_2_R)
        {
            out << "mov ";

            // get the 2nd byte
            u8 byte2 = file.get();
        }
    }
}
