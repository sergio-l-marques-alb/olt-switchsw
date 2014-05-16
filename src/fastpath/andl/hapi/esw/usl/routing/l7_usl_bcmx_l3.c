/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcmx_l3.c
*
* @purpose    USL BCMX API's 
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

#include "broad_common.h"
#include "l7_usl_bcmx_l3.h"
#include "l7_usl_l3_db.h"
#include "l7_rpc_l3.h"
#include "l7_usl_sm.h"
#include "l7_usl_trace.h"
#include "broad_l3_debug.h"

static void     *uslIpHostBcmxSema = L7_NULLPTR;
static void     *pUslIpLpmBcmxSema = L7_NULLPTR;
static void     *pUslIpTunnelInitiatorBcmxSema = L7_NULLPTR;
static void     *pUslIpTunnelTerminatorBcmxSema = L7_NULLPTR;
static void     *pUslL3IntfBcmxSema = L7_NULLPTR;
static void     *uslIpEgrNhopBcmxSema = L7_NULLPTR;

#define USL_L3_EGR_NHOP_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_HOST_DB_ID,"EGR/NHOP",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(uslIpEgrNhopBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", uslIpEgrNhopBcmxSema); \
  } \
}

#define USL_L3_EGR_NHOP_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_HOST_DB_ID,"EGR/NHOP",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(uslIpEgrNhopBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", uslIpEgrNhopBcmxSema); \
  } \
}

#define USL_L3_HOST_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_HOST_DB_ID,"ARP/NDP",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(uslIpHostBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", uslIpHostBcmxSema); \
  } \
}

#define USL_L3_HOST_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_HOST_DB_ID,"ARP/NDP",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(uslIpHostBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", uslIpHostBcmxSema); \
  } \
}

#define USL_L3_LPM_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_LPM_DB_ID,"LPM",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslIpLpmBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslIpLpmBcmxSema); \
  } \
}

#define USL_L3_LPM_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_LPM_DB_ID,"LPM",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslIpLpmBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslIpLpmBcmxSema); \
  } \
}

#define USL_L3_TUNNEL_INITIATOR_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_TUNNEL_INITIATOR_DB_ID,"TUNNEL_INITIATOR", \
                 osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslIpTunnelInitiatorBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Sema Take failed, semId %x\n", pUslIpTunnelInitiatorBcmxSema); \
  } \
}

#define USL_L3_TUNNEL_INITIATOR_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_TUNNEL_INITIATOR_DB_ID, \
                 "TUNNEL_INITIATOR",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslIpTunnelInitiatorBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Sema Give failed, semId %x\n", pUslIpTunnelInitiatorBcmxSema); \
  } \
}

#define USL_L3_TUNNEL_TERMINATOR_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_TUNNEL_TERMINATOR_DB_ID,"TUNNEL_TERMINATOR", \
                 osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslIpTunnelTerminatorBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Sema Take failed, semId %x\n", pUslIpTunnelTerminatorBcmxSema); \
  } \
}

#define USL_L3_TUNNEL_TERMINATOR_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_TUNNEL_TERMINATOR_DB_ID,"TUNNEL_TERMINATOR", \
                 osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslIpTunnelTerminatorBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Sema Give failed, semId %x\n", pUslIpTunnelTerminatorBcmxSema); \
  } \
}

#define USL_L3_INTF_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L3_INTF_DB_ID,"L3I",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslL3IntfBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslL3IntfBcmxSema); \
  } \
}

#define USL_L3_INTF_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L3_INTF_DB_ID,"L3I",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslL3IntfBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslL3IntfBcmxSema); \
  } \
}


/*********************************************************************
* @purpose  take next hop db sema
*
* @returns  void
*
* @end
*********************************************************************/
void usl_bcmx_l3_egr_nhop_take(void)
{
  USL_L3_EGR_NHOP_BCMX_LOCK_TAKE();
  return;
}

/*********************************************************************
* @purpose  give next hop db sema
*
* @returns  void
*
* @end
*********************************************************************/
void usl_bcmx_l3_egr_nhop_give(void)
{
  USL_L3_EGR_NHOP_BCMX_LOCK_GIVE();
  return;
}

/*********************************************************************
* @purpose  Initialize L3 bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_l3_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    uslIpEgrNhopBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if (uslIpEgrNhopBcmxSema == L7_NULLPTR) break;


    uslIpHostBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( uslIpHostBcmxSema == L7_NULLPTR ) break;

    pUslIpLpmBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslIpLpmBcmxSema == L7_NULLPTR ) break;
  

    pUslIpTunnelInitiatorBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslIpTunnelInitiatorBcmxSema == L7_NULLPTR ) break;

    pUslIpTunnelTerminatorBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslIpTunnelTerminatorBcmxSema == L7_NULLPTR ) break;

    pUslL3IntfBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslL3IntfBcmxSema == L7_NULLPTR ) break;


    if (l7_rpc_l3_init() != L7_SUCCESS) break;

    rc = L7_SUCCESS;

  } while (0);


  return rc;
}

/*********************************************************************
* @purpose  Enter the critical section of the synchronous databases
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @end
*********************************************************************/
void usl_l3_bcmx_suspend(void)
{
  USL_L3_LPM_BCMX_LOCK_TAKE();
  USL_L3_INTF_BCMX_LOCK_TAKE();
  USL_L3_HOST_BCMX_LOCK_TAKE();
  USL_L3_TUNNEL_INITIATOR_BCMX_LOCK_TAKE();
  USL_L3_TUNNEL_TERMINATOR_BCMX_LOCK_TAKE();
  USL_L3_EGR_NHOP_BCMX_LOCK_TAKE();
}

