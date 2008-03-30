/*
 * console.c - Console access interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *
 * Totally rewritten for using an own window by
 *  Spiro Trikaliotis <Spiro.Trikaliotis@gmx.de>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdarg.h>
#include <windows.h>

#include "console.h"
#include "utils.h"

#include "winmain.h"
#include "res.h"
#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 MAX_WIDTH is not allowed to be bigger than (MIN_XSIZE * MIN_YSIZE) !
*/

/*
 @SRT TODO: currently, MAX_WIDTH plus a prompt which is on the screen
 is not even allowed to be bigger than MIN_XSIZE; that's because the
 routines are not multi-line-input safe yet!
*/

#define MAX_CONSOLES         5
#define MAX_OUTPUT_LENGTH 2000 /* max. length of an output string after expansion with vsprintf() */
#define MAX_WIDTH           60 /* max. char count for an input line  */
#define MAX_HISTORY         20 /* maximum history entrys per console */

#define MIN_XSIZE           80
#define MIN_YSIZE           10

/*
 When resizing the window, the new window buffer is not reallocated for every
 one increment, but always in size of RESIZE_INCREMENT_...
 So, we don't need to reallocate very frequently and so we don't convert the 
 memory into little parts which cannot be rejoined.
*/
#define RESIZE_INCREMENT_X  40
#define RESIZE_INCREMENT_Y  20


/*
 The following structure fully describes a console window
*/
typedef struct tagTWindow
{
	console_t	console;		/* this _must_ be the first entry of this structure! */

	char		*pchWindowBuffer;	/* a buffer to the window contents */

	char		*pchName;			/*  the name of the window; 
									this is used to recognize the window when re-opening */

	char		*history[MAX_HISTORY];	/* ring buffer of pointers to the history entries */
	unsigned	 nHistory;		/* index to next entry to be written in history buffer */
	unsigned	 nCurrentSelectHistory;	/* index relative to nHistory to the entry of the 
									   history which will be shown next */

	/* the position of the cursor in the window */
	unsigned	 xPos;
	unsigned	 yPos;

	unsigned	 xMax;
	unsigned	 yMax;
	unsigned	 xMin;
	unsigned	 yMin;

	/* the dimensions of one character in the window */
	unsigned	 xCharDimension;
	unsigned	 yCharDimension;

	HWND		 hwndConsole;	/* the HWND of the window */
	HWND		 hwndPreviousActive;
	HDC			 hdc;			/* a DC for writing inside the window */

	int			 xWindow;		/* the position of the window for re-opening */
	int			 yWindow;		/* the position of the window for re-opening */
	BOOLEAN		 bInputReady;
	BOOLEAN		 bBlinkOn;

	char		 achInputBuffer[MAX_WIDTH+1];
	unsigned	 cntInputBuffer;
	unsigned	 posInputBuffer;
	BOOLEAN		 bInsertMode;

} TWindow;


static TWindow window_contents[MAX_CONSOLES];



static void add_to_history( TWindow *tw, const char *entry )
{
	if (entry[0])
	{
		if (tw->history[tw->nHistory] != NULL)
		{
			/* delete old history entry */
			free( tw->history[tw->nHistory] );
		};

		tw->history[tw->nHistory] = stralloc( entry );

		tw->nHistory = (tw->nHistory+1) % MAX_HISTORY;
	}
}

static const char *get_history_entry( TWindow *tw )
{
	return tw->history[(tw->nHistory - tw->nCurrentSelectHistory + MAX_HISTORY) % MAX_HISTORY];
	/*
	remark: "+ MAX_HISTORY" to avoid the following portability problem:
	what means -3 % 7? A compiler might output -3 or 4 at its own choice,
	which are both mathematically correct!
	*/
}


/*
 calculate a pointer into the pchWindowBuffer
*/
#define CALC_POS(xxx, yyy) (tw->xMax * (yyy) + (xxx))

