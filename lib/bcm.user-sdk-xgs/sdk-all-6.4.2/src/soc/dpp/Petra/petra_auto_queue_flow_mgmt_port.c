/* $Id: petra_auto_queue_flow_mgmt_port.c,v 1.10 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_auto_queue_flow_mgmt_port.c
*
* MODULE PREFIX:  soc_petra_aqfm_port
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
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/Petra/petra_api_auto_queue_flow_mgmt.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_port.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_aggregate.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_flow.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_queue.h>

#include <soc/dpp/Petra/petra_ofp_rates.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_scheduler_ports.h>
#include <soc/dpp/Petra/petra_scheduler_elements.h>
#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_AQFM_PORT_DEFAULT_CREDIT_SPEED_UP_NUMERATOR    1050
#define SOC_PETRA_AQFM_PORT_DEFAULT_MAX_RATE_SPEED_UP_NUMERATOR  1005
#define SOC_PETRA_AQFM_PORT_DEFAULT_DENOMINATOR                  1000

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

STATIC uint32
  soc_petra_aqfm_mal_dflt_shaper_get(
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_MAL_DFLT_SHAPER_GET);

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(shaper);
  shaper->sch_shaper.rate_update_mode = SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
  shaper->egq_shaper.rate_update_mode = SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_mal_dflt_shaper_get()",0,0);
}

uint32
  soc_petra_aqfm_port_default_get(
    SOC_SAND_IN  uint32              nominal_rate_kbps,
    SOC_SAND_OUT SOC_PETRA_AQFM_PORT_INFO *p_port_info
  )
  {
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_DEFAULT_GET);

  SOC_SAND_CHECK_NULL_INPUT(p_port_info);

  soc_petra_PETRA_SCH_PORT_INFO_clear(&(p_port_info->port_info));

  p_port_info->eg_port.egq_rate            = nominal_rate_kbps /
                                             SOC_PETRA_AQFM_PORT_DEFAULT_DENOMINATOR *
                                             SOC_PETRA_AQFM_PORT_DEFAULT_MAX_RATE_SPEED_UP_NUMERATOR;
  p_port_info->eg_port.sch_rate            = nominal_rate_kbps /
                                             SOC_PETRA_AQFM_PORT_DEFAULT_DENOMINATOR *
                                             SOC_PETRA_AQFM_PORT_DEFAULT_CREDIT_SPEED_UP_NUMERATOR;


  p_port_info->port_info.enable            = TRUE;
  p_port_info->port_info.hr_mode           = SOC_PETRA_SCH_HR_MODE_SINGLE_WFQ;
  p_port_info->port_info.lowest_hp_class   = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_port_default_get()",0,0);
}

uint32
  soc_petra_aqfm_port_scheduler_port_default_get(
    SOC_SAND_OUT SOC_PETRA_SCH_PORT_INFO *p_sch_port
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_SCHEDULER_PORT_DEFAULT_GET);
  SOC_SAND_CHECK_NULL_INPUT(p_sch_port);

  soc_petra_PETRA_SCH_PORT_INFO_clear(p_sch_port);

  p_sch_port->enable            = TRUE;
  p_sch_port->hr_mode           = SOC_PETRA_SCH_HR_MODE_SINGLE_WFQ;
  p_sch_port->lowest_hp_class   = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_port_scheduler_port_default_get()",0,0);
}

uint32
  soc_petra_aqfm_port_open_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO  *port_info,
    SOC_SAND_IN  uint8              update_eg_port
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_OFP_RATE_INFO
    exact_port_qos;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    shaper,
    exact_shaper;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_OPEN);

  soc_petra_PETRA_OFP_RATE_INFO_clear(&exact_port_qos);
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&exact_shaper);

  res = soc_petra_aqfm_mal_dflt_shaper_get(
          &shaper
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_petra_sch_port_sched_set_unsafe(
          unit,
          port_id,
          &(port_info->port_info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if(update_eg_port)
  {
    res = soc_petra_ofp_rates_single_port_set_unsafe(
            unit,
            port_id,
            &shaper,
            &(port_info->eg_port)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_port_open_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_all_ports_open_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO     *p_port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    port_id,
    ports_num;
  uint32
    port_i,
    first_relative_id;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_info;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    shaper,
    exact_shaper;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_ALL_PORTS_OPEN_UNSAFE);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_info);
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&exact_shaper);

  res = soc_petra_aqfm_mal_dflt_shaper_get(
          &shaper
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

  ports_num = auto_info.max_nof_ports_per_fap;

  res = soc_petra_aqfm_port_first_relative_id_get(
          unit,
          &first_relative_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for(port_i = first_relative_id; port_i < ports_num + first_relative_id; ++port_i)
  {
    res = soc_petra_aqfm_port_relative2actual_get(
            unit,
            port_i,
            &port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


    res = soc_petra_aqfm_port_open_unsafe(
            unit,
            port_id,
            p_port_info,
            FALSE
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_all_ports_open_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_port_close_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_AQFM_PORT_INFO
    port_info;
  SOC_PETRA_OFP_RATE_INFO
    exact_port_qos;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    shaper,
    exact_shaper;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_CLOSE);

  soc_petra_PETRA_AQFM_PORT_INFO_clear(&port_info);
  soc_petra_PETRA_OFP_RATE_INFO_clear(&exact_port_qos);
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&exact_shaper);

  res = soc_petra_aqfm_mal_dflt_shaper_get(
          &shaper
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_petra_aqfm_port_default_get(
          0,
          &port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  port_info.port_info.enable = FALSE;
  res = soc_petra_sch_port_sched_set_unsafe(
          unit,
          port_id,
          &(port_info.port_info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_ofp_rates_single_port_set_unsafe(
          unit,
          port_id,
          &shaper,
          &(port_info.eg_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_flow_open_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_port_update_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO *p_port_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_PORT_INFO *exact_port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  /*
    uint8
        update_eg_port = (port_id != SOC_PETRA_RCY_PORT_ID ? TRUE : FALSE);*/
  uint8
    update_eg_port = TRUE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_UPDATE_UNSAFE);

  res = soc_petra_aqfm_port_open_unsafe(
          unit,
          port_id,
          p_port_info,
          update_eg_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_port_update_unsafe()",0,0);
}

