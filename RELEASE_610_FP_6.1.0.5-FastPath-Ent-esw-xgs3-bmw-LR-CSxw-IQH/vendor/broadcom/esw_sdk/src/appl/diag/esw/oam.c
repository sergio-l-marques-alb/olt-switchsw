/*
 * $Id: oam.c,v 1.1 2011/04/18 17:11:00 mruas Exp $
 * 
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
#include <bcm/oam.h>
#include <bcm_int/esw/port.h>
#if defined(BCM_ENDURO_SUPPORT)
#include <bcm_int/esw/oam.h>
#include <bcm/rx.h>
#include <soc/higig.h>
#endif

#define CLEAN_UP_AND_RETURN(_result) \
    parse_arg_eq_done(&parse_table); \
    return (_result);

#define _isprintable(_c) (((_c) > 32) && ((_c) < 127))

#define GROUP_LIST_HEADER \
    "ID  Name                                             TX_RD\n" \
    "--- ------------------------------------------------ -----\n"

#define ENDPOINT_LIST_HEADER \
    "ID   Grp Name L Period CLDT VLAN Mod Port Tr MAC address       PktPri IntPri\n" \
    "---- --- ---- - ------ ---- ---- --- ---- -- ----------------- ------ ------\n"

static int events_on = 0;

static void print_group_faults(const char *header_string_p, uint32 faults)
{
    printk("%s: %s %s %s %s\n", header_string_p,
        (faults & BCM_OAM_GROUP_FAULT_REMOTE) ? "RDI" : "",
        (faults & BCM_OAM_GROUP_FAULT_CCM_TIMEOUT) ? "CCM_TIMEOUT" : "",
        (faults & BCM_OAM_GROUP_FAULT_CCM_ERROR) ? "CCM_ERROR" : "",
        (faults & BCM_OAM_GROUP_FAULT_CCM_XCON) ? "CCM_XCON" : "");
}

static int _cmd_esw_oam_group_print(int unit, bcm_oam_group_info_t *group_info_p,
    void *user_data)
{
    char name_string[BCM_OAM_GROUP_NAME_LENGTH + 1];
    int byte_index;
    char byte;

    /* Do another get to clear persistent faults */

    group_info_p->clear_persistent_faults =
        BCM_OAM_GROUP_FAULT_REMOTE |
        BCM_OAM_GROUP_FAULT_CCM_TIMEOUT |
        BCM_OAM_GROUP_FAULT_CCM_ERROR |
        BCM_OAM_GROUP_FAULT_CCM_XCON;

    bcm_oam_group_get(unit, group_info_p->id, group_info_p);

    for (byte_index = 0; byte_index < BCM_OAM_GROUP_NAME_LENGTH;
        ++byte_index)
    {
        byte = group_info_p->name[byte_index];

        name_string[byte_index] = _isprintable(byte) ? byte : '.';
    }

    name_string[byte_index] = 0;

    printk("%3d %48s %3c\n", group_info_p->id, name_string,
        (group_info_p->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) ? '*' : ' ');

    if (group_info_p->faults != 0)
    {
        print_group_faults("           Faults", group_info_p->faults);
    }

    if (group_info_p->persistent_faults != 0)
    {
        print_group_faults("Persistent faults", group_info_p->persistent_faults);
    }

    return BCM_E_NONE;
}

static void print_endpoint_faults(const char *header_string_p, uint32 faults)
{
    printk("%s: %s %s %s %s\n", header_string_p,
        (faults & BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT) ? "CCM_TIMEOUT" : "",
        (faults & BCM_OAM_ENDPOINT_FAULT_REMOTE) ? "RDI" : "",
        (faults & BCM_OAM_ENDPOINT_FAULT_PORT_DOWN) ? "PORT_DOWN" : "",
        (faults & BCM_OAM_ENDPOINT_FAULT_INTERFACE_DOWN) ? "INTERFACE_DOWN" : "");
}

