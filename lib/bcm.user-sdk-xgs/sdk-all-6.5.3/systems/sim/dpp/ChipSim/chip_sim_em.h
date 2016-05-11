/* $Id: chip_sim_em.h,v 1.6 Broadcom SDK $
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


#ifndef __CHIP_SIM_EM_H__
/* { */
#define __CHIP_SIM_EM_H__

#include "chip_sim.h"
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/dpp_config_defs.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_esem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>

/*
 * INCLUDE FILES:
 * {
 */

typedef enum
{
  SOC_PB_EM_TABLE_ISEM=0,
  SOC_PB_EM_TABLE_ESEM=1,
  SOC_PB_EM_TABLE_SVEM=2,
  SOC_PB_EM_TABLE_LEM=3,
  SOC_PB_EM_TABLE_ID_LAST = 4,
  ARAD_EM_TABLE_LEM=0,
  ARAD_EM_TABLE_ISEM_A=1,
  ARAD_EM_TABLE_ISEM_B=2,
  ARAD_EM_TABLE_ESEM=3,
  ARAD_EM_TABLE_RMAPEM=4,
  ARAD_EM_TABLE_OAMEM_A=5,
  ARAD_EM_TABLE_OAMEM_B=6,
  ARAD_EM_TABLE_GLEM=7,
  ARAD_EM_TABLE_ID_LAST = 8
}CHIP_SIM_EM_TABLE_TYPE;

/* from soc_pb_pp_chip_tbls.c */
#define CHIP_SIM_ISEM_ADDR_BASE 0x00900000

#define CHIP_SIM_ISEM_KEY_SIZE  ((46 - 13 + 1) + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE
#define CHIP_SIM_ISEM_DATA_SIZE ((62 - 47 + 1) + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE
#define CHIP_SIM_ISEM_TABLE_SIZE (16 * 1024)


#define CHIP_SIM_ESEM_ADDR_BASE SOC_PB_PP_ESEM_ADDR
#define CHIP_SIM_ESEM_KEY_SIZE  ((31 - 11 + 1) + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE
#define CHIP_SIM_ESEM_DATA_SIZE ((46 - 32 + 1) + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE
#define CHIP_SIM_ESEM_TABLE_SIZE (16 * 1024)  

#define CHIP_SIM_SVEM_ADDR_BASE SOC_PB_PP_SVEM_ADDR
#define CHIP_SIM_SVEM_KEY_SIZE  ((26 - 11 + 1) + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE
#define CHIP_SIM_SVEM_DATA_SIZE ((38 - 27 + 1) + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE
#define CHIP_SIM_SVEM_TABLE_SIZE (16 * 1024)  

#define CHIP_SIM_LEM_ADDR_BASE    0x11111111 /* Dummy */
#define CHIP_SIM_LEM_KEY_SIZE(unit) (SOC_DPP_DEFS_GET(unit, lem_width) + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE
#define CHIP_SIM_LEM_KEY_SIZE_MAX   (SOC_DPP_DEFS_MAX(LEM_WIDTH) + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE
#define CHIP_SIM_LEM_DATA_SIZE(unit) ((ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_BITS + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE)
#define CHIP_SIM_LEM_DATA_SIZE_MAX  (ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_BITS + SOC_SAND_NOF_BITS_IN_BYTE - 1)/SOC_SAND_NOF_BITS_IN_BYTE
#define CHIP_SIM_LEM_TABLE_SIZE   (SOC_PB_PP_FRWRD_MACT_TABLE_NOF_ENTRIES)  

#define SOC_PB_CHIP_SIM_TABLE_KEY_MAX_SIZE       SOC_SAND_MAX(SOC_SAND_MAX(CHIP_SIM_SVEM_KEY_SIZE, CHIP_SIM_LEM_KEY_SIZE_MAX), SOC_SAND_MAX(CHIP_SIM_ISEM_KEY_SIZE, CHIP_SIM_ESEM_KEY_SIZE))
#define SOC_PB_CHIP_SIM_TABLE_DATA_MAX_SIZE      SOC_SAND_MAX(SOC_SAND_MAX(CHIP_SIM_SVEM_DATA_SIZE, CHIP_SIM_LEM_DATA_SIZE_MAX), SOC_SAND_MAX(CHIP_SIM_ISEM_DATA_SIZE, CHIP_SIM_ESEM_DATA_SIZE))


/* arad */
#define ARAD_CHIP_SIM_LEM_KEY  10
#define ARAD_CHIP_SIM_LEM_PAYLOAD  6
#define ARAD_CHIP_SIM_LEM_BASE  0x1111111  /* dummy */
#define ARAD_CHIP_SIM_LEM_TABLE_SIZE  262144
#define ARAD_CHIP_SIM_ISEM_A_KEY  6
#define ARAD_CHIP_SIM_ISEM_A_PAYLOAD  2
#define ARAD_CHIP_SIM_ISEM_A_BASE  0x7A0000
#define ARAD_CHIP_SIM_ISEM_A_TABLE_SIZE  32768
#define ARAD_CHIP_SIM_ISEM_B_KEY  6
#define ARAD_CHIP_SIM_ISEM_B_PAYLOAD  2
#define ARAD_CHIP_SIM_ISEM_B_BASE  0xA10000
#define ARAD_CHIP_SIM_ISEM_B_TABLE_SIZE  32768
#define ARAD_CHIP_SIM_ESEM_KEY  ARAD_PP_ESEM_KEY_SIZE
#define ARAD_CHIP_SIM_ESEM_PAYLOAD  ARAD_PP_ESEM_ENTRY_SIZE
#define ARAD_CHIP_SIM_ESEM_BASE  0x520000
#define ARAD_CHIP_SIM_ESEM_TABLE_SIZE  16384
#define ARAD_CHIP_SIM_RMAPEM_KEY  1
#define ARAD_CHIP_SIM_RMAPEM_PAYLOAD  1
#define ARAD_CHIP_SIM_RMAPEM_BASE  0x100000
#define ARAD_CHIP_SIM_RMAPEM_TABLE_SIZE  16384
#define ARAD_CHIP_SIM_OEMA_KEY  4
#define ARAD_CHIP_SIM_OEMA_PAYLOAD  5
#define ARAD_CHIP_SIM_OEMA_BASE  0xEA0000
#define ARAD_CHIP_SIM_OEMA_TABLE_SIZE  16384

