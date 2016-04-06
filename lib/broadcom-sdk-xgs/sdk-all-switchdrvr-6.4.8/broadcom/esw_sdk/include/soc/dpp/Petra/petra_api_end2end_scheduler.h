/* $Id: petra_api_end2end_scheduler.h,v 1.12 Broadcom SDK $
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


#ifndef __SOC_PETRA_API_END2END_SCHEDULER_INCLUDED__
/* { */
#define __SOC_PETRA_API_END2END_SCHEDULER_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/*     When configuring scheduler port, this value indicates
 *     that port scheduler groups will be configured
 *     automatically, and not according to the expected port
 *     rate                                                    */
#define  SOC_PETRA_SCH_PORT_MAX_EXPECTED_RATE_AUTO (SOC_TMC_SCH_PORT_MAX_EXPECTED_RATE_AUTO)

#define SOC_PETRA_SCH_SE_HR_MODE_LAST                       SOC_TMC_SCH_SE_HR_MODE_LAST
#define SOC_PETRA_SCH_CL_CLASS_MODE_LAST                    SOC_TMC_SCH_CL_CLASS_MODE_LAST
#define SOC_PETRA_SCH_CL_CLASS_WEIGHTS_MODE_LAST            SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_LAST
#define SOC_PETRA_SCH_CL_ENHANCED_MODE_LAST                 SOC_TMC_SCH_CL_ENHANCED_MODE_LAST

#define SOC_PETRA_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS     SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS

/*     Overall number of interfaces -
 *     Channelized/Port NIFs, CPU, RCY, OLP, ERP            */

/*     Number of entries in scheduler interfaces structure    */

/*   Number of links towards the fabric                       */
#define SOC_PETRA_SCH_NOF_LINKS                    SOC_TMC_SCH_NOF_LINKS

/*   Number of RCI bucket levels                              */
#define SOC_PETRA_SCH_NOF_RCI_LEVELS               SOC_TMC_SCH_NOF_RCI_LEVELS

/* DRM; links (0..36) X RCI bucket level (0..7)  = 37 * 8 = 296 */

/*     Number of CL types that can be defined in the SCT
 *     (scheduler class type) table                           */
#define SOC_PETRA_SCH_NOF_CLASS_TYPES              SOC_TMC_SCH_NOF_CLASS_TYPES

/*     Maximal number of subflows in single flow
 *     If the flow is not composite,
 *     only the first subflow is active                       */
#define SOC_PETRA_SCH_NOF_SUB_FLOWS                SOC_TMC_SCH_NOF_SUB_FLOWS

/*     Base flow id for aggregates.
 *     Flows with lower id-s can only be
 *     simple (queue-bounded) flows                           */
#define SOC_PETRA_SCH_FLOW_BASE_AGGR_FLOW_ID       SOC_TMC_SCH_FLOW_BASE_AGGR_FLOW_ID_PETRA
#define SOC_PETRA_SCH_MAX_SE_ID                    SOC_TMC_SCH_MAX_SE_ID_PETRA

/*     invalid scheduling element id (outside the allowed range)
 *                                                            */
#define SOC_PETRA_SCH_SE_ID_INVALID                SOC_TMC_SCH_SE_ID_INVALID_PETRA

#define SOC_PETRA_SCH_MAX_FLOW_ID                  SOC_TMC_SCH_MAX_FLOW_ID_PETRA

/*     invalid flow id (outside the allowed range)
 *                                                            */
#define SOC_PETRA_SCH_FLOW_ID_INVALID              SOC_TMC_SCH_FLOW_ID_INVALID_PETRA
/* $Id: petra_api_end2end_scheduler.h,v 1.12 Broadcom SDK $
 * 0-80 are data ports, and port 80 is the ERP port, that enable sending
 * FAP-level scheduled traffic to each FAP, to be egress replicated.
 */
#define SOC_PETRA_SCH_MAX_PORT_ID                  SOC_TMC_SCH_MAX_PORT_ID_PETRA

/*     invalid port id (outside the allowed range)
 *                                                            */
#define SOC_PETRA_SCH_PORT_ID_INVALID              SOC_TMC_SCH_PORT_ID_INVALID_PETRA

#define SOC_PETRA_SCH_FLOW_HR_MIN_WEIGHT           SOC_TMC_SCH_FLOW_HR_MIN_WEIGHT
#define SOC_PETRA_SCH_FLOW_HR_MAX_WEIGHT           SOC_TMC_SCH_FLOW_HR_MAX_WEIGHT

#define SOC_PETRA_SCH_SUB_FLOW_CL_MIN_WEIGHT           SOC_TMC_SCH_SUB_FLOW_CL_MIN_WEIGHT
#define SOC_PETRA_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODES_3_4 SOC_TMC_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODES_3_4
#define SOC_PETRA_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODE_5    SOC_TMC_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODE_5

#define SOC_PETRA_SCH_NOF_SLOW_RATES                   SOC_TMC_SCH_NOF_SLOW_RATES

/*      Number of low flow control configuration
 *      the device can hold at any point                      */
#define SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS           SOC_TMC_SCH_LOW_FC_NOF_AVAIL_CONFS

/*      number of possible flow control configurations.
 *      At any point, the device can only hold
 *      SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS out of
 *      these configurations                                  */
#define SOC_PETRA_SCH_LOW_FC_NOF_VALID_CONFS           SOC_TMC_SCH_LOW_FC_NOF_VALID_CONFS

#define SOC_PETRA_SCH_NOF_GROUPS                       SOC_TMC_SCH_NOF_GROUPS

/*    Number of configurable interface weights
 *                                                            */
#define SOC_PETRA_SCH_NOF_IF_WEIGHTS                   SOC_TMC_SCH_NOF_IF_WEIGHTS

#define SOC_PETRA_SCH_SUB_FLOW_SHAPE_NO_LIMIT (SOC_TMC_SCH_SUB_FLOW_SHAPE_NO_LIMIT)


/* Scheduling elements id ranges { */

/*
*  CL: 0     - 8191
*  FQ: 8192  - 16127
*  HR: 16128 - 16383
*/
#define SOC_PETRA_CL_SE_ID_MIN   SOC_TMC_CL_SE_ID_MIN_PETRA
#define SOC_PETRA_CL_SE_ID_MAX   SOC_TMC_CL_SE_ID_MAX_PETRA
#define SOC_PETRA_FQ_SE_ID_MIN   SOC_TMC_FQ_SE_ID_MIN_PETRA
#define SOC_PETRA_FQ_SE_ID_MAX   SOC_TMC_FQ_SE_ID_MAX_PETRA
#define SOC_PETRA_HR_SE_ID_MIN   SOC_TMC_HR_SE_ID_MIN_PETRA
#define SOC_PETRA_HR_SE_ID_MAX   SOC_TMC_HR_SE_ID_MAX_PETRA

#define SOC_PETRA_SCH_CL_OFFSET_IN_QUARTET     SOC_TMC_SCH_CL_OFFSET_IN_QUARTET
#define SOC_PETRA_SCH_FQ_HR_OFFSET_IN_QUARTET  SOC_TMC_SCH_FQ_HR_OFFSET_IN_QUARTET

