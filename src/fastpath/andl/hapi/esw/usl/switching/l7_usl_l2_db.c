/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_l2_db.c
*
* @purpose    USL L2 DB API implementation 
*
* @component  USL
*
* @comments   none
*
* @create     11/21/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "cnfgr.h"
#include "unitmgr_api.h"
#include "broad_common.h"
#include "l7_usl_sm.h"
#include "l7_usl_l2_db.h"
#include "l7_usl_l2_db_int.h"
#include "l7_usl_port_db.h"
#include "l7_rpc_l2.h"
#include "l7_rpc_l2_mcast.h"
#include "l7_usl_mask.h"
#include "l7_usl_trace.h"
#include "pfc_exports.h"
#include "broad_mmu.h"

#ifdef L7_STACKING_PACKAGE
#include "l7_usl_macsync.h"
#endif

#include "bcmx/bcmx_int.h"
#include "ibde.h"

/* IP Subnet Vlan table resources */
static void                        *pUslVlanIpSubnetDbSema = L7_NULLPTR;
static usl_vlan_ipsubnet_db_elem_t *uslOperVlanIpSubnetDb;
static usl_vlan_ipsubnet_db_elem_t *uslShadowVlanIpSubnetDb;
static usl_vlan_ipsubnet_db_elem_t *uslVlanIpSubnetDbHandle;
static L7_BOOL                      uslVlanIpSubnetDbActive = L7_FALSE;

/* Mac vlan Db*/
static void                      *pUslVlanMacDbSema = L7_NULLPTR;
static usl_vlan_mac_db_elem_t    *uslOperVlanMacDb;
static usl_vlan_mac_db_elem_t    *uslShadowVlanMacDb;
static usl_vlan_mac_db_elem_t    *uslVlanMacDbHandle;
static L7_BOOL                    uslVlanMacDbActive = L7_FALSE;

/* Protected port Db */
static void                          *pUslProtectedGroupDbSema = L7_NULLPTR;
static usl_protected_group_db_elem_t *pUslOperProtectedGroupDb;
static usl_protected_group_db_elem_t *pUslShadowProtectedGroupDb;
static usl_protected_group_db_elem_t *pUslProtectedGroupDbHandle;
static L7_BOOL                        uslProtectedGroupDbActive = L7_FALSE;

/* System Db */
void *pUslSystemDbSema = L7_NULLPTR;
usl_system_db_elem_t  *pUslOperSystemDb;
usl_system_db_elem_t  *pUslShadowSystemDb;
usl_system_db_elem_t  *pUslSystemDbHandle;
L7_BOOL                uslSystemDbActive = L7_FALSE;

static USL_DB_TYPE_t   uslMacSyncDbType;

#define USL_VLAN_IPSUBNET_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_VLAN_IPSUBNET_DB_ID,"IPVLAN",osapiTaskIdSelf(),\
                __LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslVlanIpSubnetDbSema, L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n",  \
                pUslVlanIpSubnetDbSema); \
  } \
}

#define USL_VLAN_IPSUBNET_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_VLAN_IPSUBNET_DB_ID,"IPVLAN",osapiTaskIdSelf(), \
                 __LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslVlanIpSubnetDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", \
                pUslVlanIpSubnetDbSema); \
  } \
}

#define USL_VLAN_MAC_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_VLAN_MAC_DB_ID,"MACVLAN",osapiTaskIdSelf(), \
                 __LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslVlanMacDbSema, L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n",\
                pUslVlanMacDbSema); \
  } \
}

#define USL_VLAN_MAC_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_VLAN_MAC_DB_ID,"MACVLAN",osapiTaskIdSelf(), \
                 __LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslVlanMacDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", \
                pUslVlanMacDbSema); \
  } \
}

#define USL_PROTECTED_GROUP_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_PROTECTED_GRP_DB_ID,"PG",osapiTaskIdSelf(), \
                 __LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslProtectedGroupDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n",\
                pUslProtectedGroupDbSema); \
  } \
}

#define USL_PROTECTED_GROUP_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_PROTECTED_GRP_DB_ID,"PG",osapiTaskIdSelf(),\
                 __LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslProtectedGroupDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", \
                pUslProtectedGroupDbSema); \
  } \
}

