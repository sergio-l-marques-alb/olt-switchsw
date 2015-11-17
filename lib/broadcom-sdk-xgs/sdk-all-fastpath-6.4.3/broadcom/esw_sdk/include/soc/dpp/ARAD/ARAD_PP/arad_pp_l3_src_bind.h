/* $Id: arad_pp_l3_src_bind.h,v 1.4 Broadcom SDK $
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

#ifndef __ARAD_PP_L3_SRC_BIND_INCLUDED__
/* { */
#define __ARAD_PP_L3_SRC_BIND_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_l3_src_bind.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_SRC_BIND_IPV4_ADD = ARAD_PP_PROC_DESC_BASE_SRC_BIND_FIRST,
  ARAD_PP_SRC_BIND_IPV4_ADD_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_ADD_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_ADD_VERIFY,
  ARAD_PP_SRC_BIND_IPV4_GET,
  ARAD_PP_SRC_BIND_IPV4_GET_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_GET_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_GET_VERIFY,
  ARAD_PP_SRC_BIND_IPV4_REMOVE,
  ARAD_PP_SRC_BIND_IPV4_REMOVE_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_REMOVE_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_REMOVE_VERIFY,
  ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR,
  ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR_VERIFY,
  ARAD_PP_SRC_BIND_IPV6_ADD,
  ARAD_PP_SRC_BIND_IPV6_ADD_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_ADD_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_ADD_VERIFY,
  ARAD_PP_SRC_BIND_IPV6_GET,
  ARAD_PP_SRC_BIND_IPV6_GET_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_GET_INTERNAL_UNSAFE,  
  ARAD_PP_SRC_BIND_IPV6_GET_VERIFY,
  ARAD_PP_SRC_BIND_IPV6_REMOVE,
  ARAD_PP_SRC_BIND_IPV6_REMOVE_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_REMOVE_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_REMOVE_VERIFY,
  ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR,
  ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR_VERIFY, 
  ARAD_PP_SRC_BIND_IPV6_STATIC_TCAM_INIT_UNSAFE,
  ARAD_PP_SRC_BIND_GET_PROCS_PTR,
  ARAD_PP_SRC_BIND_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_SRC_BIND_PROCEDURE_DESC_LAST
} ARAD_PP_SRC_BIND_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_SRC_BIND_INVALID_SMAC_ERR = ARAD_PP_ERR_DESC_BASE_SRC_BIND_FIRST,
  ARAD_PP_SRC_BIND_IPV4_INVALID_SIP_ERR,
  ARAD_PP_SRC_BIND_IPV6_INVALID_SIP_ERR,
  ARAD_PP_SRC_BIND_INFO_IS_NULL_ERR,
  ARAD_PP_SRC_BIND_SW_DB_NO_RES_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */


  /*
   * Last element. Do no touch.
   */
  ARAD_PP_SRC_BIND_ERR_LAST
} ARAD_PP_SRC_BIND_ERR;

#define ARAD_PP_SRC_BIND_IPV4_MAX_SPOOF_ID 4096


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
  arad_pp_src_bind_ipv6_static_tcam_init_unsafe(
    SOC_SAND_IN int unit
  );

uint32
  arad_pp_src_bind_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32 
  arad_pp_src_bind_ipv4_add_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY           *src_bind_info
    );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv4_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an L3 source binding ipv4 entry. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY                  *src_bind_info -
 *     Describe which entry is added.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If True, then the entry was inserted in the L3 source bind table
 *     correctly. Otherwise, an hardware error happened.
 * REMARKS:
 *   - None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
  arad_pp_src_bind_ipv4_add_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32 
  arad_pp_src_bind_ipv4_add_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_IN  uint8                                  ignore_key,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32 
  arad_pp_src_bind_ipv4_get_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY           *src_bind_info
    );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv4_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an ipv4 entry according to its key. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY                  *src_bind_info -
 *     Describe which entry is found.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If TRUE, then the key exists in l3 source bind table. Otherwise,
 *     hasn't this entry in the table..
 * REMARKS:
 *   - None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
  arad_pp_src_bind_ipv4_get_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_OUT uint8                                 *found
    );

uint32 
  arad_pp_src_bind_ipv4_get_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_IN  uint8                                  ignore_key,
      SOC_SAND_OUT uint8                                 *found,
      SOC_SAND_OUT uint8                                 *found_isem,
      SOC_SAND_OUT uint8                                 *found_lem,
      SOC_SAND_OUT uint16                                *spoof_id
  );

uint32
  arad_pp_src_bind_ipv4_remove_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY           *src_bind_info
    );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv4_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an ipv4 entry from the l3 source bind table according to the
 *   given Key. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY                  *src_bind_info -
 *     Describe which entry is found.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If TRUE, remove the entry from table. Otherwise,
 *     there is a resouce error, e.g. the entry isn't existed.
 * REMARKS:
 *   - None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_src_bind_ipv4_remove_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32
  arad_pp_src_bind_ipv4_remove_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32
  arad_pp_src_bind_ipv4_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv4_table_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   clear all ipv4 entry inthe l3 source bind table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   - None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_src_bind_ipv4_table_clear_unsafe(
      SOC_SAND_IN  int                                 unit
    );

