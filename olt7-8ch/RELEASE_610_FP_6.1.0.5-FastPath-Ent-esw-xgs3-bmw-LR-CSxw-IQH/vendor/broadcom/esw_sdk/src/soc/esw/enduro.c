/*
 * $Id: enduro.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        enduro.c
 * Purpose:
 * Requires:    
 */


#include <sal/core/boot.h>

#include <soc/firebolt.h>
#include <soc/bradley.h>
#include <soc/enduro.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/lpm.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/er_tcam.h>
#include <soc/memtune.h>
#include <soc/devids.h>

#ifdef BCM_ENDURO_SUPPORT

/*
 * Enduro chip driver functions.  
 */
soc_functions_t soc_enduro_drv_funs = {
    soc_enduro_misc_init,
    soc_enduro_mmu_init,
    soc_enduro_age_timer_get,
    soc_enduro_age_timer_max_get,
    soc_enduro_age_timer_set,
};

typedef enum {
    _SOC_PARITY_INFO_TYPE_GENERIC,
    _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
    _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
    _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER,
    _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
    _SOC_PARITY_INFO_TYPE_MMU_PARITY,
    _SOC_PARITY_INFO_TYPE_MMUIPMC,
    _SOC_PARITY_INFO_TYPE_MMUWRED,
    _SOC_PARITY_INFO_TYPE_OAM, /* Not parity, but same interrupt */
    _SOC_PARITY_INFO_TYPE_NUM
} _soc_parity_info_type_t;

typedef struct _soc_parity_info_s {
    soc_field_t             enable_field;
    soc_field_t             error_field;
    char                    *msg;
    soc_mem_t               mem;
    _soc_parity_info_type_t type;
    soc_reg_t               control_reg;
    soc_reg_t               intr_status0_reg;
    soc_reg_t               intr_status1_reg; /* Also SBE force for ECC */
    soc_reg_t               nack_status0_reg;
    soc_reg_t               nack_status1_reg; /* Also DBE force for ECC */
} _soc_parity_info_t;

/*
 *    _SOC_PARITY_INFO_TYPE_SINGLE_PARITY
 *      PARITY_EN
 *      ENTRY_IDX, MULTIPLE_ERR, PARITY_ERR
 *
 *    _SOC_PARITY_INFO_TYPE_SINGLE_ECC
 *      ECC_EN
 *      ENTRY_IDX, DOUBLE_BIT_ERR, MULTIPLE_ERR, ECC_ERR
 *
 *    _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER
 *      PARITY_EN
 *      PORT_IDX, COUNTER_IDX, MULTIPLE_ERR, PARITY_ERR
 *
 *    _SOC_PARITY_INFO_TYPE_DUAL_PARITY
 *      PARITY_EN
 *      BUCKET_IDX, MULTIPLE_ERR, PARITY_ERR_BM
 */

