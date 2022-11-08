
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #define DEBUG 1  */

#ifdef DEBUG
#define DBG(_x) printf _x
#else
#define DBG(_x)
#endif

#if defined(DEBUG) && (DEBUG > 1)
#define DBG2(_x) printf _x
#else
#define DBG2(_x)
#endif

typedef struct _ITEM
{
    struct _ITEM *next;
    struct _ITEM *alias;
    char *string;
    char *desc;
    int flags;
} ITEM;

ITEM *list_findstr(ITEM *list, char *str)
{
    ITEM *itm = list;
    while (itm) {
        /* DBG((">%s|%s\n",itm->string, str)); */
        if (itm->string) {
            if (!strcasecmp(itm->string, str)) {
                return itm;
            }
        }
        itm = itm->next;
    }
    return NULL;
}

ITEM *list_addstr(ITEM *list, char *str)
{
    ITEM *itm;

    if ((str == NULL) || (*str == 0)) {
        return list;
    }

    itm = list_findstr(list, str);
    if (itm) {
        return itm;
    }
    itm = malloc(sizeof(ITEM));
    itm->flags=0;
    itm->desc=NULL;
    itm->alias=NULL;
    itm->string = strdup(str);
#if 0
    itm->next = list->next;
    list->next = itm;
#else
    /* add to the bottom of the list, that will result in printed list being in
     * the same order as resources/options appear in the docs */
    while (list->next) {
        list = list->next;
    }
    itm->next = NULL;
    list->next = itm;
#endif
    return itm;
}

int skipblank(FILE *f)
{
    int c;
    while(!feof(f)) {
        c = fgetc(f);
        if (c == EOF) {
            return 0;
        }
        if ((c != ' ') && (c != '\n') && (c != '\r') && (c != '\t')) {
            break;
        }
    }
    return c;
}

int skipuntil(FILE *f, int s)
{
    int c;
    while(!feof(f)) {
        c = fgetc(f);
        if (c == s) {
            break;
        }
    }
    return c;
}

int getstr(FILE *f, char *str)
{
    int c;
    while(!feof(f)) {
        c = fgetc(f);
        if ((c == EOF) || (c == ' ') || (c == '\n') || (c == '\t') || (c == ',') || (c == '=')) {
            break;
        }
        *str++ = c;
    }
    *str = 0;
    return c;
}

#define IS_PLUS4        (1<<0)
#define IS_CBM2         (1<<1)
#define IS_B500         (1<<2)
#define IS_PET          (1<<3)
#define IS_VIC20        (1<<4)
#define IS_DTV          (1<<5)
#define IS_C128         (1<<6)
#define IS_VSID         (1<<7)
#define IS_C64          (1<<8)
#define IS_C64SC        (1<<9)
#define IS_SCPU64       (1<<10)

#define IS_CARTCONV     (1<<11)
#define IS_PETCAT       (1<<12)
#define IS_C1541        (1<<13)

#define NUMPROGS 14
#define NUMEMUS 11

const char *emustring[0x10] = {
    "PLUS4",
    "CBM-II",
    "CBM-II-5x0",
    "PET",
    "VIC20",
    "C64DTV",
    "C128",
    "VSID",
    "C64",
    "C64SC",
    "SCPU64",
    "cartconv",
    "petcat",
    "c1541"
};

ITEM reslistrc = { NULL, NULL, 0}; /* resources from vicerc */
ITEM reslisttex = { NULL, NULL, 0}; /* resources in vindex */
ITEM reslisttexitm = { NULL, NULL, 0};
ITEM reslistnew = { NULL, NULL, 0};

ITEM optlistvice = { NULL, NULL, 0};
ITEM optlisttex = { NULL, NULL, 0};
ITEM optlisttex2 = { NULL, NULL, 0};
ITEM optlisttexitm = { NULL, NULL, 0};
ITEM optlistnew = { NULL, NULL, 0};


static char tmp[0x200];
static char tmp1[0x20][0x200];
static char tmp2[0x200];
static char tmpc[0x200];
static char tmpmsg[0x200+1];

