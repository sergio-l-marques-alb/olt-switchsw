/*
* $Id: vsc256_fabric_cell.c,v 1.7 Broadcom SDK $
*
* $Copyright: (c) 2017 Broadcom.
* Broadcom Proprietary and Confidential. All rights reserved.$
*
* SOC VSC256 FABRIC CELL
*/

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/legacy/error.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnxc/legacy/dnxc_memregs.h>
#include <soc/dnxc/legacy/dnxc_error.h>
#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnxc/legacy/dnxc_fabric_source_routed_cell.h>
#include <shared/bitop.h>

#include <soc/dnxc/legacy/vsc256_fabric_cell.h>

shr_error_e 
soc_dnxc_vsc256_build_payload(int unit, const dnxc_vsc256_sr_cell_t* cell, int buf_size_bytes, uint32* buf, int offset)
/* the offset should be 0 in fe1600 and 512 in jer2_arad */ 
{
  uint32 tmp_u32_sr_extra_field;
  int i;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

  if(DNXC_VSC256_INBAND_PAYLOAD_CELL_FORMAT_START + offset + DNXC_VSC256_INBAND_PAYLOAD_CELL_FORMAT_LENGTH > 8*buf_size_bytes) {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Buffer size is not large enough");
  }
  /*cell format*/
  tmp_u32_sr_extra_field = cell->payload.inband.cell_format;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_INBAND_PAYLOAD_CELL_FORMAT_START + offset, &tmp_u32_sr_extra_field, 0, DNXC_VSC256_INBAND_PAYLOAD_CELL_FORMAT_LENGTH);

  /*cell id*/
  tmp_u32_sr_extra_field = cell->payload.inband.cell_id;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_INBAND_PAYLOAD_CELL_ID_START + offset, &tmp_u32_sr_extra_field, 0, DNXC_VSC256_INBAND_PAYLOAD_CELL_ID_LENGTH);

  /*cell seq*/
  tmp_u32_sr_extra_field =  cell->payload.inband.seq_num;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_INBAND_PAYLOAD_SEQUENCE_NUMBER_START + offset, &tmp_u32_sr_extra_field, 0, DNXC_VSC256_INBAND_PAYLOAD_SEQUENCE_NUMBER_LENGTH);

  /*num of commands*/
  tmp_u32_sr_extra_field = cell->payload.inband.nof_commands;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_INBAND_PAYLOAD_NUMBER_OF_COMMANDS_START + offset, &tmp_u32_sr_extra_field, 0, DNXC_VSC256_INBAND_PAYLOAD_NUMBER_OF_COMMANDS_LENGTH);

  for(i=0 ; i<cell->payload.inband.nof_commands ; i++) {
    /*command type*/
    switch(cell->payload.inband.commands[i].opcode) {
            case soc_dnxc_inband_reg_read:  tmp_u32_sr_extra_field = 2; break;
            case soc_dnxc_inband_reg_write: tmp_u32_sr_extra_field = 3; break;
            case soc_dnxc_inband_mem_read:  tmp_u32_sr_extra_field = 0; break;
            case soc_dnxc_inband_mem_write: tmp_u32_sr_extra_field = 1; break;
            default: SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported opcode"); 
    }
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_INBAND_PAYLOAD_OPCODE_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF + offset, &tmp_u32_sr_extra_field, 0, DNXC_VSC256_INBAND_PAYLOAD_OPCODE_LENGTH);

    /*block*/
    tmp_u32_sr_extra_field = cell->payload.inband.commands[i].schan_block;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_INBAND_PAYLOAD_BLOCK_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF + offset, &tmp_u32_sr_extra_field, 0, DNXC_VSC256_INBAND_PAYLOAD_BLOCK_LENGTH);

    /*data size*/
    if(cell->payload.inband.commands[i].length <= 4) {
      tmp_u32_sr_extra_field = 0; /*32 bit*/
    } else if(cell->payload.inband.commands[i].length <= 8) {
      tmp_u32_sr_extra_field = 1; /*64 bit*/
    } else if(cell->payload.inband.commands[i].length <= 12) {
      tmp_u32_sr_extra_field = 2; /*96 bit*/
    } else if(cell->payload.inband.commands[i].length <= 16) {
      tmp_u32_sr_extra_field = 3; /*128 bit*/
    } else {
      SHR_ERR_EXIT(_SHR_E_PARAM, "Can't read more than 128 bits data"); 
    }
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_INBAND_PAYLOAD_LENGTH_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF + offset, &tmp_u32_sr_extra_field, 0, DNXC_VSC256_INBAND_PAYLOAD_LENGTH_LENGTH);

    /*data*/
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_INBAND_PAYLOAD_DATA_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF + 
      ((3 - tmp_u32_sr_extra_field) * 32) /* MSB allignment*/ 
      + offset, cell->payload.inband.commands[i].data, 0, ((tmp_u32_sr_extra_field + 1) * 32));

    /*offset*/
    tmp_u32_sr_extra_field  = cell->payload.inband.commands[i].offset;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_INBAND_PAYLOAD_ADDRESS_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF + offset, &tmp_u32_sr_extra_field, 0, DNXC_VSC256_INBAND_PAYLOAD_ADDRESS_LENGTH);

    
  }

