
#ifndef _VDC_H
#define _VDC_H

#include "types.h"

extern void REGPARM2 store_vdc(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 read_vdc(ADDRESS addr);

#endif /* _VDC_H */