extern L7_uint32 uslMacAgeTime;
void usl_mac_sem_show(void); /* needed only for printing the debug info */
int usl_db_l2_age_time_set(USL_DB_TYPE_t dbType, L7_uint32 age_time);

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
usl_vlan_ipsubnet_db_elem_t 
                   *usl_vlan_ipsubnet_db_handle_get(USL_DB_TYPE_t dbType)
{
  usl_vlan_ipsubnet_db_elem_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = uslOperVlanIpSubnetDb;
      break;

    case USL_SHADOW_DB:
      dbHandle = uslShadowVlanIpSubnetDb;
      break;

    case USL_CURRENT_DB:
      dbHandle = uslVlanIpSubnetDbHandle;
      break;

    default:
      break;   
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a vlan-ipsubnet Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_vlan_ipsubnet_db_elem(void)
{
  return sizeof(usl_vlan_ipsubnet_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a vlan-ipsubnet db element in 
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_vlan_ipsubnet_db_elem(void *item, L7_uchar8 *buffer,
                                     L7_uint32 size)
{
  usl_vlan_ipsubnet_db_elem_t *elem = item;

  osapiSnprintf(buffer, size,
                "IP Address %x Subnet%x vlan %d prio %d\n",
                elem->subnetData.ipSubnet,
                elem->subnetData.netMask,
                elem->subnetData.vlanId,
                elem->subnetData.prio); 
}

/*********************************************************************
* @purpose  Search an element in vlan ipsubnet db and return pointer to
*           it.
*
* @params   dbType     {(input)} Db type 
*           flags      {(input)} EXACT or GET_NEXT
*           searchInfo {(input)} Information about element to be searched
*           elemInfo   {(output)} Storage for elem pointer & index
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. DB lock should be held by the caller.
*
* @end
*********************************************************************/
L7_int32 usl_search_vlan_ipsubnet_db_elem(USL_DB_TYPE_t dbType,
                                          L7_uint32 flags, 
                                          uslDbElemInfo_t searchInfo,
                                          uslDbElemInfo_t *elemInfo)
{
  usl_vlan_ipsubnet_db_elem_t  *dbHandle = L7_NULLPTR;
  usl_vlan_ipsubnet_db_elem_t  *vlanIpSubnetElem = searchInfo.dbElem;
  L7_int32                      rv = BCM_E_NONE;
  L7_uint32                     idx;
  L7_uchar8                     traceStr[128];

  if (!uslVlanIpSubnetDbActive)
  {
    return rv;    
  }
  
  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d current index %d\n",
                flags, searchInfo.elemIndex);
  usl_trace_sync_detail(USL_L2_VLAN_IPSUBNET_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_vlan_ipsubnet_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      /* Start from beginning if the last index is not valid */
      if (searchInfo.elemIndex == USL_INVALID_DB_INDEX)
      {
        idx = 0;    
      }
      else
      {
        idx = searchInfo.elemIndex + 1;
      }

      while ((idx < L7_VLAN_IPSUBNET_MAX_VLANS) && (dbHandle[idx].used == L7_FALSE))
        idx++;

      if (idx < L7_VLAN_IPSUBNET_MAX_VLANS) 
      {
        elemInfo->dbElem = &(dbHandle[idx]);
        elemInfo->elemIndex = idx;
        rv = BCM_E_NONE;
      }
      else
      {
        rv = BCM_E_NOT_FOUND;
      }
   }
   else if (flags == USL_DB_EXACT_ELEM)
   {
     for (idx = 0; idx < L7_VLAN_IPSUBNET_MAX_VLANS; idx++)
     {
       if ((dbHandle[idx].used == L7_TRUE) &&
           ((memcmp(&(dbHandle[idx].subnetData.ipSubnet),
                    &(vlanIpSubnetElem->subnetData.ipSubnet),
                    sizeof(dbHandle[idx].subnetData.ipSubnet))) == 0) &&
           ((memcmp(&(dbHandle[idx].subnetData.netMask),
                    &(vlanIpSubnetElem->subnetData.netMask),
                    sizeof(dbHandle[idx].subnetData.netMask))) == 0))

       {
         elemInfo->dbElem = &(dbHandle[idx]);
         elemInfo->elemIndex = idx;
         rv = BCM_E_NONE;
         break;
       }
     }
 
     if (idx >= L7_VLAN_IPSUBNET_MAX_VLANS)
     {
       rv = BCM_E_NOT_FOUND;
     }
   }

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_vlan_ipsubnet_db_elem(USL_DB_TYPE_t     dbType, 
                                       L7_uint32         flags,
                                       uslDbElemInfo_t   searchElem,
                                       uslDbElemInfo_t  *elemInfo)
{
  uslDbElemInfo_t     ipsubnetElem;
  L7_int32            rv = BCM_E_NONE;

  if (!uslVlanIpSubnetDbActive)
  {
    return rv;    
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  ipsubnetElem.dbElem = L7_NULLPTR;
  ipsubnetElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_VLAN_IPSUBNET_DB_LOCK_TAKE();

  do
  {
    /* Search for the elem */
    rv = usl_search_vlan_ipsubnet_db_elem(dbType, flags, 
                                          searchElem, &ipsubnetElem);
    if (rv == BCM_E_NONE)
    {
      /* Element found, copy in the user memory */
      memcpy(elemInfo->dbElem, ipsubnetElem.dbElem, 
             sizeof(usl_vlan_ipsubnet_db_elem_t));
      memcpy(&(elemInfo->elemIndex), &(ipsubnetElem.elemIndex), 
             sizeof(elemInfo->elemIndex)); 
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_VLAN_IPSUBNET_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from Vlan-Ipsubnet Db
*
* @params   dbType   {(input)} 
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_vlan_ipsubnet_db_elem(USL_DB_TYPE_t   dbType, 
                                          uslDbElemInfo_t elemInfo)

{

  L7_int32                     rv = BCM_E_NONE;
  usl_vlan_ipsubnet_db_elem_t *dbHandle = L7_NULLPTR;
  uslDbElemInfo_t              ipsubnetElem;

  if (!uslVlanIpSubnetDbActive)
  {
    return rv;    
  }

  ipsubnetElem.dbElem = L7_NULLPTR;
  ipsubnetElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_VLAN_IPSUBNET_DB_LOCK_TAKE();

  do
  {
    /* If the elemIndex is specified, then use that to delete the element */
    if ((elemInfo.elemIndex != USL_INVALID_DB_INDEX) && 
        (elemInfo.elemIndex < L7_VLAN_IPSUBNET_MAX_VLANS))
    {
      dbHandle = usl_vlan_ipsubnet_db_handle_get(dbType);
      if (dbHandle != L7_NULLPTR)
      {
        memset(&(dbHandle[elemInfo.elemIndex]), 0, 
               sizeof(usl_vlan_ipsubnet_db_elem_t));
      }
      else
      {
        rv = BCM_E_NOT_FOUND;
      }
    }
    else
    {
      if (elemInfo.dbElem == L7_NULLPTR)
      {
        rv = BCM_E_FAIL;
        break;          
      }
      /* Search and delete the elem */
      rv = usl_search_vlan_ipsubnet_db_elem(dbType, USL_DB_EXACT_ELEM, 
                                            elemInfo, &ipsubnetElem);
      if (rv == BCM_E_NONE)
      {
        memset(ipsubnetElem.dbElem, 0, 
               sizeof(usl_vlan_ipsubnet_db_elem_t));    
      }
      else
      {
        /* Element not found */
        rv = BCM_E_NOT_FOUND;
      }
    }

  } while (0);
  
  USL_VLAN_IPSUBNET_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Adds the entry to the uslipsubnetvlan cfg table
*
* @param   dbType  @{(input)} dbType to update
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} Net mask 
* @param   vid     @b{(input)} vlan Id
* @param   prio    @b{(input)} priority field
*
* @returns  BCM error code
*
* @end
*
*********************************************************************/
int usl_db_vlan_ip4_add(USL_DB_TYPE_t dbType, bcm_ip_t ipaddr, 
                        bcm_ip_t netmask,bcm_vlan_t vid, int prio)
{
  int     index;
  L7_BOOL inserted = L7_FALSE;
  int     rv = BCM_E_NONE;

  if (uslVlanIpSubnetDbActive == L7_FALSE)
  {
    return rv;      
  }

  USL_VLAN_IPSUBNET_DB_LOCK_TAKE();

  for (index = 0; index < L7_VLAN_IPSUBNET_MAX_VLANS; index++)
  {
    if (!uslVlanIpSubnetDbHandle[index].used)
    {
      uslVlanIpSubnetDbHandle[index].subnetData.ipSubnet = ipaddr;
      uslVlanIpSubnetDbHandle[index].subnetData.netMask  = netmask;
      uslVlanIpSubnetDbHandle[index].subnetData.vlanId   = vid;
      uslVlanIpSubnetDbHandle[index].subnetData.prio     = prio;
      uslVlanIpSubnetDbHandle[index].used     = L7_TRUE;
      inserted = L7_TRUE;
      break;
    }
  }

  /* No free entry found */
  if (inserted == L7_FALSE)
  {
    rv = BCM_E_FULL;
  }
  
  USL_VLAN_IPSUBNET_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Deletes the entry from the uslipsubnetvlan cfg table
*
* @param   dbType  @{(input)} dbType to update
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} Net mask 
*
* @returns  BCM error code
*
* @end
*
*********************************************************************/
int usl_db_vlan_ip4_delete(USL_DB_TYPE_t dbType, bcm_ip_t ipaddr, 
                           bcm_ip_t netmask)
{
  int                         rv = BCM_E_NONE;
  uslDbElemInfo_t             elemInfo, searchInfo;
  usl_vlan_ipsubnet_db_elem_t ipsubnetElem;

  if (uslVlanIpSubnetDbActive == L7_FALSE)
  {
    return rv;      
  }

  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  memset(&ipsubnetElem, 0, sizeof(ipsubnetElem));
  memcpy(&(ipsubnetElem.subnetData.ipSubnet), &ipaddr, sizeof(ipaddr));
  memcpy(&(ipsubnetElem.subnetData.netMask), &netmask, sizeof(netmask));

  searchInfo.dbElem = &ipsubnetElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_VLAN_IPSUBNET_DB_LOCK_TAKE();
  
  rv = usl_search_vlan_ipsubnet_db_elem(dbType, USL_DB_EXACT_ELEM,
                                        searchInfo, &elemInfo); 
  if (rv == BCM_E_NONE)
  {
    /* clear the entry to indicate it is no longer used */
    memset(elemInfo.dbElem, 0, sizeof(usl_vlan_ipsubnet_db_elem_t));
  }
  
 
  USL_VLAN_IPSUBNET_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create all vlan-ipsubnet attributes at Usl Bcm layer from 
*           the given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_vlan_ipsubnet_db_elem_bcm(void *item)
{
  usl_vlan_ipsubnet_db_elem_t *elem = item;
  L7_int32            rv;

  do
  {
    rv = usl_bcm_vlan_ip4_add(&(elem->subnetData));
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      break;  
    }
    
  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete all the vlan Ipsubnet attributes at the Usl Bcm layer 
*           from the given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_vlan_ipsubnet_db_elem_bcm(void *item)
{
  usl_vlan_ipsubnet_db_elem_t *elem = item;
  L7_int32            rv;

  rv = usl_bcm_vlan_ip4_delete(&(elem->subnetData));

  return rv;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_vlan_ipsubnet_db_elem_bcm(void *shadowDbItem, 
                                              void *operDbItem)
{
  
  L7_int32                      rv = BCM_E_NONE;
  usl_vlan_ipsubnet_db_elem_t  *shadowElem =  shadowDbItem;
  usl_vlan_ipsubnet_db_elem_t  *operElem = operDbItem;

  do
  {
    if ((memcmp(&(shadowElem->subnetData),
                &(operElem->subnetData),
                sizeof(shadowElem->subnetData))) != 0) 
    {
      rv = usl_bcm_vlan_ip4_add(&(shadowElem->subnetData));
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;  
      }
      
    }
  } while (0);

  return rv;

}

static L7_RC_t 
       usl_vlan_ipsubnet_db_alloc(usl_vlan_ipsubnet_db_elem_t **ipSubnetDb)
{
  *ipSubnetDb = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                            L7_VLAN_IPSUBNET_MAX_VLANS * sizeof(usl_vlan_ipsubnet_db_elem_t));
  if (*ipSubnetDb == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(*ipSubnetDb, 0,  
         L7_VLAN_IPSUBNET_MAX_VLANS * sizeof(usl_vlan_ipsubnet_db_elem_t));

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Initialize Usl IpSubnet vlan resources
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_ipsubnet_db_init()
{
  L7_RC_t          rc = L7_ERROR;
  uslDbSyncFuncs_t vlanIpSubnetDbFuncs;

  memset(&vlanIpSubnetDbFuncs, 0, sizeof(vlanIpSubnetDbFuncs));

  do
  {
    /* Allocate db only on stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
      pUslVlanIpSubnetDbSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY);
      if (pUslVlanIpSubnetDbSema == L7_NULLPTR)
        break;

      if (usl_vlan_ipsubnet_db_alloc(&uslOperVlanIpSubnetDb) != L7_SUCCESS)
      {
        break;    
      }

      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_vlan_ipsubnet_db_alloc(&uslShadowVlanIpSubnetDb) != L7_SUCCESS)
        {
          break;    
        }
      }

      /* Register the sync routines */
      vlanIpSubnetDbFuncs.get_size_of_db_elem =  usl_get_size_of_vlan_ipsubnet_db_elem;
      vlanIpSubnetDbFuncs.get_db_elem = usl_get_vlan_ipsubnet_db_elem;
      vlanIpSubnetDbFuncs.delete_elem_from_db =  usl_delete_vlan_ipsubnet_db_elem;
      vlanIpSubnetDbFuncs.print_db_elem = usl_print_vlan_ipsubnet_db_elem;
      vlanIpSubnetDbFuncs.create_usl_bcm = usl_create_vlan_ipsubnet_db_elem_bcm;
      vlanIpSubnetDbFuncs.delete_usl_bcm = usl_delete_vlan_ipsubnet_db_elem_bcm;
      vlanIpSubnetDbFuncs.update_usl_bcm = usl_update_vlan_ipsubnet_db_elem_bcm;

     
      /* Not registering optional routines as the db element is deterministic 
      ** i.e no pointer member elements
      */
      vlanIpSubnetDbFuncs.alloc_db_elem = L7_NULLPTR;
      vlanIpSubnetDbFuncs.copy_db_elem  = L7_NULLPTR; 
      vlanIpSubnetDbFuncs.free_db_elem  = L7_NULLPTR;
      vlanIpSubnetDbFuncs.pack_db_elem  = L7_NULLPTR;
      vlanIpSubnetDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L2_VLAN_IPSUBNET_DB_ID, 
                                          &vlanIpSubnetDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);   
      }

      uslVlanIpSubnetDbActive = L7_TRUE;
      uslVlanIpSubnetDbHandle = uslOperVlanIpSubnetDb;
    }
  
    rc = L7_SUCCESS;
  }
  while (0);

  return rc;  
}

/*********************************************************************
* @purpose  Fini Usl IpSubnet vlan resources
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_ipsubnet_db_fini()
{
  L7_RC_t rc = L7_SUCCESS;

  uslVlanIpSubnetDbActive = L7_FALSE;
  uslVlanIpSubnetDbHandle = L7_NULLPTR;

  if (pUslVlanIpSubnetDbSema != L7_NULLPTR)
  {
    osapiSemaDelete(pUslVlanIpSubnetDbSema);
    pUslVlanIpSubnetDbSema = L7_NULLPTR;
  }

  if (uslOperVlanIpSubnetDb != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperVlanIpSubnetDb);
    uslOperVlanIpSubnetDb = L7_NULLPTR;
  }

  if (uslShadowVlanIpSubnetDb != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowVlanIpSubnetDb);
    uslShadowVlanIpSubnetDb = L7_NULLPTR;
  }

  return rc;  
}

/*********************************************************************
* @purpose  Invalidates all the entries of uslvlanipsubnet table as unused
*
* @param    none
*
* @returns  L7_SUCCESS
*
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_ipsubnet_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t result = L7_SUCCESS;

  if(!uslVlanIpSubnetDbActive)
    return result;

  USL_VLAN_IPSUBNET_DB_LOCK_TAKE();

  if (flags & USL_OPERATIONAL_DB)
  {
    memset(uslOperVlanIpSubnetDb, 0, 
           L7_VLAN_IPSUBNET_MAX_VLANS * sizeof(usl_vlan_ipsubnet_db_elem_t));
  }

  if (flags & USL_SHADOW_DB)
  {
    memset(uslShadowVlanIpSubnetDb, 0, 
           L7_VLAN_IPSUBNET_MAX_VLANS * sizeof(usl_vlan_ipsubnet_db_elem_t));
  }


  USL_VLAN_IPSUBNET_DB_LOCK_GIVE();

  return (result);
}

/*********************************************************************
* @purpose  Set the Vlan IPSubnet Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_vlan_ipsubnet_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_VLAN_IPSUBNET_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    uslVlanIpSubnetDbHandle = uslOperVlanIpSubnetDb;  
  }
  else if (dbType == USL_SHADOW_DB)
  {
    uslVlanIpSubnetDbHandle = uslShadowVlanIpSubnetDb;  
  }

  USL_VLAN_IPSUBNET_DB_LOCK_GIVE();

  return;
}

/*********************************************************************
* @purpose  Shows the entries of vlan ipsubnet db
*
* @param   dbType {(input)} dbType to dump
*
* @returns none
*
* @end
*
*********************************************************************/
void usl_vlan_ipsubnet_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                     uslStr[512];
  usl_vlan_ipsubnet_db_elem_t   elem, nextElem, *dbHandle;
  uslDbElemInfo_t               searchElem, elemInfo;
  L7_uint32                     entryCount = 0;


  if (uslVlanIpSubnetDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Vlan IpSubnet database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Vlan IpSubnet database is active\n");
  }

  dbHandle = usl_vlan_ipsubnet_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n", 
                   usl_db_type_name_get(dbType));
  }

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_vlan_ipsubnet_db_elem(dbType, USL_DB_NEXT_ELEM, 
                                       searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_vlan_ipsubnet_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                  uslStr);
    searchElem.elemIndex = elemInfo.elemIndex;
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                "Total number of entries in the table: %d\n",
                entryCount);

  return;
}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
usl_vlan_mac_db_elem_t 
                   *usl_vlan_mac_db_handle_get(USL_DB_TYPE_t dbType)
{
  usl_vlan_mac_db_elem_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = uslOperVlanMacDb;
      break;

    case USL_SHADOW_DB:
      dbHandle = uslShadowVlanMacDb;
      break;

    case USL_CURRENT_DB:
      dbHandle = uslVlanMacDbHandle;
      break;

    default:
      break;   
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a vlan-mac Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_vlan_mac_db_elem(void)
{
  return sizeof(usl_vlan_mac_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a vlan-mac db element in 
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_vlan_mac_db_elem(void *item, L7_uchar8 *buffer,
                                L7_uint32 size)
{
  usl_vlan_mac_db_elem_t *elem = (usl_vlan_mac_db_elem_t *) item;

  osapiSnprintf(buffer, size,
                "Mac Address %x:%x:%x:%x:%x:%x vlan %d prio %d\n",
                elem->macData.mac[0], elem->macData.mac[1],
                elem->macData.mac[2], elem->macData.mac[3],
                elem->macData.mac[4], elem->macData.mac[5],
                elem->macData.vlanId,
                elem->macData.prio); 
}

/*********************************************************************
* @purpose  Search an element in db and return pointer to it.
*
* @params   dbType     {(input)} Db type 
*           flags      {(input)} EXACT or GET_NEXT
*           searchInfo {(input)} Information about element to be searched
*           elemInfo   {(output)} Storage for elem pointer & index
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. DB lock should be held by the caller.
*
* @end
*********************************************************************/
L7_int32 usl_search_vlan_mac_db_elem(USL_DB_TYPE_t dbType,
                                     L7_uint32 flags, 
                                     uslDbElemInfo_t searchInfo,
                                     uslDbElemInfo_t *elemInfo)
{
  usl_vlan_mac_db_elem_t  *dbHandle = L7_NULLPTR;
  usl_vlan_mac_db_elem_t  *vlanMacElem = searchInfo.dbElem;
  L7_int32                 rv = BCM_E_NONE;
  L7_uint32                idx;
  L7_uchar8                traceStr[128];

  if (!uslVlanMacDbActive)
  {
    return rv;    
  }
  
  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d current index %d\n",
                flags, searchInfo.elemIndex);
  usl_trace_sync_detail(USL_L2_VLAN_MAC_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_vlan_mac_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      /* Start from beginning if the last index is not valid */
      if (searchInfo.elemIndex == USL_INVALID_DB_INDEX)
      {
        idx = 0;    
      }
      else
      {
        idx = searchInfo.elemIndex + 1;
      }

      while ((idx < L7_VLAN_MAC_MAX_VLANS) && (dbHandle[idx].used == L7_FALSE))
        idx++;

      if (idx < L7_VLAN_MAC_MAX_VLANS) 
      {
        elemInfo->dbElem = &(dbHandle[idx]);
        elemInfo->elemIndex = idx;
        rv = BCM_E_NONE;
      }
      else
      {
        rv = BCM_E_NOT_FOUND;
      }
   }
   else if (flags == USL_DB_EXACT_ELEM)
   {
     for (idx = 0; idx < L7_VLAN_MAC_MAX_VLANS; idx++)
     {
       if ((dbHandle[idx].used == L7_TRUE) &&
           ((memcmp(dbHandle[idx].macData.mac,
                    vlanMacElem->macData.mac,
                    sizeof(bcm_mac_t))) == 0))
       {
         elemInfo->dbElem = &(dbHandle[idx]);
         elemInfo->elemIndex = idx;
         rv = BCM_E_NONE;
         break;
       }
     }
 
     if (idx >= L7_VLAN_MAC_MAX_VLANS)
     {
       rv = BCM_E_NOT_FOUND;
     }
   }

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_vlan_mac_db_elem(USL_DB_TYPE_t     dbType, 
                                  L7_uint32         flags,
                                  uslDbElemInfo_t   searchElem,
                                  uslDbElemInfo_t  *elemInfo)
{
  uslDbElemInfo_t     macElem;
  L7_int32            rv = BCM_E_NONE;

  if (!uslVlanMacDbActive)
  {
    return rv;    
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  macElem.dbElem = L7_NULLPTR;
  macElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_VLAN_MAC_DB_LOCK_TAKE();

  do
  {
    /* Search for the elem */
    rv = usl_search_vlan_mac_db_elem(dbType, flags, 
                                     searchElem, &macElem);
    if (rv == BCM_E_NONE)
    {
      /* Element found, copy in the user memory */
      memcpy(elemInfo->dbElem, macElem.dbElem, 
             sizeof(usl_vlan_mac_db_elem_t));
      memcpy(&(elemInfo->elemIndex), &(macElem.elemIndex), 
             sizeof(elemInfo->elemIndex)); 
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_VLAN_MAC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from Vlan-Mac Db
*
* @params   dbType   {(input)} 
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_vlan_mac_db_elem(USL_DB_TYPE_t   dbType, 
                                     uslDbElemInfo_t elemInfo)

{

  L7_int32                     rv = BCM_E_NONE;
  usl_vlan_mac_db_elem_t      *dbHandle = L7_NULLPTR;
  uslDbElemInfo_t              macElem;

  if (!uslVlanMacDbActive)
  {
    return rv;    
  }

  macElem.dbElem = L7_NULLPTR;
  macElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_VLAN_MAC_DB_LOCK_TAKE();

  do
  {
    /* If the elemIndex is specified, then use that to delete the element */
    if ((elemInfo.elemIndex != USL_INVALID_DB_INDEX) && 
        (elemInfo.elemIndex < L7_VLAN_MAC_MAX_VLANS))
    {
      dbHandle = usl_vlan_mac_db_handle_get(dbType);
      if (dbHandle != L7_NULLPTR)
      {
        memset(&(dbHandle[elemInfo.elemIndex]), 0, 
               sizeof(usl_vlan_mac_db_elem_t));
      }
      else
      {
        rv = BCM_E_NOT_FOUND;
      }
    }
    else
    {
      if (elemInfo.dbElem == L7_NULLPTR)
      {
        rv = BCM_E_FAIL;
        break;          
      }
      /* Search and delete the elem */
      rv = usl_search_vlan_mac_db_elem(dbType, USL_DB_EXACT_ELEM, 
                                       elemInfo, &macElem);
      if (rv == BCM_E_NONE)
      {
        memset(macElem.dbElem, 0, 
               sizeof(usl_vlan_mac_db_elem_t));    
      }
      else
      {
        /* Element not found */
        rv = BCM_E_NOT_FOUND;
      }
    }

  } while (0);
  
  USL_VLAN_MAC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Adds the entry to the USL db 
*
* @param   dbType  @{(input)} db type to update
* @param   mac     @b{(input)} Mac Address
* @param   vid     @b{(input)} vlan Id 
* @param   prio    @b{(input)} priority field 
*
* @returns BCM error code
*
* @end
*
*********************************************************************/
int usl_db_vlan_mac_add(USL_DB_TYPE_t dbType, bcm_mac_t mac, 
                        bcm_vlan_t vid, int prio)
{
  int     index;
  L7_BOOL inserted = L7_FALSE;
  int     rv = BCM_E_NONE;

  if (uslVlanMacDbActive == L7_FALSE)
  {
    return rv;    
  }

  USL_VLAN_MAC_DB_LOCK_TAKE();

  for (index = 0; index < L7_VLAN_MAC_MAX_VLANS; index++)
  {
    if (uslVlanMacDbHandle[index].used == L7_NULL)
    {
      memcpy(uslVlanMacDbHandle[index].macData.mac, mac, sizeof(bcm_mac_t));
      uslVlanMacDbHandle[index].macData.vlanId = vid;
      uslVlanMacDbHandle[index].macData.prio = prio;
      uslVlanMacDbHandle[index].used = 1;
      inserted = L7_TRUE;  
      break;
    }
  }
  
  if (inserted == L7_FALSE)
  {
    rv = BCM_E_FULL;
  }

  USL_VLAN_MAC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Deletes the entry from the usl table 
*
* @param   dbType  @{(input)} db type to update
* @param   mac     @b{(input)} Mac Address
*
* @returns  BCM error code
*
* @end
*
*********************************************************************/
int usl_db_vlan_mac_delete(USL_DB_TYPE_t dbType, bcm_mac_t mac)
{
  int                         rv = BCM_E_NONE;
  uslDbElemInfo_t             elemInfo, searchInfo;
  usl_vlan_mac_db_elem_t      macElem;

  if (uslVlanMacDbActive == L7_FALSE)
  {
    return rv;      
  }

  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  memset(&macElem, 0, sizeof(macElem));
  memcpy(macElem.macData.mac, mac, sizeof(bcm_mac_t));

  searchInfo.dbElem = &macElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_VLAN_MAC_DB_LOCK_TAKE();
  
  rv = usl_search_vlan_mac_db_elem(dbType, USL_DB_EXACT_ELEM,
                                   searchInfo, &elemInfo); 
  if (rv == BCM_E_NONE)
  {
    /* clear the entry to indicate it is no longer used */
    memset(elemInfo.dbElem, 0, sizeof(usl_vlan_mac_db_elem_t));
  }
  
 
  USL_VLAN_MAC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create all vlan-mac attributes at Usl Bcm layer from 
*           the given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_vlan_mac_db_elem_bcm(void *item)
{
  usl_vlan_mac_db_elem_t *elem = item;
  L7_int32                rv;

  do
  {
    rv = usl_bcm_vlan_mac_add(&(elem->macData));
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      break;  
    }
    
  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete all the vlan mac attributes at the Usl Bcm layer 
*           from the given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_vlan_mac_db_elem_bcm(void *item)
{
  usl_vlan_mac_db_elem_t *elem = item;
  L7_int32            rv;

  rv = usl_bcm_vlan_mac_delete(&(elem->macData));

  return rv;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_vlan_mac_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  
  L7_int32                  rv = BCM_E_NONE;
  usl_vlan_mac_db_elem_t   *shadowElem =  shadowDbItem;
  usl_vlan_mac_db_elem_t   *operElem = operDbItem;

  do
  {
    if ((memcmp(&(shadowElem->macData),
                &(operElem->macData),
                sizeof(shadowElem->macData))) != 0) 
    {
      rv = usl_bcm_vlan_mac_add(&(shadowElem->macData));
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;  
      }
      
    }
  } while (0);

  return rv;

}

static L7_RC_t usl_vlan_mac_db_alloc(usl_vlan_mac_db_elem_t **macDb)
{
  *macDb = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                       L7_VLAN_MAC_MAX_VLANS * sizeof(usl_vlan_mac_db_elem_t));
  if (*macDb == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(*macDb, 0, 
         L7_VLAN_MAC_MAX_VLANS * sizeof(usl_vlan_mac_db_elem_t));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize Usl IpSubnet vlan resources
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_mac_db_init()
{
  L7_RC_t           rc = L7_ERROR;
  uslDbSyncFuncs_t  vlanMacDbFuncs;

  memset(&vlanMacDbFuncs, 0, sizeof(vlanMacDbFuncs));

  do
  {
    /* Allocate db only on stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {

      pUslVlanMacDbSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY);
      if (pUslVlanMacDbSema == L7_NULLPTR)
        break;

      if (usl_vlan_mac_db_alloc(&uslOperVlanMacDb) != L7_SUCCESS)
      {
        break;    
      }

      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_vlan_mac_db_alloc(&uslShadowVlanMacDb) != L7_SUCCESS)
        {
          break;    
        }
      }

      /* Register the sync routines */
      vlanMacDbFuncs.get_size_of_db_elem =  usl_get_size_of_vlan_mac_db_elem;
      vlanMacDbFuncs.get_db_elem = usl_get_vlan_mac_db_elem;
      vlanMacDbFuncs.delete_elem_from_db =  usl_delete_vlan_mac_db_elem;
      vlanMacDbFuncs.print_db_elem = usl_print_vlan_mac_db_elem;
      vlanMacDbFuncs.create_usl_bcm = usl_create_vlan_mac_db_elem_bcm;
      vlanMacDbFuncs.delete_usl_bcm = usl_delete_vlan_mac_db_elem_bcm;
      vlanMacDbFuncs.update_usl_bcm = usl_update_vlan_mac_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic 
      ** i.e no pointer member elements
      */
      vlanMacDbFuncs.alloc_db_elem = L7_NULLPTR;
      vlanMacDbFuncs.copy_db_elem  = L7_NULLPTR; 
      vlanMacDbFuncs.free_db_elem  = L7_NULLPTR;
      vlanMacDbFuncs.pack_db_elem  = L7_NULLPTR;
      vlanMacDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L2_VLAN_MAC_DB_ID, 
                                          &vlanMacDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);   
      }

      uslVlanMacDbActive = L7_TRUE;
      uslVlanMacDbHandle = uslOperVlanMacDb;
    }
  
    rc = L7_SUCCESS;
  }
  while (0);

  return rc;  
}

/*********************************************************************
* @purpose  Fini Vlan Mac resources
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    On error, all resources will be released
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_mac_db_fini()
{
  L7_RC_t rc = L7_SUCCESS;

  uslVlanMacDbActive = L7_FALSE;
  uslVlanMacDbHandle = L7_NULLPTR;

  if (pUslVlanMacDbSema != L7_NULLPTR)
  {
    osapiSemaDelete(pUslVlanMacDbSema);
    pUslVlanMacDbSema = L7_NULLPTR;
  }

  if (uslOperVlanMacDb != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperVlanMacDb);
    uslOperVlanMacDb = L7_NULLPTR;
  }

  if (uslShadowVlanMacDb != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowVlanMacDb);
    uslShadowVlanMacDb = L7_NULLPTR;
  }

  return rc;  
}

/*********************************************************************
* @purpose  Invalidates all the entries of uslvlanipsubnet table as unused
*
* @param    none
*
* @returns  L7_SUCCESS
*
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_mac_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t result = L7_SUCCESS;

  if(!uslVlanMacDbActive)
    return result;

  USL_VLAN_MAC_DB_LOCK_TAKE();

  if (flags & USL_OPERATIONAL_DB)
  {
    memset(uslOperVlanMacDb, 0, 
           L7_VLAN_MAC_MAX_VLANS * sizeof(usl_vlan_mac_db_elem_t));
  }

  if (flags & USL_SHADOW_DB)
  {
    memset(uslShadowVlanMacDb, 0, 
           L7_VLAN_MAC_MAX_VLANS * sizeof(usl_vlan_mac_db_elem_t));
  }


  USL_VLAN_MAC_DB_LOCK_GIVE();

  return (result);
}

/*********************************************************************
* @purpose  Shows the entries of vlan mac db
*
* @param   dbType {(input)} dbType to dump
*
* @returns none
*
* @end
*
*********************************************************************/
void usl_vlan_mac_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                     uslStr[512];
  usl_vlan_mac_db_elem_t        elem, nextElem, *dbHandle;
  uslDbElemInfo_t               searchElem, elemInfo;
  L7_uint32                     entryCount = 0;


  if (uslVlanMacDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Vlan Mac database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Vlan Mac database is active\n");
  }

  dbHandle = usl_vlan_mac_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n", 
                   usl_db_type_name_get(dbType));
  }

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_vlan_mac_db_elem(dbType, USL_DB_NEXT_ELEM, 
                                  searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_vlan_mac_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                  uslStr);
    searchElem.elemIndex = elemInfo.elemIndex;
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                "Total number of entries in the table: %d\n",
                entryCount);


  return;
}

/*********************************************************************
* @purpose  Set the Vlan Mac Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_vlan_mac_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_VLAN_MAC_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    uslVlanMacDbHandle = uslOperVlanMacDb;  
  }
  else if (dbType == USL_SHADOW_DB)
  {
    uslVlanMacDbHandle = uslShadowVlanMacDb;  
  }

  USL_VLAN_MAC_DB_LOCK_GIVE();

  return;
}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
usl_protected_group_db_elem_t 
                   *usl_protected_group_db_handle_get(USL_DB_TYPE_t dbType)
{
  usl_protected_group_db_elem_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = pUslOperProtectedGroupDb;
      break;

    case USL_SHADOW_DB:
      dbHandle = pUslShadowProtectedGroupDb;
      break;

    case USL_CURRENT_DB:
      dbHandle = pUslProtectedGroupDbHandle;
      break;

    default:
      break;   
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a protected group Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_protected_group_db_elem(void)
{
  return sizeof(usl_protected_group_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a group db element in 
*           specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_protected_group_db_elem(void *item, L7_uchar8 *buffer,
                                       L7_uint32 size)
{
  L7_uint32                      port, modid;
  usl_protected_group_db_elem_t *elem = item;

  osapiSnprintf(buffer, size,
                "Group Id %d Membership: \n",
                elem->groupData.groupId);

  for (modid = 0; modid < L7_MOD_MAX; modid++)
  {
    osapiSnprintfcat(buffer, size,
                    "Member ports for modid %d:  ", modid);
    BCM_PBMP_ITER(elem->groupData.mod_pbmp[modid], port) 
    {
      osapiSnprintfcat(buffer, size,
                      " %d ", port);
    }
    osapiSnprintfcat(buffer, size,
                    "\n");
  } 
}

/*********************************************************************
* @purpose  Search an element in db and return pointer to it.
*
* @params   dbType     {(input)} Db type 
*           flags      {(input)} EXACT or GET_NEXT
*           searchInfo {(input)} Information about element to be searched
*           elemInfo   {(output)} Storage for elem pointer & index
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. DB lock should be held by the caller.
*
* @end
*********************************************************************/
L7_int32 usl_search_protected_group_db_elem(USL_DB_TYPE_t dbType,
                                            L7_uint32 flags, 
                                            uslDbElemInfo_t searchInfo,
                                            uslDbElemInfo_t *elemInfo)
{
  usl_protected_group_db_elem_t  *dbHandle = L7_NULLPTR;
  usl_protected_group_db_elem_t  *protectedGroupElem = searchInfo.dbElem;
  L7_int32                        rv = BCM_E_NONE;
  L7_uint32                       idx;
  L7_uchar8                       traceStr[128];

  if (!uslProtectedGroupDbActive)
  {
    return rv;    
  }
  
  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d current index %d\n",
                flags, searchInfo.elemIndex);
  usl_trace_sync_detail(USL_L2_PROTECTED_GRP_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  USL_PROTECTED_GROUP_DB_LOCK_TAKE();

  do
  {

    dbHandle = usl_protected_group_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      /* Start from beginning if the last index is not valid */
      if (searchInfo.elemIndex == USL_INVALID_DB_INDEX)
      {
        idx = 0;    
      }
      else
      {
        idx = searchInfo.elemIndex + 1;
      }

      while ((idx < L7_PROTECTED_PORT_MAX_GROUPS) && 
             (dbHandle[idx].valid == L7_FALSE))
        idx++;

      if (idx < L7_PROTECTED_PORT_MAX_GROUPS) 
      {
        elemInfo->dbElem = &(dbHandle[idx]);
        elemInfo->elemIndex = idx;
        rv = BCM_E_NONE;
      }
      else
      {
        rv = BCM_E_NOT_FOUND;
      }
   }
   else if (flags == USL_DB_EXACT_ELEM)
   {
     for (idx = 0; idx < L7_PROTECTED_PORT_MAX_GROUPS; idx++)
     {
       if ((dbHandle[idx].valid == L7_TRUE) &&
           (dbHandle[idx].groupData.groupId == protectedGroupElem->groupData.groupId))
       {
         elemInfo->dbElem = &(dbHandle[idx]);
         elemInfo->elemIndex = idx;
         rv = BCM_E_NONE;
         break;
       }
     }
 
     if (idx >= L7_PROTECTED_PORT_MAX_GROUPS)
     {
       rv = BCM_E_NOT_FOUND;
     }
   }

  } while (0);

  USL_PROTECTED_GROUP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_protected_group_db_elem(USL_DB_TYPE_t     dbType, 
                                         L7_uint32         flags,
                                         uslDbElemInfo_t   searchElem,
                                         uslDbElemInfo_t  *elemInfo)
{
  uslDbElemInfo_t     pgElem;
  L7_int32            rv = BCM_E_NONE;

  if (!uslProtectedGroupDbActive)
  {
    return rv;    
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  pgElem.dbElem = L7_NULLPTR;
  pgElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_PROTECTED_GROUP_DB_LOCK_TAKE();

  do
  {
    /* Search for the elem */
    rv = usl_search_protected_group_db_elem(dbType, flags, 
                                            searchElem, &pgElem);
    if (rv == BCM_E_NONE)
    {
      /* Element found, copy in the user memory */
      memcpy(elemInfo->dbElem, pgElem.dbElem, 
             sizeof(usl_protected_group_db_elem_t));
      memcpy(&(elemInfo->elemIndex), &(pgElem.elemIndex), 
             sizeof(elemInfo->elemIndex)); 
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_PROTECTED_GROUP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from Protected Group Db
*
* @params   dbType   {(input)} 
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_protected_group_db_elem(USL_DB_TYPE_t   dbType, 
                                            uslDbElemInfo_t elemInfo)

{

  L7_int32                            rv = BCM_E_NONE;
  usl_protected_group_db_elem_t      *dbHandle = L7_NULLPTR;
  uslDbElemInfo_t                     pgElem;

  if (!uslProtectedGroupDbActive)
  {
    return rv;    
  }

  pgElem.dbElem = L7_NULLPTR;
  pgElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_PROTECTED_GROUP_DB_LOCK_TAKE();

  do
  {
    /* If the elemIndex is specified, then use that to delete the element */
    if ((elemInfo.elemIndex != USL_INVALID_DB_INDEX) && 
        (elemInfo.elemIndex < L7_PROTECTED_PORT_MAX_GROUPS))
    {
      dbHandle = usl_protected_group_db_handle_get(dbType);
      if (dbHandle != L7_NULLPTR)
      {
        memset(&(dbHandle[elemInfo.elemIndex]), 0, 
               sizeof(usl_protected_group_db_elem_t));
      }
      else
      {
        rv = BCM_E_NOT_FOUND;
      }
    }
    else
    {
      if (elemInfo.dbElem == L7_NULLPTR)
      {
        rv = BCM_E_FAIL;
        break;          
      }
      /* Search and delete the elem */
      rv = usl_search_protected_group_db_elem(dbType, USL_DB_EXACT_ELEM, 
                                              elemInfo, &pgElem);
      if (rv == BCM_E_NONE)
      {
        memset(pgElem.dbElem, 0, 
               sizeof(usl_protected_group_db_elem_t));    
      }
      else
      {
        /* Element not found */
        rv = BCM_E_NOT_FOUND;
      }
    }

  } while (0);
  
  USL_PROTECTED_GROUP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Remove a port from protected group in the USL database
*
* @param    dbType      @{(input)} dbType to modify
* @param    deleteInfo  @{(input)}  Information about the deleted port
*
*
* @returns  BCM error code
*
* @notes   
*
* @end
*********************************************************************/
int usl_db_protected_group_port_remove(USL_DB_TYPE_t dbType,
                                       usl_bcm_protected_group_delete_t *deleteInfo)
{
  L7_uint32                      i, valid = L7_FALSE;
  L7_int32                       modid, bcmPort;
  int                            rv = BCM_E_NONE;
  usl_protected_group_db_elem_t  pgElem, *elem;
  uslDbElemInfo_t                searchInfo, elemInfo;

  if (!uslProtectedGroupDbActive)
  {
    return rv; 
  }

  memset(&pgElem, 0, sizeof(pgElem));
  pgElem.groupData.groupId = deleteInfo->groupId;

  searchInfo.dbElem = &pgElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;
  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  modid = deleteInfo->modid;
  bcmPort = deleteInfo->bcmPort;

  USL_PROTECTED_GROUP_DB_LOCK_TAKE();

  rv = usl_search_protected_group_db_elem(dbType, USL_DB_EXACT_ELEM,
                                          searchInfo, &elemInfo);
  if (rv == BCM_E_NONE)
  {
    elem = (usl_protected_group_db_elem_t *) elemInfo.dbElem;

    BCM_PBMP_PORT_REMOVE(elem->groupData.mod_pbmp[modid], bcmPort);

    /* Check if the groups needs to be deleted */
    for (i = 0; i < L7_MOD_MAX; i++)
    {
      if (BCM_PBMP_NOT_NULL(elem->groupData.mod_pbmp[i]))
      {
        valid = L7_TRUE; 
        break; 
      }
    }

    if (valid == L7_FALSE)
    {
      memset(elem, 0, sizeof(usl_protected_group_db_elem_t));
    }
  }

  USL_PROTECTED_GROUP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set a protected group membership in the USL database
*
* @param    dbType      @{(input)} dbType to modify
* @param    protectedGroupInfo  @{(input)}  protected port group info
*
*
* @returns  BCM error code
*
* @notes   
*
* @end
*********************************************************************/
int usl_db_protected_group_set(USL_DB_TYPE_t dbType,
                               usl_bcm_protected_group_t *groupInfo)
{
  L7_uint32                      idx, freeIdx = USL_BCM_PROTECTED_GROUP_INVALID_ID;
  L7_BOOL                        groupFound = L7_FALSE;
  int                            rv = BCM_E_NONE;
  usl_protected_group_db_elem_t *dbHandle;

  if (!uslProtectedGroupDbActive)
  {
    return rv; 
  }


  dbHandle = usl_protected_group_db_handle_get(dbType);
  if (dbHandle == L7_FALSE)
  {
    rv = BCM_E_FAIL;
    return rv;
  }

  USL_PROTECTED_GROUP_DB_LOCK_TAKE();


  for (idx = 0; idx < L7_PROTECTED_PORT_MAX_GROUPS; idx++)
  {
    if (groupInfo->groupId == dbHandle[idx].groupData.groupId)
    {
      dbHandle[idx].valid = L7_TRUE;
      memcpy(&(dbHandle[idx].groupData), groupInfo, sizeof(*groupInfo));    
      groupFound = L7_TRUE;
      break;
    }

    if ((dbHandle[idx].valid == L7_FALSE) &&
        (freeIdx == USL_BCM_PROTECTED_GROUP_INVALID_ID))
    {
      freeIdx = idx; 
    }
  }

  if (groupFound == L7_FALSE) 
  {
    if (freeIdx != USL_BCM_PROTECTED_GROUP_INVALID_ID)
    {
      dbHandle[freeIdx].valid = L7_TRUE;
      memcpy(&(dbHandle[freeIdx].groupData), groupInfo, sizeof(*groupInfo));    
    }
    else
    {
      rv = BCM_E_FAIL;
    }
  }

  USL_PROTECTED_GROUP_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create all protected group attributes at Usl Bcm layer from 
*           the given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_protected_group_db_elem_bcm(void *item)
{
  usl_protected_group_db_elem_t *elem = item;
  L7_int32                       rv;

  do
  {
    rv = usl_bcm_protected_group_set(&(elem->groupData));
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      break;  
    }
    
  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete all the protected group attributes at the Usl Bcm layer 
*           from the given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_protected_group_db_elem_bcm(void *item)
{
  L7_uint32                         modid, port;
  usl_protected_group_db_elem_t    *elem = item;
  usl_bcm_protected_group_delete_t  deleteInfo;

  deleteInfo.groupId = elem->groupData.groupId;

  for (modid = 0; modid < L7_MOD_MAX; modid++)
  {
    BCM_PBMP_ITER(elem->groupData.mod_pbmp[modid], port) 
    {
      deleteInfo.modid = modid;
      deleteInfo.bcmPort = port;      
      usl_bcm_protected_group_port_remove(&deleteInfo);
    }
  }

  return BCM_E_NONE;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_protected_group_db_elem_bcm(void *shadowDbItem, 
                                                void *operDbItem)
{
  
  L7_int32                            rv = BCM_E_NONE;
  L7_uint32                           modid, port;
  usl_protected_group_db_elem_t      *shadowElem =  shadowDbItem;
  usl_protected_group_db_elem_t      *operElem = operDbItem;
  usl_bcm_protected_group_delete_t    deleteInfo;

  deleteInfo.groupId = shadowElem->groupData.groupId;

  do
  {
    if ((memcmp(shadowElem,
                operElem,
                sizeof(*shadowElem))) != 0) 
    {

      for (modid = 0; modid < L7_MOD_MAX; modid++)
      {
        BCM_PBMP_ITER(operElem->groupData.mod_pbmp[modid], port) 
        {
          if (!(BCM_PBMP_MEMBER(shadowElem->groupData.mod_pbmp[modid], port)))
          {
            deleteInfo.modid = modid;
            deleteInfo.bcmPort = port;      
            usl_bcm_protected_group_port_remove(&deleteInfo);
          }
        }
      }

      rv = usl_bcm_protected_group_set(&(shadowElem->groupData));
      
    }
  } while (0);

  return rv;

}

static L7_RC_t usl_protected_group_db_alloc(usl_protected_group_db_elem_t **pgDb)
{
         
  *pgDb = (usl_protected_group_db_elem_t *) osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                                        sizeof(usl_protected_group_db_elem_t) * 
                                                        (L7_PROTECTED_PORT_MAX_GROUPS));
  if (*pgDb == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(*pgDb, 0,
         sizeof(usl_protected_group_db_elem_t) * 
         (L7_PROTECTED_PORT_MAX_GROUPS));

  return L7_SUCCESS;   
}

/*********************************************************************
* @purpose  Initialize the protected group resources
*
* @param   none 
*
* @returns none
*
* @end
*
*********************************************************************/
L7_RC_t usl_protected_group_db_init()
{
  L7_RC_t           rc = L7_ERROR;
  uslDbSyncFuncs_t  protectedGroupDbFuncs;

  memset(&protectedGroupDbFuncs, 0, sizeof(protectedGroupDbFuncs));


  do
  {
    
    /* Allocate table only on stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {

      pUslProtectedGroupDbSema     = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if (pUslProtectedGroupDbSema == L7_NULLPTR)
      {
        break; 
      }
         
      if (usl_protected_group_db_alloc(&pUslOperProtectedGroupDb) != L7_SUCCESS)
      {
        break;
      }

      /* Allocate shadow table for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_protected_group_db_alloc(&pUslShadowProtectedGroupDb) != L7_SUCCESS)
        {
          break;
        }
      }

      /* Register the sync routines */
      protectedGroupDbFuncs.get_size_of_db_elem =  usl_get_size_of_protected_group_db_elem;
      protectedGroupDbFuncs.get_db_elem = usl_get_protected_group_db_elem;
      protectedGroupDbFuncs.delete_elem_from_db =  usl_delete_protected_group_db_elem;
      protectedGroupDbFuncs.print_db_elem = usl_print_protected_group_db_elem;
      protectedGroupDbFuncs.create_usl_bcm = usl_create_protected_group_db_elem_bcm;
      protectedGroupDbFuncs.delete_usl_bcm = usl_delete_protected_group_db_elem_bcm;
      protectedGroupDbFuncs.update_usl_bcm = usl_update_protected_group_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic 
      ** i.e no pointer member elements
      */
      protectedGroupDbFuncs.alloc_db_elem = L7_NULLPTR;
      protectedGroupDbFuncs.copy_db_elem  = L7_NULLPTR; 
      protectedGroupDbFuncs.free_db_elem  = L7_NULLPTR;
      protectedGroupDbFuncs.pack_db_elem  = L7_NULLPTR;
      protectedGroupDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L2_PROTECTED_GRP_DB_ID, 
                                          &protectedGroupDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);   
      }
    
      pUslProtectedGroupDbHandle = pUslOperProtectedGroupDb;
      uslProtectedGroupDbActive = L7_TRUE;
    } 
    
    rc = L7_SUCCESS;

  } while(0);

  return rc;
}

/*********************************************************************
* @purpose  Free the protected group resources
*
* @param   none 
*
* @returns none
*
* @end
*
*********************************************************************/
L7_RC_t usl_protected_group_db_fini()
{

  uslProtectedGroupDbActive = L7_FALSE;
  pUslProtectedGroupDbHandle = L7_NULLPTR;  

  if (pUslOperProtectedGroupDb != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperProtectedGroupDb);
  }

  if (pUslShadowProtectedGroupDb != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowProtectedGroupDb);
  }

  if (pUslProtectedGroupDbSema != L7_NULLPTR)
  {
    osapiSemaDelete(pUslProtectedGroupDbSema);
    pUslProtectedGroupDbSema = L7_NULLPTR;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Invalidates the content of protected group Db
*
* @param    Flag {(input)} Db type to invalidate
*
* @returns  L7_SUCCESS
*
*
* @end
*********************************************************************/
L7_RC_t usl_protected_group_db_invalidate(USL_DB_TYPE_t flag)
{
  if (uslProtectedGroupDbActive == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  USL_PROTECTED_GROUP_DB_LOCK_TAKE();

  if (flag & USL_OPERATIONAL_DB)
  {
    memset(pUslOperProtectedGroupDb,  0, 
           sizeof(usl_protected_group_db_elem_t) * (L7_PROTECTED_PORT_MAX_GROUPS));
      
  }

  if (flag & USL_SHADOW_DB)
  {
    memset(pUslShadowProtectedGroupDb,  0, 
           sizeof(usl_bcm_protected_group_t) * (L7_PROTECTED_PORT_MAX_GROUPS));
      
  }

  USL_PROTECTED_GROUP_DB_LOCK_GIVE();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Protected group Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_protected_group_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_PROTECTED_GROUP_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslProtectedGroupDbHandle = pUslOperProtectedGroupDb;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslProtectedGroupDbHandle = pUslShadowProtectedGroupDb;
  }

  USL_PROTECTED_GROUP_DB_LOCK_GIVE();

  return;
}

/*********************************************************************
* @purpose  Shows the entries of vlan mac db
*
* @param   dbType {(input)} dbType to dump
*
* @returns none
*
* @end
*
*********************************************************************/
void usl_protected_group_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8                     uslStr[512];
  usl_protected_group_db_elem_t elem, nextElem, *dbHandle;
  uslDbElemInfo_t               searchElem, elemInfo;
  L7_uint32                     entryCount = 0;


  if (uslProtectedGroupDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Protected Group database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Protected Group database is active\n");
  }

  dbHandle = usl_protected_group_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n", 
                   usl_db_type_name_get(dbType));
  }

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_protected_group_db_elem(dbType, USL_DB_NEXT_ELEM, 
                                         searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_protected_group_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                  uslStr);
    searchElem.elemIndex = elemInfo.elemIndex;
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                "Total number of entries in the table: %d\n",
                entryCount);

  return;
}

/*********************************************************************
* @purpose  Set the default values for system db elem
*
* @params   elem {(input)} system db elem 
*
* @returns  none
*
* @notes    USL System database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
static void usl_system_db_elem_init(usl_system_db_elem_t *elem)
{
  elem->l2AgingTime                   = USL_DEFAULT_AGE_TIME;
  elem->flowControlMode               = L7_FALSE;
  memset(elem->dosConfigMode,           0, sizeof(elem->dosConfigMode));
  memset(elem->dosConfigArg,            0, sizeof(elem->dosConfigArg));
  elem->dvlanEther                    = USL_DEFAULT_DVLAN_ETHERTYPE;
  elem->dvlanMode                     = L7_FALSE;
  elem->ipmcMode                      = L7_FALSE;
  elem->isValid                       = L7_FALSE;

  return;
}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
usl_system_db_elem_t *usl_system_db_handle_get(USL_DB_TYPE_t dbType)
{
  usl_system_db_elem_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = pUslOperSystemDb;
      break;

    case USL_SHADOW_DB:
      dbHandle = pUslShadowSystemDb;
      break;

    case USL_CURRENT_DB:
      dbHandle = pUslSystemDbHandle;
      break;

    default:
      break;   
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a system Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_system_db_elem(void)
{
  return sizeof(usl_system_db_elem_t);
}

/*********************************************************************
* @purpose  Delete a given element from System Db
*
* @params   dbType   {(input)} 
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_system_db_elem(USL_DB_TYPE_t   dbType, 
                                   uslDbElemInfo_t elemInfo)
{
  usl_system_db_elem_t *dbHandle = L7_NULLPTR;
  L7_int32              rv = BCM_E_NONE;

  USL_SYSTEM_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_system_db_handle_get(dbType);

    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    memset(dbHandle, 0, sizeof(usl_system_db_elem_t));
    usl_system_db_elem_init(dbHandle);

    rv = BCM_E_NONE;
 
  } while (0);
  
  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Get element from system db
*
* @params   dbType     {(input)} Db type (Oper or Shadow)
*           flags      {(input)} USL_EXACT_ELEM or USL_NEXT_ELEM
*           searchInfo {(input)} Element to be searched
*           elemInfo   {(output)} Found element info
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_get_system_db_elem(USL_DB_TYPE_t dbType,
                                L7_uint32 flags, 
                                uslDbElemInfo_t searchInfo,
                                uslDbElemInfo_t *elemInfo)
{
  usl_system_db_elem_t  *dbHandle = L7_NULLPTR;
  L7_int32               rv = BCM_E_NONE;
  L7_uchar8              traceStr[128];

  if (!uslSystemDbActive)
  {
    return rv;    
  }
  
  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: dbType %d Flags %d current index %d\n",
                dbType, flags, searchInfo.elemIndex);
  usl_trace_sync_detail(USL_SYSTEM_DB_ID, traceStr);


  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;      
  }

  USL_SYSTEM_DB_LOCK_TAKE();

  do
  {

    dbHandle = usl_system_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    if (dbHandle->isValid == L7_FALSE)
    {
      rv = BCM_E_NOT_FOUND;
      break;        
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      /* Start from beginning if the last index is not valid */
      if (searchInfo.elemIndex == USL_INVALID_DB_INDEX)
      {
        memcpy(elemInfo->dbElem, dbHandle, sizeof(usl_system_db_elem_t));
        /* Mark the currnet elem index as 0 so that
        ** Get next terminates
        */
        elemInfo->elemIndex = 0;
        rv = BCM_E_NONE;
      }
      else
      {
        rv = BCM_E_NOT_FOUND;
      }
   }
   else if (flags == USL_DB_EXACT_ELEM)
   {
     /* Always find exact elem */
     memcpy(elemInfo->dbElem, dbHandle, sizeof(usl_system_db_elem_t));
     elemInfo->elemIndex = 0;
     rv = BCM_E_NONE;
   }

  } while (0);

  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Create all system db attributes at Usl Bcm layer 
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_system_db_elem_bcm(void *item)
{
  usl_system_db_elem_t *elem = item;
  L7_int32              rv, final_rv = BCM_E_NONE;
  L7_DOSCONTROL_TYPE    type;

  do
  {
    rv = usl_bcm_flow_control_set(elem->flowControlMode, elem->systemMac.addr);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      final_rv = rv;    
    }

    for (type = DOSCONTROL_FIRST; type < DOSCONTROL_MAX; type++)
    {
      rv = usl_bcm_doscontrol_set(type, elem->dosConfigMode[type], elem->dosConfigArg[type]);
      /* Allow BCM_E_UNVAIL since all chips may not support all DOS controls. */
      if ((rv != BCM_E_UNAVAIL) && L7_BCMX_OK(rv) != L7_TRUE)
      {
        final_rv = rv;    
      }
    }

    rv = usl_bcm_ipmc_enable_set(elem->ipmcMode);
    /* Allow BCM_E_UNVAIL since all chips may not support IPMC. */
    if ((rv != BCM_E_UNAVAIL) && L7_BCMX_OK(rv) != L7_TRUE)
    {
      final_rv = rv;    
    }

    uslMacAgeTime = elem->l2AgingTime;
    usl_db_l2_age_time_set(USL_CURRENT_DB, uslMacAgeTime);

    rv = usl_bcm_dvlan_mode_set(elem->dvlanMode);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      final_rv = rv;    
    }
    
    rv = usl_bcm_dvlan_tpid_set(elem->dvlanEther);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      final_rv = rv;    
    }

    rv = usl_bcm_mmu_dropmode_set(elem->mmuDropMode);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      final_rv = rv;    
    }
  } while (0);

  return final_rv;
}

/*********************************************************************
* @purpose  Delete all the system db attributes at the Usl Bcm layer 
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_system_db_elem_bcm(void *item)
{
  /* Nothing to do here */
  return BCM_E_NONE;
}


/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_system_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  
  L7_int32                rv, final_rv = BCM_E_NONE;
  usl_system_db_elem_t   *shadowElem = shadowDbItem;
  usl_system_db_elem_t   *operElem = operDbItem;
  L7_DOSCONTROL_TYPE      type;

  do
  {
    if ((shadowElem->flowControlMode != operElem->flowControlMode) ||
        (memcmp(&(shadowElem->systemMac), &(operElem->systemMac),
                 sizeof(shadowElem->systemMac)) != 0))
    {
      rv = usl_bcm_flow_control_set(shadowElem->flowControlMode,
                                    shadowElem->systemMac.addr);    
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        final_rv = rv;    
      }
    }

    for (type = DOSCONTROL_FIRST; type < DOSCONTROL_MAX; type++)
    {
      if ((shadowElem->dosConfigMode[type] != operElem->dosConfigMode[type]) ||
          (shadowElem->dosConfigArg[type]  != operElem->dosConfigArg[type]))
      {
        rv = usl_bcm_doscontrol_set(type, shadowElem->dosConfigMode[type], shadowElem->dosConfigArg[type]);
        /* Allow BCM_E_UNVAIL since all chips may not support all DOS controls. */
        if ((rv != BCM_E_UNAVAIL) && L7_BCMX_OK(rv) != L7_TRUE)
        {
          final_rv = rv;    
        }
      }
    }

    if (shadowElem->ipmcMode != operElem->ipmcMode)
    {
      rv = usl_bcm_ipmc_enable_set(shadowElem->ipmcMode);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        final_rv = rv;    
      }
    }

    if (shadowElem->l2AgingTime != operElem->l2AgingTime)
    {
      uslMacAgeTime = shadowElem->l2AgingTime;
      usl_db_l2_age_time_set(USL_CURRENT_DB, uslMacAgeTime);
    }

    if (shadowElem->dvlanMode != operElem->dvlanMode)
    {
      rv = usl_bcm_dvlan_mode_set(shadowElem->dvlanMode);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        final_rv = rv;    
      }
    }
    
    if (shadowElem->dvlanEther != operElem->dvlanEther)
    {
      rv = usl_bcm_dvlan_tpid_set(shadowElem->dvlanEther);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        final_rv = rv;    
      }
    }

    if (shadowElem->mmuDropMode != operElem->mmuDropMode)
    {
      rv = usl_bcm_mmu_dropmode_set(shadowElem->mmuDropMode);
      if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
      {
        final_rv = rv;    
      }
    }

  } while (0);

  return final_rv;

}

