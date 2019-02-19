/* $Id: pcp_reg_access.c,v 1.8 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PCP/pcp_api_framework.h>
#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_reg_access.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>

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

#define PCP_GET_ADDR(_base_addr_, _step_, _idx_, reg_indx)                       \
          ((Pcp_base_offset[unit] + _base_addr_) + (_step_)*(_idx_) + (4*reg_indx))

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

static uint32
  Pcp_base_offset[SOC_SAND_MAX_DEVICE];

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  pcp_base_offset_set_unsafe(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32           base_offset
  )
{

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_BASE_OFFSET_SET_UNSAFE);

  Pcp_base_offset[unit] = base_offset;

  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_base_offset_set_unsafe",0,0);
}

uint32
  pcp_field_from_reg_get(
    SOC_SAND_IN  uint32           *reg_buffer,
    SOC_SAND_IN  PCP_REG_FIELD      *field,
    SOC_SAND_OUT uint32           *fld_buffer
  )
{
  uint32
    out_buff = 0,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FIELD_FROM_REG_GET);

  SOC_SAND_CHECK_NULL_INPUT(reg_buffer);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(fld_buffer);

  res = soc_sand_bitstream_get_any_field(
          reg_buffer,
          PCP_FLD_LSB(*field),
          PCP_FLD_NOF_BITS(*field),
          &out_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *fld_buffer = out_buff;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_field_from_reg_get",0,0);
}

uint32
  pcp_field_from_reg_set(
    SOC_SAND_IN  uint32           *fld_buffer,
    SOC_SAND_IN  PCP_REG_FIELD      *field,
    SOC_SAND_OUT uint32           *reg_buffer
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FIELD_FROM_REG_SET);

  SOC_SAND_CHECK_NULL_INPUT(fld_buffer);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(reg_buffer);

  res = soc_sand_bitstream_set_any_field(
          fld_buffer,
          PCP_FLD_LSB(*field),
          PCP_FLD_NOF_BITS(*field),
          reg_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_field_from_reg_get",0,0);
}

uint32
  pcp_read_fld_unsafe(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  PCP_REG_FIELD      *field,
    SOC_SAND_IN  uint32          instance_idx,
    SOC_SAND_OUT uint32           *val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_READ_FLD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(val);

  res = pcp_read_fld_at_indx_unsafe(
          unit,
          field,
          0,
          instance_idx,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_read_fld_unsafe",0,0);
}

uint32
  pcp_write_fld_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_FIELD     *field,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_IN uint32           val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_WRITE_FLD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(field);

  res = pcp_write_fld_at_indx_unsafe(
          unit,
          field,
          0,
          instance_idx,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_write_fld_unsafe",0,0);
}

uint32
  pcp_read_reg_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_ADDR      *reg,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_OUT uint32          *val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_READ_REG_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(reg);
  SOC_SAND_CHECK_NULL_INPUT(val);

  res = pcp_read_reg_at_indx_unsafe(
          unit,
          reg,
          0,
          instance_idx,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_read_reg_unsafe",0,0);
}

uint32
  pcp_write_reg_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_ADDR      *reg,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_IN  uint32          val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_WRITE_REG_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(reg);

  res = pcp_write_reg_at_indx_unsafe(
          unit,
          reg,
          0,
          instance_idx,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_write_reg_unsafe",0,0);
}

uint32
  pcp_read_fld_at_indx_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_FIELD     *field,
    SOC_SAND_IN  uint32          fld_indx,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_OUT uint32          *val
  )
{
  uint32
    interface_index = 0,
    res = SOC_SAND_OK;
  uint32
    nof_regs,
    buff_sta,
    *buff_dyn = NULL,
    *buffer = NULL;
  PCP_REG_FIELD
    act_field;
  uint32
    offset = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_READ_FLD_AT_INDX_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(val);

  if ((field->step == 0) && (instance_idx != PCP_DEFAULT_INSTANCE))
  {
    SOC_SAND_SET_ERROR_CODE(PCP_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR, 5, exit);
  }

  if( (field->step != 0) && (instance_idx == PCP_DEFAULT_INSTANCE) )
  {
    SOC_SAND_SET_ERROR_CODE(PCP_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR, 10,exit);
  }

  interface_index = (instance_idx == PCP_DEFAULT_INSTANCE ? 0 : interface_index);
  offset = PCP_GET_ADDR(field->base, field->step, interface_index,0);

  act_field.base = field->base;
  act_field.size = field->size;
  act_field.step = field->step;
  act_field.lsb  = (uint16) (field->lsb + (field->msb - field->lsb + 1) * fld_indx);
  act_field.msb  = (uint16) (field->msb + (field->msb - field->lsb + 1) * fld_indx);

  if ((nof_regs = (act_field.msb / SOC_SAND_NOF_BITS_IN_UINT32) - (act_field.lsb / SOC_SAND_NOF_BITS_IN_UINT32) + 1) == 1)
  {
    buffer = &buff_sta;
  }
  else
  {
    PCP_ALLOC(buff_dyn, uint32, nof_regs, "buff_dyn");
    buffer = buff_dyn;
  }

  res = soc_sand_mem_read_unsafe(
          unit,
          buffer,
          offset,
          nof_regs * sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = pcp_field_from_reg_get(
          buffer,
          field,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  PCP_FREE(buff_dyn);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_read_fld_at_indx_unsafe",0,0);
}

uint32
  pcp_write_fld_at_indx_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_FIELD     *field,
    SOC_SAND_IN  uint32          fld_indx,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_IN  uint32          val
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    max_fld_val = 0,
    interface_index = 0;
  uint32
    nof_regs,
    buff_sta[1],
    *buff_dyn = NULL,
    *buffer = NULL;
  PCP_REG_FIELD
    act_field;
  uint32
    offset = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_WRITE_FLD_AT_INDX_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(field);

  if ((field->step == 0) && (instance_idx != PCP_DEFAULT_INSTANCE))
  {
    SOC_SAND_SET_ERROR_CODE(PCP_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR, 15, exit);
  }

  if( (field->step != 0) && (instance_idx == PCP_DEFAULT_INSTANCE) )
  {
    SOC_SAND_SET_ERROR_CODE(PCP_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR, 20, exit);
  }

  interface_index = (instance_idx == PCP_DEFAULT_INSTANCE ? 0 : interface_index);
  offset = PCP_GET_ADDR(field->base, field->step, interface_index, 0);

  act_field.base = field->base;
  act_field.size = field->size;
  act_field.step = field->step;
  act_field.lsb = (uint8) (field->lsb + (field->msb - field->lsb + 1) * fld_indx);
  act_field.msb = (uint8) (field->msb + (field->msb - field->lsb + 1) * fld_indx);
  
  if (act_field.msb - act_field.lsb + 1 < SOC_SAND_NOF_BITS_IN_UINT32)
  {
    max_fld_val = PCP_FLD_MAX(*field);
    SOC_SAND_ERR_IF_ABOVE_MAX(val, max_fld_val, PCP_VAL_IS_OUT_OF_RANGE_ERR, 10, exit);
  }

  if ((nof_regs = (act_field.msb / SOC_SAND_NOF_BITS_IN_UINT32) - (act_field.lsb / SOC_SAND_NOF_BITS_IN_UINT32) + 1) == 1)
  {
    buffer = buff_sta;
  }
  else
  {
    PCP_ALLOC(buff_dyn, uint32, nof_regs, "buff_dyn");
    buffer = buff_dyn;
  }

  res = soc_sand_mem_read_unsafe(
          unit,
          buffer,
          offset + (act_field.lsb / SOC_SAND_NOF_BITS_IN_UINT32) * sizeof(uint32),
          nof_regs * sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = pcp_field_from_reg_set(
          &val,
          &act_field,
          buffer - (act_field.lsb / SOC_SAND_NOF_BITS_IN_UINT32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_mem_write_unsafe(
          unit,
          buffer,
          offset + (act_field.lsb / SOC_SAND_NOF_BITS_IN_UINT32) * sizeof(uint32),
          nof_regs * sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  PCP_FREE(buff_dyn);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_write_fld_at_indx_unsafe",offset,val);
}

uint32
  pcp_read_reg_at_indx_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  PCP_REG_ADDR         *reg,
    SOC_SAND_IN  uint32             reg_indx,
    SOC_SAND_IN  uint32            instance_idx,
    SOC_SAND_OUT uint32             *val
  )
{
  uint32
    interface_index = 0,
    res = SOC_SAND_OK;
  uint32
    offset = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_READ_REG_AT_INDX_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(reg);
  SOC_SAND_CHECK_NULL_INPUT(val);

  if ((reg->step == 0) && (instance_idx != PCP_DEFAULT_INSTANCE))
  {
    SOC_SAND_SET_ERROR_CODE(PCP_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR, 5, exit);
  }

  if( (reg->step != 0) && (instance_idx == PCP_DEFAULT_INSTANCE))
  {
    SOC_SAND_SET_ERROR_CODE(PCP_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR, 10,exit);
  }

  interface_index = (instance_idx == PCP_DEFAULT_INSTANCE ? 0 : interface_index);
  offset = PCP_GET_ADDR(reg->base, reg->step, instance_idx, reg_indx);

  res = soc_sand_mem_read_unsafe(
          unit,
          val,
          offset,
          sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_read_reg_at_indx_unsafe",0,0);
}

uint32
  pcp_write_reg_at_indx_unsafe(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  PCP_REG_ADDR       *reg,
    SOC_SAND_IN  uint32           reg_indx,
    SOC_SAND_IN  uint32          instance_idx,
    SOC_SAND_IN  uint32           val
  )
{
  uint32
    interface_index = 0,
      val_lcl[1],
    res = SOC_SAND_OK;
  uint32
    offset = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_WRITE_REG_AT_INDX_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(reg);

  if ((reg->step == 0) && (instance_idx != PCP_DEFAULT_INSTANCE))
  {
    SOC_SAND_SET_ERROR_CODE(PCP_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR, 5, exit);
  }

  if( (reg->step != 0) && (instance_idx == PCP_DEFAULT_INSTANCE))
  {
    SOC_SAND_SET_ERROR_CODE(PCP_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR, 10,exit);
  }

  interface_index = (instance_idx == PCP_DEFAULT_INSTANCE ? 0 : interface_index);
  offset = PCP_GET_ADDR(reg->base, reg->step, instance_idx, reg_indx);

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
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_write_reg_at_indx_unsafe",0,0);
}

uint32
  pcp_read_reg_buffer_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  PCP_REG_ADDR    *start_reg,
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_READ_REG_BUFFER_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(start_reg);
  SOC_SAND_CHECK_NULL_INPUT(val);

  offset = PCP_GET_ADDR(start_reg->base, start_reg->step, interface_index, 0);

  res = soc_sand_mem_read_unsafe(
          unit,
          val,
          offset,
          nof_regs * sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_read_reg_buffer_unsafe",0,0);
}

uint32
  pcp_write_reg_buffer_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  PCP_REG_ADDR        *start_reg_addr,
    SOC_SAND_IN  uint32            instance_idx,
    SOC_SAND_IN  uint32            nof_regs,
    SOC_SAND_IN  uint32             *val
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_WRITE_REG_BUFFER_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(val);
  SOC_SAND_CHECK_NULL_INPUT(start_reg_addr);

  offset = PCP_GET_ADDR(start_reg_addr->base, start_reg_addr->step, instance_idx,0);

  res = soc_sand_mem_write_unsafe(
          unit,
          val,
          offset,
          nof_regs * sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_write_reg_unsafe",0,0);
}

uint32
  pcp_write_array_of_flds(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  PCP_REG_ADDR      *base_reg,
    SOC_SAND_IN PCP_REG_FIELD      *flds_bits,
    SOC_SAND_IN  uint32           nof_flds,
    SOC_SAND_IN  uint32            *fld_vals
  )
{
  uint32
    reg_value;
  uint32
    indx;
  PCP_REG_ADDR
    curr_reg;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_WRITE_ARRAY_OF_FLDS);

  SOC_SAND_CHECK_NULL_INPUT(base_reg);
  SOC_SAND_CHECK_NULL_INPUT(flds_bits);
  SOC_SAND_CHECK_NULL_INPUT(fld_vals);

  curr_reg.base = base_reg->base;
  curr_reg.step = base_reg->step;

  indx = 0;
  while (indx < nof_flds)
  {
      res = pcp_read_reg_unsafe(
              unit,
              &curr_reg,
              PCP_DEFAULT_INSTANCE,
              &reg_value
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    while ((flds_bits[indx].base == curr_reg.base) && (indx < nof_flds))
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

      res = pcp_write_reg_unsafe(
              unit,
              &curr_reg,
              PCP_DEFAULT_INSTANCE,
              reg_value
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (indx < nof_flds)
    {
      curr_reg.base = flds_bits[indx].base;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_write_array_of_flds()",0,0);

}

uint32
  pcp_read_array_of_flds(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  PCP_REG_ADDR      *base_reg,
    SOC_SAND_IN  PCP_REG_FIELD     *flds_bits,
    SOC_SAND_IN  uint32           nof_flds,
    SOC_SAND_OUT uint32            *fld_vals
  )
{
  uint32
    reg_value;
  uint32
    indx;
  PCP_REG_ADDR
    curr_reg;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_READ_ARRAY_OF_FLDS);

  SOC_SAND_CHECK_NULL_INPUT(base_reg);
  SOC_SAND_CHECK_NULL_INPUT(flds_bits);
  SOC_SAND_CHECK_NULL_INPUT(fld_vals);

  curr_reg.base = base_reg->base;
  curr_reg.step = base_reg->step;

  indx = 0;
  while (indx < nof_flds)
  {
      res = pcp_read_reg_unsafe(
              unit,
              &curr_reg,
              PCP_DEFAULT_INSTANCE,
              &reg_value
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    while ((flds_bits[indx].base == curr_reg.base) && (indx < nof_flds))
    {
      fld_vals[indx] = SOC_SAND_GET_BITS_RANGE(reg_value, flds_bits[indx].msb, flds_bits[indx].lsb);
      ++ indx;
    }
    if (indx < nof_flds)
    {
      curr_reg.base = flds_bits[indx].base;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_read_array_of_flds()",0,0);
}

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
  pcp_status_fld_poll_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  PCP_REG_FIELD           *field,
    SOC_SAND_IN  uint32                 instance_ndx,
    SOC_SAND_IN  PCP_POLL_INFO           *poll_info,
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STATUS_FLD_POLL_UNSAFE);

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
         if (instance_ndx == PCP_DEFAULT_INSTANCE)
         {
           PCP_FLD_GET(*field, fld_val, 10, exit);
         }
         else
         {
           PCP_FLD_IGET(*field, fld_val, instance_ndx, 20, exit);
         }
         if (fld_val != poll_info->expected_value)
         {
           if (wait_iter > poll_info->busy_wait_nof_iters)
           {
             /*
              *  We passed the busy wait iterations -
              * start the timer-delayed iterations.
              */
             if (wait_iter <
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_status_fld_poll_unsafe()",0,0);
}

void
  PCP_POLL_INFO_clear(
    SOC_SAND_OUT PCP_POLL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_POLL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
