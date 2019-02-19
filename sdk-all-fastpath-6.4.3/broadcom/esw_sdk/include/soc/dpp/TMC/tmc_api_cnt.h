/* $Id: tmc_api_cnt.h,v 1.16 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/tmc/include/soc_tmcapi_cnt.h
*
* MODULE PREFIX:  soc_tmccnt
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_TMC_API_CNT_INCLUDED__
/* { */
#define __SOC_TMC_API_CNT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Cache length containing counters chosen by the polling
 *     algorithm.                                              */
#define  SOC_TMC_CNT_CACHE_LENGTH_PB (16)
#define  SOC_TMC_CNT_CACHE_LENGTH_ARAD (16*1024)  

#define SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL (7)

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

typedef enum
{
  /*
   *  Counter processor A
   */
  SOC_TMC_CNT_PROCESSOR_ID_A = 0,
  /*
   *  Counter processor B
   */
  SOC_TMC_CNT_PROCESSOR_ID_B = 1,
  /*
   *  Number of types in SOC_TMC_CNT_PROCESSOR_ID
   */
  SOC_TMC_CNT_NOF_PROCESSOR_IDS_PETRA_B = 2,
	/*
	 *  Counter processor C
	 */
	SOC_TMC_CNT_PROCESSOR_ID_C = 2,
	/*
	 *  Counter processor D
	 */
	SOC_TMC_CNT_PROCESSOR_ID_D = 3,
	/*
	 *  Number of types in SOC_TMC_CNT_PROCESSOR_ID
	 */
	SOC_TMC_CNT_NOF_PROCESSOR_IDS_ARAD = 4

}SOC_TMC_CNT_PROCESSOR_ID;

#define SOC_TMC_CNT_NOF_PROCESSOR_IDS                           SOC_TMC_CNT_NOF_PROCESSOR_IDS_PETRA_B

typedef enum
{
  /*
   *  Sets of two consecutive counters: enqueued data and
   *  discarded data. Must be set if the source type is
   *  'CNM_ID'.
   */
  SOC_TMC_CNT_MODE_ING_NO_COLOR = 0,
  /*
   *  Sets of four consecutive counters: enqueued green,
   *  discarded green, enqueued yellow, and discarded yellow
   *  and red data.
   */
  SOC_TMC_CNT_MODE_ING_COLOR_RES_LOW = 1,
  /*
   *  Sets of five consecutive counters: enqueued green,
   *  discarded green, enqueued yellow, discarded yellow, and
   *  discarded red data.
   */
  SOC_TMC_CNT_MODE_ING_COLOR_RES_HI = 2,
  /*
   *  Number of types in SOC_TMC_CNT_MODE_ING
   */
  SOC_TMC_CNT_NOF_MODE_INGS_PETRA_B = 3,

  /*
   *  Only EnQueue packets, 2 counters per set: green vs yellow & red.
   */
  SOC_TMC_CNT_MODE_ING_COLOR_RES_ENQ_HI = 4,
  /*
   *  Sets of one counter: enqueued data
   */
  SOC_TMC_CNT_MODE_ING_FWD_NO_COLOR = 5,
  /*
   *  Sets of one counter: discarded data.
   */
  SOC_TMC_CNT_MODE_ING_DROP_NO_COLOR = 6,
  /*
   *  Sets of one counter: data
   */
  SOC_TMC_CNT_MODE_ING_ALL_NO_COLOR = 7,
  /*
   *  Only EnQueue packets, 2 counters per set: fwd green vs fwd not-green 
   */
  SOC_TMC_CNT_MODE_ING_FWD_SIMPLE_COLOR = 8,
  /*
   *  Only EnQueue packets, 2 counters per set: drop green vs drop not-green 
   */
  SOC_TMC_CNT_MODE_ING_DROP_SIMPLE_COLOR = 9,
  /*
   * Config the entry to count according to custom_mode_params
   */
  SOC_TMC_CNT_MODE_ING_CONFIGURABLE_OFFSETS = 10,
  /*
   *  Number of types in SOC_TMC_CNT_MODE_ING
   */
  SOC_TMC_CNT_NOF_MODE_INGS_ARAD = 10

}SOC_TMC_CNT_MODE_ING;