/*********************************************************************
* @purpose  Print the contents of system db elem in specified buffer
*
* @params   elem     @{(input)} Element to be printed
* @params   pStr     @{(output)} String pointer
* @params   size     @{(input)} Size of string
*
* @returns  BCM error code
*
* @end
*********************************************************************/
void usl_print_system_db_elem(void *item,
                              L7_uchar8 *pStr, L7_uint32 size)
{
  usl_system_db_elem_t *elem = (usl_system_db_elem_t *)item;
  L7_DOSCONTROL_TYPE    type;

  osapiSnprintf(pStr, size,
                "L2 Age time %d\n",
                elem->l2AgingTime);
  osapiSnprintfcat(pStr, size, 
                   "Flow-control mode %d Mac %x:%x:%x:%x:%x:%x\n",
                   elem->flowControlMode,
                   elem->systemMac.addr[0], elem->systemMac.addr[1],
                   elem->systemMac.addr[2], elem->systemMac.addr[3],
                   elem->systemMac.addr[4], elem->systemMac.addr[5]);

  for (type = DOSCONTROL_FIRST; type < DOSCONTROL_MAX; type++)
  {
    osapiSnprintfcat(pStr, size,
                     "DOS config type %d mode %d arg %d\n",
                     type, elem->dosConfigMode[type], elem->dosConfigArg[type]);
  }

  osapiSnprintfcat(pStr, size,
                   "Dvlan mode %d ethertype %x \n",
                   elem->dvlanMode, elem->dvlanEther);

  osapiSnprintfcat(pStr, size,
                   "IPMC mode %d\n",
                   elem->ipmcMode);

  osapiSnprintfcat(pStr, size,
                   "isValid %d\n",
                   elem->isValid);

  osapiSnprintfcat(pStr,size,
                   "drop mode is %s\n",
                   (elem->mmuDropMode == MMU_DROP_MODE_INGRESS)?"ingress":"egress");

  return;
}

