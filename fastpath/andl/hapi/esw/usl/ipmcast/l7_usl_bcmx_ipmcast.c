/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcmx_ipmc.c
*
* @purpose    USL BCMX API's for IP Multicast
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

#include "l7_usl_bcmx_ipmcast.h"
#include "l7_usl_ipmcast_db.h"
#include "l7_rpc_ipmcast.h"
#include "l7_usl_sm.h"
#include "l7_usl_trace.h"
#include "l7_usl_bcmx_l3.h"

static void  *pUslIpMcastBcmxSema = L7_NULLPTR;

#define USL_IPMC_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_IPMC_ROUTE_DB_ID,"IPM",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslIpMcastBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslIpMcastBcmxSema); \
  } \
}

#define USL_IPMC_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_IPMC_ROUTE_DB_ID,"IPM",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslIpMcastBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslIpMcastBcmxSema); \
  } \
}

/*********************************************************************
* @purpose  Initialize IPMC bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_ipmc_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    pUslIpMcastBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslIpMcastBcmxSema == L7_NULLPTR ) break;

    l7_custom_rpc_ipmcast_init ();

    rc = L7_SUCCESS;
  }
  while(0);

  return rc;
}

/*********************************************************************
* @purpose  Suspend all ipmc bcmx calls
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @end
*********************************************************************/
void usl_ipmc_bcmx_suspend(void)
{
  USL_IPMC_BCMX_LOCK_TAKE();
}

/*********************************************************************
* @purpose  Resume ipmc bcmx calls
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @end
*********************************************************************/
void usl_ipmc_bcmx_resume(void)
{
  USL_IPMC_BCMX_LOCK_GIVE();
}

/*********************************************************************
* @purpose  Add a IP Mcast entry
*
* @param    data     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_add(usl_bcm_ipmc_addr_t *data)
{
  int     rv, index = USL_BCM_IPMC_INVALID_INDEX;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_IPMC_BCMX_LOCK_TAKE();

  do
  {
    /* Allocate hw index if not assigned by HAPI */
    if ((data->flags & BCM_IPMC_USE_IPMC_INDEX) == L7_FALSE)
    {
      hwRv = usl_ipmc_hw_id_allocate(data, &index);
      if (hwRv != BCM_E_NONE)
      {
        break;        
      }
      
      data->ipmc_index = index;
      data->flags |= BCM_IPMC_USE_IPMC_INDEX;
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_ipmc_add(data, 0, L7_NULL);
    }

    /* Handle Hash collisions (BCM_E_FULL) silently as a special case */
    if ((L7_BCMX_OK(hwRv) != L7_TRUE) && (hwRv != BCM_E_EXISTS) && (hwRv != BCM_E_FULL))
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_ipmc_add(data, L7_FALSE);
    if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }


  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  /* Handle Hash collisions (BCM_E_FULL) silently as a special case */
  if (L7_BCMX_OK(rv) != L7_TRUE) 
  {
    if ((rv != BCM_E_EXISTS) && (rv != BCM_E_FULL))
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
              "Failed to configure ipmc address, hwRv %d dbRv %d\n", hwRv, dbRv);
    }

    if ((data->ipmc_index != USL_BCM_IPMC_INVALID_INDEX) &&
        ((data->flags & BCM_IPMC_REPLACE) == L7_FALSE))
    {
      usl_ipmc_hw_id_free(data->ipmc_index);
    }
  }

  USL_IPMC_BCMX_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Delete a IP Mcast entry
*
* @param    data          @{(input)} The ipmc entry
* @param    keep          @{(input)} Keep the entry in Table(1) or remove entry(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_remove(usl_bcm_ipmc_addr_t *data, int keep)
{

  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_IPMC_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_ipmc_remove(data, keep, 0, L7_NULL);
    }

    /* Delete the entry from USL Db even if hw delete failed */
    dbRv = usl_db_ipmc_remove(data);

    if (usl_ipmc_hw_id_free(data->ipmc_index) != BCM_E_NONE)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to free ipmc_index %d\n", data->ipmc_index);    
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to delete ipmc address, hwRv %d dbRv %d\n", hwRv, dbRv);
  }

  USL_IPMC_BCMX_LOCK_GIVE();

  return rv;
}



