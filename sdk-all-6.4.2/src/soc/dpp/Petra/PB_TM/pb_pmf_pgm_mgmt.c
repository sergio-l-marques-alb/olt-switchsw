/* $Id: pb_pmf_pgm_mgmt.c,v 1.11 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_pgm_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_pgm.h>
#include <soc/dpp/Petra/PB_TM/pb_profile.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_parser.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PMF_PGM_MGMT_PGM_SOURCE_MAX                         (SOC_PB_PMF_NOF_PGM_MGMT_SOURCES-1)
#define SOC_PB_PMF_PGM_MGMT_PFG_MAX                                (SOC_PB_PMF_NOF_LKP_PROFILE_IDS)

#define SOC_PB_PMF_PGM_NOF_PFGS_FP                                 (5) /* 5 PFG (0-4) + Raw in 5 */
#define SOC_PB_PMF_PGM_NOF_PARSER_PMF_PROFILES                     (16)
#define SOC_PB_PMF_PGM_NOF_PORT_PMF_PROFILES                       (8)

#define SOC_PB_PMF_PGM_PFC_PART_NONE                 (0)
#define SOC_PB_PMF_PGM_PFC_PART_E                    (1)
#define SOC_PB_PMF_PGM_PFC_PART_IPV4                 (2)
#define SOC_PB_PMF_PGM_PFC_PART_IPV6                 (3)
#define SOC_PB_PMF_PGM_PFC_PART_TRILL                (4)
#define SOC_PB_PMF_PGM_PFC_PART_MPLS1                (5)
#define SOC_PB_PMF_PGM_PFC_PART_MPLS2                (6)
#define SOC_PB_PMF_PGM_PFC_PART_MPLS3                (7)


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

