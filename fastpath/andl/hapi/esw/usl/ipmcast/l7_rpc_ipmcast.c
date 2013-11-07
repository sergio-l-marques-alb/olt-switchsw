
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_rpc_ipmcast.c
*
* @purpose    RPC layer 
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

#ifdef L7_MCAST_PACKAGE

#include "osapi.h"
#include "log.h"
#include "soc/drv.h"
#include "bcmx/custom.h"
#include "bcmx/bcmx_int.h"

#include "l7_rpc_ipmcast.h"

#include "ptin_globaldefs.h"

#define RV_REPLACE(_trv, _rv)                   \
        BCMX_RV_REPLACE_OK(_trv, _rv, BCM_E_UNAVAIL)


/*********************************************************************
* @purpose  Add a IP Mcast entry
*
* @param    data     @{(input)} IPMC bcm data
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_add(usl_bcm_ipmc_addr_t *data,
                           L7_uint32 numChips, L7_uint32 *chips)
{

  int                 rv = BCM_E_NONE;
  hpcHwRpcStatus_t    rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32            app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_RC_t             rc;
  L7_uint32           i;
  hpcHwRpcData_t      rpc_data;

  rpc_data.buf = (L7_uchar8 *) data;
  rpc_data.buf_size = rpc_data.data_len = sizeof (*data);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_IPMCAST_IPMC_ADD,
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
* @purpose  Custom RPC function to handle multicast route adds.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_ipmc_add (L7_uint32 transaction_id,
                                hpcHwRpcData_t *rpc_data,
                                L7_int32 *status,
                                hpcHwRpcData_t  *rpc_resp)
{
  usl_bcm_ipmc_addr_t  *ipmc;
  int rv = BCM_E_NONE;

  ipmc = (usl_bcm_ipmc_addr_t *) rpc_data->buf;

  rv = usl_bcm_ipmc_add(ipmc, L7_FALSE);

  *status = rv;  /* Return the worst error code. */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a IP Mcast entry
*
* @param    *data         @{(input)} The ipmc entry 
* @param    keep          @{(input)} Keep the entry in Table(1) or remove entry(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_remove(usl_bcm_ipmc_addr_t *data, int keep,
                              L7_uint32 numChips, L7_uint32 *chips)
{
  int                  rv = BCM_E_NONE;
  L7_RC_t              rc;
  hpcHwRpcStatus_t     rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32            app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uchar8           *msgBuf, *rpcBuf;
  L7_uint32            i, msgSize;
  hpcHwRpcData_t       rpc_data;

  rpcBuf = l7_rpc_req_buffer_get();
  if (rpcBuf == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to get RPC request buffer\n");

    rv = BCM_E_FAIL;
    return rv;
  }

  msgBuf = rpcBuf;

  msgSize = sizeof (*data) + sizeof(keep);
  if (msgSize > l7_rpc_req_buffer_size_get())
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Msg size %d is greater than RPC buffer size %d\n",
            msgSize,l7_rpc_req_buffer_size_get());
           
    LOG_ERROR(msgSize); 
  }


  rpc_data.buf = (L7_uchar8 *) rpcBuf;
  rpc_data.buf_size = rpc_data.data_len = msgSize ;

  memcpy(msgBuf, data, sizeof(*data));
  msgBuf += sizeof(*data);
  L7_UINT32_PACK(msgBuf, keep);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_IPMCAST_IPMC_DELETE,
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

  l7_rpc_req_buffer_free(rpcBuf);

  return rc;
}

