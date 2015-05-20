/* $Id: soc_pb_pmf_pgm_mgmt.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PB_PMF_PGM_MGMT_INCLUDED__
/* { */
#define __SOC_PB_PMF_PGM_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>

#include <soc/dpp/TMC/tmc_pmf_pgm_mgmt.h>
#include <soc/dpp/PPC/ppc_api_fp.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PB_NOF_ETHERNET_FRAME_VLAN_FORMATS_HW                  (12)

#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_RAW                          SOC_TMC_FP_PKT_HDR_TYPE_RAW
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_FTMH                         SOC_TMC_FP_PKT_HDR_TYPE_FTMH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_TM                           SOC_TMC_FP_PKT_HDR_TYPE_TM
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_TM_IS                        SOC_TMC_FP_PKT_HDR_TYPE_TM_IS
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_TM_PPH                       SOC_TMC_FP_PKT_HDR_TYPE_TM_PPH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_TM_IS_PPH                    SOC_TMC_FP_PKT_HDR_TYPE_TM_IS_PPH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH                          SOC_TMC_FP_PKT_HDR_TYPE_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_ETH                      SOC_TMC_FP_PKT_HDR_TYPE_ETH_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_ETH                     SOC_TMC_FP_PKT_HDR_TYPE_IPV4_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_ETH                     SOC_TMC_FP_PKT_HDR_TYPE_IPV6_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_MPLS1_ETH                    SOC_TMC_FP_PKT_HDR_TYPE_MPLS1_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_MPLS2_ETH                    SOC_TMC_FP_PKT_HDR_TYPE_MPLS2_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_MPLS3_ETH                    SOC_TMC_FP_PKT_HDR_TYPE_MPLS3_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS1_ETH                SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS2_ETH                SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS3_ETH                SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS1_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS2_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS3_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS1_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS2_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS3_ETH               SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_IPV4_ETH                SOC_TMC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_IPV4_ETH                SOC_TMC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH
#define SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_TRILL_ETH                SOC_TMC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH
#define SOC_PB_PMF_PGM_NOF_PKT_HDR_TYPES                         SOC_TMC_NOF_FP_PKT_HDR_TYPES
typedef SOC_TMC_FP_PKT_HDR_TYPE                                  SOC_PB_PMF_PGM_PKT_HDR_TYPE;

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

#define SOC_PB_PMF_PGM_MGMT_SOURCE_PP_PORT                     SOC_TMC_PMF_PGM_MGMT_SOURCE_PP_PORT
#define SOC_PB_PMF_PGM_MGMT_SOURCE_FP                          SOC_TMC_PMF_PGM_MGMT_SOURCE_FP
#define SOC_PB_PMF_NOF_PGM_MGMT_SOURCES                        SOC_TMC_PMF_NOF_PGM_MGMT_SOURCES
typedef SOC_TMC_PMF_PGM_MGMT_SOURCE                            SOC_PB_PMF_PGM_MGMT_SOURCE;

typedef SOC_TMC_PMF_PGM_MGMT_INFO                              SOC_PB_PMF_PGM_MGMT_INFO;
typedef SOC_TMC_PMF_PGM_MGMT_PARAMS                            SOC_PB_PMF_PGM_MGMT_PARAMS;
typedef SOC_TMC_PMF_PFG_INFO                                   SOC_PB_PMF_PFG_INFO;
typedef SOC_TMC_PMF_PGM_MGMT_NDX                               SOC_PB_PMF_PGM_MGMT_NDX;
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

/*
 * Update all the PMF-Programs with this PFG
 */
uint32
  soc_pb_pmf_pgm_mgmt_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_IN  uint32                          pfg_ndx
   );

/*
 * Update all the PMF-Programs with this PFG
 */
uint32
  soc_pb_pmf_pgm_mgmt_update(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pfg_ndx
   );

/*
 * PMF Program Management
 */
uint32
  soc_pb_pmf_pgm_mgmt_profile_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX               *pmf_pgm_mgmt_ndx,
    SOC_SAND_IN  uint32                          tm_ppp_ndx,
    SOC_SAND_OUT SOC_PB_PMF_PGM_MGMT_PARAMS            *pmf_pgm_mgmt_params,
    SOC_SAND_OUT uint32                          *internal_profile
   );

