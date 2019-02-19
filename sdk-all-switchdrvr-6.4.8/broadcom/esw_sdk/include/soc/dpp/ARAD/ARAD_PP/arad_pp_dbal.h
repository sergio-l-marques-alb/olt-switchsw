/* $Id: dpp_dbal.c, v 1.95 Broadcom SDK $
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
#include <appl/diag/parse.h>

#define DBAL_MAX_PROGRAMS_PER_TABLE                         10
#define DBAL_MAX_NAME_LENGTH                                30
#define DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET        50

/* in general, when the prefix is not defined the DBAL allocates one dynamiclly, not all physical_db support dynamic allocations */
#define DBAL_PREFIX_NOT_DEFINED             (-1) 

#define SOC_DPP_DBAL_USE_COMPLETE_KEY       255
/* Take 36 * 4 bits from the lsb of the private key - this allows searching the public tables with vrf = 0 */
#define SOC_DPP_DBAL_ZERO_VRF_IN_KEY        36

typedef enum
{
    SOC_DPP_HW_KEY_LOOKUP_DISABLED,
    SOC_DPP_HW_KEY_LOOKUP_IN_LEM_1ST,
    SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND,
    SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY

}SOC_DPP_HW_KEY_LOOKUP;

typedef enum
{
    SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_ADD,
    SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_DELETE,
    SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_NUM_OF_MODE
}SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_MODE;

#define DBAL_QUAL_INFO_CLEAR(qual_info, nof_elements)   sal_memset(qual_info, 0, sizeof(SOC_DPP_DBAL_QUAL_INFO)*nof_elements)

#define SOC_DPP_DBAL_NOF_DYNAMIC_TABLES         50
/*enum that represents all the existing tables for each new table a value should be added */
typedef enum
{
    SOC_DPP_DBAL_SW_TABLE_ID_INVALID = -1,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM_DEFAULT,
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
    SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOWN_UNTAGGED_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_SINGLE_TAG_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOUBLE_TAG_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_LEARN_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_ECHO_LEM, 
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_INNER_LEARN_LEM,           /* FLP DBAL Table access by Key using Inner-Most VID Tag */
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_INNER_FWD_LEM,

/*         VTT DB                   */
    SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INITIAL_VID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_PCP_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_EVB_SINGLE_TAG_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_EVB_DOUBLE_TAG_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_INNER_OUTER_VID_PCP_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv4_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_COMPRESSED_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_COMPRESSED_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_UNINDEXED_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_UNINDEXED_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_GAL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_GAL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_GAL_SEM_B,
/* Don't change order of following enums -- start */
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L12IDX_SEM_B,
/* Don't change order of enums above  -- end*/
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_UNINDEXED_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_UNINDEXED_SEM_B,
/* Don't change order of following enums  -- start */
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L12IDX_SEM_B,
/* Don't change order of enums above  end*/
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_COUPLING_L3_L4_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DESIGNATED_VID_TRILL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_ING_NICK_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_UC_ONE_TAG_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_UC_TWO_TAG_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_SIPv4_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_HEADER_ISID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MC_DIPv4_RIF_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_SINGLE_TAG_VD_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_VD_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_5_TUPPLE_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_VD_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VD_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VD_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_OUTER_INNER_VD_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_TST1_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_ONE_TAG_TST1_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_TST1_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_TST1_MPLS_TCAM,
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
    SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_UNTAG_CHECK_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_TEST2_TCAM, 
    SOC_DPP_DBAL_SW_TABLE_ID_TEST2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_VRF_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST = SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_VRF_TCAM,

    /*       END VTT DB                 */

    SOC_DPP_DBAL_SW_TABLE_DYNAMIC_BASE_ID, /* saving location for dynamic tables*/
    SOC_DPP_DBAL_SW_TABLE_DYNAMIC_END_ID = SOC_DPP_DBAL_SW_TABLE_DYNAMIC_BASE_ID + SOC_DPP_DBAL_NOF_DYNAMIC_TABLES,


    SOC_DPP_DBAL_SW_NOF_TABLES

}SOC_DPP_DBAL_SW_TABLE_IDS;

/* enum that represents all the physical database existing */
typedef enum
{
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS,
	/* following DBs are not supported for table creation */
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_ESEM,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM1,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM2,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_RMAP,

    SOC_DPP_DBAL_PHYSICAL_DB_NOF_TYPES

}SOC_DPP_DBAL_PHYSICAL_DB_TYPES;

