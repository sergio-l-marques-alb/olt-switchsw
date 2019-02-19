/*
 * $Id: dpp_wb_engine.c,v 1.75 Broadcom SDK $
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
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#define _NO_JUMP_NEEDED_ 0xffffffff
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/scache.h>
#include <soc/drv.h>

#ifdef VALGRIND_DEBUG
/* Used for VALGRIND_DEBUG. See _dpp_wb_engine_valgrind_check_array_is_initialized for details. */
#endif 

#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <soc/wb_engine.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/soc_sw_db.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lif_ing_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <bcm_int/dpp/gport_mgmt.h>
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_sim_em.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_filter.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/wb_db_hash_tbl.h>
#endif /* BCM_ARAD_SUPPORT */




/* undefined at the end of the module */
#define NEVER_REMOVED 0xff
#define NO_FLAGS 0x0
#define DEFAULT 0xffffffff
#define VERSION(_ver) (_ver)

#ifdef BCM_PETRA_SUPPORT

int _bcm_dpp_egr_thresh_map[BCM_MAX_NUM_UNITS][SOC_TMC_NOF_FAP_PORTS_MAX];
int _bcm_dpp_egr_interface_unicast_thresh_map[BCM_MAX_NUM_UNITS][SOC_TMC_IF_NOF_NIFS];
int _bcm_dpp_egr_interface_multicast_thresh_map[BCM_MAX_NUM_UNITS][SOC_TMC_IF_NOF_NIFS];

#endif /*BCM_PETRA_SUPPORT*/

#ifdef BCM_ARAD_SUPPORT
/* the pp_Sw_db data */
ARAD_PP_SW_DB *Dpp_wb_engine_Arad_pp_sw_db;

/* Database of egress encap information per unit */
bcm_dpp_am_egress_encap_t _dpp_am_egress_encap[SOC_MAX_NUM_DEVICES] = {{0}};

/* Database of ingress LIF information per unit */
bcm_dpp_am_ingress_lif_t _dpp_am_ingress_lif[SOC_MAX_NUM_DEVICES] = {{0}};

/* Database of sync LIF information per unit */
bcm_dpp_am_sync_lif_t _dpp_am_sync_lif[SOC_MAX_NUM_DEVICES] = {{0}};

