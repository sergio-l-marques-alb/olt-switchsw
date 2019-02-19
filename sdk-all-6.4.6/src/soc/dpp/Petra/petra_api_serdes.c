/* $Id: petra_api_serdes.c,v 1.9 Broadcom SDK $
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



/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_serdes.h>
#include <soc/dpp/Petra/petra_serdes.h>

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
*     Set SerDes lane rate.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_rate_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  uint32                  rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_RATE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_srd_rate_verify(
    unit,
    lane_ndx,
    rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_rate_set_unsafe(
    unit,
    lane_ndx,
    rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_rate_set()",0,0);
}

/*********************************************************************
*     Set SerDes lane rate.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_rate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT uint32                  *rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_RATE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rate);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_rate_get_unsafe(
    unit,
    lane_ndx,
    rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_rate_get()",0,0);
}

/*********************************************************************
*     Set SerDes Physical Parameters configuration, on the
*     transmitter side.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_tx_phys_params_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_TX_PHYS_PARAMS_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_srd_tx_phys_params_verify(
          unit,
          lane_ndx,
          conf_mode_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_tx_phys_params_set_unsafe(
          unit,
          lane_ndx,
          conf_mode_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_tx_phys_params_set()",0,0);
}

/*********************************************************************
*     Set SerDes Physical Parameters configuration, on the
*     transmitter side.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_tx_phys_params_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_TX_PHYS_PARAMS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_tx_phys_params_get_unsafe(
          unit,
          lane_ndx,
          conf_mode_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_tx_phys_params_get()",0,0);
}

/*********************************************************************
*     Set, per direction (RX/TX/Both) whether to swap-polarity
*     on a certain lane.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_lane_polarity_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  uint8                 is_swap_polarity
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_LANE_POLARITY_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_srd_lane_polarity_verify(
          unit,
          lane_ndx,
          direction_ndx,
          is_swap_polarity
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_lane_polarity_set_unsafe(
          unit,
          lane_ndx,
          direction_ndx,
          is_swap_polarity,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_lane_polarity_set()",0,0);
}

/*********************************************************************
*     Set, per direction (RX/TX/Both) whether to swap-polarity
*     on a certain lane.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_lane_polarity_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT uint8                 *is_swap_polarity_rx,
    SOC_SAND_OUT uint8                 *is_swap_polarity_tx
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_LANE_POLARITY_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_swap_polarity_rx);
  SOC_SAND_CHECK_NULL_INPUT(is_swap_polarity_tx);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_lane_polarity_get_unsafe(
          unit,
          lane_ndx,
          is_swap_polarity_rx,
          is_swap_polarity_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_lane_polarity_get()",0,0);
}

/*********************************************************************
*     Set the state of the lane - power-up/power-down.
*     CAUTION! This is a low-level API that operates directly on the SerDes,
*     regardless the attached logic (Fabric/NIF). For the correct power-up/power-down sequence,
*     including powering up/down the SerDes, use the relevant API-s:
*     fabric_link_on_off_set, nif_on_off_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_lane_power_state_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_POWER_STATE     state
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_LANE_POWER_STATE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_srd_lane_power_state_verify(
          unit,
          lane_ndx,
          direction_ndx,
          state
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_lane_power_state_set_unsafe(
          unit,
          lane_ndx,
          direction_ndx,
          state
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_lane_power_state_set()",0,0);
}

/*********************************************************************
*     Set the state of the lane - power-up/power-down.
*     CAUTION! This is a low-level API that operates directly on the SerDes,
*     regardless the attached logic (Fabric/NIF). For the correct power-up/power-down sequence,
*     including powering up/down the SerDes, use the relevant API-s:
*     fabric_link_on_off_set, nif_on_off_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_lane_power_state_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_POWER_STATE     *state_rx,
    SOC_SAND_OUT SOC_PETRA_SRD_POWER_STATE     *state_tx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_LANE_POWER_STATE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(state_rx);
  SOC_SAND_CHECK_NULL_INPUT(state_tx);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_lane_power_state_get_unsafe(
          unit,
          lane_ndx,
          state_rx,
          state_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_lane_power_state_get()",0,0);
}

/*********************************************************************
*     Performs a SerDes star initialization sequence. This
*     sequence includes in-out of reset sequence for the star
*     IPU, and the CMU-s of the specified SerDes quartets. It
*     also includes CMU trimming and power-up validation.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_star_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_INFO       *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_STAR_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_srd_star_verify(
    unit,
    star_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_star_set_unsafe(
    unit,
    star_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_star_set()",0,0);
}

/*********************************************************************
*     Performs a SerDes star initialization sequence. This
*     sequence includes in-out of reset sequence for the star
*     IPU, and the CMU-s of the specified SerDes quartets. It
*     also includes CMU trimming and power-up validation.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_star_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_STAR_INFO       *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_STAR_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_star_get_unsafe(
    unit,
    star_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_star_get()",0,0);
}

/*********************************************************************
*     Performs a SerDes Quartet reset/initialization sequence.
*     The maximal supported rate of the SerDes quartet is set
*     as part of this initialization. Also, per-lane rate is
*     initialized according to the indicated rate.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_srd_qrtt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 qrtt_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_QRTT_INFO       *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_QRTT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_srd_qrtt_verify(
    unit,
    qrtt_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_qrtt_set_unsafe(
    unit,
    qrtt_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_qrtt_set()",0,0);
}

/*********************************************************************
*     Performs a SerDes Quartet reset/initialization sequence.
*     The maximal supported rate of the SerDes quartet is set
*     as part of this initialization. Also, per-lane rate is
*     initialized according to the indicated rate.
*     Details: in the H file. (search for prototype)
*********************************************************************/


