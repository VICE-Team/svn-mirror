#include "vice.h"

#ifdef GP2X
#include "gp2x/uimenu.h"
#else
#ifdef USE_GNOMEUI
#include "x11/gnome/uimenu.h"
#else
#include "x11/xaw/uimenu.h"
#endif
#endif
