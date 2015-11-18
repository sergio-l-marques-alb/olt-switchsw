/* $Id: petra_auto_queue_flow_mgmt.c,v 1.7 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/Petra/petra_api_auto_queue_flow_mgmt.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_port.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_aggregate.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_flow.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_queue.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_nif.h>


#include <soc/dpp/Petra/petra_scheduler_elements.h>

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

#define SOC_PETRA_AQFM_STRIP_LOGICAL_ID_BITS(logical_id) (logical_id & 0xFFFFFFF)
#define SOC_PETRA_AQFM_IS_PORT_ID(logical_id)            (logical_id & SOC_SAND_BIT(SOC_PETRA_AQFM_PORT_AUTO_RELATIVE_ID_BIT))
#define SOC_PETRA_AQFM_IS_AGG_ID(logical_id)             (logical_id & SOC_SAND_BIT(SOC_PETRA_AQFM_AGG_AUTO_RELATIVE_ID_BIT))

/* $Id: petra_auto_queue_flow_mgmt.c,v 1.7 Broadcom SDK $
 * To make things more simple, 80 destination IDs (== System ports) are allocated
 *  per FAP in the system.
 * This approach may be problematic in big system, when the number of Soc_petra devices is high
 */
#define SOC_PETRA_AQFM_NOF_DESTS_PER_FAP                SOC_PETRA_NOF_FAP_PORTS
#define SOC_PETRA_AQFM_FRST_NIF_PORT_ID                 1
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
  soc_petra_aqfm_system_info_save_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_SYSTEM_INFO   *p_info
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_SYSTEM_INFO_SAVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(p_info);

  ret = soc_petra_sw_db_auto_scheme_info_set(
          unit,
          p_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_system_info_save_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_port_first_relative_id_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_OUT uint32          *auto_relative_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_FIRST_RELATIVE_ID_GET);
  SOC_SAND_CHECK_NULL_INPUT(auto_relative_id);

  *auto_relative_id = 0 | SOC_SAND_BIT(SOC_PETRA_AQFM_PORT_AUTO_RELATIVE_ID_BIT);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_port_first_relative_id_get()",0,0);
}

