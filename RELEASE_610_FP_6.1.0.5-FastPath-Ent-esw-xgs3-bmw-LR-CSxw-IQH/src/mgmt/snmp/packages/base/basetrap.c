/*
 *
 * Copyright (C) 1992-1999 by SNMP Research, Incorporated.
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
 * -c private +interfaces +ifMIB +dot3 +dot1dBridge +statistics +history +alarm +event +fastPathNP201 +lagMIB +mauMod +lvl7BmLifMIB -per_file_init -traps -agent 
 */

#include "sr_conf.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif /* HAVE_MEMORY_H */
#include "sr_snmp.h"
#include "diag.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "getvar.h"
#include "trap.h"

#include "basetrap.h"

/* LVL7 */
#include "sr_ntfy.h"
#include "l7_common.h"
#include "snmp_util_api.h"

SR_FILENAME

static VarBind *private_GetTrapVar
SR_PROTOTYPE((OID *object,
              OID *inst,
              ContextInfo *contextInfo));

static VarBind *
private_GetTrapVar(
    OID *object,
    OID *inst,
    ContextInfo *contextInfo)
{
    OID *var;
    VarBind *vb = NULL;
    L7_BOOL localCip = L7_FALSE;
    ContextInfo *cip = NULL;

    if (contextInfo == NULL)
    {
        cip = AllocContextInfo(0);
  
        if(NULL == cip)
            return NULL;
              
        localCip = L7_TRUE;
    }
    else
    {
        cip = contextInfo;
    }
    
    if ((object == NULL) || (inst == NULL)) {
      if (localCip == L7_TRUE)
        FreeContextInfo(cip);
      return NULL;
    }
    var = CatOID(object, inst);
    FreeOID(object);
    FreeOID(inst);
    if (var == NULL) {
      if (localCip == L7_TRUE)
            FreeContextInfo(cip);
      return NULL;
    }
    vb = i_GetVar(cip, EXACT, var);
    if (vb == NULL) {
        FreeOID(var);
        if (localCip == L7_TRUE)
            FreeContextInfo(cip);
        return NULL;
    }
#ifdef SR_SNMPv2_PDU
    if ((vb->value.type == NO_SUCH_OBJECT_EXCEPTION) ||
        (vb->value.type == NO_SUCH_INSTANCE_EXCEPTION)) {
        FreeOID(var);
        FreeVarBindList(vb);
        if (localCip == L7_TRUE)
            FreeContextInfo(cip);
        return NULL;
    }
#endif /* SR_SNMPv2_PDU */
    FreeOID(var);
    if (localCip == L7_TRUE)
        FreeContextInfo(cip);
    return vb;
}

int
i_send_authenticationFailure_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* "1.3.6.1.6.3.1.1.5.5" = snmpTraps.5 */
    DPRINTF((APTRAP, "Sending authenticationFailure Trap\n"));
    do_trap(4, 0, vb, enterprise, "1.3.6.1.6.3.1.1.5.5");
    FreeOID(enterprise);
    return 0;
}

int
send_authenticationFailure_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_authenticationFailure_trap(add_vblist, contextInfo);
}

int
i_send_warmStart_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* "1.3.6.1.6.3.1.1.5.2" = snmpTraps.2 */
    DPRINTF((APTRAP, "Sending warmStart Trap\n"));
    do_trap(1, 0, vb, enterprise, "1.3.6.1.6.3.1.1.5.2");
    FreeOID(enterprise);
    return 0;
}

int
send_warmStart_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_warmStart_trap(add_vblist, contextInfo);
}

int
i_send_coldStart_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* "1.3.6.1.6.3.1.1.5.1" = snmpTraps.1 */
    DPRINTF((APTRAP, "Sending coldStart Trap\n"));
    do_trap(0, 0, vb, enterprise, "1.3.6.1.6.3.1.1.5.1");
    FreeOID(enterprise);
    return 0;
}

int
send_coldStart_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_coldStart_trap(add_vblist, contextInfo);
}

int
i_send_powerSupplyStatusChangeTrap_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* "1.3.6.1.4.1.6132.1.1.1.50.12" = fastPathSwitchingTraps.12 */
    DPRINTF((APTRAP, "Sending powerSupplyStatusChangeTrap Trap\n"));
    do_trap(6, 0, vb, enterprise, "powerSupplyStatusChangeTrap");
    FreeOID(enterprise);
    return 0;
}

int
send_powerSupplyStatusChangeTrap_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_powerSupplyStatusChangeTrap_trap(add_vblist, contextInfo);
}

