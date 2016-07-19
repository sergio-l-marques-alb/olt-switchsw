/* $Id: petra_reg_access.c,v 1.8 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* $Id: petra_reg_access.c,v 1.8 Broadcom SDK $
 *	A value marking invalid register in the device
 */
#define SOC_PETRA_REG_VAL_INVALID   SOC_SAND_REG_VAL_INVALID
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

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */
const uint32 Soc_petra_regs_srd_offset[SOC_PETRA_REGS_SRD_NOF_INSTANCES] =
{
  0x5100 * sizeof(uint32), /* STAR 0 */
  0x5500 * sizeof(uint32), /* STAR 1 */
  0x4900 * sizeof(uint32), /* STAR 2 */
  0x4980 * sizeof(uint32)  /* STAR 3 */
};
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
*  soc_petra_field_from_reg_get
*TYPE:
*  PROC
*DATE:
*  01/10/2007
*FUNCTION:
*  Gets field bits from an input buffer and puts them
*  in the output buffer
*INPUT:
*  SOC_SAND_IN  uint32          *reg_buffer,
*    Input buffer from which the function reads -
*    the register to read
*  SOC_SAND_IN  SOC_PETRA_REG_FIELD  *field,
*    The field from which the bits are taken
*  SOC_SAND_IN  uint32          *fld_buffer
*    Output buffer to which the function writes -
*    the field to write.
*OUTPUT:
*REMARKS:
*  The fld_buffer is zerrowed before writing to it.
*  In case of a 32-bit buffer (single register),
*  the return value will not be dependant on the previous
*  value of the buffer. Otherwise - if the buffer size
*  is larger then the field size,
*  it should be zerrowed by the user.
*****************************************************/
uint32
  soc_petra_field_from_reg_get(
    SOC_SAND_IN  uint32          *reg_buffer,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD   *field,
    SOC_SAND_OUT uint32          *fld_buffer
  )
{
  uint32
    out_buff = 0,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FIELD_FROM_REG_GET);

  SOC_SAND_CHECK_NULL_INPUT(reg_buffer);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(fld_buffer);

  res = soc_sand_bitstream_get_any_field(
          reg_buffer,
          SOC_PETRA_FLD_LSB(*field),
          SOC_PETRA_FLD_NOF_BITS(*field),
          &out_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *fld_buffer = out_buff;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_field_from_reg_get",0,0);
}

/*****************************************************
*NAME
*  soc_petra_field_from_reg_set
*TYPE:
*  PROC
*DATE:
*  01/10/2007
*FUNCTION:
*  Sets field bits in an output buffer after read from
*  an input buffer.
*INPUT:
*  SOC_SAND_IN  uint32          *fld_buffer,
*    Input buffer from which the function reads -
*    the field to set.
*  SOC_SAND_IN  SOC_PETRA_REG_FIELD  *field,
*    The field from which the bits are taken
*  SOC_SAND_IN  uint32          *reg_buffer
*    Output buffer to which the function writes -
*    the register to set with the field
*OUTPUT:
* None.
*REMARKS:
* Only the relevant bits in the output reg_buffer are changed -
* the rest is left as is.
*
*****************************************************/
uint32
  soc_petra_field_from_reg_set(
    SOC_SAND_IN  uint32          *fld_buffer,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD   *field,
    SOC_SAND_OUT uint32          *reg_buffer
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FIELD_FROM_REG_SET);

  SOC_SAND_CHECK_NULL_INPUT(fld_buffer);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(reg_buffer);

  res = soc_sand_bitstream_set_any_field(
          fld_buffer,
          SOC_PETRA_FLD_LSB(*field),
          SOC_PETRA_FLD_NOF_BITS(*field),
          reg_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_field_from_reg_get",0,0);
}

