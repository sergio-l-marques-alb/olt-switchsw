/* $Id: arad_pp_oamp_pe.c,v 1.6 Broadcom SDK $
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

#define ARAD_PP_OAMP_PE_MERGE_INST_FEM		0x1
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

/* Set values in the tcam entries, according to fields defined in
   depot2/Arad_plus/A0/docs/ARCH-specs/App-Notes/OAM/OAMP-pe-programs.docx.
   Macros shared by tcam keys and tcam masks.*/
#define SET_OAMP_PE_TCAM_PACKET_TYPE(key_or_mask, val) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 0,2)
#define SET_OAMP_PE_TCAM_MEP_TYPE(key_or_mask, val) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,6)
#define SET_OAMP_PE_TCAM_MEP_PROFILE(key_or_mask, val) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 7,10)
#define SET_OAMP_PE_TCAM_MEP_PE_PROFILE(key_or_mask, val) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 11,14)
/* Different parameters for following macro: Accepts the tcam struct and the program key from
   ARAD_PP_OAMP_PE_PROGRAMS*/
#define SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(tcam_struct,tcam_index,program_index)   do { \
    if (programs[unit][program_index].mep_pe_profile==-1) {\
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("MEP PE profile uninitialized")));\
        }\
        SET_OAMP_PE_TCAM_MEP_PE_PROFILE(tcam_struct[tcam_index].key, programs[unit][program_index].mep_pe_profile) ; }while (0)
#define SET_OAMP_PE_TCAM_MSG_TYPE(key_or_mask, val) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 15,16)

/***************************************************/
/* *************************************************/
/* Macros for setting the Jericho PE TCAM. TCAM entry depends on packet type*/

/**
 * packet source type: 
 * 0 - MEP generated 
 * 1 - MEP response 
 * 2 - Punt packet 
 * 3 - Event packet 
 * 4 - SAT machine packet 
 * 5 - packet DMA 
 * 6-7 reserved 
 *  
 * One of ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_* defines 
 */
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_JER(key_or_mask, val) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 0,2)

/* If Packet source is 0/1 (mep generated)*/
/** MEP type Taken from MEP DB entry*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_TYPE_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,6)
/** From MEP entry, MepMessageProfile or MepDiagProfile, dependent on MEP-Type*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PROFILE_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 7,13)
/** From MEP entry. If MEP-Type is BFDoIPv4-MHOP, this field
 *  will be the Diag-Profile */
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PE_PROFILE_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 14,19)
/* Different parameters for following macro: Accepts the tcam struct and the program key from
   ARAD_PP_OAMP_PE_PROGRAMS*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PE_PROFILE_KEY_JER(tcam_struct, program_index )  do { \
    if (programs[unit][program_index].mep_pe_profile==-1) {\
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("MEP PE profile uninitialized")));\
        }\
            SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PE_PROFILE_JER(tcam_struct[program_index].key, programs[unit][program_index].mep_pe_profile) ; }while (0)

/** One of  ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_* defines   */
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_MSG_TYPE_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 20,23)

/* If packet source is 2 (Punt)*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_PUNT_RESERVED_JER(key_or_mask, val )  \
            do {SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,6); SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 9,13); SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 22,23);}while(0)
/** punt profile from the RMEP DB */
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_PUNT_PUNT_PROFILE_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 7,8) 
/** As set by receive processor according to the trap reason configuration */ 
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_PUNT_TRAP_CODE_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 14,21)

/*If packet type is 3 (event)*/
/** CPU configuration register*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_EVENT_CFG_EVENT_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,23)

/*If packet type is 4 (SAT)*/
/** CPU configuration register*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_SAT_CFG1_MEP_PACKET_PE_SEL_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,19)
/** 0-15, indicating which packet generator packet came from. */
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_SAT_GENERATOR_ID_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 20,23)

/*If packet type is 3 (packet DMA)*/
/** The first 21b of the packet should be CPU inserted program selection key. The first 32b of the packet which consists the key should be dropped.*/
#define SET_OAMP_PE_TCAM_PACKET_SOURCE_DMA_PACKET_DATA_JER(key_or_mask, val ) SET_OMAP_TCAM_BY_BITS(key_or_mask, val, 3,23)