uint32
  soc_petra_srd_qrtt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 qrtt_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_QRTT_INFO       *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_QRTT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_qrtt_get_unsafe(
    unit,
    qrtt_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_qrtt_get()",0,0);
}

/*********************************************************************
*     Set SerDes parameters, for the selected SerDes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_all_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ALL_INFO        *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_ALL_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_all_verify(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_srd_all_set_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_all_set()",0,0);
}

/*********************************************************************
*     Set SerDes parameters, for the selected SerDes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_all_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_SRD_ALL_INFO        *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_ALL_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_all_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_all_get()",0,0);
}

/*********************************************************************
*     Perform auto-equalization process. This process targets
*     to achive optimal receiver configuration, resulting in
*     maximal eye-opening.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_auto_equalize(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_AEQ_MODE        mode,
    SOC_SAND_OUT SOC_PETRA_SRD_AEQ_STATUS      *status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_AUTO_EQUALIZE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_auto_equalize_unsafe(
    unit,
    lane_ndx,
    mode,
    status
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_auto_equalize()",0,0);
}

/*********************************************************************
*     Receiver physical parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_rx_phys_params_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_RX_PHYS_PARAMS_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_srd_rx_phys_params_verify(
    unit,
    lane_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_rx_phys_params_set_unsafe(
    unit,
    lane_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_rx_phys_params_set()",0,0);
}

/*********************************************************************
*     Receiver physical parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_rx_phys_params_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_RX_PHYS_PARAMS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_rx_phys_params_get_unsafe(
    unit,
    lane_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_rx_phys_params_get()",0,0);
}

void
  soc_petra_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS));
  info->pre = 0;
  info->post = 0;
  info->swing = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS));
  info->amp = 0;
  info->main = 0;
  info->pre = 0;
  info->post = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS));
  info->zcnt = 0;
  info->z1cnt = 0;
  info->dfelth = 0;
  info->tlth = 0;
  info->g1cnt = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_TX_PHYS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_TX_PHYS));
  soc_petra_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS_clear(&(info->intern));
  soc_petra_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS_clear(&(info->explct));
  info->atten = 0;
  info->media_type = SOC_PETRA_SRD_NOF_MEDIA_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_TX_PHYS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_TX_PHYS_INFO));
  soc_petra_PETRA_SRD_TX_PHYS_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_RX_PHYS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_RX_PHYS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_RX_PHYS_INFO));
  soc_petra_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS_clear(&(info->intern));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_QRTT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_QRTT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_QRTT_INFO));
  info->is_active = 0;
  info->max_expected_lane_rate = SOC_PETRA_SRD_NOF_DATA_RATES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_STAR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_STAR_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_STAR_INFO));
  for (ind=0; ind<SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX; ++ind)
  {
    soc_petra_PETRA_SRD_QRTT_INFO_clear(&(info->qrtt[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_ALL_LANE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_ALL_LANE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_ALL_LANE_INFO));
  info->enable = 0;
  info->rate_conf = SOC_PETRA_SRD_NOF_DATA_RATES;
  soc_petra_PETRA_SRD_TX_PHYS_INFO_clear(&(info->tx_phys_conf));
  info->tx_phys_conf_mode = SOC_PETRA_SRD_NOF_TX_PHYS_CONF_MODES;
  soc_petra_PETRA_SRD_RX_PHYS_INFO_clear(&(info->rx_phys_conf));
  info->power_state_conf = SOC_PETRA_SRD_NOF_POWER_STATES;
  info->is_swap_polarity_tx = 0;
  info->is_swap_polarity_rx = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_ALL_STAR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_ALL_STAR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_ALL_STAR_INFO));
  info->enable = 0;
  soc_petra_PETRA_SRD_STAR_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_ALL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_ALL_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_ALL_INFO));
  for (ind=0; ind<SOC_PETRA_SRD_NOF_STARS; ++ind)
  {
    soc_petra_PETRA_SRD_ALL_STAR_INFO_clear(&(info->star_conf[ind]));
  }
  for (ind=0; ind<SOC_PETRA_SRD_NOF_LANES; ++ind)
  {
    soc_petra_PETRA_SRD_ALL_LANE_INFO_clear(&(info->lane_conf[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_AEQ_STATUS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_AEQ_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_AEQ_STATUS));
  info->is_success = 0;
  info->eye_height_taps = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*
 *  Return the SerDes quartet index, in the range [0..14],
 *  given the Combo enumerator index.
 *  If invalid Combo index, returns SOC_PETRA_SRD_NOF_QUARTETS.
 */
