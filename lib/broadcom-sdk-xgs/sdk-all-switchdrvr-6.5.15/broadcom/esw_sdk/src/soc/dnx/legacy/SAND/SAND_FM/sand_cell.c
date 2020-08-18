/* $Id: sand_cell.c,v 1.7 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/* $Id: sand_cell.c,v 1.7 Broadcom SDK $
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

#include <soc/dnx/legacy/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/SAND/Utils/sand_bitstream.h>
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dnx/legacy/SAND/SAND_FM/sand_cell.h>
#include <shared/shrextend/shrextend_error.h>
/*
 */




/*
 */
/*****************************************************
*NAME:
*   dnx_sand_pack_control_cell
*DATE:
*   11/SEP/2002
*FUNCTION:
* packs the structured control_cell to 10 bytes
*INPUT:
*  DNX_SAND_DIRECT:
*   DNX_SAND_IN    DNX_SAND_CONTROL_CELL control_cell
*                                 - the structured control cell
*   DNX_SAND_INOUT unsigned char     *packed_control_cell
*                                 - empty 10 bytes buffer
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-Zero in case of an error.
*  DNX_SAND_INDIRECT:
*REMARKS:
*   The method uses the fields offset / shift / mask parameters
*   from the register file, to write the to the right location
*   of the 10 bytes buffer
*SEE ALSO:
*****************************************************/
shr_error_e
  dnx_sand_pack_control_cell(
    DNX_SAND_IN     DNX_SAND_CONTROL_CELL control_cell,
    DNX_SAND_INOUT  unsigned char     *packed_control_cell
  )
{
  /*
   *  - switch on the cell type
   *  - for each type pack the necessary fields to the buffer
   */
  return _SHR_E_NONE ;
}
/*
 */
/*****************************************************
*NAME:
*   dnx_sand_unpack_control_cell
*DATE:
*   11/SEP/2002
*FUNCTION:
* unpacks 10 bytes to a structured control cell
*INPUT:
*  DNX_SAND_DIRECT:
*   DNX_SAND_IN   unsigned char     *packed_control_cell
*                                 - the packed cell
*   DNX_SAND_INOUT  DNX_SAND_CONTROL_CELL *control_cell
*       - the empty structure for this procedure to fill
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-Zero in case of an error.
*  DNX_SAND_INDIRECT:
*REMARKS:
*   The method uses the fields offset / shift / mask parameters
*   from the register file, to write the to the structure
*   the values of the fields packed in the 10 bytes buffer
*SEE ALSO:
*****************************************************/
shr_error_e
  dnx_sand_unpack_control_cell(
    DNX_SAND_IN     unsigned char     *packed_control_cell,
    DNX_SAND_INOUT  DNX_SAND_CONTROL_CELL *control_cell
  )
{
  /*
   *  - switch on the cell type
   *  - for each type unpack the necessary field to the structure
   */
  return _SHR_E_NONE ;
}

