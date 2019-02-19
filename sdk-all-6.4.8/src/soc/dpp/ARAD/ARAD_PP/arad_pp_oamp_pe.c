/* $Id: arad_pp_oamp_pe.c,v 1.6 Broadcom SDK $
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

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/*Number of programs*/



/* max program number */
#define ARAD_PP_OAMP_PE_MAX_PROGRAM(unit)  ( SOC_IS_JERICHO(unit)?  32 : 8 )

/* max lfem number  */
#define ARAD_PP_OAMP_PE_MAX_LFEM  11

/* Actual number of lfems of LFEMS*/
#define ARAD_PP_OAMP_PE_NUM_LFEM(unit) ( SOC_IS_JERICHO(unit)?  ARAD_PP_OAMP_PE_MAX_LFEM : 5 )

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

#define ARAD_PP_OAMP_PE_OP_SEL_TMP_REG3		0X8
#define ARAD_PP_OAMP_PE_OP_SEL_TMP_REG4		0X9


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
#define SET_OAMP_PE_IN_FIFO_RD_BITS(reg,val)		SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,0,0)
#define SET_OAMP_PE_FEM1_SEL_BITS(reg,val)		SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,4,1)
#define SET_OAMP_PE_FEM2_SEL_BITS(reg,val)		SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,8,5)
#define SET_OAMP_PE_MUX1_SRC_BITS(reg,val)		SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,12,9)
#define SET_OAMP_PE_MERGE1_INST_BITS(reg,val)	SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,14,13)
#define SET_OAMP_PE_SHIFT1_SRC_BITS(reg,val)	SET_OAMP_PE_PROGRAM_BY_BITS(reg,val	,17,15)
#define SET_OAMP_PE_SHIFT2_SRC_BITS(reg,val)		SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,20,18 )
#define SET_OAMP_PE_FDBK_FF_WR_BIT(reg,val)		SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,21,21)
/* Arad only*/
#define SET_OAMP_PE_BUFF_WR_BIT(reg,val)	  SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,22,22)
/* Jericho only*/
#define SET_OAMP_PE_BUFF_WR_BITS(reg,val)  SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,23,22)
/* Arad only*/
#define SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg,val)	 SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,25,23)
/* Jericho only*/
#define SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg,val) SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,26,24)

#define SET_OAMP_PE_OP1_SEL_BITS(reg,val)		 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,30,27);\
                                                     } else {\
                                                         SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,29,26);\
                                                     } }while(0)

#define SET_OAMP_PE_OP2_SEL_BITS(reg,val)		 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,34,31);\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,33,30);\
                                                     } }while(0)

#define SET_OAMP_PE_ALU_ACT_BITS(reg,val)		 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,36,35);\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,35,34);\
                                                     } }while(0)

#define SET_OAMP_PE_CMP1_ACT_BITS(reg,val)		 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,39,37);\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,38,36);\
                                                     } }while(0)

#define SET_OAMP_PE_ALU_DST_BITS(reg,val)		 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,41,40);\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,40,39);\
                                                     } }while(0)

/*#define SET_OAMP_PE_NXT_INST_SEL_BITS(reg,val)	     do {if (SOC_IS_JERICHO(unit)) { \ 
                                                                            SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,39,38);  \
                                                     } else {\             
                                                      SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,39,38) ;\
                                                     } }while(0)        */
/* Arad only*/
#define SET_OAMP_PE_BUF_EOP_BIT(reg,val)		  SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,41,41)
/* Jericho only*/
#define SET_OAMP_PE_BUF_EOP_BITS(reg,val)   SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,43,42)                                                  
/* Arad only */
#define SET_OAMP_PE_BUF_EOP_FRC_BIT(reg,val)	SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,42,42)

#define SET_OAMP_PE_INST_CONST_BITS(reg,val)	 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,53,44);\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,50,43);\
                                                     } }while(0)

#define SET_OAMP_PE_FDBK_FF_RD_BIT(reg,val)	 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,54,54 );\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,51,51 );\
                                                     } }while(0)

#define SET_OAMP_PE_OP3_SEL_BITS(reg,val)	 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,58,55);\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,54,52);\
                                                     } }while(0)

