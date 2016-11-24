/*
 * $Id: vlan.c,v 1.1 2011/04/18 17:11:00 mruas Exp $
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
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/l3.h>
#include <bcm/debug.h>

#include <soc/mem.h>


cmd_result_t
if_robo_pvlan(int u, args_t *a)
{
    char *subcmd, *argpbm, *argvid;
    vlan_id_t vid = BCM_VLAN_INVALID;
    soc_port_t port;
    int rv;
    pbmp_t pbm;
    pbmp_t vid_pbmp, vid_ubmp;

    if (! sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        subcmd = "show";
    }

    if ((argpbm = ARG_GET(a)) == NULL) {
        pbm = PBMP_ALL(u);
    } else {
        if (parse_pbmp(u, argpbm, &pbm) < 0) {
            printk("%s: ERROR: unrecognized port bitmap: %s\n",
                    ARG_CMD(a), argpbm);
            return CMD_FAIL;
        }
        SOC_PBMP_AND(pbm, PBMP_ALL(u));
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        rv = BCM_E_NONE;

        PBMP_ITER(pbm, port) {
            if ((rv = bcm_port_untagged_vlan_get(u, port, &vid)) < 0) {
                    printk("Error retrieving info for port %s: %s\n",
                   SOC_PORT_NAME(u, port), bcm_errmsg(rv));
                break;
            }
    
	    printk("Port %s default VLAN is %d\n",
		   BCM_PORT_NAME(u, port), vid);
        }

        return (rv < 0) ? CMD_FAIL : CMD_OK;
    } else if (sal_strcasecmp(subcmd, "set") == 0) {
        if ((argvid = ARG_GET(a)) == NULL) {
            printk("Missing VID for set.\n");
            return CMD_USAGE;
        }
        vid = sal_ctoi(argvid, 0);
    } else {
        return CMD_USAGE;
    }

    /* Set default VLAN as indicated */

    rv = BCM_E_NONE;

    /* Check with the forward pbmp of this vlanid */
    if ((rv = bcm_vlan_port_get(u, vid, &vid_pbmp, &vid_ubmp)) < 0) {
        SOC_PBMP_ASSIGN(vid_pbmp, PBMP_ALL(u));
    }
    SOC_PBMP_AND(pbm, vid_pbmp);

    PBMP_ITER(pbm, port) {
        if ((rv = bcm_port_untagged_vlan_set(u, port, vid)) < 0) {
            printk("Error setting port %s default VLAN to %d: %s\n",
               SOC_PORT_NAME(u, port), vid, bcm_errmsg(rv));
            if ((rv == BCM_E_NOT_FOUND) ||
                (rv == BCM_E_CONFIG)) {
                printk("VLAN %d must be created and contain the ports "
                       "before being used for port default VLAN.\n", vid);
            }
            break;
        }
    }
    return CMD_OK;
}

#ifdef BCM_MAC2V_SUPPORT
int 
_mem_macvlan_get(int unit, bcm_mac_t mac_addr, 
                uint64 *data0, uint64 *data1)
{
    uint32  reg_addr, reg_len, reg_val32, count, temp;
    uint64  reg_val64 = 0, mac_field = 0;
    soc_control_t   *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        MEM_LOCK(unit, MAC2VLANm);
        /* 1. set arla_mac */
        if (mac_addr == NULL){
            return SOC_E_PARAM;
        }
        
        SAL_MAC_ADDR_TO_UINT64(mac_addr, mac_field);
        
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, ARLA_MACr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, ARLA_MACr, 0, 0);
        
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, ARLA_MACr, (uint32 *)&reg_val64, 
                MAC_ADDR_INDXf, (uint32 *)&mac_field));
    
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, (uint32 *)&reg_val64, reg_len));
        
        /* 2. set arla_rwctl(read), check for command DONE. */
        MEM_RWCTRL_REG_LOCK(soc);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, ARLA_RWCTLr, 0, 0);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, (uint32 *)&reg_val32, 1)) < 0) {
            MEM_RWCTRL_REG_UNLOCK(soc);
            goto mem_locate_exit;
        }
        temp = 1; /* Read operation */
        (DRV_SERVICES(unit)->reg_field_set)
            (unit, ARLA_RWCTLr, &reg_val32, TAB_RWf, &temp);
        temp = 0x5; /* Access MAC2VLAN table */
        (DRV_SERVICES(unit)->reg_field_set)
            (unit, ARLA_RWCTLr, &reg_val32, TAB_INDEXf, &temp);
        temp = 1;
        (DRV_SERVICES(unit)->reg_field_set)
            (unit, ARLA_RWCTLr, &reg_val32, ARL_STRTDNf, &temp);
        if ((rv = (DRV_SERVICES(unit)->reg_write)
                    (unit, reg_addr, (uint32 *)&reg_val32, 1)) < 0) {
            MEM_RWCTRL_REG_UNLOCK(soc);
            goto mem_locate_exit;
        }
    
        /* wait for complete */
        for (count = 0; count < SOC_TIMEOUT_VAL; count++) {
            if ((rv = (DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, (uint32 *)&reg_val32, 1)) < 0) {
                MEM_RWCTRL_REG_UNLOCK(soc);
                goto mem_locate_exit;
            }
            (DRV_SERVICES(unit)->reg_field_get)
                (unit, ARLA_RWCTLr, &reg_val32, ARL_STRTDNf, &temp);
            if (!temp)
                break;
        }
    
        if (count >= SOC_TIMEOUT_VAL) {
            rv = SOC_E_TIMEOUT;
            MEM_RWCTRL_REG_UNLOCK(soc);
            goto mem_locate_exit;
        }
        MEM_RWCTRL_REG_UNLOCK(soc);
    
        /* 3. get othere_table_data0 , othere_table_data1 */
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, OTHER_TABLE_DATA0r, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, OTHER_TABLE_DATA0r);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, (uint32 *)data0, reg_len)) < 0) {
            goto mem_locate_exit;
        }
        
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, OTHER_TABLE_DATA1r, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, OTHER_TABLE_DATA1r);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, (uint32 *)data1, reg_len)) < 0) {
            goto mem_locate_exit;
        }
    
     mem_locate_exit:
        MEM_UNLOCK(unit, MAC2VLANm);
        return SOC_E_NONE;
    } else {
        return SOC_E_UNAVAIL;
    }
}
#endif

