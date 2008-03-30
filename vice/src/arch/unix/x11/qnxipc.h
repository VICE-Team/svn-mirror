/* qnxipc.h
 *
 * System V IPC Emulation for QNX
 *
 */

#ifndef _QNXIPC_H
#define _QNXIPC_H

#include <sys/types.h>

#define IPC_CREAT	0001000
#define IPC_EXCL	0002000
#define IPC_NOWAIT	0004000

#define IPC_PRIVATE (key_t)0

#define IPC_RMID	0
#define IPC_STAT	1

#endif
