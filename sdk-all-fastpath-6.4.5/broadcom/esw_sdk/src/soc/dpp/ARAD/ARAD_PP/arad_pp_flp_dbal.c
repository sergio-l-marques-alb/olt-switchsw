/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
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
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT

#include <shared/bsl.h>
#include <shared/swstate/sw_state_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_dbal.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#endif


/********* FUNCTION DECLARTIONS *********/

uint32
   arad_pp_flp_dbal_oam_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    /* creating the table that related to the program */
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_OPCODE;


    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_OAM_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, ARAD_PP_LEM_ACCESS_KEY_TYPE_OAM_STATISTICS, qual_info, "oam statistics"));

    /* associating the tables to the program */
    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));

    /* updating extra look configuration for the program */  
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));  

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_oam_down_untagged_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;


    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD; 
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_UNTAGGED;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOWN_UNTAGGED_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_OAM_DOWN_UNTAGGED_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, ARAD_PP_LEM_ACCESS_KEY_TYPE_OAM_DOWN_UNTAGGED_STATISTICS, qual_info, "OAM down untagged statistics"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOWN_UNTAGGED_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP,&keys_to_table_id, NULL, 1));  

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));  

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
   arad_pp_flp_dbal_bfd_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_MY_DISCR_IPV4;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_STATISTICS, qual_info, "BFD statistics"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));


    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));
  
exit:
    SOCDNX_FUNC_RETURN
}

uint32
    arad_pp_flp_dbal_oam_single_tag_statistics_program_tables_init(   
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;
    
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD; 
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_SINGLE_TAG;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_TM_OUTER_TAG;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_SINGLE_TAG_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_OAM_SINGLE_TAG_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, ARAD_PP_LEM_ACCESS_KEY_TYPE_OAM_SINGLE_TAG_STATISTICS, qual_info, "OAM sinngle tag statistics"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_OAM_SINGLE_TAG_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));
    
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_oam_double_tag_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD; 
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_DOUBLE_TAG;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_TM_OUTER_TAG;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_TM_INNER_TAG;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOUBLE_TAG_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_OAM_DOUBLE_TAG_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_OAM_DOUBLE_TAG_STATISTICS, qual_info, "OAM double tag statistics"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOUBLE_TAG_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP,&keys_to_table_id, NULL, 1));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));
  
exit:
    SOCDNX_FUNC_RETURN;
}



uint32
   arad_pp_flp_dbal_bfd_mpls_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_MY_DISCR_MPLS;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_STATISTICS, qual_info, "BFD MPLS statistics"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP,&keys_to_table_id, NULL, 1));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_bfd_pwe_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_MY_DISCR_PWE;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_STATISTICS, qual_info, "BFD PWE statistics"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));  

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));
  
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
    arad_pp_flp_dbal_ipv4uc_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM, ARAD_PP_FLP_IPV4_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, 0, qual_info, "IPv4 UC LEM"));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_flp_dbal_ipv4uc_rpf_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM, ARAD_PP_FLP_IPV4_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, 0, qual_info, "IPv4 UC LEM RPF"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_flp_dbal_ipv4uc_default_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM_DEFAULT, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM_DEFAULT, ARAD_PP_FLP_IPV4_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, 0, qual_info, "IPv4 UC LEM default"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
