
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_bcmx_l2_mcast.c
*
* @purpose    New bcmx layer that issues commands to a list of units
*
* @component  HAPI
*
* @comments   none
*
* @create     07/14/2006
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "soc/drv.h"
#include "log.h"
#include "sal/core/libc.h"
#include "bcm/types.h"  // PTin added: compilation fix for SDK 5.10.4
#include "bcm_int/esw/multicast.h"
#include "bcm/custom.h"
#include "bcmx/custom.h"

#include "l7_rpc_l2_mcast.h"
#include "l7_usl_bcm_port.h"

#define RV_REPLACE(_trv, _rv)                   \
        BCMX_RV_REPLACE_OK(_trv, _rv, BCM_E_UNAVAIL)


/*********************************************************************
* @purpose Handle L2MC RPC commands
*
* @param    transaction_id - transaction id
* @param    buf - buffer pointer to egress object information
* @param    buf_len - buffer length
* @param    status - return code 
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_mcast_handler(L7_uint32 transaction_id,
                                    hpcHwRpcData_t *rpc_data,
                                    L7_int32 *status,
                                    hpcHwRpcData_t  *rpc_resp)
{
  L7_RC_t              rc = L7_SUCCESS;
  L7_uint32            rpcCmd;
  USL_CMD_t            updateCmd;
  L7_uchar8           *msgBuf;
  usl_bcm_mcast_addr_t bcmMcastAddr;
  L7_int32             rv = BCM_E_FAIL;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = rv;
    return L7_SUCCESS;
  }

  msgBuf = rpc_data->buf;

  L7_UINT32_UNPACK(msgBuf, rpcCmd);
  L7_UINT32_UNPACK(msgBuf, updateCmd);

  switch (rpcCmd)
  {
    /* Add/remove a multicast group */
    case L7_RPC_CMD_MCAST_ADDR_UPDATE: 
      memcpy(&bcmMcastAddr, msgBuf, sizeof(bcmMcastAddr));
      msgBuf += sizeof(bcmMcastAddr);

      if (updateCmd == USL_CMD_ADD)
      {
        rv = usl_bcm_mcast_addr_add(&bcmMcastAddr);    
      }
      else
      {
        rv = usl_bcm_mcast_addr_remove(&bcmMcastAddr);    
      }
      break;

    /* Add/remove ports to/from multicast group */
    case L7_RPC_CMD_MCAST_PORT_UPDATE:
      memcpy(&bcmMcastAddr, msgBuf, sizeof(bcmMcastAddr));
      msgBuf += sizeof(bcmMcastAddr);

      rv = usl_bcm_mcast_ports_add_remove(&bcmMcastAddr, updateCmd);
      break;

    default:
       rc = L7_NOT_SUPPORTED;
       break;
  }

  *status = rv;

  return rc;
}

/*********************************************************************
* @purpose  Initialize custom L2 Multicast RPC calls.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_l2_mcast_rpc_init (void)
{
  L7_RC_t rc;

  rc = hpcHardwareRpcRegister (L7_RPC_L2MCAST_HANDLER,
                               l7_rpc_server_mcast_handler);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  return rc;
}
/*********************************************************************
* @purpose  Add/Remove a L2 Multicast MAC entry to the HW's Database
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    updateCmd    @{(input)} Add/Remove the multicast mac
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int  l7_rpc_client_mcast_addr_update(usl_bcm_mcast_addr_t *mcAddr, 
                                     USL_CMD_t updateCmd,
                                     L7_uint32 numChips, L7_uint32 *chips)
{
  L7_RC_t             rc;
  L7_int32            appStatus [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32           i, msgSize;
  L7_uchar8          *msgBuf, *rpcBuf;
  int                 rv;
  hpcHwRpcStatus_t    rpcStatus [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t      rpcData;

  rpcBuf = l7_rpc_req_buffer_get();
  if (rpcBuf == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to get RPC request buffer\n");

    rv = BCM_E_FAIL;
    return rv;
  }

  msgBuf = rpcBuf;

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (*mcAddr) + sizeof(updateCmd);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_RPC_CMD_MCAST_ADDR_UPDATE);
  L7_UINT32_PACK(msgBuf, updateCmd);
  memcpy(msgBuf, mcAddr, sizeof(*mcAddr));
  msgBuf += sizeof(*mcAddr);
  

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_L2MCAST_HANDLER,
                        &rpcData,
                        rpcStatus,
                        appStatus, L7_NULL);
  if (rc == L7_SUCCESS)
  {
    /* Loop through units that successfully completed RPC and
    ** return the lowest return code. Since RPC is also executed
    ** on the local unit we are guaranteed at least one successful completion.
    */
    rv = BCM_E_NONE;
    for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
    {
      if (rpcStatus[i] == HPC_HW_RPC_OK)
      {
        if (appStatus[i] < rv)
        {
          rv = appStatus[i];
        }
      }
    }
  } else
  {
    /* RPC is not ready. This is probably a transient error, so treat it as a timeout.
    */
    rv = BCM_E_TIMEOUT;
  }

  l7_rpc_req_buffer_free(rpcBuf);

  return rv;
}

