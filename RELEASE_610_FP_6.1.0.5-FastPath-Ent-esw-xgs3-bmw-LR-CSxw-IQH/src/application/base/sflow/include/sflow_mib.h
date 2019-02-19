/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_proto.h
*
* @purpose   SFlow standard MIB definitions
*
* @component sflow
*
* @comments  This section is taken from sFlow v5 specification seciton
*            4. sFlow MIB
*
* @create 20-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#ifndef SFLOW_MIB_H
#define SFLOW_MIB_H

#include "l3_addrdefs.h"
#include "datatypes.h"
#include "avl_api.h"
#include "l7apptimer_api.h"
#include "l7handle_api.h"

#include "sflow_defs.h"
#include "sflow_cnfgr.h"
#include "sflow_proto.h"
struct SFLOW_agent_s;

/* sFlowAgent mib group */
typedef struct SFLOW_agent_mib_s
{
/* MIB fields */
  L7_uint32      sFlowVersion;      /* sFlow version supported by the agent */
  L7_uint32      sFlowAddressType;  /* Inband Management IP address type */
  L7_inet_addr_t sFlowAgentAddress; /* Inband Management IP address */
} SFLOW_agent_mib_t;

/* Receiver Table - sFlowRcvrTable */
typedef struct SFLOW_receiver_mib_s
{
/* MIB fields */
  L7_uint32       sFlowRcvrIndex; /* uniquely identifies a receiver record */
  L7_uchar8       sFlowRcvrOwner[L7_SFLOW_OWNER_STRING_LEN]; /* SNMP manager
                  sets it to a non null value to take ownership of the
                  receiver */
  L7_uint32       sFlowRcvrTimeout; /* This value is topped up by SNMP manager
                                       before it reaches zero */
  L7_uint32       sFlowRcvrMaxDatagramSize;
  L7_uint32       sFlowRcvrAddressType; /* Collector network address type */
  L7_inet_addr_t  sFlowRcvrAddress;     /* Collector network address */
  L7_uint32       sFlowRcvrPort;        /* Collector Layer 4 port */
  L7_uint32       sFlowRcvrDatagramVersion; /* Collector datagram version = 5 */
/* Non Standard parameters */
  L7_uint32       sFlowRcvrSnmpMode;    /* Receiver doesnt expire when disabled */
} SFLOW_receiver_mib_t;

/* sFlow Receiver runtime instance */
typedef struct SFLOW_receiver_s
{
  SFLOW_receiver_mib_t  *rcvrCfg;
                  /* sFlow datagram pay load */
  L7_uchar8       pduBuff[SFLOW_MAX_DATAGRAM_SIZE]; 
  L7_uchar8      *ptrPduBuff; /* packet fill pointer */
  L7_uint32       pduLen;     /* accumulated size - PDU is sent out when 
                                 pduLen comes close to 
                                 min(sFlowRcvrMaxDatagramSize, 
                                     SFLOW_MAX_DATAGRAM_SIZE) */

  SFLOW_sample_datagram_hdr_t sFlowDatagram;
  L7_uint32       datagramHdrLen; /* Length of the sFlow dgram header
                                       = 40Bytes IPv6 receiver
                                       = 28Bytes IPv4 receiver */
} SFLOW_receiver_t;

/* Flow Sampling Table - sFlowFsTable */
typedef struct SFLOW_sampler_mib_s
{
  L7_uint32       sFlowFsDataSource; /* This field will contain intIfNum of datasource
                                        it will be converted to OID by the
                                        SNMP APIs before sending SNMP Response
                                        to the manager.*/
  L7_uint32       sFlowFsInstance;   /* Currently only one instance is supported for
                                        each data source */
  L7_uint32       sFlowFsReceiver;   /* sFlowRcvrIndex value */
  L7_uint32       sFlowFsPacketSamplingRate; /* specifies N where "1 in N packets"
                                                are sampled */
  L7_uint32       sFlowFsMaximumHeaderSize;  /* specifies the bytes in the sampled
                                                packet to be extracted */

} SFLOW_sampler_mib_t;

/* sFlow sampler runtime instance */

/* AVL Tree Sampler Entry Strucutre */
typedef struct SFLOW_sampler_DataKey_s
{
  L7_uint32 dsIndex;
  L7_uint32 sFlowInstance;
} SFLOW_sampler_DataKey_t;

typedef struct SFLOW_sampler_s
{
  SFLOW_sampler_DataKey_t key;
  SFLOW_sampler_mib_t   *samplerCfg; /* Back pointer to cfg */
  L7_uint32              flowSampleSeqNo;
  L7_uint32              sample_pool;
  L7_uint32              drops;
  void               *next;
} SFLOW_sampler_t;

