/*
 * $Id: trident2plus_riot.c Broadcom SDK $
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
 * File:    trident2plus_riot.c
 * Purpose: Maintains all the debug information for riot
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

/* "riot" feature's diag command list */
char * techsupport_riot_diag_cmdlist[] = {
    "l2 show",
    "vlan show",
    "vlan translate show",
    "vlan action translate show",
    "vlan translate egress show",
    "vlan action translate egress show",
    "l3 intf show",
    "l3 egress show",
    "l3 multipath show",
    "l3 l3table show",
    "l3 defip show",
    "multicast show",
    "trunk show",
    NULL /* Must be the last element in this structure */
};
/* Structure that maintains memory list for
 * "riot" feature for Trident2plus chipset. */
static soc_mem_t techsupport_riot_trident2plus_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    PORT_TABm,
    SOURCE_VP_2m,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    VLAN_XLATEm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VLAN_MPLSm,
    ING_VLAN_VFI_MEMBERSHIPm,
    L3_IIFm,
    L3_IIF_PROFILEm,
    MY_STATION_TCAM_2m,
    VFI_PROFILEm,
    VLAN_TABm,
    VLAN_PROFILE_TABm,
    L2_USER_ENTRYm,
    L2Xm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    L3_DEFIPm,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    L3_IPMC_1m,
    INITIAL_L3_ECMP_GROUPm,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_IPMCm,
    L3_IPMC_REMAPm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    L3_MTU_VALUESm,
    MMU_REPL_GROUP_INFO0m,
    MMU_REPL_GROUP_INFO1m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
    EGR_DGPP_TO_NHIm,
    EGR_DGPP_TO_NHI_MODBASEm,
    EGR_DVP_ATTRIBUTEm,
    EGR_DVP_ATTRIBUTE_1m,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_PORTm,
    EGR_VFIm,
    EGR_FRAGMENT_ID_TABLEm,
    EGR_IP_TUNNELm,
    EGR_VLANm,
    EGR_VLAN_VFI_MEMBERSHIPm,
    EGR_VLAN_XLATEm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "riot" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_riot_trident2plus_reg_list[] = {
    {EGR_TUNNEL_ID_MASKr, register_type_global},
    {EGR_VXLAN_CONTROLr, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global},
    {VXLAN_CONTROLr, register_type_global},
    {VXLAN_DEFAULT_NETWORK_SVPr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {EGR_CONFIG_1r, register_type_global},
    {EGR_NIV_ETHERTYPEr, register_type_global},
    {EGR_NIV_ETHERTYPE_2r, register_type_global},
    {ING_MISC_CONFIG2r, register_type_global},
    {NIV_ETHERTYPEr, register_type_global},
    {ECMP_CONFIGr, register_type_global},
    {MMU_ENQ_LOGICAL_PORT_TO_PORT_AGG_MAPr, register_type_global},
    {MMU_ENQ_REPL_PORT_AGG_MAPr, register_type_global},
    {MMU_TOQ_REPL_PORT_AGG_MAPr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "riot" feature for Trident2plus chipset. */
techsupport_data_t techsupport_riot_trident2plus_data = {
    techsupport_riot_diag_cmdlist,
    techsupport_riot_trident2plus_reg_list,
    techsupport_riot_trident2plus_memory_table_list
};


