/*
 * $Id: $
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
 * File: qax_init.c
 */

#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/* 
 * Includes
 */ 

#include <shared/bsl.h>

/* SAL includes */
#include <sal/appl/sal.h>

/* SOC includes */
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/iproc.h>
#include <soc/mem.h>
#include <sal/core/thread.h> /* for sal_usleep */
#include <sal/core/libc.h> /* for sal_memset */

/* SOC DPP includes */
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/JER/jer_reset.h>
#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/QAX/qax_sram.h>
#include <soc/dpp/QAX/qax_mgmt.h>
#include <soc/dpp/QAX/qax_ipsec.h>

#define QAX_MASK_NIF_OVERRIDES


/* 
 * Init functions
 */

int soc_qax_init_blocks_init_global_conf(int unit)
{
    uint32 reg32_val, field_val;

    SOCDNX_INIT_FUNC_DEFS;

    /*Petra-b in system */
    if (SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_SYS_HEADER_CFGr(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_SYS_HEADER_CFGr, &reg32_val, SYSTEM_HEADERS_MODEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_SYS_HEADER_CFGr(unit, reg32_val));
    }

    /* Configure core mode, not in clear-channel mode */
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32_val));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_FABRICf, 0);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_LOCALf, SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32_val));

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_1r(unit, &reg32_val));
      /*
       *  Mesh Mode
       */
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_MESH || 
        /*treating single fap as mesh for tables configuration*/
        SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP )
    {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, MESH_MODEf, 1);
    }
    else
    {
        /* Mesh Not enabled, also for BACK2BACK devices */
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, MESH_MODEf, 0);
    }


    if(SOC_DPP_CONFIG(unit)->arad->init.fabric.is_128_in_system) {
        field_val = 0x1;
    } else if (SOC_DPP_CONFIG(unit)->arad->init.fabric.system_contains_multiple_pipe_device) {
        field_val = 0x2;
    } else {
        field_val = 0x0;
    }   
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, RESERVED_QTSf, field_val);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, TDM_ATTRIBUTEf, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_1r(unit, reg32_val));
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_3r(unit, &reg32_val));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_3r, &reg32_val, PACKET_CRC_ENf, 1);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_3r, &reg32_val, TOD_MODEf, 3);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_3r(unit, reg32_val));

    /* Init FDT transactions counter setting */
    SOCDNX_IF_ERR_EXIT(READ_TXQ_FDT_PRG_COUNTER_CFGr(unit, 0, &reg32_val));
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_MC_MASKf, 1);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_MC_VALf, 0);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_SRAM_MASKf, 0);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_SRAM_VALf, 1);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_CTXT_MASKf, 1);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_CTXT_VALf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_TXQ_FDT_PRG_COUNTER_CFGr(unit, 0, reg32_val));

    SOCDNX_IF_ERR_EXIT(READ_TXQ_FDT_PRG_COUNTER_CFGr(unit, 1, &reg32_val));
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_MC_MASKf, 1);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_MC_VALf, 0);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_SRAM_MASKf, 0);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_SRAM_VALf, 1);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_CTXT_MASKf, 1);
    soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_CTXT_VALf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_TXQ_FDT_PRG_COUNTER_CFGr(unit, 1, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_init_blocks_init_general_conf(int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32                 reg32_val = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_TXI_CREDITS_INIT_VALUEf, 0X0);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_RSTNf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_OAMP_STRICT_PRIORITYf, 0X0);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_EGQ_1_ENABLEf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_OAMP_ENABLEf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_EGQ_0_RXI_RESET_Nf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_EGQ_0_ENABLEf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PMH_SYNCE_RSTNf, 0X0);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_TXI_CREDITS_INITf, 0X0);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_OAMP_RXI_RESET_Nf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_EGQ_1_RXI_RESET_Nf, 0X1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above_64_val));

    /* Take NIF TXIs in EGQ out of reset */
    SOCDNX_IF_ERR_EXIT(soc_jer_reset_nif_txi_oor(unit));

    /* 
     * CGM configurations
     */
#ifdef QAX_COMMENT_OUT_CODE_NOT_YET_IMPLEMENTED
    reg32_val = 0;
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_2f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_3f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_0f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_5f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_4f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_1f, 0X8);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_BDBLL_BANK_SIZESr(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IQMT_IQM_BDB_OFFSETr, &reg32_val, IQM_1_BDB_OFFSETf, 0X0);
    soc_reg_field_set(unit, IQMT_IQM_BDB_OFFSETr, &reg32_val, IQM_0_BDB_OFFSETf, 0X0);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_IQM_BDB_OFFSETr(unit, reg32_val));
#endif

    /* IPS configuration*/
    /* don't pass update indication from the IPS to the CGM on SQM dequeue commands */
    SOCDNX_IF_ERR_EXIT(READ_IPS_UPDATE_INDICATIONr(unit, &reg32_val));
    soc_reg_field_set(unit, IPS_UPDATE_INDICATIONr, &reg32_val, FORCE_NO_UPDATEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_UPDATE_INDICATIONr(unit, reg32_val));

    /* set ITE_ITPP_GENERAL_CFGr */
    /* INVALID CUD value is set to 0, instead of the default 0xfffe for ingress */
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, UDH_FIXED_LENGTHf, 0x16);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, ENABLE_EEI_STAMPINGf, 1);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, ALWAYS_CHECK_IP_COMPATIBLE_MCf, 1);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, CUD_1_INVALID_VALUEf, 0x0);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, ENABLE_TERMINATIONf, 1);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, CUD_0_INVALID_VALUEf, 0x0);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, ENABLE_FALLBACK_TO_BRIDGEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_ITPP_GENERAL_CFGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOCDNX_IF_ERR_EXIT(WRITE_TAR_INVALID_CUDr(unit, 0));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_init_blocks_init_conf(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    /* ECI Globals configurations */
    SOCDNX_IF_ERR_EXIT(soc_qax_init_blocks_init_global_conf(unit));

    /* General blocks configurations */
    SOCDNX_IF_ERR_EXIT(soc_qax_init_blocks_init_general_conf(unit));

    /* Configure Sram and Dram buffers */

    /* Configure Sram buffers */
    SOCDNX_IF_ERR_EXIT( soc_qax_sram_conf_set(unit));

    /* Configure Dram Buffers */
    SOCDNX_IF_ERR_EXIT(soc_qax_dram_buffer_conf_set(unit));

    /* Setting revision fixes bits */
    SOCDNX_IF_ERR_EXIT(qax_mgmt_revision_fixes(unit));

    /* Init IPSEC block */
    SOCDNX_IF_ERR_EXIT(soc_qax_ipsec_init(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

/* Clear the portion of the memory entry that is used by the specific memory */
STATIC void clear_entry(int unit, void *entry, soc_mem_t mem) {
    sal_memset(entry, 0, (soc_mem_entry_bytes(unit, mem)+3) & 0xfc);
}

/* QAX register/memory settings as overrides part 0, to be later moved to the correct functions and removed */
STATIC int soc_qax_init_overrides_0(int unit)
{
    soc_reg_above_64_val_t above64;
    uint32 entry[20];
    int i;
    uint32 reg32;
    SOCDNX_INIT_FUNC_DEFS;

    reg32 = 0;
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_DATA_PATHf, 1);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_TCAM_BUBBLESf, 1);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_MACT_BUBBLESf, 1);

    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, NON_FIRST_FRAGMENTS_ENABLEf,
      soc_property_get(unit, spn_FIELD_IP_FIRST_FRAGMENT_PARSED, TRUE) ? 1 : 0);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, MAX_HEADER_STACKf, 5);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ADDITIONAL_TPID_ENABLEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_IHP_ENABLERSr(unit, REG_PORT_ANY, reg32));

    /* start - code translated from the BCM shell script */

#if 0
    SOCDNX_IF_ERR_EXIT(READ_CMIC_SBUS_RING_MAP_32_39r(unit, &reg32));
    soc_reg_field_set(unit, CMIC_SBUS_RING_MAP_32_39r, &reg32, RING_NUM_SBUS_ID_36f, 2);
    soc_reg_field_set(unit, CMIC_SBUS_RING_MAP_32_39r, &reg32, RING_NUM_SBUS_ID_37f, 2);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, reg32));
#endif
    /* configure broadcast IDs */
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_SBUS_BROADCAST_IDr(unit, REG_PORT_ANY, 0x7f));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, REG_PORT_ANY, 0x7f));
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_SBUS_BROADCAST_IDr(unit, REG_PORT_ANY, 0x7f));
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_SBUS_BROADCAST_IDr(unit, REG_PORT_ANY, 0x7f));
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_SBUS_BROADCAST_IDr(unit, REG_PORT_ANY, 0x7f));
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_SBUS_BROADCAST_IDr(unit, REG_PORT_ANY, 0x7f));
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_SBUS_BROADCAST_IDr(unit, REG_PORT_ANY, 0x7f));
    
    SOCDNX_IF_ERR_EXIT(WRITE_CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_DDP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_IEP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_ILB_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_IMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_IPSEC_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_ITE_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_KAPS_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_PEM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_PTS_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_TAR_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_TXQ_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));


    SOC_REG_ABOVE_64_CLEAR(above64); /* take all blocks out of sbus reset */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_BLOCKS_SBUS_RESETr, REG_PORT_ANY, 0, above64));

    SOC_REG_ABOVE_64_CLEAR(above64); /* take all blocks out of soft reset */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_BLOCKS_SOFT_RESETr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set misc controls */
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above64, PIR_RSTNf, 1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above64, PDM_RSTNf, 1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above64, PIR_EGQ_1_ENABLEf, 1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above64, PIR_OAMP_ENABLEf, 1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above64, PIR_EGQ_0_RXI_RESET_Nf, 1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above64, PIR_EGQ_0_ENABLEf, 1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above64, PIR_OAMP_RXI_RESET_Nf, 1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above64, PIR_EGQ_1_RXI_RESET_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_GP_CONTROL_9r, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_RESERVED_PCMI_1r(unit, 0x74cdcfa8));
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_RESERVED_PCMI_0r(unit, 0x55c57214));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ECI_GTIMER_CONFIGURATIONr */
    soc_reg_above_64_field32_set(unit, ECI_GTIMER_CONFIGURATIONr, above64, GTIMER_RESET_ON_TRIGGERf, 1);
    soc_reg_above_64_field32_set(unit, ECI_GTIMER_CONFIGURATIONr, above64, GTIMER_CYCLEf, 0xc306dc0);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_GTIMER_CONFIGURATIONr, REG_PORT_ANY, 0, above64));
    /* set global configuration in ECI */
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ECI_GLOBAL_MEM_OPTIONSr */
    soc_reg_above_64_field32_set(unit, ECI_GLOBAL_MEM_OPTIONSr, above64, MEM_RC_DISABLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_GLOBAL_MEM_OPTIONSr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_RESERVED_PCMI_2r(unit, 0xbba30f8e));
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_RESERVED_PCMI_3r(unit, 0xbf12b944));
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_RESERVED_PCMI_4r(unit, 0x939419fa));

    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("access to registers should be fixed for Jericho plus at places we used _REG(32|64) access routines")));
    }

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_OGER_1008r_REG32(unit, 0x20101977));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ECI_SYNCE_MASTER_PLL_CONFIGr */
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_LC_BOOSTf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_KIf, 4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_STAT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_NDIVf, 0xe4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PWRONf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_KPf, 6);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_BYPASS_PLL_ENf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_RESERVED_1f, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PWRON_LDOf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PDIVf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_PWM_RATEf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ECI_SYNCE_SLAVE_PLL_CONFIGr */
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PLL_CTRL_PWM_RATEf, 2);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PWRON_LDOf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PWRONf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_KIf, 4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_RESERVED_1f, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_KPf, 6);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PLL_CTRL_LC_BOOSTf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PDIVf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_NDIVf, 0xe4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PLL_CTRL_STAT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_BYPASS_PLL_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ECI_SYNCE_MASTER_PLL_CONFIGr */
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_LC_BOOSTf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_KIf, 4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_STAT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_NDIVf, 0xe4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PWRONf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_KPf, 6);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_BYPASS_PLL_ENf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_RESERVED_1f, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PWRON_LDOf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_RESET_Bf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PDIVf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_PWM_RATEf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ECI_SYNCE_SLAVE_PLL_CONFIGr */
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PLL_CTRL_PWM_RATEf, 2);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PWRON_LDOf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_RESET_Bf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PWRONf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_KIf, 4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_RESERVED_1f, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_KPf, 6);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PLL_CTRL_LC_BOOSTf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PDIVf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_NDIVf, 0xe4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PLL_CTRL_STAT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_BYPASS_PLL_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ECI_SYNCE_SLAVE_PLL_CONFIGr */
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_POST_RESET_Bf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PLL_CTRL_PWM_RATEf, 2);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PWRON_LDOf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_RESET_Bf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PWRONf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_KIf, 4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_RESERVED_1f, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_KPf, 6);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PLL_CTRL_LC_BOOSTf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PDIVf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_NDIVf, 0xe4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_PLL_CTRL_STAT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, above64, SYNCE_SLAVE_PLL_CFG_BYPASS_PLL_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ECI_SYNCE_MASTER_PLL_CONFIGr */
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_LC_BOOSTf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_KIf, 4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_STAT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_NDIVf, 0xe4);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PWRONf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_KPf, 6);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_BYPASS_PLL_ENf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_RESERVED_1f, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PWRON_LDOf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_POST_RESET_Bf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_RESET_Bf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PDIVf, 1);
    soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, above64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_PWM_RATEf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, REG_PORT_ANY, 0, above64));