/* Counter Polling Table - sFlowFsTable */
typedef struct SFLOW_poller_mib_s
{
  L7_uint32       sFlowCpDataSource; /* This field will contain intIfNum of datasource
                                        it will be converted to OID by the
                                        SNMP APIs before sending SNMP Response
                                        to the manager.*/
  L7_uint32       sFlowCpInstance;   /* Currently only one instance is supported for
                                        each data source */
  L7_uint32       sFlowCpReceiver;   /* sFlowRcvrIndex value */
  L7_uint32       sFlowCpInterval;   /* collect a counter sample value when timer expires */

} SFLOW_poller_mib_t;

/* sFlow poller runtime instance */

/* AVL Tree Poller Entry Strucutre */
typedef struct SFLOW_poller_DataKey_s
{
  L7_uint32 dsIndex;
  L7_uint32 sFlowInstance;
} SFLOW_poller_DataKey_t;

typedef struct SFLOW_poller_timerData_s
{
  L7_APP_TMR_HNDL_t      ctrPollTimer;
  L7_uint32              ctrPollTimerHandle;
} SFLOW_poller_timerData_t;

typedef struct SFLOW_poller_s
{
  SFLOW_poller_DataKey_t key;
  SFLOW_poller_mib_t    *pollerCfg;
/* Internal fields */
  L7_uint32                   counterSampleSeqNo;
  SFLOW_poller_timerData_t    timerData;

  void               *next;
} SFLOW_poller_t;

/* Persistent Storage Configuration Structure Format */
typedef struct SFLOW_cfgData_s
{
  L7_fileHdr_t       cfgHdr; /* Config file header */

  /* MIB Groups */
  SFLOW_receiver_mib_t receiverGroup[SFLOW_MAX_RECEIVERS];
  SFLOW_sampler_mib_t  samplerGroup[SFLOW_MAX_SAMPLERS];
  SFLOW_poller_mib_t   pollerGroup[SFLOW_MAX_POLLERS];

  L7_uint32          checkSum;     /* check sum of config file NOTE: needs to be last entry */
} SFLOW_cfgData_t;

/* Debug Configuration Data */
typedef struct sFlowDebugCfgData_s
{
  L7_BOOL sFlowDebugPacketTraceTxFlag;
} sFlowDebugCfgData_t;

/* Debug Configuration File Structure */
typedef struct sFlowDebugCfg_s
{
  L7_fileHdr_t          hdr;
  sFlowDebugCfgData_t   cfg;
  L7_uint32             checkSum;
} sFlowDebugCfg_t;

/* sFlowAgent runtime instance */
typedef struct SFLOW_agent_s
{
/* Configuration Data */
  SFLOW_cfgData_t     *sFlowCfg;

  /* Debug info */
  sFlowDebugCfg_t    *sFlowDebugCfg;   /* Persistent Debug Data */
  L7_BOOL             sFlowDebugPacketTraceTxFlag; /* Volatile Debug Data */

/* agent information */
  SFLOW_agent_mib_t    agentGroup;

/* Buffer pools, Semaphores, msgQueues, 
   sockets, taskIDs, timerCBs...*/
  L7_uint32            sFlowTaskHandle;

/* The sFlow thread reads from two queues. sFlow_Event_Queue
 * holds events to be processed. sFlow_Packet_Queue holds incoming
 * sFlow packets. */
  void                *sFlowMsgQSema;
  void                *sFlowEventQueue;
  void                *sFlowPacketQueue;
  
  sFlowCnfgrState_t    sFlowCnfgrState;
  osapiRWLock_t        sFlowCfgRWLock;

/* Receivers, Samplers, Pollers runtime instances */
  /* sampled pkt holder buffer pool */
  L7_uint32           sflowBufferPool;
  /* Buffer pool to hold timers */
  L7_uint32           appTimerBufferPoolId;
  /* Sampler AVL Tree data */
  avlTree_t           samplerAvlTree;
  avlTreeTables_t    *samplerTreeHeap;
  SFLOW_sampler_t    *samplerDataHeap;

  /* poller AVL Tree data */
  avlTree_t           pollerAvlTree;
  avlTreeTables_t    *pollerTreeHeap;
  SFLOW_poller_t     *pollerDataHeap;

  SFLOW_receiver_t    *receiver; 

  /* Socket to send datagrams to collector */
  L7_uint32            sFlowAgentv4Socket; 
  L7_uint32            sFlowAgentv6Socket;

  /* Timer Data */
  L7_APP_TMR_CTRL_BLK_t   timerCB;  /* sFlow App Timer Control Block */
  handle_list_t          *handle_list;
  void                   *handleListMemHndl;

  L7_uint32               numActivePollers;
  L7_uint32               numActiveSamplers;
  L7_uint32               congestionAlert;
} SFLOW_agent_t;
#endif /* SFLOW_MIB_H */