typedef enum
{
    SOC_DPP_DBAL_PROGRAM_KEY_A,
    SOC_DPP_DBAL_PROGRAM_KEY_B,
    SOC_DPP_DBAL_PROGRAM_KEY_C,
    SOC_DPP_DBAL_PROGRAM_KEY_D,

    SOC_DPP_DBAL_PROGRAM_KEY_A_MSB,
    SOC_DPP_DBAL_PROGRAM_KEY_B_MSB,
    SOC_DPP_DBAL_PROGRAM_KEY_C_MSB,
    SOC_DPP_DBAL_PROGRAM_KEY_D_MSB,

    SOC_DPP_DBAL_PROGRAM_NOF_KEYS

}SOC_DPP_DBAL_PROGRAM_KEYS;

#define SOC_DPP_DBAL_PROGRAM_NOF_COMPLETE_KEYS      SOC_DPP_DBAL_PROGRAM_NOF_KEYS/2

/* this is made for VTT stage */
#define MEM_TYPE_TO_KEY(mem_type)  (mem_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A) ? SOC_DPP_DBAL_PROGRAM_KEY_A : \
                                   (mem_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B) ? SOC_DPP_DBAL_PROGRAM_KEY_B : SOC_DPP_DBAL_PROGRAM_KEY_C /* get key type from mem type */

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
    uint8                       lookup_number; /* applicable only for FLP */
    SOC_DPP_DBAL_PROGRAM_KEYS   key_id; /* when using SOC_DPP_DBAL_PROGRAM_NOF_KEYS it will allocate the next available key */
    SOC_DPP_DBAL_SW_TABLE_IDS   sw_table_id;
    uint32                      public_lpm_lookup_size; /* applicable only for FLP, 0 = disabled, SOC_DPP_DBAL_USE_COMPLETE_KEY when complete key is chosen else use: Lpm_2ndLkpKey[0 +: 4*LpmPublic_2ndLkpKeySize]*/ 
}SOC_DPP_DBAL_KEY_TO_TABLE;