#ifndef QAX_MASK_NIF_OVERRIDES
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_TX_QMLF_CONFIGr[1] */
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_START_TX_THRESHOLD_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_TX_QMLF_CONFIGr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_QMLF_CONFIGr[9] */
    soc_reg_above_64_field32_set(unit, NBIH_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, 9, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_QMLF_CONFIGr[5] */
    soc_reg_above_64_field32_set(unit, NBIH_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, 5, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_QMLF_CONFIGr[4] */
    soc_reg_above_64_field32_set(unit, NBIH_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, 4, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_QMLF_CONFIGr[11] */
    soc_reg_above_64_field32_set(unit, NBIH_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, 11, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_QMLF_CONFIGr[10] */
    soc_reg_above_64_field32_set(unit, NBIH_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, 10, above64));
#endif /* QAX_MASK_NIF_OVERRIDES */



/*#ifndef QAX_MASK_NIF_OVERRIDES*/
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_NIF_PMH_DATA_BYPASS_MUXr[6] */
    soc_reg_above_64_field32_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, above64, DATA_BYPASS_MODE_ILKN_NOT_FMAL_LANE_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, REG_PORT_ANY, 6, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_NIF_PMH_DATA_BYPASS_MUXr[2] */
    soc_reg_above_64_field32_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, above64, DATA_BYPASS_MODE_ILKN_NOT_FMAL_LANE_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_NIF_PMH_DATA_BYPASS_MUXr[1] */
    soc_reg_above_64_field32_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, above64, DATA_BYPASS_MODE_ILKN_NOT_FMAL_LANE_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_NIF_PMH_DATA_BYPASS_MUXr[0] */
    soc_reg_above_64_field32_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, above64, DATA_BYPASS_MODE_ILKN_NOT_FMAL_LANE_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_NIF_PMH_DATA_BYPASS_MUXr[3] */
    soc_reg_above_64_field32_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, above64, DATA_BYPASS_MODE_ILKN_NOT_FMAL_LANE_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_NIF_PMH_DATA_BYPASS_MUXr[4] */
    soc_reg_above_64_field32_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, above64, DATA_BYPASS_MODE_ILKN_NOT_FMAL_LANE_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, REG_PORT_ANY, 4, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_NIF_PMH_DATA_BYPASS_MUXr[5] */
    soc_reg_above_64_field32_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, above64, DATA_BYPASS_MODE_ILKN_NOT_FMAL_LANE_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, REG_PORT_ANY, 5, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_NIF_PMH_DATA_BYPASS_MUXr[7] */
    soc_reg_above_64_field32_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, above64, DATA_BYPASS_MODE_ILKN_NOT_FMAL_LANE_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, REG_PORT_ANY, 7, above64));
/*#endif QAX_MASK_NIF_OVERRIDES*/



#ifndef QAX_MASK_NIF_OVERRIDES
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_QMLF_CONFIGr[0] */
    soc_reg_above_64_field32_set(unit, NBIH_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_QMLF_CONFIGr[6] */
    soc_reg_above_64_field32_set(unit, NBIH_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, 6, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_QMLF_CONFIGr[7] */
    soc_reg_above_64_field32_set(unit, NBIH_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, 7, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_QMLF_CONFIGr[8] */
    soc_reg_above_64_field32_set(unit, NBIH_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, 8, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIL_RX_QMLF_CONFIGr[3] */
    soc_reg_above_64_field32_set(unit, NBIL_RX_QMLF_CONFIGr, above64, RX_THRESHOLD_AFTER_OVERFLOW_QMLF_Nf, 0x64);
    soc_reg_above_64_field32_set(unit, NBIL_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIL_RX_QMLF_CONFIGr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIL_RX_QMLF_CONFIGr[1] */
    soc_reg_above_64_field32_set(unit, NBIL_RX_QMLF_CONFIGr, above64, RX_THRESHOLD_AFTER_OVERFLOW_QMLF_Nf, 0x64);
    soc_reg_above_64_field32_set(unit, NBIL_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIL_RX_QMLF_CONFIGr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIL_RX_QMLF_CONFIGr[0] */
    soc_reg_above_64_field32_set(unit, NBIL_RX_QMLF_CONFIGr, above64, RX_THRESHOLD_AFTER_OVERFLOW_QMLF_Nf, 0x64);
    soc_reg_above_64_field32_set(unit, NBIL_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIL_RX_QMLF_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIL_RX_QMLF_CONFIGr[2] */
    soc_reg_above_64_field32_set(unit, NBIL_RX_QMLF_CONFIGr, above64, RX_THRESHOLD_AFTER_OVERFLOW_QMLF_Nf, 0x64);
    soc_reg_above_64_field32_set(unit, NBIL_RX_QMLF_CONFIGr, above64, RX_PORT_MODE_QMLF_Nf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIL_RX_QMLF_CONFIGr, REG_PORT_ANY, 2, above64));
#endif /* QAX_MASK_NIF_OVERRIDES */

    if (SOC_IS_KALIA(unit)){

        /*Allow DRAM and SRAM contexts from same Q-Pair of the following buffers to interleave between them: FMC-LP,FMC-HP,FUC-LP,FUC-HP*/
        /*Elij - temp: I somehow couldn't do it in soc_qax_init_blocks_init_conf*/
        SOCDNX_IF_ERR_EXIT(READ_PTS_PACKET_DEQUEUE_GENERAL_CONFIGSr(unit, &reg32));
        soc_reg_field_set(unit, PTS_PACKET_DEQUEUE_GENERAL_CONFIGSr, &reg32, FAB_MEM_INTERLEAVE_ENf, 0xf);
        SOCDNX_IF_ERR_EXIT(WRITE_PTS_PACKET_DEQUEUE_GENERAL_CONFIGSr(unit, reg32));


    }

    /*Elij - temp till part 3 -> start*/

    /*      w  TXQ_PER_DTQ_CFG 0 1 QUEUE_SIZE=255
            w  TXQ_PER_DTQ_CFG 1 1 QUEUE_SIZE=1023 QUEUE_START=256
            w  TXQ_PER_DTQ_CFG 2 4 QUEUE_SIZE=2 QUEUE_START=1500
            w  TXQ_PER_DTQ_CFG 0 6 DTQ_SPR_FC_TH=1000  DTQ_PDQ_FC_2_TH=1000  DTQ_PDQ_FC_1_TH=1000 */

    /*TXQ DTQ SRAM 0*/
    clear_entry(unit, entry, TXQ_PER_DTQ_CFGm);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_STARTf, 0);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_SIZEf, 127);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_2_THf, 89);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_1_THf, 64);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_SPR_FC_THf, 89);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, TXQ_PER_DTQ_CFGm, 0, MEM_BLOCK_ALL, 0, entry));
    /*TXQ DTQ DRAM 0*/
    clear_entry(unit, entry, TXQ_PER_DTQ_CFGm);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_STARTf, 0);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_SIZEf, 511);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_2_THf, 356);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_1_THf, 256);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_SPR_FC_THf, 356);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, TXQ_PER_DTQ_CFGm, 0, MEM_BLOCK_ALL, 1, entry));
    /*TXQ DTQ SRAM 1*/
    clear_entry(unit, entry, TXQ_PER_DTQ_CFGm);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_STARTf, 128);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_SIZEf, 63);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_2_THf, 45);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_1_THf, 32);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_SPR_FC_THf, 45);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, TXQ_PER_DTQ_CFGm, 0, MEM_BLOCK_ALL, 2, entry));
    /*TXQ DTQ DRAM 1*/
    clear_entry(unit, entry, TXQ_PER_DTQ_CFGm);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_STARTf, 512);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_SIZEf, 255);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_2_THf, 179);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_1_THf, 128);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_SPR_FC_THf, 179);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, TXQ_PER_DTQ_CFGm, 0, MEM_BLOCK_ALL, 3, entry));
    /*TXQ DTQ SRAM 2*/
    clear_entry(unit, entry, TXQ_PER_DTQ_CFGm);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_STARTf, 192);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_SIZEf, 63);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_2_THf, 45);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_1_THf, 32);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_SPR_FC_THf, 45);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, TXQ_PER_DTQ_CFGm, 0, MEM_BLOCK_ALL, 4, entry));
    /*TXQ DTQ DRAM 2*/
    clear_entry(unit, entry, TXQ_PER_DTQ_CFGm);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_STARTf, 768);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, QUEUE_SIZEf, 255);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_2_THf, 179);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_PDQ_FC_1_THf, 128);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry, DTQ_SPR_FC_THf, 179);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, TXQ_PER_DTQ_CFGm, 0, MEM_BLOCK_ALL, 5, entry));


    /*
        TXQ_SRAM_DTQ_NOF_ROWS                                   256
        TXQ_DRAM_DTQ_NOF_ROWS                                   1024

        DTQ_PDQ_FC_2_THf = QUEUE_SIZE*70/100 + 1
        DTQ_PDQ_FC_1_THf = QUEUE_SIZE*50/100 + 1
        DTQ_SPR_FC_THf  = QUEUE_SIZE*70/100 + 1;

    */

    
    /*PDQ_SRAM_LOC_HP               0*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x0);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x1ff);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x17f4);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0xaa);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0xa9);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 0, entry));
    /*PDQ_DRAM_LOC_HP               1*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x0);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x91);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x6cc);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0x48);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0x48);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 1, entry));
    /*PDQ_SRAM_LOC_LP               2*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x200);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x1ff);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x17f4);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0xaa);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0xa9);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 2, entry));
    /*PDQ_DRAM_LOC_LP               3*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x92);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x91);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x6cc);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0x48);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0x48);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 3, entry));
    /*PDQ_SRAM_FUC_HP                4*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x400);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x1ff);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x17f4);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0xaa);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0xa9);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 4, entry));
    /*PDQ_DRAM_FUC_HP                5*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x124);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x91);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x6cc);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0x48);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0x48);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 5, entry));
    /*PDQ_SRAM_FUC_LP                6*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x600);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x1ff);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x17f4);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0xaa);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0xa9);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 6, entry));
    /*PDQ_DRAM_FUC_LP                7*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x1b6);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x91);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x6cc);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0x48);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0x48);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 7, entry));
    /*PDQ_SRAM_FMC_HP                8*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x800);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x1ff);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x17f4);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0xaa);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0xa9);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 8, entry));
    /*PDQ_DRAM_FMC_HP                9*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x248);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x91);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x6cc);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0x48);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0x48);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 9, entry));
    /*PDQ_SRAM_FMC_LP                10*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0xa00);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x1ff);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x17f4);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0xaa);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0xa9);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 10, entry));
    /*PDQ_DRAM_FMC_LP                11*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x2da);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x91);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x6cc);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0x48);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0x48);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 11, entry));
    /*PDQ_SRAM_DEL                   12*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0xc00);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x54);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x3f0);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0x1c);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0x3a);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 12, entry));
    /*PDQ_DRAM_DEL                   13*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0x36c);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x91);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x6cc);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0x48);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0x48);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 13, entry));
    /*PDQ_SRAM_TO_DRAM_HP            14*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0xc55);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x154);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0xff0);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0x71);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0x70);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 14, entry));
    /*PDQ_DRAM_UNUSED                15*/
    /*PDQ_SRAM_TO_DRAM_LP            16*/
    clear_entry(unit, entry, PTS_PER_PDQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_STARTf, 0xdaa);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, QUEUE_SIZEf, 0x254);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_WORD_FC_THf, 0x1bf0);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, EIR_CRDT_FC_THf, 0xc6);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, entry, DQCQ_FC_THf, 0xc5);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 16, entry));



    SOC_REG_ABOVE_64_CLEAR(above64); /* set PTS_PDQ_MC_THr */
    soc_reg_above_64_field32_set(unit, PTS_PDQ_MC_THr, above64, PDQ_MC_GFMC_DRAM_THf, 0x6c);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_MC_THr, above64, PDQ_MC_BFMC_DRAM_THf, 0x6c);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_MC_THr, above64, PDQ_MC_GFMC_SRAM_THf, 0x17f);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_MC_THr, above64, PDQ_MC_BFMC_SRAM_THf, 0x17f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, PTS_PDQ_MC_THr, REG_PORT_ANY, 0, above64));

      /*Elij - temp for part 3 -> end*/



    for(i = 0; i< 4; ++i) {
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, i, 0x79));
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IHP_REG_0119r, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, CRPS_CRPS_INIT_ONr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DDP_DDP_INIT_STATUSr, REG_PORT_ANY, 0, above64));
    sal_usleep(50000);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, SPB_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    sal_usleep(200000);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, ILB_MODEM_FIFO_CONFIGr, REG_PORT_ANY, 28, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IRE_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IEP_INCOMING_COLOR_MAPr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IMP_INCOMING_COLOR_MAPr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, PPDB_A_OEMA_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, PPDB_A_OEMB_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, PPDB_A_ISEM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, PPDB_B_LARGE_EM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, PPDB_B_ISEM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, EDB_GLEM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, EDB_ESEM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, above64));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, EGQ_EGQ_BLOCK_INIT_STATUSr, REG_PORT_ANY, 0, above64));

#ifndef QAX_MASK_NIF_OVERRIDES
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[76] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x4d);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 76, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[67] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x3e);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 67, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[66] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x6b);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 66, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[52] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x46);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 52, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[4] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 4, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[44] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x75);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 44, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[3] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x56);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[36] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0xa);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 36, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[2] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x4a);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[28] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x39);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 28, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[20] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x11);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 20, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[118] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x50);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 118, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[117] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x7c);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 117, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[112] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 112, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[108] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x47);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 108, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[104] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x6d);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 104, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[0] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x7a);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[100] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x5a);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 100, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[116] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x22);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 116, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[119] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x62);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 119, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[16] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0xb);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 16, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[1] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x4e);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[24] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x2b);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 24, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[32] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 32, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[40] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x1a);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 40, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[48] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x5c);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 48, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[56] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x70);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 56, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[60] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x37);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 60, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[64] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x51);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 64, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[65] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 65, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[68] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0xf);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 68, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[72] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x74);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 72, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[80] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x25);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 80, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[84] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x78);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 84, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[88] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x40);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 88, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[92] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x73);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 92, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_EGQ_PORT_FOR_NIF_PORTr[96] */
    soc_reg_above_64_field32_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, above64, EGQ_PORT_FOR_NIF_PORTf, 0x12);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_EGQ_PORT_FOR_NIF_PORTr, REG_PORT_ANY, 96, above64));
