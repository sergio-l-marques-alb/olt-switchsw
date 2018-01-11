/*
 * $Id: tomahawk_efp.c Broadcom SDK $
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
 * File:    tomahawk_efp.c
 * Purpose: Maintains all the debug information for efp
 *          feature for tomahawk.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_efp_diag_cmdlist[];
extern char * techsupport_efp_sw_dump_cmdlist[];

/* "efp" feature's diag command list valid only for tomahawk */
char * techsupport_efp_tomahawk_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "efp" feature's software dump command list valid only for tomahawk */
char * techsupport_efp_tomahawk_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "efp" feature for tomahawk chipset. */
static soc_mem_t techsupport_efp_tomahawk_memory_table_list[] = {
    EFP_COUNTER_TABLE_PIPE0m,
    EFP_COUNTER_TABLE_PIPE1m,
    EFP_COUNTER_TABLE_PIPE2m,
    EFP_COUNTER_TABLE_PIPE3m,
    EFP_METER_TABLE_PIPE0m,
    EFP_METER_TABLE_PIPE1m,
    EFP_METER_TABLE_PIPE2m,
    EFP_METER_TABLE_PIPE3m,
    EFP_POLICY_TABLE_PIPE0m,
    EFP_POLICY_TABLE_PIPE1m,
    EFP_POLICY_TABLE_PIPE2m,
    EFP_POLICY_TABLE_PIPE3m,
    EFP_TCAM_PIPE0m,
    EFP_TCAM_PIPE1m,
    EFP_TCAM_PIPE2m,
    EFP_TCAM_PIPE3m,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "efp" feature for tomahawk chipset. */
techsupport_reg_t techsupport_efp_tomahawk_reg_list[] = {
   {EFP_CLASSID_SELECTOR_PIPE0r, register_type_global },
   {EFP_CLASSID_SELECTOR_PIPE1r, register_type_global },
   {EFP_CLASSID_SELECTOR_PIPE2r, register_type_global },
   {EFP_CLASSID_SELECTOR_PIPE3r, register_type_global },
   {EFP_KEY4_DVP_SELECTOR_PIPE0r, register_type_global },
   {EFP_KEY4_DVP_SELECTOR_PIPE1r, register_type_global },
   {EFP_KEY4_DVP_SELECTOR_PIPE2r, register_type_global },
   {EFP_KEY4_DVP_SELECTOR_PIPE3r, register_type_global },
   {EFP_KEY4_MDL_SELECTOR_PIPE0r, register_type_global },
   {EFP_KEY4_MDL_SELECTOR_PIPE1r, register_type_global },
   {EFP_KEY4_MDL_SELECTOR_PIPE2r, register_type_global },
   {EFP_KEY4_MDL_SELECTOR_PIPE3r, register_type_global },
   {EFP_KEY8_DVP_SELECTOR_PIPE0r, register_type_global },
   {EFP_KEY8_DVP_SELECTOR_PIPE1r, register_type_global },
   {EFP_KEY8_DVP_SELECTOR_PIPE2r, register_type_global },
   {EFP_KEY8_DVP_SELECTOR_PIPE3r, register_type_global },
   {EFP_METER_CONTROL_PIPE0r, register_type_global },
   {EFP_METER_CONTROL_PIPE1r, register_type_global },
   {EFP_METER_CONTROL_PIPE2r, register_type_global },
   {EFP_METER_CONTROL_PIPE3r, register_type_global },
   {EFP_SLICE_CONTROL_PIPE0r, register_type_global },
   {EFP_SLICE_CONTROL_PIPE1r, register_type_global },
   {EFP_SLICE_CONTROL_PIPE2r, register_type_global },
   {EFP_SLICE_CONTROL_PIPE3r, register_type_global },
   {EFP_SLICE_MAP_PIPE0r, register_type_global },
   {EFP_SLICE_MAP_PIPE1r, register_type_global },
   {EFP_SLICE_MAP_PIPE2r, register_type_global },
   {EFP_SLICE_MAP_PIPE3r, register_type_global },
   {EGR_EFP_CNTR_UPDATE_CONTROL_PIPE0r, register_type_global },
   {EGR_EFP_CNTR_UPDATE_CONTROL_PIPE1r, register_type_global },
   {EGR_EFP_CNTR_UPDATE_CONTROL_PIPE2r, register_type_global },
   {EGR_EFP_CNTR_UPDATE_CONTROL_PIPE3r, register_type_global },
   {EGR_EFP_EVICTION_CONTROL_PIPE0r, register_type_global },
   {EGR_EFP_EVICTION_CONTROL_PIPE1r, register_type_global },
   {EGR_EFP_EVICTION_CONTROL_PIPE2r, register_type_global },
   {EGR_EFP_EVICTION_CONTROL_PIPE3r, register_type_global },
   {EGR_EFP_EVICTION_SEED_PIPE0r, register_type_global },
   {EGR_EFP_EVICTION_SEED_PIPE1r, register_type_global },
   {EGR_EFP_EVICTION_SEED_PIPE2r, register_type_global },
   {EGR_EFP_EVICTION_SEED_PIPE3r, register_type_global },
   {EGR_EFP_EVICTION_THRESHOLD_PIPE0r, register_type_global },
   {EGR_EFP_EVICTION_THRESHOLD_PIPE1r, register_type_global },
   {EGR_EFP_EVICTION_THRESHOLD_PIPE2r, register_type_global },
   {EGR_EFP_EVICTION_THRESHOLD_PIPE3r, register_type_global },
   {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "efp" feature for tomahawk chipset. */
techsupport_data_t techsupport_efp_tomahawk_data = {
    techsupport_efp_diag_cmdlist,
    techsupport_efp_tomahawk_reg_list,
    techsupport_efp_tomahawk_memory_table_list,
    techsupport_efp_tomahawk_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_efp_sw_dump_cmdlist,
    techsupport_efp_tomahawk_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

