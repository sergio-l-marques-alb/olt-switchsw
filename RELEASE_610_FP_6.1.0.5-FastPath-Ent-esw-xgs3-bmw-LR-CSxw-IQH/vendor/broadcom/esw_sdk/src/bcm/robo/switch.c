/*
 * $Id: switch.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * General Switch Control
 */

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/drv.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/field.h>

#include <bcm/switch.h>
#include <bcm_int/robo/port.h>

#define DOS_ATTACK_TO_CPU         0x001
#define DOS_ATTACK_SIP_EQUAL_DIP  0x002
#define DOS_ATTACK_V4_FIRST_FRAG  0x004
#define DOS_ATTACK_TCP_FLAGS      0x008
#define DOS_ATTACK_L4_PORT        0x010
#define DOS_ATTACK_TCP_FRAG       0x020
#define DOS_ATTACK_PING_FLOOD 0x040
#define DOS_ATTACK_SYN_FLOOD  0x080
#define DOS_ATTACK_TCP_SMURF  0x100
#define DOS_ATTACK_TCP_FLAGS_SF  0x200
#define DOS_ATTACK_TCP_FLAGS_FUP  0x400
#define DOS_ATTACK_SYN_FRAG  0x800
#define DOS_ATTACK_FLAG_ZERO_SEQ_ZERO  0x1000

typedef enum dos_attack_case_e{
    DOS_ATTACK_CASE_EQUAL_IP,
    DOS_ATTACK_CASE_TCP_FLAG_1,
    DOS_ATTACK_CASE_TCP_FLAG_2,
    DOS_ATTACK_CASE_TCP_FLAG_3,
    DOS_ATTACK_CASE_TCP_FLAG_4,
    DOS_ATTACK_CASE_L4_PORT,
    DOS_ATTACK_CASE_TCP_FRAG,
    DOS_ATTACK_CASE_PING_FLOOD,
    DOS_ATTACK_CASE_SYN_FLOOD,
    DOS_ATTACK_CASE_SYNACK_FLOOD,
    DOS_ATTACK_CASE_TCP_SMURF,

    MAX_NUM_DOS_ATTACK_CASES
} dos_attack_case_t;

/* Per port DoS Attack data */
typedef struct dos_attack_e {
    uint32  attacks; /* bitmap of enabled DoS Attack cases */
    uint32  params[2]; /* for Ping/SYN flood rate limit value */
    int group_id;
    int entry_id[MAX_NUM_DOS_ATTACK_CASES];
} dos_attack_t;

dos_attack_t dos_attack_data[SOC_MAX_NUM_PORTS];

#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20

#define TCP_FLAG_MASK 0x3f
#define TCP_SEQ_NUM_MASK 0xffffffff /* seq num is 32 bits */

#define IP_PROTOCOL_ID_ICMP 0x1

uint8 min_tcp_hdr_size = 5; /* 5 * 4 = 20 bytes */

#define DOS_ATTACK_ALL_PORT -1

/*
 * Function:
 *      _bcm_robo_switch_control_gport_resolve
 * Description:
 *      Decodes local physical port from a gport
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      gport - a gport to decode
 *      port - (Out) Local physical port encoded in gport
 * Returns:
 *      BCM_E_xxxx
 * Note
 *      In case of gport contains other then local port error will be returned.
 */


STATIC int
_bcm_robo_switch_control_gport_resolve(
        int unit, bcm_gport_t gport, bcm_port_t *port)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    bcm_port_t      local_port;
    int             id, isMymodid;

    if (NULL == port) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_robo_gport_resolve(unit, gport, 
                &modid, &local_port, &tgid, &id));

    if ((BCM_TRUNK_INVALID != tgid) || (-1 != id)) {
        return BCM_E_PARAM;
    }
    /* Check if modid is local */
    BCM_IF_ERROR_RETURN(
        _bcm_robo_modid_is_local(unit, modid, &isMymodid));

    if (isMymodid != TRUE) {
        return BCM_E_PARAM;
    }

    return _bcm_robo_gport_modport_api2hw_map(
                   unit, modid, local_port, &modid, port);
}

