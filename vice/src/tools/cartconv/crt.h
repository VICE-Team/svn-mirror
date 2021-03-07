

#ifndef CRT_H_
#define CRT_H_

#define CRT_HEADER_LEN      0x40
#define CRT_NAME_LEN        0x10

#define CRT_CHIP_OFFS_TYPE_HI    0x08
#define CRT_CHIP_OFFS_TYPE_LO    0x09
#define CRT_CHIP_OFFS_BANK_HI    0x0a
#define CRT_CHIP_OFFS_BANK_LO    0x0b
#define CRT_CHIP_OFFS_LOAD_HI    0x0c
#define CRT_CHIP_OFFS_LOAD_LO    0x0d
#define CRT_CHIP_OFFS_SIZE_HI    0x0e
#define CRT_CHIP_OFFS_SIZE_LO    0x0f

#define CRT_CHIP_ROM        0
#define CRT_CHIP_RAM        1
#define CRT_CHIP_FLASH      2

int read_crt_header(char *filename);
int write_crt_header(unsigned char gameline, unsigned char exromline);
int write_chip_package(unsigned int length, unsigned int bank, unsigned int address, unsigned char type);

extern unsigned char headerbuffer[CRT_HEADER_LEN];

#endif
