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

/*
 *  sr_spcli.c: contains support routines for parsing security pack
 *  and distributed security pack command lines.   As some of this
 *  shows up in command line utilities for testing, modules were extracted
 *  from security pack and distributed security pack code and 
 *  relocated here.
 *
 *  IPV6:  Needs extensive work for parsing IPv6 address 
 */

#include "sr_conf.h"
#include "sr_snmp.h"

#include <sys/types.h>

#include <string.h>

#include <stdlib.h>

#include <ctype.h>

/* ----------------------------------------------------------------------
 * kw_match:  perform string comparisons, searching for complete or partial
 *            pattern matches in the string.  (Bracket '[' indicates 
 *            beginning of optional portion of pattern)
 *
 */             
int
kw_match(
    char *s, 
    char *pat)
{ 
   
    int optional = 0;   /* Are we scanning optional part of the pattern */
 
    while ( 1 ) {
        while ( *s && *pat != '[' && *pat == *s ) {
            s++;
            pat++;
        }
 
        if ( *pat == '[' ) {
            /*
             * The remainder of the pattern is optional
             */
            optional = 1;
            pat++;
        }
 
        if ( *s == '\0' ) {
            /*
             * Done with keyword string, decide if we match
             */
            return  ( *pat == '\0' || optional );
 
        } else if ( *pat == '\0' ) {
            /*
             * End of pattern, but not keyword string
             */
            return 0;
        } else if (!( optional && *pat == *s ) ) {
            /*
             * Not the end of either and still no match
             */
            return 0;
        }
    }
}
