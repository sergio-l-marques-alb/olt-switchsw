/* $Id: petra_api_flow_control.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_flow_control.c
*
* MODULE PREFIX:  soc_petra_fc
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

#include <soc/dpp/Petra/petra_api_flow_control.h>
#include <soc/dpp/Petra/petra_flow_control.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

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
*     Configure Flow Control generation, based on Ingress
*     Statistics Queues, via NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_vsq_via_nif_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   /*
    *	Invalid for Soc_petra-B
    */
   SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);
 
  res = soc_petra_fc_ingr_gen_vsq_via_nif_verify(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fc_ingr_gen_vsq_via_nif_set_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_vsq_via_nif_set()",0,0);
}

/*********************************************************************
*     Configure Flow Control generation, based on Ingress
*     Statistics Queues, via NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_vsq_via_nif_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_ingr_gen_vsq_via_nif_get_unsafe(
    unit,
    nif_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_vsq_via_nif_get()",0,0);
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
  soc_petra_fc_ingr_gen_glb_hp_via_nif_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE fc_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);


  res = soc_petra_fc_ingr_gen_glb_hp_via_nif_verify(
    unit,
    fc_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fc_ingr_gen_glb_hp_via_nif_set_unsafe(
    unit,
    fc_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_hp_via_nif_set()",0,0);
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
  soc_petra_fc_ingr_gen_glb_hp_via_nif_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE *fc_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fc_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_ingr_gen_glb_hp_via_nif_get_unsafe(
    unit,
    fc_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_hp_via_nif_get()",0,0);
}

/*********************************************************************
*     Configure Flow Control generation, based on Ingress
*     Global Resources - low priority, via NIF, Class Based.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_verify(
    unit,
    nif_ndx,
    cls_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_set_unsafe(
    unit,
    nif_ndx,
    cls_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_set()",0,0);
}

/*********************************************************************
*     Configure Flow Control generation, based on Ingress
*     Global Resources - low priority, via NIF, Class Based.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_get_unsafe(
    unit,
    nif_ndx,
    cls_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_get()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Generation (OOB TX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_oob_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_OOB_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_ingr_gen_oob_verify(
    unit,
    oob_conf,
    cal_buff
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fc_ingr_gen_oob_set_unsafe(
    unit,
    oob_conf,
    cal_buff
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_oob_set()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Generation (OOB TX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_oob_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_OUT SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_OOB_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_ingr_gen_oob_get_unsafe(
    unit,
    oob_conf,
    cal_buff
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_oob_get()",0,0);
}

/*********************************************************************
*     Diagnostics - get the Out-Of-Band interface status
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_fc_egr_rec_oob_stat_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_EGR_REC_OOB_STAT *status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_EGR_REC_OOB_STAT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

 res = soc_petra_fc_egr_rec_oob_stat_get_unsafe(
    unit,
    oob_ndx,
    status
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_egr_rec_oob_stat_get()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Reception (OOB RX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_egr_rec_oob_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_EGR_REC_OOB_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_egr_rec_oob_verify(
    unit,
    oob_ndx,
    oob_conf,
    cal_buff
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fc_egr_rec_oob_set_unsafe(
    unit,
    oob_ndx,
    oob_conf,
    cal_buff
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_egr_rec_oob_set()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Reception (OOB RX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_egr_rec_oob_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_OUT SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_EGR_REC_OOB_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

 res = soc_petra_fc_egr_rec_oob_get_unsafe(
    unit,
    oob_ndx,
    oob_conf,
    cal_buff
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_egr_rec_oob_get()",0,0);
}


/*********************************************************************
*     Set the configuration for handling Flow Control
*     generated by (and effecting) recycling traffic, based on
*     Virtual Statistics Queues, handled on Outgoing FAP Port
*     level.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_vsq_ofp_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_RCY_PORT_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_VSQ_OFP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_rcy_vsq_ofp_verify(
    unit,
    cc_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fc_rcy_vsq_ofp_set_unsafe(
    unit,
    cc_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_vsq_ofp_set()",0,0);
}

/*********************************************************************
*     Set the configuration for handling Flow Control
*     generated by (and effecting) recycling traffic, based on
*     Virtual Statistics Queues, handled on Outgoing FAP Port
*     level.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_vsq_ofp_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_RCY_PORT_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_VSQ_OFP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_rcy_vsq_ofp_get_unsafe(
    unit,
    cc_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_vsq_ofp_get()",0,0);
}

/*********************************************************************
*     Enable/disable the specified Flow Control mode for the
*     recycling OFP-s. If enabled, Flow Control is received by
*     OFP HR schedulers, based on Global Resources Flow
*     Control indication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_glb_ofp_hr_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_ON_GLB_RCS_MODE  fc_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_GLB_OFP_HR_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fc_rcy_glb_ofp_hr_verify(
    unit,
    ofp_ndx,
    fc_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_fc_rcy_glb_ofp_hr_set_unsafe(
    unit,
    ofp_ndx,
    fc_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_glb_ofp_hr_set()",0,0);
}

/*********************************************************************
*     Enable/disable the specified Flow Control mode for the
*     recycling OFP-s. If enabled, Flow Control is received by
*     OFP HR schedulers, based on Global Resources Flow
*     Control indication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_glb_ofp_hr_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_ON_GLB_RCS_MODE  *fc_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_GLB_OFP_HR_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fc_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_rcy_glb_ofp_hr_get_unsafe(
    unit,
    ofp_ndx,
    fc_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_glb_ofp_hr_get()",0,0);
}

/*********************************************************************
*     Enable/disable reception of Flow Control for Recycling
*     ports by end to end Scheduler HR-s 192-223, represented
*     by Category 3 Connection Class in Ingress Statistics
*     Queues, when the Statistics Queues threshold is crossed.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_hr_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_HR_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);


  res = soc_petra_fc_rcy_hr_enable_verify(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fc_rcy_hr_enable_set_unsafe(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_hr_enable_set()",0,0);
}

/*********************************************************************
*     Enable/disable reception of Flow Control for Recycling
*     ports by end to end Scheduler HR-s 192-223, represented
*     by Category 3 Connection Class in Ingress Statistics
*     Queues, when the Statistics Queues threshold is crossed.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_hr_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_HR_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_fc_rcy_hr_enable_get_unsafe(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_hr_enable_get()",0,0);
}

void
  soc_petra_PETRA_FC_OOB_IF_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_FC_OOB_IF_INFO));
  info->enable = 0;
  info->cal_len = 0;
  info->cal_reps = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_FC_EGR_REC_OOB_STAT_clear(
    SOC_SAND_OUT SOC_PETRA_FC_EGR_REC_OOB_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_FC_EGR_REC_OOB_STAT));
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
  soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO));
  info->ll_enable_state = SOC_PETRA_FC_INGR_NOF_GEN_NIF_LL_STATES;
  info->cb_enable_state = SOC_PETRA_FC_INGR_NOF_GEN_NIF_CB_STATES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO));
  info->enable = 0;
  info->class_and_below = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_FC_GEN_OOB_CAL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_GEN_OOB_CAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_FC_GEN_OOB_CAL_INFO));
  info->source = SOC_PETRA_FC_NOF_GEN_OOB_SRCS;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_FC_GEN_OOB_CAL_INPT_clear(
    SOC_SAND_OUT SOC_PETRA_FC_GEN_OOB_CAL_INPT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_FC_GEN_OOB_CAL_INPT));
  soc_petra_PETRA_FC_GEN_OOB_CAL_INFO_clear(&(info->conf));
  info->weight = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_FC_REC_OOB_CAL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_REC_OOB_CAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_FC_REC_OOB_CAL_INFO));
  info->destination = SOC_PETRA_FC_NOF_REC_OOB_DESTS;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_FC_REC_OOB_CAL_INPT_clear(
    SOC_SAND_OUT SOC_PETRA_FC_REC_OOB_CAL_INPT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_FC_REC_OOB_CAL_INPT));
  soc_petra_PETRA_FC_REC_OOB_CAL_INFO_clear(&(info->conf));
  info->weight = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_FC_RCY_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_RCY_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_FC_RCY_PORT_INFO));
  info->handler = SOC_PETRA_FC_NOF_RCY_OFP_HANDLERS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_FC_PRIORITY_to_string(
    SOC_SAND_IN SOC_PETRA_FC_PRIORITY enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_PRIORITY_LOW:
    str = "Low Priority";
  break;

  case SOC_PETRA_FC_PRIORITY_HIGH:
    str = "High Priority";
  break;

  case SOC_PETRA_FC_NOF_PRIORITYS:
    str = "NOF_PRIORITIES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_FC_OOB_ID_to_string(
    SOC_SAND_IN SOC_PETRA_FC_OOB_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_OOB_ID_A:
    str = "OOB_ID_A";
  break;

  case SOC_PETRA_FC_OOB_ID_B:
    str = "OOB_ID_B";
  break;

  case SOC_PETRA_FC_NOF_OOB_IFS:
    str = "NOF_OOB_IFS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_DISABLE:
    str = "Disabled";
  break;

  case SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_ENABLE:
    str = "Enabled";
  break;

  case SOC_PETRA_FC_INGR_NOF_GEN_NIF_LL_STATES:
    str = "INGR_NOF_GEN_NIF_LL_STATES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_ENABLE_STF:
    str = "Enabled, Statistics Tag";
  break;

  case SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_ENABLE_CC:
    str = "Enabled, Connection Class";
  break;

  case SOC_PETRA_FC_INGR_NOF_GEN_NIF_CB_STATES:
    str = "INGR_NOF_GEN_NIF_CB_STATES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_FC_INGR_GEN_GLB_HP_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_NONE:
    str = "Disabled";
  break;

  case SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_LL:
    str = "Enabled, Link-Level";
  break;

  case SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_CB:
    str = "Enabled, Class Based";
  break;

  case SOC_PETRA_FC_NOF_INGR_GEN_GLB_HP_MODES:
    str = "NOF_INGR_GEN_GLB_HP_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_FC_GEN_OOB_SRC_to_string(
    SOC_SAND_IN SOC_PETRA_FC_GEN_OOB_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_GEN_OOB_SRC_STE:
    str = "Statistics Engine";
  break;

  case SOC_PETRA_FC_GEN_OOB_SRC_NIF:
    str = "Network Interface";
  break;

  case SOC_PETRA_FC_GEN_OOB_SRC_GLB_HP:
    str = "Global Resources, High Priority";
  break;

  case SOC_PETRA_FC_GEN_OOB_SRC_GLB_LP:
    str = "Global Resources, Low Priority";
  break;

  case SOC_PETRA_FC_GEN_OOB_SRC_NONE:
    str = "GEN_OOB_SRC_NONE";
  break;

  case SOC_PETRA_FC_NOF_GEN_OOB_SRCS:
    str = "NOF_GEN_OOB_SRCS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_FC_OOB_GLB_RCS_ID_to_string(
    SOC_SAND_IN SOC_PETRA_FC_OOB_GLB_RCS_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_OOB_GLB_RCS_ID_BDB:
    str = "Global Resources, Buffer Descriptor Buffers";
  break;

  case SOC_PETRA_FC_OOB_GLB_RCS_ID_UNI:
    str = "Global Resources, Unicast Buffers";
  break;

  case SOC_PETRA_FC_OOB_GLB_RCS_ID_MUL:
    str = "Global Resources, Multicast Buffers";
  break;

  case SOC_PETRA_FC_NOF_OOB_GLB_RCS_IDS:
    str = "NOF_OOB_GLB_RCS_IDS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_FC_REC_OOB_DEST_to_string(
    SOC_SAND_IN SOC_PETRA_FC_REC_OOB_DEST enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_REC_OOB_DEST_HR:
    str = "Scheduler HR";
  break;

  case SOC_PETRA_FC_REC_OOB_DEST_OFP_EGQ:
    str = "OFP, Egress Processor (EGQ)";
  break;

  case SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_LP:
    str = "OFP, Scheduler, Low Priority";
  break;

  case SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_HP:
    str = "OFP, Scheduler, High Priority";
  break;

  case SOC_PETRA_FC_REC_OOB_DEST_NIF:
    str = "Network Interface";
  break;

  case SOC_PETRA_FC_REC_OOB_DEST_NONE:
    str = "None";
  break;

  case SOC_PETRA_FC_NOF_REC_OOB_DESTS:
    str = "NOF_REC_OOB_DESTS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_FC_RCY_OFP_HANDLER_to_string(
    SOC_SAND_IN SOC_PETRA_FC_RCY_OFP_HANDLER enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_RCY_OFP_HANDLER_NONE:
    str = "None";
  break;

  case SOC_PETRA_FC_RCY_OFP_HANDLER_SCH_LP:
    str = "OFP, Scheduler, Low Priority";
  break;

  case SOC_PETRA_FC_RCY_OFP_HANDLER_SCH_HP:
    str = "OFP, Scheduler, High Priority";
  break;

  case SOC_PETRA_FC_RCY_OFP_HANDLER_EGQ:
    str = "OFP, Egress Processor (EGQ)";
  break;

  case SOC_PETRA_FC_NOF_RCY_OFP_HANDLERS:
    str = "NOF_RCY_OFP_HANDLERS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_FC_ON_GLB_RCS_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_FC_ON_GLB_RCS_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_FC_ON_GLB_RCS_MODE_DISABLE:
    str = "Disabled";
  break;

  case SOC_PETRA_FC_ON_GLB_RCS_MODE_ENABLE_LP:
    str = "Enabled, Low Priority";
  break;

  case SOC_PETRA_FC_ON_GLB_RCS_MODE_ENABLE_HP:
    str = "Enabled, High Priority";
  break;

  case SOC_PETRA_FC_NOF_ON_GLB_RCS_MODES:
    str = "NOF_ON_GLB_RCS_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


void
  soc_petra_PETRA_FC_OOB_IF_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_OOB_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Enable: %u\n\r",info->enable);
  soc_sand_os_printf("Cal_len: %u[Channels]\n\r",info->cal_len);
  soc_sand_os_printf("Cal_reps: %u[Flow controls]\n\r",info->cal_reps);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_FC_EGR_REC_OOB_STAT_print(
    SOC_SAND_IN SOC_PETRA_FC_EGR_REC_OOB_STAT *info
  )
{
  uint8
    is_err;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Not_locked:       %s\n\r",(info->not_locked)?"TRUE  *":"FALSE");
  soc_sand_os_printf("Out_of_frame:     %s\n\r",(info->out_of_frame)?"TRUE  *":"FALSE");
  soc_sand_os_printf("Dip2_alarm:       %s\n\r",(info->dip2_alarm)?"TRUE  *":"FALSE");
  soc_sand_os_printf("Nof_frame_errors: %4u%s\n\r",info->nof_frame_errors, (info->nof_frame_errors)?"  *":"");
  soc_sand_os_printf("Nof_dip2_errors:  %4u%s\n\r",info->nof_dip2_errors, (info->nof_dip2_errors)?"  *":"");

  is_err =
    SOC_SAND_NUM2BOOL(info->not_locked || info->out_of_frame || info->dip2_alarm || \
    (info->nof_frame_errors > 0) || (info->nof_dip2_errors > 0));

  soc_sand_os_printf("-------------------\n\r");
  soc_sand_os_printf("OOB Status: %s\n\r", is_err?"  ERROR":"     OK");

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "Ll_enable_state %s \n\r",
    soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_to_string(info->ll_enable_state)
  );
  soc_sand_os_printf(
    "Cb_enable_state %s \n\r",
    soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_to_string(info->cb_enable_state)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Enable: %u\n\r",info->enable);
  soc_sand_os_printf("Class_and_below: %u\n\r",info->class_and_below);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_FC_GEN_OOB_CAL_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_GEN_OOB_CAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "Source %s, ",
    soc_petra_PETRA_FC_GEN_OOB_SRC_to_string(info->source)
  );
  soc_sand_os_printf("Id: %u.\n\r",info->id);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_FC_GEN_OOB_CAL_INPT_print(
    SOC_SAND_IN SOC_PETRA_FC_GEN_OOB_CAL_INPT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Conf:");
  soc_petra_PETRA_FC_GEN_OOB_CAL_INFO_print(&(info->conf));
  soc_sand_os_printf("Weight: %u\n\r",info->weight);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_FC_REC_OOB_CAL_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_REC_OOB_CAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "Destination %s, ",
    soc_petra_PETRA_FC_REC_OOB_DEST_to_string(info->destination)
  );
  soc_sand_os_printf("Id: %u.\n\r",info->id);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_FC_REC_OOB_CAL_INPT_print(
    SOC_SAND_IN SOC_PETRA_FC_REC_OOB_CAL_INPT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Conf: ");
  soc_petra_PETRA_FC_REC_OOB_CAL_INFO_print(&(info->conf));
  soc_sand_os_printf("Weight: %u\n\r",info->weight);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_FC_RCY_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_RCY_PORT_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "Handler %s \n\r",
    soc_petra_PETRA_FC_RCY_OFP_HANDLER_to_string(info->handler)
  );
  soc_sand_os_printf("Ofp_id: %u\n\r",info->ofp_id);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

#include <soc/dpp/SAND/Utils/sand_footer.h>
