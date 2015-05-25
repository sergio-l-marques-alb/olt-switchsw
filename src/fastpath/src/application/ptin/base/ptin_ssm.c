
/*********************************************************************
*
* (C) Copyright PT Inovacao 2012
*
**********************************************************************
*
* @filename    ssm_main.c
* @purpose     SSM main functions
* @component   SSM
* @comments    none
* @create      26/11/2012
* @author      mruas
* @end
*             
**********************************************************************/

#include "ptin_ssm.h"

#ifdef SYNC_SSM_IS_SUPPORTED
#include <unistd.h>
#include "defaultconfig.h"
#include "l7_packet.h"
#include "nimapi.h"
#include "simapi.h"
#include "dtlapi.h"
#include "buff_api.h"
#include "ptin_intf.h"
#include "ptin_control.h"
#include "fw_shm.h"
#include "logger.h"

/* Buffer pool */
#define SSM_NUM_BUFFERS     100
#define SSM_BUFFER_SIZE		((SSM_PDU_DATALENGTH+4)*2)  /* Add 4 bytes for FCS, and duplicate maximum size */
L7_uint32 ssmBufferPoolId = L7_NULL;

/* Message queue */
#define SSM_MSG_QUEUE_SIZE  sizeof(ssmMsg_t)
#define SSM_MSG_QUEUE_COUNT DEFAULT_MSG_COUNT
void *ssm_queue = L7_NULLPTR;                /* reference to the SSM message queue */


/* PTin task loop period */
#define SSM_LOOP_TICK                (1000) /* in milliseconds */

static L7_uint32 ssm_loop_handle = 0;  /* periodic timer handle */

/* Task ids */
L7_uint32 ssm_task_rx_id = L7_ERROR;
L7_uint32 ssm_task_tx_id = L7_ERROR;

/* Semaphores */
void *ssmTaskRxSyncSema = L7_NULLPTR;
void *ssmTaskTxSyncSema = L7_NULLPTR;
void *ssmTimersSyncSema = L7_NULLPTR;

/* Statistics */
typedef struct ssm_stats_s
{
  L7_uint32 PDUsRx;
  L7_uint32 UnknownRx;
  L7_uint32 IllegalRx;
  L7_uint32 PDUsTx;
  L7_uint32 RxPDUsDropped;
} ssm_stats_t;
static ssm_stats_t ssm_stats[L7_MAX_INTERFACE_COUNT];

/* Tbl 43B-1 */
const L7_uchar8 ssmSlowProtocolsMulticastMacAddr[L7_MAC_ADDR_LEN] = {0x01,0x80,0xc2,0x00,0x00,0x02};

/* Internal functions */
void ssm_task_pdu_transmit( void );
void ssm_task_pdu_receive( void );
L7_RC_t ssmPduInit(void);
L7_RC_t ssmPDUReceive(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);
L7_RC_t ssmPDUProcess(L7_uint32 intf, void *buffer);
void ssmPDUSend(void);
L7_RC_t ssmPDUTransmit(L7_uint32 intIfNum);
L7_RC_t ssmTransmit(L7_uint32 intIfNum, L7_uint32 bufHandle);
L7_RC_t ssmPduHeaderTagRemove(L7_netBufHandle bufHandle);

L7_RC_t ssmCodesInit(void);
L7_RC_t ssmTimersUpdate(void);
L7_RC_t ssmCodeUpdate(L7_uint32 intIfNum, L7_uint16 ssm_code);


L7_BOOL ssm_debug_enable = 0;

void ptin_debug_ssm_enable(L7_BOOL enable)
{
  ssm_debug_enable = enable & 1;
}

#if (PTIN_BOARD_IS_MATRIX)
 #define SHMEM(slot,intf)   pfw_shm->intf[slot][intf]
#else
 #define SHMEM(slot,intf)   pfw_shm->intf[intf]
#endif

#endif


