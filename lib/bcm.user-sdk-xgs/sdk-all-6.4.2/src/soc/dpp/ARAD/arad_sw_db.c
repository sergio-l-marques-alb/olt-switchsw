#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_sw_db.c,v 1.118 Broadcom SDK $
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
#include <soc/intr.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_api_dram.h>

#include <soc/dpp/ARAD/arad_wb_db.h>
#include <soc/dpp/dpp_wb_engine.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_l3_src_bind.h>
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
#define ARAD_SW_NOF_AVAILABLE_HW_QUEUE SOC_TMC_ITM_NOF_QT_NDXS
#define ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE 2

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
          &btmp_init_info,
          &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  btmp_init_info.size = ARAD_EGQ_NOF_IFCS - (ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB-1);
  btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_EGR_PORTS_NONCHANIF2SCH_OFFSET_OCC;

  /* initialize the data to be mapped to*/
  res = soc_sand_occ_bm_create(
          &btmp_init_info,
          &(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  /* Initalize ERP interface ID to NONE */
  Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.erp_interface_id = ARAD_IF_ID_NONE;

  Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.egq_tcg_qpair_shaper_enable = FALSE;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_dpp_wb_engine_init_buffer(unit,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_RESERVED_REASSEMBLY_CONTEXTS)); /* initialize WB buffer and on warm boot restore Arad+ config */
  if (!SOC_WARM_BOOT(unit)) {
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_RESERVED_REASSEMBLY_CONTEXTS,
        Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_reserved_reassembly_context); /* initialize warm boot data */
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
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
  arad_sw_db_egr_ports_egq_tcg_qpair_shaper_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8  is_enable
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         arad_sw_db_egr_ports.egq_tcg_qpair_shaper_enable,
                         &is_enable
                         );

#ifdef BCM_WARM_BOOT_SUPPORT
    
    
    
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
  
    res = arad_wb_db_egr_port_update_egq_tcg_qpair_shaper_enable_state(unit); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_egr_ports_dsp_pp_to_base_queue_pair_mapping_set()",0,0);
}

uint8
  arad_sw_db_egr_ports_egq_tcg_qpair_shaper_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.egq_tcg_qpair_shaper_enable;
}

uint32
  arad_sw_db_egr_ports_prog_editor_profile_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 ofp_idx,
    SOC_SAND_IN ARAD_EGR_PROG_TM_PORT_PROFILE port_profile
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         arad_sw_db_egr_ports.ports_prog_editor_profile[ofp_idx],
                         &port_profile
                         );


#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }

    res = arad_wb_db_egr_port_update_ports_prog_editor_profile_state(unit, ofp_idx); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_egr_ports_prog_editor_profile_set()",0,0);
}

ARAD_EGR_PROG_TM_PORT_PROFILE
  arad_sw_db_egr_ports_prog_editor_profile_get(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  tm_port
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.ports_prog_editor_profile[tm_port];
}
ARAD_INTERFACE_ID
  arad_sw_db_egr_ports_erp_interface_get(
    SOC_SAND_IN int unit 
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.erp_interface_id;
}

uint32
  arad_sw_db_egr_ports_erp_interface_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_INTERFACE_ID erp_interface_id    
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         arad_sw_db_egr_ports.erp_interface_id,
                         &erp_interface_id
                         );

#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    
    res = arad_wb_db_egr_port_update_erp_interface_state(unit); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_egr_ports_erp_interface_set()",0,0);
}

uint32
  arad_sw_db_ofp_rates_calcal_length_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 calcal_length
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         arad_sw_db_egr_ports.calcal_length,
                         &calcal_length
                         );


#ifdef BCM_WARM_BOOT_SUPPORT
    
    
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    
    res = arad_wb_db_egr_port_update_calcal_length_state(unit); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_ofp_rates_calcal_length_set()",0,0);
}

uint32
  arad_sw_db_ofp_rates_calcal_length_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.calcal_length;
}

uint32
  arad_sw_db_ofp_rates_update_dev_changed_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 update_dev_changed
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         arad_sw_db_egr_ports.update_dev_changed,
                         &update_dev_changed);


#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
       
    res = arad_wb_db_egr_port_update_update_dev_changed_state(unit); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_ofp_rates_update_dev_changed_set()",0,0);
}

uint8
  arad_sw_db_ofp_rates_update_dev_changed_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.update_dev_changed;
}

uint32
  arad_sw_db_ofp_rates_nof_instances_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_OFP_RATES_EGQ_CHAN_ARB_ID chan_arb_id,
    SOC_SAND_IN uint32 nof_instances
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (chan_arb_id == ARAD_OFP_RATES_EGQ_CHAN_ARB_INVALID
        || chan_arb_id == ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB)
    {
        return SOC_SAND_ERR; 
    }
  
    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         arad_sw_db_egr_ports.chan_arb[chan_arb_id].nof_calcal_instances,
                         &nof_instances
                         );


#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }       
    res = arad_wb_db_egr_port_update_nof_calcal_instances_state(unit, chan_arb_id); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_ofp_rates_nof_instances_set()",0,0);
}

uint32
  arad_sw_db_ofp_rates_nof_instances_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_OFP_RATES_EGQ_CHAN_ARB_ID chan_arb_id
  )
{
  if (chan_arb_id == ARAD_OFP_RATES_EGQ_CHAN_ARB_INVALID
      || chan_arb_id == ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB)
  {
    return SOC_SAND_ERR;
  }

  return Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chan_arb[chan_arb_id].nof_calcal_instances;
  
}

uint32
  arad_sw_db_egr_ports_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     base_q_pair,
                 int                        core,
    SOC_SAND_OUT ARAD_SW_DB_DEV_EGR_RATE    *val
  )
{
  ARAD_SW_DB_INIT_DEFS;

  if(core == MEM_BLOCK_ALL) {
      core = 0;
  }
  if(core < 0 || core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      return SOC_SAND_ERR;
  }

  ARAD_SW_DB_FIELD_GET(
    unit,
    arad_sw_db_egr_ports.rates[core][base_q_pair],
    val
  );
}

uint32
  arad_sw_db_sch_port_rate_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_OUT uint32            *rate
   )
{

  if(core == MEM_BLOCK_ALL) {
      core = 0;
  }
  if(core < 0 || core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      return SOC_SAND_ERR;
  }
  
  *rate = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.rates[core][base_q_pair].sch_rates;
  return SOC_SAND_OK;
}

uint32
  arad_sw_db_sch_port_rate_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_IN  uint32            rate
   )
{
  uint32 res = SOC_SAND_OK;

  int nof_cores = 1, i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(core < 0 || core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      res = SOC_SAND_ERR;
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  
  for(i=0 ; i < nof_cores ; i++) {
      Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.rates[core + i][base_q_pair].sch_rates = rate;

#ifdef BCM_WARM_BOOT_SUPPORT
        
        if (!SOC_UNIT_NUM_VALID(unit)) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
        }
           
        res = arad_wb_db_egr_port_update_rates_state(unit, core + i, base_q_pair); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_port_rate_set()",0,0);
}

uint32
  arad_sw_db_egq_port_rate_get(
   SOC_SAND_IN   int               unit,
   SOC_SAND_IN   uint32            base_q_pair,
                 int               core,
   SOC_SAND_OUT  uint32           *rate
   )
{
  if(core == MEM_BLOCK_ALL) {
      core = 0;
  }
  if(core < 0 || core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      return SOC_SAND_ERR;
  }
  
  *rate = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.rates[core][base_q_pair].egq_rates;
  return SOC_SAND_OK;
}

uint32
  arad_sw_db_egq_port_rate_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_IN  uint32            rate
   )
{
  uint32 res = SOC_SAND_OK;
  int nof_cores = 1, i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(core < 0 || core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      res = SOC_SAND_ERR;
      SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
  }

  for(i=0 ; i < nof_cores ; i++) {
      Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.rates[core+i][base_q_pair].egq_rates = rate;

#ifdef BCM_WARM_BOOT_SUPPORT
        
        if (!SOC_UNIT_NUM_VALID(unit)) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
        }
           
        res = arad_wb_db_egr_port_update_rates_state(unit, core+i, base_q_pair); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_egq_port_rate_set()",0,0);
}

uint32
  arad_sw_db_is_port_valid_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_OUT uint8             *is_valid
   )
{
  if(core == MEM_BLOCK_ALL) {
      core = 0;
  }
  if(core < 0 || core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      return SOC_SAND_ERR;
  }
  
  *is_valid = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.rates[core][base_q_pair].valid;
  return SOC_SAND_OK;
}

uint32
  arad_sw_db_is_port_valid_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_IN  uint8             is_valid
   )
{
  uint32 res = SOC_SAND_OK;
  int nof_cores = 1, i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(core < 0 || core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      res = SOC_SAND_ERR;
      SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
  }

  for(i=0 ; i< nof_cores ; i++) {
        Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.rates[core+i][base_q_pair].valid = is_valid;

#ifdef BCM_WARM_BOOT_SUPPORT
        if (!SOC_UNIT_NUM_VALID(unit)) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
        }
           
        res = arad_wb_db_egr_port_update_rates_state(unit, core+i, base_q_pair); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_is_port_valid_set()",0,0);
}

uint32
  arad_sw_db_egr_ports_port_priority_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT ARAD_SW_DB_DEV_EGR_PORT_PRIORITY  *val
  )
{
  uint32
    idx;

  for (idx = 0; idx < ARAD_EGR_NOF_Q_PAIRS; ++idx)
  {
    val->queue_rate[idx].valid = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_priority_cal.queue_rate[idx].valid;
    if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_priority_cal.queue_rate[idx].valid)
    {
      val->queue_rate[idx].egq_bursts = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_priority_cal.queue_rate[idx].egq_bursts;
      val->queue_rate[idx].egq_rates = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_priority_cal.queue_rate[idx].egq_rates;
    }
  }
  
  return SOC_SAND_OK;
}

uint32
  arad_sw_db_egr_ports_port_priority_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                q_pair,
    SOC_SAND_IN ARAD_SW_DB_DEV_RATE      *val
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         arad_sw_db_egr_ports.port_priority_cal.queue_rate[q_pair],
                         val
                         );


#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }       
    res = arad_wb_db_egr_port_update_queue_rate_state(unit, q_pair); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_egr_ports_port_priority_set()",0,0);
}

uint32
  arad_sw_db_egr_ports_tcg_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT ARAD_SW_DB_DEV_EGR_TCG   *val
  )
{
  uint32
    ps_idx,
    tcg_idx;

  for (ps_idx = 0; ps_idx < ARAD_EGR_NOF_PS; ++ps_idx)
  {
    for (tcg_idx = 0; tcg_idx < ARAD_NOF_TCGS; ++tcg_idx)
    {
      val->tcg_rate[ps_idx][tcg_idx].valid = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.tcg_cal.tcg_rate[ps_idx][tcg_idx].valid;
      if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.tcg_cal.tcg_rate[ps_idx][tcg_idx].valid)
      {
        val->tcg_rate[ps_idx][tcg_idx].egq_bursts = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.tcg_cal.tcg_rate[ps_idx][tcg_idx].egq_bursts;
        val->tcg_rate[ps_idx][tcg_idx].egq_rates = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.tcg_cal.tcg_rate[ps_idx][tcg_idx].egq_rates;
      }
    }    
  }
  
  return SOC_SAND_OK;
}

uint32
  arad_sw_db_egr_ports_tcg_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                ps,
    SOC_SAND_IN uint32                tcg_ndx,
    SOC_SAND_IN ARAD_SW_DB_DEV_RATE      *val
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         arad_sw_db_egr_ports.tcg_cal.tcg_rate[ps][tcg_ndx],
                         val
                         );


