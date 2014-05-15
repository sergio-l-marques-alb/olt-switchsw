/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_rpc_l3.c
*
* @purpose    New layer to handle directing driver calls to specified chips
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

#ifdef L7_ROUTING_PACKAGE

#include "osapi.h"
#include "hpc_hw_api.h"
#include "log.h"
#include "ibde.h"
#include "soc/drv.h"
#include "l7_rpc_l3.h"
#include "broad_common.h"
#include "sal/core/libc.h"


#ifdef L7_STACKING_PACKAGE
/* If bulk L3 transactions are not desired, undefine this */
#define HAPI_BROAD_L3_BULK_SUPPORT   1
#endif

#define L7_RPC_L3_DEBUG(format, args...)                             \
 do                                                                  \
 {                                                                   \
   if (l3RpcDebug == L7_ENABLE)                                      \
   {                                                                 \
     sysapiPrintf("\n"format, ##args);                               \
   }                                                                 \
 } while(0)                                                          \


#define L7_RPC_L3_HOST_ADD \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 10)
#define L7_RPC_L3_HOST_DELETE \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 11)
#define L7_RPC_L3_ROUTE_ADD \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 12)
#define L7_RPC_L3_ROUTE_DELETE \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 13)
#define L7_RPC_L3_TUNNEL_INITIATOR_SET \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 14)
#define L7_RPC_L3_TUNNEL_INITIATOR_CLEAR \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 15)
#define L7_RPC_L3_TUNNEL_TERMINATOR_ADD \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 16)
#define L7_RPC_L3_TUNNEL_TERMINATOR_DELETE \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 17)
#define L7_RPC_L3_EGRESS_CREATE \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 18)
#define L7_RPC_L3_EGRESS_DESTROY \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 19)
#define L7_RPC_L3_EGRESS_MULTIPATH_CREATE \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 20)
#define L7_RPC_L3_EGRESS_MULTIPATH_DESTROY \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 21)
#define L7_RPC_L3_INTF_CREATE \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 22)
#define L7_RPC_L3_INTF_DELETE \
          HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 23)


/* Note IDs 30+ are used by IPMCAST, so we have 10-30 for L3 unicast */

/* Egress object Data to pass via RPC call */
typedef struct l7RpcEgrInfo_s { 
  L7_uint32 flags;     /* Egress flags - create/replace */
  bcm_if_t  intf;      /* Egress oject id - if replace */
  usl_bcm_l3_egress_t egr; /* Egress object information */
} l7RpcEgrInfo_t;

/* Response for each egress object */
typedef struct l7RpcEgrResp_s {
  L7_int32 rv;       /* Return code for egress create/delete operation */
  bcm_if_t egrId;    /* Egress Id */     
} l7RpcEgrResp_t;

/* Multipath egress data to pass via RPC call */
typedef struct l7RpcMpathEgrInfo_s {
  L7_uint32 flags;
  bcm_if_t  mpintf;
  L7_int32  count;
  bcm_if_t  intf[L7_RT_MAX_EQUAL_COST_ROUTES];
} l7RpcMpathEgrInfo_t;


/* To help with performance tests this function can be used to dynamically
** configure whether to use Broadcom or LVL7 RPC.
*/
static L7_uint32 use_lvl7_rpc = 1;

/* Buffer to hold/process RPC data assoicated with intfs/routes/Nhops/hosts */

static hpcHwRpcData_t rpcIntfResp[L7_MAX_UNITS_PER_STACK+1];

/* Egress nexthop object RPC Data from client to server */
static hpcHwRpcData_t rpcEgrNhopData;

/* RPC response for egress nexthop object from server */
static hpcHwRpcData_t rpcEgrNhopResp[L7_MAX_UNITS_PER_STACK+1];

/* Host RPC Data from client to server */
static hpcHwRpcData_t rpcHostData;

/* Resp data from each unit */
static hpcHwRpcData_t  rpcHostResp[L7_MAX_UNITS_PER_STACK+1];

/* Route RPC Data from client to server */
static hpcHwRpcData_t  rpcRouteData; 

/* Resp Data from each unit */
static hpcHwRpcData_t  rpcRouteResp[L7_MAX_UNITS_PER_STACK+1]; 

static L7_uint32 l3RpcDebug = L7_DISABLE;

void l7_l3_rpc_debug_enable(L7_uint32 enable)
{
  if (enable)
  {
    l3RpcDebug = L7_ENABLE; 
  }
  else
  {
    l3RpcDebug = L7_DISABLE;
  }
}

L7_uint32 l7_l3_rpc_mode (L7_uint32 mode)
{
  use_lvl7_rpc = mode;

  return mode;
}



