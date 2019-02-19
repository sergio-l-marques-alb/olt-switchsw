/*
 * $Id: trident2plus_vlan.c Broadcom SDK $
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
 * File:    trident2plus_vlan.c
 * Purpose: Maintains all the debug information for vlan
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

/* "vlan" feature's diag command list */
char * techsupport_vlan_diag_cmdlist[] = {
    "l2 show",
    "vlan show",
    "vlan translate show",
    "vlan action translate show",
    "vlan translate egress show",
    "vlan action translate egress show",
    "vlan translate range show",
    "vlan action translate range show",
    "vlan translate dtag show",
    "trunk show",
    NULL /* Must be the last element in this structure */
};
/* Structure that maintains memory list for
 * "vlan" feature for Trident2plus chipset. */
static soc_mem_t techsupport_vlan_trident2plus_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    PORT_TABm,
    ING_VLAN_RANGEm,
    SOURCE_VP_2m,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    VLAN_PROTOCOLm,
    VLAN_PROTOCOL_DATAm,
    VLAN_SUBNETm,
    VLAN_XLATE_LPm,
    VLAN_XLATEm,
    VLAN_MACm,
    SOURCE_VPm,
    VLAN_MPLSm,
    ING_VLAN_VFI_MEMBERSHIPm,
    STG_TABm,
    VLAN_PROFILE_TABm,
    VLAN_TABm,
    L2_HITDA_ONLYm,
    L2_HITSA_ONLYm,
    L2_USER_ENTRYm,
    L2_LEARN_INSERT_FAILUREm,
    L2_BULK_REPLACE_MASKm,
    L2_BULK_REPLACE_DATAm,
    L2_BULK_MATCH_MASKm,
    L2_BULK_MATCH_DATAm,
    L2_MOD_FIFOm,
    L2_ENDPOINT_IDm,
    L2Xm,
    ING_DVP_TABLEm,
    TRUNK_CBL_TABLEm,
    PORT_CBL_TABLE_MODBASEm,
    ING_DVP_2_TABLEm,
    L2MCm,
    TRUNK_GROUPm,
    TRUNK_MEMBERm,
    VLAN_PROFILE_2m,
    VFI_PROFILE_2m,
    ING_EN_EFILTER_BITMAPm,
    TRUNK_BITMAPm,
    EGR_DVP_ATTRIBUTEm,
    EGR_GPP_ATTRIBUTESm,
    EGR_MAP_MHm,
    EGR_PORTm,
    EGR_VLAN_CONTROL_1m,
    EGR_VLANm,
    EGR_VLAN_TAG_ACTION_PROFILEm,
    EGR_VLAN_VFI_MEMBERSHIPm,
    EGR_VLAN_XLATEm,
    EGR_VLAN_STGm,
    EGR_VLAN_CONTROL_3m,
    EGR_VLAN_CONTROL_2m,
    EGR_TRILL_RBRIDGE_NICKNAMESm,
    EGR_PORT_1m,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "vlan" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_vlan_trident2plus_reg_list[] = {
    {CPU_CONTROL_1r, register_type_global},
    {EGR_CONFIGr, register_type_global},
    {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global},
    {EGR_PVLAN_EPORT_CONTROLr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {VLAN_XLATE_HASH_CONTROLr, register_type_global},
    {ING_OUTER_TPIDr, register_type_global},
    {ING_MPLS_INNER_TPIDr, register_type_global},
    {EGR_OUTER_TPIDr, register_type_global},
    {IARB_LEARN_CONTROLr, register_type_global},
    {CBL_ATTRIBUTEr, register_type_global},
    {MC_CONTROL_1r, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "vlan" feature for Trident2plus chipset. */
techsupport_data_t techsupport_vlan_trident2plus_data = {
    techsupport_vlan_diag_cmdlist,
    techsupport_vlan_trident2plus_reg_list,
    techsupport_vlan_trident2plus_memory_table_list
};

