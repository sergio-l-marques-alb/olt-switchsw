
/* $Id: jer_pp_eg_encap.h,v 1.20 Broadcom SDK $
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

#ifndef __JER_PP_EG_ENCAP_INCLUDED__
/* { */
#define __JER_PP_EG_ENCAP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define JER_PP_EG_ENCAP_PROTECTION_PATH_BIT_NUM                 (14)

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

/*********************************************************************
* NAME:
 *   soc_jer_pp_eg_encap_overlay_arp_data_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *    overlay arp eedb entry changes between arad+ and jericho 
 *    Build data info to get overlay arp encap info.
 *    Build eedb entry of type "linker layer" / jericho ROO link layer format:
 *          eedb entry format:
 *            type (3b)  pcp-dei-profile (2b) drop (1b) roo-link-format-identifier (1b) DA (48b)  SA-LSB (12b)
 *            Ether-Type-Index (4b) Number of tags (2b) Remark-Profile (3b) Outer-tag-PCP-DEI (4b) Outer-tag-MSB (8b)
 *    Ether-Type-Index point to table: CfgEtherTypeIndex
 *          Format of CfgEtherTypeIndex entry:
 *          Ethernet-type(16b)  tpid_0(16b) tpid_1(16b)
 *    Additional 1/4 entry for EVE.
 *          1/4 EEDB entry format: LSB's for ROO-Outer_VLAN(16b) PCP-DEI-Inner_Vlan (4b) ROO-Inner-VLAN(16b) 
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  ll_eep_ndx,
 *      eedb entry index
 *   SOC_SAND_OUT ARAD_PP_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO ll_encap_info -
 *      To include egress encapsulation entries.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
soc_jer_pp_eg_encap_overlay_arp_data_entry_add(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 ll_eep_ndx, 
     SOC_SAND_INOUT  ARAD_PP_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
   ); 

/*********************************************************************
* NAME:
 *   soc_jer_eg_encap_roo_ll_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *    overlay arp eedb entry changes between arad+ and jericho
 *    Parse eedb entry to get overlay encap info.
 *    Parse eedb entry of type "linker layer" / jericho ROO link layer format:
 *          eedb entry format:
 *            type (3b)  pcp-dei-profile (2b) drop (1b) roo-link-format-identifier (1b) DA (48b)  SA-LSB (12b)
 *            Ether-Type-Index (4b) Number of tags (2b) Remark-Profile (3b) Outer-tag-PCP-DEI (4b) Outer-tag-MSB (8b)
 *    Ether-Type-Index point to table: CfgEtherTypeIndex
 *          Format of CfgEtherTypeIndex entry:
 *          Ethernet-type(16b)  tpid_0(16b) tpid_1(16b)
 *    Additional 1/4 entry for EVE.
 *          1/4 EEDB entry format: LSB's for ROO-Outer_VLAN(16b) PCP-DEI-Inner_Vlan (4b) ROO-Inner-VLAN(16b) 
 *
 * Note: Parse only linker layer eedb entry.
 *       CfgEtherTypeIndex parsing is not done here (see soc_jer_eg_encap_ether_type_index_get)
 * INPUT:
 *   SOC_SAND_IN  int                         unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_EG_ENCAP_ENTRY_INFO ll_encap_info -
 *      To include egress encapsulation entries.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/

soc_error_t
soc_jer_eg_encap_roo_ll_entry_get(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  uint32                      eep_ndx, 
     SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_info
   ); 






/*********************************************************************
* NAME:
 *   soc_jer_eg_encap_ether_type_index_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *    add entry in ether type index table.
      Build entry.
      Add it in HW.
      Ether type index table is an additional table for jericho roo link layer. 
 * INPUT:
 *   SOC_SAND_IN  int                         unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  int                         eth_type_index -
 *      Index for the ether_type_index table
 *   SOC_SAND_IN  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry -
 *      eth type index entry.      
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t
soc_jer_eg_encap_ether_type_index_set(
   SOC_SAND_IN  int                                        unit, 
   SOC_SAND_IN  int                                        eth_type_index,
   SOC_SAND_IN  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry
   ); 



/*********************************************************************
* NAME:
 *   soc_jer_eg_encap_ether_type_index_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *    get in ether type index table entry.
 *     Ether type index table is an additional table for jericho roo link layer.
 * 
 * INPUT:
 *   SOC_SAND_IN  int                         unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  int                         eth_type_index -
 *      Index for the ether_type_index table
 *   SOC_SAND_OUT  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry -
 *      eth type index entry.      
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t
soc_jer_eg_encap_ether_type_index_get(
   SOC_SAND_IN  int                                        unit, 
   SOC_SAND_IN  int                                        eth_type_index,
   SOC_SAND_OUT  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry
   ); 

/*********************************************************************
* NAME:
 *   soc_jer_eg_encap_ether_type_index_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Clear ether type index entry 
 * INPUT:
 *   SOC_SAND_IN  int                         unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  int                         eth_type_index -
 *      Index for the ether_type_index table 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t
soc_jer_eg_encap_ether_type_index_clear(
   SOC_SAND_IN  int unit, 
   SOC_SAND_IN  int eth_type_index); 


/*********************************************************************
* NAME:
 *   soc_jer_eg_encap_direct_bank_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Given an EEDB bank id, the bank is set to either mapped lif mode, or direct lif mode.
 * INPUT:
 *      unit        - (IN) Identifier of the device to access.
 *      bank        - (IN) Identifier of the bank to be set.
 *      is_mapped   - (IN)  TRUE: Set the bank to be mapped bank.
                           FALSE: Set the bank to be direct bank.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t
soc_jer_eg_encap_direct_bank_set(int unit, int bank, uint8 is_mapped);



/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __JER_PP_EG_ENCAP_INCLUDED__*/
#endif