/*********************************************************************
* @purpose  Create an L3 Interface
*
* @param    info     @{(input)} Interface attributes
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_l3_intf_create(usl_bcm_l3_intf_t *info,
                                 L7_uint32 numChips, L7_uint32 *chips)
{
  int rv = BCM_E_NONE;
  L7_uint32 i;
  L7_RC_t result;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t rpc_data;

  rpc_data.buf = (L7_uchar8 *)info;
  rpc_data.buf_size = rpc_data.data_len = sizeof (*info);

  /* Clear the response buffer */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(rpcIntfResp[i].buf, 0, rpcIntfResp[i].buf_size);
    rpcIntfResp[i].data_len = 0;
  }

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  result = hpcHardwareRpc (L7_ALL_UNITS,
                           L7_RPC_L3_INTF_CREATE,
                           &rpc_data,
                           rpc_status,
                           app_status,
                           rpcIntfResp);

  if (result == L7_SUCCESS)
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
        if (app_status[i] == BCM_E_NONE)
        {
          /* Check the response length */
          if (rpcIntfResp[i].data_len == sizeof(bcm_if_t))
          {
            info->bcm_data.l3a_intf_id = *(bcm_if_t *)rpcIntfResp[i].buf;

            printf("%s (%d): l3a_intf_id=%d\n", __FUNCTION__, __LINE__, info->bcm_data.l3a_intf_id);
          }
          else 
            rv = BCM_E_INTERNAL;
        }

        if (app_status[i] < rv)
        {
          rv = app_status[i];
        }
      }
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
* @purpose  Custom RPC function to handle interface create
*
* @param
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_intf_create (L7_uint32 transaction_id,
                                      hpcHwRpcData_t *rpc_data,
                                      L7_int32 *status,
                                      hpcHwRpcData_t  *rpc_resp)
{
  int rv = BCM_E_NONE;
  usl_bcm_l3_intf_t intf;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  memcpy(&intf, rpc_data->buf, sizeof(intf));

  rv = usl_bcm_l3_intf_create(&intf);
  if (L7_BCMX_OK(rv) == L7_TRUE)
  {
    *(bcm_if_t *)rpc_resp->buf = intf.bcm_data.l3a_intf_id;
    rpc_resp->data_len = sizeof (bcm_if_t);
  }
  
  *status = rv;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete an L3 Interface
*
* @param    info     @{(input)} Interface attributes
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_l3_intf_delete(usl_bcm_l3_intf_t *info,
                                 L7_uint32 numChips, L7_uint32 *chips)
{
  int rv = BCM_E_NONE;
  L7_uint32 i;
  L7_RC_t result;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t rpc_data;

  rpc_data.buf = (L7_uchar8 *)info;
  rpc_data.buf_size = rpc_data.data_len = sizeof (*info);

  result = hpcHardwareRpc (L7_ALL_UNITS,
                           L7_RPC_L3_INTF_DELETE,
                           &rpc_data,
                           rpc_status,
                           app_status,
                           L7_NULL);

  if (result == L7_SUCCESS)
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
* @purpose  Custom RPC function to handle interface delete
*
* @param
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_intf_delete (L7_uint32 transaction_id,
                                      hpcHwRpcData_t *rpc_data,
                                      L7_int32 *status,
                                      hpcHwRpcData_t  *rpc_resp)
{
  int rv = BCM_E_NONE;
  usl_bcm_l3_intf_t intf;

  memcpy(&intf, rpc_data->buf, sizeof(intf));

  rv = usl_bcm_l3_intf_delete(&intf);

  *status = rv;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a Host entry or entries to the table
*
* @param    info     @{(input)} The address and associated data
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the hosts (SDK rv)
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @notes    It is expected that the caller should only send data that
*           can fit in a single RPC transaction. In other words, there is
*           an upper limit on the number of hosts that can be sent in an
*           RPC transaction. See l7_custom_rpc_l3_max_host_get().
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_host_add(usl_bcm_l3_host_t * info, 
                                  L7_uint32 count, L7_int32 *rv,
                                  L7_uint32 numChips, L7_uint32 *chips)
{
  L7_RC_t result = L7_SUCCESS;
  L7_uint32 i, j;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  usl_bcm_l3_host_t *pBcmHostInfo;
  L7_int32 *pResp;

  L7_RPC_L3_DEBUG("l7_rpc_client_l3_host_add (%d): ******************* JVM *******************\n", __LINE__);

  if ((count == 0) || (count > l7_custom_rpc_l3_max_host_get()))
  {
    /* No data or too much data */
    L7_RPC_L3_DEBUG("l7_rpc_client_l3_host_add: (%d) count=%d\n", __LINE__, count);
    return L7_FAILURE;
  }

  /* Convert BCMX format to BCM format.  */
  memset(rpcHostData.buf, 0, rpcHostData.buf_size);
  pBcmHostInfo = (usl_bcm_l3_host_t *)rpcHostData.buf;

  for (i=0; i < count; i++)
  {
    memcpy(pBcmHostInfo, info, sizeof(*info));
    pBcmHostInfo++;
    info++;
  }
  rpcHostData.data_len = count * sizeof(usl_bcm_l3_host_t);

  /* Clear the response buffer. Expecting rv in response */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(rpcHostResp[i].buf, 0, rpcHostResp[i].buf_size);
    rpcHostResp[i].data_len = 0;
  }

  result = hpcHardwareRpc (L7_ALL_UNITS,
                           L7_RPC_L3_HOST_ADD,
                           &rpcHostData,
                           rpc_status,
                           app_status, 
                           rpcHostResp);

  if (result != L7_SUCCESS)
  {

    L7_RPC_L3_DEBUG("l7_rpc_client_l3_host_add (%d): No response from unit %d, result=%d\n", __LINE__, i, result);

    return L7_FAILURE;
  }

  /* Loop through units that successfully completed RPC and
   * return the lowest return code. Since RPC is also executed 
   * on the local unit we are guaranteed at least one successful completion.
   */
  for (i=0; i < count; i++)
  {
    rv[i] =  BCM_E_NONE;
  } 
 
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    if (rpc_status[i] == HPC_HW_RPC_OK)
    {
      /* Process the response and application status */
      if ((app_status[i] == L7_SUCCESS) && (rpcHostResp[i].data_len > 0))
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_host_add (%d): Received %d resp from unit %d\n",
                        __LINE__, rpcHostResp[i].data_len, i);

        /* Check response length */
        if (rpcHostResp[i].data_len == (count * sizeof(L7_int32)))
        {
          pResp = (L7_int32 *)rpcHostResp[i].buf;
          for(j=0; j < count; j++)
          {
            if (pResp[j] < rv[j])
            {
              /* Note the worst error code per route entry */
              rv[j] = pResp[j];
            }
          }
        }
        else
        {
          L7_RPC_L3_DEBUG("l7_rpc_client_l3_host_add (%d): No response from unit %d, result=%d\n", __LINE__, i, result);
          result = L7_FAILURE; /* Didn't get the expected response */
        }
      }
      else
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_host_add (%d): No response from unit %d, result=%d\n", __LINE__, i, result);
        result = L7_FAILURE;
      }
    }
  }

  /* Check the overall result of the transaction */
  if (result == L7_FAILURE)
  {

    L7_RPC_L3_DEBUG("l7_rpc_client_l3_host_add (%d): No response from unit %d, result=%d\n", __LINE__, i, result);

    /* Here if RPC didn't complete on all existing units, or we didn't get
     * expected reponse from existing units. Fail all the entries so that
     * all units are in sync.
     */
    for (i=0; i < count; i++)
    {
      rv[i] =  BCM_E_INTERNAL;
    }
  }
 
  L7_RPC_L3_DEBUG("l7_rpc_client_l3_host_add (%d): JVM, result=%d\n", __LINE__, result);
  return result;
}


/*********************************************************************
* @purpose  Custom RPC function to handle Host adds.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_host_add (L7_uint32 transaction_id,
                                   hpcHwRpcData_t *rpc_data,
                                   L7_int32 *status,
                                   hpcHwRpcData_t  *rpc_resp)
{
  usl_bcm_l3_host_t  *pHostInfo;
  L7_uint32 j, count = 0;
  L7_int32 rv;
  L7_int32 *pResp;
  L7_uint32 resp_len = 0;

  L7_RPC_L3_DEBUG("l7_rpc_server_l3_host_add (%d): ******************* JVM *******************\n", __LINE__);

  if (rpc_data->data_len == 0) /* No data */
  {
     *status = L7_FAILURE;
     return L7_SUCCESS;
  }

  count = rpc_data->data_len / sizeof (usl_bcm_l3_host_t);

  if (count == 0)  /* Invalid data len */
  {
    /* Indicate that there was error processing the host data.
     * Note, the response buffer is valid only when incoming host data
     * is processed and we really attempted to add the hosts.
     */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }
  
  L7_RPC_L3_DEBUG("l7_rpc_server_l3_host_add: Rpc Data %d, Count %d\n", 
                  rpc_data->data_len, count);

  pHostInfo = (usl_bcm_l3_host_t *) rpc_data->buf;
  pResp = (L7_int32 *)rpc_resp->buf;
  rpc_resp->data_len = 0;

  /* Loop through the hosts and add them to local units. */
  for (j = 0; j < count; j++)
  {
    rv = usl_bcm_l3_host_add(pHostInfo);

    /* For each host entry, copy the return code in the response */
    pResp[j] = rv;
    resp_len += sizeof(rv);
    pHostInfo++;

    L7_RPC_L3_DEBUG("l7_rpc_server_l3_host_add: count %d, returning rv %d, resp %d\n", count, rv, resp_len);
  }


  rpc_resp->data_len = resp_len;
 
  /* Indicate that all hosts were processed. This means all the hosts
   * in the request have been attempted and the response data is valid.
   * That is, actual err code per host is copied in the response
   */

  *status = L7_SUCCESS;  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a Host entry or entries
