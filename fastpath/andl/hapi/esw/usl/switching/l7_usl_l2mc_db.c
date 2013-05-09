/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_l2mc_db.c
*
* @purpose    USL L2 Mc DB API implementation 
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


#include "broad_common.h"
#include "l7_usl_l2mc_db.h"
#include "l7_usl_trace.h"
#include "l7_usl_sm.h"
#include "cnfgr.h"
#include "bcm_int/common/multicast.h" /* PTin modified: new SDK  (esw->common) */

#include "ibde.h"
#include "bcm/stack.h"

/* Mcast Table resources (semaphore and avl tree) */
static void                  *pUslL2McastDbSema = L7_NULLPTR;

avlTreeTables_t              *pUslOperL2McastTreeHeap = L7_NULLPTR;
usl_l2mcast_db_elem_t        *pUslOperL2McastDataHeap = L7_NULLPTR;
avlTree_t                     uslOperL2McastTreeData   = { 0 };

avlTreeTables_t              *pUslShadowL2McastTreeHeap = L7_NULLPTR;
usl_l2mcast_db_elem_t        *pUslShadowL2McastDataHeap = L7_NULLPTR;
avlTree_t                     uslShadowL2McastTreeData   = { 0 };

avlTree_t                    *pUslL2McastTreeHandle   = { 0 };

L7_BOOL                       uslL2McDbActive        = L7_FALSE;
usl_l2mc_hw_id_list_t        *pUslL2McHwIdList = L7_NULLPTR;
L7_int32                      uslL2McHwIdMin = 0, uslL2McHwIdMax = 0;

#define USL_L2MC_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_MCAST_DB_ID,"L2MC",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslL2McastDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n", pUslL2McastDbSema); \
  } \
}

