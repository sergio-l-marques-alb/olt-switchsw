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

static const SR_UINT32 usmStats_last_sid_array[] = {
    1, /* usmStatsUnsupportedSecLevels */
    2, /* usmStatsNotInTimeWindows */
    3, /* usmStatsUnknownUserNames */
    4, /* usmStatsUnknownEngineIDs */
    5, /* usmStatsWrongDigests */
    6  /* usmStatsDecryptionErrors */
};

const SnmpType usmStatsTypeTable[] = {
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(usmStats_t, usmStatsUnsupportedSecLevels), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(usmStats_t, usmStatsNotInTimeWindows), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(usmStats_t, usmStatsUnknownUserNames), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(usmStats_t, usmStatsUnknownEngineIDs), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(usmStats_t, usmStatsWrongDigests), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(usmStats_t, usmStatsDecryptionErrors), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry usmStatsGetInfo = {
    (SR_KGET_FPTR) new_k_usmStats_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(usmStats_t),
    I_usmStats_max,
    (SnmpType *) usmStatsTypeTable,
    NULL,
    (short) offsetof(usmStats_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the usmStats family.
 *---------------------------------------------------------------------*/
VarBind *
usmStats_get(OID *incoming, ObjectInfo *object, int searchType,
             ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &usmStatsGetInfo));
}

usmStats_t *
new_k_usmStats_get(int serialNum, ContextInfo *contextInfo,
                   int nominator, int searchType,
                   usmStats_t *data)
{
    return k_usmStats_get(serialNum, contextInfo, nominator);
}

static const SR_UINT32 usmUser_last_sid_array[] = {
    1  /* usmUserSpinLock */
};

const SnmpType usmUserTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(usmUser_t, usmUserSpinLock), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry usmUserGetInfo = {
    (SR_KGET_FPTR) new_k_usmUser_get,
#ifndef U_usmUser
    (SR_FREE_FPTR) NULL,
#else /* U_usmUser */
    (SR_FREE_FPTR) k_usmUserFreeUserpartData,
#endif /* U_usmUser */
    (int) sizeof(usmUser_t),
    I_usmUser_max,
    (SnmpType *) usmUserTypeTable,
    NULL,
    (short) offsetof(usmUser_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the usmUser family.
 *---------------------------------------------------------------------*/
VarBind *
usmUser_get(OID *incoming, ObjectInfo *object, int searchType,
            ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &usmUserGetInfo));
}

usmUser_t *
new_k_usmUser_get(int serialNum, ContextInfo *contextInfo,
                  int nominator, int searchType,
                  usmUser_t *data)
{
    return k_usmUser_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int usmUser_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the usmUser family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   usmUserSpinLock_range[] = { { 0, 2147483647 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * usmUser family.
 */
static struct sr_member_test usmUser_member_test[] =
{
    /* usmUserSpinLock */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(usmUserSpinLock_range)/sizeof(RangeTest_t), /* 1 */
      usmUserSpinLock_range, TestAndIncr_test }
};


/*
 * Initialize SrTestInfoEntry for the usmUser family.
 */
const SrTestInfoEntry usmUserTestInfo = {
    &usmUserGetInfo,
    usmUser_member_test,
    NULL,
    k_usmUser_test,
    k_usmUser_ready,
#ifdef SR_usmUser_UNDO
    usmUser_undo,
#else /* SR_usmUser_UNDO */
    NULL,
#endif /* SR_usmUser_UNDO */
    usmUser_ready,
    usmUser_set,
    usmUser_cleanup,
#ifndef U_usmUser
    (SR_COPY_FPTR) NULL
#else /* U_usmUser */
    (SR_COPY_FPTR) k_usmUserCloneUserpartData
#endif /* U_usmUser */
};

/*----------------------------------------------------------------------
 * cleanup after usmUser set/undo
 *---------------------------------------------------------------------*/
static int
usmUser_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &usmUserTestInfo);
}

#ifdef SR_usmUser_UNDO
/*----------------------------------------------------------------------
 * clone the usmUser family
 *---------------------------------------------------------------------*/
usmUser_t *
Clone_usmUser(usmUser_t *usmUser)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (usmUser_t *)SrCloneFamily(usmUser,
                         usmUserGetInfo.family_size,
                         usmUserGetInfo.type_table,
                         usmUserGetInfo.highest_nominator,
                         usmUserGetInfo.valid_offset,
                         usmUserTestInfo.userpart_clone_func,
                         usmUserGetInfo.userpart_free_func);
}

