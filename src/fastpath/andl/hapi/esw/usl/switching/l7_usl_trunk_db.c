/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_trunk_db.c
*
* @purpose    USL Trunk DB API implementation 
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
#include "l7_usl_trunk_db.h"
#include "l7_usl_trace.h"
#include "l7_usl_sm.h"
#include "cnfgr.h"

#include "ibde.h"

#ifdef L7_STACKING_PACKAGE
#include "l7_usl_macsync.h"
#endif

#include "logger.h" /* PTin added: trunks */

/* Trunk Table resources */
static void                    *pUslTrunkDbSema   = L7_NULLPTR;

/* Oper Db */
static usl_trunk_db_elem_t     *pUslOperTrunkDb = L7_NULLPTR;

/* Shadow Db */
static usl_trunk_db_elem_t     *pUslShadowTrunkDb = L7_NULLPTR;

/* Current db handle */
static usl_trunk_db_elem_t     *pUslTrunkDbHandle = L7_NULLPTR;

static L7_uint32               uslTrunkDbMaxIdx;

static L7_BOOL                 uslTrunkDbActive  = L7_FALSE;

/* Trunk Hw-id generator */

usl_trunk_hw_id_list_t *pUslTrunkHwIdList = L7_NULLPTR;

L7_int32                uslTrunkHwIdMin = 0, uslTrunkHwIdMax = 0;

#ifdef L7_STACKING_PACKAGE
L7_uint32 uslTrunkDataplaneCleanupTime  = 0; /* Total time spent doing dataplane cleanups */
L7_uint32 uslTrunkDataplaneCleanupCount = 0; /* Total dataplane cleanups */
#endif

#define USL_TRUNK_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_TRUNK_DB_ID,"LAG",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslTrunkDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n", pUslTrunkDbSema); \
  } \
}

