/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_ipmcast_db.c
*
* @purpose    IP Multicast Db implementation
*
* @component  HAPI
*
* @comments   none
*
* @create     12/2/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include "l7_common.h"

#ifdef L7_MCAST_PACKAGE

#include "broad_common.h"
#include "osapi.h"
#include "l7_usl_bcm.h"
#include "avl_api.h"
#include "platform_config.h"
#include "l7_usl_sm.h"
#include "l7_usl_ipmcast_db.h"
#include "l7_usl_port_db.h"
#include "l7_usl_ipmcast_db_int.h"
#include "sysapi.h"
#include "l7_usl_trace.h"
#include "bcmx/bcmx_int.h"
#include "unitmgr_api.h"
#include "zlib.h"

#ifdef L7_WIRELESS_PACKAGE
#include "wireless_exports.h"
#endif

#define USL_DEBUG_PRINT if (usl_debug != L7_FALSE) sysapiPrintf
static L7_BOOL usl_debug = L7_FALSE;

#define L7_USL_IPMC_KEY_SIZE       (sizeof(bcm_ip6_t) + sizeof(bcm_ip6_t) + sizeof(bcm_vlan_t))
#define L7_USL_IPMC_PORT_KEY_SIZE  (sizeof(L7_ushort16) + sizeof(L7_ushort16))


/* IPMcast Table resources (semaphore and avl tree) */
void                     *pUslIpMcastDbSema          = L7_NULLPTR;
avlTreeTables_t          *pUslOperIpMcastTreeHeap    = L7_NULLPTR;
usl_ipmc_db_elem_t       *pUslOperIpMcastDataHeap    = L7_NULLPTR;
usl_ipmc_db_elem_t      **pUslOperIpMcastDataByIndex;
avlTree_t                 uslOperIpMcastTreeData     = { 0};

avlTreeTables_t          *pUslShadowIpMcastTreeHeap    = L7_NULLPTR;
usl_ipmc_db_elem_t       *pUslShadowIpMcastDataHeap    = L7_NULLPTR;
usl_ipmc_db_elem_t      **pUslShadowIpMcastDataByIndex;
avlTree_t                 uslShadowIpMcastTreeData     = { 0};

avlTreeTables_t          *pUslOperIpMcastPortTreeHeap = L7_NULLPTR;
usl_ipmc_port_db_elem_t  *pUslOperIpMcastPortDataHeap = L7_NULLPTR;
avlTree_t                 uslOperIpMcastPortTreeData  = { 0};

avlTreeTables_t          *pUslShadowIpMcastPortTreeHeap = L7_NULLPTR;
usl_ipmc_port_db_elem_t  *pUslShadowIpMcastPortDataHeap = L7_NULLPTR;
avlTree_t                 uslShadowIpMcastPortTreeData  = { 0};

avlTree_t                *pUslIpMcastTreeHandle     = L7_NULLPTR;
avlTree_t                *pUslIpMcastPortTreeHandle = L7_NULLPTR;
usl_ipmc_db_elem_t      **pUslIpMcastDataByIndex;

avlTreeTables_t           *pUslIpMcastWlanPortTreeHeap = L7_NULLPTR;
usl_ipmc_wlan_port_elem_t *pUslIpMcastWlanPortDataHeap = L7_NULLPTR;
avlTree_t                  uslIpMcastWlanPortTreeData  = { 0};

L7_BOOL                   uslIpMcDbActive = L7_FALSE;

/* Hw Id generation data-structures */
usl_ipmc_hw_id_list_t    *pUslIpmcHwIdList = L7_NULLPTR;
L7_int32                  uslIpmcHwIdMin = 0, uslIpmcHwIdMax = 0;

static L7_BOOL                  uslIpmcInited                = L7_FALSE;
static L7_BOOL                  uslIpmcRpfModeEnable         = L7_TRUE;    /* Indicates whether IPMC entries should enable RPF checks. */

#ifdef L7_STACKING_PACKAGE
/* TIMER to be used for inuse check */
static osapiTimerDescr_t *pUslIpmcInUseTimer = L7_NULLPTR;
#endif

/* The time in milliseconds to wait between checking the in use of the IPMC entries */
#define USL_IPMC_INUSE_TIMER_INTERVAL_SEC   (20)
#define USL_IPMC_INUSE_TIMER_INTERVAL_MSEC  (USL_IPMC_INUSE_TIMER_INTERVAL_SEC*1000)


void usl_ipmc_inuse_timer(L7_uint32 arg1, L7_uint32 arg2);

void usl_db_ipmc_debug_enable(L7_BOOL enable)
{
  usl_debug = enable;
}

void usl_db_ipmc_key_print(usl_ipmc_db_key_t *keyPtr)
{
  L7_uint32 i, ipAddr[4];

  if (usl_debug == L7_TRUE)
  {
    for (i = 0; i < 4; i++)
    {
      memcpy(&ipAddr[i], &(keyPtr->s_ip_addr[i * 4]), sizeof(L7_uint32));
    }
    USL_DEBUG_PRINT("   src addr: %8.8x:%8.8x:%8.8x:%8.8x\n", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    for (i = 0; i < 4; i++)
    {
      memcpy(&ipAddr[i], &(keyPtr->mc_ip_addr[i * 4]), sizeof(L7_uint32));
    }
    USL_DEBUG_PRINT("   group addr: %8.8x:%8.8x:%8.8x:%8.8x\n", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    USL_DEBUG_PRINT("   vid: %d\n", keyPtr->vid);
  }
}

/*********************************************************************
*
* @purpose Gets the IPMC pointer to the IPMC port DB entry for a port.
*          If the entry doesn't exist, it is allocated.
*
* @param   modid      {(input)} modid of L3 port
* @param   bcmPort    {(input)} BCM port
*
* @returns pointer to IPMC port DB entry
*
* @notes
*
* @end
*
*********************************************************************/
static usl_ipmc_port_db_elem_t  *usl_db_ipmc_egress_port_ptr_get(L7_uint32 modid, L7_uint32 bcmPort)
{
  usl_ipmc_port_db_elem_t   portKey;
  usl_ipmc_port_db_elem_t  *pPortData;

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    portKey.bcmModid = modid;
    portKey.bcmPort  = bcmPort;
    pPortData = avlInsertEntry(pUslIpMcastPortTreeHandle, &portKey);
    if (pPortData == L7_NULL)
    {
      /* Entry was added */
      USL_DEBUG_PRINT("Added IPMC Port DB entry, modid %d, port %d\n", modid, bcmPort);

      pPortData = avlSearchLVL7(pUslIpMcastPortTreeHandle, &portKey, AVL_EXACT);
      if (pPortData == L7_NULL)
      {
        /* This shouldn't happen since the entry was just added. */
        LOG_ERROR(0);
      }

      pPortData->replInfo      = L7_NULL;
      pPortData->vlanId        = 0;
      pPortData->untagFlag     = 0;
      memset(pPortData->ipmcIndexMask, 0, sizeof(pPortData->ipmcIndexMask));
    }
    else if (pPortData == &portKey)
    {
      /* Error, couldn't add entry */
      pPortData = L7_NULL;
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add entry to IPMC port DB");
      break;
    }

  } while (0);

  USL_IPMC_DB_LOCK_GIVE();

  return pPortData;
}

/*********************************************************************
*
* @purpose Determines if a VLAN vector is empty.
*
* @param   vlanVector {(input)} VLAN replication info for L3 port.
*
*
* @returns BCM Error Code
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL usl_db_ipmc_vlan_vector_is_empty(bcm_vlan_vector_t vlanVector)
{
  L7_uint32 i;
  L7_BOOL   vlanVectorIsEmpty = L7_TRUE;

  for (i = 0; i < (sizeof(bcm_vlan_vector_t) / sizeof(uint32)); i++)
  {
    if (vlanVector[i] != 0)
    {
      vlanVectorIsEmpty = L7_FALSE;
      break;
    }
  }

  return vlanVectorIsEmpty;
}

/*********************************************************************
*
* @purpose Sets the VLAN replication configuration for a port in an
*          IPMC entry.
*
* @param   modid      {(input)} modid of L3 port
* @param   bcmPort    {(input)} BCM port
* @param   ipmcEntry  {(input)} IPMC entry
* @param   vlanVector {(input)} VLAN replication info for L3 port.
*
* @returns BCM Error Code
*
* @notes
*
* @end
*
*********************************************************************/
int usl_db_ipmc_repl_set(L7_uint32 modid, L7_uint32 bcmPort, L7_uint32 ipmcIndex, bcm_vlan_vector_t vlanVector)
{
  int                       rv = BCM_E_NONE;
  usl_ipmc_port_db_elem_t  *pPortData;
  bcm_vlan_vector_t         tempVlanVector;
  L7_ushort16               vlanId;
  L7_uint32                 wordIdx, bitIdx;
  usl_ipmc_port_db_repl_t  *replInfo, *replInfoPrev, *replInfoNext;

  USL_IPMC_DB_LOCK_TAKE();

  USL_DEBUG_PRINT("Storing replication info for modid %d, port %d, ipmcIndex %d\n", modid, bcmPort, ipmcIndex);

  do
  {
    pPortData = usl_db_ipmc_egress_port_ptr_get(modid, bcmPort);
    if (pPortData == L7_NULL)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Update existing entries first. */
    memcpy(tempVlanVector, vlanVector, sizeof(bcm_vlan_vector_t));
    replInfo = pPortData->replInfo;
    replInfoPrev = L7_NULLPTR;
    while (replInfo != L7_NULLPTR)
    {
      replInfoNext = replInfo->next;

      if (BCM_VLAN_VEC_GET(tempVlanVector, replInfo->vlanId))
      {
        BCM_VLAN_VEC_CLR(tempVlanVector, replInfo->vlanId);
        if (!USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, replInfo->ipmcIndexMask))
        {
          USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET(ipmcIndex, replInfo->ipmcIndexMask);
          replInfo->ipmcEntryCount++;
        }
      }
      else
      {
        if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, replInfo->ipmcIndexMask))
        {
          USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(ipmcIndex, replInfo->ipmcIndexMask);
          replInfo->ipmcEntryCount--;

          /* No need to store an empty entry. */
          if (replInfo->ipmcEntryCount == 0)
          {
            if (replInfoPrev == L7_NULLPTR)
            {
              /* Deleting first node */
              pPortData->replInfo = replInfoNext;
            }
            else
            {
              /* Deleting node other than first. */
              replInfoPrev->next = replInfoNext;
            }
            osapiFree(L7_DRIVER_COMPONENT_ID, replInfo);
            replInfo = L7_NULLPTR;
          }
        }
      }

      if (replInfo != L7_NULLPTR)
      {
        replInfoPrev = replInfo;
      }
      replInfo = replInfoNext;
    }

    /* Add the IPMC index for new VLANs. */
    for (wordIdx = 0; wordIdx < (sizeof(bcm_vlan_vector_t) / sizeof(uint32)); wordIdx++)
    {
      if (tempVlanVector[wordIdx] != 0)
      {
        for (bitIdx = 0; bitIdx < 32; bitIdx++)
        {
          vlanId = wordIdx * 32 + bitIdx;

          if (BCM_VLAN_VEC_GET(tempVlanVector, vlanId))
          {
            BCM_VLAN_VEC_CLR(tempVlanVector, vlanId);

            replInfo = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(usl_ipmc_port_db_repl_t));
            if (replInfo != L7_NULLPTR)
            {
              replInfo->next = pPortData->replInfo;
              pPortData->replInfo = replInfo;

              memset(replInfo->ipmcIndexMask, 0, sizeof(replInfo->ipmcIndexMask));
              USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET(ipmcIndex, replInfo->ipmcIndexMask);
              replInfo->vlanId = vlanId;
              replInfo->ipmcEntryCount = 1;
            }
            else
            {
              rv = BCM_E_MEMORY;
              break;
            }
          }

          if (tempVlanVector[wordIdx] == 0)
          {
            break;
          }
        }

        if (rv != BCM_E_NONE)
        {
          break;
        }
      }
    }

  } while (0);

  USL_DEBUG_PRINT("Stored replication info, rv == %d\n", rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Gets the VLAN replication configuration for a port in an
*          IPMC entry.
*
* @param   modid      {(input)}  modid of L3 port
* @param   bcmPort    {(input)}  BCM port
* @param   ipmcEntry  {(input)}  IPMC entry
* @param   vlanVector {(output)} VLAN replication info for L3 port.
*
*
* @returns BCM Error Code
*
* @notes
*
* @end
*
*********************************************************************/
int usl_db_ipmc_repl_get(L7_uint32 modid, L7_uint32 bcmPort, L7_uint32 ipmcIndex, bcm_vlan_vector_t vlanVector)
{
  int                       rv = BCM_E_NONE;
  usl_ipmc_port_db_elem_t   portKey;
  usl_ipmc_port_db_elem_t  *pPortData;
  usl_ipmc_port_db_repl_t  *replInfo;

  if (!uslIpMcDbActive)
  {
    return BCM_E_FAIL;
  }

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    memset(vlanVector, 0, sizeof(bcm_vlan_vector_t));

    portKey.bcmModid = modid;
    portKey.bcmPort  = bcmPort;
    pPortData = avlSearchLVL7(pUslIpMcastPortTreeHandle, &portKey, AVL_EXACT);
    if (pPortData != L7_NULL)
    {
      replInfo = pPortData->replInfo;

      while (replInfo != L7_NULLPTR)
      {
        if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, replInfo->ipmcIndexMask))
        {
          BCM_VLAN_VEC_SET(vlanVector, replInfo->vlanId);
        }

        replInfo = replInfo->next;
      }
    }

  } while (0);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType            {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t *usl_ipmc_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperIpMcastTreeData;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowIpMcastTreeData;
      break;

    case USL_CURRENT_DB:
      dbHandle = pUslIpMcastTreeHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Set the IPMC db handle to Operational or Shadow table
*
* @param    dbType  {(input)} Type of db to set
*
* @returns  L7_SUCCESS - if all the ports were deleted
* @returns  L7_FAILURE - An error was encountered during deletion
*
*
* @end
*********************************************************************/
L7_RC_t usl_ipmc_db_handle_set(USL_DB_TYPE_t dbType)
{
  L7_RC_t rc = L7_SUCCESS;

  /* If trying to set the dbHandle to Shadow tables when they are not allocated */
  if ((dbType == USL_SHADOW_DB) &&
      (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_FALSE))
  {
    rc = L7_FAILURE;
    return rc;
  }

  USL_IPMC_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslIpMcastTreeHandle  = &uslOperIpMcastTreeData;
    pUslIpMcastDataByIndex = pUslOperIpMcastDataByIndex;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslIpMcastTreeHandle  = &uslShadowIpMcastTreeData;
    pUslIpMcastDataByIndex = pUslShadowIpMcastDataByIndex;
  }

  USL_DEBUG_PRINT("Set IPMC DB handle to %s\n", (dbType == USL_OPERATIONAL_DB) ? "OPER" : "SHADOW");

  USL_IPMC_DB_LOCK_GIVE();

  return rc;

}

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType            {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t *usl_ipmc_port_db_handle_get(USL_DB_TYPE_t dbType)
{
  avlTree_t *dbHandle = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      dbHandle = &uslOperIpMcastPortTreeData;
      break;

    case USL_SHADOW_DB:
      dbHandle = &uslShadowIpMcastPortTreeData;
      break;

    case USL_CURRENT_DB:
      dbHandle = pUslIpMcastPortTreeHandle;
      break;

    default:
      break;
  }

  return dbHandle;
}

