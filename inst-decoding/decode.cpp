#include <fstream>
#include <string>
#include <iostream>

int main()
{
    std::string line;
    std::ifstream file("./input/input01", std::ios::in | std::ios::binary);

    unsigned char c = file.get();
    unsigned char d = file.get();
}
