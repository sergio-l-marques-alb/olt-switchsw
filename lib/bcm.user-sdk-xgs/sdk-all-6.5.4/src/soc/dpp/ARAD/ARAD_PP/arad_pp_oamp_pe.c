/* $Id: arad_pp_oamp_pe.c,v 1.6 Broadcom SDK $
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
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_OAM

#include <shared/bsl.h>


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h>

#include <shared/swstate/access/sw_state_access.h>
/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define OAM_ACCESS  sw_state_access[unit].dpp.bcm.oam

/*Number of programs*/



/* max program number */
#define ARAD_PP_OAMP_PE_MAX_PROGRAM(unit)  ( SOC_IS_JERICHO(unit)?  32 : 8 )

/* max lfem number  */
#define ARAD_PP_OAMP_PE_MAX_LFEM  (SOC_DPP_DEFS_MAX(OAMP_PE_LFEM_NOF))

/* Actual number of maps of LFEMS */
#define ARAD_PP_OAMP_PE_NUM_LFEM(unit) (SOC_DPP_DEFS_GET(unit,oamp_pe_lfem_nof))

/* Actual number of defined LFEMs */
#define _ARAD_PP_OAMP_PE_NUM_DEFINED_LFEMS 8

/* Number of program selection profiles */
#define ARAD_PP_OAMP_PE_NOF_PROG_SEL_PROFILES(_unit)     (SOC_DPP_DEFS_GET(_unit,oamp_pe_prog_sel_profile_nof))

/* max instruction entries */
/* There are 42 entries, only 0-31 are used for program pointers. The rest are for jumps and are not dynamic */

/* max size of packet /4. Default value. */
#define ARAD_PP_OAMP_PE_MAX_INSTR_COUNT 0x1000

/** Following defines taken from oamp_pe_defines.v */

#define ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO		0x1
#define ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG0	0x2
#define ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG1	0x3
#define ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG2	0x4
#define ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR	0x7
#define ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1	0x8
#define ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG2	0x9
#define ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3	0xa
#define ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4	0xb
#define ARAD_PP_OAMP_PE_MUX_SRC_FDBK_FIFO	0xc

#define ARAD_PP_OAMP_PE_OP_SEL_REG0			0x2
#define ARAD_PP_OAMP_PE_OP_SEL_REG2			0x4
#define ARAD_PP_OAMP_PE_OP_SEL_REG4			0x6
#define ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1		((SOC_IS_JERICHO(unit))? 0x6: 0x7)
#define ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2		((SOC_IS_JERICHO(unit))? 0x7: 0x8)
#define ARAD_PP_OAMP_PE_OP_SEL_INST_VAR		((SOC_IS_JERICHO(unit))? 0Xb: 0xc) 
#define ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD		((SOC_IS_JERICHO(unit))? 0xc: 0xd)  

#define ARAD_PP_OAMP_PE_OP3_SEL_FEM			0x1
#define ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG1	0x2
#define ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2	0x3
#define ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3	0x4
#define ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4	0x5
#define ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT		0x6
#define ARAD_PP_OAMP_PE_OP3_SEL_MEM_OUT		0x7

#define ARAD_PP_OAMP_PE_CMP_ACT_NEVER		0x0
#define ARAD_PP_OAMP_PE_CMP_ACT_ZERO		0x2
#define ARAD_PP_OAMP_PE_CMP_ACT_POS			0x3
#define ARAD_PP_OAMP_PE_CMP_ACT_NNEG		0x4
#define ARAD_PP_OAMP_PE_CMP_ACT_NPOS		0x5
#define ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS		0x6

#define ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_REG2	0x2
#define ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_REG3	0x3
#define ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST	0x5
#define ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B	0x6

#define ARAD_PP_OAMP_PE_SHIFT_SRC_REG3		0x3
#define ARAD_PP_OAMP_PE_SHIFT_SRC_REG4		0x4
#define ARAD_PP_OAMP_PE_SHIFT_SRC_CONST		0x5

#define ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT	0x0
#define ARAD_PP_OAMP_PE_INST_SRC_LOP_JMP	0x1
#define ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP	0x2

#define ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1	0x0
#define ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2	0x1
#define ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3	0x2
#define ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4	0x3

#define ARAD_PP_OAMP_PE_ALU_ACT_ADD			0x0
#define ARAD_PP_OAMP_PE_ALU_ACT_SUB			0x1
#define ARAD_PP_OAMP_PE_ALU_ACT_AND			0x2
#define ARAD_PP_OAMP_PE_ALU_ACT_OR          0x3

#define ARAD_PP_OAMP_PE_MERGE_INST_FEM		0x1
#define ARAD_PP_OAMP_PE_MERGE_INST_AND		0x2
#define ARAD_PP_OAMP_PE_MERGE_INST_OR		0x3

/*Jericho defines*/
#define ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR0	0X2
#define ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR1	0X3
#define ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR2	0X4
#define ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3	0X5
#define ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR4	0X6
#define ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR	0Xd
 
#define ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR0		0X1
#define ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR1		0X2
#define ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR2		0X3
#define ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR3		0X4
#define ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR4		0X5

#define ARAD_PP_OAMP_PE_OP_SEL_TMP_REG3		((SOC_IS_JERICHO(unit))? 0x8: 0x9)
#define ARAD_PP_OAMP_PE_OP_SEL_TMP_REG4		((SOC_IS_JERICHO(unit))? 0x9: 0xa)



#define ARAD_PP_OAMP_PE_OP_SEL_PRG_VAR		0Xa
#define ARAD_PP_OAMP_PE_OP_SEL_PDU_OFFS		0Xd
#define ARAD_PP_OAMP_PE_OP_SEL_ENC_OFFS		0Xe
#define ARAD_PP_OAMP_PE_OP_SEL_PKT_LNTH		0Xf

                         
#define ARAD_PP_OAMP_PE_OP3_SEL_INST_VAR	0X8
#define ARAD_PP_OAMP_PE_OP3_SEL_INST_VAR_MSB	0X9

#define ARAD_PP_OAMP_PE_CMP_ACT_NEG			0X1

#define ARAD_PP_OAMP_PE_BUFF_WR_NEVER		0X0
#define ARAD_PP_OAMP_PE_BUFF_WR_ALWAYS		0X1
#define ARAD_PP_OAMP_PE_BUFF_WR_ON_N_CMP1	0X2
#define ARAD_PP_OAMP_PE_BUFF_WR_ON_N_CMP2	0X3

#define ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_REG1	0X1
#define ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_REG2	0X2
#define ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST	0X5
#define ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B	0X6

#define ARAD_PP_OAMP_PE_MERGE_INST_CON		0X2

#define ARAD_PP_OAMP_PE_BUFF2_SIZE_1B		0X0
#define ARAD_PP_OAMP_PE_BUFF2_SIZE_2B		0X1
#define ARAD_PP_OAMP_PE_BUFF2_SIZE_3B		0X2
#define ARAD_PP_OAMP_PE_BUFF2_SIZE_4B		0X3

#define ARAD_PP_OAMP_PE_SHIFT_SRC_REG1		0X1
#define ARAD_PP_OAMP_PE_SHIFT_SRC_REG2		0X2

#define ARAD_PP_OAMP_PE_INST_SRC_LOP_CST	0X3


#define ARAD_PP_OAMP_PE_EOP_WR_NEVER		0X0
#define ARAD_PP_OAMP_PE_EOP_WR_ON_CMP1		0X1
#define ARAD_PP_OAMP_PE_EOP_WR_ON_CMP2		0X2
#define ARAD_PP_OAMP_PE_EOP_ALWAYS			0X3

/* When no FEM is used point to the number of FEM +1.
   Different number of FEMs per device.*/
#define ARAD_PP_OAMP_PE_FEM_DEF_PR(_unit) (SOC_IS_QAX(_unit)?0x1f:0xb)


/** Jericho program selection TCAM constants */
#define ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_LMM_R 0
#define ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_DMM_R 1
#define ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM 2
#define ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_OPCODE_N  4

#define ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED  0
#define ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_RESPONSE  1
#define ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_PUNT  2 
#define ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_EVENT  3
#define ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_SAT  4
#define ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_PACKET_DMA  5

/* Represents having every bit in the TCAM mask on*/
#define _OAMP_PE_TCAM_MASK_DONT_CARE (SOC_IS_JERICHO(unit)? 0xffffff : 0x1ffff)


/* Since the PE Program memory does not come with fields, we must do this by hand*/
/* Here val may also be a constant.*/
/* Used the same in Arad and Jericho.*/
#define SET_OAMP_PE_IN_FIFO_RD_BITS(reg,val)      SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_IN_FIFO_RD_BITS    )
#define SET_OAMP_PE_FEM1_SEL_BITS(reg,val)        SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_FEM1_SEL_BITS      )
#define SET_OAMP_PE_FEM2_SEL_BITS(reg,val)        SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_FEM2_SEL_BITS      )
#define SET_OAMP_PE_MUX1_SRC_BITS(reg,val)        SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_MUX1_SRC_BITS      )
#define SET_OAMP_PE_MERGE1_INST_BITS(reg,val)     SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_MERGE1_INST_BITS   )
#define SET_OAMP_PE_SHIFT1_SRC_BITS(reg,val)      SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_SHIFT1_SRC_BITS    )
#define SET_OAMP_PE_SHIFT2_SRC_BITS(reg,val)      SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_SHIFT2_SRC_BITS    )
#define SET_OAMP_PE_FDBK_FF_WR_BIT(reg,val)       SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_FDBK_FF_WR_BIT     )
#define SET_OAMP_PE_OP1_SEL_BITS(reg,val)         SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_OP1_SEL_BITS       )
#define SET_OAMP_PE_OP2_SEL_BITS(reg,val)         SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_OP2_SEL_BITS       )
#define SET_OAMP_PE_ALU_ACT_BITS(reg,val)         SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_ALU_ACT_BITS       )
#define SET_OAMP_PE_CMP1_ACT_BITS(reg,val)        SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_CMP1_ACT_BITS      )
#define SET_OAMP_PE_ALU_DST_BITS(reg,val)         SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_ALU_DST_BITS       )
#define SET_OAMP_PE_BUF_EOP_FRC_BIT(reg,val)      SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_BUF_EOP_FRC_BIT    )
#define SET_OAMP_PE_INST_CONST_BITS(reg,val)      SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_INST_CONST_BITS    )
#define SET_OAMP_PE_FDBK_FF_RD_BIT(reg,val)       SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_FDBK_FF_RD_BIT     )
#define SET_OAMP_PE_OP3_SEL_BITS(reg,val)         SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_OP3_SEL_BITS       )
#define SET_OAMP_PE_CMP2_ACT_BITS(reg,val)        SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_CMP2_ACT_BITS      )
#define SET_OAMP_PE_INST_SRC_BITS(reg,val)        SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_INST_SRC_BITS      )
#define SET_OAMP_PE_MUX2_SRC_BITS(reg,val)        SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_MUX2_SRC_BITS      )

/* For Arad only */
#define SET_OAMP_PE_BUFF_WR_BIT(reg,val)          SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_BUFF_WR_BIT        )
#define SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg,val)   SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_BITS )
#define SET_OAMP_PE_BUF_EOP_BIT(reg,val)          SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_BUF_EOP_BIT        )
#define SET_OAMP_PE_MERGE2_INST_BITS(reg,val)     SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_MERGE2_INST_BITS   )

/* For Jericho and above only */
#define SET_OAMP_PE_BUFF_WR_BITS(reg,val)         SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_BUFF_WR_BITS       )
#define SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg,val)  SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_BITS)
#define SET_OAMP_PE_BUFF2_SIZE_BITS(reg,val)      SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_BUFF2_SIZE_BITS    )
#define SET_OAMP_PE_BUF_EOP_BITS(reg,val)         SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit,reg,val,ARAD_PP_OAMP_PE_BUF_EOP_BITS       )


/*The following macro should not be used except by the above macros. */
#define SET_OAMP_PE_PROGRAM_BY_FIELD_ENUM(unit, reg, val, inst_field) \
   do {\
       uint32 __val = val; \
       if (instruction_field_size[unit][inst_field]) {\
            SHR_BITCOPY_RANGE(reg,instruction_field_start[unit][inst_field],&__val,0,instruction_field_size[unit][inst_field]);\
       }\
   } while (0);


#define WRITE_TO_TCAM_TABLE(tcam_info, table_num) \
    do { \
    uint32 tcam_reg[2]={0};\
    soc_OAMP_PE_0_PROG_TCAMm_field_set(unit, tcam_reg, KEYf, &(tcam_info.key));\
    soc_OAMP_PE_0_PROG_TCAMm_field_set(unit, tcam_reg, MASKf, &(tcam_info.mask));\
    soc_OAMP_PE_0_PROG_TCAMm_field_set(unit, tcam_reg, DATf, &(tcam_info.dat));\
    soc_OAMP_PE_0_PROG_TCAMm_field_set(unit, tcam_reg, VALIDf, &(tcam_info.valid));\
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_0_PROG_TCAMm(unit, MEM_BLOCK_ALL, table_num, tcam_reg));\
    if (SOC_IS_JERICHO(unit)) {SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_1_PROG_TCAMm(unit, MEM_BLOCK_ALL, table_num, tcam_reg));}\
} while (0)
/*
* Macro sets the first instruction of a program according to the last program pointer. 
* Macro should be called at the start of each program function in order to set the first instruction. 
* (This can't be set beforehand because the first instruction depends on what programs were previously added).                                                                                                  .
*/
#define SET_FIRST_INSTR(first_instruc, prog) \
    first_instruc = programs[unit][prog].first_instr =  last_program_pointer[unit];\
    (void)first_instruc;

/*Use this macro at your own risk! (unless this is used by the macros below)*/
#define SET_OMAP_TCAM_BY_BITS(dst,src,start_bits,end_bits ) \
    do{ \
    uint32 __src = src; \
    SHR_BITCOPY_RANGE(&(dst),start_bits, &__src, 0, end_bits-start_bits+1);\
    }while (0); 

/** Packet type encodeing:
   0 - MEP generated,
   1- MEP response,
   2 - punt packet  ,                                      
   3 - Event packet ,                      
   4 - SAT/1MEP machine 
  One of ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_* defines 
   */ 
#define SET_OAMP_PE_TCAM_PACKET_TYPE(key_or_mask, val) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 0,2)

/** MEP Type: taken from MEP DB*/
#define SET_OAMP_PE_TCAM_MEP_TYPE(key_or_mask, val) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,6)

/** MEP Profile: from MEP entry: MEP profile or MEP Diag Profile when MEP profile field does not exist */
#define SET_OAMP_PE_TCAM_MEP_PROFILE(key_or_mask, val)  \
    do {\
        if (SOC_IS_JERICHO(unit)) {\
            SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 7,13);\
        } else {\
            SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 7,10);\
        }\
    } while (0)


/** MEP Profile: from MEP entry: MEP PE profile or 0 if MEP
 *  type is BFD multi hop in Arad */
#define SET_OAMP_PE_TCAM_MEP_PE_PROFILE(key_or_mask, val) \
    do {\
        if (SOC_IS_JERICHO(unit)) {\
            SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 14,19);\
        } else {\
            SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 11,14)\
        }\
    } while (0)

/** Different parameters for following macro: Accepts the tcam struct and the program key from
   ARAD_PP_OAMP_PE_PROGRAMS*/
#define SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(tcam_struct,tcam_index,program_index)   do { \
    OAM_ACCESS.mep_pe_profile.get(unit, program_index, &u32_sw_state_value);\
    if (u32_sw_state_value==-1) {\
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("MEP PE profile uninitialized")));\
        }\
        OAM_ACCESS.mep_pe_profile.get(unit, program_index, &u32_sw_state_value);\
        SET_OAMP_PE_TCAM_MEP_PE_PROFILE(tcam_struct[tcam_index].key, u32_sw_state_value) ; }while (0)

/** Msg type encoding: 0 - LMM/LMR (includes piggy back CCM in Arad+)
   1 - DMM/R
   2 - CCM (includes piggy back CCM in Jericho)
   3 - LBM/LBR
   4-15 - Additional OpCodes (Jericho only)*/
/** One of  ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_* defines   */
#define SET_OAMP_PE_TCAM_MSG_TYPE(key_or_mask, val) \
    do {\
        if (SOC_IS_JERICHO(unit)) {\
            SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 20,23);\
        } else {\
            SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 15,16);\
        }\
    } while (0)


/* If packet source is 2 (Punt)*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_PUNT(key_or_mask, val )  \
            do {SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,6); SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 9,13); SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 22,23);}while(0)
/** punt profile from the RMEP DB */
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_PUNT_PUNT_PROFILE(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 7,8) 
/** As set by receive processor according to the trap reason configuration */ 
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_PUNT_TRAP_CODE(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 14,21)

/*If packet type is 3 (event)*/
/** CPU configuration register*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_EVENT_CFG_EVENT(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,23)

/*If packet type is 4 (SAT)*/
/** CPU configuration register*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_SAT_CFG1_MEP_PACKET_PE_SEL(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,19)
/** 0-15, indicating which packet generator packet came from. */
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_SAT_GENERATOR_ID(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 20,23)

/*If packet type is 3 (packet DMA)*/
/** The first 21b of the packet should be CPU inserted program selection key. The first 32b of the packet which consists the key should be dropped.*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_DMA_PACKET_DATA(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,23)



/**/

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/*
 * Jericho PE Profile data
 */
typedef struct {
   uint8 pe_var_8_nibble_select_0;          /* Select nibbles from mep db entry to be used by the PE as variable */
   uint8 pe_var_8_nibble_select_1;          /* Select nibbles from mep db entry to be used by the PE as variable */
   uint8 pe_var_16_nibble_select_0;         /* Select nibbles from mep db entry to be used by the PE as variable */
   uint8 pe_var_16_nibble_select_1;         /* Select nibbles from mep db entry to be used by the PE as variable */

   uint8 pe_mep_insert;                     /* add relevant MEP DB entry as prefix before the generated packet */

} JER_PP_OAMP_PE_PROF_INFO;

typedef struct
{
    int first_instr; /*The term "first instruction" and "program ptr" are used interchangeably.*/
    uint32 pe_const;

    /* Arad+ nibble selection */
    uint32 ls_nibble_select; /* If the program requires a nibble select, this value should be the LS. otherwise, -1 */
    uint32 ms_nibble_select; /* If the program requires a nibble select, this value should be the MS. otherwise, -1 */

    /* Jericho PE profile info (including nibble selection) */
    JER_PP_OAMP_PE_PROF_INFO    mep_pe_prof_info;

    uint32 (*instruction_func) (const int); /* Pointer to the function setting the instructions.
    Also acts as an indication that the program is active (this should be !=0 iff program is active).*/
} ARAD_PP_OAMP_PE_PROGRAM_INFO;



typedef enum
{
    ARAD_PP_PP_OAMP_EP_LFEM0,
    ARAD_PP_PP_OAMP_EP_LFEM1,
    ARAD_PP_PP_OAMP_EP_LFEM2,
    ARAD_PP_PP_OAMP_EP_LFEM3,
    ARAD_PP_PP_OAMP_EP_LFEM4,
    ARAD_PP_PP_OAMP_EP_LFEM5,
    ARAD_PP_PP_OAMP_EP_LFEM6,
    ARAD_PP_PP_OAMP_EP_LFEM7,
    /*nof of lfems - must be last.*/
    ARAD_PP_OAMP_EP_LFEMS_NOF_LFEMS
} ARAD_PP_OAMP_EP_LFEMS;


typedef struct
{
    uint32 mask; /* On the register this takes 17 bits*/
    uint32 key;
    uint32 dat; /*Should be anything between 0-7 - what program to run*/
    uint32 valid; /* These are uint32 as opposed to uint8 so as to make it easier to write on the memory tables*/
} ARAD_PP_TCAM_ENTRY;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* 
 * All the programs info:
 */
static ARAD_PP_OAMP_PE_PROGRAM_INFO programs[SOC_MAX_NUM_DEVICES][ARAD_PP_OAMP_PE_PROGS_NOF_PROGS];



/*
 * for every index, 1 if the lfem is to be used, zero otherwise.
*/
static uint8 lfem_usage[SOC_MAX_NUM_DEVICES][ARAD_PP_OAMP_PE_MAX_LFEM];


/* 
 * last program ID used for allocation: 
 * if -1: no programs has been allocated yet 
 * max is  8
 */
static int last_program_id[SOC_MAX_NUM_DEVICES];



/* 
 * last instruction entry used for allocation: 
 * if -1: no instruction entry has been allocated yet 
 * max is 255
 */
static int last_program_pointer[SOC_MAX_NUM_DEVICES];

/* This is a constant default instruction and may be used by all programs.*/
static soc_reg_above_64_val_t reg_default_instr;

static uint8 instruction_field_start[SOC_MAX_NUM_DEVICES][ARAD_PP_OAMP_PE_INSTRUCTION_FIELDS_NOF];
static uint8 instruction_field_size[SOC_MAX_NUM_DEVICES][ARAD_PP_OAMP_PE_INSTRUCTION_FIELDS_NOF];
int     sw_state_value;
uint32  u32_sw_state_value;

/* } */
/*************
 *  FUNCTIONS *
 *************/
/* { */

