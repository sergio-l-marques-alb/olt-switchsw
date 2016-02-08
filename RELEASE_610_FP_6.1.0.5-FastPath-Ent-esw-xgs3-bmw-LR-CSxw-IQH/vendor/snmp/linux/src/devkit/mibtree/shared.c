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



/*
 * FindMaxBitStringVal:
 *	Find the maximum value allowed for the bitstring variable, name.
 *
 *      Parameters: name   - the name of the bitstring variable.
 *      Returns     -1     - error
 *                  maxval - on success
 *
 */
SR_INT32
FindMaxBitStringVal(name)
const char *name;
{
    OID_TREE_ELE	*ote;
    SR_INT32             maxval;
    struct enumer	*p;

    maxval = 0;
    ote = LP_GetMIBNodeFromDot(name);

    if (ote == NULL) {
        return -1;
    }
    if (ote->enumer == NULL) {
	FreeOTE(ote);
        return -1;
    }

    for (p = ote->enumer; p != NULL; p = p->next) {
        if (p->number > maxval) {
            maxval = p->number;
        }
    }

    FreeOTE(ote);
    return maxval;
}

/*
 * GetEnumVal:
 *      Returns the numeric value for a string value, which may contain
 *      an enumeration (e.g., for Integer32 or BITS).
 *
 *      Parameters: name   - the name of the MIB object
 *                  value  - the value of the MIB object represented
 *                           as an ASCII string
 *      Returns     -1     - error
 *                  number - numeric form of value string
 *
 * Example values for value:
 *
 *      number only
 *      Example: 1
 *      Example: (1)
 *
 *      enumeration only
 *      Example: up
 *
 *      enumeration(number) combination
 *      Example: up(1)
 *
 * Additional notes for value:     
 *
 *      The matching of enumeration is not case-sensitive.
 *      Example: "UP" is the same as "up" or "Up".
 *
 *      The value may contain embedded spaces.
 *      Example: " up(  1) " is the same as "up(1)".
 * 
 *      If the enumeration is not recognized, and if a parenthesized
 *      numeric value is supplied that is not recognized, then the
 *      enumeration and value are assumed to be correct (i.e., it
 *      wasn't in the snmpinfo.dat file loaded into the MIB tree)
 *      and the parenthesized numeric value is returned.
 *      Example: "superDooperNewInterface(123456789)" for ifType
 *
 *      If the enumeration is recognized, and if a parenthesized
 *      numeric value is supplied that does not match the known
 *      value of the enumeration, the function returns an error.
 *
 */
