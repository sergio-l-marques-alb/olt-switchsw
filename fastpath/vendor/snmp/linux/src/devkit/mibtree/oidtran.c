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
#include "mib_tree.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME


extern MIB_TREE *default_mib_tree;
extern char *unassigned_text;

OID            *
MakeOIDFragFromDot(text_str)
    const char     *text_str;
{
    OID            *oid = NULL;
    const char     *temp_ptr, *dot_ptr, *english_dot;
    int             i, cc;
    int             dot_count;
    size_t          prefixlen;
    int             hash;
    MIB_TREE       *mt;
    OID_TREE_ELE   *ote;

    if(text_str == NULL) {
	goto fail;
    }
    dot_ptr = text_str;
    english_dot = NULL;
    dot_count = 0;

    /* see if there is an alpha descriptor at beginning */
    if (isalpha((unsigned char)*text_str)) {
        dot_ptr = strchr(text_str, '.');
	prefixlen = strcspn(text_str, ".");
        if (dot_ptr != NULL) {
            dot_ptr++;
        }

        hash = CalculateMIBHash(text_str, prefixlen);

        for (mt = default_mib_tree; mt != NULL; mt = mt->next) {
            for (ote = mt->hash_table[hash]; ote != NULL; ote = ote->hash_next){
                if (!strncmp(ote->oid_name, text_str,
			MAX(prefixlen,strlen(ote->oid_name)))) {
                    goto got_ote;
                }
            }
        }
        DPRINTF((APTRACE, "MakeOIDFragFromDot, hash table lookup failed: %s\n",
                 text_str));
	goto fail;
  got_ote:
        english_dot = ote->oid_number_str;
        if (dot_ptr) {
            dot_count = 1;
        }
        for (i=0; english_dot[i] != '\0'; i++) {
            if (english_dot[i] == '.') {
                dot_count++;
            }
        }
    }

    /* count the dots.  num +1 is the number of SIDs */
    if (dot_ptr) {
        for (i=0; dot_ptr[i] != '\0'; i++) {
            if (dot_ptr[i] == '.') {
                dot_count++;
            }
        }
    }
    if((oid = MakeOID(NULL, dot_count+1)) == NULL) {
        DPRINTF((APPACKET, "MakeOIDFragFromDot, oid MakeOID failed\n"));
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
				   &oid->oid_ptr[i])) == ~(unsigned) 0) {
                    DPRINTF((APPACKET, "MakeOIDFragFromDot, hex:\n"));
		    goto fail;
                }
            }
            else {
                if ((unsigned)(ParseSubIdOctal((const unsigned char **) &temp_ptr,
				     &oid->oid_ptr[i])) == ~(unsigned) 0) {
                    DPRINTF((APPACKET, "MakeOIDFragFromDot, octal:\n"));
		    goto fail;
                }
            }
        }
        else {
            if (*temp_ptr >= '0' && *temp_ptr <= '9') {
                if ((unsigned)(ParseSubIdDecimal((const unsigned char **) &temp_ptr,
				       &oid->oid_ptr[i])) == ~(unsigned) 0) {
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
        if (*temp_ptr == '.') {
            while (*temp_ptr == '.') {
                temp_ptr++;         /* to skip over dot */
            }
        } else if (*temp_ptr != '\0') {
            DPRINTF((APPACKET, "MakeOIDFragFromDot, expected dot: %d %s\n",
                     *temp_ptr, temp_ptr));
	    goto fail;
        } else {
            if ((cc == 0) && dot_ptr) {
                temp_ptr = dot_ptr;
            }
        }
    }                           /* end of for loop */

    return (oid);
  fail:
    FreeOID(oid);
    return NULL;
}

OID            *
MakeOIDFromDot(s)
    const char *s;
{
    OID *oid_ptr;
    int i;

    oid_ptr = MakeOIDFragFromDot(s);
    if (oid_ptr == NULL) {
        /* if MakeOIDFragFromDot fails and s contains a hypen, change
         * the hyphen to underscore and try again */
        if (strchr(s, '-') != NULL) {
            char *dup = strdup(s);
            if (dup != NULL) {
                for (i = 0; dup[i]; i++) {
                    if (dup[i] == '-') dup[i] = '_';
                }
                oid_ptr = MakeOIDFragFromDot(dup);
                free (dup);
            }

        /* if MakeOIDFragFromDot fails and s contains an underscore, change
         * the underscore to hypen and try again */
        } else if (strchr(s, '_') != NULL) {
            char *dup = strdup(s);
            if (dup != NULL) {
                for (i = 0; dup[i]; i++) {
                    if (dup[i] == '_') dup[i] = '-';
                }
                oid_ptr = MakeOIDFragFromDot(dup);
                free (dup);
            }
        }
    }
    if (oid_ptr == NULL) {
        DPRINTF((APPACKET, "MakeOIDFromDot: MakeOIDFragFromDot(%s) failed\n",
                s));
        return (NULL);
    }
    if (oid_ptr->oid_ptr[0] >= 4) {     /* we have a bogus OID */
        DPRINTF((APPACKET, "MakeOIDFromDot, illegal OID value "UINT32_FORMAT"\n",
                 oid_ptr->oid_ptr[0]));
        FreeOID(oid_ptr);
        NULLIT(oid_ptr);
        return (NULL);
    }
    return(oid_ptr);
}

OID_TREE_ELE *
OIDTreeDive(oid_tree_ptr, sid, sid_counter_ptr)
    OID_TREE_ELE   *oid_tree_ptr;
    SR_UINT32      *sid;
    int            *sid_counter_ptr;
{
    OID_TREE_ELE   *ote;
    int i;
    int sid_counter = *sid_counter_ptr;

    /* Just to be safe, and to preserve old functionality */
    if (sid_counter < 0) {
        DPRINTF((APERROR, "OIDTreeDive: sid_counter too low\n"));
        return (NULL);
    }
    if (sid_counter == 0) {
        return (NULL);
    }

    /* First check if the oid starts at our root or any sibling root */
    for (ote = oid_tree_ptr; ote != NULL; ote = ote->next) {
        if (ote->sid_value == sid[0]) {
            break;
        }
    }
    if (ote == NULL) {
        return(NULL);
    }
    oid_tree_ptr = ote;

    /* At this point, we should NOT return NULL, because we've found at least
     * one OTE node that matches. */
    /* Walk down the tree.  If an SID names a child that doesn't exist, the
     * inner for loop will complete and ote will be NULL, and the while
     * loop will terminate.  If all SIDs name children that exist, the loop
     * will terminate, and oid_tree_ptr will point to the appropriate node.
     */
    i = 1;
    while ((oid_tree_ptr != NULL) && (i < sid_counter)) {
        for (ote=oid_tree_ptr->first_descendent; ote != NULL; ote = ote->next) {
            if (ote->sid_value == sid[i]) {
                break;
            }
        }
        if (ote == NULL) {
            break;
        }
        i++;
        oid_tree_ptr = ote;
    }

    while (oid_tree_ptr != NULL) {
        if (oid_tree_ptr->oid_name != unassigned_text) {
            break;
        }
        oid_tree_ptr = oid_tree_ptr->parent;
        i--;
    }

    *sid_counter_ptr = i;
    return (oid_tree_ptr);
}

short
MakeDotFromOID(oid, buffer)
    const OID      *oid;
    char            buffer[];
{
    FNAME("MakeDotFromOID")
    MIB_TREE *mt;
    OID_TREE_ELE *ote;
    int len;
    char *buf;

    /* Handle NULL buffer argument */
    if (buffer == NULL) {
	DPRINTF((APPACKET, "%s: bad (NULL) string buffer\n", Fname));
        return(-1);
    }

    buffer[0] = '\0';

    if ((oid == NULL) || (oid->length == 0) ||
	(oid->oid_ptr == NULL)) {
	DPRINTF((APPACKET, "%s: bad (NULL) OID\n", Fname));
	return (-1);
    }

    len = 0;
    mt = default_mib_tree;
    if (mt) {
        len = oid->length - mt->prefix->length + 1;
        ote = OIDTreeDive(mt->root, &(oid->oid_ptr[mt->prefix->length-1]), &len);
        if (ote != NULL) {
            sprintf(buffer, "%s.", ote->oid_name);
        } else {
            len = 0;
        }
    }

    buf = buffer + strlen(buffer);
    for ( ; len < oid->length ; len++) {
        sprintf(buf, UINT32_FORMAT".", oid->oid_ptr[len]);
        buf += strlen(buf);
    }
    buf--;
    buf[0] = '\0';

    return (0);
}

char           *
GetType(string, value)
    const char     *string;
    SR_INT32        value;
{
    static char     ret[255];
    struct enumer  *p;
    OID_TREE_ELE *ote;

    ote = LP_GetMIBNodeFromDot(string);
    if (ote == NULL) {
        goto no_trans;
    }
    if (ote->enumer == NULL) {
        goto no_trans;
    }
    for (p = ote->enumer; p != NULL; p = p->next) {
        if (p->number == value) {
            sprintf(ret, "%s("INT32_FORMAT")", p->name, value);
            return ret;
        }
    }
  no_trans:
    sprintf(ret, INT32_FORMAT, value);
    return ret;
}

char *GetBitsTypeWithOTE(OID_TREE_ELE *ote, const OctetString *value);

/*
 * GetBitsType:
 *
 *  Checks the enumerations for an OCTET_PRIM_TYPE MIB object.  If
 *  the  OCTET_PRIM_TYPE MIB object has enumerations, it must be
 *  a BITS type, so this routine goes ahead and formats the enumerations
 *  for which the corresponding bit positions are set.
 *
 *  Returns a a string with the formatted BITS value, or NULL.  
 *  The caller is responsible for freeing the returned string.  
 */
char *
GetBitsType(string, value)
    const char *string;
    const OctetString *value;
{
    return (GetBitsTypeWithOTE(LP_GetMIBNodeFromDot(string), value));
}

char *
GetBitsTypeWithOID(oid, value)
    const OID *oid;
    const OctetString *value;
{
    return (GetBitsTypeWithOTE(LP_GetMIBNodeFromOID(oid), value));
}

char *
GetBitsTypeWithOTE(ote, value)
    OID_TREE_ELE *ote;
    const OctetString *value;
{
    unsigned char   CurrentOctet, TestOctet, EnumPrinted = 0;
    SR_INT32        onum, bnum;
    char *res;
    static const size_t res_alloc_size = 255;
    size_t pos, size;
    struct enumer   *p;

    /* ----- Return if value is NULL or octet string length is 0 ----- */
    if (value == NULL) {
        return NULL;
    }
    if (value->length == 0) {
        return NULL;
    }

    /* ----- Return if we don't have a node in the tree ----- */
    if (ote == NULL) {
        return NULL;
    }

    /* ----- Return if it is not an enumeration ----- */
    if (ote->enumer == NULL) {
        return NULL;
    }

    if ((res = (char *) malloc(res_alloc_size)) == NULL) {
	return NULL;
    }
    size = res_alloc_size;
    res[pos = 0] = '\0';

    /* ----- Loop through enumerations ----- */
    for (p = ote->enumer; p != NULL; p = p->next) {

        /* ---- Calculate octet and bit number ----- */
        onum = p->number / 8;    
        bnum = p->number - (onum * 8);

        CurrentOctet  = value->octet_ptr[onum];
        TestOctet = 0x80 >> bnum;

        if ((CurrentOctet & TestOctet) != 0x00) {

	    if (EnumPrinted ? 2 : 0 + strlen(p->name) + 1 + 10 + 1 + 1 > 
		size - pos) {
		if ((res = 
		     (char *) realloc(res, size += res_alloc_size)) == NULL) {
		    return NULL;
		}
	    }

            /* ----- Print a comma if we've printed an enumeration ----- */
            if (EnumPrinted == 1) {
                strcat(res + pos, ", ");
		pos += 2;
            }

            /* ----- Print the enumeration ----- */
            sprintf(res + pos, "%s("INT32_FORMAT")", p->name, p->number);
	    while (res[pos] != '\0') {
		pos++;
	    }
            EnumPrinted = 1;   /* We've printed an enumeration */
        }
    }

    return res;
}