/* Function declerations*/
uint32 _arad_pp_oamp_pe_write_inst_default(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_pph_add_udh(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_pph_add_udh_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_punt_udh_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_default_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_oamp_server(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_oamp_server_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_oamp_server_ccm_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_eth_tlv_on_server(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_one_dm(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_one_dm_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_echo(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_echo_add_udh(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_udp_checksum(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_up_mep_mac(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_11b_maid_end_tlv(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_ipv4_single_hop(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_ipv4_single_hop_add_udh(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_oamts_down_mep(SOC_SAND_IN int unit);
uint32 _arad_pp_oamp_pe_write_inst_micro_bfd_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_bfd_pwe_bos_fix_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_maid_48(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_maid_48_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_maid_48_udh_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_up_mep_mac_mc(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_11b_maid_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_11b_maid_udh_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_default_ccm_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_echo_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_11b_maid_end_tlv_on_server(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_pph_add_udh_ccm_jer(SOC_SAND_IN int);

/* Init the instruction field details by device type */
void _arad_pp_oamp_pe_instruction_field_details_init(int);

/* *****************************************/
/*******STATICS*****************************/
/* *****************************************/



/**
 *  Init all programs and lfem entries.
 *  All are inactive except the default program.
 * May be used for both Jericho and Arad.
 * 
 * param unit 
 * 
 * return  void 
 */
STATIC void _arad_pp_oamp_pe_programs_init(SOC_SAND_IN  int unit) {
    int i;

    last_program_id[unit] = -1; /* At this stage nothing has been inititalized*/
    last_program_pointer[unit] = 0; /*This grows dynamically as instructions are added.*/

    for (i = 0; i < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS; ++i) {
        OAM_ACCESS.prog_used.set(unit, i, -1);
        OAM_ACCESS.mep_pe_profile.set(unit, i, -1);
        programs[unit][i].ls_nibble_select =
        programs[unit][i].ms_nibble_select =
        -1;
        programs[unit][i].instruction_func = NULL;
        OAM_ACCESS.instruction_func_is_valid.set(unit, i, (programs[unit][i].instruction_func != NULL));
    }

    for (i = 0; i < ARAD_PP_OAMP_PE_NUM_LFEM(unit); ++i) {
        lfem_usage[unit][i] = 0;
    }

    _arad_pp_oamp_pe_instruction_field_details_init(unit);
}


/**
 * Init the list of of programs usage. 
 *  
 * Also set the PE const, if applicable. 
 * 
 * param unit 
 * 
 * return STATIC uint32 
 */
STATIC uint32 _arad_pp_oamp_pe_programs_usage_init(SOC_SAND_IN  int  unit) {

    soc_reg_above_64_val_t reg_field;

    uint32  user_header_size, user_header_0_size, user_header_1_size;
    uint32  user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    int     prog = 0;
    uint32  res;
    int     i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    /*In arad+, Load all except the programs push, pop, push_and_pop which are not implemented at this stage.*/
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* Set the instruction function for all the active programs:*/
        res = arad_pmf_db_fes_user_header_sizes_get( unit,
                                                     &user_header_0_size,
                                                     &user_header_1_size,
                                                     &user_header_egress_pmf_offset_0_dummy,
                                                     &user_header_egress_pmf_offset_1_dummy);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        user_header_size = (user_header_0_size + user_header_1_size) / 8;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].instruction_func = _arad_pp_oamp_pe_write_inst_default;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].pe_const = 0;

        if (user_header_size) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH].instruction_func = _arad_pp_oamp_pe_write_inst_pph_add_udh;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH].pe_const = 0;
        }

        if (soc_property_get(unit, spn_OAM_ONE_DM_ENABLE, 1)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func = _arad_pp_oamp_pe_write_inst_one_dm;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].pe_const = 0;
        }

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_server_enable", 1)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].instruction_func = _arad_pp_oamp_pe_write_inst_oamp_server;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].pe_const = 0x000c0ccc; /* Note that server + tlv bug fix uses the same const. */
        }

        programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER].instruction_func = _arad_pp_oamp_pe_write_inst_eth_tlv_on_server;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER].pe_const = 0x000c0ccc;

        if (soc_property_get(unit, spn_BFD_ECHO_ENABLED, 0)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].instruction_func = (user_header_size != 0) ? _arad_pp_oamp_pe_bfd_echo_add_udh : _arad_pp_oamp_pe_bfd_echo;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].pe_const = 0x0ec90000; /* UDP source port, stamped on the packet.*/
        }

        if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop  && SOC_IS_ARADPLUS_A0(unit)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP].instruction_func = (user_header_size != 0) ? _arad_pp_oamp_pe_bfd_ipv4_single_hop_add_udh : _arad_pp_oamp_pe_bfd_ipv4_single_hop;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP].pe_const = 0x0ec80000; /* UDP source port, stamped on the packet.*/
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP].ls_nibble_select = 22;
        }
        else {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].instruction_func = _arad_pp_oamp_pe_bfd_udp_checksum;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].pe_const = 3785; /* UDP source port, stamped on the packet.*/
        }

        if(user_header_size != 0) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP].instruction_func = _arad_pp_oamp_pe_write_inst_oamts_down_mep;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP].pe_const = 0;
        }

        programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].instruction_func = _arad_pp_oamp_pe_write_inst_up_mep_mac;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].pe_const = 0;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].ls_nibble_select = 23;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].ms_nibble_select = 27; /* Aim for the counter pointer which represents the LSB of the src mac address for Up MEPs. */

        /* multicast ITMH down MEP*/
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_itmh_mc", 0)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC].instruction_func = _arad_pp_oamp_pe_write_inst_up_mep_mac_mc;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC].pe_const = 0;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC].ls_nibble_select = 23;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC].ms_nibble_select = 27; /* Aim for the counter pointer which represents the LSB of the src mac address for Up MEPs. */
        }

        programs[unit][ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX].instruction_func = NULL; /* Same program as UP_MEP_MAC. Two MEP-PE profiles needed, thus two programs.*/ 
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX].pe_const = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].pe_const;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX].ls_nibble_select = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].ls_nibble_select;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX].ms_nibble_select = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].ms_nibble_select;

        if (soc_property_get(unit, spn_OAM_MAID_11_BYTES_ENABLE, 0)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].instruction_func = _arad_pp_oamp_pe_write_inst_11b_maid_end_tlv;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].pe_const = 0;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].ls_nibble_select = 23;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].ms_nibble_select = 27; /* Aim for the counter pointer which represents the LSB of the src mac address for Up MEPs. */

            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER].instruction_func = _arad_pp_oamp_pe_write_inst_11b_maid_end_tlv_on_server;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER].pe_const = 0x000c0ccc;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER].ls_nibble_select = 23;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER].ms_nibble_select = 27;
        }

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].instruction_func = _arad_pp_oamp_pe_write_inst_maid_48;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].ls_nibble_select = 23;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].ms_nibble_select = 27;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].pe_const = 0;
        }

        /*Setting the default instruction.*/
        SOC_REG_ABOVE_64_CLEAR(reg_default_instr);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 0);
        SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 0);
        SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
        SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, 0);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, 0);
        SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
        SET_OAMP_PE_BUF_EOP_FRC_BIT(reg_field, 0);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_default_instr, PROG_DATAf, reg_field);
    } else {
        /* Jericho*/
       res = arad_pmf_db_fes_user_header_sizes_get(unit,
                                                   &user_header_0_size,
                                                   &user_header_1_size,
                                                   &user_header_egress_pmf_offset_0_dummy,
                                                   &user_header_egress_pmf_offset_1_dummy);
        user_header_size = (user_header_0_size + user_header_1_size) / 8 ;

        if (SOC_DPP_CONFIG(unit)->pp.oam_statistics) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT_CCM].instruction_func = _arad_pp_oamp_pe_write_inst_default_ccm_jer;
            if(user_header_size) {
                programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_CCM_JER].instruction_func = _arad_pp_oamp_pe_write_inst_pph_add_udh_ccm_jer;
                programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_CCM_JER].pe_const = 0;
            }
        }
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].instruction_func = _arad_pp_oamp_pe_write_inst_default_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].pe_const = 0;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func = _arad_pp_oamp_pe_write_inst_one_dm_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].pe_const = 0;

        if (soc_property_get(unit, spn_BFD_ECHO_ENABLED, 0)) {
           programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].instruction_func = _arad_pp_oamp_pe_bfd_echo_jer;
           programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].pe_const = 0x0ec90000; /* UDP source port, stamped on the packet.*/
        }

        if (SOC_DPP_CONFIG(unit)->pp.oam_statistics) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER].instruction_func = _arad_pp_oamp_pe_write_inst_oamp_server_ccm_jer;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER].pe_const = 0x00010ccc;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER].mep_pe_prof_info.pe_var_16_nibble_select_1= 27;
            }

        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER].instruction_func = _arad_pp_oamp_pe_write_inst_oamp_server_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER].pe_const = 0x00010ccc;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER].mep_pe_prof_info.pe_var_16_nibble_select_1= 27;


        if (user_header_size) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER].instruction_func = _arad_pp_oamp_pe_write_inst_pph_add_udh_jer;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER].pe_const = 0;


            programs[unit][ARAD_PP_OAMP_PE_PROGS_UDH_PUNT_JER].instruction_func = _arad_pp_oamp_pe_write_inst_punt_udh_jer;
            /*Used for setting the FWD offset in PPH*/
            programs[unit][ARAD_PP_OAMP_PE_PROGS_UDH_PUNT_JER].pe_const = ((user_header_size) << 1) & 0x1E;

            programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_UDH_DEFAULT_JER].instruction_func = NULL; /* Same as ARAD_PP_OAMP_PE_PROGS_DEFAULT program */
        }

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
            if (user_header_size) {
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH].instruction_func = _arad_pp_oamp_pe_write_inst_maid_48_udh_jer;
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH].pe_const = 0;
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].instruction_func = _arad_pp_oamp_pe_write_inst_maid_48_jer;
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].pe_const = 0;
            } else {
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].instruction_func = _arad_pp_oamp_pe_write_inst_maid_48_jer;
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].pe_const = 0;
            }
        }

        if (SOC_DPP_CONFIG(unit)->pp.micro_bfd_support==SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4 ||
            SOC_DPP_CONFIG(unit)->pp.micro_bfd_support==SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4_6 ) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MICRO_BFD].instruction_func = _arad_pp_oamp_pe_write_inst_micro_bfd_jer;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MICRO_BFD].pe_const = 6784<<16;  /* On the two higher bytes set the micro BFD UDP dest port (6784)*/
        }
        if (soc_property_get(unit, spn_OAM_MAID_11_BYTES_ENABLE, 0)) {
            if  (user_header_size) {
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH].instruction_func = _arad_pp_oamp_pe_write_inst_11b_maid_udh_jer;
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH].pe_const = 0;
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].instruction_func = _arad_pp_oamp_pe_write_inst_11b_maid_jer;
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].pe_const = 0;
            } else {
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].instruction_func = _arad_pp_oamp_pe_write_inst_11b_maid_jer;
                programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].pe_const = 0;
            }
        }

        if (!SOC_IS_QAX(unit)) {
        /* PWE bos fix: always loaded, but not in QAX*/
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX].instruction_func = _arad_pp_oamp_pe_write_inst_bfd_pwe_bos_fix_jer;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX].pe_const = 0;
        }

        /*If statistic feature is enable we will use 'pe_var_16_nibble_select_0' for for MEP-ID(counter pointer)*/
        if (SOC_DPP_CONFIG(unit)->pp.oam_statistics) {
            for (prog=0 ;prog < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS ; ++prog) {
                programs[unit][prog].mep_pe_prof_info.pe_var_16_nibble_select_0 = 38;
            }
        }

        SOC_REG_ABOVE_64_CLEAR(reg_default_instr);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 0);
        SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 0);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_WR_ALWAYS);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PKT_LNTH);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, 0);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, 0);
        SET_OAMP_PE_BUF_EOP_BITS(reg_field, ARAD_PP_OAMP_PE_EOP_WR_ON_CMP2);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_default_instr, PROG_DATAf, reg_field);
    }

    /* Store instrunction_func validity in sw_state for WarmBoot */
    for (i=0 ; i<ARAD_PP_OAMP_PE_PROGS_NOF_PROGS ; i++) {
        OAM_ACCESS.instruction_func_is_valid.set(unit, i, (programs[unit][i].instruction_func != NULL));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));

}

/**
 * Functions initializes the array of lfem_values to their 
 * proper value. 
 * Arad and Jericho versions 
 * 
 * @param unit 
 */
void _arad_pp_oamp_pe_init_lfems(SOC_SAND_IN int unit) {
    int i;
    /*For now all LFEMS are to be used.*/
    for (i = 0; i < ARAD_PP_OAMP_PE_NUM_LFEM(unit); ++i) {
        lfem_usage[unit][i] = 1;
    }

}


/**
 * Set the mep pe profiles.
 * Arad and Jericho versions 
 * 
 * @author sinai (30/12/2013)
 */
void _arad_pp_oamp_pe_mep_pe_init(SOC_SAND_IN uint32 unit) {
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_DEFAULT, 0); /* Ignored*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_DEFAULT_CCM, 6); /*Used in Jericho only*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER, 1);
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER, 1);
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER, 1); /* Same as server */
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER, 1); /* Same as server */
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_1DM, 3);
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_BFD_ECHO, 0); /* Ignored*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM, 0); /* ignored anyways*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC, 6); /* ARAD+ only*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_UP_MEP_UDH_DEFAULT_JER, 6); /*Used in Jericho Only */
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP, 9); /*must differ from DOWN_MEP_TLV_FIX with only the LSB */
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX, 8);
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP, 0); /* Must be 0. Used for nibble select.*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV, 7);
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH, 0xd);
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER, 2);
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH, 0x0); /* Ignored*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER, 0x0); /* Ignored*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_UDH_PUNT_JER, 0x0); /* Ignored*/
	OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_CCM_JER, 0x0); /* Ignored*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_MICRO_BFD, 0x4);
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX, 0x5);
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_MAID_48, 0xa); /* Used in Jericho only*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH, 0xc); /* Used in Jericho only*/
    OAM_ACCESS.mep_pe_profile.set(unit, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC, 0xb); /* used only when custom_feature_oam_itmh_mc is set*/
}


/**
 * Self explanatory. 
 * 
 * param unit 
 * param prog - program for which to allocate. 
 * 
 * return  uint32 - error number
 */
