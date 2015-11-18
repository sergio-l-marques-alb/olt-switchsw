#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_sw_db.c,v 1.118 Broadcom SDK $
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
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_SWDB

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/intr.h>
#include <soc/error.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_api_dram.h>

#include <soc/dpp/dpp_wb_engine.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>

#include <soc/dpp/dpp_wb_engine.h>

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* maximum size of val to be inserted to multiset */
#define ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES  (16)

#define ARAD_SW_DB_DRAM_DELETED_BUFF_NONE 0xffffffff

#define ARAD_SW_1ST_AVAILABLE_HW_QUEUE SOC_TMC_ITM_NOF_QT_STATIC
#define ARAD_SW_NOF_AVAILABLE_HW_QUEUE SOC_TMC_ITM_NOF_QT_NDXS_ARAD
#define ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit) (SOC_IS_JERICHO(unit) ? SOC_TMC_ITM_NOF_QT_STATIC :  2)

#define ARAD_SW_DB_QUEUE_TYPE_IS_DYNAMIC(unit, user_q_type) \
        (user_q_type >= ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit) && user_q_type < ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit) + ARAD_SW_DB_NOF_LEGAL_DYNAMIC_QUEUE_TYPES(unit))

#define ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_VALID   0x1
#define ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_SW_ONLY 0x2

uint8 Arad_sw_db_initialized = FALSE;

/********************************************************************************************
 * Configuration
 * {
 ********************************************************************************************/


/*
 * } Configuration
 */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************************************
 * arad_sw_db_init
 * {
 *********************************************************************************************/

/********************************************************************************************
 * Initialization
 * {
 ********************************************************************************************/
uint32
  arad_sw_db_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_SW_DB_INIT);

  if (Arad_sw_db_initialized)
  {
    goto exit;
  }
  for (unit = 0; unit < SOC_SAND_MAX_DEVICE; ++unit)
  {
    Arad_sw_db.arad_device_sw_db[unit] = NULL;
  }

  Arad_sw_db_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_init()",0,0);
}
/********************************************************************************************
 * Configuration
 *********************************************************************************************/
void
  arad_sw_db_close(void)
{
  Arad_sw_db_initialized = FALSE;
}

/*********************************************************************************************
 * }
 * arad_chip_definitions
 * {
 *********************************************************************************************/


/*********************************************************************************************
 * }
 * arad_egr_ports
 * {
 *********************************************************************************************/
uint32
  arad_sw_db_dev_egr_ports_init(
    SOC_SAND_IN int      unit
  )
{
  uint32
    res; 
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  int core;
     
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_DEV_EGR_PORTS_INITIALIZE);

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);

  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports), ARAD_SW_DB_DEV_EGR_PORTS, 1);

  /* Bitmap occupation information for Channelize arbiter */
  /* Number of channelize arbiter is NOF - Non channelize arbiters */
  btmp_init_info.size = ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB - 1;
  btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_EGR_PORTS_CHANIF2CHAN_ARB_OCC;
  btmp_init_info.unit = unit;


  /* initialize the data to be mapped to*/
  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  btmp_init_info.size = ARAD_EGQ_NOF_IFCS - (ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB-1);
  btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_EGR_PORTS_NONCHANIF2SCH_OFFSET_OCC;

  /* initialize the data to be mapped to*/
  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  /* initialiaze channelized cals occupation bitmap */
  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {

      btmp_init_info.size = SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS);
      if (core == 0) {
          btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_CHANNELIZED_CALS_OCC_CORE0;
      } else if (core == 1){
          btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_CHANNELIZED_CALS_OCC_CORE1;
      } else {
          /* currently only 2 cores are supported, if more cores are added then new SOC_DPP_WB_ENGINE defines should be added for them */
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 57, exit);
      }

      /* initialize the data to be mapped to*/
      res = soc_sand_occ_bm_create(
              unit,
              &btmp_init_info,
              &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.channelized_cals_occ[core])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);
  }

  /* initialiaze modified channelized cals occupation bitmap */
  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {

      btmp_init_info.size = SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS);
      if (core == 0) {
          btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_CHANNELIZED_CALS_OCC_CORE0;
      } else if (core == 1){
          btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_CHANNELIZED_CALS_OCC_CORE1;
      } else {
          /* currently only 2 cores are supported, if more cores are added then new SOC_DPP_WB_ENGINE defines should be added for them */
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 57, exit);
      }

      /* initialize the data to be mapped to*/
      res = soc_sand_occ_bm_create(
              unit,
              &btmp_init_info,
              &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_channelized_cals_occ[core])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);
  }

  /* initialiaze e2e interfaces occupation bitmap */
  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {

      btmp_init_info.size = SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES);
      if (core == 0) {
          btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_E2E_INTERFACES_OCC_CORE0;
      } else if (core == 1){
          btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_E2E_INTERFACES_OCC_CORE1;
      } else {
          /* currently only 2 cores are supported, if more cores are added then new SOC_DPP_WB_ENGINE defines should be added for them */
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 59, exit);
      }

      /* initialize the data to be mapped to*/
      res = soc_sand_occ_bm_create(
              unit,
              &btmp_init_info,
              &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.e2e_interfaces_occ[core])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

  /* initialiaze modified e2e interfaces occupation bitmap */
  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {

      btmp_init_info.size = SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES);
      if (core == 0) {
          btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_E2E_INTERFACES_OCC_CORE0;
      } else if (core == 1){
          btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_E2E_INTERFACES_OCC_CORE1;
      } else {
          /* currently only 2 cores are supported, if more cores are added then new SOC_DPP_WB_ENGINE defines should be added for them */
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 61, exit);
      }

      /* initialize the data to be mapped to*/
      res = soc_sand_occ_bm_create(
              unit,
              &btmp_init_info,
              &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_e2e_interfaces_occ[core])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_dev_egr_ports_init()",0,0);
}

uint32
  arad_sw_db_src_bind_init(
    SOC_SAND_IN int      unit
  )
{
  uint32
    res; 
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_SRC_BIND_INITIALIZE);

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  
  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_src_binds), ARAD_SW_DB_SRC_BINDS, 1);
  /* Bitmap occupation information for spoof-id arbiter */
  btmp_init_info.size = ARAD_PP_SRC_BIND_IPV4_MAX_SPOOF_ID;
  btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_SRC_BINDS_SRCBIND_ARB_OCC;
  btmp_init_info.unit = unit;

  /* initialize the data to be mapped to*/
  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_src_binds.srcbind_arb_occ)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_src_bind_init()",0,0);
}


uint32
  arad_sw_db_reassembly_context_init(
    SOC_SAND_IN int      unit
  )
{
  uint32
    res; 
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_REASSEMBLY_CONTEXT_INITIALIZE);

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  
  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_reassembly_ctxt), ARAD_SW_DB_REASSBMEBLY_CTXT, 1);
  /* Bitmap occupation information for spoof-id arbiter */
  btmp_init_info.size = SOC_DPP_DEFS_GET(unit, num_of_reassembly_context);
  btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_REASSEMBLY_CONTEXT_OCC;
  btmp_init_info.unit = unit;

  /* initialize the data to be mapped to*/
  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_reassembly_ctxt.reassembly_ctxt_occ)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_reassembly_context_init()",0,0);
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_egr_ports_chanif2chan_arb_occ_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ;
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_egr_ports_nonchanif2sch_offset_occ_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ;
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_egr_channelized_cals_occ_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core
  )
{
    return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.channelized_cals_occ[core];
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_egr_e2e_interfaces_occ_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core
  )
{
    return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.e2e_interfaces_occ[core];
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_egr_modified_e2e_interfaces_occ_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core
  )
{
    return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_e2e_interfaces_occ[core];
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_egr_modified_channelized_cals_occ_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core
  )
{
    return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_channelized_cals_occ[core];
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_src_bind_arb_occ_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_src_binds.srcbind_arb_occ;
}


SOC_SAND_OCC_BM_PTR
  arad_sw_db_reassembly_context_occ_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_reassembly_ctxt.reassembly_ctxt_occ;
}

uint32
  arad_sw_db_egr_ports_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     base_q_pair,
    SOC_SAND_OUT ARAD_SW_DB_DEV_EGR_RATE    *val
  )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }

  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.get(unit, use_core, base_q_pair, val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_port_rate_get()",0,0);
}

uint32
  arad_sw_db_sch_port_rate_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_OUT uint32            *rate
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.sch_rates.get(unit, use_core, base_q_pair, rate);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_port_rate_get()",0,0);
}

uint32
  arad_sw_db_sch_port_rate_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_IN  uint32            rate
   )
{
  soc_error_t rv;
  int use_core = core;

  int nof_cores = 1, i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  for(i=0 ; i < nof_cores ; i++) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.sch_rates.set(unit, use_core + i, base_q_pair, rate);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_port_rate_set()",0,0);
}

uint32
  arad_sw_db_sch_priority_port_rate_set( 
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            offset,
   SOC_SAND_IN  uint32            rate,
   SOC_SAND_IN  uint8             valid
   )
{
  soc_error_t rv;
  int use_core = core;

  int nof_cores = 1, i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  for(i=0 ; i < nof_cores ; i++) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority.priority_shaper_rate.set(unit, use_core + i, offset, rate);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority.valid.set(unit, use_core + i, offset, valid);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_priority_port_rate_set()",0,0);
}