#define SOC_PETRA_SCH_DESCRETE_WEIGHT_MAX      SOC_TMC_SCH_DESCRETE_WEIGHT_MAX
/* Scheduling elements id ranges } */
/* } */

/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PETRA_SCH_FLOW_TO_QRTT_ID(flow_id)             SOC_TMC_SCH_FLOW_TO_QRTT_ID(flow_id)
#define SOC_PETRA_SCH_QRTT_TO_FLOW_ID(q_flow_id)           SOC_TMC_SCH_QRTT_TO_FLOW_ID(q_flow_id)
#define SOC_PETRA_SCH_FLOW_TO_1K_ID(flow_id)               SOC_TMC_SCH_FLOW_TO_1K_ID(flow_id)
#define SOC_PETRA_SCH_1K_TO_FLOW_ID(k_flow_id)             SOC_TMC_SCH_1K_TO_FLOW_ID(k_flow_id)


#define SOC_PETRA_SCH_1K_FLOWS_IS_IN_AGGR_RANGE(k_flow_id) SOC_TMC_SCH_1K_FLOWS_IS_IN_AGGR_RANGE_PETRA(k_flow_id)

#define SOC_PETRA_SCH_FLOW_BASE_QRTT_ID(f)                 SOC_TMC_SCH_FLOW_BASE_QRTT_ID(f)
#define SOC_PETRA_SCH_FLOW_ID_IN_QRTT(f)                   SOC_TMC_SCH_FLOW_ID_IN_QRTT(f)

/* The first subflow of a composite flow */
#define SOC_PETRA_SCH_SUB_FLOW_BASE_FLOW_0_1(sf)           SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_1(sf)
#define SOC_PETRA_SCH_SUB_FLOW_BASE_FLOW_0_2(sf)           SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_2(sf)
#define SOC_PETRA_SCH_SUB_FLOW_BASE_FLOW(sf,is_odd_even)   SOC_TMC_SCH_SUB_FLOW_BASE_FLOW(sf,is_odd_even)

/* TRUE if the subflow sf is a subflow of flow f, OddEven = TRUE */

/* TRUE if the subflow sf is a subflow of flow f, OddEven = FALSE
 * The test: same quartet, both odd/even  */

/* TRUE if the subflow sf is a subflow of flow f */
#define SOC_PETRA_SCH_IS_SUB_FLOW_OF_FLOW(f,sf,is_odd_even) SOC_TMC_SCH_IS_SUB_FLOW_OF_FLOW(f,sf,is_odd_even)

/* In composite mode (two active subflows), OddEven = TRUE
 * TRUE if sf is a second subflow, and not an independent flow */

/* In composite mode (two active subflows), OddEven = FALSE
 * TRUE if sf is a second subflow, and not an independent flow */

#define SOC_PETRA_SCH_COMPOSITE_IS_SECOND_SUBFLOW(sf, is_odd_even) SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW(sf, is_odd_even)

/*
 * The following macros check scheduling element and flow validity.
 * The precondition for using these macros is, that invalid id-s
 * will be indicated as SOC_PETRA_SCH_SE_ID_INVALID or SOC_PETRA_SCH_FLOW_ID_INVALID
 * ID_INDICATION_VALIDITY {
 */
#define SOC_PETRA_SCH_INDICATED_SE_ID_IS_VALID(id)           SOC_TMC_SCH_INDICATED_SE_ID_IS_VALID_PETRA(id)
#define SOC_PETRA_SCH_INDICATED_PORT_ID_IS_VALID(id)         SOC_TMC_SCH_INDICATED_PORT_ID_IS_VALID_PETRA(id)

#define SOC_PETRA_SCH_VERIFY_SE_ID(id, err_no,exit_place)   \
{                                                       \
  if(!(SOC_PETRA_SCH_INDICATED_SE_ID_IS_VALID(id)))         \
  {                                                     \
  SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_SE_ID_ERR,err_no, exit_place); \
  }                                                     \
}


/* ID_INDICATION_VALIDITY } */

#define SOC_PETRA_SCH_IS_DISCRETE_WFQ_MODE(m)                SOC_TMC_SCH_IS_DISCRETE_WFQ_MODE(m)
#define SOC_PETRA_SCH_IS_INDEPENDENT_WFQ_MODE(m)             SOC_TMC_SCH_IS_INDEPENDENT_WFQ_MODE(m)
#define SOC_PETRA_SCH_IS_WFQ_CLASS_VAL(class_val)            SOC_TMC_SCH_IS_WFQ_CLASS_VAL(class_val)


/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
#define SOC_PETRA_SCH_GROUP_LAST SOC_TMC_SCH_GROUP_LAST

#define SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_NONE             SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_NONE
#define SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF1              SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF1
#define SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF2              SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF2
#define SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF3              SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF3
#define SOC_PETRA_SCH_PORT_LOWEST_HP_HR_SINGLE_CLASS_AF1_WFQ   SOC_TMC_SCH_PORT_LOWEST_HP_HR_SINGLE_CLASS_AF1_WFQ
#define SOC_PETRA_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED       SOC_TMC_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED
typedef SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS                    SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS;

#define SOC_PETRA_SCH_CL_MODE_NONE                             SOC_TMC_SCH_CL_MODE_NONE
#define SOC_PETRA_SCH_CL_MODE_1                                SOC_TMC_SCH_CL_MODE_1
#define SOC_PETRA_SCH_CL_MODE_2                                SOC_TMC_SCH_CL_MODE_2
#define SOC_PETRA_SCH_CL_MODE_3                                SOC_TMC_SCH_CL_MODE_3
#define SOC_PETRA_SCH_CL_MODE_4                                SOC_TMC_SCH_CL_MODE_4
#define SOC_PETRA_SCH_CL_MODE_5                                SOC_TMC_SCH_CL_MODE_5
typedef SOC_TMC_SCH_CL_CLASS_MODE                              SOC_PETRA_SCH_CL_CLASS_MODE;

#define SOC_PETRA_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW        SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW
#define SOC_PETRA_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW     SOC_TMC_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW
#define SOC_PETRA_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_CLASS       SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_CLASS
typedef SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE                      SOC_PETRA_SCH_CL_CLASS_WEIGHTS_MODE;

#define SOC_PETRA_CL_ENHANCED_MODE_DISABLED                    SOC_TMC_CL_ENHANCED_MODE_DISABLED
#define SOC_PETRA_CL_ENHANCED_MODE_ENABLED_HP                  SOC_TMC_CL_ENHANCED_MODE_ENABLED_HP
#define SOC_PETRA_CL_ENHANCED_MODE_ENABLED_LP                  SOC_TMC_CL_ENHANCED_MODE_ENABLED_LP
typedef SOC_TMC_SCH_CL_ENHANCED_MODE                           SOC_PETRA_SCH_CL_ENHANCED_MODE;

#define SOC_PETRA_SCH_GROUP_A                                  SOC_TMC_SCH_GROUP_A
#define SOC_PETRA_SCH_GROUP_B                                  SOC_TMC_SCH_GROUP_B
#define SOC_PETRA_SCH_GROUP_C                                  SOC_TMC_SCH_GROUP_C
#define SOC_PETRA_SCH_GROUP_AUTO                               SOC_TMC_SCH_GROUP_AUTO
#define SOC_PETRA_SCH_GROUP_NONE                               SOC_TMC_SCH_GROUP_NONE
typedef SOC_TMC_SCH_GROUP                                      SOC_PETRA_SCH_GROUP;