uint32
  arad_pp_src_bind_ipv4_table_clear_internal_unsafe(
      SOC_SAND_IN  int                                 unit
    );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv4_host_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the ipv4 host table.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key -
 *     Defines the range of routing entries to retrieve.
 *   SOC_SAND_OUT ARAD_PP_SRC_BIND_IPV4_ENTRY             *ipv4_info -
 *     array of ipv4 source bind host keys
 *   SOC_SAND_OUT ARAD_PP_FRWRD_IP_ROUTE_STATUS           *ipv4_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                              *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - The lookup is performed in the SW shadow.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
    arad_pp_src_bind_ipv4_host_get_block_unsafe(
      SOC_SAND_IN    int                                        unit,
      SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key,
      SOC_SAND_OUT   ARAD_PP_SRC_BIND_IPV4_ENTRY               *ipv4_info,
      SOC_SAND_OUT   ARAD_PP_FRWRD_IP_ROUTE_STATUS             *ipv4_status,
      SOC_SAND_OUT   uint32                                    *nof_entries
  );

uint32 
  arad_pp_src_bind_ipv6_add_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY           *src_bind_info
    );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv6_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an L3 source binding ipv6 entry. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY                  *src_bind_info -
 *     Describe which entry is added.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If True, then the entry was inserted in the L3 source bind table
 *     correctly. Otherwise, an hardware error happened.
 * REMARKS:
 *   - None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
  arad_pp_src_bind_ipv6_add_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32 
  arad_pp_src_bind_ipv6_add_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_IN  uint8                                  ignore_key,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32 
  arad_pp_src_bind_ipv6_get_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY           *src_bind_info
    );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv6_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an ipv6 entry according to its key. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY                  *src_bind_info -
 *     Describe which entry is found.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If TRUE, then the key exists in l3 source bind table. Otherwise,
 *     hasn't this entry in the table..
 * REMARKS:
 *   - None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
  arad_pp_src_bind_ipv6_get_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_OUT uint8                                 *found
    );

uint32 
  arad_pp_src_bind_ipv6_get_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_IN  uint8                                  ignore_key,
      SOC_SAND_OUT uint8                                 *found,
      SOC_SAND_OUT uint8                                 *found_isem,
      SOC_SAND_OUT uint8                                 *found_lem,
      SOC_SAND_OUT uint16                                *spoof_id
    );

uint32
  arad_pp_src_bind_ipv6_remove_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY           *src_bind_info
    );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv6_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an ipv6 entry from the l3 source bind table according to the
 *   given Key. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY                  *src_bind_info -
 *     Describe which entry is found.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If TRUE, remove the entry from table. Otherwise,
 *     there is a resouce error, e.g. the entry isn't existed.
 * REMARKS:
 *   - None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_src_bind_ipv6_remove_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32
  arad_pp_src_bind_ipv6_remove_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32
  arad_pp_src_bind_ipv6_table_clear_verify(
      SOC_SAND_IN  int                                 unit
    );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv6_table_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   clear all ipv6 entry inthe l3 source bind table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   - None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_src_bind_ipv6_table_clear_unsafe(
      SOC_SAND_IN  int                                 unit
    );

uint32
  arad_pp_src_bind_ipv6_table_clear_internal_unsafe(
      SOC_SAND_IN  int                                 unit
    );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_ipv6_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the ipv6 host and subnet table.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key -
 *     Defines the range of routing entries to retrieve.
 *   SOC_SAND_OUT ARAD_PP_SRC_BIND_IPV6_ENTRY             *src_bind_ipv6 -
 *     array of ipv4 source bind host keys
 *   SOC_SAND_OUT ARAD_PP_FRWRD_IP_ROUTE_STATUS           *ipv6_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                              *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - The lookup is performed in the SW shadow.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_src_bind_ipv6_get_block_unsafe(
    SOC_SAND_IN    int                                 unit,
    SOC_SAND_INOUT ARAD_PP_IP_ROUTING_TABLE_RANGE     *block_range_key,
    SOC_SAND_OUT   ARAD_PP_SRC_BIND_IPV6_ENTRY        *src_bind_ipv6,
    SOC_SAND_OUT   ARAD_PP_FRWRD_IP_ROUTE_STATUS      *ipv6_status,
    SOC_SAND_OUT   uint32                             *nof_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_lif module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_src_bind_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_src_bind_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_lif module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_src_bind_get_errs_ptr(void);

uint32
  ARAD_PP_SRC_BIND_IPV4_ENTRY_verify(
    SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY *info
  );

uint32
  ARAD_PP_SRC_BIND_IPV6_ENTRY_verify(
    SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_L3_SRC_BIND_INCLUDED__*/
#endif