#define SOC_TMC_CNT_NOF_MODE_INGS                           SOC_TMC_CNT_NOF_MODE_INGS_PETRA_B

typedef enum
{
  /*
   *  Sets of one counter: only enqueued data.
   */
  SOC_TMC_CNT_MODE_EG_RES_NO_COLOR = 0,
  /*
   *  Sets of two consecutive counters: enqueued green and
   *  enqueued yellow.
   */
  SOC_TMC_CNT_MODE_EG_RES_COLOR = 1,
  /*
   *  Number of types in SOC_TMC_CNT_MODE_EG
   */
  SOC_TMC_CNT_NOF_MODE_EGS = 2
}SOC_TMC_CNT_MODE_EG_RES;




typedef enum
{
  /*
   *  The counting command source is the ingress Dune packet
   *  processor.
   */
  SOC_TMC_CNT_SRC_TYPE_ING_PP = 0,
  /*
   *  The counting is done on the VOQs. If this option is
   *  chosen, the 'voq_cnt' parameter must be set to define
   *  which VOQ are counted.
   */
  SOC_TMC_CNT_SRC_TYPE_VOQ = 1,
  /*
   *  The counting is done according to 12 configurable
   *  consecutive bits in the Statistic tag. If this option is
   *  chosen, the 'stag_first_bit' parameter must be set to
   *  define which bit range in the Statistic-Tag is
   *  significant. The effective range depends on the counting
   *  mode: e.g., with the 'NO_COLOR' mode, the all range (4K)
   *  is significant. With the 'COLOR_RES_LOW' (resp. 'HIGH'),
   *  the effective range is 0 - 2K-1 (resp. 1638, i.e.
   *  8K/5-1).
   */
  SOC_TMC_CNT_SRC_TYPE_STAG = 2,
  /*
   *  The counting is done on the VSQs.
   */
  SOC_TMC_CNT_SRC_TYPE_VSQ = 3,
  /*
   *  The counting is done per CNM id. The count starts only
   *  if a CNM packet is generated. The counting mode must be
   *  'NO_COLOR'.
   */
  SOC_TMC_CNT_SRC_TYPE_CNM_ID = 4,
  /*
   *  The counting source is the egress Dune packet processor.
   *  This mode can be used only if the processor index is B.
   */
  SOC_TMC_CNT_SRC_TYPE_EGR_PP = 5,
  /*
   *  Number of types in SOC_TMC_CNT_SRC_TYPE
   */
  SOC_TMC_CNT_NOF_SRC_TYPES_PETRA_B = 6,
  /*
   *  The counting command source is the ingress Dune packet
   *  processor. MSB of first pointer
   */
  SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB = 6,
  /*
   *  The counting command source is the ingress Dune packet
   *  processor. LSB of second pointer
   */
  SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB = 7,
  /*
   *  The counting command source is the ingress Dune packet
   *  processor. MSB of second pointer
   */
  SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB = 8,
	/*
	 *  The counting source is the egress Dune packet processor.
	 *  This mode can be used only if the processor index is B.
	 */
	SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB = 9,
	/*
	 *  Ingress OAM 1-LSB / 1-MSB / 2-LSB / 2-MSB 
	 */
	SOC_TMC_CNT_SRC_TYPE_OAM_ING_A = 10,
	SOC_TMC_CNT_SRC_TYPE_OAM_ING_B = 11,
	SOC_TMC_CNT_SRC_TYPE_OAM_ING_C = 12,
	SOC_TMC_CNT_SRC_TYPE_OAM_ING_D = 13,
	/*
	 *  Egress EPNI 1-LSB / 1-MSB / 2-LSB / 2-MSB 
	 */
	SOC_TMC_CNT_SRC_TYPE_EPNI_A = 14,
	SOC_TMC_CNT_SRC_TYPE_EPNI_B = 15,
	SOC_TMC_CNT_SRC_TYPE_EPNI_C = 16,
	SOC_TMC_CNT_SRC_TYPE_EPNI_D = 17,
	/*
	 *  Number of types in SOC_TMC_CNT_SRC_TYPE
	 */
	SOC_TMC_CNT_NOF_SRC_TYPES_ARAD = 18
}SOC_TMC_CNT_SRC_TYPE;


