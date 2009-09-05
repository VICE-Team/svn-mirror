/*****************************************************************************
 *
 * SOURCE FILE NAME = GRADD.H
 *
 * DESCRIPTIVE NAME = Structures and defines for the GRADD Architecture
 *
 * Copyright : COPYRIGHT IBM CORPORATION, 1996
 *             LICENSED MATERIAL - PROGRAM PROPERTY OF IBM
 *             REFER TO COPYRIGHT INSTRUCTION FORM#G120-2083
 *
 * ==========================================================================
 *
 * VERSION = 1.0
 *
 * DESCRIPTION
 *      This file contains all of the defines and
 *      data structures that are shared between the
 *      video manager and the translation layers.
 *
 * FUNCTIONS
 *
 * NOTES
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 * EXTERNAL FUNCTIONS
 *
 * CHANGE ACTIVITY =
 *  DATE      FLAG        APAR   CHANGE DESCRIPTION
 *  --------  ----------  -----  --------------------------------------
 *  mm/dd/yy  @Vr.mpppxx  xxxxx  xxxxxxx
 *
 ****************************************************************************/

#ifndef  GRADD_INCLUDED
#define GRADD_INCLUDED

#ifndef WATCOM_COMPILE
#include <ddi.h>
#endif

/*
 **  Prototype declarations for low level driver function call table.
 */
typedef ULONG CID;             // Chain ID
typedef ULONG GrID;            // Gradd ID

#define GrID_DONTCARE 0xffffffff

typedef ULONG (EXPENTRY FNHWCMD)(PVOID, PVOID);
typedef ULONG (EXPENTRY FNHWENTRY)(GrID, ULONG, PVOID, PVOID);
typedef ULONG (EXPENTRY FNSDBITBLT)(PVOID, PVOID);
typedef ULONG (EXPENTRY FNSDLINE)(PVOID, PVOID);

typedef FNHWENTRY *PFNHWENTRY;

typedef struct _GDDMODEINFO {     /* gddmodeinfo */
    ULONG ulLength;
    ULONG ulModeId;               /* used to make SETMODE request     */
    ULONG ulBpp;                  /* no of colors (bpp)               */
    ULONG ulHorizResolution;      /* horizontal pels                  */
    ULONG ulVertResolution;       /* vertical scan lines              */
    ULONG ulRefreshRate;          /* in Hz (0 if not available)       */
    PBYTE pbVRAMPhys;             /* physical address of VRAM         */
    ULONG ulApertureSize;         /* Current bank size                */
    ULONG ulScanLineSize;         /* size (in bytes) of one scan line */
    ULONG fccColorEncoding;       /* Pel format (defines above        */
    ULONG ulTotalVRAMSize;        /* Total size of VRAM in bytes      */
    ULONG cColors;                /* Total number of colors           */
} GDDMODEINFO;
typedef GDDMODEINFO *PGDDMODEINFO;

/*
 **  Structure for HWEntry router, see gdddata.c for array.
 */
typedef struct _HWCMD {   /* hwcmd */
    FNHWCMD *pfnHWCmd;
    ULONG ulRc;           /* return code if pfnHWCmd is NULL */
                          /* ring change otherwise           */
} HWCMD;
typedef HWCMD *PHWCMD;

/*
 ** Ring change codes
 */
#define DONTCARE 0
#define RING2    2

/*
 **  Prototype declarations for low level driver routines
 */
FNHWENTRY HWEntry;

/*
 **  Graphics Hardware Interface (GHI) commands for
 **  the Base Function class.
 */
#define BASE_FUNCTION_CLASS    "Base Function"

#define GHI_CMD_INIT       0
#define GHI_CMD_INITPROC   1
#define GHI_CMD_TERM       2
#define GHI_CMD_TERMPROC   3
#define GHI_CMD_QUERYCAPS  4
#define GHI_CMD_QUERYMODES 5
#define GHI_CMD_SETMODE     6
#define GHI_CMD_PALETTE     7
#define GHI_CMD_BITBLT     8
#define GHI_CMD_LINE       9
#define GHI_CMD_MOVEPTR    10
#define GHI_CMD_SETPTR     11
#define GHI_CMD_SHOWPTR    12
#define GHI_CMD_VRAM       13
#define GHI_CMD_REQUESTHW  14
#define GHI_CMD_EVENT      15
#define GHI_CMD_EXTENSION  16
#define GHI_CMD_BANK       17
#define GHI_CMD_TEXT       18
#define GHI_CMD_USERCAPS   19
#define GHI_CMD_POLYGON    20
#define GHI_CMD_MAX        21