cmd_result_t
if_robo_vlan(int unit, args_t *a)
{
    char        *subcmd, *c;
    int         r = 0;
    vlan_id_t       id = VLAN_ID_INVALID;
    pbmp_t      arg_ubmp;
    pbmp_t      arg_pbmp;
    parse_table_t   pt;
    cmd_result_t    ret_code;
    char                *bcm_vlan_mcast_flood_str[] = BCM_VLAN_MCAST_FLOOD_STR;

    BCM_PBMP_CLEAR(arg_ubmp);
    BCM_PBMP_CLEAR(arg_pbmp);

    if (! sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "create") == 0) {
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        id = parse_integer(c);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap",  PQ_DFL|PQ_PBMP,
                (void *)(0), &arg_pbmp, NULL);
        parse_table_add(&pt, "UntagBitMap",     PQ_DFL|PQ_PBMP,
                (void *)(0), &arg_ubmp, NULL);

        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            printk("%s: ERROR: Unknown option: %s\n",
               ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);

        if ((r = bcm_vlan_create(unit, id)) < 0) {
            goto bcm_err;
        }

       if ((r = bcm_vlan_port_add(unit, id, arg_pbmp, arg_ubmp)) < 0) {
           goto bcm_err;
       }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "destroy") == 0) {

        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        id = parse_integer(c);

        if ((r = bcm_vlan_destroy(unit, id)) < 0) {
            goto bcm_err;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "clear") == 0) {

        if ((r = bcm_vlan_destroy_all(unit)) < 0) {
            goto bcm_err;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "add") == 0 || 
            sal_strcasecmp(subcmd, "remove") == 0) {
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        id = parse_integer(c);

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP,
            (void *)(0), &arg_pbmp, NULL);
        if (sal_strcasecmp(subcmd, "add") == 0) {
            parse_table_add(&pt, "UntagBitMap", PQ_DFL|PQ_PBMP,
                (void *)(0), &arg_ubmp, NULL);
        }

        if (!parseEndOk( a, &pt, &ret_code)) {
            return ret_code;
        }

        if (sal_strcasecmp(subcmd, "remove") == 0) {
            if ((r = bcm_vlan_port_remove(unit, id, arg_pbmp)) < 0) {
                goto bcm_err;
            }
        } else {
            if ((r = bcm_vlan_port_add(unit, id, arg_pbmp, arg_ubmp)) < 0) {
               goto bcm_err;
           }
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {

        bcm_vlan_data_t *list;
        int count, i;
        char pfmtp[SOC_PBMP_FMT_LEN],
             pfmtu[SOC_PBMP_FMT_LEN];

        if ((c = ARG_GET(a)) != NULL) {
            id = parse_integer(c);
        }

        printk("%-8s%-10s  %-10s\n",
               "ID", "Ports", "Untagged");

        if ((r = bcm_vlan_list(unit, &list, &count)) < 0) {
            goto bcm_err;
        }

        for (i = 0; i < count; i++) {
            if (id == VLAN_ID_INVALID || list[i].vlan_tag == id) {
                printk("%-8d%s  %s\n",
                   list[i].vlan_tag,
                   SOC_PBMP_FMT(list[i].port_bitmap, pfmtp),
                   SOC_PBMP_FMT(list[i].ut_port_bitmap, pfmtu));
            }
        }

        bcm_vlan_list_destroy(unit, list, count);

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "default") == 0) {

        if ((c = ARG_GET(a)) != NULL) {
            id = parse_integer(c);
        }

        if (id == VLAN_ID_INVALID) {
            if ((r = bcm_vlan_default_get(unit, &id)) < 0) {
                goto bcm_err;
            }

            printk("Default VLAN ID is %d\n", id);
        } else {
            if ((r = bcm_vlan_default_set(unit, id)) < 0) {
                goto bcm_err;
            }

            printk("Default VLAN ID set to %d\n", id);
        }

        return CMD_OK;
    }
