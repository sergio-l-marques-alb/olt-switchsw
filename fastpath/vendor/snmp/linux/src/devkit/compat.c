/*
 *
 * Copyright (C) 1996-2006 by SNMP Research, Incorporated.
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
 * This file includes SNMP Research, Inc.'s versions of "standard"
 * functions which are missing on some systems.
 */


#include "sr_conf.h"


#include <stdio.h>


#include <ctype.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include "sr_snmp.h"



#include "sr_type.h"
#include "sr_proto.h"


#include "compat.h"

#ifndef max
#endif /* max */

/*
 *  Use this function when you need a memcmp() function that does a
 *  comparison of unsigned chars.  
 *
 *  You may need this function if you are comparing memory areas that 
 *  can legitimately have values between 0-255 (inclusive) _and_ you 
 *  need to check for which of the operands is less than/greater than 
 *  the other. 
 *
 *  On some systems the standard memcmp() function uses char operands 
 *  (instead of unsigned char operands).  The results from comparing 
 *  unsigned char values can depend upon the values of the sign bits,
 *  and thus produce unexpected results.
 */
int
SR_umemcmp(str1, str2, length)
void  *str1;
void  *str2;
int    length;
{
    unsigned char  *s1 = (unsigned char *) str1;
    unsigned char  *s2 = (unsigned char *) str2;
    int    i = 0;

    while (*s1 == *s2 && i < length) {
        i++;
        if (i == length)
            break;

        s1++;
        s2++;
    }

    return (length == 0 ? 0 : *s1 - *s2);
}





#include "sr_time.h"











