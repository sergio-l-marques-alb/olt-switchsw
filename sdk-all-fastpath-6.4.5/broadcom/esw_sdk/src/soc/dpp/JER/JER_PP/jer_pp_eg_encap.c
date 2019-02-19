
 
/* $Id: arad_pp_api_eg_encap.c,v 1.22 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>

#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 *  MACROS   *
 *************/
/* { */





#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_NOF_BITS                   12
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_MASK                       ((1 << JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_NOF_BITS) -1)

#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_OUTER_TAG_LSB        0
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_PCP_DEI_LSB          8
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_NOF_BITS                   4
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_MASK                      ((1 << JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_NOF_BITS) - 1)
/* nbr of msbs from outer vlan id for outer tag msbs field */
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_NOF_BITS             8 
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_OUTER_TAG_MSB_MASK   ((1 << JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_NOF_BITS) -1)
/* nbr of lsbs from outer vlan id for 1/4 entry */
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS             4
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_MASK                 ((1 << JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS) -1)

/* get 4 lsbs of vlan. For 1/4 eedb entry */
#define JER_PP_EG_ENCAP_OVERLAY_ARP_DATA_ENTRY_OUT_VID_LSBS(lsbs, vid) (lsbs) = (vid & JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_MASK) 

/* lsb for outer tag  */

 /* Outer-tag-MSB:
    in roo ll entry format, this field is composed of: outer-tag-PCP-DEI[11:8]; outer-tag-msbs[7:0] */
#define ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_SET(outer_tag_msb, outer_tag, pcp_dei) \
    (outer_tag_msb) = (((pcp_dei & JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_MASK)     \
                             << JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_PCP_DEI_LSB)         \
                       | ((outer_tag >> (JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS))           \
                          & JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_OUTER_TAG_MSB_MASK))

/* get outer tag msbs from field "outer tag msbs"  */
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_OUTER_TAG_GET(outer_tag_msb, outer_tag) \
   SHR_BITCOPY_RANGE(&outer_tag, JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS,                     \
                     &outer_tag_msb, JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_OUTER_TAG_LSB,    \
                     JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_NOF_BITS) 

/* get pcp dei from field "outer tag msbs" */
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_PCP_DEI_GET(outer_tag_msb, pcp_dei) \
   SHR_BITCOPY_RANGE(&pcp_dei, 0,                                                              \
                     &outer_tag_msb, JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_PCP_DEI_LSB, \
                     JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_NOF_BITS)
                     

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

/* parse eedb entry of type ROO link layer
 * format of eedb entry (88b):
 * type[87:85] (3b)  pcp-dei-profile[84:83] (2b) drop[82] (1b) roo-link-format-identifier[81] (1b) DA[80:33] (48b) SA-LSB[32:21] (12b)
 *    Ether-Type-Index[20:17] (4b) Number of tags[16:15] (2b) Remark-Profile[14:12] (3b) Outer-tag-PCP-DEI[11:8] (4b) Outer-tag-MSB[7:0] (8b)
 */
static uint32
soc_jer_pp_eg_encap_overlay_arp_data_from_roo_link_layer_entry_buffer(
   SOC_SAND_IN  int                                    unit,
   SOC_SAND_IN JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT *roo_ll_entry, 
   SOC_SAND_OUT ARAD_PP_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info 
   ) {
    uint32
        src_mac_lsbs[2]; 
    uint32 
        pcp_dei_32; 
    
    src_mac_lsbs[0] = 0; 
    src_mac_lsbs[1] = 0; 

    /* Outer-tag-MSB (in roo ll entry format, this field is composed of: outer-tag-PCP-DEI[11:8]; outer-tag-msbs[7:0] */
    /* outer tag msbs from "outer-tag msbs" field */
    JER_PP_EG_ENCAP_ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_OUTER_TAG_GET(roo_ll_entry->outer_tag_msb, overlay_arp_info->out_vid); 
    /* pcp dei from "outer tag msbs" field */
    JER_PP_EG_ENCAP_ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_PCP_DEI_GET(roo_ll_entry->outer_tag_msb, pcp_dei_32); 
    overlay_arp_info->pcp_dei = pcp_dei_32 & 0xFF; 

    /* SA lsbs */
    SHR_BITCOPY_RANGE(src_mac_lsbs, 0, 
                      &roo_ll_entry->sa_lsb, 0, 
                      JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_NOF_BITS); 
    soc_sand_pp_mac_address_long_to_struct(src_mac_lsbs, &overlay_arp_info->src_mac); 

    /* DA */
    soc_sand_pp_mac_address_long_to_struct(roo_ll_entry->dest_mac, &overlay_arp_info->dest_mac);

    /* pcp-dei-profile */
    overlay_arp_info->pcp_dei_profile = roo_ll_entry->pcp_dei_profile; 

    /* ether-type-index */
    overlay_arp_info->eth_type_index = roo_ll_entry->ether_type_index; 

    return SOC_SAND_OK;
}