typedef enum
{
  /* Default: always use */
  SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_ALWAYS = 0,
  /* For TM: if IS enabled */
  SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_TM_IS = 1,
  /* For TM: if PPH-Present enabled */
  SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_TM_PPH = 2,
  /* For TM: if IS and PPH-Present enabled */
  SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_TM_IS_PPH = 3,
  /* For Injected: also Ethernet */
  SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_ETH_INJECTED = 4,
  /*
   *  Number of types in PMF_PGM_PP_PORT_DUPLICATE
   */
  SOC_PB_NOF_PMF_PGM_PP_PORT_DUPLICATES = 5
}SOC_PB_PMF_PGM_PP_PORT_DUPLICATE;

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
   )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SUCCESS_FAILURE
    success = SOC_SAND_SUCCESS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_SET);

  /*
   * Set the PMF Program
   */
  switch (pp_port_info->header_type)
  {
  case SOC_PETRA_PORT_HEADER_TYPE_NONE:
  case SOC_PETRA_PORT_HEADER_TYPE_CPU:
  case SOC_PETRA_PORT_HEADER_TYPE_RAW:
  case SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW:
    res = soc_pb_pmf_low_level_raw_pgm_set_unsafe(
            unit,
            pmf_pgm_ndx,
            pp_port_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    break;

  case SOC_PETRA_PORT_HEADER_TYPE_ETH:
    res = soc_pb_pmf_low_level_eth_pgm_set_unsafe(
            unit,
            pfg_ndx,
            pmf_pgm_ndx,
            pp_port_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;

  case SOC_PETRA_PORT_HEADER_TYPE_TDM:
  case SOC_PETRA_PORT_HEADER_TYPE_STACKING:
    /*
     * Theoretically, the static configuration (FEMs)
     * can be set only once (for the 1st TDM PMF-Program)
     */
    res = soc_pb_pmf_low_level_stack_init_set_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = soc_pb_pmf_low_level_stack_pgm_set_unsafe(
            unit,
            pmf_pgm_ndx,
            pp_port_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;

  case SOC_PETRA_PORT_HEADER_TYPE_TM:
  case SOC_PETRA_PORT_HEADER_TYPE_PROG:
    res = soc_pb_pmf_low_level_tm_pgm_set_unsafe(
            unit,
            pmf_pgm_ndx,
            pp_port_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 100, exit);
  }

  if (success != SOC_SAND_SUCCESS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PGM_NOT_ENOUGH_ERR, 110, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_set()", 0, 0);
}


/*
 * Update all the PMF-Programs with this PFG
 */
uint32
  soc_pb_pmf_pgm_mgmt_update(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pfg_ndx
   )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_SW_DB_PMF_PGM_PROFILE
    pgm_profile;
  uint32
    pfg_lcl,
    pmf_pgm_ndx;
  SOC_PB_PORT_PP_PORT_INFO
    pp_port_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_UPDATE);

  /*
   * Update all the PMF Programs with the same PFG (expected: TM or FP)
   */
  for (pmf_pgm_ndx = 0; pmf_pgm_ndx <= SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX; ++pmf_pgm_ndx)
  {
    soc_pb_sw_db_pgm_profile_get(
      unit,
      pmf_pgm_ndx,
      &pgm_profile
    );
    /*
     * Convert to the PP info
     */
    SOC_PB_PORT_PP_PORT_INFO_clear(&pp_port_info);
    res = soc_pb_pmf_low_level_pgm_port_profile_decode(
            unit,
            pgm_profile.pp_port_info_encoded,
            FALSE,
            &pp_port_info,
            &pfg_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (
        (pgm_profile.pfg_ndx == pfg_ndx)
        && ((pp_port_info.header_type != SOC_PETRA_PORT_HEADER_TYPE_ETH)
            || (pp_port_info.header_type != SOC_PETRA_PORT_HEADER_TYPE_TM))
        )
    {
      res = soc_pb_pmf_pgm_mgmt_set(
              unit,
              pmf_pgm_ndx,
              &pp_port_info,
              pfg_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_update()", 0, 0);
}


uint32
  soc_pb_pmf_pgm_mgmt_profile_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX               *pmf_pgm_mgmt_ndx,
    SOC_SAND_IN  uint32                          tm_ppp_ndx,
    SOC_SAND_OUT SOC_PB_PMF_PGM_MGMT_PARAMS            *pmf_pgm_mgmt_params,
    SOC_SAND_OUT uint32                          *internal_profile
   )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_SW_DB_PMF_PGM_PROFILE
    pgm_profile;
  uint32
    pfg_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_PROFILE_GET);

  *internal_profile = soc_pb_sw_db_profile_ndx_pmf_pgm_get(unit, pmf_pgm_mgmt_ndx->pp_port_ndx, pmf_pgm_mgmt_ndx->pfg_ndx, tm_ppp_ndx);

  soc_pb_sw_db_pgm_profile_get(
    unit,
    *internal_profile,
    &pgm_profile
  );

  /*
   * Decode the PP port info
   */
  res = soc_pb_pmf_low_level_pgm_port_profile_decode(
          unit,
          pgm_profile.pp_port_info_encoded,
          FALSE,
          &(pmf_pgm_mgmt_params->pp_port),
          &(pfg_ndx)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * The SW DB indicates the current PFG
   */
  soc_pb_sw_db_pfg_info_get(
    unit,
    pmf_pgm_mgmt_ndx->pfg_ndx,
    &(pmf_pgm_mgmt_params->pfg_info)
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_profile_get()", pmf_pgm_mgmt_ndx->pp_port_ndx, pmf_pgm_mgmt_ndx->pfg_ndx);
}


/*
 * Conversion of the Packet-Format-Code
 */
uint32
  soc_pb_pmf_pgm_packet_format_code_convert(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_PKT_HDR_TYPE      hdr_type_ndx,
    SOC_SAND_OUT uint32                     *pfc,
    SOC_SAND_OUT uint32                     *lookup_profile
  )
{
  uint32
    threeb_first = 0,
    threeb_second = 4; /* User-Defined PFC */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_PACKET_FORMAT_CODE_CONVERT);

  *lookup_profile = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW;

  switch (hdr_type_ndx)
  {
  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_NONE;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_NONE;
  	break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_E;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_NONE;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_IPV4;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_NONE;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_IPV6;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_NONE;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_MPLS1_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS1;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_NONE;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_MPLS2_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS2;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_NONE;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_MPLS3_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS3;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_NONE;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS1_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS1;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_E;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS2_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS2;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_E;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS3_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS3;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_E;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS1_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS1;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_IPV4;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS2_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS2;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_IPV4;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS3_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS3;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_IPV4;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS1_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS1;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_IPV6;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS2_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS2;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_IPV6;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS3_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_MPLS3;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_IPV6;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV4_IPV4_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_IPV4;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_IPV4;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_IPV6_IPV4_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_IPV4;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_IPV6;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_ETH_TRILL_ETH:
    threeb_first = SOC_PB_PMF_PGM_PFC_PART_TRILL;
    threeb_second = SOC_PB_PMF_PGM_PFC_PART_E;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_RAW:
    threeb_first = SOC_PB_PARSER_PFC_RAW % 8;
    threeb_second = SOC_PB_PARSER_PFC_RAW >> 3;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_FTMH:
    threeb_first = SOC_PB_PARSER_PFC_FTMH % 8;
    threeb_second = SOC_PB_PARSER_PFC_FTMH >> 3;
    *lookup_profile = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_STACKING;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_TM:
    threeb_first = SOC_PB_PARSER_PFC_TM % 8;
    threeb_second = SOC_PB_PARSER_PFC_TM >> 3;
    *lookup_profile = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_TM_IS:
    threeb_first = SOC_PB_PARSER_PFC_TM_IS % 8;
    threeb_second = SOC_PB_PARSER_PFC_TM_IS >> 3;
    *lookup_profile = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_TM_PPH:
    threeb_first = SOC_PB_PARSER_PFC_TM_PPH % 8;
    threeb_second = SOC_PB_PARSER_PFC_TM_PPH >> 3;
    *lookup_profile = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM;
    break;

  case SOC_PB_PMF_PGM_PKT_HDR_TYPE_TM_IS_PPH:
    threeb_first = SOC_PB_PARSER_PFC_TM_IS_PPH % 8;
    threeb_second = SOC_PB_PARSER_PFC_TM_IS_PPH >> 3;
    *lookup_profile = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM;
    break;

  default:
    break;
  }

  *pfc = (threeb_second << 3) + threeb_first;
  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_packet_format_code_convert()", 0, 0);
}


/*
 * PMF Program init:
 *  Disable the PMF Program selection for table 6 & 7
 *  in case of Ethernet: all '0' and set '1' when the program is selected
 */
uint32
  soc_pb_pmf_pgm_mgmt_init(
    SOC_SAND_IN  int                    unit
  )
{
  uint32
    lookup_profile,
    pfc_hw,
    res = SOC_SAND_OK;
  SOC_PB_PMF_PGM_SELECTION_ENTRY
    entry_ndx;
  SOC_PB_PMF_PGM_VALIDITY_INFO
    pgm_validity;
  uint32
    port_pmf_profile_ndx,
    parser_pmf_profile_ndx,
    pfg_ndx,
    pkt_hdr_type_ndx;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT
    vlan_format_ndx;
  SOC_PB_PMF_PGM_PKT_HDR_TYPE
    pkt_hdr_type;
  SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA
    packet_format_code_profile_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_INIT);

  /*
   * Set the PGM selection table to 0
   */
  SOC_PB_PMF_PGM_SELECTION_ENTRY_clear(&entry_ndx);
  entry_ndx.type = SOC_TMC_PMF_PGM_SEL_TYPE_LLVP_PFC;
  for (pfg_ndx = 0; pfg_ndx < SOC_PB_PMF_PGM_NOF_PFGS_FP + 1 /* also raw */; pfg_ndx++)
  {
    entry_ndx.val.llvp_pfc.pmf_pro = pfg_ndx;
    for (vlan_format_ndx = 0; vlan_format_ndx < SOC_PB_NOF_ETHERNET_FRAME_VLAN_FORMATS_HW; vlan_format_ndx++)
    {
      SOC_PB_PMF_PGM_VALIDITY_INFO_clear(&pgm_validity);
      entry_ndx.val.llvp_pfc.llvp = vlan_format_ndx;
      res = soc_pb_pmf_pgm_selection_entry_set_unsafe(
              unit,
              &entry_ndx,
              &pgm_validity
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

  SOC_PB_PMF_PGM_SELECTION_ENTRY_clear(&entry_ndx);
  entry_ndx.type = SOC_PB_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO;
  for (parser_pmf_profile_ndx = 0; parser_pmf_profile_ndx < SOC_PB_PMF_PGM_NOF_PARSER_PMF_PROFILES; parser_pmf_profile_ndx++)
  {
    for (port_pmf_profile_ndx = 0; port_pmf_profile_ndx < SOC_PB_PMF_PGM_NOF_PORT_PMF_PROFILES; port_pmf_profile_ndx++)
    {
      entry_ndx.val.prsr_pmf.prsr = parser_pmf_profile_ndx;
      entry_ndx.val.prsr_pmf.port_pmf = port_pmf_profile_ndx;
      SOC_PB_PMF_PGM_VALIDITY_INFO_clear(&pgm_validity);
      res = soc_pb_pmf_pgm_selection_entry_set_unsafe(
              unit,
              &entry_ndx,
              &pgm_validity
            );
     SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    }
  }

  /*
   * Set the PFG profile: raw for all the Ethernet based PFC
   */
  for (pkt_hdr_type_ndx = 0; pkt_hdr_type_ndx < SOC_PB_PMF_PGM_NOF_PKT_HDR_TYPES; pkt_hdr_type_ndx++)
  {
    pkt_hdr_type = 1 << pkt_hdr_type_ndx;
    res = soc_pb_pmf_pgm_packet_format_code_convert(
            unit,
            pkt_hdr_type,
            &pfc_hw,
            &lookup_profile
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /*
     * Set the mapping table
     */
    packet_format_code_profile_tbl.packet_format_code_profile = lookup_profile;
    res = soc_pb_ihb_packet_format_code_profile_tbl_set_unsafe(
            unit,
            pfc_hw,
            &packet_format_code_profile_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_init()", 0, 0);
}

/*
 * Parser-PMF-Profile conversion
 */
STATIC
  uint32
    soc_pb_pmf_low_level_parser_pmf_profile_get(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO               *pp_port_info,
      SOC_SAND_OUT uint32                           *parser_pmf_profile
    )
{
  uint32
    fld_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PARSER_PMF_PROFILE_GET);

  switch (pp_port_info->header_type)
  {
  case SOC_PETRA_PORT_HEADER_TYPE_NONE:
  case SOC_PETRA_PORT_HEADER_TYPE_CPU:
  case SOC_PETRA_PORT_HEADER_TYPE_RAW:
  case SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW:
    fld_val = SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_RAW;
  	break;
  case SOC_PETRA_PORT_HEADER_TYPE_ETH:
    fld_val = SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_PP;
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_TDM:
  case SOC_PETRA_PORT_HEADER_TYPE_STACKING:
    fld_val = SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_FTMH;
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_TM:
  case SOC_PETRA_PORT_HEADER_TYPE_PROG:
    fld_val = SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(
                pp_port_info->is_tm_pph_present_enabled,
                pp_port_info->is_tm_ing_shaping_enabled);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  *parser_pmf_profile = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_parser_pmf_profile_get()", 0, 0);
}

/*
 * Other element validate
 */
STATIC
  uint32
    soc_pb_pmf_low_level_other_element_validate(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PB_PMF_PGM_PP_PORT_DUPLICATE       duplicate_reason_ndx,
      SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE             pgm_source,
      SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO               *pp_port_info,
      SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX                *pgm_mgmt_ndx,
      SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS             *pgm_params,
      SOC_SAND_OUT uint8                          *is_valid
    )
{
  uint32
    res;
  uint8
    valid = FALSE;
  SOC_PB_PMF_PFG_INFO
    pfg_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_OTHER_ELEMENT_VALIDATE);

  if (
      (pgm_mgmt_ndx->pfg_ndx >= SOC_PB_PMF_NOF_LKP_PROFILE_IDS)
      || (pgm_mgmt_ndx->pp_port_ndx >= SOC_PB_PORT_NOF_PP_PORTS)
     )
  {
    valid = FALSE;
  }
  else
  {
    if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_FP)
    {
      SOC_PETRA_COPY(&pfg_info, &(pgm_params->pfg_info), SOC_PB_PMF_PFG_INFO, 1);
    }
    else /* PP-Port source */
    {
      /*
       * Get the PFG info
       */
      SOC_TMC_PMF_PFG_INFO_clear(&pfg_info);
      soc_pb_sw_db_pfg_info_get(
        unit,
        pgm_mgmt_ndx->pfg_ndx,
        &(pfg_info)
       );
    }

    /*
     * Check the PP-Port owns to the PFG bitmap
     */
    if (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_ETH)
    {
      if (
           ((pgm_mgmt_ndx->pp_port_ndx < SOC_SAND_NOF_BITS_IN_UINT32)
              && (pfg_info.pp_ports_bmp.arr[0] & (1 << pgm_mgmt_ndx->pp_port_ndx)))
        || ((pgm_mgmt_ndx->pp_port_ndx >= SOC_SAND_NOF_BITS_IN_UINT32)
              && (pfg_info.pp_ports_bmp.arr[1] & (1 << (pgm_mgmt_ndx->pp_port_ndx - SOC_SAND_NOF_BITS_IN_UINT32))))
         )
      {
        valid = TRUE;
      }
      else if (pgm_mgmt_ndx->pfg_ndx == SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW)
      {
        valid = TRUE;
      }
    }
    else if (
              (  (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_RAW)
              || (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW)
              || (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_CPU))
           && (pgm_mgmt_ndx->pfg_ndx == SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW)
           )
    {
      valid = TRUE;
    }
    else if (
              (  (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_TM)
              || (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_INJECTED))
           && (pgm_mgmt_ndx->pfg_ndx == SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM)
           )
    {
      valid = TRUE;
    }
    else if (
              (  (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_TDM)
              || (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_STACKING))
           && (pgm_mgmt_ndx->pfg_ndx == SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_STACKING)
           )
    {
      valid = TRUE;
    }
  }

  /*
   * Handle the replications
   */
  if ((duplicate_reason_ndx != SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_ALWAYS) && (valid == TRUE))
  {
    valid = FALSE;

    if ((     (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_TM)
              || (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_INJECTED))
           && (duplicate_reason_ndx < SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_ETH_INJECTED)
           && (pgm_mgmt_ndx->pfg_ndx == SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM))
    {
      if ((pp_port_info->is_tm_ing_shaping_enabled == TRUE)
           && (duplicate_reason_ndx == SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_TM_IS))
      {
        valid = TRUE;
      }
      else if ((pp_port_info->is_tm_pph_present_enabled == TRUE)
              && (duplicate_reason_ndx == SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_TM_PPH))
      {
        valid = TRUE;
      }
      else if ((pp_port_info->is_tm_pph_present_enabled == TRUE)
              && (pp_port_info->is_tm_ing_shaping_enabled == TRUE)
              && (duplicate_reason_ndx == SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_TM_IS_PPH))
      {
        valid = TRUE;
      }
    }
  }
  else if ((pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_INJECTED)
        && (duplicate_reason_ndx == SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_ETH_INJECTED)
        && (pgm_mgmt_ndx->pfg_ndx == SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW))
  {
    valid = TRUE;
  }

  *is_valid = valid;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_other_element_validate()", 0, 0);
}


/*
 * PP-Port related profile addition / removal
 */
STATIC
  uint32
    soc_pb_pmf_pgm_mgmt_pp_port_profiles_remove(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  uint32                          pp_port_ndx,
      SOC_SAND_IN  uint32                          pfg_ndx,
      SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO               *pp_port_info,
      SOC_SAND_IN  uint32                          pmf_pgm_ndx
    )
{
  uint32
    data,
    data_indx,
    res = SOC_SAND_OK;
  SOC_PB_PROFILE_NDX
    profile_ndx,
    profile_ndx2;
  SOC_PB_PROFILE_PARAMS
    profile_params;
  uint8
    last_appear,
    pph_in_pp_info,
    ing_sha_in_pp_info,
    ing_shaping_enabled,
    pph_present_enabled;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_PP_PORT_PROFILES_REMOVE);

  /*
   *  Get a PMF Program: before setting it, remove all the related PMF-Programs
   *     with or without PPH-Present (if needed): possible multiple PMF-Programs
   *     for the same ports, so the removal management is explicit
   */
  SOC_PB_PROFILE_NDX_clear(&profile_ndx);
  profile_ndx.pp_port_ndx = pp_port_ndx;
  SOC_PB_PROFILE_PARAMS_clear(&profile_params);
  SOC_PETRA_COPY(&(profile_params.pp_port_info), pp_port_info, SOC_PB_PORT_PP_PORT_INFO, 1);

  ing_sha_in_pp_info = profile_params.pp_port_info.is_tm_ing_shaping_enabled;
  pph_in_pp_info = profile_params.pp_port_info.is_tm_pph_present_enabled;

  for (ing_shaping_enabled = FALSE; ing_shaping_enabled <= TRUE; ing_shaping_enabled++)
  {
    if ((ing_sha_in_pp_info == FALSE) && (ing_shaping_enabled == TRUE))
    {
      continue;
    }
    for (pph_present_enabled = FALSE; pph_present_enabled <= TRUE; pph_present_enabled++)
    {
      if ((pph_in_pp_info == FALSE) && (pph_present_enabled == TRUE))
      {
        continue;
      }
      profile_params.pp_port_info.is_tm_ing_shaping_enabled = ing_shaping_enabled;
      profile_params.pp_port_info.is_tm_pph_present_enabled = pph_present_enabled;

      /*
       * Remove the other profiles: System-Header and Port-PMF-Profiles
       */
      res = soc_pb_pmf_low_level_pgm_port_profile_encode(
              unit,
              &(profile_params.pp_port_info),
              pfg_ndx,
              TRUE,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

      res = soc_pb_sw_db_multiset_remove(
              unit,
              SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE,
              data,
              &data_indx,
              &last_appear
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
    }
  }

  SOC_PB_PROFILE_NDX_clear(&profile_ndx2);
  profile_ndx2.pmf_pgm_ndx = pmf_pgm_ndx;
  res = soc_pb_profile_remove(
          unit,
          SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE,
          &profile_ndx2
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_pp_port_profiles_remove()", 0, 0);
}

/*********************************************************************
*     If check, compute the max number of needed PMF-Programs
 *     and return success if inferior to the free PMF Programs
 *     (check also the number of free System-Header-Profiles
 *     and PP-Port-PMF-Profiles). If set, set the Program
 *     Selection, the Copy Engine and the Program Attributes
 *     according to its header type and Program parameters.
 *     Details: in the H file. (search for prototype)
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
  )
{
  uint32
    data_indx,
    encoded_pp_info,
    internal_hw_ndx,
    res = SOC_SAND_OK;
  uint32
    other_element_ndx;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    *profile_params_curr = NULL,
    *profile_params = NULL;
  uint8
    last_appear,
    is_valid_couple=0;
  SOC_PB_PMF_PGM_PP_PORT_DUPLICATE
    duplicate_reason_ndx;
  SOC_PB_PORT_PP_PORT_INFO
    pp_port_info_orig;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_MANAGE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mgmt_info);
  SOC_SAND_CHECK_NULL_INPUT(pgm_params);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PROFILE_NDX_clear(&profile_ndx);
  SOC_PETRA_ALLOC(profile_params_curr, SOC_PB_PROFILE_PARAMS, 1);
  SOC_PETRA_ALLOC(profile_params, SOC_PB_PROFILE_PARAMS, 1);
  SOC_PB_PROFILE_PARAMS_clear(profile_params);
  SOC_PB_PROFILE_PARAMS_clear(profile_params_curr);
  /*
   * 1. Verify
   */
  res = soc_pb_pmf_pgm_mgmt_manage_verify(
          unit,
          mgmt_info,
          pgm_source,
          param_ndx,
          pgm_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * In case of Count (only verify), add to the SW DB profiles,
   * and revert in case of failure
   */
  if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_FP)
  {
    SOC_PETRA_COPY(&(profile_params->pmf_pgm_mgmt_params.pfg_info), &(pgm_params->pfg_info), SOC_PB_PMF_PFG_INFO, 1);
  } /* End of FP */
  else
  {
    SOC_PETRA_COPY(&(pp_port_info_orig), &(pgm_params->pp_port), SOC_PB_PORT_PP_PORT_INFO, 1);
  }

  *success = SOC_SAND_SUCCESS;
  for(other_element_ndx = 0;  other_element_ndx < SOC_SAND_MAX(SOC_PB_PORT_NOF_PP_PORTS, SOC_PB_PMF_NOF_LKP_PROFILE_IDS); other_element_ndx++)
  {
    for(duplicate_reason_ndx = 0;  duplicate_reason_ndx < SOC_PB_NOF_PMF_PGM_PP_PORT_DUPLICATES; duplicate_reason_ndx++)
    {
      if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_FP)
      {
        profile_ndx.pmf_pgm_mgmt_ndx.pfg_ndx = param_ndx;
        profile_ndx.pmf_pgm_mgmt_ndx.pp_port_ndx = other_element_ndx;
        res = soc_pb_port_pp_port_get_unsafe(
                unit,
                other_element_ndx,
                &(pp_port_info_orig)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      }
      else
      {
        profile_ndx.pmf_pgm_mgmt_ndx.pp_port_ndx = param_ndx;
        /* 
         * Scan the highest PFGs first: the Raw must 
         * be scanned first to be overridden by a valid PFG if exist 
         */
        profile_ndx.pmf_pgm_mgmt_ndx.pfg_ndx = SOC_PB_PMF_NOF_LKP_PROFILE_IDS - other_element_ndx - 1;
      }
      /*
       * Check the PP-Port owns to the PFG bitmap
       */
      res = soc_pb_pmf_low_level_other_element_validate(
              unit,
              duplicate_reason_ndx,
              pgm_source,
              &pp_port_info_orig,
              &(profile_ndx.pmf_pgm_mgmt_ndx),
              pgm_params,
              &is_valid_couple
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (is_valid_couple == TRUE)
      {
        if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_PP_PORT)
        {
          soc_pb_sw_db_pfg_info_get(
            unit,
            profile_ndx.pmf_pgm_mgmt_ndx.pfg_ndx,
            &(profile_params->pmf_pgm_mgmt_params.pfg_info)
          );
        }

        SOC_PETRA_COPY(&(profile_params->pmf_pgm_mgmt_params.pp_port), &(pp_port_info_orig), SOC_PB_PORT_PP_PORT_INFO, 1);
        switch (duplicate_reason_ndx)
        {
        case SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_ALWAYS:
          if ((pp_port_info_orig.header_type == SOC_PETRA_PORT_HEADER_TYPE_TM)
              || (pp_port_info_orig.header_type == SOC_PETRA_PORT_HEADER_TYPE_INJECTED))
          {
            profile_params->pmf_pgm_mgmt_params.pp_port.header_type = SOC_PETRA_PORT_HEADER_TYPE_TM;
            profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_ing_shaping_enabled = FALSE;
            profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_pph_present_enabled = FALSE;
          }
          break;

        case SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_TM_IS:
          profile_params->pmf_pgm_mgmt_params.pp_port.header_type = SOC_PETRA_PORT_HEADER_TYPE_TM;
          profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_ing_shaping_enabled = TRUE;
          profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_pph_present_enabled = FALSE;
        	break;
        case SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_TM_PPH:
          profile_params->pmf_pgm_mgmt_params.pp_port.header_type = SOC_PETRA_PORT_HEADER_TYPE_TM;
          profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_ing_shaping_enabled = FALSE;
          profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_pph_present_enabled = TRUE;
          break;
        case SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_TM_IS_PPH:
          profile_params->pmf_pgm_mgmt_params.pp_port.header_type = SOC_PETRA_PORT_HEADER_TYPE_TM;
          profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_ing_shaping_enabled = TRUE;
          profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_pph_present_enabled = TRUE;
          break;
        case SOC_PB_PMF_PGM_PP_PORT_DUPLICATE_ETH_INJECTED:
          profile_params->pmf_pgm_mgmt_params.pp_port.header_type = SOC_PETRA_PORT_HEADER_TYPE_ETH;
          profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_ing_shaping_enabled = FALSE;
          profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_pph_present_enabled = FALSE;
          break;

        /* must default. Otherwise - compilation error */
        /* coverity[dead_error_begin:FALSE] */
        default:
          break;
        }

        if ((mgmt_info->is_to_set == FALSE) && (mgmt_info->is_addition == TRUE))
        {
          /*
           * SW addition: only the SW DB profile
           */
          res = soc_pb_profile_add(
                  unit,
                  SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE,
                  &profile_ndx,
                  profile_params,
                  !mgmt_info->is_2nd_iter, /* If 1st add, do not remove previous SW DB profiles, but for restore, do remove */
                  success
               );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          if (*success != SOC_SAND_SUCCESS)
          {
            break;
          }
        } /* End of SW DB addition */
        else
        {
          if (mgmt_info->is_addition == FALSE)
          {
            /*
             * In case of SW removal, only the profile
             * For HW removal, both the PMF-Program selection and
             * the SW DB profile
             */

            res = soc_pb_pmf_low_level_pgm_port_profile_encode(
                    unit,
                    &(pgm_params_previous->pp_port),
                    profile_ndx.pmf_pgm_mgmt_ndx.pfg_ndx,
                    FALSE,
                    &encoded_pp_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

            /* Remove the old program */
            res = soc_pb_sw_db_multiset_remove(
                    unit,
                    SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE,
                    encoded_pp_info,
                    &data_indx, /* Previous PMF-Program */
                    &last_appear
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

            if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_PP_PORT)
            {
              res = soc_pb_pmf_pgm_mgmt_pp_port_profiles_remove(
                      unit,
                      param_ndx,
                      profile_ndx.pmf_pgm_mgmt_ndx.pfg_ndx,
                      &(pgm_params->pp_port),
                      data_indx
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
            }

            /* Remove the old PMF-Program selection if necessary (last appearance) */
            if ((last_appear == TRUE) && (mgmt_info->is_to_set == TRUE))
            {
              res = soc_pb_pmf_pgm_mgmt_select_unsafe(
                      unit,
                      data_indx,
                      &(profile_ndx.pmf_pgm_mgmt_ndx),
                      &(profile_params->pmf_pgm_mgmt_params),
                      FALSE
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);
            }
          }/* End of SW DB Remove */
          else if ((mgmt_info->is_to_set == TRUE) && (mgmt_info->is_addition == TRUE))
          {
            profile_ndx.pmf_pgm_ndx = SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(
              profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_pph_present_enabled,
              profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_ing_shaping_enabled);
            res = soc_pb_profile_get(
                    unit,
                    SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE,
                    &profile_ndx,
                    profile_params_curr,
                    &internal_hw_ndx /* PMF-Program */
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 67, exit);

            /*
             * The PMF-Program attributes are HW set in the Profile addition already
             * Only the PMF Program selection will be updated at this step
             */
            res = soc_pb_pmf_pgm_mgmt_select_unsafe(
                    unit,
                    internal_hw_ndx,
                    &(profile_ndx.pmf_pgm_mgmt_ndx),
                    &(profile_params->pmf_pgm_mgmt_params),
                    TRUE
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
          }/* HW addition */
        }
      }
    }
  } /* End of loop on PP-Ports / PFG-ndx */

exit:
  SOC_PETRA_FREE(profile_params_curr);
  SOC_PETRA_FREE(profile_params);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_manage_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_pgm_mgmt_manage_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_INFO   *mgmt_info,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE pgm_source,
    SOC_SAND_IN  uint32              param_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_MANAGE_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_MGMT_INFO, mgmt_info, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pgm_source, SOC_PB_PMF_PGM_MGMT_PGM_SOURCE_MAX, SOC_PB_PMF_PGM_MGMT_PGM_SOURCE_OUT_OF_RANGE_ERR, 20, exit);
  if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_FP)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(param_ndx, SOC_PB_PMF_PGM_MGMT_PFG_MAX, SOC_PB_PMF_PGM_MGMT_PFG_OUT_OF_RANGE_ERR, 25, exit);
  }
  else
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(param_ndx, SOC_PB_PORT_NOF_PP_PORTS - 1, SOC_PB_PMF_PGM_MGMT_PFG_OUT_OF_RANGE_ERR, 26, exit);
  }
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_MGMT_PARAMS, pgm_params, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_manage_verify()", 0, 0);
}

/*********************************************************************
*     Insert a new element that can multiply the number of
 *     PMF-Programs: PP-Port or PFG.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_pgm_mgmt_insert_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE pgm_source,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX    *pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  )
{
  uint32
    param_ndx,
    res = SOC_SAND_OK;
  SOC_PB_PMF_PGM_MGMT_INFO
    mgmt_info;
  SOC_PB_PMF_PGM_MGMT_PARAMS
    *pgm_params_lcl = NULL;
  SOC_SAND_SUCCESS_FAILURE
    success_rmv;
  SOC_PB_PMF_PGM_MGMT_PARAMS
    pgm_params_previous;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_INSERT_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pgm_ndx);
  SOC_SAND_CHECK_NULL_INPUT(pgm_params);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   * Verify
   */
  res = soc_pb_pmf_pgm_mgmt_insert_verify(
          unit,
          pgm_source,
          pgm_ndx,
          pgm_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * 1. Save the previous PFG info and set this one
   */
  SOC_PB_PMF_PGM_MGMT_PARAMS_clear(&pgm_params_previous);
  if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_FP)
  {
    soc_pb_sw_db_pfg_info_get(
      unit,
      pgm_ndx->pfg_ndx,
      &(pgm_params_previous.pfg_info)
     );

    soc_pb_sw_db_pfg_info_set(
      unit,
      pgm_ndx->pfg_ndx,
      &(pgm_params->pfg_info)
     );
  }
  else
  {
    res = soc_pb_port_pp_port_get_unsafe(
            unit,
            pgm_ndx->pp_port_ndx,
            &(pgm_params_previous.pp_port)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }

  /*
   * 2. Verify if there is enough resources (PMF-Programs)
   *    for this PFG info / PP-Port
   */
  SOC_PB_PMF_PGM_MGMT_INFO_clear(&mgmt_info);
  mgmt_info.is_to_set = FALSE;
  mgmt_info.is_addition = TRUE;
  SOC_PETRA_ALLOC(pgm_params_lcl, SOC_PB_PMF_PGM_MGMT_PARAMS, 1);
  SOC_PB_PMF_PGM_MGMT_PARAMS_clear(pgm_params_lcl);
  if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_FP)
  {
    SOC_PETRA_COPY(&(pgm_params_lcl->pfg_info), &(pgm_params->pfg_info), SOC_PB_PMF_PFG_INFO, 1);
    param_ndx = pgm_ndx->pfg_ndx;
  }
  else
  {
    SOC_PETRA_COPY(&(pgm_params_lcl->pp_port), &(pgm_params->pp_port), SOC_PB_PORT_PP_PORT_INFO, 1);
    param_ndx = pgm_ndx->pp_port_ndx;
  }
  res = soc_pb_pmf_pgm_mgmt_manage_unsafe(
          unit,
          &mgmt_info,
          pgm_source,
          param_ndx,
          pgm_params_lcl,
          &pgm_params_previous,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * 3. In case of failure, restore previous PFG info
   */
  if (*success != SOC_SAND_SUCCESS)
  {
    if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_FP)
    {
      soc_pb_sw_db_pfg_info_set(
        unit,
        pgm_ndx->pfg_ndx,
        &(pgm_params_previous.pfg_info)
      );
    }

    /*
     * Remove the new configuration
     */
    mgmt_info.is_to_set = FALSE;
    mgmt_info.is_addition = FALSE; /* remove */
    res = soc_pb_pmf_pgm_mgmt_manage_unsafe(
            unit,
            &mgmt_info,
            pgm_source,
            param_ndx,
            pgm_params_lcl,
            &pgm_params_previous,
            &success_rmv
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    /*
     * Add the old one (one recursion at the most)
     */
    if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_FP)
    {
      SOC_PETRA_COPY(&(pgm_params_lcl->pfg_info), &(pgm_params_previous.pfg_info), SOC_PB_PMF_PFG_INFO, 1);
    }
    else
    {
      SOC_PETRA_COPY(&(pgm_params_lcl->pp_port), &(pgm_params_previous.pp_port), SOC_PB_PORT_PP_PORT_INFO, 1);
    }
    mgmt_info.is_to_set = FALSE;
    mgmt_info.is_addition = TRUE; /* SW DB add */
    mgmt_info.is_2nd_iter = TRUE; 
    res = soc_pb_pmf_pgm_mgmt_manage_unsafe(
            unit,
            &mgmt_info,
            pgm_source,
            param_ndx,
            pgm_params_lcl,
            &pgm_params_previous,
            &success_rmv
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (success_rmv != SOC_SAND_SUCCESS)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_PGM_MGMT_RESTORE_ERR, 40, exit);
    }

    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /*
   * 4. In case of success, set it (get the SW DB profile, set it, and
   *    then remove the previous PFG profiles - SW DB and HW)
   */
  mgmt_info.is_to_set = TRUE;
  mgmt_info.is_addition = TRUE;
  res = soc_pb_pmf_pgm_mgmt_manage_unsafe(
          unit,
          &mgmt_info,
          pgm_source,
          param_ndx,
          pgm_params_lcl,
          &pgm_params_previous,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  mgmt_info.is_to_set = ((pgm_params->pp_port.header_type == SOC_PETRA_PORT_HEADER_TYPE_TM)
                      || (pgm_params->pp_port.header_type == SOC_PETRA_PORT_HEADER_TYPE_INJECTED))?
                          FALSE : TRUE;    /* Modify also the PMF-Pgm selection if last appear (except if the program is TM) */
  mgmt_info.is_addition = FALSE; /* remove */
  if (pgm_source == SOC_PB_PMF_PGM_MGMT_SOURCE_FP)
  {
    SOC_PETRA_COPY(&(pgm_params_lcl->pfg_info), &(pgm_params_previous.pfg_info), SOC_PB_PMF_PFG_INFO, 1);
  }
  else
  {
    SOC_PETRA_COPY(&(pgm_params_lcl->pp_port), &(pgm_params_previous.pp_port), SOC_PB_PORT_PP_PORT_INFO, 1);
  }
  res = soc_pb_pmf_pgm_mgmt_manage_unsafe(
          unit,
          &mgmt_info,
          pgm_source,
          param_ndx,
          pgm_params_lcl,
          &pgm_params_previous,
          &success_rmv
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    

exit:
  SOC_PETRA_FREE(pgm_params_lcl);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_insert_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_pgm_mgmt_insert_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE pgm_source,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX    *pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_INSERT_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pgm_source, SOC_PB_PMF_PGM_MGMT_PGM_SOURCE_MAX, SOC_PB_PMF_PGM_MGMT_PGM_SOURCE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_MGMT_NDX, pgm_ndx, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_MGMT_PARAMS, pgm_params, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_insert_verify()", 0, 0);
}

/*********************************************************************
*     Manage the PMF-Program selection tables upon the HW
 *     insertion / removal. Atomic effect (only one line per
 *     table modified)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_pgm_mgmt_select_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX    *mgmt_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params,
    SOC_SAND_IN  uint8              is_pgm_to_add
  )
{
  uint32
    pfc_profile_previous = 0,
    pfc_profile_new = 0,
    lookup_profile,
    pfc_hw,
    parser_pmf_profile=0,
    port_pmf_profile,
    res = SOC_SAND_OK;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    profile_params;
  SOC_PB_PMF_PGM_SELECTION_ENTRY
    entry_ndx;
  SOC_PB_PMF_PGM_VALIDITY_INFO
    pgm_validity;
  uint32
    pmf_pgm_ndx_valid,
    parser_pmf_profile_ndx,
    port_pmf_profile_ndx,
    pkt_hdr_type_ndx;
  uint8
    ing_shaping_enabled,
    pph_present_enabled;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT
    vlan_format_ndx;
  SOC_PB_PMF_PGM_PKT_HDR_TYPE
    pkt_hdr_type;
  SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA
    packet_format_code_profile_tbl;
  SOC_PB_PMF_PGM_INFO
    pmf_pgm_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_SELECT_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mgmt_ndx);
  SOC_SAND_CHECK_NULL_INPUT(pgm_params);

  /*
   * Verify
   */
  res = soc_pb_pmf_pgm_mgmt_select_verify(
          unit,
          pmf_pgm_ndx,
          mgmt_ndx,
          pgm_params,
          is_pgm_to_add
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set the right tables
   */
    /*
   *  Write the PMF Program Selection for this PP-Port:
   *  Program Selection: get the PP-Port-PMF-Profile and the
   *  the Parser-PMF-Profile and set such a configuration to this PMF-Program.
   *  Assumption: the Parser-PMF-Profile and the System
   *              Header Profile are already obtained.
   */
  SOC_PB_PROFILE_NDX_clear(&profile_ndx);
  SOC_PB_PROFILE_PARAMS_clear(&profile_params);
  profile_ndx.pp_port_ndx = mgmt_ndx->pp_port_ndx;
  res = soc_pb_profile_get(
          unit,
          SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE,
          &profile_ndx,
          &profile_params,
          &port_pmf_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pmf_low_level_parser_pmf_profile_get(
          unit,
          &(pgm_params->pp_port),
          &parser_pmf_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PB_PMF_PGM_SELECTION_ENTRY_clear(&entry_ndx);
  SOC_PB_PMF_PGM_VALIDITY_INFO_clear(&pgm_validity);
  entry_ndx.type = SOC_PB_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO;
  entry_ndx.val.prsr_pmf.prsr = parser_pmf_profile;
  entry_ndx.val.prsr_pmf.port_pmf = port_pmf_profile;
  for (parser_pmf_profile_ndx = 0; parser_pmf_profile_ndx < SOC_PB_PMF_PGM_NOF_PARSER_PMF_PROFILES; parser_pmf_profile_ndx++)
  {
    for (port_pmf_profile_ndx = 0; port_pmf_profile_ndx < SOC_PB_PMF_PGM_NOF_PORT_PMF_PROFILES; port_pmf_profile_ndx++)
    {
      if (is_pgm_to_add == TRUE)
      {
        /* Take only the Parser and Port PMF profiles found */
        if ((port_pmf_profile_ndx != port_pmf_profile)
            || (parser_pmf_profile_ndx != parser_pmf_profile))
        {
          continue;
        }
      }

      res = soc_pb_pmf_pgm_selection_entry_get_unsafe(
              unit,
              &entry_ndx,
              &pgm_validity
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      /*
       * In case of TM, single selection: one PMF-Program is valid per Table 6 line
       * For Ethernet, multiple PMF-Programs per Table 6 line
       */
      if ((pgm_params->pp_port.header_type == SOC_PETRA_PORT_HEADER_TYPE_TM) &&  (is_pgm_to_add == TRUE))
      {
        SOC_PB_PMF_PGM_VALIDITY_INFO_clear(&pgm_validity);
      }
      pgm_validity.is_pgm_valid[pmf_pgm_ndx] = is_pgm_to_add;  /* TRUE for adding PMF-Programs,
                                                                  False for removal   */
      res = soc_pb_pmf_pgm_selection_entry_set_unsafe(
              unit,
              &entry_ndx,
              &pgm_validity
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
    }
  }

  /*
   * In case of TM PMF-Program, select this Program also
   * for the other Parser-PMF-Profiles (IS, PPH-Present)
   * if it is False
   * Assumption the False IS, PPH-Present TM PMF-Programs are set before
   */
  if (pgm_params->pp_port.header_type == SOC_PETRA_PORT_HEADER_TYPE_TM)
  {
    for (ing_shaping_enabled = FALSE; ing_shaping_enabled <= TRUE; ing_shaping_enabled++)
    {
      if ((pgm_params->pp_port.is_tm_ing_shaping_enabled == TRUE) && (ing_shaping_enabled == FALSE))
      {
        continue;
      }
      for (pph_present_enabled = FALSE; pph_present_enabled <= TRUE; pph_present_enabled++)
      {
        if ((pgm_params->pp_port.is_tm_pph_present_enabled == TRUE) && (pph_present_enabled == FALSE))
        {
          continue;
        }
        if (
            (ing_shaping_enabled == pgm_params->pp_port.is_tm_ing_shaping_enabled)
            && (pph_present_enabled == pgm_params->pp_port.is_tm_pph_present_enabled)
           )
        {
          /* Already set */
          continue;
        }
        
        entry_ndx.val.prsr_pmf.prsr = SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(pph_present_enabled, ing_shaping_enabled);
        res = soc_pb_pmf_pgm_selection_entry_set_unsafe(
                unit,
                &entry_ndx,
                &pgm_validity
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
      }
    }
  }

  /*
   * Set the Table7 {LLVP, PFC} with the following rule:
   * the PFC-Profile = PFG index = Lookup-Profile
   * the LLVP mapping is according to the PFG Info
   */
  if (
      (pgm_params->pp_port.header_type == SOC_PETRA_PORT_HEADER_TYPE_ETH)
      || (pgm_params->pp_port.header_type == SOC_PETRA_PORT_HEADER_TYPE_RAW)
      || (pgm_params->pp_port.header_type == SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW)
     )
  {
    for (pkt_hdr_type_ndx = 0; pkt_hdr_type_ndx < SOC_PB_PMF_PGM_NOF_PKT_HDR_TYPES; pkt_hdr_type_ndx++)
    {
      /*
       * Get the PFG profile: if already mapped, take this profile, otherwise
       * map them to the right PFG (current one) and retrieve the Program selection
       */
      pkt_hdr_type = 1 << pkt_hdr_type_ndx;
      res = soc_pb_pmf_pgm_packet_format_code_convert(
              unit,
              pkt_hdr_type,
              &pfc_hw,
              &lookup_profile
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      res = soc_pb_ihb_packet_format_code_profile_tbl_get_unsafe(
              unit,
              pfc_hw,
              &packet_format_code_profile_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
      pfc_profile_previous = packet_format_code_profile_tbl.packet_format_code_profile;
      pfc_profile_new = pfc_profile_previous;

      if ((is_pgm_to_add == TRUE) && (mgmt_ndx->pfg_ndx != SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW))
      {
        if (((pgm_params->pfg_info.hdr_format_bmp) & pkt_hdr_type) == 0)
        {
          continue;
        }
        /*
         * Set the mapping table
         */
        packet_format_code_profile_tbl.packet_format_code_profile = mgmt_ndx->pfg_ndx;
        pfc_profile_new = mgmt_ndx->pfg_ndx;
        res = soc_pb_ihb_packet_format_code_profile_tbl_set_unsafe(
                unit,
                pfc_hw,
                &packet_format_code_profile_tbl
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
      }

      /* Do not continue in case of Packet-Format-Code not Ethernet */
      if (pfc_profile_new > SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW)
      {
        continue;
      }

      SOC_PB_PMF_PGM_SELECTION_ENTRY_clear(&entry_ndx);
      SOC_PB_PMF_PGM_VALIDITY_INFO_clear(&pgm_validity);
      entry_ndx.type = SOC_TMC_PMF_PGM_SEL_TYPE_LLVP_PFC;

      for (vlan_format_ndx = 0; vlan_format_ndx < SOC_PB_NOF_ETHERNET_FRAME_VLAN_FORMATS_HW; vlan_format_ndx++)
      {
        entry_ndx.val.llvp_pfc.pmf_pro = pfc_profile_previous;
        entry_ndx.val.llvp_pfc.llvp = vlan_format_ndx;
        res = soc_pb_pmf_pgm_selection_entry_get_unsafe(
                unit,
                &entry_ndx,
                &pgm_validity
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);
        entry_ndx.val.llvp_pfc.pmf_pro = pfc_profile_new;
        pgm_validity.is_pgm_valid[pmf_pgm_ndx] = is_pgm_to_add;
        if ((mgmt_ndx->pfg_ndx != SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW) && (is_pgm_to_add == TRUE))
        {
          if (((pgm_params->pfg_info.vlan_tag_structure_bmp) & (1 << vlan_format_ndx)) == 0)
          {
            /*
             * Assumption the table is always 0, except when adding programs
             * Set the table to 0 but go through all the VLAN Tag in case
             * of a transfer of a PFC profile
             */
            pgm_validity.is_pgm_valid[pmf_pgm_ndx] = FALSE;
          }
          else
          {
            /*
             * Remove all the other Ethernet-related PMF Programs with the same VLAN * PFC
             */
            for(pmf_pgm_ndx_valid = 1; pmf_pgm_ndx_valid <= SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX; pmf_pgm_ndx_valid ++)
            {
              if ((pgm_validity.is_pgm_valid[pmf_pgm_ndx_valid] == TRUE) && (pmf_pgm_ndx_valid != pmf_pgm_ndx))
              {
                /*
                 * Get the program properties
                 */
                SOC_PB_PMF_PGM_INFO_clear(&pmf_pgm_info);
                res = soc_pb_pmf_pgm_get_unsafe(
                        unit,
                        pmf_pgm_ndx_valid,
                        &pmf_pgm_info
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                if ((pmf_pgm_info.header_type == SOC_PETRA_PORT_HEADER_TYPE_ETH)
                  && (pmf_pgm_info.bytes_to_rmv.nof_bytes == pgm_params->pp_port.first_header_size)) /* Skip the injected ports */
                {
                  pgm_validity.is_pgm_valid[pmf_pgm_ndx_valid] = FALSE;
                }
              }
            }
          }
        }
        res = soc_pb_pmf_pgm_selection_entry_set_unsafe(
                unit,
                &entry_ndx,
                &pgm_validity
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_select_unsafe()", pmf_pgm_ndx, 0);
}

uint32
  soc_pb_pmf_pgm_mgmt_select_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX    *mgmt_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *pgm_params,
    SOC_SAND_IN  uint8              is_pgm_to_add
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_MGMT_SELECT_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, SOC_PB_PMF_PGM_MGMT_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_MGMT_NDX, mgmt_ndx, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_MGMT_PARAMS, pgm_params, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_mgmt_select_verify()", pmf_pgm_ndx, 0);
}

uint32
  SOC_PB_PMF_PGM_MGMT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_MGMT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_PGM_MGMT_PARAMS_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_MGMT_PARAMS_verify()",0,0);
}

void
  SOC_PB_PMF_PGM_MGMT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_MGMT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_MGMT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_MGMT_PARAMS_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_MGMT_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_MGMT_PARAMS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_MGMT_NDX_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_MGMT_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_MGMT_NDX_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_PMF_PGM_MGMT_SOURCE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_SOURCE enum_val
  )
{
  return SOC_TMC_PMF_PGM_MGMT_SOURCE_to_string(enum_val);
}

void
  SOC_PB_PMF_PGM_MGMT_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_MGMT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_MGMT_PARAMS_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_MGMT_PARAMS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_MGMT_NDX_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_MGMT_NDX_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

uint32
  SOC_PB_PMF_PGM_MGMT_NDX_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_MGMT_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->pp_port_ndx, SOC_PB_PORT_NOF_PP_PORTS - 1,  SOC_PB_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pfg_ndx, SOC_PB_PMF_NOF_LKP_PROFILE_IDS - 1, SOC_PB_PMF_PGM_MGMT_PFG_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_MGMT_NDX_verify()",info->pp_port_ndx,info->pfg_ndx);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