static int _cmd_esw_oam_endpoint_print(int unit,
    bcm_oam_endpoint_info_t *endpoint_info_p, void *user_data)
{
    int local_tx;
    int local_rx;
    int is_remote;
    bcm_module_t module_id;
    bcm_port_t port_id;
    bcm_trunk_t trunk_id;
    bcm_port_t local_id;
    char mac_address_string[MACADDR_STR_LEN];

    /* Do another get to clear persistent faults */

    endpoint_info_p->clear_persistent_faults =
        BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT |
        BCM_OAM_ENDPOINT_FAULT_REMOTE |
        BCM_OAM_ENDPOINT_FAULT_PORT_DOWN |
        BCM_OAM_ENDPOINT_FAULT_INTERFACE_DOWN;

    bcm_oam_endpoint_get(unit, endpoint_info_p->id, endpoint_info_p);

    is_remote = endpoint_info_p->flags & BCM_OAM_ENDPOINT_REMOTE;
    local_tx = !is_remote && endpoint_info_p->ccm_period > 0;

    local_rx = !is_remote && (endpoint_info_p->flags &
        (BCM_OAM_ENDPOINT_CCM_RX | BCM_OAM_ENDPOINT_LOOPBACK |
            BCM_OAM_ENDPOINT_DELAY_MEASUREMENT | BCM_OAM_ENDPOINT_LINKTRACE));

    printk("%4d %3d ", endpoint_info_p->id, endpoint_info_p->group);
    
    if (is_remote || local_tx)
    {
        printk("%04X ", endpoint_info_p->name);
    }
    else
    {
        printk("     ");
    }
    
    printk("%1d ", endpoint_info_p->level);

    if (is_remote || local_tx)
    {
        printk("%6d ", endpoint_info_p->ccm_period);
    }
    else
    {
        printk("       ");
    }

    if (local_rx)
    {
        printk("%c%c%c%c ",
            (endpoint_info_p->flags & BCM_OAM_ENDPOINT_CCM_RX) ? '*' : ' ',
            (endpoint_info_p->flags & BCM_OAM_ENDPOINT_LOOPBACK) ? '*' : ' ',
            (endpoint_info_p->flags & BCM_OAM_ENDPOINT_DELAY_MEASUREMENT) ?
                '*' : ' ',
            (endpoint_info_p->flags & BCM_OAM_ENDPOINT_LINKTRACE) ?
                '*' : ' ');
    }
    else
    {
        printk("     ");
    }

    printk("%4d ", endpoint_info_p->vlan);

    if (BCM_FAILURE(_bcm_esw_gport_resolve(unit, endpoint_info_p->gport,
        &module_id, &port_id, &trunk_id, &local_id)))
    {
        printk("Bad GPORT   ");
    }
    else if (BCM_GPORT_IS_LOCAL(endpoint_info_p->gport))
    {
        printk("    %4d    ", port_id);
    }
    else if (BCM_GPORT_IS_MODPORT(endpoint_info_p->gport))
    {
        printk("%3d %4d    ", module_id, port_id);
    }
    else if (BCM_GPORT_IS_TRUNK(endpoint_info_p->gport))
    {
        printk("         %2d ", trunk_id);
    }
    else
    {
        printk("            ");
    }

    if (local_tx)
    {
        format_macaddr(mac_address_string, endpoint_info_p->src_mac_address);

        printk("%17s %6d %6d\n", mac_address_string, endpoint_info_p->pkt_pri,
            endpoint_info_p->int_pri);
    }
    else
    {
        printk("\n");
    }

    if (endpoint_info_p->faults != 0)
    {
        print_endpoint_faults("           Faults", endpoint_info_p->faults);
    }

    if (endpoint_info_p->persistent_faults != 0)
    {
        print_endpoint_faults("Persistent faults",
            endpoint_info_p->persistent_faults);
    }

    return BCM_E_NONE;
}