/*
 **  Defines and structures for the GHI_CMD_INIT DDI
 */
typedef struct _GDDINITIN {         /* gddinitin */
    ULONG ulLength;
    PFNHWENTRY pfnChainedHWEntry;
} GDDINITIN;
typedef GDDINITIN *PGDDINITIN;

typedef struct _GDDINITOUT {        /* gddinitout */
    ULONG ulLength;
    ULONG cFunctionClasses;
} GDDINITOUT;
typedef GDDINITOUT *PGDDINITOUT;

/*
 **  Defines and structures for the GHI_CMD_INITPROC DDI
 */
typedef struct _INITPROCOUT {         /* initprocout */
    ULONG ulLength;
    ULONG ulVRAMVirt;
} INITPROCOUT;
typedef INITPROCOUT *PINITPROCOUT;

/*
 **  Defines and structures for the GHI_CMD_QUERYCAPS DDI
 */
typedef struct _CAPSINFO {    /* capsinfo */
    ULONG ulLength;           /* sizeof CAPSINFO structure      */
    PSZ pszFunctionClassID;   /* Name describing function set   */
    ULONG ulFCFlags;          /* Function class specific flags  */
} CAPSINFO;
typedef CAPSINFO *PCAPSINFO;

/*
 ** Defines for ulFCFlags field of the CAPSINFO data structure
 ** NOTE: The flags below are GRADD Caps for the base function class.
 */
#define GC_SEND_MEM_TO_MEM  0x00000001     // GRADD wants to see M2M blits
#define GC_SIM_SRC_PAT_ROPS 0x00000002     // GRADD wants to simulate 3way rops as sequence of 2way rops.
//RESERVED - NOT USED NOW
#define GC_ALPHA_SUPPORT    0x00000004     // GRADD supports alpha blending rules and
                                           // ALPHABITBLTINFO.
                                           // If this flag is not returned,
                                           // VMAN will never call the Gradd for
                                           // Alpha Blending.
// SHOULD match DS2_* in pmddi.h
#define GC_SRC_STRETCH      0x00000008     // GRADD handles stretchblts
#define GC_POLYGON_SIZE_ANY 0x00000010     // GRADD can handle polygon(concave or convex)
                                           // of any size.
                                           // GC_POLYGON_FILL must be set
#define GC_CLIP             0x00000020                 // GRADD can handle single clip rect of
                                           // polygons for pfnPolygon and
                                           // source bitmap for pfnBitblt with
                                           // GC_SRC_STRETCH set.
                                           // see GC_CLP_COMPLEX
#define GC_CLIP_COMPLEX     0x00000040     // GRADD can handle clipping
                                           // with more than one clip rect
                                           // GC_CLIP must be set.


/* following defines GC_* SHOULD match DS_* in pmddi.h */
#define GC_TEXTBLT_DOWNLOADABLE    0x00002000     // Downloadable Fonts
#define GC_TEXTBLT_CLIPABLE        0x00004000     // CLIPPABLE    Fonts
#define GC_TEXTBLT_DS_DEVICE_FONTS 0x00008000     // Device has Hardware Fonts
#define GC_SIMPLE_LINES            0x00800000     // Device handles LINEINFO2.

// For compatibility with old source code retain old definitions
#define TEXTBLT_DOWNLOADABLE       GC_TEXTBLT_DOWNLOADABLE
#define TEXTBLT_CLIPABLE           GC_TEXTBLT_CLIPABLE
#define TEXTBLT_DS_DEVICE_FONTS    GC_TEXTBLT_DS_DEVICE_FONTS

#ifndef DS_SIMPLE_LINES
#define DS_SIMPLE_LINES GC_SIMPLE_LINES
#endif
//RESERVED - NOT USED NOW
#define GC_SRC_CONVERT  0x02000000     // send Device independent bitmaps
                                       // without converting to device internal format.
                                       // Gradd will convert during blts or
                                       // call softdraw to simulate.
                                       // If not set, GRE will convert DIB in a temporary buffer.
                                       // Gradd should set this based on performance.
#define GC_POLYGON_FILL 0x08000000     // GRADD can handle polygon(triangle and quad)
                                       // fills and POLYGONINFO. see GC_POLYGON_SIZE_ANY
                                       // if set, Graphics Engine call pfnPolygon
                                       // for fills instead of pfnBitBlt.

