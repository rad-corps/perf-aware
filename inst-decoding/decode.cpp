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

    while (u8 byte1 = file.get())
    {
        if (byte1 == -1)
        {
            break;
        }

        u8 OPCODE_MASK = byte1 & 0b11111100;
        bool DBIT_MASK = byte1 & 0b00000010;
        bool WBIT_MASK = byte1 & 0b00000001;

        if ((OPCODE_MASK & byte1) == MV_RM_2_R)
        {
            out << "mov ";
        }
        byte1 = file.get();
    }
}
