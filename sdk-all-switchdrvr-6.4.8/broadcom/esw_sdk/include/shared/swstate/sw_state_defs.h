/*
 * $Id: $
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
 * 
 * INFO: this module is the entry point (the root) for the entire SDK global 
 *       sw state.
 *       if we look at the sw state as a tree, then the root lies at shr_sw_state_t,
 *       it's main branches (module's entry points) are defined here inside this file.
 *       and it continue to expand in the specific module's .h files (which are
 *       the files in shared/swstate/sw_state_includes.h)
 *  
 */
#ifndef _SHR_SW_STATE_DEFS_H
#define _SHR_SW_STATE_DEFS_H

#include <soc/types.h>
#include <sal/core/sync.h>
#include <shared/swstate/sw_state_includes.h>
#include <shared/swstate/sw_state_workarounds.h>
#include <shared/swstate/sw_state_def_init_indexes.h>




#define SW_STATE_NULL_CHECK(param) \
    do{ \
        if((param) == NULL){ \
            _sw_state_access_debug_hook(0);\
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(_unit, "sw state ERROR: trying to free something that was never allocated\n")));\
            /* for now - just print to screen; in the future - consider returnning error */ \
            return _SHR_E_NONE; \
            } \
    } while(0)

#define SW_STATE_IS_WARM_BOOT_CHECK() \
    do{ \
        if (SOC_WARM_BOOT(_unit)) {\
            _sw_state_access_debug_hook(1);\
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(_unit,\
            "sw state ERROR: trying to set/alloc during warm reboot. for debug, use breakpoint on _sw_state_access_debug_hook\n")));\
            return _SHR_E_DISABLED;\
        }\
    } while(0)


#define SW_STATE_IS_DEINIT_CHECK() \
    do{ \
        if (SOC_IS_DETACHING(_unit)) {\
            _sw_state_access_debug_hook(2);\
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(_unit,\
            "sw state ERROR: trying to free/modify during deinit. for debug, use breakpoint on _sw_state_access_debug_hook\n")));\
            return _SHR_E_NONE;\
        }\
    } while(0)

#define SW_STATE_IS_ALREADY_ALLOCATED_CHECK(__ptr) \
    do{ \
        if (__ptr) {\
            _sw_state_access_debug_hook(3);\
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(_unit,\
            "sw state ERROR: trying to allocate something that is already allocated. for debug, use breakpoint on _sw_state_access_debug_hook\n")));\
            return _SHR_E_NONE;\
        }\
    } while(0)

