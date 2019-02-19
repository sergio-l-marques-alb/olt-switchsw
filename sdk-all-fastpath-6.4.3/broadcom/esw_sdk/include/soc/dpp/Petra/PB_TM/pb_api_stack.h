/* $Id: soc_pb_api_stack.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PB_API_STACK_INCLUDED__
/* { */
#define __SOC_PB_API_STACK_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_stack.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     The number of words needed to store the pruning bitmap. */
#define  SOC_PB_STACK_PRUN_BMP_LEN (SOC_TMC_STACK_PRUN_BMP_LEN)

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

#define SOC_PB_STACK_MAX_NOF_TM_DOMAINS_1                      SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_1
#define SOC_PB_STACK_MAX_NOF_TM_DOMAINS_8                      SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_8
#define SOC_PB_STACK_MAX_NOF_TM_DOMAINS_16                     SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_16
typedef SOC_TMC_STACK_MAX_NOF_TM_DOMAINS                       SOC_PB_STACK_MAX_NOF_TM_DOMAINS;

typedef SOC_TMC_STACK_GLBL_INFO                                SOC_PB_STACK_GLBL_INFO;
typedef SOC_TMC_STACK_PORT_DISTR_INFO                          SOC_PB_STACK_PORT_DISTR_INFO;

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
 *   soc_pb_stack_global_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   set the global information of the stacking module,
 *   including whether stacking is supported, maximum number
 *   of supported TM-domains in the system and the TM-domain
 *   of this device
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_STACK_GLBL_INFO       *info -
 *     Stacking module global info, including whether stacking
 *     is supported and maximum number of supported TM-domains
 *     in the system.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_stack_global_info_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_STACK_GLBL_INFO       *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_stack_global_info_set" API.
 *     Refer to "soc_pb_stack_global_info_set" API for details.
*********************************************************************/
uint32
  soc_pb_stack_global_info_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PB_STACK_GLBL_INFO       *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_stack_port_distribution_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to distribute and prune packets sent through
 *   this stacking port.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                local_stack_port_ndx -
 *     SOC_SAND_IN uint32 local_stack_port_ndx
 *   SOC_SAND_IN  SOC_PB_STACK_PORT_DISTR_INFO *distribution_info -
 *     Traffic distribution info for packets sent through this
 *     stacking port, including what is the next TM-domain, and
 *     what TM-domains not to pass through.
 * REMARKS:
 *   1. using this API user can define either source-specific
 *   (SS) or common Distribution-Tree 2. The stack-port-ids
 *   share the same domain with system physical ports. 3.
 *   soc_pb_sys_stack_port_to_local_port_map_set() has to be
 *   called before this API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_stack_port_distribution_info_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                local_stack_port_ndx,
    SOC_SAND_IN  SOC_PB_STACK_PORT_DISTR_INFO *distribution_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_stack_port_distribution_info_set" API.
 *     Refer to "soc_pb_stack_port_distribution_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_stack_port_distribution_info_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                local_stack_port_ndx,
    SOC_SAND_OUT SOC_PB_STACK_PORT_DISTR_INFO *distribution_info
  );

void
  SOC_PB_STACK_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PB_STACK_GLBL_INFO *info
  );

void
  SOC_PB_STACK_PORT_DISTR_INFO_clear(
    SOC_SAND_OUT SOC_PB_STACK_PORT_DISTR_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_STACK_MAX_NOF_TM_DOMAINS_to_string(
    SOC_SAND_IN  SOC_PB_STACK_MAX_NOF_TM_DOMAINS enum_val
  );

void
  SOC_PB_STACK_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PB_STACK_GLBL_INFO *info
  );

void
  SOC_PB_STACK_PORT_DISTR_INFO_print(
    SOC_SAND_IN  SOC_PB_STACK_PORT_DISTR_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_STACK_INCLUDED__*/
#endif

