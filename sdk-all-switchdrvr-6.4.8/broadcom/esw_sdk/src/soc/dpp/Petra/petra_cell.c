/* $Id: petra_cell.c,v 1.12.12.1 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/bsl.h>

#include <soc/mem.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>

#include <soc/dpp/Petra/petra_cell.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/Petra/petra_api_packet.h>
#include <soc/dpp/Petra/petra_api_cell.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME SOC_DBG_FABRIC

#define SOC_PETRA_CELL_PATH_LINK_FIP_SWITCH_POSITION      (0)
#define SOC_PETRA_CELL_PATH_LINK_FE1_SWITCH_POSITION      (1)
#define SOC_PETRA_CELL_PATH_LINK_FE2_SWITCH_POSITION      (2)
#define SOC_PETRA_CELL_PATH_LINK_FE3_SWITCH_POSITION      (3)
#define SOC_PETRA_CELL_MAX_NOF_TRIES_WAITING_FOR_ACK      (1000)
#define SOC_PETRA_CELL_ADDRESS_POSITION_0                 (0)
#define SOC_PETRA_CELL_ADDRESS_POSITION_1                 (1)
#define SOC_PETRA_CELL_ADDRESS_POSITION_2                 (2)
#define SOC_PETRA_CELL_ADDRESS_POSITION_3                 (3)
#define SOC_PETRA_CELL_ADDRESS_POSITION_4                 (4)
#define SOC_PETRA_CELL_WRITE_POSITION_0                   (0)
#define SOC_PETRA_CELL_WRITE_POSITION_1                   (1)
#define SOC_PETRA_CELL_WRITE_POSITION_2                   (2)
#define SOC_PETRA_CELL_WRITE_POSITION_4                   (4)
#define SOC_PETRA_FE600_RTP_INDIRECT_RW_ADDR              (0x0441 * 4)
#define SOC_PETRA_FE600_RTP_INDIRECT_WRITE_DATA0          (0x0420 * 4)
#define SOC_PETRA_FE600_RTP_INDIRECT_WRITE_DATA1          (0x0421 * 4)
#define SOC_PETRA_FE600_RTP_INDIRECT_WRITE_DATA2          (0x0422 * 4)
#define SOC_PETRA_FE600_RTP_INDIRECT_RW_TRIGGER           (0x0440 * 4)
#define SOC_PETRA_FE600_RTP_INDIRECT_READ_DATA0           (0x0430 * 4)
#define SOC_PETRA_FE600_RTP_INDIRECT_READ_DATA1           (0x0431 * 4)
#define SOC_PETRA_FE600_RTP_INDIRECT_READ_DATA2           (0x0432 * 4)
#define SOC_PETRA_CELL_NOF_CELL_IDENTS                    (0x1ff)

#define SOC_PETRA_MC_ID_NDX_MAX                          (16*1024-1)
#define SOC_PETRA_CELL_MC_TBL_WRITE_ACK_READ             TRUE
#define SOC_PETRA_CELL_MC_TBL_WRITE_ACK_WAIT_IN_MS       (0)
#define SOC_PETRA_CELL_MC_TBL_MAX_NOF_TRIES_FOR_ACK      SOC_PETRA_CELL_MAX_NOF_TRIES_WAITING_FOR_ACK
#define SOC_PETRA_CELL_DATA0_REG1                      3
#define SOC_PETRA_CELL_DATA0_REG2                      4
#define SOC_PETRA_CELL_DATA1_REG1                      5
#define SOC_PETRA_CELL_DATA1_REG2                      6
#define SOC_PETRA_CELL_DATA2_REG1                      6
#define SOC_PETRA_CELL_DATA2_REG2                      7
#define SOC_PETRA_CELL_DATA0_REG1_MSB                  (13)
#define SOC_PETRA_CELL_DATA0_REG1_LSB                  (0)
#define SOC_PETRA_CELL_DATA0_REG2_MSB                  (31)
#define SOC_PETRA_CELL_DATA0_REG2_LSB                  (14)
#define SOC_PETRA_CELL_DATA0_REG2_LENGTH               (SOC_PETRA_CELL_DATA0_REG2_MSB - SOC_PETRA_CELL_DATA0_REG2_LSB + 1)
#define SOC_PETRA_CELL_DATA1_REG1_MSB                  (28)
#define SOC_PETRA_CELL_DATA1_REG1_LSB                  (0)
#define SOC_PETRA_CELL_DATA1_REG2_MSB                  (31)
#define SOC_PETRA_CELL_DATA1_REG2_LSB                  (29)
#define SOC_PETRA_CELL_DATA1_REG2_LENGTH               (SOC_PETRA_CELL_DATA1_REG2_MSB - SOC_PETRA_CELL_DATA1_REG2_LSB + 1)
#define SOC_PETRA_CELL_DATA2_REG1_MSB                  (11)
#define SOC_PETRA_CELL_DATA2_REG1_LSB                  (0)
#define SOC_PETRA_CELL_DATA2_REG2_MSB                  (31)
#define SOC_PETRA_CELL_DATA2_REG2_LSB                  (12)
#define SOC_PETRA_CELL_DATA2_REG2_LENGTH               (SOC_PETRA_CELL_DATA2_REG2_MSB - SOC_PETRA_CELL_DATA2_REG2_LSB + 1)
#define SOC_PETRA_CELL_ACK_REG                          9
#define SOC_PETRA_CELL_ENTRY_REG1                      2
#define SOC_PETRA_CELL_ENTRY_REG2                      3
#define SOC_PETRA_CELL_ENTRY_REG1_MSB                  (13)
#define SOC_PETRA_CELL_ENTRY_REG1_LSB                  (0)
#define SOC_PETRA_CELL_ENTRY_REG2_MSB                  (31)
#define SOC_PETRA_CELL_ENTRY_REG2_LSB                  (31)
#define SOC_PETRA_CELL_ENTRY_REG2_LENGTH               (SOC_PETRA_CELL_ENTRY_REG2_MSB - SOC_PETRA_CELL_ENTRY_REG2_LSB + 1)
#define SOC_PETRA_CELL_FE_LOC_REG                      0
#define SOC_PETRA_CELL_FE_LOC_REG_MSB                  (16)
#define SOC_PETRA_CELL_FE_LOC_REG_LSB                  (14)
#define SOC_PETRA_CELL_PATH0_REG1                      0
#define SOC_PETRA_CELL_PATH0_REG2                      1
#define SOC_PETRA_CELL_PATH0_REG1_MSB                  (12)
#define SOC_PETRA_CELL_PATH0_REG1_LSB                  (8)
#define SOC_PETRA_CELL_PATH0_REG2_MSB                  (23)
#define SOC_PETRA_CELL_PATH0_REG2_LSB                  (23)
#define SOC_PETRA_CELL_PATH1_REG1                      0
#define SOC_PETRA_CELL_PATH1_REG2                      1
#define SOC_PETRA_CELL_PATH1_REG1_MSB                  (7)
#define SOC_PETRA_CELL_PATH1_REG1_LSB                  (3)
#define SOC_PETRA_CELL_PATH1_REG2_MSB                  (22)
#define SOC_PETRA_CELL_PATH1_REG2_LSB                  (22)
#define SOC_PETRA_CELL_PATH_REG2_TH                    (31)
#define SOC_PETRA_CELL_INBAND_CELL_REG                 (0x2c81)
#define SOC_PETRA_CELL_ACK_DATA0_REG1                  SOC_PETRA_CELL_ENTRY_REG1
#define SOC_PETRA_CELL_ACK_DATA0_REG2                  SOC_PETRA_CELL_ENTRY_REG2
#define SOC_PETRA_CELL_ACK_DATA1_REG1                  SOC_PETRA_CELL_DATA0_REG1
#define SOC_PETRA_CELL_ACK_DATA1_REG2                  SOC_PETRA_CELL_DATA0_REG2
#define SOC_PETRA_CELL_ACK_DATA2_REG1                  SOC_PETRA_CELL_DATA1_REG1
#define SOC_PETRA_CELL_ACK_DATA2_REG2                  SOC_PETRA_CELL_DATA1_REG2
#define SOC_PETRA_CELL_ACK_DATA0_REG1_MSB              (30)
#define SOC_PETRA_CELL_ACK_DATA0_REG1_LSB              (SOC_PETRA_CELL_ENTRY_REG1_LSB)
#define SOC_PETRA_CELL_ACK_DATA0_REG2_MSB              (SOC_PETRA_CELL_ENTRY_REG2_MSB)
#define SOC_PETRA_CELL_ACK_DATA0_REG2_LSB              (SOC_PETRA_CELL_ENTRY_REG2_LSB)
#define SOC_PETRA_CELL_ACK_DATA0_REG2_LENGTH           (SOC_PETRA_CELL_ENTRY_REG2_LENGTH)
#define SOC_PETRA_CELL_ACK_DATA1_REG1_MSB              (SOC_PETRA_CELL_DATA0_REG1_MSB)
#define SOC_PETRA_CELL_ACK_DATA1_REG1_LSB              (SOC_PETRA_CELL_DATA0_REG1_LSB)
#define SOC_PETRA_CELL_ACK_DATA1_REG2_MSB              (SOC_PETRA_CELL_DATA0_REG2_MSB)
#define SOC_PETRA_CELL_ACK_DATA1_REG2_LSB              (SOC_PETRA_CELL_DATA0_REG2_LSB)
#define SOC_PETRA_CELL_ACK_DATA1_REG2_LENGTH           (SOC_PETRA_CELL_DATA0_REG2_LENGTH)
#define SOC_PETRA_CELL_ACK_DATA2_REG1_MSB              (SOC_PETRA_CELL_DATA1_REG1_MSB)
#define SOC_PETRA_CELL_ACK_DATA2_REG1_LSB              (SOC_PETRA_CELL_DATA1_REG1_LSB)
#define SOC_PETRA_CELL_ACK_DATA2_REG2_MSB              (SOC_PETRA_CELL_DATA1_REG2_MSB)
#define SOC_PETRA_CELL_ACK_DATA2_REG2_LSB              (SOC_PETRA_CELL_DATA1_REG2_LSB)
#define SOC_PETRA_CELL_ACK_DATA2_REG2_LENGTH           (SOC_PETRA_CELL_DATA1_REG2_LENGTH)
#define SOC_PETRA_CELL_RTP_MC_UPDATE_TBL_ADDR          (0x00010000)
#define SOC_PETRA_CELL_RTP_MC_TBL_1_ADDR               (0x00080000)

#define SOC_PETRA_SR_DATA_CELL_ADDRESS_START_FE1600      (202)       
#define SOC_PETRA_SR_DATA_CELL_ADDRESS_FE1600            (32)
#define SOC_PETRA_SR_DATA_CELL_RW_START_FE1600           (74)
#define SOC_PETRA_SR_DATA_CELL_WRITE_LENGTH_FE1600       (128)
#define SOC_PETRA_SR_DATA_CELL_CELL_IDENT_START_FE1600   (259)
#define SOC_PETRA_SR_DATA_CELL_CELL_IDENT_LENGTH_FE1600  (11)
#define SOC_PETRA_SR_DATA_CELL_CELL_FORMAT_START_FE1600  (270)
#define SOC_PETRA_SR_DATA_CELL_CELL_FORMAT_LENGTH_FE1600 (2)
#define SOC_PETRA_SR_DATA_CELL_OPCODE_START_FE1600       (242)
#define SOC_PETRA_SR_DATA_CELL_OPCODE_LENGTH_FE1600      (2)
#define SOC_PETRA_SR_DATA_CELL_BLOCK_ID_START_FE1600     (236)
#define SOC_PETRA_SR_DATA_CELL_BLOCK_ID_LENGTH_FE1600    (6)
#define SOC_PETRA_SR_DATA_CELL_PAYLOAD_START_FE1600      (16)




/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
typedef enum
{
  /*
   *  The different indexes of the filter.
   */
  /*
   *	Multicast id index
   */
  SOC_PETRA_CELL_MC_FILTER_MC_ID = 0,
  /*
   *	FE location
   */
  SOC_PETRA_CELL_MC_FILTER_FE_LOC = 1,
  /*
   *	The two path links
   */
   SOC_PETRA_CELL_MC_FILTER_PATH_LINKS0 = 2,
   SOC_PETRA_CELL_MC_FILTER_PATH_LINKS1 = 3,
   /*
    *	The three data
    */
    SOC_PETRA_CELL_MC_FILTER_DATA0 = 4,
    SOC_PETRA_CELL_MC_FILTER_DATA1 = 5,
    SOC_PETRA_CELL_MC_FILTER_DATA2 = 6
}SOC_PETRA_CELL_MC_FILTER;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* $Id: petra_cell.c,v 1.12.12.1 Broadcom SDK $
 *	Global variable for the write API soc_petra_cell_mc_tbl_write and read
 *  to memorize the last inband cell sent
 */
