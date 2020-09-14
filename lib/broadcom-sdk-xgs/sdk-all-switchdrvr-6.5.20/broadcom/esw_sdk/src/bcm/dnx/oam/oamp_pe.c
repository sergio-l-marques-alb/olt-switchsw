/** \file oamp_pe.c
 * $Id$
 *
 * OAMP PE procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <src/bcm/dnx/oam/oamp_pe.h>
#include "src/bcm/dnx/oam/oamp_pe_infra.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <soc/dnx/swstate/auto_generated/access/oam_access.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** FEM used as offset/const */
#define OFST DNX_OAMP_PE_FEM_OFST
#define CNST DNX_OAMP_PE_FEM_CNST

/** used by BFD echo for UDP dport - 3785 */
#define DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR 0x0EC90000
#define DNX_OAMP_PE_BFD_IPV6_JR1_PROG_VAR       0x0EC90020
/** used by BFD_IPv6_MPLS for IPv6 header calculation */
#define DNX_OAMP_PE_BFD_IPV6_MPLS_PROG_VAR 0x7F0000FF
/** used by 1DM for opcode configuration */
#define DNX_OAMP_PE_1DM_PROG_VAR 0x2D10FFFC
/** used as PW-ACH  for mpls tp vccv type bfd*/
#define DNX_OAMP_PE_BFD_VCCV_PROG_VAR 0x10000021

/*
 * }
 */

/*
 * Inner functions
 * {
 */

/**
* \brief
*  Function with defaul instruction.
*  The programs outputs the incoming packet without modifications.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_function_default(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load function
     */

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /*
         * configure set of requested instructions
         */
        /* *INDENT-OFF* */
        /** read 4 bytes from IN_FIFO and output to buffer as is - perform in loop till EOP */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], ALWAYS_CMP2, LOP_NXT), DNX_OAMP_PE_INST_LABEL_DEFAULT);
        /* *INDENT-ON* */
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  MAID_48 function loading.
*  The function is used from other functions to handle missalignment and to copy MEG_ID.
*  Need to place first 48B of prefix instead of the ones in packet starting from 10B after system headers.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_function_copy_48_maid(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{

    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load function
     */

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /*
         * These set of instructions are used by all the programs need to handle MAID 48B.
         * Note that this is not stand alone program.
         *
         * The packets that are supported by the program are (UDH of different size an be added to each packet):
         * PTCH-2-ETH-PDU(taggless,single tagged,double tagged)
         * PTCH2-ITMH-OAM-TS-ETH-PDU  /with OAM-TS/
         * PTCH-2-ITMH-ETH-PDU /without OAM-TS/
         * PTCH2-ITMH-ITMH ext- OAM-TS- PPH - LSP-GAL-GACH-PDU  /with OAM-TS/
         * PTCH2-ITMH-ITMH ext- PPH - LSP-GAL-GACH-PDU  /without OAM-TS/
         * PTCH2-ITMH-ITMH ext- OAM-TS- PPH - PWE-GACH-PDU  /with OAM-TS/
         * PTCH2-ITMH-ITMH ext- PPH - PWE -GACH-PDU  /without OAM-TS/
         *
         */
        /* *INDENT-OFF* */
        /*
        INPUT:
        +-------------------------------------------------------+-----------------+-------------------+-------------------*
        | All programs that use this function                   | TMP_REG1        | IN_FIFO 4Bs       |  Feedback FIFO    |
        |-------------------------------------------------------+-----------------+-------------------|-------------------|
        | oamp_pe_maid_48_program                               | Should store the| 2B(PTCH) 2B(ITMH) | Should store 48B  |
        |-------------------------------------------------------+ offset till the +-------------------| MEG_ID taken from |
        | oamp_pe_down_mep_inj_self_contained_maid_48_program   | MEG_ID field of | 0-4(UDH) 0-4(ETH) | additional data.  |
        |-------------------------------------------------------+ the PDU         +-------------------|                   |
        |                                                       |                 |                   |                   |
        |-------------------------------------------------------+                 +-------------------|                   |
        |                                                       |                 |                   |                   |
        +-------------------------------------------------------+-----------------+-------------------*-------------------|
         */

        /** temp_reg2=temp_reg1(2bit lsb) - upd+10B misalinment */
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 3, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_MAID_48);
            /** nops because of tmp_reg2*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        /** temp_reg1-=temp_reg2 - align temp_reg1 for 4B */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        /** copy pdu headers including first 10B of OAM header */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));
        /** set temp_reg4=1, for alignment validation (set#1)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 1, NEVER_NEVER, NXT_JMP));
            /** set temp_reg4=2, for alignment validation (set#2)*/
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 2, NEVER_NEVER, NXT_JMP));
                /** set temp_reg4=3, for alignment validation (set#3)*/
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 3, NEVER_NEVER, NXT_JMP));
        /** set jump for pdu+10 % 4 == 1 (value for set#1 is ready)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_1B], NEVER_NEVER, NXT_JMP));
            /** set jump for pdu+10 % 4 == 2 (value for set#2 is ready)*/
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_2B], NEVER_NEVER, NXT_JMP));
                 /** set jump for pdu+10 % 4 == 3 (value for set#3 is ready)*/
                 INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_3B], NEVER_NEVER, NXT_JMP));

                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /*
        * pdu+10 % 4 == 0 case
        */
        /** copy 48B from FDBK fifo */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 12, ALWAYS_NEVER, LOP_CST));
        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /*
        * pdu+10 % 4 == 1 case
        */
        /** copy 1B related to misalignment, 3B from FDBK fifo */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(FDBK_FIFO,DEFAULT,INST_CONST,3B), ALU_NOP, CMN(READ, NONE, 0x1, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_MAID_48_1B);
        /** copy 1B from FDBK fifo, this will align FDBK fifo to 4B chuncks */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 0x31, ALWAYS_NEVER, NXT_JMP));
        /** copy 40B from FDBK fifo */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 10, ALWAYS_NEVER, LOP_CST));
        /** copy last byte from FDBK_FIFO */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0, ALWAYS_NEVER, NXT_JMP));
        /** copy 3B from IN_FIFO to align back */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x3, ALWAYS_NEVER, NXT_JMP));
        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /*
        * pdu+10 % 4 == 2 case
        */
        /** copy 2B related to misalignment, 2B from FDBK fifo */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(FDBK_FIFO,DEFAULT,INST_CONST,2B), ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_MAID_48_2B);
        /** copy 2B from FDBK fifo, this will align FDBK fifo to 4B chuncks */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 0x22, ALWAYS_NEVER, NXT_JMP));
        /** copy 40B from FDBK fifo */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 10, ALWAYS_NEVER, LOP_CST));
        /** copy last byte from FDBK_FIFO */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0, ALWAYS_NEVER, NXT_JMP));
        /** copy 2B from IN_FIFO to align back */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /*
        * pdu+10 % 4 == 3 case
        */
        /** copy 3B related to misalignment, 1B from FDBK fifo */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(FDBK_FIFO,DEFAULT,INST_CONST,1B), ALU_NOP, CMN(READ, NONE, 0x3, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_MAID_48_3B);
        /** copy 3B from FDBK fifo, this will align FDBK fifo to 4B chuncks */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 0x13, ALWAYS_NEVER, NXT_JMP));
        /** copy 40B from FDBK fifo */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 10, ALWAYS_NEVER, LOP_CST));
        /** copy last byte from FDBK_FIFO */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B from IN_FIFO to align back */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));
        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
        /* *INDENT-ON* */

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Default program loading.
*  The programs outputs the incoming packet without modifications.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_default_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        /*
         *  configure requested instructions
         */
        if (func_config[DNX_OAMP_PE_FUNC_DEFAULT].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DEFAULT].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_DEFAULT].first_instruction));
        }

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_DEFAULT].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  BFD_ECHO program loading.
*  The programs outputs the incoming packet with updated UDP destination port.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_bfd_echo_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    int udh_header_size = 0;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    /** Needed for setting PPH.TTL */
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        /** Calculate the parts of PPH.TTL */
        pph_ttl = 0xFF;
        pph_ttl_msb = pph_ttl >> 5;
        pph_ttl_lsb = (pph_ttl << 3) & 0xFF;

        /* *INDENT-OFF* */
        /*
        * copy:
        * PTCH(2B)
        * ITMH(5B)
        * ITMH-EXT(3B)
        * PPH(12B) - modify PPH.TTL
        * UDH based on 'udh_size' predefined size (0-17B)
        * IPv4(20B)
        * UDP sport(2B)
        * UDP dport(2B)
        * rest UDP(4B) - length, checksum
        * BFD(24B)
        **/
        /** copy 16B {PTCH, ITMH, ITMH-EXT, PPH (first 6 bytes)} */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

        /** copy next two bytes */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /** Copy 1 byte to tmp_reg1 */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

        /** Copy 1 byte to tmp_reg2 */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

            /** NOP for the value to be ready */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Write 3 PPH.TTL bits to tmp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));

        /** Write 5 PPH.TTL bits to tmp_reg2 */
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));

            /** NOP for the value to be ready */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Copy result from TMP_REG1 */
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        /** Copy result from TMP_REG2 */
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        /** copy 24B {PPH (last 2 bytes), UDH, IPv4(pending UDH size)} */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 6, ALWAYS_NEVER, LOP_CST));

        while (udh_header_size >= 4)
        {
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            udh_header_size -=4;
        }

       /*
        * handle all the possible UDP.dport offsets caused by varying UDH header size:
        * 0 - {UDP dport(2B), UDP length(2B)}
        * 1 - {UDP sport(1B), UDP dport(2B), UDP length(1B)}
        * 2 - {UDP sport(2B), UDP dport(2B)}
        * 3 - {end of IPv4 header(1B), UDP sport(2B), UDP dport(1B)}, {UDP dport(1B), UDP(3B)}
        */

        switch (udh_header_size)
        {
            case 0:
                /** Set Destination Port */
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                /** copy 2B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;

            case 1:
                /** copy 1B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                /** Set Destination Port */
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                /** copy 1B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;

            case 2:
                /** copy 2B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                /** Set Destination Port */
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                break;


            case 3:
                /** copy 3B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

                /** Set Destination Port */
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                /** copy 3B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }

        /** Copy rest of packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
        /* *INDENT-ON* */

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  BFD_ECHO program loading.
*  The programs outputs the incoming packet with updated UDP destination port.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_bfd_echo_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr, instr_const, udp_legth_offset = 0;
    uint32 saved_prog_index;
    int udh_header_size = 0;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        /* *INDENT-OFF* */
        /*
        * copy:
        * PTCH(2B)
        * ITMH(4B)
        * ITMH-EXT(3B)
        * PPH(7B)
        * UDH based on 'udh_size' predefined size (0-17B)
        * IPv4(20B)
        * UDP sport(2B)
        * UDP dport(2B)
        * rest UDP(4B) - length, checksum
        * BFD(24B)
        **/
        /** copy 36B {PTCH, ITMH, ITMH-EXT, PPH, UDH, IPv4(pending UDH size)} */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 9, ALWAYS_NEVER, LOP_CST));

        while (udh_header_size >= 4)
        {
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            udh_header_size -=4;
        }

        /*
         * handle all the possible UDP.dport offsets caused by varying UDH header size:
         * 0 - {UDP sport(2B), UDP dport(2B)}
         * 1 - {end of IPv4 header(1B), UDP sport(2B), UDP dport(1B)}, {UDP dport(1B), UDP(3B)}
         * 2 - {end of IPv4 header(2B), UDP sport(2B)}, {UDP dport(2B), UDP length(2B)}
         * 3 - {end of IPv4 header(3B), UDP sport(1B)}, {UDP sport(1B), UDP dport(2B), UDP length(1B)}
         */
        switch (udh_header_size)
        {
            case 0:
                /** copy 2B */
                instr_const = 2;
                break;
            case 1:
                /** copy 3B */
                instr_const = 3;
                /** copy 3B shifted with 1 */
                udp_legth_offset = 0x13;
                break;
            case 2:
                /** copy 4B */
                instr_const = 4;
                /** copy 2B shifted with 2 */
                udp_legth_offset = 0x22;
                break;
            case 3:
                /** copy 4B */
                instr_const = 4;
                /** copy 1B shifted with 3 */
                udp_legth_offset = 0x31;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));

        if (udh_header_size == 3)
        {
            /** copy 1B */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        }
        /** Set Destination Port */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        if (udh_header_size != 0)
        {
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, udp_legth_offset, ALWAYS_NEVER, NXT_JMP));
        }

        /** Copy rest of packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
        /* *INDENT-ON* */

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  MAID_48 program loading.
*  The program gets the packet with prefix of 80B that are not sent out.
*  Need to place first 48B of prefix instead of the ones in packet starting from 10B after system headers, after that jumps to oamp_pe_copy_48b_maid_program.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_maid_48_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /*
         * configure set of requested instructions
         */
        /*
         * The program uses additional data(80 bytes) that comes before the packet.
         * The program stores 48 bytes in feedback fifo and the rest 32 are dropped.
         * The program fill MEG-ID (at PDU+10 offset) with 48 bytes from the additional data.
         *
         * The packets that are supported by the program are (UDH of different size an be added to each packet):
         * PTCH-2-ETH-PDU(taggless,single tagged,double tagged)
         * PTCH2-ITMH-OAM-TS-ETH-PDU  /with OAM-TS/
         * PTCH-2-ITMH-ETH-PDU /without OAM-TS/
         * PTCH2-ITMH-ITMH ext- OAM-TS- PPH - LSP-GAL-GACH-PDU  /with OAM-TS/
         * PTCH2-ITMH-ITMH ext- PPH - LSP-GAL-GACH-PDU  /without OAM-TS/
         * PTCH2-ITMH-ITMH ext- OAM-TS- PPH - PWE-GACH-PDU  /with OAM-TS/
         * PTCH2-ITMH-ITMH ext- PPH - PWE -GACH-PDU  /without OAM-TS/
         *
         */
        /* *INDENT-OFF* */
        /** set PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        /** copy 48B from packet into FDBK fifo */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** skip 32B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_copy_48b_maid_program */
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
            /** NOPs because of the Jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction));
        }
    /* *INDENT-ON* */
        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  lmdm_flexible_da_maid_48_ccm program loading.
*  When LMDM flexible DMAC enabled on MAID48 MEP, 48 bytes MAID is put into extra data right after flexible DMAC (occupy 8 bytes)
*  Compared with standard maid_48 program, this program skips the first 8 bytes and then gets 48 bytes MAID
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_lmdm_flexible_da_maid_48_ccm_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /*
         * configure set of requested instructions
         */
        /*
         * The program uses additional data(80 bytes) that comes before the packet.
         * The program drops the first 8 bytes, then stores 48 bytes in feedback fifo, and the last 24 are dropped.
         * The program fill MEG-ID (at PDU+10 offset) with 48 bytes from the additional data.
         *
         * The packets that are supported by the program are (UDH of different size an be added to each packet):
         * PTCH-2-ETH-PDU(taggless,single tagged,double tagged)
         * PTCH2-ITMH-OAM-TS-ETH-PDU  /with OAM-TS/
         * PTCH-2-ITMH-ETH-PDU /without OAM-TS/
         * PTCH2-ITMH-ITMH ext- OAM-TS- PPH - LSP-GAL-GACH-PDU  /with OAM-TS/
         * PTCH2-ITMH-ITMH ext- PPH - LSP-GAL-GACH-PDU  /without OAM-TS/
         * PTCH2-ITMH-ITMH ext- OAM-TS- PPH - PWE-GACH-PDU  /with OAM-TS/
         * PTCH2-ITMH-ITMH ext- PPH - PWE -GACH-PDU  /without OAM-TS/
         *
         */
        /* *INDENT-OFF* */
        /** set PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        /** skip 8B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
        /** copy 48B from packet into FDBK fifo */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** skip 24B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 6, NEVER_NEVER, LOP_CST));

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_copy_48b_maid_program */
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
                /** NOPs because of the Jump*/
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction));
        }
    /* *INDENT-ON* */
        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  oamp_pe_down_mep_inj_self_contained_program program loading.