void readtexi(FILE *tf)
{
    int c;
    char *msg;
    int status = 0;
    ITEM *itm, *itm1, *itm2;
    int itmcnt;
    int n;
    ITEM *aliasitm = NULL;
    int newline = 1;

    msg = &tmpmsg[0];
    itmcnt = 0;
    c = skipblank(tf);
    while(!feof(tf)) {
        if (newline && (c == '@')) {
            newline = 0;
            fscanf(tf, "%s", tmp);
            /*DBG(("[%s]\n",tmp));*/
            if (!strcmp(tmp, "vindex")) {
#if 1
                fscanf(tf, " ");
                itmcnt = 0;
                c = getstr(tf, tmp1[itmcnt]);
                DBG(("resource '%s'\n",tmp1[itmcnt]));
                list_addstr(&reslisttex, tmp1[itmcnt]);
                if (c != '\n') {
                    c = skipuntil(tf, '\n');
                }
                if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')) {
                    c = skipblank(tf);
                }
                status = 0x01;
                itmcnt++;
                aliasitm = NULL;
                newline = 1;
#endif
            } else if (/* !strcmp(tmp, "cindex") || */ !strcmp(tmp, "findex")) {
#if 1
                fscanf(tf, " ");
                c = getstr(tf, tmp1[itmcnt]);
                if ((tmp1[itmcnt][0] == '-') || (tmp1[itmcnt][0] == '+')) {
                    list_addstr(&optlisttex, tmp1[itmcnt]);
                    DBG(("option '%s' ",tmp1[itmcnt]));
                    itmcnt++;
                    if (c == ',') {
                        tmp1[itmcnt][0] = skipblank(tf);
                        c = getstr(tf, &tmp1[itmcnt][1]);
                        list_addstr(&optlisttex, tmp1[itmcnt]);
                        DBG(("/ '%s' ",tmp1[itmcnt]));
                        itmcnt++;
                    }
                    DBG(("\n"));
                } else {
                    list_addstr(&optlisttex2, tmpc);
                }
#endif
#if 1
                if (c != '\n') {
                    c = skipuntil(tf, '\n');
                }
                if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')) {
                    c = skipblank(tf);
                }
                status = 0x02;
                aliasitm = NULL;
                newline = 1;
#endif
            } else if (!strcmp(tmp, "item")) {
                fscanf(tf, " ");
                c = getstr(tf, tmp2);
                DBG2(("item '%s'\n",tmp2));
                if ((tmp2[0] == '-') || (tmp2[0] == '+')) {
                    aliasitm = list_addstr(&optlisttexitm, tmp2);
                } else {
                    aliasitm = list_addstr(&reslisttexitm, tmp2);
                }
                if (c != '\n') {
                    c = skipuntil(tf, '\n');
                }
                if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')) {
                    c = skipblank(tf);
                }
                status |= 0x10;
                aliasitm->alias = NULL;
                newline = 1;
            } else if (!strcmp(tmp, "itemx")) {
                fscanf(tf, " ");
                c = getstr(tf, tmp2);
                DBG(("itemx '%s'\n",tmp2));
                if ((tmp2[0] == '-') || (tmp2[0] == '+')) {
                    itm = list_addstr(&optlisttexitm, tmp2);
                } else {
                    itm = list_addstr(&reslisttexitm, tmp2);
                }
                if (c != '\n') {
                    c = skipuntil(tf, '\n');
                }
                if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')) {
                    c = skipblank(tf);
                }
                status |= 0x20;
                if (aliasitm) {
                    aliasitm->alias = itm;

                    itm2 = NULL;
                    if ((status & 0x0f) == 1) {
                        itm1 = list_findstr(&reslisttex, itm->string);
                        itm2 = list_findstr(&reslisttex, aliasitm->string);
                    } else if ((status & 0x0f) == 2) {
                        itm1 = list_findstr(&optlisttex, itm->string);
                        itm2 = list_findstr(&optlisttex, aliasitm->string);
                    }
                    if (itm2) {
                        itm2->alias = itm1;
                    }
                    /* printf("ok %35s %p %p\n",tmp2,itm,aliasitm); */
                }
                aliasitm = itm;
                newline = 1;
            } else if (!strcmp(tmp, "end") || !strcmp(tmp, "bye")) {
                c = skipuntil(tf, '\n');
                if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')) {
                    c = skipblank(tf);
                }
                status = 0;
                newline = 1;
            }
#if 0
            else if (!strcmp(tmp, "c")) {
                c = skipuntil(tf, '\n');
                if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')) {
                    c = skipblank(tf);
                }
                status = 0;
                newline = 1;
            }
#endif
            else {
                sprintf(tmpmsg, "@%s",tmp);
                msg = &tmpmsg[strlen(tmpmsg)];
                status |= 0x100;
                goto checkmsg;
            }
        } else {
checkmsg:
            if ((status & 0xf00) != 0) {
                    /* DBG(("<%02x %02x>\n",status,c)); */
                if (c == '\n') {
                    /* DBG(("<MSG STOP>\n")); */
                    *msg++ = 0;
                    for (n=0;n<itmcnt;n++) {
                        /* printf("%02x %s\n\t%s\n",status,tmp1[n],tmpmsg); */
                        if ((status & 0x0f) == 1) {
                            itm = list_findstr(&reslisttex, tmp1[n]);
                        } else if ((status & 0x0f) == 2) {
                            itm = list_findstr(&optlisttex, tmp1[n]);
                        }
                        if (itm) {
                            if (itm->desc) {
                                free(itm->desc);
                            }
                            itm->desc = strdup(tmpmsg);
                        }
                    }
                    itmcnt = 0;
                    status = 0;
                    newline = 1;
                } else {
                    *msg++ = c;
                }
            } else {
                if( ((status & 0x00f) != 0) &&
                    ((status & 0x0f0) != 0) &&
                    ((status & 0xf00) == 0)
                    ) {
                        status |= 0x100;
                        /* DBG(("<MSG START %02x '%c'>\n",status,c)); */
                        msg = &tmpmsg[0];
                        *msg++ = c;
                        newline = 0;
                    } else {
                        if (c == '\n') {
                            newline = 1;
                        }
                    }
            }
            c = fgetc(tf);
        }

    }
}

void readvicerc(FILE *tf, char *emu, int tag)
{
    char tmp[0x100];
    int c;
    ITEM *itm;
    DBG(("reading rc for '%s'\n",emu));
    fseek(tf,0,SEEK_SET);
    while(!feof(tf)) {
        skipuntil(tf, '[');
        c = fscanf(tf,"%s",tmp);
        if (c < 1) {
            break;
        }
        tmp[strlen(tmp)-1]=0;
        DBG2(("tag %d '%s'\n",c,tmp));
        if (!strcmp(emu, tmp)) {
            DBG(("found tag %d '%s'\n",c,tmp));
            break;
        }
    }

    while(!feof(tf)) {
        c = skipblank(tf);
        if (c == '[') {
            break;
        }
        tmp[0] = c;
        c = getstr(tf, &tmp[1]);
        DBG(("resource '%s'\n",tmp));
        itm = list_addstr(&reslistrc, tmp);
        itm->flags |= tag;
        skipuntil(tf, '\n');
    }

}

