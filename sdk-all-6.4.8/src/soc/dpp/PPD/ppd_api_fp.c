/* $Id: ppd_api_fp.c,v 1.15 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_fp.c
*
* MODULE PREFIX:  ppd
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD

#include <shared/bsl.h>
#include <soc/dpp/drv.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_fp.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_fp.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_fp.h>
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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Ppd_procedure_desc_element_fp[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_PACKET_FORMAT_GROUP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_PACKET_FORMAT_GROUP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_PACKET_FORMAT_GROUP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_PACKET_FORMAT_GROUP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DATABASE_CREATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DATABASE_CREATE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DATABASE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DATABASE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DATABASE_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DATABASE_DESTROY_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DATABASE_ENTRIES_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DATABASE_ENTRIES_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_CONTROL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_CONTROL_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_CONTROL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_CONTROL_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_EGR_DB_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_EGR_DB_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_EGR_DB_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_EGR_DB_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_PACKET_DIAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_PACKET_DIAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_RESOURCE_DIAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_RESOURCE_DIAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FP_GET_PROCS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};


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
  soc_ppd_fp_packet_format_group_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx,
    SOC_SAND_IN  SOC_PPD_FP_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_PACKET_FORMAT_GROUP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_packet_format_group_set,(unit, pfg_ndx, info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_packet_format_group_set_print,(unit,pfg_ndx,info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_packet_format_group_set()", pfg_ndx, 0);
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
  soc_ppd_fp_packet_format_group_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx,
    SOC_SAND_OUT SOC_PPD_FP_PFG_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_PACKET_FORMAT_GROUP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_packet_format_group_get,(unit, pfg_ndx, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_packet_format_group_get_print,(unit,pfg_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_packet_format_group_get()", pfg_ndx, 0);
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
  soc_ppd_fp_database_create(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_DATABASE_CREATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_database_create,(unit, db_id_ndx, info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_database_create_print,(unit,db_id_ndx,info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_create()", db_id_ndx, 0);
}

/*********************************************************************
*     Get the database parameters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_database_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_OUT SOC_PPD_FP_DATABASE_INFO                    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_DATABASE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_database_get,(unit, db_id_ndx, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_database_get_print,(unit,db_id_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_get()", db_id_ndx, 0);
}

/*********************************************************************
*     Destroy the database: all its entries are suppressed and
 *     the Database-ID is freed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_database_destroy(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_DATABASE_DESTROY);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_database_destroy,(unit, db_id_ndx));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_database_destroy_print,(unit,db_id_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_destroy()", db_id_ndx, 0);
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
  soc_ppd_fp_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_ENTRY_INFO                       *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_entry_add,(unit, db_id_ndx, entry_id_ndx, info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_entry_add_print,(unit,db_id_ndx,entry_id_ndx,info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_entry_add()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_OUT uint8                                *is_found,
    SOC_SAND_INOUT SOC_PPD_FP_ENTRY_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_found);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_entry_get,(unit, db_id_ndx, entry_id_ndx, is_found, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_entry_get_print,(unit,db_id_ndx,entry_id_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_entry_get()", db_id_ndx, entry_id_ndx);
}


uint32
  soc_ppd_fp_entry_remove_by_key(
    SOC_SAND_IN    int                               unit,
    SOC_SAND_IN    uint32                               db_id_ndx,
    SOC_SAND_INOUT SOC_PPD_FP_ENTRY_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);  

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(fp_entry_remove_by_key,(unit, db_id_ndx, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_entry_add()", db_id_ndx, 0);
}

/*********************************************************************
*     Remove an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  uint32                               is_sw_remove_only
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_entry_remove,(unit, db_id_ndx, entry_id_ndx, is_sw_remove_only));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_entry_remove_print,(unit,db_id_ndx,entry_id_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_entry_remove()", db_id_ndx, entry_id_ndx);
}


/*********************************************************************
*     Get the Database entries. The function returns list of
 *     entries that were added to a database with database ID
 *     'db_id_ndx'.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_database_entries_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPD_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_DATABASE_ENTRIES_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_database_entries_get_block,(unit, db_id_ndx, block_range, entries, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_database_entries_get_block_print,(unit,db_id_ndx,block_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_entries_get_block()", db_id_ndx, 0);
}

/*********************************************************************
*     Add an entry to the Database. The database entry is
 *     selected if all the Packet Qualifier field values are in
 *     the Database entry range.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_direct_extraction_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_direct_extraction_entry_add,(unit, db_id_ndx, entry_id_ndx, info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_direct_extraction_entry_add_print,(unit,db_id_ndx,entry_id_ndx,info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_direct_extraction_entry_add()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_direct_extraction_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_OUT uint8                               *is_found,
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_found);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_direct_extraction_entry_get,(unit, db_id_ndx, entry_id_ndx, is_found, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_direct_extraction_entry_get_print,(unit,db_id_ndx,entry_id_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_direct_extraction_entry_get()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Remove an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_direct_extraction_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_direct_extraction_entry_remove,(unit, db_id_ndx, entry_id_ndx));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_direct_extraction_entry_remove_print,(unit,db_id_ndx,entry_id_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_direct_extraction_entry_remove()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get the Database entries.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_direct_extraction_db_entries_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *entries,
    SOC_SAND_OUT uint32                                *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_direct_extraction_db_entries_get,(unit, db_id_ndx, block_range, entries, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_direct_extraction_db_entries_get_print,(unit,db_id_ndx,block_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_direct_extraction_db_entries_get()", db_id_ndx, 0);
}

/*********************************************************************
*     Set one of the control options.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_control_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  core_id,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_CONTROL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(control_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_control_set,(unit, core_id, control_ndx, info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_control_set_print,(unit,control_ndx,info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_control_set()", 0, 0);
}

/*********************************************************************
*     Set one of the control options.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_control_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  core_id,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_OUT SOC_PPD_FP_CONTROL_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_CONTROL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(control_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_control_get,(unit, core_id, control_ndx, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_control_get_print,(unit,control_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_control_get()", 0, 0);
}

/*********************************************************************
*     Set the mapping between the Packet forward type and the
 *     Port profile to the Database-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_egr_db_map_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx,
    SOC_SAND_IN  uint32                   db_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_EGR_DB_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_egr_db_map_set,(unit, fwd_type_ndx, port_profile_ndx, db_id));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_egr_db_map_set_print,(unit,fwd_type_ndx,port_profile_ndx,db_id));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_egr_db_map_set()", 0, port_profile_ndx);
}

/*********************************************************************
*     Set the mapping between the Packet forward type and the
 *     Port profile to the Database-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_egr_db_map_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx,
    SOC_SAND_OUT uint32                   *db_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_EGR_DB_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(db_id);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_egr_db_map_get,(unit, fwd_type_ndx, port_profile_ndx, db_id));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_egr_db_map_get_print,(unit,fwd_type_ndx,port_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_egr_db_map_get()", 0, port_profile_ndx);
}

/*********************************************************************
*     Compress a TCAM Database: compress the entries to minimum
*     number of banks.
*********************************************************************/
uint32
  soc_ppd_fp_database_compress(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(fp_database_compress,(unit, db_id_ndx));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_database_compress,(unit,db_id_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_compress()", 0, db_id_ndx);
}

/*********************************************************************
*     Get the Field Processing of the last packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_fp_packet_diag_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core_id,
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_PACKET_DIAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(fp_packet_diag_get,(unit, core_id, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_fp_packet_diag_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_packet_diag_get()", 0, 0);
}


uint32
  soc_ppd_fp_resource_diag_get(
    SOC_SAND_IN 	int               		unit,
	SOC_SAND_IN	 	SOC_PPD_FP_RESOURCE_DIAG_MODE	mode,
    SOC_SAND_OUT 	SOC_PPD_FP_RESOURCE_DIAG_INFO	*info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FP_RESOURCE_DIAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(fp_resource_diag_get,(unit, mode, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_resource_diag_get()", 0, 0);
}


/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_fp module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_fp_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_fp;
}
void
  SOC_PPD_FP_QUAL_VAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_QUAL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_QUAL_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_ETH_TAG_FORMAT_clear(
    SOC_SAND_OUT SOC_PPD_FP_ETH_TAG_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ETH_TAG_FORMAT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PFG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_PFG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PFG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_DATABASE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_DATABASE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DATABASE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ACTION_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_DIR_EXTR_ACTION_LOC_clear(
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ACTION_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ACTION_LOC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_DIR_EXTR_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ACTION_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_CONTROL_INDEX_clear(
    SOC_SAND_OUT SOC_PPD_FP_CONTROL_INDEX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_CONTROL_INDEX_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_CONTROL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_CONTROL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_CONTROL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPD_FP_DIAG_PP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_DIAG_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIAG_PP_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_PARSER_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_PARSER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_PARSER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_PGM_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_PGM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_PGM_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_QUAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_QUAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_DB_QUAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_DB_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_DB_QUAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_TCAM_DT_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_TCAM_DT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_TCAM_DT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_MACRO_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_MACRO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_MACRO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_KEY_LSB_MSB_clear(
	  SOC_SAND_OUT SOC_PPD_FP_RESOURCE_KEY_LSB_MSB *info
	)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_KEY_LSB_MSB_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DB_BANK_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DB_BANK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DB_BANK_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_ACTION_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_ACTION_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DB_TCAM_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DB_TCAM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DB_TCAM_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DB_DE_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DB_DE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DB_DE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DB_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DB_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_BANK_DB_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_BANK_DB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_BANK_DB_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_BANK_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_BANK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_BANK_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_PRESEL_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_PRESEL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_PRESEL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DIAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DIAG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_FP_QUAL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_QUAL_TYPE enum_val
  )
{
  return SOC_PPC_FP_QUAL_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_PKT_HDR_TYPE enum_val
  )
{
  return SOC_TMC_FP_PKT_HDR_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FP_FWD_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE enum_val
  )
{
  return SOC_PPC_FP_FWD_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FP_PROCESSING_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_PROCESSING_TYPE enum_val
  )
{
  return SOC_PPC_FP_PROCESSING_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FP_BASE_HEADER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_BASE_HEADER_TYPE enum_val
  )
{
  return SOC_PPC_FP_BASE_HEADER_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FP_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_ACTION_TYPE enum_val
  )
{
  return SOC_PPC_FP_ACTION_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FP_DATABASE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_DATABASE_TYPE enum_val
  )
{
  return SOC_PPC_FP_DATABASE_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FP_PREDEFINED_ACL_KEY_to_string(
    SOC_SAND_IN  SOC_PPD_FP_PREDEFINED_ACL_KEY enum_val
  )
{
  return SOC_PPC_FP_PREDEFINED_ACL_KEY_to_string(enum_val);
}

const char*
  SOC_PPD_FP_CONTROL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_TYPE enum_val
  )
{
  return SOC_PPC_FP_CONTROL_TYPE_to_string(enum_val);
}

void
  SOC_PPD_FP_QUAL_VAL_print(
    SOC_SAND_IN  SOC_PPD_FP_QUAL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_QUAL_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PFG_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_PFG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PFG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_DATABASE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_DATABASE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DATABASE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PPD_FP_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ACTION_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_DIR_EXTR_ACTION_LOC_print(
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ACTION_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ACTION_LOC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_DIR_EXTR_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ACTION_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_CONTROL_INDEX_print(
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_CONTROL_INDEX_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_CONTROL_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_CONTROL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPD_FP_DIAG_PP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_DIAG_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_DIAG_PP_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_PARSER_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_PARSER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_PARSER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_PGM_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_PGM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_PGM_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_QUAL_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_QUAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_DB_QUAL_print(
  SOC_SAND_IN  uint32                  ind,
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_DB_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_DB_QUAL_print(ind, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_KEY_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_TCAM_DT_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_TCAM_DT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_TCAM_DT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_MACRO_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_MACRO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_MACRO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_PACKET_DIAG_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_PACKET_DIAG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_KEY_LSB_MSB_print(
	  SOC_SAND_IN	SOC_PPC_FP_KEY_BIT_TYPE_LSB_MSB lsb_msb,
	  SOC_SAND_IN SOC_PPD_FP_RESOURCE_KEY_LSB_MSB *info
	)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_KEY_LSB_MSB_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_KEY_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DB_BANK_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_DB_BANK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DB_BANK_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_ACTION_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_ACTION_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DB_TCAM_print(
    SOC_SAND_IN int                         unit,
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_DB_TCAM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DB_TCAM_print(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DB_DE_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_DB_DE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DB_DE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DB_print(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_DB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DB_print(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_BANK_DB_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_BANK_DB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_BANK_DB_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_BANK_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_BANK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_BANK_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_PRESEL_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_PRESEL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_PRESEL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FP_RESOURCE_DIAG_INFO_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FP_RESOURCE_DIAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_RESOURCE_DIAG_INFO_print(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_fp_packet_format_group_set_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx,
    SOC_SAND_IN  SOC_PPD_FP_PFG_INFO            *info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pfg_ndx: %u\n\r"),pfg_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "info:")));
  SOC_PPD_FP_PFG_INFO_print((info));

  return;
}
void
  soc_ppd_fp_packet_format_group_get_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pfg_ndx: %u\n\r"),pfg_ndx));

  return;
}
void
  soc_ppd_fp_database_create_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_DATABASE_INFO                    *info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "info:")));
  SOC_PPD_FP_DATABASE_INFO_print((info));

  return;
}
void
  soc_ppd_fp_database_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  return;
}
void
  soc_ppd_fp_database_destroy_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  return;
}
void
  soc_ppd_fp_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_ENTRY_INFO                       *info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "entry_id_ndx: %u\n\r"),entry_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "info:")));
  SOC_PPD_FP_ENTRY_INFO_print((info));

  return;
}
void
  soc_ppd_fp_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "entry_id_ndx: %u\n\r"),entry_id_ndx));

  return;
}
void
  soc_ppd_fp_entry_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "entry_id_ndx: %u\n\r"),entry_id_ndx));

  return;
}
void
  soc_ppd_fp_database_entries_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "block_range:")));
  soc_sand_SAND_TABLE_BLOCK_RANGE_print((block_range));

  return;
}
void
  soc_ppd_fp_direct_extraction_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "entry_id_ndx: %u\n\r"),entry_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "info:")));
  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO_print((info));

  return;
}
void
  soc_ppd_fp_direct_extraction_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "entry_id_ndx: %u\n\r"),entry_id_ndx));

  return;
}
void
  soc_ppd_fp_direct_extraction_entry_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "entry_id_ndx: %u\n\r"),entry_id_ndx));

  return;
}
void
  soc_ppd_fp_direct_extraction_db_entries_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_id_ndx: %u\n\r"),db_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "block_range:")));
  soc_sand_SAND_TABLE_BLOCK_RANGE_print((block_range));

  return;
}
void
  soc_ppd_fp_control_set_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX      *control_ndx,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INFO        *info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "control_ndx:")));
  SOC_PPD_FP_CONTROL_INDEX_print((control_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "info:")));
  SOC_PPD_FP_CONTROL_INFO_print((info));

  return;
}
void
  soc_ppd_fp_control_get_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX      *control_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "control_ndx:")));
  SOC_PPD_FP_CONTROL_INDEX_print((control_ndx));

  return;
}
void
  soc_ppd_fp_egr_db_map_set_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx,
    SOC_SAND_IN  uint32                   db_id
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fwd_type_ndx %s "), SOC_PPD_FP_FWD_TYPE_to_string(fwd_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %u\n\r"),port_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "db_id: %lu\n\r"),db_id));

  return;
}
void
  soc_ppd_fp_egr_db_map_get_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fwd_type_ndx %s "), SOC_PPD_FP_FWD_TYPE_to_string(fwd_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %u\n\r"),port_profile_ndx));

  return;
}
void
  soc_ppd_fp_packet_diag_get_print(
    SOC_SAND_IN  int               unit
  )
{

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

