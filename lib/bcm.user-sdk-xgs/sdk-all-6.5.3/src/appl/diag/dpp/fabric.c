/* 
 * $Id: tdm.c,v 1.0 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * File:        fabric.c
 * Purpose:     fabric CLI commands
 *
 */

#include <shared/bsl.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <appl/diag/dcmn/fabric.h>

#include <bcm/error.h>
#include <bcm/fabric.h>
#include <bcm/debug.h>

#include <bcm_int/dpp/utils.h>

#include <soc/dpp/mbcm.h>

STATIC cmd_result_t _cmd_dpp_tdm_edit(int unit, args_t *args);

#define CMD_TDM_EDIT_USAGE \
    "    edit [show] [port=<port>]\n"     \
    "        - Show tdm editing info with given port\n"               

char cmd_dpp_tdm_usage[] =
    "\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "    tdm <option> [args...]\n"
#else
    CMD_TDM_EDIT_USAGE  "\n"
#endif
    ;

static cmd_t _cmd_dpp_tdm_list[] = {
    {"EDIT",   _cmd_dpp_tdm_edit,  "\n" CMD_TDM_EDIT_USAGE,  NULL}
};

STATIC int
_tdm_editing_print(int unit, bcm_gport_t gport, bcm_fabric_tdm_editing_t *editing)
{
    char type_str[20];
    char flags_str[60] = "";
    int print_uni = 0, print_mul = 0, print_userfield = 0;	
    int i = 0;

    switch (editing->type){
      case bcmFabricTdmEditingAppend:
          sal_strncpy(type_str, "Append", 20); 
          print_userfield = 1;
          break;
      case bcmFabricTdmEditingRemove:
          sal_strncpy(type_str, "Remove", 20); 
          break;
      case bcmFabricTdmEditingNoChange:
          sal_strncpy(type_str, "Nochange", 20); 
          break;
      case bcmFabricTdmEditingCustomExternal:
          sal_strncpy(type_str, "CustomExternal", 20); 
          break;  
      default:
          return BCM_E_NONE;
    }

    if (editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS) {
        sal_sprintf(flags_str, "%s", "Ingress");
    }

    if (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS) {
        sal_sprintf(flags_str, "%s", "Egress");
    }

    if (editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {
        sal_sprintf(flags_str + sal_strlen(flags_str), "%s", ",Unicast");
        print_uni = 1;
    }
	
    if (editing->flags & BCM_FABRIC_TDM_EDITING_MULTICAST) {
        sal_sprintf(flags_str + sal_strlen(flags_str), "%s", ",Multicast");
        print_mul = 1;
    }

    if (editing->flags & BCM_FABRIC_TDM_EDITING_NON_SYMMETRIC_CRC) {
        sal_sprintf(flags_str + sal_strlen(flags_str), "%s", ",Asymmetric_CRC");
    }

    cli_out("Port:%d flags:%s type:%s add_crc:%s", gport, flags_str, type_str, (editing->add_packet_crc ? "Y":"N")); 
    if (print_uni)
        cli_out(" Dest_Port:0x%8.8x", editing->destination_port);
    if (print_mul)
        cli_out(" MC:%d", editing->multicast_id);
    if (print_userfield) {
        cli_out(" User_Field_count:%d", editing->user_field_count);
        cli_out(" User_Field:0x");
        for (i = 0; i < BCM_FABRIC_TDM_USER_FIELD_MAX_SIZE; i++)
            cli_out("%2.2x", editing->user_field[i]);	
    }
    cli_out("\n");
    return BCM_E_NONE;
}

STATIC cmd_result_t
_cmd_dpp_tdm_edit(int unit, args_t *args)
{
    cmd_result_t       ret_code;
    int                rv = BCM_E_NONE;
    parse_table_t      pt;
    char	      *subcmd;
    bcm_port_t     port = 0;
    bcm_fabric_tdm_editing_t editing;
    int            one_entry_only = 0;    
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }
    
    if (sal_strcasecmp(subcmd, "show") == 0) {
        if (ARG_CNT(args) > 0) {			
            /* Parse option */
            parse_table_init(0, &pt);
            parse_table_add(&pt, "Port", PQ_DFL | PQ_INT, 0, (void *)&port, 0);
            if (!parseEndOk(args, &pt, &ret_code)) {
                return ret_code;
            }
            one_entry_only = 1;
        }
		
        if (one_entry_only) {
            /* Get ingress */
            bcm_fabric_tdm_editing_t_init(&editing);			
            editing.flags = BCM_FABRIC_TDM_EDITING_INGRESS;

            rv = bcm_fabric_tdm_editing_get(unit, port, &editing);
            if (BCM_FAILURE(rv)) {
                cli_out("%s: Error reading fabric tdm editing: %s\n",
                        ARG_CMD(args), bcm_errmsg(rv));
                return (CMD_FAIL);
            }
			
            _tdm_editing_print(unit, port, &editing);
			
            /* Get egress */
            bcm_fabric_tdm_editing_t_init(&editing);			
            editing.flags = BCM_FABRIC_TDM_EDITING_EGRESS;

            rv = bcm_fabric_tdm_editing_get(unit, port, &editing);
            if (BCM_FAILURE(rv)) {
                cli_out("%s: Error reading fabric tdm editing: %s\n",
                        ARG_CMD(args), bcm_errmsg(rv));
                return (CMD_FAIL);
            }
			
            _tdm_editing_print(unit, port, &editing);
        } 
        else {
            BCM_PBMP_ITER(PBMP_TDM_ALL(unit), port) {
                /* Get ingress */
                bcm_fabric_tdm_editing_t_init(&editing);			
                editing.flags = BCM_FABRIC_TDM_EDITING_INGRESS;

                rv = bcm_fabric_tdm_editing_get(unit, port, &editing);
                if (BCM_FAILURE(rv)) {
                    cli_out("%s: Error reading fabric tdm editing: %s\n",
                            ARG_CMD(args), bcm_errmsg(rv));
                    return (CMD_FAIL);
                }
				
                _tdm_editing_print(unit, port, &editing);

                /* Get egress */
                bcm_fabric_tdm_editing_t_init(&editing);			
                editing.flags = BCM_FABRIC_TDM_EDITING_EGRESS;

                rv = bcm_fabric_tdm_editing_get(unit, port, &editing);
                if (BCM_FAILURE(rv)) {
                    cli_out("%s: Error reading fabric tdm editing: %s\n",
                            ARG_CMD(args), bcm_errmsg(rv));
                    return (CMD_FAIL);
                }
				
                _tdm_editing_print(unit, port, &editing);				
            }
        }

    } 

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}