/* Vlan control */
    if (sal_strcasecmp(subcmd, "control") == 0 ||
    sal_strcasecmp(subcmd, "ctrl") == 0) {
    char    *value, *tname;
    int ttype, i, varg, matched;

    static struct {         /* match enum from bcm/vlan.h */
        int     type;
        char    *name;
    } typenames[] = {
        { bcmVlanDropUnknown,   "dropunknown" },
        { bcmVlanPreferIP4,     "preferip4" },
        { bcmVlanPreferMAC,     "prefermac" },
        { bcmVlanShared,        "shared" },
        { bcmVlanSharedID,      "sharedid" },
        { bcmVlanTranslate,      "translate" },
            { bcmVlanIgnorePktTag,      "ignorepkttag" },
            { bcmVlanPreferEgressTranslate,      "preferegresstranslate" },
            { bcmVlanPerPortTranslate,      "perporttranslate" },
        { 0,            NULL }      /* LAST ENTRY */
    };

    subcmd = ARG_GET(a);
    value = ARG_GET(a);

    matched = 0;

    for (i = 0; typenames[i].name != NULL; i++) {
        tname = typenames[i].name;
        if (subcmd == NULL || sal_strcasecmp(subcmd, tname) == 0) {
        matched += 1;
        ttype = typenames[i].type;
        if (value == NULL) {
            r = bcm_vlan_control_get(unit, ttype, &varg);
            if (r < 0) {
            printk("%-20s-\t%s\n", tname, bcm_errmsg(r));
            } else {
            printk("%-20s%d\n", tname, varg);
            }
        } else {
            varg = parse_integer(value);
            r = bcm_vlan_control_set(unit, ttype, varg);
            if (r < 0) {
            printk("%s\tERROR: %s\n", tname, bcm_errmsg(r));
            }
        }
        }
    }

    if (matched == 0) {
        printk("%s: ERROR: Unknown control name\n", subcmd);
        return CMD_FAIL;
    }

    return CMD_OK;
    }
    
#ifdef BCM_PROTOCOL2V_SUPPORT
    /* Protocol vlan selection */
    if (sal_strcasecmp(subcmd, "protocol") == 0 ||
                sal_strcasecmp(subcmd, "proto") == 0) {

        bcm_port_t  port;
        int     en_act= 0;
        
        subcmd = ARG_GET(a);
        if (subcmd == NULL) {
            printk("%s: ERROR: missing protocol subcommand\n", ARG_CMD(a));
            return CMD_FAIL;
        }

        if (sal_strcasecmp(subcmd, "enable") == 0 ) {
            en_act = 1;
        } else if (sal_strcasecmp(subcmd, "disable") == 0 ) {
            en_act = 0;
        } else {
            en_act = -1;
        }
        if (en_act != -1){
            bcm_pbmp_t  pbmp;
            
            port = 0 ;            
            port = ~port;
            BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->vlan_prop_port_enable_get)
                            (unit, DRV_VLAN_PROP_PROTOCOL2V_PORT, port,  
                            (uint32 *)&pbmp));
            BCM_PBMP_CLEAR(arg_pbmp);
            BCM_PBMP_OR(arg_pbmp, pbmp);
            
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP,
                        (void *)(0), &arg_pbmp, NULL);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->vlan_prop_port_enable_set)
                            (unit, DRV_VLAN_PROP_PROTOCOL2V_PORT, arg_pbmp,
                             (en_act ? TRUE :FALSE)));   
            
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "add") == 0) {
            bcm_port_t  port;
            int     frame, ether, vlan, prio;

            /* port and frame are not used in ROBO Protocol VLAN */
            port = frame = 0;
            
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Ether", PQ_HEX, 0, &ether, NULL);
            parse_table_add(&pt, "VLan", PQ_INT, 0, &vlan, NULL);
            parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            if ((r = bcm_port_protocol_vlan_add(unit, port, frame,
                                ether, vlan)) < 0) {
                goto bcm_err;
            }
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "delete") == 0) {
            bcm_port_t  port;
            int     frame, ether;

            /* port and frame are not used in ROBO Protocol VLAN */
            port = frame = 0;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Ether", PQ_HEX, 0, &ether, NULL);
            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            if ((r = bcm_port_protocol_vlan_delete(unit, port, frame,
                            ether)) < 0) {
                goto bcm_err;
            }
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "clear") == 0) {
            bcm_port_t  port;

            /* port and frame are not used in ROBO Protocol VLAN */
            port = 0;
            
            if ((r = bcm_port_protocol_vlan_delete_all(unit,
                        port)) < 0) {
                goto bcm_err;
            }
            return CMD_OK;
        }

        printk("%s: ERROR: unknown protocol subcommand: %s\n",
                    ARG_CMD(a), subcmd);

        return CMD_FAIL;
    }