uint32 
  arad_sw_db_sch_priority_port_rate_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            offset,
   SOC_SAND_OUT int               *rate,
   SOC_SAND_OUT uint8             *valid
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority.priority_shaper_rate.get(unit, use_core, offset, rate);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority.valid.get(unit, use_core, offset, valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_priority_port_rate_get()",0,0);
}


uint32
  arad_sw_db_sch_port_tcg_rate_set( 
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            offset,
   SOC_SAND_IN  uint32            rate,
   SOC_SAND_IN  uint8             valid
   )
{
  soc_error_t rv;
  int use_core = core;

  int nof_cores = 1, i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  for(i=0 ; i < nof_cores ; i++) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_shaper.tcg_shaper_rate.set(unit, use_core + i, offset, rate);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_shaper.valid.set(unit, use_core + i, offset, valid);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_priority_port_rate_set()",0,0);
}


uint32 
  arad_sw_db_sch_port_tcg_rate_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            offset,
   SOC_SAND_OUT int               *rate,
   SOC_SAND_OUT uint8             *valid
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_shaper.tcg_shaper_rate.get(unit, use_core, offset, rate);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_shaper.valid.get(unit, use_core, offset, valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_port_tcg_rate_get()",0,0);
}


uint32
  arad_sw_db_egq_port_rate_get(
   SOC_SAND_IN   int               unit,
   SOC_SAND_IN   int               core,
   SOC_SAND_IN   uint32            base_q_pair,
   SOC_SAND_OUT  uint32           *rate
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.egq_rates.get(unit, use_core, base_q_pair, rate);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_egq_port_rate_get()",0,0);
}

uint32
  arad_sw_db_egq_port_rate_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_IN  uint32            rate
   )
{
  soc_error_t rv;
  int nof_cores = 1, i;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }

  for(i=0 ; i < nof_cores ; i++) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.egq_rates.set(unit, use_core+i, base_q_pair, rate);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_egq_port_rate_set()",0,0);
}

uint32
  arad_sw_db_is_port_valid_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_OUT uint8             *is_valid
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.valid.get(unit, use_core, base_q_pair, is_valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_is_port_valid_get()",0,0);
}

uint32
  arad_sw_db_is_port_valid_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_IN  uint8             is_valid
   )
{
  soc_error_t rv;
  int nof_cores = 1, i;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }

  for(i=0 ; i< nof_cores ; i++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.valid.set(unit, use_core+i, base_q_pair, is_valid);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_is_port_valid_set()",0,0);
}

uint32
  arad_sw_db_egr_ports_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  int core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ) {
      res = soc_sand_occ_bm_destroy(
              unit,
              Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ) {
      res = soc_sand_occ_bm_destroy(
              unit,
              Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {
      if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.channelized_cals_occ[core]) {
          res = soc_sand_occ_bm_destroy(
                  unit,
                  Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.channelized_cals_occ[core]
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
  }

  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {
      if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_channelized_cals_occ[core]) {
          res = soc_sand_occ_bm_destroy(
                  unit,
                  Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_channelized_cals_occ[core]
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      }
  }

  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {
      if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.e2e_interfaces_occ[core]) {
          res = soc_sand_occ_bm_destroy(
                  unit,
                  Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.e2e_interfaces_occ[core]
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
  }

  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {
      if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_e2e_interfaces_occ[core]) {
          res = soc_sand_occ_bm_destroy(
                  unit,
                  Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_e2e_interfaces_occ[core]
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_egr_ports_terminate()",0,0);
}

uint32
  arad_sw_db_src_bind_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_src_binds.srcbind_arb_occ) {
      res = soc_sand_occ_bm_destroy(
              unit,
              Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_src_binds.srcbind_arb_occ
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_egr_ports_terminate()",0,0);
}

uint32
  arad_sw_db_reassembly_context_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_reassembly_ctxt.reassembly_ctxt_occ) {
      res = soc_sand_occ_bm_destroy(
              unit,
              Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_reassembly_ctxt.reassembly_ctxt_occ
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_egr_ports_terminate()",0,0);
}

uint32
  arad_sw_db_device_tdm_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    tdm_context_map_id,
    res;
  uint8
    is_allocated;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tdm.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  if(!is_allocated) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tdm.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }
  for (tdm_context_map_id = 0; tdm_context_map_id < ARAD_NOF_TDM_CONTEXT_MAP; tdm_context_map_id++)
  {
      res = sw_state_access[unit].dpp.soc.arad.tm.tdm.context_map.set(unit, tdm_context_map_id, ARAD_IF_ID_NONE);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_device_tdm_init()",0,0);
}


uint32
  arad_sw_db_tm_init(
    SOC_SAND_IN int unit
  )
{
  uint8
    is_allocated;
  soc_error_t
    rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 5, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.is_simple_mode.set(unit, ARAD_SW_DB_QUEUE_TO_RATE_CLASS_MAPPING_IS_UNDEFINED);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
 
  /* No init for queue_to_rate_class_mapping_ref_count since this tabele is relevant iff queue_to_rate_class_mapping_is_simple==FALSE */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tm_init()",0,0);
}

uint32
  arad_sw_db_cnt_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->cnt), ARAD_SW_DB_CNT, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_cnt_init()",0,0);
}
uint32
  arad_sw_db_dram_init(
    SOC_SAND_IN int unit
  )
{
  uint8
    is_allocated;
  soc_error_t
    rv;
  uint32
    deleted_buf_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.dram.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.dram.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  for(deleted_buf_index=0; deleted_buf_index < ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++deleted_buf_index) {
    rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.set(unit, deleted_buf_index, ARAD_SW_DB_DRAM_DELETED_BUFF_NONE);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_dram_init()",0,0);
}

uint32
  arad_sw_db_tcam_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_id,
    is_inverse,
    tcam_db_id;
  ARAD_SW_DB_TCAM_DATA_FOR_RESTORATION 
      *restore_data;
  SOC_SAND_SORTED_LIST_PTR
    *priorities;
  SOC_SAND_SORTED_LIST_INIT_INFO
      priorities_init_info;
  SOC_SAND_HASH_TABLE_PTR
    *entry_id_to_location;
  SOC_SAND_HASH_TABLE_INIT_INFO
    entry_id_to_location_init_info;
  uint8
    is_allocated;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  restore_data = &(Arad_sw_db.arad_device_sw_db[unit]->tcam.restoration_data);

  res = soc_dpp_wb_engine_init_buffer(unit,SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_RESTORATION_DATA);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (SOC_WARM_BOOT(unit)) {
      /* if in warm reboot, reallocate pointers needed for wb restoration of dynamic data */
      for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
      {
          for (is_inverse = 0; is_inverse < 2; ++is_inverse)
          {
              if (restore_data->entries_used[bank_id][is_inverse].is_exist) {
                res = soc_sand_occ_bm_create(
                    unit,
                    &(restore_data->entries_used[bank_id][is_inverse].init_info),
                    &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_bank_entries_used[bank_id][is_inverse])
                );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
              }
          }
      }

      for (tcam_db_id = 0; tcam_db_id < ARAD_TCAM_MAX_NOF_LISTS; ++tcam_db_id)
      {
          /*realloc sorted list */
          if (restore_data->priorities[tcam_db_id].is_exist) {
              priorities = &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db_priorities[tcam_db_id]);
              soc_sand_os_memset(&priorities_init_info, 0x0, sizeof(SOC_SAND_SORTED_LIST_INIT_INFO));
              priorities_init_info = restore_data->priorities[tcam_db_id].init_info;
              priorities_init_info.get_entry_fun = NULL;
              priorities_init_info.set_entry_fun = NULL;
              priorities_init_info.cmp_func_type   = SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE_TCAM;
              res = soc_sand_sorted_list_create(unit, priorities, priorities_init_info);
              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
          }
 
          for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
          {
              if (restore_data->db_entries_used[tcam_db_id][bank_id].is_exist) {
                res = soc_sand_occ_bm_create(
                    unit,
                    &(restore_data->db_entries_used[tcam_db_id][bank_id].init_info),
                    &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db_entries_used[tcam_db_id][bank_id])
                );
                SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
              }
          }
      }


  }
  else{

      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

      if(!is_allocated) {
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
      }
      
      for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
      {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.set(unit, bank_id, FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      }

      for (tcam_db_id = 0; tcam_db_id < ARAD_TCAM_MAX_NOF_LISTS; ++tcam_db_id)
      {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.set(unit, tcam_db_id, FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      }

      sal_memset(&(Arad_sw_db.arad_device_sw_db[unit]->tcam.restoration_data), 0x0, sizeof(ARAD_SW_DB_TCAM_DATA_FOR_RESTORATION));
  }

  /*
   * Initialize the entry_id -> location hash table
   */
  entry_id_to_location = &Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location;
  entry_id_to_location_init_info.prime_handle  = unit;
  entry_id_to_location_init_info.sec_handle    = 0;
  entry_id_to_location_init_info.table_size    = SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit);
  entry_id_to_location_init_info.table_width   = SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit);
  entry_id_to_location_init_info.key_size      = ARAD_TCAM_DB_HASH_TBL_KEY_SIZE * sizeof(uint8);
  entry_id_to_location_init_info.data_size     = ARAD_TCAM_DB_HASH_TBL_DATA_SIZE * sizeof(uint8);
  entry_id_to_location_init_info.get_entry_fun = NULL;
  entry_id_to_location_init_info.set_entry_fun = NULL;
  entry_id_to_location_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_TCAM_DB_ENTRY_ID_TO_LOCATION;
  res = soc_sand_hash_table_create(unit, entry_id_to_location, entry_id_to_location_init_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  /*
   * Initialize the DB location table
   */
  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_OCC_BMS_DYNAMIC);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_DB_BANK_OCC_BMS_DYNAMIC);
  SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);
  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_HASH_TBL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_SORTED_LIST_DYNAMIC);
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  if(!SOC_WARM_BOOT(unit)) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.db_location_tbl.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);

      if(!is_allocated) {
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.db_location_tbl.alloc(unit, SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit));
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);
      }
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);

      if(!is_allocated) {
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.alloc(unit, SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit));
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);
      }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tcam_init()",0,0);
}