/*****************************************************
*NAME
*  soc_petra_read_fld_unsafe
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Read a field from Soc_petra register
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field - pointer to the requested
*                               field descriptor in registers database
*    SOC_SAND_IN  unsigned int      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32*         val - return value.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    val - the value of the requested field.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_read_fld_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field,
    SOC_SAND_IN  uint32      instance_idx,
    SOC_SAND_OUT uint32*         val
  )
{
  uint32
    interface_index = 0,
    res = SOC_SAND_OK,
    offset = 0,
    reg_val = 0,
    fld_val = 0;
  uint8
    is_srd;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_READ_FLD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(val);

  is_srd = SOC_SAND_NUM2BOOL(SOC_SAND_IS_VAL_IN_RANGE(instance_idx, SOC_PETRA_REGS_SRD_INST_BASE+1, SOC_PETRA_REGS_SRD_INST_LAST-1));

  if ((field->addr.step == 0) && (instance_idx != SOC_PETRA_DEFAULT_INSTANCE) && !(is_srd))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR, 5, exit);
  }

  if( (field->addr.step != 0) && (instance_idx == SOC_PETRA_DEFAULT_INSTANCE) )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR, 10,exit);
  }
  else if(instance_idx == SOC_PETRA_DEFAULT_INSTANCE)
  {
    interface_index = 0;
  }
  else
  {
    interface_index = instance_idx;
  }

  offset = SOC_PETRA_GET_ADDR(field->addr.base, field->addr.step, interface_index);

  if (is_srd)
  {
    offset += Soc_petra_regs_srd_offset[instance_idx - SOC_PETRA_REGS_SRD_INST_BASE - 1];
  }

  res = soc_sand_mem_read_unsafe(
          unit,
          &(reg_val),
          offset,
          sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

#if SOC_PETRA_DEBUG_IS_LVL2
  if (reg_val == SOC_PETRA_REG_VAL_INVALID)
  {
    soc_sand_os_printf(
      "ECI ACCESS WARNING: Read invalid value (0x%08x) at Address: 0x%04x\n\r",
      SOC_PETRA_REG_VAL_INVALID,
      offset/SOC_SAND_REG_SIZE_BYTES
    );
  }
#endif

  res = soc_petra_field_from_reg_get(
          &(reg_val),
          field,
          &(fld_val)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *val = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_read_fld_unsafe",0,0);
}

/*****************************************************
*NAME
*  soc_petra_write_fld_unsafe
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Write to a field of Soc_petra register
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field - pointer to the requested
*                               field descriptor in registers database
*    SOC_SAND_IN  unsigned int      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_IN uint32           val - the value to write.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_write_fld_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field,
    SOC_SAND_IN  uint32      instance_idx,
    SOC_SAND_IN  uint32          val
  )
{
  uint32
    max_fld_val = 0,
    interface_index = 0,
    res = SOC_SAND_OK;
  uint32
    offset = 0;
  uint8
    is_srd;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_WRITE_FLD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(field);

  max_fld_val = SOC_PETRA_FLD_MAX(*field);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    val, max_fld_val,
    SOC_PETRA_REGS_FIELD_VAL_OUT_OF_RANGE_ERR, 10, exit
  );

  is_srd = SOC_SAND_NUM2BOOL(SOC_SAND_IS_VAL_IN_RANGE(instance_idx, SOC_PETRA_REGS_SRD_INST_BASE+1, SOC_PETRA_REGS_SRD_INST_LAST-1));

  if ((field->addr.step == 0) && (instance_idx != SOC_PETRA_DEFAULT_INSTANCE) && !(is_srd))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR, 15, exit);
  }

  if( (field->addr.step != 0) && (instance_idx == SOC_PETRA_DEFAULT_INSTANCE) )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR, 20, exit);
  }
  else if(instance_idx == SOC_PETRA_DEFAULT_INSTANCE)
  {
    interface_index = 0;
  }
  else
  {
    interface_index = instance_idx;
  }

  offset = SOC_PETRA_GET_ADDR(field->addr.base, field->addr.step, interface_index);

  if (is_srd)
  {
    offset += Soc_petra_regs_srd_offset[instance_idx - SOC_PETRA_REGS_SRD_INST_BASE - 1];
  }

  res = soc_sand_read_modify_write(
          soc_sand_get_chip_descriptor_base_addr(unit),
          offset,
          field->lsb,
          SOC_SAND_BITS_MASK(field->msb, field->lsb),
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_write_fld_unsafe",offset,val);
}

/*****************************************************
*NAME
*  soc_petra_read_reg_unsafe
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Read Soc_petra register
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_ADDR*   reg - pointer to the requested
*                               register descriptor in registers database
*    SOC_SAND_IN  unsigned int      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32*         val - return value.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    val - the value of the requested register.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_read_reg_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR*   reg,
    SOC_SAND_IN  uint32      instance_idx,
    SOC_SAND_OUT uint32*         val
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset = 0;
  uint8
    is_srd;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_READ_REG_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(reg);
  SOC_SAND_CHECK_NULL_INPUT(val);

  is_srd = SOC_SAND_NUM2BOOL(SOC_SAND_IS_VAL_IN_RANGE(instance_idx, SOC_PETRA_REGS_SRD_INST_BASE+1, SOC_PETRA_REGS_SRD_INST_LAST-1));

  if ((reg->step == 0) && (instance_idx != SOC_PETRA_DEFAULT_INSTANCE) && !(is_srd))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR, 5, exit);
  }

  if( (reg->step != 0) && (instance_idx == SOC_PETRA_DEFAULT_INSTANCE))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR, 10,exit);
  }

  offset = SOC_PETRA_GET_ADDR(reg->base, reg->step, instance_idx);

  if (is_srd)
  {
    offset += Soc_petra_regs_srd_offset[instance_idx - SOC_PETRA_REGS_SRD_INST_BASE - 1];
  }

  res = soc_sand_mem_read_unsafe(
          unit,
          val,
          offset,
          sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

#if SOC_PETRA_DEBUG_IS_LVL2
  if (*val == SOC_PETRA_REG_VAL_INVALID)
  {
    soc_sand_os_printf(
      "ECI ACCESS WARNING: Read invalid value (0x%08x) at Address: 0x%04x\n\r",
      SOC_PETRA_REG_VAL_INVALID,
      offset/SOC_SAND_REG_SIZE_BYTES
    );
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_read_reg_unsafe",0,0);
}

/*****************************************************
*NAME
*  soc_petra_write_reg_unsafe
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Write Soc_petra register
*INPUT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_ADDR*   reg - pointer to the requested
*                               register descriptor in registers database
*    SOC_SAND_IN  unsigned int      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_IN uint32          val - the register value to write.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_write_reg_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR*   reg,
    SOC_SAND_IN  uint32      instance_idx,
    SOC_SAND_IN  uint32          val
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset = 0;
  uint8
    is_srd;
  uint32
     val_lcl[1];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_WRITE_REG_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(reg);

  is_srd = SOC_SAND_NUM2BOOL(SOC_SAND_IS_VAL_IN_RANGE(instance_idx, SOC_PETRA_REGS_SRD_INST_BASE+1, SOC_PETRA_REGS_SRD_INST_LAST-1));

  if ((reg->step == 0) && (instance_idx != SOC_PETRA_DEFAULT_INSTANCE) && !(is_srd))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR, 5, exit);
  }

  if( (reg->step != 0) && (instance_idx == SOC_PETRA_DEFAULT_INSTANCE))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR, 10,exit);
  }

  offset = SOC_PETRA_GET_ADDR(reg->base, reg->step, instance_idx);

  if (is_srd)
  {
    offset += Soc_petra_regs_srd_offset[instance_idx - SOC_PETRA_REGS_SRD_INST_BASE - 1];
  }

  *val_lcl = val;
  res = soc_sand_mem_write_unsafe(
          unit,
          val_lcl,
          offset,
          sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_write_reg_unsafe",0,0);
}

uint32
  soc_petra_read_reg_buffer_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR  *start_reg,
    SOC_SAND_IN  uint32        instance_idx,
    SOC_SAND_IN  uint32        nof_regs,
    SOC_SAND_OUT uint32         *val
  )
{
  uint32
    interface_index = instance_idx,
    res = SOC_SAND_OK;
  uint32
    offset = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_READ_REG_BUFFER_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(start_reg);
  SOC_SAND_CHECK_NULL_INPUT(val);

  offset = SOC_PETRA_GET_ADDR(start_reg->base, start_reg->step, interface_index);

  res = soc_sand_mem_read_unsafe(
          unit,
          val,
          offset,
          nof_regs * sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_read_reg_buffer_unsafe",0,0);
}

uint32
  soc_petra_write_reg_buffer_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR      *start_reg_addr,
    SOC_SAND_IN  uint32           instance_idx,
    SOC_SAND_IN  uint32           nof_regs,
    SOC_SAND_IN  uint32            *val
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_WRITE_REG_BUFFER_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(val);
  SOC_SAND_CHECK_NULL_INPUT(start_reg_addr);

  offset =
    SOC_PETRA_GET_ADDR(start_reg_addr->base, start_reg_addr->step, instance_idx);

  res = soc_sand_mem_write_unsafe(
          unit,
          val,
          offset,
          nof_regs * sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_write_reg_unsafe",0,0);
}

/**************************************************************************
* NAME:
*     soc_petra_write_array_of_flds
* TYPE:
*   PROC
* DATE:
*   Sep 25 2007
* FUNCTION:
* Write an array of fields to one register or sequence of registers.
* INPUT:
* SOC_SAND_IN  int           unit -
* SOC_SAND_IN  uint32                  base_reg -
*   base address of the first register in the sequence.
* SOC_SAND_IN  SOC_PETRA_REG_FIELD                 *flds_bits-
*   array of fields, each entry contains
*         - address of the register contains the field
*         - msb and lsb of the field.
* SOC_SAND_IN  uint32           nof_flds
*  number of fields in fld_vals array.
* SOC_SAND_IN  uint32*                fld_vals-
*   value to be written for each field respectively i.e.
*   fld_vals[i] to be written to flds_bits[i].lsb - flds_bits[i].msb .
* RETURNS:
*   NONE
* REMARKS:
*  - field assume to be not splited to more than one register.
*  - field lsb and msb is relative to the registers contains it.
*********************************************************************/
uint32
  soc_petra_write_array_of_flds(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR      *base_reg,
    SOC_SAND_IN SOC_PETRA_REG_FIELD      *flds_bits,
    SOC_SAND_IN  uint32           nof_flds,
    SOC_SAND_IN  uint32            *fld_vals
  )
{
  uint32
    reg_value;
  uint32
    indx;
  SOC_PETRA_REG_ADDR
    curr_reg;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_WRITE_ARRAY_OF_FLDS);

  SOC_SAND_CHECK_NULL_INPUT(base_reg);
  SOC_SAND_CHECK_NULL_INPUT(flds_bits);
  SOC_SAND_CHECK_NULL_INPUT(fld_vals);

  curr_reg.base = base_reg->base;
  curr_reg.step = base_reg->step;

  indx = 0;
  while (indx < nof_flds)
  {
      res = soc_petra_read_reg_unsafe(
              unit,
              &curr_reg,
              SOC_PETRA_DEFAULT_INSTANCE,
              &reg_value
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    while ((flds_bits[indx].addr.base == curr_reg.base) && (indx < nof_flds))
    {
      res = soc_sand_set_field(
              &reg_value,
              flds_bits[indx].msb,
              flds_bits[indx].lsb,
              fld_vals[indx]
            );
     SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      ++ indx;
    }

      res = soc_petra_write_reg_unsafe(
              unit,
              &curr_reg,
              SOC_PETRA_DEFAULT_INSTANCE,
              reg_value
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (indx < nof_flds)
    {
      curr_reg.base = flds_bits[indx].addr.base;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_write_array_of_flds()",0,0);

}

/**************************************************************************
* NAME:
*     soc_petra_read_array_of_flds
* TYPE:
*   PROC
* DATE:
*   Sep 25 2007
* FUNCTION:
*   Read an array of fields from one register or sequence of registers.
* INPUT:
* SOC_SAND_IN  int                 unit -
* SOC_SAND_IN  uint32                  base_reg -
*   base address of the first register in the sequence.
* SOC_SAND_IN  SOC_PETRA_REG_FIELD                 *flds_bits-
*   array of fields, each entry contains
*         - address of the register that contains the field
*         - msb and lsb of the field.
* SOC_SAND_IN  uint32           nof_flds
*   number of fields in fld_vals array.
* SOC_SAND_IN  uint32*                fld_vals-
*   value to be written for each field respectively i.e.
*   fld_vals[i] to be written to flds_bits[i].lsb - flds_bits[i].msb .
* RETURNS:
*   NONE
* REMARKS:
*  - field assume to be not splited to more than one register.
*  - field lsb and msb is relative to the register that contains it.
*********************************************************************/
uint32
  soc_petra_read_array_of_flds(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR      *base_reg,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD     *flds_bits,
    SOC_SAND_IN  uint32           nof_flds,
    SOC_SAND_OUT uint32            *fld_vals
  )
{
  uint32
    reg_value;
  uint32
    indx;
  SOC_PETRA_REG_ADDR
    curr_reg;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_READ_ARRAY_OF_FLDS);

  SOC_SAND_CHECK_NULL_INPUT(base_reg);
  SOC_SAND_CHECK_NULL_INPUT(flds_bits);
  SOC_SAND_CHECK_NULL_INPUT(fld_vals);

  curr_reg.base = base_reg->base;
  curr_reg.step = base_reg->step;

  indx = 0;
  while (indx < nof_flds)
    {
      res = soc_petra_read_reg_unsafe(
              unit,
              &curr_reg,
              SOC_PETRA_DEFAULT_INSTANCE,
              &reg_value
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    while ((flds_bits[indx].addr.base == curr_reg.base) && (indx < nof_flds))
    {
      fld_vals[indx] =
        SOC_SAND_GET_BITS_RANGE(
          reg_value,
           flds_bits[indx].msb,
           flds_bits[indx].lsb
        );
      ++ indx;
    }
    if (indx < nof_flds)
    {
      curr_reg.base = flds_bits[indx].addr.base;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_read_array_of_flds()",0,0);

}

#ifdef LINK_PA_LIBRARIES
SOC_PA_REGS*
  soc_pa_regs(void)
{
  SOC_PA_REGS*
    regs = NULL;

  soc_pa_regs_get(
    &regs
  );

  return regs;
}
#endif

SOC_PETRA_REGS*
  soc_petra_regs()
{
  SOC_PETRA_REGS*
    regs = NULL;

  soc_petra_regs_get(
    &(regs)
  );

  return regs;
}


#ifdef LINK_PA_LIBRARIES
SOC_PETRA_ECI_REGS*
  soc_petra_eci_regs()
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_ECI_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->eci);

  return blk_regs;
}

SOC_PETRA_OLP_REGS*
  soc_petra_olp_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_OLP_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->olp);

  return blk_regs;
}

SOC_PETRA_NIF_REGS*
  soc_petra_nif_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_NIF_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->nif);

  return blk_regs;
}

