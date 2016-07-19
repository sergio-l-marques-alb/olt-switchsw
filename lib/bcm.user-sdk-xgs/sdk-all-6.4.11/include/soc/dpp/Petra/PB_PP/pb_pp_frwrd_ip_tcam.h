/* $Id: pb_pp_frwrd_ip_tcam.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_frwrd_ip_tcam.h
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

#ifndef __SOC_PB_PP_FRWRD_IP_TCAM_INCLUDED__
/* { */
#define __SOC_PB_PP_FRWRD_IP_TCAM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ipv6.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

#include <soc/dpp/Petra/PB_TM/pb_sw_db_tcam_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FRWRD_IP_TCAM_IPV4_MC (1 << 0)
#define SOC_PB_PP_FRWRD_IP_TCAM_IPV6_UC (1 << 1)
#define SOC_PB_PP_FRWRD_IP_TCAM_IPV6_MC (1 << 2)

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


typedef struct
{
  SOC_PB_IP_TCAM_ENTRY_TYPE type;

  union
  {
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY ipv4_mc;
    SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY ipv6_uc;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY ipv6_mc;
    SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY ipv6_vpn;
  } key;

  uint32 vrf_ndx;
} SOC_PB_PP_IP_TCAM_ENTRY_KEY;

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
  soc_pb_pp_frwrd_ip_tcam_init_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  required_uses
  );

uint32
  soc_pb_pp_frwrd_ip_tcam_route_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  uint32                                      fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  );

uint32
  soc_pb_pp_frwrd_ip_tcam_route_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  uint8                                     exact_match,
    SOC_SAND_OUT uint32                                      *fec_id,
    SOC_SAND_OUT uint8                                     *found
  );

uint32
  soc_pb_pp_frwrd_ip_tcam_route_get_block_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_INOUT  SOC_PB_PP_IP_ROUTING_TABLE_RANGE               *block_range_key,
    SOC_SAND_OUT  SOC_PB_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_OUT uint32                                      *fec_id,
    SOC_SAND_OUT uint32                                      *nof_entries
  );



uint32
  soc_pb_pp_frwrd_ip_tcam_route_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY                       *route_key
  );



uint32
  soc_pb_pp_frwrd_ip_tcam_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  uint8                                    clear_all_vrf
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FRWRD_IP_TCAM_INCLUDED__*/
#endif

