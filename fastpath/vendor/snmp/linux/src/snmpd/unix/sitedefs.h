/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
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

#ifndef SR_SITEDEFS_H
#define SR_SITEDEFS_H

#ifdef  __cplusplus
extern "C" {
#endif

/* LVL7 */
#include "l7_sitedefs.h"
/* LVL7 end */

/*
 * Define site-specific definitions in this file.
 */

/*
 * Define 2 text strings to be displayed with the Agent starts.
 */

/*
 * Add more specifics to the banner
 */

#define STARTUP_BANNER_1 \
     "SNMP Research " GOT_DR_WEB "EMANATE" GOT_ELITE GOT_PROXY " Agent "  GOT_EVAL "Version " SR_VERSION "\n"

#define STARTUP_BANNER_2 \
    "Copyright 1989-2006 SNMP Research, Inc.\n"

/*
 * Interface layer definitions.
 */
#define PHYS_ADDR_SIZE 6
/* #define IF_TESTING_SUPPORTED */


/*
 * IP configuration.
 */

/* #define SUPPORT_IP_FORWARDING */
#define MIN_IP_TTL	2

/* #define SUPPORT_ROUTE_METRIC_1 */
/* #define SUPPORT_ROUTE_METRIC_2 */
/* #define SUPPORT_ROUTE_METRIC_3 */
/* #define SUPPORT_ROUTE_METRIC_4 */
/* #define SUPPORT_ROUTE_METRIC_5 */



#define BUFLEN 512

#define MAXCOMMLEN      64

#define MAXINTERFACES   20


/* for read-write tables in v1 MIBs in which new rows can be created */

#define atEntry_READ_CREATE
#define ipRouteEntry_READ_CREATE
#define ipNetToMediaEntry_READ_CREATE




#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_SITEDEFS_H */