#define USL_L2MC_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_MCAST_DB_ID,"L2MC",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslL2McastDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", pUslL2McastDbSema); \
  } \
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
avlTree_t *usl_l2mc_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperL2McastTreeData;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowL2McastTreeData;
      break;

    case USL_CURRENT_DB:
      dbHandle = pUslL2McastTreeHandle;
      break;

    default:
      break;   
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a L2Mc Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_l2mc_db_elem(void)
{
  return sizeof(usl_l2mcast_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a L2mc db element in specified buffer
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
void usl_print_l2mc_db_elem(void *item, L7_uchar8 *buffer,
                            L7_uint32 size)
{
  L7_uint32             modid, port, gport;
  usl_bcm_mcast_addr_t *mcastAddr = &(((usl_l2mcast_db_elem_t *)item)->bcm_data);

  osapiSnprintf(buffer, size,
                "Mac %x:%x:%x:%x:%x:%x vid %d\n",
                mcastAddr->mac[0], mcastAddr->mac[1],
                mcastAddr->mac[2], mcastAddr->mac[3],
                mcastAddr->mac[4], mcastAddr->mac[5], 
                mcastAddr->vid);
  
  osapiSnprintfcat(buffer, size,
                   "L2mc index %d cos_dst %d\n",
                   mcastAddr->l2mc_index, mcastAddr->cos_dst);
     

  for (modid = 0; modid < L7_MOD_MAX; modid++)
  {
    osapiSnprintfcat(buffer, size,
                    "Member ports for modid %d:  ", modid);

    BCM_PBMP_ITER(mcastAddr->mod_pbmp[modid], port) 
    {
      osapiSnprintfcat(buffer, size,
                      " %d ", port);
    }

    osapiSnprintfcat(buffer, size,
                    "\n");
  }
  BROAD_WLAN_PBMP_ITER(mcastAddr->wlan_pbmp, port)
  {
    BROAD_WLAN_ID_TO_GPORT(gport, port);
    osapiSnprintfcat(buffer, size,
                    " %d ", gport);
  }
 
}


/*********************************************************************
* @purpose  Search element in L2 MC db and return pointer to it
*
* @params   dbType     {(input)} Db type (Oper or Shadow)
*           flags      {(input)} USL_EXACT_ELEM or USL_NEXT_ELEM
*           searchInfo {(input)} Element to be searched
*           elemInfo   {(output)} Found element info
*
* @returns  BCM_E_NONE: Element found
*           BCM_E_NOT_FOUND: Element not found
*
* @notes    Internal API. L2 MC Db lock must be held. Returns pointer
*           to the element.
*
* @end
*********************************************************************/
static L7_int32 usl_search_l2mc_db_elem(USL_DB_TYPE_t dbType,
                                        L7_uint32 flags, 
                                        uslDbElemInfo_t searchInfo,
                                        uslDbElemInfo_t *elemInfo)
{
  avlTree_t                 *dbHandle = L7_NULLPTR;
  L7_int32                   rv = BCM_E_NOT_FOUND;
  L7_uchar8                  traceStr[128];

  
  osapiSnprintf(traceStr, sizeof(traceStr),
                "Get Elem: Flags %d \n",
                flags);
  usl_trace_sync_detail(USL_L2_MCAST_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_l2mc_db_handle_get(dbType);

    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Get next */
    if (flags == USL_DB_NEXT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_NEXT); 
    }
    else if (flags == USL_DB_EXACT_ELEM)
    {
      elemInfo->dbElem = avlSearchLVL7(dbHandle, searchInfo.dbElem, AVL_EXACT); 
    }

    if (elemInfo->dbElem == L7_NULLPTR)
    {
      rv = BCM_E_NOT_FOUND;    
    }
    else
    {
      rv = BCM_E_NONE;
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
L7_int32 usl_get_l2mc_db_elem(USL_DB_TYPE_t     dbType, 
                              L7_uint32         flags,
                              uslDbElemInfo_t   searchElem,
                              uslDbElemInfo_t  *elemInfo)
{
  L7_int32        rv = BCM_E_NONE;
  uslDbElemInfo_t mcastInfo;

  if (!uslL2McDbActive)
  {
    return rv;    
  }

  mcastInfo.dbElem = L7_NULLPTR;
  mcastInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_L2MC_DB_LOCK_TAKE();

  rv = usl_search_l2mc_db_elem(dbType, flags, searchElem, &mcastInfo);
  if (rv == BCM_E_NONE)
  {
    memcpy(elemInfo->dbElem, mcastInfo.dbElem, sizeof(usl_l2mcast_db_elem_t));    
  }

  USL_L2MC_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Delete a given element from L2 Mc Db
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
L7_int32 usl_delete_l2mc_db_elem(USL_DB_TYPE_t   dbType, 
                                 uslDbElemInfo_t elemInfo)
{
  avlTree_t              *dbHandle;
  usl_l2mcast_db_elem_t  *elem;
  L7_int32                rv = BCM_E_NONE;

  if (!uslL2McDbActive)
  {
    return rv;    
  }

  USL_L2MC_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_l2mc_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;    
    }
  
    /* Delete the elem */
    elem = avlDeleteEntry(dbHandle, elemInfo.dbElem);
    if (elem == L7_NULLPTR)
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_L2MC_DB_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Add/Remove a L2 Multicast MAC entry to/from the USL Database
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    updateCmd    @{(input)} Add or Remove the address
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l2mc_addr_update(USL_DB_TYPE_t dbType, 
                            usl_bcm_mcast_addr_t *mcastAddr, 
                            USL_CMD_t updateCmd)
{
  int                      rv = BCM_E_NONE;
  usl_l2mcast_db_elem_t   data, *pData;
  avlTree_t              *dbHandle;

  if (!uslL2McDbActive)
  {
    return rv;      
  }

  USL_L2MC_DB_LOCK_TAKE();

  do
  {

    dbHandle = usl_l2mc_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;    
    }

    memset(&data, 0, sizeof(data));

    memcpy(&(data.bcm_data), mcastAddr, sizeof(*mcastAddr));
    data.isValid = L7_TRUE;

    if (updateCmd == USL_CMD_ADD)
    {
      pData = avlInsertEntry(dbHandle, &data);

      /* avl returns the pointer to added entry on error */
      if (pData == &data)
      {
        rv = BCM_E_FAIL;
        break;
      }

      /* avl returns non-null, but not added entry address on duplicate */
      if ((pData != L7_NULL) && (pData != &data))
      {
        rv = BCM_E_EXISTS;
        break;
      }
    }
    else if (updateCmd == USL_CMD_REMOVE)
    {
      pData = avlDeleteEntry(dbHandle, &data);

      if (pData == L7_NULLPTR)
      {
        rv = BCM_E_NOT_FOUND;
        break;
      }  
    }
    else
    {
      rv = BCM_E_FAIL;
    }

  } while(0);

  USL_L2MC_DB_LOCK_GIVE();

  return rv;
}
 
/*********************************************************************
* @purpose  Add/Remove ports to a L2 Multicast MAC entry to/from the USL Database
*
* @param    mcastAddress @{(input)} Multicast address info
* @param    updateCmd    @{(input)} Add or Remove the ports
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l2mc_update_ports(USL_DB_TYPE_t dbType, usl_bcm_mcast_addr_t *mcastAddr, 
                             USL_CMD_t updateCmd)

{
  L7_uint32                idx;
  usl_l2mcast_db_elem_t    data, *pData;
  bcm_pbmp_t               tmpPbmp;
  uslDbElemInfo_t          searchInfo, elemInfo;
  int                      rv = BCM_E_NONE;

  if (!uslL2McDbActive)
  {
    return rv;      
  }

  USL_L2MC_DB_LOCK_TAKE();

  BCM_PBMP_CLEAR(tmpPbmp);

  do
  {

    memset(&data, 0, sizeof(data));

    memcpy(&(data.bcm_data.mac), &(mcastAddr->mac), sizeof(data.bcm_data.mac));
    memcpy(&(data.bcm_data.vid), &(mcastAddr->vid), sizeof(data.bcm_data.vid));
    data.isValid = L7_TRUE;

    searchInfo.dbElem = &data;
    searchInfo.elemIndex = USL_INVALID_DB_INDEX;

    elemInfo.dbElem = L7_NULLPTR;
    elemInfo.elemIndex = USL_INVALID_DB_INDEX;

    rv = usl_search_l2mc_db_elem(dbType, USL_DB_EXACT_ELEM, searchInfo, &elemInfo);
    if (rv == BCM_E_NONE)
    {
      pData = (usl_l2mcast_db_elem_t *) elemInfo.dbElem;

      for (idx = 0; idx < L7_MOD_MAX; idx++)
      {
        if (updateCmd == USL_CMD_ADD)
        {
          BCM_PBMP_OR(pData->bcm_data.mod_pbmp[idx], mcastAddr->mod_pbmp[idx]);
          BROAD_WLAN_MASKOREQ(pData->bcm_data.wlan_pbmp, mcastAddr->wlan_pbmp);
        }
        else if (updateCmd == USL_CMD_REMOVE)
        {
          BCM_PBMP_NEGATE(tmpPbmp, mcastAddr->mod_pbmp[idx]);
          BCM_PBMP_AND(pData->bcm_data.mod_pbmp[idx], tmpPbmp);
          BROAD_WLAN_MASKANDEQINV(pData->bcm_data.wlan_pbmp, mcastAddr->wlan_pbmp);
        }
        else if (updateCmd == USL_CMD_SET)
        {
          BCM_PBMP_ASSIGN(pData->bcm_data.mod_pbmp[idx], mcastAddr->mod_pbmp[idx]);    
          memcpy(&pData->bcm_data.wlan_pbmp, &mcastAddr->wlan_pbmp, sizeof(L7_WLAN_PBMP_t));
        }
        else
        {
          rv = BCM_E_FAIL;
          break;
        }
      }
    }
    else
    {
      rv = BCM_E_FAIL;
      break;
    }
  } while(0);


  USL_L2MC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Add/Remove port to L2 Multicast MAC entries to/from the USL Database
*
* @param    mcastAddress @{(input)} Multicast address info
* @param    updateCmd    @{(input)} Add or Remove the ports
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l2mc_port_update_groups(USL_DB_TYPE_t dbType, bcm_gport_t gport, L7_uint32 *l2mc_index, L7_uint32 l2mc_index_count, USL_CMD_t updateCmd)
{
  usl_l2mcast_db_elem_t    data, *pData;
  int                      rv = BCM_E_NONE;
  L7_uint32                modid;
  bcm_port_t               bcmPort;
  L7_uint32                i;
  avlTree_t               *dbHandle;

  if (!uslL2McDbActive)
  {
    return rv;      
  }

  USL_L2MC_DB_LOCK_TAKE();

  do
  {
    modid   = BCM_GPORT_MODPORT_MODID_GET(gport);
    bcmPort = BCM_GPORT_MODPORT_PORT_GET(gport);

    dbHandle = usl_l2mc_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    memset(&data, 0, sizeof(data));
    pData = avlSearchLVL7(dbHandle, &data, AVL_NEXT);

    while (pData != L7_NULLPTR)
    {
      memcpy(&data, pData, sizeof(data));

      for (i = 0; i < l2mc_index_count; i++)
      {
        if (l2mc_index[i] == pData->bcm_data.l2mc_index)
        {
          if (updateCmd == USL_CMD_ADD)
          {
            BCM_PBMP_PORT_ADD(pData->bcm_data.mod_pbmp[modid], bcmPort);
          }
          else if (updateCmd == USL_CMD_REMOVE)
          {
            BCM_PBMP_PORT_REMOVE(pData->bcm_data.mod_pbmp[modid], bcmPort);
          }
          else
          {
            rv = BCM_E_PARAM;
          }
          break;
        }
      }

      pData = avlSearchLVL7(dbHandle, &data, AVL_NEXT);
    }

  } while (0);

  USL_L2MC_DB_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Create all L2 Mc attributes at Usl Bcm layer from the given 
*           db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_create_l2mc_db_elem_bcm(void *item)
{
  L7_int32               rv;
  usl_bcm_mcast_addr_t *mcastAddr = &(((usl_l2mcast_db_elem_t *)item)->bcm_data);
  
  do
  {
    rv = usl_bcm_mcast_addr_add(mcastAddr);

  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete all the L2 Mc attributes at the Usl Bcm layer from the 
*           given db element
*
* @params   item  {(input)} Pointer to Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_l2mc_db_elem_bcm(void *item)
{
  L7_int32               rv;
  usl_bcm_mcast_addr_t  *mcastAddr = &(((usl_l2mcast_db_elem_t *)item)->bcm_data);
  
  do
  {
    rv = usl_bcm_mcast_addr_remove(mcastAddr);

  } while (0);

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
L7_int32 usl_update_l2mc_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  L7_int32                 rv = BCM_E_NONE, bcmUnit;
  L7_uint32                myModid, port;
  usl_bcm_mcast_addr_t    *shadowMcastAddr = 
                              &(((usl_l2mcast_db_elem_t *)shadowDbItem)->bcm_data);
  usl_bcm_mcast_addr_t    *operMcastAddr = 
                              &(((usl_l2mcast_db_elem_t *)operDbItem)->bcm_data);
  usl_bcm_mcast_addr_t     tmpMcastAddr;
  bcm_pbmp_t               addPbmp, removePbmp;


  do
  {
    /* If the Oper and Shadow hw indexes are not same then 
    ** the element cannot be reconciled. Return failure 
    */

    if (shadowMcastAddr->l2mc_index != operMcastAddr->l2mc_index)
    {
      rv = BCM_E_FAIL;    
    }
    else
    {

      for (bcmUnit=0; bcmUnit< bde->num_devices(BDE_SWITCH_DEVICES); bcmUnit++)
      {
        if (SOC_IS_XGS_FABRIC(bcmUnit))
        {
          continue;
        }

        /* Now resolve any differences in the local pbmp */
        rv = bcm_stk_my_modid_get(bcmUnit, &myModid);
        if (rv != BCM_E_NONE)
        {
          break;
        }

        BCM_PBMP_CLEAR(removePbmp);
        BCM_PBMP_ITER(operMcastAddr->mod_pbmp[myModid], port) 
        {
          /* Port present in Oper elem but not shadow elem */
          if (!(BCM_PBMP_MEMBER(shadowMcastAddr->mod_pbmp[myModid], port)))
          {
            BCM_PBMP_PORT_ADD(removePbmp, port);
          }
        }

        if (BCM_PBMP_NOT_NULL(removePbmp))
        {
        /* Remove all the ports in removePbmp from mcast group */
        memset(&tmpMcastAddr, 0, sizeof(tmpMcastAddr));
        memcpy(&tmpMcastAddr, shadowMcastAddr, sizeof(tmpMcastAddr));
          memset(tmpMcastAddr.mod_pbmp, 0, sizeof(tmpMcastAddr.mod_pbmp));
          memset(tmpMcastAddr.mod_ubmp, 0, sizeof(tmpMcastAddr.mod_ubmp));
        BCM_PBMP_ASSIGN(tmpMcastAddr.mod_pbmp[myModid], removePbmp);

        rv = usl_bcm_mcast_ports_add_remove(&tmpMcastAddr, USL_CMD_REMOVE);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;    
        }
        }

        BCM_PBMP_CLEAR(addPbmp);
        BCM_PBMP_ITER(shadowMcastAddr->mod_pbmp[myModid], port) 
        {
          /* Port present in Shadow elem but not Oper elem */
          if (!(BCM_PBMP_MEMBER(operMcastAddr->mod_pbmp[myModid], port)))
          {
            BCM_PBMP_PORT_ADD(addPbmp, port);
          }
        }

        if (BCM_PBMP_NOT_NULL(addPbmp))
        {
        /* Add all the ports in addPbmp to mcast group */
        memset(&tmpMcastAddr, 0, sizeof(tmpMcastAddr));
        memcpy(&tmpMcastAddr, shadowMcastAddr, sizeof(tmpMcastAddr));
          memset(tmpMcastAddr.mod_pbmp, 0, sizeof(tmpMcastAddr.mod_pbmp));
          memset(tmpMcastAddr.mod_pbmp, 0, sizeof(tmpMcastAddr.mod_pbmp));
        BCM_PBMP_ASSIGN(tmpMcastAddr.mod_pbmp[myModid], addPbmp);

        rv = usl_bcm_mcast_ports_add_remove(&tmpMcastAddr, USL_CMD_ADD);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;    
        }
      }
      
    }

      /* Update the Db only to reflect any pbmp changes for other units */      
      usl_db_l2mc_update_ports(USL_CURRENT_DB, shadowMcastAddr, USL_CMD_SET);

    }

  } while (0);

  return rv;

}

