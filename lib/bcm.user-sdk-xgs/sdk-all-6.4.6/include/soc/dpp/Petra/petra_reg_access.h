/* $Id: petra_reg_access.h,v 1.8 Broadcom SDK $
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


#ifndef __SOC_PETRA_REG_ACCESS_H_INCLUDED__
/* { */
#define __SOC_PETRA_REG_ACCESS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_api_reg_access.h>

#ifdef LINK_PB_LIBRARIES
/* 
 * soc_pb_reg_access uses function defined in soc_petra_reg_access.h, and vice versa. 
 * In order not to restuct the code, we need to include one from the other. 
 * 'coverity[include_recursion : FALSE]' doesn't work. Also added __SOC_PB_REG_ACCESS_H_INCLUDED__
 * to ignore the issue. 
 */ 
#ifndef __SOC_PB_REG_ACCESS_H_INCLUDED__ 
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>
#endif
#endif
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

/* $Id: petra_reg_access.h,v 1.8 Broadcom SDK $
 * Soc_sand field manipulations based on soc_petra fields
 * (must contain valid msb and lsb) {
 */
#define SOC_PETRA_FLD_NOF_BITS(fld)                                               \
          ((fld).msb - (fld).lsb + 1)
#define SOC_PETRA_FLD_LSB(fld)                                                    \
          (fld).lsb
#define SOC_PETRA_FLD_MASK(fld)                                                   \
          (uint32)((SOC_SAND_BIT((fld).msb) - SOC_SAND_BIT((fld).lsb)) + SOC_SAND_BIT((fld).msb))
#define SOC_PETRA_FLD_SHIFT(fld)                                                  \
          (fld).lsb
#define SOC_PETRA_FLD_MAX(fld)                                                    \
          (SOC_SAND_BITS_MASK((fld).msb - (fld).lsb, 0))
/*
 * Take value and put it in its proper location within a 'long'
 * register (other bits are zeroed).
 */
#define SOC_PETRA_FLD_IN_PLACE(val,fld) (uint32)(((uint32)(val) << ((uint32)SOC_PETRA_FLD_SHIFT(fld))) & (uint32)(SOC_PETRA_FLD_MASK(fld)))
/*
 * Get a value out of location within a 'long' register (and make sure it
 * is not effected by bits outside its predefined mask).
 */
#define SOC_PETRA_FLD_FROM_PLACE(val,fld) (((uint32)(val) & (SOC_PETRA_FLD_MASK(fld))) >> (SOC_PETRA_FLD_SHIFT(fld)))

/*
 * Take value from buff, and split it to two buffers (used when a field is split in HW
 */
#define SOC_PETRA_FIELD_VAL_TO_SPLIT_FLDS_SET(buff, fld_lsbs, fld_msbs, fld_buff_lsbs, fld_buff_msbs) \
  do { \
  *fld_buff_lsbs = SOC_SAND_GET_BITS_RANGE(*buff, SOC_PETRA_FLD_NOF_BITS(*fld_lsbs)-1, 0); \
  *fld_buff_msbs = SOC_SAND_GET_BITS_RANGE(*buff, SOC_PETRA_FLD_NOF_BITS(*fld_msbs)-1, SOC_PETRA_FLD_NOF_BITS(*fld_lsbs)); \
  } while (0)

/*
 * Build value from two fields (used when a field is split in HW
 */
#define SOC_PETRA_FIELD_VAL_TO_SPLIT_FLDS_GET(fld_buff_lsbs, fld_buff_msbs, fld_lsbs, fld_msbs, buff) \
  *buff = \
    *fld_buff_lsbs | \
    SOC_SAND_SET_BITS_RANGE( \
    *fld_buff_msbs, \
    (SOC_PETRA_FLD_NOF_BITS(*fld_lsbs) + SOC_PETRA_FLD_NOF_BITS(*fld_msbs)-1), \
    SOC_PETRA_FLD_NOF_BITS(*fld_lsbs))

/*
 * Soc_sand field manipulations }
 */

/*
 * Within an array of registers with identical fields-
 * get the index of the register to access.
 */
#define SOC_PETRA_REG_IDX_GET(fld_global_idx,nof_flds_in_reg) \
  ((fld_global_idx) / (nof_flds_in_reg))

/*
 * Within an array of registers with identical fields-
 * get the index of the field (inside the register) to access.
 */
#define SOC_PETRA_FLD_IDX_GET(fld_global_idx,nof_flds_in_reg) \
  ((fld_global_idx) % (nof_flds_in_reg))

/*
 * Set a variable of type SOC_PETRA_REG_FIELD
 */