void readviceopt(FILE *tf, char *emu, int tag)
{
    char tmp[0x100];
    int c;
    size_t oldpos;
    ITEM *itm;
    DBG(("reading opts for '%s' from vice options\n",emu));
    fseek(tf,0,SEEK_SET);
    while(!feof(tf)) {
        skipuntil(tf, '[');
        c = fscanf(tf,"%20s",tmp);
        if (c < 1) {
            break;
        }
        tmp[strlen(tmp)-1]=0;
        DBG2(("tag %d '%s'\n",c,tmp));
        if (!strcasecmp(emu, tmp)) {
            DBG(("found tag %d '%s'\n",c,tmp));
            break;
        }
    }
    while(!feof(tf)) {
        c = skipblank(tf);
        if ((c=='-')||(c=='+')) {
            oldpos = ftell(tf);
            c = getstr(tf, &tmp[0]);
            DBG(("first option '%s'\n",tmp));
            if(!strcmp(tmp, "help")
               || !strcmp(tmp, "logfile")
               || !strcmp(tmp, "f")) {
                fseek(tf,oldpos-1,SEEK_SET);
                break;
            }
        }
        skipuntil(tf, '\n');
    }

    while(!feof(tf)) {
        c = skipblank(tf);
        if (c == '[') {
            break;
        }
        DBG2(("option '%c'\n",c));

        if ((c=='-')||(c=='+')) {
            tmp[0] = c;
            c = getstr(tf, &tmp[1]);

            DBG(("option '%s'\n",tmp));
            itm = list_addstr(&optlistvice, tmp);
            itm->flags |= tag;
        } else {
            c = getstr(tf, &tmp[0]);
            DBG(("not option '%s'\n",tmp));
        }
        skipuntil(tf, '\n');
    }

}

int printlist(ITEM *list, char *hdr, int hideplus, int flags)
{
    int i = 0,ii,n = 0,skipnext;
    while (list) {
        skipnext = 0;
        if (list->string) {
            if ((hideplus == 1) && (list->next != NULL) && (list->next->string != NULL)) {
                if (!strcmp(&list->string[1], &list->next->string[1])) {
                    skipnext = 1;
                }
            }
            if (flags) {
                if (list->flags == flags) {
                    if ( 1
                        && (strcmp(list->string, "-<version>") != 0)
                        ) {
                        if (i == 0) {
                            printf("\n[%s]\n\n", hdr);i++;
                        }
                        if (skipnext) printf("+/");
                        else printf("  ");
                        printf("%s\n", list->string);
                    }
                }
            } else {
                if (1
                    && (list->flags != IS_C64SC)
                    && (list->flags != IS_C64)
                    && (list->flags != IS_VSID)
                    && (list->flags != IS_C128)
                    && (list->flags != IS_DTV)
                    && (list->flags != IS_VIC20)
                    && (list->flags != IS_PET)
                    && (list->flags != IS_B500)
                    && (list->flags != IS_PLUS4)
                    && (list->flags != IS_CBM2)
                    && (list->flags != IS_SCPU64)
                    && (list->flags != IS_CARTCONV)
                    && (list->flags != IS_PETCAT)
                    && (list->flags != IS_C1541)
                    ){
                    if (i == 0) {
                        printf("\n%s\n\n", hdr);i++;
                    }
                    if (skipnext) printf("+/");
                    else printf("  ");
                    printf("%-40s", list->string);
                    for (i=0;i<NUMEMUS;i++) {
                        if (list->flags & (1<<i)) {
                            printf("%s  ", emustring[i]);
                        } else {
                            for (ii=0;ii<(strlen(emustring[i])+2);ii++) {
                                printf(" ");
                            }
                        }
                    }
                    printf("\n");
                }
            }
            n++;
        }
        if (skipnext) {
            list = list->next;
        }
        list = list->next;
    }
    return n;
}

int strinlist(char *str, const char *list[])
{
    while (*list) {
        if (!strcmp(str, *list)) {
            return 1;
        }
        ++list;
    }
    return 0;
}

