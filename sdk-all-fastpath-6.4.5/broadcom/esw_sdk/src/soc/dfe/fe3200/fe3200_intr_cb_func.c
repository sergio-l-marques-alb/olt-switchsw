/*
 * $Id: fe3200_intr_cb_func.c, v1 Broadcom SDK $
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
 * Purpose:    Implement CallBacks function for FE3200 interrupts.
 */
 
/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_intr_handler.h>

#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/intr.h>
#include <soc/dfe/cmn/mbcm.h>

#include <soc/dfe/fe3200/fe3200_intr.h>
#include <soc/dfe/fe3200/fe3200_intr_cb_func.h>
#include <soc/dfe/fe3200/fe3200_intr_corr_act_func.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

/*************
 * FUNCTIONS *
 *************/
/*
 * Generic None handles - for CB without specific handling
 */


STATIC
 int
 fe3200_hard_reset(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rc = soc_device_reset(unit, SOC_DCMN_RESET_MODE_BLOCKS_SOFT_RESET, SOC_DCMN_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_none(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_FUNC_RETURN;
 }

STATIC
 int
 fe3200_shutdown_link(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    uint32 port = block_instance*4 + SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field;
    uint32 rc;
    SOCDNX_INIT_FUNC_DEFS;
    

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_enable_set, (unit, port, 0)); 
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }

STATIC
 int
 fe3200_special_handling(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_FUNC_RETURN;
 }

int fe3200_intr_handle_generic_none(int unit, int block_instance, fe3200_interrupt_type en_fe3200_interrupt,char *msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    
    /* print info */
    rc = fe3200_interrupt_print_info(unit, block_instance, en_fe3200_interrupt, 0, FE3200_INT_CORR_ACT_NONE, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe3200_interrupt_handles_corrective_action(unit, block_instance, en_fe3200_interrupt, msg, FE3200_INT_CORR_ACT_NONE, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int fe3200_intr_recurring_action_handle_generic_none(int unit, int block_instance, fe3200_interrupt_type en_fe3200_interrupt, char *msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    
    /* print info */
    rc = fe3200_interrupt_print_info(unit, block_instance, en_fe3200_interrupt, 1, FE3200_INT_CORR_ACT_NONE, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe3200_interrupt_handles_corrective_action(unit, block_instance, en_fe3200_interrupt, msg, FE3200_INT_CORR_ACT_NONE, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

void fe3200_interrupt_cb_init(int unit)
{
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_DCH_P_0_DESCCNTO, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_DCH_P_1_DESCCNTO, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_DCH_P_2_DESCCNTO, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_IFMFO_P_0_INT, 10,1,fe3200_special_handling,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_IFMFO_P_1_INT, 10,1,fe3200_special_handling,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_IFMFO_P_2_INT, 10,1,fe3200_special_handling,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_CPUDATACELLFNE_A_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_CPUDATACELLFNE_B_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_CPUDATACELLFNE_C_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_CPUDATACELLFNE_D_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_UNREACH_DEST_EV_INT, 10000,100,NULL,fe3200_special_handling);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_DCH_UN_EXP_CELL_P_0, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_ALTO_P_0_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_DCH_UN_EXP_ERROR_P_0, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_OUT_OF_SYNC_INT_P_0, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_DCH_UN_EXP_CELL_P_1, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_ALTO_P_1_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_DCH_UN_EXP_ERROR_P_1, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_OUT_OF_SYNC_INT_P_1, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_DCH_UN_EXP_CELL_P_2, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_ALTO_P_2_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_DCH_UN_EXP_ERROR_P_2, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_OUT_OF_SYNC_INT_P_2, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_ECC_ECC_1B_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCH_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_CCP_0_SRC_DV_CNG_LINK_INT, 1000,100,NULL,fe3200_special_handling);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_CCP_1_SRC_DV_CNG_LINK_INT, 1000,100,NULL,fe3200_special_handling);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_CCP_0_CHF_OVF_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_CCP_0_CLF_OVF_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_CCP_1_CHF_OVF_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_CCP_1_CLF_OVF_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_DCL_P_0_TAG_PAR_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_DCL_P_1_TAG_PAR_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_DCL_P_2_TAG_PAR_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_MACA_DATA_CRC_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_MACB_DATA_CRC_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_MACC_DATA_CRC_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_SOV_ERR_P_0_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_SOV_ERR_P_1_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_SOV_ERR_P_2_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_PCP_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_ECC_ECC_1B_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCL_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCM_OUT_OF_SYNC_P_0_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCM_OUT_OF_SYNC_P_1_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCM_OUT_OF_SYNC_P_2_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCM_ECC_ECC_1B_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_DCM_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_0_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_1_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_2_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_0_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_1_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_2_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_0_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_1_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_2_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_0_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_1_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_2_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_CCS_CPU_CAPT_CELL_FNE_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_CCS_UNRCH_DST_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_CCS_GSYNC_DSCRD_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_CCS_CRP_PARITY_ERR_INT, 1,1000000,fe3200_hard_reset,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_CCS_ECC_ECC_1B_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_CCS_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_0_CRH_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_1_CRH_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_2_CRH_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_3_CRH_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_0_CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_1_CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_2_CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_3_CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_0_CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_1_CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_2_CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_CRH_3_CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_GENERAL_LINK_INTEGRITY_CHANGED_INT, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_ECC_ECC_1B_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_RTP_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_0, 1,1000000,fe3200_shutdown_link,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_1, 1,1000000,fe3200_shutdown_link,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_2, 1,1000000,fe3200_shutdown_link,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_3, 1,1000000,fe3200_shutdown_link,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0, 1,1000000,fe3200_shutdown_link,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1, 1,1000000,fe3200_shutdown_link,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2, 1,1000000,fe3200_shutdown_link,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3, 1,1000000,fe3200_shutdown_link,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_2_LOS_INT_0, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_2_LOS_INT_1, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_2_LOS_INT_2, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_2_LOS_INT_3, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_0, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_1, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_2, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_3, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_4_OOF_INT_0, 1,100,fe3200_special_handling,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_4_OOF_INT_1, 1,100,fe3200_special_handling,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_4_OOF_INT_2, 1,100,fe3200_special_handling,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_4_OOF_INT_3, 1,100,fe3200_special_handling,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_4_DEC_ERR_INT_0, 1,1000000,fe3200_special_handling,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_4_DEC_ERR_INT_1, 1,1000000,fe3200_special_handling,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_4_DEC_ERR_INT_2, 1,1000000,fe3200_special_handling,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_4_DEC_ERR_INT_3, 1,1000000,fe3200_special_handling,fe3200_none);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_0, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_1, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_2, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_3, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_0, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_1, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_2, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_3, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_ECC_ECC_1B_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_FMAC_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,fe3200_special_handling,NULL);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_ECC_ECC_1B_ERR_INT, 1000,100,NULL,fe3200_hard_reset);
    dcmn_intr_add_handler(unit, FE3200_INT_FSRD_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL);


}

#undef _ERR_MSG_MODULE_NAME
