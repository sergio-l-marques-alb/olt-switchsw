/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename acl_api.c
*
* @purpose Contains LVL7 Access Control List APIs
*
* @component Access Control List
*
* @comments Many APIs in this file take a read/write lock. Control is 
*           then passed into one or more implementation functions in
*           acl.c. The lock is always managed at the API level and 
*           assumed to be already taken, if necessary, in the implementation
*           functions.
*
* @create 06/23/2002
*
* @author djohnson
*
* @end
*
**********************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "l7_common.h"
#include "nimapi.h"
#include "acl_api.h"
#include "usmdb_qos_acl_api.h"
#include "l7_cnfgr_api.h"
#include "acl.h"

extern aclCfgFileData_t  *aclCfgFileData;
extern avlTree_t         aclTree;
extern osapiRWLock_t     aclRwLock;
extern aclCnfgrState_t   aclCnfgrState;

static void aclNotify(L7_uint32 aclnum, aclEvent_t event, L7_uchar8 *oldName);


/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new named ACL in the access list table
*
* @param    aclType     @b{(input)}  access list type
* @param    *next       @b{(output)} access list index next value
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
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
L7_RC_t aclNamedIndexNextFree(L7_ACL_TYPE_t aclType, L7_uint32 *next)
{
  L7_RC_t       rc = L7_SUCCESS;

  /* check for one of the supported named IP ACL types */
  if (aclImpNamedAclTypeSupported(aclType) != L7_SUCCESS)
    return L7_NOT_SUPPORTED;

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
    if (aclImpIsTableFull() == L7_TRUE)
    {
      /* table full */
      rc = L7_TABLE_IS_FULL;
      break;
    }

    aclImpNamedIndexNextRead(aclType, next);

  } while (0);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  if (rc != L7_SUCCESS)
    *next = 0;

  return rc;
}