void checkresources(void)
{
    ITEM *list1, *itm, *itm2;
    int i;

    printf("\n** checking resources...\n\n");

    printf("The following resources are incorrectly marked '@findex'\n"
           "fix them first to use '@vindex' and then check again:\n\n");

    list1 = &reslistrc;
    i = 0;
    while (list1) {
        DBG(("check: %s\n", list1->string));
        if (list1->string) {
            itm = list_findstr(&optlisttex2, list1->string);
            if (itm) {
                printf("%s\n", list1->string);
                i++;
            }
        }
        list1 = list1->next;
    }
    if (i == 0) {
        printf("none - well done.\n");
    }

    printf("\nThe following resources do not appear in '@vindex'.\n"
           "fix them first to use '@vindex' and then check again:\n\n");

    list1 = &reslisttexitm;
    i = 0;
    while (list1) {
        DBG(("check: %s\n", list1->string));
        if (list1->string) {
            itm = list_findstr(&reslistrc, list1->string);
            if (itm) {
                itm = list_findstr(&reslisttex, list1->string);
                if (!itm) {
                    printf("%s\n", list1->string);
                    i++;
                }
            }
        }
        list1 = list1->next;
    }
    if (i == 0) {
        printf("none - well done.\n");
    } else {
        printf("\nnote: each resource should get a seperate '@vindex' entry\n"
                 "      in the form '@vindex resourcename'.\n");
    }
    printf("\n");

    list1 = &reslistrc;
    while (list1) {
        DBG(("check: %s\n", list1->string));
        if (list1->string) {
            itm = list_findstr(&reslisttex, list1->string);
            if (itm) {
                DBG(("found resource: %s\n", list1->string));
            } else {
                DBG(("'%s'\n", list1->string));
                itm2=list_addstr(&reslistnew, list1->string);
                DBG(("'%s'\n", itm2->string));
                itm2->flags=list1->flags;
            }
        }
        list1 = list1->next;
    }
    printf("The following resources appear in vicerc but not in the documentation, so\n"
           "they might be missing in the documentation:\n\n");
    i = 0;
    i += printlist(&reslistnew, "global", 0, 0);
    i += printlist(&reslistnew, "C64SC", 0, IS_C64SC);
    i += printlist(&reslistnew, "C64", 0, IS_C64);
    i += printlist(&reslistnew, "VSID", 0, IS_VSID);
    i += printlist(&reslistnew, "C128", 0, IS_C128);
    i += printlist(&reslistnew, "C64DTV", 0, IS_DTV);
    i += printlist(&reslistnew, "VIC20", 0, IS_VIC20);
    i += printlist(&reslistnew, "PET", 0, IS_PET);
    i += printlist(&reslistnew, "CBM-II-5x0", 0, IS_B500);
    i += printlist(&reslistnew, "CBM-II", 0, IS_CBM2);
    i += printlist(&reslistnew, "PLUS4", 0, IS_PLUS4);
    i += printlist(&reslistnew, "SCPU64", 0, IS_SCPU64);

    if (i == 0) {
        printf("none - well done.\n");
    }
    printf("\n");

    printf("The following resources appear to have no description: ");

    list1 = &reslisttex;
    i = 0;
    while (list1) {
        if (list1->string) {
            if ((list1->desc == NULL) && (list1->alias == NULL)) {
                i++;
            }
        }
        list1 = list1->next;
    }
    printf("(%d)\n\n", i);

    list1 = &reslisttex;
    i = 0;
    while (list1) {
        if (list1->string) {
            if ((list1->desc == NULL) && (list1->alias == NULL)) {
                printf("%s\n", list1->string);
                i++;
            }
        }
        list1 = list1->next;
    }

    if (i == 0) {
        printf("none - well done.\n");
    }
    printf("\n");

    printf("The following resources appear in the documentation but not in vicerc, so\n"
           "they might be outdated or spelled incorrectly:\n\n");

    list1 = &reslisttex;
    i = 0;
    while (list1) {
        DBG(("check: %s\n", list1->string));
        if (list1->string) {
            itm = list_findstr(&reslistrc, list1->string);
            if (itm) {
                DBG(("found: %s\n", list1->string));
            } else {
                printf("%-40s", list1->string);
                if(0
                    || !strcmp(list1->string, "FFMPEGFormat")
                    || !strcmp(list1->string, "FFMPEGAudioBitrate")
                    || !strcmp(list1->string, "FFMPEGVideoBitrate")
                    || !strcmp(list1->string, "FFMPEGAudioCodec")
                    || !strcmp(list1->string, "FFMPEGVideoCodec")
                    || !strcmp(list1->string, "FFMPEGVideoHalveFramerate")
                  ) {
                    printf("(FFMPEG only, might be disabled)");
                } else if(0
                    || !strcmp(list1->string, "MIDIEnable")
                    || !strcmp(list1->string, "MIDIMode")
                    || !strcmp(list1->string, "MIDIInDev")
                    || !strcmp(list1->string, "MIDIOutDev")
                    || !strcmp(list1->string, "MIDIDriver")
                  ) {
                    printf("(MIDI only, might be disabled)");
                } else if(0
                    || !strcmp(list1->string, "KeepMonitorOpen")
                    || !strcmp(list1->string, "KeepAspectRatio")
                    || !strcmp(list1->string, "TrueAspectRatio")
                    || !strcmp(list1->string, "VSync")
                    || !strcmp(list1->string, "Window0Width")
                    || !strcmp(list1->string, "Window0Height")
                    || !strcmp(list1->string, "Window0Xpos")
                    || !strcmp(list1->string, "Window0Ypos")
                    || !strcmp(list1->string, "Window1Width")
                    || !strcmp(list1->string, "Window1Height")
                    || !strcmp(list1->string, "Window1Xpos")
                    || !strcmp(list1->string, "Window1Ypos")
                    || !strcmp(list1->string, "StartMinimized")
                    || !strcmp(list1->string, "DisplayDepth")
                    || !strcmp(list1->string, "GTKBackend")
                    || !strcmp(list1->string, "GTKFilter")
                    || !strcmp(list1->string, "FullscreenEnable")
                    || !strcmp(list1->string, "MonitorFG")
                    || !strcmp(list1->string, "MonitorBG")
                  ) {
                    printf("(GTK3 only, not SDL)");
                } else if(0
                    || !strcmp(list1->string, "AspectRatio")
                    || !strcmp(list1->string, "SDLKbdStatusbar")
                    || !strcmp(list1->string, "SDL2Renderer")
                    || !strcmp(list1->string, "SDLWindowWidth")
                    || !strcmp(list1->string, "SDLWindowHeight")
                    || !strcmp(list1->string, "SDLGLFilter")
                    || !strcmp(list1->string, "SDLStatusbar")
                    || !strcmp(list1->string, "SDLCustomWidth")
                    || !strcmp(list1->string, "SDLCustomHeight")
                    || !strcmp(list1->string, "SDLGLAspectMode")
                    || !strcmp(list1->string, "SDLGLFlipX")
                    || !strcmp(list1->string, "SDLGLFlipY")

                    || !strcmp(list1->string, "JoyMapFile")
                    || !strcmp(list1->string, "JoyThreshold")
                    || !strcmp(list1->string, "JoyFuzz")

                    || !strcmp(list1->string, "HotkeyFile")
                    || !strcmp(list1->string, "MenuKey")
                    || !strcmp(list1->string, "MenuKeyUp")
                    || !strcmp(list1->string, "MenuKeyDown")
                    || !strcmp(list1->string, "MenuKeyLeft")
                    || !strcmp(list1->string, "MenuKeyRight")
                    || !strcmp(list1->string, "MenuKeyPageUp")
                    || !strcmp(list1->string, "MenuKeyPageDown")
                    || !strcmp(list1->string, "MenuKeyHome")
                    || !strcmp(list1->string, "MenuKeyEnd")
                    || !strcmp(list1->string, "MenuKeySelect")
                    || !strcmp(list1->string, "MenuKeyCancel")
                    || !strcmp(list1->string, "MenuKeyExit")
                    || !strcmp(list1->string, "MenuKeyMap")

                    || !strcmp(list1->string, "CrtcSDLFullscreenMode")
                    || !strcmp(list1->string, "CrtcFullscreenDevice")
                    || !strcmp(list1->string, "CrtcFullscreen")
                    || !strcmp(list1->string, "CrtcFullscreenStatusbar")
                    || !strcmp(list1->string, "TEDSDLFullscreenMode")
                    || !strcmp(list1->string, "TEDFullscreenDevice")
                    || !strcmp(list1->string, "TEDFullscreen")
                    || !strcmp(list1->string, "TEDFullscreenStatusbar")
                    || !strcmp(list1->string, "VDCSDLFullscreenMode")
                    || !strcmp(list1->string, "VDCFullscreenDevice")
                    || !strcmp(list1->string, "VDCFullscreen")
                    || !strcmp(list1->string, "VDCFullscreenStatusbar")
                    || !strcmp(list1->string, "VICSDLFullscreenMode")
                    || !strcmp(list1->string, "VICFullscreenDevice")
                    || !strcmp(list1->string, "VICFullscreen")
                    || !strcmp(list1->string, "VICFullscreenStatusbar")
                    || !strcmp(list1->string, "VICIISDLFullscreenMode")
                    || !strcmp(list1->string, "VICIIFullscreenDevice")
                    || !strcmp(list1->string, "VICIIFullscreen")
                    || !strcmp(list1->string, "VICIIFullscreenStatusbar")

                  ) {
                    printf("(SDL only, not GTK3)");
                } else if(0
                    || !strcmp(list1->string, "SDLLimitMode")
                  ) {
                    printf("(SDL1 only, not GTK3)");
                } else if(0
                    || !strcmp(list1->string, "OverClock")
                    || !strcmp(list1->string, "UseFullscreen")
                    || !strcmp(list1->string, "FOURCC")
                    || !strcmp(list1->string, "MITSHM")
                    || !strcmp(list1->string, "openGL_sync")
                    || !strcmp(list1->string, "openGL_no_sync")
                    || !strcmp(list1->string, "XSync")
                    || !strcmp(list1->string, "UseXSync")
                    || !strcmp(list1->string, "PrivateColormap")
                  ) {
                    printf("(outdated?)");
                } else if(0
                    || !strcmp(list1->string, "TraceMode")
                    || !strcmp(list1->string, "AutoPlaybackFrames")
                    || !strcmp(list1->string, "MainCPU_TRACE")
                    || !strcmp(list1->string, "Drive0CPU_TRACE")
                    || !strcmp(list1->string, "Drive1CPU_TRACE")
                    || !strcmp(list1->string, "Drive2CPU_TRACE")
                    || !strcmp(list1->string, "Drive3CPU_TRACE")
                    || !strcmp(list1->string, "IEC_TRACE")
                    || !strcmp(list1->string, "DoCoreDump")
                    || !strcmp(list1->string, "DtvBlitterLog")
                    || !strcmp(list1->string, "DtvDMALog")
                    || !strcmp(list1->string, "DtvFlashLog")
                  ) {
                    printf("(DEBUG only, might be disabled)");
                } else {
                    i++;
                }
                printf("\n");
            }
        }
        list1 = list1->next;
    }
    printf("\n");

    if (i == 0) {
        printf("none - well done.\n");
    }
}