/*********************************************************************
* @purpose  Add/Remove ports to/from a L2 Multicast Group 
*
* @param    mcAddr       @{(input)} Multicast address information
* @param    updateCmd    @{(input)} Add/Remove the multicast mac
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int  l7_rpc_client_mcast_update_ports(usl_bcm_mcast_addr_t *mcAddr, 
                                      USL_CMD_t updateCmd,
                                      L7_uint32 numChips, L7_uint32 *chips)
{
  L7_RC_t             rc;
  L7_int32            appStatus [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32           i, msgSize;
  L7_uchar8          *msgBuf, *rpcBuf;
  int                 rv;
  hpcHwRpcStatus_t    rpcStatus [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t      rpcData;

  rpcBuf = l7_rpc_req_buffer_get();
  if (rpcBuf == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to get RPC request buffer\n");

    rv = BCM_E_FAIL;
    return rv;
  }

  msgBuf = rpcBuf;

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (*mcAddr) + sizeof(updateCmd);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_RPC_CMD_MCAST_PORT_UPDATE);
  L7_UINT32_PACK(msgBuf, updateCmd);
  memcpy(msgBuf, mcAddr, sizeof(*mcAddr));
  msgBuf += sizeof(*mcAddr);


  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_L2MCAST_HANDLER,
                        &rpcData,
                        rpcStatus,
                        appStatus, L7_NULL);
  if (rc == L7_SUCCESS)
  {
    /* Loop through units that successfully completed RPC and
    ** return the lowest return code. Since RPC is also executed
    ** on the local unit we are guaranteed at least one successful completion.
    */
    rv = BCM_E_NONE;
    for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
    {
      if (rpcStatus[i] == HPC_HW_RPC_OK)
      {
        if (appStatus[i] < rv)
        {
          rv = appStatus[i];
        }
      }
    }
  } else
  {
    /* RPC is not ready. This is probably a transient error, so treat it as a timeout.
    */
    rv = BCM_E_TIMEOUT;
  }

  l7_rpc_req_buffer_free(rpcBuf);

  return rv;
}

/*********************************************************************
* @purpose  RPC client API to add a port to a list of L2 Multicast addresses
*
* @param    port              @{(input)} port being operated on
* @param    l2mc_index        @{(input)} array of L2MC indices
* @param    l2mc_index_count  @{(input)} count of L2MC indices
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_mcast_port_update_groups(bcm_gport_t port, int *l2mc_index, int l2mc_index_count, USL_CMD_t updateCmd)
{
  int        rv = BCM_E_NONE;
  uint32     argSize;
  uint32     args[BCM_CUSTOM_ARGS_MAX];
  L7_uchar8 *msgBuf = (L7_uchar8 *)args;
  L7_uint32  i;

  argSize = (l2mc_index_count * sizeof(*l2mc_index)) + sizeof(updateCmd);
  if (argSize > sizeof(args))
  {
    LOG_ERROR(argSize);
  }

  L7_UINT32_PACK(msgBuf, updateCmd);
  L7_UINT32_PACK(msgBuf, l2mc_index_count);
  for (i = 0; i < l2mc_index_count; i++)
  {
    L7_UINT32_PACK(msgBuf, l2mc_index[i]);
  }

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_MCAST_GROUPS_UPDATE, 1+l2mc_index_count, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_MCAST_GROUPS_UPDATE, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose  RPC server API to add a port to a list of L2 Multicast addresses
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} Bcast rate threshold data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_mcast_port_update_groups(int unit, bcm_port_t port, 
                                           int setget, uint32 *args)
{
  int        l2mc_index_count;
  USL_CMD_t  updateCmd;
  int        rv;
  L7_uchar8 *msgBuf = (L7_uchar8 *)args;
  L7_uint32  l2mc_index[BCM_CUSTOM_ARGS_MAX];
  L7_uint32  i;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  L7_UINT32_UNPACK(msgBuf, updateCmd);
  L7_UINT32_UNPACK(msgBuf, l2mc_index_count);
  for (i = 0; i < l2mc_index_count; i++)
  {
    L7_UINT32_UNPACK(msgBuf, l2mc_index[i]);
  }
  
  rv = usl_bcm_port_mcast_groups_update(unit, port, l2mc_index, l2mc_index_count, updateCmd);

  return rv;
}
