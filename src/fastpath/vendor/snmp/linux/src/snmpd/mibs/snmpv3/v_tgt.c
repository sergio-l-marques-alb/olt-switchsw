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

static const SR_UINT32 snmpTargetObjects_last_sid_array[] = {
    1, /* snmpTargetSpinLock */
    4, /* snmpUnavailableContexts */
    5  /* snmpUnknownContexts */
};

const SnmpType snmpTargetObjectsTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(snmpTargetObjects_t, snmpTargetSpinLock), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmpTargetObjects_t, snmpUnavailableContexts), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmpTargetObjects_t, snmpUnknownContexts), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry snmpTargetObjectsGetInfo = {
    (SR_KGET_FPTR) new_k_snmpTargetObjects_get,
#ifndef U_snmpTargetObjects
    (SR_FREE_FPTR) NULL,
#else /* U_snmpTargetObjects */
    (SR_FREE_FPTR) k_snmpTargetObjectsFreeUserpartData,
#endif /* U_snmpTargetObjects */
    (int) sizeof(snmpTargetObjects_t),
    I_snmpTargetObjects_max,
    (SnmpType *) snmpTargetObjectsTypeTable,
    NULL,
    (short) offsetof(snmpTargetObjects_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the snmpTargetObjects family.
 *---------------------------------------------------------------------*/
VarBind *
snmpTargetObjects_get(OID *incoming, ObjectInfo *object, int searchType,
                      ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpTargetObjectsGetInfo));
}

snmpTargetObjects_t *
new_k_snmpTargetObjects_get(int serialNum, ContextInfo *contextInfo,
                            int nominator, int searchType,
                            snmpTargetObjects_t *data)
{
    return k_snmpTargetObjects_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int snmpTargetObjects_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the snmpTargetObjects family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   snmpTargetSpinLock_range[] = { { 0, 2147483647 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * snmpTargetObjects family.
 */
static struct sr_member_test snmpTargetObjects_member_test[] =
{
    /* snmpTargetSpinLock */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetSpinLock_range)/sizeof(RangeTest_t), /* 1 */
      snmpTargetSpinLock_range, TestAndIncr_test },

    /* snmpUnavailableContexts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpUnknownContexts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};


/*
 * Initialize SrTestInfoEntry for the snmpTargetObjects family.
 */
const SrTestInfoEntry snmpTargetObjectsTestInfo = {
    &snmpTargetObjectsGetInfo,
    snmpTargetObjects_member_test,
    NULL,
    k_snmpTargetObjects_test,
    k_snmpTargetObjects_ready,
#ifdef SR_snmpTargetObjects_UNDO
    snmpTargetObjects_undo,
#else /* SR_snmpTargetObjects_UNDO */
    NULL,
#endif /* SR_snmpTargetObjects_UNDO */
    snmpTargetObjects_ready,
    snmpTargetObjects_set,
    snmpTargetObjects_cleanup,
#ifndef U_snmpTargetObjects
    (SR_COPY_FPTR) NULL
#else /* U_snmpTargetObjects */
    (SR_COPY_FPTR) k_snmpTargetObjectsCloneUserpartData
#endif /* U_snmpTargetObjects */
};

/*----------------------------------------------------------------------
 * cleanup after snmpTargetObjects set/undo
 *---------------------------------------------------------------------*/
static int
snmpTargetObjects_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &snmpTargetObjectsTestInfo);
}

#ifdef SR_snmpTargetObjects_UNDO
/*----------------------------------------------------------------------
 * clone the snmpTargetObjects family
 *---------------------------------------------------------------------*/
snmpTargetObjects_t *
Clone_snmpTargetObjects(snmpTargetObjects_t *snmpTargetObjects)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (snmpTargetObjects_t *)SrCloneFamily(snmpTargetObjects,
                         snmpTargetObjectsGetInfo.family_size,
                         snmpTargetObjectsGetInfo.type_table,
                         snmpTargetObjectsGetInfo.highest_nominator,
                         snmpTargetObjectsGetInfo.valid_offset,
                         snmpTargetObjectsTestInfo.userpart_clone_func,
                         snmpTargetObjectsGetInfo.userpart_free_func);
}

