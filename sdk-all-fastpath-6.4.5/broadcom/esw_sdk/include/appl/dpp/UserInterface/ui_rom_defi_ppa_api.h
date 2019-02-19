/* $Id: ui_rom_defi_ppa_api.h,v 1.2 Broadcom SDK $
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

#ifndef __UI_ROM_DEFI_PPA_API_INCLUDED__
/* { */
#define __UI_ROM_DEFI_PPA_API_INCLUDED__

#include <appl/dpp/UserInterface/ui_pure_defi_ppa_api.h>    

EXTERN CONST
   PARAM_VAL_RULES
     ppa_api_mac_addr_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        12,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    MAC address parameter. 12 hex digits. No 0 no 0x Prefix",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;  

                                                                              
EXTERN CONST
   PARAM_VAL_RULES
     ppa_api_ipv6_addr_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        32,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    IPV6 address parameter. 12 hex digits. No 0 no 0x Prefix",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;  

EXTERN CONST 
   PARAM_VAL_RULES 
     ppa_api_free_vals[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_NUMERIC,SYMB_NAME_NUMERIC, 
    { 
      { 
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE, 
        0,0, 
          /* 
           * Number of times this value can be repeated (i.e., 
           * value is an array). Must be at least 1! 
           */ 
        1, 
        (VAL_PROC_PTR)NULL 
      } 
    } 
  }, 
/* 
 * Last element. Do not remove. 
 */ 
  { 
    VAL_END_OF_LIST 
  } 
} 
#endif 
; 
EXTERN CONST 
   PARAM_VAL_RULES 
     ppa_api_empty_vals[] 
#ifdef INIT 
   = 
{ 
/* 
 * Last element. Do not remove. 
 */ 
  { 
    VAL_END_OF_LIST 
  } 
} 
#endif 
; 
/********************************************************/        
/********************************************************/        
EXTERN CONST 
   PARAM 
     ppa_api_params[] 
#ifdef INIT 
   = 
{ 
  { 
    PARAM_PPA_BRIDGE_ROUTER_APP_ID, 
    "bridge_router_app", 
    (PARAM_VAL_RULES *)&ppa_api_empty_vals[0], 
    (sizeof(ppa_api_empty_vals) / sizeof(ppa_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(2), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_BRIDGE_ROUTER_APP_FLOW_BASED_MODE_ID, 
    "flow_based_mode", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(2), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_ID, 
    "vpls_app", 
    (PARAM_VAL_RULES *)&ppa_api_empty_vals[0], 
    (sizeof(ppa_api_empty_vals) / sizeof(ppa_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_FLOW_BASED_MODE_ID, 
    "flow_based_mode", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_POLL_INTERRUPT_ID, 
    "interrupt_not_poll", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_OAM_APP_ID, 
    "oam_enable", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_NOF_MP_SERVICES_ID, 
    "nof_mp_services", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_NOF_P2P_SERVICES_ID, 
    "nof_p2p_services", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
/* 
 * Last element. Do not remove.
 */ 
  { 
    PARAM_END_OF_LIST 
  } 
} 
#endif  
;  
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __UI_ROM_DEFI_PPA_API_INCLUDED__*/
#endif