#define SOC_PETRA_SCH_SE_TYPE_NONE                             SOC_TMC_SCH_SE_TYPE_NONE
#define SOC_PETRA_SCH_SE_TYPE_HR                               SOC_TMC_SCH_SE_TYPE_HR
#define SOC_PETRA_SCH_SE_TYPE_CL                               SOC_TMC_SCH_SE_TYPE_CL
#define SOC_PETRA_SCH_SE_TYPE_FQ                               SOC_TMC_SCH_SE_TYPE_FQ
typedef SOC_TMC_SCH_SE_TYPE                                    SOC_PETRA_SCH_SE_TYPE;

#define SOC_PETRA_SCH_SE_STATE_DISABLE                         SOC_TMC_SCH_SE_STATE_DISABLE
#define SOC_PETRA_SCH_SE_STATE_ENABLE                          SOC_TMC_SCH_SE_STATE_ENABLE
typedef SOC_TMC_SCH_SE_STATE                                   SOC_PETRA_SCH_SE_STATE;

#define SOC_PETRA_SCH_HR_MODE_NONE                             SOC_TMC_SCH_HR_MODE_NONE
#define SOC_PETRA_SCH_HR_MODE_SINGLE_WFQ                       SOC_TMC_SCH_HR_MODE_SINGLE_WFQ
#define SOC_PETRA_SCH_HR_MODE_DUAL_WFQ                         SOC_TMC_SCH_HR_MODE_DUAL_WFQ
#define SOC_PETRA_SCH_HR_MODE_ENHANCED_PRIO_WFQ                SOC_TMC_SCH_HR_MODE_ENHANCED_PRIO_WFQ
typedef SOC_TMC_SCH_SE_HR_MODE                                 SOC_PETRA_SCH_SE_HR_MODE;

#define SOC_PETRA_SCH_FLOW_HR_CLASS_NONE                       SOC_TMC_SCH_FLOW_HR_CLASS_NONE
#define SOC_PETRA_SCH_FLOW_HR_CLASS_EF1                        SOC_TMC_SCH_FLOW_HR_CLASS_EF1
#define SOC_PETRA_SCH_FLOW_HR_CLASS_EF2                        SOC_TMC_SCH_FLOW_HR_CLASS_EF2
#define SOC_PETRA_SCH_FLOW_HR_CLASS_EF3                        SOC_TMC_SCH_FLOW_HR_CLASS_EF3
#define SOC_PETRA_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ             SOC_TMC_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ
#define SOC_PETRA_SCH_FLOW_HR_SINGLE_CLASS_BE1                 SOC_TMC_SCH_FLOW_HR_SINGLE_CLASS_BE1
#define SOC_PETRA_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ               SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ
#define SOC_PETRA_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ               SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ
#define SOC_PETRA_SCH_FLOW_HR_DUAL_CLASS_BE2                   SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_BE2
#define SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF1               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF1
#define SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF2               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF2
#define SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF3               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF3
#define SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF4               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF4
#define SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF5               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF5
#define SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF6               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF6
#define SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ           SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ
#define SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_BE2               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_BE2
typedef SOC_TMC_SCH_SUB_FLOW_HR_CLASS                          SOC_PETRA_SCH_SUB_FLOW_HR_CLASS;

#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1                    SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2                    SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP3                    SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP4                    SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP4
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ                SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ1               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ1
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ2               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ2
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ3               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ3
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ4               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ4
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ                SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ1               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ1
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ2               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ2
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ3               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ3
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ1               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ1
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP_0_ENHANCED          SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP_0_ENHANCED
#define SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP_5_ENHANCED          SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP_5_ENHANCED
typedef SOC_TMC_SCH_SUB_FLOW_CL_CLASS                          SOC_PETRA_SCH_SUB_FLOW_CL_CLASS;

#define SOC_PETRA_SCH_SLOW_RATE_NDX_1                          SOC_TMC_SCH_SLOW_RATE_NDX_1
#define SOC_PETRA_SCH_SLOW_RATE_NDX_2                          SOC_TMC_SCH_SLOW_RATE_NDX_2
typedef SOC_TMC_SCH_SLOW_RATE_NDX                              SOC_PETRA_SCH_SLOW_RATE_NDX;

#define SOC_PETRA_FLOW_NONE                                    SOC_TMC_FLOW_NONE
#define SOC_PETRA_FLOW_SIMPLE                                  SOC_TMC_FLOW_SIMPLE
#define SOC_PETRA_FLOW_AGGREGATE                               SOC_TMC_FLOW_AGGREGATE
typedef SOC_TMC_SCH_FLOW_TYPE                                  SOC_PETRA_SCH_FLOW_TYPE;

#define SOC_PETRA_SCH_FLOW_OFF                                 SOC_TMC_SCH_FLOW_OFF
#define SOC_PETRA_SCH_FLOW_ON                                  SOC_TMC_SCH_FLOW_ON
typedef SOC_TMC_SCH_FLOW_STATUS                                SOC_PETRA_SCH_FLOW_STATUS;

typedef SOC_TMC_SCH_DEVICE_RATE_ENTRY                          SOC_PETRA_SCH_DEVICE_RATE_ENTRY;
typedef SOC_TMC_SCH_DEVICE_RATE_TABLE                          SOC_PETRA_SCH_DEVICE_RATE_TABLE;
typedef SOC_TMC_SCH_IF_WEIGHT_ENTRY                            SOC_PETRA_SCH_IF_WEIGHT_ENTRY;
typedef SOC_TMC_SCH_IF_WEIGHTS                                 SOC_PETRA_SCH_IF_WEIGHTS;
typedef SOC_TMC_SCH_PORT_HP_CLASS_INFO                         SOC_PETRA_SCH_PORT_HP_CLASS_INFO;
typedef SOC_TMC_SCH_PORT_INFO                                  SOC_PETRA_SCH_PORT_INFO;
typedef SOC_TMC_EGRESS_PORT_QOS                                SOC_PETRA_EGRESS_PORT_QOS;
typedef SOC_TMC_EGRESS_PORT_QOS_TABLE                          SOC_PETRA_EGRESS_PORT_QOS_TABLE;
typedef SOC_TMC_SCH_SE_HR                                      SOC_PETRA_SCH_SE_HR;
typedef SOC_TMC_SCH_SE_CL                                      SOC_PETRA_SCH_SE_CL;
typedef SOC_TMC_SCH_SE_FQ                                      SOC_PETRA_SCH_SE_FQ;
typedef SOC_TMC_SCH_SE_CL_CLASS_INFO                           SOC_PETRA_SCH_SE_CL_CLASS_INFO;
typedef SOC_TMC_SCH_SE_CL_CLASS_TABLE                          SOC_PETRA_SCH_SE_CL_CLASS_TABLE;
typedef SOC_TMC_SCH_SE_PER_TYPE_INFO                           SOC_PETRA_SCH_SE_PER_TYPE_INFO;
typedef SOC_TMC_SCH_SE_INFO                                    SOC_PETRA_SCH_SE_INFO;
typedef SOC_TMC_SCH_SUB_FLOW_SHAPER                            SOC_PETRA_SCH_SUB_FLOW_SHAPER;
typedef SOC_TMC_SCH_SUB_FLOW_HR                                SOC_PETRA_SCH_SUB_FLOW_HR;
typedef SOC_TMC_SCH_SUB_FLOW_CL                                SOC_PETRA_SCH_SUB_FLOW_CL;
typedef SOC_TMC_SCH_SUB_FLOW_FQ                                SOC_PETRA_SCH_SUB_FLOW_FQ;
typedef SOC_TMC_SCH_SUB_FLOW_SE_INFO                           SOC_PETRA_SCH_SUB_FLOW_SE_INFO;
typedef SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE                     SOC_PETRA_SCH_SUB_FLOW_CREDIT_SOURCE;
typedef SOC_TMC_SCH_SUBFLOW                                    SOC_PETRA_SCH_SUBFLOW;
typedef SOC_TMC_SCH_FLOW                                       SOC_PETRA_SCH_FLOW;
typedef SOC_TMC_SCH_GLOBAL_PER1K_INFO                          SOC_PETRA_SCH_GLOBAL_PER1K_INFO;
typedef SOC_TMC_SCH_QUARTET_MAPPING_INFO                       SOC_PETRA_SCH_QUARTET_MAPPING_INFO;
typedef SOC_TMC_SCH_SLOW_RATE                                  SOC_PETRA_SCH_SLOW_RATE;

