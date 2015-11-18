/* $Id: ui_pcp_api.c,v 1.5 Broadcom SDK $
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
/* 
 * Utilities include file. 
 */ 
 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/pcp/ui_pcp_oam_api_general.h>                                                               
#include <appl/dpp/UserInterface/pcp/ui_pcp_oam_api_eth.h>                                                               
#include <appl/dpp/UserInterface/pcp/ui_pcp_oam_api_bfd.h>                                                               
#include <appl/dpp/UserInterface/pcp/ui_pcp_oam_api_mpls.h>                                                               
#include <appl/dpp/UserInterface/pcp/ui_pcp_api_framework.h>
#include <appl/dpp/UserInterface/pcp/ui_pcp_api_mgmt.h>
#include <appl/dpp/UserInterface/pcp/ui_pcp_api_statistics.h>
#include <appl/dpp/UserInterface/pcp/ui_pcp_api_diagnostics.h>                                                               


#include <appl/dpp/UserInterface/ui_pure_defi_pcp_api.h> 

#if LINK_PCP_LIBRARIES

static 
  uint32 
    Default_unit = 7; 

void 
  pcp_set_default_unit(uint32 dev_id) 
{ 
  Default_unit = dev_id; 
} 
  
uint32  
  pcp_get_default_unit() 
{ 
  return Default_unit; 
} 
  
 
/***************************************************** 
*NAME 
*  subject_pcp_api 
*TYPE: PROC 
*DATE: 29/DEC/2002 
*FUNCTION: 
*  Process input line which has an 'subject_pcp_api' subject. 
*  Input line is assumed to have been checked and 
*  found to be of right format. 
*CALLING SEQUENCE: 
*  subject_pcp_api(current_line,current_line_ptr) 
*INPUT: 
*  SOC_SAND_DIRECT: 
*    CURRENT_LINE *current_line - 
*      Pointer to prompt line to process. 
*    CURRENT_LINE **current_line_ptr - 
*      Pointer to prompt line to be displayed after 
*      this procedure finishes execution. Caller 
*      points this variable to the pointer to 
*      the next line to display. If called function wishes 
*      to set the next line to display, it replaces 
*      the pointer to the next line to display. 
*  SOC_SAND_INDIRECT: 
*    None. 
*OUTPUT: 
*  SOC_SAND_DIRECT: 
*    int - 
*      If non zero then some error has occurred. 
*  SOC_SAND_INDIRECT: 
*    Processing results. See 'current_line_ptr'. 
*REMARKS: 
*  This procedure should be carried out under 'task_safe' 
*  state (i.e., task can not be deleted while this 
*  procedure is being carried out). 
*SEE ALSO: 
*****************************************************/ 
int 
  subject_pcp_api( 
    CURRENT_LINE *current_line, 
    CURRENT_LINE **current_line_ptr 
  ) 
{ 
  unsigned int 
    match_index; 
  int 
    ret; 
 
  ret = FALSE ; 
 
  /* 
   * the rest of the line is empty 
   */ 
  if (current_line->num_param_names == 0) 
  { 
    send_string_to_screen("\n\r",FALSE) ; 
    send_string_to_screen("'subject_pcp_api()' function was called with no parameters.\n\r",FALSE) ; 
    goto exit ; 
  } 
  /* 
   * Else, there are parameters on the line (not just 'subject_pcp_api'). 
   */ 
 
  send_array_to_screen("\n\r",2) ; 
 
 
 
#ifdef UI_MGMT
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_API_MGMT_ID,1))
  { 
    ret = ui_pcp_api_mgmt(current_line); 
  } 
#endif /* UI_MGMT */
#ifdef UI_STATISTICS
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_API_STATISTICS_ID,1)) 
  { 
    ret = ui_pcp_api_statistics(current_line); 
  } 
#endif /* UI_STATISTICS */
#ifdef UI_DIAGNOSTICS
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_API_DIAGNOSTICS_ID,1)) 
  { 
    ret = ui_pcp_api_diagnostics(current_line); 
  } 
#endif /* UI_DIAGNOSTICS */
#ifdef UI_GENERAL
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_API_GENERAL_ID,1)) 
  { 
    ret = ui_pcp_oam_api_general(current_line); 
  } 
#endif /* UI_GENERAL */
#ifdef UI_ETH
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_API_ETH_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth(current_line); 
  } 
#endif /* UI_ETH */
#ifdef UI_BFD
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_API_BFD_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd(current_line); 
  } 
#endif /* UI_BFD */
#ifdef UI_MPLS
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_API_MPLS_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls(current_line); 
  } 
#endif /* UI_MPLS */
#ifdef UI_FRAMEWORK
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_API_FRAMEWORK_ID,1)) 
  { 
    ret = ui_pcp_api_framework(current_line); 
  } 
#endif /* UI_FRAMEWORK */
  else 
  { 
    /* 
     * Enter if an unknown request. 
     */ 
    send_string_to_screen( 
      "\n\r" 
      "*** pcp_api command with unknown parameters'.\n\r" 
      "    Syntax error/sw error...\n\r", 
      TRUE)  ; 
    ret = TRUE  ; 
    goto exit  ; 
  } 
 
exit: 
  return (ret); 
} 
  

#endif /* LINK_PCP_LIBRARIES */

