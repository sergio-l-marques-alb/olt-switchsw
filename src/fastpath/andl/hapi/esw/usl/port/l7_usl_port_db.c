
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_port_db.c
*
* @purpose    USL port table implementation
*
* @component  USL
*
* @comments   none
*
* @create     11/3/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "avl_api.h"
#include "osapi.h"
#include "l7_usl_port_db.h"
#include "l7_usl_port_db_int.h"
#include "l7_usl_bcm.h"
#include "l7_usl_sm.h"
#include "l7_usl_trace.h"
#include "l7_rpc_policy.h"
#include "unitmgr_api.h"
#include "l7_usl_l2_db.h"
#include "pfc_exports.h"


/* Indicates portdb initialization is complete */
static L7_BOOL             uslPortDbInited = L7_FALSE;

void                       *uslPortDbSema = L7_NULLPTR;

/* Indicates portdb is active */
L7_BOOL                    uslPortDbActive = L7_FALSE;

/* USL port database tree. Each element contains configurations for a GPORT. */
avlTree_t                     uslOperPortDbTree;
avlTreeTables_t              *uslOperPortDbTreeHeap;
usl_port_db_elem_t           *uslOperPortDbDataHeap;

avlTree_t                      uslShadowPortDbTree;
avlTreeTables_t               *uslShadowPortDbTreeHeap;
usl_port_db_elem_t            *uslShadowPortDbDataHeap;

avlTree_t                     *uslPortDbTreeHandle = L7_NULLPTR;

/* Current number of valid port entries in the port database */
static L7_uint32          uslOperPortDbNumEntries;
static L7_uint32          uslShadowPortDbNumEntries;
static L7_uint32         *pUslCurrDbNumEntries;

/* Map to store the local bcmunit/bcmport to Gport mapping */
static bcm_gport_t        uslGportMap[L7_MAX_BCM_DEVICES_PER_UNIT][L7_MAX_BCM_PORTS_PER_DEVICE];

/* Number of times conversion betweeb Gport to Bcm Unit/Port (& vice versa)
 * failed. The error counts can be printed using usl_portdb_show
 */
L7_uint32                  uslBcmUnitPortToGportConversionErrors = 0;
L7_uint32                  uslGportToBcmUnitPortConversionErrors = 0;

L7_RC_t usl_portdb_update_msg_send(L7_BOOL updateCmd, L7_uint32 targetFpUnit, bcmx_lplist_t *lpList);
L7_RC_t usl_port_db_invalidate(USL_DB_TYPE_t dbType);
L7_int32 usl_update_port_db_elem_bcm(void *shadowDbItem, void *operDbItem);

/*********************************************************************
* @purpose  Get the port db handle based on db type
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t * usl_port_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperPortDbTree;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowPortDbTree;
      break;

    case USL_CURRENT_DB:
      dbHandle = uslPortDbTreeHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Allocate memory for a port db elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
void * usl_alloc_port_db_elem()
{
  usl_port_db_elem_t *elem;

  do
  {
    elem = (usl_port_db_elem_t *) osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                              sizeof(usl_port_db_elem_t));
    if (elem == L7_NULLPTR)
    {
      USL_LOG_ERROR("Unable to allocate memory for portdb elem\n");
    }

    elem->portPoliciesMask = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                         USL_PORT_DB_POLICY_MASK_SIZE);
    if (elem->portPoliciesMask == L7_NULLPTR)
    {
      USL_LOG_ERROR("Unable to allocate memory for portdb elem\n");  
    }

    elem->portPoliciesChangeMask = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                               USL_PORT_DB_POLICY_MASK_SIZE);
    if (elem->portPoliciesChangeMask == L7_NULLPTR)
    {
      USL_LOG_ERROR("Unable to allocate memory for portdb elem\n");  
    }

  } while (0);
  
  return (void *) elem;
}

/*********************************************************************
* @purpose  Free memory for a given port db elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
void usl_free_port_db_elem(void *item)
{
  usl_port_db_elem_t *elem = item;

  if (elem != L7_NULLPTR)
  {
    if (elem->portPoliciesMask != L7_NULLPTR)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, 
                elem->portPoliciesMask);
    }

    if (elem->portPoliciesChangeMask != L7_NULLPTR)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, 
                elem->portPoliciesChangeMask);
    }

    osapiFree(L7_DRIVER_COMPONENT_ID, 
              elem);
  } 
  
  return ;
}

/*********************************************************************
* @purpose  Get the size of Port Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_port_db_elem(void)
{
  return (sizeof(usl_port_db_elem_t) + (2 * USL_PORT_DB_POLICY_MASK_SIZE));
}

/*********************************************************************
* @purpose  Copy the contents of src db elem to dst db elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
void usl_copy_port_db_elem(void *pDst, 
                           void *pSrc)
{
  usl_port_db_elem_t *pDstPort = pDst, *pSrcPort = pSrc;

  memcpy(&(pDstPort->data), &(pSrcPort->data), sizeof(uslPortDbStaticElem_t));
  memcpy(pDstPort->portPoliciesMask, pSrcPort->portPoliciesMask, 
         USL_PORT_DB_POLICY_MASK_SIZE);
  memcpy(pDstPort->portPoliciesChangeMask, pSrcPort->portPoliciesChangeMask, 
         USL_PORT_DB_POLICY_MASK_SIZE);

  return;
}
/*********************************************************************
* @purpose  Print the contents of a port db element in specified buffer
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
void usl_print_port_db_elem(void *item, L7_uchar8 *buffer,
                            L7_uint32 size)
{
  usl_port_db_elem_t  *pUslPortEntry = item;


  osapiSnprintf(buffer, size,
                "Gport %x\n",
                pUslPortEntry->data.key.gport);
  return;
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
L7_int32 usl_get_port_db_elem(USL_DB_TYPE_t     dbType, 
                              L7_uint32         flags,
                              uslDbElemInfo_t   searchElem,
                              uslDbElemInfo_t  *elemInfo)
{
  L7_int32               rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry,*searchEntry;
  avlTree_t             *dbHandle = L7_NULLPTR;

  if (!uslPortDbActive)
  {
    return rv;    
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  searchEntry = (usl_port_db_elem_t *) searchElem.dbElem;

  USL_PORT_DB_LOCK_TAKE();

  do
  {

    dbHandle = usl_port_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;    
    }

    if (flags == USL_DB_NEXT_ELEM)
    {
      pUslPortEntry = avlSearchLVL7(dbHandle, searchEntry, AVL_NEXT);
    }
    else
    {
      /* Search for the elem */
      pUslPortEntry = usl_portdb_port_record_find(dbType, 
                                                  searchEntry->data.key.gport);
    }

    if (pUslPortEntry != L7_NULLPTR)
    {
      /* Element found, copy in the user memory */
      usl_copy_port_db_elem(elemInfo->dbElem, pUslPortEntry);
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from Port Db
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
L7_int32 usl_delete_port_db_elem(USL_DB_TYPE_t   dbType, 
                                 uslDbElemInfo_t elemInfo)
{
  L7_int32              rv = BCM_E_NONE;
  L7_uint32            *numElem;
  L7_RC_t               rc;
  usl_port_db_elem_t   *elem = elemInfo.dbElem;
  

  if (!uslPortDbActive)
  {
    return rv;    
  }

  USL_PORT_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB) 
  {
    numElem = &uslOperPortDbNumEntries;    
  }
  else if (dbType == USL_SHADOW_DB)
  {
    numElem = &uslShadowPortDbNumEntries;       
  }
  else
  {
   numElem = pUslCurrDbNumEntries;
  }

  rc = usl_portdb_delete_port_record(dbType, 
                                     elem->data.key.gport,
                                     numElem);

  rv  = (rc == L7_SUCCESS) ? (BCM_E_NONE) : (BCM_E_FAIL);

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Create a given element in Port Db
*
* @params   dbItem {(input)}  Pointer to db item to be created
*
* @returns  BCM error code
*
* @notes    For port db, the new element creation should only happen
*           on Mgmt unit for connected unit ports. This step would
*           insert all the connected unit ports in mgmt unit Oper Db.
*
* @end
*********************************************************************/
L7_int32 usl_create_port_db_elem_bcm(void *dbItem)
{
  L7_uint32             thisFpUnit, mgrFpUnit;
  L7_RC_t               rc;
  usl_port_db_elem_t   *elem = dbItem, *pUslPortEntry;

  if ((rc = unitMgrNumberGet(&thisFpUnit)) != L7_SUCCESS)
  {
    USL_LOG_MSG(USL_E_LOG, "unitMgrNumberGet failed\n");
    return BCM_E_FAIL;  
  }

  if ((rc = unitMgrMgrNumberGet(&mgrFpUnit)) != L7_SUCCESS)
  {
    USL_LOG_MSG(USL_E_LOG, "unitMgrMgrNumberGet failed\n");
    return BCM_E_FAIL;  
  }

 
  /* This function should be called only on the Management unit */
  if (thisFpUnit != mgrFpUnit)
  {
    USL_LOG_MSG(USL_E_LOG, "Non-mgmt unit trying to create element for gport %x \n",
                elem->data.key.gport);
    return BCM_E_FAIL; 
  }

  /* On mgmt unit create record for the port in Oper Db and copy the contents */
  USL_PORT_DB_LOCK_TAKE();

  rc = usl_portdb_insert_port_record(USL_OPERATIONAL_DB, elem->data.key.gport,
                                     &uslOperPortDbNumEntries);
  if (rc != L7_SUCCESS)
  {
    USL_LOG_MSG(USL_E_LOG, "Failed to insert record for gport %x \n",
                elem->data.key.gport);
    USL_PORT_DB_LOCK_GIVE();
    return BCM_E_FAIL; 
  }

  pUslPortEntry = usl_portdb_port_record_find(USL_OPERATIONAL_DB, 
                                              elem->data.key.gport);

  if (pUslPortEntry == L7_NULLPTR)
  {
    USL_LOG_MSG(USL_E_LOG, "Failed to find record for gport %x \n",
                elem->data.key.gport);
    USL_PORT_DB_LOCK_GIVE();
    return BCM_E_FAIL; 
  }

  /* Copy the contents of the element in the inserted entry */
  usl_copy_port_db_elem(pUslPortEntry, elem);

  USL_PORT_DB_LOCK_GIVE();

  return BCM_E_NONE;
}


/*********************************************************************
* @purpose  Delete a given element from Port Db
*
* @params   dbItem {(input)}  Pointer to db item to be deleted
*
* @returns  BCM error code
*
* @notes    For port db, we should never get into this case
*
* @end
*********************************************************************/
L7_int32 usl_delete_port_db_elem_bcm(void *dbItem)
{
  usl_port_db_elem_t *elem = dbItem;

  USL_LOG_MSG(USL_E_LOG, "Got Port db delete element bcm for gport %x \n",
              elem->data.key.gport);

#if 0
  usl_portdb_delete_port_record(USL_OPERATIONAL_DB, elem->data.key.gport,
                                &uslOperPortDbNumEntries);
  return BCM_E_NONE; 
#else
  return BCM_E_FAIL;
#endif

}

/* Internal Api to allocate memory for port db */
static L7_RC_t usl_port_db_alloc(avlTree_t *portDbTree, avlTreeTables_t **portDbTreeHeap,
                                 usl_port_db_elem_t **portDbDataHeap)
{
  *portDbTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                (sizeof (avlTreeTables_t) * USL_PORT_DB_TREE_SIZE));
   if (*portDbTreeHeap == L7_NULLPTR)
   {
     return L7_FAILURE;    
   }

  *portDbDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                (sizeof (usl_port_db_elem_t) * USL_PORT_DB_TREE_SIZE));
   if (*portDbDataHeap == L7_NULLPTR)
   {
     return L7_FAILURE;    
   }


   /* clear the memory before creating the tree */
   memset((void*)portDbTree, 0, sizeof(avlTree_t));

   memset((void*)*portDbTreeHeap, 0, 
          (sizeof(avlTreeTables_t) * USL_PORT_DB_TREE_SIZE));

   memset((void*)*portDbDataHeap, 0 , 
          (sizeof(usl_port_db_elem_t) * USL_PORT_DB_TREE_SIZE)); 

   avlCreateAvlTree(portDbTree, 
                    *portDbTreeHeap, 
                    *portDbDataHeap, 
                    USL_PORT_DB_TREE_SIZE,
                    sizeof(usl_port_db_elem_t), 
                    0, 
                    sizeof(uslPortDbKey_t));


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize USL Port database
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
L7_RC_t usl_port_db_init()
{
  L7_RC_t            rc = L7_SUCCESS;
  uslDbSyncFuncs_t   portDbFuncs;

  memset(&portDbFuncs, 0, sizeof(portDbFuncs));


  /* Port db is allocated if stacking or PFC feature is present */
  if ((cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)  ||
      (cnfgrIsFeaturePresent(L7_PFC_COMPONENT_ID, 
                             L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) == L7_TRUE))
  {

    uslPortDbSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY);
    if (uslPortDbSema == L7_NULLPTR)
    {
      USL_LOG_ERROR("USL: unable to create Port Db lock\n");
    }

    if (usl_port_db_alloc(&uslOperPortDbTree, &uslOperPortDbTreeHeap,
                          &uslOperPortDbDataHeap) != L7_SUCCESS)
    {
      USL_LOG_ERROR("USL: unable to allocate memory for the Port database\n");
    }

    uslOperPortDbNumEntries = 0;

    /* Allocate Shadow Db for NSF */
    if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, 
                              L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
    {
      if (usl_port_db_alloc(&uslShadowPortDbTree, &uslShadowPortDbTreeHeap,
                            &uslShadowPortDbDataHeap) != L7_SUCCESS)
      {
        USL_LOG_ERROR("USL: unable to allocate memory for the Port database\n");
      }

       uslShadowPortDbNumEntries = 0;
    }

    memset(uslGportMap, BCM_GPORT_INVALID, sizeof(uslGportMap));

     /* Register the sync routines */
    portDbFuncs.get_size_of_db_elem =  usl_get_size_of_port_db_elem;
    portDbFuncs.get_db_elem = usl_get_port_db_elem;
    portDbFuncs.delete_elem_from_db =  usl_delete_port_db_elem;
    portDbFuncs.print_db_elem = usl_print_port_db_elem;
    portDbFuncs.create_usl_bcm = usl_create_port_db_elem_bcm;
    portDbFuncs.delete_usl_bcm = usl_delete_port_db_elem_bcm;
    portDbFuncs.update_usl_bcm = usl_update_port_db_elem_bcm;

    /* Registering optional routines as the db element 
    ** has pointer member elements
    */
    portDbFuncs.alloc_db_elem = usl_alloc_port_db_elem;
    portDbFuncs.copy_db_elem  = usl_copy_port_db_elem; 
    portDbFuncs.free_db_elem  = usl_free_port_db_elem;

    /* Not registering pack/unpack as portdb has custom sync 
    ** implementation.
    */
    portDbFuncs.pack_db_elem  = L7_NULLPTR;
    portDbFuncs.unpack_db_elem = L7_NULLPTR;

    if (usl_db_sync_func_table_register(USL_PORT_DB_ID, 
                                        &portDbFuncs) != L7_SUCCESS)
    {
      LOG_ERROR(0);   
    }
  
 
    uslPortDbTreeHandle = &uslOperPortDbTree;
    pUslCurrDbNumEntries = &uslOperPortDbNumEntries;
    uslPortDbInited = L7_TRUE;
    uslPortDbActive = L7_TRUE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Release all resources allocated during usl_portdb_init()
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were released
* @returns  L7_ERROR   - if any resourses were not released 
*
* @notes    Should not call this unless the an error occured during init or the
*           configurator is taking the us back to pre Phase 1   
*
* @end
*********************************************************************/
L7_RC_t usl_port_db_fini()
{
  L7_RC_t rc = L7_SUCCESS;

 /* This call will be sure to free any dynamically allocated
   resources in each port entry. */
  usl_port_db_invalidate(USL_OPERATIONAL_DB | USL_SHADOW_DB); 

  uslPortDbInited = L7_FALSE;
  uslPortDbActive = L7_FALSE;
  uslOperPortDbNumEntries = 0;
  uslShadowPortDbNumEntries = 0;

  memset(uslGportMap, BCM_GPORT_INVALID, sizeof(uslGportMap));

  avlDeleteAvlTree(&uslOperPortDbTree);

  if (uslOperPortDbTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperPortDbTreeHeap);
    uslOperPortDbTreeHeap = L7_NULLPTR;
  }

  if (uslOperPortDbDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, uslOperPortDbDataHeap);
    uslOperPortDbDataHeap = L7_NULLPTR;
  }

  avlDeleteAvlTree(&uslShadowPortDbTree);

  if (uslShadowPortDbTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowPortDbTreeHeap);
    uslShadowPortDbTreeHeap = L7_NULLPTR;
  }

  if (uslShadowPortDbDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, uslShadowPortDbDataHeap);
    uslShadowPortDbDataHeap = L7_NULLPTR;
  }
  
  return rc;
}