#endif /* defined(SR_usmUser_UNDO) */
/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
usmUser_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
             doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &usmUserTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
usmUser_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &usmUserTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
usmUser_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_usmUser_set((usmUser_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


static const SR_UINT32 usmUserEntry_last_sid_array[] = {
    1, /* usmUserEngineID */
    2, /* usmUserName */
    3, /* usmUserSecurityName */
    4, /* usmUserCloneFrom */
    5, /* usmUserAuthProtocol */
    6, /* usmUserAuthKeyChange */
    7, /* usmUserOwnAuthKeyChange */
    8, /* usmUserPrivProtocol */
    9, /* usmUserPrivKeyChange */
    10, /* usmUserOwnPrivKeyChange */
    11, /* usmUserPublic */
    12, /* usmUserStorageType */
    13, /* usmUserStatus */
    1, /* usmTargetTag (from usmTargetTagEntry) */
    0
};

/*
 * The usmUserEntryTypeTable array should be located in the
 * k_usm.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType usmUserEntryTypeTable[];

const SrIndexInfo usmUserEntryIndexInfo[] = {
#ifdef I_usmUserEngineID
    { I_usmUserEngineID, T_var_octet, -1 },
#endif /* I_usmUserEngineID */
#ifdef I_usmUserName
    { I_usmUserName, T_var_octet, -1 },
#endif /* I_usmUserName */
    { -1, -1, -1 }
};

const SrGetInfoEntry usmUserEntryGetInfo = {
    (SR_KGET_FPTR) new_k_usmUserEntry_get,
#ifndef U_usmUserEntry
    (SR_FREE_FPTR) NULL,
#else /* U_usmUserEntry */
    (SR_FREE_FPTR) k_usmUserEntryFreeUserpartData,
#endif /* U_usmUserEntry */
    (int) sizeof(usmUserEntry_t),
    I_usmUserEntry_max,
    (SnmpType *) usmUserEntryTypeTable,
    usmUserEntryIndexInfo,
    (short) offsetof(usmUserEntry_t, valid)
};

const SrGetInfoEntry usmTargetTagEntryGetInfo = {
    (SR_KGET_FPTR) new_k_usmUserEntry_get,
#ifndef U_usmUserEntry
    (SR_FREE_FPTR) NULL,
#else /* U_usmUserEntry */
    (SR_FREE_FPTR) k_usmUserEntryFreeUserpartData,
#endif /* U_usmUserEntry */
    (int) sizeof(usmUserEntry_t),
    I_usmUserEntry_max,
    (SnmpType *) usmUserEntryTypeTable,
    usmUserEntryIndexInfo,
    (short) offsetof(usmUserEntry_t, valid)
};




/*---------------------------------------------------------------------
 * Retrieve data from the usmUserEntry family.
 *---------------------------------------------------------------------*/
VarBind *
usmUserEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                 ContextInfo *contextInfo, int serialNum)
{
#if (! ( defined(I_usmUserEngineID) ) || ! ( defined(I_usmUserName) ))
    return NULL;
#else	/* (! ( defined(I_usmUserEngineID) ) || ! ( defined(I_usmUserName) )) */

    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &usmUserEntryGetInfo));

#endif	/* (! ( defined(I_usmUserEngineID) ) || ! ( defined(I_usmUserName) )) */
}

usmUserEntry_t *
new_k_usmUserEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator, int searchType,
                       usmUserEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_usmUserEntry_get(serialNum, contextInfo, nominator,
                              searchType, data->usmUserEngineID,
                              data->usmUserName);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int usmUserEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the usmUserEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  usmUserEngineID_len[] = { { 5, 32 } };