#ifdef BCM_WARM_BOOT_SUPPORT
    
    
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }       
    res = arad_wb_db_egr_port_update_tcg_rate_state(unit, ps, tcg_ndx); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_egr_ports_tcg_set()",0,0);
}

uint32
  arad_sw_db_egr_ports_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ) {
      res = soc_sand_occ_bm_destroy(
              Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  if (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ) {
      res = soc_sand_occ_bm_destroy(
              Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
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
              Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_reassembly_ctxt.reassembly_ctxt_occ
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_egr_ports_terminate()",0,0);
}

uint32
  arad_sw_db_device_lag_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->lag), ARAD_SW_DB_LAGS_INFO, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_device_lag_init()",0,0);
}

uint32
  arad_sw_db_device_tdm_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    tdm_context_map_id,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->tdm), ARAD_SW_DB_TDM, 1);

  for (tdm_context_map_id = 0; tdm_context_map_id < ARAD_NOF_TDM_CONTEXT_MAP; tdm_context_map_id++)
  {
    Arad_sw_db.arad_device_sw_db[unit]->tdm.context_map[tdm_context_map_id] = ARAD_IF_ID_NONE;
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_device_tdm_init()",0,0);
}


uint32
  arad_sw_db_tm_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->tm), ARAD_SW_DB_TM, 1);
  res = soc_dpp_wb_engine_init_buffer(unit,SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TM);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_sw_db_tm_queue_to_rate_class_mapping_is_simple_set(unit, ARAD_SW_DB_QUEUE_TO_RATE_CLASS_MAPPING_IS_UNDEFINE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  /* No init for queue_to_rate_class_mapping_ref_count since this tabele is relevant iff queue_to_rate_class_mapping_is_simple==FALSE */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tm_init()",0,0);
}

uint32
  arad_sw_db_cell_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->cell), ARAD_SW_DB_CELL, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_cell_init()",0,0);
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
  uint32
    res,
    deleted_buf_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->dram), ARAD_SW_DB_DRAM, 1);

  for(deleted_buf_index=0; deleted_buf_index < ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++deleted_buf_index) {
    Arad_sw_db.arad_device_sw_db[unit]->dram.dram_deleted_buff_list[deleted_buf_index] = ARAD_SW_DB_DRAM_DELETED_BUFF_NONE;
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
  SOC_SAND_SORTED_LIST_INFO
    *priorities;
  SOC_SAND_HASH_TABLE_INFO
    *entry_id_to_location;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  restore_data = &(Arad_sw_db.arad_device_sw_db[unit]->tcam.restoration_data);

  for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
  {
    Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[bank_id].valid = FALSE;
  }

  for (tcam_db_id = 0; tcam_db_id < ARAD_TCAM_MAX_NOF_LISTS; ++tcam_db_id)
  {
    Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].valid = FALSE;
  }

  res = soc_dpp_wb_engine_init_buffer(unit,SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_RESTORATION_DATA);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = soc_dpp_wb_engine_init_buffer(unit,SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (SOC_WARM_BOOT(unit)) {
      /* if in warm reboot, reallocate pointers needed for wb restoration of dynamic data */
      for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
      {
          for (is_inverse = 0; is_inverse < 2; ++is_inverse)
          {
              if (restore_data->entries_used[bank_id][is_inverse].is_exist) {
                res = soc_sand_occ_bm_create(
                    &(restore_data->entries_used[bank_id][is_inverse].init_info),
                    &(Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[bank_id].entries_used[is_inverse])
                );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
                Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[bank_id].valid = TRUE;
              }
          }
      }

      for (tcam_db_id = 0; tcam_db_id < ARAD_TCAM_MAX_NOF_LISTS; ++tcam_db_id)
      {
          /*realloc sorted list */
          if (restore_data->priorities[tcam_db_id].is_exist) {
              priorities = &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].priorities);
              soc_sand_SAND_SORTED_LIST_INFO_clear(priorities);
              priorities->init_info = restore_data->priorities[tcam_db_id].init_info;
              priorities->init_info.get_entry_fun = arad_tcam_db_data_structure_entry_get;
              priorities->init_info.set_entry_fun = arad_tcam_db_data_structure_entry_set;
              priorities->init_info.cmp_key_fun   = arad_tcam_db_priority_list_cmp_priority;
              res = soc_sand_sorted_list_create(priorities);
              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
          }
 
          for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
          {
              if (restore_data->db_entries_used[tcam_db_id][bank_id].is_exist) {
                res = soc_sand_occ_bm_create(
                    &(restore_data->db_entries_used[tcam_db_id][bank_id].init_info),
                    &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].entries_used[bank_id])
                );
                SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
                Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].valid = TRUE;
              }
          }
      }


  }
  else{
      sal_memset(&(Arad_sw_db.arad_device_sw_db[unit]->tcam.restoration_data), 0x0, sizeof(ARAD_SW_DB_TCAM_DATA_FOR_RESTORATION));
  }

  /*
   * Initialize the entry_id -> location hash table
   */
  entry_id_to_location = arad_sw_db_tcam_db_entry_id_to_location_get(unit);
  soc_sand_SAND_HASH_TABLE_INFO_clear(entry_id_to_location);
  entry_id_to_location->init_info.prime_handle  = unit;
  entry_id_to_location->init_info.sec_handle    = 0;
  entry_id_to_location->init_info.table_size    = SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit);
  entry_id_to_location->init_info.table_width   = SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit);
  entry_id_to_location->init_info.key_size      = ARAD_TCAM_DB_HASH_TBL_KEY_SIZE * sizeof(uint8);
  entry_id_to_location->init_info.data_size     = ARAD_TCAM_DB_HASH_TBL_DATA_SIZE * sizeof(uint8);
  entry_id_to_location->init_info.get_entry_fun = arad_tcam_db_data_structure_entry_get;
  entry_id_to_location->init_info.set_entry_fun = arad_tcam_db_data_structure_entry_set;
  entry_id_to_location->init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_TCAM_DB_ENTRY_ID_TO_LOCATION;
  res = soc_sand_hash_table_create(entry_id_to_location);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  /*
   * Initialize the DB location table
   */
  ARAD_ALLOC(Arad_sw_db.arad_device_sw_db[unit]->tcam.db_location_tbl, ARAD_TCAM_LOCATION, SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit), "location_tbl arad_tcam_db_create MEM");
  ARAD_ALLOC(Arad_sw_db.arad_device_sw_db[unit]->tcam.global_location_tbl, ARAD_TCAM_GLOBAL_LOCATION, SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit), "global location_tbl arad_tcam_db_create MEM");


  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_OCC_BMS_DYNAMIC);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_DB_BANK_OCC_BMS_DYNAMIC);
  SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);
  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_HASH_TBL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_SORTED_LIST_DYNAMIC);
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_LOCATION_TBL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_GLOBAL_LOCATION_TBL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  ARAD_DO_NOTHING_AND_EXIT;

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
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  Initialize the route_key -> entry_id hash table
   */
  hash_tbl = &Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id;
  soc_sand_SAND_HASH_TABLE_INFO_clear(hash_tbl);
  init_info                = &hash_tbl->init_info;
  init_info->prime_handle  = unit;
  init_info->sec_handle    = 0;
  init_info->table_size    = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS; /* Maximal allowed */
  init_info->table_width   = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS;
  init_info->key_size      = 20;
  init_info->data_size     = sizeof(uint32);
  init_info->get_entry_fun = arad_sw_db_buffer_get_entry;
  init_info->set_entry_fun = arad_sw_db_buffer_set_entry;
  init_info->wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_VTT_ISEM_KEY_TO_ENTRY_ID;

  res = soc_sand_hash_table_create(
          hash_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_vtt_init()", 0, 0);
}

uint32
  arad_sw_db_vsi_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_os_memset(Arad_sw_db.arad_device_sw_db[unit]->vsi.vsi_to_isid,0x0,sizeof(Arad_sw_db.arad_device_sw_db[unit]->vsi.vsi_to_isid));
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
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  Initialize the route_key -> entry_id hash table
   */
  hash_tbl = &Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id;
  soc_sand_SAND_HASH_TABLE_INFO_clear(hash_tbl);
  init_info                = &hash_tbl->init_info;
  init_info->prime_handle  = unit;
  init_info->sec_handle    = 0;
  init_info->table_size    = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS; /* Maximal allowed */
  init_info->table_width   = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS;
  init_info->key_size      = 20;
  init_info->data_size     = sizeof(uint32);
  init_info->get_entry_fun = arad_sw_db_buffer_get_entry;
  init_info->set_entry_fun = arad_sw_db_buffer_set_entry;
  init_info->wb_var_index  = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_FRWRD_IP_ROUTE_KEY_TO_ENTRY_ID;

  res = soc_sand_hash_table_create(
          hash_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  {
      /* Allocate the location table for ACL: index ARAD_KBP_FRWRD_IP_NOF_TABLES */
      ARAD_ALLOC(
          Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.location_tbl, 
          ARAD_SW_KBP_HANDLE, 
          ARAD_KBP_NL_88650_MAX_NOF_ENTRIES + SOC_DPP_DEFS_MAX_NOF_ENTRY_IDS, 
          "alloc fwd location_tbl arad_sw_db_frwrd_ip_init ACL location table MEM"
        );

      
      res = soc_dpp_wb_engine_init_buffer(unit,SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_FRWRD_IP);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40 + ARAD_KBP_FRWRD_IP_NOF_TABLES, exit);
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
    prefix,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
  {
    for (prefix = 0; prefix < ARAD_TCAM_NOF_PREFIXES; ++prefix)
    {
      Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.banks[bank_id].prefix_db[prefix] = ARAD_TCAM_MAX_NOF_LISTS;
    }
    Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.banks[bank_id].nof_dbs = 0;
  }

  res = soc_dpp_wb_engine_init_buffer(unit,SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tcam_mgmt_init()",0,0);
}

/*
 * Interrupts sw db init
*/

uint32
  arad_sw_db_interrupts_init(
    SOC_SAND_IN int unit
  )
{
    int nof_interrupts, rc;
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    rc = soc_nof_interrupts(unit, &nof_interrupts);
    if (rc) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 4, exit);
    }
    sal_memset(&(Arad_sw_db.arad_device_sw_db[unit]->interrupts), 0, sizeof(ARAD_SW_DB_INTERRUPTS));
    ARAD_ALLOC(Arad_sw_db.arad_device_sw_db[unit]->interrupts.interrupt_data, ARAD_SW_DB_INTERRUPT_DATA, nof_interrupts, "interrupt_data");

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_init()",0,0); 
}

