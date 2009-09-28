/* qnxshm.h
 *
 * System V Shared Memory Emulation for QNX
 *
 */

#ifndef VICE_QNXSHM_H
#define VICE_QNXSHM_H

#include "qnxipc.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SHM_R 0400
#define SHM_W 0200

struct shmid_ds {
    int dummy;
    int shm_nattch;
};

extern void *shmat(int shmid, const void *shmaddr, int shmflg);
extern int shmdt(const void *addr);
extern int shmctl(int shmid, int cmd, struct shmid_ds * buf);
extern int shmget(key_t key, size_t size, int flags);

#ifdef __cplusplus
}
#endif

#endif
