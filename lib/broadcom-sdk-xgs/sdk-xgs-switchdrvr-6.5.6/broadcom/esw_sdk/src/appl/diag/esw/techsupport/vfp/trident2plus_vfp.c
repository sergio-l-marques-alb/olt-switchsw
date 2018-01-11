/*
 * $Id: trident2plus_vfp.c Broadcom SDK $
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
 * File:    trident2plus_vfp.c
 * Purpose: Maintains all the debug information for vfp
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_vfp_diag_cmdlist[];
extern char * techsupport_vfp_sw_dump_cmdlist[];

/* "vfp" feature's diag command list valid only for Trident2plus */
char * techsupport_vfp_trident2plus_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "vfp" feature's software dump command list valid only for Trident2plus */
char * techsupport_vfp_trident2plus_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "vfp" feature for Trident2plus chipset. */
static soc_mem_t techsupport_vfp_trident2plus_memory_table_list[] = {
    PORT_TABm,
    LPORT_TABm,
    VFP_TCAMm,
    VFP_POLICY_TABLEm,
    VFP_HASH_FIELD_BMAP_TABLE_Bm,
    VFP_HASH_FIELD_BMAP_TABLE_Am,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "vfp" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_vfp_trident2plus_reg_list[] = {
    {VFP_KEY_CONTROL_1r, register_type_global},
    {VFP_KEY_CONTROL_2r, register_type_global},
    {VFP_SLICE_CONTROLr, register_type_global},
    {VFP_SLICE_MAPr, register_type_global},
    {VFP_POLICY_TABLE_RAM_CONTROLr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "vfp" feature for Trident2plus chipset. */
techsupport_data_t techsupport_vfp_trident2plus_data = {
    techsupport_vfp_diag_cmdlist,
    techsupport_vfp_trident2plus_reg_list,
    techsupport_vfp_trident2plus_memory_table_list,
    techsupport_vfp_trident2plus_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_vfp_sw_dump_cmdlist,
    techsupport_vfp_trident2plus_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