uint32
  arad_sw_db_vtt_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_HASH_TABLE_PTR
    *hash_tbl;
  SOC_SAND_HASH_TABLE_INIT_INFO
    init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  Initialize the route_key -> entry_id hash table
   */
  hash_tbl = &Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id;
  init_info.prime_handle  = unit;
  init_info.sec_handle    = 0;
  init_info.table_size    = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS; /* Maximal allowed */
  init_info.table_width   = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS;
  init_info.key_size      = 20;
  init_info.data_size     = sizeof(uint32);
  init_info.get_entry_fun = NULL;
  init_info.set_entry_fun = NULL;
  init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_VTT_ISEM_KEY_TO_ENTRY_ID;

  res = soc_sand_hash_table_create(
          unit,
          hash_tbl,
          init_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_vtt_init()", 0, 0);
}

uint32
  arad_sw_db_frwrd_ip_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_HASH_TABLE_PTR
    *hash_tbl;
  SOC_SAND_HASH_TABLE_INIT_INFO
    init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  Initialize the route_key -> entry_id hash table
   */
  hash_tbl = &Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id;
  init_info.prime_handle  = unit;
  init_info.sec_handle    = 0;
  init_info.table_size    = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS; /* Maximal allowed */
  init_info.table_width   = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS;
  init_info.key_size      = 20;
  init_info.data_size     = sizeof(uint32);
  init_info.get_entry_fun = NULL;
  init_info.set_entry_fun = NULL;
  init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_FRWRD_IP_ROUTE_KEY_TO_ENTRY_ID;

  res = soc_sand_hash_table_create(
          unit,
          hash_tbl,
          init_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  if(!SOC_WARM_BOOT(unit)) {
      uint8 is_allocated;
      /* Allocate the location table for ACL: index ARAD_KBP_FRWRD_IP_NOF_TABLES */
	  res = sw_state_access[unit].dpp.soc.arad.tm.kbp_frwrd_ip.is_allocated(unit, &is_allocated);
	  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

      if(!is_allocated) {
    	  res = sw_state_access[unit].dpp.soc.arad.tm.kbp_frwrd_ip.alloc(unit);
    	  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
      }

      res = sw_state_access[unit].dpp.soc.arad.tm.kbp_frwrd_ip.location_tbl.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

      if(!is_allocated) {
          res = sw_state_access[unit].dpp.soc.arad.tm.kbp_frwrd_ip.location_tbl.alloc(unit, ARAD_KBP_NL_88650_MAX_NOF_ENTRIES + SOC_DPP_DEFS_MAX_NOF_ENTRY_IDS);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      }

      
  }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_frwrd_ip_init()", 0, 0);
}

uint32
  arad_sw_db_tcam_mgmt_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    bank_id,
    prefix;
  uint8
    is_allocated;
  soc_error_t
    rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
  {
    for (prefix = 0; prefix < ARAD_TCAM_NOF_PREFIXES; ++prefix)
    {
      rv = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.prefix_db.set(unit, bank_id, prefix, ARAD_TCAM_MAX_NOF_LISTS);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tcam_mgmt_init()",0,0);
}

/*
 * Interrupts sw db init
*/

STATIC uint32
  arad_sw_db_interrupts_init(
    SOC_SAND_IN int unit
  )
{
    int nof_interrupts;
    uint8 is_allocated;
    soc_error_t rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    rv = sw_state_access[unit].dpp.soc.arad.tm.interrupts.is_allocated(unit, &is_allocated);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 2, exit);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.interrupts.alloc(unit);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 2, exit);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.is_allocated(unit, &is_allocated);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    if(!is_allocated) {
        rv = soc_nof_interrupts(unit, &nof_interrupts);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 4, exit);
        rv = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.alloc(unit, nof_interrupts);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_init()",0,0); 
}

/*
 * SW DB multiset
 */
uint32
  arad_sw_db_buffer_get_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_IN  uint8                              *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_OUT uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_os_memcpy(
    data,
    buffer + (offset * len),
    len
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_buffer_get_entry()",0,0);
}

