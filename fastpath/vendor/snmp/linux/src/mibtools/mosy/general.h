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

/* general.h - general compatibility */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef	SR_GENERAL_H
#define	SR_GENERAL_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_conf.h"
#include "sr_time.h"

#include "mosy_cfg.h"		/* system-specific configuration */

/* target-dependent defines:

	BSDFORK -	target has BSD vfork
	BSDLIBC -	target has real BSD libc
	BSDSTRS -	target has BSD strings
	SWABLIB -	target has byteorder(3n) routines
 */

#ifdef	SOCKETS
#define	SWABLIB
#endif

#ifdef	WINTLI
#define	SWABLIB
#endif

#ifdef	EXOS
#define	SWABLIB
#endif


#ifdef	BSD42
#define	BSDFORK
#define	BSDLIBC
#define	BSDSTRS
#endif

#ifdef  SVR4_UCB

#ifndef SVR4
#define SVR4
#endif

#endif

#ifdef  SVR4

#ifndef SVR3
#define SVR3
#endif

#endif

#ifdef	ROS
#undef	BSDFORK
#undef	BSDLIBC
#define	BSDSTRS

#ifndef	BSD42
#define	BSD42
#endif

#undef	SWABLIB
#endif

#if defined(sinix)
#ifndef SYS5
#define SYS5
#endif	/* SYS5 */
#endif	/* defined(sinix) */

#ifdef	SYS5
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif

#ifdef	sgi
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif


#ifdef	pyr
#undef	SWABLIB
#endif

#ifdef	XOS
#undef	SWABLIB
#endif

#ifdef	XOS_2
#undef	SWABLIB
#endif

#ifdef  apollo
#undef  SWABLIB
#endif

#ifdef	AUX
#undef	BSDFORK
#define BSDSTRS
#undef SWABLIB
#define BSDLIBC
#endif


#if (defined(_AIX) && defined(SYS5))
#define BSDSTRS
#endif	/* (defined(_AIX) && defined(SYS5)) */

#ifndef	BSDFORK
#define	vfork	fork
#endif

/*    STRINGS */

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>



#ifndef	BSDSTRS
#define	index	strchr
#define	rindex	strrchr
#endif	/* BSDSTRS */


/*    SPRINTB */

char *sprintb
    SR_PROTOTYPE((register int v,
		  register char *bits));


/*    MISC */

#ifdef	lint
#define	insque(e,p)	INSQUE ((char *) (e), (char *) (p))
#define	remque(e)	REMQUE ((char *) (e))
#endif

/*  time */

#ifndef __STDC__
extern time_t   time();
#endif




#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif				/* SR_GENERAL_H */
