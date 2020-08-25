/* $Id: jer2_tmc_api_egr_queuing.c,v 1.12 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_egr_queuing.c
*
* MODULE PREFIX:  soc_jer2_tmcegr
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

/*************
 * INCLUDES  *
 *************/
/* { */


#include <shared/bsl.h>


#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/TMC/tmc_api_egr_queuing.h>

#include <soc/dnxc/legacy/error.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

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

void
  SOC_DNX_EGR_DROP_THRESH_clear(
    DNX_SAND_OUT SOC_DNX_EGR_DROP_THRESH *info
  )
{
  uint32
    ind;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_DROP_THRESH));
  info->words_consumed = 0;
  info->packets_consumed = 0;
  info->descriptors_avail = 0;
  info->buffers_avail = 0;
  info->buffers_used = 2048;
  for (ind=0; ind<SOC_DNX_EGR_PORT_NOF_THRESH_TYPES; ++ind)
  {
    info->queue_words_consumed[ind] = 0;
  }
  for (ind = 0; ind < SOC_DNX_EGR_PORT_NOF_THRESH_TYPES; ++ind)
  {
    info->queue_pkts_consumed[ind] = 0;
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_THRESH_INFO_clear(
    DNX_SAND_OUT SOC_DNX_EGR_THRESH_INFO *info
  )
{

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_THRESH_INFO));
  info->dbuff = 0;
  info->packet_descriptors = 0;
  
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_QUEUING_TH_DB_GLOBAL_clear(
    DNX_SAND_OUT SOC_DNX_EGR_QUEUING_TH_DB_GLOBAL *info
  )
{

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&info->uc);
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&info->mc);
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&info->total);
  
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_QUEUING_TH_DB_POOL_clear(
    DNX_SAND_OUT SOC_DNX_EGR_QUEUING_TH_DB_POOL *info
  )
{

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&info->mc);
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&info->reserved);
  
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_QUEUING_TH_DB_PORT_clear(
    DNX_SAND_OUT SOC_DNX_EGR_QUEUING_TH_DB_PORT *info
  )
{
  uint8
  	traffic_class;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&info->uc);
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&info->mc_shared);
  for(traffic_class = 0; traffic_class < SOC_DNX_NOF_TRAFFIC_CLASSES; ++traffic_class)
  {
    SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&info->reserved[traffic_class]);
  }
  
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_QUEUING_DEV_TH_clear(
    DNX_SAND_OUT SOC_DNX_EGR_QUEUING_DEV_TH *info
  )
{
  uint8
  	i,
	j;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  SOC_DNX_EGR_QUEUING_TH_DB_GLOBAL_clear(&info->global);
  for(i=0; i< SOC_DNX_NOF_EGR_QUEUING_MC_POOLS; ++i)
  {
	SOC_DNX_EGR_QUEUING_TH_DB_POOL_clear(&info->pool[i]);
  }
  for(i=0; i < SOC_DNX_NOF_EGR_QUEUING_MC_POOLS; ++i)
  {
  	for(j = 0; j < SOC_DNX_NOF_TRAFFIC_CLASSES; ++j)
	{
	  SOC_DNX_EGR_QUEUING_TH_DB_POOL_clear(&info->pool_tc[i][j]);
	}
  }
  for(i=0; i < SOC_DNX_NOF_THRESH_TYPES; ++i)
  {
  	SOC_DNX_EGR_QUEUING_TH_DB_PORT_clear(&info->thresh_type[i]);
  }
  
exit:
  SHR_VOID_FUNC_EXIT;
}
#if (0)
/* { */
void
  SOC_DNX_EGR_QUEUING_MC_COS_MAP_clear(
    DNX_SAND_OUT SOC_DNX_EGR_QUEUING_MC_COS_MAP *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  info->pool_eligibility = 0;
  info->pool_id = 0;
  info->tc_group = 0;
  
exit:
  SHR_VOID_FUNC_EXIT;
}
/* } */
#endif
void
  SOC_DNX_EGR_QUEUING_IF_FC_clear(
    DNX_SAND_OUT SOC_DNX_EGR_QUEUING_IF_FC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  info->mc_pd_profile = 0;
  info->uc_profile = 0;
  
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_QUEUING_IF_UC_FC_clear(
    DNX_SAND_OUT SOC_DNX_EGR_QUEUING_IF_UC_FC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  info->pd_th = 0;
  info->pd_th_min = 0;
  info->pd_th_alpha = 0;
  info->size256_th = 0;
  info->size256_th_min = 0;
  info->size256_th_alpha = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(
    DNX_SAND_OUT SOC_DNX_EGR_FC_DEV_THRESH_INNER *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_FC_DEV_THRESH_INNER));
  info->words = 0;
  info->buffers = 0;
  info->descriptors = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_DEVICE_THRESH_clear(
    DNX_SAND_OUT SOC_DNX_EGR_FC_DEVICE_THRESH *info
  )
{
  int pool;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_FC_DEVICE_THRESH));
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&(info->global));
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&(info->scheduled));
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&(info->unscheduled));
  for(pool = 0; pool < SOC_DNX_NOF_EGR_QUEUING_MC_POOLS; ++pool) {
    SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&(info->unscheduled_pool[pool]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_MCI_THRESH_clear(
    DNX_SAND_OUT SOC_DNX_EGR_FC_MCI_THRESH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_FC_MCI_THRESH));
  info->words_unsch = 0;
  info->packets_unsch = 0;
  info->buffers_unsch = 0;
  info->descriptors_total = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_CHNIF_THRESH_clear(
    DNX_SAND_OUT SOC_DNX_EGR_FC_CHNIF_THRESH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_FC_CHNIF_THRESH));
  info->words = 0;
  info->packets = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_OFP_THRESH_clear(
    DNX_SAND_OUT SOC_DNX_EGR_FC_OFP_THRESH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_FC_OFP_THRESH));
  info->words = 0;
  info->packet_descriptors = 0;
  info->data_buffers = 0;
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_clear(&info->mc);
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_OFP_SCH_WFQ_clear(
    DNX_SAND_OUT SOC_DNX_EGR_OFP_SCH_WFQ *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_OFP_SCH_WFQ));
  info->unsched_weight = 0;
  info->sched_weight = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_OFP_SCH_INFO_clear(
    DNX_SAND_OUT SOC_DNX_EGR_OFP_SCH_INFO *info
  )
{
  SOC_DNX_EGR_Q_PRIO
    q_pair;

  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_OFP_SCH_INFO));
  info->nif_priority = SOC_DNX_EGR_OFP_INTERFACE_PRIO_LOW;
  SOC_DNX_EGR_OFP_SCH_WFQ_clear(&(info->ofp_wfq));
  SOC_DNX_EGR_OFP_SCH_WFQ_clear(&(info->ofp_wfq_high));
  for (q_pair = 0; q_pair < SOC_DNX_EGR_NOF_Q_PRIO_JER2_ARAD; ++q_pair)
  {
    SOC_DNX_EGR_OFP_SCH_WFQ_clear(&(info->ofp_wfq_q_pair[q_pair]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_Q_PRIORITY_clear(
    DNX_SAND_OUT SOC_DNX_EGR_Q_PRIORITY *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_Q_PRIORITY));
  info->tc = 0;
  info->dp = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_QUEUING_TCG_INFO_clear(
    DNX_SAND_OUT SOC_DNX_EGR_QUEUING_TCG_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);

  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_QUEUING_TCG_INFO));  
