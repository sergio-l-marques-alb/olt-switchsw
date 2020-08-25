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

static const SR_UINT32 snmpNotifyEntry_last_sid_array[] = {
    1, /* snmpNotifyName */
    2, /* snmpNotifyTag */
    3, /* snmpNotifyType */
    4, /* snmpNotifyStorageType */
    5  /* snmpNotifyRowStatus */
};

/*
 * The snmpNotifyEntryTypeTable array should be located in the
 * k_notif.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType snmpNotifyEntryTypeTable[];

const SrIndexInfo snmpNotifyEntryIndexInfo[] = {
#ifdef I_snmpNotifyName
    { I_snmpNotifyName, T_octet, -1 },
#endif /* I_snmpNotifyName */
    { -1, -1, -1 }
};

const SrGetInfoEntry snmpNotifyEntryGetInfo = {
    (SR_KGET_FPTR) new_k_snmpNotifyEntry_get,
#ifndef U_snmpNotifyEntry
    (SR_FREE_FPTR) NULL,
#else /* U_snmpNotifyEntry */
    (SR_FREE_FPTR) k_snmpNotifyEntryFreeUserpartData,
#endif /* U_snmpNotifyEntry */
    (int) sizeof(snmpNotifyEntry_t),
    I_snmpNotifyEntry_max,
    (SnmpType *) snmpNotifyEntryTypeTable,
    snmpNotifyEntryIndexInfo,
    (short) offsetof(snmpNotifyEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the snmpNotifyEntry family.
 *---------------------------------------------------------------------*/
VarBind *
snmpNotifyEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                    ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_snmpNotifyName) )
    return NULL;
#else	/* ! ( defined(I_snmpNotifyName) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpNotifyEntryGetInfo));
#endif	/* ! ( defined(I_snmpNotifyName) ) */
}

snmpNotifyEntry_t *
new_k_snmpNotifyEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator, int searchType,
                          snmpNotifyEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_snmpNotifyEntry_get(serialNum, contextInfo, nominator,
                                 searchType, data->snmpNotifyName);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int snmpNotifyEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the snmpNotifyEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  snmpNotifyName_len[] = { { 1, 32 } };
static LengthTest_t  snmpNotifyTag_len[] = { { 0, 255 } };
static RangeTest_t   snmpNotifyType_range[] = { { 1, 2 } };
static RangeTest_t   snmpNotifyStorageType_range[] = { { 1, 5 } };
static RangeTest_t   snmpNotifyRowStatus_range[] = { { 1, 6 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * snmpNotifyEntry family.
 */
static struct sr_member_test snmpNotifyEntry_member_test[] =
{
    /* snmpNotifyName */
    { MINV_LENGTH_TEST, 
      sizeof(snmpNotifyName_len)/sizeof(LengthTest_t), /* 1 */
      snmpNotifyName_len, NULL },

    /* snmpNotifyTag */
    { MINV_LENGTH_TEST, 
      sizeof(snmpNotifyTag_len)/sizeof(LengthTest_t), /* 1 */
      snmpNotifyTag_len, NULL },

    /* snmpNotifyType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpNotifyType_range)/sizeof(RangeTest_t), /* 2 */
      snmpNotifyType_range, NULL },

    /* snmpNotifyStorageType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpNotifyStorageType_range)/sizeof(RangeTest_t), /* 5 */
      snmpNotifyStorageType_range, StorageType_test },

    /* snmpNotifyRowStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpNotifyRowStatus_range)/sizeof(RangeTest_t), /* 6 */
      snmpNotifyRowStatus_range, RowStatus_test }
};


/*
 * Initialize SrTestInfoEntry for the snmpNotifyEntry family.
 */
const SrTestInfoEntry snmpNotifyEntryTestInfo = {
    &snmpNotifyEntryGetInfo,
    snmpNotifyEntry_member_test,
    k_snmpNotifyEntry_set_defaults,
    k_snmpNotifyEntry_test,
    k_snmpNotifyEntry_ready,
#ifdef SR_snmpNotifyEntry_UNDO
    snmpNotifyEntry_undo,
#else /* SR_snmpNotifyEntry_UNDO */
    NULL,
#endif /* SR_snmpNotifyEntry_UNDO */
    snmpNotifyEntry_ready,
    snmpNotifyEntry_set,
    snmpNotifyEntry_cleanup,
#ifndef U_snmpNotifyEntry
    (SR_COPY_FPTR) NULL
#else /* U_snmpNotifyEntry */
    (SR_COPY_FPTR) k_snmpNotifyEntryCloneUserpartData
#endif /* U_snmpNotifyEntry */
};

/*----------------------------------------------------------------------
 * Free the snmpNotifyEntry data object.
 *---------------------------------------------------------------------*/
