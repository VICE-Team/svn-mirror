
#ifndef WARPSPEED128_H_
#define WARPSPEED128_H_

extern int warpspeed128_crt_attach(FILE *fd, uint8_t *rawcart);
extern int warpspeed128_bin_attach(const char *filename, uint8_t *rawcart);
extern void warpspeed128_detach(void);

extern void warpspeed128_config_setup(uint8_t *rawcart);

#endif