*
* @param    host     @{(input)} The host to remove
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the hosts (SDK rv)
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @notes    It is expected that the caller should only send data that
*           can fit in a single RPC transaction. In other words, there is
*           an upper limit on the number of hosts that can be sent in an
*           RPC transaction. See l7_custom_rpc_l3_max_host_get().
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_host_delete(usl_bcm_l3_host_t * info,
                                     L7_uint32 count, L7_int32 *rv,
                                     L7_uint32 numChips, L7_uint32 *chips)
{
  L7_RC_t  result = L7_SUCCESS;
  L7_uint32 i, j;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  usl_bcm_l3_host_t *pBcmHostInfo;
  L7_int32 *pResp;

  /* Convert BCMX format to BCM format.  */
  memset(rpcHostData.buf, 0, rpcHostData.buf_size);
  pBcmHostInfo = (usl_bcm_l3_host_t *)rpcHostData.buf;

  for (i=0; i < count; i++)
  {
    memcpy(pBcmHostInfo, info, sizeof(*info));
    pBcmHostInfo++;
    info++;
  }
  rpcHostData.data_len = count * sizeof(usl_bcm_l3_host_t);

  /* Clear the response buffer. Expecting rv in response */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(rpcHostResp[i].buf, 0, rpcHostResp[i].buf_size);
    rpcHostResp[i].data_len = 0;
  }

  result = hpcHardwareRpc (L7_ALL_UNITS,
                           L7_RPC_L3_HOST_DELETE,
                           &rpcHostData,
                           rpc_status,
                           app_status, 
                           rpcHostResp);

  if (result != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Loop through units that successfully completed RPC and return
   ** the err code for each entry. Since RPC is also executed on the
   ** local unit we are guaranteed at least one successful
   ** completion.
   */

  /* At this point treat all entries as success by default. Here, we are
   * gauranteed that we have response from atleast one of the units, 
   * in which case the return code will be updated based on response.
   */
  for (i=0; i < count; i++)
  {
    rv[i] =  BCM_E_NONE;
  }

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    if (rpc_status[i] == HPC_HW_RPC_OK)
    {
      /* Process the response and application status */
      if ((app_status[i] == L7_SUCCESS) && (rpcHostResp[i].data_len > 0))
      {
        L7_RPC_L3_DEBUG("l7_rpc_server_l3_host_delete: Received %d resp from unit %d\n",
                        rpcHostResp[i].data_len, i);

        pResp = (L7_int32 *)rpcHostResp[i].buf;

        /* Check response length */
        if (rpcHostResp[i].data_len == (count * sizeof(L7_int32)))
        {
          for(j=0; j < count; j++)
          {
            if (pResp[j] < rv[j])
            {
              /* Note the worst error code per route entry */
              rv[j] = pResp[j];
            }
          }
        }
        else
          result = L7_FAILURE; /* Didn't get the expected response */
      }
      else
      {
        L7_RPC_L3_DEBUG("l7_rpc_server_l3_host_delete: No response from unit %d\n", i);
        result = L7_FAILURE;
      }
    }
  }
 
  return result;
}

/*********************************************************************
* @purpose  Custom RPC function to handle Host deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_host_delete (L7_uint32 transaction_id,
                                      hpcHwRpcData_t *rpc_data,
                                      L7_int32 *status,
                                      hpcHwRpcData_t  *rpc_resp)
{
  usl_bcm_l3_host_t  *pHostInfo = (usl_bcm_l3_host_t *)rpc_data->buf;
  L7_uint32 j, count = 0;
  L7_int32  rv;
  L7_int32 *pResp;
  L7_uint32 resp_len = 0;  

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  count = rpc_data->data_len / sizeof (usl_bcm_l3_host_t);

  if (count == 0) /* Invalid data length in the request */
  {
    /* Indicate that there was error processing the host data. 
     * Note, the response buffer is valid only when incoming host data
     * is processed and we really attempted to delete the hosts.
     */
    *status = L7_FAILURE; 
    return L7_SUCCESS;
  }

  pResp = (L7_int32 *)rpc_resp->buf;
  rpc_resp->data_len = 0;
  
  for (j = 0; j < count; j++)
  {
    rv = usl_bcm_l3_host_delete(pHostInfo);

    pResp[j] = rv;
    resp_len += sizeof(rv);
    pHostInfo++;
  }

  L7_RPC_L3_DEBUG ("l7_rpc_server_l3_host_delete: returning resp %d\n", resp_len);
  rpc_resp->data_len = resp_len;

  /* Indicate that the host processing was success. This means all the hosts
   * in the request have been attempted and the response data is valid. 
   * That is, actual err code per host is copied in the response 
   */
  *status = L7_SUCCESS;  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add an LPM entry or entries to the table
*
* @param    info     @{(input)} The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @notes    It is expected that the caller should only send data that
*           can fit in a single RPC transaction. In other words, there is
*           an upper limit on the number of routes that can be sent in an
*           RPC transaction. See l7_custom_rpc_l3_max_route_get().
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_route_add(usl_bcm_l3_route_t * info,
                                   L7_uint32 count, L7_int32 *rv,
                                   L7_uint32 numChips, L7_uint32 *chips)
{
  L7_RC_t result  = L7_SUCCESS;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  usl_bcm_l3_route_t *pBcmRouteInfo;
  L7_uint32 i, j;
  L7_int32 *pResp;

  if ((count == 0) || (count > l7_custom_rpc_l3_max_route_get()))
  {
    /* No data or too much data */
    return L7_FAILURE;
  }
  
  /* Convert BCMX route structure to the BCM route structure. */
  memset(rpcRouteData.buf, 0, rpcRouteData.buf_size);
  pBcmRouteInfo = (usl_bcm_l3_route_t *)rpcRouteData.buf;

  for (i=0; i < count; i++)
  {
    memcpy(pBcmRouteInfo, info, sizeof(*info)); 
    pBcmRouteInfo++;
    info++;
  }

  rpcRouteData.data_len = count * sizeof(usl_bcm_l3_route_t);
    
  /* Clear the response buffer. Expecting rv in response */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(rpcRouteResp[i].buf, 0, rpcRouteResp[i].buf_size);
    rpcRouteResp[i].data_len = 0;
  }

  result = hpcHardwareRpc  (L7_ALL_UNITS,
                            L7_RPC_L3_ROUTE_ADD,
                            &rpcRouteData,
                            rpc_status,
                            app_status, 
                            rpcRouteResp);

  if (result != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Loop through units that successfully completed RPC and return
   ** the err code for each entry. Since RPC is also executed on the
   ** local unit we are guaranteed at least one successful
   ** completion.
   */

  /* At this point treat all entries as success by default. Here, we are
   * gauranteed that we have response from atleast one of the units,
   * in which case the return code will be updated based on response.
   */
  for (i=0; i < count; i++)
  {
    rv[i] =  BCM_E_NONE;
  }

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    if (rpc_status[i] == HPC_HW_RPC_OK)
    {
      /* Process the response and application status */
      if ((app_status[i] == L7_SUCCESS) && (rpcRouteResp[i].data_len > 0))
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_route_add: Received %d resp from unit %d\n",
                        rpcRouteResp[i].data_len, i);

        /* Check response length */
        if (rpcRouteResp[i].data_len == (count * sizeof(L7_int32)))
        {
          pResp = (L7_int32 *)rpcRouteResp[i].buf;
          for(j=0; j < count; j++)
          {
            if (pResp[j] < rv[j])
            {
              /* Note the worst error code per route entry */
              rv[j] = pResp[j];
            }
          }
        }
        else
          result = L7_FAILURE; /* Didn't get the expected response */
      }
      else
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_route_add: No response from unit %d\n", i);
        result = L7_FAILURE;
      }
    }
  }

  /* Check the overall result of the transaction */
  if (result == L7_FAILURE)
  {
    /* Here if RPC didn't complete on all existing units, or we didn't get
     * expected reponse from existing units. Fail all the entries so that
     * all units are in sync.
     */
    for (i=0; i < count; i++)
    {
      rv[i] =  BCM_E_INTERNAL;
    }
  }
  
  return result;
}