int
i_send_fanFailureTrap_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* "1.3.6.1.4.1.6132.1.1.1.50.9" = fastPathSwitchingTraps.9 */
    DPRINTF((APTRAP, "Sending fanFailureTrap Trap\n"));
    do_trap(6, 0, vb, enterprise, "fanFailureTrap");
    FreeOID(enterprise);
    return 0;
}

int
send_fanFailureTrap_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_fanFailureTrap_trap(add_vblist, contextInfo);
}

int
i_send_linkFailureTrap_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* "1.3.6.1.4.1.6132.1.1.1.50.4" = fastPathSwitchingTraps.4 */
    DPRINTF((APTRAP, "Sending linkFailureTrap Trap\n"));
    do_trap(6, 0, vb, enterprise, "linkFailureTrap");
    FreeOID(enterprise);
    return 0;
}

int
send_linkFailureTrap_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_linkFailureTrap_trap(add_vblist, contextInfo);
}

int
i_send_broadcastStormEndTrap_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* "1.3.6.1.4.1.6132.1.1.1.50.3" = fastPathSwitchingTraps.3 */
    DPRINTF((APTRAP, "Sending broadcastStormEndTrap Trap\n"));
    do_trap(6, 0, vb, enterprise, "broadcastStormEndTrap");
    FreeOID(enterprise);
    return 0;
}

int
send_broadcastStormEndTrap_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_broadcastStormEndTrap_trap(add_vblist, contextInfo);
}

int
i_send_broadcastStormStartTrap_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* "1.3.6.1.4.1.6132.1.1.1.50.2" = fastPathSwitchingTraps.2 */
    DPRINTF((APTRAP, "Sending broadcastStormStartTrap Trap\n"));
    do_trap(6, 0, vb, enterprise, "broadcastStormStartTrap");
    FreeOID(enterprise);
    return 0;
}

int
send_broadcastStormStartTrap_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_broadcastStormStartTrap_trap(add_vblist, contextInfo);
}

int
i_send_multipleUsersTrap_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* "1.3.6.1.4.1.6132.1.1.1.50.1" = fastPathSwitchingTraps.1 */
    DPRINTF((APTRAP, "Sending multipleUsersTrap Trap\n"));
    do_trap(6, 0, vb, enterprise, "multipleUsersTrap");
    FreeOID(enterprise);
    return 0;
}

int
send_multipleUsersTrap_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_multipleUsersTrap_trap(add_vblist, contextInfo);
}

