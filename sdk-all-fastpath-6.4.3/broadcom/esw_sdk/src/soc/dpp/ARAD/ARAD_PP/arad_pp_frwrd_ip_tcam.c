#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_frwrd_ip_tcam.c,v 1.52 Broadcom SDK $
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
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD

/* uncomment this flag to enable time measurments */
/* #define ARAD_PP_KBP_TIME_MEASUREMENTS */

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>

#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv6.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_ipv6.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>

#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
#include <soc/dpp/SAND/Management/sand_low_level.h>
#endif 
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_FRWRD_IP_TCAM_MIN_BANKS                          (0)

#define ARAD_PP_FRWRD_IPV4_MC_PREFIX_SIZE                        (0)
#define ARAD_PP_FRWRD_IPV6_UC_PREFIX_SIZE                        (SOC_IS_JERICHO(unit)? 0: 4)
#define ARAD_PP_FRWRD_IPV6_MC_PREFIX_SIZE                        (4)



/* priority of each key field in TCAM */
#define ARAD_PP_IPMC_DIP_PRIO    (32)
#define ARAD_PP_IPMC_RIF_PRIO    (10)






#define ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(input_bitstream, nof_bits, output_stream) \
  res = soc_sand_bitstream_set_any_field(     \
    input_bitstream,                        \
    start_bit,                              \
    nof_bits,                               \
    output_stream                           \
  );                                      \
  SOC_SAND_CHECK_FUNC_RESULT(res,  23, exit); \
  start_bit += nof_bits;

#define ARAD_PP_IPV6_TCAM_ENTRY_TO_VRF_AND_SUBNET(key_param, vrf_ndx_param, subnet_param) \
  if(key_param->type == ARAD_IP_TCAM_ENTRY_TYPE_IPV6_UC)                   \
  {                                                                   \
    vrf_ndx_param = &key_param->vrf_ndx;                                     \
    subnet_param = &key_param->key.ipv6_uc.subnet;                                      \
  }                                                                   \
  else if(key->type == ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN)  \
  {                                                       \
    vrf_ndx_param = &key_param->vrf_ndx;                         \
    subnet_param = &key_param->key.ipv6_vpn.subnet;                          \
  } \
  else \
  { \
    vrf_ndx_param = NULL; \
    subnet_param = NULL; \
  }

#define ARAD_PP_IP_TCAM_ENTRY_KEY_TYPE_IPV6_UC_OR_VPN 0
#define ARAD_PP_IP_TCAM_ENTRY_KEY_TYPE_IPV6_MC 1
#define ARAD_PP_IP_TCAM_ENTRY_KEY_TYPE_NOF_BITS 1

#define ARAD_PP_FRWRD_IPV6_SUPPORT (1)


/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/

/* { */

/*
 *  Internal functions
 */

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
/* 
 * KBP access level functions: 
 * - once the buffer of the content and the actions is obtained, 
 * insertion / removal / get / get-block at low level 
 */

