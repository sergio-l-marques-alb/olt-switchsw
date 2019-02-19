/* $Id: soc_pb_pp_reg_access.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PB_PP_REG_ACCESS_H_INCLUDED__
/* { */
#define __SOC_PB_PP_REG_ACCESS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>

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

/* $Id: soc_pb_pp_reg_access.h,v 1.6 Broadcom SDK $
 *	Equivalent reg access macros for Soc_petra-B
 */
#define SOC_PB_PP_FLD_SET(fld_desc, fld_value, err_num, exit_place)                            \
          do                                                                               \
          {                                                                                \
            res = soc_petra_write_fld_unsafe(                                                  \
                    unit,                                                             \
                    &(fld_desc),                                                           \
                    SOC_PETRA_DEFAULT_INSTANCE,                                                \
                    (fld_value)                                                            \
                  );                                                                       \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                              \
          } while(0)

#define SOC_PB_PP_IMPLICIT_FLD_SET(fld_desc, fld_value, err_num, exit_place)                   \
          SOC_PB_IMPLICIT_FLD_SET(fld_desc, fld_value, err_num, exit_place)

#define SOC_PB_PP_FLD_GET(fld_desc, fld_value, err_num, exit_place)                            \
          do                                                                               \
          {                                                                                \
            res = soc_petra_read_fld_unsafe(                                                   \
                    unit,                                                             \
                    &(fld_desc),                                                           \
                    SOC_PETRA_DEFAULT_INSTANCE,                                                \
                    &(fld_value)                                                           \
                  );                                                                       \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                              \
          } while(0)


#define SOC_PB_PP_REG_SET(reg_desc, reg_value, err_num, exit_place)                            \
          do                                                                               \
          {                                                                                \
            res = soc_petra_write_reg_unsafe(                                                  \
                    unit,                                                             \
                    &(reg_desc.addr),                                                      \
                    SOC_PETRA_DEFAULT_INSTANCE,                                                \
                    (reg_value)                                                            \
                  );                                                                       \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                              \
          } while(0)


#define SOC_PB_PP_REG_GET(reg_desc, reg_value, err_num, exit_place)                            \
          do                                                                               \
          {                                                                                \
            res = soc_petra_read_reg_unsafe(                                                   \
                    unit,                                                             \
                    &(reg_desc.addr),                                                      \
                    SOC_PETRA_DEFAULT_INSTANCE,                                                \
                    &(reg_value)                                                           \
                  );                                                                       \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                              \
          } while(0)










#define SOC_PB_PP_FLD_FROM_REG(fld_desc, fld_value, reg_value, err_num, exit_place)            \
          do                                                                               \
          {                                                                                \
            res = soc_petra_field_from_reg_get(                                                \
                    &(reg_value),                                                          \
                    &(fld_desc),                                                           \
                    &(fld_value)                                                           \
                  );                                                                       \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                              \
          } while(0)


#define SOC_PB_PP_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place)              \
          do                                                                               \
          {                                                                                \
            res = soc_petra_field_from_reg_set(                                                \
                    &(fld_value),                                                          \
                    &(fld_desc),                                                           \
                    &(reg_value)                                                           \
                  );                                                                       \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                              \
          } while(0)

#define SOC_PB_PP_IMPLICIT_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place)     \
          SOC_PB_PP_IMPLICIT_FLD_TO_REG(fld_desc, fld_value, reg_value, err_num, exit_place)

#define SOC_PB_PP_REG_BUFFER_SET(reg_desc, nof_regs, reg_value, err_num, exit_place)           \
          do                                                                               \
          {                                                                                \
            res = soc_petra_write_reg_buffer_unsafe(                                           \
                    unit,                                                             \
                    &(reg_desc[0].addr),                                                   \
                    SOC_PETRA_DEFAULT_INSTANCE,                                                \
                    nof_regs,                                                              \
                    (reg_value)                                                            \
                  );                                                                       \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                              \
          } while(0)

#define SOC_PB_PP_REG_BUFFER_GET(reg_desc, nof_regs, reg_value, err_num, exit_place)           \
          do                                                                               \
          {                                                                                \
            res = soc_petra_read_reg_buffer_unsafe(                                            \
              unit,                                                                   \
              &(reg_desc[0].addr),                                                         \
              SOC_PETRA_DEFAULT_INSTANCE,                                                      \
              nof_regs,                                                                    \
              (reg_value)                                                                  \
              );                                                                           \
            SOC_SAND_CHECK_FUNC_RESULT(res, err_num, exit_place);                              \
          } while(0)

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

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_REG_ACCESS_H_INCLUDED__*/
#endif