/*********************************************************************
* @purpose  Insert a new port entry in the local USL Port database
*
* @param    dbType      @{(input)} dbType to be inserted
* @param    gport       @{(input)}  Key of the port to be inserted
* @param    numEntries  @{(output)} Final number of entries in Db
*
* @returns  L7_SUCCESS - if the port was inserted
* @returns  L7_ERROR   - if the port was not inserted
*
* @notes    Internal Api. Assumes Port Db Lock is held.
*       
* @end
*********************************************************************/
L7_RC_t usl_portdb_insert_port_record(USL_DB_TYPE_t dbType, 
                                      bcm_gport_t   gport,
                                      L7_uint32   *numEntries)
{
  L7_uchar8          *portPolicyMask;
  L7_RC_t             rc = L7_SUCCESS;
  usl_port_db_elem_t  portEntry, *pUslPortEntry;
  avlTree_t          *dbHandle = L7_NULLPTR;

  memset(&portEntry, 0, sizeof(portEntry));
  portEntry.data.key.gport = gport;

  if ((dbHandle = usl_port_db_handle_get(dbType)) == L7_NULLPTR)
  {
    return L7_FAILURE;    
  }
  
  /* Search if the port already exists */
  pUslPortEntry = avlSearchLVL7(dbHandle, &portEntry, AVL_EXACT);

  /* If the port entry already exists, then delete it */
  if (pUslPortEntry != L7_NULLPTR)
  {
    usl_portdb_delete_port_record(dbType, gport, numEntries);
  }

  /* Allocate the port policy mask based on BROAD_MAX_POLICIES. */
  portPolicyMask = osapiMalloc(L7_DRIVER_COMPONENT_ID, USL_PORT_DB_POLICY_MASK_SIZE);
  if (portPolicyMask == L7_NULL)
  {
    USL_LOG_ERROR("USL: Can't allocate policy mask for port %d\n", gport);
  }
  portEntry.portPoliciesMask = portPolicyMask;

  /* Allocate the port policy mask based on BROAD_MAX_POLICIES. */
  portPolicyMask = osapiMalloc(L7_DRIVER_COMPONENT_ID, USL_PORT_DB_POLICY_MASK_SIZE);
  if (portPolicyMask == L7_NULL)
  {
    USL_LOG_ERROR("USL: Can't allocate policy mask for port %d\n", gport);
  }
  portEntry.portPoliciesChangeMask = portPolicyMask;

  /* Egress filter mode is always enabled */
  portEntry.data.filterMode.flags |= BCM_PORT_VLAN_MEMBER_EGRESS;

  pUslPortEntry = avlInsertEntry(dbHandle, (void *)&portEntry);

  if (pUslPortEntry != L7_NULLPTR) /* Item was not inserted */
  {
    USL_LOG_ERROR("USL: Failed to insert port %d in USL database\n", gport);
  }
 
  /* Search the entry to find the item inserted */
  pUslPortEntry = avlSearchLVL7(dbHandle, &portEntry, AVL_EXACT);
  if (L7_NULLPTR == pUslPortEntry)
  {
    USL_LOG_ERROR("USL: Failed to insert port %d in USL database\n", gport);
  }

  (*numEntries)++;
  
  return rc;
}

/*********************************************************************
* @purpose  Delete a port entry from the local USL Port database
*
* @param    dbType      @{(input)} dbType to be updated
* @param    gport       @{(input)}  Key of the port to be removed
* @param    numEntries  @{(output)} Final number of entries in Db
*
*
* @returns  L7_SUCCESS - if the port was deleted
* @returns  L7_ERROR   - if the port was not deleted
*
* @notes    Internal Api. Assumes Port Db Lock is held.
*       
* @end
*********************************************************************/
L7_RC_t usl_portdb_delete_port_record(USL_DB_TYPE_t dbType,
                                      bcm_gport_t   gport, 
                                      L7_uint32    *numEntries)
{
  L7_RC_t               rc = L7_SUCCESS;
  usl_port_db_elem_t   *pUslPortEntry;
  uslPortDbKey_t        portKey;
  avlTree_t            *dbHandle = L7_NULLPTR;

  portKey.gport = gport;

  if ((dbHandle = usl_port_db_handle_get(dbType)) == L7_NULLPTR)
  {
    return L7_FAILURE;    
  }
  
  pUslPortEntry = avlSearchLVL7(dbHandle, &portKey, AVL_EXACT);
  if (pUslPortEntry != L7_NULL)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslPortEntry->portPoliciesMask);
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslPortEntry->portPoliciesChangeMask);
  }

  pUslPortEntry = avlDeleteEntry(dbHandle, &portKey);

  if (pUslPortEntry != L7_NULLPTR)
  {
    (*numEntries)--;
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete all the port entries from the local USL Port database
*
* @param    dbType  {(input)} Type of db to invalidate
*
* @returns  L7_SUCCESS - if all the ports were deleted
* @returns  L7_FAILURE - An error was encountered during deletion
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usl_port_db_invalidate(USL_DB_TYPE_t dbType)
{
  L7_RC_t          rc = L7_SUCCESS;
  usl_port_db_elem_t *pUslPortEntry, portEntry;

  if (!uslPortDbActive)
  {
    return rc;
  }

  USL_PORT_DB_LOCK_TAKE();

  if (dbType & USL_OPERATIONAL_DB)
  {
    memset(&portEntry, 0, sizeof(portEntry));    
    while ((pUslPortEntry = 
              avlSearchLVL7(&uslOperPortDbTree, &portEntry , AVL_NEXT)) != L7_NULLPTR)
    {
      memcpy(&portEntry, pUslPortEntry, sizeof(portEntry));
      usl_portdb_delete_port_record(USL_OPERATIONAL_DB, portEntry.data.key.gport,
                                    &uslOperPortDbNumEntries);
    }
      
    memset(uslGportMap, BCM_GPORT_INVALID, sizeof(uslGportMap));
    
  }

  if (dbType & USL_SHADOW_DB)
  {
    memset(&portEntry, 0, sizeof(portEntry));
    while ((pUslPortEntry = 
              avlSearchLVL7(&uslShadowPortDbTree, &portEntry , AVL_NEXT)) != L7_NULLPTR)
    {
      memcpy(&portEntry, pUslPortEntry, sizeof(portEntry));
      usl_portdb_delete_port_record(USL_SHADOW_DB, portEntry.data.key.gport,
                                    &uslShadowPortDbNumEntries);
    }
      
  }

  USL_PORT_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the port db handle to Operational or Shadow table
*
* @param    dbType  {(input)} Type of db to set
*
* @returns  L7_SUCCESS - if all the ports were deleted
* @returns  L7_FAILURE - An error was encountered during deletion
*
*       
* @end
*********************************************************************/
L7_RC_t usl_port_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  L7_RC_t rc = L7_SUCCESS;

  /* If trying to set the dbHandle to Shadow tables when they are not allocated */
  if ((dbType == USL_SHADOW_DB) &&
      (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, 
                             L7_STACKING_NSF_FEATURE_ID) == L7_FALSE))
  {
    rc = L7_FAILURE;
    return rc;    
  }

  USL_PORT_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    uslPortDbTreeHandle = &uslOperPortDbTree;
    pUslCurrDbNumEntries = &uslOperPortDbNumEntries;    
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslCurrDbNumEntries = &uslShadowPortDbNumEntries;
    uslPortDbTreeHandle = &uslShadowPortDbTree;        
  }
  else
  {
    rc = L7_FAILURE;
  }
  
  USL_PORT_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Add/Reomve the ports from the PortDb
*
* @param    updateCmd     @{(input)}  L7_TRUE: Adds the port to db
*                                     L7_FALSE: Remove the port from db
* @param    gport         @{(input)}  Port to be updated in the
*                                     USL port database
*
* @returns  L7_SUCCESS - if the ports were updated
* @returns  L7_FAILURE - if the ports were not updated
*
*       
* @end
*********************************************************************/
static L7_RC_t usl_portdb_update_port_record(L7_BOOL updateCmd, 
                                             bcm_gport_t gport)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Insert the port key in the USL database */
  if (updateCmd == L7_TRUE)
  {
    /* Do not insert in the Oper Db during warm start */
    if (usl_state_get() != USL_WARM_START_STATE)
    {
      if ((rc = usl_portdb_insert_port_record(USL_OPERATIONAL_DB,
                                              gport,
                                              &uslOperPortDbNumEntries)) != L7_SUCCESS)
      {
        USL_LOG_MSG(USL_E_LOG,
                   "Failed to insert gport %d in USL Oper port database\n", gport);
      }
    }

    if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
    {
      if ((rc = usl_portdb_insert_port_record(USL_SHADOW_DB,
                                              gport,
                                              &uslShadowPortDbNumEntries)) != L7_SUCCESS)
      {
        USL_LOG_MSG(USL_E_LOG,
                    "Failed to insert gport %d in USL Shadow port database\n", gport);
      }
          
    }

  }
  /* Remove the port key from the USL database */
  else
  {
    if ((rc = usl_portdb_delete_port_record(USL_OPERATIONAL_DB, 
                                            gport,
                                            &uslOperPortDbNumEntries)) != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_E_LOG,
                  "Failed to delete gport %d from USL Oper port database\n", gport);
    }

    if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                              L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
    {
      if ((rc = usl_portdb_delete_port_record(USL_SHADOW_DB, 
                                              gport,
                                              &uslShadowPortDbNumEntries)) != L7_SUCCESS)
      {
        USL_LOG_MSG(USL_E_LOG,
                    "Failed to delete gport %d from USL Shadow port database\n", gport);
      }
    }
  }

  return rc;
}

static void usl_gport_map_update(L7_BOOL updateCmd,
                                 bcm_gport_t gport, 
                                 L7_uint32 localBcmUnit,
                                 L7_uint32 bcmPort)
{
  if (updateCmd == L7_TRUE)
  {
    uslGportMap[localBcmUnit][bcmPort] = gport;
  }
  else
  {
    uslGportMap[localBcmUnit][bcmPort] = BCM_GPORT_INVALID;
  }

  return;
}

/*********************************************************************
* @purpose  Add/Reomve the ports in lpList from local and targetFpUnit 
*           USL Port database  
*
* @param    updateCmd     @{(input)}  L7_TRUE: Adds the ports to db
*                                     L7_FALSE: Remove the ports from db
* @param    targetFpUnit  @{(input)}  Unit to send the message
* @param    lpList        @{(input)}  List of ports to be updated in the
*                                     USL port database
*
* @returns  L7_SUCCESS - if the ports were updated
* @returns  L7_FAILURE - if the ports were not updated
*
* @notes    This function should be called on Management unit only.
*       
* @end
*********************************************************************/
L7_RC_t usl_portdb_update(L7_BOOL updateCmd, L7_uint32 targetFpUnit, 
                          bcmx_lplist_t *lpList)
{
  L7_RC_t           rc = L7_SUCCESS;
  L7_uint32         thisFpUnit, mgrFpUnit;
  L7_int32          i, rv;
  bcmx_lport_t      lport;
  L7_uint32         globalBcmUnit, localBcmUnit = 0, bcmPort = 0;

  if ((rc = unitMgrNumberGet(&thisFpUnit)) != L7_SUCCESS)
  {
    return rc;  
  }

  if ((rc = unitMgrMgrNumberGet(&mgrFpUnit)) != L7_SUCCESS)
  {
    return rc;  
  }

  /* This function should be called only on the Management unit */
  if (thisFpUnit != mgrFpUnit)
  {
    rc = L7_FAILURE;
    return rc; 
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Loop through the ports in the lpList and 
     update the gports in the local port database */
  for (i=0; i < BCMX_LPLIST_COUNT(lpList); i++)
  {
    lport = bcmx_lplist_index(lpList, i);

    if (BCMX_NO_SUCH_LPORT == lport)
      break;

    if (thisFpUnit == targetFpUnit) 
    {
      globalBcmUnit = BCMX_LPORT_BCM_UNIT(lport);
      rv = bcm_unit_remote_unit_get(globalBcmUnit, &localBcmUnit); 
      if (rv != BCM_E_NONE)
      {
        if (updateCmd == L7_TRUE)
        {
          USL_PORT_DB_LOCK_GIVE();
          rc = L7_FAILURE;
          return rc;
        }
        else
        {
          continue;
        }
      }

      bcmPort = BCMX_LPORT_BCM_PORT(lport);
      if ((localBcmUnit >= L7_MAX_BCM_DEVICES_PER_UNIT) || 
          (bcmPort >= L7_MAX_BCM_PORTS_PER_DEVICE))
      {
        if (updateCmd == L7_TRUE)
        {
          USL_PORT_DB_LOCK_GIVE();
          rc = L7_FAILURE;
          return rc;
        }
        else
        {
          continue;
        }
      }

      usl_gport_map_update(updateCmd, lport, localBcmUnit, bcmPort);
    }

    rc = usl_portdb_update_port_record(updateCmd, lport);

    if (rc != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Update cmd %d failed for gport %x\n",
                   updateCmd, lport);    
    }
    
  }

  USL_PORT_DB_LOCK_GIVE();

  /* Send the message to remote unit to populate the USL port database */
  if (thisFpUnit != targetFpUnit) 
  {
#ifdef L7_STACKING_PACKAGE
    unitMgrStatus_t unitStatus;  
    L7_RC_t         result;
    
    result = unitMgrUnitStatusGet(targetFpUnit, &unitStatus);
    if ((result == L7_SUCCESS) && (unitStatus == L7_UNITMGR_UNIT_OK))
    {
      rc = usl_portdb_update_msg_send(updateCmd, targetFpUnit, lpList);    
      if (rc != L7_SUCCESS)
      {
        USL_LOG_MSG(USL_E_LOG,
                    "Failed to send port update message to fpUnit %d\n", targetFpUnit);
      }
    }
#else
    LOG_ERROR(0);
#endif
  }

  return rc;
  
}

#ifdef L7_STACKING_PACKAGE

/*********************************************************************
* @purpose  Process the portdb update message on remote units 
*
* @param    msg  @{(input)}  The msg to be processed
*
* @returns  L7_SUCCESS - if the message was successfully processed
* @returns  L7_FAILURE - if errors were encountered in message processing
*
* @notes    Should be only called on remote units.
*       
* @end
*********************************************************************/
L7_RC_t usl_portdb_update_msg_process(L7_uchar8 *msg)
{
  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32   numElems, fromFpUnit, mgrFpUnit;
  L7_uchar8   *msgPtr;
  bcm_gport_t gport;
  L7_BOOL     updateCmd;
  L7_uint32   localBcmUnit, bcmPort, respUnitMask;

  numElems = *(L7_uint32 *) &msg[USL_MSG_ELEM_OFFSET];
  fromFpUnit = *(L7_uint32 *) &msg[USL_MSG_UNIT_OFFSET];
  msgPtr   = &msg[USL_MSG_DATA_OFFSET];

  /* Get the updateCmd */
  memcpy(&updateCmd, msgPtr, sizeof(updateCmd));
  msgPtr += sizeof(updateCmd);

  if ((rc = unitMgrMgrNumberGet(&mgrFpUnit)) != L7_SUCCESS)
  {
    USL_LOG_MSG(USL_E_LOG, "unitMgrMgrNumberGet failed\n");
    return rc;  
  }
  
  USL_PORT_DB_LOCK_TAKE();

  /* Insert each gport in the message to the local port database */
  while (numElems-- > 0)
  {
    memcpy(&gport, msgPtr, sizeof(gport));
    msgPtr += sizeof(gport); 
 
    memcpy(&localBcmUnit, msgPtr, sizeof(localBcmUnit));
    msgPtr += sizeof(localBcmUnit); 

    memcpy(&bcmPort, msgPtr, sizeof(bcmPort));
    msgPtr += sizeof(bcmPort); 


    usl_gport_map_update(updateCmd, gport, localBcmUnit, bcmPort);

    rc = usl_portdb_update_port_record(updateCmd, gport);

    if (rc != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Update cmd %d failed for gport %x\n",
                   updateCmd, gport);    
    }
    
  }

  USL_PORT_DB_LOCK_GIVE();

  respUnitMask = msMessageMcastAck(L7_MS_DRIVER_USL_REGISTRAR_ID, fromFpUnit,
                                   sizeof(L7_RC_t), (L7_uchar8 *) &rc, L7_FALSE);
  if (MS_UNIT_ISMASKBITSET(respUnitMask, fromFpUnit) != 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
            "USL : Failed to ack portdb update message from unit %d\n",
            fromFpUnit);   
  }

  return rc;
}