/*********************************************************************
* @purpose  Set the IPMC db handle to Operational or Shadow table
*
* @param    dbType  {(input)} Type of db to set
*
* @returns  L7_SUCCESS - if all the ports were deleted
* @returns  L7_FAILURE - An error was encountered during deletion
*
*
* @end
*********************************************************************/
L7_RC_t usl_ipmc_port_db_handle_set(USL_DB_TYPE_t dbType)
{
  L7_RC_t rc = L7_SUCCESS;

  /* If trying to set the dbHandle to Shadow tables when they are not allocated */
  if ((dbType == USL_SHADOW_DB) &&
      (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_FALSE))
  {
    rc = L7_FAILURE;
    return rc;
  }

  USL_IPMC_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslIpMcastPortTreeHandle = &uslOperIpMcastPortTreeData;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslIpMcastPortTreeHandle = &uslShadowIpMcastPortTreeData;
  }

  USL_DEBUG_PRINT("Set IPMC Port DB handle to %s\n", (dbType == USL_OPERATIONAL_DB) ? "OPER" : "SHADOW");

  USL_IPMC_DB_LOCK_GIVE();

  return rc;

}

/*********************************************************************
* @purpose  Deallocate the IPMC tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized
*
* @notes    If the system has been operational for a while, the caller should
*           have called the usl_invalidate_ipmc_db before calling this
*           in order to insure that the lplists have been properly freed
*
* @end
*********************************************************************/
L7_RC_t usl_ipmc_db_fini()
{
  L7_RC_t rc = L7_ERROR;
  L7_RC_t max;

  do
  {
    uslIpmcInited = L7_FALSE;

    if ( pUslIpMcastDbSema != L7_NULLPTR )
    {
      osapiSemaDelete(pUslIpMcastDbSema);
      pUslIpMcastDbSema = L7_NULLPTR;
    }

    if (uslIpMcDbActive != L7_TRUE)
    {
      rc = L7_SUCCESS;
      break;
    }

    /*
     * MCAST table resources
     */
    max = platMrtrRoutesMaxEntriesGet();


    avlDeleteAvlTree(&uslOperIpMcastTreeData);
    avlDeleteAvlTree(&uslOperIpMcastPortTreeData);

    /* reset the tree data */
    memset((void*)&uslOperIpMcastTreeData,0,sizeof(avlTree_t));
    memset((void*)&uslOperIpMcastPortTreeData,0,sizeof(avlTree_t));

    if ( pUslOperIpMcastTreeHeap != L7_NULLPTR )
    {
      /* get rid of artifacts, free memory, and invalidate the pointer */
      memset((void*)pUslOperIpMcastTreeHeap,0,sizeof(avlTreeTables_t)  * max);
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpMcastTreeHeap);
      pUslOperIpMcastTreeHeap = L7_NULLPTR;
    }

    if ( pUslOperIpMcastDataHeap != L7_NULLPTR )
    {
      /* get rid of artifacts, free memory, and invalidate the pointer */
      memset((void*)pUslOperIpMcastDataHeap,0,sizeof(usl_ipmc_db_elem_t) * max);
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpMcastDataHeap);
      pUslOperIpMcastDataHeap = L7_NULLPTR;
    }

    if ( pUslOperIpMcastDataByIndex != L7_NULLPTR )
    {
      /* get rid of artifacts, free memory, and invalidate the pointer */
      memset((void*)pUslOperIpMcastDataByIndex,0,sizeof(usl_ipmc_db_elem_t *) * max);
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpMcastDataByIndex);
      pUslOperIpMcastDataByIndex = L7_NULLPTR;
    }

    if ( pUslOperIpMcastPortTreeHeap != L7_NULLPTR )
    {
      /* get rid of artifacts, free memory, and invalidate the pointer */
      memset((void*)pUslOperIpMcastPortTreeHeap,0,sizeof(avlTreeTables_t) * USL_PORT_DB_TREE_SIZE);
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpMcastPortTreeHeap);
      pUslOperIpMcastPortTreeHeap = L7_NULLPTR;
    }

    if ( pUslOperIpMcastPortDataHeap != L7_NULLPTR )
    {
      /* get rid of artifacts, free memory, and invalidate the pointer */
      memset((void*)pUslOperIpMcastPortDataHeap,0,sizeof(usl_ipmc_port_db_elem_t) * USL_PORT_DB_TREE_SIZE);
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperIpMcastPortDataHeap);
      pUslOperIpMcastPortDataHeap = L7_NULLPTR;
    }

    if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
    {
      avlDeleteAvlTree(&uslShadowIpMcastTreeData);
      avlDeleteAvlTree(&uslShadowIpMcastPortTreeData);

      /* reset the tree data */
      memset((void*)&uslShadowIpMcastTreeData,     0, sizeof(avlTree_t));
      memset((void*)&uslShadowIpMcastPortTreeData, 0, sizeof(avlTree_t));


      if ( pUslShadowIpMcastTreeHeap != L7_NULLPTR )
      {
        /* get rid of artifacts, free memory, and invalidate the pointer */
        memset((void*)pUslShadowIpMcastTreeHeap, 0, sizeof(avlTreeTables_t) * max);
        osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpMcastTreeHeap);
        pUslShadowIpMcastTreeHeap = L7_NULLPTR;
      }

      if ( pUslShadowIpMcastDataHeap != L7_NULLPTR )
      {
        /* get rid of artifacts, free memory, and invalidate the pointer */
        memset((void*)pUslShadowIpMcastDataHeap, 0, sizeof(usl_ipmc_db_elem_t) * max);
        osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpMcastDataHeap);
        pUslShadowIpMcastDataHeap = L7_NULLPTR;
      }

      if ( pUslShadowIpMcastDataByIndex != L7_NULLPTR )
      {
        /* get rid of artifacts, free memory, and invalidate the pointer */
        memset((void*)pUslShadowIpMcastDataByIndex, 0, sizeof(usl_ipmc_db_elem_t *) * max);
        osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpMcastDataByIndex);
        pUslShadowIpMcastDataByIndex = L7_NULLPTR;
      }

      if ( pUslShadowIpMcastPortTreeHeap != L7_NULLPTR )
      {
        /* get rid of artifacts, free memory, and invalidate the pointer */
        memset((void*)pUslShadowIpMcastPortTreeHeap, 0, sizeof(avlTreeTables_t) * USL_PORT_DB_TREE_SIZE);
        osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpMcastPortTreeHeap);
        pUslShadowIpMcastPortTreeHeap = L7_NULLPTR;
      }

      if ( pUslShadowIpMcastPortDataHeap != L7_NULLPTR )
      {
        /* get rid of artifacts, free memory, and invalidate the pointer */
        memset((void*)pUslShadowIpMcastPortDataHeap, 0, sizeof(usl_ipmc_port_db_elem_t) * USL_PORT_DB_TREE_SIZE);
        osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowIpMcastPortDataHeap);
        pUslShadowIpMcastPortDataHeap = L7_NULLPTR;
      }
    }


    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Initialize the IPMC tables, semaphores, ...
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
L7_RC_t usl_ipmc_db_init()
{
  L7_RC_t          rc = L7_ERROR;
  L7_uint32        max;
  uslDbSyncFuncs_t ipmcDbFuncs;
  uslDbSyncFuncs_t ipmcPortDbFuncs;
  L7_uint32        bcm_unit;
  L7_int32         rv;

  do
  {

    /* Enable bcmSwitchIpmcReplicationSharing parameter in order to enable
       the bcm_ipmc_repl_set() API. */
    for (bcm_unit = 0; bcm_unit < soc_ndev; bcm_unit++)
    {
      rv = bcm_switch_control_set(bcm_unit, bcmSwitchIpmcReplicationSharing, 1);
      if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
                "Failed to enable IPMC replication sharing mode, error code %d\n",
                rv);
      }
    }

    /*
     * MCAST table resources
     */
    max = platMrtrRoutesMaxEntriesGet();

    if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
      pUslIpMcastDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
      if ( pUslIpMcastDbSema == L7_NULLPTR ) break;

      pUslOperIpMcastTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(avlTreeTables_t) * max);
      if ( pUslOperIpMcastTreeHeap == L7_NULLPTR )
        break;

      pUslOperIpMcastDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(usl_ipmc_db_elem_t) * max);
      if ( pUslOperIpMcastDataHeap == L7_NULLPTR )
        break;

      pUslOperIpMcastDataByIndex = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(usl_ipmc_db_elem_t *) * max);
      if ( pUslOperIpMcastDataByIndex == L7_NULLPTR )
        break;

      pUslOperIpMcastPortTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(avlTreeTables_t) * USL_PORT_DB_TREE_SIZE);
      if ( pUslOperIpMcastPortTreeHeap == L7_NULLPTR )
        break;

      pUslOperIpMcastPortDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(usl_ipmc_port_db_elem_t) * USL_PORT_DB_TREE_SIZE);
      if ( pUslOperIpMcastPortDataHeap == L7_NULLPTR )
        break;

      /* clear the memory before creating the tree */
      memset((void*)&uslOperIpMcastTreeData, 0, sizeof(avlTree_t));
      memset((void*)pUslOperIpMcastTreeHeap, 0, sizeof(avlTreeTables_t) * max);
      memset((void*)pUslOperIpMcastDataHeap, 0, sizeof(usl_ipmc_db_elem_t) * max);

      avlCreateAvlTree(&uslOperIpMcastTreeData, pUslOperIpMcastTreeHeap, pUslOperIpMcastDataHeap,
                       max, sizeof(usl_ipmc_db_elem_t), 0x10, L7_USL_IPMC_KEY_SIZE);

      memset((void*)&uslOperIpMcastPortTreeData, 0, sizeof(avlTree_t));
      memset((void*)pUslOperIpMcastPortTreeHeap, 0, sizeof(avlTreeTables_t) * USL_PORT_DB_TREE_SIZE);
      memset((void*)pUslOperIpMcastPortDataHeap, 0, sizeof(usl_ipmc_port_db_elem_t) * USL_PORT_DB_TREE_SIZE);

      avlCreateAvlTree(&uslOperIpMcastPortTreeData, pUslOperIpMcastPortTreeHeap, pUslOperIpMcastPortDataHeap,
                       USL_PORT_DB_TREE_SIZE, sizeof(usl_ipmc_port_db_elem_t), 0, L7_USL_IPMC_PORT_KEY_SIZE);

      /* Allocate shadow tables for NSF feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
      {
        pUslShadowIpMcastTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(avlTreeTables_t) * max);
        if ( pUslShadowIpMcastTreeHeap == L7_NULLPTR )
          break;

        pUslShadowIpMcastDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(usl_ipmc_db_elem_t) * max);
        if ( pUslShadowIpMcastDataHeap == L7_NULLPTR )
          break;

        pUslShadowIpMcastDataByIndex = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(usl_ipmc_db_elem_t *) * max);
        if ( pUslShadowIpMcastDataByIndex == L7_NULLPTR )
          break;

        pUslShadowIpMcastPortTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(avlTreeTables_t) * USL_PORT_DB_TREE_SIZE);
        if ( pUslShadowIpMcastPortTreeHeap == L7_NULLPTR )
          break;

        pUslShadowIpMcastPortDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(usl_ipmc_port_db_elem_t) * USL_PORT_DB_TREE_SIZE);
        if ( pUslShadowIpMcastPortDataHeap == L7_NULLPTR )
          break;

        /* clear the memory before creating the tree */
        memset((void*)&uslShadowIpMcastTreeData, 0, sizeof(avlTree_t));
        memset((void*)pUslShadowIpMcastTreeHeap, 0, sizeof(avlTreeTables_t) * max);
        memset((void*)pUslShadowIpMcastDataHeap, 0, sizeof(usl_ipmc_db_elem_t) * max);

        avlCreateAvlTree(&uslShadowIpMcastTreeData, pUslShadowIpMcastTreeHeap, pUslShadowIpMcastDataHeap,
                       max, sizeof(usl_ipmc_db_elem_t), 0x10, L7_USL_IPMC_KEY_SIZE);

        memset((void*)&uslShadowIpMcastPortTreeData, 0, sizeof(avlTree_t));
        memset((void*)pUslShadowIpMcastPortTreeHeap, 0, sizeof(avlTreeTables_t) * USL_PORT_DB_TREE_SIZE);
        memset((void*)pUslShadowIpMcastPortDataHeap, 0, sizeof(usl_ipmc_port_db_elem_t) * USL_PORT_DB_TREE_SIZE);

        avlCreateAvlTree(&uslShadowIpMcastPortTreeData, pUslShadowIpMcastPortTreeHeap, pUslShadowIpMcastPortDataHeap,
                       USL_PORT_DB_TREE_SIZE, sizeof(usl_ipmc_port_db_elem_t), 0, L7_USL_IPMC_PORT_KEY_SIZE);

      }

      /* Register the reconciliation/synchronization routines */
      memset(&ipmcDbFuncs, 0, sizeof(ipmcDbFuncs));
      ipmcDbFuncs.get_size_of_db_elem = usl_get_size_of_ipmc_db_elem;
      ipmcDbFuncs.get_db_elem         = usl_get_ipmc_db_elem;
      ipmcDbFuncs.delete_elem_from_db = usl_delete_ipmc_db_elem;
      ipmcDbFuncs.print_db_elem       = usl_print_ipmc_db_elem;
      ipmcDbFuncs.create_usl_bcm      = usl_create_ipmc_db_elem_bcm;
      ipmcDbFuncs.delete_usl_bcm      = usl_delete_ipmc_db_elem_bcm;
      ipmcDbFuncs.update_usl_bcm      = usl_update_ipmc_db_elem_bcm;

      if (usl_db_sync_func_table_register(USL_IPMC_ROUTE_DB_ID,
                                          &ipmcDbFuncs) != L7_SUCCESS)
      {
        LOG_ERROR(0);
      }

      memset(&ipmcPortDbFuncs, 0, sizeof(ipmcPortDbFuncs));
      ipmcPortDbFuncs.get_size_of_db_elem = usl_get_size_of_ipmc_port_db_elem;
      ipmcPortDbFuncs.get_db_elem         = usl_get_ipmc_port_db_elem;
      ipmcPortDbFuncs.delete_elem_from_db = usl_delete_ipmc_port_db_elem;
      ipmcPortDbFuncs.print_db_elem       = usl_print_ipmc_port_db_elem;
      ipmcPortDbFuncs.create_usl_bcm      = usl_create_ipmc_port_db_elem_bcm;
      ipmcPortDbFuncs.delete_usl_bcm      = usl_delete_ipmc_port_db_elem_bcm;
      ipmcPortDbFuncs.update_usl_bcm      = usl_update_ipmc_port_db_elem_bcm;

      if (usl_db_sync_func_table_register(USL_IPMC_PORT_DB_ID,
                                          &ipmcPortDbFuncs) != L7_SUCCESS)
      {
        LOG_ERROR(0);
      }

      pUslIpMcastTreeHandle  = &uslOperIpMcastTreeData;
      pUslIpMcastPortTreeHandle = &uslOperIpMcastPortTreeData;
      pUslIpMcastDataByIndex = pUslOperIpMcastDataByIndex;
      uslIpMcDbActive = L7_TRUE;

