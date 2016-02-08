/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename acl_mac_api.c
*
* @purpose Contains MAC Access Control List APIs
*
* @component Access Control List
*
* @comments Many APIs in this file take a read/write lock. Control is 
*           then passed into one or more implementation functions in
*           acl.c or acl_mac.c.  The lock is always managed at the API level
*           and assumed to be already taken, if necessary, in the 
*           implementation functions.
*
* @create 08/31/2004
*
* @author gpaussa
*
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "osapi.h"
#include "nimapi.h"
#include "acl_api.h"
#include "usmdb_qos_acl_api.h"
#include "acl.h"

extern aclCfgFileData_t  *aclCfgFileData;
extern osapiRWLock_t     aclRwLock;
extern aclCnfgrState_t   aclCnfgrState;

static void    aclMacNotify(L7_uint32 aclIndex, aclEvent_t event, L7_uchar8 *aclOldName);
static L7_RC_t aclMacRuleAcquireListUnderReadLock(L7_uint32 aclIndex, L7_uint32 rulenum,
                                                  void **acl_ptr, aclMacRuleParms_t **rule_ptr);
static L7_RC_t aclMacRuleAcquireListUnderWriteLock(L7_uint32 aclIndex, L7_uint32 rulenum,
                                                   void **acl_ptr, aclMacRuleParms_t **rule_ptr);


/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new MAC ACL in the access list table
*
* @param    *next       @b{(output)} access list index next value
*
* @returns  L7_SUCCESS
* @returns  L7_TABLE_IS_FULL  ACL table is currently full
* @returns  L7_FAILURE        all other failures
*
* @comments The index value remains free until used in a subsequent ACL create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new ACL
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, so they must call this
*           function again to get a new value.
*
* @comments Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t aclMacIndexNext(L7_uint32 *next)
{
  L7_RC_t       rc = L7_SUCCESS;

  /* check inputs */
  if (next == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  do                                            /* single-pass control loop */
  {
    /* always output a value of 0 if maximum number of ACLs are already 
     * configured
     */
    if (aclMacImpIsTableFull() == L7_TRUE)
    {
      /* table full */
      rc = L7_TABLE_IS_FULL;
      break;
    }

    aclMacImpIndexNextRead(next);

  } while (0);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  if (rc != L7_SUCCESS)
    *next = 0;

  return rc;
}

/*************************************************************************
*
* @purpose  Retrieve the min/max index values allowed for the MAC ACL table 
*
* @param    *pMin       @b{(output)} Pointer to index min output location
* @param    *pMax       @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is purely informational.  The aclMacIndexNext function
*           must be used to obtain the current index value.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexMinMaxGet(L7_uint32 *pMin, L7_uint32 *pMax)
{
  L7_RC_t       rc;

  /* check inputs */
  if ((pMin == L7_NULLPTR) || (pMax == L7_NULLPTR))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = aclMacImpIndexMinMaxGet(pMin, pMax);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Get the first MAC access list created in the system.
*
* @param    *aclIndex   @b{(output)} first access list index value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no MAC access lists exist
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexGetFirst(L7_uint32 *aclIndex)
{
  return aclMacIndexGetNext(0, aclIndex);
} 

/*********************************************************************
*
* @purpose  Get the next sequential MAC access list index
*
* @param    aclIndex    @b{(input)}  access list index to begin search
* @param    *next       @b{(output)} next access list index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no MAC access lists exist, or aclIndex is the last
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexGetNext(L7_uint32 aclIndex, L7_uint32 *next)
{
  L7_RC_t rc;

  if (next == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacImpIndexGetNext(aclIndex, next);
  
  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if aclIndex is a valid, configured MAC access list
*
* @param    aclIndex    @b{(input)}  access list index
*
* @returns  L7_SUCCESS  MAC access list is valid
* @returns  L7_FAILURE  MAC access list not valid
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexCheckValid(L7_uint32 aclIndex)
{
  L7_RC_t rc;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacImpIndexCheckValid(aclIndex);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the MAC ACL index is within proper range
*
* @param    aclIndex    @b{(input)}  access list index to begin search
*
* @returns  L7_SUCCESS  MAC access list index is in range
* @returns  L7_FAILURE  MAC access list index out of range
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexRangeCheck(L7_uint32 aclIndex)
{
  /* no need for read/write lock control here */
  return aclMacImpIndexRangeCheck(aclIndex);
}

