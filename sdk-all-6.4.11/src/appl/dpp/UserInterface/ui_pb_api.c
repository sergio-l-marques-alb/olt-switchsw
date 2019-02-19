/* $Id: ui_pb_api.c,v 1.6 Broadcom SDK $
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
/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>
                                                               
#include <appl/dpp/UserInterface/pb/ui_pb_api_nif.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_ports.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_pmf_low_level_diag.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_flow_control.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_cnt.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_diagnostics.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_cnm.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_action_cmd.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_egr_queuing.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_pmf_low_level.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_tdm.h>
#include <appl/dpp/UserInterface/ui_pure_defi_pb_api.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_ingress_traffic_mgmt.h>                                                               
#include <appl/dpp/UserInterface/pb/ui_pb_api_egr_acl.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_tcam.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_stack.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_mgmt.h>
#if LINK_PB_PP_LIBRARIES
#include <appl/dpp/UserInterface/pb/ui_pb_pp_api_mgmt.h>
#endif
#if LINK_PB_LIBRARIES

uint32
  soc_pb_get_default_unit()
{
  return soc_petra_get_default_unit();
}


/*****************************************************
*NAME
*  subject_pb_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_pb_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_pb_api(current_line,current_line_ptr)
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
  subject_pb_api(
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
    send_string_to_screen("'subject_pb_api()' function was called with no parameters.\n\r",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_pb_api').
   */

  send_array_to_screen("\n\r",2) ;




#ifdef UI_NIF
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_NIF_ID,1))
  {
    ret = ui_pb_api_nif(current_line);
  }
#endif /* UI_NIF */
#ifdef UI_PORTS
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_PORTS_ID,1))
  {
    ret = ui_pb_api_ports(current_line);
  }
#endif /* UI_PORTS */
#ifdef UI_EGR_QUEUING
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_EGR_QUEUING_ID,1))
  {
    ret = ui_pb_api_egr_queuing(current_line);
  }
#endif /* UI_EGR_QUEUING */
#ifdef UI_FLOW_CONTROL
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_FLOW_CONTROL_ID,1))
  {
    ret = ui_pb_api_flow_control(current_line);
  }
#endif /* UI_FLOW_CONTROL */
#ifdef UI_PMF_LOW_LEVEL_DIAG
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_PMF_LOW_LEVEL_DIAG_ID,1))
  {
    ret = ui_pb_api_pmf_low_level_diag(current_line);
  }
#endif /* UI_PMF_LOW_LEVEL_DIAG */

#ifdef UI_PMF_LOW_LEVEL
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_PMF_LOW_LEVEL_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level(current_line); 
  } 
#endif /* UI_PMF_LOW_LEVEL */
#ifdef UI_CNT
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_CNT_ID,1)) 
  { 
    ret = ui_pb_api_cnt(current_line); 
  } 
#endif /* UI_CNT */
#ifdef UI_TDM
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_TDM_ID,1)) 
  { 
    ret = ui_pb_api_tdm(current_line); 
  } 
#endif /* UI_TDM */
#ifdef UI_ACTION_CMD
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_ACTION_CMD_ID,1)) 
  { 
    ret = ui_pb_api_action_cmd(current_line); 
  } 
#endif /* UI_ACTION_CMD */
#ifdef UI_DIAGNOSTICS
 else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_DIAGNOSTICS_ID,1)) 
  { 
    ret = ui_pb_api_diagnostics(current_line); 
  } 
#endif /* UI_DIAGNOSTICS */

#ifdef UI_CNM
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_CNM_ID,1)) 
  { 
    ret = ui_pb_api_cnm(current_line); 
  } 
#endif /* UI_CNM */
#ifdef UI_INGRESS_TRAFFIC_MGMT
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_INGRESS_TRAFFIC_MGMT_ID,1)) 
  { 
    ret = ui_pb_api_ingress_traffic_mgmt(current_line); 
  } 
#endif /* UI_INGRESS_TRAFFIC_MGMT */
#ifdef UI_EGR_ACL
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_EGR_ACL_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl(current_line); 
  } 
#endif /* UI_EGR_ACL */
#ifdef UI_TCAM
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_TCAM_ID,1)) 
  { 
    ret = ui_pb_api_tcam(current_line); 
  } 
#endif /* UI_TCAM */
#ifdef UI_STACK
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_STACK_ID,1)) 
  { 
    ret = ui_pb_api_stack(current_line); 
  } 
#endif /* UI_STACK */
#ifdef UI_MGMT
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_MGMT_ID,1)) 
  { 
    ret = ui_pb_api_mgmt(current_line); 
  } 
#endif /* UI_MGMT */
#if LINK_PB_PP_LIBRARIES
#ifdef UI_PP_MGMT
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PP_API_MGMT_ID,1)) 
  { 
    ret = ui_pb_pp_api_mgmt(current_line); 
  } 
#endif /* UI_PP_MGMT */
#endif /* LINK_PB_PP_LIBRARIES */

  else 
  { 
    /* 
     * Enter if an unknown request. 
     */ 
    send_string_to_screen( 
      "\n\r" 
      "*** soc_pb_api command with unknown parameters'.\n\r" 
      "    Syntax error/sw error...\n\r", 
      TRUE)  ; 
    ret = TRUE  ; 
    goto exit  ; 
  } 
 
exit: 
  return (ret); 
} 

#endif /* LINK_PB_LIBRARIES */

