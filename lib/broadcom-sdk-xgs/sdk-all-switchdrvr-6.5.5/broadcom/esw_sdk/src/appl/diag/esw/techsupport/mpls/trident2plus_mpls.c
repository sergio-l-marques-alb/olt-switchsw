/*
 * $Id: trident2plus_mpls.c Broadcom SDK $
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
 * File:    trident2plus_mpls.c
 * Purpose: Maintains all the debug information for mpls
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

/* "mpls" feature's diag command list */
char * techsupport_mpls_diag_cmdlist[] = {
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
 * "mpls" feature for Trident2plus chipset. */
static soc_mem_t techsupport_mpls_trident2plus_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    PORT_TABm,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    MPLS_ENTROPY_LABEL_DATAm,
    MPLS_ENTRYm,
    VLAN_XLATEm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VLAN_MPLSm,
    VFI_1m,
    ING_MPLS_EXP_MAPPINGm,
    ING_VLAN_VFI_MEMBERSHIPm,
    L3_IIFm,
    L3_IIF_PROFILEm,
    VRFm,
    VLAN_PROFILE_TABm,
    VLAN_TABm,
    L2_USER_ENTRYm,
    L2_HITDA_ONLY_Xm,
    L2_HITDA_ONLY_Ym,
    L2_HITSA_ONLY_Xm,
    L2_HITSA_ONLY_Ym,
    L2Xm,
    L3_DEFIPm,
    RTAG7_PORT_BASED_HASHm,
    L3_DEFIP_PAIR_128m,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    INITIAL_PROT_NHI_TABLE_1m,
    TX_INITIAL_PROT_GROUP_TABLEm,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_ECMPm,
    L3_IPMCm,
    TRUNK_GROUPm,
    L3_ECMP_COUNTm,
    ING_EN_EFILTER_BITMAPm,
    L3_MTU_VALUESm,
    TRUNK_MEMBERm,
    VLAN_PROFILE_2m,
    ING_PW_TERM_SEQ_NUMm,
    TRUNK_BITMAPm,
    MMU_REPL_GROUP_INFO0m,
    MMU_REPL_GROUP_INFO1m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
    EGR_DVP_ATTRIBUTEm,
    EGR_GPP_ATTRIBUTESm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_MAC_DA_PROFILEm,
    EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
    EGR_PORTm,
    EGR_VFIm,
    EGR_VLAN_CONTROL_1m,
    EGR_IP_TUNNEL_MPLSm,
    EGR_VLANm,
    EGR_VLAN_VFI_MEMBERSHIPm,
    EGR_MPLS_EXP_MAPPING_1m,
    EGR_MPLS_EXP_MAPPING_2m,
    EGR_MPLS_PRI_MAPPINGm,
    EGR_TRILL_RBRIDGE_NICKNAMESm,
    EGR_MPLS_EXP_PRI_MAPPINGm,
    EGR_PORT_1m,
    EGR_PW_INIT_COUNTERSm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "mpls" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_mpls_trident2plus_reg_list[] = {
    {CPU_CONTROL_1r, register_type_global},
    {CPU_CONTROL_Mr, register_type_global},
    {EGR_CONFIGr, register_type_global},
    {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global},
    {GLOBAL_MPLS_RANGE_1_LOWERr, register_type_global},
    {GLOBAL_MPLS_RANGE_1_UPPERr, register_type_global},
    {GLOBAL_MPLS_RANGE_2_LOWERr, register_type_global},
    {GLOBAL_MPLS_RANGE_2_UPPERr, register_type_global},
    {HASH_CONTROLr, register_type_global},
    {L3_DEFIP_RPF_CONTROLr, register_type_global},
    {MPLS_ENTROPY_LABEL_CONFIG_64r, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global},
    {RTAG7_HASH_CONTROL_3r, register_type_global},
    {RTAG7_HASH_SELr, register_type_global},
    {RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {XLMAC_RX_VLAN_TAGr, register_type_global},
    {RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_HASH_CONTROLr, register_type_global},
    {ING_MPLS_TPIDr, register_type_global},
    {ING_MISC_CONFIGr, register_type_global},
    {ING_MPLS_INNER_TPIDr, register_type_global},
    {MPLS_ENTRY_HASH_CONTROLr, register_type_global},
    {RTAG7_MPLS_L2_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "mpls" feature for Trident2plus chipset. */
techsupport_data_t techsupport_mpls_trident2plus_data = {
    techsupport_mpls_diag_cmdlist,
    techsupport_mpls_trident2plus_reg_list,
    techsupport_mpls_trident2plus_memory_table_list
};


