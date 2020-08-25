/* $Id: jer2_arad_ingress_traffic_mgmt.h,v 1.16 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifndef __JER2_ARAD_INGRESS_TRAFFIC_MGMT_INCLUDED__
/* { */
#define __JER2_ARAD_INGRESS_TRAFFIC_MGMT_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/dnx/legacy/ARAD/arad_general.h>
#include <soc/dnx/legacy/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

  /* Maximum ITM-Connection-Class: Value 31   */
#define  JER2_ARAD_ITM_QT_CC_CLS_MAX              31

  /* Maximum ITM-Credit-Class: Value 15  */
#define  JER2_ARAD_ITM_QT_CR_CLS_MAX              (JER2_ARAD_ITM_NOF_CR_DISCNT_CLS_NDXS - 1)

  /*  Maximum ITM-Rate-Class: Value 63 */
#define  JER2_ARAD_ITM_QT_RT_CLS_MAX              63

/* Maximum ITM-WRED exponent maximal value */
#define JER2_ARAD_ITM_WQ_MAX                      31

/*  Maximum Discard-DP: Value 4 */
#define  JER2_ARAD_ITM_DISCARD_DP_MAX              4

#define JER2_ARAD_ITM_GRNT_BYTES_MAX                                  (256*1024*1024)
#define JER2_ARAD_ITM_GRNT_BDS_MAX                                    (2*1024*1024)
#define JER2_ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED                        0xffffff
#define JER2_ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS                   8
#define JER2_ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS                   4
#define JER2_ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX               (128*1024*1024)

/*     Total number of DRAM buffers. DRAM buffers are used to
*     store packets at the ingress. DRAM buffers are shared
*     between Multicast, Mini-multicast and Unicast buffers.  */
#define  JER2_ARAD_ITM_NOF_DRAM_BUFFS (SOC_DNX_ITM_NOF_DRAM_BUFFS)

/*     Maximal number of DRAM buffers used for Full Multicast. */
#define  JER2_ARAD_ITM_DBUFF_FMC_MAX (64 * 1024)

/*     Maximal number of DRAM buffers used for Mini
*     Multicast. Mini Multicast buffers are used for snooping
*     and mirroring, or for Multicast where four or less
*     copies are required.                                    */
#define  JER2_ARAD_ITM_DBUFF_MMC_MAX (SOC_DNX_ITM_DBUFF_MMC_MAX)

/*     Number of DRAM buffers IDR can save in cache (without DRAM use).. */
#define JER2_ARAD_ITM_DBUFF_CACHE 3500

#define JER2_ARAD_ITM_NOF_VSQS 356

/*
* Definition values for Ingress Queue Priority
*/

#define JER2_ARAD_ITM_PRIO_MAP_BIT_SIZE       64
#define JER2_ARAD_ITM_PRIO_MAP_SIZE_IN_UINT32S  2
#ifdef FIXME_DNX_LEGACY /** SOC_DNX_DEFS not supported */
#define JER2_ARAD_ITM_PRIO_NOF_SEGMENTS(unit)       DNX_SAND_DIV_ROUND_UP(SOC_DNX_DEFS_GET(unit, nof_queues), JER2_ARAD_ITM_PRIO_MAP_BIT_SIZE)
#endif
/*
 * Drop Tail Resolution 16 bytes
 */
#define JER2_ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION 16

/*
 *  All buffers may be used for Unicast
 */
#define  JER2_ARAD_ITM_DBUFF_UC_MAX JER2_ARAD_ITM_NOF_DRAM_BUFFS

/*
* Definition values for System Red
*/
#define JER2_ARAD_ITM_SYS_RED_DROP_P_VAL      100
#define JER2_ARAD_ITM_SYS_RED_Q_SIZE_RANGES   16
#define JER2_ARAD_ITM_SYS_RED_DRP_PROBS       16
#define JER2_ARAD_ITM_SYS_RED_BUFFS_RNGS      4

  /* Maximum ITM-VSQ-Rate-Class: Value 15  */
#define JER2_ARAD_ITM_VSQ_QT_RT_CLS_MAX  SOC_DNX_ITM_VSQ_QT_RT_CLS_MAX(unit)

#define JER2_ARAD_ITM_RATE_CLASS_MAX    SOC_DNX_ITM_RATE_CLASS_MAX
#define JER2_ARAD_NOF_VSQ_GROUPS        SOC_DNX_NOF_VSQ_GROUPS

#define JER2_ARAD_ITM_VSQ_GROUP_CTGRY_SIZE                       SOC_DNX_ITM_VSQ_GROUPA_SZE(unit) /*4*/
#define JER2_ARAD_ITM_VSQ_GROUP_CTGRY_OFFSET                     JER2_ARAD_ITM_VSQ_GROUP_CTGRY_SIZE
#define JER2_ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_SIZE           SOC_DNX_ITM_VSQ_GROUPB_SZE(unit) /*((JER2_ARAD_NOF_TRAFFIC_CLASSES) * JER2_ARAD_ITM_VSQ_GROUP_CTGRY_SIZE)*/
#define JER2_ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_OFFSET         (JER2_ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_SIZE + JER2_ARAD_ITM_VSQ_GROUP_CTGRY_OFFSET)
#define JER2_ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_SIZE         SOC_DNX_ITM_VSQ_GROUPC_SZE(unit) /*((JER2_ARAD_ITM_QT_CC_CLS_MAX+1)*2)*/
#define JER2_ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_OFFSET       (JER2_ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_SIZE + JER2_ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_OFFSET)
#define JER2_ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_SIZE                 SOC_DNX_ITM_VSQ_GROUPD_SZE(unit)
#define JER2_ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_OFFSET               (JER2_ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_SIZE + JER2_ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_OFFSET)
#define JER2_ARAD_ITM_VSQ_GROUP_LLFC_SIZE                        SOC_DNX_ITM_VSQ_GROUPE_SZE(unit)
#define JER2_ARAD_ITM_VSQ_GROUP_LLFC_OFFSET                      (JER2_ARAD_ITM_VSQ_GROUP_LLFC_SIZE + JER2_ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_OFFSET)
#define JER2_ARAD_ITM_VSQ_GROUP_PFC_SIZE                         SOC_DNX_ITM_VSQ_GROUPF_SZE(unit)
#define JER2_ARAD_ITM_VSQ_GROUP_PFC_OFFSET                       (JER2_ARAD_ITM_VSQ_GROUP_PFC_SIZE + JER2_ARAD_ITM_VSQ_GROUP_LLFC_OFFSET)

#define JER2_ARAD_ITM_VSQ_GROUP_LAST          SOC_DNX_ITM_VSQ_GROUP_LAST
#define JER2_ARAD_ITM_ADMIT_TSTS_LAST         SOC_DNX_ITM_ADMIT_TSTS_LAST

#define JER2_ARAD_ITM_DBUFF_SIZE_BYTES_MIN        SOC_DNX_ITM_DBUFF_SIZE_BYTES_MIN
#define JER2_ARAD_ITM_DBUFF_SIZE_BYTES_MAX        SOC_DNX_ITM_DBUFF_SIZE_BYTES_MAX

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

#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_00                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_00
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_01                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_01
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_02                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_02
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_03                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_03
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_04                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_04
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_05                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_05
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_06                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_06
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_07                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_07
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_08                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_08
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_09                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_09
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_10                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_10
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_11                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_11
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_12                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_12
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_13                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_13
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_14                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_14
#define JER2_ARAD_ITM_CR_DISCNT_CLS_NDX_15                     SOC_DNX_ITM_CR_DISCNT_CLS_NDX_15
#define JER2_ARAD_ITM_NOF_CR_DISCNT_CLS_NDXS                   SOC_DNX_ITM_NOF_CR_DISCNT_CLS_NDXS
typedef SOC_DNX_ITM_CR_DISCNT_CLS_NDX                          JER2_ARAD_ITM_CR_DISCNT_CLS_NDX;

#define JER2_ARAD_ITM_DBUFF_SIZE_BYTES_256                     SOC_DNX_ITM_DBUFF_SIZE_BYTES_256
#define JER2_ARAD_ITM_DBUFF_SIZE_BYTES_512                     SOC_DNX_ITM_DBUFF_SIZE_BYTES_512
#define JER2_ARAD_ITM_DBUFF_SIZE_BYTES_1024                    SOC_DNX_ITM_DBUFF_SIZE_BYTES_1024
#define JER2_ARAD_ITM_DBUFF_SIZE_BYTES_2048                    SOC_DNX_ITM_DBUFF_SIZE_BYTES_2048
#define JER2_ARAD_ITM_DBUFF_SIZE_BYTES_4096                    SOC_DNX_ITM_DBUFF_SIZE_BYTES_4096
#define JER2_ARAD_ITM_NOF_DBUFF_SIZES                          SOC_DNX_ITM_NOF_DBUFF_SIZES
typedef SOC_DNX_ITM_DBUFF_SIZE_BYTES                           JER2_ARAD_ITM_DBUFF_SIZE_BYTES;

#define JER2_ARAD_ITM_VSQ_NDX_MIN(unit)                              SOC_DNX_ITM_VSQ_NDX_MIN(unit)
#define JER2_ARAD_ITM_VSQ_NDX_MAX(unit)                              SOC_DNX_ITM_VSQ_NDX_MAX_JER2_ARAD(unit)
typedef SOC_DNX_ITM_VSQ_NDX_RNG                               JER2_ARAD_ITM_VSQ_NDX_RNG;