/*
*/
/*****************************************************
*NAME:
*   dnx_sand_pack_dest_routed_data_cell
*DATE:
*   30/NOV/2008
*FUNCTION:
* packs a structured data cell to 40 bytes (10 uint32s)
* The data cell can be a unicast / multicast data cell.
*INPUT:
*  DNX_SAND_DIRECT:
*   DNX_SAND_IN  DNX_SAND_DATA_CELL  *data_cell
*                            - the structured data cell
*   DNX_SAND_OUT uint32        *packed_data_cell
*                            - empty 10 uint32s (i.e. 40 bytes) buffer
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-Zero in case of an error.
*  DNX_SAND_INDIRECT:
*REMARKS:
*   The method uses the fields offset / shift / mask parameters
*   from the register file, to write the to the right location
*   of the 40 bytes buffer
*SEE ALSO:
*****************************************************/
static int
  dnx_sand_pack_dest_routed_data_cell(
    DNX_SAND_IN   DNX_SAND_DATA_CELL  *data_cell,
    DNX_SAND_OUT  uint32        *packed_data_cell
  )
{
  shr_error_e
    res = _SHR_E_NONE;
  uint32
    iter = 0 ;
  uint32
    tmp_u32_data_cell_field = 0;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    

  /*
  * Initialization to zero
  */
  packed_data_cell = 0 ;
  /*
   * bits 306:296 - destination id (or multicast id)
   */
  tmp_u32_data_cell_field = data_cell->data_cell.dest_routed.dest_or_mc_id ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          DNX_SAND_DATA_CELL_DEST_ID_START,
          DNX_SAND_DATA_CELL_DEST_ID_LENGTH,
          (packed_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bits 295:281 - Origin-time
   */
  tmp_u32_data_cell_field = 0 ;
  tmp_u32_data_cell_field = data_cell->data_cell.dest_routed.origin_time ;

  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          DNX_SAND_DATA_CELL_ORIGIN_TIME_START,
          DNX_SAND_DATA_CELL_ORIGIN_TIME_LENGTH,
          (packed_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bits 280:272 - Fragment number
   */
 tmp_u32_data_cell_field = data_cell->data_cell.dest_routed.frag_number ;
 res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          DNX_SAND_DATA_CELL_FRAG_NUMBER_START,
          DNX_SAND_DATA_CELL_FRAG_NUMBER_LENGTH,
          (packed_data_cell)
       );
 SHR_IF_ERR_EXIT(res);

  /*
   * Copy the payload
   */
  for(iter=0; iter < DNX_SAND_DATA_CELL_PAYLOAD_IN_BYTES ; ++iter)
  {
    tmp_u32_data_cell_field = data_cell->data_cell.dest_routed.cell_data[iter] ;
    res = dnx_sand_bitstream_set_any_field(
            &(tmp_u32_data_cell_field),
            DNX_SAND_NOF_BITS_IN_CHAR * iter + DNX_SAND_DATA_CELL_PAYLOAD_START,
            DNX_SAND_NOF_BITS_IN_CHAR,
            (packed_data_cell)
          );
    SHR_IF_ERR_EXIT(res);
  }

exit:
   SHR_FUNC_EXIT;
}

/*
 * unpacks 40 bytes to a structured data cell
 */
/*****************************************************
*NAME:
*   dnx_sand_unpack_dest_routed_data_cell
*DATE:
*   30/NOV/2008
*FUNCTION:
* unpacks 40 bytes to a structured destination routed data cell
*INPUT:
*  DNX_SAND_DIRECT:
*   DNX_SAND_IN  uint32        *packed_data_cell
*                            - the packed cell
*   DNX_SAND_OUT DNX_SAND_DATA_CELL  *data_cell
*                            - the empty structure for this procedure to fill
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-Zero in case of an error.
*  DNX_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
static int
  dnx_sand_unpack_dest_routed_data_cell(
    DNX_SAND_IN  uint32        *packed_data_cell,
    DNX_SAND_OUT DNX_SAND_DATA_CELL  *data_cell
  )
{
  uint32
    iter = 0 ,
    tmp_u32_data_cell_field = 0;
  int
    res = _SHR_E_NONE ;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    

  /*
   * bits 306:296 - destination id (or multicast id)
   */
  tmp_u32_data_cell_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          packed_data_cell,
          DNX_SAND_DATA_CELL_DEST_ID_START,
          DNX_SAND_DATA_CELL_DEST_ID_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.dest_routed.dest_or_mc_id = (uint16) tmp_u32_data_cell_field;
  /* casting allowed: only 11 significant bits in tmp_u32_data_cell_field*/

  /*
   * bits 295:281 - Origin-time
   */
  tmp_u32_data_cell_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          packed_data_cell,
          DNX_SAND_DATA_CELL_ORIGIN_TIME_START,
          DNX_SAND_DATA_CELL_ORIGIN_TIME_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.dest_routed.origin_time = (uint16) tmp_u32_data_cell_field;
  /* casting allowed: only 15 significant bits in tmp_u32_data_cell_field*/

  /*
   * bits 280:272 - Fragment number
   */
  tmp_u32_data_cell_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          packed_data_cell,
          DNX_SAND_DATA_CELL_FRAG_NUMBER_START,
          DNX_SAND_DATA_CELL_FRAG_NUMBER_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.dest_routed.frag_number = (uint16) tmp_u32_data_cell_field;
  /* casting allowed: only 9 significant bits in tmp_u32_data_cell_field*/

  /*
   * Bits 271:16 Copy the payload for both cases
   */
  for(iter=0; iter < DNX_SAND_DATA_CELL_PAYLOAD_IN_BYTES ; ++iter)
  {
    tmp_u32_data_cell_field = 0 ;
    res = dnx_sand_bitstream_get_any_field(
            (packed_data_cell) ,
            DNX_SAND_NOF_BITS_IN_CHAR * iter + DNX_SAND_DATA_CELL_PAYLOAD_START,
            DNX_SAND_NOF_BITS_IN_CHAR,
            &(tmp_u32_data_cell_field)
          );
    SHR_IF_ERR_EXIT(res);
    data_cell->data_cell.dest_routed.cell_data[iter] = (uint8) tmp_u32_data_cell_field ;
    /* casting allowed: only 8 significant bits in tmp_u32_data_cell_field*/
  }

exit:
  SHR_FUNC_EXIT;
}

/*
 * Builds a buffer for an in-band source-routed cell
 */
