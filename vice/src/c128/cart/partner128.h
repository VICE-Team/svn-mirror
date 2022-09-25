
#ifndef PARTNER128_H_
#define PARTNER128_H_

extern int partner128_crt_attach(FILE *fd, uint8_t *rawcart);
extern int partner128_bin_attach(const char *filename, uint8_t *rawcart);
extern void partner128_detach(void);

extern void partner128_config_setup(uint8_t *rawcart);

#endif