uint32
  arad_sw_db_buffer_set_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_INOUT  uint8                           *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_IN  uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_os_memcpy(
    buffer + (offset * len),
    data,
    len
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_buffer_set_entry()",0,0);
}
uint32
  arad_sw_db_vtt_terminate(
                               SOC_SAND_IN  int unit
                               )
{
  uint32
    res;
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  hash_tbl = Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id;
  if (hash_tbl->init_info.table_size != 0) {
      res = soc_sand_hash_table_destroy(unit, hash_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_vtt_terminate()",0,0);
}

uint32
  arad_sw_db_frwrd_ip_terminate(
                               SOC_SAND_IN  int unit
                               )
{
  uint32
    res;
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  hash_tbl = Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id;
  if (hash_tbl->init_info.table_size != 0) {
      res = soc_sand_hash_table_destroy(unit, hash_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_frwrd_ip_terminate()",0,0);
}


uint32 
  arad_sw_db_pmf_pls_init(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage
  )
{
    uint32
        is_tm,
        indx;
    soc_error_t
        rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_BELOW_MIN(stage, 0, ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(stage, ARAD_NOF_FP_DATABASE_STAGES, ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR, 20, exit);
    for(indx = 0; indx < ARAD_PMF_NOF_LEVELS; ++ indx) 
    {
        for(is_tm = 0; is_tm < 2; ++is_tm) 
        {
            rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.level_index.set(unit, stage, is_tm, indx, indx);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pmf_pls_init()",0,0);
}


uint32
  arad_sw_db_pmf_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res,
      fem_ndx,
    db_ndx;
  ARAD_FP_DATABASE_STAGE            
      stage;
  ARAD_PP_FP_DATABASE_INFO
      db_info;
  ARAD_PP_FP_FEM_ENTRY
      fem_entry;
  uint8
    is_for_tm,
    is_allocated;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  if(!is_allocated) {
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.alloc(unit, ARAD_NOF_FP_DATABASE_STAGES);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  }
  for (stage = 0; stage < ARAD_NOF_FP_DATABASE_STAGES; stage ++) {
      res = arad_sw_db_pmf_pls_init(unit, stage);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      ARAD_PP_FP_DATABASE_INFO_clear(&db_info);
      for (db_ndx = 0; db_ndx < ARAD_PP_FP_NOF_DBS; ++db_ndx)
      {
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.set(unit, stage, db_ndx, db_info);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      }

      ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
      for (fem_ndx = 0; fem_ndx < ARAD_PMF_LOW_LEVEL_NOF_FEMS; ++fem_ndx)
      {
          for (is_for_tm = FALSE ; is_for_tm <=TRUE; ++is_for_tm) 
          {
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(unit, stage, is_for_tm, fem_ndx, fem_entry);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
          }
      }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pmf_initialize()",0,0);
}

uint32
  arad_sw_db_eg_encap_prge_tbl_nof_dynamic_entries_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint32 *nof_dynamic_members
  )
{
    *nof_dynamic_members = ARAD_EGR_PROG_EDITOR_PRGE_MEMORY_NOF_DATA_ENTRIES;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "88650L23C", 0))
    {/*If L2 header feature is enabled - 2 last entries are reserved for this program*/
        (*nof_dynamic_members)                      -= 2;
    }
    if (1 == soc_property_get(unit, spn_BCM886XX_ERSPAN_TUNNEL_ENABLE, 1) &&
        SOC_IS_ARAD_B1_AND_BELOW(unit))
    {/*If Mirror Erspan feature is enabled - 32 entries are reserved for this program, 2 entries per mirror id*/
        (*nof_dynamic_members)                      -= (16 * 2);
    }

    if (SOC_IS_ROO_ENABLE(unit) && SOC_IS_ARADPLUS(unit)) 
    {/* If routing over overlay (ROO) feature enabled.
      * Use prge data table for overlay arp entries only in arad+, 16 entries are reserved for this program */
        (*nof_dynamic_members)                      -= (16);   
    }

    return SOC_SAND_OK;
}

/* get base index of overlay arp entries in prge data table */
uint32 
  arad_sw_db_eg_encap_prge_tbl_overlay_arp_entries_base_index_get(
     SOC_SAND_IN  int  unit, 
     SOC_SAND_OUT uint32 *overlay_arp_entry_base_index
     ) {
    *overlay_arp_entry_base_index = ARAD_EGR_PROG_EDITOR_PRGE_MEMORY_NOF_DATA_ENTRIES;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "88650L23C", 0))
    {/*If L2 header feature is enabled - 2 last entries are reserved for this program*/
        (*overlay_arp_entry_base_index)                      -= 2;
    }

    if (SOC_IS_ROO_ENABLE(unit) && SOC_IS_ARADPLUS(unit)) 
    {/* If routing over overlay (ROO) feature enabled.
      * Use prge data table for overlay arp entries only in arad+, 16 entries are reserved for this program */
        (*overlay_arp_entry_base_index)                      -= (16);   
    }

    return SOC_SAND_OK;
}

/*
 * SW DB multiset
 */
SOC_SAND_MULTI_SET_INFO*
  arad_sw_db_multiset_by_type_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type /* ARAD_SW_DB_MULTI_SET */
  )
{

  /* ARAD_NOF_SW_DB_MULTI_SETS may be changed and be more thean 0 */
  /* coverity[unsigned_compare : FALSE] */
  if (multiset_type >= ARAD_NOF_SW_DB_MULTI_SETS)
  {
    return arad_pp_sw_db_multiset_by_type_get(unit,multiset_type);
  }
}

uint32
  arad_sw_db_multiset_add(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 *val,
    SOC_SAND_OUT  uint32                *data_indx,
    SOC_SAND_OUT  uint8               *first_appear,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint8
    tmp_val[ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES];
  uint8
    add_success;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_ADD);

  multi_set = arad_sw_db_multiset_by_type_get(unit,multiset_type);

  soc_sand_U32_to_U8(val,multi_set->init_info.member_size,tmp_val);

  res = soc_sand_multi_set_member_add(
          unit,
          multi_set,
          (SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY*)tmp_val,
          data_indx,
          first_appear,
          &add_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (add_success)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_add()",0,0);
}

uint32
  arad_sw_db_multiset_remove(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32        *val,
    SOC_SAND_OUT  uint32       *data_indx,
    SOC_SAND_OUT  uint8      *last_appear
  )
{
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint8
    tmp_val[ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES];
  uint32
    val_lcl[1],
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_REMOVE);

  multi_set = arad_sw_db_multiset_by_type_get(unit,multiset_type);

  *val_lcl = *val;
  soc_sand_U32_to_U8(val_lcl,multi_set->init_info.member_size,tmp_val);

  res = soc_sand_multi_set_member_remove(
          unit,
          multi_set,
          (SOC_SAND_IN SOC_SAND_MULTI_SET_KEY*)tmp_val,
          data_indx,
          last_appear
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_remove()",0,0);
}

uint32
  arad_sw_db_multiset_lookup(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32               *val,
    SOC_SAND_OUT  uint32              *data_indx,
    SOC_SAND_OUT  uint32              *ref_count
  )
{
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint8
    tmp_val[ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES];
  uint32
    val_lcl[1],
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_LOOKUP);

  multi_set = arad_sw_db_multiset_by_type_get(unit,multiset_type);

  *val_lcl = *val;
  soc_sand_U32_to_U8(val_lcl,multi_set->init_info.member_size,tmp_val);

  res = soc_sand_multi_set_member_lookup(
          unit,
          multi_set,
          (SOC_SAND_IN SOC_SAND_MULTI_SET_KEY*)&tmp_val,
          data_indx,
          ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_lookup()",0,0);
}

uint32
  arad_sw_db_multiset_add_by_index(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 *val,
    SOC_SAND_OUT  uint32                data_indx,
    SOC_SAND_OUT  uint8               *first_appear,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint8
    tmp_val[ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES];
  uint8
    add_success;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_ADD_BY_INDEX);

  multi_set = arad_sw_db_multiset_by_type_get(unit,multiset_type);

  soc_sand_U32_to_U8(val,multi_set->init_info.member_size,tmp_val);
	
  res = soc_sand_multi_set_member_add_at_index(
          unit,
          multi_set,
          (SOC_SAND_IN	SOC_SAND_MULTI_SET_KEY*)tmp_val,
          data_indx,
          first_appear,
          &add_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
	
  if (add_success)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_add_by_index()",0,0);
}


uint32
  arad_sw_db_multiset_remove_by_index(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 data_indx,
    SOC_SAND_OUT  uint8               *last_appear
  )
{
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_REMOVE_BY_INDEX);

  multi_set = arad_sw_db_multiset_by_type_get(unit,multiset_type);

  res = soc_sand_multi_set_member_remove_by_index(
          unit,
          multi_set,
          data_indx,
          last_appear
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_remove_by_index()",0,0);
}

uint32
  arad_sw_db_multiset_clear(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type /* ARAD_SW_DB_MULTI_SET */
  )
{
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_CLEAR);

  multi_set = arad_sw_db_multiset_by_type_get(unit,multiset_type);
  res = soc_sand_multi_set_clear(
          unit,
          multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_clear()",0,0);
}

/* 
 * set all value of modport2sysport SW DB to invalid.
 */
STATIC uint32 arad_sw_db_modport2sysport_init(
    SOC_SAND_IN int unit
  )
{
  soc_error_t rv;
  uint32 modport_idx;
  uint8 is_allocated;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.alloc(unit, ARAD_NOF_MODPORT);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  for(modport_idx = 0; modport_idx < ARAD_NOF_MODPORT; ++modport_idx) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.set(unit, modport_idx, 0xffff);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, ARAD_GEN_ERR_NUM_CLEAR, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_init()", 0, 0);
}

int
    arad_sw_db_sw_state_alloc(
        SOC_SAND_IN int     unit
  )
{
    soc_error_t rv;
    uint32 res;
    int core;
    uint8 is_allocated;
    
    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.arad.tm.alloc(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    res = arad_sw_db_op_mode_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_sw_db_tm_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.alloc(unit, ARAD_NOF_SYS_PHYS_PORTS_GET(unit) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_IS_ARADPLUS(unit)) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_plus.is_allocated(unit, &is_allocated);
        SOCDNX_IF_ERR_EXIT(rv);

        if(is_allocated) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_plus.alloc(unit);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Initalize ERP interface ID to NONE */
    /* We are not going to change macros to avoid such cases */
    /* coverity[same_on_both_sides] */
    for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.erp_interface_id.set(unit, core, ARAD_IF_ID_NONE);
        SOCDNX_IF_ERR_EXIT(rv);
    }

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    rv = sw_state_access[unit].dpp.soc.arad.tm.kbp_info.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.kbp_info.alloc(unit);
      SOCDNX_IF_ERR_EXIT(rv);
    }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

    rv = sw_state_access[unit].dpp.soc.arad.tm.lag.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.lag.alloc(unit);
      SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.cell.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.cell.alloc(unit);
      SOCDNX_IF_ERR_EXIT(rv);
    }

    res = arad_sw_db_device_tdm_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_sw_db_dram_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.alloc(unit, SOC_TMC_ITM_NOF_QT_NDXS);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.is_allocated(unit, &is_allocated);
      SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
      res = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.alloc(unit, ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.vsi.is_allocated(unit, &is_allocated);
      SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.vsi.alloc(unit);
      SOCDNX_IF_ERR_EXIT(rv);
    }

    res = arad_sw_db_modport2sysport_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_sw_db_interrupts_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);


exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_device_init(
    SOC_SAND_IN int     unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_DEVICE_INIT);

  ARAD_ALLOC_ANY_SIZE(Arad_sw_db.arad_device_sw_db[unit], ARAD_SW_DB_DEVICE, 1,"Arad_sw_db.arad_device_sw_db[unit]");

  res = arad_sw_db_dev_egr_ports_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN(res,  70, exit, dpp_mcds_multicast_init(unit));


    res = arad_sw_db_tcam_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

	if(!SOC_WARM_BOOT(unit)) {
	    res = arad_sw_db_tcam_mgmt_init(unit);
	    SOC_SAND_CHECK_FUNC_RESULT(res, 183, exit);

        res = arad_sw_db_pmf_initialize(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

	}

    res = arad_sw_db_vtt_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 184, exit);

    res = arad_sw_db_frwrd_ip_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 186, exit);

     res = arad_sw_db_cnt_init(unit);
     SOC_SAND_CHECK_FUNC_RESULT(res, 232, exit);
    if (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE)
    {
        res = arad_sw_db_src_bind_init(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);

        res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SRC_BIND);
        SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
    }

    res = arad_sw_db_reassembly_context_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 281, exit);

    res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_REASSEMBLY_CONTEXT);
    SOC_SAND_CHECK_FUNC_RESULT(res, 282, exit);

    res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS);
    SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_arad_device_init()",unit,0);
}