/*************************************************************************
*
* @purpose  Retrieve the min/max index values allowed for the named ACL table 
*
* @param    aclType     @b{(input)}  access list type
* @param    *pMin       @b{(output)} Pointer to index min output location
* @param    *pMax       @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE
*
* @comments This is purely informational.  The aclNamedIndexNext function
*           must be used to obtain the current index value.
*
* @end
*
*********************************************************************/
L7_RC_t aclNamedIndexMinMaxGet(L7_ACL_TYPE_t aclType, L7_uint32 *pMin, L7_uint32 *pMax)
{
  L7_RC_t       rc;

  /* check for one of the supported named IP ACL types */
  if (aclImpNamedAclTypeSupported(aclType) != L7_SUCCESS)
    return L7_NOT_SUPPORTED;

  if ((pMin == L7_NULLPTR) || (pMax == L7_NULLPTR))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = aclImpNamedIndexMinMaxGet(aclType, pMin, pMax);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Get the first named access list index created in the system.
*
* @param    aclType     @b{(input)}  access list type
* @param    *pAclIndex  @b{(output)} pointer to ACL index value
*
* @returns  L7_SUCCESS        First named access list index was found
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE  
* @returns  L7_ERROR          No access lists have been created yet
*
*
* @end
*
*********************************************************************/
L7_RC_t aclNamedIndexGetFirst(L7_ACL_TYPE_t aclType, L7_uint32 *pAclIndex)
{
  /* use getNext function with a starting index value of 0 */
  return aclNamedIndexGetNext(aclType, 0, pAclIndex);
}

/*********************************************************************
*
* @purpose  Get the first named access list index created in the system.
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  current ACL index value
* @param    *pAclIndex  @b{(output)} pointer to next ACL index value
*
* @returns  L7_SUCCESS        Next named access list index was found
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_ERROR          No subsequent ACL indexes exist
* @returns  L7_FAILURE        All other failures
*
* @comments This function is also used for a 'getFirst' search by 
*           specifying an aclIndex value of 0.
*
* @end
*
*********************************************************************/
L7_RC_t aclNamedIndexGetNext(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex, 
                             L7_uint32 *pAclIndex)
{
  L7_RC_t       rc;

  /* check for one of the supported named IP ACL types */
  if (aclImpNamedAclTypeSupported(aclType) != L7_SUCCESS)
    return L7_NOT_SUPPORTED;

  if (pAclIndex == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = aclImpNamedIndexGetNext(aclType, aclIndex, pAclIndex);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if aclIndex is a valid, configured named access list
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  ACL index value
*
* @returns  L7_SUCCESS        Access list index was found
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE        Access list index not found
*
* @comments The only valid aclType values are L7_ACL_TYPE_IPV6 and 
*           L7_ACL_TYPE_IP.
*
* @end
*
*********************************************************************/
L7_RC_t aclNamedIndexCheckValid(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex)
{
  L7_RC_t       rc;

  /* check for one of the supported named IP ACL types */
  if (aclImpNamedAclTypeSupported(aclType) != L7_SUCCESS)
    return L7_NOT_SUPPORTED;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = aclImpNamedIndexCheckValid(aclType, aclIndex);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the aclIndex is within proper range
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  ACL index value
*
* @returns  L7_SUCCESS        Access list index is in range
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE        Access list index is out of range
*
* @comments The only valid aclType value is L7_ACL_TYPE_IPV6.
*
* @end
*
*********************************************************************/
L7_RC_t aclNamedIndexRangeCheck(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex)
{
  L7_RC_t       rc;

  /* check for one of the supported named IP ACL types */
  if (aclImpNamedAclTypeSupported(aclType) != L7_SUCCESS)
    return L7_NOT_SUPPORTED;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = aclImpNamedIndexRangeCheck(aclType, aclIndex);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Get the first access list ID created on the system.
*
* @param    L7_uint32  *aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the first access list ID was found
* @returns  L7_ERROR,   if the first access list ID was not found
* @returns  L7_FAILURE, if any other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclNumGetFirst(L7_uint32 *aclnum)
{
  /* use getNext function with a starting index value of 0 */
  return aclNumGetNext(0, aclnum);
} 

/*********************************************************************
*
* @purpose  Given aclnum, get the next access list ID
*
* @param    L7_uint32   aclnum   the current access-list ID
* @param    L7_uint32  *next     the next ACL ID
*
* @returns  L7_SUCCESS, if the next access list ID was found
* @returns  L7_ERROR,   if no subsequent access list ID was found
* @returns  L7_FAILURE, if any other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclNumGetNext(L7_uint32 aclnum, L7_uint32 *next)
{
  L7_RC_t rc;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpNumGetNext(aclnum, next);
  
  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if aclnum is a valid, configured access list
*
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclNumCheckValid(L7_uint32 aclnum)
{
  L7_RC_t rc;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpNumCheckValid(aclnum);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the supported number ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This function does not distinguish between standard and extended
*           IP ACL numbers, but considers whether the ACL ID belongs to any
*           ACL number range that is supported.
*
* @end
*
*********************************************************************/
L7_RC_t aclNumRangeCheck(L7_uint32 aclnum)
{
  /* no need for read/write lock control here */
  return aclImpNumRangeCheck(aclnum);
}

/*********************************************************************
*
* @purpose  To create a new access list.
*
* @param    L7_uint32  aclnum  the access-list identifying number
*
* @returns  L7_SUCCESS, if the access-list is added
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list already exists
* @returns  L7_TABLE_IS_FULL  maximum number of ACLs or rule nodes already created
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclCreate(L7_uint32 aclnum)
{
  void *acl_ptr;
  L7_RC_t rc;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr != L7_NULL)
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
  if (aclImpIsTableFull() == L7_TRUE)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_TABLE_IS_FULL;
  }

  if ((rc = aclCreateACL(aclnum)) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    if (rc == L7_TABLE_IS_FULL)
      return L7_TABLE_IS_FULL;
    else
    return L7_FAILURE;
  }
  
  /* no need to update the driver for a create */

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  aclNotify(aclnum, ACL_EVENT_CREATE, L7_NULLPTR);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To delete an existing access list.
*
* @param    L7_uint32  aclnum  the access-list identifying number
* @param    L7_BOOL    force   force deletion regardless of whether in use
*
* @returns  L7_SUCCESS, if the access-list is deleted
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDelete(L7_uint32 aclnum, L7_BOOL force)
{
  void *acl_ptr;
  aclStructure_t *ptr;
  L7_ACL_TYPE_t aclType;
  L7_uchar8 aclName[L7_ACL_NAME_LEN_MAX+1];

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((force != L7_FALSE) && (force != L7_TRUE))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* save old name for notification event */
  ptr = (aclStructure_t *)acl_ptr;
  aclType = aclImpTypeFromIndexGet(aclnum);
  if (aclType == L7_ACL_TYPE_IP)
  {
    osapiSnprintf(aclName, sizeof(aclName), "%u", aclnum);
  }
  else
  {
    osapiStrncpySafe(aclName, ptr->aclName, sizeof(aclName));
  }

  /* make sure list not currently in use (unless force flag is specified) */
  if ((force != L7_TRUE) &&
      (aclImpDeleteApprovalQuery(aclnum) != L7_SUCCESS))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  if (aclDeleteACL(acl_ptr) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* no update needed, done by aclDeleteACL */

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  aclNotify(aclnum, ACL_EVENT_DELETE, aclName);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To add the name to this named access list.
*
* @param    aclnum      @b{(input)} access list identifying number
* @param    *name       @b{(input)} access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list number, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclNameAdd(L7_uint32 aclnum, L7_uchar8 *name)
{
  void              *acl_ptr;
  aclStructure_t    *ptr;
  L7_uchar8         oldName[L7_ACL_NAME_LEN_MAX+1];

  /* check inputs */
  if (aclImpIndexRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if  (aclNameStringCheck(name) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULLPTR)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* return successfully if existing ACL name is same */
  ptr = (aclStructure_t *)acl_ptr;
  if (osapiStrncmp(ptr->aclName, name, sizeof(ptr->aclName)) == 0)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* save old name for notification event */
  osapiStrncpySafe(oldName, ptr->aclName, sizeof(oldName));

  /* update app */
  if (aclImpNameAdd(acl_ptr, name) != L7_SUCCESS)
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
    aclNotify(aclnum, ACL_EVENT_RENAME, oldName);
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To change the name of an existing named access list.
*
* @param    aclnum      @b{(input)} access list identifying number
* @param    *newname    @b{(input)} new access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list number, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
* @returns  L7_ALREAD_CONFIGURED  name is in use by another ACL
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclNameChange(L7_uint32 aclnum, L7_uchar8 *newname)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclStructure_t    *ptr;
  L7_uint32         newnum;
  L7_uchar8         oldName[L7_ACL_NAME_LEN_MAX+1];

  /* check inputs */
  if (aclImpIndexRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclNameStringCheck(newname) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULLPTR)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* return successfully if existing ACL name is same */
  ptr = (aclStructure_t *)acl_ptr;
  if (osapiStrncmp(ptr->aclName, newname, sizeof(ptr->aclName)) == 0)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* check for duplicate name already in use for a different ACL */
  rc = aclImpNameToIndexGet(newname, &newnum);
  if ((rc == L7_SUCCESS) && (newnum != aclnum))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ALREADY_CONFIGURED;
  }
  
  /* save old name for notification event */
  osapiStrncpySafe(oldName, ptr->aclName, sizeof(oldName));

  /* update app */
  if (aclImpNameAdd(acl_ptr, newname) != L7_SUCCESS)
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
    aclNotify(aclnum, ACL_EVENT_RENAME, oldName);
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To get the name of this access list.
*
* @param    aclnum      @b{(input)}  access list identifying number
* @param    *name       @b{(output)} access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid access list number, name ptr, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Caller must provide a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t aclNameGet(L7_uint32 aclnum, L7_uchar8 *name)
{
  void          *acl_ptr;

  /* check inputs */
  if (aclImpIndexRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (name == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULLPTR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }
  
  /* get name string from ACL entry */
  aclImpNameGet(acl_ptr, name);
  
  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Checks if the named ACL name is a valid string
*
* @param    *name       @b{(input)} access list name string
*
* @returns  L7_SUCCESS  valid named ACL name string
* @returns  L7_FAILURE  invalid named ACL name string
*
* @comments This function only checks the name string syntax for a named ACL. 
*           It does not check if any named ACL currently contains this name.
*
* @end
*
*********************************************************************/
L7_RC_t aclNameStringCheck(L7_uchar8 *name)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     nameLen;

  /* check inputs */
  if (name == L7_NULLPTR)
    return L7_FAILURE;

  /* no need for read/write lock control here */

  nameLen = (L7_uint32)strlen((char *)name);

  if ((nameLen < L7_ACL_NAME_LEN_MIN) || (nameLen > L7_ACL_NAME_LEN_MAX))
    return L7_FAILURE;

  if (aclImpNameIsAlphanumeric(name) == L7_TRUE)
    rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
*
* @purpose  To get the number of a named access list, given its name.
*
* @param    *name      @b{(input)} access list name
* @param    *aclnum    @b{(output)} access list identifying number
*                                            
* @returns  L7_SUCCESS  named access list number is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclNameToIndex(L7_uchar8 *name, L7_uint32 *aclnum)
{
  L7_RC_t       rc;

  /* check inputs */
  if ((name == L7_NULLPTR) || (strlen(name) > L7_ACL_NAME_LEN_MAX))
    return L7_FAILURE;
  if (aclnum == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* search for access list containing this name */
  rc = aclImpNameToIndexGet(name, aclnum);
  
  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Get the index number of an access list, given its ACL type and name.
*
* @param    aclType     @b{(input)}  access list type
* @param    *pName      @b{(input)}  access list name pointer
* @param    *aclNum     @b{(output)} access list identifying number
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR    ACL does not exist
* @returns  L7_FAILURE
*
* @comments Uses the ACL type and name to output the list index
*           number.  For IPV4 numbered access lists, the ACL name
*           is its number represented in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
L7_RC_t aclCommonNameToIndex(L7_ACL_TYPE_t aclType, L7_uchar8 *pName,
                             L7_uint32 *pAclNum)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     aclNum = 0;

  if ((L7_NULLPTR == pName) ||
      (L7_NULLPTR == pAclNum))
  {
    return L7_FAILURE;
  }

  if (isdigit(*(L7_char8 *)pName) != 0)
  {
    /* IPV4 numbered ACL */
    aclNum = (L7_uint32)atoi(pName);
    rc = L7_SUCCESS;
  }
  else
  {
    switch (aclType)
    {
      case L7_ACL_TYPE_IP:
      case L7_ACL_TYPE_IPV6:
        /* IPV4 or IPv6 named ACL */
        rc = aclNameToIndex(pName, &aclNum);
        break;

      case L7_ACL_TYPE_MAC:
        /* MAC named ACL */
        rc = aclMacNameToIndex(pName, &aclNum);
        break;

      default:
        /* ACL does not exist */
        rc = L7_FAILURE;
        break;

    } /* endswitch */
  }

  if (L7_SUCCESS == rc)
  {
    *pAclNum = aclNum;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get a name string for any type of access list, given its index
*
* @param    aclnum      @b{(input)}  access list identifying number
* @param    *aclType    @b{(output)} access list type
* @param    *name       @b{(output)} access list name
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @comments Determines the ACL type from its index number and outputs
*           the list name.  For IPV4 numbered access lists, the 
*           ACL number is output in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
L7_RC_t aclCommonNameStringGet(L7_uint32 aclnum, L7_ACL_TYPE_t *aclType,
                               L7_uchar8 *name)
{
  L7_RC_t       rc = L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpCommonNameStringGet(aclnum, aclType, name);
  
  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}


/*********************************************************************
*
* @purpose  To add an action to a new access list.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  action   action type (permit/deny)
*
* @returns  L7_SUCCESS, if the access option is added
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
* @returns  L7_TABLE_IS_FULL, if storage unavailable for rule
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleActionAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 action)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (action >= L7_ACL_ACTION_TOTAL)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }
  
  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_ACTION) == L7_TRUE) &&
      (p->action == action))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  if ((rc = aclActionAdd(acl_ptr, rulenum, action)) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return rc;
  }
  
  rc = aclUpdate(aclnum, ACL_MODIFY);
  if (rc != L7_SUCCESS)
    rc = L7_REQUEST_DENIED;

  /***************fix for rollback */

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  aclNotify(aclnum, ACL_EVENT_ROUTE_FILTER_MODIFY, L7_NULLPTR);
  return rc;
}


/*********************************************************************
*
* @purpose  To remove a rule from an existing access list.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
*                                            
* @returns  L7_SUCCESS, if the rule number is removed
* @returns  L7_FAILURE, if invalid access-list number or rule number
* @returns  L7_ERROR, if the access-list or rule number does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleRemove(L7_uint32 aclnum, L7_uint32 rulenum)
{
  L7_RC_t rc;
  void *acl_ptr;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  (void)aclRemoveRule(acl_ptr, rulenum);        /* ignore rc and keep going */

  rc = aclUpdate(aclnum, ACL_MODIFY);
  if (rc != L7_SUCCESS)
    rc = L7_REQUEST_DENIED;

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  aclNotify(aclnum, ACL_EVENT_ROUTE_FILTER_MODIFY, L7_NULLPTR);
  return rc;
}


/*********************************************************************
*
* @purpose  To set the assigned queue id for an access list rule.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  queueId   the queue id value              
*
* @returns  L7_SUCCESS, if assigned queue id is added
* @returns  L7_FAILURE, if invalid queue id or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleAssignQueueIdAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 queueId)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_queueid;
  L7_uint32 saved_config;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((queueId < L7_ACL_ASSIGN_QUEUE_ID_MIN) || 
      (queueId > L7_ACL_ASSIGN_QUEUE_ID_MAX))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* save */
  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_ASSIGN_QUEUEID) == L7_TRUE) &&
      (p->assignQueueId == queueId))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config   = p->configMask;
  saved_queueid  = p->assignQueueId;

  /* change in app */
  if (aclAssignQueueIdAdd(acl_ptr, rulenum, queueId) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclUpdate(aclnum, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->assignQueueId = saved_queueid;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_ATTR_ONLY_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To set the redirect interface config id for an access list rule.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *configId  the redirect interface config id
*                                            
* @returns  L7_SUCCESS, if redirect interface is added
* @returns  L7_FAILURE, if invalid redirect interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleRedirectConfigIdAdd(L7_uint32 aclnum, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  nimConfigID_t saved_configId;
  L7_uint32 saved_config;
  L7_uint32 intIfNum;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;

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

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* save */
  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_REDIRECT_INTF) == L7_TRUE) &&
      (NIM_CONFIG_ID_IS_EQUAL(&p->redirectConfigId, configId) == L7_TRUE))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* return unsuccessfully if rule contains a mirror attribute (mutually-exclusive) */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MIRROR_INTF) == L7_TRUE))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  saved_config   = p->configMask;
  NIM_CONFIG_ID_COPY(&saved_configId, &p->redirectConfigId);

  /* change in app */
  if (aclRedirectConfigIdAdd(acl_ptr, rulenum, configId) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclUpdate(aclnum, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    NIM_CONFIG_ID_COPY(&p->redirectConfigId, &saved_configId);
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_ATTR_ONLY_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To set the mirror interface config id for an access list rule.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *configId  the mirror interface config id
*                                            
* @returns  L7_SUCCESS, if mirror interface is added
* @returns  L7_FAILURE, if invalid mirror interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleMirrorConfigIdAdd(L7_uint32 aclnum, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  nimConfigID_t saved_configId;
  L7_uint32 saved_config;
  L7_uint32 intIfNum;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;

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
    {
      return L7_FAILURE;
    }
  }

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* save */
  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MIRROR_INTF) == L7_TRUE) &&
      (NIM_CONFIG_ID_IS_EQUAL(&p->mirrorConfigId, configId) == L7_TRUE))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* return unsuccessfully if rule contains a redirect attribute (mutually-exclusive) */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_REDIRECT_INTF) == L7_TRUE))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  saved_config   = p->configMask;
  NIM_CONFIG_ID_COPY(&saved_configId, &p->mirrorConfigId);

  /* change in app */
  if (aclMirrorConfigIdAdd(acl_ptr, rulenum, configId) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclUpdate(aclnum, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    NIM_CONFIG_ID_COPY(&p->mirrorConfigId, &saved_configId);
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_ATTR_ONLY_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To set the logging flag for an access list rule.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  logFlag   rule logging flag value         
*
* @returns  L7_SUCCESS, if logging flag is added
* @returns  L7_FAILURE, if invalid flag value or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleLoggingAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_BOOL logFlag)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_BOOL saved_logging;
  L7_uint32 saved_config;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((logFlag != L7_FALSE) && (logFlag != L7_TRUE))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* save */
  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_LOGGING) == L7_TRUE) &&
      (p->logging == logFlag))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config   = p->configMask;
  saved_logging  = p->logging;

  /* change in app */
  if (aclLoggingAdd(acl_ptr, rulenum, logFlag) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclUpdate(aclnum, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->logging = saved_logging;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_ATTR_ONLY_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To add a protocol to an existing access list entry.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  protocol  the protocol number             
*
* @returns  L7_SUCCESS, if the protocol is added
* @returns  L7_FAILURE, if invalid list number, rulenum, or protocol
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleProtocolAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 protocol)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uchar8 saved_protocol, saved_protmask;
  L7_uint32 saved_config;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((protocol < L7_ACL_MIN_PROTOCOL_NUM) || 
      (protocol > L7_ACL_MAX_PROTOCOL_NUM))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  /* save */
  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_PROTOCOL) == L7_TRUE) &&
      (p->protocol == protocol))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config   = p->configMask;
  saved_protocol = p->protocol;
  saved_protmask = p->protmask;

  /* change in app */
  if (aclProtocolAdd(acl_ptr, rulenum, protocol) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  /* update driver */
  rc = aclUpdate(aclnum, ACL_MODIFY);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    p->configMask = saved_config;
    p->protocol = saved_protocol;
    p->protmask = saved_protmask;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To add the source ip address and net mask.
*
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  entryId  the access-list entry id
* @param    L7_uint32  ip       the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleSrcIpMaskAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 ipAddr, L7_uint32 mask)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config, saved_ip, saved_mask;

  /* check inputs */
  if ((aclImpStdAclNumRangeCheck(aclnum) != L7_SUCCESS) &&
      (aclImpExtGroupRangeCheck(aclnum) != L7_SUCCESS))
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCIP) == L7_TRUE) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCIP_MASK) == L7_TRUE) &&
      (p->srcIp.v4.addr == ipAddr) &&
      (p->srcIp.v4.mask == mask))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config = p->configMask;
  saved_ip     = p->srcIp.v4.addr;
  saved_mask   = p->srcIp.v4.mask;

  if (aclSrcIPMaskAdd(acl_ptr, rulenum, ipAddr, mask) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->srcIp.v4.addr = saved_ip;
    p->srcIp.v4.mask = saved_mask;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_ROUTE_FILTER_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To add the source IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if source ipv6 address and prefix length are added
* @returns  L7_FAILURE, if invalid list number, entry id, or other
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleSrcIpv6AddrAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                              L7_in6_prefix_t *addr6)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_in6_prefix_t saved_addr6;

  /* check inputs */
  if (aclImpNamedIpv6IndexRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (addr6 == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same
   *
   * NOTE: Checking addr and prefix len fields individually to avoid
   *       false miscompares on uninitialized compiler pad bytes, if any exist
   */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCIPV6) == L7_TRUE) &&
      (memcmp(p->srcIp.v6.in6Addr.in6.addr8, addr6->in6Addr.in6.addr8, L7_IP6_ADDR_LEN) == 0) &&
      (p->srcIp.v6.in6PrefixLen == addr6->in6PrefixLen))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config = p->configMask;
  saved_addr6  = p->srcIp.v6;

  if (aclSrcIpv6AddrAdd(acl_ptr, rulenum, addr6) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->srcIp.v6 = saved_addr6;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }
  
  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_ROUTE_FILTER_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To add the source port.