/*
 **  Defines and structures for the GHI_CMD_QUERYMODES and GHI_CMD_SETMODE
 */
#define QUERYMODE_NUM_MODES 0x0001
#define QUERYMODE_MODE_DATA 0x0002
#define QUERYMODE_VALID     (QUERYMODE_NUM_MODES | QUERYMODE_MODE_DATA)

/*
 **  Defines and structures for the GHI_CMD_VRAM
 */
typedef struct _VRAMIN {         /* vramin */
    ULONG ulLength;
    ULONG ulFunction;
    PVOID pIn;
} VRAMIN;
typedef VRAMIN *PVRAMIN;

/*
 **  Defines for the ulFunction field of the VRAMIN data structure.
 */
#define VRAM_ALLOCATE   1
#define VRAM_DEALLOCATE 2
#define VRAM_QUERY      3
#define VRAM_REGISTER   4
#define VRAM_DEREGISTER 5

typedef struct _VRAMREGISTERIN {  /* vramregisterin */
    ULONG ulLength;               /* sizeof(VRAMREGISTERIN) */
    ULONG ulHandle;               /* de-registering handle  */
    ULONG ulFlags;                /* Input flags            */
} VRAMREGISTERIN;
typedef VRAMREGISTERIN *PVRAMREGISTERIN;

/*
 **  Defines for the ulFlags field of the VRAMREGISTERIN data structure.
 */
#define VRAM_REGISTER_HANDLE   0x0001   /* If this bit is set we have  */
                                        /* a registration if it is off */
                                        /* we have a deregistration    */

#define VRAM_REGISTER_VRAMONLY 0x1000   /* If this flag is set the app */
                                        /* is registering and telling  */
                                        /* us that it does not intend  */
                                        /* to use the accelerator      */

typedef struct _VRAMREGISTEROUT {   /* vramregisterout */
    ULONG ulLength;
    ULONG ulFlags;                  /* Returned flags (currently none def) */
    ULONG ulHandle;                 /* Returned handle                     */
} VRAMREGISTEROUT;
typedef VRAMREGISTEROUT *PVRAMREGISTEROUT;

typedef struct _VRAMALLOCIN {    /* vramallocin */
    ULONG ulLength;
    ULONG ulFlags;
    ULONG ulID;                  /* Used for de-allocation */
    ULONG ulFunction;
    ULONG ulHandle;
    ULONG ulSize;
    ULONG ulWidth;
    ULONG ulHeight;
} VRAMALLOCIN;
typedef VRAMALLOCIN *PVRAMALLOCIN;

/*
 **  Defines for the ulFlags field of the VRAMALLOCIN data structure.
 */
#define VRAM_ALLOC_SHARED     0x0001
#define VRAM_ALLOC_RECTANGLE  0x0002
#define VRAM_ALLOC_WORKBUFFER 0x0004
#define VRAM_ALLOC_STATIC     0x1000

/*
 **  Defines for the ulFunction field of the VRAMALLOCIN data structure.
 */
#define VRAM_ALLOCATE   1
#define VRAM_DEALLOCATE 2
#define VRAM_QUERY      3

typedef struct _VRAMALLOCOUT {   /* vramallocout */
    ULONG ulLength;
    ULONG ulFlags;
    ULONG ulID;
    POINTL ptlStart;
    ULONG ulSize;
    ULONG ulScanLineBytes;
} VRAMALLOCOUT;
typedef VRAMALLOCOUT *PVRAMALLOCOUT;

/*
 **  Defines and structures for the GHI_CMD_EVENT command
 */
typedef struct _HWEVENTIN {      /* hweventin  */
    ULONG ulLength;
    ULONG ulEvent;               /* Event type                     */
    PVOID pEventData;            /* Pointer to event specific data */
} HWEVENTIN;
typedef HWEVENTIN *PHWEVENTIN;

/*
 **  Defines for the ulEvent field of the HWEVENTIN data structure.
 */
#define EVENT_FOREGROUND   1
#define EVENT_BACKGROUND   2
#define EVENT_NEWCHAININFO 3

/*
 **  Defines and structures for the GHI_CMD_EXTENSION command
 */
typedef struct _HWEXTENSION {          /* hwextension */
    ULONG ulLength;
    ULONG ulXSubFunction;
    ULONG cScrChangeRects;
    PRECTL arectlScreen;
    ULONG ulXFlags;
    PVOID pXP1;
} HWEXTENSION;
typedef HWEXTENSION *PHWEXTENSION;