#endif /* QAX_MASK_NIF_OVERRIDES */

    for (i = 0; i < 256; ++i) {
        clear_entry(unit, entry, RTP_UNICAST_DISTRIBUTION_MEMORYm);
        {uint32 field_val[10]={0x0000ff00, 0xff000}; soc_mem_field_set(unit, RTP_UNICAST_DISTRIBUTION_MEMORYm, entry, LINK_BITMAPf, field_val);}
        SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, RTP_UNICAST_DISTRIBUTION_MEMORYm, 0, MEM_BLOCK_ALL, i, entry));
        clear_entry(unit, entry, RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm);
        {uint32 field_val[10]={0x0000ff00, 0xff000}; soc_mem_field_set(unit, RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm, entry, LINK_BITMAPf, field_val);}
        SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm, 0, MEM_BLOCK_ALL, i, entry));
        clear_entry(unit, entry, RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm);
        {uint32 field_val[10]={0x0000ff00, 0xff000}; soc_mem_field_set(unit, RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm, entry, LINK_BITMAPf, field_val);}
        SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm, 0, MEM_BLOCK_ALL, i, entry));
    }
    SOC_REG_ABOVE_64_CLEAR(above64); /* set RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr */
    soc_reg_above_64_field32_set(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, above64, MUL_NUM_TRAVf, 3);
    soc_reg_above_64_field32_set(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, above64, BYPASS_UPDATEf, 1);
    soc_reg_above_64_field32_set(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, above64, MC_SLOW_UPDATESf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set RTP_MC_TRAVERSE_RATEr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, RTP_MC_TRAVERSE_RATEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set RTP_MAXIMUM_BASE_INDEXr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, RTP_MAXIMUM_BASE_INDEXr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set RTP_MULTICAST_LINK_UPr */
    soc_reg_above_64_field32_set(unit, RTP_MULTICAST_LINK_UPr, above64, MC_LINK_UPf, 0xff00);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, RTP_MULTICAST_LINK_UPr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set RTP_RTP_ENABLEr */
    soc_reg_above_64_field32_set(unit, RTP_RTP_ENABLEr, above64, RTPWPf, 0x10);
    soc_reg_above_64_field32_set(unit, RTP_RTP_ENABLEr, above64, RCG_EN_MASKf, 1);
    soc_reg_above_64_field32_set(unit, RTP_RTP_ENABLEr, above64, RTPRTf, 1);
    soc_reg_above_64_field32_set(unit, RTP_RTP_ENABLEr, above64, RTP_DRMA_CRMA_UP_ENf, 1);
    soc_reg_above_64_field32_set(unit, RTP_RTP_ENABLEr, above64, RMGRf, 0x280);
    soc_reg_above_64_field32_set(unit, RTP_RTP_ENABLEr, above64, RTP_EN_MSKf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, RTP_RTP_ENABLEr, REG_PORT_ANY, 0, above64));
    clear_entry(unit, entry, ILB_PORT_CONFIG_TABLEm);
    soc_mem_field32_set(unit, ILB_PORT_CONFIG_TABLEm, entry, ITEM_2_2f, 1);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, ILB_PORT_CONFIG_TABLEm, 0, 0, MEM_BLOCK_ALL, 0, 0+(128-1), entry));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[9] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 9, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[6] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 6, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[5] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 5, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[3] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[2] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[1] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[11] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 11, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[10] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 10, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[0] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[4] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 4, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[7] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 7, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_LBRG_CONFIGURATIONr[8] */
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_OUT_OF_ORDERf, 0x1fff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_TIMEOUT_THf, 0xffff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_MAX_BUFFERSf, 0xfff);
    soc_reg_above_64_field32_set(unit, ILB_LBRG_CONFIGURATIONr, above64, LBRG_N_OUTPUT_PORTf, 0x7f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_LBRG_CONFIGURATIONr, REG_PORT_ANY, 8, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set SPB_DYNAMIC_CONFIGURATIONr */

    for (i=0; i < 16; ++i ) {
        clear_entry(unit, entry, CGM_DP_MAPPINGm);
        soc_mem_field32_set(unit, CGM_DP_MAPPINGm, entry, ING_DPf, i%4);
        soc_mem_field32_set(unit, CGM_DP_MAPPINGm, entry, EGR_DPf, i%4);
        SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, CGM_DP_MAPPINGm, 0, 0, MEM_BLOCK_ALL, i*16, i*16+15, entry));
    }

    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x10);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 1, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x14);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 2, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x18);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 3, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x1c);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 4, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x20);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 5, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x24);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 6, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x28);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 7, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x2c);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 8, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x30);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 9, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x34);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 10, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x38);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 11, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x3c);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 12, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0xe0);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x40);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 13, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0xe8);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x41);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 14, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0xf0);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x42);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 15, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0xf8);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x43);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 16, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x44);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 17, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x48);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 18, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x4c);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 19, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x50);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 20, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x54);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 21, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x58);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 22, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x5c);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 23, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x60);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 24, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x64);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 25, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x68);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 26, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x6c);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 27, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x70);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 28, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0x160);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x74);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 29, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0x168);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x75);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 30, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0x170);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x76);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 31, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0x178);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 0x77);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 32, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 8);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 1);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 34, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0x10);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 2);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 35, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0x18);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 3);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 36, entry));
    clear_entry(unit, entry, CGM_IPPPMm);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, PG_BASEf, 0x20);
    soc_mem_field32_set(unit, CGM_IPPPMm, entry, NIF_PORTf, 4);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_IPPPMm, 0, MEM_BLOCK_ALL, 37, entry));
    clear_entry(unit, entry, CGM_PP_RJCT_MASKm);
    soc_mem_field32_set(unit, CGM_PP_RJCT_MASKm, entry, VSQ_RJCT_MASKf, 0xffffff);
    soc_mem_field32_set(unit, CGM_PP_RJCT_MASKm, entry, VOQ_RJCT_MASKf, 0x1ff);
    soc_mem_field32_set(unit, CGM_PP_RJCT_MASKm, entry, GLBL_RJCT_MASKf, 0xf);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, CGM_PP_RJCT_MASKm, 0, 0, MEM_BLOCK_ALL, 0, 0+(8-1), entry));
    clear_entry(unit, entry, CGM_SYS_RED_QSIZE_RANGESm);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_11f, 0x6b0);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_8f, 0x680);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_6f, 0x5c0);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_3f, 0x4c0);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_14f, 0x6e0);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_7f, 0x5e0);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_4f, 0x580);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_10f, 0x6a0);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_5f, 0x5a0);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_1f, 0x380);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_13f, 0x6d0);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_12f, 0x6c0);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_9f, 0x690);
    soc_mem_field32_set(unit, CGM_SYS_RED_QSIZE_RANGESm, entry, RANGE_TH_2f, 0x480);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_SYS_RED_QSIZE_RANGESm, 0, MEM_BLOCK_ALL, 0, entry));
    clear_entry(unit, entry, CGM_TAR_FIFO_RJCT_THm);
    soc_mem_field32_set(unit, CGM_TAR_FIFO_RJCT_THm, entry, MIRROR_RJCT_THf, 0x3ff);
    soc_mem_field32_set(unit, CGM_TAR_FIFO_RJCT_THm, entry, SNOOP_RJCT_THf, 0x3ff);
    soc_mem_field32_set(unit, CGM_TAR_FIFO_RJCT_THm, entry, FWD_RJCT_THf, 0x3ff);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, CGM_TAR_FIFO_RJCT_THm, 0, 0, MEM_BLOCK_ALL, 0, 0+(12-1), entry));
    clear_entry(unit, entry, CGM_VOQ_DRAM_BOUND_PRMSm);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_FREE_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_FREE_MAX_THf, 0xa80);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_FREE_MAX_THf, 0xa80);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_FREE_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_MAX_THf, 0xa80);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_MAX_THf, 0xa80);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, CGM_VOQ_DRAM_BOUND_PRMSm, 0, 0, MEM_BLOCK_ALL, 0, 0+(3-1), entry));
    clear_entry(unit, entry, CGM_VOQ_DRAM_BOUND_PRMSm);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_FREE_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_MIN_THf, 0x1a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_FREE_MIN_THf, 0x1a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_FREE_MAX_THf, 0x2a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_MIN_THf, 0x1a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_FREE_MAX_THf, 0x2a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_FREE_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_MAX_THf, 0x2a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_FREE_MIN_THf, 0x1a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_MAX_THf, 0x2a0);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_VOQ_DRAM_BOUND_PRMSm, 0, MEM_BLOCK_ALL, 3, entry));
    clear_entry(unit, entry, CGM_VOQ_DRAM_BOUND_PRMSm);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_FREE_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_MIN_THf, 0x2a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_FREE_MIN_THf, 0x2a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_FREE_MAX_THf, 0x3a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_MIN_THf, 0x2a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_FREE_MAX_THf, 0x3a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_FREE_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_MAX_THf, 0x3a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_FREE_MIN_THf, 0x2a0);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_MAX_THf, 0x3a0);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_VOQ_DRAM_BOUND_PRMSm, 0, MEM_BLOCK_ALL, 4, entry));
    clear_entry(unit, entry, CGM_VOQ_DRAM_BOUND_PRMSm);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_FREE_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_FREE_MAX_THf, 0xa80);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_FREE_MAX_THf, 0xa80);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_FREE_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_MAX_THf, 0xa80);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_BOUND_MAX_THf, 0x780);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_RECOVERY_FREE_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_PDS_RECOVERY_MIN_THf, 0x180);
    soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, entry, SRAM_WORDS_BOUND_MAX_THf, 0xa80);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, CGM_VOQ_DRAM_BOUND_PRMSm, 0, 0, MEM_BLOCK_ALL, 5, 5+(59-1), entry));
    clear_entry(unit, entry, CGM_VOQ_WORDS_RJCT_PRMSm);
    soc_mem_field32_set(unit, CGM_VOQ_WORDS_RJCT_PRMSm, entry, VOQ_FADT_MAX_THf, 0x980);
    soc_mem_field32_set(unit, CGM_VOQ_WORDS_RJCT_PRMSm, entry, VOQ_FADT_MIN_THf, 0x980);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, CGM_VOQ_WORDS_RJCT_PRMSm, MEM_BLOCK_ALL, entry));
    clear_entry(unit, entry, CGM_VSQ_GRNTD_RJCT_MASKm);
    soc_mem_field32_set(unit, CGM_VSQ_GRNTD_RJCT_MASKm, entry, VSQ_RJCT_MASKf, 0xffffff);
    soc_mem_field32_set(unit, CGM_VSQ_GRNTD_RJCT_MASKm, entry, VOQ_RJCT_MASKf, 0x1ff);
    soc_mem_field32_set(unit, CGM_VSQ_GRNTD_RJCT_MASKm, entry, GLBL_RJCT_MASKf, 0xf);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, CGM_VSQ_GRNTD_RJCT_MASKm, 0, 0, MEM_BLOCK_ALL, 0, 0+(8-1), entry));
    clear_entry(unit, entry, CGM_VSQ_PG_TC_BITMAPm);
    {uint32 field_val[10]={0x49249249, 0x2492}; soc_mem_field_set(unit, CGM_VSQ_PG_TC_BITMAPm, entry, VSQ_PG_OFFSETf, field_val);}
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_VSQ_PG_TC_BITMAPm, 0, MEM_BLOCK_ALL, 1, entry));
    clear_entry(unit, entry, CGM_VSQ_PG_TC_BITMAPm);
    {uint32 field_val[10]={0x92492492, 0x4924}; soc_mem_field_set(unit, CGM_VSQ_PG_TC_BITMAPm, entry, VSQ_PG_OFFSETf, field_val);}
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_VSQ_PG_TC_BITMAPm, 0, MEM_BLOCK_ALL, 2, entry));
    clear_entry(unit, entry, CGM_VSQ_PG_TC_BITMAPm);
    {uint32 field_val[10]={0xdb6db6db, 0x6db6}; soc_mem_field_set(unit, CGM_VSQ_PG_TC_BITMAPm, entry, VSQ_PG_OFFSETf, field_val);}
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_VSQ_PG_TC_BITMAPm, 0, MEM_BLOCK_ALL, 3, entry));
    clear_entry(unit, entry, CGM_VSQ_PG_TC_BITMAPm);
    {uint32 field_val[10]={0x24924924, 0x9249}; soc_mem_field_set(unit, CGM_VSQ_PG_TC_BITMAPm, entry, VSQ_PG_OFFSETf, field_val);}
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_VSQ_PG_TC_BITMAPm, 0, MEM_BLOCK_ALL, 4, entry));
    clear_entry(unit, entry, CGM_VSQ_PG_TC_BITMAPm);
    {uint32 field_val[10]={0x6db6db6d, 0xb6db}; soc_mem_field_set(unit, CGM_VSQ_PG_TC_BITMAPm, entry, VSQ_PG_OFFSETf, field_val);}
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_VSQ_PG_TC_BITMAPm, 0, MEM_BLOCK_ALL, 5, entry));
    clear_entry(unit, entry, CGM_VSQ_PG_TC_BITMAPm);
    {uint32 field_val[10]={0xb6db6db6, 0xdb6d}; soc_mem_field_set(unit, CGM_VSQ_PG_TC_BITMAPm, entry, VSQ_PG_OFFSETf, field_val);}
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_VSQ_PG_TC_BITMAPm, 0, MEM_BLOCK_ALL, 6, entry));
    clear_entry(unit, entry, CGM_VSQ_PG_TC_BITMAPm);
    {uint32 field_val[10]={0xffffffff, 0xffff}; soc_mem_field_set(unit, CGM_VSQ_PG_TC_BITMAPm, entry, VSQ_PG_OFFSETf, field_val);}
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, CGM_VSQ_PG_TC_BITMAPm, 0, MEM_BLOCK_ALL, 7, entry));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CGM_ST_ING_BILL_SETTINGSr */
    soc_reg_above_64_field32_set(unit, CGM_ST_ING_BILL_SETTINGSr, above64, ST_ING_BILL_PP_FILTER_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CGM_ST_ING_BILL_SETTINGSr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CGM_ST_QNUM_RANGE_FILTERr */
    soc_reg_above_64_field32_set(unit, CGM_ST_QNUM_RANGE_FILTERr, above64, ST_QNUM_FILTER_LOW_TH_1f, 1);
    soc_reg_above_64_field32_set(unit, CGM_ST_QNUM_RANGE_FILTERr, above64, ST_QNUM_FILTER_LOW_TH_0f, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CGM_ST_QNUM_RANGE_FILTERr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CGM_ST_QSIZE_SCRB_SETTINGSr */
    soc_reg_above_64_field32_set(unit, CGM_ST_QSIZE_SCRB_SETTINGSr, above64, ST_QSIZE_SCRB_QNUM_LOWf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CGM_ST_QSIZE_SCRB_SETTINGSr, REG_PORT_ANY, 0, above64));

exit:
    SOCDNX_FUNC_RETURN;
}