*
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  port     the ip address
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleSrcL4PortAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 port)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_ushort16 saved_port;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((port < L7_ACL_MIN_L4PORT_NUM) || 
      (port > L7_ACL_MAX_L4PORT_NUM))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCPORT) == L7_TRUE) &&
      (p->srcPort == (L7_ushort16)port))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config = p->configMask;
  saved_port = p->srcPort;

  if (aclSrcPortAdd(acl_ptr, rulenum, port) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->srcPort = saved_port;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To add the range of the source layer 4 ports to a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleSrcL4PortRangeAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                 L7_uint32 startport, L7_uint32 endport)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_ushort16 saved_startport, saved_endport;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((startport < L7_ACL_MIN_L4PORT_NUM) || 
      (startport > L7_ACL_MAX_L4PORT_NUM))
    return L7_FAILURE;
  if ((endport < L7_ACL_MIN_L4PORT_NUM) || 
      (endport > L7_ACL_MAX_L4PORT_NUM))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCSTARTPORT) == L7_TRUE) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCENDPORT) == L7_TRUE) &&
      (p->srcStartPort == (L7_ushort16)startport) &&
      (p->srcEndPort == (L7_ushort16)endport))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config    = p->configMask;
  saved_startport = p->srcStartPort;
  saved_endport   = p->srcEndPort;

  if (aclSrcPortRangeAdd(acl_ptr, rulenum, startport, endport) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask   = saved_config;
    p->srcStartPort = saved_startport;
    p->srcEndPort   = saved_endport;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To add the destination ip address and mask.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  ipAddr   the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleDstIpMaskAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 ipAddr, L7_uint32 mask)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config, saved_ip, saved_mask;

  /* check inputs */
  if (aclImpExtGroupRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTIP) == L7_TRUE) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTIP_MASK) == L7_TRUE) &&
      (p->dstIp.v4.addr == ipAddr) &&
      (p->dstIp.v4.mask == mask))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config = p->configMask;
  saved_ip     = p->dstIp.v4.addr;
  saved_mask   = p->dstIp.v4.mask;

  if (aclDstIPMaskAdd(acl_ptr, rulenum, ipAddr, mask) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->dstIp.v4.addr = saved_ip;
    p->dstIp.v4.mask = saved_mask;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }
  
  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_ROUTE_FILTER_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To add the destination IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if destination ipv6 address and prefix length are added
* @returns  L7_FAILURE, if invalid list number, entry id, or other
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleDstIpv6AddrAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                              L7_in6_prefix_t *addr6)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_in6_prefix_t saved_addr6;

  /* check inputs */
  if (aclImpNamedIpv6IndexRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (addr6 == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same
   *
   * NOTE: Checking addr and prefix len fields individually to avoid
   *       false miscompares on uninitialized compiler pad bytes, if any exist
   */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTIPV6) == L7_TRUE) &&
      (memcmp(p->dstIp.v6.in6Addr.in6.addr8, addr6->in6Addr.in6.addr8, L7_IP6_ADDR_LEN) == 0) &&
      (p->dstIp.v6.in6PrefixLen == addr6->in6PrefixLen))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config = p->configMask;
  saved_addr6  = p->dstIp.v6;

  if (aclDstIpv6AddrAdd(acl_ptr, rulenum, addr6) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->dstIp.v6 = saved_addr6;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }
  
  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_ROUTE_FILTER_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To add the destination port.
*
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  entryId  the access-list entry id
* @param    L7_uint32  port     the ip address
*
* @returns  L7_SUCCESS, if destination ip address, net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleDstL4PortAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 port)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_ushort16 saved_port;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((port < L7_ACL_MIN_L4PORT_NUM) || 
      (port > L7_ACL_MAX_L4PORT_NUM))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTPORT) == L7_TRUE) &&
      (p->dstPort == (L7_ushort16)port))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config = p->configMask;
  saved_port = p->dstPort;

  if (aclDstPortAdd(acl_ptr, rulenum, port) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->dstPort = saved_port;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the range of the source layer 4 ports to a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleDstL4PortRangeAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                 L7_uint32 startport, L7_uint32 endport)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_ushort16 saved_startport, saved_endport;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((startport < L7_ACL_MIN_L4PORT_NUM) || 
      (startport > L7_ACL_MAX_L4PORT_NUM))
    return L7_FAILURE;
  if ((endport < L7_ACL_MIN_L4PORT_NUM) || 
      (endport > L7_ACL_MAX_L4PORT_NUM))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTSTARTPORT) == L7_TRUE) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTENDPORT) == L7_TRUE) &&
      (p->dstStartPort == (L7_ushort16)startport) &&
      (p->dstEndPort == (L7_ushort16)endport))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config = p->configMask;
  saved_startport = p->dstStartPort;
  saved_endport = p->dstEndPort;

  if (aclDstPortRangeAdd(acl_ptr, rulenum, startport, endport) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->dstStartPort = saved_startport;
    p->dstEndPort = saved_endport;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Set the match condition whereby all packets match for an
*           ACL rule
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_BOOL    match    the match condition (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list or rule number
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments This routine sets the range of all the filtering criteria
*           in a rule to the maximum, in an existing access list.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleEveryAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_BOOL match)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_BOOL saved_every;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((match != L7_FALSE) && (match != L7_TRUE))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_EVERY) == L7_TRUE) &&
      (p->every == match))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config = p->configMask;
  saved_every = p->every;

  if (aclEveryAdd(acl_ptr, rulenum, match) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->every = saved_every;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_ROUTE_FILTER_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the differentiated services code point (dscp) value
*           to a rule in an existing access list.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  dscpval  the diffserv code point value
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleIPDscpAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 dscpval)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_uchar8 saved_tosbyte, saved_tosmask;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((dscpval < L7_ACL_MIN_DSCP) || 
      (dscpval > L7_ACL_MAX_DSCP))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_IPDSCP) == L7_TRUE) &&
      ((p->tosbyte >> L7_QOS_ACL_TLV_MATCH_IPDSCP_SHIFT) == (L7_uchar8)dscpval))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config  = p->configMask;
  saved_tosbyte = p->tosbyte;
  saved_tosmask = p->tosmask;

  if (aclIPDscpAdd(acl_ptr, rulenum, dscpval) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);
  
  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->tosbyte = saved_tosbyte;
    p->tosmask = saved_tosmask;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the precedence value to a rule in an existing
*           access list.
*
* @param    L7_uint32  aclnum         access-list identifying number
* @param    L7_uint32  rulenum        the acl rule number
* @param    L7_uint32  precedenceval  the precedence value
*
* @returns  L7_SUCCESS, if the precedence value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleIPPrecedenceAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 precedenceval)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_uchar8 saved_tosbyte, saved_tosmask;

  /* check inputs */
  if (aclImpExtGroupRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((precedenceval < L7_ACL_MIN_PRECEDENCE) || 
      (precedenceval > L7_ACL_MAX_PRECEDENCE))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_IPPREC) == L7_TRUE) &&
      ((p->tosbyte >> L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_SHIFT) == (L7_uchar8)precedenceval))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config  = p->configMask;
  saved_tosbyte = p->tosbyte;
  saved_tosmask = p->tosmask;
  
  if (aclIPPrecedenceAdd(acl_ptr, rulenum, precedenceval) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);

  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->tosbyte    = saved_tosbyte;
    p->tosmask    = saved_tosmask;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To add the type of service bits and mask.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  tosbits  tos bits
