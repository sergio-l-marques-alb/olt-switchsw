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

#include <string.h>


#include <ctype.h>

/* LVL7 
#include <malloc.h>
*/
#include <stdlib.h>


#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"


#include "mib_tabl.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME

extern int      global_InitNewOIDRoutines;


#ifdef SR_MIB_TABLE_HASHING
extern struct MIB_OID *name_oid_hash_array[NAME_OID_HASH_SIZE];
#endif /* SR_MIB_TABLE_HASHING */

/*
 * Routines to create an object identifer in an OID from
 * dot notation input.
 */

/*
 * MakeOIDFragFromDot:
 *
 *   This routine is called to create a library form object identifier
 *   from an character string.  The string input is usually in the
 *   format "integer.integer.integer...."  (i.e.,
 *   "1.3.6.1.2.1.1.1.0"), but can be shortened by using the names as
 *   they appear in RFC 1157 (i.e., "sysDescr.0").  It returns a
 *   pointer to a malloc'ed data structure containing the internal
 *   library representation for an object identifier.  This identifier
 *   can then be used in calls to make_varbind() and MakePdu() (in the
 *   case of traps).  This malloc'ed data structure will be free'ed by
 *   calls to FreePdu() after the pointer has been used.
 *
 */
OID            *
MakeOIDFragFromDot_track(text_str, file, line)
    const char     *text_str;
    char * file;
    unsigned int line;
{
    OID            *oid_ptr = NULL;
    const char     *temp_ptr, *dot_ptr, *english_dot;
    int             i, cc;
    int             dot_count;
    int             prefixlen;
#ifdef SR_MIB_TABLE_HASHING
    struct MIB_OID *hash_ptr;
    int             hash;
#endif                          /* SR_MIB_TABLE_HASHING */

    if (global_InitNewOIDRoutines == 0) {
        InitNewOIDRoutines();
    }

    if(text_str == NULL) {
	goto fail;
    }
    dot_ptr = text_str;
    english_dot = NULL;

    /* see if there is an alpha descriptor at begining */
    if (isalpha((unsigned char)*text_str)) {
        dot_ptr = strchr(text_str, '.');
	prefixlen = strcspn(text_str, ".");
        if (dot_ptr != NULL) {
            dot_ptr++;
        }

#ifndef SR_MIB_TABLE_HASHING
        english_dot = NULL;
        for (i = 0; i < num_mot_entries; i++) {
            if (!strncmp(mib_oid_table[i].name, text_str,
			MAX(prefixlen,strlen(mib_oid_table[i].name)))) {
                english_dot = mib_oid_table[i].number;
                break;
            }
        }
        if (english_dot == NULL) {
            DPRINTF((APPACKET, "MakeOIDFragFromDot, table lookup failed: %s\n",
                     text_str));
	    goto fail;
        }
#else                           /* SR_MIB_TABLE_HASHING */
        hash = 0;
        temp_ptr = text_str;
	i = 0;
        while (*temp_ptr != '\0' && i++ < prefixlen) {
            hash = (hash + (int)*temp_ptr) % (int) NAME_OID_HASH_SIZE;
            temp_ptr++;
        }

        for (hash_ptr = name_oid_hash_array[hash];
             hash_ptr != NULL; hash_ptr = hash_ptr->hash_next) {
            if (!strncmp(hash_ptr->name, text_str,
			MAX((unsigned)prefixlen,strlen(hash_ptr->name)))) {
                break;
            }
        }
        if (hash_ptr == NULL) {
            DPRINTF((APPACKET,
                     "MakeOIDFragFromDot, hash table lookup failed: %s\n",
                     text_str));
	    goto fail;
        }

        english_dot = hash_ptr->number;

#endif                          /* SR_MIB_TABLE_HASHING */
    }

    /* count the dots.  num +1 is the number of SIDs */
    dot_count = 0;
    if (english_dot && dot_ptr) {
        dot_count = 1;
    }
    if (english_dot) {
        for (i=0; english_dot[i] != '\0'; i++) {
            if (english_dot[i] == '.') {
                dot_count++;
            }
        }
    }
    if (dot_ptr) {
        for (i=0; dot_ptr[i] != '\0'; i++) {
            if (dot_ptr[i] == '.') {
                dot_count++;
            }
        }
    }
    if((oid_ptr = MakeOID_track(NULL, dot_count + 1, file, line)) == NULL) {
        DPRINTF((APPACKET, "MakeOIDFragFromDot, oid_ptr MakeOID\n"));
	goto fail;
    }

    /* now we convert number.number.... strings */
    if (english_dot) {
        temp_ptr = english_dot;
        cc = 0;
    } else {
        temp_ptr = dot_ptr;
        cc = 1;
    }
    for (i = 0; i < dot_count + 1; i++) {

        if (*temp_ptr == '0') {
            if ((temp_ptr[1] == 'x') || (temp_ptr[1] == 'X')) {
                if ((unsigned)(ParseSubIdHex((const unsigned char **) &temp_ptr, 
				   &oid_ptr->oid_ptr[i])) == ~(unsigned) 0) {
                    DPRINTF((APPACKET, "MakeOIDFragFromDot, hex:\n"));
		    goto fail;
                }
            }
            else {
                if ((unsigned)(ParseSubIdOctal((const unsigned char **) &temp_ptr,
				     &oid_ptr->oid_ptr[i])) == ~(unsigned) 0) {
                    DPRINTF((APPACKET, "MakeOIDFragFromDot, octal:\n"));
		    goto fail;
                }
            }
        }
        else {
            if (*temp_ptr >= '0' && *temp_ptr <= '9') {
                if ((unsigned)(ParseSubIdDecimal((const unsigned char **) &temp_ptr,
				       &oid_ptr->oid_ptr[i])) == ~(unsigned) 0) {
                    DPRINTF((APPACKET, "MakeOIDFragFromDot, decimal:\n"));
		    goto fail;
                }
            }
            else {
                DPRINTF((APPACKET, "MakeOIDFragFromDot, bad character: %d, %s\n",
                         *temp_ptr, temp_ptr));
		goto fail;
            }
        }
        if (*temp_ptr == '.')
            temp_ptr++;         /* to skip over dot */
        else if (*temp_ptr != '\0') {
            DPRINTF((APPACKET, "MakeOIDFragFromDot, expected dot: %d %s\n",
                     *temp_ptr, temp_ptr));
	    goto fail;
        } else {
            if ((cc == 0) && dot_ptr) {
                temp_ptr = dot_ptr;
            }
        }
    }                           /* end of for loop */

    return (oid_ptr);
  fail:
    if(oid_ptr != NULL) {
	FreeOID(oid_ptr);
	NULLIT(oid_ptr);
    }
    return NULL;
}     /* MakeOIDFragFromDot() */