#endif /* defined(SR_snmpTargetObjects_UNDO) */
/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
snmpTargetObjects_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                       doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &snmpTargetObjectsTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
snmpTargetObjects_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &snmpTargetObjectsTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snmpTargetObjects_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_snmpTargetObjects_set((snmpTargetObjects_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


static const SR_UINT32 snmpTargetAddrEntry_last_sid_array[] = {
    1, /* snmpTargetAddrName */
    2, /* snmpTargetAddrTDomain */
    3, /* snmpTargetAddrTAddress */
    4, /* snmpTargetAddrTimeout */
    5, /* snmpTargetAddrRetryCount */
    6, /* snmpTargetAddrTagList */
    7, /* snmpTargetAddrParams */
    8, /* snmpTargetAddrStorageType */
    9, /* snmpTargetAddrRowStatus */
    1, /* snmpTargetAddrTMask (from snmpTargetAddrExtEntry) */
    2, /* snmpTargetAddrMMS (from snmpTargetAddrExtEntry) */
    0
};

/*
 * The snmpTargetAddrEntryTypeTable array should be located in the
 * k_tgt.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType snmpTargetAddrEntryTypeTable[];

const SrIndexInfo snmpTargetAddrEntryIndexInfo[] = {
#ifdef I_snmpTargetAddrName
    { I_snmpTargetAddrName, T_octet, -1 },
#endif /* I_snmpTargetAddrName */
    { -1, -1, -1 }
};

const SrGetInfoEntry snmpTargetAddrEntryGetInfo = {
    (SR_KGET_FPTR) new_k_snmpTargetAddrEntry_get,
#ifndef U_snmpTargetAddrEntry
    (SR_FREE_FPTR) NULL,
#else /* U_snmpTargetAddrEntry */
    (SR_FREE_FPTR) k_snmpTargetAddrEntryFreeUserpartData,
#endif /* U_snmpTargetAddrEntry */
    (int) sizeof(snmpTargetAddrEntry_t),
    I_snmpTargetAddrEntry_max,
    (SnmpType *) snmpTargetAddrEntryTypeTable,
    snmpTargetAddrEntryIndexInfo,
    (short) offsetof(snmpTargetAddrEntry_t, valid)
};

const SrGetInfoEntry snmpTargetAddrExtEntryGetInfo = {
    (SR_KGET_FPTR) new_k_snmpTargetAddrEntry_get,
#ifndef U_snmpTargetAddrEntry
    (SR_FREE_FPTR) NULL,
#else /* U_snmpTargetAddrEntry */
    (SR_FREE_FPTR) k_snmpTargetAddrEntryFreeUserpartData,
#endif /* U_snmpTargetAddrEntry */
    (int) sizeof(snmpTargetAddrEntry_t),
    I_snmpTargetAddrEntry_max,
    (SnmpType *) snmpTargetAddrEntryTypeTable,
    snmpTargetAddrEntryIndexInfo,
    (short) offsetof(snmpTargetAddrEntry_t, valid)
};




/*---------------------------------------------------------------------
 * Retrieve data from the snmpTargetAddrEntry family.
 *---------------------------------------------------------------------*/
VarBind *
snmpTargetAddrEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                        ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_snmpTargetAddrName) )
    return NULL;
#else	/* ! ( defined(I_snmpTargetAddrName) ) */

    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpTargetAddrEntryGetInfo));

#endif	/* ! ( defined(I_snmpTargetAddrName) ) */
}

snmpTargetAddrEntry_t *
new_k_snmpTargetAddrEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator, int searchType,
                              snmpTargetAddrEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_snmpTargetAddrEntry_get(serialNum, contextInfo, nominator,
                                     searchType, data->snmpTargetAddrName);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int snmpTargetAddrEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the snmpTargetAddrEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  snmpTargetAddrName_len[] = { { 1, 32 } };