/*
ifndef PPAINTSTRUCT
define LPPAINTSTRUCT PPAINTSTRUCT
endif
*/
static void redraw_window(TWindow *tw, LPPAINTSTRUCT pps)
{
	unsigned row;

	unsigned yMin = 0;
	unsigned yMax = tw->console.console_yres;

	unsigned xMin = 0;
	unsigned xMax = tw->console.console_xres;

	if (pps)
	{
		/* we have an update region, so only update necessary parts */
		xMin = pps->rcPaint.left   / tw->xCharDimension;
		yMin = pps->rcPaint.top    / tw->yCharDimension;

		/*
		 the "+ ..." force a rounding up.
		*/
		xMax = (pps->rcPaint.right  + tw->xCharDimension-1) / tw->xCharDimension;
		yMax = (pps->rcPaint.bottom + tw->yCharDimension-1) / tw->yCharDimension;
	}

	for (row = yMin; row < yMax; row++)
	{
		/* draw a single line */
		TextOut( tw->hdc, 
			xMin * tw->xCharDimension,
			row * tw->yCharDimension,
			&(tw->pchWindowBuffer[CALC_POS(xMin,row)]), 
			xMax
			);
	}
}



static void move_upwards( TWindow *tw )
{
	--tw->yPos;
}

static void scroll_up( TWindow *tw )
{
	/* move all lines one line up */
	memmove( tw->pchWindowBuffer,
		&tw->pchWindowBuffer[tw->xMax],
		CALC_POS(0,tw->yMax-1)
		);

	/* clear the last line */
	memset(&tw->pchWindowBuffer[CALC_POS(0,tw->yMax-1)],
		' ',
		tw->xMax
		);

	move_upwards(tw);

	/* force repainting of the whole window */
#if 0
	/*
	 @SRT this variant takes less processor time because
	 hopefully, the window needs not to be updated with
	 every single scroll.
	*/
	InvalidateRect( tw->hwndConsole, NULL, FALSE );
#else
	/*
	 @SRT this variant looks more realistic since every
	 single scroll can be been by the user
	*/
	redraw_window( tw, NULL );
#endif
}

static void move_downwards( TWindow *tw )
{
	if (++tw->yPos == tw->console.console_yres)
	{
		/* we must scroll the window */
		scroll_up(tw);
	}
}

static void move_backwards( TWindow *tw )
{
	if (--tw->xPos < 0)
	{
		tw->xPos = tw->console.console_xres-1;
		move_upwards(tw);
	}
}

static void move_forwards( TWindow *tw )
{
	if (++tw->xPos >= tw->console.console_xres)
	{
		tw->xPos = 0;
		move_downwards(tw);
	}
}

/*
 calculate the dimensions of one character 
 and set TWindow structure accordingly
*/
static void get_char_dimensions( TWindow *tw )
{
	SIZE size;

	GetTextExtentPoint32( tw->hdc, " ", 1, &size );

	tw->xCharDimension  = size.cx;
	tw->yCharDimension = size.cy; 
}


static void resize_window( TWindow *tw )
{
	RECT rect;

	GetClientRect( tw->hwndConsole, &rect );

	ClientToScreen( tw->hwndConsole,  (LPPOINT) &rect);
	ClientToScreen( tw->hwndConsole, ((LPPOINT) &rect) + 1);

	rect.right  = rect.left + tw->console.console_xres * tw->xCharDimension;
	rect.bottom = rect.top  + tw->console.console_yres * tw->yCharDimension;

	AdjustWindowRect( &rect, GetWindowLong( tw->hwndConsole, GWL_STYLE ), FALSE );

	MoveWindow( tw->hwndConsole, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top, TRUE );

	tw->xWindow = rect.left;
	tw->yWindow = rect.top;
}


/*
 allocate memory for the window buffer 
 and initializie TWindow structure
*/
static TWindow *allocate_window_memory( TWindow* tw )
{
	if (!tw->pchWindowBuffer)
	{
		char *p;
		unsigned n;

		n = tw->xMax * tw->yMax;

		/* allocate buffer for window contents */
		p                   =
		tw->pchWindowBuffer = xmalloc(sizeof(char) * n );

		/* clear the buffer with spaces */
		for (n; n>0; n--)
			*p++ = ' ';

	}

	return tw;
}

static TWindow *reallocate_window_memory( TWindow* tw, unsigned xDim, unsigned yDim )
{
	unsigned xOldDim = tw->xMax;
	unsigned yOldDim = tw->yMax;
	char *pOldBuffer = tw->pchWindowBuffer;

	unsigned y;

	/* get new memory buffer */
	tw->pchWindowBuffer = NULL;
	tw->xMax            = xDim+RESIZE_INCREMENT_X;
	tw->yMax            = yDim+RESIZE_INCREMENT_Y;

	allocate_window_memory( tw );

	/* now, copy the contents of the old buffer into the new one */
	for (y=0; y<yOldDim; y++)
	{
		memmove(&tw->pchWindowBuffer[CALC_POS(0,y)],
			&pOldBuffer[xOldDim * y],
			xOldDim);
	}

	/* we're done, release the old buffer */
	free( pOldBuffer );

	return tw;
}