typedef struct
{
    ARAD_FP_DATABASE_STAGE  stage;
    uint32                  program_id;/* program ID refers to the cam selection line*/
    uint8                   lookup_number; /* the desire lookup number */
    uint32                  key_id;
    uint8                   nof_bits_used_in_key;
    uint8                   ce_assigned[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]; /* which CE assigned to each qualifier */
    uint32                  public_lpm_lookup_size; /* 0 = disabled, SOC_DPP_DBAL_USE_COMPLETE_KEY when complete key is chosen else use: Lpm_2ndLkpKey[0 +: 4*LpmPublic_2ndLkpKeySize]*/
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
    SOC_PPC_FP_ACTION_TYPE          action[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    int                             nof_actions;
} SOC_DPP_DBAL_TABLE_INFO;

/* the prefix size, this set to Jericho value */
#define ARAD_PP_ISEM_ACCESS_MAX_PREFIX_SIZE_JER                     6

typedef struct SOC_DPP_DBAL_INFO_s
{
  SOC_DPP_DBAL_TABLE_INFO dbal_tables[SOC_DPP_DBAL_SW_NOF_TABLES];
  SOC_DPP_DBAL_SW_TABLE_IDS sem_a_prefix_mapping[(1 << ARAD_PP_ISEM_ACCESS_MAX_PREFIX_SIZE_JER)];
  SOC_DPP_DBAL_SW_TABLE_IDS sem_b_prefix_mapping[(1 << ARAD_PP_ISEM_ACCESS_MAX_PREFIX_SIZE_JER)];
} SOC_DPP_DBAL_INFO;

/*********** SERVICE FUNCTIONS ***********/
uint32 arad_pp_dbal_init(int unit);
uint32 arad_pp_dbal_deinit(int unit);
uint32 arad_pp_dbal_ce_info_get(int unit, uint32 table_id, ARAD_FP_DATABASE_STAGE stage, ARAD_PMF_CE *ce_array, uint8 *nof_ces, uint8 *is_key_320b, uint8 *ces_ids);
uint32 arad_pp_dbal_db_predfix_get(int unit, uint32 table_id, uint32* db_prefix);
uint32 arad_pp_dbal_table_is_initiated(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, int* is_table_initiated);
uint32 arad_pp_dbal_qualifier_full_size_get(int unit, ARAD_FP_DATABASE_STAGE stage, SOC_PPC_FP_QUAL_TYPE qual_type, uint8* qual_full_size, uint8* qual_is_in_hdr);
const char* arad_pp_dbal_key_id_to_string(int unit, uint8 key_id);

/*********** TABLE FUNCTIONS ***********/


/**************************************
 function arad_pp_dbal_table_create creates a table in the SW.
 the use of this function is mostly for static tables, where no prefix allocation is needed.
 
    for static tables:
 1. when creating table in the LEM, the additional_info should be the key_type, the prefix should be the app_type (most cases app_type = key_type)
 2. when creating table in the SEM, the additional_info has to be Zero (no meaning) prefix should be the logical DB value
 3. when creating table in the TCAM, the additional_info value could be one of the 3 options:
    (a) ARAD_PP_ISEM_ACCESS_KEY_TYPE_DUMMY: when the tcam table is not realy used in this stage, only in the PMF. so tcam resources will not be allocated
    (b) valid key_type: when the tcam table is defined staticlly, the key_type is connected to tcam_db_id hardcoded. tcam resources will not be allocated
    (c) 0: when the tcam table is created dynamiclly, tcam resources will be allocated.

    for KAPS dynamic tables:
 1. The function arad_pp_flp_dbal_kaps_table_prefix_get allocates the prefixes to the tables.
    It needs to be updated in two places for new tables, templates are provided using SOC_DPP_DBAL_SW_TABLE_ID_NEW_TABLE_KAPS.
 2. The table create function needs its qualifiers + prefix length to be 80 bits aligned.
    For example: arad_pp_flp_dbal_ipv4uc_kaps_table_create, has 3 qualifiers VRF (14 bits), IPV4-DIP (32 bits) and ZEROS (padding).
                 80 - 14 - 32 = 34. Therefore the ZEROs qualifier length and the prefix length sum must equal 34.
 3. Dynamic tables are located in the KAPS Private DB (specifically they share the 2MSbits prefix with IPV4_UC).

**************************************/
uint32 arad_pp_dbal_table_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 db_prefix, uint32 db_prefix_len,
                                  SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db, int nof_qualifiers, uint32 additional_info,
                                  SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], char* table_name);

uint32 arad_pp_dbal_dynamic_table_create(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db, int nof_qualifiers,
                                  SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], char* table_name, SOC_DPP_DBAL_SW_TABLE_IDS* table_id);

uint32 arad_pp_dbal_table_clear(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_table_print(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_table_stage_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_FP_DATABASE_STAGE stage );
uint32 arad_pp_dbal_table_actions_set(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_ACTION_TYPE action[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX]);
uint32 arad_pp_dbal_table_destroy(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_table_physical_db_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES* physical_db_type );
uint32 arad_pp_dbal_table_disassociate_all(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);

/********* PROGRAM FUNCTIONS *********/
uint32 arad_pp_dbal_key_inst_set(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, int key_id, SOC_DPP_DBAL_QUAL_INFO qual_info, int is_msb, int ce_id);
uint32 arad_pp_dbal_program_to_tables_associate(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, 
                                                SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS], uint8 use_32_bit_ce[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] [SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], int nof_valid_keys);

uint32 arad_pp_dbal_program_to_tables_associate_implicit(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, 
                                                         SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS],
                                                         uint8 qualifier_to_ce_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS][SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                                         int nof_valid_keys);
uint32 arad_pp_dbal_program_table_disassociate(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
/*********** ENTRY MANAGMENT FUNCTION ***********/
uint32 arad_pp_dbal_entry_add(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, void* payload, SOC_SAND_SUCCESS_FAILURE *success);
uint32 arad_pp_dbal_entry_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found);
uint32 arad_pp_dbal_entry_delete(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
uint32 arad_pp_dbal_entry_key_to_kbp_buffer(int unit, SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 table_size_in_bytes, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 *prefix_len, uint8 *data_bytes);
#endif

uint32 arad_pp_dbal_block_get(int unit, ARAD_PP_IP_ROUTING_TABLE_RANGE *block_range_key, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL *qual_vals_array, void* payload, uint32 *nof_entries);

/* functions used to managment entries by ID */
uint32 arad_pp_dbal_entry_add_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, ARAD_TCAM_ACTION* payload, uint8 is_for_update, SOC_SAND_SUCCESS_FAILURE *success);
uint32 arad_pp_dbal_entry_get_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found);
uint32 arad_pp_dbal_entry_delete_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success);