#define JER2_ARAD_ITM_ADMIT_TST_00                             SOC_DNX_ITM_ADMIT_TST_00
#define JER2_ARAD_ITM_ADMIT_TST_01                             SOC_DNX_ITM_ADMIT_TST_01
#define JER2_ARAD_ITM_ADMIT_TST_02                             SOC_DNX_ITM_ADMIT_TST_02
#define JER2_ARAD_ITM_ADMIT_TST_03                             SOC_DNX_ITM_ADMIT_TST_03
typedef SOC_DNX_ITM_ADMIT_TSTS                                 JER2_ARAD_ITM_ADMIT_TSTS;


#define JER2_ARAD_ITM_VSQ_GROUP_CTGRY                          SOC_DNX_ITM_VSQ_GROUP_CTGRY
#define JER2_ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS              SOC_DNX_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS
#define JER2_ARAD_ITM_VSQ_GROUP_CTGRY_CNCTN_CLS                SOC_DNX_ITM_VSQ_GROUP_CTGRY_CNCTN_CLS
#define JER2_ARAD_ITM_VSQ_GROUP_STTSTCS_TAG                    SOC_DNX_ITM_VSQ_GROUP_STTSTCS_TAG
#define JER2_ARAD_ITM_VSQ_GROUP_LLFC                           SOC_DNX_ITM_VSQ_GROUP_LLFC
#define JER2_ARAD_ITM_VSQ_GROUP_PFC                            SOC_DNX_ITM_VSQ_GROUP_PFC
#define JER2_ARAD_ITM_VSQ_GROUP_SRC_PORT                       SOC_DNX_ITM_VSQ_GROUP_SRC_PORT
#define JER2_ARAD_ITM_VSQ_GROUP_PG                             SOC_DNX_ITM_VSQ_GROUP_PG

typedef SOC_DNX_ITM_VSQ_GROUP                                  JER2_ARAD_ITM_VSQ_GROUP;

typedef SOC_DNX_ITM_DRAM_BUFFERS_INFO                          JER2_ARAD_ITM_DRAM_BUFFERS_INFO;
typedef SOC_DNX_ITM_GLOB_RCS_FC_TYPE                           JER2_ARAD_ITM_GLOB_RCS_FC_TYPE;
typedef SOC_DNX_ITM_GLOB_RCS_FC_TH                             JER2_ARAD_ITM_GLOB_RCS_FC_TH;
typedef SOC_DNX_ITM_GLOB_RCS_DROP_TH                           JER2_ARAD_ITM_GLOB_RCS_DROP_TH;
typedef SOC_DNX_ITM_QUEUE_INFO                                 JER2_ARAD_ITM_QUEUE_INFO;
typedef SOC_DNX_ITM_QUEUE_DYN_INFO                             JER2_ARAD_ITM_QUEUE_DYN_INFO;
typedef SOC_DNX_ITM_CATEGORY_RNGS                              JER2_ARAD_ITM_CATEGORY_RNGS;
typedef SOC_DNX_ITM_ADMIT_ONE_TEST_TMPLT                       JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT;
typedef SOC_DNX_ITM_ADMIT_TEST_TMPLT_INFO                      JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO;
typedef SOC_DNX_ITM_CR_DISCOUNT_INFO                           JER2_ARAD_ITM_CR_DISCOUNT_INFO;
typedef SOC_DNX_ITM_WRED_QT_DP_INFO                            JER2_ARAD_ITM_WRED_QT_DP_INFO;
typedef SOC_DNX_ITM_TAIL_DROP_INFO                             JER2_ARAD_ITM_TAIL_DROP_INFO;
typedef SOC_DNX_ITM_VSQ_FC_INFO                                JER2_ARAD_ITM_VSQ_FC_INFO;
typedef SOC_DNX_ITM_VSQ_TAIL_DROP_INFO                         JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO;
typedef SOC_DNX_ITM_VSQ_WRED_GEN_INFO                          JER2_ARAD_ITM_VSQ_WRED_GEN_INFO;
typedef SOC_DNX_ITM_INGRESS_SHAPE_Q_RANGE                      JER2_ARAD_ITM_INGRESS_SHAPE_Q_RANGE;
typedef SOC_DNX_ITM_INGRESS_SHAPE_INFO                         JER2_ARAD_ITM_INGRESS_SHAPE_INFO;
typedef SOC_DNX_ITM_PRIORITY_MAP_TMPLT                         JER2_ARAD_ITM_PRIORITY_MAP_TMPLT;
typedef SOC_DNX_ITM_SYS_RED_DROP_PROB                          JER2_ARAD_ITM_SYS_RED_DROP_PROB;
typedef SOC_DNX_ITM_SYS_RED_QT_DP_INFO                         JER2_ARAD_ITM_SYS_RED_QT_DP_INFO;
typedef SOC_DNX_ITM_SYS_RED_QT_INFO                            JER2_ARAD_ITM_SYS_RED_QT_INFO;
typedef SOC_DNX_ITM_SYS_RED_EG_INFO                            JER2_ARAD_ITM_SYS_RED_EG_INFO;
typedef SOC_DNX_ITM_SYS_RED_GLOB_RCS_THS                       JER2_ARAD_ITM_SYS_RED_GLOB_RCS_THS;
typedef SOC_DNX_ITM_SYS_RED_GLOB_RCS_VALS                      JER2_ARAD_ITM_SYS_RED_GLOB_RCS_VALS;
typedef SOC_DNX_ITM_SYS_RED_GLOB_RCS_INFO                      JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO;

typedef SOC_DNX_ITM_VSQ_NDX                                    JER2_ARAD_ITM_VSQ_NDX;

typedef enum{
  /*
   *  Statistics Tag is not used (disabled).
   */
  JER2_ARAD_ITM_STAG_ENABLE_MODE_DISABLED=0,
  /*
   *  The Statistics Tag is not kept in the QDR. This means:
   *  1. VSQs can not be configured based on statistics tag.
   *  2. In the Statistics Interface, the dequeue information
   *  is not available. It still can be used in Billing mode.
   */
  JER2_ARAD_ITM_STAG_ENABLE_MODE_STAT_IF_NO_DEQ=1,
  /*
   *  The Statistics Tag is kept in the QDR. This means: 1.
   *  VSQs can be configured based on statistics tag. 2. In
   *  the Statistics Interface, the dequeue information is
   *  available. Note: keeping the Statistics Tag in QDR
   *  consumes QDR resources, which can affect the maximal
   *  traffic bandwidth.
   */
  JER2_ARAD_ITM_STAG_ENABLE_MODE_ENABLED_WITH_DEQ=2,
  /*
   *  Total number of STAG enable modes.
   */
  JER2_ARAD_ITM_NOF_STAG_ENABLE_MODES=3
}JER2_ARAD_ITM_STAG_ENABLE_MODE;

typedef struct
{
  /*
   *  Enable (in the specified mode)/Disable STAG
   */
  JER2_ARAD_ITM_STAG_ENABLE_MODE enable_mode;
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

}JER2_ARAD_ITM_STAG_INFO;


typedef enum
{
  JER2_ARAD_ITM_FWD_ACTION_TYPE_FWD = 0,
  JER2_ARAD_ITM_FWD_ACTION_TYPE_SNOOP = 1,
  JER2_ARAD_ITM_FWD_ACTION_TYPE_INBND_MIRR = 2,
  JER2_ARAD_ITM_FWD_ACTION_TYPE_OUTBND_MIRR = 3,
  JER2_ARAD_ITM_NOF_FWD_ACTION_TYPES = 4
}JER2_ARAD_ITM_FWD_ACTION_TYPE;

typedef struct soc_dnx_guaranteed_q_resource_s {
    soc_dnx_guaranteed_pair_t dram;
    soc_dnx_guaranteed_pair_t ocb;
} soc_dnx_guaranteed_q_resource_t;

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