/*********************************************************************
* @purpose  Create the USL System db
*
* @params   none
*
* @returns  L7_RC_t
*
* @notes    USL System database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
L7_RC_t usl_system_db_init(void)
{
  L7_RC_t           rc = L7_FAILURE;
  uslDbSyncFuncs_t  systemDbFuncs;

  memset(&systemDbFuncs, 0, sizeof(systemDbFuncs));


  do
  {
    /* Allocate Db only on stackables */
    if ((cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)  ||
      (cnfgrIsFeaturePresent(L7_PFC_COMPONENT_ID, 
                             L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) == L7_TRUE))
    {
      pUslSystemDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if (pUslSystemDbSema == L7_NULLPTR)
      {
        break; 
      }

      pUslOperSystemDb =  (usl_system_db_elem_t *) osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                                               sizeof(usl_system_db_elem_t));
      if (pUslOperSystemDb == L7_NULLPTR)
      {
        break;
      }

      memset(pUslOperSystemDb, 0, sizeof(usl_system_db_elem_t));
      usl_system_db_elem_init(pUslOperSystemDb);

      /* Allocate shadow table for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, 
                                L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {

        pUslShadowSystemDb =  (usl_system_db_elem_t *) osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                                                   sizeof(usl_system_db_elem_t));
        if (pUslShadowSystemDb == L7_NULLPTR)
        {
          break;
        }

        memset(pUslShadowSystemDb, 0, sizeof(usl_system_db_elem_t));
        usl_system_db_elem_init(pUslShadowSystemDb);
 
      }

      /* Register the sync routines */
      systemDbFuncs.get_size_of_db_elem =  usl_get_size_of_system_db_elem;
      systemDbFuncs.get_db_elem = usl_get_system_db_elem;
      systemDbFuncs.delete_elem_from_db =  usl_delete_system_db_elem;
      systemDbFuncs.print_db_elem = usl_print_system_db_elem;
      systemDbFuncs.create_usl_bcm = usl_create_system_db_elem_bcm;
      systemDbFuncs.delete_usl_bcm = usl_delete_system_db_elem_bcm;
      systemDbFuncs.update_usl_bcm = usl_update_system_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic 
      ** i.e no pointer member elements
      */
      systemDbFuncs.alloc_db_elem = L7_NULLPTR;
      systemDbFuncs.copy_db_elem  = L7_NULLPTR; 
      systemDbFuncs.free_db_elem  = L7_NULLPTR;
      systemDbFuncs.pack_db_elem  = L7_NULLPTR;
      systemDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_SYSTEM_DB_ID, 
                                          &systemDbFuncs)!= L7_SUCCESS)
      {
        L7_LOG_ERROR(0);   
      }

      /* Set the Run time handle to Oper Db */
      pUslSystemDbHandle = pUslOperSystemDb;  
      uslSystemDbActive = L7_TRUE;
    }

    rc = L7_SUCCESS; 

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  De-allocate the USL system db
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t usl_system_db_fini(void)
{
  L7_RC_t rc = L7_SUCCESS;

   do
   {
     uslSystemDbActive = L7_FALSE;
     pUslSystemDbHandle = L7_NULLPTR;
  
     if (pUslOperSystemDb != L7_NULLPTR)
     {
       osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperSystemDb);
       pUslOperSystemDb = L7_NULLPTR;           
     }

     if (pUslShadowSystemDb != L7_NULLPTR)
     {
       osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowSystemDb);
       pUslShadowSystemDb = L7_NULLPTR;           
     }

     if (pUslSystemDbSema != L7_NULLPTR)
     {
       osapiSemaDelete(pUslSystemDbSema);
       pUslSystemDbSema = L7_NULLPTR;
     }

     rc = L7_SUCCESS;

   } while (0);

   return rc;
}