#ifdef KBP_COMPARE_DEBUG
uint32
    arad_pp_frwrd_ip_tcam_kbp_route_compare(
       SOC_SAND_IN  int              unit,
       SOC_SAND_IN  uint8               ltr_num,
       SOC_SAND_OUT uint8               *master_key,
       SOC_SAND_OUT uint8               *found
    )
{
    uint32
        res;
    struct kbp_device *device_p = NULL;
    struct kbp_instruction *inst_p = NULL;
    struct kbp_search_result cmp_rslt;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = FALSE;

    sal_memset(&cmp_rslt,0,sizeof(struct kbp_search_result));

    res = arad_kbp_get_device_pointer(
            unit,
            &device_p
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_CHECK_NULL_PTR(device_p, 20, exit);
    
    res = arad_kbp_alg_kbp_get_inst_pointer(
            unit,
            ltr_num,
            &inst_p
          );
    SOC_SAND_CHECK_NULL_PTR(inst_p, 30, exit);

    kbp_instruction_print(inst_p,stdout);

    res = kbp_instruction_search(
            inst_p, 
            master_key, 
            0, 
            &cmp_rslt
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_kbp_route_compare()",0,0);
}
#endif /* KBP_COMPARE_DEBUG */



STATIC
  uint32
    arad_pp_frwrd_ip_tcam_kbp_route_get(
       SOC_SAND_IN  int             unit,
       SOC_SAND_IN  uint8           frwrd_table_id,
       SOC_SAND_IN  uint32          data_indx,
       SOC_SAND_OUT uint8           *data,
       SOC_SAND_OUT uint8           *mask,
       SOC_SAND_OUT uint8           *assoData,
       SOC_SAND_OUT uint32          *priority,
       SOC_SAND_OUT uint8           *found,
       SOC_SAND_OUT int             *valid_bytes
    )
{
    uint32
        res;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    ARAD_SW_KBP_HANDLE
        location;
    struct kbp_entry_info
        entry_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = FALSE;

    res =  arad_kbp_alg_kbp_ad_db_get(
            unit,
            frwrd_table_id, 
            &ad_db_p
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 20, exit);

    /* Retrieve the record index */
    res = arad_sw_db_frwrd_ip_kbp_location_tbl_get(unit, data_indx, &location);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    /* Consider the entry not found if the db_entry is null */
    if (location.db_entry == NULL) {
        *found = FALSE;
        ARAD_DO_NOTHING_AND_EXIT;
    }
    SOC_SAND_CHECK_NULL_PTR(location.db_entry, 27, exit);    
    
    res = arad_kbp_alg_kbp_db_get(unit, frwrd_table_id, &db_p);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    /* Retrieve the entry key */
    res = kbp_entry_get_info(db_p, location.db_entry, &entry_info);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 35, exit);
    }

    (*valid_bytes) = entry_info.width_8;
    sal_memcpy(data, entry_info.data, sizeof(uint8) * (entry_info.width_8));
    sal_memcpy(mask, entry_info.mask, sizeof(uint8) * (entry_info.width_8));

    SOC_SAND_CHECK_NULL_PTR(entry_info.ad_handle, 40, exit);

    sal_memset(assoData, 0x0, sizeof(uint8) * SOC_DPP_TCAM_ACTION_ELK_KBP_GET_LEN_BYTES(unit));

    res = kbp_ad_db_get(ad_db_p, entry_info.ad_handle, assoData);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
    }

    (*priority) = entry_info.prio_len;
    (*found) = TRUE;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_kbp_route_get()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ip_tcam_kbp_lpm_route_get(
       SOC_SAND_IN  int         unit,
       SOC_SAND_IN  uint8       frwrd_table_id,
       SOC_SAND_IN  uint32      prefix_len,
       SOC_SAND_OUT uint8       *data,
       SOC_SAND_OUT uint8       *assoData,
       SOC_SAND_OUT uint8       *found
    )
{
    uint32
        res;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    struct kbp_ad 
        *ad_entry = NULL;
    struct kbp_entry
        *db_entry = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = FALSE;

    res =  arad_kbp_alg_kbp_db_get(
            unit,
            frwrd_table_id, 
            &db_p
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_CHECK_NULL_PTR(db_p, 20, exit);

    /* Retrieve the db_entry */
    kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if (!db_entry)
        goto exit;

    /* Retrieve the ad_entry */
    res =  arad_kbp_alg_kbp_ad_db_get(
            unit,
            frwrd_table_id, 
            &ad_db_p
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 40, exit);
    res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }

    SOC_SAND_CHECK_NULL_PTR(ad_entry, 90, exit);

    sal_memset(assoData, 0x0, sizeof(uint8) * SOC_DPP_TCAM_ACTION_ELK_KBP_GET_LEN_BYTES(unit));

    res = kbp_ad_db_get(ad_db_p, ad_entry, assoData);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    }

    *found = TRUE;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_kbp_lpm_route_get()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ip_tcam_kbp_route_remove(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  uint8                       frwrd_table_id,
       SOC_SAND_IN  uint32                      data_indx
    )
{
    uint32
        res, kbp_cache_mode;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    struct kbp_entry
        *db_entry;
    struct kbp_ad 
       *ad_entry = NULL;
    ARAD_SW_KBP_HANDLE
        location;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Retrieve the record index */
    res = arad_sw_db_frwrd_ip_kbp_location_tbl_get(unit, data_indx, &location);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    db_entry = location.db_entry;
    SOC_SAND_CHECK_NULL_PTR(db_entry, 10, exit);

    res = arad_kbp_alg_kbp_db_get(unit, frwrd_table_id, &db_p);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    /* Retrieve the ad_entry */
    res = kbp_entry_get_ad(db_p, location.db_entry, &ad_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 37, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(ad_entry, 20, exit);

    SOC_SAND_CHECK_NULL_PTR(db_p, 40, exit);

    res = kbp_db_delete_entry( 
            db_p,
            db_entry
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
    }

    res = arad_kbp_alg_kbp_ad_db_get(
            unit,
            frwrd_table_id, 
            &ad_db_p
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = kbp_ad_db_delete_entry(
            ad_db_p, 
            ad_entry
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Error in %s(): kbp_ad_db_delete_entry failed with error: %s!\n"), 
                              FUNCTION_NAME(),
                   kbp_get_status_string(res)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
    } 

    res = arad_sw_db_frwrd_ip_kbp_cache_mode_get(unit, &kbp_cache_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
    if (kbp_cache_mode == FALSE) {
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_set("arad_pp_frwrd_ip_tcam_kbp_route_remove", 3);
#endif 
        res = kbp_db_install(db_p);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_stop(3);
#endif 
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
        }
    }

    /* If remove was sucessfull clear location table index */
    sal_memset(&location, 0x0, sizeof(ARAD_SW_KBP_HANDLE));
    res = arad_sw_db_frwrd_ip_kbp_location_tbl_set(unit, data_indx, &location);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_kbp_route_remove()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ip_tcam_kbp_lpm_route_remove(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      frwrd_table_id,
       SOC_SAND_IN  uint32                      prefix_len,
       SOC_SAND_OUT uint8                       *data
    )
{
    uint32
        res, kbp_cache_mode;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    struct kbp_ad 
       *ad_entry = NULL;
    struct kbp_entry 
       *db_entry = NULL;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_kbp_alg_kbp_db_get(unit, frwrd_table_id, &db_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(db_p, 20, exit);

    /* Retrieve the db_entry */
    res = kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(db_p, 40, exit);

    /* Retrieve the ad_entry */
    res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(ad_entry, 60, exit);

    res = kbp_db_delete_entry( 
            db_p,
            db_entry
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
    }

    res = arad_kbp_alg_kbp_ad_db_get(
            unit,
            frwrd_table_id, 
            &ad_db_p
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    res = kbp_ad_db_delete_entry(
            ad_db_p, 
            ad_entry
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_ad_db_delete_entry failed with error: %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
    } 

    res = arad_sw_db_frwrd_ip_kbp_cache_mode_get(unit, &kbp_cache_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    if (kbp_cache_mode == FALSE) {

#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_set("arad_pp_frwrd_ip_tcam_kbp_lpm_route_remove", 4);
#endif 
        res = kbp_db_install(db_p);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_stop(4);
#endif 
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_kbp_lpm_route_remove()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ip_tcam_kbp_route_add(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  uint32                      frwrd_table_id,
       SOC_SAND_IN  uint32                      data_indx,
       SOC_SAND_IN  uint8                       is_for_update,
       SOC_SAND_IN  uint32                      priority,
       SOC_SAND_OUT  uint8                      *m_data,
       SOC_SAND_OUT  uint8                      *m_mask,
       SOC_SAND_OUT  uint8                      *assoData,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
    )
{
    int32
        res ;
    
    tableInfo 
        tbl_info;
    struct kbp_db 
       *db_p = NULL;
    struct kbp_ad_db 
       *ad_db_p = NULL;
    struct kbp_entry 
       *db_entry = NULL;
    struct kbp_ad 
       *ad_entry = NULL;
    ARAD_SW_KBP_HANDLE
       location;
    uint32
        kbp_cache_mode;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *success = SOC_SAND_SUCCESS;

    res = arad_kbp_gtm_table_info_get(
              unit,
              frwrd_table_id,
              &tbl_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_kbp_alg_kbp_db_get(
              unit,
              frwrd_table_id, 
              &db_p
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    res = kbp_db_add_ace(
            db_p, 
            m_data, 
            m_mask,
            priority,
            &db_entry);

    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        *success = SOC_SAND_FAILURE_INTERNAL_ERR;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
    }

    res =  arad_kbp_alg_kbp_ad_db_get(
            unit,
            frwrd_table_id, 
            &ad_db_p
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = kbp_ad_db_add_entry(
            ad_db_p,
            assoData,
            &ad_entry
          );

    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        *success = SOC_SAND_FAILURE_INTERNAL_ERR;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
    }

    res = kbp_entry_add_ad(
            db_p,
            db_entry,
            ad_entry
          );

    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        *success = SOC_SAND_FAILURE_INTERNAL_ERR;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
    }

    res = arad_sw_db_frwrd_ip_kbp_cache_mode_get(unit, &kbp_cache_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
    if (kbp_cache_mode == FALSE) {
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_set("arad_pp_frwrd_ip_tcam_kbp_route_add", 5);
#endif 
        res = kbp_db_install(db_p);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_stop(5);
#endif 
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "Error in %s(): Entry add : kbp_db_install with failed: %s!\n"), 
                       FUNCTION_NAME(),
                       kbp_get_status_string(res)));
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
        }
    }

    /* In case of update, remove the existing entry before updating the location table */
    if (is_for_update == TRUE) {
        res = arad_pp_frwrd_ip_tcam_kbp_route_remove(
                  unit,
                  frwrd_table_id,
                  data_indx
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    }

    /* Insert the location mapping in the table */
    location.db_entry = db_entry;
    res = arad_sw_db_frwrd_ip_kbp_location_tbl_set(unit, data_indx, &location);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_kbp_route_add()",0,0);
}


STATIC
  uint32
    arad_pp_frwrd_ip_tcam_kbp_lpm_route_add(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      frwrd_table_id,
       SOC_SAND_IN  uint32                      data_indx,
       SOC_SAND_IN  uint32                      prefix_len,
       SOC_SAND_OUT  uint8                      *data,
       SOC_SAND_OUT  uint8                      *assoData,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
    )
{
    int32
        res ;
    
    tableInfo 
        tbl_info;
    struct kbp_db 
       *db_p = NULL;
    struct kbp_ad_db 
       *ad_db_p = NULL;
    struct kbp_entry 
       *db_entry = NULL;
    struct kbp_ad 
       *ad_entry = NULL;
    uint32
        kbp_cache_mode;
    ARAD_SW_KBP_HANDLE
       location;
    int is_update = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *success = SOC_SAND_SUCCESS;
    
    
    res = arad_kbp_gtm_table_info_get(
              unit,
              frwrd_table_id,
              &tbl_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_kbp_alg_kbp_db_get(
              unit,
              frwrd_table_id, 
              &db_p
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    res =  arad_kbp_alg_kbp_ad_db_get(
            unit,
            frwrd_table_id, 
            &ad_db_p
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

    /* Check if the db_entry exists */
    kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if (db_entry != NULL) {
        is_update = 1;
    }

    if (!is_update) {
        res = kbp_db_add_prefix(
                db_p, 
                data, 
                prefix_len,
                &db_entry);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
        }

        res = kbp_ad_db_add_entry(
                ad_db_p,
                assoData,
                &ad_entry
              );

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }

        res = kbp_entry_add_ad(
                db_p,
                db_entry,
                ad_entry
              );

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
        }
    }
    else {
        res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
        }

        res = kbp_ad_db_update_entry(ad_db_p, ad_entry, assoData);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
        }
    }

    res = arad_sw_db_frwrd_ip_kbp_cache_mode_get(unit, &kbp_cache_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
    if (kbp_cache_mode == FALSE) {
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_set("arad_pp_frwrd_ip_tcam_kbp_lpm_route_add", 6);
#endif 
        res = kbp_db_install(db_p);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_stop(6);
#endif 
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): Entry add : kbp_db_install with failed: %s!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res)));
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
        }
    }

    if ((frwrd_table_id == ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED) || (frwrd_table_id == ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6) || (frwrd_table_id == ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P)) {
        location.db_entry = db_entry;
        res = arad_sw_db_frwrd_ip_kbp_location_tbl_set(unit, data_indx, &location);
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_kbp_lpm_route_add()",0,0);
}

uint32
    arad_pp_frwrd_ip_tcam_kbp_table_clear(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  uint32   frwrd_table_id
    )
{
    uint32
        res;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_kbp_alg_kbp_db_get(
              unit,
              frwrd_table_id, 
              &db_p
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    SOC_SAND_CHECK_NULL_PTR(db_p, 20, exit);

    res = arad_kbp_alg_kbp_ad_db_get(
            unit,
            frwrd_table_id, 
            &ad_db_p
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 40, exit);

    res = kbp_db_delete_all_entries(db_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
    }

    res = kbp_ad_db_delete_all_entries(ad_db_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_kbp_table_clear()",0,0);
}

#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

STATIC
  uint32
    arad_pp_frwrd_ip_tcam_callback(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  user_data
    )
{
  uint32
    res = SOC_SAND_OK,
    tcam_db_id,
    access_profile_id;
  uint8 program_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* Set access_profile id depending on TCAM DB ID */
  tcam_db_id = user_data;

  res = arad_sw_db_tcam_db_access_profile_id_get(
          unit,
          tcam_db_id,
          0, /* No 320b DB */
          &access_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch (user_data) /* tcam DB ID */
  {
  case ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_0:
    res = arad_pp_flp_lookups_ipv4uc_tcam_profile_set(
            unit,
            0,
            access_profile_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    break;
  case ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_1:
    res = arad_pp_flp_lookups_ipv4uc_tcam_profile_set(
            unit,
            1,
            access_profile_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    break;


  case ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_MC:
    if (SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable) {
      res = arad_pp_flp_lookups_ipv4compmc_with_rpf(
            unit,
              access_profile_id
            );
    } else {
      res = arad_pp_flp_app_to_prog_index_get(unit,
                                              ARAD_PP_FLP_KEY_PROGRAM_GLOBAL_IPV4COMPMC_WITH_RPF,
                                              &program_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      res = arad_pp_flp_lookups_global_ipv4compmc_with_rpf(
              unit,
              program_id,
              access_profile_id
            );
   }
   SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
   break;

  case ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_UC:
    res = arad_pp_flp_lookups_ipv6uc(
            unit,
            access_profile_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    break;

  case ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_MC:
      if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipv6_mc_forwarding_disable", 0)))
      {          
	        res = arad_pp_flp_lookups_ipv6mc(unit, access_profile_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);   
      }
    break;

  case ARAD_PP_FRWRD_TRILL_TCAM_ID_MC:
      res = arad_pp_flp_lookups_TRILL_mc(unit, TRUE, access_profile_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);   
    break;
  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_callback()", 0, 0);
}

uint32
  arad_pp_frwrd_ip_tcam_init_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  required_uses
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SUCCESS_FAILURE
    success;
  ARAD_TCAM_ACCESS_INFO
    tcam_access_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_TCAM_INIT_UNSAFE);

  /*
   *  Create TCAM databases
   */

  
  if (SOC_IS_JERICHO(unit)) {
      uint32
          loop_ndx;
      for (loop_ndx = 0; loop_ndx < 2; loop_ndx++) {
          arad_ARAD_TCAM_ACCESS_INFO_clear(&tcam_access_info);
          tcam_access_info.action_bitmap_ndx                   = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
          tcam_access_info.bank_owner                          = ARAD_TCAM_BANK_OWNER_FLP_TCAM;
          tcam_access_info.callback                            = arad_pp_frwrd_ip_tcam_callback;
          tcam_access_info.entry_size                          = ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS;
          tcam_access_info.is_direct                           = FALSE;
          tcam_access_info.min_banks                           = ARAD_PP_FRWRD_IP_TCAM_MIN_BANKS;
          tcam_access_info.prefix_size                         = 4;
          tcam_access_info.user_data                           = loop_ndx? ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_1: ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_0;
          res = arad_tcam_access_create_unsafe(
                  unit,
                  loop_ndx? ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_1: ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_0,
                  &tcam_access_info,
                  &success
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);    
      }
  }

  if (required_uses & ARAD_PP_FRWRD_IP_TCAM_IPV4_MC)
  {
    arad_ARAD_TCAM_ACCESS_INFO_clear(&tcam_access_info);

    tcam_access_info.action_bitmap_ndx                   = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
    tcam_access_info.bank_owner                          = ARAD_TCAM_BANK_OWNER_FLP_TCAM;
    tcam_access_info.callback                            = arad_pp_frwrd_ip_tcam_callback;
    tcam_access_info.entry_size                          = ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS;
    tcam_access_info.is_direct                           = FALSE;
    tcam_access_info.min_banks                           = ARAD_PP_FRWRD_IP_TCAM_MIN_BANKS;
    tcam_access_info.prefix_size                         = ARAD_PP_FRWRD_IPV4_MC_PREFIX_SIZE;
    tcam_access_info.user_data                           = ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_MC;

    res = arad_tcam_access_create_unsafe(
            unit,
            ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_MC,
            &tcam_access_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);    
  }

#if ARAD_PP_FRWRD_IPV6_SUPPORT
  if (required_uses & ARAD_PP_FRWRD_IP_TCAM_IPV6_UC)
  {
    arad_ARAD_TCAM_ACCESS_INFO_clear(&tcam_access_info);

    tcam_access_info.action_bitmap_ndx                   = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS | ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
    tcam_access_info.bank_owner                          = ARAD_TCAM_BANK_OWNER_FLP_TCAM;
    tcam_access_info.callback                            = arad_pp_frwrd_ip_tcam_callback;
    tcam_access_info.entry_size                          = ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS;
    tcam_access_info.is_direct                           = FALSE;
    tcam_access_info.min_banks                           = ARAD_PP_FRWRD_IP_TCAM_MIN_BANKS;
    tcam_access_info.prefix_size                         = ARAD_PP_FRWRD_IPV6_UC_PREFIX_SIZE;
    tcam_access_info.user_data                           = ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_UC;

    res = arad_tcam_access_create_unsafe(
            unit,
            ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_UC,
            &tcam_access_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      
  }

  if (required_uses & ARAD_PP_FRWRD_IP_TCAM_IPV6_MC)
  {
    arad_ARAD_TCAM_ACCESS_INFO_clear(&tcam_access_info);

    tcam_access_info.action_bitmap_ndx                   = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS | ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
    tcam_access_info.bank_owner                          = ARAD_TCAM_BANK_OWNER_FLP_TCAM;
    tcam_access_info.callback                            = arad_pp_frwrd_ip_tcam_callback;
    tcam_access_info.entry_size                          = ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS;
    tcam_access_info.is_direct                           = FALSE;
    tcam_access_info.min_banks                           = ARAD_PP_FRWRD_IP_TCAM_MIN_BANKS;
    tcam_access_info.prefix_size                         = ARAD_PP_FRWRD_IPV6_MC_PREFIX_SIZE;
    tcam_access_info.user_data                           = ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_MC;

    res = arad_tcam_access_create_unsafe(
            unit,
            ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_MC,
            &tcam_access_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);    
  }

  if (SOC_DPP_CONFIG(unit)->trill.transparent_service)
  {
    arad_ARAD_TCAM_ACCESS_INFO_clear(&tcam_access_info);

    tcam_access_info.action_bitmap_ndx                   = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
    tcam_access_info.bank_owner                          = ARAD_TCAM_BANK_OWNER_FLP_TCAM;
    tcam_access_info.callback                            = arad_pp_frwrd_ip_tcam_callback;
    tcam_access_info.entry_size                          = ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS;
    tcam_access_info.is_direct                           = FALSE;
    tcam_access_info.min_banks                           = ARAD_PP_FRWRD_IP_TCAM_MIN_BANKS;
    tcam_access_info.prefix_size                         = 4;
    tcam_access_info.user_data                           = ARAD_PP_FRWRD_TRILL_TCAM_ID_MC;

    res = arad_tcam_access_create_unsafe(
            unit,
            ARAD_PP_FRWRD_TRILL_TCAM_ID_MC,
            &tcam_access_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);    
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_init_unsafe()", required_uses, 0);
}

STATIC
  void
    arad_pp_ipv6_prefix_to_mask(
      SOC_SAND_IN  uint32 prefix,
      SOC_SAND_OUT uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S]
    )
{
  int32
    counter = prefix,
    word = 3;

  mask[0] = mask[1] = mask[2] = mask[3] = 0;

  while (counter >= 32)
  {
    mask[word--] = 0xffffffff;
    counter -= 32;
  }
  if (counter > 0)
  {
    mask[word] = SOC_SAND_BITS_MASK(31, 32 - counter);
  }
}

/*
 * for IP mc: 
 * <SIP,G,RIF> : -74
 * <SIP,G,*>   : -64
 * <*,G,RIF>   : -42
 * <*,G,*>     : -32 
 * <*,*,RIF>   : -10
 */
STATIC uint32
  arad_pp_ip_tcam_route_to_prio(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *key
  )
{
  uint32
    prio = SOC_SAND_U32_MAX;

  switch(key->type)
  {
      
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_UC_0:
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_UC_1:
    prio = prio - (uint32) key->key.ipv4_mc.source.prefix_len;
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_MC:

    /* RIF */
    prio = prio - ((uint32)ARAD_PP_IPMC_RIF_PRIO * key->key.ipv4_mc.inrif_valid);
    /* SIP */
    prio = prio - (uint32)key->key.ipv4_mc.source.prefix_len;
    /* DIP, if not invalid then higher with 32 */
    if(key->key.ipv4_mc.group != 0) {
        prio = prio - ARAD_PP_IPMC_DIP_PRIO;
    }
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_MC:
    prio = prio - (uint32)soc_sand_bitstream_get_nof_on_bits(&key->key.ipv6_mc.inrif.mask, 1);
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_UC:
    prio = prio - (uint32) key->key.ipv6_uc.subnet.prefix_len;
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
    prio = prio - (uint32) key->key.ipv6_vpn.subnet.prefix_len;
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
    if (SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit)){
      prio = prio - (uint32) key->key.ipv6_static_src_bind.prefix_len;
    } else {
  	  /* always 0 , all other TCAM insertions are exact match */
      prio = 0;
    }
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_TRILL_MC:
    prio = 0;
    break;
  default:
    break;
  }
   
  return prio;
}

STATIC
  uint32
    arad_pp_ip_tcam_key_to_stream(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *key,
      SOC_SAND_OUT uint8                       stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES]
  )
{
  uint32
    tmp,
    long_stream[ARAD_PP_IP_TCAM_KEY_LEN_LONGS];
  const uint32
    *vrf_ndx;
  uint8
    start_bit;
  uint32
    res;
  const SOC_SAND_PP_IPV6_SUBNET
    *subnet;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  start_bit = 0;
  tmp = 0;

  res = SOC_SAND_OK; 
  sal_memset(long_stream, 0x0, (sizeof(uint32) * ARAD_PP_IP_TCAM_KEY_LEN_LONGS));
  SOC_SAND_CHECK_FUNC_RESULT(res,  20, exit);

  switch(key->type)
  {
      
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_UC_0:
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_UC_1:
    tmp = key->key.ipv4_mc.inrif;
    ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), long_stream);

    ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&key->key.ipv4_mc.source.ip_address, 32, long_stream);
    tmp = key->key.ipv4_mc.source.prefix_len;
    ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, 8, long_stream);
    break;

    case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_MC:
      tmp = key->key.ipv4_mc.inrif_valid;
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, 1, long_stream);

      tmp = key->key.ipv4_mc.inrif;
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_DPP_DEFS_GET(unit, in_rif_nof_bits), long_stream);

      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&key->key.ipv4_mc.source.ip_address, 32, long_stream);

      tmp = key->key.ipv4_mc.source.prefix_len;
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, 8, long_stream);

      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&key->key.ipv4_mc.group, 28, long_stream);
      break;

    case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_MC:
      tmp = ARAD_PP_IP_TCAM_ENTRY_KEY_TYPE_IPV6_MC;
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, ARAD_PP_IP_TCAM_ENTRY_KEY_TYPE_NOF_BITS, long_stream);

      tmp = key->key.ipv6_mc.inrif.mask;
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_DPP_DEFS_GET(unit, in_rif_nof_bits), long_stream);

      tmp = key->key.ipv6_mc.inrif.val;
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_DPP_DEFS_GET(unit, in_rif_nof_bits), long_stream);

      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(key->key.ipv6_mc.group.address, ARAD_PP_IPV6_MC_GROUP_NOF_BITS, long_stream);
      break;

    case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_UC:
    case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
      tmp = ARAD_PP_IP_TCAM_ENTRY_KEY_TYPE_IPV6_UC_OR_VPN;
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, ARAD_PP_IP_TCAM_ENTRY_KEY_TYPE_NOF_BITS, long_stream);

      ARAD_PP_IPV6_TCAM_ENTRY_TO_VRF_AND_SUBNET(key, vrf_ndx, subnet);
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(vrf_ndx, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), long_stream);
      
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(subnet->ipv6_address.address, SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS, long_stream);

      tmp = subnet->prefix_len;
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_SAND_NOF_BITS_IN_CHAR, long_stream);
      break;
	case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(key->key.ipv6_static_src_bind.sip6.address,
                                          128,
                                          long_stream);
      if (SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit)) {
          tmp = key->key.ipv6_static_src_bind.prefix_len;
          ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_SAND_NOF_BITS_IN_CHAR, long_stream);
      }
      /*Lif index, 16 valid bits*/
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&(key->key.ipv6_static_src_bind.lif_ndx), 
										   16,
										   long_stream);
      break;
    case ARAD_IP_TCAM_ENTRY_TYPE_TRILL_MC:
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&(key->key.trill_mc.tree_nick),
                                          16,
                                          long_stream);
      tmp = key->key.trill_mc.esadi;
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp,
                                          1,
                                          long_stream);
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&(key->key.trill_mc.outer_vid),
                                          12,
                                          long_stream);
      break;
    default:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        {
            uint32 
                table_size_in_bytes,
                table_payload_in_bytes;

            res = arad_kbp_table_size_get(unit, ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(key->type), &table_size_in_bytes, &table_payload_in_bytes);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

            /* 
             * KBP keys: copy data and mask according to length written in number of bytes 
             * The length should be under 20 bytes (less than 160 bits) 
             */
            sal_memcpy(stream, key->key.elk_fwd.m_data, table_size_in_bytes);
            sal_memcpy(&(stream[table_size_in_bytes]), key->key.elk_fwd.m_mask, table_size_in_bytes);
        }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
      break;
  }

  /*
   *  If non ELK, add key type
   */
  if (!ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(key->type)) {
      tmp = key->type;
      soc_sand_bitstream_set_any_field(&tmp, 157, 3, long_stream);

      res = soc_sand_U32_to_U8(
              long_stream,
              ARAD_PP_IP_TCAM_KEY_LEN_BYTES,
              stream
            );
      SOC_SAND_CHECK_FUNC_RESULT(res,  100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_ip_tcam_key_to_stream()",0,0);
}

STATIC
  uint32
    arad_pp_ip_tcam_stream_to_key(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  uint8                 stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES],
      SOC_SAND_IN  ARAD_IP_TCAM_ENTRY_TYPE   tcam_key_type,
      SOC_SAND_OUT ARAD_PP_IP_TCAM_ENTRY_KEY *key
    )
{
  uint32
    long_stream[ARAD_PP_IP_TCAM_KEY_LEN_LONGS],
    tmp = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  sal_memset(key, 0x0, sizeof(*key));

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  if (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(tcam_key_type)) {
      uint32 
          res = SOC_SAND_OK,
          table_size_in_bytes,
          table_payload_in_bytes;

      res = arad_kbp_table_size_get(unit, ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(tcam_key_type), &table_size_in_bytes, &table_payload_in_bytes);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      key->type = tcam_key_type;
      sal_memcpy(key->key.elk_fwd.m_data, stream, table_size_in_bytes);
      sal_memcpy(key->key.elk_fwd.m_mask, &(stream[table_size_in_bytes]), table_size_in_bytes);

      ARAD_DO_NOTHING_AND_EXIT;
  }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
 
/*
 * COVERITY
 *
 * Static array "long_stream" is not overrun
 */
/* coverity[overrun-buffer-arg] */
  soc_sand_U8_to_U32(
    stream,
    ARAD_PP_IP_TCAM_KEY_LEN_BYTES,
    long_stream
  );

  soc_sand_bitstream_get_any_field(long_stream, 157, 3, &tmp);
  key->type = tmp;
  switch (key->type)
  {
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_MC:
    soc_sand_bitstream_get_any_field(long_stream, 0, 1, &tmp);
    key->key.ipv4_mc.inrif_valid = SOC_SAND_NUM2BOOL(tmp);

    soc_sand_bitstream_get_any_field(long_stream, 1, 12, &tmp);
    key->key.ipv4_mc.inrif = tmp;
 
    soc_sand_bitstream_get_any_field(long_stream, 13, 32, &key->key.ipv4_mc.source.ip_address);

    soc_sand_bitstream_get_any_field(long_stream, 45, 8, &tmp);
    key->key.ipv4_mc.source.prefix_len = (uint8) tmp;

    soc_sand_bitstream_get_any_field(long_stream, 53, 28, &key->key.ipv4_mc.group);
    if(key->key.ipv4_mc.group != 0)
    {
      key->key.ipv4_mc.group |= SOC_SAND_PP_IPV4_MC_ADDR_PREFIX;
    }
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_MC:
    soc_sand_bitstream_get_any_field(long_stream, 1, 12, &tmp);
    key->key.ipv6_mc.inrif.mask = tmp;

    soc_sand_bitstream_get_any_field(long_stream, 13, 12, &tmp);
    key->key.ipv6_mc.inrif.val = tmp;

    soc_sand_bitstream_get_any_field(long_stream, 25, ARAD_PP_IPV6_MC_GROUP_NOF_BITS, key->key.ipv6_mc.group.address);
    key->key.ipv6_mc.group.address[3] |= 0xff000000;
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_UC:
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
    soc_sand_bitstream_get_any_field(long_stream, 1, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), &key->vrf_ndx);
    
    if (key->type == ARAD_IP_TCAM_ENTRY_TYPE_IPV6_UC) {
        soc_sand_bitstream_get_any_field(long_stream, 13, SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS, key->key.ipv6_uc.subnet.ipv6_address.address);
    } else { /* ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN */
        soc_sand_bitstream_get_any_field(long_stream, 13, SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS, key->key.ipv6_vpn.subnet.ipv6_address.address);
    }

    soc_sand_bitstream_get_any_field(long_stream, 141, SOC_SAND_NOF_BITS_IN_CHAR, &tmp);
    key->key.ipv6_uc.subnet.prefix_len = (uint8) tmp;
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
  	soc_sand_bitstream_get_any_field(long_stream, 0, 128, key->key.ipv6_static_src_bind.sip6.address);
    if (SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit)) {
	    soc_sand_bitstream_get_any_field(long_stream, 128, SOC_SAND_NOF_BITS_IN_CHAR, &tmp);
        key->key.ipv6_static_src_bind.prefix_len = (uint8) tmp;
	    soc_sand_bitstream_get_any_field(long_stream, 136, 16, &(key->key.ipv6_static_src_bind.lif_ndx));
    } else {
        /*Lif index, 16 valid bits*/
	    soc_sand_bitstream_get_any_field(long_stream, 128, 16, &(key->key.ipv6_static_src_bind.lif_ndx));
    }
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_TRILL_MC:
  	soc_sand_bitstream_get_any_field(long_stream, 0, 16, &(key->key.trill_mc.tree_nick));
    soc_sand_bitstream_get_any_field(long_stream, 16, 1, &tmp);
    key->key.trill_mc.esadi = (uint8) tmp;
    soc_sand_bitstream_get_any_field(long_stream, 17, 12, &(key->key.trill_mc.outer_vid));
    break;
  default:
    break;
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_stream_to_key()", 0, 0);
}

