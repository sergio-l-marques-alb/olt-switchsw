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
 * -apo -c comm snmpCommunityMIB -userpart -v_stubs_only -row_status 
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

static const SR_UINT32 snmpCommunityEntry_last_sid_array[] = {
    1, /* snmpCommunityIndex */
    2, /* snmpCommunityName */
    3, /* snmpCommunitySecurityName */
    4, /* snmpCommunityContextEngineID */
    5, /* snmpCommunityContextName */
    6, /* snmpCommunityTransportTag */
    7, /* snmpCommunityStorageType */
    8  /* snmpCommunityStatus */
};

/*
 * The snmpCommunityEntryTypeTable array should be located in the
 * k_comm.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType snmpCommunityEntryTypeTable[];

const SrIndexInfo snmpCommunityEntryIndexInfo[] = {
#ifdef I_snmpCommunityIndex
    { I_snmpCommunityIndex, T_octet, -1 },
#endif /* I_snmpCommunityIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry snmpCommunityEntryGetInfo = {
    (SR_KGET_FPTR) new_k_snmpCommunityEntry_get,
#ifndef U_snmpCommunityEntry
    (SR_FREE_FPTR) NULL,
#else /* U_snmpCommunityEntry */
    (SR_FREE_FPTR) k_snmpCommunityEntryFreeUserpartData,
#endif /* U_snmpCommunityEntry */
    (int) sizeof(snmpCommunityEntry_t),
    I_snmpCommunityEntry_max,
    (SnmpType *) snmpCommunityEntryTypeTable,
    snmpCommunityEntryIndexInfo,
    (short) offsetof(snmpCommunityEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the snmpCommunityEntry family.
 *---------------------------------------------------------------------*/
VarBind *
snmpCommunityEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                       ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_snmpCommunityIndex) )
    return NULL;
#else	/* ! ( defined(I_snmpCommunityIndex) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpCommunityEntryGetInfo));
#endif	/* ! ( defined(I_snmpCommunityIndex) ) */
}

snmpCommunityEntry_t *
new_k_snmpCommunityEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator, int searchType,
                             snmpCommunityEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_snmpCommunityEntry_get(serialNum, contextInfo, nominator,
                                    searchType, data->snmpCommunityIndex);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int snmpCommunityEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the snmpCommunityEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  snmpCommunityIndex_len[] = { { 1, 32 } };
static LengthTest_t  snmpCommunitySecurityName_len[] = { { 1, 32 } };
static LengthTest_t  snmpCommunityContextEngineID_len[] = { { 5, 32 } };
static LengthTest_t  snmpCommunityContextName_len[] = { { 0, 32 } };
static LengthTest_t  snmpCommunityTransportTag_len[] = { { 0, 255 } };
static RangeTest_t   snmpCommunityStorageType_range[] = { { 1, 5 } };
static RangeTest_t   snmpCommunityStatus_range[] = { { 1, 6 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * snmpCommunityEntry family.
 */
static struct sr_member_test snmpCommunityEntry_member_test[] =
{
    /* snmpCommunityIndex */
    { MINV_LENGTH_TEST, 
      sizeof(snmpCommunityIndex_len)/sizeof(LengthTest_t), /* 1 */
      snmpCommunityIndex_len, NULL },

    /* snmpCommunityName */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* snmpCommunitySecurityName */
    { MINV_LENGTH_TEST, 
      sizeof(snmpCommunitySecurityName_len)/sizeof(LengthTest_t), /* 1 */
      snmpCommunitySecurityName_len, NULL },

    /* snmpCommunityContextEngineID */
    { MINV_LENGTH_TEST, 
      sizeof(snmpCommunityContextEngineID_len)/sizeof(LengthTest_t), /* 1 */
      snmpCommunityContextEngineID_len, NULL },

    /* snmpCommunityContextName */
    { MINV_LENGTH_TEST, 
      sizeof(snmpCommunityContextName_len)/sizeof(LengthTest_t), /* 1 */
      snmpCommunityContextName_len, NULL },

    /* snmpCommunityTransportTag */
    { MINV_LENGTH_TEST, 
      sizeof(snmpCommunityTransportTag_len)/sizeof(LengthTest_t), /* 1 */
      snmpCommunityTransportTag_len, NULL },

    /* snmpCommunityStorageType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpCommunityStorageType_range)/sizeof(RangeTest_t), /* 5 */
      snmpCommunityStorageType_range, StorageType_test },

    /* snmpCommunityStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpCommunityStatus_range)/sizeof(RangeTest_t), /* 6 */
      snmpCommunityStatus_range, RowStatus_test }
};


/*
 * Initialize SrTestInfoEntry for the snmpCommunityEntry family.
 */
const SrTestInfoEntry snmpCommunityEntryTestInfo = {
    &snmpCommunityEntryGetInfo,
    snmpCommunityEntry_member_test,
    k_snmpCommunityEntry_set_defaults,
    k_snmpCommunityEntry_test,
    k_snmpCommunityEntry_ready,
#ifdef SR_snmpCommunityEntry_UNDO
    snmpCommunityEntry_undo,
#else /* SR_snmpCommunityEntry_UNDO */
    NULL,
#endif /* SR_snmpCommunityEntry_UNDO */
    snmpCommunityEntry_ready,
    snmpCommunityEntry_set,
    snmpCommunityEntry_cleanup,
#ifndef U_snmpCommunityEntry
    (SR_COPY_FPTR) NULL
#else /* U_snmpCommunityEntry */
    (SR_COPY_FPTR) k_snmpCommunityEntryCloneUserpartData
#endif /* U_snmpCommunityEntry */
};

/*----------------------------------------------------------------------
 * Free the snmpCommunityEntry data object.
 *---------------------------------------------------------------------*/
void
snmpCommunityEntry_free(snmpCommunityEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 snmpCommunityEntryGetInfo.type_table,
                 snmpCommunityEntryGetInfo.highest_nominator,
                 snmpCommunityEntryGetInfo.valid_offset,
                 snmpCommunityEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after snmpCommunityEntry set/undo
 *---------------------------------------------------------------------*/
static int
snmpCommunityEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &snmpCommunityEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the snmpCommunityEntry family
 *---------------------------------------------------------------------*/
snmpCommunityEntry_t *
Clone_snmpCommunityEntry(snmpCommunityEntry_t *snmpCommunityEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (snmpCommunityEntry_t *)SrCloneFamily(snmpCommunityEntry,
                         snmpCommunityEntryGetInfo.family_size,
                         snmpCommunityEntryGetInfo.type_table,
                         snmpCommunityEntryGetInfo.highest_nominator,
                         snmpCommunityEntryGetInfo.valid_offset,
                         snmpCommunityEntryTestInfo.userpart_clone_func,
                         snmpCommunityEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
snmpCommunityEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                        doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &snmpCommunityEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
snmpCommunityEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &snmpCommunityEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snmpCommunityEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_snmpCommunityEntry_set((snmpCommunityEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


