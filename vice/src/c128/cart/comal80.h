
#ifndef C128_COMAL80_H_
#define C128_COMAL80_H_

extern int c128comal80_bin_attach(const char *filename, uint8_t *rawcart);
extern int c128comal80_crt_attach(FILE *fd, uint8_t *rawcart);
extern void c128comal80_detach(void);

extern void c128comal80_config_setup(uint8_t *rawcart);

#endif
