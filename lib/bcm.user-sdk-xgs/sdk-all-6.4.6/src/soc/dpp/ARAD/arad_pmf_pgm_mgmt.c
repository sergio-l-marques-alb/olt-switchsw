#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pmf_pgm_mgmt.c,v 1.9 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_FP

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
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_pmf_pgm_mgmt.h>
#include <soc/dpp/ARAD/arad_parser.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PMF_PGM_PFC_PART_NONE                 (0)
#define ARAD_PMF_PGM_PFC_PART_E                    (1)
#define ARAD_PMF_PGM_PFC_PART_IPV4                 (2)
#define ARAD_PMF_PGM_PFC_PART_IPV6                 (3)
#define ARAD_PMF_PGM_PFC_PART_TRILL                (4)
#define ARAD_PMF_PGM_PFC_PART_MPLS1                (5)
#define ARAD_PMF_PGM_PFC_PART_MPLS2                (6)
#define ARAD_PMF_PGM_PFC_PART_MPLS3                (7)


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
/*
 * Conversion of the Packet-Format-Code
 */
uint32
  arad_pmf_pgm_packet_format_code_convert(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PMF_PGM_PKT_HDR_TYPE      hdr_type_ndx,
    SOC_SAND_OUT uint32                     *pfc
  )
{
  uint32
    threeb_first = 0,
    threeb_second = 4; /* User-Defined PFC */

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_PGM_PACKET_FORMAT_CODE_CONVERT);

  switch (hdr_type_ndx)
  {
  case ARAD_PMF_PGM_PKT_HDR_TYPE_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_NONE;
    threeb_second = ARAD_PMF_PGM_PFC_PART_NONE;
  	break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_E;
    threeb_second = ARAD_PMF_PGM_PFC_PART_NONE;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_IPV4;
    threeb_second = ARAD_PMF_PGM_PFC_PART_NONE;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_IPV6;
    threeb_second = ARAD_PMF_PGM_PFC_PART_NONE;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_MPLS1_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS1;
    threeb_second = ARAD_PMF_PGM_PFC_PART_NONE;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_MPLS2_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS2;
    threeb_second = ARAD_PMF_PGM_PFC_PART_NONE;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_MPLS3_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS3;
    threeb_second = ARAD_PMF_PGM_PFC_PART_NONE;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS1_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS1;
    threeb_second = ARAD_PMF_PGM_PFC_PART_E;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS2_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS2;
    threeb_second = ARAD_PMF_PGM_PFC_PART_E;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_MPLS3_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS3;
    threeb_second = ARAD_PMF_PGM_PFC_PART_E;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS1_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS1;
    threeb_second = ARAD_PMF_PGM_PFC_PART_IPV4;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS2_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS2;
    threeb_second = ARAD_PMF_PGM_PFC_PART_IPV4;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_MPLS3_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS3;
    threeb_second = ARAD_PMF_PGM_PFC_PART_IPV4;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS1_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS1;
    threeb_second = ARAD_PMF_PGM_PFC_PART_IPV6;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS2_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS2;
    threeb_second = ARAD_PMF_PGM_PFC_PART_IPV6;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_MPLS3_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_MPLS3;
    threeb_second = ARAD_PMF_PGM_PFC_PART_IPV6;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV4_IPV4_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_IPV4;
    threeb_second = ARAD_PMF_PGM_PFC_PART_IPV4;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_IPV6_IPV4_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_IPV4;
    threeb_second = ARAD_PMF_PGM_PFC_PART_IPV6;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_TRILL_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_TRILL;
    threeb_second = ARAD_PMF_PGM_PFC_PART_E;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_ETH_IPV4_ETH:
    threeb_first = ARAD_PMF_PGM_PFC_PART_IPV4;
    threeb_second = ARAD_PMF_PGM_PFC_PART_E;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_RAW:
    threeb_first = ARAD_PARSER_PFC_RAW_AND_FTMH % 8;
    threeb_second = ARAD_PARSER_PFC_RAW_AND_FTMH >> 3;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_FTMH:
    threeb_first = ARAD_PARSER_PFC_RAW_AND_FTMH % 8;
    threeb_second = ARAD_PARSER_PFC_RAW_AND_FTMH >> 3;
    break;

  case ARAD_PMF_PGM_PKT_HDR_TYPE_TM:
    threeb_first = ARAD_PARSER_PFC_TM % 8;
    threeb_second = ARAD_PARSER_PFC_TM >> 3;
    break;

  default:
    break;
  }

  *pfc = (threeb_second << 3) + threeb_first;
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_pgm_packet_format_code_convert()", 0, 0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

