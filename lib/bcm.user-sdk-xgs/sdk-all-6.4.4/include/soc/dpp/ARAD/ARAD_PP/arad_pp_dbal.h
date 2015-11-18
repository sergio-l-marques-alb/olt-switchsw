/* $Id: dpp_dbal.c, v 1.95 Broadcom SDK $
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
 * $
*/

#ifndef __ARAD_PP_DBAL_INCLUDED__
#define __ARAD_PP_DBAL_INCLUDED__


#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_ce.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_db.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_pgm.h>
#include <soc/dpp/ARAD/arad_sw_db.h>

#define DBAL_MAX_PROGRAMS_PER_TABLE         10
#define DBAL_MAX_NAME_LENGTH                30

#define DBAL_QUAL_INFO_CLEAR(qual_info, nof_elements)   sal_memset(qual_info, 0, sizeof(SOC_DPP_DBAL_QUAL_INFO)*nof_elements)

/*enum that represents all the existing tables for each new table a value should be added */
typedef enum
{
    SOC_DPP_DBAL_SW_TABLE_ID_INVALID = -1,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MPLS_EXTENDED,    
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_EXTENDED,
    SOC_DPP_DBAL_SW_TABLE_ID_P2P_EXTENDED,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FLEXIBLE, 
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS,
	SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS,
	SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS,
	SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOWN_UNTAGGED_STATISTICS,
	SOC_DPP_DBAL_SW_TABLE_ID_OAM_SINGLE_TAG_STATISTICS,
	SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOUBLE_TAG_STATISTICS,


/*         VTT DB                   */
    SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_UNINDEXED_PREFIX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_UNINDEXED_PREFIX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv4_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_COMPRESSED_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID1_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_QINANY_TPID1_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_QINANY_TPID2_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_QINANY_PCP_TPID1_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_QINANY_PCP_TPID2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_TPID1_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_TPID2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID1_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID2_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_PCP_COMPRESSED_TPID1_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_PCP_COMPRESSED_TPID2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST = SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM,

/*       END VTT DB                 */

    SOC_DPP_DBAL_SW_NOF_TABLES

}SOC_DPP_DBAL_SW_TABLE_IDS;

/* enum that represents all the physical database existing */
typedef enum
{
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS,

    SOC_DPP_DBAL_PHYSICAL_DB_NOF_TYPES

}SOC_DPP_DBAL_PHYSICAL_DB_TYPES;

typedef enum
{
    SOC_DPP_DBAL_PROGRAM_KEY_A,
    SOC_DPP_DBAL_PROGRAM_KEY_B,
    SOC_DPP_DBAL_PROGRAM_KEY_C,
    SOC_DPP_DBAL_PROGRAM_KEY_D,

    SOC_DPP_DBAL_PROGRAM_NOF_KEYS

}SOC_DPP_DBAL_PROGRAM_KEYS;

typedef struct
{
    SOC_PPC_FP_QUAL_TYPE    qual_type;      /* if qualifier is in header (offset+nof_bits) has to be nibble aligned(16bit instruction) or byte aligned(32bit instruction)  */
    uint8                   qual_offset;    /* if zero, starts from the LSB*/
    uint8                   qual_nof_bits;  /* if zero full length will be used. MAX length must be 32, if bigger new qualifier is needed */
    uint8                   qual_full_size; /* The full size of the qualifier */
    uint8                   qual_is_in_hdr; /* if zero the qualifier is in the internal fields, else is in the header packet */
}SOC_DPP_DBAL_QUAL_INFO;

/* struct that pers table ID to key number */
typedef struct
{
    uint32 key_id;
    SOC_DPP_DBAL_SW_TABLE_IDS sw_table_id;
  
}SOC_DPP_DBAL_KEY_TO_TABLE;

typedef struct
{
    ARAD_FP_DATABASE_STAGE  stage;
    uint32                  program_id;/* program ID refers to the cam selection line*/
    uint32                  key_id;
    uint8                   ce_assigned[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]; /* which CE assigned to each qualifier */
}DBAL_PROGRAM_INFO;