/*********************************************************************
* @purpose  Exit the critical section of the synchronous databases
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @end
*********************************************************************/
void usl_l3_bcmx_resume(void)
{
  USL_L3_EGR_NHOP_BCMX_LOCK_GIVE();
  USL_L3_TUNNEL_TERMINATOR_BCMX_LOCK_GIVE();
  USL_L3_TUNNEL_INITIATOR_BCMX_LOCK_GIVE();
  USL_L3_HOST_BCMX_LOCK_GIVE();
  USL_L3_INTF_BCMX_LOCK_GIVE();
  USL_L3_LPM_BCMX_LOCK_GIVE();
}


/*********************************************************************
* @purpose  Create an L3 Interface
*
* @param    info          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l3_intf_create(usl_bcm_l3_intf_t *info)
{
  int rv, index = USL_BCM_L3_INTF_INVALID;
  int hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_L3_INTF_BCMX_LOCK_TAKE();

  do
  {
    /* Allocate L3 Id */
    if ((info->bcm_data.l3a_flags & BCM_L3_WITH_ID) == L7_FALSE)
    {
      hwRv = usl_l3_intf_hw_id_allocate(info, &index); 
      if (hwRv != BCM_E_NONE)
      {
        break;     
      }

      info->bcm_data.l3a_intf_id = index;
      info->bcm_data.l3a_flags |= BCM_L3_WITH_ID;
    }
    
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_INTF_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_l3_intf_create(info, 0, L7_NULL);
    }

    /* If hardware add failed then do not update USL Db */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_l3_intf_create(USL_CURRENT_DB, info);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }


  } while ( 0 );

    
  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error condition */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "USL: error creating L3 interface for mac %x:%x:%x:%x:%x:%x vid %d: hwRv=%d dbRv = %d\n",
                info->bcm_data.l3a_mac_addr[0], info->bcm_data.l3a_mac_addr[1],
                info->bcm_data.l3a_mac_addr[2], info->bcm_data.l3a_mac_addr[3],
                info->bcm_data.l3a_mac_addr[4], info->bcm_data.l3a_mac_addr[5],
                info->bcm_data.l3a_vid, hwRv, dbRv);
    if ((info->bcm_data.l3a_intf_id != USL_BCM_L3_INTF_INVALID) && 
        ((info->bcm_data.l3a_flags & BCM_L3_REPLACE) == L7_FALSE))
    {
      usl_l3_intf_hw_id_free(index);    
    }
  }
  
  USL_L3_INTF_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Get info for a L3 intf 
*
* @param    info          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @notes    L3-id is used to search for the matching intf
*
* @end
*********************************************************************/
int usl_bcmx_l3_intf_get(usl_bcm_l3_intf_t *info)
{
  int rv;

  rv = usl_db_l3_intf_get(USL_CURRENT_DB, info);

  return rv;
}

/*********************************************************************
* @purpose  Delete an L3 Interface
*
* @param    info          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l3_intf_delete(usl_bcm_l3_intf_t *info)
{
  int rv;
  int hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_L3_INTF_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_INTF_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_l3_intf_delete(info, 0, L7_NULL);
    }

    /* Cleanup the USL Db even if hw delete failed */
    dbRv = usl_db_l3_intf_delete(USL_CURRENT_DB, info);

    if (usl_l3_intf_hw_id_free(info->bcm_data.l3a_intf_id) != BCM_E_NONE)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to free L3 Intf Id %d\n", info->bcm_data.l3a_intf_id);    
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error condition */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "USL: error deleting L3 interface: hwRv=%d dbRv = %d\n",
                hwRv, dbRv);
  }


  USL_L3_INTF_BCMX_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Get the max egress nhop entries that can be handled by underlying 
*           RPC layer in a single transaction
*
* @param    none
*
* @returns  Number of egress nhop entries in a bulk transaction
*
* @end
*********************************************************************/
L7_uint32 usl_l3_egress_nhop_max_entries_get(void)
{
  return l7_custom_rpc_l3_max_egress_nhop_get();
}

extern void usl_print_l3_egr_nhop_db_elem(void *item, L7_uchar8 *buffer,
                                   L7_uint32 size);


