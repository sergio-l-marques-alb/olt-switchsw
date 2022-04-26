/* $Id: jer_pp_kaps.h, Broadcom SDK Exp $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef __JER_PP_KAPS_INCLUDED__
/* { */
#define __JER_PP_KAPS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/dpp/SAND/Utils/sand_header.h>
#include <libs/kaps/include/kaps_db.h>
#include <libs/kaps/include/kaps_default_allocator.h>
#include <libs/kaps/include/kaps_device.h>
#include <libs/kaps/include/kaps_key.h>
#include <libs/kaps/include/kaps_instruction.h>
#include <libs/kaps/include/kaps_errors.h>
#include <libs/kaps/include/kaps_ad.h>
#include <libs/kaps/include/kaps_legacy.h>
#include <libs/kaps/include/kaps_init.h>
#include <libs/kaps/include/kaps_portable.h>
#include <libs/kaps/include/kaps_dma.h>
#include <libs/kaps/include/kaps_hb.h>
#include <soc/dpp/JER/jer_drv.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES   (4)
#define JER_KAPS_MAX_NOF_KEY_SEGMENTS           (5)
#define JER_KAPS_AD_WIDTH_IN_BITS               (20)
#define JER_KAPS_AD_WIDTH_IN_BYTES              (3)

#define JER_KAPS_ENABLE_PRIVATE_DB(unit)        ((SOC_DPP_JER_CONFIG(unit)->pp.kaps_private_ip_frwrd_table_size) > 0)
#define JER_KAPS_ENABLE_PUBLIC_DB(unit)         ((SOC_DPP_JER_CONFIG(unit)->pp.kaps_public_ip_frwrd_table_size) > 0)
#define JER_KAPS_ENABLE_DMA(unit)               ((SOC_DPP_JER_CONFIG(unit)->pp.kaps_large_db_size) > 0)
#define JER_KAPS_ENABLE(unit)                   ((SOC_IS_JERICHO(unit)) && (JER_KAPS_ENABLE_PRIVATE_DB(unit) || JER_KAPS_ENABLE_PUBLIC_DB(unit) || JER_KAPS_ENABLE_DMA(unit)))

#define JER_KAPS_INRIF_WIDTH_IN_BITS            (15)
#define JER_KAPS_VRF_WIDTH_IN_BITS              (14)
#define JER_KAPS_INRIF_WIDTH_PADDING_IN_BITS    (1)

#define JER_KAPS_KEY_BUFFER_NOF_BYTES           (20)

#define JER_KAPS_TABLE_PREFIX_LENGTH            (2)
#define JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH    (6)

#define JER_KAPS_IP_PUBLIC_INDEX                (JER_KAPS_IP_CORE_0_PUBLIC_IPV4_UC_TBL_ID)

#define JER_KAPS_RPB_BLOCK_INDEX_START          (1)
#define JER_KAPS_RPB_BLOCK_INDEX_END            (SOC_IS_QAX(unit) ? 2 : 4)

/*DBAL supports up to 5-bit prefix.
  IPV4 MC + IPV6 UC/MC have a 2-bit prefix - IPv6 utilizes 157 out of 160 available bits not including table prefix.
  IPv4 UC and non-IP tables utilize the 3 LSbits, they share the remaining 2-bit prefix.
  */
#define JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_NUM   (8)
#define JER_KAPS_TABLE_PREFIX_MAX_LENGTH        (5)
#define JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_BIT   (JER_KAPS_TABLE_PREFIX_MAX_LENGTH - JER_KAPS_TABLE_PREFIX_LENGTH)

/* As used to calculate DBs count from the SOC properties, where corresponds to 1 BB*/
#define JER_KAPS_DMA_DB_NOF_ENTRIES             (1000)
/* The actual number of lines per BB, as seen from PMF */
#define JER_KAPS_DMA_DB_NOF_PMF_ENTRIES         (SOC_IS_JERICHO_PLUS_ONLY(unit) ? 16 * 1024 : (SOC_IS_QUX(unit) ? 1024 : 4096))
#define JER_KAPS_NOF_MAX_DMA_DB                 (32) 
/* DMA_DB_WIDTH is 128 for Jericho, 127 for QAX, 120 for QUX/JER+ */
#define JER_KAPS_DMA_DB_WIDTH_MAX               (128)
#define JER_KAPS_DMA_DB_WIDTH(_unit)            (SOC_IS_QUX(unit) ? 120 : (SOC_IS_QAX(_unit) ? 127 : (SOC_IS_JERICHO_PLUS(_unit) ? 120 : 128)))