#define SOC_TMC_CNT_NOF_SRC_TYPES                           SOC_TMC_CNT_NOF_SRC_TYPES_PETRA_B

typedef enum
{
  /*
   *  Each counter counts 1 consecutive queue.
   */
  SOC_TMC_CNT_Q_SET_SIZE_1_Q = 0,
  /*
   *  Each counter counts 2 consecutive queues.
   */
  SOC_TMC_CNT_Q_SET_SIZE_2_Q = 1,
  /*
   *  Each counter counts 4 consecutive queues.
   */
  SOC_TMC_CNT_Q_SET_SIZE_4_Q = 2,
  /*
   *  Each counter counts 8 consecutive queues.
   */
  SOC_TMC_CNT_Q_SET_SIZE_8_Q = 3,
  /*
   *  Number of types in SOC_TMC_CNT_Q_SET_SIZE
   */
  SOC_TMC_CNT_NOF_Q_SET_SIZES = 4
}SOC_TMC_CNT_Q_SET_SIZE;


typedef enum
{
  /*
   *  Counter-Id has the format: {Egress-MC(1),
   *  not(System-MC(1)), Queue-Pair(7), Traffic-Class(3)}.
   *  Must be set for Outgoing Ports of type TM.
   *  Configure per Queue-pair (and not per PP-Port)
   */
  SOC_TMC_CNT_MODE_EG_TYPE_TM = 0,
  /*
   *  Counter-Id equals the Packet VSI
   */
  SOC_TMC_CNT_MODE_EG_TYPE_VSI = 1,
  /*
   *  The Counter-Id equals the Packet Out-LIF
   */
  SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF = 2,
  /*
   *  Number of types in SOC_TMC_CNT_MODE_EG_TYPE
   */
  SOC_TMC_CNT_NOF_MODE_EG_TYPES_PETRA_B = 3,
  /*
   *  The Counter-Id equals the Packet ACE-Pointer from PMF
   */
  SOC_TMC_CNT_MODE_EG_TYPE_PMF = 3,
  /*
   * Same as SOC_TMC_CNT_MODE_EG_TYPE_TM, but 
   *  Configure per PP-Port (and not per Queue-pair)
   */
  SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT = 4,
  /*
   *  Number of types in SOC_TMC_CNT_MODE_EG_TYPE
   */
  SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD = 5
}SOC_TMC_CNT_MODE_EG_TYPE;

#define SOC_TMC_CNT_NOF_MODE_EG_TYPES                           SOC_TMC_CNT_NOF_MODE_EG_TYPES_PETRA_B

typedef enum
{
    SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES = 0,
    SOC_TMC_CNT_FORMAT_PKTS = 1,
    SOC_TMC_CNT_FORMAT_BYTES = 2,
    SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE = 3,
    SOC_TMC_CNT_FORMAT_IHB_COMMANDS = 4, /*unused*/
	SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS = 5,
    SOC_TMC_CNT_NOF_FORMATS_ARAD = 6
} SOC_TMC_CNT_FORMAT;

typedef enum
{
    /* count all packets, including replicated packets*/
    SOC_TMC_CNT_REPLICATED_PKTS_ALL = 0,
    /*count only forwarded packets, including Multicast replicated packets (but not snooped / mirrored packets)*/
    SOC_TMC_CNT_REPLICATED_PKTS_FRWRD_AND_MC = 1,
    /*count only forwarded packets*/
    SOC_TMC_CNT_REPLICATED_PKTS_FRWRD = 2,
    /*
    *  Number of types in SOC_TMC_CNT_REPLICATED_PKTS
    */
    SOC_TMC_CNT_NOF_REPLICATED_PKTS = 3

} SOC_TMC_CNT_REPLICATED_PKTS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Define the egress counting resolution.
   */
  SOC_TMC_CNT_MODE_EG_RES resolution;
  /*
   *  Define the Counter-ID format. Not relevant to Arad.
   */
  SOC_TMC_CNT_MODE_EG_TYPE type;
  /*
   *  Counter-ID base value. Range: 0 - 8K-1.
   */
  uint32 base_val;

} SOC_TMC_CNT_MODE_EG;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The start queue from which counting per queue is
   *  performed. Range: 0 - 32K-1.
   */
  uint32 start_q;
  /*
   *  Number of consecutive queues to count together per
   *  counter.
   */
  SOC_TMC_CNT_Q_SET_SIZE q_set_size;

} SOC_TMC_CNT_VOQ_PARAMS;