#define SET_OAMP_PE_CMP2_ACT_BITS(reg,val)	 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,61,59 );\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,57,55 );\
                                                     } }while(0)

#define SET_OAMP_PE_INST_SRC_BITS(reg,val)	 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,63,62);\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,59,58);\
                                                     } }while(0)

#define SET_OAMP_PE_MUX2_SRC_BITS(reg,val)	 do {if (SOC_IS_JERICHO(unit)) { \
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,67,64);\
                                                     } else {\
                                                        SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,63,60);\
                                                     } }while(0)
/* Arad only */
#define SET_OAMP_PE_MERGE2_INST_BITS(reg,val)	 SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,65,64 )
/* Jericho only*/
#define SET_OAMP_PE_BUFF2_SIZE_BITS(reg,val)	 SET_OAMP_PE_PROGRAM_BY_BITS(reg,val,69,68)


/*The following two macros should not be used except by the above macros. */
#define SET_OAMP_PE_PROGRAM_BY_BITS(reg, val, end,start) \
   do {\
   uint32 __val = val; \
   SHR_BITCOPY_RANGE(reg,start,&__val,0,end-start +1);\
} while (0);

#define GET_OAMP_PE_PROGRAM_BY_BITS(reg, val, end,start) \
         SHR_BITCOPY_RANGE(&val, 0,reg, 0, end-start +1);



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
    if (programs[unit][program_index].mep_pe_profile==-1) {\
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("MEP PE profile uninitialized")));\
        }\
        SET_OAMP_PE_TCAM_MEP_PE_PROFILE(tcam_struct[tcam_index].key, programs[unit][program_index].mep_pe_profile) ; }while (0)

/** Msg type encoding: 0 - LMM/LMR (includes piggy back CCM in Arad+)
   1 -DMM/R
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
   uint8 changed;                           /* Is the profile data used */

   uint8 pe_var_8_nibble_select_0;          /* Select nibbles from mep db entry to be used by the PE as variable */
   uint8 pe_var_8_nibble_select_1;          /* Select nibbles from mep db entry to be used by the PE as variable */
   uint8 pe_var_16_nibble_select_0;         /* Select nibbles from mep db entry to be used by the PE as variable */
   uint8 pe_var_16_nibble_select_1;         /* Select nibbles from mep db entry to be used by the PE as variable */

   uint8 pe_mep_insert;                     /* add relevant MEP DB entry as prefix before the generated packet */

} JER_PP_OAMP_PE_PROF_INFO;

