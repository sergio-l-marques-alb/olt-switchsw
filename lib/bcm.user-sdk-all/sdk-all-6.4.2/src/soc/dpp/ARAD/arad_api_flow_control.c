
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_api_flow_control.c,v 1.18 Broadcom SDK $
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
*/
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FLOWCONTROL
/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_flow_control.h>


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

uint32
  arad_fc_init_pfc_mapping(
    SOC_SAND_IN  int                      unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_INIT_PFC_MAPPING);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_init_pfc_mapping_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_set()", 0, 0);
}

/*********************************************************************
*     Inband Flow Control Generation PFC refresh timer Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_pfc_timer_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID           nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_PFC_TIMER_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fc_pfc_timer_verify (unit, nif_ndx, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_pfc_timer_set_unsafe(unit, nif_ndx, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_set()", 0, 0);
}

uint32
  arad_fc_pfc_timer_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT  SOC_TMC_FC_PFC_TIMER_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fc_pfc_timer_verify (unit, nif_ndx, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_pfc_timer_get_unsafe(unit, nif_ndx, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_set()", 0, 0);
}

/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Arad Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fc_gen_inbnd_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_gen_inbnd_set_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_set()", 0, 0);
}

/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Arad Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fc_gen_inbnd_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_gen_inbnd_get_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_get()", 0, 0);
}

/*********************************************************************
*     Enable/disable Flow Control generation, based on Ingress
 *     Global Resources - high priority, via NIF. Flow Control
 *     generation may be either Link Level or Class Based. For
 *     Link Level - Flow Control will be generated on all
 *     links. For Class Based - Flow Control will be generated
 *     on all Flow Control Classes.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_glb_hp_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_fc_gen_inbnd_glb_hp_set_verify(
          unit,
          fc_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_gen_inbnd_glb_hp_set_unsafe(
          unit,
          fc_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_set()", 0, 0);
}

/*********************************************************************
*     Enable/disable Flow Control generation, based on Ingress
 *     Global Resources - high priority, via NIF. Flow Control
 *     generation may be either Link Level or Class Based. For
 *     Link Level - Flow Control will be generated on all
 *     links. For Class Based - Flow Control will be generated
 *     on all Flow Control Classes.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_glb_hp_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fc_mode);

  res = arad_fc_gen_inbnd_glb_hp_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_gen_inbnd_glb_hp_get_unsafe(
          unit,
          fc_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_get()", 0, 0);
}

/*********************************************************************
*     Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *     indication to generate when using Class Based Flow
 *     Control
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_cnm_map_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_fc_gen_inbnd_cnm_map_set_verify(
          unit,
          cpid_tc_ndx,
          enable_ll,
          fc_class
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_gen_inbnd_cnm_map_set_unsafe(
          unit,
          cpid_tc_ndx,
          enable_ll,
          fc_class
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_set()", cpid_tc_ndx, 0);
}

/*********************************************************************
*     Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *     indication to generate when using Class Based Flow
 *     Control
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_cnm_map_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_OUT uint8                      *enable_ll,
    SOC_SAND_OUT uint32                      *fc_class
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable_ll);
  SOC_SAND_CHECK_NULL_INPUT(fc_class);

  res = arad_fc_gen_inbnd_cnm_map_get_verify(
          unit,
          cpid_tc_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_gen_inbnd_cnm_map_get_unsafe(
          unit,
          cpid_tc_ndx,
          enable_ll,
          fc_class
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_get()", cpid_tc_ndx, 0);
}
/*********************************************************************
*     Inband Flow Control Reception Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_rec_inbnd_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_INBND_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fc_rec_inbnd_set_verify(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_rec_inbnd_set_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_inbnd_set()", 0, 0);
}

/*********************************************************************
*     Inband Flow Control Reception Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_rec_inbnd_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_TMC_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_INBND_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fc_rec_inbnd_get_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_rec_inbnd_get_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_inbnd_get()", 0, 0);
}
/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_fc_gen_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_CAL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = arad_fc_gen_cal_set_verify(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_gen_cal_set_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_cal_set()", 0, if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_CAL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = arad_fc_gen_cal_get_verify(
          unit,
          cal_mode_ndx,
          if_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_gen_cal_get_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_cal_get()", 0, if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_rec_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_CAL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = arad_fc_rec_cal_set_verify(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_rec_cal_set_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_cal_set()", 0, if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_rec_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_CAL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = arad_fc_rec_cal_get_verify(
          unit,
          cal_mode_ndx,
          if_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_rec_cal_get_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_cal_get()", 0, if_ndx);
}

/*********************************************************************
*     Diagnostics - get the Out-Of-Band interface status
 *     Details: in the H file. (search for prototype)
*********************************************************************/
/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_ilkn_llfc_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_LLFC_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fc_ilkn_llfc_set_verify(
          unit,
          ilkn_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_ilkn_llfc_set_unsafe(
          unit,
          ilkn_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_llfc_set()", 0, 0);
}


/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_ilkn_llfc_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *gen_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_LLFC_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rec_info);
  SOC_SAND_CHECK_NULL_INPUT(gen_info);

  res = arad_fc_ilkn_llfc_get_verify(
          unit,
          ilkn_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_ilkn_llfc_get_unsafe(
          unit,
          ilkn_ndx,
          rec_info,
          gen_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_llfc_get()", 0, 0);
}
/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_nif_pause_frame_src_addr_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  res = arad_fc_nif_pause_frame_src_addr_verify(
          unit,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_nif_pause_frame_src_addr_set_unsafe(
          unit,
          mal_ndx,
          mac_addr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_nif_pause_frame_src_addr_set()", mal_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_nif_pause_frame_src_addr_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  res = arad_fc_nif_pause_frame_src_addr_verify(
          unit,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_nif_pause_frame_src_addr_get_unsafe(
          unit,
          mal_ndx,
          mac_addr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_nif_pause_frame_src_addr_get()", mal_ndx, 0);
}

uint32
  arad_fc_pfc_generic_bitmap_get(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_OUT  SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_PFC_GENERIC_BITMAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pfc_bitmap);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_pfc_generic_bitmap_verify(
          unit,
          bitmap_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_fc_pfc_generic_bitmap_get_unsafe(
          unit,
          bitmap_index,
          pfc_bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_pfc_generic_bitmap_get()", bitmap_index, 0);
}

uint32
  arad_fc_pfc_generic_bitmap_set(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_IN  SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_PFC_GENERIC_BITMAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pfc_bitmap);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_pfc_generic_bitmap_verify(
          unit,
          bitmap_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_fc_pfc_generic_bitmap_set_unsafe(
          unit,
          bitmap_index,
          pfc_bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_pfc_generic_bitmap_set()", bitmap_index, 0);
}

uint32
  arad_fc_hcfc_bitmap_set(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GENERIC_BITMAP    *hcfc_bitmap
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_PFC_GENERIC_BITMAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(hcfc_bitmap);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_hcfc_bitmap_set_unsafe(
          unit,
          bitmap_index,
          hcfc_bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_hcfc_bitmap_set()", bitmap_index, 0);
}

uint32
  arad_fc_hcfc_bitmap_get(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_OUT  SOC_TMC_FC_PFC_GENERIC_BITMAP    *hcfc_bitmap
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_PFC_GENERIC_BITMAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(hcfc_bitmap);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fc_hcfc_bitmap_get_unsafe(
          unit,
          bitmap_index,
          hcfc_bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_hcfc_bitmap_get()", bitmap_index, 0);
}



#if defined(BCM_88650_B0)

uint32
  arad_fc_ilkn_retransmit_cal_set(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_CAL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_cfg);

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    res = arad_fc_ilkn_retransmit_cal_set_verify(
          unit,
          ilkn_ndx,
          direction_ndx,
          cal_cfg
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    res = arad_fc_ilkn_retransmit_cal_set_unsafe(
            unit,
            ilkn_ndx,
            direction_ndx,
            cal_cfg
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
  }

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_cal_set()", ilkn_ndx, direction_ndx);
}

uint32
  arad_fc_ilkn_retransmit_cal_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_CAL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_cfg);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    res = arad_fc_ilkn_retransmit_cal_get_unsafe(
            unit,
            ilkn_ndx,
            direction_ndx,
            cal_cfg
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
  }

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_cal_get()", ilkn_ndx, direction_ndx);
}

#endif

void 
  ARAD_FC_ILKN_RETRANSMIT_INFO_clear(
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_FC_ILKN_RETRANSMIT_INFO));
  info->enable = 0;
  info->error_indication = 0;
  info->rx_polarity= 0;
  info->tx_polarity = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if defined(BCM_88650_B0)
void 
  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG_clear(
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_CAL_CFG *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_FC_ILKN_RETRANSMIT_CAL_CFG));
  info->enable = 0;
  info->length = 1;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