uint32 arad_sw_db_interrupts_terminate(
    SOC_SAND_IN int unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(Arad_sw_db.arad_device_sw_db[unit]);
  SOC_SAND_CHECK_NULL_INPUT(Arad_sw_db.arad_device_sw_db[unit]->interrupts.interrupt_data);

  /* deallocate the memory blocks arrays */
  ARAD_FREE(Arad_sw_db.arad_device_sw_db[unit]->interrupts.interrupt_data);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_interrupts_terminate()", 0, 0);
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

  hash_tbl = &(Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id);
  if (hash_tbl->init_info.table_size != 0) {
      res = soc_sand_hash_table_destroy(hash_tbl);
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

  hash_tbl = &(Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id);
  if (hash_tbl->init_info.table_size != 0) {
      res = soc_sand_hash_table_destroy(hash_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  ARAD_FREE(Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.location_tbl);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

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

    sal_memset(Arad_sw_db.arad_device_sw_db[unit]->pmf[stage].psl_info.levels_info,0x0, sizeof(ARAD_SW_DB_PMF_PSL_LEVEL_INFO) * 2 * ARAD_PMF_NOF_LEVELS);
    for(indx = 0; indx < ARAD_PMF_NOF_LEVELS; ++ indx) 
    {
        for(is_tm = 0; is_tm < 2; ++is_tm) 
        {
            Arad_sw_db.arad_device_sw_db[unit]->pmf[stage].psl_info.levels_info[is_tm][indx].level_index = indx;
        }
    }

    return SOC_SAND_OK;
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  for (stage = 0; stage < ARAD_NOF_FP_DATABASE_STAGES; stage ++) {
      ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->pmf[stage]), ARAD_SW_DB_PMF, 1);
      
      res = arad_sw_db_pmf_pls_init(unit, stage);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      for (db_ndx = 0; db_ndx < ARAD_PP_FP_NOF_DBS; ++db_ndx)
      {
        ARAD_PP_FP_DATABASE_INFO_clear(&(Arad_sw_db.arad_device_sw_db[unit]->pmf[stage].fp_info.db_info[db_ndx]));
      }

      for (fem_ndx = 0; fem_ndx < ARAD_PMF_LOW_LEVEL_NOF_FEMS; ++fem_ndx)
      {
        ARAD_PP_FP_FEM_ENTRY_clear(&(Arad_sw_db.arad_device_sw_db[unit]->pmf[stage].fem_entry[fem_ndx]));
      }
  }

  res = soc_dpp_wb_engine_init_buffer(unit,SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

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
          multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_clear()",0,0);
}

/* 
 * set all value of modport2sysport SW DB to invalid.
 */
STATIC uint32 arad_sw_db_modport2sysport_clear(
    SOC_SAND_IN int unit
  )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);

  res = soc_sand_os_memset(&(Arad_sw_db.arad_device_sw_db[unit]->modport2sysport), 0xff, ARAD_MODPORT2SYSPORT_SIZE);
  SOC_SAND_CHECK_FUNC_RESULT(res, ARAD_GEN_ERR_NUM_CLEAR, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_create()", 0, 0);
}

uint32 arad_sw_db_sysport2basequeue_init(SOC_SAND_IN int unit);
uint32 arad_sw_db_sysport2basequeue_terminate(SOC_SAND_IN int unit);

uint32
  arad_sw_db_device_init(
    SOC_SAND_IN int     unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_DEVICE_INIT);

  ARAD_ALLOC_ANY_SIZE(Arad_sw_db.arad_device_sw_db[unit], ARAD_SW_DB_DEVICE, 1,"Arad_sw_db.arad_device_sw_db[unit]");
  res = arad_sw_db_op_mode_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  res = arad_sw_db_dev_egr_ports_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN(res,  70, exit, dpp_mcds_multicast_init(unit));


  res = arad_sw_db_device_lag_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = arad_sw_db_device_tdm_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);


  res = arad_sw_db_cell_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  res = arad_sw_db_dram_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  res = arad_sw_db_tm_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    res = arad_sw_db_tcam_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    res = arad_sw_db_tcam_mgmt_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 183, exit);

    res = arad_sw_db_interrupts_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 183, exit);
    
    res = arad_sw_db_vtt_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 184, exit);

    res = arad_sw_db_vsi_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 185, exit);	

    res = arad_sw_db_frwrd_ip_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 186, exit);

    res = arad_sw_db_pmf_initialize(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

     res = arad_sw_db_cnt_init(unit);
     SOC_SAND_CHECK_FUNC_RESULT(res, 232, exit);

    res = arad_sw_db_modport2sysport_clear(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 252, exit);

    res = arad_sw_db_sysport2basequeue_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 254, exit);

#ifdef BCM_WARM_BOOT_SUPPORT

    
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }   
    res = arad_wb_db_init(unit, Arad_sw_db.arad_device_sw_db[unit]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);


#endif /*BCM_WARM_BOOT_SUPPORT*/
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

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_KBP);
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

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

  res = arad_sw_db_interrupts_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = arad_sw_db_vtt_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_sw_db_frwrd_ip_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  

  res = arad_sw_db_sysport2basequeue_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

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
 * LAGs
 */

uint32
  arad_sw_db_lag_in_use_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                      lag_ndx,
    SOC_SAND_IN uint8                      use
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         lag.in_use[lag_ndx],
                         (&use));

    
#ifdef BCM_WARM_BOOT_SUPPORT
    
    
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }       
    res = arad_wb_db_lag_update_in_use_state(unit, lag_ndx); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_lag_in_use_set()",0,0);
}

uint32
  arad_sw_db_lag_in_use_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                       lag_ndx,
    SOC_SAND_OUT uint8                      *use
  )
{
  ARAD_SW_DB_INIT_DEFS;

  ARAD_SW_DB_FIELD_GET(
    unit,
    lag.in_use[lag_ndx],
    use
  );
}

/*
 * TDM
 */

uint32
  arad_sw_db_tdm_context_map_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                     tdm_context_map_id,
    SOC_SAND_IN ARAD_INTERFACE_ID          if_id
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         tdm.context_map[tdm_context_map_id],
                         (&if_id));

    
#ifdef BCM_WARM_BOOT_SUPPORT
    
    
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }       
    res = arad_wb_db_tdm_update_context_map_state(unit, tdm_context_map_id); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tdm_context_map_set()",0,0);
}

uint32
  arad_sw_db_tdm_context_map_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                      tdm_context_map_id,
    SOC_SAND_OUT ARAD_INTERFACE_ID          *if_id
  )
{
  ARAD_SW_DB_INIT_DEFS;

  ARAD_SW_DB_FIELD_GET(
    unit,
    tdm.context_map[tdm_context_map_id],
    if_id
  );
}

/*
 * Cell
 */

uint32
  arad_sw_db_cell_cell_ident_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                      cell_ident
                                 )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         cell.current_cell_ident,
                         (&cell_ident)
                         );


#ifdef BCM_WARM_BOOT_SUPPORT
    
    
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }       
    res = arad_wb_db_cell_update_current_cell_ident_state(unit); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_cell_cell_ident_set()",0,0);
}

uint32
  arad_sw_db_cell_cell_ident_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint16                      *cell_ident
  )
{
  ARAD_SW_DB_INIT_DEFS;

  ARAD_SW_DB_FIELD_GET(
    unit,
    cell.current_cell_ident,
    cell_ident
  );
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
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32 res = SOC_SAND_OK;
#endif /*BCM_WARM_BOOT_SUPPORT*/
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);   
   
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        if(Arad_sw_db.arad_device_sw_db[unit]->dram.dram_deleted_buff_list[i] == buff) {
            break;
        } else {
            if( (indx_saved == 0) && Arad_sw_db.arad_device_sw_db[unit]->dram.dram_deleted_buff_list[i] == ARAD_SW_DB_DRAM_DELETED_BUFF_NONE) {
                saved_indx = i;
                indx_saved = 1;
            }
        }        
    }
 
    if(indx_saved == 1) {
        Arad_sw_db.arad_device_sw_db[unit]->dram.dram_deleted_buff_list[saved_indx] = buff;
    } 
#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    
    res = arad_wb_db_dram_update_dram_deleted_buff_list_state(unit, saved_indx); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
#endif /*BCM_WARM_BOOT_SUPPORT*/ 
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_dram_deleted_buff_list_add()",0,0);
}

uint32
  arad_sw_db_dram_deleted_buff_list_remove(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff
  )
{
    int i;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32 res = SOC_SAND_OK;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        if(Arad_sw_db.arad_device_sw_db[unit]->dram.dram_deleted_buff_list[i] == buff) {
            Arad_sw_db.arad_device_sw_db[unit]->dram.dram_deleted_buff_list[i] =  ARAD_SW_DB_DRAM_DELETED_BUFF_NONE;
            break;
        }      
    }    
#ifdef BCM_WARM_BOOT_SUPPORT
    if(i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR) {
        
    
        if (!SOC_UNIT_NUM_VALID(unit)) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
        }
        
        res = arad_wb_db_dram_update_dram_deleted_buff_list_state(unit, i); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }   
exit:
#endif /*BCM_WARM_BOOT_SUPPORT*/ 
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_dram_deleted_buff_list_remove()",0,0);
}

uint32
  arad_sw_db_dram_deleted_buff_list_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff,
    SOC_SAND_OUT uint32*     is_deleted
  )
{
    int i;
    
    *is_deleted = 0;
    
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        if(Arad_sw_db.arad_device_sw_db[unit]->dram.dram_deleted_buff_list[i] == buff) {
            *is_deleted = 1;
            break;
        }      
    }    
    
    return SOC_E_NONE;
}

uint32 
  arad_sw_db_dram_deleted_buff_list_get_all(
    SOC_SAND_IN int    unit,
    SOC_SAND_OUT uint32*    buff_list_arr,
    SOC_SAND_IN uint32      arr_size,
    SOC_SAND_OUT uint32*    buff_list_num)
{
    int i;
    *buff_list_num = 0;
    
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        if(Arad_sw_db.arad_device_sw_db[unit]->dram.dram_deleted_buff_list[i] != ARAD_SW_DB_DRAM_DELETED_BUFF_NONE) {
            buff_list_arr[*buff_list_num] = Arad_sw_db.arad_device_sw_db[unit]->dram.dram_deleted_buff_list[i];
            ++(*buff_list_num);
            if(*buff_list_num == arr_size) {
                break; 
            }
        }      
    }
    
    return SOC_E_NONE;

}


/*
 * Interrupts
*/

/* The following 6 functions are not used and remain for future use */
uint32 arad_sw_db_interrupts_cmc_irq2_mask_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_IN uint32                       val
  )
{
#ifdef BCM_WARM_BOOT_SUPPORT
  uint32 res = SOC_SAND_OK;
#endif /*BCM_WARM_BOOT_SUPPORT*/

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(cmc, SOC_CMCS_NUM_MAX-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);  

  if ((!Arad_sw_db_initialized) || (Arad_sw_db.arad_device_sw_db[unit] == NULL)) {
      SOC_EXIT;
  }
  Arad_sw_db.arad_device_sw_db[unit]->interrupts.cmc_irq2_mask[cmc] = val;

#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    
    res = arad_wb_db_interrupts_update_cmc_irq2_mask_state(unit, cmc); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_cmc_irq2_mask_set()",0,0);
}

uint32 arad_sw_db_interrupts_cmc_irq2_mask_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_OUT uint32*                     val
  )
{
  ARAD_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(val);

  SOC_SAND_ERR_IF_ABOVE_MAX(cmc, SOC_CMCS_NUM_MAX-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);   

  ARAD_SW_DB_FIELD_GET(unit, interrupts.cmc_irq2_mask[cmc], val); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_cmc_irq2_mask_get()",0,0); 
}
  
uint32 arad_sw_db_interrupts_cmc_irq3_mask_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_IN uint32                       val
  )
{
#ifdef BCM_WARM_BOOT_SUPPORT
  uint32 res = SOC_SAND_OK;
#endif /*BCM_WARM_BOOT_SUPPORT*/

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(cmc, SOC_CMCS_NUM_MAX-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);  

  if ((!Arad_sw_db_initialized) || (Arad_sw_db.arad_device_sw_db[unit] == NULL)) {
      SOC_EXIT;
  }
  Arad_sw_db.arad_device_sw_db[unit]->interrupts.cmc_irq3_mask[cmc] = val;

#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    
    res = arad_wb_db_interrupts_update_cmc_irq3_mask_state(unit, cmc); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_cmc_irq3_mask_set()",0,0);
}

