/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/* manifest.h - manifest constants */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef	SR_MANIFEST_H
#define	SR_MANIFEST_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_conf.h"

#include <sys/types.h>

 /* current ISODE distribution: major*10+minor */
#define	ISODE	80

#ifndef	SR_MOSY_CFG_H
#include "mosy_cfg.h"		/* system-specific configuration */
#endif

#ifdef PC
typedef char   *caddr_t;
#include <stdlib.h>
#endif				/* PC */


/* target-dependent defines:

	SYS5NLY -	target has SYS5 types only, no BSD types

	BSDSIGS	-	target supports BSD signals

 */

#ifdef	BSD42
#undef	SYS5NLY
#define	BSDSIGS
#endif


#ifdef	ROS
#undef	SYS5NLY
#define	BSDSIGS

#ifndef	BSD42
#define	BSD42
#endif

#endif


#ifdef	SYS5
#define	SYS5NLY
#undef	BSDSIGS

#ifdef	masscomp
#undef	SYS5NLY
#endif

#ifdef	OSX
#undef	SYS5NLY
#endif

#ifdef	XOS_2
#undef	SYS5NLY
#define	SIGEMT	SIGSEGV
#endif

#if (defined(WIN) || defined(WINTLI))
#undef	SYS5NLY
#endif	/* (defined(WIN) || defined(WINTLI)) */

#ifdef	sun
#undef	SYS5NLY
#endif

#ifdef	sgi
#undef	SYS5NLY
#endif

#ifdef SVR4
#undef  SYS5NLY
#endif



#ifdef AUX
#undef	SYS5NLY
#define BSDSIGS
#endif

#endif

#ifdef  SVR4

#ifndef SVR3
#define SVR3
#endif

#endif



/*    TYPES */

#ifndef	NOTOK
#define	NOTOK		(-1)
#define	OK		0
#define	DONE		1
#define CONNECTING_1	OK
#define CONNECTING_2	2
#endif

#ifdef SVR4

#ifdef NULLVP
#undef NULLVP
#endif

#endif

#ifndef	NULLCP
typedef char   *CP;
#define	NULLCP		((char *) 0)
#define	NULLVP		((char **) 0)
#endif

#ifndef INTDEF
#define	INTDEF	long
#endif

typedef INTDEF  integer;
#define NULLINT		((integer) 0)
#define NULLINTP	((integer *) 0)

#ifndef	makedev
#if (defined(WIN) || defined(WINTLI))
#include "sys/inet.h"

#ifndef	NFDBITS
typedef struct fd_set {
    int             fds_bits[1];
}               fd_set;

#endif

#endif	/* (defined(WIN) || defined(WINTLI)) */

#include <sys/select.h>

#ifdef	SYS5NLY
#ifndef sinix
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
#endif	/* sinix */
#endif

#endif


#ifndef FD_SET
#define	FD_SETSIZE	    (sizeof (fd_set) * 8)

#define FD_SET(f,s)	    ((s)->fds_bits[0] |= (1 << (f)))
#define FD_CLR(f,s)	    ((s)->fds_bits[0] &= ~(1 << (f)))
#define FD_ISSET(f,s)	    ((s)->fds_bits[0] & (1 << (f)))
#define FD_ZERO(s)	    ((s)->fds_bits[0] = 0)
#endif

#define	NULLFD		((fd_set *) 0)


#undef	IP
typedef int    *IP;
#define	NULLIP		((IP) 0)



#ifndef	SFD

#if (((! ( defined(SVR3) ) && ! ( defined(SUNOS4) )) && ! ( defined(BSD44) )) && ! ( defined(ultrix) ))
#define	SFD	int
#define	SFP	IFP
#else	/* (((! ( defined(SVR3) ) && ! ( defined(SUNOS4) )) && ! ( defined(BSD44) )) && ! ( defined(ultrix) )) */
#define	SFD	void
#define	SFP	VFP
#endif	/* (((! ( defined(SVR3) ) && ! ( defined(SUNOS4) )) && ! ( defined(BSD44) )) && ! ( defined(ultrix) )) */

#endif


struct udvec {			/* looks like a BSD iovec... */
    caddr_t         uv_base;
    int             uv_len;

    int             uv_inline;
};


struct qbuf {
    struct qbuf    *qb_forw;	/* doubly-linked list */
    struct qbuf    *qb_back;	/* .. */

    int             qb_len;	/* length of data */
    char           *qb_data;	/* current pointer into data */
    char            qb_base[1];	/* extensible... */
};

#define	QBFREE(qb) \
{ \
    register struct qbuf *QB, \
			 *QP; \
 \
    for (QB = (qb) -> qb_forw; QB != (qb); QB = QP) { \
	QP = QB -> qb_forw; \
	remque (QB); \
	free ((char *) QB); \
    } \
}


#ifndef	min
#define min(a, b)	((a) < (b) ? (a) : (b))
#endif	/* min */
#ifndef max
#define max(a, b)	((a) > (b) ? (a) : (b))
#endif	/* max */


#ifdef SYS5

#if (! ( defined(WINTLI) ) && ! ( defined(WIN) ))

#ifndef	sun
#endif

#else	/* (! ( defined(WINTLI) ) && ! ( defined(WIN) )) */
#define	getdtablesize()	(_NFILE - 1)
#endif	/* (! ( defined(WINTLI) ) && ! ( defined(WIN) )) */

#endif

#if defined(RT)
#define	ntohs(x)	(x)
#define	htons(x)	(x)
#define	ntohl(x)	(x)
#define	htonl(x)	(x)
#endif	/* defined(RT) */

/*    POSIX */

#ifndef	SETSID

#if ((defined(BSD44) || defined(SVR4)) || defined(_AIX))
#define	SETSID
#endif	/* ((defined(BSD44) || defined(SVR4)) || defined(_AIX)) */

#endif

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_MANIFEST_H */