/*********************************************************************
* @purpose  Send a portdb update message to remote unit 
*
* @param    updateCmd     @{(input)}  L7_TRUE: Adds the ports to db
*                                     L7_FALSE: Remove the ports from db
* @param    targetFpUnit  @{(input)}  Unit to send the message
* @param    lpList        @{(input)}  List of ports to be send in the message
*
* @returns  L7_SUCCESS - if the message was successfully sent
* @returns  L7_FAILURE - if errors were encountered in sending message
*
* @notes    Should be only called on manager unit.
*       
* @end
*********************************************************************/
L7_RC_t usl_portdb_update_msg_send(L7_BOOL updateCmd, L7_uint32 targetFpUnit, 
                                   bcmx_lplist_t *lpList)
{
  L7_uchar8      *msg, *msgPtr;
  L7_uint32       msgSize, thisFpUnit, unit;
  L7_uint32       maxElems, numElems, respUnitMask = 0, sendUnitMask = 0;
  L7_uint32       globalBcmUnit, localBcmUnit, bcmPort;
  L7_int32        i, rv;
  L7_uint32       unitResp[L7_MAX_UNITS_PER_STACK + 1];
  msMcastData_t   respData[L7_MAX_UNITS_PER_STACK + 1];
  L7_RC_t         rc = L7_SUCCESS;
  bcmx_lport_t    lport;
  bcm_gport_t     gport;

  if ((rc = unitMgrNumberGet(&thisFpUnit)) != L7_SUCCESS)
  {
    USL_LOG_MSG(USL_E_LOG, "unitMgrNumberGet failed\n");
    return rc;  
  }

  memset(unitResp, 0, sizeof(unitResp));
  memset(respData, 0, sizeof(respData));
  for (unit = 0; unit <= L7_MAX_UNITS_PER_STACK; unit++)
  {
    respData[unit].buf = (L7_uchar8 *) &unitResp[unit];    
    respData[unit].buf_size = sizeof(L7_uint32);
  }

  if ((msg = usl_control_tx_buffer_alloc()) == L7_NULLPTR)
    USL_LOG_ERROR("USL: Tx buffer not allocated for USL_PORTDB_CREATE_PORTS\n");

  msgSize = usl_control_tx_buffer_size();
  memset(msg, 0, msgSize);

  /* Calculate max number of elements that can be sent in the msg */
  maxElems = (usl_control_tx_buffer_size() - (USL_MSG_SIZE_HDR + sizeof(updateCmd))) / 
                  (sizeof(bcm_gport_t) + sizeof(localBcmUnit) + sizeof(bcmPort));

  if (maxElems == 0)
  {
    USL_LOG_ERROR("USL: Tx message size is too small for portdb create msg\n");
    usl_tx_buffer_free(msg);
  }

  /* setup the message initially */
  *(L7_uint32 *)&msg[USL_MSG_TYPE_OFFSET] = USL_PORTDB_UPDATE;
  msgPtr = &msg[USL_MSG_DATA_OFFSET];
  numElems = 0;

  /* Pack the updateCmd at the begining of data-offset */
  memcpy(msgPtr, &updateCmd, sizeof(updateCmd));
  msgPtr += sizeof(updateCmd);

  /* Loop through the ports in the lpList and insert the gports in the message */
  for (i=0; i < BCMX_LPLIST_COUNT(lpList); i++)
  {
    lport = bcmx_lplist_index(lpList, i);

    if (BCMX_NO_SUCH_LPORT == lport)
      break;

    gport = (bcm_gport_t)lport;

    globalBcmUnit = BCMX_LPORT_BCM_UNIT(lport);
    rv = bcm_unit_remote_unit_get(globalBcmUnit, &localBcmUnit); 
    if (rv != BCM_E_NONE)
    {
      if (updateCmd == L7_TRUE)
      {
        LOG_ERROR(rv);    
      }
      else
        continue;
    }

    bcmPort = BCMX_LPORT_BCM_PORT(lport);
    if ((localBcmUnit >= L7_MAX_BCM_DEVICES_PER_UNIT) || 
        (bcmPort >= L7_MAX_BCM_PORTS_PER_DEVICE))
    {
      if (updateCmd == L7_TRUE)
      {
        LOG_ERROR(rv);    
      }
      else
        continue;
    }

    memcpy(msgPtr, &gport, sizeof(gport));
    msgPtr += sizeof(gport);

    memcpy(msgPtr, &localBcmUnit, sizeof(localBcmUnit));
    msgPtr += sizeof(localBcmUnit);

    memcpy(msgPtr, &bcmPort, sizeof(bcmPort));
    msgPtr += sizeof(bcmPort);

    numElems++;

     if (numElems == maxElems)
     {
       *(L7_uint32 *) &msg[USL_MSG_UNIT_OFFSET] = thisFpUnit;
       *(L7_uint32 *) &msg[USL_MSG_ELEM_OFFSET] = numElems;
        
       MS_UNIT_SETMASKBIT(sendUnitMask, targetFpUnit);
       respUnitMask = msMessageMcast(L7_MS_DRIVER_USL_REGISTRAR_ID, sendUnitMask, msgPtr - msg,
                                     msg, L7_FALSE, respData, USL_PORT_DB_UPDATE_TIMEOUT);

       if (MS_UNIT_ISMASKBITSET(respUnitMask, targetFpUnit) == L7_FALSE)
       {
         L7_LOGF(L7_DRIVER_COMPONENT_ID, L7_LOG_SEVERITY_ERROR,
                 "Failed to send port db update msg to unit %d\n",
                 targetFpUnit);    
       }
 
        numElems = 0;
        memset(msg, 0, msgSize);
        *(L7_uint32 *)&msg[USL_MSG_TYPE_OFFSET] = USL_PORTDB_UPDATE;
        msgPtr = &msg[USL_MSG_DATA_OFFSET];

        /* Pack the updateCmd at the begining of data-offset */
        memcpy(msgPtr, &updateCmd, sizeof(updateCmd));
        msgPtr += sizeof(updateCmd);
     }
  } 

  if (numElems > 0)
  {
    L7_UINT32_SET(&msg[USL_MSG_UNIT_OFFSET], thisFpUnit);
    L7_UINT32_SET(&msg[USL_MSG_ELEM_OFFSET], numElems);
    
    MS_UNIT_SETMASKBIT(sendUnitMask, targetFpUnit);
    respUnitMask = msMessageMcast(L7_MS_DRIVER_USL_REGISTRAR_ID, sendUnitMask, msgPtr - msg,
                                  msg, L7_FALSE, respData, USL_PORT_DB_UPDATE_TIMEOUT);

    if (MS_UNIT_ISMASKBITSET(respUnitMask, targetFpUnit) == L7_FALSE)
    {
       L7_LOGF(L7_DRIVER_COMPONENT_ID, L7_LOG_SEVERITY_ERROR,
               "Failed to send port db update msg to unit %d\n",
               targetFpUnit);    
    }
  }

  usl_control_tx_buffer_free(msg);

  return rc;
}
#endif

void usl_portdb_detail_show(usl_port_db_elem_t *pUslPortEntry)
{
  L7_uint32 i, j;
  L7_uint32 byteIdx, bitIdx;

  if (pUslPortEntry == L7_NULLPTR)
  {
    return; 
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "bcast-rate flags %d bcast-rate limit %d\n",
                pUslPortEntry->data.bcastRateLimit.flags, 
                pUslPortEntry->data.bcastRateLimit.limit); 

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nmcast-rate flags %d mcast-rate limit %d\n",
                pUslPortEntry->data.mcastRateLimit.flags, 
                pUslPortEntry->data.mcastRateLimit.limit); 

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\ndlf-rate flags %d dlf-rate limit %d\n",
                pUslPortEntry->data.dlfRateLimit.flags, 
                pUslPortEntry->data.dlfRateLimit.limit); 

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nfilterMode %d\n",
                 pUslPortEntry->data.filterMode.flags);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\npriority %d\n",
                 pUslPortEntry->data.priority);
                
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nmaxFrameSize %d\n",
                pUslPortEntry->data.maxFrameSize);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nlearnMode %d\n",
                pUslPortEntry->data.learnMode);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\ndtagMode %d\n",
                pUslPortEntry->data.dtagMode);

  for (i = 0; i < USL_MAX_TPIDS_PER_PORT; i++)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "\ntpidValid[%d] %d\n",
                  i, pUslPortEntry->data.tpidValid[i]);
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "\ntpidValue[%d] 0x%x\n",
                  i, pUslPortEntry->data.tpidValue[i]);
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\npvid %d\n",
                pUslPortEntry->data.pvid);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\ndiscardMode %d\n",
                pUslPortEntry->data.discardMode);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nCosq Sched config - mode %d delay %d\n",
                 pUslPortEntry->data.cosqSchedConfig.mode, 
                 pUslPortEntry->data.cosqSchedConfig.delay);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Cosq weights - ");
  for (i = 0; i < BCM_COS_COUNT; i++)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "cos %d weight %d minKbps %d maxKbps %d",
                  i,
                  pUslPortEntry->data.cosqSchedConfig.weights[i],
                  pUslPortEntry->data.cosqSchedConfig.minKbps[i],
                  pUslPortEntry->data.cosqSchedConfig.maxKbps[i]);
  }
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\n");

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nShaper rate %d burst %d\n",
                pUslPortEntry->data.shaperConfig.rate, pUslPortEntry->data.shaperConfig.burst);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nFlow-control rx %d tx %d pauseMac %x:%x:%x:%x:%x:%x\n",
                USL_PORTDB_FLAG_IS_INDEX_SET(pUslPortEntry->data.portFlags, USL_PORT_FLAG_PAUSE_RX) ? 1 : 0,
                USL_PORTDB_FLAG_IS_INDEX_SET(pUslPortEntry->data.portFlags, USL_PORT_FLAG_PAUSE_TX) ? 1 : 0,
                pUslPortEntry->data.pauseMacAddr[0], pUslPortEntry->data.pauseMacAddr[1], 
                pUslPortEntry->data.pauseMacAddr[2], pUslPortEntry->data.pauseMacAddr[3],
                pUslPortEntry->data.pauseMacAddr[4], pUslPortEntry->data.pauseMacAddr[5]);
                
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nPhy config \n");
  for (i = 0; i < BCM_PORT_MEDIUM_COUNT; i++)
  {
    if (pUslPortEntry->data.phyConfig[i].valid == L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "medium %d enable %d preferred %d \n",
                     i, pUslPortEntry->data.phyConfig[i].mediumConfig.enable,
                     pUslPortEntry->data.phyConfig[i].mediumConfig.preferred);

      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "autoneg_enable %d autoneg_advert %d \n",
                     pUslPortEntry->data.phyConfig[i].mediumConfig.autoneg_enable,
                     pUslPortEntry->data.phyConfig[i].mediumConfig.autoneg_advert);

      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "force-speed %d force-duplex %d master %d mdix %d\n",
                     pUslPortEntry->data.phyConfig[i].mediumConfig.force_speed,
                     pUslPortEntry->data.phyConfig[i].mediumConfig.force_duplex,
                     pUslPortEntry->data.phyConfig[i].mediumConfig.master,
                     pUslPortEntry->data.phyConfig[i].mediumConfig.mdix);

    }
      
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nVlan membership mask\n");
  
  for (i = 1, j = 0; i < L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (L7_VLAN_ISMASKBITSET(pUslPortEntry->data.vlanConfig.vlan_membership_mask, i) )
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "  %d, ",  i );

      j++;
      if (j%10 == 0 )   
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
      }
    }
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nVlan tagged mask\n");
  
  for (i = 1, j = 0; i < L7_PLATFORM_MAX_VLAN_ID; i++)
  {
    if (L7_VLAN_ISMASKBITSET(pUslPortEntry->data.vlanConfig.vlan_tag_mask, i) )
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "  %d, ",  i );

      j++;
      if (j%10 == 0 )   
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
      }
    }
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nDot1s states\n");

  for (i = 0; i < HAPI_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    if (pUslPortEntry->data.dot1sConfig[i].valid)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "Stg %d state %d\n", i, pUslPortEntry->data.dot1sConfig[i].dot1sState);
    }
        
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nPbvlan table\n");
  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    if (pUslPortEntry->data.pbvlanTable[i].etherType != 0)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "ethertype %d frametype %d vid %d\n",
                    pUslPortEntry->data.pbvlanTable[i].etherType,
                    pUslPortEntry->data.pbvlanTable[i].frameType,
                    pUslPortEntry->data.pbvlanTable[i].vid);
        
    }
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nDot1x state %s\n",
                USL_PORTDB_FLAG_IS_INDEX_SET(pUslPortEntry->data.portFlags, USL_PORT_FLAG_DOT1X_STATE) 
                  ? "UNAUTHORIZED" : "AUTHORIZED" );
                

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Mirroring configuration: flags %x probePort %d \n", 
                pUslPortEntry->data.mirrorConfig.flags,
                pUslPortEntry->data.mirrorConfig.probePort);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "VLAN Control configuration: VLAN xlate enable %d\n"
                "                            VLAN miss drop enable %d\n"
                "                            VLAN xlate egress enable %d\n"
                "                            VLAN xlate egress miss drop %d\n"
                "                            VLAN xlate key first %d\n"
                "                            VLAN xlate key second %d\n",
                pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateIngressEnable,
                pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateIngressMissDrop,
                pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateEgressEnable,
                pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateEgressMissDrop,
                pUslPortEntry->data.vlanControlConfig.bcmVlanPortTranslateKeyFirst,
                pUslPortEntry->data.vlanControlConfig.bcmVlanPortTranslateKeySecond);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nPolicy mask: ");
  for (i = 0; i < BROAD_MAX_POLICIES; i++)
  {
    byteIdx = i / 8;
    bitIdx  = i % 8;

    if (pUslPortEntry->portPoliciesMask[byteIdx] & (1 << bitIdx))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "%d ", i);
    }
  }
               
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nPolicy Changed mask: ");
  for (i = 0; i < BROAD_MAX_POLICIES; i++)
  {
    byteIdx = i / 8;
    bitIdx  = i % 8;

    if (pUslPortEntry->portPoliciesChangeMask[byteIdx] & (1 << bitIdx))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "%d ", i);
    }
  }
               
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\nValid index mask: ");

  for (i = 1; i < USL_BCMX_PORT_SET_CMD_LAST; i++)
  {
    if (USL_PORTDB_VALID_MASK_IS_INDEX_SET(pUslPortEntry->data.portValidMask, i))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "%d ", i);
    }
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\n");

  return;
  
}
/*********************************************************************
* @purpose  Dump the local USL Port database
*
* @param    L7_int32   @{(input)} gport of the portentry to be printed. 
*                                 A value of -1 indicates dump all the 
*                                 entries.
* @param    L7_BOOL   @{(input)} Print detailed info
* @returns  none
*
* @notes   
*       
* @end
*********************************************************************/
void usl_portdb_show(L7_int32 gport, L7_BOOL detail)
{
  usl_port_db_elem_t *pUslPortEntry, portEntry;
  L7_uint32        bcmUnit, bcmPort;

  memset(&portEntry, 0, sizeof(portEntry));

  USL_PORT_DB_LOCK_TAKE();

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Total number of entries in port database %d\n",
                uslOperPortDbNumEntries);

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"GportToBcmUnitPortErrors = %d "
                "BcmUnitPortToGportErrors = %d\n",
                uslGportToBcmUnitPortConversionErrors,
                uslBcmUnitPortToGportConversionErrors);
 

  if (gport == -1) /* Show all port entries */
  {
    while ((pUslPortEntry = avlSearchLVL7(uslPortDbTreeHandle, &portEntry , AVL_NEXT)) != L7_NULLPTR)
    {
      memcpy(&portEntry, pUslPortEntry, sizeof(portEntry));
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Gport %d (0x%8.8x)\n", pUslPortEntry->data.key.gport, pUslPortEntry->data.key.gport); 
      if (detail == L7_TRUE)
      {
        usl_portdb_detail_show(pUslPortEntry);
      }
    }

    for (bcmUnit = 0; bcmUnit < L7_MAX_BCM_DEVICES_PER_UNIT; bcmUnit++)
      for (bcmPort = 0; bcmPort < L7_MAX_BCM_PORTS_PER_DEVICE; bcmPort++)
    {
      if (uslGportMap[bcmUnit][bcmPort] != BCM_GPORT_INVALID)
      {
        printf("Gport %d (0x%8.8x) bcmUnit %d bcmPort %d\n", 
               uslGportMap[bcmUnit][bcmPort], uslGportMap[bcmUnit][bcmPort], bcmUnit, bcmPort); 
      }
    }
  }
  else
  {
    portEntry.data.key.gport = gport;
    pUslPortEntry = avlSearchLVL7(uslPortDbTreeHandle, &portEntry , AVL_EXACT);
    if (pUslPortEntry != L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Gport %d found in the Port database\n");
      if (detail == L7_TRUE)
      {
        usl_portdb_detail_show(pUslPortEntry);
      }
        
    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Gport %d not found in the Port database\n");
    }
  }

  USL_PORT_DB_LOCK_GIVE();
 
  return; 
  
}


/*********************************************************************
* @purpose  Return the Gport mapping for a local bcmUnit/bcmPort
*
* @param    localBcmUnit   @{(input)}  Local Bcm Unit number
* @param    bcmPort        @{(input)}  Local Bcm port number
* @param    gport          @{(output)} Gport
*
* @returns  BCM_E_NOT_FOUND: If gport mapping could not be found
*           BCM_E_NONE: Gport mapping found
*
* @notes    Returns BCM_GPORT_INVALID on standalone package as 
*           port db is not active on standalone. 
*       
* @end
*********************************************************************/
int usl_bcm_unit_port_to_gport(int localBcmUnit, bcm_port_t bcmPort, 
                               bcm_gport_t *gport)
{
  if ((cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)  ||
      (cnfgrIsFeaturePresent(L7_PFC_COMPONENT_ID, 
                             L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) == L7_TRUE))
  {
  int rv = BCM_E_NONE;

  if ((localBcmUnit >= L7_MAX_BCM_DEVICES_PER_UNIT) || 
      (bcmPort >= L7_MAX_BCM_PORTS_PER_DEVICE))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
           "Invalid bcmunit %d bcmport %d, cannot convert to gport\n", 
                  localBcmUnit, bcmPort);
    return BCM_E_FAIL;    
  }

  *gport = uslGportMap[localBcmUnit][bcmPort];

  if (*gport == BCM_GPORT_INVALID)
  {
    rv = BCM_E_NOT_FOUND;   
    ++uslBcmUnitPortToGportConversionErrors;
  }

  return rv;

  }
  else
  {
    *gport = BCM_GPORT_INVALID;
    return BCM_E_NONE;
  }

}

/*********************************************************************
* @purpose  Return the Gport mapping for a local bcmUnit/bcmPort
*
* @param    gport          @{(input)}   Gport
* @param    localBcmUnit   @{(output)}  Local Bcm Unit number
* @param    bcmPort        @{(output)}  Local Bcm port number
*
* @returns  BCM_E_NOT_FOUND: If gport mapping could not be found
*           BCM_E_NONE: Gport mapping found
*
* @notes
*       
* @end
*********************************************************************/
int usl_gport_to_bcm_unit_port(bcm_gport_t gport, int *localBcmUnit, bcm_port_t *bcmPort)
{
#ifdef L7_STACKING_PACKAGE
  int unitIdx, portIdx;

  if (gport == BCM_GPORT_INVALID)
    return BCM_E_NOT_FOUND;

  for (unitIdx = 0; unitIdx < L7_MAX_BCM_DEVICES_PER_UNIT; unitIdx++)
  {
    for (portIdx = 0; portIdx < L7_MAX_BCM_PORTS_PER_DEVICE; portIdx++)
    {
      if (gport == uslGportMap[unitIdx][portIdx])
      {
        *localBcmUnit = unitIdx;
        *bcmPort = portIdx;
        return BCM_E_NONE;
      }
    }
  }

  ++uslGportToBcmUnitPortConversionErrors;
  return BCM_E_NOT_FOUND;
#else
  return BCM_E_NOT_FOUND;
#endif
}

