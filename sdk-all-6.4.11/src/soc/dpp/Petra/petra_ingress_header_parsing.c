/* $Id: petra_ingress_header_parsing.c,v 1.7 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_ingress_header_parsing.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* $Id: petra_ingress_header_parsing.c,v 1.7 Broadcom SDK $
 *  Stag index in key0 program
 */
#define SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG 0
/*
 *  Stag table offset in key0 program
 */
#define SOC_PETRA_IHP_STAG_OFFST_IN_PROG 0

/*
 *  Tmlag index in key0 program
 */
#define SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG 1
/*
 *  Tmlag table offset in key0 program
 */
#define SOC_PETRA_IHP_TMLAG_OFFST_IN_PROG 0

/*
 * Define the place where the field offset
 * exists in the Address
 */
#define SOC_PETRA_IHP_PTC_ADDRESS_FIELD_OFFSET_LSB  3
#define SOC_PETRA_IHP_PTC_ADDRESS_FIELD_OFFSET_MSB  4


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

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
*     soc_petra_ingress_header_parsing_regs_init
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
  soc_petra_ingress_header_parsing_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_HEADER_PARSING_REGS_INIT);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_header_parsing_regs_init()",0,0);
}


/*********************************************************************
* NAME:
*     soc_petra_ingress_header_parsing_init
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
  soc_petra_ingress_header_parsing_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_HEADER_PARSING_INIT);

  res = soc_petra_ingress_header_parsing_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_header_parsing_init()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_prog_ptc_cmd_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 cmd_offset,
    SOC_SAND_IN uint32 cmd_val,
    SOC_SAND_IN uint32 cmd_len
  )
{
  uint32
    fld_val = 0,
    tbl_offset = 0,
    fld_offset = 0,
    tmp,
    res;
  SOC_PETRA_REG_FIELD
    ptc_tbl_fld;
  SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA
    ptc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PROG_PTC_CMD_SET_UNSAFE);

  res = SOC_SAND_OK; sal_memset(
      &ptc_tbl_data,
      0x0,
      sizeof(ptc_tbl_data)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  fld_offset = SOC_SAND_GET_BITS_RANGE(cmd_offset, SOC_PETRA_IHP_PTC_ADDRESS_FIELD_OFFSET_MSB, SOC_PETRA_IHP_PTC_ADDRESS_FIELD_OFFSET_LSB);

  tbl_offset = SOC_SAND_GET_BITS_RANGE(cmd_offset, SOC_PETRA_IHP_PTC_ADDRESS_FIELD_OFFSET_LSB  - 1, 0);
  tmp = SOC_SAND_GET_BITS_RANGE(cmd_offset, SOC_SAND_NOF_BITS_IN_UINT32 - 1, SOC_PETRA_IHP_PTC_ADDRESS_FIELD_OFFSET_MSB + 1);
  tmp <<= SOC_PETRA_IHP_PTC_ADDRESS_FIELD_OFFSET_LSB;
  tbl_offset += tmp;

  ptc_tbl_fld.lsb = (uint8)(fld_offset * cmd_len);
  ptc_tbl_fld.msb = (uint8)(ptc_tbl_fld.lsb + cmd_len - 1);
  fld_val = cmd_val;

  res = soc_petra_ihp_ptc_commands1_tbl_get_unsafe(
          unit,
          tbl_offset,
          &ptc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_field_from_reg_set(
          &fld_val,
          &ptc_tbl_fld,
          ptc_tbl_data.ptc_commands
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_ihp_ptc_commands1_tbl_set_unsafe(
          unit,
          tbl_offset,
          &ptc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_ihp_ptc_commands2_tbl_set_unsafe(
          unit,
          tbl_offset,
          &ptc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_prog_ptc_cmd_set_unsafe()",0,0);
}
/*********************************************************************
*     Loads the default program (program number 00) for
*     ingress Packet Header processing. This program supports
*     the following functionalities:1. Ethernet and TM header
*     parsing (including ingress shaping header if exist).2.
*     Statistics tag parsing3. LAG resolution based on TM
*     header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_prog_n00_load_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    ptc_cmd_len = 0,
    res;
  uint32
    reg_idx = 0;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA
    ptc_tbl_data;
  SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA
    key_lut_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PROG_N00_LOAD_UNSAFE);

  regs = soc_petra_regs();
  tables = soc_petra_tbls();

  res = SOC_SAND_OK; sal_memset(
      &ptc_tbl_data,
      0x0,
      sizeof(ptc_tbl_data)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *  PTC Commands memory (tables 1 and 2 must be identical) {
   */
  ptc_cmd_len = SOC_PETRA_FLD_NOF_BITS(SOC_PA_TBL(tables->ihp.ptc_commands1_tbl.ptc_commands));
  ptc_cmd_len = ptc_cmd_len / SOC_PETRA_IHP_PTC_ENTRY_NOF_CMDS;


  /*
   *  Check Ingress Shaping without ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000002,
          0x00000020,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  Check Ingress Shaping with ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000000A,
          0x00000021,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);

  /*
   *  Parse ITMH skip 4B
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000200,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);

  /*
   *  Parse ITMH skip 4B
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000201,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);

  /*
   *  Parse ITMH skip 4B
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000202,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 108, exit);

  /*
   *  Parse ITMH skip 4B
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000203,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  /*
   *  Parse ITMH skip 4B
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000204,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);

  /*
   *  Parse ITMH skip 4B
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000205,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);

  /*
   *  Parse ITMH skip 4B, unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000206,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

  /*
   *  Parse ITMH skip 4B, unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000207,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);

  /*
   *  Parse ITMH skip 4B, unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000208,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  /*
   *  Parse ITMH skip 4B, unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000209,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);

  /*
   *  Parse ITMH skip 4B, unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000020A,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 124, exit);

  /*
   *  Parse ITMH skip 4B, unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000020B,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 126, exit);

  /*
   *  Parse ITMH skip 4B, unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000020C,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 128, exit);

  /*
   *  Parse ITMH skip 4B, unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000020D,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  /*
   *  Parse ITMH skip 4B, unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000020E,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 132, exit);

  /*
   *  Parse ITMH skip 4B
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000020F,
          0x000000A2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 134, exit);

  /*
   *  Parse ITMH skip 6B with ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000210,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 136, exit);

  /*
   *  Parse ITMH skip 6B with ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000211,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 138, exit);

  /*
   *  Parse ITMH skip 6B with ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000212,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  /*
   *  Parse ITMH skip 6B with ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000213,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 142, exit);

  /*
   *  Parse ITMH skip 6B with ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000214,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 144, exit);

  /*
   *  Parse ITMH skip 6B with ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000215,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 146, exit);

  /*
   *  Parse ITMH skip 6B with ext. Unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000216,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 148, exit);

  /*
   *  Parse ITMH skip 6B with ext. Unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000217,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  /*
   *  Parse ITMH skip 6B with ext. Unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000218,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);

  /*
   *  Parse ITMH skip 6B with ext. Unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000219,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 154, exit);

  /*
   *  Parse ITMH skip 6B with ext. Unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000021A,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 156, exit);

  /*
   *  Parse ITMH skip 6B with ext. Unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000021B,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 158, exit);

  /*
   *  Parse ITMH skip 6B with ext. Unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000021C,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  /*
   *  Parse ITMH skip 6B with ext. Unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000021D,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 162, exit);

  /*
   *  Parse ITMH skip 6B with ext. Unreachable
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000021E,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 164, exit);

  /*
   *  Parse ITMH skip 6B with ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x0000021F,
          0x00000122,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 166, exit);

  /*
   *  (ITMH w+wo ext) End if no EPPH
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000220,
          0x000003C0,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 168, exit);

  /*
   *  (ITMH w+wo ext) Parse EPPH check MPPH
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000226,
          0x000001A5,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

  /*
   *  EPPH without MPPH over it
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000250,
          0x000003C1,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 172, exit);

  /*
   *  EPPH with MPPH over it
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000256,
          0x000003C2,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 174, exit);

  /*
   *  Check Ingress Shaping without ext.
   */
  res = soc_petra_ihp_prog_ptc_cmd_set_unsafe(
          unit,
          0x00000001,
          0x000003A6,
          ptc_cmd_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 176, exit);

  /*
   *  PTC Commands memory (tables 1 and 2 must be identical) }
   */

  key_lut_data.ptc_cos_profile = 0;
  key_lut_data.ptc_key_program_var = 0;
  key_lut_data.ptc_key_program_ptr = SOC_PETRA_IHP_TMLAG_OFFST_IN_PROG;
  res = soc_petra_ihp_ptc_key_program_lut_tbl_set_unsafe(
          unit,
          0x88, /* 0x00000220 */
          &key_lut_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);

  key_lut_data.ptc_cos_profile = 0;
  key_lut_data.ptc_key_program_var = 0;
  key_lut_data.ptc_key_program_ptr = SOC_PETRA_IHP_TMLAG_OFFST_IN_PROG;
  res = soc_petra_ihp_ptc_key_program_lut_tbl_set_unsafe(
          unit,
          0x90, /* 0x00000250 */
          &key_lut_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);

  key_lut_data.ptc_cos_profile = 0;
  key_lut_data.ptc_key_program_var = 0;
  key_lut_data.ptc_key_program_ptr = SOC_PETRA_IHP_TMLAG_OFFST_IN_PROG;
  res = soc_petra_ihp_ptc_key_program_lut_tbl_set_unsafe(
          unit,
          0x96, /* 0x00000256 */
          &key_lut_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);

  key_lut_data.ptc_cos_profile = 0;
  key_lut_data.ptc_key_program_var = 0;
  key_lut_data.ptc_key_program_ptr = SOC_PETRA_IHP_TMLAG_OFFST_IN_PROG;
  res = soc_petra_ihp_ptc_key_program_lut_tbl_set_unsafe(
          unit,
          0x1, /* 0x00000001 */
          &key_lut_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);

  /*
   *  Custom Macro Commands {
   */


  /*
   *  Custom macro command 0 {
   *  TM port: Ingress Shaping
   */
  reg_val = 0;

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[0].dw_sel));

  fld_val = 0x0000000E;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[0].mask_left));

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[0].mask_right));

  fld_val = 0x00000003;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[0].cond_sel));

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[0].shift_a));

  fld_val = 0x00000004;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[0].shift_b));

  fld_val = 0x00000001;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[0].load_offset));

  SOC_PA_REG_SET(regs->ihp.ptc_custom_macro_cmd_reg[0], reg_val, 416, exit);
  /*
   *  Custom macro command 0 }
   */

  /*
   *  Custom macro command 1 {
   *  TM port: ITMH without ext.
   */
  reg_val = 0;

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[1].dw_sel));

  fld_val = 0x00000001;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[1].mask_left));

  fld_val = 0x0000001E;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[1].mask_right));

  fld_val = 0x00000006;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[1].cond_sel));

  fld_val = 0x00000004;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[1].shift_a));

  fld_val = 0x00000004;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[1].shift_b));

  fld_val = 0x00000001;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[1].load_offset));

  SOC_PA_REG_SET(regs->ihp.ptc_custom_macro_cmd_reg[1], reg_val, 432, exit);
  /*
   *  Custom macro command 1 }
   */

  /*
   *  Custom macro command 2 {
   *  TM port: ITMH with ext.
   */
  reg_val = 0;

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[2].dw_sel));

  fld_val = 0x00000001;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[2].mask_left));

  fld_val = 0x0000001E;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[2].mask_right));

  fld_val = 0x00000006;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[2].cond_sel));

  fld_val = 0x00000006;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[2].shift_a));

  fld_val = 0x00000006;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[2].shift_b));

  fld_val = 0x00000001;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[2].load_offset));

  SOC_PA_REG_SET(regs->ihp.ptc_custom_macro_cmd_reg[2], reg_val, 448, exit);
  /*
   *  Custom macro command 2 }
   */

  /*
   *  Custom macro command 3 {
   *  TM port: EPPH
   */
  reg_val = 0;

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[3].dw_sel));
  
  fld_val = 0x00000001;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[3].mask_left));

  fld_val = 0x0000001E;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[3].mask_right));

  fld_val = 0x00000006;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[3].cond_sel));

  fld_val = 0x00000008;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[3].shift_a));

  fld_val = 0x00000010;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[3].shift_b));

  fld_val = 0x00000001;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[3].load_offset));

  SOC_PA_REG_SET(regs->ihp.ptc_custom_macro_cmd_reg[3], reg_val, 464, exit);
  /*
   *  Custom macro command 3 }
   */

  /*
   *  Custom macro command 7 {
   *  Raw port
   */
  reg_val = 0;

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[7].dw_sel));

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[7].mask_left));

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[7].mask_right));

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[7].cond_sel));

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[7].shift_a));

  fld_val = 0x00000000;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[7].shift_b));

  fld_val = 0x00000001;
  reg_val |= SOC_PETRA_FLD_IN_PLACE(fld_val, SOC_PA_REG_DB_ACC(regs->ihp.ptc_custom_macro_cmd_reg[7].load_offset));


  SOC_PA_REG_SET(regs->ihp.ptc_custom_macro_cmd_reg[7], reg_val, 480, exit);
  /*
   *  Custom macro command 7 }
   */

  /*
   *  Custom Macro Commands }
   */

  /*
   *  Custom Macro Next Protocol
   *  For Ingress Shaping - High/Low values must be set later.
   */
  reg_val = 0x0003007b;
  SOC_PA_REG_SET(regs->ihp.ptc_custom_protocols_reg[0], reg_val, 500, exit);

  reg_val = 0x0003008c;
  SOC_PA_REG_SET(regs->ihp.ptc_custom_protocols_reg[1], reg_val, 520, exit);

  reg_val = 0x40000000;
  for (reg_idx = 2; reg_idx < SOC_PETRA_PTC_CUSTOM_PROTOCOLS_NOF_REGS;reg_idx++)
  {
    SOC_PA_REG_SET(regs->ihp.ptc_custom_protocols_reg[reg_idx], reg_val, 600, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_prog_n00_load_unsafe()",0,0);
}

/*********************************************************************
*     Set ingress port configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_port_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_IHP_PORT_INFO       *info
  )
{
  uint32
    tbl_offset = 0,
    res;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    port_info_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PORT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  tbl_offset = port_ndx;
  res = soc_petra_ihp_port_info_tbl_get_unsafe(
          unit,
          tbl_offset,
          &port_info_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_info_data.bytes_to_skip = info->sop2header_offset_bytes;
  port_info_data.bytes_to_remove = info->strip_from_sop_bytes;

  res = soc_petra_ihp_port_info_tbl_set_unsafe(
          unit,
          tbl_offset,
          &port_info_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_port_set_unsafe()",0,0);
}

/*********************************************************************
*     Set ingress port configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_port_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_IHP_PORT_INFO       *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PORT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->sop2header_offset_bytes, SOC_PETRA_IHP_SOP2HEADER_OFFSET_BYTES_MAX,
    SOC_PETRA_IHP_SOP2HEADER_OFFSET_OUT_OF_RANGE_ERR , 30, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->strip_from_sop_bytes, SOC_PETRA_IHP_BYTES_FROM_SOP_TO_STRIP_MAX,
    SOC_PETRA_IHP_STRIP_FROM_SOP_OUT_OF_RANGE_ERR , 40, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_port_verify()",0,0);
}

/*********************************************************************
*     Set ingress port configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_port_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_IHP_PORT_INFO       *info
  )
{
  uint32
    tbl_offset = 0,
    res;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    port_info_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PORT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_offset = port_ndx;
  res = soc_petra_ihp_port_info_tbl_get_unsafe(
          unit,
          tbl_offset,
          &port_info_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->sop2header_offset_bytes = port_info_data.bytes_to_skip;
  info->strip_from_sop_bytes = port_info_data.bytes_to_remove;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_port_get_unsafe()",0,0);
}

/*********************************************************************
*     Define the position and length of the statistics tag
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_stag_hdr_data_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info
  )
{
  uint32
    res;
  SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA
    key_prgrm0_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_STAG_HDR_DATA_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_key_program0_tbl_get_unsafe(
          unit,
          SOC_PETRA_IHP_STAG_OFFST_IN_PROG,
          &key_prgrm0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  key_prgrm0_data.select[SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG] = SOC_PETRA_IGNR_KEY_PROG_SEL_STAG;
  key_prgrm0_data.offset_select[SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG] = info->base_header;
  key_prgrm0_data.length[SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG] = info->length_4bits;
  key_prgrm0_data.byte_shift[SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG] = (info->offset_4bits)/2;
  key_prgrm0_data.nibble_shift[SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG] = (info->offset_4bits)%2;

  res = soc_petra_ihp_key_program0_tbl_set_unsafe(
          unit,
          SOC_PETRA_IHP_STAG_OFFST_IN_PROG,
          &key_prgrm0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_stag_hdr_data_set_unsafe()",0,0);
}

/*********************************************************************
*     Define the position and length of the statistics tag
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_stag_hdr_data_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_STAG_HDR_DATA_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->base_header, SOC_PETRA_IHP_NOF_PCKT_HEADER_IDS-1,
    SOC_PETRA_IHP_STAG_OFFSET_OUT_OF_RANGE_ERR , 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->offset_4bits, SOC_PETRA_IHP_STAG_OFFSET_4BITS_MAX,
    SOC_PETRA_IHP_STAG_OFFSET_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_stag_hdr_data_verify()",0,0);
}

/*********************************************************************
*     Define the position and length of the statistics tag
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_stag_hdr_data_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info
  )
{
  uint32
    res;
  SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA
    key_prgrm0_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_STAG_HDR_DATA_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_key_program0_tbl_get_unsafe(
          unit,
          SOC_PETRA_IHP_STAG_OFFST_IN_PROG,
          &key_prgrm0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->base_header = key_prgrm0_data.offset_select[SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG];
  info->length_4bits = key_prgrm0_data.length[SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG];
  info->offset_4bits = key_prgrm0_data.byte_shift[SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG]*2;
  info->offset_4bits += key_prgrm0_data.nibble_shift[SOC_PETRA_IHP_STAG_FLD_IDX_IN_PROG];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_stag_hdr_data_get_unsafe()",0,0);
}

/*********************************************************************
*     Define the position and length of the TM-LAG Hushing
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_tmlag_hush_field_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  )
{
  uint32
    res = SOC_SAND_OK,
    actual_nibble_offset;
  SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA
    key_prgrm0_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_TMLAG_HUSH_FIELD_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_key_program0_tbl_get_unsafe(
          unit,
          SOC_PETRA_IHP_TMLAG_OFFST_IN_PROG,
          &key_prgrm0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Compensate for two bytes' overhead in the internal buffer
   */
  actual_nibble_offset = info->offset_4bits
                           + (info->base_header == SOC_PETRA_IHP_PCKT_HEADER_ID_SOP ? 4 : 0);

  key_prgrm0_data.select[SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG] = SOC_PETRA_IGNR_KEY_PROG_SEL_TM_LAG;
  key_prgrm0_data.offset_select[SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG] = info->base_header;
  key_prgrm0_data.length[SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG] = info->length_4bits - 1;
  key_prgrm0_data.byte_shift[SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG] = (actual_nibble_offset)/2;
  key_prgrm0_data.nibble_shift[SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG] = (actual_nibble_offset)%2;

  res = soc_petra_ihp_key_program0_tbl_set_unsafe(
          unit,
          SOC_PETRA_IHP_TMLAG_OFFST_IN_PROG,
          &key_prgrm0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_tmlag_hush_field_set_unsafe()",0,0);
}

/*********************************************************************
*     Define the position and length of the TM-LAG Hushing
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_tmlag_hush_field_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_TMLAG_HUSH_FIELD_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);
  SOC_SAND_ERR_IF_ABOVE_MAX(
      info->base_header, SOC_PETRA_IHP_NOF_PCKT_HEADER_IDS-1,
      SOC_PETRA_IHP_TMLAG_OFFSET_BASE_OUT_OF_RANGE_ERR , 10, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->offset_4bits, SOC_PETRA_IHP_TMLAG_OFFSET_4BITS_MAX,
      SOC_PETRA_IHP_TMLAG_OFFSET_OUT_OF_RANGE_ERR, 20, exit
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_tmlag_hush_field_verify()",0,0);
}

/*********************************************************************
*     Define the position and length of the TM-LAG Hushing
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_tmlag_hush_field_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  )
{
  uint32
    res;
  SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA
    key_prgrm0_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_TMLAG_HUSH_FIELD_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_key_program0_tbl_get_unsafe(
          unit,
          SOC_PETRA_IHP_TMLAG_OFFST_IN_PROG,
          &key_prgrm0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->base_header = key_prgrm0_data.offset_select[SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG];
  info->length_4bits = key_prgrm0_data.length[SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG];
  info->offset_4bits = key_prgrm0_data.byte_shift[SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG] * 2;
  info->offset_4bits += key_prgrm0_data.nibble_shift[SOC_PETRA_IHP_TMLAG_FLD_IDX_IN_PROG];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_tmlag_hush_field_get_unsafe()",0,0);
}

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

