/*
 * $Id: techsupport.h  Broadcom SDK $
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
 * File:    techsupport.h
 * Purpose: This file contains the declarations of all the functions and
 *          data structures that is being used in the TechSupport utility.
*/

#include <appl/diag/parse.h>
#include <soc/mcm/allenum.h>
#include <appl/diag/shell.h>

#define DUMP_TABLE_CHANGED      (0x1 << 0)
#define DUMP_DIAG_CMDS          (0x1 << 1)
#define DUMP_REGISTERS          (0x1 << 2)
#define DUMP_MEMORIES           (0x1 << 3)
#define DUMP_LIST               (0x1 << 4)
#define DUMP_SW                 (0x1 << 5)

#define DUMP    "dump "
#define GET_REG "getreg "
#define CHANGE  "chg "
#define DIAG    "diag"
#define REG     "reg"
#define MEM     "mem"
#define VERBOSE "verbose"
#ifndef BCM_SW_STATE_DUMP_DISABLE
#define SW "sw"
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#define LIST    "list"
#define TECHSUPPORT "techsupport "
#define MAX_STR_LEN 80

#define COMMAND_BASIC  "basic"
#define COMMAND_COS    "cos"
#define COMMAND_EFP    "efp"
#define COMMAND_IFP    "ifp"
#define COMMAND_L3UC   "l3uc"
#define COMMAND_L3MC   "l3mc"
#define COMMAND_MPLS   "mpls"
#define COMMAND_MMU    "mmu"
#define COMMAND_RIOT   "riot"
#define COMMAND_NIV    "niv"
#define COMMAND_VFP    "vfp"
#define COMMAND_VLAN   "vlan"
#define COMMAND_VXLAN  "vxlan"
#define COMMAND_LOAD_BALANCE "load-balance"


 /* registername and type(global or per port) */

typedef enum register_type_e {
    register_type_per_port = 1,
    register_type_global
}register_type_t;

typedef struct techsupport_reg_s {
    soc_reg_t reg;
    register_type_t register_type; /*Currently all registers default to register_type_global*/
} techsupport_reg_t ;


typedef struct techsupport_data_s {
    char ** techsupport_data_diag_cmdlist;
    techsupport_reg_t *techsupport_data_reg_list;
    soc_mem_t *techsupport_data_mem_list;
    char ** techsupport_data_device_diag_cmdlist;
#ifndef BCM_SW_STATE_DUMP_DISABLE
    char ** techsupport_data_sw_dump_cmdlist;
    char ** techsupport_data_device_sw_dump_cmdlist;
#endif /* BCM_SW_STATE_DUMP_DISABLE */
 }techsupport_data_t;

typedef struct mbcm_techsupport_s {
    const char *techsupport_feature_name;
    techsupport_data_t *techsupport_data;
} mbcm_techsupport_t;

int techsupport_feature_process(int unit, args_t *a,
                          techsupport_data_t *techsupport_feature_data);

void techsupport_command_execute(int unit, char *command);
cmd_result_t command_techsupport(int unit, args_t *a);
int techsupport_l3uc(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_basic(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_l3mc(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_mpls(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_mmu(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_niv(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_riot(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_vlan(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_vxlan(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_cos(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_efp(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_ifp(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_vfp(int unit, args_t *a, techsupport_data_t *techsupport_data);
int techsupport_loadbalance(int unit, args_t *a, techsupport_data_t *techsupport_data);