/*********************************************************************
* NAME:
*     jer2_arad_ingress_traffic_mgmt_init
* FUNCTION:
*     Initialization of the Arad blocks configured in this module.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  jer2_arad_ingress_traffic_mgmt_init(
    DNX_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_dram_buffs_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     DRAM buffers are used to store packets at the ingress.
*     This is a resource shared between Unicast,
*     Full-Multicast and Mini-Multicast packets. There are 2M
*     buffers available. This function sets the buffers share
*     dedicated for Unicast, Full-Multicast and Mini-Multicast
*     packets. This function also sets the size of a single
*     buffer. See remarks below for limitations.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs -
*     DRAM buffers configuration - size and distribution.
* REMARKS:
*     1. Total number of DRAM buffers for Unicast,
*     Full-Multicast and Mini-Multicast packets must not
*     exceed 2M. 2. Total number of DRAM buffers, multiplied
*     by buffer size, must not exceed total available DRAM
*     size.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  jer2_arad_itm_dram_buffs_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_dram_buffs_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     DRAM buffers are used to store packets at the ingress.
*     This is a resource shared between Unicast,
*     Full-Multicast and Mini-Multicast packets. There are 2M
*     buffers available. This function sets the buffers share
*     dedicated for Unicast, Full-Multicast and Mini-Multicast
*     packets. This function also sets the size of a single
*     buffer. See remarks below for limitations.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT JER2_ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs -
*     DRAM buffers configuration - size and distribution.
* REMARKS:
*     1. Total number of DRAM buffers for Unicast,
*     Full-Multicast and Mini-Multicast packets must not
*     exceed 2M. 2. Total number of DRAM buffers, multiplied
*     by buffer size, must not exceed total available DRAM
*     size.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_dram_buffs_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT JER2_ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_glob_rcs_fc_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets the Thresholds to trigger/clear the Flow Control
*     Indication. For the different kinds of general resources
*     (bds, unicast, multicast).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_GLOB_RCS_FC_TH  *info -
*     The thresholds for setting/clearing Flow Control High
*     Priority and Low Priority.
 *   DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_FC_TH                  *exact_info -
 *     Pointer to the exact set info.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_glob_rcs_fc_set(
    DNX_SAND_IN   int                 unit,
    DNX_SAND_IN   JER2_ARAD_ITM_GLOB_RCS_FC_TH  *info,
    DNX_SAND_OUT  JER2_ARAD_ITM_GLOB_RCS_FC_TH  *exact_info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_glob_rcs_fc_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets the Thresholds to trigger/clear the Flow Control
*     Indication. For the different kinds of general resources
*     (bds, unicast, multicast).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_GLOB_RCS_FC_TH  *info -
*     The thresholds for setting/clearing Flow Control High
*     Priority and Low Priority.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_glob_rcs_fc_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_GLOB_RCS_FC_TH  *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_glob_rcs_fc_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets the Thresholds to trigger/clear the Flow Control
*     Indication. For the different kinds of general resources
*     (bds, unicast, multicast).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_FC_TH  *info -
*     The thresholds for setting/clearing Flow Control High
*     Priority and Low Priority.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_glob_rcs_fc_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_FC_TH  *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_glob_rcs_drop_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets the Thresholds to trigger/clear the Drop mechanism,
*     in which packets are dropped if the buffers of the
*     different kinds have passed their hysteresis thresholds.
*     For the different kinds of general resources (bds,
*     unicast, full-multicast, mini-multicast).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info -
*     The thresholds for setting/clearing the Drop mechanism.
 *   DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_DROP_TH                *exact_info -
 *     The exact values for the thresholds to the Drop
 *     mechanism.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_glob_rcs_drop_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info,
    DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_DROP_TH *exact_info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_glob_rcs_drop_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets the Thresholds to trigger/clear the Drop mechanism,
*     in which packets are dropped if the buffers of the
*     different kinds have passed their hysteresis thresholds.
*     For the different kinds of general resources (bds,
*     unicast, full-multicast, mini-multicast).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info -
*     The thresholds for setting/clearing the Drop mechanism.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_glob_rcs_drop_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_glob_rcs_drop_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets the Thresholds to trigger/clear the Drop mechanism,
*     in which packets are dropped if the buffers of the
*     different kinds have passed their hysteresis thresholds.
*     For the different kinds of general resources (bds,
*     unicast, full-multicast, mini-multicast).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info -
*     The thresholds for setting/clearing the Drop mechanism.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_glob_rcs_drop_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_category_rngs_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Defines packet queues categories - in contiguous blocks.
*     IQM queues are divided to 4 categories in contiguous
*     blocks. Category-4 from 'category-end-3' till the last
*     queue (32K).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_CATEGORY_RNGS *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_category_rngs_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_CATEGORY_RNGS *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_category_rngs_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Defines packet queues categories - in contiguous blocks.
*     IQM queues are divided to 4 categories in contiguous
*     blocks. Category-4 from 'category-end-3' till the last
*     queue (32K).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_CATEGORY_RNGS *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_category_rngs_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_CATEGORY_RNGS *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_category_rngs_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Defines packet queues categories - in contiguous blocks.
*     IQM queues are divided to 4 categories in contiguous
*     blocks. Category-4 from 'category-end-3' till the last
*     queue (32K).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT JER2_ARAD_ITM_CATEGORY_RNGS *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_category_rngs_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT JER2_ARAD_ITM_CATEGORY_RNGS *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_admit_test_tmplt_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 admt_tst_ndx -
*     There are 4 possible VSQ admission tests. Range 0 to 3.
*     With this procedure, one can set the tests.
*  DNX_SAND_IN  JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_admit_test_tmplt_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 admt_tst_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_admit_test_tmplt_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 admt_tst_ndx -
*     There are 4 possible VSQ admission tests. Range 0 to 3.
*     With this procedure, one can set the tests.
*  DNX_SAND_IN  JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_admit_test_tmplt_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 admt_tst_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_admit_test_tmplt_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 admt_tst_ndx -
*     There are 4 possible VSQ admission tests. Range 0 to 3.
*     With this procedure, one can set the tests.
*  DNX_SAND_OUT JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_admit_test_tmplt_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 admt_tst_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_cr_discount_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     The device has 16 possible credit-discount
*     possibilities. This procedure sets the 16 options.
*     The Credit Discount value should be calculated as following:
*     Credit-Discount =
*     -(IPG (20B)+ CRC (size of CRC field only if it is not removed by NP)) +
*     Dune_H (size of FTMH + FTMH extension (if exists)) +
*     NP_H (size of Network Processor Header, or Dune PP Header) + DRAM CRC size.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx -
*     Per queue user can select 1 credit-discount class out of
*     16 different credit-discount classes.
*  DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCOUNT_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_cr_discount_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCOUNT_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_cr_discount_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     There are 16 possible credit-discount classes.
*     Each Credit Class is configured with a value that
*     is to be added/subtracted from the credit counter at each
*     dequeue of packet. This procedure sets the 16
*     credit-discount values per credit class.
*     The Credit Discount value should be calculated as following:
*     Credit-Discount =
*     -IPG (20B)+ CRC (size of CRC field only if it is not removed by NP) +
*     NP_H (size of Network Processor Header) + Dune_H (size of ITMH+FTMH).
*     Note that this functionality will take affect only when working with
*     small packet sizes.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx -
*     Per queue user can select 1 credit-discount class out of
*     16 different credit-discount classes.
*  DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCOUNT_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_cr_discount_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCOUNT_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_cr_discount_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     There are 16 possible credit-discount classes.
*     Each Credit Class is configured with a value that
*     is to be added/subtracted from the credit counter at each
*     dequeue of packet. This procedure sets the 16
*     credit-discount values per credit class.
*     The Credit Discount value should be calculated as following:
*     Credit-Discount =
*     -(IPG (20B)+ CRC (size of CRC field only if it is not removed by NP)) +
*     Dune_H (size of FTMH + FTMH extension (if exists)) +
*     NP_H (size of Network Processor Header, or Dune PP Header) + DRAM CRC size.
*     Note that this functionality will take affect only when working with
*     small packet sizes.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx -
*     Per queue user can select 1 credit-discount class out of
*     16 different credit-discount classes.
*  DNX_SAND_OUT JER2_ARAD_ITM_CR_DISCOUNT_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_cr_discount_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_CR_DISCOUNT_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_queue_test_tmplt_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Each queue (VOQ) is assigned with a test template.
*     This Function sets the admit logic test of the queue
*     per rate-class and drop-precedence (there are 4
*     pre-configured by 'jer2_arad_itm_admit_test_tmplt_set'
*     options for test types).
*     Notice that in a queue, is a packet is chosen to be
*     rejected normally, the admit test logic will not affect it.
*     For more information about the admit test template refer to
*     the description of 'jer2_arad_itm_admit_test_tmplt_set'.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3.
*  DNX_SAND_IN  JER2_ARAD_ITM_ADMIT_TSTS      test_tmplt -
*     Enumerator indicating the test-template index.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_queue_test_tmplt_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_ADMIT_TSTS      test_tmplt
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_queue_test_tmplt_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Each queue (VOQ) is assigned with a test template.
*     This Function sets the admit logic test of the queue
*     per rate-class and drop-precedence (there are 4
*     pre-configured by 'jer2_arad_itm_admit_test_tmplt_set'
*     options for test types).
*     Notice that in a queue, is a packet is chosen to be
*     rejected normally, the admit test logic will not affect it.
*     For more information about the admit test template refer to
*     the description of 'jer2_arad_itm_admit_test_tmplt_set'.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3.
*  DNX_SAND_IN  JER2_ARAD_ITM_ADMIT_TSTS      test_tmplt -
*     Enumerator indicating the test-template index.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_queue_test_tmplt_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_ADMIT_TSTS      test_tmplt
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_queue_test_tmplt_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Each queue (VOQ) is assigned with a test template.
*     This Function sets the admit logic test of the queue
*     per rate-class and drop-precedence (there are 4
*     pre-configured by 'jer2_arad_itm_admit_test_tmplt_set'
*     options for test types).
*     Notice that in a queue, is a packet is chosen to be
*     rejected normally, the admit test logic will not affect it.
*     For more information about the admit test template refer to
*     the description of 'jer2_arad_itm_admit_test_tmplt_set'.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3.
*  DNX_SAND_OUT JER2_ARAD_ITM_ADMIT_TSTS      *test_tmplt -
*     Enumerator indicating the test-template index.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_queue_test_tmplt_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_ADMIT_TSTS      *test_tmplt
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_wred_exp_wq_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets exponential weighted queue per rate-class. The
*     calculation the device does: Average_constant =
*     2^(-RED_exp_weight)if (Instantaneous-Queue-size <
*     Average-queue-size) Average-queue-size =
*     Instantaneous-Queue-sizeelse Average-queue-size =
*     (1-Average_constant)*Average-queue-size +
*     Average_constant*Instantaneous-Queue-size To configure
*     WRED configuration which are per queue-type and dp, use
*     the functionjer2_arad_itm_wred_info_set.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                  exp_wq -
*     Constant for average queue size calculation. Range:
*     0-31. I.e., make the average factor from 1 to 2^(-31).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_wred_exp_wq_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                  exp_wq,
    DNX_SAND_IN  uint8                   enable
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_wred_exp_wq_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets exponential weighted queue per rate-class. The
*     calculation the device does: Average_constant =
*     2^(-RED_exp_weight)if (Instantaneous-Queue-size <
*     Average-queue-size) Average-queue-size =
*     Instantaneous-Queue-sizeelse Average-queue-size =
*     (1-Average_constant)*Average-queue-size +
*     Average_constant*Instantaneous-Queue-size To configure
*     WRED configuration which are per queue-type and dp, use
*     the functionjer2_arad_itm_wred_info_set.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                  exp_wq -
*     Constant for average queue size calculation. Range:
*     0-31. I.e., make the average factor from 1 to 2^(-31).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_wred_exp_wq_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                  exp_wq
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_wred_exp_wq_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets exponential weighted queue per rate-class. The
*     calculation the device does: Average_constant =
*     2^(-RED_exp_weight)if (Instantaneous-Queue-size <
*     Average-queue-size) Average-queue-size =
*     Instantaneous-Queue-sizeelse Average-queue-size =
*     (1-Average_constant)*Average-queue-size +
*     Average_constant*Instantaneous-Queue-size To configure
*     WRED configuration which are per queue-type and dp, use
*     the functionjer2_arad_itm_wred_info_set.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_OUT uint32                  *exp_wq -
*     Constant for average queue size calculation. Range:
*     0-31. I.e., make the average factor from 1 to 2^(-31).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_wred_exp_wq_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_OUT uint32                  *exp_wq
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_wred_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets WRED parameters per rate-class and drop precedence,
*     including wred-enable and the admit test logic. Also, as
*     part of the WRED parameters; max-queue,
*     WRED-thresholds/probability. To configure WRED
*     Configuration that is per queue-type only (exponential
*     weight queue), use the function
*     jer2_arad_itm_wred_exp_wq_set.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3, or SOC_DNX_NOF_DROP_PRECEDENCE which means ECN and not a real drop precedence.
*  DNX_SAND_IN  JER2_ARAD_ITM_WRED_QT_DP_INFO *info -
*     pointer to configuration structure.
*  DNX_SAND_OUT JER2_ARAD_ITM_WRED_QT_DP_INFO *exact_info -
*     Loaded with the actual WRED parameters (difference due
*     to rounding). Note: if 'min_avrg_th' & 'max_avrg_th'
*     fields are the same, no real WRED is activates. Since
*     below all are admitted and above all are discarded
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_wred_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_WRED_QT_DP_INFO *info,
    DNX_SAND_OUT JER2_ARAD_ITM_WRED_QT_DP_INFO *exact_info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_wred_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets WRED parameters per rate-class and drop precedence,
*     including wred-enable and the admit test logic. Also, as
*     part of the WRED parameters; max-queue,
*     WRED-thresholds/probability. To configure WRED
*     Configuration that is per queue-type only (exponential
*     weight queue), use the function
*     jer2_arad_itm_wred_exp_wq_set.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3, or SOC_DNX_NOF_DROP_PRECEDENCE which means ECN and not a real drop precedence.
*  DNX_SAND_IN  JER2_ARAD_ITM_WRED_QT_DP_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_wred_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_WRED_QT_DP_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_wred_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets WRED parameters per rate-class and drop precedence,
*     including wred-enable and the admit test logic. Also, as
*     part of the WRED parameters; max-queue,
*     WRED-thresholds/probability. To configure WRED
*     Configuration that is per queue-type only (exponential
*     weight queue), use the function
*     jer2_arad_itm_wred_exp_wq_set.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3, or SOC_DNX_NOF_DROP_PRECEDENCE which means ECN and not a real drop precedence.
*  DNX_SAND_OUT JER2_ARAD_ITM_WRED_QT_DP_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_wred_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_WRED_QT_DP_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_tail_drop_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets tail drop parameter - max-queue-size per rate-class
*     and drop precedence. The tail drop mechanism drops
*     packets that are mapped to queues that exceed thresholds
*     of this structure.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3, or SOC_DNX_NOF_DROP_PRECEDENCE which means ECN and not a real drop precedence.
*  DNX_SAND_IN  JER2_ARAD_ITM_TAIL_DROP_INFO  *info -
*     pointer to configuration structure.
*  DNX_SAND_OUT JER2_ARAD_ITM_TAIL_DROP_INFO  *exact_info -
*     pointer to returned exact data.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_tail_drop_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_TAIL_DROP_INFO  *info,
    DNX_SAND_OUT JER2_ARAD_ITM_TAIL_DROP_INFO  *exact_info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_tail_drop_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets tail drop parameter - max-queue-size per rate-class
*     and drop precedence. The tail drop mechanism drops
*     packets that are mapped to queues that exceed thresholds
*     of this structure.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3, or SOC_DNX_NOF_DROP_PRECEDENCE which means ECN and not a real drop precedence.
*  DNX_SAND_IN  JER2_ARAD_ITM_TAIL_DROP_INFO  *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_tail_drop_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_TAIL_DROP_INFO  *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_tail_drop_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets tail drop parameter - max-queue-size per rate-class
*     and drop precedence. The tail drop mechanism drops
*     packets that are mapped to queues that exceed thresholds
*     of this structure.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3, or SOC_DNX_NOF_DROP_PRECEDENCE which means ECN and not a real drop precedence.
*  DNX_SAND_OUT JER2_ARAD_ITM_TAIL_DROP_INFO  *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_tail_drop_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_TAIL_DROP_INFO  *info
  );


/*********************************************************************
*     Set ECN as enabled or disabled for the device
*********************************************************************/
int
  jer2_arad_itm_enable_ecn(
    DNX_SAND_IN  int   unit,
    DNX_SAND_IN  uint32   enabled /* ECN will be enabled/disabled for non zero/zero values */
  );