SR_INT32
GetEnumVal(name, value)
const char *name;
const char *value;
{
    FNAME("GetEnumVal")
    OID_TREE_ELE        *ote = NULL;
    struct enumer       *p = NULL;
    char                *copy = NULL, *nump = NULL, *cp = NULL, *sp = NULL;
    int                  len = 0, open_paren = 0, close_paren = 0, num = 0;
    int                  retval = -1;
    int                  allnum = 1, numfromtext = 0;

    /* validate parameters */
    if (name == NULL || value == NULL) {
        DPRINTF((APERROR, "%s: NULL parameter(s)\n", Fname));
        goto GetEnumVal_fail;
    }
    if ((strlen(name)) < 1) {
        DPRINTF((APERROR, "%s: name is an empty string\n", Fname));
        goto GetEnumVal_fail;
    }
    if ((len = strlen(value)) < 1) {
        DPRINTF((APERROR, "%s: value is an empty string\n", Fname));
        goto GetEnumVal_fail;
    }

    /* if string contains only digits and spaces, just convert to int */
    allnum = 1;
    for (cp = (char *) value; *cp != '\0'; cp++) {
        if ((isdigit((unsigned char)*cp)) || (isspace((unsigned char)*cp))) {
            continue;
        }
        allnum = 0;
        break;
    }
    if (allnum) {
        DPRINTF((APTRACE, "%s: value is entirely numeric\n", Fname));
        if ((sscanf((char *)value, INT32_FORMAT, &numfromtext)) != 1) {
            DPRINTF((APERROR, "%s: sscanf failed: %s\n", Fname, value));
            goto GetEnumVal_fail;
        }
        retval = numfromtext;
        goto GetEnumVal_success;
    }

    /* copy value string */
    copy = (char *)malloc(len+1);
    if (copy == NULL) {
        DPRINTF((APERROR, "%s: could not copy value\n", Fname, name));
        goto GetEnumVal_fail;
    }
    strcpy(copy, value);

    /* search for (and tokenize) a parenthesized numeric value */
    for (cp = copy; *cp != '\0'; cp++) {
        switch(*cp) {
            case '(':
                if (open_paren) {
                    /* an open parenthesis has already been encountered */
                    DPRINTF((APERROR, "%s: extra '(' in bits value\n", Fname));
                    goto GetEnumVal_fail;
                }
                open_paren = 1;
                *cp = '\0';  /* nul-terminate the enumeration part, if any */
                continue;
            case ')':
                if (!open_paren) {
                    /* a close parenthesis found before open parenthesis */
                    DPRINTF((APERROR, "%s: missing '(' in bits value\n",
                             Fname));
                    goto GetEnumVal_fail;
                }
                if (close_paren) {
                    /* a close parenthesis has already been encountered */
                    DPRINTF((APERROR, "%s: extra ')' in bits value\n", Fname));
                    goto GetEnumVal_fail;
                }
                if (!num) {
                    /* a close parenthesis has already been encountered */
                    DPRINTF((APERROR,
                             "%s: found ')' before a number in bits value\n",
                             Fname));
                    goto GetEnumVal_fail;
                }
                close_paren = 1;
                *cp = '\0';  /* nul-terminate the number part */
                continue;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (!open_paren) {
                    /* number before paren could be part of the enumeration */
                    continue;
                }
                if (close_paren) {
                    /* a close parenthesis has already been encountered */
                    DPRINTF((APERROR,
                             "%s: number following ')' in bits value\n",
                             Fname));
                    goto GetEnumVal_fail;
                }
                if (!num) {
                    /* point to beginning of parenthesized number part */
                    nump = cp;
                }
                num = 1;
                continue;
            case ' ':
                continue;
            default:
                if (open_paren) {
                    DPRINTF((APERROR,
                             "%s: bad character %c(%u) following '('\n",
                             Fname, *cp, ((unsigned)*cp)));
                    goto GetEnumVal_fail;
                }
                if (close_paren) {
                    DPRINTF((APERROR,
                             "%s: bad character %c(%u) following ')'\n",
                             Fname, *cp, ((unsigned)*cp)));
                    goto GetEnumVal_fail;
                }
                continue;
        }
    }
    if (open_paren != close_paren) {
        DPRINTF((APERROR, "%s: unbalanced parentheses in bits value\n", Fname));
        goto GetEnumVal_fail;
    }

    /* if a parenthesized number was found, convert it to an int */
    if (nump != NULL) {
        if ((sscanf(nump, INT32_FORMAT, &numfromtext)) != 1) {
            DPRINTF((APERROR, "%s: sscanf failed: %s\n", Fname, nump));
            goto GetEnumVal_fail;
        }
    }

    /* skip leading spaces to find the beginning of the enumeration */
    for (cp = copy; *cp == ' '; cp++) /* do nothing */;

    /* truncate trailing spaces */
    for (sp = cp; *sp != '\0'; sp++) {
        if (*sp == ' ') {
            *sp = '\0';
            break;
        }
    }

    /* if there is a recognized enumeration, return its numeric value */
    if (*cp != '\0') {
        /* there is an enumeration, look up its numeric value */
        ote = LP_GetMIBNodeFromDot(name);
        if (ote == NULL) {
            DPRINTF((APERROR, "%s: could not find %s in the MIB Tree\n",
                     Fname, name));
            goto GetEnumVal_fail;
        }
        if (ote->enumer == NULL) {
            DPRINTF((APWARN, "%s: %s has no enumerations in the MIB Tree\n",
                     Fname, name));
            goto GetEnumVal_fail;
        }
        for (p = ote->enumer; p != NULL; p = p->next) {
            if (strcasecmp(p->name, cp) == 0) {
                /* the numeric value of the enumeration was found */
                if (nump != NULL) {
                    /* there was also a parenthesized value, compare them */
                    if (p->number == numfromtext) {
                        /* the values match */
                        DPRINTF((APTRACE, "%s: enumeration value matches "
                                 "parenthesized value\n", Fname));
                        retval = p->number; 
                        goto GetEnumVal_success;
                    } else {
                        /* the values do not match */
                        DPRINTF((APERROR, "%s: enumeration value does not "
                                 "match parenthesized value\n", Fname));
                        goto GetEnumVal_fail;
                    }
                } else {
                    /* there was not a parenthesized value...success */
                    DPRINTF((APTRACE, "%s: enumeration value found "
                             "(no parenthesized value)\n", Fname));
                    retval = p->number; 
                    goto GetEnumVal_success;
                }
            }
        }
        /*
         * The supplied enumeration was not found.  If a parenthesized
         * number was also supplied, see if it has a known enumerated
         * value.  If not, then consider this a valid enumeration.
         *
         * Example: as new types of interfaces are being created, new
         * values for ifType are being defined that weren't defined in
         * RFC 1213 and which may not be loaded into the MIB tree.
         */
        if (nump != NULL) {
            for (p = ote->enumer; p != NULL; p = p->next) {
                if (p->number == numfromtext) {
                    DPRINTF((APERROR,
                             "%s: the correct enumeration for %d is \"%s\",\n"
                             "and the string \"%s\" is not recognized.\n",
                             Fname, numfromtext, p->name, cp));
                    goto GetEnumVal_fail;
                }
            }
        }
        DPRINTF((APWARN, "%s: enumeration \"%s\" unknown but assumed valid\n",
                 Fname, cp));
    }

    /* if a parenthesized number was found, return it */
    if (nump != NULL) {
        DPRINTF((APTRACE, "%s: returning parenthesized value\n", Fname));
        retval = numfromtext;
        goto GetEnumVal_success;
    }

    GetEnumVal_fail:
        if (copy != NULL) {
            free(copy);
            copy = NULL;
        }
        if (ote != NULL) {
            FreeOTE(ote);
            ote = NULL;
        }
        return -1;

    GetEnumVal_success:
        if (copy != NULL) {
            free(copy);
            copy = NULL;
        }
        if (ote != NULL) {
            FreeOTE(ote);
            ote = NULL;
        }
        return (retval);
}