void printresources(void)
{
    ITEM *list1;
    int num, miss;

    printf("\n** listing resources...\n\n");

    list1 = &reslisttex;
    num = 0; miss = 0;
    while (list1) {
        if (list1->string) {
            printf ("%-35s  ", list1->string);
            if (list1->desc == NULL) {
                if (list1->alias) {
                    printf ("<see:%s>\n", list1->alias->string);
                } else {
                    printf ("<description missing>\n");
                    miss++;
                }
            } else {
                printf ("%s\n", list1->desc);
            }
            num++;
        }
        list1 = list1->next;
    }
    printf("\n");

    printf("%d total, %d missing descriptions\n", num, miss);
}

void printoptions(void)
{
    ITEM *list1;
    int num, miss;

    printf("\n** listing options...\n\n");

    list1 = &optlisttex;
    num = 0; miss = 0;
    while (list1) {
        if (list1->string) {
            printf ("%-35s  ", list1->string);
            if (list1->desc == NULL) {
                if (list1->alias) {
                    printf ("<see:%s>", list1->alias->string);
                } else {
                    printf ("<description missing>");
                    miss++;
                }
            } else {
                printf ("%s", list1->desc);
            }
            printf("\n");
            num++;
        }
        list1 = list1->next;
    }
    printf("\n");

    printf("%d total, %d missing descriptions\n", num, miss);
}