uint32 arad_sw_db_interrupts_cmc_irq3_mask_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_OUT uint32*                     val
  )
{
  ARAD_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(val);

  SOC_SAND_ERR_IF_ABOVE_MAX(cmc, SOC_CMCS_NUM_MAX-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);   

  ARAD_SW_DB_FIELD_GET(unit, interrupts.cmc_irq3_mask[cmc], val); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_cmc_irq3_mask_get()",0,0); 
}
  
  
uint32 arad_sw_db_interrupts_cmc_irq4_mask_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_IN uint32                       val
  )
{
#ifdef BCM_WARM_BOOT_SUPPORT
  uint32 res = SOC_SAND_OK;
#endif /*BCM_WARM_BOOT_SUPPORT*/

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(cmc, SOC_CMCS_NUM_MAX-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);  

  if ((!Arad_sw_db_initialized) || (Arad_sw_db.arad_device_sw_db[unit] == NULL)) {
      SOC_EXIT;
  }
  Arad_sw_db.arad_device_sw_db[unit]->interrupts.cmc_irq4_mask[cmc] = val;

#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    
    res = arad_wb_db_interrupts_update_cmc_irq4_mask_state(unit, cmc); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_cmc_irq4_mask_set()",0,0);
}


uint32 arad_sw_db_interrupts_cmc_irq4_mask_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_OUT uint32*                     val
  )
{
  ARAD_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(val);

  SOC_SAND_ERR_IF_ABOVE_MAX(cmc, SOC_CMCS_NUM_MAX-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);   

  ARAD_SW_DB_FIELD_GET(unit, interrupts.cmc_irq4_mask[cmc], val); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_cmc_irq4_mask_get()",0,0); 
}

uint32
  arad_sw_db_interrupts_flags_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_IN uint32                      val 
  )
{
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32 res = SOC_SAND_OK;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    int nof_interrupts, rc;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    if (rc) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 4, exit);
    }
    SOC_SAND_ERR_IF_ABOVE_MAX(interrupt_id, nof_interrupts-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);  

    Arad_sw_db.arad_device_sw_db[unit]->interrupts.interrupt_data[interrupt_id].flags = val;

#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    
    res = arad_wb_db_interrupts_update_interrupt_flags_state(unit, interrupt_id); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_flags_set()",0,0); 
}
uint32
  arad_sw_db_interrupts_flags_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_OUT uint32*                      val
  )
{
    int nof_interrupts, rc;
    ARAD_SW_DB_INIT_DEFS;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(val);

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    if (rc) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 4, exit);
    }
    SOC_SAND_ERR_IF_ABOVE_MAX(interrupt_id, nof_interrupts-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);  

    ARAD_SW_DB_FIELD_GET(unit, interrupts.interrupt_data[interrupt_id].flags, val); 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_flags_get()",0,0); 

}

uint32
  arad_sw_db_interrupts_storm_timed_count_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_IN uint32                      val 
  )
{
#ifdef BCM_WARM_BOOT_SUPPORT
  uint32 res = SOC_SAND_OK;
#endif /*BCM_WARM_BOOT_SUPPORT*/
  int nof_interrupts, rc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rc = soc_nof_interrupts(unit, &nof_interrupts);
  if (rc) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 4, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(interrupt_id, nof_interrupts-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);  

  Arad_sw_db.arad_device_sw_db[unit]->interrupts.interrupt_data[interrupt_id].storm_timed_count = val;

#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    
    res = arad_wb_db_interrupts_update_interrupt_storm_timed_count_state(unit, interrupt_id); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_storm_timed_count_set()",0,0); 
  
}
uint32
  arad_sw_db_interrupts_storm_timed_count_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_OUT uint32*                      val 
  )
{
  ARAD_SW_DB_INIT_DEFS;
  int nof_interrupts, rc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rc = soc_nof_interrupts(unit, &nof_interrupts);
  if (rc) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 4, exit);
  }
  
  SOC_SAND_CHECK_NULL_INPUT(val);

  SOC_SAND_ERR_IF_ABOVE_MAX(interrupt_id, nof_interrupts-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);  

  ARAD_SW_DB_FIELD_GET(unit, interrupts.interrupt_data[interrupt_id].storm_timed_count , val); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_storm_timed_count_get()",0,0); 
  
}

uint32
  arad_sw_db_interrupts_storm_timed_period_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_IN uint32                      val 
  )
{
#ifdef BCM_WARM_BOOT_SUPPORT
  uint32 res = SOC_SAND_OK;
#endif /*BCM_WARM_BOOT_SUPPORT*/
  int nof_interrupts, rc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rc = soc_nof_interrupts(unit, &nof_interrupts);
  if (rc) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 4, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(interrupt_id, nof_interrupts-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);  

  Arad_sw_db.arad_device_sw_db[unit]->interrupts.interrupt_data[interrupt_id].storm_timed_period = val;

#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    
    res = arad_wb_db_interrupts_update_interrupt_storm_timed_period_state(unit, interrupt_id); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_storm_timed_period_set()",0,0); 
  
}

uint32
  arad_sw_db_interrupts_storm_timed_period_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_OUT uint32*                      val 
  )
{
    ARAD_SW_DB_INIT_DEFS;
    int nof_interrupts, rc;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    if (rc) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 4, exit);
    }
    SOC_SAND_CHECK_NULL_INPUT(val);

    SOC_SAND_ERR_IF_ABOVE_MAX(interrupt_id, nof_interrupts-1, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 100, exit);  

    ARAD_SW_DB_FIELD_GET(unit, interrupts.interrupt_data[interrupt_id].storm_timed_period , val); 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_storm_timed_period_get()",0,0); 
}

uint32
  arad_sw_db_op_mode_init(
    SOC_SAND_IN int unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_REVISION_INIT);
  Arad_sw_db.arad_device_sw_db[unit]->op_mode.is_petrab_in_system = FALSE;
  Arad_sw_db.arad_device_sw_db[unit]->op_mode.tdm_mode = ARAD_MGMT_TDM_MODE_PACKET;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_op_mode_init()",0,0);
}


void
  arad_sw_db_is_petrab_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_petrab_in_system
  )
{
  Arad_sw_db.arad_device_sw_db[unit]->op_mode.is_petrab_in_system = is_petrab_in_system;
}

uint8
  arad_sw_db_is_petrab_in_system_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->op_mode.is_petrab_in_system;
}

void
  arad_sw_db_tdm_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_MGMT_TDM_MODE tdm_mode
  )
{
  Arad_sw_db.arad_device_sw_db[unit]->op_mode.tdm_mode = tdm_mode;
}

ARAD_MGMT_TDM_MODE
  arad_sw_db_tdm_mode_get(
    SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->op_mode.tdm_mode;
}

void
  arad_sw_db_ilkn_tdm_dedicated_queuing_set(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing
  )
{
  Arad_sw_db.arad_device_sw_db[unit]->op_mode.ilkn_tdm_dedicated_queuing = ilkn_tdm_dedicated_queuing;
}

ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
  arad_sw_db_ilkn_tdm_dedicated_queuing_get(
     SOC_SAND_IN int unit
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->op_mode.ilkn_tdm_dedicated_queuing;
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

uint32
  arad_sw_db_tcam_bank_valid_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8  valid
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_VALID, &valid, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_bank_valid_set()",0,0);
}

uint32
  arad_sw_db_tcam_bank_valid_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint8  *valid
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_VALID, valid, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_bank_valid_get()",0,0);
}

uint32
  arad_sw_db_tcam_bank_has_direct_table_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8  has_direct_table
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_HAS_DIRECT_TABLE, &has_direct_table, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_bank_has_direct_table_set()",0,0);
}

uint32
  arad_sw_db_tcam_bank_has_direct_table_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint8  *has_direct_table
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_HAS_DIRECT_TABLE, has_direct_table, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_bank_has_direct_table_get()",0,0);
}

uint32
  arad_sw_db_tcam_bank_entry_size_set(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                    bank_id,
    SOC_SAND_IN ARAD_TCAM_BANK_ENTRY_SIZE entry_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRY_SIZE, &entry_size, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_bank_entry_size_set()",0,0);
}

uint32
  arad_sw_db_tcam_bank_entry_size_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    bank_id,
    SOC_SAND_OUT ARAD_TCAM_BANK_ENTRY_SIZE *entry_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRY_SIZE, entry_size, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_bank_entry_size_get()",0,0);
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
  Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[bank_id].entries_used[is_inverse] = entries_used;
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_tcam_bank_entries_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8  is_inverse
  )
{
  /* warmboot is handled at init*/
  return Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[bank_id].entries_used[is_inverse];
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
  Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].entries_used[bank_id] = entries_used;
}

SOC_SAND_OCC_BM_PTR
  arad_sw_db_tcam_db_entries_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id
  )
{
  /* warmboot is handled at init*/
  return Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].entries_used[bank_id];
}

uint32
  arad_sw_db_tcam_bank_nof_entries_free_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 nof_entries_free
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_NOF_ENTRIES_FREE, &nof_entries_free, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_bank_nof_entries_free_set()",0,0);
}
uint32
  arad_sw_db_tcam_bank_nof_entries_free_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint32 *nof_entries_free
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_NOF_ENTRIES_FREE, nof_entries_free, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_bank_nof_entries_free_get()",0,0);
}
uint32
  arad_sw_db_tcam_db_valid_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8  valid
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_VALID, &valid, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_valid_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_valid_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_OUT uint8  *valid
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_VALID, valid, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_valid_get()",0,0);
}

uint32
  arad_sw_db_tcam_db_is_direct_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8  is_direct
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_IS_DIRECT, &is_direct, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_is_direct_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_is_direct_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_OUT uint8 *is_direct
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_IS_DIRECT, is_direct, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_is_direct_get()",0,0);
}

uint32
  arad_sw_db_tcam_db_entry_size_set(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                    tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_BANK_ENTRY_SIZE entry_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_ENTRY_SIZE, &entry_size, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_entry_size_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_entry_size_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_BANK_ENTRY_SIZE *entry_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_ENTRY_SIZE, entry_size, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_entry_size_get()",0,0);
}

uint32
  arad_sw_db_tcam_db_action_bitmap_ndx_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_ACTION_SIZE action_bitmap_ndx
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_ACTION_BITMAP_NDX, &action_bitmap_ndx, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_action_bitmap_ndx_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_action_bitmap_ndx_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_ACTION_SIZE *action_bitmap_ndx
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_ACTION_BITMAP_NDX, action_bitmap_ndx, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_action_bitmap_ndx_get()",0,0);
}

uint32
  arad_sw_db_tcam_db_prefix_size_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 prefix_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_PREFIX_SIZE, &prefix_size, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_prefix_size_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_prefix_size_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_OUT uint32 *prefix_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_PREFIX_SIZE, prefix_size, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_prefix_size_get()",0,0);
}

uint32
  arad_sw_db_tcam_db_bank_used_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8  is_used
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_USED, &is_used, tcam_db_id, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_bank_used_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_bank_used_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint8  *is_used
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_USED, is_used, tcam_db_id, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_bank_used_get()",0,0);
}

uint32
  arad_sw_db_tcam_use_small_banks_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_SMALL_BANKS use_small_banks
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_USE_SMALL_BANKS, &use_small_banks, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_use_small_banks_set()",0,0);
}

uint32
  arad_sw_db_tcam_use_small_banks_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_SMALL_BANKS *use_small_banks
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_USE_SMALL_BANKS, use_small_banks, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_use_small_banks_get()",0,0);
}

uint32
  arad_sw_db_tcam_no_insertion_priority_order_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8  no_insertion_priority_order
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_NO_INSERTION_PRIORITY_ORDER, &no_insertion_priority_order, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_no_insertion_priority_order_set()",0,0);
}

uint32
  arad_sw_db_tcam_no_insertion_priority_order_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_OUT uint8  *no_insertion_priority_order
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_NO_INSERTION_PRIORITY_ORDER, no_insertion_priority_order, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_no_insertion_priority_order_get()",0,0);
}