SOC_PETRA_NIF_MAC_LANE_REGS*
  soc_petra_nif_mac_lane_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_NIF_MAC_LANE_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->nif_mac_lane);

  return blk_regs;
}

SOC_PETRA_IRE_REGS*
  soc_petra_ire_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_IRE_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->ire);

  return blk_regs;
}

SOC_PETRA_IDR_REGS*
  soc_petra_idr_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_IDR_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->idr);

  return blk_regs;
}

SOC_PETRA_IRR_REGS*
  soc_petra_irr_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_IRR_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->irr);

  return blk_regs;
}

SOC_PETRA_IHP_REGS*
  soc_petra_ihp_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_IHP_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->ihp);

  return blk_regs;
}

SOC_PETRA_IQM_REGS*
  soc_petra_iqm_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_IQM_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->iqm);

  return blk_regs;
}

SOC_PETRA_QDR_REGS*
  soc_petra_qdr_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_QDR_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->qdr);

  return blk_regs;
}

SOC_PETRA_IPS_REGS*
  soc_petra_ips_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_IPS_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->ips);

  return blk_regs;
}

SOC_PETRA_IPT_REGS*
  soc_petra_ipt_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_IPT_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->ipt);

  return blk_regs;
}

SOC_PETRA_MMU_REGS*
  soc_petra_mmu_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_MMU_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->mmu);

  return blk_regs;
}