/*********************************************************************
* @purpose  Invalidate the content of the USL system db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_system_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t               rc = L7_FAILURE;

  if (uslSystemDbActive == L7_FALSE)
  {
    rc = L7_SUCCESS;
    return rc;      
  }

  USL_SYSTEM_DB_LOCK_TAKE();

  do
  {
    /* Clear the operational table */
    if (flags & USL_OPERATIONAL_DB)
    {
      memset(pUslOperSystemDb, 0, sizeof(usl_system_db_elem_t));
      usl_system_db_elem_init(pUslOperSystemDb);
    }

    /* Clear the shadow table */
    if (flags & USL_SHADOW_DB)
    {
      memset(pUslShadowSystemDb, 0, sizeof(usl_system_db_elem_t));
      usl_system_db_elem_init(pUslShadowSystemDb);
    }
    
    rc = L7_SUCCESS;

  } while (0);

  USL_SYSTEM_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the USL System Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_system_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_SYSTEM_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslSystemDbHandle = pUslOperSystemDb;
    pUslOperSystemDb->isValid = L7_TRUE;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslSystemDbHandle = pUslShadowSystemDb; 
    pUslShadowSystemDb->isValid = L7_TRUE;
  }

  USL_SYSTEM_DB_LOCK_GIVE();

  return;
}

