/*
 *
 * Copyright (C) 2006 by SNMP Research, Incorporated.
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

#ifndef SR_PATH_CHECK_H
#define SR_PATH_CHECK_H


/*
 *  The following flag indicates that SrPathCheck() should skip checking
 *  to see if the file and path is owned by the "root" user.
 */
#define SR_PC_ANY_OWNER 0x0001


/*
 *  SrPathCheck()
 *
 *  Function:  Verifies a file is writable only by root (or its owner).
 *
 *  Inputs:    filepath - Specifies the complete file path from the file
 *                        system root and including the file name.
 *             flags    - flags that modify the function's behavior:
 *
 *                        SR_PC_ANY_OWNER - skip the "root" user check
 *
 *  Outputs:   Returns 0 if the file is writable only by root (or its owner).
 *             Returns -1 otherwise.
 *
 *  Notes:     This function also checks all subdirectories in the path
 *             from the file system root.
 *
 *             This function is useless on MS-Windows because the MS-Windows
 *             implementation of stat():
 *
 *             -  *always* returns a st_uid value of 0 (root).
 *             -  does not provide information about whether a file
 *                can be read or written by group or other.
 */
int
SrPathCheck(const char *filepath, const SR_UINT32 flags);

/* Do not put anything after this #endif */
#endif	/* SR_PATH_CHECK_H */
