/* $Id: jer_pp_kaps.h, hagayco Exp $
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

#ifndef __JER_PP_KAPS_INCLUDED__
/* { */
#define __JER_PP_KAPS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/key.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/kbp_legacy.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/kbp_portable.h>
#include <soc/dpp/JER/jer_drv.h>

#include <soc/dpp/ARAD/arad_kbp.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES   (4)
#define JER_KAPS_MAX_NOF_KEY_SEGMENTS           (5)
#define JER_KAPS_AD_WIDTH_IN_BITS               (20)

#define JER_KAPS_ENABLE_PRIVATE_DB(unit)        ((SOC_DPP_JER_CONFIG(unit)->pp.kaps_private_ip_frwrd_table_size) > 0)
#define JER_KAPS_ENABLE_PUBLIC_DB(unit)         ((SOC_DPP_JER_CONFIG(unit)->pp.kaps_public_ip_frwrd_table_size) > 0)
#define JER_KAPS_ENABLE(unit)                   ((SOC_IS_JERICHO(unit)) && (JER_KAPS_ENABLE_PRIVATE_DB(unit) || JER_KAPS_ENABLE_PUBLIC_DB(unit)))

#define JER_KAPS_INRIF_WIDTH_IN_BITS            (15)
#define JER_KAPS_VRF_WIDTH_IN_BITS              (14)
#define JER_KAPS_INRIF_WIDTH_PADDING_IN_BITS    (1)

#define JER_KAPS_KEY_BUFFER_NOF_BYTES           (20)

#define JER_KAPS_IP_PUBLIC_INDEX                (JER_KAPS_IP_FWD_PUBLIC_IPV4_UC_TBL_ID)

#define JER_KAPS_RPB_BLOCK_INDEX_START          (1)
#define JER_KAPS_RPB_BLOCK_INDEX_END            (4)


/* } */

/*************
 * ENUMS     *
 *************/
/* { */

/*Keep JER_KAPS_DB_NAMES updated*/
typedef enum
{
    JER_KAPS_IP_FWD_PRIVATE_DB_ID = 0,
    JER_KAPS_IP_RPF_PRIVATE_DB_ID = 1,
    JER_KAPS_IP_FWD_PUBLIC_DB_ID  = 2,
    JER_KAPS_IP_RPF_PUBLIC_DB_ID  = 3,
    
    JER_KAPS_IP_NOF_DB        

} JER_KAPS_IP_DB_ID;

/*Keep JER_KAPS_TABLE_NAMES updated*/
/*JER_KAPS_IP_PUBLIC_INDEX points to the first public table index*/
typedef enum
{
    JER_KAPS_IP_FWD_PRIVATE_IPV4_UC_TBL_ID  = 0, /* Table is not actually created. Private forwarding DB handle is used.*/
    JER_KAPS_IP_RPF_PRIVATE_IPV4_UC_TBL_ID  = 1, /* Table is not actually created. Private RPF DB handle is used.*/
    JER_KAPS_IP_FWD_PRIVATE_IPV6_UC_TBL_ID  = 2,
    JER_KAPS_IP_RPF_PRIVATE_IPV6_UC_TBL_ID  = 3,
    JER_KAPS_IP_FWD_PRIVATE_IPV4_MC_TBL_ID  = 4,
    JER_KAPS_IP_FWD_PRIVATE_IPV6_MC_TBL_ID  = 5,
    JER_KAPS_IP_FWD_PUBLIC_IPV4_UC_TBL_ID   = 6, /* Table is not actually created. Public forwarding DB handle is used.*/
    JER_KAPS_IP_RPF_PUBLIC_IPV4_UC_TBL_ID   = 7, /* Table is not actually created. Public RPF DB handle is used.*/
    JER_KAPS_IP_FWD_PUBLIC_IPV6_UC_TBL_ID   = 8,
    JER_KAPS_IP_RPF_PUBLIC_IPV6_UC_TBL_ID   = 9,
    JER_KAPS_IP_FWD_PUBLIC_IPV4_MC_TBL_ID   = 10,
    JER_KAPS_IP_FWD_PUBLIC_IPV6_MC_TBL_ID   = 11,
    
    JER_KAPS_IP_NOF_TABLES        

} JER_KAPS_IP_TBL_ID;

typedef enum
{
    JER_KAPS_IPV4_UC_SEARCH_ID  = 0,
    JER_KAPS_IPV6_UC_SEARCH_ID  = 1,
    JER_KAPS_IPV4_MC_SEARCH_ID  = 2,
    JER_KAPS_IPV6_MC_SEARCH_ID  = 3,
    
    JER_KAPS_NOF_SEARCHES        

} JER_KAPS_IP_SEARCH_ID;

typedef enum
{
    JER_KAPS_IPV4_UC_TABLE_PREFIX = 0,
    JER_KAPS_IPV4_MC_TABLE_PREFIX = 1,
    JER_KAPS_IPV6_UC_TABLE_PREFIX = 2,
    JER_KAPS_IPV6_MC_TABLE_PREFIX = 3,

    JER_KAPS_NOF_TABLE_PREFIXES
}JER_KAPS_TABLE_PREFIX;

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