/* TCAM and program functions are implemented differently on each chip. */
#define _OAM_OAMP_PE_FUNCTION_CALL(func, params) \
    (SOC_IS_ARADPLUS_AND_BELOW(unit))?  func##_arad params : func##_jericho params


/**/

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */


typedef struct
{
    int prog_used; /* prog ID in HW (program profile), -1 if not used */
    int first_instr; /*The term "first instruction" and "program ptr" are used interchangeably.*/
    uint32 pe_const;
    uint32 mep_pe_profile;
    uint32 ls_nibble_select; /* If the program requires a nibble select, this value should be the LS. otherwise, -1 */
    uint32 ms_nibble_select; /* If the program requires a nibble select, this value should be the MS. otherwise, -1 */
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
uint32 _arad_pp_oamp_pe_write_inst_default_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_oamp_server(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_oamp_server_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_one_dm(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_one_dm_jer(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_write_inst_eth_tlv(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_echo(SOC_SAND_IN int);
uint32 _arad_pp_oamp_pe_bfd_udp_checksum(SOC_SAND_IN int);

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
STATIC void _arad_pp_oamp_pe_programs_usage_init(SOC_SAND_IN  int  unit) {
    soc_reg_above_64_val_t reg_field;


    /*In arad+, Load all except the programs push, pop, push_and_pop which are not implemented at this stage.*/
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* Set the instruction function for all the active programs:*/
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].instruction_func = _arad_pp_oamp_pe_write_inst_default;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].pe_const = 0;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func = _arad_pp_oamp_pe_write_inst_one_dm;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].pe_const = 0;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV].instruction_func = _arad_pp_oamp_pe_write_inst_eth_tlv;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV].pe_const = 0;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV].ls_nibble_select = 14;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV].ms_nibble_select = 23;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].instruction_func = _arad_pp_oamp_pe_write_inst_oamp_server;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].pe_const = 0x000c0ccc; /* Note that server + tlv bug fix uses the same const.*/


        if (soc_property_get(unit, spn_BFD_ECHO_ENABLED, 0)) {
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].instruction_func = _arad_pp_oamp_pe_bfd_echo;
            programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].pe_const = 0x0ec90000; /* UDP source port, stamped on the packet.*/
        }

        programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].instruction_func = _arad_pp_oamp_pe_bfd_udp_checksum;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].pe_const = 3785; /* UDP source port, stamped on the packet.*/


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
        /* Jericho: load only existing program (1dm and server).*/
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].instruction_func = _arad_pp_oamp_pe_write_inst_default_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].pe_const = 0;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func = _arad_pp_oamp_pe_write_inst_one_dm_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].pe_const = 0;

        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].instruction_func = _arad_pp_oamp_pe_write_inst_oamp_server_jer;
        programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].pe_const = 0x000c0ccc; 

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
    programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].mep_pe_profile = 0;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].mep_pe_profile = 1;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].mep_pe_profile = 2;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV].mep_pe_profile = 8;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER].mep_pe_profile = 9;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_6374_LM].mep_pe_profile = 4;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_6374_DM].mep_pe_profile = 4;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_PUSH_RA].mep_pe_profile = 8;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_POP_CW].mep_pe_profile = 4;
    programs[unit][ARAD_PP_OAMP_PE_PROGS_PUSH_RA_AND_POP_CW].mep_pe_profile = 0xc; 
    programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_ECHO].mep_pe_profile = 0xf;  /* Using an invalid diag profile (and value). */
    programs[unit][ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM].mep_pe_profile = 0x0;  /* ignored anyways*/
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
    if (last_program_id[unit] > ARAD_PP_OAMP_PE_MAX_PROGRAM(unit)) {
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
STATIC uint32 _arad_pp_oamp_pe_write_to_tcam_arad(SOC_SAND_IN int unit) {
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
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, 0);
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
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_1DM);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 9);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 1);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    prog_tcam_entries[tcam_ind].dat          = programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].prog_used;
    prog_tcam_entries[tcam_ind].valid        =programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func != NULL;
    ++tcam_ind;


    /*ETH TLV */
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 ); 
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_ETH_TLV);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 2);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    prog_tcam_entries[tcam_ind].dat  = programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV].prog_used;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_ETH_TLV].instruction_func != NULL;
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

    /* 6374 LM */
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key,2 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_6374_LM );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask,3 );
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    prog_tcam_entries[tcam_ind].dat  = programs[unit][ARAD_PP_OAMP_PE_PROGS_6374_LM].prog_used;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_6374_LM].instruction_func != NULL;
    ++tcam_ind;

    /* 6374 DM */
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key,2 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key, 0xf);
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_6374_DM);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask,3 );
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 1);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    prog_tcam_entries[tcam_ind].dat  = programs[unit][ARAD_PP_OAMP_PE_PROGS_6374_DM].prog_used;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_6374_DM].instruction_func != NULL;
    ++tcam_ind;


    /*The following 3 programs are not implemented at the time of this writing */
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key,6 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask,0xf );
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_PUSH_RA);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key, 0);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 3);
    prog_tcam_entries[tcam_ind].dat  = programs[unit][ARAD_PP_OAMP_PE_PROGS_PUSH_RA].prog_used;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_PUSH_RA].instruction_func != NULL;
    ++tcam_ind;

    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, 6);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries, tcam_ind,ARAD_PP_OAMP_PE_PROGS_POP_CW);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask,3 );
    prog_tcam_entries[tcam_ind].dat  = programs[unit][ARAD_PP_OAMP_PE_PROGS_POP_CW].prog_used;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_POP_CW].instruction_func != NULL;
    ++tcam_ind;

    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].key, 0);
    SET_OAMP_PE_TCAM_PACKET_TYPE(prog_tcam_entries[tcam_ind].mask, 0);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].key, 6);
    SET_OAMP_PE_TCAM_MEP_TYPE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].key,0xf );
    SET_OAMP_PE_TCAM_MEP_PROFILE(prog_tcam_entries[tcam_ind].mask, 0xf);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE_KEY(prog_tcam_entries,tcam_ind, ARAD_PP_OAMP_PE_PROGS_PUSH_RA_AND_POP_CW);
    SET_OAMP_PE_TCAM_MEP_PE_PROFILE(prog_tcam_entries[tcam_ind].mask,0 );
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].key,0 );
    SET_OAMP_PE_TCAM_MSG_TYPE(prog_tcam_entries[tcam_ind].mask, 3);
    prog_tcam_entries[tcam_ind].dat  = programs[unit][ARAD_PP_OAMP_PE_PROGS_PUSH_RA_AND_POP_CW].prog_used;
    prog_tcam_entries[tcam_ind].valid = programs[unit][ARAD_PP_OAMP_PE_PROGS_PUSH_RA_AND_POP_CW].instruction_func != NULL;
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

    /* Sort the array so that the least inclusive masks appear first.*/
    soc_sand_os_qsort(prog_tcam_entries, tcam_ind, sizeof(ARAD_PP_TCAM_ENTRY), _arad_pp_tcam_cmp);

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

