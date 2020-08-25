/** \file oamp_pe.c
 * $Id$
 *
 * OAMP PE procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
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
#define DNX_OAMP_PE_BFD_ECHO_UDP_DPORT 0x0EC90000
/** used by BFD_IPv6_MPLS for IPv6 header calculation */
#define DNX_OAMP_PE_BFD_IPV6_MPLS_PROG_VAR 0x7F0000FF
/*
 * }
 */

/*
 * Inner functions
 * {
 */

/**
* \brief
*  Get preconfigured UDH header size.
*   \param [in] unit  -  Relevant unit.
*   \param [out] udh_size  -  preconfigured UDH size
* \return
*   shr_error_e
*/
static shr_error_e
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
    int udh_header_size = 0;
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

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        /*
         * Theese set of instructions are used by all the programs need to handle MAID 48B.
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
        | oamp_pe_maid_48_program                               | Shuold store the| 2B(PTCH) 2B(ITMH) | Shuold store 48B  |
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
        /** set temp_reg3=1, for alignment validation */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, NXT_JMP));
        /** set temp_reg4=2, for alignment validation */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 2, NEVER_NEVER, NXT_JMP));
        /** copy pdu headers including first 10B of OAM header */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));
        /** set temp_reg4=3, for alignment validation */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 3, NEVER_NEVER, NXT_JMP));
        /** set jump for pdu+10 % 4 == 1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_1B], NEVER_NEVER, NXT_JMP));
        /** set jump for pdu+10 % 4 == 2 */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_2B], NEVER_NEVER, NXT_JMP));
        /** set jump for pdu+10 % 4 == 3 */
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
        /** copy 44B {PTCH, ITMH, ITMH-EXT, PPH, UDH, IPv4(pending UDH size)} */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 11, ALWAYS_NEVER, LOP_CST));

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
        /** copy PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        /** skip 16B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));
        /** copy 48B from packet into FDBK fifo */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** skip 16B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));

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
*  DOWN_MEP_INJ_SELF_CONTAINED program loading.
*  The program gets the packet with prefix of 80B that are not sent out.
*  The program build ITMH ext and PPH header.
*  After all the system headers are built/copied this program jumps to oamp_pe_copy_48b_maid_program to place the MEG_ID 10B after PDU_OFFS.
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
    int udh_header_size = 0;
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

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        /*
         * configure set of requested instructions 
         */

        /* *INDENT-OFF* */
        /** store 0x2 in MSB of tmp-reg1, used to mask the PPH_TYPE field*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG1), CMN(NONE, NONE, 0x2, NEVER_NEVER, NXT_JMP));
        /** store 0x1 in MSB of tmp-reg3, used to mask the ITMH_BASE_EXTENSION_EXISTS field*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP));
        /** store 0x3f in MSB of tmp-reg4, used to mask the OUT_LIF*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG4), CMN(NONE, NONE, 0x3f, NEVER_NEVER, NXT_JMP));
        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_copy_48b_maid_program */
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
*  The program build ITMH ext and PPH header.
*  After all the system headers are built/copied this program jumps to oamp_pe_copy_48b_maid_program to place the MEG_ID 10B after PDU_OFFS.
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
    int udh_header_size = 0;
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

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        /*
         * configure set of requested instructions 
         */

        /* *INDENT-OFF* */
        /** store 0x2 in MSB of tmp-reg1, used to mask the PPH_TYPE field*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG1), CMN(NONE, NONE, 0x2, NEVER_NEVER, NXT_JMP));
        /** store 0x1 in MSB of tmp-reg3, used to mask the ITMH_BASE_EXTENSION_EXISTS field*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP));
        /** store 0x3f in MSB of tmp-reg4, used to mask the OUT_LIF*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG4), CMN(NONE, NONE, 0x3f, NEVER_NEVER, NXT_JMP));
        /** skip 16B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));
        /** copy 48B from packet into FDBK fifo */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** skip 16B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_copy_48b_maid_program */
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
oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    int udh_header_size = 0;
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

        /** get the predefined UDH header size to be copied */
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

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
        *-------------------------------------------------------+-----------------+-----------------+-----------------+-------------------+-------------------*
        | All programs that use this function                   | TMP_REG1        | TMP_REG3        | TMP_REG4        | IN_FIFO 4Bs       |  Feedback FIFO    |
        |-------------------------------------------------------+-----------------+-----------------+-----------------+-------------------|-------------------|
        | oamp_pe_down_mep_inj_self_contained_maid_48_program   | 0x2             | 0x1             | 0x3f            | 2B(PTCH) 2B(ITMH) | 48B MEG_ID        |
        |-------------------------------------------------------+-----------------+-----------------+-----------------+-------------------|-------------------|
        | oamp_pe_down_mep_inj_self_contained_program           | 0x2             | 0x1             | 0x3f            | 2B(PTCH) 2B(ITMH) | Empty             |
        *-------------------------------------------------------+-----------------+-----------------+-----------------+-------------------*-------------------*
         */
        /** copy 2B PTCH to buff and store first 2B of the ITMH header to tmp_reg2*/
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED);
        /** mask the MS 6 bits of the OUT_LIF temp_reg2 and store it in tmp_reg4 */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, TMP_REG4, ALU_OUT, ALWAYS, ALWAYS, TMP_REG4), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        /** copy 2B first 2 of the ITMH to buff update the PPH_TYPE to 1*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, OR), CE2_COPY(TMP_REG1, DEFAULT, NONE, 2B), ALU_NOP,CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        /** copy 3B last 2 of the ITMH to buff 3-4*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** copy last 1B of the ITMH to buff update the ITMH_BASE_ EXTENSION_EXISTS to 1*/
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, OR), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_NOP,CMN(NONE, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));
        /** store 0x1 in MSB of tmp-reg3, used to mask the Injected-AS-Extension-Present field*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x8, NEVER_NEVER, NXT_JMP));
        /** write 1B of the ITMH ext to buff(6 bits of the OUT_LIF)*/
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
        /** write 2B to buff last of the ITMH ext(OUT_LIF) */
        INSTR(CE1_COPY(NS_16_VAR_1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
        /** mask the MS 3 bits of the VSI  and store it in tmp_reg2 */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0x7, NEVER_NEVER, LOP_NXT));
        /** mask the LS 15 bits of the VSI  and store it in tmp_reg3 */
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_16_VAR_0, PRG_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        /** Check if the packet is with ASE header, if so jump*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_ASE_SELF_CONTAINED], NEVER_NEVER, NXT_JMP));
        /** NOPs because of the Jump*/
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        /*
         * Case packet is without ASE header
         */
        /** write 2B 0s first 2 of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
        /** write 1B of the PPH to buff(3 bits of the VSI)*/
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** write 2B of the PPH to buff(15 bits of the VSI)*/
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
        /** store 0x1 in MSB of tmp-reg3, used to update the Parsing-Start-Type and Forwarding-Strength field*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG1), CMN(NONE, NONE, 0x3, NEVER_NEVER, NXT_JMP));
        /** write 4B 0s of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        /** write 2B 0s (10 -11B) of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
        /** write 1B last 1 of the PPH header, set Parsing-Start-Type and Forwarding-Strength to 1 */
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** write 1B 0s UDH */
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /*
         * store the LS 2B of the PROG_VAR to tmp_reg2
         * This is used to check if the oamp_pe_down_mep_inj_self_contained_program is loaded.
         * Both program have different PE_CONST values.
         */
        INSTR(CE1_NOP, CE2_COPY(PROG_VAR, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        /** store 0x1 in tmp_reg3, used to chekc if we are in case of short/long group */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP));
        /** copy 1B shifted with 3 first 1B of the ETH header*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
        /** copy PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        /** if 0x1(tmp_reg3) - PE_CONST LS 2B(tmp_reg2) == POS jump to DNX_OAMP_PE_INST_LABEL_MAID_48 if not copy the rest*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG2, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
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
        /** copy 4B 2-5B of the ASE header*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B last 1B of the ASE header*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** write 2B 0s first 2 of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
        /** write 1B of the PPH to buff(3 bits of the VSI)*/
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** write 2B of the PPH to buff(15 bits of the VSI)*/
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
        /** store 0x1 in MSB of tmp-reg3, used to update the Parsing-Start-Type and Forwarding-Strength field*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG1), CMN(NONE, NONE, 0x3, NEVER_NEVER, NXT_JMP));
        /** write 4B 0s of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        /** write 2B 0s (10 -11B) of the PPH header*/
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, NONE), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
        /** write 1B last 1 of the PPH header, set Parsing-Start-Type and Forwarding-Strength to 1 */
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** write 1B 0s UDH */
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /*
         * store the LS 2B of the PROG_VAR to tmp_reg2
         * This is used to check if the oamp_pe_down_mep_inj_self_contained_program is loaded.
         * Both program have different PE_CONST values.
         */
        INSTR(CE1_NOP, CE2_COPY(PROG_VAR, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        /** store 0x1 in tmp_reg3, used to chekc if we are in case of short/long group */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP));
        /** copy 3B shifted with 1 first 3B of the ETH header*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
        /** copy PDU offset + 80(prefix) + 10(OAM) - 8(loop on temp_reg uses extra 2 intructions) to temp_reg1 */
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        /** if 0x1(tmp_reg3) - PE_CONST LS 2B(tmp_reg2) == POS jump to DNX_OAMP_PE_INST_LABEL_MAID_48 if not copy the rest*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG2, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
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
*  BFDoIPv6 program loading.
*  The program gets the packet with prefix of 80B that are not sent out.
*  Need to update the PPH field FWD_CODE to be 3 TxPphParsingStartTypeIpv6
*  Need to place first 48B of prefix instead of the ones in packet starting from system headers (IPv6 and UDP).
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
         * The program stores 48 bytes in feedback fifo and the rest 32 are dropped.
         * The program fill IPv6  and UDP headers with 48 bytes from the additional data and set the PPH field FWD_CODE.
         *
         * The packets that are supported by the program are (UDH of different size can be added to each packet):
         * INPUT
         * BFDoIPv4:  BFD(24B) o UDP(8B) o IPv4(20B) o [UDH(0-17B)] o PPH(7B) o ITMH-ext(3B) o ITMH(4B) o PTCH-2(2B)
         * OUTPUT
         * BFDoIPv6:  BFD(24B) o UDP(8B) o IPv6(40B) o [UDH(0-17B)] o PPH(7B) o ITMH-ext(3B) o ITMH(4B) o PTCH-2(2B)
         */

        /*
         * Copy 48B from additional data to buffer.
         * Drop 32B garbage.
         */
        /* *INDENT-OFF* */
        /** temp_reg3=3 used to mask the ls two bits of the FWD_CODE field of the PPH */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 3, NEVER_NEVER, NXT_JMP));
        /** copy 48B from addition data into FDBK fifo (40B IPV6 8B UDP)*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** drop 32B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));
        /** copy system headers till the first 1B of PPH header(PTCH(2B)+ITMH(4B)+EXT(3B)-1 )*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 2, ALWAYS_NEVER, LOP_CST));
        /** copy 2B from in_fifo and mask the ls two bits of the FWD_CODE field of the PPH */
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, OR), CE2_COPY(IN_FIFO, DEFAULT, NONE, 2B), ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /** copy 2B(PPH) shifted with 2 and drop 2B of the 1b EXT and 1B PPH(already copied)*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        /** copy 4B last 4 of the (PPH)*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        /**handle UDH missalignment*/
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
         * 0B and drop last 4 of the IPv4
         * 1B UDH and drop last 1B of the IPv4
         * 0B and drop 4B IPv4
         * 3B UDH and drop last 3B of the IPv4
         */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        /*
         * Copy IPv6 and UDP from FDBF and drop IPv4.
         */
        /** drop 24B IPv4 and UDP*/
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 6, NEVER_NEVER, LOP_CST));
        /** copy 48B from FDBK fifo IPv6 and UDP*/
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 12, ALWAYS_NEVER, LOP_CST));

        switch (udh_header_size)
        {
            case 0:
                /** 0B misalignment*/
                /*
                 * COPY PDU
                 */
                /** copy rest packet */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
                break;
            case 1:
                /** shift 1B write 3B*/
                instr_const = 0x13;
                break;
            case 2:
                /** shift 2B write 2B*/
                instr_const = 0x22;
                break;
            case 3:
                /** shift 3B write 1B*/
                instr_const = 0x31;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }

        /**CASE UDH(1B) misalignment:  instr_const = 0x13: copy 3B(BFD) shifted with 1 and drop last 1B of the UDP*/
        /**CASE UDH(2B) misalignment:  instr_const = 0x22: copy 2B(BFD) shifted with 2 and drop last 2B of the UDP*/
        /**CASE UDH(3B) misalignment:  instr_const = 0x31: copy 1B(BFD) shifted with 3 and drop last 3B of the UDP*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        /*
         * COPY PDU
         */

        /** BFD MEP type discriminator feature */

        if(dnx_data_bfd.property.discriminator_type_update_get(unit) == 1)
        {
            /** temp_reg3=0 used to compare the result */
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));

            /** Get the bit that determines if discr MSB=1 */
            INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0x4, NEVER_NEVER, LOP_NXT));

            /** Write the target byte offset to temp_reg1 */
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, ((udh_header_size + 2) & 3), NEVER_NEVER, NXT_JMP));

            /** Wait for temp_reg2 to be set */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            /** Check if the temp_reg2==0, if so jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_END], NEVER_NEVER, NXT_JMP));

            /** How many dwords before the one that contains the byte we want? */
            if(udh_header_size == 2)
            {
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 7, ALWAYS_NEVER, LOP_CST));
            }

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            /* check if function was already loaded is so jump with two nops*/
            if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
            {
                /** Jump to oamp_pe_bfd_discr_msb_1_program */
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
        }

        /** copy rest packet */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_END);
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
*  Need to place first 48B of prefix instead of the ones in packet starting from system headers (IPv6 and UDP).
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
    int instr_const;
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

        /*
         * Copy 48B from additional data to buffer.
         * Drop 32B garbage.
         */
        /* *INDENT-OFF* */
        /** temp_reg3=6 used to mask the ls two bits of the Parsing-Start-Type field of the PPH */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 6, NEVER_NEVER, NXT_JMP));
        /** copy 48B from addition data into FDBK fifo (40B IPV6 8B UDP)*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** drop 32B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));
        /** copy system headers till the last 2B of PPH header(PTCH(2B)+ITMH(5B)+EXT(3B)+PPH(12B)-2 )*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 5, ALWAYS_NEVER, LOP_CST));
        /** mask the ls two bits of the Parsing-Start-Type field of the PPH */
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, OR), CE2_COPY(IN_FIFO, DEFAULT, NONE, 2B), ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        /*
         * All system headers are written to buffer
         * UDH size misalignment follows
         */
        if (udh_header_size < 2)
        {
            switch (udh_header_size)
            {
                case 0:
                    /*
                     * CASE 0B udh
                     */
                    /** drop 4B, 2B pph(copied) and first 2B of the IPv4*/
                    INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, NONE), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
                    break;
                case 1:
                    /*
                     * CASE 1B udh
                     */
                    /** copy 1B(udh) shifted with 2 and drop 2B PPH(copied) 1B UDH and first byte of the IPv4*/
                    INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, NONE), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
            }
        }
        else
        {
            /** copy 2B(UDH) shifted with 2 and drop last 2B of the PPH(copied)*/
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
            while (udh_header_size > 4)
            {
                /** copy 4B(UDH)*/
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
                udh_header_size -= 4;
            }
            /*
             * udh_header_size_misalignment = udh - 2(2B already copied)
             */
            switch (udh_header_size)
            {
                case 4:
                    instr_const = 0x2;
                    break;
                case 1:
                    instr_const = 0x3;
                    break;
                case 2:
                    instr_const = 0x0;
                    break;
                case 3:
                    instr_const = 0x1;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
            }
            /**Copy
                2B UDH and drop last 2B of the IPv4
                1B UDH and drop last 1B of the IPv4
                0B and drop 4B IPv4
                3B UDH and drop last 3B of the IPv4
             */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        }
        /*
         * Copy IPv6 and UDP from FDBF and drop IPv4.
         */
        /** copy 48B from FDBK fifo IPv6 and UDP*/
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 12, ALWAYS_NEVER, LOP_CST));
        if (udh_header_size != 2)
        {
            /** drop 24B IPv4 and UDP*/
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 6, NEVER_NEVER, LOP_CST));
            switch (udh_header_size)
            {
                case 4:
                    /** shift 2B write 2B*/
                    instr_const = 0x22;
                    break;
                case 1:
                    /** shift 3B write 1B*/
                    instr_const = 0x31;
                    break;
                case 3:
                    /** shift 1B write 3B*/
                    instr_const = 0x13;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
            }
            /**CASE UDH(0B) misalignment:  instr_const = 0x22: copy 2B(BFD) shifted with 2 and drop last 2B of the UDP*/
            /**CASE UDH(1B) misalignment:  instr_const = 0x31: copy 1B(BFD) shifted with 3 and drop last 3B of the UDP*/
            /**CASE UDH(3B) misalignment:  instr_const = 0x13: copy 3B(BFD) shifted with 1 and drop last 1B of the UDP*/
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        }
        /*
         * COPY PDU
         */

        /** BFD MEP type discriminator feature */

        if(dnx_data_bfd.property.discriminator_type_update_get(unit) == 1)
        {
            /** temp_reg3=0 used to compare the result */
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));

            /** Get the bit that determines if discr MSB=1 */
            INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0x4, NEVER_NEVER, LOP_NXT));

            /** Write the target byte offset to temp_reg1 */
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, ((udh_header_size + 2) & 3), NEVER_NEVER, NXT_JMP));

            /** Wait for temp_reg2 to be set */
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            /** Check if the temp_reg2==0, if so jump*/
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_END], NEVER_NEVER, NXT_JMP));

            /** How many dwords before the one that contains the byte we want? */
            if(udh_header_size == 2)
            {
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 7, ALWAYS_NEVER, LOP_CST));
            }

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            /* check if function was already loaded is so jump with two nops*/
            if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
            {
                /** Jump to oamp_pe_bfd_discr_msb_1_program */
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
        }

        /** copy rest packet */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT),DNX_OAMP_PE_INST_LABEL_BFD_IPV6_END);
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
 *  Need to place first 48B of prefix instead of the ones in packet starting from system headers (IPv6 and UDP).
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
         * The program stores 48 bytes in feedback fifo and the rest 32 are dropped.
         * The program fill IPv6  and UDP headers with 48 bytes from the additional data and the LS 4 bytes of the IPv6 header are updated.
         *
         * The packets that are supported by the program are (UDH of different size can be added to each packet):
         * INPUT
         * BFDoIPv4:  BFD(24B) o UDP(8B) o IPv4(20B) o MPLS(4B) o [UDH(0-17B)] o PPH(7B) o ITMH-ext(3B) o ITMH(4B) o PTCH-2(2B)
         * OUTPUT
         * BFDoIPv6:  BFD(24B) o UDP(8B) o IPv6(40B) o MPLS(4B) o[UDH(0-17B)] o PPH(7B) o ITMH-ext(3B) o ITMH(4B) o PTCH-2(2B)
         *
         */
        /* *INDENT-OFF* */

        /** copy 48B from packet into FDBK fifo (40B IPV6 8B UDP)*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** drop 32B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));
        /** copy system headers till the MPLS header(PTCH(2B)+ITMH(4B)+EXT(3B)+PPH(7B)+(4B(UDH) or part of MPLS(4B) in case UDH < 4B) )*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 5, ALWAYS_NEVER, LOP_CST));
        /*In case the UDH size is higher than 4*/
        while (udh_header_size >= 4)
        {
            /** copy 4B(UDH)*/
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
                /** CASE 3B misalignment*/
                instr_const = 3;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }

        /**
         *  IN_FIFO:
         *      0B(MPLS) 4B IPv4
         *      1B(MPLS) 3B IPv4
         *      2B(MPLS) 2B IPv4
         *      3B(MPLS) 1B IPv4
         */
        if (udh_header_size != 0)
        {
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        }
        /** drop 16B or IPv4 */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));
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
                /*IN_FIFO: Last 3B of IPv4 and 1B UDP**/
                instr_const = 0x20;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }
        /** store the LSB of the ipv4 header to tmp_reg2*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, instr_const, NEVER_NEVER, NXT_JMP));
        /** drop 8B IPv4 till the last 4B of the IPv6*/
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, NEVER_NEVER, LOP_CST));
        /** MASK the LSB of IPv4 temp_reg2 and store it in tmp_reg2 */
        /*TMP_REG1 = 0xFF00, TMP_REG2= (LSB of IPv4 and MSB of UDP)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        /** copy 36B from FDBK fifo IPv6 */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 9, ALWAYS_NEVER, LOP_CST));
        /** Store one's compliment of LSB of IPv4 temp_reg2 and store it in tmp_reg4 */
        /*TMP_REG1 = 0xFF00, TMP_REG2= (LSB of IPv4 and 1B 0x00)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG4), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        /*IN_FIFO: 4B UDP */
        /** copy 1B 0x7f to buff(First 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B one's compliment of the LSB of IPv4 to buff(Second 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B 0x00 to buff(Third 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B the LSB of IPv4 to buff(Forth 1B of the last 4 of the IPv6 header) and drop last 4B of IPv6 header*/
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 8B from FDBK fifo UDP */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 2, ALWAYS_NEVER, LOP_CST));
        switch (udh_header_size)
        {
            case 0:
                /** copy rest packet */
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
                break;
            case 1:
                instr_const = 0x13;
                break;
            case 2:
                instr_const = 0x22;
                break;
            case 3:
                instr_const = 0x31;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }
        if (udh_header_size != 0)
        {
            /** IN_FIFO:
             *   1B UDP 3B BFD
             *   2B UDP 2B BFD
             *   3B UDP 1B BFD
             */
            /**Handle misalignment*/
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
            /** copy rest packet */
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
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
 *  Need to place first 48B of prefix instead of the ones in packet starting from system headers (IPv6 and UDP).
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
         * The program stores 48 bytes in feedback fifo and the rest 32 are dropped.
         * The program fill IPv6  and UDP headers with 48 bytes from the additional data and the LS 4 bytes of the IPv6 header are updated.
         *
         * The packets that are supported by the program are (UDH of different size can be added to each packet):
         * INPUT
         * BFDoIPv4:  BFD(24B) o UDP(8B) o IPv4(20B) o MPLS(4B) o [UDH(0-17B)] o PPH(12B) o ITMH-ext(3B) o ITMH(5B) o PTCH-2(2B)
         * OUTPUT
         * BFDoIPv6:  BFD(24B) o UDP(8B) o IPv6(40B) o MPLS(4B) o[UDH(0-17B)] o PPH(12B) o ITMH-ext(3B) o ITMH(5B) o PTCH-2(2B)
         *
         */
        /* *INDENT-OFF* */
        /** copy 48B from packet into FDBK fifo (40B IPV6 8B UDP)*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        /** drop 32B */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));
        /** copy system headers till the MPLS header(PTCH(2B)+ITMH(5B)+EXT(3B)+PPH(12B)+(2B(UDH) or part of MPLS(4B) in case UDH is '0' or '1'B)-2 )*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 6, ALWAYS_NEVER, LOP_CST));
        /*In case the UDH size is higher than 4*/
        while (udh_header_size >= 4)
        {
            /** copy 4B(UDH)*/
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            udh_header_size -= 4;
        }
        /** store 0xFF to tmp_reg1 used to mask the IPv4 LSB*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG1), CMN(NONE, NONE, 0xFF, NEVER_NEVER, NXT_JMP));
        switch (udh_header_size)
        {
            case 0:
                /** CASE 2B misalignment*/
                instr_const = 2;
                break;
            case 1:
                /** CASE 3B misalignment*/
                instr_const = 3;
                break;
            case 2:
                /**CASE 0B misalignment*/
                instr_const = 4;
                break;
            case 3:
                /** CASE 1B misalignment*/
                instr_const = 1;
                /** copy 1B UDH and 3B(MPLS)*/
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }

        /* copy
         *   2B(MPLS) and drop first 2B of the IPv4
         *   3B(MPLS) and drop first 1B of the IPv4
         *   4B(MPLS)
         *   1B(MPLS) and drop first 3B of the IPv4
         */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        /* drop 16B IPv4 till the last
         *       2B of the IPv4
         *       3B of the IPv4
         *       4B of the IPv4
         *       1B of the IPv4
         */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));

        switch (udh_header_size)
        {
            case 0:
                instr_const = 0x10;
                break;
            case 1:
                instr_const = 0x20;
                break;
            case 2:
                instr_const = 0x30;
                break;
            case 3:
                instr_const = 0;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }
        /** store the LSB of the ipv4 header to tmp_reg2*/
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, instr_const, NEVER_NEVER, NXT_JMP));
        /** drop 8B IPv4 till the last 4B of the IPv6*/
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, NEVER_NEVER, LOP_CST));
        /** MASK the LSB of IPv4 temp_reg2 and store it in tmp_reg2 */
        /*TMP_REG1 = 0xFF00, TMP_REG2= (LSB of IPv4 and MSB of UDP)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        /** copy 36B from FDBK fifo IPv6 */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 9, ALWAYS_NEVER, LOP_CST));
        /** Store one's compliment of LSB of IPv4 temp_reg2 and store it in tmp_reg4 */
        /*TMP_REG1 = 0xFF00, TMP_REG2= (LSB of IPv4 and 1B 0x00)*/
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG4), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        /*IN_FIFO: 4B UDP */
        /** copy 1B 0x7f to buff(First 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B one's compliment of the LSB of IPv4 to buff(Second 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B 0x00 to buff(Third 1B of the last 4 of the IPv6 header)*/
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
        /** copy 1B the LSB of IPv4 to buff(Forth 1B of the last 4 of the IPv6 header) and drop last 4B of IPv6 header*/
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP));
        /** copy 8B from FDBK fifo UDP */
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 2, ALWAYS_NEVER, LOP_CST));
        switch (udh_header_size)
        {
            case 0:
                instr_const = 0x22;
                break;
            case 1:
                instr_const = 0x31;
                break;
            case 2:
                instr_const = 0x0;
                break;
            case 3:
                instr_const = 0x13;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }
        /* IN_FIFO:
         *   2B UDP 2B BFD
         *   3B UDP 1B BFD
         *   4B UDP
         *   1B UDP 3B BFD
         */
        /**Handle misalignment*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
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
        +-------------------------------------------------------+-----------------+
        | All programs that use this function                   | TMP_REG1        |
        |-------------------------------------------------------+-----------------+
        | oamp_pe_bfd_discr_msb_1_program                       | Should store the|
        |-------------------------------------------------------+ byte offset     +
        |                                                       | (0-3) to the MSB|
        |-------------------------------------------------------+ of the          +
        |                                                       | discriminator   |
        |-------------------------------------------------------+                 +
        |                                                       |                 |
        +-------------------------------------------------------+-----------------+
         */

        /** set temp_reg3=1, for alignment validation */
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1);
        /** set temp_reg4=2, for alignment validation */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 2, NEVER_NEVER, NXT_JMP));
        /** set temp_reg4=3, for alignment validation */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 3, NEVER_NEVER, NXT_JMP));
        /** set jump for target byte is second to most significant byte */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1_1B], NEVER_NEVER, NXT_JMP));
        /** set jump for target byte is second to least significant byte */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1_2B], NEVER_NEVER, NXT_JMP));
        /** set jump for target byte is least significant byte */
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1_3B], NEVER_NEVER, NXT_JMP));

        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /*
        * Target byte is most significant byte case
        */
        /** Copy next 2 bytes to tmp_reg1 */
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));

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

        /** copy rest of packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /*
        * Target byte is second to most significant byte case
        */

        /** Copy next 2 bytes to tmp_reg1 */
        INSTR_LABEL(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1_1B);

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

        /** copy rest of packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /*
        * Target byte is second to least significant byte case
        */

        /** Copy two bytes */
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1_2B);

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

        /** copy rest of packet */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        /*
        * Target byte is least significant byte case
        */

        /** Copy lower word (16B) of current dword (32B) to temp_reg1 */
        INSTR_LABEL(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1_3B);

        /** Copy upper 3 bytes to buffer*/
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

        /** temp_reg2=temp_reg1 | mask */
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, LOP_NXT));

        /** NOP for the value to be ready */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /** copy result to buffer */
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        /** copy rest of packet */
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
oamp_pe_bfd_discr_msb_1_program(
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
    int udh_header_size = 0, discriminator_offset, dwords_to_discr, rem_bytes_to_discr;
    uint32 saved_prog_index;

    /** instr_entry_handle_id is dedicated for 'INSTR' macro only */
    uint32 instr_entry_handle_id;

    /** Differntiation between JR1 and JR2 modes */
    uint8 oamp_tx_format, jr1_format;

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
        if(oamp_tx_format == jr1_format)
        {
            /** JR1 headers */
            discriminator_offset = 49 + udh_header_size;
        }
        else
        {
            /** JR2 headers */
            discriminator_offset = 54 + udh_header_size;
        }

        /** Calculate the number of DWORDs to copy in the beginning */
        dwords_to_discr = discriminator_offset >> 2;

        /** Calculate remaining offset to discriminator MSB */
        rem_bytes_to_discr = (discriminator_offset & 0x3);

        /*
         * configure set of requested instructions
         */

        /* *INDENT-OFF* */
        /** store offset in LSB of tmp-reg1, used to mask select the correct byte to edit */
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, rem_bytes_to_discr, NEVER_NEVER, NXT_JMP));

        /** copy whole dwords {PTCH, ITMH, ITMH-EXT, PPH, UDH, IPv4, UDP, start of BFD (until my-discriminator)} */
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, dwords_to_discr, ALWAYS_NEVER, LOP_CST));

        /** NOPs for the temp_reg1 value */
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        /* check if function was already loaded is so jump with two nops*/
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            /** Jump to oamp_pe_bfd_discr_msb_1_program */
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
         */

        /** fem_id                  bit_0      bit_1      bit_2       bit_3       bit_4     bit_5     bit_6       bit_7       bit_8     bit_9     bit_10       bit_11     bit_12      bit_13      bit_14      bit_15       valid   hw_index*/
        /** default FEM - is set by default in HW, present here for visualization only*/
        {DNX_OAMP_PE_FEM_TAKE_ORIG, {{OFST, 0}, {OFST, 1}, {OFST, 2}, {OFST, 3}, {OFST, 4}, {OFST, 5}, {OFST, 6}, {OFST, 7}, {OFST, 8}, {OFST, 9}, {OFST, 10}, {OFST, 11}, {OFST, 12}, {OFST, 13}, {OFST, 14}, {OFST, 15}},   0,    DNX_OAMP_PE_NOT_SET}
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
    dnx_oamp_pe_mep_pe_profiles_e profile_id)
{
    return 0;
}

