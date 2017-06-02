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
 *  Contains functions for translating between OID descriptors and human-
 *  friendly instances to the dotted-decimal OID names and instances.
 */

#include "sr_conf.h"

#include <stdio.h>


#include <ctype.h>

#include <stdlib.h>

#include <malloc.h>


#include <string.h>


#ifdef WANT_SSCANF_PROTO
extern int sscanf();
#endif	/* WANT_SSCANF_PROTO */

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"
#include "lookup.h"
#include "oid_lib.h"
#include "diag.h"
#include "frmt_lib.h"
#include "oidtranh.h"
SR_FILENAME



/*
 *  MakeOIDFromHFDot()
 *
 *  Function:  Converts a text string into an OID.  The MIB table code does
 *             not have information about tables and instances, so this fun-
 *             tion cannot convert text strings with human-friendly OID
 *             instances.
 */
OID            *
MakeOIDFromHFDot(const char *text_str)
{
    return MakeOIDFromDot(text_str);
}


/*
 *  MakeOIDFragFromHFDot()
 *
 *  Function:  Converts a text string into an OID.  The MIB table code does
 *             not have information about tables and instances, so this fun-
 *             tion cannot convert text strings with human-friendly OID
 *             instances.
 */
OID  *
MakeOIDFragFromHFDot(const char *text_str)
{
    return MakeOIDFragFromDot(text_str);
}


/*
 *  MakeHFDotFromOID()
 *
 *  Function:  Converts an OID into a text string.  This function does not
 *             have information about tables and instances, so it cannot
 *             render the OID with human-friendly instances.
 */
int  
MakeHFDotFromOID(const OID  *oid,
                 char       *buffer,
                 int        bufsize,
                 int        behavior)
{
    return MakeDotFromOID(oid, buffer);
}


/*
 *  MakeHFIndexFromOID()
 *
 *  Function:  Converts an OID into a text string of index values.  This 
 *             function does not have information about tables and instances,
 *             so it cannot render the OID with human-friendly instances.
 */
int  
MakeHFIndexFromOID(const OID  *oid,
                   const OID  *prev_oid,                 
                   char       *buffer,
                   int        bufsize,
                   int        behavior)
{
    return MakeDotFromOID(oid, buffer);
}

