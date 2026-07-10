#include "../src/ini.hpp"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        return 0;
    }
    auto cfg = read_ini(argv[1]);
}