SOC_PETRA_DRC_REGS*
  soc_petra_drc_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_DRC_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->drc);

  return blk_regs;
}

SOC_PETRA_DPI_REGS*
  soc_petra_dpi_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_DPI_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->dpi);

  return blk_regs;
}

SOC_PETRA_FDR_REGS*
  soc_petra_fdr_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_FDR_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->fdr);

  return blk_regs;
}

SOC_PETRA_FDT_REGS*
  soc_petra_fdt_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_FDT_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->fdt);

  return blk_regs;
}

SOC_PETRA_FCR_REGS*
  soc_petra_fcr_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_FCR_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->fcr);

  return blk_regs;
}

SOC_PETRA_FCT_REGS*
  soc_petra_fct_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_FCT_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->fct);

  return blk_regs;
}

SOC_PETRA_MESH_TOPOLOGY_REGS*
  soc_petra_mesh_topology_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_MESH_TOPOLOGY_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->mesh_topology);

  return blk_regs;
}

SOC_PETRA_RTP_REGS*
  soc_petra_rtp_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_RTP_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->rtp);

  return blk_regs;
}

SOC_PETRA_FABRIC_MAC_REGS*
  soc_petra_fabric_mac_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_FABRIC_MAC_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->fabric_mac);

  return blk_regs;
}