/*********************************************************************
*
* @purpose  To create a new MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS        access list created
* @returns  L7_FAILURE        invalid acl identifier, all other failures
* @returns  L7_ERROR          access list already exists
* @returns  L7_TABLE_IS_FULL  maximum number of ACLs already created
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacCreate(L7_uint32 aclIndex)
{
  void          *acl_ptr;

  /* check inputs */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr != L7_NULLPTR)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* check if max number of ACLs of any type have been configured */
  if (aclImpIsTotalListsAtMax() == L7_TRUE)
  {
      (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      return L7_TABLE_IS_FULL;
  }

  /* check if maximum number of ACLs are already configured */
  if (aclMacImpIsTableFull() == L7_TRUE)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_TABLE_IS_FULL;
  }

  if (aclMacCreateACL(aclIndex) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* no need to update the driver for a create */

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  aclMacNotify(aclIndex, ACL_EVENT_CREATE, L7_NULLPTR);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To delete an existing MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
* @param    force       @b{(input)} force deletion regardless of whether in use
*
* @returns  L7_SUCCESS  access list deleted
* @returns  L7_ERROR    access list does not exist
* @returns  L7_FAILURE  invalid access list index, all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacDelete(L7_uint32 aclIndex, L7_BOOL force)
{
  void              *acl_ptr;
  aclMacStructure_t *ptr;
  L7_uchar8         aclName[L7_ACL_NAME_LEN_MAX+1];

  /* check inputs */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if ((force != L7_FALSE) && (force != L7_TRUE))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr == L7_NULLPTR)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* save old name for notification event */
  ptr = (aclMacStructure_t *)acl_ptr;
  osapiStrncpySafe(aclName, ptr->aclName, sizeof(aclName));

  /* make sure list not currently in use (unless force flag is specified) */
  if ((force != L7_TRUE) &&
      (aclImpDeleteApprovalQuery(aclIndex) != L7_SUCCESS))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  if (aclMacDeleteACL(acl_ptr) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* no update needed, done by preceding delete function */

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  aclMacNotify(aclIndex, ACL_EVENT_DELETE, aclName);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To add the name to this MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
* @param    *name       @b{(input)} access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list index, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameAdd(L7_uint32 aclIndex, L7_uchar8 *name)
{
  void              *acl_ptr;
  aclMacStructure_t *ptr;
  L7_uchar8         oldName[L7_ACL_NAME_LEN_MAX+1];

  /* check inputs */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclMacNameStringCheck(name) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr == L7_NULLPTR)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* return successfully if existing ACL name is same */
  ptr = (aclMacStructure_t *)acl_ptr;
  if (osapiStrncmp(ptr->aclName, name, sizeof(ptr->aclName)) == 0)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* save old name for notification event */
  osapiStrncpySafe(oldName, ptr->aclName, sizeof(oldName));

  /* update app */
  if (aclMacImpNameAdd(acl_ptr, name) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }
  
  /* no need to update the driver for a name change */

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification when first assigning a name to the ACL */
  if (strlen(oldName) > 0)
  {
    aclMacNotify(aclIndex, ACL_EVENT_RENAME, oldName);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To change the name of an existing MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
* @param    *newname    @b{(input)} new access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list index, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
* @returns  L7_ALREAD_CONFIGURED  name is in use by another ACL
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameChange(L7_uint32 aclIndex, L7_uchar8 *newname)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacStructure_t *ptr;
  L7_uint32         newIndex;
  L7_uchar8         oldName[L7_ACL_NAME_LEN_MAX+1];

  /* check inputs */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclMacNameStringCheck(newname) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr == L7_NULLPTR)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* return successfully if existing ACL name is same */
  ptr = (aclMacStructure_t *)acl_ptr;
  if (osapiStrncmp(ptr->aclName, newname, sizeof(ptr->aclName)) == 0)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* check for duplicate name already in use for a different ACL */
  rc = aclMacImpNameToIndexGet(newname, &newIndex);
  if ((rc == L7_SUCCESS) && (newIndex != aclIndex))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ALREADY_CONFIGURED;
  }
  
  /* save old name for notification event */
  osapiStrncpySafe(oldName, ptr->aclName, sizeof(oldName));

  /* update app */
  if (aclMacImpNameAdd(acl_ptr, newname) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }
  
  /* no need to update the driver for a name change */

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification when first assigning a name to the ACL */
  if (strlen(oldName) > 0)
  {
    aclMacNotify(aclIndex, ACL_EVENT_RENAME, oldName);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the name of this MAC access list.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    *name       @b{(output)} access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid access list index, name ptr, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Caller must provide a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameGet(L7_uint32 aclIndex, L7_uchar8 *name)
{
  void          *acl_ptr;

  /* check inputs */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if (name == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr == L7_NULLPTR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }
  
  /* get name string from ACL entry */
  aclMacImpNameGet(acl_ptr, name);
  
  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Checks if the MAC ACL name is a valid string
*
* @param    *name       @b{(input)} MAC access-list name string
*
* @returns  L7_SUCCESS  valid MAC ACL name string
* @returns  L7_FAILURE  invalid MAC ACL name string
*
* @comments This function only checks the name string syntax for a MAC ACL. 
*           It does not check if an MAC ACL currently contains this name.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameStringCheck(L7_uchar8 *name)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     nameLen;

  /* check inputs */
  if (name == L7_NULLPTR)
    return L7_FAILURE;

  /* no need for read/write lock control here */

  nameLen = (L7_uint32)strlen((char *)name);

  if ((nameLen < L7_ACL_NAME_LEN_MIN) || 
      (nameLen > L7_ACL_NAME_LEN_MAX))
    return L7_FAILURE;

  if (aclImpNameIsAlphanumeric(name) == L7_TRUE)
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  To get the index of a MAC access list, given its name.
*
* @param    *name      @b{(input)} access list name
* @param    *aclIndex  @b{(output)} access list index
*                                            
* @returns  L7_SUCCESS  MAC access list index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameToIndex(L7_uchar8 *name, L7_uint32 *aclIndex)
{
  L7_RC_t       rc;

  /* check inputs */
  if ((name == L7_NULLPTR) || (strlen((char *)name) > L7_ACL_NAME_LEN_MAX))
    return L7_FAILURE;
  if (aclIndex == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* search for access list containing this name */
  rc = aclMacImpNameToIndexGet(name, aclIndex);
  
  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  To add an action to a MAC access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    action      @b{(input)} action type (permit/deny)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  invalid acl identifier, all other failures
* @returns  L7_ERROR    access-list does not exist
* @returns  L7_REQUEST_DENIED hardware update failed
* @returns  L7_TABLE_IS_FULL, if storage unavailable for rule
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleActionAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 action)
{
  L7_BOOL           ruleAlreadyExists = L7_FALSE;
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config = 0, saved_action = 0;
  L7_uchar8         aclName[L7_ACL_NAME_LEN_MAX+1];

  /* check inputs */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclMacImpRuleNumRangeCheck(aclIndex, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (action >= L7_ACL_ACTION_TOTAL)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr == L7_NULLPTR)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* the MAC ACL name must be set before allowing any rules to be created */
  aclMacImpNameGet(acl_ptr, aclName);
  if (aclMacNameStringCheck(aclName) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }
  
  /* make note if rule already exists or not (for rollback operation later) */
  if (aclMacRuleGet(aclIndex, rulenum, &p) == L7_SUCCESS)
  {
    /* return successfully if action is same */
    if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_ACTION) == L7_TRUE) &&
        (p->action == action))
    {
      (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      return L7_SUCCESS;
    }

    /* rule already exists -- save current info for rollback */
    ruleAlreadyExists = L7_TRUE;
    saved_config = p->configMask;
    saved_action = p->action;
  }

  /* update app */
  if ((rc = aclMacActionAdd(acl_ptr, rulenum, action)) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return rc;
  }
  
  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    if (ruleAlreadyExists == L7_TRUE)
    {
      /* restore previous action for an existing rule */
      p->configMask = saved_config;
      p->action = saved_action;
      (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    }
    else
    {
      /* delete the rule from the app, since it was just created here */
      (void)aclMacRemoveRule(acl_ptr, rulenum); /* ignore rc here */
    }
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To remove a rule from an existing access list.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR    access-list or rule number does not exist
* @returns  L7_FAILURE  invalid access-list index or rule number
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleRemove(L7_uint32 aclIndex, L7_uint32 rulenum)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* update app */
  (void)aclMacRemoveRule(acl_ptr, rulenum);     /* ignore rc and keep going */

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);
  if (rc != L7_SUCCESS)
    rc = L7_REQUEST_DENIED;

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  return rc;
}

/*********************************************************************
*
* @purpose  To add the assigned queue id for an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    queueId     @b{(input)} assigned queue ID value
*
* @returns  L7_SUCCESS  assigned queue id added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleAssignQueueIdAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 queueId)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_queueid;
  L7_uint32         saved_config;

  /* check inputs */
  if ((queueId < L7_ACL_ASSIGN_QUEUE_ID_MIN) || 
      (queueId > L7_ACL_ASSIGN_QUEUE_ID_MAX))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_ASSIGN_QUEUEID) == L7_TRUE) &&
      (p->assignQueueId == queueId))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* save */
  saved_config = p->configMask;
  saved_queueid = p->assignQueueId;

  /* update app */
  if (aclMacAssignQueueIdAdd(acl_ptr, rulenum, queueId) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->assignQueueId = saved_queueid;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_ATTR_ONLY_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the redirect interface config id for an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(input)} redirected interface config id
