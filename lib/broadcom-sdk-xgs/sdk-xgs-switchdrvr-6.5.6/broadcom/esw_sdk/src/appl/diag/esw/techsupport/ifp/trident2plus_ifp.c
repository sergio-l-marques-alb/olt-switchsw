/*
 * $Id: trident2plus_ifp.c Broadcom SDK $
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
 * File:    trident2plus_ifp.c
 * Purpose: Maintains all the debug information for ifp
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_ifp_diag_cmdlist[];
extern char * techsupport_ifp_sw_dump_cmdlist[];

/* "ifp" feature's diag command list */
char * techsupport_ifp_trident2plus_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "ifp" feature's software dump command list */
char * techsupport_ifp_trident2plus_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Structure that maintains memory list for
 * "ifp" feature for Trident2plus chipset. */
static soc_mem_t techsupport_ifp_trident2plus_memory_table_list[] = {
    PORT_TABm,
    LPORT_TABm,
    PFS_PACKET_TYPE_OFFSETm,
    FP_PORT_FIELD_SELm,
    TTL_FNm,
    TOS_FNm,
    TCP_FNm,
    FP_COUNTER_TABLE_Xm,
    FP_COUNTER_TABLE_Ym,
    FP_GLOBAL_MASK_TCAM_Xm,
    FP_GLOBAL_MASK_TCAM_Ym,
    FP_GM_FIELDSm,
    FP_POLICY_TABLEm,
    FP_SLICE_KEY_CONTROLm,
    FP_SLICE_MAPm,
    FP_RANGE_CHECKm,
    FP_TCAMm,
    FP_I2E_CLASSID_SELECTm,
    FP_HG_CLASSID_SELECTm,
    FP_PORT_METER_MAPm,
    FP_METER_TABLEm,
    FP_METER_CONTROLm,
    IFP_REDIRECTION_PROFILEm,
    IFP_COS_MAPm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "ifp" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_ifp_trident2plus_reg_list[] = {
    {CPU_CONTROL_0r, register_type_global},
    {CPU_CONTROL_1r, register_type_global},
    {ING_BYPASS_CTRLr, register_type_global},
    {SW2_FP_DST_ACTION_CONTROLr, register_type_global},
    {IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr, register_type_global},
    {FP_SLICE_INDEX_CONTROLr, register_type_global},
    {FP_SLICE_ENABLEr, register_type_global},
    {FP_FORCE_FORWARDING_FIELDr, register_type_global},
    {FP_SLICE_METER_MAP_ENABLEr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "ifp" feature for Trident2plus chipset. */
techsupport_data_t techsupport_ifp_trident2plus_data = {
    techsupport_ifp_diag_cmdlist,
    techsupport_ifp_trident2plus_reg_list,
    techsupport_ifp_trident2plus_memory_table_list,
    techsupport_ifp_trident2plus_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_ifp_sw_dump_cmdlist,
    techsupport_ifp_trident2plus_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

