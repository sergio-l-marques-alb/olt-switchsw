/* $Id: jer2_tmc_api_ingress_traffic_mgmt.h,v 1.16 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef __SOC_DNX_API_INGRESS_TRAFFIC_MGMT_INCLUDED__
/* { */
#define __SOC_DNX_API_INGRESS_TRAFFIC_MGMT_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/types.h>
#include <soc/dnx/legacy/TMC/tmc_api_general.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_DNX_ITM_NOF_VSQS_IN_GROUP(unit, vsq_group) (                                                       \
            (vsq_group == SOC_DNX_ITM_VSQ_GROUP_CTGRY              ) ? (SOC_DNX_ITM_VSQ_GROUPA_SZE(unit)) : (  \
            (vsq_group == SOC_DNX_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS  ) ? (SOC_DNX_ITM_VSQ_GROUPB_SZE(unit)) : (  \
            (vsq_group == SOC_DNX_ITM_VSQ_GROUP_CTGRY_CNCTN_CLS    ) ? (SOC_DNX_ITM_VSQ_GROUPC_SZE(unit)) : (  \
            (vsq_group == SOC_DNX_ITM_VSQ_GROUP_STTSTCS_TAG        ) ? (SOC_DNX_ITM_VSQ_GROUPD_SZE(unit)) : (  \
            (vsq_group == SOC_DNX_ITM_VSQ_GROUP_SRC_PORT           ) ? (SOC_DNX_ITM_VSQ_GROUPE_SZE(unit)) : (  \
            (vsq_group == SOC_DNX_ITM_VSQ_GROUP_PG                 ) ? (SOC_DNX_ITM_VSQ_GROUPF_SZE(unit)) : 0 ))))))

#define SOC_DNX_ITM_PRIO_MAP_SIZE_IN_UINT32S 2


/* In Jericho per packet compensation is active only when compensation tag exists in PDM extention.
 * In JER2_QAX only per packet mode is active
 */

#ifdef FIXME_DNX_LEGACY
#define SOC_DNX_ITM_PER_PACKET_COMPENSATION_ENABLED(unit) (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 0 : \
                                                           ((SOC_DNX_CONFIG(unit)->pdm_extension.max_hdr_comp_ptr && \
                                                           (SOC_DNX_CONFIG(unit)->jer2_arad->init.dram.pdm_mode == JER2_ARAD_INIT_PDM_MODE_REDUCED)) || \
                                                            SOC_IS_QAX(unit)))
#else             
#define SOC_DNX_ITM_PER_PACKET_COMPENSATION_ENABLED(unit) (0)
#endif 

#ifdef FIXME_DNX_LEGACY /** SOC_DNX_CONFIG not supported */
#define SOC_DNX_ITM_COMPENSATION_LEGACY_MODE(unit) (SOC_IS_QAX(unit) && SOC_DNX_CONFIG(unit)->jer2_qax->per_packet_compensation_legacy_mode)
#endif

/*
 * Drop Tail Resolution 16 bytes
 */

/*     Total number of DRAM buffers. DRAM buffers are used to
*     store packets at the ingress. DRAM buffers are shared
*     between Multicast, Mini-multicast and Unicast buffers.  */
#define  SOC_DNX_ITM_NOF_DRAM_BUFFS (2*1024*1024)

/*
 *  All buffers may be used for Unicast
 */

/*     Maximal number of DRAM buffers used for Mini
*     Multicast. Mini Multicast buffers are used for snooping
*     and mirroring, or for Multicast where four or less
*     copies are required.                                    */
#define  SOC_DNX_ITM_DBUFF_MMC_MAX (64*1024)

/*
* Definition values for System Red
*/
#define SOC_DNX_ITM_SYS_RED_Q_SIZE_RANGES   16
#define SOC_DNX_ITM_SYS_RED_DRP_PROBS       16
#define SOC_DNX_ITM_SYS_RED_BUFFS_RNGS      4

  /* Maximum ITM-VSQ-Rate-Class: Value 15  */
#define SOC_DNX_ITM_VSQ_A_QT_RT_CLS_MAX(unit) (dnx_data_ingr_congestion.vsq.vsq_a_rate_class_nof_get(unit) -1)
#define SOC_DNX_ITM_VSQ_QT_RT_CLS_MAX(unit)   (dnx_data_ingr_congestion.vsq.vsq_rate_class_nof_get(unit) -1)
#define SOC_DNX_ITM_VSQ_NOF_RATE_CLASSES(unit)  (dnx_data_ingr_congestion.vsq.vsq_rate_class_nof_get(unit))
#define SOC_DNX_ITM_RATE_CLASS_MAX    63
#define SOC_DNX_ITM_NOF_RATE_CLASSES  (SOC_DNX_ITM_RATE_CLASS_MAX + 1)


#define SOC_DNX_ITM_WRED_GRANULARITY        (16*16)
/*2^32/100 is 42949672.96 ==> 42949673 ==> 0x28F5C29*/
#define SOC_DNX_WRED_NORMALIZE_FACTOR  (0x28F5C29)

/* Max value WRED max packet size:      */
#define SOC_DNX_ITM_WRED_MAX_PACKET_SIZE ((1 << 14) - 128)
#define SOC_DNX_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC SOC_DNX_ITM_WRED_MAX_PACKET_SIZE

/**/
#define SOC_DNX_ITM_NOF_RSRC_POOLS 2 
#define SOC_DNX_ITM_SHRD_RJCT_TH_MAX 0xfff
#define SOC_DNX_ITM_RESOURCE_ALLOCATION_SHARED_MAX 0x3fffff



/* (2^24 - 1) */
#define SOC_DNX_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX     0XFFFFFF

/* (2^16 - 1) *(2 ^ 15) */
#define SOC_DNX_ITM_GLOB_RCS_DROP_BDS_SIZE_MAX      0x7FF8000 

/* (2^23 - 1) */
#define SOC_DNX_ITM_GLOB_RCS_DROP_UC_SIZE_MAX       0X7FFFFF

/* (2^22 - 1) */
#define SOC_DNX_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX  0X3FFFFF

/* (2^24 - 1) */
#define SOC_DNX_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX      0XFFFF

/* (2^15 - 1) */
#define SOC_DNX_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX      0X7FFF

/* (2^22 - 1) */
#define SOC_DNX_ITM_VSQ_GRNT_BD_SIZE_MAX            0X3FFFFF

/* (2^22 - 1) */
#define SOC_DNX_ITM_VSQ_FC_BD_SIZE_MAX              0X3FFFFF

/* FADT adjust factor -7 <= alpha <= 7 */
#define SOC_DNX_ITM_FADT_ALPHA_MAX  (7)
#define SOC_DNX_ITM_FADT_ALPHA_MIN  (-7)
/*} */

/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* 
 * The allocation of the total per core resources between source and queue based reservation 
 * depends on one of two guarantee modes: strict and loose. 
 */
typedef enum
{
  /* 
   * Lossy guaranteed packet will be discarded only if Global shared resource is BLOCKED 
   * Lossless guaranteed packet will be discarded if Global is BLOCKED. 
   * Always permit VOQ guaranteed packets to overflow shared space.
   */
  SOC_DNX_ITM_CGM_MGMT_GUARANTEE_LOOSE = 0,
  /* 
   *  Lossy guaranteed traffic will be discarded if one of the shared resources (PG/Port/Global) is BLOCKED.
   *  Lossless guaranteed traffic will be discarded if one of the resources (PG/Port/Global) is PRE-BLOCKED.
   *  Permit VOQ guaranteed packet only if shared global resource is not BLOCKED.
   */
  SOC_DNX_ITM_CGM_MGMT_GUARANTEE_STRICT = 1
} SOC_DNX_ITM_CGM_MGMT_GUARANTEE_MODE;

/* 
 * Ingress compensation types
 */