typedef SOC_TMC_SCH_FLOW_AND_UP_INFO                           SOC_PETRA_SCH_FLOW_AND_UP_INFO;
typedef SOC_TMC_SCH_FLOW_AND_UP_PORT_INFO                      SOC_PETRA_SCH_FLOW_AND_UP_PORT_INFO;
typedef SOC_TMC_SCH_FLOW_AND_UP_SE_INFO                        SOC_PETRA_SCH_FLOW_AND_UP_SE_INFO;
typedef SOC_TMC_SCH_FLOW_AND_UP_UNION_INFO                     SOC_PETRA_SCH_FLOW_AND_UP_UNION_INFO;
/*     Class types. Range: 0 - 255                             */
typedef SOC_TMC_SCH_CL_CLASS_TYPE_ID SOC_PETRA_SCH_CL_CLASS_TYPE_ID;

/*     Scheduler element id. Valid range 0 - 16K-1.             */
typedef SOC_TMC_SCH_SE_ID SOC_PETRA_SCH_SE_ID;

/*     Port id. Range: 0 - 80.                                 */
typedef SOC_TMC_SCH_PORT_ID SOC_PETRA_SCH_PORT_ID;

/*     Flow id. Range: 0 - 56K-1.                              */
typedef SOC_TMC_SCH_FLOW_ID SOC_PETRA_SCH_FLOW_ID;


#define SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_INVERSE       SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_INVERSE
#define SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_PROPORTIONAL  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_PROPORTIONAL
#define SOC_PETRA_SCH_NOF_FLOW_IPF_CONFIG_MODES          SOC_TMC_SCH_NOF_FLOW_IPF_CONFIG_MODES
typedef SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE;

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
 *   soc_petra_sch_flow_ipf_config_mode_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the Independent-Per-Flow Weight configuration mode
 *   (proportional or inverse-proportional). The mode affects
 *   all subsequent configurations of the flow weight in
 *   Independent-Per-Flow mode, as configured by flow_set and
 *   aggregate_set APIs.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of device to access.
 *   SOC_SAND_IN  SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE mode -
 *     Independent-Per-Flow Weight configuration mode.
 * REMARKS:
 *   This API only sets the configuration mode, and affects
 *   the behavior of other APIs (flow_set, aggregate_set). It
 *   does not change current flow settings.
 * RETURNS:
 *   OK or ERROR indication.
 *********************************************************************/
uint32
  soc_petra_sch_flow_ipf_config_mode_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE mode
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_petra_sch_flow_ipf_config_mode_set" API.
 *     Refer to "soc_petra_sch_flow_ipf_config_mode_set" API for
 *     details.
*********************************************************************/
uint32
  soc_petra_sch_flow_ipf_config_mode_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE *mode
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_rate_entry_set
* TYPE:
*   PROC
* FUNCTION:
*     This function sets an entry in the device rate table.
*     Each entry sets a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links. The driver writes to
*     the following tables: Device Rate Memory (DRM)
* INPUT:
*  SOC_SAND_IN  uint32                 rci_level_ndx -
*     RCI bucket level. Range: 0 - 7
*  SOC_SAND_IN  uint32                 nof_active_links_ndx -
*     Number of current active links range: 0 - 36
*  SOC_SAND_IN  uint32                  rate -
*     The credit generation rate, in Mega-Bit-Sec. If 0 - no
*     credits are generated.
*  SOC_SAND_OUT uint32                  *exact_rate -
*     Loaded with the actual written values. These can differ
*     from the given values due to rounding.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_rate_entry_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_IN  uint32                  rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_rate_entry_get
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_device_rate_entry_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_OUT SOC_PETRA_SCH_DEVICE_RATE_ENTRY *dev_rate -
*     An entry in the device rate table. SOC_SAND_OUY
*     SOC_PETRA_SCH_DEVICE_RATE_ENTRY *exact_dev_rate - Loaded
*     with the actual written values. These can differ from
*     the given values due to rounding.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_rate_entry_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_OUT uint32                   *rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_if_shaper_rate_set
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling & CPU) its maximal credit rate. This API is
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS
*     section below.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index, consists of interface type, and an
*     interface index for network interfaces.
*  SOC_SAND_IN  uint32                  if_rate -
*     Maximum credit rate in Kilo-Bit-Sec.
*  SOC_SAND_OUT uint32                  *exact_if_rate -
*     Loaded with the actual written values. These can differ
*     from the given values due to rounding.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   This function must only be called for Channelized interfaces.
*   For one-port-interfaces, including OLP and ERP, rate configuration
*   is done according to port_rate, as part of
*   egress port configuration.
*********************************************************************/
int
  soc_petra_sch_if_shaper_rate_set_dispatch(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     uint32                tm_port,
    SOC_SAND_IN     uint32                rate
  );

uint32
  soc_petra_sch_if_shaper_rate_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  uint32                  if_rate,
    SOC_SAND_OUT uint32                  *exact_if_rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_if_shaper_rate_get
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling & CPU) its maximal credit rate. This API is
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS
*     section below.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index, consists of interface type, and an
*     interface index for network interfaces.
*  SOC_SAND_OUT uint32                  *if_rate -
*     Maximum credit rate in Kilo-Bit-Sec.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   This function must only be called for Channelized interfaces.
*   For one-port-interfaces, including OLP and ERP, rate configuration
*   is done according to port_rate, as part of
*   egress port configuration.
*********************************************************************/
int
  soc_petra_sch_if_shaper_rate_get_dispatch(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     uint32                tm_port,
    SOC_SAND_OUT    uint32                *if_rate
  );