STATIC
  void
    arad_pp_ip_tcam_entry_build(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY *key,
      SOC_SAND_OUT ARAD_TCAM_ENTRY           *entry
  )
{
  uint32
    tmp,
    mask,
    ipv6_mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S];

  ARAD_TCAM_ENTRY_clear(entry);
  /* Set the valid bit */
  entry->valid = TRUE;

  switch(key->type)
  {
  
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_UC_0:
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_UC_1:
      mask = (key->key.ipv4_mc.source.prefix_len > 0) ?
               SOC_SAND_BITS_MASK(31, 32 - key->key.ipv4_mc.source.prefix_len) : 0;
    soc_sand_bitstream_set_any_field(&key->key.ipv4_mc.source.ip_address, 0, 32, entry->value);
    soc_sand_bitstream_set_any_field(&mask,                              0, 32, entry->mask);

    soc_sand_bitstream_set_any_field(&key->key.ipv4_mc.inrif, 32, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->value);
    soc_sand_bitstream_set_any_field(&mask,                   32, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->mask);
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_MC:
    mask = SOC_SAND_BITS_MASK(27, 0);
    if(key->key.ipv4_mc.group == 0)  /* enable group masking */
    {
      mask = 0;
    }
    soc_sand_bitstream_set_any_field(&key->key.ipv4_mc.group, 0, 28, entry->value);
    soc_sand_bitstream_set_any_field(&mask,                   0, 28, entry->mask);

    mask = (key->key.ipv4_mc.source.prefix_len > 0) ?
             SOC_SAND_BITS_MASK(31, 32 - key->key.ipv4_mc.source.prefix_len) : 0;
    soc_sand_bitstream_set_any_field(&key->key.ipv4_mc.source.ip_address, 28, 32, entry->value);
    soc_sand_bitstream_set_any_field(&mask,                               28, 32, entry->mask);

    mask = (key->key.ipv4_mc.inrif_valid) ? SOC_SAND_BITS_MASK(SOC_DPP_DEFS_GET(unit, in_rif_nof_bits) - 1, 0) : 0;
    soc_sand_bitstream_set_any_field(&key->key.ipv4_mc.inrif, 60, SOC_DPP_DEFS_GET(unit, in_rif_nof_bits), entry->value);
    soc_sand_bitstream_set_any_field(&mask,                   60, SOC_DPP_DEFS_GET(unit, in_rif_nof_bits), entry->mask);
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_UC:
    arad_pp_ipv6_prefix_to_mask(key->key.ipv6_uc.subnet.prefix_len, ipv6_mask);
    if (SOC_IS_JERICHO(unit)) {
        
        soc_sand_bitstream_set_any_field(key->key.ipv6_uc.subnet.ipv6_address.address, 0, 64, entry->value);
        soc_sand_bitstream_set_any_field(ipv6_mask,                                    0, 64, entry->mask);
        soc_sand_bitstream_set_any_field(&key->key.ipv6_uc.subnet.ipv6_address.address[2], 80, 64, entry->value);
        soc_sand_bitstream_set_any_field(&ipv6_mask[2],                                    80, 64, entry->mask);
        mask = SOC_SAND_BITS_MASK(SOC_DPP_DEFS_GET(unit, vrf_nof_bits) - 1, 0);
        soc_sand_bitstream_set_any_field(&key->vrf_ndx, 144, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->value);
        soc_sand_bitstream_set_any_field(&mask,         144, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->mask);
    } else {
        soc_sand_bitstream_set_any_field(key->key.ipv6_uc.subnet.ipv6_address.address, 0, 128, entry->value);
        soc_sand_bitstream_set_any_field(ipv6_mask,                                    0, 128, entry->mask);
        mask = SOC_SAND_BITS_MASK(SOC_DPP_DEFS_GET(unit, vrf_nof_bits) - 1, 0);
        soc_sand_bitstream_set_any_field(&key->vrf_ndx, 128, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->value);
        soc_sand_bitstream_set_any_field(&mask,         128, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->mask);
    }

    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
    arad_pp_ipv6_prefix_to_mask(key->key.ipv6_vpn.subnet.prefix_len, ipv6_mask);
    if (SOC_IS_JERICHO(unit)) {
        
        soc_sand_bitstream_set_any_field(key->key.ipv6_vpn.subnet.ipv6_address.address, 0, 64, entry->value);
        soc_sand_bitstream_set_any_field(ipv6_mask,                                    0, 64, entry->mask);
        soc_sand_bitstream_set_any_field(&key->key.ipv6_vpn.subnet.ipv6_address.address[2], 80, 64, entry->value);
        soc_sand_bitstream_set_any_field(&ipv6_mask[2],                                    80, 64, entry->mask);
        mask = SOC_SAND_BITS_MASK(SOC_DPP_DEFS_GET(unit, vrf_nof_bits) - 1, 0);
        soc_sand_bitstream_set_any_field(&key->vrf_ndx, 144, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->value);
        soc_sand_bitstream_set_any_field(&mask,         144, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->mask);
    } else {
        soc_sand_bitstream_set_any_field(key->key.ipv6_vpn.subnet.ipv6_address.address, 0, 128, entry->value);
        soc_sand_bitstream_set_any_field(ipv6_mask,                                     0, 128, entry->mask);
        mask = SOC_SAND_BITS_MASK(SOC_DPP_DEFS_GET(unit, vrf_nof_bits) - 1, 0);
        soc_sand_bitstream_set_any_field(&key->vrf_ndx, 128, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->value);
        soc_sand_bitstream_set_any_field(&mask,         128, SOC_DPP_DEFS_GET(unit, vrf_nof_bits), entry->mask);
    }
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_MC:
      if (SOC_IS_JERICHO(unit)) {
          
          soc_sand_bitstream_set_any_field(key->key.ipv6_mc.group.address, 0, 64, entry->value);
          soc_sand_bitstream_set_bit_range(entry->mask, 0, 64);
          soc_sand_bitstream_set_any_field(&key->key.ipv6_mc.group.address[2], 80, 120-64, entry->value);
          soc_sand_bitstream_set_bit_range(entry->mask, 80, 80+120-64);

          soc_sand_bitstream_set_any_field(&key->key.ipv6_mc.inrif.val,  120+16, SOC_DPP_DEFS_GET(unit, in_rif_nof_bits), entry->value);
          soc_sand_bitstream_set_any_field(&key->key.ipv6_mc.inrif.mask, 120+16, SOC_DPP_DEFS_GET(unit, in_rif_nof_bits), entry->mask);
      } else {
        soc_sand_bitstream_set_any_field(key->key.ipv6_mc.group.address, 0, 120, entry->value);
        soc_sand_bitstream_set_bit_range(entry->mask, 0, 119);

        soc_sand_bitstream_set_any_field(&key->key.ipv6_mc.inrif.val,  120, SOC_DPP_DEFS_GET(unit, in_rif_nof_bits), entry->value);
        soc_sand_bitstream_set_any_field(&key->key.ipv6_mc.inrif.mask, 120, SOC_DPP_DEFS_GET(unit, in_rif_nof_bits), entry->mask);
      }
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
      
    if (SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit)){
      arad_pp_ipv6_prefix_to_mask(key->key.ipv6_static_src_bind.prefix_len, ipv6_mask);
      soc_sand_bitstream_set_any_field(key->key.ipv6_static_src_bind.sip6.address, 0, 128, entry->value);
      soc_sand_bitstream_set_any_field(ipv6_mask,                                  0, 128, entry->mask);
      mask = SOC_SAND_BITS_MASK(15, 0);
      soc_sand_bitstream_set_any_field(&key->key.ipv6_static_src_bind.lif_ndx, 128, 16, entry->value);
      soc_sand_bitstream_set_any_field(&mask,                                  128, 16, entry->mask);
    } else {
      soc_sand_bitstream_set_any_field(key->key.ipv6_static_src_bind.sip6.address, 0, 128, entry->value);
      /*Lif index, 16 valid bits*/
      soc_sand_bitstream_set_any_field(&(key->key.ipv6_static_src_bind.lif_ndx), 128, 16, entry->value);
      /* Mask all , take into account all entry */
      soc_sand_bitstream_set_bit_range(entry->mask, 0, 143); 
    }
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_TRILL_MC:
     soc_sand_bitstream_set_any_field(&(key->key.trill_mc.tree_nick), 0, 16, entry->value);
	 tmp = key->key.trill_mc.esadi;
     soc_sand_bitstream_set_any_field(&tmp, 16, 1, entry->value);
     soc_sand_bitstream_set_any_field(&(key->key.trill_mc.outer_vid), 17, 12, entry->value);
     /* Mask all , take into account all entry */
     soc_sand_bitstream_set_bit_range(entry->mask, 0, 28);
    break;
  default:
    break;
  }
}