static L7_RC_t usl_l2mc_avl_tree_alloc(avlTree_t *mcastTreeData,
                                       avlTreeTables_t **mcastTreeHeap,
                                       usl_l2mcast_db_elem_t **mcastDataHeap)
{
  /* Assume failure */
  L7_RC_t rc = L7_FAILURE;

  do
  {
    *mcastTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                 sizeof(avlTreeTables_t)*platMfdbTotalMaxEntriesGet());

    if (*mcastTreeHeap == L7_NULLPTR ) break;

    *mcastDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                 sizeof(usl_l2mcast_db_elem_t)*platMfdbTotalMaxEntriesGet());

    if (*mcastDataHeap == L7_NULLPTR ) break;

    /* clear the memory before creating the tree */
    memset((void*)mcastTreeData, 0, sizeof(avlTree_t));
    memset((void*)*mcastTreeHeap, 0, 
           sizeof(avlTreeTables_t)  * platMfdbTotalMaxEntriesGet());
    memset((void*)*mcastDataHeap, 0, 
            sizeof(usl_l2mcast_db_elem_t) * platMfdbTotalMaxEntriesGet()); 

    avlCreateAvlTree(mcastTreeData, *mcastTreeHeap, *mcastDataHeap, 
                     platMfdbTotalMaxEntriesGet(),
                     sizeof(usl_l2mcast_db_elem_t), 0x10, sizeof(bcm_mac_t) + sizeof(bcm_vlan_t));

    rc = L7_SUCCESS;
 
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Create the USL L2 MC db
*
* @params   none
*
* @returns  none
*
* @notes    USL L2 MC database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_db_init(void)
{
  L7_RC_t          rc = L7_FAILURE;
  uslDbSyncFuncs_t l2mcDbFuncs;

  memset(&l2mcDbFuncs, 0, sizeof(l2mcDbFuncs));


  do
  {
    /* Allocate db only on stackables */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {

      pUslL2McastDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);

      if ( pUslL2McastDbSema == L7_NULLPTR)  
      {
        break;
      }

      if (usl_l2mc_avl_tree_alloc(&uslOperL2McastTreeData, &pUslOperL2McastTreeHeap,
                                  &pUslOperL2McastDataHeap) != L7_SUCCESS)
      {
        break;
      }


      /* Allocate shadow table for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_l2mc_avl_tree_alloc(&uslShadowL2McastTreeData, &pUslShadowL2McastTreeHeap,
                                    &pUslShadowL2McastDataHeap) != L7_SUCCESS)
        {
          break;
        }
      }
      
      /* Register the sync routines */
      l2mcDbFuncs.get_size_of_db_elem =  usl_get_size_of_l2mc_db_elem;
      l2mcDbFuncs.get_db_elem = usl_get_l2mc_db_elem;
      l2mcDbFuncs.delete_elem_from_db =  usl_delete_l2mc_db_elem;
      l2mcDbFuncs.print_db_elem = usl_print_l2mc_db_elem;
      l2mcDbFuncs.create_usl_bcm = usl_create_l2mc_db_elem_bcm;
      l2mcDbFuncs.delete_usl_bcm = usl_delete_l2mc_db_elem_bcm;
      l2mcDbFuncs.update_usl_bcm = usl_update_l2mc_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic 
      ** i.e no pointer member elements
      */
      l2mcDbFuncs.alloc_db_elem = L7_NULLPTR;
      l2mcDbFuncs.copy_db_elem  = L7_NULLPTR; 
      l2mcDbFuncs.free_db_elem  = L7_NULLPTR;
      l2mcDbFuncs.pack_db_elem  = L7_NULLPTR;
      l2mcDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L2_MCAST_DB_ID, &l2mcDbFuncs) != L7_SUCCESS)
      {
        LOG_ERROR(0);   
      }



      pUslL2McastTreeHandle = &uslOperL2McastTreeData;
      uslL2McDbActive = L7_TRUE;
    } /* End if stacking */

    rc = L7_SUCCESS; 

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  De-allocate the USL L2 MC db
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_db_fini(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if ( pUslL2McastDbSema != L7_NULLPTR )
  {
    osapiSemaDelete(pUslL2McastDbSema);
    pUslL2McastDbSema = L7_NULLPTR; 
  }

  do
  {
    uslL2McDbActive = L7_FALSE;
    pUslL2McastTreeHandle = L7_NULLPTR;

    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
      /* free the L2 mcast resources */
      avlDeleteAvlTree(&uslOperL2McastTreeData);

      if ( pUslOperL2McastTreeHeap != L7_NULLPTR )
      {
        osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperL2McastTreeHeap);
        pUslOperL2McastTreeHeap = L7_NULLPTR;
      }

      if ( pUslOperL2McastDataHeap != L7_NULLPTR )
      {
        osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperL2McastDataHeap);
        pUslOperL2McastDataHeap = L7_NULLPTR;
      }

      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                                L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        /* free the L2 mcast resources */
        avlDeleteAvlTree(&uslShadowL2McastTreeData);

        if ( pUslShadowL2McastTreeHeap != L7_NULLPTR )
        {
          osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowL2McastTreeHeap);
          pUslShadowL2McastTreeHeap = L7_NULLPTR;
        }

        if ( pUslShadowL2McastDataHeap != L7_NULLPTR )
        {
          osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowL2McastDataHeap);
          pUslShadowL2McastDataHeap = L7_NULLPTR;
        }
      }

    } /* End if stacking */

       
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Invalidate the content of the USL L2MC db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 max;

  if (uslL2McDbActive == L7_FALSE)
  {
    rc = L7_SUCCESS;
    return rc;      
  }

  USL_L2MC_DB_LOCK_TAKE();

  do
  {

    max = platMfdbTotalMaxEntriesGet();

    /* Clear the operational table */
    if (flags & USL_OPERATIONAL_DB)
    {
      avlPurgeAvlTree(&uslOperL2McastTreeData, max);
    }

    /* Clear the shadow table */
    if (flags & USL_SHADOW_DB)
    {
      avlPurgeAvlTree(&uslShadowL2McastTreeData, max);
    }
    
    rc = L7_SUCCESS;

  } while (0);

  USL_L2MC_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the L2Mc Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_l2mc_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_L2MC_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslL2McastTreeHandle = &uslOperL2McastTreeData;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslL2McastTreeHandle = &uslShadowL2McastTreeData;
  }

  USL_L2MC_DB_LOCK_GIVE();

  return;
}