STATIC uint32 _arad_pp_oamp_pe_write_to_tcam_jericho(SOC_SAND_IN uint32 unit) {
    uint32 res;
    ARAD_PP_TCAM_ENTRY prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_NOF_PROGS] = { { 0 } };
    int i, tcam_ind=0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Same structure as the Arad version*/

    /*Default*/
    prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_DEFAULT].key    = 0;
    prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_DEFAULT].mask   = _OAMP_PE_TCAM_MASK_DONT_CARE; /* No sense using the macros. Everyhing is "Don't care" */
    prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_DEFAULT].dat    = 0; /*This is hard coded because 0 should always be the default program.*/
    prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_DEFAULT].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_DEFAULT].instruction_func != NULL;

    /*OAMP server*/
    SET_OAMP_PE_TCAM_PACKET_SOURCE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].key,0 ); /* MEP generated or MEP response*/
    SET_OAMP_PE_TCAM_PACKET_SOURCE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].mask, 1);
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_TYPE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].mask,0xf );
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PROFILE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].mask,0xf );
    /* selection only based on MEP PE profile.*/
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PE_PROFILE_KEY_JER(prog_tcam_entries, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER);
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PE_PROFILE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].mask, 0);
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_MSG_TYPE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].key, 3);
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_MSG_TYPE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].mask,3 );
    prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].dat    = programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].prog_used;
    prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].valid  = programs[unit][ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER].instruction_func != NULL;


    /* 1-DM */
    SET_OAMP_PE_TCAM_PACKET_SOURCE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].key, ARAD_PP_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED);
    SET_OAMP_PE_TCAM_PACKET_SOURCE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].mask, 0);
    /* Mep must be DM*/
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_TYPE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].mask, 0); 
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_TYPE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].key, SOC_PPC_OAM_MEP_TYPE_DM); 
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PROFILE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].mask, 0x7f); 
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_MSG_TYPE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].mask, 0);
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_MSG_TYPE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].key, ARAD_PP_OAMP_PE_TCAM_MEP_MSG_TYPE_DMM_R);
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PE_PROFILE_KEY_JER(prog_tcam_entries, ARAD_PP_OAMP_PE_PROGS_1DM);
    SET_OAMP_PE_TCAM_PACKET_SOURCE_MEP_GENERATED_MEP_PE_PROFILE_JER(prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].mask, 9);
    prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].dat          = programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].prog_used;
    prog_tcam_entries[ARAD_PP_OAMP_PE_PROGS_1DM].valid        = programs[unit][ARAD_PP_OAMP_PE_PROGS_1DM].instruction_func != NULL;


     /* Sort the array so that the least inclusive masks appear first.*/
    soc_sand_os_qsort(prog_tcam_entries, ARAD_PP_OAMP_PE_PROGS_NOF_PROGS, sizeof(ARAD_PP_TCAM_ENTRY), _arad_pp_tcam_cmp);

    for (i = 0; i < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS; ++i) {
        if (prog_tcam_entries[i].dat != -1) {
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



uint32 _arad_pp_oamp_pe_write_inst_eth_tlv(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_ETH_TLV);

/*
   TLV bug fix program

   This program removes the END_TLV field which is wrongly put between the CCM PDU and the optional PORT/INTERFACE status TLVs
   It places at the end of the packet END_TLV field full of 0's

 */

/*  Put the Variable0 (Up/Down) in the TmpReg1 */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Prepare the address for jump on Down packets in TmpReg2 */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 14));            /*  Jump address for Down packet address     */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 28));           /*   Jump address for 92 bytes case     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Put the Variable1 (Number of VLANs) to TmpReg2 - RAW format  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X2);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG2);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 35));           /*   Jump address for 96 bytes case     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));




/*
  Put the Variable1 (Number of VLANs) to TmpReg2 
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X2);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG2);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));
    */