static
  uint32
    Soc_petra_cell_mc_buffer_write[SOC_SAND_MAX_DEVICE][SOC_SAND_DATA_CELL_UINT32_SIZE],
    Soc_petra_cell_mc_buffer_read[SOC_SAND_MAX_DEVICE][SOC_SAND_DATA_CELL_UINT32_SIZE];


/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_petra_sr_fe1600_buffer_to_data_cell(
    SOC_SAND_IN  uint32                         *packed_data_cell,    
    SOC_SAND_OUT SOC_SAND_DATA_CELL                   *data_cell,
    SOC_SAND_OUT SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600 *added_data
  )
{
  uint32
    i_reg;
  uint32
    res = SOC_SAND_OK ,
    tmp_u32_data_cell_field = 0 ,
    packed_cpu_data_cell[SOC_SAND_DATA_CELL_UINT32_SIZE] ;
  uint32    
    tmp_field_for_data_cell_add_fields ,
    tmp_u32_sr_extra_field ,
    tmp_u32_sr_extra_field2 ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_os_memset(
          packed_cpu_data_cell,
          0x0,
          SOC_SAND_DATA_CELL_UINT32_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   * Inverting the registers
   */
  for (i_reg = 0 ; i_reg < SOC_SAND_DATA_CELL_UINT32_SIZE ; ++i_reg)
  {
    packed_cpu_data_cell[i_reg] = packed_data_cell[SOC_SAND_DATA_CELL_UINT32_SIZE - 1 - i_reg ] ;
  }


  /*
   * Copy of the common fields
   */
  /*
   * bits 319:318 - cell type
   */
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_DATA_CELL_CELL_TYPE_START,
          SOC_SAND_DATA_CELL_CELL_TYPE_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  data_cell->cell_type = (uint8) tmp_u32_data_cell_field;
  /* casting allowed: only 2 significant bits in tmp_u32_data_cell_field*/

  /*
   * bits 317:307 - source id
   */
  tmp_u32_data_cell_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_DATA_CELL_SOURCE_ID_START,
          SOC_SAND_DATA_CELL_SOURCE_ID_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  data_cell->source_id = (uint16) tmp_u32_data_cell_field;
  /* casting allowed: only 11 significant bits in tmp_u32_data_cell_field*/

  /*
   * Copy of the specific fields according to the data cell type:
   * either source routed or destination routed
   */
  /*
   * bits 306:304 - source level
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_DATA_CELL_SRC_LEVEL_START,
          SOC_SAND_DATA_CELL_SRC_LEVEL_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  data_cell->data_cell.source_routed.src_level = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 3 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 303:301 - destination level
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_DEST_LEVEL_START,
          SOC_SAND_DATA_CELL_DEST_LEVEL_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  data_cell->data_cell.source_routed.dest_level = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 3 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 300:296 - fip switch
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_FIP_SWITCH_START,
          SOC_SAND_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  data_cell->data_cell.source_routed.fip_switch = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 5 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 295:291 - fe1 switch
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_FE1_SWITCH_START,
          SOC_SAND_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  data_cell->data_cell.source_routed.fe1_switch = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 5 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 290:285 - fe2 switch
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_FE2_SWITCH_START,
          SOC_SAND_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  data_cell->data_cell.source_routed.fe2_switch = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 6 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 284:280 - fe3 switch
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          SOC_SAND_DATA_CELL_FE3_SWITCH_START,
          SOC_SAND_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  data_cell->data_cell.source_routed.fe3_switch = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 5 significant bits in tmp_u32_sr_extra_field*/

  /*
   * Filling the extra fields of the in-band cell
   */

  /*
   * bit 279 - FIP-switch
   */
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_POSITION,
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fip_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 223, exit);
  data_cell->data_cell.source_routed.fip_switch = (uint8) tmp_u32_sr_extra_field2 ;
  /* casting allowed: only 6 significant bit in tmp_u32_sr_extra_field2*/

  /*
   * bit 278 - FE1-switch
   */
  tmp_u32_sr_extra_field = 0 ;
  tmp_u32_sr_extra_field2 = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_POSITION,
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe1_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 233, exit);
  data_cell->data_cell.source_routed.fe1_switch = (uint8) tmp_u32_sr_extra_field2 ;
  /* casting allowed: only 6 significant bit in tmp_u32_sr_extra_field2*/

  /*
   * bit 277 - FE2-switch
   */
  tmp_u32_sr_extra_field = 0 ;
  tmp_u32_sr_extra_field2 = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_POSITION,
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe2_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 243, exit);
  data_cell->data_cell.source_routed.fe2_switch = (uint8) tmp_u32_sr_extra_field2 ;
  /* casting allowed: only 7 significant bit in tmp_u32_sr_extra_field2*/

  /*
   * bit 276 - FE3-switch
   */
  tmp_u32_sr_extra_field = 0 ;
  tmp_u32_sr_extra_field2 = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_POSITION,
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe3_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 253, exit);
  data_cell->data_cell.source_routed.fe3_switch = (uint8) tmp_u32_sr_extra_field2 ;
  /* casting allowed: only 6 significant bit in tmp_u32_sr_extra_field2*/

  /*
   * bit 275 - In-band cell
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_INBAND_CELL_POSITION,
          SOC_SAND_SR_DATA_CELL_INBAND_CELL_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
  data_cell->data_cell.source_routed.inband_cell = (uint8) tmp_u32_sr_extra_field ;
  /* casting allowed: only 1 significant bit in tmp_u32_sr_extra_field*/

  /*
   * bit 274 - Ack
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_ACK_POSITION,
          SOC_SAND_SR_DATA_CELL_ACK_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
  data_cell->data_cell.source_routed.ack = (uint8) tmp_u32_sr_extra_field ;
  /* casting allowed: only 1 significant bit in tmp_u32_sr_extra_field*/

  /*
   * bit 273 - Indirect
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_INDIRECT_POSITION,
          SOC_SAND_SR_DATA_CELL_INDIRECT_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
  data_cell->data_cell.source_routed.indirect = (uint8) tmp_u32_sr_extra_field ;
  /* casting allowed: only 1 significant bit in tmp_u32_sr_extra_field*/

  /*
   * bit 272 - Read / write (0/1)
   */
  tmp_u32_sr_extra_field = 0 ;
  res = soc_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          SOC_SAND_SR_DATA_CELL_RW_POSITION,
          SOC_SAND_SR_DATA_CELL_RW_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
  data_cell->data_cell.source_routed.read_or_write = (uint8) tmp_u32_sr_extra_field ;
  /* casting allowed: only 1 significant bit in tmp_u32_sr_extra_field*/

  /*
   * Decomposing the payload, differentiating between inband or regular
   */
  if (data_cell->data_cell.source_routed.inband_cell)
  {
    /*
     * bits  - Read / write address
     */
    tmp_field_for_data_cell_add_fields = 0 ;
    res = soc_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            SOC_PETRA_SR_DATA_CELL_ADDRESS_START_FE1600,            
            SOC_PETRA_SR_DATA_CELL_ADDRESS_FE1600,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
    soc_sand_os_memcpy(data_cell->data_cell.source_routed.add_wr_cell,&tmp_u32_sr_extra_field,sizeof(uint32));

    /*
     * bits - Write_data, read data on the ack cell
     */    
    res = soc_sand_bitstream_get_any_field(            
            packed_cpu_data_cell,
            SOC_PETRA_SR_DATA_CELL_RW_START_FE1600,
            SOC_PETRA_SR_DATA_CELL_WRITE_LENGTH_FE1600,
            data_cell->data_cell.source_routed.data_wr_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);    


    /*
     * bits 10:2 - Cell identifier
     */
    tmp_field_for_data_cell_add_fields = 0;
    res = soc_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            SOC_PETRA_SR_DATA_CELL_CELL_IDENT_START_FE1600 ,
            SOC_PETRA_SR_DATA_CELL_CELL_IDENT_LENGTH_FE1600,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);
    data_cell->data_cell.source_routed.cell_ident = (uint16) tmp_field_for_data_cell_add_fields;
    /* casting allowed: only 9 significant bits in tmp_field_for_data_cell_add_fields*/

   /*
    * bits 1:0 - Cell format (for future cell formats)
    */
    tmp_field_for_data_cell_add_fields = 0 ;
    res = soc_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            SOC_SAND_SR_DATA_CELL_CELL_FORMAT_START,
            SOC_SAND_SR_DATA_CELL_CELL_FORMAT_LENGTH,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 570, exit);
    data_cell->data_cell.source_routed.cell_format = (uint8) tmp_field_for_data_cell_add_fields;
    /* casting allowed: only 2 significant bits in tmp_field_for_data_cell_add_fields*/

    /*
     * bits - fe1600 cell format 
     */
    tmp_field_for_data_cell_add_fields = 0;
    res = soc_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            SOC_PETRA_SR_DATA_CELL_CELL_FORMAT_START_FE1600 ,
            SOC_PETRA_SR_DATA_CELL_CELL_FORMAT_LENGTH_FE1600,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);
    if (tmp_field_for_data_cell_add_fields != 1)
    {
      /*add error code*/
    }   
    
    /*
     * bits - fe1600 opcode
     */
    tmp_field_for_data_cell_add_fields = 0;
    res = soc_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            SOC_PETRA_SR_DATA_CELL_OPCODE_START_FE1600 ,
            SOC_PETRA_SR_DATA_CELL_OPCODE_LENGTH_FE1600,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);
    data_cell->data_cell.source_routed.indirect = (uint8) tmp_field_for_data_cell_add_fields >> 1;
    data_cell->data_cell.source_routed.read_or_write = (uint8) tmp_field_for_data_cell_add_fields & 0x1;

    /*
     * bits - fe1600 block id
     */
    tmp_field_for_data_cell_add_fields = 0;
    res = soc_sand_bitstream_set_any_field(
            packed_cpu_data_cell,
            SOC_PETRA_SR_DATA_CELL_BLOCK_ID_START_FE1600 ,
            SOC_PETRA_SR_DATA_CELL_BLOCK_ID_LENGTH_FE1600,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);
    added_data->block_id = (uint16) tmp_field_for_data_cell_add_fields; /*used to contain the block id*/
  }
  else /* Regular source routed cell, unpacking the same way we packed */
  {
    /*
     * bits 256:0 - whole payload for regular sr cell
     */    
    res = soc_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            SOC_PETRA_SR_DATA_CELL_PAYLOAD_START_FE1600,
            SOC_PETRA_SR_DATA_CELL_PAYLOAD_LENGTH_FE1600,
            (uint32 *) added_data->cell_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);
  }  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_buffer_to_data_cell()",0,0);
}