uint32
  soc_petra_sch_if_shaper_rate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT uint32                  *if_rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_if_weight_idx_set
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling, OLP, ERP) its weight index. Range: 0-7. The
*     actual weight value (one of 8, configurable) is in range
*     1-1023, 0 meaning inactive interface. This API is only
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS section
*     below.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index, consists of interface type, and an
*     interface index for network interfaces.
*  SOC_SAND_IN  uint32                  weight_index -
*     Interface weight index. Range: 0-7. Selects one of 8
*     configurable weights for interfaces WFQ.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_if_weight_idx_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  uint32                  weight_index
  );

uint32
soc_petra_sch_device_if_weight_idx_set_dispatch(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  uint32              weight_index
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_if_weight_idx_get
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling, OLP, ERP) its weight index. Range: 0-7. The
*     actual weight value (one of 8, configurable) is in range
*     1-1023, 0 meaning inactive interface. This API is only
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS section
*     below.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index, consists of interface type, and an
*     interface index for network interfaces.
*  SOC_SAND_OUT uint32                  *weight_index -
*     Interface weight index. Range: 0-7. Selects one of 8
*     configurable weights for interfaces WFQ.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_if_weight_idx_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT uint32                  *weight_index
  );

uint32
soc_petra_sch_device_if_weight_idx_get_dispatch(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_OUT uint32              *weight_index
  );