uint32
  soc_petra_srd_combo2qrtt_id(
    SOC_SAND_IN SOC_PETRA_COMBO_QRTT combo_qrtt
  )
{
  uint32
    qrtt_idx;

  switch(combo_qrtt) {
  case SOC_PETRA_COMBO_QRTT_0:
    qrtt_idx = 3;
    break;
  case SOC_PETRA_COMBO_QRTT_1:
    qrtt_idx = 7;
    break;
  default:
    qrtt_idx = SOC_PETRA_SRD_NOF_QUARTETS;
  }

  return qrtt_idx;
}

/*
 *  Return the Combo quartet index, in the range [0..1],
 *  given the SerDes enumerator index.
 *  If invalid SerDes index, returns SOC_PETRA_COMBO_NOF_QRTTS.
 */
uint32
  soc_petra_srd_qrtt2combo_id(
    SOC_SAND_IN uint32 srd_qrtt
  )
{
  uint32
    qrtt_idx;

  switch(srd_qrtt) {
  case 3:
    qrtt_idx = SOC_PETRA_COMBO_QRTT_0;
    break;
  case 7:
    qrtt_idx = SOC_PETRA_COMBO_QRTT_1;
    break;
  default:
    qrtt_idx = SOC_PETRA_COMBO_NOF_QRTTS;
  }

  return qrtt_idx;
}

