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

static const SR_UINT32 system_last_sid_array[] = {
    1, /* sysDescr */
    2, /* sysObjectID */
    3, /* sysUpTime */
    4, /* sysContact */
    5, /* sysName */
    6, /* sysLocation */
    7, /* sysServices */
    8  /* sysORLastChange */
};

const SnmpType systemTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(system_t, sysDescr), -1 },
    { OBJECT_ID_TYPE, SR_READ_ONLY, offsetof(system_t, sysObjectID), -1 },
    { TIME_TICKS_TYPE, SR_READ_ONLY, offsetof(system_t, sysUpTime), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(system_t, sysContact), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(system_t, sysName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(system_t, sysLocation), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(system_t, sysServices), -1 },
    { TIME_TICKS_TYPE, SR_READ_ONLY, offsetof(system_t, sysORLastChange), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry systemGetInfo = {
    (SR_KGET_FPTR) new_k_system_get,
#ifndef U_system
    (SR_FREE_FPTR) NULL,
#else /* U_system */
    (SR_FREE_FPTR) k_systemFreeUserpartData,
#endif /* U_system */
    (int) sizeof(system_t),
    I_system_max,
    (SnmpType *) systemTypeTable,
    NULL,
    (short) offsetof(system_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the system family.
 *---------------------------------------------------------------------*/
VarBind *
system_get(OID *incoming, ObjectInfo *object, int searchType,
           ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &systemGetInfo));
}

system_t *
new_k_system_get(int serialNum, ContextInfo *contextInfo,
                 int nominator, int searchType,
                 system_t *data)
{
    return k_system_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int system_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the system family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static LengthTest_t  sysContact_len[] = { { 0, 255 } };
static LengthTest_t  sysName_len[] = { { 0, 255 } };
static LengthTest_t  sysLocation_len[] = { { 0, 255 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * system family.
 */
static struct sr_member_test system_member_test[] =
{
    /* sysDescr */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* sysObjectID */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* sysUpTime */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* sysContact */
    { MINV_LENGTH_TEST, 
      sizeof(sysContact_len)/sizeof(LengthTest_t), /* 1 */
      sysContact_len, DisplayString_test },

    /* sysName */
    { MINV_LENGTH_TEST, 
      sizeof(sysName_len)/sizeof(LengthTest_t), /* 1 */
      sysName_len, DisplayString_test },

    /* sysLocation */
    { MINV_LENGTH_TEST, 
      sizeof(sysLocation_len)/sizeof(LengthTest_t), /* 1 */
      sysLocation_len, DisplayString_test },

    /* sysServices */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* sysORLastChange */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};


/*
 * Initialize SrTestInfoEntry for the system family.
 */
const SrTestInfoEntry systemTestInfo = {
    &systemGetInfo,
    system_member_test,
    NULL,
    k_system_test,
    k_system_ready,
#ifdef SR_system_UNDO
    system_undo,
#else /* SR_system_UNDO */
    NULL,
#endif /* SR_system_UNDO */
    system_ready,
    system_set,
    system_cleanup,
#ifndef U_system
    (SR_COPY_FPTR) NULL
#else /* U_system */
    (SR_COPY_FPTR) k_systemCloneUserpartData
#endif /* U_system */
};

/*----------------------------------------------------------------------
 * cleanup after system set/undo
 *---------------------------------------------------------------------*/
static int
system_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &systemTestInfo);
}

#ifdef SR_system_UNDO
/*----------------------------------------------------------------------
 * clone the system family
 *---------------------------------------------------------------------*/
system_t *
Clone_system(system_t *system)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (system_t *)SrCloneFamily(system,
                         systemGetInfo.family_size,
                         systemGetInfo.type_table,
                         systemGetInfo.highest_nominator,
                         systemGetInfo.valid_offset,
                         systemTestInfo.userpart_clone_func,
                         systemGetInfo.userpart_free_func);
}

#endif /* defined(SR_system_UNDO) */
/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
system_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
            doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &systemTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
system_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &systemTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
system_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_system_set((system_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