void usl_l2mc_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8               uslStr[765];
  usl_l2mcast_db_elem_t   elem, nextElem;
  uslDbElemInfo_t         searchElem, elemInfo;
  avlTree_t              *dbHandle;
  L7_uint32               entryCount = 0;

  if (uslL2McDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L2 Mc database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "L2 Mc database is active\n");
  }

  dbHandle = usl_l2mc_db_handle_get(dbType);
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

  memset(&elem, 0, sizeof(elem));
  memset(&nextElem, 0, sizeof(nextElem));

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l2mc_db_elem(dbType, USL_DB_NEXT_ELEM, 
                              searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_l2mc_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                  uslStr);
    memcpy(searchElem.dbElem, elemInfo.dbElem, sizeof(usl_l2mcast_db_elem_t));
    entryCount++;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                "Total number of entries in the table: %d\n",
                entryCount);
  return;
}

/*********************************************************************
* @purpose  Initialize L2MC hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_hw_id_generator_init(void)
{
  L7_uint32 max;

  /* Application max Lag count */
  max = platMfdbTotalMaxEntriesGet();


  /* There is currently no API to get the range of instance number that SDK expects.
  ** Until such API is available we use the index from 0 to max number of 
  ** Lags supported by FASTPATH
  */
  uslL2McHwIdMin = 0;
  uslL2McHwIdMax = max - 1;