/*  Test for Up/Down packet.  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
/* inst_mem_entry [`OAMP_PE_FEM2_SEL_BITS]         = 4'h1; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG2);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_AND);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);  /*  The value for testing  of 0X0?     */
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

/* -------------------------------------------------------------------------------------------------------- UP VLANs Test Part */
/* ********************************************** */
/*  */
/*     The packet is Up packet - Test for the VLANs */
/*  */
/* ********************************************** */

/*  Clean the Variable1 from the unneeded fields (Encapsulation 0 Number of VLANs) in the TmpReg2*/
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
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

/*  Put The value for the jump in case of 88 bytes in TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 21));           /*   Jump address for 88 bytes case     */
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

/*  Test for 2 VLANs Jump to the address in TmpReg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);         /*  Num of VLANs     */
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Test for 1 VLANs Jump to the address in TmpReg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);       /*  Num of VLANs     */
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Test for 0 VLANs Jump to the address in TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);         /*  Num of VLANs     */
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG1);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/* -------------------------------------------------------------------------------------------------------- DOWN VLANs Test Part */
/* ********************************************** */
/*  */
/*     The packet is DOWN packet - Test for the VLANs */
/*  */
/* ********************************************** */

/*  Clean the Variable1 from the unneeded fields (Encapsulation 0 Number of VLANs) in the TmpReg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
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

/*  Put The value for the jump in case of 100 bytes in TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 42));                /*  Jump address for 100 bytes case     */
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

/*  Test for 1 VLANs Jump to the address in TmpReg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG4);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Test for 0 VLANs Jump to the address in TmpReg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG3);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Test for 2 VLANs Jump to the address in Program Variable */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_TMP_REG1);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 53));
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 50));
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 53));
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 50));
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 53));
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 50));
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