/* QAX register/memory settings as overrides part 1, to be later moved to the correct functions and removed */
STATIC int soc_qax_init_overrides_1(int unit)
{
    soc_reg_above_64_val_t above64;
    uint32 entry[20];
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(above64);
    clear_entry(unit, entry, EGQ_ACTION_PROFILE_TABLEm);
    soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, entry, DISCARDf, 1);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, EGQ_ACTION_PROFILE_TABLEm, 0, 0, MEM_BLOCK_ALL, 0, 0+(7-1), entry));
    clear_entry(unit, entry, EGQ_PMF_KEY_GEN_LSBm);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTION_1f, 0xf468);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTION_7_32Bf, 0xf9f0);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTION_5_16Bf, 0xf468);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTIONS_5678_KEYA_VALIDSf, 5);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTION_3f, 0xf9f0);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTIONS_1234_KEYA_VALIDSf, 5);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTIONS_5678_KEYB_VALIDSf, 5);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTIONS_1234_KEYB_VALIDSf, 5);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, EGQ_PMF_KEY_GEN_LSBm, 0, 0, MEM_BLOCK_ALL, 1, 1+(2-1), entry));
    clear_entry(unit, entry, EGQ_PMF_KEY_GEN_LSBm);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTION_7_32Bf, 0xf9f0);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTIONS_5678_KEYA_VALIDSf, 4);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTION_3f, 0xf9f0);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTIONS_1234_KEYA_VALIDSf, 4);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTIONS_5678_KEYB_VALIDSf, 4);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_LSBm, entry, INSTRUCTIONS_1234_KEYB_VALIDSf, 4);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, EGQ_PMF_KEY_GEN_LSBm, 0, 0, MEM_BLOCK_ALL, 9, 9+(2-1), entry));
    clear_entry(unit, entry, EGQ_PMF_KEY_GEN_MSBm);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTION_1f, 0xf3e8);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTION_7_32Bf, 0xfa10);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTION_5_16Bf, 0xf3e8);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTIONS_5678_KEYA_VALIDSf, 5);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTION_3f, 0xfa10);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTIONS_1234_KEYA_VALIDSf, 5);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTIONS_5678_KEYB_VALIDSf, 5);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTIONS_1234_KEYB_VALIDSf, 5);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, EGQ_PMF_KEY_GEN_MSBm, 0, 0, MEM_BLOCK_ALL, 1, 1+(2-1), entry));
    clear_entry(unit, entry, EGQ_PMF_KEY_GEN_MSBm);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTION_7_32Bf, 0xf9f0);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTIONS_5678_KEYA_VALIDSf, 4);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTION_3f, 0xf9f0);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTIONS_1234_KEYA_VALIDSf, 4);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTIONS_5678_KEYB_VALIDSf, 4);
    soc_mem_field32_set(unit, EGQ_PMF_KEY_GEN_MSBm, entry, INSTRUCTIONS_1234_KEYB_VALIDSf, 4);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, EGQ_PMF_KEY_GEN_MSBm, 0, 0, MEM_BLOCK_ALL, 9, 9+(2-1), entry));
    clear_entry(unit, entry, EGQ_PMF_PROGRAM_SELECTION_CAMm);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, FORMAT_CODE_MASKf, 7);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, VALUE_1_MASKf, 0xff);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, EGRESS_PMF_PROFILEf, 5);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, HEADER_CODE_MASKf, 0xf);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, PROGRAMf, 1);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, VALUE_2_MASKf, 0xff);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, ETHERNET_TAG_FORMAT_MASKf, 0x1f);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, QUALIFIER_MASKf, 0xf);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, 0, MEM_BLOCK_ALL, 0, entry));
    clear_entry(unit, entry, EGQ_PMF_PROGRAM_SELECTION_CAMm);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, FORMAT_CODE_MASKf, 7);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, VALUE_1_MASKf, 0xff);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, EGRESS_PMF_PROFILEf, 6);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, HEADER_CODE_MASKf, 0xf);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, PROGRAMf, 2);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, VALUE_2_MASKf, 0xff);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, ETHERNET_TAG_FORMAT_MASKf, 0x1f);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, QUALIFIER_MASKf, 0xf);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, 0, MEM_BLOCK_ALL, 1, entry));
    clear_entry(unit, entry, EGQ_PMF_PROGRAM_SELECTION_CAMm);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, FORMAT_CODE_MASKf, 7);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, VALUE_1_MASKf, 0xff);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, HEADER_CODE_MASKf, 0xf);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, VALUE_2_MASKf, 0xff);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, ETHERNET_TAG_FORMAT_MASKf, 0x1f);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, QUALIFIER_MASKf, 0xf);
    soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, entry, EGRESS_PMF_PROFILE_MASKf, 7);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, 0, 0, MEM_BLOCK_ALL, 2, 2+(14-1), entry));
   
    
exit:
    SOCDNX_FUNC_RETURN;
}

/* QAX register/memory settings as overrides part 4, to be later moved to the correct functions and removed */
STATIC int soc_qax_init_overrides_4(int unit)
{
    uint32 entry[20];
    SOCDNX_INIT_FUNC_DEFS;
    clear_entry(unit, entry, EPNI_PMF_MIRROR_PROFILE_TABLEm);
    soc_mem_field32_set(unit, EPNI_PMF_MIRROR_PROFILE_TABLEm, entry, FWD_ENABLEf, 1);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, EPNI_PMF_MIRROR_PROFILE_TABLEm, 0, 0, MEM_BLOCK_ALL, 0, 0+(16-1), entry));

exit:
    SOCDNX_FUNC_RETURN;
}