STATIC _soc_parity_info_t _soc_en_ip0_parity_info[] = {
    { PARITY_ENf, VXLT_PAR_ERRf, NULL,
      VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      VLAN_XLATE_PARITY_CONTROLr,
      VLAN_XLATE_PARITY_STATUS_0r, VLAN_XLATE_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
 /* { PPA_CMD_COMPLETEf }, */
 /* { MEM_RESET_COMPLETEf }, */
 /* { AGE_CMD_COMPLETEf }, */
    { PARITY_ENf, VLAN_SUBNET_DATA_PARITY_ERRf, NULL,
      VLAN_SUBNET_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_SUBNET_DATA_PARITY_CONTROLr,
      VLAN_SUBNET_DATA_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PROTOCOL_DATA_PARITY_ERRf, NULL,
      VLAN_PROTOCOL_DATAm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PROTOCOL_DATA_PARITY_CONTROLr,
      VLAN_PROTOCOL_DATA_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PARITY_ERRf, NULL,
      VLAN_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PARITY_CONTROLr,
      VLAN_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_STG_PARITY_ERRf, NULL,
      STG_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_STG_PARITY_CONTROLr,
      VLAN_STG_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_TUNNEL_RAM_PARITY_ERRf, NULL,
      L3_TUNNELm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_TUNNEL_DATA_ONLY_PARITY_CONTROLr,
      L3_TUNNEL_DATA_ONLY_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, SOURCE_TRUNK_MAP_PARITY_ERRf, NULL,
      SOURCE_TRUNK_MAP_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SOURCE_TRUNK_MAP_PARITY_CONTROLr,
      SOURCE_TRUNK_MAP_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, VFP_POLICY_PAR_ERRf, NULL,
      VFP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFP_POLICY_PARITY_CONTROLr,
      VFP_POLICY_PARITY_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, LMEP_PAR_ERRf, NULL,
      LMEPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      LMEP_PARITY_CONTROLr,
      LMEP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, IARB_PKT_ECC_INTRf, "Iarb packet ecc error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      IARB_PKT_ECC_CONTROLr,
      IARB_PKT_ECC_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, IARB_HDR_ECC_INTRf, "Iarb header ecc error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      IARB_HDR_ECC_CONTROLr,
      IARB_HDR_ECC_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_en_ip1_parity_info[] = {
    { PARITY_ENf, VFI_PAR_ERRf, NULL,
      VFIm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFI_PARITY_CONTROLr,
      VFI_PARITY_STATUS_INTRr, INVALIDr,
      VFI_PARITY_STATUS_NACKr, INVALIDr },
    { PARITY_ENf, SVP_PAR_ERRf, NULL,
      SOURCE_VPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SOURCE_VP_PARITY_CONTROLr,
      SOURCE_VP_PARITY_STATUSr, INVALIDr,
      SOURCE_VP_PARITY_STATUS_NACKr, INVALIDr },
    { PARITY_ENf, L3_IIF_PAR_ERRf, NULL,
      L3_IIFm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_IIF_PARITY_CONTROLr,
      L3_IIF_PARITY_STATUSr, INVALIDr,
      L3_IIF_PARITY_STATUS_NACKr, INVALIDr },
    { PARITY_ENf, MPLS_ENTRY_PAR_ERRf, NULL,
      MPLS_ENTRYm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      MPLS_ENTRY_PARITY_CONTROLr,
      MPLS_ENTRY_PARITY_STATUS_INTR_0r, MPLS_ENTRY_PARITY_STATUS_INTR_1r,
      MPLS_ENTRY_PARITY_STATUS_NACK_0r, MPLS_ENTRY_PARITY_STATUS_NACK_1r },
    { PARITY_ENf, L2_ENTRY_PAR_ERRf, NULL,
      L2Xm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      L2_ENTRY_PARITY_CONTROLr,
      L2_ENTRY_PARITY_STATUS_0r, L2_ENTRY_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L2MC_PAR_ERRf, NULL,
      L2MCm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L2MC_PARITY_CONTROLr,
      L2MC_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_ENTRY_PAR_ERRf, NULL,
      L3_ENTRY_ONLYm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      L3_ENTRY_PARITY_CONTROLr,
      L3_ENTRY_PARITY_STATUS_0r, L3_ENTRY_PARITY_STATUS_1r
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_DEFIP_DATA_PAR_ERRf, NULL,
      L3_DEFIP_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_DEFIP_PARITY_CONTROLr,
      L3_DEFIP_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3MC_PAR_ERRf, NULL,
      L3_IPMCm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3MC_PARITY_CONTROLr,
      L3MC_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, MA_STATE_PAR_ERRf, NULL,
      MA_STATEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MA_STATE_PARITY_CONTROLr,
      MA_STATE_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, RMEP_PAR_ERRf, NULL,
      RMEPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      RMEP_PARITY_CONTROLr,
      RMEP_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, MAID_REDUCTION_PAR_ERRf, NULL,
      MAID_REDUCTIONm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MAID_REDUCTION_PARITY_CONTROLr,
      MAID_REDUCTION_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, MA_INDEX_PAR_ERRf, NULL,
      MA_INDEXm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MA_INDEX_PARITY_CONTROLr,
      MA_INDEX_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, PORT_CBL_TABLE_PAR_ERRf, NULL,
      PORT_CBL_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      PORT_CBL_TABLE_PARITY_CONTROLr,
      PORT_CBL_TABLE_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, INITIAL_NHOP_PAR_ERRf, NULL,
      INITIAL_ING_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      INITIAL_NHOP_PARITY_CONTROLr,
      INITIAL_NHOP_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    /* Start of parity-unrelated OAM faults */

    { ANY_RMEP_TLV_PORT_DOWN_INTRf, ANY_RMEP_TLV_PORT_DOWN_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ANY_RMEP_TLV_PORT_UP_INTRf, ANY_RMEP_TLV_PORT_UP_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ANY_RMEP_TLV_INTERFACE_DOWN_INTRf, ANY_RMEP_TLV_INTERFACE_DOWN_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ANY_RMEP_TLV_INTERFACE_UP_INTRf, ANY_RMEP_TLV_INTERFACE_UP_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { XCON_CCM_DEFECT_INTRf, XCON_CCM_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ERROR_CCM_DEFECT_INTRf, ERROR_CCM_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { SOME_RDI_DEFECT_INTRf, SOME_RDI_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { SOME_RMEP_CCM_DEFECT_INTRf, SOME_RMEP_CCM_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    /* End of parity-unrelated OAM faults */
    { PARITY_ENf, VFI_1_PAR_INTRf, NULL,
      VFI_1m, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFI_1_PARITY_CONTROLr,
      VFI_1_PARITY_STATUS_INTRr, INVALIDr,
      VFI_1_PARITY_STATUS_NACKr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_en_ip2_parity_info[] = {
    { PARITY_ENf, ING_NHOP_PAR_ERRf, NULL,
      ING_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      ING_L3_NEXT_HOP_PARITY_CONTROLr,
      ING_L3_NEXT_HOP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_METER_PAR_ERRf, NULL,
      FP_METER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_METER_PARITY_CONTROLr,
      IFP_METER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_COUNTER_PAR_ERRf, NULL,
      FP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_COUNTER_PARITY_CONTROLr,
      IFP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_POLICY_PAR_ERRf, NULL,
      FP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_POLICY_PARITY_CONTROLr,
      IFP_POLICY_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_STORM_CONTROL_PAR_ERRf, NULL,
      FP_STORM_CONTROL_METERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_STORM_CONTROL_PARITY_CONTROLr,
      IFP_STORM_CONTROL_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_MTU_VALUES_PAR_ERRf, NULL,
      L3_MTU_VALUESm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_MTU_VALUES_PARITY_CONTROLr,
      L3_MTU_VALUES_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_MASK_PAR_ERRf, NULL,
      EGR_MASKm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_MASK_PARITY_CONTROLr,
      EGR_MASK_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MODPORT_MAP_SW_PAR_ERRf, NULL,
      MODPORT_MAP_SWm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_SW_PARITY_CONTROLr,
      MODPORT_MAP_SW_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MODPORT_MAP_IM_PAR_ERRf, NULL,
      MODPORT_MAP_IMm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_IM_PARITY_CONTROLr,
      MODPORT_MAP_IM_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MODPORT_MAP_EM_PAR_ERRf, NULL,
      MODPORT_MAP_EMm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_EM_PARITY_CONTROLr,
      MODPORT_MAP_EM_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_en_ep_parity_info[] = {
    { PARITY_ENf, EGR_NHOP_PAR_ERRf, NULL,
      EGR_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_L3_NEXT_HOP_PARITY_CONTROLr,
      EGR_L3_NEXT_HOP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_L3_INTF_PAR_ERRf, NULL,
      EGR_L3_INTFm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_L3_INTF_PARITY_CONTROLr,
      EGR_L3_INTF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_MPLS_LABEL_PAR_ERRf, NULL,
      EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_MPLS_VC_AND_SWAP_LABEL_PARITY_CONTROLr,
      EGR_MPLS_VC_AND_SWAP_LABEL_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_VLAN_PAR_ERRf, NULL,
      EGR_VLANm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_VLAN_PARITY_CONTROLr,
      EGR_VLAN_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
   { PARITY_ENf, EGR_VLAN_STG_PAR_ERRf, NULL,
      EGR_VLAN_STGm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_VLAN_STG_PARITY_CONTROLr,
      EGR_VLAN_STG_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_VXLT_PAR_ERRf, NULL,
      EGR_VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      EGR_VLAN_XLATE_PARITY_CONTROLr,
      EGR_VLAN_XLATE_PARITY_STATUS_0r, EGR_VLAN_XLATE_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_IP_TUNNEL_PAR_ERRf, NULL,
      EGR_IP_TUNNELm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_IP_TUNNEL_PARITY_CONTROLr,
      EGR_IP_TUNNEL_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_PW_COUNT_PAR_ERRf, NULL,
      EGR_PW_INIT_COUNTERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_PW_INIT_COUNTERS_PARITY_CONTROLr,
      EGR_PW_INIT_COUNTERS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
/*    { PARITY_ENf, EGR_IPFIX_SESS_PAR_ERRf, NULL,
      EGR_IPFIX_SESSION_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_IPFIX_SESSION_PARITY_CONTROLr,
      EGR_IPFIX_SESSION_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_IPFIX_EXPORT_PAR_ERRf, NULL,
      EGR_IPFIX_EXPORT_FIFOm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_IPFIX_EXPORT_FIFO_PARITY_CONTROLr,
      EGR_IPFIX_EXPORT_FIFO_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
    { ECC_ENf, EP_INITBUF_DBEf, "EP_INITBUF double-bit ECC error",
      INVALIDm,_SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      EGR_INITBUF_ECC_CONTROLr,
      EGR_INITBUF_ECC_STATUS_DBEr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, EP_INITBUF_SBEf, "EP_INITBUF single-bit ECC error (corrected)",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      EGR_INITBUF_ECC_CONTROLr,
      EGR_INITBUF_ECC_STATUS_SBEr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_PERQ_COUNTERf, "EP_PERQ_COUNTER table parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_PERQ_COUNTER_PARITY_CONTROLr,
      EGR_PERQ_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_FP_COUNTERf, NULL,
      EFP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_FP_COUNTER_PARITY_CONTROLr,
      EGR_FP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_GP0_DBUFf, "EP_GP0_DATABUF memory parity error",
      EFP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_GP0_DBUF_PARITY_CONTROLr,
      EGR_GP0_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_GP1_DBUFf, "EP_GP1_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_GP1_DBUF_PARITY_CONTROLr,
      EGR_GP1_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_GP2_DBUFf, "EP_GP2_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_GP2_DBUF_PARITY_CONTROLr,
      EGR_GP2_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_XQ0_DBUFf, "EP_XQ0_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_XQ0_DBUF_PARITY_CONTROLr,
      EGR_XQ0_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_XQ1_DBUFf, "EP_XQ1_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_XQ1_DBUF_PARITY_CONTROLr,
      EGR_XQ1_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_XQ2_DBUFf, "EP_XQ2_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_XQ2_DBUF_PARITY_CONTROLr,
      EGR_XQ2_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_XQ3_DBUFf, "EP_XQ3_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_XQ3_DBUF_PARITY_CONTROLr,
      EGR_XQ3_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_LP_DBUFf, "EP_LP_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_LP_DBUF_PARITY_CONTROLr,
      EGR_LP_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_CM_DBUFf, "EGR_CM_DBUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_CM_DBUF_PARITY_CONTROLr,
      EGR_CM_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

/*
 * Is DEQ_CBPERRPTRr for MMU_CBPDATA0 - MMU_CBPDATA31 (15-bit address)?
 * How does MMU_CELLCHKm (15-bit address) report parity error?
 */
STATIC _soc_parity_info_t _soc_en_mmu_parity_info[] = {
    { AGING_CTR_PAR_ERR_ENf, AGING_CTR_PAR_ERRf, NULL,
      MMU_AGING_CTRm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { AGING_EXP_PAR_ERR_ENf, AGING_EXP_PAR_ERRf, NULL,
      MMU_AGING_EXPm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { CCP_PAR_ERR_ENf, CCP_PAR_ERRf, NULL,
      MMU_CCP_MEMm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      CCPPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { CFAP_PAR_ERR_ENf, CFAP_PAR_ERRf, NULL,
      MMU_CFAP_MEMm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      CFAPPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { CFAP_MEM_FAIL_ENf, CFAP_MEM_FAILf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ_PKTHDR_CPU_ERR_ENf, DEQ_PKTHDR_CPU_ERRf, NULL,
      MMU_CBPPKTHEADERCPUm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      DEQ_PKTHDRCPUERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ_PKTHDR0_ERR_ENf, DEQ_PKTHDR0_ERRf, NULL,
      MMU_CBPPKTHEADER0_MEM0m, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      DEQ_PKTHDR0ERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ0_NOT_IP_ERR_ENf, DEQ0_NOT_IP_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ0_CELLCRC_ERR_ENf, DEQ0_CELLCRC_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { START_BY_START_ERR_ENf, START_BY_START_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { MEM1_IPMC_TBL_PAR_ERR_ENf, MEM1_IPMC_TBL_PAR_ERRf, "MMU_IPMC_GROUP_TBL",
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMUIPMC,
      INVALIDr,
      MEM1_IPMCGRP_TBL_PARITYERRORPTRr, MEM1_IPMCGRP_TBL_PARITYERROR_STATUSr,
      INVALIDr, INVALIDr },
    { MEM1_VLAN_TBL_PAR_ERR_ENf, MEM1_VLAN_TBL_PAR_ERRf, NULL,
      MMU_IPMC_VLAN_TBLm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      MEM1_IPMCVLAN_TBL_PARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { TOQ0_CELLHDR_PAR_ERR_ENf, TOQ0_CELLHDR_PAR_ERRf, NULL,
      MMU_CBPCELLHEADERm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      TOQ_CELLHDRERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { TOQ0_PKTHDR1_PAR_ERR_ENf, TOQ0_PKTHDR1_PAR_ERRf, NULL,
      MMU_CBPPKTHEADER1_MEM0m, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      TOQ_PKTHDR1ERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { TOQ0_PKTLINK_PAR_ERR_ENf, TOQ0_PKTLINK_PAR_ERRf, NULL,
      MMU_PKTLINK0m, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      TOQ_PKTLINKERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { TOQ0_CELLLINK_PAR_ERR_ENf, TOQ0_CELLLINK_PAR_ERRf, NULL,
      MMU_CELLLINKm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      TOQ_CELLLINKERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    /* what memory is this for? */
    { TOQ0_IPMC_TBL_PAR_ERR_ENf, TOQ0_IPMC_TBL_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      TOQ_IPMCGRPERRPTR0r, INVALIDr,
      INVALIDr, INVALIDr },
    /* what memory is this for? */
    { TOQ0_VLAN_TBL_PAR_ERR_ENf, TOQ0_VLAN_TBL_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      TOQ_IPMCVLANERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    /* what is this memory ? use EGRSHAPEPARITYERRORPTRr for status? */
    { MTRO_PAR_ERR_ENf, MTRO_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { PFAP_PAR_ERR_ENf, PFAP_PAR_ERRf, NULL,
      MMU_PFAP_MEMm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      PFAPPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { PFAP_MEM_FAIL_ENf, PFAP_MEM_FAILf, NULL,
      MMU_PFAP_MEMm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { WRED_PAR_ERR_ENf, WRED_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMUWRED,
      INVALIDr,
      WRED_PARITY_ERROR_INFOr, WRED_PARITY_ERROR_BITMAPr,
      INVALIDr, INVALIDr },
    { DEQ_PKTHDR2_ERR_ENf, DEQ_PKTHDR2_PAR_ERRf, NULL,
      MMU_CBPPKTHEADER2m, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      DEQ_PKTHDR2ERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    /* what memory is this for? use E2EFC_PARITYERRORPTRr.PTRf for index */
    { E2EFC_ERR_ENf, E2EFC_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_en_xqp_parity_info[] = {
    { EHG_TX_DATAf, EHG_TX_DATAf, NULL,
      XQPORT_EHG_TX_TUNNEL_DATAm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XQPORT_PARITY_CONTROLr,
      XQPORT_EHG_TX_DATA_PARITY_STATUS_INTRr, XQPORT_FORCE_SINGLE_BIT_ERRORr,
      XQPORT_EHG_TX_DATA_PARITY_STATUS_NACKr, XQPORT_FORCE_DOUBLE_BIT_ERRORr },
    
    { XQBOD_RXFIFOf, XQBOD_RXFIFOf, "XQBOD_RXFIFO",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XQPORT_PARITY_CONTROLr,
      XQPORT_XQBOD_RXFIFO_PARITY_STATUS_INTRr, XQPORT_FORCE_SINGLE_BIT_ERRORr,
      INVALIDr, XQPORT_FORCE_DOUBLE_BIT_ERRORr },
    { XQBODE_TXFIFOf, XQBODE_TXFIFOf, "XQBODE_TXFIFO",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XQPORT_PARITY_CONTROLr,
      XQPORT_XQBODE_TXFIFO_PARITY_STATUS_INTRr, XQPORT_FORCE_SINGLE_BIT_ERRORr,
      INVALIDr, XQPORT_FORCE_DOUBLE_BIT_ERRORr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

static struct {
    uint32             cpi_bit;
    soc_block_t        blocktype;
    soc_reg_t          enable_reg;
    soc_reg_t          status_reg;
    _soc_parity_info_t *info;
} _soc_en_parity_group_info[] = {
    { 0x00001, SOC_BLK_MMU, MEM_FAIL_INT_ENr, MEM_FAIL_INT_STATr, _soc_en_mmu_parity_info },
    { 0x00002, SOC_BLK_EPIPE, EP_INTR_ENABLEr, EP_INTR_STATUSr, _soc_en_ep_parity_info },
    { 0x00004, SOC_BLK_IPIPE, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, _soc_en_ip0_parity_info },
    { 0x00008, SOC_BLK_IPIPE, IP1_INTR_ENABLEr, IP1_INTR_STATUSr, _soc_en_ip1_parity_info },
    { 0x00010, SOC_BLK_IPIPE, IP2_INTR_ENABLEr, IP2_INTR_STATUSr, _soc_en_ip2_parity_info },
 /* { 0x00020, PCIE_REPLAY_PERR, }, */
    { 0x00040, SOC_BLK_XQPORT, XQPORT_INTR_ENABLEr, XQPORT_INTR_STATUSr, _soc_en_xqp_parity_info }, /* x4 */
    { 0 }, /* table terminator */
};

STATIC soc_enduro_oam_handler_t en_oam_handler = NULL;

STATIC int
_soc_enduro_parity_block_port(int unit, soc_block_t block, soc_port_t *port)
{
    *port = SOC_BLOCK_PORT(unit, block);
    if ((*port != REG_PORT_ANY) && !SOC_PORT_VALID(unit, *port)) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_enduro_parity_enable(int unit, int enable)
{
    int group, table;
    uint32 addr, group_enable, regval, misc_cfg, cmic_enable = 0, cpi_blk_bit;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_field_t enable_field;
    soc_block_t blk;

    /* loop through each group */
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;
        group_reg = _soc_en_parity_group_info[group].enable_reg;
        group_enable = 0;

        cpi_blk_bit = _soc_en_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
                cpi_blk_bit <<= 1;
                continue;
            }
            /* loop through each table in the group */
            for (table = 0; info[table].enable_field != INVALIDf; table++) {
                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                case _SOC_PARITY_INFO_TYPE_MMU_PARITY:
                case _SOC_PARITY_INFO_TYPE_MMUIPMC:
                case _SOC_PARITY_INFO_TYPE_MMUWRED:
                case _SOC_PARITY_INFO_TYPE_OAM:
                    enable_field = info[table].enable_field;
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                case _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER:
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    reg = info[table].control_reg;
                    if (!SOC_REG_IS_VALID(unit, reg)) {
                        continue;
                    }
                    addr = soc_reg_addr(unit, reg, block_port, 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &regval));
                    soc_reg_field_set(unit, reg, &regval,
                                      info[table].enable_field,
                                      enable ? 1 : 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, regval));
                    enable_field = info[table].error_field;
                    break;
                default:
                    enable_field = INVALIDf;
                    break;
                } /* handle different parity error reporting style */
                if (enable_field != INVALIDf) {
                    soc_reg_field_set(unit, group_reg, &group_enable,
                                      enable_field, enable ? 1 : 0);
                }
            } /* loop through each table in the group */

            if (!SOC_REG_IS_VALID(unit, group_reg)) {
                cpi_blk_bit <<= 1;
                continue;
            }
            addr = soc_reg_addr(unit, group_reg, block_port, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, group_enable));
            cmic_enable |= cpi_blk_bit;
            cpi_blk_bit <<= 1;
        }
    } /* loop through each group */

    SOC_IF_ERROR_RETURN(WRITE_CMIC_CHIP_PARITY_INTR_ENABLEr(unit,
                                                            cmic_enable));

    /* MMU enables */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));

    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHK_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_GEN_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_STAT_CLEARf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));

    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &regval));
    soc_intr_enable(unit, IRQ_MEM_FAIL);

    return SOC_E_NONE;
}

int
_soc_enduro_mem_parity_control(int unit, soc_mem_t mem,
                                 int copyno, int enable)
{
    int group, table;
    uint32 cpi_blk_bit, misc_cfg;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_field_t enable_field;
    soc_block_t blk;

    if (!soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        /* Parity checking is not enabled, nothing to do */
        return SOC_E_NONE;
    }

    /* Convert component/aggregate memories to the table for which
     * the parity registers correspond. */
    switch(mem) {
    case VLAN_SUBNETm:
        mem = VLAN_SUBNET_DATA_ONLYm; /* Should be VLAN_SUBNET? */
        break;
    case L2_ENTRY_ONLYm:
        mem = L2Xm;
        break;
    case L2_USER_ENTRY_ONLYm:
    case L2_USER_ENTRY_DATA_ONLYm:
        mem = L2_USER_ENTRYm;
        break;
    case L3_DEFIPm:
        mem = L3_DEFIP_DATA_ONLYm;
        break;
    case L3_DEFIP_128m:
        mem = L3_DEFIP_128_DATA_ONLYm;
        break;
    case EGR_IP_TUNNEL_IPV6m:
    case EGR_IP_TUNNEL_MPLSm:
        mem = EGR_IP_TUNNELm;
        break;
    default:
        /* Do nothing, keep memory as provided */
        break;
    }


    /* loop through each group */
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;
        group_reg = _soc_en_parity_group_info[group].enable_reg;
        cpi_blk_bit = _soc_en_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
                cpi_blk_bit <<= 1;
                continue;
            }
            if ((copyno != MEM_BLOCK_ANY) && (blk != copyno)) {
                cpi_blk_bit <<= 1;
                continue;
            }

            /* loop through each table in the group */
            for (table = 0; info[table].enable_field != INVALIDf; table++) {
                if (mem != info[table].mem) {
                    continue;
                }

                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                case _SOC_PARITY_INFO_TYPE_MMU_PARITY:
                case _SOC_PARITY_INFO_TYPE_MMUIPMC:
                case _SOC_PARITY_INFO_TYPE_MMUWRED:
                    enable_field = info[table].enable_field;
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, group_reg, block_port,
                                                enable_field,
                                                enable ? 1 : 0));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                case _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER:
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    reg = info[table].control_reg;
                    if (!SOC_REG_IS_VALID(unit, reg)) {
                        return SOC_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, reg, block_port,
                                                info[table].enable_field,
                                                enable ? 1 : 0));
                    break;
                default:
                    enable_field = INVALIDf;
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */
            cpi_blk_bit <<= 1;
        }
    } /* loop through each group */

    /* MMU controls */
    if (enable) {
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg,
                          PARITY_STAT_CLEARf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));

        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHK_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_GEN_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg,
                          PARITY_STAT_CLEARf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, METERING_CLK_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));
    } else {
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHK_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_GEN_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, METERING_CLK_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_single_parity_error(int unit, int group,
                                          soc_port_t block_port, int table,
                                          int schan, char *msg)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 addr, reg_val;
    int index, multiple, error;

    info = _soc_en_parity_group_info[group].info;

    status_reg = schan ? info[table].nack_status0_reg :
        info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    addr = soc_reg_addr(unit, status_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_IDXf);
    multiple = soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
    error = soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERRf);
 
    if (error) {
        soc_cm_debug(DK_ERR,
                     "unit %d %s entry %d parity error\n",
                     unit, msg, index);
        if (multiple) {
            soc_cm_debug(DK_ERR,
                         "unit %d %s has multiple parity errors\n",
                         unit, msg);
        }
    } else {
        soc_cm_debug(DK_ERR,
                     "unit %d %s: parity hardware inconsistency\n",
                     unit, msg);
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_single_ecc_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       int schan, char *msg)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 addr, reg_val;
    int index, double_bit, multiple, error;

    info = _soc_en_parity_group_info[group].info;

    status_reg = schan ? info[table].nack_status0_reg :
        info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    addr = soc_reg_addr(unit, status_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_IDXf);
    double_bit = soc_reg_field_get(unit, status_reg,
                                   reg_val, DOUBLE_BIT_ERRf);
    multiple = soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
    error = soc_reg_field_get(unit, status_reg, reg_val, ECC_ERRf);
 
    if (error) {
        soc_cm_debug(DK_ERR,
                     "unit %d %s entry %d %s ECC error\n",
                     unit, msg, index, double_bit ? "double-bit" : "");
        if (multiple) {
            soc_cm_debug(DK_ERR,
                         "unit %d %s has multiple ECC errors\n",
                         unit, msg);
        }
    } else {
        soc_cm_debug(DK_ERR,
                     "unit %d %s: parity hardware inconsistency\n",
                     unit, msg);
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_dual_parity_error(int unit, int group,
                                        soc_port_t block_port, int table,
                                        int schan, char *msg)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 addr, reg_val, bitmap;
    int index, bucket_index, multiple, ix, bits, size;

    info = _soc_en_parity_group_info[group].info;

    for (ix = 0; ix < 2; ix ++) {
        if (ix == 1) {
            status_reg = schan ? info[table].nack_status1_reg :
                info[table].intr_status1_reg;
        } else {
            status_reg = schan ? info[table].nack_status0_reg :
                info[table].intr_status0_reg;
        }

        if (status_reg == INVALIDr) {
            continue;
        }

        addr = soc_reg_addr(unit, status_reg, block_port, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
        bucket_index =
            soc_reg_field_get(unit, status_reg, reg_val, BUCKET_IDXf);
        multiple =
            soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
        bitmap =
            soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BMf);
        size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BMf);

        if (bitmap != 0) {
            for (bits = 0; bits < size; bits++) {
                if (bitmap & (1 << bits)) {
                    index =
                        bucket_index * size * 2 + bits + (ix * size);
                    soc_cm_debug(DK_ERR,
                                 "unit %d %s entry %d parity error\n",
                                 unit, msg, index);
                }
            }
        }

        if (multiple) {
            soc_cm_debug(DK_ERR,
                         "unit %d %s has multiple parity errors\n",
                         unit, msg);
        }

        /* Clear parity status */
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_mmu_parity_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg, index_reg;
    uint32 addr, reg_val;
    int index;

    info = _soc_en_parity_group_info[group].info;

    status_reg = info[table].intr_status1_reg;
    index_reg = info[table].intr_status0_reg;
    if (index_reg == INVALIDr) {
        soc_cm_debug(DK_ERR, "unit %d %s parity error\n",
                     unit, msg);
        return SOC_E_NONE;
    }
    if (status_reg != INVALIDr) {
        addr = soc_reg_addr(unit, status_reg, block_port, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
        if (reg_val == 0) {
            soc_cm_debug(DK_ERR,
                         "unit %d %s: parity hardware inconsistency\n",
                         unit, msg);
            return SOC_E_NONE;
        }
    }
    addr = soc_reg_addr(unit, index_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
    index = reg_val;
 
    soc_cm_debug(DK_ERR,
                 "unit %d %s entry %d parity error\n",
                 unit, msg, index);
    /* update soc stats for some cases */
    if (info[table].error_field == CCP_PAR_ERRf) {
        SOC_CONTROL(unit)->stat.err_cpcrc++;
    } else if (info[table].error_field == CFAP_PAR_ERRf) {
        SOC_CONTROL(unit)->stat.err_cfap++;
    } 

    return SOC_E_NONE;
}


STATIC int
_soc_enduro_process_parity_error(int unit)
{
    int group, table, index, size;
    uint32 addr, cmic_status, group_status, status,
        group_enable, cpi_blk_bit;
    uint32 entry_idx;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_block_t blk;
    char *msg;
    soc_enduro_oam_handler_t oam_handler_snapshot;

    SOC_IF_ERROR_RETURN(READ_CMIC_CHIP_PARITY_INTR_STATUSr(unit,
                                                           &cmic_status));

    /* loop through each group */
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;
        group_reg = _soc_en_parity_group_info[group].status_reg;
        soc_cm_debug(DK_ERR, "unit %d %s parity processing\n",
                     unit, SOC_REG_NAME(unit, group_reg));
        cpi_blk_bit = _soc_en_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
                cpi_blk_bit <<= 1;
                continue;
            }
            if (!(cmic_status & cpi_blk_bit)) {
                cpi_blk_bit <<= 1;
                continue;
            }

            addr = soc_reg_addr(unit, group_reg, block_port, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &group_status));

            /* loop through each table in the group */
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                if (!soc_reg_field_valid(unit, group_reg,
                                         info[table].error_field)) {
                    soc_cm_debug(DK_ERR, "unit %d %s has bad error field\n",
                                 unit, SOC_MEM_NAME(unit, info[table].mem));
                    continue;
                }

                if (!soc_reg_field_get(unit, group_reg, group_status,
                                       info[table].error_field)) {
                    continue;
                }
                if (info[table].msg) {
                    msg = info[table].msg;
                } else if (info[table].mem != INVALIDm) {
                    msg = SOC_MEM_NAME(unit, info[table].mem);
                } else {
                    msg = SOC_FIELD_NAME(unit, info[table].error_field);
                }

                soc_cm_debug(DK_ERR, "unit %d %s analysis\n",
                             unit, msg);

                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                    soc_cm_debug(DK_ERR, "unit %d %s asserted\n",
                                 unit, msg);
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_single_parity_error(unit,
                              group, block_port, table, FALSE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_single_ecc_error(unit,
                              group, block_port, table, FALSE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_dual_parity_error(unit,
                                       group, block_port, table, FALSE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_mmu_parity_error(unit,
                              group, block_port, table, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMUIPMC:
                    /*
                     * status0 (MEM1_IPMCGRP_TBL_PARITYERRORPTRr) is index
                     * status1 (MEM1_IPMCGRP_TBL_PARITYERROR_STATUSr) is table id
                     *         MMU_IPMC_GROUP_TBL0m - MMU_IPMC_GROUP_TBL8m
                     */
                    reg = info[table].intr_status1_reg;
                    addr = soc_reg_addr(unit, reg, block_port, 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
                    size = soc_reg_field_length(unit, reg, STATUSf);
                    reg = info[table].intr_status0_reg;
                    for (index = 0; index < size; index++, status >>= 1) {
                        if (!(status & 1)) {
                            continue;
                        }
                        addr = soc_reg_addr(unit, reg, block_port, index);
                        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr,
                                                           &entry_idx));
                        soc_cm_debug(DK_ERR,
                                     "unit %d %s%d entry %d parity error\n",
                                     unit, msg, index, entry_idx);
                    }
                    break;
                case _SOC_PARITY_INFO_TYPE_MMUWRED:
                    /*
                     * status0 (WRED_PARITY_ERROR_INFOr) is index
                     * status1 (WRED_PARITY_ERROR_BITMAPr) is table id
                     */
                    reg = info[table].intr_status0_reg;
                    addr = soc_reg_addr(unit, reg, block_port, 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr,
                                                       &entry_idx));
                    reg = info[table].intr_status1_reg;
                    addr = soc_reg_addr(unit, reg, block_port, 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
                    if (status & 0x000003) {
                        msg = "WRED_CFG_CELL";
                    } else if (status & 0x00000c) {
                        msg = "WRED_THD_0_CELL";
                    } else if (status & 0x000030) {
                        msg = "WRED_THD_1_CELL";
                    } else if (status & 0x0000c0) {
                        msg = "WRED_CFG_PACKET";
                    } else if (status & 0x000300) {
                        msg = "WRED_THD_0_PACKET";
                    } else if (status & 0x000c00) {
                        msg = "WRED_THD_1_PACKET";
                    } else if (status & 0x003000) {
                        msg = "WRED_PORT_CFG_CELL";
                    } else if (status & 0x00c000) {
                        msg = "WRED_PORT_THD_0_CELL";
                    } else if (status & 0x030000) {
                        msg = "WRED_PORT_THD_1_CELL";
                    } else if (status & 0x0c0000) {
                        msg = "WRED_PORT_CFG_PACKET";
                    } else if (status & 0x300000) {
                        msg = "WRED_PORT_THD_0_PACKET";
                    } else if (status & 0xc00000) {
                        msg = "WRED_PORT_THD_1_PACKET";
                    } else {
                        break;
                    }
                    soc_cm_debug(DK_ERR,
                                 "unit %d %s entry %d parity error\n",
                                 unit, msg, entry_idx);
                    break;
                case _SOC_PARITY_INFO_TYPE_OAM:
                    /* OAM isn't parity-related but shares an interrupt */

                    oam_handler_snapshot = en_oam_handler;

                    if (oam_handler_snapshot != NULL)
                    {
                        SOC_IF_ERROR_RETURN(oam_handler_snapshot(unit,
                                            info[table].error_field));
                    }

                    break;
                default:
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */

            reg = _soc_en_parity_group_info[group].enable_reg;
            addr = soc_reg_addr(unit, reg, block_port, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &group_enable));
            group_enable &= ~group_status;
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, group_enable));
            group_enable |= group_status;
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, group_enable));
        } /* loop through each block for the group */
    } /* loop through each group */

    return SOC_E_NONE;
}

void
soc_enduro_parity_error(void *unit_vp, void *d1, void *d2,
                         void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    _soc_enduro_process_parity_error(unit);
    soc_intr_enable(unit, IRQ_MEM_FAIL);
}

STATIC int
_soc_enduro_mem_nack_error_process(int unit, soc_mem_t mem, int copyno)
{
    int group, table;
    _soc_parity_info_t *info;
    soc_port_t block_port;
    soc_block_t blk;
    char *msg;

    if (mem == INVALIDm) {
        return SOC_E_PARAM;
    }

    /* loop through each group */
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (copyno != blk) {
                continue;
            }
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }
            /* loop through each table in the group */
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                if (info[table].mem != mem) {
                    continue;
                }
                if (info[table].nack_status0_reg == INVALIDr) {
                    return SOC_E_NOT_FOUND;
                }
                if (info[table].msg) {
                    msg = info[table].msg;
                } else { /* mem must be valid to get her */
                    msg = SOC_MEM_NAME(unit, info[table].mem);
                }

                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_single_parity_error(unit,
                              group, block_port, table, TRUE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_single_ecc_error(unit,
                              group, block_port, table, TRUE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_dual_parity_error(unit,
                              group, block_port, table, TRUE, msg));
                    break;
                default:
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */
        } /* loop through each block of the group */
    } /* loop through each group */

    return SOC_E_NONE;
}

STATIC int
_soc_enduro_mem_ecc_force(int unit, soc_port_t block_port,
                            _soc_parity_info_t *info)
{
    soc_reg_t force_sbe_reg, force_dbe_reg;
    soc_field_t force_field;

    force_field = info->error_field;
    if (force_field == RX_FIFO_MEM_ERRf) {
        force_field = RX_FIFO_MEMf;
    } else if (force_field == TX_FIFO_MEM_ERRf) {
        force_field = TX_FIFO_MEMf;
    }

    force_sbe_reg = info->intr_status1_reg;
    force_dbe_reg = info->nack_status1_reg;

    /* Single-bit error force */
    if (SOC_REG_IS_VALID(unit, force_sbe_reg) &&
        soc_reg_field_valid(unit, force_sbe_reg, force_field)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, force_sbe_reg,
                                    block_port, force_field, 1));        
    } else {
        soc_cm_debug(DK_ERR,
                     "unit %d %s.%s not valid\n",
                     unit, SOC_REG_NAME(unit, force_sbe_reg),
                     SOC_FIELD_NAME(unit, force_field));
    }

    sal_usleep(1000); /* Wait for interrupt-mode logic to kick in */

    /* Double-bit error force */
    if (SOC_REG_IS_VALID(unit, force_dbe_reg) &&
        soc_reg_field_valid(unit, force_dbe_reg, force_field)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, force_dbe_reg,
                                    block_port, force_field, 1));        
    } else {
        soc_cm_debug(DK_ERR,
                     "unit %d %s.%s not valid\n",
                     unit, SOC_REG_NAME(unit, force_dbe_reg),
                     SOC_FIELD_NAME(unit, force_field));
    }

    return SOC_E_NONE;
}

int
_soc_enduro_mem_nack_error_test(int unit)
{
    int group, table, index, rv;
    uint32 tmp_entry[SOC_MAX_MEM_WORDS];
    uint32 parity;
    _soc_parity_info_t *info;
    soc_reg_t group_reg;
    soc_mem_t mem;
    int	 index_min, index_max, index_range;
    soc_port_t block_port;
    soc_block_t blk;
    soc_field_t par_ecc_field;

    /* loop through each group */
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;
        group_reg = _soc_en_parity_group_info[group].status_reg;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }

            for (table = 0; info[table].error_field != INVALIDf; table++) {
                mem = info[table].mem;
                if (mem == INVALIDm) {
                    continue;
                }
                if ((info[table].control_reg == INVALIDr) ||
                    !soc_reg_field_valid(unit, info[table].control_reg,
                                         info[table].enable_field)) {
                    soc_cm_debug(DK_ERR,
                                 "unit %d %s has no parity toggle\n",
                                 unit, SOC_MEM_NAME(unit, mem));
                    continue;
                }

                index_min = soc_mem_index_min(unit, mem);
                index_max = soc_mem_index_max(unit, mem);
                index_range = index_max - index_min + 1;
                index = index_min + (index_range / 2);

                if ((rv = soc_mem_write(unit, mem, blk, index,
                                        soc_mem_entry_null(unit, mem))) < 0) {
                    soc_cm_debug(DK_ERR,
                                 "unit %d %s entry %d mem write error\n",
                                 unit, SOC_MEM_NAME(unit, mem), index);
                    continue;
                }
            
                /* Disable parity */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, info[table].control_reg,
                                            block_port, info[table].enable_field, 0));

                if ((rv = soc_mem_read(unit, mem, blk, index, tmp_entry)) < 0) {
                    soc_cm_debug(DK_ERR,
                                 "unit %d %s entry %d mem read error\n",
                                 unit, SOC_MEM_NAME(unit, mem), index);
                    continue;
                }

                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER:
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    par_ecc_field = EVEN_PARITYf;
                    if (mem == L3_ECMP_COUNTm) {
                        par_ecc_field = EVEN_PARITY_0f;
                    }
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    if (block_port != REG_PORT_ANY) {
                        /* Force registers exist */
                        SOC_IF_ERROR_RETURN
                            (_soc_enduro_mem_ecc_force(unit, block_port,
                                                         &(info[table])));
                    }
                    par_ecc_field = ECCf;
                    break;
                default:
                    par_ecc_field = INVALIDf;
                }

                if (!soc_mem_field_valid(unit, mem, par_ecc_field)) {
#if !defined(SOC_NO_NAMES)
                    soc_cm_debug(DK_ERR,
                                 "unit %d %s doesn't contain %s\n",
                                 unit, SOC_MEM_NAME(unit, mem),
                                 soc_fieldnames[par_ecc_field]);
#endif
                    continue;
                }

                parity =
                    soc_mem_field32_get(unit, mem, tmp_entry, par_ecc_field);
                parity ^= 0x1; /* Bad parity */
                soc_mem_field32_set(unit, mem, tmp_entry, par_ecc_field, parity);
            

                if ((rv = soc_mem_write(unit, mem, blk, index, tmp_entry)) < 0) {
                    soc_cm_debug(DK_ERR,
                                 "unit %d %s entry %d mem write error\n",
                                 unit, SOC_MEM_NAME(unit, mem), index);
                    continue;
                }
            
                /* Enable parity */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, info[table].control_reg,
                                            block_port, info[table].enable_field, 1));

                if ((rv = soc_mem_read(unit, mem, blk, index, tmp_entry)) < 0) {
                    if (rv == SOC_E_FAIL) {
                        soc_cm_debug(DK_ERR,
                                     "unit %d NACK received for %s entry %d:\n\t",
                                     unit, SOC_MEM_NAME(unit, mem), index);
                        if ((rv = _soc_enduro_mem_nack_error_process(unit,
                                                                       mem, blk)) < 0) {
                            soc_cm_debug(DK_ERR,
                                         "unit %d %s entry %d SCHAN NACK analysis failure\n",
                                         unit, SOC_MEM_NAME(unit, mem), index);
                        }
                    } else {
                        soc_cm_debug(DK_ERR,
                                     "unit %d %s entry %d mem read error\n",
                                     unit, SOC_MEM_NAME(unit, mem), index);
                    }
                } else {
                    soc_cm_debug(DK_ERR,
                                 "unit %d %s entry %d mem parity induction failed\n",
                                 unit, SOC_MEM_NAME(unit, mem), index);
                }
                sal_usleep(1000);
            }
        }
    }

    return SOC_E_NONE;
}