typedef enum
{
  /* 
   * Per queue (destination based) compensation (translates to credit discount class profile)
   */
  SOC_DNX_ITM_PKT_SIZE_ADJUST_QUEUE = 0,

  /* 
   * Per port (source based) compensation (translates to port profile)
   */
  SOC_DNX_ITM_PKT_SIZE_ADJUST_PORT = 1,

  /* 
   * Per OutLif profile (desination based) compensation
   */
  SOC_DNX_ITM_PKT_SIZE_ADJUST_APPEND_SIZE_PTR = 2
} SOC_DNX_ITM_CGM_PKT_SIZE_ADJUST_TYPE;

/*     ITM VSQ index. Range: 0-355 (Soc_petra),   0-643 (JER2_ARAD) */
typedef uint32 SOC_DNX_ITM_VSQ_NDX;


typedef enum
{
  /*
   *  Ingress credit discount class: Value 0
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_00=0,
  /*
   *  Ingress credit discount class: Value 1
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_01=1,
  /*
   *  Ingress credit discount class: Value 2
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_02=2,
  /*
   *  Ingress credit discount class: Value 3
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_03=3,
  /*
   *  Ingress credit discount class: Value 4
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_04=4,
  /*
   *  Ingress credit discount class: Value 5
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_05=5,
  /*
   *  Ingress credit discount class: Value 6
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_06=6,
  /*
   *  Ingress credit discount class: Value 7
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_07=7,
  /*
   *  Ingress credit discount class: Value 8
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_08=8,
  /*
   *  Ingress credit discount class: Value 9
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_09=9,
  /*
   *  Ingress credit discount class: Value 10
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_10=10,
  /*
   *  Ingress credit discount class: Value 11
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_11=11,
  /*
   *  Ingress credit discount class: Value 12
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_12=12,
  /*
   *  Ingress credit discount class: Value 13
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_13=13,
  /*
   *  Ingress credit discount class: Value 14
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_14=14,
  /*
   *  Ingress credit discount class: Value 15
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_15=15,
  /*
   *  Must be the last value
   */
  SOC_DNX_ITM_NOF_CR_DISCNT_CLS_NDXS = 16
}SOC_DNX_ITM_CR_DISCNT_CLS_NDX;

typedef enum
{
  /*
   *  The size of a single data buffer in the DRAM - 256
   *  bytes.
   *  Note! The maximal allowed packet size is DBUFF_SIZSE * 8.
   */
  SOC_DNX_ITM_DBUFF_SIZE_BYTES_256=256,
  /*
   *  The size of a single data buffer in the DRAM - 512
   *  bytes.
   */
  SOC_DNX_ITM_DBUFF_SIZE_BYTES_512=512,
  /*
   *  The size of a single data buffer in the DRAM - 1024
   *  bytes.
   */
  SOC_DNX_ITM_DBUFF_SIZE_BYTES_1024=1024,
  /*
   *  The size of a single data buffer in the DRAM - 2048
   *  bytes.
   */
  SOC_DNX_ITM_DBUFF_SIZE_BYTES_2048=2048,
  /*
   *  The size of a single data buffer in the DRAM - 4096
   *  bytes.
   */
  SOC_DNX_ITM_DBUFF_SIZE_BYTES_4096=4096,
  /*
   *  Total number of DRAM data buffer sizes.
   */
  SOC_DNX_ITM_NOF_DBUFF_SIZES=4
}SOC_DNX_ITM_DBUFF_SIZE_BYTES;

#define SOC_DNX_ITM_DBUFF_SIZE_BYTES_MIN SOC_DNX_ITM_DBUFF_SIZE_BYTES_256
#define SOC_DNX_ITM_DBUFF_SIZE_BYTES_MAX SOC_DNX_ITM_DBUFF_SIZE_BYTES_2048

#define SOC_DNX_ITM_VSQ_GROUPA_SZE(unit) (dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_CTGRY)->nof)
#define SOC_DNX_ITM_VSQ_GROUPB_SZE(unit) (dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS)->nof)
#define SOC_DNX_ITM_VSQ_GROUPC_SZE(unit) (dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_CTGRY_CNCTN_CLS)->nof)
#define SOC_DNX_ITM_VSQ_GROUPD_SZE(unit) (dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_STTSTCS_TAG)->nof)
#define SOC_DNX_ITM_VSQ_GROUPE_SZE(unit) (dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_SRC_PORT)->nof)
#define SOC_DNX_ITM_VSQ_GROUPF_SZE(unit) (dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_PG)->nof)

#define SOC_DNX_ITM_VSQ_NDX_RNG_LAST(unit) (SOC_DNX_ITM_VSQ_NDX_MAX(unit) + 1)

typedef int SOC_DNX_ITM_VSQ_GROUP_SIZE;
typedef int SOC_DNX_ITM_VSQ_NDX_RNG;
typedef enum
{
  /*
   *  Admission-test-templates 0.
   */
  SOC_DNX_ITM_ADMIT_TST_00=0,
  /*
   *  Admission-test-templates 1.
   */
  SOC_DNX_ITM_ADMIT_TST_01=1,
  /*
   *  Admission-test-templates 2.
   */
  SOC_DNX_ITM_ADMIT_TST_02=2,
  /*
   *  Admission-test-templates 3.
   */
  SOC_DNX_ITM_ADMIT_TST_03=3,
  /*
   *  Must be the last value
   */
  SOC_DNX_ITM_ADMIT_TSTS_LAST
}SOC_DNX_ITM_ADMIT_TSTS;

typedef enum
{
  /*
   *  VSQ group A - category
   */
  SOC_DNX_ITM_VSQ_GROUP_CTGRY=0,
  /*
   *  VSQ group B - category and traffic class
   */
  SOC_DNX_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS=1,
  /*
   *  VSQ group C - category 2/3 and connection class
   */
  SOC_DNX_ITM_VSQ_GROUP_CTGRY_CNCTN_CLS=2,
  /*
   *  VSQ group D - statistics tag
   */
  SOC_DNX_ITM_VSQ_GROUP_STTSTCS_TAG=3,
  /*
   *  VSQ group E - Link Level Flow Control in Arad
   */
   SOC_DNX_ITM_VSQ_GROUP_LLFC = 4,
  /*
   *  VSQ group F - Port Flow Control in Arad
   */
   SOC_DNX_ITM_VSQ_GROUP_PFC = 5,
  /*
   *  VSQ group E - Link Level Flow Control in Jericho
   */
   SOC_DNX_ITM_VSQ_GROUP_SRC_PORT = 4,
  /*
   *  VSQ group F - Port Flow Control in Jericho
   */
   SOC_DNX_ITM_VSQ_GROUP_PG = 5,
  /*
   *  Must be the last value
   */
   SOC_DNX_ITM_VSQ_GROUP_LAST
}SOC_DNX_ITM_VSQ_GROUP;

#define SOC_DNX_NOF_VSQ_GROUPS             SOC_DNX_ITM_VSQ_GROUP_LAST
#define SOC_DNX_NOF_SRC_BASED_VSQ_GROUPS  (SOC_DNX_ITM_VSQ_GROUP_PG - SOC_DNX_ITM_VSQ_GROUP_STTSTCS_TAG)
#define SOC_DNX_NOF_NON_SRC_BASED_VSQ_GROUPS  (SOC_DNX_NOF_VSQ_GROUPS - SOC_DNX_NOF_SRC_BASED_VSQ_GROUPS)
#define SOC_DNX_NOF_VSQ_PG_MAPPING_PROFILES 16
typedef struct
{
  uint32 c2;
  uint32 c3;
  uint32 c1;
  uint32 max_avrg_th;
  uint32 min_avrg_th;
  uint32 vq_wred_pckt_sz_ignr;
  uint32 vq_max_szie_bds_mnt;
  uint32 vq_max_szie_bds_exp;
  uint32 vq_max_size_words_mnt;
  uint32 vq_max_size_words_exp;
} SOC_DNX_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA;