/*********** DIAGNOSTIC FUNCTIONS ***********/
uint32 arad_pp_dbal_last_packet_dump(int unit, int core_id);
uint32 arad_pp_dbal_ce_per_program_dump(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage);
uint32 arad_pp_dbal_table_info_dump(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_tables_dump(int unit, int is_full_info);
uint32 arad_pp_dbal_entry_dump(int unit, ARAD_FP_DATABASE_STAGE stage, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 val[ARAD_PP_FP_TCAM_ENTRY_SIZE]);
uint32 arad_pp_dbal_diag_entry_manage(int unit, SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_MODE mode, int table_id, args_t* arg);
uint32 arad_pp_dbal_isem_prefix_table_dump(int unit);
uint32 arad_pp_dbal_lem_prefix_table_dump(int unit);
uint32 arad_pp_dbal_dynamic_table_test(int unit);
const char* arad_pp_dbal_physical_db_to_string(SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type);
int arad_pp_dbal_program_to_string(int unit, ARAD_FP_DATABASE_STAGE stage, int cam_line, const char**str);
uint32 arad_pp_dbal_phisycal_db_dump(int unit, int mode);
uint32 arad_pp_dbal_qualifier_to_instruction(int unit, ARAD_FP_DATABASE_STAGE stage, uint8 is_32b_ce, uint32 nof_bits, SOC_PPC_FP_QUAL_TYPE qual_type, int is_msb, uint32* ce_instr_encoding);
uint32 arad_pp_dbal_qualifier_to_instruction_dump(int unit,ARAD_FP_DATABASE_STAGE stage, int is_32b_ce, int nof_bits, SOC_PPC_FP_QUAL_TYPE qual_type);

/*********** QUALIFIER MANAGMENT ***********/

uint32 arad_pp_dbal_kbp_buffer_to_entry_key(int unit, const SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 prefix_len, uint8 *data_bytes,
                                            ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]);

/*qual_index is reset to 0 when it returns*/
#define DBAL_QUAL_VALS_CLEAR(qual_vals)                                                                                      \
    {uint32 qual_index;                                                                                                      \
        sal_memset(qual_vals, 0x0, sizeof(SOC_PPC_FP_QUAL_VAL)*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);                             \
        for (qual_index = 0; qual_index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++qual_index)                                     \
        {                                                                                                                    \
            (qual_vals)[qual_index].type = BCM_FIELD_ENTRY_INVALID;                                                        \
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

/*Note the prefix_len is from 32bit */
#define DBAL_QUAL_VAL_ENCODE_IN_RIF(qual_val, inrif, mask)                                                                   \
    *qual_val.type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;                                                                            \
    *qual_val.val.arr[0] = inrif;                                                                                            \
    *qual_val.is_valid.arr[0] = mask;

#define DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, address, mask)                                                             \
    *qual_val.val.arr[0] = *(address + 0);                                                                                   \
    *qual_val.is_valid.arr[0] = *(mask + 0);                                                                                 \
    *qual_val.val.arr[1] = *(address + 1);                                                                                   \
    *qual_val.is_valid.arr[1] = *(mask + 1);

#define DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(qual_val, address, mask)                                                     \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;                                                                       \
    DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, address, mask)

#define DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(qual_val, address, mask)                                                    \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;                                                                      \
    DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, (address + 2), (mask + 2)) /*advance address by 2, for high*/

#define DBAL_QUAL_VAL_ENCODE_IPV6_SIP_LOW(qual_val, address, mask)                                                     \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;                                                                       \
    DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, address, mask)

#define DBAL_QUAL_VAL_ENCODE_IPV6_SIP_HIGH(qual_val, address, mask)                                                    \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;                                                                      \
    DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, (address + 2), (mask + 2)) /*advance address by 2, for high*/

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

uint32 arad_pp_signal_mngr_signal_get(int unit, int core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id, uint32 val[24], int* size_of_signal_in_bits);

uint32 arad_pp_signal_mngr_signal_print(int unit, int core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id);

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