#endif

#ifdef BCM_MAC2V_SUPPORT
    /* MAC address vlan selection */
    if (sal_strcasecmp(subcmd, "mac") == 0) {

        bcm_port_t  port;
        int     en_act= 0;
        
        subcmd = ARG_GET(a);
        if (subcmd == NULL) {
            printk("%s: ERROR: missing mac subcommand\n", ARG_CMD(a));
            return CMD_FAIL;
        }

        if (sal_strcasecmp(subcmd, "enable") == 0 ) {
            en_act = 1;
        } else if (sal_strcasecmp(subcmd, "disable") == 0 ) {
            en_act = 0;
        } else {
            en_act = -1;
        }
        if (en_act != -1){
            
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP,
                        (void *)(0), &arg_pbmp, NULL);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            if (BCM_PBMP_EQ(arg_pbmp, PBMP_ALL(unit))){
                if ((r = bcm_vlan_control_set(unit, bcmVlanPreferMAC, 
                                    (en_act) ? TRUE : FALSE)) < 0){
                    goto bcm_err;
                }
            } else {

                PBMP_ITER(arg_pbmp, port) {
                    if ((r = bcm_vlan_control_port_set(unit, port, 
                                        bcmVlanPortPreferMAC, 
                                        (en_act) ? TRUE : FALSE))< 0) {
                        goto bcm_err;
                    }
                }
            }

            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "add") == 0) {
            bcm_mac_t   mac;
            int     vlan, prio;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "MACaddress", PQ_MAC, 0, &mac, NULL);
            parse_table_add(&pt, "VLan", PQ_INT, 0, &vlan, NULL);
            parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            if ((r = bcm_vlan_mac_add(unit, mac, vlan, prio)) < 0) {
                goto bcm_err;
            }

            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "delete") == 0) {
            bcm_mac_t   mac;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "MACaddress", PQ_MAC, 0, &mac, NULL);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            if ((r = bcm_vlan_mac_delete(unit, mac)) < 0) {
                goto bcm_err;
            }

            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "clear") == 0) {
            if ((r = bcm_vlan_mac_delete_all(unit)) < 0) {
                goto bcm_err;
            }

            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "show") == 0) {
            bcm_mac_t   mac, loc_mac_addr;
            mac2vlan_entry_t    mv_entry0, mv_entry1;
            uint32      loc_valid;
            uint64      mac_field;
            int         found, check_next, loc_prio;
            int   loc_vid = 0;
            
            

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "MACaddress", PQ_MAC, 0, &mac, NULL);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }


            sal_memset(&mv_entry0, 0, sizeof(mac2vlan_entry_t));
            sal_memset(&mv_entry1, 0, sizeof(mac2vlan_entry_t));
            if ((r = _mem_macvlan_get(unit, mac,
                    (uint64 *)&mv_entry0, (uint64 *)&mv_entry1)) < 0) {
                goto bcm_err;
            }

            check_next = 0;
            found = 0;
            /* get the data0 vlaid bit */
            if ((r =(DRV_SERVICES(unit)->mem_field_get)
                            (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_VALID,
                            (uint32 *)&mv_entry0, (uint32 *)&loc_valid)) < 0) {
                goto bcm_err;
            }
                            
            if (loc_valid){
                if ((r =(DRV_SERVICES(unit)->mem_field_get)
                                (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_MAC,
                                (uint32 *)&mv_entry0, (uint32 *)&mac_field)) < 0) {
                    goto bcm_err;
                }
                SAL_MAC_ADDR_FROM_UINT64(loc_mac_addr, mac_field);
                if (!SAL_MAC_ADDR_CMP(loc_mac_addr, mac)){
                    if ((r =(DRV_SERVICES(unit)->mem_field_get)
                                    (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_NEW_VLANID,
                                    (uint32 *)&mv_entry0, (uint32 *)&loc_vid)) < 0) {
                        goto bcm_err;
                    }
                    if ((r =(DRV_SERVICES(unit)->mem_field_get)
                                    (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_PRIORITY,
                                    (uint32 *)&mv_entry0, (uint32 *)&loc_prio)) < 0) {
                        goto bcm_err;
                    }

                    printk("mac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d priority=%d\n",
                        loc_mac_addr[0], loc_mac_addr[1], loc_mac_addr[2], 
                        loc_mac_addr[3], loc_mac_addr[4], loc_mac_addr[5], 
                        loc_vid, loc_prio);
                    found = 1;
                    
                } else {
                    /* assign to bin1 first for not found case */
                    check_next = 1;     
                }
            }

            if (check_next) {
                /* get the data1 vlaid bit */
                if ((r =(DRV_SERVICES(unit)->mem_field_get)
                                (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_VALID,
                                (uint32 *)&mv_entry1, (uint32 *)&loc_valid)) < 0) {
                    goto bcm_err;
                }
                                
                if (loc_valid){
                    if ((r =(DRV_SERVICES(unit)->mem_field_get)
                                    (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_MAC,
                                    (uint32 *)&mv_entry1, (uint32 *)&mac_field)) < 0) {
                        goto bcm_err;
                    }
                    SAL_MAC_ADDR_FROM_UINT64(loc_mac_addr, mac_field);
                    if (!SAL_MAC_ADDR_CMP(loc_mac_addr, mac)){
                        if ((r =(DRV_SERVICES(unit)->mem_field_get)
                                        (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_NEW_VLANID,
                                        (uint32 *)&mv_entry1, (uint32 *)&loc_vid)) < 0) {
                            goto bcm_err;
                        }
                        if ((r =(DRV_SERVICES(unit)->mem_field_get)
                                        (unit, DRV_MEM_MACVLAN, DRV_MEM_FIELD_PRIORITY,
                                        (uint32 *)&mv_entry1, (uint32 *)&loc_prio)) < 0) {
                            goto bcm_err;
                        }
    
                        printk("mac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d priority=%d\n",
                            loc_mac_addr[0], loc_mac_addr[1], loc_mac_addr[2], 
                            loc_mac_addr[3], loc_mac_addr[4], loc_mac_addr[5], 
                            loc_vid, loc_prio);
                        found = 1;
                    }
                }

            }

            if (!found) {
                printk("mac=%02x:%02x:%02x:%02x:%02x:%02x not exist in MAC2VLAN table.\n",
                            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }
            return CMD_OK;
        }

        printk("%s: ERROR: Unknown MAC subcommand: %s\n", ARG_CMD(a), subcmd);

        return CMD_FAIL;
    }