#define SOC_PETRA_FLD_DEFINE(fld_var,fld_offset,fld_size) \
{                                                     \
  fld_var.lsb = fld_offset;                           \
  fld_var.msb = (fld_offset) + (fld_size) - 1;            \
}

#ifdef LINK_PA_LIBRARIES
/* Register/Field set/get using default instance { */
/*
 *  Set Soc_petra field using soc_petra_write_fld.
 *  Uses SOC_PETRA_DEFAULT_INSTANCE - suitable for non-instanced blocks.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 *    - uint32 unit
 */
#define SOC_PA_FLD_SET(fld_desc, fld_value, err_num, exit_place) \
{                                                                 \
  res = soc_petra_write_fld_unsafe(                                   \
          unit,                                              \
          &(regs->a_##fld_desc),                                  \
          SOC_PETRA_DEFAULT_INSTANCE,                                 \
          (fld_value)                                             \
        );                                                        \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);               \
}
#define SOC_PA_IMPLICIT_FLD_SET(fld_desc, fld_value, err_num, exit_place) \
{                                                                          \
  res = soc_petra_write_fld_unsafe(                                            \
          unit,                                                       \
          &(fld_desc),                                                     \
          SOC_PETRA_DEFAULT_INSTANCE,                                          \
          (fld_value)                                                      \
        );                                                                 \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                        \
}

/*
 *  Get Soc_petra field using soc_petra_read_fld.
 *  Uses SOC_PETRA_DEFAULT_INSTANCE - suitable for non-instanced blocks.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 *    - uint32 unit
 */
#define SOC_PA_FLD_GET(fld_desc, fld_value, err_num, exit_place) \
{                                                                 \
  res = soc_petra_read_fld_unsafe(                                    \
          unit,                                              \
          &(regs->a_##fld_desc),                                  \
          SOC_PETRA_DEFAULT_INSTANCE,                                 \
          &(fld_value)                                            \
        );                                                        \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);               \
}
#define SOC_PA_IMPLICIT_FLD_GET(fld_desc, fld_value, err_num, exit_place) \
{                                                                          \
  res = soc_petra_read_fld_unsafe(                                             \
          unit,                                                       \
          &(fld_desc),                                                     \
          SOC_PETRA_DEFAULT_INSTANCE,                                          \
          &(fld_value)                                                     \
        );                                                                 \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                        \
}

/*
 *  Set Soc_petra register using soc_petra_write_reg.
 *  Uses SOC_PETRA_DEFAULT_INSTANCE - suitable for non-instanced blocks.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 *    - uint32 unit
 */
#define SOC_PA_REG_SET(reg_desc, reg_value, err_num, exit_place) \
{                                                                 \
  res = soc_petra_write_reg_unsafe(                                   \
          unit,                                              \
          &(regs->a_##reg_desc.addr),                             \
          SOC_PETRA_DEFAULT_INSTANCE,                                 \
          (reg_value)                                             \
        );                                                        \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);               \
}
#define SOC_PA_IMPLICIT_REG_SET(reg_desc, reg_value, err_num, exit_place) \
{                                                                          \
  res = soc_petra_write_reg_unsafe(                                            \
          unit,                                                       \
          &(reg_desc.addr),                                                \
          SOC_PETRA_DEFAULT_INSTANCE,                                          \
          (reg_value)                                                      \
        );                                                                 \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                        \
}

/*
 *  Get Soc_petra register using soc_petra_write_reg.
 *  Uses SOC_PETRA_DEFAULT_INSTANCE - suitable for non-instanced blocks.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 *    - uint32 unit
 */
#define SOC_PA_REG_GET(reg_desc, reg_value, err_num, exit_place) \
{                                                                 \
  res = soc_petra_read_reg_unsafe(                                    \
          unit,                                              \
          &(regs->a_##reg_desc.addr),                             \
          SOC_PETRA_DEFAULT_INSTANCE,                                 \
          &(reg_value)                                            \
        );                                                        \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);               \
}
#define SOC_PA_IMPLICIT_REG_GET(reg_desc, reg_value, err_num, exit_place) \
{                                                                          \
  res = soc_petra_read_reg_unsafe(                                             \
          unit,                                                       \
          &(reg_desc.addr),                                                \
          SOC_PETRA_DEFAULT_INSTANCE,                                          \
          &(reg_value)                                                     \
        );                                                                 \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                        \
}

/* Register/Field set/get using default instance } */

/* Register/Field set/get using non-default instance { */
/*
 *  Set Soc_petra field using soc_petra_write_fld.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 *    - uint32 unit
 */