typedef enum
{
  /*
   *  Statistics Tag is not used (disabled).
   */
  SOC_DNX_ITM_STAG_ENABLE_MODE_DISABLED=0,
  /*
   *  The Statistics Tag is not kept in the QDR. This means:
   *  1. VSQs can not be configured based on statistics tag.
   *  2. In the Statistics Interface, the dequeue information
   *  is not available. It still can be used in Billing mode.
   */
  SOC_DNX_ITM_STAG_ENABLE_MODE_STAT_IF_NO_DEQ=1,
  /*
   *  The Statistics Tag is kept in the QDR. This means: 1.
   *  VSQs can be configured based on statistics tag. 2. In
   *  the Statistics Interface, the dequeue information is
   *  available. Note: keeping the Statistics Tag in QDR
   *  consumes QDR resources, which can affect the maximal
   *  traffic bandwidth.
   */
  SOC_DNX_ITM_STAG_ENABLE_MODE_ENABLED_WITH_DEQ=2,
  /*
   *  Total number of STAG enable modes.
   */
  SOC_DNX_ITM_NOF_STAG_ENABLE_MODES=3
}SOC_DNX_ITM_STAG_ENABLE_MODE;

typedef enum {
    SOC_DNX_INGRESS_THRESHOLD_INVALID = -1,
    SOC_DNX_INGRESS_THRESHOLD_TOTAL_BYTES = 0,
    /** SOC_DNX_INGRESS_THRESHOLD_SRAM_BYTES uses the same value as SOC_DNX_INGRESS_THRESHOLD_TOTAL_BYTES as a
     * workaround for not changing the NOF_TYPES which is used on a bounch of places where as SRAM_BYTES is used
     * only on DRAM bound context */
    SOC_DNX_INGRESS_THRESHOLD_SRAM_BYTES = 0,
    SOC_DNX_INGRESS_THRESHOLD_SRAM_BUFFERS,
    SOC_DNX_INGRESS_THRESHOLD_SRAM_PDS,
    SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES
} SOC_DNX_INGRESS_THRESHOLD_TYPE_E;

#define SOC_DNX_INGRESS_THRESHOLD_NOF_SRAM_TYPES    (SOC_DNX_INGRESS_THRESHOLD_SRAM_PDS - SOC_DNX_INGRESS_THRESHOLD_TOTAL_BYTES)


typedef enum {
    SOC_DNX_INGRESS_DRAM_BOUND = 0,
    SOC_DNX_INGRESS_DRAM_BOUND_RECOVERY_FAILURE,
    SOC_DNX_INGRESS_DRAM_BOUND_NOF_TYPES
} SOC_DNX_INGRESS_DRAM_BOUND_TYPE_E;

typedef struct
{
  /*
   *  The size of a single data buffer in the DRAM
   */
  SOC_DNX_ITM_DBUFF_SIZE_BYTES dbuff_size;
  /*
   *  Number of DRAM buffers dedicated to Unicast.
   *  Range: 0 - 2M.
   */
  uint32 uc_nof_buffs;
  /*
   *  Number of DRAM buffers dedicated to Full Multicast.
   *  Range: 0 - 8K.
   */
  uint32 full_mc_nof_buffs;
  /*
   *  Number of DRAM buffers dedicated to Mini Multicast.
   *  Range: 0 - 64K.
   */
  uint32 mini_mc_nof_buffs;
}SOC_DNX_ITM_DRAM_BUFFERS_INFO;

typedef struct
{
  /*
   *  This field consists of the thresholds for setting High
   *  Priority (HP) Flow Control (FC) or clearing HP FC.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO hp;
  /*
   *  This field consists of the thresholds for setting Low
   *  Priority (LP) Flow Control (FC) or clearing LP FC.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO lp;

}SOC_DNX_ITM_GLOB_RCS_FC_TYPE;

typedef struct
{
  /*
   *  These fields consist of the thresholds for setting High
   *  priority (HP) Flow Control (FC), clearing HP FC, setting
   *  Low Priority (LP) FC, or clearing LP FC For the General
   *  resources in the system. This field sets thresholds for
   *  the BDBs resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE bdbs;
  /*
   *  This field sets thresholds for the Unicast Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE unicast;
  /*
   *  This field sets thresholds for the Full Multicast Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE full_mc;
  /*
   *  This field sets thresholds for the Mini Multicast Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE mini_mc;
  /*
   *  This field sets thresholds for the OCB Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE ocb;
  /*
   *  This field sets thresholds for the OCB Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE ocb_p0;
  /*
   *  This field sets thresholds for the OCB Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE ocb_p1;
  /*
   *  This field sets thresholds for the MIX Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE mix_p0;
  /*
   *  This field sets thresholds for the MIX Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE mix_p1;
  /*
   *  This field sets thresholds for the OCB packet descriptor buffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE ocb_pdb;
  /*
   *  This field sets thresholds for the Pool0 Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE p0;
  /*
   *  This field sets thresholds for the Pool1 Dbuffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE p1;
  /*
   *  This field sets thresholds for the Pool0 packet descriptor buffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE p0_pd;
  /*
   *  This field sets thresholds for the Pool1 packet descriptor buffs
   *  resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE p1_pd;
  /*
   *  This field sets thresholds for the Pool0 bytes resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE p0_byte;
  /*
   *  This field sets thresholds for the Pool1 bytes resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE p1_byte;
  /*
   *  This field sets thresholds for the SRAM headroom 
   *  Dbuffs resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE hdrm;
  /*
   *  This field sets thresholds for the SRAM headroom 
   *  packet descriptors resources.
   */
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE hdrm_pd;

}SOC_DNX_ITM_GLOB_RCS_FC_TH;

typedef struct
{
  /*
   *  Thresholds for setting/ clearing drop mode per
   *  drop-precedence, for the BDBs (Buffer Descriptor
   *  Buffers) resources.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO bdbs[SOC_DNX_NOF_DROP_PRECEDENCE];
  SOC_DNX_THRESH_WITH_HYST_INFO ocb_bdbs[SOC_DNX_NOF_DROP_PRECEDENCE];
  /*
   *  Thresholds for setting/ clearing drop mode per
   *  drop-precedence, for the BDs (Buffer Descriptors)
   *  resources.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO bds[SOC_DNX_NOF_DROP_PRECEDENCE];
  SOC_DNX_THRESH_WITH_HYST_INFO ocb_bds[SOC_DNX_NOF_DROP_PRECEDENCE];
  /*
   *  This field sets thresholds for the Unicast Dbuffs
   *  resources.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO unicast[SOC_DNX_NOF_DROP_PRECEDENCE];
  /*
   *  This field sets thresholds for the Full Multicast Dbuffs
   *  resources.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO full_mc[SOC_DNX_NOF_DROP_PRECEDENCE];
  /*
   *  This field sets thresholds for the Mini Multicast Dbuffs
   *  resources.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO mini_mc[SOC_DNX_NOF_DROP_PRECEDENCE];
  /*
   *  This field sets thresholds for the excess DRAM memory
   *  size resources (i.e., without the guaranteed memory
   *  size). Not valid for Soc_petra-A. Units: Bytes. Range: 0 -
   *  128M.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO mem_excess[SOC_DNX_NOF_DROP_PRECEDENCE];
  SOC_DNX_THRESH_WITH_HYST_INFO ocb_mem_excess[SOC_DNX_NOF_DROP_PRECEDENCE];
  /*
   *  This field sets thresholds for the OCB Unicast Dbuffs
   *  resources.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO ocb_uc[SOC_DNX_NOF_DROP_PRECEDENCE];
  /*
   *  This field sets thresholds for the OCB Full Multicast Dbuffs
   *  resources.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO ocb_mc[SOC_DNX_NOF_DROP_PRECEDENCE];

  /*
   *
   *  This field sets thresholds for the reject thresholds based on shared OCB/Dram-mixed resource occupancy of Pool-0/1 by the source based VSQs - per DP.
   *  Valid for OCB-Only/Dram-mixed VSQs.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO ocb_shrd_pool[SOC_DNX_ITM_NOF_RSRC_POOLS][SOC_DNX_NOF_DROP_PRECEDENCE];
  SOC_DNX_THRESH_WITH_HYST_INFO mix_shrd_pool[SOC_DNX_ITM_NOF_RSRC_POOLS][SOC_DNX_NOF_DROP_PRECEDENCE];

  /* 
   * JER2_QAX and up only. All above are not relevant for JER2_QAX.
   */
  /* Set VSQ Shared Occupancy reject thresholds - Per DP */
  SOC_DNX_THRESH_WITH_HYST_INFO global_shrd_pool_0[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES][SOC_DNX_NOF_DROP_PRECEDENCE];
  SOC_DNX_THRESH_WITH_HYST_INFO global_shrd_pool_1[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES][SOC_DNX_NOF_DROP_PRECEDENCE];

  /* Set Global Free SRAM resources reject thresholds for SRAM-PDBs and SRAM-Buffers of regular and SRAM-only VOQs - per DP */
  SOC_DNX_THRESH_WITH_HYST_INFO global_free_sram[SOC_DNX_INGRESS_THRESHOLD_NOF_SRAM_TYPES][SOC_DNX_NOF_DROP_PRECEDENCE];
  SOC_DNX_THRESH_WITH_HYST_INFO global_free_sram_only[SOC_DNX_INGRESS_THRESHOLD_NOF_SRAM_TYPES][SOC_DNX_NOF_DROP_PRECEDENCE];

  /* Set Global Free DRAM-BDBs resource reject thresholds - per DP */
  SOC_DNX_THRESH_WITH_HYST_INFO global_free_dram[SOC_DNX_NOF_DROP_PRECEDENCE];

}SOC_DNX_ITM_GLOB_RCS_DROP_TH;

