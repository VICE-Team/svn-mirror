/*
 *	Functions for drawing String's with tab characters in them
 */

extern void XfwfDrawImageString(Display *display, Drawable drawable, GC gc, int x, int y, String string, int length, int *tabs);
extern void XfwfDrawString(Display *display, Drawable drawable, GC gc, int x, int y, String string, int length, int *tabs);
extern int *XfwfTablist2Tabs(char *tablist);
extern int XfwfTextWidth(XFontStruct *font, String str, int length, int *tabs);
extern char *strnchr(char *s, int c, int n);