#ifdef L7_WIRELESS_PACKAGE
  /* With wireless package and soc_feature_wlan, L2 MC entries go
  ** into IPMC table.
  */
  {
    L7_BOOL wlan_feature = L7_FALSE, bcm_unit;

    for (bcm_unit=0; bcm_unit < soc_ndev; bcm_unit++)
    {
      if soc_feature(bcm_unit, soc_feature_wlan)
      {
        wlan_feature = L7_TRUE;
        break;
      }
    }

    if (wlan_feature == L7_TRUE)
    {
      uslL2McHwIdMin = platMrtrRoutesMaxEntriesGet();
      uslL2McHwIdMax = uslL2McHwIdMin + platMfdbTotalMaxEntriesGet();
    }
  }
#endif 


  pUslL2McHwIdList = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                 sizeof(usl_l2mc_hw_id_list_t) * (uslL2McHwIdMax + 1));
  if (pUslL2McHwIdList == L7_NULLPTR) 
    return L7_FAILURE;

  memset(pUslL2McHwIdList, 0, sizeof(usl_l2mc_hw_id_list_t) * (uslL2McHwIdMax + 1));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset L2Mc hw id generator
*
* @params   none
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_hw_id_generator_reset()
{
  L7_RC_t rc = L7_SUCCESS;

  /* Clear the TrunkHwIdList array */
  memset(pUslL2McHwIdList, 0, sizeof(usl_l2mc_hw_id_list_t) * (uslL2McHwIdMax + 1));

  return rc;
}