typedef enum {
	SOC_TMC_CNT_BMAP_OFFSET_GREEN_FWD = 0,
	SOC_TMC_CNT_BMAP_OFFSET_GREEN_DROP,
	SOC_TMC_CNT_BMAP_OFFSET_YELLOW_FWD,
	SOC_TMC_CNT_BMAP_OFFSET_YELLOW_DROP,
	SOC_TMC_CNT_BMAP_OFFSET_RED_FWD,
	SOC_TMC_CNT_BMAP_OFFSET_RED_DROP,
	SOC_TMC_CNT_BMAP_OFFSET_BLACK_FWD,
	SOC_TMC_CNT_BMAP_OFFSET_BLACK_DROP,
	SOC_TMC_CNT_BMAP_OFFSET_COUNT
}SOC_TMC_CNT_BMAP_OFFSET_MAPPING;

/*each entry in the array is bitmap of which counters from SOC_TMC_CNT_BMAP_OFFSET_MAPPING is set*/
typedef struct
{
	uint32 entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_COUNT]; 
	uint32 nof_counters;
}SOC_TMC_CNT_CUSTOM_MODE_PARAMS;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Define the counting source type.
   */
  SOC_TMC_CNT_SRC_TYPE src_type;
  /*
   *  Define the ingress counting mode. Soc_petra-B: Must be set only if
   *  the counting source is not 'EGR_PP'.
   */
  SOC_TMC_CNT_MODE_ING mode_ing;
  /*
   *  Define the egress counting mode. Must be set only if the
   *  counting source is 'EGR_PP'.
   */
  SOC_TMC_CNT_MODE_EG mode_eg;
  /*
   *  Configuration of the VOQ counters. Must be set only if
   *  the counting source is 'VOQ'.
   */
  SOC_TMC_CNT_VOQ_PARAMS voq_cnt;
  /*
   *  Define the first significant bit (for a range of
   *  consecutive 12 bits) in the Statistic-Tag for the
   *  counting. Must be set only if the counting source is
   *  'STAG'.
   */
  uint32 stag_lsb;
  /* Counter format: packet, bytes, both of them or queue size. Arad-only */
  SOC_TMC_CNT_FORMAT            format;
  /* Counter replication format. Arad-only */
  SOC_TMC_CNT_REPLICATED_PKTS   replicated_pkts;
  /* Custom mode params valid only when mode_ing. Arad plus only*/
  SOC_TMC_CNT_CUSTOM_MODE_PARAMS custom_mode_params;

} SOC_TMC_CNT_COUNTERS_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If True, at least a counter overflowed.
   */
  uint8 is_overflow;
  /*
   *  Counter index of the last counter pair that overflowed.
   *  Valid only if the overflow interrupt is set.
   */
  uint32 last_cnt_id;
  /*
   *  If True, the packet count overflowed for this last
   *  counter which overflowed.
   */
  uint8 is_pckt_overflow;
  /*
   *  If True, the byte count overflowed for this last counter
   *  which overflowed.
   */
  uint8 is_byte_overflow;

} SOC_TMC_CNT_OVERFLOW;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If True, then the cache for the polling algorithm is
   *  full. The user must read the cache immediately to avoid
   *  counter overflows.
   */
  uint8 is_cache_full;
  /*
   *  Indicate if a counter overflowed.
   */
  SOC_TMC_CNT_OVERFLOW overflow_cnt;
  /*
   *  Number of active (non-empty) pair of counters, i.e. at
   *  least one count (byte or octet) is not null.
   */
  uint32 nof_active_cnts;
  /*
   *  If True, at least a counter command tried to access an
   *  invalid counter index.
   */
  uint8 is_cnt_id_invalid;

} SOC_TMC_CNT_STATUS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Index of the returned counter entry.
   */
  uint32 counter_id;
  /*
   *  Value of the packet counter. Units: Packets. Range: 0 -
   *  2^25-1.
   */
  uint32 pkt_cnt;
  /*
   *  Value of the byte counter. Units: Bytes. Range: 0 -
   *  2^32-1.
   */
  uint64 byte_cnt;

} SOC_TMC_CNT_RESULT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Array of the cache counters. Only the first
   *  'nof_counters' counters are relevant.
   */
  SOC_TMC_CNT_RESULT cnt_result[SOC_TMC_CNT_CACHE_LENGTH_ARAD];
  /*
   *  Number of valid counters in the counter result
   *  'cnt_result'. Range: 0 - 16.
   */
  uint32 nof_counters;

} SOC_TMC_CNT_RESULT_ARR;


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