#define SOC_PA_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
{                                                                            \
  res = soc_petra_write_fld_unsafe(                                              \
          unit,                                                         \
          &(regs->a_##fld_desc),                                             \
          inst_idx,                                                          \
          (fld_value)                                                        \
        );                                                                   \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                          \
}
#define SOC_PA_IMPLICIT_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
{                                                                                     \
  res = soc_petra_write_fld_unsafe(                                                       \
          unit,                                                                  \
          &(fld_desc),                                                                \
          inst_idx,                                                                   \
          (fld_value)                                                                 \
        );                                                                            \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                                   \
}

/*
 *  Get Soc_petra field using soc_petra_read_fld.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 *    - uint32 unit
 */
#define SOC_PA_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
{                                                                            \
  res = soc_petra_read_fld_unsafe(                                               \
          unit,                                                         \
          &(regs->a_##fld_desc),                                             \
          inst_idx,                                                          \
          &(fld_value)                                                       \
        );                                                                   \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                          \
}
#define SOC_PA_IMPLICIT_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
{                                                                                     \
  res = soc_petra_read_fld_unsafe(                                                        \
          unit,                                                                  \
          &(fld_desc),                                                                \
          inst_idx,                                                                   \
          &(fld_value)                                                                \
        );                                                                            \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                                   \
}

/*
 *  Set Soc_petra register using soc_petra_write_reg.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 *    - uint32 unit
 */
#define SOC_PA_REG_ISET(reg_desc, reg_value, inst_idx, err_num, exit_place) \
{                                                                            \
  res = soc_petra_write_reg_unsafe(                                              \
          unit,                                                         \
          &(regs->a_##reg_desc.addr),                                        \
          inst_idx,                                                          \
          (reg_value)                                                        \
        );                                                                   \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                          \
}
#define SOC_PA_IMPLICIT_REG_ISET(reg_desc, reg_value, inst_idx, err_num, exit_place) \
{                                                                                     \
  res = soc_petra_write_reg_unsafe(                                                       \
          unit,                                                                  \
          &(reg_desc.addr),                                                           \
          inst_idx,                                                                   \
          (reg_value)                                                                 \
        );                                                                            \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                                   \
}

/*
 *  Get Soc_petra register using soc_petra_write_reg.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 *    - uint32 unit
 */
#define SOC_PA_REG_IGET(reg_desc, reg_value, inst_idx, err_num, exit_place) \
{                                                                            \
  res = soc_petra_read_reg_unsafe(                                               \
          unit,                                                         \
          &(regs->a_##reg_desc.addr),                                        \
          inst_idx,                                                          \
          &(reg_value)                                                       \
        );                                                                   \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                          \
}
#define SOC_PA_IMPLICIT_REG_IGET(reg_desc, reg_value, inst_idx, err_num, exit_place) \
{                                                                                     \
  res = soc_petra_read_reg_unsafe(                                                        \
          unit,                                                                  \
          &(reg_desc.addr),                                                           \
          inst_idx,                                                                   \
          &(reg_value)                                                                \
        );                                                                            \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                                   \
}

/*
 *  Get field value from soc_petra register value.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 */
#define SOC_PA_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
{                                                                                 \
  res = soc_petra_field_from_reg_get(                                                 \
          &(reg_value),                                                           \
          &(regs->a_##fld_desc),                                                  \
          &(fld_value)                                                            \
        );                                                                        \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                               \
}
#define SOC_PA_IMPLICIT_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
{                                                                                          \
  res = soc_petra_field_from_reg_get(                                                          \
          &(reg_value),                                                                    \
          &(fld_desc),                                                                     \
          &(fld_value)                                                                     \
        );                                                                                 \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                                        \
}

/*
 *  Set field value into soc_petra register value.
 *  Assumes the the following variables are defined:
 *    - uint32 res
 */
#define SOC_PA_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
{                                                                               \
  res = soc_petra_field_from_reg_set(                                               \
          &(fld_value),                                                         \
          &(regs->a_##fld_desc),                                                \
          &(reg_value)                                                          \
        );                                                                      \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                             \
}
#define SOC_PA_IMPLICIT_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
{                                                                                        \
  res = soc_petra_field_from_reg_set(                                                        \
          &(fld_value),                                                                  \
          &(fld_desc),                                                                   \
          &(reg_value)                                                                   \
        );                                                                               \
  SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                                      \
}

/*
 *  Set the register to a_regs
 */
#define SOC_PA_REG_DB_ACC(fld_desc)               regs->a_##fld_desc

#define SOC_PA_REG_DB_ACC_REF(reg_desc)           &(SOC_PA_REG_DB_ACC(reg_desc))

#else

#define SOC_PA_FLD_SET(param1, param2, err_num, exit_place) 											SOC_PETRA_INVALID_DEVICE_ERROR(param2, 0, err_num, exit_place)
#define SOC_PA_IMPLICIT_FLD_SET(param1, param2, err_num, exit_place)              SOC_PETRA_INVALID_DEVICE_ERROR(param2, 0, err_num, exit_place)
#define SOC_PA_FLD_GET(param1, param2, err_num, exit_place)                       SOC_PETRA_INVALID_DEVICE_ERROR(param2, 0, err_num, exit_place)
#define SOC_PA_IMPLICIT_FLD_GET(param1, param2, err_num, exit_place)              SOC_PETRA_INVALID_DEVICE_ERROR(param2, 0, err_num, exit_place)
#define SOC_PA_REG_SET(param1, param2, err_num, exit_place)                       SOC_PETRA_INVALID_DEVICE_ERROR(param2, 0, err_num, exit_place)
#define SOC_PA_IMPLICIT_REG_SET(param1, param2, err_num, exit_place)              SOC_PETRA_INVALID_DEVICE_ERROR(param2, 0, err_num, exit_place)
#define SOC_PA_REG_GET(param1, param2, err_num, exit_place)                       SOC_PETRA_INVALID_DEVICE_ERROR(param2, 0, err_num, exit_place)
#define SOC_PA_IMPLICIT_REG_GET(param1, param2, err_num, exit_place)              SOC_PETRA_INVALID_DEVICE_ERROR(param2, 0, err_num, exit_place)
#define SOC_PA_FLD_ISET(param1, param2, param3, err_num, exit_place)              SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_IMPLICIT_FLD_ISET(param1, param2, param3, err_num, exit_place)     SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_FLD_IGET(param1, param2, param3, err_num, exit_place)              SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_IMPLICIT_FLD_IGET(param1, param2, param3, err_num, exit_place)     SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_REG_ISET(param1, param2, param3, err_num, exit_place)              SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_IMPLICIT_REG_ISET(param1, param2, param3, err_num, exit_place)     SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_REG_IGET(param1, param2, param3, err_num, exit_place)              SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_IMPLICIT_REG_IGET(param1, param2, param3, err_num, exit_place)     SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_FLD_FROM_REG(param1, param2, param3, err_num, exit_place)          SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_IMPLICIT_FLD_FROM_REG(param1, param2, param3, err_num, exit_place) SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_FLD_TO_REG(param1, param2, param3, err_num, exit_place)            SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)
#define SOC_PA_IMPLICIT_FLD_TO_REG(param1, param2, param3, err_num, exit_place)   SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)

#endif /* LINK_PA_LIBRARIES */

#ifdef LINK_PB_LIBRARIES
#else

#define SOC_PB_FLD_SET(fld_desc, fld_value, err_num, exit_place) 											    SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, 0, err_num, exit_place)
#define SOC_PB_IMPLICIT_FLD_SET(fld_desc, fld_value, err_num, exit_place)                 SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, 0, err_num, exit_place)
#define SOC_PB_FLD_GET(fld_desc, fld_value, err_num, exit_place)                          SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, 0, err_num, exit_place)
#define SOC_PB_IMPLICIT_FLD_GET(fld_desc, fld_value, err_num, exit_place)                 SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, 0, err_num, exit_place)
#define SOC_PB_REG_SET(reg_desc, reg_value, err_num, exit_place)                          SOC_PETRA_INVALID_DEVICE_ERROR(reg_value, 0, err_num, exit_place)
#define SOC_PB_IMPLICIT_REG_SET(reg_desc, reg_value, err_num, exit_place)                 SOC_PETRA_INVALID_DEVICE_ERROR(reg_value, 0, err_num, exit_place)
#define SOC_PB_REG_GET(reg_desc, reg_value, err_num, exit_place)                          SOC_PETRA_INVALID_DEVICE_ERROR(reg_value, 0, err_num, exit_place)
#define SOC_PB_IMPLICIT_REG_GET(reg_desc, reg_value, err_num, exit_place)                 SOC_PETRA_INVALID_DEVICE_ERROR(reg_value, 0, err_num, exit_place)
#define SOC_PB_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place)               SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, inst_idx, err_num, exit_place)
#define SOC_PB_IMPLICIT_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place)      SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, inst_idx, err_num, exit_place)
#define SOC_PB_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place)               SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, inst_idx, err_num, exit_place)
#define SOC_PB_IMPLICIT_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place)      SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, inst_idx, err_num, exit_place)
#define SOC_PB_REG_ISET(reg_desc, reg_value, inst_idx, err_num, exit_place)               SOC_PETRA_INVALID_DEVICE_ERROR(reg_value, inst_idx, err_num, exit_place)
#define SOC_PB_IMPLICIT_REG_ISET(reg_desc, reg_value, inst_idx, err_num, exit_place)      SOC_PETRA_INVALID_DEVICE_ERROR(reg_value, inst_idx, err_num, exit_place)
#define SOC_PB_REG_IGET(reg_desc, reg_value, inst_idx, err_num, exit_place)               SOC_PETRA_INVALID_DEVICE_ERROR(reg_value, inst_idx, err_num, exit_place)
#define SOC_PB_IMPLICIT_REG_IGET(preg_desc, reg_value, inst_idx, err_num, exit_place)     SOC_PETRA_INVALID_DEVICE_ERROR(reg_value, inst_idx, err_num, exit_place)
#define SOC_PB_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place)          SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, reg_value, err_num, exit_place)
#define SOC_PB_IMPLICIT_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place) SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, reg_value, err_num, exit_place)
#define SOC_PB_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place)            SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, reg_value, err_num, exit_place)
#define SOC_PB_IMPLICIT_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place)   SOC_PETRA_INVALID_DEVICE_ERROR(fld_value, reg_value, err_num, exit_place)

#endif /* LINK_PB_LIBRARIES */


/*
 *	Common macros
 */

/*
 *  Differentiate the register according to the unit.
 */
#define SOC_PETRA_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
{                                                                                \
  switch(soc_petra_sw_db_ver_get(unit)) {                                       \
    case SOC_PETRA_DEV_VER_A:                                                        \
      SOC_PA_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place)  \
      break;                                                                     \
    case SOC_PETRA_DEV_VER_B:                                                        \
      SOC_PB_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place)  \
      break;                                                                     \
    default:                                                                     \
      SOC_PA_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place)  \
      break;                                                                     \
  }                                                                              \
}

#define SOC_PETRA_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
{                                                                              \
  switch(soc_petra_sw_db_ver_get(unit)) {                                     \
    case SOC_PETRA_DEV_VER_A:                                                      \
      SOC_PA_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place)  \
      break;                                                                   \
    case SOC_PETRA_DEV_VER_B:                                                      \
      SOC_PB_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place)  \
      break;                                                                   \
    default:                                                                   \
      SOC_PA_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place)  \
      break;                                                                   \
  }                                                                            \
}

