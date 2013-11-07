/*
 *
 * Copyright (C) 2004-2006 by SNMP Research, Incorporated.
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

/* --- Needed for the malloc() prototype --- */
#include <stdlib.h>

#ifdef SR_CLEAR_MALLOC
/* --- Needed for the memset() prototype --- */
#include <string.h>
#endif /* SR_CLEAR_MALLOC */



/*
 *  srstrcat()
 *
 *  Function:  Provides a string concatenation function that is easier to
 *             use than strcat.  This function allocates a bigger buffer
 *             automatically.
 *
 *  Inputs:    original - The original string to which the second string
 *                        argument should be appended.  This argument may
 *                        be NULL.  If non-null, this argument may be freed
 *                        by this function, so it must be malloc'ed or
 *                        produced by strdup().
 *
 *             append   - The string to append to the end of the first
 *                        argument.  This argument may be NULL.  This
 *                        argument is *NOT* freed by this function, so it
 *                        can be a constant.
 *
 *  Output:    Returns a string produced by concatenating the original and
 *             append strings.  The returned string must be freed the caller.
 *             If the additional memory cannot be allocated, the original
 *             string is returned.
 *
 *  Notes:     This function is intended to be easier-to-use than the standard
 *             strcat() function.  Because of the way it handles memory, it
 *             may be used to build up arbitrarily long strings instead of
 *             using a fixed-length buffer whose max size must be known
 *             beforehand.   Example:
 *
 *                char *result = NULL;
 *
 *                result = srstrcat(NULL,   "<xmlStartElement>");
 *                result = srstrcat(result, "XML content");
 *                result = srstrcat(result, "</xmlStartElement>");
 *
 *                printf("%s", result);
 *                free(result);
 */
char *
srstrcat(char *original, char *append)
{
    int   append_length, original_length;
    char  *result = NULL;

    /* --- Return the original string if no append string --- */
    if (append == NULL) {
        return original;
    }

    /* --- Determine the string lengths --- */
    append_length = strlen(append);
    if (original == NULL) {
        original_length = 0;
    }
    else {
        original_length = strlen(original);
    }

    /* -- Allocate sufficient memory to contain the concatenated strings -- */
    result = (char *) malloc(append_length+original_length+1);
    if (result == NULL) {
        return original;
    }
    result[0] = '\0';
#ifdef SR_CLEAR_MALLOC
    memset(result, 0x00, (append_length+original_length+1));
#endif /* SR_CLEAR_MALLOC */
 
    /* --- Concatenate the two strings --- */
    if (original != NULL) {
        strcat(result, original);
        free(original);
    }
    strcat(result, append);
 
    /* --- Return the concatenated string --- */
    return result;
}   /* srstrcat() */


