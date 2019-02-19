/* $Id: pb_pp_eg_qos.h,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_eg_qos.h
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

#ifndef __SOC_PB_PP_EG_QOS_INCLUDED__
/* { */
#define __SOC_PB_PP_EG_QOS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_eg_qos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

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
  SOC_PB_PP_EG_QOS_PORT_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_EG_QOS_FIRST,
  SOC_PB_PP_EG_QOS_PORT_INFO_SET_PRINT,
  SOC_PB_PP_EG_QOS_PORT_INFO_SET_UNSAFE,
  SOC_PB_PP_EG_QOS_PORT_INFO_SET_VERIFY,
  SOC_PB_PP_EG_QOS_PORT_INFO_GET,
  SOC_PB_PP_EG_QOS_PORT_INFO_GET_PRINT,
  SOC_PB_PP_EG_QOS_PORT_INFO_GET_VERIFY,
  SOC_PB_PP_EG_QOS_PORT_INFO_GET_UNSAFE,
  SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET,
  SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET_PRINT,
  SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET_UNSAFE,
  SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET_VERIFY,
  SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET,
  SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET_PRINT,
  SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET_VERIFY,
  SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET_UNSAFE,
  SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET,
  SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET_PRINT,
  SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET_UNSAFE,
  SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET_VERIFY,
  SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET,
  SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET_PRINT,
  SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET_VERIFY,
  SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_SET,
  SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_SET_PRINT,
  SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_SET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_SET_VERIFY,
  SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_GET,
  SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_GET_PRINT,
  SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_GET_VERIFY,
  SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_GET_UNSAFE,
  SOC_PB_PP_EG_QOS_GET_PROCS_PTR,
  SOC_PB_PP_EG_QOS_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_EG_QOS_PROCEDURE_DESC_LAST
} SOC_PB_PP_EG_QOS_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_EG_QOS_DSCP_EXP_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_EG_QOS_FIRST,
  SOC_PB_PP_EG_QOS_IN_DSCP_EXP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_QOS_EXP_MAP_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_QOS_PHP_TYPE_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_EG_QOS_ERR_LAST
} SOC_PB_PP_EG_QOS_ERR;

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
  soc_pb_pp_eg_qos_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_qos_port_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets port information for egress QoS setting, including
 *   profiles used for QoS remarking.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Out Local port ID.
 *   SOC_SAND_IN  SOC_PB_PP_EG_QOS_PORT_INFO                    *port_qos_info -
 *     The port information for QoS remark
 * REMARKS:
 *   Soc_petra-B only API, error is returned if called for T20E.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PORT_INFO                    *port_qos_info
  );

uint32
  soc_pb_pp_eg_qos_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PORT_INFO                    *port_qos_info
  );

uint32
  soc_pb_pp_eg_qos_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_qos_port_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_eg_qos_port_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_QOS_PORT_INFO                    *port_qos_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_qos_params_php_remark_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets how to remark QoS parameters upon PHP operation.
 *   When uniform pop performed the dscp_exp value is
 *   remarked.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY               *php_key -
 *     The key used for mapping including exp of the poped
 *     header
 *   SOC_SAND_IN  uint32                                  dscp_exp -
 *     The new value for dscp_exp, this overwrites the dscp_exp
 *     value calculated by the ingress pipe. This value will be
 *     mapped and used for the QoS fileds in the transmitted
 *     packet see soc_ppd_eg_qos_params_remark_set()
 * REMARKS:
 *   - Soc_petra-B only API, error is returned if called for
 *   T20E. - When pop type is pipe then the dscp_exp value is
 *   taken from the TOS/TC/EXP field in the networking header
 *   above the popped MPLS shim - When pop into MPLS the
 *   dscp_exp value not modified
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_params_php_remark_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_IN  uint32                                  dscp_exp
  );

uint32
  soc_pb_pp_eg_qos_params_php_remark_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_IN  uint32                                  dscp_exp
  );

uint32
  soc_pb_pp_eg_qos_params_php_remark_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY               *php_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_qos_params_php_remark_set_unsafe" API.
 *     Refer to "soc_pb_pp_eg_qos_params_php_remark_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_params_php_remark_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_OUT uint32                                  *dscp_exp
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_qos_params_remark_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remark QoS parameters, i.e. map in-dscp/exp and DP to
 *   out-dscp/exp in order to be set in outgoing packet
 *   headers.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY                      *in_qos_key -
 *     Incomming qos value (in-dscp-exp) as set by ingress
 *     pipe, or overwritten by POP functionalty see
 *     soc_ppd_eg_qos_params_php_remark_set()
 *   SOC_SAND_IN  SOC_PB_PP_EG_QOS_PARAMS                       *out_qos_params -
 *     This mapped values, used to set the Qos fields (TOS, TC,
 *     EXP) of the outer header(forwarded header and if PHP was
 *     performed then the network header above the
 *     forwarded/poped header)
 * REMARKS:
 *   Soc_petra-B only API, error is returned if called for T20E.
 *   - The out_qos_params used for - Setting the Qos of the
 *   forwarded header- Setting the Qos encapsulated header in
 *   case the encapsulation of type uniform - When tunnel
 *   encapsulation pushed to the packet and the encapsulation
 *   type is uniform, then the EXP of the pushed tunnel is
 *   taken from out_qos_params
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_params_remark_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PARAMS                       *out_qos_params
  );

uint32
  soc_pb_pp_eg_qos_params_remark_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PARAMS                       *out_qos_params
  );

uint32
  soc_pb_pp_eg_qos_params_remark_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY                      *in_qos_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_qos_params_remark_set_unsafe" API.
 *     Refer to "soc_pb_pp_eg_qos_params_remark_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_params_remark_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_OUT SOC_PB_PP_EG_QOS_PARAMS                       *out_qos_params
  );

/********************************************************************* 
* Invalid for Soc_petraB. 
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_qos_params_remark_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  );

uint32
  soc_pb_pp_eg_encap_qos_params_remark_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  );

uint32
  soc_pb_pp_eg_encap_qos_params_remark_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key
  );

/********************************************************************* 
* Invalid for Soc_petraB. 
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_qos_params_remark_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_qos_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_eg_qos module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_eg_qos_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_qos_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_eg_qos module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_eg_qos_get_errs_ptr(void);

uint32
  SOC_PB_PP_EG_QOS_MAP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY *info
  );

uint32
  SOC_PB_PP_EG_QOS_PARAMS_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PARAMS *info
  );

uint32
  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY *info
  );

uint32
  SOC_PB_PP_EG_QOS_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PORT_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_EG_QOS_INCLUDED__*/
#endif

