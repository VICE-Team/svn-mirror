
#include "cartridge.h"

#include "cartconv.h"
#include "crt.h"
#include "vic20-saver.h"

/* this table must be in correct order so it can be indexed by CRT ID */
/*
    exrom, game, sizes, bank size, load addr, num banks, data type, name, option, saver

    num banks == 0 - take number of banks from input file size

    exrom/game are always 0 for vic20
*/
const cart_t cart_info_vic20[] = {

    {0, 0, CARTRIDGE_SIZE_2KB |
           CARTRIDGE_SIZE_4KB |
           CARTRIDGE_SIZE_8KB |
           CARTRIDGE_SIZE_12KB |
           CARTRIDGE_SIZE_16KB,    0x2000, 0xa000,   0, CRT_CHIP_ROM, "Generic VIC20 Cartridge",            "vic20", save_generic_vic20_crt},
    {0, 0, CARTRIDGE_SIZE_2MB,     0x2000, 0xa000, 256, CRT_CHIP_ROM, CARTRIDGE_VIC20_NAME_MEGACART,         "mega", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_2MB,     0x2000, 0xa000, 256, CRT_CHIP_ROM, CARTRIDGE_VIC20_NAME_BEHRBONZ,           "bb", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_4MB,     0x2000, 0xa000, 512, CRT_CHIP_ROM, CARTRIDGE_VIC20_NAME_FP,              "vicfp", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_512KB |
           CARTRIDGE_SIZE_8MB |
           CARTRIDGE_SIZE_16MB,    0x2000, 0xa000,   0, CRT_CHIP_ROM, CARTRIDGE_VIC20_NAME_UM,                 "um", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_512KB,   0x2000, 0xa000,  64, CRT_CHIP_ROM, CARTRIDGE_VIC20_NAME_FINAL_EXPANSION,   "fe3", save_regular_crt},
    {0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
};