/* table info */
typedef struct
{
    uint8                           is_table_initiated;
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES  physical_db_type;
    uint32                          db_prefix; /* in KBP equals to frwrd_table_id*/
    uint32                          db_prefix_len; /* prefix length in bits */
    SOC_DPP_DBAL_QUAL_INFO          qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int                             nof_qualifiers;
    uint32                          additional_table_info; /* table extra information that is needed. for LEM/SEM/TCAM tables it is used for key type */
    char                            table_name[DBAL_MAX_NAME_LENGTH];
    DBAL_PROGRAM_INFO               table_programs[DBAL_MAX_PROGRAMS_PER_TABLE];
    int                             nof_table_programs;
    uint32                          nof_entries_added_to_table;
} SOC_DPP_DBAL_TABLE_INFO;


/*********** GENERAL FUNCTIONS ***********/
uint32 arad_pp_dbal_init(int unit);
uint32 arad_pp_dbal_deinit(int unit);
uint32 arad_pp_dbal_ce_info_get(int unit, uint32 table_id, ARAD_FP_DATABASE_STAGE stage, ARAD_SW_DB_PMF_CE *ce_array, uint8 *nof_ces, uint8 *is_key_320b, uint8 *ces_ids);
uint32 arad_pp_dbal_db_predfix_get(int unit, uint32 table_id, uint32* db_prefix);

/*********** TABLE FUNCTIONS ***********/
uint32 arad_pp_dbal_table_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 db_prefix, uint32 db_prefix_len,
                                  SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db, int nof_qualifiers, uint32 additional_info,
                                  SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], char* table_name);

uint32 arad_pp_dbal_table_clear(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);

/********* PROGRAM FUNCTIONS *********/
uint32 arad_pp_dbal_program_to_tables_associate(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, 
                                                SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS], uint8 use_32_bit_ce[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] [SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], int nof_valid_keys);

uint32 arad_pp_dbal_program_to_tables_associate_implicit(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, 
                                                         SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS],
                                                         uint8 qualifier_to_ce_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS][SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                                         int nof_valid_keys);

uint32 arad_pp_dbal_table_physical_db_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES* physical_db_type );

/*********** ENTRY MANAGMENT FUNCTION ***********/
uint32 arad_pp_dbal_entry_add(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, void* payload, SOC_SAND_SUCCESS_FAILURE *success);
uint32 arad_pp_dbal_entry_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found);
uint32 arad_pp_dbal_entry_delete(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success);


/* functions used to managment entries by ID */
uint32 arad_pp_dbal_entry_add_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, ARAD_TCAM_ACTION* payload, uint8 is_for_update, SOC_SAND_SUCCESS_FAILURE *success);
uint32 arad_pp_dbal_entry_get_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found);
uint32 arad_pp_dbal_entry_delete_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success);


/*********** DIAGNOSTIC FUNCTIONS ***********/
uint32 arad_pp_dbal_last_packet_dump(int unit);
uint32 arad_pp_dbal_ce_per_program_dump(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage);
uint32 arad_pp_dbal_table_info_dump(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_tables_dump(int unit, int is_full_info);
uint32 arad_pp_dbal_entry_dump(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]);
const char* arad_pp_dbal_physical_db_to_string(SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type);


/*********** QUALIFIER MANAGMENT ***********/

/*qual_index is reset to 0 when it returns*/
#define DBAL_QUAL_VALS_CLEAR(qual_vals)                                                                                      \
    {uint32 qual_index;                                                                                                      \
        for (qual_index = 0; qual_index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++qual_index)                                     \
        {                                                                                                                    \
            SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[qual_index]);                                                               \
        }                                                                                                                    \
    }

#define DBAL_QUAL_VAL_ENCODE_VRF(qual_val, vrf_ndx)                                                                          \
    *qual_val.type = SOC_PPC_FP_QUAL_IRPP_VRF;                                                                               \
    *qual_val.val.arr[0] = vrf_ndx;                                                                                          \
    *qual_val.is_valid.arr[0] = SOC_SAND_U32_MAX;

#define DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len)                                                          \
    *qual_val.val.arr[0] = ip_address;                                                                                       \
    /*In case route_key->prefix_len is 0 then is_valid is also 0*/                                                           \
    *qual_val.is_valid.arr[0] = prefix_len ? (SOC_SAND_U32_MAX << (SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS - prefix_len)) : 0;

#define DBAL_QUAL_VAL_ENCODE_IPV4_DIP(qual_val, ip_address, prefix_len)                                                      \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;                                                                       \
    DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len);