* @param    L7_uint32  tosmask  tos mask
*
* @returns  L7_SUCCESS, if the service type parameters are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleIPTosAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                        L7_uint32 tosbits, L7_uint32 tosmask)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config;
  L7_uchar8 saved_tosbyte, saved_tosmask;

  /* check inputs */
  if (aclImpExtGroupRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((tosbits < L7_ACL_MIN_TOS_BIT) || 
      (tosbits > L7_ACL_MAX_TOS_BIT))
    return L7_FAILURE;
  if (tosmask > L7_ACL_MAX_TOS_BIT)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_IPTOS) == L7_TRUE) &&
      (p->tosbyte == (L7_uchar8)tosbits) &&
      (p->tosmask == (L7_uchar8)tosmask))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config  = p->configMask;
  saved_tosbyte = p->tosbyte;
  saved_tosmask = p->tosmask;
  
  if (aclIPTosAdd(acl_ptr, rulenum, tosbits, tosmask) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);
  
  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->tosbyte = saved_tosbyte;
    p->tosmask = saved_tosmask;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add the IPv6 flow label.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  flowlbl  flow label value
*
* @returns  L7_SUCCESS, if the value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleIpv6FlowLabelAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                L7_uint32 flowlbl)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclRuleParms_t *p;
  L7_uint32 saved_config, saved_flowlbl;

  /* check inputs */
  if (aclImpNamedIpv6IndexRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if ((flowlbl < L7_ACL_MIN_FLOWLBL) || 
      (flowlbl > L7_ACL_MAX_FLOWLBL))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  rc = aclRuleGet(aclnum,rulenum,&p);

  /* return successfully if rule already exists and value is same */
  if ((rc == L7_SUCCESS) &&
      (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_FLOWLBLV6) == L7_TRUE) &&
      (p->flowlbl == flowlbl))
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  saved_config  = p->configMask;
  saved_flowlbl = p->flowlbl;
  
  if (aclIpv6FlowLabelAdd(acl_ptr, rulenum, flowlbl) != L7_SUCCESS)
  {
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
    return L7_FAILURE;
  }

  rc = aclUpdate(aclnum, ACL_MODIFY);
  
  if (rc != L7_SUCCESS)
  {
    p->configMask = saved_config;
    p->flowlbl = saved_flowlbl;
    (void)aclUpdate(aclnum, ACL_MODIFY);        /* ignore rc here */
    rc = L7_REQUEST_DENIED;
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  /* don't send notification if rollback occurred */
  if (rc != L7_REQUEST_DENIED)
  {
    aclNotify(aclnum, ACL_EVENT_MODIFY, L7_NULLPTR);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  seqNum     the ACL evaluation order sequence number
*
* @returns  L7_SUCCESS, if ACL successfully added to intf,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED, if intf,dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED, if feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL, if ACL assignment at capacity for intf, dir
* @returns  L7_REQUEST_DENIED, if error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclInterfaceDirectionAdd(L7_uint32 intIfNum, L7_uint32 direction, 
                                 L7_uint32 aclnum, L7_uint32 seqNum)
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check for valid direction and translate to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpInterfaceDirectionAdd(intIfNum, aclDir, aclnum, seqNum);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  To remove an access list from the specified interface and direction.
*
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if ACL successfully removed from intf,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL does not exist for this aclnum
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclInterfaceDirectionRemove(L7_uint32 intIfNum, L7_uint32 direction, 
                                    L7_uint32 aclnum)
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check for valid direction and translate to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpInterfaceDirectionRemove(intIfNum, aclDir, aclnum);

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Check if the specified interface and direction is valid
*           for use with ACL
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS  intf,dir is valid for ACL
* @returns  L7_ERROR    intf,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The interface type and direction supported for ACL use   
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the interface
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the interface
*           and direction (see aclIsInterfaceInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirGet(L7_uint32 intIfNum, L7_uint32 direction)
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check validity of the intIfNum */
  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  /* check for supported interface type */
  if (aclIsValidIntf(intIfNum) == L7_FALSE)
    return L7_FAILURE;

  /* check for valid direction and translate to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpIntfDirGet(intIfNum, aclDir);

  (void)ACL_WRITE_LOCK_GIVE(aclRwLock);

  return rc;
}

/*********************************************************************
*
* @purpose  Get next sequential interface and direction that is valid
*           for use with ACL
*
* @param    intIfNum        @b{(input)}  internal interface number
* @param    direction       @b{(input)}  interface direction
*                                        (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *nextIntIfNum   @b{(output)} next internal interface number
* @param    *nextDirection  @b{(output)} next interface direction
*                                        (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS  next intf,dir for ACL was obtained
* @returns  L7_ERROR    no more valid intf,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The interface type and direction supported for ACL use   
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an interface and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the interface
*           and direction (see aclIsInterfaceInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirGetNext(L7_uint32 intIfNum, L7_uint32 direction,
                          L7_uint32 *nextIntIfNum, L7_uint32 *nextDirection)
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* translate direction to internal value
   *
   * NOTE:  Anything other than inbound direction is treated as outbound
   *        for purposes of finding next direction
   */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    aclDir = ACL_INTF_DIR_OUT;

  /* check output pointers */
  if ((nextIntIfNum == L7_NULLPTR) || 
      (nextDirection == L7_NULLPTR))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpIntfDirGetNext(intIfNum, aclDir, nextIntIfNum, &aclDir);
  if (rc == L7_SUCCESS)
  {
    /* translate internal direction to external API value */
    if (aclDirIntToExt(aclDir, nextDirection) != L7_SUCCESS)
      rc = L7_FAILURE;
  }

  (void)ACL_WRITE_LOCK_GIVE(aclRwLock);

  return rc;
}

/*********************************************************************
*
* @purpose  Verifies that the ACL sequence number is in use for the  
*           specified interface and direction
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL interface sequence number
*
* @returns  L7_SUCCESS  sequence number exists for intf,dir
* @returns  L7_ERROR    sequence number does not exist for intf,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments A sequence number value may be used only once for a given
*           interface and direction pair.  
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirSequenceGet(L7_uint32 intIfNum, L7_uint32 direction, 
                              L7_uint32 seqNum)
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check validity of the intIfNum */
  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  /* check for supported interface type */
  if (aclIsValidIntf(intIfNum) == L7_FALSE)
    return L7_FAILURE;

  /* check for valid direction and translate to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  /* check if sequence number is in range */
  if ((seqNum < L7_ACL_MIN_INTF_SEQ_NUM) || 
      (seqNum > L7_ACL_MAX_INTF_SEQ_NUM))
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpIntfDirSequenceGet(intIfNum, aclDir, seqNum);

  (void)ACL_WRITE_LOCK_GIVE(aclRwLock);

  return rc;
}

/*********************************************************************
*
* @purpose  Get next sequential ACL interface sequence number in use 
*           for the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL interface sequence number
* @param    *nextSeqNum @b{(output)} next ACL interface sequence number
*
* @returns  L7_SUCCESS  next sequence number was obtained
* @returns  L7_ERROR    no more sequence numbers for intf,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments Use a seqNum value of 0 to find the first ACL sequence   
*           number for this interface and direction, if it exists.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirSequenceGetNext(L7_uint32 intIfNum, L7_uint32 direction, 
                                  L7_uint32 seqNum, L7_uint32 *nextSeqNum)
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check validity of the intIfNum */
  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  /* check for supported interface type */
  if (aclIsValidIntf(intIfNum) == L7_FALSE)
    return L7_FAILURE;

  /* check for valid direction and translate to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  /* NOTE:  Any seqNum value is allowed as input to a getNext function */

  /* check output pointer */
  if (nextSeqNum == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpIntfDirSequenceGetNext(intIfNum, aclDir, seqNum, nextSeqNum);

  (void)ACL_WRITE_LOCK_GIVE(aclRwLock);

  return rc;
}

/*********************************************************************
*
* @purpose  Get the first access list rule given an ACL ID
*
* @param    L7_uint32   aclnum   the access-list identifying number
* @param    L7_uint32   *rule    the next rule ID
*
* @returns  L7_SUCCESS, if the first rule for this ACL ID was found
* @returns  L7_FAILURE, 
* @returns  L7_ERROR,   if no rules have been created for this ACL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleGetFirst(L7_uint32 aclnum, L7_uint32 *rule)
{
  L7_RC_t rc;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpRuleGetFirst(aclnum, rule);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Get the next access list rule given an ACL ID
*
* @param    L7_uint32   aclnum   the access-list ID
* @param    L7_uint32   rule     the current rule ID
* @param    L7_uint32  *next     the next rule ID
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_FAILURE, if rule is the last valid rule ID for this ACL
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleGetNext(L7_uint32 aclnum, L7_uint32 rule, L7_uint32 *next)
{
  L7_RC_t rc;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpRuleGetNext(aclnum, rule, next);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if aclnum is a valid, configured access list
*
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleCheckValid(L7_uint32 aclnum, L7_uint32 rulenum)
{
  aclRuleParms_t *acl_rp;
  L7_RC_t rc;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum,rulenum,&acl_rp);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the ACL rule number belongs to a supported number range
*
* @param    L7_uint32   aclnum     the access-list identifying number
* @param    L7_uint32   rulenum    the current rule ID
*
* @returns  L7_SUCCESS, if the rule ID belongs to a supported range
* @returns  L7_FAILURE, if the rule ID is out of range
*
* @comments The aclnum parameter is required by this function in case there 
*           is ever a need to support separate rule number ranges based
*           on the type of ACL (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleNumRangeCheck(L7_uint32 aclnum, L7_uint32 rulenum)
{
  /* no need for read/write lock control here */
  return aclImpRuleNumRangeCheck(aclnum, rulenum);
}