uint32
  soc_petra_sr_data_cell_to_fe1600_buffer(
    SOC_SAND_IN  SOC_SAND_DATA_CELL   *data_cell,
    SOC_SAND_OUT uint32         *packed_data_cell,
    SOC_SAND_IN  SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600 *added_data
  )
{
  uint32
    i_reg = 0 ,
    length ; /* for the length due to the inscription of the common fields*/
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp_u32_inversion = 0,
    tmp_u32_data_cell_field = 0,
    packed_cpu_data_cell[SOC_SAND_DATA_CELL_UINT32_SIZE];
  uint32
    iter = 0;
  uint32    
    tmp_u32_sr_extra_field ,
    tmp_u32_sr_extra_field2 ;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  res = soc_sand_os_memset(
          packed_cpu_data_cell,
          0x0,
          SOC_SAND_DATA_CELL_UINT32_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   * Copy of the common fields
   */
  /*
   * bits 319:318 - Copy of the cell type
   */
  tmp_u32_data_cell_field = data_cell->cell_type ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          SOC_SAND_DATA_CELL_CELL_TYPE_START ,
          SOC_SAND_DATA_CELL_CELL_TYPE_LENGTH,
          packed_data_cell
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * bits 317:307 - source id
   */
  tmp_u32_data_cell_field = data_cell->source_id ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          SOC_SAND_DATA_CELL_SOURCE_ID_START,
          SOC_SAND_DATA_CELL_SOURCE_ID_LENGTH,
          packed_data_cell
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  length = SOC_SAND_SR_DATA_CELL_NOT_COMMON_LENGTH ;
  
  /*
   * bits 306:304 - source level
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.src_level ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_SRC_LEVEL_START ,
          SOC_SAND_DATA_CELL_SRC_LEVEL_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   * bits 303:301 - destination level
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.dest_level ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_DEST_LEVEL_START ,
          SOC_SAND_DATA_CELL_DEST_LEVEL_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   * bits 300:296 - fip switch
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fip_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_FIP_SWITCH_START ,
          SOC_SAND_DATA_CELL_FIP_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
   * bits 295:291 - fe1 switch
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fe1_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_FE1_SWITCH_START ,
          SOC_SAND_DATA_CELL_FE1_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  /*
   * bits 290:285 - fe2 switch
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fe2_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_FE2_SWITCH_START ,
          SOC_SAND_DATA_CELL_FE2_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 77, exit);

  /*
   * bits 284:280 - fe3 switch
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fe3_switch ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_DATA_CELL_FE3_SWITCH_START ,
          SOC_SAND_DATA_CELL_FE3_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  /*
   * Filling the values between the first fields and the payload - bits 279:272
   */
  /*
   * bit 279 - FIP-switch
   */
  tmp_u32_sr_extra_field = 0;
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fip_switch ;
  res = soc_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 215, exit);
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_POSITION ,
          SOC_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

  /*
   * bit 278 - FE1-switch
   */
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe1_switch ;
  res = soc_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_POSITION ,
          SOC_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

  /*
   * bit 277 - FE2-switch
   */
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe2_switch ;
  res = soc_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 235, exit);
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_POSITION ,
          SOC_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

  /*
   * bit 276 - FE3-switch
   */
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe3_switch ;
  res = soc_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_START,
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 245, exit);
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_POSITION ,
          SOC_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);

  /*
  * bit 275 - In-band cell
  */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.inband_cell ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_INBAND_CELL_POSITION ,
          SOC_SAND_SR_DATA_CELL_INBAND_CELL_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

  /*
   * bit 274 - Ack
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.ack ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_ACK_POSITION ,
          SOC_SAND_SR_DATA_CELL_ACK_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);

  /*
   * bit 273 - Indirect
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.indirect ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_INDIRECT_POSITION ,
          SOC_SAND_SR_DATA_CELL_INDIRECT_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);

  /*
   * bit 272 - Read / write (0/1)
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.read_or_write ;
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          SOC_SAND_SR_DATA_CELL_RW_POSITION ,
          SOC_SAND_SR_DATA_CELL_RW_LENGTH,
          (packed_cpu_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
  

  /*
   * Building the payload, differentiating between inband or regular source routed cell
   */
  if (data_cell->data_cell.source_routed.inband_cell)
  {
    /*
     * bits  - Read / write address
     */
    soc_sand_os_memcpy(&tmp_u32_sr_extra_field,data_cell->data_cell.source_routed.add_wr_cell,sizeof(uint32));

    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            SOC_PETRA_SR_DATA_CELL_ADDRESS_START_FE1600,            
            SOC_PETRA_SR_DATA_CELL_ADDRESS_FE1600,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
    /*
     * bits - Write_data, read data on the ack cell
     */    
    res = soc_sand_bitstream_set_any_field(
            data_cell->data_cell.source_routed.data_wr_cell,
            SOC_PETRA_SR_DATA_CELL_RW_START_FE1600,
            SOC_PETRA_SR_DATA_CELL_WRITE_LENGTH_FE1600,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);    

    /*
     * bits 10:2 - Cell identifier
     */
    tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.cell_ident;
    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            SOC_PETRA_SR_DATA_CELL_CELL_IDENT_START_FE1600 ,
            SOC_PETRA_SR_DATA_CELL_CELL_IDENT_LENGTH_FE1600,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);

    /*
     * bits 1:0 - (same location as for fe600) - Cell format (for future cell formats)
     */
    tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.cell_format;
    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            SOC_SAND_SR_DATA_CELL_CELL_FORMAT_START ,
            SOC_SAND_SR_DATA_CELL_CELL_FORMAT_LENGTH,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

    /*
     * bits - fe1600 cell format == 0x1 (vsc128 for fe1600)
     */
    tmp_u32_sr_extra_field = 0x1;
    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            SOC_PETRA_SR_DATA_CELL_CELL_FORMAT_START_FE1600 ,
            SOC_PETRA_SR_DATA_CELL_CELL_FORMAT_LENGTH_FE1600,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);

    /*
     * bits - fe1600 number of valid commands - being overwritten by hw to 1.
     */
    
    /*
     * bits - fe1600 opcode
     */
    tmp_u32_sr_extra_field = (data_cell->data_cell.source_routed.indirect << 1) + data_cell->data_cell.source_routed.read_or_write;
    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            SOC_PETRA_SR_DATA_CELL_OPCODE_START_FE1600 ,
            SOC_PETRA_SR_DATA_CELL_OPCODE_LENGTH_FE1600,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);

    /*
     * bits - fe1600 block id
     */
    tmp_u32_sr_extra_field = added_data->block_id; /*used to contain the block id*/
    res = soc_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            SOC_PETRA_SR_DATA_CELL_BLOCK_ID_START_FE1600 ,
            SOC_PETRA_SR_DATA_CELL_BLOCK_ID_LENGTH_FE1600,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);

  }
  else /* Regular source routed cell (not inband) */
  {
    /*
     * bits 256:0 - whole payload for regular sr cell
     */
    tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.add_wr_cell[iter];
    res = soc_sand_bitstream_set_any_field(
            (uint32 *) added_data->cell_data,
            SOC_PETRA_SR_DATA_CELL_PAYLOAD_START_FE1600,
            SOC_PETRA_SR_DATA_CELL_PAYLOAD_LENGTH_FE1600,
            packed_cpu_data_cell
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);
  }

  res = soc_sand_bitstream_set_any_field(
          packed_cpu_data_cell,
          0,
          length,
          (packed_data_cell)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   * Inverting the registers
   */
  for (i_reg = 0 ; i_reg < SOC_SAND_DATA_CELL_UINT32_SIZE / 2; ++i_reg)
  {
    tmp_u32_inversion = (packed_data_cell[SOC_SAND_DATA_CELL_UINT32_SIZE - 1 - i_reg ]);
    (packed_data_cell[SOC_SAND_DATA_CELL_UINT32_SIZE - 1 - i_reg ]) = (packed_data_cell[i_reg ]) ;
    (packed_data_cell[i_reg ]) = tmp_u32_inversion ;
  }      
  

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_data_cell_to_buffer()",0,0);

}



/*
 * Builds the data cell fields from the input.
 */
