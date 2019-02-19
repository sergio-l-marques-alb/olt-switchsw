/* $Id: petra_api_ingress_scheduler.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_api_ingress_scheduler.h
*
* MODULE PREFIX:  soc_petra_ingress_scheduler
*
* FILE DESCRIPTION:  This file holds the API functions and Structures
*                    which implement the Soc_petra ingress scheduler.
*                    The file contains the standard get/set, clear and print.
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_PETRA_API_INGRESS_SCHEDULER_INCLUDED__
/* { */
#define __SOC_PETRA_API_INGRESS_SCHEDULER_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_ingress_scheduler.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_ING_SCH_MAX_WEIGHT_VALUE                        63
#define SOC_PETRA_ING_SCH_MAX_MAX_CREDIT_VALUE                    65535
#define SOC_PETRA_ING_SCH_MAX_NOF_ENTRIES                         SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS
#define SOC_PETRA_ING_SCH_MAX_ID_VALUE                            SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS

#define SOC_PETRA_ING_SCH_CLOS_NOF_HP_SHAPERS       3
#define SOC_PETRA_ING_SCH_CLOS_NOF_GLOBAL_SHAPERS   2
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

#define SOC_PETRA_ING_SCH_MESH_LOCAL                           SOC_TMC_ING_SCH_MESH_LOCAL
#define SOC_PETRA_ING_SCH_MESH_CON1                            SOC_TMC_ING_SCH_MESH_CON1
#define SOC_PETRA_ING_SCH_MESH_CON2                            SOC_TMC_ING_SCH_MESH_CON2
#define SOC_PETRA_ING_SCH_MESH_CON3                            SOC_TMC_ING_SCH_MESH_CON3
#define SOC_PETRA_ING_SCH_MESH_CON4                            SOC_TMC_ING_SCH_MESH_CON4
#define SOC_PETRA_ING_SCH_MESH_CON5                            SOC_TMC_ING_SCH_MESH_CON5
#define SOC_PETRA_ING_SCH_MESH_CON6                            SOC_TMC_ING_SCH_MESH_CON6
#define SOC_PETRA_ING_SCH_MESH_CON7                            SOC_TMC_ING_SCH_MESH_CON7
#define SOC_PETRA_ING_SCH_MESH_LAST                            SOC_TMC_ING_SCH_MESH_LAST
typedef SOC_TMC_ING_SCH_MESH_CONTEXTS                          SOC_PETRA_ING_SCH_MESH_CONTEXTS;

typedef SOC_TMC_ING_SCH_SHAPER                                 SOC_PETRA_ING_SCH_SHAPER;
typedef SOC_TMC_ING_SCH_MESH_CONTEXT_INFO                      SOC_PETRA_ING_SCH_MESH_CONTEXT_INFO;
typedef SOC_TMC_ING_SCH_MESH_INFO                              SOC_PETRA_ING_SCH_MESH_INFO;
typedef SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT                       SOC_PETRA_ING_SCH_CLOS_WFQ_ELEMENT;
typedef SOC_TMC_ING_SCH_CLOS_WFQS                              SOC_PETRA_ING_SCH_CLOS_WFQS;
typedef SOC_TMC_ING_SCH_CLOS_HP_SHAPERS                        SOC_PETRA_ING_SCH_CLOS_HP_SHAPERS;
typedef SOC_TMC_ING_SCH_CLOS_SHAPERS                           SOC_PETRA_ING_SCH_CLOS_SHAPERS;
typedef SOC_TMC_ING_SCH_CLOS_INFO                              SOC_PETRA_ING_SCH_CLOS_INFO;

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
*     soc_petra_ingress_scheduler_mesh_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes:[per-destination]-shaper-rates,
*     [per-destination]-weights. NOTES:1. The 'Verify'
*     function - includes Verification of weight and shaper
*     values range. For Mesh also verification of the
*     'nof_entries' and 'id' fields in the INFO structure.2.
*     The 'Get' function - The implementation of the 'get'
*     function of the structure for the Ing Sch Mesh
*     configuration, the SOC_PETRA_ING_SCH_MESH_INFO structure is
*     a 'Fill All' function. Meaning it will get the entire
*     information of the structure from the registers, instead
*     of information for a specific context.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info -
*     mesh_info pointer to configuration structure.
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *exact_mesh_info -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *exact_mesh_info
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_mesh_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes:[per-destination]-shaper-rates,
*     [per-destination]-weights. NOTES:1. The 'Verify'
*     function - includes Verification of weight and shaper
*     values range. For Mesh also verification of the
*     'nof_entries' and 'id' fields in the INFO structure.2.
*     The 'Get' function - The implementation of the 'get'
*     function of the structure for the Ing Sch Mesh
*     configuration, the SOC_PETRA_ING_SCH_MESH_INFO structure is
*     a 'Fill All' function. Meaning it will get the entire
*     information of the structure from the registers, instead
*     of information for a specific context.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info -
*     mesh_info pointer to configuration structure.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_clos_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes:[local/fabric]-shaper-rates,
*     [local/fabric]-weights. NOTES:1. The 'Verify' function -
*     includes Verification of weight and shaper values range.
*     2. The 'Get' function - The implementation of the 'get'
*     function of the structure for the Ing Sch Clos
*     configuration, the SOC_PETRA_ING_SCH_CLOS_INFO structure is
*     a 'Fill All' function. Meaning it will get the entire
*     information of the structure from the registers, instead
*     of information for a specific context.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info -
*     clos_info pointer to configuration structure.
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_clos_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes:[local/fabric]-shaper-rates,
*     [local/fabric]-weights. NOTES:1. The 'Verify' function -
*     includes Verification of weight and shaper values range.
*     2. The 'Get' function - The implementation of the 'get'
*     function of the structure for the Ing Sch Clos
*     configuration, the SOC_PETRA_ING_SCH_CLOS_INFO structure is
*     a 'Fill All' function. Meaning it will get the entire
*     information of the structure from the registers, instead
*     of information for a specific context.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info -
*     clos_info pointer to configuration structure.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
  );

