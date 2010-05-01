/*
 * $Id: directory.h,v 1.34 2010/03/12 15:16:49 franklahm Exp $
 *
 * Copyright (c) 1990,1991 Regents of The University of Michigan.
 * All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation, and that the name of The University
 * of Michigan not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. This software is supplied as is without expressed or
 * implied warranties of any kind.
 *
 *	Research Systems Unix Group
 *	The University of Michigan
 *	c/o Mike Clark
 *	535 W. William Street
 *	Ann Arbor, Michigan
 *	+1-313-763-0525
 *	netatalk@itd.umich.edu
 */

#ifndef AFPD_DIRECTORY_H
#define AFPD_DIRECTORY_H 1

#include <sys/cdefs.h>
#include <sys/types.h>
/*#include <sys/stat.h>*/ /* including it here causes some confusion */
#include <netatalk/endian.h>
#include <dirent.h>

/* sys/types.h usually snarfs in major/minor macros. if they don't
 * try this file. */
#ifndef major
#include <sys/sysmacros.h>
#endif

#include <atalk/directory.h>

#include "globals.h"
#include "volume.h"

#define DIRTREE_COLOR_RED    0
#define DIRTREE_COLOR_BLACK  1

#define DIRF_FSMASK	(3<<0)
#define DIRF_NOFS	(0<<0)
#define DIRF_AFS	(1<<0)
#define DIRF_UFS	(2<<0)

#define DIRF_OFFCNT     (1<<4)	/* offsprings count is valid */
#define DIRF_CNID	(1<<5)  /* renumerate id */

#define AFPDIR_READ	(1<<0)

/* directory bits */
#define DIRPBIT_ATTR	0
#define DIRPBIT_PDID	1
#define DIRPBIT_CDATE	2
#define DIRPBIT_MDATE	3
#define DIRPBIT_BDATE	4
#define DIRPBIT_FINFO	5
#define DIRPBIT_LNAME	6
#define DIRPBIT_SNAME	7
#define DIRPBIT_DID	8
#define DIRPBIT_OFFCNT	9
#define DIRPBIT_UID	10
#define DIRPBIT_GID	11
#define DIRPBIT_ACCESS	12
#define DIRPBIT_PDINFO  13         /* ProDOS Info */
#define DIRPBIT_UNIXPR  15

#define FILDIRBIT_ISDIR        (1 << 7) /* is a directory */
#define FILDIRBIT_ISFILE       (0)      /* is a file */

/* reserved directory id's */
#define DIRDID_ROOT_PARENT    htonl(1)  /* parent directory of root */
#define DIRDID_ROOT           htonl(2)  /* root directory */

/* file/directory ids. what a mess. we scramble things in a vain attempt
 * to get something meaningful */
#ifndef AFS

#if 0
#define CNID_XOR(a)  (((a) >> 16) ^ (a))
#define CNID_DEV(a)   ((((CNID_XOR(major((a)->st_dev)) & 0xf) << 3) | \
	(CNID_XOR(minor((a)->st_dev)) & 0x7)) << 24)
#define CNID_INODE(a) (((a)->st_ino ^ (((a)->st_ino & 0xff000000) >> 8)) \
				       & 0x00ffffff)
#define CNID_FILE(a)  (((a) & 0x1) << 31)
#define CNID(a,b)     (CNID_DEV(a) | CNID_INODE(a) | CNID_FILE(b))
#endif

#define CNID(a,b)     ((a)->st_ino & 0xffffffff)

#else /* AFS */
#define CNID(a,b)     (((a)->st_ino & 0x7fffffff) | CNID_FILE(b))
#endif /* AFS */


struct maccess {
    u_char	ma_user;
    u_char	ma_world;
    u_char	ma_group;
    u_char	ma_owner;
};

#define	AR_USEARCH	(1<<0)
#define	AR_UREAD	(1<<1)
#define	AR_UWRITE	(1<<2)
#define	AR_UOWN		(1<<7)

extern struct dir       *dirnew (const char *, const char *);
extern void             dirfreename (struct dir *);
extern void             dirfree (struct dir *);
extern struct dir	*dirsearch (const struct vol *, u_int32_t);
extern struct dir	*dirlookup (struct vol *, u_int32_t);
extern struct dir       *dirsearch_byname (const struct vol *, struct dir *,char *);

extern struct dir	*adddir (struct vol *, struct dir *, 
                                               struct path *);

extern int              movecwd (struct vol *, struct dir *);
extern int              deletecurdir (struct vol *);
extern struct path      *cname (struct vol *, struct dir *,
                             char **);
extern mode_t           mtoumode (struct maccess *);
extern void             utommode (struct stat *, struct maccess *);
extern int getdirparams (const struct vol *, u_int16_t, struct path *,
                                 struct dir *, char *, size_t *);
extern int setdirparams (struct vol *, struct path *, u_int16_t, char *);
extern int renamedir(const struct vol *, int, char *, char *, struct dir *,
                     struct dir *, char *);
extern int path_error (struct path *, int error);

extern void setdiroffcnt (struct dir *dir, struct stat *st,  u_int32_t count);
extern int dirreenumerate (struct dir *dir, struct stat *st);

typedef int (*dir_loop)(struct dirent *, char *, void *);

extern int  for_each_dirent (const struct vol *, char *, dir_loop , void *);

extern int  check_access (char *name , int mode);
extern int file_access   (struct path *path, int mode);

extern int netatalk_unlink (const char *name);

extern int caseenumerate (const struct vol *, struct path *, struct dir *);

extern hash_t *dirhash (void);
/* from enumerate.c */
extern char *check_dirent (const struct vol *, char *);

/* FP functions */
int afp_createdir (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);
int afp_opendir (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);
int afp_setdirparams (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);
int afp_closedir (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);
int afp_mapid (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);
int afp_mapname (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);
int afp_syncdir (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);

/* from enumerate.c */
int afp_enumerate (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);
int afp_enumerate_ext (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);
int afp_enumerate_ext2 (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);

/* from catsearch.c */
int afp_catsearch (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);
int afp_catsearch_ext (AFPObj *obj, char *ibuf, size_t ibuflen, char *rbuf,  size_t *rbuflen);

#endif