/*
 **  Defines for the ulXFlags field of the HWEXTENSION data structure.
 */
#define X_REQUESTHW 1

/*
 ** GRADD.SYS defines        
 */

#define GRADDPDD_IOCTL   0x10
#define GRADDPDD_LOCK    0
#define GRADDPDD_UNLOCK  1
#define GRADDPDD_SETMTRR 2

typedef struct _MTRRIN {
    ULONG ulPhysAddr;            /* Starting Physical address              */
    ULONG ulLength;              /* Range to be enabled for WC             */
    ULONG ulFlags;               /* Flags as defined above. Bit 27 is      */
                                 /* interrpreted as a Enable MTRR if set   */
                                 /* and as a Disable MTRR if not set       */
} MTRRIN, *PMTRRIN;

ULONG GHSetMTRR(PMTRRIN, BOOL);

/*
 ** The following are the possible values for the flags which are returned when
 ** calling GRADDPDD_SETMTRR these are also defined in GRADDSYS.H for use by 
 ** GRADD.SYS please make sure any updates are kept in synch.
 **
 ** The first(low order byte) is reserved for Processor Identification and is 
 ** interpreted as a number.  The 2nd byte is reserved for future use. The
 ** 3rd byte is interpreted as a set of flags indicating what support is 
 ** available and the high order byte is interpreted as a number corresponding
 ** to the command passed in to the SetMTRR function.
 */
#define INTEL_PROCESSOR 0x00000001
#define AMD_PROCESSOR   0x00000002
#define CYRIX_PROCESSOR 0x00000003
#define CPUID_SUPPORT   0x00010000
#define MTRR_SUPPORT    0x00020000
#define MTRR_ENABLED    0x00040000
#define MTRR_AVAILABLE  0x00080000
#define WC_ENABLED      0x00100000

#define VENDOR_MASK  0x000000FF
#define COMMAND_MASK 0xFF000000

/*
 ** Or one of the following into the ulFlags field of MTRRIN to 
 ** enabled/disable WC
 */
#define MTRR_DISABLE 0x00000000
#define MTRR_ENABLE  0x01000000
#define MTRR_QUERY   0x02000000
#define MTRR_CMD_MAX MTRR_QUERY
  
/*
 ** End of GRADD.SYS defines 
 */

/*
 **  Prototype declarations for Video Manager Interface (VMI)
 */

typedef ULONG (EXPENTRY FNVMIENTRY)(GrID, ULONG, PVOID, PVOID);

/*
 **  VMI commands which map directly to GRADD commands
 */
#define VMI_CMD_INIT       GHI_CMD_INIT
#define VMI_CMD_INITPROC   GHI_CMD_INITPROC
#define VMI_CMD_TERM       GHI_CMD_TERM
#define VMI_CMD_TERMPROC   GHI_CMD_TERMPROC
#define VMI_CMD_QUERYCAPS  GHI_CMD_QUERYCAPS
#define VMI_CMD_QUERYMODES GHI_CMD_QUERYMODES
#define VMI_CMD_SETMODE    GHI_CMD_SETMODE
#define VMI_CMD_PALETTE    GHI_CMD_PALETTE
#define VMI_CMD_BITBLT     GHI_CMD_BITBLT
#define VMI_CMD_LINE       GHI_CMD_LINE
#define VMI_CMD_MOVEPTR    GHI_CMD_MOVEPTR
#define VMI_CMD_SETPTR     GHI_CMD_SETPTR
#define VMI_CMD_SHOWPTR    GHI_CMD_SHOWPTR
#define VMI_CMD_VRAM       GHI_CMD_VRAM
#define VMI_CMD_REQUESTHW  GHI_CMD_REQUESTHW
#define VMI_CMD_EVENT      GHI_CMD_EVENT
#define VMI_CMD_EXTENSION  GHI_CMD_EXTENSION
#define VMI_CMD_BANK       GHI_CMD_BANK
#define VMI_CMD_TEXT       GHI_CMD_TEXT
#define VMI_CMD_USERCAPS   GHI_CMD_USERCAPS
#define VMI_CMD_POLYGON    GHI_CMD_POLYGON

/*
 **  VMI commands unique to the video manager
 */
#define VMI_CMD_BASE 0x1000