static void free_window_memory( TWindow *tw )
{
	if (tw->pchWindowBuffer)
	{
		free( tw->pchWindowBuffer);
		tw->pchWindowBuffer = NULL;
	}

	if (tw->pchName)
	{
		free( tw->pchName );
	}
}


static void draw_character( TWindow *tw, char ch )
{
	TextOut( tw->hdc,
		tw->xPos * tw->xCharDimension,
		tw->yPos * tw->yCharDimension,
		&ch,
		1
		);
}


static void draw_current_character( TWindow *tw )
{
	TextOut( tw->hdc,
		tw->xPos * tw->xCharDimension,
		tw->yPos * tw->yCharDimension,
		&tw->pchWindowBuffer[CALC_POS(tw->xPos, tw->yPos)],
		1
		);
}


static void restore_current_character( TWindow *tw )
{
	/* remark: in general, we don't need to bother 
	   synchronizing with the blink timer, because the 
	   timer cannot due to the cooperative multi-
	   tasking!
	*/

	tw->bBlinkOn = FALSE;
	draw_current_character( tw );
}

static void paint_cursor( TWindow *tw )
{
	int yFirstCursorLine = tw->yPos * tw->yCharDimension + tw->yCharDimension;
	int yLastCursorLine  = tw->yPos * tw->yCharDimension + tw->yCharDimension;
	int xLeft            = tw->xPos * tw->xCharDimension;
	int xRight           = (tw->xPos+1) * tw->xCharDimension - 1;
	int rop2Old;

	POINT point;

	tw->bBlinkOn = TRUE;

	yFirstCursorLine -= tw->yCharDimension / (tw->bInsertMode ? 2 : 4);


	rop2Old = SetROP2( tw->hdc, R2_NOT );

	while (yFirstCursorLine < yLastCursorLine)
	{
		MoveToEx( tw->hdc, xLeft,  yFirstCursorLine,  &point );
		LineTo  ( tw->hdc, xRight, yFirstCursorLine++        );
	}

	SetROP2( tw->hdc, rop2Old );
}


static void console_out_character(TWindow *tw, const unsigned char ch)
{
	if (ch>=32)
	{
		tw->pchWindowBuffer[CALC_POS(tw->xPos, tw->yPos)] = ch;
		draw_current_character(tw);
		move_forwards(tw);
	}
	else
	{
		/* do we have a backspace? */
		if (ch==8)
		{
			move_backwards(tw);
			tw->pchWindowBuffer[CALC_POS(tw->xPos,tw->yPos)] = ' ';
			draw_current_character(tw);
		}

		/* do we have a return? */
		if ( (ch==13) || (ch=='\n'))
		{
			tw->xPos = 0;
			move_downwards(tw);
		}
	}
}


static void draw_current_input( TWindow *tw )
{
	int xPos = tw->xPos;
	int yPos = tw->yPos;

	/* go to where the input line begins */
	/* @SRT TODO: does not work with multi-line inputs */
	tw->xPos -= tw->posInputBuffer;

	/* set zero at end of input buffer */

	tw->achInputBuffer[tw->cntInputBuffer] = 0;

	console_out( (console_t*) tw, "%s", tw->achInputBuffer );

	/* output a blank to delete a possibly character after the
	   input (needed for outputting after backspace or delete
	*/
	console_out_character( tw, ' ' );

	/* restore cursor position */
	tw->xPos = xPos;
	tw->yPos = yPos;
}


void start_timer( TWindow *tw )
{
	SetTimer( tw->hwndConsole, 1, 500, NULL );
}

int console_out(console_t *log, const char *format, ...)
{
	TWindow *tw = (TWindow*) log;

	va_list ap;

	char ch;
	char buffer[MAX_OUTPUT_LENGTH];
	char *pBuffer      = buffer;
	char *pWriteBuffer = &tw->pchWindowBuffer[CALC_POS(tw->xPos,tw->yPos)];

    va_start(ap, format);
    vsprintf(buffer, format, ap);

	/* restore character under cursor */
	restore_current_character( tw );

	while ( (ch = *pBuffer++) != 0)
	{
		console_out_character( tw, ch );
	}

	return 0;
}




