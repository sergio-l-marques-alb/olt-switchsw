#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_llp_trap.c,v 1.6 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP

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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_llp_trap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_trap.h>

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
*     Sets port information for Link Layer Traps, including
 *     which reserved Multicast profile and which Traps are
 *     enabled...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_trap_port_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_PORT_INFO                      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_llp_trap_port_info_set_verify(
          unit,
          local_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_trap_port_info_set_unsafe(
          unit,
          core_id,
          local_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_port_info_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets port information for Link Layer Traps, including
 *     which reserved Multicast profile and which Traps are
 *     enabled...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_trap_port_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_PORT_INFO                      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_llp_trap_port_info_get_verify(
          unit,
          local_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_trap_port_info_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_port_info_get()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets information for ARP trapping, including My-IP
 *     addresses (used to Trap ARP Requests)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_trap_arp_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_ARP_INFO                       *arp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_ARP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(arp_info);

  res = arad_pp_llp_trap_arp_info_set_verify(
          unit,
          arp_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_trap_arp_info_set_unsafe(
          unit,
          arp_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_arp_info_set()", 0, 0);
}

/*********************************************************************
*     Sets information for ARP trapping, including My-IP
 *     addresses (used to Trap ARP Requests)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_trap_arp_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_ARP_INFO                       *arp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_ARP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(arp_info);

  res = arad_pp_llp_trap_arp_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_trap_arp_info_get_unsafe(
          unit,
          arp_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_arp_info_get()", 0, 0);
}

/*********************************************************************
*     Sets Trap information for IEEE reserved multicast
 *     (Ethernet Header. DA matches 01-80-c2-00-00-XX where XX =
 *     8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_trap_reserved_mc_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key,
    SOC_SAND_IN  ARAD_PP_ACTION_PROFILE                          *trap_action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(reserved_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(trap_action);

  res = arad_pp_llp_trap_reserved_mc_info_set_verify(
          unit,
          reserved_mc_key,
          trap_action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_trap_reserved_mc_info_set_unsafe(
          unit,
          reserved_mc_key,
          trap_action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_reserved_mc_info_set()", 0, 0);
}

/*********************************************************************
*     Sets Trap information for IEEE reserved multicast
 *     (Ethernet Header. DA matches 01-80-c2-00-00-XX where XX =
 *     8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_trap_reserved_mc_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key,
    SOC_SAND_OUT ARAD_PP_ACTION_PROFILE                          *trap_action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(reserved_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(trap_action);

  res = arad_pp_llp_trap_reserved_mc_info_get_verify(
          unit,
          reserved_mc_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_trap_reserved_mc_info_get_unsafe(
          unit,
          reserved_mc_key,
          trap_action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_reserved_mc_info_get()", 0, 0);
}

/*********************************************************************
*     Sets a programmable trap, a trap that may be set to
 *     packets according to L2/L3/L4 attributes.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_trap_prog_trap_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prog_trap_qual);

  res = arad_pp_llp_trap_prog_trap_info_set_verify(
          unit,
          prog_trap_ndx,
          prog_trap_qual
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_trap_prog_trap_info_set_unsafe(
          unit,
          prog_trap_ndx,
          prog_trap_qual
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_prog_trap_info_set()", prog_trap_ndx, 0);
}

/*********************************************************************
*     Sets a programmable trap, a trap that may be set to
 *     packets according to L2/L3/L4 attributes.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_trap_prog_trap_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prog_trap_qual);

  res = arad_pp_llp_trap_prog_trap_info_get_verify(
          unit,
          prog_trap_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_trap_prog_trap_info_get_unsafe(
          unit,
          prog_trap_ndx,
          prog_trap_qual
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_prog_trap_info_get()", prog_trap_ndx, 0);
}

void
  ARAD_PP_LLP_TRAP_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_ARP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_ARP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_ARP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_RESERVED_MC_KEY_clear(
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_RESERVED_MC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_PROG_TRAP_L2_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_PROG_TRAP_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_PROG_TRAP_L3_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_PROG_TRAP_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_PROG_TRAP_L4_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_PROG_TRAP_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(
    SOC_SAND_OUT ARAD_PP_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_LLP_TRAP_PORT_ENABLE_to_string(
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_PORT_ENABLE enum_val
  )
{
  return SOC_PPC_LLP_TRAP_PORT_ENABLE_to_string(enum_val);
}

const char*
  ARAD_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_to_string(
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_PROG_TRAP_COND_SELECT enum_val
  )
{
  return SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_to_string(enum_val);
}

void
  ARAD_PP_LLP_TRAP_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_ARP_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_ARP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_ARP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_RESERVED_MC_KEY_print(
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_RESERVED_MC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_PROG_TRAP_L2_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_PROG_TRAP_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_PROG_TRAP_L3_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_PROG_TRAP_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_PROG_TRAP_L4_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_PROG_TRAP_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_TRAP_PROG_TRAP_QUALIFIER_print(
    SOC_SAND_IN  ARAD_PP_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