int
_bcm_robo_dos_attack_set(int unit, bcm_port_t port, int dos_attack_type, int param)
{
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_field_range_t range;
    int x = 0, y = 0;
    int *entry_id;
    pbmp_t pbm, pbm_mask;
    uint32 entry_size;

    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_ASSIGN(pbm_mask, PBMP_ALL(unit));
    
    switch (dos_attack_type) {
        case bcmSwitchDosAttackToCpu:
            /* Get maximum entry number by chips */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->dev_prop_get)
                            (unit, DRV_DEV_PROP_CFP_TCAM_SIZE, &entry_size));

            /* Dynamically allocate memory for entry_id */
            entry_id = (int *)sal_alloc(entry_size * sizeof(int), "entry_id");
            if (entry_id == NULL) {
                soc_cm_print("Insufficient memory.\n");
                return BCM_E_MEMORY;
            }
            /* initialize to zero */
            sal_memset(entry_id, 0, entry_size * sizeof(int));
            
            if (param) {
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (!dos_attack_data[port].attacks) {
                        sal_free(entry_id); 
                        return BCM_E_UNAVAIL;
                    } else if (!(dos_attack_data[port].attacks & DOS_ATTACK_TO_CPU)) {
                        for (y = 0; y < MAX_NUM_DOS_ATTACK_CASES; y++) {
                            if (dos_attack_data[port].entry_id[y] != 0) {
                                entry = dos_attack_data[port].entry_id[y];
                                /* Prevent to change action for the same entry id */
                                if (entry_id[entry] == 0) {
                                    entry_id[entry] = entry;
                                    if ((y == DOS_ATTACK_CASE_PING_FLOOD) || 
                                        (y == DOS_ATTACK_CASE_SYN_FLOOD) || 
                                        (y == DOS_ATTACK_CASE_SYNACK_FLOOD)) {
                                        rv = bcm_field_action_remove(unit, entry, bcmFieldActionRpDrop);
                                    } else {
                                        rv = bcm_field_action_remove(unit, entry, bcmFieldActionDrop);
                                    }
                                    if (rv == BCM_E_NOT_FOUND) {
                                        dos_attack_data[port].entry_id[y] = 0;
                                    } else if (rv == BCM_E_NONE) {
                                        rv = bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0);
                                        if (rv < 0) {
                                            sal_free(entry_id);
                                            return rv;
                                        }
                                        rv = bcm_field_entry_install(unit, entry);
                                        if (rv < 0) {
                                            sal_free(entry_id);
                                            return rv;
                                        }
                                    } else {
                                        sal_free(entry_id); 
                                        return rv;
                                    }
                                }                                    
                            }
                        }
                        dos_attack_data[port].attacks |= DOS_ATTACK_TO_CPU;
                    }
                    sal_free(entry_id);
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        if (!dos_attack_data[x].attacks) {
                            sal_free(entry_id);
                            return BCM_E_UNAVAIL;
                        } else if (!(dos_attack_data[x].attacks & DOS_ATTACK_TO_CPU)) {
                            for (y = 0; y < MAX_NUM_DOS_ATTACK_CASES; y++) {
                                if (dos_attack_data[x].entry_id[y] != 0) {
                                    entry = dos_attack_data[x].entry_id[y];
                                    /* Prevent to change action for the same entry id */
                                    if (entry_id[entry] == 0) {
                                        entry_id[entry] = entry;
                                         if ((y == DOS_ATTACK_CASE_PING_FLOOD) || 
                                             (y == DOS_ATTACK_CASE_SYN_FLOOD) || 
                                             (y == DOS_ATTACK_CASE_SYNACK_FLOOD)) {
                                            rv = bcm_field_action_remove(unit, entry, bcmFieldActionRpDrop);
                                         } else {
                                            rv = bcm_field_action_remove(unit, entry, bcmFieldActionDrop);
                                         }
                                         if (rv == BCM_E_NOT_FOUND) {
                                             dos_attack_data[x].entry_id[y] = 0;
                                         }else if (rv == BCM_E_NONE) {
                                             rv = bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0);
                                             if (rv < 0) {
                                                 sal_free(entry_id);
                                                 return rv;
                                             }
                                             rv = bcm_field_entry_install(unit, entry);
                                             if (rv < 0) {
                                                 sal_free(entry_id);
                                                 return rv;
                                             }
                                         } else {
                                            sal_free(entry_id);
                                            return rv;
                                         }
                                    }
                                }
                            }
                            dos_attack_data[x].attacks |= DOS_ATTACK_TO_CPU;
                        }
                    }
                sal_free(entry_id);
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    if ((dos_attack_data[port].attacks & ~DOS_ATTACK_TO_CPU) &&
                        (dos_attack_data[port].attacks & DOS_ATTACK_TO_CPU)) {
                        for (y = 0; y < MAX_NUM_DOS_ATTACK_CASES; y++) {
                            if (dos_attack_data[port].entry_id[y] != 0) {
                                entry = dos_attack_data[port].entry_id[y];
                                /* Prevent to change action for the same entry id */
                                if (entry_id[entry] == 0) {
                                    entry_id[entry] = entry;
                                    rv = bcm_field_action_remove(unit, entry, bcmFieldActionCopyToCpu);
                                    if (rv == BCM_E_NOT_FOUND) {
                                        dos_attack_data[port].entry_id[y] = 0;
                                    } else if (rv == BCM_E_NONE) {
                                        if ((y == DOS_ATTACK_CASE_PING_FLOOD) || 
                                            (y == DOS_ATTACK_CASE_SYN_FLOOD) || 
                                            (y == DOS_ATTACK_CASE_SYNACK_FLOOD)) {
                                            rv = bcm_field_action_add(unit, entry, bcmFieldActionRpDrop, 0, 0);
                                            if (rv < 0) {
                                                sal_free(entry_id);
                                                return rv;
                                            }
                                        } else {
                                            rv = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
                                            if (rv < 0) {
                                                sal_free(entry_id);
                                                return rv;
                                            }
                                        }
                                        rv = bcm_field_entry_install(unit, entry);
                                        if (rv < 0) {
                                            sal_free(entry_id);
                                            return rv;
                                        }
                                    } else {
                                        sal_free(entry_id);
                                        return rv;
                                    }
                                }                                    
                            }
                        }
                        dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                    sal_free(entry_id);
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        if ((dos_attack_data[x].attacks & ~DOS_ATTACK_TO_CPU) &&
                            (dos_attack_data[x].attacks & DOS_ATTACK_TO_CPU)) {
                            for (y = 0; y < MAX_NUM_DOS_ATTACK_CASES; y++) {
                                if (dos_attack_data[x].entry_id[y] != 0) {
                                    entry = dos_attack_data[x].entry_id[y];
                                    /* Prevent to change action for the same entry id */
                                    if (entry_id[entry] == 0) {
                                        entry_id[entry] = entry;
                                        rv = bcm_field_action_remove(unit, entry, bcmFieldActionCopyToCpu);
                                        if (rv == BCM_E_NOT_FOUND) {
                                            dos_attack_data[x].entry_id[y] = 0;
                                        } else if (rv == BCM_E_NONE) {
                                            if ((y == DOS_ATTACK_CASE_PING_FLOOD) || 
                                                (y == DOS_ATTACK_CASE_SYN_FLOOD) || 
                                                (y == DOS_ATTACK_CASE_SYNACK_FLOOD)) {
                                                rv = bcm_field_action_add(unit, entry, bcmFieldActionRpDrop, 0, 0);
                                                if (rv < 0) {
                                                    sal_free(entry_id);
                                                    return rv;
                                                }
                                            } else {
                                                rv = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
                                                if (rv < 0) {
                                                    sal_free(entry_id);
                                                    return rv;
                                                }
                                            }
                                            rv = bcm_field_entry_install(unit, entry);
                                            if (rv < 0) {
                                                sal_free(entry_id);
                                                return rv;
                                            }
                                        } else {
                                            sal_free(entry_id);
                                            return rv;
                                        }
                                    }
                                }
                            }
                            dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                        }
                    }
                    sal_free(entry_id);
                }
            }
            break;
        case bcmSwitchDosAttackSipEqualDip:
            if (SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                rv = BCM_E_UNAVAIL;
                return rv;
            }
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcIpEqualDstIp);
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }
                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_EQUAL_IP] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                        port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_EQUAL_IP] = entry;
                    }
                }
         
                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_SrcIpEqualDstIp(unit, entry, 0x1));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_SIP_EQUAL_DIP;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_SIP_EQUAL_DIP;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_EQUAL_IP];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_EQUAL_IP] = 0;
                        dos_attack_data[port].attacks &= ~DOS_ATTACK_SIP_EQUAL_DIP;
                    } else {
                        return rv;
                    }
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_EQUAL_IP];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                            dos_attack_data[x].entry_id[DOS_ATTACK_CASE_EQUAL_IP] = 0;
                            dos_attack_data[x].attacks &= ~DOS_ATTACK_SIP_EQUAL_DIP;
                        }
                    } else {
                        return rv;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackMinTcpHdrSize:
            /* Setting min TCP header size */
            min_tcp_hdr_size = param;
            break;
        case bcmSwitchDosAttackV4FirstFrag:
            if (param) {
                for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                    dos_attack_data[x].attacks |= DOS_ATTACK_V4_FIRST_FRAG;
                }
            } else {
                for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                    dos_attack_data[x].attacks &= ~DOS_ATTACK_V4_FIRST_FRAG;
                }
            }
            break;
        case bcmSwitchDosAttackTcpFlags:
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTcpControl);
                if (!(SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit))) {
                    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyRangeCheck);
                }
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }

                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }

                /*
                 * Case 1: TCP SYN flag = 1 & source port < 1024
                 */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                        port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, TCP_FLAG_SYN, TCP_FLAG_MASK));
                if (!(SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit))) {
                    BCM_IF_ERROR_RETURN(
                        bcm_field_range_create(unit, &range, BCM_FIELD_RANGE_SRCPORT, 0, 1023));
                    BCM_IF_ERROR_RETURN(
                        bcm_field_qualify_RangeCheck(unit, entry, range, 0));
                }
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                /*
                 * Case 2: TCP flag = 0
                 */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, 0x0, TCP_FLAG_MASK));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                /*
                 * Case 3: TCP FIN, URG, PSH = 1
                 */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, 
                                             TCP_FLAG_FIN | TCP_FLAG_URG | TCP_FLAG_PSH, 
                                             TCP_FLAG_MASK));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                /*
                 * Case 4: TCP SYN and FIN = 1
                 */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, 
                                             TCP_FLAG_SYN | TCP_FLAG_FIN, 
                                             TCP_FLAG_MASK));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_TCP_FLAGS;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_TCP_FLAGS;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1] = 0;
                    } else {
                        return rv;
                    }

                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2] = 0;
                    } else {
                        return rv;
                    }

                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3];
                        if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3] = 0;
                    } else {
                        return rv;
                    }

                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4] = 0;
                    } else {
                        return rv;
                    }

                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TCP_FLAGS;
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                        return rv;
                    }
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                        return rv;
                    }
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                        return rv;
                    }
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                        return rv;
                    }

                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1] = 0;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2] = 0;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3] = 0;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4] = 0;

                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TCP_FLAGS;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackTcpFlagsSF:
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTcpControl);
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }

                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }

                /*
                 * Case 4: TCP SYN and FIN = 1
                 */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, 
                                             TCP_FLAG_SYN | TCP_FLAG_FIN, 
                                             TCP_FLAG_MASK));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_TCP_FLAGS_SF;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_TCP_FLAGS_SF;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4] = 0;

                        dos_attack_data[port].attacks &= ~DOS_ATTACK_TCP_FLAGS_SF;
                    } else {
                        return rv;
                    }
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                            dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_4] = 0;

                            dos_attack_data[x].attacks &= ~DOS_ATTACK_TCP_FLAGS_SF;
                        }
                    } else {
                        return rv;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackTcpFlagsFUP:
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTcpControl);
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }

                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }

                /*
                 * Case 3: TCP FIN, URG, PSH = 1
                 */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, 
                                             TCP_FLAG_FIN | TCP_FLAG_URG | TCP_FLAG_PSH, 
                                             TCP_FLAG_MASK));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_TCP_FLAGS_FUP;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_TCP_FLAGS_FUP;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3] = 0;

                        dos_attack_data[port].attacks &= ~DOS_ATTACK_TCP_FLAGS_FUP;
                    } else {
                        return rv;
                    }
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                            dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_3] = 0;

                            dos_attack_data[x].attacks &= ~DOS_ATTACK_TCP_FLAGS_FUP;
                        }
                    } else {
                        return rv;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackSynFrag:
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTcpControl);
                if (!(SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit))) {
                    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyRangeCheck);
                }
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }

                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }

                /*
                 * Case 1: TCP SYN flag = 1 & source port < 1024
                 */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                        port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, TCP_FLAG_SYN, TCP_FLAG_MASK));
                if (!(SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit))) {
                    BCM_IF_ERROR_RETURN(
                        bcm_field_range_create(unit, &range, BCM_FIELD_RANGE_SRCPORT, 0, 1023));
                    BCM_IF_ERROR_RETURN(
                        bcm_field_qualify_RangeCheck(unit, entry, range, 0));
                }
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_SYN_FRAG;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_SYN_FRAG;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1] = 0;

                        dos_attack_data[port].attacks &= ~DOS_ATTACK_SYN_FRAG;
                    } else {
                        return rv;
                    }
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                            dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_1] = 0;

                            dos_attack_data[x].attacks &= ~DOS_ATTACK_SYN_FRAG;
                        }
                    } else {
                        return rv;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackFlagZeroSeqZero:
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTcpControl);
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }

                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }

                /*
                 * Case 2: TCP flag = 0
                 */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, 0x0, TCP_FLAG_MASK));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_FLAG_ZERO_SEQ_ZERO;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_FLAG_ZERO_SEQ_ZERO;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2] = 0;

                        dos_attack_data[port].attacks &= ~DOS_ATTACK_FLAG_ZERO_SEQ_ZERO;
                    } else {
                        return rv;
                    }
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                            dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FLAG_2] = 0;

                            dos_attack_data[x].attacks &= ~DOS_ATTACK_FLAG_ZERO_SEQ_ZERO;
                        }
                    } else {
                        return rv;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackL4Port:
            if (SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                rv = BCM_E_UNAVAIL;
                return rv;
            }
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEqualL4Port);
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }
                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_L4_PORT] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_L4_PORT] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_EqualL4Port(unit, entry, 0x1));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_L4_PORT;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_L4_PORT;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_L4_PORT];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_L4_PORT] = 0;
                        dos_attack_data[port].attacks &= ~DOS_ATTACK_L4_PORT;
                    } else {
                        return rv;
                    }
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_L4_PORT];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                            dos_attack_data[x].entry_id[DOS_ATTACK_CASE_L4_PORT] = 0;
                            dos_attack_data[x].attacks &= ~DOS_ATTACK_L4_PORT;
                        }
                    } else {
                        return rv;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackTcpFrag:
            if (SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                rv = BCM_E_UNAVAIL;
                return rv;
            }
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTcpHeaderSize);
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }
                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FRAG] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FRAG] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpHeaderSize(unit, entry, min_tcp_hdr_size, 0xf));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_TCP_FRAG;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_TCP_FRAG;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FRAG];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_FRAG] = 0;
                        dos_attack_data[port].attacks &= ~DOS_ATTACK_TCP_FRAG;
                    } else {
                        return rv;
                    }
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_FRAG];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                            dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_FRAG] = 0;
                            dos_attack_data[x].attacks &= ~DOS_ATTACK_TCP_FRAG;
                        }
                    } else {
                        return rv;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackPingFlood:
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }

                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_PING_FLOOD] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_PING_FLOOD] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_IpProtocol(unit, entry, IP_PROTOCOL_ID_ICMP, 0xff));
                BCM_IF_ERROR_RETURN(
                    bcm_field_meter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_meter_set(unit, entry, BCM_FIELD_METER_COMMITTED, 64, 1024));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionRpDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_PING_FLOOD;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_PING_FLOOD;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_PING_FLOOD];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_PING_FLOOD] = 0;
                        dos_attack_data[port].attacks &= ~DOS_ATTACK_PING_FLOOD;
                    } else {
                        return rv;
                    }
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_PING_FLOOD];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                            dos_attack_data[x].entry_id[DOS_ATTACK_CASE_PING_FLOOD] = 0;
                            dos_attack_data[x].attacks &= ~DOS_ATTACK_PING_FLOOD;
                        }
                    } else {
                        return rv;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackSynFlood:
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTcpControl);
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }

                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }
                /* SYN Flood */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_SYN_FLOOD] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_SYN_FLOOD] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, TCP_FLAG_SYN, TCP_FLAG_MASK));
                BCM_IF_ERROR_RETURN(
                    bcm_field_meter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_meter_set(unit, entry, BCM_FIELD_METER_COMMITTED, 64, 1024));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionRpDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                /* SYN-ACK Flood */
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_SYNACK_FLOOD] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_SYNACK_FLOOD] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_TcpControl(unit, entry, \
                        TCP_FLAG_SYN|TCP_FLAG_ACK, TCP_FLAG_MASK));
                BCM_IF_ERROR_RETURN(
                    bcm_field_meter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_meter_set(unit, entry, BCM_FIELD_METER_COMMITTED, 64, 1024));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionRpDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_SYN_FLOOD;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_SYN_FLOOD;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_SYN_FLOOD];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_SYN_FLOOD] = 0;
                    } else {
                        return rv;
                    }

                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_SYNACK_FLOOD];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_SYNACK_FLOOD] = 0;
                    } else {
                        return rv;
                    }
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_SYN_FLOOD;
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_SYN_FLOOD];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                        return rv;
                    }

                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_SYNACK_FLOOD];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                        return rv;
                    }

                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_SYN_FLOOD] = 0;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_SYNACK_FLOOD] = 0;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_SYN_FLOOD;
                    }
                }
            }
            break;
        case bcmSwitchDosAttackTcpSmurf:
            if (param) {
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp);
                if (port != DOS_ATTACK_ALL_PORT) {
                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
                    }
                }

                rv = bcm_field_group_create(unit, qset, 
                    BCM_FIELD_GROUP_PRIO_ANY, &group);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_create(unit, group, &entry);
                if (rv < 0) {
                    return rv;
                }
                rv = bcm_field_entry_prio_set(unit, entry, 
                    BCM_FIELD_ENTRY_PRIO_HIGHEST);
                if (rv < 0) {
                    return rv;
                }

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].group_id = group;
                    dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_SMURF] = entry;

                    if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_SrcPort
                            (unit, entry, 0, BCM_FIELD_EXACT_MATCH_MASK,
                            port, BCM_FIELD_EXACT_MATCH_MASK));
                    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
                        || SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                        BCM_PBMP_PORT_ADD(pbm, port);
                        BCM_IF_ERROR_RETURN(
                            bcm_field_qualify_InPorts
                            (unit, entry, pbm, pbm_mask));
                    }
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].group_id = group;
                        dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_SMURF] = entry;
                    }
                }

                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_IpProtocol(unit, entry, IP_PROTOCOL_ID_ICMP, 0xff));
                BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_DstIp(unit, entry, 0xff, 0xff));
                BCM_IF_ERROR_RETURN(
                    bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
                BCM_IF_ERROR_RETURN(
                    bcm_field_counter_create(unit, entry));
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_install(unit, entry));

                if (port != DOS_ATTACK_ALL_PORT) {
                    dos_attack_data[port].attacks |= DOS_ATTACK_TCP_SMURF;
                    dos_attack_data[port].attacks &= ~DOS_ATTACK_TO_CPU;
                } else {
                    for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                        dos_attack_data[x].attacks |= DOS_ATTACK_TCP_SMURF;
                        dos_attack_data[x].attacks &= ~DOS_ATTACK_TO_CPU;
                    }
                }
            } else {
                if (port != DOS_ATTACK_ALL_PORT) {
                    entry = 
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_SMURF];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        dos_attack_data[port].entry_id[DOS_ATTACK_CASE_TCP_SMURF] = 0;
                        dos_attack_data[port].attacks &= ~DOS_ATTACK_TCP_SMURF;
                    } else {
                        return rv;
                    }
                } else {
                    /*
                     * Since DOS_ATTACK_ALL_PORT set all port's data base same
                     * value, retrieve port 0's entry id to remove entry.
                     */
                    entry = 
                        dos_attack_data[0].entry_id[DOS_ATTACK_CASE_TCP_SMURF];
                    if (entry) {
                        rv = bcm_field_entry_destroy(unit, entry);
                    }
                    if ((rv == BCM_E_NONE) || (rv == BCM_E_NOT_FOUND)) {
                        for (x = 0; x < SOC_MAX_NUM_PORTS; x++) {
                            dos_attack_data[x].entry_id[DOS_ATTACK_CASE_TCP_SMURF] = 0;
                            dos_attack_data[x].attacks &= ~DOS_ATTACK_TCP_SMURF;
                        }
                    } else {
                        return rv;
                    }
                }
            }
            break;
        default:
            return BCM_E_UNAVAIL;
    } 

    return BCM_E_NONE;
}