typedef struct
{
  /* 
   * Used for ref count updates - 
   * When we map a queue to a new credit request profile, 
   * We must update it's ref count  
   */
  /*
   *  Credit Class Identifier. According to the credit class
   *  queue type the credit discount is determined and the
   *  header compensation takes place. Range: 0-15
   */
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX credit_cls;
  /*
   *  Rate Class Identifier. According to the Rate class queue
   *  type the WRED queue features are configured. Range: 0-63
   */
  uint32 rate_cls;
  /*
   *  Connection Class Identifier. Typically this value is in
   *  correlation with the Traffic-Class in the packet header.
   *  (Used For specifying the VSQ to gather statistics)Range:
   *  0-31
   */
  uint32 vsq_connection_cls;
  /*
   *  Traffic Class Identifier. (Used For specifying the VSQ
   *  to gather statistics)Range: 0-7
   */
  uint32 vsq_traffic_cls;

}SOC_DNX_ITM_QUEUE_INFO;

typedef struct
{

    /* Q Instantaneous size */
    uint32 pq_inst_que_size[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];

    /** queue avrg size -- in bytes */
    uint32 pq_avrg_que_size;  

}SOC_DNX_ITM_QUEUE_DYN_INFO;

typedef struct
{
  /*
   *  Category 0 includes ingress queues: 0 to
   *  'vsq_ctgry_end0' (inclusive). Range: 0-32,767.
   */
  uint32 vsq_ctgry0_end;
  /*
   *  Category 1 includes ingress queues: 'vsq_ctgry_end0+1'
   *  to 'vsq_ctgry_end1' (inclusive). Range: 0-32,767.
   */
  uint32 vsq_ctgry1_end;
  /*
   *  Category 2 includes ingress queues: 'vsq_ctgry_end1+1'
   *  to 'vsq_ctgry_end2' (inclusive). Category 3 includes
   *  ingress queues: 'vsq_ctgry_end2+1' to '32K-1'
   *  (inclusive). Range: 0-32,767.
   */
  uint32 vsq_ctgry2_end;
}SOC_DNX_ITM_CATEGORY_RNGS;

typedef struct
{
  /*
   *  Indicator.1 - do category VSQ test 0 - do not do above
   *  test
   */
  uint8 ctgry_test_en;
  /*
   *  Indicator.1 - do "category AND Traffic-Class" VSQ test0
   *  - do not do above test
   */
  uint8 ctgry_trffc_test_en;
  /*
   *  Indicator.1 - do "category2/3 AND Connection-Class" VSQ
   *  test0 - do not do above test
   */
  uint8 ctgry2_3_cnctn_test_en;
  /*
   *  Indicator.1 - do "Statistics TAG" VSQ test0 - do not do
   *  above test
   */
  uint8 sttstcs_tag_test_en;
  /*
   *  Indicator.1 - do "PFC" VSQ test0 - do not do
   *  above test
   */ 
  uint8 pfc_test_en;
  /*
   *  Indicator.1 - do "LLFC" VSQ test0 - do not do
   *  above test
   */ 
  uint8 llfc_test_en;
}SOC_DNX_ITM_ADMIT_ONE_TEST_TMPLT;

typedef struct
{
  /*
   *  Packet accepted if testA or testB pass. RjctTmplt[0] -
   *  Category-VSQ testRjctTmplt[1] -
   *  Category&Connection-class VSQ testRjctTmplt[2] -
   *  Category2/3 & Class test VSQ testRjctTmplt[3] -
   *  Statistic TAG VSQ test
   */
  SOC_DNX_ITM_ADMIT_ONE_TEST_TMPLT test_a;
  /*
   *  Packet accepted if testA or testB pass.(same tmplt
   *  structure as testA).
   */
  SOC_DNX_ITM_ADMIT_ONE_TEST_TMPLT test_b;
}SOC_DNX_ITM_ADMIT_TEST_TMPLT_INFO;


typedef struct
{
  /*
   *  AKA header compensation. When a packet is sent and a
   *  credit is deducted from the queue's credit balance,
   *  NP/external header (or any other size) can be discounted
   *  from result, in order not to affect overall output rate
   *  (note that this discount is signed, which means it can
   *  be added to the credit balance). Range: +/- 0 to 127.
   *  Resolution: 1 Byte.
   */
  int32 discount;
}SOC_DNX_ITM_CR_DISCOUNT_INFO;

typedef struct
{
  /*
   *  Indicator:
   *  1 - enable WRED test
   *  0 - disable test (always accept)
   */
  uint8 wred_en;
  /*
   *  Exponential queue size averaging weight. Range:
   *  0-31. I.e., make the average factor from 1 to 2^(-31).
   */
  uint32 exp_wq;
}SOC_DNX_ITM_VSQ_WRED_GEN_INFO;

typedef struct
{
  /*
   *  Indicator:
   *  1 - enable WRED test
   *  0 - disable test
   *  For VSQ WRED, this indication is ignored.
   *  To enable VSQ WRED, use 'soc_jer2_tmcitm_vsq_wred_gen_set' API.
   */
  uint8 wred_en;
  /*
   *  Indicator:1 - discard packet size in WRED test0 -
   *  consider packet size in WRED test
   */
  uint8 ignore_packet_size;
  /*
   *  Relating to queue average size. Minimum Average Threshold
   *  - Below this threshold, packet is admitted into the
   *  queue. Resolution of bytes. The device resolution is of 16
   *  bytes. Has to be lower than max_avrg_th. For Soc_petra-A,
   *  Range: 0 - 2^24-1. For Soc_petra-B, Range: 0 - 2^31
   *  for Arad queues, 0 - 2^32-1 for Arad VSQs.
   */
  uint32 min_avrg_th;
  /*
   *  Relating to queue average size. Maximum Average Threshold
   *  - Above this threshold, packet is discarded from the
   *  queue. Resolution of bytes. The device resolution is of 16
   *  bytes. Has to be higher than max_avrg_th. For Soc_petra-A,
   *  Range: 0 - 2^24-1. For Soc_petra-B, Range: 0 - 2^31
   *  for Arad queues, 0 - 2^32-1 for Arad VSQs.
   */
  uint32 max_avrg_th;
  /*
   *  Max packet size for the WRED algorithm. In the WRED
   *  algorithm, the device granularity is of power of 2
   *  packet sizes. Between 2^0 to 2^15E.g., 127 bytes will be
   *  round up to 128 bytes. Note - this size should not
   *  exceed WRED_MAX_PACKET_SIZE. Range: 0 - 16K -
   *  128. Resolution: 1 Byte.
   */
  uint32 max_packet_size;
  /*
   *  The maximum probability of discarding a packet (when the
   *  queue reaches the maximum average size ('max_avrg_th')
   *  and the packet size is the maximum size
   *  ('max_packet_size')). 1% = 1, 23% = 23. Range: 0 - 100.
   */
  uint32 max_probability;
}SOC_DNX_ITM_WRED_QT_DP_INFO;

