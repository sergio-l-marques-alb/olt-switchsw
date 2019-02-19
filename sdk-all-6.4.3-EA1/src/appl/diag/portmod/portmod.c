/*
 *         
 * $Id:$
 * 
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 *     
 */


#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/port.h>

#include <soc/portmod/portmod.h>


#define MAX_PORTS (500)
#define MAX_PMS (37)


char portmod_diag_usage[] = "portmod <options>\n"
     "Subcommands:\n"
     "  device - show PM information of the unit\n"
     "  port - print specified ports information\n"
     "device subcommand\n"
     "-----------------\n"  
     "  no arguments for this sub command"
     "port subcommand\n"
     "---------------\n"
     "  start_port <optional> - the first port to display\n"
     "  end_port <optional> - the last port to display\n"
     "  pm_id <optional> - show ports that belongs to specific PM\n"
     "  interface <optional> - show ports with specific interface type\n";

#ifdef PORTMOD_DIAG
STATIC cmd_result_t
enum_value_to_string(enum_mapping_t *mapping,  int val, char **str)
{
    char *key;
    int i = 0;

    key = mapping[0].key;
    while (key != NULL) {
        if (val == mapping[i].enum_value) {
            *str = mapping[i].key;
            return CMD_OK;
        }
        i++;
        key = mapping[i].key;
    }
    return CMD_FAIL;
}
#endif




STATIC
cmd_result_t
portmod_device_diag(int unit, args_t *a){
    int pm, rv;
    int phy;
    int range_start, range_end;
    char *pm_type = "";
    char *unknow_pm_type = "unknown";
    int is_first_range;
    int is_virtual = FALSE;
    portmod_pm_diag_info_t diag_info;

    /*no args*/
  
    if (ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
        return CMD_FAIL;
    }

    cli_out(" PM id |    type     |    phys    \n");
    cli_out("----------------------------------\n");
    for( pm = 0; pm <= MAX_PMS; pm++){
        rv = portmod_pm_diag_info_get(unit, pm, &diag_info);
        if (BCM_FAILURE(rv)){
            continue;
        }
        if(diag_info.type == portmodDispatchTypeCount){
            /*invalid port*/
            continue;
        }
#ifdef PORTMOD_DIAG
        rv = enum_value_to_string(portmod_dispatch_type_t_mapping, diag_info.type, &pm_type);
        if(rv != CMD_OK){
            pm_type = unknow_pm_type;
        }
        else{
            pm_type += sal_strlen("portmodDispatchType");
        }
#endif
        cli_out("   %02d  | %-10s  | ",  pm, pm_type);
        is_virtual = TRUE;
        /*add phys_str*/

        BCM_PBMP_ITER(diag_info.phys, phy){
            is_virtual = FALSE;
            break;
        }
        if(is_virtual){
            cli_out("\n");
            continue;
        } 
        
        range_start = phy;
        range_end = phy;
        is_first_range = TRUE;
        BCM_PBMP_ITER(diag_info.phys, phy){
            if(phy == range_start){ /*first one*/
                continue;
            }
            if(phy != range_end + 1){
                if(!is_first_range){
                    cli_out(",");
                }
                cli_out("%03d - %03d", range_start, range_end);
                range_start = phy;
                range_end = phy;
                is_first_range = FALSE;
            }
            else{
                range_end++;
            }
        }
        if(!is_first_range){
            cli_out(",");
        }
        cli_out("%03d - %03d\n", range_start, range_end);
        
    }
    return CMD_OK;
}


STATIC
cmd_result_t
portmod_port_diag(int unit, args_t *a){
    parse_table_t    pt;
    int start_port, end_port;
    int port, rv;
    int phy;
    int range_start, range_end;
    int is_first_range;
    int is_virtual = FALSE;
    char *interface_types_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
    portmod_port_diag_info_t diag_info;
  
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "start_port", PQ_INT,
                    (void *)0, &start_port, 0);
    parse_table_add(&pt, "end_port", PQ_INT, (void *)MAX_PORTS, &end_port, 0);

    if (parse_arg_eq(a, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if (ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    cli_out("port | alias | PM id | port type |         phys         \n");
    cli_out("--------------------------------------------------------\n");
    for( port = start_port; port <= end_port; port++){
        rv = portmod_port_diag_info_get(unit, port, &diag_info);
        if (BCM_FAILURE(rv)){
            continue;
        }
        if(diag_info.pm_id < 0){
            /*invalid port*/
            continue;
        }
        if(diag_info.original_port == port){
            cli_out(" %03d |       |   %02d  | %-9s | ", port, diag_info.pm_id, interface_types_names[diag_info.interface]);
        }
        else{
            cli_out(" %03d | %03d |   %02d  | %-9s | ", port, diag_info.original_port, diag_info.pm_id, interface_types_names[diag_info.interface]);
        }
        is_virtual = TRUE;
        /*add phys_str*/

        BCM_PBMP_ITER(diag_info.phys, phy){
            is_virtual = FALSE;
            break;
        }
        if(is_virtual){
            cli_out("virtual\n");
            continue;
        } 
        if(diag_info.interface == SOC_PORT_IF_QSGMII){
            /* coverity[uninit_use_in_call] */
            cli_out("%03d.%d\n", phy, diag_info.sub_phy);
            continue;
        }
        
        range_start = phy;
        range_end = phy;
        is_first_range = TRUE;
        BCM_PBMP_ITER(diag_info.phys, phy){
            if(phy == range_start){ /*first one*/
                continue;
            }
            if(phy != range_end + 1){
                if(!is_first_range){
                    cli_out(",");
                }
                if(range_start == range_end){
                    cli_out("%03d", range_start);
                } else {
                    cli_out("%03d - %03d", range_start, range_end);
                }
                range_start = phy;
                range_end = phy;
                is_first_range = FALSE;
            }
            else{
                range_end++;
            }
        }
        if(!is_first_range){
            cli_out(",");
        }
        if(range_start == range_end){
            cli_out("%03d\n", range_start);
        } else {
            cli_out("%03d - %03d\n", range_start, range_end);
        }
        
    }
    return CMD_OK;
}



cmd_result_t
cmd_portmod_diag(int unit, args_t *a){
    char *c;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }
    c = ARG_GET(a);
    if (c == NULL){
        return CMD_USAGE;
    }
    if(sal_strcasecmp(c, "port") == 0) {
        return portmod_port_diag(unit, a);
    }
    else if(sal_strcasecmp(c, "device") == 0){
        return portmod_device_diag(unit,a);
    }
    return CMD_USAGE;
    
}