void
  SOC_TMC_CNT_VOQ_PARAMS_clear(
    SOC_SAND_OUT SOC_TMC_CNT_VOQ_PARAMS *info
  );

void
  SOC_TMC_CNT_CUSTOM_MODE_PARAMS_clear(
    SOC_SAND_OUT SOC_TMC_CNT_CUSTOM_MODE_PARAMS *info
  );

void
  SOC_TMC_CNT_COUNTERS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_CNT_COUNTERS_INFO *info
  );

void
  SOC_TMC_CNT_OVERFLOW_clear(
    SOC_SAND_OUT SOC_TMC_CNT_OVERFLOW *info
  );

void
  SOC_TMC_CNT_STATUS_clear(
    SOC_SAND_OUT SOC_TMC_CNT_STATUS *info
  );

void
  SOC_TMC_CNT_RESULT_clear(
    SOC_SAND_OUT SOC_TMC_CNT_RESULT *info
  );

void
  SOC_TMC_CNT_RESULT_ARR_clear(
    SOC_SAND_OUT SOC_TMC_CNT_RESULT_ARR *info
  );

void
SOC_TMC_CNT_MODE_EG_clear(
  SOC_SAND_OUT SOC_TMC_CNT_MODE_EG *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_CNT_PROCESSOR_ID_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID enum_val
  );

const char*
  SOC_TMC_CNT_MODE_ING_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_MODE_ING enum_val
  );

const char*
  SOC_TMC_CNT_MODE_EG_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_MODE_EG_RES enum_val
  );

const char*
  SOC_TMC_CNT_SRC_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_SRC_TYPE enum_val
  );

const char*
  SOC_TMC_CNT_Q_SET_SIZE_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_Q_SET_SIZE enum_val
  );
const char*
SOC_TMC_CNT_MODE_EG_TYPE_to_string(
  SOC_SAND_IN  SOC_TMC_CNT_MODE_EG_TYPE enum_val
  );

const char*
SOC_TMC_CNT_FORMAT_to_string(
  SOC_SAND_IN  SOC_TMC_CNT_FORMAT enum_val
  );

const char*
  SOC_TMC_CNT_BMAP_OFFSET_MAPPING_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_BMAP_OFFSET_MAPPING enum_val
  );

void
  SOC_TMC_CNT_CUSTOM_MODE_PARAMS_print(
    SOC_SAND_IN  SOC_TMC_CNT_CUSTOM_MODE_PARAMS *info
  );

void
  SOC_TMC_CNT_VOQ_PARAMS_print(
    SOC_SAND_IN  SOC_TMC_CNT_VOQ_PARAMS *info
  );

void
  SOC_TMC_CNT_COUNTERS_INFO_print(
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO *info
  );

void
  SOC_TMC_CNT_OVERFLOW_print(
    SOC_SAND_IN  SOC_TMC_CNT_OVERFLOW *info
  );

void
  SOC_TMC_CNT_STATUS_print(
    SOC_SAND_IN  SOC_TMC_CNT_STATUS *info
  );

void
  SOC_TMC_CNT_RESULT_print(
    SOC_SAND_IN  SOC_TMC_CNT_RESULT *info
  );
void SOC_TMC_CNT_MODE_EG_print
    (
    SOC_SAND_IN  SOC_TMC_CNT_MODE_EG *info
    );
void
  SOC_TMC_CNT_RESULT_ARR_print(
    SOC_SAND_IN  SOC_TMC_CNT_RESULT_ARR *info
  );


#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_TMC_API_CNT_INCLUDED__*/
#endif