char *console_in(console_t *log)
{
	TWindow *tw = (TWindow*) log;

	char *p;

	tw->posInputBuffer =
	tw->cntInputBuffer = 0;
	tw->bInsertMode    =
	tw->bInputReady    = FALSE;

	tw->nCurrentSelectHistory = 0;

	/* set a timer for the cursor */
	start_timer( tw );

	do 
	{
		ui_dispatch_next_event();
	}
	while (!tw->bInputReady);

	p = stralloc( tw->achInputBuffer );


    /* Remove trailing newlines.  */
    {
        int len;

        for (len = strlen(p);
             len > 0 && (p[len - 1] == '\r'
                         || p[len - 1] == '\n');
             len--)
            p[len - 1] = '\0';
    }

	/* stop the timer for the cursor */
	KillTimer(tw->hwndConsole, 1); 

	/* restore character under cursor */
	restore_current_character( tw );

	add_to_history( tw, p );

	return p;
}



static void replace_current_input( TWindow *tw, const char *p )
{
	/* @SRT TODO: not multi-line safe! */

	unsigned nOldBufferLength = tw->cntInputBuffer;

	/* restore character under cursor */
	restore_current_character( tw );

	strcpy( tw->achInputBuffer, p );
	tw->cntInputBuffer = strlen( tw->achInputBuffer );

	draw_current_input( tw );
	tw->xPos += tw->cntInputBuffer - tw->posInputBuffer;
	tw->posInputBuffer = tw->cntInputBuffer;

	/* test: is the old line longer than the new one? */
	if (tw->cntInputBuffer < nOldBufferLength )
	{
		/* yes, delete old lines with blanks */
		int xPos = tw->xPos;

		nOldBufferLength -= tw->cntInputBuffer;

		while (nOldBufferLength-- > 0)
		{
			console_out_character( tw, ' ' );
		}

		tw->xPos = xPos;
	}
}


static void external_resize_window( TWindow *tw, int nWidth, int nHeight )
{
	unsigned xDim = nWidth  / tw->xCharDimension;
	unsigned yDim = nHeight / tw->yCharDimension;

	/* make sure the cursor is visible even in the new area */
	xDim = max( xDim, tw->xPos + 1 );

	/* make sure the minimum window sizes */
	xDim = max( xDim, MIN_XSIZE );
	yDim = max( yDim, MIN_YSIZE );

	/* @SRT TODO: if a multi-line-input is given, make sure that the
	   x dimension is not changed OR that the input is correctly redrawn!
	*/

	/* 
	test if window is bigger than ever before; if so,
	get new memory for new window buffer
	*/
	if ((xDim > tw->xMax) || (yDim > tw->yMax))
		reallocate_window_memory( tw, xDim, yDim );

	tw->console.console_xres = xDim;
	tw->console.console_yres = yDim;

	/* make sure the cursor is inside the visible area */
	while (tw->yPos >= yDim)
	{
		scroll_up( tw );
	}

	resize_window( tw );
}


/* window procedure */
static long CALLBACK console_window_proc(HWND hwnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)

