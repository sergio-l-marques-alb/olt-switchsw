/* $Id: pb_api_flow_control.c,v 1.11 Broadcom SDK $
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
*/
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_api_flow_control.c
*
* MODULE PREFIX:  soc_pb_fc
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_flow_control.h>
#include <soc/dpp/Petra/PB_TM/pb_flow_control.h>

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

/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Soc_petra Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_fc_gen_inbnd_set_verify(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_gen_inbnd_set_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_set()", 0, 0);
}

/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Soc_petra Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_fc_gen_inbnd_get_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_gen_inbnd_get_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_get()", 0, 0);
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
  soc_pb_fc_gen_inbnd_glb_hp_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_GLB_HP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_fc_gen_inbnd_glb_hp_set_verify(
          unit,
          fc_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_gen_inbnd_glb_hp_set_unsafe(
          unit,
          fc_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_glb_hp_set()", 0, 0);
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
  soc_pb_fc_gen_inbnd_glb_hp_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_GLB_HP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fc_mode);

  res = soc_pb_fc_gen_inbnd_glb_hp_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_gen_inbnd_glb_hp_get_unsafe(
          unit,
          fc_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_glb_hp_get()", 0, 0);
}

/*********************************************************************
*     Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *     indication to generate when using Class Based Flow
 *     Control
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_cnm_map_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_CNM_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_fc_gen_inbnd_cnm_map_set_verify(
          unit,
          cpid_tc_ndx,
          enable_ll,
          fc_class
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_gen_inbnd_cnm_map_set_unsafe(
          unit,
          cpid_tc_ndx,
          enable_ll,
          fc_class
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_cnm_map_set()", cpid_tc_ndx, 0);
}

/*********************************************************************
*     Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *     indication to generate when using Class Based Flow
 *     Control
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_cnm_map_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_OUT uint8                      *enable_ll,
    SOC_SAND_OUT uint32                      *fc_class
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_CNM_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable_ll);
  SOC_SAND_CHECK_NULL_INPUT(fc_class);

  res = soc_pb_fc_gen_inbnd_cnm_map_get_verify(
          unit,
          cpid_tc_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_gen_inbnd_cnm_map_get_unsafe(
          unit,
          cpid_tc_ndx,
          enable_ll,
          fc_class
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_cnm_map_get()", cpid_tc_ndx, 0);
}

/*********************************************************************
*     Inband Flow Control Reception Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_fc_rec_inbnd_set_verify(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rec_inbnd_set_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_set()", 0, 0);
}

/*********************************************************************
*     Inband Flow Control Reception Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_fc_rec_inbnd_get_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rec_inbnd_get_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_get()", 0, 0);
}

/*********************************************************************
*     Selects the OFP, priority and Reaction Point for inband
 *     FC reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_ofp_map_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO         *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_OFP_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_fc_rec_inbnd_ofp_map_set_verify(
          unit,
          nif_ndx,
          fc_cls_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rec_inbnd_ofp_map_set_unsafe(
          unit,
          nif_ndx,
          fc_cls_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_ofp_map_set()", 0, fc_cls_ndx);
}

/*********************************************************************
*     Selects the OFP, priority and Reaction Point for inband
 *     FC reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_ofp_map_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx,
    SOC_SAND_OUT SOC_PB_FC_REC_OFP_MAP_INFO         *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_OFP_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_fc_rec_inbnd_ofp_map_get_verify(
          unit,
          nif_ndx,
          fc_cls_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rec_inbnd_ofp_map_get_unsafe(
          unit,
          nif_ndx,
          fc_cls_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_ofp_map_get()", 0, fc_cls_ndx);
}

/*********************************************************************
*     Set the configuration for handling Flow Control
 *     generated by (and effecting) recycling traffic. Can be
 *     triggered by crossing Virtual Statistics Queues (CT3CC),
 *     or by crossing High or Low Global Resources threshold.
 *     Handled on Outgoing FAP Port level - Scheduler HR or
 *     EGQ.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rcy_ofp_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_OFP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_fc_rcy_ofp_set_verify(
          unit,
          trgr_ndx,
          react_point_ndx,
          prio_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rcy_ofp_set_unsafe(
          unit,
          trgr_ndx,
          react_point_ndx,
          prio_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_ofp_set()", trgr_ndx, 0);
}

/*********************************************************************
*     Set the configuration for handling Flow Control
 *     generated by (and effecting) recycling traffic. Can be
 *     triggered by crossing Virtual Statistics Queues (CT3CC),
 *     or by crossing High or Low Global Resources threshold.
 *     Handled on Outgoing FAP Port level - Scheduler HR or
 *     EGQ.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rcy_ofp_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_OUT SOC_PB_FC_RCY_PORT_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_OFP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_fc_rcy_ofp_get_verify(
          unit,
          trgr_ndx,
          react_point_ndx,
          prio_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rcy_ofp_get_unsafe(
          unit,
          trgr_ndx,
          react_point_ndx,
          prio_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_ofp_get()", trgr_ndx, 0);
}

/*********************************************************************
*     Enable/disable reception of Flow Control for Recycling
 *     ports by end to end Scheduler HR-s 192-223, represented
 *     by Category 3 Connection Class in Ingress Statistics
 *     Queues, when the Statistics Queues threshold is crossed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rcy_hr_enable_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_HR_ENABLE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_fc_rcy_hr_enable_set_verify(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rcy_hr_enable_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_hr_enable_set()", 0, 0);
}

/*********************************************************************
*     Enable/disable reception of Flow Control for Recycling
 *     ports by end to end Scheduler HR-s 192-223, represented
 *     by Category 3 Connection Class in Ingress Statistics
 *     Queues, when the Statistics Queues threshold is crossed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rcy_hr_enable_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint8                      *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_HR_ENABLE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = soc_pb_fc_rcy_hr_enable_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rcy_hr_enable_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_hr_enable_get()", 0, 0);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_CAL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = soc_pb_fc_gen_cal_set_verify(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_gen_cal_set_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_cal_set()", 0, if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_PB_FC_GEN_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_CAL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = soc_pb_fc_gen_cal_get_verify(
          unit,
          cal_mode_ndx,
          if_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_gen_cal_get_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_cal_get()", 0, if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_CAL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = soc_pb_fc_rec_cal_set_verify(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rec_cal_set_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_cal_set()", 0, if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_PB_FC_REC_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_CAL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = soc_pb_fc_rec_cal_get_verify(
          unit,
          cal_mode_ndx,
          if_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_rec_cal_get_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          cal_conf,
          cal_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_cal_get()", 0, if_ndx);
}

/*********************************************************************
*     Configures physical parameters for out-of-band Flow 
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_oob_phy_params_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_PHY_PARAMS_INFO          *phy_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_OOB_PHY_PARAMS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(phy_params);

  res = soc_pb_fc_oob_phy_params_set_verify(
          unit,
          if_ndx,
          phy_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_oob_phy_params_set_unsafe(
          unit,
          if_ndx,
          phy_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_oob_phy_params_set()", 0, if_ndx);
}

/*********************************************************************
*     Configures physical parameters for out-of-band Flow 
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_oob_phy_params_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT  SOC_PB_FC_PHY_PARAMS_INFO         *phy_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_OOB_PHY_PARAMS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(phy_params);

  res = soc_pb_fc_oob_phy_params_get_verify(
          unit,
          if_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_oob_phy_params_get_unsafe(
          unit,
          if_ndx,
          phy_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_oob_phy_params_get()", 0, if_ndx);
}

/*********************************************************************
*     Diagnostics - get the Out-Of-Band interface status
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_egr_rec_oob_stat_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   oob_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_REC_STAT             *status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_EGR_REC_OOB_STAT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(status);

  res = soc_pb_fc_egr_rec_oob_stat_get_verify(
          unit,
          oob_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_egr_rec_oob_stat_get_unsafe(
          unit,
          oob_ndx,
          status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_egr_rec_oob_stat_get()", 0, 0);
}

/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_ilkn_llfc_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_ILKN_LLFC_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_fc_ilkn_llfc_set_verify(
          unit,
          ilkn_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_ilkn_llfc_set_unsafe(
          unit,
          ilkn_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_ilkn_llfc_set()", 0, 0);
}

/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_ilkn_llfc_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO           *gen_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_ILKN_LLFC_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rec_info);
  SOC_SAND_CHECK_NULL_INPUT(gen_info);

  res = soc_pb_fc_ilkn_llfc_get_verify(
          unit,
          ilkn_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_ilkn_llfc_get_unsafe(
          unit,
          ilkn_ndx,
          rec_info,
          gen_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_ilkn_llfc_get()", 0, 0);
}

/*********************************************************************
*     Defines FC Oversubscription scheme on the NIFs. For up
 *     to 100GE traffic on the device NIFs should be set to
 *     'NONE'. Otherwise, should be set according to the
 *     oversubscription level. The scheme affects internal NIF
 *     FC thresholds each scheme defines a different thresholds
 *     preset.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_oversubscr_scheme_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    scheme
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_fc_nif_oversubscr_scheme_set_verify(
          unit,
          scheme
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_nif_oversubscr_scheme_set_unsafe(
          unit,
          scheme
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_oversubscr_scheme_set()", 0, 0);
}

/*********************************************************************
*     Defines FC Oversubscription scheme on the NIFs. For up
 *     to 100GE traffic on the device NIFs should be set to
 *     'NONE'. Otherwise, should be set according to the
 *     oversubscription level. The scheme affects internal NIF
 *     FC thresholds each scheme defines a different thresholds
 *     preset.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_oversubscr_scheme_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    *scheme
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(scheme);

  res = soc_pb_fc_nif_oversubscr_scheme_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_nif_oversubscr_scheme_get_unsafe(
          unit,
          scheme
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_oversubscr_scheme_get()", 0, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_quanta_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  uint32                       pause_quanta
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_QUANTA_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_fc_nif_pause_quanta_set_verify(
          unit,
          mal_ndx,
          pause_quanta
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_nif_pause_quanta_set_unsafe(
          unit,
          mal_ndx,
          pause_quanta
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_quanta_set()", mal_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_quanta_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT uint32                       *pause_quanta
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_QUANTA_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pause_quanta);

  res = soc_pb_fc_nif_pause_quanta_get_verify(
          unit,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_nif_pause_quanta_get_unsafe(
          unit,
          mal_ndx,
          pause_quanta
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_quanta_get()", mal_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_frame_src_addr_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  res = soc_pb_fc_nif_pause_frame_src_addr_set_verify(
          unit,
          mal_ndx,
          mac_addr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_nif_pause_frame_src_addr_set_unsafe(
          unit,
          mal_ndx,
          mac_addr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_frame_src_addr_set()", mal_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_frame_src_addr_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  res = soc_pb_fc_nif_pause_frame_src_addr_get_verify(
          unit,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_nif_pause_frame_src_addr_get_unsafe(
          unit,
          mal_ndx,
          mac_addr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_frame_src_addr_get()", mal_ndx, 0);
}

/*********************************************************************
*     Enable/disable the mapping between the VSQ-Pointer and
 *     the Incoming-NIF port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_vsq_by_incoming_nif_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint8 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_VSQ_BY_INCOMING_NIF_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_vsq_by_incoming_nif_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_vsq_by_incoming_nif_set()", 0, 0);
}

/*********************************************************************
*     Enable/disable the mapping between the VSQ-Pointer and
 *     the Incoming-NIF port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_vsq_by_incoming_nif_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_VSQ_BY_INCOMING_NIF_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_fc_vsq_by_incoming_nif_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_vsq_by_incoming_nif_get()", 0, 0);
}

void
  SOC_PB_FC_GEN_INBND_CB_clear(
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_CB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_GEN_INBND_CB));
  info->inherit = SOC_PB_FC_NOF_INBND_CB_INHERITS;
  info->glbl_rcs_low = 0;
  info->cnm_intercept_enable = 0;
  info->nif_cls_bitmap = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_GEN_INBND_LL_clear(
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_LL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_GEN_INBND_LL));
  info->cnm_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_GEN_INBND_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_GEN_INBND_INFO));
  info->mode = SOC_PB_FC_NOF_INBND_MODES;
  SOC_PB_FC_GEN_INBND_CB_clear(&(info->cb));
  SOC_PB_FC_GEN_INBND_LL_clear(&(info->ll));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_REC_INBND_CB_clear(
    SOC_SAND_OUT SOC_PB_FC_REC_INBND_CB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_REC_INBND_CB));
  info->inherit = SOC_PB_FC_NOF_INBND_CB_INHERITS;
  info->sch_hr_bitmap = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_REC_INBND_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_REC_INBND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_REC_INBND_INFO));
  info->mode = SOC_PB_FC_NOF_INBND_MODES;
  SOC_PB_FC_REC_INBND_CB_clear(&(info->cb));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_REC_OFP_MAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_REC_OFP_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_REC_OFP_MAP_INFO));
  info->react_point = SOC_PB_FC_NOF_REC_OFP_RPS;
  info->ofp_ndx = 0;
  info->priority = SOC_TMC_FC_NOF_OFP_PRIORITYS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_RCY_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_RCY_PORT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_RCY_PORT_INFO));
  for (ind = 0; ind < SOC_PB_FC_NOF_FAP_PORTS_UINT32S; ++ind)
  {
    info->ofp_bitmap[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PB_FC_CAL_IF_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_CAL_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_CAL_IF_INFO));
  info->enable = 0;
  info->cal_len = 0;
  info->cal_reps = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_PHY_PARAMS_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_PHY_PARAMS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_PHY_PARAMS_INFO));
  info->is_on_if_oof = FALSE;
  info->is_sampled_rising_edge = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_CAL_REC_STAT_clear(
    SOC_SAND_OUT SOC_PB_FC_CAL_REC_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_CAL_REC_STAT));
  info->not_locked = 0;
  info->out_of_frame = 0;
  info->dip2_alarm = 0;
  info->nof_frame_errors = 0;
  info->nof_dip2_errors = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_GEN_CALENDAR_clear(
    SOC_SAND_OUT SOC_PB_FC_GEN_CALENDAR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_GEN_CALENDAR));
  info->source = SOC_PB_FC_NOF_GEN_CAL_SRCS;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_REC_CALENDAR_clear(
    SOC_SAND_OUT SOC_PB_FC_REC_CALENDAR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_REC_CALENDAR));
  info->destination = SOC_PB_FC_NOF_REC_CAL_DESTS;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_ILKN_LLFC_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_FC_ILKN_LLFC_INFO));
  info->multi_use_mask = 0;
  info->cal_channel = SOC_TMC_FC_NOF_ILKN_CAL_LLFCS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_FC_OFP_PRIORITY_to_string(
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FC_OFP_PRIORITY_LP:
    str = "lp";
  break;
  case SOC_TMC_FC_OFP_PRIORITY_HP:
    str = "hp";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_INBND_MODE_to_string(
    SOC_SAND_IN  SOC_PB_FC_INBND_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_INBND_MODE_DISABLED:
    str = "disabled";
  break;
  case SOC_PB_FC_INBND_MODE_LL:
    str = "ll";
  break;
  case SOC_PB_FC_INBND_MODE_CB:
    str = "cb";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_INBND_CB_INHERIT_to_string(
    SOC_SAND_IN  SOC_PB_FC_INBND_CB_INHERIT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_INBND_CB_INHERIT_DISABLED:
    str = "disabled";
  break;
  case SOC_PB_FC_INBND_CB_INHERIT_UP:
    str = "up";
  break;
  case SOC_PB_FC_INBND_CB_INHERIT_DOWN:
    str = "down";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_INGR_GEN_GLB_HP_MODE_to_string(
    SOC_SAND_IN  SOC_PB_FC_INGR_GEN_GLB_HP_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_NONE:
    str = "none";
  break;
  case SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_LL:
    str = "ll";
  break;
  case SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_CB:
    str = "cb";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_REC_OFP_RP_to_string(
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_RP enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_REC_OFP_RP_SCH:
    str = "sch";
  break;
  case SOC_PB_FC_REC_OFP_RP_EGQ:
    str = "egq";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_RCY_OFP_HANDLER_to_string(
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_RCY_OFP_HANDLER_SCH:
    str = "sch";
  break;
  case SOC_PB_FC_RCY_OFP_HANDLER_EGQ:
    str = "egq";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_RCY_GLB_RCS_ID_to_string(
    SOC_SAND_IN  SOC_PB_FC_RCY_GLB_RCS_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_RCY_GLB_RCS_ID_LP:
    str = "lp";
  break;
  case SOC_PB_FC_RCY_GLB_RCS_ID_HP:
    str = "hp";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_CAL_MODE_to_string(
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FC_CAL_MODE_SPI_OOB:
    str = "spi_oob";
  break;
  case SOC_TMC_FC_CAL_MODE_ILKN_INBND:
    str = "ilkn_inbnd";
  break;
  case SOC_TMC_FC_CAL_MODE_ILKN_OOB:
    str = "ilkn_oob";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_OOB_ID_to_string(
    SOC_SAND_IN  SOC_PB_FC_OOB_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FC_OOB_ID_A:
    str = "a";
  break;
  case SOC_TMC_FC_OOB_ID_B:
    str = "b";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_GEN_CAL_SRC_to_string(
    SOC_SAND_IN  SOC_PB_FC_GEN_CAL_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_GEN_CAL_SRC_STE:
    str = "ste";
  break;
  case SOC_PB_FC_GEN_CAL_SRC_NIF:
    str = "nif";
  break;
  case SOC_PB_FC_GEN_CAL_SRC_GLB_HP:
    str = "glb_hp";
  break;
  case SOC_PB_FC_GEN_CAL_SRC_GLB_LP:
    str = "glb_lp";
  break;
  case SOC_PB_FC_GEN_CAL_SRC_NONE:
    str = "none";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_CAL_GLB_RCS_ID_to_string(
    SOC_SAND_IN  SOC_PB_FC_CAL_GLB_RCS_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_CAL_GLB_RCS_ID_BDB:
    str = "bdb";
  break;
  case SOC_PB_FC_CAL_GLB_RCS_ID_UNI:
    str = "uni";
  break;
  case SOC_PB_FC_CAL_GLB_RCS_ID_MUL:
    str = "mul";
  break;
  case SOC_PB_FC_NOF_CAL_GLB_RCS_IDS:
    str = "nof_cal_glb_rcs_ids";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_REC_CAL_DEST_to_string(
    SOC_SAND_IN  SOC_PB_FC_REC_CAL_DEST enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_REC_CAL_DEST_HR:
    str = "hr";
  break;
  case SOC_PB_FC_REC_CAL_DEST_OFP_EGQ_LP:
    str = "ofp_egq_lp";
  break;
  case SOC_PB_FC_REC_CAL_DEST_OFP_EGQ_HP:
    str = "ofp_egq_hp";
  break;
  case SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_LP:
    str = "ofp_sch_hr_lp";
  break;
  case SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_HP:
    str = "ofp_sch_hr_hp";
  break;
  case SOC_PB_FC_REC_CAL_DEST_NIF:
    str = "nif";
  break;
  case SOC_PB_FC_REC_CAL_DEST_NONE:
    str = "none";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_ILKN_CAL_LLFC_to_string(
    SOC_SAND_IN  SOC_PB_FC_ILKN_CAL_LLFC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FC_ILKN_CAL_LLFC_NONE:
    str = "none";
  break;
  case SOC_TMC_FC_ILKN_CAL_LLFC_CH_0:
    str = "ch_0";
  break;
  case SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N:
    str = "ch_16n";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_to_string(
    SOC_SAND_IN  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_NONE:
    str = "none";
  break;
  case SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_120GE_00:
    str = "scheme_120ge_00";
  break;
  case SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_160GE_00:
    str = "scheme_160ge_00";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_FC_EXTEND_Q_NOF_PRIOS_to_string(
    SOC_SAND_IN  SOC_PB_FC_EXTEND_Q_NOF_PRIOS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_FC_EXTEND_Q_NOF_PRIOS_2:
    str = "nof_prios_2";
  break;
  case SOC_PB_FC_EXTEND_Q_NOF_PRIOS_4:
    str = "nof_prios_4";
  break;
  case SOC_PB_FC_EXTEND_Q_NOF_PRIOS_6:
    str = "nof_prios_6";
  break;
  case SOC_PB_FC_EXTEND_Q_NOF_PRIOS_8:
    str = "nof_prios_8";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

void
  SOC_PB_FC_GEN_INBND_CB_print(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_CB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("inherit %s ", SOC_PB_FC_INBND_CB_INHERIT_to_string(info->inherit));
  soc_sand_os_printf("glbl_rcs_low: %u\n\r", info->glbl_rcs_low);
  soc_sand_os_printf("cnm_intercept_enable: %u\n\r",info->cnm_intercept_enable);
  soc_sand_os_printf("nif_cls_bitmap: %u\n\r", info->nif_cls_bitmap);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_GEN_INBND_LL_print(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_LL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("cnm_enable: %u\n\r",info->cnm_enable);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_GEN_INBND_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mode %s ", SOC_PB_FC_INBND_MODE_to_string(info->mode));
  soc_sand_os_printf("cb:");
  SOC_PB_FC_GEN_INBND_CB_print(&(info->cb));
  soc_sand_os_printf("ll:");
  SOC_PB_FC_GEN_INBND_LL_print(&(info->ll));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_REC_INBND_CB_print(
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_CB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("inherit %s ", SOC_PB_FC_INBND_CB_INHERIT_to_string(info->inherit));
  soc_sand_os_printf("sch_hr_bitmap: %u\n\r", info->sch_hr_bitmap);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_REC_INBND_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mode %s ", SOC_PB_FC_INBND_MODE_to_string(info->mode));
  soc_sand_os_printf("cb:");
  SOC_PB_FC_REC_INBND_CB_print(&(info->cb));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_REC_OFP_MAP_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("react_point %s ", SOC_PB_FC_REC_OFP_RP_to_string(info->react_point));
  soc_sand_os_printf("ofp_ndx: %u\n\r",info->ofp_ndx);
  soc_sand_os_printf("priority %s ", SOC_PB_FC_OFP_PRIORITY_to_string(info->priority));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_RCY_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_FC_NOF_FAP_PORTS_UINT32S; ++ind)
  {
    soc_sand_os_printf("ofp_bitmap[%u]: %u\n\r",ind,info->ofp_bitmap[ind]);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_CAL_IF_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("cal_len: %u\n\r",info->cal_len);
  soc_sand_os_printf("cal_reps: %u\n\r",info->cal_reps);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_PHY_PARAMS_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_PHY_PARAMS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("is_on_if_oof: %u\n\r",info->is_on_if_oof);
  soc_sand_os_printf("is_sampled_rising_edge: %u\n\r",info->is_sampled_rising_edge);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_CAL_REC_STAT_print(
    SOC_SAND_IN  SOC_PB_FC_CAL_REC_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("not_locked: %u\n\r",info->not_locked);
  soc_sand_os_printf("out_of_frame: %u\n\r",info->out_of_frame);
  soc_sand_os_printf("dip2_alarm: %u\n\r",info->dip2_alarm);
  soc_sand_os_printf("nof_frame_errors: %u\n\r",info->nof_frame_errors);
  soc_sand_os_printf("nof_dip2_errors: %u\n\r",info->nof_dip2_errors);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_GEN_CALENDAR_print(
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("source %s ", SOC_PB_FC_GEN_CAL_SRC_to_string(info->source));
  soc_sand_os_printf("id: %u\n\r",info->id);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_REC_CALENDAR_print(
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("destination %s ", SOC_PB_FC_REC_CAL_DEST_to_string(info->destination));
  soc_sand_os_printf("id: %u\n\r",info->id);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_FC_ILKN_LLFC_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("multi_use_mask: %u\n\r", info->multi_use_mask);
  soc_sand_os_printf("cal_channel %s ", SOC_PB_FC_ILKN_CAL_LLFC_to_string(info->cal_channel));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