/*********************************************************************
* @purpose  Get a port's record in portdb
*
* @param    dbType  @{(input)} dbType to get the record    
* @param    gport   @{(input)}  Port number
*
* @returns  L7_NULL - If port could not be found in port-database
* @returns  Pointer to port's record in portdb
*
* @notes    Port Db lock should be taken before calling this
*           API.
*       
* @end
*********************************************************************/
usl_port_db_elem_t* usl_portdb_port_record_find(USL_DB_TYPE_t dbType, 
                                                bcm_gport_t gport)
{
  avlTree_t          *dbHandle = L7_NULLPTR;
  usl_port_db_elem_t *pUslPortEntry = L7_NULLPTR;
  uslPortDbKey_t      portKey;

  memcpy(&portKey, &gport, sizeof(portKey));

  if ((L7_FALSE == uslPortDbInited) || (L7_FALSE == uslPortDbActive))
  {
    return L7_NULLPTR;  
  }

  dbHandle = usl_port_db_handle_get(dbType);
  if (dbHandle == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }
  
  pUslPortEntry = avlSearchLVL7(dbHandle, &portKey, AVL_EXACT);
  if (pUslPortEntry == L7_NULLPTR)
  {
    /* Check if port-database supports storing the gport type */
  }

  return pUslPortEntry;

}



/* Print the bcast rate attribute in specified buffer */
void usl_port_print_bcast_rate(USL_DB_TYPE_t dbType,
                               usl_bcm_port_rate_limit_t *bcastLimit, 
                               L7_uchar8 *buffer,
                               L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Bcast rate %d flags %d\n",
                usl_db_type_name_get(dbType),
                bcastLimit->limit, bcastLimit->flags);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_policy_update_bcm(L7_int32            unit, 
                                    L7_int32            port,
                                    usl_port_db_elem_t *shadowElem,
                                    usl_port_db_elem_t *operElem)
{
  L7_int32    rv = BCM_E_NONE;
  L7_int32    tmprv;
  L7_uchar8   uslStr[128];
  L7_uint32   policyId;
  L7_uint32   byteIdx, bitIdx;
  L7_BOOL     policyApply, policyRemove;

  /* Update the Hw with Shadow value */
  if ((memcmp(&(shadowElem->portPoliciesMask), 
             &(operElem->portPoliciesMask),
             USL_PORT_DB_POLICY_MASK_SIZE) != 0) ||
      (memcmp(&(shadowElem->portPoliciesChangeMask), 
             &(operElem->portPoliciesChangeMask),
             USL_PORT_DB_POLICY_MASK_SIZE) != 0))
  {
    for (policyId = 0; policyId < BROAD_MAX_POLICIES; policyId++)
    {
      byteIdx = policyId / 8;
      bitIdx  = policyId % 8;

      policyApply  = L7_FALSE;
      policyRemove = L7_FALSE;

      if (((shadowElem->portPoliciesChangeMask[byteIdx] & (1 << bitIdx)) != 0) &&
          ((operElem->portPoliciesChangeMask[byteIdx]   & (1 << bitIdx)) == 0))
      {
        /* This port has been explicitly configured for this policy in just the
           shadow DB. Apply the shadow DB config. */
        if ((shadowElem->portPoliciesMask[byteIdx] & (1 << bitIdx)) != 0)
        {
          policyApply = L7_TRUE;
        }
        else
        {
          policyRemove = L7_TRUE;
        }
      }
      else if (((shadowElem->portPoliciesChangeMask[byteIdx] & (1 << bitIdx)) != 0) &&
               ((operElem->portPoliciesChangeMask[byteIdx]   & (1 << bitIdx)) != 0))
      {
        /* This port has been explicitly configured for this policy in both the
           shadow and operational DBs. */
        if (((shadowElem->portPoliciesMask[byteIdx] & (1 << bitIdx)) != 0) &&
            ((operElem->portPoliciesMask[byteIdx]   & (1 << bitIdx)) == 0))
        {
          /* If the policy is applied in the shadow table, but not the oper table,
             apply the policy to the port. */
          policyApply = L7_TRUE;
        }
        else if (((shadowElem->portPoliciesMask[byteIdx] & (1 << bitIdx)) == 0) &&
                 ((operElem->portPoliciesMask[byteIdx]   & (1 << bitIdx)) != 0))
        {
          /* If the policy is not applied in the shadow table, but is in the oper table,
             remove the policy from the port. */
          policyRemove = L7_TRUE;
        }
      }

      if (policyApply == L7_TRUE)
      {
        tmprv = usl_bcm_policy_port_apply(unit, policyId, port);
        sprintf(uslStr, "Applying policy %d to unit %d port %d, rv = %d", policyId, unit, port, tmprv);
        usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);
        if (tmprv < rv)
        {
          rv = tmprv;
        }
      }
      else if (policyRemove == L7_TRUE)
      {
        tmprv = usl_bcm_policy_port_remove(unit, policyId, port);
        sprintf(uslStr, "Removing policy %d from unit %d port %d, rv = %d", policyId, unit, port, tmprv);
        usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);
        if (tmprv < rv)
        {
          rv = tmprv;
        }
      }
    }
  }

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_bcast_rate_update_bcm(L7_int32 unit, L7_int32 port,
                                        usl_port_db_elem_t *shadowElem,
                                        usl_port_db_elem_t *operElem)
{
  L7_int32    rv = BCM_E_NONE;
  L7_uchar8   uslStr[128];

  usl_port_print_bcast_rate(USL_OPERATIONAL_DB,
                            &(operElem->data.bcastRateLimit), uslStr, 
                            sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  
  usl_port_print_bcast_rate(USL_SHADOW_DB,
                            &(shadowElem->data.bcastRateLimit), uslStr, 
                            sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.bcastRateLimit), 
             &(operElem->data.bcastRateLimit),
             sizeof(shadowElem->data.bcastRateLimit)) != 0)
  {
    
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");

    rv = usl_bcm_port_rate_bcast_set(unit, port, 
                                     &(shadowElem->data.bcastRateLimit));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the broadcast rate threshold parameter in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    bcast_limit      @{(input)}  Broadcast rate threshold params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_bcast_rate_set(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              usl_bcm_port_rate_limit_t *bcast_limit)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.bcastRateLimit), bcast_limit, 
           sizeof(pUslPortEntry->data.bcastRateLimit));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_BCAST_RATE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/* Print the mcast rate attribute in specified buffer */
void usl_port_print_mcast_rate(USL_DB_TYPE_t dbType,
                               usl_bcm_port_rate_limit_t *mcastLimit, 
                               L7_uchar8 *buffer,
                               L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Mcast rate %d flags %d\n",
                usl_db_type_name_get(dbType),
                mcastLimit->limit, mcastLimit->flags);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_mcast_rate_update_bcm(L7_int32 unit, L7_int32 port,
                                        usl_port_db_elem_t *shadowElem,
                                        usl_port_db_elem_t *operElem)
{
  L7_int32    rv = BCM_E_NONE;
  L7_uchar8   uslStr[128];

  usl_port_print_mcast_rate(USL_OPERATIONAL_DB,
                            &(operElem->data.mcastRateLimit), uslStr, 
                            sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_mcast_rate(USL_SHADOW_DB,
                            &(shadowElem->data.mcastRateLimit), uslStr, 
                            sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);


  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.mcastRateLimit),  
             &(operElem->data.mcastRateLimit),
             sizeof(shadowElem->data.mcastRateLimit)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");

    rv = usl_bcm_port_rate_mcast_set(unit, port, 
                                     &(shadowElem->data.mcastRateLimit));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the multicast rate threshold parameter in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    mcast_limit      @{(input)}  Multicast rate threshold params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_mcast_rate_set(USL_DB_TYPE_t dbType, 
                              bcm_gport_t gport, 
                              usl_bcm_port_rate_limit_t *mcast_limit)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.mcastRateLimit), mcast_limit, 
           sizeof(pUslPortEntry->data.mcastRateLimit));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_MCAST_RATE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/* Print the dlf rate attribute in specified buffer */
void usl_port_print_dlf_rate(USL_DB_TYPE_t dbType,
                             usl_bcm_port_rate_limit_t *dlfLimit, 
                             L7_uchar8 *buffer,
                             L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Dlf rate %d flags %d\n",
                usl_db_type_name_get(dbType),
                dlfLimit->limit, dlfLimit->flags);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_dlf_rate_update_bcm(L7_int32 unit, L7_int32 port,
                                      usl_port_db_elem_t *shadowElem,
                                      usl_port_db_elem_t *operElem)
{
  L7_int32    rv = BCM_E_NONE;
  L7_uchar8   uslStr[128];

  usl_port_print_dlf_rate(USL_OPERATIONAL_DB, 
                          &(operElem->data.dlfRateLimit), uslStr, 
                          sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_dlf_rate(USL_SHADOW_DB, 
                          &(shadowElem->data.dlfRateLimit), uslStr, 
                          sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.dlfRateLimit),  
             &(operElem->data.dlfRateLimit),
             sizeof(shadowElem->data.dlfRateLimit)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_rate_dlfbc_set(unit, port, 
                                     &(shadowElem->data.dlfRateLimit));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the dlf rate threshold parameter in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    dlfbc_limit      @{(input)}  Dlf rate threshold params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_dlfbc_rate_set(USL_DB_TYPE_t dbType, 
                              bcm_gport_t gport, 
                              usl_bcm_port_rate_limit_t *dlfbc_limit)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.dlfRateLimit), dlfbc_limit, 
           sizeof(pUslPortEntry->data.dlfRateLimit));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_DLF_RATE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/* Print the filter mode attribute in specified buffer */
void usl_port_print_filter_mode(USL_DB_TYPE_t dbType,
                                usl_bcm_port_filter_mode_t *mode, 
                                L7_uchar8 *buffer,
                                L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Filter mode %d\n",
                usl_db_type_name_get(dbType), mode->flags);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_filter_mode_update_bcm(L7_int32 unit, L7_int32 port,
                                         usl_port_db_elem_t *shadowElem,
                                         usl_port_db_elem_t *operElem)
{
  L7_int32                   rv = BCM_E_NONE;
  L7_uchar8                  uslStr[128];
  usl_bcm_port_filter_mode_t filterMode;

  memset(&filterMode, 0, sizeof(filterMode));

  usl_port_print_filter_mode(USL_OPERATIONAL_DB,
                             &(operElem->data.filterMode), uslStr, 
                             sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_filter_mode(USL_SHADOW_DB,
                             &(shadowElem->data.filterMode), uslStr, 
                             sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);


  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.filterMode),
             &(operElem->data.filterMode),
             sizeof(shadowElem->data.filterMode)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");

    if ((operElem->data.filterMode.flags & BCM_PORT_VLAN_MEMBER_EGRESS) &&
        ((shadowElem->data.filterMode.flags & BCM_PORT_VLAN_MEMBER_EGRESS) == L7_FALSE))
    {
      filterMode.flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
      filterMode.setFlags = L7_FALSE;
      rv = usl_bcm_port_vlan_member_set(unit, port, 
                                        &filterMode);    

    }

    if (((operElem->data.filterMode.flags & BCM_PORT_VLAN_MEMBER_EGRESS) == L7_FALSE) &&
        (shadowElem->data.filterMode.flags & BCM_PORT_VLAN_MEMBER_EGRESS))
    {
      filterMode.flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
      filterMode.setFlags = L7_TRUE;
      rv = usl_bcm_port_vlan_member_set(unit, port, 
                                        &filterMode);    
    }

    if ((operElem->data.filterMode.flags & BCM_PORT_VLAN_MEMBER_INGRESS) &&
        ((shadowElem->data.filterMode.flags & BCM_PORT_VLAN_MEMBER_INGRESS) == L7_FALSE))
    {
      filterMode.flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
      filterMode.setFlags = L7_FALSE;
      rv = usl_bcm_port_vlan_member_set(unit, port, 
                                        &filterMode);    

    }

    if (((operElem->data.filterMode.flags & BCM_PORT_VLAN_MEMBER_INGRESS) == L7_FALSE) &&
        (shadowElem->data.filterMode.flags & BCM_PORT_VLAN_MEMBER_INGRESS))
    {
      filterMode.flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
      filterMode.setFlags = L7_TRUE;
      rv = usl_bcm_port_vlan_member_set(unit, port, 
                                        &filterMode);    
    }

    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the filtering mode for the port in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    mode              @{(input)}   Filtering mode data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_filter_mode_set(USL_DB_TYPE_t dbType,
                               bcm_gport_t gport, 
                               usl_bcm_port_filter_mode_t *mode)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    if (mode->setFlags == L7_TRUE)
    {
      pUslPortEntry->data.filterMode.flags |= mode->flags;
    }
    else
    {
      pUslPortEntry->data.filterMode.flags &= (~(mode->flags));
    }
    
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_FILTER_MODE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the priority attribute in specified buffer */
void usl_port_print_priority(USL_DB_TYPE_t dbType,
                             usl_bcm_port_priority_t *priority, 
                             L7_uchar8 *buffer,
                             L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Priority %d\n",
                usl_db_type_name_get(dbType), *priority);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_priority_update_bcm(L7_int32 unit, L7_int32 port,
                                      usl_port_db_elem_t *shadowElem,
                                      usl_port_db_elem_t *operElem)
{
  L7_int32 rv = BCM_E_NONE;
  L7_uchar8   uslStr[128];

  usl_port_print_priority(USL_OPERATIONAL_DB,
                          &(operElem->data.priority), uslStr, 
                          sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_priority(USL_SHADOW_DB,
                          &(shadowElem->data.priority), uslStr, 
                          sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.priority), &(operElem->data.priority),
             sizeof(shadowElem->data.priority)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_untagged_priority_set(unit, port, 
                                            &(shadowElem->data.priority));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the priority for the port in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    priority           @{(input)}  Priority data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_priority_set(USL_DB_TYPE_t dbType,
                            bcm_gport_t gport, 
                            usl_bcm_port_priority_t *priority)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.priority), priority, 
            sizeof(pUslPortEntry->data.priority));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_PRIORITY_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the max frame size attribute in specified buffer */
void usl_port_print_max_frame_size(USL_DB_TYPE_t dbType,
                                   usl_bcm_port_frame_size_t *frameSize, 
                                   L7_uchar8 *buffer,
                                   L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Max frame size %d\n",
                usl_db_type_name_get(dbType), *frameSize);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_max_frame_update_bcm(L7_int32 unit, L7_int32 port,
                                       usl_port_db_elem_t *shadowElem,
                                       usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  usl_port_print_max_frame_size(USL_OPERATIONAL_DB,
                                &(operElem->data.maxFrameSize), uslStr, 
                                sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_max_frame_size(USL_SHADOW_DB,
                                &(shadowElem->data.maxFrameSize), uslStr, 
                                sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.maxFrameSize), 
             &(operElem->data.maxFrameSize),
             sizeof(shadowElem->data.maxFrameSize)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID,"Updating Hw\n");
    rv = usl_bcm_port_frame_max_set(unit, port, 
                                    &(shadowElem->data.maxFrameSize));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the max frame size for a port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    priority           @{(input)}  Max frame size
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_max_frame_set(USL_DB_TYPE_t dbType,
                             bcm_gport_t gport, 
                             usl_bcm_port_frame_size_t *max_frame_size)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.maxFrameSize), max_frame_size, 
           sizeof(pUslPortEntry->data.maxFrameSize));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_MAX_FRAME_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the learn mode attribute in specified buffer */
void usl_port_print_learn_mode(USL_DB_TYPE_t dbType,
                               usl_bcm_port_learn_mode_t *mode, 
                               L7_uchar8 *buffer,
                               L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Learn mode %d\n",
                usl_db_type_name_get(dbType), *mode);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_learn_mode_update_bcm(L7_int32 unit, L7_int32 port,
                                        usl_port_db_elem_t *shadowElem,
                                        usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  usl_port_print_learn_mode(USL_OPERATIONAL_DB,
                            &(operElem->data.learnMode), uslStr, 
                            sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_learn_mode(USL_SHADOW_DB,
                            &(shadowElem->data.learnMode), uslStr, 
                            sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.learnMode), &(operElem->data.learnMode),
             sizeof(shadowElem->data.learnMode)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID,"Updating Hw\n");
    rv = usl_bcm_port_learn_set(unit, port, 
                                &(shadowElem->data.learnMode));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the learn mode for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    learnMode          @{(input)}  Learn mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_learn_set(USL_DB_TYPE_t dbType,
                         bcm_gport_t gport, 
                         usl_bcm_port_learn_mode_t *learn_mode)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.learnMode), learn_mode, 
           sizeof(pUslPortEntry->data.learnMode));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_LEARN_MODE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the learn mode attribute in specified buffer */
