
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_bcmx_l2.c
*
* @purpose    New bcmx layer that issues commands to a list of units
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "comm_mask.h"
#include "osapi.h"
#include "ibde.h"
#include "log.h"
#include "broad_common.h"
#include "bcm/stack.h"
#include "hpc_hw_api.h"
#include "soc/drv.h"
#include "sal/core/libc.h"

#include "l7_usl_mask.h"
#include "l7_rpc_l2.h"

#ifdef L7_STACKING_PACKAGE
#include "l7_usl_macsync.h"
#endif

#include "broad_mmu.h"

#define L7_RPC_DVLAN_HANDLER                       HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,98)
#define L7_RPC_PROTECTED_GROUP_SET_HANDLER         HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,299)
#define L7_RPC_TRUNK_LEARN_MODE_HANDLER            HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,300)
#define L7_RPC_VLAN_BULK_HANDLER                   HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,301)
#define L7_RPC_PROTECTED_GROUP_DELETE_HANDLER      HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,302)
#define L7_RPC_TRUNK_HANDLER                       HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,303)
#define L7_RPC_STG_HANDLER                         HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,304)
#define L7_RPC_VLAN_IP_SUBNET_ADD_HANDLER          HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,305)
#define L7_RPC_VLAN_IP_SUBNET_DELETE_HANDLER       HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,306)
#define L7_RPC_VLAN_MAC_ADD_HANDLER                HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,307)
#define L7_RPC_VLAN_MAC_DELETE_HANDLER             HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,308)
#define L7_RPC_VLAN_HANDLER                        HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,309)
#define L7_RPC_FLOW_CONTROL_HANDLER                HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,310)
#define L7_RPC_VLAN_MCAST_FLOOD_SET_HANDLER        HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,311)
#define L7_RPC_VLAN_CONTROL_FLAG_UPDATE_HANDLER    HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,312)
#define L7_RPC_VLAN_CONTROL_FWD_MODE_HANDLER       HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,313)
#define L7_RPC_IPMCAST_ENABLE_SET                  HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,314)
#define L7_RPC_DOS_CONTROL_HANDLER                 HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,315)
#define L7_RPC_DVLAN_TRANSLATION_HANDLER           HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,702)
#define L7_RPC_DVLAN_TPID_HANDLER                  HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,703)
#define L7_RPC_MMU_DROPMODE_HANDLER                HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,704)


/* Message to send bulk vlan create/delete cmds */
static vlanBulkMsgElem_t *uslVlanBulkDataMsg;
static L7_uint32 uslVlanBulkDataMsgSize = 0;

/* Message to receive bulk vlan create/delete cmd return codes */
static L7_int32 *uslVlanBulkResponseMsg;
static L7_uint32 uslVlanBulkResponseMsgSize = 0;

/* Buffer to hold return codes from each unit for vlan bulk configuration */
/* Resp data from each unit */
static hpcHwRpcData_t  rpcVlanBulkResponse[L7_MAX_UNITS_PER_STACK + 1];

/*********************************************************************
* @purpose  Add a L2 MAC address to the HW's forwarding database
*
* @param    l2addr     @{(input)} the MAC address, settings, and flags
* @param    port_block @{(input)} List of ports to filter for this addr
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_bcmx_l2_addr_add(bcmx_l2_addr_t * l2addr, bcmx_lplist_t *port_block,
                        L7_uint32 numChips, L7_uint32 *chips)
{
  int rc = BCM_E_NONE;

  rc = bcmx_l2_addr_add(l2addr,port_block);

  return rc;
}

/*********************************************************************
* @purpose  Remove a L2 MAC address from the HW's forwarding database
*
* @param    mac_addr     @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_bcmx_l2_addr_delete(bcm_mac_t mac_addr, bcm_vlan_t vid,
                           L7_uint32 numChips, L7_uint32 *chips)
{
  int rc = BCM_E_NONE;

  rc = bcmx_l2_addr_delete(mac_addr, vid);

  return rc;
}

/*********************************************************************
* @purpose  Set the aging time
*
* @param    ageTime     @{(input)} The aging time in seconds
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_bcmx_l2_age_timer_set(L7_int32 ageTime)
{
  return bcmx_l2_age_timer_set(ageTime);
}


/*********************************************************************
* @purpose  Makes the rpc call for a dvlan mode change.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/

int l7_rpc_client_dvlan_mode_set(L7_uint32 mode)
{
    int rc;
    int              app_status[L7_MAX_UNITS_PER_STACK + 1];
    hpcHwRpcStatus_t rpc_status[L7_MAX_UNITS_PER_STACK + 1];
    hpcHwRpcData_t   rpc_data;
    L7_uint32        i;

    rpc_data.buf = (L7_uchar8*)&mode;
    rpc_data.buf_size = rpc_data.data_len = sizeof(L7_uint32);

    rc = hpcHardwareRpc(L7_ALL_UNITS,
                        L7_RPC_DVLAN_HANDLER,
                        &rpc_data,
                        rpc_status,
                        app_status, L7_NULL);

    if (L7_SUCCESS == rc)
    {
      for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
      {
        if (HPC_HW_RPC_OK == rpc_status[i])
        {
          if (app_status[i] < BCM_E_NONE) 
          {
            return app_status[i];
          }
        }
      }
    }
    else
    {
      return BCM_E_TIMEOUT;
    }

    return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Makes the rpc call for a dvlan translation mode change.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_dvlan_translation_set(L7_BOOL direction, L7_BOOL enable)
{
    int rc;
    hpcHwRpcStatus_t rpc_status[L7_MAX_UNITS_PER_STACK + 1];
    int              app_status[L7_MAX_UNITS_PER_STACK + 1];
    hpcHwRpcData_t   rpc_data;
    L7_uint32        i;
    L7_uint32 msg[2];

    msg[0] = direction;
    msg[1] = enable;

    rpc_data.buf = (L7_uchar8*)msg;
    rpc_data.buf_size = rpc_data.data_len = sizeof(msg);

    rc = hpcHardwareRpc(L7_ALL_UNITS,
                        L7_RPC_DVLAN_TRANSLATION_HANDLER,
                        &rpc_data,
                        rpc_status,
                        app_status, L7_NULL);

    if (L7_SUCCESS == rc)
    {
      for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
      {
        if (HPC_HW_RPC_OK == rpc_status[i])
        {
          if (app_status[i] < BCM_E_NONE) 
          {
            return app_status[i];
          }
        }
      }
    }
    else
    {
      return BCM_E_TIMEOUT;
    }

    return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Makes the rpc call for a TPID change.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_dvlan_tpid_set(L7_ushort16 etherType)
{
    int rc;
    int              app_status[L7_MAX_UNITS_PER_STACK + 1];
    hpcHwRpcStatus_t rpc_status[L7_MAX_UNITS_PER_STACK + 1];
    hpcHwRpcData_t   rpc_data;
    L7_uint32        i;
    L7_ushort16 msg;

    msg = etherType;

    rpc_data.buf = (L7_uchar8*)&msg;
    rpc_data.buf_size = rpc_data.data_len = sizeof(msg);

    rc = hpcHardwareRpc(L7_ALL_UNITS,
                        L7_RPC_DVLAN_TPID_HANDLER,
                        &rpc_data,
                        rpc_status,
                        app_status, L7_NULL);

    if (L7_SUCCESS == rc)
    {
      for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
      {
        if (HPC_HW_RPC_OK == rpc_status[i])
        {
          if (app_status[i] < BCM_E_NONE) 
          {
            return app_status[i];
          }
        }
      }
    }
    else
    {
      return BCM_E_TIMEOUT;
    }

    return BCM_E_NONE;
} 

/*********************************************************************
* @purpose  Custom RPC function to handle DVLAN mode changes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/

L7_RC_t l7_rpc_server_dvlan_mode_set(L7_uint32 tid,
                                     hpcHwRpcData_t *rpc_data,
                                     L7_int32 *status,
                                     hpcHwRpcData_t  *rpc_resp)
{
    int rv;
    L7_uint32 mode;
    
    if (rpc_data->data_len != sizeof(L7_uint32))
    {
        *status = BCM_E_FAIL;
        return L7_SUCCESS;
    }
    mode = *(L7_uint32 *) rpc_data->buf;

    rv = usl_bcm_dvlan_mode_set(mode);

    *status = rv;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Custom RPC function to handle DVLAN translation changes.
*
* @param
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_dvlan_translation_set(L7_uint32        tid,
                                            hpcHwRpcData_t  *rpc_data,
                                            L7_int32        *status,
                                            hpcHwRpcData_t  *rpc_resp)
{
    int rv;
    L7_BOOL direction;
    L7_BOOL enable;
    

    if (rpc_data->data_len != 2*sizeof(L7_uint32))
    {
        *status = BCM_E_FAIL;
        return L7_SUCCESS;
    }
    direction  = *(L7_uint32 *) rpc_data->buf;
    enable     = *(L7_uint32 *) (rpc_data->buf + 4);

    rv = usl_bcm_dvlan_translation_set(direction, enable);

    *status = rv;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Makes the rpc call for a TPID change.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_dvlan_tpid_set(L7_uint32        tid,
                                     hpcHwRpcData_t  *rpc_data,
                                     L7_int32        *status,
                                     hpcHwRpcData_t  *rpc_resp)
{
    int rv;
    L7_ushort16 tpid;

    if (rpc_data->data_len != sizeof(L7_ushort16))
    {
        *status = BCM_E_FAIL;
        return L7_SUCCESS;
    }
    tpid = *(L7_ushort16 *) rpc_data->buf;

    rv = usl_bcm_dvlan_tpid_set(tpid);

    *status = rv;
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  DVLAN Init routing to register the RPC call handler.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
void l7_dvlan_rpc_init()
{
    L7_RC_t rc;

    rc = hpcHardwareRpcRegister(L7_RPC_DVLAN_HANDLER, l7_rpc_server_dvlan_mode_set);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR(rc);
    }

    rc = hpcHardwareRpcRegister(L7_RPC_DVLAN_TRANSLATION_HANDLER, 
                                l7_rpc_server_dvlan_translation_set);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR(rc);
    }

    rc = hpcHardwareRpcRegister(L7_RPC_DVLAN_TPID_HANDLER,
                                l7_rpc_server_dvlan_tpid_set);

    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR(rc);
    }

}


/*********************************************************************
* @purpose  Delete a port from protected port group 
*
*
*
* @returns  Defined by Broadcom driver
*
* @notes   
*
* @end
*********************************************************************/