/*********************************************************************
* @purpose  Custom RPC function to handle Route adds.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_route_add (L7_uint32 transaction_id,
                                    hpcHwRpcData_t *rpc_data,
                                    L7_int32 *status,
                                    hpcHwRpcData_t  *rpc_resp)
{
  usl_bcm_l3_route_t  *pRouteInfo;
  L7_uint32  j, count = 0;
  L7_int32 rv;
  L7_int32 *resp;
  L7_uint32 resp_len = 0;

  if (rpc_data->data_len == 0) /* No data */
  {
     *status = L7_FAILURE;
     return L7_SUCCESS;
  }

  count = rpc_data->data_len / sizeof (usl_bcm_l3_route_t);

  if (count == 0)  /* Invalid data len */
  {
    /* Indicate that there was error processing the route data.
     * Note, the response buffer is valid only when incoming route data
     * is processed and we really attempted to add the routes.
     */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }
  
  L7_RPC_L3_DEBUG("l7_rpc_server_l3_route_add: Rpc Data %d, Count %d\n", 
                  rpc_data->data_len, count);

  pRouteInfo = (usl_bcm_l3_route_t *) rpc_data->buf;
  resp = (L7_int32 *)rpc_resp->buf;
  rpc_resp->data_len = 0;

  /* Loop through the routes and add them to local units. */
  for (j = 0; j < count; j++)
  {
    rv = usl_bcm_l3_route_add(pRouteInfo);

    /* For each route entry, copy the return code in the response */
    resp[j] = rv;
    resp_len += sizeof(rv);
    pRouteInfo++;
  }

  L7_RPC_L3_DEBUG("l7_rpc_server_l3_route_add: returning resp %d\n", resp_len);

  rpc_resp->data_len = resp_len;
 
  /* Indicate that all routes were processed. This means all the routes
   * in the request have been attempted and the response data is valid.
   * That is, actual err code per route is copied in the response
   */

  *status = L7_SUCCESS;  

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete LPM entry or entries from the table
*
* @param    info     @{(input)} The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @notes    It is expected that the caller should only send data that
*           can fit in a single RPC transaction. In other words, there is
*           an upper limit on the number of routes that can be sent in an
*           RPC transaction. See l7_custom_rpc_l3_max_route_get().
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_route_delete(usl_bcm_l3_route_t * info,
                                      L7_uint32 count, L7_int32 *rv,
                                      L7_uint32 numChips, L7_uint32 *chips)
{
  L7_RC_t result = L7_SUCCESS;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  usl_bcm_l3_route_t *pBcmRouteInfo;
  L7_uint32 i, j;
  L7_int32 *resp;

   
  if (count > l7_custom_rpc_l3_max_route_get())
  {
    return L7_FAILURE;
  }

  /* Convert BCMX route structure to the BCM route structure. */
  pBcmRouteInfo = (usl_bcm_l3_route_t *)rpcRouteData.buf;
  memset(pBcmRouteInfo, 0, rpcRouteData.buf_size);

  for (i=0; i < count; i++)
  {
    memcpy(pBcmRouteInfo, info, sizeof(*info));
    pBcmRouteInfo++;
    info++;
  }
  rpcRouteData.data_len = count * sizeof(usl_bcm_l3_route_t);

  /* Clear the response buffer */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(rpcRouteResp[i].buf, 0, rpcRouteResp[i].buf_size);
    rpcRouteResp[i].data_len = 0;
  }

  result = hpcHardwareRpc  (L7_ALL_UNITS,
                            L7_RPC_L3_ROUTE_DELETE,
                            &rpcRouteData,
                            rpc_status,
                            app_status, 
                            rpcRouteResp);

  if (result != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Loop through units that successfully completed RPC and return
   ** the err code for each entry. Since RPC is also executed on the
   ** local unit we are guaranteed at least one successful
   ** completion.
  */

  /* At this point treat all entries as success by default. Here, we are
   * gauranteed that we have response from atleast one of the units, 
   * in which case the return code will be updated based on response.
   */
  for (i=0; i < count; i++)
  {
    rv[i] =  BCM_E_NONE;
  }

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    if (rpc_status[i] == HPC_HW_RPC_OK)
    {
      /* Process the response and application status */
      if ((app_status[i] == L7_SUCCESS) && (rpcRouteResp[i].data_len > 0))
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_route_delete: Received %d resp from unit %d\n",
                        rpcRouteResp[i].data_len, i);

        resp = (L7_int32 *)rpcRouteResp[i].buf;
        /* Check response length */
        if (rpcRouteResp[i].data_len == (count * sizeof(L7_int32)))
        {
          for(j=0; j < count; j++)
          {
            if (resp[j] < rv[j])
            {
              /* Note the worst error code per route entry */
              rv[j] = resp[j];
            }
          }
        }
        else
          result = L7_FAILURE; /* Didn't get the expected response */
      }
      else
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_route_delete: No response from unit %d\n", i);
        result = L7_FAILURE;
      }
    }
  }

  return result;
}


/*********************************************************************
* @purpose  Custom RPC function to handle Route deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_route_delete (L7_uint32 transaction_id,
                                       hpcHwRpcData_t *rpc_data,
                                       L7_int32 *status,
                                       hpcHwRpcData_t  *rpc_resp)
{
  usl_bcm_l3_route_t  *pRouteInfo;
  L7_uint32 j;
  L7_uint32 count=0;
  int rv;
  L7_int32 *resp;
  L7_uint32 resp_len = 0;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  count = rpc_data->data_len / sizeof (usl_bcm_l3_route_t);

  if (count == 0) /* Invalid data length in the request */
  {
    /* Indicate that there was error processing the route data. 
     * Note, the response buffer is valid only when incoming route data
     * is processed and we really attempted to delete the routes.
     */
    *status = L7_FAILURE; 
    return L7_SUCCESS;
  }

  pRouteInfo = (usl_bcm_l3_route_t *) rpc_data->buf;
  resp = (L7_int32 *)rpc_resp->buf;
  rpc_resp->data_len = 0;

  L7_RPC_L3_DEBUG("l7_rpc_server_l3_route_delete: Rpc Data %d, Count %d\n",
                  rpc_data->data_len, count);

  for (j = 0; j < count; j++)
  {
    rv = usl_bcm_l3_route_delete(pRouteInfo);

    /* For each route entry, copy the return code in the response */
    resp[j] = rv;
    resp_len += sizeof(rv);
    pRouteInfo++;
  }

  L7_RPC_L3_DEBUG ("l7_rpc_server_l3_route_delete: returning resp %d\n", resp_len);

  rpc_resp->data_len = resp_len;

  /* Indicate that the route processing was success. This means all the routes
   * in the request have been attempted and the response data is valid. 
   * That is, actual err code per route is copied in the response 
   */
  *status = L7_SUCCESS;  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a L3 egress next hop object(s)