/*********************************************************************
*
* @purpose  Checks if aclnum is currently assigned to this intf,dir pair
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  intf       the interface number
* @param    L7_uint32  direction  the interface direction
*
* @returns  L7_SUCCESS, if the ACL is assigned to this intf,dir
* @returns  L7_FAILURE, if the ACL is not assigned to this intf,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirCheckValid(L7_uint32 aclnum, L7_uint32 intf, L7_uint32 direction)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 aclDir, i;
  aclIntfCfgData_t *pCfg;
  aclIntfParms_t   *pList;

  /* check validity of aclnum */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS) 
    return L7_FAILURE;

  if (aclMapIntfIsConfigurable(intf, &pCfg) != L7_TRUE)
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

    if (pList->id.aclType == L7_ACL_TYPE_IP 
        || pList->id.aclType == L7_ACL_TYPE_IPV6)
    {
      if (pList->id.aclId == aclnum)
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
* @purpose Check to see if vlanNum is a valid VLAN ID for the purpose
*          of binding ACLs to it.
*
* @param L7_uint32 vlan       @b((input)) VLAN Number
*
* @returns L7_TRUE     If valid VLAN
* @returns L7_FALSE    If not valid VLAN
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL aclIsValidVlan( L7_uint32 vlanNum )
{
  /* 
   * place any checking required to validate that the vlanNum provided is 
   * suitable for binding ACLs here
   */
  if ((vlanNum < L7_ACL_MIN_VLAN_ID) ||
      (vlanNum > L7_ACL_MAX_VLAN_ID))
  {
    return(L7_FALSE);
  }
  return( L7_TRUE );
}

/*********************************************************************
*
* @purpose  Checks if aclnum is currently assigned to this vlan,dir pair
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  vlan       the VLAN number
* @param    L7_uint32  direction  the traffic direction
*
* @returns  L7_SUCCESS, if the ACL is assigned to this vlan,dir
* @returns  L7_FAILURE, if the ACL is not assigned to this vlan,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirCheckValid( L7_uint32 aclnum,
                              L7_uint32 vlan,
                              L7_uint32 direction )
{
  L7_RC_t           rc = L7_FAILURE;
  L7_uint32         aclDir;
  L7_uint32         i;
  aclVlanCfgData_t *pCfg;
  aclIntfParms_t   *pList;

  /* check validity of aclnum */
  if ( aclImpNumRangeCheck( aclnum ) != L7_SUCCESS )
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
    return( L7_FAILURE );
  }

  if ( osapiReadLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  pList = pCfg->vlan_dir_Tb[aclDir].assignList;

  for ( i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++, pList++ )
  {
    if ( pList->inUse != L7_TRUE )
    {
      break;
    }

    if ( L7_ACL_TYPE_IP == pList->id.aclType 
         || L7_ACL_TYPE_IPV6 == pList->id.aclType)
    {
      if ( pList->id.aclId == aclnum )
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
* @purpose  Checks to see if a user has configured a field in a rule
*           in an access list
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  field     ruleFields_t enum 
*
* @returns  L7_TRUE, if the field has been configured
* @returns  L7_FALSE, if the field has not been configured
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL aclIsFieldConfigured(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 field)
{
  L7_BOOL rc;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FALSE;

  rc = aclImpIsFieldConfigured(aclnum, rulenum, field);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FALSE;

  return rc;
}

/*********************************************************************
*
* @purpose  To get the action for a rule.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *action  action type (permit/deny) (aclActionIndex_t)
*
* @returns  L7_SUCCESS, if the access option is retrieved
* @returns  L7_FAILURE, if there is no action set
* @returns  L7_ERROR,   if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleActionGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *action)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (action == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *action = acl_rp->action;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the assigned queue id from an existing access list rule entry.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *queueId   the queue id         
*
* @returns  L7_SUCCESS, if the queue id is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleAssignQueueIdGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *queueId)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (queueId == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *queueId = acl_rp->assignQueueId;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the redirect interface config id from an existing 
*           access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    nimConfigID_t *configId the redirect interface config id
*
* @returns  L7_SUCCESS, if the interface number is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleRedirectConfigIdGet(L7_uint32 aclnum, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (configId == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  NIM_CONFIG_ID_COPY(configId, &acl_rp->redirectConfigId);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the mirror interface config id from an existing 
*           access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    nimConfigID_t *configId the mirror interface config id
*
* @returns  L7_SUCCESS, if the interface number is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleMirrorConfigIdGet(L7_uint32 aclnum, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (configId == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  NIM_CONFIG_ID_COPY(configId, &acl_rp->mirrorConfigId);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the logging flag from an existing access list rule entry.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *logFlag   the rule logging flag value
*
* @returns  L7_SUCCESS, if the logging flag is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleLoggingGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_BOOL *logFlag)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (logFlag == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *logFlag = acl_rp->logging;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get a protocol from an existing access list entry.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *protocol  the protocol number
*
* @returns  L7_SUCCESS, if the protocol is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleProtocolGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *protocol)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (protocol == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *protocol = (L7_uint32)acl_rp->protocol;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the source ip address and mask.
*
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  aclnum   the access-list entry id
* @param    L7_uint32  *ipAddr  the ip address
* @param    L7_uint32  *mask    the ip address mask
*
* @returns  L7_SUCCESS, if source ip address and mask are added
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleSrcIpMaskGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                            L7_uint32 *ipAddr, L7_uint32 *mask)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if ((aclImpStdAclNumRangeCheck(aclnum) != L7_SUCCESS) &&
      (aclImpExtGroupRangeCheck(aclnum) != L7_SUCCESS))
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (ipAddr == L7_NULLPTR)
    return L7_FAILURE;
  if (mask == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *ipAddr = acl_rp->srcIp.v4.addr;
  *mask   = acl_rp->srcIp.v4.mask;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the source IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if source ipv6 address and prefix len are retrieved
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleSrcIpv6AddrGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                              L7_in6_prefix_t *addr6)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpNamedIpv6IndexRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (addr6 == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *addr6 = acl_rp->srcIp.v6;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the L4 source port.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *port    L4 port number
*
* @returns  L7_SUCCESS, if L4 source port is retrieved
* @returns  L7_FAILURE, if invalid list number or entry id 
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleSrcL4PortGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *port)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check validity of aclnum and rulenum */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (port == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *port = acl_rp->srcPort;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the range of the source layer 4 ports of a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  rulenum     the acl rule number
* @param    L7_uint32  *startport  the start port number
* @param    L7_uint32  *endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is retrieved
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleSrcL4PortRangeGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                 L7_uint32 *startport, L7_uint32 *endport)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (startport == L7_NULLPTR)
    return L7_FAILURE;
  if (endport == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *startport = acl_rp->srcStartPort;
  *endport = acl_rp->srcEndPort;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the destination ip address and mask.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *ipAddr  the ip address
* @param    L7_uint32  *mask    the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are retrieved
* @returns  L7_FAILURE, if invalid list number, entry id, or port
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleDstIpMaskGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                            L7_uint32 *ipAddr, L7_uint32 *mask)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpExtGroupRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (ipAddr == L7_NULLPTR)
    return L7_FAILURE;
  if (mask == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *ipAddr = acl_rp->dstIp.v4.addr;
  *mask = acl_rp->dstIp.v4.mask;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the destination IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if destination ipv6 address and prefix len are retrieved
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleDstIpv6AddrGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                              L7_in6_prefix_t *addr6)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpNamedIpv6IndexRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (addr6 == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *addr6 = acl_rp->dstIp.v6;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the L4 destination port.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *port    the L4 port number
*
* @returns  L7_SUCCESS, if destination ip address, net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or ip
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleDstL4PortGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *port)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (port == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *port = acl_rp->dstPort;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the range of the destination layer 4 ports for a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  rulenum     the acl rule number
* @param    L7_uint32  *startport  the start port number
* @param    L7_uint32  *endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is retrieved
* @returns  L7_FAILURE, if invalid list number, entry id, or port
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleDstL4PortRangeGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                 L7_uint32 *startport, L7_uint32 *endport)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (startport == L7_NULLPTR)
    return L7_FAILURE;
  if (endport == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *startport = acl_rp->dstStartPort;
  *endport = acl_rp->dstEndPort;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the match-all or 'every' status for a rule
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  every    true or false, match every packet
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleEveryGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_BOOL *every)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (every == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *every = acl_rp->every;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the differentiated services code point (dscp) value
*           for a rule in an existing access list.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  *dscpval  the diffserv code point value
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleIPDscpGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *dscpval)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpExtGroupOrIpv6RangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (dscpval == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *dscpval = 0;
  *dscpval = (L7_uint32) ((acl_rp->tosbyte) >> L7_QOS_ACL_TLV_MATCH_IPDSCP_SHIFT);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the precedence value for a rule in an existing
*           access list.
*
* @param    L7_uint32  aclnum          access-list identifying number
* @param    L7_uint32  rulenum         the acl rule number
* @param    L7_uint32  *precedenceval  the precedence value
*
* @returns  L7_SUCCESS, if the precedence value is retrieved
* @returns  L7_FAILURE, if invalid list number, entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleIPPrecedenceGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *precedenceval)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpExtGroupRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (precedenceval == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *precedenceval = (L7_uint32) ((acl_rp->tosbyte) >> L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_SHIFT);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the type of service bits and mask.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  *tosbits  tos bits
* @param    L7_uint32  *tosmask  tos mask
*
* @returns  L7_SUCCESS, if the service type parameters are added
* @returns  L7_FAILURE, if invalid list number, entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleIPTosGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                        L7_uint32 *tosbits, L7_uint32 *tosmask)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpExtGroupRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (tosbits == L7_NULLPTR)
    return L7_FAILURE;
  if (tosmask == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *tosbits = (L7_uint32)acl_rp->tosbyte;
  *tosmask = (L7_uint32)acl_rp->tosmask;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the IPv6 flow label.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  *flowlbl  flow label value
*
* @returns  L7_SUCCESS, if the value is retrieved
* @returns  L7_FAILURE, if invalid list number, entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleIpv6FlowLabelGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                L7_uint32 *flowlbl)
{
  L7_RC_t rc;
  aclRuleParms_t *acl_rp;

  /* check inputs */
  if (aclImpNamedIpv6IndexRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS)
    return L7_FAILURE;
  if (flowlbl == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGet(aclnum, rulenum, &acl_rp);
  if (rc == L7_ERROR)
  {
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_ERROR;
  }

  *flowlbl = (L7_uint32)acl_rp->flowlbl;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           interface and direction
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid interface, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the interface in this 
*                       direction
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @comments The aclId contained in the output listInfo must be interpreted 
*           based on the aclType (for example, aclnum for IP, aclIndex for 
*           MAC).
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirAclListGet(L7_uint32 intIfNum, L7_uint32 direction,
                             L7_ACL_INTF_DIR_LIST_t *listInfo)
{
  L7_RC_t           rc;
  L7_uint32         aclDir;
  aclIntfCfgData_t  *pCfg;

  /* check for a configurable interface */
  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* check for valid direction and translate to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  /* check pointer parm */
  if (listInfo == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpIntfDirAclListGet(intIfNum, aclDir, listInfo);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Gets a list of interfaces to which an ACL is assigned in the
*           specified direction
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  direction   the direction of application of ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  *intfList   list of interfaces (L7_ACL_INTF_MAX_COUNT long)
*
* @returns  L7_SUCCESS, if a list of interfaces was built
* @returns  L7_FAILURE, if invalid identifying number or 
*                       internal interface number does not exist
* @returns  L7_ERROR,   if access-list not assigned to any interface
*                       in this direction (no interface list was built)
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @end
*
*********************************************************************/
L7_RC_t aclAssignedIntfDirListGet(L7_uint32 aclnum, L7_uint32 direction,
                                  L7_ACL_ASSIGNED_INTF_LIST_t *intfList)
{                            
  L7_RC_t rc;
  L7_uint32 aclDir;

  /* check inputs */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;
  if (intfList == L7_NULLPTR)
    return L7_FAILURE;

  /* translate direction to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpAssignedIntfDirListGet(aclnum, aclDir, intfList);

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  To check if an interface and direction is in use by any access list.
*
* @param    L7_uint32  intIfNum  the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_TRUE, if the intf,dir has at least one ACL attached
* @returns  L7_FALSE, if the intf,dir has no ACL attached
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL aclIsInterfaceInUse(L7_uint32 intIfNum, L7_uint32 direction)
{
  L7_uint32 aclDir;
  L7_BOOL rc = L7_FALSE;
  aclIntfCfgData_t  *pCfg;

  /* check for valid direction and translate to internal value */
  if (aclDirExtToInt(direction, &aclDir) != L7_SUCCESS)
    return L7_FALSE;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
      return L7_FALSE;

    rc = aclImpIsInterfaceInUse(intIfNum, aclDir);

    if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
      return L7_FALSE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To add an access list to the specified vlan and direction.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  seqNum     the ACL evaluation order sequence number
*
* @returns  L7_SUCCESS, if ACL successfully added to vlan,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED, if vlan,dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED, if feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL, if ACL assignment at capacity for vlan, dir
* @returns  L7_REQUEST_DENIED, if error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirectionAdd( L7_uint32 vlanNum,
                             L7_uint32 direction,
                             L7_uint32 aclnum,
                             L7_uint32 seqNum )
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check for valid direction and translate to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  if ( osapiWriteLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  rc = aclImpVlanDirectionAdd( vlanNum, aclDir, aclnum, seqNum );

  if ( osapiWriteLockGive( aclRwLock ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  To remove an vlan and the access list application
*           direction to an existing access list.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if ACL successfully removed from vlan,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL does not exist for this aclnum
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirectionRemove( L7_uint32 vlanNum,
                                L7_uint32 direction,
                                L7_uint32 aclnum )
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check for valid direction and translate to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  if ( osapiWriteLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  rc = aclImpVlanDirectionRemove( vlanNum, aclDir, aclnum );

  if ( osapiWriteLockGive( aclRwLock ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  Verifies that the specified vlan and direction is valid
*           for use with ACL
*
* @param    vlanNum    @b{(input)}   interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS  vlan,dir is valid for ACL
* @returns  L7_ERROR    vlan,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the vlan
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the vlan
*           and direction (see usmDbQosAclIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirGet( L7_uint32 vlanNum,
                       L7_uint32 direction )
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check for supported VLAN */
  if ( L7_FALSE == aclIsValidVlan( vlanNum ) )
  {
    return( L7_FAILURE );
  }

  /* check for valid direction and translate to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  if ( osapiWriteLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }
  rc = aclImpVlanDirGet( vlanNum, aclDir );
  osapiWriteLockGive( aclRwLock );
  return( rc );
}

/*********************************************************************
*
* @purpose  Get next sequential vlan and direction that is valid
*           for use with ACL
*
* @param    vlanNum         @b{(input)}  interal vlan number
* @param    direction       @b{(input)}  vlan direction
*                                        (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *nextVlanNum    @b{(output)} next interal vlan number
* @param    *nextDirection  @b{(output)} next vlan direction (internal)
*
* @returns  L7_SUCCESS  next vlan,dir for ACL was obtained
* @returns  L7_ERROR    no more valid vlan,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an vlan and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the vlan
*           and direction (see usmDbQosAclIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirGetNext( L7_uint32 vlanNum,
                           L7_uint32 direction,
                           L7_uint32 *nextVlanNum,
                           L7_uint32 *nextDirection )
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* translate direction to internal value
   *
   * NOTE:  Anything other than inbound direction is treated as outbound
   *        for purposes of finding next direction
   */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    aclDir = ACL_INTF_DIR_OUT;
  }

  /* check output pointers */
  if ( ( L7_NULLPTR == nextVlanNum ) ||
       ( L7_NULLPTR == nextDirection ) )
  {
    return( L7_FAILURE );
  }

  if ( osapiWriteLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }
  rc = aclImpVlanDirGetNext( vlanNum, aclDir, nextVlanNum, &aclDir );
  if ( L7_SUCCESS == rc )
  {
    /* translate internal direction to external API value */
    if ( aclDirIntToExt( aclDir, nextDirection ) != L7_SUCCESS )
    {
      rc = L7_FAILURE;
    }
  }
  osapiWriteLockGive( aclRwLock );
  return( rc );
}

/*********************************************************************
*
* @purpose  Get next sequential vlan ID configured with an ACL binding.
*
* @param    vlanNum       @b{(input)}  current vlan ID
* @param    *nextVlanNum  @b{(input)}  next vlan ID, if any
*
* @returns  L7_SUCCESS  next vlan ID was obtained
* @returns  L7_ERROR    invalid parameter or other failure
* @returns  L7_FAILURE  no more vlan IDs found
*
* @comments Use a vlanid value of 0 to find the first vlan ID configured
*           with an ACL binding.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanIdGetNext(L7_uint32 vlanNum, L7_uint32 *nextVlanNum)
{
  L7_RC_t rc;

  if (osapiWriteLockTake(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return(L7_ERROR);
  }

  rc = aclImpVlanIdGetNext(vlanNum, nextVlanNum);

  if (osapiWriteLockGive(aclRwLock) != L7_SUCCESS)
  {
    return(L7_ERROR);
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Verifies that the ACL sequence number is in use for the
*           specified vlan and direction.
*
* @param    vlanNum    @b{(input)}  interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
*
* @returns  L7_SUCCESS  sequence number exists for vlan,dir
* @returns  L7_ERROR    sequence number does not exist for vlan,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments A sequence number value may be used only once for a given
*           vlan and direction pair.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirSequenceGet( L7_uint32 vlanNum,
                               L7_uint32 direction,
                               L7_uint32 seqNum )
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check for supported VLAN */
  if ( L7_FALSE == aclIsValidVlan( vlanNum ) )
  {
    return( L7_FAILURE );
  }

  /* check for valid direction and translate to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  /* check if sequence number is in range */
  if ( ( seqNum < L7_ACL_MIN_VLAN_SEQ_NUM ) ||
       ( seqNum > L7_ACL_MAX_VLAN_SEQ_NUM ) )
  {
    return( L7_FAILURE );
  }

  if ( osapiWriteLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }
  rc = aclImpVlanDirSequenceGet( vlanNum, aclDir, seqNum );
  osapiWriteLockGive( aclRwLock );
  return( rc );
}

/*********************************************************************
*
* @purpose  Get next sequential ACL vlan sequence number in use
*           for the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
* @param    *nextSeqNum @b{(output)} next ACL vlan sequence number
*
* @returns  L7_SUCCESS  next sequence number was obtained
* @returns  L7_ERROR    no more sequence numbers for vlan,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments Use a seqNum value of 0 to find the first ACL sequence
*           number for this vlan and direction, if it exists.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirSequenceGetNext( L7_uint32 vlanNum,
                                   L7_uint32 direction,
                                   L7_uint32 seqNum,
                                   L7_uint32 *nextSeqNum )
{
  L7_uint32 aclDir;
  L7_RC_t rc;

  /* check for supported VLAN */
  if ( L7_FALSE == aclIsValidVlan( vlanNum ) )
  {
    return( L7_FAILURE );
  }

  /* check for valid direction and translate to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  /* NOTE:  Any seqNum value is allowed as input to a getNext function */

  /* check output pointer */
  if ( L7_NULLPTR == nextSeqNum )
  {
    return( L7_FAILURE );
  }

  if ( osapiWriteLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }
  rc = aclImpVlanDirSequenceGetNext( vlanNum, aclDir, seqNum, nextSeqNum );
  osapiWriteLockGive( aclRwLock );
  return( rc );
}

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           vlan and direction
*
* @param    vlanNum     @b{(input)}  interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid vlan, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the vlan in this
*                       direction
*
* @comments An ACL can be shared/associated with multiple vlans.
*
* @comments The aclId contained in the output listInfo must be interpreted
*           based on the aclType (for example, aclnum for IP, aclIndex for
*           MAC).
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirAclListGet( L7_uint32 vlanNum,
                              L7_uint32 direction,
                              L7_ACL_VLAN_DIR_LIST_t *listInfo )
{
  L7_RC_t           rc = L7_FAILURE;
  L7_uint32         aclDir;
  aclVlanCfgData_t  *pCfg;

  /* check for a configurable interface */
  if ( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  /* check for valid direction and translate to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  /* check pointer parm */
  if ( L7_NULLPTR == listInfo )
  {
    return( L7_FAILURE );
  }

  if ( osapiReadLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  rc = aclImpVlanDirAclListGet( vlanNum, aclDir, listInfo );

  if ( osapiReadLockGive( aclRwLock ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  Gets a list of vlans to which an ACL is assigned in the
*           specified direction
*
* @param    L7_uint32  aclnum      the access-list identifying number
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
L7_RC_t aclAssignedVlanDirListGet( L7_uint32 aclnum,
                                   L7_uint32 direction,
                                   L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList )
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 aclDir;

  /* check inputs */
  if ( ( aclImpNumRangeCheck( aclnum ) != L7_SUCCESS ) ||
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
  rc = aclImpAssignedVlanDirListGet( aclnum, aclDir, vlanList );

  if ( osapiReadLockGive( aclRwLock ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  To check if an vlan and direction is in use by any access list.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_TRUE, if the vlan,dir has at least one ACL attached
* @returns  L7_FALSE, if the vlan,dir has no ACL attached
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL aclIsVlanInUse( L7_uint32 vlanNum,
                        L7_uint32 direction )
{
  L7_uint32         aclDir;
  L7_BOOL           rc = L7_FALSE;
  aclVlanCfgData_t *pCfg;

  /* check for valid direction and translate to internal value */
  if ( aclDirExtToInt( direction, &aclDir ) != L7_SUCCESS )
  {
    return( L7_FALSE );
  }

  if ( L7_TRUE == aclMapVlanIsConfigurable( vlanNum, &pCfg ) )
  {
    if ( osapiReadLockTake( aclRwLock, L7_WAIT_FOREVER ) != L7_SUCCESS )
    {
      return( L7_FALSE );
    }
    rc = aclImpIsVlanInUse( vlanNum, aclDir );

    if ( osapiReadLockGive( aclRwLock ) != L7_SUCCESS )
    {
      return( L7_FALSE );
    }
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
* @comments This value encompasses all ACL types.
*
* @end
*********************************************************************/
L7_RC_t aclMaxNumGet(L7_uint32 *pMax)
{
  *pMax = L7_ACL_MAX_LISTS;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the current number of ACLs configured
*
* @param    pCurr  @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value encompasses all ACL types.
*
* @end
*********************************************************************/
L7_RC_t aclCurrNumGet(L7_uint32 *pCurr)
{
  L7_RC_t   rc;
  L7_uint32 aclCountIp = 0, aclCountMac = 0;

  /* check inputs */
  if (pCurr == L7_NULLPTR)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpCurrNumGet(&aclCountIp);
  if (rc == L7_SUCCESS)
    rc = aclMacImpCurrNumGet(&aclCountMac);

  *pCurr = aclCountIp + aclCountMac;

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Creates a single TLV containing active rules of the specified
*           ACL within it and return the data in TLV block storage
*           provided by the caller.
*
* @param    aclnum      @b{(input)}  access-list identifying number
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
L7_RC_t aclTlvGet(L7_uint32 aclnum, L7_tlvHandle_t tlvHandle)
{
  L7_RC_t       rc;
  L7_ACL_TYPE_t aclType;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  aclType = aclImpTypeFromIndexGet(aclnum);
  if (aclImpNumCheckValid(aclnum) != L7_SUCCESS)
  {
    aclType = L7_ACL_TYPE_NONE;
  }

  if ((aclType == L7_ACL_TYPE_IP) ||
      (aclType == L7_ACL_TYPE_IPV6))
  {
    rc = aclImpTlvGet(aclType, aclnum, tlvHandle);
  }
  else
  {
    rc = L7_ERROR;                      /* ACL not found for allowed types */
  }

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*************************************************************************
* @purpose  Generic callback registration function for signaling ACL config changes.
*           configuration change to an ACL. 
*
* @param    tableId           @b{(input)} callback table identifier
* @param    funcPtr           @b{(input)} generic pointer to callback function
* @param    compId            @b{(input)} component ID
* @param    displayStr        @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS if successful
*           L7_REQUEST_DENIED if the ACL component is not yet initialized
*           L7_FAILURE if function pointer is bad or if the maximum
*                      number of callbacks are already registered.
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           bu the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
* @end
*********************************************************************/
static L7_RC_t aclCommonCallbackRegister(aclCallbackTableId_t tableId,
                                         L7_VOIDFUNCPTR_t funcPtr,
                                         L7_COMPONENT_IDS_t compId,
                                         L7_uchar8 *displayStr)
{
  L7_uint32 i, maxEnt;
  aclCallbackEntry_t *pTableStart, *pCbEntry;
  L7_RC_t rc = L7_FAILURE;

  if (aclCallbackRegisterIsReady() != L7_TRUE)
    return L7_REQUEST_DENIED;

  if ((tableId < ACL_CALLBACK_TABLE_ID_FIRST) ||
      (tableId >= ACL_CALLBACK_TABLE_ID_TOTAL))
  {
    LOG_MSG("Invalid ACL callback registration table ID %d for component %u\n", 
            tableId, compId);
    return L7_FAILURE;
  }

  if ((funcPtr == L7_NULLPTR) ||
      ((compId <= L7_FIRST_COMPONENT_ID) || (compId >= L7_LAST_COMPONENT_ID)))
  {
    LOG_MSG("Invalid ACL callback registration for component %u\n", compId);
    return L7_FAILURE;
  }

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* get table info */
  pTableStart = pAclCallbackInfo_g->table[tableId].pStart;
  maxEnt      = pAclCallbackInfo_g->table[tableId].maxEntries;

  /* prevent duplicate registration */
  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr == funcPtr)
    {
      (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      return L7_SUCCESS;
    }
  }

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr == L7_NULLPTR)
    {
      /* this index available */
      pCbEntry->funcPtr = funcPtr;
      pCbEntry->compId = compId;
      if (displayStr != L7_NULLPTR)
      {
        osapiStrncpySafe(pCbEntry->displayStr, displayStr, sizeof(pCbEntry->displayStr));
      }
      rc = L7_SUCCESS;
      break;
    }   
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*************************************************************************
* @purpose  Generic callback unregistration function for signaling ACL config changes.
*
* @param    tableId           @b{(input)} callback table identifier
* @param    funcPtr           @b{(input)} generic pointer to callback function
*
* @returns  L7_SUCCESS if successful
*           L7_REQUEST_DENIED if the ACL component is not yet initialized
*           L7_FAILURE if function pointer is NULL or if it is not found
*
* @comments 
*
* @end
*********************************************************************/
static L7_RC_t aclCommonCallbackUnregister(aclCallbackTableId_t tableId,
                                           L7_VOIDFUNCPTR_t funcPtr)
{
  L7_VOIDFUNCPTR_t voidFuncPtr = L7_NULLPTR;
  L7_uint32 i, maxEnt;
  aclCallbackEntry_t *pTableStart, *pCbEntry;
  L7_RC_t rc = L7_FAILURE;

  if (aclCallbackRegisterIsReady() != L7_TRUE)
    return L7_REQUEST_DENIED;

  if ((tableId < ACL_CALLBACK_TABLE_ID_FIRST) ||
      (tableId >= ACL_CALLBACK_TABLE_ID_TOTAL) ||
      (funcPtr == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* get table info */
  pTableStart = pAclCallbackInfo_g->table[tableId].pStart;
  maxEnt      = pAclCallbackInfo_g->table[tableId].maxEntries;

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr == funcPtr)
    {
      pCbEntry->funcPtr = voidFuncPtr;
      pCbEntry->compId = 0;
      memset(pCbEntry->displayStr, 0, sizeof(pCbEntry->displayStr));
      rc = L7_SUCCESS;
      break;
    }
  }

  if (ACL_WRITE_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/*************************************************************************
* @purpose  Generic function to indicate whether a callback routine is already registered
*           for ACL changes.
*
* @param    tableId           @b{(input)} callback table identifier
* @param    funcPtr           @b{(input)} generic pointer to callback function
*
* @returns  L7_TRUE if already registered
*           L7_FALSE if not registered
*
* @comments 
*
* @end
*********************************************************************/
static L7_BOOL aclCommonCallbackIsRegistered(aclCallbackTableId_t tableId,
                                             L7_VOIDFUNCPTR_t funcPtr)
{
  L7_uint32 i, maxEnt;
  aclCallbackEntry_t *pTableStart, *pCbEntry;
  L7_BOOL rc = L7_FALSE;

  if (aclCallbackRegisterIsReady() != L7_TRUE)
    return L7_FALSE;

  if ((tableId < ACL_CALLBACK_TABLE_ID_FIRST) ||
      (tableId >= ACL_CALLBACK_TABLE_ID_TOTAL) ||
      (funcPtr == L7_NULLPTR))
  {
    return L7_FALSE;
  }

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FALSE;

  /* get table info */
  pTableStart = pAclCallbackInfo_g->table[tableId].pStart;
  maxEnt      = pAclCallbackInfo_g->table[tableId].maxEntries;

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr == funcPtr)
    {
      rc = L7_TRUE;
      break;
    }
  }

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FALSE;

  return rc;
}

/*************************************************************************
* @purpose  Generic function to display contents of the specified ACL callback table.
*
* @param    tableId           @b{(input)} callback table identifier
* @param    tableNameStr      @b{(input)} table name display string
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
static L7_RC_t aclCommonCallbackTableShow(aclCallbackTableId_t tableId,
                                          L7_uchar8 *tableNameStr)
{
  L7_uint32 i, maxEnt;
  aclCallbackEntry_t *pTableStart, *pCbEntry;
  L7_char8 compIdStr[L7_COMPONENT_MNEMONIC_MAX_LEN];
  L7_BOOL displayHdr = L7_TRUE;
  L7_uint32 msgLvlReqd = ACL_MSGLVL_ON;

  if (aclCallbackRegisterIsReady() != L7_TRUE)
    return L7_REQUEST_DENIED;

  if ((tableId < ACL_CALLBACK_TABLE_ID_FIRST) ||
      (tableId >= ACL_CALLBACK_TABLE_ID_TOTAL) ||
      (tableNameStr == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* get table info */
  pTableStart = pAclCallbackInfo_g->table[tableId].pStart;
  maxEnt      = pAclCallbackInfo_g->table[tableId].maxEntries;

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr != L7_NULLPTR)
    {
      /* only display the header output prior to the first valid entry */
      if (displayHdr == L7_TRUE)
      {
        ACL_PRT(msgLvlReqd, "\nContents of %s table:\n", tableNameStr);
        displayHdr = L7_FALSE;
      }

      if (cnfgrApiComponentMnemonicGet(pCbEntry->compId, compIdStr) != L7_SUCCESS)
      {
        osapiSnprintf(compIdStr, sizeof(compIdStr), "<unknown>");
      }
      ACL_PRT(msgLvlReqd,
              " [%2u] funcPtr=0x%8.8x comp=%-*s descr=%s\n",
              i, (L7_uint32)pCbEntry->funcPtr, sizeof(compIdStr), compIdStr, pCbEntry->displayStr);
    }
  } /* endfor */

  ACL_PRT(msgLvlReqd, "\n");

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Register a route filter callback function to be called when there is a 
*           configuration change to an ACL. 
*
* @param    funcPtr     @b{(input)} pointer to the callback function
* @param    compId      @b{(input)} component ID
* @param    displayStr  @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer bad, or maximum callbacks already registered
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           by the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
* @end
*********************************************************************/
L7_RC_t aclRouteFilterCallbackRegister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr,
                                       L7_COMPONENT_IDS_t compId,
                                       L7_uchar8 *displayStr)
{
  return aclCommonCallbackRegister(ACL_CALLBACK_TABLE_ID_ROUTE_FILTER, 
                                   (L7_VOIDFUNCPTR_t)funcPtr, compId, displayStr);
}

/*************************************************************************
* @purpose  Remove a route filter callback function from the list of functions to
*           be called when an ACL changes. 
*
* @param    funcPtr  @b{(input)} pointer to callback function to unregister
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer NULL or not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclRouteFilterCallbackUnregister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr)
{
  return aclCommonCallbackUnregister(ACL_CALLBACK_TABLE_ID_ROUTE_FILTER, 
                                     (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Indicate whether a route filter callback function is already registered
*           for ACL changes.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
*
* @returns  L7_TRUE     already registered
* @returns  L7_FALSE    not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_BOOL aclRouteFilterCallbackIsRegistered(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr)
{
  return aclCommonCallbackIsRegistered(ACL_CALLBACK_TABLE_ID_ROUTE_FILTER, 
                                       (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Display the contents of the route filter callback table.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclRouteFilterCallbackTableShow(void)
{
  return aclCommonCallbackTableShow(ACL_CALLBACK_TABLE_ID_ROUTE_FILTER, 
                                    "Route Filter Callback");
}

/*************************************************************************
* @purpose  Register a notify event callback function to be called 
*           whenever there is a configuration change to an ACL. 
*
* @param    funcPtr     @b{(input)} pointer to the callback function
* @param    compId      @b{(input)} component ID
* @param    displayStr  @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer bad, or maximum callbacks already registered
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           by the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
*
* @end
*********************************************************************/
L7_RC_t aclNotifyEventCallbackRegister(ACL_NOTIFY_EVENT_FUNCPTR_t funcPtr,
                                       L7_COMPONENT_IDS_t compId,
                                       L7_uchar8 *displayStr)
{
  return aclCommonCallbackRegister(ACL_CALLBACK_TABLE_ID_NOTIFY_EVENT, 
                                   (L7_VOIDFUNCPTR_t)funcPtr, compId, displayStr);
}

/*************************************************************************
* @purpose  Remove a notify event callback function from the registered list.
*
* @param    funcPtr  @b{(input)} pointer to callback function to unregister
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer NULL or not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclNotifyEventCallbackUnregister(ACL_NOTIFY_EVENT_FUNCPTR_t funcPtr)
{
  return aclCommonCallbackUnregister(ACL_CALLBACK_TABLE_ID_NOTIFY_EVENT, 
                                     (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Indicate whether a notify event callback function is already 
*           registered for ACL changes.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
*
* @returns  L7_TRUE     already registered
* @returns  L7_FALSE    not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclNotifyEventCallbackIsRegistered(ACL_NOTIFY_EVENT_FUNCPTR_t funcPtr)
{
  return aclCommonCallbackIsRegistered(ACL_CALLBACK_TABLE_ID_NOTIFY_EVENT, 
                                       (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Display the contents of the notify event callback table.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclNotifyEventCallbackTableShow(void)
{
  return aclCommonCallbackTableShow(ACL_CALLBACK_TABLE_ID_NOTIFY_EVENT, 
                                    "Notify Event Callback");
}

/*************************************************************************
* @purpose  Register a delete approval callback function to be called 
*           prior to deleting an ACL.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
* @param    compId      @b{(input)} component ID
* @param    displayStr  @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer bad, or maximum callbacks already registered
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           by the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
*
* @end
*********************************************************************/
L7_RC_t aclDeleteApprovalCallbackRegister(ACL_DELETE_APPROVAL_FUNCPTR_t funcPtr,
                                          L7_COMPONENT_IDS_t compId,
                                          L7_uchar8 *displayStr)
{
  return aclCommonCallbackRegister(ACL_CALLBACK_TABLE_ID_DELETE_APPROVAL,
                                   (L7_VOIDFUNCPTR_t)funcPtr, compId, displayStr);
}

/*************************************************************************
* @purpose  Remove a delete approval callback function from the registered list.
*
* @param    funcPtr  @b{(input)} pointer to callback function to unregister
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer NULL or not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclDeleteApprovalCallbackUnregister(ACL_DELETE_APPROVAL_FUNCPTR_t funcPtr)
{
  return aclCommonCallbackUnregister(ACL_CALLBACK_TABLE_ID_DELETE_APPROVAL, 
                                     (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Indicate whether a delete approval callback function is already 
*           registered for ACL changes.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
*
* @returns  L7_TRUE     already registered
* @returns  L7_FALSE    not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclDeleteApprovalCallbackIsRegistered(ACL_DELETE_APPROVAL_FUNCPTR_t funcPtr)
{
  return aclCommonCallbackIsRegistered(ACL_CALLBACK_TABLE_ID_DELETE_APPROVAL, 
                                       (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Display the contents of the delete approval callback table.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclDeleteApprovalCallbackTableShow(void)
{
  return aclCommonCallbackTableShow(ACL_CALLBACK_TABLE_ID_DELETE_APPROVAL, 
                                    "Delete Approval Callback");
}

/*********************************************************************
*
* @purpose  Indicates whether the ACL component can accept callback registrations
*
* @param    
*
* @returns  L7_TRUE if the ACL component ready for callback registrations
*           L7_FALSE otherwise.
*
* @comments An L7_FALSE return from this routine usually means the caller
*           is attempting to register too soon, such as during phase1 init
*           instead of phase2.
*
* @end
*
*********************************************************************/
L7_BOOL aclCallbackRegisterIsReady(void)
{
  return pAclCallbackInfo_g->isInitialized;
}

/*************************************************************************
* @purpose  Display the contents of all supported ACL callback tables.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclCallbackTableShow(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (aclCallbackRegisterIsReady() != L7_TRUE)
    return L7_REQUEST_DENIED;

  if (aclRouteFilterCallbackTableShow() != L7_SUCCESS)
    rc = L7_FAILURE;

  if (aclNotifyEventCallbackTableShow() != L7_SUCCESS)
    rc = L7_FAILURE;

  if (aclDeleteApprovalCallbackTableShow() != L7_SUCCESS)
    rc = L7_FAILURE;

  return rc;
}

/*************************************************************************
* @purpose  Apply an ACL as a route filter. 
*
* @param    aclnum  @b{(input)} number of access list to filter route
* @param    routePrefix  @b{(input)} Destination prefix of route
* @param    routeMask  @b{(input)} Destination mask of route
* @param    action  @b{(output)} FILTER_ACTION_PERMIT if the route passes the filter 
*                                FILTER_ACTION_DENY if the route does not pass
*
* @returns  L7_SUCCESS
*
* @comments If no ACL exists with the given aclnum, action is set to permit.
*           Rules that do not have a srcIp and srcMask field configured are
*           skipped. If a rule has destIp and dstMask configured, those fields
*           are applied in addition to srcIp and srcMask. All other fields are
*           ignored. The action is set to that of the first matching rule. 
*           If no rules are configured, the action is set to deny.
*
* @comments Note that CLI inverts inverted masks entered as part of 
*           access list statements, so no need to invert masks here.
*
* @end
*********************************************************************/
L7_RC_t aclRouteFilter(L7_uint32 aclnum,
                       L7_uint32 routePrefix,
                       L7_uint32 routeMask,
                       FILTER_ACTION_t *action)
{
  L7_RC_t rc;
  L7_uint32 rulenum;
  aclRuleParms_t *ruleParms;

  if (aclComponentInitialized() == L7_FALSE)
    return L7_FAILURE;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (aclFindACL(aclnum) == L7_NULL)
  {
    /* no ACL exists, so permit route */
    *action = FILTER_ACTION_PERMIT; 
    (void)ACL_READ_LOCK_GIVE(aclRwLock);
    return L7_SUCCESS;
  }

  /* Iterate through rules in ACL, looking for a rule that matches dest */
  *action = FILTER_ACTION_DENY;      /* default action */
  rc = aclImpRuleGetFirst(aclnum, &rulenum);
  while (rc == L7_SUCCESS)
  {
    if (aclRuleGet(aclnum, rulenum, &ruleParms) == L7_SUCCESS)
    {
      if (ruleParms->every == L7_TRUE)
      {
        *action = (FILTER_ACTION_t) ruleParms->action;
        break;
      }

      /* skip rules that don't have src address and mask configured */
      if (aclImpIsFieldConfigured(aclnum, rulenum, ACL_SRCIP) &&
          aclImpIsFieldConfigured(aclnum, rulenum, ACL_SRCIP_MASK)) 
      {
        if ((ruleParms->srcIp.v4.addr & ruleParms->srcIp.v4.mask) == 
            (routePrefix & ruleParms->srcIp.v4.mask))
        {
          /* prefix matches rule. */
          if ((aclImpIsFieldConfigured(aclnum, rulenum, ACL_DSTIP) == L7_FALSE) ||
              (aclImpIsFieldConfigured(aclnum, rulenum, ACL_DSTIP_MASK) == L7_FALSE))
          {
            /* destination params not configured, so no need to apply 
               them to mask */
            *action = (FILTER_ACTION_t) ruleParms->action;
            break;
          }
          if ((ruleParms->dstIp.v4.addr & ruleParms->dstIp.v4.mask) ==
              (routeMask & ruleParms->dstIp.v4.mask))
          {
            *action = (FILTER_ACTION_t) ruleParms->action;
            break;
          }
        }
      }
    }
    rc = aclImpRuleGetNext(aclnum, rulenum, &rulenum);
  }

  if (ACL_READ_LOCK_GIVE(aclRwLock) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Notify registered users of a change to an ACL.
*
* @param    aclnum      @b{(input)} ACL associated with the event
* @param    event       @b{(input)} Event indication
* @param    aclOldName  @b{(input)} Previous ACL name, or L7_NULLPTR
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
static void aclNotify(L7_uint32 aclnum, aclEvent_t event, L7_uchar8 *aclOldName)
{
  L7_ACL_TYPE_t   aclType;

  aclType = aclImpTypeFromIndexGet(aclnum);

  if ((aclType != L7_ACL_TYPE_IP) &&
      (aclType != L7_ACL_TYPE_IPV6))
    return;

  if (ACL_READ_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;

  aclImpNotify(event, aclType, aclnum, aclOldName);

  (void)ACL_READ_LOCK_GIVE(aclRwLock);
}