*                                            
* @returns  L7_SUCCESS  redirect interface config id added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  invalid redirect interface or other failure
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleRedirectConfigIdAdd(L7_uint32 aclIndex, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  nimConfigID_t     saved_configId;
  L7_uint32         saved_config;
  L7_uint32         intIfNum;

  /* only allow redirection to supported interface types
   *
   * NOTE: Skip this checking when not in READY state, since the interfaces
   *       have not been created yet during phase 3 config apply.
   */
  if (ACL_IS_READY == L7_TRUE)
  {
    /* get intIfNum to do some validity checking */
    if ((nimIntIfFromConfigIDGet(configId, &intIfNum) != L7_SUCCESS) ||
        (aclIsValidIntf(intIfNum) != L7_TRUE))
    {
      return L7_FAILURE;
    }
  }

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_REDIRECT_INTF) == L7_TRUE) &&
      (NIM_CONFIG_ID_IS_EQUAL(&p->redirectConfigId, configId) == L7_TRUE))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* return unsuccessfully if rule contains a mirror attribute (mutually-exclusive) */
  if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_MIRROR_INTF) == L7_TRUE)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* save */
  saved_config = p->configMask;
  NIM_CONFIG_ID_COPY(&saved_configId, &p->redirectConfigId);

  /* update app */
  if (aclMacRedirectConfigIdAdd(acl_ptr, rulenum, configId) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    NIM_CONFIG_ID_COPY(&p->redirectConfigId, &saved_configId);
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_ATTR_ONLY_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the mirror interface config id for an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(input)} mirrored interface config id
*                                            
* @returns  L7_SUCCESS  mirror interface config id added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  invalid mirror interface or other failure
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleMirrorConfigIdAdd(L7_uint32 aclIndex, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  nimConfigID_t     saved_configId;
  L7_uint32         saved_config;
  L7_uint32         intIfNum;

  /* only allow mirroring to supported interface types
   *
   * NOTE: Skip this checking when not in READY state, since the interfaces
   *       have not been created yet during phase 3 config apply.
   */
  if (ACL_IS_READY == L7_TRUE)
  {
    /* get intIfNum to do some validity checking */
    if ((nimIntIfFromConfigIDGet(configId, &intIfNum) != L7_SUCCESS) ||
        (aclIsValidIntf(intIfNum) != L7_TRUE))
      return L7_FAILURE;
  }

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_MIRROR_INTF) == L7_TRUE) &&
      (NIM_CONFIG_ID_IS_EQUAL(&p->mirrorConfigId, configId) == L7_TRUE))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* return unsuccessfully if rule contains a redirect attribute (mutually-exclusive) */
  if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_REDIRECT_INTF) == L7_TRUE)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* save */
  saved_config = p->configMask;
  NIM_CONFIG_ID_COPY(&saved_configId, &p->mirrorConfigId);

  /* update app */
  if (aclMacMirrorConfigIdAdd(acl_ptr, rulenum, configId) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    NIM_CONFIG_ID_COPY(&p->mirrorConfigId, &saved_configId);
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_ATTR_ONLY_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the logging flag for an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    logFlag     @b{(input)} rule logging flag value
*
* @returns  L7_SUCCESS  logging flag is added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid flag value or other failure
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleLoggingAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_BOOL logFlag)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_BOOL           saved_logging;
  L7_uint32         saved_config;

  /* check inputs */
  if ((logFlag != L7_FALSE) && (logFlag != L7_TRUE))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_LOGGING) == L7_TRUE) &&
      (p->logging == logFlag))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* save */
  saved_config = p->configMask;
  saved_logging = p->logging;

  /* update app */
  if (aclMacLoggingAdd(acl_ptr, rulenum, logFlag) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->logging = saved_logging;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_ATTR_ONLY_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the class of service (cos) value.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    cos         @b{(input)} class of service (cos) value
*
* @returns  L7_SUCCESS  class of service value added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCosAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 cos)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_uchar8         saved_cos;

  if ((cos < L7_ACL_MIN_COS) || (cos > L7_ACL_MAX_COS))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_COS) == L7_TRUE) &&
      (p->cos == cos))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* save */
  saved_config = p->configMask;
  saved_cos = p->cos;

  /* update app */
  if (aclMacCosAdd(acl_ptr, rulenum, cos) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->cos = saved_cos;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the secondary class of service (cos2) value.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    cos         @b{(input)} secondary class of service (cos2) value
*
* @returns  L7_SUCCESS  secondary class of service value added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCos2Add(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 cos2)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_uchar8         saved_cos2;

  /* check inputs */
  if ((cos2 < L7_ACL_MIN_COS) || (cos2 > L7_ACL_MAX_COS))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_COS2) == L7_TRUE) &&
      (p->cos2 == cos2))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* save */
  saved_config = p->configMask;
  saved_cos2 = p->cos2;

  /* update app */
  if (aclMacCos2Add(acl_ptr, rulenum, cos2) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->cos2 = saved_cos2;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the destination MAC address and mask.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *dstmac     @b{(input)} destination MAC address
* @param    *dstmask    @b{(input)} destination MAC mask
*
* @returns  L7_SUCCESS  destination MAC added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleDstMacAdd(L7_uint32 aclIndex, L7_uint32 rulenum,
                            L7_uchar8 *dstmac, L7_uchar8 *dstmask)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_uchar8         saved_dstmac[L7_MAC_ADDR_LEN];
  L7_uchar8         saved_dstmask[L7_MAC_ADDR_LEN];

  /* check inputs */
  if ((dstmac == L7_NULLPTR) || (dstmask == L7_NULLPTR))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_DSTMAC) == L7_TRUE) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_DSTMAC_MASK) == L7_TRUE))
  {
    if ((memcmp(p->dstMac, dstmac, (size_t)L7_MAC_ADDR_LEN) == 0) &&
        (memcmp(p->dstMacMask, dstmask, (size_t)L7_MAC_ADDR_LEN) == 0))
    {
      (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      return L7_SUCCESS;
    }
  }

  /* save */
  saved_config = p->configMask;
  memcpy(saved_dstmac, p->dstMac, (size_t)L7_MAC_ADDR_LEN);
  memcpy(saved_dstmask, p->dstMacMask, (size_t)L7_MAC_ADDR_LEN);

  /* update app */
  if (aclMacDstMacAdd(acl_ptr, rulenum, dstmac, dstmask) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    memcpy(p->dstMac, saved_dstmac, sizeof(p->dstMac));
    memcpy(p->dstMacMask, saved_dstmask, sizeof(p->dstMacMask));
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the Ethertype keyword identifier.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    keyid       @b{(input)} Ethertype keyword identifier
* @param    value       @b{(input)} Ethertype custom value
*
* @returns  L7_SUCCESS  Ethertype keyword identifier added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments The value parameter is only meaningful when keyid is set to 
*           L7_QOS_ETYPE_KEYID_CUSTOM.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleEtypeKeyAdd(L7_uint32 aclIndex, L7_uint32 rulenum, 
                              L7_QOS_ETYPE_KEYID_t keyid, L7_uint32 value)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_ushort16       saved_keyid, saved_value;

  /* check inputs */
  if ((keyid <= L7_QOS_ETYPE_KEYID_NONE) || (keyid >= L7_QOS_ETYPE_KEYID_TOTAL))
    return L7_FAILURE;

  if ((keyid == L7_QOS_ETYPE_KEYID_CUSTOM) && (value != L7_QOS_ETYPE_VALUE_UNUSED))
  {
    /* only check value range if 'custom' etype is specified */
    if ((value < L7_QOS_ETYPE_CUSTOM_VAL_MIN) || 
        (value > L7_QOS_ETYPE_CUSTOM_VAL_MAX))
      return L7_FAILURE;
  }

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_ETYPE_KEYID) == L7_TRUE) &&
      (p->etypeKeyId == keyid))
  {
    /* only compare Etype value if this is a custom keyid */
    if (p->etypeKeyId != L7_QOS_ETYPE_KEYID_CUSTOM)
    {
      (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      return L7_SUCCESS;
    }
    if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_ETYPE_VALUE) == L7_TRUE) &&
        (p->etypeValue == (L7_ushort16)value))
    {
      (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      return L7_SUCCESS;
    }
  }

  /* save */
  saved_config = p->configMask;
  saved_keyid = p->etypeKeyId;
  saved_value = p->etypeValue;

  /* update app */
  if (aclMacEtypeKeyAdd(acl_ptr, rulenum, keyid, value) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->etypeKeyId = saved_keyid;
    p->etypeValue = saved_value;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Set the match condition whereby all packets match for a
*           MAC ACL rule
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    every       @b{(input)} 'match every' flag value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS  'match every' condition added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleEveryAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_BOOL every)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_uchar8         saved_every;

  /* check inputs */
  if ((every != L7_TRUE) && (every != L7_FALSE))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_EVERY) == L7_TRUE) &&
      (p->every == every))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* save */
  saved_config = p->configMask;
  saved_every = p->every;

  /* update app */
  if (aclMacEveryAdd(acl_ptr, rulenum, every) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->every = saved_every;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the source MAC address and mask.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *srcmac     @b{(input)} source MAC address
* @param    *srcmask    @b{(input)} source MAC mask
*
* @returns  L7_SUCCESS  source MAC added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleSrcMacAdd(L7_uint32 aclIndex, L7_uint32 rulenum,
                            L7_uchar8 *srcmac, L7_uchar8 *srcmask)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_uchar8         saved_srcmac[L7_MAC_ADDR_LEN];
  L7_uchar8         saved_srcmask[L7_MAC_ADDR_LEN];

  /* check inputs */
  if ((srcmac == L7_NULLPTR) || (srcmask == L7_NULLPTR))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_SRCMAC) == L7_TRUE) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_SRCMAC_MASK) == L7_TRUE))
  {
    if ((memcmp(p->srcMac, srcmac, (size_t)L7_MAC_ADDR_LEN) == 0) &&
        (memcmp(p->srcMacMask, srcmask, (size_t)L7_MAC_ADDR_LEN) == 0))
    {
      (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      return L7_SUCCESS;
    }
  }

  /* save */
  saved_config = p->configMask;
  memcpy(saved_srcmac, p->srcMac, (size_t)L7_MAC_ADDR_LEN);
  memcpy(saved_srcmask, p->srcMacMask, (size_t)L7_MAC_ADDR_LEN);

  /* update app */
  if (aclMacSrcMacAdd(acl_ptr, rulenum, srcmac, srcmask) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    memcpy(p->srcMac, saved_srcmac, sizeof(p->srcMac));
    memcpy(p->srcMacMask, saved_srcmask, sizeof(p->srcMacMask));
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add a single VLAN ID.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan        @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanIdAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 vlan)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_ushort16       saved_vlan;

  /* check inputs */
  if ((vlan < L7_ACL_MIN_VLAN_ID) || (vlan > L7_ACL_MAX_VLAN_ID))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_VLANID) == L7_TRUE) &&
      (p->vlanId == (L7_ushort16)vlan))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* save */
  saved_config = p->configMask;
  saved_vlan = p->vlanId;

  /* update app */
  if (aclMacVlanIdAdd(acl_ptr, rulenum, vlan) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->vlanId = saved_vlan;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add a VLAN ID range.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    startvlan   @b{(input)} starting VLAN ID
* @param    endvlan     @b{(input)} ending VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID range added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanIdRangeAdd(L7_uint32 aclIndex, L7_uint32 rulenum, 
                                 L7_uint32 startvlan, L7_uint32 endvlan)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_ushort16       saved_startvlan, saved_endvlan;

  /* check inputs */
  if ((startvlan < L7_ACL_MIN_VLAN_ID) || (startvlan > L7_ACL_MAX_VLAN_ID))
    return L7_FAILURE;
  if ((endvlan < L7_ACL_MIN_VLAN_ID) || (endvlan > L7_ACL_MAX_VLAN_ID))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_VLANID_START) == L7_TRUE) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_VLANID_END) == L7_TRUE))
  {
    if ((p->vlanIdStart == (L7_ushort16)startvlan) &&
        (p->vlanIdEnd == (L7_ushort16)endvlan))
    {
      (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      return L7_SUCCESS;
    }
  }

  /* save */
  saved_config = p->configMask;
  saved_startvlan = p->vlanIdStart;
  saved_endvlan = p->vlanIdEnd;

  /* update app */
  if (aclMacVlanIdRangeAdd(acl_ptr, rulenum, startvlan, endvlan) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->vlanIdStart = saved_startvlan;
    p->vlanIdEnd = saved_endvlan;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add a single Secondary VLAN ID (vlan2).
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan2       @b{(input)} Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanId2Add(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 vlan2)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_ushort16       saved_vlan2;

  /* check inputs */
  if ((vlan2 < L7_ACL_MIN_VLAN_ID) || (vlan2 > L7_ACL_MAX_VLAN_ID))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_VLANID2) == L7_TRUE) &&
      (p->vlanId2 == (L7_ushort16)vlan2))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* save */
  saved_config = p->configMask;
  saved_vlan2 = p->vlanId2;

  /* update app */
  if (aclMacVlanId2Add(acl_ptr, rulenum, vlan2) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->vlanId2 = saved_vlan2;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add a Secondary VLAN ID (vlan2) range.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    startvlan2  @b{(input)} starting Secondary VLAN ID
* @param    endvlan2    @b{(input)} ending Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID range added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanId2RangeAdd(L7_uint32 aclIndex, L7_uint32 rulenum, 
                                  L7_uint32 startvlan2, L7_uint32 endvlan2)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         saved_config;
  L7_ushort16       saved_startvlan2, saved_endvlan2;

  /* check inputs */
  if ((startvlan2 < L7_ACL_MIN_VLAN_ID) || (startvlan2 > L7_ACL_MAX_VLAN_ID))
    return L7_FAILURE;
  if ((endvlan2 < L7_ACL_MIN_VLAN_ID) || (endvlan2 > L7_ACL_MAX_VLAN_ID))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderWriteLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  /* return successfully if existing rule value is same */
  if ((ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_VLANID2_START) == L7_TRUE) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_VLANID2_END) == L7_TRUE))
  {
    if ((p->vlanId2Start == (L7_ushort16)startvlan2) &&
        (p->vlanId2End == (L7_ushort16)endvlan2))
    {
      (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      return L7_SUCCESS;
    }
  }

  /* save */
  saved_config = p->configMask;
  saved_startvlan2 = p->vlanId2Start;
  saved_endvlan2 = p->vlanId2End;

  /* update app */
  if (aclMacVlanId2RangeAdd(acl_ptr, rulenum, startvlan2, endvlan2) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclMacUpdate(aclIndex, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->vlanId2Start = saved_startvlan2;
    p->vlanId2End = saved_endvlan2;
    (void)aclMacUpdate(aclIndex, ACL_MODIFY); /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclMacNotify(aclIndex, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add a MAC access list to the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    direction   @b{(input)} interface direction to apply ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  intf, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for intf, dir
* @returns  L7_REQUEST_DENIED error applying ACL to hardware
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacInterfaceDirectionAdd(L7_uint32 intIfNum, L7_uint32 direction, 
                                    L7_uint32 aclIndex, L7_uint32 seqNum)
{
  L7_uint32         aclDir;
  L7_RC_t           rc;

  /* translate direction to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacImpInterfaceDirectionAdd(intIfNum, aclDir, aclIndex, seqNum);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    direction   @b{(input)} interface direction to apply ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS  intIfNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or interface does not exist
* @returns  L7_ERROR    ACL does not exist for this index
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacInterfaceDirectionRemove(L7_uint32 intIfNum, L7_uint32 direction, 
                                       L7_uint32 aclIndex)
{
  L7_uint32   aclDir;
  L7_RC_t     rc;

  /* translate direction to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacImpInterfaceDirectionRemove(intIfNum, aclDir, aclIndex);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  To add a MAC access list to the specified vlan and direction.
*
* @param    vlanNum     @b{(input)} internal vlan number
* @param    direction   @b{(input)} vlan direction to apply ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  vlan, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for vlan, dir
* @returns  L7_REQUEST_DENIED error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanDirectionAdd( L7_uint32 vlanNum,
                                L7_uint32 direction,
                                L7_uint32 aclIndex,
                                L7_uint32 seqNum )
{
  L7_uint32   aclDir;
  L7_RC_t     rc;

  /* translate direction to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  if ( osapiWriteLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  rc = aclMacImpVlanDirectionAdd( vlanNum, aclDir, aclIndex, seqNum );

  if ( osapiWriteLockGive( aclRwLock ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified vlan and direction.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclIndex   the access-list identifying index
*
* @returns  L7_SUCCESS  vlanNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or vlan does not exist
* @returns  L7_ERROR    ACL does not exist for this index
* @returns  L7_REQUEST_DENIED     error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanDirectionRemove( L7_uint32 vlanNum,
                                   L7_uint32 direction,
                                   L7_uint32 aclIndex )
{
  L7_uint32   aclDir;
  L7_RC_t     rc;

  /* translate direction to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  if ( osapiWriteLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  rc = aclMacImpVlanDirectionRemove( vlanNum, aclDir, aclIndex );

  if ( osapiWriteLockGive( aclRwLock ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  Get the first access list rule given a MAC ACL index
*
* @param    aclIndex    @b{(input)}  access list index
* @param    *rulenum    @b{(output)} first configured rule number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no rules exist for this ACL
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleGetFirst(L7_uint32 aclIndex, L7_uint32 *rulenum)
{
  L7_RC_t       rc;

  if (rulenum == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacImpRuleGetFirst(aclIndex, rulenum);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Get the next access list rule given a MAC ACL index
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  current rule number
* @param    *next       @b{(output)} next configured rule number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no more rules exist for this ACL
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleGetNext(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *next)
{
  L7_RC_t       rc;

  if (next == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacImpRuleGetNext(aclIndex, rulenum, next);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if rulenum is valid, configured for this MAC access list
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  current rule number
*
* @returns  L7_SUCCESS  rule is valid
* @returns  L7_ERROR    rule not valid
* @returns  L7_FAILURE  access list does not exist, or other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCheckValid(L7_uint32 aclIndex, L7_uint32 rulenum)
{
  L7_RC_t           rc;
  aclMacRuleParms_t *acl_rp;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacRuleGet(aclIndex, rulenum, &acl_rp);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the MAC ACL rule number is within proper range
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  current rule number
*
* @returns  L7_SUCCESS  rule in range
* @returns  L7_ERROR    rule out of range
* @returns  L7_FAILURE  access list does not exist, or other failures
*
* @comments The aclIndex parameter is required by this function in case there 
*           is ever a need to support separate rule number ranges based
*           on the type of ACL (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleNumRangeCheck(L7_uint32 aclIndex, L7_uint32 rulenum)
{
  /* no need for read/write lock control here */
  return aclMacImpRuleNumRangeCheck(aclIndex, rulenum);
}

/*********************************************************************
*
* @purpose  Checks if aclIndex is currently assigned to this intf,dir pair
*
* @param    aclIndex    @b{(input)}  access list index
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*                                      (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS, if the MAC ACL is assigned to this intf,dir
* @returns  L7_FAILURE, if the MAC ACL is not assigned to this intf,dir
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIntfDirCheckValid(L7_uint32 aclIndex, L7_uint32 intIfNum, L7_uint32 direction)
{
  L7_RC_t           rc = L7_FAILURE;
  L7_uint32         aclDir, i;
  aclIntfCfgData_t  *pCfg;
  aclIntfParms_t    *pList;

  /* check validity of aclnum */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS) 
    return L7_FAILURE;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* translate direction to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  pList = pCfg->intf_dir_Tb[aclDir].assignList;

  for (i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++, pList++)
  {
    if (pList->inUse != L7_TRUE)
      break;

    if (pList->id.aclType == L7_ACL_TYPE_MAC)
    {
      if (pList->id.aclId == aclIndex)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
  } /* endfor i */

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}


/*********************************************************************
*
* @purpose  Checks if aclIndex is currently assigned to this vlan,dir pair
*
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  vlan       the VLAN number
* @param    L7_uint32  direction  the VLAN direction
*
* @returns  L7_SUCCESS, if the MAC ACL is assigned to this intf,dir
* @returns  L7_FAILURE, if the MAC ACL is not assigned to this intf,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanDirCheckValid( L7_uint32 aclIndex,
                                 L7_uint32 vlan,
                                 L7_uint32 direction )
{
  L7_RC_t           rc = L7_FAILURE;
  L7_uint32         aclDir, i;
  aclVlanCfgData_t  *pCfg;
  aclIntfParms_t    *pList;

  /* check validity of aclnum */
  if ( aclMacImpIndexRangeCheck( aclIndex ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  if ( aclMapVlanIsConfigurable( vlan, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  /* translate direction to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }

  if ( osapiReadLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  pList = pCfg->vlan_dir_Tb[aclDir].assignList;

  for ( i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++, pList++ )
  {
    if ( pList->inUse != L7_TRUE )
    {
      break;
    }

    if ( pList->id.aclType == L7_ACL_TYPE_MAC )
    {
      if ( pList->id.aclId == aclIndex )
      {
        rc = L7_SUCCESS;
        break;
      }
    }
  } /* endfor i */

  if ( osapiReadLockGive( aclRwLock ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  Checks if specified MAC access list rule field is configured
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  current rule number
* @param    field       @b{(input)}  rule field of interest
*
* @returns  L7_TRUE     rule field is configured
* @returns  L7_FALSE    rule field not configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL aclMacIsFieldConfigured(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 field)
{
  L7_BOOL rc;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FALSE;

  rc = aclMacImpIsFieldConfigured(aclIndex, rulenum, field);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FALSE;

  return rc;
}

/*********************************************************************
*
* @purpose  To get an action fom a MAC access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *action     @b{(output)} action type (permit/deny)
*
* @returns  L7_SUCCESS  rule action retrieved
* @returns  L7_FAILURE  invalid acl identifier, all other failures
* @returns  L7_ERROR    access-list does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleActionGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *action)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if (action == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *action = p->action;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the assigned queue id from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *queueId    @b{(output)} assigned queue ID value
*
* @returns  L7_SUCCESS  assigned queue id retrieved
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleAssignQueueIdGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *queueId)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if (queueId == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *queueId = p->assignQueueId;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the redirect interface config id from an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(output)} redirected interface config id
*                                            
* @returns  L7_SUCCESS  redirect interface config id retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  invalid redirect interface or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleRedirectConfigIdGet(L7_uint32 aclIndex, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if (configId == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  NIM_CONFIG_ID_COPY(configId, &p->redirectConfigId);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the mirror interface config id from an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(output)} mirrored interface config id
*                                            
* @returns  L7_SUCCESS  mirror interface config id retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  invalid mirror interface or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleMirrorConfigIdGet(L7_uint32 aclIndex, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if (configId == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  NIM_CONFIG_ID_COPY(configId, &p->mirrorConfigId);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the logging flag from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *logFlag    @b{(output)} rule logging flag value
*
* @returns  L7_SUCCESS  logging flag is retrieved
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid log flag or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleLoggingGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_BOOL *logFlag)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if (logFlag == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *logFlag = p->logging;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the class of service (cos) value from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *cos        @b{(output)} class of service (cos) value
*
* @returns  L7_SUCCESS  class of service value retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCosGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *cos)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  if (cos == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *cos = p->cos;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the secondary class of service (cos2) value from 
*           an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *cos2       @b{(output)} secondary class of service (cos) value
*
* @returns  L7_SUCCESS  secondary class of service value retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCos2Get(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *cos2)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  if (cos2 == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *cos2 = p->cos2;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the destination MAC address and mask from
*           an access list rule
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *dstmac     @b{(output)} destination MAC address
* @param    *dstmask    @b{(output)} destination MAC mask
*
* @returns  L7_SUCCESS  destination MAC added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleDstMacGet(L7_uint32 aclIndex, L7_uint32 rulenum,
                            L7_uchar8 *dstmac, L7_uchar8 *dstmask)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if ((dstmac == L7_NULLPTR) || (dstmask == L7_NULLPTR))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  memcpy(dstmac, p->dstMac, (size_t)L7_MAC_ADDR_LEN);
  memcpy(dstmask, p->dstMacMask, (size_t)L7_MAC_ADDR_LEN);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the Ethertype keyword identifier from an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *keyid      @b{(output)} Ethertype keyword identifier
* @param    *value      @b{(output)} Ethertype custom value       
*
* @returns  L7_SUCCESS  Ethertype keyword identifier retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments The *value output is only meaningful when the *keyid 
*           reads L7_QOS_ETYPE_KEYID_CUSTOM.  It is set to zero
*           otherwise.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleEtypeKeyGet(L7_uint32 aclIndex, L7_uint32 rulenum, 
                              L7_QOS_ETYPE_KEYID_t *keyid, L7_uint32 *value)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if ((keyid == L7_NULLPTR) || (value == L7_NULLPTR))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *keyid = p->etypeKeyId;
  *value = 0;

  /* only provide an etype value if the keyid is set to 'custom' */
  if (p->etypeKeyId == L7_QOS_ETYPE_KEYID_CUSTOM)
    *value = p->etypeValue;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the match condition whereby all packets match for a
*           MAC ACL rule
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *every      @b{(output)} 'match every' flag value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS  'match every' condition retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleEveryGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_BOOL *every)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if (every == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *every = p->every;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the source MAC address and mask from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *srcmac     @b{(output)} source MAC address
* @param    *srcmask    @b{(output)} source MAC mask
*
* @returns  L7_SUCCESS  source MAC retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleSrcMacGet(L7_uint32 aclIndex, L7_uint32 rulenum,
                            L7_uchar8 *srcmac, L7_uchar8 *srcmask)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if ((srcmac == L7_NULLPTR) || (srcmask == L7_NULLPTR))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  memcpy(srcmac, p->srcMac, (size_t)L7_MAC_ADDR_LEN);
  memcpy(srcmask, p->srcMacMask, (size_t)L7_MAC_ADDR_LEN);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get a single VLAN ID from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *vlan       @b{(output)} VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanIdGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *vlan)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if (vlan == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *vlan = p->vlanId;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get a VLAN ID range from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *startvlan  @b{(output)} starting VLAN ID
* @param    *endvlan    @b{(output)} ending VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID range retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanIdRangeGet(L7_uint32 aclIndex, L7_uint32 rulenum, 
                                 L7_uint32 *startvlan, L7_uint32 *endvlan)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if ((startvlan == L7_NULLPTR) || (endvlan == L7_NULLPTR))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *startvlan = p->vlanIdStart;
  *endvlan = p->vlanIdEnd;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get a single Secondary VLAN ID (vlan2) from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *vlan2      @b{(output)} Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanId2Get(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *vlan2)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if (vlan2 == L7_NULLPTR)
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *vlan2 = p->vlanId2;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get a Secondary VLAN ID (vlan2) range from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *startvlan2 @b{(output)} starting Secondary VLAN ID
* @param    *endvlan2   @b{(output)} ending Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID range retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanId2RangeGet(L7_uint32 aclIndex, L7_uint32 rulenum, 
                                  L7_uint32 *startvlan2, L7_uint32 *endvlan2)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclMacRuleParms_t *p;

  /* check inputs */
  if ((startvlan2 == L7_NULLPTR) || (endvlan2 == L7_NULLPTR))
    return L7_FAILURE;

  /* this call acquires the aclRwLock -- must be released prior to returning */
  rc = aclMacRuleAcquireListUnderReadLock(aclIndex, rulenum, &acl_ptr, &p);
  if (rc != L7_SUCCESS)
    return rc;

  *startvlan2 = p->vlanId2Start;
  *endvlan2 = p->vlanId2End;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets a list of interfaces to which an ACL is assigned in the
*           specified direction
*
* @param    aclIndex    @b{(input)}  access list index
* @param    direction   @b{(input)}  interface direction of interest
*                                      (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *intfList   @b{(output)} list of interfaces (L7_ACL_INTF_MAX_COUNT long)
*
* @returns  L7_SUCCESS  list of interfaces was built
* @returns  L7_FAILURE  invalid access list index, or interface does not exist
* @returns  L7_ERROR    access list not assigned to any interface
*                         in this direction (no interface list was built)
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacAssignedIntfDirListGet(L7_uint32 aclIndex, L7_uint32 direction,
                                     L7_ACL_ASSIGNED_INTF_LIST_t *intfList)
{                            
  L7_RC_t       rc;
  L7_uint32     aclDir;

  /* check inputs */
  if ((aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS) ||
      (intfList == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  /* translate direction to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacImpAssignedIntfDirListGet(aclIndex, aclDir, intfList);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Gets a list of vlan a MAC ACL is associated with for the
*           specified direction
*
* @param    L7_uint32  aclIndex    the access-list identifying index
* @param    L7_uint32  direction   the direction of application of ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  *vlanList   list of vlans (L7_ACL_VLAN_MAX_COUNT long)
*
* @returns  L7_SUCCESS, if a list of vlans was built
* @returns  L7_FAILURE, if invalid identifying number or
*                       internal vlan number does not exist
* @returns  L7_ERROR,   if access-list not assigned to any vlan
*                       in this direction (no vlan list was built)
*
* @comments An ACL can be shared/associated with multiple vlans.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacAssignedVlanDirListGet( L7_uint32 aclIndex,
                                      L7_uint32 direction,
                                      L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList )
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     aclDir;

  /* check inputs */
  if ( ( aclMacImpIndexRangeCheck( aclIndex ) != L7_SUCCESS ) ||
       ( L7_NULLPTR == vlanList ) )
  {
    return( L7_FAILURE );
  }

  /* translate direction to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  if ( osapiReadLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  rc = aclMacImpAssignedVlanDirListGet( aclIndex, aclDir, vlanList );

  if ( osapiReadLockGive( aclRwLock ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  return( rc );
}

/*************************************************************************
* @purpose  Get the maximum number of ACLs that can be configured
*
* @param    pMax  @b{(output)} Pointer to the maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t aclMacMaxNumGet(L7_uint32 *pMax)
{
  if (pMax != L7_NULLPTR)
    *pMax = L7_ACL_MAX_LISTS;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the current number of MAC ACLs configured
*
* @param    pCurr  @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t aclMacCurrNumGet(L7_uint32 *pCurr)
{
  L7_RC_t   rc;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacImpCurrNumGet(pCurr);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Creates a single TLV containing active rules of the specified
*           MAC ACL within it and return the data in TLV block storage
*           provided by the caller.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    tlvHandle   @b{(input)}  TLV block handle (supplied by caller)
*
* @returns  L7_SUCCESS  TLV successfully built
* @returns  L7_ERROR    Access list does not exist
* @returns  L7_FAILURE  All other failures
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t aclMacTlvGet(L7_uint32 aclIndex, L7_tlvHandle_t tlvHandle)
{
  L7_RC_t       rc;
  L7_ACL_TYPE_t aclType;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  aclType = aclImpTypeFromIndexGet(aclIndex);
  if (aclMacImpIndexCheckValid(aclIndex) != L7_SUCCESS)
  {
    aclType = L7_ACL_TYPE_NONE;
  }

  if (aclType == L7_ACL_TYPE_MAC)
  {
    rc = aclImpTlvGet(aclType, aclIndex, tlvHandle);
  }
  else
  {
    rc = L7_ERROR;                      /* ACL not found for allowed types */
  }

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Notify registered users of a change to a MAC ACL.
*
* @param    aclIndex    @b{(input)} access list index
* @param    event       @b{(input)} event indication
* @param    aclOldName  @b{(input)} previous ACL name, or L7_NULLPTR
*
* @returns  void
*
* @comments It is important that this function only use a read lock.
*           The callback functions could lead to a path that takes a   
*           write lock which would cause a deadlock condition. 
*
* @comments The aclOldName parm is used for a rename or delete event
*           only, and is set to L7_NULLPTR otherwise.
*
* @comments If the ACL is being named for the first time, the aclOldName
*           parameter is non-null and points to an empty string "".
*
* @end
*
*********************************************************************/
static void aclMacNotify(L7_uint32 aclIndex, aclEvent_t event, L7_uchar8 *aclOldName)
{
  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
      return;

  aclImpNotify(event, L7_ACL_TYPE_MAC, aclIndex, aclOldName);

  (void)ACL_READ_LOCK_GIVE(aclRwLock);
}

/*********************************************************************
*
* @purpose  To obtain the read lock for a given MAC ACL rule
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    **acl_ptr   @b{(output)} ACL element pointer
* @param    **rule_ptr  @b{(output)} ACL rule content pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  other failure
*
* @comments An L7_SUCCESS response means the aclRwLock has been taken.
*           The caller is responsible for giving back this lock.
*
* @comments This is an internal function, not part of the MAC ACL API.
*
* @end
*
*********************************************************************/
static L7_RC_t aclMacRuleAcquireListUnderReadLock(L7_uint32 aclIndex, L7_uint32 rulenum,
                                                  void **acl_ptr, aclMacRuleParms_t **rule_ptr)
{
  /* check inputs */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclMacImpRuleNumRangeCheck(aclIndex, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((acl_ptr == L7_NULLPTR) || (rule_ptr == L7_NULLPTR))
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  *acl_ptr = aclMacFindACL(aclIndex);
  if (*acl_ptr == L7_NULLPTR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  if (aclMacRuleGet(aclIndex, rulenum, rule_ptr) != L7_SUCCESS)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To obtain the write lock for a given MAC ACL rule
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    **acl_ptr   @b{(output)} ACL element pointer
* @param    **rule_ptr  @b{(output)} ACL rule content pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  other failure
*
* @comments An L7_SUCCESS response means the aclRwLock has been taken.
*           The caller is responsible for giving back this lock.
*
* @comments This is an internal function, not part of the MAC ACL API.
*
* @end
*
*********************************************************************/
static L7_RC_t aclMacRuleAcquireListUnderWriteLock(L7_uint32 aclIndex, L7_uint32 rulenum,
                                                   void **acl_ptr, aclMacRuleParms_t **rule_ptr)
{
  /* check inputs */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclMacImpRuleNumRangeCheck(aclIndex, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((acl_ptr == L7_NULLPTR) || (rule_ptr == L7_NULLPTR))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  *acl_ptr = aclMacFindACL(aclIndex);
  if (*acl_ptr == L7_NULLPTR)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  if (aclMacRuleGet(aclIndex, rulenum, rule_ptr) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  return L7_SUCCESS;
}