#define VMI_CMD_QUERYCHAININFO   VMI_CMD_BASE + 0
#define VMI_CMD_QUERYCURRENTMODE VMI_CMD_BASE + 1
#define VMI_CMD_EXCLUDEPTR       VMI_CMD_BASE + 2
#define VMI_CMD_UPDATECHAININFO  VMI_CMD_BASE + 3
#define VMI_CMD_MAX              VMI_CMD_BASE + 4

/*
 **  Defines and structures for VMI_CMD_QUERYCHAININFO
 */
typedef struct _GRADDINFO {       /* graddinfo */
    GrID gid;
    PSZ pszGraddName;
    PFNHWENTRY pGraddEntry;
    PFNHWENTRY pChainEntry;
    ULONG cModes;
    struct _GDDMODEINFO *pModeInfo;
    struct _CAPSINFO *pCapsInfo;
    struct _GRADDINFO *pNextGraddInfo;
} GRADDINFO ;
typedef GRADDINFO *PGRADDINFO;

typedef struct _CHAININFO {       /* chaininfo */
    CID cid;
    PSZ pszChainName;
    PFNHWENTRY pChainHWEntry;
    PGRADDINFO pGraddList;
    struct _CHAININFO *pNextChainInfo;
  } CHAININFO ;
typedef CHAININFO *PCHAININFO;

typedef struct _VMIQCI {          /* vmiqci */
    ULONG ulLength;
    PCHAININFO pciHead;           /* Head of the GRADD chain      */
} VMIQCI;
typedef VMIQCI  *PVMIQCI;

/*
 **  Defines and structures for VMI_CMD_EXCLUDEPTR
 */
typedef struct _EXCLUDEPTRIN {       /* excludeptrin                    */
    ULONG ulLength;                  /* sizeof(EXCLUDEPTRIN)            */
    BOOL fAdd;                       /* TRUE - add, FASE - remove rects */
    ULONG cExcludeRects;             /* count of exclude rectangles     */
    PRECTL apRectls;                 /* array of exclude rectangles     */
} EXCLUDEPTRIN;
typedef EXCLUDEPTRIN *PEXCLUDEPTRIN;

/*
 ** Video Helper services exported by VMAN
 */
typedef struct _MEMINFO    {   /* meminfo */
    ULONG ulPhysAddr;
    ULONG ulMemLen;
    ULONG ulVirtAddr;
    PVOID pNextMI;
} MEMINFO;
typedef MEMINFO *PMEMINFO;

ULONG VHMap(PVOID, ULONG);
ULONG VHMapVRAM(ULONG, ULONG, PBYTE *, PBYTE *);
ULONG VHLockMem(PVOID, ULONG, BOOL);
ULONG VHUnLockMem(ULONG);
PVOID VHAllocMem(ULONG);
ULONG VHFreeMem(PVOID);
ULONG VHPhysToVirt(PMEMINFO);
ULONG VHGetVRAMInfo(PMEMINFO);
ULONG VHSetMTRR(PMTRRIN, BOOL);
ULONG EXPENTRY VMIEntry(GrID, ULONG, PVOID, PVOID);
ULONG EXPENTRY VHSortTriangle(PPOINTL pptl, PPOINTL pptlBounds);
ULONG EXPENTRY VHSortQuad(PPOINTL pptl, PPOINTL pptlBounds);

// Defines for xxxxxx

#define TRAPEZOID_COLLINEAR          0x0000
#define TRAPEZOID_TRIANGLE           0x0001
#define TRAPEZOID_TWO_TRIANGLES      0x0002
#define TRAPEZOID_DIAMOND            0x0004
#define TRAPEZOID_MINY_HORIZONTAL    0x0010
#define TRAPEZOID_MAXY_HORIZONTAL    0x0020
#define TRAPEZOID_LEFT_SPLIT         0x0040
#define TRAPEZOID_RIGHT_SPLIT        0x0080
#define TRAPEZOID_MIDY_HORIZONTAL    0x0100
#define TRAPEZOID_SECOND_SPLIT_LEFT  0x0200
#define TRAPEZOID_SECOND_SPLIT_RIGHT 0x0400

/*
 **  Defines for the ulFlags field of the HEADER data structure.
 */
#define SERIALIZE_REQUIRED 0x1

#define QUERYCAPS     1L
#define QUERYCAPSLIST 2L
#define SETCAP        3L

typedef struct _USERCAPSIN {
    ULONG ulLength;
    ULONG ulFunction;
    ULONG ulSize;
} USERCAPSIN;
typedef USERCAPSIN *PUSERCAPSIN;

#endif   /* ifndef GRADD_INCLUDED */