STATIC
  uint32
    arad_pp_frwrd_ip_tcam_ip_db_id_get(
      SOC_SAND_IN ARAD_IP_TCAM_ENTRY_TYPE entry_type
    )
{
  uint32
    tcam_db_id;

  switch (entry_type)
  {
  case ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_0:
    tcam_db_id = ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_0;
    break;

  case ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_1:
    tcam_db_id = ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_1;
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV4_MC:
    tcam_db_id = ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_MC;
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_UC:
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
    tcam_db_id = ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_UC;
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_MC:
    tcam_db_id = ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_MC;
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
  	tcam_db_id = ARAD_PP_SRC_BIND_TCAM_ID_IPV6_STATIC;
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_TRILL_MC:
    tcam_db_id = ARAD_PP_FRWRD_TRILL_TCAM_ID_MC;
    break;
  default:
    tcam_db_id = ARAD_TCAM_MAX_NOF_LISTS;
    break;
  }

  return tcam_db_id;
}


STATIC uint32
  arad_pp_ip_tcam_hash_tbl_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  ARAD_IP_TCAM_ENTRY_TYPE tcam_key_type,
    SOC_SAND_OUT SOC_SAND_HASH_TABLE_INFO **hash_tbl
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(hash_tbl);

  *hash_tbl = arad_sw_db_frwrd_ip_route_key_to_entry_id_get(unit);
  
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_hash_tbl_get()", 0, 0);
}