/**************************************************************************
*
* @purpose   Allocate all necessary SSM resources
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     Initialization routine
*         
*
* @end
*
*************************************************************************/
L7_RC_t ssm_init(void)
{
#ifdef SYNC_SSM_IS_SUPPORTED
  /* Initialize statistics */
  memset(ssm_stats,0x00,sizeof(ssm_stats));

  /* Initialize buffer pool for incoming PDUs */
  if (bufferPoolInit(SSM_NUM_BUFFERS, SSM_BUFFER_SIZE, "SSM Bufs",
                     &ssmBufferPoolId) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Error allocating buffers!");
    return L7_FAILURE;
  }

  /* create the semaphores */
  ssmTimersSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ssmTimersSyncSema == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Unable to create ssmTimersSyncSema semaphore()");
    return(L7_FAILURE);
  }
  /*semaphore creation for task protection over the common data*/
  ssmTaskRxSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ssmTaskRxSyncSema == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Unable to create ssm rx task semaphore()");
    return(L7_FAILURE);
  }
  ssmTaskTxSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ssmTaskTxSyncSema == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Unable to create ssm tx task semaphore()");
    return(L7_FAILURE);
  }

  /* Create SSM tasks */
  if ( (ssm_task_rx_id = osapiTaskCreate("SSM rx task", ssm_task_pdu_receive, 0, 0,
                                         L7_DEFAULT_STACK_SIZE,
                                         L7_DEFAULT_TASK_PRIORITY,
                                         L7_DEFAULT_TASK_SLICE)) == L7_ERROR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create SSM RX task!");
    return L7_FAILURE;
  }
  /* Wait for task to be launched */
  if (osapiWaitForTaskInit (L7_SSM_RX_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to start SSM RX task!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_PTIN_CNFGR, "SSM RX task launch OK");

  if ( (ssm_task_tx_id = osapiTaskCreate("SSM tx task", ssm_task_pdu_transmit, 0, 0,
                                         L7_DEFAULT_STACK_SIZE,
                                         L7_DEFAULT_TASK_PRIORITY,
                                         L7_DEFAULT_TASK_SLICE)) == L7_ERROR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create SSM TX task!");
    return L7_FAILURE;
  }
  /* Wait for task to be launched */
  if (osapiWaitForTaskInit (L7_SSM_TX_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to start SSM TX task!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_PTIN_CNFGR, "SSM TX task launch OK");

  /* Initialize shared memory and internal structures */
  ssmCodesInit();
#endif

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose   Remove all related SSM resources
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     Deinitialization routine
*         
*
* @end
*
*************************************************************************/
L7_RC_t ssm_fini(void)
{
#ifdef SYNC_SSM_IS_SUPPORTED

  /* Close shared memory zone */
  fw_shm_close();

  /* Remove task */
  if (ssm_task_rx_id != L7_ERROR)
  {
    osapiTaskDelete(ssm_task_rx_id);
    ssm_task_rx_id = L7_ERROR;
  }
  if (ssm_task_tx_id != L7_ERROR)
  {
    osapiTaskDelete(ssm_task_tx_id);
    ssm_task_tx_id = L7_ERROR;
  }

  /* Delete semaphores */
  if (ssmTimersSyncSema != L7_NULLPTR)
  {
    osapiSemaDelete(ssmTimersSyncSema);
    ssmTimersSyncSema = L7_NULLPTR;
  }
  if (ssmTaskRxSyncSema != L7_NULLPTR)
  {
    osapiSemaDelete(ssmTaskRxSyncSema);
    ssmTaskRxSyncSema = L7_NULLPTR;
  }
  if (ssmTaskTxSyncSema != L7_NULLPTR)
  {
    osapiSemaDelete(ssmTaskTxSyncSema);
    ssmTaskTxSyncSema = L7_NULLPTR;
  }

  /* Delete message queue */
  if (ssm_queue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(ssm_queue);
    ssm_queue = L7_NULLPTR;
  }

  /* Release buffer pools */
  if (ssmBufferPoolId != L7_NULL)
  {
    bufferPoolDelete(ssmBufferPoolId);
    ssmBufferPoolId = L7_NULL;
  }
#endif

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Receives an SSM PDU
*
* @param    intf            interface number received on
* @param    bufHandle       handle to the PDU location in DTL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if port not found
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ssmPDUReceive(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
#ifdef SYNC_SSM_IS_SUPPORTED

  L7_uchar8  *data;
  L7_uint16   etherType;
  L7_uchar8   subType;
  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32   intIfNum;
  L7_uchar8  *buffer = L7_NULLPTR;
  L7_uint32   dataLength = 0;
  //ssm_pdu_t  *ssm_pdu;
  ssmMsg_t    msg;
  L7_int      offset;

  /* Use physical port */
  intIfNum = pduInfo->intIfNum;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /*check if this PDU is meant for SSM by checking the DA*/
  if (memcmp(data, ssmSlowProtocolsMulticastMacAddr, L7_MAC_ADDR_LEN) != 0)
  {
    //SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  offset = 0;
  /* Outer vlan? */
  memcpy(&etherType, &data[SSM_ETHTYPE_OFFSET+offset], sizeof(L7_uint16));
  etherType = osapiNtohs(etherType);

  if (etherType==0x8100 || etherType==0x88a8 || etherType==0x9100)
  {
    offset += 4;

    /* Inner vlan? */
    memcpy(&etherType, &data[SSM_ETHTYPE_OFFSET+offset], sizeof(L7_uint16));
    if (etherType==0x8100 || etherType==0x88a8 || etherType==0x9100)
    {
      offset += 4;
    }
  }

  /*remove any header tagging*/
  //rc = ssmPduHeaderTagRemove(bufHandle);

  //SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /* Map packet fields */
  //ssm_pdu = (ssm_pdu_t *) data;

  /*check to see if the PDU carries the slow protocols type field of 0x8809*/
  memcpy(&etherType, &data[SSM_ETHTYPE_OFFSET + offset], sizeof(L7_uint16));
  etherType = osapiNtohs(etherType);

  if (etherType != SSM_SLOW_PROTOCOL_TYPE)
  {
    ssm_stats[intIfNum].UnknownRx++;
    //SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  /* Verify slow protocol subtype */
  memcpy(&subType, &data[SSM_SUBTYPE_OFFSET + offset], sizeof(L7_uchar8));
  if (subType != SSM_PROTOCOL_SUBTYPE)
  {
    if (subType == SSM_ILLEGAL_SUBTYPE ||
        subType >= SSM_ILLEGAL_SUBTYPE_MIN)
    { /*increment counter for illegal packet*/
      ssm_stats[intIfNum].IllegalRx++;
      //SYSAPI_NET_MBUF_FREE(bufHandle);
      return L7_FAILURE;
    }

    ssm_stats[intIfNum].UnknownRx++;
    //SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  /*increment counter*/
  ssm_stats[intIfNum].PDUsRx++;

  /* Good packet at the moment */

  if (ssm_debug_enable)
  {
    LOG_TRACE(LOG_CTX_PTIN_SSM,"Packet from intIfNum %u, rxPort %u", pduInfo->intIfNum, pduInfo->rxPort);
  }

  /* Remove tags */
  if (ssmPduHeaderTagRemove(bufHandle)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Error extracting vlans");
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_SUCCESS;
  }

  /* Get new data and data_length without tags */
  SYSAPI_NET_MBUF_GET_DATASTART (bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);

  /* Allocate buffer wich will store the packet data */
  if ((bufferPoolAllocate(ssmBufferPoolId, &buffer) != L7_SUCCESS) ||
      (dataLength > SSM_BUFFER_SIZE))
  {
    /* increment  appropriate statistics */
    ssm_stats[intIfNum].RxPDUsDropped++;
    LOG_ERR(LOG_CTX_PTIN_SSM,"Error allocating buffer");
    /* consume packet and return success*/
    bufferPoolFree(ssmBufferPoolId,  buffer);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_SUCCESS;
  }
  /* We have a good buffer, copy the incoming mbuf into our buffer */
  memcpy(buffer, data, dataLength);

  /* Now free the mbuf */
  SYSAPI_NET_MBUF_FREE(bufHandle);

  /*send msg to LAC queue with lacpPduRx and bufHandle and the port 
  it was received in*/
  memset(&msg,0x00,sizeof(ssmMsg_t));
  msg.event = 1;
  msg.intf = intIfNum;
  msg.intfData.bufHandle = buffer;

  rc = osapiMessageSend(ssm_queue, &msg, (L7_uint32)SSM_MSG_QUEUE_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if ( rc != L7_SUCCESS )
  {
    bufferPoolFree(ssmBufferPoolId,  buffer);
    /* By returning a success we ensure that this mbuf is not freed 
       again by sysnet*/
    LOG_ERR(LOG_CTX_PTIN_SSM,"Error in processing pdu (%d)", rc);
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
#else
  return L7_FAILURE;
#endif
}

/*********************************************************************
* @purpose  Decodes and services an incoming request from the stats
*           manager for a SSM statistic
*
* @param    c           Pointer to storage allocated by stats
                        manager to hold the results
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Only called by stats manager
*       
* @end
*********************************************************************/
void ssmStatGet(void *statHandle)
{
}

/*********************************************************************
* @purpose  Clears the port statistics. 
*          
* @param    intIfNum @b{(input)} Internal Interface Number of the port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Clears all the ssm PDU stats associated with this port. 
*
* @end
*********************************************************************/
L7_RC_t ssmPortStatsClear(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


#ifdef SYNC_SSM_IS_SUPPORTED
/*********************************************************************
* @purpose  SSM task which serves the SSM reception
*
* @param    
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void ssm_task_pdu_receive()
{
  ssmMsg_t msg;
  L7_uint32 status;

  ssm_queue = (void*)osapiMsgQueueCreate("ssm_queue", SSM_MSG_QUEUE_COUNT, SSM_MSG_QUEUE_SIZE);

  if (osapiTaskInitDone(L7_SSM_RX_TASK_SYNC) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  if (ssm_queue == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Unable to create msg queue for ssm_task!");
    return;
  }

  /* Register callback to receive messages! */
  ssmPduInit();

  while (1)
  {

    status = (L7_uint32) osapiMessageReceive(ssm_queue,
                                             (void*)&msg,
                                             (L7_uint32)SSM_MSG_QUEUE_SIZE,
                                             L7_WAIT_FOREVER);

    /* Process message */
    osapiSemaTake(ssmTaskRxSyncSema, L7_WAIT_FOREVER);
    ssmPDUProcess(msg.intf, msg.intfData.bufHandle);
    osapiSemaGive(ssmTaskRxSyncSema);
  }
}


/*********************************************************************
* @purpose  SSM task which serves the SSM transmission
*
* @param    
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void ssm_task_pdu_transmit( void )
{
  if (osapiTaskInitDone(L7_SSM_TX_TASK_SYNC) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  /* Register a period timer */
  if (osapiPeriodicUserTimerRegister(SSM_LOOP_TICK, &ssm_loop_handle) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Error registering period timer! CRASH!");
    PTIN_CRASH();
  }

  /* Loop */
  while (1) {
    osapiPeriodicUserTimerWait(ssm_loop_handle);

    /* Transmit PDU */
    osapiSemaTake(ssmTaskTxSyncSema, L7_WAIT_FOREVER);

    ssmPDUSend();

    /* Update slot/intf timers */
    ssmTimersUpdate();

    osapiSemaGive(ssmTaskTxSyncSema);
  }
}


/**************************************************************************
*
* @purpose   Init the LACPM  
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     register the receive function with sysnet
*         
*
* @end
*
*************************************************************************/
L7_RC_t ssmPduInit(void)
{
  L7_RC_t rc = L7_SUCCESS;
  #if 0
  sysnetNotifyEntry_t snEntry;

  /*Register the pdu receive function with sysnet utility*/
  strcpy(snEntry.funcName, "ssmPDUReceive");
  snEntry.notify_pdu_receive = ssmPDUReceive;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, ssmSlowProtocolsMulticastMacAddr, L7_MAC_ADDR_LEN);
  rc = sysNetRegisterPduReceive(&snEntry); 
  #endif

  return rc;
}

L7_uint16 ssm_timer[SSM_N_SLOTS][SSM_N_INTFS];


/*********************************************************************
* @purpose  Process an SSM message
*
* @param    intf            interface number received on
* @param    bufHandle       handle to the PDU location in DTL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if port not found
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ssmPDUProcess(L7_uint32 intf, void *buffer)
{
  L7_uint32 itu_oui;
  ssm_pdu_t *pdu;

  pdu = (ssm_pdu_t *) buffer;

  if (ssm_debug_enable)
  {
    LOG_TRACE(LOG_CTX_PTIN_SSM,"Packet received on interface %u:",intf);
    LOG_TRACE(LOG_CTX_PTIN_SSM," DMAC=%02x:%02x:%02x:%02x:%02x:%02x",pdu->dmac[0],pdu->dmac[1],pdu->dmac[2],pdu->dmac[3],pdu->dmac[4],pdu->dmac[5]);
    LOG_TRACE(LOG_CTX_PTIN_SSM," SMAC=%02x:%02x:%02x:%02x:%02x:%02x",pdu->smac[0],pdu->smac[1],pdu->smac[2],pdu->smac[3],pdu->smac[4],pdu->smac[5]);
    LOG_TRACE(LOG_CTX_PTIN_SSM," EtherType     = 0x%04x",osapiNtohs(pdu->etherType));
    LOG_TRACE(LOG_CTX_PTIN_SSM," SubType       = 0x%02x",pdu->subType);
    LOG_TRACE(LOG_CTX_PTIN_SSM," ITU-OUI       = 0x%02x%02x%02x",pdu->itu_oui[0],pdu->itu_oui[1],pdu->itu_oui[2]);
    LOG_TRACE(LOG_CTX_PTIN_SSM," ITU-T Subtype = 0x%04x",osapiNtohs(pdu->itut_subtype));
    LOG_TRACE(LOG_CTX_PTIN_SSM," Version       = 0x%02x",pdu->version);
    LOG_TRACE(LOG_CTX_PTIN_SSM," SSM Type      = 0x%02x",pdu->ssm_type);
    LOG_TRACE(LOG_CTX_PTIN_SSM," SSM Length    = 0x%04x",osapiNtohs(pdu->ssm_length));
    LOG_TRACE(LOG_CTX_PTIN_SSM," SSM Code      = 0x%02x",pdu->ssm_code);
    LOG_TRACE(LOG_CTX_PTIN_SSM,"End of Packet processing");
  }

  /* Validar restantes campos do campo */
  /* ITU-OUI */
  itu_oui = ((L7_uint32) pdu->itu_oui[0]<<16) | ((L7_uint32) pdu->itu_oui[1]<<8) | ((L7_uint32) pdu->itu_oui[2]);

  if (itu_oui != SSM_ITU_OUI)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"ITU-T field is not valid: received=0x%x, expected=0x%x",itu_oui,SSM_ITU_OUI);
    bufferPoolFree(ssmBufferPoolId,  buffer);
    return L7_FAILURE;
  }
  /* Protocol version */
  if ((pdu->version >> 4) != 1)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Protocol version is not the expected: received=%u, expected=1",pdu->version >> 4);
    bufferPoolFree(ssmBufferPoolId,  buffer);
    return L7_FAILURE;
  }
  /* SSM type */
  if (pdu->ssm_type!=1)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"SSM type is not the expected: received=%u, expected=1",pdu->ssm_type);
    bufferPoolFree(ssmBufferPoolId,  buffer);
    return L7_FAILURE;
  }
  /* SSM length must be one, or greater */
  if (osapiNtohs(pdu->ssm_length) < SSM_L4_LENGTH)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"SSM length is less than %u bytes (%u)",SSM_L4_LENGTH, osapiNtohs(pdu->ssm_length));
    bufferPoolFree(ssmBufferPoolId,  buffer);
    return L7_FAILURE;
  }

  /* Update slot/intf timers */
  ssmCodeUpdate(intf, pdu->ssm_code);

  /* Release now buffer */
  bufferPoolFree(ssmBufferPoolId,  buffer);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Transmit SSM messages