typedef struct
{
    int prog_used; /* prog ID in HW (program profile), -1 if not used */
    int first_instr; /*The term "first instruction" and "program ptr" are used interchangeably.*/
    uint32 pe_const;
    uint32 mep_pe_profile;

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


/* } */
/*************
 *  FUNCTIONS *
 *************/
/* { */

/* Function declerations*/
uint32 _arad_pp_oamp_pe_write_inst_default(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_pph_add_udh(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_default_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_oamp_server(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_oamp_server_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_one_dm(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_one_dm_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_echo(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_echo_add_udh(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_udp_checksum(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_up_mep_mac(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_11b_maid_end_tlv(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_maid_external(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_ipv4_single_hop(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_ipv4_single_hop_add_udh(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_oamts_down_mep(SOC_SAND_IN int unit);
uint32 _arad_pp_oamp_pe_write_inst_micro_bfd_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_bfd_pwe_bos_fix_jer(SOC_SAND_IN int);



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
        programs[unit][i].prog_used =
            programs[unit][i].mep_pe_profile =
            programs[unit][i].ls_nibble_select =
            programs[unit][i].ms_nibble_select =
            -1;
        programs[unit][i].instruction_func = NULL;
        programs[unit][i].mep_pe_prof_info.changed = 0;
    }

    for (i = 0; i < ARAD_PP_OAMP_PE_NUM_LFEM(unit); ++i) {
        lfem_usage[unit][i] = 0;
    }
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

    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    uint32 res;

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
        user_header_size = (user_header_0_size + user_header_1_size) / 8 ;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].instruction_func = _arad_pp_oamp_pe_write_inst_default;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].pe_const = 0;

        if (user_header_size) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH].instruction_func = _arad_pp_oamp_pe_write_inst_pph_add_udh;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH].pe_const = 0; 
        }

        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func = _arad_pp_oamp_pe_write_inst_one_dm;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].pe_const = 0;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].instruction_func = _arad_pp_oamp_pe_write_inst_oamp_server;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].pe_const = 0x000c0ccc; /* Note that server + tlv bug fix uses the same const.*/

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

        if(user_header_size != 0)
        {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP].instruction_func = _arad_pp_oamp_pe_write_inst_oamts_down_mep;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP].pe_const = 0;
        }


        programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].instruction_func = _arad_pp_oamp_pe_write_inst_up_mep_mac;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].pe_const = 0;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].ls_nibble_select = 23;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].ms_nibble_select = 27; /* Aim for the counter pointer which represents the LSB of the src mac address for Up MEPs. */

        programs[unit][ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX].instruction_func = NULL; /* Same program as UP_MEP_MAC. Two MEP-PE profiles needed, thus two programs.*/ 
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX].pe_const = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].pe_const;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX].ls_nibble_select = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].ls_nibble_select;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX].ms_nibble_select = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].ms_nibble_select;


        if (soc_property_get(unit, spn_OAM_MAID_11_BYTES_ENABLE, 0)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].instruction_func = _arad_pp_oamp_pe_write_inst_11b_maid_end_tlv;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].pe_const = 0;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].ls_nibble_select = 23;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].ms_nibble_select = 27; /* Aim for the counter pointer which represents the LSB of the src mac address for Up MEPs. */
        }

        if (soc_property_get(unit, spn_OAM_MAID_48_BYTES_EXTERNAL_ENABLE, 0)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_EXTERNAL].instruction_func = _arad_pp_oamp_pe_write_inst_maid_external;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_EXTERNAL].pe_const = 0x12345678;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_EXTERNAL].ls_nibble_select = 23;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_EXTERNAL].ms_nibble_select = 27; /* Aim for the counter pointer which represents the LSB of the src mac address for Up MEPs. */
        }


        /*Setting the default instruction.*/
        SOC_REG_ABOVE_64_CLEAR(reg_default_instr);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
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
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].instruction_func = _arad_pp_oamp_pe_write_inst_default_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].pe_const = 0;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func = _arad_pp_oamp_pe_write_inst_one_dm_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].pe_const = 0;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].instruction_func = _arad_pp_oamp_pe_write_inst_oamp_server_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].pe_const = 0x000c0ccc; 

        if (SOC_DPP_CONFIG(unit)->pp.micro_bfd_support==SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4 || 
            SOC_DPP_CONFIG(unit)->pp.micro_bfd_support==SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4_6 ) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MICRO_BFD].instruction_func = _arad_pp_oamp_pe_write_inst_micro_bfd_jer; 
            programs[unit][ARAD_PP_OAMP_PE_PROGS_MICRO_BFD].pe_const = 6784<<16;  /* On the two higher bytes set the micro BFD UDP dest port (6784)*/
        }

        /* PWE bos fix: always loaded*/
        programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX].instruction_func = _arad_pp_oamp_pe_write_inst_bfd_pwe_bos_fix_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX].pe_const = 0;

        SOC_REG_ABOVE_64_CLEAR(reg_default_instr);
        SOC_REG_ABOVE_64_CLEAR(reg_field); 
        SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
        SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
        SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
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
    programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].mep_pe_profile = 0; /* Ignored*/
    programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].mep_pe_profile = 1; 
    programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].mep_pe_profile = 3;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER].mep_pe_profile = 2;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].mep_pe_profile = 0x0;  /* Ignored*/
    programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].mep_pe_profile = 0x0;  /* ignored anyways*/
    programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].mep_pe_profile = 0x6;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP].mep_pe_profile = 0x9; /*must differ from DOWN_MEP_TLV_FIX with only the LSB */
    programs[unit][ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX].mep_pe_profile = 0x8;
	programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP].mep_pe_profile = 0x0;  /* Must be 0. Used for nibble select.*/
    programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].mep_pe_profile = 0x7;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH].mep_pe_profile = 0x0;/* Ignored*/
    programs[unit][ARAD_PP_OAMP_PE_PROGS_MICRO_BFD].mep_pe_profile = 0x4;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX].mep_pe_profile = 0x5;
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
    programs[unit][prog].prog_used = ++last_program_id[unit];


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
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /*First, filling the tcam entries, one for each program.*/

    /*Default*/
    prog_tcam_entries[tcam_ind].key    = 0;
    prog_tcam_entries[tcam_ind].mask   = _OAMP_PE_TCAM_MASK_DONT_CARE; /* No sense using the macros. Everyhing is "Don't care" */
    prog_tcam_entries[tcam_ind].dat    = 0; /*This is hard coded because 0 should always be the default program.*/
    prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].instruction_func != NULL;
    ++tcam_ind;

    /*OAMP server*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 7);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, SOC_PPC_OAM_MEP_TYPE_ETH_OAM);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind,ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 3);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,3 );
    prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].prog_used;
    prog_tcam_entries[tcam_ind].valid  =programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].instruction_func != NULL;
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
    prog_tcam_entries[tcam_ind].dat          = programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].prog_used;
    prog_tcam_entries[tcam_ind].valid        =programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func != NULL;
    ++tcam_ind;


    /*Eth tlv on server*/
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind,ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 6);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key,2 );
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER].prog_used;
    prog_tcam_entries[tcam_ind].valid  =programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER].instruction_func != NULL;
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
    prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP].prog_used;
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
    prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].prog_used;
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
    prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].prog_used;
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
    prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].prog_used;
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
    prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP].prog_used;
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
     prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].prog_used;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].instruction_func != NULL;
     LOG_VERBOSE(BSL_LS_SOC_OAM,
                 (BSL_META_U(unit,
                             "up mep mac: Inserting into tcam entry with dat =%d, key=%x\n"), prog_tcam_entries[tcam_ind].dat, prog_tcam_entries[tcam_ind].key));
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
     prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC].prog_used; /* Uses the same program as the UP_MEP_MAC program. */
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
    prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].prog_used;
    prog_tcam_entries[tcam_ind].valid  =programs[unit][ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV].instruction_func != NULL;
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
     prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH].prog_used; 
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH].instruction_func != NULL;
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
     prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_MICRO_BFD].prog_used;
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
     prog_tcam_entries[tcam_ind].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX].prog_used;
     prog_tcam_entries[tcam_ind].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX].instruction_func != NULL; 
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

    /*  #5 if (tmp_reg1 == 8'h3), jump to tmp_reg2 (`a) - (OAM-TS present) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

    /*  #11 put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


    /*  #16 copy tmp_reg3 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

    /*  #18 copy 2 bytes to buffer (21,22) */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #19 put zeros for UDH1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