/* OAM BCM */
uint8 _dpp_oam_is_init[SOC_MAX_NUM_DEVICES] = {0};
uint8 _dpp_bfd_is_init[SOC_MAX_NUM_DEVICES] = {0};
uint32 _dpp_oam_acc_ref_counter[SOC_MAX_NUM_DEVICES] = {0};
uint32 _dpp_oam_traps_ref_counter[SOC_MAX_NUM_DEVICES][SOC_PPD_NOF_TRAP_CODES] = {{0}};
uint8 _dpp_oam_y1731_opcode_to_intenal_opcode_map[BCM_MAX_NUM_UNITS][SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT]={{0}};
uint32 _bcm_oamp_error_type_to_trap_id_map[BCM_MAX_NUM_UNITS][SOC_PPD_OAM_OAMP_TRAP_TYPE_COUNT]={{0}};
uint32 _bcm_oamp_rx_trap_code_ref_count[SOC_MAX_NUM_DEVICES][SOC_PPD_NOF_TRAP_CODES] = {{0}};
uint32 _dpp_bfd_mpls_udp_sport_ref_counter[SOC_MAX_NUM_DEVICES]={0};
uint32 _dpp_bfd_ipv4_udp_sport_ref_counter[SOC_MAX_NUM_DEVICES]={0};
uint32 _dpp_bfd_ipv4_multi_hop_acc_ref_counter[SOC_MAX_NUM_DEVICES]={0};
uint32 _dpp_bfd_pdu_ref_counter[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_bfd_mpls_tp_cc_ref_counter[SOC_MAX_NUM_DEVICES]={0};

/* OAM SOC */
uint32 _dpp_arad_pp_oam_trap_code_recycle[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_oam_trap_code_snoop[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_oam_trap_code_frwrd[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_oam_trap_code_drop[BCM_MAX_NUM_UNITS]={0};
/*Error traps*/
uint32 _dpp_arad_pp_oam_trap_code_trap_to_cpu_level[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_oam_trap_code_trap_to_cpu_passive[BCM_MAX_NUM_UNITS]={0};

/*BFD trap codes*/
uint32 _dpp_arad_pp_bfd_trap_code_trap_to_cpu[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_bfd_trap_code_oamp_bfd_cc_mpls_tp[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_bfd_trap_code_oamp_bfd_ipv4[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_bfd_trap_code_oamp_bfd_mpls[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_bfd_trap_code_oamp_bfd_pwe[BCM_MAX_NUM_UNITS]={0};

/*Eth mirror profiles */
uint32 _dpp_arad_pp_oam_mirror_profile_snoop_to_cpu[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_oam_mirror_profile_recycle[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_oam_mirror_profile_err_level[BCM_MAX_NUM_UNITS]={0};
uint32 _dpp_arad_pp_oam_mirror_profile_err_passive[BCM_MAX_NUM_UNITS]={0};

uint32 _dpp_arad_pp_tcam_last_entry_id[BCM_MAX_NUM_UNITS]={0};

/* OAM End */

/*arad_sw_db*/
ARAD_SW_DB Arad_sw_db;

int
soc_dpp_wb_engine_Arad_pp_sw_db_get(ARAD_PP_SW_DB *sw_db)
{
    Dpp_wb_engine_Arad_pp_sw_db = sw_db;
    return SOC_E_NONE;
}

#define PP_SW_DB_DEVICE (Dpp_wb_engine_Arad_pp_sw_db->device[unit])

#endif /* BCM_ARAD_SUPPORT */

#ifdef VALGRIND_DEBUG

/* This is used to force the generation of code (machine instructions) for assignments to this variable. */
/* Specifically we want to avoid the situation where the compiler decides to optimize out the assignment. */
static int _dpp_wb_engine_valgrind_init_check_var = 0;

/* May be used for debugging VALGRIND errors. */
/* When enabled this code "uses" all the contents of var. */
/* "Uses" in this context means doing a branch depending on the contents of var. */
/* This is done to allow Valgrind to throw an error when uninitialized memory is written */
/* into a warmboot variable. */
/* If you have a Valgrind error here, then you are probably writing uninitialized memory to a warmboot array/variable. */
STATIC void _dpp_wb_engine_valgrind_check_array_is_initialized(const void *arr, uint32 outer_length, uint32 inner_length, uint32 outer_jump, uint32 inner_jump, uint32 data_size, int var)
{
  uint32 outer_arr_ndx, inner_arr_ndx;
  const uint8 *src;
  int i;

  if (arr == NULL) {
    return;
  }

  if (outer_jump == 0xffffffff) {
    outer_jump = inner_length * data_size;
  }

  if (inner_jump == 0xffffffff) {
    inner_jump = data_size;
  }

  for (outer_arr_ndx = 0; outer_arr_ndx < outer_length; outer_arr_ndx++) {
    for (inner_arr_ndx = 0; inner_arr_ndx < inner_length; inner_arr_ndx++) {

      src = (uint8*)arr + (outer_arr_ndx * outer_jump) + (inner_arr_ndx * inner_jump);

      for (i = 0; i < data_size; i++) {
        int byte = src[i];
        if (byte) {
          _dpp_wb_engine_valgrind_init_check_var = byte;
        } else {
          _dpp_wb_engine_valgrind_init_check_var = byte - 1;
       }
      }
    }
  }
}
#else /* VALGRIND_DEBUG */
#endif /* VALGRIND_DEBUG */

STATIC int soc_dpp_wb_engine_init_tables(int unit, int buffer_id);



int
soc_dpp_wb_engine_enable_dynamic_var(int unit, int var_idx, soc_wb_engine_dynamic_var_info_t var, uint8 *data_orig)
{
    return soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx, var, data_orig);
}

int
soc_dpp_wb_engine_disable_dynamic_var(int unit, int var_idx)
{
    return soc_wb_engine_disable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx);
}

/* enable/disable occ_bm */ 
int
soc_dpp_wb_engine_enable_dynamic_occ_bm(int unit, int var_idx, SOC_SAND_OCC_BM_PTR occ_bm)
{
    soc_wb_engine_dynamic_var_info_t var;
    int rv = SOC_E_NONE;

    sal_memset(&var, 0, sizeof(var));

    var.offset = 0; /*ignored*/
    var.flags = 0;
    var.outer_arr_length = 1;
    var.outer_arr_jump = 0;
    var.inner_arr_jump = sizeof(uint8);
    var.is_enabled = 1;
    var.data_size = sizeof(uint8);

    var.inner_arr_length = 1;
    rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx  + WB_ENGINE_OCC_BM_CACHE_ENABLED, var, &(occ_bm->cache_enabled));
    SOCDNX_IF_ERR_RETURN(rv);

    var.inner_arr_length = occ_bm->buffer_size;
    rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx + WB_ENGINE_OCC_BM_LEVELS_BUFFER, var, occ_bm->levels_buffer);
    SOCDNX_IF_ERR_RETURN(rv);

    if (occ_bm->support_cache) {
        var.inner_arr_length = occ_bm->buffer_size;
        rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx + WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, var, occ_bm->levels_cache_buffer);
        SOCDNX_IF_ERR_RETURN(rv);
    }

    return rv;
}

int
soc_dpp_wb_engine_disable_dynamic_occ_bm(int unit, int var_idx)
{
    int i, 
        rv = SOC_E_NONE;

    for (i = var_idx; i < (var_idx + WB_ENGINE_OCC_BM_INNER_VARS_NUM); i++) {
        rv = soc_wb_engine_disable_dynamic_var(unit, SOC_DPP_WB_ENGINE, i);
        SOCDNX_IF_ERR_RETURN(rv);
    }

    return rv;
}

/* enable/disable hash table */ 
int
soc_dpp_wb_engine_enable_dynamic_hash_tbl(int unit, int var_idx, SOC_SAND_HASH_TABLE_INFO hash_tbl)
{
    soc_wb_engine_dynamic_var_info_t var;
    int rv = SOC_E_NONE;

    var.offset = 0; /*ignored*/
    var.flags = 0;
    var.outer_arr_length = 1;
    var.outer_arr_jump = 0;
    var.inner_arr_jump = 0;
    var.is_enabled = 1;
    

    var.data_size = hash_tbl.init_info.key_size;
    var.inner_arr_jump = hash_tbl.init_info.key_size;
    var.inner_arr_length = hash_tbl.init_info.table_size;
    rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx + WB_ENGINE_HASH_TABLE_KEYS, var, hash_tbl.hash_data.keys);
    SOCDNX_IF_ERR_RETURN(rv);

    var.data_size = hash_tbl.hash_data.ptr_size;
    var.inner_arr_jump = hash_tbl.hash_data.ptr_size;
    var.inner_arr_length = hash_tbl.init_info.table_size;
    rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx + WB_ENGINE_HASH_TABLE_NEXT, var, hash_tbl.hash_data.next);
    SOCDNX_IF_ERR_RETURN(rv);

    var.data_size = hash_tbl.hash_data.ptr_size;
    var.inner_arr_jump = hash_tbl.hash_data.ptr_size;
    var.inner_arr_length = hash_tbl.init_info.table_width;
    rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx + WB_ENGINE_HASH_TABLE_LISTS_HEAD, var, hash_tbl.hash_data.lists_head);
    SOCDNX_IF_ERR_RETURN(rv);

    rv =  soc_dpp_wb_engine_enable_dynamic_occ_bm(unit, var_idx + WB_ENGINE_HASH_TABLE_MEMORY_USE, hash_tbl.hash_data.memory_use);
    SOCDNX_IF_ERR_RETURN(rv);

    return rv;
}

int
soc_dpp_wb_engine_disable_dynamic_hash_tbl(int unit, int var_idx)
{
    int i, 
        rv = SOC_E_NONE;

    for (i = var_idx; i < (var_idx + WB_ENGINE_HASH_TABLE_INNER_VARS_NUM); i++) {
        rv = soc_wb_engine_disable_dynamic_var(unit, SOC_DPP_WB_ENGINE, i);
        SOCDNX_IF_ERR_RETURN(rv);
    }

    return rv;
}

/* enable/disable hash table */ 
int
soc_dpp_wb_engine_enable_dynamic_sorted_list(int unit, int var_idx, SOC_SAND_SORTED_LIST_INFO sorted_list)
{
    soc_wb_engine_dynamic_var_info_t var;
    int rv = SOC_E_NONE;

    sal_memset(&var, 0, sizeof(var));

    var.offset = 0; /*ignored*/
    var.flags = 0;
    var.outer_arr_length = 1;
    var.outer_arr_jump = 0;
    var.inner_arr_jump = 0;
    var.is_enabled = 1;
    

    var.data_size = sorted_list.init_info.key_size;
    var.inner_arr_jump = sorted_list.init_info.key_size;
    var.inner_arr_length = sorted_list.init_info.list_size;

    rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx + WB_ENGINE_SORTED_LIST_KEYS, var, sorted_list.list_data.keys);
    SOCDNX_IF_ERR_RETURN(rv);

    var.data_size = sorted_list.list_data.ptr_size;
    var.inner_arr_jump = sorted_list.list_data.ptr_size;
    var.inner_arr_length = sorted_list.init_info.list_size + 2;
    rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx + WB_ENGINE_SORTED_LIST_NEXT, var, sorted_list.list_data.next);
    SOCDNX_IF_ERR_RETURN(rv);

    var.data_size = sorted_list.list_data.ptr_size;
    var.inner_arr_jump = sorted_list.list_data.ptr_size;
    var.inner_arr_length = sorted_list.init_info.list_size + 2;
    rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx + WB_ENGINE_SORTED_LIST_PREV, var, sorted_list.list_data.prev);
    SOCDNX_IF_ERR_RETURN(rv);

    var.data_size = sorted_list.init_info.data_size;
    var.inner_arr_jump = sorted_list.init_info.data_size;
    var.inner_arr_length = sorted_list.init_info.list_size;
    rv =  soc_wb_engine_enable_dynamic_var(unit, SOC_DPP_WB_ENGINE, var_idx + WB_ENGINE_SORTED_LIST_DATA, var, sorted_list.list_data.data);
    SOCDNX_IF_ERR_RETURN(rv);

    rv =  soc_dpp_wb_engine_enable_dynamic_occ_bm(unit, var_idx + WB_ENGINE_SORTED_LIST_MEMORY_USE, sorted_list.list_data.memory_use);
    SOCDNX_IF_ERR_RETURN(rv);

    return rv;
}