/*********************************************************************
* @purpose  Custom RPC function to handle multicast route deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_ipmc_delete (L7_uint32 transaction_id,
                                   hpcHwRpcData_t *rpc_data,
                                   L7_int32 *status,
                                   hpcHwRpcData_t  *rpc_resp)
{
  int                  rv = BCM_E_NONE;
  L7_uint32            keep;
  usl_bcm_ipmc_addr_t   ipmc;
  L7_uchar8           *msgBuf;
  
  memset(&ipmc, 0, sizeof(ipmc));

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }


  /* Unpack the data */
  msgBuf = rpc_data->buf;
  memcpy(&ipmc, msgBuf, sizeof(ipmc));
  msgBuf += sizeof(ipmc);
  L7_UINT32_UNPACK(msgBuf, keep);

  rv = usl_bcm_ipmc_delete(&ipmc, keep);

  *status = rv;  

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Modify the entry in the table
*
* @param    ipmc  @{(input)} IPMC Bcm data
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_port_tgid_set(usl_bcm_ipmc_addr_t *ipmc,
                                     L7_uint32 numChips, L7_uint32 *chips)
{
  int               rv = BCM_E_NONE;
  L7_RC_t           rc;
  hpcHwRpcStatus_t  rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32          app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32         i;
  hpcHwRpcData_t    rpc_data;


  rpc_data.buf = (L7_uchar8 *) ipmc;
  rpc_data.buf_size = rpc_data.data_len = sizeof (*ipmc);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_IPMCAST_RPF_SET,
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
L7_RC_t l7_rpc_server_ipmc_rpf_set (L7_uint32 transaction_id,
                                    hpcHwRpcData_t *rpc_data,
                                    L7_int32 *status,
                                    hpcHwRpcData_t  *rpc_resp)
{
  usl_bcm_ipmc_addr_t  *ipmc;
  int                  rv = BCM_E_NONE;

  ipmc = (usl_bcm_ipmc_addr_t *) rpc_data->buf;

  rv = usl_bcm_ipmc_rpf_set(ipmc);

  *status = rv;  

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set L2 ports for an ip multicast group
*
* @param    ipmc_addr    @{(input)} IPMC Address info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_set_l2_ports(usl_bcm_ipmc_addr_t *ipmc_addr, 
                                    L7_uint32 numChips, L7_uint32 *chips)

{
  int                   rv = BCM_E_NONE;
  L7_RC_t               rc;
  hpcHwRpcStatus_t      rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32              app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_uint32             i;
  hpcHwRpcData_t        rpc_data;


  rpc_data.buf = (L7_uchar8 *) ipmc_addr;
  rpc_data.buf_size = rpc_data.data_len = sizeof (*ipmc_addr);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_IPMCAST_L2_PORT_SET,
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
*
* @purpose Sets the L2 ports for the multicast group.
*
* @param    
*
* @returns BCM Error Code
*
* @notes This function sets the L2 ports for the specified group and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
L7_RC_t l7_rpc_server_ipmc_set_l2_ports (L7_uint32 transaction_id,
                                         hpcHwRpcData_t *rpc_data,
                                         L7_int32 *status,
                                         hpcHwRpcData_t  *rpc_resp)

{
  usl_bcm_ipmc_addr_t *ipmc_addr;
  int                 rv = BCM_E_NONE;


  ipmc_addr = (usl_bcm_ipmc_addr_t *) rpc_data->buf;
  
  rv = usl_bcm_ipmc_set_l2_ports(ipmc_addr);

  *status = rv;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Add L2 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_add_l2_port_groups (bcmx_lport_t      port,
                                       L7_uint32        *ipmc_index,
                                       L7_uint32         num_groups,
                                       L7_uint32         vlan_id,
                                       L7_uint32         tagged)
{
  int                        rv = BCM_E_NONE;
  L7_uint32                  i;
  usl_bcmx_port_ipmc_cmd_t  *ipmc_cmd;
  uint32                     args[BCM_CUSTOM_ARGS_MAX];
  uint32                     n_args;

  memset (&args, 0, sizeof (args));

  if ((num_groups < 1) || (num_groups > L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL ))
  {
    LOG_ERROR (num_groups);
  }

  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  /* Pack the IPMC index list into a bit mask. We can do this because IPMC indexes
  ** are less or equal to the maximum IPMC table size.
  */
  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
    {
      LOG_ERROR (ipmc_index[i]);
    }

    USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET (ipmc_index[i], 
                                         ipmc_cmd->ipmc_index_mask);
  }

  ipmc_cmd->vlan_id = vlan_id;
  ipmc_cmd->tagged = tagged;

  n_args = sizeof(usl_bcmx_port_ipmc_cmd_t)/sizeof(uint32);
  if (sizeof(usl_bcmx_port_ipmc_cmd_t)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_L2_PORT_ADD, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_L2_PORT_ADD, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose Add L2 ports to the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int l7_rpc_server_ipmc_add_l2_port_groups (int unit, bcm_port_t port, int setget,
                                           uint32 *args)
{
  int                            rv = BCM_E_NONE;
  usl_bcmx_port_ipmc_cmd_t      *ipmc_cmd;


  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  rv = usl_bcm_ipmc_add_l2_port_groups(unit, port, ipmc_cmd);

  return rv;
}


/*********************************************************************
*
* @purpose Delete L2 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_delete_l2_port_groups(bcmx_lport_t port,
                                         L7_uint32    *ipmc_index,
                                         L7_uint32    num_groups,
                                         L7_uint32    vlan_id)
{
  int                           rv = BCM_E_NONE;
  L7_uint32                     i;
  usl_bcmx_port_ipmc_cmd_t     *ipmc_cmd;
  uint32                        args[BCM_CUSTOM_ARGS_MAX];
  uint32                        n_args;

  memset (&args, 0, sizeof (args));

  if ((num_groups < 1) || 
      (num_groups > L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL ))
  {
    LOG_ERROR (num_groups);
  }

  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  /* Pack the IPMC index list into a bit mask. We can do this because IPMC indexes
  ** are less or equal to the maximum IPMC table size.
  */
  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
    {
      LOG_ERROR (ipmc_index[i]);
    }

    USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET (ipmc_index[i],
                                         ipmc_cmd->ipmc_index_mask);
  }

  ipmc_cmd->vlan_id = vlan_id;
  ipmc_cmd->tagged = 0;

  n_args = sizeof(usl_bcmx_port_ipmc_cmd_t)/sizeof(uint32);
  if (sizeof(usl_bcmx_port_ipmc_cmd_t)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_L2_PORT_DELETE, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_L2_PORT_DELETE, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose Delete L2 ports from the multicast group.
*
* @param   
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int l7_rpc_server_ipmc_delete_l2_port_groups (int unit, bcm_port_t port, 
                                              int setget, uint32 *args)
{
  int                           rv = BCM_E_NONE;
  usl_bcmx_port_ipmc_cmd_t     *ipmc_cmd;
  
  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  rv = usl_bcm_ipmc_delete_l2_port_groups(unit, port, ipmc_cmd);

  return rv;
}

/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_add_l3_port_groups (bcmx_lport_t port,
                                       L7_uint32    *ipmc_index,
                                       L7_uint32    num_groups,
                                       L7_uint32    vlan_id,
                                       L7_uint32    tagged,
                                       L7_uchar8    *mac,
                                       L7_uint32    ttl)
{
  int                          rv = BCM_E_NONE;
  L7_uint32                    i;
  usl_bcmx_port_ipmc_cmd_t    *ipmc_cmd;
  uint32                       args[BCM_CUSTOM_ARGS_MAX];
  uint32                       n_args;

  memset (&args, 0, sizeof (args));

  if ((num_groups < 1) || (num_groups > L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL ))
  {
    LOG_ERROR (num_groups);
  }

  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  /* Pack the IPMC index list into a bit mask. We can do this because IPMC indexes
  ** are less or equal to the maximum IPMC table size.
  */
  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
    {
      LOG_ERROR (ipmc_index[i]);
    }

    USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET (ipmc_index[i], 
                                         ipmc_cmd->ipmc_index_mask);
  }

  ipmc_cmd->vlan_id = vlan_id;
  ipmc_cmd->tagged = tagged;

  memcpy (ipmc_cmd->mac, mac, 6);
  ipmc_cmd->ttl = ttl;
  
  n_args = sizeof(usl_bcmx_port_ipmc_cmd_t)/sizeof(uint32);
  if (sizeof(usl_bcmx_port_ipmc_cmd_t)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_L3_PORT_ADD, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_L3_PORT_ADD, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param    
*
* @returns BCM Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int l7_rpc_server_ipmc_add_l3_port_groups (int unit, bcm_port_t port, int setget,
                                           uint32 *args)
{
  int                       rv = BCM_E_NONE;
  usl_bcmx_port_ipmc_cmd_t *ipmc_cmd;

  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  rv = usl_bcm_ipmc_add_l3_port_groups(unit, port, ipmc_cmd);

  return rv;
}

/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - single IPMC index to modify with this call.
* @param   encap_id - encap_id associated with the port.
*
* @returns BCMX Error Code
*
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_egress_port_add (bcmx_lport_t port,
                                    L7_uint32    *ipmc_index,
                                    L7_uint32    encap_id)
{
  int                          rv = BCM_E_NONE;
  usl_bcmx_port_ipmc_cmd_t    *ipmc_cmd;
  uint32                       args[BCM_CUSTOM_ARGS_MAX];
  uint32                       n_args;

  memset (&args, 0, sizeof (args));


  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  if (*ipmc_index >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
  {
    LOG_ERROR (*ipmc_index);
  }

  USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET (*ipmc_index, 
                                       ipmc_cmd->ipmc_index_mask);

  ipmc_cmd->encap_id = encap_id;
  
  n_args = sizeof(usl_bcmx_port_ipmc_cmd_t)/sizeof(uint32);
  if (sizeof(usl_bcmx_port_ipmc_cmd_t)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_EGRESS_PORT_ADD, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_EGRESS_PORT_ADD, args);
  #endif

  return rv;
}
/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param    
*
* @returns BCM Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int l7_rpc_server_ipmc_egress_port_add (int unit, bcm_port_t port, int setget,
                                        uint32 *args)
{
  int                       rv = BCM_E_NONE;
  usl_bcmx_port_ipmc_cmd_t *ipmc_cmd;

  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  rv = usl_bcm_ipmc_egress_port_add(unit, port, ipmc_cmd);

  return rv;
}

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_delete_l3_port_groups (bcmx_lport_t port,
                                          L7_uint32    *ipmc_index,
                                          L7_uint32    num_groups,
                                          L7_uint32    vlan_id)
{
  int rv = BCM_E_NONE;
  L7_uint32 i;
  usl_bcmx_port_ipmc_cmd_t *ipmc_cmd;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  memset (&args, 0, sizeof (args));

  if ((num_groups < 1) || (num_groups > L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL ))
  {
    LOG_ERROR (num_groups);
  }

  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  /* Pack the IPMC index list into a bit mask. We can do this because IPMC indexes
  ** are less or equal to the maximum IPMC table size.
  */
  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
    {
      LOG_ERROR (ipmc_index[i]);
    }

    USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET (ipmc_index[i],
                                         ipmc_cmd->ipmc_index_mask);
  }

  ipmc_cmd->vlan_id = vlan_id;

  n_args = sizeof(usl_bcmx_port_ipmc_cmd_t)/sizeof(uint32);
  if (sizeof(usl_bcmx_port_ipmc_cmd_t)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_L3_PORT_DELETE, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_L3_PORT_DELETE, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int l7_rpc_server_ipmc_delete_l3_port_groups (int unit, bcm_port_t port, 
                                              int setget, uint32 *args)
{
  int                            rv = BCM_E_NONE;
  usl_bcmx_port_ipmc_cmd_t      *ipmc_cmd;
  
  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  rv = usl_bcm_ipmc_delete_l3_port_groups(unit, port, ipmc_cmd);

  return rv;
}

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - single IPMC index to modify with this call.
* @param   encap_id - encap_id associated with the port.
*
* @returns BCMX Error Code
*
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_egress_port_delete (bcmx_lport_t port,
                                       L7_uint32    *ipmc_index,
                                       L7_uint32    encap_id)
{
  int rv = BCM_E_NONE;
  usl_bcmx_port_ipmc_cmd_t *ipmc_cmd;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  memset (&args, 0, sizeof (args));

  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  if (*ipmc_index >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
  {
    LOG_ERROR (*ipmc_index);
  }
  USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET (*ipmc_index,
                                       ipmc_cmd->ipmc_index_mask);

  ipmc_cmd->encap_id = encap_id;

  n_args = sizeof(usl_bcmx_port_ipmc_cmd_t)/sizeof(uint32);
  if (sizeof(usl_bcmx_port_ipmc_cmd_t)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_EGRESS_PORT_DELETE, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_IPMC_EGRESS_PORT_DELETE, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int l7_rpc_server_ipmc_egress_port_delete (int unit, bcm_port_t port, 
                                              int setget, uint32 *args)
{
  int                            rv = BCM_E_NONE;
  usl_bcmx_port_ipmc_cmd_t      *ipmc_cmd;
  
  ipmc_cmd = (usl_bcmx_port_ipmc_cmd_t *) args;

  rv = usl_bcm_ipmc_egress_port_delete(unit, port, ipmc_cmd);

  return rv;
}


/*********************************************************************
* @purpose  Initialize custom IP Multicast RPC calls.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
void l7_custom_rpc_ipmcast_init (void)
{
  L7_RC_t rc;


    rc = hpcHardwareRpcRegister (L7_RPC_IPMCAST_IPMC_ADD,
                                 l7_rpc_server_ipmc_add);
    if (rc != L7_SUCCESS)
    {
      LOG_ERROR (rc);
    }

    rc = hpcHardwareRpcRegister (L7_RPC_IPMCAST_IPMC_DELETE,
                                 l7_rpc_server_ipmc_delete);
    if (rc != L7_SUCCESS)
    {
      LOG_ERROR (rc);
    }

    rc = hpcHardwareRpcRegister (L7_RPC_IPMCAST_RPF_SET,
                                 l7_rpc_server_ipmc_rpf_set);
    if (rc != L7_SUCCESS)
    {
      LOG_ERROR (rc);
    }

    rc = hpcHardwareRpcRegister (L7_RPC_IPMCAST_L2_PORT_SET,
                                 l7_rpc_server_ipmc_set_l2_ports);
    if (rc != L7_SUCCESS)
    {
      LOG_ERROR (rc);
    }
}

/*********************************************************************
*
* @purpose Handle custom bcmx commands for port configuration 
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} config data
* @param    rv            @{(output)} return value if the command
*                                     was executed
*
*
* @returns L7_TRUE: If the command was handled
*          L7_FALSE: If the command was not handled
*
* @notes 
*
* @end
*
*********************************************************************/
L7_BOOL l7_rpc_server_ipmc_handler(int unit, bcm_port_t port, int setget,
                                   int type, uint32 *args, int *rv)
{
  L7_BOOL handled = L7_TRUE;

  switch (type)
  {
    case USL_BCMX_IPMC_L2_PORT_ADD:
      *rv = l7_rpc_server_ipmc_add_l2_port_groups (unit, port, setget, args);
      break;

    case USL_BCMX_IPMC_L2_PORT_DELETE:
      *rv = l7_rpc_server_ipmc_delete_l2_port_groups (unit, port, setget, args);
      break;

    case USL_BCMX_IPMC_L3_PORT_ADD:
      *rv = l7_rpc_server_ipmc_add_l3_port_groups (unit, port, setget, args);
      break;

    case USL_BCMX_IPMC_L3_PORT_DELETE:
      *rv = l7_rpc_server_ipmc_delete_l3_port_groups (unit, port, setget, args);
      break;

    case USL_BCMX_IPMC_EGRESS_PORT_ADD:
      *rv = l7_rpc_server_ipmc_egress_port_add (unit, port, setget, args);
      break;
    case USL_BCMX_IPMC_EGRESS_PORT_DELETE:
      *rv = l7_rpc_server_ipmc_egress_port_delete (unit, port, setget, args);
      break;

     default: 
       handled = L7_FALSE;
       break;
    }

  return handled;
}


#endif /* L7_MCAST_PACKAGE */
