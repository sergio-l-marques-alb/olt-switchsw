/*
 *
 * Copyright (C) 1999-2006 by SNMP Research, Incorporated.
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

#ifndef SR_SR_VRSN_H
#define SR_SR_VRSN_H

#ifdef  __cplusplus
extern "C" {
#endif

#define SR_MAJOR_VERSION      16
#define SR_MINOR_VERSION      2
#define SR_BRANCH_VERSION     0
#define SR_PATCH_VERSION      4
#define SR_MAJOR_VERSION_STR  "16"
#define SR_MINOR_VERSION_STR  "2"
#define SR_BRANCH_VERSION_STR "0"
#define SR_PATCH_VERSION_STR  "4"
#define SR_VERSION            "16.2.0.4"

/*BAB*/
#define GOT_DR_WEB "" 

#define GOT_ELITE "/Lite"

#define GOT_PROXY ""

#ifdef SR_EPIC
#define GOT_EPIC "with EPIC "
#else
#define GOT_EPIC ""
#endif /*SR_EPIC*/ 

#define GOT_EVAL ""

#define STARTUP_BANNER_MIBOBJ \
     "SNMP Research " GOT_DR_WEB "EMANATE" GOT_ELITE GOT_PROXY " Agent " GOT_EPIC GOT_EVAL "Version " SR_VERSION "\n"

/*BAB*/

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_SR_VRSN_H */