/*
 * MakeBitsValue:
 *      Returns the octet string value needed for a set of a bitstring 
 *      variable. The value string can be a string of one or more numbers
 *      or enumerated values separated by commas. This routine returns NULL on 
 *      error and the octet string on success. 
 *
 *      Parameters: name   - the name of the bitstring variable
 *                  value  - bit(s) to set for this variable. 
 *      Returns     NULL   - error
 *                  octet string - String with all the bits set for the given
 *                                 input values. 
 *
 * Example values for value:
 *
 *	setall - set all possible bits for this bitstring variable.
 *
 *	clearall - clear all possible bits for this bitstring variable.
 *
 *	number  - set one particular bit in bitstring variable.
 *	Example: 4
 *
 *	enumeration  - set one particular bit in bitstring variable.
 *	Example: wednesday
 *
 *	enumeration(number)  - set one particular bit in bitstring variable.
 *	Example: wednesday(3)
 *
 *	Any combination of enumerations and numbers separated by commas.
 *	Example: 4,tuesday,5,wednesday(3),0 
 * 
 * Additional notes for value:     
 *
 *      The matching of enumeration is not case-sensitive.
 *      Example: "WEDNESDAY" is the same as "wednesday" or "WeDnEsDaY".
 *
 *      The value may contain embedded spaces.
 *      Example: " wednesday(  3) " is the same as "wednesday(3)".
 * 
 */
OctetString *
MakeBitsValue(name, value)
const char *name;
const char *value;
{
    FNAME("MakeBitsValue")
    SR_INT32             bitpos, bytepos, len, maxval, val, pos;
    OctetString         *os_ptr;
    char                *token, *buf, *cp;

    if (name == NULL) {
        DPRINTF((APWARN, "%s: incoming name is NULL.\n", Fname));
        return NULL;
    }
    if (value == NULL) {
        DPRINTF((APWARN, "%s: incoming value is NULL.\n", Fname));
        return NULL;
    }

    maxval = FindMaxBitStringVal(name);
    if (maxval == -1) {
        return NULL;
    }
 
    len = maxval / 8;
    if ( (len * 8) != maxval) {
        len++;  /* pad to nearest full octet */
    }
    os_ptr = MakeOctetString(NULL, len);
    if (os_ptr == NULL) {
        return NULL;
    }

    /* While loop to strip off each value from string. Also check each value
     * to see whether what we have is enumerated or just a number value.
     */

    buf = (char *)value;

    /* set all bits */
    if (strcmp(buf, "-setall") == 0) {
        for (pos = 0; pos < len; pos++) {
                os_ptr->octet_ptr[pos] = 0xFF;
        }
        buf = NULL;
    }
    /* clear all bits */
    else if (strcmp(buf, "-clearall") == 0) {
        /* Nothing to be done here. Should already be zeroed out. */
        buf = NULL;
    }
        
    while (buf != NULL) {
        /* Get next token */
        cp = strchr(buf, ',');

        if (cp != NULL) {
            token = buf;
            *cp = '\0';
            buf = cp + 1;
        }
        else {
            token = buf;
        }

        if ((val = GetEnumVal(name, token)) == -1) {
            DPRINTF((APTRACE, "%s: Unable to parse given arguments.\n", Fname));
            FreeOctetString(os_ptr);
            return NULL;
        }
        if (val > maxval) {
            DPRINTF((APERROR, 
                     "%s: the value specified (%d) is greater than\n"
                     "the maximum value (%d) for the specified MIB object\n"
                     "(%s).\n", 
                     Fname, val, maxval, name));
            FreeOctetString(os_ptr);
            return NULL;
        }

        bytepos = val / 8;
        bitpos = val - (bytepos * 8);
        os_ptr->octet_ptr[bytepos] |= 0x80 >> bitpos;

        if (cp == NULL) {
            buf = NULL;
        }
    }

    return os_ptr;
}

