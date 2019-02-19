/* $Id: soc_petra_bitstream.c,v 1.5 Broadcom SDK $
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
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_bitstream.c,v 1.5 Broadcom SDK $
 *	Write any value to the specified table field.
 *  Note: assumes the size of src_data are same as register-size (uint32)
 */
uint32
  soc_petra_field_in_place_set(
    SOC_SAND_IN  uint32           *src_data,
    SOC_SAND_IN  SOC_PETRA_TBL_FIELD    *field,
    SOC_SAND_OUT uint32           *dst_data
  )
{
  uint32
    fld_lsb,
    fld_msb,
    fld_size = 0,
    last_data,
    last_data_idx,
    last_fld_max;
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REG_FIELD
    last_reg_fld;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FIELD_IN_PLACE_SET);

  SOC_SAND_CHECK_NULL_INPUT(src_data);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(dst_data);

  fld_msb = field->msb;
  fld_lsb = field->lsb;

  fld_size = SOC_PETRA_FLD_SIZE_BITS(fld_msb, fld_lsb);

  /*
   *	Verify no overflow, check
   *  the last significant uint32 of the src_data
   */

  last_reg_fld.lsb = 0;
  last_reg_fld.msb = (uint8)((fld_msb - fld_lsb) % SOC_SAND_REG_SIZE_BITS);
  last_fld_max = SOC_PETRA_FLD_MAX(last_reg_fld);

  last_data_idx = (fld_size - 1) / SOC_SAND_REG_SIZE_BITS;
  last_data = src_data[last_data_idx];
  
  if (last_data > last_fld_max)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_FIELD_VAL_OUT_OF_RANGE_ERR, 5, exit);
  }

  res = soc_sand_bitstream_set_any_field(
          src_data,
          fld_lsb,
          fld_size,
          dst_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_field_in_place_set()",0,0);
}


uint32
  soc_petra_field_in_place_get(
    SOC_SAND_IN  uint32           *src_data,
    SOC_SAND_IN  SOC_PETRA_TBL_FIELD   *field,
    SOC_SAND_OUT uint32           *dst_data
  )
{
  uint32
    fld_lsb,
    fld_msb,
    fld_size = 0;
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FIELD_IN_PLACE_GET);

  SOC_SAND_CHECK_NULL_INPUT(src_data);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(dst_data);

  fld_msb = field->msb;
  fld_lsb = field->lsb;

  fld_size = SOC_PETRA_FLD_SIZE_BITS(fld_msb, fld_lsb);

  res = soc_sand_bitstream_get_any_field(
          src_data,
          fld_lsb,
          fld_size,
          dst_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_field_in_place_get()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
