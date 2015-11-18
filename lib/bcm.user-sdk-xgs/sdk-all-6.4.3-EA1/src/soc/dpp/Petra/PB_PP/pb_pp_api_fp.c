/* $Id: pb_pp_api_fp.c,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_fp.c
*
* MODULE PREFIX:  soc_pb_pp
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
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_fp.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>

#include <soc/dpp/Petra/petra_header_parsing_utils.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FP_SUBNET_LENGTH_IPV4_MAX                        (32)
#define SOC_PB_PP_FP_SUBNET_LENGTH_IPV6_MAX                        (64)

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
*     Set a Packet Format Group (PFG). The packet format group
 *     defines the supported Packet formats. The user must
 *     indicate for each Database which Packet format(s) are
 *     associated with this Database. E.g.: A Packet Format
 *     Group including only IPv6 packets can be defined to use
 *     Databases with IPv6 Destination-IP qualifiers.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_format_group_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_fp_packet_format_group_set_verify(
          unit,
          pfg_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_packet_format_group_set_unsafe(
          unit,
          pfg_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_packet_format_group_set()", pfg_ndx, 0);
}

/*********************************************************************
*     Set a Packet Format Group (PFG). The packet format group
 *     defines the supported Packet formats. The user must
 *     indicate for each Database which Packet format(s) are
 *     associated with this Database. E.g.: A Packet Format
 *     Group including only IPv6 packets can be defined to use
 *     Databases with IPv6 Destination-IP qualifiers.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_format_group_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_PFG_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_fp_packet_format_group_get_verify(
          unit,
          pfg_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_packet_format_group_get_unsafe(
          unit,
          pfg_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_packet_format_group_get()", pfg_ndx, 0);
}

/*********************************************************************
*     Create a database. Each database specifies the action
 *     types to perform and the qualifier fields for this
 *     Database. Entries in the database specify the specific
 *     actions to be taken upon specific values of the
 *     packet. E.g.: Policy Based Routing database update the
 *     FEC value according to DSCP DIP and In-RIF. An entry in
 *     the database may set the FEC of a packet with DIP
 *     1.2.2.3, DSCP value 7 and In-RIF 3 to be 9.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_database_create(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_CREATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_fp_database_create_verify(
          unit,
          db_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_database_create_unsafe(
          unit,
          db_id_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_create()", db_id_ndx, 0);
}

/*********************************************************************
*     Get the database parameters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_database_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO                    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_fp_database_get_verify(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_get()", db_id_ndx, 0);
}

/*********************************************************************
*     Destroy the database: all its entries are suppressed and
 *     the Database-ID is freed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_database_destroy(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_DESTROY);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_fp_database_destroy_verify(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_database_destroy_unsafe(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_destroy()", db_id_ndx, 0);
}

/*********************************************************************
*     Add an entry to the Database. The database entry is
 *     selected if the entire relevant packet field values are
 *     matched to the database entry qualifiers values. When
 *     the packet is qualified to several entries, the entry
 *     with the strongest priority is chosen.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO                       *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_fp_entry_add_verify(
          unit,
          db_id_ndx,
          entry_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_entry_add_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_add()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_found);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_fp_entry_get_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_entry_get_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          is_found,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_get()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Remove an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_fp_entry_remove_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_entry_remove_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_remove()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get the Database entries. The function returns list of
 *     entries that were added to a database with database ID
 *     'db_id_ndx'.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_database_entries_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = soc_pb_pp_fp_database_entries_get_block_verify(
          unit,
          db_id_ndx,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_database_entries_get_block_unsafe(
          unit,
          db_id_ndx,
          block_range,
          entries,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_entries_get_block()", db_id_ndx, 0);
}

/*********************************************************************
*     Add an entry to the Database. The database entry is
 *     selected if all the Packet Qualifier field values are in
 *     the Database entry range.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_fp_direct_extraction_entry_add_verify(
          unit,
          db_id_ndx,
          entry_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_direct_extraction_entry_add_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_entry_add()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_found);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_fp_direct_extraction_entry_get_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_direct_extraction_entry_get_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          is_found,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_entry_get()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Remove an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_fp_direct_extraction_entry_remove_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_direct_extraction_entry_remove_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_entry_remove()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get the Database entries.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_db_entries_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = soc_pb_pp_fp_direct_extraction_db_entries_get_verify(
          unit,
          db_id_ndx,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_direct_extraction_db_entries_get_unsafe(
          unit,
          db_id_ndx,
          block_range,
          entries,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_db_entries_get()", db_id_ndx, 0);
}

/*********************************************************************
*     Set one of the control options.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_control_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_CONTROL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(control_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_fp_control_set_verify(
          unit,
          control_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_control_set_unsafe(
          unit,
          control_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_control_set()", 0, 0);
}

/*********************************************************************
*     Set one of the control options.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_control_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_CONTROL_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_CONTROL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(control_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_fp_control_get_verify(
          unit,
          control_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_control_get_unsafe(
          unit,
          control_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_control_get()", 0, 0);
}

/*********************************************************************
*     Set the mapping between the Packet forward type and the
 *     Port profile to the Database-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DB_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_fp_egr_db_map_set_verify(
          unit,
          fwd_type_ndx,
          port_profile_ndx,
          db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_egr_db_map_set_unsafe(
          unit,
          fwd_type_ndx,
          port_profile_ndx,
          db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_db_map_set()", 0, port_profile_ndx);
}

/*********************************************************************
*     Set the mapping between the Packet forward type and the
 *     Port profile to the Database-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_OUT uint32                     *db_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DB_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(db_id);

  res = soc_pb_pp_fp_egr_db_map_get_verify(
          unit,
          fwd_type_ndx,
          port_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_egr_db_map_get_unsafe(
          unit,
          fwd_type_ndx,
          port_profile_ndx,
          db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_db_map_get()", 0, port_profile_ndx);
}

uint32
  soc_pb_pp_fp_qual_type_preset(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PP_FP_PREDEFINED_ACL_KEY predefined_key,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_TYPE          qual_types[SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX]
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    qual_type_ndx;
  SOC_PB_PP_FP_CONTROL_INFO
    control_info;
  SOC_PB_PP_FP_CONTROL_INDEX
    control_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_QUAL_TYPE_PRESET1);

  for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
  {
    qual_types[qual_type_ndx] = SOC_PB_PP_NOF_FP_QUAL_TYPES;
  }

  qual_type_ndx = 0;
  switch (predefined_key)
  {
    /*
     * L2
     */
  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2:
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_VLAN_FORMAT;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_2ND_VLAN_TAG;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_SA;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_DA;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_ETHERTYPE;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT;
  	break;

    /*
     * L3 IPv4
     */
  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4:
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_HI;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_NEXT_PRTCL;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DF;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_MF;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SIP;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DIP;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SRC_PORT;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DEST_PORT;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TOS;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TCP_CTL;

    SOC_PB_PP_FP_CONTROL_INDEX_clear(&control_index);
    SOC_PB_PP_FP_CONTROL_INFO_clear(&control_info);
    control_index.type = SOC_PB_PP_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE;
    res = soc_pb_pp_fp_control_get_unsafe(
            unit,
            &control_index,
            &control_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (control_info.val[0] == 0)
    {
      qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT;
      qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_IN_VID;
    }
    else
    {
      qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_IRPP_IN_LIF;
    }
    break;

    /*
     * L3 IPv6
     */
  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6:
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_IPV6_L4OPS;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL;

    SOC_PB_PP_FP_CONTROL_INDEX_clear(&control_index);
    SOC_PB_PP_FP_CONTROL_INFO_clear(&control_info);
    control_index.type = SOC_PB_PP_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE;
    res = soc_pb_pp_fp_control_get_unsafe(
            unit,
            &control_index,
            &control_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (control_info.val[0] == 0)
    {
      qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT;
    }
    else
    {
      qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_IPV6_TCP_CTL;
    }

    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_IRPP_IN_LIF;
    break;
 
  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH:
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_ETHERTYPE;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_IRPP_ETH_TAG_FORMAT;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_2ND_VLAN_TAG;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_SA;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_DA;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA;
    break;

  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4:
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TOS;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_ERPP_IPV4_NEXT_PROTOCOL;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SIP;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DIP;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA;
    break;

  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS:
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_ERPP_FTMH;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_ERPP_PAYLOAD;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL_ID_FWD;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP_FWD;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL_FWD;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA;
    break;

  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_TM:
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_ERPP_FTMH;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_ERPP_PAYLOAD;
    qual_types[qual_type_ndx++] = SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA;
    break;

  default:
    break;
 }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_type_preset()", 0, 0);
}

