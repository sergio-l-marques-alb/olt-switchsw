/* $Id: petra_api_ofp_rates.c,v 1.9 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_ofp_rates.c
*
* MODULE PREFIX:  soc_petra_ofp_rates
*
* FILE DESCRIPTION:
*   Rates and burst configuration of the Outgoing FAP Ports.
*   The configuration envolves End-to-end scheduler and Egress processor.
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

#include <soc/dpp/Petra/petra_api_ofp_rates.h>
#include <soc/dpp/Petra/petra_api_ports.h>
#include <soc/dpp/Petra/petra_ofp_rates.h>
#include <soc/dpp/error.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_TM/pb_ofp_rates.h>
#endif

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
*     Configures Outgoing FAP Ports (OFP) rates, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     calculates from the given table the calendars
*     granularity, writes it to the device and changes the
*     active calendars. It also saves the values in the
*     software database for single-entry changes in the
*     future. For ports mapped to Network Interfaces, The
*     configuration is per MAC Lane. This indicates a single
*     interface for SPAUI/XAUI, or 4 SGMII interfaces.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  SOC_SAND_CHECK_NULL_INPUT(ofp_rate_tbl);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_rates_verify(
          unit,
          mal_ndx,
          shaper,
          ofp_rate_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ofp_rates_set_unsafe(
          unit,
          mal_ndx,
          shaper,
          ofp_rate_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_set()",0,0);
}

/*********************************************************************
*     Configures Outgoing FAP Ports (OFP) rates, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     calculates from the given table the calendars
*     granularity, writes it to the device and changes the
*     active calendars. It also saves the values in the
*     software database for single-entry changes in the
*     future. For ports mapped to Network Interfaces, The
*     configuration is per MAC Lane. This indicates a single
*     interface for SPAUI/XAUI, or 4 SGMII interfaces.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  SOC_SAND_CHECK_NULL_INPUT(ofp_rate_tbl);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_rates_get_unsafe(
          unit,
          mal_ndx,
          shaper,
          ofp_rate_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

uint32
  soc_petra_ofp_all_ofp_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 get_exact,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_ALL_OFP_RATES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ofp_rate_tbl);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_all_ofp_rates_get_unsafe(
          unit,
          get_exact,
          ofp_rate_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_all_ofp_rates_get()",0,0);
}

/*********************************************************************
*     Configures a single Outgoing FAP Port (OFP) rate, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     re-calculates the appropriate values from the current
*     values and the updated info. It also saves the values in
*     the software database for single-entry changes in the
*     future. For Network Interfaces, The configuration is per
*     MAC Lane. This indicates a single interface for
*     SPAUI/XAUI, or 4 SGMII interfaces.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_single_port_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATE_INFO       *ofp_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_UPDATE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  SOC_SAND_CHECK_NULL_INPUT(ofp_rate);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_rates_single_port_verify(
          unit,
          ofp_ndx,
          shaper,
          ofp_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ofp_rates_single_port_set_unsafe(
          unit,
          ofp_ndx,
          shaper,
          ofp_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_single_port_set()",0,0);
}

/*********************************************************************
*     Get a single Outgoing FAP Port (OFP) rate/burst
*     configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_single_port_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATE_INFO       *ofp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SINGLE_PORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ofp);
  SOC_SAND_CHECK_NULL_INPUT(shaper);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_rates_single_port_get_unsafe(
          unit,
          ofp_ndx,
          shaper,
          ofp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_single_port_get()",0,0);
}

/*********************************************************************
*     Configure MAL-level shaping. This is required when the
*     shaping rate is different from the accumulated rate of
*     the OFP-s mapped to the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_mal_shaper_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *exact_shaper
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_MAL_SHAPER_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  SOC_SAND_CHECK_NULL_INPUT(exact_shaper);

  res = soc_petra_ofp_rates_mal_shaper_verify(
          unit,
          mal_ndx,
          shaper
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_rates_mal_shaper_set_unsafe(
          unit,
          mal_ndx,
          shaper,
          exact_shaper
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_mal_shaper_set()",0,0);
}

/*********************************************************************
*     Configure MAL-level shaping. This is required when the
*     shaping rate is different from the accumulated rate of
*     the OFP-s mapped to the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_mal_shaper_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_MAL_SHAPER_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(shaper);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_rates_mal_shaper_get_unsafe(
          unit,
          mal_ndx,
          shaper
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_mal_shaper_get()",0,0);
}

/*********************************************************************
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_fat_pipe_rate_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_FAT_PIPE_RATE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fatp_rate);

  res = soc_petra_ofp_fat_pipe_rate_verify(
          unit,
          fatp_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_fat_pipe_rate_set_unsafe(
          unit,
          fatp_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_fat_pipe_rate_set()",0,0);
}

/*********************************************************************
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_fat_pipe_rate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_FAT_PIPE_RATE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fatp_rate);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_fat_pipe_rate_get_unsafe(
          unit,
          fatp_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_fat_pipe_rate_get()",0,0);
}

#ifdef LINK_PB_LIBRARIES
/*********************************************************************
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
*     Note: this API is applicable only under the assumption that
*     the NIF-s that are part of the Fat-pipe are non-channelized.
*     When using channelized NIFs under Fat-pipe, the ofp_rate_set API may be used
*     in the following manner:
*      - For EGQ rate, set the rate for each Port that is part of the Fat-pipe
*        independently (as the proportional portion of the overall Fat-pipe rate.
*      - For SCH rate, set the rate as the overall Fat-pipe rate, only for the
*        Fat-pipe destination port (i.e. OFP 1 for Fat-pipe A, OFP 2 for Fat-pipe B,
*        OFP 3 for Fat-pipe C)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ofp_fat_pipe_rate_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID               fatp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_OFP_FAT_PIPE_RATE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fatp_rate);

  res = soc_pb_ofp_fat_pipe_rate_verify(
          unit,
          fatp_ndx,
          fatp_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_ofp_fat_pipe_rate_set_unsafe(
          unit,
          fatp_ndx,
          fatp_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ofp_fat_pipe_rate_set()",0,0);
}

/*********************************************************************
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ofp_fat_pipe_rate_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID               fatp_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_OFP_FAT_PIPE_RATE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fatp_rate);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_ofp_fat_pipe_rate_get_unsafe(
          unit,
          fatp_ndx,
          fatp_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ofp_fat_pipe_rate_get()",0,0);
}
#endif

uint32
  soc_petra_ofp_rates_egq_calendar_validate(
    SOC_SAND_IN  int                    unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_EGQ_CALENDAR_VALIDATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_rates_egq_calendar_validate_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_egq_calendar_validate()",0,0);
}

/*********************************************************************
*     Update the device after the computation of the
*     calendars.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_update_device_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_device_updated
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_UPDATE_DEVICE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_rates_update_device_set_unsafe(
          unit,
          is_device_updated
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_update_device_set()",0,0);
}

/*********************************************************************
*     Update the device after the computation of the
*     calendars.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_update_device_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_device_updated
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_UPDATE_DEVICE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_device_updated);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ofp_rates_update_device_get_unsafe(
          unit,
          is_device_updated
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_update_device_get()",0,0);
}

void
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATES_MAL_SHPR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_OFP_RATE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATES_TBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_OFP_FAT_PIPE_RATE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_FAT_PIPE_RATE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_OFP_RATES_CAL_SET_to_string(
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_CAL_SET enum_val
  )
{
  return SOC_TMC_OFP_RATES_CAL_SET_to_string(enum_val);
}

const char*
  soc_petra_PETRA_OFP_SHPR_UPDATE_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_OFP_SHPR_UPDATE_MODE enum_val
  )
{
  return SOC_TMC_OFP_SHPR_UPDATE_MODE_to_string(enum_val);
}

void
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_print(
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATES_MAL_SHPR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_print(
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATES_MAL_SHPR_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_OFP_RATE_INFO_print(
    SOC_SAND_IN  SOC_PETRA_OFP_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_OFP_RATES_TBL_INFO_print(
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATES_TBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_OFP_FAT_PIPE_RATE_INFO_print(
    SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_FAT_PIPE_RATE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*NEW GLUE LAYER {*/
int
  soc_petra_ofp_rates_sch_single_port_rate_sw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 rate
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper;
    SOC_TMC_OFP_RATE_INFO ofp_rate;   

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATE_INFO_clear(&ofp_rate);

    /* Get existing configuration */
    res = soc_petra_ofp_rates_single_port_get(unit, tm_port, &mal_shaper, &ofp_rate);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    /* Set OFP rate Info */
    /* ofp_rate.port_id = ofp_ndx; */
    ofp_rate.sch_rate = rate;
    mal_shaper.sch_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    mal_shaper.egq_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    
    /* Set configuration */
    res = soc_petra_ofp_rates_single_port_set(unit, tm_port, &mal_shaper, &ofp_rate);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

int
  soc_petra_ofp_rates_sch_single_port_rate_hw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  int                    core
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}
uint32
  soc_petra_ofp_rates_egq_single_port_rate_sw_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  uint32                 rate
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper;
    SOC_TMC_OFP_RATE_INFO ofp_rate;   

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATE_INFO_clear(&ofp_rate);

    /* Get existing configuration */
    res = soc_petra_ofp_rates_single_port_get(unit, ofp_ndx, &mal_shaper, &ofp_rate);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Set OFP rate Info */
    ofp_rate.port_id = ofp_ndx; 
    ofp_rate.egq_rate = rate;
    mal_shaper.sch_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    mal_shaper.egq_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    
    /* Set configuration */
    res = soc_petra_ofp_rates_single_port_set(unit, ofp_ndx, &mal_shaper, &ofp_rate);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

uint32
  soc_petra_ofp_rates_egq_single_port_rate_hw_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SET);
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

uint32
  soc_petra_ofp_rates_single_port_max_burst_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  uint32                 max_burst
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper;
    SOC_TMC_OFP_RATE_INFO ofp_rate;   

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATE_INFO_clear(&ofp_rate);

    /* Get existing configuration */
    res = soc_petra_ofp_rates_single_port_get(unit, ofp_ndx, &mal_shaper, &ofp_rate);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Set OFP rate Info */
    ofp_rate.port_id = ofp_ndx; 
    ofp_rate.max_burst = max_burst;
    mal_shaper.sch_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    mal_shaper.egq_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    
    /* Set configuration */
    res = soc_petra_ofp_rates_single_port_set(unit, ofp_ndx, &mal_shaper, &ofp_rate);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

int
  soc_petra_ofp_rates_sch_single_port_rate_hw_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  int                    core,
    SOC_SAND_OUT uint32                 *rate
  )
{
    uint32 res = SOC_SAND_OK, rv;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper;
    SOC_TMC_OFP_RATE_INFO ofp_rate;   

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATE_INFO_clear(&ofp_rate);

    /* Get existing configuration */
    res = soc_petra_ofp_rates_single_port_get(unit, tm_port, &mal_shaper, &ofp_rate);
    rv = handle_sand_result(res);
    SOC_IF_ERROR_RETURN(rv);

    
    /* Set OFP rate Info */
    *rate = ofp_rate.sch_rate;

    return SOC_E_NONE;
}

uint32
  soc_petra_ofp_rates_egq_single_port_rate_hw_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT uint32                 *rate
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper;
    SOC_TMC_OFP_RATE_INFO ofp_rate;   

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATE_INFO_clear(&ofp_rate);

    /* Get existing configuration */
    res = soc_petra_ofp_rates_single_port_get(unit, ofp_ndx, &mal_shaper, &ofp_rate);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Set OFP rate Info */
    *rate = ofp_rate.egq_rate;

 exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

uint32
  soc_petra_ofp_rates_single_port_max_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT uint32                 *max_burst
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper;
    SOC_TMC_OFP_RATE_INFO ofp_rate;   

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATE_INFO_clear(&ofp_rate);

    /* Get existing configuration */
    res = soc_petra_ofp_rates_single_port_get(unit, ofp_ndx, &mal_shaper, &ofp_rate);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Set OFP rate Info */
    *max_burst = ofp_rate.max_burst;

 exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

uint32
  soc_petra_ofp_rates_sch_interface_shaper_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                 if_shaper_rate
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 mal_ndx = 0;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper, mal_exact_shaper;    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_exact_shaper);
    
    /* Translate port to mal */
    res = soc_petra_port_ofp_mal_get(unit, ofp_ndx, &mal_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    /* Get existing configuration */
    res = soc_petra_ofp_rates_mal_shaper_get(unit, mal_ndx, &mal_shaper);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Set Shaper Info */
    mal_shaper.sch_shaper.rate = if_shaper_rate;
    mal_shaper.sch_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    mal_shaper.egq_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    
    /* Set configuration */
    res = soc_petra_ofp_rates_mal_shaper_set(unit, mal_ndx, &mal_shaper, &mal_exact_shaper);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

uint32
  soc_petra_ofp_rates_egq_interface_shaper_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                 if_shaper_rate
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 mal_ndx = 0;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper, mal_exact_shaper;    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_exact_shaper);
    
    /* Translate port to mal */
    res = soc_petra_port_ofp_mal_get(unit, ofp_ndx, &mal_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    /* Get existing configuration */
    res = soc_petra_ofp_rates_mal_shaper_get(unit, mal_ndx, &mal_shaper);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Set Shaper Info */
    mal_shaper.egq_shaper.rate = if_shaper_rate;
    mal_shaper.sch_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    mal_shaper.egq_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE;
    
    /* Set configuration */
    res = soc_petra_ofp_rates_mal_shaper_set(unit, mal_ndx, &mal_shaper, &mal_exact_shaper);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

uint32
  soc_petra_ofp_rates_sch_interface_shaper_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  ofp_ndx,
    SOC_SAND_OUT  uint32                 *if_shaper_rate
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 mal_ndx = 0;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper, mal_exact_shaper;    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_exact_shaper);
    
    /* Translate port to mal */
    res = soc_petra_port_ofp_mal_get(unit, ofp_ndx, &mal_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    /* Get existing configuration */
    res = soc_petra_ofp_rates_mal_shaper_get(unit, mal_ndx, &mal_shaper);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Set Shaper Info */
    *if_shaper_rate = mal_shaper.sch_shaper.rate;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

uint32
  soc_petra_ofp_rates_egq_interface_shaper_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  ofp_ndx,
    SOC_SAND_OUT  uint32                 *if_shaper_rate
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 mal_ndx = 0;
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO mal_shaper, mal_exact_shaper;    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Clear */
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_shaper);
    SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(&mal_exact_shaper);
    
    /* Translate port to mal */
    res = soc_petra_port_ofp_mal_get(unit, ofp_ndx, &mal_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    /* Get existing configuration */
    res = soc_petra_ofp_rates_mal_shaper_get(unit, mal_ndx, &mal_shaper);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Set Shaper Info */
    *if_shaper_rate = mal_shaper.egq_shaper.rate;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get()",0,0);
}

/*NEW GLUE LAYER }*/




#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