STATIC uint32 _arad_pp_oamp_pe_allocate_program(SOC_SAND_IN int unit, SOC_SAND_IN  ARAD_PP_OAMP_PE_PROGRAMS  prog) {
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* alloc program number */
    OAM_ACCESS.prog_used.set(unit, prog, ++last_program_id[unit]);


    if (last_program_id[unit] >= ARAD_PP_OAMP_PE_MAX_PROGRAM(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Out of programs.")));
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/** 
 *  Init all programs that are supposed to be loaded, according
 *  to the global variables.
 *  Also set the programs const data.
 * 
 * param unit 
 * 
 * return uint32 - error number
 */
STATIC uint32 _arad_pp_oamp_pe_programs_alloc(SOC_SAND_IN int unit) {
    uint32 res;
    ARAD_PP_OAMP_PE_PROGRAMS prog;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    for (prog=0 ;prog < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS ; ++prog) {
        if (programs[unit][prog].instruction_func) {
            res = _arad_pp_oamp_pe_allocate_program(unit, prog);
            SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);

        }
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * Function is used to compare tcam entries. 
 * tcam1 > tcam2 iff (both tcam entries are valid and tcam1 is 
 * less inclusive than tcam2, by mask) or (tcam1 is valid and 
 * tcam 2 is not). 
 * It is important to note that this is a total order 
 * (otherwise, the set of tcam entries may not be sorted). 
 * 
 * @param tcam1 
 * @param tcam2 
 * 
 * @return  int - Positive if tcam1 > tcam2 , 
 */
STATIC int _arad_pp_tcam_cmp(void *tcam1, void *tcam2) {
    ARAD_PP_TCAM_ENTRY *tcam_ent_1 = (ARAD_PP_TCAM_ENTRY *)tcam1;
    ARAD_PP_TCAM_ENTRY *tcam_ent_2 = (ARAD_PP_TCAM_ENTRY *)tcam2;
    /*First, determine if one is valid and the other is not.*/
    if ((tcam_ent_1->valid != tcam_ent_2->valid) || (tcam_ent_1->valid == 0)) {
        return (tcam_ent_2->valid - tcam_ent_1->valid);
    }

    /* Both are valid. see if one is more inclusive than the other.*/
    if (tcam_ent_1->mask != tcam_ent_2->mask) {
        if ((tcam_ent_1->mask | tcam_ent_2->mask) == tcam_ent_1->mask) {
            return 1;
        }
        if ((tcam_ent_1->mask | tcam_ent_2->mask) == tcam_ent_2->mask) {
            return -1;
        }
    }
    return 0;

}


#define SWAP_INDECIES_IN_ARRAY_OF_TCAM_ENTRIES(arr, ind_1, ind_2) \
    do {\
        ARAD_PP_TCAM_ENTRY tmp = arr[ind_1];\
        arr[ind_1] = arr[ind_2];\
        arr[ind_2] = tmp;\
     } while(0)

#define FIND_MOST_EXCLUSIVE_ENTRY_IN_RANGE(arr, start,end,result)\
         do {\
         int j;\
         result = start;\
         for (j=start; j<end ; ++j) {\
             if (_arad_pp_tcam_cmp(&arr[j], &arr[result]) <0 ) {\
                 result=j;\
             }\
         }\
}while (0) 


/**
 * Sort the array of TCAM indecies from most exclusive to least 
 * exclusive. 
 * 
 * @author sinai (08/12/2014)
 * 
 * @param arr 
 * @param arr_size 
 */
void arad_pp_oamp_pe_sort_tcam_by_mask(ARAD_PP_TCAM_ENTRY * arr,int arr_size){
    int i;
    int cur_max;

    for (i=0 ; i<arr_size ; ++i) {
        /* Find most exclusive entry in places i,...,arr_size and put that entry in the front*/
        FIND_MOST_EXCLUSIVE_ENTRY_IN_RANGE(arr,i,arr_size,cur_max);
        SWAP_INDECIES_IN_ARRAY_OF_TCAM_ENTRIES(arr, i, cur_max); 
    }
}


#define ARAD_PP_OAMP_PE_PTCAM_SIZE_MAX 64

/**
 * Function writes to the tcam table (1.7.2.30 in 
 * arad_plus_registers_internal.pdf) according to the programs 
 * that are active. 
 * Arad and Jericho versions 
 * 
 * 
 * param unit 
 * 
 * return STATIC uint32 - fail or OK.
 */
STATIC uint32 _arad_pp_oamp_pe_write_to_tcam(SOC_SAND_IN int unit) {
    uint32 res;
    int i,  tcam_ind = 0;
    ARAD_PP_TCAM_ENTRY prog_tcam_entries[ARAD_PP_OAMP_PE_PTCAM_SIZE_MAX] = { { 0 } };
    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pmf_db_fes_user_header_sizes_get( unit,
                                                       &user_header_0_size,
                                                       &user_header_1_size,
                                                       &user_header_egress_pmf_offset_0_dummy,
                                                       &user_header_egress_pmf_offset_1_dummy);
    user_header_size = (user_header_0_size + user_header_1_size) / 8 ;

    /*First, filling the tcam entries, one for each program.*/

    /*Default*/
    prog_tcam_entries[tcam_ind].key   = 0;
    prog_tcam_entries[tcam_ind].mask  = _OAMP_PE_TCAM_MASK_DONT_CARE; /* No sense using the macros. Everyhing is "Don't care" */
    prog_tcam_entries[tcam_ind].dat   = 0; /*This is hard coded because 0 should always be the default program.*/
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].instruction_func != NULL;
    ++tcam_ind;

    /*OAMP server (Excluding A+ CCMs which requires TLV fix)*/
    prog_tcam_entries[tcam_ind].key  = 0;
    prog_tcam_entries[tcam_ind].mask = _OAMP_PE_TCAM_MASK_DONT_CARE;
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* GENERATED + RESPONSE */
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    if(!SOC_IS_JERICHO(unit)) {
        SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_LMM_R);
        SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* LMM/R + DMM/R */
    }
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].instruction_func != NULL;
    ++tcam_ind;

    /*OAMP server on Jericho for CCM packets + Statistics. In priority over OAMP_Server for Jericho.*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* GENERATED + RESPONSE */
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* Select only CCMs*/
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER].instruction_func != NULL;
    ++tcam_ind;

    /*OAMP server for Jericho*/
    prog_tcam_entries[tcam_ind].key  = 0;
    prog_tcam_entries[tcam_ind].mask = _OAMP_PE_TCAM_MASK_DONT_CARE;
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* GENERATED + RESPONSE */
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER].instruction_func != NULL;
    ++tcam_ind;


    /*OAMP server + TLV offset fix*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER].instruction_func != NULL;
    ++tcam_ind;

     /*OAMP server + MAID 11B + TLV offset fix*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind,ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER].instruction_func != NULL;
    ++tcam_ind;

    /* 1 DM */
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key,SOC_PPC_OAM_MEP_TYPE_DM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0x0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_1DM);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_DMM_R);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_1DM, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func != NULL;
    ++tcam_ind;

    /*BFD single hop*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 ); /* select mep type 4 (IP m-hop) or 1 (IP 1-hop)*/
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 2);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  =programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP].instruction_func != NULL;
    ++tcam_ind;

    /*BFD Echo*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 ); /* select mep type 4 (IP m-hop) or 1 (IP 1-hop)*/
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_BFD_ECHO);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 2);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_BFD_ECHO, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  =programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].instruction_func != NULL;
    ++tcam_ind;

    /*BFD UDP checksum: 2 entries: 1 for IP one hop, one for IP multi hop*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 2);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  =programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].instruction_func != NULL;
    ++tcam_ind;

    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 2);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  =programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].instruction_func != NULL;
    ++tcam_ind;

    /* DOWN MEP packets with OAM-TS*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* MEP generated and MEP response packets*/
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0x1); /* MEP PE profile selection based: Either this one or ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX */ 
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 0);/* DMM/Rs, LMM/Rs.*/
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0x1 );
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP].instruction_func != NULL;
    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "up mep mac: Inserting into tcam entry with dat =%d, key=%x\n"), prog_tcam_entries[tcam_ind].dat, prog_tcam_entries[tcam_ind].key)); 
    ++tcam_ind;

    /* UP MEP SRC MAC LSB fix + TLV fix*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* MEP generated and MEP response packets*/
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0); /* MEP PE profile based selection*/
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 2);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0x3 ); /* All MEP packets.*/
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].instruction_func != NULL;
    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "up mep mac: Inserting into tcam entry with dat =%d, key=%x\n"), prog_tcam_entries[tcam_ind].dat, prog_tcam_entries[tcam_ind].key));
    ++tcam_ind;



    /* MC ITMH */
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* select mep type 0 (eth oam)*/
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC); /* MEP PE profile based selection*/
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0x3); /* Select only CCMs*/
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC].instruction_func != NULL;
    ++tcam_ind;

    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* MEP generated only*/
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0); /* MEP PE profile based selection*/
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 2);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0x0); /* CCMs only*/
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC, &sw_state_value); /* Uses the same program as the UP_MEP_MAC program. */
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].instruction_func != NULL;
    ++tcam_ind;

    /* MAID 11B*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0); /* MEP PE profile based selection*/
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 2);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );/* Only for CCMs*/
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  =programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].instruction_func != NULL;
    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "internal MAID: Inserting into tcam entry with dat =%d, key=%x\n"), prog_tcam_entries[tcam_ind].dat, prog_tcam_entries[tcam_ind].key));
    ++tcam_ind;

    /* MAID 11B+UDH*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0); /* MEP PE profile based selection*/
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 2);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );/* Only for CCMs*/
    OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH, &sw_state_value);
    prog_tcam_entries[tcam_ind].dat   = sw_state_value;
    prog_tcam_entries[tcam_ind].valid  =programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH].instruction_func != NULL;
    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "internal MAID: Inserting into tcam entry with dat =%d, key=%x\n"), prog_tcam_entries[tcam_ind].dat, prog_tcam_entries[tcam_ind].key));
    ++tcam_ind;

    /* PPH add UDH program. Used for everything except loopback*/
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0x3); /* Observe only the third bit - must be 0 (not 1MEP machine)*/
     SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0xf);
     SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH);
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0x3);
     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH, &sw_state_value);
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH].instruction_func != NULL;
     ++tcam_ind;


     /* Program selection UP MEP with UDH(ARAD_PP_OAMP_PE_PROGS_UP_MEP_UDH_DEFAULT_JER),actually is the same as default program,relevant for Jericho only*/

     prog_tcam_entries[tcam_ind].key  = 0;
     prog_tcam_entries[tcam_ind].mask = _OAMP_PE_TCAM_MASK_DONT_CARE;
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* GENERATED + RESPONSE */
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_UP_MEP_UDH_DEFAULT_JER);
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0x0);
     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_DEFAULT, &sw_state_value);    /* Uses the same program as the ARAD_PP_OAMP_PE_PROGS_DEFAULT program. */
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid  = ((user_header_size > 0) && SOC_IS_JERICHO(unit));
     ++tcam_ind;


     /*Punt*/
     prog_tcam_entries[tcam_ind].key  = 0;
     prog_tcam_entries[tcam_ind].mask = _OAMP_PE_TCAM_MASK_DONT_CARE;
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_PUNT);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0x1); /* PUNT + EVENT packets - Jericho only*/
     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_UDH_PUNT_JER, &sw_state_value);
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_UDH_PUNT_JER].instruction_func != NULL;
     ++tcam_ind;


    /* Program selection UP MEP (ARAD_PP_OAMP_PE_PROGS_UP_MEP_UDH_DEFAULT_JER),actually is the same as default ccm program,relevant for Jericho only (TO ADD STATISTICS MEP-ID)*/
     prog_tcam_entries[tcam_ind].key  = 0;
     prog_tcam_entries[tcam_ind].mask = _OAMP_PE_TCAM_MASK_DONT_CARE;
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* GENERATED + RESPONSE */
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* Select only CCMs*/
     if( user_header_size)
     {
        SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_UP_MEP_UDH_DEFAULT_JER);
        SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0x0);
     }

     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_DEFAULT_CCM, &sw_state_value);
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT_CCM].instruction_func != NULL;
     ++tcam_ind;


     /* PPH add UDH program for Jericho. Used for DOWN-MEP and CCM*/
     prog_tcam_entries[tcam_ind].key  = 0;
     prog_tcam_entries[tcam_ind].mask = _OAMP_PE_TCAM_MASK_DONT_CARE;
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* GENERATED + RESPONSE */
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* Select only CCMs*/
     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_CCM_JER, &sw_state_value);
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_CCM_JER].instruction_func != NULL;
     ++tcam_ind;



     /* PPH add UDH program for Jericho. Used for everything except loopback*/
     prog_tcam_entries[tcam_ind].key  = 0;
     prog_tcam_entries[tcam_ind].mask = _OAMP_PE_TCAM_MASK_DONT_CARE;
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 1); /* GENERATED + RESPONSE */
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0x3);
     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER, &sw_state_value);
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER].instruction_func != NULL;
     ++tcam_ind;


     /*Micro BFD */
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
     SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP);
     SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* select mep type 3 (IP 1-hop)*/
     SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
     SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_MICRO_BFD);
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* Select only CCMs*/
     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_MICRO_BFD, &sw_state_value);
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_MICRO_BFD].instruction_func != NULL; 
     ++tcam_ind;

     /*PWE BOS fix (jericho) */
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
     SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE);
     SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* select mep type 6 (bfd pwe)*/
     SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
     SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX); /* MEP PE profile based selection*/
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* Select only CCMs*/
     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX, &sw_state_value);
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX].instruction_func != NULL;
     ++tcam_ind;

     /*MAID 48 byte, jericho */
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
     SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
     SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* select mep type 0 (eth oam)*/
     SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
     SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_MAID_48); /* MEP PE profile based selection*/
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* Select only CCMs*/
     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_48, &sw_state_value);
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].instruction_func != NULL;
     ++tcam_ind;

     /*MAID 48 byte+UDH, jericho */
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
     SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
     SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
     SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* select mep type 0 (eth oam)*/
     SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
     SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind, ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH); /* MEP PE profile based selection*/
     SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_CCM);
     SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0); /* Select only CCMs*/
     OAM_ACCESS.prog_used.get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH, &sw_state_value);
     prog_tcam_entries[tcam_ind].dat   = sw_state_value;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH].instruction_func != NULL;
     ++tcam_ind;

    /* Sort the array so that the least inclusive masks appear first.*/
    arad_pp_oamp_pe_sort_tcam_by_mask(prog_tcam_entries, tcam_ind);

    tcam_ind=0;
    for (i = 0; i < SOC_DPP_DEFS_GET(unit,oamp_pe_prog_tcam_size); ++i) {
        if (prog_tcam_entries[i].valid) {
            LOG_VERBOSE(BSL_LS_SOC_OAM,
                        (BSL_META_U(unit,
                                    "Iteration %d, Inserting into tcam entry with dat =%d, key=%x\n"),i, prog_tcam_entries[i].dat, prog_tcam_entries[i].key));
            WRITE_TO_TCAM_TABLE(prog_tcam_entries[i], tcam_ind);
            ++tcam_ind;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/* *********************************************************************************/
/**
 * The following 9 functions write the various instructions on 
 * the oamp pe memory table. 
 *  
 * A function for each program. 
 * 
 * @param unit 
 * 
 * @return STATIC uint32 
 */
uint32 _arad_pp_oamp_pe_write_inst_default(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);


    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, 0);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, 0);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_FRC_BIT(reg_field, 0);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);


    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 READ_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ANY, 0, reg_data));

    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ANY, last_program_pointer[unit]++, reg_data));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * Add UDH program. The programm assumes PPH is always present. 
 * OAM-TS may or may not be present. 
 *  
 * 
 * @author sinai (03/05/2015)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_write_inst_pph_add_udh(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    uint32 user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    int fst_inst_addr;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH);


    res = arad_pmf_db_fes_user_header_sizes_get(
               unit,
               &user_header_0_size,
               &user_header_1_size,
               &user_header_egress_pmf_offset_0_dummy,
               &user_header_egress_pmf_offset_1_dummy
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;


/*
     #1 copy 4 bytes to buffer; in_fifo[15:14] -> tmp_reg1[1:0] (using FEM 1)
     #2 `a -> tmp_reg2
     #3 copy 4 bytes to buffer (bytes 5..8);{UDH_len,1'b0} -> tmp_reg3
     #4 copy 2 bytes to buffer (bytes 9,10)
     #5 if (tmp_reg1 == 8'h3), jump to `a (OAM-TS present)
     #6 NOP
     #7 NOP
     #8 copy tmp_reg3 to buffer
     #9  copy 1 byte shifted by 3 to buffer (byte 12)
     #10 copy 4 bytes to buffer (bytes 13..16)
     #11 put zeros for UDH1
     #12 put zeros for UDH2
     #13 copy till the end
     #14 a':copy 2 bytes shifted by 2 to buffer(bytes 11,12)
     #15 copy 4 bytes to buffer (bytes 13..16)
     #16 copy tmp_reg3 to buffer
     #17 copy 3 bytes shifted by 1 to buffer (bytes 18, 19, 20)
     #18 copy 2 bytes to buffer (21,22)
     #19 put zeros for UDH1
     #20 put zeros for UDH2
     #21 copy 2 bytes shifted by 2 to buffer (bytes 23,24)
     #22 copy the rest of the packet
*/



    /*  #1 copy 4 bytes to buffer - system headers (PTCH(2), ITMH(2 of 4); in_fifo[15:14] -> tmp_reg1[1:0] (using FEM 1) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #2 `a -> tmp_reg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 13));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #3 {UDH_len,1'b0} -> tmp_reg3; copy 4 bytes to buffer (bytes 5..8) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, ((user_header_0_size + user_header_1_size) << 1) & 0x1E);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #4 copy 2 bytes to buffer (bytes 9,10) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #5 if (tmp_reg1 == 8'h3), jump to tmp_reg2 (`a) - (OAM-TS present) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);  /*  The value for testing  of 0X0?
     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #6 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #7 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #8 copy tmp_reg3 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #9  copy 1 byte shifted by 3 to buffer (byte 12) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #10 copy 4 bytes to buffer (bytes 13..16) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #11 put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #12 put zeros for UDH2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (user_header_1_size));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #13 copy till the end */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr));
    /*  #14 - a' : #14 copy 2 bytes shifted by 2 to buffer(bytes 11,12) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #15 copy 4 bytes to buffer (bytes 13..16) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #16 copy tmp_reg3 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #17 copy 3 bytes shifted by 1 to buffer (bytes 18, 19, 20) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #18 copy 2 bytes to buffer (21,22) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #19 put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #20 put zeros for UDH2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_1_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #21 copy 2 bytes shifted by 2 to buffer (bytes 23,24) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #22 copy the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}



/**
 * @author (04/04/2016)
 * 
 * @param unit
 * 
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_pph_add_udh_ccm_jer(SOC_SAND_IN int unit) {
    uint32 res;
    int fst_inst_addr;
    int pph_add_udh_first_instruction;
    soc_reg_above_64_val_t reg_data, reg_field;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_CCM_JER);

    pph_add_udh_first_instruction= programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER].first_instr;
    if (pph_add_udh_first_instruction <= 0) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("The program %d is not initialized"), pph_add_udh_first_instruction));
    }

/*Add OAM statistic support and jumps to _arad_pp_oamp_pe_write_inst_pph_add_udh_jer which adds UDH.
    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics))
        {
          1 Copy 2 bytes to the buffer (PTCH-2)
          2 Write 2 bytes MEP-Index from PKT_VAR3 to buffer
          3 Copy 2 bytes shifted by 2 to the buffer
        }
    4 Jump to original program (ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER) */


        /*  #1 Copy 2 bytes to buffer (PTCH-2 header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

 

        /*  #2 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #3 In_fifo[15:14] -> tmp_reg1[1:0] (using FEM 1) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X1);
        SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x20);
        SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

       /*  #4 Copy 2 bytes shifted by 2 to buffer (2 bytes of ITMH header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


        /*  #5 Prepare the address for jump in case: no PPH, no OAM-TS (`b -> tmp_reg4) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, (pph_add_udh_first_instruction + 37));
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #6 Prepare the address for jump in case: no PPH, OAM-TS present (`c -> tmp_reg2) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, (pph_add_udh_first_instruction + 41));
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


     /*  #4 Jump to ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER program */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, pph_add_udh_first_instruction + 7);  /*jumps after adding statistics in _arad_pp_oamp_pe_write_inst_pph_add_udh_jer*/
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #5 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #6 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * @author (03/05/2015)
 * 
 * @param unit 
 * 
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_pph_add_udh_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    uint32 user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    int fst_inst_addr;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER);


    res = arad_pmf_db_fes_user_header_sizes_get(
               unit,
               &user_header_0_size,
               &user_header_1_size,
               &user_header_egress_pmf_offset_0_dummy,
               &user_header_egress_pmf_offset_1_dummy
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;

    /*
    Add UDH program.The program assumes packet structure: ITMH-PPH(optionally)-OAM-TS(optionally) and add a UDH after the last system header.
    When PPH present PPH.fwh header is updated to be equal the size of the UDH.

  if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics))
  {   1 Write 2 MSB bytes to the buffer (PTCH-2)
      2 in_fifo[15:14] -> tmp_reg1[1:0] (using FEM 1)
      3 Write zero byte to buffer
      4 Write zero byte to buffer
      5 Copy 2 bytes shifted
      6 Prepare the address for jump in case: no PPH, no OAM-TS (`b -> tmp_reg4)
      7 Prepare the address for jump in case: no PPH, OAM-TS present (`c -> tmp_reg2)
  } else {
    #1 Copy 4 bytes to buffer - system headers (PTCH(2), ITMH(2 of 4); in_fifo[15:14] -> tmp_reg1[1:0] (using FEM 1)
    #2 Prepare the address for jump in case: no PPH, no OAM-TS (`b -> tmp_reg4)
    #3 Prepare the address for jump in case: no PPH, OAM-TS present (`c -> tmp_reg2)
    }
    #4 {UDH_len,1'b0} -> tmp_reg3;
    #5 Copy 2 bytes to buffer (bytes 9,10)
    #6 if (tmp_reg1 == 8'h0), jump to tmp_reg4 (`b) - (no PPH, no OAM-TS)
    #7 NOP
    #8 NOP
  if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)){
    #9 Prepare the address for jump in case: PPH present, OAM-TS present (`a -> tmp_reg4)
   }else {
    #9 Prepare the address for jump in case: PPH present, OAM-TS present (`a -> tmp_reg4)
    }
    #10 if (tmp_reg1 == 8'h2), jump to tmp_reg2 (`c) - (OAM-TS present, no PPH)
    #11 NOP
    #12 NOP
    #13 if (tmp_reg1 == 8'h3), jump to tmp_reg4 (`a) - (OAM-TS present, PPH present)
    #14 NOP
    #15 NOP
    #16 Copy 2 bytes to buffer
    #17 Copy 2 bytes to buffer
    #18 Copy tmp_reg3 to buffer
    #19 Copy 1 byte shifted by 3 to buffer (byte 12)
    #20 Copy 4 bytes to buffer (bytes 13..16)
    #21 put zeros for UDH1
    #22 put zeros for UDH2
    #23 Copy the rest of packet
    #24 `a Copy 2 bytes to buffer (bytes 9,10) (PPH present, OAM-TS present)
    #25 copy 4 bytes
    #26 Copy 4 bytes to buffer (bytes 13..16)
    #27 Copy tmp_reg3 to buffer
    #28 Copy 3 bytes shifted by 1 to buffer (bytes 18, 19, 20)
    #29 Copy 2 bytes to buffer (21,22)
    #30 Put zeros for UDH1
    #31 Put zeros for UDH2
    #32 Copy 2 bytes shifted by 2 to buffer (bytes 23,24)
    #33 Copy the rest of packet
    #34 'b' Put zeros for UDH1  (no PPH, no OAM-TS)
    #35 Put zeros for UDH2
    #36 Copy 2 bytes shifted by 2 to buffer (bytes 10..12)
    #37 Copy the rest of packet
    #38 Copy 2 bytes shifted by 2 to buffer (bytes 10..12)  (no PPH, OAM-TS present)
    #39 `c Copy 4 bytes to buffer (bytes 9..12)  (no PPH, OAM-TS present)
    #40 put zeros for UDH1
    #41 put zeros for UDH2
    #42 Copy the rest of packet
    */



    if ((SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics))){
        /* #1 Copy 2 bytes to buffer (PTCH-2 header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #2 Write 0x00 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /*  #3 Write 0x00 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /*  #4. In_fifo[15:14] -> tmp_reg1[1:0] (using FEM 1) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X1);
        SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x20);
        SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


        /*  #5 Copy 2 bytes shifted by 2 to buffer (2 bytes of ITMH header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #6 Prepare the address for jump in case: no PPH, no OAM-TS (`b -> tmp_reg4) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 37));
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #7 Prepare the address for jump in case: no PPH, OAM-TS present (`c -> tmp_reg2) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 41));
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

     } else {
        /*  #1 copy 4 bytes to buffer - system headers (PTCH(2), ITMH(2 of 4); in_fifo[15:14] -> tmp_reg1[1:0] (using FEM 1) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X1);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x20);
        SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


        /*  #2 Prepare the address for jump in case: no PPH, no OAM-TS (`b -> tmp_reg4) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 33));
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #3 Prepare the address for jump in case: no PPH, OAM-TS present (`c -> tmp_reg2) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 37));
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
    }
    /*  #4 {UDH_len,1'b0} -> tmp_reg3; */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, ((user_header_0_size + user_header_1_size) << 1) & 0x1E);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #5 Copy 2 bytes to buffer (bytes 9,10) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #6 if (tmp_reg1 == 8'h0), jump to tmp_reg4 (`b) - (no PPH, no OAM-TS)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);  /*  The value for testing  of 0X0? */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #7 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #8 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    if ((SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics))){
    /*  #9 Prepare the address for jump in case: PPH present, OAM-TS present (`a -> tmp_reg4) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 27));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
    } else {
     /*  #9 Prepare the address for jump in case: PPH present, OAM-TS present (`a -> tmp_reg4) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 23));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    }

    /*  #10 if (tmp_reg1 == 8'h2), jump to tmp_reg2 (`c) - (OAM-TS present, no PPH) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);  /*  The value for testing  of 0X0? */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #11 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #12 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #13 if (tmp_reg1 == 8'h3), jump to tmp_reg4 (`a) - (OAM-TS present, PPH present) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);  /*  The value for testing  of 0X0? */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #14 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #15 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #16 Copy 2 bytes to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #17 Copy 2 bytes to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #18 Copy tmp_reg3 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x11);/* MSB nibble for SHIFT,LSB nibble for SIZE_SRC */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #19 Copy 1 byte shifted by 3 to buffer (byte 12) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x30);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #20 Copy 4 bytes to buffer (bytes 13..16) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #21 put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #22 put zeros for UDH2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (user_header_1_size));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                   WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /* # 23 Copy the rest of packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr));

    /*  #24 `a Copy 2 bytes to buffer (bytes 9,10) (PPH present, OAM-TS present)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));



    /*  #25 copy 4 bytes   */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #26 Copy 4 bytes to buffer (bytes 13..16) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #27 Copy tmp_reg3 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x11); /* MSB nibble for SHIFT,LSB nibble for SIZE_SRC */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #28 Copy 3 bytes shifted by 1 to buffer (bytes 18, 19, 20) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x10);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #29 Copy 2 bytes to buffer (21,22) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #30 Put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #31 Put zeros for UDH2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (user_header_1_size));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));



    /*  #32 Copy 2 bytes shifted by 2 to buffer (bytes 23,24) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /* # 33 Copy the rest of packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr));


    /*  #34 Put zeros for UDH1  (no PPH, no OAM-TS)*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #35 Put zeros for UDH2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (user_header_1_size));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

      /*  #36 Copy 2 bytes shifted by 2 to buffer (bytes 10..12) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /* # 37 Copy the rest of packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr));

    /*  #38 Copy 2 bytes shifted by 2 to buffer (bytes 10..12)  (no PPH, OAM-TS present)    */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));



    /*  #39 `c Copy 4 bytes to buffer (bytes 9..12)  (no PPH, OAM-TS present)    */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #40 put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #41 put zeros for UDH2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (user_header_1_size));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /* # 42 Copy the rest of packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr));



exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * Add UDH program for punt/event packets. The programm assumes following packet format:
 * IN:  PTCH-2(2bytes) - ITMH(4) - PPH(7) - FHEI(3) - FTMH(9) - Rest Of Packet
 * OUT: PTCH-2(2bytes) - dummy statistics if needed(2) - ITMH(4) - PPH(7) - FHEI(3) - dummy UDH(according soc properties) -FTMH(9) -  - Rest Of Packet
 * Program also update the Forward-Header-Offset in PPH header that should be equal to UDH size
 *
 * @author MM (30/03/2016)
 *
 * @param unit
 *
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_punt_udh_jer(SOC_SAND_IN int unit) {
    uint32 res;
    int fst_inst_addr;
    soc_reg_above_64_val_t reg_data, reg_field;
    uint32 user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_UDH_PUNT_JER);


    res = arad_pmf_db_fes_user_header_sizes_get(
            unit,
            &user_header_0_size,
            &user_header_1_size,
            &user_header_egress_pmf_offset_0_dummy,
            &user_header_egress_pmf_offset_1_dummy
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;

    /* Program Summary Step-by-Step:
     * #1(1)  Copy PTCH-2(2 bytes)
     * #-(2)  If statistic set '0'(1byte)
     * #-(3)  If statistic set '0'(1byte)
     * #2(4)  Copy ITMH(2 bytes - 0-1)
     * #3(5)  Copy ITMH(2 bytes - 2-3) + PPH(2bytes - 0-1) and fixing FWD offset in PPH
     * #4(6)  Copy PPH(4 bytes - 2-5)
     * #5(7)  Copy PPH(1 byte - 6) + FHEI(3 bytes)
     * #6(8)  Set '0' for udh size
     * #7(9)  Set '0' for udh size
     * #8(10) Copy Rest of packet
     */

    /*  #1 Copy PTCH-2(2 bytes) to the buffer
     * Bytes 0-1
     * */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    if ((SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {
        /*  #2 Write 0x00 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /*  #3 Write 0x00 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    }

    /*  #2(4 - if statistic) Copy 2MSB bytes of ITMH header
     *  Bytes 2-3*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #3(5) Copy 4 bytes(2LSB of ITMH + 2MSB PPH) + fix FWD Header offset according UDH size
     * UDH size is taken from Program variable(see programm initialization)
     * Bytes 4-7
     * */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_OR);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /* Next two instructions copy 8 bytes(each instruction 4 bytes) - 5LSB of PPH and FHEI */
    /*  #4(6)
     *  Bytes 8-11*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #5(7)
     * Bytes 12-15
     * */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #6(8) put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #7(9) put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (user_header_1_size));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /* # 8(10) Copy the rest of packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));

}

/**
 * ARAD+
 * 
 * @author Ori H (04/26/2016)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_write_inst_maid_48(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MAID_48);

/*
    full MAID program
    Description:
        MEPs which require a full MAID will be configured with a dedicated PE profile. The MEP_DB.MAID for these MEPs will be an index to the PE generic memory
        A programmable editor program (selected by the PE profile) will perform the following:
        -   Read 48 consecutive entries from the PE generic memory, starting with the entry at the index  specified in the MEP entry
        -   Replace the MAID on the generated packet with the data read from the generic memory
          

#0  copy in_fifo[15:0] to to tmp_reg1 ** in_fifo[7] holds the up/down bit
#1  put #a to tmp_reg4     
#2  put (pk_var2 & 'h6) to tmp_reg2
#3  put #c to tmp_reg3 
#4  if tmp_reg1 & 'h80 == 0 jump to tmp_reg4 ** if down jump to #a
#5  NOP
#6  NOP
#7  copy 16 bytes, rd in_fifo
#8  jump to #b
#9  NOP
#10 NOP
#11 #a: copy 20 bytes 
#12 #b: if tmp_reg2 == 0, jump to tmp_reg3 ** tmp_reg2 holds the encapsulation, tmp_reg3 is #c
#13 NOP
#14 NOP
#15 copy 4 bytes ** VLAN TAG1
#16 if (tmp_reg2 - 2) == 0, jump to tmp_reg3 ** encapsulation == 1, jump to #c
#17 nop
#18 NOP
#19 copy 4 bytes **VLAN TAG2
#20 copy 4 bytes to buffer (bytes 1-4 of CCM PDU
#21 copy 4 bytes to buffer (bytes 5-8 of CCM PDU)
#22 copy 2 bytes to buffer (bytes 9-10 of CCM PDU)
#23 read entry from in fifo (first 4 bytes of MEG ID - MEG-ID reserved, MEG-ID format, MEG-ID length, first byte of ccm-i
#24 discard (bytes 2,3,4,5 of ccm-icc)
#25 discard oam-icc 4,5,6 bytes  and store in TMP_REG4 MAID
#26 prepare address of `c in TMP_REG3
#27 NOP
#28 clear TMP_REG2
#29 NOP
#30 next address ready in TMP_REG4, read memory at TMP_REG1
#31 increment TMP_REG2 by 1
#32 increment TMP_REG4 by 1 
#33 copy TMP_REG1 to buffer
#34 if TMP_REG2 < 48, jump to TMP_REG3 (`c)
#35 prepare value of 8'h3 in TMP_REG1 
#36 NOP
#37 put 9 to tmp_reg2
#38 put 3 to tmp_reg1
#39 NOP
#40 discard 36 bytes 
#41 copy 12 bytes 
#42 copy 3 bytes (CCM PDU 72 ,73, 74)
#43 Put 8'h0 to buffer, TmpReg1 <= LENGTH - BYTES_RD ** can be 1,5,9
#44 put #d to tmp_reg3 
#45 NOP
#46 NOP
#47 if (TmpReg1 + 1) == 0, jump to tmp_reg3 ** NO TLV 
#48 copy one lower bytes, rd in_fifo 
#49 copy three higher bytes
#50 if TmpReg1- 3 == 0, jump to tmp_reg3 ** one TLV
#51 nop 
#52 nop
#53 copy one lower byte
#54 copy three higher bytes
#55 put 8'h0 to buffer, set EOP
*/ 

/*  #0  copy in_fifo[15:0] to to tmp_reg1 ** in_fifo[7] holds the up/down bit */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X2);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #1  put #a to tmp_reg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 11); /*  should be #a      */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #2  put (pk_var2 & 'h6) to tmp_reg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X6);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #3  put #c to tmp_reg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 20); /*  should be #c      */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #4  if tmp_reg1 & 'h80 == 0 jump to tmp_reg4 ** if down jump to #a */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X80);  /*  The value for testing  of 0X0?     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #5  nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #6  nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #7  copy 16 bytes, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 8);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #8  jump to #b */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 12);  /*  Should be #b     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #9 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #10 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #11 = #a: copy 20 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO); 
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 12);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #12=#b: if tmp_reg2 == 0, jump to tmp_reg3 ** tmp_reg2 holds the encapsulation, tmp_reg3 is #c */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #13	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #14	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #15 copy 4 bytes ** VLAN TAG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #16 if (tmp_reg2 - 2) == 0, jump to tmp_reg3 ** encapsulation == 1, jump to #c */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #17 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #18 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #19 copy 4 bytes **VLAN TAG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #20 copy 4 bytes to buffer (bytes 1-4 of CCM PDU) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #21 copy 4 bytes to buffer (bytes 5-8 of CCM PDU),*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #22 copy 2 bytes to buffer (bytes 9-10 of CCM PDU) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #23 read entry from in fifo (first 4 bytes of MEG ID - MEG-ID reserved, MEG-ID format, MEG-ID length, first byte of ccm-icc)*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #24 discardo (bytes 2,3,4,5 of ccm-icc)*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #25 discard oam-icc 4,5,6 bytes  and store in TMP_REG1 MAID_4  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0X2);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X2);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #26 prepare address of `c in TMP_REG3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 29);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #27 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #28 clear TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
/*  #29 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  `c:#30 next address ready in TMP_REG4, read memory at TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_MEM_OUT); 
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD); 
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #31 increment TMP_REG2 by 1*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
/*  #32 increment TMP_REG4 by 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #33 copy TMP_REG1 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1); 
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM); 
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST); 
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER); 
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP); 
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #34 if TMP_REG2 < 48, jump to TMP_REG3 (`c) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEG);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 48);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #35 prepare value of 8'h3 in TMP_REG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
/*  #36 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #37=#c:put 9 to tmp_reg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 9);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
/*  #38=#c:put 3 to tmp_reg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
/*  #39 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
/*  #40 discard 36 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #41 copy 12 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #42	copy 3 bytes (CCM PDU 72 ,73, 74) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #43 Put 8'h0 to buffer, TmpReg1 <= LENGTH - BYTES_RD ** can be 1,5,9 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #44 put #d to tmp_reg3  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 55); /*  should be #d      */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #45 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #46 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #47 if (TmpReg1 + 1) == 0, jump to tmp_reg3 ** NO TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #48 copy one lower bytes, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #49 copy three higher bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #50 if TmpReg1- 3 == 0, jump to tmp_reg3 ** one TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #51 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #52 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #53 copy one lower byte */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #54 copy three higher bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #55 #d:put 8'h0 to buffer, set EOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