/*********************************************************************
* @purpose  Create an egress next hop object(s)
*
* @param    pFlags    @{(input)}  Flags for each nexthops 
* @param    pBcmInfo @{(input)}  BCMX data for each nexthops
* @param    count     @{(input)}  Number of nexthops passed
* @param    pEgrIntf  @{(output)} Egress Ids of the nexthops
* @param    rv        @{(output)} Return code each of the routes (SDK rv)
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_egress_create(L7_uint32 *pFlags, 
                                  usl_bcm_l3_egress_t *pBcmInfo,
                                  L7_uint32 count,
                                  bcm_if_t *pEgrIntf,
                                  L7_int32 *rv)
{
  L7_RC_t              result = L7_SUCCESS;
  L7_uint32            i;
  usl_bcm_l3_egress_t *tmpBcmInfo;
  bcm_if_t             index;
  usl_egr_nhop_db_elem_t  data;
  L7_uchar8               uslStr[256];

  USL_L3_EGR_NHOP_BCMX_LOCK_TAKE();  

  do
  {
    usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                          "In usl_bcmx_l3_egress_create\n");

    tmpBcmInfo = pBcmInfo;
    for (i=0; i < count; i++)
    {
      index = USL_BCM_L3_INTF_INVALID;

      usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                           "Next-hop Info:\n");
      memcpy(&(data.egrInfo), tmpBcmInfo, sizeof(*tmpBcmInfo));
      usl_print_l3_egr_nhop_db_elem(&data, uslStr, sizeof(uslStr));
      usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                            uslStr);

      /* Allocate Egress nhop Id if not already given */
      if ((pFlags[i] & BCM_L3_WITH_ID) == L7_FALSE)
      {
        rv[i] = usl_l3_egr_nhop_hw_id_allocate(tmpBcmInfo, &index);
        if (rv[i] != BCM_E_NONE)
        {
          USL_LOG_MSG(USL_BCM_E_LOG,
                      "USL: Failed to allocate hw id for egress nhop, rv=%d\n",
                      rv[i]);
          result = L7_FAILURE;
          break;
        }
        osapiSnprintf(uslStr, sizeof(uslStr),
                      "No id given, allocated %d...\n",
                      index);
                      
        usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                              uslStr);
        pEgrIntf[i] = index;
        pFlags[i] |= BCM_L3_WITH_ID;
      }

      tmpBcmInfo++;
    }

    if (result != L7_SUCCESS)
    {
      for (i=0; i < count; i++)
      {
        rv[i] = BCM_E_FAIL;
      }

      break;        
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_EGR_NHOP_DB_ID) == L7_TRUE)
    {
      
      usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                            "Calling RPC Api...\n");
      result = l7_rpc_client_l3_egress_create(pFlags, pBcmInfo, count,
                                              pEgrIntf, rv, 0, L7_NULL);
    }
    else /* Populate the output parameters */
    {
      result = L7_SUCCESS;
      for (i=0; i < count; i++)
      {
        rv[i] = BCM_E_NONE;
      }
    }

    if (result == L7_FAILURE)
      break;

    tmpBcmInfo = pBcmInfo;
    for (i=0; i < count; i++, tmpBcmInfo++)
    {
      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        if (rv[i] != BCM_E_FULL)
        {
          USL_LOG_MSG(USL_BCM_E_LOG,
                      "USL: Error creating/modifying egress object 0x%x in Hw, rv=%d\n",
                      pEgrIntf[i], rv[i]);
        }
        
      }
      else
      {
        usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                              "Updating Db..\n");
        rv[i] = usl_db_l3_egress_create(USL_CURRENT_DB, pEgrIntf[i], tmpBcmInfo);
        if (L7_BCMX_OK(rv[i]) != L7_TRUE)
        {
          USL_LOG_MSG(USL_BCM_E_LOG,
                      "USL: Error creating/modifying egress object 0x%x in Db, rv=%d\n",
                      pEgrIntf[i], rv[i]);
        }
      }

      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        if ((pEgrIntf[i] != USL_BCM_L3_INTF_INVALID) && 
           ((pFlags[i] & BCM_L3_REPLACE) == L7_FALSE))
        {
          
          osapiSnprintf(uslStr, sizeof(uslStr),
                        "Db update failed, freeing id %d\n",
                        pEgrIntf[i]); 
          usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                                uslStr);
          usl_l3_egr_nhop_hw_id_free(pEgrIntf[i]);    
        }
      }
    }
    
  } while (0);

  USL_L3_EGR_NHOP_BCMX_LOCK_GIVE();

  return result;
}

