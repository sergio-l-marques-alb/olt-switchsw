/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_proto.h
*
* @purpose   SFlow Protocol Datagram format definitions
*
* @component sflow
*
* @comments  This section lists all the macros used in the sFlow
*            component
*
* @create 19-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#ifndef SFLOW_DEFS_H
#define SFLOW_DEFS_H

#include "osapi_support.h"
#include "commdefs.h"
#include "defaultconfig.h"

#define L7_SFLOW_OWNER_STRING_LEN     FD_SFLOW_OWNER_LENGTH
#define SFLOW_AGENT_VERSION_LEN       256
#define SFLOW_SW_VERSION_LEN          20
#define SFLOW_FIRST_RCVR_INDEX 1
#define SFLOW_MIB_VERSION "1.3"
typedef enum {
  L7_SFLOW_DS_TYPE_IFINDEX = 0,
  L7_SFLOW_DS_TYPE_VLAN    = 1,
  L7_SFLOW_DS_TYPE_ENTPHYS = 2
} L7_SFLOW_DS_TYPE_t;


#define SFLOW_MIN_HEADER_SIZE         L7_SFLOW_MIN_HEADER_SIZE 
#define SFLOW_MAX_HEADER_SIZE         L7_SFLOW_MAX_HEADER_SIZE 
#define SFLOW_DEFAULT_HEADER_SIZE     L7_SFLOW_DEFAULT_HEADER_SIZE 
#define SFLOW_DEFAULT_COLLECTOR_PORT  L7_SFLOW_DEFAULT_COLLECTOR_PORT
#define SFLOW_DEFAULT_SAMPLING_RATE   L7_SFLOW_DEFAULT_SAMPLING_RATE
#define SFLOW_MAX_DATAGRAM_SIZE       L7_SFLOW_MAX_DATAGRAM_SIZE
#define SFLOW_MIN_DATAGRAM_SIZE       L7_SFLOW_MIN_DATAGRAM_SIZE
#define SFLOW_MAX_RECEIVERS           L7_SFLOW_MAX_RECEIVERS
#define SFLOW_MAX_SAMPLERS            L7_SFLOW_MAX_SAMPLERS
#define SFLOW_MAX_POLLERS             L7_SFLOW_MAX_POLLERS

#define SFLOW_SAMPLE_INGRESS_REASON_CODE L7_MBUF_RX_SAMPLE_SOURCE
#define SFLOW_SAMPLE_EGRESS_REASON_CODE  L7_MBUF_RX_SAMPLE_DEST

#define SFLOW_FLOW_RECORDS_PER_SAMPLE    1
#define SFLOW_FLOW_DATA_PER_SAMPLE       1
#define SFLOW_COUNTER_RECORDS_PER_SAMPLE 2
#define SFLOW_PDU_RECORDS_PER_SAMPLE     1
#define SFLOW_SAMPLE_FRAME_FCS_LEN       4 /* CRC32 */

#define SFLOW_EVENT_QUEUE       "sflowEventQueue"
#define SFLOW_EVENTQ_MSG_COUNT  DEFAULT_MSG_COUNT

#define SFLOW_PACKET_QUEUE      "sflowPacketQueue"
#define SFLOW_PACKETQ_MSG_COUNT 1024
#define SFLOW_PACKET_BUFF_NAME  "sflow sample"

/* config Files sFlow */
#define SFLOW_CFG_FILENAME      "sflow.cfg"
#define SFLOW_CFG_VER_1         0x1
#define SFLOW_CFG_VER_CURRENT   SFLOW_CFG_VER_1

/* Debug Files sFlow */
#define SFLOW_DEBUG_CFG_FILENAME          "sflow_debug.cfg"
#define SFLOW_DEBUG_CFG_VER_1              0x1
#define SFLOW_DEBUG_CFG_VER_CURRENT        SFLOW_DEBUG_CFG_VER_1

#define SFLOW_TASK "sFlowTask"


#define SFLOW_TIMER_INTERVAL 1000 /* milliseconds */

#define SFLOW_CHOKE_THRESHOLD 20 /* Number of samples that can 
                                    be dropped continuously before 
                                    sampling rate throttling is done */
#define SFLOW_CONGESTION_ALERT_TIMEOUT 2 /* Wait time in seconds  
                                            before start taking note of
                                            CHOKE signals */

#define SFLOW_VAL_TO_DS_INDEX(x,y) ((x)|(((y) << 24) & 0xFF000000))
#define SFLOW_DS_INDEX_TO_VAL(x)   ((x) & 0x00FFFFFF)

/* PDU Write helpers */

#define SFLOW_PUT_32(val, cp) \
  do { \
       (val) = osapiHtonl((val));\
       memcpy((cp), &(val), sizeof(L7_uint32));\
       (cp) += sizeof(L7_uint32); \
     } while (0)

#define SFLOW_PUT_64(val, cp) \
  do { \
       (val) = osapiHtonll((val));\
       memcpy((cp), &(val), sizeof(L7_uint64));\
       (cp) += sizeof(L7_uint64); \
     } while (0)

#define SFLOW_PUT_DATA(data, len, cp) (memcpy (cp, data, len), (cp) += (len))
#endif /* SFLOW_DEFS_H  */
