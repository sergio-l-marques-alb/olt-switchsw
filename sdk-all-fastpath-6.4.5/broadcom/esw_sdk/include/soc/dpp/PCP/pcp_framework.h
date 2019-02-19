/* $Id: pcp_framework.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PCP_FRAMEWORK_INCLUDED__
/* { */
#define __SOC_PCP_FRAMEWORK_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PCP/pcp_api_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_PROC_DESC_BASE_GENERAL_FIRST                  (SOC_PCP_PROC_BITS)
#define PCP_PROC_DESC_BASE_GENERAL_LAST                   (PCP_PROC_DESC_BASE_GENERAL_FIRST + 30)
#define PCP_PROC_DESC_BASE_MGMT_FIRST                     (PCP_PROC_DESC_BASE_GENERAL_LAST)
#define PCP_PROC_DESC_BASE_MGMT_LAST                      (PCP_PROC_DESC_BASE_MGMT_FIRST + 60)
#define PCP_PROC_DESC_BASE_INTERRUPTS_FIRST               (PCP_PROC_DESC_BASE_MGMT_LAST)
#define PCP_PROC_DESC_BASE_INTERRUPTS_LAST                (PCP_PROC_DESC_BASE_INTERRUPTS_FIRST + 12)
#define PCP_PROC_DESC_BASE_CALLBACK_FIRST                 (PCP_PROC_DESC_BASE_INTERRUPTS_LAST)
#define PCP_PROC_DESC_BASE_CALLBACK_LAST                  (PCP_PROC_DESC_BASE_CALLBACK_FIRST + 12)
#define PCP_PROC_DESC_BASE_STATISTICS_FIRST               (PCP_PROC_DESC_BASE_CALLBACK_LAST)
#define PCP_PROC_DESC_BASE_STATISTICS_LAST                (PCP_PROC_DESC_BASE_STATISTICS_FIRST + 20)
#define PCP_PROC_DESC_BASE_DIAGNOSTICS_FIRST              (PCP_PROC_DESC_BASE_STATISTICS_LAST)
#define PCP_PROC_DESC_BASE_DIAGNOSTICS_LAST               (PCP_PROC_DESC_BASE_DIAGNOSTICS_FIRST + 20)
#define PCP_PROC_DESC_BASE_OAM_GENERAL_FIRST              (PCP_PROC_DESC_BASE_DIAGNOSTICS_LAST)
#define PCP_PROC_DESC_BASE_OAM_GENERAL_LAST               (PCP_PROC_DESC_BASE_OAM_GENERAL_FIRST + 32)
#define PCP_PROC_DESC_BASE_OAM_ETH_FIRST                  (PCP_PROC_DESC_BASE_OAM_GENERAL_LAST)
#define PCP_PROC_DESC_BASE_OAM_ETH_LAST                   (PCP_PROC_DESC_BASE_OAM_ETH_FIRST + 100)
#define PCP_PROC_DESC_BASE_OAM_BFD_FIRST                  (PCP_PROC_DESC_BASE_OAM_ETH_LAST)
#define PCP_PROC_DESC_BASE_OAM_BFD_LAST                   (PCP_PROC_DESC_BASE_OAM_BFD_FIRST + 52)
#define PCP_PROC_DESC_BASE_OAM_MPLS_FIRST                 (PCP_PROC_DESC_BASE_OAM_BFD_LAST)
#define PCP_PROC_DESC_BASE_OAM_MPLS_LAST                  (PCP_PROC_DESC_BASE_OAM_MPLS_FIRST + 60)
#define PCP_PROC_DESC_BASE_FRAMEWORK_FIRST                (PCP_PROC_DESC_BASE_OAM_MPLS_LAST)
#define PCP_PROC_DESC_BASE_FRAMEWORK_LAST                 (PCP_PROC_DESC_BASE_FRAMEWORK_FIRST + 14)
#define PCP_PROC_DESC_BASE_CHIP_REGS_FIRST                (PCP_PROC_DESC_BASE_FRAMEWORK_LAST)
#define PCP_PROC_DESC_BASE_CHIP_REGS_LAST                 (PCP_PROC_DESC_BASE_CHIP_REGS_FIRST + 20)
#define PCP_PROC_DESC_BASE_CHIP_TBLS_FIRST                (PCP_PROC_DESC_BASE_CHIP_REGS_LAST)
#define PCP_PROC_DESC_BASE_CHIP_TBLS_LAST                 (PCP_PROC_DESC_BASE_CHIP_TBLS_FIRST + 40)
#define PCP_PROC_DESC_BASE_TBL_ACCESS_FIRST               (PCP_PROC_DESC_BASE_CHIP_TBLS_LAST)
#define PCP_PROC_DESC_BASE_TBL_ACCESS_LAST                (PCP_PROC_DESC_BASE_TBL_ACCESS_FIRST + 130)
#define PCP_PROC_DESC_BASE_FRWRD_MACT_FIRST               (PCP_PROC_DESC_BASE_TBL_ACCESS_LAST)
#define PCP_PROC_DESC_BASE_FRWRD_MACT_LAST                (PCP_PROC_DESC_BASE_FRWRD_MACT_FIRST + 45)
#define PCP_PROC_DESC_BASE_LEM_ACCESS_FIRST               (PCP_PROC_DESC_BASE_FRWRD_MACT_LAST)
#define PCP_PROC_DESC_BASE_LEM_ACCESS_LAST                (PCP_PROC_DESC_BASE_LEM_ACCESS_FIRST + 10)
#define PCP_PROC_DESC_BASE_FRWRD_MACT_MGMT_FIRST          (PCP_PROC_DESC_BASE_LEM_ACCESS_LAST)
#define PCP_PROC_DESC_BASE_FRWRD_MACT_MGMT_LAST           (PCP_PROC_DESC_BASE_FRWRD_MACT_MGMT_FIRST + 150)
#define PCP_PROC_DESC_BASE_FRWRD_ILM_FIRST                (PCP_PROC_DESC_BASE_FRWRD_MACT_MGMT_LAST)
#define PCP_PROC_DESC_BASE_FRWRD_ILM_LAST                 (PCP_PROC_DESC_BASE_FRWRD_ILM_FIRST + 35)
#define PCP_PROC_DESC_BASE_FRWRD_IPV4_FIRST                (PCP_PROC_DESC_BASE_FRWRD_ILM_LAST)
#define PCP_PROC_DESC_BASE_FRWRD_IPV4_LAST                 (PCP_PROC_DESC_BASE_FRWRD_IPV4_FIRST + 80)
#define PCP_PROC_DESC_BASE_ARR_MEM_ALLOCATOR_FIRST         (PCP_PROC_DESC_BASE_FRWRD_IPV4_LAST)

