/*
 * uilib.c - Common UI elements for the Windows user interface.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Manfred Spraul <manfreds@colorfullife.com>
 *  Andreas Matthies <andreas.matthies@arcormail.de>
 *  Tibor Biczo <crown@mail.matav.hu>
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

#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "diskimage.h"
#include "imagecontents.h"
#include "resources.h"
#include "ui.h"
#include "uilib.h"
#include "utils.h"
#include "vdrive.h"
#include "winmain.h"
#include "res.h"

/* Mingw & pre VC 6 headers doesn't have this definition */
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING    0x00800000
#endif

static char *(*read_content_func)(const char *);
static char **autostart_result;
static char *(*get_filename_from_content)(char *, int);

typedef struct {
    char*           (*content_read_function)(const char *);
    char*           (*get_filename_from_content)(char *, int);
    LPOFNHOOKPROC   hook_proc;
    int             TemplateID;
} ui_file_selector_style_type;

static UINT APIENTRY tape_hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam);
static UINT APIENTRY hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam);
char *read_disk_image_contents(const char *name);
char *read_tape_image_contents(const char *name);
char *read_disk_or_tape_image_contents(const char *name);
static char *get_filename_from_disk(char *filename, int index);
static char *get_filename_from_tape(char *filename, int index);
static char *get_filename_from_disk_or_tape(char *filename, int index);

static ui_file_selector_style_type  styles[]={
    /* FILE_SELECTOR_DEFAULT_STYLE */
    {NULL,NULL,NULL,0},
    /* FILE_SELECTOR_TAPE_STYLE */
    {read_tape_image_contents,get_filename_from_tape,tape_hook_proc,IDD_OPENTAPE_TEMPLATE},
    /* FILE_SELECTOR_DISK_STYLE */
    {read_disk_image_contents,get_filename_from_disk,hook_proc,IDD_OPEN_TEMPLATE},
    /* FILE_SELECTOR_DISK_AND_TAPE_STYLE */
    {read_disk_or_tape_image_contents,get_filename_from_disk_or_tape,hook_proc,IDD_OPEN_TEMPLATE},
    /* DUMMY entry Insert new styles before this */
    {NULL,NULL,NULL,0}
};

char *read_disk_image_contents(const char *name)
{
image_contents_t    *contents;
char                *s;

    contents=image_contents_read_disk(name);
    if (contents==NULL) {
        return NULL;
    }
    s=image_contents_to_string(contents);
    image_contents_destroy(contents);
    return s;
}

char *read_tape_image_contents(const char *name)
{
image_contents_t    *contents;
char                *s;

    contents=image_contents_read_tape(name);
    if (contents==NULL) {
        return NULL;
    }
    s=image_contents_to_string(contents);
    image_contents_destroy(contents);
    return s;
}

char *read_disk_or_tape_image_contents(const char *name)
{
char    *tmp;

    tmp=read_disk_image_contents(name);
    if (tmp==NULL) {
        tmp=read_tape_image_contents(name);
    }
    return tmp;
}

static void create_content_list(char *text, HWND list)
{
char    *start;
char    buffer[256];
int     index;

    if (text==NULL) return;
    start=text;
    index=0;
    while (1) {
        if (*start==0x0a) {
            buffer[index]=0;
            index=0;
            SendMessage(list,LB_ADDSTRING,0,(LPARAM)buffer);
        } else if (*start==0x0d) {
        } else if (*start==0) {
            break;
        } else {
            buffer[index++]=*start;
        }
        start++;
    }

}

/* The following functions can be removed once the selector gives back
   the program number (index) and feed autostart_xyz() with this number
   instead of retriving the program name itself.  */
#if 1
static char *get_filename_from_disk(char *filename, int index)
{
image_contents_t            *contents;
image_contents_file_list_t  *current;
char                        *s;

    contents=image_contents_read_disk(filename);
    if (contents==NULL) {
        return NULL;
    }
    s=NULL;
    if (index!=0) {
        current=contents->file_list;
        index--;
        while ((index!=0) && (current!=NULL)) {
            current=current->next;
            index--;
        }
        if (current!=NULL) {
            s=stralloc(current->name);
        }
    }
    image_contents_destroy(contents);
    return s;
}