*
* @notes    
*       
* @end
*********************************************************************/
void ssmPDUSend(void)
{
  L7_uint32 intIfNum;
  L7_uint16 slot, intf;
  L7_RC_t   rc;

  //LOG_INFO(LOG_CTX_PTIN_SSM,"This is the place where i will transmit SSM messages!");

  for (slot=0; slot<SSM_N_SLOTS; slot++)
  {
    for (intf=0; intf<SSM_N_INTFS_IN_USE; intf++)
    {
      /* Check if tranmission is allowed for this slot/intf */
      if ( !((SHMEM(slot,intf).ssm_tx >> 16) & 1) )
        continue;

      /* Find the respective intIfNum, and proceed to tranmission */
      rc = ptin_intf_slotPort2IntIfNum(slot+1, intf, &intIfNum);

      if (rc == L7_SUCCESS)
      {
        ssmPDUTransmit(intIfNum);
      }
      else
      {
        //if (ssm_debug_enable)
        //  LOG_ERR(LOG_CTX_PTIN_SSM,"Invalid slot %u / intf %u... no tranmission executed",slot,intf);
      }
    }
  }
}

/**************************************************************************
*
* @purpose    Transmits a SSM message to an interface through dtl
*
* @param      intIfNum : Interface to send message
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t ssmPDUTransmit(L7_uint32 intIfNum)
{
  L7_netBufHandle bufHandle = L7_NULL;
  ssm_pdu_t *pdu;
  L7_uchar8 *data;
  L7_uint16 frameLength;
  L7_uint16 slot, intf;
  L7_uint32 itu_oui;
  L7_RC_t   rc;

  /* Convert port to slot/intf */
  rc = ptin_intf_intIfNum2SlotPort(intIfNum, &slot, &intf, L7_NULLPTR);

  if (rc != L7_SUCCESS)
  {
    //if (ssm_debug_enable)
    //  LOG_ERR(LOG_CTX_PTIN_SSM,"Cannot convert intIfNum %u to slot/intf",intIfNum);
    return L7_FAILURE;
  }

  if (slot >= SSM_N_SLOTS || intf >= SSM_N_INTFS_IN_USE)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Invalid slot/intf %u/%u", slot, intf);
    return L7_FAILURE;
  }

  /* Adjust slot value */
  if (slot>0)  slot--;

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Error initializing bufHandle");
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, SSM_PDU_DATALENGTH);
  pdu = (ssm_pdu_t *)data;
  frameLength = SSM_PDU_DATALENGTH;

  memset(pdu,0,sizeof(ssm_pdu_t));

  /* Destination address */
  memcpy(pdu->dmac, ssmSlowProtocolsMulticastMacAddr, L7_MAC_ADDR_LEN);

  /* Source address */
  if (nimGetIntfAddress(intIfNum, L7_NULL, pdu->smac)!=L7_SUCCESS)
  {
    if (ssm_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_SSM,"Error getting MAC address of intIfNum %u",intIfNum);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_SUCCESS;
  }
  //simGetSystemMac(pdu->smac);

  /* Ethertype */
  pdu->etherType = osapiHtons(SSM_SLOW_PROTOCOL_TYPE);

  /* Subtype */
  pdu->subType = SSM_PROTOCOL_SUBTYPE;

  /* ITU-OUI */
  itu_oui = osapiHtonl(SSM_ITU_OUI);
  pdu->itu_oui[0] = (SSM_ITU_OUI>>16) & 0xff;
  pdu->itu_oui[1] = (SSM_ITU_OUI>>8 ) & 0xff;
  pdu->itu_oui[2] = (SSM_ITU_OUI    ) & 0xff;

  /* ITU-T Subtype */
  pdu->itut_subtype = osapiHtons(SSM_ITU_SUBTYPE);

  /* Version */
  pdu->version = SSM_VERSION;

  /* SSM type */
  pdu->ssm_type = SSM_L4_TYPE;

  /* SSM length */
  pdu->ssm_length = osapiHtons(SSM_L4_LENGTH);

  /* SSM code */
  pdu->ssm_code = SHMEM(slot,intf).ssm_tx & 0x0f; // 0x0f;

  /* Insert Outer Vlan with VLAN 1 */
#if (PTIN_BOARD_IS_GPON)
  memmove(&data[16], &data[12], frameLength - 12);

  data[12] = (FD_DVLANTAG_802_1Q_ETHERTYPE >> 8) & 0xff;
  data[13] = FD_DVLANTAG_802_1Q_ETHERTYPE & 0xff;
  data[14] = 0x00;
  data[15] = 0x01;

  frameLength += 4;
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frameLength);
#endif

  /*FCS calculation*/
  /*done by lower layers*/

  if (ssmTransmit(intIfNum, bufHandle)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Error transmiting packet");
    return L7_FAILURE;
  }

  if (ssm_debug_enable)
    LOG_TRACE(LOG_CTX_PTIN_SSM,"SSM transmitted to intIfNum %u (slot=%u/intf=%u) with SSM_code=%x (length=%u)",
              intIfNum, slot, intf, SHMEM(slot,intf).ssm_tx, frameLength);

  return L7_SUCCESS;
}


/**************************************************************************
*
* @purpose   Transmit a pdu to DTL  
*
* @param     intIfNum     the internal interface number the PDU is to be sent through
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes     transmits both LACPDU and Marker Response pdu
*         
*
* @end
*
*************************************************************************/
L7_RC_t ssmTransmit(L7_uint32 intIfNum, L7_uint32 bufHandle)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_NIM_QUERY_DATA_t nimQueryData;
  DTL_CMD_TX_INFO_t dtlCmdInfo;
  L7_uint32 adminState, linkState;
  L7_INTF_TYPES_t intfType;
 
  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
  {
    /* should never get here */
    //LOG_ERR(LOG_CTX_PTIN_SSM,"nimIntfQuery error (intIfNum=%u)",intIfNum);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }
  else if (/*(nimQueryData.data.state != L7_INTF_ATTACHING) &&
           (nimQueryData.data.state != L7_INTF_DETACHING) &&*/
           (nimQueryData.data.state != L7_INTF_ATTACHED))
  {
    /* HW not present, free the buffer and return success */
    //if (ssm_debug_enable)
    //  LOG_ERR(LOG_CTX_PTIN_SSM,"intIfNum %u is not yet attached!",intIfNum);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_SUCCESS;
  }

  /* Get interface status */
  if (nimGetIntfType(intIfNum,&intfType)!=L7_SUCCESS ||
      nimGetIntfAdminState(intIfNum,&adminState)!=L7_SUCCESS ||
      nimGetIntfLinkState(intIfNum,&linkState)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Error getting intIfNum %u status!",intIfNum);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  /* Interface must be physical */
  if (intfType!=L7_PHYSICAL_INTF)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"intIfNum %u is not physical",intIfNum);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_SUCCESS;
  }
  /* Interface must be a physical port */
  if (adminState!=L7_ENABLE || linkState!=L7_UP)
  {
    //if (ssm_debug_enable)
    //  LOG_ERR(LOG_CTX_PTIN_SSM,"intIfNum %u not ready",intIfNum);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_SUCCESS;
  }

  /* only talk to the hardware when the hardware is valid */

  /*send it back to DTL for transmit*/
  bzero((char *)&dtlCmdInfo,(L7_int32)sizeof(DTL_CMD_TX_INFO_t));
  dtlCmdInfo.intfNum= intIfNum;
  dtlCmdInfo.priority = 1;
  dtlCmdInfo.typeToSend = DTL_L2RAW_UNICAST;

  rc = dtlPduTransmit( bufHandle, DTL_CMD_TX_L2, &dtlCmdInfo);

  if (rc == L7_SUCCESS)
  {
    ssm_stats[intIfNum].PDUsTx++;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Error transmiting packet to intIfNum %u",intIfNum);
  }

  if (ssm_debug_enable)
    LOG_TRACE(LOG_CTX_PTIN_SSM,"Packet transmitted successfully to intIfNum %u",intIfNum);

  return rc;
}


/**************************************************************************
*
* @purpose   Remove any tagging that may be there between the src addr and the lengthType field  
*
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     assumes that PDU is meant for dot3ad
*         
*
* @end
*
*************************************************************************/
L7_RC_t ssmPduHeaderTagRemove(L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_uchar8 *strippedData;
  L7_int32 dataLength;
  L7_int32 offset = 0;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);

  offset = sysNetDataOffsetGet(data);
/*subtract two bytes as we need the lenghtType field*/
/*subtract 12 bytes for the mac addrrs*/
  offset = offset - L7_ENET_HDR_SIZE - 2;

  if (offset > 0)
  {
    /* adjust frame start and length */
    strippedData = &data[offset];
    SYSAPI_NET_MBUF_SET_DATASTART(bufHandle, strippedData);
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength - offset);

    /* move the data */
    for (offset = (L7_ENET_HDR_SIZE - 1); offset >= 0; offset--) 
      strippedData[offset] = data[offset];
  }
  return L7_SUCCESS;
}

/**
 * Initialize shared memory zone, and internal variables
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t ssmCodesInit(void)
{
  /* Initialize shared memory */
  if (pfw_shm!=L7_NULLPTR)
  {
    /* Initialize data structure */
    memset(pfw_shm,0x00,sizeof(t_fw_shm));
  }

  /* Initialize timers */
  memset(ssm_timer,0x00,sizeof(ssm_timer));

  return L7_SUCCESS;
}

/**
 * Update timers of all slots and interfaces. 
 * This routine must be called once every second. 
 * For slots/intfs with timers>5 seconds, the ssm code 
 * 
 * @param intIfNum : Interface of incoming SSM packet
 * @param ssm_code : SSM code of the SSM packet
 * 
 * @return L7_RC_t : L7_SUCCESS - SSM code successfully updated 
 *                   L7_FAILURE - SSM code was not updated 
 */

L7_RC_t ssmTimersUpdate(void)
{
  L7_uint16 slot, intf;
#if (!PTIN_BOARD_IS_STANDALONE)
  L7_uint32 intIfNum, ptin_port;
  L7_uint32 linkState;
#endif

  osapiSemaTake(ssmTimersSyncSema, L7_WAIT_FOREVER);

  /* Run all slots and timers */
  for (slot=0; slot<SSM_N_SLOTS; slot++)
  {
    for (intf=0; intf<SSM_N_INTFS_IN_USE; intf++)
    {
      /* Increment timer */
      if (ssm_timer[slot][intf]<=6)
      {
        ssm_timer[slot][intf]++;
      }

      /* Update link status (only for SLOT systems... standalone are dealt at ptin_control.c) */
    #if (!PTIN_BOARD_IS_STANDALONE)
      if (ptin_intf_slotPort2port(slot+1, intf, &ptin_port) == L7_SUCCESS &&
          ptin_intf_port2intIfNum(ptin_port, &intIfNum) == L7_SUCCESS &&
          nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS)
      {
        SHMEM(slot,intf).link  = (linkState==L7_UP);

        /* Update activity status (only for SF local ports -> slot 0) */
        if ((PTIN_SYSTEM_ETH_PORTS_MASK >> ptin_port) & 1)
        {
          SHMEM(slot,intf).link |=
               (((ptin_control_port_activity[ptin_port] & PTIN_PORTACTIVITY_MASK_RX_ACTIVITY) == PTIN_PORTACTIVITY_MASK_RX_ACTIVITY) << 1) |
               (((ptin_control_port_activity[ptin_port] & PTIN_PORTACTIVITY_MASK_TX_ACTIVITY) == PTIN_PORTACTIVITY_MASK_TX_ACTIVITY) << 2);
        }
      }
      else
      {
        SHMEM(slot,intf).link = L7_FALSE;
      }
    #endif

      /* If the 5 seconds mark was reached, write 0x0f for the shared memory */
      if (ssm_timer[slot][intf]>5)
      {
        SHMEM(slot,intf).ssm_rx = 0x0f;

        if (ssm_debug_enable)
        {
          if (ssm_timer[slot][intf]==6)
            LOG_TRACE(LOG_CTX_PTIN_SSM,"slot %u / intf %u timed out... ssm code is 0xf",slot,intf);
        }
      }
    }
  }

  osapiSemaGive(ssmTimersSyncSema);

  return L7_SUCCESS;
}