STATIC uint32
soc_petra_build_data_cell_for_fe1600(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST          *sr_link_list,
    SOC_SAND_IN  soc_reg_t                        reg,
    SOC_SAND_IN  soc_mem_t                        mem,
    SOC_SAND_IN  int32                         port_or_copyno,
    SOC_SAND_IN  int32                         index,
    SOC_SAND_IN  uint32                         *data_in,
    SOC_SAND_IN  uint32                         nof_words,
    SOC_SAND_IN  uint32                        is_write,
    SOC_SAND_IN  uint32                        is_memory,
    SOC_SAND_IN  uint8                        is_inband,
    SOC_SAND_OUT SOC_SAND_DATA_CELL                   *data_cell_sent,
    SOC_SAND_OUT SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600 *added_data
  )
{
  int32
    res = SOC_SAND_OK;
  uint8
    at;
  int32
    block,
    index_valid;    
  uint32
    maddr,
    blkoff;
  soc_mem_info_t
    *meminfo;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  /*
   * Construction of the cell information
   */
  data_cell_sent->cell_type = 1 ;
  data_cell_sent->source_id = (uint16) unit ;
  /* casting allowed: the unit should be in 11 bits*/
  data_cell_sent->data_cell.source_routed.src_level = 1 ;
  data_cell_sent->data_cell.source_routed.dest_level =
    (uint8) soc_sand_actual_entity_value( sr_link_list->dest_entity_type ) ;
  /* casting allowed: only 3 significant bits in the destination level */

  data_cell_sent->data_cell.source_routed.fip_switch =
    sr_link_list->path_links[SOC_PETRA_CELL_PATH_LINK_FIP_SWITCH_POSITION] ;
  /* casting allowed: only 6 significant bits */
  data_cell_sent->data_cell.source_routed.fe1_switch =
    sr_link_list->path_links[SOC_PETRA_CELL_PATH_LINK_FE1_SWITCH_POSITION] ;
  /* casting allowed: only 6 significant bits */
  data_cell_sent->data_cell.source_routed.fe2_switch =
    sr_link_list->path_links[SOC_PETRA_CELL_PATH_LINK_FE2_SWITCH_POSITION] ;
  /* casting allowed: only 7 significant bits in tmp_cell_info */
  data_cell_sent->data_cell.source_routed.fe3_switch =
    sr_link_list->path_links[SOC_PETRA_CELL_PATH_LINK_FE3_SWITCH_POSITION] ;
  /* casting allowed: only 6 significant bits in tmp_cell_info */

  if (is_inband)
  {
    data_cell_sent->data_cell.source_routed.indirect = is_memory ; /*reserved bit*/
    data_cell_sent->data_cell.source_routed.read_or_write = is_write ;

    data_cell_sent->data_cell.source_routed.inband_cell = 1 ;
    data_cell_sent->data_cell.source_routed.ack = 0 ;

    if (!is_memory) /* Direct command */
    {
      if (!SOC_REG_IS_VALID(unit, reg)) {
        LOG_ERROR(BSL_LS_SOC_FABRIC,
                  (BSL_META_U(unit,
                              "Invalid register %s\n"),
                   SOC_REG_NAME(unit, reg)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
      }

      maddr = soc_reg_addr_get(unit, reg, port_or_copyno, index, SOC_REG_ADDR_OPTION_WRITE, &block, &at);
      added_data->block_id = ((maddr >> SOC_BLOCK_BP) & 0xf) | (((maddr >> SOC_BLOCK_MSB_BP) & 0x1) << 4);

      /*clear block from address*/
      blkoff = ((added_data->block_id & 0xf) << SOC_BLOCK_BP) | (((added_data->block_id >> 4) & 0x3) << SOC_BLOCK_MSB_BP);
      data_cell_sent->data_cell.source_routed.add_wr_cell[0] = maddr - blkoff;

      if(is_write) {
        soc_sand_os_memcpy(data_cell_sent->data_cell.source_routed.data_wr_cell,data_in,nof_words * sizeof(uint32));
      }
    }
  
    else /* Indirect command */
    {
      if (!soc_mem_is_valid(unit, mem)) {
        LOG_ERROR(BSL_LS_SOC_FABRIC,
                  (BSL_META_U(unit,
                              "Invalid memory %s\n"),
                   SOC_MEM_NAME(unit, mem)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
      }
      meminfo = &SOC_MEM_INFO(unit, mem);

      if (port_or_copyno == MEM_BLOCK_ANY) {
        block = SOC_MEM_BLOCK_ANY(unit, mem);
      } else {
        block = port_or_copyno;
      }

      if (!SOC_MEM_BLOCK_VALID(unit, mem, block)) {
        LOG_ERROR(BSL_LS_SOC_FABRIC,
                  (BSL_META_U(unit,
                              "invalid block %d for memory %s\n"),
                   block, SOC_MEM_NAME(unit, mem)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
      }

      index_valid = (index >= 0 && index <= soc_mem_index_max(unit, mem));
      if (!index_valid) {
        LOG_ERROR(BSL_LS_SOC_FABRIC,
                  (BSL_META_U(unit,
                              "invalid index %d for memory %s\n"),
                   index, SOC_MEM_NAME(unit, mem)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
      }

      maddr = soc_mem_addr_get(unit, mem, 0, block, index, &at);
      added_data->block_id = ((maddr >> SOC_BLOCK_BP) & 0xf) | (((maddr >> SOC_BLOCK_MSB_BP) & 0x1) << 4);
        
      /*clear block from address*/
      blkoff = ((added_data->block_id & 0xf) << SOC_BLOCK_BP) | (((added_data->block_id >> 4) & 0x3) << SOC_BLOCK_MSB_BP);
      data_cell_sent->data_cell.source_routed.add_wr_cell[0] = maddr - blkoff;

      if(is_write) {
        if(meminfo->bytes > 128) {
          LOG_ERROR(BSL_LS_SOC_FABRIC,
                    (BSL_META_U(unit,
                                "can't read more than 128 byte data\n")));
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
        }
        soc_sand_os_memcpy(data_cell_sent->data_cell.source_routed.data_wr_cell,data_in,nof_words * sizeof(uint32));
      }
    }

    res = soc_petra_sw_db_cell_cell_ident_get(
            unit,
            &(data_cell_sent->data_cell.source_routed.cell_ident)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = soc_petra_sw_db_cell_cell_ident_set(
            unit,
            ( (data_cell_sent->data_cell.source_routed.cell_ident + 1) % SOC_PETRA_CELL_NOF_CELL_IDENTS )
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    data_cell_sent->data_cell.source_routed.cell_format = 0 ;
  }
  else /* Regular source routed cell, not inband*/
  {
    data_cell_sent->data_cell.source_routed.indirect = 0 ;
    data_cell_sent->data_cell.source_routed.read_or_write = 0 ;

    data_cell_sent->data_cell.source_routed.inband_cell = 0 ;
    data_cell_sent->data_cell.source_routed.ack = 0 ;

    /*use 256 bits in added_data->cell_data*/
    soc_sand_os_memcpy(added_data->cell_data,data_in,nof_words * sizeof(uint32));    

    data_cell_sent->data_cell.source_routed.cell_format = 0 ;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_build_data_cell_for_fe1600()",0,0);
}

/*
 * Builds the data cell fields from the input.
 */
STATIC uint32
soc_petra_build_data_cell_for_fe600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in,
    SOC_SAND_IN  uint32                        nof_words,
    SOC_SAND_IN  uint8                       is_write,
    SOC_SAND_IN  uint8                       is_indirect,
    SOC_SAND_IN  uint8                       is_inband,
    SOC_SAND_OUT SOC_SAND_DATA_CELL                  *data_cell_sent
  )
{
  uint32
    res = SOC_SAND_OK ,
    tmp_field_for_data_cell_add_fields = 0,
    trigger = 0x1 ,
    iter_nof_words = 0 ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_BUILD_DATA_CELL_FOR_FE600);

  /*
   * Construction of the cell information
   */
  data_cell_sent->cell_type = 1 ;
  data_cell_sent->source_id = (uint16) unit ;
  /* casting allowed: the unit should be in 11 bits*/
  data_cell_sent->data_cell.source_routed.src_level = 1 ;
  data_cell_sent->data_cell.source_routed.dest_level =
    (uint8) soc_sand_actual_entity_value( sr_link_list->dest_entity_type ) ;
  /* casting allowed: only 3 significant bits in the destination level */

  data_cell_sent->data_cell.source_routed.fip_switch =
    sr_link_list->path_links[SOC_PETRA_CELL_PATH_LINK_FIP_SWITCH_POSITION] ;
  /* casting allowed: only 6 significant bits */
  data_cell_sent->data_cell.source_routed.fe1_switch =
    sr_link_list->path_links[SOC_PETRA_CELL_PATH_LINK_FE1_SWITCH_POSITION] ;
  /* casting allowed: only 6 significant bits */
  data_cell_sent->data_cell.source_routed.fe2_switch =
    sr_link_list->path_links[SOC_PETRA_CELL_PATH_LINK_FE2_SWITCH_POSITION] ;
  /* casting allowed: only 7 significant bits in tmp_cell_info */
  data_cell_sent->data_cell.source_routed.fe3_switch =
    sr_link_list->path_links[SOC_PETRA_CELL_PATH_LINK_FE3_SWITCH_POSITION] ;
  /* casting allowed: only 6 significant bits in tmp_cell_info */

  if (is_inband)
  {
    data_cell_sent->data_cell.source_routed.indirect = is_indirect ;
    data_cell_sent->data_cell.source_routed.read_or_write = is_write ;

    data_cell_sent->data_cell.source_routed.inband_cell = 1 ;
    data_cell_sent->data_cell.source_routed.ack = 0 ;

    if (!is_indirect) /* Direct command */
    {
      if (!is_write) /* Direct read command: 1 word in each cell */
      {
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_0] = (uint16) offset ;
        /* casting allowed: only 16 significant bit in offset */
      }
      else /* Direct write command: 5 words in each cell */
      {
        for (iter_nof_words = 0 ; iter_nof_words < nof_words ; ++iter_nof_words)
        {
          data_cell_sent->data_cell.source_routed.add_wr_cell[iter_nof_words] = (uint16) offset ;
          /* casting allowed: only 16 significant bit in offset */
          data_cell_sent->data_cell.source_routed.data_wr_cell[iter_nof_words] = data_in[ iter_nof_words ] ;
        }
      }
    }
    else /* Indirect command */
    {
      if (!is_write) /* Indirect read command: 5 words in each cell */
      {
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_0] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_RW_ADDR ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_1] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_RW_TRIGGER ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_2] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_READ_DATA0 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_3] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_READ_DATA1 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_4] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_READ_DATA2 ;

        data_cell_sent->data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_0] = SOC_SAND_BIT(31)|offset;
        /* read address: bit(31) + base + offset */
        data_cell_sent->data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_1] = trigger ;
        /* write trigger */
        for (iter_nof_words = 0 ; iter_nof_words < SOC_PETRA_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL ; ++iter_nof_words)
        {
          data_cell_sent->data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_2 + iter_nof_words] = 0x0 ;
        }
        /* write data equals zero for the three last words */
      }
      else /* Indirect write command: 5 words in each cell */
      {
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_0] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_RW_ADDR ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_1] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_WRITE_DATA0 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_2] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_WRITE_DATA1 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_3] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_WRITE_DATA2 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[SOC_PETRA_CELL_ADDRESS_POSITION_4] =
          (uint16) SOC_PETRA_FE600_RTP_INDIRECT_RW_TRIGGER ;

        data_cell_sent->data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_0] = offset;
        /* write address: base + offset*/
        for (iter_nof_words = 0 ; iter_nof_words < nof_words ; ++iter_nof_words)
        {
          data_cell_sent->data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_1 + iter_nof_words] =
            data_in[ iter_nof_words ] ;
        }
        data_cell_sent->data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_4] = trigger ;
        /* write trigger */
      }
    }

    res = soc_petra_sw_db_cell_cell_ident_get(
            unit,
            &(data_cell_sent->data_cell.source_routed.cell_ident)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

    res = soc_petra_sw_db_cell_cell_ident_set(
            unit,
            ( (data_cell_sent->data_cell.source_routed.cell_ident + 1) % SOC_PETRA_CELL_NOF_CELL_IDENTS )
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 9, exit);

    data_cell_sent->data_cell.source_routed.cell_format = 0 ;
  }
  else /* Regular source routed cell, not inband*/
  {
    data_cell_sent->data_cell.source_routed.indirect = 0 ;
    data_cell_sent->data_cell.source_routed.read_or_write = 0 ;

    data_cell_sent->data_cell.source_routed.inband_cell = 0 ;
    data_cell_sent->data_cell.source_routed.ack = 0 ;

    for (iter_nof_words = 0 ; iter_nof_words < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter_nof_words)
    {
      data_cell_sent->data_cell.source_routed.data_wr_cell[iter_nof_words] = data_in[ iter_nof_words ] ;
    }

    for (iter_nof_words = 0 ; iter_nof_words < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter_nof_words)
    {
      tmp_field_for_data_cell_add_fields = 0 ;

      res = soc_sand_bitstream_get_any_field(
              &(data_in[SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD]),
              iter_nof_words * SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      data_cell_sent->data_cell.source_routed.add_wr_cell[iter_nof_words] = (uint16) tmp_field_for_data_cell_add_fields;
      /* casting allowed: only 16 significant bits in tmp_field_for_data_cell_add_fields*/
    }

    tmp_field_for_data_cell_add_fields = 0 ;
    res = soc_sand_bitstream_get_any_field(
            &(data_in[SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD]),
            SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD * SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
            SOC_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_LENGTH,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    data_cell_sent->data_cell.source_routed.cell_ident = (uint16) tmp_field_for_data_cell_add_fields;
    /* casting allowed: only 16 significant bits in tmp_field_for_data_cell_add_fields*/

    data_cell_sent->data_cell.source_routed.cell_format = 0 ;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_build_data_cell_for_fe600()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_cell_regs_init
* FUNCTION:
*   Initialization of the Soc_petra blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC uint32
  soc_petra_cell_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_REGS_INIT);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->fdt.transmit_cell_output_link_number_reg.cpu_cell_size, 0x3f, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_cell_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_cell_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_cell_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    offset,
    cell_mc_buffer_write[SOC_SAND_DATA_CELL_UINT32_SIZE],
    cell_mc_buffer_read[SOC_SAND_DATA_CELL_UINT32_SIZE],
    data_in = 0,
    res = SOC_SAND_OK;
  SOC_SAND_DATA_CELL
    data_cell_write,
    data_cell_read;
  SOC_PETRA_SR_CELL_LINK_LIST
    sr_link_list;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_INIT);
  res = SOC_SAND_OK; sal_memset(&data_cell_write, 0x0, sizeof(SOC_SAND_DATA_CELL));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = SOC_SAND_OK; sal_memset(&data_cell_read, 0x0, sizeof(SOC_SAND_DATA_CELL));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  soc_petra_PETRA_SR_CELL_LINK_LIST_clear(&sr_link_list);

  res = soc_petra_cell_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Initialize the global write and read variables
   */
  sr_link_list.dest_entity_type = SOC_SAND_FE2_ENTITY;
  offset = SOC_PETRA_CELL_RTP_MC_UPDATE_TBL_ADDR;
  res = soc_petra_build_data_cell_for_fe600(
          unit,
          &sr_link_list,
          offset,
          &data_in,
          0,
          TRUE,
          TRUE,
          TRUE,
          &data_cell_write
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_data_cell_to_buffer(
          &data_cell_write,
          cell_mc_buffer_write
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  offset = SOC_PETRA_CELL_RTP_MC_TBL_1_ADDR;
  res = soc_petra_build_data_cell_for_fe600(
          unit,
          &sr_link_list,
          offset,
          &data_in,
          0,
          FALSE,
          TRUE,
          TRUE,
          &data_cell_read
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_sand_data_cell_to_buffer(
          &data_cell_read,
          cell_mc_buffer_read
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  SOC_PETRA_COPY(Soc_petra_cell_mc_buffer_write[unit], cell_mc_buffer_write, uint32, SOC_SAND_DATA_CELL_UINT32_SIZE);
  SOC_PETRA_COPY(Soc_petra_cell_mc_buffer_read[unit], cell_mc_buffer_read, uint32, SOC_SAND_DATA_CELL_UINT32_SIZE);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_init()",0,0);
}

/*
 * Sends a cell and acts on the trigger
 */
uint32
  soc_petra_sr_send_cell(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       is_fe1600,
    SOC_SAND_IN  SOC_SAND_DATA_CELL  *data_cell_sent,
    SOC_SAND_IN SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600 *added_data
  )
{
  uint32
    tmp_u32_fip_switch = 0 ,
    tmp_u32_output_link_in_five_bits = 0 ,
    res = SOC_SAND_OK;
  uint32
    packed_cpu_data_cell_sent[SOC_SAND_DATA_CELL_UINT32_SIZE] ;
  uint32
    idx = 0 ;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SR_SEND_CELL);

  regs = soc_petra_regs();

  res = SOC_SAND_OK; sal_memset(
          packed_cpu_data_cell_sent,
          0x0,
          SOC_SAND_DATA_CELL_UINT32_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   * Pack the cell.
   */
  if (is_fe1600)
  {
    res = soc_petra_sr_data_cell_to_fe1600_buffer(
          data_cell_sent,
          packed_cpu_data_cell_sent,
          added_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } 
  else
  {
    res = soc_sand_data_cell_to_buffer(
          data_cell_sent,
          packed_cpu_data_cell_sent
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  
  

 /*
   * Copy the data.
   */
  for (idx = 0; idx < SOC_SAND_DATA_CELL_UINT32_SIZE; ++idx)
  {
    SOC_PETRA_REG_SET(regs->fdt.cpu_data_cell_reg[idx], packed_cpu_data_cell_sent[idx], 30, exit);
  }

  /*
   * Fix the output link which is the fip switch link
   */
  tmp_u32_fip_switch = data_cell_sent->data_cell.source_routed.fip_switch ;

  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_fip_switch),
          0 ,
          SOC_SAND_DATA_CELL_FIP_SWITCH_LENGTH+1,
          &tmp_u32_output_link_in_five_bits
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  SOC_PETRA_FLD_SET(regs->fdt.transmit_cell_output_link_number_reg.cpu_link_num, tmp_u32_output_link_in_five_bits, 35, exit);

  /*
   * Set the trigger.
   */
  SOC_PETRA_FLD_SET(regs->fdt.transmit_data_cell_trigger_reg.cpu_trg, 0x1, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sr_send_cell()",0,0);
}

STATIC uint32
  soc_petra_cell_ack_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                       *ack,
    SOC_SAND_OUT uint8                      *success
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val,
    success_a,
    success_b,
    success_c,
    success_d;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CELL_ACK_GET);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(ack);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   * Verification if the cell was received on the A side,
   * or on the B, C or D side
   */

  SOC_PETRA_REG_GET(regs->fdr.interrupt_reg, reg_val, 10, exit);

  SOC_PETRA_FLD_FROM_REG(regs->fdr.interrupt_reg.cpudatacellfne_a0, success_a, reg_val, 20, exit);
  SOC_PETRA_FLD_FROM_REG(regs->fdr.interrupt_reg.cpudatacellfne_a1, success_b, reg_val, 21, exit);
  SOC_PETRA_FLD_FROM_REG(regs->fdr.interrupt_reg.cpudatacellfne_b0, success_c, reg_val, 22, exit);
  SOC_PETRA_FLD_FROM_REG(regs->fdr.interrupt_reg.cpudatacellfne_b1, success_d, reg_val, 23, exit);

  if ( (success_a && success_b)  ||
       (success_a && success_c)  ||
       (success_a && success_d)  ||
       (success_b && success_c)  ||
       (success_b && success_d)  ||
       (success_c && success_d) )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }

  if ( (!success_a) && (!success_b) && (!success_c) && (!success_d) )
  {
    *success = FALSE;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  if (success_a)
  {
    res = soc_sand_mem_read(
            unit,
            ack,
            SOC_PETRA_REG_DB_ACC(regs->fdr.cpu_data_cell_a_reg[0].addr.base),
            SOC_SAND_DATA_CELL_BYTE_SIZE,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (success_b)
  {
    res = soc_sand_mem_read(
            unit,
            ack,
            SOC_PETRA_REG_DB_ACC(regs->fdr.cpu_data_cell_b_reg[0].addr.base),
            SOC_SAND_DATA_CELL_BYTE_SIZE,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (success_c)
  {
    res = soc_sand_mem_read(
            unit,
            ack,
            SOC_PETRA_REG_DB_ACC(regs->fdr.cpu_data_cell_c_reg[0].addr.base),
            SOC_SAND_DATA_CELL_BYTE_SIZE,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (success_d)
  {
    res = soc_sand_mem_read(
            unit,
            ack,
            SOC_PETRA_REG_DB_ACC(regs->fdr.cpu_data_cell_d_reg[0].addr.base),
            SOC_SAND_DATA_CELL_BYTE_SIZE,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  /*
   *	No Check the received cell is an ACK (only inband)
   */
  *success = TRUE;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_cell_ack_get()",0,0);
}

STATIC uint32
  soc_petra_sr_rcv_cell(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        is_fe1600,
    SOC_SAND_OUT SOC_SAND_DATA_CELL                   *data_cell,
    SOC_SAND_OUT SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600 *added_data,
    SOC_SAND_OUT uint8                        *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    packed_cpu_data_cell_rcv[SOC_SAND_DATA_CELL_UINT32_SIZE] ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SR_RCV_CELL);

  SOC_SAND_CHECK_NULL_INPUT(data_cell);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = SOC_SAND_OK; sal_memset(packed_cpu_data_cell_rcv, 0x0, SOC_SAND_DATA_CELL_UINT32_SIZE * sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   *	Get the ack
   */
  res = soc_petra_cell_ack_get(
          unit,
          packed_cpu_data_cell_rcv,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Parsing to a source-routed cell
   */
  if (is_fe1600)
  {
    res = soc_petra_sr_fe1600_buffer_to_data_cell(
          packed_cpu_data_cell_rcv,          
          data_cell,
          added_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } 
  else
  {
    res = soc_sand_buffer_to_data_cell(
          packed_cpu_data_cell_rcv,
          0,
          data_cell
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } 

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_rcv_cell()",0,0);
}

uint32
  soc_petra_transaction_with_fe1600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  soc_reg_t                       reg,
    SOC_SAND_IN  soc_mem_t                       mem,
    SOC_SAND_IN  int32                        port_or_copyno,
    SOC_SAND_IN  int32                        index,
    SOC_SAND_IN  uint32                        *data_in,
    SOC_SAND_IN  uint32                        size, /* In Bytes*/
    SOC_SAND_IN  uint8                       is_write,
    SOC_SAND_IN  uint8                       is_inband,
    SOC_SAND_IN  uint8                       is_memory,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint8
    success = FALSE;
  uint32
    i;
  uint32
    start_long = 0 ,
    nof_words = 0 ,    
    res = SOC_SAND_OK ;
  SOC_SAND_DATA_CELL
    data_cell_sent,
    data_cell_rcv;
  SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600
    added_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK; sal_memset(&data_cell_sent, 0x0, sizeof(SOC_SAND_DATA_CELL));
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  res = SOC_SAND_OK; sal_memset(&data_cell_rcv, 0x0, sizeof(SOC_SAND_DATA_CELL));
  SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);
  nof_words = size;
  
  if (is_inband)
  {
    /*
     * Construction of the cell information
     */
    res = soc_petra_build_data_cell_for_fe1600(
            unit,
            sr_link_list,
            reg,
            mem,
            port_or_copyno,
            index,
            data_in + start_long,
            nof_words,
            is_write,
            is_memory,
            is_inband,
            &data_cell_sent,
            &added_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*
     * Sending and receiving the cell
     */
    res = soc_petra_sr_send_and_wait_ack(
            unit,
            &data_cell_sent,
            TRUE,
            &data_cell_rcv ,
            &added_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    if (
        data_cell_sent.data_cell.source_routed.cell_ident
        != data_cell_rcv.data_cell.source_routed.cell_ident
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_DIFFERENT_CELL_IDENT_ERR, 153, exit);
    }

  }
  else if (!is_write) /* Read for a regular source routed cell*/
  {
     res = soc_petra_sr_rcv_cell(
            unit,
            TRUE,
            &data_cell_rcv,
            &added_data,
            &success            
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);

    if (success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_NO_RECEIVED_CELL_ERR, 155, exit);
    }


  }
  else /* Write for a regular source routed cell*/
  {
    /*
     * Construction of the cell information
     */
    res = soc_petra_build_data_cell_for_fe1600(
            unit,
            sr_link_list,
            reg,
            mem,
            port_or_copyno,
            index,
            data_in + start_long,
            nof_words,
            is_write,
            is_memory,
            is_inband,
            &data_cell_sent,
            &added_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);

    res = soc_petra_sr_send_cell(
            unit,
            TRUE,
            &data_cell_sent,
            &added_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 158, exit);
  }

  /*
   * Construction of the output in case of read
   */
  if (is_inband)
  {
    if(!is_write) /*read*/
    {
      for (i=0; i < size; i++)
      {
        data_out[i] = data_cell_rcv.data_cell.source_routed.data_wr_cell[i];
      }        
    }
  }
  else if (!is_write) /* Regular source routed cell for read, copy of the payload */
  {
    soc_sand_os_memcpy(data_out,added_data.cell_data, size);
  }    
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_transaction_with_fe1600()",0,0);
}



/*
 * Builds the data, packs to a buffer, sends the cell,
 * receives the buffer, and decomposes it to the output.
 */
STATIC uint32
  soc_petra_transaction_with_fe600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in,
    SOC_SAND_IN  uint32                        size,/* In Bytes */
    SOC_SAND_IN  uint8                       is_write,
    SOC_SAND_IN  uint8                       is_indirect,
    SOC_SAND_IN  uint8                       is_inband,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint8
    success = FALSE;
  uint32
    iter = 0 ,
    iter_size = 0 ;
  uint32
    start_long = 0 ,
    nof_words = 0 ,
    tmp_field_for_data_cell_add_fields = 0,
    tmp_data_out[SOC_PETRA_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL] = {0},
    tmp_not_inband_data_out[SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S] = {0},
    res = SOC_SAND_OK ;
  SOC_SAND_DATA_CELL
    data_cell_sent,
    data_cell_rcv ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TRANSACTION_WITH_FE600);

  /*
   * Sending the necessary number of cells according to the size
   */
  while (iter_size < size)
  {
    res = SOC_SAND_OK; sal_memset(&data_cell_sent, 0x0, sizeof(SOC_SAND_DATA_CELL));
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    res = SOC_SAND_OK; sal_memset(&data_cell_rcv, 0x0, sizeof(SOC_SAND_DATA_CELL));
    SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);
    /*
     * Number of words to copy
     */
    if (is_inband)
      {
      if( ( !is_write ) && ( !is_indirect ) ) /* Read direct: one word per cell, i.e. the offset*/
      {
        nof_words = 1  ;
      }
      else if( ( is_write ) && ( !is_indirect ) ) /* Write direct: five words per cell */
      {
        nof_words = SOC_SAND_MIN(SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD , (size - iter_size) / SOC_PETRA_CELL_NOF_BYTES_IN_UINT32 );
      }
      else if( ( !is_write ) && ( is_indirect ) ) /* Indirect Read: three words of data_in per cell */
      {
        nof_words = SOC_PETRA_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL;
      }
      else /* Indirect Write: three words of data_in per cell */
      {
        nof_words = SOC_SAND_MIN(SOC_PETRA_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL , (size - iter_size) / SOC_PETRA_CELL_NOF_BYTES_IN_UINT32 );
      }
    }
    else /* Simple source routed cell */
    {
      nof_words = 1;
    }
    if (is_inband)
    {
      /*
       * Construction of the cell information
       */
      res = soc_petra_build_data_cell_for_fe600(
              unit,
              sr_link_list,
              offset,
              data_in + start_long,
              nof_words,
              is_write,
              is_indirect,
              is_inband,
              &data_cell_sent
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      /*
       * Sending and receiving the cell
       */
      res = soc_petra_sr_send_and_wait_ack(
              unit,
              &data_cell_sent,
              FALSE,
              &data_cell_rcv,
              NULL
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

      if (
          data_cell_sent.data_cell.source_routed.cell_ident
          != data_cell_rcv.data_cell.source_routed.cell_ident
         )
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_DIFFERENT_CELL_IDENT_ERR, 153, exit);
      }

    }
    else if (!is_write) /* Read for a regular source routed cell*/
    {
       res = soc_petra_sr_rcv_cell(
              unit,
              FALSE,
              &data_cell_rcv,
              NULL,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);

      if (success == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_NO_RECEIVED_CELL_ERR, 155, exit);
      }


    }
    else /* Write for a regular source routed cell*/
    {
      /*
       * Construction of the cell information
       */
      res = soc_petra_build_data_cell_for_fe600(
              unit,
              sr_link_list,
              offset,
              data_in + start_long,
              nof_words,
              is_write,
              is_indirect,
              is_inband,
              &data_cell_sent
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);

      res = soc_petra_sr_send_cell(
              unit,
              FALSE,
              &data_cell_sent,
              NULL
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 158, exit);
    }

    /*
     * Construction of the output in case of read
     */
    if (is_inband)
    {
      if(!is_write)
      {
        if(is_indirect) /* Indirect read */
        {
          if(start_long != 0)
          {
            SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_WRITE_OUT_OF_BOUNDARY,160,exit);
          }

          tmp_data_out[start_long] =
            data_cell_rcv.data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_0] ;
          tmp_data_out[start_long + 1] =
            data_cell_rcv.data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_1] ;
          tmp_data_out[start_long + 2] =
            data_cell_rcv.data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_2] ;
        }
        else /* Direct read */
        {
          if(start_long >= SOC_PETRA_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL)
          {
            SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_WRITE_OUT_OF_BOUNDARY,162,exit);
          }

          tmp_data_out[start_long] = data_cell_rcv.data_cell.source_routed.data_wr_cell[SOC_PETRA_CELL_WRITE_POSITION_0] ;
        }
      }
    }
    else if (!is_write) /* Regular source routed cell for read, copy of the payload */
    {
      if (start_long > SOC_PETRA_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_WRITE_OUT_OF_BOUNDARY,162,exit);
      }
      
      for (iter = 0 ; iter < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
      {
        tmp_not_inband_data_out[start_long + iter] = data_cell_rcv.data_cell.source_routed.data_wr_cell[iter] ;
      }

      for (iter = 0 ; iter < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
      {
        tmp_field_for_data_cell_add_fields = data_cell_rcv.data_cell.source_routed.add_wr_cell[iter];

        res = soc_sand_bitstream_set_any_field(
                &(tmp_field_for_data_cell_add_fields),
                SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH * iter,
                SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
                &(tmp_not_inband_data_out[start_long + SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD])
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
      }

      tmp_field_for_data_cell_add_fields = data_cell_rcv.data_cell.source_routed.cell_ident;

      res = soc_sand_bitstream_set_any_field(
              &(tmp_field_for_data_cell_add_fields),
              SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH * SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD,
              SOC_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_LENGTH,
              &(tmp_not_inband_data_out[start_long + SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

    }

    if (is_inband)
    {
      if( ( !is_write ) && ( !is_indirect ) ) /* Read direct: one word per cell*/
      {
        ++start_long  ;
        iter_size = iter_size + SOC_PETRA_CELL_NOF_BYTES_IN_UINT32 ;
      }
      else if( ( is_write ) && ( !is_indirect ) ) /* Write direct: five words per cell */
      {
        start_long = start_long + SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ;
        iter_size = iter_size + SOC_PETRA_CELL_NOF_BYTES_IN_UINT32 * SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ;
      }
      else /* Indirect R/W: three words of data_in per cell */
      {
        start_long = start_long + SOC_PETRA_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL ;
        iter_size = iter_size + SOC_PETRA_CELL_NOF_BYTES_IN_UINT32 * SOC_PETRA_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL ;
      }
    }
    else
    {
      start_long += SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S;
      iter_size = iter_size + SOC_PETRA_CELL_NOF_BYTES_IN_UINT32 * SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S ;
    }
  }
  /*
   * Construction of the data_out in case of reading
   */
  if(
     (!is_write)
     && (is_inband)
    )
  {
    for (iter = 0 ; iter < start_long ; ++iter)
    {
      data_out[iter] = tmp_data_out[iter];
    }
  }
  else if(
          (!is_inband)
          && (!is_write)
         )
  {
    for (iter = 0 ; iter < start_long ; ++iter)
    {
      data_out[iter] = tmp_not_inband_data_out[iter];
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_transaction_with_fe600()",0,0);


}

/*
 * Allows the cpu to generate a direct read command
 */
uint32
  soc_petra_read_from_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_READ_FROM_FE600_UNSAFE);

  /*
   * Builds the data, packs to a buffer, sends the cell,
   * receives the buffer, and decomposes it to the output.
   */
  res = soc_petra_transaction_with_fe600(
          unit,
          sr_link_list,
          offset,
          NULL,
          size,
          0,
          0,
          1,
          data_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_read_from_fe600_unsafe()",0,0);
}
/*
 * Allows the cpu to generate a direct write command
 */
uint32
  soc_petra_write_to_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST        *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_WRITE_FROM_FE600_UNSAFE);

  /*
   * Construction of the read direct cell information
   */
  /*
   * Builds the data, packs to a buffer, sends the cell,
   * receives the buffer, and decomposes it to the output.
   */
  res = soc_petra_transaction_with_fe600(
          unit,
          sr_link_list,
          offset,
          data_in,
          size,
          1,
          0,
          1,
          NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_write_to_fe600_unsafe()",0,0);
}


/*
 * Allows the cpu to generate a indirect read command
 */
uint32
  soc_petra_indirect_read_from_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST          *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INDIRECT_READ_FROM_FE600_UNSAFE);

  /*
   * Builds the data cell, packs to a buffer, sends the cell,
   * receives the buffer, and decomposes it to the output.
   */
  res = soc_petra_transaction_with_fe600(
          unit,
          sr_link_list,
          offset,
          NULL,
          size ,
          0,
          1,
          1,
          data_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_indirect_read_from_fe600_unsafe()",0,0);

}


/*
 * Allows the cpu to generate a indirect write command
 */
uint32
  soc_petra_indirect_write_to_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in
  )
{
  uint32
    res = SOC_SAND_OK ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INDIRECT_WRITE_FROM_FE600_UNSAFE);

  /*
   * Builds the data cell, packs to a buffer, sends the cell,
   * receives the buffer, and decomposes it to the output.
   */
  res = soc_petra_transaction_with_fe600(
          unit,
          sr_link_list,
          offset,
          data_in,
          size ,
          1,
          1,
          1,
          NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_indirect_write_fe600_unsafe()",0,0);

}

/*
 * Allows the cpu to generate an interaction with a CPU of a SOC_SAND_FE600
 */
uint32
  soc_petra_cpu2cpu_write_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST          *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        *data_in
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CPU2CPU_WITH_FE600_WRITE_UNSAFE);

  /*
   * Builds the data cell, packs to a buffer, sends the cell,
   * receives the buffer, and decomposes it to the output.
   */
  res = soc_petra_transaction_with_fe600(
          unit,
          sr_link_list,
          0,
          data_in,
          size,
          1,
          0,
          0,
          NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cpu2cpu_write_unsafe()",0,0);

}


/*
 * Allows the cpu to generate an interaction with a CPU of a SOC_SAND_FE600
 */
uint32
  soc_petra_cpu2cpu_read_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CPU2CPU_WITH_FE600_READ_UNSAFE);

  /*
   * Builds the data cell, packs to a buffer, sends the cell,
   * receives the buffer, and decomposes it to the output.
   */
  res = soc_petra_transaction_with_fe600(
          unit,
          NULL,
          0,
          NULL,
          SOC_SAND_DATA_CELL_PAYLOAD_IN_BYTES,
          0,
          0,
          0,
          data_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cpu2cpu_read_unsafe()",0,0);

}


/*
 * Sends a cell and waits for an ack
 */
uint32
  soc_petra_sr_send_and_wait_ack(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_DATA_CELL                   *data_cell_sent,
    SOC_SAND_IN  uint8                        is_fe1600,
    SOC_SAND_OUT SOC_SAND_DATA_CELL                   *data_cell_rcv,
    SOC_SAND_OUT SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600 *added_data
  )
{
  uint8
    success = FALSE;
  uint32
    try_i = 0 ;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SR_SEND_AND_WAIT_ACK);

  res = soc_petra_sr_send_cell(
          unit,
          is_fe1600,
          data_cell_sent,
          added_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  do
  {
    res = soc_petra_sr_rcv_cell(
            unit,
            is_fe1600,
            data_cell_rcv, 
            added_data,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  } while((success == FALSE) && (try_i++ < SOC_PETRA_CELL_MAX_NOF_TRIES_WAITING_FOR_ACK));

  if (try_i >= SOC_PETRA_CELL_MAX_NOF_TRIES_WAITING_FOR_ACK)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_NO_RECEIVED_CELL_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_send_and_wait_ack()",0,0);
}

STATIC
  uint32
    soc_petra_cell_mc_tbl_inband_build(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                                 mc_id_ndx,
      SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info,
      SOC_SAND_IN  uint8                                 is_write
    )
{
  uint32
    soc_petra_cell_mc_buffer[SOC_SAND_DATA_CELL_UINT32_SIZE],
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CELL_MC_TBL_INBAND_BUILD);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(info);

  if (is_write)
  {
   SOC_PETRA_COPY(soc_petra_cell_mc_buffer, Soc_petra_cell_mc_buffer_write[unit], uint32, SOC_SAND_DATA_CELL_UINT32_SIZE);
  }
  else
  {
    SOC_PETRA_COPY(soc_petra_cell_mc_buffer, Soc_petra_cell_mc_buffer_read[unit], uint32, SOC_SAND_DATA_CELL_UINT32_SIZE);
  }

  if (info->filter[SOC_PETRA_CELL_MC_FILTER_MC_ID])
  {
    /*
     *	Modify the bits of the indirect address:
     *  Bits: 31 [3], 30:0 [2]
     */
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_ENTRY_REG1] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_ENTRY_REG1_MSB, SOC_PETRA_CELL_ENTRY_REG1_LSB);
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_ENTRY_REG1] |=
      SOC_SAND_SET_FLD_IN_PLACE((mc_id_ndx>>SOC_PETRA_CELL_ENTRY_REG2_LENGTH), 0,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_ENTRY_REG1_MSB, SOC_PETRA_CELL_ENTRY_REG1_LSB));
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_ENTRY_REG2] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_ENTRY_REG2_MSB, SOC_PETRA_CELL_ENTRY_REG2_LSB);
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_ENTRY_REG2] |=
      SOC_SAND_SET_FLD_IN_PLACE(mc_id_ndx, SOC_PETRA_CELL_ENTRY_REG2_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_ENTRY_REG2_MSB, SOC_PETRA_CELL_ENTRY_REG2_LSB));
  }

  if (info->filter[SOC_PETRA_CELL_MC_FILTER_FE_LOC])
  {
    /*
     *	Modify the bits of the destination type:
     *  Bits: 15:13 [0], for FE2 011(3), for FE1 110(6)
     */
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_FE_LOC_REG] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_FE_LOC_REG_MSB, SOC_PETRA_CELL_FE_LOC_REG_LSB);
    switch(info->fe_location)
    {
    case SOC_PETRA_CELL_FE_LOCATION_FE1:
      fld_val = 6;
      break;

    case SOC_PETRA_CELL_FE_LOCATION_FE2:
      fld_val = 3;
      break;
    
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_FE_LOCATION_OUT_OF_RANGE_ERR, 10, exit);
      break;
    }
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_FE_LOC_REG] |=
      SOC_SAND_SET_FLD_IN_PLACE(fld_val, SOC_PETRA_CELL_FE_LOC_REG_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_FE_LOC_REG_MSB, SOC_PETRA_CELL_FE_LOC_REG_LSB));
  }

  if (info->filter[SOC_PETRA_CELL_MC_FILTER_PATH_LINKS0])
  {
    /*
     *	Modify the bits of the output link:
     *  Bits: 12:8 [0], and 23 [1] (if link > 31)
     */
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_PATH0_REG1] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_PATH0_REG1_MSB, SOC_PETRA_CELL_PATH0_REG1_LSB);
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_PATH0_REG1] |=
      SOC_SAND_SET_FLD_IN_PLACE(info->path_links[0], SOC_PETRA_CELL_PATH0_REG1_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_PATH0_REG1_MSB, SOC_PETRA_CELL_PATH0_REG1_LSB));
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_PATH0_REG2] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_PATH0_REG2_MSB, SOC_PETRA_CELL_PATH0_REG2_LSB);
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_PATH0_REG2] |=
      SOC_SAND_SET_FLD_IN_PLACE((info->path_links[0] > SOC_PETRA_CELL_PATH_REG2_TH), SOC_PETRA_CELL_PATH0_REG2_LSB,
        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_PATH0_REG2_MSB, SOC_PETRA_CELL_PATH0_REG2_LSB));

    /*
     *	Modify the output link register
     */
    fld_val = info->path_links[0];
    SOC_PETRA_FLD_SET(regs->fdt.transmit_cell_output_link_number_reg.cpu_link_num, fld_val, 10, exit);
  }

  if (info->filter[SOC_PETRA_CELL_MC_FILTER_PATH_LINKS1])
  {
    /*
     *	Modify the bits of the fe1 output link:
     *  Bits: 7:3 [0], and 22 [1] (if link > 31)
     */
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_PATH1_REG1] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_PATH1_REG1_MSB, SOC_PETRA_CELL_PATH1_REG1_LSB);
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_PATH1_REG1] |=
      SOC_SAND_SET_FLD_IN_PLACE(info->path_links[1], SOC_PETRA_CELL_PATH1_REG1_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_PATH1_REG1_MSB, SOC_PETRA_CELL_PATH1_REG1_LSB));
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_PATH1_REG2] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_PATH1_REG2_MSB, SOC_PETRA_CELL_PATH1_REG2_LSB);
    soc_petra_cell_mc_buffer[SOC_PETRA_CELL_PATH1_REG2] |=
      SOC_SAND_SET_FLD_IN_PLACE((info->path_links[1] > SOC_PETRA_CELL_PATH_REG2_TH), SOC_PETRA_CELL_PATH1_REG2_LSB,
                          SOC_SAND_BITS_MASK(SOC_PETRA_CELL_PATH1_REG2_MSB, SOC_PETRA_CELL_PATH1_REG2_LSB));
  }

  /*
   *	Write the data
   */
   res = soc_sand_mem_write(
           unit,
           soc_petra_cell_mc_buffer,
           SOC_PETRA_CELL_INBAND_CELL_REG  * sizeof(uint32),
           sizeof(soc_petra_cell_mc_buffer),
           FALSE
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   *	Write the trigger
   */
   SOC_PETRA_FLD_SET(regs->fdt.transmit_data_cell_trigger_reg.cpu_trg, 0x1, 50, exit);

   if (is_write)
   {
     SOC_PETRA_COPY(Soc_petra_cell_mc_buffer_write[unit], soc_petra_cell_mc_buffer, uint32, SOC_SAND_DATA_CELL_UINT32_SIZE);
   }
   else
   {
     SOC_PETRA_COPY(Soc_petra_cell_mc_buffer_read[unit], soc_petra_cell_mc_buffer, uint32, SOC_SAND_DATA_CELL_UINT32_SIZE);
   }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_cell_mc_tbl_write_unsafe()",mc_id_ndx,0);
}