/*********************************************************************
* @purpose  Destroy an egress next hop object(s)
*
* @param    pBcmInfo @{(input)}  BCMX data for each nexthops
* @param    pEgrIntf    @{(input)} Egress object interface id(s)
* @param    count       @{(input)} Number of egress Ids to destroy
* @param    rv          @{(output)} Return code for each entry
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_egress_destroy(usl_bcm_l3_egress_t *pBcmInfo,
                                   bcm_if_t *pEgrIntf, L7_uint32 count, 
                                   L7_int32 *rv)
{
  L7_RC_t                 result = L7_FAILURE;
  L7_uint32               i;
  usl_egr_nhop_db_elem_t  data;
  L7_uchar8               uslStr[256];
  usl_bcm_l3_egress_t    *tmpBcmInfo;

  USL_L3_EGR_NHOP_BCMX_LOCK_TAKE();

  do
  {
    
    usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                          "In usl_bcmx_l3_egress_destroy\n");

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_EGR_NHOP_DB_ID) == L7_TRUE)
    {
      usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                            "Calling RPC Api..\n");
      result = l7_rpc_client_l3_egress_destroy(pBcmInfo, pEgrIntf, count, rv, 0, L7_NULL);
    }
    else /* Populate the output parameters */
    {
      result = L7_FAILURE;
      for (i=0; i < count; i++)
      {
        rv[i] = BCM_E_NONE;      
      }
    }

    tmpBcmInfo = pBcmInfo;

    /* Handle error code for each entry */
    for (i=0; i < count; i++, tmpBcmInfo++)
    {
      /* If Hw delete failed, log the error but cleanup the USL Db */
      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        if (rv[i] != BCM_E_NOT_FOUND)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                  "USL: Couldn't delete egress object %d from Hw, rv=%d\n",
                  pEgrIntf[i], rv[i]);
        }
      }


      usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                           "Next-hop Info:\n");
      memcpy(&(data.egrInfo), tmpBcmInfo, sizeof(*tmpBcmInfo));
      usl_print_l3_egr_nhop_db_elem(&data, uslStr, sizeof(uslStr));
      usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                            uslStr);


      usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                            "Updating DB...\n");
      
      rv[i] = usl_db_l3_egress_destroy(USL_CURRENT_DB, 
                                       tmpBcmInfo, 
                                       pEgrIntf[i]);
      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                "USL: error deleting L3 Egress object %d from Db, rc=%d\n", 
                pEgrIntf[i], result);
        rv[i] = BCM_E_NOT_FOUND;
      }


      osapiSnprintf(uslStr, sizeof(uslStr),
                    "Freeing id %d\n",
                     pEgrIntf[i]); 
      usl_trace_sync_detail(USL_L3_EGR_NHOP_DB_ID, 
                            uslStr);

      if (usl_l3_egr_nhop_hw_id_free(pEgrIntf[i]) != BCM_E_NONE)
      {
        USL_LOG_MSG(USL_BCM_E_LOG,
                    "Failed to delete L3 Egr Nhop 0x%x\n", pEgrIntf[i]);   
      }
      
    }
 
  } while (0);

  USL_L3_EGR_NHOP_BCMX_LOCK_GIVE();

  return result;
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
int usl_bcmx_l3_egress_multipath_create(L7_uint32 flags, L7_int32 intf_count,
                                        bcm_if_t * intf_array, bcm_if_t *mpintf)
{
  int     rv, hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  int     index = USL_BCM_L3_INTF_INVALID;

  USL_L3_EGR_NHOP_BCMX_LOCK_TAKE();  

  do
  {
    /* Allocate Mpath Egress nhop Id if not already given */
    if ((flags & BCM_L3_WITH_ID) == L7_FALSE)
    {
      
      hwRv = usl_l3_mpath_egr_nhop_hw_id_allocate(intf_count, intf_array, &index);
      if (hwRv != BCM_E_NONE)
      {
        break;    
      }

      *mpintf = index;
      flags |= BCM_L3_WITH_ID;
      HAPI_BROAD_L3_BCMX_DBG(BCM_E_NONE, "usl_bcmx_l3_egress_multipath_create allocated id %d\n",
                             *mpintf);

    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_MPATH_EGR_NHOP_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_l3_egress_multipath_create(flags, intf_count, 
                                                      intf_array, mpintf,
                                                      0, L7_NULL);
    }

    /* critical error, log it */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_l3_egress_multipath_create(USL_CURRENT_DB, *mpintf, intf_count, intf_array);

  } while (0);

  rv = min(hwRv, dbRv);

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    if (rv != BCM_E_FULL)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "USL: Error creating/modifying multipath object, Id 0x%x hwRv %d dbRv %d\n",
                  *mpintf, hwRv, dbRv);
    }

    if ((*mpintf != USL_BCM_L3_INTF_INVALID) && 
        ((flags & BCM_L3_REPLACE) == L7_FALSE))
    {
      usl_l3_mpath_egr_nhop_hw_id_free(*mpintf);    
    }
  }

  USL_L3_EGR_NHOP_BCMX_LOCK_GIVE();

  return rv;
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
int usl_bcmx_l3_egress_multipath_destroy(bcm_if_t mpintf)
{
  int     rv, hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_L3_EGR_NHOP_BCMX_LOCK_TAKE();  

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_MPATH_EGR_NHOP_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_l3_egress_multipath_destroy(mpintf, 0, L7_NULL);
    }

    /* check for problems with the delete */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      if ((hwRv != BCM_E_NOT_FOUND) && (hwRv != BCM_E_EMPTY))
      {
        USL_LOG_MSG(USL_BCM_E_LOG,
                    "USL: Couldn't delete the multipath object 0x%x from Hw, rv=%d\n",
                     mpintf, hwRv);
      }
    }

    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_l3_egress_multipath_destroy(USL_CURRENT_DB, mpintf);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
             "USL: error destroying Mpath L3 Egress object %d in Db, rv=%d\n",
              mpintf, dbRv);
    }


   HAPI_BROAD_L3_BCMX_DBG(BCM_E_NONE, "usl_bcmx_l3_egress_multipath_destroy  id %d\n",
                          mpintf);

    if (usl_l3_mpath_egr_nhop_hw_id_free(mpintf) != BCM_E_NONE)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                 "USL: Couldn't free the mpintf id 0x%x\n",
                  mpintf);
    }
      

  } while (0);

  rv = min(hwRv, dbRv);

  USL_L3_EGR_NHOP_BCMX_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Get the max host entries that can be handled by underlying RPC layer