static int _cmd_esw_oam_group_endpoints_traverse(int unit,
    bcm_oam_group_info_t *group_info_p, void *user_data)
{
    return bcm_oam_endpoint_traverse(unit, group_info_p->id,
        _cmd_esw_oam_endpoint_print, NULL);
}

static int _cmd_esw_oam_event_handle(int unit, uint32 flags,
    bcm_oam_event_type_t event_type, bcm_oam_group_t group,
    bcm_oam_endpoint_t endpoint, void *user_data)
{
    int group_valid = 0;
    int endpoint_valid = 0;
    const char *oam_event_string;

    switch (event_type)
    {
        case bcmOAMEventEndpointPortDown:
            oam_event_string = "Port down";
            endpoint_valid = 1;
            break;

        case bcmOAMEventEndpointPortUp:
            oam_event_string = "Port up";
            endpoint_valid = 1;
            break;

        case bcmOAMEventEndpointInterfaceDown:
            oam_event_string = "Interface down";
            endpoint_valid = 1;
            break;

        case bcmOAMEventEndpointInterfaceUp:
            oam_event_string = "Interface up";
            endpoint_valid = 1;
            break;

        case bcmOAMEventGroupCCMxcon:
            oam_event_string = "CCM xcon";
            group_valid = 1;
            break;

        case bcmOAMEventGroupCCMError:
            oam_event_string = "CCM error";
            group_valid = 1;
            break;

        case bcmOAMEventGroupRemote:
            oam_event_string = "Some remote defect";
            group_valid = 1;
            endpoint_valid = 1;
            break;

        case bcmOAMEventGroupCCMTimeout:
            oam_event_string = "Some CCM timeout";
            group_valid = 1;
            endpoint_valid = 1;
            break;

        default:
            oam_event_string = "Unknown event";
    }

    printk("OAM event: %s%s", oam_event_string,
        (flags & BCM_OAM_EVENT_FLAGS_MULTIPLE) ? " (multiple)" : "");

    if (group_valid)
    {
        printk(" - Group %d", group);
    }

    if (endpoint_valid)
    {
        printk(" - Endpoint %d", endpoint);
    }

    printk("\n");

    return BCM_E_NONE;
}
#if defined(BCM_ENDURO_SUPPORT)
static int 
_cmd_esw_oam_tx(int unit, int port, int flags, bcm_mac_t *mac, bcm_oam_endpoint_info_t *endpoint_info)
{
    bcm_pkt_t pkt;
    enet_hdr_t *ep = NULL;
    oam_hdr_t *op = NULL;
    int vp = 0;        
    int rv = 0;
    bcm_module_t module_id;
    bcm_port_t port_id;
    bcm_trunk_t trunk_id;
    int local_id;
    soc_higig_hdr_t *xgh = (soc_higig_hdr_t *)pkt._higig;
    
    sal_memset(&pkt, 0, sizeof(bcm_pkt_t));
    
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, endpoint_info->gport,
        &module_id, &port_id, &trunk_id, &local_id));

    if (BCM_GPORT_IS_MIM_PORT(endpoint_info->gport) ||
        BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
        vp = 1;
    }
    
    soc_higig_field_init(unit, xgh);
    soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG2_START);
    soc_higig_field_set(unit, xgh, HG_opcode, SOC_HIGIG_OP_UC);
    soc_higig_field_set(unit, xgh, HG_cos, 0x7);
    
    if (vp) {
        soc_higig_field_set(unit, xgh, HG_ppd_type, 0x2);
        soc_higig_field_set(unit, xgh, HG_fwd_type, 0x4);
        soc_higig_field_set(unit, xgh, HG_multipoint, 0x0);
        soc_higig_field_set(unit, xgh, HG_dst_vp, local_id);
        /* unicast */
        soc_higig_field_set(unit, xgh, HG_dst_type, 0);
        soc_higig_field_set(unit, xgh, HG_src_vp, local_id);
        soc_higig_field_set(unit, xgh, HG_src_type, 0);
    } else {
        soc_higig_field_set(unit, xgh, HG_ppd_type, 0x0);
        soc_higig_field_set(unit, xgh, HG_dst_mod, module_id);
        soc_higig_field_set(unit, xgh, HG_dst_port, port_id+1);
        soc_higig_field_set(unit, xgh, HG_src_mod, module_id);
        soc_higig_field_set(unit, xgh, HG_src_port, port_id);
        soc_higig_field_set(unit, xgh, HG_vlan_tag, endpoint_info->vlan);
        soc_higig_field_set(unit, xgh, HG_ingress_tagged, 1);
    }

    pkt.alloc_ptr = (uint8 *)soc_cm_salloc(unit, 128, "TX");        
    if (pkt.alloc_ptr == NULL) {        
        printk("WARNING: Could not alloc tx buffer. Memory error.\n");    
        return CMD_FAIL;
    } else {        
        pkt._pkt_data.data = pkt.alloc_ptr;        
        pkt.pkt_data = &pkt._pkt_data;        
        pkt.blk_count = 1;        
        pkt._pkt_data.len = 128;    
    }        

    /* setup the packet */    
    pkt.flags &= ~BCM_TX_CRC_FLD;    
    pkt.flags |= BCM_TX_CRC_REGEN; 
    pkt.flags |= BCM_TX_HG_READY | BCM_TX_ETHER;
    
    sal_memset(pkt.pkt_data[0].data, 0, pkt.pkt_data[0].len);        
    ep = (enet_hdr_t *)(pkt.pkt_data[0].data);
    
    ENET_SET_MACADDR(ep->en_dhost, mac);    
    ENET_SET_MACADDR(ep->en_shost, endpoint_info->src_mac_address);
    
    ep->en_tag_tpid = bcm_htons(0x8100);
    ep->en_tag_ctrl = bcm_htons(VLAN_CTRL(5, 0, endpoint_info->vlan));
    
    ep = (enet_hdr_t *)(pkt.pkt_data[0].data+4);
    ep->en_tag_tpid = bcm_htons(0x8100);
    ep->en_tag_ctrl = bcm_htons(VLAN_CTRL(5, 0, endpoint_info->vlan));
    ep->en_tag_len  = bcm_htons(0x8902);

    op = (oam_hdr_t *)(&ep->en_snap_dsap);
    op->mdl_ver = (0x3 << 5);
    if (flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
        op->opcode = 43;
        op->first_tlvoffset = 12;
    } else {
        op->opcode = 47;
        op->first_tlvoffset = 32;
    }
    op->flags = 0x1;
    
    if ((rv = bcm_tx(unit, &pkt, NULL)) != BCM_E_NONE) {        
        soc_cm_sfree(unit, pkt.alloc_ptr);
        debugk(DK_ERR, "bcm_tx failed: Unit %d: %s\n",                   
            unit, bcm_errmsg(rv));
        return rv;        
    }

    return CMD_OK;
}
#endif /* BCM_ENDURO_SUPPORT */
char cmd_esw_oam_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: oam <option> [args...]\n"
#else
    "\n"
    "  oam init\n"
    "  oam group add [ID=<id>] Name=<name> [RemoteDefect]\n"
    "  oam group replace ID=<id> Name=<name> [RemoteDefect]\n"
    "  oam group delete <id>\n"
    "  oam group show\n"
    "  oam endpoint add Group=<group id> [ID=<id>] [Remote] [CCM] [LB] [DM] [LM]\n"
    "                   [LT] [PBBTE] [UPMEP] Name=name Level=<level> PERiod=<period>\n"
    "                   Vlan=<vlan>\n"
    "                   Port=<port>|ModPort=<modid.port>|TrunkGroupId=<tid>\n"
    "                   [SrcMACaddress=<mac>] [DstMACaddress=<mac>]\n"
    "                   [PKTPRI=<pri>] [INTPRI=<intpri>]\n"
    "  oam endpoint replace Group=<group id> [ID=<id>] [Remote] [CCM] [LB] [DM]\n"
    "                       [LM] [LT] [PBBTE] [UPMEP] [Name=name] Level=<level>\n"
    "                       Period=<period> Vlan=<vlan>\n"
    "                       Port=<port>|ModPort=<modid.port>|TrunkGroupId=<tid>\n"
    "                       [SrcMACaddress=<mac>] [DstMACaddress=<mac>]\n"
    "                       [PKTPRI=<pri>] [INTPRI=<intpri>]\n"
    "  oam endpoint delete <id>\n"
    "  oam endpoint show\n"
    "  oam events [on|off]\n"