/*
 * MakeOIDFromDot:
 *
 *   This routine is called to create a library form object identifier
 *   from an character string.  The string input is usually in the
 *   format "integer.integer.integer...."  (i.e.,
 *   "1.3.6.1.2.1.1.1.0"), but can be shortened by using the names as
 *   they appear in RFC 1157 (i.e., "sysDescr.0").  It returns a
 *   pointer to a malloc'ed data structure containing the internal
 *   library representation for an object identifier.  This identifier
 *   can then be used in calls to make_varbind() and MakePdu() (in the
 *   case of traps).  This malloc'ed data structure will be free'ed by
 *   calls to FreePdu() after the pointer has been used.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_obj_id_from_dot(). 
 */

OID            *
MakeOIDFromDot_track(s, file, line)
    const char *s;
    char * file;
    unsigned int line;
{
    OID *oid_ptr;
    oid_ptr = MakeOIDFragFromDot_track(s, file, line);
    if (oid_ptr == NULL) {
        DPRINTF((APPACKET, "MakeOIDFromDot: MakeOIDFragFromDot(%s) failed\n", 
		s));
        return (NULL);
    }
    if (oid_ptr->oid_ptr[0] >= 4) {     /* we have a bogus OID */
        DPRINTF((APPACKET, "MakeOIDFromDot, illegal OID value %lu\n",
                 (unsigned long)oid_ptr->oid_ptr[0]));
        FreeOID(oid_ptr);
        NULLIT(oid_ptr);
        return (NULL);
    }
    return(oid_ptr);
}     /* MakeOIDFromDot() */


/*
 * MakeDotFromOID:
 * 
 *   This routine is called to convert an Object Identifier library
 *   construct into a dot notation character string, usually for us in
 *   a human interface.  The dot-notation output is the usual form
 *   (1.2.3.4.1.2.1.1) with the a MIB name subsituted for the most
 *   possible sub-identifiers starting from the left
 *   (1.3.6.1.2.1.1.1.0 becomes sysDescr.0).  The MIB names included
 *   in the library are found in the mib_oid_table in snmp-mib.h.
 *   This include file is used in the compilation of oid_lib.c, part
 *   of the libsnmpuser.a library and need not be included in
 *   applications.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_dot_from_obj_id().
 *
 * There are two versions of this function, one which does a linear search,
 * a one which does a (pseudo-)binary search.  The binary search is about
 * an order of magnitude faster, but is not as thouroughly tested.
 */