/* Build ROO link layer
 * format of eedb entry (88b):
 * type[87:85] (3b)  pcp-dei-profile[84:83] (2b) drop[82] (1b) roo-link-format-identifier[81] (1b) DA[80:33] (48b) SA-LSB[32:21] (12b)
 *    Ether-Type-Index[20:17] (4b) Number of tags[16:15] (2b) Remark-Profile[14:12] (3b) Outer-tag-PCP-DEI[11:8] (4b) Outer-tag-MSB[7:0] (8b)
 * Note: type is allocated in the function that write in eedb. 
 */
static uint32
soc_jer_pp_eg_encap_overlay_arp_data_to_roo_link_layer_entry_buffer(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN ARAD_PP_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info, 
    SOC_SAND_OUT JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT *roo_ll_entry
   ) {

    uint32 src_mac[2];

    src_mac[0] =0; 
    src_mac[1] =0; 

    /* Outer-tag-MSB (in roo ll entry format, this field is composed of: outer-tag-PCP-DEI[11:8]; outer-tag-msbs[7:0] */
    ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_SET(
       roo_ll_entry->outer_tag_msb, overlay_arp_info->out_vid, overlay_arp_info->pcp_dei); 

    /* Remark-Profile */
    roo_ll_entry->remark_profile = 0; 

    /* Number of tags */
    roo_ll_entry->nof_tags = overlay_arp_info->nof_tags;  

    /* Ether-Type-Index */
    roo_ll_entry->ether_type_index = overlay_arp_info->eth_type_index; 

    /* SA-LSB */
    soc_sand_pp_mac_address_struct_to_long(&overlay_arp_info->src_mac, src_mac); 
    roo_ll_entry->sa_lsb =  (src_mac[0] & JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_MASK); 

    /* DA */
    soc_sand_pp_mac_address_struct_to_long(&overlay_arp_info->dest_mac, roo_ll_entry->dest_mac);

    /* drop: Initialized as 0. */

    /* pcp-dei-profile (2b) */
    roo_ll_entry->pcp_dei_profile = overlay_arp_info->pcp_dei_profile; 

    return SOC_SAND_OK;
}


#define JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE    (2)



uint32
soc_jer_pp_eg_encap_overlay_arp_data_entry_add(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_INOUT  ARAD_PP_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
   ) 
{   
  uint32 
      res = SOC_SAND_OK;

  JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT         tbl_data; 
  
  JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT  quarter_entry; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(ll_encap_info);

  /* overlay arp eedb entry changes between arad+ and jericho
   * 
   * in jer:  To build the arp entry, we use an eedb entry of type "linker layer" / jericho ROO link layer format:
   */

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));
  soc_sand_os_memset(&quarter_entry, 0x0, sizeof(JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT)); 

  /* 
   * 2. add additional 1/4 entry if needed
   *    2.1 allocate entry ???
   *    2.2 build the entry
   *    2.3 add it in HW
   * 3. build ROO link layer
   *    3.1 build the entry
   *    3.2 add it in HW
   */

  /* 2. additional 1/4 entry if needed (EVE required) */
  if (ll_encap_info->nof_tags != 0) {
      /* 2.1 allocate entry */
      
      /* 2.2 build the entry */
      quarter_entry.roo_inner_vlan = ll_encap_info->inner_vid; 
      quarter_entry.pcp_dei_inner_vlan = ll_encap_info->inner_pcp_dei; 
      JER_PP_EG_ENCAP_OVERLAY_ARP_DATA_ENTRY_OUT_VID_LSBS(quarter_entry.roo_outer_vlan_lsbs, ll_encap_info->out_vid); 
      /* 2.3 add it in HW */
      
  }

  /* 3. build ROO link layer */
  /*    3.1 build the entry */
  res = soc_jer_pp_eg_encap_overlay_arp_data_to_roo_link_layer_entry_buffer(
     unit, ll_encap_info, &tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

   /*    3.2 add it in HW */
  res = soc_jer_eg_encap_access_roo_link_layer_format_tbl_set(
     unit, overlay_ll_eep_ndx, &tbl_data); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_overlay_arp_data_entry_add_unsafe()",0,0);

}



