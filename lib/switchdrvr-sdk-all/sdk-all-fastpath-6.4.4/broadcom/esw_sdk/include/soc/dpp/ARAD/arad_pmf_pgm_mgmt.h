/* $Id: arad_pmf_pgm_mgmt.h,v 1.4 Broadcom SDK $
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

#ifndef __ARAD_PMF_PGM_MGMT_INCLUDED__
/* { */
#define __ARAD_PMF_PGM_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_pmf_pgm_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define ARAD_NOF_ETHERNET_FRAME_VLAN_FORMATS_HW                  (12)

#define ARAD_PMF_PGM_PKT_HDR_TYPE_RAW                          SOC_TMC_FP_PKT_HDR_TYPE_RAW
#define ARAD_PMF_PGM_PKT_HDR_TYPE_FTMH                         SOC_TMC_FP_PKT_HDR_TYPE_FTMH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_TM                           SOC_TMC_FP_PKT_HDR_TYPE_TM
#define ARAD_PMF_PGM_PKT_HDR_TYPE_ETH                          SOC_TMC_FP_PKT_HDR_TYPE_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_ETH                      SOC_TMC_FP_PKT_HDR_TYPE_ETH_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_ETH                     SOC_TMC_FP_PKT_HDR_TYPE_IPV4_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_ETH                     SOC_TMC_FP_PKT_HDR_TYPE_IPV6_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_MPLS1_ETH                    SOC_TMC_FP_PKT_HDR_TYPE_MPLS1_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_MPLS2_ETH                    SOC_TMC_FP_PKT_HDR_TYPE_MPLS2_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_MPLS3_ETH                    SOC_TMC_FP_PKT_HDR_TYPE_MPLS3_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS1_ETH                SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS2_ETH                SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS3_ETH                SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS1_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS2_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS3_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS1_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS2_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS3_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_IPV4_ETH                SOC_TMC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_IPV4_ETH                SOC_TMC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_TRILL_ETH                SOC_TMC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH
#define ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_IPV4_ETH                 SOC_TMC_FP_PKT_HDR_TYPE_ETH_IPV4_ETH
#define ARAD_PMF_PGM_NOF_PKT_HDR_TYPES                         SOC_TMC_NOF_FP_PKT_HDR_TYPES
typedef SOC_TMC_FP_PKT_HDR_TYPE                                  ARAD_PMF_PGM_PKT_HDR_TYPE;

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

uint32
  arad_pmf_pgm_packet_format_code_convert(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PMF_PGM_PKT_HDR_TYPE      hdr_type_ndx,
    SOC_SAND_OUT uint32                     *pfc
  );
#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PMF_PGM_MGMT_INCLUDED__*/
#endif