void usl_port_print_dtag_mode(USL_DB_TYPE_t dbType,
                              usl_bcm_port_dtag_mode_t *mode, 
                              L7_uchar8 *buffer,
                              L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Dtag mode %d\n",
                usl_db_type_name_get(dbType), *mode);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_dtag_mode_update_bcm(L7_int32 unit, L7_int32 port,
                                       usl_port_db_elem_t *shadowElem,
                                       usl_port_db_elem_t *operElem)
{
  L7_int32 rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  usl_port_print_dtag_mode(USL_OPERATIONAL_DB,
                           &(operElem->data.dtagMode), uslStr, 
                            sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_dtag_mode(USL_SHADOW_DB,
                           &(shadowElem->data.dtagMode), uslStr, 
                           sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.dtagMode), &(operElem->data.dtagMode),
             sizeof(shadowElem->data.dtagMode)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_dtag_mode_set(unit, port, 
                                    &(shadowElem->data.dtagMode));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the dtag mode for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    dtagMode           @{(input)}  dtag mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_dtag_mode_set(USL_DB_TYPE_t dbType,
                             bcm_gport_t gport, 
                             usl_bcm_port_dtag_mode_t *dtag_mode)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.dtagMode), dtag_mode,
           sizeof(pUslPortEntry->data.dtagMode));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_DTAG_MODE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the learn mode attribute in specified buffer */
void usl_port_print_tpid(USL_DB_TYPE_t        dbType,
                         L7_uchar8           *tpidValid,
                         usl_bcm_port_tpid_t *tpidValue, 
                         L7_uchar8           *buffer,
                         L7_uint32            size)
{
  L7_uint32 i;

  osapiSnprintf(buffer, size,
                "%s val: tpidValid %d tpidValue %x\n",
                usl_db_type_name_get(dbType), tpidValid[0], tpidValue[0]);

  for (i = 0; i < USL_MAX_TPIDS_PER_PORT; i++)
  {
    osapiSnprintfcat(buffer, size,
                     "%s val: tpidValid %d tpidValue %x\n",
                     usl_db_type_name_get(dbType), tpidValid[i], tpidValue[i]);
  }
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_tpid_update_bcm(L7_int32 unit, L7_int32 port,
                                  usl_port_db_elem_t *shadowElem,
                                  usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE, final_rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];
  L7_uint32    shadowTpidIndex, operTpidIndex;
  L7_BOOL      shadowEntryFound;

  usl_port_print_tpid(USL_OPERATIONAL_DB,
                      operElem->data.tpidValid,
                      operElem->data.tpidValue,
                      uslStr, 
                      sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_tpid(USL_SHADOW_DB,
                      shadowElem->data.tpidValid,
                      shadowElem->data.tpidValue,
                      uslStr, 
                      sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  /* Process deletes first */
  for (operTpidIndex = 0; operTpidIndex < USL_MAX_TPIDS_PER_PORT; operTpidIndex++)
  {
    if (operElem->data.tpidValid[operTpidIndex])
    {
      /* If this TPID is not in the shadow entry, delete it. */
      shadowEntryFound = L7_FALSE;
      for (shadowTpidIndex = 0; shadowTpidIndex < USL_MAX_TPIDS_PER_PORT; shadowTpidIndex++)
      {
        if (shadowElem->data.tpidValid[shadowTpidIndex])
        {
          if (shadowElem->data.tpidValue[shadowTpidIndex] == operElem->data.tpidValue[operTpidIndex])
          {
            shadowElem->data.tpidValid[shadowTpidIndex] = 0; /* Mark as invalid in order to skip while processing the 'adds'. */
            shadowEntryFound = L7_TRUE;
            break;
          }
        }
      }
      if (shadowEntryFound == L7_FALSE)
      {
        usl_trace_sync_detail(USL_PORT_DB_ID, "Deleting TPID from Hw\n");
        rv = usl_bcm_port_tpid_delete(unit, port, &(operElem->data.tpidValue[operTpidIndex]));
        if (rv < final_rv)
        {
          final_rv = rv;
        }
      }
    }
  }

  /* Process Adds next */
  for (shadowTpidIndex = 0; shadowTpidIndex < USL_MAX_TPIDS_PER_PORT; shadowTpidIndex++)
  {
    if (shadowElem->data.tpidValid[shadowTpidIndex])
    {
      usl_trace_sync_detail(USL_PORT_DB_ID, "Adding TPID to Hw\n");
      rv = usl_bcm_port_tpid_add(unit, port, &(shadowElem->data.tpidValue[shadowTpidIndex]));
      if (rv < final_rv)
      {
        final_rv = rv;
      }
    }
  }

  return final_rv;
}

/*********************************************************************
* @purpose  Set the tpid for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    dtagMode           @{(input)}  dtag mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_tpid_set(USL_DB_TYPE_t        dbType,
                        bcm_gport_t          gport, 
                        usl_bcm_port_tpid_t *tpid)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;
  usl_bcm_port_tpid_t    tempTpid;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  memcpy(&tempTpid, tpid, sizeof(tempTpid)); 

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memset(pUslPortEntry->data.tpidValue, 0, sizeof(pUslPortEntry->data.tpidValue));
    memset(pUslPortEntry->data.tpidValid, 0, sizeof(pUslPortEntry->data.tpidValid));

    pUslPortEntry->data.tpidValid[0] = 1;
    pUslPortEntry->data.tpidValue[0] = tempTpid;

    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_TPID_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Add a tpid for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    dtagMode         @{(input)}  dtag mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_tpid_add(USL_DB_TYPE_t        dbType,
                        bcm_gport_t          gport, 
                        usl_bcm_port_tpid_t *tpid)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;
  usl_bcm_port_tpid_t    tempTpid;
  int                    tpidIndex, freeTpidIndex;
  L7_BOOL                tpidExists = L7_FALSE;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  memcpy(&tempTpid, tpid, sizeof(tempTpid)); 

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    freeTpidIndex = -1;
    for (tpidIndex = 0; tpidIndex < USL_MAX_TPIDS_PER_PORT; tpidIndex++)
    {
      if (pUslPortEntry->data.tpidValid[tpidIndex] == 0)
      {
        if (freeTpidIndex == -1)
        {
          freeTpidIndex = tpidIndex;
        }
      }
      else if (pUslPortEntry->data.tpidValue[tpidIndex] == tempTpid)
      {
        tpidExists = L7_TRUE;
        break;
      }
    }

    if (tpidExists == L7_FALSE)
    {
      if (freeTpidIndex != -1)
      {
        pUslPortEntry->data.tpidValid[freeTpidIndex] = 1;
        pUslPortEntry->data.tpidValue[freeTpidIndex] = tempTpid;

        USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                        USL_BCMX_PORT_TPID_SET);
      }
      else
      {
        rv = BCM_E_RESOURCE;
      }
    }
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a tpid from the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    dtagMode         @{(input)}  dtag mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_tpid_delete(USL_DB_TYPE_t        dbType,
                           bcm_gport_t          gport, 
                           usl_bcm_port_tpid_t *tpid)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;
  usl_bcm_port_tpid_t    tempTpid;
  int                    tpidIndex;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  memcpy(&tempTpid, tpid, sizeof(tempTpid)); 

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    for (tpidIndex = 0; tpidIndex < USL_MAX_TPIDS_PER_PORT; tpidIndex++)
    {
      if ((pUslPortEntry->data.tpidValid[tpidIndex] == 1) && 
          (pUslPortEntry->data.tpidValue[tpidIndex] == tempTpid))
      {
        pUslPortEntry->data.tpidValid[tpidIndex] = 0;
        pUslPortEntry->data.tpidValue[tpidIndex] = 0;

        USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                        USL_BCMX_PORT_TPID_SET);
      }
    }
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/* Print the pvid attribute in specified buffer */
void usl_port_print_pvid(USL_DB_TYPE_t dbType,
                         bcm_vlan_t *pvid, 
                         L7_uchar8 *buffer,
                         L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: pvid %d\n",
                usl_db_type_name_get(dbType), *pvid);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_pvid_update_bcm(L7_int32 unit, L7_int32 port,
                                  usl_port_db_elem_t *shadowElem,
                                  usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  usl_port_print_pvid(USL_OPERATIONAL_DB,
                      &(operElem->data.pvid), uslStr, 
                      sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_pvid(USL_SHADOW_DB,
                      &(shadowElem->data.pvid), uslStr, 
                      sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.pvid), &(operElem->data.pvid),
             sizeof(shadowElem->data.pvid)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_untagged_vlan_set(unit, port, 
                                        &(shadowElem->data.pvid));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the pvid for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    pvid               @{(input)}  Pvid
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_pvid_set(USL_DB_TYPE_t dbType,
                        bcm_gport_t gport, 
                        bcm_vlan_t *pvid)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.pvid), pvid, 
           sizeof(pUslPortEntry->data.pvid));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_PVID_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the discard mode attribute in specified buffer */
void usl_port_print_discard_mode(USL_DB_TYPE_t dbType,
                                 bcm_port_discard_t *mode, 
                                 L7_uchar8 *buffer,
                                 L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Discard mode %d\n",
                usl_db_type_name_get(dbType), *mode);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_discard_mode_update_bcm(L7_int32 unit, L7_int32 port,
                                          usl_port_db_elem_t *shadowElem,
                                          usl_port_db_elem_t *operElem)
{
  L7_int32 rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  usl_port_print_discard_mode(USL_OPERATIONAL_DB,
                              &(operElem->data.discardMode), uslStr, 
                              sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_discard_mode(USL_SHADOW_DB,
                              &(shadowElem->data.discardMode), uslStr, 
                              sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.discardMode), &(operElem->data.discardMode),
             sizeof(shadowElem->data.discardMode)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_discard_set(unit, port, 
                                  &(shadowElem->data.discardMode));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the discard mode for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    mode               @{(input)}  Discard mode
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_discard_set(USL_DB_TYPE_t dbType,
                           bcm_gport_t gport, 
                           bcm_port_discard_t *mode)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.discardMode), mode, 
           sizeof(pUslPortEntry->data.discardMode));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_DISCARD_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the phy config attribute in specified buffer */
void usl_port_print_phy_config(USL_DB_TYPE_t dbType,
                               usl_port_db_phy_config_t *config, 
                               L7_uchar8 *buffer,
                               L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s Phyconfig val: \n",
                usl_db_type_name_get(dbType));

  osapiSnprintfcat(buffer, size,
                   "Autoneg_advert %d an_enable %d enable %d force_duplex %d\n",
                   config->mediumConfig.autoneg_advert,
                   config->mediumConfig.autoneg_enable,
                   config->mediumConfig.enable,
                   config->mediumConfig.force_duplex);

  osapiSnprintfcat(buffer, size,
                   "force_speed %d master %d mdix %d preferred %d\n",
                   config->mediumConfig.force_speed,
                   config->mediumConfig.master,
                   config->mediumConfig.mdix,
                   config->mediumConfig.preferred);

  osapiSnprintfcat(buffer, size, 
                   "Advert_ability: flags %d interface %d loopback %d medium %d pause %d fd %d hd %d\n",
                   config->mediumConfig.advert_ability.flags,
                   config->mediumConfig.advert_ability.interface,
                   config->mediumConfig.advert_ability.loopback,
                   config->mediumConfig.advert_ability.medium,
                   config->mediumConfig.advert_ability.pause,
                   config->mediumConfig.advert_ability.speed_full_duplex,
                   config->mediumConfig.advert_ability.speed_half_duplex);
                       
  return;
}


/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_phy_config_update_bcm(L7_int32 unit, L7_int32 port,
                                        usl_port_db_elem_t *shadowElem,
                                        usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE, i;
  L7_uchar8    uslStr[512];
  L7_BOOL      operDbMediumValid, shadowDbMediumValid; 

  for (i = 0; i < BCM_PORT_MEDIUM_COUNT; i++)
  {
    operDbMediumValid = operElem->data.phyConfig[i].valid;
    shadowDbMediumValid = shadowElem->data.phyConfig[i].valid;

    if (shadowDbMediumValid == L7_FALSE)
    {
      osapiSnprintf(uslStr, sizeof(uslStr),
                    "Medium %d is not valid\n", i);
      usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);
      continue;
    }
    else
    {
      osapiSnprintf(uslStr, sizeof(uslStr),
                    "Medium %d data:\n", i);
      usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);
    }
    
    usl_port_print_phy_config(USL_OPERATIONAL_DB,
                              &(operElem->data.phyConfig[i]), uslStr, 
                              sizeof(uslStr));
    usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

    usl_port_print_phy_config(USL_SHADOW_DB,
                              &(shadowElem->data.phyConfig[i]), uslStr, 
                              sizeof(uslStr));
    usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);


    /* Compare and update the Hw with Shadow value */
    if (memcmp(&(shadowElem->data.phyConfig[i]), &(operElem->data.phyConfig[i]),
               sizeof(shadowElem->data.phyConfig[i])) != 0)
    {
      rv = usl_bcm_port_medium_config_set(unit, port, 
                                          i, &(shadowElem->data.phyConfig[i].mediumConfig));    
    }
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the medium config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    medium             @{(input)}  Phy medium
* @param    config             @{(input)}  Phy config data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_medium_config_set(USL_DB_TYPE_t dbType,
                                 bcm_gport_t gport, 
                                 bcm_port_medium_t medium,
                                 bcm_phy_config_t  *config)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    pUslPortEntry->data.phyConfig[medium].valid = L7_TRUE;
    memcpy(&(pUslPortEntry->data.phyConfig[medium].mediumConfig), config, 
           sizeof(*config));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_PHY_CONFIG_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Set the flow control config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    pauseConfig        @{(input)}  Flow-control config data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_flow_control_set(USL_DB_TYPE_t dbType,
                                bcm_gport_t gport, 
                                usl_bcm_port_pause_config_t *pauseConfig)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    if (pauseConfig->pauseTx)
    {
      USL_PORTDB_FLAG_INDEX_SET(pUslPortEntry->data.portFlags, 
                                USL_PORT_FLAG_PAUSE_TX);
    }
    else
    {
      USL_PORTDB_FLAG_INDEX_CLR(pUslPortEntry->data.portFlags, 
                                USL_PORT_FLAG_PAUSE_TX);
    }
 
    if (pauseConfig->pauseRx)
    {
      USL_PORTDB_FLAG_INDEX_SET(pUslPortEntry->data.portFlags, 
                                USL_PORT_FLAG_PAUSE_RX);
    }
    else
    {
      USL_PORTDB_FLAG_INDEX_CLR(pUslPortEntry->data.portFlags, 
                                USL_PORT_FLAG_PAUSE_RX);
    }

    memcpy(&(pUslPortEntry->data.pauseMacAddr), &(pauseConfig->pauseMacAddr), 
           sizeof(pUslPortEntry->data.pauseMacAddr));

    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_FLOW_CONTROL_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the cosq config attribute in specified buffer */