uint32
  arad_sw_db_tcam_sparse_priorities_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8  sparse_priorities
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_SPARSE_PRIORITIES, &sparse_priorities, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_sparse_priorities_set()",0,0);
}

uint32
  arad_sw_db_tcam_sparse_priorities_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_OUT uint8  *sparse_priorities
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_SPARSE_PRIORITIES, sparse_priorities, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_sparse_priorities_get()",0,0);
}

uint32
  arad_sw_db_tcam_db_bank_nof_entries_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 bank_nof_entries
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_NOF_ENTRIES, &bank_nof_entries, tcam_db_id, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_bank_nof_entries_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_bank_nof_entries_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint32 *bank_nof_entries
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_NOF_ENTRIES, bank_nof_entries, tcam_db_id, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_bank_nof_entries_get()",0,0);
}

uint32
  arad_sw_db_tcam_db_access_profile_id_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 access_profile_array_id,
    SOC_SAND_IN uint32 access_profile_id    
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_ACCESS_PROFILE_ID, &access_profile_id, tcam_db_id, access_profile_array_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_access_profile_id_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_access_profile_id_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_IN  uint32 access_profile_array_id,
    SOC_SAND_OUT uint32 *access_profile_id
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_ACCESS_PROFILE_ID, access_profile_id, tcam_db_id, access_profile_array_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_access_profile_id_get()",0,0);
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
    uint32 forbidden_dbs[ARAD_BIT_TO_U32(ARAD_TCAM_MAX_NOF_LISTS)];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_FORBIDDEN_DBS, forbidden_dbs, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    if (is_forbidden) {
        SHR_BITSET(forbidden_dbs, tcam_db_other);
    }
    else {
        SHR_BITCLR(forbidden_dbs, tcam_db_other);
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_FORBIDDEN_DBS, forbidden_dbs, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
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
    uint32
        forbidden_dbs[ARAD_BIT_TO_U32(ARAD_TCAM_MAX_NOF_LISTS)];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_FORBIDDEN_DBS, forbidden_dbs, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    *is_forbidden  = soc_sand_bitstream_test_bit(forbidden_dbs, tcam_db_other);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_forbidden_dbs_get()",0,0);
}

uint32
  arad_sw_db_tcam_db_prefix_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_PREFIX *prefix
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_PREFIX, prefix, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_prefix_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_prefix_get(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint32      tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_PREFIX *prefix
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_PREFIX, prefix, tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_prefix_get()",0,0);
}

SOC_SAND_SORTED_LIST_INFO *
  arad_sw_db_tcam_db_priorities_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return &Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].priorities;
}

SOC_SAND_HASH_TABLE_INFO *
  arad_sw_db_tcam_db_entry_id_to_location_get(
    SOC_SAND_IN int unit
  )
{
  return &Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location;
}


uint32
  arad_sw_db_tcam_db_location_tbl_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32             location_tbl_ndx,
    SOC_SAND_IN ARAD_TCAM_LOCATION *location
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_LOCATION_TABLE, location, location_tbl_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_location_tbl_set()",0,0);
}

uint32 
  arad_sw_db_tcam_db_location_tbl_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             location_tbl_ndx,
    SOC_SAND_OUT ARAD_TCAM_LOCATION *location
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_LOCATION_TABLE, location, location_tbl_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_location_tbl_get()",0,0);
}

uint32
  arad_sw_db_tcam_global_location_tbl_set(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                    location_tbl_ndx,
    SOC_SAND_IN ARAD_TCAM_GLOBAL_LOCATION *location
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_GLOBAL_LOCATION_TABLE, location, location_tbl_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_global_location_tbl_set()",0,0);
}

uint32 
  arad_sw_db_tcam_global_location_tbl_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    location_tbl_ndx,
    SOC_SAND_OUT ARAD_TCAM_GLOBAL_LOCATION *location
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_GLOBAL_LOCATION_TABLE, location, location_tbl_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_global_location_tbl_get()",0,0);
}


uint32
  arad_sw_db_tcam_access_profile_valid_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint8  valid
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_VALID, &valid, profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_access_profile_valid_set()",0,0);
}

uint32
  arad_sw_db_tcam_access_profile_valid_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 profile,
    SOC_SAND_OUT uint8  *valid
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_VALID, valid, profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_access_profile_valid_get()",0,0);
}

uint32
  arad_sw_db_tcam_access_profile_min_banks_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 min_banks
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_MIN_BANKS, &min_banks, profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_access_profile_min_banks_set()",0,0);
}

uint32
  arad_sw_db_tcam_access_profile_min_banks_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 profile,
    SOC_SAND_OUT uint32 *min_banks
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_MIN_BANKS, min_banks, profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_access_profile_min_banks_get()",0,0);
}

uint32
  arad_sw_db_tcam_access_profile_bank_owner_set(
    SOC_SAND_IN int                  unit,
    SOC_SAND_IN uint32               profile,
    SOC_SAND_IN ARAD_TCAM_BANK_OWNER bank_owner
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_BANK_OWNER, &bank_owner, profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_access_profile_bank_owner_set()",0,0);
}

uint32
  arad_sw_db_tcam_access_profile_bank_owner_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32               profile,
    SOC_SAND_OUT ARAD_TCAM_BANK_OWNER *bank_owner
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_BANK_OWNER, bank_owner, profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_access_profile_bank_owner_get()",0,0);
}

void
  arad_sw_db_tcam_access_profile_callback_set(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint32           profile,
    SOC_SAND_IN ARAD_TCAM_MGMT_SIGNAL callback
  )
{
  /* not restored in wb, need to be re-set externally*/
  Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.profiles[profile].callback = callback;
}

ARAD_TCAM_MGMT_SIGNAL
  arad_sw_db_tcam_access_profile_callback_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  )
{
  /* not restored in wb, need to be re-set externally*/
  return Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.profiles[profile].callback;
}

uint32
  arad_sw_db_tcam_access_profile_user_data_set(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  profile,
    SOC_SAND_IN uint32  user_data
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_USER_DATA, &user_data, profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_access_profile_user_data_set()",0,0);
}

uint32
  arad_sw_db_tcam_access_profile_user_data_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 profile,
    SOC_SAND_OUT uint32 *user_data
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_USER_DATA, user_data, profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_access_profile_user_data_get()",0,0);
}
uint32
  arad_sw_db_tcam_managed_bank_prefix_db_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 prefix,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_MANAGED_BANK_PREFIX_DB, &tcam_db_id, bank_id, prefix);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_managed_bank_prefix_db_set()",0,0);
}

uint32
  arad_sw_db_tcam_managed_bank_prefix_db_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_IN  uint32 prefix,
    SOC_SAND_OUT uint32 *tcam_db_id
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_MANAGED_BANK_PREFIX_DB, tcam_db_id, bank_id, prefix);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_managed_bank_prefix_db_get()",0,0);
}

uint32
  arad_sw_db_tcam_managed_bank_nof_db_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 nof_dbs
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_MANAGED_BANK_NOF_DB, &nof_dbs, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_managed_bank_nof_db_set()",0,0);
}

uint32
  arad_sw_db_tcam_managed_bank_nof_db_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint32 *nof_dbs
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_MANAGED_BANK_NOF_DB, nof_dbs, bank_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_managed_bank_nof_db_get()",0,0);
}
SOC_SAND_HASH_TABLE_INFO *
  arad_sw_db_vtt_isem_key_to_entry_id_get(
    SOC_SAND_IN int unit
  )
{
  return &Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id;
}

uint32
  arad_sw_db_vsi_isid_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 vsi,
    SOC_SAND_IN uint32 isid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_SW_DB_FIELD_SET(res, 
                       unit,
                       vsi.vsi_to_isid[vsi],
                       &isid
                       );


#ifdef BCM_WARM_BOOT_SUPPORT
    
  
  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }
  
  res = arad_wb_db_vsi_update_isid_state(unit,vsi); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_vsi_isid_set()",0,0);
}

uint32
  arad_sw_db_vsi_isid_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 vsi
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->vsi.vsi_to_isid[vsi];
}

SOC_SAND_HASH_TABLE_INFO *
  arad_sw_db_frwrd_ip_route_key_to_entry_id_get(
    SOC_SAND_IN int unit
  )
{
  return &Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id;
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
uint32
  arad_sw_db_frwrd_ip_kbp_location_tbl_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN  uint32                  data_index,
    SOC_SAND_OUT ARAD_SW_KBP_HANDLE      *location
  )
{
      uint32 res = SOC_SAND_OK;

      SOC_SAND_INIT_ERROR_DEFINITIONS(0);

      res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_LOCATION_TABLE, location, data_index);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  exit:
      SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_frwrd_ip_kbp_location_tbl_get()",0,0);
}

uint32
  arad_sw_db_frwrd_ip_kbp_location_tbl_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN  uint32                  data_index,
    SOC_SAND_IN ARAD_SW_KBP_HANDLE      *location
  )
{
      uint32 res = SOC_SAND_OK;

      SOC_SAND_INIT_ERROR_DEFINITIONS(0);

      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_LOCATION_TABLE, location, data_index);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  exit:
      SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_frwrd_ip_kbp_location_tbl_set()",0,0);
}

uint32
  arad_sw_db_frwrd_ip_kbp_cache_mode_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint32 *val
  )
{
    uint32 res = SOC_SAND_OK;
    uint8 cache_mode;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_CACHE_MODE, &cache_mode);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    *val = cache_mode;

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_frwrd_ip_kbp_cache_mode_get()",0,0);
}

uint32
  arad_sw_db_frwrd_ip_kbp_cache_mode_set(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32  val
  )
{
    uint32 res = SOC_SAND_OK;
    uint8 cache_mode = val ? 1 : 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_CACHE_MODE, &cache_mode);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_frwrd_ip_kbp_cache_mode_set()",0,0);
}
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */


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

          if(Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[bank_id].valid)
      {
        for (is_inverse = FALSE; is_inverse <= TRUE; is_inverse++) {
            res = soc_sand_occ_bm_destroy(Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[bank_id].entries_used[is_inverse]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
      }
  }

  for (tcam_db_id=0; tcam_db_id < ARAD_TCAM_MAX_NOF_LISTS; tcam_db_id++)
  {
    if(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].valid)
    {
      res = soc_sand_sorted_list_destroy(
        &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].priorities)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
      for (bank_id=0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); bank_id++)
      {
          if(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].bank_nof_entries[bank_id])
          {
              res = soc_sand_occ_bm_destroy(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].entries_used[bank_id]);
              SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          }
      }
    }
  }

  hash_tbl = &(Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location);
  if (hash_tbl->init_info.table_size != 0) {
      res = soc_sand_hash_table_destroy(hash_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  ARAD_FREE(Arad_sw_db.arad_device_sw_db[unit]->tcam.db_location_tbl);
  ARAD_FREE(Arad_sw_db.arad_device_sw_db[unit]->tcam.global_location_tbl);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tcam_terminate()",0,0);
}



uint32
  arad_sw_db_pgm_ce_instr_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             prog_ndx,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             ce_ndx,
    SOC_SAND_IN  ARAD_SW_DB_PMF_CE  *ce_instr
  )
{
  uint32 res = SOC_SAND_OK;
  ARAD_SW_DB_PMF_CE pgm_ce[ARAD_SW_DB_PMF_NOF_CYCLES][ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_CE, pgm_ce, stage, prog_ndx);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  pgm_ce[cycle_ndx][ce_ndx] = *ce_instr;

  res = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_CE, pgm_ce, stage, prog_ndx);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_ce_instr_set()",0,0);

}