/*********************************************************************
*     Return if ECN is enabled for the device
*********************************************************************/
int
  jer2_arad_itm_get_ecn_enabled(
    DNX_SAND_IN  int   unit,
    DNX_SAND_OUT uint32   *enabled /* will return non zero if /ECN is enabled */
  );


/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_qt_rt_cls_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Each Virtual Statistics Queue has a VSQ-Rate-Class.
*     This function assigns a VSQ with its Rate Class.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx -
*     Vsq in group index to which to configure rate class. the
*     index shoud be according the group it is, as following:
*     vsq_group 0 (category): 0-3 vsq_group 1 (cat + traffic
*     class): 0-32 vsq_group 2 (cat2/3 + connection class):
*     0-64 vsq_group 3 (statistics tag): 0-255
*  DNX_SAND_IN  uint32                 vsq_rt_cls -
*     Vsq rate class to configure, range: 0-15.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_qt_rt_cls_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  int                 core_id,
    DNX_SAND_IN  uint8               is_ocb_only,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_qt_rt_cls_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets Virtual Statistics Queue Rate-Class.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx -
*     Vsq in group index to which to configure rate class. the
*     index shoud be according the group it is, as following:
*     vsq_group 0 (category): 0-3 vsq_group 1 (cat + traffic
*     class): 0-32 vsq_group 2 (cat2/3 + connection class):
*     0-64 vsq_group 3 (statistics tag): 0-255
*  DNX_SAND_IN  uint32                 vsq_rt_cls -
*     Vsq rate class to configure, range: 0-15.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_qt_rt_cls_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  int                 core_id,
    DNX_SAND_IN  uint8               is_ocb_only,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_qt_rt_cls_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets Virtual Statistics Queue Rate-Class.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx -
*     Vsq in group index to which to configure rate class. the
*     index shoud be according the group it is, as following:
*     vsq_group 0 (category): 0-3 vsq_group 1 (cat + traffic
*     class): 0-32 vsq_group 2 (cat2/3 + connection class):
*     0-64 vsq_group 3 (statistics tag): 0-255
*  DNX_SAND_OUT uint32                 *vsq_rt_cls -
*     Vsq rate class to configure, range: 0-15.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_qt_rt_cls_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  int                 core_id,
    DNX_SAND_IN  uint8               is_ocb_only,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    DNX_SAND_OUT uint32                 *vsq_rt_cls
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_fc_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets Virtual Statistics Queue, includes: vsq-id,
*     rate-class
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     VSQ rate class index. Range: 0-63
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_FC_INFO     *info -
*     pointer to configuration structure.
*  DNX_SAND_OUT JER2_ARAD_ITM_VSQ_FC_INFO     *exact_info -
*     May vary due to rounding.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_fc_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_FC_INFO     *info,
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_FC_INFO     *exact_info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_fc_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets Virtual Statistics Queue, includes: vsq-id,
*     rate-class
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     VSQ rate class index. Range: 0-63
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_FC_INFO     *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_fc_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_FC_INFO     *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_fc_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets Virtual Statistics Queue, includes: vsq-id,
*     rate-class
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     VSQ rate class index. Range: 0-63
*  DNX_SAND_OUT JER2_ARAD_ITM_VSQ_FC_INFO     *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_fc_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_FC_INFO     *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_tail_drop_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets tail drop parameter on the VSQ - max-queue-size in
*     words and in buffer-descriptors per vsq-rate-class and
*     drop precedence. The tail drop mechanism drops packets
*     that are mapped to queues that exceed thresholds of this
*     structure.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     Ingress VSQ group index. Range: 0 to 3.
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     Ingress rate class. Range: 0 to 15.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *info -
*     pointer to configuration structure.
*  DNX_SAND_OUT JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *exact_info -
*     Pointer to returned exact data.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_tail_drop_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *info,
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *exact_info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_tail_drop_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets tail drop parameter on the VSQ - max-queue-size in
*     words and in buffer-descriptors per vsq-rate-class and
*     drop precedence. The tail drop mechanism drops packets
*     that are mapped to queues that exceed thresholds of this
*     structure.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     Ingress VSQ group index. Range: 0 to 3.
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     Ingress rate class. Range: 0 to 15.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_tail_drop_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_tail_drop_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Sets tail drop parameter on the VSQ - max-queue-size in
*     words and in buffer-descriptors per vsq-rate-class and
*     drop precedence. The tail drop mechanism drops packets
*     that are mapped to queues that exceed thresholds of this
*     structure.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     Ingress VSQ group index. Range: 0 to 3.
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     Ingress rate class. Range: 0 to 15.
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     The drop precedence to set. Range: 0-3.
*  DNX_SAND_OUT JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_tail_drop_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  );

/********************************************************************* 
* NAME:
*     jer2_arad_itm_vsq_tail_drop_get_default 
*     Get tail drop default parameters on the VSQ - max-queue-size in
*     words and in buffer-descriptors per vsq-rate-class.
*     The tail drop mechanism drops packets
*     that are mapped to queues that exceed thresholds of this
*     structure.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *info -
*     pointer to configuration structure.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_tail_drop_get_default(
      DNX_SAND_IN  int                 unit,
      DNX_SAND_OUT JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_wred_gen_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This procedure sets VSQ WRED general configurations,
*     includes: WRED-enable and exponential-weight-queue (for
*     the WRED algorithm).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     VSQ rate class. Range: 0-15
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_WRED_GEN_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_wred_gen_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_wred_gen_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This procedure sets VSQ WRED general configurations,
*     includes: WRED-enable and exponential-weight-queue (for
*     the WRED algorithm).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     VSQ rate class. Range: 0-15
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_WRED_GEN_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_wred_gen_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_wred_gen_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This procedure sets VSQ WRED general configurations,
*     includes: WRED-enable and exponential-weight-queue (for
*     the WRED algorithm).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     VSQ rate class. Range: 0-15
*  DNX_SAND_OUT JER2_ARAD_ITM_VSQ_WRED_GEN_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_wred_gen_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );
/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_wred_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     A WRED test for each packet versus the packet queue or
*     VSQ that the packet is mapped to is performed. This
*     procedure sets Virtual Statistics Queue WRED, includes:
*     WRED-thresholds/probability.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     VSQ rate class. Range: 0-15
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     drop-precedence. Range: 0-3.
*  DNX_SAND_IN  JER2_ARAD_ITM_WRED_QT_DP_INFO *info -
*     pointer to configuration structure.
*  DNX_SAND_OUT JER2_ARAD_ITM_WRED_QT_DP_INFO *exact_info -
*     May vary due to rounding.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_wred_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_WRED_QT_DP_INFO *info,
    DNX_SAND_OUT JER2_ARAD_ITM_WRED_QT_DP_INFO *exact_info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_wred_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     A WRED test for each packet versus the packet queue or
*     VSQ that the packet is mapped to is performed. This
*     procedure sets Virtual Statistics Queue WRED, includes:
*     WRED-thresholds/probability.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     VSQ rate class. Range: 0-15
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     drop-precedence. Range: 0-3.
*  DNX_SAND_IN  JER2_ARAD_ITM_WRED_QT_DP_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_wred_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_WRED_QT_DP_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_wred_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     A WRED test for each packet versus the packet queue or
*     VSQ that the packet is mapped to is performed. This
*     procedure sets Virtual Statistics Queue WRED, includes:
*     WRED-thresholds/probability.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     There are 4 groups of vsqs (0-3).
*  DNX_SAND_IN  uint32                 vsq_rt_cls_ndx -
*     VSQ rate class. Range: 0-15
*  DNX_SAND_IN  uint32                 drop_precedence_ndx -
*     drop-precedence. Range: 0-3.
*  DNX_SAND_OUT JER2_ARAD_ITM_WRED_QT_DP_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_vsq_wred_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  uint32                 vsq_rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_WRED_QT_DP_INFO *info
  );
int
  jer2_arad_itm_man_exp_buffer_set(
    DNX_SAND_IN  int32  value,
    DNX_SAND_IN  uint32 mnt_lsb,
    DNX_SAND_IN  uint32 mnt_nof_bits,
    DNX_SAND_IN  uint32 exp_lsb,
    DNX_SAND_IN  uint32 exp_nof_bits,
    DNX_SAND_IN  uint8 is_signed,
    DNX_SAND_OUT uint32  *output_field,
    DNX_SAND_OUT int32  *exact_value
  );
int
  jer2_arad_itm_man_exp_buffer_get(
    DNX_SAND_IN  uint32  buffer,
    DNX_SAND_IN  uint32 mnt_lsb,
    DNX_SAND_IN  uint32 mnt_nof_bits,
    DNX_SAND_IN  uint32 exp_lsb,
    DNX_SAND_IN  uint32 exp_nof_bits,
    DNX_SAND_IN  uint8 is_signed,
    DNX_SAND_OUT int32  *value
  );

int
jer2_itm_vsq_in_group_size_get(
    DNX_SAND_IN  int                      unit, 
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx, 
    DNX_SAND_OUT uint32                   *vsq_in_group_size);

int
  jer2_arad_itm_vsq_idx_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint8                    is_cob_only,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  );
/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_counter_set
* TYPE:
*   PROC
* FUNCTION:
*     Select VSQ for monitoring. The selected VSQ counter can
*     be further read, indicating the number of packets
*     enqueued to the VSQ.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     VSQ group: 0 - Group A, VSQs 0->3, Category. 1 - Group
*     B, VSQs 4->35, Category & TC. 2 - Group C, VSQs
*     36->99,Category & CC 2/3. 3 - Group D, VSQs 100 -> 355,
*     STAG.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx -
*     VSQ in-group index:Group A (Category) Range: 0-3. Group
*     B (Category + TC) Range: 0-32. Group C (Category 2/3 +
*     CC) Range: 0-64. Group D (STAG) Range: 0-255. DNX_SAND_OUT
*     uint32 *pckt_count - Number of packets enqueued to the
*     specified VSQ
* REMARKS:
*     1. The counter is read using the vsq_counter_read API,
*        or any counter read/print API of jer2_arad_stat module.
*     2. This API is considered deprecated.
*        It is replaced by the jer2_arad_stat_vsq_cnt_select_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int
  jer2_arad_itm_vsq_counter_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  int                 core_id,
    DNX_SAND_IN  uint8               is_cob_only,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_counter_verify
* TYPE:
*   PROC
* FUNCTION:
*     Select VSQ for monitoring. The selected VSQ counter can
*     be further read, indicating the number of packets
*     enqueued to the VSQ.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx -
*     VSQ group: 0 - Group A, VSQs 0->3, Category. 1 - Group
*     B, VSQs 4->35, Category & TC. 2 - Group C, VSQs
*     36->99,Category & CC 2/3. 3 - Group D, VSQs 100 -> 355,
*     STAG.
*  DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx -
*     VSQ in-group index:Group A (Category) Range: 0-3. Group
*     B (Category + TC) Range: 0-32. Group C (Category 2/3 +
*     CC) Range: 0-64. Group D (STAG) Range: 0-255. DNX_SAND_OUT
*     uint32 *pckt_count - Number of packets enqueued to the
*     specified VSQ
* REMARKS:
*     1. The counter is read using the vsq_counter_read API,
*        or any counter read/print API of jer2_arad_stat module.
*     2. This API is considered deprecated.
*        It is replaced by the jer2_arad_stat_vsq_cnt_select_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int
  jer2_arad_itm_vsq_counter_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  int                 core_id,
    DNX_SAND_IN  uint8               is_cob_only,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_counter_get
* TYPE:
*   PROC
* FUNCTION:
*     Select VSQ for monitoring. The selected VSQ counter can
*     be further read, indicating the number of packets
*     enqueued to the VSQ.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT  JER2_ARAD_ITM_VSQ_GROUP       *vsq_group_ndx -
*     VSQ group: 0 - Group A, VSQs 0->3, Category. 1 - Group
*     B, VSQs 4->35, Category & TC. 2 - Group C, VSQs
*     36->99,Category & CC 2/3. 3 - Group D, VSQs 100 -> 355,
*     STAG.
*  DNX_SAND_OUT  JER2_ARAD_ITM_VSQ_NDX         *vsq_in_group_ndx -
*     VSQ in-group index:Group A (Category) Range: 0-3. Group
*     B (Category + TC) Range: 0-32. Group C (Category 2/3 +
*     CC) Range: 0-64. Group D (STAG) Range: 0-255. DNX_SAND_OUT
*     uint32 *pckt_count - Number of packets enqueued to the
*     specified VSQ
* REMARKS:
*     1. The counter is read using the vsq_counter_read API,
*        or any counter read/print API of jer2_arad_stat module.
*     2. This API is considered deprecated.
*        It is replaced by the jer2_arad_stat_vsq_cnt_select_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int
  jer2_arad_itm_vsq_counter_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  int                 core_id,
    DNX_SAND_OUT uint8                    *is_cob_only,
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_GROUP       *vsq_group_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_NDX         *vsq_in_group_ndx
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_vsq_counter_read
* TYPE:
*   PROC
* FUNCTION:
*     Indicates the number of packets enqueued to the
*     monitored VSQ.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT uint32                  *pckt_count -
*     Number of packets enqueued to the VSQ
* REMARKS:
*  1. The counter is selected using the vsq_counter_set API.
*  2. Maximal value (0xFFFFFFFF) indicates counter overflow.
*  3. Can be also read using counter read/print APIs of jer2_arad_stat module.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int
  jer2_arad_itm_vsq_counter_read(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  int                 core_id,
    DNX_SAND_OUT uint32                  *pckt_count
  );



int
  jer2_arad_itm_queue_is_ocb_only_get(
     DNX_SAND_IN  int                 unit,
     DNX_SAND_IN  int                 core,
     DNX_SAND_IN  uint32              queue_ndx,
     DNX_SAND_OUT uint8               *enable
     );

/*********************************************************************
* NAME:
*     jer2_arad_itm_queue_dyn_info_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Gets the dynamic info of a queue
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                  queue_ndx -
*     The ID of the queue to be set (Range: 0 - 32768).
*  DNX_SAND_OUT JER2_ARAD_ITM_QUEUE_DYN_INFO      *info -
*     pointer to dynamic info structure.
* REMARKS:
*     None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_queue_dyn_info_get(
	DNX_SAND_IN  int 				unit,
    DNX_SAND_IN  int                 core,
	DNX_SAND_IN  uint32 				 queue_ndx,
	DNX_SAND_OUT JER2_ARAD_ITM_QUEUE_DYN_INFO	  *info
  );

int
  jer2_arad_ingress_drop_status(
    DNX_SAND_IN int   unit,
    DNX_SAND_OUT uint32 *is_max_size 
  );

/*
 * set the dynamic queue thresholds for the guaranteed resource.
 * The threshold is used to achieve the resource guarantee for queues,
 * ensuring that not to much of the resource is allocated bound the total of guarantees.
 */