void usl_port_print_cosq_config(USL_DB_TYPE_t dbType,
                               usl_bcm_port_cosq_sched_config_t *config, 
                               L7_uchar8 *buffer,
                               L7_uint32 size)
{
  L7_uint32 i;

  osapiSnprintf(buffer, size,
                "%s Cosqconfig val: \n",
                usl_db_type_name_get(dbType));

  osapiSnprintfcat(buffer, size,
                   "Mode %d delay %d\n",
                   config->mode, config->delay);

  for (i = 0; i < BCM_COS_COUNT; i++)
  {
    osapiSnprintfcat(buffer, size,
                     "Cos %d: Weight %d minKbps %d maxKbps %d\n",
                     i, config->weights[i], 
                     config->minKbps[i], config->maxKbps[i]);
  }

  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_cosq_sched_update_bcm(L7_int32 unit, L7_int32 port,
                                        usl_port_db_elem_t *shadowElem,
                                        usl_port_db_elem_t *operElem)
{
  L7_int32 rv = BCM_E_NONE;
  L7_uchar8    uslStr[256];

  usl_port_print_cosq_config(USL_OPERATIONAL_DB,
                             &(operElem->data.cosqSchedConfig), uslStr, 
                             sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_cosq_config(USL_SHADOW_DB,
                             &(shadowElem->data.cosqSchedConfig), uslStr, 
                             sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.cosqSchedConfig), &(operElem->data.cosqSchedConfig),
             sizeof(shadowElem->data.cosqSchedConfig)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_cosq_sched_set(unit, port, 
                                     &(shadowElem->data.cosqSchedConfig));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the Cosq sched config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    cosqSchedConfig    @{(input)}  Cosq Sched config data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_cosq_sched_set(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              usl_bcm_port_cosq_sched_config_t *cosqSchedConfig)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.cosqSchedConfig), cosqSchedConfig, 
           sizeof(pUslPortEntry->data.cosqSchedConfig));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_COSQ_SCHED_CONFIG_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the shaper config attribute in specified buffer */
void usl_port_print_shaper_config(USL_DB_TYPE_t dbType,
                                  usl_bcm_port_shaper_config_t *config, 
                                  L7_uchar8 *buffer,
                                  L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s Shaper config val: \n",
                usl_db_type_name_get(dbType));

  osapiSnprintfcat(buffer, size,
                   "Rate %d burst %d\n",
                   config->rate, config->burst);

  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_shaper_config_update_bcm(L7_int32 unit, L7_int32 port,
                                           usl_port_db_elem_t *shadowElem,
                                           usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  usl_port_print_shaper_config(USL_OPERATIONAL_DB,
                               &(operElem->data.shaperConfig), uslStr, 
                               sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_shaper_config(USL_SHADOW_DB,
                               &(shadowElem->data.shaperConfig), uslStr, 
                               sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.shaperConfig), &(operElem->data.shaperConfig), 
             sizeof(shadowElem->data.shaperConfig)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_rate_egress_set(unit, port, 
                                      &(shadowElem->data.shaperConfig));    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the shaper config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    cosqSchedConfig    @{(input)}  Cosq Sched config data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_shaper_config_set(USL_DB_TYPE_t dbType,
                                 bcm_gport_t gport, 
                                 usl_bcm_port_shaper_config_t *shaperConfig)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.shaperConfig), shaperConfig, 
           sizeof(pUslPortEntry->data.shaperConfig));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_SHAPER_CONFIG_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the shaper config attribute in specified buffer */
void usl_port_print_vlan_membership_config(USL_DB_TYPE_t dbType,
                                           usl_bcm_port_vlan_t *config, 
                                           L7_uchar8 *buffer,
                                           L7_uint32 size)
{
  L7_uchar8 tmpMemMask, tmpTagMask;
  L7_uint32 vlanId = 0, k, pos;

  osapiSnprintf(buffer, size,
                "%s content: \n",
                usl_db_type_name_get(dbType));

  osapiSnprintfcat(buffer, size,
                   "Vlan membership \n");

  for (k = 0; k < L7_VLAN_INDICES; k++) 
  {
    pos = 0;
    tmpMemMask = config->vlan_membership_mask.value[k];
    tmpTagMask = config->vlan_tag_mask.value[k];
    while (tmpMemMask) 
    {
      pos++;
      if (tmpMemMask & 0x1) 
      {
        vlanId = (k*8) + pos;

        osapiSnprintfcat(buffer, size,
                         "%4d(%2s)  ", vlanId,
                         (tmpTagMask & 0x1) ? ("t") : ("u"));
      }

      tmpMemMask >>= 1;
      tmpTagMask >>= 1;
    }
  }


  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_vlan_config_update_bcm(L7_int32 unit, L7_int32 port,
                                         usl_port_db_elem_t *shadowElem,
                                         usl_port_db_elem_t *operElem)
{
  L7_uchar8    uslStr[1250];
  L7_uchar8    operMemMask, operTagMask;
  L7_uchar8    shadowMemMask, shadowTagMask;
  L7_uint32    vlanId = 0, k, bitpos, index;
  L7_int32     rv = BCM_E_NONE;
  L7_BOOL      tagged;

  usl_port_print_vlan_membership_config(USL_OPERATIONAL_DB,
                                        &(operElem->data.vlanConfig), uslStr, 
                                        sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_vlan_membership_config(USL_SHADOW_DB,
                                        &(shadowElem->data.vlanConfig), uslStr, 
                                        sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.vlanConfig), &(operElem->data.vlanConfig), 
             sizeof(shadowElem->data.vlanConfig)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    
    for (k = 0; k < L7_VLAN_INDICES; k++) 
    {
      shadowMemMask = shadowElem->data.vlanConfig.vlan_membership_mask.value[k];
      shadowTagMask = shadowElem->data.vlanConfig.vlan_tag_mask.value[k];

      operMemMask = operElem->data.vlanConfig.vlan_membership_mask.value[k];
      operTagMask = operElem->data.vlanConfig.vlan_tag_mask.value[k];
  
      if ((operMemMask != shadowMemMask) || (operTagMask != shadowTagMask))
      {
        index = 1;
        bitpos = 1;
        while (index <= 8)
        {
          vlanId = (k*8) + index;

          /* vlan present in oper but not shadow */
          if ((operMemMask & bitpos) && (!(shadowMemMask & bitpos)))
          {
            /* Ignore the return code as vlan might have been deleted */
            (void) usl_bcm_vlan_port_update(unit, port, USL_CMD_REMOVE, vlanId, L7_FALSE);
        
          }

          /* vlan present in shadow but not in oper */
          if ((shadowMemMask & bitpos) && (!(operMemMask & bitpos)))
          {
            /* Add the port to vlan */    
            tagged = (shadowTagMask & bitpos) ? (L7_TRUE) : (L7_FALSE);
            rv = usl_bcm_vlan_port_update(unit, port, USL_CMD_ADD, vlanId, tagged);
          }

          /* vlan present in both db, check tagging */
          if ((shadowMemMask & bitpos) && (operMemMask & bitpos))
          {
            /* Tag in Oper but not shadow */
            if ((operTagMask & bitpos) && (!(shadowTagMask & bitpos)))
            {
              rv = usl_bcm_vlan_port_update(unit, port, USL_CMD_ADD, vlanId, L7_FALSE);    
            }

            /* Tag in shadow but not in oper */
            if ((shadowTagMask & bitpos) && (!(operTagMask & bitpos)))
            {
              rv = usl_bcm_vlan_port_update(unit, port, USL_CMD_ADD, vlanId, L7_TRUE);        
            }

          }

          index++;
          bitpos <<= 1;
        } /* End While */
      } /* End if */
    } /* End for */
    
  } /* End if memcmp */

  return rv;
}

/*********************************************************************
* @purpose  Update the port vlan membership in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    cmd              @{(input)}  L7_TRUE: Add vlans to the port
*                                        L7_FALSE: Remove vlans from the port
* @param    vid              @{(input)}  Vlan-id
* @param    tagged           @{(input)}  Port is tagged
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_vlan_port_update(USL_DB_TYPE_t dbType,
                            bcm_gport_t gport, 
                            USL_CMD_t   cmd,
                            bcm_vlan_t  vid,
                            L7_BOOL     tagged)
{
  int                   rv = BCM_E_NONE;
  usl_port_db_elem_t   *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    if (cmd == USL_CMD_ADD)
    {
      L7_VLAN_SETMASKBIT(pUslPortEntry->data.vlanConfig.vlan_membership_mask, 
                         vid);     
      if (tagged == L7_TRUE)
      {
        L7_VLAN_SETMASKBIT(pUslPortEntry->data.vlanConfig.vlan_tag_mask, 
                           vid);     
      }
    }
    else
    {
      L7_VLAN_CLRMASKBIT(pUslPortEntry->data.vlanConfig.vlan_membership_mask, 
                         vid);     
      L7_VLAN_CLRMASKBIT(pUslPortEntry->data.vlanConfig.vlan_tag_mask, 
                         vid);     
    }

    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_VLAN_CONFIG_SET);
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

 
/*********************************************************************
* @purpose  Update the port vlan membership in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    vlanConfig         @{(input)}  Vlan config data
* @param    cmd                @{(input)}  L7_TRUE: Add vlans to the port
*                                          L7_FALSE: Remove vlans from the port
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_vlan_membership_update(USL_DB_TYPE_t dbType,
                                      bcm_gport_t gport, 
                                      usl_bcm_port_vlan_t *vlanConfig, 
                                      L7_BOOL cmd)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t *pUslPortEntry;
  L7_uint32           vlanIdx, pos, vlanId; 
  L7_uchar8           membMask, tagMask;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    for (vlanIdx = 0; vlanIdx < L7_VLAN_INDICES; vlanIdx++) 
    {
      membMask = vlanConfig->vlan_membership_mask.value[vlanIdx];
      tagMask  = vlanConfig->vlan_tag_mask.value[vlanIdx];
      pos      = 0;

      while (membMask) 
      {
        pos++;
        if (membMask & 0x1) 
        {
          vlanId = (vlanIdx*8) + pos;
          if (cmd == L7_TRUE)
          {
            L7_VLAN_SETMASKBIT(pUslPortEntry->data.vlanConfig.vlan_membership_mask, 
                               vlanId);     
            if (tagMask & 0x1)
            {
              L7_VLAN_SETMASKBIT(pUslPortEntry->data.vlanConfig.vlan_tag_mask, 
                                 vlanId);     
            }
          }
          else
          {
            L7_VLAN_CLRMASKBIT(pUslPortEntry->data.vlanConfig.vlan_membership_mask, 
                               vlanId);     
            L7_VLAN_CLRMASKBIT(pUslPortEntry->data.vlanConfig.vlan_tag_mask, 
                               vlanId);     
          }
        }

        membMask >>= 1;
        tagMask  >>= 1;
      } /* End While */
    } /* End for */

    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_VLAN_CONFIG_SET);
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the shaper config attribute in specified buffer */
void usl_port_print_stg_config(USL_DB_TYPE_t dbType,
                               usl_port_db_stg_config_t *config, 
                               L7_uchar8 *buffer,
                               L7_uint32 size)
{
  L7_uint32 i;

  osapiSnprintf(buffer, size,
                "%s Stg config val: \n",
                usl_db_type_name_get(dbType));

  for (i = 0; i < HAPI_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    if (config[i].valid)
    {
      osapiSnprintfcat(buffer, size,    
                       "stg %d state %d\n",
                       i, config[i].dot1sState);
    }
  }

  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_stg_state_update_bcm(L7_int32 unit, L7_int32 port,
                                       usl_port_db_elem_t *shadowElem,
                                       usl_port_db_elem_t *operElem)
{
  L7_uchar8    uslStr[128];
  L7_int32     rv = BCM_E_NONE;
  L7_uint32    i;
  
  usl_port_print_stg_config(USL_OPERATIONAL_DB,
                            operElem->data.dot1sConfig, uslStr, 
                            sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_stg_config(USL_SHADOW_DB,
                            shadowElem->data.dot1sConfig, uslStr, 
                            sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(shadowElem->data.dot1sConfig, operElem->data.dot1sConfig,
             sizeof(shadowElem->data.dot1sConfig)) != 0)
  {
    for (i = 0; i < HAPI_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
      /* Stg present in both db's, update based on shadow db state. */
      if ((shadowElem->data.dot1sConfig[i].valid == L7_TRUE) &&
          (operElem->data.dot1sConfig[i].valid == L7_TRUE) &&
          (shadowElem->data.dot1sConfig[i].dot1sState != 
               operElem->data.dot1sConfig[i].dot1sState))
      {
        rv = usl_bcm_stg_stp_set(unit, port, i,
                                 shadowElem->data.dot1sConfig[i].dot1sState); 
      } 
      /* New Stg state */
      else if ((shadowElem->data.dot1sConfig[i].valid == L7_TRUE) &&
               (operElem->data.dot1sConfig[i].valid == L7_FALSE))
      {
        rv = usl_bcm_stg_stp_set(unit, port, i,
                                 shadowElem->data.dot1sConfig[i].dot1sState); 
      }
      /* Port is no longer part of stg. Set the state to disabled
      */
      else if ((shadowElem->data.dot1sConfig[i].valid == L7_FALSE) &&
               (operElem->data.dot1sConfig[i].valid == L7_TRUE))
      {
        /* Ignore the return code as stg might have been destroyed */
        (void) usl_bcm_stg_stp_set(unit, port, i, BCM_STG_STP_DISABLE);
      }

    }
  }

  return rv;
}


/*********************************************************************
* @purpose  Set the spanning-tree stg state for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    stg                @{(input)}  stg-id
* @param    stpState           @{(input)}  state
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_stg_stp_update(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              bcm_stg_t stg , 
                              bcm_stg_stp_t stpState)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    
    pUslPortEntry->data.dot1sConfig[stg].dot1sState = stpState;
    if (stpState != BCM_STG_STP_DISABLE)
    {
      pUslPortEntry->data.dot1sConfig[stg].valid = L7_TRUE;
    }
    else
    {
      pUslPortEntry->data.dot1sConfig[stg].valid = L7_FALSE;
    }

    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_STG_STATE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the pbvlan config attribute in specified buffer */
void usl_port_print_pbvlan_config(USL_DB_TYPE_t dbType,
                                  usl_bcm_port_pbvlan_config_t *config, 
                                  L7_uchar8 *buffer,
                                  L7_uint32 size)
{
  L7_uint32 i;

  osapiSnprintf(buffer, size,
                "%s Pbvlan config val: \n",
                usl_db_type_name_get(dbType));

  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    if (config[i].etherType != 0)
    {
      osapiSnprintfcat(buffer, size,    
                       "Protocol ethertype %x frametype %d vlan %d\n",
                       config[i].etherType, config[i].frameType, config[i].vid);
    }
  }

  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_pbvlan_config_update_bcm(L7_int32 unit, L7_int32 port,
                                           usl_port_db_elem_t *shadowElem,
                                           usl_port_db_elem_t *operElem)
{
  L7_uchar8                uslStr[128];
  L7_int32                 rv = BCM_E_NONE;
  L7_uint32                i, j;
  L7_BOOL                  found;
  bcm_port_ethertype_t     etherType;
  

  usl_port_print_pbvlan_config(USL_OPERATIONAL_DB,
                               operElem->data.pbvlanTable, uslStr, 
                               sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_pbvlan_config(USL_SHADOW_DB,
                               shadowElem->data.pbvlanTable, uslStr, 
                               sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(shadowElem->data.pbvlanTable, operElem->data.pbvlanTable,
             sizeof(shadowElem->data.pbvlanTable)) != 0)
  {
    /* Loop through all the Operational entries and reconcile */
    for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
    {
      found = L7_FALSE;
      etherType = operElem->data.pbvlanTable[i].etherType;
      if (etherType == 0)
      {
        continue;    
      }

      /* Search the entry in Shadow, if found then update if needed */
      for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
      {
        if ((shadowElem->data.pbvlanTable[j].etherType == etherType) &&
            ((shadowElem->data.pbvlanTable[j].frameType != 
                              operElem->data.pbvlanTable[i].frameType) ||
            (shadowElem->data.pbvlanTable[j].vid != 
                              operElem->data.pbvlanTable[i].vid)))
     
        {
          rv = usl_bcm_port_protocol_vlan_config(unit, port,
                                                 &(shadowElem->data.pbvlanTable[j]),
                                                 L7_TRUE);
          found = L7_TRUE;
          shadowElem->data.pbvlanTable[j].etherType = 0;
          break;
        }
      }

      /* Entry not found in shadow. Delete it */
      if (found == L7_FALSE)
      {
        rv = usl_bcm_port_protocol_vlan_config(unit, port,
                                               &(operElem->data.pbvlanTable[i]),
                                               L7_FALSE);
      }
    }

    /* Walk through all the remaining entries in Shadow and add it to hw */
    for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
    {
      etherType = shadowElem->data.pbvlanTable[i].etherType;
      if (etherType == 0)
      {
        continue;    
      }

      rv = usl_bcm_port_protocol_vlan_config(unit, port,
                                             &(shadowElem->data.pbvlanTable[i]),
                                             L7_TRUE);

    }
  }

  return rv;
}

/*********************************************************************
* @purpose  Update the protocol-based vlan configuration in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    pbvlanConfig       @{(input)}  Vlan config data
* @param    cmd                @{(input)}  L7_TRUE: Add pbvlan config to the port
*                                          L7_FALSE: Remove pbvlan config from the port
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_protocol_vlan_update(USL_DB_TYPE_t dbType,
                                    bcm_gport_t gport, 
                                    usl_bcm_port_pbvlan_config_t *pbvlanConfig, 
                                    L7_BOOL cmd)
{
  int                 rv = BCM_E_FAIL;
  usl_port_db_elem_t    *pUslPortEntry;
  L7_uint32           i; 

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
    {
      if (cmd == L7_FALSE) /* remove */
      {
        if (pUslPortEntry->data.pbvlanTable[i].etherType == pbvlanConfig->etherType)
        {
          memset(&(pUslPortEntry->data.pbvlanTable[i]), 0, 
                 sizeof(pUslPortEntry->data.pbvlanTable[i]));
          rv = BCM_E_NONE;
          break;
        }
      }
      else /* add */
      {
        if (pUslPortEntry->data.pbvlanTable[i].etherType == 0)
        {
          memcpy(&(pUslPortEntry->data.pbvlanTable[i]), pbvlanConfig,
                 sizeof(pUslPortEntry->data.pbvlanTable[i]));
          rv = BCM_E_NONE;
          break;
        }
      }
    } /* End for */

    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_PBVLAN_CONFIG_SET);
  } /* End else */
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_dot1x_config_update_bcm(L7_int32 unit, L7_int32 port,
                                          usl_port_db_elem_t *shadowElem,
                                          usl_port_db_elem_t *operElem)
{
  L7_uchar8                  uslStr[128];
  L7_int32                   rv = BCM_E_NONE;
  L7_DOT1X_PORT_STATUS_t     operStatus, shadowStatus;

  if (USL_PORTDB_FLAG_IS_INDEX_SET(operElem->data.portFlags, 
                                   USL_PORT_FLAG_DOT1X_STATE))
  {
    osapiSnprintf(uslStr, sizeof(uslStr),
                  "%s val: Dot1x Status %s\n",
                  usl_db_type_name_get(USL_OPERATIONAL_DB), "Unauthorized");
    operStatus = L7_DOT1X_PORT_STATUS_UNAUTHORIZED;
      
  }
  else
  {
    osapiSnprintf(uslStr, sizeof(uslStr),
                  "%s val: Dot1x Status %s\n",
                  usl_db_type_name_get(USL_OPERATIONAL_DB), "Authorized"); 
    operStatus = L7_DOT1X_PORT_STATUS_AUTHORIZED;

  }

  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  if (USL_PORTDB_FLAG_IS_INDEX_SET(shadowElem->data.portFlags, 
                                   USL_PORT_FLAG_DOT1X_STATE))
  {
    osapiSnprintf(uslStr, sizeof(uslStr),
                  "%s val: Dot1x Status %s\n",
                  usl_db_type_name_get(USL_SHADOW_DB), "Unauthorized");
    shadowStatus = L7_DOT1X_PORT_STATUS_UNAUTHORIZED;
      
  }
  else
  {
    osapiSnprintf(uslStr, sizeof(uslStr),
                  "%s val: Dot1x Status %s\n",
                  usl_db_type_name_get(USL_SHADOW_DB), "Authorized");
    shadowStatus = L7_DOT1X_PORT_STATUS_AUTHORIZED;

  }

  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (operStatus != shadowStatus)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_dot1x_config(unit, port, 
                                   shadowStatus);    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the dot1x state for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    dot1xState         @{(input)}  state
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_dot1x_config(USL_DB_TYPE_t dbType,
                            bcm_gport_t gport, 
                            L7_DOT1X_PORT_STATUS_t state)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    if (state == L7_DOT1X_PORT_STATUS_AUTHORIZED )
    {
      USL_PORTDB_FLAG_INDEX_CLR(pUslPortEntry->data.portFlags, 
                                USL_PORT_FLAG_DOT1X_STATE);
    }
    else
    {
      USL_PORTDB_FLAG_INDEX_SET(pUslPortEntry->data.portFlags, 
                                USL_PORT_FLAG_DOT1X_STATE);
    }

    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_DOT1X_CONFIG_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Get the dot1x state for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    dot1xState         @{(output)} state
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_dot1x_config_get(USL_DB_TYPE_t dbType,
                                bcm_gport_t gport, 
                                L7_DOT1X_PORT_STATUS_t *state)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    *state = USL_PORTDB_FLAG_IS_INDEX_SET(pUslPortEntry->data.portFlags, 
                                          USL_PORT_FLAG_DOT1X_STATE) ? 
                                          L7_DOT1X_PORT_STATUS_UNAUTHORIZED : 
                                          L7_DOT1X_PORT_STATUS_AUTHORIZED;
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the mirror attribute in specified buffer */
void usl_port_print_mirror_config(USL_DB_TYPE_t dbType,
                                  usl_bcm_port_mirror_config_t *config, 
                                  L7_uchar8 *buffer,
                                  L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s mirror config\n",
                usl_db_type_name_get(dbType));

  osapiSnprintfcat(buffer, size,
                   "Flags %d probePort %x stackUnit %d\n",
                   config->flags, config->probePort, config->stackUnit);

  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_mirror_config_update_bcm(L7_int32 unit, L7_int32 port,
                                           usl_port_db_elem_t *shadowElem,
                                           usl_port_db_elem_t *operElem)
{
  L7_int32                        rv = BCM_E_NONE;
  L7_uchar8                       uslStr[128];
  usl_bcm_port_mirror_config_t    mirrorConfig;

  memset(&mirrorConfig, 0, sizeof(mirrorConfig));

  if (USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                         USL_BCMX_PORT_MIRROR_CONFIG_SET))
  {
    usl_port_print_mirror_config(USL_OPERATIONAL_DB,
                                 &(operElem->data.mirrorConfig),
                                 uslStr, sizeof(uslStr));    
    usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);
  }
  
  

  if (USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                         USL_BCMX_PORT_MIRROR_CONFIG_SET))
  {
    usl_port_print_mirror_config(USL_SHADOW_DB,
                                 &(shadowElem->data.mirrorConfig),
                                 uslStr, sizeof(uslStr));    
    usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);
  }


  /* Update the Hw with Shadow value */
  if ((!(USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                         USL_BCMX_PORT_MIRROR_CONFIG_SET))) &&
     (USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                         USL_BCMX_PORT_MIRROR_CONFIG_SET)))

  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_mirror_set(unit, port, &(shadowElem->data.mirrorConfig));
  }
  else if ((USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                               USL_BCMX_PORT_MIRROR_CONFIG_SET)) &&
            (USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                                USL_BCMX_PORT_MIRROR_CONFIG_SET)) &&
            (memcmp(&(shadowElem->data.mirrorConfig), &(operElem->data.mirrorConfig),
                  sizeof(shadowElem->data.mirrorConfig)) != 0))
  {
    /* First disable mirroring */
    mirrorConfig.flags = 0;
    mirrorConfig.probePort = operElem->data.mirrorConfig.probePort;
    mirrorConfig.stackUnit = operElem->data.mirrorConfig.stackUnit;
    rv = usl_bcm_port_mirror_set(unit, port, &mirrorConfig);
    
    /* Reconfigure the new settings */
    rv = usl_bcm_port_mirror_set(unit, port, &(shadowElem->data.mirrorConfig));
  }
  else if ((USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                               USL_BCMX_PORT_MIRROR_CONFIG_SET)) &&
           (!(USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                               USL_BCMX_PORT_MIRROR_CONFIG_SET))))
  {
    /* Set the flags to zero to disable mirroring on the port */
    mirrorConfig.flags = 0;
    mirrorConfig.probePort = operElem->data.mirrorConfig.probePort;
    mirrorConfig.stackUnit = operElem->data.mirrorConfig.stackUnit;

    rv = usl_bcm_port_mirror_set(unit, port, &mirrorConfig);
  }

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Set the mirror config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    mirrorConfig       @{(input)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_mirror_set(USL_DB_TYPE_t dbType,
                          bcm_gport_t gport, 
                          usl_bcm_port_mirror_config_t *mirrorConfig)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.mirrorConfig), mirrorConfig, 
           sizeof(pUslPortEntry->data.mirrorConfig));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_MIRROR_CONFIG_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Get the mirror config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    mirrorConfig       @{(input)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_mirror_get(USL_DB_TYPE_t dbType,
                          bcm_gport_t gport, 
                          usl_bcm_port_mirror_config_t *mirrorConfig)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(mirrorConfig, &(pUslPortEntry->data.mirrorConfig), 
           sizeof(usl_bcm_port_mirror_config_t));
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Add a policy to a port. 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    policy             @{(input)}  Policy Id
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_policy_add(USL_DB_TYPE_t dbType,
                          bcm_gport_t gport, 
                          L7_uint32 policyId)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;
  L7_uint32           byteIdx, bitIdx;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  if (policyId >= BROAD_MAX_POLICIES)
  {
    return BCM_E_PARAM;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    byteIdx = policyId / 8;
    bitIdx  = policyId % 8;
    pUslPortEntry->portPoliciesMask[byteIdx]       |= (1 << bitIdx);
    pUslPortEntry->portPoliciesChangeMask[byteIdx] |= (1 << bitIdx);
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_POLICY_SET_HANDLER);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Remove a policy from a port. 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    policy             @{(input)}  Policy Id
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_policy_remove(USL_DB_TYPE_t dbType,
                             bcm_gport_t gport, 
                             L7_uint32 policyId)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;
  L7_uint32              byteIdx, bitIdx;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  if (policyId >= BROAD_MAX_POLICIES)
  {
    return BCM_E_PARAM;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    byteIdx = policyId / 8;
    bitIdx  = policyId % 8;
    pUslPortEntry->portPoliciesMask[byteIdx]       &= ~(1 << bitIdx);
    pUslPortEntry->portPoliciesChangeMask[byteIdx] |= (1 << bitIdx);
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_POLICY_SET_HANDLER);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_enable_update_bcm(L7_int32 unit, L7_int32 port,
                                    usl_port_db_elem_t *shadowElem,
                                    usl_port_db_elem_t *operElem)
{

  L7_uchar8                  uslStr[128];
  L7_int32                   rv = BCM_E_NONE;
  L7_BOOL                    operStatus, shadowStatus;

  if (USL_PORTDB_FLAG_IS_INDEX_SET(operElem->data.portFlags, 
                                   USL_PORT_FLAG_ADMIN_MODE))
  {
    osapiSnprintf(uslStr, sizeof(uslStr),
                  "%s val: Admin status %s\n",
                  usl_db_type_name_get(USL_OPERATIONAL_DB), "Enabled");
    operStatus = L7_TRUE;
      
  }
  else
  {
    osapiSnprintf(uslStr, sizeof(uslStr),
                  "%s val: Admin status %s\n",
                  usl_db_type_name_get(USL_OPERATIONAL_DB), "Disabled"); 
    operStatus = L7_FALSE;

  }

  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  if (USL_PORTDB_FLAG_IS_INDEX_SET(shadowElem->data.portFlags, 
                                   USL_PORT_FLAG_ADMIN_MODE))
  {
    osapiSnprintf(uslStr, sizeof(uslStr),
                  "%s val: Admin status %s\n",
                  usl_db_type_name_get(USL_SHADOW_DB), "Enabled");
    shadowStatus = L7_TRUE;
      
  }
  else
  {
    osapiSnprintf(uslStr, sizeof(uslStr),
                  "%s val: Admin status %s\n",
                  usl_db_type_name_get(USL_SHADOW_DB), "Disabled"); 
    shadowStatus = L7_FALSE;

  }

  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (operStatus != shadowStatus)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_enable_set(unit, port, 
                                 shadowStatus);    
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the admin mode for a port 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Admin mode
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_enable_set(USL_DB_TYPE_t dbType,
                           bcm_gport_t gport, 
                           L7_int32 enable)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    if (enable)
    {
      USL_PORTDB_FLAG_INDEX_SET(pUslPortEntry->data.portFlags,
                                USL_PORT_FLAG_ADMIN_MODE);
    }
    else
    {
      USL_PORTDB_FLAG_INDEX_CLR(pUslPortEntry->data.portFlags, 
                                USL_PORT_FLAG_ADMIN_MODE);
    }

    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                    USL_BCMX_PORT_ADMIN_MODE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/* Print the admin mode attribute in specified buffer */
void usl_port_print_wred_config(USL_DB_TYPE_t dbType,
                                usl_bcm_port_wred_config_t *config, 
                                L7_uchar8     *buffer,
                                L7_uint32      size)
{
  L7_uint32  i, j;

  osapiSnprintf(buffer, size,
                "%s wred config\n",
                usl_db_type_name_get(dbType));

  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    osapiSnprintfcat(buffer, size,
                     "Cos %d gain %d flags %d\n",
                     i, config->gain, config->flags);

    /* PTin modified: Allow 6 DP levels */
    for (j = 0; j < (L7_MAX_CFG_DROP_PREC_LEVELS*2); j++)
    {
      osapiSnprintfcat(buffer, size,
                       "Drop probability %d minThrehold %d maxThreshold %d\n",
                       config->dropProb[i][j], 
                       config->minThreshold[i][j],
                       config->maxThreshold[i][j]);
    }

  }

  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_wred_update_bcm(L7_int32 unit, L7_int32 port,
                                  usl_port_db_elem_t *shadowElem,
                                  usl_port_db_elem_t *operElem)
{
  L7_uchar8                       uslStr[128];
  L7_int32                        rv = BCM_E_NONE;
  usl_bcm_port_wred_config_t      wredConfig;

  memset(&wredConfig, 0, sizeof(wredConfig));

  if (USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                         USL_BCMX_PORT_WRED_CONFIG_SET))
  {
    usl_port_print_wred_config(USL_OPERATIONAL_DB,
                               &(operElem->data.wredConfig),
                               uslStr, sizeof(uslStr));    
  }
  
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  if (USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                         USL_BCMX_PORT_WRED_CONFIG_SET))
  {
    usl_port_print_wred_config(USL_SHADOW_DB,
                               &(operElem->data.wredConfig),
                               uslStr, sizeof(uslStr));    
  }

  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if ((!(USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                         USL_BCMX_PORT_WRED_CONFIG_SET))) &&
     (USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                         USL_BCMX_PORT_WRED_CONFIG_SET)))

  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_wred_set(unit, port, &(shadowElem->data.wredConfig));
  }
  else if ((USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                               USL_BCMX_PORT_WRED_CONFIG_SET)) &&
           (USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                               USL_BCMX_PORT_WRED_CONFIG_SET)) &&
           (memcmp(&(shadowElem->data.wredConfig), &(operElem->data.wredConfig),
            sizeof(shadowElem->data.wredConfig)) != 0))
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_wred_set(unit, port, &(shadowElem->data.wredConfig));
  }
  else if ((USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                               USL_BCMX_PORT_WRED_CONFIG_SET)) &&
           (!(USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                               USL_BCMX_PORT_WRED_CONFIG_SET))))
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    /* Set the flags to zero to disable wred on the port */
    memset(&wredConfig, 0, sizeof(wredConfig));
    rv = usl_bcm_port_wred_set(unit, port, &wredConfig);
  }


  return rv;
}


