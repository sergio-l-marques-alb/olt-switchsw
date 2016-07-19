/* $Id: pcp_api_tbl_access.c,v 1.3 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
                                                                    
#include <soc/dpp/PCP/pcp_api_tbl_access.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>
#include <soc/dpp/PCP/pcp_api_framework.h>

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
  pcp_eci_qdr_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_QDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_QDR_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_eci_qdr_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ECI_qdr_tbl_get()",0,0);
}

uint32
  pcp_eci_qdr_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_QDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_QDR_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_eci_qdr_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ECI_qdr_tbl_set()",0,0);
}

uint32
  pcp_eci_rld1_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_RLD1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD1_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_eci_rld1_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ECI_rld1_tbl_get()",0,0);
}

uint32
  pcp_eci_rld1_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD1_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_eci_rld1_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ECI_rld1_tbl_set()",0,0);
}

uint32
  pcp_eci_rld2_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_RLD2_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD2_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_eci_rld2_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ECI_rld2_tbl_get()",0,0);
}

uint32
  pcp_eci_rld2_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD2_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD2_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_eci_rld2_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ECI_rld2_tbl_set()",0,0);
}

uint32
  pcp_elk_fid_counter_profile_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_fid_counter_profile_db_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_fid_counter_profile_db_tbl_get()",0,0);
}

uint32
  pcp_elk_fid_counter_profile_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_fid_counter_profile_db_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_fid_counter_profile_db_tbl_set()",0,0);
}

uint32
  pcp_elk_fid_counter_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FID_COUNTER_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FID_COUNTER_DB_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_fid_counter_db_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_fid_counter_db_tbl_get()",0,0);
}

uint32
  pcp_elk_fid_counter_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FID_COUNTER_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FID_COUNTER_DB_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_fid_counter_db_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_fid_counter_db_tbl_set()",0,0);
}

uint32
  pcp_elk_sys_port_is_mine_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_SYS_PORT_IS_MINE_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_sys_port_is_mine_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_sys_port_is_mine_tbl_get()",0,0);
}

uint32
  pcp_elk_sys_port_is_mine_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_SYS_PORT_IS_MINE_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_sys_port_is_mine_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_sys_port_is_mine_tbl_set()",0,0);
}

uint32
  pcp_elk_aging_cfg_table_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_AGING_CFG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_AGING_CFG_TABLE_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_aging_cfg_table_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_aging_cfg_table_tbl_get()",0,0);
}

uint32
  pcp_elk_aging_cfg_table_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_AGING_CFG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_AGING_CFG_TABLE_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_aging_cfg_table_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_aging_cfg_table_tbl_set()",0,0);
}

uint32
  pcp_elk_flush_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FLUSH_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FLUSH_DB_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_flush_db_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_flush_db_tbl_get()",0,0);
}

uint32
  pcp_elk_flush_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FLUSH_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FLUSH_DB_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_flush_db_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_flush_db_tbl_set()",0,0);
}

uint32
  pcp_elk_dsp_event_table_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_DSP_EVENT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_DSP_EVENT_TABLE_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_dsp_event_table_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_dsp_event_table_tbl_get()",0,0);
}

uint32
  pcp_elk_dsp_event_table_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_DSP_EVENT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_DSP_EVENT_TABLE_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_elk_dsp_event_table_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_ELK_dsp_event_table_tbl_set()",0,0);
}

uint32
  pcp_oam_pr2_tcdp_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_PR2_TCDP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_PR2_TCDP_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_pr2_tcdp_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_pr2_tcdp_tbl_get()",0,0);
}

uint32
  pcp_oam_pr2_tcdp_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_PR2_TCDP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_PR2_TCDP_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_pr2_tcdp_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_pr2_tcdp_tbl_set()",0,0);
}

uint32
  pcp_oam_ext_mep_index_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_EXT_MEP_INDEX_DB_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_ext_mep_index_db_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_ext_mep_index_db_tbl_get()",0,0);
}

uint32
  pcp_oam_ext_mep_index_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_EXT_MEP_INDEX_DB_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_ext_mep_index_db_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_ext_mep_index_db_tbl_set()",0,0);
}

uint32
  pcp_oam_ext_mep_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_EXT_MEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_EXT_MEP_DB_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_ext_mep_db_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_ext_mep_db_tbl_get()",0,0);
}

uint32
  pcp_oam_ext_mep_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_EXT_MEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_EXT_MEP_DB_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_ext_mep_db_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_ext_mep_db_tbl_set()",0,0);
}

uint32
  pcp_oam_mep_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_MEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MEP_DB_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mep_db_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_mep_db_tbl_get()",0,0);
}

uint32
  pcp_oam_mep_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_MEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MEP_DB_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mep_db_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_mep_db_tbl_set()",0,0);
}

uint32
  pcp_oam_rmep_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_DB_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_rmep_db_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_rmep_db_tbl_get()",0,0);
}

uint32
  pcp_oam_rmep_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_DB_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_rmep_db_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_rmep_db_tbl_set()",0,0);
}

uint32
  pcp_oam_rmep_hash_0_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_HASH_0_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_HASH_0_DB_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_rmep_hash_0_db_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_rmep_hash_0_db_tbl_get()",0,0);
}

uint32
  pcp_oam_rmep_hash_0_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_HASH_0_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_HASH_0_DB_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_rmep_hash_0_db_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_rmep_hash_0_db_tbl_set()",0,0);
}

uint32
  pcp_oam_rmep_hash_1_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_HASH_1_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_HASH_1_DB_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_rmep_hash_1_db_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_rmep_hash_1_db_tbl_get()",0,0);
}

uint32
  pcp_oam_rmep_hash_1_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_HASH_1_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_HASH_1_DB_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_rmep_hash_1_db_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_rmep_hash_1_db_tbl_set()",0,0);
}

uint32
  pcp_oam_lmdb_cmn_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_LMDB_CMN_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_LMDB_CMN_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_lmdb_cmn_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_lmdb_cmn_tbl_get()",0,0);
}

uint32
  pcp_oam_lmdb_cmn_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_LMDB_CMN_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_LMDB_CMN_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_lmdb_cmn_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_lmdb_cmn_tbl_set()",0,0);
}

uint32
  pcp_oam_lmdb_tx_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_LMDB_TX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_LMDB_TX_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_lmdb_tx_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_lmdb_tx_tbl_get()",0,0);
}

uint32
  pcp_oam_lmdb_tx_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_LMDB_TX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_LMDB_TX_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_lmdb_tx_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_lmdb_tx_tbl_set()",0,0);
}

uint32
  pcp_oam_dmdb_tx_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_DMDB_TX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_DMDB_TX_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_dmdb_tx_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_dmdb_tx_tbl_get()",0,0);
}

uint32
  pcp_oam_dmdb_tx_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_DMDB_TX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_DMDB_TX_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_dmdb_tx_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_dmdb_tx_tbl_set()",0,0);
}

uint32
  pcp_oam_dmdb_rx_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_DMDB_RX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_DMDB_RX_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_dmdb_rx_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_dmdb_rx_tbl_get()",0,0);
}

uint32
  pcp_oam_dmdb_rx_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_DMDB_RX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_DMDB_RX_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_dmdb_rx_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_OAM_dmdb_rx_tbl_set()",0,0);
}

uint32
  pcp_sts_ing_counters_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_STS_ING_COUNTERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_ING_COUNTERS_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_sts_ing_counters_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_STS_ing_counters_tbl_get()",0,0);
}

uint32
  pcp_sts_ing_counters_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_STS_ING_COUNTERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_ING_COUNTERS_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_sts_ing_counters_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_STS_ing_counters_tbl_set()",0,0);
}

uint32
  pcp_sts_egr_counters_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_STS_EGR_COUNTERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_EGR_COUNTERS_TBL_GET);
      
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_sts_egr_counters_tbl_get_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_STS_egr_counters_tbl_get()",0,0);
}

uint32
  pcp_sts_egr_counters_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_STS_EGR_COUNTERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_EGR_COUNTERS_TBL_SET);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_sts_egr_counters_tbl_set_unsafe(
      unit,
      entry_offset,
      tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_STS_egr_counters_tbl_set()",0,0);
}



/* } */
                                                                    
#include <soc/dpp/SAND/Utils/sand_footer.h>
                                                                    