static char *get_filename_from_tape(char *filename, int index)
{
image_contents_t            *contents;
image_contents_file_list_t  *current;
char                        *s;

    contents=image_contents_read_tape(filename);
    if (contents==NULL) {
        return NULL;
    }
    s=NULL;
    if (index!=0) {
        current=contents->file_list;
        index--;
        while ((index!=0) && (current!=NULL)) {
            current=current->next;
            index--;
        }
        if (current!=NULL) {
            s=stralloc(current->name);
        }
    }
    image_contents_destroy(contents);
    return s;
}

static char *get_filename_from_disk_or_tape(char *filename, int index)
{
char    *s;

    s=get_filename_from_disk(filename,index);
    if (s==NULL) {
        s=get_filename_from_tape(filename,index);
    }
    return s;
}
#endif

static  HFONT   hfont;

static UINT APIENTRY tape_hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam)
{
    
HWND    preview;
char    *contents;
char    filename[256];
int     index;

    preview=GetDlgItem(hwnd,IDC_PREVIEW);
    switch (uimsg) {
        case WM_INITDIALOG:
            SetWindowText(GetDlgItem(GetParent(hwnd),IDOK),"&Attach");
            /*  maybe there's a better font-definition (FIXME) */
            /*  I think it's OK now (Tibor) */
            hfont = CreateFont(-12,-7,0,0,400,0,0,0,0,0,0,
                DRAFT_QUALITY,FIXED_PITCH|FF_MODERN,NULL);
            if (hfont)
                SendDlgItemMessage(hwnd,IDC_PREVIEW,WM_SETFONT,
                    (WPARAM)hfont,MAKELPARAM(TRUE,0));
            break;
        case WM_NOTIFY:
            if (((OFNOTIFY*)lparam)->hdr.code==CDN_SELCHANGE) {
                SendMessage(preview,LB_RESETCONTENT,0,0);
                if (SendMessage(((OFNOTIFY*)lparam)->hdr.hwndFrom,
                    CDM_GETFILEPATH,256,(LPARAM)filename)>=0) {
                    if (read_content_func!=NULL) {
                        contents=read_content_func(filename);
                        create_content_list(contents,preview);
                    }
                }
            }
            break;
        case WM_COMMAND:
            switch (HIWORD(wparam)) {
                case LBN_DBLCLK:
                    if (autostart_result!=NULL) {
                        index=SendMessage((HWND)lparam,LB_GETCURSEL,0,0);
                        if (SendMessage(GetParent(hwnd),
                            CDM_GETFILEPATH,256,(LPARAM)filename)>=0) {
                            *autostart_result=get_filename_from_content(filename,index);
                            SendMessage(GetParent(hwnd),WM_COMMAND,MAKELONG(IDOK,BN_CLICKED),(LPARAM)GetDlgItem(GetParent(hwnd),IDOK));
                        }
                    }
                    break;
            }
            break;
        case WM_DESTROY:
            if (hfont!=NULL) {
                DeleteObject(hfont);
                hfont=NULL;
            }
            break;
    }
    return 0;
}

static char *image_type_name[] = 
    { "d64","d71","d80","d81","d82","g64","x64",NULL };
static int  image_type[] = {
    DISK_IMAGE_TYPE_D64,
    DISK_IMAGE_TYPE_D71,
    DISK_IMAGE_TYPE_D80,
    DISK_IMAGE_TYPE_D81,
    DISK_IMAGE_TYPE_D82,
    DISK_IMAGE_TYPE_G64,
    DISK_IMAGE_TYPE_X64
};