int
  jer2_arad_itm_dyn_total_thresh_set(
    DNX_SAND_IN  int   unit,
    DNX_SAND_IN  int32    reservation_increase /* the (signed) amount in which the thresholds should decrease (according to 100% as will be set for DP 0) */
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_priority_map_tmplt_verify
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Four sets of queues-priorities maps are held in the
*     device. Per map: describes a segment of 64 contiguous
*     queues. Each queue is either high or low priority.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  uint32                 map_ndx -
*  DNX_SAND_IN  JER2_ARAD_ITM_PRIORITY_MAP_TMPLT *info -
*     info pointer to configuration structure.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_priority_map_tmplt_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 map_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_priority_map_tmplt_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Four sets of queues-priorities maps are held in the
*     device. Per map: describes a segment of 64 contiguous
*     queues. Each queue is either high or low priority.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  uint32                 map_ndx -
*   Index of map to set. Range: 0 - 3.
*  DNX_SAND_IN  JER2_ARAD_ITM_PRIORITY_MAP_TMPLT *info -
*     info pointer to configuration structure.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_priority_map_tmplt_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 map_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_priority_map_tmplt_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Four sets of queues-priorities maps are held in the
*     device. Per map: describes a segment of 64 contiguous
*     queues. Each queue is either high or low priority.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  uint32                 map_ndx -
*   Index of map to set. Range: 0 - 3.
*  DNX_SAND_OUT JER2_ARAD_ITM_PRIORITY_MAP_TMPLT *info -
*     info pointer to configuration structure.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_priority_map_tmplt_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 map_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_priority_map_tmplt_select_verify
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     The 32K ingress-queues range is segmented into 512
*     segments of 64 contiguous queues, that is, queues 64N to
*     64N+63 that all have the same map-id (one of four).
* INPUT:
*  DNX_SAND_IN  uint32                  queue_64_ndx -
*   The index to the contiguous 64 queues. Range: 0 - 511.
*  DNX_SAND_IN  uint32                 priority_map -
*   The map-id. Range: 0 - 3.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_priority_map_tmplt_select_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                  queue_64_ndx,
    DNX_SAND_IN  uint32                 priority_map
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_priority_map_tmplt_select_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     The 32K ingress-queues range is segmented into 512
*     segments of 64 contiguous queues, that is, queues 64N to
*     64N+63 that all have the same map-id (one of four).
* INPUT:
*  DNX_SAND_IN  uint32                  queue_64_ndx -
*   The index to the contiguous 64 queues. Range: 0 - 511.
*  DNX_SAND_IN  uint32                 priority_map -
*   The map-id. Range: 0 - 3.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_priority_map_tmplt_select_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                  queue_64_ndx,
    DNX_SAND_IN  uint32                 priority_map
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_priority_map_tmplt_select_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     The 32K ingress-queues range is segmented into 512
*     segments of 64 contiguous queues, that is, queues 64N to
*     64N+63 that all have the same map-id (one of four).
* INPUT:
*  DNX_SAND_IN  uint32                  queue_64_ndx -
*   The index to the contiguous 64 queues. Range: 0 - 511.
*  DNX_SAND_IN  uint32                 priority_map -
*   The map-id. Range: 0 - 3.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_priority_map_tmplt_select_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                  queue_64_ndx,
    DNX_SAND_OUT uint32                 *priority_map
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_drop_prob_verify
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     System Red drop probabilities table fill in. The system
*     Red mechanism uses a table of 16 probabilities. The
*     table is used by indexes which choose 1 out of the 16
*     options.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_DROP_PROB *info -
*     info A pointer to the system red queue type info
*     configuration.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_drop_prob_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_DROP_PROB *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_drop_prob_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     System Red drop probabilities table fill in. The system
*     Red mechanism uses a table of 16 probabilities. The
*     table is used by indexes which choose 1 out of the 16
*     options.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_DROP_PROB *info -
*     info A pointer to the system red queue type info
*     configuration.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_drop_prob_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_DROP_PROB *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_drop_prob_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     System Red drop probabilities table fill in. The system
*     Red mechanism uses a table of 16 probabilities. The
*     table is used by indexes which choose 1 out of the 16
*     options.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_DROP_PROB *info -
*     info A pointer to the system red queue type info
*     configuration.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_drop_prob_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_DROP_PROB *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_queue_size_boundaries_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     System Red queue size boundaries, per queue type - rate
*     class. The queue size ranges table is set. For each
*     queue type and drop-precedence,
*     drop/pass/drop-with-probability parameters are set using
*     the function jer2_arad_itm_sys_red_qt_dp_info_set.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class Queue Type. Range: 0 to 63.
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_QT_INFO *info -
*     A pointer to the system red queue type info
*     configuration.
*  DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_INFO *exact_info -
*     May vary due to rounding.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_queue_size_boundaries_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_QT_INFO *info,
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_INFO *exact_info
  );

int
  jer2_arad_itm_sys_red_queue_size_boundaries_set2(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_QT_INFO *info,
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_INFO *exact_info,
    DNX_SAND_IN int mnt_lsb,
    DNX_SAND_IN int mnt_nof_bits,
    DNX_SAND_IN int exp_lsb,
    DNX_SAND_IN int exp_nof_bits
  );
/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_queue_size_boundaries_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     System Red queue size boundaries, per queue type - rate
*     class. The queue size ranges table is set. For each
*     queue type and drop-precedence,
*     drop/pass/drop-with-probability parameters are set using
*     the function jer2_arad_itm_sys_red_qt_dp_info_set.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class Queue Type. Range: 0 to 63.
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_QT_INFO *info -
*     A pointer to the system red queue type info
*     configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_queue_size_boundaries_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_QT_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_queue_size_boundaries_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     System Red queue size boundaries, per queue type - rate
*     class. The queue size ranges table is set. For each
*     queue type and drop-precedence,
*     drop/pass/drop-with-probability parameters are set using
*     the function jer2_arad_itm_sys_red_qt_dp_info_set.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress rate class Queue Type. Range: 0 to 63.
*  DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_INFO *info -
*     A pointer to the system red queue type info
*     configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_queue_size_boundaries_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_INFO *info
  );