int l7_rpc_client_protected_group_port_remove(usl_bcm_protected_group_delete_t 
                                                *deleteInfo) 
{
  hpcHwRpcStatus_t           rpc_status[L7_MAX_UNITS_PER_STACK + 1];
  int                        app_status[L7_MAX_UNITS_PER_STACK + 1];
  int                        rv = BCM_E_NONE;
  L7_RC_t                    rc;
  hpcHwRpcData_t             rpc_data;

  if (sizeof(*deleteInfo)  > hpcHardwareRpcMaxMessageLengthGet())
    return BCM_E_FAIL;

  rpc_data.buf = (L7_uchar8 *) deleteInfo;
  rpc_data.buf_size = rpc_data.data_len = sizeof(*deleteInfo);

  rc = hpcHardwareRpc(L7_ALL_UNITS,
                      L7_RPC_PROTECTED_GROUP_DELETE_HANDLER,
                      &rpc_data,
                      rpc_status,
                      app_status, L7_NULL);
  if (L7_SUCCESS == rc)
  {
    int i;

    for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
    {
      if (HPC_HW_RPC_OK == rpc_status[i])
      {
        if (app_status[i] < rv)
          rv = app_status[i];
      }
    }
  }
  else
  {
    rv = BCM_E_TIMEOUT;
  }

  return rv;
}


/*********************************************************************
* @purpose  Custom RPC function to set protected group membership
*
* @param    
*
* @returns  
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_protected_group_port_remove(L7_uint32 tid,
                                                  hpcHwRpcData_t *rpc_data,
                                                  L7_int32 *status,
                                                  hpcHwRpcData_t  *rpc_resp)
{
  int                             rv = BCM_E_NONE;
  usl_bcm_protected_group_delete_t *deleteInfo;

  deleteInfo = (usl_bcm_protected_group_delete_t *) rpc_data->buf;

  rv = usl_bcm_protected_group_port_remove(deleteInfo);

  *status = rv;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Make the RPC call to set the protected port group membership
*
* @param    group_list          @{(input)} pointer to list of protected 
*                                          group members
*
* @returns  Defined by Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_protected_group_set(usl_bcm_protected_group_t *group_list)
{
    hpcHwRpcStatus_t           rpc_status[L7_MAX_UNITS_PER_STACK + 1];
    int                        app_status[L7_MAX_UNITS_PER_STACK + 1];
    int                        rv = BCM_E_NONE;
    L7_RC_t                    rc;
    hpcHwRpcData_t             rpc_data;

    if (sizeof(usl_bcm_protected_group_t) > hpcHardwareRpcMaxMessageLengthGet())
        return BCM_E_FAIL;

    rpc_data.buf = (L7_uchar8*)group_list;
    rpc_data.buf_size = rpc_data.data_len = sizeof(usl_bcm_protected_group_t);

    rc = hpcHardwareRpc(L7_ALL_UNITS,
                        L7_RPC_PROTECTED_GROUP_SET_HANDLER,
                        &rpc_data,
                        rpc_status,
                        app_status, L7_NULL);
    if (L7_SUCCESS == rc)
    {
        int i;

        for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
        {
            if (HPC_HW_RPC_OK == rpc_status[i])
            {
                if (app_status[i] < rv)
                    rv = app_status[i];
            }
        }
    }
    else
    {
        rv = BCM_E_TIMEOUT;
    }

    return rv;
}

/*********************************************************************
* @purpose  Custom RPC function to set protected group membership
*
* @param    
*
* @returns  
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_protected_group_set(L7_uint32 tid,
                                          hpcHwRpcData_t *rpc_data,
                                          L7_int32 *status,
                                          hpcHwRpcData_t  *rpc_resp)
{
  int                     rv = BCM_E_NONE;
  L7_uchar8              *pBuf;
  usl_bcm_protected_group_t groupInfo;

  if (rpc_data->data_len != sizeof(usl_bcm_protected_group_t))
  {
    *status = BCM_E_FAIL;
    return L7_SUCCESS;
  }

  pBuf = rpc_data->buf;
  memcpy(&groupInfo, pBuf, sizeof(groupInfo));
  
  rv = usl_bcm_protected_group_set(&groupInfo);
  
  *status = rv;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Protected group init routine to register the RPC call handler.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_protected_group_rpc_init(void)
{
    L7_RC_t rc;

    rc = hpcHardwareRpcRegister(L7_RPC_PROTECTED_GROUP_SET_HANDLER, 
                                l7_rpc_server_protected_group_set);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR(rc);
    }

    rc = hpcHardwareRpcRegister(L7_RPC_PROTECTED_GROUP_DELETE_HANDLER, 
                                l7_rpc_server_protected_group_port_remove);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR(rc);
    }
  
  return rc;
}


/*********************************************************************
* @purpose  Create a VLAN in the HW's table
*
* @param    vid      @{(input)} the VLAN ID to be created
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_vlan_create(bcm_vlan_t vid,
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (vid);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_TRUE);
  L7_UINT16_PACK(msgBuf, vid);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_HANDLER,
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
* @purpose  Create a VLAN in the HW's table
*
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_vlan_update (L7_uint32 transaction_id,
                                   hpcHwRpcData_t *rpc_data,
                                   L7_int32 *status,
                                   hpcHwRpcData_t  *rpc_resp)
{
  int               rv = BCM_E_NONE;
  L7_uchar8        *pBuf;
  L7_uint32         cmd;
  bcm_vlan_t        vid;
  

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  pBuf = rpc_data->buf;

  L7_UINT32_UNPACK(pBuf, cmd);
  L7_UINT16_UNPACK(pBuf, vid);

  rv = usl_bcm_vlan_update(vid, cmd);  

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Remove the VLAN from the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be removed
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int l7_rpc_client_vlan_destroy(bcm_vlan_t vid,
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (vid);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_FALSE);
  L7_UINT16_PACK(msgBuf, vid);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_HANDLER,
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
* @purpose  RPC Client function to set the mcast flood mode for a vlan
*
* @param    vid          @{(input)} 
* @param    floodMode    @{(input)}
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int l7_rpc_client_vlan_mcast_flood_set(bcm_vlan_t vid,
                                       bcm_vlan_mcast_flood_t mcastFloodMode,
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (vid) + sizeof(mcastFloodMode);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT16_PACK(msgBuf, vid);
  L7_UINT32_PACK(msgBuf, mcastFloodMode);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_MCAST_FLOOD_SET_HANDLER,
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
* @purpose  RPC Server function to set the mcast flood mode for a vlan
*
* @param    Standard RPC parameters
*
* @returns  L7_RC_t
*       
* @end
*********************************************************************/