/*********************************************************************
* @purpose  Set the dvlan mode data in Db
*
* @params   dbType    @{(input)} db type to update
* @params   mode      @{(input)} Dvlan mode
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_dvlan_mode_set(USL_DB_TYPE_t dbType, L7_uint32 mode)
{
  int                   rv = BCM_E_NONE;
  usl_system_db_elem_t *dbHandle;

  if (uslSystemDbActive == L7_FALSE)
  {
    return rv;      
  }

  dbHandle = usl_system_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  USL_SYSTEM_DB_LOCK_TAKE();

  dbHandle->dvlanMode  = mode;

  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the dvlan translation data in Db
*
* @params   dbType    @{(input)} db type to update
* @params   direction @{(input)} ingress or egress
* @params   enable    @{(input)} enable/disable vlan translation
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_dvlan_translation_set(USL_DB_TYPE_t dbType, L7_BOOL direction, L7_BOOL enable)
{
  int                   rv = BCM_E_NONE;

  rv = usl_db_port_all_vlan_translate_set(dbType, direction, enable);

  return rv;
}

/*********************************************************************
* @purpose  Set the dvlan TPID data in Db
*
* @params   dbType    @{(input)} db type to update
* @params   etherType @{(input)} Dvlan ethertype
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_dvlan_tpid_set(USL_DB_TYPE_t dbType, L7_ushort16 ethertype) 
{
  int                   rv = BCM_E_NONE;
  usl_system_db_elem_t *dbHandle;

  if (uslSystemDbActive == L7_FALSE)
  {
    return rv;      
  }

  dbHandle = usl_system_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  USL_SYSTEM_DB_LOCK_TAKE();

  dbHandle->dvlanEther = ethertype;

  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the L2 age-time in Db
*
* @params   age_time  @{(input)} age time
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l2_age_time_set(USL_DB_TYPE_t dbType, L7_uint32 age_time)
{
  int rv = BCM_E_NONE;
  usl_system_db_elem_t *dbHandle;

  if (uslSystemDbActive == L7_FALSE)
  {
    return rv;      
  }

  dbHandle = usl_system_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }


  USL_SYSTEM_DB_LOCK_TAKE();

  dbHandle->l2AgingTime = age_time;

  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the flow-control mode and mac
*
* @params   dbType {(input)} db type to update
* @params   age_time  @{(input)} age time
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_flow_control_set(USL_DB_TYPE_t dbType, L7_uint32 mode, L7_uchar8 *mac)
{
  int rv = BCM_E_NONE;
  usl_system_db_elem_t *dbHandle;

  if (uslSystemDbActive == L7_FALSE)
  {
    return rv;      
  }

  dbHandle = usl_system_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }


  USL_SYSTEM_DB_LOCK_TAKE();

  dbHandle->flowControlMode = mode;
  memcpy(&(dbHandle->systemMac),
          mac,
          sizeof(dbHandle->systemMac)); 

  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Get the flow-control mode and mac
*
* @params   dbType {(input)} db type to update
* @params   age_time  @{(input)} age time
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_flow_control_get(USL_DB_TYPE_t dbType, L7_uint32 *mode, L7_uchar8 *mac)
{
  int rv = BCM_E_NONE;
  usl_system_db_elem_t *dbHandle;

  if (uslSystemDbActive == L7_FALSE)
  {
    return rv;      
  }

  dbHandle = usl_system_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }


  USL_SYSTEM_DB_LOCK_TAKE();

  *mode = dbHandle->flowControlMode;
  memcpy(mac,&(dbHandle->systemMac),
         sizeof(dbHandle->systemMac)); 

  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the DOS control mode
*
* @params   dbType  @{(input)}  db type to update
* @param    type    @{{input}}  type of DOS control
* @param    mode    @{{input}}  enable=1,disable=0
* @param    arg     @{{input}}  argument for certain types of DOS control
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_doscontrol_set(USL_DB_TYPE_t dbType, L7_DOSCONTROL_TYPE type, L7_uint32 mode, L7_uint32 arg)
{
  int rv = BCM_E_NONE;
  usl_system_db_elem_t *dbHandle;

  if (uslSystemDbActive == L7_FALSE)
  {
    return rv;      
  }

  dbHandle = usl_system_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }


  USL_SYSTEM_DB_LOCK_TAKE();

  dbHandle->dosConfigMode[type] = mode;
  dbHandle->dosConfigArg[type]  = arg;

  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the mmu drop mode
*
* @params   dbType {(input)} db type to update
* @params   mode   {(input)} drop mode
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_mmu_dropmode_set(USL_DB_TYPE_t dbType, L7_uint32 mode)
{
  int rv = BCM_E_NONE;
  usl_system_db_elem_t *dbHandle;

  if (uslSystemDbActive == L7_FALSE)
  {
    return rv;      
  }

  dbHandle = usl_system_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  USL_SYSTEM_DB_LOCK_TAKE();

  dbHandle->mmuDropMode = mode;

  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the IPMC global mode
* 
* @param    dbType {(input)} dbType to update
* @param    enabled {(input)} IP Mcast global mode
*
* @returns  BCM error code 
*
* @end
*********************************************************************/
int usl_db_ipmc_mode_set(USL_DB_TYPE_t dbType, L7_BOOL enabled)
{
  int rv = BCM_E_NONE;
  usl_system_db_elem_t *dbHandle;

  if (uslSystemDbActive == L7_FALSE)
  {
    return rv;      
  }

  dbHandle = usl_system_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }


  USL_SYSTEM_DB_LOCK_TAKE();

  dbHandle->ipmcMode = enabled;

  USL_SYSTEM_DB_LOCK_GIVE();

  return rv;
}