/*********************************************************************
* NAME:
*     soc_petra_sch_if_weight_conf_set
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     This function sets the device interfaces scheduler
*     weight configuration. Up to 8 weight configuration can
*     be pre-configured. Each scheduler interface will be
*     configured to use one of these pre-configured weights.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_IF_WEIGHTS      *if_weights -
*     The weighs configuration for the device interfaces
*     scheduler.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_if_weight_conf_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_IF_WEIGHTS      *if_weights
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_if_weight_conf_get
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     This function sets the device interfaces scheduler
*     weight configuration. Up to 8 weight configuration can
*     be pre-configured. Each scheduler interface will be
*     configured to use one of these pre-configured weights.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_OUT SOC_PETRA_SCH_IF_WEIGHTS      *if_weights -
*     The weighs configuration for the device interfaces
*     scheduler.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_if_weight_conf_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_SCH_IF_WEIGHTS      *if_weights
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets a single class type in the table. The driver writes
*     to the following tables: CL-Schedulers Type (SCT)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 unit -
*     Identifier of core on device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_CL_CLASS_TYPE_ID cl_type_ndx -
*     The index of the class type to configure (0-255).
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type -
*     A Scheduler class type.
*  SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_INFO *exact_class_type -
*     Loaded with the actual parameters given difference due
*     to rounding.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Out of the 8 K CL SE-s,
*   7936 can be configured as enhanced,  since 7936 FQ-s exist.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_INFO *exact_class_type
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_get
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_class_type_params_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_CL_CLASS_TYPE_ID cl_type_ndx -
*     The index of the class type to configure (0-255).
*  SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type -
*     A Scheduler class type.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_table_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets the scheduler class type table as a whole.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct -
*     A Scheduler class type table.
*  SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_TABLE *exact_sct -
*     Loaded with the actual parameters given difference due
*     to rounding.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_table_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_TABLE *exact_sct
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_table_get
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_class_type_params_table_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct -
*     A Scheduler class type table.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_table_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct
  );


/*********************************************************************
* NAME:
*     soc_petra_sch_flow2se_id
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Calculates se_id given the appropriate flow_id
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_id -
*     flow id, 0 - 56K-1
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
SOC_PETRA_SCH_SE_ID
  soc_petra_sch_flow2se_id(
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_id
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_se2flow_id
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Calculates flow_id given the appropriate se_id
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_id -
*     Scheduling element id, 0 - 16K-1
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
SOC_PETRA_SCH_FLOW_ID
  soc_petra_sch_se2flow_id(
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_id
  );
/*****************************************************
* NAME
*   soc_petra_sch_se_get_type_by_id
* TYPE:
*   PROC
* DATE:
*   24/10/2007
* FUNCTION:
*   Return the  scheduling element type
*   with id-range matching the se_id.
* INPUT:
*   SOC_PETRA_SCH_SE_ID                   se_id -
*     scheduling element id.
* RETURNS:
*   scheduling element type with id-range matching the se_id
* REMARKS:
*    None.
*****************************************************/
SOC_PETRA_SCH_SE_TYPE
  soc_petra_sch_se_get_type_by_id(
    SOC_PETRA_SCH_SE_ID                   se_id
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_se2port_id
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Calculates port id given the appropriate scheduling
*     element id
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_id -
*     flow id, 0 - 16K-1
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
SOC_PETRA_SCH_PORT_ID
  soc_petra_sch_se2port_id(
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_id
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_port2se_id
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Calculates scheduling element id given the appropriate
*     port id
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_id -
*     Port id, 0 - 80
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
SOC_PETRA_SCH_SE_ID
  soc_petra_sch_port2se_id(
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_id
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_port_sched_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets the scheduler-port state (enable/disable), its
*     HR mode of operation (single or dual) and low flow control
*     ignore policy.
*     The driver writes to the following tables:
*     Scheduler Enable Memory (SEM),
*     HR-Scheduler-Configuration (SHC),
*     Flow Group Memory (FGM)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core (on device) to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx -
*     Port index (0-79).
*  SOC_SAND_IN SOC_PETRA_SCH_PORT_INFO      *port_info -
*     Scheduler port configuration info.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. Scheduler group will be set upon port open (status is enable)
*   2. As part of 'close port' procedure, this API must be called
*      with status = disable.
*   3. If port state is 'enable', max_expected_rate (port_info)
*      must be non-zero.
*********************************************************************/
uint32
  soc_petra_sch_port_sched_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_INFO      *port_info
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_port_sched_get
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_port_sched_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx -
*     Port index (0-79).
*  SOC_SAND_OUT SOC_PETRA_SCH_PORT_INFO      *port_info -
*     Scheduler port configuration info.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_port_sched_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_PORT_INFO      *port_info
  );

/*****************************************************
* NAME
*   soc_petra_sch_port_hp_class_conf_set
* TYPE:
*   PROC
* DATE:
*   31/10/2007
* FUNCTION:
*   Sets the group of available configurations for high priority
*   hr class settings.
*   Out of 5 possible configurations, 4 are available at any time.
* INPUT:
*   SOC_SAND_IN  int                      unit -
*     Identifier of device to access.
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info -
*    A set of available configurations for port low flow control
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_port_hp_class_conf_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_port_hp_class_conf_get
* TYPE:
*   PROC
* FUNCTION:
*     Sets the group of available configurations for high
*     priority hr class settings. Out of 5 possible
*     configurations, 4 are available at any time.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_OUT SOC_PETRA_SCH_PORT_HP_CLASS_INFO *hp_class_info -
*     A set of available configurations for port low flow
*     control.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_port_hp_class_conf_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_SCH_PORT_HP_CLASS_INFO *hp_class_info
  );


/*********************************************************************
* NAME:
*     soc_petra_sch_slow_max_rates_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     This function sets the slow rates. A flow might be in
*     slow state, and in that case lower rate is needed.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_SLOW_RATE      slow_max_rates -
*     The rates in Kbit/s.
*  SOC_SAND_OUT SOC_PETRA_SCH_SLOW_RATE      exact_slow_max_rates -
*     Loaded with the actual parameters given difference due
*     to rounding.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_slow_max_rates_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SLOW_RATE      *slow_max_rates,
    SOC_SAND_OUT SOC_PETRA_SCH_SLOW_RATE      *exact_slow_max_rates
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_slow_max_rates_get
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_slow_max_rates_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_OUT SOC_PETRA_SCH_SLOW_RATE      *slow_max_rates -
*     The rates in Kbit/s. SOC_SAND_IN uint32 slow_max_rate2 -
*     Loaded with the actual parameters given difference due
*     to rounding.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_slow_max_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_SCH_SLOW_RATE      *slow_max_rates
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_aggregate_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets an aggregate scheduler. It configures an elementary
*     scheduler, and defines a credit flow to this scheduler
*     from a 'father' scheduler. The driver writes to the
*     following tables: Scheduler Enable Memory (SEM),
*     HR-Scheduler-Configuration (SHC), CL-Schedulers
*     Configuration (SCC), Flow Group Memory (FGM) Shaper
*     Descriptor Memory (SHD) Flow Sub-Flow (FSF) Flow
*     Descriptor Memory (FDM) Shaper Descriptor Memory
*     Static(SHDS) Flow Descriptor Memory Static (FDMS)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_ndx -
*     Scheduler element index.
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO        *se -
*     Scheduling element parameters to set.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow -
*     Flow parameters to set
*  SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *exact_flow -
*     Loaded with the actual written values. These can differ
*     from the given values due to rounding.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. Dual shaper configuration (part of the se configuration)
*   is only set when the spouse aggregate is enabled -
*   after enabling the second aggregate.
*   When the spouse is not enabled, dual shaper configuration  will be unset.
*   2. Out of the 8 K CL SE-s, 7936 can be configured as enhanced,
*   since 7936 FQ-s exist.
*   3. When the scheduling element (se) status is 'disable',
*   the scheduling element will be disabled and any other configuration
*   of the scheduling element will remain intact.
*********************************************************************/
uint32
  soc_petra_sch_aggregate_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO        *se,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *exact_flow
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_aggregate_group_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets the group of an aggregate scheduler. 
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_ndx -
*     Scheduler element index.
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO        *se -
*     Scheduling element parameters to set.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow -
*     Flow parameters to set
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_aggregate_group_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID    se_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO  *se,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW     *flow
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_aggregate_get
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_aggregate_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_ndx -
*     Scheduler element index.
*  SOC_SAND_OUT SOC_PETRA_SCH_SE_INFO        *se -
*     Scheduling element parameters to set.
*  SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *flow -
*     Flow parameters to set
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. Dual shaper configuration (part of the se configuration)
*   is only set when the spouse aggregate is enabled -
*   after enabling the second aggregate.
*   When the spouse is not enabled, dual shaper configuration  will be unset.
*   2. Out of the 8 K CL SE-s, 7936 can be configured as enhanced,
*   since 7936 FQ-s exist.
*********************************************************************/
uint32
  soc_petra_sch_aggregate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_INFO        *se,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *flow
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_delete
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Delete a scheduler flow,
*     Flow Descriptor Memory Static (FDMS)
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx -
*     Flow index to configure. This index must be identical to
*     the subflow index of the first subflow in the
*     flow structure.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Flow deletion may also be achieved by calling soc_petra_sch_flow_set
*   with FALSE on 'is_valid' property of the flows to delete.
*********************************************************************/
uint32
  soc_petra_sch_flow_delete(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID         flow_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets a scheduler flow, from a scheduling element (or
*     elements) to another element .
*     The driver writes to the following tables:
*     Scheduler Enable Memory (SEM),
*     Shaper Descriptor Memory Static(SHDS),
*     Flow Sub-Flow (FSF),
*     Flow Descriptor Memory Static (FDMS)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 unit -
*     Identifier of core on device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx -
*     Flow index to configure. This index must be identical to
*     the subflow index of the first subflow in the
*     flow structure.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow -
*     Flow parameters to set
*  SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *exact_flow -
*     Loaded with the actual written values. These can differ
*     from the given values due to rounding.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. When OddEven is FALSE (1-3 configuration),
*     composite configuration (flow with two subflows)
*     is only applicable for aggregates - not for simple flows.
*   2. If a sub-flow is declared invalid (flow->sub_flow.is_valid == FALSE),
*      it is deleted. The only relevant configuration for such a sub-flow
*      is the sub-flow 'id' field.
*   3. The sub-flow id-s are validated by the driver according to the flow_ndx
*      and the per-1K configuration. The index of the first sub-flow must be
*      equal to flow_ndx, and the index of the second sub-flow is according to
*      the odd-even configuration.
*********************************************************************/
uint32
  soc_petra_sch_flow_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *exact_flow
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_get
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_flow_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx -
*     Flow index to configure. This index must be identical to
*     the subflow index of the first (even) subflow in the
*     flow structure.
*  SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *flow -
*     Flow parameters to set
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_flow_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *flow
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_status_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Set flow state to off/on. The state of the flow will be
*     updated, unless was configured otherwise. Note: useful
*     for virtual flows, for which the flow state must be
*     explicitly set
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx -
*     The flow id (0-56K) of the requested flow.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_STATUS    state -
*     The requested state.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_flow_status_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_STATUS    state
  );


/*********************************************************************
* NAME:
*     soc_petra_sch_per1k_info_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_per1k_info_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  uint32                 k_flow_ndx -
*     The index of the 1K flows to configure. Range: 24 - 55.
*     Corresponds to flow-id range of 24K - 56K-1
*  SOC_SAND_IN  SOC_PETRA_SCH_GLOBAL_PER1K_INFO *per1k_info -
*     Per 1K flows configuration. Includes the following: -
*     interdigitated - odd/even - cir/eir
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_per1k_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_GLOBAL_PER1K_INFO *per1k_info
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_per1k_info_get
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets configuration for 1K flows/aggregates (256
*     quartets). Flows interdigitated mode configuration must
*     match the interdigitated mode configurations of the
*     queues they are mapped to. Note1: the following flow
*     configuration is not allowed: interdigitated = TRUE,
*     odd_even = FALSE. The reason for this is that
*     interdigitated configuration defines flow-queue mapping,
*     but a flow with odd_even configuration = FALSE cannot be
*     mapped to a queue. Note2: this configuration is only
*     relevant to flow_id-s in the range 24K - 56K.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  uint32                 k_flow_ndx -
*     The index of the 1K flows to configure. Range: 24 - 55.
*     Corresponds to flow-id range of 24K - 56K-1
*  SOC_SAND_OUT SOC_PETRA_SCH_GLOBAL_PER1K_INFO *per1k_info -
*     Per 1K flows configuration. Includes the following: -
*     interdigitated - odd/even - cir/eir
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   For indexes in range 24-55, the configuration is read from the device.
*   For indexes in range 0-23, per-1K configuration is irrelevant,
*   and the values of per1k_info are constant. Those values are returned
*   by the API.
*********************************************************************/
uint32
  soc_petra_sch_per1k_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_GLOBAL_PER1K_INFO *per1k_info
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_to_queue_mapping_set
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets the mapping from flow to queue and to source fap.
*     The configuration is per quartet (up to 4 quartets). The
*     mapping depends on the following parameters: -
*     interdigitated mode - composite mode The driver writes
*     to the following tables: Flow to Queue Mapping (FQM)
*     Flow to FIP Mapping (FFM)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  uint32                 quartet_ndx -
*     The index of the base flow quartet to configure. Range:
*     0 - 16K-1. Corresponds to flow-id range of 0 - 56K -1.
*  SOC_SAND_IN  uint32                 nof_quartets_to_map -
*     The number of quartets to map. This parameter is
*     required for validation purposes only - it can be
*     derived from the interdigitated mode configuration and
*     the composite mode configuration, as following:
*     -------------------------------------------------
*    |InterDigitated | Composite | nof_quartets_to_map |
*     -------------------------------------------------
*    |      0        |     0     |         1           |
*     -------------------------------------------------
*    |      1        |     0     |         2           |
*     -------------------------------------------------
*    |      0        |     1     |         2           |
*     -------------------------------------------------
*    |      1        |     1     |         4           |
*     -------------------------------------------------
*  SOC_SAND_IN  SOC_PETRA_SCH_QUARTET_MAPPING_INFO *quartet_flow_info -
*     per 4 flows configuration
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. A single mapping configuration covers a range of
*   four consecutive queue id-s, and up to 16 consecutive flow id-s,
*   depending on the mode.
*   The consecutive quartets configuration will be validated
*   with previous quartets.     This means the quartets must be configured in order -      first the lower indexes, then the higher ones.     The configuration order must be kept:     - per 2 quartets if InterDigitated = 0     - per 4 quartets if InterDigitated = 1  2. The source fap configuration is per - 8 flows,      and not per quartet.      The second quartet      source fap configuration will be validated with the first one.
*********************************************************************/
uint32
  soc_petra_sch_flow_to_queue_mapping_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_IN  uint32                 nof_quartets_to_map,
    SOC_SAND_IN  SOC_PETRA_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_to_queue_mapping_get
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_flow_to_queue_mapping_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  int                 core -
*     Identifier of core on device to access.
*  SOC_SAND_IN  uint32                 quartet_ndx -
*     The index of the base quartet to configure. Range: 0 -
*    16K-1. Corresponds to flow-id range of 0 - 56K -1.
*  SOC_SAND_OUT uint32                 *nof_quartets_to_map -
*     The number of quartets to map. This parameter is
*     required for validation purposes only - it can be
*     derived from the interdigitated mode configuration and
*     the composite mode configuration, as following:
*     -------------------------------------------------
*    |InterDigitated | Composite | nof_quartets_to_map |
*     -------------------------------------------------
*    |      0        |     0     |         1           |
*     -------------------------------------------------
*    |      1        |     0     |         2           |
*     -------------------------------------------------
*    |      0        |     1     |         2           |
*     -------------------------------------------------
*    |      1        |     1     |         4           |
*     -------------------------------------------------
*  SOC_SAND_OUT SOC_PETRA_SCH_QUARTET_MAPPING_INFO *quartet_flow_info -
*     per 4 flows configuration
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_flow_to_queue_mapping_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  );

/*****************************************************
*NAME
* soc_petra_sch_flow_id_verify_unsafe
*TYPE:
*  PROC
*DATE:
*  21/10/2007
*FUNCTION:
*  verifies validity of flow id
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_id -
*     flow index to check for validity
*RETURNS:
*    OK or ERROR indication.
*REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_flow_id_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID         flow_id
  );

uint8
  soc_petra_sch_is_flow_id_valid(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID       flow_id
  );

/*****************************************************
*NAME
* soc_petra_sch_se_id_verify_unsafe
*TYPE:
*  PROC
*DATE:
*  21/10/2007
*FUNCTION:
*  verifies validity of scheduling element id
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        se_id -
*     scheduling elelment index to check for validity
*RETURNS:
*    OK or ERROR indication.
*REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_se_id_verify_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID        se_id
  );

uint8
  soc_petra_sch_is_se_id_valid(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID        se_id
  );

uint32
  soc_petra_sch_port_id_verify_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID      port_id
  );

uint8
  soc_petra_sch_is_port_id_valid(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_id
  );

/*****************************************************
* NAME
*   soc_petra_sch_k_flow_id_verify_unsafe
* TYPE:
*   PROC
* DATE:
*   30/10/2007
* FUNCTION:
*   ID verification for per_1_k configurations,
*   i.e. configurations covering 1K flow id-s
* INPUT:
*   SOC_SAND_IN  int        unit -
*   SOC_SAND_IN  uint32        k_flow_id -
*     flow id in 1K units,
*     0 is the first flow id relevant for per_1_k configuration,
*     as defined by SOC_PETRA_SCH_FLOW_BASE_AGGR_FLOW_ID
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_k_flow_id_verify_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        k_flow_id
  );

uint32
  soc_petra_sch_quartet_id_verify_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        quartet_id
  );

/* STRUCTURE MGMT { */

void
  soc_petra_PETRA_SCH_DEVICE_RATE_ENTRY_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_DEVICE_RATE_ENTRY *info
  );

