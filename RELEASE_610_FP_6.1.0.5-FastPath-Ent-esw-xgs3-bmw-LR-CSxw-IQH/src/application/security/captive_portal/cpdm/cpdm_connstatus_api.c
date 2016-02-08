#include <time.h>
#include "commdefs.h"
#include "datatypes.h"
#include "sysapi.h"
#include "default_cnfgr.h"
#include "l7_cnfgr_api.h"
#include "captive_portal_common.h"
#include "cpdm_connstatus_util.h"
#include "cpdm_connstatus_api.h"
#include "cpdm.h"
#include "cpim_api.h"
#include "cpdm_api.h"
#include "cp_trap_api.h"
#include "wdm_client_api.h"
#include "clustering_api.h"
#include "cp_cluster.h"
#include "cpdm_clustering_api.h"

extern cpdmOprData_t *cpdmOprData;

/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);  
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);  


static avlTree_t         cacStatusTree;
static L7_BOOL           cacStatusTreeReady = L7_FALSE;
static avlTreeTables_t * cacStatusTreeHeap  = L7_NULLPTR;
static void            * cacStatusDataHeap  = L7_NULLPTR;
static L7_uint32         currMaxCACStatus   = 0;

static avlTree_t         clientConnStatusTree;
static L7_BOOL           clientConnStatusTreeReady = L7_FALSE;
static avlTreeTables_t * clientConnStatusTreeHeap  = L7_NULLPTR;
static void            * clientConnStatusDataHeap  = L7_NULLPTR;
static L7_uint32         clientConnStatusTreeMaxSize = 0;

avlTree_t                peerSwClientTree;
avlTreeTables_t         *peerSwClientTreeHeap = L7_NULLPTR;
void                    *peerSwClientDataHeap = L7_NULLPTR;

avlTree_t                peerSwTree;
avlTreeTables_t         *peerSwTreeHeap = L7_NULLPTR;
void                    *peerSwDataHeap = L7_NULLPTR;

static avlTree_t         intfStatusTree;
static L7_BOOL           intfStatusTreeReady = L7_FALSE;
static avlTreeTables_t * intfStatusTreeHeap  = L7_NULLPTR;
static void            * intfStatusDataHeap  = L7_NULLPTR;
static L7_uint32         currMaxIntfStatus   = 0;

static avlTree_t         actStatusTree;
static L7_BOOL           actStatusTreeReady = L7_FALSE;
static avlTreeTables_t * actStatusTreeHeap  = L7_NULLPTR;
static void            * actStatusDataHeap  = L7_NULLPTR;
static L7_uint32         currMaxActStatus   = 0;

static avlTree_t         intfConnTree;
static L7_BOOL           intfConnTreeReady = L7_FALSE;
static avlTreeTables_t * intfConnTreeHeap  = L7_NULLPTR;
static void            * intfConnDataHeap  = L7_NULLPTR;


typedef L7_BOOL (*conn_del_pred)(cpdmClientConnStatusData_t *,
                                 void *);

static 
void clientConnTablesAllDeleteHelp(L7_enetMacAddr_t mac,
                                   conn_del_pred    pred,
                                   void             *pred_data);


static
L7_RC_t cpdmClientConnStatusIPMatchHelper(L7_IP_ADDR_t  ip,
            L7_enetMacAddr_t * macAddr);

static
L7_RC_t clientConnStatusDeleteHelper(L7_enetMacAddr_t * mac, 
                                     L7_BOOL doDeauth);

extern cpdmConfigData_t *cpdmFindConfig(cpId_t cpId, L7_BOOL getNext);

/*********************************************************************
*
*  ACTIVITY STATUS API FUNCTIONS
*
*********************************************************************/

/*********************************************************************
*
* @purpose  Initialize our Activity Status  tree
*
* @param    L7_uint32  maxActStatus  @b{(input)} max entries to support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmActStatusInit(L7_uint32 maxActStatus)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    actStatusTreeHeap = (avlTreeTables_t *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxActStatus * sizeof(avlTreeTables_t));
    if (L7_NULLPTR == actStatusTreeHeap)
    {
       break; /* no point in continuing */
    }
    actStatusDataHeap = (void *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxActStatus * sizeof(cpdmActivityStatusData_t));
    if (L7_NULLPTR == actStatusDataHeap)
    {
       break; /* no point in continuing */
    }
    avlCreateAvlTree(&actStatusTree, 
         actStatusTreeHeap, 
         actStatusDataHeap,
                     maxActStatus, sizeof(cpdmActivityStatusData_t),
                     0,  sizeof(cpId_t));
    actStatusTreeReady = L7_TRUE;
    currMaxActStatus = maxActStatus;
    rc = L7_SUCCESS;
  } while(0);

  if (L7_SUCCESS != rc)
  {
    if ( (L7_NULLPTR == actStatusDataHeap) ||
         (L7_NULLPTR == actStatusTreeHeap)) 
    {
      LOG_ERROR(0); /* no point in continuing */
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Finalize our activity-status tree (remove all entries)
*
* @parms    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmActStatusFinalize(void)
{
  SEMA_TAKE(WRITE);   
  avlPurgeAvlTree(&actStatusTree, currMaxActStatus);
  SEMA_GIVE(WRITE);   
}

/*********************************************************************
*
* @purpose  Find a tree entry with the given CP id as a key
*
* @param    cpId  cpId   @b{(input)} the key, of course
*
* @returns  void  pointer to cpdmActivityStatusData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
static 
cpdmActivityStatusData_t *
cpdmFindActStatus(cpId_t cpId)
{
  if (!actStatusTreeReady)
  {
    return L7_NULLPTR;
  }
  return avlSearchLVL7(&actStatusTree, &cpId, AVL_EXACT);   
}

/*********************************************************************
*
* @purpose  Add status entry to intf assoc status table
*           progress table
*
* @param    cpId_t        cpId  @b{(input)} captive portal ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmActStatusAdd(cpId_t cpId)
{
  cpdmActivityStatusData_t entry, *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady)
    return L7_FAILURE;

  entry.cpId      = cpId;

  if (L7_SUCCESS!=cpdmCPConfigOperStatusGet(cpId,&entry.status))
  {
    entry.status = L7_CP_INST_OPER_STATUS_ENABLED;
  }
  if (L7_SUCCESS!=cpdmCPConfigBlockedStatusGet(cpId,&entry.blocked))
  {
    entry.blocked = L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED;
  }
  if (L7_SUCCESS!=cpdmCPConfigDisableReasonGet(cpId,&entry.reason))
  {
    entry.reason = L7_CP_INST_DISABLE_REASON_NONE;
  }
  if (L7_SUCCESS!=cpdmCPConfigAuthenticatedUsersGet(cpId,&entry.users))
  {
    entry.users = 0;
  }

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = avlInsertEntry(&actStatusTree, &entry);
    if (L7_NULLPTR != pEntry) /* if item not inserted */
      break;                  /* ..then die           */
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   
  return rc;
}


/*********************************************************************
*
* @purpose  Delete activation status entry
*
* @param    cpId_t      cpId  @b{(input)} CP ID to wipe
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmActStatusDelete(cpId_t cpId)
{
  cpdmActivityStatusData_t entry, *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady)
  {
    return L7_FAILURE;
  }
  entry.cpId = cpId;
  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = avlSearchLVL7(&actStatusTree, &entry, AVL_EXACT);
    if (L7_NULLPTR != pEntry) 
    {
      pEntry = avlDeleteEntry(&actStatusTree, &entry);
      if (L7_NULLPTR == pEntry) 
  break;                  
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   
  return rc;
}

/*********************************************************************
*
* @purpose  Check for existing id in the activity status tree
*
* @param    cpId_t  cpId  @b{(input)} CP ID to  check
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmActStatusGet(cpId_t cpId)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;

  if (!actStatusTreeReady)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);   
  pEntry = cpdmFindActStatus(cpId);
  SEMA_GIVE(READ);   
  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Check for next CP ID in our activity status tree
*
* @param    cpid_t cpId       @b{(input)} CP ID
* @param    cpid_t *nextCpId  @b{(output)} next CP ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmActStatusNextGet(cpId_t cpId, cpId_t * nextCpId)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;

  if (!actStatusTreeReady || !nextCpId)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);   
  do 
  {
    pEntry = avlSearchLVL7(&actStatusTree, &cpId, AVL_NEXT);   
    if (L7_NULLPTR == pEntry) 
      break;
    *nextCpId = pEntry->cpId;
  } while(0);
  SEMA_GIVE(READ);   
  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Create activation & activity status entries for CP instances
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void cpdmActStatusUpdate(void)
{
  cpId_t cpId = 0;

  while (L7_SUCCESS==cpdmCPConfigNextGet(cpId,&cpId))
  {
    cpdmActStatusAdd(cpId);
  }
}

/*********************************************************************
*
* @purpose  Find the number of enabled/active CP instances
*
* @param    L7_uint32 * pCount @b{(output)} count of active instances
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An active CP instance is determined by the following;
*           - Global CP mode must be enbled
*           - The particular CP instance must be enabled
*           - The particular CP instance must have at least one
*             assigned interface (regardless of block status)
*
* @end
*
*********************************************************************/
L7_RC_t cpdmActStatusOperEnabledCount(L7_uint32 * pCount)
{
  L7_uint32 intIfNum = (L7_uint32) 0;
  cpId_t next_cpId = 0;
  cpId_t cpId = 0;
  L7_CP_INST_OPER_STATUS_t status;
  L7_BOOL enabled = L7_FALSE;

  if (L7_NULLPTR == pCount)
  {
    return L7_FAILURE;
  }

  *pCount = 0;
  while (L7_SUCCESS==cpdmCPConfigNextGet(cpId,&cpId))
  {
    if (L7_SUCCESS != cpdmCPConfigOperStatusGet(cpId, &status))
    {
      LOG_MSG("Failed to get operational status for cpId:%d",cpId);
      return L7_FAILURE;
    }
    if (L7_CP_INST_OPER_STATUS_ENABLED == status)
    {
      if (L7_SUCCESS != cpdmCPConfigIntIfNumNextGet(cpId, 0, &next_cpId, &intIfNum))
      {
        LOG_MSG("Failed to get IntIfNum for cpId:%d",cpId);
        return L7_FAILURE;
      }
      if (cpId == next_cpId)
      {
        if (L7_SUCCESS != cpdmIntfStatusGetIsEnabled(intIfNum, &enabled))
        {
          LOG_MSG("Failed to get operational status for intIfNum:%d",intIfNum);
          return L7_FAILURE;
        }
        if (enabled == L7_TRUE)
        {
          *pCount += 1;
        }
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve enabled status for CP
*
* @param    cpId_t cpId    @b{(input)} interface number
* @param    L7_BOOL * isEnabled @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigOperStatusGet(cpId_t cpId, L7_CP_INST_OPER_STATUS_t *status)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady || !status)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpdmFindActStatus(cpId);
    if (L7_NULLPTR == pEntry)
      break;
    *status = pEntry->status;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Get the authentication status of a CP instance.
*
* @param    cpid_t                    cpid    @b{(input)} CP config id
* @param    L7_CP_INST_BLOCK_STATUS_t *status @b{(output)} blocked status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigBlockedStatusGet(cpId_t cpId, L7_CP_INST_BLOCK_STATUS_t *blocked)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady || !blocked)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpdmFindActStatus(cpId);
    if (L7_NULLPTR == pEntry)
      break;
    *blocked = pEntry->blocked;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve reason for disabling of CP instance
*
* @param    cpId_t cpId    @b{(input)} interface number
* @param    L7_CP_INST_DISABLE_REASON_t * why @b{(output)} reason for
*                                   disabling of CP instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigDisableReasonGet(cpId_t cpId, 
             L7_CP_INST_DISABLE_REASON_t *reason)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady || !reason)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpdmFindActStatus(cpId);
    if (L7_NULLPTR == pEntry)
      break;
    *reason = pEntry->reason;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Get the number of users authenticated to a CP instance.
*
* @param    cpid_t    cpid       @b{(input)} CP config id
* @param    L7_uint32 *authUsers @b{(output)} number of authenticated users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigAuthenticatedUsersGet(cpId_t cpId, L7_uint32 *authUsers)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpdmFindActStatus(cpId);
    if (L7_NULLPTR == pEntry)
      break;
    *authUsers = pEntry->users;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}



/*********************************************************************
*
* @purpose  Set operational status for CP ID
*
* @param    cpId_t cpId    @b{(input)} CP ID
* @param    L7_CP_INST_OPER_STATUS_t   status @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigOperStatusSet(cpId_t cpId, L7_CP_INST_OPER_STATUS_t status)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpdmFindActStatus(cpId);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->status = status;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
  {
    cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
    if (cpClusterFlagSet(CP_CLUSTER_INST_STATUS_SEND) != L7_SUCCESS)
    {
      LOG_MSG("cpdmCPConfigOperStatusSet: Failed to set instance status send flag.\n");
      cpdmOprData->cpLocalSw.cpId = 0;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Set operational status for ALL CP IDs
*
* @param    cpId_t cpId    @b{(input)} CP ID
* @param    L7_CP_INST_OPER_STATUS_t   status @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigOperStatusAllSet(L7_CP_INST_OPER_STATUS_t status)
{
  cpId_t cpId = 0;

  while (L7_SUCCESS==cpdmCPConfigNextGet(cpId,&cpId))
  {
    if (L7_SUCCESS != cpdmCPConfigOperStatusSet(cpId, status))
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set blocked status for CP ID
*
* @param    cpId_t cpId    @b{(input)} CP ID
* @param    L7_CP_INST_BLOCK_STATUS_t   status @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigBlockedStatusSet(cpId_t cpId, L7_CP_INST_BLOCK_STATUS_t blocked)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpdmFindActStatus(cpId);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->blocked = blocked;
    rc = L7_SUCCESS;

    cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
    if (cpClusterFlagSet(CP_CLUSTER_INST_BLOCK_STATUS_SEND) != L7_SUCCESS)
    {
      LOG_MSG("cpdmCPConfigBlockedStatusSet: Failed to set instance block status send flag.\n");
      cpdmOprData->cpLocalSw.cpId = 0;
    }

  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}


/*********************************************************************
*
* @purpose  Set reason for disabling of CP instance
*
* @param    cpId_t cpId    @b{(input)} CP instance ID
* @param    L7_CP_WHY_DISABLED_t  why @b{(input)} reason for
*                                    disabling of CP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigDisableReasonSet(cpId_t cpId, 
             L7_CP_INST_DISABLE_REASON_t reason)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpdmFindActStatus(cpId);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->reason = reason;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}


/*********************************************************************
*
* @purpose  Retrieve number of users of CP instance
*
* @param    cpId_t cpId  @b{(input)} CP ID
* @param    L7_uint32 * users @b{(output)} number of users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigAuthenticatedUsersSet(cpId_t cpId, L7_uint32 authUsers)
{
  cpdmActivityStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!actStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpdmFindActStatus(cpId);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->users = authUsers;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
  {
    cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
    if (cpClusterFlagSet(CP_CLUSTER_INST_STATUS_SEND) != L7_SUCCESS)
    {
      LOG_MSG("cpdmCPConfigAuthenticatedUsersSet: Failed to set instance status send flag.\n");
      cpdmOprData->cpLocalSw.cpId = 0;
    }
  }

  return rc;
}



/*********************************************************************
*
*  INTERFACE STATUS API FUNCTIONS
*
*********************************************************************/

/*********************************************************************
*
* @purpose  Initialize our IntfStatus  tree
*
* @param    L7_uint32  maxIntfStatus  @b{(input)} max entries to support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusInit(L7_uint32 maxIntfStatus)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    intfStatusTreeHeap = (avlTreeTables_t *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxIntfStatus * sizeof(avlTreeTables_t));
    if (L7_NULLPTR == intfStatusTreeHeap)
    {
       break; /* no point in continuing */
    }
    intfStatusDataHeap = (void *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxIntfStatus * sizeof(cpdmIntfStatusData_t));
    if (L7_NULLPTR == intfStatusDataHeap)
    {
       break; /* no point in continuing */
    }
    avlCreateAvlTree(&intfStatusTree, intfStatusTreeHeap, 
                     intfStatusDataHeap,
                     maxIntfStatus, sizeof(cpdmIntfStatusData_t),
                     0,  sizeof(L7_uint32));
    intfStatusTreeReady = L7_TRUE;
    currMaxIntfStatus = maxIntfStatus;
    rc = L7_SUCCESS;
  } while(0);

  if (L7_SUCCESS != rc)
  {
    if ( (L7_NULLPTR == intfStatusDataHeap) ||
         (L7_NULLPTR == intfStatusTreeHeap)) 
    {
      LOG_ERROR(0); /* no point in continuing */
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Finalize our intf-activation-status tree
*
* @parms    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmIntfStatusFinalize(void)
{
  SEMA_TAKE(WRITE);   
  avlPurgeAvlTree(&intfStatusTree, currMaxIntfStatus);   
  SEMA_GIVE(WRITE);   
}


/*********************************************************************
*
* @purpose  Find a tree entry with the given intf id as a key
*
* @param    L7_uint32  intfId   @b{(input)} the key, of course
*
* @returns  void  pointer to cpdmIntfStatusData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
static 
cpdmIntfStatusData_t *
cpdmFindIntfStatus(L7_uint32 intfId)
{
  if (!intfStatusTreeReady)
  {
    return L7_NULLPTR;
  }
  return avlSearchLVL7(&intfStatusTree, &intfId, AVL_EXACT);   
}

/*********************************************************************
*
* @purpose  Add status entry to intf assoc status table
*           progress table
*
* @param    L7_uint32         intfId  @b{(input)} internal I/F ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusAdd(L7_uint32   intfId)
{
  cpdmIntfStatusData_t entry, *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!intfStatusTreeReady)
    return L7_FAILURE;

  entry.intfId    = intfId;
  entry.isEnabled = L7_FALSE;
  entry.why       = L7_INTF_WHY_DISABLED_ADMIN; 
  entry.users     = 0;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = avlInsertEntry(&intfStatusTree, &entry);
    if (L7_NULLPTR != pEntry) /* if item not inserted */
      break;                  /* ..then die           */
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   
  return rc;
}


/*********************************************************************
*
* @purpose  Delete  interface status table entry
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusDelete(L7_uint32 intfId)
{
  cpdmIntfStatusData_t entry, *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!intfStatusTreeReady)
  {
    return L7_FAILURE;
  }
  entry.intfId = intfId;
  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = avlSearchLVL7(&intfStatusTree, &entry, AVL_EXACT);
    if (L7_NULLPTR != pEntry) 
    {
      pEntry = avlDeleteEntry(&intfStatusTree, &entry);
      if (L7_NULLPTR == pEntry) 
  break;                  
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Check for existing inteface in the intf status tree
*
* @param    L7_uint32  intfId  @b{(input)} interface ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusGet(L7_uint32 intfId)
{
  cpdmIntfStatusData_t *pEntry = L7_NULLPTR;

  if (!intfStatusTreeReady)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);   
  pEntry = cpdmFindIntfStatus(intfId);
  SEMA_GIVE(READ);   
  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Check for existing interface in our status tree
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusNextGet(L7_uint32 intfId, L7_uint32 * nextIntfId)
{
  cpdmIntfStatusData_t *pEntry = L7_NULLPTR;

  if (!intfStatusTreeReady || !nextIntfId)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);   
  do 
  {
    pEntry = avlSearchLVL7(&intfStatusTree, &intfId, AVL_NEXT);   
    if (L7_NULLPTR == pEntry) 
      break;
    *nextIntfId = pEntry->intfId;
  } while(0);
  SEMA_GIVE(READ);   
  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Retrieve enabled status for intf
*
* @param    L7_uint32 intfId    @b{(input)} interface number
* @param    L7_BOOL * isEnabled @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusGetIsEnabled(L7_uint32 intfId, L7_BOOL * isEnabled)
{
  cpdmIntfStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!intfStatusTreeReady || !isEnabled)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpdmFindIntfStatus(intfId);
    if (L7_NULLPTR == pEntry)
      break;
    *isEnabled = pEntry->isEnabled;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}


/*********************************************************************
*
* @purpose  Retrieve reason for disabling of intf
*
* @param    L7_uint32 intfId    @b{(input)} interface number
* @param    L7_INTF_WHY_DISABLED_t * why @b{(output)} reason for
*                               disabling of I/F
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusGetWhy(L7_uint32 intfId, L7_INTF_WHY_DISABLED_t * why)
{
  cpdmIntfStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!intfStatusTreeReady || !why)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpdmFindIntfStatus(intfId);
    if (L7_NULLPTR == pEntry)
      break;
    *why = pEntry->why;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}


/*********************************************************************
*
* @purpose  Retrieve number of users of intf
*
* @param    L7_uint32 intfId  @b{(input)} interface number
* @param    L7_uint32 * users @b{(output)} number of users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusGetUsers(L7_uint32 intfId, L7_uint32 * users)
{
  cpdmIntfStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!intfStatusTreeReady)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpdmFindIntfStatus(intfId);
    if (L7_NULLPTR == pEntry)
      break;
    *users = pEntry->users;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}



/*********************************************************************
*
* @purpose  Set enabled status for intf
*
* @param    L7_uint32 intfId    @b{(input)} interface number
* @param    L7_BOOL   isEnabled @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusSetIsEnabled(L7_uint32 intfId, L7_BOOL isEnabled)
{
  cpdmIntfStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  cpId_t cpId;

  if (!intfStatusTreeReady)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpdmFindIntfStatus(intfId);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->isEnabled = isEnabled;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
  {
    /* get CP ID associated to this interface */
    if (cpdmCPConfigIntIfNumFind(intfId, &cpId) == L7_SUCCESS)
    {
      cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
    }
    cpdmOprData->cpLocalSw.intf = intfId;
    if (cpClusterFlagSet(CP_CLUSTER_INST_INTF_STATUS_SEND) != L7_SUCCESS)
    {
      LOG_MSG("cpdmIntfStatusSetIsEnabled: Failed to set instance intf status send flag.\n");
      cpdmOprData->cpLocalSw.cpId = 0;
      cpdmOprData->cpLocalSw.intf = 0;
    }
  }

  return rc;
}