/*  NOP */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the first byte of the first packet word, removing the two bytes bytes */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write one byte of TMP_REG_4 to buffer */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the last two bytes of the first packet word, removing the two bytes bytes */
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  write OPCODE */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  write OPCODE */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  NOP */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the first byte of the first packet word, removing the two bytes bytes */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  write one byte of TMP_REG_4 to buffer */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the last two bytes of the first packet word, removing the two bytes bytes */
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #2 copy bytes 37,38 to buffer */
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

/*  #3 write the two byte of destination port from prog. variable */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


/*  #5 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

    /*  #3 Copy 2 bytes - system headers ITMH-EXT (last 1 of 3), PPH (1 bytes of 7),
     */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #4 Copy byte from TmpReg3 to buffer (should be the new PPH.Offset)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

    /*  #7 push first part of zero bytes of UDH
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #10copy bytes 37,38 to buffer
     */
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

    /*  #11 write the two byte of destination port from prog. variable
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


    /*  #13 put 16'h0 to buffer (UDP checksum invalid)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 32);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #2 copy bytes 41,42 to buffer */
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


/*  #3 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #3 if TmpReg1 is between 0x0 - 0xd, jump to value in TmpReg2(MULTIHOP_LABEL)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

    /*  #7 write the two byte of destination port from prog. variable */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


    /*  #9 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 56);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #12 put 32'h0 to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


    /*  #15 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/*  #4 Copy 2 bytes - system headers ITMH-EXT (last 1 of 3), PPH (1 bytes of 7), */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /* #5 Copy byte from TmpReg3 to buffer (should be the new PPH.Offset)
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

    /*  #8 push first part of zero bytes of UDH
     */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 28);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                  WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));

    /*  #11 if TmpReg1 is between 0x0 - 0xd, jump to value in TmpReg2(MULTIHOP_LABEL)  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

    /*  #15 write the two byte of destination port from prog. variable */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


    /*  #17 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 56);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
            WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  #20 put 32'h0 to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


    /*  #23 put 16'h0 to buffer (UDP checksum invalid) */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    soc_reg_above_64_val_t reg_data, reg_field;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
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
uint32 _arad_pp_oamp_pe_write_inst_micro_bfd_jer(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MICRO_BFD);

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