/* QAX register/memory settings as overrides part 5, to be later moved to the correct functions and removed */
STATIC int soc_qax_init_overrides_5(int unit)
{
    soc_reg_above_64_val_t above64, field_a64;
    uint32 entry[20];
    SOCDNX_INIT_FUNC_DEFS;

    clear_entry(unit, entry, EPNI_REC_CMD_CONFm);
    soc_mem_field32_set(unit, EPNI_REC_CMD_CONFm, entry, MIRROR_PROBABILITYf, 0xffffffff);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, EPNI_REC_CMD_CONFm, 0, 0, MEM_BLOCK_ALL, 0, 0+(16-1), entry));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_WFQ_PACKET_SIZEr */
    soc_reg_above_64_field32_set(unit, EGQ_WFQ_PACKET_SIZEr, above64, WFQ_PKT_SIZEf, 0x501);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_WFQ_PACKET_SIZEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_PS_MODEr */
    soc_reg_above_64_field32_set(unit, EGQ_PS_MODEr, above64, PS_MODEf, 0x55555);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_PS_MODEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr */
    soc_reg_above_64_field32_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, above64, MAP_FLEXIBLE_UDH_0_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, above64, MAP_FLEXIBLE_UDH_3_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, above64, MAP_FLEXIBLE_UDH_1_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, above64, MAP_FLEXIBLE_UDH_2_TYPE_TO_SIZEf, 0x8d1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_LAG_FILTER_ENABLEr */
    soc_reg_above_64_field32_set(unit, EGQ_LAG_FILTER_ENABLEr, above64, ENABLE_LAG_FILTER_MCf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_LAG_FILTER_ENABLEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_KEYA_DATA_BASE_PROFILEr */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0xffffffff; field_a64[1]=0xffff;
    soc_reg_above_64_field_set(unit, EGQ_KEYA_DATA_BASE_PROFILEr, above64, KEY_A_DATA_BASE_PROFILEf, field_a64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_KEYA_DATA_BASE_PROFILEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_GENERAL_RQP_CONFIGr */
    soc_reg_above_64_field32_set(unit, EGQ_GENERAL_RQP_CONFIGr, above64, RCM_MODEf, 2);
    soc_reg_above_64_field32_set(unit, EGQ_GENERAL_RQP_CONFIGr, above64, RCM_TDM_RESERVEDf, 0xa);
    soc_reg_above_64_field32_set(unit, EGQ_GENERAL_RQP_CONFIGr, above64, FIELD_13_17f, 0xe);
    soc_reg_above_64_field32_set(unit, EGQ_GENERAL_RQP_CONFIGr, above64, FIELD_18_18f, 1);
    soc_reg_above_64_field32_set(unit, EGQ_GENERAL_RQP_CONFIGr, above64, FIELD_11_11f, 1);
    soc_reg_above_64_field32_set(unit, EGQ_GENERAL_RQP_CONFIGr, above64, FIELD_19_19f, 1);
    soc_reg_above_64_field32_set(unit, EGQ_GENERAL_RQP_CONFIGr, above64, FIELD_20_20f, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_GENERAL_RQP_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_CNM_GENERAL_CONFIGr */
    soc_reg_above_64_field32_set(unit, EGQ_CNM_GENERAL_CONFIGr, above64, CNM_ETHERNET_TYPE_CODEf, 8);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_CNM_GENERAL_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_ACTION_PROFILE_5r */
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_5r, above64, ACTION_PROFILE_IPV6_LINK_LOCAL_SRCf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_5r, above64, ACTION_PROFILE_IPV6_SITE_LOCAL_DSTf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_5r, above64, ACTION_PROFILE_IPV6_HOP_BY_HOPf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_5r, above64, ACTION_PROFILE_IPV6_LINK_LOCAL_DSTf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_5r, above64, ACTION_PROFILE_IPV6_IPV4_MAPPED_DSTf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_5r, above64, ACTION_PROFILE_IPV6_SITE_LOCAL_SRCf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_5r, above64, ACTION_PROFILE_IPV6_IPV4_COMPATIBLE_DSTf, 8);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_ACTION_PROFILE_5r, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_ACTION_PROFILE_1r */
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_1r, above64, ACTION_PROFILE_RQP_DISCARDf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_1r, above64, ACTION_PROFILE_INVALID_OTMf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_1r, above64, ACTION_PROFILE_UNACCEPTABLE_FRAME_TYPEf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_1r, above64, ACTION_PROFILE_LAG_MULTICASTf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_1r, above64, ACTION_PROFILE_BOUNCE_BACKf, 8);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_ACTION_PROFILE_1r, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_ACTION_PROFILE_2r */
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_2r, above64, ACTION_PROFILE_UNKNOWN_DAf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_2r, above64, ACTION_PROFILE_SPLIT_HORIZONf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_2r, above64, ACTION_PROFILE_SRC_EQUAL_DESTf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_2r, above64, ACTION_PROFILE_TTL_SCOPEf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_2r, above64, ACTION_PROFILE_TRILL_TTL_ZEROf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_2r, above64, ACTION_PROFILE_TRILL_SAME_INTERFACEf, 8);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_ACTION_PROFILE_2r, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_ACTION_PROFILE_3r */
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_3r, above64, ACTION_PROFILE_IPV4_HEADER_LENGTH_ERRORf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_3r, above64, ACTION_PROFILE_IPV4_OPTIONSf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_3r, above64, ACTION_PROFILE_IPV4_TOTAL_LENGTH_ERRORf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_3r, above64, ACTION_PROFILE_IPV4_CHECKSUM_ERRORf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_3r, above64, ACTION_PROFILE_IP_VERSION_ERRORf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_3r, above64, ACTION_PROFILE_CNM_INTERCEPTf, 8);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_ACTION_PROFILE_3r, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_ACTION_PROFILE_4r */
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_4r, above64, ACTION_PROFILE_IPV6_LOOPBACKf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_4r, above64, ACTION_PROFILE_IP_TTL_EQUALS_ZEROf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_4r, above64, ACTION_PROFILE_IPV4_DIP_EQUALS_ZEROf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_4r, above64, ACTION_PROFILE_IPV4_SIP_IS_MCf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_4r, above64, ACTION_PROFILE_IPV6_UNSPECIFIED_DSTf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_4r, above64, ACTION_PROFILE_IPV4_SIP_EQUALS_DIPf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_4r, above64, ACTION_PROFILE_IPV6_UNSPECIFIED_SRCf, 8);
    soc_reg_above_64_field32_set(unit, EGQ_ACTION_PROFILE_4r, above64, ACTION_PROFILE_IPV6_SIP_IS_MCf, 8);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_ACTION_PROFILE_4r, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r */
    soc_reg_above_64_field32_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, above64, CFG_JBUG_FIX_7_DISABLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr */
    soc_reg_above_64_field32_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, above64, SHAPER_PACKET_RATE_CONSTf, 0x80);
    soc_reg_above_64_field32_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, above64, IGNORE_FCf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_GENERAL_RQP_DEBUG_CONFIGr */
    soc_reg_above_64_field32_set(unit, EGQ_GENERAL_RQP_DEBUG_CONFIGr, above64, TDM_PACKET_SIZE_FILTER_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_GENERAL_RQP_DEBUG_CONFIGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_IFC_IS_CHANNELIZEDr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_IFC_IS_CHANNELIZEDr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_KEYB_DATA_BASE_PROFILEr */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0xffffffff; field_a64[1]=0xffff;
    soc_reg_above_64_field_set(unit, EGQ_KEYB_DATA_BASE_PROFILEr, above64, KEY_B_DATA_BASE_PROFILEf, field_a64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_KEYB_DATA_BASE_PROFILEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_MC_PRIORITY_LOOKUP_TABLEr */
    soc_reg_above_64_field32_set(unit, EGQ_MC_PRIORITY_LOOKUP_TABLEr, above64, MC_PRIORITYf, 0xffffffff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_MC_PRIORITY_LOOKUP_TABLEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_WDRR_PACKET_SIZEr */
    soc_reg_above_64_field32_set(unit, EGQ_WDRR_PACKET_SIZEr, above64, WDRR_PKT_SIZEf, 0x501);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_WDRR_PACKET_SIZEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0xffffffff; field_a64[1]=0xffffffff; field_a64[2]=0xffffffff;
    soc_reg_above_64_field_set(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, above64, NIF_NO_FRAG_Hf, field_a64);
    soc_reg_above_64_field32_set(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, above64, OLP_NO_FRAGf, 1);
    soc_reg_above_64_field32_set(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, above64, CPU_NO_FRAGf, 1);
    soc_reg_above_64_field32_set(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, above64, NIF_NO_FRAG_Lf, 0x4068c06);
    soc_reg_above_64_field32_set(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, above64, RCY_NO_FRAGf, 1);
    soc_reg_above_64_field32_set(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, above64, OAM_NO_FRAGf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr */
    soc_reg_above_64_field32_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, above64, CFG_LINK_P_16_FIXED_LATENCY_SETTINGf, 0xa);
    soc_reg_above_64_field32_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, above64, CFG_NATIVE_LINK_P_9_FIXED_LATENCY_SETTINGf, 9);
    soc_reg_above_64_field32_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, above64, CFG_NATIVE_LINK_P_16_FIXED_LATENCY_SETTINGf, 6);
    soc_reg_above_64_field32_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, above64, CFG_LINK_P_9_FIXED_LATENCY_SETTINGf, 9);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EPNI_CFG_LATENCY_ACCURACYr */
    soc_reg_above_64_field32_set(unit, EPNI_CFG_LATENCY_ACCURACYr, above64, CFG_LATENCY_ACCURACYf, 5);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_CFG_LATENCY_ACCURACYr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EPNI_REG_0303r */
    soc_reg_above_64_field32_set(unit, EPNI_REG_0303r, above64, FIELD_11_11f, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_REG_0303r, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr */
    soc_reg_above_64_field32_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, above64, MAP_FLEXIBLE_UDH_0_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, above64, MAP_FLEXIBLE_UDH_3_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, above64, MAP_FLEXIBLE_UDH_1_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, above64, MAP_FLEXIBLE_UDH_2_TYPE_TO_SIZEf, 0x8d1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_STAMPING_USR_DEF_OUTLIF_ENABLEr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_STAMPING_USR_DEF_OUTLIF_ENABLEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_STAMPING_FTMH_OUTLIF_ENABLEr */
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FTMH_OUTLIF_ENABLEr, above64, STAMP_FTMH_OUTLIF_ENf, 0xfd);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_STAMPING_FTMH_OUTLIF_ENABLEr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PPH_RES_EN_TERMr */
    soc_reg_above_64_field32_set(unit, ITE_PPH_RES_EN_TERMr, above64, PPH_RES_EN_TERM_MASKf, 3);
    soc_reg_above_64_field32_set(unit, ITE_PPH_RES_EN_TERMr, above64, PPH_RES_EN_TERMf, 3);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PPH_RES_EN_TERMr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PACKET_LATENCY_MEASUREMENTr[7] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, REG_PORT_ANY, 7, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PACKET_LATENCY_MEASUREMENTr[6] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, REG_PORT_ANY, 6, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PACKET_LATENCY_MEASUREMENTr[4] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, REG_PORT_ANY, 4, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PACKET_LATENCY_MEASUREMENTr[3] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PACKET_LATENCY_MEASUREMENTr[2] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PACKET_LATENCY_MEASUREMENTr[1] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PACKET_LATENCY_MEASUREMENTr[0] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_MAPPING_QUEUE_TYPE_TO_MIRROR_PACKETr */
    soc_reg_above_64_field32_set(unit, ITE_MAPPING_QUEUE_TYPE_TO_MIRROR_PACKETr, above64, MIRROR_BITMAP_TABLEf, 4);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAPPING_QUEUE_TYPE_TO_MIRROR_PACKETr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_MAPPING_FWD_CODE_TO_TERM_ENr */
    soc_reg_above_64_field32_set(unit, ITE_MAPPING_FWD_CODE_TO_TERM_ENr, above64, EN_TERM_UP_TO_ETHf, 0x14);
    soc_reg_above_64_field32_set(unit, ITE_MAPPING_FWD_CODE_TO_TERM_ENr, above64, EN_TERM_UP_TO_FWD_CODEf, 0xff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAPPING_FWD_CODE_TO_TERM_ENr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_MAP_UDH_TYPE_TO_LENGTHr[0] */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0xcc520c41; field_a64[1]=0xd62d4941; field_a64[2]=0x0c4183dc; field_a64[3]=0x4941cc52; field_a64[4]=0x83dcd62d; field_a64[5]=0xcc520c41;field_a64[6]=0xd62d4941; field_a64[7]=0x0c4183dc; field_a64[8]=0x4941cc52; field_a64[9]=0x83dcd62d;
    soc_reg_above_64_field_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, above64, TYPE_TO_LENGTHf, field_a64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_MAP_UDH_TYPE_TO_LENGTHr[1] */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0xcc520c41; field_a64[1]=0xd62d4941; field_a64[2]=0x0c4183dc; field_a64[3]=0x4941cc52; field_a64[4]=0x83dcd62d; field_a64[5]=0xcc520c41;field_a64[6]=0xd62d4941; field_a64[7]=0x0c4183dc; field_a64[8]=0x4941cc52; field_a64[9]=0x83dcd62d;
    soc_reg_above_64_field_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, above64, TYPE_TO_LENGTHf, field_a64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_MAP_UDH_TYPE_TO_LENGTHr[2] */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0xcc520c41; field_a64[1]=0xd62d4941; field_a64[2]=0x0c4183dc; field_a64[3]=0x4941cc52; field_a64[4]=0x83dcd62d; field_a64[5]=0xcc520c41;field_a64[6]=0xd62d4941; field_a64[7]=0x0c4183dc; field_a64[8]=0x4941cc52; field_a64[9]=0x83dcd62d;
    soc_reg_above_64_field_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, above64, TYPE_TO_LENGTHf, field_a64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_MAP_UDH_TYPE_TO_LENGTHr[3] */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0xcc520c41; field_a64[1]=0xd62d4941; field_a64[2]=0x0c4183dc; field_a64[3]=0x4941cc52; field_a64[4]=0x83dcd62d; field_a64[5]=0xcc520c41;field_a64[6]=0xd62d4941; field_a64[7]=0x0c4183dc; field_a64[8]=0x4941cc52; field_a64[9]=0x83dcd62d;
    soc_reg_above_64_field_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, above64, TYPE_TO_LENGTHf, field_a64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_MAPPING_QUEUE_TYPE_TO_SNOOP_PACKETr */
    soc_reg_above_64_field32_set(unit, ITE_MAPPING_QUEUE_TYPE_TO_SNOOP_PACKETr, above64, SNOOP_BITMAP_TABLEf, 2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAPPING_QUEUE_TYPE_TO_SNOOP_PACKETr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PACKET_LATENCY_FORMATr */
    soc_reg_above_64_field32_set(unit, ITE_PACKET_LATENCY_FORMATr, above64, LATENCY_FORMATf, 2);
    soc_reg_above_64_field32_set(unit, ITE_PACKET_LATENCY_FORMATr, above64, LATENCY_ACCURACYf, 5);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PACKET_LATENCY_FORMATr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_PACKET_LATENCY_MEASUREMENTr[5] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, REG_PORT_ANY, 5, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set ITE_STAMPING_FABRIC_HEADER_ENABLEr */
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, above64, STAMP_FWD_ACTIONf, 1);
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, above64, STAMP_LB_KEY_EXT_ENf, 1);
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, above64, STAMP_FWDACTION_TDMf, 1);
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, above64, STAMP_DP_ENf, 0xff);
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, above64, STAMP_FAP_PORTf, 1);
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, above64, STAMP_DSP_EXT_ENf, 1);
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, above64, SNP_STAMP_TRAP_CODEf, 1);
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, above64, STAMP_CNI_BITf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, above64));
    clear_entry(unit, entry, TXQ_PER_WFQ_CFGm);
    soc_mem_field32_set(unit, TXQ_PER_WFQ_CFGm, entry, WFQ_WEIGHT_0f, 0x3b);
    soc_mem_field32_set(unit, TXQ_PER_WFQ_CFGm, entry, WFQ_WEIGHT_1f, 0x3b);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, TXQ_PER_WFQ_CFGm, 0, 0, MEM_BLOCK_ALL, 0, 0+(4-1), entry));
    clear_entry(unit, entry, TXQ_PRIORITY_BITS_MAPPING_2_FDTm);
    soc_mem_field32_set(unit, TXQ_PRIORITY_BITS_MAPPING_2_FDTm, entry, PRIORITY_BITS_MAPPING_2_FDTf, 2);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, TXQ_PRIORITY_BITS_MAPPING_2_FDTm, 0, 0, MEM_BLOCK_ALL, 0, 0+(32-1), entry));
    clear_entry(unit, entry, TXQ_PRIORITY_BITS_MAPPING_2_FDTm);
    soc_mem_field32_set(unit, TXQ_PRIORITY_BITS_MAPPING_2_FDTm, entry, PRIORITY_BITS_MAPPING_2_FDTf, 1);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, TXQ_PRIORITY_BITS_MAPPING_2_FDTm, 0, 0, MEM_BLOCK_ALL, 32, 32+(32-1), entry));
    clear_entry(unit, entry, TXQ_PRIORITY_BITS_MAPPING_2_FDTm);
    soc_mem_field32_set(unit, TXQ_PRIORITY_BITS_MAPPING_2_FDTm, entry, PRIORITY_BITS_MAPPING_2_FDTf, 2);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, TXQ_PRIORITY_BITS_MAPPING_2_FDTm, 0, 0, MEM_BLOCK_ALL, 64, 64+(64-1), entry));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set TXQ_LOCAL_FIFO_CFGr */
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_EGQ_FC_THf, 0x48);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_PDQ_FC_TH_2f, 64);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_PDQ_FC_TH_2f, 102);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_SPR_FC_THf, 64);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_PDQ_FC_TH_1f, 76);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_EGQ_FC_THf, 0x73);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_SPR_FC_THf, 102);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_GEN_RCI_THf, 0x73);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_GEN_RCI_THf, 0x48);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_PDQ_FC_TH_1f, 48);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, TXQ_LOCAL_FIFO_CFGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set TXQ_TXQ_GENERAL_CONFIGURATIONr */
    soc_reg_above_64_field32_set(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, above64, DTQ_IS_MC_BMAPf, 7);
    soc_reg_above_64_field32_set(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, above64, DTQ_MODEf, 2);
    soc_reg_above_64_field32_set(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, above64, DTQ_DEQUEUE_WHEN_EOB_ON_QUEUEf, 0x2a);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, above64));

    clear_entry(unit, entry, PTS_PER_WFQ_CFGm);
    soc_mem_field32_set(unit, PTS_PER_WFQ_CFGm, entry, WFQ_WEIGHT_0f, 0x18);
    soc_mem_field32_set(unit, PTS_PER_WFQ_CFGm, entry, WFQ_WEIGHT_1f, 0x18);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, PTS_PER_WFQ_CFGm, 0, 0, MEM_BLOCK_ALL, 0, 0+(11-1), entry));
    clear_entry(unit, entry, PTS_PER_WFQ_MESH_CFGm);
    soc_mem_field32_set(unit, PTS_PER_WFQ_MESH_CFGm, entry, WFQ_WEIGHT_0f, 0x18);
    soc_mem_field32_set(unit, PTS_PER_WFQ_MESH_CFGm, entry, WFQ_WEIGHT_2f, 0x18);
    soc_mem_field32_set(unit, PTS_PER_WFQ_MESH_CFGm, entry, WFQ_WEIGHT_1f, 0x18);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, PTS_PER_WFQ_MESH_CFGm, 0, 0, MEM_BLOCK_ALL, 0, 0+(2-1), entry));

    SOC_REG_ABOVE_64_CLEAR(above64); /* Code added to override the default value of PTS_PDQ_OVTH_FC_MASKr */
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_FUC_HP_OVTH_MASKf, 0x40);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_FUC_LP_OVTH_MASKf, 0x41);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_FMC_HP_OVTH_MASKf, 0x140);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_FMC_LP_OVTH_MASKf, 0x555);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_LOC_HP_OVTH_MASKf, 4);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_LOC_LP_OVTH_MASKf, 5);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_TO_DRAM_HP_OVTH_MASKf, 1);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_TO_DRAM_LP_OVTH_MASKf, 1);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_FUC_HP_OVTH_MASKf, 0x40);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_FUC_LP_OVTH_MASKf, 0x42);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_FMC_HP_OVTH_MASKf, 0x280);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_FMC_LP_OVTH_MASKf, 0xaaa);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_LOC_HP_OVTH_MASKf, 8);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_LOC_LP_OVTH_MASKf, 0xa);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, PTS_PDQ_OVTH_FC_MASKr, REG_PORT_ANY, 0, above64));

    SOC_REG_ABOVE_64_CLEAR(above64); /* set DDP_DDP_GENERAL_CONFIGURATIONr */
    soc_reg_above_64_field32_set(unit, DDP_DDP_GENERAL_CONFIGURATIONr, above64, CNI_STAMP_MODEf, 7);
    soc_reg_above_64_field32_set(unit, DDP_DDP_GENERAL_CONFIGURATIONr, above64, FC_LP_DEQUEUE_WORDS_THf, 0xfa0);
    soc_reg_above_64_field32_set(unit, DDP_DDP_GENERAL_CONFIGURATIONr, above64, MIN_FABRIC_BURST_SIZEf, 0x10);
    soc_reg_above_64_field32_set(unit, DDP_DDP_GENERAL_CONFIGURATIONr, above64, PTS_FC_HP_THf, 0x8c);
    soc_reg_above_64_field32_set(unit, DDP_DDP_GENERAL_CONFIGURATIONr, above64, BDQ_IRDY_WORD_THf, 0x400);
    soc_reg_above_64_field32_set(unit, DDP_DDP_GENERAL_CONFIGURATIONr, above64, PTS_FC_LP_THf, 0x78);
    soc_reg_above_64_field32_set(unit, DDP_DDP_GENERAL_CONFIGURATIONr, above64, PTS_SPR_FC_THf, 0x64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DDP_DDP_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set DDP_PACKET_LATENCY_FORMATr */
    soc_reg_above_64_field32_set(unit, DDP_PACKET_LATENCY_FORMATr, above64, LATENCY_FORMATf, 2);
    soc_reg_above_64_field32_set(unit, DDP_PACKET_LATENCY_FORMATr, above64, LATENCY_ACCURACYf, 5);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DDP_PACKET_LATENCY_FORMATr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set DDP_FBC_CONTROLr */
    soc_reg_above_64_field32_set(unit, DDP_FBC_CONTROLr, above64, FBC_INITf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DDP_FBC_CONTROLr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set DDP_FBC_CONTROLr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DDP_FBC_CONTROLr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FDR_LINK_LEVEL_FLOW_CONTROLr */
    soc_reg_above_64_field32_set(unit, FDR_LINK_LEVEL_FLOW_CONTROLr, above64, FIELD_18_26f, 0xbf);
    soc_reg_above_64_field32_set(unit, FDR_LINK_LEVEL_FLOW_CONTROLr, above64, IFM_FULL_LEVEL_0f, 0x5f);
    soc_reg_above_64_field32_set(unit, FDR_LINK_LEVEL_FLOW_CONTROLr, above64, LNK_LVL_FC_THf, 0x37);
    soc_reg_above_64_field32_set(unit, FDR_LINK_LEVEL_FLOW_CONTROLr, above64, FIELD_8_8f, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDR_LINK_LEVEL_FLOW_CONTROLr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FDR_FDR_ENABLERS_REGISTER_2r */
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, above64, PCP_ENABLEDf, 1);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, above64, FIELD_1_3f, 3);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, above64, FIELD_0_0f, 1);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, above64, MC_PRI_2_PIPEf, 1);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, above64, PARALLEL_DATA_PATHf, 2);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, above64, FIELD_25_31f, 3);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, above64, MC_PRI_0_PIPEf, 2);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, above64, MC_PRI_1_PIPEf, 2);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, above64, MC_PRI_3_PIPEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FDR_FDR_ENABLERS_REGISTER_1r */
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, above64, FIELD_4_11f, 0xff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FDT_LOAD_BALANCING_CONFIGURATIONr */
    soc_reg_above_64_field32_set(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, above64, IPT_WFQ_ENf, 1);
    soc_reg_above_64_field32_set(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, above64, FIELD_3_3f, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FDT_FDT_ENABLER_REGISTERr */
    soc_reg_above_64_field32_set(unit, FDT_FDT_ENABLER_REGISTERr, above64, PCP_ENABLEDf, 1);
    soc_reg_above_64_field32_set(unit, FDT_FDT_ENABLER_REGISTERr, above64, FIELD_24_24f, 1);
    soc_reg_above_64_field32_set(unit, FDT_FDT_ENABLER_REGISTERr, above64, PARALLEL_DATA_PATHf, 2);
    soc_reg_above_64_field32_set(unit, FDT_FDT_ENABLER_REGISTERr, above64, FIELD_25_25f, 1);
    soc_reg_above_64_field32_set(unit, FDT_FDT_ENABLER_REGISTERr, above64, FIELD_2_2f, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FDT_FDT_CONTEXTS_INTERLEAVINGr */
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, above64, INTERLEAVING_IPT_TO_IPT_2_ENf, 0x3b);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, above64, INTERLEAVING_IPT_TO_IRE_ENf, 0x3f);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, above64, INTERLEAVING_IPT_TO_IPT_0_ENf, 0x3e);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, above64, INTERLEAVING_IPT_TO_IPT_4_ENf, 0x2f);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, above64, INTERLEAVING_IPT_TO_IPT_5_ENf, 0x1f);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, above64, INTERLEAVING_IPT_TO_IPT_1_ENf, 0x3d);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, above64, INTERLEAVING_IPT_TO_IPT_3_ENf, 0x37);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FDT_FDT_CONTEXTS_FABRIC_MAPPINGr */
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_FABRIC_MAPPINGr, above64, FABRIC_MAPPING_MODEf, 1);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_FABRIC_MAPPINGr, above64, FABRIC_MAPPING_IPT_0_ENf, 3);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_FABRIC_MAPPINGr, above64, FABRIC_MAPPING_IPT_1_ENf, 2);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_FABRIC_MAPPINGr, above64, FABRIC_MAPPING_IRE_ENf, 4);
    soc_reg_above_64_field32_set(unit, FDT_FDT_CONTEXTS_FABRIC_MAPPINGr, above64, FABRIC_MAPPING_PKT_BOUNDRYf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDT_FDT_CONTEXTS_FABRIC_MAPPINGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FDT_REG_016Cr */
    soc_reg_above_64_field32_set(unit, FDT_REG_016Cr, above64, FIELD_24_47f, 0x48);
    soc_reg_above_64_field32_set(unit, FDT_REG_016Cr, above64, FIELD_48_71f, 0x49);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDT_REG_016Cr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FDT_TDM_CONFIGURATIONr */
    soc_reg_above_64_field32_set(unit, FDT_TDM_CONFIGURATIONr, above64, IRE_TDM_PRIORITYf, 3);
    soc_reg_above_64_field32_set(unit, FDT_TDM_CONFIGURATIONr, above64, TDM_SOURCE_FAP_IDf, 0x22);
    soc_reg_above_64_field32_set(unit, FDT_TDM_CONFIGURATIONr, above64, IRE_CPU_PRIORITYf, 3);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDT_TDM_CONFIGURATIONr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FCR_EFMS_CONFIGURATIONr */
    soc_reg_above_64_field32_set(unit, FCR_EFMS_CONFIGURATIONr, above64, EFMS_DBG_SRC_PIPE_MASKf, 0x7ff);
    soc_reg_above_64_field32_set(unit, FCR_EFMS_CONFIGURATIONr, above64, EFMS_STATUS_BYPASSf, 0x88);
    soc_reg_above_64_field32_set(unit, FCR_EFMS_CONFIGURATIONr, above64, EFMS_DBG_FILTER_MASKf, 0x3ffff);
    soc_reg_above_64_field32_set(unit, FCR_EFMS_CONFIGURATIONr, above64, EFMS_AGING_TIMERf, 3);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FCR_EFMS_CONFIGURATIONr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set FCT_FCT_ENABLER_REGISTERr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FCT_FCT_ENABLER_REGISTERr, REG_PORT_ANY, 0, above64));