uint32
  soc_pb_pp_fp_qual_val_encode(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO *qual_val_encode,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL             *qual_val
  )
{
  uint32
    mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    mac_add_long_valid[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    dest_buffer,
    asd_buffer,
    subnet_length,
    res;
  uint32
    ipv6_first_long,
    arr_ndx;
  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE
    appl_type;
  SOC_PETRA_HPU_FTMH_HDR
    ftmh_hdr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_QUAL_VAL_ENCODE);

  SOC_PB_PP_FP_QUAL_VAL_clear(qual_val);

  switch (qual_val_encode->type)
  {
  case SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_FWD_DECISION:
    qual_val->type = SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_DEST;

    /*
     * Transform to 17b encoding
     */
    if (qual_val_encode->val.fd.dest_nof_bits == 16)
    {
      appl_type = SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT; /* EM encoding*/
    }
    else
    {
      appl_type = SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_PMF;
    }
    res = soc_pb_pp_fwd_decision_in_buffer_build(
            appl_type,
            &(qual_val_encode->val.fd.fwd_dec),
            FALSE,
            &dest_buffer,
            &asd_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    qual_val->val.arr[0] = dest_buffer;
    qual_val->val.arr[1] = asd_buffer;
  	break;

  case SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_MAC_ADDRESS:
    qual_val->type = SOC_PB_PP_FP_QUAL_HDR_SA;
    /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    res = soc_sand_pp_mac_address_struct_to_long(
            &(qual_val_encode->val.mac.mac),
            mac_add_long
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    res = soc_sand_pp_mac_address_struct_to_long(
            &(qual_val_encode->val.mac.is_valid),
            mac_add_long_valid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for (arr_ndx = 0; arr_ndx < SOC_SAND_U64_NOF_UINT32S; ++arr_ndx)
    {
      qual_val->val.arr[arr_ndx] = mac_add_long[arr_ndx];
      qual_val->is_valid.arr[arr_ndx] = mac_add_long_valid[arr_ndx];
    }
  	break;

  case SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_IPV4_SUBNET:
    qual_val->type = SOC_PB_PP_FP_QUAL_HDR_IPV4_SIP;
    qual_val->val.arr[0] = qual_val_encode->val.ipv4.ip.address[0];
    subnet_length = qual_val_encode->val.ipv4.subnet_length;
    /*
     * In case of no subnet length, no valid bits
     */
    if (subnet_length != 0)
    {
      if (subnet_length > SOC_PB_PP_FP_SUBNET_LENGTH_IPV4_MAX)
      {
        subnet_length = SOC_PB_PP_FP_SUBNET_LENGTH_IPV4_MAX;
      }
      qual_val->is_valid.arr[0] = SOC_SAND_BITS_MASK(31, 31 - (subnet_length - 1));
    }
  	break;

  case SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_IPV6_SUBNET:
    qual_val->type = SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH;

    /*
     * Set Data
     */
    if (qual_val_encode->val.ipv6.is_low == TRUE)
    {
      ipv6_first_long = 0; /*Take bits 63:0*/
    }
    else
    {
      ipv6_first_long = 2; /*Take bits 127:64*/
    }
    for (arr_ndx = 0; arr_ndx < SOC_SAND_U64_NOF_UINT32S; ++arr_ndx)
    {
      qual_val->val.arr[arr_ndx] = qual_val_encode->val.ipv6.ip.address[arr_ndx + ipv6_first_long];
    }

    /*
     * Set mask
     */
    subnet_length = qual_val_encode->val.ipv6.subnet_length;

    /*
     * In case of no subnet length, no valid bits
     */
    if (subnet_length != 0)
    {
      if (subnet_length > SOC_PB_PP_FP_SUBNET_LENGTH_IPV6_MAX)
      {
        subnet_length = SOC_PB_PP_FP_SUBNET_LENGTH_IPV6_MAX;
      }
      if (subnet_length > 32)
      {
        qual_val->is_valid.arr[1] = SOC_SAND_BITS_MASK(31, 0);
        qual_val->is_valid.arr[0] = SOC_SAND_BITS_MASK(31, 31 - ((subnet_length - 32) - 1));
      }
      else
      {
        qual_val->is_valid.arr[1] = SOC_SAND_BITS_MASK(31, 31 - (subnet_length - 1));
      }
    }
  	break;

  case SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_ETH_TAG_FORMAT:
    qual_val->type = SOC_PB_PP_FP_QUAL_IRPP_ETH_TAG_FORMAT;
    qual_val->is_valid.arr[0] = SOC_SAND_BITS_MASK(4, 0);
    qual_val->val.arr[0] = (qual_val_encode->val.etf.tag_format.tag_outer << 3) +
      (qual_val_encode->val.etf.tag_format.is_priority << 2) +
      qual_val_encode->val.etf.tag_format.tag_inner;
    break;

  case SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_FTMH:
    qual_val->type = SOC_PB_PP_FP_QUAL_ERPP_FTMH;
    /* Verify the FTMH */
    res = soc_petra_hpu_ftmh_build_verify(&(qual_val_encode->val.ftmh.ftmh));
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    /*
     * Convert FTMH to long
     */
    soc_petra_PETRA_HPU_FTMH_HDR_clear(&ftmh_hdr);
    res = soc_petra_hpu_ftmh_build(
            &(qual_val_encode->val.ftmh.ftmh),
            &ftmh_hdr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

    qual_val->val.arr[0] = ftmh_hdr.extension_outlif; /* 15:0 */
    qual_val->val.arr[0] = qual_val->val.arr[0] +
      ((SOC_SAND_GET_BITS_RANGE(ftmh_hdr.base[0], 15, 0)) << 16);
    qual_val->val.arr[1] = SOC_SAND_GET_BITS_RANGE(ftmh_hdr.base[0], 31, 16);
    qual_val->val.arr[1] = qual_val->val.arr[1] +
      ((SOC_SAND_GET_BITS_RANGE(ftmh_hdr.base[1], 15, 0)) << 16);

    qual_val->is_valid.arr[0] = SOC_SAND_U32_MAX; /* All bits are valid */
    qual_val->is_valid.arr[1] = SOC_SAND_U32_MAX;
    break;

  default:
    break;
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_val_encode()", 0, 0);
}

/*********************************************************************
*     Get the Field Processing of the last packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_diag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PACKET_DIAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_fp_packet_diag_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_packet_diag_get()", 0, 0);
}

uint32
  soc_pb_pp_fp_qual_val_encode_fwd_decision(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO fwd_dec1,
    SOC_SAND_IN  uint32 dest_nof_bits1,
    SOC_SAND_OUT uint32 *qual_val_dest,
    SOC_SAND_OUT uint32 *qual_val_add
  )
{
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO
    qual_val_encode;
  SOC_PB_PP_FP_QUAL_VAL
    qual_val;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(qual_val_dest);
  SOC_SAND_CHECK_NULL_INPUT(qual_val_add);
  
  qual_val_encode.type = SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_FWD_DECISION;
  soc_sand_os_memcpy(&(qual_val_encode.val.fd.fwd_dec), &fwd_dec1, sizeof(SOC_PB_PP_FRWRD_DECISION_INFO));
  qual_val_encode.val.fd.dest_nof_bits = dest_nof_bits1;

  res = soc_pb_pp_fp_qual_val_encode(
          unit,
          &qual_val_encode,
          &qual_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *qual_val_dest = qual_val.val.arr[0];
  *qual_val_add = qual_val.val.arr[1];
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_val_encode_fwd_decision()", 0, 0);
}

uint32
  soc_pb_pp_fp_qual_val_encode_mac_address(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS is_valid1,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  )
{
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO
    qual_val_encode;

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  qual_val_encode.type = SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_MAC_ADDRESS;
  soc_sand_os_memcpy(&(qual_val_encode.val.mac.mac), &mac1, sizeof(SOC_SAND_PP_MAC_ADDRESS));
  soc_sand_os_memcpy(&(qual_val_encode.val.mac.is_valid), &is_valid1, sizeof(SOC_SAND_PP_MAC_ADDRESS));

  res = soc_pb_pp_fp_qual_val_encode(
          unit,
          &qual_val_encode,
          qual_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_val_encode_mac_address()", 0, 0);
}

uint32
  soc_pb_pp_fp_qual_val_encode_ipv4_subnet(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_ADDRESS ip1,
    SOC_SAND_IN  uint32 subnet_length1,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  )
{
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO
    qual_val_encode;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  qual_val_encode.type = SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_IPV4_SUBNET;
  soc_sand_os_memcpy(&(qual_val_encode.val.ipv4.ip), &ip1, sizeof(SOC_SAND_PP_IPV4_ADDRESS));
  qual_val_encode.val.ipv4.subnet_length = subnet_length1;

  res = soc_pb_pp_fp_qual_val_encode(
          unit,
          &qual_val_encode,
          qual_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_val_encode_ipv4_subnet()", 0, 0);
}

uint32
  soc_pb_pp_fp_qual_val_encode_ipv6_subnet(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS ip1,
    SOC_SAND_IN  uint32 subnet_length1,
    SOC_SAND_IN  uint8 is_low1,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  )
{
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO
    qual_val_encode;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  qual_val_encode.type = SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_IPV6_SUBNET;
  soc_sand_os_memcpy(&(qual_val_encode.val.ipv6.ip), &(ip1), sizeof(SOC_SAND_PP_IPV6_ADDRESS));
  qual_val_encode.val.ipv6.subnet_length = subnet_length1;
  qual_val_encode.val.ipv6.is_low = is_low1;

  res = soc_pb_pp_fp_qual_val_encode(
          unit,
          &qual_val_encode,
          qual_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_val_encode_ipv6_subnet()", 0, 0);
}

uint32
  soc_pb_pp_fp_qual_val_encode_eth_tag_format(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  SOC_PB_PP_FP_ETH_TAG_FORMAT tag_format,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  )
{
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO
    qual_val_encode;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  qual_val_encode.type = SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_ETH_TAG_FORMAT;
  soc_sand_os_memcpy(&(qual_val_encode.val.etf.tag_format), &tag_format, sizeof(SOC_PB_PP_FP_ETH_TAG_FORMAT));
  
  res = soc_pb_pp_fp_qual_val_encode(
          unit,
          &qual_val_encode,
          qual_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_val_encode_eth_tag_format()", 0, 0);
}

uint32
  soc_pb_pp_fp_qual_val_encode_ftmh(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FTMH ftmh1,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  )
{
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO
    qual_val_encode;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  qual_val_encode.type = SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_FTMH;
  qual_val_encode.val.ftmh.ftmh = ftmh1;

  res = soc_pb_pp_fp_qual_val_encode(
          unit,
          &qual_val_encode,
          qual_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_val_encode_ftmh()", 0, 0);
}

uint32
  soc_pb_pp_fp_egress_action_type_preset(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT SOC_PB_PP_FP_ACTION_TYPE *action_types
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  action_types[0] = SOC_PB_PP_FP_ACTION_TYPE_EGR_TRAP;
  action_types[1] = SOC_PB_PP_FP_ACTION_TYPE_EGR_OFP;
  action_types[2] = SOC_PB_PP_FP_ACTION_TYPE_EGR_TC_DP;
  action_types[3] = SOC_PB_PP_FP_ACTION_TYPE_EGR_OUTLIF;

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egress_action_type_preset()", 0, 0);
}

void
  SOC_PB_PP_FP_QUAL_VAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_QUAL_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_ETH_TAG_FORMAT_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_ETH_TAG_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ETH_TAG_FORMAT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PFG_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PFG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PFG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_DATABASE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DATABASE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ACTION_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ACTION_LOC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ACTION_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_CONTROL_INDEX_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_CONTROL_INDEX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_CONTROL_INDEX_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_CONTROL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_CONTROL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_CONTROL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PB_PP_FP_DIAG_PP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DIAG_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIAG_PP_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_PARSER_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_PARSER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_PARSER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_PGM_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_PGM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_PGM_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_QUAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_QUAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_DB_QUAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_DB_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_DB_QUAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_TCAM_DT_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_TCAM_DT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_TCAM_DT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_MACRO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_MACRO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_MACRO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_FTMH_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_clear(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_FP_QUAL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE enum_val
  )
{
  return SOC_PPC_FP_QUAL_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_PKT_HDR_TYPE enum_val
  )
{
  return SOC_TMC_FP_PKT_HDR_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FP_FWD_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE enum_val
  )
{
  return SOC_PPC_FP_FWD_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FP_PROCESSING_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_PROCESSING_TYPE enum_val
  )
{
  return SOC_PPC_FP_PROCESSING_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FP_BASE_HEADER_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_BASE_HEADER_TYPE enum_val
  )
{
  return SOC_PPC_FP_BASE_HEADER_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FP_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE enum_val
  )
{
  return SOC_PPC_FP_ACTION_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FP_DATABASE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_TYPE enum_val
  )
{
  return SOC_PPC_FP_DATABASE_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_PREDEFINED_ACL_KEY enum_val
  )
{
  return SOC_PPC_FP_PREDEFINED_ACL_KEY_to_string(enum_val);
}

const char*
  SOC_PB_PP_FP_CONTROL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_TYPE enum_val
  )
{
  return SOC_PPC_FP_CONTROL_TYPE_to_string(enum_val);
}

void
  SOC_PB_PP_FP_QUAL_VAL_print(
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_QUAL_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PFG_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PFG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_DATABASE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DATABASE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ACTION_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ACTION_LOC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ACTION_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_CONTROL_INDEX_print(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_CONTROL_INDEX_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_CONTROL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_CONTROL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PB_PP_FP_DIAG_PP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DIAG_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIAG_PP_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_PARSER_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_PARSER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_PARSER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_PGM_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_PGM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_PGM_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_QUAL_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_QUAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_DB_QUAL_print(
    SOC_SAND_IN  uint32                  ind,
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_DB_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_DB_QUAL_print(ind, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_TCAM_DT_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_TCAM_DT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_TCAM_DT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_MACRO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_MACRO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_MACRO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_PACKET_DIAG_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