uint32
  arad_sw_db_device_close(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_DEVICE_CLOSE);

  res = arad_sw_db_egr_ports_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_sw_db_tcam_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  res = arad_sw_db_vtt_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_sw_db_frwrd_ip_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, dpp_mcds_multicast_terminate(unit));

  if (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE)
  {
    res = arad_sw_db_src_bind_terminate(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

    res = arad_sw_db_reassembly_context_terminate(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  
  ARAD_FREE_ANY_SIZE(Arad_sw_db.arad_device_sw_db[unit]);
  Arad_sw_db.arad_device_sw_db[unit] = NULL;

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_device_close()",0,0);
}
/*
 * Cnt
 */
uint32
  arad_sw_db_cnt_buff_and_index_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                     proc_id,
    SOC_SAND_IN uint32                     *buff,
    SOC_SAND_IN uint32                     index
                                 )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         cnt.host_buff[proc_id],
                         (&buff)
                         );
    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         cnt.buff_line_ndx[proc_id],
                         (&index)
                         );


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_cnt_buff_and_index_set()",0,0);
}
uint32
  arad_sw_db_cnt_buff_and_index_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                     proc_id,
    SOC_SAND_OUT uint32                     **buff,
    SOC_SAND_OUT uint32                     *index
                                 )
{
    /*

    ARAD_SW_DB_INIT_DEFS;

    ARAD_SW_DB_FIELD_GET(
                        unit,
                        cnt.host_buff[proc_id],
                        buff);
    ARAD_SW_DB_FIELD_GET(
                        unit,
                        cnt.(buff_line_ndx[proc_id]),
                        index);
    */
    *buff = Arad_sw_db.arad_device_sw_db[unit]->cnt.host_buff[proc_id];
    *index = Arad_sw_db.arad_device_sw_db[unit]->cnt.buff_line_ndx[proc_id];
    return *index;

}

uint32
  arad_sw_db_dram_deleted_buff_list_add(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff
  )
{
    int i, saved_indx=0, indx_saved=0;
    soc_error_t rv;
    uint32 dram_deleted_buff_list;
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);   
   
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.get(unit, i, &dram_deleted_buff_list);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
        if(dram_deleted_buff_list == buff) {
            break;
        } else {
            if( (indx_saved == 0) && dram_deleted_buff_list == ARAD_SW_DB_DRAM_DELETED_BUFF_NONE) {
                saved_indx = i;
                indx_saved = 1;
            }
        }        
    }
 
    if(indx_saved == 1) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.set(unit, saved_indx, buff);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    } 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_dram_deleted_buff_list_add()",0,0);
}

uint32
  arad_sw_db_dram_deleted_buff_list_remove(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff
  )
{
    int i;
    soc_error_t rv;
    uint32 dram_deleted_buff_list;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.get(unit, i, &dram_deleted_buff_list);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
        if(dram_deleted_buff_list == buff) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.set(unit, i, ARAD_SW_DB_DRAM_DELETED_BUFF_NONE);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
            break;
        }      
    }    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_dram_deleted_buff_list_remove()",0,0);
}

int
  arad_sw_db_dram_deleted_buff_list_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff,
    SOC_SAND_OUT uint32*     is_deleted
  )
{
    int i;
    soc_error_t rv;
    uint32 dram_deleted_buff_list;
    
    SOCDNX_INIT_FUNC_DEFS;  
    
    *is_deleted = 0;
    
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.get(unit, i, &dram_deleted_buff_list);
        SOCDNX_IF_ERR_EXIT(rv);
        if(dram_deleted_buff_list == buff) {
            *is_deleted = 1;
            break;
        }      
    }    
    
exit:
  SOCDNX_FUNC_RETURN;
}

int 
  arad_sw_db_dram_deleted_buff_list_get_all(
    SOC_SAND_IN int    unit,
    SOC_SAND_OUT uint32*    buff_list_arr,
    SOC_SAND_IN uint32      arr_size,
    SOC_SAND_OUT uint32*    buff_list_num)
{
    int i;
    soc_error_t rv;
    uint32 dram_deleted_buff_list;
    
    SOCDNX_INIT_FUNC_DEFS;  

    *buff_list_num = 0;
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.get(unit, i, &dram_deleted_buff_list);
        SOCDNX_IF_ERR_EXIT(rv);
        if(dram_deleted_buff_list != ARAD_SW_DB_DRAM_DELETED_BUFF_NONE) {
            buff_list_arr[*buff_list_num] = dram_deleted_buff_list;
            ++(*buff_list_num);
            if(*buff_list_num == arr_size) {
                break; 
            }
        }      
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_op_mode_init(
    SOC_SAND_IN int unit
  )
{
  uint8 is_allocated;
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_REVISION_INIT);

  rv = sw_state_access[unit].dpp.soc.arad.tm.op_mode.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 25, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.op_mode.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 25, exit);
  }

  rv = sw_state_access[unit].dpp.soc.arad.tm.op_mode.is_petrab_in_system.set(unit, FALSE);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 35, exit);

  rv = sw_state_access[unit].dpp.soc.arad.tm.op_mode.tdm_mode.set(unit, ARAD_MGMT_TDM_MODE_PACKET);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_op_mode_init()",0,0);
}

void
  arad_sw_db_is_petrab_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_petrab_in_system
  )
{
  sw_state_access[unit].dpp.soc.arad.tm.op_mode.is_petrab_in_system.set(unit, is_petrab_in_system);
}

uint8
  arad_sw_db_is_petrab_in_system_get(
    SOC_SAND_IN int unit
  )
{
    uint8 is_petrab_in_system;
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.is_petrab_in_system.get(unit, &is_petrab_in_system);
    return is_petrab_in_system;
}

void
  arad_sw_db_tdm_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_MGMT_TDM_MODE tdm_mode
  )
{
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.tdm_mode.set(unit, tdm_mode);
}

ARAD_MGMT_TDM_MODE
  arad_sw_db_tdm_mode_get(
    SOC_SAND_IN int unit
  )
{
    ARAD_MGMT_TDM_MODE tdm_mode;
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.tdm_mode.get(unit, &tdm_mode);
    return tdm_mode;
}

void
  arad_sw_db_ilkn_tdm_dedicated_queuing_set(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing
  )
{
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.ilkn_tdm_dedicated_queuing.set(unit, ilkn_tdm_dedicated_queuing);
}

ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
  arad_sw_db_ilkn_tdm_dedicated_queuing_get(
     SOC_SAND_IN int unit
  )
{
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.ilkn_tdm_dedicated_queuing.get(unit, &ilkn_tdm_dedicated_queuing);
    return ilkn_tdm_dedicated_queuing;
}

uint32
  arad_sw_db_tcam_occ_bm_restore_set(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                   bank_id,
    SOC_SAND_IN uint32                   is_inverse,
    SOC_SAND_IN ARAD_TCAM_OCC_BM_RESTORE occ_bm_restore
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_OCC_BM_RESTORE, &occ_bm_restore, bank_id, is_inverse);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_sorted_list_restore_set()",0,0);
}

uint32
  arad_sw_db_tcam_occ_bm_restore_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   bank_id,
    SOC_SAND_IN  uint32                   is_inverse,
    SOC_SAND_OUT ARAD_TCAM_OCC_BM_RESTORE *occ_bm_restore
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_OCC_BM_RESTORE, occ_bm_restore, bank_id, is_inverse);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_occ_bm_restore_get()",0,0);
}

uint32
  arad_sw_db_tcam_db_bank_occ_bm_restore_set(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                   tcam_db_id,
    SOC_SAND_IN uint32                   bank_id,
    SOC_SAND_IN ARAD_TCAM_OCC_BM_RESTORE occ_bm_restore
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_OCC_BM_RESTORE, &occ_bm_restore, tcam_db_id, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_bank_occ_bm_restore_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_bank_occ_bm_restore_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   tcam_db_id,
    SOC_SAND_IN  uint32                   bank_id,
    SOC_SAND_OUT ARAD_TCAM_OCC_BM_RESTORE *occ_bm_restore
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_OCC_BM_RESTORE, occ_bm_restore, tcam_db_id, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_bank_occ_bm_restore_get()",0,0);
}


uint32
  arad_sw_db_tcam_sorted_list_restore_set(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN uint32                        tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_SORTED_LIST_RESTORE sorted_list_restore
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_SORTED_LIST_RESTORE, &sorted_list_restore, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_sorted_list_restore_set()",0,0);
}

uint32
  arad_sw_db_tcam_sorted_list_restore_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_SORTED_LIST_RESTORE *sorted_list_restore
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_SORTED_LIST_RESTORE, sorted_list_restore, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_sorted_list_restore_get()",0,0);
}

void
  arad_sw_db_tcam_bank_entries_used_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32       bank_id,
    SOC_SAND_IN uint8        is_inverse,
    SOC_SAND_IN SOC_SAND_OCC_BM_PTR entries_used
  )
{
  /* warmboot is handled at init*/
  Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_bank_entries_used[bank_id][is_inverse] = entries_used;
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_tcam_bank_entries_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8  is_inverse
  )
{
  /* warmboot is handled at init*/
  return Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_bank_entries_used[bank_id][is_inverse];
}

