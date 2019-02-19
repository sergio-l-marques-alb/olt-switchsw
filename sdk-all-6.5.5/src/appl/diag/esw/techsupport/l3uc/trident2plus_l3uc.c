/*
 * $Id: trident2plus_l3uc.c Broadcom SDK $
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
 * File:    trident2plus_l3uc.c
 * Purpose: Maintains all the debug information for l3uc(layer 3 unicast)
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

/* "l3uc" feature's diag command list */
char * techsupport_l3uc_diag_cmdlist[] = {
    "l2 show",
    "vlan show",
    "l3 intf show",
    "l3 egress show",
    "l3 multipath show",
    "l3 l3table show",
    "l3 defip show",
#ifdef ALPM_ENABLE
    "l3 alpm show",
#endif
    "l3 ip6host show",
    "l3 ip6route show",
    "l3 tunnel_init show",
    "l3 tunnel_term show",
    "trunk show",
    NULL /* Must be the last element in this structure */
};
/* Structure that maintains memory list for
 * "l3uc" feature for Trident2plus chipset. */
static soc_mem_t techsupport_l3uc_trident2plus_memory_table_list[] = {
    PORT_TABm,
    MY_STATION_TCAMm,
    VLAN_MPLSm,
    ING_VLAN_VFI_MEMBERSHIPm,
    L3_IIFm,
    L3_IIF_PROFILEm,
    VRFm,
    VLAN_TABm,
    L3_ENTRY_IPV6_UNICASTm,
    ING_ACTIVE_L3_IIF_PROFILEm,
    L3_ENTRY_IPV4_UNICASTm,
    L3_DEFIPm,
    L3_DEFIP_PAIR_128m,
    L3_DEFIP_ALPM_IPV4_1m,
    L3_DEFIP_ALPM_IPV6_64m,
    L3_DEFIP_ALPM_IPV6_128m,
    L3_DEFIP_ALPM_IPV6_64_1m,
    L3_DEFIP_ONLYm,
    L3_DEFIP_DATA_ONLYm,
    L3_DEFIP_ALPM_IPV4m,
    INITIAL_ING_L3_NEXT_HOPm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    L3_DEFIP_HIT_ONLY_Xm,
    L3_DEFIP_HIT_ONLY_Ym,
    L3_ENTRY_HIT_ONLY_Xm,
    L3_ENTRY_HIT_ONLY_Ym,
    ING_L3_NEXT_HOPm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    L3_MTU_VALUESm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_VLANm,
    EGR_VLAN_VFI_MEMBERSHIPm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "l3uc" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_l3uc_trident2plus_reg_list[] = {
    {ING_CONFIG_64r, register_type_global},
    {L3_DEFIP_RPF_CONTROLr, register_type_global},
    {L2_TABLE_HASH_CONTROLr, register_type_global},
    {L3_TABLE_HASH_CONTROLr, register_type_global},
    {SHARED_TABLE_HASH_CONTROLr, register_type_global},
    {ISS_MEMORY_CONTROL_84r, register_type_global},
    {L3_DEFIP_ALPM_CFGr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "l3uc" feature for Trident2plus chipset. */
techsupport_data_t techsupport_l3uc_trident2plus_data = {
    techsupport_l3uc_diag_cmdlist,
    techsupport_l3uc_trident2plus_reg_list,
    techsupport_l3uc_trident2plus_memory_table_list
};