/*
 * Value to use in clone_table_id to indicate that
 * the table handle is the same as the DB handle.    */
#define JER_KAPS_TABLE_USE_DB_HANDLE            (0xFF)

/* Translate KAPS result to SOC result */
#define JER_KAPS_TO_SOC_RESULT(result)     ((result == 0) ? SOC_SAND_OK : SOC_SAND_ERR)

#ifndef KAPS_WARMBOOT_TYPE_DEFINED
#define KAPS_WARMBOOT_TYPE_DEFINED
/* kaps device warmboot info */
typedef struct kaps_warmboot_s{
    FILE *kaps_file_fp;
    kaps_device_issu_read_fn  kaps_file_read;
    kaps_device_issu_write_fn kaps_file_write;
}kaps_warmboot_t;
#endif /* KAPS_WARMBOOT_TYPE_DEFINED */

/* } */

/*************
 * ENUMS     *
 *************/
/* { */

/*Keep JER_KAPS_DB_NAMES updated*/
typedef enum
{
    JER_KAPS_IP_CORE_0_PRIVATE_DB_ID = 0,
    JER_KAPS_IP_CORE_1_PRIVATE_DB_ID = 1,
    JER_KAPS_IP_CORE_0_PUBLIC_DB_ID  = 2,
    JER_KAPS_IP_CORE_1_PUBLIC_DB_ID  = 3,
    
    JER_KAPS_IP_NOF_DB        

} JER_KAPS_IP_DB_ID;

