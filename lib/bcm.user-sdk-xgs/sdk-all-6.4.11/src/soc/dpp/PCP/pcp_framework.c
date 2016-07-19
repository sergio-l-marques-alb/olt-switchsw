/* $Id: pcp_framework.c,v 1.8 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

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

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_framework[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_PROCEDURE_DESC_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ERRORS_DESC_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRAMEWORK_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRAMEWORK_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_framework[] =
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
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_api_framework module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_framework_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_framework;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_framework module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_framework_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_framework;
}

uint32
  pcp_get_err_text(
    uint32  err_id,
    char           **err_name,
    char           **err_text
  )
{
  char
    *error_name,
    *error_description = NULL;
  uint16
      error_code ;
  int32
    err;
  CONST SOC_ERROR_DESC_ELEMENT
    *error_desc_element_ptr;
  uint8
    err_found = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  error_code = soc_sand_get_error_code_from_error_word(err_id) ;
  err = soc_sand_error_code_to_string(
          (uint32)error_code,&error_name,&error_description
        );

  err_found = SOC_SAND_NUM2BOOL(err == 0);

  if (!err_found)
  {

    /*
     * First search soc_petra errors.
     */
    if (!err_found)
    {

      soc_sand_linear_find_error(
        (uint32)error_code,
        pcp_errors_ptr_get(),&error_desc_element_ptr) ;

      err_found = SOC_SAND_NUM2BOOL(error_desc_element_ptr != NULL);
      if (err_found)
      {
        error_description = (char *)error_desc_element_ptr->err_text ;
      }
    }
    if (!err_found)
    {
      /*
       * Soc_petra search has failed. Search soc_sand errors.
       */
      soc_sand_linear_find_error(
        (uint32)error_code,
        soc_sand_get_errors_ptr(),&error_desc_element_ptr) ;
      err_found = SOC_SAND_NUM2BOOL(error_desc_element_ptr != NULL);
      if (err_found)
      {
        error_description = (char *)error_desc_element_ptr->err_text ;
      }
    }
  }
  if (error_description)
  {
    *err_text = error_description ;
  }
  else
  {
    *err_text = "No error text" ;
    soc_sand_set_error_code_into_error_word(
                0,&ex) ;
  }

  if (error_name)
  {
    *err_name = error_name;
  }
  else
  {
    *err_name = "No error name" ;
  }

  if (ex != no_err)
  {
    soc_sand_error_handler(ex, "error in pcp_get_err_text()", exit_place,0,0,0,0,0) ;
  }
  return ex ;
}

/*****************************************************
*NAME
*  pcp_disp_result
*TYPE: PROC
*DATE: 18/FEB/2003
*FUNCTION:
*  Display return value and related information for
*  SOC_PETRA driver services.
*CALLING SEQUENCE:
*  disp_pcp_result(result.error_id,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    uint32 result.error_id -
*      Return value of any FE driver service.
*    int8          *proc_name -
*      Name of procedure for which 'result.error_id'
*      was returned.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int32 -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  pcp_disp_result(
    SOC_SAND_IN uint32          pcp_api_result,
    SOC_SAND_IN char              *proc_name
  )
{
  int32
    ret ;
  uint32
      proc_id ,
      res = 0;
  char
    *err_name,
    *err_text,
    *soc_sand_proc_name,
    *soc_sand_module_name ;

  ret = 0 ;

  res = pcp_get_err_text(
        pcp_api_result,
        &err_name,&err_text
      );
  if (soc_sand_get_error_code_from_error_word(res)!= SOC_SAND_OK)
  {
    err_text = "No error code description (or procedure id) found" ;
  }

  proc_id = soc_sand_get_proc_id_from_error_word(pcp_api_result) ;

  pcp_proc_id_to_string(
    (uint32)proc_id,
    &soc_sand_module_name,
    &soc_sand_proc_name
  );
  if (soc_sand_get_error_code_from_error_word(pcp_api_result) != SOC_SAND_OK)
  {
    LOG_CLI((BSL_META("\n\r"
                      "**>\'%s\' -\n\r"
                      "Err code: 0x%X (fail):\n\r"
                      "Name: %s\n\r" /*Error name*/
                      "Desc: %s\n\r" /*Error description*/
                      "Procedure id: 0x%04X (Mod: %s, Proc: %s)\n\r"),
             proc_name,pcp_api_result, err_name, err_text,
             (uint8)proc_id,soc_sand_module_name,soc_sand_proc_name));
  }
  else
  {
    /*
     * No print on success
     */
  }
  return (ret) ;
}
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