void
  soc_petra_PETRA_SCH_DEVICE_RATE_TABLE_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_DEVICE_RATE_TABLE *info
  );

void
  soc_petra_PETRA_SCH_IF_WEIGHT_ENTRY_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_IF_WEIGHT_ENTRY *info
  );

void
  soc_petra_PETRA_SCH_IF_WEIGHTS_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_IF_WEIGHTS *info
  );

void
  soc_petra_PETRA_SCH_PORT_HP_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_PORT_HP_CLASS_INFO *info
  );

void
  soc_petra_PETRA_SCH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_PORT_INFO *info
  );

void
  soc_petra_PETRA_SCH_SE_CL_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_INFO *info
  );

void
  soc_petra_PETRA_SCH_SE_CL_CLASS_TABLE_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_TABLE *info
  );

void
  soc_petra_PETRA_SCH_SE_PER_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_SE_PER_TYPE_INFO *info
  );

void
  soc_petra_PETRA_SCH_SE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_SE_INFO *info
  );

void
  soc_petra_PETRA_SCH_SUB_FLOW_SHAPER_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_SCH_SUB_FLOW_SHAPER *info
  );

void
  soc_petra_PETRA_SCH_SUB_FLOW_HR_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_SUB_FLOW_HR *info
  );

void
  soc_petra_PETRA_SCH_SUB_FLOW_CL_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_SUB_FLOW_CL *info
  );

void
  soc_petra_PETRA_SCH_SUB_FLOW_SE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_SUB_FLOW_SE_INFO *info
  );

void
  soc_petra_PETRA_SCH_SUB_FLOW_CREDIT_SOURCE_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_SUB_FLOW_CREDIT_SOURCE *info
  );

void
  soc_petra_PETRA_SCH_SUBFLOW_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_SCH_SUBFLOW *info
  );

void
  soc_petra_PETRA_SCH_FLOW_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW *info
  );

void
  soc_petra_PETRA_SCH_GLOBAL_PER1K_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_GLOBAL_PER1K_INFO *info
  );

void
  soc_petra_PETRA_SCH_QUARTET_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_QUARTET_MAPPING_INFO *info
  );