#ifdef L7_WIRELESS_PACKAGE
      if (cnfgrIsFeaturePresent(L7_FLEX_WIRELESS_COMPONENT_ID, L7_WIRELESS_L2_CENTTNNL_FEATURE_ID) == L7_TRUE)
      {
        /* wlan ipmc port info */
        pUslIpMcastWlanPortTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                           sizeof(avlTreeTables_t) * USL_PORT_DB_TREE_SIZE);

        if (pUslIpMcastWlanPortTreeHeap == L7_NULLPTR) break;

        pUslIpMcastWlanPortDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                           sizeof(usl_ipmc_wlan_port_elem_t) * USL_PORT_DB_TREE_SIZE);

        if (pUslIpMcastWlanPortDataHeap == L7_NULLPTR) break;

        /* clear the memory before creating the tree */
        memset((void*)&uslIpMcastWlanPortTreeData,0,sizeof(avlTree_t));
        memset((void*)pUslIpMcastWlanPortTreeHeap,0,sizeof(avlTreeTables_t)  * USL_PORT_DB_TREE_SIZE);
        memset((void*)pUslIpMcastWlanPortDataHeap,0,sizeof(usl_ipmc_wlan_port_elem_t) * USL_PORT_DB_TREE_SIZE);

        avlCreateAvlTree(&uslIpMcastWlanPortTreeData, pUslIpMcastWlanPortTreeHeap, pUslIpMcastWlanPortDataHeap,
                         USL_PORT_DB_TREE_SIZE, sizeof(usl_ipmc_wlan_port_elem_t), 0x10,
                         sizeof(bcm_if_t)+sizeof(bcm_vlan_t)+sizeof(bcmx_lport_t));
      }
#endif
   }

   /* all is good if we get here */
   rc = L7_SUCCESS;

  } while ( 0 );

  /* if we fail, clean up all resources */
  if (rc != L7_SUCCESS)
    usl_ipmc_db_fini();
  else
  {
    uslIpmcInited = L7_TRUE;

#ifdef L7_STACKING_PACKAGE
    /* create the timer for the inuse check */
    osapiTimerAdd ( (void*)usl_ipmc_inuse_timer, USL_IPMC_INUSE_TIMER_POP, 0,
                    USL_IPMC_INUSE_TIMER_INTERVAL_MSEC, &pUslIpmcInUseTimer );
#endif
  }

  return rc;
}

/*********************************************************************
* @purpose  Clear all entries from the IPMC databases
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
L7_RC_t usl_ipmc_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t rc = L7_ERROR;
  L7_uint32 max;
  usl_ipmc_port_db_elem_t key, *pData;
  usl_ipmc_port_db_repl_t   *replInfo, *replInfoNext;

  if (uslIpMcDbActive != L7_TRUE)
    return L7_SUCCESS;

  USL_IPMC_DB_LOCK_TAKE();

  do
  {

    /*
     * MCAST table resources
     */
    max = platMrtrRoutesMaxEntriesGet();

    if (flags & USL_OPERATIONAL_DB)
    {
      USL_DEBUG_PRINT("Invalidating OPER IPMC DB\n");

      avlPurgeAvlTree(&uslOperIpMcastTreeData,max);

      memset(&key, 0, sizeof(key));
      pData = avlSearchLVL7(&uslOperIpMcastPortTreeData, &key, AVL_EXACT);
      if (pData == L7_NULLPTR)
      {
        pData = avlSearchLVL7(&uslOperIpMcastPortTreeData, &key, AVL_NEXT);
      }
      while (pData != L7_NULL)
      {
        memcpy(&key, pData, sizeof(key));

        replInfo = pData->replInfo;

        while (replInfo != L7_NULLPTR)
        {
          replInfoNext = replInfo->next;

          osapiFree(L7_DRIVER_COMPONENT_ID, replInfo);

          replInfo = replInfoNext;
        }
        pData->replInfo = L7_NULLPTR;

        pData = avlSearchLVL7(&uslOperIpMcastPortTreeData, &key, AVL_NEXT);
      }
      avlPurgeAvlTree(&uslOperIpMcastPortTreeData, USL_PORT_DB_TREE_SIZE);
    }


    if ((flags & USL_SHADOW_DB) &&
        (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE))
    {
      USL_DEBUG_PRINT("Invalidating SHADOW IPMC DB\n");

      avlPurgeAvlTree(&uslShadowIpMcastTreeData,max);

      memset(&key, 0, sizeof(key));
      pData = avlSearchLVL7(&uslShadowIpMcastPortTreeData, &key, AVL_EXACT);
      if (pData == L7_NULLPTR)
      {
        pData = avlSearchLVL7(&uslShadowIpMcastPortTreeData, &key, AVL_NEXT);
      }
      while (pData != L7_NULL)
      {
        memcpy(&key, pData, sizeof(key));

        replInfo = pData->replInfo;

        while (replInfo != L7_NULLPTR)
        {
          replInfoNext = replInfo->next;

          osapiFree(L7_DRIVER_COMPONENT_ID, replInfo);

          replInfo = replInfoNext;
        }
        pData->replInfo = L7_NULLPTR;

        pData = avlSearchLVL7(&uslShadowIpMcastPortTreeData, &key, AVL_NEXT);
      }
      avlPurgeAvlTree(&uslShadowIpMcastPortTreeData, USL_PORT_DB_TREE_SIZE);
    }

    rc = L7_SUCCESS;
  } while ( 0);


  USL_IPMC_DB_LOCK_GIVE();

  return rc;
}