L7_RC_t l7_rpc_server_vlan_mcast_flood_set (L7_uint32 transaction_id,
                                            hpcHwRpcData_t *rpc_data,
                                            L7_int32 *status,
                                            hpcHwRpcData_t  *rpc_resp)
{
  int               rv = BCM_E_NONE;
  L7_uchar8        *pBuf;
  L7_uint32         mcastFloodMode;
  bcm_vlan_t        vid;
  

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  pBuf = rpc_data->buf;

  L7_UINT16_UNPACK(pBuf, vid);
  L7_UINT32_UNPACK(pBuf, mcastFloodMode);

  rv = usl_bcm_vlan_mcast_flood_set(vid, mcastFloodMode);  

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Update the control flags for a vlan
*
* @param    vid          @{(input)} 
* @param    flags        @{(input)} 
* @param    cmd          @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int l7_rpc_client_vlan_control_flag_update(bcm_vlan_t vid,
                                           L7_uint32  flags,
                                           USL_CMD_t    cmd,
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (vid) + sizeof(flags) + sizeof(cmd);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT16_PACK(msgBuf, vid);
  L7_UINT32_PACK(msgBuf, flags);
  L7_UINT32_PACK(msgBuf, cmd);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_CONTROL_FLAG_UPDATE_HANDLER,
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
* @purpose  Update the control flags for a vlan
*
* @param    vid               @{(input)} 
* @param    forwarding_mode   @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int l7_rpc_client_vlan_control_fwd_mode_set(bcm_vlan_t vid, bcm_vlan_forward_t forwarding_mode)
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (vid) + sizeof(forwarding_mode);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT16_PACK(msgBuf, vid);
  memcpy(msgBuf, &forwarding_mode, sizeof(forwarding_mode));
  msgBuf += sizeof(forwarding_mode);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_CONTROL_FWD_MODE_HANDLER,
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
* @purpose  RPC Server function to set the mcast flood mode for a vlan
*
* @param    Standard RPC parameters
*
* @returns  L7_RC_t
*       
* @end
*********************************************************************/

L7_RC_t l7_rpc_server_vlan_control_flag_update (L7_uint32 transaction_id,
                                                hpcHwRpcData_t *rpc_data,
                                                L7_int32 *status,
                                                hpcHwRpcData_t  *rpc_resp)
{
  int               rv = BCM_E_NONE;
  L7_uchar8        *pBuf;
  L7_uint32         flags;
  L7_BOOL           set;
  USL_CMD_t         cmd;
  bcm_vlan_t        vid;
  

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  pBuf = rpc_data->buf;

  L7_UINT16_UNPACK(pBuf, vid);
  L7_UINT32_UNPACK(pBuf, flags);
  L7_UINT32_UNPACK(pBuf, set);

  if (set == L7_TRUE)
  {
    cmd = USL_CMD_ADD;    
  }
  else
  {
    cmd = USL_CMD_REMOVE;
  }

  rv = usl_bcm_vlan_control_flag_update(vid, flags, cmd);  

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  RPC Server function to set the fwd mode for a vlan
*
* @param    Standard RPC parameters
*
* @returns  L7_RC_t
*       
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_vlan_control_fwd_mode_set(L7_uint32       transaction_id,
                                                hpcHwRpcData_t *rpc_data,
                                                L7_int32       *status,
                                                hpcHwRpcData_t *rpc_resp)
{
  int                rv = BCM_E_NONE;
  L7_uchar8         *pBuf;
  bcm_vlan_forward_t forwarding_mode;
  bcm_vlan_t         vid;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  pBuf = rpc_data->buf;

  L7_UINT16_UNPACK(pBuf, vid);
  memcpy(&forwarding_mode, pBuf, sizeof(forwarding_mode));
  pBuf += sizeof(forwarding_mode);

  rv = usl_bcm_vlan_control_fwd_mode_set(vid, forwarding_mode);  

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the max number of vlans that can be handled in a RPC transaction
*
* @param    none
*
* @returns  Number of vlan entries per RPC transaction
*
* @end
*********************************************************************/
L7_uint32 l7_rpc_bulk_vlan_max_msg_get(void)
{
  L7_uint32 maxRpcLen = 0;
  maxRpcLen = hpcHardwareRpcMaxMessageLengthGet();

  if (maxRpcLen == 0)
  {
    L7_LOG_ERROR(0);
  }


  return ((maxRpcLen - USL_VLAN_BULK_MSG_HDR_SIZE) /sizeof(vlanBulkMsgElem_t));
}

/*********************************************************************
* @purpose  Custom RPC function to configure vlans
*
* @param    
*
* @returns  
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_vlan_bulk_update(L7_uint32 tid, 
                                       hpcHwRpcData_t *rpcData,
                                       L7_int32 *status,
                                       hpcHwRpcData_t  *rpcResp)
{
  int             rv = BCM_E_NONE;
  L7_uchar8      *dataBuffer, *responseBuffer, *responseMsg;
  L7_uint32       numElems, numFailures = 0, vlanId;
  L7_BOOL         msgType;


  dataBuffer  = (L7_uchar8 *)rpcData->buf;
  responseBuffer = (L7_uchar8 *)rpcResp->buf;
  responseMsg = (L7_uchar8 *)rpcResp->buf;

  msgType  = *(L7_uint32 *) &dataBuffer[USL_VLAN_BULK_MSG_TYPE_OFFSET];
  numElems = *(L7_uint32 *) &dataBuffer[USL_VLAN_BULK_MSG_ELEM_OFFSET];


  dataBuffer += USL_VLAN_BULK_MSG_DATA_OFFSET;
  responseBuffer += USL_VLAN_BULK_MSG_DATA_OFFSET;

  while (numElems > 0) 
  {
    vlanId = *(L7_uint32 *) dataBuffer;  
    rv = BCM_E_NONE;

    rv = usl_bcm_vlan_update(vlanId, msgType);

    *(L7_int32 *)responseBuffer = rv;
    responseBuffer += sizeof(L7_int32);
    dataBuffer     += sizeof(L7_uint32);

    if (rv != BCM_E_NONE) 
      numFailures++;

    numElems--;
  } /* End while */
  
  *(L7_uint32 *)&responseMsg[USL_VLAN_BULK_MSG_TYPE_OFFSET] = (L7_uint32)msgType;
  *(L7_uint32 *)&responseMsg[USL_VLAN_BULK_MSG_ELEM_OFFSET] = (L7_uint32)numFailures;
 
   rpcResp->data_len = rpcResp->buf_size;

  *status = BCM_E_NONE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose VLAN bulk init to register the RPC call handler.
*
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_vlan_rpc_init()
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unitIdx, maxNumVlansPerMsg;
 
  do
  {
    /* Allocate memory for vlan bulk RPC message commands */

    maxNumVlansPerMsg = l7_rpc_bulk_vlan_max_msg_get();

    uslVlanBulkDataMsgSize = USL_VLAN_BULK_MSG_HDR_SIZE + 
                             maxNumVlansPerMsg *  sizeof(vlanBulkMsgElem_t);

    uslVlanBulkDataMsg = (vlanBulkMsgElem_t *) osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                                           uslVlanBulkDataMsgSize
                                                          );
    if (uslVlanBulkDataMsg == L7_NULLPTR) 
    {
      break;
    }

    uslVlanBulkResponseMsgSize = USL_VLAN_BULK_MSG_HDR_SIZE + 
                                 maxNumVlansPerMsg *  sizeof(vlanBulkMsgElem_t);

    uslVlanBulkResponseMsg = (L7_int32 *) osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                                      uslVlanBulkResponseMsgSize
                                                      );
    if (uslVlanBulkResponseMsg == L7_NULLPTR) 
    {
      break;
    }


    rc = L7_SUCCESS;
    for (unitIdx = 0; unitIdx < (L7_MAX_UNITS_PER_STACK + 1); unitIdx++) 
    {
      rpcVlanBulkResponse[unitIdx].buf = (L7_uchar8 *) osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                                                   uslVlanBulkResponseMsgSize
                                                                  );
      if (rpcVlanBulkResponse[unitIdx].buf == L7_NULLPTR) 
      {
        rc = L7_FAILURE;
        break;
      }

      rpcVlanBulkResponse[unitIdx].buf_size = uslVlanBulkResponseMsgSize;
    }

    if (rc != L7_SUCCESS)
    {
      break; 
    }

    rc = hpcHardwareRpcRegister(L7_RPC_VLAN_BULK_HANDLER, l7_rpc_server_vlan_bulk_update);
    if (rc != L7_SUCCESS)
    {
      break;
    }

    rc = hpcHardwareRpcRegister(L7_RPC_VLAN_HANDLER, l7_rpc_server_vlan_update);
    if (rc != L7_SUCCESS)
    {
      break;
    }

    rc = hpcHardwareRpcRegister(L7_RPC_VLAN_MCAST_FLOOD_SET_HANDLER, 
                                l7_rpc_server_vlan_mcast_flood_set);
    if (rc != L7_SUCCESS)
    {
      break;
    }

    rc = hpcHardwareRpcRegister(L7_RPC_VLAN_CONTROL_FLAG_UPDATE_HANDLER, 
                                l7_rpc_server_vlan_control_flag_update);
    if (rc != L7_SUCCESS)
    {
      break;
    }

    rc = hpcHardwareRpcRegister(L7_RPC_VLAN_CONTROL_FWD_MODE_HANDLER, 
                                l7_rpc_server_vlan_control_fwd_mode_set);
    if (rc != L7_SUCCESS)
    {
      break;
    }

  } while (0);
 
  return rc;
} 

