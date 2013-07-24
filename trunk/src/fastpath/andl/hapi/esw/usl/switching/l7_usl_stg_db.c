/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_stg_db.c
*
* @purpose    USL Stg DB API implementation 
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
#include "l7_usl_stg_db.h"
#include "l7_usl_bcm_port.h"
#include "l7_usl_trace.h"
#include "l7_usl_sm.h"
#include "cnfgr.h"

#include "ibde.h"

/* Db Semaphore */
static void                       *pUslStgDbSema   = L7_NULLPTR;

/* Oper Db */
static usl_stg_db_elem_t          *pUslOperStgDb   = L7_NULLPTR;

/* Shadow Db */
static usl_stg_db_elem_t          *pUslShadowStgDb = L7_NULLPTR;

/* Current Db Handle */
static usl_stg_db_elem_t          *pUslStgDbHandle = L7_NULLPTR;

static L7_uint32                   uslStgDbMaxIdx  = 0;

static L7_BOOL                     uslStgDbActive        = L7_FALSE;

/* Hw Index generation */
static usl_stg_hw_id_list_t       *pUslStgHwIdList = L7_NULLPTR;
static L7_int32                    uslStgHwIdMin = 0, uslStgHwIdMax = 0;


#define USL_STG_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_STG_DB_ID,"STG",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslStgDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n", pUslStgDbSema); \
  } \
}