soc_error_t
soc_jer_eg_encap_roo_ll_entry_get(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  uint32                      eep_ndx, 
     SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_info
   ) {

    uint32
       res = SOC_SAND_OK; 

    JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT tbl_data; 

    JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT  quarter_entry; 



    ARAD_PP_EG_ENCAP_EEP_TYPE                  
        eep_type_ndx = SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL;
    ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
        cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    SOC_SAND_CHECK_NULL_INPUT(encap_info);

    soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));
    soc_sand_os_memset(&quarter_entry, 0x0, sizeof(JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT)); 

    SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_info);

    /* 1. parse ROO link layer
     *    1.1 get the eedb entry of type roo link layer
     *    1.2 parse eedb entry to overlay arp data info
     * 2. parse additional 1/4 entry if needed
     *    2.1 check if entry exist
     *    2.2 get the 1/4 entry
     *    2.3 parse the entry to overlay arp data info
     */

    res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(
       unit, eep_ndx, &cur_eep_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ARAD_PP_EG_ENCAP_VERIFY_EEP_TYPE_COMPATIBLE_TO_ACCESS_TYPE(
       eep_ndx,eep_type_ndx, cur_eep_type);

    /* 1. parse ROO link layer*/

    /*    1.1 get the eedb entry of type roo link layer */
    res = soc_jer_eg_encap_access_roo_link_layer_format_tbl_get(
       unit, eep_ndx, &tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /*    1.2 parse eedb entry to overlay arp data info */
    encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP; 
    res = soc_jer_pp_eg_encap_overlay_arp_data_from_roo_link_layer_entry_buffer(
       unit, &tbl_data, &(encap_info->entry_val.overlay_arp_encap_info)); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


    /* 2. parse additional 1/4 entry if needed */

    
    
    /*   2.3 parse 1/4 entry to overlay arp data info */
    encap_info->entry_val.overlay_arp_encap_info.inner_vid = 
        quarter_entry.pcp_dei_inner_vlan; 
    encap_info->entry_val.overlay_arp_encap_info.inner_pcp_dei = 
        quarter_entry.pcp_dei_inner_vlan; 
    SHR_BITCOPY_RANGE(&encap_info->entry_val.overlay_arp_encap_info.out_vid, 0, 
                      &quarter_entry.roo_outer_vlan_lsbs, 0, 
                      JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS); 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_eg_encap_roo_ll_entry_get()", 0, 0); 
}






/* Build cfg ether type index table entry. 
 * format of ether type index tabe entry:
 * ether type[47:32] tpid0[31:16] tpid1[15:0]
 * eth_type_index_entry
   */
uint32
soc_jer_pp_eg_encap_eth_type_index_to_cfg_ether_type_index_tbl_entry(
   SOC_SAND_IN  int                                     unit,
   SOC_SAND_IN  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry, 
   SOC_SAND_OUT uint32                                  *cfg_ether_type_index_tbl_data
   ) {

  uint32 tmp; 
  uint32 fld_offset; 

  uint32 res = SOC_SAND_OK;

  fld_offset = 0; 

  /* tpid 1 (inner tpid) */
  tmp = eth_type_index_entry->tpid_1;   
  SHR_BITCOPY_RANGE(
     cfg_ether_type_index_tbl_data, fld_offset, &tmp, 0, SOC_SAND_PP_TPID_NOF_BITS);

  fld_offset += SOC_SAND_PP_TPID_NOF_BITS; 

  /* tpid 0 (outer tpid) */
  tmp =  eth_type_index_entry->tpid_0;
  SHR_BITCOPY_RANGE(
     cfg_ether_type_index_tbl_data, fld_offset, &tmp, 0, SOC_SAND_PP_TPID_NOF_BITS); 

  fld_offset += SOC_SAND_PP_TPID_NOF_BITS; 

  /* ether_type */
  tmp = eth_type_index_entry->ether_type; 
  SHR_BITCOPY_RANGE(
     cfg_ether_type_index_tbl_data, fld_offset, &tmp, 0, SOC_SAND_PP_ETHER_TYPE_NOF_BITS);

  return res; 
}


/* Parse cfg ether type index table entry. 
 * format of ether type index tabe entry:
   ether type[47:32] tpid0[31:16] tpid1[15:0] */
uint32
soc_jer_pp_eg_encap_eth_type_index_from_cfg_ether_type_index_tbl_entry(
   SOC_SAND_IN  int                                     unit,
   SOC_SAND_OUT  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry, 
   SOC_SAND_IN uint32                                  *cfg_ether_type_index_tbl_entry
   ) {
  uint32 tmp; 
  uint32 fld_offset; 
  uint32 res = SOC_SAND_OK;


  fld_offset = 0; 

  /* tpid 0 (outer tpid) */
  SHR_BITCOPY_RANGE(
     &tmp, 0, cfg_ether_type_index_tbl_entry, fld_offset, SOC_SAND_PP_TPID_NOF_BITS); 
  eth_type_index_entry->tpid_1 = (uint16) (tmp & 0xFFFF); 

  fld_offset += SOC_SAND_PP_TPID_NOF_BITS; 

  /* tpid 1 (inner tpid) */
  SHR_BITCOPY_RANGE(
     &tmp, 0, cfg_ether_type_index_tbl_entry, fld_offset, SOC_SAND_PP_TPID_NOF_BITS); 
  eth_type_index_entry->tpid_0 = (uint16) (tmp & 0xFFFF); 

  fld_offset += SOC_SAND_PP_TPID_NOF_BITS; 

  /* ether_type */
  SHR_BITCOPY_RANGE(
     &tmp, 0, cfg_ether_type_index_tbl_entry, fld_offset, SOC_SAND_PP_ETHER_TYPE_NOF_BITS); 
  eth_type_index_entry->ether_type = (uint16) (tmp & 0xFFFF); 

  return res; 
}




soc_error_t
soc_jer_eg_encap_ether_type_index_set(
   SOC_SAND_IN  int                                        unit, 
   SOC_SAND_IN  int                                        eth_type_index,
   SOC_SAND_IN  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry
   ) {

    uint32
       res = SOC_SAND_OK; 

    uint32 cfg_ether_type_index_tbl_data[JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    soc_sand_os_memset(&cfg_ether_type_index_tbl_data, 0x0, sizeof(uint32)*JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE);

  /*  build the entry   */
  res = soc_jer_pp_eg_encap_eth_type_index_to_cfg_ether_type_index_tbl_entry(
     unit, eth_type_index_entry, cfg_ether_type_index_tbl_data); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  /* add it in HW */
  res = WRITE_EPNI_CFG_ETHER_TYPE_INDEXm(unit, MEM_BLOCK_ANY, eth_type_index, cfg_ether_type_index_tbl_data); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_eg_encap_ether_type_index_set()", 0, 0); 
}
soc_error_t
soc_jer_eg_encap_ether_type_index_get(
   SOC_SAND_IN  int                                        unit, 
   SOC_SAND_IN  int                                        eth_type_index,
   SOC_SAND_OUT  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry
   ) {

    uint32
       res = SOC_SAND_OK; 

    uint32 cfg_ether_type_index_tbl_data[JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    soc_sand_os_memset(&cfg_ether_type_index_tbl_data, 0x0, sizeof(uint32)*JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE);

    /* get the eth type index entry */
    res = READ_EPNI_CFG_ETHER_TYPE_INDEXm(
       unit, MEM_BLOCK_ANY, 
       eth_type_index, 
       cfg_ether_type_index_tbl_data); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    /* parse eth type index table entry to ether type index info */ 
    res = soc_jer_pp_eg_encap_eth_type_index_from_cfg_ether_type_index_tbl_entry(
       unit, eth_type_index_entry, cfg_ether_type_index_tbl_data); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_eg_encap_ether_type_index_set()", 0, 0); 
}

soc_error_t
soc_jer_eg_encap_ether_type_index_clear(
   SOC_SAND_IN  int unit, 
   SOC_SAND_IN  int eth_type_index) {

    uint32
       res = SOC_SAND_OK; 

     uint64 reg_64_val; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

     COMPILER_64_ZERO(reg_64_val); 
  
    res = WRITE_EPNI_CFG_ETHER_TYPE_INDEXm(
      unit, MEM_BLOCK_ANY, eth_type_index, &reg_64_val); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_pp_eg_encap_ether_type_index_clear()", 0, 0); 
}

soc_error_t
soc_jer_eg_encap_direct_bank_set(int unit, int bank, uint8 is_mapped){
    int rv;
    uint64 buffer;
    SOCDNX_INIT_FUNC_DEFS;


    /* There are two mapping_is_required registers, one for outlif and one for eei. They should be the same, so get the first one
       and write the result to both. */

    rv = soc_reg_above_64_field64_read(unit, EPNI_CFG_OUTLIF_MAPPING_IS_REQUIREDr, REG_PORT_ANY, 0, CFG_OUTLIF_MAPPING_IS_REQUIREDf, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    if (is_mapped) {
        COMPILER_64_BITSET(buffer, bank);
    } else {
        COMPILER_64_BITCLR(buffer, bank);
    }

    rv = soc_reg_above_64_field64_modify(unit, EPNI_CFG_OUTLIF_MAPPING_IS_REQUIREDr, REG_PORT_ANY, 0, CFG_OUTLIF_MAPPING_IS_REQUIREDf, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_reg_above_64_field64_modify(unit, EPNI_CFG_EEI_MAPPING_IS_REQUIREDr, REG_PORT_ANY, 0, CFG_EEI_MAPPING_IS_REQUIREDf, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_eg_encap_extension_mapping_set(int unit, int bank, uint32 is_extended, uint32 extesnion_bank){

    uint32 tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    /*get current configuration*/
    SOCDNX_IF_ERR_EXIT(READ_EDB_EEDB_MAP_TO_PROTECTION_PTRm(unit, MEM_BLOCK_ANY, bank, &tbl_data));

       /* set enable bit*/
      soc_mem_field_set(unit, EDB_EEDB_MAP_TO_PROTECTION_PTRm , &tbl_data, PROTECTION_PTR_ENABLEf, &is_extended);

      /* set protection pointer (extension to bank id)*/
      soc_mem_field_set(unit, EDB_EEDB_MAP_TO_PROTECTION_PTRm , &tbl_data, PROTECTION_PTR_TABLE_ADDR_MSBf, &extesnion_bank);

    /*set configuration*/
    SOCDNX_IF_ERR_EXIT(WRITE_EDB_EEDB_MAP_TO_PROTECTION_PTRm(unit, MEM_BLOCK_ANY, bank, &tbl_data));

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_eg_encap_extension_mapping_get(int unit, int bank, uint32 *is_extended, uint32 *extesnion_bank){
   
    uint32 tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    /*get current configuration*/
    SOCDNX_IF_ERR_EXIT(READ_EDB_EEDB_MAP_TO_PROTECTION_PTRm(unit, MEM_BLOCK_ANY, bank, &tbl_data));

      /* get enable bit*/
      soc_mem_field_get(unit, EDB_EEDB_MAP_TO_PROTECTION_PTRm, &tbl_data, PROTECTION_PTR_ENABLEf, is_extended);

      /* get protection pointer (extension to bank id)*/
      soc_mem_field_get(unit, EDB_EEDB_MAP_TO_PROTECTION_PTRm, &tbl_data, PROTECTION_PTR_TABLE_ADDR_MSBf, extesnion_bank);


exit:
  SOCDNX_FUNC_RETURN;
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