/*********************************************************************
*
* @purpose  Set reason for disabling of intf
*
* @param    L7_uint32 intfId    @b{(input)} interface number
* @param    L7_INTF_WHY_DISABLED_t  why @b{(input)} reason for
*                                    disabling of I/F
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusSetWhy(L7_uint32 intfId, L7_INTF_WHY_DISABLED_t why)
{
  cpdmIntfStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!intfStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpdmFindIntfStatus(intfId);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->why = why;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}


/*********************************************************************
*
* @purpose  Retrieve number of users of intf
*
* @param    L7_uint32 intfId  @b{(input)} interface number
* @param    L7_uint32 * users @b{(output)} number of users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusSetUsers(L7_uint32 intfId, L7_uint32  users)
{
  cpdmIntfStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  cpId_t cpId;

  if (!intfStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpdmFindIntfStatus(intfId);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->users = users;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
  {
    /* get CP ID associated to this interface */
    if (cpdmCPConfigIntIfNumFind(intfId, &cpId) == L7_SUCCESS)
    {
      cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
    }
    cpdmOprData->cpLocalSw.intf = intfId;
    if (cpClusterFlagSet(CP_CLUSTER_INST_INTF_STATUS_SEND) != L7_SUCCESS)
    {
      LOG_MSG("cpdmIntfStatusSetUsers: Failed to set instance intf status send flag.\n");
      cpdmOprData->cpLocalSw.cpId = 0;
      cpdmOprData->cpLocalSw.intf = 0;
    }
  }

  return rc;
}






/*********************************************************************
*
*  CLIENT ASSOCIATION CONNECTION STATUS DB FUNCTIONS
*
*********************************************************************/

