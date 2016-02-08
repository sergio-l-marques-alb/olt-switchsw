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
 */


/*
 * Arguments used to create this file:
 * -apo -f mibcompflags -userpart -v_stubs_only 
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <stddef.h>
#include "sr_snmp.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "inst_lib.h"
#include "oid_lib.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME
#include "min_v.h"
#include "mibout.h"

static const SR_UINT32 atEntry_last_sid_array[] = {
    1, /* atIfIndex */
    2, /* atPhysAddress */
    3  /* atNetAddress */
};

const SnmpType atEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(atEntry_t, atIfIndex), 0 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(atEntry_t, atPhysAddress), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(atEntry_t, atNetAddress), 1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrIndexInfo atEntryIndexInfo[] = {
#ifdef I_atIfIndex
    { I_atIfIndex, T_uint, -1 },
#endif /* I_atIfIndex */
#ifdef I_atNetAddress
    { I_atNetAddress, T_networkaddress, 5 },
#endif /* I_atNetAddress */
    { -1, -1, -1 }
};

const SrGetInfoEntry atEntryGetInfo = {
    (SR_KGET_FPTR) new_k_atEntry_get,
#ifndef U_atEntry
    (SR_FREE_FPTR) NULL,
#else /* U_atEntry */
    (SR_FREE_FPTR) k_atEntryFreeUserpartData,
#endif /* U_atEntry */
    (int) sizeof(atEntry_t),
    I_atEntry_max,
    (SnmpType *) atEntryTypeTable,
    atEntryIndexInfo,
    (short) offsetof(atEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the atEntry family.
 *---------------------------------------------------------------------*/
VarBind *
atEntry_get(OID *incoming, ObjectInfo *object, int searchType,
            ContextInfo *contextInfo, int serialNum)
{
#if (! ( defined(I_atIfIndex) ) || ! ( defined(I_atNetAddress) ))
    return NULL;
#else	/* (! ( defined(I_atIfIndex) ) || ! ( defined(I_atNetAddress) )) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &atEntryGetInfo));
#endif	/* (! ( defined(I_atIfIndex) ) || ! ( defined(I_atNetAddress) )) */
}

atEntry_t *
new_k_atEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator, int searchType,
                  atEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_atEntry_get(serialNum, contextInfo, nominator,
                         searchType, data->atIfIndex, data->atNetAddress);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int atEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif


/*
 * Initialize the sr_member_test array with one entry per object in the
 * atEntry family.
 */
static struct sr_member_test atEntry_member_test[] =
{
    /* atIfIndex */
    { MINV_NO_TEST, 0, NULL, Identity_test },

    /* atPhysAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* atNetAddress */
    { MINV_NO_TEST, 0, NULL, Identity_test }
};


/*
 * Initialize SrTestInfoEntry for the atEntry family.
 */
const SrTestInfoEntry atEntryTestInfo = {
    &atEntryGetInfo,
    atEntry_member_test,
#ifdef atEntry_READ_CREATE
    k_atEntry_set_defaults,
#else /* atEntry_READ_CREATE */
    NULL,
#endif /* atEntry_READ_CREATE */
    k_atEntry_test,
    k_atEntry_ready,
#ifdef SR_atEntry_UNDO
    atEntry_undo,
#else /* SR_atEntry_UNDO */
    NULL,
#endif /* SR_atEntry_UNDO */
    atEntry_ready,
    atEntry_set,
    atEntry_cleanup,
#ifndef U_atEntry
    (SR_COPY_FPTR) NULL
#else /* U_atEntry */
    (SR_COPY_FPTR) k_atEntryCloneUserpartData
#endif /* U_atEntry */
};

/*----------------------------------------------------------------------
 * Free the atEntry data object.
 *---------------------------------------------------------------------*/
void
atEntry_free(atEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 atEntryGetInfo.type_table,
                 atEntryGetInfo.highest_nominator,
                 atEntryGetInfo.valid_offset,
                 atEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after atEntry set/undo
 *---------------------------------------------------------------------*/
static int
atEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &atEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the atEntry family
 *---------------------------------------------------------------------*/
atEntry_t *
Clone_atEntry(atEntry_t *atEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (atEntry_t *)SrCloneFamily(atEntry,
                         atEntryGetInfo.family_size,
                         atEntryGetInfo.type_table,
                         atEntryGetInfo.highest_nominator,
                         atEntryGetInfo.valid_offset,
                         atEntryTestInfo.userpart_clone_func,
                         atEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
atEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
             doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &atEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
atEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &atEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
atEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_atEntry_set((atEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