void checkoptions(void)
{
    ITEM *list1, *itm, *itm2;
    int i, skipnext;

    printf("\n** checking command line options...\n\n");

    printf("The following look like options, but they do not appear in '@findex'.\n"
           "fix them first to use '@findex' and then check again:\n\n");

    list1 = &optlisttexitm;
    i = 0;
    while (list1) {
        if (list1->string) {
            DBG(("check: '%s'\n", list1->string));
            itm = list_findstr(&optlisttex, list1->string);
            if (!itm) {
                if ( 1
                    && (strcmp(list1->string,"-") != 0)
                    && (strcmp(list1->string,"--") != 0)
                    && (strcmp(list1->string,"----") != 0)
                    && (strcmp(list1->string,"-<version>") != 0)
                ) {
                    printf("%s\n", list1->string);
                    i++;
                }
            }
        }
        list1 = list1->next;
    }
    if (i == 0) {
        printf("none - well done.\n");
    }
    printf("\n");

    printf("The following options appear in vice but not in the documentation, so\n"
           "they might be missing in the documentation (%d):\n\n", i);

    list1 = &optlistvice;
    i = 0;
    while (list1) {
        DBG(("check: %s\n", list1->string));
        if (list1->string) {
            itm = list_findstr(&optlisttex, list1->string);
            if (itm) {
                DBG(("found option: %s\n", list1->string));
            } else {
                if ( 1
                    && (strcmp(list1->string,"--") != 0)
                    && (strcmp(list1->string,"-<version>") != 0)
                ) {
                    DBG(("'%s'\n", list1->string));
                    itm2=list_addstr(&optlistnew, list1->string);
                    DBG(("'%s'\n", itm2->string));
                    itm2->flags=list1->flags;
                    i++;
                }
            }
        }
        list1 = list1->next;
    }

    if (i == 0) {
        printf("none - well done.\n");
    } else {
        printlist(&optlistnew, "global", 1, 0);
        printlist(&optlistnew, "C64SC", 1, IS_C64SC);
        printlist(&optlistnew, "C64", 1, IS_C64);
        printlist(&optlistnew, "VSID", 1, IS_VSID);
        printlist(&optlistnew, "C128", 1, IS_C128);
        printlist(&optlistnew, "C64DTV", 1, IS_DTV);
        printlist(&optlistnew, "VIC20", 1, IS_VIC20);
        printlist(&optlistnew, "PET", 1, IS_PET);
        printlist(&optlistnew, "CBM-II-5x0", 1, IS_B500);
        printlist(&optlistnew, "CBM-II", 1, IS_CBM2);
        printlist(&optlistnew, "PLUS4", 1, IS_PLUS4);
        printlist(&optlistnew, "SCPU64", 1, IS_SCPU64);

        printlist(&optlistnew, "petcat", 1, IS_PETCAT);
        printlist(&optlistnew, "cartconv", 1, IS_CARTCONV);
        printlist(&optlistnew, "c1541", 1, IS_C1541);
    }

    printf("\nThe following options appear to have no description: ");

    list1 = &optlisttex;
    i = 0;
    while (list1) {
        if (list1->string) {
            if (list1->desc == NULL) {
                i++;
            }
        }
        list1 = list1->next;
    }
    printf("(%d)\n\n", i);

    list1 = &optlisttex;
    i = 0;
    while (list1) {
        if (list1->string) {
            if (list1->desc == NULL) {
                printf("%s\n", list1->string);
                i++;
            }
        }
        list1 = list1->next;
    }

    if (i == 0) {
        printf("none - well done.\n");
    }
    printf("\n");

    printf("The following options appear in the documentation but not in vice, so\n"
           "they might be outdated or spelled incorrectly:\n\n");
#if 0
    list1 = &optlistvice;
    while (list1) {
        if (list1->string) {
            printf("%s\n", list1->string);
        }
        list1 = list1->next;
    }
#endif
    list1 = &optlisttex;
    i = 0; skipnext = 0;
    while (list1) {
        DBG(("check: %s\n", list1->string));
        if (list1->string) {
            itm = list_findstr(&optlistvice, list1->string);
            if (list1->next && !strcmp(&list1->string[1], &list1->next->string[1])) {
/*                printf("'%s' - '%s'\n", &list1->string[1], &list1->next->string[1]); */
                skipnext = 1;
            }
            if (itm) {
                DBG(("found: %s\n", list1->string));
            } else {
                if (skipnext) printf("+/");
                else printf("  ");
                printf("%-40s", list1->string);
                if(0
                    || !strcmp(list1->string, "-ffmpegaudiobitrate")
                    || !strcmp(list1->string, "-ffmpegvideobitrate")
                  ) {
                    printf("(FFMPEG only, might be disabled)");
                } else if(0
                    || !strcmp(list1->string, "-miditype")
                    || !strcmp(list1->string, "-midi")
                    || !strcmp(list1->string, "+midi")
                    || !strcmp(list1->string, "-midiin")
                    || !strcmp(list1->string, "-midiout")
                    || !strcmp(list1->string, "-mididrv")
                  ) {
                    printf("(MIDI only, might be disabled)");
                } else if(0
                    || !strcmp(list1->string, "-keepaspect")
                    || !strcmp(list1->string, "+keepaspect")
                    || !strcmp(list1->string, "-trueaspect")
                    || !strcmp(list1->string, "+trueaspect")
                    || !strcmp(list1->string, "-vsync")
                    || !strcmp(list1->string, "+vsync")
                    || !strcmp(list1->string, "-keepmonopen")
                    || !strcmp(list1->string, "+keepmonopen")
                    || !strcmp(list1->string, "-refreshonbreak")
                    || !strcmp(list1->string, "+refreshonbreak")
                    || !strcmp(list1->string, "-minimized")
                    || !strcmp(list1->string, "+minimized")
                    || !strcmp(list1->string, "-displaydepth")
                    || !strcmp(list1->string, "-gtkbackend")
                    || !strcmp(list1->string, "-gtkfilter")
                    || !strcmp(list1->string, "-fullscreen")
                    || !strcmp(list1->string, "+fullscreen")
                  ) {
                    printf("(GTK3 only, not SDL)");
                } else if(0
                    || !strcmp(list1->string, "-aspect")
                    || !strcmp(list1->string, "-statusbar")
                    || !strcmp(list1->string, "+statusbar")
                    || !strcmp(list1->string, "-kbdstatusbar")
                    || !strcmp(list1->string, "+kbdstatusbar")
                    || !strcmp(list1->string, "-sdlaspectmode")
                    || !strcmp(list1->string, "-sdlflipx")
                    || !strcmp(list1->string, "+sdlflipx")
                    || !strcmp(list1->string, "-sdlflipy")
                    || !strcmp(list1->string, "+sdlflipy")
                    || !strcmp(list1->string, "-joymap")
                    || !strcmp(list1->string, "-joythreshold")
                    || !strcmp(list1->string, "-joyfuzz")
                    || !strcmp(list1->string, "-sdlglfilter")
                    || !strcmp(list1->string, "+sdlglfilter")
                    || !strcmp(list1->string, "-sdl2renderer")
                    || !strcmp(list1->string, "-sdl2dualwindow")
                    || !strcmp(list1->string, "-paddles2inputjoyaxis")
                    || !strcmp(list1->string, "-hotkeyfile")
                    || !strcmp(list1->string, "-menukey")
                    || !strcmp(list1->string, "-menukeyup")
                    || !strcmp(list1->string, "-menukeydown")
                    || !strcmp(list1->string, "-menukeyleft")
                    || !strcmp(list1->string, "-menukeyright")
                    || !strcmp(list1->string, "-menukeypageup")
                    || !strcmp(list1->string, "-menukeypagedown")
                    || !strcmp(list1->string, "-menukeyhome")
                    || !strcmp(list1->string, "-menukeyend")
                    || !strcmp(list1->string, "-menukeyselect")
                    || !strcmp(list1->string, "-menukeycancel")
                    || !strcmp(list1->string, "-menukeyexit")
                    || !strcmp(list1->string, "-menukeymap")
                    || !strcmp(list1->string, "-sdlcustomw")
                    || !strcmp(list1->string, "-sdlcustomh")
                    || !strcmp(list1->string, "-sdlinitialw")
                    || !strcmp(list1->string, "-sdlinitialh")
                    || !strcmp(list1->string, "-CRTCSDLfullmode")
                    || !strcmp(list1->string, "-CRTCfulldevice")
                    || !strcmp(list1->string, "-CRTCfull")
                    || !strcmp(list1->string, "+CRTCfull")
                    || !strcmp(list1->string, "-TEDSDLfullmode")
                    || !strcmp(list1->string, "-TEDfulldevice")
                    || !strcmp(list1->string, "-TEDfull")
                    || !strcmp(list1->string, "+TEDfull")
                    || !strcmp(list1->string, "-VDCSDLfullmode")
                    || !strcmp(list1->string, "-VDCfulldevice")
                    || !strcmp(list1->string, "-VDCfull")
                    || !strcmp(list1->string, "+VDCfull")
                    || !strcmp(list1->string, "-VICSDLfullmode")
                    || !strcmp(list1->string, "-VICfulldevice")
                    || !strcmp(list1->string, "-VICfull")
                    || !strcmp(list1->string, "+VICfull")
                    || !strcmp(list1->string, "-VICIISDLfullmode")
                    || !strcmp(list1->string, "-VICIIfulldevice")
                    || !strcmp(list1->string, "-VICIIfull")
                    || !strcmp(list1->string, "+VICIIfull")
                  ) {
                    printf("(SDL only, not GTK3)");
                } else if(0
                    || !strcmp(list1->string, "-sdllimitmode")
                  ) {
                    printf("(SDL1 only, not GTK3)");
                } else if(0
                    || !strcmp(list1->string, "-mitshm")
                    || !strcmp(list1->string, "+mitshm")
                    || !strcmp(list1->string, "-fourcc")
                    || !strcmp(list1->string, "+fourcc")
                    || !strcmp(list1->string, "-xsync")
                    || !strcmp(list1->string, "+xsync")
                    || !strcmp(list1->string, "-colormap")
                    || !strcmp(list1->string, "+colormap")
                  ) {
                    printf("(outdated?)");
                } else if(0
                    || !strcmp(list1->string, "-debug")
                    || !strcmp(list1->string, "-trace_mode")
                    || !strcmp(list1->string, "-autoplaybackframes")
                    || !strcmp(list1->string, "-trace_maincpu")
                    || !strcmp(list1->string, "+trace_maincpu")
                    || !strcmp(list1->string, "-trace_drive0")
                    || !strcmp(list1->string, "+trace_drive0")
                    || !strcmp(list1->string, "-trace_drive1")
                    || !strcmp(list1->string, "+trace_drive1")
                    || !strcmp(list1->string, "-trace_drive2")
                    || !strcmp(list1->string, "+trace_drive2")
                    || !strcmp(list1->string, "-trace_drive3")
                    || !strcmp(list1->string, "+trace_drive3")
                    || !strcmp(list1->string, "-trace_iec")
                    || !strcmp(list1->string, "+trace_iec")
                    || !strcmp(list1->string, "-dtvblitterlog")
                    || !strcmp(list1->string, "+dtvblitterlog")
                    || !strcmp(list1->string, "-dtvdmalog")
                    || !strcmp(list1->string, "+dtvdmalog")
                    || !strcmp(list1->string, "-dtvflashlog")
                    || !strcmp(list1->string, "+dtvflashlog")
                  ) {
                    printf("(DEBUG only, might be disabled)");
                } else {
                    i++;
                }
                printf("\n");
            }
        }
        if (skipnext) {
            list1 = list1->next;
        }
        skipnext = 0;
        list1 = list1->next;
    }
    printf("\n");

    if (i == 0) {
        printf("none - well done.\n");
    }
}