int
_bcm_robo_dos_attack_get(int unit, bcm_port_t port, int dos_attack_type, int *param)
{
    bcm_port_t query_port;

    if (port == DOS_ATTACK_ALL_PORT) {
        /* if query all ports' status, return status of port #0 */
        query_port = 0;
    } else {
        query_port = port;
    }

    switch(dos_attack_type) {
        case bcmSwitchDosAttackToCpu:
            /* 
             * A primary check point for DoS Attack?
             */
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_TO_CPU) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackSipEqualDip:
            if (SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                return BCM_E_UNAVAIL;
            }
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_SIP_EQUAL_DIP) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackMinTcpHdrSize:
            *param = min_tcp_hdr_size;
            break;
        case bcmSwitchDosAttackV4FirstFrag:
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_V4_FIRST_FRAG) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackTcpFlags:
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_TCP_FLAGS) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackTcpFlagsSF:
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_TCP_FLAGS_SF) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackTcpFlagsFUP:
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_TCP_FLAGS_FUP) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackSynFrag:
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_SYN_FRAG) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackFlagZeroSeqZero:
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_FLAG_ZERO_SEQ_ZERO) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackL4Port:
            if (SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                return BCM_E_UNAVAIL;
            }
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_L4_PORT) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackTcpFrag:
            if (SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
                return BCM_E_UNAVAIL;
            }
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_TCP_FRAG) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackPingFlood:
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_PING_FLOOD) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackSynFlood:
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_SYN_FLOOD) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        case bcmSwitchDosAttackTcpSmurf:
            if (dos_attack_data[query_port].attacks & DOS_ATTACK_TCP_SMURF) {
                *param = 1;
            } else {
                *param = 0;
            }
            break;
        default:
            return SOC_E_UNAVAIL;
    }
    
    return SOC_E_NONE;
}