uint32 _arad_pp_oamp_pe_write_inst_oamp_server(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER);



/*  Write lower two bytes of prog. variable to buffer (PTCH_2 outer),  */
/*  copy value of PKT REG0 to TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG0);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP */
/*  NOP, and set TMP_REG1 to 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Read memory and store result in TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_MEM_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  FEM1, SHIFT1 - shift left programm variable by one byte and OR it with TC/DP bits */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X4);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_OR);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
/*  NOP */
/*  Write two bytes from pkt var 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the first byte of the first packet word, removing the two bytes bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write one byte of TMP_REG_4 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the last two bytes of the first packet word, removing the two bytes bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  copy the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));

    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "OAMP Server program successfully loaded!\n")));           

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}



uint32 _arad_pp_oamp_pe_write_inst_one_dm(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_1DM);

    /*
 second version of 
 change the DMM to 1DM PDU program
 Note that.
 1. The system and encapsulation headers overall length is always even
 2. The system and encapsulation headers overall lenght is at least 16
 
 */
/*  Substract DMM PDU pkt size  (45)  8) from the packet length,  */
/*  to find header length, store the result in TMP_REG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);       /*  PKT_LENGTH     */
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 37 + 12);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Prepare address for the JMP in TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 14);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  Prepare address for the JMP in TMP_REG3*/


    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 16);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  loop till TMP_REG1 - bytes_read > 0 */
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  if header length module 4 is not zero, perform the following:*/
/*  1. write 3 lower bytes of the current word to output (2 last bytes of header and MEL+VESION of DMM PDU */
/*  2. write 1DM opcode (45) to the output, */
/*  3. write 1 lower byte of the next word to output (DMM PDU FLAGS */
/*  else */
/*  1. write lower byte of the current word to output ( MEL+VESION of DMM PDU)*/
/*  2. write 1DM opcode (45) to the output, */
/*  3. write the third byte of the current word to output  (DMM PDU FLAGS) */


/*
  prepare 1DM offset (16) in TMP_REG4 
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 16);
    write_instruction_mem(fst_inst_addr + 3, inst_mem_entry);
    

   prepare 1DM opcode (45) in TMP_REG3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 45);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  prepare value 2 for shift in TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  mask all but two lower bits of TMP_REG1 if zero jump to TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);       /*  PKT_LENGTH     */
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Prepare address for the JMP in TMP_REG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 18);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  case of modulo 4 not zero - write 3 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write OPCODE */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write flags from in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  jump to TMP_REG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
/* inst_mem_entry [`OAMP_PE_INST_CONST_BITS]	= (fst_inst_addr + 19); */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  REG2 should point here */
/*  case of modulo 4 equal to zero - write four bytes from in_fifo to output */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write MEL+VERSION to output - one lower byte from IN_FIFO */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  write OPCODE */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  write third byte from in_fifo to output - flags  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_REG2);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  TMP_REG1 should point here */
/*  write offset */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write 3X4 bytes of timestamps (all zero) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  write 4 bytes of timestamp (all zero) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  empty in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, 0);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, 0);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 0);
    SET_OAMP_PE_BUF_EOP_FRC_BIT(reg_field, 0);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/* write END TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUF_EOP_FRC_BIT(reg_field, 1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "1DM program successfully loaded!\n")));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

uint32 _arad_pp_oamp_pe_write_inst_eth_tlv_on_server(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER);

    /*
   TLV bug fix Server program

   This program removes the END_TLV field which is wrongly put between the CCM PDU and the optional PORT/INTERFACE status TLVs
   It places at the end of the packet END_TLV field full of 0's

 */

/*  Write lower two bytes of prog. variable to buffer (PTCH_2 outer),  */
/*  copy value of PKT REG0 to TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG0);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  NOP */
/*  NOP, and set TMP_REG1 to 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Read memory and store result in TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_MEM_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  FEM1, SHIFT1 - shift left programm variable by one byte and OR it with TC/DP bits */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X4);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_OR);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  NOP */
/*  Write two bytes from pkt var 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the first byte of the first packet word, removing the two bytes bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  write one byte of TMP_REG_4 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the last two bytes of the first packet word, removing the two bytes bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/* ---------------------------------------------------------------------------------------------- Server part of the program ends here */
/*  Put the Variable0 (Encapsulation 0 Number of VLANs) in the TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
/* inst_mem_entry [`OAMP_PE_FEM2_SEL_BITS]         = 4'h1; */
/* inst_mem_entry [`OAMP_PE_MUX2_SRC_BITS]         = `OAMP_PE_MUX_SRC_OFFST_REG2; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X6);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  Put The value for the jump in case of 88 bytes in TmpReg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 18));           /*   Jump address for 88 bytes case     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Put the Address for the jump in case of 92 bytes in TmpReg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 25));           /*   Jump address for 92 bytes case     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Put the Address for the jump in case of 96 bytes in TmpReg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 32));           /*   Jump address for 96 bytes case     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* -------------------------------------------------------------------------------------------------------- UP VLANs Test Part */
/* ********************************************** */
/*  */
/*     The packet is Up packet - Test for the VLANs */
/*  */
/* ********************************************** */
/*  Test for 0 VLANs Jump to the address in TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);         /*  Num of VLANs     */
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Test for 1 VLANs Jump to the address in TmpReg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);       /*  Num of VLANs     */
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Test for 2 VLANs Jump to the address in TmpReg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);         /*  Num of VLANs     */
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/* -------------------------------------------------------------------------------------------------------- 88 Bytes Part */
/* ********************************************** */
/*  */
/*  The case of 88 bytes before the PDU */
/*  */
/* ********************************************** */
/*  */
/*  Put the address for the jump in case of two optional TLVs */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 43));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Copy 88Bytes from In-Fifo to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 80);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Test for One or Two opt TLVs */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG1);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_POS);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 95);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Jump to the address for One TLV only */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 40));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));




/* -------------------------------------------------------------------------------------------------------- 92 Bytes Part */
/* ********************************************** */
/*  */
/*  The case of 92 bytes before the PDU */
/*  */
/* ********************************************** */
/*  */
/*  Put the address for the jump in case of two optional TLVs */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 43));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Copy 92Bytes from In-Fifo to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 84);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Test for One or Two opt TLVs */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG1);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_POS);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 99);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Jump to the address for One TLV only */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 40));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/* -------------------------------------------------------------------------------------------------------- 96 Bytes Part */
/* ********************************************** */
/*  */
/*  The case of 96 bytes before the PDU */
/*  */
/* ********************************************** */
/*  */
/*  Put the address for the jump in case of two optional TLVs */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 43));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Copy 96 Bytes from In-Fifo to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 88);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Test for One or Two opt TLVs */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG1);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_POS);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 103);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Jump to the address for One TLV only */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 40));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/* -------------------------------------------------------------------------------------------------------- Common Part */
/* ****************************/

/*  */
/*  Only one TLV is present */
/*  */
/* ****************************/

/*  Copy the next two bytes to the Buffer */
/*  Use the FEM0 to copy the 4'th byte to the third. This will cut the END_TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0X0);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
/* inst_mem_entry [`OAMP_PE_CMP1_ACT_BITS]	      = `OAMP_PE_CMP_ACT_ALWAYS; */
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the one TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the END_TLV.  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* ****************************/

/*  */
/*  Two TLVs are present */
/*  */
/* ****************************/

/*  Copy the next two bytes to the Buffer */
/*  Use the FEM0 to copy the 4'th byte to the third. This will cut the END_TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0X0);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
/* inst_mem_entry [`OAMP_PE_CMP1_ACT_BITS]	      = `OAMP_PE_CMP_ACT_ALWAYS; */
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the first TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the second TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the END_TLV.  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "Ethernet TLV on server program successfully loaded!\n")));
           exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * BFD echo instructions.
 * 
 * @author sinai (17/08/2014)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_bfd_echo(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_BFD_ECHO);



/*
BFD echo programm.
    The program needs to stamp a destination port of 3785 (taken from 16 MSB of programm variable) and UDP checksum of 16'h0 , both at constant position. 
    The packet structure is as follows: PTCH(2), ITMH(4), ITMH-EXT(3),PPH(7), IPV4(20), UDP(8), BFD
    UDP header offset - 36 bytes (PTCH(2),ITMH(4),ITMH-EXT(3), PPH(7),IPV4(20)). Destination port offset is 38 bytes, UDP checksum offset is 42
    Your-descriptor (4 bytes) should be swapped with my-descriptor. My-descriptor is at offset 48, you-descriptor is at offset 52.
*/

    
/*  #1 Copy the first 36 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #2 copy bytes 37,38 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #3 write the two byte of destination port from prog. variable */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #4 copy bytes 41,42 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #5 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #6 copy bytes 45,46,47,48 to buffer (BFD Flags) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #7 copy the bytes 49,50,51,52 to feedback fifo (My descriptor) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #8 copy bytes 53,54,55,56 to buffer (Your descriptor) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #9 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #10 write four bytes from feedback fifo to buffer (My descriptor) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_FDBK_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #11 copy the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr)); 

LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "BFD echo program succesfully loaded!\n")));


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong."))); 
}

/**
 * BFD echo instructions.
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_bfd_echo_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    uint32 user_header_0_size, user_header_1_size, user_header_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_BFD_ECHO);

/*
BFD echo programm.
     The progrm adds UDH header after the PPH and stamps PPH.Offset with the UDH lenghth
 	The program needs to stamp a destination port of 3785 (taken from 16 MSB of programm variable) and UDP checksum of 16'h0 , both at constant position. 
 	The packet structure is as follows: PTCH(2), ITMH(4), ITMH-EXT(3),PPH(7), IPV4(20), UDP(8), BFD
 	UDP header offset - 36 bytes (PTCH(2),ITMH(4),ITMH-EXT(3), PPH(7),IPV4(20)). Destination port offset is 38 bytes, UDP checksum offset is 42
 	Your-descriptor (4 bytes) should be swapped with my-descriptor. My-descriptor is at offset 48, you-descriptor is at offset 52.
*/
    res = arad_pmf_db_fes_user_header_sizes_get(
               unit,
               &user_header_0_size,
               &user_header_1_size,
               &user_header_egress_pmf_offset_0_dummy,
               &user_header_egress_pmf_offset_1_dummy
             );
    user_header_size = (user_header_0_size + user_header_1_size) / 8 ;

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;


      if (user_header_size) {
          if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

            /*#1 Copy 2 bytes to buffer (PTCH-2 header) */
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            SOC_REG_ABOVE_64_CLEAR(reg_field);
            SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
            SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
            SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
            SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
            SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
            SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
            SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
            SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
            SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
            soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

            /*  #2 Write {UDH_len,1'b0} -> tmp_reg3;  */
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            SOC_REG_ABOVE_64_CLEAR(reg_field);
            SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
            SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
            SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
            SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
            SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
            SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
            SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
            SET_OAMP_PE_INST_CONST_BITS(reg_field,  ((user_header_0_size + user_header_1_size) << 1) & 0x1E); /* {0X0,oamp_pe_udh_size[3:0], 0}; */
            soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                        WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

            /*  #3 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            SOC_REG_ABOVE_64_CLEAR(reg_field);
            SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
            SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
            SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
            SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
            SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
            SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
            SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
            SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
            soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

            /*  #4 Copy 2 bytes shifted by 2 to buffer (2 bytes of ITMH header) */
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            SOC_REG_ABOVE_64_CLEAR(reg_field);
            SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
            SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
            SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
            SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
            SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
            SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
            SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
            SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
            SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
            SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
            soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

          } else {

            /*  #0 Copy 4 bytes and {UDH_len,1'b0} -> tmp_reg3;  */
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            SOC_REG_ABOVE_64_CLEAR(reg_field);
            SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
            SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
            SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
            SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
            SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
            SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
            SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
            SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
            SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
            SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
            SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
            SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
            SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
            SET_OAMP_PE_INST_CONST_BITS(reg_field,  ((user_header_0_size + user_header_1_size) << 1) & 0x1E); /* {0X0,oamp_pe_udh_size[3:0], 0}; */
            soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

          }

      /*  #1 Copy 4 bytes to buffer  */
      SOC_REG_ABOVE_64_CLEAR(reg_data);
      SOC_REG_ABOVE_64_CLEAR(reg_field);
      SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
      SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
      SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
      SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
      SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
      SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
      SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
      SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
      SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
      soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


      /*  #2 Copy 2 bytes to buffer */
      SOC_REG_ABOVE_64_CLEAR(reg_data);
      SOC_REG_ABOVE_64_CLEAR(reg_field);
      SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
      SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
      SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
      SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
      SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
      SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
      SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
      SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
      SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
      soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


      /*  #3 Copy tmp_reg3 to buffer */
      SOC_REG_ABOVE_64_CLEAR(reg_data);
      SOC_REG_ABOVE_64_CLEAR(reg_field);
      SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
      SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
      SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
      SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
      SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
      SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
      SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
      SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
      SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
      SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x11); /* MSB nibble for SHIFT,LSB nibble for SIZE_SRC */
      soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


      /*  #4 Copy 1 byte shifted by 3 to buffer (byte 12) */
      SOC_REG_ABOVE_64_CLEAR(reg_data);
      SOC_REG_ABOVE_64_CLEAR(reg_field);
      SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
      SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
      SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
      SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
      SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
      SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
      SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
      SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
      SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
      SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x30);
      soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


      /*  #5 Copy 4 bytes to buffer (bytes 13..16) */
      SOC_REG_ABOVE_64_CLEAR(reg_data);
      SOC_REG_ABOVE_64_CLEAR(reg_field);
      SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
      SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
      SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
      SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
      SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
      SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
      SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
      SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
      SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
      soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


      /*  #6 Put zeros for UDH1 */
      SOC_REG_ABOVE_64_CLEAR(reg_data);
      SOC_REG_ABOVE_64_CLEAR(reg_field);
      SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
      SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
      SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
      SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
      SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
      SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
      SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
      SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
      soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

      /*  #7 Put zeros for UDH2 */
      SOC_REG_ABOVE_64_CLEAR(reg_data);
      SOC_REG_ABOVE_64_CLEAR(reg_field);
      SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
      SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
      SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
      SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
      SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
      SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
      SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
      SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_1_size);
      soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

      } else if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

            /* #1 Copy 2 bytes to buffer (PTCH-2 header) */
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            SOC_REG_ABOVE_64_CLEAR(reg_field);
            SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
            SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
            SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
            SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
            SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
            SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
            SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
            SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
            SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
            soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

           /*  #2 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            SOC_REG_ABOVE_64_CLEAR(reg_field);
            SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
            SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
            SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
            SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
            SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
            SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
            SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
            SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
            soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

            /*  #3 Copy 2 bytes shifted by 2 to buffer (2 bytes of ITMH header) */
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            SOC_REG_ABOVE_64_CLEAR(reg_field);
            SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
            SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
            SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
            SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
            SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
            SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
            SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
            SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
            SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
            SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
            soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
      }

    /*  #1 Copy the first 36 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));          

    /*  #2 copy bytes 37,38 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #3 write the two byte of destination port from prog. variable */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #4 copy bytes 41,42 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #5 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #6 copy bytes 45,46,47,48 to buffer (BFD Flags) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #7 copy the bytes 49,50,51,52 to feedback fifo (My descriptor) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #8 copy bytes 53,54,55,56 to buffer (Your descriptor) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #9 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #10 write four bytes from feedback fifo to buffer (My descriptor) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_FDBK_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #11 copy the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr)); 

LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "BFD echo program succesfully loaded!\n")));


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong."))); 
}

/**
 * BFD echo instructions.
 * 
 * @author sinai (17/08/2014)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_bfd_echo_add_udh(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    uint32 user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_BFD_ECHO);

    res = arad_pmf_db_fes_user_header_sizes_get(
               unit,
               &user_header_0_size,
               &user_header_1_size,
               &user_header_egress_pmf_offset_0_dummy,
               &user_header_egress_pmf_offset_1_dummy
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;



    /*
    BFD echo programm.
        The progrm adds UDH header after the PPH and stamps PPH.Offset with the UDH lenghth
        The program needs to stamp a destination port of 3785 (taken from 16 MSB of programm variable) and UDP checksum of 16'h0 , both at constant position.
        The packet structure is as follows: PTCH(2), ITMH(4), ITMH-EXT(3),PPH(7), IPV4(20), UDP(8), BFD
        UDP header offset - 36 bytes (PTCH(2),ITMH(4),ITMH-EXT(3), PPH(7),IPV4(20)). Destination port offset is 38 bytes, UDP checksum offset is 42
        Your-descriptor (4 bytes) should be swapped with my-descriptor. My-descriptor is at offset 48, you-descriptor is at offset 52.
    */


    /*  #1 Copy 4 bytes - system headers (PTCH(2), ITMH(2 of 4), put instruction const to TmpReg3 - should be UDH size (0-8) with 1'b0
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field,  ((user_header_0_size + user_header_1_size) << 1) &0x1E);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #2 Copy 4 bytes - system headers - ITMH (next 2 of 4), ITMH-EXT (first 2 of 3)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #3 Copy 2 bytes - system headers ITMH-EXT (last 1 of 3), PPH (1 bytes of 7),
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #4 Copy byte from TmpReg3 to buffer (should be the new PPH.Offset)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #5 Copy 1 byte - system headers  PPH (byte 3 bytes of 7),
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #6 copy 4 bytes - the rest of the PPH
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #7 push first part of zero bytes of UDH
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #8  push next part of zero bytes of UDH
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_1_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #9 Copy the first 36 bytes to buffer
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #10copy bytes 37,38 to buffer
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #11 write the two byte of destination port from prog. variable
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #12 copy bytes 41,42 to buffer
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #13 put 16'h0 to buffer (UDP checksum invalid)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #14 copy bytes 45,46,47,48 to buffer (BFD Flags)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #15 copy the bytes 49,50,51,52 to feedback fifo (My descriptor)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #16 copy bytes 53,54,55,56 to buffer (Your descriptor)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #17 NOP
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #18 write four bytes from feedback fifo to buffer (My descriptor)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_FDBK_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #19 copy the rest of the packet
     */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong."))); 
}

/**
 * Bug fix in Arad+.
 * 
 * @author sinai (19/08/2014)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_bfd_udp_checksum(SOC_SAND_IN int unit) {
        uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM);
    
/*
BFD UDP checksum program.
    The program needs to stamp a destination port of 3785 (taken from 16 MSB of programm variable) and UDP checksum of 16'h0 , both at constant position. 
    The packet structure is as follows: PTCH(2), ITMH(4), ITMH-EXT(3),PPH(7), IPV4(20), UDP(8), BFD
    UDP header offset - 36 bytes (PTCH(2),ITMH(4),ITMH-EXT(3), PPH(7),IPV4(20)). Destination port offset is 38 bytes, UDP checksum offset is 42
    Your-descriptor (4 bytes) should be swapped with my-descriptor. My-descriptor is at offset 48, you-descriptor is at offset 52.
*/

/*  #1 Copy the first 36 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 32);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #2 copy bytes 41,42 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #3 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #4 copy the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));


    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "BFD UDP checksum program succesfully loaded!\n")));


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}



uint32 _arad_pp_oamp_pe_bfd_ipv4_single_hop(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP);

#define MULTIHOP_LABEL  13
#define COPY_ROP_LABEL  16
    /*
BFD single hop programm.
    If MEP_DB.Entry.IP-TTL-TOS-Profile==0xf or 0xe the UDP destination port should be changed, UDP.Dest-Port to 3784 (taken from 16 MSB of programm variable)
    and UDP checksum of 16'h0 , both at constant position.
    If  MEP_DB.Entry.IP-TTL-TOS-Profile==0xe the Min Echo RX Interval should be changed to '0' (offset 64)
    The IP-TTL-TOS profile comes in pkt_var2[3:0]
    The incoming packet structure is as follows: PTCH(2), ITMH(4), ITMH-EXT(3),PPH(7), IPV4(20), UDP(8), BFD
    IP header offset is
    UDP header offset - 36 bytes (PTCH(2),ITMH(4),ITMH-EXT(3), PPH(7),IPV4(20)). Destination port offset is 38 bytes, UDP checksum offset is 42

     */