static UINT APIENTRY hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam)
{
    HWND        preview;
    HWND        image_type_list;
    char        *contents;
    char        filename[256];
    int         counter;
    int         msg_type;
    int         append_extension = 0;
    int         is_it_standard_extension=0;
    char        *extension;
    int         index;
    LV_FINDINFO find;
    LV_ITEM     item;

    preview=GetDlgItem(hwnd,IDC_PREVIEW);
    switch (uimsg) {
        case WM_INITDIALOG:
            SetWindowText(GetDlgItem(GetParent(hwnd),IDOK),"&Attach");
            image_type_list=GetDlgItem(hwnd,IDC_BLANK_IMAGE_TYPE);
            for (counter = 0; image_type_name[counter]; counter++) {
                SendMessage(image_type_list,CB_ADDSTRING,0,
                    (LPARAM)image_type_name[counter]);
            }
            SendMessage(image_type_list,CB_SETCURSEL,(WPARAM)0,0);
            /* maybe there's a better font-definition (FIXME) */
            /*  I think it's OK now (Tibor) */
            hfont = CreateFont(-12,-7,0,0,400,0,0,0,0,0,0,
                DRAFT_QUALITY,FIXED_PITCH|FF_MODERN,NULL);
            if (hfont) {
                SendDlgItemMessage(hwnd,IDC_PREVIEW,WM_SETFONT,
                    (WPARAM)hfont,MAKELPARAM(TRUE,0));
            }
            SetDlgItemText(hwnd,IDC_BLANK_IMAGE_NAME,"Vice");
            SetDlgItemText(hwnd,IDC_BLANK_IMAGE_ID,"1a");
            break;
        case WM_NOTIFY:
            if (((OFNOTIFY*)lparam)->hdr.code==CDN_SELCHANGE) {
                SendMessage(preview,LB_RESETCONTENT,0,0);
                if (SendMessage(((OFNOTIFY*)lparam)->hdr.hwndFrom,
                    CDM_GETFILEPATH,256,(LPARAM)filename)>=0) {
                    if (read_content_func!=NULL) {
                        contents=read_content_func(filename);
                        create_content_list(contents,preview);
                    }
                }
            }
            break;
        case WM_COMMAND:
            msg_type = LOWORD(wparam);
            switch (msg_type) {
                case IDC_BLANK_IMAGE:
                    if (SendMessage(GetParent(hwnd),
                        CDM_GETSPEC,256,(LPARAM)filename)<=1) {
                        ui_error("Please enter a filename.");
                        return -1;
                    }
                    if (strchr(filename,'.') == NULL) {
                        append_extension = 1;
                        is_it_standard_extension=1;
                    } else {
                        /*  Find last dot in name */
                        extension=strrchr(filename,'.');
                        /*  Skip dot */
                        extension++;
                        /*  Figure out if it's a standard extension */
                        for (counter=0; image_type_name[counter]; counter++) {
                            if (strncasecmp(extension,image_type_name[counter],strlen(image_type_name[counter]))==0) {
                                is_it_standard_extension=1;
                                break;
                            }
                        }
                    }
                    if (SendMessage(GetParent(hwnd),
                        CDM_GETFILEPATH,256,(LPARAM)filename)>=0)
                    {
                        char disk_name[32];
                        char disk_id[3];
                        char format_name[40];

                        counter = 
                            SendMessage(GetDlgItem(hwnd,IDC_BLANK_IMAGE_TYPE),
                                CB_GETCURSEL,0,0);
                        if (append_extension) {
                            strcat(filename,".");
                            strcat(filename,image_type_name[counter]);
                        }
                        if (file_exists_p(filename)) {
                            int ret;
                            ret = MessageBox(hwnd, "Overwrite existing image?",
                                "VICE question", MB_YESNO | MB_ICONQUESTION);
                            if (ret != IDYES)
                                return -1;
                        }
                        GetDlgItemText(hwnd,IDC_BLANK_IMAGE_NAME,disk_name,17);
                        GetDlgItemText(hwnd,IDC_BLANK_IMAGE_ID,disk_id,3);
                        sprintf(format_name,"%s,%s",disk_name,disk_id);
                        if (vdrive_internal_create_format_disk_image(filename,
                            format_name, image_type[counter])<0)
                        {
                            ui_error("Cannot create image");
                            return -1;
                        }
                        /*  Select filter:
                            If we have a standard extension, select the disk filters,
                            but leave at 'All files' if it was already there, otherwise
                            select 'All files'
                        */
                        index=SendMessage(GetDlgItem(GetParent(hwnd),0x470),CB_GETCOUNT,0,0);
                        if (is_it_standard_extension) {
                            if (index-1!=SendMessage(GetDlgItem(GetParent(hwnd),0x470),CB_GETCURSEL,0,0)) {
                                SendMessage(GetDlgItem(GetParent(hwnd),0x470),CB_SETCURSEL,0,0);
                            }
                        } else {
                            SendMessage(GetDlgItem(GetParent(hwnd),0x470),CB_SETCURSEL,index-1,0);
                        }
                        /*  Notify main window about filter change */
                        SendMessage(GetParent(hwnd),WM_COMMAND,MAKELONG(0x470,CBN_SELENDOK),(LPARAM)GetDlgItem(GetParent(hwnd),0x470));

                        /*  Now find filename in ListView & select it */
                        SendMessage(GetParent(hwnd),CDM_GETSPEC,256,(LPARAM)filename);
                        if (append_extension) {
                            strcat(filename,".");
                            strcat(filename,image_type_name[counter]);
                        }
                        find.flags=LVFI_STRING;
                        find.psz=filename;
                        index=SendMessage(GetDlgItem(GetDlgItem(GetParent(hwnd),0x461),1),LVM_FINDITEM,-1,(LPARAM)&find);
                        item.stateMask=LVIS_SELECTED|LVIS_FOCUSED;
                        item.state=LVIS_SELECTED|LVIS_FOCUSED;
                        SendMessage(GetDlgItem(GetDlgItem(GetParent(hwnd),0x461),1),LVM_SETITEMSTATE,index,(LPARAM)&item);
                    }
                    break;
            }
            switch (HIWORD(wparam)) {
                case LBN_DBLCLK:
                    if (autostart_result!=NULL) {
                        index=SendMessage((HWND)lparam,LB_GETCURSEL,0,0);
                        if (SendMessage(GetParent(hwnd),
                            CDM_GETFILEPATH,256,(LPARAM)filename)>=0) {
                            *autostart_result=get_filename_from_content(filename,index);
                            SendMessage(GetParent(hwnd),WM_COMMAND,MAKELONG(IDOK,BN_CLICKED),(LPARAM)GetDlgItem(GetParent(hwnd),IDOK));
                        }
                    }
                    break;
            }
            break;
        case WM_DESTROY:
            if (hfont!=NULL) {
                DeleteObject(hfont);
                hfont=NULL;
            }
            break;
    }
    return 0;
}