#define ARAD_CHIP_SIM_OEMB_KEY  4
#define ARAD_CHIP_SIM_OEMB_PAYLOAD  4
#define ARAD_CHIP_SIM_OEMB_BASE  0x14A0000
#define ARAD_CHIP_SIM_OEMB_TABLE_SIZE  16384

#define ARAD_CHIP_SIM_GLEM_KEY  sizeof(int)
#define ARAD_CHIP_SIM_GLEM_PAYLOAD  sizeof(int)
#define ARAD_CHIP_SIM_GLEM_BASE  0x1800000
#define ARAD_CHIP_SIM_GLEM_TABLE_SIZE  (96 * 1024)


#define ARAD_CHIP_SIM_TABLE_KEY_MAX_SIZE       (ARAD_CHIP_SIM_LEM_KEY)
#define ARAD_CHIP_SIM_TABLE_DATA_MAX_SIZE      (ARAD_CHIP_SIM_LEM_PAYLOAD)

#define CHIP_SIM_TABLE_KEY_MAX_SIZE       SOC_SAND_MAX(ARAD_CHIP_SIM_TABLE_KEY_MAX_SIZE,SOC_PB_CHIP_SIM_TABLE_DATA_MAX_SIZE)
#define CHIP_SIM_TABLE_DATA_MAX_SIZE      SOC_SAND_MAX(ARAD_CHIP_SIM_TABLE_DATA_MAX_SIZE,SOC_PB_CHIP_SIM_TABLE_DATA_MAX_SIZE)

#define CHIPSIM_MAX_TRAVERSE_SIZE   100

uint32 
chip_sim_em_offset_to_table_id (
               SOC_SAND_IN   uint32     offset,
               SOC_SAND_OUT  uint32     *id
                );
uint32
chip_sim_exact_match_entry_add_unsafe(
              SOC_SAND_IN  int         unit,
              SOC_SAND_IN  uint32      offset, /* ISEM,LEM,ESEM,SVEM */
              SOC_SAND_IN  uint32      *key_ptr,
              SOC_SAND_IN  uint32     key_size,
              SOC_SAND_IN  uint32      *data_ptr,
              SOC_SAND_IN  uint32     data_size,
              SOC_SAND_OUT uint8     *success
              );
uint32
chip_sim_exact_match_entry_get_unsafe(
              SOC_SAND_IN  int         unit,
              SOC_SAND_IN  uint32      offset,
              SOC_SAND_IN  uint32      *key_ptr,
              SOC_SAND_IN  uint32     key_size,
              SOC_SAND_OUT  uint32     *data_ptr,
              SOC_SAND_IN  uint32     data_size,
              SOC_SAND_OUT  uint8    *found
              );
uint32
soc_sand_exact_match_entry_get_by_index_unsafe(
              SOC_SAND_IN  int         unit,
              SOC_SAND_IN  uint32      tbl_offset,
              SOC_SAND_IN  uint32      entry_offset,
              SOC_SAND_OUT  uint32      *key_ptr,
              SOC_SAND_IN  uint32     key_size,
              SOC_SAND_OUT  uint32     *data_ptr,
              SOC_SAND_IN  uint32     data_size,
              SOC_SAND_OUT  uint8    *found
              );
uint32
chip_sim_exact_match_entry_remove_unsafe(
              SOC_SAND_IN  int         unit,
              SOC_SAND_IN  uint32      offset,
              SOC_SAND_IN  uint32      *key_ptr,
              SOC_SAND_IN  uint32     key_size
              );

void chip_sim_em_free(void);

uint32 chip_sim_em_init(SOC_SAND_DEVICE_TYPE device_type);

uint32
chip_sim_em_get_block(
   SOC_SAND_IN       int         unit,
   SOC_SAND_IN       uint32      tbl_offset,
   SOC_SAND_IN       uint32     key_size,
   SOC_SAND_IN       uint32     data_size,
   SOC_SAND_IN       void*      *filter_data,
   SOC_SAND_OUT      uint32     *key_array,
   SOC_SAND_OUT      uint32     *data_array,
   SOC_SAND_OUT      uint32     *cnt,
   SOC_SAND_INOUT    SOC_SAND_TABLE_BLOCK_RANGE  *block_range);

uint32
  chip_sim_em_match_rule_mac(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_key, 
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *match);


/* } __CHIP_SIM_EM_H__*/
#endif