*  The program build ITMH ext and PPH header.
*  This program jumps to oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained and build ITMH ext and PPH header.
*  The MEG_ID is stored in feedback FIFO, TMP_REG1 should store the offset till MEG_ID field.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_down_mep_inj_self_contained_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */
    /*
     * The program uses additional data(80 bytes) that comes before the packet.
     * The program stores 48 bytes in feedback fifo and the rest 32 are dropped.
     * It uses nibble selection to build ITMH ext OUT_LIF from MEP_DB
     * The program builds PPH header and take the VSI from MEP_DB
     * The program fill MEG-ID (at PDU+10 offset) with 48 bytes from the additional data.
     *
     * The packets that are supported by the program are (UDH of different size an be added to each packet) also (taggless,single tagged,double tagged):
     * INPUT:
     * PTCH-2(2B) - ITMH(5B)- ETH - PDU (without ASE)
     * PTCH-2(2B) - ITMH(5B) - ASE(6B) - ETH - PDU (with ASE)
     * OUTPUT:
     * PTCH-2(2B) - ITMH(5B)- ITMH-EXT(3B) - PPH(12B) - ETH - PDU (without ASE)
     * PTCH-2(2B) - ITMH(5B) - ASE(6B) - PPH(12B) - ETH - PDU (with ASE)
     */
    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /*
         * configure set of requested instructions
         */

        /* *INDENT-OFF* */
        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained*/
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED], NEVER_NEVER, NXT_JMP));
                /** NOPs because of the Jump*/
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config
                            [DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction));
        }

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
        /* *INDENT-ON* */
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  DOWN_MEP_INJ_SELF_CONTAINED_MAID_48 program loading.
*  The program gets the packet with prefix of 80B that are not sent out.
*  The program jumps tooamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained function and build ITMH ext and PPH header.
*  After all the system headers are built/copied this program jumps to oamp_pe_function_copy_48_maid function to place the MEG_ID 10B after PDU_OFFS.
*  The MEG_ID is stored in feedback FIFO, TMP_REG1 should store the offset till MEG_ID field.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_down_mep_inj_self_contained_maid_48_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */
    /*
     * The program uses additional data(80 bytes) that comes before the packet.
     * The program stores 48 bytes in feedback fifo and the rest 32 are dropped.
     * It usses nibble selection to build ITMH ext OUT_LIF from MEP_DB
     * The program builds PPH header and take the VSI from MEP_DB
     * The program fill MEG-ID (at PDU+10 offset) with 48 bytes from the additional data.
     *
     * The packets that are supported by the program are (UDH of different size an be added to each packet) also (taggless,single tagged,double tagged):
     * INPUT:
     * PTCH-2(2B) - ITMH(5B)- ETH - PDU (without ASE)
     * PTCH-2(2B) - ITMH(5B) - ASE(6B) - ETH - PDU (with ASE)
     * OUTPUT:
     * PTCH-2(2B) - ITMH(5B)- ITMH-EXT(3B) - PPH(12B) - ETH - PDU (without ASE)
     * PTCH-2(2B) - ITMH(5B) - ASE(6B) - PPH(12B) - ETH - PDU (with ASE)
     */
    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /*
         * configure set of requested instructions
         */

        /* *INDENT-OFF* */
        /** copy 48B from packet into FDBK fifo */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** skip 32B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained*/
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED], NEVER_NEVER, NXT_JMP));
            /** NOPs because of the Jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config
                            [DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction));
        }

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
        /* *INDENT-ON* */
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained function loading.
*  The function is used from both program for self_contained entries to build ITMH ext and PPH header.
*  It also updates ITMH field ITMH_BASE_ EXTENSION_EXISTS and in case of maid 48 B group jumps to DNX_OAMP_PE_FUNC_COPY_MAID_48 function.
*  Need to place first 48B of prefix instead of the ones in packet starting from 10B after system headers.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_jr2(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load function
     */

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /*
         * configure set of requested instructions
         */

        /*
         * Theese set of instructions are used by both programs self sontained .
         * Note that this is not stand alone program.
         *
         *
         */
        /* *INDENT-OFF* */
        /*
        INPUT:
        *-------------------------------------------------------+-------------------+-------------------*
        | All programs that use this function                   | IN_FIFO 4Bs       |  Feedback FIFO    |
        |-------------------------------------------------------+-------------------|-------------------|
        | oamp_pe_down_mep_inj_self_contained_maid_48_program   | 2B(PTCH) 2B(ITMH) | 48B MEG_ID        |
        |-------------------------------------------------------+-------------------|-------------------|
        | oamp_pe_down_mep_inj_self_contained_program           | 2B(PTCH) 2B(ITMH) | Empty             |
        *-------------------------------------------------------+-------------------*-------------------*
         */
        /** store 0x1 in MSB of tmp-reg3, used to mask the ITMH_BASE_EXTENSION_EXISTS field*/
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED);

        /** copy 4B(2B PTCH + 2B(MSB) ITMH and update PPH_Exist bit) and save ITMH LSB two bytes in TMP_REG2 for ITMH_BASE Extension field*/
        INSTR(CE1_COPY(IN_FIFO, PPH_EXIST_UPDATE, VLD_B, VLD_B, FEM), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG2), CMN(READ, NONE, 0x20,  ALWAYS_NEVER, NXT_JMP));

        /** copy 2B of the ITMH(3-4)*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /** copy last 1B of the ITMH to buff update the ITMH_BASE_ EXTENSION_EXISTS to 1*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, OR), CE2_COPY(TMP_REG3, DEFAULT, TMP_REG3, 1B), ALU_NOP,CMN(NONE, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));

        /** store 0x8 in MSB of tmp-reg3, used to mask the Injected-AS-Extension-Present field => tmp_reg = 0x0800*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x8, NEVER_NEVER, NXT_JMP));
        /** write 1B of the ITMH ext to buff(6 bits of the OUT_LIF)*/
        INSTR(CE1_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(NS_16_VAR_1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /**
        * store the LSB Byte of the PROG_VAR to tmp_reg2 => tmp_reg2=0x0100(maid48) or 0x0000(not maid48)
        * This is used to check if is it maid48 flow or not.
        * Both program have different PE_CONST values.
        */
        INSTR(CE1_NOP, CE2_COPY(PROG_VAR, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x30, NEVER_NEVER, NXT_JMP));

        /** Check if the packet is with ASE header, if so jump*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_ASE_SELF_CONTAINED], NEVER_NEVER, NXT_JMP));
            /** store 0x3 in MSB of tmp-reg3, used for:
             *  1. update the Parsing-Start-Type and Forwarding-Strength field
             *  2. check if packet is maid48 flow(compare tmp_reg1, where program const resides)
             */
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x3, NEVER_NEVER, NXT_JMP));

            /* Prepare 0x2 for setting in PPH(FHEI-Ext 5 bytes) */
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG4), CMN(NONE, NONE, 0x2, NEVER_NEVER, NXT_JMP));

        /*
         * Case packet is without ASE header
         */
        /** Building PPH Header + 1 byte FHEI-Ext*/
        /** write 2B 0s first 2 of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** write 1B of the PPH to buff(3MSB bits of the VSI)*/
        INSTR(CE1_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT, INST_CONST , INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));
        /** write 2B of the PPH to buff(15LSB bits of the VSI)*/
        INSTR(CE1_COPY(NS_16_VAR_0, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** write 4B 0s of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        /** write 4B (2B from TMP_REG4 setting FHEI-Ext (10 -11B) of the PPH header + 1B from TMP_REG1_MSB (12B of PPH Header) + 1B from TMP_REG1 LSB (FHEI-Ext first byte)*/
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** PPH Header built*/

        /** Set FHEI-Ext byte 1-5*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        /** Set UDH 1 byte (zero) + copy 1B shifted with 3 first 1B of the ETH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
        /** copy PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        /** if 0x0300(tmp_reg3) & PE_CONST LS maid48(0x0100)/not maid48(0x0000)(tmp_reg2) POS jump to DNX_OAMP_PE_INST_LABEL_MAID_48 if not copy the rest*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG2, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
            /** NOPs because of the Jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));


        /*
         * Case packet is with ASE header
         */
        /** copy 1B shifted with 3 First B of the ASE header*/
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_ASE_SELF_CONTAINED);
        /** copy 2B 2-3B of the ASE header*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
        /** zeroing 2B 4-5B of the ASE header in order to reset counter interface value*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B last 1B of the ASE header*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));


        /** Building PPH Header + 1 byte FHEI-Ext*/
        /** write 2B 0s first 2 of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** write 1B of the PPH to buff(3MSB bits of the VSI)*/
        INSTR(CE1_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT, INST_CONST , INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));
        /** write 2B of the PPH to buff(15LSB bits of the VSI)*/
        INSTR(CE1_COPY(NS_16_VAR_0, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** write 4B 0s of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
        /** write 4B (2B from TMP_REG4 setting FHEI-Ext (10 -11B) of the PPH header + 1B from TMP_REG1_MSB (12B of PPH Header) + 1B from TMP_REG1 LSB (FHEI-Ext first byte)*/
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** PPH Header Built*/

        /** Set FHEI-Ext byte 1-5*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        /** Set UDH 1 byte (zero) */
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));
        /** Copy 3B shifted with 1 first 3B of the ETH header*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));

        /** copy PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        /** if 0x0300(tmp_reg3) & PE_CONST LS maid48(0x0100)/not maid48(0x0000)(tmp_reg2) POS jump to DNX_OAMP_PE_INST_LABEL_MAID_48 if not copy the rest*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG2, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
            /** NOPs because of the Jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
        /* *INDENT-ON* */
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained function loading.
*  The function is used from both program for self_contained entries to build ITMH ext and PPH header in jer1 mode.
*  It also updates ITMH field ITMH_BASE_ EXTENSION_EXISTS and in case of maid 48`B group jumps to DNX_OAMP_PE_FUNC_COPY_MAID_48 function.
*  Need to place first 48B of prefix instead of the ones in packet starting from 10B after system headers.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_jr1(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;
    int udh_header_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load function
     */

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** get the predefined UDH header size to be set  */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        /*
         * configure set of requested instructions
         */

        /*
         * These set of instructions are used by both programs self contained .
         * Note that this is not stand alone program.
         *
         *
         */
        /* *INDENT-OFF* */
        /*
        INPUT:
        *-------------------------------------------------------+-------------------+-------------------*
        | All programs that use this function                   | IN_FIFO 4Bs       |  Feedback FIFO    |
        |-------------------------------------------------------+-------------------|-------------------|
        | oamp_pe_down_mep_inj_self_contained_maid_48_program   | 2B(PTCH) 2B(ITMH) | 48B MEG_ID        |
        |-------------------------------------------------------+-------------------|-------------------|
        | oamp_pe_down_mep_inj_self_contained_program           | 2B(PTCH) 2B(ITMH) | Empty             |
        *-------------------------------------------------------+-------------------*-------------------*
         */
        /** store 0x1 in MSB of tmp-reg3, used to set the ITMH_BASE_EXTENSION_EXISTS field*/
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED);

        /* Store in TMP_REG4 the memory offset where VSI is resides */
        INSTR(CE1_NOP, CE2_COPY(NS_16_VAR_0, VSI_GEN_MEM_OFFSET_JR1, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

        /** copy 4B(2B PTCH + 2B(MSB) ITMH and update PPH_Exist bit) and save ITMH MSB two bytes in TMP_REG2 for OAM-TS exist check*/
        INSTR(CE1_COPY(IN_FIFO, PPH_EXIST_UPDATE_JR1, VLD_B, VLD_B, FEM), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG2), CMN(READ, NONE, 0x20,  ALWAYS_NEVER, NXT_JMP));

            /** Setting TMP_REG1 to be 0x0001 for OAM-TS indication*/
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP));


        /** copy last 2B of the ITMH to buff update the ITMH_BASE_ EXTENSION_EXISTS to 1*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, OR), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B), ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /* Read VSI(14 lsb bits from Gen Memory to TMP_REG4 */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(MEM_OUT, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));

        /** store 0x8 in MSB of tmp-reg3, used to mask the PPH-TYPE field(OAM-TS exist) => tmp_reg = 0x0800*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x80, NEVER_NEVER, NXT_JMP));
        /** write 1B of the ITMH ext to buff(2 bits of the OUT_LIF)*/
        INSTR(CE1_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT_JR1, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(NS_16_VAR_1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /** Check if the packet is without OAM-TS header header, if so jump*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_PPH_START_JR1], NEVER_NEVER, NXT_JMP));
           /**
            * store the LSB Byte of the PROG_VAR to tmp_reg2 => tmp_reg2=0x0100(maid48) or 0x0000(not maid48)
            * This is used to check if is it maid48 flow or not.
            * Both program have different PE_CONST values.
            */
            INSTR(CE1_NOP, CE2_COPY(PROG_VAR, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x30, NEVER_NEVER, NXT_JMP));

           /** store 0x3 in MSB(0x0300) of tmp-reg3, used for:
             *  1. update the Parsing-Start-Type and Forwarding-Strength field
             *  2. check if packet is maid48 flow(compare tmp_reg1, where program const resides)
             */
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x3, NEVER_NEVER, NXT_JMP));

        /*
         * Case packet is with ASE header
         */
        /** copy 2B shifted with 2 First B of the ASE header*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        /** Setting zero to TMP_REG1 for OAM-TS header indication */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        /** copy 4B last of the ASE header and s*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        /*
         * Case packet is without ASE header
         */
        /** Building PPH Header + 1 byte FHEI-Ext*/
        /** write 2B of PPH header - taken from program variable and 'OR' with 2MSB bits of Out_Lif*/
        INSTR_LABEL(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_PPH_START_JR1);
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, INST_CONST, INST_CONST, OR), CE2_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT_JR1, INST_CONST, 1B), ALU_NOP, CMN(NONE, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));

        /** write 2B of PPH to buff(14LSB bits of the VSI)*/
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /** write 2B(0x0000) of PPH(byte 0-1) + 2B('0') FHEI-Extention(byte 3-4)*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        /** From this point we should write 3 byte of FHEI-Ext(LSB) + UDH Header that could be 0-8 bytes */
        if(udh_header_size>1)
        {
            /** If UDH Header bigger than 1, write 3 byte of FHEI-Ext + 1 byte of UDH Header */
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
            if(udh_header_size > 5)
            {

                /**
                 * This case is for total UDH length 6,7 or 8 bytes
                 * If UDH Header bigger than 5, write UDH Header bytes 1-4
                 * */
                INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
                /** Write the rest */
                INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, udh_header_size - 5, ALWAYS_NEVER, NXT_JMP));
            }
            else
            {
                /** Write rest of UDH Header(total udh length 2,3,4 or 5 bytes) */
                INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, udh_header_size - 1, ALWAYS_NEVER, NXT_JMP));
            }
        }
        else
        {
             /** write 3 byte of FHEI-Ext(byte(0-3) +  UDH Header(0 or 1 byte)*/
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, udh_header_size + 3, ALWAYS_NEVER, NXT_JMP));
        }

        /** Check if the packet is with ASE header, if so jump(we aligned in this case*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, TMP_REG1, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_ETHERNET_JR1], NEVER_NEVER, NXT_JMP));
            /** NOPs because of the Jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** copy 2B of Ethernet Header in case OAM-TS not exist, for case OAM-TS exist we skip this instruction as we alligned */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        /** copy PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 instructions) to temp_reg1 */
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_ETHERNET_JR1);
        /** if 0x0300(tmp_reg3) & PE_CONST LS maid48(0x0100)/not maid48(0x0000)(tmp_reg2) POS jump to DNX_OAMP_PE_INST_LABEL_MAID_48 if not copy the rest*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG2, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
            /** NOPs because of the Jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_dispatch(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

     /** Calculate the offset to the discriminator */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_jr1(unit,
                                                                                            label_mode,
                                                                                            func_config,
                                                                                            params,
                                                                                            inst_labels,
                                                                                            next_instruction_index,
                                                                                            first_instruction));
    }
    else
    {
        SHR_IF_ERR_EXIT(oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_jr2(unit,
                                                                                            label_mode,
                                                                                            func_config,
                                                                                            params,
                                                                                            inst_labels,
                                                                                            next_instruction_index,
                                                                                            first_instruction));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  oamp_pe_function_up_mep_server_maid_48 function loading.
*  The function is support Jericho2 and Inter-op mode(Jericho1)..
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/

static shr_error_e
oamp_pe_function_up_mep_server_maid_48(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** set PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        /** copy 48B from packet into FDBK fifo */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** skip 32B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));

        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            /** Set PTCH2(outer)*/
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

            /** Build ITMH Jer1 */
            INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_DP_JR1, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_0, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        }
        else
        {
            /** Set PTCH2(outer) + 1MSB(bits 32-39) byte of ITMH*/
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x3, ALWAYS_NEVER, NXT_JMP));

            /** Build ITMH Jr2 */
            INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_DP, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_0, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        }

        INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_TC, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_1, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_copy_48b_maid_program */
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
               /** NOPs because of the Jump*/
               INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
               INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


/**
* \brief
*  oamp_pe_function_up_mep_server function loading.
*  The function is support Jericho2 and Inter-op mode(Jericho1)..
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/

static shr_error_e
oamp_pe_function_up_mep_server(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            /** Set PTCH2(outer)*/
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

            /** Build ITMH Jer1 */
            INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_DP_JR1, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_0, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        }
        else
        {
            /** Set PTCH2(outer) + 1MSB(bits 32-39) byte of ITMH*/
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x3, ALWAYS_NEVER, NXT_JMP));

            /** Build ITMH Jr2 */
            INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_DP, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_0, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        }

        INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_TC, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_1, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  OAM DM JUMBO tlv function loading.
*  The function is to build Jumbo DM frames
*  step1: calculate copied length of different format accroding to packet length
*        UP: PTCHoETHoDMM/R, size=57; Down: PTCHoITMHoETHoDMM/R, size=68; MPLS: PTCHoITMHoPPHoLSPoGALoACHoDMM/R, size=78; PWE:  PTCHoITMHoPPHoPWEoACHoDMM/R, size=74;
*  step2: copy packet except endTlv
*  step3: calculate Gen mem index to get TLV data , includes tlv length, and tlv data.
*  step4: copy TLV data
*  step5: copy endTlv
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_function_dm_jumbo_tlv(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** Store (PKT_LNTH - 4 - 8) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(PKT_LNTH, INST_VAR, ALU_OUT, ALWAYS, NEVER, TMP_REG1), CMN(NONE, NONE, 12, NEVER_NEVER, NXT_JMP));/*LOP_NXT*/

        /**Store PKT_LNTH & 3 in TMP_REG2, which use to  to align back  if the mep is up/down/mpls */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(PKT_LNTH, INST_VAR, ALU_OUT, ALWAYS, NEVER, TMP_REG2), CMN(NONE, NONE, 3, NEVER_NEVER, NXT_JMP));
        /** set temp_reg4=2*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 2, NEVER_NEVER, NXT_JMP));
        /** set temp_reg3=1*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, NXT_JMP));
        /** copy system header + pdu (offset + actually 32) from fifo,  copy length from reg1  */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));

        /**
         * UP:   PTCHoETHoDMM/R ,57%4 remainder =1, just for endtlv, on more copy
         * Down: PTCHoITMHoETHoDMM/R , 68%4, remainder =0,  endtlv needs 1 bytes, 3 bytes copy needed.
         * MPLS: PTCHoITMHoPPHoLSPoGALoACHoDMM/R, 78%4 remainder =2,  endtlv needs 1 bytes, 1 byte copy needed.
         * PWE:  PTCHoITMHoPPHoPWEoACHoDMM/R, 74%4 remainder =2,  endtlv needs 1 bytes, 1 byte copy needed.
        */
        /** if reg 2 is equal to 2, jump to  MPLS, 1'B COPY  */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_MPLS_COPY], NEVER_NEVER, NXT_JMP));
        /** if reg 2 is equal to 1, jump to  UP, no more COPY  */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_UP_COPY], NEVER_NEVER, NXT_JMP));

            /** NOPs */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** DOWN COPY Jump to here*/
        /** Down default to here*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /**MPLS/PWE COPY Jump to here*/
        /** copy 1B from IN_FIFO */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x1, ALWAYS_NEVER, NXT_JMP), (DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_MPLS_COPY ));

        /**UP COPY  Jump to here*/
        /* copy TLV type==3 to buff */
        INSTR_LABEL(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 3, ALWAYS_NEVER, NXT_JMP), (DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_UP_COPY));

        /** set temp_reg1 ==  PRG_VAR of rsp_program */
        INSTR(CE1_NOP, CE2_NOP,ALU_SUB(PRG_VAR, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_RSP], NEVER_NEVER, NXT_JMP));
            /** NOPs */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /**
         * mep_id save to reg1 , using FEM and shitf to get mep_id/16
         * mep_id to get gen mem (mep_id /4/16 ) and store in TMP_REG4 the memory offset where tlv length is resides, reg4 use for gen mem access
         * for responsor to get mep id
         */
        /** generator to get mep_id */
        INSTR(CE1_NOP, CE2_COPY(NS_16_VAR_0, TLV_DATA_OFFSET , INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        /**  jump to common process*/
        INSTR(CE1_NOP, CE2_NOP,ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_COMMON], NEVER_NEVER, NXT_JMP));
            /** NOPs */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** responsor to get mep_id */
        INSTR_LABEL(CE1_NOP, CE2_COPY(NS_16_VAR_0,  TLV_DATA_OFFSET_RSP, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP), (DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_RSP));

        /** common process */
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP, (DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_COMMON));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            /** mep_id/16   + 256 save to reg4, it's index of gen mem (LSB in gen_mem )*/
            INSTR(CE1_NOP, CE2_NOP, ALU_ADD(TMP_REG4, INST_VAR, ALU_OUT, ALWAYS, NEVER, TMP_REG4), CMN(NONE, NONE, 256, NEVER_NEVER, NXT_JMP));
                /** mep_id/16   + 512 save to reg4, it's index of gen mem (MSB in gen_mem )*/
                INSTR(CE1_NOP, CE2_NOP, ALU_ADD(TMP_REG4, INST_VAR, ALU_OUT, ALWAYS, NEVER, TMP_REG4), CMN(NONE, NONE, 512, NEVER_NEVER, NXT_JMP));

        /** read   tlv length of data from MEM and store it to reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(MEM_OUT, TMP_REG1), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
            /** read LSB of data from MEM and store it to reg3 */
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(MEM_OUT, TMP_REG3), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
                /* read MSB from MEM and store it to reg2 */
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(MEM_OUT, TMP_REG2), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        /** NOPs */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** copy tlv length to buffer  ( from reg1 )*/
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /*Start to tlv loop, read tlv data from reg2 + reg3*/
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, VLD_B, CONCATENATE), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B), ALU_SUB(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, NPOS, TMP_REG1), CMN(NONE, NONE, 12, ALWAYS_NEVER, LOP_NXT));

        /** copy rest packet: end TLV*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0x1, ALWAYS_CMP2, LOP_NXT));

   }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Single hop BFD random dest ip function loading
*  The function is to build PPH and remove LSP label for single hop BFD packet
*  step1:  Copy 26B {PTCH, ITMH, ITMH-EXT, MSB 11bytes PPH }
*  step2: Update LSB 1st byte of PPH
*  step3: Skip 4B LSP label
*  step4: Copy the reset of packet
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_function_bfd_single_hop_random_dip(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    /** Needed for setting PPH.TTL */
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    /** Differntiation between JR1 and JR2 modes */
    uint8 oamp_tx_format, jr1_format;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** Find out which format is used: JR1 or JR2 */
        oamp_tx_format = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        jr1_format = dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit);

        if(oamp_tx_format == jr1_format)
        {
            /** copy 20B {PTCH, ITMH, ITMH-EXT, MSB 11bytes PPH } */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 5, ALWAYS_NEVER, LOP_CST));
        }
        else
        {
            /** Calculate the parts of PPH.TTL */
            pph_ttl = 0xFF;
            pph_ttl_msb = pph_ttl >> 5;
            pph_ttl_lsb = (pph_ttl << 3) & 0xFF;


            /** copy 16B {PTCH, ITMH, ITMH-EXT, MSB 6bytes PPH } */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

            /** copy next two bytes */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
            /** Copy 1 byte to tmp_reg1 */
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));
            /** Copy 1 byte to tmp_reg2 */
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
            /** Write 3 PPH.TTL bits to tmp_reg1 */
            INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));
            /** Write 5 PPH.TTL bits to tmp_reg2 */
            INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));
            /** NOP for the value to be ready */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            /** Copy result from TMP_REG1 */
            INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
            /** Copy result from TMP_REG2 */
            INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
        }

        /** copy 1B PPH && remove 3B {1b'LSBof pph, udh, 1b'MSB of label} */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 1, ALWAYS_NEVER, NXT_JMP));

        /** Update LSB 1st byte of PPH, includes Parsing-Start-Type(TxPphParsingStartTypeIpv4= 0x2) */
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 5, ALWAYS_NEVER, NXT_JMP));

        /** build UDH */
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        /** remove 3b'LSB of label*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
   }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  MPLS-TP vccv type BFD program.
*  The function is to build MPLS-TP vccv type BFD packet
*  step1:  Copy PTCH + ITMH+PPH+LSP
*  step2: Insert PW-ACH (0x10000021)
*  step3: Copy the reset
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_function_bfd_mplstp_vccv(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** copy 12B' header(ptch(2) + itmh(5) +itmh-ext(3) + pph MSB2Byte*/
        /** copy 8B (2b'ptch + 5'itmh + 1b'ext) */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            /**  copy 4B and set FAI to 5 with FEM*/
            INSTR(CE1_COPY(IN_FIFO, VCCV_BFD, VLD_B, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0,  ALWAYS_NEVER, NXT_JMP));

        /** copy 12bytes, rest of 10b'pph  + 1b' udh + 1b' lsp*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, LOP_CST));
        /**Copylsb 3B of LSP && remove 1b'*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

        /** write the 4 byte of PW-ACH from prog. variable*/
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        /** set 1b' bytes of IP header*/
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x45, ALWAYS_NEVER, NXT_JMP));
        /**  copy 7 bytes && remove 1b' ttl  */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
        /** copy TTL 255  to buff */
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 255, ALWAYS_NEVER, NXT_JMP));
        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

   }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  oamp_pe_function_lmdm_flexible_da function loading.
*  The function gets flexible DMAC from extra data (80 bytes prefix of packet) and then replace the original DMAC with this flexible DMAC
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/

static shr_error_e
oamp_pe_function_lmdm_flexible_da(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** copy 8B from packet into FDBK fifo */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 0, NEVER_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 0, NEVER_NEVER, NXT_JMP));
        /** skip 72B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 18, NEVER_NEVER, LOP_CST));

        /** Store 0x80 in in MSB of tmp-reg2,  used to check down or up MEP */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, NXT_JMP));

        /** Copy 2 bytes of PTCH-2 header to buffer, and store this first 2 bytes in tmp-reg3 */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_COPY(IN_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG3),CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
            /** NOP(Because of tmp-reg3)*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));

        /** Check the packet for up or down MEP.
          * In case of packet for up MEP, jump to label DNX_OAMP_PE_INST_LABEL_FLEXIBLE_DA_UP_MEP.
          * In case of packet for down MEP continue. */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG2, INST_VAR, NEVER, POS, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_FLEXIBLE_DA_UP_MEP], NEVER_NEVER, NXT_JMP));
            /** NOPs because of the Jump */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** For down MEP.
          * 1) copy the first 2 bytes of ITMH header to buffer.
          * 2) copy the last 3 bytes of ITMH header and the first byte of OMTS header to buffer.
          * 3) copy the 2nd--5th bytes of OMTS header to buffer.
          * 4) copy the 6th byte of OMTS header to buffer, and drop the first 3 bytes of old DMAC. */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x01, ALWAYS_NEVER, NXT_JMP));;

        /** copy 6 byte DMAC from FDBK fifo */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0x02, ALWAYS_NEVER, NXT_JMP));

        /** drop the last 3 bytes of old DMAC, copy the first byte of SMAC to buffer. */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /** For up MEP.
          * 1) copy 6 byte DMAC from FDBK fifo
          * 2) drop the 6 bytes of old DMAC. */
        INSTR_LABEL(CE1_COPY(FDBK_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_FLEXIBLE_DA_UP_MEP);
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0x02, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
   }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


/**
* \brief
*  BFDoIPv6 program loading.
*  The program gets the packet with prefix of 80B that are not sent out.
*  Need to update the PPH field FWD_CODE to be 3 TxPphParsingStartTypeIpv6
*  Need to place first 40B of prefix instead of the ones in packet starting from system headers (IPv6).
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dictates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_bfd_ipv6_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr, instr_const;
    int udh_header_size = 0;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;
    uint32 saved_prog_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {

        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));
        /*
         * configure set of requested instructions
         *
         * The program uses additional data(80 bytes) that comes before the packet.
         * The program stores 40 bytes in feedback fifo and the rest 40 are dropped.
         * The program fills IPv6 header with 40 bytes from the additional data and set the PPH field FWD_CODE.
         *
         * The packets that are supported by the program are (UDH of different size can be added to each packet):
         * INPUT
         * BFDoIPv4:  BFD(24B) o UDP(8B) o IPv4(20B) o [UDH(0-17B)] o PPH(7B) o ITMH-ext(3B) o ITMH(4B) o PTCH-2(2B)
         * OUTPUT
         * BFDoIPv6:  BFD(24B) o UDP(8B) o IPv6(40B) o [UDH(0-17B)] o PPH(7B) o ITMH-ext(3B) o ITMH(4B) o PTCH-2(2B)
         */

        /*
         * Copy 40B from additional data to buffer.
         * Drop 40B garbage.
         */
        /* *INDENT-OFF* */
        /** temp_reg1=0x20 used as IPv6 payload length */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        /** temp_reg4=0x0 used as IPv6 flow label */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        /** temp_reg3=3 used to mask the ls two bits of the FWD_CODE field of the PPH */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 3, NEVER_NEVER, NXT_JMP));
        /** copy 40B from addition data into FDBK fifo (IPV6 header)*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 10, NEVER_NEVER, LOP_CST));
        /** drop 40B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 10, NEVER_NEVER, LOP_CST));
        /** copy system headers till the first 1B of PPH header(PTCH(2B)+ITMH(4B)+EXT(3B)-1 )*/

        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        /** copy 2B from in_fifo and mask the ls two bits of the FWD_CODE field of the PPH */
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, OR), CE2_COPY(IN_FIFO, DEFAULT, NONE, 2B), ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** copy 2B(PPH) shifted with 2 and drop 2B of the 1b EXT and 1B PPH(already copied)*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        /** copy 4B last 4 of the (PPH)*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        /**handle UDH misalignment*/
        while (udh_header_size >= 4)
        {
            /** copy 4B(UDH)*/
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            udh_header_size -= 4;
        }

        switch (udh_header_size)
        {
            case 0:
                instr_const = 0;
                break;
            case 1:
                instr_const = 1;
                break;
            case 2:
                instr_const = 2;
                break;
            case 3:
                instr_const = 3;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }
        /**Copy
         * 0B UDH and drop first 4B of IPv4
         * 1B UDH and drop first 3B of IPv4
         * 2B UDH and drop first 2B of IPv4
         * 3B UDH and drop first 1B of IPv4
         */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        /*
         * Copy IPv6 from FDBF and drop IPv4.
         */
        /** drop 16B IPv4 */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));

        /*
         * Copy IPv6 from FDBF and drop IPv4.
         */

        /* copy first 2B from FDBK fifo (first 2B of IPv6 header) setting the is_echo flag to 0
         * and store these bytes to tmp_reg3 (the ls bit of these bytes is the is_echo flag)
         */
        INSTR(CE1_COPY(FDBK_FIFO, BFD_IPV6_ECHO, INST_CONST, INST_CONST, FEM), CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG3), CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        /** copy 2B from TMP_REG4 (flow label) */
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        /** copy the udp source port (which is saved in the flow label' field to TMP_REG4) */
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, READ, 0x22, NEVER_NEVER, NXT_JMP));

        /** copy 2 bytes from TMP_REG1 (ipv6 payload length) and 2 bytes from FDBK_FIFO (next header and hop limit) */
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, CONCATENATE),
            CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        /** Get the bit that determines seamless BFD */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 2, NEVER_NEVER, LOP_NXT));

        /** copy the UDP checksum (which is saved in the UDP length field) to TMP_REG1 */
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG1), CMN(NONE, READ, 0, NEVER_NEVER, NXT_JMP));

        /** copy the last 32B of the IPv6 header (SIP and DIP) from the FDBK_FIFO */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 8, ALWAYS_NEVER, LOP_CST));

        /** Check if the temp_reg2==0 (not seamless BFD.)  If so, jump*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_SBFD], NEVER_NEVER, NXT_JMP));

        /** tmp_reg2=0x1 used to extract the is_echo flag from tmp_reg2 */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG2), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Set Source Port and Destination Port */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        /** drop 4B  */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

        switch (udh_header_size)
        {
            case 0:
                /** drop 3B */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

                /** copy 1 byte from UDP and 2 bytes from TMP_REG1 (UDP checksum) */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG1, DEFAULT, NONE, 2B),
                    ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                /** Drop two more bytes - UDP checksum */
                /* INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP)); */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** Copy 4 more bytes */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
                break;

            case 1:
                /** copy 2B. drop 1 */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x12, ALWAYS_NEVER, NXT_JMP));

                /** copy 2 bytes from TMP_REG1 (UDP checksum) */
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                /** copy 3B. Drop 1 */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));

                break;

            case 2:
                /** copy 2B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

                /** copy 2 bytes from TMP_REG1 (UDP checksum) */
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                /** copy 2B. Drop 2 */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

                break;

            case 3:
            default:
                /** copy 1B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

                /** copy 1 byte from UDP and 2 bytes from TMP_REG1 (UDP checksum) */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG1, DEFAULT, NONE, 2B),
                    ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                /** copy 1B. Drop 3 */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;
        }

        /** Unconditional jump - skip to discrimimator MSB */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG4, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_ECHO], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /* check if the is_echo flag is off */
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG2),
                CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO], NEVER_NEVER, NXT_JMP),
                DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_SBFD);

            /* the next two commands are executed anyway, no matter what is the ALU_AND result */

            /* copy 2B from TMP_REG4 (UDP source port) */
            INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));


            /** copy the UDP dest_port of the original packet to TMP_REG4 */
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

        /* copy the UDP dest port of echo */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT , NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /* copy the UDP length */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT , INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        /* jump to skip the case of !is_echo */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_ECHO], NEVER_NEVER, NXT_JMP));

            /* the next two commands are executed anyway, no matter what is the ALU_AND result */
            /** copy the UDP checksum from TMP_REG1 and remove 4 bytes of UDP from the IN_FIFO */
            INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));

            if (udh_header_size != 0) {
                /* remove 4 more bytes from in fifo to be aligned with the !is_echo case */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
            } else {
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            }

        switch (udh_header_size)
        {
            case 0:
                /** copy 2 bytes of UDP (bytes 2-3) */
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO);
                /** write bytes 4-5 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
                /** copy the UDP checksum from TMP_REG1 and remove 4 last bytes of UDP from the IN_FIFO */
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                /** copy 4 more bytes before checking the discr MSB issue */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
                break;
            case 1:
                /** shift 3B write 1B*/
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO);
                /** write bytes 3-5 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x3, ALWAYS_NEVER, NXT_JMP));
                /** copy the UDP checksum from TMP_REG1 and remove 4 last bytes of UDP from the IN_FIFO */
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                /** copy 3 bytes of BFD shifted by 1 byte */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                break;
            case 2:
                /** drop 4 bytes (2 of IPv4 and 2 of UDP)*/
                INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO);
                /** copy bytes 2-5 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
                /** copy the UDP checksum from TMP_REG1 */
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                /** copy 2 bytes of BFD shifted by 2 bytes */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;
            case 3:
            default:
                /** drop 4 bytes (3 of IPv4 and 1 of UDP)*/
                INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO);
                /** copy bytes 2-4 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                /** copy byte 5 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));
                /** copy the UDP checksum from TMP_REG1 */
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                /** copy 1 byte of BFD shifted by 3 bytes */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;
        }

        /*
         * COPY PDU
         */

        /** BFD MEP type discriminator feature */

        /** Get the bit that determines if discr MSB=1 */
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG4), CMN(NONE, NONE, 0x1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_ECHO);

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Check if the temp_reg4==0, if so jump*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG4, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_bfd_discr_msb_1_function */
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
            /** NOPs because of the Jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }

        /* *INDENT-ON* */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  BFDoIPv6 program loading.
*  The program gets the packet with prefix of 80B that are not sent out.
*  Need to update the PPH field Parsing-Start-Type to be 3 TxPphParsingStartTypeIpv6
*  Need to place first 40B of prefix instead of the ones in packet starting from system headers (IPv6).
*  The UDP checksum is stored in the IPv6 'payload length' field
*  So, when copying the IPv6 header, we replace that field's content to 32 (UDP header's length (8) + BFD header's length (24))
*  When copying the UDP header from the original packet, we replace the checksum with the value which was
*     stored in the IPv6 'payload length' field
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dictates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_bfd_ipv6_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;
    uint32 saved_prog_index;

    /** Needed for setting PPH.TTL */
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /*
         * configure set of requested instructions
         */
        /*
         * The program uses additional data(80 bytes) that comes before the packet.
         * The program stores 48 bytes in feedback fifo and the rest 32 are dropped.
         * The program fill IPv6  and UDP headers with 48 bytes from the additional data and set the PPH field Parsing-Start-Type field.
         *
         * The packets that are supported by the program are (UDH of different size can be added to each packet):
         * INPUT
         * BFDoIPv4:  BFD(24B) o UDP(8B) o IPv4(20B) o [UDH(0-17B)] o PPH(12B) o ITMH-ext(3B) o ITMH(5B) o PTCH-2(2B)
         * OUTPUT
         * BFDoIPv6:  BFD(24B) o UDP(8B) o IPv6(40B) o [UDH(0-17B)] o PPH(12B) o ITMH-ext(3B) o ITMH(5B) o PTCH-2(2B)
         */

        /** Calculate the parts of PPH.TTL */
        pph_ttl = 0xFF;
        pph_ttl_msb = pph_ttl >> 5;
        pph_ttl_lsb = (pph_ttl << 3) & 0xFF;

        /*
         * Copy 40B from additional data to buffer.
         * Drop 40B garbage.
         */
        /* *INDENT-OFF* */
        /** temp_reg4=0x0 used as IPv6 flow label */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        /** temp_reg3=6 used to mask the ls two bits of the Parsing-Start-Type field of the PPH */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 6, NEVER_NEVER, NXT_JMP));
        /** copy 40B from addition data into FDBK fifo (40B IPV6)*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 10, NEVER_NEVER, LOP_CST));
        /** drop 40B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 10, NEVER_NEVER, LOP_CST));
        /** copy system headers till the last 2B of PPH header(PTCH(2B)+ITMH(5B)+EXT(3B)+PPH(12B)-6 )*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));
        /** copy next two bytes */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** Copy 1 byte to tmp_reg1 */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));
        /** Copy 1 byte to tmp_reg2 */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        /** Write 3 PPH.TTL bits to tmp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));
        /** Write 5 PPH.TTL bits to tmp_reg2 */
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));
        /** NOP for the value to be ready */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        /** Copy result from TMP_REG1 */
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
        /** Copy result from TMP_REG2 */
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
        /** temp_reg1=0x20 used as IPv6 payload length */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

        /** mask the ls two bits of the Parsing-Start-Type field of the PPH */
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, OR), CE2_COPY(IN_FIFO, DEFAULT, NONE, 2B), ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /** copy 1B(udh) shifted with 2 and drop 2B PPH(copied) 1B UDH and first byte of the IPv4*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, NONE), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));

        /*
         * Copy IPv6 from FDBF and drop IPv4.
         */


        /* copy first 2B from FDBK fifo (first 2B of IPv6 header) setting the is_echo flag to 0
         * and store these bytes to tmp_reg2 (the ls bit of these bytes is the is_echo flag)
         */
        INSTR(CE1_COPY(FDBK_FIFO, BFD_IPV6_ECHO, INST_CONST, INST_CONST, FEM), CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        /** copy 2B from TMP_REG4 (flow label) */
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        /** copy the udp source port (which is saved in the flow label' field to TMP_REG4) */
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, READ, 0x22, NEVER_NEVER, NXT_JMP));

        /** copy 2 bytes from TMP_REG1 (ipv6 payload length) and 2 bytes from FDBK_FIFO (next header and hop limit) */
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, CONCATENATE),
            CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        /** Get the bit that determines seamless BFD */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 2, NEVER_NEVER, LOP_NXT));

        /** copy the UDP checksum (which is saved in the UDP length field) to TMP_REG1 */
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG1), CMN(NONE, READ, 0, NEVER_NEVER, NXT_JMP));

        /** copy the last 32B of the IPv6 header (SIP and DIP) from the FDBK_FIFO */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 8, ALWAYS_NEVER, LOP_CST));

        /** drop 16B IPv4*/
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));

        /** Check if the temp_reg3==0 (not seamless BFD.)  If so, jump*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_SBFD], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Set Source Port and Destination Port */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        /** drop 4B: non-seamless Source and destination port, and 3 more for alignment */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

        /** Get the bit that determines if discr MSB=1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT));

        /** copy 1 byte from UDP and 2 bytes from TMP_REG1 (UDP checksum) */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG1, DEFAULT, NONE, 2B),
            ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

        /** Drop two more bytes - UDP checksum */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        /** Unconditional jump - skip to discrimimator MSB */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG4, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_DISCR_MSB], NEVER_NEVER, NXT_JMP));

        /** tmp_reg3=0x1 used to extract the is_echo flag from tmp_reg2 */
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_SBFD);

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /* check if the is_echo flag is off */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO], NEVER_NEVER, NXT_JMP));

        /* the next two commands are executed anyway, no matter what is the ALU_AND result */

            /* copy 2B from TMP_REG4 (UDP source port) */
            INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));


            /** copy the UDP dest_port of the original packet to TMP_REG4 */

            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));


        /*
         *   if (is_echo) {
         *     TMP_REG4 = 0xec9; (ECHO UDP destination port)
         *   }
         */
        INSTR(CE1_NOP, CE2_COPY(PROG_VAR, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));

        /** Get the bit that determines if discr MSB=1 */
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO );

        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /*  copy UDP dest port (bytes 2-3) */
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        /** copy 1 byte of UDP (byte 4) */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        /** copy 1 byte from UDP and 2 bytes from TMP_REG1 (UDP checksum) */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG1, DEFAULT, NONE, 2B),
            ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

        /** copy the first byte of the BFD header (shifted by 3 bytes) */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        /*
         * COPY PDU
         */

        /** BFD MEP type discriminator feature */

        /** Check if the temp_reg2==0, if so jump*/
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP),DNX_OAMP_PE_INST_LABEL_BFD_IPV6_DISCR_MSB);

        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_bfd_discr_msb_1_function */
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
                /** NOPs because of the Jump*/
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }

        /* *INDENT-ON* */
        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  BFDoIPv6oMPLS program loading.
 *  The program gets the packet with prefix of 80B that are not sent out.
 *  Need to update the LS 4 bytes of IPv6 header
 *  Need to place first 40B of prefix instead of the ones in packet starting from system headers (IPv6 header).
 *   \param [in] unit  -  Relevant unit.
 *   \param [in] label_mode  -  if set in dictates that need to go through instructions without setting to tables to map labels to 'inst_labels'
 *   \param [in] program_id  -  requested program to be configured
 *   \param [in] program_const  -  const value related to the requested program
 *   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
 *   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
 *   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to define the programs
 *   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
 *   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
 * \return
 *   shr_error_e
 */
static shr_error_e
oamp_pe_bfd_ipv6_mpls_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    int instr_const;
    int udh_header_size = 0;
    uint32 saved_prog_index;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));
        /*
         * configure set of requested instructions
         */
        /*
         * The program uses additional data(80 bytes) that comes before the packet.
         * The program stores 40 bytes in feedback fifo and the rest 40 are dropped.
         * The program fill IPv6 header with 40 bytes from the additional data and the LS 4 bytes of the IPv6 header are updated.
         *
         * The packets that are supported by the program are (UDH of different size can be added to each packet):
         * INPUT
         * BFDoIPv4:  BFD(24B) o UDP(8B) o IPv4(20B) o MPLS(4B) o [UDH(0-16B)] o PPH(7B) o ITMH-ext(3B) o ITMH(4B) o PTCH-2(2B)
         * OUTPUT
         * BFDoIPv6:  BFD(24B) o UDP(8B) o IPv6(40B) o MPLS(4B) o[UDH(0-16B)] o PPH(7B) o ITMH-ext(3B) o ITMH(4B) o PTCH-2(2B)
         *
         */
        /* *INDENT-OFF* */


        /** temp_reg1=0x20 used as IPv6 payload length */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        /** temp_reg4=0x0 used as IPv6 flow label */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        /** copy 40B from packet into FDBK fifo (IPV6 header)*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 10, NEVER_NEVER, LOP_CST));
        /** drop 40B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 10, NEVER_NEVER, LOP_CST));

        /** copy system headers till the MPLS header(PTCH(2B)+ITMH(4B)+EXT(3B)+PPH(7B)+(4B(UDH) or part of MPLS(4B) in case UDH < 4B) )*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 5, ALWAYS_NEVER, LOP_CST));
        /*In case the UDH size is higher than 4*/
        while (udh_header_size >= 4)
        {
            /** copy 4B(4B(UDH) or part of MPLS(4B) in case UDH < 4B) */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            udh_header_size -= 4;
        }
        /** store 0xFF to tmp_reg1 used to mask the IPv4 LSB*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG1), CMN(NONE, NONE, 0xFF, NEVER_NEVER, NXT_JMP));

        switch (udh_header_size)
        {
            case 0:
                /** CASE 0B misalignment*/
                instr_const = 0;
                break;
            case 1:
                /** CASE 1B misalignment*/
                instr_const = 1;
                break;
            case 2:
                /**CASE 2B misalignment*/
                instr_const = 2;
                break;
            case 3:
            default:
                /** CASE 3B misalignment*/
                instr_const = 3;
                break;
        }
        /**Copy
         * 0B MPLS and drop first 4B of IPv4
         * 1B MPLS and drop first 3B of IPv4
         * 2B MPLS and drop first 2B of IPv4
         * 3B MPLS and drop first 1B of IPv4
         */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        /** drop 12B of IPv4 */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, NEVER_NEVER, LOP_CST));

        if (udh_header_size != 0) {
            /** drop 4B of IPv4 */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
        }

        switch (udh_header_size)
        {
            case 0:
                /*IN_FIFO: Last 4B of IPv4**/
                instr_const = 0x30;
                break;
            case 1:
                /*IN_FIFO: Last 1B of IPv4 and 3B UDP**/
                instr_const = 0x0;
                break;
            case 2:
                /*IN_FIFO: Last 2B of IPv4 and 2B UDP**/
                instr_const = 0x10;
                break;
            case 3:
            default:
                /*IN_FIFO: Last 3B of IPv4 and 1B UDP**/
                instr_const = 0x20;
                break;
        }

        /** store the LSB of the ipv4 header to tmp_reg2*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, instr_const, NEVER_NEVER, NXT_JMP));
        /** MASK the LSB of IPv4 temp_reg2 and store it in tmp_reg2 */
        /*TMP_REG1 = 0xFF00, TMP_REG2= (LSB of IPv4 and MSB of UDP)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));

        /** copy first 2B from FDBK fifo (first 2B of IPv6 header) */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        /** copy 2B from TMP_REG4 (flow label) */
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        /** copy the udp source port (which is saved in the flow label' field to TMP_REG4) */
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, READ, 0x22, NEVER_NEVER, NXT_JMP));

        /** copy 2 bytes from TMP_REG3 (ipv6 payload length) and 2 bytes from FDBK_FIFO (next header and hop limit) */
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, CONCATENATE),
            CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        /** copy the UDP checksum (which is saved in the UDP length field) to TMP_REG3 */
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG3), CMN(NONE, READ, 0, NEVER_NEVER, NXT_JMP));

        /** copy 28B of the IPv6 (16B SIP + 12 first bytes of DIP) header from the FDBK_FIFO */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 7, ALWAYS_NEVER, LOP_CST));

        /** Store one's complement of LSB of IPv4 temp_reg2 and store it in tmp_reg1 */
        /*TMP_REG1 = 0xFF00, TMP_REG2= (LSB of IPv4 and 1B 0x00)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));

        /** copy 1B 0x7f to buff(First 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x7F, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B one's compliment of the LSB of IPv4 to buff(Second 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B 0x00 to buff(Third 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B the LSB of IPv4 to buff(Forth 1B of the last 4 of the IPv6 header) and drop last 4B of IPv6 header*/
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP));

        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Get the bit that determines seamless BFD */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 4, NEVER_NEVER, LOP_NXT));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Check if the temp_reg2==0 (not seamless BFD.)  If so, jump */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Seamless BFD */

        /** Set Source Port and Destination Port */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        /** Unconditional jump - copy rest of packet */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2),
                CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD], NEVER_NEVER, NXT_JMP));

        switch (udh_header_size)
        {
            case 0:

                    /** Instructions before non-conditional jump */

                    /** drop 7B: non-seamless Source and destination port, and 3 lasts bytes of IPv4 */
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

                /** Non-SBFD instructions */

                /** drop 4 bytes of IPv4 */
                INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD);
                /* copy 2B from TMP_REG4 (UDP source port) */
                INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
                /** shift 2B write 2B (bytes 2-3 of UDP) */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                /** write byte 4 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));

                /** For SBFD, jump here */

                /** write byte 5 of UDP */
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD);
                /** copy the UDP checksum from TMP_REG3 and remove 4 last bytes of UDP from the IN_FIFO */
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                break;
            case 1:

                    /** Instructions before non-conditional jump */

                    /** Drop 4B */
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** Non-SBFD instructions */

                /* copy 2B from TMP_REG4 (UDP source port) */
                INSTR_LABEL(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD);
                /** shift 3B write 1B*/
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                /** write byte 3 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));

                /** For SBFD, jump here */

                /** write byte 4-5 of UDP */
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x12, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD);
                /** copy the UDP checksum from TMP_REG3 and remove 4 last bytes of UDP from the IN_FIFO */
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                /** copy 3 bytes of BFD shifted by 1 byte */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                break;
            case 2:
                    /** Instructions before non-conditional jump */

                    /** Drop 4B */
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** Non-SBFD instructions */

                /* copy 2B from TMP_REG4 (UDP source port) */
                INSTR_LABEL(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD);
                /** copy bytes 2-3 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

                /** For SBFD, jump here */

                /** copy bytes 4-5 of UDP */
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD);
                /** copy the UDP checksum from TMP_REG3 */
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                /** copy 2 bytes of BFD shifted by 2 bytes */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;
            case 3:
            default:

                    /** Instructions before non-conditional jump */

                    /** Drop 4B */
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** Non-SBFD instructions */

                /* copy 2B from TMP_REG4 (UDP source port) */
                INSTR_LABEL(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD);
                /** copy bytes 2-3 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x12, ALWAYS_NEVER, NXT_JMP));

                /** For SBFD, jump here */

                /** copy byte 4 of UDP */
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD);
                /** copy byte 5 of UDP */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));
                /** copy the UDP checksum from TMP_REG3 */
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                /** copy 1 byte of BFD shifted by 3 bytes */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;
         }

        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /* *INDENT-ON* */

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  BFDoIPv6oMPLS program loading.
 *  The program gets the packet with prefix of 80B that are not sent out.
 *  Need to update the LS 4 bytes of IPv6 header
 *  Need to place first 40B of prefix instead of the ones in packet starting from system headers (IPv6).
 *   \param [in] unit  -  Relevant unit.
 *   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
 *   \param [in] program_id  -  requested program to be configured
 *   \param [in] program_const  -  const value related to the requested program
 *   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
 *   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
 *   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
 *   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
 *   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
 * \return
 *   shr_error_e
 */
static shr_error_e
oamp_pe_bfd_ipv6_mpls_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    /** Needed for setting PPH.TTL */
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** Calculate the parts of PPH.TTL */
        pph_ttl = 0xFF;
        pph_ttl_msb = pph_ttl >> 5;
        pph_ttl_lsb = (pph_ttl << 3) & 0xFF;

        /*
         * configure set of requested instructions
         */
        /*
         * The program uses additional data(80 bytes) that comes before the packet.
         * The program stores 40 bytes in feedback fifo and the rest 40 are dropped.
         * The program fill IPv6 header with 40 bytes from the additional data and the LS 4 bytes are updated.
         *
         * The packets that are supported by the program are (UDH of different size can be added to each packet):
         * INPUT
         * BFDoIPv4:  BFD(24B) o UDP(8B) o IPv4(20B) o MPLS(4B) o UDH(1B) o PPH(12B) o ITMH-ext(3B) o ITMH(5B) o PTCH-2(2B)
         * OUTPUT
         * BFDoIPv6:  BFD(24B) o UDP(8B) o IPv6(40B) o MPLS(4B) o UDH(1B) o PPH(12B) o ITMH-ext(3B) o ITMH(5B) o PTCH-2(2B)
         *
         */
        /* *INDENT-OFF* */
        /** temp_reg3=0x20 used as IPv6 payload length */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        /** temp_reg4=0x0 used as IPv6 flow label */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        /** copy 40B from packet into FDBK fifo (40B IPV6)*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 10, NEVER_NEVER, LOP_CST));
        /** drop 40B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 10, NEVER_NEVER, LOP_CST));

        /** copy whole dwords until PPH TTL {PTCH, ITMH, ITMH-EXT, first six bytes of PPH} */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

        /** copy next two bytes */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /** Copy 1 byte to tmp_reg1 */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

        /** Copy 1 byte to tmp_reg2 */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

        /** Get the bit that determines if discr MSB=1 */
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO );

        /** Write 3 PPH.TTL bits to tmp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));

        /** Write 5 PPH.TTL bits to tmp_reg2 */
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));

            /** NOP for the value to be ready */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Copy result from TMP_REG1 */
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        /** Copy result from TMP_REG2 */
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        /** copy 4 more bytes: (PPH(last 2B)+UDH(1B)+ first byte of MPLS)*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

        /** store 0xFF to tmp_reg1 used to mask the IPv4 LSB*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG1), CMN(NONE, NONE, 0xFF, NEVER_NEVER, NXT_JMP));

        /*
         * copy 3B(MPLS) and drop first 1B of the IPv4
         */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
        /* drop 16B IPv4 till the last 3B of the IPv4 */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));

        /** store the LSB of the ipv4 header to tmp_reg2*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

        /** MASK the LSB of IPv4 temp_reg2 and store it in tmp_reg2 */
        /*TMP_REG1 = 0xFF00, TMP_REG2= (LSB of IPv4 and MSB of UDP)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));


        /** copy first 2B from FDBK fifo (first 2B of IPv6 header) */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        /** copy 2B from TMP_REG4 (flow label) */
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        /** copy the udp source port (which is saved in the flow label' field to TMP_REG4) */
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, READ, 0x22, NEVER_NEVER, NXT_JMP));

        /** copy 2 bytes from TMP_REG3 (ipv6 payload length) and 2 bytes from FDBK_FIFO (next header and hop limit) */
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, CONCATENATE),
            CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        /** copy the UDP checksum (which is saved in the UDP length field) to TMP_REG3 */
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG3), CMN(NONE, READ, 0, NEVER_NEVER, NXT_JMP));

        /** copy 28B of the IPv6 (16B SIP + 12 first bytes of DIP) header from the FDBK_FIFO */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 7, ALWAYS_NEVER, LOP_CST));

        /** Store one's compliment of LSB of IPv4 temp_reg2 and store it in tmp_reg1 */
        /*TMP_REG1 = 0xFF00, TMP_REG2= (LSB of IPv4 and 1B 0x00)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        /*IN_FIFO: 4B UDP */
        /** copy 1B 0x7f to buff(First 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x7F, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B one's compliment of the LSB of IPv4 to buff(Second 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B 0x00 to buff(Third 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B the LSB of IPv4 to buff(Forth 1B of the last 4 of the IPv6 header) and drop last 4B of IPv6 header*/
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Get the bit that determines seamless BFD */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 4, NEVER_NEVER, LOP_NXT));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Check if the temp_reg2==0 (not seamless BFD.)  If so, jump */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_NO_SBFD], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Seamless BFD */

        /** Set Source Port and Destination Port */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        /** Unconditional jump - copy rest of packet */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2),
                CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_AFTER_SBFD], NEVER_NEVER, NXT_JMP));

        /** drop 7B: non-seamless Source and destination port, and 3 lasts bytes of IPv4 */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

        /* copy 2B from TMP_REG4 (UDP source port) and drop last 3 bytes of IPv4 and 1 byte of UDP */
        INSTR_LABEL(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP),
                DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_NO_SBFD);

        /** copy 3 bytes of UDP (2-4) shifted by 1 */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));

        /** copy 1 byte from UDP and 2 bytes from TMP_REG3 (UDP checksum) */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B),
            ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_AFTER_SBFD);

        /** copy the first byte of the BFD header (shifted by 3 bytes) */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /* *INDENT-ON* */
        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Force discriminator MSB=1 function loading.
*  The function is used from other functions to force BFD discriminator MSB to be 1.
*  It is assumed that all the system headers have been copied, the current
*  dword contains the most significant byte of the discriminator, and temp_reg1 contains
*  which byte it is (0 - most significant byte of dword; 3 - least significant byte of dword.)
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [out] func_config  -  TCAM structure including all information
*   \param [in] params  -  various parameters that can be used for any perpose
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] first_instruction  -  first instruction that was configured for that function
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_function_discr_msb_1(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    /** Used to find correct offset */
    int udh_header_size = 0, rem_bytes_to_discr;

    /** Differntiation between JR1 and JR2 modes */
    uint8 oamp_tx_format, jr1_format;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load function
     */

    /*
     * Load program
     */
    /** check if function was already loaded */
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        /** Find out which format is used: JR1 or JR2 */
        oamp_tx_format = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        jr1_format = dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit);

        /** Calculate the offset to the discriminator */
        if (oamp_tx_format == jr1_format)
        {
            rem_bytes_to_discr = udh_header_size & 0x3;
        }
        else
        {
            rem_bytes_to_discr = (2 + udh_header_size) & 0x3;
        }

        /*
         * This set of instructions is used by all the programs need to force discriminator MSB=1.
         * Note that this is not stand alone program.
         *
         * The packets that are supported by the program are any non-PWE BFD packet.
         * UDH of different size an be added to each packet.
         *
         */
        /* *INDENT-OFF* */
        /*
        INPUT:
        +-------------------------------------------------------+
        | All programs that use this function                   |
        |-------------------------------------------------------+
        | oamp_pe_bfd_default_program                           |
        |-------------------------------------------------------+
        | oamp_pe_bfd_ipv6_program                              |
        |-------------------------------------------------------+
        | oamp_pe_bfd_ipv6_jr1_program                          |
        +-------------------------------------------------------+
         */

        switch(rem_bytes_to_discr)
        {
            /*
            * Target byte is most significant byte case
            */
            case 0:
                /** Copy next 2 bytes to tmp_reg1 */
                INSTR_LABEL(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1);

                /** Set MSB of temp_reg2 to 0x80 */
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, NXT_JMP));

                /** NOP for the value to be ready */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** temp_reg3=temp_reg1 | temp_reg2 */
                INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));

                /** NOP for the value to be ready */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** copy result to buffer */
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                /** Copy lower 3 bytes to buffer*/
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                break;

            /*
            * Target byte is second to most significant byte case
            */
            case 1:
                /** Copy next 2 bytes to tmp_reg1 */
                INSTR_LABEL(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1);

                /** Copy one byte - last byte before discriminator */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                /** temp_reg2=temp_reg1 | mask */
                INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, LOP_NXT));

                /** NOP for the value to be ready */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** copy result to buffer */
                INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

                /** Copy two bytes */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;

            /*
            * Target byte is second to least significant byte case
            */
            case 2:
                /** Copy two bytes */
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1);

                /** Copy next 2 bytes to tmp_reg1 */
                INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x22, NEVER_NEVER, NXT_JMP));

                /** Set MSB of temp_reg2 to 0x80 */
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, NXT_JMP));

                /** NOP for the value to be ready */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** temp_reg3=temp_reg1 | temp_reg2 */
                INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));

                /** NOP for the value to be ready */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** copy result to buffer */
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                /** Copy lower byte to buffer*/
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;

            /*
            * Target byte is least significant byte case
            */
            case 3:
                /** Copy lower word (16B) of current dword (32B) to temp_reg1 */
                INSTR_LABEL(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1);

                /** Copy upper 3 bytes to buffer*/
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

                /** temp_reg2=temp_reg1 | mask */
                INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, LOP_NXT));

                /** NOP for the value to be ready */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                /** copy result to buffer */
                INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
                break;
        }
        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /* *INDENT-ON* */

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Discriminator with MEP type program loading.
 *  The program sets the most significant bit of the discriminator
 *  in an injected BFD packet to 1.  This is applied when the
 *  relevant SOC property is activated and the endpoint type
 *  is set to 1 in the discriminator provided in the API.
 *   \param [in] unit  -  Relevant unit.
 *   \param [in] label_mode  -  if set dictates that need to go through instructions without setting to tables to map labels to 'inst_labels'
 *   \param [in] program_id  -  requested program to be configured
 *   \param [in] program_const  -  const value related to the requested program
 *   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
 *   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
 *   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to divine the programs
 *   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
 *   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
 * \return
 *   shr_error_e
 */
static shr_error_e
oamp_pe_bfd_default_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    int udh_header_size = 0, discriminator_offset, dwords_to_discr;
    uint32 saved_prog_index;

    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    /** Differntiation between JR1 and JR2 modes */
    uint8 oamp_tx_format, jr1_format;

    /** Needed for setting PPH.TTL */
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));

    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        /** Find out which format is used: JR1 or JR2 */
        oamp_tx_format = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        jr1_format = dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit);

        /** Calculate the number of bytes to the discriminator */
        if (oamp_tx_format == jr1_format)
        {
            /** JR1 headers */
            discriminator_offset = 48 + udh_header_size;
        }
        else
        {
            /** JR2 headers */
            discriminator_offset = 54 + udh_header_size;

            /** Calculate the parts of PPH.TTL */
            pph_ttl = 0xFF;
            pph_ttl_msb = pph_ttl >> 5;
            pph_ttl_lsb = (pph_ttl << 3) & 0xFF;
        }

        /** Calculate the number of DWORDs to copy in the beginning */
        dwords_to_discr = discriminator_offset >> 2;

        /*
         * configure set of requested instructions
         */

        /* *INDENT-OFF* */
        if(oamp_tx_format == jr1_format)
        {
            /** PPH TTL not needed in JR1 format */

            /** Get the bit that determines if discr MSB=1 */
            INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO );

            /** copy whole dwords {PTCH, ITMH, ITMH-EXT, PPH, UDH, IPv4, UDP, start of BFD (until my-discriminator)} */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, dwords_to_discr, ALWAYS_NEVER, LOP_CST));

                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            /** Check if the temp_reg3==0, if so jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            /** copy whole dwords until PPH TTL {PTCH, ITMH, ITMH-EXT, first six bytes of PPH} */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

            /** copy next two bytes */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

            /** Copy 1 byte to tmp_reg1 */
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

            /** Copy 1 byte to tmp_reg2 */
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

            /** Get the bit that determines if discr MSB=1 */
            INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO );

            /** Write 3 PPH.TTL bits to tmp_reg1 */
            INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));

            /** Write 5 PPH.TTL bits to tmp_reg2 */
            INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));

                /** NOP for the value to be ready */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            /** Copy result from TMP_REG1 */
            INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

            /** Copy result from TMP_REG2 */
            INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

            /** copy whole dwords {UDH, IPv4, UDP, start of BFD (until my-discriminator)} */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, dwords_to_discr-5, ALWAYS_NEVER, LOP_CST));

            /** Check if the temp_reg3==0, if so jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_bfd_discr_msb_1_function */
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
                /** NOPs because of the Jump*/
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }
        /* *INDENT-ON* */

        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    This program add UDH for Ethernet OAM LM&DM Down-MEP packets.
 *    For CCM packets UDH Header will be added by PMF.
 *    This program is relevant for Jericho1 mode(Inter-op) and only in case UDH is exist.
 *    Input:  PTCH-2(2B)oITMH(4B)oOAM-TS(6B)oETH_OAM_PDU
 *    Output: PTCH-2(2B)oITMH(4B)oOAM-TS(6B)oUDH(udh size)oETH_OAM_PDU
 *   \param [in] unit  -  Relevant unit.
 *   \param [in] label_mode  -  if set dictates that need to go through instructions without setting to tables to map labels to 'inst_labels'
 *   \param [in] program_id  -  requested program to be configured
 *   \param [in] program_const  -  const value related to the requested program
 *   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
 *   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
 *   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to divine the programs
 *   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
 *   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
 * \return
 *   shr_error_e
 */
static shr_error_e
oamp_pe_down_lm_dm_add_udh_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    int udh_header_size = 0;
    uint32 saved_prog_index;

    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));

    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        /* *INDENT-OFF* */
        /** copy 12B from IN_FIFO(PTCH2+ITMH+OAM-TS*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, LOP_CST));

        /** Add UDH */
        if(udh_header_size <= 4)
        {
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, udh_header_size, ALWAYS_NEVER, NXT_JMP));
        }
        else
        {
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, (udh_header_size - 4), ALWAYS_NEVER, NXT_JMP));
        }

        /** copy rest packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /* *INDENT-ON* */

        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Server UP Mep program loading
*  The programs outputs the incoming packet without modifications.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_up_mep_server_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        /*
         *  configure requested instructions
         */
        if (func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER].first_instruction));
        }

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Server UP Mep program loading for maid48
*  The programs outputs the incoming packet without modifications.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_up_mep_server_maid_48_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        
        if (func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48].first_instruction));
        }

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  One-way DM program loading.
*  The programs outputs the incoming packet with updated opcode.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  If set indicates that need to go through instructions without setting to tables to map labels to 'inst_labels'.
*   \param [in] program_id  -  Requested program to be configured.
*   \param [in] program_const  -  Const value related to the requested program.
*   \param [out] inst_labels  -  Holds instruction indexes for corresponding labels.
*   \param [out] next_instruction_index  -  Next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set.
*   \param [out] next_valid_program_index  -  Program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs.
*   \param [out] prog_index  -  Program index as set in OAMP_PE_PROGRAMS_PROPERTIES.
*   \param [out] func_config  -  Function configurations including function names and their first instructions (configured on the way).
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_1dm_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** Check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /* *INDENT-OFF* */
        /*
         * The program fills opcode (at PDU+1 offset) with value of 0x2D.
         *
         * The packets that are supported by the program are (UDH of different size can be added to each packet):
         * PTCH-2-ETH-PDU(taggless, single tagged, double tagged)
         * PTCH2-ITMH-ASE-ETH-PDU
         * PTCH2-ITMH-ITMH ext-ASE-PPH-LSP-GAL-GACH-PDU
         * PTCH2-ITMH-ITMH ext-ASE-PPH-PWE-GACH-PDU
         *
         */
        /** set PDU offset - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 8, NEVER_NEVER, LOP_NXT));
        /** temp_reg2=PDU_OFFS(2bit lsb) - PDU miss-alignment */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 3, NEVER_NEVER, LOP_NXT));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        /** set temp_reg1(12bit msb) - bytes to read from IN_FIFO */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, PRG_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        /** copy pdu headers up to PDU offset bytes aligned to 4 */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));
        /** set temp_reg4=0, for alignment validation (set#0) */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));
            /** set temp_reg4=1, for alignment validation (set#1) */
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 1, NEVER_NEVER, NXT_JMP));
                /** set temp_reg4=2, for alignment validation (set#2) */
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 2, NEVER_NEVER, NXT_JMP));
        /** set jump for pdu % 4 == 0 (value for set#0 is ready)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_1B], NEVER_NEVER, NXT_JMP));
            /** set jump for pdu % 4 == 1 (value for set#1 is ready)*/
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_2B], NEVER_NEVER, NXT_JMP));
                /** set jump for pdu % 4 == 2 (value for set#2 is ready)*/
                INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_3B], NEVER_NEVER, NXT_JMP));
                /** NOPs because of the jump */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /*
        * pdu+1 % 4 == 0 case
        */
        /** copy first 4B of OAM header */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        /** Jump to TLV offset/END TLV part */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_TIME_STAMP_TLV], NEVER_NEVER, NXT_JMP));
        /** set 1DM opcode using program variable */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B shifted with 1B of the OAM header */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        /*
        * pdu+1 % 4 == 1 case
        */
        /** copy 1B related to miss-alignment */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_1DM_1B);
        /** Jump to TLV offset/END TLV part */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_TIME_STAMP_TLV], NEVER_NEVER, NXT_JMP));
        /** set 1DM opcode using program variable */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B shifted with 2 First B of the OAM header */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));

        /*
        * pdu+1 % 4 == 2 case
        */
        /** copy 2B related to miss-alignment */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_1DM_2B);
        /** Jump to TLV offset/END TLV part */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_TIME_STAMP_TLV], NEVER_NEVER, NXT_JMP));
        /** set 1DM opcode using program variable */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B shifted with 3 First B of the OAM header */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        /*
        * pdu+1 % 4 == 3 case
        */
        /** copy 3B related to miss-alignment */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_1DM_3B);
        /** set 1DM opcode using program variable */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B flags */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 1, ALWAYS_NEVER, NXT_JMP));

        /** set TLV offset using program variable */
        INSTR_LABEL(CE1_COPY(PROG_VAR, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_1DM_TIME_STAMP_TLV);
        /** write 16B 0s tx_timestamp_f and rx_timestamp_f */
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, LOP_CST));
        /** empty IN_FIFO */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, NEVER_NEVER, LOP_NXT));
        /** write 1B 0s end_tlv */
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_ALWAYS, NXT_JMP));

        /* *INDENT-ON* */
        /*
         * Set program properties
         */
        /** Configure saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   DM JUMBO TLV program loading.
*  The programs is used to build Jumbo Dm frames
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_dm_jumbo_tlv_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        /*
         *  configure requested instructions
         */
        if (func_config[DNX_OAMP_PE_FUNC_DM_JUMBO_TLV].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DM_JUMBO_TLV].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_DM_JUMBO_TLV].first_instruction));
        }

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_DM_JUMBO_TLV].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Single Hop BFD random dip program loading.
*  The programs is used to rebuild pph and remove label for random dip Single Hop BFD
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_bfd_single_hop_random_dip_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        /*
         *  configure requested instructions
         */
        if (func_config[DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP].first_instruction));
        }

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Mpls-tp vccv type BFD program loading.
*  The programs is used to rebuild  vccv type BFD packet
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_bfd_mplstp_vccv_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        /*
         *  configure requested instructions
         */
        if (func_config[DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD].first_instruction));
        }

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  LM/DM flexible dmac program loading
*  The programs replace DMAC with flexible MAC address stored in extra data for LM/DM packet.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_lmdm_flexible_da_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        /*
         *  configure requested instructions
         */
        if (func_config[DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA].first_instruction));
        }

        /*
         * set program properties
         */
        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Signal Degradation program loading
*  The programs Signal Degradation bit in OAM flags PDU.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_signal_detect_ccm_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** Check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

       /** skip 80B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 20, NEVER_NEVER, LOP_CST));

        /* *INDENT-OFF* */
       INSTR(CE1_NOP, CE2_NOP, ALU_AND(PRG_VAR, NS_8, INST_VAR, ALWAYS, ZERO, TMP_REG4),CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_SIGNAL_DETECT_COPY_REST], NEVER_NEVER, NXT_JMP));
                /** NOPs because of the jump */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

       /** If we are here mean that we should update signal detect bit and 8 bytes already copied */
       INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, ALWAYS_NEVER, LOP_CST));

       /** Set Signal Detect Bit*/
       INSTR(CE1_COPY(IN_FIFO, SIGNAL_DETECT_SET, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

       /** Copy rest of packet, means no update is needed, Signal Detect bit should remain */
       INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT), DNX_OAMP_PE_INST_LABEL_SIGNAL_DETECT_COPY_REST);

        /* *INDENT-ON* */
        /*
         * Set program properties
         */
        /** Configure saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Signal Degradation maid 48 program loading
*  The programs Signal Degradation bit in OAM flags PDU with maid48.
*   \param [in] unit  -  Relevant unit.
*   \param [in] label_mode  -  if set in dicates that need to go through instructions without setting to tables to map labels to 'inst_labels'
*   \param [in] program_id  -  requested program to be configured
*   \param [in] program_const  -  const value related to the requested program
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
*   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
*   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs
*   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
*   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
* \return
*   shr_error_e
*/
static shr_error_e
oamp_pe_signal_detect_maid_48_ccm_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify configurations to load program
     */

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    /** Check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /* *INDENT-OFF* */
        /** set PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        /** copy 48B from packet into FDBK fifo */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** skip 32B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));

        /** Jump to maid 48 program is signal detect not required */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(PRG_VAR, NS_8, INST_VAR, ALWAYS, ZERO, TMP_REG4),CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
                /** NOPs because of the jump */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** If we are here mean that we should update signal detect bit and 8 bytes already copied */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, ALWAYS_NEVER, LOP_CST));

        /** Jump to oamp_pe_copy_48b_maid_program to part with 1B not aligned*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(NONE, NONE, INST_VAR, ALWAYS, ALWAYS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_1B], NEVER_NEVER, NXT_JMP));

           /** Set Signal Detect Bit*/
           INSTR(CE1_COPY(IN_FIFO, SIGNAL_DETECT_SET, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
           /** copy 4 bytes */
           INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

        /* *INDENT-ON* */
        /*
         * Set program properties
         */
        /** Configure saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Modfies BFDoIPv4 packet to seamless BFD format..
 *  The program sets the source port of the UDP header to
 *  the value set by the user, and the destination port to 7784.
 *   \param [in] unit  -  Relevant unit.
 *   \param [in] label_mode  -  if set dictates that need to go through instructions without setting to tables to map labels to 'inst_labels'
 *   \param [in] program_id  -  requested program to be configured
 *   \param [in] program_const  -  const value related to the requested program
 *   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
 *   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
 *   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to divine the programs
 *   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
 *   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
 * \return
 *   shr_error_e
 */
static shr_error_e
oamp_pe_bfd_seamless_bfd_ipv4_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;

    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    /** Needed for setting PPH.TTL */
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));

    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** Calculate the parts of PPH.TTL */
        pph_ttl = 0xFF;
        pph_ttl_msb = pph_ttl >> 5;
        pph_ttl_lsb = (pph_ttl << 3) & 0xFF;

        /* *INDENT-OFF* */
        /*
        * copy:
        * PTCH(2B)
        * ITMH(5B)
        * ITMH-EXT(3B)
        * PPH(12B)
        * UDH based on 'udh_size' predefined size (0-17B)
        * IPv4(20B)
        * UDP sport(2B)
        * UDP dport(2B)
        * rest UDP(4B) - length, checksum
        * BFD(24B)
        **/

        /** copy 16B {PTCH, ITMH, ITMH-EXT, PPH (first 6 bytes)} */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

        /** copy next two bytes */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        /** Copy 1 byte to tmp_reg3 */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG3), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

        /** Copy 1 byte to tmp_reg2 */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

            /** NOP for the value to be ready */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** Write 3 PPH.TTL bits to tmp_reg3 */
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG3, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));

        /** Write 5 PPH.TTL bits to tmp_reg2 */
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));

        /** set PDU offset - 20 (loop on temp_reg uses extra 2 instructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 20, NEVER_NEVER, LOP_NXT));

        /** Copy result from TMP_REG3 */
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        /** Copy result from TMP_REG2 */
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        /** copy pdu headers up to PDU offset bytes aligned to 4 */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));

        /** copy 3B */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

        /** Set Source Port and Destination Port */
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        /** copy 1B */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        /** Get the bit that determines if discr MSB=1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT) );

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** copy 4B */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

        /** BFD MEP type discriminator feature */

        /** Check if the temp_reg2==0, if so jump*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_bfd_discr_msb_1_function */
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
                /** NOPs because of the Jump*/
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }

        /* *INDENT-ON* */

        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Modfies BFDoIPv4 packet to seamless BFD format for JR1 mode.
 *  The program sets the source port of the UDP header to
 *  the value set by the user, and the destination port to 7784.
 *   \param [in] unit  -  Relevant unit.
 *   \param [in] label_mode  -  if set dictates that need to go through instructions without setting to tables to map labels to 'inst_labels'
 *   \param [in] program_id  -  requested program to be configured
 *   \param [in] program_const  -  const value related to the requested program
 *   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
 *   \param [out] next_instruction_index  -  next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set
 *   \param [out] next_valid_program_index  -  program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to divine the programs
 *   \param [out] prog_index  -  program index as set in OAMP_PE_PROGRAMS_PROPERTIES
 *   \param [out] func_config  -  function configurations including function names and their first instructions (configured on the way)
 * \return
 *   shr_error_e
 */
static shr_error_e
oamp_pe_bfd_seamless_bfd_ipv4_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;

    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;
    uint32 udh_header_size = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Load program
     */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));

    /** check if program was already loaded */
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, &udh_header_size));

        /** The extra bytes were copied before calling this function */
        udh_header_size = udh_header_size & 0x3;

        /* *INDENT-OFF* */
        /*
        * copy:
        * PTCH(2B)
        * ITMH(5B)
        * ITMH-EXT(3B)
        * PPH(12B)
        * UDH based on 'udh_size' predefined size (0-17B)
        * IPv4(20B)
        * UDP sport(2B)
        * UDP dport(2B)
        * rest UDP(4B) - length, checksum
        * BFD(24B)
        **/

        /** set PDU offset - 20 (loop on temp_reg uses extra 2 instructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 20, NEVER_NEVER, LOP_NXT));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** copy pdu headers up to PDU offset bytes aligned to 4 */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));

        switch (udh_header_size)
        {
            case 0:
                /** copy 4B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

                /** Set Source Port and Destination Port */
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

                /** drop 4 bytes (OAMP generated UDP ports) */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

                /** copy 4B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
                break;

            case 1:
                /** copy 1B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                /** Set Source Port and Destination Port */
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

                /** drop 4 bytes (OAMP generated UDP ports) */
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

                /** copy 3B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));

                /** copy 4B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
                break;

            case 2:
                /** copy 2B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                /** Set Source Port and Destination Port */
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

                /** copy 2B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;

            case 3:
            default:
                /** copy 3B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

                /** Set Source Port and Destination Port */
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

                /** copy 1B */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;

        }

        /** Get the bit that determines if discr MSB=1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT) );

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** copy 4B */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

        /** BFD MEP type discriminator feature */

        /** Check if the temp_reg2==0, if so jump*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_bfd_discr_msb_1_function */
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
                /** NOPs because of the Jump*/
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }

        /* *INDENT-ON* */

        /** configures saved_prog_index for future use */
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check whether a given 'fem_id' should be loaded to HW based on SOC properties.
*   \param [in] unit  -  Relevant unit.
*   \param [in] fem_id  -  FEM id to be validated
* \return
*   return 0 if FEM should not be loaded.
*/
static int
dnx_oamp_pe_check_fem_soc(
    int unit,
    dnx_oamp_pe_fems_e fem_id)
{
    return 1;
}

/**
* \brief
*  Initialize FEM structure with predefined values to be set to HW later.
*   \param [in] unit  -  Relevant unit.
*   \param [out] fem_actions  -  FEM structure including all information
* \return
*   shr_error_e
*/
static shr_error_e
dnx_oamp_pe_fem_init(
    int unit,
    dnx_oamp_pe_fem_config_s fem_actions[DNX_OAMP_PE_FEM_LAST])
{
    /* *INDENT-OFF* */
    dnx_oamp_pe_fem_config_s fem_actions_local[DNX_OAMP_PE_FEM_LAST] = {

        /**
         * THESE HAVE TO BE IN THE SAME ORDER AS THE "DNX_OAMP_PE_FEM_" enum!
         * The enumeration value at the start of the line is used to validate this.
         *
         * DNX_OAMP_PE_FEM_OUTLIF_VSI_LSB_SHIFT - used in  oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained in two places:
         *          1. MSB byte of FEM for VSI(3 MSB bits) by shifting them by 2(as a result of nibble select not aligned) 0b000xxx00 => 0x00000xxx
         *          2. LSB byte of FEM for OUT_LIF(6 MSB bits) by shifting them by 2(as a result of nibble select not aligned) 0bxxxxxx00 => 0x00xxxxxx
         * DNX_OAMP_PE_FEM_PPH_EXIST_UPDATE - used in oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained for setting PPH Exist in ITMH header
         *          This FEM is copy 2 MSB to buffer and setting PPH exist bit(33) to 1.
         *          !!! In case of FEM needed can be done without using FEM, but will take more instructions.
         * DNX_OAMP_PE_FEM_BFD_IPV6_ECHO - used in oamp_pe_bfd_ipv6_program
         *          we store the is_echo flag in the ls bit of the second byte of the extra data
         *          this FEM is used to set this bit to zero when copying it to the buffer
         * DNX_OAMP_PE_FEM_ITMH_DP - used in up_mep server w/wo maid 48 programs for extracting DP bits from packet variable for ITMH header(byte 3-4)
         * DNX_OAMP_PE_FEM_ITMH_DP_JR1 - used in up_mep server w/wo maid 48 programs for extracting DP bits from packet variable for ITMH header(byte 3-4)
         * DNX_OAMP_PE_FEM_ITMH_TC - used in up_mep server w/wo maid 48 programs for extracting TC bits from packet variable for ITMH header(byte 0-1)
         * DNX_OAMP_PE_FEM_VCCV_BFD - used in oamp_pe_function_bfd_mplstp_vccv to set FAI to 5, which means  {6~9 bits = 0x0101}
         */

        /** fem_id                                    bit_0      bit_1       bit_2       bit_3       bit_4       bit_5       bit_6       bit_7       bit_8     bit_9     bit_10       bit_11     bit_12      bit_13      bit_14      bit_15       valid   hw_index*/
        /** default FEM - is set by default in HW, present here for visualization only*/
        {DNX_OAMP_PE_FEM_OUTLIF_VSI_LSB_SHIFT,     {{OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}, {OFST, 19}, {CNST, 0},  {CNST, 0},  {OFST, 20}, {OFST, 21}, {OFST, 22}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_OUTLIF_VSI_LSB_SHIFT_JR1, {{OFST, 28}, {OFST, 29}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {OFST, 30}, {OFST, 31}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_PPH_EXIST_UPDATE,         {{OFST, 0},  {OFST, 1},  {OFST, 2},  {OFST, 3},  {OFST, 4},  {OFST, 5},  {OFST, 6},  {OFST, 7},  {OFST, 8},  {CNST, 1},  {OFST, 10}, {OFST, 11}, {OFST, 12}, {OFST, 13}, {OFST, 14}, {OFST, 15}},   1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_PPH_EXIST_UPDATE_JR1,     {{OFST, 0},  {OFST, 1},  {OFST, 2},  {OFST, 3},  {OFST, 4},  {OFST, 5},  {OFST, 6},  {OFST, 7},  {OFST, 8},  {OFST, 9},  {OFST, 10}, {OFST, 11}, {OFST, 12}, {OFST, 13}, {CNST, 1},  {OFST, 15}},   1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_VSI_GEN_MEM_OFFSET_JR1,   {{OFST, 1},  {OFST, 2},  {OFST, 3},  {OFST, 4},  {OFST, 5},  {OFST, 6},  {OFST, 7},  {OFST, 8},  {OFST, 9},  {OFST, 10}, {OFST, 11}, {OFST, 12}, {OFST, 13}, {OFST, 14}, {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_BFD_IPV6_ECHO,            {{CNST, 0},  {OFST, 17}, {OFST, 18}, {OFST, 19}, {OFST, 20}, {OFST, 21}, {OFST, 22}, {OFST, 23}, {OFST, 24}, {OFST, 25}, {OFST, 26}, {OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}},   1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_ITMH_DP,                  {{CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {OFST, 26}, {OFST, 27}},   1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_ITMH_DP_JR1,              {{CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {OFST, 26}, {OFST, 27}, {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_ITMH_TC,                  {{CNST, 0},  {OFST, 28}, {OFST, 29}, {OFST, 30}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_SIGNAL_DETECT_SET,        {{OFST, 16}, {OFST, 17}, {OFST, 18}, {OFST, 19}, {OFST, 20}, {OFST, 21}, {CNST, 1},  {OFST, 23}, {OFST, 24}, {OFST, 25}, {OFST, 26}, {OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_TLV_DATA_OFFSET,          {{OFST, 21}, {OFST, 22}, {OFST, 23}, {OFST, 24}, {OFST, 25}, {OFST, 26}, {OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_TLV_DATA_OFFSET_RSP,      {{OFST, 23}, {OFST, 24}, {OFST, 25}, {OFST, 26}, {OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_VCCV_BFD,                 {{OFST, 0},  {OFST, 1},  {OFST, 2},  {OFST, 3},  {OFST, 4},  {OFST, 5},  {CNST, 1},  {CNST, 0},  {CNST, 1},  {CNST, 0},  {OFST, 10}, {OFST, 11}, {OFST, 12}, {OFST, 13}, {OFST, 14}, {OFST, 15}},   1,    DNX_OAMP_PE_NOT_SET},
    };
    /* *INDENT-ON* */
    int ii, jj;

    SHR_FUNC_INIT_VARS(unit);
    /*
     *  Update predifined values based on configuration
     */

    /** go through all FEMs and set 'hw_index' for the ones to be loaded */
    for (ii = 0, jj = 0; ii < DNX_OAMP_PE_FEM_LAST; ii++)
    {
        if ((fem_actions_local[ii].valid) && dnx_oamp_pe_check_fem_soc(unit, ii))
        {
            fem_actions_local[ii].hw_index = jj;
            jj++;
        }
    }

    /*
     * Copy return values and perform result basic sanity
     */

    /** copy final FEM configuration to be returned */
    for (ii = 0; ii < DNX_OAMP_PE_FEM_LAST; ii++)
    {
        if (fem_actions_local[ii].fem_id != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "fem_actions_local at index %d is out of order.\n", ii);
        }
        fem_actions[ii].fem_id = fem_actions_local[ii].fem_id;
        fem_actions[ii].valid = fem_actions_local[ii].valid;
        fem_actions[ii].hw_index = fem_actions_local[ii].hw_index;
        for (jj = 0; jj < DNX_DATA_MAX_OAM_OAMP_OAMP_PE_NOF_FEM_BITS; jj++)
        {
            fem_actions[ii].bit[jj][0] = fem_actions_local[ii].bit[jj][0];
            fem_actions[ii].bit[jj][1] = fem_actions_local[ii].bit[jj][1];
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check whether a given 'fem_id' should be loaded to HW based on SOC properties.
*   \param [in] unit  -  Relevant unit.
*   \param [in] profile_id  -  MEP_PE_PROFILE id to be validated
* \return
*   return 1 if MEP_PE profile should not be loaded.
*/
static int
dnx_oamp_pe_mep_pe_profile_soc_disable(
    int unit,
    dbal_enum_value_field_mep_pe_profile_sw_e profile_id)
{
    return 0;
}

/**
* \brief
*  Initialize MEP_PE_PROFILE HW and SW structures with predefined values to be set later.
*   \param [in] unit  -  Relevant unit.
*   \param [out] mep_pe_profiles  -  MEP_PE_PROFILE structure including all information
* \return
*   shr_error_e
*/
static shr_error_e
dnx_oamp_pe_mep_pe_profile_init(
    int unit,
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES])
{

    int system_headers_mode;

/* *INDENT-OFF* */
dnx_oamp_mep_pe_profile_s mep_pe_profiles_local[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES] = {

    /** profile_id                                            BYTE_0_OFFSET   BYTE_1_OFFSET   WORD_0_OFFSET   WORD_1_OFFSET     MEP_INSERT   valid   hw_index*/

    /** default mep pe profile */
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT,                            0,              0,              0,              0,              0,          1,   0},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO,                           0,              0,              0,              0,              0,          1,   1},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,27,             48,             29,             50,             0,          1,   2},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED,        27,             48,             29,             50,             0,          1,   3},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48,                            0,              0,              0,              0,              0,          1,   4},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DEFAULT,                        50,             0,              0,              0,              0,          1,   5},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6,                           50,             0,              0,              0,              0,          1,   6},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS,                      47,             0,              0,              0,              0,          1,   8},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP,                           0,              0,              0,              0,              0,          1,   10},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48,                   0,              0,              0,              0,              0,          1,   11},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER,                      0,              0,              29,             50,             0,          1,   12},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48,              0,              0,              29,             50,             0,          1,   13},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM,                                0,              0,              0,              0,              0,          1,   14},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48,                        0,              0,              0,              0,              0,          1,   15},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA,                   0,              0,              29,             50,             0,          1,   16},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48,           0,              0,              29,             50,             0,          1,   17},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT,                      48,             0,              0,              0,              0,          1,   18},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48,              48,             0,              0,              0,              0,          1,   19},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48,               0,              0,              29,             50,             0,          1,   20},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV,                       0,              0,              29,             50,             0,          1,   21},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4,                  50,             0,              0,              0,              0,          1,   22},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6,                  50,             0,              0,              0,              0,          1,   23},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SINGLE_HOP_BFD_RANDOM_DIP,           0,             0,              0,              0,              0,          1,   24},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV,                    0,             0,              0,              0,              0,          1,   25}
};

    /* *INDENT-ON* */
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    if ((mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP].hw_index & 0xfffe) !=
        (mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48].hw_index & 0xfffe))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Down Mep PE hardware profile value w/wo maid48 should be different by 1 lsb bit only.\n");
    }

    if ((mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER].hw_index & 0xfffe) !=
        (mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48].hw_index & 0xfffe))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "UP Mep Server PE hardware profile value w/wo maid48 should be different by 1 lsb bit only.\n");
    }

    if ((mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV].hw_index & 0xfffe) !=
        (mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].hw_index & 0xfffe))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Jumbo DM tlv PE hardware profile value w/wo maid48 should be different by 1 lsb bit only.\n");
    }

    if ((mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48].hw_index & 0xffef) !=
        (mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].hw_index & 0xffef))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "JUMBO MAID48 and not jumbo maid 48  should be different by 1 4'th bit.\n");
    }

    /*
     *  Update predifined values based on configuration
     */
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

     /** In Inter-op mode VSI is taken from gen_mem and pointer to gen_mem is MEP_ID
      *  byte_0 not in use*/
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED].word_0_offset = 54;
        mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED].byte_0_offset = 0;
        mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].word_0_offset = 54;
        mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].byte_0_offset = 0;
    }

    mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV].word_0_offset = 54;
    mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV].byte_0_offset = 0;
    mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].word_0_offset = 54;
    mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].byte_0_offset = 0;

    /*
     * go through all MEP_PE_PROFILEs and unset 'valid' if needed.
     * 'hw_index' is manually managed due to use limitations.
     */
    for (ii = 0; ii < DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES; ii++)
    {
        if ((mep_pe_profiles_local[ii].valid) && dnx_oamp_pe_mep_pe_profile_soc_disable(unit, ii))
        {
            mep_pe_profiles_local[ii].valid = 0;
        }
    }

    /*
     * Copy return values and perform result basic sanity
     */
    for (ii = 0; ii < DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES; ii++)
    {
        if (mep_pe_profiles_local[ii].profile_id != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "mep_pe_profile_actions at index %d is out of order.\n", ii);
        }
    }

    /** copy final MEP_PE_PROFILE configuration to be returned */
    sal_memcpy(mep_pe_profiles, mep_pe_profiles_local, sizeof(mep_pe_profiles_local));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check whether a given 'tcam_id' should be loaded to HW based on SOC properties.
*   \param [in] unit  -  Relevant unit.
*   \param [in] tcam_config_local  -  TCAM structure including all information
*   \param [in] tcam_id  -  TCAM id to be validated
*   \param [in] udh_header_size - udh size
* \return
*   return 0 if TCAM should not be loaded.
*/
static int
dnx_oamp_pe_tcam_soc_enable(
    int unit,
    oamp_pe_tcam_config_s tcam_config_local[DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES],
    dbal_enum_value_field_oamp_pe_prog_tcam_enum_e tcam_id,
    int udh_header_size)
{
    int system_headers_mode;
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    switch (tcam_id)
    {
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48:
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA:
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48:
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV:
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV_RSP:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DEFAULT:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_ADD_UDH_JR1:
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_MAID48_CCM_UDH_JR1:
            tcam_config_local[tcam_id].valid = ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
                                                && (udh_header_size != 0));
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        default:
            break;
    }

    return tcam_config_local[tcam_id].valid;
}

/**
* \brief
*  Initialize TCAM structure with predefined values to be set to HW later.
*   \param [in] unit  -  Relevant unit.
*   \param [out] tcam_config  -  TCAM structure including all information
* \return
*   shr_error_e
*/
static shr_error_e
dnx_oamp_pe_tcam_init(
    int unit,
    oamp_pe_tcam_config_s tcam_config[DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES])
{
    /* *INDENT-OFF* */
    /*
    * the order represents the order of the TCAM memory (entry at index 1 is checked first)
    * hw_index must be set to DNX_OAMP_PE_NOT_SET as it will over-writen
    */
    oamp_pe_tcam_config_s tcam_config_local[DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES] = {

        /** tcam_id                                                                         program_ptr                                           valid   program_id                                                                program_const                            key_type                        key_type_mask  key                                                                                                                                                                                                     hw_index*/
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED,                 oamp_pe_down_mep_inj_self_contained_program,          1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED,           0x00000000,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x1,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED},         {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x3}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,         oamp_pe_down_mep_inj_self_contained_maid_48_program,  1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,   0x00000001,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x1,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48}, {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM,   oamp_pe_down_mep_inj_self_contained_program,          1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED,           0x00000000,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x1,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48}, {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x1}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO_JR1,                                oamp_pe_bfd_echo_jr1_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO_JR1,                          DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO},                            {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS_JR1,                           oamp_pe_bfd_ipv6_mpls_jr1_program,                    1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS_JR1,                     DNX_OAMP_PE_BFD_IPV6_MPLS_PROG_VAR,      OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS},      {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS},                       {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_JR1,                                oamp_pe_bfd_ipv6_jr1_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_JR1,                          DNX_OAMP_PE_BFD_IPV6_JR1_PROG_VAR,       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6},                            {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS,                               oamp_pe_bfd_ipv6_mpls_program,                        1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS,                         DNX_OAMP_PE_BFD_IPV6_MPLS_PROG_VAR,      OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS},      {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS},                       {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6,                                    oamp_pe_bfd_ipv6_program,                             1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6,                              DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6},                            {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48,                                     oamp_pe_maid_48_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48,                               0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48},                             {0x10}, {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO,                                    oamp_pe_bfd_echo_program,                             1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO,                              DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO},                            {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DEFAULT,                                 oamp_pe_bfd_default_program,                          1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_DEFAULT,                           0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DEFAULT},                         {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_ADD_UDH_JR1,                        oamp_pe_down_lm_dm_add_udh_jr1_program,               1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_ADD_UDH_JR1,                  0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x1,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP},                            {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x1}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_MAID48_CCM_UDH_JR1,                 oamp_pe_maid_48_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48,                               0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48},                    {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER,                               oamp_pe_up_mep_server_program,                        1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER,                         0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER},                       {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER_MAID_48,                       oamp_pe_up_mep_server_maid_48_program,                1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER_MAID_48,                 0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48},               {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM,                                         oamp_pe_1dm_program,                                  1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_1DM,                                   DNX_OAMP_PE_1DM_PROG_VAR,                OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM},                                 {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR},          {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48,                                 oamp_pe_1dm_program,                                  1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_1DM,                                   DNX_OAMP_PE_1DM_PROG_VAR,                OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48},                         {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR},          {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48_CCM,                             oamp_pe_maid_48_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48,                               0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48},                         {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA,                            oamp_pe_lmdm_flexible_da_program,                     1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA,                      0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA},                    {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x1}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48,                    oamp_pe_lmdm_flexible_da_program,                     1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA,                      0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48},            {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x1}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM,                oamp_pe_lmdm_flexible_da_maid_48_ccm_program,         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM,          0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48},            {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_CCM,                           oamp_pe_signal_detect_ccm_program,                    1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_CCM,                     0x00000002,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE},     {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT},                       {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_MAID_48_CCM,                   oamp_pe_signal_detect_maid_48_ccm_program,            1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_MAID_48_CCM,             0x00000002,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE},     {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48},               {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV,                                oamp_pe_dm_jumbo_tlv_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV,                          0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48},                {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR},          {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV_RSP,                            oamp_pe_dm_jumbo_tlv_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV_RSP,                      1,                                       OAMP_PE_TCAM_KEY_MEP_RESPONSE,  0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48},                {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR},          {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4,                           oamp_pe_bfd_seamless_bfd_ipv4_program,                1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4,                     7784,                                    OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4},                   {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6,                           oamp_pe_bfd_ipv6_program,                             1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6,                     DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6},                   {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4_JR1,                       oamp_pe_bfd_seamless_bfd_ipv4_jr1_program,            1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4_JR1,                 7784,                                    OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4},                   {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_JR1,                       oamp_pe_bfd_ipv6_jr1_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6_JR1,                 DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6},                   {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP,                   oamp_pe_bfd_single_hop_random_dip_program,            1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP,             0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS},      {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SINGLE_HOP_BFD_RANDOM_DIP},           {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV,                            oamp_pe_bfd_mplstp_vccv_program,                      1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_MPLS_TP_VCCV,                      DNX_OAMP_PE_BFD_VCCV_PROG_VAR,           OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS},      {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV},                    {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DEFAULT,                                     oamp_pe_default_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DEFAULT,                               0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT},                             {0x3F}, {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0xF}}},DNX_OAMP_PE_NOT_SET}
    };
    /* *INDENT-ON* */
    int ii, jj;
    int system_headers_mode;
    int udh_header_size = 0;
    uint32 prog_var_tmp = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     *  Update predifined values based on configuration
     */

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

     /** Calculate the offset to the discriminator */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
         /**
          * Updating program variable for several programs.
          */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));
        prog_var_tmp = ((0x20 << 24) | (((udh_header_size << 1) | 1) << 16) | (0xc0 << 8));

         /** First 3 bytes(MSB of Program variable represent first 3 bytes of PPH Header */
        tcam_config_local[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].program_const =
            prog_var_tmp |
            tcam_config_local[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].program_const;

        tcam_config_local
            [DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM].program_const =
            prog_var_tmp |
            tcam_config_local
            [DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM].program_const;

        tcam_config_local[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].program_const =
            prog_var_tmp |
            tcam_config_local[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].program_const;
    }

    for (ii = 0, jj = 0; ii < DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES; ii++)
    {
        if (dnx_oamp_pe_tcam_soc_enable(unit, tcam_config_local, ii, udh_header_size))
        {
            tcam_config_local[ii].hw_index = jj;
            jj++;
        }
    }

    /*
     * Copy return values and perform result basic sanity
     */
    for (ii = 0; ii < DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES; ii++)
    {
        if (tcam_config_local[ii].tcam_id != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tcam_config_local at index %d is out of order.\n", ii);
        }
    }

    sal_memcpy(tcam_config, tcam_config_local, sizeof(tcam_config_local));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Initialize functions structure with predefined values.
*   \param [in] unit  -  Relevant unit.
*   \param [out] func_config  -  TCAM structure including all information
* \return
*   shr_error_e
*/
static shr_error_e
dnx_oamp_pe_func_init(
    int unit,
    oamp_pe_func_config_t func_config[DNX_OAMP_PE_FUNC_NOF])
{
    /* *INDENT-OFF* */
    oamp_pe_func_config_t func_config_local[DNX_OAMP_PE_FUNC_NOF] = {
        /** func_id                                                       func_ptr                                                                  first_instruction */
        {DNX_OAMP_PE_FUNC_DEFAULT,                                        oamp_pe_function_default,                                                 DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_COPY_MAID_48,                                   oamp_pe_function_copy_48_maid,                                            DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED, oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_dispatch, DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_DISCR_MSB_1,                                    oamp_pe_function_discr_msb_1,                                             DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_UP_MEP_SERVER,                                  oamp_pe_function_up_mep_server,                                           DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48,                          oamp_pe_function_up_mep_server_maid_48,                                   DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA,                               oamp_pe_function_lmdm_flexible_da,                                        DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_DM_JUMBO_TLV,                                   oamp_pe_function_dm_jumbo_tlv,                                            DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP,                          oamp_pe_function_bfd_single_hop_random_dip,                               DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD,                                  oamp_pe_function_bfd_mplstp_vccv,                                         DNX_OAMP_PE_NOT_SET},
};
    /* *INDENT-ON* */
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    /*
     *  Update predifined values based on configuration
     */

    /*
     * Copy return values and perform result basic sanity
     */
    for (ii = 0; ii < DNX_OAMP_PE_FUNC_NOF; ii++)
    {
        if (func_config_local[ii].func_id != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "func_config_local at index %d is out of order.\n", ii);
        }
    }

    sal_memcpy(func_config, func_config_local, sizeof(func_config_local));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Initialize labels array with invalid values.
*   \param [in] unit  -  Relevant unit.
*   \param [out] inst_labels  -  holds instruction indexes for corresponding labels
* \return
*   shr_error_e
*/
static shr_error_e
dnx_oamp_pe_init_inst_labels(
    int unit,
    int inst_labels[DNX_OAMP_PE_INST_LABEL_LAST])
{
    int inst_labels_local[DNX_OAMP_PE_INST_LABEL_LAST];
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DNX_OAMP_PE_INST_LABEL_LAST; ii++)
    {
        inst_labels_local[ii] = DNX_OAMP_PE_NOT_SET;
    }

    /*
     * Copy return values
     */

    sal_memcpy(inst_labels, inst_labels_local, sizeof(inst_labels_local));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * End of Inner functions
 * }
 */

/*
 * APIs
 * {
 */
shr_error_e
dnx_oamp_pe_module_init(
    int unit)
{
    int ii, prog_index;
    /** index of default instruction */
    int default_instruction;
    /** maximal number of instructions to execute in single program */
    int max_nof_instructions;
    /** next instruction index to be used to set in OAMP_PE_INSTRUCTIONS - value is updated with every new instruction set */
    int next_instruction_index = 0;
    /** program index to be used in OAMP_PE_PROGRAMS_PROPERTIES to devine the programs */
    int next_valid_program_index = 0;
    /** FEM array including all FEMs */
    dnx_oamp_pe_fem_config_s fem_actions[DNX_OAMP_PE_FEM_LAST];
    /** MEP_PE_PROFILE array including all profiles information */
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES];
    /** holds instruction indexes for corresponding labels */
    int inst_labels[DNX_OAMP_PE_INST_LABEL_LAST];
    /** TCAM configuration values */
    oamp_pe_tcam_config_s tcam_config[DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES];
    oamp_pe_func_config_t func_config[DNX_OAMP_PE_FUNC_NOF];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize local variables
     */
    default_instruction = dnx_data_oam.oamp.oamp_pe_default_instruction_index_get(unit);
    max_nof_instructions = dnx_data_oam.oamp.oamp_pe_max_nof_instructions_get(unit);

    /** initialize instruction labels */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_init_inst_labels(unit, inst_labels));
    /** initialize FEMs values */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_fem_init(unit, fem_actions));
    /** initialize MEP_PE_PROFILEs values */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_init(unit, mep_pe_profiles));
    /** initialize TCAM values */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_tcam_init(unit, tcam_config));
    /** initialize function values */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_func_init(unit, func_config));

    /** initialize MEP_PE profiles */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_set(unit, mep_pe_profiles));
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_set(unit, mep_pe_profiles));

    /** initialize FEM table */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_fem_set(unit, fem_actions));

    /** initialize OAM_PE_INSTRUCTION_CONFIG table */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_instruction_config_init
                    (unit, default_instruction, max_nof_instructions, DBAL_ENUM_FVAL_DEBUG_MODE_OFF));

    /*
     * configure all programs
     */

    /*
     * go over the instructions to map labels to instruction indexes - no entry is added to any table.
     */
    for (ii = DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES - 1; ii > -1; ii--)
    {
        if (tcam_config[ii].valid)
        {
            SHR_IF_ERR_EXIT(tcam_config[ii].program_ptr
                            (unit, DNX_OAMP_PE_LABEL_MODE_ON, tcam_config[ii].program_id, tcam_config[ii].program_const,
                             inst_labels, &next_instruction_index, &next_valid_program_index, &prog_index,
                             func_config));
        }
    }
    /** reset values */
    next_instruction_index = 0;
    next_valid_program_index = 0;
    /** re-initialize function values */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_func_init(unit, func_config));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_OAMP_PE_PROGRAM_SW_ENUM2HW));

    /*
     * perform the actual table configuration.
     */
    for (ii = DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES - 1; ii > -1; ii--)
    {
        if (tcam_config[ii].valid)
        {
            SHR_IF_ERR_EXIT(tcam_config[ii].program_ptr
                            (unit, DNX_OAMP_PE_LABEL_MODE_OFF, tcam_config[ii].program_id,
                             tcam_config[ii].program_const, inst_labels, &next_instruction_index,
                             &next_valid_program_index, &prog_index, func_config));
            if (prog_index != DNX_OAMP_PE_NOT_SET)
            {
                /** configure TCAM entry for program selection */
                SHR_IF_ERR_EXIT(dnx_oamp_pe_programs_tcam_set(unit, tcam_config[ii], prog_index));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_pe_sw_mep_pe_profile_get(
    int unit,
    dbal_enum_value_field_mep_pe_profile_sw_e profile,
    uint32 *profile_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROFILE_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE_SW, profile);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, profile_id));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get preconfigured UDH header size.
*   \param [in] unit  -  Relevant unit.
*   \param [out] udh_size  -  preconfigured UDH size
* \return
*   shr_error_e
*/
shr_error_e
oamp_pe_udh_header_size_get(
    int unit,
    uint32 *udh_size)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_UDH, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_UDH_SIZE, INST_SINGLE, udh_size));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * End of APIs
 * }
 */

/*
 * Utilities
 * {
 */

/*
 * }
 * End of utilities
 */