uint32
  soc_petra_aqfm_port_actual2relative_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32          auto_actual_id,
    SOC_SAND_OUT uint32          *auto_relative_id
  )
{
  uint32
    res;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_fatp_ports,
    nof_fatp_ports_offset,
    nof_nif_ports,
    nof_rcy_ports;
  uint32
    auto_relative_port_id = 0;
  uint8
    is_fatp_port;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_ACTUAL2RELATIVE_GET);

  SOC_SAND_CHECK_NULL_INPUT(auto_relative_id);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_info);
  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  /*
   *  CPU | 0 | 1 | 2 | 3 | 4 | ... | 73 | ... | 78 |
   * --------------------------------
   *   0  | X | 0 | 1 | 2 | 3 | ... | X  | ... | X  |
   * --------------------------------
   *   1  | 0 | 1 | 2 | 3 | 4 | ... | X  | ... | X  |
   * --------------------------------
   *   2  | 0 | 1 | 2 | 3 | 4 | ... | 73 | ... | X  |
   * --------------------------------
   *  ...
   * --------------------------------
   *   7  | 0 | 1 | 2 | 3 | 4 | ... | 73 | ... | 78  |
   * --------------------------------
   */
  nof_nif_ports = auto_info.nof_nif_ports;

  nof_rcy_ports = 0;
  if(auto_info.rcy_port_id != SOC_PETRA_FAP_PORT_ID_INVALID)
  {
    nof_rcy_ports = 1;
  }

  nof_fatp_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);

  nof_fatp_ports_offset = (nof_fatp_ports > 0)?(nof_fatp_ports-1):0;

  res = soc_petra_nif_is_fat_pipe_port(
          unit,
          auto_actual_id,
          &is_fatp_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*ports 1 to nof_nif_ports are NIF ports */
  if ((auto_actual_id >= SOC_PETRA_AQFM_FRST_NIF_PORT_ID) &&
      (auto_actual_id < SOC_PETRA_AQFM_FRST_NIF_PORT_ID + nof_nif_ports + nof_fatp_ports_offset)
     )
  {
    if (is_fatp_port)
    {
      if (auto_actual_id == SOC_PETRA_FAT_PIPE_FAP_PORT_ID)
      {
        auto_relative_port_id = auto_actual_id - SOC_PETRA_AQFM_FRST_NIF_PORT_ID;
      }
      else
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORT_ID_CONSUMED_BY_FAT_PIPE_ERR, 25, exit);
      }
    }
    else
    {
      auto_relative_port_id = auto_actual_id - nof_fatp_ports_offset - SOC_PETRA_AQFM_FRST_NIF_PORT_ID;
    }
  }
  /*Port '0' is the first CPU port*/
  else if(auto_actual_id == SOC_PETRA_FRST_CPU_PORT_ID && auto_info.nof_cpu_ports)/*first CPU port*/
  {
    auto_relative_port_id = nof_nif_ports;
  }
  /*Ports 73 - 78 may also be CPU ports*/
  else if((auto_actual_id >= SOC_PETRA_SCND_CPU_PORT_ID) &&
          (auto_actual_id <= SOC_PETRA_LAST_CPU_PORT_ID) &&
          (auto_info.nof_cpu_ports > 1)
         )
  {
    auto_relative_port_id = (auto_actual_id - SOC_PETRA_SCND_CPU_PORT_ID) + nof_nif_ports + 1;
  }
  /*If recycling port is activated by the aqfm, SOC_PETRA_AQFM_RCY_PORT_ID_DEFAULT
  is the recycling port
  */
  else if(auto_actual_id == SOC_PETRA_AQFM_RCY_PORT_ID_DEFAULT && nof_rcy_ports)
  {
    auto_relative_port_id = nof_nif_ports + auto_info.nof_cpu_ports;
  }
  else if(auto_actual_id == SOC_PETRA_OLP_PORT_ID &&
          !nof_rcy_ports && auto_info.support_olp_port
         )
  {
    auto_relative_port_id = nof_nif_ports + auto_info.nof_cpu_ports;
  }
  else if(auto_actual_id == SOC_PETRA_OLP_PORT_ID &&
    nof_rcy_ports && auto_info.support_olp_port
    )
  {
    auto_relative_port_id = nof_nif_ports + auto_info.nof_cpu_ports + 1;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_AQFM_INVALID_PORT_ID_ERR, 44, exit);
  }

 *auto_relative_id = auto_relative_port_id | SOC_SAND_BIT(SOC_PETRA_AQFM_PORT_AUTO_RELATIVE_ID_BIT);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_port_actual2relative_get()",auto_actual_id,0);
}