/*********************************************************************
* @purpose  Add a IP Mcast entry to the USL Db
*
* @param    bcm_ipmc        {(input)} IP Mcast entry info
*           replace_entry   {(input)} Indicates if this replaces an existing entry.
*
* @returns BCM Error Code
*
* @end
*********************************************************************/
int usl_db_ipmc_add(usl_bcm_ipmc_addr_t *bcm_ipmc, L7_BOOL replace_entry)
{
  usl_ipmc_db_elem_t data, *pData;
  int                rv = BCM_E_NONE;
  L7_int32           max;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

  /* initialize to zero so the invalidate will work */
  memset((void *)&data, 0, sizeof(data));

  memcpy(&(data.bcm_data.s_ip_addr), &(bcm_ipmc->s_ip_addr), sizeof(data.bcm_data.s_ip_addr));
  memcpy(&(data.bcm_data.mc_ip_addr), &(bcm_ipmc->mc_ip_addr), sizeof(data.bcm_data.mc_ip_addr));
  memcpy(&(data.bcm_data.vid), &(bcm_ipmc->vid), sizeof(data.bcm_data.vid));

  data.isValid = L7_TRUE;
  data.lastHit = osapiUpTimeRaw();

  data.bcm_data.flags = bcm_ipmc->flags;
  data.bcm_data.cos   = bcm_ipmc->cos;

  data.bcm_data.ts         = bcm_ipmc->ts;
  data.bcm_data.port_tgid  = bcm_ipmc->port_tgid;
  data.bcm_data.mod_id     = bcm_ipmc->mod_id;
  data.bcm_data.ipmc_index = bcm_ipmc->ipmc_index;
  memcpy(data.bcm_data.l2_pbmp, bcm_ipmc->l2_pbmp, sizeof(data.bcm_data.l2_pbmp));
  memcpy(data.bcm_data.l2_ubmp, bcm_ipmc->l2_ubmp, sizeof(data.bcm_data.l2_ubmp));
  memcpy(data.bcm_data.l3_pbmp, bcm_ipmc->l3_pbmp, sizeof(data.bcm_data.l3_pbmp));

    /* get the info in our AVL tree */
  pData = avlInsertEntry(pUslIpMcastTreeHandle, &data );

  if (pData == &data)
  {
    rv = BCM_E_FAIL;
  }
  else if (pData)
  {
    pData->lastHit = osapiUpTimeRaw();
    if (replace_entry)
    {
      memcpy(&pData->bcm_data, bcm_ipmc, sizeof(pData->bcm_data));
      rv = BCM_E_NONE;
    }
    else
    {
      rv = BCM_E_EXISTS;
    }
  }
  else
  {
    pData = avlSearchLVL7(pUslIpMcastTreeHandle, &data, AVL_EXACT);
    max = platMrtrRoutesMaxEntriesGet();
    if (data.bcm_data.ipmc_index < max)
    {
      pUslIpMcastDataByIndex[data.bcm_data.ipmc_index] = pData;
    }
    else
    {
      LOG_ERROR(data.bcm_data.ipmc_index);
    }
  }

  USL_DEBUG_PRINT("Adding entry to IPMC DB, rv == %d\n", rv);
  usl_db_ipmc_key_print((usl_ipmc_db_key_t *)&data.bcm_data);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Remove a IPMC group entry
*
* @param    bcm_ipmc {(input)} IP Mcast entry info
*
* @returns BCM Error Code
*
* @end
*********************************************************************/
int usl_db_ipmc_remove(usl_bcm_ipmc_addr_t *bcm_ipmc)
{
  usl_ipmc_db_elem_t elem;
  uslDbElemInfo_t    elemInfo;

  elemInfo.dbElem    = &elem;
  elemInfo.elemIndex = 0; /* Not used */

  memset((void *)&elem, 0, sizeof(elem));
  memcpy(&(elem.bcm_data.s_ip_addr),  &(bcm_ipmc->s_ip_addr),  sizeof(elem.bcm_data.s_ip_addr));
  memcpy(&(elem.bcm_data.mc_ip_addr), &(bcm_ipmc->mc_ip_addr), sizeof(elem.bcm_data.mc_ip_addr));
  memcpy(&(elem.bcm_data.vid),        &(bcm_ipmc->vid),        sizeof(elem.bcm_data.vid));

  return usl_delete_ipmc_db_elem(USL_CURRENT_DB, elemInfo);
}

/*********************************************************************
* @purpose  Update the port_tgid for a group
*
* @param    bcm_ipmc {(input)} IP Mcast entry info
*
* @returns BCM Error Code
*
* @end
*********************************************************************/
int usl_db_ipmc_port_tgid_set(usl_bcm_ipmc_addr_t *bcm_ipmc)
{
  usl_ipmc_db_key_t   key;
  usl_ipmc_db_elem_t *pData;
  int                 rv = BCM_E_NONE;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

   /* initialize to zero so the invalidate will work */
  memset((void *)&key , 0, sizeof(key));

  memcpy(&(key.s_ip_addr), &(bcm_ipmc->s_ip_addr), sizeof(key.s_ip_addr));
  memcpy(&(key.mc_ip_addr), &(bcm_ipmc->mc_ip_addr), sizeof(key.mc_ip_addr));
  memcpy(&(key.vid), &(bcm_ipmc->vid), sizeof(key.vid));

  /* get the info in our AVL tree */
  pData = avlSearchLVL7(pUslIpMcastTreeHandle, &key ,AVL_EXACT);

  if (pData != L7_NULLPTR)
  {
    pData->bcm_data.ts        = bcm_ipmc->ts;
    pData->bcm_data.mod_id    = bcm_ipmc->mod_id;
    pData->bcm_data.port_tgid = bcm_ipmc->port_tgid;
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_DEBUG_PRINT("Setting ts %d, modid %d, port_tgid %d in IPMC DB, rv == %d\n", bcm_ipmc->ts, bcm_ipmc->mod_id, bcm_ipmc->port_tgid, rv);
  usl_db_ipmc_key_print(&key);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Update the L2 ports for a group
*
* @param    bcm_ipmc {(input)} IP Mcast entry info
* @param    cmd      {(input)} Add/Remove ports from L2 bitmap for group
*
* @returns BCM Error Code
*
* @end
*********************************************************************/
int usl_db_ipmc_l2_ports_update(usl_bcm_ipmc_addr_t *ipmc, USL_CMD_t updateCmd)
{
  usl_ipmc_db_key_t    key;
  usl_ipmc_db_elem_t  *pData;
  int                  rv = BCM_E_NONE;
  L7_uint32            idx;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

   /* initialize to zero so the invalidate will work */
  memset((void *)&key, 0, sizeof(key));

  memcpy(&(key.s_ip_addr), &(ipmc->s_ip_addr), sizeof(key.s_ip_addr));
  memcpy(&(key.mc_ip_addr), &(ipmc->mc_ip_addr), sizeof(key.mc_ip_addr));
  memcpy(&(key.vid), &(ipmc->vid), sizeof(key.vid));

  /* get the info in our AVL tree */
  pData = avlSearchLVL7(pUslIpMcastTreeHandle, &key ,AVL_EXACT);

  if (pData != L7_NULLPTR)
  {
    for (idx = 0; idx < L7_MOD_MAX; idx++)
    {
      if (updateCmd == USL_CMD_ADD)
      {
        BCM_PBMP_OR(pData->bcm_data.l2_pbmp[idx], ipmc->l2_pbmp[idx]);
        BCM_PBMP_REMOVE(pData->bcm_data.l2_ubmp[idx], ipmc->l2_pbmp[idx]);
        BCM_PBMP_OR(pData->bcm_data.l2_ubmp[idx], ipmc->l2_ubmp[idx]);
      }
      else if (updateCmd == USL_CMD_REMOVE)
      {
        BCM_PBMP_REMOVE(pData->bcm_data.l2_pbmp[idx], ipmc->l2_pbmp[idx]);
        BCM_PBMP_REMOVE(pData->bcm_data.l2_ubmp[idx], ipmc->l2_pbmp[idx]);
      }
      else if (updateCmd == USL_CMD_SET)
      {
        BCM_PBMP_ASSIGN(pData->bcm_data.l2_pbmp[idx], ipmc->l2_pbmp[idx]);
        BCM_PBMP_ASSIGN(pData->bcm_data.l2_ubmp[idx], ipmc->l2_ubmp[idx]);
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
    rv = BCM_E_NOT_FOUND;
  }

  USL_DEBUG_PRINT("Updating (%s) L2 ports in IPMC DB, rv == %d\n",
                  (updateCmd == USL_CMD_ADD) ? "ADD" : (updateCmd == USL_CMD_REMOVE) ? "REMOVE" : "SET", rv);
  usl_db_ipmc_key_print(&key);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Update the L3 ports for a group
*
* @param    bcm_ipmc {(input)} IP Mcast entry info
* @param    cmd      {(input)} Add/Remove ports from L3 bitmap for group
*
* @returns BCM Error Code
*
* @end
*********************************************************************/
int usl_db_ipmc_l3_ports_update(usl_bcm_ipmc_addr_t *ipmc, USL_CMD_t updateCmd)
{
  usl_ipmc_db_key_t    key;
  usl_ipmc_db_elem_t  *pData;
  int                  rv = BCM_E_NONE;
  L7_uint32            idx;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

   /* initialize to zero so the invalidate will work */
  memset((void *)&key, 0, sizeof(key));

  memcpy(&(key.s_ip_addr), &(ipmc->s_ip_addr), sizeof(key.s_ip_addr));
  memcpy(&(key.mc_ip_addr), &(ipmc->mc_ip_addr), sizeof(key.mc_ip_addr));
  memcpy(&(key.vid), &(ipmc->vid), sizeof(key.vid));

  /* get the info in our AVL tree */
  pData = avlSearchLVL7(pUslIpMcastTreeHandle, &key ,AVL_EXACT);

  if (pData != L7_NULLPTR)
  {
    for (idx = 0; idx < L7_MOD_MAX; idx++)
    {
      if (updateCmd == USL_CMD_ADD)
      {
        BCM_PBMP_OR(pData->bcm_data.l3_pbmp[idx], ipmc->l3_pbmp[idx]);
      }
      else if (updateCmd == USL_CMD_REMOVE)
      {
        BCM_PBMP_REMOVE(pData->bcm_data.l3_pbmp[idx], ipmc->l3_pbmp[idx]);
      }
      else if (updateCmd == USL_CMD_SET)
      {
        BCM_PBMP_ASSIGN(pData->bcm_data.l3_pbmp[idx], ipmc->l3_pbmp[idx]);
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
    rv = BCM_E_NOT_FOUND;
  }

  USL_DEBUG_PRINT("Updating (%s) L3 ports in IPMC DB, rv == %d\n",
                  (updateCmd == USL_CMD_ADD) ? "ADD" : (updateCmd == USL_CMD_REMOVE) ? "REMOVE" : "SET", rv);
  usl_db_ipmc_key_print(&key);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set/Remove a port in L2 bitmap for a number of groups
*
* @param   modid       {(input)}  Modid of the port
* @param   bcmPort     {(input)}  Bcm port number
* @param   cmd         {(input)}  Add/Remove the port to groups
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call.
* @param   tagged      {(input)} Flag indicating whether this VLAN should be tagged.
*
*
* @returns BCM Error Code
*
* @end
*********************************************************************/
int usl_db_ipmc_update_l2_port_groups (L7_int32    modid,
                                       L7_int32    bcmPort,
                                       USL_CMD_t   updateCmd,
                                       L7_uint32  *ipmc_index,
                                       L7_uint32   num_groups,
                                       L7_uint32   vlan_id,
                                       L7_uint32   tagged)
{
  usl_ipmc_db_elem_t *pData;
  L7_uint32           i;
  int                 rv = BCM_E_NONE;
  L7_uint32           max;

  max = platMrtrRoutesMaxEntriesGet();

  if (uslIpMcDbActive != L7_TRUE)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] < max)
    {
      if (pUslIpMcastDataByIndex[ipmc_index[i]] != L7_NULLPTR)
      {
        pData = pUslIpMcastDataByIndex[ipmc_index[i]];

        if (updateCmd == USL_CMD_ADD)
        {
          BCM_PBMP_PORT_ADD(pData->bcm_data.l2_pbmp[modid], bcmPort);
          if (tagged == L7_FALSE)
          {
            BCM_PBMP_PORT_ADD(pData->bcm_data.l2_ubmp[modid], bcmPort);
          }
          else
          {
            BCM_PBMP_PORT_REMOVE(pData->bcm_data.l2_ubmp[modid], bcmPort);
          }
        }
        else if (updateCmd == USL_CMD_REMOVE)
        {
          BCM_PBMP_PORT_REMOVE(pData->bcm_data.l2_pbmp[modid], bcmPort);
          BCM_PBMP_PORT_REMOVE(pData->bcm_data.l2_ubmp[modid], bcmPort);
        }

        USL_DEBUG_PRINT("Updating (%s) L2 port in IPMC DB, modid %d, port %d, rv == %d\n",
                        (updateCmd == USL_CMD_ADD) ? "ADD" : "REMOVE", modid, bcmPort, rv);
        usl_db_ipmc_key_print((usl_ipmc_db_key_t *)&pData->bcm_data);
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "NULL pointer for IPMC index %d", ipmc_index[i]);
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Bad IPMC index %d", ipmc_index[i]);
    }
  }

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set/Remove a port in L3 bitmap for a number of groups
*
* @param   modid           {(input)}  Modid of the port
* @param   bcmPort         {(input)}  Bcm port number
* @param   cmd             {(input)}  Add/Remove the port to groups
* @param   *index          {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups      {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id         {(input)}  outgoing VLAN ID.
*
* @returns BCM Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging.
*
*
* @end
*********************************************************************/
int
usl_db_ipmc_update_l3_port_groups (L7_int32   modid,
                                   L7_int32   bcmPort,
                                   USL_CMD_t  updateCmd,
                                   L7_uint32 *ipmc_index,
                                   L7_uint32  num_groups,
                                   L7_uint32  vlan_id)
{
  usl_ipmc_db_elem_t      *pData;
  usl_ipmc_port_db_elem_t *pPortData;
  L7_uint32                i;
  int                      rv = BCM_E_NONE;
  bcm_vlan_vector_t        vlanVector;
  L7_uint32                max;

  max = platMrtrRoutesMaxEntriesGet();

  if (uslIpMcDbActive != L7_TRUE)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

  pPortData = usl_db_ipmc_egress_port_ptr_get(modid, bcmPort);
  if (pPortData == L7_NULL)
  {
    USL_IPMC_DB_LOCK_GIVE();
    return BCM_E_FAIL;
  }

  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] < max)
    {
      if (pUslIpMcastDataByIndex[ipmc_index[i]] != L7_NULLPTR)
      {
        pData = pUslIpMcastDataByIndex[ipmc_index[i]];

        usl_db_ipmc_repl_get(modid, bcmPort, pData->bcm_data.ipmc_index, vlanVector);

        if (updateCmd == USL_CMD_ADD)
        {
          BCM_VLAN_VEC_SET(vlanVector, vlan_id);
          BCM_PBMP_PORT_ADD(pData->bcm_data.l3_pbmp[modid], bcmPort);
          USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET(pData->bcm_data.ipmc_index, pPortData->ipmcIndexMask);
        }
        else
        {
          BCM_VLAN_VEC_CLR(vlanVector, vlan_id);

          if (usl_db_ipmc_vlan_vector_is_empty(vlanVector))
          {
            BCM_PBMP_PORT_REMOVE(pData->bcm_data.l3_pbmp[modid], bcmPort);
            USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(pData->bcm_data.ipmc_index, pPortData->ipmcIndexMask);
          }
        }

        rv = usl_db_ipmc_repl_set(modid, bcmPort, pData->bcm_data.ipmc_index, vlanVector);

        USL_DEBUG_PRINT("Updating (%s) L3 port in IPMC DB, modid %d, port %d, ipmcIndex %d, vlan %d, rv == %d\n",
                        (updateCmd == USL_CMD_ADD) ? "ADD" : "REMOVE", modid, bcmPort, ipmc_index[i], vlan_id, rv);
        usl_db_ipmc_key_print((usl_ipmc_db_key_t *)&pData->bcm_data);
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "NULL pointer for IPMC index %d", ipmc_index[i]);
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Bad IPMC index %d", ipmc_index[i]);
    }
  }

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Return info for a IPMC group from USL Db
*
* @params   dbType    @{(input)} DbType to be searched
* @params   key       @{(input)} Key to search the group
* @params   groupInfo @{(output)} Group Info
*
* @returns  BCM error code
*
* @notes
*
* @end
*********************************************************************/
int usl_db_ipmc_get(USL_DB_TYPE_t dbType,
                    usl_bcm_ipmc_addr_t *ipmcKey,
                    usl_bcm_ipmc_addr_t *groupInfo)
{
  int                   rv = BCM_E_NONE;
  avlTree_t            *mcastTree;
  usl_ipmc_db_key_t     key;
  usl_ipmc_db_elem_t   *pData;

  if (!uslIpMcDbActive)
  {
    rv = BCM_E_NOT_FOUND;
    return rv;
  }

  if ((dbType == USL_SHADOW_DB) && (pUslShadowIpMcastTreeHeap == L7_NULLPTR))
  {
    rv = BCM_E_NOT_FOUND;
    return rv;
  }


  USL_IPMC_DB_LOCK_TAKE();

  mcastTree = (dbType == USL_SHADOW_DB) ? (&uslShadowIpMcastTreeData) : (&uslOperIpMcastTreeData);

   /* initialize to zero so the invalidate will work */
  memset((void *)&key , 0, sizeof(key));

  memcpy(&(key.s_ip_addr), &(ipmcKey->s_ip_addr), sizeof(key.s_ip_addr));
  memcpy(&(key.mc_ip_addr), &(ipmcKey->mc_ip_addr), sizeof(key.mc_ip_addr));
  memcpy(&(key.vid), &(ipmcKey->vid), sizeof(key.vid));

  /* get the info in our AVL tree */
  pData = avlSearchLVL7(mcastTree, &key ,AVL_EXACT);

  if (pData != L7_NULLPTR)
  {
    memcpy(groupInfo, pData, sizeof(*groupInfo));
  }
  else
  {
    rv = BCM_E_NOT_FOUND;
  }

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Initialize IPMC hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_ipmc_hw_id_generator_init(void)
{
  L7_uint32 max;

  max = platMrtrRoutesMaxEntriesGet();

  /* There is currently no API to get the range of instance number that SDK expects.
  ** Until such API is available we use the index from 0 to max number of
  ** IPMC Groups supported by FASTPATH
  */
  uslIpmcHwIdMin = 0;
  uslIpmcHwIdMax = max - 1;

  pUslIpmcHwIdList = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                 sizeof(usl_ipmc_hw_id_list_t) * (uslIpmcHwIdMax + 1));
  if (pUslIpmcHwIdList == L7_NULLPTR)
    return L7_FAILURE;

  memset(pUslIpmcHwIdList, 0, sizeof(usl_ipmc_hw_id_list_t) * (uslIpmcHwIdMax + 1));

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
L7_RC_t usl_ipmc_hw_id_generator_reset()
{
  L7_RC_t rc = L7_SUCCESS;

  /* Clear the TrunkHwIdList array */
  memset(pUslIpmcHwIdList, 0, sizeof(usl_ipmc_hw_id_list_t) * (uslIpmcHwIdMax + 1));

  return rc;
}

/*********************************************************************
* @purpose  Reset and populate the IPMC Hw id generator from USL
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
L7_RC_t usl_ipmc_hw_id_generator_sync()
{
  usl_ipmc_db_elem_t    searchElem, ipmcElem;
  uslDbElemInfo_t       searchInfo, elemInfo;
  L7_uint32             ipmcIndex;

  usl_ipmc_hw_id_generator_reset();

  memset(&ipmcElem, 0, sizeof(ipmcElem));
  memset(&searchElem, 0, sizeof(searchElem));

  searchInfo.dbElem = (void *) &searchElem;
  searchInfo.elemIndex = USL_INVALID_DB_INDEX;

  elemInfo.dbElem = &ipmcElem;
  elemInfo.elemIndex = USL_INVALID_DB_INDEX;

  while (usl_get_ipmc_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM,
                              searchInfo, &elemInfo) == BCM_E_NONE)
  {
    memcpy(searchInfo.dbElem, elemInfo.dbElem,
           sizeof(usl_ipmc_db_elem_t));
    memcpy(&(searchInfo.elemIndex), &(elemInfo.elemIndex),
           sizeof(elemInfo.elemIndex));

    ipmcIndex = ((usl_ipmc_db_elem_t *)elemInfo.dbElem)->bcm_data.ipmc_index;

    if ((ipmcIndex < uslIpmcHwIdMin) || (ipmcIndex > uslIpmcHwIdMax))
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Invalid ipmcIndex %d \n",
                  ipmcIndex);
      continue;
    }

    /* Mark this index as used in IpmcHwList */
    pUslIpmcHwIdList[ipmcIndex].used = L7_TRUE;

  }

  return L7_SUCCESS;
}

void usl_ipmc_hw_id_generator_dump()
{
  L7_uint32 idx, entryCount = 0;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "IPMC Min Index %d Max Index %d Used Hw Indexes: ",
                uslIpmcHwIdMin, uslIpmcHwIdMax);
  for (idx = uslIpmcHwIdMin; idx <= uslIpmcHwIdMax; idx++)
  {
    if (pUslIpmcHwIdList[idx].used == L7_TRUE)
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
* @purpose  Allocate a hw ipmc index for a given group
*
* @param    groupInfo @{(input)}  ipmc group
* @param    index     @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_ipmc_hw_id_allocate(usl_bcm_ipmc_addr_t *ipmcAddr, L7_int32 *index)
{
  int                  rv = BCM_E_FULL, dbRv;
  usl_bcm_ipmc_addr_t  groupInfo;
  L7_int32             idx = uslIpmcHwIdMin;

  *index = 0;
  memset(&groupInfo, 0, sizeof(groupInfo));

  do
  {
    /* Search the operational db for this group.
    ** Always search, the operational DB, even during
    ** warm start, as we want to ensure that entries
    ** added after a failover get the same IPMC index
    ** that was used previously.
    ** If group is not found then an unused index is generated.
    */
    dbRv = usl_db_ipmc_get(USL_OPERATIONAL_DB, ipmcAddr, &groupInfo);
    if (dbRv == BCM_E_NONE) /* Group found */
    {
      *index = groupInfo.ipmc_index;
      if ((*index > uslIpmcHwIdMax) || (*index < uslIpmcHwIdMin))
      {
        LOG_ERROR(*index);
      }

      /* Mark this index as used in HwIdList */
      pUslIpmcHwIdList[*index].used = L7_TRUE;
      rv = BCM_E_NONE;
      break;
    }

    for (idx = uslIpmcHwIdMin; idx <= uslIpmcHwIdMax; idx++)
    {
      /* Found an unused index */
      if (pUslIpmcHwIdList[idx].used == L7_FALSE)
      {
        pUslIpmcHwIdList[idx].used = L7_TRUE;
        *index = idx;
        rv = BCM_E_NONE;
        break;
      }
    }
  } while(0);

  return rv;
}

/*********************************************************************
* @purpose  Free a given ipmc index
*
* @param    index   @{(input)} ipmc index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_ipmc_hw_id_free(L7_int32 index)
{
  int rv = BCM_E_NONE;

  if ((index < uslIpmcHwIdMin) || (index > uslIpmcHwIdMax))
  {
    rv = BCM_E_FAIL;
    return rv;
  }
  else
  {
    pUslIpmcHwIdList[index].used = L7_FALSE;
  }

  return rv;
}


/*
 * Debug routines : Works only for IPv4. Dont try for IPv6.
 */
L7_uint32 usl_ipmc_print(USL_DB_TYPE_t dbType,
                         L7_uint32 count,L7_uint32 mcip, L7_uint32 sip, L7_uint32 vid, L7_BOOL hitOnly)
{

  usl_ipmc_db_elem_t  data;
  usl_ipmc_db_elem_t *pData;
  L7_uint32           num_found = 0;
  L7_uint32           tmpPort, i;
  avlTree_t          *mcastTree;

  if (!uslIpMcDbActive)
  {
    sysapiPrintf("USL IPMC Db is not active\n");
  }
  else
  {
    sysapiPrintf("USL IPMC Db is active\n");
  }

  if (dbType == USL_SHADOW_DB)
  {
    mcastTree = &uslShadowIpMcastTreeData;
  }
  else /* Default to Operational */
  {
    mcastTree = &uslOperIpMcastTreeData;
  }

  USL_IPMC_DB_LOCK_TAKE();

  sysapiPrintf("\nThis routine works only for IPv4 \n");
  memset(&data, 0, sizeof(data));

  /* pack key */
  memcpy(&(data.bcm_data.mc_ip_addr), &mcip, L7_IPV4_ADDRESS_SIZE);
  memcpy(&(data.bcm_data.s_ip_addr), &sip, L7_IPV4_ADDRESS_SIZE);
  data.bcm_data.vid = vid;

  if (mcip || sip || vid)
    pData = avlSearchLVL7(mcastTree, &data ,AVL_EXACT);
  else
    pData = avlSearchLVL7(mcastTree, &data ,AVL_NEXT);

  if (pData != L7_NULLPTR)
    memcpy (&data, pData, sizeof (usl_ipmc_db_elem_t ));

  while (pData != L7_NULLPTR)
  {
    /* Skip IPv6 entries */
    if (!(pData->bcm_data.flags & BCM_IPMC_IP6))
    {
      if ((hitOnly == L7_FALSE) || (data.lastHit && hitOnly))
      {
        sysapiPrintf("****** IPMC INDEX = %d *******\n",
                     data.bcm_data.ipmc_index);
        sysapiPrintf("Ip addr=%x:%x:%x:%x , Src IP addr=%x:%x:%x:%x , VID=%d\n",
                     data.bcm_data.mc_ip_addr[0],
                     data.bcm_data.mc_ip_addr[1],
                     data.bcm_data.mc_ip_addr[2],
                     data.bcm_data.mc_ip_addr[3],
                     data.bcm_data.s_ip_addr[0],
                     data.bcm_data.s_ip_addr[1],
                     data.bcm_data.s_ip_addr[2],
                     data.bcm_data.s_ip_addr[3],
                     data.bcm_data.vid);

        sysapiPrintf("Port_tgid=%d , IPMC Index=%d , Flags=%x , Inuse=%d\n",
                     data.bcm_data.port_tgid,
                     data.bcm_data.ipmc_index,
                     data.bcm_data.flags, data.lastHit);

        sysapiPrintf("L2 port bitmap\n");
        for (i = 0; i < L7_MOD_MAX; i++)
        {
          if (BCM_PBMP_NOT_NULL(pData->bcm_data.l2_pbmp[i]))
          {
            sysapiPrintf("modid %d pbmp\n", i);
            BCM_PBMP_ITER(pData->bcm_data.l2_pbmp[i], tmpPort)
            {
              sysapiPrintf("%d ", tmpPort);
            }
            sysapiPrintf("\n");
          }
        }

        sysapiPrintf("L2 untagged port bitmap\n");
        for (i = 0; i < L7_MOD_MAX; i++)
        {
          if (BCM_PBMP_NOT_NULL(pData->bcm_data.l2_ubmp[i]))
          {
            sysapiPrintf("modid %d pbmp\n", i);
            BCM_PBMP_ITER(pData->bcm_data.l2_ubmp[i], tmpPort)
            {
              sysapiPrintf("%d ", tmpPort);
            }
            sysapiPrintf("\n");
          }
        }

        sysapiPrintf("L3 port bitmap\n");
        for (i = 0; i < L7_MOD_MAX; i++)
        {
          if (BCM_PBMP_NOT_NULL(pData->bcm_data.l3_pbmp[i]))
          {
            sysapiPrintf("modid %d pbmp\n", i);
            BCM_PBMP_ITER(pData->bcm_data.l3_pbmp[i], tmpPort)
            {
              sysapiPrintf("%d ", tmpPort);
            }
            sysapiPrintf("\n");
          }
        }
      }

    }

    pData = avlSearchLVL7(mcastTree, &data, AVL_NEXT);

    if (pData)
      memcpy (&data, pData, sizeof (usl_ipmc_db_elem_t ));

    num_found++;

    if (num_found == count)
    {
      break;
    }
  }

  USL_IPMC_DB_LOCK_GIVE();

  return num_found;
}

void usl_ipmc_sem_show()
{
  if (uslIpmcInited)
    sysapiPrintf("IPMC Sema = 0x%0.8x\n",pUslIpMcastDbSema);
}

void usl_ipmc_debug_help()
{
  sysapiPrintf("\n****************** USL IPMC DEBUG HELP ***************\n");
  sysapiPrintf("USL IPMC %s\n",(uslIpmcInited)?"active":"inactive");
  sysapiPrintf("usl_ipmc_print(count,mcip, sip, vid)\n");
  sysapiPrintf("usl_ipmc_sem_show() - show all semaphores for ipmc\n");
}


#ifdef L7_STACKING_PACKAGE

/*********************************************************************
* @purpose  The timer callback routine for the IPMC in use check
*
* @param    ipmc_index    @{(input)} The Multicast IP address index
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
void usl_ipmc_inuse_timer(L7_uint32 arg1, L7_uint32 arg2)
{
  L7_RC_t rc = L7_SUCCESS;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_uchar8 *msg;
  static L7_uint32 msgLength;
  L7_uint32 thisFpUnit;

  if (firstTime == L7_TRUE)
  {
    msgLength = USL_MSG_SIZE_HDR ;
    msg = osapiMalloc(L7_DRIVER_COMPONENT_ID,msgLength);
    firstTime = L7_FALSE;
  }

  if ((rc = unitMgrNumberGet(&thisFpUnit)) == L7_SUCCESS)
  {

    *(L7_uint32 *) &msg[USL_MSG_TYPE_OFFSET] = USL_IPMC_INUSE_TIMER_POP;
    *(L7_uint32 *) &msg[USL_MSG_UNIT_OFFSET] = thisFpUnit;
    *(L7_uint32 *) &msg[USL_MSG_ELEM_OFFSET] = 0;

    rc = usl_worker_task_msg_send(msg, msgLength, L7_NO_WAIT );

    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
              "usl_task_ipmc_msg_send(): failed to send with %d\n",rc);
    }

  }

  /* always re add the timer */
  osapiTimerAdd ( (void*)usl_ipmc_inuse_timer, arg1, arg2, USL_IPMC_INUSE_TIMER_INTERVAL_MSEC,
                  &pUslIpmcInUseTimer );

  return;
}


/*********************************************************************
* @purpose  Check for in use impc groups
*
* @end
*********************************************************************/
void usl_ipmc_inuse_check(void)
{
  usl_ipmc_db_elem_t *pData;
  bcm_ipmc_addr_t   ipmcInfo;
  usl_ipmc_db_key_t ipmcInuse;
  int bcm_unit;
  L7_uchar8 *msg;
  L7_uchar8 *msgPtr;
  L7_uint32 mgmtUnit, thisFpUnit, now, maxEntries;
  usl_ipmc_db_elem_t data;
  L7_uint32 num_elements = 0;
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL isHit;

  memset(&data, 0, sizeof(usl_ipmc_db_elem_t));

  /* if we can't get the mgmt unit number, get out */
  if (unitMgrMgrNumberGet(&mgmtUnit) != L7_SUCCESS)
    return;

  if (unitMgrNumberGet(&thisFpUnit)  != L7_SUCCESS)
    return;

  USL_IPMC_DB_LOCK_TAKE();

  msg = usl_tx_buffer_alloc();

  /* Find out max entries can be sent in the mgs buffer */
  maxEntries = (usl_tx_buffer_size() - (USL_MSG_SIZE_HDR + sizeof(L7_int32)))
                  / sizeof(usl_ipmc_db_key_t);

  /* We are seeing issues where large (4k - 8k) messages are occasionally
     being dropped on the manager unit in HW when the mgr CPU is receiving
     alot of IGMP joins and IPMC data.
     This issue does not seem to occur if we limit the
     max message size here to ~2K. See 64191.*/
  maxEntries /= 4;

  /* check that at least one entry will fit in the message */
  if (maxEntries == 0)
  {
    USL_LOG_ERROR("USL: Tx message size is too small for ipmc inuse check\n");
    usl_tx_buffer_free(msg);
    USL_IPMC_DB_LOCK_GIVE();
    return;
  }

  now = osapiUpTimeRaw();

  /* setup the message initially */
  *(L7_uint32 *)&msg[USL_MSG_TYPE_OFFSET] = USL_IPMC_INUSE;

  msgPtr = &msg[USL_MSG_DATA_OFFSET];

  pData = avlSearchLVL7(pUslIpMcastTreeHandle, &data ,AVL_NEXT);

  while ( pData )
  {
    isHit = L7_FALSE;
    /* check for the inuse */
    for (bcm_unit = 0; bcm_unit < soc_ndev; bcm_unit++)
    {
      if (!BCM_IS_FABRIC(bcm_unit))
      {
        memset(&ipmcInuse, 0, sizeof(ipmcInuse));
        memset(&ipmcInfo,  0, sizeof(ipmcInfo));
        /* Read and clear the entry */
        ipmcInfo.flags = BCM_IPMC_HIT_CLEAR;
        if (pData->bcm_data.flags & BCM_IPMC_IP6)
        {
          memcpy(&(ipmcInfo.s_ip6_addr), &(pData->bcm_data.s_ip_addr), L7_IPV6_ADDRESS_SIZE);
          memcpy(&(ipmcInfo.mc_ip6_addr),&(pData->bcm_data.mc_ip_addr), L7_IPV6_ADDRESS_SIZE);
          ipmcInfo.flags |= BCM_IPMC_IP6;

          memcpy(&(ipmcInuse.s_ip_addr), &(pData->bcm_data.s_ip_addr), L7_IPV6_ADDRESS_SIZE);
          memcpy(&(ipmcInuse.mc_ip_addr),&(pData->bcm_data.mc_ip_addr), L7_IPV6_ADDRESS_SIZE);
        }
        else
        {
          memcpy(&(ipmcInfo.s_ip_addr), &(pData->bcm_data.s_ip_addr), L7_IPV4_ADDRESS_SIZE);
          memcpy(&(ipmcInfo.mc_ip_addr),&(pData->bcm_data.mc_ip_addr), L7_IPV4_ADDRESS_SIZE);

          memcpy(&(ipmcInuse.s_ip_addr), &(pData->bcm_data.s_ip_addr), L7_IPV4_ADDRESS_SIZE);
          memcpy(&(ipmcInuse.mc_ip_addr),&(pData->bcm_data.mc_ip_addr), L7_IPV4_ADDRESS_SIZE);

        }
        ipmcInfo.vid    = pData->bcm_data.vid;
        ipmcInuse.vid = pData->bcm_data.vid;

        bcm_ipmc_find(bcm_unit, &ipmcInfo);  /* IPv6 compliant API */

        if (ipmcInfo.flags & BCM_IPMC_HIT)
        {
          /* break out of the chip loop */
          isHit = L7_TRUE;
        }
      }
    }

    if (isHit == L7_TRUE)
    {
      /* the entry is hit, send a result */
      num_elements++;

      memcpy(msgPtr, &ipmcInuse, sizeof(usl_ipmc_db_key_t));
      msgPtr += sizeof(usl_ipmc_db_key_t);

      pData->lastHit = now;
    }
    else if (mgmtUnit == thisFpUnit)
    {
      if ((now - pData->lastHit) > 2*USL_IPMC_INUSE_TIMER_INTERVAL_SEC)
      {
        /* we haven't seen this in a while, let's set it to not hit */
        pData->lastHit = 0;
      }
    }
    else
    {
      /* Not on the mgmt unit, set it to not hit */
      pData->lastHit = 0;
    }

    if (num_elements == maxEntries)
    {
      *(L7_uint32 *) &msg[USL_MSG_UNIT_OFFSET] = thisFpUnit;
      *(L7_uint32 *) &msg[USL_MSG_ELEM_OFFSET] = num_elements;
      if (thisFpUnit != mgmtUnit)
      {
        rc = sysapiHpcMessageSend(L7_DRIVER_COMPONENT_ID, mgmtUnit, msgPtr - msg , msg);
      }
      if (rc != L7_SUCCESS)
      {
        usl_trace_generic("USL: failed to do ipmc inuse check\n");
      }

      num_elements = 0;
      *(L7_uint32 *)&msg[USL_MSG_TYPE_OFFSET] = USL_IPMC_INUSE;
      msgPtr = &msg[USL_MSG_DATA_OFFSET];
    }

    pData = avlSearchLVL7(pUslIpMcastTreeHandle, pData, AVL_NEXT);

  }

  /* send the message if there are any inuse addresses */
  if ((num_elements > 0) && (thisFpUnit != mgmtUnit))
  {
    *(L7_uint32 *) &msg[USL_MSG_UNIT_OFFSET] = thisFpUnit;
    *(L7_uint32 *) &msg[USL_MSG_ELEM_OFFSET] = num_elements;

    /* send the message and move on */
    rc = sysapiHpcMessageSend(L7_DRIVER_COMPONENT_ID, mgmtUnit, msgPtr - msg , msg);

    if (rc != L7_SUCCESS)
    {
      usl_trace_generic("USL: failed to do ipmc inuse check\n");
    }
  }

  usl_tx_buffer_free(msg);

  USL_IPMC_DB_LOCK_GIVE();

  return;
}

/*********************************************************************
* @purpose  Process the inuse check message
*
* @param    msg     @{(input)} The message with the groups
*
* @end
*********************************************************************/
void usl_ipmc_inuse_process(L7_uchar8 *msg)
{
  L7_uint32 elemSize, index, numElem;
  usl_ipmc_db_elem_t data ;
  usl_ipmc_db_elem_t *pData;
  L7_uchar8 *msgPtr;

  memset(&data, 0, sizeof(usl_ipmc_db_elem_t));

  /* usl_ipmc_db_key_t size is copied by the sender */
  elemSize = sizeof(usl_ipmc_db_key_t);

  numElem = *(L7_uint32 *) &msg[USL_MSG_ELEM_OFFSET];
  msgPtr  = &msg[USL_MSG_DATA_OFFSET];

  USL_IPMC_DB_LOCK_TAKE();

  for (index = 0; index < numElem; index++)
  {
    memcpy((void *)&(data), msgPtr, elemSize);

    pData = avlSearchLVL7(pUslIpMcastTreeHandle, &data ,AVL_EXACT);

    if (pData != L7_NULLPTR)
    {
      pData->lastHit = osapiUpTimeRaw();
    }

    msgPtr += elemSize;
  }

  USL_IPMC_DB_LOCK_GIVE();

  return;
}

/*********************************************************************
* @purpose  Test whether an IPMC group is in use
*
* @param    bcm_ipmc      @{(input)}  IPMC entry to be used
*
* @returns  L7_TRUE if in use
*
* @notes    DOES NOT ADD DOWNSTREAM PORTS
*
* @end
*********************************************************************/
L7_BOOL usl_ipmc_inuse_get(usl_bcm_ipmc_addr_t *bcm_ipmc)
{
  usl_ipmc_db_elem_t data;
  usl_ipmc_db_elem_t *pData;
  L7_BOOL rc = L7_FALSE;


  /* initialize to zero */
  memset((void *)&data,0,sizeof(data));


  memcpy(&(data.bcm_data.s_ip_addr), &(bcm_ipmc->s_ip_addr), sizeof(data.bcm_data.s_ip_addr));
  memcpy(&(data.bcm_data.mc_ip_addr), &(bcm_ipmc->mc_ip_addr), sizeof(data.bcm_data.mc_ip_addr));
  memcpy(&data.bcm_data.vid, &(bcm_ipmc->vid), sizeof(bcm_vlan_t));

  USL_IPMC_DB_LOCK_TAKE();

  /* get the info in our AVL tree */
  pData = avlSearchLVL7(pUslIpMcastTreeHandle, &data, AVL_EXACT);

  if (pData != L7_NULLPTR)
  {
    if (pData->lastHit)
      rc = L7_TRUE;
    else
      rc = L7_FALSE;
  }
  else
  {
    rc = L7_FALSE;
  }

  USL_IPMC_DB_LOCK_GIVE();

  return rc;
}
#endif /* L7_STACKING_PACKAGE */


/*********************************************************************
*
* @purpose Sets the IPMC egress config for a port.
*
* @param   modid      {(input)} modid of L3 port
* @param   bcmPort    {(input)} BCM port
* @param   vlanId     {(input)} egress VLAN Id
* @param   untagged   {(input)} egress untagged config
*
*
* @returns BCM Error Code
*
* @notes
*
* @end
*
*********************************************************************/
int usl_db_ipmc_egress_port_set(L7_uint32 modid, L7_uint32 bcmPort, L7_ushort16 vlanId, L7_ushort16 untagged)
{
  int                       rv = BCM_E_NONE;
  usl_ipmc_port_db_elem_t  *pPortData;

  if (!uslIpMcDbActive)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    pPortData = usl_db_ipmc_egress_port_ptr_get(modid, bcmPort);
    if (pPortData == L7_NULL)
    {
      rv = BCM_E_FAIL;
      break;
    }

    pPortData->vlanId    = vlanId;
    pPortData->untagFlag = untagged;

  } while (0);

  USL_DEBUG_PRINT("Setting egress port info in IPMC Port DB, modid %d, port %d, vlan %d, untagged %d, rv == %d\n",
                  modid, bcmPort, vlanId, untagged, rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Get the size of IPMC Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_ipmc_db_elem(void)
{
  return (sizeof(usl_ipmc_db_elem_t));
}

/*********************************************************************
* @purpose  Get the size of IPMC Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_ipmc_port_db_elem(void)
{
  return (sizeof(usl_ipmc_port_db_elem_t));
}

/*********************************************************************
* @purpose  Print the contents of a IPMC Db element in specified buffer
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
void usl_print_ipmc_db_elem(void *item, L7_uchar8 *buffer, L7_uint32 size)
{
  usl_ipmc_db_elem_t  *pUslIpmcEntry = item;
  L7_uint32            ipAddr[4];
  L7_uint32            i;
  char                 pfmt1[SOC_PBMP_FMT_LEN];

  if ((pUslIpmcEntry->bcm_data.flags & BCM_IPMC_IP6) == BCM_IPMC_IP6)
  {
    for (i = 0; i < 4; i++)
    {
      memcpy(&ipAddr[i], &pUslIpmcEntry->bcm_data.s_ip_addr[i * 4], sizeof(L7_uint32));
    }
    osapiSnprintf(buffer, size, "src addr: %8.8x:%8.8x:%8.8x:%8.8x\n", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    for (i = 0; i < 4; i++)
    {
      memcpy(&ipAddr[i], &pUslIpmcEntry->bcm_data.mc_ip_addr[i * 4], sizeof(L7_uint32));
    }
    osapiSnprintfcat(buffer, size, "group addr: %8.8x:%8.8x:%8.8x:%8.8x\n", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
  }
  else
  {
    memcpy(&ipAddr[0], &pUslIpmcEntry->bcm_data.s_ip_addr[0], sizeof(L7_uint32));
    osapiSnprintf(buffer, size, "src addr: %8.8x\n", ipAddr[0]);
    memcpy(&ipAddr[0], &pUslIpmcEntry->bcm_data.mc_ip_addr[0], sizeof(L7_uint32));
    osapiSnprintfcat(buffer, size, "group addr: %8.8x\n", ipAddr[0]);
  }
  osapiSnprintfcat(buffer, size, "vid: %d\n",        pUslIpmcEntry->bcm_data.vid);
  osapiSnprintfcat(buffer, size, "ts: %d\n",         pUslIpmcEntry->bcm_data.ts);
  osapiSnprintfcat(buffer, size, "port_tgid: %d\n",  pUslIpmcEntry->bcm_data.port_tgid);
  osapiSnprintfcat(buffer, size, "mod_id: %d\n",     pUslIpmcEntry->bcm_data.mod_id);
  osapiSnprintfcat(buffer, size, "ipmc_index: %d\n", pUslIpmcEntry->bcm_data.ipmc_index);
  osapiSnprintfcat(buffer, size, "flags: 0x%x\n",    pUslIpmcEntry->bcm_data.flags);

  for (i = 0; i < L7_MOD_MAX; i++)
  {
    if (BCM_PBMP_NOT_NULL(pUslIpmcEntry->bcm_data.l2_pbmp[i]))
    {
      osapiSnprintfcat(buffer, size, "l2_pbmp[%d]: %s \n", i, SOC_PBMP_FMT(pUslIpmcEntry->bcm_data.l2_pbmp[i], pfmt1));
    }
  }

  for (i = 0; i < L7_MOD_MAX; i++)
  {
    if (BCM_PBMP_NOT_NULL(pUslIpmcEntry->bcm_data.l3_pbmp[i]))
    {
      osapiSnprintfcat(buffer, size, "l3_pbmp[%d]: %s \n", i, SOC_PBMP_FMT(pUslIpmcEntry->bcm_data.l3_pbmp[i], pfmt1));
    }
  }

  return;
}

/*********************************************************************
* @purpose  Print the contents of a IPMC Db element in specified buffer
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
void usl_print_ipmc_port_db_elem(void *item, L7_uchar8 *buffer, L7_uint32 size)
{
  usl_ipmc_port_db_elem_t  *pUslIpmcPortEntry = item;
  L7_uint32                 i, max;

  osapiSnprintf(buffer, size, "bcmModid: %d\n", pUslIpmcPortEntry->bcmModid);
  osapiSnprintfcat(buffer, size, "bcmPort: %d\n", pUslIpmcPortEntry->bcmPort);
  osapiSnprintfcat(buffer, size, "vlanId: %d\n", pUslIpmcPortEntry->vlanId);
  osapiSnprintfcat(buffer, size, "untagFlag: %d\n", pUslIpmcPortEntry->untagFlag);
  osapiSnprintfcat(buffer, size, "replInfo: %x\n", pUslIpmcPortEntry->replInfo);
  osapiSnprintfcat(buffer, size, "member of IPMC entries: ");
  max = platMrtrRoutesMaxEntriesGet();
  for (i = 0; i < max; i++)
  {
    if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(i, pUslIpmcPortEntry->ipmcIndexMask))
    {
      osapiSnprintfcat(buffer, size, "%d ", i);
    }
  }
  osapiSnprintfcat(buffer, size, "\n");

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
L7_int32 usl_get_ipmc_db_elem(USL_DB_TYPE_t     dbType,
                              L7_uint32         flags,
                              uslDbElemInfo_t   searchElem,
                              uslDbElemInfo_t  *elemInfo)
{
  L7_int32               rv = BCM_E_NONE;
  usl_ipmc_db_elem_t    *pUslIpmcEntry, *searchEntry;
  avlTree_t             *dbHandle = L7_NULLPTR;

  if (!uslIpMcDbActive)
  {
    return rv;
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;
  }

  searchEntry = (usl_ipmc_db_elem_t *) searchElem.dbElem;

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_ipmc_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    if (flags == USL_DB_NEXT_ELEM)
    {
      pUslIpmcEntry = avlSearchLVL7(dbHandle, searchEntry, AVL_NEXT);
    }
    else
    {
      pUslIpmcEntry = avlSearchLVL7(dbHandle, searchEntry, AVL_EXACT);
    }

    if (pUslIpmcEntry != L7_NULLPTR)
    {
      /* Element found, copy in the user memory */
      memcpy(elemInfo->dbElem, pUslIpmcEntry, sizeof(usl_ipmc_db_elem_t));
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_IPMC_DB_LOCK_GIVE();

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
L7_int32 usl_get_ipmc_port_db_elem(USL_DB_TYPE_t     dbType,
                                   L7_uint32         flags,
                                   uslDbElemInfo_t   searchElem,
                                   uslDbElemInfo_t  *elemInfo)
{
  L7_int32                 rv = BCM_E_NONE;
  usl_ipmc_port_db_elem_t *pUslIpmcPortEntry, *searchEntry;
  avlTree_t               *dbHandle = L7_NULLPTR;

  if (!uslIpMcDbActive)
  {
    return rv;
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;
  }

  searchEntry = (usl_ipmc_port_db_elem_t *) searchElem.dbElem;

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    dbHandle = usl_ipmc_port_db_handle_get(dbType);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    if (flags == USL_DB_NEXT_ELEM)
    {
      pUslIpmcPortEntry = avlSearchLVL7(dbHandle, searchEntry, AVL_NEXT);
    }
    else
    {
      pUslIpmcPortEntry = avlSearchLVL7(dbHandle, searchEntry, AVL_EXACT);
    }

    if (pUslIpmcPortEntry != L7_NULLPTR)
    {
      /* Element found, copy in the user memory */
      memcpy(elemInfo->dbElem, pUslIpmcPortEntry, sizeof(usl_ipmc_port_db_elem_t));
    }
    else
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from IPMC Db
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
L7_int32 usl_delete_ipmc_db_elem(USL_DB_TYPE_t   dbType,
                                 uslDbElemInfo_t elemInfo)
{
  usl_ipmc_db_elem_t      *elem = elemInfo.dbElem;
  avlTree_t               *dbHandle = L7_NULLPTR;
  avlTree_t               *portDbHandle = L7_NULLPTR;
  usl_ipmc_db_key_t        key;
  usl_ipmc_port_db_elem_t  portKey;
  usl_ipmc_db_elem_t      *pData;
  usl_ipmc_port_db_elem_t *pPortData;
  int                      rv = BCM_E_NONE;
  L7_int32                 max, ipmcIndex;
  bcm_vlan_vector_t        emptyVlanVector;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

  /* initialize to zero so the invalidate will work */
  memset(emptyVlanVector, 0, sizeof(bcm_vlan_vector_t));
  memset((void *)&key, 0, sizeof(key));

  memcpy(&(key.s_ip_addr),  &(elem->bcm_data.s_ip_addr),  sizeof(key.s_ip_addr));
  memcpy(&(key.mc_ip_addr), &(elem->bcm_data.mc_ip_addr), sizeof(key.mc_ip_addr));
  memcpy(&(key.vid),        &(elem->bcm_data.vid),        sizeof(key.vid));

  do
  {
    if ((dbHandle = usl_ipmc_db_handle_get(dbType)) == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    if ((portDbHandle = usl_ipmc_port_db_handle_get(dbType)) == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Delete the info in our AVL tree */
    pData = avlSearchLVL7(dbHandle, &key, AVL_EXACT);
    if (pData != L7_NULLPTR)
    {
      ipmcIndex = pData->bcm_data.ipmc_index;

      pData = avlDeleteEntry(dbHandle, &key);

      if (pData != L7_NULLPTR)
      {
        pData->isValid = L7_FALSE;
        max = platMrtrRoutesMaxEntriesGet();
        if (ipmcIndex < max)
        {
          if (dbType == USL_OPERATIONAL_DB)
          {
            pUslOperIpMcastDataByIndex[ipmcIndex] = L7_NULLPTR;
          }
          else if (dbType == USL_SHADOW_DB)
          {
            pUslShadowIpMcastDataByIndex[ipmcIndex] = L7_NULLPTR;
          }
          else
          {
            pUslIpMcastDataByIndex[ipmcIndex] = L7_NULLPTR;
          }
        }
        else
        {
          LOG_ERROR(ipmcIndex);
        }

        /* Deleting shadow DB entries is a normal part of reconciliation... don't
           modify IPMC port entries in the shadow DB. */
        if (dbType != USL_SHADOW_DB)
        {
          memset(&portKey, 0, sizeof(portKey));
          pPortData = avlSearchLVL7(portDbHandle, &portKey, AVL_EXACT);
          if (pPortData == L7_NULLPTR)
          {
            pPortData = avlSearchLVL7(portDbHandle, &portKey, AVL_NEXT);
          }

          while (pPortData != L7_NULLPTR)
          {
            memcpy(&portKey, pPortData, sizeof(portKey));

            if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, pPortData->ipmcIndexMask))
            {
              USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(ipmcIndex, pPortData->ipmcIndexMask);
              rv = usl_db_ipmc_repl_set(pPortData->bcmModid, pPortData->bcmPort, ipmcIndex, emptyVlanVector);
            }

            pPortData = avlSearchLVL7(portDbHandle, &portKey, AVL_NEXT);
          }
        }
      }
      else
      {
        rv = BCM_E_NOT_FOUND;
      }
    }
    else
    {
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_DEBUG_PRINT("Deleting entry from %s IPMC DB, rv == %d\n",
                  (dbType == USL_CURRENT_DB) ? "CURRENT" : (dbType == USL_OPERATIONAL_DB) ? "OPER" : "SHADOW", rv);
  usl_db_ipmc_key_print(&key);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from IPMC Db
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
L7_int32 usl_delete_ipmc_port_db_elem(USL_DB_TYPE_t   dbType,
                                      uslDbElemInfo_t elemInfo)
{
  usl_ipmc_port_db_elem_t   *elem = elemInfo.dbElem;
  avlTree_t                 *dbHandle = L7_NULLPTR;
  usl_ipmc_port_db_elem_t    key;
  usl_ipmc_port_db_elem_t   *pData;
  int                        rv = BCM_E_NONE;
  usl_ipmc_port_db_repl_t   *replInfo, *replInfoNext;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

  /* initialize to zero so the invalidate will work */
  memset((void *)&key, 0, sizeof(key));
  key.bcmModid = elem->bcmModid;
  key.bcmPort  = elem->bcmPort;

  do
  {
    if ((dbHandle = usl_ipmc_port_db_handle_get(dbType)) == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Need to search and free vlan vector first. */
    pData = avlSearchLVL7(dbHandle, &key, AVL_EXACT);
    if (pData != L7_NULLPTR)
    {
      replInfo = pData->replInfo;

      while (replInfo != L7_NULLPTR)
      {
        replInfoNext = replInfo->next;

        osapiFree(L7_DRIVER_COMPONENT_ID, replInfo);

        replInfo = replInfoNext;
      }
      pData->replInfo = L7_NULLPTR;

      /* Delete the info in our AVL tree */
      pData = avlDeleteEntry(dbHandle, &key);
    }
    else
    {
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_DEBUG_PRINT("Deleting IPMC Port DB entry, rv == %d\n", rv);
  USL_DEBUG_PRINT("  modid %d, port %d\n", key.bcmModid, key.bcmPort);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create a given element in IPMC Db
*
* @params   dbItem {(input)}  Pointer to db item to be created
*
* @returns  BCM error code
*
* @notes
*
* @end
*********************************************************************/
L7_int32 usl_create_ipmc_db_elem_bcm(void *dbItem)
{
  int                   rv            = BCM_E_NONE;
  usl_ipmc_db_elem_t   *pUslIpmcEntry = dbItem;

  USL_IPMC_DB_LOCK_TAKE();

  USL_DEBUG_PRINT("Creating IPMC element\n");
  usl_db_ipmc_key_print((usl_ipmc_db_key_t *)&pUslIpmcEntry->bcm_data);

  rv = usl_bcm_ipmc_add(&pUslIpmcEntry->bcm_data, L7_FALSE);
  if (rv == BCM_E_FULL)
  {
    /* If there was a hash collision, treat the operation as a success
       for the immediate code path. On the mgmt unit, given an indication
       to HAPI that the HW insert failed so that the HAPI retry mechanism
       can take effect. */
    rv = BCM_E_NONE;

    usl_db_sync_failure_notify_callback(USL_IPMC_ROUTE_DB_ID, &pUslIpmcEntry->bcm_data);
  }

  USL_DEBUG_PRINT("Created IPMC element, rv == %d\n", rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create a given element in IPMC Db
*
* @params   dbItem {(input)}  Pointer to db item to be created
*
* @returns  BCM error code
*
* @notes
*
* @end
*********************************************************************/
L7_int32 usl_create_ipmc_port_db_elem_bcm(void *dbItem)
{
  int                        rv = BCM_E_NONE;
  usl_ipmc_port_db_elem_t   *pUslIpmcPortEntry = dbItem;
  usl_ipmc_db_elem_t        *pUslIpmcEntry;
  L7_uint32                  bcm_unit, modid;
  L7_BOOL                    processCreate = L7_FALSE;
  L7_uint32                  ipmcIndex;
  bcm_vlan_vector_t          vlanVector;
  usl_ipmc_port_db_repl_t   *replInfo;

  USL_IPMC_DB_LOCK_TAKE();

  USL_DEBUG_PRINT("Creating IPMC Port element modid %d, port %d\n", pUslIpmcPortEntry->bcmModid, pUslIpmcPortEntry->bcmPort);

  do
  {
    /* Ignore creates for entries that we already know about. */
    if (avlSearchLVL7(pUslIpMcastPortTreeHandle, pUslIpmcPortEntry, AVL_EXACT) != L7_NULL)
    {
      USL_DEBUG_PRINT("  Ignoring, entry exists.\n");
      break;
    }

    /* Ignore port create calls for port that are not on this Fastpath unit. */
    for (bcm_unit=0; bcm_unit < soc_ndev; bcm_unit++)
    {
      rv = bcm_stk_my_modid_get(bcm_unit, &modid);
      if (rv != BCM_E_NONE)
      {
        break;
      }
      if (modid == pUslIpmcPortEntry->bcmModid)
      {
        processCreate = L7_TRUE;
        break;
      }
    }

    if (processCreate == L7_FALSE)
    {
      USL_DEBUG_PRINT("  Ignoring, no matching modid on this unit.\n");
      break;
    }

    rv = usl_bcm_ipmc_egress_port_set(pUslIpmcPortEntry->bcmModid, pUslIpmcPortEntry->bcmPort, pUslIpmcPortEntry->vlanId, pUslIpmcPortEntry->untagFlag);

    /* Need to iterate through all IPMC entries */
    for (ipmcIndex = 0; ipmcIndex < platMrtrRoutesMaxEntriesGet(); ipmcIndex++)
    {
      if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, pUslIpmcPortEntry->ipmcIndexMask))
      {
        pUslIpmcEntry = pUslIpMcastDataByIndex[ipmcIndex];
        if (pUslIpmcEntry != L7_NULLPTR)
        {
          memset(vlanVector, 0, sizeof(bcm_vlan_vector_t));

          replInfo = pUslIpmcPortEntry->replInfo;
          while (replInfo != L7_NULLPTR)
          {
            if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, replInfo->ipmcIndexMask))
            {
              BCM_VLAN_VEC_SET(vlanVector, replInfo->vlanId);
            }
            replInfo = replInfo->next;
          }

          rv = usl_bcm_ipmc_l3_port_repl_set(pUslIpmcPortEntry->bcmModid,
                                             pUslIpmcPortEntry->bcmPort,
                                             &pUslIpmcEntry->bcm_data,
                                             vlanVector);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;
          }
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Mismatch between IPMC DB and Port DB, modid %d, port %d, ipmcIndex %d",
                  pUslIpmcPortEntry->bcmModid,
                  pUslIpmcPortEntry->bcmPort,
                  ipmcIndex);
          rv = BCM_E_FAIL;
          break;
        }
      }
    }

  } while (0);

  USL_DEBUG_PRINT("Created IPMC Port element, rv == %d\n", rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from IPMC Db
*
* @params   dbItem {(input)}  Pointer to db item to be deleted
*
* @returns  BCM error code
*
* @notes    For IPMC Db, we should never get into this case
*
* @end
*********************************************************************/
L7_int32 usl_delete_ipmc_db_elem_bcm(void *dbItem)
{
  int rv;
  usl_ipmc_db_elem_t   *pUslIpmcEntry = dbItem;

  USL_IPMC_DB_LOCK_TAKE();

  USL_DEBUG_PRINT("Deleting IPMC element\n");
  usl_db_ipmc_key_print((usl_ipmc_db_key_t *)&pUslIpmcEntry->bcm_data);

  rv = usl_bcm_ipmc_delete(&pUslIpmcEntry->bcm_data, L7_FALSE);

  USL_DEBUG_PRINT("Deleted IPMC element, rv == %d\n", rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from IPMC Db
*
* @params   dbItem {(input)}  Pointer to db item to be deleted
*
* @returns  BCM error code
*
* @notes    For IPMC Db, we should never get into this case
*
* @end
*********************************************************************/
L7_int32 usl_delete_ipmc_port_db_elem_bcm(void *dbItem)
{
  int                        rv = BCM_E_NONE;
  usl_ipmc_port_db_elem_t   *pUslIpmcPortEntry = dbItem;
  bcm_vlan_vector_t          vlanVector;
  avlTree_t                 *ipmcDbHandle = L7_NULLPTR;
  usl_ipmc_db_elem_t        *pUslIpmcEntry, searchEntry;
  uslDbElemInfo_t            elemInfo;

  USL_IPMC_DB_LOCK_TAKE();

  USL_DEBUG_PRINT("Deleting IPMC Port element, modid %d, port %d\n", pUslIpmcPortEntry->bcmModid, pUslIpmcPortEntry->bcmPort);

  do
  {
    memset(vlanVector, 0, sizeof(bcm_vlan_vector_t));

    ipmcDbHandle = usl_ipmc_db_handle_get(USL_CURRENT_DB);
    if (ipmcDbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;
    }

    /* Need to iterate through all IPMC entries. */
    memset(&searchEntry, 0, sizeof(searchEntry));
    pUslIpmcEntry = avlSearchLVL7(ipmcDbHandle, &searchEntry, AVL_NEXT);
    while (pUslIpmcEntry != L7_NULL)
    {
      /* If this port is a member of this IPMC entry, set the replication info. */
      if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(pUslIpmcEntry->bcm_data.ipmc_index, pUslIpmcPortEntry->ipmcIndexMask))
      {
        rv = usl_bcm_ipmc_l3_port_repl_set(pUslIpmcPortEntry->bcmModid,
                                           pUslIpmcPortEntry->bcmPort,
                                           &pUslIpmcEntry->bcm_data,
                                           vlanVector);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
      }

      memcpy(&searchEntry, pUslIpmcEntry, sizeof(searchEntry));
      pUslIpmcEntry = avlSearchLVL7(ipmcDbHandle, &searchEntry, AVL_NEXT);
    }

    elemInfo.dbElem    = dbItem;
    elemInfo.elemIndex = 0; /* Not used */
    rv = usl_delete_ipmc_port_db_elem(USL_CURRENT_DB, elemInfo);

  } while (0);

  USL_DEBUG_PRINT("Deleted IPMC Port element, rv == %d\n", rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.
*
* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_ipmc_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  L7_int32                 rv              = BCM_E_NONE;
  usl_bcm_ipmc_addr_t     *shadowMcastAddr = &(((usl_ipmc_db_elem_t *)shadowDbItem)->bcm_data);
  usl_bcm_ipmc_addr_t     *operMcastAddr   = &(((usl_ipmc_db_elem_t *)operDbItem)->bcm_data);

  USL_IPMC_DB_LOCK_TAKE();

  USL_DEBUG_PRINT("Updating IPMC element\n");
  usl_db_ipmc_key_print((usl_ipmc_db_key_t *)shadowMcastAddr);

  do
  {
    /* If the Oper and Shadow hw indices are not same then
    ** the element cannot be reconciled. Return failure
    */
    if (shadowMcastAddr->ipmc_index != operMcastAddr->ipmc_index)
    {
      rv = BCM_E_FAIL;
      break;
    }
    else
    {
      /* Replace the entry regardless of whether the shadow value is different than the
         operational value. As part of the failover process, we disable RPF checking on
         all IPMC entries to minimize IPMC data loss due to RPF failures. Reconciliation
         is our chance to re-enable the RPF check for each entry. Take this opportunity
         to update any other fields that may have changed (e.g. L2 pbmp, L3 pbmp, RPF port). */
      rv = usl_bcm_ipmc_add(shadowMcastAddr, L7_TRUE);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;
      }
    }

  } while (0);

  USL_DEBUG_PRINT("Updated IPMC element, rv == %d\n", rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.
*
* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_ipmc_port_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  L7_int32                 rv                  = BCM_E_NONE;
  usl_ipmc_port_db_elem_t *shadowPortEntry     = shadowDbItem;
  usl_ipmc_port_db_elem_t *operPortEntry       = operDbItem;
  usl_ipmc_db_elem_t      *pUslIpmcEntry;
  L7_uint32                ipmcIndex;
  bcm_vlan_vector_t        shadowVlanVector, operVlanVector;
  usl_ipmc_port_db_repl_t *replInfo;

  USL_IPMC_DB_LOCK_TAKE();

  USL_DEBUG_PRINT("Updating IPMC Port element, modid %d, port %d\n", shadowPortEntry->bcmModid, shadowPortEntry->bcmPort);

  do
  {
    /* Compare the VLAN vectors for each IPMC index in the shadow entry and operational entry. */
    for (ipmcIndex = 0; ipmcIndex < platMrtrRoutesMaxEntriesGet(); ipmcIndex++)
    {
      memset(shadowVlanVector, 0, sizeof(bcm_vlan_vector_t));
      memset(operVlanVector, 0, sizeof(bcm_vlan_vector_t));

      if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, shadowPortEntry->ipmcIndexMask))
      {
        replInfo = shadowPortEntry->replInfo;
        while (replInfo != L7_NULLPTR)
        {
          if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, replInfo->ipmcIndexMask))
          {
            BCM_VLAN_VEC_SET(shadowVlanVector, replInfo->vlanId);
          }
          replInfo = replInfo->next;
        }
      }

      if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, operPortEntry->ipmcIndexMask))
      {
        replInfo = operPortEntry->replInfo;
        while (replInfo != L7_NULLPTR)
        {
          if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmcIndex, replInfo->ipmcIndexMask))
          {
            BCM_VLAN_VEC_SET(operVlanVector, replInfo->vlanId);
          }
          replInfo = replInfo->next;
        }
      }

      if (memcmp(shadowVlanVector,
                 operVlanVector,
                 sizeof(bcm_vlan_vector_t)) != 0)
      {
        pUslIpmcEntry = pUslIpMcastDataByIndex[ipmcIndex];
        if (pUslIpmcEntry != L7_NULLPTR)
        {
          rv = usl_bcm_ipmc_l3_port_repl_set(shadowPortEntry->bcmModid,
                                             shadowPortEntry->bcmPort,
                                             &pUslIpmcEntry->bcm_data,
                                             shadowVlanVector);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;
          }
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Mismatch between IPMC DB and Port DB, modid %d, port %d, ipmcIndex %d",
                  shadowPortEntry->bcmModid,
                  shadowPortEntry->bcmPort,
                  ipmcIndex);
          rv = BCM_E_FAIL;
          break;
        }
      }
    }
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      break;
    }

    /* Check if the VLAN and untagged info are different. */
    if ((shadowPortEntry->vlanId    != operPortEntry->vlanId) ||
        (shadowPortEntry->untagFlag != operPortEntry->untagFlag))
    {
      rv = usl_bcm_ipmc_egress_port_set(shadowPortEntry->bcmModid,
                                        shadowPortEntry->bcmPort,
                                        shadowPortEntry->vlanId,
                                        shadowPortEntry->untagFlag);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;
      }
    }

  } while (0);

  USL_DEBUG_PRINT("Updated IPMC Port element, rv == %d\n", rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Sets the VLAN replication configuration for a port in an
*          IPMC entry. If there are one or more VLANs replicated,
*          the port is added to the L3 pbmp of the IPMC entry.
*          If there are zero VLANs replicated, the port is
*          removed from the IPMC entry.
*
* @param   modid      {(input)} modid of L3 port
* @param   bcmPort    {(input)} BCM port
* @param   ipmcEntry  {(input)} IPMC entry
* @param   vlanVector {(input)} VLAN replication info for L3 port.
*
*
* @returns BCM Error Code
*
* @notes
*
* @end
*
*********************************************************************/
int usl_db_ipmc_l3_port_repl_set(L7_uint32 modid, L7_uint32 bcmPort, usl_bcm_ipmc_addr_t *ipmcEntry, bcm_vlan_vector_t vlanVector)
{
  usl_ipmc_db_key_t         key;
  usl_ipmc_db_elem_t       *pData;
  usl_ipmc_port_db_elem_t  *pPortData;
  int                       rv = BCM_E_NONE;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return rv;
  }

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    memset((void *)&key , 0, sizeof(key));

    memcpy(&(key.s_ip_addr), &(ipmcEntry->s_ip_addr), sizeof(key.s_ip_addr));
    memcpy(&(key.mc_ip_addr), &(ipmcEntry->mc_ip_addr), sizeof(key.mc_ip_addr));
    memcpy(&(key.vid), &(ipmcEntry->vid), sizeof(key.vid));

    USL_DEBUG_PRINT("Setting replication info for modid %d, port %d, ipmcIndex %d\n", modid, bcmPort, ipmcEntry->ipmc_index);
    usl_db_ipmc_key_print(&key);

    /* get the info in our AVL tree */
    pData = avlSearchLVL7(pUslIpMcastTreeHandle, &key, AVL_EXACT);
    pPortData = usl_db_ipmc_egress_port_ptr_get(modid, bcmPort);
    if (pPortData == L7_NULL)
    {
      rv = BCM_E_FAIL;
      break;
    }

    if (pData != L7_NULLPTR)
    {
      if (usl_db_ipmc_vlan_vector_is_empty(vlanVector))
      {
        USL_DEBUG_PRINT("  VLAN vector is empty.\n");
        /* Remove the port. */
        BCM_PBMP_PORT_REMOVE(pData->bcm_data.l3_pbmp[modid], bcmPort);
        USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(pData->bcm_data.ipmc_index, pPortData->ipmcIndexMask);
      }
      else
      {
        /* Add the port. */
        BCM_PBMP_PORT_ADD(pData->bcm_data.l3_pbmp[modid], bcmPort);
        USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET(pData->bcm_data.ipmc_index, pPortData->ipmcIndexMask);
      }

      /* Store the VLAN replication info for this port. */
      rv = usl_db_ipmc_repl_set(modid, bcmPort, ipmcEntry->ipmc_index, vlanVector);
    }
    else
    {
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);

  USL_DEBUG_PRINT("Set replication info, rv == %d\n", rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Sets the RPF check mode. This function should
*          be called on each stack unit
*          to disable RPF checking after a management failover occurs.
*          This function will iterate through all IPMC entries in the
*          operational DB and disable RPF checking. This will minimize
*          IPMC data loss during a failover.
*          This function should be called to enable RPF checking before
*          reconciliation occurs so that updated IPMC entries and newly
*          created IPMC entries have RPF checking enabled.
*
* @param   enable     {(input)} RPF checking mode
*
* @returns BCM Error Code
*
* @notes
*
* @end
*
*********************************************************************/
int usl_db_ipmc_rpf_check_mode_set(L7_BOOL enable)
{
  int rv = BCM_E_NONE;
  usl_ipmc_db_elem_t    *pUslIpmcEntry, searchEntry;
  avlTree_t             *dbHandle = L7_NULLPTR;

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    uslIpmcRpfModeEnable = enable;

    USL_DEBUG_PRINT("Setting RPF check mode %d\n", enable);

    if (uslIpmcRpfModeEnable == L7_FALSE)
    {
      dbHandle = usl_ipmc_db_handle_get(USL_OPERATIONAL_DB);
      if (dbHandle == L7_NULLPTR)
      {
        rv = BCM_E_FAIL;
        break;
      }

      memset(&searchEntry, 0, sizeof(searchEntry));
      pUslIpmcEntry = avlSearchLVL7(dbHandle, &searchEntry, AVL_NEXT);

      while (pUslIpmcEntry != L7_NULL)
      {
        memcpy(&searchEntry, pUslIpmcEntry, sizeof(searchEntry));

        /* The following call can be used to replace the existing entry
           and ensure that the RPF check is disabled for this entry. */
        rv = usl_bcm_ipmc_rpf_set(&pUslIpmcEntry->bcm_data);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }

        pUslIpmcEntry = avlSearchLVL7(dbHandle, &searchEntry, AVL_NEXT);
      }
    }

  } while (0);

  USL_DEBUG_PRINT("Set RPF check mode, rv == %d\n", rv);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Gets the RPF check mode.
*
* @param
*
* @returns L7_BOOL
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL usl_db_ipmc_rpf_check_mode_get()
{
  return uslIpmcRpfModeEnable;
}

/*********************************************************************
*
* @purpose Gets the USL info for an IPMC entry based on an IPMC index.
*
* @param   index         {(input)}  IPMC index
*          l7_ipmc       {{output}} IPMC entry data
*
* @returns BCM_E_xxx
*
* @notes
*
* @end
*
*********************************************************************/
int usl_db_ipmc_entry_get_by_index(L7_uint32 index, usl_bcm_ipmc_addr_t *l7_ipmc)
{
  int       rv = BCM_E_NOT_FOUND;
  L7_uint32 max;

  max = platMrtrRoutesMaxEntriesGet();

  USL_IPMC_DB_LOCK_TAKE();

  if (index < max)
  {
    if (pUslIpMcastDataByIndex[index] != L7_NULLPTR)
    {
      memcpy(l7_ipmc, pUslIpMcastDataByIndex[index], sizeof(usl_bcm_ipmc_addr_t));
      rv = BCM_E_NONE;
    }
  }

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

L7_RC_t usl_db_ipmc_wlan_l3_find_delete(L7_uint32 l3_intf_id, bcm_gport_t wlanvp,
                                    bcm_vlan_t vlan_id, bcm_if_t *egrintf,
                                    L7_uint32 *use_count, usl_bcm_l3_egress_t *egr,
                                    L7_int32 *self_owned)
{
  usl_ipmc_wlan_port_elem_t data;
  usl_ipmc_wlan_port_elem_t *pData;
  L7_RC_t rc = L7_FAILURE;

  memset((void *)&data, 0, sizeof(usl_ipmc_wlan_port_elem_t));
  data.intf = l3_intf_id;
  data.lport = wlanvp;
  data.vlan = vlan_id;

  pData = avlSearchLVL7(&uslIpMcastWlanPortTreeData, &data ,AVL_EXACT);

  if (pData != L7_NULLPTR)
  {
    pData->use_count--;
    memcpy(&data, pData, sizeof(usl_ipmc_wlan_port_elem_t));
    *egrintf = data.egrintf;
    *use_count = data.use_count;
    *self_owned = data.self_owned;
    memcpy(egr, &data.egr, sizeof(usl_bcm_l3_egress_t));
    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t usl_db_ipmc_wlan_l3_find_add(L7_uint32 l3_intf_id, bcm_gport_t wlanvp,
                                 bcm_vlan_t vlan_id, bcm_if_t *egrintf)
{
  usl_ipmc_wlan_port_elem_t data;
  usl_ipmc_wlan_port_elem_t *pData;
  L7_RC_t rc = L7_FAILURE;

  memset((void *)&data, 0, sizeof(usl_ipmc_wlan_port_elem_t));
  data.intf = l3_intf_id;
  data.lport = wlanvp;
  data.vlan = vlan_id;

  pData = avlSearchLVL7(&uslIpMcastWlanPortTreeData, &data ,AVL_EXACT);

  if (pData != L7_NULLPTR)
  {
    memcpy(&data, pData, sizeof(usl_ipmc_wlan_port_elem_t));
    *egrintf = data.egrintf;
    pData->use_count++;
    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t usl_db_ipmc_wlan_l3_insert(L7_uint32 mc_index, L7_uint32 l3_intf_id, bcm_gport_t wlanvp,
                                   bcm_vlan_t vlan_id, bcm_if_t egrintf,
                                   usl_bcm_l3_egress_t *egr, L7_int32 self_owned)
{
  usl_ipmc_wlan_port_elem_t data;
  usl_ipmc_wlan_port_elem_t *pData = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  memset((void *)&data, 0, sizeof(usl_ipmc_wlan_port_elem_t));
  data.intf = l3_intf_id;
  data.lport = wlanvp;
  data.vlan = vlan_id;
  data.egrintf = egrintf;
  data.mc_index = mc_index;
  data.use_count = 1;
  data.self_owned = self_owned;
  memcpy(&data.egr, egr, sizeof(data.egr));

  pData = avlInsertEntry(&uslIpMcastWlanPortTreeData, &data);

  if (pData == &data)
  {
    USL_LOG_ERROR("USL: Error adding egress object entry \n");
  }
  else if (pData == L7_NULLPTR)
  {
    /* object was inserted */

    /* Search the entry to find the item inserted */
    pData = avlSearchLVL7(&uslIpMcastWlanPortTreeData, &data, AVL_EXACT);
    if (L7_NULL == pData)
    {
      LOG_ERROR(wlanvp); /* Cannot find the inserted item */
    }

    memcpy(pData, &data, sizeof(usl_ipmc_wlan_port_elem_t));

    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t usl_db_ipmc_wlan_l3_delete(L7_uint32 l3_intf_id, bcm_gport_t wlanvp,
                               bcm_vlan_t vlan_id, bcm_if_t egrintf)
{
  usl_ipmc_wlan_port_elem_t data;
  usl_ipmc_wlan_port_elem_t *pData;
  L7_RC_t rc = L7_FAILURE;

  memset((void *)&data, 0, sizeof(usl_ipmc_wlan_port_elem_t));
  data.intf = l3_intf_id;
  data.lport = wlanvp;
  data.vlan = vlan_id;
  data.egrintf = egrintf;

  pData = avlDeleteEntry(&uslIpMcastWlanPortTreeData, &data);

  if (pData == L7_NULLPTR)
  {
    rc = L7_FAILURE;
  }
  else
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
*
* @purpose Cleans up the IPMC DB for failed units.
*
* @param   modids        {{input}}  array of modids
*          modid_count   {{input}}  count of modids in array
*
* @returns BCM_E_xxx
*
* @notes
*
* @end
*
*********************************************************************/
int usl_ipmc_db_dataplane_cleanup(L7_int32 *modids, L7_uint32 modid_count)
{
  int                     rv = BCM_E_NONE;
  usl_ipmc_db_elem_t      ipmcKey, *pData;
  usl_ipmc_port_db_elem_t ipmcPortKey, *pPortData;
  L7_uint32               i;
  uslDbElemInfo_t         elemInfo;

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    /* Iterate through all the IPMC entries, clearing the L2 and L3 pbmps
       for the affected modids. */
    memset(&ipmcKey, 0, sizeof(ipmcKey));

    pData = avlSearchLVL7(pUslIpMcastTreeHandle, &ipmcKey, AVL_NEXT);
    while (pData != L7_NULL)
    {
      memcpy(&ipmcKey, pData, sizeof(ipmcKey));

      for (i = 0; i < modid_count; i++)
      {
        BCM_PBMP_CLEAR(pData->bcm_data.l2_pbmp[modids[i]]);
        BCM_PBMP_CLEAR(pData->bcm_data.l2_ubmp[modids[i]]);
        BCM_PBMP_CLEAR(pData->bcm_data.l3_pbmp[modids[i]]);
      }

      pData = avlSearchLVL7(pUslIpMcastTreeHandle, &ipmcKey, AVL_NEXT);
    }

    /* Delete IPMC port DB entries for ports that are affected by the missing
       modids. */
    memset(&ipmcPortKey, 0, sizeof(ipmcPortKey));

    pPortData = avlSearchLVL7(pUslIpMcastPortTreeHandle, &ipmcPortKey, AVL_EXACT);
    if (pData == L7_NULLPTR)
    {
      pPortData = avlSearchLVL7(pUslIpMcastPortTreeHandle, &ipmcPortKey, AVL_NEXT);
    }
    while (pPortData != L7_NULL)
    {
      memcpy(&ipmcPortKey, pPortData, sizeof(ipmcPortKey));

      for (i = 0; i < modid_count; i++)
      {
        if (pPortData->bcmModid == modids[i])
        {
          elemInfo.dbElem    = pPortData;
          elemInfo.elemIndex = 0;

          rv = usl_delete_ipmc_port_db_elem(USL_CURRENT_DB, elemInfo);
          break;
        }
      }

      pPortData = avlSearchLVL7(pUslIpMcastPortTreeHandle, &ipmcPortKey, AVL_NEXT);
    }
  } while (0);

  USL_IPMC_DB_LOCK_GIVE();

  return rv;
}

void usl_db_ipmc_dump(L7_BOOL showSingleEntry, L7_int32 ipmcIndex)
{
  usl_ipmc_db_elem_t   key;
  usl_ipmc_db_elem_t  *pData;
  L7_uchar8            buffer[4096];
  L7_uint32            entryCount = 0;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return;
  }

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    memset((void *)&key , 0, sizeof(key));
    pData = avlSearchLVL7(pUslIpMcastTreeHandle, &key, AVL_NEXT);

    while (pData != L7_NULL)
    {
      if ((showSingleEntry == L7_FALSE) || (ipmcIndex == pData->bcm_data.ipmc_index))
      {
        entryCount++;
        usl_print_ipmc_db_elem(pData, buffer, sizeof(buffer));
        sysapiPrintf("%s", buffer);
        if (showSingleEntry == L7_TRUE)
        {
          break;
        }
        sysapiPrintf("==========================================\n");
      }

      memcpy(&key, pData, sizeof(key));
      pData = avlSearchLVL7(pUslIpMcastTreeHandle, &key, AVL_NEXT);
    }

  } while (0);

  USL_IPMC_DB_LOCK_GIVE();

  if (showSingleEntry == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Total number of entries in the table: %d\n",
                  entryCount);
  }

  return;
}

/*********************************************************************
* @purpose  Print the contents of a IPMC Db element in specified buffer
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
void usl_print_ipmc_wlan_port_db_elem(void *item, L7_uchar8 *buffer, L7_uint32 size)
{
  usl_ipmc_wlan_port_elem_t  *pUslIpmcPortEntry = item;

  osapiSnprintf(buffer, size, "intf: %d\n", pUslIpmcPortEntry->intf);
  osapiSnprintfcat(buffer, size, "wlan gport: %d\n", pUslIpmcPortEntry->lport);
  osapiSnprintfcat(buffer, size, "vlan: %d\n", pUslIpmcPortEntry->vlan);
  osapiSnprintfcat(buffer, size, "egrintf: %d\n", pUslIpmcPortEntry->egrintf);
  osapiSnprintfcat(buffer, size, "mc_index: %d\n", pUslIpmcPortEntry->mc_index);
  osapiSnprintfcat(buffer, size, "use_count: %d\n", pUslIpmcPortEntry->use_count);
  osapiSnprintfcat(buffer, size, "self_owned: %d\n", pUslIpmcPortEntry->self_owned);
  osapiSnprintfcat(buffer, size, "\n");

  return;
}

void usl_db_ipmc_wlan_port_dump(L7_BOOL showSingleEntry, L7_int32 ipmcIndex)
{
  usl_ipmc_db_elem_t   key;
  usl_ipmc_db_elem_t  *pData;
  L7_uchar8            buffer[4096];
  L7_uint32            entryCount = 0;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return;
  }

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    memset((void *)&key , 0, sizeof(key));
    pData = avlSearchLVL7(&uslIpMcastWlanPortTreeData, &key, AVL_NEXT);

    while (pData != L7_NULL)
    {
      if ((showSingleEntry == L7_FALSE) || (ipmcIndex == pData->bcm_data.ipmc_index))
      {
        entryCount++;
        usl_print_ipmc_wlan_port_db_elem(pData, buffer, sizeof(buffer));
        sysapiPrintf("%s", buffer);
        if (showSingleEntry == L7_TRUE)
        {
          break;
        }
        sysapiPrintf("==========================================\n");
      }

      memcpy(&key, pData, sizeof(key));
      pData = avlSearchLVL7(&uslIpMcastWlanPortTreeData, &key, AVL_NEXT);
    }

  } while (0);

  USL_IPMC_DB_LOCK_GIVE();

  if (showSingleEntry == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Total number of entries in the table: %d\n",
                  entryCount);
  }

  return;
}

void usl_db_ipmc_port_dump(L7_BOOL showVlanVector, L7_BOOL showSingleEntry, L7_int32 modid, L7_int32 bcmPort)
{
  usl_ipmc_port_db_elem_t   key;
  usl_ipmc_port_db_elem_t  *pPortData;
  L7_uchar8                 buffer[4096];
  L7_uint32                 max, i;
  bcm_vlan_vector_t         vlanVector;
  L7_uint32                 wordIndex, bitIndex;

  if (uslIpMcDbActive != L7_TRUE)
  {
    return;
  }

  max = platMrtrRoutesMaxEntriesGet();

  USL_IPMC_DB_LOCK_TAKE();

  do
  {
    memset((void *)&key , 0, sizeof(key));
    pPortData = avlSearchLVL7(pUslIpMcastPortTreeHandle, &key, AVL_EXACT);
    if (pPortData == L7_NULLPTR)
    {
      pPortData = avlSearchLVL7(pUslIpMcastPortTreeHandle, &key, AVL_NEXT);
    }

    while (pPortData != L7_NULL)
    {
      if ((showSingleEntry == L7_FALSE) || ((modid == pPortData->bcmModid) && (bcmPort == pPortData->bcmPort)))
      {
        usl_print_ipmc_port_db_elem(pPortData, buffer, sizeof(buffer));
        sysapiPrintf("%s", buffer);

        if (showVlanVector == L7_TRUE)
        {
          for (i = 0; i < max; i++)
          {
            if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(i, pPortData->ipmcIndexMask))
            {
              sysapiPrintf("IPMC index: %d\n", i);
              if (usl_db_ipmc_repl_get(pPortData->bcmModid, pPortData->bcmPort, i, vlanVector) == BCM_E_NONE)
              {
                sysapiPrintf("Replicated in VLANS: ");
                for (wordIndex = 0; wordIndex < (sizeof(bcm_vlan_vector_t) / sizeof(uint32)); wordIndex++)
                {
                  if (vlanVector[wordIndex] != 0)
                  {
                    for (bitIndex = 0; bitIndex < 32; bitIndex++)
                    {
                      if (vlanVector[wordIndex] & (1 << bitIndex))
                      {
                        sysapiPrintf("%d ", wordIndex * 32 + bitIndex);
                      }
                    }
                  }
                }
                sysapiPrintf("\n");
              }
            }
          }
        }

        if (showSingleEntry == L7_TRUE)
        {
          break;
        }
        sysapiPrintf("==========================================\n");
      }

      memcpy(&key, pPortData, sizeof(key));
      pPortData = avlSearchLVL7(pUslIpMcastPortTreeHandle, &key, AVL_NEXT);
    }

  } while (0);

  USL_IPMC_DB_LOCK_GIVE();

  return;
}
#endif /* L7_MCAST_PACKAGE */