char *vicercname;
char *viceoptname;
char *vicetexiname;
int checkopt = 0;
int checkres = 0;
int printopt = 0;
int printres = 0;

int main(int argc, char *argv[])
{
FILE *tf;
/*int n;*/

    if (argc != 5) {
        printf("checkdoc - scan vice.texi for some common problems\n\n");
        printf("usage: checkdoc [-all | -opt | -res] texifile vicerc optsfile\n");
        exit(-1);
    }
#if 0
    n = 0; while (n != 5) {
        printf("<%s> ", argv[n]);
        n++;
    }
    printf("\n");
#endif
    if (!strcmp(argv[1],"-all")) {
        checkopt++;
        checkres++;
        printopt++;
        printres++;
    }
    if (!strcmp(argv[1],"-opt")) {
        checkopt++;
    }
    if (!strcmp(argv[1],"-res")) {
        checkres++;
    }
    if (!strcmp(argv[1],"-listres")) {
        printres++;
    }
    if (!strcmp(argv[1],"-listopt")) {
        printopt++;
    }

    vicetexiname = argv[2];
    vicercname = argv[3];
    viceoptname = argv[4];

    printf("** initializing...\n\n");

    if ((tf = fopen(vicetexiname,"rb")) == NULL) {
        fprintf(stderr, "error: couldn't open %s.\n", vicetexiname);
        exit(-1);
    }
    printf("reading %s.\n", vicetexiname);
    readtexi(tf);
    fclose(tf);

    if ((tf = fopen(vicercname,"rb")) == NULL) {
        fprintf(stderr, "error: couldn't open %s.\n", vicercname);
        exit(-1);
    }
    printf("reading %s.\n", vicercname);

    readvicerc(tf,"PLUS4", IS_PLUS4);
    readvicerc(tf,"CBM-II", IS_CBM2);
    readvicerc(tf,"CBM-II-5x0", IS_B500);
    readvicerc(tf,"PET", IS_PET);
    readvicerc(tf,"VIC20", IS_VIC20);
    readvicerc(tf,"C64DTV", IS_DTV);
    readvicerc(tf,"C128", IS_C128);
    readvicerc(tf,"VSID", IS_VSID);
    readvicerc(tf,"C64", IS_C64);
    readvicerc(tf,"C64SC", IS_C64SC);
    readvicerc(tf,"SCPU64", IS_SCPU64);
    fclose(tf);

    tf = fopen(viceoptname,"rb");
    if (!tf) {
        fprintf(stderr, "error: couldn't open %s.\n", viceoptname);
        exit(-1);
    }
    printf("reading %s.\n", viceoptname);
    readviceopt(tf,"PLUS4", IS_PLUS4);
    readviceopt(tf,"CBM-II", IS_CBM2);
    readviceopt(tf,"CBM-II-5x0", IS_B500);
    readviceopt(tf,"PET", IS_PET);
    readviceopt(tf,"VIC20", IS_VIC20);
    readviceopt(tf,"C64DTV", IS_DTV);
    readviceopt(tf,"C128", IS_C128);
    readviceopt(tf,"VSID", IS_VSID);
    readviceopt(tf,"C64", IS_C64);
    readviceopt(tf,"C64SC", IS_C64SC);
    readviceopt(tf,"SCPU64", IS_SCPU64);
    readviceopt(tf,"petcat", IS_PETCAT);
    readviceopt(tf,"cartconv", IS_CARTCONV);
    readviceopt(tf,"c1541", IS_C1541);
    fclose(tf);

    if (checkres) {
        checkresources();
    }
    if (checkopt) {
        checkoptions();
    }
    if (printres) {
        printresources();
    }
    if (printopt) {
        printoptions();
    }
    return 0;
}
