/****************************************************************************

	DirMgr.c

	This file contains the C code to implement the DirectoryMgr system.

	This system is intended to manage filtered and sorted directory
	lists.

 ****************************************************************************/

/*
 * Author:
 * 	Brian Totty
 * 	Department of Computer Science
 * 	University Of Illinois at Urbana-Champaign
 *	1304 West Springfield Avenue
 * 	Urbana, IL 61801
 *
 * 	totty@cs.uiuc.edu
 *
 * (Sep 5, 1995: bugfix by Bert Bos <bert@let.rug.nl>, search for [BB])
 * (Apr 7, 1997: changed NO_REGEXP into !HAVE_REGEXP_H and autoconf.h added
 *               by Ettore Perazzoli <ettore@comm2000.it>)
 * (Apr 7, 1997: fixed behavior when regexp.h is not available by Ettore
 *		 Perazzoli <ettore@comm2000.it>, search for [EP])
 * (May 4, 1997: miscellaneous fixes by Ettore Perazzoli <ettore@comm2000.it>,
 *               search for [EP])
 * (Mar 19, 1998: disabled usage of regexp.h on {Free,Net}BSD.  Ettore
 *               Perazzoli <ettore@comm2000.it>, search for [EP])
 */

#include "autoconf.h"

#include <stdlib.h>		/* [EP] 05/04/97 */

#include "DirMgr.h"

/* [EP] 03/19/98 */
#if defined __FreeBSD__ || defined __NetBSD__
#undef HAVE_REGEXP_H
#endif

#ifdef HAVE_REGEXP_H
#include "RegExp.h"
#endif

#define	DIR_MGR_FSM_SIZE 1024

/*---------------------------------------------------------------------------*

                   S I M P L E    I N T E R F A C E

 *---------------------------------------------------------------------------*/

DirectoryMgr *DirectoryMgrSimpleOpen(path,sort_type,pattern)
char *path;
int sort_type;
char *pattern;
{
	DirectoryMgr *dm;
	PFI f_func,s_func;
	char *f_data;

	if (pattern == NULL) pattern = "*";
	if (!DirectoryMgrSimpleFilterFunc(pattern,&f_func,&f_data))
	{
		return(NULL);
	}
	if (!DirectoryMgrSimpleSortingFunc(sort_type,&s_func))
	{
		free(f_data);
		return(NULL);
	}
	dm = DirectoryMgrOpen(path,s_func,f_func,f_data,TRUE);
	return(dm);
} /* End DirectoryMgrSimpleOpen */


int DirectoryMgrSimpleRefilter(dm,pattern)
DirectoryMgr *dm;
char *pattern;
{
	PFI f_func;
	char *f_data;

	if (!DirectoryMgrSimpleFilterFunc(pattern,&f_func,&f_data))
	{
		return(FALSE);
	}
	DirectoryMgrRefilter(dm,f_func,f_data,TRUE);
	return(TRUE);
} /* End DirectoryMgrSimpleRefilter */


int DirectoryMgrSimpleResort(dm,sort_type)
DirectoryMgr *dm;
int sort_type;
{
	PFI c_func;

	if (!DirectoryMgrSimpleSortingFunc(sort_type,&c_func))
	{
		return(FALSE);
	}
	DirectoryMgrResort(dm,c_func);
	return(TRUE);
} /* End DirectoryMgrSimpleResort */


/*---------------------------------------------------------------------------*

                    N O R M A L    I N T E R F A C E

 *---------------------------------------------------------------------------*/

int DirectoryMgrCanOpen(path)
char *path;
{
	int status;
	Directory dir;

	status = DirectoryOpen(path,&dir);
	if (status == TRUE) DirectoryClose(&dir);
	return(status);
} /* End DirectoryMgrCanOpen */