/*********************************************************************
* @purpose  Set the wred parameters for a port 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    wredParams  @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_wred_set(USL_DB_TYPE_t dbType,
                         bcm_gport_t gport, 
                         usl_bcm_port_wred_config_t *wredParams)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.wredConfig), wredParams, sizeof(*wredParams));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                    USL_BCMX_PORT_WRED_CONFIG_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}


/* Print the pvid attribute in specified buffer */
void usl_port_print_sflow(USL_DB_TYPE_t dbType,
                          usl_bcm_port_sflow_config_t *sflowConfig, 
                          L7_uchar8 *buffer,
                          L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s val: Sflow Ing rate %d Egr rate %d\n",
                usl_db_type_name_get(dbType), 
                sflowConfig->ingressSamplingRate,
                sflowConfig->egressSamplingRate);
  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_sflow_update_bcm(L7_int32 unit, L7_int32 port,
                                   usl_port_db_elem_t *shadowElem,
                                   usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  usl_port_print_sflow(USL_OPERATIONAL_DB,
                       &(operElem->data.sflowConfig), uslStr, 
                       sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  usl_port_print_sflow(USL_SHADOW_DB,
                       &(shadowElem->data.sflowConfig), uslStr, 
                       sizeof(uslStr));
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (memcmp(&(shadowElem->data.sflowConfig), &(operElem->data.sflowConfig),
             sizeof(shadowElem->data.sflowConfig)) != 0)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_sflow_config_set(unit, port, 
                                       &(shadowElem->data.sflowConfig));    
  }

  return rv;
}