int
  jer2_arad_itm_sys_red_queue_size_boundaries_get2(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_INFO *info,
    DNX_SAND_IN int mnt_lsb,
    DNX_SAND_IN int mnt_nof_bits,
    DNX_SAND_IN int exp_lsb,
    DNX_SAND_IN int exp_nof_bits
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_q_based_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Configures the ingress system red parameters per q-type
*     - rate class and drop-precedence. This includes the
*     thresholds and drop probability, which determine the
*     behavior of the algorithm according to the queue size
*     index.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress QDP Queue Type. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 sys_red_dp_ndx -
*     The drop precedence to be affected by this
*     configuration. Range: 0 - 3.
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *info -
*     Pointer to configuration structure. DNX_SAND_OUT
*     JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *exact_info - To be filled
*     with exact configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_q_based_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 sys_red_dp_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_q_based_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Configures the ingress system red parameters per q-type
*     - rate class and drop-precedence. This includes the
*     thresholds and drop probability, which determine the
*     behavior of the algorithm according to the queue size
*     index.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress QDP Queue Type. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 sys_red_dp_ndx -
*     The drop precedence to be affected by this
*     configuration. Range: 0 - 3.
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *info -
*     Pointer to configuration structure. DNX_SAND_OUT
*     JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *exact_info - To be filled
*     with exact configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_q_based_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 sys_red_dp_ndx,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_q_based_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     Configures the ingress system red parameters per q-type
*     - rate class and drop-precedence. This includes the
*     thresholds and drop probability, which determine the
*     behavior of the algorithm according to the queue size
*     index.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 rt_cls_ndx -
*     Ingress QDP Queue Type. Range: 0 to 63.
*  DNX_SAND_IN  uint32                 sys_red_dp_ndx -
*     The drop precedence to be affected by this
*     configuration. Range: 0 - 3.
*  DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *info -
*     Pointer to configuration structure. DNX_SAND_OUT
*     JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *exact_info - To be filled
*     with exact configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_q_based_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 sys_red_dp_ndx,
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_eg_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     At the outgoing FAP port, a System-Queue-Size is
*     maintained. Per a configurable aging-period time the
*     queue is aged. System-Queue-Size has two again models
*     (when aging time arrived): reset or decrement. Reset
*     sets the System-Queue-Size to zero, decrement decrease
*     the size of the OFP System-Queue-Size with one. Note:
*     though this function is not an ITM function, it resides
*     here due to relevance to other System RED functions.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_EG_INFO *info -
*     pointer to configuration structure.
*  DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_EG_INFO *exact_info -
*     To be filled with exact configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_eg_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_EG_INFO *info,
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_EG_INFO *exact_info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_eg_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     At the outgoing FAP port, a System-Queue-Size is
*     maintained. Per a configurable aging-period time the
*     queue is aged. System-Queue-Size has two again models
*     (when aging time arrived): reset or decrement. Reset
*     sets the System-Queue-Size to zero, decrement decrease
*     the size of the OFP System-Queue-Size with one. Note:
*     though this function is not an ITM function, it resides
*     here due to relevance to other System RED functions.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_EG_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_eg_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_EG_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_eg_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     At the outgoing FAP port, a System-Queue-Size is
*     maintained. Per a configurable aging-period time the
*     queue is aged. System-Queue-Size has two again models
*     (when aging time arrived): reset or decrement. Reset
*     sets the System-Queue-Size to zero, decrement decrease
*     the size of the OFP System-Queue-Size with one. Note:
*     though this function is not an ITM function, it resides
*     here due to relevance to other System RED functions.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_EG_INFO *info -
*     pointer to configuration structure.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_eg_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_EG_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_glob_rcs_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     In the System Red mechanism there is an aspect of
*     Consumed Resources. This mechanism gives the queues a
*     value that is compared with the value of the queue size
*     index - the maximum of the 2 is sent to the threshold
*     tests. The queues are divided to 4 ranges. In 3 types:
*     Free Unicast Data buffers Thresholds, Free Multicast
*     Data buffers Thresholds, Free BD buffers Thresholds.
*     This function determines the thresholds of the ranges
*     and the values of the ranges (0-15).
*     Note that the value of the queue is attributed to the
*     consumed resources (as opposed to the free resources).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info -
*     pointer to configuration structure. DNX_SAND_OUT
*     JER2_ARAD_ITM_SYS_RED_GLOB_RCS_VAL_INFO *exact_info - To be
*     filled with exact configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_glob_rcs_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_glob_rcs_verify
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     In the System Red mechanism there is an aspect of
*     Consumed Resources. This mechanism gives the queues a
*     value that is compared with the value of the queue size
*     index - the maximum of the 2 is sent to the threshold
*     tests. The queues are divided to 4 ranges. In 3 types:
*     Free Unicast Data buffers Thresholds, Free Multicast
*     Data buffers Thresholds, Free BD buffers Thresholds.
*     This function determines the thresholds of the ranges
*     and the values of the ranges (0-15).
*     Note that the value of the queue is attributed to the
*     consumed resources (as opposed to the free resources).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info -
*     pointer to configuration structure. DNX_SAND_OUT
*     JER2_ARAD_ITM_SYS_RED_GLOB_RCS_VAL_INFO *exact_info - To be
*     filled with exact configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_glob_rcs_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_itm_sys_red_glob_rcs_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     In the System Red mechanism there is an aspect of
*     Consumed Resources. This mechanism gives the queues a
*     value that is compared with the value of the queue size
*     index - the maximum of the 2 is sent to the threshold
*     tests. The queues are divided to 4 ranges. In 3 types:
*     Free Unicast Data buffers Thresholds, Free Multicast
*     Data buffers Thresholds, Free BD buffers Thresholds.
*     This function determines the thresholds of the ranges
*     and the values of the ranges (0-15).
*     Note that the value of the queue is attributed to the
*     consumed resources (as opposed to the free resources).
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info -
*     pointer to configuration structure. DNX_SAND_OUT
*     JER2_ARAD_ITM_SYS_RED_GLOB_RCS_VAL_INFO *exact_info - To be
*     filled with exact configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_sys_red_glob_rcs_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );



/*********************************************************************
* NAME:
*     jer2_arad_itm_convert_admit_one_test_tmplt_to_u32
* TYPE:
*   PROC
* DATE:
*   Nov 18 2007
* FUNCTION:
*     Convert the type JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT to uint32
*     in order to be written to the register field.
* INPUT:
*  DNX_SAND_IN JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT test -
*     the JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT type consists of 4 Booleans,
*     to be converted to a 4 bits
*  DNX_SAND_OUT uint32                       *test_in_sand_u32 -
*     pointer to the converted value.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_convert_admit_one_test_tmplt_to_u32(
    DNX_SAND_IN JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT test,
    DNX_SAND_OUT uint32                       *test_in_sand_u32
  );
/*********************************************************************
* NAME:
*     jer2_arad_itm_convert_u32_to_admit_one_test_tmplt
* TYPE:
*   PROC
* DATE:
*   Nov 18 2007
* FUNCTION:
*     Convert the type uint32 to JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT
*     in order to be received from the register field.
* INPUT:
*  DNX_SAND_IN  uint32 test_in_sand_u32 -
*    read from the register.
*  DNX_SAND_OUT JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT *test -
*     pointer to the converted value.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_convert_u32_to_admit_one_test_tmplt(
    DNX_SAND_IN  uint32                       test_in_sand_u32,
    DNX_SAND_OUT JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT *test
  );

int
  jer2_arad_itm_dbuff_internal2size(
    DNX_SAND_IN  uint32                   dbuff_size_internal,
    DNX_SAND_OUT JER2_ARAD_ITM_DBUFF_SIZE_BYTES *dbuff_size
  );

int
  jer2_arad_itm_dbuff_size2internal(
    DNX_SAND_IN  JER2_ARAD_ITM_DBUFF_SIZE_BYTES dbuff_size,
    DNX_SAND_OUT uint32                   *dbuff_size_internal
  );

/*
 *	Extension to the Arad API
 */
int
  jer2_arad_b_itm_glob_rcs_drop_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info
  );

int
    jer2_arad_b_itm_glob_rcs_drop_set(
      DNX_SAND_IN  int                   unit,
      DNX_SAND_IN  JER2_ARAD_THRESH_WITH_HYST_INFO mem_size[JER2_ARAD_NOF_DROP_PRECEDENCE],
      DNX_SAND_OUT JER2_ARAD_THRESH_WITH_HYST_INFO exact_mem_size[JER2_ARAD_NOF_DROP_PRECEDENCE]
    );

int
  jer2_arad_b_itm_glob_rcs_drop_get(
    DNX_SAND_IN  int                   unit,
    DNX_SAND_OUT JER2_ARAD_THRESH_WITH_HYST_INFO *mem_size
  );

/*********************************************************************
* NAME:
 *   jer2_arad_itm_committed_q_size_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the size of committed queue size (i.e., the
 *   guaranteed memory) for each VOQ, even in the case that a
 *   set of queues consume most of the memory resources.
 * INPUT:
 *   DNX_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  uint32                    grnt_bytes -
 *     Value of the guaranteed memory size. Units: Bytes. Range:
 *     0 - 256M.
 *   DNX_SAND_OUT uint32                    *exact_grnt_bytes -
 *     Exact value of the guaranteed memory size. Units: Bytes.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_itm_committed_q_size_set(
    DNX_SAND_IN  int                  unit,
    DNX_SAND_IN  uint32                  rt_cls_ndx,
    DNX_SAND_IN  SOC_DNX_ITM_GUARANTEED_INFO *info, 
    DNX_SAND_OUT SOC_DNX_ITM_GUARANTEED_INFO *exact_info 
  );

int
  jer2_arad_itm_committed_q_size_set_verify(
    DNX_SAND_IN  int                  unit,
    DNX_SAND_IN  uint32                  rt_cls_ndx,
    DNX_SAND_IN  SOC_DNX_ITM_GUARANTEED_INFO *info
  );

uint32
  jer2_arad_itm_committed_q_size_get_verify(
    DNX_SAND_IN  int        unit,
    DNX_SAND_IN  uint32        rt_cls_ndx
  );
/*********************************************************************
*     Gets the configuration set by the
 *     "jer2_arad_itm_committed_q_size_set" API.
 *     Refer to "jer2_arad_itm_committed_q_size_set" API for
 *     details.
*********************************************************************/
int
  jer2_arad_itm_committed_q_size_get(
    DNX_SAND_IN  int                  unit,
    DNX_SAND_IN  uint32                  rt_cls_ndx,
    DNX_SAND_OUT SOC_DNX_ITM_GUARANTEED_INFO *exact_info 
  );