static LengthTest_t  snmpTargetAddrTAddress_len[] = { { 1, 255 } };
static RangeTest_t   snmpTargetAddrTimeout_range[] = { { 0, 2147483647 } };
static RangeTest_t   snmpTargetAddrRetryCount_range[] = { { 0, 255 } };
static LengthTest_t  snmpTargetAddrTagList_len[] = { { 0, 255 } };
static LengthTest_t  snmpTargetAddrParams_len[] = { { 1, 32 } };
static RangeTest_t   snmpTargetAddrStorageType_range[] = { { 1, 5 } };
static RangeTest_t   snmpTargetAddrRowStatus_range[] = { { 1, 6 } };
static LengthTest_t  snmpTargetAddrTMask_len[] = { { 0, 255 } };
static RangeTest_t   snmpTargetAddrMMS_range[] = { { 0, 2147483647 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * snmpTargetAddrEntry family.
 */
static struct sr_member_test snmpTargetAddrEntry_member_test[] =
{
    /* snmpTargetAddrName */
    { MINV_LENGTH_TEST, 
      sizeof(snmpTargetAddrName_len)/sizeof(LengthTest_t), /* 1 */
      snmpTargetAddrName_len, NULL },

    /* snmpTargetAddrTDomain */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* snmpTargetAddrTAddress */
    { MINV_LENGTH_TEST, 
      sizeof(snmpTargetAddrTAddress_len)/sizeof(LengthTest_t), /* 1 */
      snmpTargetAddrTAddress_len, NULL },

    /* snmpTargetAddrTimeout */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetAddrTimeout_range)/sizeof(RangeTest_t), /* 1 */
      snmpTargetAddrTimeout_range, NULL },

    /* snmpTargetAddrRetryCount */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetAddrRetryCount_range)/sizeof(RangeTest_t), /* 1 */
      snmpTargetAddrRetryCount_range, NULL },

    /* snmpTargetAddrTagList */
    { MINV_LENGTH_TEST, 
      sizeof(snmpTargetAddrTagList_len)/sizeof(LengthTest_t), /* 1 */
      snmpTargetAddrTagList_len, NULL },

    /* snmpTargetAddrParams */
    { MINV_LENGTH_TEST, 
      sizeof(snmpTargetAddrParams_len)/sizeof(LengthTest_t), /* 1 */
      snmpTargetAddrParams_len, NULL },

    /* snmpTargetAddrStorageType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetAddrStorageType_range)/sizeof(RangeTest_t), /* 5 */
      snmpTargetAddrStorageType_range, StorageType_test },

    /* snmpTargetAddrRowStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetAddrRowStatus_range)/sizeof(RangeTest_t), /* 6 */
      snmpTargetAddrRowStatus_range, RowStatus_test },

    /* snmpTargetAddrTMask */
    { MINV_LENGTH_TEST, 
      sizeof(snmpTargetAddrTMask_len)/sizeof(LengthTest_t), /* 1 */
      snmpTargetAddrTMask_len, NULL },

    /* snmpTargetAddrMMS */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetAddrMMS_range)/sizeof(RangeTest_t), /* 1 */
      snmpTargetAddrMMS_range, NULL }
};


/*
 * Initialize SrTestInfoEntry for the snmpTargetAddrEntry family.
 */
const SrTestInfoEntry snmpTargetAddrEntryTestInfo = {
    &snmpTargetAddrEntryGetInfo,
    snmpTargetAddrEntry_member_test,
    k_snmpTargetAddrEntry_set_defaults,
    k_snmpTargetAddrEntry_test,
    k_snmpTargetAddrEntry_ready,
#ifdef SR_snmpTargetAddrEntry_UNDO
    snmpTargetAddrEntry_undo,
#else /* SR_snmpTargetAddrEntry_UNDO */
    NULL,
#endif /* SR_snmpTargetAddrEntry_UNDO */
    snmpTargetAddrEntry_ready,
    snmpTargetAddrEntry_set,
    snmpTargetAddrEntry_cleanup,
#ifndef U_snmpTargetAddrEntry
    (SR_COPY_FPTR) NULL
#else /* U_snmpTargetAddrEntry */
    (SR_COPY_FPTR) k_snmpTargetAddrEntryCloneUserpartData
#endif /* U_snmpTargetAddrEntry */
};