SOC_PETRA_MSW_REGS*
  soc_petra_msw_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_MSW_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->msw);

  return blk_regs;
}

SOC_PETRA_EGQ_REGS*
  soc_petra_egq_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_EGQ_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->egq);

  return blk_regs;
}

SOC_PETRA_CFC_REGS*
  soc_petra_cfc_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_CFC_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->cfc);

  return blk_regs;
}

SOC_PETRA_SCH_REGS*
  soc_petra_sch_regs(void)
{
  SOC_PA_REGS* regs = NULL;
  SOC_PETRA_SCH_REGS* blk_regs = NULL;

  regs = soc_pa_regs();
  blk_regs = (regs == NULL) ? NULL: &(regs->sch);

  return blk_regs;
}
#endif

/*********************************************************************
*     Preform polling on the value of a given register.
*     According to the poll_info structure, the polling is
*     first performed as busy-wait, repeatedly reading the
*     register. Then, a polling is performed with the
*     requested timer delay between consecutive reads. The
*     'success' parameter returns TRUE if the expected value
*     is read, and FALSE if all the polling iterations do not
*     read the expected value (timeout).
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_status_fld_poll_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD           *field,
    SOC_SAND_IN  uint32                 instance_ndx,
    SOC_SAND_IN  SOC_PETRA_POLL_INFO           *poll_info,
    SOC_SAND_OUT uint8                 *success
  )
{
  uint32
    fld_val,
    res;
  uint32
    wait_iter = 0;
  uint8
    is_timeout = FALSE,
    is_low_sim_active;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STATUS_FLD_POLL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(poll_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  if (!is_low_sim_active)
  {
    do {
         if (instance_ndx == SOC_PETRA_DEFAULT_INSTANCE)
         {
           SOC_PETRA_IMPLICIT_FLD_GET(*field, fld_val, 10, exit);
         }
         else
         {
           SOC_PETRA_IMPLICIT_FLD_IGET(*field, fld_val, instance_ndx, 20, exit);
         }
         if (fld_val != poll_info->expected_value)
         {
           if (wait_iter > poll_info->busy_wait_nof_iters)
           {
             /*
              *  We passed the busy wait iterations -
              * start the timer-delayed iterations.
              */
             if (wait_iter <=
                 (poll_info->busy_wait_nof_iters +
                  poll_info->timer_nof_iters)
                )
             {
               sal_msleep(poll_info->timer_delay_msec);
             }
             else
             {
               /*
                *  We passed both the busy wait iterations
                * and the timer-delayed iterations.
                * Declare timeout and exit the loop.
                */
               is_timeout = TRUE;
             }
           }
         }

         wait_iter++;
       } while((fld_val != poll_info->expected_value) && (is_timeout == FALSE));
  }

  *success = SOC_SAND_NUM2BOOL_INVERSE(is_timeout);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_status_fld_poll_unsafe()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