typedef struct
{
  /*
   *  If (instantaneous-queue-size >
   *  maximum-instantaneous-queue-size) then the packet is
   *  discarded.
   *  Range: 0 - 256MB. Units: Bytes.
   *  Actual resolution: 16 Bytes.
   */
  uint32 max_inst_q_size;
}SOC_DNX_ITM_TAIL_DROP_INFO;

typedef struct
{
    uint32 max_threshold[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];

    uint32 min_threshold[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];

    int32 adjust_factor[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];

} SOC_DNX_ITM_FADT_INFO;

typedef struct
{
    uint32 max_threshold;
    uint32 min_threshold;
    int alpha;

    uint32 free_max_threshold;
    uint32 free_min_threshold;

} SOC_DNX_ITM_DRAM_BOUND_THRESHOLD;



typedef struct 
{
    /*
      we don't use array for sram buffers/sram pds to avoid index shitfing 
       -- SOC_DNX_INGRESS_THRESHOLD_SRAM_BYTES defined as 1 and indexes should start at 0 
    */
    SOC_DNX_ITM_DRAM_BOUND_THRESHOLD sram_buffers_dram_threshold[SOC_DNX_INGRESS_DRAM_BOUND_NOF_TYPES];
    SOC_DNX_ITM_DRAM_BOUND_THRESHOLD sram_pds_dram_threshold[SOC_DNX_INGRESS_DRAM_BOUND_NOF_TYPES];
    SOC_DNX_ITM_DRAM_BOUND_THRESHOLD sram_bytes_dram_threshold[SOC_DNX_INGRESS_DRAM_BOUND_NOF_TYPES];

    uint32 qsize_recovery_th;

} SOC_DNX_ITM_DRAM_BOUND_INFO;


/* This value given to a set function will ignore the field */
#define SOC_DNX_ITM_GUARANTEED_INFO_DO_NOT_SET ((uint32)-1)

typedef struct
{
  /*  the guaranteed queue size -- per threshold type */
  uint32 guaranteed_size[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];

} SOC_DNX_ITM_GUARANTEED_INFO;

typedef struct soc_dnx_guaranteed_pair_s {
    uint32  total;/* book keeping of the total guaranteed resource for VOQs */
    uint32  used; /* book keeping of the used guaranteed resource for VOQs */
} soc_dnx_guaranteed_pair_t;