*
* @param    flags       @{(input)} Egress object flags
* @param    pBcmxInfo   @{(input)} Egress object attributes
* @param    count       @{(input)} Number of egress objects
* @param    egrIntf     @{(input/output)} Egress object id(s)
* @param    rv          @{(output)} Return code(s) 
* @param    numChips    @{(input)} The number of bcm unit IDs in the list
* @param    chips       @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, rv is invalid
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_egress_create(L7_uint32 *pFlags, usl_bcm_l3_egress_t *pBcmInfo,
                                       L7_uint32 count, bcm_if_t *pEgrIntf, L7_int32 *rv,
                                       L7_uint32 numChips, L7_uint32 *chips)
{
  L7_RC_t result = L7_SUCCESS;
  L7_uint32 i, j;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  l7RpcEgrInfo_t *pRpcEgrInfo;
  l7RpcEgrResp_t *pRpcEgrResp;

  printf("l7_rpc_client_l3_egress_create (%d): pFlags=0x%x, mac %.2x:%.2x:%.2x:%.2x:%.2x:%.2x, intf=%d, port=%d\n", 
		   __LINE__, *pFlags, pBcmInfo->bcm_data.mac_addr[0], pBcmInfo->bcm_data.mac_addr[1], pBcmInfo->bcm_data.mac_addr[2], 
         pBcmInfo->bcm_data.mac_addr[3], pBcmInfo->bcm_data.mac_addr[4], pBcmInfo->bcm_data.mac_addr[5], pBcmInfo->bcm_data.intf, pBcmInfo->bcm_data.port);

  if ((count == 0) || (count > l7_custom_rpc_l3_max_egress_nhop_get()))
  {
    /* No data or too much data */
    return L7_FAILURE;
  }

  memset(rpcEgrNhopData.buf, 0, rpcEgrNhopData.buf_size);
  pRpcEgrInfo = (l7RpcEgrInfo_t *)rpcEgrNhopData.buf;

  /* Build the RPC data that needs to be passed */
  for (i=0; i < count; i++)
  { 
    pRpcEgrInfo->flags = pFlags[i];
    /* Pass egress object Id, as this can be input param */
    pRpcEgrInfo->intf = pEgrIntf[i];

    memcpy(&pRpcEgrInfo->egr, pBcmInfo, sizeof(*pBcmInfo));
    
    pBcmInfo++;
    pRpcEgrInfo++;
  }

  rpcEgrNhopData.data_len = count * sizeof(l7RpcEgrInfo_t);

  /* Clear the response buffer */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(rpcEgrNhopResp[i].buf, 0, rpcEgrNhopResp[i].buf_size);
    rpcEgrNhopResp[i].data_len = 0;
  }

  result = hpcHardwareRpc (L7_ALL_UNITS,
                           L7_RPC_L3_EGRESS_CREATE,
                           &rpcEgrNhopData,
                           rpc_status,
                           app_status,
                           rpcEgrNhopResp);

  if (result != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Loop through units that successfully completed RPC and
  ** return the lowest return code. Since RPC is also executed
  ** on the local unit we are guaranteed at least one successful completion.
  */
  for (i=0; i < count; i++)
  {
    rv[i] =  BCM_E_NONE;
  }

  for (i=0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    if (rpc_status[i] == HPC_HW_RPC_OK)
    {
      /* Process the response and application status */
      if ((app_status[i] == L7_SUCCESS) && (rpcEgrNhopResp[i].data_len > 0))
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_egress_create: Received %d resp from unit %d\n",
                        rpcEgrNhopResp[i].data_len, i);

        pRpcEgrResp = (l7RpcEgrResp_t *)rpcEgrNhopResp[i].buf;

        /* Check response length */
        if (rpcEgrNhopResp[i].data_len == (count * (sizeof(l7RpcEgrResp_t))))
        {
          for(j=0; j < count; j++)
          {
            if (pRpcEgrResp[j].rv == BCM_E_NONE)
            {
              /* All units must return the same egress id */
              pEgrIntf[j] = pRpcEgrResp[j].egrId;
            }
 
            if (pRpcEgrResp[j].rv < rv[j])
            {
              /* Note the worst error code per egress object */
              rv[j] = pRpcEgrResp[j].rv;
            }
          }
        }
        else
          result = L7_FAILURE; /* Didn't get the expected response */
      }
      else
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_egress_create: No response from unit %d\n", i);
        result = L7_FAILURE;
      }
    }
  }
 
  /* Check the overall result of the transaction */
  if (result == L7_FAILURE)
  {
    /* Here if RPC didn't complete on all existing units, or we didn't get
     * expected reponse from existing units. Fail all the entries
     */
    for (i=0; i < count; i++)
    {
      rv[i] =  BCM_E_INTERNAL;
    }
  }

  return result;
}


/*********************************************************************
* @purpose  Custom RPC function to create egress objects
*
* @param    transaction_id - transaction id
* @param    rpc_data - buffer pointer to egress object information
* @param    status - return code if failure
* @param    rpc_resp - response containing error code and egress id
*
* @returns  L7_RC_t
*
* @notes    To synchronize with L7 RPC implementation of host/route, we
*           must use L7 RPC and not BCMX for egress objects. There are 
*           scenarios, like clear config, where L7 RPCs for host/route are
*           skipped, but BCMX calls will not be.
*           Also, L7 RPC has performance benefit when multiple BCM units
*           exist on a single FastPath unit.
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_egress_create (L7_uint32 transaction_id,
                                        hpcHwRpcData_t *rpc_data,
                                        L7_int32 *status,
                                        hpcHwRpcData_t *rpc_resp)
{
  int rv;
  L7_uint32 j, count = 0, resp_len = 0;
  l7RpcEgrInfo_t *pRpcEgrInfo;
  l7RpcEgrResp_t *pRpcEgrResp;
  bcm_if_t       egrId;

  if (rpc_data->data_len == 0) /* No data */
  {
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  count = rpc_data->data_len / sizeof (l7RpcEgrInfo_t);

  if (count == 0)  /* Invalid data len */
  {
    /* Indicate that there was error processing the egress next hop  data.
     * Note, the response buffer is valid only when incoming egress data
     * is processed and we really attempted to create the egress objects.
     */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  L7_RPC_L3_DEBUG("l7_rpc_server_l3_egress_create: Rpc Data %d, Count %d\n", 
                  rpc_data->data_len, count);

  pRpcEgrInfo = (l7RpcEgrInfo_t *)rpc_data->buf;
  pRpcEgrResp = (l7RpcEgrResp_t *)rpc_resp->buf;
  rpc_resp->data_len = 0;


  /* Loop through the egress entries */
  for (j=0; j < count; j++)
  {

    /* EgrId can be input or output parameter */
    egrId = pRpcEgrInfo->intf;

    rv = usl_bcm_l3_egress_create(pRpcEgrInfo->flags, &(pRpcEgrInfo->egr), &egrId);

    /* Return egress object ID and rv. This is required when egress objects
     * are created [not used when current egress obj is modified].
     */
    pRpcEgrResp->rv = rv;
    pRpcEgrResp->egrId = egrId;
    pRpcEgrInfo++;
    pRpcEgrResp++;
    resp_len += sizeof (l7RpcEgrResp_t);
  }

  rpc_resp->data_len = resp_len;

  /* Indicate that all egress objects were processed */
  *status = L7_SUCCESS;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Destroy an L3 egress next hop object(s)
*
* @param    pBcmInfo @{(input)}  BCMX data for each nexthops
* @param    pEgrIntf @{(input)} Egress object id(s)
* @param    count    @{(input)  Number of egress objects to delete
* @param    rv       @{(output) Return code for each delete
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_egress_destroy(usl_bcm_l3_egress_t *pBcmInfo,
                                        bcm_if_t *pEgrIntf, L7_uint32 count, 
                                        L7_int32 *rv, L7_uint32 numChips, 
                                        L7_uint32 *chips)
{
  L7_RC_t result = L7_FAILURE;
  L7_uint32 i, j;
  L7_int32  *pResp;
  l7RpcEgrInfo_t *pRpcEgrInfo;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];

  if ((count == 0) || (count > l7_custom_rpc_l3_max_egress_nhop_get()))
  {
    /* No data or too much data */
    return L7_FAILURE;
  }

  /* Copy egress ids in the RPC data buffer */
  memset(rpcEgrNhopData.buf, 0, rpcEgrNhopData.buf_size);
  pRpcEgrInfo = (l7RpcEgrInfo_t *)rpcEgrNhopData.buf;

  /* Build the RPC data that needs to be passed */
  for (i=0; i < count; i++)
  { 
    pRpcEgrInfo->intf = pEgrIntf[i];
    memcpy(&pRpcEgrInfo->egr, pBcmInfo, sizeof(*pBcmInfo));
    
    pBcmInfo++;
    pRpcEgrInfo++;
  }

  rpcEgrNhopData.data_len = count * sizeof(l7RpcEgrInfo_t);

  /* Clear the response buffer */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(rpcEgrNhopResp[i].buf, 0, rpcEgrNhopResp[i].buf_size);
    rpcEgrNhopResp[i].data_len = 0;
  }


  result = hpcHardwareRpc (L7_ALL_UNITS,
                           L7_RPC_L3_EGRESS_DESTROY,
                           &rpcEgrNhopData,
                           rpc_status,
                           app_status,
                           rpcEgrNhopResp);

  if (result != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Loop through units that successfully completed RPC and
   ** return the lowest return code. Since RPC is also executed
   ** on the local unit we are guaranteed at least one successful completion.
   */
  for (i=0; i < count; i++)
  {
    rv[i] = BCM_E_NONE;
  }

  for (i=0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
   if (rpc_status[i] == HPC_HW_RPC_OK)
    {
      /* Process the response and application status */
      if ((app_status[i] ==  L7_SUCCESS) && (rpcEgrNhopResp[i].data_len > 0))
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_egress_destroy: Received %d resp from unit %d\n",
                        rpcEgrNhopResp[i].data_len, i);

          /* Expecting return code per entry in response */
        if (rpcEgrNhopResp[i].data_len == (count * sizeof(L7_int32)))
        {
          pResp = (L7_int32 *)rpcEgrNhopResp[i].buf;
          for(j=0; j < count; j++)
          {
            if (pResp[j] < rv[j])
            {
              /* Note the worst error code per egress entry */
              rv[j] = pResp[j];
            }
          }
        }
        else
          result = L7_FAILURE; /* Didn't get the expected response */
      }
      else
      {
        L7_RPC_L3_DEBUG("l7_rpc_client_l3_egress_destroy: No response from unit %d\n", i);
        result = L7_FAILURE;
      }
    }
  }

  return result;
}

/*********************************************************************
* @purpose  Custom RPC function to delete egress objects
*
* @param    transaction_id - transaction id
* @param    buf - buffer pointer to egress object information
* @param    buf_len - buffer length
* @param    status - return code 
*
* @returns  none
*
* @notes    Egress destroy must use L7 RPC implementation to synchronize
*           with host/route RPC operations.
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_egress_destroy (L7_uint32 transaction_id,
                                         hpcHwRpcData_t *rpc_data,
                                         L7_int32 *status,
                                         hpcHwRpcData_t  *rpc_resp)
{
  int rv;
  L7_uint32 j;
  L7_uint32 count=0, resp_len=0;
  L7_int32  *pResp;
  l7RpcEgrInfo_t *pRpcEgrInfo;

  if (rpc_data->data_len == 0)
  {
    /* No data in the RPC request. Can't do much */
    *status = L7_FAILURE;
    return L7_SUCCESS;
  }

  count = rpc_data->data_len / sizeof (l7RpcEgrInfo_t);

  if (count == 0) /* Invalid data length in the request */
  {
    /* Indicate that there was error processing the route data. 
     * Note, the response buffer is valid only when incoming route data
     * is processed and we really attempted to delete the routes.
     */
    *status = L7_FAILURE; 
    return L7_SUCCESS;
  }

  pRpcEgrInfo = (l7RpcEgrInfo_t *)rpc_data->buf;
  pResp = (L7_int32 *) rpc_resp->buf;
  rpc_resp->data_len = 0;

  L7_RPC_L3_DEBUG("l7_rpc_server_l3_egress_destroy: Rpc Data %d, Count %d\n",
                  rpc_data->data_len, count);
  
  for (j = 0; j < count; j++)
  {
    rv = usl_bcm_l3_egress_destroy(&(pRpcEgrInfo->egr), pRpcEgrInfo->intf);
    pResp[j] = rv; 
    resp_len += sizeof(rv);

    pRpcEgrInfo++;

  }

  L7_RPC_L3_DEBUG ("l7_rpc_server_l3_egress_destroy: returning resp %d\n", resp_len);
  rpc_resp->data_len = resp_len;

  /* Indicate that all egress objects were processed and response is valid */
  *status = L7_SUCCESS;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Create a multipath/ECMP egress next hop object