/*********************************************************************
* @purpose  Modify the port_tgid for the entry
*
* @param    *ipmc         @{(input)} pointer to the IPMC entry.
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_port_tgid_set(usl_bcm_ipmc_addr_t *ipmc)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_IPMC_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_ipmc_port_tgid_set(ipmc, 0, L7_NULL); 
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_ipmc_port_tgid_set(ipmc);
    if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to update port_tgid, hwRv %d dbRv %d\n", hwRv, dbRv);
  }

  USL_IPMC_BCMX_LOCK_GIVE();

  return rv; 
}

/*********************************************************************
* @purpose  Set L2 ports for a IPMC group
*
* @params   ipMcAddress {(input)} IPMC group info
*
* @returns  Defined by the Broadcom driver
*
* @notes    The ports in L2 bitmap are replaces the existing L2 ports 
*           for the group.
* @end
*********************************************************************/
int usl_bcmx_ipmc_set_l2_ports(usl_bcm_ipmc_addr_t *ipmcAddress)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_IPMC_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_ipmc_set_l2_ports(ipmcAddress, 0, L7_NULL);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_ipmc_l2_ports_update(ipmcAddress, USL_CMD_SET);
    if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to set L2 ports for IPMC group, hwRv %d dbRv %d\n", hwRv, dbRv);
  }

  USL_IPMC_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set a port in L2 bitmap for a number of groups
*
* @param   port        {(input)}  BCMX Lport
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call.
* @param   tagged      {(input)} Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_add_l2_port_groups (bcmx_lport_t port,
                                      L7_uint32    *ipmc_index,
                                      L7_uint32    num_groups,
                                      L7_uint32    vlan_id,
                                      L7_uint32    tagged)
{
  int       rv ;
  int       hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  L7_int32  bcmPort, modid;

  USL_IPMC_BCMX_LOCK_TAKE();

  do
  {

    modid   = BCM_GPORT_MODPORT_MODID_GET(port);
    bcmPort = BCM_GPORT_MODPORT_PORT_GET(port);

    if ((modid == HAPI_BROAD_INVALID_MODID) ||
        (bcmPort == HAPI_BROAD_INVALID_MODPORT))
    {
      hwRv = BCM_E_PARAM;
      break;
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_ipmc_add_l2_port_groups(port, ipmc_index, 
                                                   num_groups, vlan_id, 
                                                   tagged);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_ipmc_update_l2_port_groups(modid, bcmPort, 
                                             USL_CMD_ADD, 
                                             ipmc_index, num_groups, 
                                             vlan_id, tagged);
    if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to set lport 0x%x to IPMC groups, hwRv %d dbRv %d\n",
            port, hwRv, dbRv);
  }

  USL_IPMC_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a port from L2 bitmap for a number of groups
*
* @param   port        {(input)}  BCMX Lport
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call. 
* @param   tagged      {(input)}  not used
*
* @returns BCMX Error Code
*
* @notes This function removes L2 ports from the specified groups. 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_delete_l2_port_groups (bcmx_lport_t port,
                                         L7_uint32    *ipmc_index,
                                         L7_uint32    num_groups,
                                         L7_uint32    vlan_id,
                                         L7_uint32    tagged)
                                  
{
  int       rv ;
  int       hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  L7_int32  bcmPort, modid;
  
  USL_IPMC_BCMX_LOCK_TAKE();

  do
  {

    modid   = BCM_GPORT_MODPORT_MODID_GET(port);
    bcmPort = BCM_GPORT_MODPORT_PORT_GET(port);

    if ((modid == HAPI_BROAD_INVALID_MODID) || 
        (bcmPort == HAPI_BROAD_INVALID_MODPORT))
    {
      hwRv = BCM_E_PARAM;
      break;
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_ipmc_delete_l2_port_groups(port, ipmc_index, 
                                                      num_groups, vlan_id);
    }
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_ipmc_update_l2_port_groups(modid, bcmPort, 
                                             USL_CMD_REMOVE, 
                                             ipmc_index, num_groups, 
                                             vlan_id, tagged);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to delete lport 0x%x from IPMC groups, hwRv %d dbRv %d\n", 
            port, hwRv, dbRv);
  }

  USL_IPMC_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set a port in L3 bitmap for a number of groups
*
* @param   port        {(input)}  BCMX Lport
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call.
* @param   tagged      {(input)} Flag indicating whether this VLAN should be tagged.
* @param   mac         {(input)} Mac-address
* @param   ttl         {(input)} ttl
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int
usl_bcmx_ipmc_add_l3_port_groups (bcmx_lport_t port,
                                  L7_uint32    *ipmc_index,
                                  L7_uint32    num_groups,
                                  L7_uint32    vlan_id,
                                  L7_uint32    tagged,
                                  L7_uchar8    *mac,
                                  L7_uint32    ttl)
{
  int       rv ;
  int       hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  L7_int32  bcmPort, modid, i;
  bcm_vlan_vector_t vlanVector;
  
  USL_IPMC_BCMX_LOCK_TAKE();

  do
  {

    modid   = BCM_GPORT_MODPORT_MODID_GET(port);
    bcmPort = BCM_GPORT_MODPORT_PORT_GET(port);

    if ((modid == HAPI_BROAD_INVALID_MODID) ||
        (bcmPort == HAPI_BROAD_INVALID_MODPORT))
    {
      hwRv = BCM_E_PARAM;
      break;
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_ipmc_add_l3_port_groups(port, ipmc_index, num_groups, 
                                                   vlan_id, tagged, mac, ttl);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* If application requests no tagging then do it only if there is one VLAN 
    ** replicated on the port.
    ** If more than one VLAN is replicated on the port then force tagging mode.
    */
    if (tagged == 0)
    {
      for (i = 0; i < num_groups; i++)
      {
        rv = usl_db_ipmc_repl_get(modid, bcmPort, ipmc_index[i], vlanVector);
        /* Ignore this VLAN ID. */
        BCM_VLAN_VEC_CLR(vlanVector, vlan_id);
        if (rv == BCM_E_NONE)
        {
          if (!usl_db_ipmc_vlan_vector_is_empty(vlanVector))
          {
            /* Force tagging.
            */
            tagged = 1;
            break;
          }
        } 
      }
    }

    dbRv = usl_db_ipmc_egress_port_set(modid, bcmPort, vlan_id, !tagged);
    if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }
    dbRv = usl_db_ipmc_update_l3_port_groups(modid, bcmPort, 
                                             USL_CMD_ADD, ipmc_index, 
                                             num_groups, vlan_id);
    if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to add lport 0x%x to IPMC groups, hwRv %d dbRv %d\n", 
            port, hwRv, dbRv);
  }

  USL_IPMC_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Remove a port from L3 bitmap for a number of groups
*
* @param   port        {(input)}  BCMX Lport
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call.
* @param   tagged      {(input)} Flag indicating whether this VLAN should be tagged.
* @param   mac         {(input)} Mac-address (not used)
* @param   ttl         {(input)} ttl (not used)
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int
usl_bcmx_ipmc_delete_l3_port_groups (bcmx_lport_t port,
                                     L7_uint32    *ipmc_index,
                                     L7_uint32    num_groups,
                                     L7_uint32    vlan_id,
                                     L7_uint32    tagged,
                                     L7_uchar8    *mac,
                                     L7_uint32    ttl)
{
  int       rv;
  int       hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  L7_int32  bcmPort, modid;
  
  USL_IPMC_BCMX_LOCK_TAKE();

  do
  {

    modid   = BCM_GPORT_MODPORT_MODID_GET(port);
    bcmPort = BCM_GPORT_MODPORT_PORT_GET(port);

    if ((modid == HAPI_BROAD_INVALID_MODID) ||
        (bcmPort == HAPI_BROAD_INVALID_MODPORT))
    {
      hwRv = BCM_E_PARAM;
      break;
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID))
    {
      hwRv = l7_rpc_client_ipmc_delete_l3_port_groups(port, ipmc_index, 
                                                      num_groups, vlan_id);
    }


    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_ipmc_update_l3_port_groups(modid, bcmPort, 
                                             USL_CMD_REMOVE, 
                                             ipmc_index, num_groups, 
                                             vlan_id);
    if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to remove lport 0x%x from IPMC groups, hwRv %d dbRv %d\n", 
            port, hwRv, dbRv);
  }

  USL_IPMC_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Get whether an IPMC Group is in use