/*
 *                           #0  Mov 4lsb bits of PktVar2(IP-TTL-TOS-Profile) to TmpReg1
 *                           #1  Mov "MultiHop Label" to TmpReg2
 *                           #2  Copy first 36 bytes(Till UDP) to Buffer
 *                           #3  If IP-TTL-TOS-Profile is MultiHop(0x0 - 0xd) jmp to "MultiHop Label"
 * SINGLE HOP CODE ONLY:     #4  NOP
 *                           #5  Mov "SingleHop Label" to TmpReg2
 *                           #6  Copy UDP SrcPort
 *                           #7  Set  UDP DstPort taken from program variable
 *                           #8  Copy UDP Length
 *                           #9  Set UDP checksum (0x0)
 *                           #10 If IP-TTL-TOS-Profile is 0xf jmp to "Copy Rest of Packet Label"
 * SINGLE HOP(with Echo '0') #11 Copy 20 bytes of BFD Header (this instruction will copy 8 bytes of BFD header
 *                                                            if branch was taken on previous instruction)
 *                           #12 Set 0x0 in BFD "Required Min Echo RX Interval" field & sign "End Of Packet" <--- Last Instruction in Single Hop with modification
 * MultiHop CODE ONLY        #13 Copy UDP Src&Dst Port's
 *                           #14 Copy UDP Length
 *                           #15 Set UDP checksum (0x0)
 *                           #16 Copy Rest of Packet
 */


    /*  #0 Clear all bits of PktVar2 except the lower 4, put the result in TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xf);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #1 Put the address for the jump in case of ttl-tos-profile is multi-hop (0x0 - 0xd) to TmpReg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + MULTIHOP_LABEL));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #2 Copy the first 36 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #3 if TmpReg1 is between 0x0 - 0xd, jump to value in TmpReg2(MULTIHOP_LABEL)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEG);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xd);  /*  TTl-Tos profine in bfd ipv4 multi-hop 0x0 - 0xd */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #4 NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #5 Put the address for the jump in case of ttl-tos-profile equal to 4'hf to TmpReg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + COPY_ROP_LABEL));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #6 copy bytes 37,38 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #7 write the two byte of destination port from prog. variable */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #8 copy bytes 41,42 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    /*  #9 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #10 if TmpReg1 is equal to 0xf, jump to value in TmpReg2(COPY_ROP_LABEL)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xf);  /*  TTl-Tos profine in bfd ipv4 single hop is 15 */

    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #11 Copy the 20 bytes of BFD Packet bytes to buffer in case Jump is taken in previous command this instruction will copy only 8 bytes*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 56);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #12 put 32'h0 to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    SET_OAMP_PE_BUF_EOP_FRC_BIT(reg_field,1); /*Set EOP*/
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
    WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    /*Last instruction for Single Hop program that change Program Min Echo Interval to be 0*/

    /* This Part is related to Multi_Hop packets */
    /*  #13 copy bytes 37-40 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #14 copy bytes 41,42 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
    							 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    /*  #15 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
    							 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* Used for Multi Hop packets and Single Hop packets that Min Interval change not required */
    /*  #16 copy the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
    							 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));

#undef MULTIHOP_LABEL
#undef COPY_ROP_LABEL

exit:
	SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

uint32 _arad_pp_oamp_pe_bfd_ipv4_single_hop_add_udh(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    uint32 user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#define MULTIHOP_LABEL 21
#define COPY_ROP_LABEL 24

    res = arad_pmf_db_fes_user_header_sizes_get(
               unit,
               &user_header_0_size,
               &user_header_1_size,
               &user_header_egress_pmf_offset_0_dummy,
               &user_header_egress_pmf_offset_1_dummy
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP);

	/*
    BFD single hop programm.
        Insert UDH header - a sequence of zeros. Put the length of this UDH header into PPH.HdrOffset (which is zero initially
        If MEP_DB.Entry.IP-TTL-TOS-Profile==0xf or 0xe the UDP destination port should be changed, UDP.Dest-Port to 3784 (taken from 16 MSB of programm variable)
        and UDP checksum of 16'h0 , both at constant position.
        If  MEP_DB.Entry.IP-TTL-TOS-Profile==0xe the Min Echo RX Interval should be changed to '0' (offset 64 + udh size)
        The IP-TTL-TOS profile comes in pkt_var2[3:0]
        The incoming packet structure is as follows: PTCH(2), ITMH(4), ITMH-EXT(3),PPH(7), IPV4(20), UDP(8), BFD
        IP header offset is
        UDP header offset - 36 bytes (PTCH(2),ITMH(4),ITMH-EXT(3), PPH(7),IPV4(20)). Destination port offset is 38 bytes, UDP checksum offset is 42 + + udh size

    */

    /*
     *                           #0  Mov 4lsb bits of PktVar2(IP-TTL-TOS-Profile) to TmpReg1
     *                           #1  Mov "MultiHop Label" to TmpReg2
     *                           #2  Copy first 4 bytes + Mov UDH size to TmpReg 3
     *                           #3  Copy 4 bytes (4-7)
     *                           #4  Copy 2 bytes (8-9)
     *                           #5  Put PPH Ofset (From TmpReg3)
     *                           #6  Copy 1 byte (11)
     *                           #7  Copy 4 bytes(12-15)
     *                           #8  Put  first part of UDH header
     *                           #9  Put  second part of UDH header
     *                           #10 Copy IP heaader to Buffer
     *                           #11 If IP-TTL-TOS-Profile is MultiHop(0x0 - 0xd) jmp to "MultiHop Label"
     * SINGLE HOP CODE ONLY:     #12 NOP
     *                           #13 Mov "SingleHop Label" to TmpReg2
     *                           #14 Copy UDP SrcPort
     *                           #15 Set  UDP DstPort taken from program variable
     *                           #16 Copy UDP Length
     *                           #17 Set UDP checksum (0x0)
     *                           #18 If IP-TTL-TOS-Profile is 0xf jmp to "Copy Rest of Packet Label"
     * SINGLE HOP(with Echo '0') #19 Copy 20 bytes of BFD Header (this instruction will copy 8 bytes of BFD header
     *                                                            if branch was taken on previous instruction)
     *                           #20 Set 0x0 in BFD "Required Min Echo RX Interval" field & sign "End Of Packet" <--- Last Instruction in Single Hop with modification
     * MultiHop CODE ONLY        #21 Copy UDP Src&Dst Port's
     *                           #22 Copy UDP Length
     *                           #23 Set UDP checksum (0x0)
     *                           #24 Copy Rest of Packet
     */


    /*  #0 Clear all bits of PktVar2 except the lower 4, put the result in TmpReg1  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xf);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /* #1 Put the address for the jump in case of ttl-tos-profile is multi-hop (0x0 - 0xd) to TmpReg2   */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + MULTIHOP_LABEL));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #2 Copy 4 bytes - system headers (PTCH(2), ITMH(2 of 4), put instruction const to TmpReg3 - should be UDH size (0-8) with 1'b0  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field,  ((user_header_0_size + user_header_1_size) << 1) & 0x1E);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #3 Copy 4 bytes - system headers - ITMH (next 2 of 4), ITMH-EXT (first 2 of 3) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*  #4 Copy 2 bytes - system headers ITMH-EXT (last 1 of 3), PPH (1 bytes of 7), */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /* #5 Copy byte from TmpReg3 to buffer (should be the new PPH.Offset)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #6 Copy 1 byte - system headers  PPH (byte 3 bytes of 7),
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #7 copy 4 bytes - the rest of the PPH
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #8 push first part of zero bytes of UDH
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #9 push next part of zero bytes of UDH
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (user_header_1_size));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /* #10 Copy the rest 20 bytes of the headers, till byte 36
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #11 if TmpReg1 is between 0x0 - 0xd, jump to value in TmpReg2(MULTIHOP_LABEL)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEG);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xd);  /*  TTl-Tos profine in bfd ipv4 multi-hop 0x0 - 0xd */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #12 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #13 Put the address for the jump in case of ttl-tos-profile equal to 4'hf to TmpReg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + COPY_ROP_LABEL));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #14 copy bytes 37,38 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #15 write the two byte of destination port from prog. variable */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #16 copy bytes 41,42 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    /*  #17 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #18 if TmpReg1 is equal to 0xf, jump to value in TmpReg2(COPY_ROP_LABEL)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xf);  /*  TTl-Tos profine in bfd ipv4 single hop is 15 */

    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #19 Copy the 20 bytes of BFD Packet bytes to buffer in case Jump is taken in previous command this instruction will copy only 8 bytes*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 56);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #20 put 32'h0 to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    SET_OAMP_PE_BUF_EOP_FRC_BIT(reg_field,1); /*Set EOP*/
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
    WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    /*Last instruction for Single Hop program that change Program Min Echo Interval to be 0*/

    /* This Part is related to Multi_Hop packets */
    /*  #21 copy bytes 37-40 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #22 copy bytes 41,42 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    /*  #23 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, 0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* Used for Multi Hop packets and Single Hop packets that Min Interval change not required */
    /*  #24 copy the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));

#undef MULTIHOP_LABEL
#undef COPY_ROP_LABEL
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}



/********************************************************************************************/
/* Jericho versions*/
/********************************************************************************************/

/**
 * Jericho default instruction
 * 
 * @author sinai (07/07/2014)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_write_inst_default_jer(SOC_SAND_IN int unit) {
    uint32 res;
    int fst_inst_addr;
    soc_reg_above_64_val_t reg_data, reg_field;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_DEFAULT);
/*Add OAM statistic support
if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics))
{ 1 Write 2 MSB bytes to the buffer (PTCH-2) and store 2 LSB byte to TMP_REG1
  2 Write zero byte to buffer
  3 Write zero byte to buffer
  4 Copy 2 bytes from TMP_REG1 to buffer
}
  5 Copy the rest of packet */

  if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

/*  #1 Write 2 MSB bytes to the buffer and store 2 LSB byte to TMP_REG1 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
        SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #2 Write 0x00 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #3 Write 0x00 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #4 Copy 2 bytes from TMP_REG1 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    }

/*  #5 Copy the rest of paket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_WR_ALWAYS);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PKT_LNTH);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, 0);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, 0);
    SET_OAMP_PE_BUF_EOP_BITS(reg_field, ARAD_PP_OAMP_PE_EOP_WR_ON_CMP2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ANY, last_program_pointer[unit]++, reg_data));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * @author (21/12/2015)
 * 
 * @param unit 
 *  
 * uint32 
 */

uint32 _arad_pp_oamp_pe_write_inst_default_ccm_jer(SOC_SAND_IN int unit) {
    uint32 res;
    int fst_inst_addr;
    soc_reg_above_64_val_t reg_data, reg_field;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_DEFAULT_CCM);

/*Add OAM statistic support
if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics))
{ 1 Write 2 MSB bytes to the buffer (PTCH-2) and store 2 LSB byte to TMP_REG1
  2 Write 2 bytes MEP-Index from PKT_VAR3 to buffer
  3 NOP
  4 Copy 2 bytes from TMP_REG1 to buffer
}
  5 Copy the rest of packet */

if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

/*  #1 Write 2 MSB bytes to the buffer and store 2 LSB byte to TMP_REG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*  #2 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*  #3 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*  #4 Copy 2 bytes from TMP_REG1 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
    }

/*  #5 Copy the rest of paket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_WR_ALWAYS);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PKT_LNTH);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, 0);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, 0);
    SET_OAMP_PE_BUF_EOP_BITS(reg_field, ARAD_PP_OAMP_PE_EOP_WR_ON_CMP2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ANY, last_program_pointer[unit]++, reg_data)); 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * Micro bfd, jericho only.
 * 
 * @author sinai (09/09/2015)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_write_inst_maid_48_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MAID_48);
/*
    full MAID program
    Description:
        MEPs which require a full MAID will be configured with a dedicated PE profile. The MEP_DB.MAID for these MEPs will be an index to the PE generic memory
        A programmable editor program (selected by the PE profile) will perform the following:
        -	Read 24 consecutive entries from the PE generic memory, starting with the entry at the index  specified in the MEP entry
        -	Replace the MAID on the generated packet with the data read from the generic memory
        
    The program uses PDU_OFFSET, as passed to the PE by the TXM. PDU_OFFSET holds the size of the system and encapsulation headers. Hence, the MAID starts 
    10 byte
    Possible values of PDU_OFFSET
        16, 20, 24, 26, 28, 30, 34, 
    The program supports statisrics.   
    #1. PDU_OFFSET & 'h3 -> TMP_REG1	
    #2. copy from fifo to buffer till bytes read < PDU_OFFSET .
    #3. copy 4 bytes to buffer (MEP-ID(2), MEG_ID reserved(1), MEG_ID type(1))
    #4. copy 4 bytes to buffer (MEG-ID length(1), ICC1-3(3))
    #5 copy 4 bytes to buffer ((ICC4-6(3)
    
*/


    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)){
        /* #1 Copy 2 bytes to buffer (PTCH-2 header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


        /*  #2 Write 0x00 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /*  #3 Write 0x00 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /*  #4 Copy 2 bytes shifted by 2 to buffer (2 bytes of ITMH header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

         /*  #5 prepare `a in TMP_REG3 (_arad_pp_oamp_pe_write_inst_maid_48_udh_jer jumps here in case that have statistics)*/
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 17);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    }else {
        /*  #6 Prepare `a in TMP_REG3 (_arad_pp_oamp_pe_write_inst_maid_48_udh_jer jumps here in case that do not have statistics)*/
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 13);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    }

/*  #7. Copy to buffer system and encapsulation headers, and first 8 bytes of CCM PDU */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PDU_OFFS);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 8);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #8 if OAMP_PE_OP_SEL_PDU_OFFS is multiplyer of 4, jump to TMP_REG3(`a) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PDU_OFFS);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #9 NOP*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #10 NOP*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #11 read entry from in fifo (first 4 bytes of MEG ID - MEG-ID reserved, MEG-ID format, MEG-ID length, first byte of ccm-icc) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #12 read entry from in fifo (bytes 2,3,4,5 of ccm-icc)*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    if(SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)){
/*  #13 prepare address of `c in TMP_REG3 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 26);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    }else{
    /*  #13 prepare address of `c in TMP_REG3 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    }


/*  #14 discard byte 6 of ccm-icc, put MAID to TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 00);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X13);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #15  clear TMP_REG2, */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #16 jump to  TMP_REG3  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 24);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #17 NOP*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #18 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #19 `a: handle pdu alligned to 4: copy MEP_ID to buffer, discard MEG-ID reserved byte, MEG-ID format byte */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #20 read from in_fifo 4 bytes and discard them (MEG-ID length, oam-icc 1,2,3 bytes) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #21 discard oam-icc 4,5,6 bytes  and store in TMP_REG1 MAID_1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 00);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X33);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #22 put MAID_2 into TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X2); /*  put byte 3 into byte 0 */
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    if(SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)){
    /*  #23 prepare address of `c in TMP_REG3 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 26);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    }else{
    /*  #23 prepare address of `c in TMP_REG3 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    }

/*  #24 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #25 add TMP_REG1 and TMP_REG2 to get the 16 bits of MAID and store in TMP_REG4*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #26 clear TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #27 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  `c:#28 next address ready in TMP_REG4, read memory at TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_MEM_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #29 increment TMP_REG2 by 1*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #30 increment TMP_REG4 by 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #31 copy TMP_REG1 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #32 if TMP_REG2 < 24, jump to TMP_REG3 (`c) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEG);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 24);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #33 prepare value of 8'h3 in TMP_REG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #34 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #35 read 8 entries from in fifo (previous value of MAID)*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_CST);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 8);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #36 copy two lower bytes, if PDU not alligned to 4*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_WR_ON_N_CMP1);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PDU_OFFS);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_POS);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #37 copy till the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));



exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * MAID 48 + UDH, Jericho only
 * 
 * @author 26/02/2016
 * 
 * @param unit
 * 
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_maid_48_udh_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    uint32 user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    int fst_inst_addr;
    int maid48_first_instruction;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH);

/*
    The program adds UDH after the ITMH header only for "ETH OAM Tx Down Packet".
    The program supports statistics.
    At the end this program jumps to ARAD_PP_OAMP_PE_PROGS_MAID_48 program.

    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {
        #1 Copy 2 bytes to buffer (PTCH-2 header) 
        #2 Write 2 bytes MEP-Index from PKT_VAR3 to buffer
        #3 Copy 2 bytes shifted by 2 to buffer (2 bytes of ITMH header)
    }else {
        #4 Copy 4 bytes to buffer 
    }
    #5 Copy 2 bytes to buffer 
    #6 Put zeros for UDH1
    #7 Put zeros for UDH2
    #8 Copy 2 bytes shifted by 2 to buffer
    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {
        #9 Jump to 5 instruction of ARAD_PP_OAMP_PE_PROGS_MAID_48 program in case it has statistics
    }else{
        #9 Jump to first instruction of ARAD_PP_OAMP_PE_PROGS_MAID_48 program in case that it does not have statistics 
    } 
    #10 NOP 
    #11 NOP 
*/
    res = arad_pmf_db_fes_user_header_sizes_get(
               unit,
               &user_header_0_size,
               &user_header_1_size,
               &user_header_egress_pmf_offset_0_dummy,
               &user_header_egress_pmf_offset_1_dummy
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;

   maid48_first_instruction= programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_48].first_instr;
   if (maid48_first_instruction <= 0) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("The program %d is not initialized"), maid48_first_instruction));
    }

    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

        /*  #1 Copy 2 bytes to buffer (PTCH-2 header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #2 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #3 Copy 2 bytes shifted by 2 to buffer (2 bytes of ITMH header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    }else {
        /*  #4 Copy 4 bytes to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
    }

    /*  #5 Copy 2 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #6 Put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #7 Put zeros for UDH2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_1_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #8 Copy 2 bytes shifted by 2 to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {
        /*  #9 Jump to 5 instruction of ARAD_PP_OAMP_PE_PROGS_MAID_48 program in case it has statistics */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, maid48_first_instruction +4);   /*Add 2B statistics and UDH then jump to full MAID program (ARAD_PP_OAMP_PE_PROGS_MAID_48 program)*/
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
    }else{
        /*  #9 Jump to first instruction of ARAD_PP_OAMP_PE_PROGS_MAID_48 program in case that it does not have statistics */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, maid48_first_instruction);    /*Add UDH then jump to full MAID program (ARAD_PP_OAMP_PE_PROGS_MAID_48 program)*/
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
    }

    /*  #10 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

     /*#11 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * Micro bfd, jericho only.
 * 
 * @author sinai (09/09/2015)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_write_inst_micro_bfd_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    uint32 user_header_0_size, user_header_1_size, user_header_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MICRO_BFD);
    res = arad_pmf_db_fes_user_header_sizes_get(
                    unit,
                    &user_header_0_size,
                    &user_header_1_size,
                    &user_header_egress_pmf_offset_0_dummy,
                    &user_header_egress_pmf_offset_1_dummy
               );
        user_header_size = (user_header_0_size + user_header_1_size) / 8 ;
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        user_header_0_size /=8;
        user_header_1_size /=8;
/*
        micro BFD programm
        Description:
            Micro BFD frames need a different UDP-dest-port.
        Program requirements:
            Change UDP-dest-port to 6784.
        Program selection:
            MEP-TYPE==BFD single hop   AND   MEP-PE-profile.
        Assumptions:
            Packet type is BFD over IPv4 single hop.


     Add OAM statistic and UDH support

    if (user_header_size) {

        if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {
    With UDH, with statistics
            #1 Copy 2 bytes to buffer (PTCH-2 header)
            #2 Write {UDH_len,1'b0} -> tmp_reg3;
            #3 Write 2 bytes MEP-Index from PKT_VAR3 to buffer
            #4 Copy 2 bytes shifted by 2 to buffer (2 bytes of ITMH header)
        } else {
    With UDH, no statistics
            #5 Write {UDH_len,1'b0} -> tmp_reg3 and Copy 4 bytes (PTCH-2 header and 2 bytes of ITMH)
        }
    Common part for  With UDH, with or no statistics
        #6 Copy 4 bytes to buffer ( 2 bytes ITMH , 2 bytes ITMH Base Extension)
        #7 Copy 2 bytes to buffer ( 1 byte ITMH Base Extension,1 byte of PPH header)
        #8 Copy tmp_reg3 to buffer (change the fwd_header_offset of the PPH header)
        #9 Copy 1 byte shifted by 3 to buffer (3th byte of PPH header)
        #10 Copy 4 bytes to buffer  (the rest of the PPH header)
        #11 Put zeros for UDH1
        #12 Put zeros for UDH2
    } else if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {
    No UDH, with statistics
        #13 Write 2 MSB bytes (PTCH-2) to the buffer and store 2 LSB byte to TMP_REG1
        #14 Write 2 bytes MEP-Index from PKT_VAR3 to buffer
        #15 NOP
        #16 Copy 2 bytes from TMP_REG1 to buffer
    }
    No UDH, no statistics(Regular version) and common part for the other cases
    #17 copy to buffer bytes to 36
    #18 copy two buffer bytes 37,38, read the data fifo
    #19 write to buffer first two bytes of the prog. constant
    #20 copy the rest of the packet.
*/

    if (user_header_size) {

        if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

        /* With UDH, with statistics*/

        /*  #1 Copy 2 bytes to buffer (PTCH-2 header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #2 Write {UDH_len,1'b0} -> tmp_reg3; */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field,  ((user_header_0_size + user_header_1_size) << 1) & 0x1E); /* {0X0,oamp_pe_udh_size[3:0], 0}; */
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #3 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #4 Copy 2 bytes shifted by 2 to buffer (2 bytes of ITMH header) */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        } else {

        /* With UDH, no statistics*/

        /*  #5 Write {UDH_len,1'b0} -> tmp_reg3 and Copy 4 bytes (PTCH-2 header and 2 bytes of ITMH)  */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field,ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field,  ((user_header_0_size + user_header_1_size) << 1) & 0x1E); /* {0X0,oamp_pe_udh_size[3:0], 0}; */
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        }
        /* Common part for  With UDH, with or no statistics*/

        /*  #6 Copy 4 bytes to buffer ( 2 bytes ITMH , 2 bytes ITMH Base Extension)  */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


        /*  #7 Copy 2 bytes to buffer ( 1 byte ITMH Base Extension,1 byte of PPH header)*/
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #8 Copy tmp_reg3 to buffer (change the fwd_header_offset of the PPH header)*/
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x11);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


        /*  #9 Copy 1 byte shifted by 3 to buffer (3th byte of PPH header)*/
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x30);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #10 Copy 4 bytes to buffer  (the rest of the PPH header)*/
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #11 Put zeros for UDH1 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #12 Put zeros for UDH2 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_1_size);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    } else if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

        /* No UDH, with statistics*/

        /*  #13 Write 2 MSB bytes (PTCH-2) to the buffer and store 2 LSB byte to TMP_REG1 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
        SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #14 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #15 NOP */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #16 Copy 2 bytes from TMP_REG1 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        }

        /* No UDH, no statistics(Regular version) and common part for the other cases*/

        /* #17 copy to buffer bytes to 36 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /* #18 copy two buffer bytes 37,38, read the data fifo */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /* #19 write to buffer first two bytes of the prog. constant */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /* #20 copy the rest of the packet. */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * 
 * 
 * @author sinai (13/09/2015)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_write_inst_bfd_pwe_bos_fix_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    uint32 user_header_0_size, user_header_1_size, user_header_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX);

    res = arad_pmf_db_fes_user_header_sizes_get(
               unit,
               &user_header_0_size,
               &user_header_1_size,
               &user_header_egress_pmf_offset_0_dummy,
               &user_header_egress_pmf_offset_1_dummy
             );
    user_header_size = (user_header_0_size + user_header_1_size) / 8 ;
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;
/*
      PWE-BOS Fix program.
      Description:  
          For MEPs of type BFD over PWE the BOS bit is always on, whether or not a GAL label is present after the PWE label. 
      Program requirements: 
          When a GAL label is present, the BOS on the PWE label should be turned off.
      Program selection: 
          MEP-PE-profile based.
      Assumptions: 
          Program may assume the structure of the packet is PTCH-o-ITMH-o-PPH-o-PWE-o-GAL-o-GACH-o-BFD.
  
      Pseudo code:
     Add OAM statistic and UDH support

    if (user_header_size) {
        if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {
            #1 Copy 2 bytes to buffer
            #2 Write {UDH_len,1'b0} -> tmp_reg3;
            #3 Write 2 bytes MEP-Index from PKT_VAR3 to buffer
            #4 Copy 2 bytes shifted by 2 to buffer
        } else {
            #5 Write {UDH_len,1'b0} -> tmp_reg3 and Copy 4 bytes
            }
        #6 Copy 4 bytes to buffer
        #7 Copy 2 bytes to buffer
        #8 Copy tmp_reg3 to buffer
        #9 Copy 1 byte shifted by 3 to buffer
        #10 Copy 4 bytes to buffer
        #11 Put zeros for UDH1
        #12 Put zeros for UDH2
    } else if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)){
            #13 Write 2 MSB bytes (PTCH-2) to the buffer and store 2 LSB byte to TMP_REG3
            #14 Write 2 bytes MEP-Index from PKT_VAR3 to buffer
            #15 Prepare 8'h1 in TMP_VAR1
            #16 Copy 2 bytes from TMP_REG3 to buffer
            #17 Copy to buffer bytes 5-16
        } else {
            #18 Prepare 8'h1 in TMP_VAR1
            #19 Copy to buffer bytes 1-16
        }
    #20 Copy to buffer two bytes 17,18. Store in tmp_reg2 bytes 19,20  read the data fifo
    #21 NOP
    #22 NOP
    #23 TMP_REG2(bytes 20,19) - 1 -> TMP_REG2
    #24 NOP
    #25 NOP
    #26 write TMP_REG2 to buffer
    #27 copy the rest of the packet.
*/