/*Keep JER_KAPS_TABLE_NAMES updated*/
/*JER_KAPS_IP_PUBLIC_INDEX points to the first public table index*/
/* KAPS IPV4_UC TBL_IDs are also used by dynamic tables */
typedef enum
{
    JER_KAPS_IP_CORE_0_PRIVATE_IPV4_UC_TBL_ID  = 0, /* Table is not actually created. Private forwarding DB handle is used.*/
    JER_KAPS_IP_CORE_1_PRIVATE_IPV4_UC_TBL_ID  = 1, /* Table is not actually created. Private CORE_1 DB handle is used.*/
    JER_KAPS_IP_CORE_0_PRIVATE_IPV6_UC_TBL_ID  = 2,
    JER_KAPS_IP_CORE_1_PRIVATE_IPV6_UC_TBL_ID  = 3,
    JER_KAPS_IP_CORE_0_PRIVATE_IPV4_MC_TBL_ID  = 4,
    JER_KAPS_IP_CORE_0_PRIVATE_IPV6_MC_TBL_ID  = 5,
    JER_KAPS_IP_CORE_0_PUBLIC_IPV4_UC_TBL_ID   = 6, /* Table is not actually created. Public forwarding DB handle is used.*/
    JER_KAPS_IP_CORE_1_PUBLIC_IPV4_UC_TBL_ID   = 7, /* Table is not actually created. Public CORE_1 DB handle is used.*/
    JER_KAPS_IP_CORE_0_PUBLIC_IPV6_UC_TBL_ID   = 8,
    JER_KAPS_IP_CORE_1_PUBLIC_IPV6_UC_TBL_ID   = 9,
    JER_KAPS_IP_CORE_0_PUBLIC_IPV4_MC_TBL_ID   = 10,
    JER_KAPS_IP_CORE_0_PUBLIC_IPV6_MC_TBL_ID   = 11,
    
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

/*Default table prefixes, these only account for the 2 MSbits, if there are more than 4 tables, IPV4_UC is extended (depends on soc properties) */
typedef enum
{
    JER_KAPS_IPV6_MC_TABLE_PREFIX = 0,
    JER_KAPS_IPV6_UC_TABLE_PREFIX = 1,
    JER_KAPS_IPV4_MC_TABLE_PREFIX = 2,
    JER_KAPS_IPV4_UC_AND_NON_IP_TABLE_PREFIX = 3,

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

typedef struct kaps_instruction               JER_KAPS_INSTRUCTION;
typedef struct kaps_key                       JER_KAPS_KEY;
typedef struct kaps_db                        JER_KAPS_DB;
typedef struct kaps_ad_db                     JER_KAPS_AD_DB;
typedef struct kaps_hb_db                     JER_KAPS_HB_DB;
typedef struct kaps_dma_db                    JER_KAPS_DMA_DB;
typedef struct kaps_device                    JER_KAPS_DEVICE;
typedef struct kaps_complete_search_result    JER_KAPS_SEARCH_RESULT;
typedef struct kaps_ad                        JER_KAPS_AD;
typedef struct kaps_entry                     JER_KAPS_ENTRY;
typedef struct kaps_entry_iter                JER_KAPS_ENTRY_ITER;
typedef struct kaps_entry_info                JER_KAPS_ENTRY_INFO;
typedef struct kaps_db_stats                  JER_KAPS_DB_STATS;
typedef struct kaps_hb                        JER_KAPS_HB;
typedef struct kaps_allocator                 JER_KAPS_ALLOCATOR;

/* Represents one segment in the master key */
typedef struct
{
    /* A logical name to be recognized with this segment */
    char name[20];

    /* Number of bits to take */
    uint8 nof_bits;

    /* A type (JER_KAPS_KEY)to be recognized with this segment */
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

    /* Associated hitbit DB handle */
    JER_KAPS_HB_DB *hb_db_p;

    /* direct access DB handle */
    JER_KAPS_DMA_DB *dma_db_p;

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
    /* Number of valid tables, equal to the number of active RPBs */
    /* Used for instruction initialization */
    uint8 valid_tables_num;

    /* Number of valid tables, equal to the total number of RPBs */
    /* Used for key construction */
    uint8 max_tables_num;

    /* The Tables that should be searched.
     */
    JER_KAPS_IP_TBL_ID tbl_id[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];

    /* An instruction represents a search for a packet.
     */
    JER_KAPS_INSTRUCTION *inst_p;

} JER_KAPS_SEARCH_CONFIG;


typedef struct genericJerAppData
{
    JER_KAPS_ALLOCATOR *dalloc_p;
    void *kaps_xpt_p;
    JER_KAPS_DEVICE *kaps_device_p;

} genericJerAppData;

typedef struct
{
    PARSER_HINT_PTR JER_KAPS_DB       *db_p;
    PARSER_HINT_PTR JER_KAPS_AD_DB    *ad_db_p;
    PARSER_HINT_PTR JER_KAPS_HB_DB    *hb_db_p;
                    uint8                is_valid;
} JER_KAPS_DB_HANDLES;

typedef struct
{
    PARSER_HINT_PTR JER_KAPS_DMA_DB    *dma_db_p;
                    uint8                is_valid;
} JER_KAPS_DMA_DB_HANDLES;

typedef struct
{
    PARSER_HINT_PTR JER_KAPS_INSTRUCTION *inst_p;
} JER_KAPS_INSTRUCTION_HANDLES;

typedef struct
{
    PARSER_HINT_PTR JER_KAPS_DB       *tbl_p;
} JER_KAPS_TABLE_HANDLES;



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

uint32 jer_kaps_deinit_app(int unit, int no_sync_flag);

/* sync KAPS SW state */
int jer_kaps_sync(int unit);

/* enable/disable autosync for KAPS SW state */
int jer_kaps_autosync_set(int unit, int enable);

/* register file and read/write functions for KAPS warmboot usage*/
void jer_kaps_warmboot_register(int unit,
                                FILE *file_fp,
                                kaps_device_issu_read_fn read_fn, 
                                kaps_device_issu_write_fn write_fn);

void jer_kaps_db_get(int unit,
                     uint32 tbl_id,
                     JER_KAPS_DB **db_p);

void jer_kaps_ad_db_get(int unit,
                        uint32 tbl_id,
                        JER_KAPS_AD_DB **ad_db_p);

void jer_kaps_hb_db_get(int unit,
                        uint32 tbl_id,
                        JER_KAPS_HB_DB **ad_hb_p);

void jer_kaps_dma_db_get(int unit,
                         uint32 db_id,
                         JER_KAPS_DMA_DB **db_p);

void jer_kaps_search_config_get(int unit,
                         uint32 search_id,
                         JER_KAPS_SEARCH_CONFIG *search_cfg_p);

int jer_kaps_search_generic(int unit, uint32 search_id, uint8 key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][JER_KAPS_KEY_BUFFER_NOF_BYTES],
                            uint32 *return_is_matched, uint32 *return_prefix_len, uint8  return_payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)],
                            JER_KAPS_SEARCH_RESULT *return_cmp_rslt);

void jer_kaps_table_config_get(int unit,
                               uint32 tbl_id,
                               JER_KAPS_TABLE_CONFIG *table_cfg_p);

void jer_kaps_ad_db_by_db_id_get(int unit,
                                 uint32 db_id,
                                 JER_KAPS_AD_DB **ad_db_p);

uint8 jer_kaps_clone_of_db_id_get(int unit,
                                  uint32 db_id);

void* jer_kaps_kaps_xpt_p_get(int unit);

void* jer_kaps_app_data_get(int unit);

uint32 jer_kaps_hb_timer(int unit);


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __JER_PP_KAPS_INCLUDED__ */

#endif