{
	TWindow *tw = (TWindow*) GetWindowLong( hwnd, GWL_USERDATA );

	switch (msg)
	{
	case WM_SIZE:
		external_resize_window( tw, LOWORD(lParam), HIWORD(lParam) );
		return 0;

	case WM_CLOSE:
		/* if the window is closed, i.e. by pressing the close
		   button, we simulate a typing of the 'x' command
		*/

		if (tw->achInputBuffer)
		{
			tw->bInputReady       = TRUE;
			tw->achInputBuffer[0] = 'x';
			tw->achInputBuffer[1] = 0;
			tw->posInputBuffer    =
			tw->cntInputBuffer    = 2;
		}	
		/* FALL THROUGH */

	case WM_DESTROY:
		/* no PostQuitMessage(), because else, the whole application
		   (VICE) would be closed - as it occurred in the old version
		   with the standard console
		*/
		return 0;

	case WM_TIMER:
		if (wParam == 1)
		{
			if (tw->bBlinkOn)
			{
				/* restore previous character */
				restore_current_character( tw );
			}
			else
			{
				/* paint cursor */
				paint_cursor( tw );
			}

			start_timer( tw );
			return 0;
		}
		break;


	case WM_KEYDOWN:
		{
			int nVirtKey = (int) wParam;

			switch (nVirtKey)
			{
			case VK_UP:
				if (tw->nCurrentSelectHistory < MAX_HISTORY)
				{
					const char *p;
					++tw->nCurrentSelectHistory;

					p = get_history_entry(tw);

					if (p)
						replace_current_input( tw, p );
					else
					{
						/* undo the increment above */
						/*
						 remark: get_history_entry() above depends on the
						 increment! 
						*/
						--tw->nCurrentSelectHistory;
					}
				}
				return 0;

			case VK_DOWN:
				if (tw->nCurrentSelectHistory > 1)
				{
					--tw->nCurrentSelectHistory;
					replace_current_input( tw, get_history_entry( tw ) );
				}
				else
				{
					tw->nCurrentSelectHistory = 0;
					replace_current_input( tw, "" );
				}
				return 0;

			case VK_LEFT:
				/* restore character under cursor */
				restore_current_character( tw );

				if (tw->posInputBuffer>0)
				{
					--tw->posInputBuffer;
					move_backwards(tw);
				}
				return 0;

			case VK_RIGHT:
				/* restore character under cursor */
				restore_current_character( tw );

				if (tw->posInputBuffer < tw->cntInputBuffer)
				{
					++tw->posInputBuffer;
					move_forwards(tw);
				}
				return 0;

			case VK_HOME:
				/* restore character under cursor */
				restore_current_character( tw );

				/* @SRT TODO: not multi-line safe! */
				tw->xPos -= tw->posInputBuffer;
				tw->posInputBuffer = 0;
				return 0;

			case VK_END:
				/* restore character under cursor */
				restore_current_character( tw );

				tw->xPos += (tw->cntInputBuffer - tw->posInputBuffer );
				tw->posInputBuffer = tw->cntInputBuffer;
				return 0;


			case VK_INSERT:
				tw->bInsertMode = tw->bInsertMode ? FALSE : TRUE;

				/* repaint the cursor: */
				restore_current_character( tw );
				paint_cursor( tw );
				start_timer( tw );
				return 0;

			case VK_DELETE:
				/* check not to clear more characters than there were */
				if (tw->posInputBuffer < tw->cntInputBuffer)
				{
					/* only process del if we're not at the end of the buffer */
					--tw->cntInputBuffer;

					memmove(
						&tw->achInputBuffer[tw->posInputBuffer], 
						&tw->achInputBuffer[tw->posInputBuffer+1],
						tw->cntInputBuffer - tw->posInputBuffer
						);
				}

				draw_current_input( tw );

				return 0;
			}
		}
		break;

	case WM_CHAR:
		{
			/* a key is pressed, process it! */
			char chCharCode = (char) wParam;

			/* restore character under cursor */
			restore_current_character( tw );

			if (chCharCode >= 32)
			{
				/* it's a printable character, process it */

				if (tw->bInsertMode)
				{
					/* insert mode */

					/* only insert if there's room in the buffer */
					if (tw->cntInputBuffer < MAX_WIDTH)
					{
						++tw->cntInputBuffer;

						memmove(
							&tw->achInputBuffer[tw->posInputBuffer+1], 
							&tw->achInputBuffer[tw->posInputBuffer],
							tw->cntInputBuffer - tw->posInputBuffer
							);

						draw_current_input( tw );

						tw->achInputBuffer[tw->posInputBuffer++] = chCharCode;

						/* output the character */
						console_out_character( tw, chCharCode );
					}
				}
				else
				{
					/* overwrite mode */

					/* processing only if the buffer is not full! */
					if (tw->cntInputBuffer < MAX_WIDTH)
					{
						tw->achInputBuffer[tw->posInputBuffer++] = chCharCode;

						/* output the character */
						console_out_character( tw, chCharCode );

						/* if we're at the end of the buffer, it's a kind of insert mode */
						if (tw->cntInputBuffer < tw->posInputBuffer)
							++tw->cntInputBuffer;
					}
				}

				return 0;
			}


			if (chCharCode == 8)
			{
				/* it's a backspace, process it if possible */

				/* check not to clear more characters than there were */
				if (tw->posInputBuffer > 0)
				{
					/* move the characters forward */

					if (tw->posInputBuffer < tw->cntInputBuffer)
					{
						memmove(
							&tw->achInputBuffer[tw->posInputBuffer-1], 
							&tw->achInputBuffer[tw->posInputBuffer],
							tw->cntInputBuffer - tw->posInputBuffer
							);

						--tw->cntInputBuffer;
						draw_current_input( tw );

						move_backwards(tw);
					}
					else
					{
						/* only last character deleted, use faster method */
						console_out_character( tw, chCharCode );
						--tw->cntInputBuffer;
					}

					--tw->posInputBuffer;
				}

				return 0;
			}

			if (chCharCode == 13)
			{
				/* it's a CR, so the input is ready */
				tw->achInputBuffer[tw->cntInputBuffer] = 0;
				tw->bInputReady                        = TRUE;

				console_out_character( tw, chCharCode );
				return 0;
			}

			/* any other key will be ignored */
		}
		break;


	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hwnd,&ps);

			redraw_window( tw, &ps );

			EndPaint(hwnd,&ps);

			return 0;
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


static TWindow *find_console_entry(const char *id)
{
	int i;

	for (i=0; i<MAX_CONSOLES; i++)
	{
		if (!window_contents[i].pchName)
			break;

		if (strcmp(window_contents[i].pchName, id)==0)
			return &window_contents[i];
	}

	if (i < MAX_CONSOLES)
	{
		window_contents[i].pchName = stralloc(id);

		/* do first inits */
		window_contents[i].xMax                 =
		window_contents[i].console.console_xres = 80;
		window_contents[i].yMax                 =
		window_contents[i].console.console_yres = 25;

		window_contents[i].xMin                 = MIN_XSIZE;
		window_contents[i].yMin                 = MIN_YSIZE;

		window_contents[i].xWindow              = CW_USEDEFAULT;
		window_contents[i].yWindow              = CW_USEDEFAULT;

		return &window_contents[i];
	}

	return NULL;
}

console_t *console_open(const char *id)
{
	TWindow *tw;

	{
		static BOOLEAN bFirstCall = TRUE;

		if (bFirstCall)
		{
			WNDCLASS wc;

			bFirstCall = FALSE;

			/* initialize the whole private structure */
			memset( &window_contents, 0, sizeof(TWindow)*MAX_CONSOLES );

			/* Register 2nd window class for the monitor window */
			wc.style         = CS_OWNDC;
			wc.lpfnWndProc   = console_window_proc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = winmain_instance;
			wc.hIcon         = LoadIcon(winmain_instance, MAKEINTRESOURCE(IDI_ICON1));
			wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0) + 1);
			wc.lpszMenuName  = 0; /* @SRT: no menu yet MAKEINTRESOURCE(menu); */
			wc.lpszClassName = CONSOLE_CLASS;

			RegisterClass(&wc);
		}
	}

	tw = find_console_entry( id );

	allocate_window_memory( tw );


    tw->hwndConsole = CreateWindow(CONSOLE_CLASS,
		id,
		WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SIZEBOX,
        tw->xWindow,
        tw->yWindow,
        1,
        1,
        NULL,
        NULL,
        winmain_instance,
        NULL);


	/* get the previous active window, and set myself active */
	tw->hwndPreviousActive = SetActiveWindow( tw->hwndConsole );

	/* get a DC and select proper font */
	tw->hdc = GetDC( tw->hwndConsole );
	SelectObject( tw->hdc, GetStockObject( ANSI_FIXED_FONT ) );

	/* set colors for output */
	SetTextColor( tw->hdc, GetSysColor( COLOR_WINDOWTEXT ) );
	SetBkColor( tw->hdc, GetSysColor( COLOR_WINDOW ) );

	/* store pointer to structure with window */
	SetWindowLong( tw->hwndConsole, GWL_USERDATA, (long) tw );

	/* get the dimensions of one char */
	get_char_dimensions( tw );

	/* set the window to the correct size */
	resize_window( tw );

	/* now show the window */
	ShowWindow( tw->hwndConsole, SW_SHOW );

	tw->console.console_can_stay_open = 1;

    return (console_t*) tw;
}

int console_close(console_t *log)
{
	TWindow *tw = (TWindow*) log;

	ReleaseDC( tw->hwndConsole, tw->hdc );

	DestroyWindow(tw->hwndConsole);

	tw->hwndConsole = NULL;

	/* set the previous active window as new active one */
	SetActiveWindow( tw->hwndPreviousActive );

    return 0;
}


int console_shutdown(console_t *log)
{
	if (log)
	{
		console_close( log );
		free_window_memory( (TWindow*) log );
	}

	UnregisterClass(CONSOLE_CLASS,winmain_instance);

	return 0;
}
