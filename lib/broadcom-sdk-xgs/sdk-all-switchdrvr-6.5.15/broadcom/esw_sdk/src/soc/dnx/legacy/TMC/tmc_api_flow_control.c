/* $Id: jer2_tmc_api_flow_control.c,v 1.7 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
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

#include <soc/dnx/legacy/TMC/tmc_api_flow_control.h>

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

void SOC_DNX_FC_PFC_GENERIC_BITMAP_clear(DNX_SAND_OUT SOC_DNX_FC_PFC_GENERIC_BITMAP *generic_bm)
{
  uint32
    i;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(generic_bm, _SHR_E_PARAM, "generic_bm");

  sal_memset(generic_bm, 0x0, sizeof(SOC_DNX_FC_PFC_GENERIC_BITMAP));

  for(i = 0; i<SOC_DNX_FC_PFC_GENERIC_BITMAP_SIZE / 32; i++)
  {
    generic_bm->bitmap[i] = 0;
  }

exit:
  SHR_VOID_FUNC_EXIT;
}

void SOC_DNX_FC_CAL_IF_INFO_clear(DNX_SAND_OUT SOC_DNX_FC_CAL_IF_INFO *cal_info)
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");

  sal_memset(cal_info, 0x0, sizeof(SOC_DNX_FC_CAL_IF_INFO));

  cal_info->enable = 0;
  cal_info->cal_len = 0;
  cal_info->cal_reps = 0;


exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FC_GEN_INBND_CB_clear(
    DNX_SAND_OUT SOC_DNX_FC_GEN_INBND_CB *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FC_GEN_INBND_CB));
  info->inherit = SOC_DNX_FC_NOF_INBND_CB_INHERITS;
  info->glbl_rcs_low = 0;
  info->cnm_intercept_enable = 0;
  info->nif_cls_bitmap = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}


void
  SOC_DNX_FC_GEN_INBND_LL_clear(
    DNX_SAND_OUT SOC_DNX_FC_GEN_INBND_LL *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FC_GEN_INBND_LL));
  info->cnm_enable = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FC_REC_INBND_CB_clear(
    DNX_SAND_OUT SOC_DNX_FC_REC_INBND_CB *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FC_REC_INBND_CB));
  info->inherit = SOC_DNX_FC_NOF_INBND_CB_INHERITS;
  info->sch_hr_bitmap = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FC_GEN_INBND_INFO_clear(
    DNX_SAND_OUT SOC_DNX_FC_GEN_INBND_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FC_GEN_INBND_INFO));
  info->mode = SOC_DNX_FC_NOF_INBND_MODES;
  SOC_DNX_FC_GEN_INBND_CB_clear(&(info->cb));
  SOC_DNX_FC_GEN_INBND_LL_clear(&(info->ll));
  SOC_DNX_FC_GEN_INBND_PFC_clear(&(info->pfc));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FC_GEN_INBND_PFC_clear(
    DNX_SAND_OUT SOC_DNX_FC_GEN_INBND_PFC *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  dnx_sand_os_memset(info, 0x0, sizeof(SOC_DNX_FC_GEN_INBND_PFC));
  info->inherit = SOC_DNX_FC_NOF_INBND_PFC_INHERITS;
  info->glbl_rcs_low = 0;
  info->cnm_intercept_enable = 0;
  info->nif_cls_bitmap = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FC_REC_CALENDAR_clear(
    DNX_SAND_OUT SOC_DNX_FC_REC_CALENDAR *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FC_REC_CALENDAR));
  info->destination = SOC_DNX_FC_NOF_REC_CAL_DESTS;
  info->id = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FC_GEN_CALENDAR_clear(
    DNX_SAND_OUT SOC_DNX_FC_GEN_CALENDAR *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FC_GEN_CALENDAR));
  info->source = SOC_DNX_FC_NOF_GEN_CAL_SRCS;
  info->id = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FC_REC_INBND_INFO_clear(
    DNX_SAND_OUT SOC_DNX_FC_REC_INBND_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FC_REC_INBND_INFO));
  info->mode = SOC_DNX_FC_NOF_INBND_MODES;
  SOC_DNX_FC_REC_INBND_CB_clear(&(info->cb));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FC_REC_OFP_MAP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_FC_REC_OFP_MAP_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FC_REC_OFP_MAP_INFO));
  info->react_point = SOC_DNX_FC_NOF_REC_OFP_RPS;
  info->ofp_ndx = 0;
  info->priority = SOC_DNX_FC_NOF_OFP_PRIORITYS;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_FC_ILKN_LLFC_INFO_clear(
    DNX_SAND_OUT SOC_DNX_FC_ILKN_LLFC_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_FC_ILKN_LLFC_INFO));
  info->multi_use_mask = 0;
  info->cal_channel = SOC_DNX_FC_NOF_ILKN_CAL_LLFCS;
exit:
  SHR_VOID_FUNC_EXIT;
}

/* } */