/*********************************************************************
* @purpose  Synchronize the L2Mc hwId generator with contents of Oper Db
*
* @params   None
*
* @returns  L7_RC_t. Assumes BCMX is suspended.
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_hw_id_generator_sync()
{
  usl_l2mcast_db_elem_t searchElem, l2mcElem;
  uslDbElemInfo_t       searchInfo, elemInfo;
  L7_int32             l2mcIndex, hwIndex;


  memset(&l2mcElem, 0, sizeof(l2mcElem));
  memset(&searchElem, 0, sizeof(searchElem));

  searchInfo.dbElem = (void *) &searchElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &l2mcElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_l2mc_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM,
                              searchInfo, &elemInfo) == BCM_E_NONE)
  {
    memcpy(searchInfo.dbElem, elemInfo.dbElem, 
           sizeof(usl_l2mcast_db_elem_t));
    memcpy(&(searchInfo.elemIndex), &(elemInfo.elemIndex), 
           sizeof(elemInfo.elemIndex));

    l2mcIndex = ((usl_l2mcast_db_elem_t *)elemInfo.dbElem)->bcm_data.l2mc_index;

    
    hwIndex = _BCM_MULTICAST_ID_GET(l2mcIndex);   
    if ((hwIndex < uslL2McHwIdMin) || (hwIndex > uslL2McHwIdMax))
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Invalid L2McIndex %d \n",
                  hwIndex);
      continue;    
    }

    /* Mark this index as used in StgHwList */    
    pUslL2McHwIdList[hwIndex].used = L7_TRUE;
       
  }

  return L7_SUCCESS;
}

