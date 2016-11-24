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


#define DO_NOT_DECLARE_MIB_OID_TABLE

#include "sr_snmp.h"

#include "mib_tabl.h"

#ifdef SR_MIB_TABLE_HASHING
#define HASHNEXT(x) x, NULL
#else /* SR_MIB_TABLE_HASHING */
#define HASHNEXT(x) x
#endif /* SR_MIB_TABLE_HASHING */
static struct MIB_OID default_mib_oid_table[] = {
    { "iso",   HASHNEXT("1") },
    { 0,       HASHNEXT(0) }
};
struct MIB_OID *mib_oid_table = default_mib_oid_table;
int             num_mot_entries;
char           *unassigned_text = "UNASSIGNED";

#ifdef SR_MIB_TABLE_HASHING
struct MIB_OID *name_oid_hash_array[NAME_OID_HASH_SIZE];
#endif /* SR_MIB_TABLE_HASHING */


/*
 * InitNewOIDRoutines:
 *
 *   Routines to initialize the new data structures that will speed
 *   name * to OID and OID to name translations at the expense of
 *   memory and * initialization time.
 *
 * The former name (pre-snmp12.1.0.0 release) was init_new_oid_routines().
 */
int             global_InitNewOIDRoutines = 0;

void
InitNewOIDRoutines()
{
#ifdef SR_MIB_TABLE_HASHING
    int i;

    /* Clear out any old info from hash array */
    for (i = 0; i < NAME_OID_HASH_SIZE; i++) {
        name_oid_hash_array[i] = NULL;
    }

    InitNameOIDHashArray();
#else /* SR_MIB_TABLE_HASHING */
    for (num_mot_entries = 0;
         mib_oid_table[num_mot_entries].name != NULL;
         num_mot_entries++);
#endif /* SR_MIB_TABLE_HASHING */

    global_InitNewOIDRoutines = 1;
}

/*
 * InitNameOIDHashArray: 
 *
 * The former name (pre-snmp12.1.0.0 release) was init_name_oid_hash_array(). 
 */
#ifdef SR_MIB_TABLE_HASHING
void
InitNameOIDHashArray()
{
    int             i, j, hash;
    struct MIB_OID *temp_hash_ptr;

    /* for each line in the mib translation table */
    for (i = 0; mib_oid_table[i].name != NULL; i++) {
        /* calculate hash */
        hash = 0;
        for (j = (strlen(mib_oid_table[i].name) - 1); j >= 0; j--) {
            hash = (hash + mib_oid_table[i].name[j]) % NAME_OID_HASH_SIZE;
        }

        /* now add entry to hash array */
        if (name_oid_hash_array[hash] == NULL) {
            name_oid_hash_array[hash] = &(mib_oid_table[i]);
            name_oid_hash_array[hash]->hash_next = NULL;
        } else {
            /* Walk to the end of the list, and put it there */
            temp_hash_ptr = name_oid_hash_array[hash];
            while (temp_hash_ptr->hash_next != NULL) {
                temp_hash_ptr = temp_hash_ptr->hash_next;
            }
            temp_hash_ptr->hash_next = &(mib_oid_table[i]);
            temp_hash_ptr->hash_next->hash_next = NULL;
        }
    }
    num_mot_entries = i;
}
#endif /* SR_MIB_TABLE_HASHING */