*
* @param    ipmc      @{(input)} pointer to the IPMC entry.
* @param    inuse     @{(output)} L7_TRUE: Group is in use
*                                 L7_FALSE: Group is not in use
*
* @returns  Defined by the Broadcom driver
*
* @notes    Does not takes BCMX Lock to avoid blocking the application
*           when IPMC BCMX is suspended.
* @end
*********************************************************************/
int usl_bcmx_ipmc_inuse_get(usl_bcm_ipmc_addr_t *ipmc, L7_BOOL *inuse)
{
  int     rv = BCM_E_NONE;

  *inuse = L7_FALSE;

#ifdef L7_STACKING_PACKAGE
  /* On Stackables, we query the USL Db to get the group use status */
  *inuse = usl_ipmc_inuse_get(ipmc);

#else

  L7_uint32         flags = 0;

  ipmc->flags |= BCM_IPMC_HIT_CLEAR;

  rv = usl_bcm_ipmc_inuse_get(ipmc, &flags);

  if (L7_BCMX_OK(rv) == L7_TRUE) 
  {
    if (flags & BCM_IPMC_HIT)
      *inuse = L7_TRUE;
  }
  else
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "Failed to get inuse state of entry index %d\n", ipmc->ipmc_index);
  }

#endif


  return rv; 
}

/*********************************************************************
* @purpose  Add a WLAN virtual port to an IP multicast group
*
* @param    index       @{(input)} ipmc hardware table index
* @param    l3a_intf_id @{(input)} L3 if id of vlan routing interface
* @param    vlan_id     @{(input)} vlan routing interface vlan  id
* @param    port        @{(input)} wlan gport
* @param    phys_port   @{(input)} physical port where wlan vp is terminated
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_wlan_l3_port_add(L7_int32 index, L7_int32 l3a_intf_id,
                                   L7_uint32 vlan_id, bcm_port_t port,
                                   bcm_gport_t phys_port)
{
  L7_int32 rv = BCM_E_NONE;
  bcm_if_t egrintf=0;
  L7_int32 egr_rv = 0;
  L7_uint32 flags = 0;
  L7_RC_t rc=L7_FAILURE;
  usl_bcm_l3_egress_t egr;
  L7_int32 self_owned = 1;

  memset(&egr, 0, sizeof(egr));

  USL_IPMC_BCMX_LOCK_TAKE();
  rc = usl_db_ipmc_wlan_l3_find_add(l3a_intf_id, port, vlan_id, &egrintf);
  if (rc != L7_SUCCESS)
  {
    egr.bcm_data.intf = l3a_intf_id;
    egr.bcm_data.port = port;
    egr.bcm_data.vlan = vlan_id;
    rv = usl_bcmx_l3_egress_create(&flags, &egr, 1, &egrintf, &egr_rv);
    if (egr_rv == BCM_E_EXISTS)
    {
      self_owned = 0;
    }
  }

  if (rc != L7_SUCCESS && 
      (rv == BCM_E_NONE || rv == BCM_E_EXISTS) && 
      (egr_rv == BCM_E_NONE || egr_rv == BCM_E_EXISTS))
  {
    rc = usl_db_ipmc_wlan_l3_insert(index, l3a_intf_id, port, vlan_id, egrintf, &egr, self_owned);
    rv = BCM_E_NONE;
  }

  if (rc == L7_SUCCESS && rv == BCM_E_NONE && egrintf != 0)
  {
    rv = l7_rpc_client_ipmc_egress_port_add(phys_port, &index, egrintf);

    if (rv != BCM_E_NONE)
    {
      sysapiPrintf(" bcmx_multicast_egress_add with port %d; rv = %d  mcindex %d \n", 
             port, rv, index);
    }
  }
  else
  {
    sysapiPrintf(" usl_bcmx_l3_egress_create / bcmx_l3_egress_find failed  with port %d; rv = %d; mcindex %d egr_rv %d; egrintf %d\n", 
           port, rv, index, egr_rv, egrintf);

  }

  USL_IPMC_BCMX_LOCK_GIVE();
  return rv;
}

/*********************************************************************
* @purpose  Delete a WLAN virtual port from an IP multicast group
*
* @param    index       @{(input)} ipmc hardware table index
* @param    l3a_intf_id @{(input)} L3 if id of vlan routing interface
* @param    vlan_id     @{(input)} vlan routing interface vlan  id
* @param    port        @{(input)} wlan gport
* @param    phys_port   @{(input)} physical port where wlan vp is terminated
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_wlan_l3_port_delete(L7_int32 index, L7_int32 l3a_intf_id,
                                      L7_uint32 vlan_id, bcm_port_t port,
                                      bcm_gport_t phys_port)
{
  int rv = BCM_E_NONE;
  bcm_if_t egrintf;
  L7_int32 egr_rv;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 use_count = 0;
  L7_int32 self_owned = 0;
  usl_bcm_l3_egress_t egr;

  memset(&egr, 0, sizeof(egr));

  USL_IPMC_BCMX_LOCK_TAKE();
  rc = usl_db_ipmc_wlan_l3_find_delete(l3a_intf_id, port, vlan_id, &egrintf, 
                                       &use_count, &egr, &self_owned);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf(" bcm_l3_egress_find with port %d; rv = %d \n", port, rv);
  }
  else
  {
    rv = l7_rpc_client_ipmc_egress_port_delete(phys_port, &index, egrintf);
    if (rv != BCM_E_NONE)
    {
      sysapiPrintf(" bcm_multicast_egress_delete with port %d; rv = %d \n", port, rv);
    }

    if (use_count == 0)
    {
      if (self_owned == 1)
      {
        rv = usl_bcmx_l3_egress_destroy(&egr, &egrintf, 1, &egr_rv);
        if (rv != BCM_E_NONE)
        {
          sysapiPrintf(" bcm_l3_egress_destroy with port %d; rv = %d \n", port, rv);
        }
      }
      usl_db_ipmc_wlan_l3_delete(l3a_intf_id, port, vlan_id, egrintf);
    }

  }

  USL_IPMC_BCMX_LOCK_GIVE();
  return rv;
}

#endif /* L7_MCAST_PACKAGE */
