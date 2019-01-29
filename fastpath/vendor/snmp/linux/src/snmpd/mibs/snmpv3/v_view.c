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

static const SR_UINT32 vacmMIBViews_last_sid_array[] = {
    1  /* vacmViewSpinLock */
};

const SnmpType vacmMIBViewsTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(vacmMIBViews_t, vacmViewSpinLock), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry vacmMIBViewsGetInfo = {
    (SR_KGET_FPTR) new_k_vacmMIBViews_get,
#ifndef U_vacmMIBViews
    (SR_FREE_FPTR) NULL,
#else /* U_vacmMIBViews */
    (SR_FREE_FPTR) k_vacmMIBViewsFreeUserpartData,
#endif /* U_vacmMIBViews */
    (int) sizeof(vacmMIBViews_t),
    I_vacmMIBViews_max,
    (SnmpType *) vacmMIBViewsTypeTable,
    NULL,
    (short) offsetof(vacmMIBViews_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the vacmMIBViews family.
 *---------------------------------------------------------------------*/
VarBind *
vacmMIBViews_get(OID *incoming, ObjectInfo *object, int searchType,
                 ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &vacmMIBViewsGetInfo));
}

vacmMIBViews_t *
new_k_vacmMIBViews_get(int serialNum, ContextInfo *contextInfo,
                       int nominator, int searchType,
                       vacmMIBViews_t *data)
{
    return k_vacmMIBViews_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int vacmMIBViews_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the vacmMIBViews family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   vacmViewSpinLock_range[] = { { 0, 2147483647 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * vacmMIBViews family.
 */
static struct sr_member_test vacmMIBViews_member_test[] =
{
    /* vacmViewSpinLock */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmViewSpinLock_range)/sizeof(RangeTest_t), /* 1 */
      vacmViewSpinLock_range, TestAndIncr_test }
};


/*
 * Initialize SrTestInfoEntry for the vacmMIBViews family.
 */
const SrTestInfoEntry vacmMIBViewsTestInfo = {
    &vacmMIBViewsGetInfo,
    vacmMIBViews_member_test,
    NULL,
    k_vacmMIBViews_test,
    k_vacmMIBViews_ready,
#ifdef SR_vacmMIBViews_UNDO
    vacmMIBViews_undo,
#else /* SR_vacmMIBViews_UNDO */
    NULL,
#endif /* SR_vacmMIBViews_UNDO */
    vacmMIBViews_ready,
    vacmMIBViews_set,
    vacmMIBViews_cleanup,
#ifndef U_vacmMIBViews
    (SR_COPY_FPTR) NULL
#else /* U_vacmMIBViews */
    (SR_COPY_FPTR) k_vacmMIBViewsCloneUserpartData
#endif /* U_vacmMIBViews */
};

/*----------------------------------------------------------------------
 * cleanup after vacmMIBViews set/undo
 *---------------------------------------------------------------------*/
static int
vacmMIBViews_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &vacmMIBViewsTestInfo);
}

#ifdef SR_vacmMIBViews_UNDO
/*----------------------------------------------------------------------
 * clone the vacmMIBViews family
 *---------------------------------------------------------------------*/
vacmMIBViews_t *
Clone_vacmMIBViews(vacmMIBViews_t *vacmMIBViews)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (vacmMIBViews_t *)SrCloneFamily(vacmMIBViews,
                         vacmMIBViewsGetInfo.family_size,
                         vacmMIBViewsGetInfo.type_table,
                         vacmMIBViewsGetInfo.highest_nominator,
                         vacmMIBViewsGetInfo.valid_offset,
                         vacmMIBViewsTestInfo.userpart_clone_func,
                         vacmMIBViewsGetInfo.userpart_free_func);
}