void
  soc_petra_PETRA_SCH_SLOW_RATE_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_SLOW_RATE *info
  );

void
  soc_petra_PETRA_SCH_FLOW_AND_UP_INFO_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW_AND_UP_INFO *info,
    SOC_SAND_IN uint32                         is_full /*is_full == false --> clear the relevant fields for the next stage algorithm*/
  );

void
  soc_petra_PETRA_SCH_FLOW_AND_UP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW_AND_UP_PORT_INFO *info
  );

void
  soc_petra_PETRA_SCH_FLOW_AND_UP_SE_INFO_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW_AND_UP_SE_INFO *info
  );
#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS enum_val
  );

const char*
  soc_petra_PETRA_SCH_CL_CLASS_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_CL_CLASS_MODE enum_val
  );

const char*
  soc_petra_PETRA_SCH_CL_CLASS_WEIGHTS_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_CL_CLASS_WEIGHTS_MODE enum_val
  );


const char*
  soc_petra_PETRA_SCH_CL_ENHANCED_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_CL_ENHANCED_MODE enum_val
  );



const char*
  soc_petra_PETRA_SCH_GROUP_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_GROUP enum_val
  );



const char*
  soc_petra_PETRA_SCH_SE_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_SE_TYPE enum_val
  );



const char*
  soc_petra_PETRA_SCH_SE_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_SE_STATE enum_val
  );



const char*
  soc_petra_PETRA_SCH_SE_HR_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_SE_HR_MODE enum_val
  );



const char*
  soc_petra_PETRA_SCH_SUB_FLOW_HR_CLASS_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_SUB_FLOW_HR_CLASS enum_val
  );



const char*
  soc_petra_PETRA_SCH_SUB_FLOW_CL_CLASS_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_SUB_FLOW_CL_CLASS enum_val
  );



const char*
  soc_petra_PETRA_SCH_SLOW_RATE_NDX_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_SLOW_RATE_NDX enum_val
  );



const char*
  soc_petra_PETRA_SCH_FLOW_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_FLOW_TYPE enum_val
  );



const char*
  soc_petra_PETRA_SCH_FLOW_STATUS_to_string(
    SOC_SAND_IN SOC_PETRA_SCH_FLOW_STATUS enum_val
  );



void
  soc_petra_PETRA_SCH_DEVICE_RATE_ENTRY_print(
    SOC_SAND_IN SOC_PETRA_SCH_DEVICE_RATE_ENTRY *info
  );



void
  soc_petra_PETRA_SCH_DEVICE_RATE_TABLE_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_PETRA_SCH_DEVICE_RATE_TABLE *info
  );


void
  soc_petra_PETRA_SCH_IF_WEIGHT_ENTRY_print(
    SOC_SAND_IN SOC_PETRA_SCH_IF_WEIGHT_ENTRY *info
  );



void
  soc_petra_PETRA_SCH_IF_WEIGHTS_print(
    SOC_SAND_IN SOC_PETRA_SCH_IF_WEIGHTS *info
  );



void
  soc_petra_PETRA_SCH_PORT_HP_CLASS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SCH_PORT_HP_CLASS_INFO *info
  );



void
  soc_petra_PETRA_SCH_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_SCH_PORT_INFO *info,
    SOC_SAND_IN uint32           port_id
  );


void
  soc_petra_PETRA_SCH_SE_HR_print(
    SOC_SAND_IN SOC_PETRA_SCH_SE_HR *info
  );



void
  soc_petra_PETRA_SCH_SE_CL_print(
    SOC_SAND_IN SOC_PETRA_SCH_SE_CL *info
  );



void
  soc_petra_PETRA_SCH_SE_FQ_print(
    SOC_SAND_IN SOC_PETRA_SCH_SE_FQ *info
  );



void
  soc_petra_PETRA_SCH_SE_CL_CLASS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SCH_SE_CL_CLASS_INFO *info
  );



void
  soc_petra_PETRA_SCH_SE_CL_CLASS_TABLE_print(
    SOC_SAND_IN SOC_PETRA_SCH_SE_CL_CLASS_TABLE *info
  );

void
  soc_petra_PETRA_SCH_SE_PER_TYPE_INFO_print(
    SOC_SAND_IN SOC_PETRA_SCH_SE_PER_TYPE_INFO *info,
    SOC_SAND_IN SOC_PETRA_SCH_SE_TYPE type
  );

void
  soc_petra_PETRA_SCH_SE_INFO_print(
    SOC_SAND_IN SOC_PETRA_SCH_SE_INFO *info
  );



void
  soc_petra_PETRA_SCH_SUB_FLOW_SHAPER_print(
    SOC_SAND_IN SOC_PETRA_SCH_SUB_FLOW_SHAPER *info
  );



void
  soc_petra_PETRA_SCH_SUB_FLOW_HR_print(
    SOC_SAND_IN SOC_PETRA_SCH_SUB_FLOW_HR *info
  );



void
  soc_petra_PETRA_SCH_SUB_FLOW_CL_print(
    SOC_SAND_IN SOC_PETRA_SCH_SUB_FLOW_CL *info
  );

void
  soc_petra_PETRA_SCH_SUB_FLOW_FQ_print(
    SOC_SAND_IN SOC_PETRA_SCH_SUB_FLOW_FQ *info
  );

void
  soc_petra_PETRA_SCH_SUB_FLOW_SE_INFO_print(
    SOC_SAND_IN SOC_PETRA_SCH_SUB_FLOW_SE_INFO *info,
    SOC_SAND_IN SOC_PETRA_SCH_SE_TYPE se_type
  );



void
  soc_petra_PETRA_SCH_SUB_FLOW_CREDIT_SOURCE_print(
    SOC_SAND_IN SOC_PETRA_SCH_SUB_FLOW_CREDIT_SOURCE *info
  );



void
  soc_petra_PETRA_SCH_SUBFLOW_print(
    SOC_SAND_IN SOC_PETRA_SCH_SUBFLOW *info,
    SOC_SAND_IN uint8 is_table_flow,
    SOC_SAND_IN uint32 subflow_id
  );


void
  soc_petra_PETRA_SCH_FLOW_print(
    SOC_SAND_IN SOC_PETRA_SCH_FLOW *info,
    SOC_SAND_IN uint8 is_table
  );



void
  soc_petra_PETRA_SCH_GLOBAL_PER1K_INFO_print(
    SOC_SAND_IN SOC_PETRA_SCH_GLOBAL_PER1K_INFO *info
  );



void
  soc_petra_PETRA_SCH_QUARTET_MAPPING_INFO_print(
    SOC_SAND_IN SOC_PETRA_SCH_QUARTET_MAPPING_INFO *info
  );

void
  soc_petra_PETRA_SCH_SLOW_RATE_print(
    SOC_SAND_IN SOC_PETRA_SCH_SLOW_RATE *info
  );
/* STRUCTURE MGMT } */

uint32
  soc_petra_flow_and_up_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  uint32               flow_id,
    SOC_SAND_IN     uint32                          reterive_status,
    SOC_SAND_INOUT  SOC_PETRA_SCH_FLOW_AND_UP_INFO    *flow_and_up_info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_END2END_SCHEDULER_INCLUDED__*/
#endif
