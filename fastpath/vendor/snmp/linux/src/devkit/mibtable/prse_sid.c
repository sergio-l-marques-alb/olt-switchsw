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

#include "sr_conf.h"

#include <stdio.h>


#include <ctype.h>

#include <stdlib.h>

#include <malloc.h>


#include <string.h>


#ifdef WANT_SSCANF_PROTO
extern int sscanf();
#endif	/* WANT_SSCANF_PROTO */

#include "sr_snmp.h"
#include "oid_lib.h"   /* for [Dec|Hex|Oct]Char2L  functions  */
#include "diag.h"
SR_FILENAME


#ifdef WANT_2L_FUNCS
#include "sr_type.h"
#include "sr_proto.h"
#include "sr_2l.h"

/*
 *   The following routines provide replacements for the sscanf
 *   function in the packaged agent system WARNING: they are
 *   non-portable to the extent that the target may not be using the
 *   ASCII collating sequence and must be re-written in that
 *   (unlikely) case.
 */

/*
 * DecChar2L:
 *
 * The former name (pre-snmp12.1.0.0 release) was decchar2l().
 */
int
DecChar2L(str, value)
    const char *str;
    SR_UINT32  *value;
{
    if ((str[0] >= '0') && (str[0] <= '9')) {
	*value = str[0] - '0';
	return (1);
    }
    /* was not a digit ... signal a problem */
    *value = ((SR_INT32)0);
    return (0);
}

/*
 * HexChar2L:
 *
 * The former name (pre-snmp12.1.0.0 release) was hexchar2l().
 */
int
HexChar2L(str, value)
    const char *str;
    SR_UINT32  *value;
{
    if (((str[0] >= '0') && (str[0] <= '9')) ||
	((str[0] >= 'a') && (str[0] <= 'f')) ||
	((str[0] >= 'A') && (str[0] <= 'F'))) {

	*value = (str[0] <= '9') ? (str[0] - '0') :
	    ((str[0] <= 'F') ? (str[0] - 'A' + 10) : (str[0] - 'a' + 10));
	return (1);
    }
    /* was not a digit ... signal a problem */
    *value = ((SR_INT32)0);
    return (0);
}

/*
 * OctChar2L:
 *
 * The former name (pre-snmp12.1.0.0 release) was octchar2l().
 */
int
OctChar2L(str, value)
    const char *str;
    SR_UINT32  *value;
{
    if ((str[0] >= '0') && (str[0] <= '7')) {
	*value = str[0] - '0';
	return (1);
    }
    /* was not a digit ... signal a problem */
    *value = ((SR_INT32)0);
    return (0);
}

/*
 * Str2L:
 *
 * The former name (pre-snmp12.1.0.0 release) was str2l().
 */
int
Str2L(str, base, value)
    const char *str;
    int        base;
    SR_UINT32  *value;
{
    int             i, j;

    SR_UINT32   result;

    result = ((SR_INT32)0);

    if (base == 8)
	for (i = 0; str[i] >= '0' && str[i] <= '7'; i++) {
	    result = result * base + (str[i] - '0');
	}
    else if (base == 10)
	for (i = 0; str[i] >= '0' && str[i] <= '9'; i++) {
	    result = result * base + (str[i] - '0');
	}
    else if (base == 16)
	for (i = 0; (((str[i] >= '0') && (str[i] <= '9')) ||
		     ((str[i] >= 'a') && (str[i] <= 'f')) ||
		     ((str[i] >= 'A') && (str[i] <= 'F'))); i++) {

	    j = (str[i] <= '9') ? (str[i] - '0') :
		((str[i] <= 'F') ? (str[i] - 'A' + 10) : (str[i] - 'a' + 10));
	    result = result * base + j;
	}
    *value = result;
    return (1);
}

#endif	/* WANT_2L_FUNCS */

/*
 * ParseSubIdHex:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_sub_id_hex().
 */
