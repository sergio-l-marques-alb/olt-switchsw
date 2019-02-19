#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#ifdef BCM_88675_A0
/* $Id: jer_ingress_packet_queuing.c,v  $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INGRESS

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/JER/jer_ingress_packet_queuing.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#ifdef THESE_INCLUDES_ARE_NOT_CURRENTLY_NEEDED
#include <soc/dpp/cosq.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_scheduler_end2end.h>

#include <soc/dpp/ARAD/arad_ports.h>

#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/mem.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>
#endif /* THESE_INCLUDES_ARE_NOT_CURRENTLY_NEEDED */

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* Max & min values for struct ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO:      */
#define JER_IPQ_EXPLICIT_MAPPING_MODE_INFO_BASE_QUEUE_ID_MIN      0


/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */




/*********************************************************************
* NAME:
*     arad_ipq_init
* FUNCTION:
*     Initialization of the Arad blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
jer_ipq_init(
   SOC_SAND_IN  int                 unit
   ) {
   uint32 res = SOC_E_NONE;
   SOC_TMC_IPQ_TR_CLS cls_id;
   uint32 system_red = ARAD_IS_VOQ_MAPPING_INDIRECT(unit) ? 1 : 0;
   uint32 mcr_limit_uc;
   uint64 reg64;
   uint32 reg32;
   int core;
   SOCDNX_INIT_FUNC_DEFS;

   /*
    *  Traffic Class mapping -
    *    initialize to "no change"
    */
    for (cls_id = SOC_TMC_IPQ_TR_CLS_MIN; cls_id <= SOC_TMC_IPQ_TR_CLS_MAX; cls_id++) {
        /*res = arad_ipq_traffic_class_map_set_unsafe(unit, cls_id, cls_id);*/ 
        /*SOCDNX_SAND_IF_ERR_EXIT(res);*/
    }

    /* Set System red enable by default */
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOCDNX_IF_ERR_EXIT(READ_IPS_IPS_GENERAL_CONFIGURATIONSr(unit, core, &reg64));
        soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DISCARD_ALL_IQM_MSGf, 0);
        soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DISCARD_ALL_CRDTf, 0);
        soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DISABLE_STATUS_MSG_GENf, 0);
        soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DIS_DEQ_CMDSf, 0);
        soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, ENABLE_SYSTEM_REDf, system_red);
        soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, UPDATE_MAX_QSZ_FROM_LOCALf, 1);
        soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, SEND_FSM_FOR_BIGGER_MAX_QUEUE_SIZEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_IPS_GENERAL_CONFIGURATIONSr(unit, core, reg64));

    }

    res = soc_reg_above_64_field32_read(unit, IRR_MCR_FIFO_CONFIGr, REG_PORT_ANY, 0, MCR_N_LIMIT_UCf, &mcr_limit_uc);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    /*
     * When FC is received from IQM, use separate thresholds for snoop/mirror, and separate counters
     */
    res = soc_reg_above_64_field32_modify(unit, IRR_UNICAST_FIFO_THRESHOLDSr, REG_PORT_ANY, 0, MCR_N_UC_FIFO_SNOOP_THRESHOLDf, ARAD_IPQ_UC_FIFO_SNOOP_THRESHOLD(mcr_limit_uc));
    res = soc_reg_above_64_field32_modify(unit, IRR_UNICAST_FIFO_THRESHOLDSr, REG_PORT_ANY, 0, MCR_N_UC_FIFO_MIRROR_THRESHOLDf, ARAD_IPQ_UC_FIFO_MIRROR_THRESHOLD(mcr_limit_uc));
    SOCDNX_IF_ERR_EXIT(res);


    /* configure credit deq command to support full rate */
    reg32 = 0;
    soc_reg_field_set(unit, IPS_DEQ_COMMAND_CREDIT_ADJUSTr, &reg32, DEQ_COMMAND_CREDIT_THf, 0x30);
    soc_reg_field_set(unit, IPS_DEQ_COMMAND_CREDIT_ADJUSTr, &reg32, LFSR_BIT_NUMf, 0x6);
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_DEQ_COMMAND_CREDIT_ADJUSTr(unit, SOC_CORE_ALL, reg32));

exit:
    SOCDNX_FUNC_RETURN;
}


#ifdef JER_OVERRIDE_ARAD_IMPLMENTATION

/*********************************************************************
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
jer_ipq_explicit_mapping_mode_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
    ) {

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    /*
     * JER_IPQ_EXPLICIT_MAPPING_MODE_INFO_BASE_QUEUE_ID_MIN may be changed and be grater then zero.
     */
    /* coverity[unsigned_compare] */
    if ((info->base_queue_id < JER_IPQ_EXPLICIT_MAPPING_MODE_INFO_BASE_QUEUE_ID_MIN) ||
        (info->base_queue_id >= SOC_DPP_DEFS_GET(unit, nof_queues))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,(_BSL_SOCDNX_MSG("base queue ID is out of range")));
    }

    LOG_ERROR(BSL_LS_SOC_INIT,
              (BSL_META_U(unit,
                          "Jericho function not implemented yet\n")));
           
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_ipq_explicit_mapping_mode_info_get(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_OUT SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
   ) {

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(info);
    LOG_ERROR(BSL_LS_SOC_INIT,
              (BSL_META_U(unit,
                          "Jericho function not implemented yet\n")));
           
exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif /* BCM_88675_A0 */