/* -------------------------------------------------------------------------------------------------------- 100 Bytes Part */
/* ********************************************** */
/*  */
/*  The case of 100 bytes before the PDU */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 53));
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Copy 100 Bytes from In-Fifo to the Buffer */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 92);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 107);
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, (fst_inst_addr + 50));
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
                            "Ethernet TLV program successfully loaded!\n")));
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



uint32 _arad_pp_oamp_pe_write_inst_lm_6374(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SET_FIRST_INSTR(fst_inst_addr, ARAD_PP_OAMP_PE_PROGS_6374_LM);


    /*
 Inserts four bytes from "program variable" into the front of the packet
 */

/* #1 Copy 12Bytes from In-Fifo to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NNEG);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xc);  /* 12     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Get the last two bytes before the OAM-TS header and put them in the buffer */
/*  Store the third byte - the OAM-TS.offset field inside in the TempReg1 */
/*  Store the whole 4 bytes also in the Feedback Fifo. Later the last byte from them will be outputted as the first byte from the PPH */

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #7  Store the number 16 in TmpReg2. This is the constant which needs to be added to OAM-TS.offset field */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 16);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #7 Put the value for the shifting after the calculation of the OAM-TS.offset field in TmpReg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Shift the TmpReg1 one byte right with fem and store it in TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X2);     /*  Take the 1'st byte and put in on byte 0     */
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Put value for the shifting of the data from the Feedback Fifo in the TmpReg4 */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* #6 NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Add 20 to the OAM-TS.offset field and store the result in TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* #6 NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* #6 NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the calculated OAM-TS.offset field to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_REG3);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the first byte from the PPH header from the 4'th byte in the Feedback Fifo.  */
/*  Put the number for the last two bytes of the ACH/GACH to the TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_FDBK_FIFO);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0a);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the next 3 portions of 4 bytes form the In-Fifo to the Buffer. */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NNEG);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 20);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Pop the last two bytes of the ACH/GACH and the first two bytes from the PDU from the In-Fifo */
/*  The two bytes from the ACH/GACH will be replaced with 0x000A */
/*  The bytes are not outputed */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
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

/*  Output the last two bytes from the ACH/GACH which are situated in TmpReg1 to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
/* inst_mem_entry [`OAMP_PE_BUF_EOP_BIT]           = 1'b1; */
/* inst_mem_entry [`OAMP_PE_CMP1_ACT_BITS]	      = `OAMP_PE_CMP_ACT_ALWAYS; */
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Pop the next 3 portions of 4 bytes form the In-Fifo to the Buffer. */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NNEG);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 36);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Pop the last two bytes from the PDU from the In-Fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
/* inst_mem_entry [`OAMP_PE_BUF_EOP_BIT]         = 1'b1; */
/* inst_mem_entry [`OAMP_PE_CMP1_ACT_BITS]	      = `OAMP_PE_CMP_ACT_ALWAYS; */
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/* write_instruction_mem (fst_inst_addr+ 16,inst_mem_entry_copy_till_eop); **inst_mem_entry_empty_fifo_till_eop); **inst_mem_entry_copy_till_eop); */

/* ****************************************/

/*  */
/*  RFC6374 PDU building */
/*  */
/* ****************************************/




/*  Output the ProgramVariable (Version, Flags, Control Code, Message Length from the PDU) to the Buffer */
/*  Prepare the value for the DFlags and OTF fields in the TempReg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xd0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Prepare the value for the shifting of the DFlags and OTF fields from TmpReg4 to the Buffer - Destination TmpReg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
/* inst_mem_entry [`OAMP_PE_MERGE2_INST_BITS]      = `OAMP_PE_MERGE_INST_FEM; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
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


/* #5 NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Prepare the value for the Reserved field in the TmpReg4 - The value is 0s */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
/* inst_mem_entry [`OAMP_PE_MERGE2_INST_BITS]      = `OAMP_PE_MERGE_INST_FEM; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the DFlags and OTF fields from TmpReg4 to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Prepare the DS field in the TmpReg4 */
/*  WARNING !!!!!! The upper 2 MS bits belong to Session-ID field. Must be 0s. Do not change them! */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
/* inst_mem_entry [`OAMP_PE_MERGE2_INST_BITS]      = `OAMP_PE_MERGE_INST_FEM; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X31);          /*  DS field      */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Prepare the value for the counter for the last 40 bytes full of 0s in the TmpReg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 12);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Output the Reserved field (3 Bytes) + The First MSB from the Session ID field which is full with 0s */
/*  Prepare the zeros for the last 40 bytes of 0s in the TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the Session-ID field from the Program Offset Register 3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0X1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG2);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Oputput the DS field  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output 40 bytes full of 0s */

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Oputput the last 4bytes of 0s before the END_TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Oputput the END_TLV field  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "RFC6374 LM program successfully loaded!\n")));
           exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}