exit:
    SOCDNX_FUNC_RETURN;
}

/* QAX register/memory settings as overrides part 6, to be later moved to the correct functions and removed */
STATIC int soc_qax_init_overrides_6(int unit)
{
#ifndef QAX_MASK_NIF_OVERRIDES
    soc_reg_above_64_val_t field_a64;
#endif
    soc_reg_above_64_val_t above64; 
    uint32 entry[20];
    int i;

#ifndef QAX_MASK_NIF_OVERRIDES
    int clp0 = SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0));
    int clp1 = SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1));
    int clp2 = SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 2));
    int clp3 = SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 3));
    int xlp0 = SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0));
    int xlp1 = SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1));
    int xlp2 = SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 2));
    int xlp3 = SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 3));
#endif /* QAX_MASK_NIF_OVERRIDES */

    SOCDNX_INIT_FUNC_DEFS;
    clear_entry(unit, entry, IHB_PFC_INFOm);
    soc_mem_field32_set(unit, IHB_PFC_INFOm, entry, L_4_LOCATIONf, 1);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_PFC_INFOm, 0, MEM_BLOCK_ALL, 32, entry));
    clear_entry(unit, entry, IHB_PFC_INFOm);
    soc_mem_field32_set(unit, IHB_PFC_INFOm, entry, L_4_LOCATIONf, 1);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IHB_PFC_INFOm, 0, 0, MEM_BLOCK_ALL, 48, 48+(16-1), entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 1);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 1, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 2);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 2, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 3);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 3, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 4);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 4, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 5);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 5, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 6);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 6, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 7);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 7, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 8);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 8, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 9);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 9, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 0xa);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 10, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 0xb);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 11, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 12, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 0xd);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 13, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 0xe);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 14, entry));
    clear_entry(unit, entry, IHB_SNOOP_ACTIONm);
    soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, entry, SNOOP_ACTIONf, 0xf);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, IHB_SNOOP_ACTIONm, 0, MEM_BLOCK_ALL, 15, entry));
    clear_entry(unit, entry, IHB_SNP_ACT_PROFILEm);
    soc_mem_field32_set(unit, IHB_SNP_ACT_PROFILEm, entry, SAMPLING_PROBABILITYf, 0xffffff);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IHB_SNP_ACT_PROFILEm, 0, 0, MEM_BLOCK_ALL, 0, 0+(16-1), entry));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set IHB_LAG_LB_KEY_CFGr */
    soc_reg_above_64_field32_set(unit, IHB_LAG_LB_KEY_CFGr, above64, LAG_LB_HASH_INDEX_1f, 3);
    soc_reg_above_64_field32_set(unit, IHB_LAG_LB_KEY_CFGr, above64, LAG_LB_KEY_SEEDf, 1);
    soc_reg_above_64_field32_set(unit, IHB_LAG_LB_KEY_CFGr, above64, LAG_LB_HASH_INDEXf, 3);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IHB_LAG_LB_KEY_CFGr, REG_PORT_ANY, 0, above64));
    
#ifndef QAX_MASK_NIF_OVERRIDES
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIL_PM_STRAP_LED_OUTPUT_DELAYr */
    soc_reg_above_64_field32_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, above64, PML_3_STRAP_LED_OUTPUT_DELAYf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_TX_QMLF_CONFIGr[1] */
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_3_QMLF_Nf, 1);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_START_TX_THRESHOLD_QMLF_Nf, 2);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_2_QMLF_Nf, 1);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_1_QMLF_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_TX_QMLF_CONFIGr, REG_PORT_ANY, 1, above64));
#endif /* QAX_MASK_NIF_OVERRIDES */

    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_SYNC_ETH_SQUELCH_DIS_TH_REGr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_SYNC_ETH_SQUELCH_DIS_TH_REGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_SYNC_ETH_CFGr[1] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_SYNC_ETH_CFGr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_SYNC_ETH_CFGr[0] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_SYNC_ETH_CFGr, REG_PORT_ANY, 0, above64));

