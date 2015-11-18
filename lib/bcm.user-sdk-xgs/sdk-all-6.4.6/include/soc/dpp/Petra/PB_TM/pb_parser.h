/* $Id: pb_parser.h,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_parser.h
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

#ifndef __SOC_PB_PARSER_INCLUDED__
/* { */
#define __SOC_PB_PARSER_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_db.h>

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

#define SOC_PB_PARSER_PROG_RAW_ADDR_START                          (0x0000)
#define SOC_PB_PARSER_PROG_ETH_ADDR_START                          (0x0001)
#define SOC_PB_PARSER_PROG_TM_ADDR_START                           (0x0002)
#define SOC_PB_PARSER_PROG_FTMH_ADDR_START                         (0x0003)
#define SOC_PB_PARSER_PROG_INJECTED_EXT_KEY_ADDR_START             (0x0004)

#define SOC_PB_PARSER_DEFAULT_KEY_PROGRAM_PROFILE                  (0)
#define SOC_PB_PARSER_INJECTED_KEY_PROGRAM_PROFILE                 (1)
#define SOC_PB_PARSER_NON_ETH_KEY_PROGRAM_PROFILE                  (2)

#define SOC_PB_PARSER_PP_CONTEXT_RAW                               (SOC_PB_PARSER_PROG_RAW_ADDR_START)
#define SOC_PB_PARSER_PP_CONTEXT_ETH                               (SOC_PB_PARSER_PROG_ETH_ADDR_START)
#define SOC_PB_PARSER_PP_CONTEXT_ITMH                              (SOC_PB_PARSER_PROG_TM_ADDR_START)
#define SOC_PB_PARSER_PP_CONTEXT_FTMH                              (SOC_PB_PARSER_PROG_FTMH_ADDR_START)
#define SOC_PB_PARSER_PP_CONTEXT_INJECTED_EXT_KEY                  (SOC_PB_PARSER_PROG_INJECTED_EXT_KEY_ADDR_START)

/* Non written programs */
#define SOC_PB_PARSER_PP_CONTEXT_PROG                              (7)

#define SOC_PB_PARSER_PFC_RAW                                (0x0020)
#define SOC_PB_PARSER_PFC_TM                                 (0x0030)
#define SOC_PB_PARSER_PFC_TM_IS                              (0x0031)
#define SOC_PB_PARSER_PFC_TM_PPH                             (0x0034)
#define SOC_PB_PARSER_PFC_TM_IS_PPH                          (0x0035)
#define SOC_PB_PARSER_PFC_FTMH                               (0x0028)

#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_ETH                 SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH           
#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_ETH_ETH             SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_ETH       
#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH            SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH      
#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH            SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH      
#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH           SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH     
#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH           SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH     
#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH           SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH     
#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_ETH_TRILL_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_TRILL_ETH 
#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH 
#define SOC_PB_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH 
typedef SOC_TMC_PARSER_PKT_HDR_STK_TYPE                    SOC_PB_PARSER_PKT_HDR_STK_TYPE;


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

uint32
  soc_pb_parser_init(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_parser_ingress_shape_state_set(
     SOC_SAND_IN int                                 unit,
     SOC_SAND_IN uint8                                 enable,
     SOC_SAND_IN uint32                                  q_low,
     SOC_SAND_IN uint32                                  q_high
  );

uint32
  soc_pb_parser_nof_bytes_to_remove_set(
    SOC_SAND_IN int      unit,
    SOC_SAND_IN uint32      pp_port_ndx,
    SOC_SAND_IN uint32      nof_bytes_to_skip
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PARSER_INCLUDED__*/
#endif