#define SOC_PETRA_REG_SET(reg_desc, reg_value, err_num, exit_place)  \
{                                                                 \
  switch(soc_petra_sw_db_ver_get(unit)) {                        \
    case SOC_PETRA_DEV_VER_A:                                         \
      SOC_PA_REG_SET(reg_desc, reg_value, err_num, exit_place)   \
      break;                                                      \
    case SOC_PETRA_DEV_VER_B:                                         \
      SOC_PB_REG_SET(reg_desc, reg_value, err_num, exit_place)   \
      break;                                                      \
    default:                                                      \
      SOC_PA_REG_SET(reg_desc, reg_value, err_num, exit_place)   \
      break;                                                      \
  }                                                               \
}

#define SOC_PETRA_REG_GET(reg_desc, reg_value, err_num, exit_place)  \
{                                                                 \
  switch(soc_petra_sw_db_ver_get(unit)) {                        \
    case SOC_PETRA_DEV_VER_A:                                         \
      SOC_PA_REG_GET(reg_desc, reg_value, err_num, exit_place)   \
      break;                                                      \
    case SOC_PETRA_DEV_VER_B:                                         \
      SOC_PB_REG_GET(reg_desc, reg_value, err_num, exit_place)   \
      break;                                                      \
    default:                                                      \
      SOC_PA_REG_GET(reg_desc, reg_value, err_num, exit_place)   \
      break;                                                      \
  }                                                               \
}