void usl_l2mc_hw_id_generator_dump()
{ 
  L7_uint32 idx, entryCount = 0;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "L2Mc Min Index %d Max Index %d Used Hw Indexes: ",
                uslL2McHwIdMin, uslL2McHwIdMax);
  for (idx = uslL2McHwIdMin; idx <= uslL2McHwIdMax; idx++)
  {
    if (pUslL2McHwIdList[idx].used == L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "%d, ", idx);
      entryCount++;
    }
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\n");

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                "Total number of used hw indexes: %d\n",
                entryCount);

  return;
}

/*********************************************************************
* @purpose  Allocate a hw l2mc index for a given group
*
* @param    groupInfo @{(input)}  L2MC group
* @param    index     @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_l2mc_hw_id_allocate(usl_bcm_mcast_addr_t *mcAddr, L7_int32 *group)
{
  int                     rv = BCM_E_FULL, dbRv;
  L7_int32                idx = uslL2McHwIdMin, hwIndex = 0;
  usl_bcm_mcast_addr_t   *groupInfo;
  usl_l2mcast_db_elem_t   searchElem, elem;
  uslDbElemInfo_t         searchInfo, elemInfo;

  *group = 0;

  memset(&searchElem, 0, sizeof(searchElem));
  memset(&elem, 0, sizeof(elem));
  memcpy(&(searchElem.bcm_data), mcAddr, sizeof(*mcAddr));

  searchInfo.dbElem = &searchElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &elem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  do
  {
    /* If USL is in warm start state, then search the operational db for this group.
    ** If group is not found then an unused index is generated.
    */
    
    if (usl_state_get() == USL_WARM_START_STATE)
    {
      dbRv = usl_get_l2mc_db_elem(USL_OPERATIONAL_DB, USL_DB_EXACT_ELEM, 
                                  searchInfo, &elemInfo);

      if (dbRv == BCM_E_NONE) /* Group found */
      {
        groupInfo = &(((usl_l2mcast_db_elem_t *)elemInfo.dbElem)->bcm_data);
        *group = groupInfo->l2mc_index;
        
        hwIndex = _BCM_MULTICAST_ID_GET(*group);   
        if ((hwIndex > uslL2McHwIdMax) || (hwIndex < uslL2McHwIdMin))
        {
          LOG_ERROR(*group);    
        }

        /* Mark this index as used in HwList */ 
        pUslL2McHwIdList[hwIndex].used = L7_TRUE;
        rv = BCM_E_NONE;
      }
    }

    if (rv != BCM_E_NONE)
    {
    for (idx = uslL2McHwIdMin; idx <= uslL2McHwIdMax; idx++)
    {
      /* Found an unused index */
      if (pUslL2McHwIdList[idx].used == L7_FALSE)
      {
        pUslL2McHwIdList[idx].used = L7_TRUE;    
          hwIndex = idx;
        rv = BCM_E_NONE;
        break;
      }
    }
    }


