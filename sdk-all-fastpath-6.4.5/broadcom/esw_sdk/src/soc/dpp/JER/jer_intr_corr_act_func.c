/*
 * $Id: jer_appl_intr_corr_act_func.c, v1 Broadcom SDK $
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
 * Purpose:    Implement Correction action functions for jericho interrupts.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <sal/core/time.h>
#include <shared/bsl.h>
#include <sal/core/dpc.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <soc/dpp/mbcm.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_api_dram.h> 
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/jer_intr_cb_func.h>
#include <soc/dpp/JER/jer_intr_corr_act_func.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_oam.h>


/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

#define JER_INTERRUPTS_SCH_FLOW_TO_FIP_MAPPING_FFM_SIZE 16384
#define JER_INTERRUPTS_NOF_FLOWS_PER_FFM_ENTRY 8
#define REG_FIRST_BLK_TYPE(regblklist) regblklist[0]

/*************
 * FUNCTIONS *
 *************/

int reg2first_schan(int unit,soc_reg_t reg,uint32 *block)
{
  soc_reg_info_t reg_info = SOC_REG_INFO(unit, reg);
  int block_type = REG_FIRST_BLK_TYPE(reg_info.block);
  int blk=-1;
  int schan;
  SOCDNX_INIT_FUNC_DEFS;
  SOC_BLOCK_ITER(unit, blk, block_type) {
      schan =  SOC_BLOCK_INFO(unit,blk).schan;
      *block = schan;
      SOC_EXIT;
  }

  SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Failed to translate cnt reg to schan ")));

exit:
    SOCDNX_FUNC_RETURN;

}