void usl_system_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8             uslStr[1024];
  usl_system_db_elem_t *dbHandle;

  if (uslSystemDbActive == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "System Db is not active\n");      

    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "System Db is active\n");      
  }


  dbHandle = usl_system_db_handle_get(dbType);
  if (dbHandle != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Dumping %s \n",
                  usl_db_type_name_get(dbType));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%s is not present\n", 
                   usl_db_type_name_get(dbType));
    return;
  }


  usl_print_system_db_elem(dbHandle, uslStr, sizeof(uslStr));
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                uslStr);

  return;
}
/*********************************************************************
* @purpose  Release all resources allocated during usl_l2_init()
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were released
* @returns  L7_ERROR   - if any resourses were not released 
*
* @notes    Should not call this unless the an error occured during init or the
*           configurator is taking the us back to pre Phase 1   
*
* @notes    If the system has been operational for a while, the caller should
*           have called the usl_l2_database_invalidate before calling this
*           in order to insure that the lplists have been properly freed       
* @end
*********************************************************************/
L7_RC_t usl_l2_db_fini()
{
  L7_RC_t rc = L7_SUCCESS;

  (void) usl_vlan_db_fini();
  (void) usl_trunk_db_fini();
  (void) usl_stg_db_fini();
  (void) usl_l2mc_db_fini();
  (void) usl_vlan_ipsubnet_db_fini();
  (void) usl_vlan_mac_db_fini();
  (void) usl_protected_group_db_fini();
  (void) usl_system_db_fini();

  return rc;
}