uint32
  soc_petra_aqfm_port_relative2actual_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32          auto_relative_id,
    SOC_SAND_OUT uint32          *auto_actual_id
  )
{
  uint32
    res;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_fatp_ports,
    nof_fatp_ports_offset,
    actual_port_id,
    nof_nif_ports,
    nof_rcy_ports,
    relative_port_id;
  uint8
    is_fatp_port;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_RELATIVE2ACTUAL_GET);

  SOC_SAND_CHECK_NULL_INPUT(auto_actual_id);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_info);
  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

 /*
  *  CPU | 0 | 1 | 2 | 3 | 4 | ... | 73 | ... | 78 |
  * --------------------------------
  *   0  | X | 0 | 1 | 2 | 3 | ... | X  | ... | X  |
  * --------------------------------
  *   1  | 0 | 1 | 2 | 3 | 4 | ... | X  | ... | X  |
  * --------------------------------
  *   2  | 0 | 1 | 2 | 3 | 4 | ... | 73 | ... | X  |
  * --------------------------------
  *  ...
  * --------------------------------
  *   7  | 0 | 1 | 2 | 3 | 4 | ... | 73 | ... | 78  |
  * --------------------------------
  */

  relative_port_id = SOC_PETRA_AQFM_STRIP_LOGICAL_ID_BITS(auto_relative_id);

  nof_nif_ports = auto_info.nof_nif_ports;
  nof_rcy_ports = 0;
  if(auto_info.rcy_port_id != SOC_PETRA_FAP_PORT_ID_INVALID)
  {
    nof_rcy_ports = 1;
  }

  nof_fatp_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);

  nof_fatp_ports_offset = (nof_fatp_ports > 0)?(nof_fatp_ports - 1):0;

  /*The first nof_nif_ports relative ports are NIF ports*/
  /*The NIF ports mapped to 1 - nof_nif_ports*/
  if (relative_port_id < nof_nif_ports)
  {
    actual_port_id = relative_port_id + SOC_PETRA_AQFM_FRST_NIF_PORT_ID;

    res = soc_petra_nif_is_fat_pipe_port(
            unit,
            (actual_port_id + nof_fatp_ports_offset),
            &is_fatp_port
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if ((!is_fatp_port))
    {
      actual_port_id += nof_fatp_ports_offset;
    }
  }

  /*The next consecutive relative ports are the CPU ports*/
  else if(relative_port_id < (nof_nif_ports + auto_info.nof_cpu_ports))
  {
    if(relative_port_id == nof_nif_ports)/*first CPU port*/
    {
      actual_port_id = SOC_PETRA_FRST_CPU_PORT_ID;
    }
    else
    {
      actual_port_id = SOC_PETRA_SCND_CPU_PORT_ID + (relative_port_id - (nof_nif_ports + 1));
    }
  }
  /*The last port ID is the Recycling port ID*/
  else if(relative_port_id == (nof_nif_ports + auto_info.nof_cpu_ports) && nof_rcy_ports)
  {
    actual_port_id = SOC_PETRA_AQFM_RCY_PORT_ID_DEFAULT;
  }
  else if(relative_port_id == (nof_nif_ports + auto_info.nof_cpu_ports) &&
          !nof_rcy_ports && auto_info.support_olp_port
         )
  {
    actual_port_id = SOC_PETRA_OLP_PORT_ID;
  }
  else if(relative_port_id == (nof_nif_ports + auto_info.nof_cpu_ports + 1) &&
    nof_rcy_ports && auto_info.support_olp_port
    )
  {
    actual_port_id = SOC_PETRA_OLP_PORT_ID;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_AQFM_INVALID_PORT_ID_ERR, 44, exit);
  }

  *auto_actual_id = actual_port_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_port_relative2actual_get()",auto_relative_id,0);
}

uint32
  soc_petra_aqfm_nof_relative_port_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_OUT uint32          *nof_ports
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_rcy_ports;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_NOF_RELATIVE_PORT_GET);

  SOC_SAND_CHECK_NULL_INPUT(nof_ports);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_info);
  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  nof_rcy_ports = 0;
  if(auto_info.rcy_port_id != SOC_PETRA_FAP_PORT_ID_INVALID)
  {
    nof_rcy_ports = 1;
  }

  *nof_ports = auto_info.nof_nif_ports + auto_info.nof_cpu_ports + nof_rcy_ports + auto_info.support_olp_port ;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_nof_relative_port_get()",0,0);
}


uint32
  soc_petra_aqfm_agg_relative_id_get(
    SOC_SAND_IN  uint32 agg_level,
    SOC_SAND_IN  uint32 agg_index,
    SOC_SAND_OUT uint32  *agg_relative_id
  )
{
  uint32
    agg_rel_agg_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_RELATIVE_ID_GET);

  SOC_SAND_CHECK_NULL_INPUT(agg_relative_id);

  agg_rel_agg_id |= SOC_SAND_SET_BYTE_1(agg_level);
  agg_rel_agg_id |= SOC_SAND_SET_BYTE_2(agg_index);
  agg_rel_agg_id |= SOC_SAND_BIT(SOC_PETRA_AQFM_AGG_AUTO_RELATIVE_ID_BIT);

  *agg_relative_id = agg_rel_agg_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_relative_id_get()",0,0);
}