/*********************************************************************
* @purpose  Make the RPC call to perform the vlan bulk configuration
* @param    vlanBulkDataMsg         @{(input)} vlan bulk message
* @param    vlanBulkDataMsgSize     @{(input)} size of the message
* @param    vlanBulkResponseMsg     @{(input)} vlan bulk response message
* @param    vlanBulkResponseMsgSize @{(input)} size of the response message
*
* @notes    vlanBulkResponseMsg is populated only if there were any failures.
*
* @returns  Defined by Broadcom driver
*
* @end
*********************************************************************/
L7_int32 l7_rpc_vlan_bulk_configure(vlanBulkMsgElem_t *vlanBulkDataMsg, L7_uint32 vlanBulkDataMsgSize,
                                     L7_int32 *vlanBulkResponseMsg, L7_uint32 vlanBulkResponseMsgSize,
                                     L7_uint32 numChips, L7_uint32 *chips)
{
  L7_RC_t             rc;
  L7_int32            appStatus [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32           i;
  hpcHwRpcStatus_t    rpcStatus [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t      rpcVlanBulkData;
  L7_uint32           failure = 0;
  int rv;

  rpcVlanBulkData.buf = (L7_uchar8 *) vlanBulkDataMsg;
  rpcVlanBulkData.buf_size = rpcVlanBulkData.data_len = vlanBulkDataMsgSize;

  
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(rpcVlanBulkResponse[i].buf, 0, rpcVlanBulkResponse[i].buf_size);
    rpcVlanBulkResponse[i].data_len = vlanBulkResponseMsgSize;
  }

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_BULK_HANDLER,
                        &rpcVlanBulkData,
                        rpcStatus,
                        appStatus,
                        rpcVlanBulkResponse);
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

    
    /* Check if there were any failures on any unit */
    for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
    {
      failure = *(L7_uint32 *)&(rpcVlanBulkResponse[i].buf[USL_VLAN_BULK_MSG_ELEM_OFFSET]); 
      if (failure > 0) 
      {
        break;
      }
    }

    if (failure > 0) 
    {
      L7_uint32 numElems, numFailure, offset;
      L7_BOOL   elemFail;
      L7_uchar8 *msgPtr, *retPtr;
      L7_int32   tmpRv, msgRv;

      /* If there were any failures, then walk through the response
      ** buffer from all the units and build the vlanBulkResponseMsg
      ** for the usl layer.
      */

      numElems = l7_rpc_bulk_vlan_max_msg_get();
      numFailure = 0; 
      msgPtr = (L7_uchar8 *)vlanBulkResponseMsg;
      msgPtr += USL_VLAN_BULK_MSG_DATA_OFFSET;
     
      offset = USL_VLAN_BULK_MSG_DATA_OFFSET;
     
      while (numElems > 0) 
      {
        elemFail = L7_FALSE;

        for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
        {
          retPtr = (L7_uchar8 *)rpcVlanBulkResponse[i].buf + offset;

          tmpRv = *(L7_int32 *)retPtr;
          msgRv = *(L7_int32 *)msgPtr;
          if (tmpRv < msgRv) 
          {
            elemFail = L7_TRUE;
            *(L7_int32 *)msgPtr = tmpRv;
          }
        }

        if (elemFail == L7_TRUE) 
        {
          numFailure++;
        }
        
        msgPtr += sizeof(L7_int32);
        offset += sizeof(L7_int32); 
        numElems--;
      }

      msgPtr = (L7_uchar8 *)vlanBulkResponseMsg;
      *(L7_uint32 *) &msgPtr[USL_VLAN_BULK_MSG_ELEM_OFFSET] = numFailure;
    }
  }
  else
  {
    /* RPC is not ready. This is probably a transient error, so
     * treat it as a timeout.
    */
    rv = BCM_E_TIMEOUT;
  }

  return rv;
}


/*********************************************************************
* @purpose  Handle failures in bulk configuration of vlans in the HW
*
* @param    uslVlanBulkCmd  @{(input)} Vlan bulk command that failed
* @param    uslVlanBulkDataMsg @{(input)} pointer to list off vlan-ids to be created
* @param    uslVlanBulkResponseMsg   @{(input)}  pointer to rv
* @param    vlanMaskFailure  @{(output)} mask populated by usl with 
*                                       failed vlan-ids
* @param    vlanFailureCount @{(output)} Number of vlans that could not
*                                       be created
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
void l7_rpc_vlan_bulk_process_failure(L7_BOOL  uslVlanBulkCmd,
                                      vlanBulkMsgElem_t *uslVlanBulkDataMsg, 
                                      L7_int32 *uslVlanBulkResponseMsg,
                                      L7_VLAN_MASK_t *vlanMaskFailure, 
                                      L7_uint32 *vlanFailureCount
                                     )
{
  L7_uint32   numFailedVlans = 0, vlanId;
  L7_uchar8   *msgBuffer, *rspBuffer;
  L7_int32    tmpRv;

  msgBuffer = (L7_uchar8 *)uslVlanBulkDataMsg;
  rspBuffer = (L7_uchar8 *)uslVlanBulkResponseMsg; 

  numFailedVlans = *(L7_uint32 *) &msgBuffer[USL_VLAN_BULK_MSG_ELEM_OFFSET];
  msgBuffer += USL_VLAN_BULK_MSG_DATA_OFFSET;
  rspBuffer += USL_VLAN_BULK_MSG_DATA_OFFSET;

  while (numFailedVlans > 0) 
  {
    /* Get the return code */
    tmpRv = *(L7_int32 *)rspBuffer;

    /* If vlan creation failed */
    if (tmpRv != BCM_E_NONE) 
    {
      vlanId = *(L7_uint32 *)msgBuffer;

      /* Populate the application vlan failure mask */
      L7_VLAN_SETMASKBIT(*vlanMaskFailure, vlanId);
      (*vlanFailureCount)++;
    }
    
    msgBuffer += sizeof(L7_uint32);
    rspBuffer += sizeof(L7_int32);
    numFailedVlans--;
  }

  return;
}


/*********************************************************************
* @purpose  Create/Delete a list of vlans in the HW
*
* @param    bulkCmd          @{(input)} L7_TRUE: Create the Vlans
*                                       L7_FALSE: Delete the Vlans
* @param    vlanMask         @{(input)} Pointer to mask of vlan-ids to be created
* @param    numVlans         @{(input)} number of vlans to be created
* @param    vlanMaskFailure  @{(output)} Pointer to mask populated by usl with 
*                                       failed vlan-ids
* @param    vlanFailureCount @{(output)} Pointer to integer to pass the number 
*                                        of vlans that could not be created
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_vlan_bulk_configure(L7_BOOL bulkCmd,
                                      L7_VLAN_MASK_t *vlanMask, 
                                      L7_ushort16 numVlans,
                                      L7_VLAN_MASK_t *vlanMaskFailure,
                                      L7_uint32 *vlanFailureCount)
{
  int                       rv = BCM_E_NONE;
  L7_uchar8                 tmpVlanMask, *msg;
  L7_ushort16               vidx = 0, pos = 0;
  L7_uint32                 maxNumElemsPerMsg, numFailedVlans;
  L7_uint32                 numElems, vlanId;
  L7_BOOL                   uslVlanBulkCmd;


  do
  {
    /* Maximum number of elements that can be sent in a message.*/
    maxNumElemsPerMsg  = l7_rpc_bulk_vlan_max_msg_get();

    memset(uslVlanBulkDataMsg, 0, uslVlanBulkDataMsgSize);
    memset(uslVlanBulkResponseMsg, 0, uslVlanBulkResponseMsgSize);
    numElems = 0;

    if (bulkCmd == L7_TRUE) 
    {
      /* Create vlan */
      uslVlanBulkCmd = L7_TRUE;
    }
    else
    {
      /* Delete vlan */
      uslVlanBulkCmd = L7_FALSE;
    }

    msg = (L7_uchar8 *)uslVlanBulkDataMsg;
    *(L7_uint32 *) &msg[USL_VLAN_BULK_MSG_TYPE_OFFSET] = uslVlanBulkCmd;
    msg += USL_VLAN_BULK_MSG_DATA_OFFSET;
    

    for (vidx = 0; vidx < L7_VLAN_INDICES; vidx++) 
    {
      pos = 0;
	  tmpVlanMask = vlanMask->value[vidx];
	  while (tmpVlanMask) 
	  {
	    pos++;
	    if (tmpVlanMask & 0x1) 
	    {
		  vlanId = (vidx*8) + pos;

          /* Copy the vlan-id in the msg */
          *(L7_uint32 *) msg = vlanId;
          msg += sizeof(L7_uint32);
          numElems++;

          /* Time to send the message if the buffer is full */
          if (numElems == maxNumElemsPerMsg) 
          {
            msg = (L7_uchar8 *)uslVlanBulkDataMsg;
            *(L7_uint32 *) &msg[USL_VLAN_BULK_MSG_ELEM_OFFSET] = numElems;
            rv = l7_rpc_vlan_bulk_configure(uslVlanBulkDataMsg, uslVlanBulkDataMsgSize, 
                                            uslVlanBulkResponseMsg, uslVlanBulkResponseMsgSize,
                                            0, L7_NULL);
            if (rv != BCM_E_NONE) 
            {
              USL_LOG_MSG(USL_BCM_E_LOG,"USL: error performing vlan bulk cmd %d in HW rv=%d\n",
                          uslVlanBulkCmd, rv);
            }

            /* Handle if there were any failures */
            msg = (L7_uchar8 *)uslVlanBulkResponseMsg;
            numFailedVlans =  *(L7_uint32 *) &msg[USL_VLAN_BULK_MSG_ELEM_OFFSET];
            if (numFailedVlans > 0) 
            {
              l7_rpc_vlan_bulk_process_failure(uslVlanBulkCmd, uslVlanBulkDataMsg, 
                                               uslVlanBulkResponseMsg, vlanMaskFailure, 
                                               vlanFailureCount);
            }

            memset(uslVlanBulkDataMsg, 0, uslVlanBulkDataMsgSize);
            memset(uslVlanBulkResponseMsg, 0, uslVlanBulkResponseMsgSize);
            numElems = 0;
            msg = (L7_uchar8 *)uslVlanBulkDataMsg;
            *(L7_uint32 *) &msg[USL_VLAN_BULK_MSG_TYPE_OFFSET] = uslVlanBulkCmd;
            msg += USL_VLAN_BULK_MSG_DATA_OFFSET;
          }

	      numVlans--;
	    }
	    tmpVlanMask >>= 1;
	  }

	  if (numVlans == 0) 
	  {
        /* Check if another message needs to be send */
        if (numElems > 0) 
        {
          msg = (L7_uchar8 *)uslVlanBulkDataMsg;
          *(L7_uint32 *) &msg[USL_VLAN_BULK_MSG_ELEM_OFFSET] = numElems;
          rv = l7_rpc_vlan_bulk_configure(uslVlanBulkDataMsg, uslVlanBulkDataMsgSize, 
                                          uslVlanBulkResponseMsg, uslVlanBulkResponseMsgSize,
                                          0, L7_NULL);
          if (rv != BCM_E_NONE) 
          {
            USL_LOG_MSG(USL_BCM_E_LOG,"USL: error performing vlan bulk cmd %d in HW rv=%d\n",
                        uslVlanBulkCmd, rv);
          }

          /* Handle if there were any failures */
          msg = (L7_uchar8 *)uslVlanBulkResponseMsg;
          numFailedVlans =  *(L7_uint32 *) &msg[USL_VLAN_BULK_MSG_ELEM_OFFSET];
          if (numFailedVlans > 0) 
          {
            l7_rpc_vlan_bulk_process_failure(uslVlanBulkCmd, uslVlanBulkDataMsg, 
                                             uslVlanBulkResponseMsg, vlanMaskFailure, 
                                             vlanFailureCount);
          }
        }

	    break;
	  }
    }
  } while ( 0 );


  return rv;
}