/*********************************************************************
*     Configure an entry of the SOC_SAND_FE600 multicast tables via
 *     inband indirect write cells.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_cell_mc_tbl_write_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data
  )
{
  uint32
    reg_val = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CELL_MC_TBL_WRITE_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);


  if (info->filter[SOC_PETRA_CELL_MC_FILTER_DATA0])
  {
    /*
     *	Modify the bits of the first data:
     *  Bits: 13:0 [3], and 31:14 [4]
     */
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA0_REG1] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_DATA0_REG1_MSB, SOC_PETRA_CELL_DATA0_REG1_LSB);
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA0_REG1] |=
      SOC_SAND_SET_FLD_IN_PLACE(tbl_data->data[0] >> SOC_PETRA_CELL_DATA0_REG2_LENGTH, SOC_PETRA_CELL_DATA0_REG1_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_DATA0_REG1_MSB, SOC_PETRA_CELL_DATA0_REG1_LSB));
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA0_REG2] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_DATA0_REG2_MSB, SOC_PETRA_CELL_DATA0_REG2_LSB);
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA0_REG2] |=
      SOC_SAND_SET_FLD_IN_PLACE(tbl_data->data[0], SOC_PETRA_CELL_DATA0_REG2_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_DATA0_REG2_MSB, SOC_PETRA_CELL_DATA0_REG2_LSB));
  }

  if (info->filter[SOC_PETRA_CELL_MC_FILTER_DATA1])
  {
    /*
     *	Modify the bits of the second data:
     *  Bits: 28:0 [5], and 31:29 [6]
     */
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA1_REG1] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_DATA1_REG1_MSB, SOC_PETRA_CELL_DATA1_REG1_LSB);
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA1_REG1] |=
      SOC_SAND_SET_FLD_IN_PLACE(tbl_data->data[1] >> SOC_PETRA_CELL_DATA1_REG2_LENGTH, SOC_PETRA_CELL_DATA1_REG1_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_DATA1_REG1_MSB, SOC_PETRA_CELL_DATA1_REG1_LSB));
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA1_REG2] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_DATA1_REG2_MSB, SOC_PETRA_CELL_DATA1_REG2_LSB);
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA1_REG2] |=
      SOC_SAND_SET_FLD_IN_PLACE(tbl_data->data[1], SOC_PETRA_CELL_DATA1_REG2_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_DATA1_REG2_MSB, SOC_PETRA_CELL_DATA1_REG2_LSB));
  }

  if (info->filter[SOC_PETRA_CELL_MC_FILTER_DATA2])
  {
    /*
     *	Modify the bits of the third data:
     *  Bits: 11:0 [6], and 31:12 [7]
     */
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA2_REG1] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_DATA2_REG1_MSB, SOC_PETRA_CELL_DATA2_REG1_LSB);
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA2_REG1] |=
      SOC_SAND_SET_FLD_IN_PLACE(tbl_data->data[2] >> SOC_PETRA_CELL_DATA2_REG2_LENGTH, SOC_PETRA_CELL_DATA2_REG1_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_DATA2_REG1_MSB, SOC_PETRA_CELL_DATA2_REG1_LSB));
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA2_REG2] &=
      SOC_SAND_ZERO_BITS_MASK(SOC_PETRA_CELL_DATA2_REG2_MSB, SOC_PETRA_CELL_DATA2_REG2_LSB);
    Soc_petra_cell_mc_buffer_write[unit][SOC_PETRA_CELL_DATA2_REG2] |=
      SOC_SAND_SET_FLD_IN_PLACE(tbl_data->data[2], SOC_PETRA_CELL_DATA2_REG2_LSB,
                        SOC_SAND_BITS_MASK(SOC_PETRA_CELL_DATA2_REG2_MSB, SOC_PETRA_CELL_DATA2_REG2_LSB));
  }

  /*
   *	Build all the cell and send it
   */
  res = soc_petra_cell_mc_tbl_inband_build(
          unit,
          mc_id_ndx,
          info,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#if SOC_PETRA_CELL_MC_TBL_WRITE_ACK_READ
   sal_msleep(SOC_PETRA_CELL_MC_TBL_WRITE_ACK_WAIT_IN_MS);
  
   /*
   *	Read part of the Ack of the inband cell and empty the FIFO
   */
   SOC_PETRA_REG_GET(regs->fdr.cpu_data_cell_a_reg[SOC_PETRA_CELL_ACK_REG], reg_val, 60, exit);
   SOC_PETRA_REG_GET(regs->fdr.cpu_data_cell_b_reg[SOC_PETRA_CELL_ACK_REG], reg_val, 70, exit);
   SOC_PETRA_REG_GET(regs->fdr.cpu_data_cell_c_reg[SOC_PETRA_CELL_ACK_REG], reg_val, 80, exit);
   SOC_PETRA_REG_GET(regs->fdr.cpu_data_cell_d_reg[SOC_PETRA_CELL_ACK_REG], reg_val, 90, exit);
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_cell_mc_tbl_write_unsafe()",mc_id_ndx,0);
}