static LengthTest_t  usmUserName_len[] = { { 1, 32 } };
static LengthTest_t  usmUserPublic_len[] = { { 0, 32 } };
static RangeTest_t   usmUserStorageType_range[] = { { 1, 5 } };
static RangeTest_t   usmUserStatus_range[] = { { 1, 6 } };
static LengthTest_t  usmTargetTag_len[] = { { 0, 255 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * usmUserEntry family.
 */
static struct sr_member_test usmUserEntry_member_test[] =
{
    /* usmUserEngineID */
    { MINV_LENGTH_TEST, 
      sizeof(usmUserEngineID_len)/sizeof(LengthTest_t), /* 1 */
      usmUserEngineID_len, NULL },

    /* usmUserName */
    { MINV_LENGTH_TEST, 
      sizeof(usmUserName_len)/sizeof(LengthTest_t), /* 1 */
      usmUserName_len, NULL },

    /* usmUserSecurityName */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* usmUserCloneFrom */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* usmUserAuthProtocol */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* usmUserAuthKeyChange */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* usmUserOwnAuthKeyChange */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* usmUserPrivProtocol */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* usmUserPrivKeyChange */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* usmUserOwnPrivKeyChange */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* usmUserPublic */
    { MINV_LENGTH_TEST, 
      sizeof(usmUserPublic_len)/sizeof(LengthTest_t), /* 1 */
      usmUserPublic_len, NULL },

    /* usmUserStorageType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(usmUserStorageType_range)/sizeof(RangeTest_t), /* 5 */
      usmUserStorageType_range, StorageType_test },

    /* usmUserStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(usmUserStatus_range)/sizeof(RangeTest_t), /* 6 */
      usmUserStatus_range, RowStatus_test },

    /* usmTargetTag */
    { MINV_LENGTH_TEST, 
      sizeof(usmTargetTag_len)/sizeof(LengthTest_t), /* 1 */
      usmTargetTag_len, NULL }
};


/*
 * Initialize SrTestInfoEntry for the usmUserEntry family.
 */
const SrTestInfoEntry usmUserEntryTestInfo = {
    &usmUserEntryGetInfo,
    usmUserEntry_member_test,
    k_usmUserEntry_set_defaults,
    k_usmUserEntry_test,
    k_usmUserEntry_ready,
#ifdef SR_usmUserEntry_UNDO
    usmUserEntry_undo,
#else /* SR_usmUserEntry_UNDO */
    NULL,
#endif /* SR_usmUserEntry_UNDO */
    usmUserEntry_ready,
    usmUserEntry_set,
    usmUserEntry_cleanup,
#ifndef U_usmUserEntry
    (SR_COPY_FPTR) NULL
#else /* U_usmUserEntry */
    (SR_COPY_FPTR) k_usmUserEntryCloneUserpartData
#endif /* U_usmUserEntry */
};


/*
 * Initialize SrTestInfoEntry for the usmTargetTagEntry family
 * which augments usmUserEntry.
 */
const SrTestInfoEntry usmTargetTagEntryTestInfo = {
    &usmTargetTagEntryGetInfo,
    usmUserEntry_member_test,
    k_usmUserEntry_set_defaults,
    k_usmUserEntry_test,
    k_usmUserEntry_ready,
#ifdef SR_usmUserEntry_UNDO
    usmUserEntry_undo,
#else /* SR_usmUserEntry_UNDO */
    NULL,
#endif /* SR_usmUserEntry_UNDO */
    usmUserEntry_ready,
    usmUserEntry_set,
    usmUserEntry_cleanup,
#ifndef U_usmUserEntry
    (SR_COPY_FPTR) NULL
#else /* U_usmUserEntry */
    (SR_COPY_FPTR) k_usmUserEntryCloneUserpartData
#endif /* U_usmUserEntry */
};

/*----------------------------------------------------------------------
 * Free the usmUserEntry data object.
 *---------------------------------------------------------------------*/
void
usmUserEntry_free(usmUserEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 usmUserEntryGetInfo.type_table,
                 usmUserEntryGetInfo.highest_nominator,
                 usmUserEntryGetInfo.valid_offset,
                 usmUserEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after usmUserEntry set/undo
 *---------------------------------------------------------------------*/
static int
usmUserEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &usmUserEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the usmUserEntry family
 *---------------------------------------------------------------------*/
usmUserEntry_t *
Clone_usmUserEntry(usmUserEntry_t *usmUserEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (usmUserEntry_t *)SrCloneFamily(usmUserEntry,
                         usmUserEntryGetInfo.family_size,
                         usmUserEntryGetInfo.type_table,
                         usmUserEntryGetInfo.highest_nominator,
                         usmUserEntryGetInfo.valid_offset,
                         usmUserEntryTestInfo.userpart_clone_func,
                         usmUserEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
usmUserEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                  doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &usmUserEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
usmUserEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &usmUserEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
usmUserEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_usmUserEntry_set((usmUserEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


