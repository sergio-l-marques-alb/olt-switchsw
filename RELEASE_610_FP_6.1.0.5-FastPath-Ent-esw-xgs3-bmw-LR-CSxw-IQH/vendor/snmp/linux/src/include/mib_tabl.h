/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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

#ifndef SR_MIB_TABL_H
#define SR_MIB_TABL_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef SR_MIB_TABLE_HASHING
#define NAME_OID_HASH_SIZE 2000
#endif				/* SR_MIB_TABLE_HASHING */

struct MIB_OID {
    char           *name;
    char           *number;
#ifdef SR_MIB_TABLE_HASHING
    struct MIB_OID    *hash_next;
#endif /* SR_MIB_TABLE_HASHING */
};

#ifndef DO_NOT_DECLARE_MIB_OID_TABLE
extern struct MIB_OID *mib_oid_table;
extern char *unassigned_text;
extern int num_mot_entries;
#endif				/* DO_NOT_DECLARE_MIB_OID_TABLE */


void InitNameOIDHashArray(void);

void InitNewOIDRoutines(void);

#ifdef SR_MIB_TABLE_HASHING
void InitNameOIDHashArray(void);
#endif /* SR_MIB_TABLE_HASHING */

#ifdef  __cplusplus
}
#endif

/* Do Not put anything after this #endif */
#endif	/* !SR_MIB_TABL_H */