uint32
  soc_pb_pmf_pgm_mgmt_init(
    SOC_SAND_IN  int                    unit
  );

uint32
  soc_pb_pmf_pgm_packet_format_code_convert(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_PKT_HDR_TYPE      hdr_type_ndx,
    SOC_SAND_OUT uint32                     *pfc,
    SOC_SAND_OUT uint32                     *lookup_profile
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_pgm_mgmt_manage_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   If check, compute the max number of needed PMF-Programs
 *   and return success if inferior to the free PMF Programs
 *   (check also the number of free System-Header-Profiles
 *   and PP-Port-PMF-Profiles). If set, set the Program
 *   Selection, the Copy Engine and the Program Attributes
 *   according to its header type and Program parameters.
 * INPUT:
 *   SOC_SAND_IN  int              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_INFO   *mgmt_info -
 *     Parameters of the management: addition / removal,
 *     traffic affective or not, only check or set.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE pgm_source -
 *     Source of the request for adding / removing
 *     PMF-Programs.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params -
 *     Program parameter to add / remove (according to the
 *     Program source).
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success -
 *     Indicate if the program is added successfully.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_pgm_mgmt_manage_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_INFO   *mgmt_info,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE pgm_source,
    SOC_SAND_IN  uint32              param_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params_previous,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  );

uint32
  soc_pb_pmf_pgm_mgmt_manage_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_INFO   *mgmt_info,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE pgm_source,
    SOC_SAND_IN  uint32              param_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_pgm_mgmt_insert_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Insert a new element that can multiply the number of
 *   PMF-Programs: PP-Port or PFG.
 * INPUT:
 *   SOC_SAND_IN  int              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE pgm_source -
 *     Source of the request for adding / removing
 *     PMF-Programs.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX    *pgm_ndx -
 *     PMF-Program new element: PP-Port definition or
 *     Packet-Format-Group definition.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params -
 *     Program parameter to add / remove (according to the
 *     Program source).
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success -
 *     Indicate if the program is added successfully.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_pgm_mgmt_insert_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE pgm_source,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX    *pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  );

uint32
  soc_pb_pmf_pgm_mgmt_insert_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE pgm_source,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX    *pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_pgm_mgmt_select_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage the PMF-Program selection tables upon the HW
 *   insertion / removal. Atomic effect (only one line per
 *   table modified)
 * INPUT:
 *   SOC_SAND_IN  int              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32              pmf_pgm_ndx -
 *     PMF-Program. Range: 0 - 31.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_INFO   *mgmt_info -
 *     Parameters of the management: addition / removal,
 *     traffic affective or not, only check or set.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params -
 *     Program parameter to add / remove (according to the
 *     Program source).
 *   SOC_SAND_IN  uint8              is_pgm_to_add -
 *     If True, then the PMF-Program is added to the PMF
 *     Program selection, otherwise removed.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_pgm_mgmt_select_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX    *mgmt_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params,
    SOC_SAND_IN  uint8              is_pgm_to_add
  );

uint32
  soc_pb_pmf_pgm_mgmt_select_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX    *mgmt_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params,
    SOC_SAND_IN  uint8              is_pgm_to_add
  );

uint32
  SOC_PB_PMF_PGM_MGMT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_INFO *info
  );

uint32
  SOC_PB_PMF_PGM_MGMT_PARAMS_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *info
  );

void
  SOC_PB_PMF_PGM_MGMT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_MGMT_INFO *info
  );

void
  SOC_PB_PMF_PGM_MGMT_PARAMS_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_MGMT_PARAMS *info
  );

void
  SOC_PB_PMF_PGM_MGMT_NDX_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_MGMT_NDX *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_PMF_PGM_MGMT_SOURCE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE enum_val
  );

void
  SOC_PB_PMF_PGM_MGMT_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_INFO *info
  );

void
  SOC_PB_PMF_PGM_MGMT_PARAMS_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *info
  );

void
  SOC_PB_PMF_PGM_MGMT_NDX_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

uint32
  SOC_PB_PMF_PGM_MGMT_NDX_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PMF_PGM_MGMT_INCLUDED__*/
#endif