uint32
  soc_petra_cell_mc_tbl_write_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CELL_MC_TBL_WRITE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mc_id_ndx, SOC_PETRA_MC_ID_NDX_MAX, SOC_PETRA_MC_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  soc_petra_PETRA_CELL_MC_TBL_INFO_verify(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  soc_petra_PETRA_CELL_MC_TBL_DATA_verify(tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_cell_mc_tbl_write_verify()",mc_id_ndx,0);
}

/*********************************************************************
*     Get an entry of the SOC_SAND_FE600 multicast tables via an inband
 *     indirect read cell.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_cell_mc_tbl_read_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info,
    SOC_SAND_OUT SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data
  )
{
  uint32
    ack[SOC_SAND_DATA_CELL_UINT32_SIZE],
    reg_val = 0,
    try_i = 0,
    res = SOC_SAND_OK;
  uint8
    success = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CELL_MC_TBL_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  res = SOC_SAND_OK; sal_memset(ack, 0x0, SOC_SAND_DATA_CELL_UINT32_SIZE * sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Build all the cell and send it
   */
  res = soc_petra_cell_mc_tbl_inband_build(
          unit,
          mc_id_ndx,
          info,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   *	Get the Ack
   */
  do
  {
    res = soc_petra_cell_ack_get(
            unit,
            ack,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  } while((success == FALSE) && (try_i++ < SOC_PETRA_CELL_MC_TBL_MAX_NOF_TRIES_FOR_ACK));

  if (try_i >= SOC_PETRA_CELL_MC_TBL_MAX_NOF_TRIES_FOR_ACK)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_NO_RECEIVED_CELL_ERR, 30, exit);
  }

  /*
   *	Get the data from the ACK
   */
  reg_val = 0;
  reg_val += SOC_SAND_GET_FLD_FROM_PLACE(
              ack[SOC_PETRA_CELL_ACK_DATA0_REG1], SOC_PETRA_CELL_ACK_DATA0_REG1_LSB,
              SOC_SAND_BITS_MASK(SOC_PETRA_CELL_ACK_DATA0_REG1_MSB, SOC_PETRA_CELL_ACK_DATA0_REG1_LSB)
             ) << SOC_PETRA_CELL_ACK_DATA0_REG2_LENGTH;
  reg_val += SOC_SAND_GET_FLD_FROM_PLACE(
              ack[SOC_PETRA_CELL_ACK_DATA0_REG2],
              SOC_PETRA_CELL_ACK_DATA0_REG2_LSB,
              SOC_SAND_BITS_MASK(SOC_PETRA_CELL_ACK_DATA0_REG2_MSB, SOC_PETRA_CELL_ACK_DATA0_REG2_LSB)
             );
  tbl_data->data[0] = reg_val;

  reg_val = 0;
  reg_val += SOC_SAND_GET_FLD_FROM_PLACE(
              ack[SOC_PETRA_CELL_ACK_DATA1_REG1], SOC_PETRA_CELL_ACK_DATA1_REG1_LSB,
              SOC_SAND_BITS_MASK(SOC_PETRA_CELL_ACK_DATA1_REG1_MSB, SOC_PETRA_CELL_ACK_DATA1_REG1_LSB)
             ) << SOC_PETRA_CELL_ACK_DATA1_REG2_LENGTH;
  reg_val += SOC_SAND_GET_FLD_FROM_PLACE(
              ack[SOC_PETRA_CELL_ACK_DATA1_REG2],
              SOC_PETRA_CELL_ACK_DATA1_REG2_LSB,
              SOC_SAND_BITS_MASK(SOC_PETRA_CELL_ACK_DATA1_REG2_MSB, SOC_PETRA_CELL_ACK_DATA1_REG2_LSB)
             );
  tbl_data->data[1] = reg_val;

  reg_val = 0;
  reg_val += SOC_SAND_GET_FLD_FROM_PLACE(
              ack[SOC_PETRA_CELL_ACK_DATA2_REG1],
              SOC_PETRA_CELL_ACK_DATA2_REG1_LSB,
              SOC_SAND_BITS_MASK(SOC_PETRA_CELL_ACK_DATA2_REG1_MSB, SOC_PETRA_CELL_ACK_DATA2_REG1_LSB)
             ) << SOC_PETRA_CELL_ACK_DATA2_REG2_LENGTH;
  reg_val += SOC_SAND_GET_FLD_FROM_PLACE(
              ack[SOC_PETRA_CELL_ACK_DATA2_REG2],
              SOC_PETRA_CELL_ACK_DATA2_REG2_LSB,
              SOC_SAND_BITS_MASK(SOC_PETRA_CELL_ACK_DATA2_REG2_MSB, SOC_PETRA_CELL_ACK_DATA2_REG2_LSB)
             );
  tbl_data->data[2] = reg_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_cell_mc_tbl_read_unsafe()",mc_id_ndx,0);
}

uint32
  soc_petra_cell_mc_tbl_read_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CELL_MC_TBL_READ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mc_id_ndx, SOC_PETRA_MC_ID_NDX_MAX, SOC_PETRA_MC_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  soc_petra_PETRA_CELL_MC_TBL_INFO_verify(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_cell_mc_tbl_read_verify()",mc_id_ndx,0);
}


uint32
  soc_petra_PETRA_CELL_MC_TBL_DATA_verify(
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_PETRA_CELL_MC_TBL_DATA_verify()",0,0);
}

uint32
  soc_petra_PETRA_CELL_MC_TBL_INFO_verify(
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fe_location, SOC_PETRA_CELL_NOF_FE_LOCATIONS-1, SOC_PETRA_CELL_FE_LOCATION_OUT_OF_RANGE_ERR, 10, exit);
  for (ind = 0; ind < SOC_PETRA_CELL_MC_NOF_LINKS; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->path_links[ind], 63, SOC_PETRA_CELL_PATH_LINKS_OUT_OF_RANGE_ERR, 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_PETRA_CELL_MC_TBL_INFO_verify()",0,0);
}
#if SOC_PETRA_DEBUG

#endif /* SOC_PETRA_DEBUG */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