static int
soc_enduro_pipe_mem_clear(int unit)
{
    uint32              rval;
    int                 pipe_init_usec;
    soc_timeout_t       to;

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries in largest IPIPE table, L2_ENTRYm */
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 0x8000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table, EGR_L3_NEXT_HOPm */
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x4000);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            soc_cm_debug(DK_WARN, "unit %d : ING_HW_RESET timeout\n", unit);
            break;
        }
    } while (TRUE);

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            soc_cm_debug(DK_WARN, "unit %d : EGR_HW_RESET timeout\n", unit);
            break;
        }
    } while (TRUE);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* Need to clear ING_PW_TERM_COUNTERS since it's not handled by reset control */
    if (!(SAL_BOOT_QUICKTURN) && SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, ING_PW_TERM_COUNTERSm, COPYNO_ALL, TRUE));
    }
    return SOC_E_NONE;
}

uint32 tdm[84] = {2,10,18,26,27,28,29,
                  3,11,19,26,27,28,29,
                  4,12,20,26,27,28,29,
                  5,13,21,26,27,28,29,
                  6,14,22,26,27,28,29,
                  7,15,23,26,27,28,29,
                  8,16,24,26,27,28,29,
                  9,17,25,26,27,28,29,
                  1,30,30,26,27,28,29,
                  1,30,30,26,27,28,29,
                  1,30,30,26,27,28,29,
                  0,30,30,26,27,28,29};