void
  arad_sw_db_tcam_db_entries_used_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32       tcam_db_id,
    SOC_SAND_IN uint32       bank_id,
    SOC_SAND_IN SOC_SAND_OCC_BM_PTR entries_used
  )
{
  /* warmboot is handled at init*/
  Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db_entries_used[tcam_db_id][bank_id] = entries_used;
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_tcam_db_entries_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id
  )
{
  /* warmboot is handled at init*/
  return Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db_entries_used[tcam_db_id][bank_id];
}
uint32
  arad_sw_db_tcam_db_forbidden_dbs_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 tcam_db_other,
    SOC_SAND_IN uint8  is_forbidden
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (is_forbidden) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.forbidden_dbs.bit_set(unit, tcam_db_id, tcam_db_other);
    }
    else {
        rv = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.forbidden_dbs.bit_clear(unit, tcam_db_id, tcam_db_other);
    }

    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_forbidden_dbs_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_forbidden_dbs_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 tcam_db_other,
    SOC_SAND_OUT uint8 *is_forbidden
  )
{
    soc_error_t
        rv;
    uint8
        bit_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.forbidden_dbs.bit_get(unit, tcam_db_id, tcam_db_other, &bit_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    *is_forbidden  = (bit_val > 0);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_forbidden_dbs_get()",0,0);
}
SOC_SAND_SORTED_LIST_INFO *
  arad_sw_db_tcam_db_priorities_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db_priorities[tcam_db_id];
}

SOC_SAND_HASH_TABLE_INFO *
  arad_sw_db_tcam_db_entry_id_to_location_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location;
}

SOC_SAND_HASH_TABLE_INFO *
  arad_sw_db_entry_key_to_entry_id_hash_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id;
}

SOC_SAND_HASH_TABLE_INFO *
  arad_sw_db_frwrd_ip_route_key_to_entry_id_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id;
}



uint32
  arad_sw_db_tcam_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    bank_id,
    tcam_db_id,
    res;
  uint8
      is_inverse;
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  uint32
    bank_nof_entries;
  uint8
    valid;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  res = SOC_SAND_OK;

  /* Add destroy of ARAD_SW_DB_TCAM_MGMT_INFO like in Petra B */
  for (bank_id=0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); bank_id++ )
  {
      /* For jericho sumulation, all banks can be allocated.  */
      #ifndef PLISIM   
      if (SOC_IS_JERICHO(unit) && (bank_id != 12) && (bank_id != 13)) {
          
          continue;
      }
      else 
      #endif /* PLISIM */
      {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.get(unit, bank_id, &valid);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        if(valid)
        {
          for (is_inverse = FALSE; is_inverse <= TRUE; is_inverse++) {
            res = soc_sand_occ_bm_destroy(unit, Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_bank_entries_used[bank_id][is_inverse]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          }
        }
      }
  }

  for (tcam_db_id=0; tcam_db_id < ARAD_TCAM_MAX_NOF_LISTS; tcam_db_id++)
  {
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, tcam_db_id, &valid);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    if(valid)
    {
      res = soc_sand_sorted_list_destroy(
        unit,
        Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db_priorities[tcam_db_id]
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
      for (bank_id=0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); bank_id++)
      {
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, bank_id, &bank_nof_entries);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);
          if(bank_nof_entries)
          {
              res = soc_sand_occ_bm_destroy(unit, Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db_entries_used[tcam_db_id][bank_id]);
              SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          }
      }
    }
  }

  hash_tbl = Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location;
  if (hash_tbl->init_info.table_size != 0) {
      res = soc_sand_hash_table_destroy(unit, hash_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tcam_terminate()",0,0);
}



/*
 * PMF resource sattus API
 * SW state for CE bitmap per program, stage
 */

/*
 * FP info {
 */

/*
 * Field Processor
 */
uint32
  arad_sw_db_fp_db_entry_bitmap_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  uint32                     bank_index,
    SOC_SAND_IN  uint32                     entry_index,
    SOC_SAND_IN  uint8                      is_used
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(is_used) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.entry_bitmap.bit_set(unit, stage, bank_index, entry_index); 
    }
    else {
        rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.entry_bitmap.bit_clear(unit, stage, bank_index, entry_index); 
    }
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_db_entry_bitmap_set()",0,0);
}

uint32
  arad_sw_db_fp_db_entry_bitmap_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  uint32                     bank_index,
    SOC_SAND_IN  uint32                     entry_index,
    SOC_SAND_OUT uint8                      *is_used
  )
{
    soc_error_t
        rv;
    uint8
        bit_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.entry_bitmap.bit_get(unit, stage, bank_index, entry_index, &bit_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    *is_used  = (bit_val > 0 );

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_db_entry_bitmap_get()",0,0);
}
 
uint32
  arad_sw_db_fp_db_entry_bitmap_clear(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE     stage
  )
{
    soc_error_t
        rv;
    int i;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (i = 0; i < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); i++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.entry_bitmap.bit_range_clear(unit, stage, i, 0, SOC_DPP_DEFS_MAX_TCAM_NOF_LINES_IN_LONGS*32);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_db_entry_bitmap_clear()",0,0);
}
 
/*
 * FP info }
 */


/*
 * check/set if a (egress) local port has a reassembly context reserved for it
 * for a non mirroring application. 
 */
uint32
  arad_sw_db_is_port_reserved_for_reassembly_context(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_OUT uint8  *is_reserved /* returns one of: 0 for not reserved, 1 for reserved */
  )
{
  uint32
    port_reserved_reassembly_context;
  soc_error_t
    rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(local_port, ARAD_NOF_LOCAL_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_CHECK_NULL_INPUT(is_reserved);

  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_reserved_reassembly_context.get(unit, local_port / SOC_SAND_NOF_BITS_IN_UINT32, &port_reserved_reassembly_context);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  *is_reserved = (port_reserved_reassembly_context >> (local_port % SOC_SAND_NOF_BITS_IN_UINT32)) & 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_is_port_reserved_for_reassembly_context()",local_port,0);
}

uint32
  arad_sw_db_set_port_reserved_for_reassembly_context(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint8   reserve /* 0 will cancel reservation, other values will reserve */
  )
{
  soc_error_t
    rv;
  uint32 mask = 1 << (local_port % SOC_SAND_NOF_BITS_IN_UINT32);
  uint32 value;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(local_port, ARAD_NOF_LOCAL_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);

  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_reserved_reassembly_context.get(unit, local_port / SOC_SAND_NOF_BITS_IN_UINT32, &value);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  if (reserve) {
    value |= mask;
  } else {
    value &= ~mask;
  }
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_reserved_reassembly_context.set(unit, local_port / SOC_SAND_NOF_BITS_IN_UINT32, value);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_set_port_reserved_for_reassembly_context()",local_port,reserve);
}


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP 	 
int	 
arad_sw_db_sw_dump(int unit) 	 
{
    uint32                          i, j;
    int                             core;
    soc_error_t                     rv;
    uint16                          current_cell_ident;
    ARAD_INTERFACE_ID               context_map;
    ARAD_SW_DB_DEV_EGR_RATE         rates;
    ARAD_SW_DB_DEV_RATE             tcg_rate;
    ARAD_EGR_PROG_TM_PORT_PROFILE   ports_prog_editor_profile;
    ARAD_SW_DB_DEV_RATE             queue_rate;
    uint32                          calcal_length;
    ARAD_SW_DB_DEV_EGR_CHAN_ARB     chan_arb;

    SOCDNX_INIT_FUNC_DEFS;  
        

    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n ARAD SOC TM:")));
    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n ------------")));

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.cell.current_cell_ident.get(unit, &current_cell_ident));
    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n current_cell_ident:   %u\n"),  current_cell_ident));

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); i++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.calcal_length.get(unit, i, &calcal_length);
        SOCDNX_IF_ERR_EXIT(rv);
        LOG_VERBOSE(BSL_LS_SOC_SWDB,
                    (BSL_META_U(unit,
                                "\n calcal_length:        %u\n"),  calcal_length));
    }

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); i++) {
        for (j = 0; j < ARAD_EGR_NOF_Q_PAIRS; j++) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority_cal.queue_rate.get(unit, i, j, &queue_rate);
            SOCDNX_IF_ERR_EXIT(rv);
            if(queue_rate.valid) {
                LOG_VERBOSE(BSL_LS_SOC_SWDB,
                            (BSL_META_U(unit,
                                        "\n queue_rate (%03d): valid %hhu egq_rates %u egq_bursts %u\n"), 
                                        i, 
                             queue_rate.valid, 
                             queue_rate.egq_rates, 
                             queue_rate.egq_bursts));
            }
        }
    }

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); i++) {
        for (j = 0; j < SOC_DPP_DEFS_GET(unit, nof_channelized_calendars); j++) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.chan_arb.get(unit, i, j, &chan_arb);
            SOCDNX_IF_ERR_EXIT(rv);
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n nof_calcal_instances (%02d):  %u\n"), i, chan_arb.nof_calcal_instances));
        }
    }

    for (i = 0; i < ARAD_NOF_FAP_PORTS; i++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.ports_prog_editor_profile.get(unit, i, &ports_prog_editor_profile);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_prog_editor_profile != 0) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n ports_prog_editor_profile (%03d):  %hu\n"), i, ports_prog_editor_profile));
        }
    }

    for (i = 0; i < ARAD_NOF_LAG_GROUPS_MAX; i++) {
        uint8 in_use;
        rv = sw_state_access[unit].dpp.soc.arad.tm.lag.in_use.get(unit, i, &in_use);
        SOCDNX_IF_ERR_EXIT(rv);
        if(in_use != 0) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n in_use (%04d):  %hhu\n"), i, in_use));
        }
    }

    for (i = 0; i < ARAD_NOF_LOCAL_PORTS; i++) {
        uint32  local_to_reassembly_context;
        rv = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.get(unit, i, &local_to_reassembly_context);
        SOCDNX_IF_ERR_EXIT(rv);
        if(local_to_reassembly_context != i) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n local_to_reassembly_context (%03d):  %u\n"), i, local_to_reassembly_context));
        }
    }

    for (i = 0; i < ARAD_NOF_TDM_CONTEXT_MAP; i++) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.context_map.get(unit, i, &context_map));
        if(context_map != i) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n tdm_context_map (%03d):  %u\n"), i, context_map));
        }
    }
 
    for (core = 0; core < SOC_DPP_DEFS_GET(unit, nof_cores); core++) {
        for (i = 0; i < ARAD_EGR_NOF_PS; i++) {
            for (j = 0; j < ARAD_NOF_TCGS; j++) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_cal.tcg_rate.get(unit, core, i, j, &tcg_rate);
                SOCDNX_IF_ERR_EXIT(rv);
                if(tcg_rate.valid) {
                    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                                (BSL_META_U(unit,
                                            "\n eg_mult_nof_vlan_bitmaps (%02d, %01d): valid %hhu egq_rates %u egq_bursts %u\n"), 
                                            i, j, 
                                 tcg_rate.valid, 
                                 tcg_rate.egq_rates, 
                                 tcg_rate.egq_bursts));
                }
            }
        }
    }

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); i++) {
        for (j = 0; j < ARAD_EGR_NOF_BASE_Q_PAIRS; j++) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.get(unit, i, j, &rates);
            SOCDNX_IF_ERR_EXIT(rv);
            if(rates.valid) {
                LOG_VERBOSE(BSL_LS_SOC_SWDB,
                            (BSL_META_U(unit,
                                        "\n rates (%02d, %03d): valid %hhu sch_rates %u egq_rates %u egq_bursts %u\n"), 
                                        i, j, 
                             rates.valid, 
                             rates.sch_rates, 
                             rates.egq_rates, 
                             rates.egq_bursts));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#define ARAD_DEVICE_NUMBER_BITS 11
#define ARAD_DEVICE_NUMBER_MASK 0x7ff

/*
Set a fap_id x fap_port_id to system physical port mapping.
Performs allocation inside the data structure if needed.
*/
uint32 arad_sw_db_modport2sysport_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id,
    SOC_SAND_IN ARAD_SYSPORT sysport
  )
{
  soc_error_t rv;
  uint32 modport;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_id, ARAD_NOF_FAPS_IN_SYSTEM, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_port_id, ARAD_NOF_FAP_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(sysport, ARAD_NOF_SYS_PHYS_PORTS_GET(unit), SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);

  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);

  modport = fap_id | (fap_port_id << ARAD_DEVICE_NUMBER_BITS);
  SOC_SAND_ERR_IF_ABOVE_NOF(modport, ARAD_NOF_MODPORT, SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);
  rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.set(unit, modport, sysport);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_set()", fap_id, fap_port_id);
}