/*********************************************************************
* @purpose  Makes the rpc call for a trunk learn mode change.
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
*
*
* @returns  none
*
* @end
*********************************************************************/

L7_RC_t l7_rpc_client_trunk_learn_mode_set(L7_uint32 appId, 
                                           bcm_trunk_t tgid, L7_BOOL learningLocked)
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (appId) + sizeof (tgid) + 
                                                sizeof(learningLocked);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, appId);  
  L7_UINT32_PACK(msgBuf, tgid);
  L7_UINT32_PACK(msgBuf, learningLocked);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_TRUNK_LEARN_MODE_HANDLER,
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
* @purpose  Custom RPC function to handle Trunk Learn mode changes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/

L7_RC_t l7_rpc_server_trunk_learn_mode_set(L7_uint32 transaction_id,
                                           hpcHwRpcData_t *rpc_data,
                                           L7_int32 *status,
                                           hpcHwRpcData_t  *rpc_resp)
{
  int         rv = BCM_E_NONE;
  L7_uchar8  *pBuf;
  L7_uint32   appId;
  bcm_trunk_t tid;
  L7_BOOL     learnMode;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  pBuf = rpc_data->buf;

  L7_UINT32_UNPACK(pBuf, appId);
  L7_UINT32_UNPACK(pBuf, tid);
  L7_UINT32_UNPACK(pBuf, learnMode);

  rv = usl_bcm_trunk_learn_mode_set(appId, tid, learnMode);

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets hashing mechanism for existing trunk.
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    psc          @{(input)} The hashing mechanism.
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_trunk_psc_set (L7_uint32 appId, bcm_trunk_t tid,
                                 int psc,
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (appId) + sizeof (tid) + sizeof(psc);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_RPC_CMD_TRUNK_PSC_SET);
  L7_UINT32_PACK(msgBuf, appId);  
  L7_UINT32_PACK(msgBuf, tid);
  L7_UINT32_PACK(msgBuf, psc);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_TRUNK_HANDLER,
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
* @purpose  Sets hashing mechanism for existing trunk
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
L7_RC_t l7_rpc_server_trunk_psc_set (L7_uint32 transaction_id,
                                     hpcHwRpcData_t *rpc_data,
                                     L7_int32 *status,
                                     hpcHwRpcData_t  *rpc_resp)
{
  int         rv = BCM_E_NONE;
  L7_uchar8  *pBuf;
  L7_uint32   cmd, appId;
  bcm_trunk_t tid;
  int         psc;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  pBuf = rpc_data->buf;

  L7_UINT32_UNPACK(pBuf, cmd);
  L7_UINT32_UNPACK(pBuf, appId);
  L7_UINT32_UNPACK(pBuf, tid);
  L7_UINT32_UNPACK(pBuf, psc);

  rv = usl_bcm_trunk_psc_set(appId, tid, psc);

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Create a TRUNK in the driver
*
* @param    appId        @{(input)}  Application-identifier for trunk
* @param    tid          @{(output)} The Trunk ID
* @param    flags        @{(input)}  Trunk specific flags
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_trunk_create(L7_uint32 appId, bcm_trunk_t * tid,
                               L7_uint32 flags, L7_uint32 numChips, 
                               L7_uint32 *chips)
{
  L7_RC_t             rc;
  L7_int32            appStatus [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32           i, msgSize;
  L7_uchar8          *msgBuf, *rpcBuf;
  int                 trunkId = BCM_TRUNK_INVALID, lastTrunkId = BCM_TRUNK_INVALID;
  int                 rv;
  hpcHwRpcStatus_t    rpcStatus [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t      rpcData;
  hpcHwRpcData_t      *appRspBuf;


  rpcBuf = l7_rpc_req_buffer_get();
  if (rpcBuf == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to get RPC request buffer\n");

    rv = BCM_E_FAIL;
    return rv;
  }
  

  appRspBuf = l7_rpc_resp_buffer_get();
  if (appRspBuf == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to get RPC response buffer\n");

    l7_rpc_req_buffer_free(rpcBuf);
    rv = BCM_E_FAIL;
    return rv;
  }
  

  msgBuf = rpcBuf;

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (appId) + sizeof(bcm_trunk_t) + sizeof(flags);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_RPC_CMD_TRUNK_CREATE);
  L7_UINT32_PACK(msgBuf, appId);
  L7_UINT32_PACK(msgBuf, *tid);
  L7_UINT32_PACK(msgBuf, flags);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_TRUNK_HANDLER,
                        &rpcData,
                        rpcStatus,
                        appStatus, 
                        appRspBuf);

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
        if (appStatus[i] == BCM_E_NONE)
        {
          /* Check the response length */
          if (appRspBuf[i].data_len == sizeof(bcm_trunk_t))
          {
            trunkId = *(bcm_trunk_t *)appRspBuf[i].buf;

            /* Ensure that all the units generate same trunk-id */
            if ((lastTrunkId != BCM_TRUNK_INVALID) && 
                (trunkId != lastTrunkId))
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                      "Different trunk-id generated by unit %d for Trunk appId %d\n",
                      i, appId);
              L7_LOG_ERROR(appId);
            }
            else
            {
              lastTrunkId = trunkId;
            }
          }
          else 
            rv = BCM_E_INTERNAL;
        }

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

  *tid = trunkId;

  l7_rpc_resp_buffer_free(appRspBuf);
  l7_rpc_req_buffer_free(rpcBuf);

  return rv; 


}

