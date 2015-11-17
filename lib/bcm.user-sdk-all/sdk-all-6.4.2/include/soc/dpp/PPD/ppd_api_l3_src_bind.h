/******************************************************************
*
* $Id: ppd_api_l3_src_bind.h,v 1.4 Broadcom SDK $
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
*
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_l3_src_bind.h
*
* MODULE PREFIX:  soc_ppd_src_bind
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

#ifndef __SOC_PPD_API_L3_SRC_BIND_INCLUDED__
/* { */
#define __SOC_PPD_API_L3_SRC_BIND_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_l3_src_bind.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>

#include <soc/dpp/PPD/ppd_api_general.h>

/* } */
/*************
 * DEFINES   *
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
  SOC_PPD_SRC_BIND_IPV4_ADD = SOC_PPD_PROC_DESC_BASE_SRC_BIND_FIRST,
  SOC_PPD_SRC_BIND_IPV4_ADD_PRINT,
  SOC_PPD_SRC_BIND_IPV4_GET,
  SOC_PPD_SRC_BIND_IPV4_GET_PRINT,
  SOC_PPD_SRC_BIND_IPV4_REMOVE,
  SOC_PPD_SRC_BIND_IPV4_REMOVE_PRINT,
  SOC_PPD_SRC_BIND_IPV4_TABLE_CLEAR,
  SOC_PPD_SRC_BIND_IPV4_TABLE_CLEAR_PRINT,
  SOC_PPD_SRC_BIND_IPV6_ADD,
  SOC_PPD_SRC_BIND_IPV6_ADD_PRINT,
  SOC_PPD_SRC_BIND_IPV6_GET,
  SOC_PPD_SRC_BIND_IPV6_GET_PRINT,
  SOC_PPD_SRC_BIND_IPV6_REMOVE,
  SOC_PPD_SRC_BIND_IPV6_REMOVE_PRINT,
  SOC_PPD_SRC_BIND_IPV6_TABLE_CLEAR,
  SOC_PPD_SRC_BIND_IPV6_TABLE_CLEAR_PRINT,
  SOC_PPD_SRC_BIND_GET_PROCS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  /*
   * Last element. Do no touch.
   */
  SOC_PPD_SRC_BIND_PROCEDURE_DESC_LAST
} SOC_PPD_SRC_BIND_PROCEDURE_DESC;

typedef SOC_PPC_SRC_BIND_IPV4_ENTRY                  SOC_PPD_SRC_BIND_IPV4_ENTRY;

typedef SOC_PPC_SRC_BIND_IPV6_ENTRY                  SOC_PPD_SRC_BIND_IPV6_ENTRY;

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
* NAME:
 *   soc_ppd_src_bind_ipv4_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an L3 source binding ipv4 entry. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY                  *src_bind_info -
 *     Describe which entry is added.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If True, then the entry was inserted in the L3 source bind table
 *     correctly. Otherwise, an hardware error happened.
 * REMARKS:
 *   - Only support by ARAD.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_ipv4_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an ipv4 entry according to its key. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY                  *src_bind_info -
 *     Describe which entry is found.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If TRUE, then the key exists in l3 source bind table. Otherwise,
 *     hasn't this entry in the table..
 * REMARKS:
 *   - Only support by ARAD.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_ipv4_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an ipv4 entry from the l3 source bind table according to the
 *   given Key. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY                  *src_bind_info -
 *     Describe which entry is found.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If TRUE, remove the entry from table. Otherwise,
 *     there is a resouce error, e.g. the entry isn't existed.
 * REMARKS:
 *   - Only support by ARAD.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_ipv4_table_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *   clear all ipv4 entry inthe l3 source bind table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   - Only support by ARAD.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_table_clear(
    SOC_SAND_IN  int                           unit
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_ipv4_host_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the ipv4 host table.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key -
 *     Defines the range of routing entries to retrieve.
 *   SOC_SAND_OUT SOC_PPD_SRC_BIND_IPV4_ENTRY             *ipv4_info -
 *     array of ipv4 source bind host keys
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                              *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - The lookup is performed in the HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_host_get_block(
    SOC_SAND_IN    int                              unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE      *block_range,
    SOC_SAND_OUT   SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT   SOC_PPD_FRWRD_IP_ROUTE_STATUS   *routes_status,
    SOC_SAND_OUT   uint32                          *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_ipv6_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an L3 source binding ipv6 entry. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY                  *src_bind_info -
 *     Describe which entry is added.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If True, then the entry was inserted in the L3 source bind table
 *     correctly. Otherwise, an hardware error happened.
 * REMARKS:
 *   - Only support by ARAD.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv6_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_ipv6_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an ipv6 entry according to its key. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY                  *src_bind_info -
 *     Describe which entry is found.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If TRUE, then the key exists in l3 source bind table. Otherwise,
 *     hasn't this entry in the table..
 * REMARKS:
 *   - Only support by ARAD.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv6_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_ipv6_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an ipv6 entry from the l3 source bind table according to the
 *   given Key. Given a key, e.g. (lif_ndx)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY                  *src_bind_info -
 *     Describe which entry is found.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                     *success -
 *     If TRUE, remove the entry from table. Otherwise,
 *     there is a resouce error, e.g. the entry isn't existed.
 * REMARKS:
 *   - Only support by ARAD.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv6_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_ipv6_table_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *   clear all ipv6 entry inthe l3 source bind table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   - Only support by ARAD.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv6_table_clear(
    SOC_SAND_IN  int                           unit
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_ipv6_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the ipv6 host and subnet table.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE                *block_range_key -
 *     Defines the range of routing entries to retrieve.
 *   SOC_SAND_OUT SOC_PPD_SRC_BIND_IPV6_ENTRY             *src_bind_ipv6 -
 *     array of ipv4 source bind host keys
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *routes_status -
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
  soc_ppd_src_bind_ipv6_get_block(
    SOC_SAND_IN    int                              unit,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE  *block_range,
    SOC_SAND_OUT   SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT   SOC_PPD_FRWRD_IP_ROUTE_STATUS   *routes_status,
    SOC_SAND_OUT   uint32                          *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_src_bind_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_src_bind module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_src_bind_get_procs_ptr(void);

void
  SOC_PPD_SRC_BIND_IPV4_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info
  );

void
  SOC_PPD_SRC_BIND_IPV6_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info
  );


#if SOC_PPD_DEBUG_IS_LVL3
void
  SOC_PPD_SRC_BIND_IPV4_INFO_print(
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY *src_bind_info
  );

void
  SOC_PPD_SRC_BIND_IPV6_INFO_print(
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY *src_bind_info
  );

void
  soc_ppd_src_bind_ipv4_add_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info
  );

void
  soc_ppd_src_bind_ipv4_get_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info
  );

void
  soc_ppd_src_bind_ipv4_remove_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info
  );

void
  soc_ppd_src_bind_ipv4_table_clear_print(
    SOC_SAND_IN  int                           unit
  );

void
  soc_ppd_src_bind_ipv6_add_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info
  );

void
  soc_ppd_src_bind_ipv6_get_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info
  );

void
  soc_ppd_src_bind_ipv6_remove_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info
  );

void
  soc_ppd_frwrd_get_block_print(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE   *block_range_key
  );

void
  soc_ppd_src_bind_ipv6_table_clear_print(
    SOC_SAND_IN  int                           unit
  );
#endif

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_L3_SRC_BIND_INCLUDED__*/
#endif