#define SOC_PETRA_FLD_GET(fld_desc, fld_value, err_num, exit_place)  \
{                                                                 \
  switch(soc_petra_sw_db_ver_get(unit)) {                        \
    case SOC_PETRA_DEV_VER_A:                                         \
      SOC_PA_FLD_GET(fld_desc, fld_value, err_num, exit_place)   \
      break;                                                      \
    case SOC_PETRA_DEV_VER_B:                                         \
      SOC_PB_FLD_GET(fld_desc, fld_value, err_num, exit_place)   \
      break;                                                      \
    default:                                                      \
      SOC_PA_FLD_GET(fld_desc, fld_value, err_num, exit_place)   \
      break;                                                      \
  }                                                               \
}

#define SOC_PETRA_FLD_SET(fld_desc, fld_value, err_num, exit_place)  \
{                                                                 \
  switch(soc_petra_sw_db_ver_get(unit)) {                        \
    case SOC_PETRA_DEV_VER_A:                                         \
      SOC_PA_FLD_SET(fld_desc, fld_value, err_num, exit_place)   \
      break;                                                      \
    case SOC_PETRA_DEV_VER_B:                                         \
      SOC_PB_FLD_SET(fld_desc, fld_value, err_num, exit_place)   \
      break;                                                      \
    default:                                                      \
      SOC_PA_FLD_SET(fld_desc, fld_value, err_num, exit_place)   \
      break;                                                      \
  }                                                               \
}

#define SOC_PETRA_REG_ISET(reg_desc, reg_value, inst_idx, err_num, exit_place) \
{                                                                           \
  switch(soc_petra_sw_db_ver_get(unit)) {                                  \
    case SOC_PETRA_DEV_VER_A:                                                   \
      SOC_PA_REG_ISET(reg_desc, reg_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
    case SOC_PETRA_DEV_VER_B:                                                   \
      SOC_PB_REG_ISET(reg_desc, reg_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
    default:                                                                \
      SOC_PA_REG_ISET(reg_desc, reg_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
  }                                                                         \
}

#define SOC_PETRA_REG_IGET(reg_desc, reg_value, inst_idx, err_num, exit_place) \
{                                                                           \
  switch(soc_petra_sw_db_ver_get(unit)) {                                  \
    case SOC_PETRA_DEV_VER_A:                                                   \
      SOC_PA_REG_IGET(reg_desc, reg_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
    case SOC_PETRA_DEV_VER_B:                                                   \
      SOC_PB_REG_IGET(reg_desc, reg_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
    default:                                                                \
      SOC_PA_REG_IGET(reg_desc, reg_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
  }                                                                         \
}

#define SOC_PETRA_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
{                                                                           \
  switch(soc_petra_sw_db_ver_get(unit)) {                                  \
    case SOC_PETRA_DEV_VER_A:                                                   \
      SOC_PA_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
    case SOC_PETRA_DEV_VER_B:                                                   \
      SOC_PB_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
    default:                                                                \
      SOC_PA_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
  }                                                                         \
}
#define SOC_PETRA_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
{                                                                           \
  switch(soc_petra_sw_db_ver_get(unit)) {                                  \
    case SOC_PETRA_DEV_VER_A:                                                   \
      SOC_PA_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
    case SOC_PETRA_DEV_VER_B:                                                   \
      SOC_PB_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
    default:                                                                \
      SOC_PA_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place)  \
      break;                                                                \
  }                                                                         \
}

#define SOC_PETRA_IMPLICIT_FLD_SET(fld_desc, fld_value, err_num, exit_place) \
{                                                                        \
  switch(soc_petra_sw_db_ver_get(unit)) {                               \
    case SOC_PETRA_DEV_VER_A:                                                \
      SOC_PA_IMPLICIT_FLD_SET(fld_desc, fld_value, err_num, exit_place) \
      break;                                                             \
    case SOC_PETRA_DEV_VER_B:                                                \
      SOC_PB_IMPLICIT_FLD_SET(fld_desc, fld_value, err_num, exit_place) \
      break;                                                             \
    default:                                                             \
      SOC_PA_IMPLICIT_FLD_SET(fld_desc, fld_value, err_num, exit_place) \
      break;                                                             \
  }                                                                      \
}

#define SOC_PETRA_IMPLICIT_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
{                                                                        \
  switch(soc_petra_sw_db_ver_get(unit)) {                               \
    case SOC_PETRA_DEV_VER_A:                                                \
      SOC_PA_IMPLICIT_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
      break;                                                             \
    case SOC_PETRA_DEV_VER_B:                                                \
      SOC_PB_IMPLICIT_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
      break;                                                             \
    default:                                                             \
      SOC_PA_IMPLICIT_FLD_ISET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
      break;                                                             \
  }                                                                      \
}

#define SOC_PETRA_IMPLICIT_FLD_GET(fld_desc, fld_value, err_num, exit_place) \
{                                                                        \
  switch(soc_petra_sw_db_ver_get(unit)) {                               \
    case SOC_PETRA_DEV_VER_A:                                                \
      SOC_PA_IMPLICIT_FLD_GET(fld_desc, fld_value, err_num, exit_place) \
      break;                                                             \
    case SOC_PETRA_DEV_VER_B:                                                \
      SOC_PB_IMPLICIT_FLD_GET(fld_desc, fld_value, err_num, exit_place) \
      break;                                                             \
    default:                                                             \
      SOC_PA_IMPLICIT_FLD_GET(fld_desc, fld_value, err_num, exit_place) \
      break;                                                             \
  }                                                                      \
}

#define SOC_PETRA_IMPLICIT_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place) \
{                                                                                   \
  switch(soc_petra_sw_db_ver_get(unit)) {                                          \
    case SOC_PETRA_DEV_VER_A:                                                           \
      SOC_PA_IMPLICIT_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place)      \
      break;                                                                        \
    case SOC_PETRA_DEV_VER_B:                                                           \
      SOC_PB_IMPLICIT_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place)      \
      break;                                                                        \
    default:                                                                        \
      SOC_PA_IMPLICIT_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place)      \
      break;                                                                        \
  }                                                                                 \
}

#define SOC_PETRA_IMPLICIT_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
{                                                                                        \
  switch(soc_petra_sw_db_ver_get(unit)) {                                               \
    case SOC_PETRA_DEV_VER_A:                                                                \
      SOC_PA_IMPLICIT_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
      break;                                                                             \
    case SOC_PETRA_DEV_VER_B:                                                                \
      SOC_PB_IMPLICIT_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
      break;                                                                             \
    default:                                                                             \
      SOC_PA_IMPLICIT_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place) \
      break;                                                                             \
  }                                                                                      \
}

/*
 *	Common macros for invalid calls
 */
#define SOC_PETRA_INVALID_DEVICE_ERROR(param2, param3, err_num, exit_place)         \
  SOC_SAND_IGNORE_UNUSED_VAR(res);                                                  \
  SOC_SAND_IGNORE_UNUSED_VAR(param2);                                               \
  SOC_SAND_IGNORE_UNUSED_VAR(param3);                                               \
  if (err_num > 0)                                                              \
  {                                                                             \
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_UNSUPPORTED_FOR_DEVICE_ERR, 1000+err_num, exit_place);  \
  }


#if defined(LINK_PA_LIBRARIES) && defined(LINK_PB_LIBRARIES)
#define SOC_PETRA_REG_DB_ACC_REF(reg_desc) \
  (SOC_PETRA_IS_DEV_PETRA_A)? SOC_PA_REG_DB_ACC_REF(reg_desc) : SOC_PB_REG_DB_ACC_REF(reg_desc)

#define SOC_PETRA_REG_DB_ACC(fld_desc) \
  (SOC_PETRA_IS_DEV_PETRA_A)? SOC_PA_REG_DB_ACC(fld_desc) : SOC_PB_REG_DB_ACC(fld_desc)
#endif /* LINK_PA_LIBRARIES && LINK_PB_LIBRARIES */

#if defined(LINK_PA_LIBRARIES) && !defined(LINK_PB_LIBRARIES)
  #define SOC_PETRA_REG_DB_ACC_REF(reg_desc) SOC_PA_REG_DB_ACC_REF(reg_desc)
  #define SOC_PETRA_REG_DB_ACC(fld_desc) SOC_PA_REG_DB_ACC(fld_desc)
#endif /* LINK_PA_LIBRARIES && !LINK_PB_LIBRARIES */

#if defined(LINK_PB_LIBRARIES) && !defined(LINK_PA_LIBRARIES)
  #define SOC_PETRA_REG_DB_ACC_REF(reg_desc) SOC_PB_REG_DB_ACC_REF(reg_desc)
  #define SOC_PETRA_REG_DB_ACC(fld_desc) SOC_PB_REG_DB_ACC(fld_desc)
#endif /* !LINK_PA_LIBRARIES && LINK_PB_LIBRARIES */




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
  );

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
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
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
  );

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
  );

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
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_IN  uint32          val - the register value to write.
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
);

uint32
  soc_petra_write_reg_buffer_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR      *start_reg_addr,
    SOC_SAND_IN  uint32           instance_idx,
    SOC_SAND_IN  uint32           nof_regs,
    SOC_SAND_IN  uint32            *val
  );

uint32
  soc_petra_read_reg_buffer_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR  *start_reg,
    SOC_SAND_IN  uint32        instance_idx,
    SOC_SAND_IN  uint32        nof_regs,
    SOC_SAND_OUT uint32         *val
  );

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
  );


/**************************************************************************
* NAME:
*     soc_petra_read_array_of_flds
* TYPE:
*   PROC
* DATE:
*   Sep 25 2007
* FUNCTION:
* Read an array of fields from one register or sequence of registers.
* INPUT:
* SOC_SAND_IN  int                 unit -
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
  soc_petra_read_array_of_flds(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR      *base_reg,
    SOC_SAND_IN SOC_PETRA_REG_FIELD      *flds_bits,
    SOC_SAND_IN  uint32           nof_flds,
    SOC_SAND_OUT  uint32            *fld_vals
  );

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
*****************************************************/
uint32
  soc_petra_field_from_reg_get(
    SOC_SAND_IN  uint32          *reg_buffer,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD   *field,
    SOC_SAND_OUT uint32          *fld_buffer
  );

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
  );

#ifdef LINK_PA_LIBRARIES
SOC_PA_REGS*
  soc_pa_regs(void);

SOC_PETRA_ECI_REGS*
  soc_petra_eci_regs(void);

SOC_PETRA_OLP_REGS*
  soc_petra_olp_regs(void);

SOC_PETRA_NIF_REGS*
  soc_petra_nif_regs(void);

SOC_PETRA_NIF_MAC_LANE_REGS*
  soc_petra_nif_mac_lane_regs(void);

SOC_PETRA_IRE_REGS*
  soc_petra_ire_regs(void);

SOC_PETRA_IDR_REGS*
  soc_petra_idr_regs(void);

SOC_PETRA_IRR_REGS*
  soc_petra_irr_regs(void);

SOC_PETRA_IHP_REGS*
  soc_petra_ihp_regs(void);

SOC_PETRA_IQM_REGS*
  soc_petra_iqm_regs(void);

SOC_PETRA_QDR_REGS*
  soc_petra_qdr_regs(void);

SOC_PETRA_IPS_REGS*
  soc_petra_ips_regs(void);

SOC_PETRA_IPT_REGS*
  soc_petra_ipt_regs(void);

SOC_PETRA_MMU_REGS*
  soc_petra_mmu_regs(void);

SOC_PETRA_DRC_REGS*
  soc_petra_drc_regs(void);

SOC_PETRA_DPI_REGS*
  soc_petra_dpi_regs(void);

SOC_PETRA_FDR_REGS*
  soc_petra_fdr_regs(void);

SOC_PETRA_FDT_REGS*
  soc_petra_fdt_regs(void);

SOC_PETRA_FCR_REGS*
  soc_petra_fcr_regs(void);

SOC_PETRA_FCT_REGS*
  soc_petra_fct_regs(void);

SOC_PETRA_MESH_TOPOLOGY_REGS*
  soc_petra_mesh_topology_regs(void);

SOC_PETRA_RTP_REGS*
  soc_petra_rtp_regs(void);

SOC_PETRA_FABRIC_MAC_REGS*
  soc_petra_fabric_mac_regs(void);

SOC_PETRA_MSW_REGS*
  soc_petra_msw_regs(void);

SOC_PETRA_EGQ_REGS*
  soc_petra_egq_regs(void);

SOC_PETRA_CFC_REGS*
  soc_petra_cfc_regs(void);

SOC_PETRA_SCH_REGS*
  soc_petra_sch_regs(void);
#endif

SOC_PETRA_REGS*
  soc_petra_regs(void);

/*********************************************************************
* NAME:
*     soc_petra_status_fld_poll_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Preform polling on the value of a given register.
*     According to the poll_info structure, the polling is
*     first performed as busy-wait, repeatedly reading the
*     register. Then, a polling is performed with the
*     requested timer delay between consecutive reads. The
*     'success' parameter returns TRUE if the expected value
*     is read, and FALSE if all the polling iterations do not
*     read the expected value (timeout).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_REG_FIELD           *field -
*     The field to read. The read value is compared to the
*     expected value on each polling iteration.
*   SOC_SAND_IN  uint32        instance_ndx -
*     For an instanced field (e.g. DRAM, MAL) - the instance index.
*     For a non-instanced field (single instance of the BLOCK exist),
*     must be SOC_PETRA_DEFAULT_INSTANCE.
*  SOC_SAND_IN  SOC_PETRA_POLL_INFO           *poll_info -
*     The polling parameters (expected value, number of
*     iterations, delay).
*  SOC_SAND_OUT uint8                 *success -
*     TRUE if the expected value is read. FALSE if timeout
*     occurs, and the expected value is not read.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_status_fld_poll_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD           *field,
    SOC_SAND_IN  uint32                 instance_ndx,
    SOC_SAND_IN  SOC_PETRA_POLL_INFO           *poll_info,
    SOC_SAND_OUT uint8                 *success
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_REG_ACCESS_H_INCLUDED__*/
#endif