uint32
  soc_petra_aqfm_queue_id_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32           destination_id,
    SOC_SAND_IN  uint32          class_id,
    SOC_SAND_OUT uint32           *auto_queue_id
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    dest_offset;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_QUEUE_ID_GET);

  SOC_SAND_CHECK_NULL_INPUT(auto_queue_id);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_info);
  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  dest_offset = (destination_id - SOC_PETRA_AQFM_FIRST_DESTINATION_ID) * auto_info.nof_traffic_classes;

  *auto_queue_id = SOC_PETRA_AQFM_FIRST_QUEUE_ID + dest_offset + class_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_queue_id_get()",0,0);
}

uint32
  soc_petra_aqfm_flow_id_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32          source_fap_id,
    SOC_SAND_IN  uint32          port_id,
    SOC_SAND_IN  uint32          class_id,
    SOC_SAND_OUT uint32           *auto_flow_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    src_fap_offset,
    port_id_offset,
    class_offset,
    port_index;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_FLOW_ID_GET);

  SOC_SAND_CHECK_NULL_INPUT(auto_flow_id);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_info);
  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  res = soc_petra_aqfm_port_actual2relative_get(
          unit,
          port_id,
          &port_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  port_index = SOC_PETRA_AQFM_STRIP_LOGICAL_ID_BITS(port_index);

  src_fap_offset = SOC_SAND_DIV_ROUND_UP(
                     source_fap_id *
                     SOC_PETRA_AQFM_NOF_DESTS_PER_FAP *
                     auto_info.nof_traffic_classes,
                     4
                   );

  src_fap_offset *=4;

  port_id_offset = port_index *
                   auto_info.nof_traffic_classes;

  class_offset = class_id;

  *auto_flow_id = SOC_PETRA_AQFM_FIRST_FLOW_ID +
                    (src_fap_offset + port_id_offset + class_offset) * 2;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_flow_id_get()",0,0);
}

uint32
  soc_petra_aqfm_system_physical_port_id_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  dest_id,
    SOC_SAND_OUT uint32  *system_physical_port_id
  )
{
  uint32
    res,
    relative_dest_id;
  uint32
    local_port;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_SYSTEM_PHYSICAL_PORT_ID_GET);

  SOC_SAND_CHECK_NULL_INPUT(system_physical_port_id);

  relative_dest_id = dest_id % SOC_PETRA_AQFM_NOF_DESTS_PER_FAP;

  res = soc_petra_aqfm_port_relative2actual_get(
          unit,
          relative_dest_id,
          &local_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  dest_id - (minus) relative_dest_id is local port 0 in the requested device
   */
  *system_physical_port_id =
    SOC_PETRA_AQFM_FIRST_PHYSICAL_SYSTEM_PORT_ID + local_port + (dest_id - relative_dest_id);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_system_physical_port_id_get()",0,0);
}

uint32
  soc_petra_aqfm_destination_id_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32          destination_fap_id,
    SOC_SAND_IN  uint32          destination_port_id,
    SOC_SAND_OUT uint32           *auto_destination_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    dest_fap_offset,
    dest_port_offset;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_DESTINATION_ID_GET);

  SOC_SAND_CHECK_NULL_INPUT(auto_destination_id);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_info);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  res = soc_petra_aqfm_port_actual2relative_get(
          unit,
          destination_port_id,
          &dest_port_offset
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  dest_fap_offset = destination_fap_id * SOC_PETRA_AQFM_NOF_DESTS_PER_FAP;

  dest_port_offset = SOC_PETRA_AQFM_STRIP_LOGICAL_ID_BITS(dest_port_offset);

  *auto_destination_id = SOC_PETRA_AQFM_FIRST_DESTINATION_ID + dest_fap_offset + dest_port_offset;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_destination_id_get()",0,0);
}