*
* @param    flags        @{(input)} Flags = create/replace
* @param    intf_count   @{(input)} Number of egress nhops
* @param    intf_array   @{(input)} Array of egress nhop Ids
* @param    mpintf       @{(input/ouput)} Multipath egress obj id
*
* @returns  Defined by the Broadcom driver
*
* @notes
*
* @end
*********************************************************************/
int l7_rpc_client_l3_egress_multipath_create(L7_uint32 flags, L7_int32 intf_count,
                                             bcm_if_t * intf_array, bcm_if_t *mpintf,
                                             L7_uint32 numChips, L7_uint32 *chips)
{
  int rv = BCM_E_NONE;
  hpcHwRpcData_t rpc_data;
  l7RpcMpathEgrInfo_t rpcMpathEgrInfo;

  if (use_lvl7_rpc == 0)
  {
    /* Call BCMX API instead of L7 RPC implementation */
    rv = bcmx_l3_egress_multipath_create(flags, intf_count, intf_array, mpintf);
  }
  else
  {
    L7_RC_t rc;
    hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
    L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
    L7_uint32 i;

    memset(&rpcMpathEgrInfo, 0, sizeof(rpcMpathEgrInfo));

    rpcMpathEgrInfo.flags = flags;
    rpcMpathEgrInfo.mpintf = *mpintf;  /* Input/Output param */
    rpcMpathEgrInfo.count = intf_count;

    for (i = 0; i < intf_count; i++)
    {
      rpcMpathEgrInfo.intf[i] = intf_array[i];
    }

    rpc_data.buf = (L7_uchar8 *) &rpcMpathEgrInfo;
    rpc_data.buf_size = rpc_data.data_len = sizeof(rpcMpathEgrInfo);
  
    rc = hpcHardwareRpc (L7_ALL_UNITS,
                         L7_RPC_L3_EGRESS_MULTIPATH_CREATE,
                         &rpc_data,
                         rpc_status,
                         app_status,
                         L7_NULL);

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
          if (app_status[i] <= rv)
          {
            rv = app_status[i];
          }
          else
          {
            /* A successful egress create returns identical egrID on every unit */
            *mpintf = app_status[i];
          }
        }
      }
    }
    else
    {
      /* RPC is not ready. This is probably a transient error, so
       * treat it as a timeout.
      */
      rv = BCM_E_TIMEOUT;
    }
  }

  return rv;
}


/*********************************************************************
* @purpose  Custom RPC function to create multipath egress objects
*
* @param    transaction_id - transaction id
* @param    buf - buffer pointer to multipath object information
* @param    buf_len - buffer length
* @param    status - return code if failure, egress id if success.
*
* @returns  none
*
* @notes    To synchronize with L7 RPC implementation of host/route, we
*           must use L7 RPC and not BCMX for multipath objects. There are
*           scenarios, like clear config, where L7 RPCs for host/route are
*           skipped, but BCMX calls will not be.
*           Also, L7 RPC has performance benefit when multiple BCM units
*           exist on a single FastPath unit.
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_egress_multipath_create (L7_uint32 transaction_id,
                                                  hpcHwRpcData_t *rpc_data,
                                                  L7_int32 *status,
                                                  hpcHwRpcData_t *rpc_resp)
{
  int rv;
  bcm_if_t  mpintf;
  l7RpcMpathEgrInfo_t *pRpcMpathEgrInfo;

  pRpcMpathEgrInfo = (l7RpcMpathEgrInfo_t *)rpc_data->buf;

  /* Flags can be input or output parameter */
  mpintf = pRpcMpathEgrInfo->mpintf;

  rv = usl_bcm_l3_egress_multipath_create(pRpcMpathEgrInfo->flags, 
                                          pRpcMpathEgrInfo->count,
                                          pRpcMpathEgrInfo->intf,
                                          &mpintf);

  if (L7_BCMX_OK(rv) == L7_TRUE)
  {
    *status = mpintf;
  }
  else
  {
    *status = rv;
  }
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Destroy a multipath/ECMP egress next hop object
*
* @param    mpintf   @{(input)} Multipath egress object id
*
* @returns  Defined by the Broadcom driver
*
* @notes
*
* @end
*********************************************************************/
int l7_rpc_client_l3_egress_multipath_destroy(bcm_if_t mpintf,
                                              L7_uint32 numChips, 
                                              L7_uint32 *chips)
{
  int rv;
  hpcHwRpcData_t rpc_data;

  if (use_lvl7_rpc == 0)
  {
    rv = bcmx_l3_egress_multipath_destroy(mpintf);
  }
  else
  {
    L7_uint32 i;
    L7_RC_t rc;
    hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
    L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];

    rpc_data.buf =  (L7_uchar8 *) &mpintf;
    rpc_data.buf_size = rpc_data.data_len = sizeof(bcm_if_t);

    rc = hpcHardwareRpc (L7_ALL_UNITS,
                         L7_RPC_L3_EGRESS_MULTIPATH_DESTROY,
                         &rpc_data,
                         rpc_status,
                         app_status,
                         L7_NULL);

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
    }
    else
    {
      /* RPC is not ready. This is probably a transient error, so
       * treat it as a timeout.
      */
      rv = BCM_E_TIMEOUT;
    }
  }

  return rv;
}