cmd_result_t
cmd_dpp_tdm(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
        _cmd_dpp_tdm_list, COUNTOF(_cmd_dpp_tdm_list));
}

cmd_result_t
diag_dpp_fabric_thresholds_types_parse(int unit, char *th_name, diag_dnx_fabric_link_th_info_t *thresholds_info)
{
    if (!sal_strncasecmp(th_name, "RCI", strlen(th_name))) {
        thresholds_info->th_types_rx_nof = 3;
        thresholds_info->th_types_rx[0] = bcmFabricLinkRxRciLvl1FC;
        thresholds_info->th_types_rx[1] = bcmFabricLinkRxRciLvl2FC;
        thresholds_info->th_types_rx[2] = bcmFabricLinkRxRciLvl3FC;
    } else if (!sal_strncasecmp(th_name, "LLFC", strlen(th_name))) {
        thresholds_info->th_types_rx_nof = 1;
        thresholds_info->th_types_rx[0] = bcmFabricLinkRxFifoLLFC;
    } else {
        cli_out("Threshold type is not supported (%s).\n", th_name);
        return CMD_USAGE;
    }
    return CMD_OK;
}
  
  
cmd_result_t
diag_dpp_fabric_thresholds_threshold_to_str(int unit, bcm_fabric_link_threshold_type_t type, char **type_name)
{
    switch (type)
    {
        /*LLFC*/
        case bcmFabricLinkRxFifoLLFC:
            *type_name = "LLFC";
            break;                                            
        /*RCI*/
        case bcmFabricLinkRxRciLvl1FC:
            *type_name = "RCI1";
            break;
        case bcmFabricLinkRxRciLvl2FC:
            *type_name = "RCI2";
            break;
        case bcmFabricLinkRxRciLvl3FC:
            *type_name = "RCI3";
            break;
        default:
            *type_name = "unknown";
            break;
    }
    return CMD_OK;
}
  
cmd_result_t
diag_dpp_fabric_queues_print(int unit)
{
    int rv;
    soc_dpp_fabric_queues_info_t queues_info;

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_fabric_queues_info_get,(unit, &queues_info));
    if (BCM_FAILURE(rv)){
        return CMD_FAIL;
    }

    /* print DTQ */

    cli_out("\n\nData read from DRAM to IPT Queues:\n\n");
    cli_out("=========\n\n");

    if (queues_info.soc_dpp_fabric_dtq_data_max_occ_val) {
        cli_out("Data cells: Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_dtq_data_max_occ_val, queues_info.soc_dpp_fabric_dtq_data_max_occ_ctx);
    }
    if (queues_info.soc_dpp_fabric_dtq_ctrl_max_occ_val) {
        cli_out("Control cells: Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_dtq_ctrl_max_occ_val, queues_info.soc_dpp_fabric_dtq_ctrl_max_occ_ctx);
    }
    /* print DBLF */

    cli_out("\n\nDRAM buffer pointers Queues:\n\n");
    cli_out("=========\n\n");

    if (queues_info.soc_dpp_fabric_dblf0_max_occ_val) {
        cli_out("DBLF 0: Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_dblf0_max_occ_val, queues_info.soc_dpp_fabric_dblf0_max_occ_ctx);
    }
    if (queues_info.soc_dpp_fabric_dblf1_max_occ_val) {
        cli_out("DBLF 1: Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_dblf1_max_occ_val, queues_info.soc_dpp_fabric_dblf1_max_occ_ctx);
    }

    return CMD_OK;
}