/*
 * Initialize SrTestInfoEntry for the snmpTargetAddrExtEntry family
 * which augments snmpTargetAddrEntry.
 */
const SrTestInfoEntry snmpTargetAddrExtEntryTestInfo = {
    &snmpTargetAddrExtEntryGetInfo,
    snmpTargetAddrEntry_member_test,
    k_snmpTargetAddrEntry_set_defaults,
    k_snmpTargetAddrEntry_test,
    k_snmpTargetAddrEntry_ready,
#ifdef SR_snmpTargetAddrEntry_UNDO
    snmpTargetAddrEntry_undo,
#else /* SR_snmpTargetAddrEntry_UNDO */
    NULL,
#endif /* SR_snmpTargetAddrEntry_UNDO */
    snmpTargetAddrEntry_ready,
    snmpTargetAddrEntry_set,
    snmpTargetAddrEntry_cleanup,
#ifndef U_snmpTargetAddrEntry
    (SR_COPY_FPTR) NULL
#else /* U_snmpTargetAddrEntry */
    (SR_COPY_FPTR) k_snmpTargetAddrEntryCloneUserpartData
#endif /* U_snmpTargetAddrEntry */
};

/*----------------------------------------------------------------------
 * Free the snmpTargetAddrEntry data object.
 *---------------------------------------------------------------------*/
void
snmpTargetAddrEntry_free(snmpTargetAddrEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 snmpTargetAddrEntryGetInfo.type_table,
                 snmpTargetAddrEntryGetInfo.highest_nominator,
                 snmpTargetAddrEntryGetInfo.valid_offset,
                 snmpTargetAddrEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after snmpTargetAddrEntry set/undo
 *---------------------------------------------------------------------*/
static int
snmpTargetAddrEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &snmpTargetAddrEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the snmpTargetAddrEntry family
 *---------------------------------------------------------------------*/
snmpTargetAddrEntry_t *
Clone_snmpTargetAddrEntry(snmpTargetAddrEntry_t *snmpTargetAddrEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (snmpTargetAddrEntry_t *)SrCloneFamily(snmpTargetAddrEntry,
                         snmpTargetAddrEntryGetInfo.family_size,
                         snmpTargetAddrEntryGetInfo.type_table,
                         snmpTargetAddrEntryGetInfo.highest_nominator,
                         snmpTargetAddrEntryGetInfo.valid_offset,
                         snmpTargetAddrEntryTestInfo.userpart_clone_func,
                         snmpTargetAddrEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
snmpTargetAddrEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &snmpTargetAddrEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
snmpTargetAddrEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &snmpTargetAddrEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snmpTargetAddrEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_snmpTargetAddrEntry_set((snmpTargetAddrEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


static const SR_UINT32 snmpTargetParamsEntry_last_sid_array[] = {
    1, /* snmpTargetParamsName */
    2, /* snmpTargetParamsMPModel */
    3, /* snmpTargetParamsSecurityModel */
    4, /* snmpTargetParamsSecurityName */
    5, /* snmpTargetParamsSecurityLevel */
    6, /* snmpTargetParamsStorageType */
    7, /* snmpTargetParamsRowStatus */
    1, /* apoTargetVersion (from apoTargetParamsEntry) */
    0
};

/*
 * The snmpTargetParamsEntryTypeTable array should be located in the
 * k_tgt.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType snmpTargetParamsEntryTypeTable[];

const SrIndexInfo snmpTargetParamsEntryIndexInfo[] = {
#ifdef I_snmpTargetParamsName
    { I_snmpTargetParamsName, T_octet, -1 },
#endif /* I_snmpTargetParamsName */
    { -1, -1, -1 }
};

const SrGetInfoEntry snmpTargetParamsEntryGetInfo = {
    (SR_KGET_FPTR) new_k_snmpTargetParamsEntry_get,
#ifndef U_snmpTargetParamsEntry
    (SR_FREE_FPTR) NULL,
#else /* U_snmpTargetParamsEntry */
    (SR_FREE_FPTR) k_snmpTargetParamsEntryFreeUserpartData,
#endif /* U_snmpTargetParamsEntry */
    (int) sizeof(snmpTargetParamsEntry_t),
    I_snmpTargetParamsEntry_max,
    (SnmpType *) snmpTargetParamsEntryTypeTable,
    snmpTargetParamsEntryIndexInfo,
    (short) offsetof(snmpTargetParamsEntry_t, valid)
};

const SrGetInfoEntry apoTargetParamsEntryGetInfo = {
    (SR_KGET_FPTR) new_k_snmpTargetParamsEntry_get,
#ifndef U_snmpTargetParamsEntry
    (SR_FREE_FPTR) NULL,
#else /* U_snmpTargetParamsEntry */
    (SR_FREE_FPTR) k_snmpTargetParamsEntryFreeUserpartData,
#endif /* U_snmpTargetParamsEntry */
    (int) sizeof(snmpTargetParamsEntry_t),
    I_snmpTargetParamsEntry_max,
    (SnmpType *) snmpTargetParamsEntryTypeTable,
    snmpTargetParamsEntryIndexInfo,
    (short) offsetof(snmpTargetParamsEntry_t, valid)
};




/*---------------------------------------------------------------------
 * Retrieve data from the snmpTargetParamsEntry family.
 *---------------------------------------------------------------------*/
VarBind *
snmpTargetParamsEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                          ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_snmpTargetParamsName) )
    return NULL;
#else	/* ! ( defined(I_snmpTargetParamsName) ) */

    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpTargetParamsEntryGetInfo));

#endif	/* ! ( defined(I_snmpTargetParamsName) ) */
}

snmpTargetParamsEntry_t *
new_k_snmpTargetParamsEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator, int searchType,
                                snmpTargetParamsEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_snmpTargetParamsEntry_get(serialNum, contextInfo, nominator,
                                       searchType, data->snmpTargetParamsName);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int snmpTargetParamsEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the snmpTargetParamsEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  snmpTargetParamsName_len[] = { { 1, 32 } };
static RangeTest_t   snmpTargetParamsMPModel_range[] = { { 0, 2147483647 } };
static RangeTest_t   snmpTargetParamsSecurityModel_range[] = { { 1, 2147483647 } };
static LengthTest_t  snmpTargetParamsSecurityName_len[] = { { 0, 255 } };
static RangeTest_t   snmpTargetParamsSecurityLevel_range[] = { { 1, 3 } };
static RangeTest_t   snmpTargetParamsStorageType_range[] = { { 1, 5 } };
static RangeTest_t   snmpTargetParamsRowStatus_range[] = { { 1, 6 } };
static RangeTest_t   apoTargetVersion_range[] = { { 0, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * snmpTargetParamsEntry family.
 */
static struct sr_member_test snmpTargetParamsEntry_member_test[] =
{
    /* snmpTargetParamsName */
    { MINV_LENGTH_TEST, 
      sizeof(snmpTargetParamsName_len)/sizeof(LengthTest_t), /* 1 */
      snmpTargetParamsName_len, NULL },

    /* snmpTargetParamsMPModel */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetParamsMPModel_range)/sizeof(RangeTest_t), /* 1 */
      snmpTargetParamsMPModel_range, NULL },

    /* snmpTargetParamsSecurityModel */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetParamsSecurityModel_range)/sizeof(RangeTest_t), /* 1 */
      snmpTargetParamsSecurityModel_range, NULL },

    /* snmpTargetParamsSecurityName */
    { MINV_LENGTH_TEST, 
      sizeof(snmpTargetParamsSecurityName_len)/sizeof(LengthTest_t), /* 1 */
      snmpTargetParamsSecurityName_len, NULL },

    /* snmpTargetParamsSecurityLevel */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetParamsSecurityLevel_range)/sizeof(RangeTest_t), /* 3 */
      snmpTargetParamsSecurityLevel_range, NULL },

    /* snmpTargetParamsStorageType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetParamsStorageType_range)/sizeof(RangeTest_t), /* 5 */
      snmpTargetParamsStorageType_range, StorageType_test },

    /* snmpTargetParamsRowStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpTargetParamsRowStatus_range)/sizeof(RangeTest_t), /* 6 */
      snmpTargetParamsRowStatus_range, RowStatus_test },

    /* apoTargetVersion */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(apoTargetVersion_range)/sizeof(RangeTest_t), /* 3 */
      apoTargetVersion_range, NULL }
};


