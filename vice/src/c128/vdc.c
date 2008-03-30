
#include "vice.h"

#include "mem.h"

static int vdc_ptr;
BYTE vdc[0x100];

void REGPARM2 store_vdc(ADDRESS addr, BYTE value)
{
    if (addr & 1) {
	vdc[vdc_ptr] = value;
	if (vdc_ptr == 0x1e) {
	    vdc[0x12] = LOAD(0xa3d);
	    vdc[0x13] = LOAD(0xa3c);
	}
    } else {
	vdc_ptr = value;
    }
}

BYTE REGPARM1 read_vdc(ADDRESS addr)
{
    if (addr & 1) {
	if (vdc_ptr == 0x12)
	    return LOAD(0xa3d);
	else if (vdc_ptr == 0x13)
	    return LOAD(0xa3c);
	return ((vdc_ptr < 37) ? vdc[vdc_ptr] : 0xff);
    } else
	return 0x9f;
}
