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

static const SR_UINT32 snmpSet_last_sid_array[] = {
    1  /* snmpSetSerialNo */
};

const SnmpType snmpSetTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(snmpSet_t, snmpSetSerialNo), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry snmpSetGetInfo = {
    (SR_KGET_FPTR) new_k_snmpSet_get,
#ifndef U_snmpSet
    (SR_FREE_FPTR) NULL,
#else /* U_snmpSet */
    (SR_FREE_FPTR) k_snmpSetFreeUserpartData,
#endif /* U_snmpSet */
    (int) sizeof(snmpSet_t),
    I_snmpSet_max,
    (SnmpType *) snmpSetTypeTable,
    NULL,
    (short) offsetof(snmpSet_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the snmpSet family.
 *---------------------------------------------------------------------*/
VarBind *
snmpSet_get(OID *incoming, ObjectInfo *object, int searchType,
            ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpSetGetInfo));
}

snmpSet_t *
new_k_snmpSet_get(int serialNum, ContextInfo *contextInfo,
                  int nominator, int searchType,
                  snmpSet_t *data)
{
    return k_snmpSet_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int snmpSet_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the snmpSet family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   snmpSetSerialNo_range[] = { { 0, 2147483647 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * snmpSet family.
 */
static struct sr_member_test snmpSet_member_test[] =
{
    /* snmpSetSerialNo */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpSetSerialNo_range)/sizeof(RangeTest_t), /* 1 */
      snmpSetSerialNo_range, TestAndIncr_test }
};


/*
 * Initialize SrTestInfoEntry for the snmpSet family.
 */
const SrTestInfoEntry snmpSetTestInfo = {
    &snmpSetGetInfo,
    snmpSet_member_test,
    NULL,
    k_snmpSet_test,
    k_snmpSet_ready,
#ifdef SR_snmpSet_UNDO
    snmpSet_undo,
#else /* SR_snmpSet_UNDO */
    NULL,
#endif /* SR_snmpSet_UNDO */
    snmpSet_ready,
    snmpSet_set,
    snmpSet_cleanup,
#ifndef U_snmpSet
    (SR_COPY_FPTR) NULL
#else /* U_snmpSet */
    (SR_COPY_FPTR) k_snmpSetCloneUserpartData
#endif /* U_snmpSet */
};

/*----------------------------------------------------------------------
 * cleanup after snmpSet set/undo
 *---------------------------------------------------------------------*/
static int
snmpSet_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &snmpSetTestInfo);
}

#ifdef SR_snmpSet_UNDO
/*----------------------------------------------------------------------
 * clone the snmpSet family
 *---------------------------------------------------------------------*/
snmpSet_t *
Clone_snmpSet(snmpSet_t *snmpSet)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (snmpSet_t *)SrCloneFamily(snmpSet,
                         snmpSetGetInfo.family_size,
                         snmpSetGetInfo.type_table,
                         snmpSetGetInfo.highest_nominator,
                         snmpSetGetInfo.valid_offset,
                         snmpSetTestInfo.userpart_clone_func,
                         snmpSetGetInfo.userpart_free_func);
}

#endif /* defined(SR_snmpSet_UNDO) */
/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
snmpSet_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
             doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &snmpSetTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
snmpSet_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &snmpSetTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snmpSet_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_snmpSet_set((snmpSet_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