int
soc_enduro_misc_init(int unit)
{
#define NUM_XQPORT 4
    uint32              rval; 
    uint32              prev_reg_addr;
    uint64              reg64;
    int                 port, i;
    uint32              mode[NUM_XQPORT];
    uint32              *arr;
    int                 tdm_size;
    iarb_tdm_table_entry_t iarb_tdm;
    arb_tdm_table_entry_t arb_tdm;
    uint64              multipass;
    uint16              dev_id;
    uint8               rev_id;

    mode[0] = mode[1] = mode[2] = mode[3] = 1; /* gport-mode */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    switch (dev_id) {
      case BCM56132_DEVICE_ID:
      case BCM56332_DEVICE_ID:
      case BCM56338_DEVICE_ID:
          mode[0] = mode[1] = 2; /* xport-mode */
          soc_port_cmap_set(unit, 26, SOC_CTR_TYPE_XE);
          soc_port_cmap_set(unit, 27, SOC_CTR_TYPE_XE);
          break;
      case BCM56334_DEVICE_ID:
          mode[0] = mode[1] = mode[2] = mode[3] = 2; /* xport-mode */
          soc_port_cmap_set(unit, 26, SOC_CTR_TYPE_XE);
          soc_port_cmap_set(unit, 27, SOC_CTR_TYPE_XE);
          soc_port_cmap_set(unit, 28, SOC_CTR_TYPE_XE);
          soc_port_cmap_set(unit, 29, SOC_CTR_TYPE_XE);
          break;
      default:
        break;
    }
    
    /* set the mode for XQPORT blocks */
    if (SOC_PORT_VALID(unit, 26)) {
        SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, 26, &rval));
        soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, XQPORT_MODE_BITSf, mode[0]);
        SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, 26, rval));
    }
    if (SOC_PORT_VALID(unit, 27)) {
        SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, 27, &rval));
        soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, XQPORT_MODE_BITSf, mode[1]);
        SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, 27, rval));
    }
    if (SOC_PORT_VALID(unit, 28)) {
        SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, 28, &rval));
        soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, XQPORT_MODE_BITSf, mode[2]);
        SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, 28, rval));
    }
    if (SOC_PORT_VALID(unit, 29)) {
        SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, 29, &rval));
        soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, XQPORT_MODE_BITSf, mode[3]);
        SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, 29, rval));
    }

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    /* Program for Different TDM mode */
    tdm_size = 84;
    arr = tdm;
    for (i = 0; i < tdm_size; i++) {
        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        sal_memset(&arb_tdm, 0, sizeof(arb_tdm_table_entry_t));
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf, 
                                        arr[i]);
        soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf, 
                                       arr[i]); 
        if (i == tdm_size - 1) {
            soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, WRAP_ENf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                  &iarb_tdm));
        SOC_IF_ERROR_RETURN(WRITE_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                 &arb_tdm));
    }
    rval = 0;
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, 
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    if (!SOC_IS_RELOADING(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_enduro_pipe_mem_clear(unit));
    }

    /* Turn on ingress/egress/mmu parity */
    if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        _soc_enduro_parity_enable(unit, TRUE);
        if (soc_property_get(unit, "parity_test", FALSE)) {
            _soc_enduro_mem_nack_error_test(unit);
        }
    }

    /*
     * Egress Enable
     */
    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, EGR_ENABLEr, &reg64, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, reg64));
    }

    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, EPC_LINK_BMAP_64r, &reg64, PORT_BITMAP_LOf,
                          SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAP_64r(unit, reg64));

    /* GMAC init should be moved to mac */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    prev_reg_addr = 0xffffffff;
    PBMP_GE_ITER(unit, port) {
        uint32  reg_addr;
        if (IS_XQ_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }
    prev_reg_addr = 0xffffffff;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_GE_ITER(unit, port) {
        uint32  reg_addr;
        if (IS_XQ_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }

    /* XMAC init should be moved to mac */
    if (SOC_PBMP_NOT_NULL(PBMP_XE_ALL(unit)) ||
        SOC_PBMP_NOT_NULL(PBMP_HG_ALL(unit))) {
        rval = 0;
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, XPORT_ENf, 1);
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
        PBMP_HG_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
        }
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 0);
        PBMP_XE_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
        }
    }

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L2DST_HIT_ENABLEf, 0);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_VALIDATION_ENf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));


    /* Backwards compatible mirroring by default */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, 
                      DRACO_1_5_MIRRORING_MODE_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          DRACO1_5_MIRRORf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));

    SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIGr, &rval, DRACO1_5_MIRRORf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIGr(unit, rval));


    SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, RING_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));

    /*
     * Set reference clock (based on 200MHz core clock)
     * to be 200MHz * (1/40) = 5MHz
     */
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVISORf, 40);
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVIDENDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));

    /* Match the Internal MDC freq with above for External MDC */
    rval = 0;
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf, 40);
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    /*
     * Set reference clock (based on 200MHz core clock)
     * to be 200MHz * (1/8) = 25MHz
     */
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval, DIVISORf,  8);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval, DIVIDENDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_I2Cr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval, DIVISORf,  8);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval, DIVIDENDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_STDMAr(unit, rval));

    rval = 0x01; /* 125KHz I2C sampling rate based on 5Mhz reference clock */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_I2C_STATr(unit, rval));

    /* GMAC init should be moved to mac */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);
    PBMP_GE_ITER(unit, port) {
        if (IS_XQ_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_GE_ITER(unit, port) {
        if (IS_XQ_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }

    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, VT_MISS_UNTAGf, 0);

    /* Enable pri/cfi remarking on egress ports for BW compatibility. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT)));

    /* 
     * Set the per-chip STORM_CONTROL_METER_MAPPING register for now:
     * 0th meter - BCAST 
     * 1st meter - MCAST (including UNKNOWN/KNOWN and IPMC/L2MC);
     * 2rd meter - DLF.
     * The rate APIs rely on above setting.
     */
    {
        soc_field_t  fields[] = { BCAST_METER_INDEXf, KNOWN_L2MC_METER_INDEXf,
            UNKNOWN_L2MC_METER_INDEXf, KNOWN_IPMC_METER_INDEXf, 
            UNKNOWN_IPMC_METER_INDEXf, DLFBC_METER_INDEXf };
        uint32 values[] =  { 0, 1, 1, 1, 1, 2 };


        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, STORM_CONTROL_METER_MAPPINGr,
                                     REG_PORT_ANY, COUNTOF(values), fields, values));

        /* Enable vrf based l3 lookup by default. */
        SOC_IF_ERROR_RETURN
           (soc_reg_field32_modify(unit, VRF_MASKr, REG_PORT_ANY, MASKf, 0));

        /* Setup SW2_FP_DST_ACTION_CONTROL */
        fields[0] = HGTRUNK_RES_ENf;
        fields[1] = LAG_RES_ENf;
        values[0] = values[1] = 1;
        SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, SW2_FP_DST_ACTION_CONTROLr,
                                                     REG_PORT_ANY, 2, fields, values));
    }

    /* Initialize the multipass loopback bitmap to the loopback port */
    COMPILER_64_ZERO(multipass);
    soc_reg64_field32_set(unit, MULTIPASS_LOOPBACK_BITMAP_64r, &multipass, 
                        BITMAPf, (1 << 1));
    SOC_IF_ERROR_RETURN(WRITE_MULTIPASS_LOOPBACK_BITMAP_64r(unit, multipass));
   
