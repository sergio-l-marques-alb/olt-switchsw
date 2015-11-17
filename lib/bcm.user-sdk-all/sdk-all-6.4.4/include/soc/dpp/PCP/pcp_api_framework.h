/* $Id: pcp_api_framework.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PCP_API_FRAMEWORK_INCLUDED__
/* { */
#define __SOC_PCP_API_FRAMEWORK_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

typedef enum
{
  PCP_GET_ERR_TEXT = SOC_PCP_PROC_BITS,
  PCP_PROC_ID_TO_STRING,

/*
 * Procedure identifiers.
 * }
 */
  PCP_TEXT_ERR_GET,
  PCP_INTERRUPT_HANDLER_UNSAFE,

/*
 * pcp_oam_general.h
*/
	PCP_GEN_ERR_NUM_ALLOC,
	PCP_GEN_ERR_NUM_ALLOC_ANY_SET,
	PCP_GEN_ERR_NUM_FREE,
	PCP_GEN_ERR_NUM_CLEAR,

/*
 * pcp_init.h
*/
  PCP_MGMT_ECI_TBLS_INIT,
  PCP_MGMT_ELK_TBLS_INIT,
  PCP_MGMT_OAM_TBLS_INIT,
  PCP_MGMT_STS_TBLS_INIT,
  PCP_DIAG_TBLS_DUMP_TABLES_GET,
  PCP_MGMT_TBLS_INIT,

/*
 * pcp_reg_access.c
*/
  PCP_BASE_OFFSET_SET_UNSAFE,
  PCP_FIELD_FROM_REG_GET,
  PCP_FIELD_FROM_REG_SET,
  PCP_READ_FLD_UNSAFE,
  PCP_WRITE_FLD_UNSAFE,
  PCP_READ_REG_UNSAFE,
  PCP_WRITE_REG_UNSAFE,
  PCP_READ_FLD_AT_INDX_UNSAFE,
  PCP_WRITE_FLD_AT_INDX_UNSAFE,
  PCP_READ_REG_AT_INDX_UNSAFE,
  PCP_WRITE_REG_AT_INDX_UNSAFE,
  PCP_READ_REG_BUFFER_UNSAFE,
  PCP_WRITE_REG_BUFFER_UNSAFE,
  PCP_WRITE_ARRAY_OF_FLDS,
  PCP_READ_ARRAY_OF_FLDS,
  PCP_STATUS_FLD_POLL_UNSAFE,

/*
 * pcp_chip_defines.c
*/
  PCP_CHIP_TIME_TO_TICKS,
  PCP_CHIP_TICKS_TO_TIME,

 /*
   *	This must be the last field
   */
  PCP_NOF_PROCS
} PCP_PROCS;


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

CONST SOC_ERROR_DESC_ELEMENT
  *pcp_errors_ptr_get(
    void
  );

CONST SOC_PROCEDURE_DESC_ELEMENT
  *pcp_procedures_ptr_get(
    void
  );

uint32
  pcp_proc_id_to_string(
    uint32 in_proc_id,
    char          **out_module_name,
    char          **out_proc_name
  );

/*********************************************************************
* NAME:
 *   pcp_procedure_desc_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add the pool of SOC_SAND_FAP20V procedure descriptors to the
 *   all-system sorted pool.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_procedure_desc_add(void);

/*********************************************************************
* NAME:
 *   pcp_errors_desc_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add the pool of error descriptors to the all-system
 *   sorted pool.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_errors_desc_add(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_API_FRAMEWORK_INCLUDED__*/
#endif

