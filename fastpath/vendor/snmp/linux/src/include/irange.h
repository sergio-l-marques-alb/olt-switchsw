/*
 *
 * Copyright (C) 2005-2006 by SNMP Research, Incorporated.
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
 *  This file defines structures, constants, and prototypes for APIs
 *  that parse strings containing integer ranges.
 */

#ifndef SR_IRANGE_H
#define SR_IRANGE_H

#ifdef  __cplusplus
extern "C" {
#endif


/*
 *  The following structure holds information about integer ranges.  The
 *  ParseIntegerRanges() function returns an array of these structures.
 */
typedef struct iRange_t {
    SR_INT32  count;   /* Number in the range, including first and last */
    SR_INT32  first;   /* First number of the range */
    SR_INT32  last;    /* Last number of the range */
} iRange;


/*
 *  ParseIntegerRanges()
 *
 *  Function:  Parses a string containing one or more integer ranges into
 *             an array of iRange structures.
 *
 *  Inputs:   rangeString - specifies a string of integer ranges.
 *
 *            Format example:    161,165,201-210,300,305-301,400-403-405
 *
 *            This example would result in the following array of iRange
 *            structures:
 *
 *            Array Element   Count   First   Last   Notes
 *            -------------   -----   -----   ----   -----------------------
 *                 0            1      161    161
 *                 1            1      165    165
 *                 2            10     201    210
 *                 3            1      300    300
 *                 4            5      305    301
 *                 5            0       0      0     Invalid range specified
 *                 6           -1       0      0     Array Terminator
 *
 *  Outputs:  On failure, returns NULL.  On success, returns an array of
 *            portRange structures.  The last element of the array has the
 *            count value set to -1.  Note this array must be freed by the
 *            caller with the standard free() function.
 */
iRange *ParseIntegerRanges(const char *rangeString);


#ifdef  __cplusplus
}
#endif

#endif  /* SR_IRANGE_H */