/* Maps the VOQ TC to the VSQ TC for the new VSQ types per interface */
shr_error_e
jer2_arad_itm_pfc_tc_map_set(const int unit, const int tc_in, const int port_id, const int tc_out);

shr_error_e
jer2_arad_itm_pfc_tc_map_get(const int unit, const int tc_in, const int port_id, int *tc_out); 

int
  jer2_itm_dp_discard_set_verify(
    DNX_SAND_IN  int        unit,
    DNX_SAND_IN  uint32        discard_dp
  );

/**
 * Initialize the Drop Precedence Mapping: 
 * <dp-meter-cmd,incoming-dp,meter-processor-dp> -> <ingress-dp,egress-dp> 
 * The (auto-detected) mode (PP or TM) determines the mapping (for TM we ignore the meter and the 
 * ingress-dp is passed through). 
 * 
 * @param unit 
 * 
 * @return uint32 
 */
int 
  jer2_arad_itm_setup_dp_map(
    DNX_SAND_IN  int unit
  );

/*
 * Set the alpha value of fair adaptive tail drop for the given rate class and DP.
 * Arad+ only.
 */
int
  jer2_arad_plus_itm_alpha_set(
    DNX_SAND_IN  int       unit,
    DNX_SAND_IN  uint32       rt_cls_ndx,
    DNX_SAND_IN  uint32       drop_precedence_ndx,
    DNX_SAND_IN  int32        alpha 
  );

/*
 * Get the alpha value of fair adaptive tail drop for the given rate class and DP.
 * Arad+ only.
 */
int
  jer2_arad_plus_itm_alpha_get(
    DNX_SAND_IN  int       unit,
    DNX_SAND_IN  uint32       rt_cls_ndx,
    DNX_SAND_IN  uint32       drop_precedence_ndx,
    DNX_SAND_OUT int32        *alpha 
  );

/*
 * Arad+ only: enable/disable fair adaptive tail drop (Free BDs dynamic MAX queue size)
 */
int
  jer2_arad_plus_itm_fair_adaptive_tail_drop_enable_set(
    DNX_SAND_IN  int   unit,
    DNX_SAND_IN  uint8    enabled /* 0=disabled, non zero=enabled */
  );

/*
 * Arad+ only: Check if fair adaptive tail drop (Free BDs dynamic MAX queue size) is enabled.
 */
int
  jer2_arad_plus_itm_fair_adaptive_tail_drop_enable_get(
    DNX_SAND_IN  int   unit,
    DNX_SAND_OUT uint8    *enabled /* return value: 0=disabled, 1=enabled */
  );

int
  jer2_arad_itm_fadt_tail_drop_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_IN  SOC_DNX_ITM_FADT_INFO  *info,
    DNX_SAND_OUT SOC_DNX_ITM_FADT_INFO  *exact_info
  );

int
  jer2_arad_itm_fadt_tail_drop_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 rt_cls_ndx,
    DNX_SAND_IN  uint32                 drop_precedence_ndx,
    DNX_SAND_OUT SOC_DNX_ITM_FADT_INFO  *info
  );

void
  jer2_arad_JER2_ARAD_ITM_DRAM_BUFFERS_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_DRAM_BUFFERS_INFO *info
  );


void
  jer2_arad_JER2_ARAD_ITM_GLOB_RCS_FC_TYPE_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_FC_TYPE *info
  );

void
  jer2_arad_JER2_ARAD_ITM_GLOB_RCS_FC_TH_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_FC_TH *info
  );

void
  jer2_arad_JER2_ARAD_ITM_GLOB_RCS_DROP_TH_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info
  );

void
  jer2_arad_JER2_ARAD_ITM_QUEUE_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_QUEUE_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_CATEGORY_RNGS_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_CATEGORY_RNGS *info
  );

void
  jer2_arad_JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT *info
  );

void
  jer2_arad_JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_CR_DISCOUNT_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_CR_DISCOUNT_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_WRED_QT_DP_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_WRED_QT_DP_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_TAIL_DROP_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_TAIL_DROP_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_VSQ_FC_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_FC_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_VSQ_WRED_GEN_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_STAG_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_STAG_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_INGRESS_SHAPE_Q_RANGE_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_INGRESS_SHAPE_Q_RANGE *info
  );

void
  jer2_arad_JER2_ARAD_ITM_INGRESS_SHAPE_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_INGRESS_SHAPE_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_PRIORITY_MAP_TMPLT_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );

void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_DROP_PROB_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_DROP_PROB *info
  );

void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_QT_DP_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_QT_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_QT_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_EG_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_EG_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_GLOB_RCS_THS_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_GLOB_RCS_THS *info
  );

void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_GLOB_RCS_VALS_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_GLOB_RCS_VALS *info
  );

void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );

#if JER2_ARAD_DEBUG_IS_LVL1


const char*
  jer2_arad_JER2_ARAD_ITM_DBUFF_SIZE_BYTES_to_string(
    DNX_SAND_IN JER2_ARAD_ITM_DBUFF_SIZE_BYTES enum_val
  );


const char*
  jer2_arad_JER2_ARAD_ITM_ADMIT_TSTS_to_string(
    DNX_SAND_IN JER2_ARAD_ITM_ADMIT_TSTS enum_val
  );



const char*
  jer2_arad_JER2_ARAD_ITM_VSQ_GROUP_to_string(
    DNX_SAND_IN JER2_ARAD_ITM_VSQ_GROUP enum_val
  );



void
  jer2_arad_JER2_ARAD_ITM_DRAM_BUFFERS_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_DRAM_BUFFERS_INFO *info
  );


void
  jer2_arad_JER2_ARAD_ITM_GLOB_RCS_FC_TYPE_print(
    DNX_SAND_IN JER2_ARAD_ITM_GLOB_RCS_FC_TYPE *info
  );



void
  jer2_arad_JER2_ARAD_ITM_GLOB_RCS_FC_TH_print(
    DNX_SAND_IN JER2_ARAD_ITM_GLOB_RCS_FC_TH *info
  );

void
  jer2_arad_JER2_ARAD_ITM_GLOB_RCS_DROP_TH_print_no_table(
    DNX_SAND_IN JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info
  );

void
  jer2_arad_JER2_ARAD_ITM_GLOB_RCS_DROP_TH_print(
    DNX_SAND_IN JER2_ARAD_ITM_GLOB_RCS_DROP_TH *info
  );



void
  jer2_arad_JER2_ARAD_ITM_QUEUE_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_QUEUE_INFO *info
  );



void
  jer2_arad_JER2_ARAD_ITM_CATEGORY_RNGS_print(
    DNX_SAND_IN JER2_ARAD_ITM_CATEGORY_RNGS *info
  );



void
  jer2_arad_JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT_print(
    DNX_SAND_IN JER2_ARAD_ITM_ADMIT_ONE_TEST_TMPLT *info
  );



void
  jer2_arad_JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_CR_DISCOUNT_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_CR_DISCOUNT_INFO *info
  );



void
  jer2_arad_JER2_ARAD_ITM_WRED_QT_DP_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_WRED_QT_DP_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_TAIL_DROP_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_TAIL_DROP_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_VSQ_FC_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_VSQ_FC_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  );

void
  jer2_arad_JER2_ARAD_ITM_VSQ_WRED_GEN_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );



void
  jer2_arad_JER2_ARAD_ITM_STAG_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_STAG_INFO *info
  );



void
  jer2_arad_JER2_ARAD_ITM_INGRESS_SHAPE_Q_RANGE_print(
    DNX_SAND_IN JER2_ARAD_ITM_INGRESS_SHAPE_Q_RANGE *info
  );

void
  jer2_arad_JER2_ARAD_ITM_INGRESS_SHAPE_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_INGRESS_SHAPE_INFO *info
  );



void
  jer2_arad_JER2_ARAD_ITM_PRIORITY_MAP_TMPLT_print(
    DNX_SAND_IN JER2_ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );



void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_DROP_PROB_print(
    DNX_SAND_IN JER2_ARAD_ITM_SYS_RED_DROP_PROB *info
  );



void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_QT_DP_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );



void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_QT_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_SYS_RED_QT_INFO *info
  );



void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_EG_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_SYS_RED_EG_INFO *info
  );



void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_GLOB_RCS_THS_print(
    DNX_SAND_IN JER2_ARAD_ITM_SYS_RED_GLOB_RCS_THS *info
  );



void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_GLOB_RCS_VALS_print(
    DNX_SAND_IN JER2_ARAD_ITM_SYS_RED_GLOB_RCS_VALS *info
  );



void
  jer2_arad_JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO_print(
    DNX_SAND_IN JER2_ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );


#endif /* JER2_ARAD_DEBUG_IS_LVL1 */



/* } __JER2_ARAD_INGRESS_TRAFFIC_MGMT_INCLUDED__*/
#endif