int
jer_interrupt_data_collection_for_shadowing(
    int unit, 
    int block_instance, 
    jer_interrupt_type en_interrupt, 
    char* special_msg, 
    jer_int_corr_act_type* p_corrective_action,
    jer_interrupt_mem_err_info* shadow_correct_info)
{
    int rc;
    uint64 counter;
    uint32 cntf, cnt_overflowf, addrf, addr_validf;
    soc_reg_t cnt_reg = INVALIDr;
    unsigned numels;
    int index;
    soc_mem_t mem;
    char* memory_name; 
    int cached_flag;
    uint32 block;
    int32 is_par=0, is_ecc_2b=0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(shadow_correct_info);
    *p_corrective_action = JER_INT_CORR_ACT_NONE;

    switch(en_interrupt) {
    case JER_INT_IHB_ECC_ECC_2B_ERR_INT:

                cnt_reg = IHB_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_MTRPS_EM_ECC_ECC_2B_ERR_INT:

                cnt_reg = MTRPS_EM_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IPST_ECC_ECC_2B_ERR_INT:

                cnt_reg = IPST_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_NBIL_ECC_ECC_2B_ERR_INT:

                cnt_reg = NBIL_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_PPDB_A_ECC_ECC_2B_ERR_INT:

                cnt_reg = PPDB_A_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_CRPS_ECC_ECC_2B_ERR_INT:

                cnt_reg = CRPS_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_DRCH_ECC_ECC_2B_ERR_INT:

                cnt_reg = DRCH_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_OCB_ECC_ECC_2B_ERR_INT:

                cnt_reg = OCB_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_OAMP_ECC_ECC_2B_ERR_INT:

                cnt_reg = OAMP_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_DRCD_ECC_ECC_2B_ERR_INT:

                cnt_reg = DRCD_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_FDA_ECC_ECC_2B_ERR_INT:

                cnt_reg = FDA_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IRE_ECC_ECC_2B_ERR_INT:

                cnt_reg = IRE_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_EGQ_ECC_ECC_2B_ERR_INT:

                cnt_reg = EGQ_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_ECI_ECC_ECC_2B_ERR_INT:

                cnt_reg = ECI_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_FMAC_ECC_ECC_2B_ERR_INT:

                cnt_reg = FMAC_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IRR_ECC_ECC_2B_ERR_INT:

                cnt_reg = IRR_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_ILKN_PMH_ECC_ECC_2B_ERR_INT:

                cnt_reg = ILKN_PMH_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IQMT_ECC_ECC_2B_ERR_INT:

                cnt_reg = IQMT_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IDR_MMU_ECC_2B_ERR_INT:
    case JER_INT_IDR_ECC_ECC_2B_ERR_INT:

                cnt_reg = IDR_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_SCH_ECC_ECC_2B_ERR_INT:

                cnt_reg = SCH_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_DRCF_ECC_ECC_2B_ERR_INT:

                cnt_reg = DRCF_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IHP_ECC_ECC_2B_ERR_INT:

                cnt_reg = IHP_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_FDT_ECC_ECC_2B_ERR_INT:

                cnt_reg = FDT_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_RTP_ECC_ECC_2B_ERR_INT:

                cnt_reg = RTP_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_NBIH_ECC_ECC_2B_ERR_INT:

                cnt_reg = NBIH_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_MESH_TOPOLOGY_ECC_ECC_2B_ERR_INT:

                cnt_reg = MESH_TOPOLOGY_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_DRCC_ECC_ECC_2B_ERR_INT:

                cnt_reg = DRCC_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IQM_ECC_ECC_2B_ERR_INT:

                cnt_reg = IQM_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_FDR_ECC_ECC_2B_ERR_INT:
    case JER_INT_FDR_FDR_4_P_1_ECC_2B_ERR_INT:
    case JER_INT_FDR_FDR_4_P_2_ECC_2B_ERR_INT:
    case JER_INT_FDR_FDR_4_P_3_ECC_2B_ERR_INT:

                cnt_reg = FDR_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_DRCG_ECC_ECC_2B_ERR_INT:

                cnt_reg = DRCG_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IPS_ECC_ECC_2B_ERR_INT:

                cnt_reg = IPS_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_EPNI_ECC_ECC_2B_ERR_INT:

                cnt_reg = EPNI_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_MRPS_ECC_ECC_2B_ERR_INT:

                cnt_reg = MRPS_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_ILKN_PML_ECC_ECC_2B_ERR_INT:

                cnt_reg = ILKN_PML_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_FCR_ECC_ECC_2B_ERR_INT:

                cnt_reg = FCR_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_MMU_ECC_ECC_2B_ERR_INT:

                cnt_reg = MMU_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_CFC_ECC_ECC_2B_ERR_INT:

                cnt_reg = CFC_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_OLP_ECC_ECC_2B_ERR_INT:

                cnt_reg = OLP_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_FCT_ECC_ECC_2B_ERR_INT:

                cnt_reg = FCT_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_PPDB_B_ECC_ECC_2B_ERR_INT:

                cnt_reg = PPDB_B_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_FSRD_ECC_ECC_2B_ERR_INT:

                cnt_reg = FSRD_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_DRCA_ECC_ECC_2B_ERR_INT:

                cnt_reg = DRCA_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_EDB_ECC_ECC_2B_ERR_INT:

                cnt_reg = EDB_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IPT_ECC_ECC_2B_ERR_INT:

                cnt_reg = IPT_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_DRCB_ECC_ECC_2B_ERR_INT:

                cnt_reg = DRCB_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_DRCE_ECC_ECC_2B_ERR_INT:

                cnt_reg = DRCE_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_ecc_2b =  TRUE;
                break;
    case JER_INT_IHB_ECC_PARITY_ERR_INT:

                cnt_reg = IHB_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_NBIL_ECC_PARITY_ERR_INT:

                cnt_reg = NBIL_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_DRCH_DRAM_CMD_PARITY_ERR:
    case JER_INT_DRCH_ECC_PARITY_ERR_INT:

                cnt_reg = DRCH_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_OCB_ECC_PARITY_ERR_INT:

                cnt_reg = OCB_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_OAMP_ECC_PARITY_ERR_INT:

                cnt_reg = OAMP_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_DRCD_DRAM_CMD_PARITY_ERR:
    case JER_INT_DRCD_ECC_PARITY_ERR_INT:

                cnt_reg = DRCD_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_FDA_ECC_PARITY_ERR_INT:

                cnt_reg = FDA_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_EGQ_ECC_PARITY_ERR_INT:

                cnt_reg = EGQ_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_FMAC_ECC_PARITY_ERR_INT:

                cnt_reg = FMAC_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_ILKN_PMH_ECC_PARITY_ERR_INT:

                cnt_reg = ILKN_PMH_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_IQMT_ECC_PARITY_ERR_INT:

                cnt_reg = IQMT_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_DRCF_DRAM_CMD_PARITY_ERR:
    case JER_INT_DRCF_ECC_PARITY_ERR_INT:

                cnt_reg = DRCF_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_SCH_ECC_PARITY_ERR_INT:

                cnt_reg = SCH_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_IHP_ECC_PARITY_ERR_INT:

                cnt_reg = IHP_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_RTP_ECC_PARITY_ERR_INT:

                cnt_reg = RTP_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_FDT_ECC_PARITY_ERR_INT:

                cnt_reg = FDT_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_NBIH_ECC_PARITY_ERR_INT:

                cnt_reg = NBIH_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_MESH_TOPOLOGY_ECC_PARITY_ERR_INT:

                cnt_reg = MESH_TOPOLOGY_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_DRCC_DRAM_CMD_PARITY_ERR:
    case JER_INT_DRCC_ECC_PARITY_ERR_INT:

                cnt_reg = DRCC_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_IQM_ECC_PARITY_ERR_INT:

                cnt_reg = IQM_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_FDR_ECC_PARITY_ERR_INT:

                cnt_reg = FDR_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_DRCG_DRAM_CMD_PARITY_ERR:
    case JER_INT_DRCG_ECC_PARITY_ERR_INT:

                cnt_reg = DRCG_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_EPNI_ECC_PARITY_ERR_INT:

                cnt_reg = EPNI_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_FCR_ECC_PARITY_ERR_INT:

                cnt_reg = FCR_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_ILKN_PML_ECC_PARITY_ERR_INT:

                cnt_reg = ILKN_PML_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_MMU_ECC_PARITY_ERR_INT:

                cnt_reg = MMU_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_OLP_ECC_PARITY_ERR_INT:

                cnt_reg = OLP_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_CFC_ECC_PARITY_ERR_INT:

                cnt_reg = CFC_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_FCT_ECC_PARITY_ERR_INT:

                cnt_reg = FCT_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_DRCA_DRAM_CMD_PARITY_ERR:
    case JER_INT_DRCA_ECC_PARITY_ERR_INT:

                cnt_reg = DRCA_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_EDB_ECC_PARITY_ERR_INT:

                cnt_reg = EDB_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_IPT_ECC_PARITY_ERR_INT:

                cnt_reg = IPT_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_DRCE_DRAM_CMD_PARITY_ERR:
    case JER_INT_DRCE_ECC_PARITY_ERR_INT:

                cnt_reg = DRCE_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;
    case JER_INT_DRCB_DRAM_CMD_PARITY_ERR:
    case JER_INT_DRCB_ECC_PARITY_ERR_INT:

                cnt_reg = DRCB_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(reg2first_schan(unit,cnt_reg,&block));
                is_par =  TRUE;
                break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt ")));
      }

    if(is_par) {

         rc = soc_reg_get(unit, cnt_reg, REG_PORT_ANY, 0, &counter);
         SOCDNX_IF_ERR_EXIT(rc);

        /* get address validity bit */
        addr_validf = soc_reg64_field32_get(unit, cnt_reg, counter, PARITY_ERR_ADDR_VALIDf);

        /* get memory address bit */
        addrf = soc_reg64_field32_get(unit, cnt_reg, counter, PARITY_ERR_ADDRf);

         /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, cnt_reg, counter, PARITY_ERR_CNT_OVERFLOWf);

        /* get counter value of 2 bit error */
        cntf = soc_reg64_field32_get(unit, cnt_reg, counter, PARITY_ERR_CNTf);
    }    

    if(is_ecc_2b) {

        rc = soc_reg_get(unit, cnt_reg, REG_PORT_ANY, 0, &counter);
         SOCDNX_IF_ERR_EXIT(rc);

        /* get address validity bit */
        addr_validf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_2B_ERR_ADDR_VALIDf);

        /* get memory address bit */
        addrf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_2B_ERR_ADDRf);

        /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_2B_ERR_CNT_OVERFLOWf);

        /* get counter value of 2 bit error */
        cntf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_2B_ERR_CNTf);
    }

    if (addr_validf != 0) {

        mem = soc_addr_to_mem_extended(unit, block, 0xff, addrf);

        if(mem!= INVALIDm) {
            memory_name = SOC_MEM_NAME(unit, mem);
        } else {
            memory_name = NULL;
        }
         
        switch(mem) {
        case INVALIDm:
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory is not accessible",
                    cntf, cnt_overflowf, addrf);
            break;     
        default:

            /* support for shadow memories*/
            rc = interrupt_memory_cached(unit, mem, block_instance, &cached_flag);
            if(rc != SOC_E_NONE) {
                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory not cached",
                            cntf, cnt_overflowf, addrf);
            }
            else if(TRUE == cached_flag) {
            
                rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
                SOCDNX_IF_ERR_EXIT(rc);

                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                            cntf, cnt_overflowf, addrf, memory_name, numels, index);
                /* set corrective action */
                *p_corrective_action = JER_INT_CORR_ACT_SHADOW;
                shadow_correct_info->array_index = numels;
                shadow_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
                shadow_correct_info->min_index = index;
                shadow_correct_info->max_index = index;
                shadow_correct_info->mem = mem;
                
            } 
            else {
                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s",
                            cntf, cnt_overflowf, addrf, memory_name);
            }
        }
    
     } else {
         sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x address is not valid",
                     cntf, cnt_overflowf, addrf);
     }

