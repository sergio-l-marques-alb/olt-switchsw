
/* $Id: arad_pp_mgmt.h,v 1.6 Broadcom SDK $
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

#ifndef __ARAD_PP_MGMT_INCLUDED__
/* { */
#define __ARAD_PP_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_mgmt.h>
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
  ARAD_PP_MGMT_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_MGMT_FIRST,
  ARAD_PP_MGMT_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  ARAD_PP_MGMT_DEVICE_INIT,
  ARAD_PP_MGMT_DEVICE_CLOSE,
  ARAD_PP_MGMT_OPERATION_MODE_SET,
  ARAD_PP_MGMT_OPERATION_MODE_GET,
  ARAD_PP_MGMT_OPERATION_MODE_SET_UNSAFE,
  ARAD_PP_MGMT_OPERATION_MODE_VERIFY,
  ARAD_PP_MGMT_OPERATION_MODE_GET_UNSAFE,
  ARAD_PP_MGMT_ELK_MODE_SET,
  ARAD_PP_MGMT_ELK_MODE_SET_PRINT,
  ARAD_PP_MGMT_ELK_MODE_SET_UNSAFE,
  ARAD_PP_MGMT_ELK_MODE_SET_VERIFY,
  ARAD_PP_MGMT_ELK_MODE_GET,
  ARAD_PP_MGMT_ELK_MODE_GET_PRINT,
  ARAD_PP_MGMT_ELK_MODE_GET_VERIFY,
  ARAD_PP_MGMT_ELK_MODE_GET_UNSAFE,
  ARAD_PP_MGMT_USE_ELK_SET,
  ARAD_PP_MGMT_USE_ELK_SET_PRINT,
  ARAD_PP_MGMT_USE_ELK_SET_UNSAFE,
  ARAD_PP_MGMT_USE_ELK_SET_VERIFY,
  ARAD_PP_MGMT_USE_ELK_GET,
  ARAD_PP_MGMT_USE_ELK_GET_PRINT,
  ARAD_PP_MGMT_USE_ELK_GET_VERIFY,
  ARAD_PP_MGMT_USE_ELK_GET_UNSAFE,
  ARAD_PP_MGMT_PROC_ERR_MECH_INIT,

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_MGMT_PROCEDURE_DESC_LAST
} ARAD_PP_MGMT_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_MGMT_FIRST,
  ARAD_PP_MGMT_INGRESS_PKT_RATE_OUT_OF_RANGE_ERR,
  ARAD_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_MGMT_ERR_LAST
} ARAD_PP_MGMT_ERR;

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
*     arad_pp_mgmt_operation_mode_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set arad_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode -
*     arad_pp device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mgmt_operation_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode
  );

/*********************************************************************
* NAME:
*     arad_pp_mgmt_operation_mode_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set arad_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode -
*     arad_pp device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mgmt_operation_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode
  );

/*********************************************************************
* NAME:
*     arad_pp_mgmt_operation_mode_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set arad_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT ARAD_PP_MGMT_OPERATION_MODE *op_mode -
*     arad_pp device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mgmt_operation_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_PP_MGMT_OPERATION_MODE *op_mode
  );

/*********************************************************************
* NAME:
*     arad_pp_mgmt_device_close_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Close the Device, and clean HW and SW.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to close.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mgmt_device_close_unsafe(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_mgmt_elk_mode_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the ELK interface mode.
 * INPUT:
 *   SOC_SAND_IN  int           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode -
 *     The requested operation mode for the ELK interface.
 *   SOC_SAND_OUT uint32           *ingress_pkt_rate -
 *     The effective processing rate of the ingress device in
 *     packets per second.
 * REMARKS:
 *   1. The ELK's physical interface has to be configured
 *   before calling this function. See arad_nif_elk_set().2.
 *   Arad-B rev. B0 modes are not available on earlier
 *   revisions.3. The ingress device's rate is limited by the
 *   processing rate of the ELK interface, affecting all
 *   packets entering the device.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mgmt_elk_mode_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  );

uint32
  arad_pp_mgmt_elk_mode_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode
  );

uint32
  arad_pp_mgmt_elk_mode_get_verify(
    SOC_SAND_IN  int           unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mgmt_elk_mode_set_unsafe" API.
 *     Refer to "arad_pp_mgmt_elk_mode_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_mgmt_elk_mode_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_ELK_MODE *elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  );

/*********************************************************************
* NAME:
 *   arad_pp_mgmt_use_elk_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Determine whether the specified lookup is externalized
 *   or not.
 * INPUT:
 *   SOC_SAND_IN  int           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type -
 *     The lookup type in question.
 *   SOC_SAND_IN  uint8           use_elk -
 *     Whether to use the ELK for that lookup or not.
 * REMARKS:
 *   IPv4 multicast and IPv6 lookups can only be performed
 *   externally in Arad-B rev. B0 modes.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mgmt_use_elk_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  );

uint32
  arad_pp_mgmt_use_elk_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  );

uint32
  arad_pp_mgmt_use_elk_get_verify(
    SOC_SAND_IN  int           unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mgmt_use_elk_set_unsafe" API.
 *     Refer to "arad_pp_mgmt_use_elk_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_mgmt_use_elk_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_LKP_TYPE *lkp_type,
    SOC_SAND_OUT uint8           *use_elk
  );

/*********************************************************************
* NAME:
 *   arad_pp_mgmt_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_mgmt_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_mgmt_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_mgmt_get_errs_ptr(void);

/* } */


#if ARAD_PP_DEBUG_IS_LVL1

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_MGMT_INCLUDED__*/
#endif


