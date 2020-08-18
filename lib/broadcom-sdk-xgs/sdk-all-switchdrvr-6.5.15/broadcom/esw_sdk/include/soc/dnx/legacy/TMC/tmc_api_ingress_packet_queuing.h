/* $Id: jer2_jer2_jer2_tmc_api_ingress_packet_queuing.h,v 1.7 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_jer2_jer2_tmc/include/soc_jer2_jer2_jer2_tmcapi_ingress_packet_queuing.h
*
* MODULE PREFIX:  soc_jer2_jer2_jer2_tmcipq
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

#ifndef __SOC_DNX_API_INGRESS_PACKET_QUEUING_INCLUDED__
/* { */
#define __SOC_DNX_API_INGRESS_PACKET_QUEUING_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dnx/legacy/TMC/tmc_api_general.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_DNX_IPQ_INVALID_FLOW_QUARTET    0x3fff
/* } */

/*************
 * MACROS    *
 *************/
/* { */
#define SOC_DNX_IPQ_Q_TO_QRTT_ID(que_id) ((que_id)/4)
#define SOC_DNX_IPQ_QRTT_TO_Q_ID(q_que_id) ((q_que_id)*4)
#define SOC_DNX_IPQ_Q_TO_1K_ID(que_id) ((que_id)/1024)
#define SOC_DNX_IPQ_1K_TO_Q_ID(k_que_id) ((k_que_id)*1024)

/* Max & min values for Stak Lag and base_queue:      */
#define SOC_DNX_IPQ_STACK_LAG_DOMAIN_MIN     0
#define SOC_DNX_IPQ_STACK_LAG_DOMAIN_MAX     64

#define SOC_DNX_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MIN     0
#define SOC_DNX_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX     (SOC_IS_JERICHO(unit)? 256 : 64)
#define SOC_DNX_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_ALL     0xffffffff

#define SOC_DNX_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_MIN     0
#define SOC_DNX_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_MAX     4
#define SOC_DNX_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_ALL     0xffffffff

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/*     IPQ Queue type - Traffic Class. Range: 0-7        */
typedef uint32 SOC_DNX_IPQ_TR_CLS;

typedef enum
{
  /*
   *  Ingress-Packet-traffic-class: Value 0
   */
  SOC_DNX_IPQ_TR_CLS_MIN=0,
  /*
   *  Ingress-Packet-traffic-class: Value 7
   */
  SOC_DNX_IPQ_TR_CLS_MAX=7,
  /*
   *  Must be the last value
   */
  SOC_DNX_IPQ_TR_CLS_RNG_LAST
}SOC_DNX_IPQ_TR_CLS_RNG;

typedef struct
{
  /*
   *  Explicit queue ID packet Base queue number. Range 0 to
   *  nof_queues.
   */
  uint32 base_queue_id;
  /*
   *  TRUE - queue is 'base_queue_id' + Packet's explicit
   *  queue ID (add).
   *  FALSE - queue is 'base_queue_id' - Packet's
   *  explicit queue ID (subtract).
   */
  uint8 queue_id_add_not_decrement;
}SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO;
typedef struct
{
  /*
   *  If set the specific queue is valid, Otherwise the queue
   *  is set to be invalid and packets are not sent to it.
   */
  uint8 valid;
  /*
   *  Packet is stored in queue: base_queue + class. Range: 0
   *  - 32K-1.
   */
  uint32 base_queue;

} SOC_DNX_IPQ_BASEQ_MAP_INFO;

typedef struct
{
  /*
   *  The index of the first flow quartet to which the queue
   *  quartet points. A single queue quartet might point to 1
   *  quartets of flows, 2 quartets of flows or 4 quartets of
   *  flow, depending on the 'Interdigitated' and 'Composite'
   *  modes.
   */
  uint32 flow_quartet_index;
  /*
   *  When TRUE, the flow quartets that this queue quartet
   *  points to, might use composite flows. The user might
   *  configure the flows to use only one sub-flow, but the
   *  second sub-flow is still reserved as far as the
   *  flow-queues mapping is concerned.
   */
  int is_composite;
} SOC_DNX_IPQ_QUARTET_MAP_INFO;

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
  SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );

void
  SOC_DNX_IPQ_BASEQ_MAP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_IPQ_BASEQ_MAP_INFO *info
  );

void
  SOC_DNX_IPQ_QUARTET_MAP_INFO_clear(
    DNX_SAND_OUT SOC_DNX_IPQ_QUARTET_MAP_INFO *info
  );


const char*
  SOC_DNX_IPQ_TR_CLS_RNG_to_string(
    DNX_SAND_IN SOC_DNX_IPQ_TR_CLS_RNG enum_val
  );

void
  SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(
    DNX_SAND_IN SOC_DNX_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );

void
  SOC_DNX_IPQ_BASEQ_MAP_INFO_print(
    DNX_SAND_IN  SOC_DNX_IPQ_BASEQ_MAP_INFO *info
  );

void
  SOC_DNX_IPQ_QUARTET_MAP_INFO_print(
    DNX_SAND_IN SOC_DNX_IPQ_QUARTET_MAP_INFO *info
  );

void
  soc_jer2_jer2_jer2_tmcips_non_empty_queues_print(
    DNX_SAND_IN  int   unit,
    DNX_SAND_IN  uint32   print_first_local_flow
  );


/* } */


/* } __SOC_DNX_API_INGRESS_PACKET_QUEUING_INCLUDED__*/
#endif