program pseudo-code:
    1. skip first 16(sys-header_length) + 20 (IP) + 2 (UDP src port) =  38 bytes
    2. stamp 6784 in the following 2 bytes
    3. copy the reset of the packet.
 
 #1. copy to buffer bytes 1-36
 #2. copy two buffer bytes 37,38, read the data fifo
 #3. write to buffer first two bytes of the prog. constant
 #4. copy the rest of the packet.
 */

/* #1. copy to buffer bytes 1-36 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/* #2. copy two buffer bytes 37,38, read the data fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/* #3. write to buffer first two bytes of the prog. constant */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/* #4. copy the rest of the packet. */
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
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX);
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
      a. Copy to buffer PTCH(2) + ITMH(4) + ITMH_EXT(3) + PPH(7) + PWE_LABEL_MSB(2) = 18 bytes
      b. Copy to buffer byte 19 | 8'h1
      c. Copy the rest of the packet.
  
      c. Copy the rest of the packet.
  
  
  */
/*  #0. Prepare 8'h1 in TMP_VAR1*/
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


/*  #1. copy to buffer bytes 1-16 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/* #2. copy to buffer two bytes 17,18. Store in tmp_reg2 bytes 19,20  read the data fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
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

/*  #3 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #4 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #5 TMP_REG2(bytes 20,19) - 1 -> TMP_REG2*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/*  #6 NOP*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #7 NOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #8 write TMP_REG2 to buffer*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG2);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #9. copy the rest of the packet.*/
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
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER);



    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR0);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0f);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  Write lower two bytes of prog. variable to buffer (PTCH_2 outer), */
/*  copy value of PKT REG0 to TMP_REG4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_WR_ALWAYS);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_PKT_VAR0);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  NOP */
/*  NOP, and set TMP_REG1 to 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field,ARAD_PP_OAMP_PE_OP_SEL_INST_VAR );
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X4);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
/* inst_mem_entry[`OAMP_PE_MUX2_SRC_BITS] 		= `OAMP_PE_MUX_SRC_OFFST_REG0; */
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_OR);
    SET_OAMP_PE_SHIFT2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X22);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  NOP */
/*  Write two bytes from pkt var 1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PKT_VAR1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the first byte of the first packet word, removing the two bytes bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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



/*  write one byte of TMP_REG_4 to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X11);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* write the last two bytes of the first packet word, removing the two bytes bytes */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BITS(reg_field, 1);
    SET_OAMP_PE_BUFF1_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X20);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  copy the rest of the packet */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_default_instr));

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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #8	copy byte 13 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
        SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
        soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                      WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL,last_program_pointer[unit]++ , reg_data));


    /*  #20 push first part of zero bytes of UDH */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/*  #32=#c:put 24 to tmp_reg1 */
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


/*  #36	copy 2 bytes (CCM PDU 73, 74) */
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


/*  #45 copy three higher bytes */
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

/*  #50 copy three higher bytes */
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

/*  #51	#d:put 8'h0 to buffer, set EOP */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #8	copy byte 13 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #25	copy 4 bytes to buffer (bytes 5-8 of CCM PDU), put 2 to tmp_reg4 */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #26 copy 2 bytes to buffer (bytes 9-10 of CCM PDU) */
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

/*  #27	write tmp_reg2[7:0] to buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #29	copy 3 bytes from in_fifo, (bytes 17-19) */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #30	write tmp_reg4[7:0] to buffer */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #31	copy 1 byte from in_fifo, shifted by three, read in_fifo (bytes 20) */
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