typedef struct  {
    soc_dnx_guaranteed_pair_t guaranteed[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_DNX_ITM_GUARANTEED_RESOURCE;


typedef struct
{
  /*
   *  Hysteresis Thresholds, giving a threshold over which the
   *  flow control of the virtual queue is asserted and a
   *  threshold below which the flow control is deasserted.
   *  Value is compared to instantaneous size. For Soc_petra-A,
   *  Range: 0 - 2^24-1. For Soc_petra-B, Range: 0 - 2^28-1.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO q_size_fc;
  /*
   *  Hysteresis Thresholds, giving a threshold over which the
   *  flow control of the virtual queue is asserted and a
   *  threshold below which flow control is deasserted. Value
   *  is compared VSQ in consumed BDs size. Range: 0 - 2^22-1.
   */
  SOC_DNX_THRESH_WITH_HYST_INFO bd_size_fc;
  /*
   *  JER2_QAX and up only.
   *  Hysteresis Thresholds, giving a threshold over which the
   *  flow control of the virtual queue is asserted and a
   *  threshold below which flow control is deasserted.   
   */
  SOC_DNX_THRESH_WITH_HYST_INFO size_fc[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
  /*
   *  FADT Thresholds, giving a dynamic threshold over which the
   *  flow control of the virtual queue is asserted and a
   *  threshold below which flow control is deasserted.   
   *  JER2_QAX: Relevant only for VSQ-F.
   */
  SOC_DNX_THRESH_WITH_FADT_HYST_INFO fadt_size_fc[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
}SOC_DNX_ITM_VSQ_FC_INFO;

typedef struct
{
  /* JER2_QAX and up */
  /* Taildrop per resource type (words, sram-words, sram-pds) */
  uint32 max_inst_q_size_th[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
}SOC_DNX_ITM_VSQ_TAIL_DROP_INFO;

typedef struct
{
  /* Field of maximum FADT threshold */
  soc_field_t max_field;
  /* Field of minimum FADT threshold */
  soc_field_t min_field;
  /* Field of FADT alpha parameter */
  soc_field_t alpha_field;
} SOC_DNX_ITM_VSQ_FADT_FIELDS_INFO;

typedef struct
{
    /* Max guaranteed */
    uint32 max_guaranteed[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
    /* Max shared */
    uint32 max_shared[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
    /* Max headroom */
    uint32 max_headroom[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_DNX_ITM_VSQ_SRC_PORT_INFO;

typedef struct
{
    /* Max headroom */
    uint32 max_headroom;
    /* SRAM only: Max headroom nominal */
    uint32 max_headroom_nominal;
    /* SRAM only: Max headroom extension */
    uint32 max_headroom_extension;
} SOC_DNX_ITM_VSQ_PG_HDRM_INFO;

typedef struct
{
    /* Max guaranteed */
    uint32 max_guaranteed[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
    /* Max shared - FADT */
    SOC_DNX_FADT_INFO max_shared[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
    /* Max headroom */
    SOC_DNX_ITM_VSQ_PG_HDRM_INFO max_headroom[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_DNX_ITM_VSQ_PG_INFO;


typedef struct
{
  /* 
   * 
   */
  uint32 sign;
  /*
   *  FADT Flow control adjust factor.
   *     If FadtDropExp[3] is set
   *      - FADT-Fc-Set-TH = Free-Shared-Resource <<FadtFcExp[2:-0]
   *     Else,
   *     - FADT-Fc-Set-TH = Free-Shared-Resource >> FadtFcExp[2:-0]
   */
  uint32 exp;
  /* 
   * FADT Flow control clear adjust factor.
   *   If FadtDropExp[3] is set
   *     - FADT-Fc-Clr-TH = Free-Shared-Resource <<FadtFcExp[2:-0] - FadtFcOffset
   *   Else,
   *     - FADT-Fc-Clr-TH = Free-Shared-Resource >> FadtFcExp[2:-0] - FadtFcOffset 
   */
  uint32 offset;
  /* 
   * If VSQ-Size smaller than FadtFcFloor, than flow control indication is cleared.
   */
  uint32 floor;
} SOC_DNX_ITM_VSQ_FADT_FC_INFO;

typedef struct
{
  /*Define whether the PG belongs to Pool-0 or Pool-1.*/
  uint8 pool_id;
  /*If set, PG is lossles (Can use Headroom and mask some of the admit tests)*/
  uint8 is_lossles;
  /* 
   * If set, PG will use the VSQ-Port guaranteed area (not dedicated guaranteed per PG). 
   * Otherwise, PG will have its own guaranteed area 
   */ 
  uint8 use_min_port;
  /* 
   *  FADT Flow control adjust factor, and FADT Flow control clear adjust factor.
   */
  SOC_DNX_ITM_VSQ_FADT_FC_INFO fadt_fc;
  /* JER2_QAX */
  /*
   * Used for admission logic masks.
   */
  uint8 admit_profile;

} SOC_DNX_ITM_VSQ_PG_PRM;

typedef struct
{
  /*
   *  Enable (in the specified mode)/Disable STAG
   */
  SOC_DNX_ITM_STAG_ENABLE_MODE enable_mode;
  /*
   *  If available for VSQ, the MSB of the VSQ field in the
   *  statistics Tag. The value in the field between the MSB
   *  and LSB defines a VSQ number that is used by the STE,
   *  when gathering statistics information. (VSQ group D).
   *  Note: a total of 256 VSQs are supported using the
   *  Stat-tag (8 bits). Range: 0 - 31.
   */
  uint32 vsq_index_msb;
  /*
   *  If available for VSQ, the LSB of the VSQ field in the
   *  statistics Tag. Range: 0 - 31.
   */
  uint32 vsq_index_lsb;
  /*
   *  If set, then the Statistics-Tag carries Drop Precedence
   *  (DP) field. In this case, the packet DP value is
   *  retrieved from the statistics tag, and not from the DP
   *  field in the ITMH. Note: this configuration must be
   *  consistent with per-port configuration of the STAG
   *  Generation.
   */
  uint8 dropp_en;
  /*
   *  Defines the LSB of the DP in the STAG, if enabled.
   *  Range: 0 - 30.
   */
  uint32 dropp_lsb;

}SOC_DNX_ITM_STAG_INFO;

typedef struct
{
  /*
   *  Ingress Shaping defines a continuous range of VOQs that
   *  used for Ingress shaped packets. First queue ID for
   *  ingress shaping queues The lower value. Note: Base-Q can
   *  not be part of the range. Range: 0 - 32K-1.
   */
  uint32 q_num_low;
  /*
   *  Last queue ID for ingress shaping queues. Note: Base-Q
   *  can not be part of the range. Range: 0 - 32K-1. For Soc_petra-B: 0 - 32K-2.
   */
  uint32 q_num_high;

}SOC_DNX_ITM_INGRESS_SHAPE_Q_RANGE;

typedef struct
{
  /*
   *  If TRUE, ingress shaping is enabled. Note: If disabled,
   *  all the other fields in this structure are ignored.
   */
  uint8 enable;
  /*
   *  Ingress shaping queues range (low, high).
   */
  SOC_DNX_ITM_INGRESS_SHAPE_Q_RANGE q_range;
  /*
   *  Ingress Shaping rate. Units: Kbps.
   */
  uint32 rate;
  /*
   *  End-to-End Scheduler Port, dedicated to ingress shaping.
   *  Range: 0-79. Note: the scheduling hierarchy under this
   *  port distributes credits to Ingress Shaping queues. The
   *  ingress shaping queues must be mapped to scheduler flows
   *  that are part of scheduling hierarchy under this port.
   */
  uint32 sch_port;

}SOC_DNX_ITM_INGRESS_SHAPE_INFO;

typedef struct
{
  /*
   *  Describes one segment of 64 contiguous queues. If bit
   *  value is 1: high priority. If bit value is 0: low
   *  priority. Note that map[0] is the LSBs, and that
   *  map[SOC_DNX_ITM_PRIO_CONST_MAP_UINT32_SIZE -1] is the
   *  MSBs. Some examples, assuming that
   *  [SOC_DNX_ITM_PRIO_CONST_MAP_UINT32_SIZE is 2:Segment 2 is
   *  map[0] & 1 << 2Segment 40 is map[1] & 1 << 8
   */
  uint32 map[SOC_DNX_ITM_PRIO_MAP_SIZE_IN_UINT32S];

}SOC_DNX_ITM_PRIORITY_MAP_TMPLT;

typedef struct
{
  /*
   *  System Red drop probability values. Range: 0 - 10000.
   *  (100 means 1%).
   */
  uint32 drop_probs[SOC_DNX_ITM_SYS_RED_DRP_PROBS];

}SOC_DNX_ITM_SYS_RED_DROP_PROB;

typedef struct
{
  /*
   *  Indicator:1 - Enable System Red. 0 - System RED is
   *  disabled (test and parameters ignored).
   */
  uint8 enable;
  /*
   *  Admit Threshold: Threshold below which packets are
   *  admitted. Range: 0-15.
   */
  uint32 adm_th;
  /*
   *  Above adm_th and below prob_th drop with probability
   *  drp_prob_low. Range: 0-15.
   */
  uint32 prob_th;
  /*
   *  Drop threshold: above this threshold packet is always
   *  dropped. Above prob_th and below drp_th drop with
   *  probability drp_prob_high. Range: 0-15.
   */
  uint32 drp_th;
  /*
   *  Drop probability index, used when Q-size range is
   *  between adm_th and prob_th (index to drop-p
   *  table). Range: 0-15.
   */
  uint32 drp_prob_low;
  /*
   *  Drop probability index, used when Q-size range is
   *  between prob_th and drp_th (index to drop-p
   *  table). Range: 0-15.
   */
  uint32 drp_prob_high;

}SOC_DNX_ITM_SYS_RED_QT_DP_INFO;

typedef struct
{
  /*
   *  Boundaries which map queue size to system red
   *  Virtual-OFP-Queue-Size index. Virtual-OFP-Queue-Size 0 is
   *  mapped by 0 - queue_size_boundaries[0].
   *  Virtual-OFP-Queue-Size 1 is mapped by queue_size_boundaries[0] -
   *  queue_size_boundaries[1].
   *  Virtual-OFP-Queue-Size 15 is mapped by queue_size_boundaries[14] -
   *  max_q_size.
   *  Range (for each entry): 0 - 256MB. Units: Bytes.
   *  Actual resolution: 16 Byte.
   */
  uint32 queue_size_boundaries[SOC_DNX_ITM_SYS_RED_Q_SIZE_RANGES];

}SOC_DNX_ITM_SYS_RED_QT_INFO;

typedef struct
{
  /*
   *  Indicator:
   *  TRUE - System RED is enabled in Scheduler.
   *  FALSE - System RED is disabled in Scheduler.
   *  Note: if disabled the System Red mechanism in the SCH is shut,
   *  meaning that the Max-Q-Size that is sent from the scheduler in
   *  the mechanism is always 0 and therefore packets are
   *  always admitted if when referring to this test.
   */
  uint8 enable;
  /*
   *  Unit in millisecond. Every "aging_timer" the aging
   *  mechanism accesses the queue size saved for a port. If a
   *  message has been received with an updated queue size
   *  since the last time the aging mechanism accessed the
   *  port this mechanism continues on to the next port.
   *  Otherwise it changes the queue size according to
   *  'reset_expired_q_size'. Values of 0 or 1 means do not
   *  decrement the queue size values.
   *  Range: 0 - 2717 (for CLK=250M or 4ns).
   */
  uint32 aging_timer;
  /*
   *  Indicator:
   *  1 - The 'aging timer' mechanism resets the
   *  queue size.
   *  0 - The 'aging timer' mechanism decrements
   *  it by one.
   */
  uint8 reset_expired_q_size;
  /*
   *  Indicator:1 - Only aging decrements the System-RED queue
   *  size (a smaller value from the same flow does not affect
   *  the Max-Q-size).0 - Flow status message and aging can
   *  decrement System-RED queue size.
   */
  uint8 aging_only_dec_q_size;
}SOC_DNX_ITM_SYS_RED_EG_INFO;

typedef struct
{
  /*
   *  Free Unicast Data buffers Thresholds, used for
   *  configuring thresholds of consumed resources. Free Data
   *  buffers Threshold Define value. Value is used to generate
   *  general-source-Q-size (4 bit). If free-dbuff > dbuff_th0
   *  -> Qsize=Val0. If free-dbuff between dbuff_th0 and
   *  dbuff_th1 > Qsize=Val1. If free -dbuff between dbuff_th2
   *  and dbuff_th1 > Qsize=Val2If free-Uni-dbuff- below
   *  dbuff_th2, then Qsize =Val3 Range: 0 - 0xFFFFF.
   */
  uint32 unicast_rng_ths[SOC_DNX_ITM_SYS_RED_BUFFS_RNGS - 1];
  /*
   *  Free Multicast Data buffers Thresholds. For Soc_petra-A,
   *  Range: 0 - 0x1FFF. For Soc_petra-B, Range: 0 - 0xFFFF.
   */
  uint32 multicast_rng_ths[SOC_DNX_ITM_SYS_RED_BUFFS_RNGS - 1];
  /*
   *  Free BD buffers Thresholds. Range: 0 - 0x7FFF.
   */
  uint32 bds_rng_ths[SOC_DNX_ITM_SYS_RED_BUFFS_RNGS - 1];
  /*
   *  OCB buffers Thresholds. Range: 0 - 0x7FFF.
   */
  uint32 ocb_rng_ths[SOC_DNX_ITM_SYS_RED_BUFFS_RNGS - 1];

}SOC_DNX_ITM_SYS_RED_GLOB_RCS_THS;

typedef struct
{
  /*
   *  Free Unicast Data buffers range values, used for
   *  configuring values of the ranges of the consumed
   *  resources. Define respective source-Q-sizes values for
   *  each range of the Free Data buffers. Source-Q-size value
   *  assigned to Range-0/1/2/3. Value = [0..15].
   */
  uint32 unicast_rng_vals[SOC_DNX_ITM_SYS_RED_BUFFS_RNGS];
  /*
   *  Free Multicast Data buffers range valuesValue = [0..15].
   */
  uint32 multicast_rng_vals[SOC_DNX_ITM_SYS_RED_BUFFS_RNGS];
  /*
   *  Free BD buffers range values. Value = [0..15].
   */
  uint32 bds_rng_vals[SOC_DNX_ITM_SYS_RED_BUFFS_RNGS];
  /*
   *  OCB buffers range values. Value = [0..15].
   */
  uint32 ocb_rng_vals[SOC_DNX_ITM_SYS_RED_BUFFS_RNGS];
}SOC_DNX_ITM_SYS_RED_GLOB_RCS_VALS;

typedef struct
{
  /*
   *  The consumed resources are divided into 4 blocks, for
   *  each category (Unicast Data Buffers, Multicast Data
   *  Buffer and BD buffers range). Each block is assigned
   *  with a value, which is compared to the 4bit
   *  representative of the Queue Size in the System Red test,
   *  the maximum between the two is sent to the egress.
   */
  SOC_DNX_ITM_SYS_RED_GLOB_RCS_THS thresholds;
  /*
   *  The value that is assigned to each block.
   */
  SOC_DNX_ITM_SYS_RED_GLOB_RCS_VALS values;
}SOC_DNX_ITM_SYS_RED_GLOB_RCS_INFO;

typedef struct 
{

  /*
   * Mapping between old TC to new TC.
   * Array index is old TC
   * Array value is new TC
   */
  uint8 new_tc[SOC_DNX_NOF_TRAFFIC_CLASSES];
}SOC_DNX_ITM_TC_MAPPING;

typedef struct
{
    /* Pool 0 size */
    uint32 pool_0;
    /* Pool 1 size */
    uint32 pool_1;
    /* Headroom size */
    uint32 headroom;
    /* Nominal Headroom size */
    uint32 nominal_headroom;
    /* Reserved/guarantee size */
    uint32 reserved;
    /* Maximal size (total - 1) */
    uint32 max;
} SOC_DNX_ITM_INGRESS_CONGESTION_RESOURCE;

typedef struct
{
    SOC_DNX_ITM_INGRESS_CONGESTION_RESOURCE global[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_DNX_ITM_INGRESS_CONGESTION_MGMT;

typedef struct
{
    /* Number of reserved resources */
    uint32 reserved[SOC_DNX_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_DNX_ITM_INGRESS_RESERVED_RESOURCE;

/* 
 * Ingress congested resources types
 */
typedef enum
{
    SOC_DNX_ITM_CGM_RSRC_STAT_MIN_FREE_BDB = 0,
    SOC_DNX_ITM_CGM_RSRC_STAT_MIN_FREE_SRAM_BUFFERS,
    SOC_DNX_ITM_CGM_RSRC_STAT_MIN_FREE_SRAM_PDB
} SOC_DNX_ITM_CGM_RSRC_STAT_TYPE;


typedef struct
{
    soc_gport_t dest_gport; 
    int cosq;
    uint64 latency;  
    uint32 latency_flow;
}SOC_DNX_MAX_LATENCY_PACKETS_INFO;

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

int  dnx_tmc_itm_mantissa_exp_field_set(
    int                  unit,
    soc_mem_t mem_id,
    soc_field_t field_id,
    int mantissa_bits,
    int exp_bits,
    int factor,
    int round_up,
    void *data,
    uint32 threshold, 
    uint32* result_threshold
        );

void  dnx_tmc_itm_mantissa_exp_field_get(
    int                  unit,
    soc_mem_t mem_id,
    soc_field_t field_id,
    int mantissa_bits,
    int exp_bits,
    int factor,
    void *data,
    uint32* result_threshold
        );

void
  SOC_DNX_ITM_DRAM_BUFFERS_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_DRAM_BUFFERS_INFO *info
  );

void
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_clear(
    DNX_SAND_OUT SOC_DNX_ITM_GLOB_RCS_FC_TYPE *info
  );

void
  SOC_DNX_ITM_GLOB_RCS_FC_TH_clear(
    DNX_SAND_OUT SOC_DNX_ITM_GLOB_RCS_FC_TH *info
  );

void
  SOC_DNX_ITM_GLOB_RCS_DROP_TH_clear(
    DNX_SAND_OUT SOC_DNX_ITM_GLOB_RCS_DROP_TH *info
  );

void
  SOC_DNX_ITM_QUEUE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_QUEUE_INFO *info
  );

void
  SOC_DNX_ITM_CATEGORY_RNGS_clear(
    DNX_SAND_OUT SOC_DNX_ITM_CATEGORY_RNGS *info
  );

void
  SOC_DNX_ITM_ADMIT_ONE_TEST_TMPLT_clear(
    DNX_SAND_OUT SOC_DNX_ITM_ADMIT_ONE_TEST_TMPLT *info
  );

void
  SOC_DNX_ITM_ADMIT_TEST_TMPLT_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

void
  SOC_DNX_ITM_CR_DISCOUNT_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_CR_DISCOUNT_INFO *info
  );

void
  SOC_DNX_ITM_WRED_QT_DP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_WRED_QT_DP_INFO *info
  );

void
  SOC_DNX_ITM_TAIL_DROP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_TAIL_DROP_INFO *info
  );

void
  SOC_DNX_ITM_FADT_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_FADT_INFO *info
  );

void
  SOC_DNX_ITM_DRAM_BOUND_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_DRAM_BOUND_INFO *info
  );

SOC_DNX_ITM_DRAM_BOUND_THRESHOLD* SOC_DNX_ITM_DRAM_BOUND_INFO_thresh_get(
    int                 unit,
    SOC_DNX_ITM_DRAM_BOUND_INFO* info,
    SOC_DNX_INGRESS_DRAM_BOUND_TYPE_E dram_thresh,
    SOC_DNX_INGRESS_THRESHOLD_TYPE_E resource_type);

void
SOC_DNX_ITM_VSQ_PG_PRM_clear (
   SOC_DNX_ITM_VSQ_PG_PRM *info
);
void
  SOC_DNX_ITM_VSQ_SRC_PORT_INFO_clear (
     SOC_DNX_ITM_VSQ_SRC_PORT_INFO  *info
  );
void
  SOC_DNX_ITM_VSQ_PG_INFO_clear (
     SOC_DNX_ITM_VSQ_PG_INFO  *info
  );

void
  SOC_DNX_ITM_VSQ_FC_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_VSQ_FC_INFO *info
  );

void
  SOC_DNX_ITM_VSQ_TAIL_DROP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_VSQ_TAIL_DROP_INFO *info
  );

void
  SOC_DNX_ITM_VSQ_WRED_GEN_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_VSQ_WRED_GEN_INFO *info
  );

void
  SOC_DNX_ITM_STAG_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_STAG_INFO *info
  );

void
  SOC_DNX_ITM_INGRESS_SHAPE_Q_RANGE_clear(
    DNX_SAND_OUT SOC_DNX_ITM_INGRESS_SHAPE_Q_RANGE *info
  );

void
  SOC_DNX_ITM_INGRESS_SHAPE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_INGRESS_SHAPE_INFO *info
  );

void
  SOC_DNX_ITM_PRIORITY_MAP_TMPLT_clear(
    DNX_SAND_OUT SOC_DNX_ITM_PRIORITY_MAP_TMPLT *info
  );

void
  SOC_DNX_ITM_SYS_RED_DROP_PROB_clear(
    DNX_SAND_OUT SOC_DNX_ITM_SYS_RED_DROP_PROB *info
  );

void
  SOC_DNX_ITM_SYS_RED_QT_DP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_SYS_RED_QT_DP_INFO *info
  );

void
  SOC_DNX_ITM_SYS_RED_QT_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_SYS_RED_QT_INFO *info
  );

void
  SOC_DNX_ITM_SYS_RED_EG_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_SYS_RED_EG_INFO *info
  );

void
  SOC_DNX_ITM_SYS_RED_GLOB_RCS_THS_clear(
    DNX_SAND_OUT SOC_DNX_ITM_SYS_RED_GLOB_RCS_THS *info
  );

void
  SOC_DNX_ITM_SYS_RED_GLOB_RCS_VALS_clear(
    DNX_SAND_OUT SOC_DNX_ITM_SYS_RED_GLOB_RCS_VALS *info
  );

void
  SOC_DNX_ITM_SYS_RED_GLOB_RCS_INFO_clear(
    DNX_SAND_OUT SOC_DNX_ITM_SYS_RED_GLOB_RCS_INFO *info
  );
void
  SOC_DNX_ITM_TC_MAPPING_clear(
    DNX_SAND_OUT SOC_DNX_ITM_TC_MAPPING *info
  );

const char*
  SOC_DNX_ITM_CR_DISCNT_CLS_NDX_to_string(
    DNX_SAND_IN  SOC_DNX_ITM_CR_DISCNT_CLS_NDX enum_val
  );

const char*
  SOC_DNX_ITM_DBUFF_SIZE_BYTES_to_string(
    DNX_SAND_IN SOC_DNX_ITM_DBUFF_SIZE_BYTES enum_val
  );

const char*
  SOC_DNX_ITM_VSQ_NDX_RNG_to_string(
     DNX_SAND_IN  int unit,
     DNX_SAND_IN SOC_DNX_ITM_VSQ_NDX_RNG enum_val
  );

const char*
  SOC_DNX_ITM_ADMIT_TSTS_to_string(
    DNX_SAND_IN SOC_DNX_ITM_ADMIT_TSTS enum_val
  );

const char*
  SOC_DNX_ITM_VSQ_GROUP_to_string(
    DNX_SAND_IN SOC_DNX_ITM_VSQ_GROUP enum_val
  );

void
  SOC_DNX_ITM_DRAM_BUFFERS_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_DRAM_BUFFERS_INFO *info
  );

void
  SOC_DNX_ITM_GLOB_RCS_FC_TYPE_print(
    DNX_SAND_IN SOC_DNX_ITM_GLOB_RCS_FC_TYPE *info
  );

void
  SOC_DNX_ITM_GLOB_RCS_FC_TH_print(
    DNX_SAND_IN SOC_DNX_ITM_GLOB_RCS_FC_TH *info
  );

void
  SOC_DNX_ITM_GLOB_RCS_DROP_TH_print_no_table(
    DNX_SAND_IN SOC_DNX_ITM_GLOB_RCS_DROP_TH *info
  );

void
  SOC_DNX_ITM_GLOB_RCS_DROP_TH_print(
    DNX_SAND_IN SOC_DNX_ITM_GLOB_RCS_DROP_TH *info
  );

void
  SOC_DNX_ITM_QUEUE_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_QUEUE_INFO *info
  );