exit:
  SHR_FUNC_EXIT; 
}

shr_error_e 
soc_dnxc_vsc256_parse_payload(int unit, const uint32* buf, dnxc_vsc256_sr_cell_t* cell, int offset)
{
  uint32 val = 0;
  int i;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

  sal_memset(&cell->payload.inband, 0, sizeof(dnxc_vsc256_inband_payload_t));/* Cleaning the data */

  /*cell format*/
  SHR_BITCOPY_RANGE(&val, 0, buf, DNXC_VSC256_INBAND_PAYLOAD_CELL_FORMAT_START + offset, DNXC_VSC256_INBAND_PAYLOAD_CELL_FORMAT_LENGTH);

  cell->payload.inband.cell_format = val & 0xff;

  /*cell id*/
  SHR_BITCOPY_RANGE(&(cell->payload.inband.cell_id), 0, buf, DNXC_VSC256_INBAND_PAYLOAD_CELL_ID_START + offset, DNXC_VSC256_INBAND_PAYLOAD_CELL_ID_LENGTH);

  /*cell seq*/
  SHR_BITCOPY_RANGE(&(cell->payload.inband.seq_num), 0, buf, DNXC_VSC256_INBAND_PAYLOAD_SEQUENCE_NUMBER_START + offset, DNXC_VSC256_INBAND_PAYLOAD_SEQUENCE_NUMBER_LENGTH);

  /*num of commands*/
  SHR_BITCOPY_RANGE(&(cell->payload.inband.nof_commands), 0, buf, DNXC_VSC256_INBAND_PAYLOAD_NUMBER_OF_COMMANDS_START + offset, DNXC_VSC256_INBAND_PAYLOAD_NUMBER_OF_COMMANDS_LENGTH);
  if(cell->payload.inband.nof_commands > DNXC_VSC256_INBAND_PAYLOAD_MAX_VALID_COMMANDS) {
    cell->payload.inband.nof_commands = DNXC_VSC256_INBAND_PAYLOAD_MAX_VALID_COMMANDS;
  }

  for(i=0 ; i<cell->payload.inband.nof_commands ; i++) {
    /*command type*/
    val = 0;
    SHR_BITCOPY_RANGE(&val, 0, buf, DNXC_VSC256_INBAND_PAYLOAD_OPCODE_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF + offset, DNXC_VSC256_INBAND_PAYLOAD_OPCODE_LENGTH);
    val &= (0xffffffffL) >> (32 - DNXC_VSC256_INBAND_PAYLOAD_OPCODE_LENGTH);
    switch(val) {
          case 0:  cell->payload.inband.commands[i].opcode = soc_dnxc_inband_mem_read; break;
          case 1: cell->payload.inband.commands[i].opcode = soc_dnxc_inband_mem_write; break;
          case 2:  cell->payload.inband.commands[i].opcode = soc_dnxc_inband_reg_read; break;
          case 3: cell->payload.inband.commands[i].opcode = soc_dnxc_inband_reg_write; break;
          /* must default. Otherwise - compilation error */
          /* coverity[dead_error_begin:FALSE] */
          default: SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported opcode"); 
    }

    /*block*/
    val = 0;
    SHR_BITCOPY_RANGE(&val, 0, buf, DNXC_VSC256_INBAND_PAYLOAD_BLOCK_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF + offset, DNXC_VSC256_INBAND_PAYLOAD_BLOCK_LENGTH);
    cell->payload.inband.commands[i].schan_block = val;

    /*data size*/
    val = 0;
    SHR_BITCOPY_RANGE(&val, 0, buf, DNXC_VSC256_INBAND_PAYLOAD_LENGTH_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF + offset, DNXC_VSC256_INBAND_PAYLOAD_LENGTH_LENGTH);
    val &= (0xffffffffL) >> (32 - DNXC_VSC256_INBAND_PAYLOAD_LENGTH_LENGTH);
    cell->payload.inband.commands[i].length = (val+1)*4;

    /*data*/
    SHR_BITCOPY_RANGE(cell->payload.inband.commands[i].data, 0, buf, DNXC_VSC256_INBAND_PAYLOAD_DATA_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF 
      + ((3-val)*32)/* MSB allignment*/
      + offset, ((val+1)*32) );

    /*offset*/
    SHR_BITCOPY_RANGE(&(cell->payload.inband.commands[i].offset), 0, buf, DNXC_VSC256_INBAND_PAYLOAD_ADDRESS_START-i*DNXC_VSC256_INBAND_PAYLOAD_DIFF + offset, DNXC_VSC256_INBAND_PAYLOAD_ADDRESS_LENGTH);

    
  }

exit:
  SHR_FUNC_EXIT; 
}