int
_bcm_robo_hw_dos_attack_set(int unit, bcm_switch_control_t type, int arg)
{
    int rv = BCM_E_NONE;
    uint32 dos_type, temp;
    
    switch(type) {
        case bcmSwitchDosAttackMinTcpHdrSize:
            dos_type = DRV_DOS_MIN_TCP_HDR_SZ;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackSipEqualDip:
            dos_type = DRV_DOS_LAND;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackTcpFlags:
            /* 4 cases */
            dos_type = DRV_DOS_SYN_WITH_SP_LT1024;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            dos_type = DRV_DOS_SYN_FIN_SCAN;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            dos_type = DRV_DOS_XMASS_WITH_TCP_SEQ_ZERO;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            dos_type = DRV_DOS_NULL_WITH_TCP_SEQ_ZERO;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackSynFrag:
            dos_type = DRV_DOS_SYN_WITH_SP_LT1024;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackTcpFlagsSF:
            dos_type = DRV_DOS_SYN_FIN_SCAN;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackTcpFlagsFUP:
            dos_type = DRV_DOS_XMASS_WITH_TCP_SEQ_ZERO;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackFlagZeroSeqZero:
            dos_type = DRV_DOS_NULL_WITH_TCP_SEQ_ZERO;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackL4Port:
            dos_type = DRV_DOS_BLAT;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackTcpPortsEqual:
            dos_type = DRV_DOS_TCP_BLAT;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackUdpPortsEqual:
            dos_type = DRV_DOS_UDP_BLAT;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackTcpFrag:
            /* Apply to set global value min_tcp_hdr_size to register first */
            dos_type = DRV_DOS_MIN_TCP_HDR_SZ;
            temp = min_tcp_hdr_size;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            dos_type = DRV_DOS_TCP_FRAG;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackTcpOffset:
            dos_type = DRV_DOS_TCP_FRAG_OFFSET;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackTcpSmurf:
            dos_type = DRV_DOS_SMURF;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackIcmpFragments:
            /* 2 cases */
            dos_type = DRV_DOS_ICMPV4_FRAGMENTS;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            dos_type = DRV_DOS_ICMPV6_FRAGMENTS;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackIcmpPktOversize:
            dos_type = DRV_DOS_MAX_ICMPV4_SIZE;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackIcmpV6PingSize:
            dos_type = DRV_DOS_MAX_ICMPV6_SIZE;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        case bcmSwitchDosAttackIcmp:
            /* 2 cases */
            /* Apply to set global value max_icmpv4_size to register first */
            dos_type = DRV_DOS_ICMPV4_LONG_PING;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            /* Apply to set global value max_icmpv6_size to register first */
            dos_type = DRV_DOS_ICMPV6_LONG_PING;
            temp = arg;
            rv = ((DRV_SERVICES(unit)->dos_enable_set)
                (unit, dos_type, temp));
            break;
        default:
            rv = BCM_E_UNAVAIL;
    }

    return rv;
}