#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_SRD_STAR_ID_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_STAR_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_STAR_ID_0:
    str = "STAR_0";
  break;

  case SOC_PETRA_SRD_STAR_ID_1:
    str = "STAR_1";
  break;

  case SOC_PETRA_SRD_STAR_ID_2:
    str = "STAR_2";
  break;

  case SOC_PETRA_SRD_STAR_ID_3:
    str = "STAR_3";
  break;

  case SOC_PETRA_SRD_NOF_STAR_IDS:
    str = "NOF_STAR_IDS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_SRD_DATA_RATE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_DATA_RATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_DATA_RATE_1000_00:
    str = "1000.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_1041_67:
    str = "1041.67 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_1171_88:
    str = "1171.88 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_1250_00:
    str = "1250.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_1302_03:
    str = "1302.03 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_1333_33:
    str = "1333.33 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_1562_50:
    str = "1562.50 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_2343_75:
    str = "2343.75 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_2500_00:
    str = "2500.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_2604_16:
    str = "2604.16 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_2666_67:
    str = "2666.67 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_2083_33:
    str = "2083.33 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_3000_00:
    str = "3000.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_3125_00:
    str = "3125.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_3125_00_FDR:
    str = "3125.00 FDR Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_3750_00:
    str = "3750.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_4000_00:
    str = "4000.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_4166_67:
    str = "4166.67 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_4687_50:
    str = "4687.50 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_5000_00:
    str = "5000.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_5208_33:
    str = "5208.33 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_5333_33:
    str = "5333.33 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_5833_33:
    str = "5833.33 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_6000_00:
    str = "6000.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_6250_00:
    str = "6250.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_4375_00:
    str = "4375.00 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_5468_75:
    str = "5468.75 Mbps";
  break;

  case SOC_PETRA_SRD_DATA_RATE_4250_00:
    str = "4250.00 Mbps";
  break;

  case SOC_PETRA_SRD_NOF_DATA_RATES:
    str = "NOF_DATA_RATES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_MEDIA_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_MEDIA_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_MEDIA_TYPE_CHIP2CHIP:
    str = "CHIP2CHIP";
  break;

  case SOC_PETRA_SRD_MEDIA_TYPE_SHORT_BACKPLANE:
    str = "SHORT_BACKPLANE";
  break;

  case SOC_PETRA_SRD_MEDIA_TYPE_LONG_BACKPLANE:
    str = "LONG_BACKPLANE";
  break;

  case SOC_PETRA_SRD_NOF_MEDIA_TYPES:
    str = "NOF_MEDIA_TYPES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_TX_PHYS_CONF_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_TX_PHYS_CONF_MODE_INTERNAL:
    str = "INTERNAL";
  break;

  case SOC_PETRA_SRD_TX_PHYS_CONF_MODE_EXPLICIT:
    str = "EXPLICIT";
  break;

  case SOC_PETRA_SRD_TX_PHYS_CONF_MODE_ATTEN:
    str = "ATTEN";
  break;

  case SOC_PETRA_SRD_TX_PHYS_CONF_MODE_MEDIA_TYPE:
    str = "MEDIA_TYPE";
  break;

  case SOC_PETRA_SRD_NOF_TX_PHYS_CONF_MODES:
    str = "NOF_TX_PHYS_CONF_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_POWER_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_POWER_STATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_POWER_STATE_DOWN:
    str = "DOWN";
  break;

  case SOC_PETRA_SRD_POWER_STATE_UP:
    str = "UP";
  break;

  case SOC_PETRA_SRD_POWER_STATE_UP_AND_RELOCK:
    str = "UP_AND_RELOCK";
  break;

  case SOC_PETRA_SRD_NOF_POWER_STATES:
    str = "NOF_POWER_STATES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_AEQ_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_AEQ_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_AEQ_MODE_BLIND:
    str = "BLIND";
  break;

  case SOC_PETRA_SRD_AEQ_MODE_STEADY_STATE:
    str = "STEADY_STATE";
  break;

  case SOC_PETRA_SRD_NOF_AEQ_MODES:
    str = "NOF_AEQ_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


void
  soc_petra_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->swing < SOC_PETRA_SRD_TX_SWING_MV_MIN)
  {
    soc_sand_os_printf(
      " The EXPLICIT configuration could not be retreived\n\r"
      " from the SerDes internal configuration.\n\r"
      " Try using INTERNAL mode instead.\n\r"
    );
  }
  else
  {
    soc_sand_os_printf("   Pre-emphasis: %u[%%], ",info->pre);
    soc_sand_os_printf("Post-emphasis: %u[%%], ",info->post);
    soc_sand_os_printf("Swing: %u[mV]\n\r",info->swing);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_TX_PHYS_ATTEN_PARAMS_print(
    SOC_SAND_IN uint32 atten
  )
{
  if (atten == SOC_PETRA_SRD_TX_ATTEN_INVALID)
  {
    soc_sand_os_printf(
      "   The BY-ATTENUATION configuration could not be retreived\n\r"
      "   from the SerDes internal configuration.\n\r"
      "   Try using INTERNAL mode instead.\n\r"
    );
  }
  else
  {
    soc_sand_os_printf("   Attenuation: %u.%u[dB] \n\r", atten/10, atten%10);
  }
}



void
  soc_petra_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Amp: %u, ", info->amp);
  soc_sand_os_printf("Main: %u, ", info->main);
  soc_sand_os_printf("Pre: %u, ", info->pre);
  soc_sand_os_printf("Post: %u.\n\r", info->post);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS_print(
    SOC_SAND_IN SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Zcnt:   %u, ", info->zcnt);
  soc_sand_os_printf("Z1cnt:  %u, ", info->z1cnt);
  soc_sand_os_printf("Dfelth: %u, ", info->dfelth);
  soc_sand_os_printf("Tlth:   %u, ", info->tlth);
  soc_sand_os_printf("G1cnt:  %u.\n\r", info->g1cnt);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_TX_PHYS_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode,
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(conf_mode)
  {
  case SOC_PETRA_SRD_TX_PHYS_CONF_MODE_INTERNAL:
    soc_sand_os_printf("   Internal: ");
    soc_petra_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS_print(&(info->intern));
  break;

  case SOC_PETRA_SRD_TX_PHYS_CONF_MODE_EXPLICIT:
    soc_sand_os_printf("   Explicit: ");
    soc_petra_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS_print(&(info->explct));
  break;

  case SOC_PETRA_SRD_TX_PHYS_CONF_MODE_ATTEN:
    soc_sand_os_printf("   Attenuation: ");
    soc_petra_PETRA_SRD_TX_PHYS_ATTEN_PARAMS_print(info->atten);
  break;

  case SOC_PETRA_SRD_TX_PHYS_CONF_MODE_MEDIA_TYPE:
    soc_sand_os_printf(
      "   Media_type %s \n\r",
      soc_petra_PETRA_SRD_MEDIA_TYPE_to_string(info->media_type)
    );
  break;

  default:
    soc_sand_os_printf("   Unknown");
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_TX_PHYS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode,
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_INFO      *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("  Conf:\n\r");
  soc_petra_PETRA_SRD_TX_PHYS_print(conf_mode, &(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_RX_PHYS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_RX_PHYS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("  Intern: ");
  soc_petra_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS_print(&(info->intern));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_QRTT_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_QRTT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Is_active: %u\n\r",info->is_active);

  /* The macro SOC_PETRA_SRD_IS_RATE_ENUM may be used for signed varible also */
  /* coverity[unsigned_compare : FALSE] */
  if (SOC_PETRA_SRD_IS_RATE_ENUM(info->max_expected_lane_rate))
  {
    soc_sand_os_printf(
      "     Max_expected_lane_rate: %s\n\r",
      soc_petra_PETRA_SRD_DATA_RATE_to_string(info->max_expected_lane_rate)
    );
  }
  else
  {
    soc_sand_os_printf(
      "     Max_expected_lane_rate: %u.%02u Mbps\n\r",
      info->max_expected_lane_rate/1000, info->max_expected_lane_rate%1000
    );
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_STAR_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_STAR_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX; ++ind)
  {
    soc_sand_os_printf("    Qrtt[%2u]: ",ind);
    soc_petra_PETRA_SRD_QRTT_INFO_print(&(info->qrtt[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_ALL_LANE_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode,
    SOC_SAND_IN SOC_PETRA_SRD_ALL_LANE_INFO     *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* The macro SOC_PETRA_SRD_IS_RATE_ENUM may be used for signed varible also */
  /* coverity[unsigned_compare : FALSE] */
  if (SOC_PETRA_SRD_IS_RATE_ENUM(info->rate_conf))
  {
    soc_sand_os_printf(
      "   Rate_conf:           %s\n\r",
      soc_petra_PETRA_SRD_DATA_RATE_to_string(info->rate_conf)
    );
  }
  else
  {
    soc_sand_os_printf(
      "   Rate_conf:         %u.%02u Mbps\n\r",
      info->rate_conf/1000, info->rate_conf%1000
    );
  }
  soc_sand_os_printf("   Tx_phys_conf:\n\r");
  soc_petra_PETRA_SRD_TX_PHYS_INFO_print(conf_mode,&(info->tx_phys_conf));
  soc_sand_os_printf("   Rx_phys_conf:\n\r");
  soc_petra_PETRA_SRD_RX_PHYS_INFO_print(&(info->rx_phys_conf));
  soc_sand_os_printf(
    "   Power_state_conf:    %s \n\r",
    soc_petra_PETRA_SRD_POWER_STATE_to_string(info->power_state_conf)
  );
  soc_sand_os_printf("   Is_swap_polarity_tx: %u\n\r",info->is_swap_polarity_tx);
  soc_sand_os_printf("   Is_swap_polarity_rx: %u\n\r",info->is_swap_polarity_rx);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_ALL_STAR_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_ALL_STAR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("   Enable: %u\n\r",info->enable);
  soc_sand_os_printf("   Conf:\n\r");
  soc_petra_PETRA_SRD_STAR_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_ALL_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode,
    SOC_SAND_IN SOC_PETRA_SRD_ALL_INFO          *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_PETRA_SRD_NOF_STARS; ++ind)
  {
    soc_sand_os_printf("  Star_conf[%u]:\n\r",ind);
    soc_petra_PETRA_SRD_ALL_STAR_INFO_print(&(info->star_conf[ind]));
  }

  for (ind=0; ind<SOC_PETRA_SRD_NOF_LANES; ++ind)
  {
    soc_sand_os_printf("\n\r");
    soc_sand_os_printf("  Lane_conf[%2u]:\n\r",ind);
    soc_petra_PETRA_SRD_ALL_LANE_INFO_print(conf_mode,&(info->lane_conf[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_AEQ_STATUS_print(
    SOC_SAND_IN SOC_PETRA_SRD_AEQ_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Is_success:      %u\n\r",info->is_success);
  soc_sand_os_printf("Eye_height_taps: %u\n\r",info->eye_height_taps);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