shr_error_e 
soc_dnxc_vsc256_build_header(int unit, const dnxc_vsc256_sr_cell_t* cell, int buf_size_bytes, uint32* buf) 
{
  soc_dnxc_device_type_actual_value_t tmp_actual_value;
  uint32 tmp_u32_data_cell_field;
  SHR_FUNC_INIT_VARS(unit);

  /* We assume that the buffer is initialized, and its size is (at least) DNXC_VSC256_SR_DATA_CELL_HEADER_SIZE, and it points to the begin of the header. */

  if(buf_size_bytes < DNXC_VSC256_SR_DATA_CELL_HEADER_SIZE) {
    SHR_ERR_EXIT(_SHR_E_PARAM, "SR header minimum buffer size is %d",DNXC_VSC256_SR_DATA_CELL_HEADER_SIZE);
  }

  /*
  * bits - Copy of the cell type
  */
  tmp_u32_data_cell_field = cell->header.cell_type;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_CELL_TYPE_START, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_CELL_TYPE_LENGTH);

  /*
   * bits  - source id
   */
  tmp_u32_data_cell_field = cell->header.source_device;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_SOURCE_ID_START,&(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_SOURCE_ID_LENGTH);


  /*
   * bits  - source level
   */
  SHR_IF_ERR_EXIT(soc_dnxc_actual_entity_value(unit, cell->header.source_level, &tmp_actual_value));
  tmp_u32_data_cell_field = (uint32)tmp_actual_value;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_SRC_LEVEL_START,&(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_SRC_LEVEL_LENGTH);

  /*
   * bits  - destination level
   */
  SHR_IF_ERR_EXIT(soc_dnxc_actual_entity_value(unit, cell->header.destination_level, &tmp_actual_value));
  tmp_u32_data_cell_field = (uint32)tmp_actual_value;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_DEST_LEVEL_START, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_DEST_LEVEL_LENGTH);

  /*
   * bits  - fip switch
   */
  tmp_u32_data_cell_field = cell->header.fip_link;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_START, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH);
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_POSITION_1, &(tmp_u32_data_cell_field), DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_START_1,  DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH_1);

  /*
   * bits  - fe1 switch
   */
  tmp_u32_data_cell_field = cell->header.fe1_link;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH);
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_1, &(tmp_u32_data_cell_field), DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START_1,  DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_1);
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_2, &(tmp_u32_data_cell_field), DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START_2,  DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_2);

  /*
   * bits  - fe2 switch
   */
  tmp_u32_data_cell_field = cell->header.fe2_link;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH);
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_1, &(tmp_u32_data_cell_field), DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START_1,  DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_1);
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_2, &(tmp_u32_data_cell_field), DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START_2,  DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_2);

  /*
   * bits  - fe3 switch
   */
  tmp_u32_data_cell_field = cell->header.fe3_link;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH);
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_1, &(tmp_u32_data_cell_field), DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START_1,  DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_1);
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_2, &(tmp_u32_data_cell_field), DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START_2,  DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_2);

  /*
   * bit - In-band cell
   */
  tmp_u32_data_cell_field = cell->header.is_inband;
  SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_INBAND_CELL_POSITION, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_INBAND_CELL_LENGTH);

  /*
   * bits - pipe index
   */
  if (cell->header.pipe_id != -1)
  {
      tmp_u32_data_cell_field = 0x1;
      SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_POSITION, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_LENGTH);
      tmp_u32_data_cell_field = cell->header.pipe_id;
      SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_POSITION, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_PIPE_ID_LENGTH);
  } else {
      tmp_u32_data_cell_field = 0x0;
      SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_POSITION, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_LENGTH);
      tmp_u32_data_cell_field = 0x0;
      SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_POSITION, &(tmp_u32_data_cell_field), 0,  DNXC_VSC256_SR_DATA_CELL_PIPE_ID_LENGTH);
  }