#define USL_TRUNK_DB_LOCK_GIVE() \
  { \
    usl_trace_sema(USL_L2_TRUNK_DB_ID,"LAG",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
    if (osapiSemaGive(pUslTrunkDbSema) != L7_SUCCESS) \
    { \
      USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", pUslTrunkDbSema); \
    } \
  }

extern void usl_trunk_learn_mode_set(L7_uint32 tgid, L7_uint32 learningLocked);
extern L7_BOOL usl_trunk_learn_mode_get(L7_uint32 tgid);



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
usl_trunk_db_elem_t *usl_trunk_db_handle_get(USL_DB_TYPE_t dbType)
{
  usl_trunk_db_elem_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = pUslOperTrunkDb;
      break;

    case USL_SHADOW_DB:
      dbHandle = pUslShadowTrunkDb;
      break;

    case USL_CURRENT_DB:
      dbHandle = pUslTrunkDbHandle;
      break;

    default:
      break;   
  }

  return dbHandle;
}


/*********************************************************************
* @purpose  Get the size of a Trunk Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_trunk_db_elem(void)
{
  return sizeof(usl_trunk_db_elem_t);
}

/*********************************************************************
* @purpose  Print the contents of a trunk db element in specified buffer
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
void usl_print_trunk_db_elem(void *item, L7_uchar8 *buffer,
                             L7_uint32 size)
{
  L7_uint32 i;
  usl_trunk_db_elem_t *elem = item;

  osapiSnprintf(buffer, size,
                "Trunk-id %d App-id %d\n",
                elem->tid, elem->appId);

  osapiSnprintfcat(buffer, size,
                   "PSC=%d,DLF Index =%d,MC Index=%d,IPMC Index=%d\n",
                   elem->add_info.psc,
				   elem->add_info.dlf_index,
                   elem->add_info.mc_index,
				   elem->add_info.ipmc_index);

  osapiSnprintfcat(buffer, size,
                   "Member ports in the Trunk(): ");

  for (i = 0; i < elem->add_info.num_ports; i++)
  {
    osapiSnprintfcat(buffer, size,
                     "Modid/Modport %d/%d, ", 
                     elem->add_info.tm[i],
                     elem->add_info.tp[i]);

  }
  
  osapiSnprintfcat(buffer, size, "\n");

}  

/*********************************************************************
* @purpose  Search an element in trunk db and return pointer to
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
* @notes    Internal API. Trunk DB lock should be held by the caller.
*
* @end
*********************************************************************/
static L7_int32 usl_search_trunk_db_elem(USL_DB_TYPE_t dbType,
                                         L7_uint32 flags, 
                                         uslDbElemInfo_t searchInfo,
                                         uslDbElemInfo_t *elemInfo)
{
  usl_trunk_db_elem_t  *dbHandle = L7_NULLPTR;
  usl_trunk_db_elem_t  *trunkElem = searchInfo.dbElem;
  L7_int32              rv = BCM_E_NONE;
  L7_uint32             idx;
  L7_uchar8             traceStr[128];

  
  osapiSnprintf(traceStr, sizeof(traceStr),
                "Search elem: Flags %d index %d\n",
                flags, searchInfo.elemIndex);

  usl_trace_sync_detail(USL_L2_TRUNK_DB_ID, traceStr);


  elemInfo->dbElem = L7_NULLPTR;
  elemInfo->elemIndex = USL_INVALID_DB_INDEX;

  do
  {

    dbHandle = usl_trunk_db_handle_get(dbType);

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

      while ((idx < uslTrunkDbMaxIdx) && (dbHandle[idx].isValid == L7_FALSE))
        idx++;

      if (idx < uslTrunkDbMaxIdx) 
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
      if (trunkElem == L7_NULLPTR)
      {
        rv = BCM_E_FAIL;
        break;          
      }

      for (idx = 0; idx < uslTrunkDbMaxIdx; idx++)
      {
        if ((dbHandle[idx].isValid == L7_TRUE) &&
            (dbHandle[idx].appId == trunkElem->appId))
        {
          elemInfo->dbElem = &(dbHandle[idx]);
          elemInfo->elemIndex = idx;
          rv = BCM_E_NONE;
          break;
        }  
      }

      if (idx >= uslTrunkDbMaxIdx) 
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
L7_int32 usl_get_trunk_db_elem(USL_DB_TYPE_t     dbType, 
                               L7_uint32         flags,
                               uslDbElemInfo_t   searchElem,
                               uslDbElemInfo_t  *elemInfo)
{
  uslDbElemInfo_t     trunkElem;
  L7_int32            rv = BCM_E_NONE;

  if (!uslTrunkDbActive)
  {
    return rv;    
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  trunkElem.dbElem = L7_NULLPTR;
  trunkElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_TRUNK_DB_LOCK_TAKE();

  do
  {
    /* Search for the elem */
    rv = usl_search_trunk_db_elem(dbType, flags, searchElem, &trunkElem);
    if (rv == BCM_E_NONE)
    {
      /* Element found, copy in the user memory */
      memcpy(elemInfo->dbElem, trunkElem.dbElem, sizeof(usl_trunk_db_elem_t));
      memcpy(&(elemInfo->elemIndex), &(trunkElem.elemIndex), 
             sizeof(elemInfo->elemIndex)); 
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_TRUNK_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from Trunk Db
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
L7_int32 usl_delete_trunk_db_elem(USL_DB_TYPE_t   dbType, 
                                  uslDbElemInfo_t elemInfo)
{
  usl_trunk_db_elem_t  *dbHandle;
  uslDbElemInfo_t       trunkElem;
  L7_int32              rv = BCM_E_NONE;

  if (!uslTrunkDbActive)
  {
    return rv;    
  }

  trunkElem.dbElem = L7_NULLPTR;
  trunkElem.elemIndex = USL_INVALID_DB_INDEX;

  USL_TRUNK_DB_LOCK_TAKE();

  do
  {
    /* If the elemIndex is specified, then use that to delete the element */
    if ((elemInfo.elemIndex != USL_INVALID_DB_INDEX) && 
        (elemInfo.elemIndex < uslTrunkDbMaxIdx))
    {
      dbHandle = usl_trunk_db_handle_get(dbType);
      if (dbHandle != L7_NULLPTR)
      {
        memset(&(dbHandle[elemInfo.elemIndex]), 0, sizeof(usl_trunk_db_elem_t));
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
      rv = usl_search_trunk_db_elem(dbType, USL_DB_EXACT_ELEM, elemInfo, &trunkElem);
      if (rv == BCM_E_NONE)
      {
        memset(trunkElem.dbElem, 0, sizeof(usl_trunk_db_elem_t));    
      }
      else
      {
        /* Element not found */
        rv = BCM_E_NOT_FOUND;
      }
    }

  } while (0);
  
  USL_TRUNK_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the laern mode for a trunk
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
* @params   learnMode @{(input)} Learn mode
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_trunk_learn_mode_set(USL_DB_TYPE_t dbType, L7_uint32 appId,
                                bcm_trunk_t tid, L7_BOOL learningLocked)
{
  int                  rv = BCM_E_NONE;
  usl_trunk_db_elem_t  elem;
  uslDbElemInfo_t      searchElem, elemInfo;

  /* Update the db only if it is active */
  if (!uslTrunkDbActive)
  {
    return rv; 
  }

  memset(&elem, 0, sizeof(elem));
  elem.appId = appId;
 
  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_TRUNK_DB_LOCK_TAKE();

  rv = usl_search_trunk_db_elem(dbType, USL_DB_EXACT_ELEM, searchElem, &elemInfo);

  if (rv == BCM_E_NONE)
  {
    ((usl_trunk_db_elem_t *)elemInfo.dbElem)->learningLocked  = learningLocked;
  }
  else
  {
    rv = BCM_E_FAIL;
  }

  USL_TRUNK_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the hash mode for a trunk
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
* @params   psc       @{(input)} Trunk hash index
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_trunk_hash_set(USL_DB_TYPE_t dbType, L7_uint32 appId, 
                          bcm_trunk_t tid, int psc)
{
  int                  rv = BCM_E_NONE;
  usl_trunk_db_elem_t  elem;
  uslDbElemInfo_t      searchElem, elemInfo;

  /* Update the db only if it is active */
  if (!uslTrunkDbActive)
  {
    return rv; 
  }

  memset(&elem, 0, sizeof(elem));
  elem.appId = appId;
 
  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_TRUNK_DB_LOCK_TAKE();

  rv = usl_search_trunk_db_elem(dbType, USL_DB_EXACT_ELEM, searchElem, &elemInfo);

  if (rv == BCM_E_NONE)
  {
    ((usl_trunk_db_elem_t *)elemInfo.dbElem)->add_info.psc  = psc;
  }
  else
  {
    rv = BCM_E_FAIL;
  }

  USL_TRUNK_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create a trunk in the USL db
*
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_create_trunk(USL_DB_TYPE_t dbType, L7_uint32 appId, bcm_trunk_t tid)
{
  L7_uint32            idx;
  int                  rv = BCM_E_FAIL;
  usl_trunk_db_elem_t  elem, *dbHandle;
  uslDbElemInfo_t      searchElem, elemInfo;

  /* Update the db only if it is active */
  if (!uslTrunkDbActive)
  {
    rv = BCM_E_NONE;
    return rv; 
  }

  memset(&elem, 0, sizeof(elem));
  elem.appId = appId;
 
  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_TRUNK_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_trunk_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;     
    }

    /* make sure the Trunk is not in the database */
    rv = usl_search_trunk_db_elem(dbType, USL_DB_EXACT_ELEM, searchElem, &elemInfo);

    if (rv == BCM_E_NOT_FOUND)
    {
      for (idx = 0; idx < uslTrunkDbMaxIdx ; idx++)
      {
        if (dbHandle[idx].isValid == L7_FALSE)
        {
          dbHandle[idx].isValid = L7_TRUE;
          dbHandle[idx].tid     = tid;
          dbHandle[idx].appId   = appId;
          rv = BCM_E_NONE;
          break; 
        }
      }

      
      /* could not find an available index */
      if (idx == uslTrunkDbMaxIdx)
      {
        rv = BCM_E_FAIL;
        break;
      } 
    }
  } while(0);

  USL_TRUNK_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a trunk from the USL db
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_delete_trunk(USL_DB_TYPE_t dbType, L7_uint32 appId, 
                        bcm_trunk_t tid)
{
  int                    rv = BCM_E_NONE;
  uslDbElemInfo_t        elemInfo;
  usl_trunk_db_elem_t    elem;

  /* Update the db only if it is active */
  if (!uslTrunkDbActive)
  {
    return rv; 
  }

  elem.appId = appId;
  elemInfo.dbElem = &elem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  rv = usl_delete_trunk_db_elem(dbType, elemInfo);

  return rv;
}


/*********************************************************************
* @purpose  Update a trunk information in the USL db
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
* @params   add_info  @{(input)} Trunk information
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_update_trunk_info(USL_DB_TYPE_t dbType, L7_uint32 appId,
                             bcm_trunk_t tid, bcm_trunk_add_info_t * add_info)
{
  L7_int32             rv = BCM_E_NONE;
  usl_trunk_db_elem_t  elem;
  uslDbElemInfo_t      searchElem, elemInfo;

  /* Update the db only if it is active */
  if (!uslTrunkDbActive)
  {
    return rv; 
  }

  memset(&elem, 0, sizeof(elem));
  elem.appId = appId;
 
  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_TRUNK_DB_LOCK_TAKE();

  rv = usl_search_trunk_db_elem(dbType, USL_DB_EXACT_ELEM, searchElem, &elemInfo);

  if (rv == BCM_E_NONE)
  {
    memcpy(&(((usl_trunk_db_elem_t *)elemInfo.dbElem)->add_info), add_info, 
           sizeof(*add_info));
  }
  else
  {
    rv = BCM_E_FAIL;
  }

  USL_TRUNK_DB_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Create all trunk attributes at Usl Bcm layer from the given 
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
L7_int32 usl_create_trunk_db_elem_bcm(void *item)
{
  L7_int32               rv;
  L7_uint32              flags;
  usl_trunk_db_elem_t   *elem = item;

  do
  {
    flags = USL_BCM_TRUNK_CREATE_WITH_ID;
    rv = usl_bcm_trunk_create(elem->appId, flags, &(elem->tid));
    if (L7_BCMX_OK(rv) != L7_TRUE)
      break;

    rv = usl_bcm_trunk_set(elem->appId, elem->tid, &(elem->add_info));
    if (L7_BCMX_OK(rv) != L7_TRUE)
      break;
   
    usl_trunk_learn_mode_set(elem->tid, elem->learningLocked);
    usl_db_trunk_learn_mode_set(USL_CURRENT_DB, elem->appId, 
                                elem->tid, elem->learningLocked);
    
  } while (0);

  return rv;
}

/*********************************************************************
* @purpose  Delete all the trunk attributes at the Usl Bcm layer from the 
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
L7_int32 usl_delete_trunk_db_elem_bcm(void *item)
{
  usl_trunk_db_elem_t *elem = item;
  L7_int32             rv;

  /* Destroying trunk takes care of removing all the member ports */
  rv = usl_bcm_trunk_destroy(elem->appId, elem->tid);

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
L7_int32 usl_update_trunk_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  
  L7_int32                 rv = BCM_E_NONE;
  usl_trunk_db_elem_t     *shadowElem = shadowDbItem;
  usl_trunk_db_elem_t     *operElem = operDbItem;

  do
  {
    /* If the Oper and Shadow hw trunk-id are not same then 
    ** the element cannot be reconciled. Return failure */
    if (operElem->tid != shadowElem->tid)
    {
      rv = BCM_E_FAIL;
    }
    else
    {
      /* Resolve other differences */
      if ((memcmp(&(shadowElem->add_info), &(operElem->add_info),
                   sizeof(shadowElem->add_info))) != 0)
      {
        /* update using shadow elem */
        rv = usl_bcm_trunk_set(shadowElem->appId, shadowElem->tid, 
                               &(shadowElem->add_info));    
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;    
        }
      }

      if (shadowElem->learningLocked != operElem->learningLocked)
      {
        usl_trunk_learn_mode_set(shadowElem->tid, shadowElem->learningLocked);
        usl_db_trunk_learn_mode_set(USL_CURRENT_DB, shadowElem->appId, 
                                    shadowElem->tid, shadowElem->learningLocked);
      }
    }
     
  } while (0);

  return rv;
}

static L7_RC_t usl_trunk_db_alloc(usl_trunk_db_elem_t **trunkDb)
{
  *trunkDb = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                  sizeof(usl_trunk_db_elem_t) * uslTrunkDbMaxIdx);
  if (*trunkDb == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset((void*)*trunkDb, 0, uslTrunkDbMaxIdx*sizeof(usl_trunk_db_elem_t));

  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Create the USL Trunk db
*
* @params   none
*
* @returns  none
*
* @notes    USL Trunk database is allocated on Stacking Platform
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_db_init(void)
{
  L7_RC_t            rc = L7_FAILURE;
  uslDbSyncFuncs_t   trunkDbFuncs;

  memset(&trunkDbFuncs, 0, sizeof(trunkDbFuncs));

  uslTrunkDbMaxIdx = platIntfLagIntfMaxCountGet();

  do
  {

    pUslTrunkDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslTrunkDbSema == L7_NULLPTR ) break;

    if (usl_trunk_db_alloc(&pUslOperTrunkDb) != L7_SUCCESS)
    {
      break;    
    }

    /* Allocate shadow tables for NSF feature */
    if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, 
                              L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
    {
      if (usl_trunk_db_alloc(&pUslShadowTrunkDb) != L7_SUCCESS)
      {
        break;    
      }
    }

    /* Register the sync routines */
    trunkDbFuncs.get_size_of_db_elem =  usl_get_size_of_trunk_db_elem;
    trunkDbFuncs.get_db_elem = usl_get_trunk_db_elem;
    trunkDbFuncs.delete_elem_from_db =  usl_delete_trunk_db_elem;
    trunkDbFuncs.print_db_elem = usl_print_trunk_db_elem;
    trunkDbFuncs.create_usl_bcm = usl_create_trunk_db_elem_bcm;
    trunkDbFuncs.delete_usl_bcm = usl_delete_trunk_db_elem_bcm;
    trunkDbFuncs.update_usl_bcm = usl_update_trunk_db_elem_bcm;

    /* Not registering optional routines as the db element is deterministic 
    ** i.e no pointer member elements
    */
    trunkDbFuncs.alloc_db_elem = L7_NULLPTR;
    trunkDbFuncs.copy_db_elem  = L7_NULLPTR; 
    trunkDbFuncs.free_db_elem  = L7_NULLPTR;
    trunkDbFuncs.pack_db_elem  = L7_NULLPTR;
    trunkDbFuncs.unpack_db_elem = L7_NULLPTR;

    if (usl_db_sync_func_table_register(USL_L2_TRUNK_DB_ID, 
                                        &trunkDbFuncs) != L7_SUCCESS)
    {
      LOG_ERROR(0);   
    }


    pUslTrunkDbHandle = pUslOperTrunkDb;
    uslTrunkDbActive = L7_TRUE;

    rc = L7_SUCCESS; 

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Deallocate the USL Trunk db
*
* @params   none
*
* @returns  none
*
* @notes    USL Trunk database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_db_fini(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    uslTrunkDbActive = L7_FALSE;
    pUslTrunkDbHandle = L7_NULLPTR;

    if (pUslTrunkDbSema  != L7_NULLPTR)
    {
      osapiSemaDelete(pUslTrunkDbSema);
      pUslTrunkDbSema  = L7_NULLPTR;
    }

    if (pUslOperTrunkDb != L7_NULLPTR)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperTrunkDb);
      pUslOperTrunkDb = L7_NULLPTR;
    }

    if (pUslShadowTrunkDb != L7_NULLPTR)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowTrunkDb);
      pUslShadowTrunkDb = L7_NULLPTR;
    }

    rc = L7_SUCCESS; 

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Delete all the trunks from the hardware
*
* @params   
*
* @returns  L7_RC_t
*
* @notes    Works on the content of Operational Db. Database contents
*           are not cleared. Called from hpcHardwareDriverReset as
*           bcm_clear would not cleanup trunk.
*
* @end
*********************************************************************/
void usl_trunk_clear()
{
  L7_int32             rv = BCM_E_NONE;
  L7_uint32            unit;
  usl_trunk_db_elem_t  elem;
  uslDbElemInfo_t      searchElem, elemInfo;

  /* Update the db only if it is active */
  if (!uslTrunkDbActive)
  {
    return; 
  }

  memset(&elem, 0, sizeof(elem));
 
  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = L7_NULLPTR;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  USL_TRUNK_DB_LOCK_TAKE();


  /* Walk through all the valid trunks in Oper Db and delete from Hw */
  while (usl_search_trunk_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM, 
                                  searchElem, &elemInfo) == BCM_E_NONE)
  {
    for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
    {
      if (!SOC_IS_XGS_FABRIC(unit))
      {
        rv = bcm_trunk_destroy(unit, 
                               ((usl_trunk_db_elem_t *)elemInfo.dbElem)->tid);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          USL_LOG_MSG(USL_BCM_E_LOG,
                      "Trunk %d destroy failed, rv %d\n", 
                      ((usl_trunk_db_elem_t *)elemInfo.dbElem)->tid, 
                      rv);    
        }
      }
    }

    memcpy(searchElem.dbElem, elemInfo.dbElem, sizeof(usl_trunk_db_elem_t));
    memcpy(&(searchElem.elemIndex), &(elemInfo.elemIndex),
            sizeof(elemInfo.elemIndex));
  } 

  USL_TRUNK_DB_LOCK_GIVE();

  return;
}
  
/*********************************************************************
* @purpose  Invalidate the content of the USL trunk db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t rc = L7_FAILURE;

  if (uslTrunkDbActive == L7_FALSE)
  {
    rc = L7_SUCCESS;
    return rc;      
  }

  /* Acquire the Db lock */
  USL_TRUNK_DB_LOCK_TAKE();

  do
  {
    /* Clear the operational table */
    if (flags & USL_OPERATIONAL_DB)
    {
      memset(pUslOperTrunkDb, 0, 
             sizeof(usl_trunk_db_elem_t) * uslTrunkDbMaxIdx);
    }

    /* Clear the shadow table */
    if (flags & USL_SHADOW_DB)
    {
      memset(pUslShadowTrunkDb, 0, 
             sizeof(usl_trunk_db_elem_t) * uslTrunkDbMaxIdx);
    }
    
    rc = L7_SUCCESS;

  } while (0);


  USL_TRUNK_DB_LOCK_GIVE();

  return rc;

}

/*********************************************************************
* @purpose  Set the Trunk Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_trunk_current_db_handle_set(USL_DB_TYPE_t dbType)
{
  USL_TRUNK_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslTrunkDbHandle = pUslOperTrunkDb;    
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslTrunkDbHandle = pUslShadowTrunkDb;        
  }

  USL_TRUNK_DB_LOCK_GIVE();

  return;
}



void usl_trunk_db_dump(USL_DB_TYPE_t dbType)
{
  L7_uchar8             uslStr[512];
  usl_trunk_db_elem_t   elem, nextElem, *dbHandle;
  uslDbElemInfo_t       searchElem, elemInfo;
  L7_uint32             entryCount = 0;


  if (uslTrunkDbActive == L7_FALSE)
  {

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Trunk database is not active\n");
    return;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Trunk database is active\n");
  }

  dbHandle = usl_trunk_db_handle_get(dbType);
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

  elem.appId = 0;

  searchElem.dbElem = &elem;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &nextElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_trunk_db_elem(dbType, USL_DB_NEXT_ELEM, 
                               searchElem, &elemInfo) == BCM_E_NONE)
  {
    usl_print_trunk_db_elem(elemInfo.dbElem, uslStr, sizeof(uslStr));
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
* @purpose  Initialize trunk hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_hw_id_generator_init(void)
{
  L7_uint32 max;

  /* Application max Lag count */
  max = platIntfLagIntfMaxCountGet();


  /* There is currently no API to get the range of instance number that SDK expects.
  ** Until such API is available we use the index from 0 to max number of 
  ** Lags supported by FASTPATH
  */
  uslTrunkHwIdMin = 0;
  uslTrunkHwIdMax = max - 1;

  pUslTrunkHwIdList = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                                  sizeof(usl_trunk_hw_id_list_t) * (uslTrunkHwIdMax + 1));
  if (pUslTrunkHwIdList == L7_NULLPTR) 
    return L7_FAILURE;

  memset(pUslTrunkHwIdList, 0, sizeof(usl_trunk_hw_id_list_t) * (uslTrunkHwIdMax + 1));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset trunk hw id generator
*
* @params   none
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_hw_id_generator_reset()
{
  L7_RC_t rc = L7_SUCCESS;

  /* Clear the TrunkHwIdList array */
  memset(pUslTrunkHwIdList, 0, 
         sizeof(usl_trunk_hw_id_list_t) * (uslTrunkHwIdMax + 1));

  return rc;
}


/*********************************************************************
* @purpose  Synchronize the trunk hwId generator with contents of Oper Db
*
* @params   None
*
* @returns  L7_RC_t. Assumes BCMX is suspended.
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_hw_id_generator_sync()
{
  usl_trunk_db_elem_t searchElem, trunkElem;
  uslDbElemInfo_t     searchInfo, elemInfo;
  bcm_trunk_t         tid;

  memset(&searchElem, 0, sizeof(searchElem));
  memset(&trunkElem, 0, sizeof(trunkElem));

  searchElem.appId = 0;
  searchInfo.dbElem =  &searchElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &trunkElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_trunk_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM,
                               searchInfo, &elemInfo) == BCM_E_NONE)
  {
  
    memcpy(searchInfo.dbElem, elemInfo.dbElem, 
           sizeof(usl_trunk_db_elem_t));
    memcpy(&(searchInfo.elemIndex), &(elemInfo.elemIndex), 
            sizeof(elemInfo.elemIndex));

    tid = ((usl_trunk_db_elem_t *)elemInfo.dbElem)->tid;
 
    if ((tid < uslTrunkHwIdMin) || (tid > uslTrunkHwIdMax))
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Invalid tid %d for appId %d\n",
                  tid, ((usl_trunk_db_elem_t *)elemInfo.dbElem)->appId);
      continue;    
    }

    
    /* Mark this index as used in TrunkHwList */    
    pUslTrunkHwIdList[tid].used = L7_TRUE;
       
  }

  return L7_SUCCESS;
}

void usl_trunk_hw_id_generator_dump()
{ 
  L7_uint32 idx, entryCount = 0;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Trunk Min Index %d Max Index %d Used Hw Indexes: ",
                uslTrunkHwIdMin, uslTrunkHwIdMax);
  for (idx = uslTrunkHwIdMin; idx <= uslTrunkHwIdMax; idx++)
  {
    if (pUslTrunkHwIdList[idx].used == L7_TRUE)
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
* @purpose  Allocate a hw trunk-id for given appId
*
* @param    appId      @{(input)}  Application identifier for Trunk
* @param    tid        @{(output)} Hw trunk-id
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*           BCM_E_UNAVAIL: Id generation feature is not available
*
* @end
*********************************************************************/
int usl_trunk_hw_id_allocate(L7_uint32 appId, bcm_trunk_t *tid)
{
  int                 rv = BCM_E_NONE;
  usl_trunk_db_elem_t searchElem, trunkElem;
  //L7_int32            idx;
  uslDbElemInfo_t     searchInfo, elemInfo;

//  *tid = BCM_TRUNK_INVALID; /* PTin removed: trunks */

  memset(&searchElem, 0, sizeof(searchElem));
  memset(&trunkElem, 0, sizeof(trunkElem));

  searchElem.appId = appId;

  searchInfo.dbElem = &searchElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &trunkElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  do
  {
    /* If USL is in warm start state, then search the operational db for this appId.
    ** If appId is not found then an unused index is generated.
    */
    if (usl_state_get() == USL_WARM_START_STATE)
    {
      rv = usl_get_trunk_db_elem(USL_OPERATIONAL_DB, USL_DB_EXACT_ELEM, 
                                 searchInfo, &elemInfo);
      if (rv == BCM_E_NONE)
      {
        
        *tid = ((usl_trunk_db_elem_t *) elemInfo.dbElem)->tid;

        if (*tid == BCM_TRUNK_INVALID)
        {
          LOG_ERROR(*tid);    
        }


        /* Mark this index as used in TrunkHwList */    
        pUslTrunkHwIdList[*tid].used = L7_TRUE;
        break;
      }
    }

    /* PTin added: trunks */
    /* Try to use the suggested tid */
    if ((*tid >= uslTrunkHwIdMin) && (*tid <= uslTrunkHwIdMax))
    {
      if (pUslTrunkHwIdList[*tid].used == L7_FALSE)
      {
        pUslTrunkHwIdList[*tid].used = L7_TRUE;    
        //  *tid = idx;
        LOG_PT_DEBUG(LOG_CTX_TRUNKS, "Suggested Trunk ID# %d is free", (int)*tid);
        rv = BCM_E_NONE;
        break;
      }
      LOG_PT_CRITIC(LOG_CTX_TRUNKS, "Suggested Trunk ID# %d is NOT FREE!", (int)*tid);
    }
    else
    {
      *tid = BCM_TRUNK_INVALID;
      LOG_PT_ERR(LOG_CTX_TRUNKS, "Invalid trunk id");
    }
    /* PTin end */
    
    /* PTin removed: LAGs */
    #if 0
    for (idx = uslTrunkHwIdMin; idx <= uslTrunkHwIdMax; idx++)
    {
      /* Found an unused index */
      if (pUslTrunkHwIdList[idx].used == L7_FALSE)
      {
        pUslTrunkHwIdList[idx].used = L7_TRUE;    
        *tid = idx;
        LOG_PT_DEBUG(LOG_CTX_TRUNKS, "Allocating a new Trunk ID# %d", (int)*tid);
        rv = BCM_E_NONE;
        break;
      }
    }
    #endif
  } while(0);


  if (*tid == BCM_TRUNK_INVALID)
  {
    rv = BCM_E_FULL;  
  }

  return rv;
}

/*********************************************************************
* @purpose  Free a given trunkId 
*
* @param    tid   @{(input)} Hw trunk-id
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_trunk_hw_id_free(bcm_trunk_t tid)
{
  int rv = BCM_E_NONE;

  if ((tid < uslTrunkHwIdMin) || (tid > uslTrunkHwIdMax))
  {
    rv = BCM_E_FAIL;
    return rv;
  }
  else
  {
    pUslTrunkHwIdList[tid].used = L7_FALSE;
  }

  return rv;
}

#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Search trunk db to determine if tgid is valid.
*
* @params   dbType     {(input)} Db type 
*           tid       {(input)}  BCM tid
*
* @returns  BCM_E_NONE:      tgid valid
*           BCM_E_NOT_FOUND: tgid not valid
*
* @notes    
*
* @end
*********************************************************************/
L7_int32 usl_trunk_db_tgid_validate(USL_DB_TYPE_t dbType, bcm_trunk_t tid)
{
  usl_trunk_db_elem_t  *dbHandle = L7_NULLPTR;
  L7_int32              rv = BCM_E_NONE;
  L7_uint32             idx;

  USL_TRUNK_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_trunk_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }
    for (idx = 0; idx < uslTrunkDbMaxIdx; idx++)
    {
      if ((dbHandle[idx].isValid == L7_TRUE) &&
          (dbHandle[idx].tid == tid))
      {
        rv = BCM_E_NONE;
        break;
      }  
    }
  
    if (idx >= uslTrunkDbMaxIdx) 
    {
      rv = BCM_E_NOT_FOUND;
    }
  } while (0);

  USL_TRUNK_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Used to clean up the HW trunk tables of any references to 
*           modids of the old manager unit. This function is called
*           by each stack unit after a failover occurs.
*
* @param    *missingModIds     @{(input)} array of missing modIds
*            missingModIdCount @{(input)} count of missing modIds
*
* @returns  BCM_E_NONE
*           BCM_E_FAIL
*
* @end
*********************************************************************/
int usl_trunk_db_dataplane_cleanup(L7_int32 *missingModIds, L7_uint32 missingModIdCount)
{
  int                  rv = BCM_E_NONE;
  int                  tmprv;
  L7_uint32            trunkIdx, memberIdx, modIdIdx;
  usl_trunk_db_elem_t *dbHandle = L7_NULLPTR;
  bcm_trunk_add_info_t addInfo;
  L7_BOOL              trunkModified, memberOkay;
  L7_uchar8            flush_msg[USL_MSG_SIZE_HDR + 4];
  L7_uint32            startTime, endTime;

  /* Iterate through all the of the trunks. */
  USL_TRUNK_DB_LOCK_TAKE();

  dbHandle = usl_trunk_db_handle_get(USL_OPERATIONAL_DB);
  if (dbHandle == L7_NULLPTR)
  {
    USL_TRUNK_DB_LOCK_GIVE();
    return BCM_E_FAIL;
  }

  startTime = osapiTimeMillisecondsGet();

  for (trunkIdx = 0; trunkIdx < uslTrunkDbMaxIdx; trunkIdx++)
  {
    if (dbHandle[trunkIdx].isValid)
    {
      memcpy(&addInfo, &dbHandle[trunkIdx].add_info, sizeof(bcm_trunk_add_info_t));
      addInfo.num_ports = 0; /* Clear the members and build back up based on valid modids. */
/*
      memset(addInfo.member_flags, 0, sizeof(addInfo.member_flags));
*/
      memset(addInfo.tp,           0, sizeof(addInfo.tp));
      memset(addInfo.tm,           0, sizeof(addInfo.tm));
      trunkModified = L7_FALSE;

      /* Iterate through all the of the trunk members. */
      for (memberIdx = 0; memberIdx < dbHandle[trunkIdx].add_info.num_ports; memberIdx++)
      {
        memberOkay = L7_TRUE;
        for (modIdIdx = 0; modIdIdx < missingModIdCount; modIdIdx++)
        {
          if (missingModIds[modIdIdx] == dbHandle[trunkIdx].add_info.tm[memberIdx])
          {
            /* Remove this trunk member. */
            memberOkay    = L7_FALSE;
            trunkModified = L7_TRUE;
          }
        }

        if (memberOkay == L7_TRUE)
        {
/*
          memcpy(&addInfo.member_flags[addInfo.num_ports], 
                 &dbHandle[trunkIdx].add_info.member_flags[memberIdx], 
                 sizeof(addInfo.member_flags[0]));
*/
          memcpy(&addInfo.tp[addInfo.num_ports], 
                 &dbHandle[trunkIdx].add_info.tp[memberIdx], 
                 sizeof(addInfo.tp[0]));
          memcpy(&addInfo.tm[addInfo.num_ports], 
                 &dbHandle[trunkIdx].add_info.tm[memberIdx], 
                 sizeof(addInfo.tm[0]));
          addInfo.num_ports++;
        }
      }

      if (trunkModified == L7_TRUE)
      {
        if (addInfo.num_ports == 0)
        {
          tmprv = usl_bcm_trunk_destroy(dbHandle[trunkIdx].appId, dbHandle[trunkIdx].tid);
          if (tmprv < rv)
          {
            USL_LOG_MSG(USL_BCM_E_LOG,
                        "Trunk %d destroy failed, rv %d\n", 
                        dbHandle[trunkIdx].tid, 
                        rv);    
            rv = tmprv;
          }

          /* Flush any L2 entries pointing to this trunk. */
          /* sending to the local unit */
          *(L7_uint32 *) &flush_msg[USL_MSG_TYPE_OFFSET] = USL_MAC_TABLE_TRUNK_FLUSH;
          *(L7_uint32 *) &flush_msg[USL_MSG_DATA_OFFSET] = dbHandle[trunkIdx].tid;

          usl_macsync_msg_receiver_callback(flush_msg, sizeof(flush_msg));
        }
        else
        {
          /* Make sure DLF, MC, and IPMC indices are w/in proper bounds. */
          if (addInfo.dlf_index >= addInfo.num_ports)
          {
            addInfo.dlf_index = 0;
          }
          if (addInfo.mc_index >= addInfo.num_ports)
          {
            addInfo.mc_index = 0;
          }
          if (addInfo.ipmc_index >= addInfo.num_ports)
          {
            addInfo.ipmc_index = 0;
          }
          tmprv = usl_bcm_trunk_set(dbHandle[trunkIdx].appId, dbHandle[trunkIdx].tid, &addInfo);
          if (tmprv < rv)
          {
            USL_LOG_MSG(USL_BCM_E_LOG,
                        "Trunk %d set failed, rv %d\n", 
                        dbHandle[trunkIdx].tid, 
                        rv);    
            rv = tmprv;
          }
        }
      }
    }
  }

  endTime = osapiTimeMillisecondsGet();
  uslTrunkDataplaneCleanupTime += osapiTimeMillisecondsDiff(endTime,startTime);
  uslTrunkDataplaneCleanupCount++;

  USL_TRUNK_DB_LOCK_GIVE();

  return rv;
}

void usl_trunk_db_dataplane_cleanup_debug_show()
{
  sysapiPrintf("uslTrunkDataplaneCleanupTime (ms) == %d\n", uslTrunkDataplaneCleanupTime);
  sysapiPrintf("uslTrunkDataplaneCleanupCount     == %d\n", uslTrunkDataplaneCleanupCount);
  sysapiPrintf("average                      (ms) == %d\n", uslTrunkDataplaneCleanupTime / uslTrunkDataplaneCleanupCount);
}

void usl_trunk_db_dataplane_cleanup_debug_clear()
{
  uslTrunkDataplaneCleanupTime  = 0;
  uslTrunkDataplaneCleanupCount = 0;
}
#endif