*           in a single transaction
*
* @param    none
*
* @returns  Number of host entries in a bulk transaction
*
* @end
*********************************************************************/
L7_uint32 usl_l3_host_max_entries_get(void)
{
  return l7_custom_rpc_l3_max_host_get();
}

/*********************************************************************
* @purpose  Add a Host entry(or entries)
*
* @param    info     @{(input)}  The address and associated data
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the hosts (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @notes    Forces hit to be true for source and destination in the
*           host data.  Also sets emtry last hit times to current uptime.
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_host_add(usl_bcm_l3_host_t *info, L7_uint32 count, L7_int32 *rv)
{
  L7_RC_t   result = L7_FAILURE;
  L7_uint32 i;
  L7_char8 addr_str[64];


  USL_L3_HOST_BCMX_LOCK_TAKE();

  do
  {

    for (i=0; i < count; i++)
    {
      info[i].l3a_flags |= (BCM_L3_S_HIT | BCM_L3_D_HIT); /* make sure hit set */
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_HOST_DB_ID) == L7_TRUE)
    {
      result = l7_rpc_client_l3_host_add(info, count, rv, 0, L7_NULL);
    }
    else /* Populate the output parameters */
    {
      result = L7_SUCCESS;
      for (i=0; i < count; i++)
      {
        rv[i] = BCM_E_NONE;
      }
    }

    if (result == L7_FAILURE)
      break;

    /* Handle error code for each host entry */
    for (i=0; i < count; i++, info++)
    {
      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        /* critical error, log it */
        /* For ospf point-to-point link, a 32-bit host route conflicts with
         * host entry. SDK has logic to add host as route, which fails to 
         * distinguish between a host entry and 32-bit host route entry. The
         * end-result is BCM_E_EXISTS. As far as traffic is concerned, we need
         * only one of the entries, either host or 32-bit host route.
         */
        if ((rv[i] != BCM_E_FULL) && (rv[i] != BCM_E_EXISTS))
        {
          USL_L3_NTOP(info->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                      (info->l3a_flags & BCM_L3_IP6
                       ? (void *)&info->hostKey.addr.l3a_ip6_addr
                       : (void *)&info->hostKey.addr.l3a_ip_addr),
                       addr_str, sizeof(addr_str));
          USL_LOG_MSG(USL_BCM_E_LOG,
                      "USL: Error adding ARP/NDP entry of IP(%s) to Hw, rv=%d\n",
                      addr_str, rv[i]);
        }
      }
      else
      {
        rv[i] = usl_db_l3_host_add(USL_CURRENT_DB, info);
        if (L7_BCMX_OK(rv[i]) != L7_TRUE)
        {
          
          USL_L3_NTOP(info->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                      (info->l3a_flags & BCM_L3_IP6
                       ? (void *)&info->hostKey.addr.l3a_ip6_addr
                       : (void *)&info->hostKey.addr.l3a_ip_addr),
                       addr_str, sizeof(addr_str));
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                  "USL: Error adding ARP/NDP entry of IP(%s) to Db, rv=%d\n",
                  addr_str, rv[i]);
        }
      }
    }
     
  } while ( 0 );

  USL_L3_HOST_BCMX_LOCK_GIVE();

  return result;
}

