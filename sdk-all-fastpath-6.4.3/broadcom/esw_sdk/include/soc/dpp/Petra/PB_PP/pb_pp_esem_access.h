/* $Id: pb_pp_esem_access.h,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_esem_access.h
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

#ifndef __SOC_PB_PP_ESEM_INCLUDED__
/* { */
#define __SOC_PB_PP_ESEM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PB_PP_ESEM_ADDR                                      (0x00280000)
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
  SOC_PB_PP_ESEM_GET_PROCS_PTR = SOC_PB_PP_PROC_DESC_BASE_ESEM_FIRST,
  SOC_PB_PP_ESEM_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_ESEM_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_ESEM_ENTRY_GET_UNSAFE,
  SOC_PB_PP_ESEM_ENTRY_REMOVE_UNSAFE,
  SOC_PB_PP_ESEM_TBL_WRITE,
  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_ESEM_PROCEDURE_DESC_LAST
} SOC_PB_PP_ESEM_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_ESEM_NOT_READY_ERR = SOC_PB_PP_ERR_DESC_BASE_ESEM_FIRST,
  SOC_PB_PP_ESEM_KEY_TYPE_INVALID_ERR,
  SOC_PB_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR,
  SOC_PB_PP_ESEM_NOT_READ_TIMEOUT_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_ESEM_ERR_LAST
} SOC_PB_PP_ESEM_ERR;

typedef enum
{
  /*
   *	VLAN Domain x VSI
   */
  SOC_PB_PP_ESEM_KEY_TYPE_VD_VSI,
  /*
   *	VLAN Domain x C-VID
   */
  SOC_PB_PP_ESEM_KEY_TYPE_VD_CVID,
  SOC_PB_PP_NOF_ESEM_KEY_TYPES
}SOC_PB_PP_ESEM_KEY_TYPE;

typedef union
{
  struct
  {
    uint32 vd;
    uint32 vsi;
  } vd_vsi;

  struct
  {
    uint32 vd;
    uint32 cvid;
  } vd_cvid;

} SOC_PB_PP_ESEM_KEY_INFO;

typedef struct
{
  SOC_PB_PP_ESEM_KEY_TYPE key_type;
  SOC_PB_PP_ESEM_KEY_INFO key_info;
} SOC_PB_PP_ESEM_KEY;

typedef struct
{
  uint32 out_ac;
} SOC_PB_PP_ESEM_ENTRY;

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
  soc_pb_pp_esem_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_OUT SOC_PB_PP_ESEM_ENTRY                          *esem_entry,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_esem_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ESEM_KEY                            *svem_key,
    SOC_SAND_IN  SOC_PB_PP_ESEM_ENTRY                          *svem_entry,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_esem_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ESEM_KEY                            *svem_key,
    SOC_SAND_OUT uint8                                 *success
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_esem_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_esem module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_esem_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_esem_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_esem module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_esem_get_errs_ptr(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_ESEM_INCLUDED__*/
#endif