int
i_send_fallingAlarm_trap(
    OID             *alarmIndex,
    OID             *alarmVariable,
    OID             *alarmSampleType,
    OID             *alarmValue,
    OID             *alarmFallingThreshold,
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    VarBind         *temp_vb = NULL;
    
    if ((alarmIndex == NULL) ||
        (alarmVariable == NULL) ||
        (alarmSampleType == NULL) ||
        (alarmValue == NULL) ||
        (alarmFallingThreshold == NULL)) {
        FreeOID(alarmIndex);
        FreeOID(alarmVariable);
        FreeOID(alarmSampleType);
        FreeOID(alarmValue);
        FreeOID(alarmFallingThreshold);
        DPRINTF((APTRAP, "i_send_fallingAlarm_trap: "));
        DPRINTF((APTRAP, "At least one incoming OID is NULL\n"));
        return -1;
    }

    /* 1.3.6.1.2.1.16.3.1.1.8 = alarmFallingThreshold */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.8"),
              alarmFallingThreshold, contextInfo);
    if (temp_vb == NULL) {
        DPRINTF((APTRAP, "i_send_fallingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(alarmIndex);
        FreeOID(alarmVariable);
        FreeOID(alarmSampleType);
        FreeOID(alarmValue);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.16.3.1.1.5 = alarmValue */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.5"),
              alarmValue, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_fallingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(alarmIndex);
        FreeOID(alarmVariable);
        FreeOID(alarmSampleType);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.16.3.1.1.4 = alarmSampleType */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.4"),
              alarmSampleType, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_fallingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(alarmIndex);
        FreeOID(alarmVariable);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.16.3.1.1.3 = alarmVariable */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.3"),
              alarmVariable, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_fallingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(alarmIndex);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.16.3.1.1.1 = alarmIndex */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.1"),
              alarmIndex, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_fallingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* "1.3.6.1.2.1.16.0.2" = rmonEventsV2.2 */
    DPRINTF((APTRAP, "Sending fallingAlarm Trap\n"));
    do_trap(6, 0, vb, enterprise, "1.3.6.1.2.1.16.0.2");
    FreeOID(enterprise);
    return 0;
}

int
send_fallingAlarm_trap(
    SR_INT32 alarmIndex_0,
    SR_INT32 alarmIndex_1,
    SR_INT32 alarmIndex_2,
    SR_INT32 alarmIndex_3,
    SR_INT32 alarmIndex_4,
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    int index;
    OID inst;
    SR_UINT32 buffer[5];
    OID *alarmIndex;
    OID *alarmVariable;
    OID *alarmSampleType;
    OID *alarmValue;
    OID *alarmFallingThreshold;
    inst.oid_ptr = buffer;

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_0;
    inst.length = index;
    alarmIndex = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_1;
    inst.length = index;
    alarmVariable = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_2;
    inst.length = index;
    alarmSampleType = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_3;
    inst.length = index;
    alarmValue = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_4;
    inst.length = index;
    alarmFallingThreshold = CloneOID(&inst);

    return i_send_fallingAlarm_trap(alarmIndex, alarmVariable, alarmSampleType, alarmValue, alarmFallingThreshold, add_vblist, contextInfo);
}

int
i_send_risingAlarm_trap(
    OID             *alarmIndex,
    OID             *alarmVariable,
    OID             *alarmSampleType,
    OID             *alarmValue,
    OID             *alarmRisingThreshold,
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    VarBind         *temp_vb = NULL;
    
    if ((alarmIndex == NULL) ||
        (alarmVariable == NULL) ||
        (alarmSampleType == NULL) ||
        (alarmValue == NULL) ||
        (alarmRisingThreshold == NULL)) {
        FreeOID(alarmIndex);
        FreeOID(alarmVariable);
        FreeOID(alarmSampleType);
        FreeOID(alarmValue);
        FreeOID(alarmRisingThreshold);
        DPRINTF((APTRAP, "i_send_risingAlarm_trap: "));
        DPRINTF((APTRAP, "At least one incoming OID is NULL\n"));
        return -1;
    }

    /* 1.3.6.1.2.1.16.3.1.1.7 = alarmRisingThreshold */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.7"),
              alarmRisingThreshold, contextInfo);
    if (temp_vb == NULL) {
        DPRINTF((APTRAP, "i_send_risingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(alarmIndex);
        FreeOID(alarmVariable);
        FreeOID(alarmSampleType);
        FreeOID(alarmValue);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.16.3.1.1.5 = alarmValue */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.5"),
              alarmValue, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_risingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(alarmIndex);
        FreeOID(alarmVariable);
        FreeOID(alarmSampleType);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.16.3.1.1.4 = alarmSampleType */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.4"),
              alarmSampleType, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_risingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(alarmIndex);
        FreeOID(alarmVariable);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.16.3.1.1.3 = alarmVariable */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.3"),
              alarmVariable, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_risingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(alarmIndex);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.16.3.1.1.1 = alarmIndex */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.16.3.1.1.1"),
              alarmIndex, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_risingAlarm_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* "1.3.6.1.2.1.16.0.1" = rmonEventsV2.1 */
    DPRINTF((APTRAP, "Sending risingAlarm Trap\n"));
    do_trap(6, 0, vb, enterprise, "1.3.6.1.2.1.16.0.1");
    FreeOID(enterprise);
    return 0;
}

int
send_risingAlarm_trap(
    SR_INT32 alarmIndex_0,
    SR_INT32 alarmIndex_1,
    SR_INT32 alarmIndex_2,
    SR_INT32 alarmIndex_3,
    SR_INT32 alarmIndex_4,
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    int index;
    OID inst;
    SR_UINT32 buffer[5];
    OID *alarmIndex;
    OID *alarmVariable;
    OID *alarmSampleType;
    OID *alarmValue;
    OID *alarmRisingThreshold;
    inst.oid_ptr = buffer;

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_0;
    inst.length = index;
    alarmIndex = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_1;
    inst.length = index;
    alarmVariable = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_2;
    inst.length = index;
    alarmSampleType = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_3;
    inst.length = index;
    alarmValue = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) alarmIndex_4;
    inst.length = index;
    alarmRisingThreshold = CloneOID(&inst);

    return i_send_risingAlarm_trap(alarmIndex, alarmVariable, alarmSampleType, alarmValue, alarmRisingThreshold, add_vblist, contextInfo);
}

int
i_send_topologyChange_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* 1.3.6.1.2.1.17 = dot1dBridge */
    if ((enterprise = MakeOIDFromDot("1.3.6.1.2.1.17")) == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_topologyChange_trap: "));
        DPRINTF((APTRAP, "MakeOIDFromDot failed for enterprise OID\n"));
        return -1;
    }

    /* "1.3.6.1.2.1.17.0.2" = dot1dBridge.0.2 */
    DPRINTF((APTRAP, "Sending topologyChange Trap\n"));
    do_trap(6, 2, vb, enterprise, "1.3.6.1.2.1.17.0.2");
    FreeOID(enterprise);
    return 0;
}

int
send_topologyChange_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_topologyChange_trap(add_vblist, contextInfo);
}

int
i_send_newRoot_trap(
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    
    /* 1.3.6.1.2.1.17 = dot1dBridge */
    if ((enterprise = MakeOIDFromDot("1.3.6.1.2.1.17")) == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_newRoot_trap: "));
        DPRINTF((APTRAP, "MakeOIDFromDot failed for enterprise OID\n"));
        return -1;
    }

    /* "1.3.6.1.2.1.17.0.1" = dot1dBridge.0.1 */
    DPRINTF((APTRAP, "Sending newRoot Trap\n"));
    do_trap(6, 1, vb, enterprise, "1.3.6.1.2.1.17.0.1");
    FreeOID(enterprise);
    return 0;
}

int
send_newRoot_trap(
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    return i_send_newRoot_trap(add_vblist, contextInfo);
}

int
i_send_linkUp_trap(
    OID             *ifIndex,
    OID             *ifAdminStatus,
    OID             *ifOperStatus,
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    VarBind         *temp_vb = NULL;
    
    if ((ifIndex == NULL) ||
        (ifAdminStatus == NULL) ||
        (ifOperStatus == NULL)) {
        FreeOID(ifIndex);
        FreeOID(ifAdminStatus);
        FreeOID(ifOperStatus);
        DPRINTF((APTRAP, "i_send_linkUp_trap: "));
        DPRINTF((APTRAP, "At least one incoming OID is NULL\n"));
        return -1;
    }

    /* 1.3.6.1.2.1.2.2.1.8 = ifOperStatus */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.2.2.1.8"),
              ifOperStatus, contextInfo);
    if (temp_vb == NULL) {
        DPRINTF((APTRAP, "i_send_linkUp_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(ifIndex);
        FreeOID(ifAdminStatus);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.2.2.1.7 = ifAdminStatus */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.2.2.1.7"),
              ifAdminStatus, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_linkUp_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(ifIndex);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.2.2.1.1 = ifIndex */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.2.2.1.1"),
              ifIndex, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_linkUp_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* "1.3.6.1.6.3.1.1.5.4" = snmpTraps.4 */
    DPRINTF((APTRAP, "Sending linkUp Trap\n"));
    do_trap(3, 0, vb, enterprise, "1.3.6.1.6.3.1.1.5.4");
    FreeOID(enterprise);
    return 0;
}

int
send_linkUp_trap(
    SR_INT32 ifIndex_0,
    SR_INT32 ifIndex_1,
    SR_INT32 ifIndex_2,
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    int index;
    OID inst;
    SR_UINT32 buffer[3];
    OID *ifIndex;
    OID *ifAdminStatus;
    OID *ifOperStatus;
    inst.oid_ptr = buffer;

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) ifIndex_0;
    inst.length = index;
    ifIndex = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) ifIndex_1;
    inst.length = index;
    ifAdminStatus = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) ifIndex_2;
    inst.length = index;
    ifOperStatus = CloneOID(&inst);

    return i_send_linkUp_trap(ifIndex, ifAdminStatus, ifOperStatus, add_vblist, contextInfo);
}