#if 0 /* S/W don't need to program these registers in the initialization, since they are initialized to zeros after reset */
     /* Use same value as CAMBIST screen test */
    rval = 0;
    soc_reg_field_set(unit, FP_CAM_CONTROL_TM_7_THRU_0r, &rval,
                      ALL_TCAMS_TM_7_0f, 0x10);
    SOC_IF_ERROR_RETURN(WRITE_FP_CAM_CONTROL_TM_7_THRU_0r(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL0r, &rval, CAM0_TMf, 0x10);
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL0r, &rval, CAM1_TMf, 0x10);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL0r(unit, rval));
    rval = 0;
#endif

    return SOC_E_NONE;
}

#define TR_MMU_NUM_PG   8
#define TR_MMU_NUM_COS  8

/* Standard Ethernet MTU, 1536 bytes (1 cell = 128 bytes) */
#define TR_MMU_ETH_FRAME_CELLS  12

/* Jumbo Frame MTU, 9216 (1 cell = 128 bytes) */
#define TR_MMU_JUMBO_FRAME_CELLS  72

/* MAX Frame MTU, 16384 (1 cell = 128 bytes) */
#define TR_MMU_MAX_FRAME_CELLS  128

#define TR_MMU_PORT_MIN_CELLS  72
#define TR_MMU_PORT_MIN_PKTS   1