/*********************************************************************
* @purpose  Delete a host entry or entries from the table
*
* @param    info     @{(input)}  IP address and associated data
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the host (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_host_delete(usl_bcm_l3_host_t *info, 
                                L7_uint32 count, L7_int32 *rv)
{
  L7_RC_t result = L7_FAILURE;
  L7_uint32 i;
  L7_char8 addr_str[64];

  USL_L3_HOST_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_HOST_DB_ID) == L7_TRUE)
    {
      result = l7_rpc_client_l3_host_delete(info, count, rv, 0, L7_NULL);
    }
    else /* Populate the output parameters */
    {
      result = L7_SUCCESS;
      for (i=0; i < count; i++)
      {
        rv[i] = BCM_E_NONE;
      }
    }

    if (result != L7_SUCCESS)
      break;

    /* check for problems with the delete */
    for (i=0; i < count; i++, info++)
    {
      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        if ((rv[i] != BCM_E_NOT_FOUND) && (rv[i] != BCM_E_EMPTY))
        {
          USL_L3_NTOP(info->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                     (info->l3a_flags & BCM_L3_IP6
                     ? (void *)&info->hostKey.addr.l3a_ip6_addr
                     : (void *)&info->hostKey.addr.l3a_ip_addr),
                     addr_str, sizeof(addr_str));
          USL_LOG_MSG(USL_BCM_E_LOG,
                      "USL: Error deleting the ARP/NDP of IP(%s) from Hw, error code=%d\n",
                      addr_str, rv[i]);
        }
      }
      
      /* Update the USL Db even if hw delete failed */
      rv[i] = usl_db_l3_host_delete(USL_CURRENT_DB, info);
      
      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        USL_L3_NTOP(info->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                    (info->l3a_flags & BCM_L3_IP6
                    ? (void *)&info->hostKey.addr.l3a_ip6_addr
                    : (void *)&info->hostKey.addr.l3a_ip_addr),
                    addr_str, sizeof(addr_str));
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                "USL: Error deleting the ARP/NDP of IP(%s) from Hw, error code=%d\n",
                 addr_str, rv[i]);

      }
    }

  } while ( 0 );

  USL_L3_HOST_BCMX_LOCK_GIVE();

  return result;
}


/*********************************************************************
* @purpose  Get the max route entries that can be handled by underlying RPC layer 
*           in a single transaction
*
* @param    none
*
* @returns  Number of route entries in a bulk transaction
*
* @end
*********************************************************************/
L7_uint32 usl_l3_route_max_entries_get(void)
{
  return l7_custom_rpc_l3_max_route_get();
}


/*********************************************************************
* @purpose  Add route entry (or entries) to the table
*
* @param    info     @{(input)}  The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_route_add(usl_bcm_l3_route_t *info, 
                              L7_uint32 count, L7_int32 *rv)
{
  L7_RC_t    result = L7_FAILURE;
  L7_uint32  i;
  L7_char8   addr_str[64];
  L7_char8   mask_str[64];
  usl_bcm_l3_route_t *tmpInfo;

  USL_L3_LPM_BCMX_LOCK_TAKE();

  do
  {

    tmpInfo = info;
    
    for (i=0; i < count; i++)
    {
      if (uslDebugL3Enable)
      {
        printf("USL_BCMX: Adding route %x/%x\n",
               tmpInfo->routeKey.l3a_subnet.l3a_ip_net,
               tmpInfo->routeKey.l3a_mask.l3a_ip_mask);
      }
      tmpInfo++;
    }

    /* Note: It is OK to invoke 'route add on an existing route with replace
     * flag set. Do not enforce checks for duplicate routes. 
     */

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_LPM_DB_ID) == L7_TRUE)
    {
      if (uslDebugL3Enable)
      {
        printf("USL_BCMX: Calling RPC layer\n");
      }
      result = l7_rpc_client_l3_route_add(info, count, rv, 0,L7_NULL);
    }
    else /* Populate the output parameters*/
    { 
      result = L7_SUCCESS;
      for (i=0; i < count; i++)
      {
        rv[i] = BCM_E_NONE;
      }
    }
    

    if (result == L7_FAILURE)
    {
      break;
    }

    /* Handle error code for each entry */
    for (i=0; i < count; i++, info++)
    {
      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        /* critical error, log it */
        if (rv[i] != BCM_E_FULL)
        {
          USL_L3_NTOP(info->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                     (info->l3a_flags & BCM_L3_IP6
                      ? (void *)&info->routeKey.l3a_subnet.l3a_ip6_net
                      : (void *)&info->routeKey.l3a_subnet.l3a_ip_net),
                      addr_str,
                      sizeof(addr_str));
          USL_L3_NTOP(info->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                      (info->l3a_flags & BCM_L3_IP6
                       ? (void *)&info->routeKey.l3a_mask.l3a_ip6_mask
                       : (void *)&info->routeKey.l3a_mask.l3a_ip_mask),
                      mask_str,
                      sizeof(mask_str));
          USL_LOG_MSG(USL_INFO_LOG,
                      "USL: Error adding route (%s / %s) to Hw, error code=%d\n",
                      addr_str, mask_str, rv[i]);
        }
      }
      else
      {
        if (uslDebugL3Enable)
        {
          printf("USL_BCMX: updating db\n");
        }
        rv[i] = usl_db_l3_route_add(USL_CURRENT_DB, info);
        if (L7_BCMX_OK(rv[i]) != L7_TRUE)
        {

          USL_L3_NTOP(info->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                     (info->l3a_flags & BCM_L3_IP6
                      ? (void *)&info->routeKey.l3a_subnet.l3a_ip6_net
                      : (void *)&info->routeKey.l3a_subnet.l3a_ip_net),
                      addr_str,
                      sizeof(addr_str));
          USL_L3_NTOP(info->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                      (info->l3a_flags & BCM_L3_IP6
                       ? (void *)&info->routeKey.l3a_mask.l3a_ip6_mask
                       : (void *)&info->routeKey.l3a_mask.l3a_ip_mask),
                      mask_str,
                      sizeof(mask_str));
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                  "USL: Error adding route (%s / %s) to Db, error code=%d\n",
                  addr_str, mask_str, rv[i]);
        }
      }
    }
  } while ( 0 );

  USL_L3_LPM_BCMX_LOCK_GIVE();

  return result;
}