exit:
  SHR_FUNC_EXIT;  
}

shr_error_e soc_dnxc_vsc256_parse_header(int unit, soc_reg_above_64_val_t reg_val, dnxc_vsc256_sr_cell_t* cell)
{
  soc_dnxc_device_type_actual_value_t tmp_actual_value = 0;
  uint32 val;
  SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

  /* We assume that the data is in bits 0-88 of the register. */

  sal_memset(&cell->header, 0, sizeof(dnxc_vsc256_sr_cell_header_t));
  /*
  * bits - Copy of the cell type
  */
  SHR_BITCOPY_RANGE(&cell->header.cell_type, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_CELL_TYPE_START, DNXC_VSC256_SR_DATA_CELL_CELL_TYPE_LENGTH);

  /*
   * bits  - source id
   */
  SHR_BITCOPY_RANGE((uint32 *)&cell->header.source_device, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_SOURCE_ID_START, DNXC_VSC256_SR_DATA_CELL_SOURCE_ID_LENGTH);


  /*
   * bits  - source level
   */
  SHR_BITCOPY_RANGE(&tmp_actual_value, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_SRC_LEVEL_START, DNXC_VSC256_SR_DATA_CELL_SRC_LEVEL_LENGTH);
  SHR_IF_ERR_EXIT(soc_dnxc_device_entity_value(unit, tmp_actual_value, &cell->header.source_level));
  
  /*
   * bits  - destination level
   */
  tmp_actual_value = 0;
  SHR_BITCOPY_RANGE(&tmp_actual_value, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_DEST_LEVEL_START, DNXC_VSC256_SR_DATA_CELL_DEST_LEVEL_LENGTH);
  SHR_IF_ERR_EXIT(soc_dnxc_device_entity_value(unit, tmp_actual_value, &cell->header.destination_level));
  
  /*
   * bits  - fip switch
   */
  val = 0;
  SHR_BITCOPY_RANGE(&val, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_START, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH);
  SHR_BITCOPY_RANGE(&val, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_START_1, reg_val, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_POSITION_1, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH_1);
  cell->header.fip_link = val;
  /*
   * bits  - fe1 switch
   */
  val = 0;
  SHR_BITCOPY_RANGE(&val, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH);
  SHR_BITCOPY_RANGE(&val, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START_1, reg_val, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_1, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_1);
  SHR_BITCOPY_RANGE(&val, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START_2, reg_val, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_2, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_2);
  cell->header.fe1_link = val;

  /*
   * bits  - fe2 switch
   */
  val = 0;
  SHR_BITCOPY_RANGE(&val, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH);
  SHR_BITCOPY_RANGE(&val, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START_1, reg_val, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_1, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_1);
  SHR_BITCOPY_RANGE(&val, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START_2, reg_val, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_2, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_2);
  cell->header.fe2_link = val;

  /*
   * bits  - fe3 switch
   */
  val = 0;
  SHR_BITCOPY_RANGE(&val, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH);
  SHR_BITCOPY_RANGE(&val, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START_1, reg_val, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_1, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_1);
  SHR_BITCOPY_RANGE(&val, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START_2, reg_val, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_2, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_2);
  cell->header.fe3_link = val;

  /*
   * bit - In-band cell
   */
  SHR_BITCOPY_RANGE(&cell->header.is_inband, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_INBAND_CELL_POSITION, DNXC_VSC256_SR_DATA_CELL_INBAND_CELL_LENGTH);

  /*
   * bits - pipe index
   */
  val = 0;
  SHR_BITCOPY_RANGE(&val, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_POSITION, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_LENGTH);
  if (val)
  {
      val = 0;
      SHR_BITCOPY_RANGE(&val, 0, reg_val, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_POSITION, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_LENGTH);
      cell->header.pipe_id = val;
  }

  
exit:
  SHR_FUNC_EXIT;  
}

#undef BSL_LOG_MODULE
