
#ifndef C128_GENERIC_H_
#define C128_GENERIC_H_

extern int c128generic_bin_attach(const char *filename, uint8_t *rawcart);
extern int c128generic_crt_attach(FILE *fd, uint8_t *rawcart);
extern void c128generic_detach(void);

extern void c128generic_config_setup(uint8_t *rawcart);

#endif