exit:
  SHR_VOID_FUNC_EXIT;
}

#if (0)
/* { */
void
  SOC_DNX_EGR_TCG_SCH_WFQ_clear(
    DNX_SAND_OUT SOC_DNX_EGR_TCG_SCH_WFQ *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_EGR_TCG_SCH_WFQ));
  info->tcg_weight_valid = FALSE;
exit:
  SHR_VOID_FUNC_EXIT;
}
/* } */
#endif

const char*
  SOC_DNX_EGR_Q_PRIO_to_string(
    DNX_SAND_IN  SOC_DNX_EGR_Q_PRIO enum_val
  )
{
  
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_EGR_Q_PRIO_LOW:
    str = "low";
  break;
  case SOC_DNX_EGR_Q_PRIO_HIGH:
    str = "high";
  break;
  case SOC_DNX_EGR_NOF_Q_PRIO_PB:
  case SOC_DNX_EGR_NOF_Q_PRIO_JER2_ARAD:
    str = "nof_q_prio";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_EGR_PORT_THRESH_TYPE_to_string(
    DNX_SAND_IN SOC_DNX_EGR_PORT_THRESH_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_EGR_PORT_THRESH_TYPE_0:
    str = "type_0";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_1:
    str = "type_1";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_2:
    str = "type_2";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_3:
    str = "type_3";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_4:
    str = "type_4";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_5:
    str = "type_5";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_6:
    str = "type_6";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_7:
    str = "type_7";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_8:
    str = "type_8";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_9:
    str = "type_9";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_10:
    str = "type_10";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_11:
    str = "type_11";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_12:
    str = "type_12";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_13:
    str = "type_13";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_14:
    str = "type_14";
  break;
  case SOC_DNX_EGR_PORT_THRESH_TYPE_15:
    str = "type_15";
  break;
  case SOC_DNX_EGR_PORT_NOF_THRESH_TYPES:
    str = "Not Initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_EGR_MCI_PRIO_to_string(
    DNX_SAND_IN SOC_DNX_EGR_MCI_PRIO enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_EGR_MCI_GUARANTEED:
    str = "mci_guaranteed";
  break;
  case SOC_DNX_EGR_MCI_BE:
    str = "mci_be";
  break;

  case SOC_DNX_EGR_NOF_MCI_PRIORITIES:
    str = "Not Initialized";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_EGR_OFP_INTERFACE_PRIO_to_string(
    DNX_SAND_IN SOC_DNX_EGR_OFP_INTERFACE_PRIO enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_EGR_OFP_INTERFACE_PRIO_HIGH:
    str = "high";
  break;
  case SOC_DNX_EGR_OFP_INTERFACE_PRIO_MID:
    str = "mid";
  break;
  case SOC_DNX_EGR_OFP_INTERFACE_PRIO_LOW:
    str = "low";
  break;

  case SOC_DNX_EGR_OFP_CHNIF_NOF_PRIORITIES:
    str = "Not Initialized";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_EGR_OFP_SCH_MODE_to_string(
    DNX_SAND_IN SOC_DNX_EGR_OFP_SCH_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_EGR_HP_OVER_LP_ALL:
    str = "hp_over_lp_all";
  break;
  case SOC_DNX_EGR_HP_OVER_LP_PER_TYPE:
    str = "hp_over_lp_per_type";
  break;
  case SOC_DNX_EGR_HP_OVER_LP_FAIR:
    str = "hp_over_lp_fair";
  break;
  case SOC_DNX_EGR_OFP_NOF_SCH_MODES:
    str = "Not Initialized";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_EGR_Q_PRIO_MAPPING_TYPE_to_string(
    DNX_SAND_IN SOC_DNX_EGR_Q_PRIO_MAPPING_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_EGR_UCAST_TO_SCHED:
    str = "ucast_to_sched";
  break;
  case SOC_DNX_EGR_MCAST_TO_UNSCHED:
    str = "mcast_to_unsched";
  break;
  case SOC_DNX_EGR_NOF_Q_PRIO_MAPPING_TYPES:
    str = " Not initialized";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_EGR_DROP_THRESH_print(
    DNX_SAND_IN SOC_DNX_EGR_DROP_THRESH *info
  )
{
  uint32
    ind;
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Words_consumed: %u[Words]\n\r"),info->words_consumed));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Packets_consumed: %u[Packets]\n\r"),info->packets_consumed));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Descriptors_avail: %u[Descriptors]\n\r"),info->descriptors_avail));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Buffers_avail: %u[Data buffers]\n\r"),info->buffers_avail));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Buffers_used: %u\n\r"),info->buffers_used));
  for (ind=0; ind<SOC_DNX_EGR_PORT_NOF_THRESH_TYPES; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Queue_words_consumed[%u]: %u[Words]\n\r"),ind,info->queue_words_consumed[ind]));
  }
  for (ind = 0; ind < SOC_DNX_EGR_PORT_NOF_THRESH_TYPES; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "Queue_pkts_consumed[%u]: %u[Packets]\n\r"),ind,info->queue_pkts_consumed[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_THRESH_INFO_print(
    DNX_SAND_IN SOC_DNX_EGR_THRESH_INFO *info
  )
{

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Dbuff: %u[DB]\n\r"),info->dbuff));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Packet_descriptors: %u[PD]\n\r"),info->packet_descriptors));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_print(
    DNX_SAND_IN SOC_DNX_EGR_FC_DEV_THRESH_INNER *info
  )
{

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Words: %u[Words]\n\r"),info->words));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Buffers: %u[Buffers]\n\r"),info->buffers));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Descriptors: %u[Descriptors]\n\r"),info->descriptors));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_DEVICE_THRESH_print(
    DNX_SAND_IN SOC_DNX_EGR_FC_DEVICE_THRESH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Global:\n\r")));
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_print(&(info->global));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Scheduled:\n\r")));
  SOC_DNX_EGR_FC_DEV_THRESH_INNER_print(&(info->scheduled));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_MCI_THRESH_print(
    DNX_SAND_IN SOC_DNX_EGR_FC_MCI_THRESH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Words_unsch: %u[Words]\n\r"),info->words_unsch));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Packets_unsch: %u[Packets]\n\r"),info->packets_unsch));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Buffers_unsch: %u[Buffers]\n\r"),info->buffers_unsch));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Descriptors_total: %u[Descriptors]\n\r"),info->descriptors_total));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_CHNIF_THRESH_print(
    DNX_SAND_IN SOC_DNX_EGR_FC_CHNIF_THRESH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Words: %u[Words]\n\r"),info->words));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Packets: %u[Packets]\n\r"),info->packets));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_FC_OFP_THRESH_print(
    DNX_SAND_IN SOC_DNX_EGR_FC_OFP_THRESH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Words: %u[Words]\n\r"),info->words));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_OFP_SCH_WFQ_print(
    DNX_SAND_IN SOC_DNX_EGR_OFP_SCH_WFQ *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Unsched_weight: %u\n\r"),info->unsched_weight));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "  Sched_weight: %u\n\r"),info->sched_weight));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_OFP_SCH_INFO_print(
    DNX_SAND_IN SOC_DNX_EGR_OFP_SCH_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Nif_priority %s \n\r"),
           SOC_DNX_EGR_OFP_INTERFACE_PRIO_to_string(info->nif_priority)
           ));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "Ofp_wfq: \n\r")));
  SOC_DNX_EGR_OFP_SCH_WFQ_print(&(info->ofp_wfq));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "High Ofp_wfq: \n\r")));
  SOC_DNX_EGR_OFP_SCH_WFQ_print(&(info->ofp_wfq_high));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_QUEUING_TCG_INFO_print(
    DNX_SAND_IN  SOC_DNX_EGR_QUEUING_TCG_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "tcg_ndx %d \n\r"),
           info->tcg_ndx[0]
           ));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_TCG_SCH_WFQ_print(
    DNX_SAND_IN  SOC_DNX_EGR_TCG_SCH_WFQ *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  if (info->tcg_weight_valid)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "tcg_weight %d \n\r"),
             info->tcg_weight
             ));
  }
  else
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "tcg_weight disable \n\r"
                 )));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_EGR_Q_PRIORITY_print(
    DNX_SAND_IN  SOC_DNX_EGR_Q_PRIORITY *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "dp: %u\n\r"),info->dp));
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