#ifdef L7_WIRELESS_PACKAGE

    /* With wireless package and soc_feature_wlan, L2 MC entries go
    ** into IPMC table.
    */
    if (rv == BCM_E_NONE)
    {
      L7_BOOL wlan_feature = L7_FALSE, bcm_unit;

      for (bcm_unit=0; bcm_unit < soc_ndev; bcm_unit++)
      { 
        if soc_feature(bcm_unit, soc_feature_wlan)
        {
          wlan_feature = L7_TRUE;
          break;
        }
      }
 
      if (wlan_feature == L7_TRUE)
      {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_WLAN, hwIndex);
      }
      else
      {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2, hwIndex);
      }
    }
#else
    if (rv == BCM_E_NONE)
    {
      _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2, hwIndex);
    }
#endif

  } while(0);


  return rv;
}

/*********************************************************************
* @purpose  Free a given L2 mc index
*
* @param    index   @{(input)} L2 Mc index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_l2mc_hw_id_free(L7_int32 group)
{
  int rv = BCM_E_NONE, hwIndex;


  hwIndex = _BCM_MULTICAST_ID_GET(group);   

  if ((hwIndex < uslL2McHwIdMin) || (hwIndex > uslL2McHwIdMax))
  {
    rv = BCM_E_FAIL;
    return rv;
  }
  else
  {
    pUslL2McHwIdList[hwIndex].used = L7_FALSE;
  }

  return rv;
}

