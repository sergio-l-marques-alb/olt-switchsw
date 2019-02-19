/* $Id: tmc_pmf_pgm_mgmt.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_TMC_PMF_PGM_MGMT_INCLUDED__
/* { */
#define __SOC_TMC_PMF_PGM_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_ports.h>

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
   *  The PMF Program addition / removal is triggered by the
   *  PP-Port.
   */
  SOC_TMC_PMF_PGM_MGMT_SOURCE_PP_PORT = 0,
  /*
   *  The PMF Program addition / removal is triggered by the
   *  FP.
   */
  SOC_TMC_PMF_PGM_MGMT_SOURCE_FP = 1,
  /*
   *  Number of types in SOC_TMC_PMF_PGM_MGMT_SOURCE
   */
  SOC_TMC_PMF_NOF_PGM_MGMT_SOURCES = 2
}SOC_TMC_PMF_PGM_MGMT_SOURCE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If True, then the configuration must be set. If False,
   *  only verify there is enough resource for the new
   *  configuration.
   */
  uint8 is_to_set;
  /*
   *  If True, then the Database is created. Otherwise, the
   *  Database is deleted.
   */
  uint8 is_addition;
  /* 
   * If True, second iteration (in case of failure) 
   */
  uint8 is_2nd_iter;

} SOC_TMC_PMF_PGM_MGMT_INFO;

typedef enum
{
  /*
   *  Raw packet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_RAW = 0x1,
  /*
   *  Packet with first header FTMH (e.g., for stacking ports)
   *  and possibly with a PPH afterwards.
   */
  SOC_TMC_FP_PKT_HDR_TYPE_FTMH = 0x2,
  /*
   *  Only ITMH as parsed header
   */
  SOC_TMC_FP_PKT_HDR_TYPE_TM = 0x4,
  /*
   *  Ingress Shaping + ITMH headers
   */
  SOC_TMC_FP_PKT_HDR_TYPE_TM_IS = 0x8,
  /*
   *  ITMH + PPH headers
   */
  SOC_TMC_FP_PKT_HDR_TYPE_TM_PPH = 0x10,
  /*
   *  Ingress Shaping + ITMH + PPH headers
   */
  SOC_TMC_FP_PKT_HDR_TYPE_TM_IS_PPH = 0x20,
  /*
   *  Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_ETH = 0x40,
  /*
   *  MAC-in-MAC
   */
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_ETH = 0x80,
  /*
   *  IPv4 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_ETH = 0x100,
  /*
   *  IPv6 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_ETH = 0x200,
  /*
   *  MPLS over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_MPLS1_ETH = 0x400,
  /*
   *  MPLS x 2 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_MPLS2_ETH = 0x800,
  /*
   *  MPLS x 3 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_MPLS3_ETH = 0x1000,
  /*
   *  Ethernet over MPLS over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH = 0x2000,
  /*
   *  Ethernet over MPLS x 2 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH = 0x4000,
  /*
   *  Ethernet over MPLS x 3 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH = 0x8000,
  /*
   *  IPv4 over MPLS over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH = 0x10000,
  /*
   *  IPv4 over MPLS x 2 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH = 0x20000,
  /*
   *  IPv4 over MPLS x 3 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH = 0x40000,
  /*
   *  IPv6 over MPLS over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH = 0x80000,
  /*
   *  IPv6 over MPLS x 2 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH = 0x100000,
  /*
   *  IPv6 over MPLS x 3 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH = 0x200000,
  /*
   *  IPv4 over IPv4 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH = 0x400000,
  /*
   *  IPv6 over IPv4 over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH = 0x800000,
  /*
   *  Ethernet over Trill over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH = 0x1000000,
  /*
   *  Ethernet over IP over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_IPV4_ETH = 0x2000000,
  /*
   *  Fiber Channel over Ethernet
   */
  SOC_TMC_FP_PKT_HDR_TYPE_FC_ETH = 0x4000000,
  /*
   *  Number of types in SOC_TMC_FP_PKT_HDR_TYPE
   */
  SOC_TMC_NOF_FP_PKT_HDR_TYPES = 25
}SOC_TMC_FP_PKT_HDR_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  PP-Port index. Range: 0 - 63.
   */
  uint32 pp_port_ndx;
  /*
   *  Packet-Format-Group (relevant if Header type is
   *  Ethernet) given by the FP. Range: 0 - 7.
   */
  uint32 pfg_ndx;

} SOC_TMC_PMF_PGM_MGMT_NDX;


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

void
  SOC_TMC_PMF_PGM_MGMT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_MGMT_INFO *info
  );

void
  SOC_TMC_PMF_PGM_MGMT_NDX_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_MGMT_NDX *info
  );

#if SOC_TMC_DEBUG_IS_LVL1
const char*
  SOC_TMC_FP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_FP_PKT_HDR_TYPE enum_val
  );

const char*
  SOC_TMC_PMF_PGM_MGMT_SOURCE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_MGMT_SOURCE enum_val
  );

void
  SOC_TMC_PMF_PGM_MGMT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_MGMT_INFO *info
  );

void
  SOC_TMC_PMF_PGM_MGMT_NDX_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_MGMT_NDX *info
  );

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_TMC_PMF_PGM_MGMT_INCLUDED__*/
#endif