/*********************************************************************
* @purpose  Delete an LPM entry or entries from the table
*
* @param    info     @{(input)}  The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_route_delete(usl_bcm_l3_route_t *info, L7_uint32 count, L7_int32 *rv)
{
  L7_RC_t result = L7_FAILURE;
  L7_uint32 i;
  L7_char8 addr_str[64];
  L7_char8 mask_str[64];
  usl_bcm_l3_route_t *tmpInfo;

  USL_L3_LPM_BCMX_LOCK_TAKE(); 

  do
  {
    
    tmpInfo = info;
    
    for (i=0; i < count; i++)
    {
      if (uslDebugL3Enable)
      {
        printf("USL_BCMX: Deleting route %x/%x\n",
               tmpInfo->routeKey.l3a_subnet.l3a_ip_net,
               tmpInfo->routeKey.l3a_mask.l3a_ip_mask);
      }
      tmpInfo++;
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_LPM_DB_ID) == L7_TRUE)
    {
      if (uslDebugL3Enable)
      {
        printf("USL_BCMX: calling RPC...\n");
      }
      result = l7_rpc_client_l3_route_delete(info, count, rv, 0, L7_NULL);
    }
    else
    {
      result = L7_SUCCESS;
      for (i=0; i < count; i++)
      {
        rv[i] = BCM_E_NONE;
      }
    }

    if (result != L7_SUCCESS)
      break;

    /* Handle error code for each entry */
    tmpInfo = info;
    for (i=0; i < count; i++, tmpInfo++)
    {
      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        if ((rv[i] != BCM_E_NOT_FOUND) && (rv[i] != BCM_E_EMPTY))
        {
          USL_L3_NTOP(tmpInfo->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                      (tmpInfo->l3a_flags & BCM_L3_IP6
                      ? (void *)&tmpInfo->routeKey.l3a_subnet.l3a_ip6_net
                      : (void *)&tmpInfo->routeKey.l3a_subnet.l3a_ip_net),
                      addr_str,
                      sizeof(addr_str));
          USL_L3_NTOP(tmpInfo->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                      (tmpInfo->l3a_flags & BCM_L3_IP6
                       ? (void *)&tmpInfo->routeKey.l3a_mask.l3a_ip6_mask
                       : (void *)&tmpInfo->routeKey.l3a_mask.l3a_ip_mask),
                      mask_str,
                      sizeof(mask_str));
          USL_LOG_MSG(USL_BCM_E_LOG,
                      "USL: Error deleting the Route(%s / %s) from Hw, error code=%d\n",
                      addr_str, mask_str, rv[i]);
        }
      }

      
      if (uslDebugL3Enable)
      {
        printf("USL_BCMX: updating db...\n");
      }

      /* Update the USL Db even if hw delete failed */
      rv[i] = usl_db_l3_route_delete(USL_CURRENT_DB, tmpInfo);
      if (L7_BCMX_OK(rv[i]) != L7_TRUE)
      {
        USL_L3_NTOP(tmpInfo->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                    (tmpInfo->l3a_flags & BCM_L3_IP6
                    ? (void *)&tmpInfo->routeKey.l3a_subnet.l3a_ip6_net
                    : (void *)&tmpInfo->routeKey.l3a_subnet.l3a_ip_net),
                    addr_str,
                    sizeof(addr_str));
       USL_L3_NTOP(tmpInfo->l3a_flags & BCM_L3_IP6 ? L7_AF_INET6 : L7_AF_INET,
                   (tmpInfo->l3a_flags & BCM_L3_IP6
                   ? (void *)&tmpInfo->routeKey.l3a_mask.l3a_ip6_mask
                   : (void *)&tmpInfo->routeKey.l3a_mask.l3a_ip_mask),
                   mask_str,
                   sizeof(mask_str));
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                "USL: Error deleting the Route(%s / %s) from Db, error code=%d\n",
                addr_str, mask_str, rv[i]);
      }
     
    }

  } while ( 0 );

  USL_L3_LPM_BCMX_LOCK_GIVE();

  return result;
}