#define USL_STG_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_STG_DB_ID,"STG",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslStgDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", pUslStgDbSema); \
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
usl_stg_db_elem_t *usl_stg_db_handle_get(USL_DB_TYPE_t dbType)
{
  usl_stg_db_elem_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = pUslOperStgDb;
      break;

    case USL_SHADOW_DB:
      dbHandle = pUslShadowStgDb;
      break;

    case USL_CURRENT_DB:
      dbHandle = pUslStgDbHandle;
      break;

    default:
      break;   
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Get the size of a Stg Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_stg_db_elem(void)
{
  return sizeof(usl_stg_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a stg db element in specified buffer
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
void usl_print_stg_db_elem(void *item, L7_uchar8 *buffer,
                           L7_uint32 size)
{
  L7_uint32          idx;
  usl_stg_db_elem_t *elem = item;

  osapiSnprintf(buffer, size,
                "Stg-id %d App-id %d, Attached Vlan mask:\n",
                elem->stgId, elem->appId);
     
  for (idx = 1; idx < L7_MAX_VLANS; idx++) 
  {
    if (L7_VLAN_ISMASKBITSET(elem->mask, idx))
    {
      osapiSnprintfcat(buffer, size,
                       "%3d ",
                       idx);
    }
  }

  osapiSnprintfcat(buffer, size,
                   "\n");
}

/*********************************************************************
* @purpose  Search an element from stg db and return pointer to
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
* @notes    Internal API. STG DB lock should be held by the caller.
*
* @end
*********************************************************************/
static L7_int32 usl_search_stg_db_elem(USL_DB_TYPE_t dbType,
                                       L7_uint32 flags, 
                                       uslDbElemInfo_t searchInfo,
                                       uslDbElemInfo_t *elemInfo)
{
  usl_stg_db_elem_t  *dbHandle = L7_NULLPTR;
  usl_stg_db_elem_t  *stgElem = searchInfo.dbElem;
  L7_int32            rv = BCM_E_NONE;
  L7_uint32           idx;
  L7_uchar8           traceStr[128];

  
  osapiSnprintf(traceStr, sizeof(traceStr),
                "Search elem: Flags %d index %d\n",
                flags, searchInfo.elemIndex);

  usl_trace_sync_detail(USL_L2_STG_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_stg_db_handle_get(dbType);

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

      while ((idx < uslStgDbMaxIdx) && (dbHandle[idx].isValid == L7_FALSE))
        idx++;

      if (idx < uslStgDbMaxIdx) 
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
      if (stgElem == L7_NULLPTR)
      {  
        rv = BCM_E_FAIL;
        break;  
      }

      for (idx = 0; idx < uslStgDbMaxIdx; idx++)
      {
        if ((dbHandle[idx].isValid == L7_TRUE) &&
            (dbHandle[idx].appId == stgElem->appId))
        {
          elemInfo->dbElem = &(dbHandle[idx]);
          elemInfo->elemIndex = idx;
          rv = BCM_E_NONE;
          break;
        }  
      }

      if (idx >= uslStgDbMaxIdx) 
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
* @notes    Caller must allocate memory to copy the stg db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_stg_db_elem(USL_DB_TYPE_t     dbType, 
                             L7_uint32         flags,
                             uslDbElemInfo_t   searchElem,
                             uslDbElemInfo_t  *elemInfo)
{
  uslDbElemInfo_t     stgElem;
  L7_int32            rv = BCM_E_NONE;

  if (!uslStgDbActive)
  {
    return rv;    
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  stgElem.dbElem = L7_NULLPTR;
  stgElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_STG_DB_LOCK_TAKE();

  do
  {
    /* Search for the elem */
    rv = usl_search_stg_db_elem(dbType, flags, searchElem, &stgElem);
    if (rv == BCM_E_NONE)
    {
      /* Element found, copy in the user memory */
      memcpy(elemInfo->dbElem, stgElem.dbElem, sizeof(usl_stg_db_elem_t));
      memcpy(&(elemInfo->elemIndex), &(stgElem.elemIndex), 
             sizeof(elemInfo->elemIndex)); 
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_STG_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from Stg Db
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
L7_int32 usl_delete_stg_db_elem(USL_DB_TYPE_t   dbType, 
                                uslDbElemInfo_t elemInfo)
{
  usl_stg_db_elem_t  *dbHandle;
  uslDbElemInfo_t     stgElem;
  L7_int32            rv = BCM_E_NONE;

  if (!uslStgDbActive)
  {
    return rv;    
  }

  stgElem.dbElem = L7_NULLPTR;
  stgElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_STG_DB_LOCK_TAKE();

  do
  {
    /* If the elemIndex is specified, then use that to delete the element */
    if ((elemInfo.elemIndex != USL_INVALID_DB_INDEX) && 
        (elemInfo.elemIndex < uslStgDbMaxIdx))
    {
      dbHandle = usl_stg_db_handle_get(dbType);
      if (dbHandle != L7_NULLPTR)
      {
        memset(&(dbHandle[elemInfo.elemIndex]), 0, sizeof(usl_stg_db_elem_t));
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
      rv = usl_search_stg_db_elem(dbType, USL_DB_EXACT_ELEM, elemInfo, &stgElem);
      if (rv == BCM_E_NONE)
      {
        memset(stgElem.dbElem, 0, sizeof(usl_stg_db_elem_t));    
      }
      else
      {
        /* Element not found */
        rv = BCM_E_NOT_FOUND;
      }
    }

  } while (0);
  
  USL_STG_DB_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Create a Stg
*
* @params   dbType @{(input)} Type of db to be modified
* @params   appId @{(input)} Application specific identifier for the stg
* @params   stgId @{(input)} Hardware stg-id
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_create_stg(USL_DB_TYPE_t dbType, L7_uint32 appId, bcm_stg_t stg)
{
  L7_uint32            idx;
  int                  rv = BCM_E_NONE;
  usl_stg_db_elem_t   *dbHandle, elem;
  uslDbElemInfo_t      searchElem, elemInfo;

  if (!uslStgDbActive)
  {
    return rv;      
  }

  elem.appId = appId;

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_STG_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_stg_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* make sure the stg is not in the database */
    rv = usl_search_stg_db_elem(dbType, USL_DB_EXACT_ELEM, searchElem, &elemInfo);

    /* stg does not exists */
    if (rv == BCM_E_NOT_FOUND)
    {
      /* find an available key */
      for (idx = 0; idx < uslStgDbMaxIdx ; idx++)
      {
        if (dbHandle[idx].isValid == L7_FALSE)
        {
          dbHandle[idx].isValid = L7_TRUE;
          dbHandle[idx].stgId = stg;
          dbHandle[idx].appId = appId;
          rv = BCM_E_NONE;
          break;
        }
      }

      /* could not find an available index */
      if (idx == uslStgDbMaxIdx)
      {
        rv = BCM_E_FAIL;
        break;
      } 
    }
    else
    {
      rv = BCM_E_EXISTS;
      break;
    }
  } while (0);

  USL_STG_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a Stg
*
* @params   dbType @{(input)} Type of db to be modified
* @params   appId @{(input)} Application specific identifier for the stg
* @params   stgId  @{(input)} Hardware stg-id
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_destroy_stg(USL_DB_TYPE_t dbType, L7_uint32 appInstId, bcm_stg_t stg)
{
  L7_int32            rv = BCM_E_NONE; 
  usl_stg_db_elem_t    elem;
  uslDbElemInfo_t      elemInfo;
 

  if (!uslStgDbActive)
  {
    return rv;      
  }

  elem.appId = appInstId;

  elemInfo.dbElem = &elem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  rv = usl_delete_stg_db_elem(dbType, elemInfo);

  return rv;
}

/*********************************************************************
* @purpose  Add/Remove a vlan to/from stg
*
* @params   dbType @{(input)} Type of db to be modified
* @params   stg @{(input)} Hardware stg-id
* @params   vid @{(input)} Vlan-id to be added/removed
* @params   cmd @{(input)} Db cmd (Add or Remove)
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_stg_vlan_update(USL_DB_TYPE_t dbType, L7_uint32 appId,
                           bcm_stg_t stg, bcm_vlan_t vid, USL_CMD_t cmd)
{
  L7_int32             rv = BCM_E_NONE;
  usl_stg_db_elem_t    elem;
  uslDbElemInfo_t      searchInfo, elemInfo;

  if (!uslStgDbActive)
  {
    return rv;      
  }

  elem.appId = appId;

  searchInfo.dbElem = &elem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_STG_DB_LOCK_TAKE();

  do
  {
    /* no need to handle stg 1, we don't track it */
    if (stg == 1) break;

    /* Search for the stg in Db */
    rv = usl_search_stg_db_elem(dbType, USL_DB_EXACT_ELEM, searchInfo, &elemInfo);
    if (rv == BCM_E_NONE)
    {
      if (cmd == USL_CMD_ADD)
      {
        L7_VLAN_SETMASKBIT(((usl_stg_db_elem_t *)elemInfo.dbElem)->mask ,vid);
      }
      else if (cmd == USL_CMD_REMOVE)
      {
        L7_VLAN_CLRMASKBIT(((usl_stg_db_elem_t *)elemInfo.dbElem)->mask ,vid);
        break;  
      }
      else /* unsupported cmd */
      {
        rv = BCM_E_FAIL;
        break;
      }
 
    }
    else
    {
      rv = BCM_E_FAIL;
    }
    
  } while (0);


  USL_STG_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create all stg attributes at Usl Bcm layer from the given 
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
L7_int32 usl_create_stg_db_elem_bcm(void *item)
{
  L7_uchar8           tmpVlanMask;
  L7_int32            rv;
  L7_uint32           flags, vidx, pos, vlanId;
  usl_stg_db_elem_t  *elem = item;

  do
  {
    flags = USL_BCM_STG_CREATE_WITH_ID;
    rv = usl_bcm_stg_create(elem->appId, flags, &(elem->stgId));
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      break;  
    }

    /* Add vlans to stg */
    for (vidx = 0; vidx < L7_VLAN_INDICES; vidx++) 
    {
      pos = 0;
	  tmpVlanMask = elem->mask.value[vidx];
	  while (tmpVlanMask) 
	  {
	    pos++;
        
	    if (tmpVlanMask & 0x1)
	    {
	      vlanId = (vidx*8) + pos;
          rv = usl_bcm_stg_vlan_update(elem->appId, elem->stgId, 
                                       vlanId, USL_CMD_ADD);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;  
          }
        }

        tmpVlanMask >>= 1;
      }
       
    }
    
  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete all the stg attributes at the Usl Bcm layer from the 
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
L7_int32 usl_delete_stg_db_elem_bcm(void *item)
{
  L7_int32             rv, i, port;
  usl_stg_db_elem_t   *elem = item;
  bcm_port_config_t    pcfg; 

  /* Set the port state for all the ports to Disable */
  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
  {
    if (SOC_IS_XGS_FABRIC(i)) 
    {
      continue;
    }

    if (bcm_port_config_get(i, &pcfg) == BCM_E_NONE) 
    {
      BCM_PBMP_ITER(pcfg.port, port)
      {
        (void) usl_bcm_stg_stp_set(i, port, elem->stgId, BCM_STG_STP_DISABLE);
      }
    }
  }
  
  /* No need to remove the vlans from stg as this call
  ** takes care of both
  */
  rv = usl_bcm_stg_destroy(elem->appId, elem->stgId);

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
L7_int32 usl_update_stg_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  L7_uchar8               tmpShadowVlanMask, tmpOperVlanMask;
  L7_int32                rv = BCM_E_NONE;
  L7_uint32               vidx, pos, vlanId;
  usl_stg_db_elem_t      *shadowElem = shadowDbItem;
  usl_stg_db_elem_t      *operElem = operDbItem;

  do
  {
    /* If the Oper and Shadow hw stg-id are not same then 
    ** the element cannot be reconciled. Return failure */
    if (operElem->stgId != shadowElem->stgId)
    {
      rv = BCM_E_FAIL;
    }
    else
    {
      /* Compare the Vlan masks */
      for (vidx = 0; vidx < L7_VLAN_INDICES; vidx++) 
      {
        pos = 0;
	    tmpShadowVlanMask = shadowElem->mask.value[vidx];
        tmpOperVlanMask   = operElem->mask.value[vidx];

        /* Mask is same, so continue */
        if (tmpShadowVlanMask == tmpOperVlanMask)
        {
          continue;    
        }

        /* Walk-through all the bits in the mask and resolve differences */
	    while ((tmpShadowVlanMask)  || (tmpOperVlanMask))
	    {
	      pos++;
	      vlanId = (vidx*8) + pos;

          /* Vlan present in shadow and not in oper */
	      if ((tmpShadowVlanMask & 0x1) && (!(tmpOperVlanMask & 0x1)))
	      {
            /* Add vlan to stg */
            rv = usl_bcm_stg_vlan_update(shadowElem->appId, shadowElem->stgId, 
                                         vlanId, USL_CMD_ADD);
            if (L7_BCMX_OK(rv) != L7_TRUE)
            {
              break;  
            }

          }

          /* Vlan present in oper and not in shadow */
	      if ((tmpOperVlanMask & 0x1) && (!(tmpShadowVlanMask & 0x1)))
	      {
            /* Remove vlan from stg */
            rv = usl_bcm_stg_vlan_update(shadowElem->appId, shadowElem->stgId, 
                                         vlanId, USL_CMD_REMOVE);
            if (L7_BCMX_OK(rv) != L7_TRUE)
            {
              break;  
            }

          }

          tmpShadowVlanMask >>= 1;
          tmpOperVlanMask >>= 1;
        }
      }
    }

  } while (0);

  return rv;

}

static L7_RC_t usl_stg_db_alloc(usl_stg_db_elem_t **stgDb)
{

  *stgDb = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                  sizeof(usl_stg_db_elem_t) * uslStgDbMaxIdx);
  if (*stgDb == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset((void*)*stgDb, 0, sizeof(usl_stg_db_elem_t) * uslStgDbMaxIdx);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Create the USL STG db
*
* @params   none
*
* @returns  none
*
* @notes    USL Stg database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
L7_RC_t usl_stg_db_init(void)
{
  L7_RC_t          rc = L7_FAILURE;
  uslDbSyncFuncs_t stgDbFuncs;

  memset(&stgDbFuncs, 0, sizeof(stgDbFuncs));
  
  /* +1 for CIST */
  uslStgDbMaxIdx  = platDot1sInstancesMaxEntriesGet() + 1;

  /* Database is present only on stackables */

  do
  {
    /* Allocate STG database only on Stacking platform */
    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
           
      pUslStgDbSema   = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if ( pUslStgDbSema == L7_NULLPTR ) break;

      if (usl_stg_db_alloc(&pUslOperStgDb) != L7_SUCCESS)
      {
        break;    
      }

      /* Allocate shadow table for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, 
                                L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        if (usl_stg_db_alloc(&pUslShadowStgDb) != L7_SUCCESS)
        {
          break;    
        }
      }

      /* Register the sync routines */
      stgDbFuncs.get_size_of_db_elem =  usl_get_size_of_stg_db_elem;
      stgDbFuncs.get_db_elem = usl_get_stg_db_elem;
      stgDbFuncs.delete_elem_from_db =  usl_delete_stg_db_elem;
      stgDbFuncs.print_db_elem = usl_print_stg_db_elem;
      stgDbFuncs.create_usl_bcm = usl_create_stg_db_elem_bcm;
      stgDbFuncs.delete_usl_bcm = usl_delete_stg_db_elem_bcm;
      stgDbFuncs.update_usl_bcm = usl_update_stg_db_elem_bcm;


      /* Not registering optional routines as the db element is deterministic 
      ** i.e no pointer member elements
      */
      stgDbFuncs.alloc_db_elem = L7_NULLPTR;
      stgDbFuncs.copy_db_elem  = L7_NULLPTR; 
      stgDbFuncs.free_db_elem  = L7_NULLPTR;
      stgDbFuncs.pack_db_elem  = L7_NULLPTR;
      stgDbFuncs.unpack_db_elem = L7_NULLPTR;

      if (usl_db_sync_func_table_register(USL_L2_STG_DB_ID, 
                                          &stgDbFuncs) != L7_SUCCESS)
      {
        LOG_ERROR(0);   
      }


      pUslStgDbHandle = pUslOperStgDb;
      uslStgDbActive = L7_TRUE;

    } /* End if cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) */

    rc = L7_SUCCESS;
  }
  while (0);

  return rc;
}

/*********************************************************************
* @purpose  De-allocate the USL STG db
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t usl_stg_db_fini(void)
{
  L7_RC_t   rc = L7_SUCCESS;

  if (!uslStgDbActive)
  {
    return L7_SUCCESS; 
  }

  uslStgDbActive = L7_FALSE;
  pUslStgDbHandle = L7_NULLPTR;

  /* free the STG resources */
  if (pUslStgDbSema != L7_NULLPTR)
  {
    osapiSemaDelete(pUslStgDbSema);
    pUslStgDbSema = L7_NULLPTR;
  }

  if (pUslOperStgDb != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperStgDb);
  }


  if (pUslShadowStgDb != L7_NULLPTR)
  {
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowStgDb);
  }

  return rc;
}

/*********************************************************************
* @purpose  Invalidate the content of the USL Stg db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_stg_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t   rc = L7_SUCCESS;

  if (uslStgDbActive == L7_FALSE)
  {
    rc = L7_SUCCESS;
    return rc;      
  }

  USL_STG_DB_LOCK_TAKE();

  do
  {
    /* Clear the operational table */
    if (flags & USL_OPERATIONAL_DB)
    {
      memset(pUslOperStgDb, 0, sizeof(usl_stg_db_elem_t) * uslStgDbMaxIdx); 
    }

    /* Clear the Shadow table */
    if (flags & USL_SHADOW_DB)
    {
      memset(pUslShadowStgDb, 0, sizeof(usl_stg_db_elem_t) * uslStgDbMaxIdx); 
    }

  } while (0);

  USL_STG_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the Stg Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_stg_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_STG_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslStgDbHandle = pUslOperStgDb;    
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslStgDbHandle = pUslShadowStgDb;        
  }

  USL_STG_DB_LOCK_GIVE();

  return;
}

void usl_stg_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8           uslStr[768];
  usl_stg_db_elem_t   elem, nextElem, *dbHandle;
  uslDbElemInfo_t     searchElem, elemInfo;
  L7_uint32           entryCount = 0;


  if (uslStgDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Stg database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Stg database is active\n");
  }

  dbHandle = usl_stg_db_handle_get(dbType);
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

  elem.appId = 0;

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_stg_db_elem(dbType, USL_DB_NEXT_ELEM, searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_stg_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
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
* @purpose  Initialize stg hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_stg_hw_id_generator_init(void)
{
  L7_uint32 max;

  /* +1 for CIST, +1 for SDK Reserved */
  max = platDot1sInstancesMaxEntriesGet() + 2; 

  /* There is currently no API to get the range of instance number that SDK expects.
  ** Until such API is available we use the index from 0 to max number of 
  ** spanning-tree instances supported by FASTPATH
  */
  uslStgHwIdMin = 0;
  uslStgHwIdMax = max - 1;

  pUslStgHwIdList = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                sizeof(usl_stg_hw_id_list_t) * (uslStgHwIdMax + 1));
  if (pUslStgHwIdList == L7_NULLPTR) 
    return L7_FAILURE;

  memset(pUslStgHwIdList, 0, sizeof(usl_stg_hw_id_list_t) * (uslStgHwIdMax + 1));

  /* SDK reserves stgid 0 and 1 for default spanning-tree group */
  pUslStgHwIdList[0].used = pUslStgHwIdList[1].used = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset stg hw id generator
*
* @params   None
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_stg_hw_id_generator_reset()
{
  L7_RC_t rc = L7_SUCCESS;

  /* Clear the StgHwIdList array */
  memset(pUslStgHwIdList, 0, sizeof(usl_stg_hw_id_list_t) * 
                                          (uslStgHwIdMax + 1));

  /* SDK reserves stgid 0 and 1 for default spanning-tree group */
  pUslStgHwIdList[0].used = pUslStgHwIdList[1].used = L7_TRUE;

  return rc;
}

/*********************************************************************
* @purpose  Synchronize the Stg hwId generator with contents of Oper Db
*
* @params   None
*
* @returns  L7_RC_t. Assumes BCMX is suspended.
*
* @end
*********************************************************************/
L7_RC_t usl_stg_hw_id_generator_sync()
{
  usl_stg_db_elem_t searchStgElem, stgElem;
  uslDbElemInfo_t   searchInfo, elemInfo;
  bcm_stg_t         stg;
  
  memset(&searchStgElem, 0, sizeof(searchStgElem));
  memset(&stgElem, 0, sizeof(stgElem));

  searchStgElem.appId = 0;
  searchInfo.dbElem = (void *) &searchStgElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &stgElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_stg_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM,
                             searchInfo, &elemInfo) == BCM_E_NONE)
  {
    memcpy(searchInfo.dbElem, elemInfo.dbElem, 
           sizeof(usl_stg_db_elem_t));
    memcpy(&(searchInfo.elemIndex), &(elemInfo.elemIndex), 
           sizeof(elemInfo.elemIndex));

    stg = ((usl_stg_db_elem_t *)elemInfo.dbElem)->stgId;

    if ((stg < uslStgHwIdMin) || (stg > uslStgHwIdMax))
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Invalid stg-id %d for appId %d\n",
                  stg, ((usl_stg_db_elem_t *)elemInfo.dbElem)->appId);
      continue;    
    }

    
    /* Mark this index as used in StgHwList */    
    pUslStgHwIdList[stg].used = L7_TRUE;
       
  }

  return L7_SUCCESS;
}

void usl_stg_hw_id_generator_dump()
{ 
  L7_uint32 idx, entryCount = 0;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Stg Min Index %d Max Index %d Used Hw Indexes: ",
                uslStgHwIdMin, uslStgHwIdMax);
  for (idx = uslStgHwIdMin; idx <= uslStgHwIdMax; idx++)
  {
    if (pUslStgHwIdList[idx].used == L7_TRUE)
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
* @purpose  Allocate a stgId for given appId
*
* @param    appId      @{(input)} Application identifier for STG
* @param    stg        @{(output)} Hw stg-id
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_stg_hw_id_allocate(L7_uint32 appId, bcm_stg_t *stg)
{
  int               rv = BCM_E_NONE;
  usl_stg_db_elem_t searchStgElem, stgElem;
  L7_int32          idx;
  uslDbElemInfo_t   searchInfo, elemInfo;

  *stg = BCM_STG_INVALID;

  memset(&searchStgElem, 0, sizeof(searchStgElem));
  memset(&stgElem, 0, sizeof(stgElem));

  searchStgElem.appId = appId;
  searchInfo.dbElem = (void *) &searchStgElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  
  elemInfo.dbElem = &stgElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  do
  {
    /* If USL is in warm start state, then search the operational db for this stg.
    ** If Stg is not found then an unused index is generated.
    */
    
    if (usl_state_get() == USL_WARM_START_STATE)
    {
      rv = usl_get_stg_db_elem(USL_OPERATIONAL_DB, USL_DB_EXACT_ELEM, 
                               searchInfo, &elemInfo);
      if (rv == BCM_E_NONE)
      {
        *stg = ((usl_stg_db_elem_t *)elemInfo.dbElem)->stgId;

        if (*stg == BCM_STG_INVALID)
        {
          LOG_ERROR(*stg);    
        }

        /* Mark this index as used in StgHwList */    
        pUslStgHwIdList[*stg].used = L7_TRUE;
        break;
      }
    }

    for (idx = uslStgHwIdMin; idx <= uslStgHwIdMax; idx++)
    {
      /* Found an unused index */
      if (pUslStgHwIdList[idx].used == L7_FALSE)
      {
        pUslStgHwIdList[idx].used = L7_TRUE;    
        *stg = idx;
        rv = BCM_E_NONE;
        break;
      }
    }
  } while(0);

  if (*stg == BCM_STG_INVALID)
  {
    rv = BCM_E_FULL;  
  }

  return rv;
}

/*********************************************************************
* @purpose  Free a given stgId 
*
* @param    stg        @{(input)} Hw stg-id
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_stg_hw_id_free(bcm_stg_t stg)
{
  int rv = BCM_E_NONE;

  if ((stg < uslStgHwIdMin) || (stg > uslStgHwIdMax))
  {
    rv = BCM_E_FAIL;
  }
  else
  {
    pUslStgHwIdList[stg].used = L7_FALSE;
  }

  return rv;
}

