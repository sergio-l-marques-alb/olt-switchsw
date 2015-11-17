/* $Id: soc_pb_sw_db_tcam_mgmt.c,v 1.5 Broadcom SDK $
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

#include <soc/dpp/Petra/PB_TM/pb_sw_db_tcam_mgmt.h>

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

#define SOC_PB_SW_DB_TCAM_USE_TO_LIST_ID(tcam_app_type, acl_id)   \
  ((tcam_app_type) >= SOC_PB_TCAM_USER_FP) ? (tcam_app_type) + ((acl_id) << 1) : (tcam_app_type);

#define SOC_PB_SW_DB_TCAM_USE_TO_RESOURCES_ID(tcam_app_type,acl_id)   \
  SOC_PB_SW_DB_TCAM_USE_TO_LIST_ID(tcam_app_type,acl_id)

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

/********************************************************************************************
 * Configuration
 * {
 ********************************************************************************************/


/* $Id: soc_pb_sw_db_tcam_mgmt.c,v 1.5 Broadcom SDK $
 * } Configuration
 */


/*************
 * FUNCTIONS *
 *************/
/* { */


/********************************************************************************************
 * Initialization
 * {
 ********************************************************************************************/




uint32
  soc_pb_sw_db_tcam_use_type_resources_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER           tcam_app_type,
    SOC_SAND_IN  uint32                    acl_id,
    SOC_SAND_OUT uint8                     resources[SOC_PB_TCAM_NOF_BANKS]
  )
{
 SOC_PB_SW_DB_TCAM_MGMT_INFO
   *tcam_mgmt;
 uint32
   list_use_id;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_sw_db_tcam_info_get(
          unit,
          &tcam_mgmt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  list_use_id = SOC_PB_SW_DB_TCAM_USE_TO_RESOURCES_ID(tcam_app_type,acl_id);

  res = SOC_SAND_OK; sal_memcpy(
             resources,
             tcam_mgmt->uses_info[list_use_id].resources,
             sizeof(uint8) * SOC_PB_TCAM_NOF_BANKS
           );
 SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_use_type_resources_get()",0,0);
}


uint32
  soc_pb_sw_db_tcam_use_type_resources_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER           tcam_app_type,
    SOC_SAND_IN  uint32                    acl_id,
    SOC_SAND_IN  uint8                     resources[SOC_PB_TCAM_NOF_BANKS]
  )
{
 SOC_PB_SW_DB_TCAM_MGMT_INFO
   *tcam_mgmt;
 uint32
   list_use_id;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_sw_db_tcam_info_get(
          unit,
          &tcam_mgmt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  list_use_id = SOC_PB_SW_DB_TCAM_USE_TO_RESOURCES_ID(tcam_app_type,acl_id);

  res = SOC_SAND_OK; sal_memcpy(
             tcam_mgmt->uses_info[list_use_id].resources,
             resources,
             sizeof(uint8) * SOC_PB_TCAM_NOF_BANKS
           );
 SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_use_type_resources_set()",0,0);
}

uint32
  soc_pb_sw_db_tcam_use_key_src_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER                tcam_app_type,
    SOC_SAND_IN  uint32                    acl_id,
    SOC_SAND_OUT SOC_PB_PMF_TCAM_KEY_SRC       *key_src
  )
{
 SOC_PB_SW_DB_TCAM_MGMT_INFO
   *tcam_mgmt;
 uint32
   list_use_id;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_sw_db_tcam_info_get(
          unit,
          &tcam_mgmt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  list_use_id = SOC_PB_SW_DB_TCAM_USE_TO_RESOURCES_ID(tcam_app_type,acl_id);

  *key_src = tcam_mgmt->uses_info[list_use_id].key_src;
 
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_use_key_src_get()",0,0);
}

uint32
  soc_pb_sw_db_tcam_use_key_src_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER                tcam_app_type,
    SOC_SAND_IN  uint32                    acl_id,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SRC        key_src
  )
{
 SOC_PB_SW_DB_TCAM_MGMT_INFO
   *tcam_mgmt;
 uint32
   list_use_id;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_sw_db_tcam_info_get(
          unit,
          &tcam_mgmt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  list_use_id = SOC_PB_SW_DB_TCAM_USE_TO_RESOURCES_ID(tcam_app_type,acl_id);

  tcam_mgmt->uses_info[list_use_id].key_src = key_src;

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_use_key_src_set()",0,0);
}

uint32
  soc_pb_sw_db_tcam_use_fem_is_present_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER                tcam_app_type,
    SOC_SAND_IN  uint32                    acl_id,
    SOC_SAND_IN  uint32                    fem_id,
    SOC_SAND_IN  uint32                    cycle_id,
    SOC_SAND_IN  uint8                   is_present
  )
{
 SOC_PB_SW_DB_TCAM_MGMT_INFO
   *tcam_mgmt;
 uint32
   list_use_id;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_sw_db_tcam_info_get(
          unit,
          &tcam_mgmt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  list_use_id = SOC_PB_SW_DB_TCAM_USE_TO_RESOURCES_ID(tcam_app_type,acl_id);

  tcam_mgmt->uses_info[list_use_id].is_in_fem[fem_id][cycle_id] = is_present;

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_use_fem_is_present_set()",0,0);
}

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>