/*********************************************************************
*
* @purpose  Initialize our CACStatus  tree
*
* @param    L7_uint32  maxCACStatus @b{(input)} max entries to support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCACStatusInit(L7_uint32 maxCACStatus)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    cacStatusTreeHeap = (avlTreeTables_t *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxCACStatus * sizeof(avlTreeTables_t));
    if (L7_NULLPTR == cacStatusTreeHeap)
    {
       break; /* no point in continuing */
    }
    cacStatusDataHeap = (void *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxCACStatus * sizeof(cpdmClientAssocConnStatusData_t));
    if (L7_NULLPTR == cacStatusDataHeap)
    {
       break; /* no point in continuing */
    }
    avlCreateAvlTree(&cacStatusTree, cacStatusTreeHeap, cacStatusDataHeap,
                     maxCACStatus, sizeof(cpdmClientAssocConnStatusData_t),
                     0,  sizeof(CPMACPair_t));
    cacStatusTreeReady = L7_TRUE;
    currMaxCACStatus = maxCACStatus;
    rc = L7_SUCCESS;
  } while(0);

  if (L7_SUCCESS != rc)
  {
    if ( (L7_NULLPTR == cacStatusDataHeap) ||
         (L7_NULLPTR == cacStatusTreeHeap)) 
    {
      LOG_ERROR(0); /* no point in continuing */
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Finalize our interface association conn status tree
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmCACStatusFinalize(void)
{
  SEMA_TAKE(WRITE);   
  avlPurgeAvlTree(&cacStatusTree, currMaxCACStatus);
  SEMA_GIVE(WRITE);   
}


/*********************************************************************
*
* @purpose  Find a tree entry with the given intf id as a key
*
* @param    L7_uint32  intfId   @b{(input)} the key, of course
*
* @returns  void  pointer to cpdmClientAssocConnStatusData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
static 
cpdmClientAssocConnStatusData_t * 
  cpdmFindCACStatus(cpId_t cpId, L7_enetMacAddr_t * mac)
{
  cpdmClientAssocConnStatusData_t cac;
 
  cac.assoc.cpId = cpId;
  memcpy(&(cac.assoc.macAddr), mac, sizeof(L7_enetMacAddr_t));
  cac.avlPtr = L7_NULLPTR;
  return avlSearchLVL7(&cacStatusTree, &cac, AVL_EXACT);   
}



/*********************************************************************
*
* @purpose  Check for existing CP instance/client MAC association
*
* @param  cpId_t           cpId @b{(input)} CP instance to find
* @param  L7_enetMacAddr_t *mac @b{(input)} MAC address to find
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConnStatusGet(cpId_t cpId, L7_enetMacAddr_t * mac)
{
  cpdmClientAssocConnStatusData_t *pEntry = L7_NULLPTR;

  if (!cacStatusTreeReady || (L7_NULLPTR == mac))
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);   
  pEntry = cpdmFindCACStatus(cpId, mac);
  SEMA_GIVE(READ);   
  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}


/*********************************************************************
*
* @purpose  Find NEXT association of CP instance and client MAC
*
* @param  cpId_t           cpId @b{(input)} CP instance to find
* @param  L7_enetMacAddr_t *mac @b{(input)} MAC address to find
* @param  cpId_t           *pCpId @b{(output)} next CP instance
* @param  L7_enetMacAddr_t *pMmac @b{(output)} next MAC address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConnStatusNextGet(cpId_t cpId, 
                                L7_enetMacAddr_t * mac,
                                cpId_t * pCpId,
                                L7_enetMacAddr_t * pMac)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmClientAssocConnStatusData_t entry, *pEntry = L7_NULLPTR;

  if ((!cacStatusTreeReady) || (L7_NULLPTR == pCpId) ||
      (L7_NULLPTR == pCpId) || (L7_NULLPTR == pMac))
    return L7_FAILURE;
  entry.assoc.cpId = cpId;
  memcpy(&(entry.assoc.macAddr), mac, sizeof(L7_enetMacAddr_t));

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = avlSearchLVL7(&cacStatusTree, &entry, AVL_NEXT);   
    if (L7_NULLPTR == pEntry) 
      break;
    *pCpId = pEntry->assoc.cpId;
    memcpy(pMac, &(pEntry->assoc.macAddr), sizeof(L7_enetMacAddr_t));
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   
  return  rc;
}



/*********************************************************************
*
*  CLIENT CONNECTION STATUS DB FUNCTIONS
*
*********************************************************************/

/*********************************************************************
*
* @purpose  Initialize our ClientConnectionStatus  tree
*
* @param    L7_uint32  maxClientConnectionStatus  @b{(input)} max entries to support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

static
int ifNumMACPair_cmp(void * pair1, void * pair2, size_t ignored)
{
  ifNumMACPair_t * p1 = (ifNumMACPair_t *) pair1;
  ifNumMACPair_t * p2 = (ifNumMACPair_t *) pair2;

  if (p1->intIfNum != p2->intIfNum)
    return (p1->intIfNum - p2->intIfNum);
  return memcmp(&(p1->macAddr), &(p2->macAddr), sizeof(L7_enetMacAddr_t));
}

static
int mac_cmp(void * mac1, void * mac2, size_t ignored)
{
  L7_enetMacAddr_t * m1 = (L7_enetMacAddr_t *) mac1;
  L7_enetMacAddr_t * m2 = (L7_enetMacAddr_t *) mac2;

  return memcmp(m1, m2, sizeof(L7_enetMacAddr_t));
}


L7_RC_t cpdmClientConnStatusInit(L7_uint32 maxClientConnStatus)
{
  L7_RC_t rc = L7_FAILURE;

  
  do
  {
    clientConnStatusTreeHeap = (avlTreeTables_t *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxClientConnStatus * sizeof(avlTreeTables_t));
    if (L7_NULLPTR == clientConnStatusTreeHeap)
    {
       break; /* no point in continuing */
    }
    clientConnStatusDataHeap = (void *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxClientConnStatus * 
                  sizeof(cpdmClientConnStatusData_t));
    if (L7_NULLPTR == clientConnStatusDataHeap)
    {
       break; /* no point in continuing */
    }
    avlCreateAvlTree(&clientConnStatusTree, 
                     clientConnStatusTreeHeap, 
                     clientConnStatusDataHeap,
                     maxClientConnStatus, 
                     sizeof(cpdmClientConnStatusData_t),
                     0,  sizeof(L7_enetMacAddr_t));

    (void) avlSetAvlTreeComparator(&clientConnStatusTree, 
                                   (avlComparator_t) mac_cmp);

    clientConnStatusTreeMaxSize = maxClientConnStatus;
    clientConnStatusTreeReady = L7_TRUE;


    /* check if clustering is supported on the switch */
    if (clusterSupportGet() == L7_SUCCESS)
    {
      /* allocate memory for peer switch client data structure */
      peerSwClientTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                                            sizeof(avlTreeTables_t)*(CP_CLIENT_CONN_STATUS_MAX*2));
      peerSwClientDataHeap = (void *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                                 sizeof(cpdmPeerSwClientStatus_t)*(CP_CLIENT_CONN_STATUS_MAX*2));
      if ((peerSwClientTreeHeap == L7_NULLPTR) || (peerSwClientDataHeap == L7_NULLPTR))
      {
        break;
      }
      avlCreateAvlTree(&peerSwClientTree, peerSwClientTreeHeap, peerSwClientDataHeap,
                       CP_CLIENT_CONN_STATUS_MAX*2,
                       sizeof(cpdmPeerSwClientStatus_t), 0, sizeof(L7_enetMacAddr_t));
      (void)avlSetAvlTreeComparator(&peerSwClientTree, (avlComparator_t)mac_cmp);


      /* allocate memory and initialize tables for other switches in cluster */
      peerSwTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                                      sizeof(avlTreeTables_t)*(clusterMaxMembersGet()-1));
      peerSwDataHeap = (void *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                           sizeof(cpdmPeerSwStatus_t)*(clusterMaxMembersGet()-1));
      if ((peerSwTreeHeap == L7_NULLPTR) || (peerSwDataHeap == L7_NULLPTR))
      {
        break;
      }
      avlCreateAvlTree(&peerSwTree, peerSwTreeHeap, peerSwDataHeap,
                       clusterMaxMembersGet()-1,
                       sizeof(cpdmPeerSwStatus_t), 0, sizeof(L7_enetMacAddr_t));
      (void)avlSetAvlTreeComparator(&peerSwTree, (avlComparator_t)mac_cmp);
    }


    intfConnTreeHeap = (avlTreeTables_t *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxClientConnStatus * sizeof(avlTreeTables_t));
    if (L7_NULLPTR == intfConnTreeHeap)
    {
       break; /* no point in continuing */
    }
    intfConnDataHeap = (void *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxClientConnStatus * 
                  sizeof(cpdmIntfClientConnStatusData_t));
    if (L7_NULLPTR == intfConnDataHeap)
    {
       break; /* no point in continuing */
    }
    avlCreateAvlTree(&intfConnTree, intfConnTreeHeap,
                     intfConnDataHeap, maxClientConnStatus, 
                     sizeof(cpdmIntfClientConnStatusData_t),
                     0,  sizeof(ifNumMACPair_t));
    (void) avlSetAvlTreeComparator(&intfConnTree, 
                                   (avlComparator_t) ifNumMACPair_cmp);
    intfConnTreeReady = L7_TRUE;

    if (L7_SUCCESS != connTransQInit(CP_CLIENT_CONN_STATUS_MAX * 
                                     MAX_CP_CONN_TRANS_TYPES))
    {
      break;
    }

    rc = L7_SUCCESS;
  } while(0);

  if (L7_SUCCESS != rc)
  {
    if ( (L7_NULLPTR == clientConnStatusDataHeap) ||
         (L7_NULLPTR == clientConnStatusTreeHeap)) 
    {
      LOG_ERROR(0); /* no point in continuing */
    }

    if ( (L7_NULLPTR == intfConnDataHeap) ||
         (L7_NULLPTR == intfConnTreeHeap)) 
    {
      LOG_ERROR(0); /* no point in continuing */
    }
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Find a tree entry with the given intf id as a key
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
*
* @returns  void  pointer to cpdmClientConnStatusData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
static 
cpdmClientConnStatusData_t * 
cpdmFindClientConnStatus(L7_enetMacAddr_t * macAddr)
{
  if (!clientConnStatusTreeReady || !macAddr)
  {
    return L7_NULLPTR;
  }
  return avlSearchLVL7(&clientConnStatusTree, macAddr, AVL_EXACT);   
}


/*********************************************************************
*
* @purpose  Find next connection transaction type, if it exists
*
* @param    cpConnTransactionType_t * pType  @b{(output)} trans type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new transaction available,
*           or if parameter is garbage. Does NOT remove transaction
*           from queue.
* @end
*
*********************************************************************/
L7_RC_t
cpdmClientConnStatusNextTransTypeGet(L7_uint32 * pType)
{
  cpConnTransaction_t trans;
  L7_RC_t rc = L7_FAILURE;

  do 
  {
    if (L7_NULLPTR == pType)
      break;
    SEMA_TAKE(WRITE);   
    rc = connTransQTop(&trans); /* do not pop! */
    SEMA_GIVE(WRITE);   

    if (L7_SUCCESS != rc)    
      break;
    *pType = trans.trType;
  } while(0);
  return rc;
}


/*********************************************************************
*
* @purpose  Purge all pending transactions
*
* @param    void
*
* @returns  void
*
* @end
*
*********************************************************************/
void
cpdmClientConnStatusNextTransPurge(void)
{
  SEMA_TAKE(WRITE);   
  connTransQFinalize(); 
  SEMA_GIVE(WRITE);   
}


/*********************************************************************
*
* @purpose  Get next new connection transaction info, if it exists
*
* @param    L7_enetMacAddr_t * mac  @b{(output)} client MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new transaction available,
*           if next transaction is not for the addition of a new
*           client, or if parameter is garbage.  Removes transaction
*           from queue.
* @end
*
*********************************************************************/

L7_RC_t
cpdmClientConnStatusNewGet(L7_enetMacAddr_t * mac)
{
  L7_RC_t rc = L7_FAILURE;

  do 
  {
    cpConnTransaction_t trans;
    L7_RC_t topRC = L7_FAILURE;
    if (L7_NULLPTR == mac)
      break;
    SEMA_TAKE(WRITE);   
    topRC = connTransQTop(&trans);
    if ((L7_SUCCESS == topRC) && (CP_CONN_NEW == trans.trType))
    {
      connTransQPop();
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);   
    if (L7_SUCCESS != rc)    
      break;
    memcpy(mac, &(trans.tr.newConn.mac), sizeof(L7_enetMacAddr_t));
  } while(0);
  return rc;
}


/*********************************************************************
*
* @purpose  Get next delete connection transaction info, if it exists
*
* @param    L7_enetMacAddr_t * mac  @b{(output)} client MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new transaction available,
*           if next transaction is not for the deletion of a 
*           client, or if parameter is garbage.  Removes transaction
*           from queue.
* @end
*
*********************************************************************/


L7_RC_t
cpdmClientConnStatusDeleteGet(L7_enetMacAddr_t * mac)
{
  L7_RC_t rc = L7_FAILURE;

  do 
  {
    cpConnTransaction_t trans;
    L7_RC_t topRC = L7_FAILURE;
    if (L7_NULLPTR == mac)
      break;
    SEMA_TAKE(WRITE);   
    topRC = connTransQTop(&trans);
    if ((L7_SUCCESS == topRC) && (CP_CONN_DELETE == trans.trType))
    {
      connTransQPop();      
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);   
    if (L7_SUCCESS != rc)    
      break;
    memcpy(mac, &(trans.tr.deleteConn.mac), sizeof(L7_enetMacAddr_t));
  } while(0);
  return rc;
}



/*********************************************************************
*
* @purpose  Get next deauth connection transaction info, if it exists
*
* @param    L7_enetMacAddr_t * pPeerMac   @b{(output)} peer sw MAC
* @param    L7_enetMacAddr_t * pClientMac @b{(output)} client MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new transaction available,
*           if next transaction is not for the deauthentication of a 
*           client, or if parameter is garbage.  Removes transaction
*           from queue.
* @end
*
*********************************************************************/

L7_RC_t
cpdmClientConnStatusDeauthGet(L7_enetMacAddr_t * pPeerMac,
                              L7_enetMacAddr_t * pClientMac)
{
  L7_RC_t rc = L7_FAILURE;

  do 
  {
    cpConnTransaction_t trans;
    L7_RC_t topRC = L7_FAILURE;
    if ((L7_NULLPTR == pPeerMac) || (L7_NULLPTR == pClientMac))
      break;
    SEMA_TAKE(WRITE);   
    topRC = connTransQTop(&trans);
    if ((L7_SUCCESS == topRC) && (CP_CONN_DEAUTH == trans.trType))
    {
      connTransQPop();
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);   
    if (L7_SUCCESS != rc)    
      break;
    memcpy(pPeerMac,   &(trans.tr.deauthConn.peerMac), sizeof(L7_enetMacAddr_t));
    memcpy(pClientMac, &(trans.tr.deauthConn.clientMac), sizeof(L7_enetMacAddr_t));
  } while(0);
  return rc;
}


/*********************************************************************
*
* @purpose  Add new connection information if available
*
* @param    L7_enetMacAddr_t* macAddr  @b{(input)} MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new information can be added,
*           or if parameter is garbage.
*
* @end
*
*********************************************************************/
L7_RC_t
cpdmClientConnStatusNewAdd(L7_enetMacAddr_t * mac)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  rc = newConnAdd(mac);
  SEMA_GIVE(WRITE);   
  return rc;
}