#ifndef SR_MIB_TABLE_BINARY

short
MakeDotFromOID(oid_ptr, buffer)
    const OID      *oid_ptr;
    char            buffer[];
{
    /* Handle NULL buffer argument */
    if (buffer == NULL) {
	DPRINTF((APPACKET, "MakeDotFromOID, bad (NULL) string buffer\n"));
        return(-1);
    }

    buffer[0] = '\0';

    if ((oid_ptr == NULL) || (oid_ptr->length == 0) ||
	(oid_ptr->oid_ptr == NULL)) {
	DPRINTF((APPACKET, "MakeDotFromOID, bad (NULL) OID\n"));
	return (-1);
    }

    if (global_InitNewOIDRoutines == 0) {
	InitNewOIDRoutines();
    }

    if (GetStringFromSubIds(buffer, oid_ptr->oid_ptr,
			     (short) oid_ptr->length) == -1) {
	DPRINTF((APPACKET, "MakeDotFromOID, GetStringFromSubIds\n"));
	return (-1);
    }

    return (0);
}

/*
 * GetStringFromSubIds:
 *
 * The former name (pre-snmp12.1.0.0 release) was get_str_from_sub_ids().
 */
short
GetStringFromSubIds(string_buffer, sid, sid_counter)
    char            string_buffer[];
    SR_UINT32      *sid;
    int             sid_counter;
{
    int             i;
    char            temp_buffer[20];
    int             dot_count;
    int             best_i, best_len, test_len;

    /* build test string ... put dots between each entry but not after last */
    for (i = 0; i < sid_counter; i++) {
	(void) sprintf(temp_buffer, "%lu", (unsigned long)sid[i]);
	if ((i + 1) != sid_counter) {
	    (void) strcat(string_buffer, temp_buffer);
	    (void) strcat(string_buffer, ".");
	}
	else
	    (void) strcat(string_buffer, temp_buffer);
    }

    /*
     * Now, let us try to convert the dotted decimal header of the string in
     * temp_buffer into a more reasonable ASCII string.
     */
/* Can do a binary search here, since snmp-mib.h is sorted by OID */

    best_i = -1;
    best_len = -1;
    /* OK, now find the best fit to this string */
    for (i = 0; mib_oid_table[i].name != NULL; i++) {
        test_len = 0;
        while ((mib_oid_table[i].number[test_len] == string_buffer[test_len]) &&
               (mib_oid_table[i].number[test_len] != '\0') &&
               (string_buffer[test_len] != '\0')) {
            test_len++;
        }

	/* 
         * test_len is a count of the number successfully matched
         * i.e., mib_oid_table[i].number[test_len] is the first
         * mismatch; i.e., mib_oid_table[i].number[test_len-1] is the
         * last good match want to back up to the first character
         * before the last . or null 
         */

	/**
	 * this is what the equation works out to be just looking at it but i
	 * simply could not sleep with such a thing so i applied demorgans
	 * theorem and got the resulting code below which should be
	 * equivalent but run faster 
         * while ((test_len > 0) 
         *        && 
         *        (!((mib_oid_table[i].number[test_len] == '.') ||
	 *           (mib_oid_table[i].number[test_len] == '\0')) 
         *        ||
         *        !((string_buffer[test_len] == '.') || 
         *          (string_buffer[test_len] == '\0')) 
         *        )
         *       ) {
	 * 
	 **/

	while ((test_len > 0)
	       &&
	       (
		((mib_oid_table[i].number[test_len] != '.') &&
		 (mib_oid_table[i].number[test_len] != '\0'))
		||
		((string_buffer[test_len] != '.') &&
		 (string_buffer[test_len] != '\0'))
		)
	    ) {
	    test_len--;
	}			/* while */
	if ((test_len > best_len) && ((unsigned) test_len == strlen(mib_oid_table[i].number))) {
	    best_i = i;
	    best_len = test_len;
	}			/* if test_len > */
    }				/* for i */

    /*
     * Did we get something?  If so, then replace dot-digit string with the
     * name.
     */

    if (best_i > -1) {
	(void) strcpy(string_buffer, mib_oid_table[best_i].name);

	/* count the dots.  num +1 is the number of SIDs */
	dot_count = 0;
	for (i = 0; mib_oid_table[best_i].number[i] != '\0'; i++) {
	    if (mib_oid_table[best_i].number[i] == '.')
		dot_count++;
	}

	/* now flesh out from where the name leaves off */

	for (i = dot_count + 1; i < sid_counter; i++) {
	    (void) sprintf(temp_buffer, ".%lu", (unsigned long)sid[i]);
	    (void) strcat(string_buffer, temp_buffer);
	}
    }
    /* else, just return the dot string we already filled in */
    return (1);
}

