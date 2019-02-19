
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_eg_qos.c,v 1.9 Broadcom SDK $
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
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_eg_qos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_qos.h>

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

/*********************************************************************
*     Sets port information for egress QoS setting, including
 *     profiles used for QoS remarking.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_qos_port_info_set(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_EG_QOS_PORT_INFO                        *port_qos_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_QOS_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_qos_info);

  res = arad_pp_eg_qos_port_info_set_verify(
          unit,
          local_port_ndx,
          port_qos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_port_info_set_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          port_qos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_port_info_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets port information for egress QoS setting, including
 *     profiles used for QoS remarking.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_qos_port_info_get(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT ARAD_PP_EG_QOS_PORT_INFO                        *port_qos_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_QOS_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_qos_info);

  res = arad_pp_eg_qos_port_info_get_verify(
          unit,
          local_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_port_info_get_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          port_qos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_port_info_get()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets how to remark QoS parameters upon PHP operation.
 *     When uniform pop performed the dscp_exp value is
 *     remarked.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_qos_params_php_remark_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_EG_QOS_PHP_REMARK_KEY                   *php_key,
    SOC_SAND_IN  uint32                                      dscp_exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_QOS_PARAMS_PHP_REMARK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(php_key);

  res = arad_pp_eg_qos_params_php_remark_set_verify(
          unit,
          php_key,
          dscp_exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_php_remark_set_unsafe(
          unit,
          php_key,
          dscp_exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_params_php_remark_set()", 0, 0);
}

/*********************************************************************
*     Sets how to remark QoS parameters upon PHP operation.
 *     When uniform pop performed the dscp_exp value is
 *     remarked.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_qos_params_php_remark_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_EG_QOS_PHP_REMARK_KEY                   *php_key,
    SOC_SAND_OUT uint32                                      *dscp_exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_QOS_PARAMS_PHP_REMARK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(php_key);
  SOC_SAND_CHECK_NULL_INPUT(dscp_exp);

  res = arad_pp_eg_qos_params_php_remark_get_verify(
          unit,
          php_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_php_remark_get_unsafe(
          unit,
          php_key,
          dscp_exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_params_php_remark_get()", 0, 0);
}

#ifdef BCM_88660
uint32
  arad_pp_eg_qos_params_marking_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_PP_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_IN ARAD_PP_EG_QOS_MARKING_PARAMS *qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_qos_params_marking_set_verify(unit, qos_key, qos_params);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_marking_set_unsafe(unit, qos_key, qos_params);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_params_marking_set()", 0, 0);
}

uint32
  arad_pp_eg_qos_params_marking_get(
    SOC_SAND_IN     int unit,
    SOC_SAND_IN     ARAD_PP_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_OUT    ARAD_PP_EG_QOS_MARKING_PARAMS *qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_qos_params_marking_get_verify(unit, qos_key, qos_params);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_marking_get_unsafe(unit, qos_key, qos_params);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_params_marking_get()", 0, 0);
}

uint32
  arad_pp_eg_qos_global_info_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN ARAD_PP_EG_QOS_GLOBAL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_qos_global_info_set_verify(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_global_info_set_unsafe(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_global_info_set()", 0, 0);
}

uint32 
  arad_pp_eg_qos_global_info_get(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT ARAD_PP_EG_QOS_GLOBAL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_qos_global_info_get_verify(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_global_info_get_unsafe(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_global_info_get()", 0, 0);
}

#endif /* BCM_88660 */

/*********************************************************************
*     Remark QoS parameters, i.e. map in-dscp/exp and DP to
 *     out-dscp/exp in order to be set in outgoing packet
 *     forwarding headers.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  arad_pp_eg_qos_params_remark_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_EG_QOS_MAP_KEY                          *in_qos_key,
    SOC_SAND_IN  ARAD_PP_EG_QOS_PARAMS                           *out_qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_QOS_PARAMS_REMARK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_qos_params);

  res = arad_pp_eg_qos_params_remark_set_verify(
          unit,
          in_qos_key,
          out_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_remark_set_unsafe(
          unit,
          in_qos_key,
          out_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_params_remark_set()", 0, 0);
}

/*********************************************************************
*     Remark QoS parameters, i.e. map in-dscp/exp and DP to
 *     out-dscp/exp in order to be set in outgoing packet
 *     headers.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_qos_params_remark_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_EG_QOS_MAP_KEY                          *in_qos_key,
    SOC_SAND_OUT ARAD_PP_EG_QOS_PARAMS                           *out_qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_QOS_PARAMS_REMARK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_qos_params);

  res = arad_pp_eg_qos_params_remark_get_verify(
          unit,
          in_qos_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_remark_get_unsafe(
          unit,
          in_qos_key,
          out_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_qos_params_remark_get()", 0, 0);
}

/*********************************************************************
*     Remark QoS parameters, i.e. map in-dscp/exp remark_profile and 
 *     header pkt type to out-dscp/exp in order to be set in outgoing
 *     packet encapsulated headers.
 *     Details: in the H file. (search for prototype)
 *    Valid only for ARAD.
 *********************************************************************/
uint32
  arad_pp_eg_encap_qos_params_remark_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_QOS_PARAMS_REMARK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_encap_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_encap_qos_params);

  res = arad_pp_eg_encap_qos_params_remark_set_verify(
          unit,
          in_encap_qos_key,
          out_encap_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_qos_params_remark_set_unsafe(
          unit,
          in_encap_qos_key,
          out_encap_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_qos_params_remark_set()", 0, 0);
}

/*********************************************************************
*     Remark QoS parameters, i.e. map in-dscp/exp remark_profile and 
 *     header pkt type to out-dscp/exp in order to be set in outgoing
 *     packet encapsulated headers.
 *     Details: in the H file. (search for prototype)
 *    Valid only for ARAD.
 *********************************************************************/
uint32
  arad_pp_eg_encap_qos_params_remark_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_QOS_PARAMS_REMARK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_encap_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_encap_qos_params);

  res = arad_pp_eg_encap_qos_params_remark_get_verify(
          unit,
          in_encap_qos_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_qos_params_remark_get_unsafe(
          unit,
          in_encap_qos_key,
          out_encap_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_qos_params_remark_get()", 0, 0);
}

void
  ARAD_PP_EG_QOS_MAP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_EG_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_QOS_MAP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_QOS_PARAMS_clear(
    SOC_SAND_OUT ARAD_PP_EG_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_QOS_PARAMS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_ENCAP_QOS_MAP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_QOS_MAP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_ENCAP_QOS_PARAMS_clear(
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_QOS_PARAMS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_QOS_PHP_REMARK_KEY_clear(
    SOC_SAND_OUT ARAD_PP_EG_QOS_PHP_REMARK_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_QOS_PHP_REMARK_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_QOS_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_QOS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_QOS_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_EG_QOS_UNIFORM_PHP_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_EG_QOS_UNIFORM_PHP_TYPE enum_val
  )
{
  return SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE_to_string(enum_val);
}

void
  ARAD_PP_EG_QOS_MAP_KEY_print(
    SOC_SAND_IN  ARAD_PP_EG_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_QOS_MAP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_QOS_PARAMS_print(
    SOC_SAND_IN  ARAD_PP_EG_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_QOS_PARAMS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_QOS_PHP_REMARK_KEY_print(
    SOC_SAND_IN  ARAD_PP_EG_QOS_PHP_REMARK_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_QOS_PHP_REMARK_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_QOS_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_QOS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_QOS_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