#define TR_MMU_GLOBAL_HDRM_LIMIT_CELLS  636
#define TR_MMU_PG_HDRM_LIMIT_CELLS  36
#define TR_MMU_PG_HDRM_LIMIT_PKTS   36

#define TR_MMU_RESET_OFFSET_CELLS  24
#define TR_MMU_RESET_OFFSET_PKTS   2

#define TR_MMU_MOP_POLICY  7
#define TR_MMU_SOP_POLICY  0

int
soc_enduro_mmu_init(int unit)
{
    uint32              rval;
    int                 port, i, num_ports;
    int                 total_cells, total_pkts;
    int                 pg_hdrm_limit, global_hdrm_limit;
    int                 total_shared_limit, op_buffer_shared_limit;
    int                 port_min, q_min;

    /* Total number of cells */
    total_cells = 16 * 1024; /* 16K cells */

    /* Total number of packet pointers */
    total_pkts = 6 * 1024; /* 6K packet pointers */

    /*
     * Ingress based threshholds
     */

    /* CELL THRESHOLDS */
    /* Per-port minimum */
    rval = 0;
    soc_reg_field_set(unit, PORT_MIN_CELLr, &rval, PORT_MINf, 
                      TR_MMU_PORT_MIN_CELLS);
    port_min = 0;
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr(unit, port, rval));
        port_min += TR_MMU_PORT_MIN_CELLS;
    }

    /* Per-PG minimum. With only one PG in use PORT_MIN should be sufficient */
    rval = 0;
    PBMP_XE_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_PG; i++) {
            SOC_IF_ERROR_RETURN(WRITE_PG_MIN_CELLr(unit, port, i, rval));
        }
    }
    PBMP_HG_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_PG; i++) {
            SOC_IF_ERROR_RETURN(WRITE_PG_MIN_CELLr(unit, port, i, rval));
        }
    }
    PBMP_GE_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_MIN_CELLr(unit, port, 0, rval));
    }

    /* Per-PG headroom space. 
     * For non-GE ports, currently only using one PG.
     */
    rval = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, 
                      PG_HDRM_LIMITf, TR_MMU_PG_HDRM_LIMIT_CELLS);
    soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_GEf, 1);

    pg_hdrm_limit = 0;
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit, port, 0, 0));
        if (IS_CPU_PORT(unit, port) || IS_GE_PORT(unit, port)) {
            continue;
        }
        for (i = 1; i < TR_MMU_NUM_PG; i++) {
            if (i == (TR_MMU_NUM_PG - 1)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PG_HDRM_LIMIT_CELLr(unit, port, i, rval));
                pg_hdrm_limit += TR_MMU_PG_HDRM_LIMIT_CELLS;
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_PG_HDRM_LIMIT_CELLr(unit, port, i, 0));
            }
        }
    }

    /* Per-device limit */
    global_hdrm_limit = TR_MMU_GLOBAL_HDRM_LIMIT_CELLS;
    rval = 0;
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval,
                      GLOBAL_HDRM_LIMITf, global_hdrm_limit);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr(unit, rval));

    /* Use whatever is left over for shared cells */
    total_shared_limit = total_cells;
    total_shared_limit -= port_min;
    total_shared_limit -= pg_hdrm_limit;
    total_shared_limit -= global_hdrm_limit;
    rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMIT_CELLr, &rval,
                      TOTAL_SHARED_LIMITf, total_shared_limit);
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMIT_CELLr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &rval,
                      PORT_SHARED_LIMITf, 7); /* alpha index 7 */
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &rval,
                      PORT_SHARED_DYNAMICf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_CELLr(unit, port, rval));
    }

    /* Max packet size in cells */
    rval = 0;
    soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval,
                      PORT_MAX_PKT_SIZEf, TR_MMU_MAX_FRAME_CELLS);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, port, rval));
    }

    /* Per-PG reset offset.
     * For non-GE ports, currently only using one PG.
     */
    rval = 0;
    soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &rval,
                      PG_RESET_OFFSETf, TR_MMU_RESET_OFFSET_CELLS);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit, port, 0, 0));
        if (IS_CPU_PORT(unit, port) || IS_GE_PORT(unit, port)) {
            continue;
        }
        for (i = 1; i < TR_MMU_NUM_PG; i++) {
            if (i == (TR_MMU_NUM_PG - 1)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PG_RESET_OFFSET_CELLr(unit, port, i, rval));
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_PG_RESET_OFFSET_CELLr(unit, port, i, 0));
            }
        }
    }
    
    /* Currently everything is zero, but keep code for reference */
    rval = 0;
    soc_reg_field_set(unit, PG_RESET_FLOOR_CELLr, &rval,
                      PG_RESET_FLOORf, 0); 
    PBMP_XE_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_PG; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PG_RESET_FLOOR_CELLr(unit, port, i, rval));
        }
    }
    PBMP_HG_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_PG; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PG_RESET_FLOOR_CELLr(unit, port, i, rval));
        }
    }

    rval = 0;
    soc_reg_field_set(unit, PORT_SC_MIN_CELLr, &rval, PORT_SC_MINf, 0);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_SC_MIN_CELLr(unit, port, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, PORT_QM_MIN_CELLr, &rval, PORT_QM_MINf, 0);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_QM_MIN_CELLr(unit, port, rval));
    }

    /* PACKET THRESHOLDS */
    /* Per-port minimum */
    port_min = 0;
    rval = 0;
    soc_reg_field_set(unit, PORT_MIN_PACKETr, &rval, PORT_MINf, 
                      TR_MMU_PORT_MIN_PKTS);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_PACKETr(unit, port, rval));
        port_min += TR_MMU_PORT_MIN_PKTS;
    }

    /* Per-PG minimum. With only one PG in use PORT_MIN should be sufficient */
    rval = 0;
    PBMP_XE_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_PG; i++) {
            SOC_IF_ERROR_RETURN(WRITE_PG_MIN_PACKETr(unit, port, i, rval));
        }
    }
    PBMP_HG_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_PG; i++) {
            SOC_IF_ERROR_RETURN(WRITE_PG_MIN_PACKETr(unit, port, i, rval));
        }
    }
    PBMP_GE_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_MIN_PACKETr(unit, port, 0, rval));
    }

    /* Per-PG headroom space. 
     * For non-GE ports, currently only using one PG.
     */
    rval = 0;
    pg_hdrm_limit = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMIT_PACKETr, &rval,
                      PG_HDRM_LIMITf, TR_MMU_PG_HDRM_LIMIT_PKTS);

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_PACKETr(unit, port, 0, 0));
        if (IS_CPU_PORT(unit, port) || IS_GE_PORT(unit, port)) {
            continue;
        }
        for (i = 1; i < TR_MMU_NUM_PG; i++) {
            if (i == (TR_MMU_NUM_PG - 1)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PG_HDRM_LIMIT_PACKETr(unit, port, i, rval));
                pg_hdrm_limit += TR_MMU_PG_HDRM_LIMIT_PKTS;
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_PG_HDRM_LIMIT_PACKETr(unit, port, i, 0));
            }
        }
    }

    /* Use whatever is left over for shared pkts */
    total_shared_limit = total_pkts;
    total_shared_limit -= port_min;
    total_shared_limit -= pg_hdrm_limit;
    rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMIT_PACKETr, &rval,
                      TOTAL_SHARED_LIMITf, total_shared_limit);
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMIT_PACKETr(unit, rval));


    rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &rval,
                      PORT_SHARED_LIMITf, 7); /* alpha index 7 */
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &rval,
                      PORT_SHARED_DYNAMICf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_PACKETr(unit, port, rval));
    }

    /* Per-PG reset offset.
     * For non-GE ports, currently only using one PG.
     */
    rval = 0;
    soc_reg_field_set(unit, PG_RESET_OFFSET_PACKETr, &rval,
                      PG_RESET_OFFSETf, TR_MMU_RESET_OFFSET_PKTS);
    
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_PACKETr(unit, port, 0, 0));
        if (IS_CPU_PORT(unit, port) || IS_GE_PORT(unit, port)) {
            continue;
        }
        for (i = 1; i < TR_MMU_NUM_PG; i++) {
            if (i == (TR_MMU_NUM_PG - 1)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PG_RESET_OFFSET_PACKETr(unit, port, i, rval));
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_PG_RESET_OFFSET_PACKETr(unit, port, i, 0));
            }
        }
    }

    rval = 0;
    soc_reg_field_set(unit, PORT_SC_MIN_PACKETr, &rval, PORT_SC_MINf, 0);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_SC_MIN_PACKETr(unit, port, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, PORT_QM_MIN_PACKETr, &rval, PORT_QM_MINf, 0);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_QM_MIN_PACKETr(unit, port, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval, PG0_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval, PG1_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval, PG2_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval, PG3_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval, PG4_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval, PG5_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval, PG6_THRESH_SELf, 0x8);
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_THRESH_SELr(unit, port, rval));
    }
    
    rval = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI0_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI1_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI2_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI3_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI4_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI5_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI6_GRPf, TR_MMU_NUM_PG - 1);
    PBMP_XE_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, rval));
    }
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, rval));
    }
    rval = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI7_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI8_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI9_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI10_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI11_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI12_GRPf, TR_MMU_NUM_PG - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI13_GRPf, TR_MMU_NUM_PG - 1);
    PBMP_XE_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, rval));
    }
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, PORT_PAUSE_ENABLEr, &rval, 
                          PORT_PAUSE_ENABLEf,
                          SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_PORT_PAUSE_ENABLEr(unit, rval));

    /* CELL THRESHOLDS */
    q_min = TR_MMU_ETH_FRAME_CELLS;
    num_ports = NUM_ALL_PORT(unit);
    op_buffer_shared_limit = total_cells;
    op_buffer_shared_limit -= (q_min * num_ports * TR_MMU_NUM_COS);

    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLr, &rval, 
                      OP_BUFFER_SHARED_LIMIT_CELLf, op_buffer_shared_limit);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLr(unit, rval));

    /* Same for YELLOW/RED limits */
    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLr, &rval, 
                      OP_BUFFER_LIMIT_YELLOW_CELLf, 
                      (op_buffer_shared_limit >> 3));
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLr(unit, rval));

    /*
     * OP_SHARED_LIMIT set to 3/4 of op_buffer_shared_limit
     * OP_SHARED_RESET_VALUE set to 1/2 op_buffer_shared_limit
     */
    rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &rval, 
                      OP_SHARED_LIMIT_CELLf, ((op_buffer_shared_limit * 3) / 4));
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &rval, 
                      OP_SHARED_RESET_VALUE_CELLf, (op_buffer_shared_limit / 2));
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &rval, 
                      PORT_LIMIT_ENABLE_CELLf, 1);
    PBMP_ALL_ITER(unit, port) { 
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_CELLr(unit, port, rval));
    }

    /* Same for YELLOW/RED limits */
    rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_YELLOW_CELLr, &rval,
                      OP_PORT_LIMIT_YELLOW_CELLf, 
                      (((op_buffer_shared_limit * 3) / 4) >> 3));
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_YELLOW_CELLr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_RED_CELLr(unit, port, rval));
    }

    /* Program the queue reset offset */
    soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_CELLr, &rval,
                      Q_RESET_OFFSET_CELLf, TR_MMU_RESET_OFFSET_CELLS);
    PBMP_ALL_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_COS; i++) {
            /* COSQ 0..7 */
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_RESET_OFFSET_CELLr(unit, port, i, rval));
        }
    }

    rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                      Q_MIN_CELLf, q_min);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                      Q_LIMIT_ENABLE_CELLf, 0x1);
    /* Use dynamic threshold limits */
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval, 
                      Q_LIMIT_DYNAMIC_CELLf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                      Q_SHARED_LIMIT_CELLf, 6); /* alpha index 6 */

    PBMP_ALL_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_COS; i++) {
            /* COSQ 0..7 */
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIG_CELLr(unit, port, i, rval));
        }
    }

    /* PACKET THRESHOLDS */
    q_min = 1;
    op_buffer_shared_limit = total_pkts;
    op_buffer_shared_limit -= (q_min * num_ports * TR_MMU_NUM_COS);

    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_PACKETr, &rval, 
                      OP_BUFFER_SHARED_LIMIT_PACKETf, op_buffer_shared_limit);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_PACKETr(unit, rval));

    /* Same for YELLOW/RED limits */
    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_PACKETr, &rval, 
                      OP_BUFFER_LIMIT_YELLOW_PACKETf, 
                      (op_buffer_shared_limit >> 3));
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_PACKETr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_PACKETr(unit, rval));

    /*
     * OP_SHARED_LIMIT set to 3/4 op_buffer_shared_limit
     * OP_SHARED_RESET_VALUE set to 1/2 op_buffer_shared_limit
     */
    rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &rval, 
                      OP_SHARED_LIMIT_PACKETf, ((op_buffer_shared_limit * 3) / 4));
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &rval, 
                      OP_SHARED_RESET_VALUE_PACKETf, (op_buffer_shared_limit / 4));
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &rval, 
                      PORT_LIMIT_ENABLE_PACKETf, 1);
    PBMP_ALL_ITER(unit, port) { 
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_PACKETr(unit, port, rval));
    }

    /* Same for YELLOW/RED limits */
    rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_YELLOW_PACKETr, &rval,
                      OP_PORT_LIMIT_YELLOW_PACKETf, 
                      (((op_buffer_shared_limit * 3) / 4) >> 3));
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_YELLOW_PACKETr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_RED_PACKETr(unit, port, rval));
    }

    /* Program the queue reset offset */
    soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_PACKETr, &rval,
                      Q_RESET_OFFSET_PACKETf, TR_MMU_RESET_OFFSET_PKTS);
    PBMP_ALL_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_COS; i++) {
            /* COSQ 0..7 */
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_RESET_OFFSET_PACKETr(unit, port, i, rval));
        }
    }

    rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &rval,
                      Q_MIN_PACKETf, q_min);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &rval,
                      Q_LIMIT_ENABLE_PACKETf, 0x1);
    /* Use dynamic threshold limits */
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &rval, 
                      Q_LIMIT_DYNAMIC_PACKETf, 0x1);
    /* Dynamic threshold limit is alpha of 4 */
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &rval,
                      Q_SHARED_LIMIT_PACKETf, 6); /* alpha index 6 */

    PBMP_ALL_ITER(unit, port) {
        for (i = 0; i < TR_MMU_NUM_COS; i++) {
            /* COSQ 0..7 */
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIG_PACKETr(unit, port, i, rval));
        }
    }

    rval = 0;
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval,
                      MOP_POLICYf, TR_MMU_MOP_POLICY);
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval,
                      SOP_POLICYf, TR_MMU_SOP_POLICY);
    SOC_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval));

    /* No flow control for COS 0-7 */
    rval = 0;
    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                      PORT_PRI_XON_ENABLEf, 0);
    PBMP_XE_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_XON_ENABLEr(unit, port, rval));
    }
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_XON_ENABLEr(unit, port, rval));
    }

    /* Port enable */
    rval=0;
    soc_reg_field_set(unit, INPUT_PORT_RX_ENABLEr, &rval, 
                          INPUT_PORT_RX_ENABLEf,
                          SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLEr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_OUTPUT_PORT_RX_ENABLEr(unit, rval));

    return SOC_E_NONE;
}