#endif

#ifdef  BCM_V2V_SUPPORT
    /* VLAN translate selection */
    if (sal_strcasecmp(subcmd, "translate") == 0) {
        int arg_vtmode;
        int     en_act= 0;
        bcm_port_t  port;

        subcmd = ARG_GET(a);
        
        if (subcmd == NULL) {
            printk("%s: ERROR: Missing translate subcommand\n", ARG_CMD(a));
            return CMD_USAGE;
        }

        if (sal_strcasecmp(subcmd, "enable") == 0 ) {
            en_act = 1;
        } else if (sal_strcasecmp(subcmd, "disable") == 0 ) {
            en_act = 0;
        } else {
            en_act = -1;
        }
        if (en_act != -1){
            bcm_pbmp_t  pbmp;
            
            port = 0 ;            
            port = ~port;
            BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->vlan_prop_port_enable_get)
                            (unit, DRV_VLAN_PROP_V2V_PORT, port,  
                            (uint32 *)&pbmp));
            BCM_PBMP_CLEAR(arg_pbmp);
            BCM_PBMP_OR(arg_pbmp, pbmp);
            
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP,
                        (void *)(0), &arg_pbmp, NULL);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            if (BCM_PBMP_EQ(arg_pbmp, PBMP_ALL(unit))){
                if ((r = bcm_vlan_control_set(unit, bcmVlanTranslate, 
                                    (en_act) ? TRUE : FALSE)) < 0){
                    goto bcm_err;
                }
            } else {

                PBMP_ITER(arg_pbmp, port) {
                    if ((r = bcm_vlan_control_port_set(unit, port, 
                                        bcmVlanTranslateIngressEnable, 
                                        (en_act) ? TRUE : FALSE))< 0) {
                        goto bcm_err;
                    }
                }
            }

            return CMD_OK;
        }
        
        if (sal_strcasecmp(subcmd, "On") == 0) {
            arg_vtmode = 1;
        } else if (sal_strcasecmp(subcmd, "Off") == 0){
            arg_vtmode = 0;
        } else {
            arg_vtmode = -1;
        }

        if (arg_vtmode != -1) {
            if ((r = bcm_vlan_control_set(unit, bcmVlanTranslate,
                        (arg_vtmode) ? TRUE : FALSE)) < 0) {
                goto bcm_err;
            }
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "add") == 0) {
            int     port, old_vlan, new_vlan, prio;
            int     dtag = 0;
            static  char *dt_mode[] = {
                            "Flase","True",NULL}; /* double tagged mode */

            /* port and priority is not used in ROBO VLAN translation */
            port = prio = old_vlan = new_vlan = 0;
          
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_INT, 0, &port,   NULL);
            parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);
            parse_table_add(&pt, "NewVLan", PQ_INT, 0, &new_vlan, NULL);
            parse_table_add(&pt, "DTag", 
                                PQ_DFL|PQ_MULTI, 0, &dtag, dt_mode);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            if (dtag == 1){ 
                if ((r = bcm_vlan_dtag_add(unit, port,
                                old_vlan, new_vlan, prio)) < 0) {
                    goto bcm_err;
                }
            } else if (dtag == 0){
                if ((r = bcm_vlan_translate_add(unit, port,
                                old_vlan, new_vlan, prio)) < 0) {
                    goto bcm_err;
                }
            }

            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "get") == 0) {
            int	port, prio = -1;
            int old_vlan;
            bcm_vlan_t new_vlan;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_INT, 0, &port, NULL);
            parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            if (!BCM_VLAN_VALID(old_vlan)) {
                goto bcm_err;
            }
            if ((r = bcm_vlan_translate_get(unit, port,
                                            (bcm_vlan_t)old_vlan, 
                                            &new_vlan, 
                                            &prio)) < 0) {
                goto bcm_err;
            }

            /* The priority key is ignored in ROBO vlan translation */
            printk("New Vlan ID = %d, Priority = %d\n", new_vlan, prio);
            return CMD_OK;
        }
    
        if (sal_strcasecmp(subcmd, "delete") == 0) {
            int     port, old_vlan;

            /* port and priority is not used in ROBO VLAN translation */
            port = 0;
          
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_INT, 0, &port,   NULL);
            parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            if ((r = bcm_vlan_translate_delete(unit, port, old_vlan)) < 0) {
                goto bcm_err;
            }

            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "clear") == 0) {
            if ((r = bcm_vlan_translate_delete_all(unit)) < 0) {
                goto bcm_err;
            }

            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "egress") == 0) {
            char * subsubcmd = ARG_GET(a);
            if (subsubcmd == NULL) {
                printk("%s: ERROR: Missing translate egress subcommand\n",
		       ARG_CMD(a));
                return CMD_FAIL;
            }

            if (sal_strcasecmp(subsubcmd, "add") == 0) {
                int port, old_vlan, new_vlan, prio, cng;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_INT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);
                parse_table_add(&pt, "NewVLan", PQ_INT, 0, &new_vlan, NULL);
                parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
                parse_table_add(&pt, "Cng", PQ_INT, 0, &cng, NULL);

                if (!parseEndOk( a, &pt, &ret_code)) {
                    return ret_code;
                }

                if (cng) {
                    prio |= BCM_PRIO_DROP_FIRST;
                }

                if ((r = bcm_vlan_translate_egress_add(unit, port,
                                            old_vlan, new_vlan, prio)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }
            if (sal_strcasecmp(subsubcmd, "get") == 0) {
                int port,  prio = -1;
                int old_vlan;
                bcm_vlan_t new_vlan;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_INT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);

                if (!parseEndOk( a, &pt, &ret_code)) {
                    return ret_code;
                }

                if (!BCM_VLAN_VALID(old_vlan)) {
                    goto bcm_err;
                }
                if ((r = bcm_vlan_translate_egress_get(unit, port,
                                                       (bcm_vlan_t)old_vlan, 
                                                       &new_vlan, 
                                                       &prio)) < 0) {
                    goto bcm_err;
                }

                printk("New Vlan ID = %d, Priority = %d\n", new_vlan, prio);
                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "delete") == 0) {
                int port, old_vlan;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_INT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);

                if (!parseEndOk( a, &pt, &ret_code)) {
                    return ret_code;
                }

                if ((r = bcm_vlan_translate_egress_delete(unit, port,
							  old_vlan)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "clear") == 0) {
                if ((r = bcm_vlan_translate_egress_delete_all(unit)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }
        }

        if (sal_strcasecmp(subcmd, "dtag") == 0) {
            char * subsubcmd = ARG_GET(a);
            if (subsubcmd == NULL) {
                printk("%s: ERROR: Missing translate dtag subcommand\n",
		       ARG_CMD(a));
                return CMD_FAIL;
            }

            if (sal_strcasecmp(subsubcmd, "add") == 0) {
                int port, old_vlan, new_vlan, prio, cng;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_INT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);
                parse_table_add(&pt, "NewVLan", PQ_INT, 0, &new_vlan, NULL);
                parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
                parse_table_add(&pt, "Cng", PQ_INT, 0, &cng, NULL);

                if (!parseEndOk( a, &pt, &ret_code)) {
                    return ret_code;
                }

                if (cng) {
                    prio |= BCM_PRIO_DROP_FIRST;
                }

                if ((r = bcm_vlan_dtag_add(unit, port,
                                           old_vlan, new_vlan, prio)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "get") == 0) {
                int port, prio = -1;
                int old_vlan;
                bcm_vlan_t new_vlan;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_INT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);

                if (!parseEndOk( a, &pt, &ret_code)) {
                    return ret_code;
                }

                if (!BCM_VLAN_VALID(old_vlan)) {
                    goto bcm_err;
                }
                if ((r = bcm_vlan_dtag_get(unit, port, (bcm_vlan_t)old_vlan, 
                                           &new_vlan, &prio)) < 0) {
                    goto bcm_err;
                }

                printk("New Vlan ID = %d, Priority = %d\n", new_vlan, prio);
                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "delete") == 0) {
                int port, old_vlan;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_INT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);

                if (!parseEndOk( a, &pt, &ret_code)) {
                    return ret_code;
                }

                if ((r = bcm_vlan_dtag_delete(unit, port, old_vlan)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "clear") == 0) {
                if ((r = bcm_vlan_dtag_delete_all(unit)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }
        }

        printk("%s: ERROR: Unknown translate subcommand: %s\n",
                ARG_CMD(a), subcmd);

        return CMD_FAIL;
    }