DirectoryMgr *DirectoryMgrOpen(path,c_func,f_func,f_data,free_data)
char *path;
PFI c_func,f_func;
char *f_data;
int free_data;
{
	DirectoryMgr *dm;

	dm = (DirectoryMgr *)calloc(1,sizeof(DirectoryMgr));
	if (dm == NULL)
	{
		fprintf(stderr,"DirectoryMgrOpen: out of memory\n");
		if (free_data && f_data) free(f_data);
		return(NULL);
	}
	if (DirectoryOpen(path,DirectoryMgrDir(dm)) == FALSE)
	{
		fprintf(stderr,"DirectoryMgrOpen: can't open dir '%s'\n",
			DirectoryMgrDir(dm)->path); /* [EP] 05/04/97 */
		free(dm);
		if (free_data && f_data) free(f_data);
		return(NULL);
	}
	DirectoryMgrCompFunc(dm) = c_func;
	DirectoryMgrRefilter(dm,f_func,f_data,free_data);
	return(dm);
} /* End DirectoryMgrOpen */


void DirectoryMgrClose(dm)
DirectoryMgr *dm;
{
	free(DirectoryMgrData(dm));
	free(DirectoryMgrSortedPtrs(dm));
	if (DirectoryMgrFilterData(dm) && DirectoryMgrFreeFilterData(dm))
	{
		free(DirectoryMgrFilterData(dm));
	}
	DirectoryClose(DirectoryMgrDir(dm));
	free(dm);
} /* End DirectoryMgrClose */


int DirectoryMgrRefilter(dm,f_func,f_data,f_free)
DirectoryMgr *dm;
PFI f_func;
char *f_data;
int f_free;
{
	if (DirectoryMgrFilterData(dm) && DirectoryMgrFreeFilterData(dm))
	{
		free(DirectoryMgrFilterData(dm));
	}
	DirectoryMgrFilterFunc(dm) = f_func;
	DirectoryMgrFilterData(dm) = f_data;
	DirectoryMgrFreeFilterData(dm) = f_free;
	DirectoryMgrRefresh(dm);
	return 0;		/* [EP] 05/04/97 */
} /* End DirectoryMgrRefilter */


int DirectoryMgrRefresh(dm)
DirectoryMgr *dm;
{
	int err,data_size,ptrs_size,i;
	DirEntryCons *head,*tail,*cons;
	DirEntry *dm_data,**dm_ptrs;
	PFI f_func;
	char *f_data;

	tail = NULL;		/* make compiler happy [EP] 05/04/97 */
	DirectoryMgrTotalCount(dm) = 0;
	DirectoryMgrFilteredCount(dm) = 0;
	DirectoryRestart(DirectoryMgrDir(dm));
	if (DirectoryMgrData(dm)) free(DirectoryMgrData(dm));
	if (DirectoryMgrSortedPtrs(dm)) free(DirectoryMgrSortedPtrs(dm));
	head = NULL;
	f_func = DirectoryMgrFilterFunc(dm);
	f_data = DirectoryMgrFilterData(dm);
	while (1)
	{
		cons = (DirEntryCons *)malloc(sizeof(DirEntryCons));
		if (cons == NULL)
		{
			fprintf(stderr,
				"DirectoryMgrRefresh: Can't Alloc Cons\n");
			exit(-1);
		}
		err = DirectoryReadNextEntry(DirectoryMgrDir(dm),
					     &(cons->dir_entry));
		if (err == FALSE)
		{
			free(cons);
			break;
		}
		++ DirectoryMgrTotalCount(dm);
		if ((f_func == NULL) ||
		    (f_func && f_func(&(cons->dir_entry),f_data)))
		{
			cons->next = NULL;
			if (head == NULL)
				head = cons;
			    else
				tail->next = cons;
			tail = cons;
			++ DirectoryMgrFilteredCount(dm);
		}
		    else			/* Filter Failed */
		{
			free(cons);
		}
	}

	data_size = sizeof(DirEntry) * DirectoryMgrFilteredCount(dm);
	ptrs_size = sizeof(DirEntry *) * DirectoryMgrFilteredCount(dm);
	dm_data = (DirEntry *)malloc(data_size);
	dm_ptrs = (DirEntry **)malloc(ptrs_size);
	/*
	 * BUGFIX: Next line changed from:
	 * if ((dm_data == NULL) || (dm_ptrs == NULL))
	 * after a bug report by Andrew Robinson, Feb 13, '95 [BB]
	 */
	if (((dm_data == NULL) && (data_size > 0))
	    || ((dm_ptrs == NULL) && (ptrs_size > 0)))
	{
		fprintf(stderr,"DirectoryMgrRefresh: Out of memory\n");
		exit(1);
	}
	DirectoryMgrData(dm) = dm_data;
	DirectoryMgrSortedPtrs(dm) = dm_ptrs;

	for (i = 0; i < DirectoryMgrFilteredCount(dm); i++)
	{
		DirectoryMgrData(dm)[i] = head->dir_entry;
		DirectoryMgrSortedPtrs(dm)[i] = &(DirectoryMgrData(dm)[i]);
		cons = head->next;
		free(head);
		head = cons;
	}

	DirectoryMgrResort(dm,DirectoryMgrCompFunc(dm));
	DirectoryMgrRestart(dm);
	return(TRUE);
} /* End DirectoryMgrRefresh */