char *ui_select_file(HWND hwnd, const char *title, const char *filter, int style, char **autostart)
{
    char name[1024] = "";
    OPENFILENAME ofn;

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = winmain_instance;
    ofn.lpstrFilter = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = name;
    ofn.nMaxFile = sizeof(name);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = title;
    if (styles[style].TemplateID!=0) {
        ofn.Flags = (OFN_EXPLORER
                     | OFN_HIDEREADONLY
                     | OFN_NOTESTFILECREATE
                     | OFN_FILEMUSTEXIST
                     | OFN_ENABLEHOOK
                     | OFN_ENABLETEMPLATE
                     | OFN_SHAREAWARE
                     | OFN_ENABLESIZING);
        ofn.lpfnHook = styles[style].hook_proc;
        ofn.lpTemplateName = MAKEINTRESOURCE(styles[style].TemplateID);
    } else {
        ofn.Flags = (OFN_EXPLORER
                     | OFN_HIDEREADONLY
                     | OFN_NOTESTFILECREATE
                     | OFN_FILEMUSTEXIST
                     | OFN_SHAREAWARE);
        ofn.lpfnHook = NULL;
        ofn.lpTemplateName = NULL;
    }
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;

    read_content_func=styles[style].content_read_function;
    get_filename_from_content=styles[style].get_filename_from_content;
    autostart_result=autostart;
    if (GetOpenFileName(&ofn)) {
        return stralloc(name);
    } else {
        return NULL;
    }
}

void ui_set_res_num(char *res, int value, int num)
{
    char tmp[256];

    sprintf(tmp, res, num);
    resources_set_value(tmp, (resource_value_t *) value);
}

BOOL CALLBACK GetParentEnumProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwWndThread = GetWindowThreadProcessId(hwnd,NULL);


	if(dwWndThread == GetCurrentThreadId()) {
		*(HWND*)lParam = hwnd;
		return FALSE;
	}
	return TRUE;	
}

HWND GetParentHWND()
{
	HWND hwndOut = NULL;

	EnumWindows(GetParentEnumProc,
				(LPARAM)&hwndOut);

	if(hwndOut == NULL)
		return NULL;
	return GetLastActivePopup(hwndOut);
}