#if defined(BCM_ENDURO_SUPPORT)
    "  oam tx ID=<id> Port=<port> [DM] [LM] [DstMACaddress=<mac>]\n"
#endif
#endif
    ;

cmd_result_t cmd_esw_oam(int unit, args_t *args)
{
    char *arg_string_p = NULL;
    parse_table_t parse_table;
    bcm_oam_group_info_t group_info;
    bcm_oam_endpoint_info_t endpoint_info;
    int remote_defect = 0;
    char *name_string_p = NULL;
    int is_remote = 0;
    int local_tx;
    int local_rx;
    int rx_ccm = 0;
    int rx_lb = 0;
    int rx_lm = 0;
    int rx_dm = 0;
    int rx_lt = 0;
    int pbbte = 0;
    int up_mep = 0;
    int endpoint_name;
    int level;
    int ccm_period;
    int vlan;
    bcm_port_t port;
    bcm_mod_port_t modport = {-1, -1};
    int trunk_id;
    int pkt_pri;
    int int_pri;
    bcm_oam_event_types_t event_types = {{0}};
    int result;
#if defined(BCM_ENDURO_SUPPORT)
    bcm_mac_t  mac;
#endif

    arg_string_p = ARG_GET(args);

    if (arg_string_p == NULL)
    {
        return CMD_USAGE;
    }

    if (!sh_check_attached(ARG_CMD(args), unit))
    {
        return CMD_FAIL;
    }

    if (sal_strcasecmp(arg_string_p, "init") == 0)
    {
        result = bcm_oam_init(unit);

        if (BCM_FAILURE(result))
        {
            printk("Command failed.  %s.\n", bcm_errmsg(result));

            return CMD_FAIL;
        }

        printk("OAM module initialized.\n");
    }
    else if (sal_strcasecmp(arg_string_p, "group") == 0)
    {
        arg_string_p = ARG_GET(args);

        if (arg_string_p == NULL ||
            sal_strcasecmp(arg_string_p, "show") == 0)
        {
            printk(GROUP_LIST_HEADER);

            if (BCM_FAILURE(bcm_oam_group_traverse(unit,
                _cmd_esw_oam_group_print, NULL)))
            {
                return CMD_FAIL;
            }
        }
        else if (sal_strcasecmp(arg_string_p, "delete") == 0)
        {
            if (BCM_FAILURE(bcm_oam_group_destroy(unit,
                parse_integer(ARG_GET(args)))))
            {
                return CMD_FAIL;
            }
        }
        else
        {
            bcm_oam_group_info_t_init(&group_info);

            if (sal_strcasecmp(arg_string_p, "replace") == 0)
            {
                group_info.flags |= BCM_OAM_GROUP_REPLACE;
            }
            else if (sal_strcasecmp(arg_string_p, "add") != 0)
            {
                printk("Invalid OAM group command: %s\n", arg_string_p);

                return CMD_FAIL;
            }

            parse_table_init(unit, &parse_table);

            parse_table_add(&parse_table, "ID", PQ_INT,
                (void *) BCM_OAM_GROUP_INVALID, &group_info.id, NULL);

            parse_table_add(&parse_table, "Name", PQ_STRING | PQ_DFL, 0,
                &name_string_p, NULL);

            parse_table_add(&parse_table, "RemoteDefect",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &remote_defect, NULL);

            if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
            {
                printk("Invalid option: %s\n", ARG_CUR(args));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if (group_info.id != BCM_OAM_GROUP_INVALID)
            {
                group_info.flags |= BCM_OAM_GROUP_WITH_ID;
            }

            if (name_string_p == NULL)
            {
                printk("A group name is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            sal_strncpy((char *)group_info.name, name_string_p,
                        BCM_OAM_GROUP_NAME_LENGTH - 1);

            if (remote_defect)
            {
                group_info.flags |= BCM_OAM_GROUP_REMOTE_DEFECT_TX;
            }

            result = bcm_oam_group_create(unit, &group_info);

            if (BCM_FAILURE(result))
            {
                printk("Command failed.  %s.\n", bcm_errmsg(result));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            parse_arg_eq_done(&parse_table);
            
            printk("OAM group %d created.\n", group_info.id);
        }
    }
#if defined(BCM_ENDURO_SUPPORT)
    else if (sal_strcasecmp(arg_string_p, "tx") == 0)
    {    
        parse_table_init(unit, &parse_table);

        parse_table_add(&parse_table, "ID", PQ_INT,
            (void *) BCM_OAM_GROUP_INVALID, &endpoint_info.id, NULL);

        parse_table_add(&parse_table, "DM",
            PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
            &rx_dm, NULL);

        parse_table_add(&parse_table, "LM",
            PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
            &rx_lm, NULL);
        
        parse_table_add(&parse_table, "Port", PQ_PORT, (void *) -1, &port, 
                        NULL);

        parse_table_add(&parse_table, "DstMACaddress", PQ_MAC | PQ_DFL, 0,
                &mac, NULL);
        
        if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
        {
            printk("Invalid option: %s\n", ARG_CUR(args));

            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }
        
        result = bcm_oam_endpoint_get(unit, endpoint_info.id, &endpoint_info);
        
        if (BCM_FAILURE(result))
        {
            printk("Command failed.  %s.\n", bcm_errmsg(result));
        
            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }
        
        if (!rx_dm && !rx_lm) {
            
            printk("DM or LM is required.\n");

            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }
        
        if (rx_dm) {
            pbbte |= BCM_OAM_ENDPOINT_DELAY_MEASUREMENT;
        } else {
            pbbte |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
        }

        result = _cmd_esw_oam_tx(unit, port, pbbte, &mac, &endpoint_info);
        if (BCM_FAILURE(result))
        {
            printk("Command failed.  %s.\n", bcm_errmsg(result));
        
            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }
        parse_arg_eq_done(&parse_table);
    }
#endif /* BCM_ENDURO_SUPPORT */
    else if (sal_strcasecmp(arg_string_p, "endpoint") == 0)
    {
        arg_string_p = ARG_GET(args);

        if (arg_string_p == NULL ||
            sal_strcasecmp(arg_string_p, "show") == 0)
        {
            
            printk(ENDPOINT_LIST_HEADER);

            if (BCM_FAILURE(bcm_oam_group_traverse(unit,
                _cmd_esw_oam_group_endpoints_traverse, NULL)))
            {
                return CMD_FAIL;
            }
        }
        else if (sal_strcasecmp(arg_string_p, "delete") == 0)
        {
            result = bcm_oam_endpoint_destroy(unit,
                parse_integer(ARG_GET(args)));

            if (BCM_FAILURE(result))
            {
                printk("Command failed. %s\n", bcm_errmsg(result));

                return CMD_FAIL;
            }
        }
        else
        {
            bcm_oam_endpoint_info_t_init(&endpoint_info);

            if (sal_strcasecmp(arg_string_p, "replace") == 0)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_REPLACE;
            }
            else if (sal_strcasecmp(arg_string_p, "add") != 0)
            {
                printk("Invalid OAM endpoint command: %s\n", arg_string_p);

                return CMD_FAIL;
            }
            
            parse_table_init(unit, &parse_table);

            parse_table_add(&parse_table, "Group", PQ_INT, (void *) -1,
                &endpoint_info.group, NULL);

            parse_table_add(&parse_table, "ID", PQ_INT,
                (void *) BCM_OAM_ENDPOINT_INVALID, &endpoint_info.id, NULL);

            parse_table_add(&parse_table, "Remote",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &is_remote, NULL);

            parse_table_add(&parse_table, "CCM",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_ccm, NULL);

            parse_table_add(&parse_table, "LB",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_lb, NULL);

            parse_table_add(&parse_table, "DM",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_dm, NULL);

            parse_table_add(&parse_table, "LM",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_lm, NULL);

            parse_table_add(&parse_table, "LT",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_lt, NULL);

            parse_table_add(&parse_table, "PBBTE",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &pbbte, NULL);

            parse_table_add(&parse_table, "UPMEP",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &up_mep, NULL);


            /* Needed for remote and local tx, not for local rx */

            parse_table_add(&parse_table, "Name", PQ_HEX, (void *) -1,
                &endpoint_name, NULL);

            /* Needed for all types */

            parse_table_add(&parse_table, "Level", PQ_INT, (void *) -1,
                &level, NULL);

            /* Needed for remote and local tx, not for local rx */

            parse_table_add(&parse_table, "PERiod", PQ_INT, (void *) -1,
                &ccm_period, NULL);

            /* Needed for all types */

            parse_table_add(&parse_table, "Vlan", PQ_INT,
                (void *) BCM_VLAN_INVALID, &vlan, NULL);

            parse_table_add(&parse_table, "Port", PQ_PORT, (void *) -1, &port,
                NULL);

            parse_table_add(&parse_table, "ModPort", PQ_MOD_PORT | PQ_DFL, 0,
                &modport, NULL);

            parse_table_add(&parse_table, "TrunkGroupId", PQ_INT,
                (void *) BCM_TRUNK_INVALID, &trunk_id, NULL);

            /* Needed for local tx, not remote or local rx */

            parse_table_add(&parse_table, "SrcMACaddress", PQ_MAC | PQ_DFL, 0,
                &endpoint_info.src_mac_address, NULL);

            parse_table_add(&parse_table, "DstMACaddress", PQ_MAC | PQ_DFL, 0,
                &endpoint_info.dst_mac_address, NULL);

            /* Needed for local tx, not remote or local rx */

            parse_table_add(&parse_table, "PKTPRI", PQ_INT, 0, &pkt_pri,
                NULL);

            /* Needed for local tx, not remote or local rx */

            parse_table_add(&parse_table, "INTPRI", PQ_INT, 0, &int_pri,
                NULL);

            if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
            {
                printk("Invalid option: %s\n", ARG_CUR(args));

                parse_arg_eq_done(&parse_table);

                return CMD_FAIL;
            }

            local_tx = !is_remote && (ccm_period > 0);
            local_rx = !is_remote && (rx_ccm || rx_lb || rx_dm || rx_lm || rx_lt);

            if (endpoint_info.id != BCM_OAM_ENDPOINT_INVALID)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
            }

            /* Check for parameters needed for all types */

            if (endpoint_info.group < 0)
            {
                printk("A group is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if (level < 0)
            {
                printk("A level is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if (vlan == BCM_VLAN_INVALID)
            {
                printk("A VLAN is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if (modport.port != -1)
            {
                BCM_GPORT_MODPORT_SET(endpoint_info.gport, modport.mod,
                    modport.port);
            }
            else if (port != -1)
            {
                BCM_GPORT_LOCAL_SET(endpoint_info.gport, port);
            }
            else if (trunk_id != BCM_TRUNK_INVALID)
            {
                BCM_GPORT_TRUNK_SET(endpoint_info.gport, trunk_id);
            }
            else
            {
                printk("An port or trunk group ID is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            endpoint_info.level = level;
            endpoint_info.vlan = vlan;

            /* Check for parameters needed for remote and local rx */

            if (is_remote || local_rx)
            {
                if (modport.port != -1)
                {
                    BCM_GPORT_MODPORT_SET(endpoint_info.gport, modport.mod,
                        modport.port);
                }
                else if (port != -1)
                {
                    BCM_GPORT_LOCAL_SET(endpoint_info.gport, port);
                }
                else if (trunk_id != BCM_TRUNK_INVALID)
                {
                    BCM_GPORT_TRUNK_SET(endpoint_info.gport, trunk_id);
                }
                else
                {
                    printk("An port or trunk group ID is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }
            }
            
            /* Check for parameters needed for remote and local tx */

            if (is_remote || local_tx)
            {
                if (endpoint_name < 0)
                {
                    printk("An endpoint name is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }

                if (ccm_period < 0)
                {
                    printk("A CCM period is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }

                endpoint_info.name = (uint16) endpoint_name;
                endpoint_info.ccm_period = ccm_period;
            }
            
            /* Check for parameters needed only for local tx */

            if (local_tx)
            {
                if (BCM_MAC_IS_ZERO(endpoint_info.src_mac_address))
                {
                    printk("A source MAC address is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }

#if defined(BCM_ENDURO_SUPPORT)
                if (SOC_IS_ENDURO(unit)) {
                    if (BCM_MAC_IS_ZERO(endpoint_info.dst_mac_address)) {
                        printk("A destination MAC address is required.\n");
                        CLEAN_UP_AND_RETURN(CMD_FAIL);
                    }
                }
#endif /* BCM_ENDURO_SUPPORT */

                if (pkt_pri < 0)
                {
                    printk("A packet priority is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }

                if (int_pri < 0)
                {
                    printk("An internal priority is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }

                endpoint_info.pkt_pri = pkt_pri;
                endpoint_info.int_pri = int_pri;
            }

            if (is_remote)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
            }

            if (rx_ccm)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_CCM_RX;
            }

            if (rx_lb)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_LOOPBACK;
            }

            if (rx_dm)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_DELAY_MEASUREMENT;
            }
            
            if (rx_lm)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
            }

            if (rx_lt)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_LINKTRACE;
            }
            
            if (pbbte)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_PBB_TE;
            }

            if (up_mep)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
            }

            endpoint_info.type = bcmOAMEndpointTypeEthernet;

            result = bcm_oam_endpoint_create(unit, &endpoint_info);

            if (BCM_FAILURE(result))
            {
                printk("Command failed. %s\n", bcm_errmsg(result));

                parse_arg_eq_done(&parse_table);

                return CMD_FAIL;
            }

            parse_arg_eq_done(&parse_table);
            
            printk("OAM endpoint %d created.\n", endpoint_info.id);
        }
    }
    else if (sal_strcasecmp(arg_string_p, "events") == 0)
    {
        arg_string_p = ARG_GET(args);

        if (arg_string_p != NULL)
        {
            if (sal_strcasecmp(arg_string_p, "on") == 0)
            {
                BCM_OAM_EVENT_TYPE_SET_ALL(event_types);

                if (BCM_FAILURE(bcm_oam_event_register(unit, event_types,
                    _cmd_esw_oam_event_handle, NULL)))
                {
                    return CMD_FAIL;
                }

                events_on = 1;
            }
            else if (sal_strcasecmp(arg_string_p, "off") == 0)
            {
                BCM_OAM_EVENT_TYPE_SET_ALL(event_types);

                if (BCM_FAILURE(bcm_oam_event_unregister(unit, event_types,
                    _cmd_esw_oam_event_handle)))
                {
                    return CMD_FAIL;
                }

                events_on = 0;
            }
            else
            {
                printk("Invalid OAM events option: %s\n", arg_string_p);

                return CMD_FAIL;
            }
        }

        printk("OAM events %s\n", events_on ? "on" : "off");
    }
    else
    {
        printk("Invalid OAM subcommand: %s\n", arg_string_p);

        return CMD_FAIL;
    }

    return CMD_OK;
}