uint32 _arad_pp_oamp_pe_write_inst_dm_6374(SOC_SAND_IN int unit) {
    uint32 res;
    soc_reg_above_64_val_t reg_data, reg_field;
    int fst_inst_addr;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SET_FIRST_INSTR(fst_inst_addr,ARAD_PP_OAMP_PE_PROGS_6374_DM);

    /*
 Inserts four bytes from "program variable" into the front of the packet
 */

/* #1 Copy 12Bytes from In-Fifo to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NNEG);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xc);  /* 12     */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Get the last two bytes before the OAM-TS header and put them in the buffer */
/*  Store the third byte - the OAM-TS.offset field inside in the TempReg1 */
/*  Store the whole 4 bytes also in the Feedback Fifo. Later the last byte from them will be outputted as the first byte from the PPH */

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_MERGE2_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_FDBK_FF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #7  Store the number 8 in TmpReg2. This is the constant which needs to be added to OAM-TS.offset field */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 8);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  #7 Put the value for the shifting after the calculation of the OAM-TS.offset field in TmpReg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Shift the TmpReg1 one byte right with fem and store it in TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0X2);     /*  Take the 1'st byte and put in on byte 0     */
    SET_OAMP_PE_MUX2_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_FEM);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Put value for the shifting of the data from the Feedback Fifo in the TmpReg4 */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* #6 NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Add 20 to the OAM-TS.offset field and store the result in TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG1);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* #6 NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/* #6 NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the calculated OAM-TS.offset field to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_REG3);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the first byte from the PPH header from the 4'th byte in the Feedback Fifo.  */
/*  Put the number for the last two bytes of the ACH/GACH to the TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FDBK_FF_RD_BIT(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_FDBK_FIFO);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG4);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0c);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the next 3 portions of 4 bytes form the In-Fifo to the Buffer. */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NNEG);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 20);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Pop the last two bytes of the ACH/GACH and the first two bytes from the PDU from the In-Fifo */
/*  The two bytes from the ACH/GACH will be replaced with 0x000C */
/*  The bytes are not outputed */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
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

/*  Output the last two bytes from the ACH/GACH which are situated in TmpReg1 to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
/* inst_mem_entry [`OAMP_PE_BUF_EOP_BIT]           = 1'b1; */
/* inst_mem_entry [`OAMP_PE_CMP1_ACT_BITS]	      = `OAMP_PE_CMP_ACT_ALWAYS; */
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/* write_instruction_mem (fst_inst_addr+ 15,inst_mem_entry_copy_till_eop); **inst_mem_entry_empty_fifo_till_eop); **inst_mem_entry_copy_till_eop); */

/*  Pop the next 3 portions of 4 bytes form the In-Fifo. */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_BYTES_RD);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 56);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Pop the last three bytes from the PDU from the In-Fifo */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_IN_FIFO);
    SET_OAMP_PE_IN_FIFO_RD_BITS(reg_field, 1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 0);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
/* inst_mem_entry [`OAMP_PE_BUF_EOP_BIT]           = 1'b1; */
/* inst_mem_entry [`OAMP_PE_CMP1_ACT_BITS]	      = `OAMP_PE_CMP_ACT_ALWAYS; */
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/* write_instruction_mem (fst_inst_addr+ 16,inst_mem_entry_copy_till_eop); **inst_mem_entry_empty_fifo_till_eop); **inst_mem_entry_copy_till_eop); */


/* ****************************************/

/*  */
/*  RFC6374 PDU building */
/*  */
/* ****************************************/




