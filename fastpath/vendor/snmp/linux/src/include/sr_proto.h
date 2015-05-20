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

#ifndef SR_SR_PROTO_H
#define SR_SR_PROTO_H

#ifdef  __cplusplus
extern "C" {
#endif

/* this platform doesn't support "far" pointers */
#define	FAR

#if defined(_MSC_VER)
/* allow PC object files to put data in far data segments */
#define FAR     __far
#endif	/* defined(_MSC_VER) */




#ifdef __STDC__
#ifndef SR_ANSI_PROTOTYPES 
#define SR_ANSI_PROTOTYPES
#endif /*  SR_ANSI_PROTOTYPES */
#endif /* __STDC__ */




#ifndef SR_ANSI_PROTOTYPES 
#define  SR_PROTOTYPE(X) ()
#else /*  SR_ANSI_PROTOTYPES */
#define  SR_PROTOTYPE(X) X
#endif /*  SR_ANSI_PROTOTYPES */

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* ! SR_SR_PROTO_H_ */
