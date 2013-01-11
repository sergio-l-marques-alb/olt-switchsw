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
 * -apo -f mibcompflags -userpart -v_stubs_only -row_status 
 * 
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
#include "tmq.h"
#include "rowstatf.h"
#include "diag.h"
SR_FILENAME
#include "min_v.h"
#include "mibout.h"

static const SR_UINT32 vacmSecurityToGroupEntry_last_sid_array[] = {
    1, /* vacmSecurityModel */
    2, /* vacmSecurityName */
    3, /* vacmGroupName */
    4, /* vacmSecurityToGroupStorageType */
    5  /* vacmSecurityToGroupStatus */
};

/*
 * The vacmSecurityToGroupEntryTypeTable array should be located in the
 * k_group.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType vacmSecurityToGroupEntryTypeTable[];

const SrIndexInfo vacmSecurityToGroupEntryIndexInfo[] = {
#ifdef I_vacmSecurityModel
    { I_vacmSecurityModel, T_uint, -1 },
#endif /* I_vacmSecurityModel */
#ifdef I_vacmSecurityName
    { I_vacmSecurityName, T_var_octet, -1 },
#endif /* I_vacmSecurityName */
    { -1, -1, -1 }
};

const SrGetInfoEntry vacmSecurityToGroupEntryGetInfo = {
    (SR_KGET_FPTR) new_k_vacmSecurityToGroupEntry_get,
#ifndef U_vacmSecurityToGroupEntry
    (SR_FREE_FPTR) NULL,
#else /* U_vacmSecurityToGroupEntry */
    (SR_FREE_FPTR) k_vacmSecurityToGroupEntryFreeUserpartData,
#endif /* U_vacmSecurityToGroupEntry */
    (int) sizeof(vacmSecurityToGroupEntry_t),
    I_vacmSecurityToGroupEntry_max,
    (SnmpType *) vacmSecurityToGroupEntryTypeTable,
    vacmSecurityToGroupEntryIndexInfo,
    (short) offsetof(vacmSecurityToGroupEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the vacmSecurityToGroupEntry family.
 *---------------------------------------------------------------------*/
VarBind *
vacmSecurityToGroupEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                             ContextInfo *contextInfo, int serialNum)
{
#if (! ( defined(I_vacmSecurityModel) ) || ! ( defined(I_vacmSecurityName) ))
    return NULL;
#else	/* (! ( defined(I_vacmSecurityModel) ) || ! ( defined(I_vacmSecurityName) )) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &vacmSecurityToGroupEntryGetInfo));
#endif	/* (! ( defined(I_vacmSecurityModel) ) || ! ( defined(I_vacmSecurityName) )) */
}

vacmSecurityToGroupEntry_t *
new_k_vacmSecurityToGroupEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator, int searchType,
                                   vacmSecurityToGroupEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_vacmSecurityToGroupEntry_get(serialNum, contextInfo, nominator,
                                          searchType, data->vacmSecurityModel,
                                          data->vacmSecurityName);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int vacmSecurityToGroupEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the vacmSecurityToGroupEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   vacmSecurityModel_range[] = { { 1, 2147483647 } };
static LengthTest_t  vacmSecurityName_len[] = { { 1, 32 } };
static LengthTest_t  vacmGroupName_len[] = { { 1, 32 } };
static RangeTest_t   vacmSecurityToGroupStorageType_range[] = { { 1, 5 } };
static RangeTest_t   vacmSecurityToGroupStatus_range[] = { { 1, 6 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * vacmSecurityToGroupEntry family.
 */
static struct sr_member_test vacmSecurityToGroupEntry_member_test[] =
{
    /* vacmSecurityModel */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmSecurityModel_range)/sizeof(RangeTest_t), /* 1 */
      vacmSecurityModel_range, NULL },

    /* vacmSecurityName */
    { MINV_LENGTH_TEST, 
      sizeof(vacmSecurityName_len)/sizeof(LengthTest_t), /* 1 */
      vacmSecurityName_len, NULL },

    /* vacmGroupName */
    { MINV_LENGTH_TEST, 
      sizeof(vacmGroupName_len)/sizeof(LengthTest_t), /* 1 */
      vacmGroupName_len, NULL },

    /* vacmSecurityToGroupStorageType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmSecurityToGroupStorageType_range)/sizeof(RangeTest_t), /* 5 */
      vacmSecurityToGroupStorageType_range, StorageType_test },

    /* vacmSecurityToGroupStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmSecurityToGroupStatus_range)/sizeof(RangeTest_t), /* 6 */
      vacmSecurityToGroupStatus_range, RowStatus_test }
};


/*
 * Initialize SrTestInfoEntry for the vacmSecurityToGroupEntry family.
 */
const SrTestInfoEntry vacmSecurityToGroupEntryTestInfo = {
    &vacmSecurityToGroupEntryGetInfo,
    vacmSecurityToGroupEntry_member_test,
    k_vacmSecurityToGroupEntry_set_defaults,
    k_vacmSecurityToGroupEntry_test,
    k_vacmSecurityToGroupEntry_ready,
#ifdef SR_vacmSecurityToGroupEntry_UNDO
    vacmSecurityToGroupEntry_undo,
#else /* SR_vacmSecurityToGroupEntry_UNDO */
    NULL,
#endif /* SR_vacmSecurityToGroupEntry_UNDO */
    vacmSecurityToGroupEntry_ready,
    vacmSecurityToGroupEntry_set,
    vacmSecurityToGroupEntry_cleanup,
#ifndef U_vacmSecurityToGroupEntry
    (SR_COPY_FPTR) NULL
#else /* U_vacmSecurityToGroupEntry */
    (SR_COPY_FPTR) k_vacmSecurityToGroupEntryCloneUserpartData
#endif /* U_vacmSecurityToGroupEntry */
};

/*----------------------------------------------------------------------
 * Free the vacmSecurityToGroupEntry data object.
 *---------------------------------------------------------------------*/
void
vacmSecurityToGroupEntry_free(vacmSecurityToGroupEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 vacmSecurityToGroupEntryGetInfo.type_table,
                 vacmSecurityToGroupEntryGetInfo.highest_nominator,
                 vacmSecurityToGroupEntryGetInfo.valid_offset,
                 vacmSecurityToGroupEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after vacmSecurityToGroupEntry set/undo
 *---------------------------------------------------------------------*/
static int
vacmSecurityToGroupEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &vacmSecurityToGroupEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the vacmSecurityToGroupEntry family
 *---------------------------------------------------------------------*/
vacmSecurityToGroupEntry_t *
Clone_vacmSecurityToGroupEntry(vacmSecurityToGroupEntry_t *vacmSecurityToGroupEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (vacmSecurityToGroupEntry_t *)SrCloneFamily(vacmSecurityToGroupEntry,
                         vacmSecurityToGroupEntryGetInfo.family_size,
                         vacmSecurityToGroupEntryGetInfo.type_table,
                         vacmSecurityToGroupEntryGetInfo.highest_nominator,
                         vacmSecurityToGroupEntryGetInfo.valid_offset,
                         vacmSecurityToGroupEntryTestInfo.userpart_clone_func,
                         vacmSecurityToGroupEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
vacmSecurityToGroupEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                              doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &vacmSecurityToGroupEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
vacmSecurityToGroupEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &vacmSecurityToGroupEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
vacmSecurityToGroupEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_vacmSecurityToGroupEntry_set((vacmSecurityToGroupEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