uint32
  arad_sw_db_pgm_ce_instr_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_IN  uint32                 cycle_ndx,
    SOC_SAND_IN  uint32                 ce_ndx,
    SOC_SAND_OUT ARAD_SW_DB_PMF_CE      *ce_instr
  )
{
  uint32 res = SOC_SAND_OK;
  ARAD_SW_DB_PMF_CE pgm_ce[ARAD_SW_DB_PMF_NOF_CYCLES][ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_CE, pgm_ce, stage, prog_ndx);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  *ce_instr = pgm_ce[cycle_ndx][ce_ndx];

  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_ce_instr_get()",0,0);

}

uint32
  arad_sw_db_pgm_fes_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             prog_ndx,
    SOC_SAND_IN  uint32             fes_ndx,
    SOC_SAND_IN  ARAD_SW_DB_PMF_FES  *fes
  )
{
    uint32 res = SOC_SAND_OK;
    ARAD_SW_DB_PMF_FES pgm_fes[ARAD_SW_DB_PMF_NOF_FES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_FES, pgm_fes, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    pgm_fes[fes_ndx] = *fes;

    res = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_FES, pgm_fes, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_fes_set()",0,0);
}

uint32
  arad_sw_db_pgm_fes_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_IN  uint32                 fes_ndx,
    SOC_SAND_OUT ARAD_SW_DB_PMF_FES      *fes
  )
{
    uint32 res = SOC_SAND_OK;
    ARAD_SW_DB_PMF_FES pgm_fes[ARAD_SW_DB_PMF_NOF_FES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_FES, pgm_fes, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    *fes = pgm_fes[fes_ndx];

    

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_fes_get()",0,0);
}


uint32
  arad_sw_db_pgm_all_fes_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_OUT ARAD_SW_DB_PMF_FES     fes_arr[ARAD_PMF_LOW_LEVEL_NOF_FESS]
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_FES, fes_arr, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pgm_all_fes_get()",0,0);
}

uint32
  arad_sw_db_fp_fem_entry_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                fem_ndx,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY  *fem_entry
  )
{
    soc_error_t
        rv;
    ARAD_FP_DATABASE_STAGE
      stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; /* Single stage with FEMs */
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FEM_ENTRY, fem_entry, stage, fem_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_fem_entry_set()",0,0);
}

uint32
  arad_sw_db_fp_fem_entry_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             fem_ndx,
    SOC_SAND_OUT ARAD_PP_FP_FEM_ENTRY   *fem_entry
  )
{
    ARAD_FP_DATABASE_STAGE
        stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; /* Single stage with FEMs */
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FEM_ENTRY, fem_entry, stage, fem_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_fem_entry_get()",0,0);
}


/*
 * PMF resource sattus API
 */
uint32
  arad_sw_db_pgm_ce_rsrc_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             prog_ndx,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             *ce_rsrc
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 ce[ARAD_SW_DB_PMF_NOF_CYCLES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);



    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_CE_RSRC, ce, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    ce[cycle_ndx] = *ce_rsrc;

    res = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_CE_RSRC, ce, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_ce_rsrc_set()",0,0);
}

uint32
  arad_sw_db_pgm_ce_rsrc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_IN  uint32                 cycle_ndx,
    SOC_SAND_OUT uint32                 *ce_rsrc
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 ce[ARAD_SW_DB_PMF_NOF_CYCLES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);



    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_CE_RSRC, ce, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    *ce_rsrc = ce[cycle_ndx];

    

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_ce_rsrc_get()",0,0);
}


uint32
  arad_sw_db_pgm_key_rsrc_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             prog_ndx,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             *key_rsrc
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 key[ARAD_SW_DB_PMF_NOF_CYCLES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);



    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_KEY_RSRC, key, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    key[cycle_ndx] = *key_rsrc;

    res = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_KEY_RSRC, key, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_key_rsrc_set()",0,0);
}

uint32
  arad_sw_db_pgm_key_rsrc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_IN  uint32                 cycle_ndx,
    SOC_SAND_OUT uint32                 *key_rsrc
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 key[ARAD_SW_DB_PMF_NOF_CYCLES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);



    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_KEY_RSRC, key, stage, prog_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    *key_rsrc = key[cycle_ndx];

    

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_key_rsrc_get()",0,0);
}

uint32
  arad_sw_db_pgm_program_rsrc_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             *program_rsrc
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 progs[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_PROGS)];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_PROGRAM_RSRC, progs, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    progs[0] = *program_rsrc;

    res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_PROGRAM_RSRC, progs, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_program_rsrc_set()",0,0);
}

uint32
  arad_sw_db_pgm_program_rsrc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_OUT uint32                 *program_rsrc
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 progs[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_PROGS)];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_PROGRAM_RSRC, progs, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    *program_rsrc = progs[0];

    

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pgm_program_rsrc_get()",0,0);
}


uint32
  arad_sw_db_pmf_db_info_set(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                      db_ndx,
    SOC_SAND_IN  ARAD_SW_DB_PMF_DB_INFO      *db_info
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_DB_INFO, db_info, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_db_info_set()",0,0);
}

uint32
  arad_sw_db_pmf_db_info_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                      db_ndx,
    SOC_SAND_OUT ARAD_SW_DB_PMF_DB_INFO      *db_info
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_DB_INFO, db_info, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_db_info_get()",0,0);
}

uint32
  arad_sw_db_pmf_db_prio_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             db_ndx,
    SOC_SAND_OUT uint32             *prio
  )
{
    ARAD_SW_DB_PMF_DB_INFO
        db_info;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_DB_INFO, &db_info, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    *prio = db_info.prio;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_db_prio_get()",0,0);
}

uint32
  arad_sw_db_pmf_db_prog_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  uint32                    db_ndx,
    SOC_SAND_OUT uint32                    *exist_progs
)
{
    ARAD_SW_DB_PMF_DB_INFO
        db_info;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_DB_INFO, &db_info, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    /* assuming number of programs is 32*/
    *exist_progs = *(db_info.progs);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_db_prog_get()",0,0);
}


uint32
  arad_sw_db_pmf_db_prog_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_IN  uint32                   db_ndx,
    SOC_SAND_IN uint32                    exist_progs
)
{
    soc_error_t
        rv;
    ARAD_SW_DB_PMF_DB_INFO
        db_info;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_DB_INFO, &db_info, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    /* assuming number of programs is 32*/
    *(db_info.progs) = exist_progs;

    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_DB_INFO, &db_info, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_db_prog_set()",0,0);
}

uint32
  arad_sw_db_pmf_db_prog_add(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_IN  uint32                   db_ndx,
    SOC_SAND_IN uint32                    prog_id
)
{
    soc_error_t
        rv;
    ARAD_SW_DB_PMF_DB_INFO
        db_info;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_DB_INFO, &db_info, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    /* assuming number of programs is 32*/
    SHR_BITSET(db_info.progs, prog_id);

    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_DB_INFO, &db_info, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_db_prog_add()",0,0);
}

uint32
  arad_sw_db_pmf_psl_pgm_dbs_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_IN  uint32                   pgm_ndx,
    SOC_SAND_IN uint32                    pgms_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_DB_PMB, pgms_db_pmb, stage, pgm_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_pgm_dbs_set()",0,0);
}

uint32
  arad_sw_db_pmf_psl_pgm_dbs_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_IN  uint32                   pgm_ndx,
    SOC_SAND_OUT uint32                   pgms_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_DB_PMB, pgms_db_pmb, stage, pgm_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_pgm_dbs_get()",0,0);
}


/*
 * for new PFG what are the DBs.
 */
uint32
  arad_sw_db_pmf_psl_pfg_dbs_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_IN  uint32                   pfg_ndx,
    SOC_SAND_IN uint32                    pfgs_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_PFG_DBS, pfgs_db_pmb, stage, pfg_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_pfg_dbs_set()",0,0);
}

uint32
  arad_sw_db_pmf_psl_pfg_dbs_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_IN  uint32                   pfg_ndx,
    SOC_SAND_OUT uint32                   pfgs_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_PFG_DBS, pfgs_db_pmb, stage, pfg_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_pfg_dbs_get()",0,0);
}

uint32
  arad_sw_db_pmf_psl_default_dbs_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_IN uint32                    default_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_DEFAULT_DBS, default_db_pmb, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_default_dbs_set()",0,0);
}


uint32
  arad_sw_db_pmf_psl_default_dbs_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_OUT uint32                   default_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_DEFAULT_DBS, default_db_pmb, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_default_dbs_get()",0,0);
}

uint32
  arad_sw_db_pmf_psl_pfg_tms_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_IN uint32                    pfg_tm_bmp[ARAD_BIT_TO_U32(ARAD_PMF_NOF_GROUPS)]
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_PFG_TMS, pfg_tm_bmp, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_pfg_tms_set()",0,0);
}


uint32
  arad_sw_db_pmf_psl_pfg_tms_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_OUT uint32                   pfg_tm_bmp[ARAD_BIT_TO_U32(ARAD_PMF_NOF_GROUPS)]
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_PFG_TMS, pfg_tm_bmp, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_pfg_tms_get()",0,0);
}

/*
 * PMF PSL info {
 */



uint32 
  arad_sw_db_level_info_get(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32             level_indx,
      SOC_SAND_IN  uint32             is_tm,
      SOC_SAND_OUT  ARAD_SW_DB_PMF_PSL_LEVEL_INFO  *level_info
  )
{
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO  *tmp_level_info_arr = NULL;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_LEVEL_INFO_GET);

    ARAD_ALLOC(tmp_level_info_arr, ARAD_SW_DB_PMF_PSL_LEVEL_INFO, ARAD_PMF_NOF_LEVELS_MAX_ALL_STAGES, "arad_sw_db_level_info_get.tmp_level_info_arr");
    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_LEVELS_INFO, tmp_level_info_arr, stage, is_tm);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    *level_info = tmp_level_info_arr[level_indx];

exit:
    ARAD_FREE(tmp_level_info_arr);
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_level_info_get()",0,0);
}

uint32 
  arad_sw_db_level_info_set(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32             level_indx,
      SOC_SAND_IN  uint32             is_tm,
      SOC_SAND_IN  ARAD_SW_DB_PMF_PSL_LEVEL_INFO  *level_info
  )
{
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO  *tmp_level_info_arr = NULL;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_ALLOC(tmp_level_info_arr, ARAD_SW_DB_PMF_PSL_LEVEL_INFO, ARAD_PMF_NOF_LEVELS_MAX_ALL_STAGES, "arad_sw_db_level_info_set.tmp_level_info_arr");
    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_LEVELS_INFO, tmp_level_info_arr, stage, is_tm);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    tmp_level_info_arr[level_indx] = *level_info;
    res = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_LEVELS_INFO, tmp_level_info_arr, stage, is_tm);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
    ARAD_FREE(tmp_level_info_arr);
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_level_info_set()",0,0);
}


uint32 
  arad_sw_db_pmf_psl_init_info_set(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
      SOC_SAND_OUT  ARAD_PMF_SEL_INIT_INFO  *init_info
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_INIT_INFO, init_info, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_init_info_set()",0,0);
}

uint32 
  arad_sw_db_pmf_psl_init_info_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
      SOC_SAND_OUT  ARAD_PMF_SEL_INIT_INFO  *init_info
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_INIT_INFO, init_info, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pmf_psl_init_info_get()",0,0);
}

/*
 * FP info {
 */

/*
 * Field Processor
 */
uint32
  arad_sw_db_fp_db_info_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32              db_ndx,
    SOC_SAND_IN  ARAD_PP_FP_DATABASE_INFO *db_info
   )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  res = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_INFO, db_info, stage, db_ndx);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_fp_db_info_set()", 0, 0);
}

uint32
  arad_sw_db_fp_db_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32               db_ndx,
    SOC_SAND_OUT ARAD_PP_FP_DATABASE_INFO *db_info
 )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_INFO, db_info, stage, db_ndx);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_fp_db_info_get()",0,0);
}

