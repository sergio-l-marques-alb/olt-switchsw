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

static const SR_UINT32 vacmAccessEntry_last_sid_array[] = {
    1, /* vacmAccessContextPrefix */
    2, /* vacmAccessSecurityModel */
    3, /* vacmAccessSecurityLevel */
    4, /* vacmAccessContextMatch */
    5, /* vacmAccessReadViewName */
    6, /* vacmAccessWriteViewName */
    7, /* vacmAccessNotifyViewName */
    8, /* vacmAccessStorageType */
    9  /* vacmAccessStatus */
};

/*
 * The vacmAccessEntryTypeTable array should be located in the
 * k_ac.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType vacmAccessEntryTypeTable[];

const SrIndexInfo vacmAccessEntryIndexInfo[] = {
#ifdef I_vacmAccessEntryIndex_vacmGroupName
    { I_vacmAccessEntryIndex_vacmGroupName, T_var_octet, -1 },
#endif /* I_vacmAccessEntryIndex_vacmGroupName */
#ifdef I_vacmAccessContextPrefix
    { I_vacmAccessContextPrefix, T_var_octet, -1 },
#endif /* I_vacmAccessContextPrefix */
#ifdef I_vacmAccessSecurityModel
    { I_vacmAccessSecurityModel, T_uint, -1 },
#endif /* I_vacmAccessSecurityModel */
#ifdef I_vacmAccessSecurityLevel
    { I_vacmAccessSecurityLevel, T_uint, -1 },
#endif /* I_vacmAccessSecurityLevel */
    { -1, -1, -1 }
};

const SrGetInfoEntry vacmAccessEntryGetInfo = {
    (SR_KGET_FPTR) new_k_vacmAccessEntry_get,
#ifndef U_vacmAccessEntry
    (SR_FREE_FPTR) NULL,
#else /* U_vacmAccessEntry */
    (SR_FREE_FPTR) k_vacmAccessEntryFreeUserpartData,
#endif /* U_vacmAccessEntry */
    (int) sizeof(vacmAccessEntry_t),
    I_vacmAccessEntry_max,
    (SnmpType *) vacmAccessEntryTypeTable,
    vacmAccessEntryIndexInfo,
    (short) offsetof(vacmAccessEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the vacmAccessEntry family.
 *---------------------------------------------------------------------*/
VarBind *
vacmAccessEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                    ContextInfo *contextInfo, int serialNum)
{
#if (((! ( defined(I_vacmAccessEntryIndex_vacmGroupName) ) || ! ( defined(I_vacmAccessContextPrefix) )) || ! ( defined(I_vacmAccessSecurityModel) )) || ! ( defined(I_vacmAccessSecurityLevel) ))
    return NULL;
#else	/* (((! ( defined(I_vacmAccessEntryIndex_vacmGroupName) ) || ! ( defined(I_vacmAccessContextPrefix) )) || ! ( defined(I_vacmAccessSecurityModel) )) || ! ( defined(I_vacmAccessSecurityLevel) )) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &vacmAccessEntryGetInfo));
#endif	/* (((! ( defined(I_vacmAccessEntryIndex_vacmGroupName) ) || ! ( defined(I_vacmAccessContextPrefix) )) || ! ( defined(I_vacmAccessSecurityModel) )) || ! ( defined(I_vacmAccessSecurityLevel) )) */
}

vacmAccessEntry_t *
new_k_vacmAccessEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator, int searchType,
                          vacmAccessEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_vacmAccessEntry_get(serialNum, contextInfo, nominator,
                                 searchType, data->vacmGroupName,
                                 data->vacmAccessContextPrefix,
                                 data->vacmAccessSecurityModel,
                                 data->vacmAccessSecurityLevel);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int vacmAccessEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the vacmAccessEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  vacmAccessContextPrefix_len[] = { { 0, 32 } };
static RangeTest_t   vacmAccessSecurityModel_range[] = { { 0, 2147483647 } };
static RangeTest_t   vacmAccessSecurityLevel_range[] = { { 1, 3 } };
static RangeTest_t   vacmAccessContextMatch_range[] = { { 1, 2 } };
static LengthTest_t  vacmAccessReadViewName_len[] = { { 0, 32 } };
static LengthTest_t  vacmAccessWriteViewName_len[] = { { 0, 32 } };
static LengthTest_t  vacmAccessNotifyViewName_len[] = { { 0, 32 } };
static RangeTest_t   vacmAccessStorageType_range[] = { { 1, 5 } };
static RangeTest_t   vacmAccessStatus_range[] = { { 1, 6 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * vacmAccessEntry family.
 */
static struct sr_member_test vacmAccessEntry_member_test[] =
{
    /* vacmAccessContextPrefix */
    { MINV_LENGTH_TEST, 
      sizeof(vacmAccessContextPrefix_len)/sizeof(LengthTest_t), /* 1 */
      vacmAccessContextPrefix_len, NULL },

    /* vacmAccessSecurityModel */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmAccessSecurityModel_range)/sizeof(RangeTest_t), /* 1 */
      vacmAccessSecurityModel_range, NULL },

    /* vacmAccessSecurityLevel */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmAccessSecurityLevel_range)/sizeof(RangeTest_t), /* 3 */
      vacmAccessSecurityLevel_range, NULL },

    /* vacmAccessContextMatch */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmAccessContextMatch_range)/sizeof(RangeTest_t), /* 2 */
      vacmAccessContextMatch_range, NULL },

    /* vacmAccessReadViewName */
    { MINV_LENGTH_TEST, 
      sizeof(vacmAccessReadViewName_len)/sizeof(LengthTest_t), /* 1 */
      vacmAccessReadViewName_len, NULL },

    /* vacmAccessWriteViewName */
    { MINV_LENGTH_TEST, 
      sizeof(vacmAccessWriteViewName_len)/sizeof(LengthTest_t), /* 1 */
      vacmAccessWriteViewName_len, NULL },

    /* vacmAccessNotifyViewName */
    { MINV_LENGTH_TEST, 
      sizeof(vacmAccessNotifyViewName_len)/sizeof(LengthTest_t), /* 1 */
      vacmAccessNotifyViewName_len, NULL },

    /* vacmAccessStorageType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmAccessStorageType_range)/sizeof(RangeTest_t), /* 5 */
      vacmAccessStorageType_range, StorageType_test },

    /* vacmAccessStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmAccessStatus_range)/sizeof(RangeTest_t), /* 6 */
      vacmAccessStatus_range, RowStatus_test }
};


/*
 * Initialize SrTestInfoEntry for the vacmAccessEntry family.
 */
const SrTestInfoEntry vacmAccessEntryTestInfo = {
    &vacmAccessEntryGetInfo,
    vacmAccessEntry_member_test,
    k_vacmAccessEntry_set_defaults,
    k_vacmAccessEntry_test,
    k_vacmAccessEntry_ready,
#ifdef SR_vacmAccessEntry_UNDO
    vacmAccessEntry_undo,
#else /* SR_vacmAccessEntry_UNDO */
    NULL,
#endif /* SR_vacmAccessEntry_UNDO */
    vacmAccessEntry_ready,
    vacmAccessEntry_set,
    vacmAccessEntry_cleanup,
#ifndef U_vacmAccessEntry
    (SR_COPY_FPTR) NULL
#else /* U_vacmAccessEntry */
    (SR_COPY_FPTR) k_vacmAccessEntryCloneUserpartData
#endif /* U_vacmAccessEntry */
};

/*----------------------------------------------------------------------
 * Free the vacmAccessEntry data object.
 *---------------------------------------------------------------------*/
void
vacmAccessEntry_free(vacmAccessEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 vacmAccessEntryGetInfo.type_table,
                 vacmAccessEntryGetInfo.highest_nominator,
                 vacmAccessEntryGetInfo.valid_offset,
                 vacmAccessEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after vacmAccessEntry set/undo
 *---------------------------------------------------------------------*/
static int
vacmAccessEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &vacmAccessEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the vacmAccessEntry family
 *---------------------------------------------------------------------*/
vacmAccessEntry_t *
Clone_vacmAccessEntry(vacmAccessEntry_t *vacmAccessEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (vacmAccessEntry_t *)SrCloneFamily(vacmAccessEntry,
                         vacmAccessEntryGetInfo.family_size,
                         vacmAccessEntryGetInfo.type_table,
                         vacmAccessEntryGetInfo.highest_nominator,
                         vacmAccessEntryGetInfo.valid_offset,
                         vacmAccessEntryTestInfo.userpart_clone_func,
                         vacmAccessEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
vacmAccessEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                     doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &vacmAccessEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
vacmAccessEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &vacmAccessEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
vacmAccessEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_vacmAccessEntry_set((vacmAccessEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