/*********************************************************************
* @purpose  Add a tunnel initiator
*
* @param    intf          @{(input)} The tunnel interface
* @param    initiator     @{(input)} The tunnel initiator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_tunnel_initiator_set(bcm_l3_intf_t *intf,
                                  bcm_tunnel_initiator_t *initiator)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  
  USL_L3_TUNNEL_INITIATOR_BCMX_LOCK_TAKE();  

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_TUNNEL_INITIATOR_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_tunnel_initiator_set(intf, initiator, 0, L7_NULL);
    }

    /* Do not update the USL db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_l3_tunnel_initiator_set(USL_CURRENT_DB, intf, initiator);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
             
  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    if (rv != BCM_E_FULL)
    {
      USL_LOG_MSG(USL_BCM_E_LOG, "USL: Error adding tunnel initiator "
                  "of intf %u, hwRv %d dbRv %d\n", intf->l3a_intf_id, hwRv, dbRv);
    }
  }

  USL_L3_TUNNEL_INITIATOR_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a tunnel initiator
*
* @param    intf        @{(input)} The tunnel interface
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_tunnel_initiator_clear(bcm_l3_intf_t *intf)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_L3_TUNNEL_INITIATOR_BCMX_LOCK_TAKE(); 

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_TUNNEL_INITIATOR_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_tunnel_initiator_clear(intf, 0, L7_NULL);
    }

    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_l3_tunnel_initiator_clear(USL_CURRENT_DB, intf);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
    
  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG,"USL: Couldn't delete the tunnel initiator "
                "for intf %u, hwRv %d dbRv %d\n", intf->l3a_intf_id, hwRv, dbRv);
  }

  USL_L3_TUNNEL_INITIATOR_BCMX_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Add a tunnel terminator
*
* @param    terminator     @{(input)} The tunnel terminator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_tunnel_terminator_add(bcm_tunnel_terminator_t *terminator)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_L3_TUNNEL_TERMINATOR_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_TUNNEL_TERMINATOR_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_tunnel_terminator_add(terminator, 0, L7_NULL);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_l3_tunnel_terminator_add(USL_CURRENT_DB, terminator);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
            
  } while ( 0 );

  rv = min(hwRv, dbRv);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    USL_LOG_MSG(USL_BCM_E_LOG, "USL: Error adding tunnel terminator "
                "(sip=%x,dip=%x), hwRv %d dbRv %d\n",
                terminator->sip, terminator->dip, hwRv, dbRv);
  }
  
  USL_L3_TUNNEL_TERMINATOR_BCMX_LOCK_GIVE();  

  return rv;
}

/*********************************************************************
* @purpose  Delete a tunnel terminator
*
* @param    terminator     @{(input)} The tunnel terminator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_tunnel_terminator_delete(bcm_tunnel_terminator_t *terminator)
{
  int  rv;
  int  hwRv = BCM_E_NONE, dbRv = BCM_E_NONE; 

  USL_L3_TUNNEL_TERMINATOR_BCMX_LOCK_TAKE();  

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L3_TUNNEL_TERMINATOR_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_tunnel_terminator_delete(terminator, 0, L7_NULL);
    }

    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_l3_tunnel_terminator_delete(USL_CURRENT_DB, terminator);
    
  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    USL_LOG_MSG(USL_BCM_E_LOG,"USL: Couldn't delete the tunnel terminator "
                "(sip=%x,dip=%x), hwRv %d dbRv %d\n",
                terminator->sip, terminator->dip, hwRv, dbRv);
  }
    
  USL_L3_TUNNEL_TERMINATOR_BCMX_LOCK_GIVE();

  return rv;
}

#ifdef L7_STACKING_PACKAGE

/*********************************************************************
* @purpose  Get hit (and some other) flags about a host from the
*           USL host entry for that host, also get the last hit times
*           for that host.
*
* @param    family        @{(input)}  The address family
* @param    l3_addr       @{(input)}  Pointer to the host L3 address
* @param    flagsPtr      @{(output)} Pointer where to put flags
* @param    dstTimePtr    @{(output)} Pointer where to put dest last hit time
* @param    srcTimePtr    @{(output)} Pointer where to put src last hit time
* @param    inHw          @{(output)} L7_TRUE if it found the host & filled in the flags
*                                     L7_FALSE if it did not find the host
*
* @returns  BCM_ERROR_CODE
*
* @notes    Does not take BCMX LOCK to avoid blocking the application when
*           L3 BCMX is suspended. 
*           Copies l3a_flags from Broadcom host data.
*           Clears the USL copy of the _HIT flags.
*           Only fills in last hit times if appropriate _HIT flag was set.
*
* @end
*********************************************************************/
int usl_bcmx_l3host_hit_flags_get(const L7_uint32 family,
                                  const L7_uint8  *l3_addr,
                                  L7_uint32 *flagsPtr,
                                  L7_uint32 *dstTimePtr,
                                  L7_uint32 *srcTimePtr,
                                  L7_BOOL   *inHw)
{
  *inHw = usl_l3host_hit_flags_get(family, l3_addr,
                                   flagsPtr, dstTimePtr,
                                   srcTimePtr);
  return BCM_E_NONE; 
}
#endif /* L7_STACKING_PACKAGE */

#endif /* L7_ROUTING_PACKAGE */