void
snmpNotifyEntry_free(snmpNotifyEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 snmpNotifyEntryGetInfo.type_table,
                 snmpNotifyEntryGetInfo.highest_nominator,
                 snmpNotifyEntryGetInfo.valid_offset,
                 snmpNotifyEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after snmpNotifyEntry set/undo
 *---------------------------------------------------------------------*/
static int
snmpNotifyEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &snmpNotifyEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the snmpNotifyEntry family
 *---------------------------------------------------------------------*/
snmpNotifyEntry_t *
Clone_snmpNotifyEntry(snmpNotifyEntry_t *snmpNotifyEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (snmpNotifyEntry_t *)SrCloneFamily(snmpNotifyEntry,
                         snmpNotifyEntryGetInfo.family_size,
                         snmpNotifyEntryGetInfo.type_table,
                         snmpNotifyEntryGetInfo.highest_nominator,
                         snmpNotifyEntryGetInfo.valid_offset,
                         snmpNotifyEntryTestInfo.userpart_clone_func,
                         snmpNotifyEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
snmpNotifyEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                     doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &snmpNotifyEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
snmpNotifyEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &snmpNotifyEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snmpNotifyEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_snmpNotifyEntry_set((snmpNotifyEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


static const SR_UINT32 snmpNotifyFilterProfileEntry_last_sid_array[] = {
    1, /* snmpNotifyFilterProfileName */
    2, /* snmpNotifyFilterProfileStorType */
    3  /* snmpNotifyFilterProfileRowStatus */
};

/*
 * The snmpNotifyFilterProfileEntryTypeTable array should be located in the
 * k_notif.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType snmpNotifyFilterProfileEntryTypeTable[];

const SrIndexInfo snmpNotifyFilterProfileEntryIndexInfo[] = {
#ifdef I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName
    { I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName, T_octet, -1 },
#endif /* I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName */
    { -1, -1, -1 }
};

const SrGetInfoEntry snmpNotifyFilterProfileEntryGetInfo = {
    (SR_KGET_FPTR) new_k_snmpNotifyFilterProfileEntry_get,
#ifndef U_snmpNotifyFilterProfileEntry
    (SR_FREE_FPTR) NULL,
#else /* U_snmpNotifyFilterProfileEntry */
    (SR_FREE_FPTR) k_snmpNotifyFilterProfileEntryFreeUserpartData,
#endif /* U_snmpNotifyFilterProfileEntry */
    (int) sizeof(snmpNotifyFilterProfileEntry_t),
    I_snmpNotifyFilterProfileEntry_max,
    (SnmpType *) snmpNotifyFilterProfileEntryTypeTable,
    snmpNotifyFilterProfileEntryIndexInfo,
    (short) offsetof(snmpNotifyFilterProfileEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the snmpNotifyFilterProfileEntry family.
 *---------------------------------------------------------------------*/
VarBind *
snmpNotifyFilterProfileEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                                 ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName) )
    return NULL;
#else	/* ! ( defined(I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpNotifyFilterProfileEntryGetInfo));
#endif	/* ! ( defined(I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName) ) */
}

snmpNotifyFilterProfileEntry_t *
new_k_snmpNotifyFilterProfileEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator, int searchType,
                                       snmpNotifyFilterProfileEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_snmpNotifyFilterProfileEntry_get(serialNum, contextInfo, nominator,
                                              searchType,
                                              data->snmpTargetParamsName);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int snmpNotifyFilterProfileEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the snmpNotifyFilterProfileEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  snmpNotifyFilterProfileName_len[] = { { 1, 32 } };
static RangeTest_t   snmpNotifyFilterProfileStorType_range[] = { { 1, 5 } };
static RangeTest_t   snmpNotifyFilterProfileRowStatus_range[] = { { 1, 6 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * snmpNotifyFilterProfileEntry family.
 */
static struct sr_member_test snmpNotifyFilterProfileEntry_member_test[] =
{
    /* snmpNotifyFilterProfileName */
    { MINV_LENGTH_TEST, 
      sizeof(snmpNotifyFilterProfileName_len)/sizeof(LengthTest_t), /* 1 */
      snmpNotifyFilterProfileName_len, NULL },

    /* snmpNotifyFilterProfileStorType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpNotifyFilterProfileStorType_range)/sizeof(RangeTest_t), /* 5 */
      snmpNotifyFilterProfileStorType_range, StorageType_test },

    /* snmpNotifyFilterProfileRowStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpNotifyFilterProfileRowStatus_range)/sizeof(RangeTest_t), /* 6 */
      snmpNotifyFilterProfileRowStatus_range, RowStatus_test }
};


/*
 * Initialize SrTestInfoEntry for the snmpNotifyFilterProfileEntry family.
 */
const SrTestInfoEntry snmpNotifyFilterProfileEntryTestInfo = {
    &snmpNotifyFilterProfileEntryGetInfo,
    snmpNotifyFilterProfileEntry_member_test,
    k_snmpNotifyFilterProfileEntry_set_defaults,
    k_snmpNotifyFilterProfileEntry_test,
    k_snmpNotifyFilterProfileEntry_ready,
#ifdef SR_snmpNotifyFilterProfileEntry_UNDO
    snmpNotifyFilterProfileEntry_undo,
#else /* SR_snmpNotifyFilterProfileEntry_UNDO */
    NULL,
#endif /* SR_snmpNotifyFilterProfileEntry_UNDO */
    snmpNotifyFilterProfileEntry_ready,
    snmpNotifyFilterProfileEntry_set,
    snmpNotifyFilterProfileEntry_cleanup,
#ifndef U_snmpNotifyFilterProfileEntry
    (SR_COPY_FPTR) NULL
#else /* U_snmpNotifyFilterProfileEntry */
    (SR_COPY_FPTR) k_snmpNotifyFilterProfileEntryCloneUserpartData
#endif /* U_snmpNotifyFilterProfileEntry */
};

/*----------------------------------------------------------------------
 * Free the snmpNotifyFilterProfileEntry data object.
 *---------------------------------------------------------------------*/
void
snmpNotifyFilterProfileEntry_free(snmpNotifyFilterProfileEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 snmpNotifyFilterProfileEntryGetInfo.type_table,
                 snmpNotifyFilterProfileEntryGetInfo.highest_nominator,
                 snmpNotifyFilterProfileEntryGetInfo.valid_offset,
                 snmpNotifyFilterProfileEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after snmpNotifyFilterProfileEntry set/undo
 *---------------------------------------------------------------------*/
static int
snmpNotifyFilterProfileEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &snmpNotifyFilterProfileEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the snmpNotifyFilterProfileEntry family
 *---------------------------------------------------------------------*/
snmpNotifyFilterProfileEntry_t *
Clone_snmpNotifyFilterProfileEntry(snmpNotifyFilterProfileEntry_t *snmpNotifyFilterProfileEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (snmpNotifyFilterProfileEntry_t *)SrCloneFamily(snmpNotifyFilterProfileEntry,
                         snmpNotifyFilterProfileEntryGetInfo.family_size,
                         snmpNotifyFilterProfileEntryGetInfo.type_table,
                         snmpNotifyFilterProfileEntryGetInfo.highest_nominator,
                         snmpNotifyFilterProfileEntryGetInfo.valid_offset,
                         snmpNotifyFilterProfileEntryTestInfo.userpart_clone_func,
                         snmpNotifyFilterProfileEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
snmpNotifyFilterProfileEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &snmpNotifyFilterProfileEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
snmpNotifyFilterProfileEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &snmpNotifyFilterProfileEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snmpNotifyFilterProfileEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_snmpNotifyFilterProfileEntry_set((snmpNotifyFilterProfileEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


static const SR_UINT32 snmpNotifyFilterEntry_last_sid_array[] = {
    1, /* snmpNotifyFilterSubtree */
    2, /* snmpNotifyFilterMask */
    3, /* snmpNotifyFilterType */
    4, /* snmpNotifyFilterStorageType */
    5  /* snmpNotifyFilterRowStatus */
};

/*
 * The snmpNotifyFilterEntryTypeTable array should be located in the
 * k_notif.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType snmpNotifyFilterEntryTypeTable[];

const SrIndexInfo snmpNotifyFilterEntryIndexInfo[] = {
#ifdef I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName
    { I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName, T_var_octet, -1 },
#endif /* I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName */
#ifdef I_snmpNotifyFilterSubtree
    { I_snmpNotifyFilterSubtree, T_oid, -1 },
#endif /* I_snmpNotifyFilterSubtree */
    { -1, -1, -1 }
};

const SrGetInfoEntry snmpNotifyFilterEntryGetInfo = {
    (SR_KGET_FPTR) new_k_snmpNotifyFilterEntry_get,
#ifndef U_snmpNotifyFilterEntry
    (SR_FREE_FPTR) NULL,
#else /* U_snmpNotifyFilterEntry */
    (SR_FREE_FPTR) k_snmpNotifyFilterEntryFreeUserpartData,
#endif /* U_snmpNotifyFilterEntry */
    (int) sizeof(snmpNotifyFilterEntry_t),
    I_snmpNotifyFilterEntry_max,
    (SnmpType *) snmpNotifyFilterEntryTypeTable,
    snmpNotifyFilterEntryIndexInfo,
    (short) offsetof(snmpNotifyFilterEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the snmpNotifyFilterEntry family.
 *---------------------------------------------------------------------*/
VarBind *
snmpNotifyFilterEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                          ContextInfo *contextInfo, int serialNum)
{
#if (! ( defined(I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName) ) || ! ( defined(I_snmpNotifyFilterSubtree) ))
    return NULL;
#else	/* (! ( defined(I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName) ) || ! ( defined(I_snmpNotifyFilterSubtree) )) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpNotifyFilterEntryGetInfo));
#endif	/* (! ( defined(I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName) ) || ! ( defined(I_snmpNotifyFilterSubtree) )) */
}

snmpNotifyFilterEntry_t *
new_k_snmpNotifyFilterEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator, int searchType,
                                snmpNotifyFilterEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_snmpNotifyFilterEntry_get(serialNum, contextInfo, nominator,
                                       searchType,
                                       data->snmpNotifyFilterProfileName,
                                       data->snmpNotifyFilterSubtree);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int snmpNotifyFilterEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the snmpNotifyFilterEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  snmpNotifyFilterMask_len[] = { { 0, 16 } };
static RangeTest_t   snmpNotifyFilterType_range[] = { { 1, 2 } };
static RangeTest_t   snmpNotifyFilterStorageType_range[] = { { 1, 5 } };
static RangeTest_t   snmpNotifyFilterRowStatus_range[] = { { 1, 6 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * snmpNotifyFilterEntry family.
 */
static struct sr_member_test snmpNotifyFilterEntry_member_test[] =
{
    /* snmpNotifyFilterSubtree */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* snmpNotifyFilterMask */
    { MINV_LENGTH_TEST, 
      sizeof(snmpNotifyFilterMask_len)/sizeof(LengthTest_t), /* 1 */
      snmpNotifyFilterMask_len, NULL },

    /* snmpNotifyFilterType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpNotifyFilterType_range)/sizeof(RangeTest_t), /* 2 */
      snmpNotifyFilterType_range, NULL },

    /* snmpNotifyFilterStorageType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpNotifyFilterStorageType_range)/sizeof(RangeTest_t), /* 5 */
      snmpNotifyFilterStorageType_range, StorageType_test },

    /* snmpNotifyFilterRowStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpNotifyFilterRowStatus_range)/sizeof(RangeTest_t), /* 6 */
      snmpNotifyFilterRowStatus_range, RowStatus_test }
};


/*
 * Initialize SrTestInfoEntry for the snmpNotifyFilterEntry family.
 */
const SrTestInfoEntry snmpNotifyFilterEntryTestInfo = {
    &snmpNotifyFilterEntryGetInfo,
    snmpNotifyFilterEntry_member_test,
    k_snmpNotifyFilterEntry_set_defaults,
    k_snmpNotifyFilterEntry_test,
    k_snmpNotifyFilterEntry_ready,
#ifdef SR_snmpNotifyFilterEntry_UNDO
    snmpNotifyFilterEntry_undo,
#else /* SR_snmpNotifyFilterEntry_UNDO */
    NULL,
#endif /* SR_snmpNotifyFilterEntry_UNDO */
    snmpNotifyFilterEntry_ready,
    snmpNotifyFilterEntry_set,
    snmpNotifyFilterEntry_cleanup,
#ifndef U_snmpNotifyFilterEntry
    (SR_COPY_FPTR) NULL
#else /* U_snmpNotifyFilterEntry */
    (SR_COPY_FPTR) k_snmpNotifyFilterEntryCloneUserpartData
#endif /* U_snmpNotifyFilterEntry */
};

/*----------------------------------------------------------------------
 * Free the snmpNotifyFilterEntry data object.
 *---------------------------------------------------------------------*/
void
snmpNotifyFilterEntry_free(snmpNotifyFilterEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 snmpNotifyFilterEntryGetInfo.type_table,
                 snmpNotifyFilterEntryGetInfo.highest_nominator,
                 snmpNotifyFilterEntryGetInfo.valid_offset,
                 snmpNotifyFilterEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after snmpNotifyFilterEntry set/undo
 *---------------------------------------------------------------------*/
static int
snmpNotifyFilterEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &snmpNotifyFilterEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the snmpNotifyFilterEntry family
 *---------------------------------------------------------------------*/
snmpNotifyFilterEntry_t *
Clone_snmpNotifyFilterEntry(snmpNotifyFilterEntry_t *snmpNotifyFilterEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (snmpNotifyFilterEntry_t *)SrCloneFamily(snmpNotifyFilterEntry,
                         snmpNotifyFilterEntryGetInfo.family_size,
                         snmpNotifyFilterEntryGetInfo.type_table,
                         snmpNotifyFilterEntryGetInfo.highest_nominator,
                         snmpNotifyFilterEntryGetInfo.valid_offset,
                         snmpNotifyFilterEntryTestInfo.userpart_clone_func,
                         snmpNotifyFilterEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
snmpNotifyFilterEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                           doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &snmpNotifyFilterEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
snmpNotifyFilterEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &snmpNotifyFilterEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snmpNotifyFilterEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_snmpNotifyFilterEntry_set((snmpNotifyFilterEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