/*  Output the ProgramVariable (Version, Flags, Control Code, Message Length from the PDU) to the Buffer */
/*  Prepare the value for the DFlags and OTF fields in the TempReg4 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_PROG_VAR);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_VLD_B);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xd0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Prepare the value for the shifting of the QTF, OTF and RPTF fields from TmpReg4 to the Buffer - Destination TmpReg3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
/* inst_mem_entry [`OAMP_PE_MERGE2_INST_BITS]      = `OAMP_PE_MERGE_INST_FEM; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG3);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Prepare the value for the RPTF field in the TmpReg2. */
/*  Warning!!! The 4LSbits from this byte belong to the Reserved field, must keep them 0 */
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
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0Xa0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Prepare the value for the Reserved field in the TmpReg4 - The value is 0s */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
/* inst_mem_entry [`OAMP_PE_MERGE2_INST_BITS]      = `OAMP_PE_MERGE_INST_FEM; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X0);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/* #5 NOP - Bereket */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  Output the QTF and OTF fields from TmpReg4 to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the RPTF field from TmpReg2 to the Buffer */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG2);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));



/*  Prepare the DS field in the TmpReg4 */
/*  WARNING !!!!!! The upper 2 MS bits belongs to Session-ID field. Must be 0s. Do not change them! */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
/* inst_mem_entry [`OAMP_PE_MERGE2_INST_BITS]      = `OAMP_PE_MERGE_INST_FEM; */
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG4);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X31);          /*  DS field      */
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Prepare the value for the counter for the last 40 bytes full of 0s in the TmpReg2 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 8);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Output the Reserved field (2 Bytes) + The First MSB from the Session ID field which is full with 0s */
/*  Prepare the zeros for the last 40 bytes of 0s in the TmpReg1 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_ADD);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 3);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output the Session-ID field from the Program Offset Register 3 */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0X1);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_OFFST_REG2);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_CONST);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 0X2);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Oputput the DS field  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG4);
    SET_OAMP_PE_SHIFT1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_SHIFT_SRC_REG3);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Output 40 bytes full of 0s */

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_OP1_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_TMP_REG2);
    SET_OAMP_PE_OP2_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP_SEL_INST_VAR);
    SET_OAMP_PE_OP3_SEL_BITS(reg_field, ARAD_PP_OAMP_PE_OP3_SEL_ALU_OUT);
    SET_OAMP_PE_ALU_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_ACT_SUB);
    SET_OAMP_PE_ALU_DST_BITS(reg_field, ARAD_PP_OAMP_PE_ALU_DST_TMP_REG2);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ZERO);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_LOP_NXT);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Oputput the last 4bytes of 0s before the END_TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


/*  Oputput the last 4bytes of 0s before the END_TLV */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 4);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));

/*  Oputput the END_TLV field  */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SET_OAMP_PE_FEM1_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_FEM2_SEL_BITS(reg_field, 0Xb);
    SET_OAMP_PE_MUX1_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_MUX_SRC_TMP_REG1);
    SET_OAMP_PE_MERGE1_INST_BITS(reg_field, ARAD_PP_OAMP_PE_MERGE_INST_FEM);
    SET_OAMP_PE_BUFF_WR_BIT(reg_field, 1);
    SET_OAMP_PE_BUFF_SIZE_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_CONST);
    SET_OAMP_PE_BUF_EOP_BIT(reg_field, 1);
    SET_OAMP_PE_CMP1_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_ALWAYS);
    SET_OAMP_PE_CMP2_ACT_BITS(reg_field, ARAD_PP_OAMP_PE_CMP_ACT_NEVER);
    SET_OAMP_PE_INST_SRC_BITS(reg_field, ARAD_PP_OAMP_PE_INST_SRC_NXT_JMP);
    SET_OAMP_PE_INST_CONST_BITS(reg_field, 1);
    soc_OAMP_PE_PROGRAMm_field_set(unit, reg_data, PROG_DATAf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit,
                                 WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, last_program_pointer[unit]++, reg_data));


    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "RFC6374 DM program successfully loaded!\n")));
           exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}



uint32 _arad_pp_oamp_pe_write_inst_push_ra(SOC_SAND_IN int unit) {
    /*Program not implemented at the time of writing*/

    LOG_ERROR(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "Push RA program not implemented. Program should not be loaded;\n")));
               return -1;
}



uint32 _arad_pp_oamp_pe_write_inst_pop_cw(SOC_SAND_IN int unit) {
    /*Program not implemented at the time of writing*/
    LOG_ERROR(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "Pop CW program not implemented. Program should not be loaded;\n")));
               return -1;
}



