
/* $Id: jer_pp_eg_encap_access.h,v 1.20 Broadcom SDK $
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

#ifndef __JER_PP_EG_ENCAP_ACCESS_INCLUDED__
/* { */
#define __JER_PP_EG_ENCAP_ACCESS_INCLUDED__


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

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

 


/* format of eedb entry (88b):
 * type[87:85] (3b)  pcp-dei-profile[84:83] (2b) drop[82] (1b) roo-link-format-identifier[81] (1b) DA[80:33] (48b) SA-LSB[32:21] (12b)
 *    Ether-Type-Index[20:17] (4b) Number of tags[16:15] (2b) Remark-Profile[14:12] (3b) Outer-tag-MSB[11:0] (12b)
*/
typedef struct 
{
  uint32 pcp_dei_profile;
  uint32 drop; 
  uint32 roo_link_format_identifier; 
  uint32 dest_mac[2];
  uint32 sa_lsb; 
  uint32 ether_type_index; 
  uint32 nof_tags; 
  uint32 remark_profile; 
  uint32 outer_tag_pcp_dei; 
  /* Outer-tag-PCP-DEI[11:8] Outer_tag_msb [7:0] */
  uint32 outer_tag_msb;  
} JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT; 


/* format of ROO Link Layer additional quarter entry (20b):
 * roo-outer-vlan-lsbs[19:16] pcp-dei-inner-vlan[15:12] roo-inner-vlan[11:0] */
typedef struct 
{
    uint32 roo_inner_vlan; 
    uint32 pcp_dei_inner_vlan; 
    uint32 roo_outer_vlan_lsbs; 
} JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT; 

/* inner vlan */
#define JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_INNER_VLAN_LSB              0
#define JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_INNER_VLAN_NOF_BITS         SOC_SAND_PP_VID_NOF_BITS
/* pcp dei inner vlan */    
#define JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_PCP_DEI_INNER_VLAN_LSB      (JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_INNER_VLAN_LSB + JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_INNER_VLAN_NOF_BITS)
#define JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_PCP_DEI_INNER_VLAN_NOF_BITS (SOC_SAND_PP_PCP_NOF_BITS + SOC_SAND_PP_CFI_NOF_BITS)
/* outer vlan lsbs */
#define JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_OUTER_VLAN_LSBS_LSB         (JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_PCP_DEI_INNER_VLAN_LSB + JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_PCP_DEI_INNER_VLAN_NOF_BITS)
#define JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_OUTER_VLAN_LSBS_NOF_BITS    4



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



soc_error_t
  soc_jer_eg_encap_access_roo_link_layer_format_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT  *tbl_data
  );

soc_error_t
  soc_jer_eg_encap_access_roo_link_layer_format_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT  *tbl_data
  ); 

 

soc_error_t 
  soc_jer_eg_encap_access_roo_link_layer_quarter_entry_format_tbl_get(
     SOC_SAND_IN  int             unit,
     SOC_SAND_IN  uint32          outlif,
     SOC_SAND_OUT  JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT  *tbl_data
   ); 



/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __JER_PP_EG_ENCAP_ACCESS_INCLUDED__*/
#endif

