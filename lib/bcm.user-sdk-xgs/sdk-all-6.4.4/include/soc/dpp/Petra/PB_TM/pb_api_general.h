/* $Id: pb_api_general.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/SOC_PB_TM/include/soc_pb_api_general.h
*
* MODULE PREFIX:  pb
*
* FILE DESCRIPTION: General configurations for Soc_petra Driver API-s.
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_PB_API_GENERAL_INCLUDED__
/* { */
#define __SOC_PB_API_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_MAL_ID_CPU                                               SOC_PETRA_IF_ID_CPU
#define SOC_PB_MAL_ID_OLP                                               SOC_PETRA_IF_ID_OLP
#define SOC_PB_MAL_ID_RCY                                               SOC_PETRA_IF_ID_RCY
#define SOC_PB_MAL_ID_ERP                                               SOC_PETRA_IF_ID_ERP
#define SOC_PB_MAL_ID_NONE                                              SOC_PETRA_IF_ID_NONE

#define SOC_PB_MAL_TYPE_NONE                                            SOC_PETRA_MAL_TYPE_NONE
#define SOC_PB_MAL_TYPE_CPU                                             SOC_PETRA_MAL_TYPE_CPU
#define SOC_PB_MAL_TYPE_RCY                                             SOC_PETRA_MAL_TYPE_RCY
#define SOC_PB_MAL_TYPE_OLP                                             SOC_PETRA_MAL_TYPE_OLP
#define SOC_PB_MAL_TYPE_ERP                                             SOC_PETRA_MAL_TYPE_ERP
#define SOC_PB_MAL_TYPE_NIF                                             SOC_PETRA_MAL_TYPE_NIF

#define SOC_PB_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE                     SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE
#define SOC_PB_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET                      SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET

#define SOC_PB_PKT_FRWRD_TYPE_BRIDGE                 SOC_TMC_PKT_FRWRD_TYPE_BRIDGE
#define SOC_PB_PKT_FRWRD_TYPE_IPV4_UC                SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC
#define SOC_PB_PKT_FRWRD_TYPE_IPV4_MC                SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC
#define SOC_PB_PKT_FRWRD_TYPE_IPV6_UC                SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC
#define SOC_PB_PKT_FRWRD_TYPE_IPV6_MC                SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC
#define SOC_PB_PKT_FRWRD_TYPE_MPLS                   SOC_TMC_PKT_FRWRD_TYPE_MPLS
#define SOC_PB_PKT_FRWRD_TYPE_TRILL                  SOC_TMC_PKT_FRWRD_TYPE_TRILL
#define SOC_PB_PKT_FRWRD_TYPE_CPU_TRAP               SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP
#define SOC_PB_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM      SOC_TMC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM
#define SOC_PB_PKT_FRWRD_TYPE_TM                     SOC_TMC_PKT_FRWRD_TYPE_TM
typedef SOC_TMC_PKT_FRWRD_TYPE                        SOC_PB_PKT_FRWRD_TYPE;

#define SOC_PB_CONNECTION_DIRECTION_NDX_MAX         (SOC_PETRA_NOF_CONNECTION_DIRECTIONS-1)
#define SOC_PB_PORT_DIRECTION_NDX_MAX               (SOC_PETRA_NOF_CONNECTION_DIRECTIONS-1)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PB_DO_NOTHING_AND_EXIT                    \
  do                                              \
  {                                               \
    SOC_SAND_IGNORE_UNUSED_VAR(res);                  \
    goto exit;                                    \
  } while(0)
          

#define SOC_PB_IS_ERR_RES(res_)                       \
  (soc_sand_get_error_code_from_error_word(res_) != SOC_SAND_OK)

#define SOC_PB_IF2MAL_NDX(if_id)                      \
  (SOC_PB_NIF_IS_PB_ID(if_id)?SOC_PB_NIF2MAL_GLBL_ID(soc_pb_nif2intern_id(if_id)):(if_id))

#define SOC_PB_MAL2IF_NDX(mal_id, if_ndx)             \
  if ((mal_id)<SOC_PB_NOF_MAC_LANES) {                \
    SOC_PB_NIF_TYPE nif_type;                         \
                                                      \
    res = soc_pb_nif_mal_type_get(                    \
            unit,                                \
            mal_id,                                   \
            &nif_type                                 \
          );                                          \
    SOC_SAND_CHECK_FUNC_RESULT(res, 7777, exit);      \
                                                      \
    if_ndx = soc_pb_nif_intern2nif_id(nif_type, SOC_PB_MAL2NIF_GLBL_ID(mal_id));\
  } else {                                            \
    if_ndx = (mal_id);                                \
  }

/* Interface Ports Macros { */


#define SOC_PB_IS_CPU_IF_ID(if_id) \
  (SOC_PETRA_IS_CPU_IF_ID(if_id))

#define SOC_PB_IS_OLP_IF_ID(if_id) \
  (SOC_PETRA_IS_OLP_IF_ID(if_id))

#define SOC_PB_IS_RCY_IF_ID(if_id) \
  (SOC_PETRA_IS_RCY_IF_ID(if_id))

#define SOC_PB_IS_ERP_IF_ID(if_id) \
  (SOC_PETRA_IS_ERP_IF_ID(if_id))


#define SOC_PB_IS_NONE_IF_ID(if_id) \
  (SOC_PETRA_IS_NONE_IF_ID(if_id))

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
  soc_pb_interface_id_verify(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx
  );

uint32
  soc_pb_if_type_from_id(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID   soc_pb_nif_id,
    SOC_SAND_OUT SOC_PETRA_INTERFACE_TYPE *interface_type
  );

uint32
  soc_pb_mal_equivalent_id_verify(
    SOC_SAND_IN  uint32 mal_ndx
  );

SOC_PETRA_MAL_EQUIVALENT_TYPE
  soc_pb_mal_type_from_id(
    SOC_SAND_IN uint32 mal_id
  );

SOC_PETRA_REV
  soc_pb_revision_get(
    SOC_SAND_IN  int  unit
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PB_API_GENERAL_INCLUDED__*/
#endif