uint32 arad_sw_db_modport2sysport_remove(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  res = arad_sw_db_modport2sysport_set(unit,fap_id, fap_port_id, ARAD_SW_DB_MODPORT2SYSPORT_INVALID_SYSPORT);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_remove()", fap_id, fap_port_id);
}

/*
Get a fap_id x fap_port_id to system physical port mapping.
If the mapping does not exist, the value of ARAD_NOF_SYS_PHYS_PORTS is returned
*/
uint32 arad_sw_db_modport2sysport_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id,
    SOC_SAND_OUT ARAD_SYSPORT *sysport
  )
{
  uint32 modport;
  ARAD_SYSPORT modport2sysport;
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(sysport);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_id, ARAD_NOF_FAPS_IN_SYSTEM, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_port_id, ARAD_NOF_FAP_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 15, exit);
  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);

  modport = fap_id | (fap_port_id << ARAD_DEVICE_NUMBER_BITS);
  SOC_SAND_ERR_IF_ABOVE_NOF(modport, ARAD_NOF_MODPORT, SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);
  rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.get(unit, modport, &modport2sysport);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);
  if (modport2sysport != ARAD_SW_DB_MODPORT2SYSPORT_INVALID_SYSPORT) {
      *sysport = modport2sysport;
  } else {
      *sysport = ARAD_SYS_PHYS_PORT_INVALID(unit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_get()", fap_id, fap_port_id);
}

/*
Get a reverse system physical port to fap_id x fap_port_id mapping.
Works by searching the mapping till finding the system physical port.
If the mapping does not exist, the value of ARAD_SW_DB_MODPORT2SYSPORT_REVERSE_GET_NOT_FOUND is returned in fap_id and in fap_port_id.
In direct mapping mode the fap and its port may not be found. In this case their returned value will be ARAD_SW_DB_MODPORT2SYSPORT_REVERSE_GET_NOT_FOUND.
*/
uint32 arad_sw_db_modport2sysport_reverse_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_SYSPORT sysport,
    SOC_SAND_OUT uint32 *fap_id,
    SOC_SAND_OUT uint32 *fap_port_id
  )
{
  unsigned modport_i;
  ARAD_SYSPORT modport2sysport;
  soc_error_t rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(fap_id);
  SOC_SAND_CHECK_NULL_INPUT(fap_port_id);
  SOC_SAND_ERR_IF_ABOVE_NOF(sysport, ARAD_NOF_SYS_PHYS_PORTS_GET(unit), SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);
  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);

  for (modport_i = 0; modport_i < ARAD_NOF_MODPORT; ++modport_i) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.get(unit, modport_i, &modport2sysport);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);
      if (modport2sysport == sysport){
          *fap_id = modport_i & ARAD_DEVICE_NUMBER_MASK;
          *fap_port_id = modport_i >> ARAD_DEVICE_NUMBER_BITS;
          break;
      }
  }

  if (*fap_id >= ARAD_NOF_FAPS_IN_SYSTEM ||
      *fap_port_id >= ARAD_NOF_FAP_PORTS ||
      modport_i == ARAD_NOF_MODPORT) {
      *fap_id = *fap_port_id = ARAD_SW_DB_MODPORT2SYSPORT_REVERSE_GET_NOT_FOUND;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_reverse_get()", sysport, 0);
}

uint32
  arad_sw_db_sysport2queue_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32          core_id,
    SOC_SAND_IN ARAD_SYSPORT    sysport,
    SOC_SAND_IN uint8           valid,
    SOC_SAND_IN uint8           sw_only,
    SOC_SAND_IN uint32          base_queue
   )
{
    uint8 valid_flags = 0;
    uint32 base_queue_lcl;
    int core_index = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid unit")));
    }
    
    if (ARAD_IS_VOQ_MAPPING_INDIRECT(unit) && ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID_INDIRECT < sysport) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid sysport: %d"), sysport));
    }
    if (ARAD_IS_VOQ_MAPPING_DIRECT(unit) && ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID < sysport){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid sysport: %d"), sysport));
    }
    if (core_id != SOC_CORE_ALL && core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid core ID: %d"), core_id));
    }

    valid_flags = 0;
    if (valid) {
        valid_flags |= ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_VALID;
    }
    if (sw_only) {
        valid_flags |= ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_SW_ONLY;
    } 
    SOC_DPP_ASSYMETRIC_CORES_ITER(core_id, core_index) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.valid_flags.set(unit, (sysport * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_index, valid_flags));
        base_queue_lcl = (valid) ? base_queue : ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE;
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.base_queue.set(unit, (sysport * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_index, base_queue_lcl));
    }

    exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_sysport2queue_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  ARAD_SYSPORT    sysport,
    SOC_SAND_OUT uint8          *valid,
    SOC_SAND_OUT uint8          *sw_only,
    SOC_SAND_OUT uint32         *base_queue
   )
{
    int core_offset = 0;
    uint8 valid_flags = 0;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid unit")));
    }
    if (ARAD_IS_VOQ_MAPPING_INDIRECT(unit) && ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID_INDIRECT < sysport) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid sysport: %d"), sysport));
    }
    if (ARAD_IS_VOQ_MAPPING_DIRECT(unit) && ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID < sysport){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid sysport: %d"), sysport));
    }
    if (core_id != SOC_CORE_ALL && !SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit)){
        if (core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid core_id: %d"),sysport));
        }
        core_offset = core_id;
    } else {
        core_offset = 0;
    }
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.valid_flags.get(unit, (sysport * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_offset, &valid_flags));
    
    *valid = (valid_flags & ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_VALID) ? TRUE : FALSE;
    *sw_only = (valid_flags & ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_SW_ONLY) ? TRUE : FALSE;

    if (*valid) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.base_queue.get(unit, (sysport * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_offset, base_queue));
    } else {
        *base_queue = ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE;
    }
   
exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_queue_type_ref_count_exchange(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  int            core,
    SOC_SAND_IN  uint8          orig_q_type,
    SOC_SAND_IN  uint8          new_q_type)
{    
    soc_error_t
        rv;
    uint32 
        orig_q_type_ref_count, new_q_type_ref_count;
    int 
        nof_queue_remaped;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Invalid unit: %d"), unit));
    }
    if((core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core != BCM_CORE_ALL){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Core %d out of range"), core));
    } else if (core == BCM_CORE_ALL) {
        nof_queue_remaped = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    } else {
        nof_queue_remaped = 1;
    }
    if (orig_q_type >= SOC_TMC_ITM_NOF_QT_NDXS && orig_q_type != SOC_TMC_ITM_QT_NDX_INVALID) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Invalid queue type: %d"), orig_q_type));
    }
    if (new_q_type >= SOC_TMC_ITM_NOF_QT_NDXS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Invalid queue type: %d"), new_q_type));
    }

    if (orig_q_type != SOC_TMC_ITM_QT_NDX_INVALID) {
        /*decrease original q type ref count*/
        rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.get(unit, orig_q_type, &orig_q_type_ref_count);
        SOCDNX_IF_ERR_EXIT(rv);

        if (!orig_q_type_ref_count) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("No Queues mapped to queue type: %d"), orig_q_type));
        }
        orig_q_type_ref_count -= nof_queue_remaped;

        rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.set(unit, orig_q_type, orig_q_type_ref_count);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /*increase original q type ref count*/
    rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.get(unit, new_q_type, &new_q_type_ref_count);
    SOCDNX_IF_ERR_EXIT(rv);

    new_q_type_ref_count += nof_queue_remaped;

    rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.set(unit, new_q_type, new_q_type_ref_count);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/* Get the hardware queue type mapped to from the user queue type. Returns ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE in mapped_q_type if not found */
uint32
  arad_sw_db_queue_type_map_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8          user_q_type,  /* input user queue type (predefined type or user defined allocated type) */
    SOC_SAND_OUT uint8*         mapped_q_type /* output hardware queue type, 0 if not mapped */
  )
{
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(mapped_q_type);

  if (user_q_type == SOC_TMC_ITM_QT_NDX_INVALID) {
       *mapped_q_type = SOC_TMC_ITM_QT_NDX_INVALID;
  } else if (ARAD_SW_DB_QUEUE_TYPE_IS_DYNAMIC(unit, user_q_type)) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit,
        user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
        mapped_q_type);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      if (!*mapped_q_type) {
          *mapped_q_type = ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE;
      }
  } else if (user_q_type >= SOC_TMC_ITM_PREDEFIEND_OFFSET &&
             (user_q_type < SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_NOF_QT_STATIC ||
              user_q_type == SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_QT_PUSH_Q_NDX)) {
      *mapped_q_type = user_q_type - SOC_TMC_ITM_PREDEFIEND_OFFSET;
  } else {
      SOC_SAND_SET_ERROR_CODE(ARAD_ITM_IPS_QT_RNG_OUT_OF_RANGE_ERR, 10, exit); /* unsupported user queue type */
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_queue_type_map_get()", unit, user_q_type);
}

/*
 * Get the hardware queue type mapped to from the user queue type, allocating it if it was not allocated before.
 * Returns ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE in mapped_q_type if mapping is not possible since all hardware types (credit request profiles) are used.
 * If given a predefined queue type, will just return it as output as it does not use dynamic allocation.
 */
uint32
  arad_sw_db_queue_type_map_get_alloc(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8          user_q_type,  /* input user queue type (predefined type or user defined allocated type) */
    SOC_SAND_OUT uint8*         mapped_q_type /* output hardware queue type, 0 if not mapped */
  )
{
  uint8       q_type_map;
  soc_error_t rv;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(mapped_q_type);

  if (ARAD_SW_DB_QUEUE_TYPE_IS_DYNAMIC(unit, user_q_type)) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit,
        user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
        &q_type_map);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      if (SOC_IS_JERICHO(unit)) {
          /*in Jericho there are more profiles so we staticly map between user_q_type an HW, and never try to catch preconfigure profiles*/
          if (!q_type_map) {
              rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.set(unit,
                user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
                user_q_type);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
              *mapped_q_type = user_q_type;
          } else {
              *mapped_q_type = q_type_map;
          }
      } else {
          if (!q_type_map) { /* if the user queue is not mapped, try to map it */
              uint8 reverse_mapping[ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1 - ARAD_SW_1ST_AVAILABLE_HW_QUEUE] = {0};
              uint8 i, hw_q;
              for (i = ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit); i < ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1; ++i) {
                  rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit,
                    i - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
                    &q_type_map);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);
                  if ((hw_q = q_type_map)) {
                      hw_q -= ARAD_SW_1ST_AVAILABLE_HW_QUEUE;
                      if ((hw_q >= ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1 - ARAD_SW_1ST_AVAILABLE_HW_QUEUE) || reverse_mapping[hw_q]) {
                          SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 99, exit); /* internal error */
                      }
                      reverse_mapping[hw_q] = 1;
                  }
              }
              hw_q = 0;
              for (i = 0; i < ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1 - ARAD_SW_1ST_AVAILABLE_HW_QUEUE; ++i) {
                  if (!reverse_mapping[i]) {
                      hw_q = i + ARAD_SW_1ST_AVAILABLE_HW_QUEUE;
                      break;
                  }
              }
              rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.set(unit,
                user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
                hw_q ? hw_q : ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE /* also handle the case of no available hardware queue */);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 40, exit);
          }
          rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit,
            user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
            mapped_q_type);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 50, exit);
      }
      } else if (user_q_type >= SOC_TMC_ITM_PREDEFIEND_OFFSET && user_q_type < SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_NOF_QT_STATIC) {
          *mapped_q_type = user_q_type - SOC_TMC_ITM_PREDEFIEND_OFFSET;
      } else {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_IPS_QT_RNG_OUT_OF_RANGE_ERR, 10, exit); /* unsupported user queue type */
      }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_queue_type_map_get()", unit, user_q_type);
}

/* Get the user queue type mapped from the given hardware queue type. */
uint32
  arad_sw_db_queue_type_map_reverse_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8          mapped_q_type,  /* input hardware queue type, 0 if not mapped */
    SOC_SAND_OUT uint8*         user_q_type     /* output user queue type (predefined type or user defined allocated type */
  )
{
  uint8       q_type_map;
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(user_q_type);

  if (mapped_q_type < SOC_TMC_ITM_NOF_QT_STATIC || mapped_q_type == SOC_TMC_ITM_QT_PUSH_Q_NDX) {
    *user_q_type = mapped_q_type + SOC_TMC_ITM_PREDEFIEND_OFFSET;
  } else if (SOC_IS_JERICHO(unit)) {
      *user_q_type = mapped_q_type;
  } else {
    uint8 i;
    if (mapped_q_type >= ARAD_SW_NOF_AVAILABLE_HW_QUEUE) {
        SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 20, exit); /* internal error */
    }
    for (i = 0; i < ARAD_SW_DB_NOF_LEGAL_DYNAMIC_QUEUE_TYPES(unit); ++i) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit, i, &q_type_map);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      if (q_type_map == mapped_q_type) {
        break;
      }
    }
    if (i >= ARAD_SW_DB_NOF_LEGAL_DYNAMIC_QUEUE_TYPES(unit)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 20, exit); /* no user queue type is mapped to this hardware value; */
    }
    *user_q_type = i + ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_queue_type_map_reverse_get()", unit, mapped_q_type);
}
uint32 
    arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_exchange(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      old_rate_class,
       SOC_SAND_IN  uint32                      new_rate_class) 
{
    uint32 res = SOC_SAND_OK;
    uint32 new_ref_count, old_ref_count;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if (old_rate_class != new_rate_class) 
    {
        SOC_SAND_ERR_IF_ABOVE_MAX(old_rate_class, SOC_TMC_ITM_RATE_CLASS_MAX,ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(new_rate_class, SOC_TMC_ITM_RATE_CLASS_MAX,ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 20, exit);

        res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ref_count.get(unit, old_rate_class, &old_ref_count);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ref_count.get(unit, new_rate_class, &new_ref_count);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

        SOC_SAND_ERR_IF_ABOVE_MAX(new_ref_count, ARAD_MAX_QUEUE_ID(unit), ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 60, exit);

        res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ref_count.set(unit, new_rate_class, new_ref_count+1);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

        res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ref_count.set(unit, old_rate_class, old_ref_count-1);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_exchange()",0,0);
}

/* Mark the given egress multicast group as open or not in SWDB */
uint32 arad_sw_db_egress_group_open_set(
    SOC_SAND_IN  int     unit, /* device */
    SOC_SAND_IN  uint32  group_id,  /* multicast ID */
    SOC_SAND_IN  uint8   is_open    /* non zero value will mark the group as open */
)
{
    soc_error_t rv;
  
  SOCDNX_INIT_FUNC_DEFS;
  if (!SOC_UNIT_NUM_VALID(unit)) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid unit")));
  } else if (group_id >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("input too big")));
  }



  if(is_open) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_set(unit, group_id);
  } else {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_clear(unit, group_id);
  }
  SOCDNX_IF_ERR_EXIT(rv);

exit:
  SOCDNX_FUNC_RETURN;
}


/* Mark all egress multicast groups as open or not in SWDB */
uint32 arad_sw_db_egress_group_open_set_all(
    SOC_SAND_IN  int     unit, /* device */
    SOC_SAND_IN  uint8   is_open    /* non zero value will mark the group as open */
)
{
    uint32 i;
    uint8 cur_bit;
    soc_error_t rv;
  
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_get(unit, i , &cur_bit);
        SOCDNX_IF_ERR_EXIT(rv);
        if ((cur_bit ? 1: 0) != (is_open ? 1 : 0)) {
            if(is_open) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_set(unit, i);
            } else {
                rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_clear(unit, i);
            }
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
  SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */

