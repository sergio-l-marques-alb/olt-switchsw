/* $Id: pcp_reg_access.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PCP_REG_ACCESS_H_INCLUDED__
/* { */
#define __SOC_PCP_REG_ACCESS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_reg_access.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define PCP_DEFAULT_INSTANCE   0xff

/* } */
/*************
 *  MACROS   *
 *************/
/* { */

#define PCP_FLD_SIZE_BITS(msb, lsb) (msb - lsb + 1)
#define PCP_FLD_NOF_BITS(fld)                                                     \
          ((fld).msb - (fld).lsb + 1)
#define PCP_FLD_LSB(fld)                                                          \
          (fld).lsb
#define PCP_FLD_MASK(fld)                                                         \
          ((SOC_SAND_BIT((fld).msb) - SOC_SAND_BIT((fld).lsb)) + SOC_SAND_BIT((fld).msb))
#define PCP_FLD_SHIFT(fld)                                                        \
          (fld).lsb
#define PCP_FLD_MAX(fld)                                                          \
          (SOC_SAND_BITS_MASK((fld).msb - (fld).lsb, 0))

#define PCP_FLD_AT_INDX_SET(fld_desc, fld_indx, fld_value, err_num, exit_place)   \
          do                                                                      \
          {                                                                       \
            res = pcp_write_fld_at_indx_unsafe(                                   \
                    unit,                                                    \
                    &(fld_desc),                                                  \
                    fld_indx,                                                     \
                    PCP_DEFAULT_INSTANCE,                                         \
                    (fld_value)                                                   \
                  );                                                              \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                     \
          } while(0)

#define PCP_FLD_SET(fld_desc, fld_value, err_num, exit_place)                     \
          PCP_FLD_AT_INDX_SET(fld_desc, 0, fld_value, err_num, exit_place)

#define PCP_FLD_AT_INDX_GET(fld_desc, reg_indx, fld_value, err_num, exit_place)   \
          do                                                                      \
          {                                                                       \
            res = pcp_read_fld_at_indx_unsafe(                                    \
                    unit,                                                    \
                    &(fld_desc),                                                  \
                    reg_indx,                                                     \
                    PCP_DEFAULT_INSTANCE,                                         \
                    &(fld_value)                                                  \
                  );                                                              \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                     \
          } while(0)

#define PCP_FLD_GET(fld_desc, fld_value, err_num, exit_place)                     \
          PCP_FLD_AT_INDX_GET(fld_desc, 0, fld_value, err_num, exit_place)

#define PCP_REG_AT_INDX_SET(reg_desc, reg_indx, reg_value, err_num, exit_place)   \
          do                                                                      \
          {                                                                       \
            res = pcp_write_reg_at_indx_unsafe(                                   \
                    unit,                                                    \
                    &(reg_desc.addr),                                             \
                    reg_indx,                                                     \
                    PCP_DEFAULT_INSTANCE,                                         \
                    (reg_value)                                                   \
                  );                                                              \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                     \
          } while(0)





#define PCP_REG_AT_INDX_GET(reg_desc, reg_indx, reg_value, err_num, exit_place)   \
          do                                                                      \
          {                                                                       \
            res = pcp_read_reg_at_indx_unsafe(                                    \
                    unit,                                                    \
                    &(reg_desc.addr),                                             \
                    reg_indx,                                                     \
                    PCP_DEFAULT_INSTANCE,                                         \
                    &(reg_value)                                                  \
                  );                                                              \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                     \
          } while(0)

#define PCP_REG_GET(reg_desc, reg_value, err_num, exit_place)                     \
          PCP_REG_AT_INDX_GET(reg_desc, 0, reg_value, err_num, exit_place)

#define PCP_REG_SET(fld_desc, fld_value, err_num, exit_place)                    \
          PCP_REG_AT_INDX_SET(fld_desc, 0, fld_value, err_num, exit_place)


#define PCP_FLD_IGET(fld_desc, fld_value, inst_idx, err_num, exit_place)          \
          do                                                                      \
          {                                                                       \
            res = pcp_read_fld_unsafe(                                            \
                    unit,                                                    \
                    &(fld_desc),                                                  \
                    inst_idx,                                                     \
                    &(fld_value)                                                  \
                  );                                                              \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                     \
          } while(0)



#define PCP_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place)     \
          do                                                                      \
          {                                                                       \
            res = pcp_field_from_reg_get(                                         \
                    &(reg_value),                                                 \
                    &(fld_desc),                                                  \
                    &(fld_value)                                                  \
                  );                                                              \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                     \
          } while(0)

#define PCP_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place)       \
          do                                                                      \
          {                                                                       \
            res = pcp_field_from_reg_set(                                         \
                    &(fld_value),                                                 \
                    &(fld_desc),                                                  \
                    &(reg_value)                                                  \
                  );                                                              \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                     \
          } while(0)

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef SOC_TMC_POLL_INFO                                      PCP_POLL_INFO;

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

uint32
  pcp_base_offset_set_unsafe(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32           base_offset
  );

uint32
  pcp_read_fld_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_FIELD*  field,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_OUT uint32*         val
  );

uint32
  pcp_write_fld_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_FIELD*    field,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_IN  uint32          val
  );

uint32
  pcp_read_reg_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_ADDR*   reg,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_OUT uint32*         val
  );

uint32
  pcp_write_reg_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_ADDR      *reg,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_IN  uint32          val
  );

uint32
  pcp_write_reg_buffer_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  PCP_REG_ADDR        *start_reg_addr,
    SOC_SAND_IN  uint32           instance_idx,
    SOC_SAND_IN  uint32           nof_regs,
    SOC_SAND_IN  uint32            *val
  );

uint32
  pcp_read_reg_buffer_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  PCP_REG_ADDR     *start_reg,
    SOC_SAND_IN  uint32        instance_idx,
    SOC_SAND_IN  uint32        nof_regs,
    SOC_SAND_OUT uint32         *val
  );

uint32
  pcp_write_array_of_flds(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  PCP_REG_ADDR      *base_reg,
    SOC_SAND_IN PCP_REG_FIELD      *flds_bits,
    SOC_SAND_IN  uint32           nof_flds,
    SOC_SAND_IN  uint32            *fld_vals
  );

uint32
  pcp_read_array_of_flds(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  PCP_REG_ADDR      *base_reg,
    SOC_SAND_IN PCP_REG_FIELD      *flds_bits,
    SOC_SAND_IN  uint32           nof_flds,
    SOC_SAND_OUT  uint32            *fld_vals
  );

uint32
  pcp_field_from_reg_get(
    SOC_SAND_IN  uint32          *reg_buffer,
    SOC_SAND_IN  PCP_REG_FIELD   *field,
    SOC_SAND_OUT uint32          *fld_buffer
  );

uint32
  pcp_field_from_reg_set(
    SOC_SAND_IN  uint32          *fld_buffer,
    SOC_SAND_IN  PCP_REG_FIELD   *field,
    SOC_SAND_OUT uint32          *reg_buffer
  );

uint32
  pcp_read_reg_at_indx_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_ADDR*   reg,
    SOC_SAND_IN  uint32         reg_indx,
    SOC_SAND_IN  uint32        instance_idx,
    SOC_SAND_OUT uint32*        val
  );

uint32
  pcp_write_reg_at_indx_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_ADDR*   reg,
    SOC_SAND_IN  uint32         reg_indx,
    SOC_SAND_IN  uint32        instance_idx,
    SOC_SAND_IN uint32        val
  );

uint32
  pcp_read_fld_at_indx_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_FIELD*   field,
    SOC_SAND_IN  uint32          fld_indx,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_OUT  uint32         *val
  );

uint32
  pcp_write_fld_at_indx_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  PCP_REG_FIELD*   field,
    SOC_SAND_IN  uint32          fld_indx,
    SOC_SAND_IN  uint32         instance_idx,
    SOC_SAND_IN  uint32          val
  );

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
  );

void
  PCP_POLL_INFO_clear(
    SOC_SAND_OUT PCP_POLL_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_REG_ACCESS_H_INCLUDED__*/
#endif
