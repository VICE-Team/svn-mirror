

#ifndef CRT_H_
#define CRT_H_

#define CRT_HEADER_LEN      0x40
#define CRT_NAME_LEN        0x10

#define CRT_CHIP_ROM        0
#define CRT_CHIP_RAM        1
#define CRT_CHIP_FLASH      2

int read_crt_header(char *filename);
int write_crt_header(unsigned char gameline, unsigned char exromline);
int write_chip_package(unsigned int length, unsigned int bank, unsigned int address, unsigned char type);

extern unsigned char headerbuffer[CRT_HEADER_LEN];

#endif