typedef struct kbp_instruction      JER_KAPS_INSTRUCTION;
typedef struct kbp_key              JER_KAPS_KEY;
typedef struct kbp_db               JER_KAPS_DB;
typedef struct kbp_ad_db            JER_KAPS_AD_DB;

/* Represents one segment in the master key */
typedef struct
{
    /* A logical name to be recognized with this segment */
    char name[20];

    /* Number of bits to take */
    uint8 nof_bits;

    /* A type (kbp_key_field_type)to be recognized with this segment */
    int type;

} JER_KAPS_KEY_SEGMENT;

typedef struct 
{
    /* The number of key segments - the number of fields that
     * are looked up in the table.
     */
    uint32 nof_key_segments;

    /* Key Segment - one of several fields that are being searched
     * in the table. The key segment should correspond to the D/M
     * structure when adding a record.
     */
    JER_KAPS_KEY_SEGMENT key_segment[JER_KAPS_MAX_NOF_KEY_SEGMENTS];

} JER_KAPS_KEY_FIELDS;

/* Configuration information needed for one lookup DB */
typedef struct 
{
    /* This table configuration is valid */
    uint8 valid;

    /* DB size - number of entries to allocate for
     * this table
     */ 
    uint32 db_size;

    /* Clone of DB ID - this table is a clone of db_id.
     * if JER_KAPS_IP_NOF_DB then not a clone.
     */
    uint8 clone_of_db_id;

    /* DB handle */
    JER_KAPS_DB *db_p;

    /* Associated data DB handle */
    JER_KAPS_AD_DB *ad_db_p;

} JER_KAPS_DB_CONFIG;

/* Configuration information needed for one lookup table */
typedef struct
{
    /* DB ID - the DB this table belongs to */
    uint8 db_id;

    /* Clone of TBL ID - this table is a clone of tbl_id.
     * if JER_KAPS_IP_NOF_TABLES then not a clone. 
     * if JER_KAPS_TABLE_USE_DB_HANDLE then DB's handle is used instead of creating a new table handle. 
     */
    uint8 clone_of_tbl_id;

    /* key fields information. */
    JER_KAPS_KEY_FIELDS key_fields;

    /* Table handle */
    JER_KAPS_DB *tbl_p;

} JER_KAPS_TABLE_CONFIG;


/* Configuration information needed for one search instruction */
typedef struct 
{
    /* This search configuration is valid */
    uint8 valid;

    /* The Tables that should be searched.
     */
    JER_KAPS_IP_TBL_ID tbl_id[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];

    /* An instruction represents a search for a packet.
     */
    JER_KAPS_INSTRUCTION *inst_p;

} JER_KAPS_SEARCH_CONFIG;


typedef struct genericJerAppData
{
    globalGTMInfo g_gtmInfo[JER_KAPS_IP_NOF_DB];

    struct kbp_allocator *dalloc_p;
    void *kaps_xpt_p;
    struct kbp_device *kaps_device_p;

} genericJerAppData;

typedef struct
{
    PARSER_HINT_PTR struct kbp_db       *db_p;
    PARSER_HINT_PTR struct kbp_ad_db    *ad_db_p;
                    uint8                is_valid;
} JER_KAPS_DB_HANDLES;

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

void jer_kaps_sw_init(int unit);

uint32 jer_kaps_init_app(int unit);

uint32 jer_kaps_deinit_app(int unit);

/* sync KAPS SW state */
int jer_kaps_sync(int unit);

/* register file and read/write functions for KAPS warmboot usage*/
void jer_kaps_warmboot_register(int unit,
                                FILE *file_fp,
                                kbp_device_issu_read_fn read_fn, 
                                kbp_device_issu_write_fn write_fn);

void jer_kaps_db_get(int unit,
                     uint32 tbl_id,
                     struct kbp_db **db_p);

void jer_kaps_ad_db_get(int unit,
                        uint32 tbl_id,
                        struct kbp_ad_db **ad_db_p);

void jer_kaps_search_config_get(int unit,
								uint32 search_id,
								JER_KAPS_SEARCH_CONFIG *search_cfg_p);

int jer_kaps_search_generic(int unit, uint32 search_id, uint8 key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][JER_KAPS_KEY_BUFFER_NOF_BYTES] );

void jer_kaps_table_config_get(int unit,
                               uint32 tbl_id,
                               JER_KAPS_TABLE_CONFIG *table_cfg_p);

int jer_kaps_sw_search_test(int unit, uint32 tbl_id, uint8 *key, uint8 *payload, int32 *prefix_len, int32 *is_matched);

int jer_kaps_hw_search_test(int unit, uint32 search_id, uint8 *master_key, struct kbp_search_result *cmp_rslt);

int jer_kaps_show_table(int unit, uint32 tbl_idx);
int jer_kaps_show_ip_search(int unit, uint32 ip_search);

int jer_pp_kaps_diag_01(int unit);
int jer_pp_kaps_diag_02(int unit);

int jer_pp_kaps_tcam_bist(int unit);

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __JER_PP_KAPS_INCLUDED__ */

#endif