#if SOC_PETRA_DEBUG

void
  soc_petra_aqfm_port_port_scheme_port_print(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 port_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_SCH_PORT_INFO
    sch_port;
  /* The stucture SOC_PETRA_OFP_RATES_TBL_INFO is very big */
  SOC_PETRA_OFP_RATES_TBL_INFO
    *port_table = NULL;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    shaper;
  uint32
    mal_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_PORT_SCHEME_PORT_PRINT);

  soc_petra_PETRA_SCH_PORT_INFO_clear(&sch_port);
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&shaper);

  res = soc_petra_port_ofp_mal_get_unsafe(
          unit,
          port_id,
          &mal_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PETRA_ALLOC(port_table, SOC_PETRA_OFP_RATES_TBL_INFO, 1);
  soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(port_table);

  res = soc_petra_ofp_rates_get(
          unit,
          mal_idx,
          &shaper,
          port_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_sch_port_sched_get(
          unit,
          port_id,
          &sch_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  soc_petra_PETRA_OFP_RATE_INFO_print(&(port_table->rates[port_id]));
  soc_petra_PETRA_SCH_PORT_INFO_print(&sch_port, port_id);

exit:
  SOC_PETRA_FREE(port_table);
  if (ex != no_err)
  {
    soc_sand_os_printf("Error occurred in %s.\n\r", "soc_petra_aqfm_port_port_scheme_port_print");
  }
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0)
}

#endif /* SOC_PETRA_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