exit:
    SOCDNX_FUNC_RETURN;
}




int
jer_interrupt_handles_corrective_action_do_nothing (
  int unit,
  int block_instance,
  jer_interrupt_type interrupt_id,
  char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*empty function*/

    SOCDNX_FUNC_RETURN;
}


int jer_interrupt_handles_corrective_action_soft_reset(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char *msg)
{
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rc = soc_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
jer_interrupt_handles_corrective_action_print(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char* msg_print,
    char* msg)
{
    soc_interrupt_db_t* interrupt;
    char print_msg[JER_INTERRUPT_PRINT_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id]);

    /* prepare string for print */
#if defined(SOC_NO_NAMES)
    sal_snprintf(print_msg, JER_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d | %s\n ", 
                interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#else
    sal_snprintf(print_msg, JER_INTERRUPT_PRINT_MSG_SIZE,"name=%s, id=%d, index=%d, block=%d, unit=%d | %s\n ", 
                interrupt->name, interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#endif

    LOG_INFO(BSL_LS_SOC_INTR, (BSL_META_U(unit,"%s"), print_msg));
    
exit:
    SOCDNX_FUNC_RETURN;
}

int jer_interrupt_handles_corrective_action_handle_oamp_event_fifo(int unit, 
                                                                       int block_instance, 
                                                                       jer_interrupt_type en_arad_interrupt, 
                                                                       char* msg)
{

	sal_dpc(arad_pp_oam_dma_event_handler, INT_TO_PTR(unit), INT_TO_PTR(JER_PP_OAM_API_DMA_EVENT_TYPE_EVENT),0,0,0 );
    return SOC_E_NONE;
}
 
int jer_interrupt_handles_corrective_action_handle_oamp_stat_event_fifo(int unit,
                                                                            int block_instance,
                                                                            jer_interrupt_type en_arad_interrupt,
                                                                            char* msg)
{

	sal_dpc(arad_pp_oam_dma_event_handler, INT_TO_PTR(unit), INT_TO_PTR(JER_PP_OAM_API_DMA_EVENT_TYPE_STAT_EVENT),0,0,0 );
    return SOC_E_NONE;
}
int
jer_interrupt_handles_corrective_action_shutdown_fbr_link(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char* msg)
{
   uint32 port = block_instance*4 + SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].bit_in_field;
    uint32 rc;
    SOCDNX_INIT_FUNC_DEFS;
    

    rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_enable_set, (unit, port, 0,0)); 
    SOCDNX_IF_ERR_EXIT(rc);

exit:


    SOCDNX_FUNC_RETURN;

}