int
i_send_linkDown_trap(
    OID             *ifIndex,
    OID             *ifAdminStatus,
    OID             *ifOperStatus,
    VarBind         *add_vblist,
    ContextInfo     *contextInfo)
{
    OID             *enterprise = NULL;
    VarBind         *vb = NULL;
    VarBind         *temp_vb = NULL;
    
    if ((ifIndex == NULL) ||
        (ifAdminStatus == NULL) ||
        (ifOperStatus == NULL)) {
        FreeOID(ifIndex);
        FreeOID(ifAdminStatus);
        FreeOID(ifOperStatus);
        DPRINTF((APTRAP, "i_send_linkDown_trap: "));
        DPRINTF((APTRAP, "At least one incoming OID is NULL\n"));
        return -1;
    }

    /* 1.3.6.1.2.1.2.2.1.8 = ifOperStatus */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.2.2.1.8"),
              ifOperStatus, contextInfo);
    if (temp_vb == NULL) {
        DPRINTF((APTRAP, "i_send_linkDown_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(ifIndex);
        FreeOID(ifAdminStatus);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.2.2.1.7 = ifAdminStatus */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.2.2.1.7"),
              ifAdminStatus, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_linkDown_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        FreeOID(ifIndex);
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* 1.3.6.1.2.1.2.2.1.1 = ifIndex */
    temp_vb = private_GetTrapVar(MakeOIDFromDot("1.3.6.1.2.1.2.2.1.1"),
              ifIndex, contextInfo);
    if (temp_vb == NULL) {
        if (vb != NULL) {
            FreeVarBindList(vb);
        }
        DPRINTF((APTRAP, "i_send_linkDown_trap: "));
        DPRINTF((APTRAP, "private_GetTrapVar() failed\n"));
        return -1;
    }
    
    temp_vb->next_var = vb;
    vb = temp_vb;

    /* "1.3.6.1.6.3.1.1.5.3" = snmpTraps.3 */
    DPRINTF((APTRAP, "Sending linkDown Trap\n"));
    do_trap(2, 0, vb, enterprise, "1.3.6.1.6.3.1.1.5.3");
    FreeOID(enterprise);
    return 0;
}

int
send_linkDown_trap(
    SR_INT32 ifIndex_0,
    SR_INT32 ifIndex_1,
    SR_INT32 ifIndex_2,
    VarBind *add_vblist,
    ContextInfo *contextInfo)
{
    int index;
    OID inst;
    SR_UINT32 buffer[3];
    OID *ifIndex;
    OID *ifAdminStatus;
    OID *ifOperStatus;
    inst.oid_ptr = buffer;

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) ifIndex_0;
    inst.length = index;
    ifIndex = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) ifIndex_1;
    inst.length = index;
    ifAdminStatus = CloneOID(&inst);

    index = 0;
    inst.oid_ptr[index++] = (unsigned long) ifIndex_2;
    inst.length = index;
    ifOperStatus = CloneOID(&inst);

    return i_send_linkDown_trap(ifIndex, ifAdminStatus, ifOperStatus, add_vblist, contextInfo);
}

/*************************************************************************************************/

int
snmp_AuthenticationFailureTrapSend()
{
    return send_authenticationFailure_trap(NULL, NULL);
}
                                     
int
snmp_WarmStartTrapSend()
{
    return send_warmStart_trap(NULL, NULL);
}

int
snmp_ColdStartTrapSend()
{
    return send_coldStart_trap(NULL, NULL);
}

int
snmp_FanFailureTrapSend()
{
    return send_fanFailureTrap_trap(NULL, NULL);
}

int
snmp_PowerSupplyStatusChangeTrapSend()
{
    return send_powerSupplyStatusChangeTrap_trap(NULL, NULL);
}

int
snmp_LinkFailureTrapSend()
{
    return send_linkFailureTrap_trap(NULL, NULL);
}

int
snmp_BroadcastStormEndTrapSend()
{
    return send_broadcastStormEndTrap_trap(NULL, NULL);
}

int
snmp_BroadcastStormStartTrapSend()
{
    return send_broadcastStormStartTrap_trap(NULL,NULL);
}

int
snmp_MultipleUsersTrapSend()
{
    return send_multipleUsersTrap_trap(NULL,NULL);
}

int
snmp_FallingAlarmTrapSend( L7_int32 alarmIndex)
{
    return send_fallingAlarm_trap(alarmIndex, alarmIndex, alarmIndex, alarmIndex, alarmIndex, NULL, NULL );
}

int
snmp_RisingAlarmTrapSend ( L7_int32 alarmIndex)
{
   return send_risingAlarm_trap ( alarmIndex, alarmIndex, alarmIndex, alarmIndex, alarmIndex, NULL, NULL );
}

int
snmp_TopologyChangeTrapSend()
{
    return send_topologyChange_trap(NULL, NULL);
}

int
snmp_NewRootTrapSend()
{
    return send_newRoot_trap(NULL, NULL);
}

int
snmp_LinkUpTrapSend( L7_int32 ifIndex )
{
    return send_linkUp_trap(ifIndex, ifIndex, ifIndex, NULL, NULL);
}

int
snmp_LinkDownTrapSend( L7_int32 ifIndex )
{
    return send_linkDown_trap(ifIndex, ifIndex, ifIndex, NULL, NULL);
}

int
snmp_failedUserLoginTrapSend()
{
  return send_UserLoginTrap_trap();
}

int
snmp_lockedUserLoginTrapSend()
{
  return send_UserLockoutTrap_trap();
}    

int
snmp_dhcpSnoopingIntfErrorDisabledTrapSend(L7_uint32 ifIndex)
{
  return send_dhcpSnoopingIntfErrorDisabled_trap(ifIndex);
}
 

int
snmp_daiIntfErrorDisabledTrapSend(L7_uint32 ifIndex)
{
  return send_daiIntfErrorDisabled_trap(ifIndex);
}

/*********************************************************************
*
* @purpose  Creates an User login trap OID and a VarBind structure and sends
*           them as a Management Interface trap.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t send_UserLoginTrap_trap()
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("failedUserLoginTrap");

  if (snmpTrapOID == NULL)
  {
    return L7_FAILURE;
  }

  status = SendNotificationsSMIv2Params(snmpTrapOID, NULL, NULL);

  FreeOID(snmpTrapOID);

  if (status == 0)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Creates an User login trap OID and a VarBind structure and sends
*           them as a Management Interface trap.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t send_UserLockoutTrap_trap()
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("userLockoutTrap");

  if (snmpTrapOID == NULL)
  {
    return L7_FAILURE;
  }

  status = SendNotificationsSMIv2Params(snmpTrapOID, NULL, NULL);

  FreeOID(snmpTrapOID);

  if (status == 0)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Sends when an interface is error disabled by DAI on an
*           interface
*
* @param    ifIndex       interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t send_daiIntfErrorDisabled_trap(L7_uint32 ifIndex)
{
  L7_uint32 status;
  VarBind *temp_vb = NULL;
  OID *snmpTrapOID;
  OID *oid_ifIndex;
  VarBind *var_ifIndex;

  /* Making OIDs for daiIntfErrorDisabledTrap, ifIndex */
  snmpTrapOID = MakeOIDFromDot("daiIntfErrorDisabledTrap");

  oid_ifIndex = MakeOIDFromDot("ifIndex");

  /* Creating Var Binds */
  var_ifIndex = MakeVarBindWithValue(oid_ifIndex, NULL,
                                     INTEGER_TYPE, &ifIndex);
  FreeOID(oid_ifIndex);

  if (snmpTrapOID == NULL || var_ifIndex == NULL)
  {
    FreeOID(snmpTrapOID);
    FreeVarBind(var_ifIndex);
    return L7_FAILURE;
  }

  temp_vb = var_ifIndex;
  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  Sends when an interface is error disabled by DHCP Snooping
*           on an interface
*
* @param    ifIndex       interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t send_dhcpSnoopingIntfErrorDisabled_trap(L7_uint32 ifIndex)
{
  L7_uint32 status;
  VarBind *temp_vb = NULL;
  OID *snmpTrapOID;
  OID *oid_ifIndex;
  VarBind *var_ifIndex;

  /* Making OIDs for daiIntfErrorDisabledTrap, ifIndex */
  snmpTrapOID = MakeOIDFromDot("dhcpSnoopingIntfErrorDisabledTrap");

  oid_ifIndex = MakeOIDFromDot("ifIndex");

  /* Creating Var Binds */
  var_ifIndex = MakeVarBindWithValue(oid_ifIndex, NULL,
                                     INTEGER_TYPE, &ifIndex);
  FreeOID(oid_ifIndex);

  if (snmpTrapOID == NULL || var_ifIndex == NULL)
  {
    FreeOID(snmpTrapOID);
    FreeVarBind(var_ifIndex);
    return L7_FAILURE;
  }

  temp_vb = var_ifIndex;
  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

#ifdef FEAT_METRO_CPE_V1_0
/*********************************************************************
*
* @purpose  This Notification indicates there exists no startup config file.
*           This notification are sent every 120 seconds if startup-config file.
*           does not exist and SSH is enabled.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmp_noStartupConfigNotificationTrapSend()
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("noStartupConfigTrap");

  if (snmpTrapOID == NULL)
  {
    return L7_FAILURE;
  }

  status = SendNotificationsSMIv2Params(snmpTrapOID, NULL, NULL);

  FreeOID(snmpTrapOID);

  if (status == 0)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
/*    return send_noStartupConfig_trap(macAddr, ifIndex, ifIndex, NULL, NULL);*/
}
#endif