#ifndef QAX_MASK_NIF_OVERRIDES
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_REQ_PIPE_0_LOW_ENr */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0xfffff000; field_a64[1]=0xfffff000; field_a64[2]=0xfffff000; field_a64[3]=0xfffff000;
    soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, above64, RX_REQ_PIPE_0_LOW_ENf, field_a64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_PORTS_SRSTNr */
    soc_reg_above_64_field32_set(unit, NBIH_RX_PORTS_SRSTNr, above64, RX_PORTS_SRSTNf, 0x1f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_PORTS_SRSTNr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_ILKN_MULTIPLE_USE_BITSr[1] */
    soc_reg_above_64_field32_set(unit, NBIH_ILKN_MULTIPLE_USE_BITSr, above64, ILKN_RX_N_MULTIPLE_USE_BITS_TO_CFC_MASKf, 0xff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_ILKN_MULTIPLE_USE_BITSr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_ILKN_MULTIPLE_USE_BITSr[0] */
    soc_reg_above_64_field32_set(unit, NBIH_ILKN_MULTIPLE_USE_BITSr, above64, ILKN_RX_N_MULTIPLE_USE_BITS_TO_CFC_MASKf, 0xff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_ILKN_MULTIPLE_USE_BITSr, REG_PORT_ANY, 0, above64));
#endif /* QAX_MASK_NIF_OVERRIDES */

    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_TX_GEN_LLFC_FROM_QMLF_MASKr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_TX_GEN_LLFC_FROM_QMLF_MASKr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_TX_GEN_LLFC_FROM_CFC_MASKr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_TX_GEN_LLFC_FROM_CFC_MASKr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_TX_BITMAP_TYPE_Cr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_TX_BITMAP_TYPE_Cr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_TX_BITMAP_TYPE_Br */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_TX_BITMAP_TYPE_Br, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_RX_BITMAP_TYPE_Br */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_RX_BITMAP_TYPE_Br, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_RX_BITMAP_TYPE_Ar */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_RX_BITMAP_TYPE_Ar, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[8] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 8, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[5] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 5, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[4] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 4, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_MASK_FC_WHEN_LINK_FAILr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_MASK_FC_WHEN_LINK_FAILr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKN_TX_GEN_CHFC_FROM_CFC_MASKr[1] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKN_TX_GEN_CHFC_FROM_CFC_MASKr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKN_RX_CHFC_TO_CFC_MASKr[3] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKN_RX_CHFC_TO_CFC_MASKr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKN_RX_CHFC_TO_CFC_MASKr[2] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKN_RX_CHFC_TO_CFC_MASKr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKN_RX_CHFC_TO_CFC_MASKr[0] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKN_RX_CHFC_TO_CFC_MASKr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[9] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 9, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[8] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 8, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[7] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 7, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[4] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 4, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[47] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 47, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[45] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 45, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[43] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 43, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[42] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 42, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[40] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 40, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[37] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 37, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[35] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 35, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[30] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 30, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[29] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 29, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[28] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 28, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[21] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 21, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[17] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 17, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[15] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 15, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[13] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 13, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[12] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 12, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[10] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 10, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[0] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[11] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 11, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[14] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 14, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[16] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 16, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[18] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 18, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[19] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 19, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[1] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[20] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 20, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[22] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 22, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[23] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 23, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[24] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 24, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[25] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 25, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[26] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 26, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[27] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 27, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[2] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[31] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 31, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[32] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 32, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[33] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 33, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[34] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 34, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[36] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 36, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[38] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 38, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[39] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 39, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[3] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[41] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 41, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[44] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 44, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[46] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 46, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[5] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 5, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_GEN_SAFC_REFRESH_TIMERr[6] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_GEN_SAFC_REFRESH_TIMERr, REG_PORT_ANY, 6, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKNr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKNr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKN_RX_CHFC_TO_CFC_MASKr[1] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKN_RX_CHFC_TO_CFC_MASKr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKN_RX_CHFC_CAL_LENr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKN_RX_CHFC_CAL_LENr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKN_TX_GEN_CHFC_FROM_CFC_MASKr[0] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKN_TX_GEN_CHFC_FROM_CFC_MASKr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKN_TX_GEN_CHFC_FROM_CFC_MASKr[2] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKN_TX_GEN_CHFC_FROM_CFC_MASKr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_ILKN_TX_GEN_CHFC_FROM_CFC_MASKr[3] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_ILKN_TX_GEN_CHFC_FROM_CFC_MASKr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_LLFC_STOP_TX_FROM_CFC_MASKr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_LLFC_STOP_TX_FROM_CFC_MASKr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_CFGr */
    soc_reg_above_64_field32_set(unit, NBIH_FC_PFC_CFGr, above64, INSERT_TWO_IDLES_IN_TX_CALf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_CFGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[0] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[10] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 10, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[11] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 11, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[1] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 1, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[2] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[3] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[6] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 6, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[7] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 7, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_QMLF_MASKr[9] */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_QMLF_MASKr, REG_PORT_ANY, 9, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_TX_BITMAP_TYPE_Ar */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_TX_BITMAP_TYPE_Ar, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_PFC_TX_BITMAP_TYPE_Dr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_PFC_TX_BITMAP_TYPE_Dr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_FC_TX_LOW_PASS_FILTERr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_FC_TX_LOW_PASS_FILTERr, REG_PORT_ANY, 0, above64));

#ifndef QAX_MASK_NIF_OVERRIDES
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_ILKN_MULTIPLE_USE_BITSr[2] */
    soc_reg_above_64_field32_set(unit, NBIH_ILKN_MULTIPLE_USE_BITSr, above64, ILKN_RX_N_MULTIPLE_USE_BITS_TO_CFC_MASKf, 0xff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_ILKN_MULTIPLE_USE_BITSr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_ILKN_MULTIPLE_USE_BITSr[3] */
    soc_reg_above_64_field32_set(unit, NBIH_ILKN_MULTIPLE_USE_BITSr, above64, ILKN_RX_N_MULTIPLE_USE_BITS_TO_CFC_MASKf, 0xff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_ILKN_MULTIPLE_USE_BITSr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_RX_REQ_PIPE_0_HIGH_ENr */
    soc_reg_above_64_field32_set(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, above64, RX_REQ_PIPE_0_HIGH_ENf, 0xff3);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, REG_PORT_ANY, 0, above64));
#endif /* QAX_MASK_NIF_OVERRIDES */

    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_SYNC_ETH_SQUELCH_EN_TH_REGr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_SYNC_ETH_SQUELCH_EN_TH_REGr, REG_PORT_ANY, 0, above64));

#ifndef QAX_MASK_NIF_OVERRIDES
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_TX_PORTS_SRSTNr */
    soc_reg_above_64_field32_set(unit, NBIH_TX_PORTS_SRSTNr, above64, TX_PORTS_SRSTNf, 0x1f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_TX_PORTS_SRSTNr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_TX_QMLF_CONFIGr[2] */
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_3_QMLF_Nf, 1);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_START_TX_THRESHOLD_QMLF_Nf, 2);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_PORT_MODE_QMLF_Nf, 2);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_2_QMLF_Nf, 1);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_1_QMLF_Nf, 1);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_0_QMLF_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_TX_QMLF_CONFIGr, REG_PORT_ANY, 2, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIH_TX_QMLF_CONFIGr[3] */
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_3_QMLF_Nf, 1);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_START_TX_THRESHOLD_QMLF_Nf, 2);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_PORT_MODE_QMLF_Nf, 2);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_2_QMLF_Nf, 1);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_1_QMLF_Nf, 1);
    soc_reg_above_64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, above64, TX_STOP_DATA_TO_PORT_MACRO_MLF_0_QMLF_Nf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIH_TX_QMLF_CONFIGr, REG_PORT_ANY, 3, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIL_RX_PORTS_SRSTNr */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0x11111111; field_a64[1]=0xf1111;
    soc_reg_above_64_field_set(unit, NBIL_RX_PORTS_SRSTNr, above64, RX_PORTS_SRSTNf, field_a64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIL_RX_PORTS_SRSTNr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set NBIL_TX_PORTS_SRSTNr */
    SOC_REG_ABOVE_64_CLEAR(field_a64);
    field_a64[0]=0x11111111; field_a64[1]=0xf1111;
    soc_reg_above_64_field_set(unit, NBIL_TX_PORTS_SRSTNr, above64, TX_PORTS_SRSTNf, field_a64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, NBIL_TX_PORTS_SRSTNr, REG_PORT_ANY, 0, above64));
#ifdef PLISIM
    if (!SAL_BOOT_PLISIM) { /* Do not run on pcid simulator not properly supporting port blocks */
#endif
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_POWER_SAVEr.CLP2 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_POWER_SAVEr, clp0, 0, above64)); /* power on CLP0 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_POWER_SAVEr, clp1, 0, above64)); /* power on CLP1 */
        soc_reg_above_64_field32_set(unit, CLPORT_POWER_SAVEr, above64, XPORT_CORE0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_POWER_SAVEr, clp2, 0, above64)); /* power off CLP2 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_POWER_SAVEr, clp3, 0, above64)); /* power off CLP3 */
        sal_usleep(50000);
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MODE_REGr.CLP1 */
        soc_reg_above_64_field32_set(unit, CLPORT_MODE_REGr, above64, XPORT0_PHY_PORT_MODEf, 4);
        soc_reg_above_64_field32_set(unit, CLPORT_MODE_REGr, above64, FIELD_9_9f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_MODE_REGr, above64, FIELD_10_10f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_MODE_REGr, above64, XPORT0_CORE_PORT_MODEf, 4);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MODE_REGr, clp1, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* make CLP0 enter soft reset */
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_SOFT_RESETr, clp0, 0, above64)); /* Enter CLP0 into soft reset */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_SOFT_RESETr, clp1, 0, above64)); /* Enter CLP1 into soft reset */
        sal_usleep(50000);
        SOCDNX_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, clp0, 0)); /* take CLP0 out of reset */
        SOCDNX_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, clp1, 0)); /* take CLP1 out of reset */
        sal_usleep(50000);
        SOCDNX_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, clp0, 0)); /* take CLP0 out of soft reset */
        SOCDNX_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, clp1, 0)); /* take CLP1 out of soft reset */
        sal_usleep(50000);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_SOFT_RESETr, 0, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_ENABLE_REGr.CLP0 */
        soc_reg_above_64_field32_set(unit, CLPORT_ENABLE_REGr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_ENABLE_REGr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_ENABLE_REGr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_ENABLE_REGr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_ENABLE_REGr, clp0, 0, above64)); /* enable sub ports of CLP0 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_ENABLE_REGr, clp1, 0, above64)); /* enable sub ports of CLP1 */
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MAC_RSV_MASKr.xe4 */
        soc_reg_above_64_field32_set(unit, CLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MAC_RSV_MASKr, 4, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MAC_RSV_MASKr.xe3 */
        soc_reg_above_64_field32_set(unit, CLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MAC_RSV_MASKr, 3, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MAC_RSV_MASKr.xe2 */
        soc_reg_above_64_field32_set(unit, CLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MAC_RSV_MASKr, 2, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MAC_RSV_MASKr.xe1 */
        soc_reg_above_64_field32_set(unit, CLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MAC_RSV_MASKr, 1, 0, above64));
#if 0
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_SOFT_RESETr.CLP1 */
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_SOFT_RESETr, clp1, 0, above64));
        sal_usleep(50000);
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MAC_CONTROLr.CLP1 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MAC_CONTROLr, clp1, 0, above64));
        sal_usleep(50000);
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_SOFT_RESETr.CLP1 */
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, CLPORT_SOFT_RESETr, above64, PORT1f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_SOFT_RESETr, clp1, 0, above64));
        sal_usleep(10000);
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_ENABLE_REGr.CLP1 */
        soc_reg_above_64_field32_set(unit, CLPORT_ENABLE_REGr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_ENABLE_REGr, clp1, 0, above64));
#endif
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MAC_RSV_MASKr.xe5 */
        soc_reg_above_64_field32_set(unit, CLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MAC_RSV_MASKr, 5, 0, above64));
#ifdef PLISIM
    } /* Do not run on pcid simulator not properly supporting port blocks */
#endif
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CHIP_CONFIGr.PMQ0 */
    soc_reg_above_64_field32_set(unit, CHIP_CONFIGr, above64, IP_TDMf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CHIP_CONFIGr, 0, 0, above64));
#ifdef PLISIM
    if (!SAL_BOOT_PLISIM) { /* Do not run on pcid simulator not properly supporting port blocks */
#endif
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_SOFT_RESETr.XLP0 */
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_SOFT_RESETr, xlp0, 0, above64));
#ifdef PLISIM
    } /* Do not run on pcid simulator not properly supporting port blocks */
#endif
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CHIP_SWRSTr.PMQ0 */
    soc_reg_above_64_field32_set(unit, CHIP_SWRSTr, above64, SOFT_RESET_QSGMII_PCSf, 1);
    soc_reg_above_64_field32_set(unit, CHIP_SWRSTr, above64, SOFT_RESET_GPORT0f, 1);
    soc_reg_above_64_field32_set(unit, CHIP_SWRSTr, above64, SOFT_RESET_GPORT1f, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CHIP_SWRSTr, 0, 0, above64));
#ifdef PLISIM
    if (!SAL_BOOT_PLISIM) { /* Do not run on pcid simulator not properly supporting port blocks */
#endif
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_SOFT_RESETr.XLP0 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_SOFT_RESETr, xlp0, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_CONTROLr.XLP0 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_CONTROLr, xlp0, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_ENABLE_REGr.XLP0 */
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_ENABLE_REGr, xlp0, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe20 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 20, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe18 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 18, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe17 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 17, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe19 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 19, 0, above64));
#ifdef PLISIM
    } /* Do not run on pcid simulator not properly supporting port blocks */
#endif
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CHIP_CONFIGr.PMQ1 */
    soc_reg_above_64_field32_set(unit, CHIP_CONFIGr, above64, IP_TDMf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CHIP_CONFIGr, 1, 0, above64));
#ifdef PLISIM
    if (!SAL_BOOT_PLISIM) { /* Do not run on pcid simulator not properly supporting port blocks */
#endif
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_SOFT_RESETr.XLP1 */
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_SOFT_RESETr, xlp1, 0, above64));
#ifdef PLISIM
    } /* Do not run on pcid simulator not properly supporting port blocks */
#endif
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CHIP_SWRSTr.PMQ1 */
    soc_reg_above_64_field32_set(unit, CHIP_SWRSTr, above64, SOFT_RESET_QSGMII_PCSf, 1);
    soc_reg_above_64_field32_set(unit, CHIP_SWRSTr, above64, SOFT_RESET_GPORT0f, 1);
    soc_reg_above_64_field32_set(unit, CHIP_SWRSTr, above64, SOFT_RESET_GPORT1f, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CHIP_SWRSTr, 1, 0, above64));
#ifdef PLISIM
    if (!SAL_BOOT_PLISIM) { /* Do not run on pcid simulator not properly supporting port blocks */
#endif
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_SOFT_RESETr.XLP1 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_SOFT_RESETr, xlp1, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_CONTROLr.XLP1 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_CONTROLr, xlp1, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_ENABLE_REGr.XLP1 */
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_ENABLE_REGr, xlp1, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe23 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 23, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe21 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 21, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe22 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 22, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe24 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 24, 0, above64));
#ifdef PLISIM
    } /* Do not run on pcid simulator not properly supporting port blocks */