void DirectoryMgrResort(dm,c_func)
DirectoryMgr *dm;
PFI c_func;
{
	DirectoryMgrCompFunc(dm) = c_func;
	if (c_func != NULL)
	{
		qsort(DirectoryMgrSortedPtrs(dm),DirectoryMgrFilteredCount(dm),
		      sizeof(DirEntry *),DirectoryMgrCompFunc(dm));
	}
	DirectoryMgrRestart(dm);
} /* End DirectoryMgrResort */

/*---------------------------------------------------------------------------*

                  I T E R A T I O N    C O M M A N D S

 *---------------------------------------------------------------------------*/

int DirectoryMgrGotoItem(dm,i)
DirectoryMgr *dm;
int i;
{
	if (i < 0 || i >= DirectoryMgrFilteredCount(dm)) return(FALSE);
	DirectoryMgrCurrentIndex(dm) = i;
	return(TRUE);
} /* End DirectoryMgrGotoItem */


int DirectoryMgrGotoNamedItem(dm,name)
DirectoryMgr *dm;
char *name;
{
	int i;
	DirEntry *entry;

	for (i = 0; i < DirectoryMgrFilteredCount(dm); i++)
	{
		entry = DirectoryMgrSortedPtrs(dm)[i];
		if (strcmp(DirEntryFileName(entry),name) == 0)
		{
			DirectoryMgrCurrentIndex(dm) = i;
			return(TRUE);
		}
	}
	return(FALSE);
} /* End DirectoryMgrGotoNamedItem */


void DirectoryMgrRestart(dm)
DirectoryMgr *dm;
{
	DirectoryMgrCurrentIndex(dm) = 0;
} /* End DirectoryMgrRestart */


DirEntry *DirectoryMgrCurrentEntry(dm)
DirectoryMgr *dm;
{
	int index;

	index = DirectoryMgrCurrentIndex(dm);
	if (index < 0 || index >= DirectoryMgrFilteredCount(dm)) return(NULL);
	return(DirectoryMgrSortedPtrs(dm)[index]);
} /* End DirectoryMgrCurrentEntry */


DirEntry *DirectoryMgrNextEntry(dm)
DirectoryMgr *dm;
{
	int index;

	index = DirectoryMgrCurrentIndex(dm);
	if (index >= DirectoryMgrFilteredCount(dm)) return(NULL);
	++ DirectoryMgrCurrentIndex(dm);
	return(DirectoryMgrSortedPtrs(dm)[index]);
} /* End DirectoryMgrNextEntry */


DirEntry *DirectoryMgrPrevEntry(dm)
DirectoryMgr *dm;
{
	int index;

	index = DirectoryMgrCurrentIndex(dm) - 1;
	if (index < 0) return(NULL);
	-- DirectoryMgrCurrentIndex(dm);
	return(DirectoryMgrSortedPtrs(dm)[index]);
} /* End DirectoryMgrPrevEntry */

/*---------------------------------------------------------------------------*

                   U T I L I T Y    F U N C T I O N S

 *---------------------------------------------------------------------------*/

int DirectoryMgrSimpleFilterFunc(pattern,ff_ptr,fd_ptr)
char *pattern;
PFI *ff_ptr;
char **fd_ptr;
{
#ifdef HAVE_REGEXP_H
        char regexp[2048];

	*ff_ptr = DirectoryMgrFilterName;
	*fd_ptr = (char *)malloc(sizeof(char) * DIR_MGR_FSM_SIZE);
	if (*fd_ptr == NULL) return(FALSE);
	RegExpPatternToRegExp(pattern,regexp);
	RegExpCompile(regexp,*fd_ptr,DIR_MGR_FSM_SIZE);
#else
	/* [EP] 04/07/97 */
	*ff_ptr = *fd_ptr = NULL;
#endif
	return(TRUE);
} /* End DirectoryMgrSimpleFilterFunc */