#define DBAL_QUAL_VAL_ENCODE_IPV4_SIP(qual_val, ip_address, prefix_len)                                                      \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;                                                                       \
    DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len);

#define DBAL_QUAL_VAL_ENCODE_IN_RIF(qual_val, inrif)                                                                         \
    *qual_val.type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;                                                                            \
    *qual_val.val.arr[0] = inrif;                                                                                            \
    *qual_val.is_valid.arr[0] = SOC_SAND_U32_MAX;

#define DBAL_QUAL_VAL_ENCODE_IPV6(unit, qual_val, address, mask)                                                             \
    *qual_val.val.arr[0] = (CMVEC(unit).big_endian_other ? *(address + 0) : _shr_swap32(*(address + 0)));                    \
    *qual_val.is_valid.arr[0] = *(mask + 0);                                                                                 \
    *qual_val.val.arr[1] = (CMVEC(unit).big_endian_other ? *(address + 1) : _shr_swap32(*(address + 1)));                    \
    *qual_val.is_valid.arr[1] = *(mask + 1);

#define DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(unit, qual_val, address, mask)                                                     \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;                                                                       \
    DBAL_QUAL_VAL_ENCODE_IPV6(unit, qual_val, address, mask)

#define DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(unit, qual_val, address, mask)                                                    \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;                                                                      \
    DBAL_QUAL_VAL_ENCODE_IPV6(unit, qual_val, (address + 2), (mask + 2)) /*advance address by 2, for high*/

#define DBAL_QUAL_VAL_ENCODE_IPV6_SIP_LOW(unit, qual_val, address, mask)                                                     \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;                                                                       \
    DBAL_QUAL_VAL_ENCODE_IPV6(unit, qual_val, address, mask)

#define DBAL_QUAL_VAL_ENCODE_IPV6_SIP_HIGH(unit, qual_val, address, mask)                                                    \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;                                                                      \
    DBAL_QUAL_VAL_ENCODE_IPV6(unit, qual_val, (address + 2), (mask + 2)) /*advance address by 2, for high*/



/*********** SIGNAL MANAGER ***********/

typedef struct
{
    uint8  is_valid;   
    uint32 prm_blk;
    uint16 addr_high;
    uint16 addr_low;
    uint32 msb;
    uint32 lsb;
    int signal_length_in_bits;/* this is the signal length according to the signal table */
    int signal_offset;        /* offset in bytes! inside the signal, used for returning aligned value */
    
} SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO;

typedef enum
{
    /* FLP - LEM*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LEM1_KEY, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LEM2_KEY, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM1_2_FLP_RES, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM2_2_FLP_RES, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM1_2_FLP_HIT, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM2_2_FLP_HIT, 

    /* FLP - LPM*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM1_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM1_KEY_2,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM2_KEY, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM2_KEY_2,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM1_2_FLP_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_RES, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM1_2_FLP_HIT, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_HIT, 

    /* FLP - TCAM */    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_TCAM_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TCAM_2_FLP_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TCAM_2_FLP_HIT, 

    /* FLP - KBP*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_OPCODE,    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART1, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART2, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART3, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART4, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART5, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES,       
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_HIT,       
    
    /* VT - SEM A*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_KEY, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_RES, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_HIT, 
    
    /* VT - SEM B*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_KEY, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_RES, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_HIT, 

    /* VT - TCAM*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_KEY_LSB,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_KEY_MSB,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_RES, 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_HIT,

    /* TT - SEM A*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_HIT,

    /* TT - SEM B*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_HIT,

    /* TT - TCAM*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_HIT,

    /* other signals */
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_RIF,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_RIF_PROFILE,

    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_NOF_TYPES

}SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE;

uint32 arad_pp_signal_mngr_signal_get(int unit, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id, uint32 val[24], int* size_of_signal_in_bits);

uint32 arad_pp_signal_mngr_signal_print(int unit, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id);

uint32 arad_pp_signal_mngr_signal_ids_get(
           int unit, 
           SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, 
           int lookup_number, 
           ARAD_FP_DATABASE_STAGE  stage,
           SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE key_signal_id[5],
           SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* res_signal_id,
           SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* hit_signal_id,
           SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* opcode_signal_id, 
           int* nof_key_signals);


#endif/*__ARAD_PP_DBAL_INCLUDED__*/