#endif
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CHIP_CONFIGr.PMQ2 */
    soc_reg_above_64_field32_set(unit, CHIP_CONFIGr, above64, IP_TDMf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CHIP_CONFIGr, 2, 0, above64));
#ifdef PLISIM
    if (!SAL_BOOT_PLISIM) { /* Do not run on pcid simulator not properly supporting port blocks */
#endif
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_SOFT_RESETr.XLP2 */
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_SOFT_RESETr, xlp2, 0, above64));
#ifdef PLISIM
    } /* Do not run on pcid simulator not properly supporting port blocks */
#endif
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CHIP_SWRSTr.PMQ2 */
    soc_reg_above_64_field32_set(unit, CHIP_SWRSTr, above64, SOFT_RESET_QSGMII_PCSf, 1);
    soc_reg_above_64_field32_set(unit, CHIP_SWRSTr, above64, SOFT_RESET_GPORT0f, 1);
    soc_reg_above_64_field32_set(unit, CHIP_SWRSTr, above64, SOFT_RESET_GPORT1f, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CHIP_SWRSTr, 2, 0, above64));
#ifdef PLISIM
    if (!SAL_BOOT_PLISIM) { /* Do not run on pcid simulator not properly supporting port blocks */
#endif
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_CONTROLr.XLP2 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_CONTROLr, xlp2, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_SOFT_RESETr.XLP2 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_SOFT_RESETr, xlp2, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_ENABLE_REGr.XLP2 */
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_ENABLE_REGr, xlp2, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe28 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 28, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe27 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 27, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe25 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 25, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe26 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 26, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_SOFT_RESETr.XLP3 */
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_SOFT_RESETr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_SOFT_RESETr, xlp3, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_CONTROLr.XLP3 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_CONTROLr, xlp3, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_SOFT_RESETr.XLP3 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_SOFT_RESETr, xlp3, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_ENABLE_REGr.XLP3 */
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT2f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT3f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT1f, 1);
        soc_reg_above_64_field32_set(unit, XLPORT_ENABLE_REGr, above64, PORT0f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_ENABLE_REGr, xlp3, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe31 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 31, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe30 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 30, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe29 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 29, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MAC_RSV_MASKr.xe32 */
        soc_reg_above_64_field32_set(unit, XLPORT_MAC_RSV_MASKr, above64, MASKf, 0x20058);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MAC_RSV_MASKr, 32, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MIB_RESETr.CLP0 */
        soc_reg_above_64_field32_set(unit, CLPORT_MIB_RESETr, above64, CLR_CNTf, 0xf);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MIB_RESETr, clp0, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MIB_RESETr.CLP1 */
        soc_reg_above_64_field32_set(unit, CLPORT_MIB_RESETr, above64, CLR_CNTf, 0xf);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MIB_RESETr, clp1, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MIB_RESETr.XLP0 */
        soc_reg_above_64_field32_set(unit, XLPORT_MIB_RESETr, above64, CLR_CNTf, 0xf);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MIB_RESETr, xlp0, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MIB_RESETr.XLP1 */
        soc_reg_above_64_field32_set(unit, XLPORT_MIB_RESETr, above64, CLR_CNTf, 0xf);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MIB_RESETr, xlp1, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MIB_RESETr.XLP2 */
        soc_reg_above_64_field32_set(unit, XLPORT_MIB_RESETr, above64, CLR_CNTf, 0xf);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MIB_RESETr, xlp2, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MIB_RESETr.XLP3 */
        soc_reg_above_64_field32_set(unit, XLPORT_MIB_RESETr, above64, CLR_CNTf, 0xf);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MIB_RESETr, xlp3, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MIB_RESETr.CLP0 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MIB_RESETr, clp0, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set CLPORT_MIB_RESETr.CLP1 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLPORT_MIB_RESETr, clp1, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MIB_RESETr.XLP0 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MIB_RESETr, xlp0, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MIB_RESETr.XLP1 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MIB_RESETr, xlp1, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MIB_RESETr.XLP2 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MIB_RESETr, xlp2, 0, above64));
        SOC_REG_ABOVE_64_CLEAR(above64); /* set XLPORT_MIB_RESETr.XLP3 */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLPORT_MIB_RESETr, xlp3, 0, above64));
#ifdef PLISIM
    } /* Do not run on pcid simulator not properly supporting port blocks */
#endif
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_TX_CTRLr.xe4 */
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_TX_CTRLr, 4, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_TX_CTRLr.xe2 */
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_TX_CTRLr, 2, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_MAX_SIZEr.xe4 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_MAX_SIZEr, 4, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_MAX_SIZEr.xe3 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_MAX_SIZEr, 3, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_MAX_SIZEr.xe2 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_MAX_SIZEr, 2, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_MAX_SIZEr.xe1 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_MAX_SIZEr, 1, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_LSS_CTRLr.xe4 */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_LSS_CTRLr, 4, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_LSS_CTRLr.xe2 */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_LSS_CTRLr, 2, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_LSS_CTRLr.xe1 */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_LSS_CTRLr, 1, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_CTRLr.xe3 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x30);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_CTRLr, 3, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_CTRLr.xe2 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x30);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_CTRLr, 2, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe4 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 4, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe3 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 3, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe1 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 1, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe2 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 2, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_CTRLr.xe1 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x30);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_CTRLr, 1, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_CTRLr.xe4 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x30);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_CTRLr, 4, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_LSS_CTRLr.xe3 */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_LSS_CTRLr, 3, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_TX_CTRLr.xe1 */
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_TX_CTRLr, 1, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_TX_CTRLr.xe3 */
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_TX_CTRLr, 3, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe2 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 2, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe1 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 1, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe3 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 3, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe4 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 4, 0, above64));
#if 1
    sal_usleep(50000);
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_LSS_CTRLr.xe5 */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_LSS_CTRLr, 5, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_CTRLr.xe5 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x30);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_CTRLr, 5, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe5 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 5, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_RX_MAX_SIZEr.xe5 */
    soc_reg_above_64_field32_set(unit, CLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_RX_MAX_SIZEr, 5, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_TX_CTRLr.xe5 */
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, CLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_TX_CTRLr, 5, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set CLMAC_CTRLr.xe5 */
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, TX_ENf, 1);
    soc_reg_above_64_field32_set(unit, CLMAC_CTRLr, above64, ALLOW_40B_AND_GREATER_PKTSf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, CLMAC_CTRLr, 5, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe18 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 18, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe20 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 20, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe19 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 19, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe17 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 17, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe19 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 19, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe18 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 18, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe17 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 17, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe18 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 18, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe19 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 19, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe20 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 20, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe17 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 17, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe20 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 20, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe18 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 18, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe17 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 17, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe19 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 19, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe20 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 20, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe20 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 20, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe19 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 19, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe17 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 17, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe18 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 18, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe24 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 24, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe21 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 21, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe24 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 24, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe21 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 21, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe24 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 24, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe21 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 21, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe24 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 24, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe21 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 21, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe22 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 22, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe23 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 23, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe22 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 22, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe23 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 23, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe22 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 22, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe23 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 23, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe22 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 22, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe23 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 23, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe24 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 24, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe23 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 23, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe21 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 21, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe22 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 22, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe27 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 27, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe25 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 25, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe28 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 28, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe25 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 25, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe28 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 28, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe27 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 27, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe26 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 26, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe25 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 25, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe26 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 26, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe27 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 27, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe26 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 26, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe27 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 27, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe28 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 28, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe25 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 25, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe26 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 26, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe28 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 28, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe28 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 28, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe27 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 27, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe25 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 25, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe26 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 26, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe32 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 32, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe31 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 31, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe31 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 31, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe30 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 30, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe32 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 32, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe29 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 29, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe30 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 30, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe31 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 31, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe32 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SOFT_RESETf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 32, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe29 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 29, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe30 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 30, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_CTRLr.xe31 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, RUNT_THRESHOLDf, 0x10);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRIP_CRCf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_RX_CTRLr, above64, STRICT_PREAMBLEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_CTRLr, 31, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe29 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 29, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_RX_MAX_SIZEr.xe32 */
    soc_reg_above_64_field32_set(unit, XLMAC_RX_MAX_SIZEr, above64, RX_MAX_SIZEf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_RX_MAX_SIZEr, 32, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe29 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 29, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_TX_CTRLr.xe30 */
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, PAD_THRESHOLDf, 0x40);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_THRESHOLDf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, TX_PREAMBLE_LENGTHf, 8);
    soc_reg_above_64_field32_set(unit, XLMAC_TX_CTRLr, above64, AVERAGE_IPGf, 0xc);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_TX_CTRLr, 30, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe32 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 32, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe30 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 30, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe29 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 29, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set XLMAC_CTRLr.xe31 */
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, EXTENDED_HIG2_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, SW_LINK_STATUSf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, RX_ENf, 1);
    soc_reg_above_64_field32_set(unit, XLMAC_CTRLr, above64, TX_ENf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, XLMAC_CTRLr, 31, 0, above64));
#endif /* 0 */

#endif /* QAX_MASK_NIF_OVERRIDES */

    SOC_REG_ABOVE_64_CLEAR(above64); /* set ILB_FPC_STATUSr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ILB_FPC_STATUSr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set SPB_DYNAMIC_CONFIGURATIONr */
    soc_reg_above_64_field32_set(unit, SPB_DYNAMIC_CONFIGURATIONr, above64, FIELD_1_6f, 0x30);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, SPB_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set DDP_FBC_DEBUGr */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DDP_FBC_DEBUGr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_INIT_FQP_TXI_CMICMr */
    soc_reg_above_64_field32_set(unit, EGQ_INIT_FQP_TXI_CMICMr, above64, INIT_FQP_TXI_CMICMf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_INIT_FQP_TXI_CMICMr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_INIT_FQP_TXI_OLPr */
    soc_reg_above_64_field32_set(unit, EGQ_INIT_FQP_TXI_OLPr, above64, INIT_FQP_TXI_OLPf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_INIT_FQP_TXI_OLPr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_INIT_FQP_TXI_OAMr */
    soc_reg_above_64_field32_set(unit, EGQ_INIT_FQP_TXI_OAMr, above64, INIT_FQP_TXI_OAMf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_INIT_FQP_TXI_OAMr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_INIT_FQP_TXI_RCYr */
    soc_reg_above_64_field32_set(unit, EGQ_INIT_FQP_TXI_RCYr, above64, INIT_FQP_TXI_RCYf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_INIT_FQP_TXI_RCYr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_INIT_FQP_TXI_SATr */
    soc_reg_above_64_field32_set(unit, EGQ_INIT_FQP_TXI_SATr, above64, INIT_FQP_TXI_SATf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_INIT_FQP_TXI_SATr, REG_PORT_ANY, 0, above64));
    SOC_REG_ABOVE_64_CLEAR(above64); /* set EGQ_INIT_FQP_TXI_IPSECr */
    soc_reg_above_64_field32_set(unit, EGQ_INIT_FQP_TXI_IPSECr, above64, INIT_FQP_TXI_IPSECf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_INIT_FQP_TXI_IPSECr, REG_PORT_ANY, 0, above64));

/*#ifndef QAX_MASK_NIF_OVERRIDES*/
    for (i=0; i<4; ++i) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIH_NIF_PM_CFGr, REG_PORT_ANY, i, PMH_N_TSC_RSTBf, 1));
    }
    for (i=0; i<3; ++i) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIL_NIF_PM_CFGr, REG_PORT_ANY, 0, PML_N_TSC_RSTBf, 1));
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIL_NIF_PM_CFG_3r, REG_PORT_ANY, 0, PML_N_TSC_RSTBf, 1));
/*#endif QAX_MASK_NIF_OVERRIDES*/


exit:
    SOCDNX_FUNC_RETURN;
}


/* QAX register/memory settings as overrides, to be later moved to the correct functions and removed */
int soc_qax_init_overrides(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_qax_init_overrides_0(unit));
    SOCDNX_IF_ERR_EXIT(soc_qax_init_overrides_1(unit));
    SOCDNX_IF_ERR_EXIT(soc_qax_init_overrides_4(unit));
    SOCDNX_IF_ERR_EXIT(soc_qax_init_overrides_5(unit));
    SOCDNX_IF_ERR_EXIT(soc_qax_init_overrides_6(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

/* QAX define share overrides for emulation */
int soc_qax_init_dpp_defs_overrides(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_DEFS_SET(unit, nof_pm4x25, 4);
    SOC_DPP_DEFS_SET(unit, nof_pm4x10, 4);
    SOC_DPP_DEFS_SET(unit, nof_pm4x10q, 3);
    SOC_DPP_DEFS_SET(unit, pmh_base_lane, 0);
    SOC_DPP_DEFS_SET(unit, pml_base_lane, 16);
    SOC_DPP_DEFS_SET(unit, nof_pms_per_nbi, 4);
    SOC_DPP_DEFS_SET(unit, nof_instances_nbil, 1);
    SOC_DPP_DEFS_SET(unit, nof_lanes_per_nbi, 16);
    SOC_DPP_DEFS_SET(unit, nof_ports_nbih, 16);
    SOC_DPP_DEFS_SET(unit, nof_ports_nbil, 52);

    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
* NAME:
*     soc_qax_tdq_contexts_init
* TYPE:
*   PROC
* DATE:
*   Dec 15 2015
* FUNCTION:
*     Init TXQ contexts
* INPUT:
*  SOC_SAND_IN  int                                unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   None
*********************************************************************/

soc_error_t
soc_qax_tdq_contexts_init(
      SOC_SAND_IN  int                 unit
    )
{

    /*need to init tdx context. Reference can be soc_jer_ipt_contexts_init*/

    SOCDNX_INIT_FUNC_DEFS;


    /*exit:*/
      SOCDNX_FUNC_RETURN;

}