/*********************************************************************
* @purpose  Custom RPC function to delete multipath egress objects
*
* @param    transaction_id - transaction id
* @param    buf - buffer pointer to multipath object information
* @param    buf_len - buffer length
* @param    status - return code if failure, egress id if success.
*
* @returns  none
*
* @notes    To synchronize with L7 RPC implementation of host/route, we
*           must use L7 RPC and not BCMX for multipath objects. There are
*           scenarios, like clear config, where L7 RPCs for host/route are
*           skipped, but BCMX calls will not be.
*           Also, L7 RPC has performance benefit when multiple BCM units
*           exist on a single FastPath unit.
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_egress_multipath_destroy (L7_uint32 transaction_id,
                                                   hpcHwRpcData_t *rpc_data,
                                                   L7_int32 *status,
                                                   hpcHwRpcData_t *rpc_resp)
{
  int rv;
  bcm_if_t mpIntf;

  memcpy(&mpIntf, rpc_data->buf, sizeof(bcm_if_t));

  rv = usl_bcm_l3_egress_multipath_destroy(mpIntf);
  
  *status = rv;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a Tunnel Initiator entry to the table
*
* @param    intf       @{(input)} Tunnel interface info
* @param    initiator  @{(input)} Tunnel initiator info
* @param    numChips   @{(input)} The number of bcm unit IDs in the list
* @param    chips      @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_tunnel_initiator_set(bcm_l3_intf_t *intf,
                                       bcm_tunnel_initiator_t *initiator,
                                       L7_uint32 numChips, L7_uint32 *chips)
{
  int rv = BCM_E_NONE;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_RC_t rc;
  struct {
    bcm_l3_intf_t bcm_intf;
    bcm_tunnel_initiator_t bcm_initiator;
  } bcm_data;
  L7_uint32 i;
  hpcHwRpcData_t   rpc_data;

  /* Convert BCMX structures to the BCM structures
  ** (since these are the same type we just do a structure copy
  ** as the compiler will complain is this ever changes).
  */
  bcm_data.bcm_intf = *intf;
  bcm_data.bcm_initiator = *initiator;

  rpc_data.buf = (L7_uchar8 *) &bcm_data;
  rpc_data.buf_size = rpc_data.data_len = sizeof(bcm_data);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_L3_TUNNEL_INITIATOR_SET,
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
  }
  else
  {
    /* RPC is not ready. This is probably a transient error, so treat it as a timeout.
    */
    rv = BCM_E_TIMEOUT;
  }

  
  return rv;
}

/*********************************************************************
* @purpose  Custom RPC function to handle tunnel initiator sets.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_tunnel_initiator_set (L7_uint32 transaction_id,
                                               hpcHwRpcData_t *rpc_data,
                                               L7_int32 *status,
                                               hpcHwRpcData_t  *rpc_resp)
{
  bcm_l3_intf_t *intf;
  bcm_tunnel_initiator_t *initiator;
  int rv;

  intf = (bcm_l3_intf_t *) rpc_data->buf;
  initiator = (bcm_tunnel_initiator_t *) (rpc_data->buf + sizeof(*intf));
 
  rv = usl_bcm_l3_tunnel_initiator_set(intf, initiator);

  *status = rv;  

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a Tunnel Initiator entry from the table
*
* @param    intf       @{(input)} Tunnel interface info
* @param    numChips   @{(input)} The number of bcm unit IDs in the list
* @param    chips      @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_tunnel_initiator_clear(bcm_l3_intf_t *intf,
                                         L7_uint32 numChips, L7_uint32 *chips)
{
  int rv = BCM_E_NONE;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_RC_t rc;
  L7_uint32 i;
  hpcHwRpcData_t    rpc_data;

  
  rpc_data.buf = (L7_uchar8 *) intf;
  rpc_data.buf_size = rpc_data.data_len = sizeof(*intf);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_L3_TUNNEL_INITIATOR_CLEAR,
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
  }
  else
  {
    /* RPC is not ready. This is probably a transient error, so treat it as a timeout.
    */
    rv = BCM_E_TIMEOUT;
  }

  
  return rv;
}

/*********************************************************************
* @purpose  Custom RPC function to handle tunnel initiator clears.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_tunnel_initiator_clear (L7_uint32 transaction_id,
                                                 hpcHwRpcData_t *rpc_data,
                                                 L7_int32 *status,
                                                 hpcHwRpcData_t  *rpc_resp)
{
  bcm_l3_intf_t *intf;
  int rv;

  intf = (bcm_l3_intf_t *) rpc_data->buf;

  rv = usl_bcm_l3_tunnel_initiator_clear(intf);

  *status = rv;  

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a Tunnel Terminator entry to the table
*
* @param    terminator @{(input)} Tunnel terminator info
* @param    numChips   @{(input)} The number of bcm unit IDs in the list
* @param    chips      @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_tunnel_terminator_add(bcm_tunnel_terminator_t *terminator,
                                        L7_uint32 numChips, L7_uint32 *chips)
{
  int rv = BCM_E_NONE;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_RC_t rc;
  L7_uint32 i;
  hpcHwRpcData_t    rpc_data;

  
  rpc_data.buf = (L7_uchar8 *) terminator;
  rpc_data.buf_size = rpc_data.data_len = sizeof(*terminator);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_L3_TUNNEL_TERMINATOR_ADD,
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
  }
  else
  {
    /* RPC is not ready. This is probably a transient error, so treat it as a timeout.
    */
    rv = BCM_E_TIMEOUT;
  }

  
  return rv;
}

/*********************************************************************
* @purpose  Custom RPC function to handle tunnel terminator adds.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_tunnel_terminator_add (L7_uint32 transaction_id,
                                                hpcHwRpcData_t *rpc_data,
                                                L7_int32 *status,
                                                hpcHwRpcData_t  *rpc_resp)

{
  bcm_tunnel_terminator_t *terminator;
  int rv;

  terminator = (bcm_tunnel_terminator_t *) rpc_data->buf;

  rv = usl_bcm_l3_tunnel_terminator_add(terminator);

  *status = rv;  

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a Tunnel Terminator entry from the table
*
* @param    terminator @{(input)} Tunnel terminator info
* @param    numChips   @{(input)} The number of bcm unit IDs in the list
* @param    chips      @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_tunnel_terminator_delete(bcm_tunnel_terminator_t *terminator,
                                           L7_uint32 numChips, L7_uint32 *chips)
{
  int rv = BCM_E_NONE;
  hpcHwRpcStatus_t rpc_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_int32 app_status [L7_MAX_UNITS_PER_STACK + 1];
  L7_RC_t rc;
  L7_uint32 i;
  hpcHwRpcData_t    rpc_data;

  rpc_data.buf = (L7_uchar8 *) terminator;
  rpc_data.buf_size = rpc_data.data_len = sizeof(*terminator);

  rc = hpcHardwareRpc  (L7_ALL_UNITS,
                        L7_RPC_L3_TUNNEL_TERMINATOR_DELETE,
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
  }
  else
  {
    /* RPC is not ready. This is probably a transient error, so treat it as a timeout.
    */
    rv = BCM_E_TIMEOUT;
  }

  return rv;
}