if (user_header_size) {

    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

/* With UDH, with statistics*/

/*  #1 Copy 2 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


/*  #2 Write {UDH_len,1'b0} -> tmp_reg3; */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field,  ((user_header_0_size + user_header_1_size) << 1) & 0x1E); /* {0X0,oamp_pe_udh_size[3:0], 0}; */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


/*  #3 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


/*  #4 Copy 2 bytes shifted by 2 to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    } else {

/* With UDH, no statistics*/

/*  #5 Write {UDH_len,1'b0} -> tmp_reg3 and Copy 4 bytes  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field,  ((user_header_0_size + user_header_1_size) << 1) & 0x1E); /* {0X0,oamp_pe_udh_size[3:0], 0}; */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    }
/* Common part for  With UDH, with or no statistics*/

/*  #6 Copy 4 bytes to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


/*  #7 Copy 2 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


/*  #8 Copy tmp_reg3 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x11); /* MSB nibble for SHIFT,LSB nibble for SIZE_SRC */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


/*  #9 Copy 1 byte shifted by 3 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x30);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


/*  #10 Copy 4 bytes to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


/*  #11 Put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*  #12 Put zeros for UDH2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_1_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    } else if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)){   

/* No UDH, with statistics*/

/*#  13 Write 2 MSB bytes (PTCH-2) to the buffer and store 2 LSB byte to TMP_REG3 */
     SOC_REG_ABOVE_64_CLEAR(reg_data);
     SOC_REG_ABOVE_64_CLEAR(reg_field);
     SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
     SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
     SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
     SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
     SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
     SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
     SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
     SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
     SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
     SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
     SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
     SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
     SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
     SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
     SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
     soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
     SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#  14 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
     SOC_REG_ABOVE_64_CLEAR(reg_data);
     SOC_REG_ABOVE_64_CLEAR(reg_field);
     SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
     SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
     SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
     SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
     SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
     SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
     SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
     SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
     soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
     SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*  #15. Prepare 8'h1 in TMP_VAR1 */
     SOC_REG_ABOVE_64_CLEAR(reg_data);
     SOC_REG_ABOVE_64_CLEAR(reg_field);
     SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
     SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
     SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
     SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
     SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
     SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
     SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0);
     SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
     SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
     soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
     SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*  #16 Copy 2 bytes from TMP_REG3 to buffer */
     SOC_REG_ABOVE_64_CLEAR(reg_data);
     SOC_REG_ABOVE_64_CLEAR(reg_field);
     SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
     SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
     SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
     SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
     SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
     SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
     SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
     SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
     SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
     SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
     soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
     SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
              WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*  #17 Copy to buffer bytes 5-16 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 8);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

 } else {

/* No UDH, no statistics (Regular version)*/

/*  #18. Prepare 8'h1 in TMP_VAR1*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #19 Copy to buffer bytes 1-16 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 8);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
 }

/* Common part for all of the cases*/

/* #20 Copy to buffer two bytes 17,18. Store in tmp_reg2 bytes 19,20  read the data fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #21 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #22 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #23 TMP_REG2(bytes 20,19) - 1 -> TMP_REG2*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X100);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #24 NOP*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #25 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #26 Write TMP_REG2 to buffer*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG2);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #27 Copy the rest of the packet.*/
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * OAMP server instruction, jericho.
 * 
 * @author sinai (07/07/2014)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_write_inst_oamp_server_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    uint32 user_header_0_size, user_header_1_size, user_header_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER);
    res = arad_pmf_db_fes_user_header_sizes_get(
                     unit,
                     &user_header_0_size,
                     &user_header_1_size,
                     &user_header_egress_pmf_offset_0_dummy,
                     &user_header_egress_pmf_offset_1_dummy
                );
    user_header_size = (user_header_0_size + user_header_1_size) / 8;
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;

/*   oamp pe server - ITMH in Jericho Style   */
/*   Adds Outer PTCH-2, ITMH and changes imternal PTCH-2 for Remote up-mep  */
/*   injection/response packets  */
/*   ----------------------------------------------------  */
/*   NOP  */
/*   NOP, and set TMP_REG1 to 1  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {
/*   Write lower two bytes of prog . variable to buffer (PTCH_2 outer), and  */
/*   2 Zero bytes  */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR0);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X24);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    } else {
/*   Write lower two bytes of prog . variable to buffer (PTCH_2 outer), and  */
/*   2 Zero bytes  */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR0);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    }
/*   NOP  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   ITMH Jericho Style [31:24]:  */
/*   OR of Prog_Var[23:16] and DP from FEM2 on VAR0  */
/*   Prog_Var[23:16]: Need to shift [23:16] 1 Byte to the left as we ned it at [31:24].   */
/*   Performing isolation of DP field into bits 28:27 with rest zeros (first FEM2 situates DP on  */
/*   [12:11] of FEM2 output, all other bits are zeros, then shift by 2 bytes).  */
/*   after MERGE1 OR, we write 1 Byte to Buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X5);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_OR);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG1);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X21);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   ITMH Jericho Style[23:8]:  */
/*   Write two bytes from pkt var 1  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   ITMH Jericho Style [7:0]- OR of Prog_Var[31:24] and TC from FEM2 on VAR0  */
/*   Prog_Var[31:24]: Need no shift, as it is needed a 31:24.   */
/*   Performing isolation of TC field into bits 27:25 with rest zeros (first FEM2 situates TC on  */
/*   [11:9] of FEM2 output, all other bits are zeros, then shift by 2 bytes).  */
/*   after MERGE1 OR, we write 1 Byte to Buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X6);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_OR);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X21);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    if (user_header_size) {
/*   Write user_header_0_size bytes of zeros  */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   Write user_header_1_size bytes of zeros  */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_1_size);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    }
/*  write the first byte of the first packet word, removing the two bytes bytes  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   write one byte of PKT_VAR_4 to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0X7);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X31);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write the last two bytes of the first packet word, removing the two bytes bytes  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0X1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X20);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   copy the rest of the packet  */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * OAMP server instruction, jericho.
 * 
 * @author  (09/05/2016)
 * 
 * @param unit 
 * 
 * @return uint32 
 */

uint32 _arad_pp_oamp_pe_write_inst_oamp_server_ccm_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER);

/*   Write lower two bytes of prog . variable to buffer (PTCH_2 outer),   */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR0);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   Write the 2 bytes of VAR3 to buffer (Statistics MEP-Index),   */
/*   In addition Write the Constant 1 into Reg1  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field,  ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
/*  inst_mem_entry[`OAMP_PE_SHIFT1_SRC_BITS]	= `OAMP_PE_SHIFT_SRC_CONST;  */
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0X1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X01);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   Jump to server_jer program after the insertion of the statistics  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER].first_instr + 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   NOP  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*   NOP  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}




/**
 * 1DM program, jericho.
 * 
 * @author sinai (07/07/2014)
 * 
 * @param unit 
 * 
 * @return uint32 
 */

uint32 _arad_pp_oamp_pe_write_inst_one_dm_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_1DM);

/*
 second version of 
 change the DMM to 1DM PDU program
 Note that.
 1. The system and encapsulation headers overall length is always even
 2. The system and encapsulation headers overall lenght is at least 16
 
 */
/*  Substract DMM PDU pkt size  (45)  8) from the packet length,*/
/*  to find header length, store the result in TMP_REG1*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PDU_OFFS);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 12);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Prepare address for the JMP in TMP_REG2*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 15);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  prepare opcode 'd45 in MSB*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
/* inst_mem_entry [`OAMP_PE_OP1_SEL_BITS]		= `OAMP_PE_OP_SEL_INST_VAR;*/
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_INST_VAR_MSB);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 45);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* ** NOP*/
/* inst_mem_entry = `OAMP_PE_INSTRUCT_WIDTH'h0;*/
/* inst_mem_entry[`OAMP_PE_CMP2_ACT_BITS]		= `OAMP_PE_CMP_ACT_NEVER;*/
/* inst_mem_entry[`OAMP_PE_INST_SRC_BITS]		= `OAMP_PE_INST_SRC_NXT_JMP;*/
/* write_instruction_mem (fst_inst_addr+2,inst_mem_entry);*/


/*  loop till TMP_REG1 - bytes_read > 0*/
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_WR_ALWAYS);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  if header length module 4 is not zero, perform the following:*/
/*  1. write 3 lower bytes of the current word to output (2 last bytes of header and MEL+VESION of DMM PDU*/
/*  2. write 1DM opcode (45) to the output,*/
/*  3. write 1 lower byte of the next word to output (DMM PDU FLAGS)*/
/*  else*/
/*  1. write lower byte of the current word to output ( MEL+VESION of DMM PDU)*/
/*  2. write 1DM opcode (45) to the output, */
/*  3. write the third byte of the current word to output  (DMM PDU FLAGS) */


/*  mask all but two lower bits of TMP_REG1 if zero jump to TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PDU_OFFS);       /*  PKT_LENGTH  */
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  NOP  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  case of modulo 4 not zero - write 3 bytes  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write OPCODE  */
/*  write first byte of in_fifo - flags  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
/* inst_mem_entry [`OAMP_PE_MUX1_SRC_BITS]		= `OAMP_PE_MUX_SRC_TMP_REG3;  */
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
/* inst_mem_entry [`OAMP_PE_MERGE1_INST_BITS]	= `OAMP_PE_MERGE_INST_FEM;  */
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_CON);
/* inst_mem_entry[`OAMP_PE_SHIFT1_SRC_BITS]	= `OAMP_PE_BUFF_SIZE_SRC_CONST;  */
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
/* inst_mem_entry [`OAMP_PE_BUFF_SIZE_SRC_BITS]= `OAMP_PE_BUFF_SIZE_SRC_CONST;  */
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_BUFF2_SIZE_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF2_SIZE_1B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 45);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));




/*  write offset 'd16  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
/* inst_mem_entry [`OAMP_PE_MUX1_SRC_BITS]		= `OAMP_PE_MUX_SRC_TMP_REG4;  */
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
/* inst_mem_entry[`OAMP_PE_SHIFT1_SRC_BITS]	= `OAMP_PE_BUFF_SIZE_SRC_CONST;  */
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
/* inst_mem_entry [`OAMP_PE_BUFF_SIZE_SRC_BITS]= `OAMP_PE_BUFF_SIZE_SRC_CONST;  */
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X10);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  write 16 bytes of timestamp (all zero)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
/* inst_mem_entry [`OAMP_PE_BUFF_SIZE_SRC_BITS]= `OAMP_PE_BUFF_SIZE_SRC_CONST;  */
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
/* inst_mem_entry [`OAMP_PE_INST_SRC_BITS]		= `OAMP_PE_INST_SRC_NXT_JMP;  */
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_CST);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  empty 4 bytes from in fifo first  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
/* inst_mem_entry [`OAMP_PE_INST_SRC_BITS]		= `OAMP_PE_INST_SRC_LOP_CST;  */
/* inst_mem_entry [`OAMP_PE_INST_CONST_BITS]	= 8'h2;  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  empty 4 bytes from in fifo first  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
/* inst_mem_entry [`OAMP_PE_INST_SRC_BITS]		= `OAMP_PE_INST_SRC_LOP_CST;  */
/* inst_mem_entry [`OAMP_PE_INST_CONST_BITS]	= 8'h2;  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write END TLV  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUF_EOP_BITS(reg_field, ARAD_PP_OAMP_PE_EOP_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  REG2 should point here  */
/*  case of modulo 4 equal to zero - write four bytes from in_fifo to output  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write MEL+VERSION to output - one lower byte from IN_FIFO  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  write OPCODE  */
/* write  third byte from in_fifo to output - flags  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG3);
/* inst_mem_entry [`OAMP_PE_MUX1_SRC_BITS]		= `OAMP_PE_MUX_SRC_INST_VAR;`OAMP_PE_MUX_SRC_TMP_REG3  */
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
/* inst_mem_entry [`OAMP_PE_MERGE1_INST_BITS]	= `OAMP_PE_MERGE_INST_FEM;  */
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_CON);
/* inst_mem_entry[`OAMP_PE_SHIFT1_SRC_BITS]	= `OAMP_PE_BUFF_SIZE_SRC_CONST;  */
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
/* inst_mem_entry [`OAMP_PE_BUFF1_SIZE_SRC_BITS]= `OAMP_PE_BUFF1_SIZE_SRC_VLD_B;  */
    SET_OAMP_PE_BUFF2_SIZE_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF2_SIZE_1B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X21);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));




/*  write offset 'd16  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
/* inst_mem_entry [`OAMP_PE_MUX1_SRC_BITS]		= `OAMP_PE_MUX_SRC_TMP_REG4;  */
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
/* inst_mem_entry[`OAMP_PE_SHIFT1_SRC_BITS]	= `OAMP_PE_BUFF_SIZE_SRC_CONST;  */
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
/* inst_mem_entry [`OAMP_PE_BUFF_SIZE_SRC_BITS]= `OAMP_PE_BUFF_SIZE_SRC_CONST;  */
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X10);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  write 16 bytes of timestamp (all zero)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
/* inst_mem_entry [`OAMP_PE_BUFF_SIZE_SRC_BITS]= `OAMP_PE_BUFF_SIZE_SRC_CONST;  */
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
/* inst_mem_entry [`OAMP_PE_INST_SRC_BITS]		= `OAMP_PE_INST_SRC_NXT_JMP;  */
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_CST);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  empty 16 bytes from in fifo first  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 0);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
/* inst_mem_entry [`OAMP_PE_INST_SRC_BITS]		= `OAMP_PE_INST_SRC_NXT_JMP;  */
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_CST);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/* write END TLV  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUF_EOP_BITS(reg_field, ARAD_PP_OAMP_PE_EOP_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * up-MEP mac restriction, Arad+.
 *
 * @author AvivG (10/23/2014)
 *
 * @param unit
 *
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_up_mep_mac(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    uint32 user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

     res = arad_pmf_db_fes_user_header_sizes_get(
                unit,
                &user_header_0_size,
                &user_header_1_size,
                &user_header_egress_pmf_offset_0_dummy,
                &user_header_egress_pmf_offset_1_dummy
              );
     SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
     user_header_0_size /=8;
     user_header_1_size /=8;


    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC);

/*
   TLV bug fix program
        For UP-MEP packets the program rewrites 8 lsb of the MAC SA with a value, taken from the gen_mem. gen_mem is accessed with the local port value (PktVar0[7:0]
   This program removes the END_TLV field which is wrongly put between the CCM PDU and the optional PORT/INTERFACE status TLVs
   It places at the end of the packet END_TLV field full of 0's

    These instructions are related to ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC program as well.

0    copy in_fifo[15:0] & 'h8 to to tmp_reg1 / *  in_fifo[7] holds the up/down bit * /
1   put #a to tmp_reg4        
2    put (pk_var2 & 'h6) to tmp_reg2
3    put #c to tmp_reg3
4    if down jump to #a
5    nop
6    nop
7    copy 12 bytes, rd in_fifo
8    copy byte 13,
9    put PktVar[2][15:8] to buffer
10    copy bytes 15,16, rd in_fifo
    11        prepare jump, put address of #e in tmp_reg4
    12        nop
    13        nop 
    14        if length > 70, jump to end
15    jump to #b
16    nop
17    nop
18   #a: copy 2 bytes PTCH
    19   copy 4 bytes ITMH
    20   copy UDH_0
    21   copy UDH_1
    22   copy ETH SA,DA(in loop)
    23   copy ETH Ether_type
24    #b: if tmp_reg2 == 0, jump to tmp_reg3 / *  tmp_reg2 holds the encapsulation #c * /
25    nop
26    nop
27    copy 4 bytes / *  VLAN TAG1 * /
28    if (tmp_reg2 - 2) == 1, jump to tmp_reg3 / *  encapsulation == 1, jump to #c * /
29    nop
30    nop
31    copy 4 bytes / * VLAN TAG2 * /
32    #c:put 24 to tmp_reg1
33    nop
34    nop
35    tmp_reg1 <= tmp_reg - 4, copy in_fifo to buffer, loop till alu_out != 0
36    copy 2 bytes (CCM PDU 73, 74)
37    TmpReg1 <= LENGTH - BYTES_RD / *  can be 1,5,9 * /
38    nop
39    nop
40    if (TmpReg1 - 1) == 0, jump to tmp_reg3 / *  NO TLV * /
41    put #d to tmp_reg3
42    nop
43    nop
44    copy two lower bytes
45    copy two higher bytes
46    if TmpReg1- 5 == 0, jump to tmp_reg3
47    nop
48    nop
49    copy two lower bytes
50    copy two higher bytes
51    #d:put 8'h0 to buffer, set EOP
52   copy rest of packet
 */


/*  #0	copy in_fifo[15:0] to to tmp_reg1 ** in_fifo[7] holds the up/down bit */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X2);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #1	put #a to tmp_reg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 18); /*  should be #a */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #2	put (pk_var2 & 'h6) to tmp_reg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X6);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #3	put #c to tmp_reg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 32); /*  should be #c */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #4	if tmp_reg1 & 'h80 == 0 jump to tmp_reg4 ** if down jump to #a */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
/* inst_mem_entry [`OAMP_PE_FEM2_SEL_BITS]         = 4'h1; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X80);  /*  The value for testing  of 0X0? */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #5	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #6	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #7	copy 12 bytes, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #8	copy byte 13 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #9 put PktVar[2][15:8] to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG2);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #10 copy bytes 15,16, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #11 Put the address for the jump in case of packet length is less than 70 in TmpReg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 51));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #12,13 Add two NOPs*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    /*  #14 if REG4 is less than 70 , jump to value in TmpReg4  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4); /* REG4: packet size*/
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEG);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 70);  /*  Packets of size < 70 are Not CCMs. Jump to the end in this case.  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data)); 


    /*  #15 jump to #b */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 24);  /*  Should be #b */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #16 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #17	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #18 = #a: copy 4 bytes: PTCH + 2 bytes ITMH */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #19 Copy 2 bytes - end of ITMH, do not read from fifo */
    /* The instructons below support ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC program. */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
            SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
        SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #20 push first part of zero bytes of UDH */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #21 push next part of zero bytes of UDH */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (user_header_1_size));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #22: copy Ethertype */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                    WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /* #23: copy 12 bytes: SA, DA  */
     SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 12);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #24=#b: if tmp_reg2 == 0, jump to tmp_reg3 ** tmp_reg2 holds the encapsulation, tmp_reg3 is #c */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #25	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #26nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  #27 copy 4 bytes ** VLAN TAG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #28	if (tmp_reg2 - 2) == 0, jump to tmp_reg3 ** encapsulation == 1, jump to #c */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #29	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #30	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #31	copy 4 bytes **VLAN TAG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #32=#c:put 24 to tmp_reg1 */
/* TLV fix start and it supports also ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC program.*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 24); /*  should be #c */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #33	copy 4 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #34	copy 4 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #35 copy 64 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #36	copy 2 bytes (CCM PDU 73, 74) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #37 TmpReg1 <= LENGTH - BYTES_RD ** can be 1,5,9 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X6);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #38 put #d to tmp_reg3  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 51); /*  should be #d */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #39	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #40	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #41	if (TmpReg1 + 1) == 0, jump to tmp_reg3 ** NO TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #42 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #43 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #44 copy one lower bytes, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #45 copy three higher bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #46	if TmpReg1- 3 == 0, jump to tmp_reg3 ** one TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #47	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #48 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #49 copy one lower byte */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #50 copy three higher bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #51	#d:put 8'h0 to buffer, set EOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
/* inst_mem_entry [`OAMP_PE_MUX1_SRC_BITS]	      = `OAMP_PE_MUX_SRC_OFFST_REG2; */
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #52, copy rest of packet (default instruction)*/
	SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
								 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}




/**
 * Arad+ ITMH with MC destination.
 *
 * @author Liat Amir (11/23/2015)
 *
 * @param unit
 *
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_up_mep_mac_mc(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    int uc_program_first_instruction;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC);

    /* Check that programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].first_instr has been updated */

    /* Set to uc_program_first_instruction the first instructoin address of ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC program*/
    uc_program_first_instruction= programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].first_instr;

/*
   TLV bug fix program
    For DOWN-MEP MC packets the program reset 2 bits(18,19) of FWD_DEST_INFO in ITMH header (field encoding will be Multicast-ID) + TLV fix, which is done by ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC program.


#0 Set tmp reg 2 to 0xff
#1 Set tmp reg 1 to 0xf3
#2 NOP
#3 Shift r2 in 2 bytes
#4 NOP
#5 NOP
#6 r1=r2+r1
#7 = copy PTCH
#8 Put (pk_var2 & 'h6) to tmp_reg2
#9 2 bytes ITMH + tmp 1 (clear mc bit)
#10 In TMP_REG3 is saved the instruction number that hold start of TLV fix part
#11 Jump to common part of UC program
#12 NOP
#13 NOP */

    /*#0 Set tmp reg 2 to 0xff*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0xFF);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #1 Set tmp reg 1 to 0xf3*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0xF3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #2 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #3 Shift r2 in 2 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG2);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #4 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #5 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #6 r1=r2+r1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #7 = copy PTCH */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #8 Put (pk_var2 & 'h6) to tmp_reg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X6);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #9 2 bytes ITMH + tmp 1 (clear mc bit)*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #10 In TMP_REG3 is saved the instruction number that hold start of TLV fix part */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, uc_program_first_instruction+32);   /*The instruction number that hold start of TLV fix part located in UC program*/
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #11 Jump to common part of UC program */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, uc_program_first_instruction+19);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #12 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #13 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * 11 bytes MAID internal support, Arad+.
 *
 * @author AvivG (11/06/2014)
 *
 * @param unit
 *
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_11b_maid_end_tlv(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV);

    /*  #0	copy in_fifo[15:0] to to tmp_reg1 ** in_fifo[7] holds the up/down bit */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X2);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #1	put #a to tmp_reg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 14); /*  should be #a      */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #2	put (pk_var2 & 'h6) to tmp_reg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X6);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #3	put #c to tmp_reg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 23); /*  should be #c      */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #4	if tmp_reg1 & 'h80 == 0 jump to tmp_reg4 ** if down jump to #a */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