int
soc_dpp_wb_engine_disable_dynamic_sorted_list(int unit, int var_idx)
{
    int i, 
        rv = SOC_E_NONE;

    for (i = var_idx; i < (var_idx + WB_ENGINE_SORTED_LIST_INNER_VARS_NUM); i++) {
        rv = soc_wb_engine_disable_dynamic_var(unit, SOC_DPP_WB_ENGINE, i);
        SOCDNX_IF_ERR_RETURN(rv);
    }

    return rv;
}

int soc_dpp_wb_engine_init_buffer(int unit, int buffer_id)
{
    int rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    /* 
     * init the buffer info and buffer's variables info in wb engine tables, 
     * each buffer init it's own data. 
     * note that the location of original data is different in each boot.
     */ 
    rc = soc_dpp_wb_engine_init_tables(unit, buffer_id);
    if (rc != SOC_E_NONE) {
        SOCDNX_IF_ERR_EXIT(rc);
    }

    rc = soc_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE, buffer_id, FALSE);
    if (rc != SOC_E_NONE) {
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * init of engine tables is done per buffer due to timing issues. 
 * init of a variable should be done only after variable location is determined 
 * (if vars are dynamically alocated then init must be called after relevant allocations occured)  
 */ 

/* general definitions to be included at the beginning of engine's init_tables functiom  */
/* these defs are later used by ADD_DYNAMIC_BUFF and ADD_DYNAMIC_VAR_WITH_FEATURES below */

STATIC int
soc_dpp_wb_engine_init_tables(int unit, int buffer_id)
{
    int rv = SOC_E_NONE;
#ifdef BCM_ARAD_SUPPORT
    int i, j;
#endif /* BCM_ARAD_SUPPORT */
    WB_ENGINE_INIT_TABLES_DEFS;
    SOCDNX_INIT_FUNC_DEFS;
    sal_memset(&tmp_var_info, 0, sizeof(tmp_var_info));

    /* all of dpp_wb_engine depracated code was packed into this macro */
    /* make sure not to use any of the logic inside it and avoid making any changes to it */
    /* unless it's a crucial bugfix */
    SOC_DPP_WB_ENGINE_DEPRECATED_CODE;

#ifdef BCM_ARAD_SUPPORT

        SOC_DPP_WB_ENGINE_DECLARATIONS_BEGIN
    /*  add your buffer and vars declarations here in the form:
     *  SOC_DPP_WB_ENGINE_ADD_BUFF()
     *  {
     *      SOC_DPP_WB_ENGINE_ADD_VAR()
     *      SOC_DPP_WB_ENGINE_ADD_ARR()
     *      SOC_DPP_WB_ENGINE_ADD_2D_ARR()
     *  }
     */

        SOC_DPP_WB_ENGINE_ADD_BUFF(SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_INLIF_SW_DB, "Local InLIF SW DB used by Jericho", NULL, VERSION(1), SOC_WB_ENGINE_PRE_RELEASE)
        {
            SOC_DPP_WB_ENGINE_ADD_VAR(SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_INIT,
                "Local Inlif initalized",
                SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_INLIF_SW_DB,
                sizeof(uint8),
                VERSION(1));

            SOC_DPP_WB_ENGINE_ADD_ARR(SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_DB,
                "Local Inlif Database",
                SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_INLIF_SW_DB,
                sizeof(bcm_dpp_am_local_inlif_info_t),
                _BCM_DPP_AM_INGRESS_LIF_NOF_INLIF_IDS,
                VERSION(1));

             SOC_DPP_WB_ENGINE_ADD_ARR(SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO,
                "Local Inlif Bank Info",
                SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_INLIF_SW_DB,
                sizeof(bcm_dpp_am_local_inlif_table_bank_info_t),
                _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS,
                VERSION(1));

             SOC_DPP_WB_ENGINE_ADD_ARR(SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_APP_TYPE_INFO,
                "Local Inlif Application types Info",
                SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_INLIF_SW_DB,
                sizeof(bcm_dpp_am_local_inlif_application_type_info_t),
                bcm_dpp_am_ingress_lif_nof_app_types,
                VERSION(1));
        }
        SOC_DPP_WB_ENGINE_ADD_BUFF(SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_OUTLIF_SW_DB, "Local OutLIF SW DB used by Jericho", NULL, VERSION(1), SOC_WB_ENGINE_PRE_RELEASE)
        {
            SOC_DPP_WB_ENGINE_ADD_VAR(SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_OUTLIF_INIT,
                "Local Outlif initalized",
                SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_OUTLIF_SW_DB,
                sizeof(uint8),
                VERSION(1));

            SOC_DPP_WB_ENGINE_ADD_ARR(SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_OUTLIF_DB,
                "Local outlif Database",
                SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_OUTLIF_SW_DB,
                sizeof(bcm_dpp_am_local_out_lif_info_t),
                _BCM_DPP_AM_INGRESS_LIF_NOF_OUTLIF_IDS,
                VERSION(1));

        }

        SOC_DPP_WB_ENGINE_ADD_BUFF(SOC_DPP_WB_ENGINE_BUFFER_JER_EEDB_BANKS_SW_DB, "EEDB bank configuration SW DB used by Jericho", NULL, VERSION(1), SOC_WB_ENGINE_PRE_RELEASE)
        {
             SOC_DPP_WB_ENGINE_ADD_ARR(SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO,
                "EEDB banks",
                SOC_DPP_WB_ENGINE_BUFFER_JER_EEDB_BANKS_SW_DB,
                sizeof(arad_pp_eg_encap_eedb_bank_info_t),
                _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS,
                VERSION(1));

             SOC_DPP_WB_ENGINE_ADD_ARR(SOC_DPP_WB_ENGINE_VAR_JER_TOP_BANK_INFO,
                "EEDB Top banks",
                SOC_DPP_WB_ENGINE_BUFFER_JER_EEDB_BANKS_SW_DB,
                sizeof(arad_pp_eg_encap_eedb_top_bank_info_t),
                _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS,
                VERSION(1));
        }

        /* !! dont add new declarations below this point !! */
        SOC_DPP_WB_ENGINE_DECLARATIONS_END

#endif /* BCM_ARAD_SUPPORT */

    /* validate that only one non-empty buffer is initialized per function call*/
    SOC_WB_ENGINE_INIT_TABLES_SANITY(rv);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}




/* is called on device init to allocate the engine's tables */
int
soc_dpp_wb_engine_init(int unit)
{
    /* init pointers array - to be used to support addition of sw_state that was created prior to wb_engine*/
    return soc_wb_engine_init_tables(unit, SOC_DPP_WB_ENGINE, SOC_DPP_WB_ENGINE_NOF_BUFFERS, SOC_DPP_WB_ENGINE_VAR_NOF_VARS);
}

int
soc_dpp_wb_engine_deinit(int unit)
{
#ifdef BCM_ARAD_SUPPORT
    /* return static vars to default values*/
    sal_memset(&_dpp_am_egress_encap[unit] ,0, sizeof(bcm_dpp_am_egress_encap_t));
    sal_memset(&_dpp_am_ingress_lif[unit] ,0, sizeof(bcm_dpp_am_ingress_lif_t));
    sal_memset(&_dpp_am_sync_lif[unit] ,0, sizeof(bcm_dpp_am_sync_lif_t));
    sal_memset(&_dpp_oam_is_init[unit] ,0, sizeof(uint8));
    sal_memset(&_dpp_bfd_is_init[unit] ,0, sizeof(uint8));
    sal_memset(&_dpp_oam_acc_ref_counter[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_oam_traps_ref_counter[unit] ,0, sizeof(uint32) * (SOC_PPD_NOF_TRAP_CODES));
    sal_memset(&_dpp_bfd_mpls_udp_sport_ref_counter[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_oam_y1731_opcode_to_intenal_opcode_map[unit] ,0, sizeof(uint8));
    sal_memset(&_dpp_bfd_ipv4_multi_hop_acc_ref_counter[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_bfd_pdu_ref_counter[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_bfd_mpls_tp_cc_ref_counter[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_bfd_mpls_tp_cc_ref_counter[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_trap_code_recycle[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_trap_code_snoop[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_trap_code_frwrd[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_trap_code_drop[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_trap_code_trap_to_cpu_level[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_trap_code_trap_to_cpu_passive[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_bfd_trap_code_trap_to_cpu[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_bfd_trap_code_oamp_bfd_cc_mpls_tp[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_bfd_trap_code_oamp_bfd_ipv4[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_bfd_trap_code_oamp_bfd_mpls[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_bfd_trap_code_oamp_bfd_pwe[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_mirror_profile_snoop_to_cpu[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_mirror_profile_recycle[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_mirror_profile_err_level[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_oam_mirror_profile_err_passive[unit] ,0, sizeof(uint32));
    sal_memset(&_dpp_arad_pp_tcam_last_entry_id[unit] ,0, sizeof(uint32));
#endif
    return soc_wb_engine_deinit_tables(unit, SOC_DPP_WB_ENGINE);
}


/* bitstream function to replace sand_bitstream for warmboot recoverable data*/
int
soc_dpp_wb_engine_sand_bitstream_set_any_field(
    int       unit,
    int       var_ndx,
    uint32    outer_array_ndx,
    uint32    start_bit,
    uint32    nof_bits,
    uint32    *input_buffer)
{
    int i;
    uint32 res;
    soc_error_t rv;
    uint32 *output_buffer = NULL;
    uint32 alloc_count, start_reg, end_reg, end_bit;

    SOCDNX_INIT_FUNC_DEFS;

    end_bit = start_bit + nof_bits - 1;
    start_reg = (start_bit>>5);
    end_reg = (end_bit>>5);
    alloc_count = end_reg - start_reg + 1;

    SOCDNX_ALLOC(output_buffer, uint32, alloc_count, "bitstream");

    for (i=start_reg; i<=end_reg; i++) {
        rv = soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PRIMARY, var_ndx, outer_array_ndx, i, (uint8 *)(&(output_buffer[i-start_reg])));
        SOCDNX_IF_ERR_EXIT(rv);
    }

    res = soc_sand_bitstream_set_any_field(input_buffer,(start_bit&0x0000001F),nof_bits,output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    for (i=start_reg; i<=end_reg; i++) {
        rv = soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PRIMARY, var_ndx, outer_array_ndx, i, (uint8 *)(&(output_buffer[i-start_reg])));
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FREE(output_buffer);
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_wb_engine_sand_bitstream_get_any_field(
    int       unit,
    int       var_ndx,
    uint32    outer_array_ndx,
    uint32    start_bit,
    uint32    nof_bits,
    uint32    *output_buffer
  )
{
    uint32 *input_buffer = NULL;
    uint32 res;
    soc_error_t rv;
    int i;
    uint32 alloc_count, start_reg, end_reg, end_bit;

    SOCDNX_INIT_FUNC_DEFS;

    end_bit = start_bit + nof_bits - 1;
    start_reg = (start_bit>>5);
    end_reg = (end_bit>>5);
    alloc_count = end_reg - start_reg + 1;

    SOCDNX_ALLOC(input_buffer, uint32, alloc_count, "bitstream");

    for (i=start_reg; i<=end_reg; i++) {
        rv = soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PRIMARY, var_ndx, outer_array_ndx, i, (uint8 *)(&(input_buffer[i-start_reg])));
        SOCDNX_IF_ERR_EXIT(rv);
    }

    res = soc_sand_bitstream_get_any_field(input_buffer,(start_bit&0x0000001F),nof_bits,output_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FREE(input_buffer);
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_wb_engine_sand_bitstream_set(
    int       unit,
    int       var_ndx,
    uint32    place,
    uint32    bit_indicator
  )
{
    uint32 bit_stream=0;
    uint32 res;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PRIMARY, var_ndx, 0, (place>>5), (uint8 *)(&bit_stream));
    SOCDNX_IF_ERR_EXIT(rv);

    /*call regular bitstream operation with only the bit offset inside the register*/
    res = soc_sand_bitstream_set((&bit_stream),(place & 0x0000001F),bit_indicator);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    rv = soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PRIMARY, var_ndx, 0, (place>>5), (uint8 *)(&bit_stream));
    SOCDNX_IF_ERR_EXIT(rv);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_wb_engine_sand_bitstream_test_bit(
    int       unit,
    int       var_ndx,
    uint32    place,
    uint8     *result
  )
{
    uint32 bit_stream;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PRIMARY, var_ndx, 0, (place>>5), (uint8 *)(&bit_stream)));
    *result = (bit_stream & SOC_SAND_BIT(place & 0x0000001F)) > 0;

exit:
    SOCDNX_FUNC_RETURN;
}

#ifdef BCM_WARM_BOOT_SUPPORT

int 
soc_dpp_wb_engine_update_var_info(int unit, int var_idx, uint32 data_size, uint32 outer_arr_length, uint32 inner_arr_length)
{
    return soc_wb_engine_update_var_info(unit, SOC_DPP_WB_ENGINE, var_idx, data_size, outer_arr_length, inner_arr_length);
}

int
soc_dpp_wb_engine_sync(int unit)
{
#ifdef BCM_ARAD_SUPPORT
#ifdef BCM_WARM_BOOT_SUPPORT
    int i;
    int rv;
#endif
#endif

#ifdef BCM_ARAD_SUPPORT
#ifdef BCM_WARM_BOOT_SUPPORT   
    /* prepare bcm hash tables data to be saved to external storage*/
    for (i=0; i<SOC_DPP_WB_HASH_TBLS_NUM; i++) {
        rv = dpp_fill_wb_arrays_from_hash_tbl(unit,i);
        if(rv != SOC_E_NONE){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Unit:%d failed to prepare hash table %d for wb during soc_dpp_wb_engine_sync\n"),
                       unit, i));
            return rv;
        }
    }
#endif
#endif

    return soc_wb_engine_sync(unit, SOC_DPP_WB_ENGINE);
}

#endif /*BCM_WARM_BOOT_SUPPORT*/

/* defined at the beginning of the module */
#undef _ERR_MSG_MODULE_NAME
#undef NEVER_REMOVED
#undef NO_FLAGS
#undef DEFAULT
#undef VERSION
#undef _NO_JUMP_NEEDED_