BOOL CALLBACK TextDlgProc(HWND hwndDlg,		// handle to dialog box
			  UINT uMsg,		// message
			  WPARAM wParam,	// first message parameter
			  LPARAM lParam  );	// second message parameter
struct TEXTDLGDATA {
	char *szCaption;
	char *szHeader;
	char *szText;
};

void ui_show_text(HWND hWnd,
		const char* szCaption,
		const char* szHeader,
		const char* szText)
{
	struct TEXTDLGDATA info;
	char * szRNText;
	int i,j;

	szRNText = (char*)HeapAlloc(GetProcessHeap(),0,2*lstrlen(szText)+1);
	i=j=0;
	while(szText[i] != '\0') {
		if(szText[i] == '\n')
			szRNText[j++] = '\r';
		szRNText[j++] = szText[i++];
	}
	szRNText[j] = '\0';

	info.szCaption = (char *)szCaption;
	info.szHeader = (char *)szHeader;
	info.szText = szRNText;

//	if(hWnd == HWND_AUTO)
//		hWnd = GetParentHWND();
	DialogBoxParam(GetModuleHandle(NULL),	// GetModuleHandle(NULL) returns the instance handle
										// of the executable that created the current process.
										// Win32: module handle == instance handle == task [Win3.1 legacy]
			MAKEINTRESOURCE(IDD_TEXTDLG),
			hWnd,
			TextDlgProc,
			(LPARAM)&info);
	HeapFree(GetProcessHeap(),0,szRNText);
}

// FIXME: the client area with the scroll bars 
//		disabled would be larger, this function
//		is not perfect.
void AutoHideScrollBar(HWND hWnd, int fnBar)
{
	BOOL bResult;
	SCROLLINFO scInfo;
	UINT uiDiff;
	
	scInfo.cbSize = sizeof(scInfo);
	scInfo.fMask = SIF_RANGE|SIF_PAGE;
	bResult = GetScrollInfo(hWnd, fnBar, &scInfo);

	if(!bResult)
		return;

	uiDiff= scInfo.nMax-scInfo.nMin;
	if(scInfo.nPage > uiDiff)
		ShowScrollBar(hWnd,fnBar, 0);
}



BOOL CALLBACK TextDlgProc(HWND hwndDlg,		// handle to dialog box
				UINT uMsg,	// message
				WPARAM wParam,	// first message parameter
				LPARAM lParam)	// second message parameter
{
    switch (uMsg) {
        case WM_INITDIALOG:
		{
			struct TEXTDLGDATA* pInfo = (struct TEXTDLGDATA*) lParam;
			SetWindowText(hwndDlg,pInfo->szCaption);
			SetDlgItemText(hwndDlg, IDC_HEADER, pInfo->szHeader);

			SetDlgItemText(hwndDlg,	IDC_TEXT, pInfo->szText);
			SendDlgItemMessage(hwndDlg,
								IDC_TEXT,
								EM_SETREADONLY,
								1,	// wParam: read-only flag
								0);	// lParam: unused.
			AutoHideScrollBar(GetDlgItem(hwndDlg, IDC_TEXT),SB_HORZ);
			AutoHideScrollBar(GetDlgItem(hwndDlg, IDC_TEXT),SB_VERT);
			return TRUE;
		}
		case WM_CTLCOLORSTATIC:
			// The text box should use the normal colors, but the contents must
			// be read-only.
			// A read-only text box uses WM_CTLCOLORSTATIC, but a read-write
			// text box uses WM_CTLCOLOREDIT.
			if((HWND)lParam == GetDlgItem(hwndDlg,IDC_TEXT)) {
				// the return value is passed directly,
				// SetWindowLong(DWL_MSGRESULT) is ignored.
				return DefDlgProc(hwndDlg, WM_CTLCOLOREDIT, wParam, lParam);
			} else {
				return FALSE;
			}
			
        case WM_CLOSE:
            EndDialog(hwndDlg,0);
            return TRUE;
        case WM_COMMAND:
			switch (LOWORD(wParam)) {
                case IDCANCEL:
				case IDOK:
	                EndDialog(hwndDlg, 0);
		            return TRUE;
			}
            break;
    }
    return FALSE;
}