int
_bcm_robo_hw_dos_attack_get(int unit, bcm_switch_control_t type, int *arg)
{
    int rv = BCM_E_NONE;
    uint32 dos_type, temp;
    
    switch(type) {
        case bcmSwitchDosAttackMinTcpHdrSize:
            dos_type = DRV_DOS_MIN_TCP_HDR_SZ;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackSipEqualDip:
            dos_type = DRV_DOS_LAND;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackTcpFlags:
            /* 4 cases */
            /* Just need to get one of the cases */
            dos_type = DRV_DOS_SYN_WITH_SP_LT1024;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackSynFrag:
            dos_type = DRV_DOS_SYN_WITH_SP_LT1024;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackTcpFlagsSF:
            dos_type = DRV_DOS_SYN_FIN_SCAN;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackTcpFlagsFUP:
            dos_type = DRV_DOS_XMASS_WITH_TCP_SEQ_ZERO;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackFlagZeroSeqZero:
            dos_type = DRV_DOS_NULL_WITH_TCP_SEQ_ZERO;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackL4Port:
            dos_type = DRV_DOS_BLAT;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
             *arg = temp;
            break;
        case bcmSwitchDosAttackTcpPortsEqual:
            dos_type = DRV_DOS_TCP_BLAT;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
             *arg = temp;
            break;
        case bcmSwitchDosAttackUdpPortsEqual:
            dos_type = DRV_DOS_UDP_BLAT;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
             *arg = temp;
            break;
        case bcmSwitchDosAttackTcpFrag:
            dos_type = DRV_DOS_TCP_FRAG;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
             *arg = temp;
            break;
        case bcmSwitchDosAttackTcpOffset:
            dos_type = DRV_DOS_TCP_FRAG_OFFSET;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
             *arg = temp;
            break;
        case bcmSwitchDosAttackTcpSmurf:
            dos_type = DRV_DOS_SMURF;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
             *arg = temp;
            break;
        case bcmSwitchDosAttackIcmpFragments:
            /* 2 cases */
            /* Just need to get one of the cases */
            dos_type = DRV_DOS_ICMPV4_FRAGMENTS;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackIcmpPktOversize:
            dos_type = DRV_DOS_MAX_ICMPV4_SIZE;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackIcmpV6PingSize:
            dos_type = DRV_DOS_MAX_ICMPV6_SIZE;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        case bcmSwitchDosAttackIcmp:
            /* 2 cases */
            /* Just need to get one of the cases */
            dos_type = DRV_DOS_ICMPV4_LONG_PING;
            rv = ((DRV_SERVICES(unit)->dos_enable_get)
                (unit, dos_type, &temp));
            *arg = temp;
            break;
        default:
            rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *  bcm_robo_switch_control_set
 * Description:
 *  Set traffic-to-CPU configuration parameters for the switch.
 *      All switch chip ports will be configured with the same settings.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to modify.
 *      arg - The value with which to set the parameter.
 * Returns:
 *      BCM_E_xxxx 
 */
int 
bcm_robo_switch_control_set(int unit,
                  bcm_switch_control_t type,
                  int arg)
{
    uint32  type_mask = 0;
    uint32  snoop_type_mask = 0;
    int rv = BCM_E_UNAVAIL;
    soc_port_t port = 0;
    soc_pbmp_t bmp;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : %s(),type=%d,arg=%d\n", __func__, type, arg);

    BCM_PBMP_CLEAR(bmp);

    /* Process DoS Attack cases */
    switch(type) {
        /* global flags and variable setting */
        case bcmSwitchDosAttackMinTcpHdrSize:
        case bcmSwitchDosAttackV4FirstFrag:
            if (soc_feature(unit, soc_feature_hw_dos_prev) ||
                soc_feature(unit, soc_feature_field)) {
                return(_bcm_robo_dos_attack_set(unit, DOS_ATTACK_ALL_PORT, type, arg));
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        /* apply to all front ports */
        case bcmSwitchDosAttackToCpu:
        case bcmSwitchDosAttackSipEqualDip:
        case bcmSwitchDosAttackTcpFlags:
        case bcmSwitchDosAttackTcpFlagsSF:
        case bcmSwitchDosAttackTcpFlagsFUP:
        case bcmSwitchDosAttackSynFrag:
        case bcmSwitchDosAttackFlagZeroSeqZero:
        case bcmSwitchDosAttackL4Port:
        case bcmSwitchDosAttackTcpPortsEqual:
        case bcmSwitchDosAttackUdpPortsEqual:
        case bcmSwitchDosAttackTcpFrag:
        case bcmSwitchDosAttackTcpOffset:
        case bcmSwitchDosAttackIcmpFragments:
        case bcmSwitchDosAttackIcmpPktOversize:
        case bcmSwitchDosAttackIcmpV6PingSize:
        case bcmSwitchDosAttackIcmp:
        case bcmSwitchDosAttackPingFlood:
        case bcmSwitchDosAttackSynFlood:
        case bcmSwitchDosAttackTcpSmurf:
            if (soc_feature(unit, soc_feature_hw_dos_prev)) {
                rv = _bcm_robo_hw_dos_attack_set(unit, type, arg);
            }
            if ((rv == BCM_E_UNAVAIL) && 
                (soc_feature(unit, soc_feature_field))) {
                rv = _bcm_robo_dos_attack_set(unit, DOS_ATTACK_ALL_PORT, type, arg);
            }
            return rv;
            break;
        default:
            break;
    }

    /* get trap configuring status */
    BCM_PBMP_ASSIGN(bmp, PBMP_ALL(unit));
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->trap_get)
                    (unit, port, &type_mask));
    
    /* bcm53115, bcm53118 new feature on supporting different CPU snooping/traping 
     * behavior per separated the IGMP/MLD packet types.
     *
     * The designed process for this new feature are :
     *  1. set TRUE at bcmSwitchxxxxToCPU in bcm53115, bcm53118 :
     *      - Do nothing if "toCPU" is set already!(can be snoop||trap)
     *      - else set to SNOOPING by default behavior.
     *  2. set FALSE at bcmSwitchxxxxToCPU in bcm53115, bcm53118 :
     *      - set to disable on SNOOPING and TRAPING by default behavior.
     *  3. set TRUE at bcmSwitchxxxxDrop in bcm53115, bcm53118 :
     *      - Return BCM_E_UNAVAIL if "toCPU" is not set for chip limitation!
     *          (i.e. neither snooping nor traping)
     *      - Do nothing if "TRAPING" is set already!
     *      - else set to "TRAPING" by default behavior.
     *  4. set FALSE at bcmSwitchxxxxDrop in bcm53115, bcm53118 :
     *      - Do nothing if "toCPU" is reset or if "SNOOPING" is set already!
     *      - else set to SNOOPING by default behavior.
     */ 
    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)){
        int     snoop_action = FALSE;
        int     trap_action = FALSE;
        
        /* get snoop configuring status */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->snoop_get)
                        (unit, &snoop_type_mask));
        
        switch(type) {
        /* IGMP snooping sections */
        case bcmSwitchIgmpToCPU:
            if (arg){
                if ((snoop_type_mask & DRV_SNOOP_IGMP) || 
                        (type_mask & DRV_SWITCH_TRAP_IGMP) ||
                        (((snoop_type_mask & DRV_SNOOP_IGMP_QUERY) ||
                                (type_mask & DRV_SWITCH_TRAP_IGMP_QUERY)) &&
                            ((snoop_type_mask & DRV_SNOOP_IGMP_REPLEV) ||
                                (type_mask & DRV_SWITCH_TRAP_IGMP_REPLEV)) &&
                            ((snoop_type_mask & DRV_SNOOP_IGMP_UNKNOW) ||
                                (type_mask & DRV_SWITCH_TRAP_IGMP_UNKNOW)))){
                                    
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    snoop_type_mask |= DRV_SNOOP_IGMP;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_DISABLE;
                }
            } else {
                
                snoop_type_mask &= ~DRV_SNOOP_IGMP;
                /* our design for chip limitation : 
                 *  - set "DRV_SNOOP_IGMP_DISABLE" to disable all type of 
                 *      IGMP trap/snoop .
                 */
                snoop_type_mask |= DRV_SNOOP_IGMP_DISABLE;
            }
            snoop_action = TRUE;
            break;
        case bcmSwitchIgmpPktDrop:
            if (arg){
                if(type_mask & DRV_SWITCH_TRAP_IGMP){
                    /* current setting is traping already */
                    return  BCM_E_NONE;
                } else {
                    /* all IGMP packet types will be set to Trap here 
                     * (no matter if there is one (or two) IGMP type been 
                     *  configured as snoop/trap/disable.) 
                     */
                    type_mask |= DRV_SWITCH_TRAP_IGMP;
                    type_mask &= ~DRV_SWITCH_TRAP_IGMP_DISABLE;
                    
                    trap_action = TRUE;
                }
            } else {
                if ((type_mask & DRV_SWITCH_TRAP_IGMP_DISABLE) || 
                        (snoop_type_mask & DRV_SNOOP_IGMP) || 
                        (snoop_type_mask & DRV_SNOOP_IGMP_DISABLE)){
                    /* current setting is snooping or IGMP packet control
                     * (means toCPU) is disabled already.
                     *  (bcm53115, bcm53118 have no drop feature originally when IGMP 
                     *  control disabled.)
                     */
                    return  BCM_E_NONE;
                } else {
                    /* all IGMP packet types will be set to Snoop here 
                     * (no matter if there is one (or two) IGMP type been 
                     *  configured as snoop/trap/disable.) 
                     */
                     
                    snoop_type_mask |= DRV_SNOOP_IGMP;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_DISABLE;
                    
                    snoop_action = TRUE;
                }
            }
            break;

        case bcmSwitchIgmpQueryToCpu:
            if (arg){
                if ((snoop_type_mask & DRV_SNOOP_IGMP) ||
                        (type_mask & DRV_SWITCH_TRAP_IGMP) ||
                        ((snoop_type_mask & DRV_SNOOP_IGMP_QUERY) ||
                            (type_mask & DRV_SWITCH_TRAP_IGMP_QUERY))){
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    snoop_type_mask |= DRV_SNOOP_IGMP_QUERY;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_QUERY_DISABLE;
                }
            } else {
                snoop_type_mask &= ~DRV_SNOOP_IGMP_QUERY;
                /* our design for chip limitation : 
                 *  - set "DRV_SNOOP_IGMP_QUERY_DISABLE" to disable IGMP
                 *       trap/snoop on query type.
                 */
                snoop_type_mask |= DRV_SNOOP_IGMP_QUERY_DISABLE;
                
            }
            snoop_action = TRUE;
            break;

        case bcmSwitchIgmpQueryDrop:
            if (arg){
                if((type_mask & DRV_SWITCH_TRAP_IGMP) ||
                        (type_mask & DRV_SWITCH_TRAP_IGMP_QUERY)){
                    /* current setting is traping already */
                    return  BCM_E_NONE;
                } else {
                    /* IGMP query packet types will be set to Trap here 
                     * (no matter if there is one (or two) IGMP type been 
                     *  configured as snoop/trap/disable.) 
                     */
                    type_mask |= DRV_SWITCH_TRAP_IGMP_QUERY;
                    type_mask &= ~DRV_SWITCH_TRAP_IGMP_DISABLE;
                    
                    trap_action = TRUE;
                }
            } else {
                if ((type_mask & DRV_SWITCH_TRAP_IGMP_DISABLE) || 
                        (snoop_type_mask & DRV_SNOOP_IGMP) || 
                        (snoop_type_mask & DRV_SNOOP_IGMP_QUERY) ||
                        (snoop_type_mask & DRV_SNOOP_IGMP_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_IGMP_QUERY_DISABLE)){
                    /* current setting is snooping or IGMP packet control
                     * (means toCPU) is disabled already.
                     *  (bcm53115, bcm53118 have no drop feature originally when IGMP 
                     *  control disabled.)
                     */
                    return  BCM_E_NONE;
                } else {
                    /* IGMP query packet types will be set to Snoop here */
                     
                    snoop_type_mask |= DRV_SNOOP_IGMP_QUERY;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_QUERY_DISABLE;
                    
                    snoop_action = TRUE;
                }
            }
            break;

        case bcmSwitchIgmpReportLeaveToCpu:
            if (arg){
                if ((snoop_type_mask & DRV_SNOOP_IGMP) ||
                        (type_mask & DRV_SWITCH_TRAP_IGMP) ||
                        ((snoop_type_mask & DRV_SNOOP_IGMP_REPLEV) ||
                            (type_mask & DRV_SWITCH_TRAP_IGMP_REPLEV))){
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    snoop_type_mask |= DRV_SNOOP_IGMP_REPLEV;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_REPLEV_DISABLE;
                }
            } else {
                snoop_type_mask &= ~DRV_SNOOP_IGMP_REPLEV;
                /* our design for chip limitation : 
                 *  - set "DRV_SNOOP_IGMP_QUERY_DISABLE" to disable IGMP
                 *       trap/snoop on query type.
                 */
                snoop_type_mask |= DRV_SNOOP_IGMP_REPLEV_DISABLE;
                
            }
            snoop_action = TRUE;
            break;

        case bcmSwitchIgmpReportLeaveDrop:
            if (arg){
                if((type_mask & DRV_SWITCH_TRAP_IGMP) ||
                        (type_mask & DRV_SWITCH_TRAP_IGMP_REPLEV)){
                    /* current setting is traping already */
                    return  BCM_E_NONE;
                } else {
                    /* IGMP report/leave packet types will be set to Trap here 
                     * (no matter if there is one (or two) IGMP type been 
                     *  configured as snoop/trap/disable.) 
                     */
                    type_mask |= DRV_SWITCH_TRAP_IGMP_REPLEV;
                    type_mask &= ~DRV_SWITCH_TRAP_IGMP_DISABLE;
                    
                    trap_action = TRUE;
                }
            } else {
                if ((type_mask & DRV_SWITCH_TRAP_IGMP_DISABLE) || 
                        (snoop_type_mask & DRV_SNOOP_IGMP) || 
                        (snoop_type_mask & DRV_SNOOP_IGMP_REPLEV) ||
                        (snoop_type_mask & DRV_SNOOP_IGMP_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_IGMP_REPLEV_DISABLE)){
                    /* current setting is snooping or IGMP packet control
                     * (means toCPU) is disabled already.
                     *  (bcm53115, bcm53118 have no drop feature originally when IGMP 
                     *  control disabled.)
                     */
                    return  BCM_E_NONE;
                } else {
                    /* IGMP report/leave packet types will be set to Snoop here */
                     
                    snoop_type_mask |= DRV_SNOOP_IGMP_REPLEV;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_REPLEV_DISABLE;
                    
                    snoop_action = TRUE;
                }
            }
            break;

        case bcmSwitchIgmpUnknownToCpu:
            if (arg){
                if ((snoop_type_mask & DRV_SNOOP_IGMP) ||
                        (type_mask & DRV_SWITCH_TRAP_IGMP) ||
                        ((snoop_type_mask & DRV_SNOOP_IGMP_UNKNOW) ||
                            (type_mask & DRV_SWITCH_TRAP_IGMP_UNKNOW))){
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    snoop_type_mask |= DRV_SNOOP_IGMP_UNKNOW;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_UNKNOW_DISABLE;
                }
            } else {
                snoop_type_mask &= ~DRV_SNOOP_IGMP_UNKNOW;
                /* our design for chip limitation : 
                 *  - set "DRV_SNOOP_IGMP_QUERY_DISABLE" to disable IGMP
                 *       trap/snoop on query type.
                 */
                snoop_type_mask |= DRV_SNOOP_IGMP_UNKNOW_DISABLE;
                
            }
            snoop_action = TRUE;
            break;

        case bcmSwitchIgmpUnknownDrop:
            if (arg){
                if ((snoop_type_mask & DRV_SNOOP_IGMP_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_IGMP_UNKNOW_DISABLE)) {
                    /* bcm53115, bcm53118 have no drop feature when IGMP packet control 
                     *  been disabled.
                     */
                    return  BCM_E_UNAVAIL;
                } else if((type_mask & DRV_SWITCH_TRAP_IGMP) ||
                        (type_mask & DRV_SWITCH_TRAP_IGMP_UNKNOW)){
                    /* current setting is traping already */
                    return  BCM_E_NONE;
                } else {
                    /* IGMP unknow packet types will be set to Trap here 
                     * (no matter if there is one (or two) IGMP type been 
                     *  configured as snoop/trap/disable.) 
                     */
                    type_mask |= DRV_SWITCH_TRAP_IGMP_UNKNOW;
                    type_mask &= ~DRV_SWITCH_TRAP_IGMP_DISABLE;
                    
                    trap_action = TRUE;
                }
            } else {
                if ((type_mask & DRV_SWITCH_TRAP_IGMP_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_IGMP) || 
                        (snoop_type_mask & DRV_SNOOP_IGMP_UNKNOW) ||
                        (snoop_type_mask & DRV_SNOOP_IGMP_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_IGMP_UNKNOW_DISABLE)){
                    /* current setting is snooping or IGMP packet control
                     * (means toCPU) is disabled already.
                     *  (bcm53115, bcm53118 have no drop feature originally when IGMP 
                     *  control disabled.)
                     */
                    return  BCM_E_NONE;
                } else {
                    /* IGMP unknow packet types will be set to Snoop here */
                     
                    snoop_type_mask |= DRV_SNOOP_IGMP_UNKNOW;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_IGMP_UNKNOW_DISABLE;
                    
                    snoop_action = TRUE;
                }
            }
            break;
        
        /* MLD sections */
        case bcmSwitchMldPktToCpu:
            if (arg){
                if ((snoop_type_mask & DRV_SNOOP_MLD) || 
                        (type_mask & DRV_SWITCH_TRAP_MLD) ||
                        (((snoop_type_mask & DRV_SNOOP_MLD_QUERY) ||
                                (type_mask & DRV_SWITCH_TRAP_MLD_QUERY)) &&
                            ((snoop_type_mask & DRV_SNOOP_MLD_REPDONE) ||
                                (type_mask & DRV_SWITCH_TRAP_MLD_REPDONE)))){
                                    
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    snoop_type_mask |= DRV_SNOOP_MLD;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_DISABLE;
                }
            } else {
                
                snoop_type_mask &= ~DRV_SNOOP_MLD;
                /* our design for chip limitation : 
                 *  - set "DRV_SNOOP_MLD_DISABLE" to disable all type of 
                 *      IGMP trap/snoop .
                 */
                snoop_type_mask |= DRV_SNOOP_MLD_DISABLE;
            }
            snoop_action = TRUE;
            break;
        case bcmSwitchMldPktDrop:
            if (arg){
                if(type_mask & DRV_SWITCH_TRAP_MLD){
                    /* current setting is traping already */
                    return  BCM_E_NONE;
                } else {
                    /* all MLD packet types will be set to Trap here 
                     * (no matter if there is one (or two) MLD type been 
                     *  configured as snoop/trap/disable.) 
                     */
                    type_mask |= DRV_SWITCH_TRAP_MLD;
                    type_mask &= ~DRV_SWITCH_TRAP_MLD_DISABLE;
                    
                    trap_action = TRUE;
                }
            } else {
                if ((type_mask & DRV_SWITCH_TRAP_IGMP_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_MLD) || 
                        (snoop_type_mask & DRV_SNOOP_MLD_DISABLE)){
                    /* current setting is snooping or MLD packet control
                     * (means toCPU) is disabled already.
                     *  (bcm53115, bcm53118 have no drop feature originally when MLD 
                     *  control disabled.)
                     */
                    return  BCM_E_NONE;
                } else {
                    /* all MLD packet types will be set to Snoop here 
                     * (no matter if there is one (or two) MLD type been 
                     *  configured as snoop/trap/disable.) 
                     */
                     
                    snoop_type_mask |= DRV_SNOOP_MLD;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_DISABLE;
                    
                    snoop_action = TRUE;
                }
            }
            break;

        case bcmSwitchMldQueryToCpu:
            if (arg){
                if ((snoop_type_mask & DRV_SNOOP_MLD) ||
                        (type_mask & DRV_SWITCH_TRAP_MLD) ||
                        ((snoop_type_mask & DRV_SNOOP_MLD_QUERY) ||
                            (type_mask & DRV_SWITCH_TRAP_MLD_QUERY))){
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    snoop_type_mask |= DRV_SNOOP_MLD_QUERY;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_QUERY_DISABLE;
                }
            } else {
                snoop_type_mask &= ~DRV_SNOOP_MLD_QUERY;
                /* our design for chip limitation : 
                 *  - set "DRV_SNOOP_MLD_QUERY_DISABLE" to disable MLD
                 *       trap/snoop on query type.
                 */
                snoop_type_mask |= DRV_SNOOP_MLD_QUERY_DISABLE;
                
            }
            snoop_action = TRUE;
            break;

        case bcmSwitchMldQueryDrop:
            if (arg){
                if((type_mask & DRV_SWITCH_TRAP_MLD) ||
                        (type_mask & DRV_SWITCH_TRAP_MLD_QUERY)){
                    /* current setting is traping already */
                    return  BCM_E_NONE;
                } else {
                    /* MLD query packet types will be set to Trap here 
                     * (no matter if there is one (or two) MLD type been 
                     *  configured as snoop/trap/disable.) 
                     */
                    type_mask |= DRV_SWITCH_TRAP_MLD_QUERY;
                    type_mask &= ~DRV_SWITCH_TRAP_MLD_DISABLE;
                    
                    trap_action = TRUE;
                }
            } else {
                if ((type_mask & DRV_SWITCH_TRAP_IGMP_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_MLD) || 
                        (snoop_type_mask & DRV_SNOOP_MLD_QUERY) ||
                        (snoop_type_mask & DRV_SNOOP_MLD_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_MLD_QUERY_DISABLE)){
                    /* current setting is snooping or MLD packet control
                     * (means toCPU) is disabled already.
                     *  (bcm53115, bcm53118 have no drop feature originally when MLD 
                     *  control disabled.)
                     */
                    return  BCM_E_NONE;
                } else {
                    /* MLD query packet types will be set to Snoop here */
                     
                    snoop_type_mask |= DRV_SNOOP_MLD_QUERY;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_QUERY_DISABLE;
                    
                    snoop_action = TRUE;
                }
            }
            break;

        case bcmSwitchMldReportDoneToCpu:
            if (arg){
                if ((snoop_type_mask & DRV_SNOOP_MLD) ||
                        (type_mask & DRV_SWITCH_TRAP_MLD) ||
                        ((snoop_type_mask & DRV_SNOOP_MLD_REPDONE) ||
                            (type_mask & DRV_SWITCH_TRAP_MLD_REPDONE))){
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    snoop_type_mask |= DRV_SNOOP_MLD_REPDONE;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_REPDONE_DISABLE;
                }
            } else {
                snoop_type_mask &= ~DRV_SNOOP_MLD_REPDONE;
                /* our design for chip limitation : 
                 *  - set "DRV_SNOOP_MLD_REPDONE_DISABLE" to disable MLD
                 *       trap/snoop on report/done type.
                 */
                snoop_type_mask |= DRV_SNOOP_MLD_REPDONE_DISABLE;
                
            }
            snoop_action = TRUE;
            break;

        case bcmSwitchMldReportDoneDrop:
            if (arg){
                if((type_mask & DRV_SWITCH_TRAP_MLD) ||
                        (type_mask & DRV_SWITCH_TRAP_MLD_REPDONE)){
                    /* current setting is traping already */
                    return  BCM_E_NONE;
                } else {
                    /* MLD report/done packet types will be set to Trap here 
                     * (no matter if there is one (or two) MLD type been 
                     *  configured as snoop/trap/disable.) 
                     */
                    type_mask |= DRV_SWITCH_TRAP_MLD_REPDONE;
                    type_mask &= ~DRV_SWITCH_TRAP_MLD_DISABLE;
                    
                    trap_action = TRUE;
                }
            } else {
                if ((type_mask & DRV_SWITCH_TRAP_IGMP_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_MLD) || 
                        (snoop_type_mask & DRV_SNOOP_MLD_REPDONE) ||
                        (snoop_type_mask & DRV_SNOOP_MLD_DISABLE) ||
                        (snoop_type_mask & DRV_SNOOP_MLD_REPDONE_DISABLE)){
                    /* current setting is snooping or MLD packet control
                     * (means toCPU) is disabled already.
                     *  (bcm53115, bcm53118 have no drop feature originally when MLD 
                     *  control disabled.)
                     */
                    return  BCM_E_NONE;
                } else {
                    /* MLD report/leave packet types will be set to Snoop here */
                     
                    snoop_type_mask |= DRV_SNOOP_MLD_REPDONE;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_DISABLE;
                    snoop_type_mask &= ~DRV_SNOOP_MLD_REPDONE_DISABLE;
                    
                    snoop_action = TRUE;
                }
            }
            break;
        
        /* other request */
        case bcmSwitchArpReplyToCpu:    /* ARP */
            if (arg){
                if (snoop_type_mask & DRV_SNOOP_ARP){
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    snoop_type_mask |= DRV_SNOOP_ARP;
                }
            } else {
                snoop_type_mask &= ~DRV_SNOOP_ARP;
            }
            snoop_action = TRUE;
            break;

        case bcmSwitchDhcpPktToCpu:     /* DHCP */
            if (arg){
                if (snoop_type_mask & DRV_SNOOP_DHCP){
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    snoop_type_mask |= DRV_SNOOP_DHCP;
                }
            } else {
                snoop_type_mask &= ~DRV_SNOOP_DHCP;
            }
            snoop_action = TRUE;
            break;

        case bcmSwitchIcmpRedirectToCpu:
            /* bcm53115, bcm53118 support redirect to CPU about ICMP is ICMPv6 only. 
             *  The ICMPv4 can only work at snoop but not trap feature.
             */
            if (arg){
                if (type_mask & DRV_SWITCH_TRAP_ICMPV6){
                    /* enabled already */
                    return  BCM_E_NONE;
                } else {
                    type_mask |= DRV_SWITCH_TRAP_ICMPV6;
                    type_mask &= ~DRV_SWITCH_TRAP_ICMPV6_DISABLE;
                    trap_action = TRUE;
                }
            } else {
                if (type_mask & DRV_SWITCH_TRAP_ICMPV6){
                    
                    type_mask &= ~DRV_SWITCH_TRAP_ICMPV6;
                    type_mask |= DRV_SWITCH_TRAP_ICMPV6_DISABLE;
                    trap_action = TRUE;
                } else {
                    /* disabled already !!
                     *  two cases might happened in this section.
                     *  1. icmp v6 snoop enabled and trap disabled.
                     *  2. icmp v6 snoop and trap are both disabled
                     */
                    return  BCM_E_NONE;
                }
            }
            break;

        default :
            break;
        }   /* switch case */
        
        if (snoop_action || trap_action){
            if (snoop_action){    /* doing snoop configuration */
            
                soc_cm_debug(DK_VERBOSE, "%s,%d,snoop set with mask=0x%X\n", 
                        __func__, __LINE__,snoop_type_mask);
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->snoop_set)
                                (unit, snoop_type_mask));
            }
            
            if (trap_action){    /* doing trap configuration */
            
                soc_cm_debug(DK_VERBOSE, "%s,%d,trap set with mask=0x%X\n", 
                        __func__, __LINE__, type_mask);
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->trap_set)
                                (unit, bmp, type_mask));
            }
            /* reutn for type parameter is handled */
            return BCM_E_NONE;
        }
        
    }   /* SOC_IS_ROBO53115(unit), SOC_IS_ROBO53118(unit) */

    switch(type) {
    case bcmSwitchIgmpToCPU:
        if (type_mask & DRV_SWITCH_TRAP_IGMP){
            if (arg){
                return  BCM_E_NONE;
            } else {
                type_mask &= ~DRV_SWITCH_TRAP_IGMP;
                /* IGMP and MLD using the same field for BCM53242 */
                if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                    type_mask &= ~DRV_SWITCH_TRAP_MLD;
                }
            }
        } else {
            if (arg){
                type_mask |= DRV_SWITCH_TRAP_IGMP;
                /* IGMP and MLD using the same field for BCM53242 */
                if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                    type_mask |= DRV_SWITCH_TRAP_MLD;
                }
            } else {
                return  BCM_E_NONE;
            }
        }
        
        break;
    case bcmSwitchBpduToCPU:        
       if (type_mask & DRV_SWITCH_TRAP_BPDU1){
            if (arg){
                return  BCM_E_NONE;
            } else {
                type_mask &= ~DRV_SWITCH_TRAP_BPDU1;
            }
        } else {
            if (arg){
                type_mask |= DRV_SWITCH_TRAP_BPDU1;
            } else {
                return  BCM_E_NONE;
            }
        }
        break;
    case bcmSwitch8021xToCPU:
       if (type_mask & DRV_SWITCH_TRAP_8021X){
            if (arg){
                return  BCM_E_NONE;
            } else {
                type_mask &= ~DRV_SWITCH_TRAP_8021X;
            }
        } else {
            if (arg){
                type_mask |= DRV_SWITCH_TRAP_8021X;
            } else {
                return  BCM_E_NONE;
            }
        }
        break;
    case bcmSwitchBcstToCPU:
        if (type_mask & DRV_SWITCH_TRAP_BCST) {
            if (arg) {
                return BCM_E_NONE;
            } else {
                type_mask &= ~DRV_SWITCH_TRAP_BCST;
            }
        } else {
            if (arg) {
                type_mask |= DRV_SWITCH_TRAP_BCST;
            } else {
                return BCM_E_NONE;
            }
        }
        break;
    case bcmSwitchMldPktToCpu:
        if (soc_feature(unit, soc_feature_igmp_mld_support)) {
            if (type_mask & DRV_SWITCH_TRAP_MLD) {
                if (arg) {
                    return BCM_E_NONE;
                } else {
                    type_mask &= ~DRV_SWITCH_TRAP_MLD;
                    /* IGMP and MLD using the same field for BCM53242 */
                    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                        type_mask &= ~DRV_SWITCH_TRAP_IGMP;
                    }
                    /* ICMPv6 and MLD using the same field for BCM5395 */
                    if (SOC_IS_ROBO5395(unit)) {
                        type_mask &= ~DRV_SWITCH_TRAP_ICMPV6;
                    }
                }
            } else {
                if (arg) {
                    type_mask |= DRV_SWITCH_TRAP_MLD;
                    /* IGMP and MLD using the same field for BCM53242 */
                    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                        type_mask |= DRV_SWITCH_TRAP_IGMP;
                    }
                    /* ICMPv6 and MLD using the same field for BCM5395 */
                    if (SOC_IS_ROBO5395(unit)) {
                        type_mask |= DRV_SWITCH_TRAP_ICMPV6;
                    }
                } else {
                    return BCM_E_NONE;
                }
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_UNAVAIL;
        break;
    }
    
    soc_cm_debug(DK_VERBOSE, 
                "bcm_robo_switch_control_set():\n"
                "\t bcm_type=%d, drv_type=0x%x\n",
                type, type_mask);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->trap_set)
                    (unit, bmp, type_mask));
    
    return BCM_E_NONE;

}               