#define PCP_ERR_DESC_BASE_GENERAL_FIRST                   (SOC_SAND_PCP_START_ERR_NUMBER)
#define PCP_ERR_DESC_BASE_GENERAL_LAST                    (PCP_ERR_DESC_BASE_GENERAL_FIRST + 55)
#define PCP_ERR_DESC_BASE_MGMT_FIRST                      (PCP_ERR_DESC_BASE_GENERAL_LAST)
#define PCP_ERR_DESC_BASE_MGMT_LAST                       (PCP_ERR_DESC_BASE_MGMT_FIRST + 21)
#define PCP_ERR_DESC_BASE_INTERRUPTS_FIRST                (PCP_ERR_DESC_BASE_MGMT_LAST)
#define PCP_ERR_DESC_BASE_INTERRUPTS_LAST                 (PCP_ERR_DESC_BASE_INTERRUPTS_FIRST + 5)
#define PCP_ERR_DESC_BASE_CALLBACK_FIRST                  (PCP_ERR_DESC_BASE_INTERRUPTS_LAST)
#define PCP_ERR_DESC_BASE_CALLBACK_LAST                   (PCP_ERR_DESC_BASE_CALLBACK_FIRST + 5)
#define PCP_ERR_DESC_BASE_STATISTICS_FIRST                (PCP_ERR_DESC_BASE_CALLBACK_LAST)
#define PCP_ERR_DESC_BASE_STATISTICS_LAST                 (PCP_ERR_DESC_BASE_STATISTICS_FIRST + 6)
#define PCP_ERR_DESC_BASE_DIAGNOSTICS_FIRST               (PCP_ERR_DESC_BASE_STATISTICS_LAST)
#define PCP_ERR_DESC_BASE_DIAGNOSTICS_LAST                (PCP_ERR_DESC_BASE_DIAGNOSTICS_FIRST + 5)
#define PCP_ERR_DESC_BASE_OAM_GENERAL_FIRST               (PCP_ERR_DESC_BASE_DIAGNOSTICS_LAST)
#define PCP_ERR_DESC_BASE_OAM_GENERAL_LAST                (PCP_ERR_DESC_BASE_OAM_GENERAL_FIRST + 12)
#define PCP_ERR_DESC_BASE_OAM_ETH_FIRST                   (PCP_ERR_DESC_BASE_OAM_GENERAL_LAST)
#define PCP_ERR_DESC_BASE_OAM_ETH_LAST                    (PCP_ERR_DESC_BASE_OAM_ETH_FIRST + 40)
#define PCP_ERR_DESC_BASE_OAM_BFD_FIRST                   (PCP_ERR_DESC_BASE_OAM_ETH_LAST)
#define PCP_ERR_DESC_BASE_OAM_BFD_LAST                    (PCP_ERR_DESC_BASE_OAM_BFD_FIRST + 29)
#define PCP_ERR_DESC_BASE_OAM_MPLS_FIRST                  (PCP_ERR_DESC_BASE_OAM_BFD_LAST)
#define PCP_ERR_DESC_BASE_OAM_MPLS_LAST                   (PCP_ERR_DESC_BASE_OAM_MPLS_FIRST + 26)
#define PCP_ERR_DESC_BASE_FRAMEWORK_FIRST                 (PCP_ERR_DESC_BASE_OAM_MPLS_LAST)
#define PCP_ERR_DESC_BASE_FRAMEWORK_LAST                  (PCP_ERR_DESC_BASE_FRAMEWORK_FIRST + 10)
#define PCP_ERR_DESC_BASE_CHIP_REGS_FIRST                 (PCP_ERR_DESC_BASE_FRAMEWORK_LAST)
#define PCP_ERR_DESC_BASE_CHIP_REGS_LAST                  (PCP_ERR_DESC_BASE_CHIP_REGS_FIRST + 20)
#define PCP_ERR_DESC_BASE_CHIP_TBLS_FIRST                 (PCP_ERR_DESC_BASE_CHIP_REGS_LAST)
#define PCP_ERR_DESC_BASE_CHIP_TBLS_LAST                  (PCP_ERR_DESC_BASE_CHIP_TBLS_FIRST + 40)
#define PCP_ERR_DESC_BASE_TBL_ACCESS_FIRST                (PCP_ERR_DESC_BASE_CHIP_TBLS_LAST)
#define PCP_ERR_DESC_BASE_TBL_ACCESS_LAST                 (PCP_ERR_DESC_BASE_TBL_ACCESS_FIRST + 60)
#define PCP_ERR_DESC_BASE_FRWRD_MACT_FIRST                (PCP_ERR_DESC_BASE_TBL_ACCESS_LAST)
#define PCP_ERR_DESC_BASE_FRWRD_MACT_LAST                 (PCP_ERR_DESC_BASE_FRWRD_MACT_FIRST + 25)
#define PCP_ERR_DESC_BASE_LEM_ACCESS_FIRST                (PCP_ERR_DESC_BASE_FRWRD_MACT_LAST)
#define PCP_ERR_DESC_BASE_LEM_ACCESS_LAST                 (PCP_ERR_DESC_BASE_LEM_ACCESS_FIRST + 35)
#define PCP_ERR_DESC_BASE_FRWRD_MACT_MGMT_FIRST           (PCP_ERR_DESC_BASE_LEM_ACCESS_LAST)
#define PCP_ERR_DESC_BASE_FRWRD_MACT_MGMT_LAST            (PCP_ERR_DESC_BASE_FRWRD_MACT_MGMT_FIRST + 35)
#define PCP_ERR_DESC_BASE_FRWRD_ILM_FIRST                 (PCP_ERR_DESC_BASE_FRWRD_MACT_MGMT_LAST)
#define PCP_ERR_DESC_BASE_FRWRD_ILM_LAST                  (PCP_ERR_DESC_BASE_FRWRD_ILM_FIRST + 10)
#define PCP_ERR_DESC_BASE_FRWRD_IPV4_FIRST                (PCP_ERR_DESC_BASE_FRWRD_ILM_LAST)
#define PCP_ERR_DESC_BASE_FRWRD_IPV4_LAST                 (PCP_ERR_DESC_BASE_FRWRD_IPV4_FIRST + 40)
#define PCP_ERR_DESC_BASE_ARRAY_MEMORY_ALLOCATOR_FIRST   (PCP_ERR_DESC_BASE_FRWRD_IPV4_LAST)



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
   * Auto generated. Do not edit following section {
   */
  PCP_PROCEDURE_DESC_ADD = PCP_PROC_DESC_BASE_FRAMEWORK_FIRST,
  PCP_ERRORS_DESC_ADD,
  PCP_FRAMEWORK_GET_PROCS_PTR,
  PCP_FRAMEWORK_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_FRAMEWORK_PROCEDURE_DESC_LAST
} PCP_FRAMEWORK_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  PCP_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR = PCP_ERR_DESC_BASE_FRAMEWORK_FIRST,
  PCP_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR,
  PCP_VAL_IS_OUT_OF_RANGE_ERR,
  PCP_ALLOC_TO_NON_NULL_ERR,


  /*
   * Last element. Do no touch.
   */
  PCP_FRAMEWORK_ERR_LAST
} PCP_FRAMEWORK_ERR;


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
 *   pcp_framework_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_api_framework module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_framework_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_framework_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_api_framework module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_framework_get_errs_ptr(void);

uint32
  pcp_disp_result(
    SOC_SAND_IN uint32          pcp_api_result,
    SOC_SAND_IN char              *proc_name
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_FRAMEWORK_INCLUDED__*/
#endif