/*********************************************************************
* @purpose  Create a trunk
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
L7_RC_t l7_rpc_server_trunk_create (L7_uint32 transaction_id,
                                    hpcHwRpcData_t *rpc_data,
                                    L7_int32 *status,
                                    hpcHwRpcData_t  *rpc_resp)
{
  int         rv = BCM_E_NONE;
  L7_uchar8  *msgBuf, *respBuf;
  L7_uint32   cmd, appId, flags = 0;
  bcm_trunk_t tid = BCM_TRUNK_INVALID;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  /* Expecting a response buffer to pass tid back */
  if (rpc_resp == L7_NULLPTR)
  {
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  msgBuf = rpc_data->buf;
  respBuf = rpc_resp->buf;
 
  rpc_resp->data_len = 0;

  L7_UINT32_UNPACK(msgBuf, cmd);
  L7_UINT32_UNPACK(msgBuf, appId);
  L7_UINT32_UNPACK(msgBuf, tid);
  L7_UINT32_UNPACK(msgBuf, flags);


  rv = usl_bcm_trunk_create(appId, flags, &tid);

  /* Pack the tid in response buffer */
  L7_UINT32_PACK(respBuf, tid);
  rpc_resp->data_len = sizeof(bcm_trunk_t);

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Destroy a TRUNK in the driver
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_trunk_destroy(L7_uint32 appId, bcm_trunk_t tid,
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (appId) + sizeof (tid);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_RPC_CMD_TRUNK_DESTROY);
  L7_UINT32_PACK(msgBuf, appId);
  L7_UINT32_PACK(msgBuf, tid);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_TRUNK_HANDLER,
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
* @purpose  Delete a trunk
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
L7_RC_t l7_rpc_server_trunk_destroy (L7_uint32 transaction_id,
                                     hpcHwRpcData_t *rpc_data,
                                     L7_int32 *status,
                                     hpcHwRpcData_t  *rpc_resp)
{
  int         rv = BCM_E_NONE;
  L7_uchar8  *msgBuf;
  L7_uint32   cmd, appId;
  bcm_trunk_t tid;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  msgBuf = rpc_data->buf;

  L7_UINT32_UNPACK(msgBuf, cmd);
  L7_UINT32_UNPACK(msgBuf, appId);
  L7_UINT32_UNPACK(msgBuf, tid);


  rv = usl_bcm_trunk_destroy(appId, tid);

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Set the characteristics of a TRUNK in the HW's trunk table
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    add_info     @{(input)} The ports, algorithm, and flags for the trunk
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_trunk_set(L7_uint32 appId, bcm_trunk_t tid, 
                            bcm_trunk_add_info_t * add_info,
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (appId) + 
                                    sizeof (tid) + sizeof(*add_info);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_RPC_CMD_TRUNK_INFO_SET);
  L7_UINT32_PACK(msgBuf, appId);
  L7_UINT32_PACK(msgBuf, tid);
  memcpy(msgBuf, add_info, sizeof(*add_info));
  msgBuf += sizeof(*add_info);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_TRUNK_HANDLER,
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
* @purpose Set trunk's info 
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
L7_RC_t l7_rpc_server_trunk_info_set (L7_uint32 transaction_id,
                                      hpcHwRpcData_t *rpc_data,
                                      L7_int32 *status,
                                      hpcHwRpcData_t  *rpc_resp)
{
  int                  rv = BCM_E_NONE;
  L7_uchar8           *msgBuf;
  L7_uint32            cmd, appId;
  bcm_trunk_t          tid;
  bcm_trunk_add_info_t addInfo;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  msgBuf = rpc_data->buf;

  L7_UINT32_UNPACK(msgBuf, cmd);
  L7_UINT32_UNPACK(msgBuf, appId);
  L7_UINT32_UNPACK(msgBuf, tid);
  memcpy(&addInfo, msgBuf, sizeof(addInfo));
  msgBuf += sizeof(addInfo);

  rv = usl_bcm_trunk_set(appId, tid, &addInfo);
  
  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose Handle trunk RPC commands
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
L7_RC_t  l7_rpc_server_trunk_handler (L7_uint32 transaction_id,
                                      hpcHwRpcData_t *rpc_data,
                                      L7_int32 *status,
                                      hpcHwRpcData_t  *rpc_resp)
{
  L7_uchar8           *msgBuf;
  L7_uint32            cmd;
  L7_RC_t              rc = L7_SUCCESS;
  

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  msgBuf = rpc_data->buf;

  /* Get the RPC trunk command */
  L7_UINT32_UNPACK(msgBuf, cmd);

  switch (cmd)
  {
    case L7_RPC_CMD_TRUNK_INFO_SET:
      return l7_rpc_server_trunk_info_set(transaction_id, rpc_data, status, rpc_resp);
      break;

    case L7_RPC_CMD_TRUNK_CREATE:
      return l7_rpc_server_trunk_create(transaction_id, rpc_data, status, rpc_resp);
      break;

    case L7_RPC_CMD_TRUNK_DESTROY:
      return l7_rpc_server_trunk_destroy(transaction_id, rpc_data, status, rpc_resp);
      break;

    case L7_RPC_CMD_TRUNK_PSC_SET:
      return l7_rpc_server_trunk_psc_set(transaction_id, rpc_data, status, rpc_resp);
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*********************************************************************
* @purpose Initialize Trunk RPC 
*
* @params  none
*
* @returns none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_trunk_rpc_init(void)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = hpcHardwareRpcRegister(L7_RPC_TRUNK_HANDLER, l7_rpc_server_trunk_handler);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }


  rc = hpcHardwareRpcRegister(L7_RPC_TRUNK_LEARN_MODE_HANDLER, l7_rpc_server_trunk_learn_mode_set);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  return rc;
}

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    appId   @{(input)} Unique application identifier for 
*                              this stg
* @param    stg      @{(input/output)} Pointer the Spanning Tree Group
* @param    stgFlag  @{(input)} USL Stg flag    
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_stg_create(L7_uint32 appId, bcm_stg_t *pStg,
                             L7_uint32 stgFlags,
                             L7_uint32 numChips, L7_uint32 *chips)
{

  L7_RC_t             rc;
  L7_int32            appStatus [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32           i, msgSize;
  L7_uchar8          *msgBuf, *rpcBuf;
  bcm_stg_t           stgId = BCM_STG_INVALID, lastStgId = BCM_STG_INVALID;
  int                 rv;
  hpcHwRpcStatus_t    rpcStatus [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t      rpcData;
  hpcHwRpcData_t      *appRspBuf;


  rpcBuf = l7_rpc_req_buffer_get();
  if (rpcBuf == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to get RPC request buffer\n");

    rv = BCM_E_FAIL;
    return rv;
  }
  

  appRspBuf = l7_rpc_resp_buffer_get();
  if (appRspBuf == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to get RPC response buffer\n");

    l7_rpc_req_buffer_free(rpcBuf);
    rv = BCM_E_FAIL;
    return rv;
  }
  

  msgBuf = rpcBuf;

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (appId) + sizeof(bcm_stg_t) + sizeof(stgFlags);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_RPC_CMD_STG_CREATE);
  L7_UINT32_PACK(msgBuf, appId);
  L7_UINT32_PACK(msgBuf, *pStg);
  L7_UINT32_PACK(msgBuf, stgFlags);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_STG_HANDLER,
                        &rpcData,
                        rpcStatus,
                        appStatus, 
                        appRspBuf);

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
        if (appStatus[i] == BCM_E_NONE)
        {
          /* Check the response length */
          if (appRspBuf[i].data_len == sizeof(bcm_stg_t))
          {
            stgId = *(bcm_stg_t *)appRspBuf[i].buf;

            /* Ensure that all the units generate same trunk-id */
            if ((lastStgId != BCM_STG_INVALID) && 
                (stgId != lastStgId))
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                      "Different stgId generated by unit %d for appId %d\n",
                      i, appId);
              L7_LOG_ERROR(appId);
            }
            else
            {
              lastStgId = stgId;
            }
          }
          else 
            rv = BCM_E_INTERNAL;
        }

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

  *pStg = stgId;

  l7_rpc_resp_buffer_free(appRspBuf);
  l7_rpc_req_buffer_free(rpcBuf);

  return rv; 

}

/*********************************************************************
* @purpose  Create a Spanning-tree group
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
L7_RC_t l7_rpc_server_stg_create (L7_uint32 transaction_id,
                                  hpcHwRpcData_t *rpc_data,
                                  L7_int32 *status,
                                  hpcHwRpcData_t  *rpc_resp)
{
  int         rv = BCM_E_NONE;
  L7_uchar8  *msgBuf, *respBuf;
  L7_uint32   rpcCmd;
  L7_uint32   appId;
  bcm_stg_t   stgId = BCM_STG_INVALID;
  L7_uint32   flags;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  /* Expecting a response buffer to pass stgId back */
  if (rpc_resp == L7_NULLPTR)
  {
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  msgBuf = rpc_data->buf;
  respBuf = rpc_resp->buf;
 
  rpc_resp->data_len = 0;

  L7_UINT32_UNPACK(msgBuf, rpcCmd);
  L7_UINT32_UNPACK(msgBuf, appId);
  L7_UINT32_UNPACK(msgBuf, stgId);
  L7_UINT32_UNPACK(msgBuf, flags);

  rv = usl_bcm_stg_create(appId, flags, &stgId);

  /* Pack the stgId in response buffer */
  L7_UINT32_PACK(respBuf, stgId);
  rpc_resp->data_len = sizeof(bcm_stg_t);
  
  *status = rv;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Destroy a Spanning-tree Group
*
* @param    appInstId @{(input)} Application instId
* @param    stgId    @{(input)} Hardware-id of spanning-tree group
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_stg_destroy(L7_uint32 appInstId, bcm_stg_t stgId,
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (stgId) + sizeof (appInstId);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_RPC_CMD_STG_DESTROY);
  L7_UINT32_PACK(msgBuf, appInstId);
  L7_UINT32_PACK(msgBuf, stgId);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_STG_HANDLER,
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
* @purpose  Delete a Spanning-tree group
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
L7_RC_t l7_rpc_server_stg_destroy (L7_uint32 transaction_id,
                                   hpcHwRpcData_t *rpc_data,
                                   L7_int32 *status,
                                   hpcHwRpcData_t  *rpc_resp)
{
  int         rv = BCM_E_NONE;
  L7_uchar8  *msgBuf;
  L7_uint32   rpcCmd, appInstId;
  bcm_stg_t   stgId;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  msgBuf = rpc_data->buf;

  L7_UINT32_UNPACK(msgBuf, rpcCmd);
  L7_UINT32_UNPACK(msgBuf, appInstId);
  L7_UINT32_UNPACK(msgBuf, stgId);


  rv = usl_bcm_stg_destroy(appInstId, stgId);
  
  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Add/Remove a VLAN to/from a Spanning-tree Group
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
* @param    updateCmd    @{(input)} Add/Remove Vid to Stg
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_stg_vlan_update(L7_uint32 appInstId, bcm_stg_t stg, 
                                  bcm_vlan_t vid, USL_CMD_t updateCmd,
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

  msgSize = L7_RPC_REQ_BUFF_HDR_SIZE + sizeof (appInstId) + sizeof (stg) + 
                                           sizeof(vid) + sizeof(updateCmd);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    L7_LOG_ERROR(msgSize); 
  }


  rpcData.buf = (L7_uchar8 *) rpcBuf;
  rpcData.buf_size = rpcData.data_len = msgSize ;

  L7_UINT32_PACK(msgBuf, L7_RPC_CMD_STG_VLAN_UPDATE);
  L7_UINT32_PACK(msgBuf, appInstId);
  L7_UINT32_PACK(msgBuf, stg);
  L7_UINT16_PACK(msgBuf, vid);
  L7_UINT32_PACK(msgBuf, updateCmd);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_STG_HANDLER,
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
* @purpose  Add/Remove a VLAN to/from a Spanning-tree Group
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
L7_RC_t l7_rpc_server_stg_vlan_update (L7_uint32 transaction_id,
                                       hpcHwRpcData_t *rpc_data,
                                       L7_int32 *status,
                                       hpcHwRpcData_t  *rpc_resp)
{
  int              rv = BCM_E_NONE;
  L7_uchar8       *msgBuf;
  L7_uint32        rpcCmd, appInstId;
  bcm_stg_t        stgId;
  bcm_vlan_t       vid;
  USL_CMD_t updateCmd;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  msgBuf = rpc_data->buf;

  L7_UINT32_UNPACK(msgBuf, rpcCmd);
  L7_UINT32_UNPACK(msgBuf, appInstId);
  L7_UINT32_UNPACK(msgBuf, stgId);
  L7_UINT16_UNPACK(msgBuf, vid);
  L7_UINT32_UNPACK(msgBuf, updateCmd);


  rv = usl_bcm_stg_vlan_update(appInstId, stgId, vid, updateCmd);
  
  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose Handle Spanning-tree RPC commands
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
L7_RC_t  l7_rpc_server_stg_handler (L7_uint32 transaction_id,
                                    hpcHwRpcData_t *rpc_data,
                                    L7_int32 *status,
                                    hpcHwRpcData_t  *rpc_resp)
{
  L7_uchar8           *msgBuf;
  L7_uint32            rpcCmd;
  L7_RC_t              rc = L7_SUCCESS;
  

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  msgBuf = rpc_data->buf;

  /* Get the RPC trunk command */
  L7_UINT32_UNPACK(msgBuf, rpcCmd);

  switch (rpcCmd)
  {
    case L7_RPC_CMD_STG_CREATE:
      return l7_rpc_server_stg_create(transaction_id, rpc_data, status, rpc_resp);
      break;

    case L7_RPC_CMD_STG_DESTROY:
      return l7_rpc_server_stg_destroy(transaction_id, rpc_data, status, rpc_resp);
      break;

    case L7_RPC_CMD_STG_VLAN_UPDATE:
      return l7_rpc_server_stg_vlan_update(transaction_id, rpc_data, status, rpc_resp);
      break;

    default:
      rc = L7_NOT_SUPPORTED;
      break;
  }

  return rc;
}

/*********************************************************************
* @purpose Initialize Spanning-tree RPC 
*
* @params  none
*
* @returns none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_stg_rpc_init(void)
{
  L7_RC_t rc;

  rc = hpcHardwareRpcRegister(L7_RPC_STG_HANDLER, l7_rpc_server_stg_handler);

  return rc;
}

/*********************************************************************
* @purpose  Adds the Ip Subnet Vlan entry to the  HW table
*
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} net mask
* @param   vid     @b{(input)} vlan Id
* @param   prio    @b{(input)} priority field
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int l7_rpc_client_vlan_ip4_add(bcm_ip_t ipaddr, bcm_ip_t netmask, bcm_vlan_t vid, int prio)
{
  L7_RC_t                     rc;
  L7_int32                    appStatus [L7_MAX_UNITS_PER_STACK + 1];
  int                         rv;
  hpcHwRpcStatus_t            rpcStatus [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t              rpcData;
  usl_bcm_vlan_ipsubnet_t     ipSubnetData;
  L7_uint32                   i;

  memset(&ipSubnetData, 0, sizeof(ipSubnetData));

  ipSubnetData.ipSubnet = ipaddr;
  ipSubnetData.netMask  =  netmask;
  ipSubnetData.vlanId   = vid;
  ipSubnetData.prio     = prio;

  rpcData.buf = (L7_uchar8 *) &ipSubnetData;
  rpcData.buf_size = rpcData.data_len = sizeof(ipSubnetData) ;

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_IP_SUBNET_ADD_HANDLER,
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


  return rv; 

}

/*********************************************************************
* @purpose  Adds the Ip Subnet Vlan entry to the  HW table
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
L7_RC_t l7_rpc_server_vlan_ip4_add (L7_uint32 transaction_id,
                                    hpcHwRpcData_t *rpc_data,
                                    L7_int32 *status,
                                    hpcHwRpcData_t  *rpc_resp)
{
  int                         rv = BCM_E_NONE;
  usl_bcm_vlan_ipsubnet_t    *ipSubnetData;  

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  ipSubnetData = (usl_bcm_vlan_ipsubnet_t *) rpc_data->buf;

  rv = usl_bcm_vlan_ip4_add(ipSubnetData);
  
  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Deletes the Ip Subnet Vlan entry to the  HW table
*
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} net mask
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int l7_rpc_client_vlan_ip4_delete(bcm_ip_t ipaddr, bcm_ip_t netmask)
{
  L7_RC_t                     rc;
  L7_int32                    appStatus [L7_MAX_UNITS_PER_STACK + 1];
  int                         rv;
  hpcHwRpcStatus_t            rpcStatus [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t              rpcData;
  usl_bcm_vlan_ipsubnet_t     ipSubnetData;
  L7_uint32                   i;

  memset(&ipSubnetData, 0, sizeof(ipSubnetData));

  ipSubnetData.ipSubnet = ipaddr;
  ipSubnetData.netMask  =  netmask;
  
  rpcData.buf = (L7_uchar8 *) &ipSubnetData;
  rpcData.buf_size = rpcData.data_len = sizeof(ipSubnetData) ;

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_IP_SUBNET_DELETE_HANDLER,
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


  return rv; 

}

/*********************************************************************
* @purpose  Deletes the Ip Subnet Vlan entry from the  HW table
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
L7_RC_t l7_rpc_server_vlan_ip4_delete (L7_uint32 transaction_id,
                                       hpcHwRpcData_t *rpc_data,
                                       L7_int32 *status,
                                       hpcHwRpcData_t  *rpc_resp)
{
  int                          rv = BCM_E_NONE;
  usl_bcm_vlan_ipsubnet_t     *ipSubnetData;  

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  ipSubnetData = (usl_bcm_vlan_ipsubnet_t *) rpc_data->buf;

  rv = usl_bcm_vlan_ip4_delete(ipSubnetData);

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose Initialize IP Subnet Vlan RPC 
*
* @params  none
*
* @returns none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_vlan_ipsubnet_rpc_init(void)
{
  L7_RC_t rc;

  rc = hpcHardwareRpcRegister(L7_RPC_VLAN_IP_SUBNET_ADD_HANDLER,
                              l7_rpc_server_vlan_ip4_add);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  rc = hpcHardwareRpcRegister(L7_RPC_VLAN_IP_SUBNET_DELETE_HANDLER, 
                              l7_rpc_server_vlan_ip4_delete);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  return rc;
}

/*********************************************************************
* @purpose  Adds the entry to the HW 
*
* @param   mac     @b{(input)} Mac Address
* @param   vid     @b{(input)} vlan Id 
* @param   prio    @b{(input)} priority field 
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int l7_rpc_client_vlan_mac_add(bcm_mac_t mac, bcm_vlan_t vid, int prio)
{
  L7_RC_t                     rc;
  L7_int32                    appStatus [L7_MAX_UNITS_PER_STACK + 1];
  int                         rv;
  hpcHwRpcStatus_t            rpcStatus [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t              rpcData;
  usl_bcm_vlan_mac_t     macData;
  L7_uint32                   i;

  memset(&macData, 0, sizeof(macData));

  memcpy(&(macData.mac), mac, sizeof(bcm_mac_t));
  macData.vlanId = vid;
  macData.prio = prio;
  
  rpcData.buf = (L7_uchar8 *) &macData;
  rpcData.buf_size = rpcData.data_len = sizeof(macData) ;

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_MAC_ADD_HANDLER,
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


  return rv; 

}

/*********************************************************************
* @purpose  Adds the Vlan mac entry to the HW 

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
L7_RC_t l7_rpc_server_vlan_mac_add (L7_uint32 transaction_id,
                                    hpcHwRpcData_t *rpc_data,
                                    L7_int32 *status,
                                    hpcHwRpcData_t  *rpc_resp)
{
  int                         rv = BCM_E_NONE;
  usl_bcm_vlan_mac_t     *macData;  

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  macData = (usl_bcm_vlan_mac_t *) rpc_data->buf;

  rv = usl_bcm_vlan_mac_add(macData);

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Delete the entry from HW
*
* @param   mac     @b{(input)} Mac Address
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int l7_rpc_client_vlan_mac_delete(bcm_mac_t mac)
{
  L7_RC_t                     rc;
  L7_int32                    appStatus [L7_MAX_UNITS_PER_STACK + 1];
  int                         rv;
  hpcHwRpcStatus_t            rpcStatus [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t              rpcData;
  usl_bcm_vlan_mac_t      macData;
  L7_uint32                   i;

  memset(&macData, 0, sizeof(macData));

  memcpy(&(macData.mac), mac, sizeof(bcm_mac_t));
  
  rpcData.buf = (L7_uchar8 *) &macData;
  rpcData.buf_size = rpcData.data_len = sizeof(macData) ;

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_VLAN_MAC_DELETE_HANDLER,
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


  return rv; 

}

/*********************************************************************
* @purpose  Delete the entry from HW
*
* @param    transaction_id - transaction id
* @param    buf - buffer pointer to egress object information
* @param    buf_len - buffer length
* @param    status - return code 
*
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
L7_RC_t l7_rpc_server_vlan_mac_delete (L7_uint32 transaction_id,
                                       hpcHwRpcData_t *rpc_data,
                                       L7_int32 *status,
                                       hpcHwRpcData_t  *rpc_resp)
{
  int                  rv = BCM_E_NONE;
  usl_bcm_vlan_mac_t  *macData;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  macData = (usl_bcm_vlan_mac_t *) rpc_data->buf;

  rv = usl_bcm_vlan_mac_delete(macData);

  /* Indicate that all egress objects were processed and response is valid */
  *status = rv;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose Initialize Mac Vlan RPC 
*
* @params  none
*
* @returns none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_vlan_mac_rpc_init(void)
{
  L7_RC_t rc;

  rc = hpcHardwareRpcRegister(L7_RPC_VLAN_MAC_ADD_HANDLER, l7_rpc_server_vlan_mac_add);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  rc = hpcHardwareRpcRegister(L7_RPC_VLAN_MAC_DELETE_HANDLER, l7_rpc_server_vlan_mac_delete);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  return rc;
}

/* only used in this file by the set and the handler */
typedef struct {
  L7_uint32 mode; 
  L7_uchar8 mac[6];
} BROAD_FLOW_CONTROL_RPC_t;

typedef struct {
  L7_DOSCONTROL_TYPE type;
  L7_uint32          mode; 
  L7_uint32          arg; 
} BROAD_DOS_CONTROL_RPC_t;

/*********************************************************************
* @purpose  Makes the rpc call for Flow Control
*
* @param    unit    @{{input}}  L7_ALL_UNITS - to enable on all units
*                               or unit number of the specific unit
* @param    mode - 1 for enable, 0 for disable
*
* @returns  
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_flow_control_set(L7_uint32 unit, L7_uint32 mode,L7_uchar8 *mac)
{
    int rc;
    hpcHwRpcStatus_t rpc_status[L7_MAX_UNITS_PER_STACK + 1];
    int              app_status[L7_MAX_UNITS_PER_STACK + 1];
    L7_uint32        i;
    hpcHwRpcData_t   rpc_data;
    BROAD_FLOW_CONTROL_RPC_t data;

    data.mode = mode;
    memcpy(data.mac,mac,sizeof(data.mac));

    rpc_data.buf = (L7_uchar8*)&data;
    rpc_data.buf_size = rpc_data.data_len = sizeof(data);

    rc = hpcHardwareRpc(unit,
                        L7_RPC_FLOW_CONTROL_HANDLER,
                        &rpc_data,
                        rpc_status,
                        app_status, L7_NULL);

    if (L7_SUCCESS == rc)
    {
      for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
      {
        if (HPC_HW_RPC_OK == rpc_status[i])
        {
          if (app_status[i] < BCM_E_NONE) 
          {
            return app_status[i];
          }
        }
      }
    }
    else
    {
      return BCM_E_TIMEOUT;
    }

    return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Makes the rpc call for DOS Control
*
* @param    type    @{{input}}  type of DOS control
* @param    mode    @{{input}}  enable=1,disable=0
* @param    arg     @{{input}}  argument for certain types of DOS control
*
* @returns  
*
* @end
*********************************************************************/
int l7_rpc_client_doscontrol_set(L7_DOSCONTROL_TYPE type, L7_uint32 mode, L7_uint32 arg)
{
    L7_RC_t                 rc;
    int                     rv = BCM_E_NONE;
    hpcHwRpcStatus_t        rpc_status[L7_MAX_UNITS_PER_STACK + 1];
    L7_int32                app_status[L7_MAX_UNITS_PER_STACK + 1];
    hpcHwRpcData_t          rpc_data;
    BROAD_DOS_CONTROL_RPC_t data;

    data.type = type;
    data.mode = mode;
    data.arg  = arg;

    rpc_data.buf = (L7_uchar8*)&data;
    rpc_data.buf_size = rpc_data.data_len = sizeof(data);

    rc = hpcHardwareRpc(L7_ALL_UNITS,
                        L7_RPC_DOS_CONTROL_HANDLER,
                        &rpc_data,
                        rpc_status,
                        app_status, L7_NULL);

    if (L7_SUCCESS == rc)
    {
      int i;

      for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
      {
        if (HPC_HW_RPC_OK == rpc_status[i])
        {
          if (app_status[i] < rv)
            rv = app_status[i];
        }
      }
    }
    else
    {
      rv = BCM_E_TIMEOUT;
    }

    return rv;
}

/*********************************************************************
* @purpose  Custom RPC function to handle Flow Control
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_flow_control_set(L7_uint32 tid,
                                       hpcHwRpcData_t *rpc_data,
                                       L7_int32 *status,
                                       hpcHwRpcData_t  *rpc_resp)
{
    int rv = BCM_E_NONE;
    BROAD_FLOW_CONTROL_RPC_t *data;
    
    if (rpc_data->data_len != sizeof(BROAD_FLOW_CONTROL_RPC_t ))
    {
        *status = BCM_E_FAIL;
        return L7_SUCCESS;
    }
    
    data = (BROAD_FLOW_CONTROL_RPC_t *) rpc_data->buf;

    rv = usl_bcm_flow_control_set(data->mode, data->mac);

    *status = rv;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Custom RPC function to handle DOS Control
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_doscontrol_set(L7_uint32       tid,
                                     hpcHwRpcData_t *rpc_data,
                                     L7_int32       *status,
                                     hpcHwRpcData_t *rpc_resp)
{
    int rv = BCM_E_NONE;
    BROAD_DOS_CONTROL_RPC_t *data;
    
    if (rpc_data->data_len != sizeof(BROAD_DOS_CONTROL_RPC_t ))
    {
        *status = BCM_E_FAIL;
        return L7_SUCCESS;
    }
    
    data = (BROAD_DOS_CONTROL_RPC_t *) rpc_data->buf;

    rv = usl_bcm_doscontrol_set(data->type, data->mode, data->arg);

    *status = rv;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable IPMC in the chip
*
* @param    enabled     @{(input)} enable(1) or disable(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_enable(int enabled,
                              L7_uint32 numChips, L7_uint32 *chips)
{
  int                 rv = BCM_E_NONE;
  L7_RC_t             rc;
  hpcHwRpcStatus_t    rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32            app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32           i;
  hpcHwRpcData_t      rpc_data;


  rpc_data.buf = (L7_uchar8 *) &enabled;
  rpc_data.buf_size = rpc_data.data_len = sizeof (enabled);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_IPMCAST_ENABLE_SET,
                        &rpc_data,
                        rpc_status,
                        app_status, L7_NULL);
  if (rc == L7_SUCCESS)
  {
    /* Loop through units that successfully completed RPC and
    ** return the lowest return code. Since RPC is also executed 
    ** on the local unit we are guaranteed at least one successful completion.
    */
    rv = BCM_E_NONE;
    for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
    {
      if (rpc_status[i] == HPC_HW_RPC_OK)
      {
        if (app_status[i] < rv)
        {
          rv = app_status[i];
        }
      }
    }
  } else
  {
    /* RPC is not ready. This is probably a transient error, so treat it as a timeout.
    */
    rv = BCM_E_TIMEOUT;
  }


  return rv;
}

/*********************************************************************
* @purpose  Custom RPC function to handle modifications to the
*           source interface when processing RPF failures.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_ipmc_enable_set(L7_uint32 transaction_id,
                                      hpcHwRpcData_t *rpc_data,
                                      L7_int32 *status,
                                      hpcHwRpcData_t  *rpc_resp)
{
  L7_uint32           *enable;
  int                  rv = BCM_E_NONE;

  enable = (L7_uint32 *) rpc_data->buf;

  rv = usl_bcm_ipmc_enable_set(*enable);

  *status = rv;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Makes the rpc call for dropmode (ingress or egress)
*
* @param    mode    @{{input}}  enable=1,disable=0
*
* @returns  
*
* @end
*********************************************************************/
int l7_rpc_client_mmu_dropmode_set(L7_uint32 mode)
{
  int rc;
  int rv = BCM_E_NONE;
  hpcHwRpcStatus_t rpc_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcStatus_t app_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t   rpc_data;
  int data;

  data = mode;

  rpc_data.buf = (L7_uchar8*)&data;
  rpc_data.buf_size = rpc_data.data_len = sizeof(data);

  rc = hpcHardwareRpc(L7_ALL_UNITS,
                      L7_RPC_MMU_DROPMODE_HANDLER,
                      &rpc_data,
                      rpc_status,
                      0, L7_NULL);
  
  if (L7_SUCCESS == rc)
  {
      int i;

      for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
      {
          if (HPC_HW_RPC_OK == rpc_status[i])
          {
              if (app_status[i] < rv)
                  rv = app_status[i];
          }
      }
  }
  else
  {
      rv = BCM_E_TIMEOUT;
  }
  
  return rv;
}
/*********************************************************************
* @purpose  Custom RPC function switch mmu to ingress/egress drop mode
*
* @param    
*
* @returns  L7_RC_t
*   
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_mmu_dropmode_set(L7_uint32 tid,
                                        hpcHwRpcData_t *rpc_data,
                                        L7_int32 *status,
                                        hpcHwRpcData_t  *rpc_resp)
{
  int rv = BCM_E_NONE;
  int  *data;
  
  if (rpc_data->data_len != sizeof(int))
  {
      *status = BCM_E_FAIL;
      return L7_SUCCESS;
  }                   
                      
  data = (int *) rpc_data->buf;
  
  rv = usl_bcm_mmu_dropmode_set (*data);
  
  *status = rv;
  return L7_SUCCESS;
}     
 
/*********************************************************************
* @purpose  Flow control Init to register the RPC call handler.
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/
void l7_system_rpc_init()
{
  L7_RC_t rc;

  rc = hpcHardwareRpcRegister(L7_RPC_FLOW_CONTROL_HANDLER, 
                              l7_rpc_server_flow_control_set);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  rc = hpcHardwareRpcRegister(L7_RPC_DOS_CONTROL_HANDLER, 
                              l7_rpc_server_doscontrol_set);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_IPMCAST_ENABLE_SET,
                               l7_rpc_server_ipmc_enable_set);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }

  rc = hpcHardwareRpcRegister(L7_RPC_MMU_DROPMODE_HANDLER,
                              l7_rpc_server_mmu_dropmode_set);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }
} 