uint32 
    arad_pp_flp_dbal_ipv4uc_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, JER_KAPS_IPV4_UC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "IPv4 UC KAPS"));

    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
    arad_pp_flp_dbal_ipv4uc_rpf_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS, JER_KAPS_IPV4_UC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "IPv4 UC RPF KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_flp_dbal_ipv4mc_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        /* LSB */
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 4;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
        qual_info[3].qual_nof_bits = 12;
     
        /* MSB */
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
        qual_info[4].qual_offset = 12;
        qual_info[4].qual_nof_bits = 3;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[5].qual_nof_bits = 1;
        qual_info[6].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[7].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
    	qual_info[7].qual_offset = 16;
        qual_info[7].qual_nof_bits = 16;
        qual_info[8].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[8].qual_offset = 4;
        qual_info[8].qual_nof_bits = 12;
        qual_info[9].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
     
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, JER_KAPS_IPV4_MC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 10, 0, qual_info, "IPv4 MC KAPS"));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_flp_dbal_ipv6uc_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
         DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        /* LSB */
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[1].qual_nof_bits = 32;
        qual_info[1].qual_offset = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[2].qual_nof_bits = 32;

        /* MSB */
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[3].qual_nof_bits = 32;
        qual_info[3].qual_offset = 32;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[4].qual_nof_bits = 32;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS, JER_KAPS_IPV6_UC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "IPv6 UC KAPS"));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_flp_dbal_ipv6uc_rpf_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        /* LSB */
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        qual_info[1].qual_nof_bits = 32;
        qual_info[1].qual_offset = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        qual_info[2].qual_nof_bits = 32;

        /* MSB */
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[3].qual_nof_bits = 32;
        qual_info[3].qual_offset = 32;        
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[4].qual_nof_bits = 32;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS, JER_KAPS_IPV6_UC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "IPv6 UC RPF KAPS"));


    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_flp_dbal_ipv6mc_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        /* LSB */
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 1;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[2].qual_nof_bits = 32;
        qual_info[2].qual_offset = 32;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[3].qual_nof_bits = 32;

        /* MSB */
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[4].qual_nof_bits = 32;
        qual_info[4].qual_offset = 32;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[5].qual_nof_bits = 32;
        qual_info[6].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS, JER_KAPS_IPV6_MC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 7, 0, qual_info, "IPv6 MC KAPS"));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

#endif /* defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/

uint32
   arad_pp_flp_dbal_ipv4uc_with_rpf_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_lem_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_kaps_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
#endif /*#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/

    
    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM;
    keys_to_table_id[0].lookup_number = 1;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[1].lookup_number = 2;

    keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS;
    keys_to_table_id[2].lookup_number = 1;
    keys_to_table_id[2].public_lpm_lookup_size = SOC_DPP_DBAL_USE_COMPLETE_KEY;

    keys_to_table_id[3].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
    keys_to_table_id[3].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
    keys_to_table_id[3].lookup_number = 2;
    keys_to_table_id[3].public_lpm_lookup_size = SOC_DPP_DBAL_USE_COMPLETE_KEY;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV4UC_WITH_RPF, ARAD_FP_DATABASE_STAGE_INGRESS_FLP,keys_to_table_id, NULL, 4));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV4UC_WITH_RPF, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_ipv4uc_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
#endif /*#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/
    
    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[0].lookup_number = 2;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
    keys_to_table_id[1].lookup_number = 2;
    keys_to_table_id[1].public_lpm_lookup_size = SOC_DPP_DBAL_USE_COMPLETE_KEY;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV4UC, ARAD_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                keys_to_table_id, NULL, 2));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV4UC, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));
  
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv4compmc_with_rpf_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_lem_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_kaps_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_kaps_table_create(unit));
#endif /*#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/
    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM;
    keys_to_table_id[0].lookup_number = 1;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[1].lookup_number = 2;

    keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS;
    keys_to_table_id[2].lookup_number = 1;
    keys_to_table_id[2].public_lpm_lookup_size = 36;

    keys_to_table_id[3].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
    keys_to_table_id[3].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS;
    keys_to_table_id[3].lookup_number = 2;
    keys_to_table_id[3].public_lpm_lookup_size = 36;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV4COMPMC_WITH_RPF, ARAD_FP_DATABASE_STAGE_INGRESS_FLP,keys_to_table_id, NULL, 4));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV4COMPMC_WITH_RPF, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_ipv4uc_with_l3vpn_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;
  
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_default_lem_table_create(unit));
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
#endif /*#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/

  
    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM_DEFAULT;
    keys_to_table_id[0].lookup_number = 1;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[1].lookup_number = 2;

    keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
    keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
    keys_to_table_id[2].lookup_number = 2;
    keys_to_table_id[2].public_lpm_lookup_size = 36;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV4UC_WITH_DEFAULT, ARAD_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                 keys_to_table_id, NULL, 3));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV4UC_WITH_DEFAULT, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));  

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
uint32
   arad_pp_flp_dbal_ipv6uc_with_rpf_program_tables_init(
     int unit,
     int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kaps_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_rpf_kaps_table_create(unit));

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS;
    keys_to_table_id[0].lookup_number = 1;
    keys_to_table_id[0].public_lpm_lookup_size = 36;/* use 36 bit of Key C*/

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS;
    keys_to_table_id[1].lookup_number = 2;
    keys_to_table_id[1].public_lpm_lookup_size = 36;/* use 36 bit of Key D*/

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                 keys_to_table_id, NULL, 2));
exit:
    SOCDNX_FUNC_RETURN;
}
#endif /*#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/