/*  #32	copy 4 bytes to buffer (bytes 21-24 of CCM PDU) */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #33 copy 48 bytes */
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


/*  #34	copy 2 bytes (CCM PDU 73, 74) */
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


/*  #43 copy three higher bytes */
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

/*  #48 copy three higher bytes */
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

/*  #49	#d:put 8'h0 to buffer, set EOP */
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


/**
 * 
 * 
 * @author sinai (24/11/2014)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 _arad_pp_oamp_pe_write_inst_maid_external(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_MAID_EXTERNAL);

    /*
   a.	Fix TLV
   b.	Fix SRC MAC LSB
   c.	Add a PTCH, ITMH to the packet. ITMH destination should be taken from the PE!9var.

  
   The two first (a and b) are taken from UP_MEP_MAC with some modification.
   For (c) the program will do:
   Build the PTCH 
       - The PTCH 16 bits are taken from the 16 MSB of the program-variable.
   Build the ITMH. 
       - 8 bits from the instruction
       - 8 bits from the instruction
       - ITMH.Destination.Dest_system_port should be taken from Program-variable LSB (16 bits)
     
       Instructions:
     
    take 2 MSBs from program const, set on buffer (PTHC) 
    put ITMH, byte 1 in tmp_reg1 
    put ITMH, byte 2 in tmp_reg2 
    put 0 in tmp_reg3 
    put tmp_reg1 on buffer 
    put tmp_reg2 on buffer 
    put ITMH, destination bits on buffer 
     
    copy in_fifo[15:0] & 'h8 to to tmp_reg1   in_fifo[7] holds the up/down bit  
    put (pk_var2 & 'h6) to tmp_reg2
    put #c to tmp_reg3
    if down jump to #a
    nop
    nop
    copy 12 bytes, rd in_fifo
    copy byte 13, 
    put PktVar[2][15:8] to buffer
    copy bytes 15,16, rd in_fifo
    jump to #b
    nop
    nop
    #a: copy 20 bytes
    #b: if tmp_reg2 == 0, jump to tmp_reg3 ( tmp_reg2 holds the encapsulation #c  )
    nop 
    nop
    copy 4 bytes  VLAN TAG1 
    if (tmp_reg2 - 2) == 1, jump to tmp_reg3   encapsulation == 1, jump to #c  
    nop
    nop
    copy 4 bytes  VLAN TAG2  
    #c:put 64 to tmp_reg1
    nop
    nop
    tmp_reg1 <= tmp_reg - 4, copy in_fifo to buffer, loop till alu_out != 0
    copy 2 bytes (CCM PDU 73, 74)
    TmpReg1 <= LENGTH - BYTES_RD   can be 1,5,9  
    nop
    nop
    if (TmpReg1 - 1) == 0, jump to tmp_reg3   NO TLV  
    put #d to tmp_reg3
    nop
    nop
    copy two lower bytes
    copy two higher bytes
    if TmpReg1- 5 == 0, jump to tmp_reg3
    nop
    nop
    copy two lower bytes
    copy two higher bytes
    #d:put 8'h0 to buffer, set EOP
    

 */

    /* #0: Take PTCH from two higher bytes on program memory an put it on the buffer*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /*  16 MSB ITMH: PPH_type=2'b0, INN_MIRR_DIS=1'b0, SNOOP CMD=4'b0, FWD_TC=3'b111, FWD_DP=2'b11,  destination-encoding=4'b1100  */
/*  #1	copy instruction const value (8 bits) to tmp_reg1  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xab);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    /* #2: Set second byte of ITMH on tmp-reg-2*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD); /*  ALU output is instruction const  */
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2); /*  Store data const in reg#2  */
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xcd); 
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data)); 



    /* #3: NOP*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

    /* #4:  Set tmp_reg_1 to buffer. Represents first byte of ITMH.*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
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


    /* #5: Set tmp_reg_2 to buffer. Represents second byte of ITMH.*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG2);
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

    /* #6: ITMH, destination (16 bits): Taken from lower bytes of instruction const, set on buffer.*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  From here it is the SRC_MAC_TLV program   */
/*  ----------------------------------------------------  */