uint32
  arad_pp_ip_tcam_entry_hw_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *route_key,
    SOC_SAND_IN  uint8                       is_for_update,
    SOC_SAND_IN  ARAD_TCAM_ACTION           *action,
    SOC_SAND_IN  uint32                     data_indx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  )
{
    uint32
      res = SOC_SAND_OK,
      tcam_db_id,
      priority;
    ARAD_TCAM_ENTRY
      entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(success);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  if (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(route_key->type)) {
      uint32 
          table_size_in_bytes,
          table_payload_in_bytes;
      uint8 
          assoData[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES],
          m_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES],
          m_mask[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];

      sal_memset(m_data, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);
      sal_memset(m_mask, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);
      sal_memset(assoData, 0x0, sizeof(uint8) * SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES);

      res = arad_kbp_table_size_get(unit, ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(route_key->type), &table_size_in_bytes, &table_payload_in_bytes);
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
      sal_memcpy(m_data, route_key->key.elk_fwd.m_data, table_size_in_bytes);
      sal_memcpy(m_mask, route_key->key.elk_fwd.m_mask, table_size_in_bytes);
      sal_memcpy(assoData, action->elk_ad_value, table_payload_in_bytes);

      res = arad_pp_frwrd_ip_tcam_kbp_route_add(
                unit,
                ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(route_key->type),
                data_indx,
                is_for_update,
                route_key->key.elk_fwd.priority,
                m_data,
                m_mask,
                assoData,
                success
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else 
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
  {
      tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(route_key->type);
      priority = arad_pp_ip_tcam_route_to_prio(unit, route_key);
      arad_pp_ip_tcam_entry_build(unit, route_key, &entry);
      entry.is_for_update = is_for_update;
      res = arad_tcam_managed_db_entry_add_unsafe(
              unit,
              tcam_db_id,
              data_indx,
              FALSE,
              priority,
              &entry,
              action,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_entry_hw_add()", 0, 0);
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
  arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(
     SOC_SAND_IN  uint8     *mask,
     SOC_SAND_IN  uint32    table_size_in_bytes,
     SOC_SAND_OUT uint32    *prefix_len
     )
{
  uint32 length;
  uint8 bytes_idx, bits_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(mask);
  SOC_SAND_CHECK_NULL_INPUT(prefix_len);

  /* 
   * Getting the prefix length by scanning the mask from lsb to msb,
   * searching for the first '0'. 
   */ 
  length = table_size_in_bytes * SOC_SAND_NOF_BITS_IN_BYTE;

  for (bytes_idx = (table_size_in_bytes -1); bytes_idx > 0; bytes_idx--) {
       if (mask[bytes_idx] == SOC_SAND_U8_MAX) {
           length -= SOC_SAND_NOF_BITS_IN_BYTE;
       }
       else {
           break;
       }
  }
  for (bits_idx = 0; bits_idx < SOC_SAND_NOF_BITS_IN_BYTE; bits_idx++)
  {
      if ((mask[bytes_idx] >> bits_idx) == 0)
      {
          length -= bits_idx;
          break;
      }
  }

  *prefix_len = length;

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_lpm_prefix_len_get()", 0, 0);
}

uint32
  arad_pp_frwrd_ip_tcam_entry_hw_add(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *route_key,
    SOC_SAND_IN  ARAD_TCAM_ACTION           *action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
    )
{
    uint32
      res = SOC_SAND_OK,
      prefix_len = 0;
  uint8 
      assoData[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES],
      m_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES],
      m_mask[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];
  uint32 
      table_size_in_bytes,
      table_payload_in_bytes;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(success);

  sal_memset(m_data, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);
  sal_memset(m_mask, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);
  sal_memset(assoData, 0x0, sizeof(uint8) * SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES);

  res = arad_kbp_table_size_get(unit, ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(route_key->type), &table_size_in_bytes, &table_payload_in_bytes);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  sal_memcpy(m_data, route_key->key.elk_fwd.m_data, table_size_in_bytes);
  sal_memcpy(m_mask, route_key->key.elk_fwd.m_mask, table_size_in_bytes);
  sal_memcpy(assoData, action->elk_ad_value, table_payload_in_bytes);

  res = arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(m_mask, table_size_in_bytes, &prefix_len);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  res = arad_pp_frwrd_ip_tcam_kbp_lpm_route_add(
            unit,
            ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(route_key->type),
            route_key->key.elk_fwd.priority, /* all cases == 0 only when fp_database_info.flags == SOC_PPC_FP_DATABASE_INFO_FLAGS_EXTENDED_FWRD_DB it means the entry index */
            prefix_len,
            m_data,
            assoData,
            success
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 00, exit);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_entry_hw_add()", 0, 0);
}

uint32
  arad_pp_frwrd_ip_tcam_entry_hw_remove(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *route_key
    )
{
  uint32
      res = SOC_SAND_OK,
      prefix_len = 0;
  uint32 
      table_size_in_bytes,
      table_payload_in_bytes;
  uint8 
      m_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES],
      m_mask[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  sal_memset(m_data, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);
  sal_memset(m_mask, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);

  res = arad_kbp_table_size_get(unit, ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(route_key->type), &table_size_in_bytes, &table_payload_in_bytes);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  sal_memcpy(m_data, route_key->key.elk_fwd.m_data, table_size_in_bytes);
  sal_memcpy(m_mask, route_key->key.elk_fwd.m_mask, table_size_in_bytes);

  res = arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(m_mask, table_size_in_bytes, &prefix_len);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  res = arad_pp_frwrd_ip_tcam_kbp_lpm_route_remove(
            unit,
            ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(route_key->type),
            prefix_len,
            m_data
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_entry_hw_add()", 0, 0);
}

uint32
  arad_pp_frwrd_ip_tcam_entry_hw_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *route_key,
    SOC_SAND_OUT ARAD_TCAM_ACTION           *action,
    SOC_SAND_OUT uint8                      *found
    )
{
  uint32
      res = SOC_SAND_OK,
      prefix_len = 0;
  uint32 
      table_size_in_bytes,
      table_payload_in_bytes;
  uint8 
      m_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES],
      m_mask[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  sal_memset(m_data, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);
  sal_memset(m_mask, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);

  res = arad_kbp_table_size_get(unit, ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(route_key->type), &table_size_in_bytes, &table_payload_in_bytes);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  sal_memcpy(m_data, route_key->key.elk_fwd.m_data, table_size_in_bytes);
  sal_memcpy(m_mask, route_key->key.elk_fwd.m_mask, table_size_in_bytes);

  res = arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(m_mask, table_size_in_bytes, &prefix_len);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  res = arad_pp_frwrd_ip_tcam_kbp_lpm_route_get(
            unit,
            ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(route_key->type),
            prefix_len,
            m_data,
            action->elk_ad_value,
            found
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_entry_hw_add()", 0, 0);
}
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

uint32
  arad_pp_ip_tcam_entry_hw_general_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     tcam_db_id,
    SOC_SAND_IN  ARAD_IP_TCAM_ENTRY_TYPE    tcam_key_type,
    SOC_SAND_IN  uint32                     data_indx
  )
{
    uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  if (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(tcam_key_type)) {
      res = arad_pp_frwrd_ip_tcam_kbp_route_remove(
                unit,
                ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(tcam_key_type),
                data_indx
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  }
  else 
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
  {
      res = arad_tcam_managed_db_entry_remove_unsafe(
              unit,
              tcam_db_id,
              data_indx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_entry_hw_general_remove()", 0, 0);
}

STATIC uint32
  arad_pp_ip_tcam_entry_hw_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_IP_TCAM_ENTRY_TYPE    tcam_key_type,
    SOC_SAND_IN  uint32                     data_indx
  )
{
    uint32
      res = SOC_SAND_OK,
      tcam_db_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(tcam_key_type);
  res = arad_pp_ip_tcam_entry_hw_general_remove(
          unit,
          tcam_db_id,
          tcam_key_type,
          data_indx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_entry_hw_remove()", 0, 0);
}

uint32
  arad_pp_ip_tcam_entry_hw_general_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     tcam_db_id,
    SOC_SAND_IN  ARAD_IP_TCAM_ENTRY_TYPE    tcam_key_type,
    SOC_SAND_IN  uint32                     entry_id,
    SOC_SAND_IN  uint8                      hit_bit_clear,
    SOC_SAND_OUT ARAD_TCAM_ENTRY            *entry,
    SOC_SAND_OUT uint32                     *priority,
    SOC_SAND_OUT ARAD_TCAM_ACTION           *action,
    SOC_SAND_OUT uint8                      *found,
    SOC_SAND_OUT uint8                      *hit_bit
  )
{
    uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(found);
  SOC_SAND_CHECK_NULL_INPUT(hit_bit);
  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(priority);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  if (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(tcam_key_type)) {
      uint8 m_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];
      uint8 m_mask[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];
      int nof_valid_bytes = 0;
      *hit_bit = 0; /* Featrue not supported in KBP */
      
      res = arad_pp_frwrd_ip_tcam_kbp_route_get(
                unit,
                ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(tcam_key_type),
                entry_id,
                m_data,
                m_mask,
                action->elk_ad_value,
                priority,
                found,
                &nof_valid_bytes
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

      ARAD_TCAM_ENTRY_clear(entry);

      /* Convert the entry key according to KBP constraints */
      res = arad_pp_frwrd_ip_tcam_route_from_kbp_key_decode(unit, m_data, m_mask, nof_valid_bytes, entry->value, entry->mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      /* Convert the entry payload according to KBP constraints */
      res = arad_pp_frwrd_ip_tcam_route_from_kbp_payload_buffer_decode(
               unit,
               ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(tcam_key_type),
               action->elk_ad_value,
               action->value
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);      
  }
  else 
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
  {
      res = arad_tcam_db_entry_get_unsafe(
              unit,
              tcam_db_id,
              entry_id,
              hit_bit_clear, 
              priority,
              entry,
              action,
              found,
              hit_bit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_entry_hw_general_get()", 0, 0);
}


STATIC
  uint32
    arad_pp_ip_tcam_entry_hw_get(
        SOC_SAND_IN  int                     unit,
        SOC_SAND_IN  ARAD_IP_TCAM_ENTRY_TYPE    tcam_key_type,
        SOC_SAND_IN  uint32                     entry_id,
        SOC_SAND_IN  uint8                     hit_bit_clear,
        SOC_SAND_OUT ARAD_TCAM_ACTION    *action,
        SOC_SAND_OUT uint8               *found,
        SOC_SAND_OUT uint8               *hit_bit
    )
{
    uint32
      res = SOC_SAND_OK,
      tcam_db_id,
      priority;
    ARAD_TCAM_ENTRY
      entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(found);
  SOC_SAND_CHECK_NULL_INPUT(hit_bit);

  tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(tcam_key_type);
  res = arad_pp_ip_tcam_entry_hw_general_get(
            unit,
            tcam_db_id,
            tcam_key_type,
            entry_id,
            hit_bit_clear,
            &entry,
            &priority,
            action,
            found,
            hit_bit
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_entry_hw_get()", 0, 0);
}

uint32
  arad_pp_frwrd_ip_tcam_route_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  ARAD_TCAM_ACTION                                *action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint8
    stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES];
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  uint8
    found,
    entry_added = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_TCAM_ROUTE_ADD_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  *success = SOC_SAND_SUCCESS;

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  if (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_FORWARDING(route_key->type)) {
      res = arad_pp_frwrd_ip_tcam_entry_hw_add(
              unit,
              route_key,
              action,
              success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else 
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
  {
      /*
       *  Search for the route_key in the route_key -> entry_id hash table
       */
      res = arad_pp_ip_tcam_hash_tbl_get(unit, route_key->type, &hash_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

      res = arad_pp_ip_tcam_key_to_stream(unit, route_key, stream);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = soc_sand_hash_table_entry_lookup(
              hash_tbl,
              stream,
              &data_indx,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (!found)
      {
        /*
         *  Insert the rule to the route_key -> entry_id table
         */
        res = soc_sand_hash_table_entry_add(
                hash_tbl,
                stream,
                &data_indx,
                &entry_added
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
      else
      {
        entry_added = TRUE;
      }

      if (entry_added)
      {

        /*
         *  Insert the new rule to the TCAM
         */
        res = arad_pp_ip_tcam_entry_hw_add(
            unit,
                route_key,
                found,
                action,
                data_indx,
                success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        /* 
         * Remove the entry from the hash list only if it was added just now: 
         * the assumption is that the TCAM insertion does not remove 
         * an existing entry in case of failure.
         */
        if ((*success != SOC_SAND_SUCCESS) && (!found))
        {
          res = soc_sand_hash_table_entry_remove_by_index(
                  hash_tbl,
                  data_indx
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        }
      }
      else
      {
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_route_add_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_ip_tcam_route_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY *route_key,
    SOC_SAND_IN  uint8               exact_match,
    SOC_SAND_OUT ARAD_TCAM_ACTION    *action,
    SOC_SAND_OUT uint8               *found,
    SOC_SAND_OUT uint8               *hit_bit
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint8
      stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES];
  uint32
    entry_id;
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  uint32
    tcam_db_id;
  ARAD_TCAM_ENTRY
    entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  if ((exact_match & ARAD_PP_FRWRD_IP_EXACT_MATCH) 
      /* Always Exact match for KBP entry get */
      || (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(route_key->type)))
  {
    /*
     *  Search for the route_key in the route_key -> entry_id hash table
     */
      res = arad_pp_ip_tcam_hash_tbl_get(unit, route_key->type, &hash_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    res = arad_pp_ip_tcam_key_to_stream(unit, route_key, stream);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_sand_hash_table_entry_lookup(
            hash_tbl,
            stream,
            &data_indx,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    entry_id = data_indx;
  }
  else
  {
      /*
       *  Determine the database ID
       */
      tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(
                     route_key->type
                   );

    /*
     *  Search the TCAM for the first match
     */
    arad_pp_ip_tcam_entry_build(
      unit,
      route_key,
      &entry
    );

    res = arad_tcam_db_entry_search_unsafe(
            unit,
            tcam_db_id,
            &entry,
            &entry_id,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (*found)
  {
      res = arad_pp_ip_tcam_entry_hw_get(
                unit,
                route_key->type,
                entry_id,
                exact_match & ARAD_PP_FRWRD_IP_CLEAR_ON_GET, /* hit_bit_clear */
                action,
                found,
                hit_bit
              );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_route_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ip_tcam_route_get_block_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_INOUT  ARAD_PP_IP_ROUTING_TABLE_RANGE           *block_range_key,
    SOC_SAND_OUT  ARAD_PP_IP_TCAM_ENTRY_KEY                  *route_key,
    SOC_SAND_OUT ARAD_TCAM_ACTION                            *action,
    SOC_SAND_OUT uint32                                      *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    arr_indx,
    data_indx;
  uint32
    nof_scanned,
    nof_retrieved;
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  SOC_SAND_HASH_TABLE_ITER
    iter;
  uint8
    stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES];
  ARAD_PP_IP_TCAM_ENTRY_KEY
    key;
  uint8
      hit_bit,            
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_TCAM_ROUTE_GET_BLOCK_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  if ((block_range_key->entries_to_scan == 0) || (block_range_key->entries_to_act == 0)) {
      ARAD_DO_NOTHING_AND_EXIT;
  }

  /* Assumption that there is at least one route */
  res = arad_pp_ip_tcam_hash_tbl_get(unit, route_key[0].type, &hash_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  iter = block_range_key->start.payload.arr[0];
  nof_scanned   = 0;
  nof_retrieved = 0;
  arr_indx      = 0;
  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)
          && (nof_scanned < block_range_key->entries_to_scan)
          && (nof_retrieved < block_range_key->entries_to_act))
  {
      /* the iter is saved for next-block run */
      res = soc_sand_hash_table_get_next(
              hash_tbl,
              &iter,
              stream,
              &data_indx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if(SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)) {
          break;
      }

    ++nof_scanned;
    res = arad_pp_ip_tcam_stream_to_key(
            unit,
            stream,
            route_key[0].type, /* for KBP decryption only */
            &key
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    if (key.type == route_key[arr_indx].type)
    {
      /* For IPv6 Unicast, verify VRFs are equal */
      if (key.type != ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN || key.vrf_ndx == route_key[arr_indx].vrf_ndx)
      {
        route_key[arr_indx] = key;
        res = arad_pp_ip_tcam_entry_hw_get(
                  unit,
                  key.type,
                  data_indx,
                  TRUE, /* hit_bit_clear */
                  &action[arr_indx],
                  &found,
                  &hit_bit
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        
        /*
         *  If found == FALSE we should indicate an internal error
         */

        ++nof_retrieved;
        ++arr_indx;
      }
    }
  }

  *nof_entries = arr_indx;
  
  block_range_key->entries_to_act = nof_retrieved;
  block_range_key->entries_to_scan = nof_scanned;

  if(SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
  {
    block_range_key->start.payload.arr[0] = SOC_SAND_UINT_MAX;
    block_range_key->start.payload.arr[1] = SOC_SAND_UINT_MAX;
  }
  else
  {
    block_range_key->start.payload.arr[0] = iter;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_get_block_unsafe()",0,0);
}

uint32
  arad_pp_frwrd_ip_tcam_route_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY                       *route_key
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint8
    found;
  uint8
	  stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES];
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  if (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_FORWARDING(route_key->type)) {
      res = arad_pp_frwrd_ip_tcam_entry_hw_remove(unit, route_key);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  else 
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
  {
      /*
       *  Search for the route_key in the route_key -> entry_id hash table
       */
  res = arad_pp_ip_tcam_hash_tbl_get(unit, route_key->type, &hash_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_ip_tcam_key_to_stream(unit, route_key, stream);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = soc_sand_hash_table_entry_lookup(
              hash_tbl,
              stream,
              &data_indx,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (!found)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IP_TCAM_ENTRY_DOESNT_EXIST_ERR, 30, exit);
      }

      res = soc_sand_hash_table_entry_remove_by_index(
              hash_tbl,
              data_indx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);



      res = arad_pp_ip_tcam_entry_hw_remove(unit, route_key->type, data_indx);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_route_remove_unsafe()", 0, 0);
}


 
uint32
  arad_pp_frwrd_ip_tcam_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  uint8                                    clear_all_vrf
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  SOC_SAND_HASH_TABLE_ITER
    iter;
  uint8
    stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES];
  ARAD_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ip_tcam_hash_tbl_get(unit, route_key->type, &hash_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);
  res = soc_sand_hash_table_get_next(
          hash_tbl,
          &iter,
          stream,
          &data_indx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
  {
      res = arad_pp_ip_tcam_stream_to_key(
              unit,
              stream,
              route_key->type, /* for KBP decryption only */
              &key
           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    if (key.type == route_key->type)
    {
      if (route_key->type != ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN
           || clear_all_vrf || key.vrf_ndx == route_key->vrf_ndx)
      {
        res = arad_pp_frwrd_ip_tcam_route_remove_unsafe(
                unit,
                &key
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
    }

    res = soc_sand_hash_table_get_next(
            hash_tbl,
            &iter,
            stream,
            &data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_routing_table_clear_unsafe()", 0, 0);
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
/* 
 * Conversion functions from application-dependent 
 * structure (already transformed in a LEM access key) 
 * to the regular TCAM entry handling functions: 
 * add, remove, get and get-block 
 *  
 * In ARAD_PP_LEM_ACCESS_KEY, only nof_params and param[] 
 * are used. 
 */
static 
    ARAD_PP_FRWRD_IP_TCAM_KBP_TABLE_ATTRIBUTES
        Arad_pp_frwrd_ip_tcam_kbp_table_attributes[ARAD_KBP_FRWRD_IP_NOF_TABLES] = {
            /* LSB-#bits-param0,   param1,       param2,      param3,        param4,     logical_entry_size_in_bytes */
            {       {{0, 32},     {32,12},       {0, 0},       {0, 0},       {0, 0},},        6}, /* IPV4_UC_RPF_0 */
            {       {{0, 32},     {32,12},       {0, 0},       {0, 0},       {0, 0},},        6}, /* IPV4_UC_RPF_1 */
            {       {{0, 32},     {32,24},      {64,12},       {0, 0},       {0, 0},},        10}, /* IPV4_MC */
            {       {{0,128},    {128,12},       {0, 0},       {0, 0},       {0, 0},},        18}, /* IPV6_UC_RPF_0 */
            {       {{0,128},    {128,12},       {0, 0},       {0, 0},       {0, 0},},        18}, /* IPV6_UC_RPF_1 */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
            {       {{0,128},    {128,120},    {248,12},       {0, 0},       {0, 0},},        33}, /* IPV6_MC */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
            {       {{0,120},    {248,12},       {0, 0},       {0, 0},       {0, 0},},        17}, /* IPV6_MC */
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
            {       {{0,20},      {20, 3},      {23, 8},      {31,12},       {0, 0},},        6}, /* LSR */
            {       {{0,16},       {0, 0},       {0, 0},       {0, 0},       {0, 0},},        2}, /* TRILL_UC */
            {       {{0,16},      {16,15},      {31, 1},       {0, 0},       {0, 0},},        4}, /* TRILL_MC */
            {       {{0,16},      {16,15},      {31, 1},       {0, 0},       {0, 0},},        4}, /* DUMMY 0 */
            {       {{0,16},      {16,15},      {31, 1},       {0, 0},       {0, 0},},        4}, /* DUMMY 0 */
            {       {{0,16},      {16,15},      {31, 1},       {0, 0},       {0, 0},},        4}, /* DUMMY 0 */
            {       {{0,16},      {16,15},      {31, 1},       {0, 0},       {0, 0},},        4}, /* DUMMY 0 */
            {       {{0,32},      {32,12},      {44, 24},     {0, 0},        {0, 0},},        9}, /* LSR_IP_SHARED */
            {       {{0,32},      {32,12},      {0, 0},       {0, 0},        {0, 0},},        6}, /* LSR_IP_SHARED_FOR_IP */
            {       {{0,24},      {0,0},        {0, 0},       {0, 0},        {0, 0},},        3}, /* LSR_IP_SHARED_FOR_LSR */
        };




uint32
  arad_pp_frwrd_ip_tcam_route_from_kbp_key_decode(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint8                       *data,
    SOC_SAND_IN  uint8                       *mask,
    SOC_SAND_IN  int                         valid_bytes,
    SOC_SAND_OUT uint32                      *route_key_data,
    SOC_SAND_OUT uint32                      *route_key_mask
  )
{
    uint32
        byte_ndx,
        byte_array_idx,
        fld_val;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(data);
    SOC_SAND_CHECK_NULL_INPUT(mask);

    /*
     * Transform according to KBP expectations: 
     *  - Array indexation from max size-1 (e.g. 9 when the database size is 10 Bytes) to 0
     */

    for (byte_ndx = 0; byte_ndx < valid_bytes; ++byte_ndx)
    {
        byte_array_idx = valid_bytes - byte_ndx - 1;
        fld_val = data[byte_array_idx];
        SHR_BITCOPY_RANGE(route_key_data, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), &fld_val, 0, SOC_SAND_NOF_BITS_IN_BYTE);
        fld_val = mask[byte_array_idx];
        fld_val = (~fld_val); /* Inverse of PPD convention here for mask: 0 - care, 1 - don't care */
        SHR_BITCOPY_RANGE(route_key_mask, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), &fld_val, 0, SOC_SAND_NOF_BITS_IN_BYTE);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_from_kbp_key_decode()",0,0);
}

/* Build the mask according to the properties of data */
uint32
  arad_pp_frwrd_ip_tcam_route_kbp_route_key_mask_build(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_data,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY     *route_key_mask
  )
{
    uint32
        param_ndx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_KEY_clear(route_key_mask);

    route_key_mask->nof_params = route_key_data->nof_params;
    for (param_ndx = 0; param_ndx < route_key_data->nof_params; ++param_ndx)
    {
       route_key_mask->param[param_ndx].nof_bits = route_key_data->param[param_ndx].nof_bits;
       /* Set all the bits to 1, all is important */
       SHR_BITSET_RANGE(route_key_mask->param[param_ndx].value, 0 /* LSB */, route_key_mask->param[param_ndx].nof_bits);
    }
    
    ARAD_DO_NOTHING_AND_EXIT;
        
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_kbp_route_key_mask_build()",0,0);
}

/* Build/parse the LEM Key and a value */
uint32
  arad_pp_frwrd_ip_tcam_lem_key_encode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  uint32                     param_ndx,
    SOC_SAND_IN  uint32                     fld_val, /* only for values with less than 32 bits */
    SOC_SAND_INOUT ARAD_PP_LEM_ACCESS_KEY     *route_key
  )
{
    uint32
        value32;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(route_key);

    route_key->param[param_ndx].nof_bits = Arad_pp_frwrd_ip_tcam_kbp_table_attributes[frwrd_table_id].lsb_nof_bits[param_ndx][1 /* nof-bits */];
    if (route_key->param[param_ndx].nof_bits <= 32) {
        /* Copy only what is necessary */
        value32 = fld_val;
        SHR_BITCOPY_RANGE(route_key->param[param_ndx].value, 0, &value32, 0, route_key->param[param_ndx].nof_bits);
    }

    if (route_key->param[param_ndx].nof_bits) {
        /* increase the nof-params only if valid parameter: its nof-bits is not null */
        route_key->nof_params++;
    }
    
    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_lem_key_encode()",0,0);
}



/* Build the buffer for data and mask - then encode according to KBP expectations */
STATIC
 uint32
  arad_pp_frwrd_ip_tcam_route_kbp_buffer_encode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_data,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_mask,
    SOC_SAND_OUT uint8                        *data,
    SOC_SAND_OUT uint8                        *mask
  )
{
    uint32
        res,
        param_ndx,
        buffer_lsb,
        buffer_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_LONGS],
        buffer_mask[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_LONGS];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(data);
    SOC_SAND_CHECK_NULL_INPUT(mask);

    sal_memset(data, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);
    sal_memset(mask, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);
    sal_memset(buffer_data, 0x0, sizeof(uint32) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_LONGS);
    sal_memset(buffer_mask, 0x0, sizeof(uint32) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_LONGS);

    /* Build the buffer */
    for (param_ndx = 0; param_ndx < route_key_data->nof_params; ++param_ndx)
    {
        /* Get the LSB of each parameter according to the static table per forwarding type */
        buffer_lsb = Arad_pp_frwrd_ip_tcam_kbp_table_attributes[frwrd_table_id].lsb_nof_bits[param_ndx][0/*lsb*/];
        SHR_BITCOPY_RANGE(buffer_data, buffer_lsb, route_key_data->param[param_ndx].value, 0 /* LSB */, route_key_data->param[param_ndx].nof_bits);
        SHR_BITCOPY_RANGE(buffer_mask, buffer_lsb, route_key_mask->param[param_ndx].value, 0 /* LSB */, route_key_data->param[param_ndx].nof_bits);
    }

    res = arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode(
            unit, 
            Arad_pp_frwrd_ip_tcam_kbp_table_attributes[frwrd_table_id].logical_entry_size_in_bytes,
            buffer_data,
            buffer_mask,
            data,
            mask
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_kbp_buffer_encode()",0,0);
}

uint32
  arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     logical_entry_size_in_bytes,
    SOC_SAND_IN  uint32                     *buffer_data,
    SOC_SAND_IN  uint32                     *buffer_mask,
    SOC_SAND_OUT uint8                      *data,
    SOC_SAND_OUT uint8                      *mask
  )
{
    uint32
        byte_ndx,
        byte_array_idx,
        fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(data);
    SOC_SAND_CHECK_NULL_INPUT(mask);
    SOC_SAND_CHECK_NULL_INPUT(buffer_data);
    SOC_SAND_CHECK_NULL_INPUT(buffer_mask);

    sal_memset(data, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);
    sal_memset(mask, 0x0, sizeof(uint8) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES);

    /*
     * Transform according to KBP expectations: 
     *  - In KBP mask: 0 - care, 1 - don't care
     *  - Array indexation from max size-1 (e.g. 9 when the database size is 10 Bytes) to 0
     */
    for (byte_ndx = 0; byte_ndx < ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES; ++byte_ndx)
    {
        mask[byte_ndx] = 0xFF;
    }

    for (byte_ndx = 0; byte_ndx < logical_entry_size_in_bytes; ++byte_ndx)
    {
        byte_array_idx = logical_entry_size_in_bytes - byte_ndx - 1;
        /* Build data */
        fld_val = 0;
        SHR_BITCOPY_RANGE(&fld_val, 0, buffer_data, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), SOC_SAND_NOF_BITS_IN_BYTE);
        data[byte_array_idx] = (uint8) (fld_val & 0xFF);
        /* Build mask */
        fld_val = 0;
        SHR_BITCOPY_RANGE(&fld_val, 0, buffer_mask, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), SOC_SAND_NOF_BITS_IN_BYTE);
        fld_val = (~fld_val); /* Inverse of PPD convention here for mask: 0 - care, 1 - don't care */
        mask[byte_array_idx] = (uint8) (fld_val & 0xFF);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode()",0,0);
}

/* Build the buffer for data and mask - then encode according to KBP expectations */
STATIC
 uint32
  arad_pp_frwrd_ip_tcam_route_kbp_buffer_decode(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID       frwrd_table_id,
    SOC_SAND_IN  uint8                          *data,
    SOC_SAND_IN  uint8                          *mask,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY        *route_key_data,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY        *route_key_mask
  )
{
    uint32
        param_ndx,
        byte_ndx,
        byte_array_idx,
        buffer_lsb,
        res,
        buffer_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_LONGS],
        buffer_mask[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_LONGS],
        fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(data);
    SOC_SAND_CHECK_NULL_INPUT(mask);

    ARAD_PP_LEM_ACCESS_KEY_clear(route_key_data);
    ARAD_PP_LEM_ACCESS_KEY_clear(route_key_mask);
    sal_memset(buffer_data, 0x0, sizeof(uint32) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_LONGS);
    sal_memset(buffer_mask, 0x0, sizeof(uint32) * ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_LONGS);

    /*
     * Transform according to KBP expectations: 
     *  - In KBP mask: 0 - care, 1 - don't care
     *  - Array indexation from max size-1 (e.g. 9 when the database size is 10 Bytes) to 0
     */
    for (byte_ndx = 0; byte_ndx < Arad_pp_frwrd_ip_tcam_kbp_table_attributes[frwrd_table_id].logical_entry_size_in_bytes; ++byte_ndx)
    {
        byte_array_idx = Arad_pp_frwrd_ip_tcam_kbp_table_attributes[frwrd_table_id].logical_entry_size_in_bytes - byte_ndx - 1;
        /* Build data */
        fld_val = data[byte_array_idx];
        SHR_BITCOPY_RANGE(buffer_data, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), &fld_val, 0, SOC_SAND_NOF_BITS_IN_BYTE);
        /* Build mask */
        fld_val = mask[byte_array_idx];
        fld_val = (~fld_val); /* Inverse of PPD convention here for mask: 0 - care, 1 - don't care */
        SHR_BITCOPY_RANGE(buffer_mask, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), &fld_val, 0, SOC_SAND_NOF_BITS_IN_BYTE);
    }

    /* Build the LEM params */
    for (param_ndx = 0; param_ndx < ARAD_PP_LEM_KEY_MAX_NOF_PARAMS; ++param_ndx)
    {
        res = arad_pp_frwrd_ip_tcam_lem_key_encode(
                unit,
                frwrd_table_id,
                param_ndx,
                0, /* do not try to set a value */
                route_key_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

        res = arad_pp_frwrd_ip_tcam_lem_key_encode(
                unit,
                frwrd_table_id,
                param_ndx,
                0, /* do not try to set a value */
                route_key_mask
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
    }

    /* Build the LEM params */
    for (param_ndx = 0; param_ndx < route_key_data->nof_params; ++param_ndx)
    {
        /* Get the LSB of each parameter according to the static table per forwarding type */
        buffer_lsb = Arad_pp_frwrd_ip_tcam_kbp_table_attributes[frwrd_table_id].lsb_nof_bits[param_ndx][0/*lsb*/];
        SHR_BITCOPY_RANGE(route_key_data->param[param_ndx].value, 0 /* LSB */, buffer_data, buffer_lsb, route_key_data->param[param_ndx].nof_bits);
        SHR_BITCOPY_RANGE(route_key_mask->param[param_ndx].value, 0 /* LSB */, buffer_mask, buffer_lsb, route_key_data->param[param_ndx].nof_bits);
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_kbp_buffer_decode()",0,0);
}


/* Build the buffer for payload - then encode according to KBP expectations */
uint32
  arad_pp_frwrd_ip_tcam_route_to_kbp_payload_buffer_encode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     table_payload_in_bytes,
    SOC_SAND_IN  uint32                     *asso_data_buffer,
    SOC_SAND_OUT uint8                      *asso_data
  )
{
    uint32
        byte_ndx,
        byte_array_idx,
        fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(asso_data_buffer);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    sal_memset(asso_data, 0x0, sizeof(uint8) * SOC_DPP_TCAM_ACTION_ELK_KBP_GET_LEN_BYTES(unit));

    /*
     * Transform according to KBP expectations: 
     *  - Array indexation from max size-1 (e.g. 3 when the payload size is 4 Bytes) to 0
     */
    for (byte_ndx = 0; byte_ndx < table_payload_in_bytes; ++byte_ndx)
    {
        byte_array_idx = table_payload_in_bytes - byte_ndx - 1;
        /* Build data */
        fld_val = 0;
        SHR_BITCOPY_RANGE(&fld_val, 0, asso_data_buffer, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), SOC_SAND_NOF_BITS_IN_BYTE);
        asso_data[byte_array_idx] = (uint8) (fld_val & 0xFF);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_to_kbp_payload_buffer_encode()",0,0);
}


/* Build the buffer for payload - then encode according to KBP expectations */
STATIC
 uint32
  arad_pp_frwrd_ip_tcam_route_kbp_payload_encode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD *payload,
    SOC_SAND_OUT uint8                      *asso_data
  )
{
    uint32
        res,
        asso_data_buffer[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_LONGS];
    uint32
      table_size_in_bytes,
        table_payload_in_bytes;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(payload);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    sal_memset(asso_data, 0x0, sizeof(uint8) * SOC_DPP_TCAM_ACTION_ELK_KBP_GET_LEN_BYTES(unit));
    sal_memset(asso_data_buffer, 0x0, sizeof(uint32) * SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_LONGS);

    /* Build the buffer */
    /* Encode structure into data buffer */
    res = arad_pp_lem_access_payload_build(
            unit,
            payload,
            asso_data_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /*
     * Transform according to KBP expectations   
     */
    res = arad_kbp_table_size_get(unit, frwrd_table_id, &table_size_in_bytes, &table_payload_in_bytes); 
    SOC_SAND_CHECK_FUNC_RESULT(res,  71, exit);
    res = arad_pp_frwrd_ip_tcam_route_to_kbp_payload_buffer_encode(
            unit,
            table_payload_in_bytes,
            asso_data_buffer,
            asso_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_kbp_payload_encode()",0,0);
}

/* Build the buffer for payload - then encode according to KBP expectations */
uint32
  arad_pp_frwrd_ip_tcam_route_from_kbp_payload_buffer_decode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  uint8                      *asso_data,
    SOC_SAND_OUT uint32                     *asso_data_buffer
  )
{
    uint32
        byte_ndx,
        byte_array_idx,
        payload_size_in_bytes,
        table_size_in_bytes,
        res,
        fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(asso_data_buffer);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    res = arad_kbp_table_size_get(unit, frwrd_table_id, &table_size_in_bytes, &payload_size_in_bytes); 
    SOC_SAND_CHECK_FUNC_RESULT(res,  71, exit);

    /*
     * Transform according to KBP expectations: 
     *  - Array indexation from max size-1 (e.g. 9 when the database size is 10 Bytes) to 0
     */
    for (byte_ndx = 0; byte_ndx < payload_size_in_bytes; ++byte_ndx)
    {
        byte_array_idx = payload_size_in_bytes - byte_ndx - 1;
        fld_val = asso_data[byte_array_idx];
        SHR_BITCOPY_RANGE(asso_data_buffer, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), &fld_val, 0, SOC_SAND_NOF_BITS_IN_BYTE);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_from_kbp_payload_buffer_decode()",0,0);
}

STATIC
 uint32
  arad_pp_frwrd_ip_tcam_route_kbp_payload_decode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  uint32                     lookup_id,
    SOC_SAND_IN  uint8                      *asso_data,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD *payload
  )
{
    uint32
        res,
        asso_data_buffer[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_LONGS];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(payload);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    sal_memset(asso_data_buffer, 0x0, sizeof(uint32) * SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_LONGS);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);

    res = arad_pp_frwrd_ip_tcam_route_from_kbp_payload_buffer_decode(
             unit,
             frwrd_table_id,
             asso_data,
             asso_data_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Build the buffer */
    if (lookup_id == 0) {
        res = arad_pp_lem_access_payload_parse(
                unit,
                ARAD_PP_LEM_ENTRY_TYPE_UNKNOWN,
                asso_data_buffer,
                payload
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
    else {
        
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_kbp_payload_decode()",0,0);
}

#ifdef KBP_COMPARE_DEBUG
/* Compare to see if insertion and deletion was correct */
uint32
  arad_pp_frwrd_ip_tcam_route_kbp_compare(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  uint8                      *m_data
  )
{
    uint32
        res;
    ARAD_KBP_FRWRD_IP_LTR   
        ltr_id;
    uint8 
        master_key[80];
    uint8 found = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(master_key, 0x0, sizeof(uint8) * 80);

    switch (frwrd_table_id) {
    case ARAD_KBP_FRWRD_TBL_ID_LSR:
        sal_memcpy(&master_key[0], &m_data[0], 6);
        break;
    case ARAD_KBP_FRWRD_TBL_ID_TRILL_UC:
        sal_memcpy(&master_key[0], &m_data[0], 2);
        break;
    case ARAD_KBP_FRWRD_TBL_ID_TRILL_MC:
        sal_memcpy(&master_key[0], &m_data[0], 4);
        break;
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
    case ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1: /* do not compare for table 0 */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
    case ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0: 
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
        sal_memcpy(&master_key[0], &m_data[0], 2);
        sal_memcpy(&master_key[2], &m_data[2], 16);
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
        sal_memcpy(&master_key[18], &m_data[2], 16);
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
        break;
    case ARAD_KBP_FRWRD_TBL_ID_IPV6_MC:
        sal_memcpy(&master_key[2],  &m_data[0], 2);
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
        sal_memcpy(&master_key[4],  &m_data[17], 16);
        sal_memcpy(&master_key[20], &m_data[2], 15);
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
        sal_memcpy(&master_key[4],  &m_data[2], 15);
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
        break;
    case ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1: /* do not compare for table 0 */
        sal_memcpy(master_key, &m_data[0], 6);
        sal_memcpy(&master_key[6], &m_data[2], 4);
        break;
    case ARAD_KBP_FRWRD_TBL_ID_IPV4_MC:
        /* Two first bits are VRF, searched in different table */
        sal_memcpy(&master_key[2], &m_data[0], 2);
        sal_memcpy(&master_key[4], &m_data[6], 4);
        sal_memcpy(&master_key[8], &m_data[2], 4);
        break;
    
    case ARAD_KBP_FRWRD_IP_NOF_TABLES:
        sal_memcpy(&master_key[0], &m_data[0], 2);
        break;
    case (ARAD_KBP_FRWRD_IP_NOF_TABLES + 1):
        sal_memcpy(&master_key[2], &m_data[0], 6);
        break;
    default:
        ARAD_DO_NOTHING_AND_EXIT;
        break;
    }

    res = arad_kbp_table_ltr_id_get(
            unit, 
            frwrd_table_id,
            &ltr_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit); 

    res = arad_pp_frwrd_ip_tcam_kbp_route_compare(
            unit,
            ltr_id, 
            master_key, 
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_kbp_compare()",0,0);
}
#endif /* KBP_COMPARE_DEBUG */


/* Add entry according to data and mask */
uint32
  arad_pp_frwrd_ip_tcam_route_kbp_add_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_data,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_mask,
    SOC_SAND_IN  uint32                     priority,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD *payload,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  )
{
    uint32
        res;
    ARAD_PP_IP_TCAM_ENTRY_KEY
      key;
    ARAD_TCAM_ACTION                                
        action;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_memset(&key, 0x0, sizeof(key));
    soc_sand_os_memset(&action, 0x0, sizeof(action));

    key.type = ARAD_IP_TCAM_FROM_KBP_FRWRD_IP_TBL_ID(frwrd_table_id);
    key.key.elk_fwd.priority = priority;

    /* 
     * Encode Key: 
     * - use LEM access logic to transform the LEM access key to a 
     * buffer (done both on data and mask) 
     * - transform the buffer to the order expected by KBP 
     */
    res = arad_pp_frwrd_ip_tcam_route_kbp_buffer_encode(
            unit,
            frwrd_table_id,
            route_key_data,
            route_key_mask,
            key.key.elk_fwd.m_data,
            key.key.elk_fwd.m_mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    /* Encode action in a buffer */
    res = arad_pp_frwrd_ip_tcam_route_kbp_payload_encode(
            unit,
            frwrd_table_id,
            payload,
            action.elk_ad_value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit); 

    /* Add record */
    res = arad_pp_frwrd_ip_tcam_route_add_unsafe(
            unit,
            &key,
            &action,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

#ifdef KBP_COMPARE_DEBUG
    res = arad_pp_frwrd_ip_tcam_route_kbp_compare(
            unit,
            frwrd_table_id,
            key.key.elk_fwd.m_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit); 
#endif
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_kbp_add_unsafe()",0,0);
}

/* Get entry according to data and mask */
uint32
  arad_pp_frwrd_ip_tcam_route_kbp_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_data,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_mask,
    SOC_SAND_OUT uint32                     *priority,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD *payload,
    SOC_SAND_OUT uint8                      *found
  )
{
    uint32
        res;
    ARAD_PP_IP_TCAM_ENTRY_KEY
      key;
    ARAD_TCAM_ACTION                                
        action;
    uint8
        hit_bit;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_memset(&key, 0x0, sizeof(key));
    soc_sand_os_memset(&action, 0x0, sizeof(action));

    *priority = 0;
    *found = 0;
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);

    key.type = ARAD_IP_TCAM_FROM_KBP_FRWRD_IP_TBL_ID(frwrd_table_id);

    /* 
     * Encode Key: 
     * - use LEM access logic to transform the LEM access key to a 
     * buffer (done both on data and mask) 
     * - transform the buffer to the order expected by KBP 
     */
    res = arad_pp_frwrd_ip_tcam_route_kbp_buffer_encode(
            unit,
            frwrd_table_id,
            route_key_data,
            route_key_mask,
            key.key.elk_fwd.m_data,
            key.key.elk_fwd.m_mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    if (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_FORWARDING(key.type)) {
        /* Get record */
        res = arad_pp_frwrd_ip_tcam_entry_hw_get(
                unit,
                &key,
                &action,
                found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 
    }
    else {
        /* Get record */
        res = arad_pp_frwrd_ip_tcam_route_get_unsafe(
            unit,
                &key,
                TRUE /* exact_match */,
                &action,
                found,
                &hit_bit
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 
    }


    /* Parse the action value if found */
    if (*found) 
    {
        *priority = key.key.elk_fwd.priority;
        /* Encode action in a buffer */
        res = arad_pp_frwrd_ip_tcam_route_kbp_payload_decode(
                unit,
                frwrd_table_id,
                0 /* lookup_id */,
                action.elk_ad_value,
                payload
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit); 
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_kbp_get_unsafe()",0,0);
}

/* Remove entry according to data and mask */
uint32
  arad_pp_frwrd_ip_tcam_route_kbp_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_data,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_mask
  )
{
    uint32
        res;
    ARAD_PP_IP_TCAM_ENTRY_KEY
      key;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_memset(&key, 0x0, sizeof(key));

    key.type = ARAD_IP_TCAM_FROM_KBP_FRWRD_IP_TBL_ID(frwrd_table_id);

    /* 
     * Encode Key: 
     * - use LEM access logic to transform the LEM access key to a 
     * buffer (done both on data and mask) 
     * - transform the buffer to the order expected by KBP 
     */
    res = arad_pp_frwrd_ip_tcam_route_kbp_buffer_encode(
            unit,
            frwrd_table_id,
            route_key_data,
            route_key_mask,
            key.key.elk_fwd.m_data,
            key.key.elk_fwd.m_mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    /* Remove Record */
    res = arad_pp_frwrd_ip_tcam_route_remove_unsafe(
            unit,
            &key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

#ifdef KBP_COMPARE_DEBUG
    res = arad_pp_frwrd_ip_tcam_route_kbp_compare(
            unit,
            frwrd_table_id,
            key.key.elk_fwd.m_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit); 
#endif
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_kbp_remove_unsafe()",0,0);
}

uint32
  arad_pp_frwrd_ip_tcam_route_kbp_get_block_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID                   frwrd_table_id,
    SOC_SAND_INOUT  ARAD_PP_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY                    *route_key_data,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY                    *route_key_mask,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD                 *payload,
    SOC_SAND_OUT uint32                                     *nof_entries
  )
{
    uint32
      res = SOC_SAND_OK,
      indx;
    ARAD_PP_IP_TCAM_ENTRY_KEY
      *keys = NULL;
    ARAD_TCAM_ACTION
      *actions = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(block_range_key);
    SOC_SAND_CHECK_NULL_INPUT(route_key_data);
    SOC_SAND_CHECK_NULL_INPUT(route_key_mask);
    SOC_SAND_CHECK_NULL_INPUT(payload);
    SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    if(block_range_key->entries_to_act == 0)
    {
      *nof_entries = 0;
      goto exit;
    }

    ARAD_ALLOC_ANY_SIZE(keys, ARAD_PP_IP_TCAM_ENTRY_KEY, block_range_key->entries_to_act, "keys frwrd_ipv6_vrf_route_get_block");
    ARAD_ALLOC_ANY_SIZE(actions, ARAD_TCAM_ACTION, block_range_key->entries_to_act, "actions frwrd_ipv6_vrf_route_get_block");

    for(indx = 0; indx < block_range_key->entries_to_act; ++indx)
    {
      keys[indx].type = ARAD_IP_TCAM_FROM_KBP_FRWRD_IP_TBL_ID(frwrd_table_id);
    }

    res = arad_pp_frwrd_ip_tcam_route_get_block_unsafe(
             unit,
             block_range_key,
             keys,
             actions,
             nof_entries
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    for(indx = 0; indx < *nof_entries; ++indx)
    {
        /* Convert the entries */
        res = arad_pp_frwrd_ip_tcam_route_kbp_buffer_decode(
                unit,
                frwrd_table_id,
                keys[indx].key.elk_fwd.m_data,
                keys[indx].key.elk_fwd.m_mask,
                &route_key_data[indx],
                &route_key_mask[indx]
             );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

        /* Translate the actions */
        res = arad_pp_frwrd_ip_tcam_route_kbp_payload_decode(
                unit,
                frwrd_table_id,
                0 /* lookup_id */,
                actions[indx].elk_ad_value,
                &payload[indx]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit); 
    }

exit:
  if(keys != NULL)
  {
      soc_sand_os_free(keys);
  }
  if(actions != NULL)
  {
      soc_sand_os_free(actions);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_route_kbp_get_block_unsafe()", 0, 0);
}

#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

/*
 * given a location in tcam bank, search for the entry in SW database.
 * If exists get the value+mask from from hash table and write it,
 * otherwise, write an empty entry.
 * the argument global_location is only valid if entry_exists is true
 */
uint32
  arad_pp_frwrd_ip_tcam_rewrite_entry(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  uint8                      entry_exists,
     SOC_SAND_IN  ARAD_TCAM_GLOBAL_LOCATION  *global_location,
     SOC_SAND_IN  ARAD_TCAM_LOCATION         *location
  )
{
    uint32
        res = SOC_SAND_OK;
    SOC_SAND_HASH_TABLE_INFO
        *hash_tbl;
    uint8
        found;
    uint8
      stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES];

    ARAD_PP_IP_TCAM_ENTRY_KEY
        route_key;
    ARAD_IP_TCAM_ENTRY_TYPE
        tcam_key_type;
    ARAD_TCAM_ENTRY
        entry;
    ARAD_TCAM_BANK_ENTRY_SIZE
        entry_size;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(global_location);
    SOC_SAND_CHECK_NULL_INPUT(location);

    /*
     * In case FLP is the owner of the TCAM bank, the handling of the
     * error is different for TCAM banks and action tables (SRAM memory).
     * The reason for the different handling is that TCAM entries (keys)
     * are saved in SW, however  the actions aren't. When it is needed
     * to read the action, then the route key is supplied and action is
     * retrieved from HW.
     */

    found = FALSE;
    ARAD_TCAM_ENTRY_clear(&entry);

    if(entry_exists)
    {
        /* In case entry exists it is necessary to reconstruct
         * its key and mask in order to rewrite it */
        tcam_key_type = global_location->tcam_db_id;

        /* get the hash table for specific tcam database */
        res = arad_pp_ip_tcam_hash_tbl_get(unit, tcam_key_type, &hash_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

        /* if entry exists get its value */
        res = soc_sand_hash_table_get_by_index(hash_tbl, global_location->entry_id, stream, &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

        if(found)
        {
            /* if entry was found convert its value to route key format */
            res = arad_pp_ip_tcam_stream_to_key(unit, stream, tcam_key_type, &route_key);
            SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

            /* convert route key format to tcam entry */
            arad_pp_ip_tcam_entry_build(unit, &route_key, &entry);
            entry.is_for_update = FALSE;
        }
        else
        {
            /* error if entry was not found - entry_exists indicates otherwise */
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
        }
    }
    /* else :
     * In case entry does not exist then rewrite a cleared
     * entry in order to refresh the key, mask and parity
     */

    res = arad_sw_db_tcam_bank_entry_size_get(unit, location->bank_id, &entry_size);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    /* write entry to specific location in HW without SW management */
    res = arad_tcam_entry_rewrite(unit, entry_exists, global_location->tcam_db_id, location, entry_size, &entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_rewrite_entry()", 0, 0);
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */


