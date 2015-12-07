/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  broad_hpc_rpc.c
*
* @purpose   Technology specific functions for the RPC mechanism.
*
* @component
*
* @create    03/14/2005
*
* @author    Andrey Tsigler
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "buff_api.h"
#include "log.h"
#include "unitmgr_api.h"
#include "broad_common.h"
#include "appl/cputrans/cputrans.h"
#include "appl/cputrans/atp.h"
#include "osapi_support.h"
#include "hpc_hw_api.h"
#include "l7_usl_sm.h"
#include "logger.h"   /* PTin added: debug */

L7_uchar8 *l7RpcReqMsgBuf = L7_NULLPTR;
L7_uint32  l7RpcReqMsgBufSize = 0;
void      *l7RpcReqMsgBufSema = L7_NULLPTR;

hpcHwRpcData_t l7RpcRespMsgBuf[L7_MAX_UNITS_PER_STACK + 1];
L7_uint32      l7RpcRespMsgBufSize = 0;
void          *l7RpcRespMsgBufSema = L7_NULLPTR;

/* Storage for transport message buffer pool identifier 
** Defined in broad_hpc_stacking.c
*/
extern L7_uint32 hpc_transport_buffer_pool_id;

/* RPC Critical section protection.
*/
static void * hpc_rpc_sem;

/* Semaphore used by the RPC component to wait for answers from remote units.
*/
static void * hpc_rpc_collect_sem;

/* RPC Transaction semaphore.
*/
static void * hpc_rpc_trans_sem;

#ifdef L7_STACKING_PACKAGE

/* RPC Server queue.
*/
static void * hpc_rpc_queue;

#define HPC_RPC_QUEUE_MSG_SIZE ((hpcHardwareRpcMaxMessageLengthGet ()) + sizeof (rpc_header_t))

#endif

/* RPC transaction number.
*/
static L7_uint32 rpc_transaction_number = 0;

/* Per-unit status of the RPC transaction.
*/
typedef struct 
{
  L7_BOOL rpc_active; /* RPC transaction is in progress for this unit */

  L7_BOOL tx_pending;  /* Waiting for the TX-Complete ACK             */
  int     tx_status;   /* Return code from the TX operation.          */

  L7_BOOL            rpc_pending; /* Waiting for reply from remote unit          */ 
  hpcHwRpcStatus_t   rpc_status;  /* RPC Server status on the remote unit        */

  int     app_status;  /* Return code from the remote unit.           */

  hpcHwRpcData_t *rpc_resp; /* Return data from remote application */
} rpc_client_t;

static rpc_client_t rpc_client_info [L7_MAX_UNITS_PER_STACK + 1];

/* RPC Message header.
*/
typedef struct 
{
  L7_BOOL   rpc_request; /* L7_TRUE - Request. L7_FALSE - Reply. */

  L7_BOOL   rpc_reply_needed; /* L7_TRUE - Send RPC reply. L7_FALSE - Don't send RPC reply. */

  L7_uint32 client_unit;  /* Unit number of the sender */
  L7_uint32 server_unit;  /* Receiver unit number */
  L7_uint32 transaction_id; /* Unique identifier for the transaction */
  L7_uint32 transaction_number; /* Transaction udentifier */
  L7_uint32 buffer_size;  /* Size of the RPC data excluding the header. */
  hpcHwRpcStatus_t server_status; /* RPC Server Status */

  L7_uint32 app_status; /* Return code from remote application */
  
} rpc_header_t;


/* Maximum number of supported RPC registered functions.
*/
#define HPC_HW_RPC_MAX_CALLBACKS  64

typedef struct
{
  hpc_rpc_callback_t   callback;
  L7_uint32            transaction_id;
} hpc_rpc_callback_list_t;

static hpc_rpc_callback_list_t hpc_rpc_callback[HPC_HW_RPC_MAX_CALLBACKS]; 

/* RPC statistics.
*/
typedef struct 
{
  L7_uint32 rpc_num_functions; /* Handlers registered with RPC */

  L7_uint32 rpc_client_count;  /* Total number of RPC requests */
  L7_uint32 rpc_server_rx_count;  /* Total number of RPC requests received by the server */
  L7_uint32 rpc_server_tx_count;  /* Total number of RPC replies sent by the server */
  L7_uint32 remote_rpc_requests;
  L7_uint32 tx_no_buffer;
  L7_uint32 tx_bad;
  L7_uint32 tx_completes_good;
  L7_uint32 tx_completes_bad;
  L7_uint32 rpc_timeouts;

  L7_uint32 rpc_out_of_sync;  /* RPC mesasge received for an invalid transaction */

  /* Number of times we waited and dispatched the hpc_rpc_collect_sem.
  */
  L7_uint32 collect_sem_wait;
  L7_uint32 collect_sem_dispatch;

} hpc_rpc_stats_t;

static hpc_rpc_stats_t hpc_rpc_stats;

/*********************************************************************
* @purpose  Dispatch RPC request.
*
* @param    rpc_header - RPC header (Input)
* @param    rpc_data   - Application data in the RPC request (Input)
* @param    rpc_resp   - Application response for the RPC request (Output)
*
* @comments If RPC handler has data to be sent back as RPC response, the 
*           data is stored in rpc_resp and the length is stored in 
*           rpc_resp.data_len. The max size of rpc_resp buffer is indicated by
*           rpc_resp.buf_size.
*
* @end
*********************************************************************/
static void hpcHardwareRpcDispatch (rpc_header_t * rpc_header,
                                    hpcHwRpcData_t  * rpc_data,
                                    hpcHwRpcData_t  * rpc_resp)
{
  L7_uint32 i;
  L7_RC_t   rc = L7_SUCCESS;

  for (i = 0; i < HPC_HW_RPC_MAX_CALLBACKS; i++)
  {
    if (hpc_rpc_callback[i].transaction_id == rpc_header->transaction_id)
    {
      //LOG_DEBUG(LOG_CTX_MISC, "hpc_rpc_callback[i].callback = 0x%08X", (unsigned int)hpc_rpc_callback[i].callback); /* PTin added: debug */
      rc = (hpc_rpc_callback[i].callback) (rpc_header->transaction_id,
                                           rpc_data, 
                                           &rpc_header->app_status,
                                           rpc_resp);
      break;
    }
  }

  if (i == HPC_HW_RPC_MAX_CALLBACKS)
  {
    rpc_header->server_status = HPC_HW_RPC_NO_SERVER;
  } else 
  {
    if (rc != L7_SUCCESS)
    {
      rpc_header->server_status = HPC_HW_RPC_RX_ERROR;
    } else
    {
      rpc_header->server_status = HPC_HW_RPC_OK;
    }
  }

}

/*********************************************************************
* @purpose  Enter RPC Critical section.
* 
* @comments
*
* @end
*********************************************************************/
static void hpcBroadRpcCritEnter (void)
{
  L7_RC_t rc;

  rc = osapiSemaTake(hpc_rpc_sem, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }
}

/*********************************************************************
* @purpose  Exit RPC Critical section.
* 
* @comments
*
* @end
*********************************************************************/
static void hpcBroadRpcCritExit (void)
{
  osapiSemaGive (hpc_rpc_sem);
}

#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Callback for receiving asynchronous TX-Complete
* 	    notifications from ATP.
* 
* @comments
*
* @end
*********************************************************************/
void hpcBroadRpcMsgSentCb(uint8 *pkt_buf, void *cookie, int rv)
{
  L7_uint32 unit;

  unit = (L7_uint32) cookie;
  if (unit > L7_MAX_UNITS_PER_STACK)
  {
    L7_LOG_ERROR(unit);  /* Something gone wrong... */
  }

  if (rv != BCM_E_NONE)
  {
    hpc_rpc_stats.tx_completes_bad++;
  } else
  {
    hpc_rpc_stats.tx_completes_good++;
  }

  rpc_client_info[unit].tx_pending = L7_FALSE;
  rpc_client_info[unit].tx_status = rv;

  /* return packet buffer to free pool */
  bufferPoolFree(hpc_transport_buffer_pool_id, pkt_buf);
}

/*********************************************************************
* @purpose  Send a message to a specific unit in the stack. 
*
* @param    unit        Unit to which this message should be sent.
* @param    msg_length  number of bytes in payload buffer
* @param    buffer      pointer to the payload to be sent
*
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport, message not acknowledged
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t hpcHardwareAsyncMessageSend(L7_uint32 unit, 
                                    rpc_header_t  *rpc_header,
                                    L7_uint32 msg_length, 
                                    L7_uchar8* buffer)
{
  int rv = BCM_E_NONE;
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 *hpc_buffer;
  L7_uchar8 *buff_ptr;
  cpudb_key_t dest_key;
  L7_uint32   atp_len;

  /* retrieve the cpu key from cpudb 
  */
  if (unitMgrUnitIdKeyGet(unit, (L7_enetMacAddr_t *)&(dest_key.key)) != L7_SUCCESS)
  {
    hpc_rpc_stats.tx_bad++;
    return L7_FAILURE;
  }

  atp_len = msg_length + CPUTRANS_HEADER_BYTES + sizeof (rpc_header_t);

  do
  {
    rc = bufferPoolAllocate(hpc_transport_buffer_pool_id, &hpc_buffer);
    if (rc != L7_SUCCESS)
    {
      hpc_rpc_stats.tx_no_buffer++;
      osapiSleepMSec (10);
    }
  } while (rc != L7_SUCCESS);

  /* adjust pointer to start of data.
  */
  buff_ptr = hpc_buffer + CPUTRANS_DATA_OFS;

  /* Copy message header into the buffer.
  */
  memcpy (buff_ptr, rpc_header, sizeof (rpc_header_t));

  buff_ptr += sizeof (rpc_header_t);

  /* copy the callers data into the payload 
  */
  if (msg_length > 0)
  {
    memcpy(buff_ptr, buffer, msg_length);
  }

  rv = atp_tx(dest_key, 
                LVL7_RPC_TRANSPORT_PORT_NUMBER, 
                hpc_buffer, 
                atp_len,
                CPUTRANS_NO_HEADER_ALLOC, 
                hpcBroadRpcMsgSentCb, 
                (void *) unit);

  if (rv != BCM_E_NONE)
  {
      hpc_rpc_stats.tx_bad++;
      rc = L7_FAILURE;
      bufferPoolFree(hpc_transport_buffer_pool_id, hpc_buffer);
  }

  return(rc);
}


/*********************************************************************
* @purpose  Callback for receiving RPC messages.
* 
* @comments
*
* @end
*********************************************************************/
bcm_rx_t hpcBroadRpcMsgHandler(cpudb_key_t src_key, 
                               int         client_id,
                               bcm_pkt_t   *pkt,
                               uint8       *pkt_buf, 
                               int         len, 
                               void        *cookie)
{
  rpc_header_t *rpc_header;
  L7_RC_t      rc;
  L7_uint32    unit;
  L7_uint32    i;
  L7_BOOL      rpc_pending;

  rpc_header = (rpc_header_t *) pkt_buf;

#if 0
    printf("Handler - Request=%d, Transaction=%d\n", 
           rpc_header->rpc_request,
           rpc_header->transaction_number);
#endif

  if (rpc_header->rpc_request == L7_TRUE)
  {
    rc = osapiMessageSend (hpc_rpc_queue, 
                           pkt_buf, 
                           len, 
                           L7_NO_WAIT, 
                           L7_MSG_PRIORITY_NORM);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR(rc);
    }
  } else
  {
    if (rpc_transaction_number != rpc_header->transaction_number)
    {
      hpc_rpc_stats.rpc_out_of_sync++;
    } else
    {
      rc = osapiSemaTake (hpc_rpc_trans_sem, L7_WAIT_FOREVER);
      if (rc != L7_SUCCESS)
      {
        L7_LOG_ERROR(rc);
      }

      unit = rpc_header->server_unit;
      
      rpc_client_info[unit].rpc_pending = L7_FALSE;
      rpc_client_info[unit].rpc_status = rpc_header->server_status;
      rpc_client_info[unit].app_status = rpc_header->app_status;
 
      /* If application expects an RPC response, check if the RPC reply has 
       * any data to be sent back to application
       */

      if (rpc_client_info[unit].rpc_resp != (hpcHwRpcData_t *)L7_NULL)
      {
        if (rpc_header->buffer_size > 0)
        {
          hpcHwRpcData_t *resp  = rpc_client_info[unit].rpc_resp;
          L7_uchar8  *buf = (L7_uchar8 *)(pkt_buf + sizeof(rpc_header_t));
 
          /* Copy the RPC response and update the response length. */
          if (rpc_header->buffer_size <= resp->buf_size) 
          {
            resp->data_len = rpc_header->buffer_size;
            memcpy (resp->buf, buf, resp->data_len);
          }
          else
          {
            resp->data_len = 0; /* We received more data than expected. Can't copy */
          }
        }
      }

      /* If all units completed the RPC transaction
      ** then signal the RPC client.
      */
      rpc_pending = L7_FALSE;
      for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
      {
        if (rpc_client_info[i].rpc_pending)
        {
          rpc_pending = L7_TRUE;
          break;
        }
      }

      if (rpc_pending == L7_FALSE)
      {
        hpc_rpc_stats.collect_sem_dispatch++;
        osapiSemaGive (hpc_rpc_collect_sem);
      }

      rc = osapiSemaGive (hpc_rpc_trans_sem);
      if (rc != L7_SUCCESS)
      {
        L7_LOG_ERROR(rc);
      }
    }
  }


  return(BCM_RX_HANDLED);

}

/*********************************************************************
* @purpose  RPC server task.
* 
* @comments
*
* @end
*********************************************************************/
void hpcBroadRpcServer(void)
{
  L7_uchar8 *msg_buf;
  hpcHwRpcData_t app_data;
  rpc_header_t *rpc_header;
  hpcHwRpcData_t app_resp;
  L7_RC_t   rc;

  /* Buffer to hold the incoming RPC request */
  msg_buf = osapiMalloc (L7_DRIVER_COMPONENT_ID, HPC_RPC_QUEUE_MSG_SIZE);
  if (msg_buf == 0)
  {
    L7_LOG_ERROR(0);
  }

  /* Buffer to hold the RPC response for the incoming RPC request */
  app_resp.buf_size = hpcHardwareRpcMaxMessageLengthGet();
  app_resp.data_len = 0;
  app_resp.buf = osapiMalloc (L7_DRIVER_COMPONENT_ID, app_resp.buf_size);
  if (msg_buf == 0)
  {
    L7_LOG_ERROR(0);
  }

  while (1)
  {
    if (osapiMessageReceive(hpc_rpc_queue, (void *)msg_buf,
                          HPC_RPC_QUEUE_MSG_SIZE,
                          L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOG_ERROR(0);
    }

    hpc_rpc_stats.rpc_server_rx_count++;

    rpc_header = (rpc_header_t *) msg_buf;
    app_data.buf = msg_buf + (sizeof (rpc_header_t));
    app_data.data_len = app_data.buf_size = rpc_header->buffer_size;

    /* If there is application data to be sent back as RPC response, then the
     * RPC handler must set the rpc_resp.data_len to non-zero value. The max response
     * that can be sent is indicated by the size of RPC response buffer.
     */
    app_resp.data_len = 0;
    hpcHardwareRpcDispatch (rpc_header, &app_data, &app_resp);  

    /* Sanity check the response length */
    if (app_resp.data_len > hpcHardwareRpcMaxMessageLengthGet())
    {
      app_resp.data_len = 0; /* Can't send the response */
    }

    if (rpc_header->rpc_reply_needed == L7_TRUE)
    {
      hpc_rpc_stats.rpc_server_tx_count++;

      rpc_header->rpc_request = L7_FALSE; /* This message is a reply */
      rpc_header->buffer_size = app_resp.data_len;

  #if 0
      printf("Server - Sending Transaction %d\n", rpc_header->transaction_number);
  #endif
  
      rc = hpcHardwareAsyncMessageSend(rpc_header->client_unit, 
                                       rpc_header, 
                                       app_resp.data_len, 
                                       (L7_uchar8 *) app_resp.buf);
      if (rc != L7_SUCCESS)
      {
        hpc_rpc_stats.tx_bad++;
      } 
    }


  }
}

#endif

/*********************************************************************
* @purpose  Initialize RPC buffers
*
* @params   none
*
* @returns  none
*
* @returns  
*       
* @end
*********************************************************************/
L7_RC_t l7_rpc_buffer_init(void)
{
  L7_uint32 i;

  l7RpcReqMsgBufSize = hpcHardwareRpcMaxMessageLengthGet();

  /* Allocate RPC request message */
  l7RpcReqMsgBuf = osapiMalloc (L7_DRIVER_COMPONENT_ID, (l7RpcReqMsgBufSize));
  if (l7RpcReqMsgBuf == L7_NULLPTR)
    L7_LOG_ERROR("USL: unable to allocate l7RpcReqMsgBuf\n");

  l7RpcReqMsgBufSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (l7RpcReqMsgBufSema == L7_NULLPTR)
    L7_LOG_ERROR("USL: unable to create the l7RpcReqMsgBufSema\n");


  /* Allocate RPC response message */
  l7RpcRespMsgBufSize = hpcHardwareRpcMaxMessageLengthGet();

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    l7RpcRespMsgBuf[i].buf = osapiMalloc (L7_DRIVER_COMPONENT_ID, (l7RpcRespMsgBufSize));
    if (l7RpcRespMsgBuf[i].buf == L7_NULLPTR)
      L7_LOG_ERROR("USL: unable to allocate l7RpcReqMsgBuf\n");
    l7RpcRespMsgBuf[i].buf_size = l7RpcRespMsgBufSize;
  }

  l7RpcRespMsgBufSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (l7RpcRespMsgBufSema == L7_NULLPTR)
    L7_LOG_ERROR("USL: unable to create the l7RpcRespMsgBufSema\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get a buffer to send RPC request message
*
* @params   none
*
* @returns  none
*
* @returns  L7_NULLPTR if the buffer isn't allocated, otherwise a pointer to buffer
*       
* @end
*********************************************************************/
L7_uchar8* l7_rpc_req_buffer_get(void)
{
  if ((l7RpcReqMsgBuf == L7_NULLPTR) || (l7RpcReqMsgBufSema == L7_NULLPTR))
    L7_LOG_ERROR("USL: unable to allocate buffer before USL is initted\n");


  /* The corresponding give is when the buffer is freed later on */
  osapiSemaTake(l7RpcReqMsgBufSema,L7_WAIT_FOREVER);

  memset(l7RpcReqMsgBuf, 0, l7RpcReqMsgBufSize);

  return l7RpcReqMsgBuf;
}

/*********************************************************************
* @purpose  Free the RPC request buffer
*
* @param    Pointer to buffer
*
* @returns  L7_SUCCESS or L7_FAILURE
*       
* @end
*********************************************************************/
L7_RC_t l7_rpc_req_buffer_free(L7_uchar8 *buffer)
{
  if ((l7RpcReqMsgBuf == L7_NULLPTR) || (l7RpcReqMsgBufSema == L7_NULLPTR))
    L7_LOG_ERROR("USL: unable to free RPC buffer \n");

  /* The corresponding give is when the buffer is freed later on */
  osapiSemaGive(l7RpcReqMsgBufSema);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the RPC req buffer size
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*       
* @end
*********************************************************************/
L7_RC_t l7_rpc_req_buffer_size_get()
{
  if ((l7RpcReqMsgBuf == L7_NULLPTR) || (l7RpcReqMsgBufSema == L7_NULLPTR))
    L7_LOG_ERROR("USL: RPC buffer not allocated\n");

  return l7RpcReqMsgBufSize;
}

/*********************************************************************
* @purpose  Get a buffer to get RPC response message
*
* @params   none
*
* @returns  none
*
* @returns  L7_NULLPTR if the buffer isn't allocated, otherwise a pointer to buffer
*       
* @end
*********************************************************************/
hpcHwRpcData_t* l7_rpc_resp_buffer_get(void)
{
  L7_uint32 i;

  /* The corresponding give is when the buffer is freed later on */
  osapiSemaTake(l7RpcRespMsgBufSema,L7_WAIT_FOREVER);

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(l7RpcRespMsgBuf[i].buf, 0, l7RpcRespMsgBufSize);
    l7RpcRespMsgBuf[i].buf_size = l7RpcRespMsgBufSize;
  }

  return (&l7RpcRespMsgBuf[0]);
}

/*********************************************************************
* @purpose  Free the RPC response buffer
*
* @param    Pointer to buffer
*
* @returns  L7_SUCCESS or L7_FAILURE
*       
* @end
*********************************************************************/
L7_RC_t l7_rpc_resp_buffer_free(hpcHwRpcData_t *buffer)
{
  /* The corresponding give is when the buffer is freed later on */
  osapiSemaGive(l7RpcRespMsgBufSema);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize RPC transport.
* 
* @comments
*
* @end
*********************************************************************/
void hpcBroadRpcInit (void)
{
  hpc_rpc_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_FULL);

  hpc_rpc_trans_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_FULL);

  hpc_rpc_collect_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_EMPTY);

  memset (&hpc_rpc_callback, 0, sizeof (hpc_rpc_callback));

  l7_rpc_buffer_init();

  /* RPC server is included only on Stackable platforms */
#ifdef L7_STACKING_PACKAGE
  atp_register(LVL7_RPC_TRANSPORT_PORT_NUMBER, ATP_F_REASSEM_BUF, hpcBroadRpcMsgHandler,
                 NULL, -1, -1);

  hpc_rpc_queue = (void *)osapiMsgQueueCreate("hpc_rpc_queue",
                                               L7_MAX_UNITS_PER_STACK * 2,
                                                   HPC_RPC_QUEUE_MSG_SIZE);
  /* Create HPC RPC task.
  */
  if (osapiTaskCreate("hpcBroadRpcServer", hpcBroadRpcServer,
                      0, 0, L7_DEFAULT_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    L7_LOG_ERROR(0);
  }
#endif
  
}

/*********************************************************************
* @purpose    Maximum size of the data portion of the RPC message.
*           
* @comments
*
* @end
*********************************************************************/
L7_uint32 hpcHardwareRpcMaxMessageLengthGet (void)
{
  L7_uint32 hpc_len;

   hpc_len =  hpcHardwareTransportMaxMessageLengthGet ();
   hpc_len -= sizeof (rpc_header_t);

   return hpc_len;
}

/******************************************************************************
* @purpose    Execute the RPC transaction on a given unit or on all 
*             eligible units. Eligible units are units that are found 
*             in the stack and that don't have code or configuration 
*             mismatch.
*
*  target_unit_number - Indicates the unit number on which the RPC transaction
*                       is to be executed. If the target_unit_number is 
*                       L7_ALL_UNITS then the RPC transaction is executed 
*                       on all eligible units.
*
*  rpc_transaction_id - Unique identifier for the RPC function.
*                       On the server side some code must register
*                       a handler for this transaction_id.
*
*  rpc_data           - This data is passed from the client to the server.
*
*  rpc_status         - Pointer to an array where RPC puts its status. 
*                       If the target_unit_number is L7_ALL_UNITS, then the array 
*                       is indexed by unit number and the caller must 
*                       allocate at least L7_MAX_UNITS_PER_STACK + 1) element 
*                       in the array.
*
*  app_status        -  A 4-byte status returned by the remote application. 
*                       If the target_unit_number is L7_ALL_UNITS, then the caller 
*                       must allocate (L7_MAX_UNITS_PER_STACK + 1) elements 
*                       in the array.
*                       The caller may set this parameter to NULL. In that case
*                       no information is returned from the remote server, which
*                       improves RPC performce 2x.
*
*  app_resp         -   If the RPC transaction is expected to receive data from
*                       from server, this points to a response buffer to store
*                       received data.If the target_unit_number is L7_ALL_UNITS, then
*                       the caller must allocate (L7_MAX_UNITS_PER_STACK + 1)
*                       elements to receive data. If no data is expected in response
*                       then this pointer must be set to L7_NULL.
*
* @comments
*
* @end
***************************************************************************************/
L7_RC_t hpcHardwareRpc  (L7_uint32          target_unit_number,
                         L7_uint32          rpc_transaction_id,
                         hpcHwRpcData_t    *rpc_data,
                         hpcHwRpcStatus_t  *rpc_status,
                         L7_int32          *app_status,
                         hpcHwRpcData_t    *app_resp)
{
  L7_uint32         i;
  L7_RC_t           rc;
  unitMgrStatus_t   unit_status;
  L7_uint32         local_unit;
  L7_uint32         mgmt_unit;
  L7_BOOL           localUnitIsMgr;
  rpc_header_t      rpc_header;
  L7_BOOL           dispatched_remote_rpc;
  hpcHwRpcData_t   *rpc_resp;
  hpcHwRpcStatus_t *unit_rpc_status[L7_MAX_UNITS_PER_STACK + 1];
  L7_int32         *unit_app_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t   *unit_app_resp[L7_MAX_UNITS_PER_STACK + 1];

  rc = unitMgrNumberGet (&local_unit);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  localUnitIsMgr = L7_FALSE;
  if (unitMgrMgrNumberGet(&mgmt_unit) == L7_SUCCESS)
  {
    if (mgmt_unit == local_unit)
    {
      localUnitIsMgr = L7_TRUE;
    }
  }

  if (target_unit_number > L7_ALL_UNITS)
  {
    return L7_FAILURE;    
  }

  hpcBroadRpcCritEnter ();

  hpc_rpc_stats.rpc_client_count++;

  memset (&rpc_client_info, 0, sizeof (rpc_client_info));
  memset (unit_rpc_status, 0, sizeof(unit_rpc_status));
  memset (unit_app_status, 0, sizeof(unit_app_status));
  memset (unit_app_resp, 0, sizeof(unit_app_resp));

  /* Populate the local arrays with caller's status pointers */
  if (target_unit_number == L7_ALL_UNITS)
  {
    for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
    {
      unit_rpc_status[i] = &rpc_status[i];
      if (app_status != L7_NULLPTR)
      {
        unit_app_status[i] = &app_status[i];
      }
      if (app_resp != L7_NULLPTR)
      {
        unit_app_resp[i]   = &app_resp[i];
      }
    }
  }
  else
  {
    unit_rpc_status[target_unit_number] = rpc_status;
    if (app_status != L7_NULLPTR)
    {
      unit_app_status[target_unit_number] = app_status;
    }
    if (app_resp != L7_NULLPTR)
    {
      unit_app_resp[target_unit_number]   = app_resp;
    }
  }

  /* Set up the transaction table. Mark all units that will
  ** be waiting for an RPC call.
  */
  rc = osapiSemaTake (hpc_rpc_trans_sem, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    if ((target_unit_number != L7_ALL_UNITS) && (target_unit_number != i))
    {
      continue;    
    }

    rc = unitMgrUnitStatusGet (i, &unit_status);
    if ((rc != L7_SUCCESS) || (unit_status != L7_UNITMGR_UNIT_OK) ||
        ((localUnitIsMgr == L7_TRUE) && (usl_unit_sync_check(i) == L7_FALSE)))
    {
      *(unit_rpc_status[i]) = HPC_HW_RPC_NO_UNIT;
      continue; /* Ignore units that aren't there. */
    }

    *(unit_rpc_status[i]) = HPC_HW_RPC_OK;  /* Assume that RPC is OK until proven otherwise. */
    rpc_client_info[i].rpc_pending = L7_TRUE;
  }

  rc = osapiSemaGive (hpc_rpc_trans_sem);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  /* Dispatch calls to remote units only on Stackables */
#ifdef L7_STACKING_PACKAGE
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    if (i == local_unit)
    {
      continue;  /* Skip local units. */
    }

    if (rpc_client_info[i].rpc_pending != L7_TRUE)
    {
      continue;
    }

    hpc_rpc_stats.remote_rpc_requests++;

    rpc_client_info[i].tx_pending = L7_TRUE;
    rpc_client_info[i].rpc_active = L7_TRUE;

    rpc_header.buffer_size = rpc_data->data_len;
    rpc_header.client_unit = local_unit;
    rpc_header.server_unit = i;
    rpc_header.server_status = 0;
    rpc_header.app_status = 0;
    rpc_header.transaction_id = rpc_transaction_id;
    rpc_header.transaction_number = rpc_transaction_number;

    rpc_header.rpc_request = L7_TRUE; /* This message is a request */

    if (unit_app_status[i] == L7_NULLPTR)
    {
      rpc_header.rpc_reply_needed = L7_FALSE;
    } else
    {
      rpc_header.rpc_reply_needed = L7_TRUE;
    }

    if (unit_app_resp[i] != L7_NULLPTR)
    {
      rpc_resp = unit_app_resp[i];
      rpc_resp->data_len = 0;
      rpc_client_info[i].rpc_resp = rpc_resp;
    } else
    {
      rpc_client_info[i].rpc_resp = L7_NULL;
    }

    rc = hpcHardwareAsyncMessageSend(i, &rpc_header, 
                                     rpc_data->data_len, rpc_data->buf);
    if (rc != L7_SUCCESS)
    {
      *(unit_rpc_status[i]) = HPC_HW_RPC_TX_ERROR;
      rpc_client_info[i].tx_pending = L7_FALSE;
      rpc_client_info[i].rpc_pending = L7_FALSE;
      rpc_client_info[i].rpc_active = L7_FALSE;
    } 
  }
#endif

  if ((target_unit_number == L7_ALL_UNITS) ||
      (target_unit_number == local_unit))
  {
    /* While waiting for remote units to do the work, 
    ** dispatch the callback for the local unit.
    */
    rpc_header.buffer_size = rpc_data->data_len;
    rpc_header.client_unit = local_unit;
    rpc_header.server_unit = local_unit;
    rpc_header.server_status = 0;
    rpc_header.app_status = 0;
    rpc_header.transaction_id = rpc_transaction_id;
    rpc_header.transaction_number = rpc_transaction_number;

    if (unit_app_resp[local_unit] != L7_NULLPTR)
    {
      rpc_resp = unit_app_resp[local_unit];
      rpc_resp->data_len = 0;
    }
    else
    {
      rpc_resp = L7_NULL;
    }

    hpcHardwareRpcDispatch (&rpc_header, rpc_data, rpc_resp);

    rpc_client_info[local_unit].tx_pending = L7_FALSE;
    rpc_client_info[local_unit].rpc_active = L7_TRUE;
    rpc_client_info[local_unit].rpc_status = rpc_header.server_status;
    rpc_client_info[local_unit].app_status = rpc_header.app_status;
  }
  
  dispatched_remote_rpc = L7_FALSE;

  /* If caller doesn't want any reply from the remote server then 
  ** we are done. Otherwise wait for responses and collect 
  ** status.
  */
  if (app_status != 0)
  {
    /* It is possible that all remote clients already responded 
    ** to the RPC call. If that is the case then we shouldn't wait
    ** on the "collect" semaphore.
    */
    rc = osapiSemaTake (hpc_rpc_trans_sem, L7_WAIT_FOREVER);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR(rc);
    }

    rpc_client_info[local_unit].rpc_pending = L7_FALSE;

    for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
    {
      if (rpc_client_info[i].rpc_pending == L7_TRUE)
      {
        dispatched_remote_rpc = L7_TRUE;
        break;
      }
    }

    rc = osapiSemaGive (hpc_rpc_trans_sem);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR(rc);
    }

  }

#ifdef L7_STACKING_PACKAGE 
  /* If we dispatched any remote RPC calls then we need to wait
  ** until everything completes.
  */
  if (dispatched_remote_rpc == L7_TRUE)
  {
    int rpc_timeout;
    int atp_timeout;
    int atp_retry_count;

    atp_timeout_get (&atp_timeout, &atp_retry_count);
    rpc_timeout = 2 * ((atp_timeout / 1000) * atp_retry_count);


    hpc_rpc_stats.collect_sem_wait++;
    rc = osapiSemaTake(hpc_rpc_collect_sem, rpc_timeout);

    /* If we timed out then cancel the RPC transaction and free
    ** the collection semaphore.
    */
    if (rc != L7_SUCCESS)
    {
      hpc_rpc_stats.rpc_timeouts++;
      rpc_transaction_number++;
      osapiSemaGive (hpc_rpc_collect_sem);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "hpcHardwareRpc: RPC Timeout for transaction %d\n",
              rpc_transaction_id);                   
    }

  }
#endif

  if (app_status != 0)
  {
    /* Gather RPC results.
    */
    for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
    {
      if (rpc_client_info[i].rpc_active == L7_FALSE)
      {
        continue;
      }

      if (rpc_client_info[i].rpc_pending == L7_TRUE)
      {
        *(unit_rpc_status[i]) = HPC_HW_RPC_TIMEOUT;
      } else
      {
        *(unit_rpc_status[i]) = rpc_client_info[i].rpc_status;
        *(unit_app_status[i]) = rpc_client_info[i].app_status;
      }
    }
  }


  rpc_transaction_number++;
  hpcBroadRpcCritExit ();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Register an RPC service function.
* 
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareRpcRegister (L7_uint32 rpc_transaction_id,
                                hpc_rpc_callback_t  rpc_callback)
{
  L7_uint32 i;

  for (i = 0; i < HPC_HW_RPC_MAX_CALLBACKS; i++)
  {
    /* Duplicate registrations aren't allowed.
    */
    if (hpc_rpc_callback[i].transaction_id == rpc_transaction_id)
    {
      L7_LOG_ERROR(rpc_transaction_id);
    }

    if (hpc_rpc_callback[i].callback == 0)
    {
      hpc_rpc_callback[i].callback = rpc_callback;
      hpc_rpc_callback[i].transaction_id = rpc_transaction_id;
      break;
    }
  }

  if (i == HPC_HW_RPC_MAX_CALLBACKS)
  {
    L7_LOG_ERROR(rpc_transaction_id);
  }

  hpc_rpc_stats.rpc_num_functions++;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print RPC statistics.
* 
* @comments
*
* @end
*********************************************************************/
L7_uint32 hpcRpcStats (void)
{
  printf("RPC Registered Functions: %d\n", hpc_rpc_stats.rpc_num_functions);
  printf("RPC Client Requests: %d\n", hpc_rpc_stats.rpc_client_count);
  printf("RPC Server Requests: %d\n", hpc_rpc_stats.rpc_server_rx_count);
  printf("RPC Server Replies:  %d\n", hpc_rpc_stats.rpc_server_tx_count);
  printf("RPC Client Remote TX: %d\n", hpc_rpc_stats.remote_rpc_requests);
  printf("RPC Good TX Callbacks: %d\n", hpc_rpc_stats.tx_completes_good);
  printf("RPC Bad TX:           %d\n", hpc_rpc_stats.tx_bad);
  printf("RPC Bad TX Callbacks: %d\n", hpc_rpc_stats.tx_completes_bad);
  printf("RPC RX Timeouts:      %d\n", hpc_rpc_stats.rpc_timeouts);
  printf("RPC RX Out of Sync:   %d\n", hpc_rpc_stats.rpc_out_of_sync);
  printf("RPC No Buffer:        %d\n", hpc_rpc_stats.tx_no_buffer);
  printf("RPC Collect Sem Wait: %d\n", hpc_rpc_stats.collect_sem_wait);
  printf("RPC Collect Sem Disp: %d\n", hpc_rpc_stats.collect_sem_dispatch);


  return 0;
}

#ifdef L7_STACKING_PACKAGE

/************************************************************************
*************************************************************************
** Test Functions
*************************************************************************
************************************************************************/
#define TEST1_ID   HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 998)
#define TEST2_ID   HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 997)

/********
** Sample Server Function 1.
********/
L7_RC_t hpc_rpc_server_test_1 (L7_uint32 transaction_id,
                               hpcHwRpcData_t *rpc_data,
                               L7_int32 *status,
                               hpcHwRpcData_t  *rpc_resp)
{
  L7_uint32 app_data;
  static L7_uint32 execute_count = 0;

  printf("hpc_rpc_server_test_1 \n");
  printf("  Transaction  %d\n", transaction_id);

  app_data = *(L7_uint32 *) rpc_data->buf;

  printf("  App Data:    %d\n", app_data);
  printf("  Buf Len:     %d\n", rpc_data->data_len);

  execute_count++;
  *status = execute_count;
  
  if (rpc_resp != L7_NULL)
  {
    *(L7_uint32 *)rpc_resp->buf = 0xfeed0000 + execute_count;
    rpc_resp->data_len = sizeof(L7_uint32);
  }

  return L7_SUCCESS;
}

/********
** Sample Server Function 2.
********/
L7_RC_t hpc_rpc_server_test_2 (L7_uint32 transaction_id,
                               hpcHwRpcData_t *rpc_data,
                               L7_int32 *status,
                               hpcHwRpcData_t  *rpc_resp)
{
  L7_uint32 app_data;
  static L7_uint32 execute_count = 0;

  app_data = *(L7_uint32 *) rpc_data->buf;

  /* Sleep for specified number of milliseconds.
  */
  osapiSleepMSec (app_data);

  execute_count++;
  *status = execute_count;
  return L7_SUCCESS;
}

/********
** Register two server functions.
********/

L7_uint32 hpcDebugRegister (void)
{
  L7_RC_t rc;


    rc = hpcHardwareRpcRegister (TEST1_ID,
                                 hpc_rpc_server_test_1);
    printf("Registered test 1, rc = %d\n", rc);

    rc = hpcHardwareRpcRegister (TEST2_ID,
                                 hpc_rpc_server_test_2);
    printf("Registered test 2, rc = %d\n", rc);



  return 0;
}

/********
** Invoke Test 1
********/
L7_uint32 hpcDebugTest1(L7_uint32 param)
{
  hpcHwRpcData_t    rpc_data;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_RC_t rc;
  L7_uint32 i;

  rpc_data.buf = (L7_uchar8 *)&param;
  rpc_data.buf_size = rpc_data.data_len = sizeof (param);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        TEST1_ID,
                        &rpc_data,
                        rpc_status,
                        app_status,
                        L7_NULL);
  printf("hpcHardwareRpc - rc = %d\n", rc);

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    printf("Unit: %d   rpc_status: %d   app_status: %d\n", i, rpc_status[i], app_status[i]);
  }

  return 0;
}


/********
** Invoke Test 2
********/
L7_uint32 hpcDebugTest2(L7_uint32 num_iter, L7_uint32 sleep_msec)
{
  hpcHwRpcData_t    rpc_data;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32 i;
  L7_uint32 start_time;
  L7_uint32 end_time;
  L7_uint32 transaction_rate;
  L7_uint32 delta_time;

  start_time = osapiTimeMillisecondsGet ();


  printf("Starting test. Iterations: %d, Sleep Time %d ms, Start Time %d\n",
                num_iter,
              sleep_msec,
              start_time);

  rpc_data.buf = (L7_uchar8 *) &sleep_msec;
  rpc_data.buf_size = rpc_data.data_len = sizeof(sleep_msec);

  for (i = 0; i < num_iter; i++)
  {
    (void)hpcHardwareRpc  (L7_ALL_UNITS,
                           TEST2_ID,
                           &rpc_data,
                           rpc_status,
                           app_status,
                           L7_NULL);
  }

  end_time = osapiTimeMillisecondsGet ();

  //delta_time = end_time - start_time;
  delta_time = osapiTimeMillisecondsDiff(end_time,start_time);
  if (delta_time > 10000)
  {
  transaction_rate = num_iter / (delta_time / 1000);
  } else
  {
    transaction_rate = (num_iter * 1000) / delta_time;
  }

  printf("Completed %d iterations in %d milliseconds (%d transactions per second)\n",
         num_iter,
         delta_time,
         transaction_rate);

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    printf("Unit: %d   rpc_status: %d   app_status: %d\n", i, rpc_status[i], app_status[i]);
  }

  return 0;
}

/********
** Invoke Test 2, Without Reply.
********/
L7_uint32 hpcDebugTest2a(L7_uint32 num_iter, L7_uint32 sleep_msec)
{
  hpcHwRpcData_t    rpc_data;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32 i;
  L7_uint32 start_time;
  L7_uint32 end_time;
  L7_uint32 transaction_rate;
  L7_uint32 delta_time;

  start_time = osapiTimeMillisecondsGet ();


  printf("Starting test. Iterations: %d, Sleep Time %d ms, Start Time %d\n",
                num_iter,
              sleep_msec,
              start_time);

  rpc_data.buf = (L7_uchar8 *) &sleep_msec;
  rpc_data.buf_size = rpc_data.data_len = sizeof (sleep_msec);

  for (i = 0; i < num_iter; i++)
  {
    (void)hpcHardwareRpc  (L7_ALL_UNITS,
                           TEST2_ID,
                           &rpc_data,
                           rpc_status,
                           0, 0);
  }
  end_time = osapiTimeMillisecondsGet ();

  //delta_time = end_time - start_time;
  delta_time = osapiTimeMillisecondsDiff(end_time,start_time);
  if (delta_time > 10000)
  {
  transaction_rate = num_iter / (delta_time / 1000);
  } else
  {
    transaction_rate = (num_iter * 1000) / delta_time;
  }

  printf("Completed %d iterations in %d milliseconds (%d transactions per second)\n",
         num_iter,
         delta_time,
         transaction_rate);

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    printf("Unit: %d   rpc_status: %d   \n", i, rpc_status[i]);
  }

  return 0;
}

/********
** Invoke Test 3: RPC to one unit
********/
L7_uint32 hpcDebugTest3(L7_uint32 unit, L7_uint32 num_iter, L7_uint32 sleep_msec)
{
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t    rpc_data;
  L7_uint32 i;
  L7_uint32 start_time;
  L7_uint32 end_time;
  L7_uint32 transaction_rate;
  L7_uint32 delta_time;

  start_time = osapiTimeMillisecondsGet ();


  printf("Starting test. Iterations: %d, Sleep Time %d ms, Start Time %d\n",
                num_iter,
              sleep_msec,
              start_time);

  rpc_data.buf = (L7_uchar8 *) &sleep_msec;
  rpc_data.buf_size = rpc_data.data_len = sizeof(sleep_msec);

  for (i = 0; i < num_iter; i++)
  {
    (void)hpcHardwareRpc  (unit,
                           TEST2_ID,
                           &rpc_data,
                           rpc_status,
                           app_status,
                           L7_NULL);
  }

  end_time = osapiTimeMillisecondsGet ();

  //delta_time = end_time - start_time;
  delta_time = osapiTimeMillisecondsDiff(end_time,start_time);
  if (delta_time > 10000)
  {
  transaction_rate = num_iter / (delta_time / 1000);
  } else
  {
    transaction_rate = (num_iter * 1000) / delta_time;
  }

  printf("Completed %d iterations in %d milliseconds (%d transactions per second)\n",
         num_iter,
         delta_time,
         transaction_rate);

  printf("rpc_status: %d   app_status: %d\n", rpc_status[0], app_status[0]);
 

  return 0;
}

/********
** Invoke Test 1 with rpc response
********/
L7_uint32 hpcDebugTestResp(L7_uint32 param)
{
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32 buf [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t rpc_resp[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t rpc_data;
  L7_RC_t rc;
  L7_uint32 i;

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    rpc_resp[i].buf = (L7_uchar8 *)&buf[i];
    rpc_resp[i].buf_size = sizeof(L7_uint32);
    rpc_resp[i].data_len = 0;
    buf[i] = 0;
  }
 
  rpc_data.buf = (L7_uchar8 *)&param;
  rpc_data.data_len = rpc_data.buf_size = sizeof(param);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        TEST1_ID,
                        &rpc_data,
                        rpc_status,
                        app_status,
                        rpc_resp);

  printf("hpcHardwareRpc - rc = %d\n", rc);

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    printf("Unit: %d   rpc_status: %d   app_status: %d  rpc_response: %x, resp_len %d\n", 
           i, rpc_status[i], app_status[i], *(L7_uint32 *)rpc_resp[i].buf,
           rpc_resp[i].data_len);
  }

  return 0;
}

/********
** Invoke Test 1 to a particular unit with rpc response
********/
L7_uint32 hpcDebugTestResp1(L7_uint32 unit, L7_uint32 param)
{
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32 buf;
  hpcHwRpcData_t rpc_resp[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t rpc_data;
  L7_RC_t rc;

  rpc_resp[0].buf = (L7_uchar8 *)&buf;
  rpc_resp[0].buf_size = sizeof(L7_uint32);
  rpc_resp[0].data_len = 0;
  buf = 0;
 
  rpc_data.buf = (L7_uchar8 *)&param;
  rpc_data.data_len = rpc_data.buf_size = sizeof(param);

  rc = hpcHardwareRpc  (unit,
                        TEST1_ID,
                        &rpc_data,
                        rpc_status,
                        app_status,
                        rpc_resp);

  printf("hpcHardwareRpc - rc = %d\n", rc);

  printf("Unit: %d   rpc_status: %d   app_status: %d  rpc_response: %x, resp_len %d\n", 
          unit, rpc_status[0], app_status[0], *(L7_uint32 *)rpc_resp[0].buf,
          rpc_resp[0].data_len);
 

  return 0;
}
#endif