/*  #7	copy in_fifo[15:0] to to tmp_reg1 ** in_fifo[7] holds the up/down bit  */
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

/*  #8	put #a to tmp_reg4  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 21); /*  should be #a  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #9	put (pk_var2 & 'h6) to tmp_reg2  */
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

/*  #10 put #c to tmp_reg3  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 30); /*  should be #c  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #11	if tmp_reg1 & 'h80 == 0 jump to tmp_reg4 ** if down jump to #a  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
/* inst_mem_entry [`OAMP_PE_FEM2_SEL_BITS]         = 4'h1;  */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X80);  /*  The value for testing  of 0X0?  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #12	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
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

/*  #14	copy 12 bytes, rd in_fifo  */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #15	copy byte 13  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/*  #16 put PktVar[2][15:8] to buffer  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


/*  #17 copy bytes 15,16, rd in_fifo  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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


    /*  #18 jump to #b  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 21);  /*  Should be #b  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #19 nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #20	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #21 = #a: copy 20 bytes  */
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


/*  #22=#b: if tmp_reg2 == 0, jump to tmp_reg3 ** tmp_reg2 holds the encapsulation, tmp_reg3 is #c  */
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


/*  #23	nop   */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #24	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  #25 copy 4 bytes ** VLAN TAG1  */
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


/*  #26	if (tmp_reg2 - 2) == 0, jump to tmp_reg3 ** encapsulation == 1, jump to #c  */
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


/*  #27	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #28	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #29	copy 4 bytes **VLAN TAG2  */
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

/*  #30=#c:put 24 to tmp_reg1  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 24); /*  should be #c  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #31	copy 4 bytes  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/*  #32	copy 4 bytes  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 0);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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

/*  #33 copy 64 bytes  */
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


/*  #34	copy 2 bytes (CCM PDU 73, 74)  */
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
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #35 TmpReg1 <= LENGTH - BYTES_RD ** can be 1,5,9  */
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


/*  #36 put #d to tmp_reg3   */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, fst_inst_addr + 48); /*  should be #d  */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #37	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #38	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #39	if (TmpReg1 + 1) == 0, jump to tmp_reg3 ** NO TLV  */
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

/*  #40 nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #41 nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  #42 copy one lower bytes, rd in_fifo  */
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


/*  #43 copy three higher bytes  */
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


/*  #44	if TmpReg1- 3 == 0, jump to tmp_reg3 ** one TLV  */
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


/*  #45	nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #46 nop  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #47 copy one lower byte  */
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

/*  #48 copy three higher bytes  */
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