/*
 * Function:
 *  bcm_robo_switch_control_get
 * Description:
 *  Get the current traffic-to-CPU configuration parameters for the switch.
 *      All switch chip ports are configured with the same settings.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx 
 */
int 
bcm_robo_switch_control_get(int unit,
                  bcm_switch_control_t type,
                  int *arg)
{
    uint32  type_mask = 0;
    uint32  snoop_type_mask = 0;
    int rv = BCM_E_UNAVAIL;
    soc_port_t port = 0;

    soc_cm_debug(DK_VERBOSE, 
                "BCM API : %s(),type=%d\n", __func__, type);

    switch(type) {
        case bcmSwitchDosAttackMinTcpHdrSize:
        case bcmSwitchDosAttackV4FirstFrag:
            if (soc_feature(unit, soc_feature_hw_dos_prev) ||
                soc_feature(unit, soc_feature_field)) {
                return(_bcm_robo_dos_attack_get(unit, DOS_ATTACK_ALL_PORT, type, arg));
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchDosAttackToCpu:
        case bcmSwitchDosAttackSipEqualDip:
        case bcmSwitchDosAttackTcpFlags:
        case bcmSwitchDosAttackTcpFlagsSF:
        case bcmSwitchDosAttackTcpFlagsFUP:
        case bcmSwitchDosAttackSynFrag:
        case bcmSwitchDosAttackFlagZeroSeqZero:
        case bcmSwitchDosAttackL4Port:
        case bcmSwitchDosAttackTcpPortsEqual:
        case bcmSwitchDosAttackUdpPortsEqual:
        case bcmSwitchDosAttackTcpFrag:
        case bcmSwitchDosAttackTcpOffset:
        case bcmSwitchDosAttackIcmpFragments:
        case bcmSwitchDosAttackIcmpPktOversize:
        case bcmSwitchDosAttackIcmpV6PingSize:
        case bcmSwitchDosAttackIcmp:
        case bcmSwitchDosAttackPingFlood:
        case bcmSwitchDosAttackSynFlood:
        case bcmSwitchDosAttackTcpSmurf:
            if (soc_feature(unit, soc_feature_hw_dos_prev)) {
                rv = _bcm_robo_hw_dos_attack_get(unit, type, arg);
            }
            if ((rv == BCM_E_UNAVAIL) && 
                (soc_feature(unit, soc_feature_field))) {
                rv = _bcm_robo_dos_attack_get(unit, 
                            DOS_ATTACK_ALL_PORT, type, arg);
            }
            return rv;
            break;
        default:
            break;
    }

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->trap_get)
                    (unit, port, &type_mask));
                    
    /* bcm53115, bcm53118 new feature on supporting different CPU snooping/traping 
     * behavior per separated the IGMP/MLD packet types.
     *
     * The designed process for this new feature are :
     *  1. bcmSwitchxxxxToCPU :
     *      - Return TRUE either snoop or trap is set will reutrn true.
     *      - else return FALSE.
     *  2. bcmSwitchxxxxDrop :
     *      - Return TRUE only when the trap is set.
     *      - else return FALSE.
     */ 
    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)){
        
        /* get snoop configuring status */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->snoop_get)
                        (unit, &snoop_type_mask));
        
        *arg = FALSE;   /* assigned the default return value */
        switch(type) {
        /* IGMP sections */
        case bcmSwitchIgmpToCPU:
            /* bcmSwitchIgmpToCPU got true only when all types of the IGMP 
             * pkt snooping is enabled already.
             */ 
                if ((snoop_type_mask & DRV_SNOOP_IGMP) || 
                        (type_mask & DRV_SWITCH_TRAP_IGMP) ||
                        (((snoop_type_mask & DRV_SNOOP_IGMP_QUERY) ||
                                (type_mask & DRV_SWITCH_TRAP_IGMP_QUERY)) &&
                            ((snoop_type_mask & DRV_SNOOP_IGMP_REPLEV) ||
                                (type_mask & DRV_SWITCH_TRAP_IGMP_REPLEV)) &&
                            ((snoop_type_mask & DRV_SNOOP_IGMP_UNKNOW) ||
                                (type_mask & DRV_SWITCH_TRAP_IGMP_UNKNOW)))){

                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchIgmpPktDrop:
            if ((type_mask & DRV_SWITCH_TRAP_IGMP) ||
                    ((type_mask & DRV_SWITCH_TRAP_IGMP_QUERY) && 
                    (type_mask & DRV_SWITCH_TRAP_IGMP_REPLEV) &&
                    (type_mask & DRV_SWITCH_TRAP_IGMP_UNKNOW))){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchIgmpQueryToCpu:
            if ((snoop_type_mask & DRV_SNOOP_IGMP_QUERY) ||
                        (type_mask & DRV_SWITCH_TRAP_IGMP_QUERY)){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchIgmpQueryDrop:
            if (type_mask & DRV_SWITCH_TRAP_IGMP_QUERY){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchIgmpReportLeaveToCpu:
            if ((snoop_type_mask & DRV_SNOOP_IGMP_REPLEV) ||
                        (type_mask & DRV_SWITCH_TRAP_IGMP_REPLEV)){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchIgmpReportLeaveDrop:
            if (type_mask & DRV_SWITCH_TRAP_IGMP_REPLEV){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchIgmpUnknownToCpu:
            if ((snoop_type_mask & DRV_SNOOP_IGMP_UNKNOW) ||
                        (type_mask & DRV_SWITCH_TRAP_IGMP_UNKNOW)){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchIgmpUnknownDrop:
            if (type_mask & DRV_SWITCH_TRAP_IGMP_UNKNOW){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        
        /* MLD sections */
        case bcmSwitchMldPktToCpu:
            if ((snoop_type_mask & DRV_SNOOP_MLD) ||
                    (type_mask & DRV_SWITCH_TRAP_MLD) || 
                    (((snoop_type_mask & DRV_SNOOP_MLD_QUERY) ||
                            (type_mask & DRV_SWITCH_TRAP_MLD_QUERY)) &&
                        ((snoop_type_mask & DRV_SNOOP_MLD_REPDONE) ||
                            (type_mask & DRV_SWITCH_TRAP_MLD_REPDONE)))){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchMldPktDrop:
            if ((type_mask & DRV_SWITCH_TRAP_MLD) ||
                    ((type_mask & DRV_SWITCH_TRAP_IGMP_QUERY) && 
                    (type_mask & DRV_SWITCH_TRAP_MLD_REPDONE))){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchMldQueryToCpu:
            if ((snoop_type_mask & DRV_SNOOP_MLD_QUERY) ||
                        (type_mask & DRV_SWITCH_TRAP_MLD_QUERY)){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchMldQueryDrop:
            if (type_mask & DRV_SWITCH_TRAP_MLD_QUERY){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchMldReportDoneToCpu:
            if ((snoop_type_mask & DRV_SNOOP_MLD_REPDONE) ||
                        (type_mask & DRV_SWITCH_TRAP_MLD_REPDONE)){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchMldReportDoneDrop:
            if (type_mask & DRV_SWITCH_TRAP_MLD_REPDONE){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        
        /* others request */
        case bcmSwitchIcmpRedirectToCpu:
            if (type_mask & DRV_SWITCH_TRAP_ICMPV6){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchArpReplyToCpu:    /* ARP */
            if (snoop_type_mask & DRV_SNOOP_ARP){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        case bcmSwitchDhcpPktToCpu:     /* DHCP */
            if (snoop_type_mask & DRV_SNOOP_DHCP){
                *arg = TRUE;
            }
            return  BCM_E_NONE;
            break;
        default :
            break;
        }   /* switch case */
                
    }   /* SOC_IS_ROBO53115(unit), SOC_IS_ROBO53118(unit) */
                    
    switch(type) {
    case bcmSwitchIgmpToCPU:
        *arg = (type_mask & DRV_SWITCH_TRAP_IGMP) ? TRUE : FALSE;
        break;
    case bcmSwitchBpduToCPU:        
        *arg = (type_mask & DRV_SWITCH_TRAP_BPDU1) ? TRUE : FALSE;
        break;
    case bcmSwitch8021xToCPU:
        *arg = (type_mask & DRV_SWITCH_TRAP_8021X) ? TRUE : FALSE;
        break;
    case bcmSwitchBcstToCPU:
        *arg = (type_mask & DRV_SWITCH_TRAP_BCST) ? TRUE : FALSE;
        break;
    case bcmSwitchMldPktToCpu:
        *arg = (type_mask & DRV_SWITCH_TRAP_MLD) ? TRUE : FALSE;
        break;

    default:
        return BCM_E_UNAVAIL;
        break;
    }
    
    soc_cm_debug(DK_VERBOSE, 
                "bcm_robo_switch_control_get():\n"
                "\t bcm_type=%d, drv_type=0x%x, arg=%d\n",
                type, type_mask, *arg);
    
    
    return BCM_E_NONE;
}               

/*
 * Function:
 *  bcm_robo_switch_control_port_set
 * Description:
 *  Specify general switch behaviors on a per-port basis.
 * Parameters:
 *  unit - Device unit number
 *  port - Port to affect
 *  type - The desired configuration parameter to modify
 *  arg - The value with which to set the parameter
 * Returns:
 *  BCM_E_xxx
 */

int
bcm_robo_switch_control_port_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg)
{
    int rv = BCM_E_NONE;
    uint32  type_mask = 0;
    soc_pbmp_t bmp;
        
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_switch_control_gport_resolve(unit, port, &port));
    }
    BCM_PBMP_CLEAR(bmp);

    switch(type) {
        case bcmSwitchDosAttackToCpu:
        case bcmSwitchDosAttackSipEqualDip:
        case bcmSwitchDosAttackMinTcpHdrSize:
        case bcmSwitchDosAttackV4FirstFrag:
        case bcmSwitchDosAttackTcpFlags:
        case bcmSwitchDosAttackTcpFlagsSF:
        case bcmSwitchDosAttackTcpFlagsFUP:
        case bcmSwitchDosAttackSynFrag:
        case bcmSwitchDosAttackFlagZeroSeqZero:
        case bcmSwitchDosAttackL4Port:
        case bcmSwitchDosAttackTcpFrag:
        case bcmSwitchDosAttackPingFlood:
        case bcmSwitchDosAttackSynFlood:
        case bcmSwitchDosAttackTcpSmurf:
            /* 
             * A primary check point for DoS Attack?
             */
            if (soc_feature(unit, soc_feature_field)) {
                rv = _bcm_robo_dos_attack_set
                    (unit, port, type, arg);
            } else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitch8021xToCPU:
            if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
                /* get per-port trap configuring status */
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->trap_get)
                            (unit, port, &type_mask));
               if (type_mask & DRV_SWITCH_TRAP_8021X){
                    if (arg){
                        return  BCM_E_NONE;
                    } else {
                        type_mask &= ~DRV_SWITCH_TRAP_8021X;
                    }
                } else {
                    if (arg){
                        type_mask |= DRV_SWITCH_TRAP_8021X;
                    } else {
                        return  BCM_E_NONE;
                    }
                }
                BCM_PBMP_PORT_ADD(bmp, port);
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->trap_set)
                            (unit, bmp, type_mask));
            } else {
                return BCM_E_UNAVAIL;
            } 
            break;
        default:
            return BCM_E_UNAVAIL;
    }


    return rv;
}

/*
 * Function:
 *  bcm_robo_switch_control_port_get
 * Description:
 *  Retrieve general switch behaviors on a per-port basis
 * Parameters:
 *  unit - Device unit number
 *  port - Port to check
 *  type - The desired configuration parameter to retrieve
 *  arg - Pointer to where the retrieved value will be written
 * Returns:
 *  BCM_E_xxx
 */

int
bcm_robo_switch_control_port_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg)
{
    int rv = BCM_E_NONE;
    uint32  type_mask = 0;
        
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_switch_control_gport_resolve(unit, port, &port));
    }

    switch(type) {
        case bcmSwitchDosAttackToCpu:
        case bcmSwitchDosAttackSipEqualDip:
        case bcmSwitchDosAttackMinTcpHdrSize:
        case bcmSwitchDosAttackV4FirstFrag:
        case bcmSwitchDosAttackTcpFlags:
        case bcmSwitchDosAttackTcpFlagsSF:
        case bcmSwitchDosAttackTcpFlagsFUP:
        case bcmSwitchDosAttackSynFrag:
        case bcmSwitchDosAttackFlagZeroSeqZero:
        case bcmSwitchDosAttackL4Port:
        case bcmSwitchDosAttackTcpFrag:
        case bcmSwitchDosAttackPingFlood:
        case bcmSwitchDosAttackSynFlood:
        case bcmSwitchDosAttackTcpSmurf:
            if (soc_feature(unit, soc_feature_field)) {
                rv = _bcm_robo_dos_attack_get
                    (unit, port, type, arg);
            } else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitch8021xToCPU:
            if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) { 
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->trap_get)
                            (unit, port, &type_mask));
                *arg = (type_mask & DRV_SWITCH_TRAP_8021X) ? TRUE : FALSE;
            } else {
                return BCM_E_UNAVAIL;
            }  
            break;
        default:
            return BCM_E_UNAVAIL;
    }


    return rv;
}