uint32
   arad_pp_flp_dbal_ipv6uc_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kaps_table_create(unit));
#endif /*#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS;
    keys_to_table_id[0].lookup_number = 2;
    keys_to_table_id[0].public_lpm_lookup_size = 36;/* use 36 bit of Key D*/

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV6UC, ARAD_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id,
                                                                 NULL, 1));
  
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_ipv6mc_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
  
    SOCDNX_INIT_FUNC_DEFS;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6mc_kaps_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_rpf_kaps_table_create(unit));
#endif /*#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS;
    keys_to_table_id[0].lookup_number = 1;
    keys_to_table_id[0].public_lpm_lookup_size = 36; /* use 36 bit of Key C*/

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS;
    keys_to_table_id[1].lookup_number = 2;
    keys_to_table_id[1].public_lpm_lookup_size = 36;/* use 36 bit of Key D*/

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, ARAD_PP_FLP_KEY_PROGRAM_IPV6MC, ARAD_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, 2));

exit:
    SOCDNX_FUNC_RETURN;
}













#if defined(INCLUDE_KBP) && !defined(BCM_88030)
uint32
    arad_pp_dbal_flp_kbp_ip_tables_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    if(ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD; /* 1 bit */
        qual_info[2].qual_nof_bits = 1;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD; /* 4 bit */
        qual_info[3].qual_nof_bits = 4;
        qual_info[3].qual_offset = 16;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD; /* 16 bit */        
        qual_info[4].qual_nof_bits = 16;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR;
        qual_info[5].qual_nof_bits = 3;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MPLS_EXTENDED, DBAL_PREFIX_NOT_DEFINED, 0,
                                                    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 6, 0, qual_info, "IPv4 MPLS extended UC KBP"));
    }

    if(ARAD_KBP_ENABLE_IPV6_EXTENDED){
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
        qual_info[0].qual_offset = 32;        
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
        qual_info[1].qual_nof_bits = 32;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
        qual_info[2].qual_offset = 32;        
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
        qual_info[3].qual_nof_bits = 32;
        qual_info[3].qual_nof_bits = 32;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;        
                
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_EXTENDED, DBAL_PREFIX_NOT_DEFINED, 0,
                                             SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 5, 0, qual_info, "IPv6 extended UC KBP"));        
    }

    if(ARAD_KBP_ENABLE_P2P_EXTENDED){
         DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
         qual_info[0].qual_type = SOC_PPC_FP_QUAL_IN_LIF_PROFILE;        
         SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_P2P_EXTENDED, DBAL_PREFIX_NOT_DEFINED, 0,
                                                      SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 1, 0, qual_info, "P2P extended UC KBP"));
    }
 
    if(ARAD_KBP_ENABLE_IPV4_MC && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_ipv4_mc_flexible_fwd_table", 0)){
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
        qual_info[0].qual_nof_bits = 27;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[3].qual_nof_bits = 4;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FLEXIBLE, DBAL_PREFIX_NOT_DEFINED, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 5, 0, qual_info, "IPv4 MC flexible KBP"));
    }
exit:
     SOCDNX_FUNC_RETURN;
}

#endif 