int
ParseSubIdHex(_temp_ptr, value)
    const unsigned char **_temp_ptr;
    SR_UINT32  *value;
{
    int             i;
#ifndef WANT_2L_FUNCS
    unsigned int    scan_value;
#endif	/* WANT_2L_FUNCS */
    SR_UINT32       temp_value;
    const unsigned char  *temp_ptr = *_temp_ptr;

    temp_ptr++;         /* skip the '0' */
    temp_ptr++;         /* skip the 'x' or 'X' */

    *value = ((SR_INT32)0);

    for (i = 0; ((i < 8) && (*temp_ptr != '.') && (*temp_ptr != '\0')); i++) {
        if (!isxdigit((int) *temp_ptr)) {
            DPRINTF((APWARN, "ParseSubIdHex, bad digit: %s\n", temp_ptr));
            *_temp_ptr = temp_ptr;
            return (-1);
        }

#ifdef WANT_2L_FUNCS
        if (HexChar2L((char *)temp_ptr, &temp_value) != 1) {
#else
        if (sscanf((char *) temp_ptr, "%1x", &scan_value) != 1) {
#endif	/* WANT_2L_FUNCS */

            DPRINTF((APERROR, "ParseSubIdHex, serious error.\n"));
            *_temp_ptr = temp_ptr;
            return (-1);
        }
#ifndef WANT_2L_FUNCS
        temp_value = (SR_UINT32)scan_value;
#endif	/* WANT_2L_FUNCS */
        /* Have to mask because MSC does not handle longs well occasionally */
        *value = (*value << 4) + (0x0f & (SR_UINT32) temp_value);
        temp_ptr++;
    }                           /* end of for loop */

    *_temp_ptr = temp_ptr;
    return (0);
}

/*
 * ParseSubIdOctal:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_sub_id_octal().
 */
int
ParseSubIdOctal(_temp_ptr, value)
    const unsigned char **_temp_ptr;
    SR_UINT32      *value;
{
    short           i;
#ifndef WANT_2L_FUNCS
    int         scan_value;
#endif /* WANT_2L_FUNCS */
    SR_UINT32       temp_value;
    const unsigned char  *temp_ptr = *_temp_ptr;

    *value = ((SR_INT32)0);

    for (i = 0; ((i < 11) && (*temp_ptr != '.') && (*temp_ptr != '\0')); i++) {
        if (*temp_ptr < '0' || *temp_ptr > '7') {
            DPRINTF((APWARN, "ParseSubIdOctal, bad digit: %s\n", temp_ptr));
            *_temp_ptr = temp_ptr;
            return (-1);
        }

#ifdef WANT_2L_FUNCS
        if (OctChar2L((char *)temp_ptr, &temp_value) != 1) {
#else
        if (sscanf((char *) temp_ptr, "%1o", &scan_value) != 1) {
#endif	/* WANT_2L_FUNCS */

            DPRINTF((APERROR, "ParseSubIdOctal, serious error.\n"));
            *_temp_ptr = temp_ptr;
            return (-1);
        }
#ifndef WANT_2L_FUNCS
        temp_value = (SR_UINT32)scan_value;
#endif	/* WANT_2L_FUNCS */
        /* Have to mask because MSC does not handle longs well occasionally */
        *value = (*value << 3) + (0x07 & (SR_UINT32) temp_value);
        temp_ptr++;
    }                           /* end of for loop */

    *_temp_ptr = temp_ptr;
    return (0);
}

/*
 * ParseSubIdDecimal:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_sub_id_decimal().
 */
int
ParseSubIdDecimal(_temp_ptr, value)
    const unsigned char **_temp_ptr;
    SR_UINT32      *value;
{
    short           i;
#ifndef WANT_2L_FUNCS
    int         scan_value;
#endif /* WANT_2L_FUNCS */
    SR_UINT32   temp_value;
    const unsigned char  *temp_ptr = *_temp_ptr;

    *value = ((SR_INT32)0);
    for (i = 0; (i < 11) && (*temp_ptr != '.') && (*temp_ptr != '\0'); i++) {
        if (*temp_ptr < '0' || *temp_ptr > '9') {
            DPRINTF((APWARN, "ParseSubIdDecimal, bad digit: %s\n", temp_ptr));
            *_temp_ptr = temp_ptr;
            return (-1);
        }

#ifdef WANT_2L_FUNCS
        if (DecChar2L((char *)temp_ptr, &temp_value) != 1) {
#else
        if (sscanf((char *) temp_ptr, "%1d", &scan_value) != 1) {
#endif	/* WANT_2L_FUNCS */

            DPRINTF((APERROR, "ParseSubIdDecimal, serious error.\n"));
            *_temp_ptr = temp_ptr;
            return (-1);
        }
#ifndef WANT_2L_FUNCS
        temp_value = (SR_UINT32)scan_value;
#endif	/* WANT_2L_FUNCS */
        /* Have to mask because MSC does not handle longs well occasionally */
        *value = (*value * (SR_UINT32) 10) + (0x0f & (SR_UINT32) temp_value);
        temp_ptr++;
    }                           /* end of for loop */

    *_temp_ptr = temp_ptr;
    return (0);
}