/*  #49	#d:put 8'h0 to buffer, set EOP  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
/* inst_mem_entry [`OAMP_PE_MUX1_SRC_BITS]	      = `OAMP_PE_MUX_SRC_OFFST_REG2;  */
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
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
        if (programs[unit][prog].instruction_func
            && programs[unit][prog].mep_pe_prof_info.changed) {
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

    /*
     * Lfem values, each a constant 96 bit field
     */
    CONST static soc_reg_above_64_val_t lfem_values[ARAD_PP_OAMP_PE_MAX_LFEM]= {
        /*LFEM 0*/ {0x20820820, 0x20408208, 0x1c61440c}, /* Used by server + end TLV program*/
        /*LFEM 1*/ {0x208203CE, 0x8208208, 0x82082082 }, /*Used by the default + UDH program */
        /*LFEM 2*/ {0x5C6DA658, 0x08207DE7, 0x82082082}, /* Used by MAID external program, MAID 11B + end-TLV program, Up MEP MAC + end TLV program*/
        /*LFEM 3*/ {0x40820820, 0x61440C20, 0x2CA2481C},
        /*LFEM 4*/ {0x58820820, 0x081C6DA6, 0x82082082}, /* Used by server program (including Jericho)*/
        /** Jericho LFEMS (all zero) */
        /*LFEM 5*/ {0, 0, 0},
        /*LFEM 6*/ {0, 0, 0},
        /*LFEM 7*/ {0, 0, 0},
        /*LFEM 8*/ {0, 0, 0},
        /*LFEM 9*/ {0, 0, 0},
        /*LFEM 10*/ {0, 0, 0}

    };

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    _arad_pp_oamp_pe_programs_init(unit);

    /*First, initialize and allocate the programs we want, together with their respective lfems. and mep pe profiles. */
    res = _arad_pp_oamp_pe_programs_usage_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

    _arad_pp_oamp_pe_init_lfems(unit);
    _arad_pp_oamp_pe_mep_pe_init(unit);

    res = _arad_pp_oamp_pe_programs_alloc(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    if (SOC_IS_JERICHO(unit)) {
        res = _jer_pp_oamp_pe_write_pe_profiles_info(unit);
    }
    else if (SOC_IS_ARADPLUS(unit)) {
        res = _arad_pp_oamp_pe_write_nibble_selection_profiles(unit);
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);



    /*Given the programs initialized above, we write to the tcam table and then 
      the Programmable Editor program mem.*/
    res = _arad_pp_oamp_pe_write_to_tcam(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    res = _arad_pp_oamp_pe_write_to_pe_prog_mem(unit);
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
        soc_reg_field_set(unit, OAMP_PE_INSTr, &reg32, MAX_INSTR_COUNTf, ARAD_PP_OAMP_PE_MAX_INSTR_COUNT);
        soc_reg_field_set(unit, OAMP_PE_INSTr,&reg32, PE_DEFAULT_INSTRUCTIONf, 0); /* instruction 0 should always be the default.*/
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_INSTr(unit,reg32));
    }


    /*Now each PE-PTR and PE-const*/
    for (i = 0; i < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS; ++i) {
        if (programs[unit][i].prog_used != -1) {
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
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    for (i = 0; i < ARAD_PP_OAMP_PE_NUM_LFEM(unit); ++i) {
        if (lfem_usage[unit][i]) {
            SOC_REG_ABOVE_64_CLEAR(reg_field); 
            /* indexes 0-5 use register OAMP_PE_FEM_CFG_1r, 6-10 use OAMP_PE_FEM_CFG_2r. In Arad only the latter*/
            soc_reg_above_64_field_set(unit, fem_prog_reg[i/6], reg_data_fem[i/6], fem_prog_fields[i], lfem_values[i]);
        }
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_FEM_CFG_1r(unit, reg_data_fem[0]));
    if (SOC_IS_JERICHO(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_OAMP_PE_FEM_CFG_2r(unit, reg_data_fem[1])); 
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




void
  arad_pp_oamp_pe_program_profile_get(
    SOC_SAND_IN   int                                 unit,
    SOC_SAND_IN   ARAD_PP_OAMP_PE_PROGRAMS     program_id,
    SOC_SAND_OUT  uint32                                 *program_profile
  )
{
    *program_profile = programs[unit][program_id].mep_pe_profile;
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
    case ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER";  
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
	case ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX:
	    *prog_name = "ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX";  
	    break;
    case ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_MAID_EXTERNAL:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_MAID_EXTERNAL";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_MICRO_BFD:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_MICRO_BFD";  
        break;
    case ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX:
        *prog_name = "ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX";  
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
        if (programs[unit][program_id].instruction_func) {
            _arad_pp_oamp_pe_prog_id_to_prog_name(unit, program_id, &prog_name);
            LOG_CLI((BSL_META_U(unit,
                                "program %d: %s\n"), program_id, prog_name));
            LOG_CLI((BSL_META_U(unit,
                                "selected program: %d\n"), programs[unit][program_id].prog_used));
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
    soc_reg_above_64_val_t reg_val, last_prog;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_JERICHO(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PE_STATUSr(unit, reg_val));
        soc_reg_above_64_field_get(unit, OAMP_PE_STATUSr, reg_val, LAST_PROG_IDX_1f, last_prog);
        LOG_CLI((BSL_META_U(unit,
                            "Last invoked OAMP PE program:\n")));  

        for (program_id = 0; program_id < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS; program_id++) {
            if (programs[unit][program_id].prog_used == last_prog[0]) {
                _arad_pp_oamp_pe_prog_id_to_prog_name(unit, program_id, &prog_name);
                LOG_CLI((BSL_META_U(unit,
                                    "program %d: %s\n"), program_id, prog_name));
                LOG_CLI((BSL_META_U(unit,
                                    "selected program: %d\n"), programs[unit][program_id].prog_used));
                break;
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