/**
 * Update SSM code for a specific interface. 
 * This routine is called when an SSM packet is received. 
 * The timer of the provided interface is resetted. 
 * 
 * @param intIfNum : Interface of incoming SSM packet
 * @param ssm_code : SSM code of the SSM packet
 * 
 * @return L7_RC_t : L7_SUCCESS - SSM code successfully updated 
 *                   L7_FAILURE - SSM code was not updated 
 */
L7_RC_t ssmCodeUpdate(L7_uint32 intIfNum, L7_uint16 ssm_code)
{
  L7_uint16 slot, intf;
  L7_RC_t   rc;

  /* Convert port to slot/intf */
  rc = ptin_intf_intIfNum2SlotPort(intIfNum, &slot, &intf, L7_NULLPTR);

  if (rc != L7_SUCCESS)
  {
    //if (ssm_debug_enable)
    //  LOG_ERR(LOG_CTX_PTIN_SSM,"Cannot convert intIfNum %u to slot/intf",intIfNum);
    return L7_FAILURE;
  }

  /* Adjust slot value */
  if (slot>0)  slot--;

  osapiSemaTake(ssmTimersSyncSema, L7_WAIT_FOREVER);
  /* Reset timer */
  ssm_timer[slot][intf] = 0;
  osapiSemaGive(ssmTimersSyncSema);

  /* Update ssm code (only for LSbits) */
  SHMEM(slot,intf).ssm_rx = ssm_code & 0x000f;

  if (ssm_debug_enable)
    LOG_TRACE(LOG_CTX_PTIN_SSM,"SSM code (0x%02x) updated for slot=%u, intf=%u (intIfNum=%u)",ssm_code,slot,intf,intIfNum);

  return L7_SUCCESS;
}