uint32
  arad_sw_db_fp_db_entries_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32              db_ndx,
    SOC_SAND_IN  ARAD_SW_DB_FP_ENTRY  *db_entries
  )
{
    uint32
        res = SOC_SAND_OK;
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_ENTRIES, db_entries, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_fp_db_entries_set()", 0, 0);
}

uint32
  arad_sw_db_fp_db_entries_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32               db_ndx,
    SOC_SAND_OUT ARAD_SW_DB_FP_ENTRY   *db_entries
  )
{
    uint32
        res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_ENTRIES, db_entries, stage, db_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_fp_db_entries_get()",0,0);
}

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
    uint8 entry_bitmap[SOC_DPP_DEFS_MAX_TCAM_NOF_LINES_IN_BYTES];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_ENTRY_BITMAP, entry_bitmap, stage, bank_index);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    if(is_used) {
        SHR_BITSET(entry_bitmap, entry_index); 
    }
    else {
        SHR_BITCLR(entry_bitmap, entry_index); 
    }

    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_ENTRY_BITMAP, entry_bitmap, stage, bank_index);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
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
    uint8
        entry_bitmap[SOC_DPP_DEFS_MAX_TCAM_NOF_LINES_IN_BYTES];
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_ENTRY_BITMAP, entry_bitmap, stage, bank_index);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    *is_used  = ((SHR_BITGET(entry_bitmap, entry_index) > 0 ) ? 1 : 0);
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
    uint8 entry_bitmap[SOC_DPP_DEFS_MAX_TCAM_NOF_LINES_IN_BYTES];
    int i;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(entry_bitmap, 0x0, sizeof(uint8) * SOC_DPP_DEFS_MAX_TCAM_NOF_LINES_IN_BYTES);

    for (i=0; i<SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); i++) {
        rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_ENTRY_BITMAP, entry_bitmap, stage, i);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_db_entry_bitmap_clear()",0,0);
}
 
uint32
  arad_sw_db_fp_db_udf_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                     udf_ndx,
    SOC_SAND_IN  ARAD_PMF_CE_QUAL_INFO   *qual_info
   )
{
    uint32
        res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_UDF, qual_info, stage, udf_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_fp_db_udf_set()", 0, 0);
}


uint32
  arad_sw_db_fp_db_udf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                      udf_ndx,
    SOC_SAND_OUT ARAD_PMF_CE_QUAL_INFO         *qual_info
    )
{
    uint32
        res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_UDF, qual_info, stage, udf_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_fp_db_udf_get()",0,0);
}

uint32
  arad_sw_db_fp_inner_eth_nof_tags_set(
    SOC_SAND_IN int                                unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN uint32                             pfg_ndx,
    SOC_SAND_IN uint32                             inner_eth_nof_tags
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_INNER_ETH_NOF_TAGS, &inner_eth_nof_tags, stage, pfg_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_inner_eth_nof_tags_set()",0,0);
}

uint32
  arad_sw_db_fp_inner_eth_nof_tags_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pfg_ndx,
    SOC_SAND_OUT uint32                            *inner_eth_nof_tags
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_INNER_ETH_NOF_TAGS, inner_eth_nof_tags, stage, pfg_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_inner_eth_nof_tags_get()",0,0);
}


uint32
  arad_sw_db_fp_key_change_size_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN ARAD_FP_DATABASE_STAGE stage,
    SOC_SAND_IN uint8                  key_change_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_KEY_CHANGE_SIZE, &key_change_size, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_key_change_size_set()",0,0);
}

uint32
  arad_sw_db_fp_key_change_size_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_OUT uint8                             *key_change_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_KEY_CHANGE_SIZE, key_change_size, stage);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_key_change_size_get()",0,0);
}


/*
 * FP info }
 */


uint32
  arad_sw_db_local_to_reassembly_context_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                  port,
    SOC_SAND_IN uint32                  reassembly_context
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         lag.local_to_reassembly_context[port],
                         (&reassembly_context));

    
#ifdef BCM_WARM_BOOT_SUPPORT
    
    
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }   
    res = arad_wb_db_lag_update_local_to_reassembly_context_state(unit, port); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_local_to_reassembly_context_set()",port,0);
}

uint32
  arad_sw_db_local_to_reassembly_context_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                   port
  )
{
  return Arad_sw_db.arad_device_sw_db[unit]->lag.local_to_reassembly_context[port];
}


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
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(local_port, ARAD_NOF_LOCAL_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_CHECK_NULL_INPUT(is_reserved);

  *is_reserved = (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_reserved_reassembly_context[
    local_port / SOC_SAND_NOF_BITS_IN_UINT32] >> (local_port % SOC_SAND_NOF_BITS_IN_UINT32)) & 1;

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
  uint32 *value_ptr = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_reserved_reassembly_context +
    local_port / SOC_SAND_NOF_BITS_IN_UINT32;
  uint32 value = *value_ptr;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(local_port, ARAD_NOF_LOCAL_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);

  if (reserve) {
    value |= mask;
  } else {
    value &= ~mask;
  }
  if (value != *value_ptr) {
      *value_ptr = value;
      rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_RESERVED_REASSEMBLY_CONTEXTS,
        Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_reserved_reassembly_context);  /* update warm boot data */
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_set_port_reserved_for_reassembly_context()",local_port,reserve);
}

uint32
  arad_sw_db_pfg_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_TMC_PMF_PFG_INFO      *pfg_info
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PFG_INFO, pfg_info, stage, pfg_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pfg_info_set()",0,0);
}

uint32
  arad_sw_db_pfg_info_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_OUT SOC_TMC_PMF_PFG_INFO      *pfg_info
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PFG_INFO, pfg_info, stage, pfg_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_pfg_info_get()",0,0);
}





#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP 	 
void 	 
arad_sw_db_sw_dump(int unit) 	 
{
    uint32                i, j;    
    ARAD_SW_DB_DEVICE   *db;


        

    db = Arad_sw_db.arad_device_sw_db[unit];

    if(Arad_sw_db_initialized == FALSE ||
       db                     == NULL) {

        LOG_ERROR(BSL_LS_SOC_SWDB,
                  (BSL_META_U(unit,
                              "ERROR: ARAD sw db is not initialized\n")));
        return;

    }


    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n ARAD SOC TM:")));
    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n ------------")));

    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n current_cell_ident:   %u\n"),  db->cell.current_cell_ident));
    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n calcal_length:        %u\n"),  db->arad_sw_db_egr_ports.calcal_length));
    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n update_dev_changed:   %hhu\n"), db->arad_sw_db_egr_ports.update_dev_changed));
    

    for (i = 0; i < ARAD_EGR_NOF_Q_PAIRS; i++) {
        if(db->arad_sw_db_egr_ports.port_priority_cal.queue_rate[i].valid) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n queue_rate (%03d): valid %hhu egq_rates %u egq_bursts %u\n"), 
                                    i, 
                         db->arad_sw_db_egr_ports.port_priority_cal.queue_rate[i].valid, 
                         db->arad_sw_db_egr_ports.port_priority_cal.queue_rate[i].egq_rates, 
                         db->arad_sw_db_egr_ports.port_priority_cal.queue_rate[i].egq_bursts));
        }
    }

    for (i = 0; i < ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB; i++) {
        LOG_VERBOSE(BSL_LS_SOC_SWDB,
                    (BSL_META_U(unit,
                                "\n nof_calcal_instances (%02d):  %u\n"), i, db->arad_sw_db_egr_ports.chan_arb[i].nof_calcal_instances));
    }

    for (i = 0; i < ARAD_NOF_FAP_PORTS; i++) {
        if(db->arad_sw_db_egr_ports.ports_prog_editor_profile[i] != 0) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n ports_prog_editor_profile (%03d):  %hu\n"), i, db->arad_sw_db_egr_ports.ports_prog_editor_profile[i]));
        }
    }

    for (i = 0; i < ARAD_NOF_LAG_GROUPS_MAX; i++) {
        if(db->lag.in_use[i] != 0) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n in_use (%04d):  %hhu\n"), i, db->lag.in_use[i]));
        }
    }

    for (i = 0; i < ARAD_NOF_LOCAL_PORTS; i++) {
        if(db->lag.local_to_reassembly_context[i] != i) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n local_to_reassembly_context (%03d):  %u\n"), i, db->lag.local_to_reassembly_context[i]));
        }
    }

    for (i = 0; i < ARAD_NOF_TDM_CONTEXT_MAP; i++) {
        if(db->tdm.context_map[i] != i) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n tdm_context_map (%03d):  %u\n"), i, db->tdm.context_map[i]));
        }
    }
 
    for (i = 0; i < ARAD_EGR_NOF_PS; i++) {
        for (j = 0; j < ARAD_NOF_TCGS; j++) {
            if(db->arad_sw_db_egr_ports.tcg_cal.tcg_rate[i][j].valid) {
                LOG_VERBOSE(BSL_LS_SOC_SWDB,
                            (BSL_META_U(unit,
                                        "\n eg_mult_nof_vlan_bitmaps (%02d, %01d): valid %hhu egq_rates %u egq_bursts %u\n"), 
                                        i, j, 
                             db->arad_sw_db_egr_ports.tcg_cal.tcg_rate[i][j].valid, 
                             db->arad_sw_db_egr_ports.tcg_cal.tcg_rate[i][j].egq_rates, 
                             db->arad_sw_db_egr_ports.tcg_cal.tcg_rate[i][j].egq_bursts));
            }
        }
    }

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); i++) {
        for (j = 0; j < ARAD_EGR_NOF_BASE_Q_PAIRS; j++) {
            if(db->arad_sw_db_egr_ports.rates[i][j].valid) {
                LOG_VERBOSE(BSL_LS_SOC_SWDB,
                            (BSL_META_U(unit,
                                        "\n rates (%02d, %03d): valid %hhu sch_rates %u egq_rates %u egq_bursts %u\n"), 
                                        i, j, 
                             db->arad_sw_db_egr_ports.rates[i][j].valid, 
                             db->arad_sw_db_egr_ports.rates[i][j].sch_rates, 
                             db->arad_sw_db_egr_ports.rates[i][j].egq_rates, 
                             db->arad_sw_db_egr_ports.rates[i][j].egq_bursts));
            }
        }
    }

}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#define ARAD_DEVICE_NUMBER_BITS 11
#define ARAD_DEVICE_NUMBER_MASK 0x7ff

uint32 
    arad_sw_db_sysport2basequeue_init(
       SOC_SAND_IN int unit
    )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);

    Arad_sw_db.arad_device_sw_db[unit]->tm.sysport2basequeue = NULL;
    if (!SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit)) {
        ARAD_ALLOC(Arad_sw_db.arad_device_sw_db[unit]->tm.sysport2basequeue, ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE, ARAD_NOF_SYS_PHYS_PORTS_GET(unit) * SOC_DPP_DEFS_GET(unit, nof_cores), "sysport2basequeue");
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sysport2basequeue_init()",unit,0);

}
uint32 
    arad_sw_db_sysport2basequeue_terminate(
       SOC_SAND_IN int unit
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);

    if (Arad_sw_db.arad_device_sw_db[unit]->tm.sysport2basequeue != NULL) {
        ARAD_FREE(Arad_sw_db.arad_device_sw_db[unit]->tm.sysport2basequeue);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sysport2basequeue_terminate()",unit,0);
}

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
#ifdef BCM_WARM_BOOT_SUPPORT
  uint32 res = SOC_SAND_OK;
