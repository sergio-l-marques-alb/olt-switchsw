/* $Id: pcp_diagnostics.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PCP_DIAGNOSTICS_INCLUDED__
/* { */
#define __SOC_PCP_DIAGNOSTICS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PCP/pcp_api_diagnostics.h>
#include <soc/dpp/PCP/pcp_framework.h>

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
   * Auto generated. Do not edit following section {
   */
  PCP_DIAG_STS_SYNC_STATUS_GET = PCP_PROC_DESC_BASE_DIAGNOSTICS_FIRST,
  PCP_DIAG_STS_SYNC_STATUS_GET_PRINT,
  PCP_DIAG_STS_SYNC_STATUS_GET_UNSAFE,
  PCP_DIAG_STS_SYNC_STATUS_GET_VERIFY,
  PCP_DIAG_DMA_RX_STATUS_GET,
  PCP_DIAG_DMA_RX_STATUS_GET_PRINT,
  PCP_DIAG_DMA_RX_STATUS_GET_UNSAFE,
  PCP_DIAG_DMA_RX_STATUS_GET_VERIFY,
  PCP_DIAGNOSTICS_GET_PROCS_PTR,
  PCP_DIAGNOSTICS_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_DIAGNOSTICS_PROCEDURE_DESC_LAST
} PCP_DIAGNOSTICS_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_DIAGNOSTICS_ERR_LAST
} PCP_DIAGNOSTICS_ERR;


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
 *   pcp_diag_sts_sync_status_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This API returns the sync status of the statistics
 *   interface
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access
 *   SOC_SAND_OUT uint8                       *status -
 *     T - The statistics interface is syncedF - The statistics
 *     interface is not synced
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_diag_sts_sync_status_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT uint8                       *status
  );

uint32
  pcp_diag_sts_sync_status_get_verify(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
* NAME:
 *   pcp_diag_dma_rx_status_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This API returns if there are any packets waiting to be
 *   handled by the CPU on the class specified
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access
 *   SOC_SAND_IN  SOC_SAND_PP_TC                      traffic_class -
 *     Select the traffic class that should be handled
 *   SOC_SAND_OUT uint8                       *status -
 *     T - There are packets pending on the specified class. F -
 *     There are no packets pending on the specified class.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_diag_dma_rx_status_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_PP_TC                      traffic_class,
    SOC_SAND_OUT uint8                       *status
  );

uint32
  pcp_diag_dma_rx_status_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_PP_TC                      traffic_class
  );

/*********************************************************************
* NAME:
 *   pcp_diagnostics_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_api_diagnostics module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_diagnostics_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_diagnostics_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_api_diagnostics module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_diagnostics_get_errs_ptr(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_DIAGNOSTICS_INCLUDED__*/
#endif