void
  soc_petra_PETRA_ING_SCH_SHAPER_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_SHAPER *info
  );

void
  soc_petra_PETRA_ING_SCH_MESH_CONTEXT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_CONTEXT_INFO *info
  );

void
  soc_petra_PETRA_ING_SCH_MESH_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_WFQ_ELEMENT_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_WFQ_ELEMENT *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_WFQS_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_WFQS *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_HP_SHAPERS_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_HP_SHAPERS *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_SHAPERS_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_SHAPERS *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_ING_SCH_MESH_CONTEXTS_to_string(
    SOC_SAND_IN SOC_PETRA_ING_SCH_MESH_CONTEXTS enum_val
  );

void
  soc_petra_PETRA_ING_SCH_SHAPER_print(
    SOC_SAND_IN SOC_PETRA_ING_SCH_SHAPER *info
  );

void
  soc_petra_PETRA_ING_SCH_MESH_CONTEXT_INFO_print(
    SOC_SAND_IN SOC_PETRA_ING_SCH_MESH_CONTEXT_INFO *info
  );

void
  soc_petra_PETRA_ING_SCH_MESH_INFO_print(
    SOC_SAND_IN SOC_PETRA_ING_SCH_MESH_INFO *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_WFQ_ELEMENT_print(
    SOC_SAND_IN SOC_PETRA_ING_SCH_CLOS_WFQ_ELEMENT *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_WFQS_print(
    SOC_SAND_IN SOC_PETRA_ING_SCH_CLOS_WFQS *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_HP_SHAPERS_print(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_HP_SHAPERS *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_SHAPERS_print(
    SOC_SAND_IN SOC_PETRA_ING_SCH_CLOS_SHAPERS *info
  );

void
  soc_petra_PETRA_ING_SCH_CLOS_INFO_print(
    SOC_SAND_IN SOC_PETRA_ING_SCH_CLOS_INFO *info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_conversion_test_api
* TYPE:
*   PROC
* DATE:
*   Oct 12 2007
* FUNCTION:
*     This procedure perform a test that compares the rate values to
*     the exact rate values that are received after the
*     conversion function.
*     The procedure tests 2 main criteria:
*     1. That the exact error percentage from the rate does not exceed limit.
*     2. That the exact is always larger than rate.
* INPUT:
*   SOC_SAND_IN uint8 is_regression - if true less values are checked.
* RETURNS:
*   SOC_SAND_DIRECT: uint8 pass - whether the test has passed or not.
*
*
*********************************************************************/

uint8
  soc_petra_ingress_scheduler_conversion_test_api(
    SOC_SAND_IN uint8 is_regression,
    SOC_SAND_IN uint8 silent
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_INGRESS_SCHEDULER_INCLUDED__*/
#endif
