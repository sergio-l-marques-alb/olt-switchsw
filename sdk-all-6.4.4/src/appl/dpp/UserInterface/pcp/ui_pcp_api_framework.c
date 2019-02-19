/* $Id: ui_pcp_api_framework.c,v 1.5 Broadcom SDK $
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
/* 
 * Utilities include file. 
 */ 
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_pcp_api.h> 
 
  
#include <soc/dpp/PCP/pcp_api_framework.h>
#include <appl/dpp/UserInterface/pcp/ui_pcp_api_framework.h>

#if LINK_PCP_LIBRARIES

#ifdef UI_FRAMEWORK
/******************************************************************** 
 *  Function handler: procedure_desc_add (section framework)
 ********************************************************************/
int 
  ui_pcp_api_framework_procedure_desc_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_api_framework"); 
  soc_sand_proc_name = "pcp_procedure_desc_add"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_procedure_desc_add(
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_procedure_desc_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_procedure_desc_add");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: errors_desc_add (section framework)
 ********************************************************************/
int 
  ui_pcp_api_framework_errors_desc_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_api_framework"); 
  soc_sand_proc_name = "pcp_errors_desc_add"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_errors_desc_add(
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_errors_desc_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_errors_desc_add");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_FRAMEWORK/* { framework*/
/******************************************************************** 
 *  Section handler: framework
 ********************************************************************/ 
int 
  ui_pcp_api_framework( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pcp_api_framework"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_PROCEDURE_DESC_ADD_PROCEDURE_DESC_ADD_ID,1)) 
  { 
    ret = ui_pcp_api_framework_procedure_desc_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_ERRORS_DESC_ADD_ERRORS_DESC_ADD_ID,1)) 
  { 
    ret = ui_pcp_api_framework_errors_desc_add(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after framework***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* framework */ 


#endif /* LINK_PCP_LIBRARIES */ 