#else /* SR_MIB_TABLE_BINARY */

short
MakeDotFromOID(oid, buffer)
    const OID      *oid;
    char            buffer[];
{
    unsigned long sid;
    int lo, hi, mid, pos, len;
    char *str, *nstr;
    char sid_str[32];

    buffer[0] = '\0';
    if ((oid == NULL) || (oid->length == 0) ||
        (oid->oid_ptr == NULL)) {
        DPRINTF((APPACKET, "MakeDotFromOID, bad (NULL) OID\n"));
        return (-1);
    }

    if (global_InitNewOIDRoutines == 0) {
        InitNewOIDRoutines();
    }

    lo = 0;
    hi = num_mot_entries - 1;
    while (lo < hi) {
        mid = (lo + hi) / 2;
        if (mid == lo) {
            mid++;
        }
        str = mib_oid_table[mid].number;
        pos = 0;
        len = 0;
        while ((str[len] != '\0') && (pos < oid->length)) {
            sscanf(&str[len], "%lu", &sid);
            if (sid < oid->oid_ptr[pos]) {
                lo = mid + 1;
                break;
            } else
            if (sid > oid->oid_ptr[pos]) {
                hi = mid - 1;
                break;
            }
            pos++;
            nstr = strchr(&str[len], '.');
            if (nstr == NULL) {
                lo = mid;
                break;
            }
            if (pos >= oid->length) {
                hi = mid - 1;
                break;
            }
            len = (int)(nstr - str) + 1;
        }
    }

    strcpy(buffer, "");
    if (pos > 0) {
        strcpy(buffer, mib_oid_table[lo].name);
        pos = 1;
        nstr = strchr(mib_oid_table[lo].number, '.');
        while (nstr) {
            nstr++;
            nstr = strchr(nstr, '.');
            pos++;
        }
    }
    while (pos < oid->length) {
        sprintf(sid_str, ".%lu", (unsigned long)oid->oid_ptr[pos]);
        strcat(buffer, sid_str);
        pos++;
    }

    return (0);
}

#endif /* SR_MIB_TABLE_BINARY */

/*
 * GetType:
 *
 *   The name of this routine (pre-snmp12.1.0.0 release) was get_type().
 */
char           *
GetType(string, value)
    const char     *string;
    SR_INT32        value;
{
    static char     ret[32];
#if SIZEOF_INT == 4
    sprintf(ret, "%d", value);
#else	/*  SIZEOF_INT == 4 */
    sprintf(ret, "%ld", value);
#endif	/*  SIZEOF_INT == 4 */
    return (ret);
}     /* GetType() */

/*
 * GetBitsType:
 */
char *
GetBitsType(string, value)
    const char *string;
    const OctetString *value;
{
    return NULL;
}     /* GetBitsType() */


/*
 * GetBitsTypeWithOID:
 */
char *
GetBitsTypeWithOID(oid, value)
    const OID *oid;
    const OctetString *value;
{
    return NULL;
}     /* GetBitsTypeWithOID() */


/*
 * GetBaseOIDString
 */
char *
GetBaseOIDString(oid)
    const OID *oid;
{
    char *number, *p;
    SR_UINT32 i;

    number = (char *)malloc(oid->length * 11);
    if (number == NULL) {
        return NULL;
    }
    for (p = number, i = 0; i < (SR_UINT32)oid->length; p += strlen(p), i++) {
        sprintf(p, "%lu.", (unsigned long)oid->oid_ptr[i]);
    }
    p[0] = '\0';

    for (i = 0; mib_oid_table[i].name != NULL; i++) {
        if (!strcmp(mib_oid_table[i].number, number)) {
            free(number);
            return mib_oid_table[i].name;
        }
    }
    free(number);
    return NULL;
}
