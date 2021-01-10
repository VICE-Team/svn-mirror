

#ifndef CRT_H_
#define CRT_H_

int write_crt_header(unsigned char gameline, unsigned char exromline);
int write_chip_package(unsigned int length, unsigned int bank, unsigned int address, unsigned char type);

#endif