/*********************************************************************
* @purpose  Custom RPC function to handle tunnel terminator deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_server_l3_tunnel_terminator_delete (L7_uint32 transaction_id,
                                                   hpcHwRpcData_t *rpc_data,
                                                   L7_int32 *status,
                                                   hpcHwRpcData_t  *rpc_resp)
{
  bcm_tunnel_terminator_t *terminator;
  int rv;

  terminator = (bcm_tunnel_terminator_t *) rpc_data->buf;

  rv = usl_bcm_l3_tunnel_terminator_delete(terminator);

  *status = rv;
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize custom L3 RPC calls.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_l3_init (void)
{
  L7_RC_t rc;
  L7_uint32 maxEntries = 0;
  L7_uint32 size = 0;
  L7_uint32 i;

  /* Allocate buffer for L3 interface response */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    size = sizeof(bcm_if_t);
    rpcIntfResp[i].buf = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);
    if (rpcIntfResp[i].buf == L7_NULLPTR)
    {
      LOG_ERROR(0);
    }
    memset (rpcIntfResp[i].buf, 0, size);
    rpcIntfResp[i].buf_size = size;
    rpcIntfResp[i].data_len = 0; 
  }

  /* Allocate buffer for storing egress nexthop information */
  /* Note: This buffer is used for both nexthop create and destroy.
   * The BCMX data is different for create and destroy. We could have
   * more deletes in a RPC call as we require just egress id to delete
   */
  maxEntries = l7_custom_rpc_l3_max_egress_nhop_get();
  size = maxEntries * sizeof(l7RpcEgrInfo_t);
  rpcEgrNhopData.buf = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);

  if (rpcEgrNhopData.buf == L7_NULLPTR)
  {
    LOG_ERROR(0);
  }
  
  memset (rpcEgrNhopData.buf, 0, size);
  rpcEgrNhopData.buf_size = size;
  rpcEgrNhopData.data_len = 0;
 
  /* Expecting egress Id + return code in response from each unit */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    size = maxEntries * (sizeof(l7RpcEgrResp_t));
    rpcEgrNhopResp[i].buf = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);
    if (rpcEgrNhopResp[i].buf == L7_NULLPTR)
    {
      LOG_ERROR(0);
    }
    memset (rpcEgrNhopResp[i].buf, 0, size);
    rpcEgrNhopResp[i].buf_size = size;
    rpcEgrNhopResp[i].data_len = 0;
  }

  /* Allocate buffer for storing host information */
  maxEntries =  l7_custom_rpc_l3_max_host_get();
  size = maxEntries * sizeof(usl_bcm_l3_host_t);

  rpcHostData.buf = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);

  if (rpcHostData.buf == L7_NULLPTR)
  {
    LOG_ERROR(0);
  }

  memset (rpcHostData.buf, 0, size);
  rpcHostData.buf_size = size;
  rpcHostData.data_len = 0;

  /* Expecting return codes for each of the host entries from each unit */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    size = maxEntries * sizeof(int);
    rpcHostResp[i].buf = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);
    if (rpcHostResp[i].buf == L7_NULLPTR)
    {
      LOG_ERROR(0);
    }
    memset (rpcHostResp[i].buf, 0, size);
    rpcHostResp[i].buf_size = size;
    rpcHostResp[i].data_len = 0;
  }

  /* Allocate buffer for storing route information */
  maxEntries =  l7_custom_rpc_l3_max_route_get();
  size = maxEntries * sizeof(usl_bcm_l3_route_t);

  rpcRouteData.buf = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);
  
  if (rpcRouteData.buf == L7_NULLPTR)
  {
    LOG_ERROR(0);
  }

  memset (rpcRouteData.buf, 0, size);
  rpcRouteData.buf_size = size;
  rpcRouteData.data_len = 0;
 
  /* Expecting return codes for each of the route entries from each unit */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    size = maxEntries * sizeof(int);
    rpcRouteResp[i].buf = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);
    if (rpcRouteResp[i].buf == L7_NULLPTR)
    {
      LOG_ERROR(0);
    }
    memset (rpcRouteResp[i].buf, 0, size);
    rpcRouteResp[i].buf_size = size;
    rpcRouteResp[i].data_len = 0;
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_HOST_ADD,
                               l7_rpc_server_l3_host_add);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_HOST_DELETE,
                               l7_rpc_server_l3_host_delete);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_ROUTE_ADD,
                               l7_rpc_server_l3_route_add);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_ROUTE_DELETE,
                               l7_rpc_server_l3_route_delete);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_EGRESS_CREATE,
                               l7_rpc_server_l3_egress_create);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_EGRESS_DESTROY,
                               l7_rpc_server_l3_egress_destroy);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_EGRESS_MULTIPATH_CREATE,
                               l7_rpc_server_l3_egress_multipath_create);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_EGRESS_MULTIPATH_DESTROY,
                               l7_rpc_server_l3_egress_multipath_destroy);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }


  rc = hpcHardwareRpcRegister (L7_RPC_L3_TUNNEL_INITIATOR_SET,
                               l7_rpc_server_l3_tunnel_initiator_set);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_TUNNEL_INITIATOR_CLEAR,
                               l7_rpc_server_l3_tunnel_initiator_clear);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_TUNNEL_TERMINATOR_ADD,
                               l7_rpc_server_l3_tunnel_terminator_add);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_TUNNEL_TERMINATOR_DELETE,
                               l7_rpc_server_l3_tunnel_terminator_delete);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  /* Implement L3 interface create/delete using L7 RPC too. This way
   * all L3 BCMX calls use L7 RPC. Intermixing L7 RPC calls and BCMX
   * calls results in synchronization issues with egress objects.
   */
  rc = hpcHardwareRpcRegister (L7_RPC_L3_INTF_CREATE,  
                               l7_rpc_server_l3_intf_create);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  rc = hpcHardwareRpcRegister (L7_RPC_L3_INTF_DELETE,
                               l7_rpc_server_l3_intf_delete);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  return rc;
}


/*********************************************************************
* @purpose  Get the max egress objects that can be handled in a RPC transaction
*
* @param    none
*
* @returns  Number of egress objects per RPC transaction
*
* @end
*********************************************************************/
L7_uint32 l7_custom_rpc_l3_max_egress_nhop_get(void)
{
#ifdef HAPI_BROAD_L3_BULK_SUPPORT
  L7_uint32 maxRpcLen = 0;
  maxRpcLen = hpcHardwareRpcMaxMessageLengthGet();

  if (maxRpcLen == 0)
  {
    LOG_ERROR(0);
  }

  return (maxRpcLen/sizeof(l7RpcEgrInfo_t));
#else
  return 1;
#endif
}


/*********************************************************************
* @purpose  Get the max host entries that can be handled in a RPC transaction
*
* @param    none
*
* @returns  Number of host entries per RPC transaction
*
* @end
*********************************************************************/
L7_uint32 l7_custom_rpc_l3_max_host_get(void)
{
#ifdef HAPI_BROAD_L3_BULK_SUPPORT
  L7_uint32 maxRpcLen = 0;
  maxRpcLen = hpcHardwareRpcMaxMessageLengthGet();

  if (maxRpcLen == 0)
  {
    LOG_ERROR(0);
  }

  return (maxRpcLen/sizeof(usl_bcm_l3_host_t));
#else
  return 1;
#endif
}      


/*********************************************************************
* @purpose  Get the max route entries that can be handled in a RPC transaction
*
* @param    none
*
* @returns  Number of route entries per RPC transaction
*
* @end
*********************************************************************/
L7_uint32 l7_custom_rpc_l3_max_route_get(void)
{
#ifdef HAPI_BROAD_L3_BULK_SUPPORT
  L7_uint32 maxRpcLen = 0;
  maxRpcLen = hpcHardwareRpcMaxMessageLengthGet();

  if (maxRpcLen == 0)
  {
    LOG_ERROR(0);
  }

  return (maxRpcLen/sizeof(usl_bcm_l3_route_t));
#else
  return 1;
#endif
}

#endif /* L7_ROUTING_PACKAGE */