int
soc_enduro_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_enduro_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_enduro_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}

void soc_enduro_oam_handler_register(soc_enduro_oam_handler_t handler)
{
    en_oam_handler = handler;
}

int
soc_en_xqport_mode_change(int unit, soc_port_t port, soc_mac_mode_t mode)
{
    int rv = SOC_E_NONE;
    uint32 old_bits, bits, rval, val2, to_usec;
    uint64 rval64;
    soc_field_t xq_rst2 = INVALIDf;
    soc_field_t ep_port_rst = INVALIDf;
    soc_field_t ep_mmu_rst = INVALIDf;
    mac_driver_t *macd;

    if (!SOC_PORT_VALID(unit, port)) {
        return SOC_E_PORT;
    }

    switch (port) {
    case 26:
        ep_port_rst = EGR_XQP0_PORT_INT_RESETf;
        ep_mmu_rst = EGR_XQP0_MMU_INT_RESETf;
        xq_rst2 = XQ0_HOTSWAP_RST_Lf;
        break;
    case 27:
        ep_port_rst = EGR_XQP1_PORT_INT_RESETf;
        ep_mmu_rst = EGR_XQP1_MMU_INT_RESETf;
        xq_rst2 = XQ1_HOTSWAP_RST_Lf;
        break;
    case 28:
        ep_port_rst = EGR_XQP2_PORT_INT_RESETf;
        ep_mmu_rst = EGR_XQP2_MMU_INT_RESETf;
        xq_rst2 = XQ2_HOTSWAP_RST_Lf;
        break;
    case 29:
        ep_port_rst = EGR_XQP3_PORT_INT_RESETf;
        ep_mmu_rst = EGR_XQP3_MMU_INT_RESETf;
        xq_rst2 = XQ3_HOTSWAP_RST_Lf;
        break;
    default:
        return SOC_E_PORT;
    }

    SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
    old_bits = soc_reg_field_get(unit, XQPORT_MODE_REGr, rval,
                                 XQPORT_MODE_BITSf);
    bits = mode == SOC_MAC_MODE_10000 ? 2 : 1;
    if (bits == old_bits) {
        return SOC_E_NONE;
    }

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));

    /* Disable MAC TX and RX */
    SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, port, FALSE));

    /* Block linkscan and sbus access */
    soc_linkscan_pause(unit);
    COUNTER_LOCK(unit);
    /* Egress disable */
    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set(unit, EGR_ENABLEr, &rval64, PRT_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval64));

    /* Hold the XQPORT hotswap reset */
    SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(unit, &val2));
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val2, 
                      xq_rst2, 0);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, val2));
    sal_usleep(to_usec);

    /* Clear the ECRC register */
    SOC_IF_ERROR_RETURN(WRITE_ECRCr(unit, port, 0));

    /* Reset the EP */
    SOC_IF_ERROR_RETURN(READ_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, &val2));
    soc_reg_field_set(unit, EDATABUF_XQP_FLEXPORT_CONFIGr, &val2, 
                      ep_port_rst, 1);
    soc_reg_field_set(unit, EDATABUF_XQP_FLEXPORT_CONFIGr, &val2, 
                      ep_mmu_rst, 1);
    SOC_IF_ERROR_RETURN(WRITE_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, val2));
    sal_usleep(to_usec);

    SOC_IF_ERROR_RETURN(READ_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, &val2));
    soc_reg_field_set(unit, EDATABUF_XQP_FLEXPORT_CONFIGr, &val2, 
                      ep_port_rst, 0);
    soc_reg_field_set(unit, EDATABUF_XQP_FLEXPORT_CONFIGr, &val2, 
                      ep_mmu_rst, 0);
    SOC_IF_ERROR_RETURN(WRITE_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, val2));
    sal_usleep(to_usec);

    /* Bring the XQPORT block out of reset */
    SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(unit, &val2));
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &val2, 
                      xq_rst2, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, val2));
    sal_usleep(to_usec);

    /* Bring the hyperlite out of reset */
    soc_xgxs_reset(unit, port);
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XQPORT_XGXS_NEWCTL_REGr, 
                                               port, TXD1G_FIFO_RSTBf, 0xf));

    /* Change the XQPORT block mode */
    SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
    soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, 
                      XQPORT_MODE_BITSf, bits);
    SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, port, rval));

    /* Egress Enable */
    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set(unit, EGR_ENABLEr, &rval64, PRT_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval64));

    /* Port probe and initialization */
    SOC_IF_ERROR_RETURN(MAC_INIT(macd, unit, port));

    /* Resume sbus access and linkscan */
    COUNTER_UNLOCK(unit);
    soc_linkscan_continue(unit);
    return rv;
}
#endif /* BCM_ENDURO_SUPPORT */
