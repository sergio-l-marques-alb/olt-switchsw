/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  sFlow_cnfgr.h
*
* @purpose   sFlow configurator interface definitions
*
* @component sFlow
*
* @comments  none
*
* @create    22-Nov-2007
*
* @author    drajendra
*
* @end
*             
**********************************************************************/

#ifndef SFLOW_CNFGR_H
#define SFLOW_CNFGR_H

#include "l7_cnfgr_api.h"
#include "l7_resources.h" 
#include "nimapi.h"

#include "sflow_defs.h"
#define SFLOW_IS_READY (((sFlowCnfgrState == SFLOW_PHASE_INIT_3) || \
                         (sFlowCnfgrState == SFLOW_PHASE_EXECUTE) || \
                         (sFlowCnfgrState == SFLOW_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))


#define FD_CNFGR_SFLOW_DEFAULT_STACK_SIZE     L7_DEFAULT_STACK_SIZE
#define FD_CNFGR_SFLOW_DEFAULT_TASK_PRIORITY  L7_DEFAULT_TASK_PRIORITY
#define FD_CNFGR_SFLOW_DEFAULT_TASK_SLICE     L7_DEFAULT_TASK_SLICE

typedef enum
{
  SFLOW_PHASE_INIT_0 = 0,
  SFLOW_PHASE_INIT_1,
  SFLOW_PHASE_INIT_2,
  SFLOW_PHASE_WMU,
  SFLOW_PHASE_INIT_3,
  SFLOW_PHASE_EXECUTE,
  SFLOW_PHASE_UNCONFIG_1,
  SFLOW_PHASE_UNCONFIG_2,
} sFlowCnfgrState_t;

typedef enum
{
  SFLOW_CNFGR_MSG = 0,
  SFLOW_INTF_EVENT,
  SFLOW_TIMER_TICK,
  SFLOW_SAMPLE_RX,
  SFLOW_OWNER_STRING_TIMEOUT_SET,
  SFLOW_SAMPLING_RATE_SET,
  SFLOW_POLL_INTERVAL_SET,
  SFLOW_COUNTER_RCVR_SET,
  SFLOW_SAMPLER_RCVR_SET,
  SFLOW_CHOKE,
  SFLOW_RCVR_ADDR_SET,
  SFLOW_NIM_STARTUP_EVENT

} sFlowMessages_t;

typedef struct sflowIntfChangeParms_s
{
  L7_uint32 event;
  NIM_CORRELATOR_t correlator;
} sflowIntfChangeParms_t;

typedef struct
{
  L7_uint32        msgId;    /* Of type sFlowMessages_t */
  union {
    L7_CNFGR_CMD_DATA_t     cmdData;
    sflowIntfChangeParms_t  sflowIntfChangeParms;
    L7_uchar8               stringVal[L7_SFLOW_OWNER_STRING_LEN];
    L7_inet_addr_t          addrVal;
    NIM_STARTUP_PHASE_t     nimStartupPhase;
  } u;

  L7_uint32        uintVal;
  L7_uint32        dataSource;
  L7_uint32        sflowInstance;
  L7_uint32        receiverIndex;
} sFlowEventMsg_t;
#define SFLOW_MSG_SIZE sizeof(sFlowEventMsg_t)

/* Snoop PDU Message format */
typedef struct sflow_Msg_s
{
  L7_uint32        msgId;     /* Of type sFlowMessages_t = SFLOW_SAMPLE_RX */
  L7_uint32        dsIndex ;  /*data source on which PDU was received */
  L7_uint32        instance ; /*instance of data source on which PDU was received */
  L7_uint32        direction; /*Direction in which PDU was sampled */
  L7_uchar8       *sflowSampleBuffer; /* Pointer to the received PDU */
  L7_uint32        dataLength;  /* Length of sampled PDU */
} sflowPDU_Msg_t;
#define SFLOW_PDU_MSG_SIZE     sizeof(sflowPDU_Msg_t)

/* Configurator event handing routines */
void sFlowApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);  
void sFlowCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t sFlowCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t * pResponse,
                                    L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t sFlowCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                    L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t sFlowCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t * pResponse,
                                    L7_CNFGR_ERR_RC_t * pReason);                                   
void sFlowCnfgrFiniPhase1Process();                         
void sFlowCnfgrFiniPhase2Process();                         
void sFlowCnfgrFiniPhase3Process();                         
L7_RC_t sFlowCnfgrNoopProccess(L7_CNFGR_RESPONSE_t * pResponse,
                               L7_CNFGR_ERR_RC_t * pReason);
L7_RC_t sFlowCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t * pResponse,
                                L7_CNFGR_ERR_RC_t * pReason);

L7_BOOL sFlowIsReady(void);
#endif /* SFLOW_CNFGR_H */