/* inst_mem_entry [`OAMP_PE_FEM2_SEL_BITS]         = 4'h1; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X80);  /*  The value for testing  of 0X0?     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #5	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #6	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #7	copy 12 bytes, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #8	copy byte 13 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #9 put PktVar[2][15:8] to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG2);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #10 copy bytes 15,16, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    /*  #11 jump to #b */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 15);  /*  Should be #b     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #12 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #13	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #14 = #a: copy 20 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 12);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #15=#b: if tmp_reg2 == 0, jump to tmp_reg3 ** tmp_reg2 holds the encapsulation, tmp_reg3 is #c */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #16	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #17	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  #18 copy 4 bytes ** VLAN TAG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #19	if (tmp_reg2 - 2) == 0, jump to tmp_reg3 ** encapsulation == 1, jump to #c */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #20	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #21	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #22	copy 4 bytes **VLAN TAG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #23=#c:put 16 to tmp_reg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 16);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #24	copy 4 bytes to buffer (bytes 1-4 of CCM PDU), put 4 to tmp_reg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #25	copy 4 bytes to buffer (bytes 5-8 of CCM PDU), put 2 to tmp_reg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #26 copy 2 bytes to buffer (bytes 9-10 of CCM PDU) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #27	write tmp_reg2[7:0] to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG2);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #28	copy 3 bytes from in_fifo, shifted by one, read in_fifo (bytes 14-16 of CCM PDU) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #29	copy 3 bytes from in_fifo, (bytes 17-19) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #30	write tmp_reg4[7:0] to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #31	copy 1 byte from in_fifo, shifted by three, read in_fifo (bytes 20) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #32	copy 4 bytes to buffer (bytes 21-24 of CCM PDU) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #33 copy 48 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #34	copy 2 bytes (CCM PDU 73, 74) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #35 Put 8'h0 to buffer, TmpReg1 <= LENGTH - BYTES_RD ** can be 1,5,9 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #36 put #d to tmp_reg3  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 49); /*  should be #d      */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #37	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #38	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #39	if (TmpReg1 + 1) == 0, jump to tmp_reg3 ** NO TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #40 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  41# nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #42 copy one lower bytes, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #43 copy three higher bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #44	if TmpReg1- 3 == 0, jump to tmp_reg3 ** one TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #45	nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #46 nop */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #47 copy one lower byte */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #48 copy three higher bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #49	#d:put 8'h0 to buffer, set EOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 *  TLV bug fix Server program + 11 bytes MAID internal
support,Arad+
 *
 *@author (18/01/2016)
 *
 * @param unit
 *
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_11b_maid_end_tlv_on_server(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER);

/* 1. The program add OAMP server
   2. The program rewrites MEGID as following:
        new_MEGID[0] = 'd4
        new_MEGID[1 - 6] = old_MEGID[3 - 8]
        new_MEGID[7] = 'd2
        new_MEGID[8,9,10,11,12,13,14] = old_MEGID[9,10,11,12,13,14,15]
        new_MEGID[15-47] = {33{8'h0}}
    3. The program removes the END_TLV field which is wrongly put between the CCM PDU and the optional PORT/INTERFACE status TLVs.
       It places at the end of the packet END_TLV field full of 0's


#0 Write lower two bytes of prog. variable to buffer (PTCH_2 outer),
    copy value of PKT REG0 to TMP_REG4
#1  NOP
    NOP, and set TMP_REG1 to 1
#2  NOP
#3  Read memory and store result in TMP_REG4
#4  FEM1, SHIFT1 - shift left programm variable by one byte and OR it with TC/DP bits
#5  NOP
    Write two bytes from pkt var 1
#6  write the first byte of the first packet word, removing the two bytes bytes
#7  write one byte of TMP_REG_4 to buffer
#8  write the last two bytes of the first packet word, removing the two bytes bytes
 ---------------------------------------------------------------------------------------------- Server part of the program ends here
#9 Put 4 to TMP_REG1  (prepare value for # copy 16 bytes)
#10 Put (pk_var2 & 'h6) to TMP_REG2 == Put the Variable0 (Encapsulation 0 Number of VLANs) in the TMP_REG2
#11 Put #a to TMP_REG3
#12 Copy 16 bytes
#13 If TMP_REG2 == 0, jump to TMP_REG3 ** TMP_REG2 holds the encapsulation, TMP_REG3 is #a
#14 NOP
#15 NOP
#16 Copy 4 bytes ** VLAN TAG1
#17 If TMP_REG2 - 2 == 0, jump to TMP_REG3 ** encapsulation == 1, jump to #a
#18 NOP
#19 NOP
#20 Copy 4 bytes **VLAN TAG2
#21 #a: Put 3 to TMP_REG3 **prepare the value for the loop which copy 36 bytes
#22 Copy 4 bytes to buffer (bytes 1-4 of CCM PDU), put 4 to TMP_REG2
#23 Copy 4 bytes to buffer (bytes 5-8 of CCM PDU), put 2 to TMP_REG4
#24 Copy 2 bytes to buffer (bytes 9-10 of CCM PDU)
#25 Write TMP_REG2 to buffer
#26 Copy 3 bytes from in_fifo, shifted by one, read in_fifo (bytes 14-16 of CCM PDU)
#27 Copy 3 bytes from in_fifo, (bytes 17-19)
#28 Write TMP_REG4 to buffer
#29 Copy 1 byte from in_fifo, shifted by three, read in_fifo (bytes 20)
#30 Copy 4 bytes to buffer (bytes 21-24 of CCM PDU)
#31 Copy 4 bytes to buffer (bytes 25-28 of CCM PDU)
#32 Write 0x00 to buffer
#33 Copy 36 bytes to buffer (bytes 29-64 of CCM PDU )
#34 Copy 4 bytes to buffer (bytes 65-68 of CCM PDU )
#35 Copy 4 bytes to buffer (bytes 69-72 of CCM PDU )
 -------------------------------------------------------------------------------------------------- End of MAID11 part of the program
#36 Copy 2 bytes (CCM PDU 73, 74)
#37 Put in TMP_REG1 = LENGTH - BYTES_RD ** can be 1,3,7
#38 Put #b to TMP_REG3
#39 NOP
#40 NOP
#41 If TMP_REG1 + 1 == 0, jump to TMP_REG3 ** NO TLV
#42 NOP
#43 NOP
#44 Copy one lower bytes, rd in_fifo
#45 Copy three higher bytes
#46 If TMP_REG1 - 3 == 0, jump to TMP_REG3 ** one TLV
#47 NOP
#48 NOP
#49 Copy one lower bytes, rd in_fifo
#50 Copy three higher bytes
#51 #b: Put 8'h0 to buffer, set EOP
*/


/* #0 Write lower two bytes of prog. variable to buffer (PTCH_2 outer),  */
/*  copy value of PKT REG0 to TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG0);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#1  NOP */
/*  NOP, and set TMP_REG1 to 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#2  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#3  Read memory and store result in TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_MEM_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#4  FEM1, SHIFT1 - shift left programm variable by one byte and OR it with TC/DP bits */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X4);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG0);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_OR);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#5  NOP */
/*  Write two bytes from pkt var 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#6 write the first byte of the first packet word, removing the two bytes bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#7  write one byte of TMP_REG_4 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#8 write the last two bytes of the first packet word, removing the two bytes bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
/* ---------------------------------------------------------------------------------------------- Server part of the program ends here */

/*#9  Put 4 to TMP_REG1  (prepare value for # copy 16 bytes) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#10  Put (pk_var2 & 'h6) to TMP_REG2 == Put the Variable0 (Encapsulation 0 Number of VLANs) in the TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X6);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#11  Put #a to TMP_REG3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 21); /*  should be #a  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#12  Copy 16 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#13  If TMP_REG2 == 0, jump to TMP_REG3 ** TMP_REG2 holds the encapsulation, TMP_REG3 is #a */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#14  NOP  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#15  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#16  Copy 4 bytes ** VLAN TAG1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#17  If TMP_REG2 - 2 == 0, jump to TMP_REG3 ** encapsulation == 1, jump to #a */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#18  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#19  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#20  Copy 4 bytes **VLAN TAG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#21  #a: Put 3 to TMP_REG3 **prepare the value for the loop which copy 36 bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#22  Copy 4 bytes to buffer (bytes 1-4 of CCM PDU), put 4 to TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#23  Copy 4 bytes to buffer (bytes 5-8 of CCM PDU), put 2 to TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#24  Copy 2 bytes to buffer (bytes 9-10 of CCM PDU) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#25  Write TMP_REG2 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG2);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#26  Copy 3 bytes from in_fifo, shifted by one, read in_fifo (bytes 14-16 of CCM PDU) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#27  Copy 3 bytes from in_fifo, (bytes 17-19) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#28  Write TMP_REG4 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#29  Copy 1 byte from in_fifo, shifted by three, read in_fifo (bytes 20) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#30  Copy 4 bytes to buffer (bytes 21-24 of CCM PDU) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#31 Copy 4 bytes to buffer (bytes 25-28 of CCM PDU) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#32  Write 0x00 to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#33  Copy 36 bytes to buffer (bytes 29-64 of CCM PDU ) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG3);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#34  Copy 4 bytes to buffer (bytes 65-68 of CCM PDU ) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#35  Copy 4 bytes to buffer (bytes 69-72 of CCM PDU ) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
/* ----------------------------------------------------------------------------------------------------- End of MAID11 part of the program */

/*#36  Copy 2 bytes (CCM PDU 73, 74) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#37  Put in TMP_REG1 = LENGTH - BYTES_RD ** can be 1,3,7  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_REG4);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#38  Put #b to TMP_REG3  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 51);  /*  should be #b */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#39  NOP  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#40  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#41  If TMP_REG1 + 1 == 0, jump to TMP_REG3 ** NO TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#42  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#43 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#44  Copy one lower bytes, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#45  Copy three higher bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#46  If TMP_REG1 - 3 == 0, jump to TMP_REG3 ** one TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#47  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#48  NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#49  Copy one lower bytes, rd in_fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#50  Copy three higher bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

/*#51  #b: Put 8'h0 to buffer, set EOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * 11 bytes MAID internal support, Jericho
 *
 * @author (01/12/2015)
 *
 * @param unit
 *
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_11b_maid_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV);

/*Add OAM statistic support
if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics))
{ 1 Write 2 MSB bytes to the buffer (PTCH-2) and store 2 LSB byte to TMP_REG1
  2 Write 2 bytes MEP-Index from PKT_VAR3 to buffer
  3 NOP
  4 Copy 2 bytes from TMP_REG1 to buffer
  5 Put the address of the part when PDU_OFFS%4=2 (#a) to TMP_REG4 (fst_instr_addr+21)
}

else  {
1    Put the address of the part when PDU_OFFS%4=2 (#a) to TMP_REG4 (fst_instr_addr+17)
}
2    Store PDU_OFFS & 3 in TMP_REG3
3    Read PDU_OFFS bytes and copy 8 bytes to buffer
4    If TMP_REG3-2=0 jump to TMP_REG4
5    NOP
6    NOP
7    Write 2 MSB bytes and discard 2 LSB bytes
8    Write 0x04 to buffer (first number)
9    Shifted 3 bytes left and copy them to buffer
10    Write 3 MSB bytes to the buffer and store 1 LSB byte to TMP_REG2
11    Write 0x02 to buffer (second number)
12    NOP
13    Copy 1 byte from TMP_REG2 to buffer
14    Copy 4 bytes to buffer
15    Copy 4 bytes to buffer
16    Write 0x00 to buffer (third number)
17    Copy the rest of packet

18    (#a) Write 0x04 to buffer (first number)
19    Shifted 1 byte 3 times left and copy it to buffer
20    Copy 4 bytes to buffer
21    Write 1 MSB byte to buffer and store 3 bytes to feedback fifo
22    Write 0x02 to buffer (second number)
23    NOP
24    Read from feedback fifo and write to buffer
25    Copy 4 bytes to buffer
26    Write 0x00 to buffer (third number)
27    Copy the rest of packet
*/

    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

/*  #1 Write 2 MSB bytes to the buffer (PTCH-2) and store 2 LSB byte to TMP_REG1 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
        SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #2 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #3 NOP */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #4 Copy 2 bytes from TMP_REG1 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #5 Put the address of the part when PDU_OFFS%4=2 (#a) to TMP_REG4 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 21);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    } else {
/*  #1 Put the address of the part when PDU_OFFS%4=2 (#a) to TMP_REG4 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 17);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    }
    
/*  #2 Store PDU_OFFS & 3 in TMP_REG3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PDU_OFFS);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);             /*  mask bits [n:2], leave bits [1:0] for the future testing */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #3 Read PDU_OFFS bytes and copy 8 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PDU_OFFS);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #4 If TMP_REG3-2=0 jump to TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG3);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NPOS);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #5 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #6 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #7 Write 2 MSB bytes and discard 2 LSB bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #8 Write 0x04 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x04);   /* first number */
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #9 Shifted 3 bytes left and copy them to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x13);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #10 Write 3 MSB bytes to the buffer and store 1 LSB byte to TMP_REG2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x33);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #11 Write 0x02 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);   /* second number */
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #12 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #13 Copy 1 byte from TMP_REG2 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG2);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x01);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #14 Copy 4 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #15 Copy 4 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #16 Write 0x00 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);   /* third number */
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* # 17 Copy the rest of packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));

/*  #18 Write 0x04 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x04);   /* first number */
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #19 Shifted 1 byte 3 times left and copy it to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x31);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #20 Copy 4 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #21 Write 1 MSB byte to buffer and store 3 bytes to feedback fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x01);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #22 Write 0x02 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x02);   /* second number */
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #23 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #24 Read from feedback fifo and write to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_FDBK_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x13);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #25 Copy 4 bytes to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #26 Write 0x00 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_INST_VAR);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x00);   /* third number */
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #27 Copy till the rest of packet   */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr)); 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * 11 bytes MAID internal support + UDH, Jericho
 *
 * @author (03/03/2016)
 *
 * @param unit
 *
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_11b_maid_udh_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    uint32 user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    int maid11_first_instruction;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH);

    res = arad_pmf_db_fes_user_header_sizes_get(
               unit,
               &user_header_0_size,
               &user_header_1_size,
               &user_header_egress_pmf_offset_0_dummy,
               &user_header_egress_pmf_offset_1_dummy
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;

    maid11_first_instruction= programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].first_instr;

/*The program adds UDH after the ITMH header only for "ETH OAM Tx Down Packet".
  It supports the cases with and without oam_statistics. At the end this program jumps to ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV program.
Add OAM statistic support
if  (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics))
{   #0 Copy 2 bytes to buffer
    #1 Write 2 bytes MEP-Index from PKT_VAR3 to buffer
    #2 Copy 2 bytes shifted by 2 to buffer
    #3 Copy 2 bytes to buffer
    #4 Put zeros for UDH1
    #5 Put zeros for UDH2
    #6 Copy 2 bytes shifted by 2 to buffer
    #7 Put the address of the part when PDU_OFFS%4=2 (#a) to TMP_REG4
    #8 Jump to MAID 11 program
    #9 NOP
    #10 NOP
}

else  {
    #0 Copy 4 bytes to buffer
    #1 Copy 2 bytes to buffer
    #2 Put zeros for UDH1
    #3 Put zeros for UDH2
    #4 Copy 2 bytes shifted by 2 to buffer
    #5 Put the address of the part when PDU_OFFS%4=2 (#a) to TMP_REG4
    #6 Jump to MAID 11 program
    #7 NOP
    #8 NOP
}
*/

    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.oam_statistics)) {

        /*  #0 Copy 2 bytes to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #1 Write 2 bytes MEP-Index from PKT_VAR3 to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR3);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /*  #2 Copy 2 bytes shifted by 2 to buffer  */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #3 Copy 2 bytes to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #4 Put zeros for UDH1 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #5 Put zeros for UDH2 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_1_size);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #6 Copy 2 bytes shifted by 2 to buffer  */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #7 Put the address of the part when PDU_OFFS%4=2 (#a) to TMP_REG4 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, maid11_first_instruction + 21);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /*  #8 Jump to MAID 11 program */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, maid11_first_instruction+5);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #9 NOP */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*10 NOP */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    } else {
        /*  #0 Copy 4 bytes to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #1 Copy 2 bytes to buffer */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #2 Put zeros for UDH1 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #3 Put zeros for UDH2 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_1_size);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #4 Copy 2 bytes shifted by 2 to buffer  */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
        SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
        SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
        SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
        SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 0x22);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #5 Put the address of the part when PDU_OFFS%4=2 (#a) to TMP_REG4 */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, maid11_first_instruction + 17);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

        /*  #6 Jump to MAID 11 program */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
        SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
        SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        SET_OAMP_PE_INST_CONST_BITS(reg_field, maid11_first_instruction+2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*  #7 NOP */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

        /*#8 NOP */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
        SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                     WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 *
 *
 * @author sinai&mark (30/03/2014)
 *
 * @param unit
 *
 * @return uint32
 */
uint32 _arad_pp_oamp_pe_write_inst_oamts_down_mep(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    uint32 user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP);

    res = arad_pmf_db_fes_user_header_sizes_get(
               unit,
               &user_header_0_size,
               &user_header_1_size,
               &user_header_egress_pmf_offset_0_dummy,
               &user_header_egress_pmf_offset_1_dummy
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    user_header_0_size /=8;
    user_header_1_size /=8;


    /*  #1 Copy 4 bytes - system headers - PTCH-2 (2 bytes) +  ITMHT (first 2 of 4)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #2 Copy 4 bytes - system headers - ITMHT (last 2 of 4) + OAM-TS (first 2 of 6)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #3 Copy 4 bytes - system headers - ITMHT (last 4 of 6)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #4 push first part of zero bytes of UDH */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, user_header_0_size);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #5 push next part of zero bytes of UDH */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_FEM_DEF_PR(unit));
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (user_header_1_size));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #6 copy the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_default_instr));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/* *********************************************************************************/




/**
 * Function writes to the program memory table 
 * according to the programs that are active.
 *  
 * 
 * param unit 
 * 
 * return STATIC uint32 - fail or OK.
 */
STATIC uint32 _arad_pp_oamp_pe_write_to_pe_prog_mem(SOC_SAND_IN int unit) {
    uint32 res;
    ARAD_PP_OAMP_PE_PROGRAMS prog;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /*Run whichever program is defined to be active.*/

    /*start with default which should always be active*/

    for (prog=0 ;prog < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS ; ++prog) {
        if (programs[unit][prog].instruction_func) {
            res = programs[unit][prog].instruction_func(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);

        }
    }




exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * Function finds the relevant nibble select profile fields
 * according to the program pe profile.
 *
 * params
 *      unit
 *      prog_pe_profile - Program PE profile value
 *      ls_field - (OUT) PE_LS_NIBBLE_SELECT_PROF_xf
 *      ms_field - (OUT) PE_MS_NIBBLE_SELECT_PROF_xf
 *
 * return STATIC uint32 - fail or OK.
 */
STATIC uint32 _arad_pp_oamp_pe_nibble_select_profile_get(
   SOC_SAND_IN  int             unit,
   SOC_SAND_IN  uint32          prog_pe_profile,
   SOC_SAND_OUT soc_field_t     *ls_field,
   SOC_SAND_OUT soc_field_t     *ms_field
   ) {

    /* Use array to select the right field alias since they're not defined in order */
    soc_field_t arr_ls_nibble_select_f[16] = {
        PE_LS_NIBBLE_SELECT_PROF_0f,PE_LS_NIBBLE_SELECT_PROF_1f,
        PE_LS_NIBBLE_SELECT_PROF_2f,PE_LS_NIBBLE_SELECT_PROF_3f,
        PE_LS_NIBBLE_SELECT_PROF_4f,PE_LS_NIBBLE_SELECT_PROF_5f,
        PE_LS_NIBBLE_SELECT_PROF_6f,PE_LS_NIBBLE_SELECT_PROF_7f,
        PE_LS_NIBBLE_SELECT_PROF_8f,PE_LS_NIBBLE_SELECT_PROF_9f,
        PE_LS_NIBBLE_SELECT_PROF_10f,PE_LS_NIBBLE_SELECT_PROF_11f,
        PE_LS_NIBBLE_SELECT_PROF_12f,PE_LS_NIBBLE_SELECT_PROF_13f,
        PE_LS_NIBBLE_SELECT_PROF_14f,PE_LS_NIBBLE_SELECT_PROF_15f,
    };

    /* Use array to select the right field alias since they're not defined in order */
    soc_field_t arr_ms_nibble_select_f[16] = {
        PE_MS_NIBBLE_SELECT_PROF_0f,PE_MS_NIBBLE_SELECT_PROF_1f,
        PE_MS_NIBBLE_SELECT_PROF_2f,PE_MS_NIBBLE_SELECT_PROF_3f,
        PE_MS_NIBBLE_SELECT_PROF_4f,PE_MS_NIBBLE_SELECT_PROF_5f,
        PE_MS_NIBBLE_SELECT_PROF_6f,PE_MS_NIBBLE_SELECT_PROF_7f,
        PE_MS_NIBBLE_SELECT_PROF_8f,PE_MS_NIBBLE_SELECT_PROF_9f,
        PE_MS_NIBBLE_SELECT_PROF_10f,PE_MS_NIBBLE_SELECT_PROF_11f,
        PE_MS_NIBBLE_SELECT_PROF_12f,PE_MS_NIBBLE_SELECT_PROF_13f,
        PE_MS_NIBBLE_SELECT_PROF_14f,PE_MS_NIBBLE_SELECT_PROF_15f,
    };

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(ls_field);
    SOC_SAND_CHECK_NULL_INPUT(ms_field);

    if (prog_pe_profile >= ARAD_PP_OAMP_PE_NOF_PROG_SEL_PROFILES(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Undefined program pe profile: %d."), prog_pe_profile));
    }

    /* Return needed values */
    *ls_field = arr_ls_nibble_select_f[prog_pe_profile];
    *ms_field = arr_ms_nibble_select_f[prog_pe_profile];

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}



soc_error_t arad_pp_oam_oamp_pe_gen_mem_set(int unit, int gen_mem_index, int gen_mem_data){

    uint32 res,reg,reg_val =gen_mem_data ;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_OAMP_PE_GEN_MEMm_field_set(unit, &reg, PE_GEN_DATAf, &reg_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_OAMP_PE_GEN_MEMm(unit, MEM_BLOCK_ALL, gen_mem_index, &reg)); 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * Function writes the nibble select according to the programs
 * profile.
 *
 * param unit
 *
 * return STATIC uint32 - fail or OK.
 */
STATIC uint32 _arad_pp_oamp_pe_write_nibble_selection_profiles(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t   reg_val;
    ARAD_PP_OAMP_PE_PROGRAMS prog;
    uint32 program_profile;
    soc_field_t ls_prof_f=0, ms_prof_f=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS_A0(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error: Arad+ function called for other device (_arad_pp_oamp_pe_write_nibble_selection_profiles)")));
    }

    for (prog=0 ;prog < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS ; ++prog) {
        if (programs[unit][prog].instruction_func || prog==ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX) { 
            /* Special dispensation. DOWN_MEP_TLV_FIX Program needs MEP-PE-Profile but not program.*/
            /* Program is active - if necessary, load the nibble select values */

            /* Get program profile */
            arad_pp_oamp_pe_program_profile_get(unit, prog, &program_profile);

            /* Find relevant fields for the program profile */
            res = _arad_pp_oamp_pe_nibble_select_profile_get(unit, program_profile, &ls_prof_f, &ms_prof_f);
            SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

            if (programs[unit][prog].ls_nibble_select != -1) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_NIBBLE_SELECT_FOR_PEr(unit, reg_val));
                soc_reg_above_64_field32_set(unit, OAMP_NIBBLE_SELECT_FOR_PEr, reg_val, ls_prof_f,
                                             programs[unit][prog].ls_nibble_select);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_NIBBLE_SELECT_FOR_PEr(unit, reg_val));
            }
            if (programs[unit][prog].ms_nibble_select != -1) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_NIBBLE_SELECT_FOR_PEr(unit, reg_val));
                soc_reg_above_64_field32_set(unit, OAMP_NIBBLE_SELECT_FOR_PEr, reg_val, ms_prof_f,
                                             programs[unit][prog].ms_nibble_select);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_OAMP_NIBBLE_SELECT_FOR_PEr(unit, reg_val));
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/**
 * Function write the pe profile information according to the
 * programs profile.
 *
 * param unit
 *
 * return STATIC uint32 - fail or OK.
 */
STATIC uint32 _jer_pp_oamp_pe_write_pe_profiles_info(SOC_SAND_IN int unit) {
    uint32 res;
    ARAD_PP_OAMP_PE_PROGRAMS prog;
    uint32 program_profile;
    uint32 pe_prof_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error: Jericho function called for other device (_jer_pp_oamp_pe_write_pe_profiles_info)")));
    }

    for (prog=0 ;prog < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS ; ++prog) {
        OAM_ACCESS.prog_used.get(unit, prog, &sw_state_value);
        if (sw_state_value != -1) {
            /* Program is active and uses the pe_profile data */

            /* Get program profile */
            arad_pp_oamp_pe_program_profile_get(unit, prog, &program_profile);

            /* Load the data */
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_PE_PROFr(unit, program_profile, &pe_prof_data));

            soc_reg_field_set(unit, OAMP_PE_PROFr, &pe_prof_data, PE_VAR_8_NIBBLE_SELECT_0_PROF_Nf,
                              programs[unit][prog].mep_pe_prof_info.pe_var_8_nibble_select_0);
            soc_reg_field_set(unit, OAMP_PE_PROFr, &pe_prof_data, PE_VAR_8_NIBBLE_SELECT_1_PROF_Nf,
                              programs[unit][prog].mep_pe_prof_info.pe_var_8_nibble_select_1);
            soc_reg_field_set(unit, OAMP_PE_PROFr, &pe_prof_data, PE_VAR_16_NIBBLE_SELECT_0_PROF_Nf,
                              programs[unit][prog].mep_pe_prof_info.pe_var_16_nibble_select_0);
            soc_reg_field_set(unit, OAMP_PE_PROFr, &pe_prof_data, PE_VAR_16_NIBBLE_SELECT_1_PROF_Nf,
                              programs[unit][prog].mep_pe_prof_info.pe_var_16_nibble_select_1);

            soc_reg_field_set(unit, OAMP_PE_PROFr, &pe_prof_data, PE_MEP_INSERT_PROF_Nf,
                              programs[unit][prog].mep_pe_prof_info.pe_mep_insert);

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_OAMP_PE_PROFr(unit, program_profile, pe_prof_data));
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/************************************************/
/* **********************************************/
/***********************************************/

#define SET_PE_FIRST_INSRTUCTION_PTR(index, instr_ptr,reg_data) \
    do {\
    soc_reg_above_64_val_t reg_field={0};\
    reg_field[0] = instr_ptr;\
    if (SOC_IS_JERICHO(unit)) {\
        uint32 reg32=0;\
        soc_reg_field_set(unit,OAMP_PE_PTRr,&reg32, PE_PTR_Nf,instr_ptr );\
        SOC_SAND_SOC_IF_ERROR_RETURN(res,50,exit, WRITE_OAMP_PE_PTRr(unit,index,reg32));\
    }\
    else {\
        uint32 pe_ptr_fields[] = { PE_PTR_0f, PE_PTR_1f, PE_PTR_2f, PE_PTR_3f, PE_PTR_4f, PE_PTR_5f, PE_PTR_6f, PE_PTR_7f };\
        soc_reg_above_64_field_set(unit, OAMP_PE_FIRST_INSTr, reg_data, pe_ptr_fields[index], reg_field);\
    }\
} while (0)
    
#define SET_PE_CONST_DATA(index, const_data,reg_data) \
    do {\
    soc_reg_above_64_val_t reg_field={0};\
    reg_field[0] = const_data;\
    if (SOC_IS_JERICHO(unit)) {\
        uint32 reg32=0;\
        soc_reg_field_set(unit,OAMP_PE_CONSTr,&reg32, PE_CONST_Nf,const_data );\
        SOC_SAND_SOC_IF_ERROR_RETURN(res,50,exit, WRITE_OAMP_PE_CONSTr(unit, index,reg32));\
    }\
    else {\
        uint32 pe_const_fields[] = { PE_CONST_0f, PE_CONST_1f, PE_CONST_2f, PE_CONST_3f, PE_CONST_4f, PE_CONST_5f, PE_CONST_6f, PE_CONST_7f, };\
        soc_reg_above_64_field_set(unit, OAMP_PE_FIRST_INSTr, reg_data, pe_const_fields[index], reg_field);\
    }\
} while (0)

#define WRITE_OAMP_FIRST_ISTRUCTION_REGISTER(regg_data)\
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)){ /* Jericho has already been written*/ \
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_FIRST_INSTr(unit, reg_data));\
    }