int DirectoryMgrSimpleSortingFunc(sort_type,sf_ptr)
int sort_type;
PFI *sf_ptr;
{
	*sf_ptr = NULL;
	switch (sort_type)
	{
	    case DIR_MGR_SORT_NONE:
		break;
	    case DIR_MGR_SORT_NAME:
		*sf_ptr = DirectoryMgrCompareName;
		break;
	    case DIR_MGR_SORT_SIZE_ASCENDING:
		*sf_ptr = DirectoryMgrCompareSizeAscending;
		break;
	    case DIR_MGR_SORT_SIZE_DESCENDING:
		*sf_ptr = DirectoryMgrCompareSizeDescending;
		break;
	    case DIR_MGR_SORT_NAME_DIRS_FIRST:
		*sf_ptr = DirectoryMgrCompareNameDirsFirst;
		break;
	    case DIR_MGR_SORT_ACCESS_ASCENDING:
		*sf_ptr = DirectoryMgrCompareLastAccessAscending;
		break;
	    case DIR_MGR_SORT_ACCESS_DESCENDING:
		*sf_ptr = DirectoryMgrCompareLastAccessDescending;
		break;
	    default:
		fprintf(stderr,"Bad sort type %d\n",sort_type);
		return(FALSE);
	}
	return(TRUE);
} /* End DirectoryMgrSimpleSortingFunc */

/*---------------------------------------------------------------------------*

                    S O R T I N G    R O U T I N E S

 *---------------------------------------------------------------------------*/

int DirectoryMgrCompareName(e1p,e2p)
DirEntry **e1p,**e2p;
{
	return(strcmp(DirEntryFileName(*e1p),DirEntryFileName(*e2p)));
} /* End DirectoryMgrCompareName */


int DirectoryMgrCompareNameDirsFirst(e1p,e2p)
DirEntry **e1p,**e2p;
{
	if (DirEntryLeadsToDir(*e1p))
	{
		if (!DirEntryLeadsToDir(*e2p)) return(-1);
	}
	    else if (DirEntryLeadsToDir(*e2p))
	{
		return(1);
	}
	return(strcmp(DirEntryFileName(*e1p),DirEntryFileName(*e2p)));
} /* End DirectoryMgrCompareNameDirsFirst */


int DirectoryMgrCompareSizeAscending(e1p,e2p)
DirEntry **e1p,**e2p;
{
	if (DirEntryFileSize(*e1p) < DirEntryFileSize(*e2p))
		return (-1);
	    else if (DirEntryFileSize(*e1p) == DirEntryFileSize(*e2p))
		return (0);
	    else
		return (1);
} /* End DirectoryMgrCompareSizeAscending */


int DirectoryMgrCompareSizeDescending(e1p,e2p)
DirEntry **e1p,**e2p;
{
	if (DirEntryFileSize(*e1p) > DirEntryFileSize(*e2p))
		return (-1);
	    else if (DirEntryFileSize(*e1p) == DirEntryFileSize(*e2p))
		return (0);
	    else
		return (1);
} /* End DirectoryMgrCompareSizeDescending */


int DirectoryMgrCompareLastAccessAscending(e1p,e2p)
DirEntry **e1p,**e2p;
{
	return((long)DirEntryLastAccess(*e1p) >
	       (long)DirEntryLastAccess(*e2p));
} /* End DirectoryMgrCompareLastAccessAscending */


int DirectoryMgrCompareLastAccessDescending(e1p,e2p)
DirEntry **e1p,**e2p;
{
	return((long)DirEntryLastAccess(*e1p) <
	       (long)DirEntryLastAccess(*e2p));
} /* End DirectoryMgrCompareLastAccessDescending */

/*---------------------------------------------------------------------------*

                     F I L T E R    R O U T I N E S

 *---------------------------------------------------------------------------*/

int DirectoryMgrFilterName(de,fsm)
DirEntry *de;
char *fsm;
{
#ifdef HAVE_REGEXP_H
	return(RegExpMatch(DirEntryFileName(de),fsm));
#else
	return(TRUE);
#endif
} /* End DirectoryMgrFilterName */