#endif /* defined(SR_vacmMIBViews_UNDO) */
/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
vacmMIBViews_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                  doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &vacmMIBViewsTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
vacmMIBViews_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &vacmMIBViewsTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
vacmMIBViews_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_vacmMIBViews_set((vacmMIBViews_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


static const SR_UINT32 vacmViewTreeFamilyEntry_last_sid_array[] = {
    1, /* vacmViewTreeFamilyViewName */
    2, /* vacmViewTreeFamilySubtree */
    3, /* vacmViewTreeFamilyMask */
    4, /* vacmViewTreeFamilyType */
    5, /* vacmViewTreeFamilyStorageType */
    6  /* vacmViewTreeFamilyStatus */
};

/*
 * The vacmViewTreeFamilyEntryTypeTable array should be located in the
 * k_view.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType vacmViewTreeFamilyEntryTypeTable[];

const SrIndexInfo vacmViewTreeFamilyEntryIndexInfo[] = {
#ifdef I_vacmViewTreeFamilyViewName
    { I_vacmViewTreeFamilyViewName, T_var_octet, -1 },
#endif /* I_vacmViewTreeFamilyViewName */
#ifdef I_vacmViewTreeFamilySubtree
    { I_vacmViewTreeFamilySubtree, T_var_oid, -1 },
#endif /* I_vacmViewTreeFamilySubtree */
    { -1, -1, -1 }
};

const SrGetInfoEntry vacmViewTreeFamilyEntryGetInfo = {
    (SR_KGET_FPTR) new_k_vacmViewTreeFamilyEntry_get,
#ifndef U_vacmViewTreeFamilyEntry
    (SR_FREE_FPTR) NULL,
#else /* U_vacmViewTreeFamilyEntry */
    (SR_FREE_FPTR) k_vacmViewTreeFamilyEntryFreeUserpartData,
#endif /* U_vacmViewTreeFamilyEntry */
    (int) sizeof(vacmViewTreeFamilyEntry_t),
    I_vacmViewTreeFamilyEntry_max,
    (SnmpType *) vacmViewTreeFamilyEntryTypeTable,
    vacmViewTreeFamilyEntryIndexInfo,
    (short) offsetof(vacmViewTreeFamilyEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the vacmViewTreeFamilyEntry family.
 *---------------------------------------------------------------------*/
VarBind *
vacmViewTreeFamilyEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                            ContextInfo *contextInfo, int serialNum)
{
#if (! ( defined(I_vacmViewTreeFamilyViewName) ) || ! ( defined(I_vacmViewTreeFamilySubtree) ))
    return NULL;
#else	/* (! ( defined(I_vacmViewTreeFamilyViewName) ) || ! ( defined(I_vacmViewTreeFamilySubtree) )) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &vacmViewTreeFamilyEntryGetInfo));
#endif	/* (! ( defined(I_vacmViewTreeFamilyViewName) ) || ! ( defined(I_vacmViewTreeFamilySubtree) )) */
}

vacmViewTreeFamilyEntry_t *
new_k_vacmViewTreeFamilyEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator, int searchType,
                                  vacmViewTreeFamilyEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_vacmViewTreeFamilyEntry_get(serialNum, contextInfo, nominator,
                                         searchType,
                                         data->vacmViewTreeFamilyViewName,
                                         data->vacmViewTreeFamilySubtree);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int vacmViewTreeFamilyEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the vacmViewTreeFamilyEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  vacmViewTreeFamilyViewName_len[] = { { 1, 32 } };
static LengthTest_t  vacmViewTreeFamilyMask_len[] = { { 0, 16 } };
static RangeTest_t   vacmViewTreeFamilyType_range[] = { { 1, 2 } };
static RangeTest_t   vacmViewTreeFamilyStorageType_range[] = { { 1, 5 } };
static RangeTest_t   vacmViewTreeFamilyStatus_range[] = { { 1, 6 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * vacmViewTreeFamilyEntry family.
 */
static struct sr_member_test vacmViewTreeFamilyEntry_member_test[] =
{
    /* vacmViewTreeFamilyViewName */
    { MINV_LENGTH_TEST, 
      sizeof(vacmViewTreeFamilyViewName_len)/sizeof(LengthTest_t), /* 1 */
      vacmViewTreeFamilyViewName_len, NULL },

    /* vacmViewTreeFamilySubtree */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* vacmViewTreeFamilyMask */
    { MINV_LENGTH_TEST, 
      sizeof(vacmViewTreeFamilyMask_len)/sizeof(LengthTest_t), /* 1 */
      vacmViewTreeFamilyMask_len, NULL },

    /* vacmViewTreeFamilyType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmViewTreeFamilyType_range)/sizeof(RangeTest_t), /* 2 */
      vacmViewTreeFamilyType_range, NULL },

    /* vacmViewTreeFamilyStorageType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmViewTreeFamilyStorageType_range)/sizeof(RangeTest_t), /* 5 */
      vacmViewTreeFamilyStorageType_range, StorageType_test },

    /* vacmViewTreeFamilyStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(vacmViewTreeFamilyStatus_range)/sizeof(RangeTest_t), /* 6 */
      vacmViewTreeFamilyStatus_range, RowStatus_test }
};


/*
 * Initialize SrTestInfoEntry for the vacmViewTreeFamilyEntry family.
 */
const SrTestInfoEntry vacmViewTreeFamilyEntryTestInfo = {
    &vacmViewTreeFamilyEntryGetInfo,
    vacmViewTreeFamilyEntry_member_test,
    k_vacmViewTreeFamilyEntry_set_defaults,
    k_vacmViewTreeFamilyEntry_test,
    k_vacmViewTreeFamilyEntry_ready,
#ifdef SR_vacmViewTreeFamilyEntry_UNDO
    vacmViewTreeFamilyEntry_undo,
#else /* SR_vacmViewTreeFamilyEntry_UNDO */
    NULL,
#endif /* SR_vacmViewTreeFamilyEntry_UNDO */
    vacmViewTreeFamilyEntry_ready,
    vacmViewTreeFamilyEntry_set,
    vacmViewTreeFamilyEntry_cleanup,
#ifndef U_vacmViewTreeFamilyEntry
    (SR_COPY_FPTR) NULL
#else /* U_vacmViewTreeFamilyEntry */
    (SR_COPY_FPTR) k_vacmViewTreeFamilyEntryCloneUserpartData
#endif /* U_vacmViewTreeFamilyEntry */
};

/*----------------------------------------------------------------------
 * Free the vacmViewTreeFamilyEntry data object.
 *---------------------------------------------------------------------*/
void
vacmViewTreeFamilyEntry_free(vacmViewTreeFamilyEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 vacmViewTreeFamilyEntryGetInfo.type_table,
                 vacmViewTreeFamilyEntryGetInfo.highest_nominator,
                 vacmViewTreeFamilyEntryGetInfo.valid_offset,
                 vacmViewTreeFamilyEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after vacmViewTreeFamilyEntry set/undo
 *---------------------------------------------------------------------*/
static int
vacmViewTreeFamilyEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &vacmViewTreeFamilyEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the vacmViewTreeFamilyEntry family
 *---------------------------------------------------------------------*/
vacmViewTreeFamilyEntry_t *
Clone_vacmViewTreeFamilyEntry(vacmViewTreeFamilyEntry_t *vacmViewTreeFamilyEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (vacmViewTreeFamilyEntry_t *)SrCloneFamily(vacmViewTreeFamilyEntry,
                         vacmViewTreeFamilyEntryGetInfo.family_size,
                         vacmViewTreeFamilyEntryGetInfo.type_table,
                         vacmViewTreeFamilyEntryGetInfo.highest_nominator,
                         vacmViewTreeFamilyEntryGetInfo.valid_offset,
                         vacmViewTreeFamilyEntryTestInfo.userpart_clone_func,
                         vacmViewTreeFamilyEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
vacmViewTreeFamilyEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &vacmViewTreeFamilyEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
vacmViewTreeFamilyEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &vacmViewTreeFamilyEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
vacmViewTreeFamilyEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_vacmViewTreeFamilyEntry_set((vacmViewTreeFamilyEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