uint32 _arad_pp_oamp_pe_write_inst_push_ra_and_pop_cw(SOC_SAND_IN int unit) {
    /*Program not implemented at the time of writing*/

    LOG_ERROR(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "Push RA and Pop CW program not implemented. Program should not be loaded;\n")));
               return -1;
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

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(ls_field);
    SOC_SAND_CHECK_NULL_INPUT(ms_field);

    switch (prog_pe_profile) {
    case 0:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_0f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_0f;
        break;
    case 1:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_1f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_1f;
        break;
    case 10:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_10f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_10f;
        break;
    case 11:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_11f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_11f;
        break;
    case 12:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_12f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_12f;
        break;
    case 13:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_13f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_13f;
        break;
    case 14:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_14f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_14f;
        break;
    case 15:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_15f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_15f;
        break;
    case 2:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_2f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_2f;
        break;
    case 3:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_3f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_3f;
        break;
    case 4:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_4f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_4f;
        break;
    case 5:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_5f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_5f;
        break;
    case 6:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_6f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_6f;
        break;
    case 7:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_7f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_7f;
        break;
    case 8:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_8f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_8f;
        break;
    case 9:
        *ls_field = PE_LS_NIBBLE_SELECT_PROF_9f;
        *ms_field = PE_MS_NIBBLE_SELECT_PROF_9f;
        break;
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong.")));
}


/**
 * Used ONLY by the server program. If any future program needs 
 * this the configurations must be changed. 
 * 
 * @author sinai (09/09/2014)
 * 
 * @param unit 
 * 
 * @return STATIC uint32 
 */
STATIC uint32    _arad_pp_oamp_pe_write_pe_gen_mem(SOC_SAND_IN int unit) {
    uint32 res,reg,i;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (i = 0; i < 256; ++i) {
        /* For entry N put the value N.*/
        soc_OAMP_PE_GEN_MEMm_field_set(unit, &reg, PE_GEN_DATAf, &i);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_OAMP_PE_GEN_MEMm(unit, MEM_BLOCK_ALL, i, &reg));
    }

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

    for (prog=0 ;prog < ARAD_PP_OAMP_PE_PROGS_NOF_PROGS ; ++prog) {
        if (programs[unit][prog].instruction_func) {
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
    int i, prog_used = 0;

    /*
     * Lfem values, each a constant 96 bit field
     */
    CONST static soc_reg_above_64_val_t lfem_values[ARAD_PP_OAMP_PE_MAX_LFEM]= {
        /*LFEM 0*/ {0x20820820, 0x20408208, 0x1c61440c},
        /*LFEM 1*/ {0x57595513, 0x08207DE7, 0x82082082},
        /*LFEM 2*/ {0x5C6DA658, 0x08207DE7, 0x82082082},
        /*LFEM 3*/ {0x40820820, 0x61440C20, 0x2CA2481C},
        /*LFEM 4*/ {0x58820820, 0x081C6DA6, 0x82082082},
        /** Jericho LFEMS (all zero) */
        /*LFEM 5*/ {0, 0, 0},
        /*LFEM 6*/ {0, 0, 0},
        /*LFEM 7*/ {0, 0, 0},
        /*LFEM 8*/ {0, 0, 0},
        /*LFEM 9*/ {0, 0, 0},
        /*LFEM 10*/ {0, 0, 0}

    };
    /* May very well be backwards!!! Remove this comment once that has been verified not to be the case.*/

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    _arad_pp_oamp_pe_programs_init(unit);

    /*First, initialize and allocate the programs we want, together with their respective lfems. and mep pe profiles. */
    _arad_pp_oamp_pe_programs_usage_init(unit);

    _arad_pp_oamp_pe_init_lfems(unit);
    _arad_pp_oamp_pe_mep_pe_init(unit);

    res = _arad_pp_oamp_pe_programs_alloc(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    res = _arad_pp_oamp_pe_write_nibble_selection_profiles(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);


    res = _arad_pp_oamp_pe_write_pe_gen_mem(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);


    /*Given the programs initialized above, we write to the tcam table and then 
      the Programmable Editor program mem.*/
    res = _OAM_OAMP_PE_FUNCTION_CALL(_arad_pp_oamp_pe_write_to_tcam, (unit));
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

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