int jer_interrupt_handles_corrective_action_hard_reset(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char *msg)
{
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "Decision: Hard reset!\n")));
#endif

    rc = soc_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


/* Corrective Action main function */
int
jer_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char *msg,
    jer_int_corr_act_type corr_act,
    void *param1, 
    void *param2)
{
    int rc;
    uint32 flags;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    SOCDNX_IF_ERR_EXIT(rc);
    
    /* Corrective action will be performed only if BCM_AND_USR_CB flag set or corrective action override flag not overriden */
    if(((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0) && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = JER_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
    case JER_INT_CORR_ACT_NONE:
        rc = jer_interrupt_handles_corrective_action_do_nothing(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case JER_INT_CORR_ACT_SOFT_RESET:
        rc = jer_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case JER_INT_CORR_ACT_HARD_RESET:
        rc = jer_interrupt_handles_corrective_action_hard_reset(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case JER_INT_CORR_ACT_PRINT:
        rc = jer_interrupt_handles_corrective_action_print(unit, block_instance, interrupt_id,(char*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case JER_INT_CORR_ACT_SHADOW:
        rc = jer_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,(jer_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case JER_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
        rc = jer_interrupt_handles_corrective_action_handle_oamp_event_fifo(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case JER_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO:
        rc = jer_interrupt_handles_corrective_action_handle_oamp_stat_event_fifo(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case JER_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
        rc = jer_interrupt_handles_corrective_action_shutdown_fbr_link(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("UnKnown corrective action")));
  }
   
exit:
  SOCDNX_FUNC_RETURN;
}

int jer_interrupt_print_info(
      int unit,
    int block_instance,
    jer_interrupt_type en_jer_interrupt,
    int recurring_action,
    jer_int_corr_act_type corr_act,
    char *special_msg)
{
    int rc;
    uint32 flags;  
    soc_interrupt_db_t* interrupt;

    char cur_special_msg[JER_INTERRUPT_SPECIAL_MSG_SIZE];
    char cur_corr_act_msg[JER_INTERRUPT_COR_ACT_MSG_SIZE];
    char print_msg[JER_INTERRUPT_PRINT_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_jer_interrupt]);

    if (special_msg == NULL) {
        sal_snprintf(cur_special_msg, sizeof(cur_special_msg), "None");
    } else {
        if(sal_strlen(special_msg) >= JER_INTERRUPT_SPECIAL_MSG_SIZE) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Too long special message"))); 
        }
        sal_strncpy(cur_special_msg, special_msg, sizeof(cur_special_msg) - 1);
    }

    rc = soc_interrupt_flags_get(unit, en_jer_interrupt, &flags);
    SOCDNX_IF_ERR_EXIT(rc);

    /* Corrective action will be performed only if BCM_AND_USR_CB flag set or corrective action override flag not overriden */
    if(((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0) && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = JER_INT_CORR_ACT_NONE;
    }
  
 

    switch(corr_act) {
        case JER_INT_CORR_ACT_HARD_RESET:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Hard Reset");
            break;
        case JER_INT_CORR_ACT_NONE:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "None");
            break;
        case JER_INT_CORR_ACT_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Soft Reset");
            break;
        case JER_INT_CORR_ACT_PRINT:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Print");
            break;
        case JER_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
            sal_sprintf(cur_corr_act_msg, "Handle OAMP Event Fifo");
            break;
        case JER_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO:
            sal_sprintf(cur_corr_act_msg, "Handle OAMP Statistics Event Fifo");
            break;
        case JER_INT_CORR_ACT_SHADOW:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Shadow");
            break;
        case JER_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown link");
            break;
        default:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Unknown");
    }

    /* prepare string for print */
#if defined(SOC_NO_NAMES)
    sal_snprintf(print_msg, JER_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ", 
                en_jer_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#else
    sal_snprintf(print_msg, JER_INTERRUPT_PRINT_MSG_SIZE,"name=%s, id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ", 
                interrupt->name, en_jer_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#endif

    /* Print per interrupt mechanism */
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE)) {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,"%s"), print_msg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int 
jer_interrupt_handles_corrective_action_shadow(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    jer_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg)
{
    int rc;
    uint32* data_entry=0;
    uint8 orig_read_through_flag;
    uint8 *vmap;
    int entry_dw;
    int current_index;
    soc_mem_t mem;
    int copyno;

    SOCDNX_INIT_FUNC_DEFS;

    mem = shadow_correct_info_p->mem;
    copyno = (shadow_correct_info_p->copyno == COPYNO_ALL) ? SOC_MEM_BLOCK_ANY(unit, mem) : shadow_correct_info_p->copyno;

    SOCDNX_NULL_CHECK(shadow_correct_info_p);
 
    if (arad_tbl_is_dynamic(unit, mem)) {
        SOC_EXIT;
    }

    if (!soc_mem_cache_get(unit, mem, copyno)) {
         SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
 
    entry_dw = soc_mem_entry_words(unit, mem);
    data_entry = sal_alloc((entry_dw * 4), "ARAD_INTERRUPT Shadow data entry allocation");

    if (NULL == data_entry) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    
    orig_read_through_flag = SOC_MEM_FORCE_READ_THROUGH(unit);
    SOC_MEM_FORCE_READ_THROUGH_SET(unit, 0); /* read from cache */
    for (current_index = shadow_correct_info_p->min_index ; current_index <= shadow_correct_info_p->max_index ; current_index++) {

        vmap = SOC_MEM_STATE(unit, mem).vmap[copyno];
        if (!CACHE_VMAP_TST(vmap, current_index) || SOC_MEM_TEST_SKIP_CACHE(unit)) {
            LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"Cache memory %s at entry %u is not available"),
                       SOC_MEM_NAME(unit, mem), current_index));
        }

        rc = soc_mem_array_read(unit, mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, data_entry);
        SOCDNX_IF_ERR_EXIT(rc);

        rc = soc_mem_array_write(unit, mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, data_entry);
        SOCDNX_IF_ERR_EXIT(rc);
    }
    SOC_MEM_FORCE_READ_THROUGH_SET(unit, orig_read_through_flag);

exit:
    if(data_entry != NULL) {
        sal_free(data_entry);
    }
    SOCDNX_FUNC_RETURN;
}



#undef _ERR_MSG_MODULE_NAME

