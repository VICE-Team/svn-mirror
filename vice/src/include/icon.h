
/* Include the correct pixmap for the emulator's icon. */

#ifdef CBM64
#include "x64_icon.xpm"
#elif defined (C128)
#include "x128_icon.xpm"
#elif defined (PET)
#include "xpet_icon.xpm"
#elif defined (VIC20)
#include "xvic_icon.xpm"
#endif