#if (0)
/* { */
#define SW_STATE_OUT_OF_BOUND_CHECK(__index, __max, __param) \
    do{ \
        if (__index > __max) {\
            _sw_state_access_debug_hook(4);\
			LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(_unit,\
			"\nSW STATE ERROR: INDEX OUT OF BOUND.\n\
Trying to access "#__param" with index %d out of %d.\n\
For debug, use breakpoint on _sw_state_access_debug_hook\n"), (int)(__index), (int)(__max)));\
            return _SHR_E_PARAM;\
        }\
    } while(0)
/* } */
#else
/* { */
#define SW_STATE_OUT_OF_BOUND_CHECK(__index, __max, __param)
/* } */
#endif

/* there is another null check above SW_STATE_NULL_CHECK for free purpose
    this null check is for use purpose */

#define SW_STATE_NULL_ACCESS_CHECK(param) \
    do{ \
        if((param) == NULL){ \
            _sw_state_access_debug_hook(5);\
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(_unit, "\nsw state ERROR: NULL REFERENCE \n")));\
            return _SHR_E_MEMORY; \
        } \
    } while(0)


 
typedef struct soc_dpp_bcm_sw_state_s{
    PARSER_HINT_PTR bcm_stg_info_t                  *stg;
    PARSER_HINT_PTR _dpp_policer_state_t            *policer;
    PARSER_HINT_PTR _bcm_dpp_counter_state_t        *counter;
    PARSER_HINT_PTR _bcm_petra_mirror_unit_data_t   *mirror;
    PARSER_HINT_PTR bcm_dpp_l3_info_t               *l3;
    PARSER_HINT_PTR l2_data_t                       *l2;
    PARSER_HINT_PTR trunk_state_t                   *trunk;
    PARSER_HINT_PTR _bcm_dpp_vlan_unit_state_t      *vlan;
    PARSER_HINT_PTR _bcm_dpp_vswitch_bookkeeping_t  *vswitch;
    PARSER_HINT_PTR bcm_dpp_rx_info_t               *rx;
    PARSER_HINT_PTR bcm_dpp_failover_info_t         *failover;
    PARSER_HINT_PTR bcm_dpp_gport_mgmt_info_t       *gport_mgmt;
    PARSER_HINT_PTR bcm_dpp_bfd_info_t              *bfd;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_info_t       *alloc_mngr;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_utils_info_t *alloc_mngr_utils;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_glif_info_t  *alloc_mngr_glif;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_local_lif_info_t  *alloc_mngr_local_lif;
    PARSER_HINT_PTR bcm_dpp_ipmc_info_t             *ipmc;
    PARSER_HINT_PTR bcm_dpp_switch_info_t           *_switch;
    PARSER_HINT_PTR bcm_dpp_port_info_t             *port;
    PARSER_HINT_PTR bcm_dpp_trill_state_t           *trill;
    PARSER_HINT_PTR bcm_dpp_stack_config_t          *stack;
    PARSER_HINT_PTR bcm_dpp_qos_state_t             *qos;
    PARSER_HINT_PTR bcm_dpp_mim_info_t              mim;
    PARSER_HINT_PTR bcm_dpp_oam_info_t              *oam;
    PARSER_HINT_PTR bcm_dpp_cosq_info_t             *cosq;
    PARSER_HINT_PTR bcm_dpp_field_info_t            *field;
} soc_dpp_bcm_sw_state_t;

typedef struct soc_dpp_soc_arad_sw_state_s{
    PARSER_HINT_PTR soc_arad_sw_state_tm_t *tm;
    PARSER_HINT_PTR soc_arad_sw_state_pp_t *pp;
} soc_dpp_soc_arad_sw_state_t;
/*
 * Entry point structure for all SAND utilities.
 */
typedef struct soc_dpp_soc_sand_sw_state_s{
  PARSER_HINT_PTR soc_sand_sw_state_sorted_list_t *sorted_list ;
  PARSER_HINT_PTR soc_sand_sw_state_occ_bitmap_t  *occ_bitmap ;
  PARSER_HINT_PTR soc_sand_sw_state_hash_table_t  *hash_table ;
  PARSER_HINT_PTR soc_sand_sw_state_multi_set_t   *multi_set ;
} soc_dpp_soc_sand_sw_state_t ;


typedef struct soc_jericho_sw_state_tm_s {
    uint16 nof_remote_faps_with_remote_credit_value;
    PARSER_HINT_PTR SOC_TMC_FC_PFC_GEN_BMP_INFO *pfc_gen_bmp;
} soc_jericho_sw_state_tm_t;


typedef struct soc_jericho_sw_state_pp_s {
  PARSER_HINT_PTR jer_pp_mpls_term_t mpls_term;
} soc_jericho_sw_state_pp_t; 

typedef struct soc_dpp_soc_jericho_sw_state_s {
    soc_jericho_sw_state_tm_t tm;
    soc_jericho_sw_state_pp_t pp;
} soc_dpp_soc_jericho_sw_state_t;

typedef struct soc_dpp_soc_config_s{
    uint32      autosync;
} soc_dpp_soc_config_t;

typedef struct soc_dpp_soc_sw_state_s{
    soc_dpp_soc_arad_sw_state_t arad;
	soc_dpp_soc_jericho_sw_state_t jericho;
	soc_dpp_soc_sand_sw_state_t sand;
    soc_dpp_soc_config_t config;
} soc_dpp_soc_sw_state_t;

typedef struct soc_dpp_shr_sw_state_s{
    _sw_state_res_unit_desc_t       *resmgr_info;
    sw_state_hash_table_db_t        htbls;
    sw_state_res_tag_bitmap_info_t  bmp_info;
    _shr_template_unit_desc_t       *template_info;
} soc_dpp_shr_sw_state_t;

/*To be moved out*/
/******************************************** Example begin ***************************************************/
typedef struct mini_example_s {
    int                     regular;
    int                     *singlePtr;
    int                     staticArr[4];
    int                     staticArrArr[2][3];

	PARSER_HINT_PTR         int *ptrVar;
    PARSER_HINT_ARR         int *arrVar;
    PARSER_HINT_ARR_ARR     int **arrArr;
    PARSER_HINT_ARR_PTR     int **arrPtr;
		
    SHR_BITDCL				*shr_bitdcl_singlePtr;
	SHR_BITDCL		        **shr_bitdcl_doublePtr;
    SHR_BITDCL				shr_bitdcl_staticArr[4]; 
} mini_example_t;

typedef struct mix_stat_dyn_example_s {
	PARSER_HINT_PTR         int *staticArrDynPtr[3];
    PARSER_HINT_ARR         int *staticArrDynArr[3];
    PARSER_HINT_ARR_PTR     int **staticArrDynArrPtr[2];
    PARSER_HINT_ARR_ARR     int **staticArrArrDynArrArr[2][4];
} mix_stat_dyn_example_t;

typedef struct one_level_example_s {
    mini_example_t          oneLevelRegular;
    mini_example_t          *oneLevelSinglePtr;
    mini_example_t          oneLevelStaticArr[2];
    mini_example_t          oneLevelStaticArrArr[2][3];
	  mini_example_t          oneLevelStaticArrArr3[3][4][5];
    PARSER_HINT_PTR         mini_example_t *oneLevelPtr;
    PARSER_HINT_ARR         mini_example_t *oneLevelArr;
    PARSER_HINT_ARR_ARR     mini_example_t **oneLevelArrArr;
    PARSER_HINT_ARR_PTR     mini_example_t **oneLevelArrPtr;
} one_level_example_t;

typedef struct two_levels_example_s {
    one_level_example_t     twoLevelsRegular;
    one_level_example_t     *twoLevelsSinglePtr;
    one_level_example_t     twoLevelsStaticArrInt[2];
    one_level_example_t     twoLevelsStaticArrArr[2][3];
    PARSER_HINT_PTR         one_level_example_t *twoLevelsPtr;
    PARSER_HINT_ARR         one_level_example_t *twoLevelsArr;
    PARSER_HINT_ARR_ARR     one_level_example_t **twoLevelsArrArr;
    PARSER_HINT_ARR_PTR     one_level_example_t **twoLevelsArrPtr;
	PARSER_HINT_ALLOW_WB_ACCESS	one_level_example_t twoLevelsRegularAllowWbAccess;
	
} two_levels_example_t;


#ifndef BCM_ISSU_TEST_AFTER_ISSU
#define TEST_ISSU_MY_ARRAY_SIZE  10
#define TEST_ISSU_MY_ARRAY_SIZE_0  2
#define TEST_ISSU_MY_ARRAY_SIZE_1  3
#endif /* BCM_ISSU_TEST_AFTER_ISSU */
#ifdef BCM_ISSU_TEST_AFTER_ISSU
#define TEST_ISSU_MY_ARRAY_SIZE  20
#define TEST_ISSU_MY_ARRAY_SIZE_0  5
#define TEST_ISSU_MY_ARRAY_SIZE_1  7
#endif /* BCM_ISSU_TEST_AFTER_ISSU */

typedef struct sw_state_issu_example_s {
    uint8                   o1234567890123456789012345678901234567890123456789;
    uint8                   myUint8;
    uint16                  myUint16;
    uint32                  myArray[TEST_ISSU_MY_ARRAY_SIZE]; 
    uint32                  myArray2D[TEST_ISSU_MY_ARRAY_SIZE_0][TEST_ISSU_MY_ARRAY_SIZE_1]; 
#ifndef BCM_ISSU_TEST_AFTER_ISSU
    uint32                  myUint32; 
#endif
#ifdef BCM_ISSU_TEST_AFTER_ISSU
    uint64                  myUint64; 
#endif /* BCM_ISSU_TEST_AFTER_ISSU */
} sw_state_issu_example_t;
/******************************************** Example end *****************************************************/


typedef struct soc_dpp_sw_state_s {
    soc_dpp_bcm_sw_state_t  bcm;
    soc_dpp_soc_sw_state_t  soc;
    soc_dpp_shr_sw_state_t  shr;
} soc_dpp_sw_state_t;

typedef struct shr_sw_state_s {
                    soc_dpp_sw_state_t        dpp;
#ifdef _SHR_SW_STATE_EXM
    PARSER_HINT_PTR two_levels_example_t      *exm;
    PARSER_HINT_PTR mix_stat_dyn_example_t    *exmMixStatDyn; 
    PARSER_HINT_PTR mini_example_t            *miniExm;
    PARSER_HINT_PTR sw_state_issu_example_t *issu_example;
#endif
} shr_sw_state_t;

extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

#endif /* _SHR_SW_STATE_DEFS_H */