/*********************************************************************
* @purpose  Set the sflow config for a port 
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    sflowParams @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_sflow_config_set(USL_DB_TYPE_t dbType,
                                 bcm_gport_t gport, 
                                 usl_bcm_port_sflow_config_t *sflowConfig)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.sflowConfig), sflowConfig, sizeof(*sflowConfig));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                    USL_BCMX_PORT_SFLOW_CONFIG_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Get the sflow config for a port 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    wredParams  @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_sflow_config_get(USL_DB_TYPE_t dbType,
                                 bcm_gport_t gport, 
                                 usl_bcm_port_sflow_config_t *sflowConfig)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    if (USL_PORTDB_VALID_MASK_IS_INDEX_SET(pUslPortEntry->data.portValidMask,
                                           USL_BCMX_PORT_SFLOW_CONFIG_SET))
    {
      memcpy(sflowConfig, &(pUslPortEntry->data.sflowConfig), sizeof(*sflowConfig));
    }
    else
    {
      rv = BCM_E_FAIL;
    }
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Set the VLAN translation config for all ports
*
* @param    dbType      @{(input)}  dbType to modify
* @param    direction   @{(input)}  TRUE = ingress, FALSE = egress
* @param    enable      @{(input)}  enable mode
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_all_vlan_translate_set(USL_DB_TYPE_t dbType, L7_BOOL direction, L7_BOOL enable)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry, searchEntry;
  avlTree_t             *dbHandle = L7_NULLPTR;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_port_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;    
    }

    memset(&searchEntry, 0, sizeof(searchEntry));
    pUslPortEntry = avlSearchLVL7(dbHandle, &searchEntry, AVL_EXACT);
    if (pUslPortEntry == L7_NULL)
    {
      pUslPortEntry = avlSearchLVL7(dbHandle, &searchEntry, AVL_NEXT);
    }

    while (pUslPortEntry != L7_NULL)
    {
      memcpy(&searchEntry, pUslPortEntry, sizeof(searchEntry));

      if (direction == L7_TRUE)
      {
        /* Ingress */
        pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateIngressEnable = enable;
        USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                        USL_BCMX_PORT_VLAN_XLATE_INGRESS_ENABLE_SET);
      }
      else
      {
        /* Egress */
        pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateEgressEnable = enable;
        USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                        USL_BCMX_PORT_VLAN_XLATE_EGRESS_ENABLE_SET);

        pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateEgressMissDrop = enable;
        USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                        USL_BCMX_PORT_VLAN_XLATE_EGRESS_MISS_DROP_SET);
      }

      pUslPortEntry = avlSearchLVL7(dbHandle, &searchEntry, AVL_NEXT);
    }

  } while (0);

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_vlan_translate_ingress_enable_update_bcm(L7_int32 unit, L7_int32 port,
                                                           usl_port_db_elem_t *shadowElem,
                                                           usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate ingress enable %d\n",
                usl_db_type_name_get(USL_OPERATIONAL_DB), 
                operElem->data.vlanControlConfig.bcmVlanTranslateIngressEnable);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate ingress enable %d\n",
                usl_db_type_name_get(USL_SHADOW_DB), 
                shadowElem->data.vlanControlConfig.bcmVlanTranslateIngressEnable);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (operElem->data.vlanControlConfig.bcmVlanTranslateIngressEnable != shadowElem->data.vlanControlConfig.bcmVlanTranslateIngressEnable)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_vlan_translate_ingress_enable_set(unit, port, shadowElem->data.vlanControlConfig.bcmVlanTranslateIngressEnable);
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_ingress_enable_set(USL_DB_TYPE_t dbType,
                                                  bcm_gport_t   gport, 
                                                  L7_BOOL       enable)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateIngressEnable = enable;
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                    USL_BCMX_PORT_VLAN_XLATE_INGRESS_ENABLE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_vlan_translate_ingress_miss_drop_update_bcm(L7_int32 unit, L7_int32 port,
                                                              usl_port_db_elem_t *shadowElem,
                                                              usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate ingress miss drop %d\n",
                usl_db_type_name_get(USL_OPERATIONAL_DB), 
                operElem->data.vlanControlConfig.bcmVlanTranslateIngressMissDrop);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate ingress miss drop %d\n",
                usl_db_type_name_get(USL_SHADOW_DB), 
                shadowElem->data.vlanControlConfig.bcmVlanTranslateIngressMissDrop);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (operElem->data.vlanControlConfig.bcmVlanTranslateIngressMissDrop != shadowElem->data.vlanControlConfig.bcmVlanTranslateIngressMissDrop)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_vlan_translate_ingress_miss_drop_set(unit, port, shadowElem->data.vlanControlConfig.bcmVlanTranslateIngressMissDrop);
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_ingress_miss_drop_set(USL_DB_TYPE_t dbType,
                                                     bcm_gport_t   gport, 
                                                     L7_BOOL       drop)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateIngressMissDrop = drop;
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                    USL_BCMX_PORT_VLAN_XLATE_INGRESS_MISS_DROP_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_vlan_translate_egress_enable_update_bcm(L7_int32 unit, L7_int32 port,
                                                          usl_port_db_elem_t *shadowElem,
                                                          usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate egress enable %d\n",
                usl_db_type_name_get(USL_OPERATIONAL_DB), 
                operElem->data.vlanControlConfig.bcmVlanTranslateEgressEnable);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate egress enable %d\n",
                usl_db_type_name_get(USL_SHADOW_DB), 
                shadowElem->data.vlanControlConfig.bcmVlanTranslateEgressEnable);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (operElem->data.vlanControlConfig.bcmVlanTranslateEgressEnable != shadowElem->data.vlanControlConfig.bcmVlanTranslateEgressEnable)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_vlan_translate_egress_enable_set(unit, port, shadowElem->data.vlanControlConfig.bcmVlanTranslateEgressEnable);
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_egress_enable_set(USL_DB_TYPE_t dbType,
                                                 bcm_gport_t   gport, 
                                                 L7_BOOL       enable)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateEgressEnable = enable;
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                    USL_BCMX_PORT_VLAN_XLATE_EGRESS_ENABLE_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_vlan_translate_egress_miss_drop_update_bcm(L7_int32 unit, L7_int32 port,
                                                             usl_port_db_elem_t *shadowElem,
                                                             usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate egress miss drop %d\n",
                usl_db_type_name_get(USL_OPERATIONAL_DB), 
                operElem->data.vlanControlConfig.bcmVlanTranslateEgressMissDrop);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate egress miss drop %d\n",
                usl_db_type_name_get(USL_SHADOW_DB), 
                shadowElem->data.vlanControlConfig.bcmVlanTranslateEgressMissDrop);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (operElem->data.vlanControlConfig.bcmVlanTranslateEgressMissDrop != shadowElem->data.vlanControlConfig.bcmVlanTranslateEgressMissDrop)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_vlan_translate_egress_miss_drop_set(unit, port, shadowElem->data.vlanControlConfig.bcmVlanTranslateEgressMissDrop);
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_egress_miss_drop_set(USL_DB_TYPE_t dbType,
                                                    bcm_gport_t   gport, 
                                                    L7_BOOL       drop)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    pUslPortEntry->data.vlanControlConfig.bcmVlanTranslateEgressMissDrop = drop;
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                    USL_BCMX_PORT_VLAN_XLATE_EGRESS_MISS_DROP_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_vlan_translate_key_first_update_bcm(L7_int32 unit, L7_int32 port,
                                                      usl_port_db_elem_t *shadowElem,
                                                      usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate key first %d\n",
                usl_db_type_name_get(USL_OPERATIONAL_DB), 
                operElem->data.vlanControlConfig.bcmVlanPortTranslateKeyFirst);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate key first %d\n",
                usl_db_type_name_get(USL_SHADOW_DB), 
                shadowElem->data.vlanControlConfig.bcmVlanPortTranslateKeyFirst);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (operElem->data.vlanControlConfig.bcmVlanPortTranslateKeyFirst != shadowElem->data.vlanControlConfig.bcmVlanPortTranslateKeyFirst)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_vlan_translate_key_first_set(unit, port, shadowElem->data.vlanControlConfig.bcmVlanPortTranslateKeyFirst);
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_key_first_set(USL_DB_TYPE_t            dbType,
                                             bcm_gport_t              gport, 
                                             bcm_vlan_translate_key_t key)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    pUslPortEntry->data.vlanControlConfig.bcmVlanPortTranslateKeyFirst = key;
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                    USL_BCMX_PORT_VLAN_XLATE_KEY_FIRST_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_vlan_translate_key_second_update_bcm(L7_int32 unit, L7_int32 port,
                                                       usl_port_db_elem_t *shadowElem,
                                                       usl_port_db_elem_t *operElem)
{
  L7_int32     rv = BCM_E_NONE;
  L7_uchar8    uslStr[128];

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate key second %d\n",
                usl_db_type_name_get(USL_OPERATIONAL_DB), 
                operElem->data.vlanControlConfig.bcmVlanPortTranslateKeySecond);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  osapiSnprintf(uslStr, sizeof(uslStr),
                "%s val: vlan translate key second %d\n",
                usl_db_type_name_get(USL_SHADOW_DB), 
                shadowElem->data.vlanControlConfig.bcmVlanPortTranslateKeySecond);
  usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);

  /* Update the Hw with Shadow value */
  if (operElem->data.vlanControlConfig.bcmVlanPortTranslateKeySecond != shadowElem->data.vlanControlConfig.bcmVlanPortTranslateKeySecond)
  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_vlan_translate_key_second_set(unit, port, shadowElem->data.vlanControlConfig.bcmVlanPortTranslateKeySecond);
  }

  return rv;
}

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_key_second_set(USL_DB_TYPE_t            dbType,
                                              bcm_gport_t              gport, 
                                              bcm_vlan_translate_key_t key)
{
  int                    rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    pUslPortEntry->data.vlanControlConfig.bcmVlanPortTranslateKeySecond = key;
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask,
                                    USL_BCMX_PORT_VLAN_XLATE_KEY_SECOND_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;
}
/* Print the prc attribute in specified buffer */
void usl_port_print_pfc_config(USL_DB_TYPE_t dbType,
                               usl_bcm_port_pfc_config_t *config, 
                               L7_uchar8 *buffer,
                               L7_uint32 size)
{
  osapiSnprintf(buffer, size,
                "%s PFC config\n",
                usl_db_type_name_get(dbType));

  osapiSnprintfcat(buffer, size,
                   "Mode=%d No-drop-Pri-Bmp=0x%0.2x\n",
                   config->mode, config->no_drop_priority_bmp);

  return;
}

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
L7_int32 usl_port_pfc_config_update_bcm(L7_int32 unit, L7_int32 port,
                                           usl_port_db_elem_t *shadowElem,
                                           usl_port_db_elem_t *operElem)
{
  L7_int32                        rv = BCM_E_NONE;
  L7_uchar8                       uslStr[128];
  usl_bcm_port_pfc_config_t    pfcConfig;

  memset(&pfcConfig, 0, sizeof(pfcConfig));

  if (USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                         USL_BCMX_PORT_PFC_CONFIG_SET))
  {
    usl_port_print_pfc_config(USL_OPERATIONAL_DB,
                                 &(operElem->data.pfcConfig),
                                 uslStr, sizeof(uslStr));    
    usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);
  }
  
  

  if (USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                         USL_BCMX_PORT_PFC_CONFIG_SET))
  {
    usl_port_print_pfc_config(USL_SHADOW_DB,
                                 &(shadowElem->data.pfcConfig),
                                 uslStr, sizeof(uslStr));    
    usl_trace_sync_detail(USL_PORT_DB_ID, uslStr);
  }


  /* Update the Hw with Shadow value */
  if ((!(USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                         USL_BCMX_PORT_PFC_CONFIG_SET))) &&
     (USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                         USL_BCMX_PORT_PFC_CONFIG_SET)))

  {
    usl_trace_sync_detail(USL_PORT_DB_ID, "Updating Hw\n");
    rv = usl_bcm_port_pfc_config_set(unit, port, &(shadowElem->data.pfcConfig));
  }
  else if ((USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                               USL_BCMX_PORT_PFC_CONFIG_SET)) &&
            (USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                                USL_BCMX_PORT_PFC_CONFIG_SET)) &&
            (memcmp(&(shadowElem->data.pfcConfig), &(operElem->data.pfcConfig),
                  sizeof(shadowElem->data.pfcConfig)) != 0))
  {
    /* First disable pfc */
    pfcConfig.mode = 0;
    pfcConfig.no_drop_priority_bmp = 0;

    rv = usl_bcm_port_pfc_config_set(unit, port, &pfcConfig);
    
    /* Reconfigure the new settings */
    rv = usl_bcm_port_pfc_config_set(unit, port, &(shadowElem->data.pfcConfig));
  }
  else if ((USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                               USL_BCMX_PORT_PFC_CONFIG_SET)) &&
           (!(USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                               USL_BCMX_PORT_PFC_CONFIG_SET))))
  {
    /* Set the flags to zero to disable pfc on the port */
    pfcConfig.mode = 0;
    pfcConfig.no_drop_priority_bmp = 0;

    rv = usl_bcm_port_pfc_config_set(unit, port, &pfcConfig);
  }

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Set the pfc config for the port in the USL database
*
* @param    dbType          @{(input)}  dbType to modify
* @param    gport           @{(input)}  Port number
* @param    pfcConfig       @{(input)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_pfc_config_set(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              usl_bcm_port_pfc_config_t *pfcConfig)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(&(pUslPortEntry->data.pfcConfig), pfcConfig, 
           sizeof(pUslPortEntry->data.pfcConfig));
    USL_PORTDB_VALID_MASK_INDEX_SET(pUslPortEntry->data.portValidMask, 
                                    USL_BCMX_PORT_PFC_CONFIG_SET);
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Get the pfc config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    pfcConfig        @{(output)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_pfc_config_get(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              usl_bcm_port_pfc_config_t *pfcConfig)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    memcpy(pfcConfig, &(pUslPortEntry->data.pfcConfig), 
           sizeof(usl_bcm_port_pfc_config_t));
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  return rv;

}


/*********************************************************************
* @purpose  Get the pfc parms for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    mmuParms         @{(output)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_mmu_parms_get(USL_DB_TYPE_t dbType,
                             bcm_gport_t gport, 
                             usl_bcm_port_mmu_parms_t *mmuParms)
{
  int                 rv = BCM_E_NONE;
  usl_port_db_elem_t    *pUslPortEntry;

  if (uslPortDbActive == L7_FALSE)
  {
    rv = BCM_E_NONE;
    return rv;
  }
  else if ((gport == BCM_GPORT_INVALID) || (L7_NULLPTR == mmuParms))
  {
    rv = BCM_E_NOT_FOUND;
    return rv;
  }

  USL_PORT_DB_LOCK_TAKE();

  /* Search for the port record in portdb. It is possible that port record may
  ** not be found if the GPORT type is not supported
  */
  pUslPortEntry = usl_portdb_port_record_find(dbType, gport);
  if (pUslPortEntry != L7_NULLPTR)
  {
    mmuParms->pfc     = pUslPortEntry->data.pfcConfig.mode;
    mmuParms->pfc_bmp = pUslPortEntry->data.pfcConfig.no_drop_priority_bmp;
    mmuParms->pause   = USL_PORTDB_FLAG_IS_INDEX_SET(pUslPortEntry->data.portFlags,
                                                     USL_PORT_FLAG_PAUSE_TX);
    mmuParms->pause  |= USL_PORTDB_FLAG_IS_INDEX_SET(pUslPortEntry->data.portFlags,
                                                     USL_PORT_FLAG_PAUSE_RX);
    mmuParms->mtu     = pUslPortEntry->data.maxFrameSize;
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_PORT_DB_LOCK_GIVE();

  if (pUslPortEntry != L7_NULLPTR)
  {
    L7_uint32 mode = 0; 
    L7_uchar8 mac[6];

    usl_db_flow_control_get(USL_OPERATIONAL_DB , &mode, mac);
    mmuParms->pause |= mode;
  }

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
L7_int32 usl_update_port_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  
  L7_int32                      rv = BCM_E_NONE, unit, port;
  L7_BOOL                       handleCmd;
  L7_uint32                     operDbCmdValid, shadowDbCmdValid;
  usl_port_db_elem_t           *shadowElem = shadowDbItem;
  usl_port_db_elem_t           *operElem = operDbItem;
  USL_BCMX_PORT_SET_COMMAND_t   cmdId;

  rv = usl_gport_to_bcm_unit_port(shadowElem->data.key.gport,
                                  &unit, &port);
  if (rv != BCM_E_NONE)
  {
    return rv;    
  }


  for (cmdId = USL_BCMX_PORT_SET_CMD_FIRST; cmdId < USL_BCMX_PORT_SET_CMD_LAST;
       cmdId++)
  {

    operDbCmdValid = USL_PORTDB_VALID_MASK_IS_INDEX_SET(operElem->data.portValidMask, 
                                                        cmdId);
    shadowDbCmdValid = USL_PORTDB_VALID_MASK_IS_INDEX_SET(shadowElem->data.portValidMask, 
                                                          cmdId);

    handleCmd = L7_TRUE;

    /* if oper & shadow cmd data is invalid then move on to next cmd */
    if ((!operDbCmdValid) && (!shadowDbCmdValid))
    {
      continue;        
    }


    /* The cmd's listed below are always expected to have valid Oper and Shadow
    ** Db value. If that is not the case, then treat it as a bug and log_msg.
    */
    /*if (((!operDbCmdValid) && (shadowDbCmdValid)) || */
    if ((operDbCmdValid) &&  (!shadowDbCmdValid)) 
    {
      switch (cmdId)
      {
        case USL_BCMX_PORT_BCAST_RATE_SET:
        case USL_BCMX_PORT_MCAST_RATE_SET:
        case USL_BCMX_PORT_DLF_RATE_SET:
        case USL_BCMX_PORT_FILTER_MODE_SET:
        case USL_BCMX_PORT_PRIORITY_SET:
        case USL_BCMX_PORT_MAX_FRAME_SET:
        case USL_BCMX_PORT_LEARN_MODE_SET:
        case USL_BCMX_PORT_DTAG_MODE_SET:
        case USL_BCMX_PORT_PVID_SET:
        case USL_BCMX_PORT_DISCARD_SET:
        case USL_BCMX_PORT_PHY_CONFIG_SET:
        case USL_BCMX_PORT_ADMIN_MODE_SET:
        case USL_BCMX_PORT_DOT1X_CONFIG_SET:
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
                  "Reconciliation error: Unit/Port %d/%d: CmdId %d "
                  "does not have a valid value, operValid %d shadowValid %d\n",
                      unit, port, cmdId, operDbCmdValid, shadowDbCmdValid); 
          handleCmd = L7_FALSE;
          break;

        default:
          break;
      }
    }

    if (handleCmd == L7_FALSE)
    {
      continue;    
    }

    switch (cmdId)
    {
      case USL_BCMX_POLICY_SET_HANDLER:
        rv = usl_port_policy_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_FLOW_CONTROL_SET:
        rv = BCM_E_NONE;
        break;

      case USL_BCMX_PORT_BCAST_RATE_SET:
        rv = usl_port_bcast_rate_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_MCAST_RATE_SET:
        rv = usl_port_mcast_rate_update_bcm(unit, port, shadowElem, operElem); 
        break;

      case USL_BCMX_PORT_DLF_RATE_SET:
        rv = usl_port_dlf_rate_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_FILTER_MODE_SET:
        rv = usl_port_filter_mode_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_PRIORITY_SET:
        rv = usl_port_priority_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_MAX_FRAME_SET:
        rv = usl_port_max_frame_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_LEARN_MODE_SET:
        rv = usl_port_learn_mode_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_DTAG_MODE_SET:
        rv = usl_port_dtag_mode_update_bcm(unit, port, shadowElem, operElem);
        break;
 
      case USL_BCMX_PORT_TPID_SET:
        rv = usl_port_tpid_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_PVID_SET:
        rv = usl_port_pvid_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_DISCARD_SET:
        rv = usl_port_discard_mode_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_PHY_CONFIG_SET:
        rv = usl_port_phy_config_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_COSQ_SCHED_CONFIG_SET:
        rv = usl_port_cosq_sched_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_SHAPER_CONFIG_SET:
        rv = usl_port_shaper_config_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_VLAN_CONFIG_SET:
        rv = usl_port_vlan_config_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_STG_STATE_SET:
        rv = usl_port_stg_state_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_PBVLAN_CONFIG_SET:
        rv = usl_port_pbvlan_config_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_DOT1X_CONFIG_SET:
        rv = usl_port_dot1x_config_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_MIRROR_CONFIG_SET:
        rv = usl_port_mirror_config_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_ADMIN_MODE_SET:
        rv = usl_port_enable_update_bcm(unit, port, shadowElem, operElem);
        break; 

      case USL_BCMX_PORT_WRED_CONFIG_SET:
        rv = usl_port_wred_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_SFLOW_CONFIG_SET:
        rv = usl_port_sflow_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_VLAN_XLATE_INGRESS_ENABLE_SET:
        rv = usl_port_vlan_translate_ingress_enable_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_VLAN_XLATE_INGRESS_MISS_DROP_SET:
        rv = usl_port_vlan_translate_ingress_miss_drop_update_bcm(unit, port, shadowElem, operElem);
        break;
  
      case USL_BCMX_PORT_VLAN_XLATE_EGRESS_ENABLE_SET:
        rv = usl_port_vlan_translate_egress_enable_update_bcm(unit, port, shadowElem, operElem);
        break;
  
      case USL_BCMX_PORT_VLAN_XLATE_EGRESS_MISS_DROP_SET:
        rv = usl_port_vlan_translate_egress_miss_drop_update_bcm(unit, port, shadowElem, operElem);
        break;
  
      case USL_BCMX_PORT_VLAN_XLATE_KEY_FIRST_SET:
        rv = usl_port_vlan_translate_key_first_update_bcm(unit, port, shadowElem, operElem);
        break;
  
      case USL_BCMX_PORT_VLAN_XLATE_KEY_SECOND_SET:
        rv = usl_port_vlan_translate_key_second_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_PORT_PFC_CONFIG_SET:
        rv = usl_port_pfc_config_update_bcm(unit, port, shadowElem, operElem);
        break;

      case USL_BCMX_DOT1X_CLIENT_BLOCK:
      case USL_BCMX_PORT_SET_CMD_LAST:
      default:  
        rv = BCM_E_FAIL;
        break;
    }
  } /* End for */

  return rv;
}