/*
 * Initialize SrTestInfoEntry for the snmpTargetParamsEntry family.
 */
const SrTestInfoEntry snmpTargetParamsEntryTestInfo = {
    &snmpTargetParamsEntryGetInfo,
    snmpTargetParamsEntry_member_test,
    k_snmpTargetParamsEntry_set_defaults,
    k_snmpTargetParamsEntry_test,
    k_snmpTargetParamsEntry_ready,
#ifdef SR_snmpTargetParamsEntry_UNDO
    snmpTargetParamsEntry_undo,
#else /* SR_snmpTargetParamsEntry_UNDO */
    NULL,
#endif /* SR_snmpTargetParamsEntry_UNDO */
    snmpTargetParamsEntry_ready,
    snmpTargetParamsEntry_set,
    snmpTargetParamsEntry_cleanup,
#ifndef U_snmpTargetParamsEntry
    (SR_COPY_FPTR) NULL
#else /* U_snmpTargetParamsEntry */
    (SR_COPY_FPTR) k_snmpTargetParamsEntryCloneUserpartData
#endif /* U_snmpTargetParamsEntry */
};


/*
 * Initialize SrTestInfoEntry for the apoTargetParamsEntry family
 * which augments snmpTargetParamsEntry.
 */
const SrTestInfoEntry apoTargetParamsEntryTestInfo = {
    &apoTargetParamsEntryGetInfo,
    snmpTargetParamsEntry_member_test,
    k_snmpTargetParamsEntry_set_defaults,
    k_snmpTargetParamsEntry_test,
    k_snmpTargetParamsEntry_ready,
#ifdef SR_snmpTargetParamsEntry_UNDO
    snmpTargetParamsEntry_undo,
#else /* SR_snmpTargetParamsEntry_UNDO */
    NULL,
#endif /* SR_snmpTargetParamsEntry_UNDO */
    snmpTargetParamsEntry_ready,
    snmpTargetParamsEntry_set,
    snmpTargetParamsEntry_cleanup,
#ifndef U_snmpTargetParamsEntry
    (SR_COPY_FPTR) NULL
#else /* U_snmpTargetParamsEntry */
    (SR_COPY_FPTR) k_snmpTargetParamsEntryCloneUserpartData
#endif /* U_snmpTargetParamsEntry */
};

/*----------------------------------------------------------------------
 * Free the snmpTargetParamsEntry data object.
 *---------------------------------------------------------------------*/
void
snmpTargetParamsEntry_free(snmpTargetParamsEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 snmpTargetParamsEntryGetInfo.type_table,
                 snmpTargetParamsEntryGetInfo.highest_nominator,
                 snmpTargetParamsEntryGetInfo.valid_offset,
                 snmpTargetParamsEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after snmpTargetParamsEntry set/undo
 *---------------------------------------------------------------------*/
static int
snmpTargetParamsEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &snmpTargetParamsEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the snmpTargetParamsEntry family
 *---------------------------------------------------------------------*/
snmpTargetParamsEntry_t *
Clone_snmpTargetParamsEntry(snmpTargetParamsEntry_t *snmpTargetParamsEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (snmpTargetParamsEntry_t *)SrCloneFamily(snmpTargetParamsEntry,
                         snmpTargetParamsEntryGetInfo.family_size,
                         snmpTargetParamsEntryGetInfo.type_table,
                         snmpTargetParamsEntryGetInfo.highest_nominator,
                         snmpTargetParamsEntryGetInfo.valid_offset,
                         snmpTargetParamsEntryTestInfo.userpart_clone_func,
                         snmpTargetParamsEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
snmpTargetParamsEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                           doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &snmpTargetParamsEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
snmpTargetParamsEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &snmpTargetParamsEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snmpTargetParamsEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_snmpTargetParamsEntry_set((snmpTargetParamsEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