/*********************************************************************
*
* @purpose  Add delete connection information if available
*
* @param    L7_enetMacAddr_t* macAddr  @b{(input)} MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new information can be added,
*           or if parameter is garbage.
*
* @end
*
*********************************************************************/
L7_RC_t
cpdmClientConnStatusDeleteAdd(L7_enetMacAddr_t * mac)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  rc = deleteConnAdd(mac);
  SEMA_GIVE(WRITE);   
  return rc;
}

/*********************************************************************
*
* @purpose  Add deauth connection information if available
*
* @param    L7_enetMacAddr_t*  @b{(input)} peer MAC addr
* @param    L7_enetMacAddr_t*  @b{(input)} client MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new information can be added,
*           or parameters are garbage.
*
* @end
*
*********************************************************************/
L7_RC_t
cpdmClientConnStatusDeauthAdd(L7_enetMacAddr_t * pPeerMac,
                              L7_enetMacAddr_t * pClientMac)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  rc = deauthConnAdd(pPeerMac, pClientMac);
  SEMA_GIVE(WRITE);   
  return rc;
}






/*********************************************************************
*
* @purpose  Add MAC to the client connection status table
*
* @param    L7_enetMacAddr_t* macAddr  @b{(input)} MAC addr
* @param    L7_uint32 intIfNum  @b{(input)} internal interface number
* @param    L7_uchar8 *userName @b{(input)} name of session's user
* @param    L7_IP_ADDR_t ipAddr @b{(input)} IP address of client
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t
cpdmClientConnStatusAdd(L7_enetMacAddr_t * mac,
                        L7_uchar8 * userName,
                        L7_uint32 intIfNum,
                        L7_IP_ADDR_t ipAddr)
{
  cpdmClientConnStatusData_t entry, *pEntry = L7_NULLPTR;
  cpdmIntfClientConnStatusData_t intfConn, *pIntfConn = L7_NULLPTR;
  cpdmClientAssocConnStatusData_t cpAssoc, *pCPAssoc = L7_NULLPTR;

  /* We need these booleans because someone thought that avlInsertTree() 
   * should return NULL on success -- so we can't tell if that element 
   * pointer value, once assigned, is either a successful return code,
   * or if the pointer was just initialized.  
   * Thanks for increasing my stack size, pal.
   */
  L7_BOOL entry_added = L7_FALSE;
  L7_BOOL intfConn_added = L7_FALSE;
  L7_BOOL cpAssoc_added = L7_FALSE;

  L7_RC_t rc = L7_FAILURE;
  cpId_t cpId;

  

  if (!clientConnStatusTreeReady || !intfConnTreeReady || 
      (L7_NULLPTR == mac) || (L7_NULLPTR == userName))
  {
    return L7_FAILURE;
  }

  memset(&entry, 0, sizeof(cpdmClientConnStatusData_t));
  if (L7_SUCCESS != cpdmCPConfigIntIfNumFind(intIfNum,&cpId))
  {
    LOG_MSG("no CP ID associated with intf %d?\n", intIfNum);
    return L7_FAILURE;
  }

  memcpy(&entry.macAddr, mac, sizeof(L7_enetMacAddr_t));
  entry.ip = ipAddr;
  osapiStrncpySafe(entry.userName, userName, CP_USER_LOCAL_USERNAME_MAX+1);
  entry.intfId = intIfNum;
  entry.cpId = cpId;
  {
    time_t now = time(NULL);
    entry.sessionStart = entry.idleStart = now;
    entry.lastRefresh = now;
  }

  /* Initialize current counts to previous -- we'll subtract
   * one from the other when reporting byte counts etc.
   */
  if (L7_SUCCESS != cpimGetStatistics(intIfNum, mac, &entry.previous))
  {
    LOG_MSG("client connection: can't get stats for intf %d\n", 
      intIfNum);
    memset(&entry.previous, 0, sizeof(txRxCount_t));
  }
  memcpy(&entry.current, &entry.previous, sizeof(txRxCount_t));

  cpAssoc.assoc.cpId = cpId;
  memcpy(&cpAssoc.assoc.macAddr, mac, sizeof(L7_enetMacAddr_t));
  cpAssoc.avlPtr = L7_NULLPTR;

  intfConn.assoc.intIfNum = intIfNum;
  memcpy(&intfConn.assoc.macAddr, mac, sizeof(L7_enetMacAddr_t));
  intfConn.avlPtr = L7_NULLPTR;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = avlInsertEntry(&clientConnStatusTree, &entry);
    if (L7_NULLPTR != pEntry) /* if item not inserted */
    {
      LOG_MSG("%s: client not added to status table", __FUNCTION__);
      break;                  /* ..then die           */
    }
    else
      entry_added = L7_TRUE;

    pCPAssoc = avlInsertEntry(&cacStatusTree, &cpAssoc);
    if (L7_NULLPTR != pCPAssoc) /* if item not inserted */
    {
      LOG_MSG("%s: client not added to CAC status table", __FUNCTION__);
      break;                    /* ..then die           */
    }
    else
      cpAssoc_added = L7_TRUE;

    pIntfConn = avlInsertEntry(&intfConnTree, &intfConn);
    if (L7_NULLPTR != pIntfConn) /* if item not inserted */
    {
      LOG_MSG("%s: client not added to intf/conn table", __FUNCTION__);
      break;                     /* ..then die           */
    }
    else
      intfConn_added = L7_TRUE;

    /* Increment the global count for CP */
    cpdmOprData->cpGlobalStatus.users.authenticated++;

    {
      cpdmActivityStatusData_t *pActStat = L7_NULLPTR;
      cpdmIntfStatusData_t *pIntfStat = L7_NULLPTR;

      /* Increment count of users for this CP instance */
      if (0 < cpId)
      {
        pActStat = cpdmFindActStatus(cpId);
        if (L7_NULLPTR != pActStat)
        {
          pActStat->users++;

          if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
          {
            cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
            if (cpClusterFlagSet(CP_CLUSTER_INST_STATUS_SEND) != L7_SUCCESS)
            {
              LOG_MSG("cpdmClientConnStatusAdd: Failed to set instance status send flag.\n");
              cpdmOprData->cpLocalSw.cpId = 0;
            }
          }
        }
      }

      /* Increment count of users on the netwk interface */
      pIntfStat = cpdmFindIntfStatus(intIfNum);
      if (L7_NULLPTR != pIntfStat)
      {
        pIntfStat->users++;

        if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
        {
          cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
          cpdmOprData->cpLocalSw.intf = intIfNum;
          if (cpClusterFlagSet(CP_CLUSTER_INST_INTF_STATUS_SEND) != L7_SUCCESS)
          {
            LOG_MSG("cpdmClientConnStatusAdd: Failed to set instance intf status send flag.\n");
            cpdmOprData->cpLocalSw.cpId = 0;
            cpdmOprData->cpLocalSw.intf = 0;
          }
        }

      }
    }

    rc = newConnAdd(&(entry.macAddr));
  } while(0);


  if (L7_SUCCESS == rc)
  {
#ifdef L7_WIRELESS_PACKAGE
    wdmAssocClientCPAuthFlagSet(entry.macAddr,L7_TRUE);
#endif
  }
  else
  {
    if (L7_TRUE == entry_added)
      (void) avlDeleteEntry(&clientConnStatusTree, &entry);
    if (L7_TRUE == cpAssoc_added)
      (void) avlDeleteEntry(&cacStatusTree, &cpAssoc);
    if (L7_TRUE == intfConn_added)
      (void) avlDeleteEntry(&intfConnTree, &intfConn);
  }
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Helper routine to update various parameters used primarily
*           upon successful authentication.
*
* @param    L7_enetMacAddr_t* macAddr @b{(input)} existing client
* @param    cpdmClientConnStatusData_t* macAddr @b{(input)} new data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUpdate(L7_enetMacAddr_t * mac,
                                   cpdmClientConnStatusData_t * client)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == mac)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(mac);
    if (L7_NULLPTR == pEntry) 
    {
      break;
    }
    memcpy(&(pEntry->switchMacAddr), &client->switchMacAddr, sizeof(L7_enetMacAddr_t)); 
    pEntry->whichSwitch = client->whichSwitch;
    pEntry->switchIp = client->switchIp;
    pEntry->protocolMode = client->protocolMode;
    pEntry->verifyMode = client->verifyMode;
    pEntry->uId = client->uId;
    memcpy(&(pEntry->limits), &(client->limits), sizeof(cpConnectionLimits_t));
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
* @purpose  Update client status upon successful authentication.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusRoamUpdate(L7_enetMacAddr_t *macAddr)
{
  cpdmClientConnStatusData_t *ptrData = L7_NULLPTR;
  cpdmConfigData_t *pData = L7_NULLPTR;
  cpId_t cpId;
  L7_RC_t rc = L7_FAILURE;

  if (macAddr == L7_NULLPTR)
  {
    return rc;
  }

  SEMA_TAKE(WRITE);
  ptrData = cpdmFindClientConnStatus(macAddr);
  if (ptrData != L7_NULLPTR)
  {
    ptrData->limits.sessionTimeout = cpdmOprData->cpClientAuthInfo.sessionTime;
    ptrData->cumulative.bytesTransmitted = cpdmOprData->cpClientAuthInfo.bytesTx;
    ptrData->cumulative.bytesReceived = cpdmOprData->cpClientAuthInfo.bytesRx;
    ptrData->cumulative.packetsTransmitted = cpdmOprData->cpClientAuthInfo.pktsTx;
    ptrData->cumulative.packetsReceived = cpdmOprData->cpClientAuthInfo.pktsRx;

    cpId = (cpId_t)cpdmOprData->cpClientAuthInfo.cpId;
    pData = cpdmFindConfig(cpId, L7_FALSE);
    if (pData != L7_NULLPTR)
    {
      pData->userUpRate = cpdmOprData->cpClientAuthInfo.userUpRate;
      pData->userDownRate = cpdmOprData->cpClientAuthInfo.userDownRate;
    }

    rc = L7_SUCCESS;
  }
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
* @purpose  Update client in the controller connection status table
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_enetMacAddr_t swMac         @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusCrtlUpdate(L7_enetMacAddr_t clientMacAddr,
                                       L7_enetMacAddr_t swMac)
{
  cpdmClientConnStatusData_t *ptrData = L7_NULLPTR;
  clusterMemberID_t memberID;
  L7_IP_ADDR_t swIP;
  L7_in6_addr_t ipV6;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if (!clientConnStatusTreeReady)
  {
    return L7_FAILURE;
  }

  memcpy(&memberID, &swMac, sizeof(L7_enetMacAddr_t));
  if (clusterMemberIdentityGet(&memberID, &swIP, &ipV6) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  ptrData = avlSearchLVL7(&clientConnStatusTree, &clientMacAddr, AVL_EXACT);
  if (ptrData != L7_NULLPTR)
  {
    memcpy(&ptrData->switchMacAddr, &swMac, sizeof(L7_enetMacAddr_t));
    ptrData->switchIp = swIP;
    ptrData->whichSwitch = CP_AUTH_ON_PEER_SWITCH; /* as peer owns it now */
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
* @purpose  Add peer switch client to the connection status table
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_IP_ADDR_t     ipAddr        @b{(input)} client IP address
* @param    L7_enetMacAddr_t swMac         @b{(input)} switch MAC address
* @param    L7_uchar8        userName      @b{(input)} user name
* @param    L7_LOGIN_TYPE_t  prtlMode      @b{(input)} protocol mode
* @param    CP_VERIFY_MODE_t verifyMode    @b{(input)} verify mode
* @param    L7_uint32        intf          @b{(input)} internal interface number
* @param    L7_uint32        cpId          @b{(input)} instance index
* @param    txRxCount_t      stats         @b{(input)} statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If the client entry already exists, it is updated.
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerAddUpdate(L7_enetMacAddr_t clientMacAddr, L7_IP_ADDR_t ipAddr,
                                          L7_enetMacAddr_t swMac, L7_uchar8 *userName,
                                          L7_LOGIN_TYPE_t prtlMode, CP_VERIFY_MODE_t verifyMode,
                                          L7_uint32 intf, L7_uint32 cpId, txRxCount_t *stats)
{
  cpdmClientConnStatusData_t data, *ptrData = L7_NULLPTR;
  clusterMemberID_t memberID;
  L7_IP_ADDR_t swIP;
  L7_in6_addr_t ipV6;
  cpId_t cpId16;
  L7_RC_t rc = L7_FAILURE;

  

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if (!clientConnStatusTreeReady || (userName == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  memcpy(&memberID, &swMac, sizeof(L7_enetMacAddr_t));
  if (clusterMemberIdentityGet(&memberID, &swIP, &ipV6) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  ptrData = avlSearchLVL7(&clientConnStatusTree, &clientMacAddr, AVL_EXACT);
  if (ptrData == L7_NULLPTR)
  {
    /* add entry */
    memset(&data, 0x00, sizeof(cpdmClientConnStatusData_t));
    memcpy(&data.macAddr, &clientMacAddr, sizeof(L7_enetMacAddr_t));
    data.ip = ipAddr;
    memcpy(&data.switchMacAddr, &swMac, sizeof(L7_enetMacAddr_t));
    data.switchIp = swIP;
    data.whichSwitch = CP_AUTH_ON_PEER_SWITCH;
    osapiStrncpySafe(data.userName, userName, CP_USER_LOCAL_USERNAME_MAX+1);
    data.protocolMode = prtlMode;
    data.verifyMode = verifyMode;
    data.intfId = intf;
    cpId16 = (cpId_t)cpId;
    data.cpId = cpId16;
    time_t now = time(NULL);
    data.sessionStart = data.idleStart = now;
    memset(&data.previous, 0, sizeof(txRxCount_t));
    memset(&data.current, 0, sizeof(txRxCount_t));
    memcpy(&data.cumulative, stats, sizeof(txRxCount_t));
    data.lastRefresh = now;

    ptrData = avlInsertEntry(&clientConnStatusTree, &data);
    if (ptrData == L7_NULLPTR)
    {
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  else
  {
    /* update entry */
    memcpy(&ptrData->switchMacAddr, &swMac, sizeof(L7_enetMacAddr_t));
    ptrData->switchIp = swIP;
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
* @purpose  Set flag to delete a peer switch client(s)
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    cpId_t           cpId          @b{(input)} CP instance
* @param    L7_BOOL          all           @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerDeleteFlagSet(L7_enetMacAddr_t clientMacAddr,
                                              cpId_t cpId, L7_BOOL all)
{
  cpdmClientConnStatusData_t *ptrData = L7_NULLPTR;
  L7_RC_t rc = L7_SUCCESS;

  

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if (all == L7_TRUE)
  {
    if (cpClusterFlagSet(CP_CLUSTER_DEAUTH_ALL_CLIENTS_SEND) != L7_SUCCESS)
    {
      LOG_MSG("cpdmClientConnStatusPeerDeleteFlagSet: Failed to set deauth all clients send flag.\n");
      rc = L7_FAILURE;
    }
  }
  else if ((cpId >= CP_ID_MIN) && (cpId <= CP_ID_MAX))
  {
    cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
    if (cpClusterFlagSet(CP_CLUSTER_DEAUTH_INST_CLIENTS_SEND) != L7_SUCCESS)
    {
      LOG_MSG("cpdmClientConnStatusPeerDeleteFlagSet: Failed to set deauth instance clients send flag.\n");
      cpdmOprData->cpLocalSw.cpId = 0;
      rc = L7_FAILURE;
    }
  }
  else if (cpClusterValidMacAddr(clientMacAddr.addr) == L7_TRUE)
  {
    L7_BOOL gotAddr = L7_FALSE;
    L7_enetMacAddr_t peerMac;

    SEMA_TAKE(READ);
    ptrData = avlSearchLVL7(&clientConnStatusTree, &clientMacAddr, AVL_EXACT);
    if (ptrData != L7_NULLPTR)
    {
      gotAddr = L7_TRUE;
      memcpy(&peerMac, &ptrData->switchMacAddr, sizeof(L7_enetMacAddr_t));      
    }
    SEMA_GIVE(READ);

    if (L7_TRUE == gotAddr)
    {
      SEMA_TAKE(WRITE);      
      /* rc = deauthConnAdd(&peerMac, &clientMacAddr); */
      rc = deleteConnAdd(&clientMacAddr);
      SEMA_GIVE(WRITE);
    }
    else
    {
      rc = L7_FAILURE;
    }

  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete peer switch client from the connection status table
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_enetMacAddr_t swMacAddr     @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/

static
L7_BOOL has_switch_mac(cpdmClientConnStatusData_t * pEntry,
                       void * pData)
{
   L7_enetMacAddr_t * pMac = (L7_enetMacAddr_t *) pData;
   if (0 == memcmp(&(pEntry->switchMacAddr), pMac,
                   sizeof(L7_enetMacAddr_t))) {
       return L7_TRUE;
   }
   return L7_FALSE;
}

L7_RC_t cpdmClientConnStatusPeerDelete(L7_enetMacAddr_t clientMacAddr,
                                       L7_enetMacAddr_t swMacAddr)
{
  if (clusterSupportGet() != L7_SUCCESS) 
  {
        LOG_MSG("%s: NOT SUPPORTED", __FUNCTION__);
        return L7_NOT_SUPPORTED;
  }

  if (!clientConnStatusTreeReady) 
  {
        LOG_MSG("%s: FAIL", __FUNCTION__);
        return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  clientConnTablesAllDeleteHelp(clientMacAddr, has_switch_mac,
                               (void *) &swMacAddr);
  SEMA_GIVE(WRITE);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete all clients for the specified peer switch from the 
*           connection status table
*
* @param    L7_enetMacAddr_t swMacAddr @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerAllDelete(L7_enetMacAddr_t swMacAddr)
{
  cpdmClientConnStatusData_t *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t macAddr;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if (!clientConnStatusTreeReady)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  while ((ptrData=avlSearchLVL7(&clientConnStatusTree, &macAddr, AVL_NEXT))
         != L7_NULLPTR)
  {
    memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));
    clientConnTablesAllDeleteHelp(macAddr, has_switch_mac, (void *) &swMacAddr);
  }
  SEMA_GIVE(WRITE);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete all peer switch clients from connection status table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function is called when the local switch leaves the 
*           cluster or when a switch is no longer the cluster controller.
*           UI should never call this function.
*
* @end
*********************************************************************/

static
L7_BOOL authed_on_peer(cpdmClientConnStatusData_t * pEntry,
                       void * pUnused)
{
  if (pEntry->whichSwitch == CP_AUTH_ON_PEER_SWITCH) 
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}


L7_RC_t cpdmClientConnStatusPeerPurge(void)
{
  cpdmClientConnStatusData_t *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t macAddr;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if (!clientConnStatusTreeReady)
  {
    return L7_FAILURE;
  }

  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  SEMA_TAKE(WRITE);
  ptrData = avlSearchLVL7(&clientConnStatusTree, &macAddr, AVL_NEXT);
  while (ptrData != L7_NULLPTR)
  {
    memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));
    clientConnTablesAllDeleteHelp(macAddr, authed_on_peer, L7_NULLPTR);
    ptrData = avlSearchLVL7(&clientConnStatusTree, &macAddr, AVL_NEXT);
  }
  SEMA_GIVE(WRITE);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update peer switch client statistics
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    txRxCount_t      stats         @b{(input)} pointer to statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerStatsUpdate(L7_enetMacAddr_t clientMacAddr,
                                            txRxCount_t *stats)
{
  cpdmClientConnStatusData_t *ptrData = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if ((!clientConnStatusTreeReady) || (stats == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  ptrData = avlSearchLVL7(&clientConnStatusTree, &clientMacAddr, AVL_EXACT);
  if (ptrData != L7_NULLPTR)
  {
    memcpy(&ptrData->cumulative, stats, sizeof(txRxCount_t));
    ptrData->lastRefresh = osapiUTCTimeNow();
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
* @purpose  Set user initiation flag for client deletion
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpdmClientConnStatusDeleteUserRequestSet(void)
{
  if (clusterSupportGet() == L7_SUCCESS)
  {
    cpdmOprData->cpLocalSw.userInitiated = L7_TRUE;
  }
  return;
}


/*********************************************************************
*
* @purpose  Wipe MAC from the client connection status table
*
* @param    L7_enetMacAddr_t* macAddr @b{(input)} MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusDelete(L7_enetMacAddr_t * mac)
{
  cpdmAuthWhichSwitch_t swType;

  if (clusterSupportGet() == L7_SUCCESS)
  {
    if (cpdmClientConnStatusWhichSwitchGet(mac, &swType) != L7_SUCCESS)
    {
      LOG_MSG("%s: Failed to get client switch type", __FUNCTION__);
      return L7_FAILURE;
    }

    if (swType == CP_AUTH_ON_PEER_SWITCH)
    {
      if (cpdmOprData->cpLocalSw.userInitiated == L7_TRUE)
      {
        cpdmOprData->cpLocalSw.userInitiated = L7_FALSE;
        return cpdmClientConnStatusPeerDeleteFlagSet(*mac, 0, L7_FALSE);
      }
      else
      {
        return L7_SUCCESS; /* its just a new peer client notification */
      }
    }
  }

  return clientConnStatusDeleteHelper(mac, L7_TRUE);
}


/*********************************************************************
*
* @purpose  Wipe MAC from the client connection status table
*
* @param    L7_enetMacAddr_t* macAddr @b{(input)} MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusDeleteNoDeauth(L7_enetMacAddr_t * mac)
{
  return clientConnStatusDeleteHelper(mac, L7_FALSE);
}




/*********************************************************************
*
* @purpose  Wipe out all connection information from our various
*           tables for a particular MAC, unless it fails to match
*           the criterion of a passed-in predicate.
*
* @param    L7_enetMacAddr_t  mac    @b{(input)} MAC addr
* @param    L7_BOOL           deauth @b{(input)} deauth client?
* @param    conn_del_pred     pred   @b{(input)} predicate
* @param    void *            data   @b{(input)} predicate data
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function uses NO sema protection, and is here to
*           minimize the codebase.  It should NEVER be exported.
*           
*
* @end
*
*********************************************************************/
static
void clientConnTablesAllDeleteHelp(L7_enetMacAddr_t mac,
                                   conn_del_pred    pred,
                                   void             *data)
{ 
  {
     cpdmClientConnStatusData_t entry, *pEntry =
        avlSearchLVL7(&clientConnStatusTree, &mac, AVL_EXACT);
     if (L7_NULLPTR == pEntry) 
     {
        LOG_MSG("%s: entry not found?", __FUNCTION__);
        return;
     }
     if ((L7_NULLPTR != pred) && (L7_FALSE == pred(pEntry, data))) {
         /* criteria not met, not deleting */
        return;
     }
     memcpy(&entry.macAddr, &mac, sizeof(mac));
     (void) avlDeleteEntry(&clientConnStatusTree, &entry);
  }
  {
     L7_uint32 del = 0, att = 0;  /* deleted, attempted */
     cpdmClientAssocConnStatusData_t cpAssoc, *pAssoc;
     memset(&cpAssoc, 0, sizeof(cpAssoc));
     while(L7_NULLPTR !=
           (pAssoc = avlSearchLVL7(&cacStatusTree, &cpAssoc, AVL_NEXT)))
     {
       if (0 == memcmp(&mac, &(pAssoc->assoc.macAddr), sizeof(mac))) 
       {
         if (L7_NULLPTR != avlDeleteEntry(&cacStatusTree, pAssoc))
         {
            del++;
         }
         att++;
       }
       memcpy(&cpAssoc, pAssoc, sizeof(cpAssoc));
     }
     if (att != del)
     {
        LOG_MSG("%s: %d of %d items removed from CAC status table",
                __FUNCTION__, del, att);
     }
  }
  {
     cpdmIntfClientConnStatusData_t intfConn, *pConn;
     L7_uint32 del = 0, att = 0;  /* deleted, attempted */
     memset(&intfConn, 0, sizeof(intfConn)); 
     while(L7_NULLPTR !=
              (pConn = avlSearchLVL7(&intfConnTree, &intfConn, AVL_NEXT)))
     {
        if (0 == memcmp(&mac, &(pConn->assoc.macAddr), sizeof(mac))) 
        {
          if (L7_NULLPTR != avlDeleteEntry(&intfConnTree, pConn)) 
          {
             del++;
          }
          att++;
        }
        memcpy(&intfConn, pConn, sizeof(intfConn));
     }
     if (att != del)
     {
        LOG_MSG("%s: %d of %d items removed from intf conn table",
                __FUNCTION__, del, att);
     }
  }
}


static
void    clientConnIntfDeauth(L7_enetMacAddr_t mac)
{
  L7_enetMacAddr_t thisMac;
  L7_uint32        thisIntIf = 0;

  memset(&thisMac, 0, sizeof(thisMac));
  while(L7_SUCCESS ==
        cpdmInterfaceConnStatusNextGet(thisIntIf, &thisMac,
                                       &thisIntIf, &thisMac))
  {
    if (0 == memcmp(&thisMac, &mac, sizeof(thisMac))) 
    {
      (void) cpimIntfDeauthenticate(thisIntIf, &thisMac);
    }
  }
}


static
L7_RC_t clientConnStatusDeleteHelper(L7_enetMacAddr_t * mac, L7_BOOL doDeauth)
{
  cpdmClientConnStatusData_t entry, *pEntry = L7_NULLPTR;
  cpdmClientAssocConnStatusData_t cpAssoc;
  cpdmIntfClientConnStatusData_t intfConn;
  cpdmActivityStatusData_t *pActStat = L7_NULLPTR;
  cpdmIntfStatusData_t *pIntfStat = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum = 0;
  cpId_t cpId = 0;
  L7_enetMacAddr_t swMac; 
  L7_IP_ADDR_t ipAddr = 0;

  if (!mac || !clientConnStatusTreeReady ||  
      !intfConnTreeReady || !intfStatusTreeReady)
  {
    return rc;
  }

  memset(&entry,   0, sizeof(cpdmClientConnStatusData_t));
  memcpy(&entry.macAddr,   mac, sizeof(L7_enetMacAddr_t));
  memcpy(&cpAssoc.assoc.macAddr, mac, sizeof(L7_enetMacAddr_t));
  memcpy(&intfConn.assoc.macAddr, mac, sizeof(L7_enetMacAddr_t));
  cpAssoc.avlPtr = L7_NULLPTR;
  intfConn.avlPtr = L7_NULLPTR; 

  if (L7_TRUE == doDeauth)
  {
    clientConnIntfDeauth(*mac);
  }

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = avlSearchLVL7(&clientConnStatusTree, &entry, AVL_EXACT);

    if (L7_NULLPTR == pEntry) 
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
              "%s: MAC address %02x:%02x:%02x:%02x:%02x:%02x not found, I/F = %d",
              __FUNCTION__, mac->addr[0], mac->addr[1], mac->addr[2],
              mac->addr[3], mac->addr[4], mac->addr[5], intIfNum);
      break; /* don't continue */
    }

    intIfNum = intfConn.assoc.intIfNum = pEntry->intfId;    
    memcpy(&swMac, &(pEntry->switchMacAddr), sizeof(L7_enetMacAddr_t));
    ipAddr = pEntry->ip;
    cpId = pEntry->cpId; 
    cpAssoc.assoc.cpId = cpId;    

    clientConnTablesAllDeleteHelp(*mac, L7_NULLPTR, L7_NULLPTR);

    /* Decrement the global count for CP */
    if (0 < cpdmOprData->cpGlobalStatus.users.authenticated)
    {
      cpdmOprData->cpGlobalStatus.users.authenticated--;
    }
    /* Decrement count of users for this CP instance */
    if (0 < cpId)
    {
      pActStat = cpdmFindActStatus(cpId);
      if ((L7_NULLPTR != pActStat) && (0 < pActStat->users))
      {
         pActStat->users--;

        if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
        {
          cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
          if (cpClusterFlagSet(CP_CLUSTER_INST_STATUS_SEND) != L7_SUCCESS)
          {
            LOG_MSG("clientConnStatusDeleteHelper: Failed to set instance status send flag.\n");
            cpdmOprData->cpLocalSw.cpId = 0;
          }
        }

      }
    }
    /* Decrement count of users on the netwk interface */
    pIntfStat = cpdmFindIntfStatus(intIfNum);
    if ((L7_NULLPTR != pIntfStat) && (0 < pIntfStat->users))
    {
      pIntfStat->users--;

      if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
      {
        cpdmOprData->cpLocalSw.cpId = (L7_uint32)cpId;
        cpdmOprData->cpLocalSw.intf = intIfNum;
        if (cpClusterFlagSet(CP_CLUSTER_INST_INTF_STATUS_SEND) != L7_SUCCESS)
        {
          LOG_MSG("clientConnStatusDeleteHelper: Failed to set instance intf status send flag.\n");
          cpdmOprData->cpLocalSw.cpId = 0;
          cpdmOprData->cpLocalSw.intf = 0;
        }
      }

    }

    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(WRITE);   

  /* if RC is good and we've deauthorized, send a trap. */
  if ((L7_SUCCESS == rc) && (L7_TRUE == doDeauth))
  {
    cpTrapClientDisconnected(&entry.macAddr, ipAddr, &swMac, cpId, intIfNum);

#ifdef L7_WIRELESS_PACKAGE
    wdmAssocClientCPAuthFlagSet(entry.macAddr, L7_FALSE);
#endif

    rc = cpdmClientConnStatusDeleteAdd(&(entry.macAddr));
    if (L7_SUCCESS != rc)
    {
      LOG_MSG("%s: Failed to enq client delete info", __FUNCTION__);
    }
  }

  return rc;
}


/*********************************************************************
*
* @purpose  Delete ALL entries in client connection table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusDeleteAll(void)
{
  L7_enetMacAddr_t mac;
  cpdmAuthWhichSwitch_t swType;

  if (!clientConnStatusTreeReady) 
  {
    return L7_FAILURE;
  }
  memset(&mac, 0, sizeof(L7_enetMacAddr_t));

  while(L7_SUCCESS == cpdmClientConnStatusNextGet(&mac, &mac))
  {
    if (cpdmClientConnStatusWhichSwitchGet(&mac, &swType) != L7_SUCCESS)
    {
      LOG_MSG("cpdmClientConnStatusDelete: Failed to get client switch type.\n");
      return L7_FAILURE;
    }

    if (swType == CP_AUTH_ON_PEER_SWITCH)
    {
      continue;
    }

    /* No sema protection here, so it is possible our entry to get
     * deleted before this call.  So ignore the return code in the
     * next line just in case.
     */
    (void) cpdmClientConnStatusDelete(&mac);
     memset(&mac, 0, sizeof(L7_enetMacAddr_t));
  }

  if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
  {
    memset(&mac, 0, sizeof(L7_enetMacAddr_t));
    if (cpdmClientConnStatusPeerDeleteFlagSet(mac, 0, L7_TRUE) != L7_SUCCESS)
    {
      LOG_MSG("cpdmClientConnStatusDeleteAll: Failed to set deauth all clients send flag.\n");
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Delete entries in client connection table used by 
*           a captive portal instance
*
* @param    cpId_t cpId, input, id of the CP instance to unplug
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusByCPIdDeleteAll(cpId_t cpId)
{
  cpdmClientConnStatusData_t entry, *pEntry = L7_NULLPTR;
  L7_enetMacAddr_t mac;
  L7_RC_t          rc = L7_SUCCESS;

  if (!clientConnStatusTreeReady) 
  {
    return L7_FAILURE;
  }

  while (L7_SUCCESS == rc) 
  {
    memset(&entry,  0, sizeof(cpdmClientConnStatusData_t));

    SEMA_TAKE(READ);   
    /* If there's an entry in the client conn status table for the
     * given CP ID, find it, and copy the MAC address
     */
    for( pEntry = avlSearchLVL7(&clientConnStatusTree, &entry, AVL_NEXT);
         L7_NULLPTR != pEntry;
         pEntry = avlSearchLVL7(&clientConnStatusTree, pEntry, AVL_NEXT))
    {
      if ((cpId == pEntry->cpId) && (pEntry->whichSwitch == CP_AUTH_ON_LOCAL_SWITCH))
      {
        memcpy(&mac, &(pEntry->macAddr),  sizeof(L7_enetMacAddr_t));      
        break;
      }
    }
    SEMA_GIVE(READ);     

    if (L7_NULLPTR == pEntry)
      break; /* no work left to do */

    rc = cpdmClientConnStatusDelete(&mac);
  }

  if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
  {
    memset(&mac, 0, sizeof(L7_enetMacAddr_t));
    if (cpdmClientConnStatusPeerDeleteFlagSet(mac, cpId, L7_FALSE) != L7_SUCCESS)
    {
      LOG_MSG("cpdmClientConnStatusByCPIdDeleteAll: Failed to set deauth CP instance clients send flag.\n");
      rc = L7_FAILURE;
    }
  }

  return rc;
}



/*********************************************************************
*
* @purpose  Delete entries in client connection table used by 
*           a particular interface
*
* @param    L7_uint32 intIfNum, input, number of instance to unplug
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusByIntfDeleteAll(L7_uint32 intIfNum)
{
  cpdmClientConnStatusData_t entry, *pEntry = L7_NULLPTR;
  L7_enetMacAddr_t mac;
  L7_RC_t          rc = L7_SUCCESS;

  if (!clientConnStatusTreeReady) 
  {
    return L7_FAILURE;
  }

  while (L7_SUCCESS == rc) 
  {
    memset(&entry,  0, sizeof(cpdmClientConnStatusData_t));

    SEMA_TAKE(READ);   
    /* If there's an entry in the client conn status table for the
     * given CP ID, find it, and copy the MAC address
     */
    for( pEntry = avlSearchLVL7(&clientConnStatusTree, &entry, AVL_NEXT);
         L7_NULLPTR != pEntry;
         pEntry = avlSearchLVL7(&clientConnStatusTree, pEntry, AVL_NEXT))
    {
      if (intIfNum == pEntry->intfId) 
      {
        memcpy(&mac, &(pEntry->macAddr),  sizeof(L7_enetMacAddr_t));      
        break;
      }
    }
    SEMA_GIVE(READ);     

    if (L7_NULLPTR == pEntry)
      break; /* no work left to do */

    rc = cpdmClientConnStatusDelete(&mac);
  }

  return rc;
}


/*********************************************************************
*
* @purpose  Delete entries in client connection table by name
*
* @param    L7_uchar8*  userName @b{(input)} name of user
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusByNameDelete(L7_uchar8 * userName)
{
  cpdmClientConnStatusData_t entry, *pEntry = L7_NULLPTR;
  L7_enetMacAddr_t mac;
  L7_RC_t rc = L7_SUCCESS;

  if (!userName || !clientConnStatusTreeReady) 
  {
    return L7_FAILURE;
  }

  while (L7_SUCCESS == rc) 
  {
    memset(&entry,  0, sizeof(cpdmClientConnStatusData_t));

    SEMA_TAKE(READ);   
    /* If there's an entry in the client conn status table for the
     * given user name, find it, and copy the MAC address
     */
    for( pEntry = avlSearchLVL7(&clientConnStatusTree, &entry, AVL_NEXT);
         L7_NULLPTR != pEntry;
         pEntry = avlSearchLVL7(&clientConnStatusTree, pEntry, AVL_NEXT))
    {
      if (0 == strcmp(userName, pEntry->userName)) 
      {
        memcpy(&mac, &(pEntry->macAddr),  sizeof(L7_enetMacAddr_t));      
        break;
      }
    }
    SEMA_GIVE(READ);     

    if (L7_NULLPTR == pEntry)
      break; /* no work left to do */

    rc = cpdmClientConnStatusDelete(&mac);
  }
  return rc;
}





/*********************************************************************
*
* @purpose  Find the next connection with the given uid
*
* @param    uId_t @b{(input)} uid - user id
* @param    L7_enetMacAddr_t * @b{(input)} mac - starting key
* @param    L7_enetMacAddr_t * @b{(output)} mac - resulting key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only makes sense with locally-verified connections
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusByUIdFind(uId_t uId, 
                                      L7_enetMacAddr_t * mac,
                                      L7_enetMacAddr_t * nextMac)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if ((L7_FALSE == clientConnStatusTreeReady) || 
      (L7_NULLPTR == mac) || (L7_NULLPTR == nextMac))
  {
    return rc;
  }

  SEMA_TAKE(READ);   
  for( pEntry = avlSearchLVL7(&clientConnStatusTree, mac, AVL_NEXT);
       L7_NULLPTR != pEntry;
       pEntry = avlSearchLVL7(&clientConnStatusTree, &(pEntry->macAddr), AVL_NEXT))
  {
    if (uId == pEntry->uId) 
    {
      memcpy(nextMac, &(pEntry->macAddr),  sizeof(L7_enetMacAddr_t));      
      rc = L7_SUCCESS;
      break;
    }
  }
  SEMA_GIVE(READ);     
  return rc;
}



/*********************************************************************
*
* @purpose  Get entry in the interface connection status table
*
* @param    L7_uint32  intIfNum @{(input)} internal interface number
* @param    L7_enetMacAddr *  mac @{(input)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmInterfaceConnStatusGet(L7_uint32 intIfNum, 
                                   L7_enetMacAddr_t * mac) 
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == mac)
    return rc;
  SEMA_TAKE(READ);
  do 
  {
    cpdmIntfClientConnStatusData_t conn, *pConn = L7_NULLPTR;    
    conn.assoc.intIfNum = intIfNum;
    memcpy(&(conn.assoc.macAddr), mac, sizeof(L7_enetMacAddr_t));
    conn.avlPtr = L7_NULLPTR; 
    pConn = avlSearchLVL7(&intfConnTree, &conn, AVL_EXACT);
    if (L7_NULLPTR == pConn)
      break;
    rc = L7_SUCCESS;
  } while (0);
  SEMA_GIVE(READ);
  return rc;
}


/*********************************************************************
*
* @purpose  Get next entry in the interface connection status table
*
* @param  L7_uint32    intIfNum @{(input)} internal interface number
* @param  L7_enetMacAddr   *mac @{(input)} MAC address
* @param  L7_uint32  *pIntIfNum @{(output)} internal interface number
* @param  L7_enetMacAddr  *pMac @{(output)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmInterfaceConnStatusNextGet(L7_uint32 intIfNum, 
                                       L7_enetMacAddr_t * mac,
                                       L7_uint32 *pIntIfNum, 
                                       L7_enetMacAddr_t * pMac)
{
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == mac) || (L7_NULLPTR == pMac) ||
      (L7_NULLPTR == pIntIfNum))
    return rc;
  SEMA_TAKE(READ);
  do 
  {
    cpdmIntfClientConnStatusData_t conn, *pConn = L7_NULLPTR;    
    conn.assoc.intIfNum = intIfNum;
    memcpy(&(conn.assoc.macAddr), mac, sizeof(L7_enetMacAddr_t));
    conn.avlPtr = L7_NULLPTR; 
    pConn = avlSearchLVL7(&intfConnTree, &conn, AVL_NEXT);
    if (L7_NULLPTR == pConn)
      break;
    *pIntIfNum = pConn->assoc.intIfNum;
    memcpy(pMac, &(pConn->assoc.macAddr), sizeof(L7_enetMacAddr_t));
    rc = L7_SUCCESS;
  } while (0);
  SEMA_GIVE(READ);
  return rc;
}


/*********************************************************************
*
* @purpose  Check for existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusGet(L7_enetMacAddr_t * macAddr)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;

  SEMA_TAKE(READ);   
  pEntry = cpdmFindClientConnStatus(macAddr);
  SEMA_GIVE(READ);   
  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}


/*********************************************************************
*
* @purpose  Check for NEXT MAC addr associated with a
*           connection status entry, given the current MAC addr
*
* @param    L7_enetMacAddr_t *  macAddr  @b{(input)} macAddr
* @param    L7_enetMacAddr_t *  nextMac  @b{(output)} next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusNextGet(L7_enetMacAddr_t * macAddr, 
                                    L7_enetMacAddr_t * nextMac)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;

  if ((!clientConnStatusTreeReady) || 
       (L7_NULLPTR == macAddr)     ||
       (L7_NULLPTR == nextMac))
    return L7_FAILURE;
  SEMA_TAKE(READ);   
  do 
  {
    pEntry = avlSearchLVL7(&clientConnStatusTree, macAddr, AVL_NEXT);   
    if (L7_NULLPTR == pEntry) 
      break;
    memcpy(nextMac, &(pEntry->macAddr), sizeof(L7_enetMacAddr_t));
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   
  return  rc;
}


/*********************************************************************
*
* @purpose  Find the next connection status entry matching the 
*           given user name
*
* @param    L7_uchar8 *  userId  @b{(input)} name to match
* @param    L7_enetMacAddr_t *  macAddr  @b{(input)} macAddr
* @param    L7_enetMacAddr_t *  nextMac  @b{(output)} next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cpdmClientConnStatusByNameFind(L7_uchar8 *userName, 
                                       L7_enetMacAddr_t * macAddr, 
                                       L7_enetMacAddr_t * nextMac)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmClientConnStatusData_t entry, *pEntry = L7_NULLPTR;

  if ((!clientConnStatusTreeReady) ||
       (L7_NULLPTR == userName)    || 
       (L7_NULLPTR == macAddr)     ||
       (L7_NULLPTR == nextMac))
    return L7_FAILURE;

  memcpy(&(entry.macAddr), macAddr, sizeof(L7_enetMacAddr_t));
  SEMA_TAKE(READ);   
  do 
  {
    for( pEntry = avlSearchLVL7(&clientConnStatusTree, &entry, AVL_NEXT);
         L7_NULLPTR != pEntry;
         pEntry = avlSearchLVL7(&clientConnStatusTree, pEntry, AVL_NEXT))
    {
      if (0 == strcmp(userName, pEntry->userName)) 
      {
        memcpy(&nextMac, &(pEntry->macAddr),  sizeof(L7_enetMacAddr_t));      
        rc = L7_SUCCESS;
        break;
      }
    }
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}



/*********************************************************************
*
* @purpose  Find the next connection status entry matching the 
*           given CP ID
*
* @param    cpId_t  cpId  @b{(input)} CP ID to match
* @param    L7_enetMacAddr_t *  macAddr  @b{(input)} macAddr
* @param    L7_enetMacAddr_t *  nextMac  @b{(output)} next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cpdmClientConnStatusByCPIDFind(cpId_t cpId, 
                                       L7_enetMacAddr_t * macAddr, 
                                       L7_enetMacAddr_t * nextMac)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;

  if ((!clientConnStatusTreeReady) || (L7_NULLPTR == macAddr) ||
       (L7_NULLPTR == nextMac))
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    for( pEntry = avlSearchLVL7(&clientConnStatusTree, macAddr, AVL_NEXT);
         L7_NULLPTR != pEntry;
         pEntry = avlSearchLVL7(&clientConnStatusTree, 
                                &(pEntry->macAddr), AVL_NEXT))
    {
      if (cpId == pEntry->cpId)
      {
        memcpy(&nextMac, &(pEntry->macAddr),  sizeof(L7_enetMacAddr_t));      
        rc = L7_SUCCESS;
        break;
      }
    }
  } while(0);
  SEMA_GIVE(READ);   
  return  rc;
}


/*********************************************************************
*
* @purpose  Find first entry that matches given IP address
*
* @param    L7_IP_ADDR_t * ip @b{(input)} IP to search for
* @param    L7_enetMacAddr_t * macAddr  @b{(output)} first
*                                 matching MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments HELPER ROUTINE - do not use outside this file!
*
* @end
*
*********************************************************************/

L7_RC_t cpdmClientConnStatusIPMatchHelper(L7_IP_ADDR_t ip,
            L7_enetMacAddr_t * macAddr)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_enetMacAddr_t local, *pMac = macAddr;

  if (!macAddr)
  {  /* Use local copy, caller doesn't care about output value */
    pMac = &local;
    memset(pMac, 0, sizeof(L7_enetMacAddr_t));
  }
  else
  {
    pMac = macAddr;
  }


  SEMA_TAKE(READ);   
  do 
  {
    /* get first element */
    pEntry = avlSearchLVL7(&clientConnStatusTree, pMac, AVL_NEXT);   

    while(L7_NULLPTR != pEntry) /* while elements left */
    {
      memcpy(pMac, &(pEntry->macAddr), sizeof(L7_enetMacAddr_t));
      if (ip == pEntry->ip)
      {
  rc = L7_SUCCESS;
  break;
      }
      pEntry = avlSearchLVL7(&clientConnStatusTree, pMac, AVL_NEXT);   
    }
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Find first entry that matches given IP address
*
* @param    L7_IP_ADDR_t * ip @b{(input)} IP to search for
* @param    L7_enetMacAddr_t * macAddr  @b{(output)} first
*                                 matching MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr can be NULL; output value is discarded.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusFirstIPMatch(L7_IP_ADDR_t  ip,
           L7_enetMacAddr_t * macAddr)
{
  if (L7_NULLPTR != macAddr)
    memset(macAddr, 0, sizeof(L7_enetMacAddr_t));   

  return cpdmClientConnStatusIPMatchHelper(ip, macAddr);
}

/*********************************************************************
*
* @purpose  Find next entry that matches given IP address
*
* @param    L7_IP_ADDR_t * ip @b{(input)} IP to search for
* @param    L7_enetMacAddr_t * macAddr  @b{(output)} MAC
*                                from which to start
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr can be NULL; output value is discarded.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusNextIPMatch(L7_IP_ADDR_t  ip,
          L7_enetMacAddr_t * macAddr)
{
  return cpdmClientConnStatusIPMatchHelper(ip, macAddr);
}

/********************************************************************* 
* @purpose  Get the next client on a given interface
*
* @param    L7_uint32         intf         @b{(input)} input interface
* @param    L7_enetMacAddr_t  macAddr      @b{(input)} input MAC
* @param    L7_enetMacAddr_t  *nextMacAddr @b{(output)} next MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIntfClientNextGet(L7_uint32 intf, L7_enetMacAddr_t macAddr, 
                                              L7_enetMacAddr_t *nextMacAddr)
{
  L7_uint32 val;

  if (nextMacAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  while (cpdmClientConnStatusNextGet(&macAddr, &macAddr) == L7_SUCCESS)
  {
    if (cpdmClientConnStatusIntfIdGet(&macAddr, &val) == L7_SUCCESS)
    {
      if (val == intf)
      {
        memcpy(nextMacAddr, &macAddr, sizeof(L7_enetMacAddr_t));
        return L7_SUCCESS;       
      }
    }
  }
  return L7_FAILURE;
}

/********************************************************************* 
* @purpose  Get the next client associated to a given CP Configuration
*
* @param    cpId_t            cpId         @b{(input)} CP ID
* @param    L7_enetMacAddr_t  macAddr      @b{(input)} input MAC
* @param    L7_enetMacAddr_t  *nextMacAddr @b{(output)} next MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusCPIDClientNextGet(cpId_t cpId, L7_enetMacAddr_t macAddr, 
                                              L7_enetMacAddr_t *nextMacAddr)
{
  cpId_t val;

  if (nextMacAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  while (cpdmClientConnStatusNextGet(&macAddr, &macAddr) == L7_SUCCESS)
  {
    if (cpdmClientConnStatusCpIdGet(&macAddr, &val) == L7_SUCCESS)
    {
      if (val == cpId)
      {
        memcpy(nextMacAddr, &macAddr, sizeof(L7_enetMacAddr_t));
        return L7_SUCCESS;       
      }
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get ip with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_IP_ADDR_t * ip @b{(output)} value of "ip"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIpGet(L7_enetMacAddr_t * macAddr,
              L7_IP_ADDR_t * ip)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == ip)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *ip = pEntry->ip; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Get MAC address of authenticating switch 
*           with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_enetMacAddr_t * switchMacAddr  @b{(output)} switch MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSwitchMacAddrGet(L7_enetMacAddr_t * macAddr,
                 L7_enetMacAddr_t * switchMacAddr)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == switchMacAddr)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    memcpy(switchMacAddr, &(pEntry->switchMacAddr), sizeof(L7_enetMacAddr_t)); 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}



/*********************************************************************
*
* @purpose  Get ip of authenticating switch 
*           with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_IP_ADDR_t * ip @b{(output)} value of "ip"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSwitchIpGet(L7_enetMacAddr_t * macAddr,
                L7_IP_ADDR_t * switchIp)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == switchIp)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *switchIp = pEntry->switchIp; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Get user id associated with connection
*           with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    uId_t * uid @b{(output)} value of "uid"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only meaningful for locally verified connections. 
*           (Use the userName field for RADIUS or guest verification.)
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUIdGet(L7_enetMacAddr_t * macAddr,
                                   uId_t * uId)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == macAddr) || (L7_NULLPTR == uId))
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *uId = pEntry->uId; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   
  return rc;
}

/*********************************************************************
*
* @purpose  Get which switch is authenticating with existing MAC 
*           for connection status
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    cpdmAuthWhichSwitch_t * whichSwitch @b{(output)} 
*            value of "whichSwitch"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusWhichSwitchGet(L7_enetMacAddr_t * macAddr,
                   cpdmAuthWhichSwitch_t * wswitch)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == wswitch)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *wswitch = pEntry->whichSwitch; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}


/*********************************************************************
*
* @purpose  Get userName with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uchar8 * userName @b{(output)} value of "userName"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUserNameGet(L7_enetMacAddr_t * macAddr,
                L7_uchar8 * userName)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == userName)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    osapiStrncpySafe(userName, pEntry->userName, CP_USER_LOCAL_USERNAME_MAX+1);
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}



/*********************************************************************
*
* @purpose  Get protocolMode with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_LOGIN_TYPE_t * protocolMode @b{(output)} value of "protocolMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusProtocolModeGet(L7_enetMacAddr_t * macAddr,
              L7_LOGIN_TYPE_t * protocolMode)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == protocolMode)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *protocolMode = pEntry->protocolMode; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}


/*********************************************************************
*
* @purpose  Get verifyMode with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    CP_VERIFY_MODE_t * verifyMode @b{(output)} value of "verifyMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusVerifyModeGet(L7_enetMacAddr_t * macAddr,
              CP_VERIFY_MODE_t * verifyMode)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == verifyMode)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *verifyMode = pEntry->verifyMode; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Get userLogoutFlag with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_CP_USER_LOGOUT_FLAG_t * userLogoutFlag @b{(output)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUserLogoutFlagGet(L7_enetMacAddr_t * macAddr,
                  L7_CP_USER_LOGOUT_FLAG_t * userLogoutFlag)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == userLogoutFlag)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *userLogoutFlag = pEntry->userLogoutFlag; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Set userLogoutFlag with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_CP_USER_LOGOUT_FLAG_t userLogoutFlag @b{(output)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUserLogoutFlagSet(L7_enetMacAddr_t * macAddr,
                   L7_CP_USER_LOGOUT_FLAG_t userLogoutFlag)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == macAddr)
    return L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->userLogoutFlag = userLogoutFlag;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}


/*********************************************************************
*
* @purpose  Get intfId with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * intfId @b{(output)} value of "intfId"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIntfIdGet(L7_enetMacAddr_t * macAddr,
              L7_uint32 * intfId)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == intfId)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *intfId = pEntry->intfId; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}



/*********************************************************************
*
* @purpose  Get cpId with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    cpId_t * cpId @b{(output)} value of "cpId"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusCpIdGet(L7_enetMacAddr_t * macAddr,
            cpId_t * cpId)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == cpId)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *cpId = pEntry->cpId; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Get session timeout with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * session timeout @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSessionTimeoutGet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 * timeout)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == timeout)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *timeout = pEntry->limits.sessionTimeout; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Get idle timeout with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * session timeout @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIdleTimeoutGet(L7_enetMacAddr_t * macAddr,
                                           L7_uint32 * timeout)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == timeout)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *timeout = pEntry->limits.idleTimeout; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Get sessionTime with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * sessionTime @b{(output)} value of "sessionTime"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSessionTimeGet(L7_enetMacAddr_t * macAddr,
                                           L7_uint32 * sessionTime)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == sessionTime)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *sessionTime = time(NULL) - pEntry->sessionStart; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}



/*********************************************************************
*
* @purpose  Get connectTime with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * connectTime @b{(output)} value of "connectTime"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIdleTimeGet(L7_enetMacAddr_t * macAddr,
                                        L7_uint32 * idleTime)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == idleTime)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *idleTime = time(NULL) - pEntry->idleStart; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/********************************************************************* 
* @purpose  Get maxBandwidthUp for this client
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client
* @param    L7_uint32 *maxBandwidthUp @b{(output)} max up stream
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxBandwidthUpGet(L7_enetMacAddr_t * macAddr,
                                             L7_uint32 * maxBandwidthUp)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == maxBandwidthUp)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *maxBandwidthUp = pEntry->limits.maxBandwidthUp; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/********************************************************************* 
* @purpose  Get maxBandwidthDown for this client
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client
* @param    L7_uint32 *maxBandwidthDown @b{(output)} max down stream
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxBandwidthDownGet(L7_enetMacAddr_t * macAddr,
                                             L7_uint32 * maxBandwidthDown)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == maxBandwidthDown)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *maxBandwidthDown = pEntry->limits.maxBandwidthDown; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/********************************************************************* 
* @purpose  Get maxInputOctets for this client
*
* @param    L7_enetMacAddr_t  maxAddr @b{(input)} Client
* @param    L7_uint32 *maxInputOctets @b{(output)} Resolved max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxInputOctetsGet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 * maxInputOctets)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == maxInputOctets)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *maxInputOctets = pEntry->limits.maxInputOctets; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/********************************************************************* 
* @purpose  Get maxOutputOctets for this client
*
* @param    L7_enetMacAddr_t  maxAddr @b{(input)} Client
* @param    L7_uint32 *maxOutputOctets @b{(output)} Resolved max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxOutputOctetsGet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 * maxOutputOctets)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == maxOutputOctets)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *maxOutputOctets = pEntry->limits.maxOutputOctets; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/********************************************************************* 
* @purpose  Get maxTotalOctets for this client
*
* @param    L7_enetMacAddr_t  maxAddr @b{(input)} Client
* @param    L7_uint32 *maxTotalOctets @b{(output)} Resolved max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxTotalOctetsGet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 * maxTotalOctets)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == maxTotalOctets)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *maxTotalOctets = pEntry->limits.maxTotalOctets; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Get last refresh timestamp of existing MAC 
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * connectTime @b{(output)} value of "lastRefresh"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusLastRefreshGet(L7_enetMacAddr_t * macAddr,
                                           L7_uint32 * lastRefresh)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == lastRefresh)
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *lastRefresh = pEntry->lastRefresh; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Get session statistics for connection with given MAC
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * bytesTransmitted @b{(output)} bytes tx'ed
* @param    L7_uint32 * bytesReceived    @b{(output)} bytes rx'ed
* @param    L7_uint32 * pktsTransmitted @b{(output)} packets tx'ed
* @param    L7_uint32 * pktsReceived    @b{(output)} packets rx'ed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusStatisticsGet(L7_enetMacAddr_t * macAddr,
                                          L7_uint64 * bytesTransmitted,
                                          L7_uint64 * bytesReceived,
                                          L7_uint64 * pktsTransmitted,
                                          L7_uint64 * pktsReceived)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == bytesTransmitted) ||
      (L7_NULLPTR == bytesReceived)    ||
      (L7_NULLPTR == pktsTransmitted)  ||
      (L7_NULLPTR == pktsReceived))
    return L7_FAILURE;

  SEMA_TAKE(READ);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    *bytesTransmitted = pEntry->cumulative.bytesTransmitted;
    *bytesReceived    = pEntry->cumulative.bytesReceived;
    *pktsTransmitted  = pEntry->cumulative.packetsTransmitted;
    *pktsReceived     = pEntry->cumulative.packetsReceived;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return  rc;
}
 

/*********************************************************************
*
* @purpose  Set authenticating swith MAC address 
*           with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_enetMacAddr_t * switchMacAddr  @b{(input)} switch MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSwitchMacAddrSet(L7_enetMacAddr_t * macAddr,
                                             L7_enetMacAddr_t * switchMacAddr)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == switchMacAddr)
    return L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    memcpy(&(pEntry->switchMacAddr), switchMacAddr, sizeof(L7_enetMacAddr_t)); 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Set switch ip with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_IP_ADDR_t  ip @b{(input)} new value of "switchIp"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSwitchIpSet(L7_enetMacAddr_t * macAddr,
                                        L7_IP_ADDR_t  switchIp)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == macAddr)
    return L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->switchIp = switchIp; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}


/*********************************************************************
*
* @purpose  Set which switch is authenticating with existing MAC 
*           for connection status
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    cpdmAuthWhichSwitch_t  whichSwitch @b{(input)} 
*            value of "whichSwitch"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusWhichSwitchSet(L7_enetMacAddr_t * macAddr,
                   cpdmAuthWhichSwitch_t wswitch)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->whichSwitch = wswitch ;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Set uid  for connection status
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    uid_t   uid @b{(input)} user id to use
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only has meaning for locally verified connections.
*           TODO: We really shouldn't export this function as an
*           API.  Nuke this when we restructure the code again.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUIdSet(L7_enetMacAddr_t * macAddr,
                                   uId_t uId)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->uId = uId;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}


/*********************************************************************
*
* @purpose  Set protocolMode with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_LOGIN_TYPE_t  protocolMode @b{(input)} new value of "protocolMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusProtocolModeSet(L7_enetMacAddr_t * macAddr,
                                            L7_LOGIN_TYPE_t  protocolMode)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->protocolMode = protocolMode; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}


/*********************************************************************
*
* @purpose  Set verifyMode with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    CP_VERIFY_MODE_t  verifyMode @b{(input)} new value of "verifyMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusVerifyModeSet(L7_enetMacAddr_t * macAddr,
                                          CP_VERIFY_MODE_t  verifyMode)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->verifyMode = verifyMode; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}


/*********************************************************************
*
* @purpose  Set cpId with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    cpId_t  cpId @b{(input)} new value of "cpId"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusCpIdSet(L7_enetMacAddr_t * macAddr,
                                    cpId_t  cpId)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->cpId = cpId; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Set session timeout with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     timeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSessionTimeoutSet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 timeout)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->limits.sessionTimeout = timeout; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Set idle timeout with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     timeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIdleTimeoutSet(L7_enetMacAddr_t * macAddr,
                                           L7_uint32 timeout)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->limits.idleTimeout = timeout; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Set max input octets with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     octets @b{(input)} max bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxInputOctetsSet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 octets)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->limits.maxInputOctets = octets; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Set max output octets with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     octets @b{(input)} max bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxOutputOctetsSet(L7_enetMacAddr_t * macAddr,
                                               L7_uint32 octets)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->limits.maxOutputOctets = octets; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
*
* @purpose  Set max total octets with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     octets @b{(input)} max bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxTotalOctetsSet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 octets)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->limits.maxTotalOctets = octets; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
*
* @purpose  reset idle time with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32  connectTime @b{(input)} new value of "connectTime"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIdleStartReset(L7_enetMacAddr_t * macAddr)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->idleStart = time(NULL);
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}

/*********************************************************************
*
* @purpose  reset last refresh time for existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusLastRefreshReset(L7_enetMacAddr_t * macAddr)
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);   
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry) 
      break;
    pEntry->lastRefresh = time(NULL);
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return  rc;
}


/*********************************************************************
*
* @purpose  Update session statistics for connection with given MAC
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_BOOL bPreviousStats @b{(input)} act as if we've never
*           collected statistics (useful for roaming)
* @param    txRxCount_t * txRx  @b{(output)} current statistics set
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  txRx can be NULL if new stats are not desired
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusStatisticsUpdate(L7_enetMacAddr_t * macAddr,
             txRxCount_t * txRx, L7_BOOL bNoPreviousStats )
{
  cpdmClientConnStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum = 0;
  txRxCount_t  txRxNow; 
  time_t now;
  L7_BOOL changed = L7_FALSE;

  SEMA_TAKE(READ);   
  pEntry = cpdmFindClientConnStatus(macAddr);
  if (L7_NULLPTR != pEntry) 
    /*intIfNum = pEntry->intfId;*/
  {
    if (pEntry->whichSwitch == CP_AUTH_ON_PEER_SWITCH)
    {
      SEMA_GIVE(READ);
      return L7_SUCCESS;
    }
    else
    {
      intIfNum = pEntry->intfId;
    }
  }
  SEMA_GIVE(READ);   

  if ((0 == intIfNum) ||
      (L7_SUCCESS != cpimGetStatistics(intIfNum, macAddr, &txRxNow)))
  {
    return rc; /* fail */
  }

  now = time(NULL);
  SEMA_TAKE(WRITE);   /* OK, we have statistics to refresh */
  do 
  {
    pEntry = cpdmFindClientConnStatus(macAddr);
    if (L7_NULLPTR == pEntry)      
      break;

    if (L7_TRUE == bNoPreviousStats)
    {
      memset(&(pEntry->previous), 0, sizeof(txRxCount_t));
    }
    else
    {
      memcpy(&(pEntry->previous), &(pEntry->current), sizeof(txRxCount_t));
    }
    memcpy(&(pEntry->current), &(txRxNow), sizeof(txRxCount_t));
    pEntry->lastRefresh = now;

    if (0 != memcmp(&(pEntry->previous), &(pEntry->current), sizeof(txRxCount_t)))
    {
      /* Now ensure that current counts really are greater than our previous counts.
       * If not, let previous values == current ones, since we don't really know
       * what else to do in this case.
       */
      do 
      {
        if (pEntry->previous.bytesTransmitted > pEntry->current.bytesTransmitted)
          break;
        if (pEntry->previous.bytesReceived > pEntry->current.bytesReceived)
          break;
        if (pEntry->previous.packetsTransmitted > pEntry->current.packetsTransmitted)
          break;
        if (pEntry->previous.packetsReceived > pEntry->current.packetsReceived)
          break;
        changed = L7_TRUE; /* Ok, current counts really are greater than previous */
      } while (0);

      if (L7_FALSE == changed)
      {
         memcpy(&(pEntry->current), &(pEntry->previous), sizeof(txRxCount_t));           
      }
    }


    if ((L7_FALSE == bNoPreviousStats) && changed)
    {
      pEntry->idleStart = now;
#if 0
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
        "....RESET IDLE PERIOD");
#endif
    }    
    /* If we have no "previous" stats (according to the caller of this function),
     * we roamed.. or at least we're saying that we can't trust the "previous" 
     * counters.  So don't reset the start time of our current idle period
     * in this case.
     */

    if (changed)
    {
      pEntry->cumulative.bytesTransmitted += 
        (pEntry->current.bytesTransmitted - pEntry->previous.bytesTransmitted);    
      pEntry->cumulative.bytesReceived += 
        (pEntry->current.bytesReceived - pEntry->previous.bytesReceived);    
      pEntry->cumulative.packetsTransmitted += 
        (pEntry->current.packetsTransmitted - pEntry->previous.packetsTransmitted);    
      pEntry->cumulative.packetsReceived += 
        (pEntry->current.packetsReceived - pEntry->previous.packetsReceived);    

#if 0  /* enable this only if you really like numbers */
        L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
          "bytes TXed = %u, RXed = %u; pkts TXed = %u, RXed = %u\n",
          (L7_uint32) pEntry->cumulative.bytesTransmitted, 
          (L7_uint32) pEntry->cumulative.bytesReceived,
          (L7_uint32) pEntry->cumulative.packetsTransmitted, 
          (L7_uint32) pEntry->cumulative.packetsReceived);
#endif

    }

    if (L7_NULLPTR != txRx)
    {
      memcpy(txRx, &(pEntry->cumulative), sizeof(txRxCount_t));
    }


    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   
  return  rc;
}


/*********************************************************************
*
* @purpose  Return if CAC status tree is full
*
* @param    void
*
* @returns  L7_TRUE if full, L7_FALSE if not
*
* @comments none
*
* @end
*
*********************************************************************/
L7_BOOL cpdmClientConnStatusConnectFull(void)
{
  L7_BOOL rc = L7_TRUE;
  SEMA_TAKE(READ);   
  rc = (avlTreeCount(&clientConnStatusTree) == clientConnStatusTreeMaxSize);
  SEMA_GIVE(READ);   
  return rc;
}



/*********************************************************************
*
* @purpose  Monitor and deauthenticate sessions 
*
* @param    void
*
* @returns  L7_TRUE if full, L7_FALSE if not
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmMonitorSessions(void)
{
  cpdmMonitorSessions_private();
}


/*********************************************************************
* @purpose  Get the total number of switch authenticated clients
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value 
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusTotalGet(L7_ushort16 *value)
{
  if (!clientConnStatusTreeReady || !value)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(READ);
  *value = clientConnStatusTree.count;
  SEMA_GIVE(READ);

  return L7_SUCCESS;
}