/**
 *  
 * "Driver" for the OAMP Programable Editor initializer.
 * 
 * param unit 
 * 
 * return uint32 - error number 
 */
uint32 arad_pp_oamp_pe_unsafe(SOC_SAND_IN int unit) {
    uint32
       res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data, reg_field;
    soc_reg_above_64_val_t reg_data_fem[2];
    uint32 fem_prog_fields[] = { FEM_PROG_0f, FEM_PROG_1f, FEM_PROG_2f, FEM_PROG_3f, FEM_PROG_4f,
    /** Jericho fields:*/
         FEM_PROG_5f, FEM_PROG_6f, FEM_PROG_7f, FEM_PROG_8f, FEM_PROG_9f,FEM_PROG_10f};
    uint32 fem_prog_reg[] = {OAMP_PE_FEM_CFG_1r, OAMP_PE_FEM_CFG_2r};
    uint32 reg32;
    int i, prog_used = 0;
    uint8 oam_pe_is_init = 0;

    
    /*
     * Lfem values, each a constant 96 bit field
     */
    CONST static soc_reg_above_64_val_t lfem_values[_ARAD_PP_OAMP_PE_NUM_DEFINED_LFEMS]= {
        /*LFEM 0*/ {0x20820820, 0x20408208, 0x1c61440c}, /* Used by server + end TLV program*/
        /*LFEM 1*/ {0x208203CE, 0x8208208,  0x82082082}, /*Used by the default + UDH program */
        /*LFEM 2*/ {0x5C6DA658, 0x08207DE7, 0x82082082}, /* Used by MAID 11B + end-TLV program, Up MEP MAC + end TLV program*/
        /*LFEM 3*/ {0x40820820, 0x61440C20, 0x2CA2481C},
        /*LFEM 4*/ {0x58820820, 0x081C6DA6, 0x82082082}, /* Used by server program (for ITMH Arad Style)*/
        /*LFEM 5*/ {0x20820820, 0x8208208, 0x82081962},  /* Used by server program (for ITMH[31:24] Jericho style)*/
        /*LFEM 6*/ {0x20820820, 0xB6A08208, 0x82082071}, /* Used by server program (for ITMH[7:0] Jericho style)*/
        /*LFEM 7*/ {0x95513491, 0x8206175, 0x82082082}, /* Used in server to shift MEPDB COUNTER by 1 bit right*/
    };

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sw_state_access[unit].dpp.soc.arad.pp.oamp_pe.oamp_pe_init.get(unit, &oam_pe_is_init);
    if(oam_pe_is_init){
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" PE has been initialized, error must happen.")));
    }
    _arad_pp_oamp_pe_programs_init(unit);

    /*First, initialize and allocate the programs we want, together with their respective lfems. and mep pe profiles. */
    res = _arad_pp_oamp_pe_programs_usage_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

    _arad_pp_oamp_pe_init_lfems(unit);
    _arad_pp_oamp_pe_mep_pe_init(unit);

    res = _arad_pp_oamp_pe_programs_alloc(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    /*Given the programs initialized above, we write to the tcam table and then
      the Programmable Editor program mem.*/
    res = _arad_pp_oamp_pe_write_to_tcam(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    res = _arad_pp_oamp_pe_write_to_pe_prog_mem(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    if (SOC_IS_JERICHO(unit)) {
        res = _jer_pp_oamp_pe_write_pe_profiles_info(unit);
    }
    else if (SOC_IS_ARADPLUS(unit)) {
        res = _arad_pp_oamp_pe_write_nibble_selection_profiles(unit);
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);



    /* Now that we have the program pointers (first instruction) for each program,
       We can fill the PE First Instr register as well as the pe const.*/
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        reg_field[0] =  ARAD_PP_OAMP_PE_MAX_INSTR_COUNT;
        soc_reg_above_64_field_set(unit, OAMP_PE_FIRST_INSTr, reg_data, MAX_INSTR_COUNTf, reg_field);
    } else {
        uint32 reg32 = 0;
        soc_field_t default_instr_field = SOC_IS_QAX(unit) ? DEFAULT_INST_ADDRf : PE_DEFAULT_INSTRUCTIONf;
        soc_reg_field_set(unit, OAMP_PE_INSTr, &reg32, MAX_INSTR_COUNTf, ARAD_PP_OAMP_PE_MAX_INSTR_COUNT);
        soc_reg_field_set(unit, OAMP_PE_INSTr,&reg32, default_instr_field, 0); /* instruction 0 should always be the default.*/
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_INSTr(unit,reg32));
    }


    /*Now each PE-PTR and PE-const*/
    for (i = 0; i < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS; ++i) {
        OAM_ACCESS.prog_used.get(unit, i, &sw_state_value);
        if (sw_state_value != -1) {
            SET_PE_FIRST_INSRTUCTION_PTR(prog_used, programs[unit][i].first_instr,reg_data);
            SET_PE_CONST_DATA(prog_used, programs[unit][i].pe_const, reg_data);
            ++prog_used;
        }
    }

    WRITE_OAMP_FIRST_ISTRUCTION_REGISTER(reg_data); 


    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "PE prog register set, now setting lfems\n")));
    /*Finally, set the LFEMS.*/
    if (SOC_IS_QAX(unit)) {
        for (i = 0; (i < ARAD_PP_OAMP_PE_NUM_LFEM(unit)) && (i < _ARAD_PP_OAMP_PE_NUM_DEFINED_LFEMS); ++i) {
            if (lfem_usage[unit][i]) {
                soc_reg_above_64_field_set(unit, OAMP_PE_FEM_MAPr, reg_data_fem[0], PE_FEM_MAPf, lfem_values[i]); /* Can't write constant directly to register :-( */
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_FEM_MAPr(unit, i, reg_data_fem[0]));
            }
        }
    }
    else {
        for (i = 0; (i < ARAD_PP_OAMP_PE_NUM_LFEM(unit)) && (i < _ARAD_PP_OAMP_PE_NUM_DEFINED_LFEMS); ++i) {
            if (lfem_usage[unit][i]) {
                /* indexes 0-5 use register OAMP_PE_FEM_CFG_1r, 6-10 use OAMP_PE_FEM_CFG_2r. In Arad only the latter*/
                soc_reg_above_64_field_set(unit, fem_prog_reg[i/6], reg_data_fem[i/6], fem_prog_fields[i], lfem_values[i]);
            }
        }

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_FEM_CFG_1r(unit, reg_data_fem[0]));
        if (SOC_IS_JERICHO(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_FEM_CFG_2r(unit, reg_data_fem[1]));
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "Done with the lfems.\n")));

    if (SOC_IS_ARADPLUS_A0(unit)) {
        /*
         * OAMP fix enable (affects TLV creation and maybe more).
         * This bit should be set for Arad+ only (not Jericho and above)
         */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_SPARE_REGISTER_3r(unit, &reg32));
        reg32 |= (1<<31);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, WRITE_OAMP_SPARE_REGISTER_3r(unit, reg32));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

/* Init the instruction field details by device type */
void _arad_pp_oamp_pe_instruction_field_details_init(int unit)
{
    sal_memset(instruction_field_size[unit], 0, sizeof(instruction_field_size[unit]));

    if (SOC_IS_QAX(unit)) {
        instruction_field_start[unit][ARAD_PP_OAMP_PE_IN_FIFO_RD_BITS] = 0;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FEM1_SEL_BITS] = 1;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FEM2_SEL_BITS] = 6;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MUX1_SRC_BITS] = 11;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MERGE1_INST_BITS] = 16;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_SHIFT1_SRC_BITS] = 18;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_SHIFT2_SRC_BITS] = 21;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FDBK_FF_WR_BIT] = 24;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUFF_WR_BITS] = 25;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_BITS] = 27;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_OP1_SEL_BITS] = 30;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_OP2_SEL_BITS] = 34;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_ALU_ACT_BITS] = 38;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_CMP1_ACT_BITS] = 40;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_ALU_DST_BITS] = 43;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUF_EOP_BITS] = 45;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUF_EOP_FRC_BIT] = 43;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_INST_CONST_BITS] = 47;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FDBK_FF_RD_BIT] = 58;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_OP3_SEL_BITS] = 59;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_CMP2_ACT_BITS] = 63;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_INST_SRC_BITS] = 66;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MUX2_SRC_BITS] = 68;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUFF2_SIZE_BITS] = 73;

        instruction_field_size[unit][ARAD_PP_OAMP_PE_IN_FIFO_RD_BITS] = 0 - 0 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FEM1_SEL_BITS] = 5 - 1 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FEM2_SEL_BITS] = 10 - 6 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MUX1_SRC_BITS] = 15 - 11 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MERGE1_INST_BITS] = 17 - 16 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_SHIFT1_SRC_BITS] = 20 - 18 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_SHIFT2_SRC_BITS] = 23 - 21 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FDBK_FF_WR_BIT] = 24 - 24 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUFF_WR_BITS] = 26 - 25 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_BITS] = 29 - 27 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_OP1_SEL_BITS] = 33 - 30 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_OP2_SEL_BITS] = 37 - 34 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_ALU_ACT_BITS] = 39 - 38 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_CMP1_ACT_BITS] = 42 - 40 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_ALU_DST_BITS] = 44 - 43 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUF_EOP_BITS] = 46 - 45 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_INST_CONST_BITS] = 57 - 47 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FDBK_FF_RD_BIT] = 58 - 58 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_OP3_SEL_BITS] = 62 - 59 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_CMP2_ACT_BITS] = 65 - 63 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_INST_SRC_BITS] = 67 - 66 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MUX2_SRC_BITS] = 72 - 68 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUFF2_SIZE_BITS] = 74 - 73 + 1;

    }
    else if (SOC_IS_JERICHO(unit)) {
        instruction_field_start[unit][ARAD_PP_OAMP_PE_IN_FIFO_RD_BITS] = 0;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FEM1_SEL_BITS] = 1;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FEM2_SEL_BITS] = 5;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MUX1_SRC_BITS] = 9;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MERGE1_INST_BITS] = 13;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_SHIFT1_SRC_BITS] = 15;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_SHIFT2_SRC_BITS] = 18;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FDBK_FF_WR_BIT] = 21;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUFF_WR_BITS] = 22;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_BITS] = 24;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_OP1_SEL_BITS] = 27;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_OP2_SEL_BITS] = 31;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_ALU_ACT_BITS] = 35;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_CMP1_ACT_BITS] = 37;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_ALU_DST_BITS] = 40;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUF_EOP_BITS] = 42;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUF_EOP_FRC_BIT] = 43;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_INST_CONST_BITS] = 44;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FDBK_FF_RD_BIT] = 54;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_OP3_SEL_BITS] = 55;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_CMP2_ACT_BITS] = 59;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_INST_SRC_BITS] = 62;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MUX2_SRC_BITS] = 64;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUFF2_SIZE_BITS] = 68;

        instruction_field_size[unit][ARAD_PP_OAMP_PE_IN_FIFO_RD_BITS] = 0 - 0 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FEM1_SEL_BITS] = 4 - 1 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FEM2_SEL_BITS] = 8 - 5 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MUX1_SRC_BITS] = 12 - 9 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MERGE1_INST_BITS] = 14 - 13 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_SHIFT1_SRC_BITS] = 17 - 15 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_SHIFT2_SRC_BITS] = 20 - 18 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FDBK_FF_WR_BIT] = 21 - 21 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUFF_WR_BITS] = 23 - 22 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_BITS] = 26 - 24 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_OP1_SEL_BITS] = 30 - 27 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_OP2_SEL_BITS] = 34 - 31 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_ALU_ACT_BITS] = 36 - 35 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_CMP1_ACT_BITS] = 39 - 37 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_ALU_DST_BITS] = 41 - 40 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUF_EOP_BITS] = 43 - 42 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_INST_CONST_BITS] = 53 - 44 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FDBK_FF_RD_BIT] = 54 - 54 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_OP3_SEL_BITS] = 58 - 55 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_CMP2_ACT_BITS] = 61 - 59 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_INST_SRC_BITS] = 63 - 62 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MUX2_SRC_BITS] = 67 - 64 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUFF2_SIZE_BITS] = 69 - 68 + 1;
    }
    else { /* Arad+ and below */
        instruction_field_start[unit][ARAD_PP_OAMP_PE_IN_FIFO_RD_BITS] = 0;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FEM1_SEL_BITS] = 1;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FEM2_SEL_BITS] = 5;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MUX1_SRC_BITS] = 9;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MERGE1_INST_BITS] = 13;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_SHIFT1_SRC_BITS] = 15;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_SHIFT2_SRC_BITS] = 18;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FDBK_FF_WR_BIT] = 21;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUFF_WR_BIT] = 22;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_BITS] = 23;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_OP1_SEL_BITS] = 26;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_OP2_SEL_BITS] = 30;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_ALU_ACT_BITS] = 34;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_CMP1_ACT_BITS] = 36;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_ALU_DST_BITS] = 39;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUF_EOP_BIT] = 41;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_BUF_EOP_FRC_BIT] = 42;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_INST_CONST_BITS] = 43;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_FDBK_FF_RD_BIT] = 51;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_OP3_SEL_BITS] = 52;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_CMP2_ACT_BITS] = 55;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_INST_SRC_BITS] = 58;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MUX2_SRC_BITS] = 60;
        instruction_field_start[unit][ARAD_PP_OAMP_PE_MERGE2_INST_BITS] = 64;

        instruction_field_size[unit][ARAD_PP_OAMP_PE_IN_FIFO_RD_BITS] = 0 - 0 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FEM1_SEL_BITS] = 4 - 1 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FEM2_SEL_BITS] = 8 - 5 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MUX1_SRC_BITS] = 12 - 9 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MERGE1_INST_BITS] = 14 - 13 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_SHIFT1_SRC_BITS] = 17 - 15 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_SHIFT2_SRC_BITS] = 20 - 18 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FDBK_FF_WR_BIT] = 21 - 21 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUFF_WR_BIT] = 22 - 22 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_BITS] = 25 - 23 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_OP1_SEL_BITS] = 29 - 26 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_OP2_SEL_BITS] = 33 - 30 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_ALU_ACT_BITS] = 35 - 34 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_CMP1_ACT_BITS] = 38 - 36 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_ALU_DST_BITS] = 40 - 39 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUF_EOP_BIT] = 41 - 41 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_BUF_EOP_FRC_BIT] = 42 - 42 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_INST_CONST_BITS] = 50 - 43 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_FDBK_FF_RD_BIT] = 51 - 51 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_OP3_SEL_BITS] = 54 - 52 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_CMP2_ACT_BITS] = 57 - 55 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_INST_SRC_BITS] = 59 - 58 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MUX2_SRC_BITS] = 63 - 60 + 1;
        instruction_field_size[unit][ARAD_PP_OAMP_PE_MERGE2_INST_BITS] = 65 - 64 + 1;

    }
}


void
  arad_pp_oamp_pe_program_profile_get(
    SOC_SAND_IN   int                                 unit,
    SOC_SAND_IN   ARAD_PP_OAMP_PE_PROGRAMS     program_id,
    SOC_SAND_OUT  uint32                                 *program_profile
  )
{
     OAM_ACCESS.mep_pe_profile.get(unit, program_id, &u32_sw_state_value);
     *program_profile = u32_sw_state_value;
}

STATIC void
  _arad_pp_oamp_pe_prog_id_to_prog_name(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 prog_id,
    SOC_SAND_OUT char** prog_name
)
{
    switch(prog_id)
    {
    case ARAD_PP_OAMP_PE_PROGS_DEFAULT:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_DEFAULT";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_DEFAULT_CCM:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_DEFAULT_CCM";
        break;
    case ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER";
        break;
    case ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER";
        break;
    case ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER";
        break;
    case ARAD_PP_OAMP_PE_PROGS_1DM:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_1DM";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_BFD_ECHO:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_BFD_ECHO";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP";
        break;    
    case ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC:
       *prog_name = "ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC";
        break;
    case ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC";
        break;
    case ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX";
        break;
    case ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER";
        break;
    case ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_UDH_PUNT_JER:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_UDH_PUNT_JER";
        break;
    case ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_CCM_JER:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_CCM_JER";
        break;
    case ARAD_PP_OAMP_PE_PROGS_MICRO_BFD:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_MICRO_BFD";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH";
        break;
    case ARAD_PP_OAMP_PE_PROGS_MAID_48:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_MAID_48";  
        break;
    default:
        *prog_name = "Unknown program"; 
        break;
    }
}



uint32
  arad_pp_oamp_pe_print_all_programs_data(int unit)
{
    int program_id;
    char* prog_name;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (program_id = 0; program_id < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS; program_id++) {
        OAM_ACCESS.instruction_func_is_valid.get(unit, program_id, &sw_state_value);
        if (sw_state_value) {
            _arad_pp_oamp_pe_prog_id_to_prog_name(unit, program_id, &prog_name);
            LOG_CLI((BSL_META_U(unit,
                                "program %d: %s\n"), program_id, prog_name));
            OAM_ACCESS.prog_used.get(unit, program_id, &sw_state_value);
            LOG_CLI((BSL_META_U(unit,
                                "selected program: %d\n"), sw_state_value));
        }
    }
    SOC_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}

uint32
  arad_pp_oamp_pe_print_last_program_data(int unit)
{    
    uint32 res = SOC_SAND_OK;
    int program_id;
    char* prog_name;
    uint32 num_oamp_pe = 1;
    uint32 oamp_pe_id;
    soc_reg_above_64_val_t reg_val;
    uint32 last_prog[2];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_JERICHO(unit)) {
        if (SOC_IS_QAX(unit)) {
            num_oamp_pe = 2;
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PE_STATUS_1r(unit, reg_val));
            last_prog[0] = soc_reg_above_64_field32_get(unit, OAMP_PE_STATUS_1r, reg_val, LAST_PROG_IDX_1f);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PE_STATUS_2r(unit, reg_val));
            last_prog[1] = soc_reg_above_64_field32_get(unit, OAMP_PE_STATUS_2r, reg_val, LAST_PROG_IDX_2f);
        }
        else {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PE_STATUSr(unit, reg_val));
            last_prog[0] = soc_reg_above_64_field32_get(unit, OAMP_PE_STATUSr, reg_val, LAST_PROG_IDX_1f);
        }

        LOG_CLI((BSL_META_U(unit,
                            "Last invoked OAMP PE program:\n")));  

        for (oamp_pe_id = 0; oamp_pe_id < num_oamp_pe; ++oamp_pe_id) {
            for (program_id = 0; program_id < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS; program_id++) {
            OAM_ACCESS.prog_used.get(unit, program_id, &sw_state_value);
            if (sw_state_value == last_prog[0]) {
                    _arad_pp_oamp_pe_prog_id_to_prog_name(unit, program_id, &prog_name);
                    if (num_oamp_pe > 1) {
                        LOG_CLI((BSL_META_U(unit,"OAMP-PE-%d:\n"),oamp_pe_id));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "  program %d: %s\n"), program_id, prog_name));
                OAM_ACCESS.prog_used.get(unit, program_id, &sw_state_value);
                    LOG_CLI((BSL_META_U(unit,
                                    "selected program: %d\n"), sw_state_value));
                    break;
                }
            }
        }
   } else {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("diag oam prge last=1 unsupported for given device.")));
   }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}



/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

