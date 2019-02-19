/*
 * $Id: cosq.c,v 1.13 Broadcom SDK $
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
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/mbcm.h>
#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/PB_TM/pb_ofp_rates.h>
#endif /* BCM_PETRAB_SUPPORT */
#include <bcm_int/dpp/utils.h>

soc_error_t
  soc_dpp_cosq_flow_and_up_info_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  int                             core,
    SOC_SAND_IN  uint8                              is_flow,
    SOC_SAND_IN  uint32                             dest_id, /* Destination port or flow ndx */
    SOC_SAND_IN  uint32                             reterive_status,
    SOC_SAND_INOUT  SOC_TMC_SCH_FLOW_AND_UP_INFO    *flow_and_up_info
  )
{
  uint8    
    valid=0;
  uint32
    ret = SOC_E_NONE,
    flow_id =0,
    queue_quartet_ndx=0;
  uint32
    mapped_fap_id = 0,
    mapped_fap_port_id = 0;
  int 
      core_idx = core;
  uint8 
      is_fap_id_local,
      is_sw_only = FALSE;
#if defined(BROADCOM_DEBUG)
  
#endif

  SOCDNX_INIT_FUNC_DEFS;  

  if (flow_and_up_info->credit_sources_nof == 0)
  {
      /*First level info*/
      if (is_flow) {
         flow_id = dest_id;
      }else {
         ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, dest_id, &mapped_fap_id, &mapped_fap_port_id));

         if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
         {
            LOG_INFO(BSL_LS_SOC_COSQ,
                     (BSL_META_U(unit,
                                 "soc_petra_sys_phys_to_local_port_map_get.\n\r")));
            SOCDNX_SAND_IF_ERR_RETURN(ret);
         }

         SOCDNX_IF_ERR_EXIT(soc_dpp_is_fap_id_local_and_get_core_id(unit, mapped_fap_id, &is_fap_id_local, &core_idx));

         if (!is_fap_id_local)
         {
            LOG_INFO(BSL_LS_SOC_COSQ,
                     (BSL_META_U(unit,
                                 "Destination is on remote FAP. Cannot print flow and up.\n\r")));
            SOCDNX_SAND_IF_ERR_RETURN(ret);
         }

         ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_destination_id_packets_base_queue_id_get,(unit, core_idx, dest_id, &valid, &is_sw_only, &(flow_and_up_info->base_queue)));
         
         if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
         {
            LOG_INFO(BSL_LS_SOC_COSQ,
                     (BSL_META_U(unit,
                                 "soc_petra_ipq_destination_id_packets_base_queue_id_get.\n\r")));
            SOCDNX_IF_ERR_EXIT(ret);
         }

         if (!valid)
         {
           SOCDNX_IF_ERR_EXIT(SOC_E_EXISTS);
         }

         queue_quartet_ndx = (flow_and_up_info->base_queue) / 4;

         ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_queue_to_flow_mapping_get,(unit, core, queue_quartet_ndx, &(flow_and_up_info->qrtt_map_info)));

         if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
         {        
            LOG_INFO(BSL_LS_SOC_COSQ,
                     (BSL_META_U(unit,
                                 "soc_petra_ipq_queue_to_flow_mapping_get.\n\r")));
            SOCDNX_SAND_IF_ERR_RETURN(ret);
         }
         flow_id = (flow_and_up_info->qrtt_map_info).flow_quartet_index * 4;
      }
  }
  
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_flow_and_up_info_get,(unit, core_idx, flow_id, reterive_status, flow_and_up_info));
  SOCDNX_SAND_IF_ERR_RETURN(ret);

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_hr2ps_info_get(
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   int                                core,
    SOC_SAND_IN   uint32                             se_id,
    SOC_SAND_OUT  soc_hr2ps_info_t                   *hr2ps_info

  )
{

  uint32                          ret = SOC_E_NONE;
  uint32                          port_id = 0, tc = 0;

  SOC_TMC_SCH_PORT_INFO sch_port_info;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(hr2ps_info);

  SOC_TMC_SCH_PORT_INFO_clear(&sch_port_info);

  /* get port_id and tc */
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_se2port_tc_id, (unit, core, se_id, &port_id, &tc));
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_sch_se2port_tc_id\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }

  /* get scheduler info */
  ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_aggregate_get, (unit, core, se_id, &(hr2ps_info->se_info), &(hr2ps_info->flow_info))); 
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_sch_se_get_type_by_id\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }

  /* get rate info*/
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_sch_port_priority_rate_get,(unit, core, port_id , tc, &(hr2ps_info->kbits_sec_max)));
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_ofp_rates_sch_port_priority_rate_get\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }

  /* get burst info*/
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_sch_port_priority_max_burst_get,(unit,core, port_id , tc, &(hr2ps_info->max_burst)));
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_ofp_rates_sch_port_priority_max_burst_get\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }

  /* get sched info*/
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_port_sched_get,(unit, core, port_id, &sch_port_info));
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_sch_port_sched_get\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }
  hr2ps_info->mode = sch_port_info.tcg_ndx[tc];
  hr2ps_info->weight = 0;

  SOCDNX_SAND_IF_ERR_RETURN(ret);

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_non_empty_queues_info_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                queue_to_read_from,
    SOC_SAND_IN  uint32                max_array_size,
    SOC_SAND_OUT soc_ips_queue_info_t* queues,
    SOC_SAND_OUT uint32*               nof_queues_filled,
    SOC_SAND_OUT uint32*               next_queue,
    SOC_SAND_OUT uint32*               reached_end
  )
{

    uint32 ret;
#if defined(BROADCOM_DEBUG)
  
#endif

  SOCDNX_INIT_FUNC_DEFS; 

  

    ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ips_non_empty_queues_info_get,(unit, core_id, queue_to_read_from,
                                                                            max_array_size,queues,nof_queues_filled,next_queue,reached_end));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_queue_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_INFO      *info    
  )
{
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;
  
    ret = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_queue_info_get,(unit, SOC_CORE_ALL, queue_ndx, info)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_queue_category_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT int                  *voq_category
  )
{
    uint32 ret;
    SOC_TMC_ITM_CATEGORY_RNGS info;
    int queue_category = 0;	
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;

    SOC_TMC_ITM_CATEGORY_RNGS_clear(&info);

    ret = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_category_rngs_get,(unit, BCM_CORE_ALL, &info)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    if (queue_ndx <= info.vsq_ctgry0_end) {
        queue_category = 0;
    } else if (queue_ndx <= info.vsq_ctgry1_end) {
        queue_category = 1;
    } else if (queue_ndx <= info.vsq_ctgry2_end) {
        queue_category = 2;
    } else {
        queue_category = 3;
    }
    *voq_category = queue_category;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_queue_to_flow_mapping_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_IPQ_QUARTET_MAP_INFO          *queue_map_info
  )
{
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;
  
    ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ipq_queue_to_flow_mapping_get,
            (unit, SOC_CORE_ALL, SOC_TMC_IPQ_Q_TO_QRTT_ID(queue_ndx), queue_map_info));
	SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_alpha_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT int32                  *alpha 
  )
{
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;
  
    ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_alpha_get,
            (unit, rt_cls_ndx, drop_precedence_ndx, alpha));
	SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT  SOC_TMC_ITM_ADMIT_TSTS     *test_tmplt
  )
{	
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif
	
    SOCDNX_INIT_FUNC_DEFS;
	  
    ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_queue_test_tmplt_get,
				(unit, rt_cls_ndx, drop_precedence_ndx, test_tmplt));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
	
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_queue_dyn_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_DYN_INFO      *info    
  )
{
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;

#ifdef BCM_ARAD_SUPPORT
    if (SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_ARAD) {
        ret = arad_itm_queue_dyn_info_get(unit, SOC_CORE_ALL, queue_ndx, info);
        SOCDNX_SAND_IF_ERR_RETURN(ret);
    }
    else 
#endif
    {
        ret = SOC_E_UNAVAIL;
        return ret;
    }

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_vsq_index_global_to_group_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsq_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_GROUP      *soc_vsq_group_type,
    SOC_SAND_OUT uint32                        *vsq_index,
    SOC_SAND_OUT uint8                         *is_ocb_only
  )
{
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;

    ret = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit, mbcm_dpp_itm_vsq_index_global2group,(unit, vsq_id, soc_vsq_group_type, vsq_index, is_ocb_only)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

int 
soc_dpp_voq_max_size_drop(int unit, uint32 *is_max_size) 
{
    uint32 ret;
    SOCDNX_INIT_FUNC_DEFS;

    ret = (MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_ingress_drop_status,(unit, is_max_size)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}


/*
 * This function receives a FAP ID and returnes wethrer the FAP ID is local
 * (of a core of the local device) and if so, returns the core ID.
 * For single core devices or devices in single core mode, the core ID is always 0.
 */
soc_error_t soc_dpp_is_fap_id_local_and_get_core_id(
    SOC_SAND_IN   int      unit,
    SOC_SAND_IN   uint32   fap_id,    /* input FAP ID */
    SOC_SAND_OUT  uint8    *is_local, /* returns TRUE/FASLE based on if fap_id is of a local core */
    SOC_SAND_OUT  int      *core_id   /* if is_local returns TRUE, will contain the core ID of the FAP ID */
)
{
    uint8 is_fap_id_local;
    uint32 local_base_fap_id;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(core_id);

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &local_base_fap_id))); /* get the base FAP ID of the local device */

    if (fap_id >= local_base_fap_id && fap_id < local_base_fap_id + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        is_fap_id_local = TRUE;
        *core_id = fap_id - local_base_fap_id;
    } else {
        is_fap_id_local = FALSE;
        *core_id = SOC_CORE_ALL;
    }

    if (is_local != NULL) {
        *is_local = is_fap_id_local;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_fc_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info 
  )
{
    uint32 ret;

    SOCDNX_INIT_FUNC_DEFS;
  
    ret = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fc_status_info_get, (unit, fc_status_key, fc_status_info)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