/*****************************************************
*NAME:
*   dnx_sand_pack_source_routed_data_cell
*DATE:
*   07/DEC/2008
*FUNCTION:
* packs a structured data cell to 40 bytes (10 uint32s)
* The data cell is a source routed data cell
*INPUT:
*  DNX_SAND_DIRECT:
*   DNX_SAND_IN  DNX_SAND_DATA_CELL  *data_cell
*                            - the structured cpu data cell
*   DNX_SAND_OUT uint32        *packed_cpu_data_cell
*                            - empty 40 bytes buffer
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-Zero in case of an error.
*  DNX_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
static int
  dnx_sand_pack_source_routed_data_cell(
    DNX_SAND_IN  DNX_SAND_DATA_CELL                   *data_cell,
    DNX_SAND_OUT uint32                         *packed_cpu_data_cell
  )
{
  uint32
    iter = 0,
    tmp_u32_sr_extra_field ,
    tmp_u32_sr_extra_field2 ;
  int
    res = _SHR_E_NONE ;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    

  /*
   * bits 306:304 - source level
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.src_level ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_DATA_CELL_SRC_LEVEL_START ,
          DNX_SAND_DATA_CELL_SRC_LEVEL_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bits 303:301 - destination level
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.dest_level ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_DATA_CELL_DEST_LEVEL_START ,
          DNX_SAND_DATA_CELL_DEST_LEVEL_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bits 300:296 - fip switch
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fip_switch ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_DATA_CELL_FIP_SWITCH_START ,
          DNX_SAND_DATA_CELL_FIP_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bits 295:291 - fe1 switch
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fe1_switch ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_DATA_CELL_FE1_SWITCH_START ,
          DNX_SAND_DATA_CELL_FE1_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bits 290:285 - fe2 switch
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fe2_switch ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_DATA_CELL_FE2_SWITCH_START ,
          DNX_SAND_DATA_CELL_FE2_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bits 284:280 - fe3 switch
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.fe3_switch ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_DATA_CELL_FE3_SWITCH_START ,
          DNX_SAND_DATA_CELL_FE3_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * Filling the values between the first fields and the payload - bits 279:272
   */
  /*
   * bit 279 - FIP-switch
   */
  tmp_u32_sr_extra_field = 0;
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fip_switch ;
  res = dnx_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          DNX_SAND_SR_DATA_CELL_FIP_SWITCH_START,
          DNX_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_FIP_SWITCH_POSITION ,
          DNX_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bit 278 - FE1-switch
   */
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe1_switch ;
  res = dnx_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          DNX_SAND_SR_DATA_CELL_FE1_SWITCH_START,
          DNX_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_FE1_SWITCH_POSITION ,
          DNX_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bit 277 - FE2-switch
   */
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe2_switch ;
  res = dnx_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          DNX_SAND_SR_DATA_CELL_FE2_SWITCH_START,
          DNX_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_FE2_SWITCH_POSITION ,
          DNX_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bit 276 - FE3-switch
   */
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe3_switch ;
  res = dnx_sand_bitstream_get_any_field(
          &(tmp_u32_sr_extra_field2),
          DNX_SAND_SR_DATA_CELL_FE3_SWITCH_START,
          DNX_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_FE3_SWITCH_POSITION ,
          DNX_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
  * bit 275 - In-band cell
  */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.inband_cell ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_INBAND_CELL_POSITION ,
          DNX_SAND_SR_DATA_CELL_INBAND_CELL_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bit 274 - Ack
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.ack ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_ACK_POSITION ,
          DNX_SAND_SR_DATA_CELL_ACK_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bit 273 - Indirect
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.indirect ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_INDIRECT_POSITION ,
          DNX_SAND_SR_DATA_CELL_INDIRECT_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bit 272 - Read / write (0/1)
   */
  tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.read_or_write ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_RW_POSITION ,
          DNX_SAND_SR_DATA_CELL_RW_LENGTH,
          (packed_cpu_data_cell)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * Building the payload, differentiating between inband or regular source routed cell
   */
  if (data_cell->data_cell.source_routed.inband_cell)
  {
    for(iter = 0; iter < DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
    {
      if (data_cell->data_cell.source_routed.add_wr_cell[iter] != 0)
      {
        /*
         * bit 255, 206, 157, 108, 59 - Valid
         */
        res = dnx_sand_bitstream_set_bit(
                packed_cpu_data_cell,
                DNX_SAND_SR_DATA_CELL_VALID_START +
                (DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * DNX_SAND_SR_DATA_CELL_WORD_LENGTH
              );
        SHR_IF_ERR_EXIT(res);

        /*
         * bits 254:239, 205:190, 156:141, 107:92, 58:43 - Read / write address
         */
        tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.add_wr_cell[iter];
        res = dnx_sand_bitstream_set_any_field(
                &(tmp_u32_sr_extra_field),
                DNX_SAND_SR_DATA_CELL_ADDRESS_START +
                (DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * DNX_SAND_SR_DATA_CELL_WORD_LENGTH ,
                DNX_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
                packed_cpu_data_cell
              );
        SHR_IF_ERR_EXIT(res);
        /*
         * bits 238:207, 189:158, 140:109, 91:60, 42:11 - Write_data, read data on the ack cell
         */
        tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.data_wr_cell[iter];
        res = dnx_sand_bitstream_set_any_field(
                &(tmp_u32_sr_extra_field),
                DNX_SAND_SR_DATA_CELL_RW_START +
                (DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * DNX_SAND_SR_DATA_CELL_WORD_LENGTH,
                DNX_SAND_SR_DATA_CELL_WRITE_LENGTH,
                packed_cpu_data_cell
              );
        SHR_IF_ERR_EXIT(res);
      }
    }

    /*
     * bits 10:2 - Cell identifier
     */
    tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.cell_ident;
    res = dnx_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            DNX_SAND_SR_DATA_CELL_CELL_IDENT_START ,
            DNX_SAND_SR_DATA_CELL_CELL_IDENT_LENGTH,
            packed_cpu_data_cell
          );
    SHR_IF_ERR_EXIT(res);

    /*
     * bits 1:0 - Cell format (for future cell formats)
     */
    tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.cell_format;
    res = dnx_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            DNX_SAND_SR_DATA_CELL_CELL_FORMAT_START ,
            DNX_SAND_SR_DATA_CELL_CELL_FORMAT_LENGTH,
            packed_cpu_data_cell
          );
    SHR_IF_ERR_EXIT(res);
  }
  else /* Regular source routed cell (not inband) */
  {
    /* The order is decided arbitrarily to be different: all the data, all the addresses and then cell_ident */
    for(iter = 0; iter < DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
    {
      /*
       * bits 95:80, 79:64, 63:48, 47:32, 31:16 - Read / write address
       */
      tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.add_wr_cell[iter];
      res = dnx_sand_bitstream_set_any_field(
              &(tmp_u32_sr_extra_field),
              DNX_SAND_SR_DATA_CELL_NOT_INBAND_ADDRESS_START +
              (DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * DNX_SAND_SR_DATA_CELL_ADDRESS_LENGTH ,
              DNX_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              packed_cpu_data_cell
            );
      SHR_IF_ERR_EXIT(res);
      /*
       * bits 255:224, 223:192, 191:160, 159:128, 127:96 - Write_data, read data on the ack cell
       */
      tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.data_wr_cell[iter];
      res = dnx_sand_bitstream_set_any_field(
              &(tmp_u32_sr_extra_field),
              DNX_SAND_SR_DATA_CELL_NOT_INBAND_RW_START +
              (DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter) * DNX_SAND_SR_DATA_CELL_WRITE_LENGTH,
              DNX_SAND_SR_DATA_CELL_WRITE_LENGTH,
              packed_cpu_data_cell
            );
      SHR_IF_ERR_EXIT(res);
    }

    /*
     * bits 15:0 - Cell identifier (the 16 bits are used)
     */
    tmp_u32_sr_extra_field = data_cell->data_cell.source_routed.cell_ident;
    res = dnx_sand_bitstream_set_any_field(
            &(tmp_u32_sr_extra_field),
            DNX_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_START ,
            DNX_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_LENGTH,
            packed_cpu_data_cell
          );
    SHR_IF_ERR_EXIT(res);
  }

exit:
  SHR_FUNC_EXIT;

}

/*
 * Builds an in-band source-routed cell from a buffer
 */
/*****************************************************
*NAME:
*   dnx_sand_unpack_source_routed_data_cell
*DATE:
*   07/DEC/2008
*FUNCTION:
* Build an in-band source-routed cell from a buffer
*INPUT:
*  DNX_SAND_DIRECT:
*   DNX_SAND_IN  uint32         *packed_cpu_data_cell
*                             - 40 bytes buffer with data
*   DNX_SAND_IN  uint8        is_fe600
*                             - an adaptation to fe600
*   DNX_SAND_OUT DNX_SAND_DATA_CELL   *data_cell
*                             - the structured source routed cpu data cell
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-Zero in case of an error.
*  DNX_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
static int
  dnx_sand_unpack_source_routed_data_cell(
    DNX_SAND_IN  uint32         *packed_cpu_data_cell,
    DNX_SAND_IN  uint8        is_fe600,
    DNX_SAND_OUT DNX_SAND_DATA_CELL   *data_cell
  )
{
  uint32
    iter = 0 ,
    tmp_field_for_data_cell_add_fields ,
    tmp_u32_sr_extra_field ,
    tmp_u32_sr_extra_field2 ;
  int
    res = _SHR_E_NONE ;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    

  /*
   * bits 306:304 - source level
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          DNX_SAND_DATA_CELL_SRC_LEVEL_START - is_fe600,
          DNX_SAND_DATA_CELL_SRC_LEVEL_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.src_level = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 3 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 303:301 - destination level
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          DNX_SAND_DATA_CELL_DEST_LEVEL_START - is_fe600,
          DNX_SAND_DATA_CELL_DEST_LEVEL_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.dest_level = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 3 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 300:296 - fip switch
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          DNX_SAND_DATA_CELL_FIP_SWITCH_START - is_fe600,
          DNX_SAND_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.fip_switch = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 5 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 295:291 - fe1 switch
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          DNX_SAND_DATA_CELL_FE1_SWITCH_START - is_fe600,
          DNX_SAND_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.fe1_switch = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 5 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 290:285 - fe2 switch
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          DNX_SAND_DATA_CELL_FE2_SWITCH_START - is_fe600,
          DNX_SAND_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.fe2_switch = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 6 significant bits in tmp_u32_sr_extra_field*/

  /*
   * bits 284:280 - fe3 switch
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          packed_cpu_data_cell,
          DNX_SAND_DATA_CELL_FE3_SWITCH_START - is_fe600,
          DNX_SAND_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.fe3_switch = (uint8) tmp_u32_sr_extra_field;
  /* casting allowed: only 5 significant bits in tmp_u32_sr_extra_field*/

  /*
   * Filling the extra fields of the in-band cell
   */

  /*
   * bit 279 - FIP-switch
   */
  res = dnx_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          DNX_SAND_SR_DATA_CELL_FIP_SWITCH_POSITION - is_fe600,
          DNX_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fip_switch ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_FIP_SWITCH_START,
          DNX_SAND_SR_DATA_CELL_FIP_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.fip_switch = (uint8) tmp_u32_sr_extra_field2 ;
  /* casting allowed: only 6 significant bit in tmp_u32_sr_extra_field2*/

  /*
   * bit 278 - FE1-switch
   */
  tmp_u32_sr_extra_field = 0 ;
  tmp_u32_sr_extra_field2 = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          DNX_SAND_SR_DATA_CELL_FE1_SWITCH_POSITION - is_fe600,
          DNX_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe1_switch ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_FE1_SWITCH_START,
          DNX_SAND_SR_DATA_CELL_FE1_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.fe1_switch = (uint8) tmp_u32_sr_extra_field2 ;
  /* casting allowed: only 6 significant bit in tmp_u32_sr_extra_field2*/

  /*
   * bit 277 - FE2-switch
   */
  tmp_u32_sr_extra_field = 0 ;
  tmp_u32_sr_extra_field2 = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          DNX_SAND_SR_DATA_CELL_FE2_SWITCH_POSITION - is_fe600,
          DNX_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe2_switch ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_FE2_SWITCH_START,
          DNX_SAND_SR_DATA_CELL_FE2_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.fe2_switch = (uint8) tmp_u32_sr_extra_field2 ;
  /* casting allowed: only 7 significant bit in tmp_u32_sr_extra_field2*/

  /*
   * bit 276 - FE3-switch
   */
  tmp_u32_sr_extra_field = 0 ;
  tmp_u32_sr_extra_field2 = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          DNX_SAND_SR_DATA_CELL_FE3_SWITCH_POSITION - is_fe600,
          DNX_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  tmp_u32_sr_extra_field2 = data_cell->data_cell.source_routed.fe3_switch ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_sr_extra_field),
          DNX_SAND_SR_DATA_CELL_FE3_SWITCH_START,
          DNX_SAND_SR_DATA_CELL_FE3_SWITCH_LENGTH,
          &(tmp_u32_sr_extra_field2)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.fe3_switch = (uint8) tmp_u32_sr_extra_field2 ;
  /* casting allowed: only 6 significant bit in tmp_u32_sr_extra_field2*/

  /*
   * bit 275 - In-band cell
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          DNX_SAND_SR_DATA_CELL_INBAND_CELL_POSITION - is_fe600,
          DNX_SAND_SR_DATA_CELL_INBAND_CELL_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.inband_cell = (uint8) tmp_u32_sr_extra_field ;
  /* casting allowed: only 1 significant bit in tmp_u32_sr_extra_field*/

  /*
   * bit 274 - Ack
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          DNX_SAND_SR_DATA_CELL_ACK_POSITION - is_fe600,
          DNX_SAND_SR_DATA_CELL_ACK_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.ack = (uint8) tmp_u32_sr_extra_field ;
  /* casting allowed: only 1 significant bit in tmp_u32_sr_extra_field*/

  /*
   * bit 273 - Indirect
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          DNX_SAND_SR_DATA_CELL_INDIRECT_POSITION - is_fe600,
          DNX_SAND_SR_DATA_CELL_INDIRECT_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.indirect = (uint8) tmp_u32_sr_extra_field ;
  /* casting allowed: only 1 significant bit in tmp_u32_sr_extra_field*/

  /*
   * bit 272 - Read / write (0/1)
   */
  tmp_u32_sr_extra_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          (packed_cpu_data_cell),
          DNX_SAND_SR_DATA_CELL_RW_POSITION - is_fe600,
          DNX_SAND_SR_DATA_CELL_RW_LENGTH,
          &(tmp_u32_sr_extra_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->data_cell.source_routed.read_or_write = (uint8) tmp_u32_sr_extra_field ;
  /* casting allowed: only 1 significant bit in tmp_u32_sr_extra_field*/

  /*
   * Decomposing the payload, differentiating between inband or regular
   */
  if (data_cell->data_cell.source_routed.inband_cell)
  {
    for(iter = 0; iter < DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
    {
      /*
       * bits 254:239, 205:190, 156:141, 107:92, 58:43 - Read / write address
       */
      tmp_field_for_data_cell_add_fields = 0 ;
      res = dnx_sand_bitstream_get_any_field(
              packed_cpu_data_cell,
              DNX_SAND_SR_DATA_CELL_ADDRESS_START +
              iter * DNX_SAND_SR_DATA_CELL_WORD_LENGTH - is_fe600,
              DNX_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SHR_IF_ERR_EXIT(res);
      data_cell->data_cell.source_routed.add_wr_cell[DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter] =
        (uint16) tmp_field_for_data_cell_add_fields;
      /* casting allowed: only 16 significant bits in tmp_field_for_data_cell_add_fields*/

      /*
       * bits 238:207, 189:158, 140:109, 91:60, 42:11 - Write_data, read data on the ack cell
       */
      tmp_field_for_data_cell_add_fields = 0 ;
      res = dnx_sand_bitstream_get_any_field(
              packed_cpu_data_cell,
              DNX_SAND_SR_DATA_CELL_RW_START +
              iter * DNX_SAND_SR_DATA_CELL_WORD_LENGTH - is_fe600,
              DNX_SAND_SR_DATA_CELL_WRITE_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SHR_IF_ERR_EXIT(res);
      data_cell->data_cell.source_routed.data_wr_cell[DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter] =
        tmp_field_for_data_cell_add_fields ;
    }

    /*
     * bits 10:2 - Cell identifier
     */
    tmp_field_for_data_cell_add_fields = 0 ;
    res = dnx_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            DNX_SAND_SR_DATA_CELL_CELL_IDENT_START - is_fe600,
            DNX_SAND_SR_DATA_CELL_CELL_IDENT_LENGTH,
            &(tmp_field_for_data_cell_add_fields)
          );
    SHR_IF_ERR_EXIT(res);
    data_cell->data_cell.source_routed.cell_ident = (uint16) tmp_field_for_data_cell_add_fields;
    /* casting allowed: only 9 significant bits in tmp_field_for_data_cell_add_fields*/

    /*
    * bits 1:0 - Cell format (for future cell formats)
    */
    tmp_field_for_data_cell_add_fields = 0 ;
    res = dnx_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            DNX_SAND_SR_DATA_CELL_CELL_FORMAT_START - is_fe600,
            DNX_SAND_SR_DATA_CELL_CELL_FORMAT_LENGTH,
            &(tmp_field_for_data_cell_add_fields)
          );
    SHR_IF_ERR_EXIT(res);
    data_cell->data_cell.source_routed.cell_format = (uint8) tmp_field_for_data_cell_add_fields;
    /* casting allowed: only 2 significant bits in tmp_field_for_data_cell_add_fields*/
  }
  else /* Regular source routed cell, unpacking the same way we packed */
  {
    /* The order is decided arbitrarily to be different: all the data, all the addresses and then cell_ident */
    for(iter = 0; iter < DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
    {
      /*
       * bits 95:80, 79:64, 63:48, 47:32, 31:16 - Read / write address
       */
      tmp_field_for_data_cell_add_fields = 0 ;
      res = dnx_sand_bitstream_get_any_field(
              packed_cpu_data_cell,
              DNX_SAND_SR_DATA_CELL_NOT_INBAND_ADDRESS_START +
              iter * DNX_SAND_SR_DATA_CELL_ADDRESS_LENGTH  - is_fe600,
              DNX_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SHR_IF_ERR_EXIT(res);
      data_cell->data_cell.source_routed.add_wr_cell[DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter] = 
        (uint16) tmp_field_for_data_cell_add_fields;
      /* casting allowed: only 16 significant bits in tmp_field_for_data_cell_add_fields*/
      /*
       * bits 255:224, 223:192, 191:160, 159:128, 127:96 - Write_data, read data on the ack cell
       */
      tmp_field_for_data_cell_add_fields = 0 ;
      res = dnx_sand_bitstream_get_any_field(
              packed_cpu_data_cell,
              DNX_SAND_SR_DATA_CELL_NOT_INBAND_RW_START +
              iter * DNX_SAND_SR_DATA_CELL_WRITE_LENGTH  - is_fe600,
              DNX_SAND_SR_DATA_CELL_WRITE_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SHR_IF_ERR_EXIT(res);
      data_cell->data_cell.source_routed.data_wr_cell[DNX_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD - 1 - iter] = 
        tmp_field_for_data_cell_add_fields;
    }

    /*
     * bits 15:0 - Cell identifier (the 16 bits are used)
     */
    tmp_field_for_data_cell_add_fields = 0 ;
    res = dnx_sand_bitstream_get_any_field(
            packed_cpu_data_cell,
            DNX_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_START - is_fe600,
            DNX_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_LENGTH,
            &(tmp_field_for_data_cell_add_fields)
          );
    SHR_IF_ERR_EXIT(res);
    data_cell->data_cell.source_routed.cell_ident = (uint16) tmp_field_for_data_cell_add_fields;
    /* casting allowed: only 16 significant bits in tmp_field_for_data_cell_add_fields*/
  }
exit:
  SHR_FUNC_EXIT;

}


/*****************************************************
*NAME:
*   dnx_sand_data_cell_to_buffer
*DATE:
*   08/12/2008
*FUNCTION:
* Converts a Dnx_soc_sand_data_cell to the appropriate buffer according
* to its type (a source routed or destination routed cell)
*INPUT:
*  DNX_SAND_DIRECT:
*   DNX_SAND_IN  DNX_SAND_DATA_CELL    *data_cell
*                              - the structured data cell
*   DNX_SAND_OUT uint32          *packed_data_cell
*                              - empty 40 bytes buffer
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-Zero in case of an error.
*  DNX_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
int
  dnx_sand_data_cell_to_buffer(
    DNX_SAND_IN  DNX_SAND_DATA_CELL   *data_cell,
    DNX_SAND_OUT uint32         *packed_data_cell
  )
{
  int
    res = _SHR_E_NONE;
  uint32
    i_reg = 0 ,
    length ; /* for the length due to the inscription of the common fields*/
  uint32
    tmp_u32_inversion = 0,
    tmp_u32_data_cell_field = 0,
    tmp_buffer_with_offset[DNX_SAND_DATA_CELL_UINT32_SIZE];

  SHR_FUNC_INIT_VARS(NO_UNIT);
    


  res = dnx_sand_os_memset(
          tmp_buffer_with_offset,
          0x0,
          DNX_SAND_DATA_CELL_UINT32_SIZE * sizeof(uint32)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * Copy of the common fields
   */
  /*
   * bits 319:318 - Copy of the cell type
   */
  tmp_u32_data_cell_field = data_cell->cell_type ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          DNX_SAND_DATA_CELL_CELL_TYPE_START ,
          DNX_SAND_DATA_CELL_CELL_TYPE_LENGTH,
          packed_data_cell
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * bits 317:307 - source id
   */
  tmp_u32_data_cell_field = data_cell->source_id ;
  res = dnx_sand_bitstream_set_any_field(
          &(tmp_u32_data_cell_field),
          DNX_SAND_DATA_CELL_SOURCE_ID_START,
          DNX_SAND_DATA_CELL_SOURCE_ID_LENGTH,
          packed_data_cell
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * Copy of the specific fields according to the data cell type:
   * either source routed or destination routed
   */
  switch(data_cell->cell_type)
  {
  case DATA_CELL_TYPE_SOURCE_ROUTED:
    length = DNX_SAND_SR_DATA_CELL_NOT_COMMON_LENGTH ;
    res = dnx_sand_pack_source_routed_data_cell(
            data_cell,
            tmp_buffer_with_offset
          );
    SHR_IF_ERR_EXIT(res);

    res = dnx_sand_bitstream_set_any_field(
            tmp_buffer_with_offset,
            0,
            length,
            (packed_data_cell)
          );
    SHR_IF_ERR_EXIT(res);

    /*
     * Inverting the registers
     */
    for (i_reg = 0 ; i_reg < DNX_SAND_DATA_CELL_UINT32_SIZE / 2; ++i_reg)
    {
      tmp_u32_inversion = (packed_data_cell[DNX_SAND_DATA_CELL_UINT32_SIZE - 1 - i_reg ]);
      (packed_data_cell[DNX_SAND_DATA_CELL_UINT32_SIZE - 1 - i_reg ]) = (packed_data_cell[i_reg ]) ;
      (packed_data_cell[i_reg ]) = tmp_u32_inversion ;
    }
    break;

  case DATA_CELL_TYPE_DESTINATION_ROUTED:
    length = DNX_SAND_SR_DATA_CELL_NOT_COMMON_LENGTH ;
    res = dnx_sand_pack_dest_routed_data_cell(
            data_cell,
            tmp_buffer_with_offset
          );
    SHR_IF_ERR_EXIT(res);
    res = dnx_sand_bitstream_set_any_field(
            tmp_buffer_with_offset,
            0,
            length,
            (packed_data_cell)
          );
    SHR_IF_ERR_EXIT(res);
    break;

  default:
    res = _SHR_E_INTERNAL;
    SHR_IF_ERR_EXIT(res);
  }

exit:
   SHR_FUNC_EXIT;

}
/*
 * unpacks 40 bytes to a structured data cell
 */
/*****************************************************
*NAME:
*   dnx_sand_buffer_to_data_cell
*DATE:
*   09/12/2008
*FUNCTION:
* Converts a buffer to the appropriate Dnx_soc_sand_data_cell according to its type
*INPUT:
*  DNX_SAND_DIRECT:
*   DNX_SAND_IN  uint32        *packed_data_cell
*                            - the packed cell
*   DNX_SAND_IN  uint8       is_fe600
*                            - an adaptation for fe600
*   DNX_SAND_OUT DNX_SAND_DATA_CELL  *data_cell
*                            - the empty structure for this procedure to fill
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-Zero in case of an error.
*  DNX_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
int
  dnx_sand_buffer_to_data_cell(
    DNX_SAND_IN  uint32        *packed_data_cell,
    DNX_SAND_IN  uint8       is_fe600,
    DNX_SAND_OUT DNX_SAND_DATA_CELL  *data_cell
  )
{
  int
    res = _SHR_E_NONE ;
  uint32
    i_reg,
    tmp_u32_data_cell_field = 0 ,
    tmp_inverted_packed_data_cell[DNX_SAND_DATA_CELL_UINT32_SIZE] ;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    

  res = dnx_sand_os_memset(
          tmp_inverted_packed_data_cell,
          0x0,
          DNX_SAND_DATA_CELL_UINT32_SIZE * sizeof(uint32)
        );
  SHR_IF_ERR_EXIT(res);

  /*
   * Inverting the registers
   */
  for (i_reg = 0 ; i_reg < DNX_SAND_DATA_CELL_UINT32_SIZE ; ++i_reg)
  {
    tmp_inverted_packed_data_cell[i_reg] = packed_data_cell[DNX_SAND_DATA_CELL_UINT32_SIZE - 1 - i_reg ] ;
  }


  /*
   * Copy of the common fields
   */
  /*
   * bits 319:318 - cell type
   */
  res = dnx_sand_bitstream_get_any_field(
          (tmp_inverted_packed_data_cell),
          DNX_SAND_DATA_CELL_CELL_TYPE_START,
          DNX_SAND_DATA_CELL_CELL_TYPE_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->cell_type = (uint8) tmp_u32_data_cell_field;
  /* casting allowed: only 2 significant bits in tmp_u32_data_cell_field*/

  /*
   * bits 317:307 - source id
   */
  tmp_u32_data_cell_field = 0 ;
  res = dnx_sand_bitstream_get_any_field(
          (tmp_inverted_packed_data_cell),
          DNX_SAND_DATA_CELL_SOURCE_ID_START - is_fe600,
          DNX_SAND_DATA_CELL_SOURCE_ID_LENGTH,
          &(tmp_u32_data_cell_field)
        );
  SHR_IF_ERR_EXIT(res);
  data_cell->source_id = (uint16) tmp_u32_data_cell_field;
  /* casting allowed: only 11 significant bits in tmp_u32_data_cell_field*/

  /*
   * Copy of the specific fields according to the data cell type:
   * either source routed or destination routed
   */
  switch(data_cell->cell_type)
  {
  case DATA_CELL_TYPE_SOURCE_ROUTED:
    res = dnx_sand_unpack_source_routed_data_cell(
            tmp_inverted_packed_data_cell,
            is_fe600,
            data_cell
          );
    SHR_IF_ERR_EXIT(res);
    break;

  case DATA_CELL_TYPE_DESTINATION_ROUTED:
    SHR_IF_ERR_EXIT(res);
    res = dnx_sand_unpack_dest_routed_data_cell(
            tmp_inverted_packed_data_cell,
            data_cell
          );
    SHR_IF_ERR_EXIT(res);
    break;

  default:
    res = _SHR_E_INTERNAL;
    SHR_IF_ERR_EXIT(res);
  }

exit:
  SHR_FUNC_EXIT;
}



/*
 */
/*****************************************************
*NAME:
*  dnx_sand_actual_entity_value
*DATE:
*  12/NOV/2002
*FUNCTION:
*  translates the FE1_ENTITY, FE2_ENTITY, etc,
*  into the real 3 bit field, expected within a cell
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_DEVICE_ENTITY device_entity -
*       logic name of device entity
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    The value to actually load in the cell
*  DNX_SAND_INDIRECT:
*REMARKS:
* Input value can be out of the above range in 2 cases:
* 1. It is FE13_ENTITY, which is defined for other services,
*    but has no meaning here, in the context of cells,
*    so we return DNX_SAND_ACTUAL_BAD_VALUE (0xFF)
* 2. All other values that are not defined above:
*    we return DNX_SAND_ACTUAL_OUT_OF_RANGE (0xFFFFFFFF)
*SEE ALSO:
*****************************************************/
uint32
  dnx_sand_actual_entity_value(
    DNX_SAND_IN DNX_SAND_DEVICE_ENTITY device_entity
  )
{
  uint32
    res ;
  /*
   */
  res = 0 ;
  switch(device_entity)
  {
    case DNX_SAND_FE1_ENTITY:
    {
      res = DNX_SAND_ACTUAL_FE1_VALUE;
      break;
    }
    case DNX_SAND_FE2_ENTITY:
    {
      res = DNX_SAND_ACTUAL_FE2_VALUE;
      break;
    }
    case DNX_SAND_FE3_ENTITY:
    {
      res = DNX_SAND_ACTUAL_FE3_VALUE;
      break;
    }
    case DNX_SAND_FAP_ENTITY:
    {
      res = DNX_SAND_ACTUAL_FAP_VALUE;
      break;
    }
    case DNX_SAND_FOP_ENTITY:
    {
      res = DNX_SAND_ACTUAL_FOP_VALUE;
      break;
    }
    case DNX_SAND_FIP_ENTITY:
    {
      res = DNX_SAND_ACTUAL_FIP_VALUE;
      break;
    }
    case DNX_SAND_DONT_CARE_ENTITY:
    case DNX_SAND_FE13_ENTITY:
    {
      /*
       * In the context of cells, there is no.
       * FE13 or DONT_CARE entity.
       */
      res = DNX_SAND_ACTUAL_BAD_VALUE;
      break;
    }
    default:
    {
      /*
       * This is the case of bad use of the method.
       * (Input value is out of range)
       */
      res = DNX_SAND_ACTUAL_OUT_OF_RANGE;
      break;
    }
  }
  return res ;
}


/*
 */
/*****************************************************
*NAME:
*  dnx_sand_real_entity_value
*DATE:
*  12/NOV/2002
*FUNCTION:
*  translates the FE1_ENTITY, FE2_ENTITY, etc,
*  from the real 3 bit field in the cell
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_IN DNX_SAND_ENTITY_LEVEL_TYPE device_entity_3b
*       the three bit field in the cell
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    The real entity value (FE1, FE2, etc.)
*  DNX_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
DNX_SAND_DEVICE_ENTITY
  dnx_sand_real_entity_value(
    DNX_SAND_IN DNX_SAND_ENTITY_LEVEL_TYPE device_entity_3b
  )
{
  uint32
    res ;
  /*
   */
  res = 0 ;
  switch(device_entity_3b)
  {
    case DNX_SAND_ACTUAL_FE1_VALUE:
    {
      res = DNX_SAND_FE1_ENTITY;
      break;
    }
    case DNX_SAND_ACTUAL_FE2_VALUE:
    {
      res = DNX_SAND_FE2_ENTITY;
      break;
    }
    case DNX_SAND_ACTUAL_FE3_VALUE:
    {
      res = DNX_SAND_FE3_ENTITY;
      break;
    }
    case DNX_SAND_ACTUAL_FAP_VALUE:
    {
      res = DNX_SAND_FAP_ENTITY;
      break;
    }
    case DNX_SAND_ACTUAL_FOP_VALUE:
    {
      res = DNX_SAND_FOP_ENTITY;
      break;
    }
    case DNX_SAND_ACTUAL_FIP_VALUE:
    {
      res = DNX_SAND_FIP_ENTITY;
      break;
    }
    default:
    {
      /*
       * This is the case of bad use of the method.
       * (Input value is out of range)
       */
      res = DNX_SAND_REAL_ENTITY_VALUE_OUT_OF_RANGE;
      break;
    }
  }
  return res ;
}
