/* $Id: jer2_tmc_api_cnm.c,v 1.8 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_cnm.c
*
* MODULE PREFIX:  jer2_tmc
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


#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

#include <soc/dnx/legacy/TMC/tmc_api_cnm.h>

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
  SOC_DNX_CNM_Q_MAPPING_INFO_clear(
    DNX_SAND_OUT SOC_DNX_CNM_Q_MAPPING_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CNM_Q_MAPPING_INFO));
  info->q_base = 0;
  info->q_set = SOC_DNX_CNM_NOF_Q_SETS;
  info->nof_queues = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_CONGESTION_TEST_INFO_clear(
    DNX_SAND_OUT SOC_DNX_CNM_CONGESTION_TEST_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CNM_CONGESTION_TEST_INFO));
  info->is_mc_also = 0;
  info->is_ingr_rep_also = 0;
  info->is_snoop_also = 0;
  info->is_mirr_also = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_CP_PROFILE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_CNM_CP_PROFILE_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CNM_CP_PROFILE_INFO));
  info->q_eq = 0;
  info->cpw_power = 0;
  for (ind = 0; ind < SOC_DNX_CNM_NOF_SAMPLING_BASES; ++ind)
  {
    info->sampling_base[ind] = 0;
  }
  info->max_neg_fb_value = 0;
  info->quant_div = 0;
  info->is_sampling_th_random = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_CPQ_INFO_clear(
    DNX_SAND_OUT SOC_DNX_CNM_CPQ_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CNM_CPQ_INFO));
  info->is_cp_enabled = 0;
  info->profile = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_PPH_clear(
    DNX_SAND_OUT SOC_DNX_CNM_PPH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CNM_PPH));
  info->ing_vlan_edit_cmd = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_PDU_clear(
    DNX_SAND_OUT SOC_DNX_CNM_PDU *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CNM_PDU));
  info->ether_type = 0;
  info->version = 0;
  info->res_v = 0;
  for (ind = 0; ind < SOC_DNX_CNM_NOF_UINT32S_IN_CP_ID; ++ind)
  {
    info->cp_id_6_msb[ind] = 0;
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_PETRA_B_PP_clear(
    DNX_SAND_OUT SOC_DNX_CNM_PETRA_B_PP *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CNM_PETRA_B_PP));
  SOC_DNX_CNM_PPH_clear(&(info->pph));
  SOC_DNX_CNM_PDU_clear(&(info->pdu));
  info->ether_type = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_PACKET_clear(
    DNX_SAND_OUT SOC_DNX_CNM_PACKET *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CNM_PACKET));
  info->tc = 0;
  info->dp = 0;
  info->qsig = 0;
  info->cp_id_4_msb = 0;
  info->dest_tm_port = SOC_TCM_CNM_DEST_TM_PORT_AS_INCOMING;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_CP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_CNM_CP_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_CNM_CP_INFO));
  info->is_cp_enabled = 0;
  SOC_DNX_CNM_PETRA_B_PP_clear(&(info->pp));
  info->pkt_gen_mode = SOC_DNX_CNM_NOF_GEN_MODES;
  SOC_DNX_CNM_PACKET_clear(&(info->pckt));
exit:
  SHR_VOID_FUNC_EXIT;
}


const char*
  SOC_DNX_CNM_GEN_MODE_to_string(
    DNX_SAND_IN  SOC_DNX_CNM_GEN_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_CNM_GEN_MODE_EXT_PP:
    str = "ext_pp";
  break;
  case SOC_DNX_CNM_GEN_MODE_PETRA_B_PP:
    str = "dune_pp1";
  break;
  case SOC_DNX_CNM_GEN_MODE_SAMPLING:
    str = "sampling";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_CNM_Q_SET_to_string(
    DNX_SAND_IN  SOC_DNX_CNM_Q_SET enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_CNM_Q_SET_8_CPS:
    str = "set_8_cps";
  break;
  case SOC_DNX_CNM_Q_SET_4_CPS:
    str = "set_4_cps";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_DNX_CNM_Q_MAPPING_INFO_print(
    DNX_SAND_IN  SOC_DNX_CNM_Q_MAPPING_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "q_base: %u\n\r"),info->q_base));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "q_set %s "), SOC_DNX_CNM_Q_SET_to_string(info->q_set)));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "nof_queues: %u\n\r"),info->nof_queues));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_CONGESTION_TEST_INFO_print(
    DNX_SAND_IN  SOC_DNX_CNM_CONGESTION_TEST_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_mc_also: %u\n\r"),info->is_mc_also));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_ingr_rep_also: %u\n\r"),info->is_ingr_rep_also));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_snoop_also: %u\n\r"),info->is_snoop_also));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_mirr_also: %u\n\r"),info->is_mirr_also));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_CP_PROFILE_INFO_print(
    DNX_SAND_IN  SOC_DNX_CNM_CP_PROFILE_INFO *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "q_eq: %u[Bytes]\n\r"),info->q_eq));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "cpw_power: %d[Bytes]\n\r"),info->cpw_power));
  for (ind = 0; ind < SOC_DNX_CNM_NOF_SAMPLING_BASES; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "sampling_base[%u]: %u[Bytes]\n\r"),ind,info->sampling_base[ind]));
  }
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "max_neg_fb_value: %u[Bytes]\n\r"),info->max_neg_fb_value));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "quant_div: %u\n\r"),info->quant_div));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_sampling_th_random: %u\n\r"),info->is_sampling_th_random));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_CPQ_INFO_print(
    DNX_SAND_IN  SOC_DNX_CNM_CPQ_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_cp_enabled: %u\n\r"),info->is_cp_enabled));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "profile: %u\n\r"),info->profile));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_PPH_print(
    DNX_SAND_IN  SOC_DNX_CNM_PPH *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "ing_vlan_edit_cmd: %u\n\r"),info->ing_vlan_edit_cmd));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_PDU_print(
    DNX_SAND_IN  SOC_DNX_CNM_PDU *info
  )
{
  uint32
    ind;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "ether_type: %u\n\r"),info->ether_type));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "version: %u\n\r"),info->version));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "res_v: %u\n\r"),info->res_v));
  for (ind = 0; ind < SOC_DNX_CNM_NOF_UINT32S_IN_CP_ID; ++ind)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "cp_id_6_msb[%u]: %u\n\r"),ind,info->cp_id_6_msb[ind]));
  }
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_PETRA_B_PP_print(
    DNX_SAND_IN  SOC_DNX_CNM_PETRA_B_PP *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "pph:")));
  SOC_DNX_CNM_PPH_print(&(info->pph));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "pdu:")));
  SOC_DNX_CNM_PDU_print(&(info->pdu));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "ether_type: %u\n\r"),info->ether_type));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_PACKET_print(
    DNX_SAND_IN SOC_DNX_CNM_PACKET *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "cp_id_4_msb: %u\n\r"),info->cp_id_4_msb));

  if (info->dest_tm_port == SOC_TCM_CNM_DEST_TM_PORT_AS_INCOMING)
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "dest_tm_port: same as incoming\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(NO_UNIT,
                        "dest_tm_port: %u\n\r"),info->dest_tm_port));
  }

exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_CNM_CP_INFO_print(
    DNX_SAND_IN  SOC_DNX_CNM_CP_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "is_cp_enabled: %u\n\r"),info->is_cp_enabled));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "pckt:")));
  SOC_DNX_CNM_PACKET_print(&(info->pckt));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "pkt_gen_mode %s "), SOC_DNX_CNM_GEN_MODE_to_string(info->pkt_gen_mode)));
  if (info->pkt_gen_mode == SOC_DNX_CNM_GEN_MODE_PETRA_B_PP)
  {
    SOC_DNX_CNM_PETRA_B_PP_print(&(info->pp));
  }

exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