/*********************************************************************
* @purpose  Initialize the Layer 2 Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
L7_RC_t usl_l2_db_init()
{
  L7_RC_t rc = L7_ERROR;
  

  do
  {

    uslMacSyncDbType = USL_OPERATIONAL_DB;

    if (usl_vlan_db_init() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "USL vlan database creation failed\n");
      break;
    }

    if (usl_trunk_db_init() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "USL trunk database creation failed\n");
      break;
    }

    if (usl_stg_db_init() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "USL stg database creation failed\n");
      break;
    }


    if (usl_l2mc_db_init() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "USL l2mc database creation failed\n");
      break;
    }

    if (usl_vlan_ipsubnet_db_init() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "USL ipsubnet vlan database creation failed\n");
      break;
    }

    if (usl_vlan_mac_db_init() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "USL mac vlan database creation failed\n");
      break;
    }

    if (usl_protected_group_db_init() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "USL protected group database creation failed\n");
      break;
    }

    if (usl_system_db_init() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "USL system database creation failed\n");
      break;
    }

    /* If we got here, all must going well */
    rc = L7_SUCCESS;

  } while ( 0 );

  /* if we failed, clean up all resources */
  if (rc != L7_SUCCESS)
    usl_l2_db_fini();

  return rc;
}

/*********************************************************************
* @purpose  Clear all entries from the L2 databases
*
* @param    void
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @notes    Use this function to reset all the tables to the default, no 
*           resources will be deallocated
*
* @end
*********************************************************************/
L7_RC_t usl_l2_db_invalidate(USL_DB_TYPE_t flag)
{
  L7_RC_t rc = L7_SUCCESS;

  do
  {

    if (usl_vlan_db_invalidate(flag) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "Failed to invalidate Vlan Db, dbTypes %d\n", flag);
      break;
    }
  
    if (usl_trunk_db_invalidate(flag) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "Failed to invalidate Trunk Db, dbTypes %d\n", flag);
      break;
    }
  
    if (usl_stg_db_invalidate(flag) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "Failed to invalidate Stg Db, dbTypes %d\n", flag);
      break;
    }
  
    if (usl_l2mc_db_invalidate(flag) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "Failed to invalidate L2Mc Db, dbTypes %d\n", flag);

      break;
    }

    if (usl_vlan_ipsubnet_db_invalidate(flag) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "Failed to invalidate IPSubnet Vlan Db, dbTypes %d\n", flag);

      break;
    }
 
    if (usl_vlan_mac_db_invalidate(flag) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "Failed to invalidate Mac Vlan Db, dbTypes %d\n", flag);

      break;
    }

    if (usl_protected_group_db_invalidate(flag) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "Failed to invalidate protcted group Db, dbTypes %d\n", flag);

      break;
    }
   
    if (usl_system_db_invalidate(flag) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "Failed to invalidate system Db, dbTypes %d\n", flag);

      break;
    }
  
    rc = L7_SUCCESS;

  } while ( 0 );


  return rc;
}

/*********************************************************************
* @purpose  Set the L2 db handle to Operational or Shadow table
*
* @param    dbType  {(input)} Type of db to set
*
* @returns  L7_SUCCESS - if all the ports were deleted
* @returns  L7_FAILURE - An error was encountered during deletion
*
*       
* @end
*********************************************************************/
L7_RC_t usl_l2_db_handle_set(USL_DB_TYPE_t dbType)
{
  L7_RC_t rc = L7_SUCCESS;

  /* If trying to set the dbHandle to Shadow tables when they are not allocated */
  if ((dbType == USL_SHADOW_DB) &&
      (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_FALSE))
  {
    rc = L7_FAILURE;
    return rc;    
  }

  uslMacSyncDbType = dbType;

  usl_vlan_current_db_handle_set(dbType);
  usl_trunk_current_db_handle_set(dbType);
  usl_stg_current_db_handle_set(dbType);
  usl_l2mc_current_db_handle_set(dbType);
  usl_vlan_ipsubnet_current_db_handle_set(dbType);
  usl_vlan_mac_current_db_handle_set(dbType);
  usl_protected_group_current_db_handle_set(dbType);
  usl_system_current_db_handle_set(dbType);
  
  return rc;
}

/*********************************************************************
* @purpose  Gets the current DB Type for the L2 macsync table.
*
* @param    
*
* @returns  USL_DB_TYPE_t
*
*       
* @end
*********************************************************************/
USL_DB_TYPE_t usl_l2_db_macsync_dbtype_get()
{
  return uslMacSyncDbType;
}

/*********************************************************************
* @purpose  Initialize the Layer 2 hw id generators
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
L7_RC_t usl_l2_hw_id_generator_init()
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    if (usl_stg_hw_id_generator_init() != L7_SUCCESS)
    {
      break;    
    }

    if (usl_trunk_hw_id_generator_init() != L7_SUCCESS)
    {
      break;    
    }

    if (usl_l2mc_hw_id_generator_init() != L7_SUCCESS)
    {
      break;    
    }

 
    rc = L7_SUCCESS;
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Reset the Layer 2 hw id generators
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
L7_RC_t usl_l2_hw_id_generator_reset()
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    if (usl_stg_hw_id_generator_reset() != L7_SUCCESS)
    {
      break;    
    }

    if (usl_trunk_hw_id_generator_reset() != L7_SUCCESS)
    {
      break;    
    }

    if (usl_l2mc_hw_id_generator_reset() != L7_SUCCESS)
    {
      break;    
    }

    rc = L7_SUCCESS;
  } while(0);

  return rc;
}


/*********************************************************************
* @purpose  Reset and populate the Layer2 Hw id generator from USL
*           Operational Db
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    Called during warm start to mark the hw indexes in Operational
*           Db as used.
*       
* @end
*********************************************************************/
L7_RC_t usl_l2_hw_id_generator_sync()
{
  usl_l2_hw_id_generator_reset();
  
  usl_stg_hw_id_generator_sync();

  usl_trunk_hw_id_generator_sync();

  usl_l2mc_hw_id_generator_sync();

  return L7_SUCCESS;
}


void usl_l2_sem_show()
{
  if ((cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)  ||
      (cnfgrIsFeaturePresent(L7_PFC_COMPONENT_ID, 
                             L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) == L7_TRUE))
  {
    sysapiPrintf("System Db sema %x\n", pUslSystemDbSema);
    sysapiPrintf("IP Vlan Db sema %x\n", pUslVlanIpSubnetDbSema);
    sysapiPrintf("Mac Vlan Db sema %x\n", pUslVlanMacDbSema);
    sysapiPrintf("Protected Grp Db sema %x\n", pUslProtectedGroupDbSema);
#ifdef L7_STACKING_PACKAGE
    usl_mac_sem_show();
#endif
  }
}

void usl_l2_debug_help()
{
  sysapiPrintf("\n****************** USL L2 DEBUG HELP ***************\n");
  sysapiPrintf("usl_l2mc_db_dump(dbType) - L2 Mc table\n");
  sysapiPrintf("usl_trunk__db_dump(dbType) - Trunk table\n");
  sysapiPrintf("usl_stg_db_dump(dbType) - Stg table\n");
  sysapiPrintf("usl_vlan_db_dump(dbType) - Vlan table\n");
  sysapiPrintf("usl_vlan_ipsubnet_db_dump(dbType) - IP Vlan table\n");
  sysapiPrintf("usl_vlan_mac_db_dump(dbType) - Mac Vlan table\n");
  sysapiPrintf("usl_protected_group_db_dump(dbType) - Protected group table\n");
  sysapiPrintf("usl_system_db_dump(dbType) - System table\n");
  sysapiPrintf("\nUse dbType %d for Operational table, %d for Shadow table\n",
               USL_OPERATIONAL_DB, USL_SHADOW_DB); 
  sysapiPrintf("usl_l2_sem_show() - display all the semaphores\n");


}


