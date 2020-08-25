/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_vlan_db.c
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

#include "l7_usl_vlan_db.h"
#include "l7_usl_trace.h"
#include "broad_common.h"
#include "cnfgr.h"

/* VLAN table resources */
static void                 *pUslVlanDbSema = L7_NULLPTR;

/* Operational Db Pointer */
static usl_vlan_db_elem_t   *pUslOperVlanDb = L7_NULLPTR;

/* Shadow Db Pointer */
static usl_vlan_db_elem_t   *pUslShadowVlanDb = L7_NULLPTR;

/* Current Db Handle: Either Operational or Shadow */
static usl_vlan_db_elem_t   *pUslVlanDbHandle = L7_NULLPTR;

static L7_uint32             uslVlanDbMaxIdx  = 4096;

static L7_BOOL               uslVlanDbActive  = L7_FALSE;

#define USL_VLAN_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_VLAN_DB_ID,"VID",osapiTaskIdSelf(),__LINE__,L7_TRUE);\
  if (osapiSemaTake(pUslVlanDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslVlanDbSema); \
  } \
}

#define USL_VLAN_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_VLAN_DB_ID,"VID",osapiTaskIdSelf(),__LINE__,L7_FALSE);\
  if (osapiSemaGive(pUslVlanDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslVlanDbSema); \
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
usl_vlan_db_elem_t *usl_vlan_db_handle_get(USL_DB_TYPE_t dbType)
{
  usl_vlan_db_elem_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = pUslOperVlanDb;
      break;

    case USL_SHADOW_DB:
      dbHandle = pUslShadowVlanDb;
      break;

    case USL_CURRENT_DB:
      dbHandle = pUslVlanDbHandle;
      break;

    default:
      break;   
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a vlan Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_vlan_db_elem(void)
{
  return sizeof(usl_vlan_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a vlan db element in specified buffer
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
void usl_print_vlan_db_elem(void *elem, L7_uchar8 *buffer,
                            L7_uint32 size)
{
  usl_vlan_db_elem_t *item = elem;

  osapiSnprintf(buffer, size,
                "Vlan %d control_flags 0x%x mcast_flood_mode %d forwarding_mode %d\n",
                item->vid, item->controlFlags, item->mcastFloodMode, item->forwarding_mode);
}

/*********************************************************************
* @purpose  Search an element from vlan db and return pointer to
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
* @notes    Internal API. VLAN DB lock should be held by the caller.
*
* @end
*********************************************************************/
static L7_int32 usl_search_vlan_db_elem(USL_DB_TYPE_t dbType,
                                        L7_uint32 flags, 
                                        uslDbElemInfo_t searchInfo,
                                        uslDbElemInfo_t *elemInfo)
{
  usl_vlan_db_elem_t  *dbHandle = L7_NULLPTR;
  usl_vlan_db_elem_t  *vlanElem = searchInfo.dbElem;
  L7_int32             rv = BCM_E_NONE;
  L7_uint32            idx;
  L7_uchar8            traceStr[128];

  
  osapiSnprintf(traceStr, sizeof(traceStr),
                "Search elem: Flags %d index %d\n",
                flags, searchInfo.elemIndex);

  usl_trace_sync_detail(USL_L2_VLAN_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_vlan_db_handle_get(dbType);

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

      while ((idx < uslVlanDbMaxIdx) && (dbHandle[idx].valid == L7_FALSE))
        idx++;

      if (idx < uslVlanDbMaxIdx) 
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
     if (vlanElem == L7_NULLPTR)
     {
       rv = BCM_E_FAIL;
       break;    
     }

     /* Currently vlan-id and the table-index is same */
     if (vlanElem->vid >= uslVlanDbMaxIdx)
     {
       rv = BCM_E_NOT_FOUND;
       break;
     }

     if (dbHandle[vlanElem->vid].valid == L7_TRUE)
     {
       elemInfo->dbElem = &(dbHandle[vlanElem->vid]);
       elemInfo->elemIndex = vlanElem->vid;
       rv = BCM_E_NONE;
     } 
     else
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
* @notes    Caller must allocate memory to copy the vlan db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_vlan_db_elem(USL_DB_TYPE_t     dbType, 
                              L7_uint32         flags,
                              uslDbElemInfo_t   searchElem,
                              uslDbElemInfo_t  *elemInfo)
{
  uslDbElemInfo_t     vlanElem;
  L7_int32            rv = BCM_E_NONE;

  if (!uslVlanDbActive)
  {
    return rv;    
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  vlanElem.dbElem = L7_NULLPTR;
  vlanElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_VLAN_DB_LOCK_TAKE();

  do
  {
    /* Search for the elem */
    rv = usl_search_vlan_db_elem(dbType, flags, searchElem, &vlanElem);
    if (rv == BCM_E_NONE)
    {
      /* Element found, copy in the user memory */
      memcpy(elemInfo->dbElem, vlanElem.dbElem, sizeof(usl_vlan_db_elem_t));
      memcpy(&(elemInfo->elemIndex), &(vlanElem.elemIndex), sizeof(elemInfo->elemIndex)); 
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_VLAN_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from Vlan Db
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
L7_int32 usl_delete_vlan_db_elem(USL_DB_TYPE_t   dbType, 
                                 uslDbElemInfo_t elemInfo)
{
  uslDbElemInfo_t     vlanElem;
  L7_int32            rv = BCM_E_NONE;

  if (!uslVlanDbActive)
  {
    return rv;    
  }

  vlanElem.dbElem = L7_NULLPTR;
  vlanElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_VLAN_DB_LOCK_TAKE();

  do
  {
    
    if (elemInfo.dbElem == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;    
    }

    /* Search and delete the elem */
    rv = usl_search_vlan_db_elem(dbType, USL_DB_EXACT_ELEM, elemInfo, &vlanElem);
    if (rv == BCM_E_NONE)
    {
      memset(vlanElem.dbElem, 0, sizeof(usl_vlan_db_elem_t));    
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_VLAN_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Update the vlan database
*
* @param    dbType        @{(input)} Db type to be modified
* @param    vid           @{(input)} the VLAN ID to be created or deleted
* @param    updateCmd     @{(input)} update cmd
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_vlan_update(USL_DB_TYPE_t dbType, bcm_vlan_t vid, USL_CMD_t cmd)
{
  int                rv = BCM_E_NONE;
  usl_vlan_db_elem_t vlanElem, *dbHandle;
  uslDbElemInfo_t    elemInfo;

  /* Update the db only if it is active */
  if (!uslVlanDbActive) 
  {
    return rv;
  }

  /* Pack the key to be searched */
  memset(&vlanElem, 0, sizeof(vlanElem));
  vlanElem.vid = vid;

  elemInfo.dbElem = &vlanElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  switch (cmd)
  {
    /* Create vlan */
    case USL_CMD_ADD:

      USL_VLAN_DB_LOCK_TAKE();

      dbHandle = usl_vlan_db_handle_get(dbType);
      if (dbHandle != L7_NULLPTR)
      {
        dbHandle[vid].valid = L7_TRUE;
        dbHandle[vid].vid = vid;
        dbHandle[vid].mcastFloodMode = BCM_VLAN_MCAST_FLOOD_UNKNOWN;
        dbHandle[vid].forwarding_mode = bcmVlanForwardBridging;
      }
      else
      {
        rv = BCM_E_FAIL;
      }

      USL_VLAN_DB_LOCK_GIVE();

      break;

    /* Delete vlan */
    case USL_CMD_REMOVE:
      rv = usl_delete_vlan_db_elem(dbType, elemInfo);
      break;

    default:
      rv = BCM_E_FAIL;
      break;
  }


  return rv;
}

/*********************************************************************
* @purpose  Set the mcast flood mode for a vlan
*
* @param    dbType       @{(input)} Db type to be modified
* @param    vid          @{(input)} 
* @param    floodMode    @{(input)}
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_db_vlan_mcast_flood_set(USL_DB_TYPE_t dbType,
                                bcm_vlan_t vid, 
                                bcm_vlan_mcast_flood_t mcastFloodMode)
{
  int                   rv = BCM_E_NONE;
  usl_vlan_db_elem_t    elem; 
  uslDbElemInfo_t       searchElem, vlanElem;

  /* Update the db only if it is active */
  if (!uslVlanDbActive) 
  {
    return rv;
  }

  /* Pack the key to be searched */
  memset(&elem, 0, sizeof(elem));
  elem.vid = vid;

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  vlanElem.dbElem = L7_NULLPTR;
  vlanElem.elemIndex = USL_INVALID_DB_INDEX;
  
  USL_VLAN_DB_LOCK_TAKE();

  do
  {
    rv = usl_search_vlan_db_elem(dbType, USL_DB_EXACT_ELEM, 
                                 searchElem, &vlanElem);

    if (rv == BCM_E_NONE)
    {
      ((usl_vlan_db_elem_t *)(vlanElem.dbElem))->mcastFloodMode = mcastFloodMode;
    }

  } while (0);

  USL_VLAN_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Update the control flags for a vlan
*

* @param    dbType       @{(input)} Db type to be modified
* @param    vid          @{(input)} 
* @param    flags        @{(input)} 
* @param    cmd          @{(input)} USL_CMD_ADD: Add the specified flags
*                                                to existing flags
*                                   USL_CMD_REMOVE: Remove the flags
*                                                   from the existing flags
*                                   USL_CMD_SET: Override the existing flags
*                                                with the specified flags.
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_db_vlan_control_flag_update(USL_DB_TYPE_t dbType,
                                    bcm_vlan_t vid, L7_uint32 flags, 
                                    USL_CMD_t  cmd)
{
  int                   rv = BCM_E_NONE;
  usl_vlan_db_elem_t    elem; 
  uslDbElemInfo_t       searchElem, vlanElem;

  /* Update the db only if it is active */
  if (!uslVlanDbActive) 
  {
    return rv;
  }

  /* Pack the key to be searched */
  memset(&elem, 0, sizeof(elem));
  elem.vid = vid;

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  vlanElem.dbElem = L7_NULLPTR;
  vlanElem.elemIndex = USL_INVALID_DB_INDEX;
  
  USL_VLAN_DB_LOCK_TAKE();

  do
  {

    rv = usl_search_vlan_db_elem(dbType, USL_DB_EXACT_ELEM, 
                                 searchElem, &vlanElem);

    if (rv == BCM_E_NONE)
    {
      switch (cmd) 
      {
        case USL_CMD_ADD:
          ((usl_vlan_db_elem_t *)(vlanElem.dbElem))->controlFlags |= flags;
          break;

        case USL_CMD_REMOVE:
          ((usl_vlan_db_elem_t *)(vlanElem.dbElem))->controlFlags &= ~flags;
          break;

        case USL_CMD_SET:
          ((usl_vlan_db_elem_t *)(vlanElem.dbElem))->controlFlags = flags;
          break;
  
        default:
          rv = BCM_E_FAIL;
          break;
      }
    }
    
  } while (0);

  USL_VLAN_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Update the forwarding mode for a vlan
*
* @param    dbType                 @{(input)} Db type to be modified
* @param    vid                    @{(input)} 
* @param    forwarding_mode        @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_db_vlan_control_fwd_mode_set(USL_DB_TYPE_t dbType,
                                     bcm_vlan_t vid, bcm_vlan_forward_t forwarding_mode)
{
  int                   rv = BCM_E_NONE;
  usl_vlan_db_elem_t    elem; 
  uslDbElemInfo_t       searchElem, vlanElem;

  /* Update the db only if it is active */
  if (!uslVlanDbActive) 
  {
    return rv;
  }

  /* Pack the key to be searched */
  memset(&elem, 0, sizeof(elem));
  elem.vid = vid;

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  vlanElem.dbElem = L7_NULLPTR;
  vlanElem.elemIndex = USL_INVALID_DB_INDEX;
  
  USL_VLAN_DB_LOCK_TAKE();

  do
  {
    rv = usl_search_vlan_db_elem(dbType, USL_DB_EXACT_ELEM, 
                                 searchElem, &vlanElem);

    if (rv == BCM_E_NONE)
    {
      ((usl_vlan_db_elem_t *)(vlanElem.dbElem))->forwarding_mode = forwarding_mode;
    }
    
  } while (0);

  USL_VLAN_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create all vlan attributes at Usl Bcm layer from the given 
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
L7_int32 usl_create_vlan_db_elem_bcm(void *item)
{
  usl_vlan_db_elem_t *elem = item;
  L7_int32            rv;

  do
  {
    /* Create the vlan */
    rv = usl_bcm_vlan_update(elem->vid, L7_TRUE);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      break;  
    }

    /* Set the mcast flood mode */
    rv = usl_bcm_vlan_mcast_flood_set(elem->vid, elem->mcastFloodMode);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      break;  
    }

    /* Set the control flag */
    rv = usl_bcm_vlan_control_flag_update(elem->vid, elem->controlFlags, 
                                          USL_CMD_ADD);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      break;  
    }
    
    /* Set the forwarding mode */
    rv = usl_bcm_vlan_control_fwd_mode_set(elem->vid, elem->forwarding_mode);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      break;  
    }
    
  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete all the vlan attributes at the Usl Bcm layer from the 
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
L7_int32 usl_delete_vlan_db_elem_bcm(void *item)
{
  usl_vlan_db_elem_t *elem = item;
  L7_int32            rv;

  /* Set the mcast flood mode */
  usl_bcm_vlan_mcast_flood_set(elem->vid, elem->mcastFloodMode);

  /* Clear the vlan control flags */
  usl_bcm_vlan_control_flag_update(elem->vid, elem->controlFlags, 
                                   USL_CMD_REMOVE);

  /* Delete the vlan */
  rv = usl_bcm_vlan_update(elem->vid, L7_FALSE);

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
L7_int32 usl_update_vlan_db_elem_bcm(void *shadowDbItem, void *operDbItem)
                                     
{
  
  L7_int32                rv = BCM_E_NONE;
  usl_vlan_db_elem_t     *shadowElem = shadowDbItem;
  usl_vlan_db_elem_t     *operElem = operDbItem;

  do
  {
    if (memcmp(&(shadowElem->controlFlags), &(operElem->controlFlags), 
                sizeof(shadowElem->controlFlags)) != 0)
    {
    
      usl_bcm_vlan_control_flag_update(shadowElem->vid, 
                                       operElem->controlFlags, USL_CMD_REMOVE);
    
      rv = usl_bcm_vlan_control_flag_update(shadowElem->vid, 
                                            shadowElem->controlFlags, USL_CMD_ADD);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;  
      }
    }

    if (memcmp(&(shadowElem->mcastFloodMode), &(operElem->mcastFloodMode), 
                sizeof(shadowElem->mcastFloodMode)) != 0)
    {
      rv = usl_bcm_vlan_mcast_flood_set(shadowElem->vid, 
                                        shadowElem->mcastFloodMode);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;  
      }
    }

    if (memcmp(&(shadowElem->forwarding_mode), &(operElem->forwarding_mode), 
                sizeof(shadowElem->forwarding_mode)) != 0)
    {
      rv = usl_bcm_vlan_control_fwd_mode_set(shadowElem->vid, shadowElem->forwarding_mode);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;  
      }
    }
  } while (0);

  return rv;

}

static L7_RC_t usl_vlan_db_alloc(usl_vlan_db_elem_t **vlanDb)
{
  *vlanDb = osapiMalloc(L7_DRIVER_COMPONENT_ID, uslVlanDbMaxIdx * 
                                            sizeof(usl_vlan_db_elem_t));
  if (*vlanDb == L7_NULLPTR)
  {
    return L7_FAILURE;    
  }

  memset(*vlanDb, 0, uslVlanDbMaxIdx * sizeof(usl_vlan_db_elem_t));

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Create the USL Vlan db
*
* @params   none
*
* @returns  none
*
* @notes    USL Vlan database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_db_init(void)
{
  L7_RC_t          rc = L7_FAILURE;
  uslDbSyncFuncs_t vlanDbFuncs;

  memset(&vlanDbFuncs, 0, sizeof(vlanDbFuncs));

  do
  {
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
      pUslVlanDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);

      if ( pUslVlanDbSema == L7_NULLPTR)  
      {
        break;
      }

      if (usl_vlan_db_alloc(&pUslOperVlanDb) != L7_SUCCESS)
      {
        break;
      }

      /* Allocate shadow table for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, 
                                L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_vlan_db_alloc(&pUslShadowVlanDb) != L7_SUCCESS)
        {
          break;
        }
      }
      
      /* Register the sync routines */
      vlanDbFuncs.get_size_of_db_elem =  usl_get_size_of_vlan_db_elem;
      vlanDbFuncs.get_db_elem = usl_get_vlan_db_elem;
      vlanDbFuncs.delete_elem_from_db =  usl_delete_vlan_db_elem;
      vlanDbFuncs.print_db_elem = usl_print_vlan_db_elem;
      vlanDbFuncs.create_usl_bcm = usl_create_vlan_db_elem_bcm;
      vlanDbFuncs.delete_usl_bcm = usl_delete_vlan_db_elem_bcm;
      vlanDbFuncs.update_usl_bcm = usl_update_vlan_db_elem_bcm;

      /* Not registering optional routines as the db element is deterministic 
      ** i.e no pointer member elements
      */
      vlanDbFuncs.alloc_db_elem = L7_NULLPTR;
      vlanDbFuncs.copy_db_elem  = L7_NULLPTR; 
      vlanDbFuncs.free_db_elem  = L7_NULLPTR;
      vlanDbFuncs.pack_db_elem  = L7_NULLPTR;
      vlanDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L2_VLAN_DB_ID, &vlanDbFuncs) != L7_SUCCESS)
      {
        L7_LOG_ERROR(0);   
      }

      pUslVlanDbHandle = pUslOperVlanDb;
      uslVlanDbActive = L7_TRUE;
    }
    

    rc = L7_SUCCESS; 

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  De-allocate the USL Vlan db
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_db_fini(void)
{
  L7_RC_t rc = L7_SUCCESS;

   do
   {
     uslVlanDbActive = L7_FALSE;

     if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
     {
       if (pUslVlanDbSema != L7_NULLPTR)
       {
         osapiSemaDelete(pUslVlanDbSema);
         pUslVlanDbSema = L7_NULLPTR;
       }

       if (pUslOperVlanDb != L7_NULLPTR) 
       {
         osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperVlanDb);
       }

       if (pUslShadowVlanDb != L7_NULLPTR) 
       {
         osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowVlanDb);
       }

       pUslVlanDbHandle = L7_NULLPTR;
     }
       
   } while (0);

   return rc;
}

/*********************************************************************
* @purpose  Invalidate the content of the USL Vlan db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t rc = L7_FAILURE;

  if (uslVlanDbActive == L7_FALSE)
  {
    rc = L7_SUCCESS;
    return rc;      
  }

  USL_VLAN_DB_LOCK_TAKE();

  do
  {
    /* Clear the operational table */
    if (flags & USL_OPERATIONAL_DB)
    {
      memset(pUslOperVlanDb, 0, uslVlanDbMaxIdx * sizeof(usl_vlan_db_elem_t));
    }

    /* Clear the shadow table */
    if (flags & USL_SHADOW_DB)
    {
      memset(pUslShadowVlanDb, 0, uslVlanDbMaxIdx * sizeof(usl_vlan_db_elem_t));
    }
    
    rc = L7_SUCCESS;

  } while (0);

  USL_VLAN_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the Vlan Current Db Handle to Operational or Shadow
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_vlan_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_VLAN_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslVlanDbHandle = pUslOperVlanDb;    
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslVlanDbHandle = pUslShadowVlanDb;
  }

  USL_VLAN_DB_LOCK_GIVE();

  return;
}

/* Add the cpu port on all the vlans on mgmt unit */
int usl_vlan_db_dataplane_cleanup()
{

  usl_vlan_db_elem_t  elem, nextElem;
  uslDbElemInfo_t     searchElem, elemInfo;
  int                 rv;
  

  if (uslVlanDbActive == L7_FALSE)
  {
    return BCM_E_FAIL;
  }

  elem.vid = 0;

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_vlan_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM, searchElem, &elemInfo) == BCM_E_NONE)
  {
    rv = usl_bcm_vlan_cpu_port_update(nextElem.vid);
    searchElem.elemIndex = elemInfo.elemIndex;
  }

  return BCM_E_NONE;
}

void usl_vlan_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8           uslStr[128];
  usl_vlan_db_elem_t  elem, nextElem, *dbHandle;
  uslDbElemInfo_t     searchElem, elemInfo;
  L7_uint32           entryCount = 0;


  if (uslVlanDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Vlan database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Vlan database is active\n");
  }

  dbHandle = usl_vlan_db_handle_get(dbType);
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

  elem.vid = 0;

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_vlan_db_elem(dbType, USL_DB_NEXT_ELEM, searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_vlan_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
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