void
  SOC_DNX_ITM_CATEGORY_RNGS_print(
    DNX_SAND_IN SOC_DNX_ITM_CATEGORY_RNGS *info
  );

void
  SOC_DNX_ITM_ADMIT_ONE_TEST_TMPLT_print(
    DNX_SAND_IN SOC_DNX_ITM_ADMIT_ONE_TEST_TMPLT *info
  );

void
  SOC_DNX_ITM_ADMIT_TEST_TMPLT_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

void
  SOC_DNX_ITM_CR_DISCOUNT_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_CR_DISCOUNT_INFO *info
  );

void
  SOC_DNX_ITM_WRED_QT_DP_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_WRED_QT_DP_INFO *info
  );

void
  SOC_DNX_ITM_TAIL_DROP_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_TAIL_DROP_INFO *info
  );

void
  SOC_DNX_ITM_VSQ_FC_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_VSQ_FC_INFO *info
  );

void
  SOC_DNX_ITM_VSQ_TAIL_DROP_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_VSQ_TAIL_DROP_INFO *info
  );

void
  SOC_DNX_ITM_VSQ_WRED_GEN_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_VSQ_WRED_GEN_INFO *info
  );

void
  SOC_DNX_ITM_STAG_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_STAG_INFO *info
  );

void
  SOC_DNX_ITM_INGRESS_SHAPE_Q_RANGE_print(
    DNX_SAND_IN SOC_DNX_ITM_INGRESS_SHAPE_Q_RANGE *info
  );