uint32
  soc_petra_aqfm_agg_base_ids_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                port_id,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_BASE_IDS *agg_base_ids
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_ports,
    relative_port_id,
    next_relative_port,
    auto_relative_port_id;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_BASE_IDS_GET);

  SOC_SAND_CHECK_NULL_INPUT(agg_base_ids);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_info);
  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_info
          );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  nof_ports = SOC_PETRA_AQFM_NOF_DESTS_PER_FAP;

  res = soc_petra_aqfm_port_actual2relative_get(
          unit,
          port_id,
          &auto_relative_port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  relative_port_id = SOC_PETRA_AQFM_STRIP_LOGICAL_ID_BITS(auto_relative_port_id);
  next_relative_port = relative_port_id + 1;

  /*
   * HR available SE. The first 64 HR scheduler ports are reserved for the port.
   * Thus may NOT be allocated as aggregates (that's why SOC_PETRA_NOF_FAP_PORTS is added).
   */
  agg_base_ids->hr.min =
    auto_info.agg_base_id.hr.min +
    (((auto_info.agg_base_id.hr.max - auto_info.agg_base_id.hr.min + 1) / nof_ports) * relative_port_id);

  agg_base_ids->hr.max =
    auto_info.agg_base_id.hr.min +
    (((auto_info.agg_base_id.hr.max - auto_info.agg_base_id.hr.min + 1) / nof_ports) * next_relative_port);

  /*
   * CL available SE
   */
  agg_base_ids->cl.min =
    auto_info.agg_base_id.cl.min +
    (((auto_info.agg_base_id.cl.max - auto_info.agg_base_id.cl.min + 1) / nof_ports) * relative_port_id);

  agg_base_ids->cl.max =
    auto_info.agg_base_id.cl.min +
    (((auto_info.agg_base_id.cl.max - auto_info.agg_base_id.cl.min + 1) / nof_ports) * next_relative_port);

  /*
   * FQ available SE
   */
  agg_base_ids->fq.min =
    auto_info.agg_base_id.fq.min +
    (((auto_info.agg_base_id.fq.max - auto_info.agg_base_id.fq.min + 1) / nof_ports) * relative_port_id);

  agg_base_ids->fq.max =
    auto_info.agg_base_id.fq.min +
    (((auto_info.agg_base_id.fq.max - auto_info.agg_base_id.fq.min + 1) / nof_ports) * next_relative_port);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_base_ids_get()", port_id, 0);
}

uint32
  soc_petra_aqfm_credit_sources_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        port_id,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW  *logical_flow,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW  *physical_flow
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    sub_flow_i,
    nof_sub_flows,
    logical_credit_source;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_GET_CREDIT_SOURCES);

  SOC_SAND_CHECK_NULL_INPUT(logical_flow);
  SOC_SAND_CHECK_NULL_INPUT(physical_flow);

  nof_sub_flows = logical_flow->sub_flow[1].is_valid == TRUE ? 2 : 1;

  for(sub_flow_i = 0; sub_flow_i < nof_sub_flows; ++sub_flow_i)
  {
    logical_credit_source = logical_flow->sub_flow[sub_flow_i].credit_source.id;

    if(logical_credit_source == SOC_PETRA_AQFM_CREDIT_SOURCE_IS_LOCAL_PORT)
    {
      physical_flow->sub_flow[sub_flow_i].credit_source.id = SOC_PETRA_HR_SE_ID_MIN + port_id;
    }
    else if(SOC_PETRA_AQFM_IS_PORT_ID(logical_credit_source))
    {
      res = soc_petra_aqfm_port_relative2actual_get(
              unit,
              logical_credit_source,
              &(physical_flow->sub_flow[sub_flow_i].credit_source.id)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if(SOC_PETRA_AQFM_IS_AGG_ID(logical_credit_source))
    {
      res = soc_petra_aqfm_agg_actual_id_get(
              unit,
              port_id,
              logical_credit_source,
              &(physical_flow->sub_flow[sub_flow_i].credit_source.id)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_AQFM_CREDIT_SOURCE_ID_ERR, 2, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_credit_sources_get()",0,0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