#endif      /* BCM_V2V_SUPPORT */

    /* Set per Vlan property (Must be the last)*/
    {
        bcm_vlan_control_vlan_t	vlan_control, default_control;
        int outer_tpid, learn_disable, unknown_ip6_mcast_to_cpu;
        int def_outer_tpid, def_learn_disable, def_unknown_ip6_mcast_to_cpu;
        int unknown_ip4_mcast_to_cpu, def_unknown_ip4_mcast_to_cpu;
        int def_ip4_disable, ip4_disable;
        int def_ip6_disable, ip6_disable;
        int def_ip4_mcast_disable, ip4_mcast_disable;
        int def_ip6_mcast_disable, ip6_mcast_disable;
        int def_ip4_mcast_l2_disable, ip4_mcast_l2_disable;
        int def_ip6_mcast_l2_disable, ip6_mcast_l2_disable;
        int def_mpls_disable, mpls_disable;
        int def_cosq_enable, cosq_enable;
        bcm_cos_queue_t cosq;

        id = parse_integer(subcmd);


       sal_memset(&default_control, 0, sizeof(vlan_control));
	if (VLAN_ID_VALID(id)) {
            r = bcm_vlan_control_vlan_get(unit, id, &default_control);
            if (r < 0) {
                goto bcm_err;
            }

            sal_memcpy(&vlan_control, &default_control, sizeof(vlan_control));

            def_outer_tpid    = default_control.outer_tpid;
            outer_tpid        = def_outer_tpid;

            def_learn_disable = (default_control.flags & 
                                BCM_VLAN_LEARN_DISABLE) ? 1 : 0;
            learn_disable     = def_learn_disable;

            def_unknown_ip6_mcast_to_cpu = (default_control.flags &
                                      BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU) ? 1 : 0;
            unknown_ip6_mcast_to_cpu     =  def_unknown_ip6_mcast_to_cpu;

            def_unknown_ip4_mcast_to_cpu = (default_control.flags &
                                      BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU) ? 1 : 0;
            unknown_ip4_mcast_to_cpu     = def_unknown_ip4_mcast_to_cpu;

            def_ip4_disable = (default_control.flags & 
                                BCM_VLAN_IP4_DISABLE) ? 1 : 0;
            ip4_disable     = def_ip4_disable;

            def_ip6_disable = (default_control.flags & 
                                BCM_VLAN_IP6_DISABLE) ? 1 : 0;
            ip6_disable     = def_ip6_disable;

            def_ip4_mcast_disable = (default_control.flags & 
                                BCM_VLAN_IP4_MCAST_DISABLE) ? 1 : 0;
            ip4_mcast_disable     = def_ip4_mcast_disable;

            def_ip6_mcast_disable = (default_control.flags & 
                                BCM_VLAN_IP6_MCAST_DISABLE) ? 1 : 0;
            ip6_mcast_disable     = def_ip6_mcast_disable;

            def_ip4_mcast_l2_disable = (default_control.flags & 
                                BCM_VLAN_IP4_MCAST_L2_DISABLE) ? 1 : 0;
            ip4_mcast_l2_disable     = def_ip4_mcast_l2_disable;

            def_ip6_mcast_l2_disable = (default_control.flags & 
                                BCM_VLAN_IP6_MCAST_L2_DISABLE) ? 1 : 0;
            ip6_mcast_l2_disable     = def_ip6_mcast_l2_disable;

            def_mpls_disable = (default_control.flags & 
                                BCM_VLAN_MPLS_DISABLE) ? 1 : 0;
            mpls_disable     = def_mpls_disable;

            def_cosq_enable = (default_control.flags & 
                               BCM_VLAN_COSQ_ENABLE) ? 1 : 0;
            cosq_enable     = def_cosq_enable;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "VRF", PQ_INT | PQ_DFL, &default_control.vrf,
                            &vlan_control.vrf, NULL);
            parse_table_add(&pt, "OuterTPID", PQ_HEX | PQ_DFL, &def_outer_tpid, 
                            &outer_tpid, NULL);
            parse_table_add(&pt, "LearnDisable", PQ_INT | PQ_DFL, 
                            &def_learn_disable, &learn_disable, NULL);
            parse_table_add(&pt, "UnknownIp6McastToCpu", PQ_INT | PQ_DFL, 
                            &def_unknown_ip6_mcast_to_cpu, 
                            &unknown_ip6_mcast_to_cpu, NULL);
            parse_table_add(&pt, "UnknownIp4McastToCpu", PQ_INT | PQ_DFL, 
                            &def_unknown_ip4_mcast_to_cpu, 
                            &unknown_ip4_mcast_to_cpu, NULL);
            parse_table_add(&pt, "Ip4Disable", PQ_INT | PQ_DFL, 
                            &def_ip4_disable, &ip4_disable, NULL);
            parse_table_add(&pt, "Ip6Disable", PQ_INT | PQ_DFL, 
                            &def_ip6_disable, &ip6_disable, NULL);
            parse_table_add(&pt, "Ip4McastDisable", PQ_INT | PQ_DFL, 
                            &def_ip4_mcast_disable, &ip4_mcast_disable, NULL);
            parse_table_add(&pt, "Ip6McastDisable", PQ_INT | PQ_DFL, 
                            &def_ip6_mcast_disable, &ip6_mcast_disable, NULL);
            parse_table_add(&pt, "Ip4McastL2Disable", PQ_INT | PQ_DFL, 
                            &def_ip4_mcast_l2_disable, &ip4_mcast_l2_disable, NULL);
            parse_table_add(&pt, "Ip6McastL2Disable", PQ_INT | PQ_DFL, 
                            &def_ip6_mcast_l2_disable, &ip6_mcast_l2_disable, NULL);
            parse_table_add(&pt, "MplsDisable", PQ_INT | PQ_DFL, 
                            &def_mpls_disable, &mpls_disable, NULL);
            parse_table_add(&pt, "CosqEnable", PQ_INT | PQ_DFL, 
                            &def_cosq_enable, &cosq_enable, NULL);
            parse_table_add(&pt, "Cosq", PQ_INT | PQ_DFL, 
                            0, &cosq, NULL);
            parse_table_add(&pt, "Ip6McastFloodMode", PQ_MULTI | PQ_DFL, 
                            &default_control.ip6_mcast_flood_mode, 
                            &vlan_control.ip6_mcast_flood_mode,
                            bcm_vlan_mcast_flood_str);
            parse_table_add(&pt, "Ip4McastFloodMode", PQ_MULTI | PQ_DFL, 
                            &default_control.ip4_mcast_flood_mode, 
                            &vlan_control.ip4_mcast_flood_mode,
                            bcm_vlan_mcast_flood_str);
            parse_table_add(&pt, "L2McastFloodMode", PQ_MULTI | PQ_DFL, 
                            &default_control.l2_mcast_flood_mode, 
                            &vlan_control.l2_mcast_flood_mode,
                            bcm_vlan_mcast_flood_str);
            parse_table_add(&pt, "IfClass", PQ_INT | PQ_DFL, 
                            &default_control.if_class, 
                            &vlan_control.if_class, NULL);

            if (!parseEndOk( a, &pt, &ret_code)) {
                return ret_code;
            }

            vlan_control.outer_tpid     = (uint16) outer_tpid;
            vlan_control.cosq           = cosq;
            vlan_control.flags = (learn_disable ? 
                                          BCM_VLAN_LEARN_DISABLE : 0);
            vlan_control.flags |= (unknown_ip6_mcast_to_cpu ? 
                                          BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU : 0);
            vlan_control.flags |= (unknown_ip4_mcast_to_cpu? 
                                          BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU : 0);
            vlan_control.flags |= (ip4_disable ? 
                                          BCM_VLAN_IP4_DISABLE : 0);
            vlan_control.flags |= (ip6_disable ? 
                                          BCM_VLAN_IP6_DISABLE : 0);
            vlan_control.flags |= (ip4_mcast_disable ? 
                                          BCM_VLAN_IP4_MCAST_DISABLE : 0);
            vlan_control.flags |= (ip6_mcast_disable ? 
                                          BCM_VLAN_IP6_MCAST_DISABLE : 0);
            vlan_control.flags |= (ip4_mcast_l2_disable ? 
                                          BCM_VLAN_IP4_MCAST_L2_DISABLE : 0);
            vlan_control.flags |= (ip6_mcast_l2_disable ? 
                                          BCM_VLAN_IP6_MCAST_L2_DISABLE : 0);
            vlan_control.flags |= (mpls_disable ? 
                                          BCM_VLAN_MPLS_DISABLE : 0);
            vlan_control.flags |= (cosq_enable ? 
                                          BCM_VLAN_COSQ_ENABLE : 0);
 
            if ((r = bcm_vlan_control_vlan_set(unit, id, vlan_control)) < 0) {
                goto bcm_err;
            }

            return CMD_OK;
        }
    }

    return CMD_USAGE;

 bcm_err:
    printk("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
    return CMD_FAIL;
}