uint32 
    arad_pp_flp_dbal_program_info_dump(int unit, uint32 prog_id)
{
    ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA flp_key_cons_lsb, flp_key_cons_msb;
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA lookups_tbl;    

    SOCDNX_INIT_FUNC_DEFS;    

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_cons_lsb));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_cons_msb));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &lookups_tbl));

    LOG_CLI((BSL_META(" KEYS:")));

    if ((flp_key_cons_lsb.key_a_inst_0_to_5_valid) || (flp_key_cons_msb.key_a_inst_0_to_5_valid)) {
        LOG_CLI((BSL_META("\t Key A: LSB = 0x%x, MSB = 0x%x,"), flp_key_cons_lsb.key_a_inst_0_to_5_valid, flp_key_cons_msb.key_a_inst_0_to_5_valid));
    } else {
        LOG_CLI((BSL_META("\t Key A: no CE assigned,")));
    }

    if (flp_key_cons_lsb.key_b_inst_0_to_5_valid || (flp_key_cons_msb.key_b_inst_0_to_5_valid)) {
        LOG_CLI((BSL_META("\tKey B: LSB = 0x%x, MSB = 0x%x,"), flp_key_cons_lsb.key_b_inst_0_to_5_valid, flp_key_cons_msb.key_b_inst_0_to_5_valid));
    } else {
        LOG_CLI((BSL_META("\tKey B: no CE assigned,")));
    }

    if (flp_key_cons_lsb.key_c_inst_0_to_5_valid || (flp_key_cons_msb.key_a_inst_0_to_5_valid)) {
        LOG_CLI((BSL_META("\tKey C: LSB = 0x%02x, MSB = 0x%x"), flp_key_cons_lsb.key_a_inst_0_to_5_valid, flp_key_cons_msb.key_a_inst_0_to_5_valid));
    } else {
        LOG_CLI((BSL_META("\tKey C: no CE assigned ")));
    }

    if (SOC_IS_JERICHO(unit)) {
        if (flp_key_cons_lsb.key_d_inst_0_to_7_valid || (flp_key_cons_msb.key_d_inst_0_to_7_valid)) {
            LOG_CLI((BSL_META(",\tKey D: LSB = 0x%x, MSB = 0x%x"), flp_key_cons_lsb.key_d_inst_0_to_7_valid, flp_key_cons_msb.key_d_inst_0_to_7_valid));
        } else {
            LOG_CLI((BSL_META(",\tKey D: no CE assigned ")));
        }
    }

    LOG_CLI((BSL_META("\n LOOKUPS: ")));

    if(lookups_tbl.lem_1st_lkp_valid == 1) {
        LOG_CLI((BSL_META("\tLEM 1st with: %s"), arad_pp_dbal_key_id_to_string(lookups_tbl.lem_1st_lkp_key_select) ));
    }else{
        LOG_CLI((BSL_META("\tLEM 1st isn't valid")));
    }
    if(lookups_tbl.lem_2nd_lkp_valid == 1) {
        LOG_CLI((BSL_META("\tLEM 2nd with: %s"), arad_pp_dbal_key_id_to_string(lookups_tbl.lem_2nd_lkp_key_select) ));
    }else{
        LOG_CLI((BSL_META("\tLEM 2nd isn't valid")));
    }    

    if(lookups_tbl.lpm_1st_lkp_valid == 1) {
        LOG_CLI((BSL_META("\tLPM 1st with: %s"), arad_pp_dbal_key_id_to_string(lookups_tbl.lpm_1st_lkp_key_select) ));
    }else{
        LOG_CLI((BSL_META("\tLPM 1st isn't valid")));
    }    

    if(lookups_tbl.lpm_2nd_lkp_valid == 1) {
        LOG_CLI((BSL_META("\tLPM 2nd with: %s"),arad_pp_dbal_key_id_to_string(lookups_tbl.lpm_2nd_lkp_key_select) ));
    }else{
        LOG_CLI((BSL_META("\tLPM 2nd isn't valid")));
    }    

    if (lookups_tbl.tcam_lkp_db_profile != ARAD_TCAM_ACCESS_PROFILE_INVALID ) {
        LOG_CLI((BSL_META("\tTCAM 1st with: %s DB %d"), arad_pp_dbal_key_id_to_string(lookups_tbl.tcam_lkp_key_select), lookups_tbl.tcam_lkp_db_profile ));        
    }else{
        LOG_CLI((BSL_META("\tTCAM 1st isn't valid")));
    }    

    if (SOC_IS_JERICHO(unit)) {
        if (lookups_tbl.tcam_lkp_db_profile_1 != ARAD_TCAM_ACCESS_PROFILE_INVALID ) {
            LOG_CLI((BSL_META("\tTcam 2nd with: %s DB %d "), arad_pp_dbal_key_id_to_string(lookups_tbl.tcam_lkp_key_select_1), lookups_tbl.tcam_lkp_db_profile_1 ));
        }        
    }

    if(lookups_tbl.elk_lkp_valid == 1){
        LOG_CLI((BSL_META("\tKBP with: A=%d, B=%d, C=%d opcode %d\n"), lookups_tbl.elk_key_a_valid_bytes,lookups_tbl.elk_key_b_valid_bytes, lookups_tbl.elk_key_c_valid_bytes, lookups_tbl.elk_opcode));        
    }

    LOG_CLI((BSL_META("\n\n")));
exit:
     SOCDNX_FUNC_RETURN;
}