/**
* \brief
*  Initialize MEP_PE_PROFILE HW and SW structures with predefined values to be set later.
*   \param [in] unit  -  Relevant unit.
*   \param [out] mep_pe_profiles  -  MEP_PE_PROFILE structure including all information
*   \param [out] mep_pe_profile_sw  -  mapping of SW MEP-PE profile naming to HW
* \return
*   shr_error_e
*/
static shr_error_e
dnx_oamp_pe_mep_pe_profile_init(
    int unit,
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DNX_OAMP_PE_MEP_PE_PROFILE_LAST],
    int mep_pe_profile_sw[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES][2])
{
/* *INDENT-OFF* */
dnx_oamp_mep_pe_profile_s mep_pe_profiles_local[DNX_OAMP_PE_MEP_PE_PROFILE_LAST] = {

    /** 
        * THESE HAVE TO BE IN THE SAME ORDER AS THE "DNX_OAMP_PE_MEP_PE_PROFILE_" enum! 
        * The enumeration value at the start of the line is used to validate this. 
        */
    /** profile_id                                            BYTE_0_OFFSET   BYTE_1_OFFSET   WORD_0_OFFSET   WORD_1_OFFSET     MEP_INSERT   valid   hw_index*/

    /** default mep pe profile */
    {DNX_OAMP_PE_MEP_PE_PROFILE_DEFAULT,                            0,              0,              0,              0,              0,          1,   0},
    {DNX_OAMP_PE_MEP_PE_PROFILE_BFD_ECHO,                           0,              0,              0,              0,              0,          1,   1},
    {DNX_OAMP_PE_MEP_PE_PROFILE_48B_MAID,                           0,              0,              0,              0,              0,          1,   2},
    {DNX_OAMP_PE_MEP_PE_PROFILE_BFD_DISCR_MSB_1,                    0,              0,              0,              0,              0,          1,   3},
    {DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6,                           27,             0,              0,              0,              0,          1,   4},
    {DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6_DISCR_MSB_1,               27,             0,              0,              0,              0,          1,   5},
    {DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6_MPLS,                      27,             0,              0,              0,              0,          1,   6},
    {DNX_OAMP_PE_MEP_PE_PROFILE_RESERVED,                           0,              0,              0,              0,              0,          1,   7},
    {DNX_OAMP_PE_MEP_PE_PROFILE_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,48,             27,             50,             29,             0,          1,   8},
    {DNX_OAMP_PE_MEP_PE_PROFILE_DOWN_MEP_INJ_SELF_CONTAINED,        48,             27,             50,             29,             0,          1,   9},
};

int mep_pe_profile_sw_local[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES][2] = {
    /** key - dbal_enum_value_field_mep_pe_profile_sw_e                          value - mep_pe_profile dnx_oamp_pe_mep_pe_profiles_e */
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT,                                    DNX_OAMP_PE_MEP_PE_PROFILE_DEFAULT},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO,                                   DNX_OAMP_PE_MEP_PE_PROFILE_BFD_ECHO},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48,                                    DNX_OAMP_PE_MEP_PE_PROFILE_48B_MAID},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DISCR_MSB_1,                            DNX_OAMP_PE_MEP_PE_PROFILE_BFD_DISCR_MSB_1},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6,                                   DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_DISCR_MSB_1,                       DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6_DISCR_MSB_1},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS,                              DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6_MPLS},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_RESERVED,                                   DNX_OAMP_PE_MEP_PE_PROFILE_RESERVED},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,        DNX_OAMP_PE_MEP_PE_PROFILE_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED,                DNX_OAMP_PE_MEP_PE_PROFILE_DOWN_MEP_INJ_SELF_CONTAINED},
};
    /* *INDENT-ON* */
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Update predifined values based on configuration
     */

    /*
     * go through all MEP_PE_PROFILEs and unset 'valid' if needed. 
     * 'hw_index' is manually managed due to use limitations.
     */
    for (ii = 0; ii < DNX_OAMP_PE_MEP_PE_PROFILE_LAST; ii++)
    {
        if ((mep_pe_profiles_local[ii].valid) && dnx_oamp_pe_mep_pe_profile_soc_disable(unit, ii))
        {
            mep_pe_profiles_local[ii].valid = 0;
        }
    }

    /*
     * Copy return values and perform result basic sanity
     */
    for (ii = 0; ii < DNX_OAMP_PE_MEP_PE_PROFILE_LAST; ii++)
    {
        if (mep_pe_profiles_local[ii].profile_id != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "mep_pe_profile_actions at index %d is out of order.\n", ii);
        }
    }

    /** copy final MEP_PE_PROFILE configuration to be returned */
    sal_memcpy(mep_pe_profiles, mep_pe_profiles_local, sizeof(mep_pe_profiles_local));

    /** copy final MEP_PE_PROFILE_SW configuration to be returned */
    for (ii = 0; ii < DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES; ii++)
    {
        if (mep_pe_profile_sw_local[ii][0] != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "mep_pe_profile_sw_local at index %d is out of order.\n", ii);
        }
        mep_pe_profile_sw[ii][0] = mep_pe_profile_sw_local[ii][0];
        mep_pe_profile_sw[ii][1] = mep_pe_profile_sw_local[ii][1];
    }

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
* \return
*   return 0 if TCAM should not be loaded.
*/
static int
dnx_oamp_pe_tcam_soc_enable(
    int unit,
    oamp_pe_tcam_config_s tcam_config_local[DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES],
    dbal_enum_value_field_oamp_pe_prog_tcam_enum_e tcam_id)
{
    int system_headers_mode;

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    switch (tcam_id)
    {
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED:
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48:
            break;
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
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48:
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DISCR_MSB_1:
            tcam_config_local[tcam_id].valid = dnx_data_bfd.property.discriminator_type_update_get(unit);
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
        /** tcam_id                                                                 program_ptr                                           valid   program_id                                                             program_const                       key_type                        key                                                                                                                                                              hw_index*/
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED,         oamp_pe_down_mep_inj_self_contained_program,          1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED,        0xfffe0001,                         OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM,       0x0, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_DOWN_MEP_INJ_SELF_CONTAINED,         0x0, DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR,  0xF}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48, oamp_pe_down_mep_inj_self_contained_maid_48_program,  1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,0xfffe0000,                         OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM,       0x0, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48, 0x0, DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR,  0xF}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO_JR1,                        oamp_pe_bfd_echo_jr1_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO_JR1,                       DNX_OAMP_PE_BFD_ECHO_UDP_DPORT,     OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP, 0x0, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_BFD_ECHO,                            0x0, DBAL_ENUM_FVAL_MSG_TYPE_OTHER,            0x0}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS_JR1,                   oamp_pe_bfd_ipv6_mpls_jr1_program,                    1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS_JR1,                  DNX_OAMP_PE_BFD_IPV6_MPLS_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS,      0x0, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6_MPLS,                       0x0, DBAL_ENUM_FVAL_MSG_TYPE_OTHER,            0x0}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_JR1,                        oamp_pe_bfd_ipv6_jr1_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_JR1,                       0,                                  OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP, 0xF, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6,                            0x1, DBAL_ENUM_FVAL_MSG_TYPE_OTHER,            0x0}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS,                       oamp_pe_bfd_ipv6_mpls_program,                        1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS,                      DNX_OAMP_PE_BFD_IPV6_MPLS_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS,      0x0, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6_MPLS,                       0x0, DBAL_ENUM_FVAL_MSG_TYPE_OTHER,            0x0}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6,                            oamp_pe_bfd_ipv6_program,                             1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6,                           0,                                  OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP, 0xF, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_BFD_IPV6,                            0x1, DBAL_ENUM_FVAL_MSG_TYPE_OTHER,            0x0}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48,                             oamp_pe_maid_48_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48,                            0,                                  OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM,       0x3, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_48B_MAID,                        0x0, DBAL_ENUM_FVAL_MSG_TYPE_OTHER,           0x0}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO,                            oamp_pe_bfd_echo_program,                             1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO,                           DNX_OAMP_PE_BFD_ECHO_UDP_DPORT,     OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP, 0x0, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_BFD_ECHO,                            0x0, DBAL_ENUM_FVAL_MSG_TYPE_OTHER,            0x0}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DISCR_MSB_1,                     oamp_pe_bfd_discr_msb_1_program,                      1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_DISCR_MSB_1,                    0,                                  OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP, 0xF, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_BFD_DISCR_MSB_1,                     0x0, DBAL_ENUM_FVAL_MSG_TYPE_OTHER,            0x0}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DEFAULT,                             oamp_pe_default_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DEFAULT,                            0,                                  OAMP_PE_TCAM_KEY_MEP_GENERATED, {{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM,       0xF, 0, 0x7F, DNX_OAMP_PE_MEP_PE_PROFILE_DEFAULT,                             0x3F, DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR, 0xF}},DNX_OAMP_PE_NOT_SET}
    };
    /* *INDENT-ON* */
    int ii, jj;

    SHR_FUNC_INIT_VARS(unit);
    /*
     *  Update predifined values based on configuration
     */
    for (ii = 0, jj = 0; ii < DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES ; ii++)
    {
        if (dnx_oamp_pe_tcam_soc_enable(unit, tcam_config_local, ii))
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
        /** func_id                                                       func_ptr                                                           first_instruction */
        {DNX_OAMP_PE_FUNC_DEFAULT,                                        oamp_pe_function_default,                                          DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_COPY_MAID_48,                                   oamp_pe_function_copy_48_maid,                                     DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED, oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained,   DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_DISCR_MSB_1,                                    oamp_pe_function_discr_msb_1,                                      DNX_OAMP_PE_NOT_SET}
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
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DNX_OAMP_PE_MEP_PE_PROFILE_LAST];
    int mep_pe_profile_sw[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES][2];
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
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_init(unit, mep_pe_profiles, mep_pe_profile_sw));
    /** initialize TCAM values */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_tcam_init(unit, tcam_config));
    /** initialize function values */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_func_init(unit, func_config));

    /** initialize MEP_PE profiles */
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_set(unit, mep_pe_profiles));
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_set(unit, mep_pe_profile_sw, mep_pe_profiles));

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
    for (ii = DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES - 1; ii > -1 ; ii--)
    {
        if (tcam_config[ii].valid)
        {
            SHR_IF_ERR_EXIT(tcam_config[ii].program_ptr
                            (unit, DNX_OAMP_PE_LABEL_MODE_ON, tcam_config[ii].program_id, tcam_config[ii].program_const,
                             inst_labels, &next_instruction_index, &next_valid_program_index, &prog_index, func_config));
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
    for (ii = DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES - 1; ii > -1 ; ii--)
    {
        if (tcam_config[ii].valid)
        {
            SHR_IF_ERR_EXIT(tcam_config[ii].program_ptr
                            (unit, DNX_OAMP_PE_LABEL_MODE_OFF, tcam_config[ii].program_id, tcam_config[ii].program_const,
                             inst_labels, &next_instruction_index, &next_valid_program_index, &prog_index, func_config));
            if (prog_index != DNX_OAMP_PE_NOT_SET)
            {
                /** configure TCAM entry for program selection */
                SHR_IF_ERR_EXIT(dnx_oamp_pe_programs_tcam_set(unit, tcam_config[ii], prog_index));
            }
        }
    }

    /**
     * These profiles have the same program and the same code.  They must
     * have the same value except for the different LSB for this feature
     * to work!
     */
    if((UTILEX_GET_BIT(DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6, 0) != 0) ||
       (DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_DISCR_MSB_1 !=
        (DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6 | 1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MEP PE profiles pairs that support the EP type in discriminator feature must\n"
                "have the same values except for a different LSB -> the LSB for discr MSB=0 must be 0, and\n"
                "the LSB for discr MSB=1 must be 1.\n");
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
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

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