void ssm_debug_dump(void)
{
  L7_uint slot, intf;

  if (pfw_shm==L7_NULLPTR)
  {
    printf("Shared memory not defined!\r\n");
    return;
  }

  printf("Dumping SSM shared memory:\r\n");
  for (slot=0; slot<SSM_N_SLOTS; slot++)
  {
    printf("slot=%02u: { ",slot+1);
    for (intf=0; intf<SSM_N_INTFS_IN_USE; intf++)
    {
      printf("rx=0x%05x/tx=0x%05x/lnk=%u ",
             SHMEM(slot,intf).ssm_rx & 0xfffff,
             SHMEM(slot,intf).ssm_tx & 0xfffff,
             SHMEM(slot,intf).link);
    }
    printf("}\r\n");
  }

  fflush(stdout);
}

void ssm_debug_write(L7_uint16 slot, L7_uint16 intf, L7_uint32 ssm_rx, L7_uint32 ssm_tx, L7_uint32 link)
{
  if (pfw_shm==L7_NULLPTR)
  {
    printf("Shared memory not defined!\r\n");
    return;
  }

  /* Validate args */
  if (slot>=SSM_N_SLOTS || intf>=SSM_N_INTFS_IN_USE)
  {
    printf("Slot (%u) or intf (%u) is out of range.\r\n",slot, intf);
    return;
  }

  SHMEM(slot,intf).ssm_tx = ssm_tx;
  SHMEM(slot,intf).ssm_rx = ssm_rx;
  SHMEM(slot,intf).link   = link;

  printf("Done!\r\n");
}

#endif