void
  SOC_DNX_ITM_INGRESS_SHAPE_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_INGRESS_SHAPE_INFO *info
  );

void
  SOC_DNX_ITM_PRIORITY_MAP_TMPLT_print(
    DNX_SAND_IN SOC_DNX_ITM_PRIORITY_MAP_TMPLT *info
  );

void
  SOC_DNX_ITM_SYS_RED_DROP_PROB_print(
    DNX_SAND_IN SOC_DNX_ITM_SYS_RED_DROP_PROB *info
  );

void
  SOC_DNX_ITM_SYS_RED_QT_DP_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_SYS_RED_QT_DP_INFO *info
  );

void
  SOC_DNX_ITM_SYS_RED_QT_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_SYS_RED_QT_INFO *info
  );

void
  SOC_DNX_ITM_SYS_RED_EG_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_SYS_RED_EG_INFO *info
  );

void
  SOC_DNX_ITM_SYS_RED_GLOB_RCS_THS_print(
    DNX_SAND_IN SOC_DNX_ITM_SYS_RED_GLOB_RCS_THS *info
  );

void
  SOC_DNX_ITM_SYS_RED_GLOB_RCS_VALS_print(
    DNX_SAND_IN SOC_DNX_ITM_SYS_RED_GLOB_RCS_VALS *info
  );

void
  SOC_DNX_ITM_SYS_RED_GLOB_RCS_INFO_print(
    DNX_SAND_IN SOC_DNX_ITM_SYS_RED_GLOB_RCS_INFO *info
  );


/* } */

/*
 * Ingress compensation source types
*/
typedef enum
{
    SOC_DNX_PKT_SIZE_ADJUST_SRC_SCHEDULER,
    SOC_DNX_PKT_SIZE_ADJUST_SRC_CRPS_IQM_IRPP,
    SOC_DNX_PKT_SIZE_ADJUST_SRC_CRPS_IQM,
    SOC_DNX_PKT_SIZE_ADJUST_SRC_STAT_REPOR_IN,
    SOC_DNX_PKT_SIZE_ADJUST_SRC_STAT_REPOR_OUT
} SOC_DNX_COMPENSATION_PKT_SIZE_SRC_TYPE;


/* } __SOC_DNX_API_INGRESS_TRAFFIC_MGMT_INCLUDED__*/
#endif
