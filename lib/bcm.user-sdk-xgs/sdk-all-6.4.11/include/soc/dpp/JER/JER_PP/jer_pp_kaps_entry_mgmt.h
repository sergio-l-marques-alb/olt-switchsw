/* $Id: jer_pp_kaps_entry_mgmt.h, hagayco Exp $
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

#ifndef __JER_PP_KAPS_ENTRY_MGMT_INCLUDED__
/* { */
#define __JER_PP_KAPS_ENTRY_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/key.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/kbp_legacy.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/kbp_portable.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define JER_KAPS_AD_BUFFER_NOF_BYTES    4
#define JER_KAPS_TBL_PREFIX_NOF_BITS    2
#define JER_KAPS_DMA_BUFFER_NOF_BYTES    8

/* } */

/*************
 * ENUMS     *
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
    


/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
uint32 jer_pp_kaps_entry_add_hw(SOC_SAND_IN  int                          unit,
                                SOC_SAND_IN  uint32                       table_id,
                                SOC_SAND_IN  uint32                       prefix_len,
                                SOC_SAND_OUT  uint8                      *data,
                                SOC_SAND_OUT  uint8                      *assoData,
                                SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success);

uint32 jer_pp_kaps_entry_remove_hw(SOC_SAND_IN  int                          unit,
                                   SOC_SAND_IN  uint32                       table_id,
                                   SOC_SAND_IN  uint32                       prefix_len,
                                   SOC_SAND_OUT uint8                       *data);

uint32 jer_pp_kaps_entry_get_hw(SOC_SAND_IN  int          unit,
                                SOC_SAND_IN  uint8        table_id,
                                SOC_SAND_IN  uint32       prefix_len,
                                SOC_SAND_OUT uint8       *data,
                                SOC_SAND_OUT uint8       *assoData,
                                SOC_SAND_OUT uint8       *found);

uint32 jer_pp_kaps_entry_get_block_hw(SOC_SAND_IN    int                             unit,
                                      SOC_SAND_IN    SOC_DPP_DBAL_SW_TABLE_IDS       dbal_table_id,
                                      SOC_SAND_IN    uint32                          vrf_ndx,
                                      SOC_SAND_IN    SOC_DPP_DBAL_TABLE_INFO        *dbal_table,
                                      SOC_SAND_INOUT ARAD_PP_IP_ROUTING_TABLE_RANGE *block_range_key,
                                      SOC_SAND_OUT   ARAD_PP_FP_QUAL_VAL            *qual_vals_array,
                                      SOC_SAND_OUT   void                           *payload,
                                      SOC_SAND_OUT   uint32                         *nof_entries);

uint32 jer_pp_kaps_route_to_kaps_payload_buffer_encode(SOC_SAND_IN  int                         unit,
                                                       SOC_SAND_IN  uint32                     *asso_data_buffer,
                                                       SOC_SAND_OUT uint8                      *asso_data);

uint32 jer_pp_kaps_dma_byffer_to_kaps_payload_buffer_encode(
                                                        SOC_SAND_IN  int                        unit,
                                                        SOC_SAND_IN  uint32                     *asso_data_buffer,
                                                        SOC_SAND_OUT uint8                      *asso_data);

uint32 jer_pp_kaps_route_to_kaps_payload_buffer_decode(SOC_SAND_IN  int                         unit,
                                                       SOC_SAND_IN  uint8                      *asso_data,
                                                       SOC_SAND_OUT uint32                     *asso_data_buffer);

uint32 jer_pp_kaps_payload_to_mc_dest_id(SOC_SAND_IN  int                                     unit,
                                         SOC_SAND_IN  uint32                                  payload,
                                         SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID              *dest_id);

uint32 jer_pp_kaps_dbal_table_id_translate(SOC_SAND_IN  int                       unit,
                                           SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS table_id,
                                           SOC_SAND_IN  ARAD_PP_FP_QUAL_VAL       qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                           SOC_SAND_IN  uint8                     private_table,
                                           SOC_SAND_OUT JER_KAPS_IP_TBL_ID       *kaps_table_id);

uint32 jer_pp_kaps_table_id_to_dbal_translate(SOC_SAND_IN  int                        unit,
                                              SOC_SAND_IN  JER_KAPS_IP_TBL_ID         kaps_table_id,
                                              SOC_SAND_OUT SOC_DPP_DBAL_SW_TABLE_IDS *table_id);

uint32 jer_pp_kaps_entry_table_clear(SOC_SAND_IN    int unit, 
                                     SOC_SAND_IN    uint8 tbl_id);

void jer_pp_kaps_key_encode(SOC_SAND_IN    uint8 tbl_prefix,
                           SOC_SAND_IN    uint8 nof_quals,
                           SOC_SAND_IN    uint64 *qual_vals,
                           SOC_SAND_IN    uint8 *qual_nof_bits,
                           SOC_SAND_OUT   uint8 *data_bytes);

uint32 jer_pp_kaps_db_enabled(SOC_SAND_IN    int              unit,
                              SOC_SAND_IN    SOC_PPC_VRF_ID   vrf_ndx);

uint32 jer_pp_kaps_dma_entry_add(SOC_SAND_IN  int                         unit,
                                 SOC_SAND_IN  uint32                      key,
                                 SOC_SAND_IN  uint8                      *data,
                                 SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success);

uint32 jer_pp_kaps_dma_entry_remove( SOC_SAND_IN  int                         unit,
                                     SOC_SAND_IN  uint32                      key,
                                     SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success);

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __JER_PP_KAPS_ENTRY_MGMT_INCLUDED__ */

#endif