#endif /* BCM_WARM_BOOT_SUPPORT */
  uint32 modport;
  ARAD_SYSPORT* modport2sysport;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_id, ARAD_NOF_FAPS_IN_SYSTEM, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_port_id, ARAD_NOF_FAP_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(sysport, ARAD_NOF_SYS_PHYS_PORTS_GET(unit), SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);

  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);
  modport2sysport = Arad_sw_db.arad_device_sw_db[unit]->modport2sysport;
  SOC_SAND_CHECK_NULL_INPUT(modport2sysport);

  modport = fap_id | (fap_port_id << ARAD_DEVICE_NUMBER_BITS);
  SOC_SAND_ERR_IF_ABOVE_NOF(modport, ARAD_NOF_MODPORT, SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);
  modport2sysport[modport] = sysport;

#ifdef BCM_WARM_BOOT_SUPPORT
  /* update warm boot file */
  res = arad_wb_db_update_modport2sysport_state(unit, modport);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
#endif /* BCM_WARM_BOOT_SUPPORT */

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
  ARAD_SYSPORT* modport2sysport;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(sysport);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_id, ARAD_NOF_FAPS_IN_SYSTEM, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_port_id, ARAD_NOF_FAP_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 15, exit);
  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);
  modport2sysport = Arad_sw_db.arad_device_sw_db[unit]->modport2sysport;
  SOC_SAND_CHECK_NULL_INPUT(modport2sysport);

  modport = fap_id | (fap_port_id << ARAD_DEVICE_NUMBER_BITS);
  SOC_SAND_ERR_IF_ABOVE_NOF(modport, ARAD_NOF_MODPORT, SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);
  if (modport2sysport[modport] != ARAD_SW_DB_MODPORT2SYSPORT_INVALID_SYSPORT) {
      *sysport = modport2sysport[modport];
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
  ARAD_SYSPORT* modport2sysport;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(fap_id);
  SOC_SAND_CHECK_NULL_INPUT(fap_port_id);
  SOC_SAND_ERR_IF_ABOVE_NOF(sysport, ARAD_NOF_SYS_PHYS_PORTS_GET(unit), SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);
  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);
  modport2sysport = Arad_sw_db.arad_device_sw_db[unit]->modport2sysport;
  SOC_SAND_CHECK_NULL_INPUT(modport2sysport);

  for (modport_i = 0; modport_i < ARAD_NOF_MODPORT; ++modport_i) {
      if (modport2sysport[modport_i] == sysport){
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
    SOC_SAND_IN ARAD_SYSPORT    sysport,
    SOC_SAND_IN uint32          core_id,
    SOC_SAND_IN uint8           valid,
    SOC_SAND_IN uint8           sw_only,
    SOC_SAND_IN uint32          base_queue
   )
{
    uint32 core_bitmap = 0x0;
    int core_offset = 0;
    ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE *sysport2basequeue;
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
    if (core_id != SOC_CORE_ALL){
        if (core_id < SOC_DPP_DEFS_GET(unit, nof_cores)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid core_id: %d"), sysport));
        }
        SHR_BITSET(&core_bitmap, core_id);
    } else {
        SHR_BITSET_RANGE(&core_bitmap, 0, SOC_DPP_DEFS_GET(unit, nof_cores));
    }

    sysport2basequeue = Arad_sw_db.arad_device_sw_db[unit]->tm.sysport2basequeue;
    SOCDNX_NULL_CHECK(sysport2basequeue);
    
    for (core_offset = 0 ;core_offset < SOC_DPP_DEFS_GET(unit, nof_cores) ;core_offset++) {
        if (SHR_BITGET(&core_bitmap, core_offset)) {
            if (valid) {
                sysport2basequeue[sysport + core_offset].valid_flags |= ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_VALID;
            } else {
                sysport2basequeue[sysport + core_offset].valid_flags &= ~(ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_VALID);
            }

            if (sw_only) {
                sysport2basequeue[sysport + core_offset].valid_flags |= ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_SW_ONLY;
            } else {
                sysport2basequeue[sysport + core_offset].valid_flags &= ~(ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_SW_ONLY);
            }
            sysport2basequeue[sysport + core_offset].base_queue = (valid) ? base_queue : ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE;
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* update warm boot file */
    SOCDNX_SAND_IF_ERR_EXIT(arad_sw_db_tm_update_sysport2basequeue_state(unit, sysport));
#endif /* BCM_WARM_BOOT_SUPPORT */

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_sysport2queue_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_SYSPORT    sysport,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_OUT uint8          *valid,
    SOC_SAND_OUT uint8          *sw_only,
    SOC_SAND_OUT uint32         *base_queue
   )
{
    int core_offset = 0;
    ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE *sysport2basequeue;
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
    if (core_id != SOC_CORE_ALL){
        if (core_id < SOC_DPP_DEFS_GET(unit, nof_cores)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid core_id: %d"),sysport));
        }
        core_offset = core_id;
    } else {
        core_offset = 0;
    }

    sysport2basequeue = Arad_sw_db.arad_device_sw_db[unit]->tm.sysport2basequeue;
    SOCDNX_NULL_CHECK(sysport2basequeue);

    *valid   = sysport2basequeue[sysport + core_offset].valid_flags | ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_VALID;
    *sw_only = sysport2basequeue[sysport + core_offset].valid_flags | ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_SW_ONLY;
    if (*valid) {
        *base_queue = sysport2basequeue[sysport + core_offset].base_queue;
    } else {
        *base_queue = ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE;
    }
   
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
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(mapped_q_type);

  if (user_q_type >= ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE && user_q_type < ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE + ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES) {
    *mapped_q_type = Arad_sw_db.arad_device_sw_db[unit]->q_type_map[user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE];
    if (!*mapped_q_type) {
      *mapped_q_type = ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE;
    }
  } else if (user_q_type >= SOC_TMC_ITM_PREDEFIEND_OFFSET &&
    (user_q_type < SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_NOF_QT_STATIC ||
     user_q_type == SOC_TMC_ITM_PREDEFIEND_OFFSET + ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE + ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES)) {
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
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32 res;
#endif /*BCM_WARM_BOOT_SUPPORT*/
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(mapped_q_type);

  if (user_q_type >= ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE && user_q_type < ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE + ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES) {
    uint8 *mapping = &Arad_sw_db.arad_device_sw_db[unit]->q_type_map[-ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE];

    if (!mapping[user_q_type]) { /* if the user queue is not mapped, try to map it */
      uint8 reverse_mapping[ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1 - ARAD_SW_1ST_AVAILABLE_HW_QUEUE] = {0};
      uint8 i, hw_q;
      for (i = ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE; i < ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1; ++i) {
        if ((hw_q = mapping[i])) {
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
      mapping[user_q_type] = hw_q ? hw_q : ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE; /* also handle the case of no available hardware queue */
    }

    *mapped_q_type = mapping[user_q_type];
#ifdef BCM_WARM_BOOT_SUPPORT
    
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 5, exit);
    }
    
    res = arad_wb_db__update_q_type_map_state(unit, user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/
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
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(user_q_type);

  if (mapped_q_type < SOC_TMC_ITM_NOF_QT_STATIC || mapped_q_type == ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE + ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES) {
    *user_q_type = mapped_q_type + SOC_TMC_ITM_PREDEFIEND_OFFSET;
  } else {
    uint8 i;
    if (mapped_q_type >= ARAD_SW_NOF_AVAILABLE_HW_QUEUE)
    SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 20, exit); /* internal error */
    for (i = 0; i < ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES; ++i) {
      if (Arad_sw_db.arad_device_sw_db[unit]->q_type_map[i] == mapped_q_type) {
        break;
      }
    }
    if (i >= ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES) {
      SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 20, exit); /* no user queue type is mapped to this hardware value; */
    }
    *user_q_type = i + ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_queue_type_map_reverse_get()", unit, mapped_q_type);
}

uint32 
    arad_sw_db_tm_queue_to_rate_class_mapping_is_simple_set(
       SOC_SAND_IN int                          unit,
       SOC_SAND_IN uint8                        queue_to_rate_class_mapping_is_simple
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         tm.queue_to_rate_class_mapping.is_simple_mode,
                         &queue_to_rate_class_mapping_is_simple
                         );

#ifdef BCM_WARM_BOOT_SUPPORT
    

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    res = arad_sw_db_tm_update_queue_to_rate_class_mapping_is_simple_state(unit); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tm_queue_to_rate_class_mapping_is_simple_set()",0,0);
}
uint32
    arad_sw_db_tm_queue_to_rate_class_mapping_is_simple_get(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_OUT uint8*                      queue_to_rate_class_mapping_is_simple
  )
{
    ARAD_SW_DB_INIT_DEFS;
    ARAD_SW_DB_FIELD_GET(unit,
                         tm.queue_to_rate_class_mapping.is_simple_mode,
                         queue_to_rate_class_mapping_is_simple);
}

uint32 
    arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_set(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      rate_class,
       SOC_SAND_IN uint32                       ref_count
  ) 
{

    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         tm.queue_to_rate_class_mapping.ref_count[rate_class],
                         &ref_count
                         );
#ifdef BCM_WARM_BOOT_SUPPORT
    
  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }
  
  res = arad_sw_db_tm_update_queue_to_rate_class_mapping_ref_count_state(unit, ref_count);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_set()",0,0);
}

uint32 
    arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_get(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      rate_class,
       SOC_SAND_OUT uint32*                     ref_count
  ) 
{
    ARAD_SW_DB_INIT_DEFS;
    ARAD_SW_DB_FIELD_GET(unit,
                         tm.queue_to_rate_class_mapping.ref_count[rate_class],
                         ref_count);
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

        res = arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_get(unit,old_rate_class, &old_ref_count);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        res = arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_get(unit, new_rate_class, &new_ref_count);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        SOC_SAND_ERR_IF_ABOVE_MAX(new_ref_count, ARAD_MAX_QUEUE_ID(unit), ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 60, exit);

        res = arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_set(unit, new_rate_class, new_ref_count+1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

        res = arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_set(unit, old_rate_class, old_ref_count-1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
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
  uint32 group_open_index = group_id / SOC_SAND_NOF_BITS_IN_UINT32;
  uint32 group_open_bit = group_id % SOC_SAND_NOF_BITS_IN_UINT32;
  uint32* word;
  
  SOCDNX_INIT_FUNC_DEFS;
  if (!SOC_UNIT_NUM_VALID(unit)) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid unit")));
  } else if (group_id >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("input too big")));
  }

  word = Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_multicast.egress_groups_open_data + group_open_index; /* get the pointer to the word needing the change */

#ifdef BCM_WARM_BOOT_SUPPORT
  if (((*word >> group_open_bit) & 1) != is_open ? 1 : 0) {
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (is_open) {
      *word |= (((uint32)1) << group_open_bit);
    } else {
      *word &= ~(((uint32)1) << group_open_bit);
    }
   
#ifdef BCM_WARM_BOOT_SUPPORT
    /* update warm boot file */
    SOCDNX_SAND_IF_ERR_EXIT(arad_wb_db_multicast_update_egress_groups_open_data_state(unit, group_open_index));
  }
#endif /* BCM_WARM_BOOT_SUPPORT */

exit:
  SOCDNX_FUNC_RETURN;
}

/* Check if the given egress multicast group is created=opened, will return 1 if the group is marked as open, or 0 */
uint8 arad_sw_db_egress_group_open_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 group_id /* multicast ID */
)
{
  DPP_MC_ASSERT(group_id < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
  return 1 & (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_multicast.egress_groups_open_data[group_id / SOC_SAND_NOF_BITS_IN_UINT32] >> (group_id % SOC_SAND_NOF_BITS_IN_UINT32));
}

/* return the address of the egress_groups_open_data pointer in SWDB */
uint32** arad_sw_db_get_egress_group_open_data_address(
    SOC_SAND_IN  int    unit
)
{
  return &Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_multicast.egress_groups_open_data;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */

