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
 *
 */



#include "sr_conf.h"


#include <stdio.h>

#include <stdlib.h>

#include <errno.h>

#include <sys/types.h>





#include <string.h>

#include <malloc.h>

#include <netinet/in.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "oid_lib.h"
#include "ntfy_oid.h"
#include "ntfy_cvt.h"
/* Put this back if you start using DPRINTF in this file */
#include "diag.h"
SR_FILENAME

#include "sr_ip.h"    /* for GetLocalIPAddress() */



extern SR_UINT32 GetLocalIPAddress(void);

Pdu *
SrConvertV1NotificationToV2Notification(
    const Pdu *v1_pdu)
{
    Pdu *v2_pdu = NULL;
    VarBind *vb1 = NULL;
    VarBind *vb2 = NULL;
    VarBind *vb3 = NULL;
    VarBind *tvb = NULL;
    OID *snmpTrapOID = NULL;
    OID *snmpTrapEnterprise = NULL;
    SR_INT32 curtime;

    switch (v1_pdu->u.trappdu.generic_trap) {
        case COLD_START_TRAP:
            snmpTrapOID = CloneOID(coldStartTrap);
            snmpTrapEnterprise = CloneOID(v1_pdu->u.trappdu.enterprise);
            if ((snmpTrapOID == NULL) || (snmpTrapEnterprise == NULL)) {
                goto fail;
            }
            break;
        case WARM_START_TRAP:
            snmpTrapOID = CloneOID(warmStartTrap);
            snmpTrapEnterprise = CloneOID(v1_pdu->u.trappdu.enterprise);
            if ((snmpTrapOID == NULL) || (snmpTrapEnterprise == NULL)) {
                goto fail;
            }
            break;
        case LINK_DOWN_TRAP:
            snmpTrapOID = CloneOID(linkDownTrap);
            snmpTrapEnterprise = CloneOID(v1_pdu->u.trappdu.enterprise);
            if ((snmpTrapOID == NULL) || (snmpTrapEnterprise == NULL)) {
                goto fail;
            }
            break;
        case LINK_UP_TRAP:
            snmpTrapOID = CloneOID(linkUpTrap);
            snmpTrapEnterprise = CloneOID(v1_pdu->u.trappdu.enterprise);
            if ((snmpTrapOID == NULL) || (snmpTrapEnterprise == NULL)) {
                goto fail;
            }
            break;
        case AUTHEN_FAILURE_TRAP:
            snmpTrapOID = CloneOID(authenFailureTrap);
            snmpTrapEnterprise = CloneOID(v1_pdu->u.trappdu.enterprise);
            if ((snmpTrapOID == NULL) || (snmpTrapEnterprise == NULL)) {
                goto fail;
            }
            break;
        case EGP_NEIGHBOR_LOSS_TRAP:
            snmpTrapOID = CloneOID(egpNeighborLossTrap);
            snmpTrapEnterprise = CloneOID(v1_pdu->u.trappdu.enterprise);
            if ((snmpTrapOID == NULL) || (snmpTrapEnterprise == NULL)) {
                goto fail;
            }
            break;
        case ENTERPRISE_TRAP:
            snmpTrapOID =
                MakeOID(NULL, v1_pdu->u.trappdu.enterprise->length + 2);
            if (snmpTrapOID == NULL) {
                goto fail;
            }
            memcpy(snmpTrapOID->oid_ptr,
                   v1_pdu->u.trappdu.enterprise->oid_ptr,
                   (size_t)(v1_pdu->u.trappdu.enterprise->length * sizeof(SR_UINT32)));
            snmpTrapOID->oid_ptr[snmpTrapOID->length - 1] =
                (SR_UINT32)v1_pdu->u.trappdu.specific_trap;
            break;
        default:
            goto fail;
    }

    v2_pdu = MakePdu(SNMPv2_TRAP_TYPE, 0L, 0L, 0L, NULL, NULL, 0L, 0L, 0L);
    if (v2_pdu == NULL) {
        goto fail;
    }

    if (v1_pdu->var_bind_list != NULL) {
        tvb = CloneVarBindList(v1_pdu->var_bind_list);
        if (tvb == NULL) {
            goto fail;
        }
    }

    curtime = (SR_INT32)v1_pdu->u.trappdu.time_ticks;
    vb1 = MakeVarBindWithValue(&sysUpTime_OID, NULL, TIME_TICKS_TYPE, &curtime);
    if (vb1 == NULL) {
        goto done;
    }

    vb2 = MakeVarBindWithValue(&snmpTrapOID_OID, NULL,
                               OBJECT_ID_TYPE, snmpTrapOID);
    if (vb2 == NULL) {
        goto done;
    }
    snmpTrapOID = NULL;

    if (snmpTrapEnterprise != NULL) {
        vb3 = MakeVarBindWithValue(&snmpTrapEnterprise_OID, NULL,
                                   OBJECT_ID_TYPE, snmpTrapEnterprise);
        if (vb3 == NULL) {
            goto done;
        }
        snmpTrapEnterprise = NULL;
    }

    vb1->next_var = vb2;
    vb2->next_var = tvb;
    tvb = vb1;
    vb1 = NULL;
    vb2 = NULL;

    v2_pdu->var_bind_list = tvb;
    if (tvb != NULL) {
        while (tvb->next_var != NULL) {
            tvb = tvb->next_var;
        }
    }
    v2_pdu->var_bind_end_ptr = tvb;
    tvb = NULL;

    if (v2_pdu->var_bind_end_ptr == NULL) {
        v2_pdu->var_bind_list = vb3;
        v2_pdu->var_bind_end_ptr = vb3;
    } else {
        v2_pdu->var_bind_end_ptr->next_var = vb3;
        v2_pdu->var_bind_end_ptr = vb3;
    }
    vb3 = NULL;

    goto done;

  fail:
    if (v2_pdu) {
        FreePdu(v2_pdu);
    }

  done:
    if (vb1) {
        FreeVarBindList(vb1);
    }
    if (vb2) {
        FreeVarBindList(vb2);
    }
    if (vb3) {
        FreeVarBindList(vb3);
    }
    if (tvb) {
        FreeVarBindList(tvb);
    }
    if (snmpTrapOID) {
        FreeOID(snmpTrapOID);
    }
    if (snmpTrapEnterprise) {
        FreeOID(snmpTrapEnterprise);
    }
    return v2_pdu;
}

Pdu *
SrConvertV2NotificationToV1Notification(
    const Pdu *v2_pdu,
    const OID *default_enterprise,
    const OctetString *agent_addr)
{
    VarBind *vb1, *vb2, *vb, *evb;
    VarBind *tvb = NULL;
    VarBind *loopVb = NULL;
    Pdu *v1_pdu = NULL;
    OID *enterprise = NULL;
    OctetString *localAddrOS = NULL;
    SR_INT32 genTrap;
    SR_INT32 specTrap;
    SR_UINT32 time_ticks;
    static SR_UINT32 _snmpTraps[] = { 1, 3, 6, 1, 6, 3, 1, 1, 5 };
    static OID snmpTraps = {
        sizeof(_snmpTraps) / sizeof(SR_UINT32),
       _snmpTraps };
    OID *defaultEnterprise = NULL;

    vb1 = v2_pdu->var_bind_list;
    vb2 = vb1->next_var;
    vb = vb2->next_var;
    evb = NULL;

    /*
     * Search for snmpTrapEnterprise.0 in the varbind list. If present,
     * save the varbind in evb. Also search for any COUNTER_64_TYPE varbinds
     * in the varbind list. If any are present, this notification cannot
     * be sent because COUNTER_64_TYPE is an SMIv2 type (SNMPv2c/SNMPv3).
     */
    if (vb != NULL) {
        for (loopVb = vb; loopVb != NULL; loopVb = loopVb->next_var) {
            if (!CmpOID(loopVb->name, &snmpTrapEnterprise_OID)) {
               evb = loopVb;
            }
            else if (loopVb->value.type == COUNTER_64_TYPE) {
                goto done;    
            }

        }
        tvb = CloneVarBindList(vb);
        if (tvb == NULL) {
            goto done;    
        }
    }

    time_ticks = (SR_UINT32)vb1->value.sl_value;

    specTrap = 0;

    if (CmpOID(vb2->value.oid_value, coldStartTrap) == 0) {
        genTrap = COLD_START_TRAP;
        if (evb != NULL) {
            defaultEnterprise = evb->value.oid_value;
        }
    } else
    if (CmpOID(vb2->value.oid_value, warmStartTrap) == 0) {
        genTrap = WARM_START_TRAP;
        if (evb != NULL) {
            defaultEnterprise = evb->value.oid_value;
        }
    } else
    if (CmpOID(vb2->value.oid_value, linkDownTrap) == 0) {
        genTrap = LINK_DOWN_TRAP;
        if (evb != NULL) {
            defaultEnterprise = evb->value.oid_value;
        }
    } else
    if (CmpOID(vb2->value.oid_value, linkUpTrap) == 0) {
        genTrap = LINK_UP_TRAP;
        if (evb != NULL) {
            defaultEnterprise = evb->value.oid_value;
        }
    } else
    if (CmpOID(vb2->value.oid_value, authenFailureTrap) == 0) {
        genTrap = AUTHEN_FAILURE_TRAP;
        if (evb != NULL) {
            defaultEnterprise = evb->value.oid_value;
        }
    } else
    if (CmpOID(vb2->value.oid_value, egpNeighborLossTrap) == 0) {
        genTrap = EGP_NEIGHBOR_LOSS_TRAP;
        if (evb != NULL) {
            defaultEnterprise = evb->value.oid_value;
        }
    } else {
        genTrap = ENTERPRISE_TRAP;
        enterprise = CloneOID(vb2->value.oid_value);
        if (enterprise == NULL) {
            goto done;
        }
        enterprise->length--;
        specTrap = enterprise->oid_ptr[enterprise->length];
        /* If next subid is 0, remove it as well */
        if (enterprise->oid_ptr[enterprise->length - 1] == 0) {
            enterprise->length--;
        }
    }

    /*
     * If we have not set the enterprise field yet, set it to the
     * value in the snmpTrapEnterprise varbind if possible
     * (defaultEnterprise). If this varbind does not exist, set
     * the enterprise to the OID value for snmpTraps.
     */

    if (enterprise == NULL) {
        if (defaultEnterprise != NULL) {
            enterprise = CloneOID(defaultEnterprise);
        }
        else {
            enterprise = CloneOID(&snmpTraps);
        }
        if (enterprise == NULL) {
            goto done;
        }
    }

    localAddrOS = CloneOctetString(agent_addr);
    if (localAddrOS == NULL) {
        goto done;
    }

    v1_pdu = MakePdu(TRAP_TYPE,
                     0L, 0L, 0L,
                     enterprise,
                     localAddrOS,
                     genTrap,
                     specTrap,
                     time_ticks);
    if (v1_pdu == NULL) {
        goto done;
    }
    enterprise = NULL;
    localAddrOS = NULL;

    v1_pdu->var_bind_list = tvb;
    if (tvb != NULL) {
        while (tvb->next_var != NULL) {
            tvb = tvb->next_var;
        }
    }
    v1_pdu->var_bind_end_ptr = tvb;
    tvb = NULL;

  done:
    if (tvb) {
        FreeVarBindList(tvb);
    }
    if (enterprise) {
        FreeOID(enterprise);
    }
    if (localAddrOS) {
        FreeOctetString(localAddrOS);
    }
    return v1_pdu;
}

