/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename acl_mac.c
*
* @purpose MAC Access Control List internal functions
*
* @component ACL
*
* @comments This file contains implementation functions that touch the
*           actual ACL data structures. Functions in this file should
*           not call back out to functions acl_api.c or acl_mac_api.c,
*           since this risks creating deadlocks by attempting
*           to take the read/write lock more than once.
*
* @create 08/31/2004
*
* @author gpaussa
*
* @end
*
**********************************************************************/


#include <string.h>
#include <ctype.h>
#include "l7_common.h"
#include "comm_mask.h"
#include "osapi.h"
#include "osapi_support.h"
#include "tlv_api.h"
#include "acl_api.h"
#include "dtl_acl.h"
#include "usmdb_qos_acl_api.h"
#include "acl.h"
#include "sysapi.h"
#include "log.h"

avlTree_t         aclMacTree;
avlTree_t         *pAclMacTree = &aclMacTree;
avlTreeTables_t   aclMacTreeHeap[L7_ACL_MAX_LISTS+1];
aclMacStructure_t aclMacDataHeap[L7_ACL_MAX_LISTS+1];

/* Number of bytes in mask */
#define ACL_MAC_INDEX_INDICES           (((L7_ACL_MAX_LISTS + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[ACL_MAC_INDEX_INDICES];
} ACL_MAC_INDEX_MASK_t;

/* NOTE: Index 0 is reserved and must not be used */
static ACL_MAC_INDEX_MASK_t   aclMacIndexInUseMask;

L7_uint32         acl_mac_max_entries = L7_ACL_MAX_LISTS;
static L7_uint32  aclMacIndexNextFree = 0;         /* next available ACL index   */

char *aclMacEtypeKeyidStr[] = 
{ 
  "none", "custom", "appletalk", "arp", "ibmsna", "ipv4", "ipv6", "ipx",
  "mplsmcast", "mplsucast", "netbios", "novell", "pppoe", "rarp"
};

extern aclCfgFileData_t  *aclCfgFileData;
extern L7_uint32         acl_curr_entries_g;    /* total current list entries */

extern aclCnfgrState_t   aclCnfgrState;
extern char *aclRuleCfgMaskStr[];

extern char              *acl_direction_str[];

/*********************************************************************
*
* @purpose  To create a MAC access list tree.
*
* @param    maxListSize @b{(input)} number of nodes to create in ACL tree
*
* @returns  void
*
* @comments Creates a new instance of MAC ACL, allocating space for up to
*           maxListSize ACLs.
*
* @end
*
*********************************************************************/
void aclMacCreateTree(L7_uint32 maxListSize)
{
  /* check validity of 'maxListSize' */
  if ((maxListSize == 0) || (maxListSize > L7_ACL_MAX_LISTS))
  {
    acl_mac_max_entries = L7_ACL_MAX_LISTS;
  }
  else
  {
    acl_mac_max_entries = maxListSize;
  }

  avlCreateAvlTree(&aclMacTree, aclMacTreeHeap, aclMacDataHeap, acl_mac_max_entries, 
                   (L7_uint32)sizeof(aclMacStructure_t), 0x10, (L7_uint32)sizeof(L7_uint32));
  (void)avlSetAvlTreeComparator(&aclMacTree, avlCompareULong32);


  /* initialize the MAC ACL index in-use table */
  memset(&aclMacIndexInUseMask, 0, sizeof(aclMacIndexInUseMask));

  /* establish initial ACL index next values */
  aclMacImpIndexNextUpdate();
}


/*********************************************************************
*
* @purpose  To delete a MAC access list tree.
*
* @param    void
*
* @returne  void
*
* @comments Destroys the instance of MAC ACL, releasing the previously
*           allocated database and control space.
*
* @end
*
*********************************************************************/
void aclMacDeleteTree(void)
{
  avlPurgeAvlTree(&aclMacTree, acl_mac_max_entries);

  /* reset all MAC ACL index in-use table entries */
  memset(&aclMacIndexInUseMask, 0, sizeof(aclMacIndexInUseMask));

  /* reset MAC ACL index next value */
  aclMacIndexNextFree = 0;
}


/*********************************************************************
*
* @purpose  Provide the current value of the MAC ACL index next variable
*
* @param    *next       @b{(output)} access list index next value
*
* @returns  void
*
* @comments Only provides an output value if the next parm is non-null.
*
* @comments Does not guarantee this index value will be valid at the time
*           an ACL create is attempted.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclMacImpIndexNextRead(L7_uint32 *next)
{
  if (next != L7_NULLPTR)
    *next = aclMacIndexNextFree;
}


/*********************************************************************
*
* @purpose  Retrieve the min/max index values allowed for the MAC ACL table 
*
* @param    *pMin       @b{(output)} Pointer to index min output location
* @param    *pMax       @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
L7_RC_t aclMacImpIndexMinMaxGet(L7_uint32 *pMin, L7_uint32 *pMax)
{
  if ((pMin == L7_NULLPTR) || (pMax == L7_NULLPTR))
    return L7_FAILURE;

  *pMin = L7_ACL_MAC_MIN_INDEX;
  *pMax = (L7_ACL_MAC_MIN_INDEX + acl_mac_max_entries - 1);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize in-use table entry for a MAC ACL
*
* @param    void
*
* @returns  void  
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclMacImpIndexInUseInit(void)
{
  /* initialize the MAC ACL index in-use table */
  memset(&aclMacIndexInUseMask, 0, sizeof(aclMacIndexInUseMask));

  /* establish initial ACL index next values */
  aclMacImpIndexNextUpdate();
  return;
}

/*********************************************************************
*
* @purpose  Update in-use table entry for a MAC ACL
*
* @param    aclIndex    @b{(input)} access list index
* @param    inUse       @b{(input)} mark list in use or not
*
* @returns  void  
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclMacImpIndexInUseUpdate(L7_uint32 aclIndex, L7_BOOL inUse)
{
  if (L7_TRUE == inUse)
  {
    L7_INTF_SETMASKBIT(aclMacIndexInUseMask, ACL_MAC_INDEX_NORMALIZE(aclIndex));
  }
  else
  {
    L7_INTF_CLRMASKBIT(aclMacIndexInUseMask, ACL_MAC_INDEX_NORMALIZE(aclIndex));
  }
  return;
}

/*********************************************************************
*
* @purpose  Determine next available MAC acess list index value and 
*           update the 'IndexNext' value
*
* @param    void
*
* @returns  void  
*
* @comments Always looks for first index not already in use, starting with 1.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclMacImpIndexNextUpdate(void)
{
  L7_uint32     i, imax;

  imax = acl_mac_max_entries;

  for (i = 1; i <= imax; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * an ACL using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(aclMacIndexInUseMask, i) == L7_FALSE)
      break;
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > imax)
    aclMacIndexNextFree = 0;
  else
    aclMacIndexNextFree = (i - 1) + L7_ACL_MAC_MIN_INDEX;
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
* @comments This function is also used for a 'getFirst' search by 
*           specifying an aclIndex value of 0.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpIndexGetNext(L7_uint32 aclIndex, L7_uint32 *next)
{
  L7_RC_t           rc;
  aclMacStructure_t *p;

  p = (aclMacStructure_t *)avlSearchLVL7(&aclMacTree, &aclIndex, L7_MATCH_GETNEXT);
  
  if (p == L7_NULLPTR)
  {
    rc = L7_ERROR;
  }
  else 
  {
    *next = p->aclIndex;
    rc = L7_SUCCESS;
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  Checks if aclIndex is a valid, configured MAC access list
*
* @param    aclIndex    @b{(input)}  access list index to begin search
*
* @returns  L7_SUCCESS  MAC access list is valid
* @returns  L7_FAILURE  MAC access list not valid
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpIndexCheckValid(L7_uint32 aclIndex)
{
  L7_RC_t           rc = L7_SUCCESS;
  aclMacStructure_t *p;

  p = (aclMacStructure_t *)avlSearchLVL7(&aclMacTree, &aclIndex, L7_MATCH_EXACT); 
  if (p == L7_NULLPTR)
  {
    rc = L7_FAILURE;
  }
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
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpIndexRangeCheck(L7_uint32 aclIndex)
{
  L7_RC_t       rc = L7_FAILURE;

  if ((aclIndex >= L7_ACL_MAC_MIN_INDEX) && (aclIndex <= L7_ACL_MAC_MAX_INDEX))
    rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
*
* @purpose  To find a MAC access list identified by its aclIndex.
*
* @param    aclIndex    @b{(input)}  access list index
*
* @returns  void *      access list element pointer
* @returns  L7_NULLPTR  access list element not found for this aclIndex
*
* @comments
*
* @end
*
*********************************************************************/
void * aclMacFindACL(L7_uint32 aclIndex)
{
  return avlSearchLVL7(&aclMacTree, &aclIndex, L7_MATCH_EXACT);
}


/*********************************************************************
*
* @purpose  Checks if specified MAC access list rule field is configured
*
* @param    *acl_ptr    @b{(input)}  access list element pointer
* @param    rulenum     @b{(input)}  current rule number
* @param    field       @b{(input)}  rule field of interest
*
* @returns  L7_TRUE     rule field is configured
* @returns  L7_FALSE    rule field not configured
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_BOOL aclMacIsRuleFieldConfigured(void *acl_ptr, L7_uint32 rulenum, L7_uint32 field)
{
  L7_BOOL           found = L7_FALSE;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;
  L7_uint32         result;

  ptr = (aclMacStructure_t *)acl_ptr;
  curr = ptr->head;

  while ((curr != L7_NULLPTR) && (curr->last == L7_FALSE))
  {
    if (rulenum == curr->ruleNum)
    {
      found = L7_TRUE;
      break;
    }
    curr = curr->next;
  }

  if (found == L7_TRUE)
  {
    result = (curr->configMask) & (1 << field);
    if (result == 0)
    {
      found = L7_FALSE;
    }
  }

  return found;
} 


/*********************************************************************
*
* @purpose  To create a new MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS  access list created
* @returns  L7_FAILURE  invalid acl identifier, all other failures
* @returns  L7_ERROR    access list already exists
* @returns  L7_TABLE_IS_FULL  maximum number of ACLs or rule nodes already created
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacCreateACL(L7_uint32 aclIndex)
{
  L7_RC_t           rc;
  aclMacStructure_t acl;
  
  aclMacRuleParms_t *parms;
  aclMacStructure_t *ptr;

  rc = osapiSemaTake(aclMacTree.semId, L7_WAIT_FOREVER);
  if (rc == L7_SUCCESS)
  {
    /* check if overall number of access lists have already been created */
    if (aclImpIsTotalListsAtMax() == L7_TRUE)
    {
      (void)osapiSemaGive(aclMacTree.semId);
      return L7_TABLE_IS_FULL;
    }

    parms = (aclMacRuleParms_t *)aclRuleNodeAllocate((L7_uint32)sizeof(aclMacRuleParms_t));
    if (parms == L7_NULLPTR)
    {
      (void)osapiSemaGive(aclMacTree.semId);
      return L7_TABLE_IS_FULL;
    }

    memset(parms, 0, sizeof(*parms));
    memset(&acl, 0, sizeof(acl));

    acl.aclIndex = aclIndex;
    acl.ruleCount = 0;
    acl.head = parms;

    /* construct the implicit 'deny all' final rule */
    acl.head->ruleNum = L7_ACL_DEFAULT_RULE_NUM;
    acl.head->action = L7_ACL_DENY;
    acl.head->every = L7_TRUE;

    acl.head->last = L7_TRUE;
    acl.head->next = L7_NULLPTR;

    acl.nextacl = L7_NULLPTR;

    ptr = avlInsertEntry(&aclMacTree, &acl);
    if (ptr == L7_NULLPTR)
    {
      acl_curr_entries_g++;
      rc = L7_SUCCESS;
    }
    else
    {
      aclRuleNodeFree((void *)parms);
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      /* update the 'indexNext' value now that a new ACL was created */
      L7_INTF_SETMASKBIT(aclMacIndexInUseMask, ACL_MAC_INDEX_NORMALIZE(aclIndex));
      aclMacImpIndexNextUpdate();

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    }

    (void)osapiSemaGive(aclMacTree.semId);
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To delete the specified MAC access list
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
*
* @returns  L7_SUCCESS  access list deleted
* @returns  L7_FAILURE  invalid access list index, all other failures
* @returns  L7_ERROR    access list does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacDeleteACL(void *acl_ptr)
{
  L7_uint32                   aclIndex;
  L7_uint32                   d, i, rule;
  L7_RC_t                     rc;
  aclMacStructure_t           *ptr;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList;

  ptr = (aclMacStructure_t *)acl_ptr;
  aclIndex = ptr->aclIndex;

  /* remove ACL from all interfaces in each direction */
  for (d=0; d<ACL_INTF_DIR_MAX; d++)
  {
    rc = aclMacImpAssignedIntfDirListGet(ptr->aclIndex, d, &intfList);

    /* make sure ACL exists for an interface in order to remove it */
    if (rc != L7_SUCCESS)
      continue;

    for (i=0; i<intfList.count; i++)
    {
      rc = aclMacImpInterfaceDirectionRemove(intfList.intIfNum[i], d, ptr->aclIndex);
    }
  }

  /* remove ACL from all VLANs in each direction */
  for ( d = 0; d < ACL_INTF_DIR_MAX; d++ )
  {
    rc = aclMacImpAssignedVlanDirListGet( ptr->aclIndex, d, &vlanList );

    /* make sure ACL exists for a VLAN in order to remove it */
    if ( rc != L7_SUCCESS )
    {
      continue;
    }

    for ( i = 0; i < vlanList.count; i++ )
    {
      rc = aclMacImpVlanDirectionRemove( vlanList.vlanNum[i], d, ptr->aclIndex );
    }
  }

  /* remove all rules */
  while (aclMacImpRuleGetFirst(ptr->aclIndex, &rule) == L7_SUCCESS)
  {
    rc = aclMacRemoveRule(ptr, rule);
  }
  
  rc = osapiSemaTake(aclMacTree.semId, L7_WAIT_FOREVER);
  if (rc == L7_SUCCESS)
  {
    aclRuleNodeFree((void *)ptr->head);  /* free implicit 'deny all' final rule */

    ptr = avlDeleteEntry(&aclMacTree, acl_ptr);
    if (ptr == L7_NULLPTR) /* item not found in AVL tree */
    {
      rc = L7_FAILURE;
    }
    else /* deleted */
    {
      if (acl_curr_entries_g > 0)
      {
        acl_curr_entries_g--;
      }

      /* update the 'indexNext' value now that this ACL was deleted */
      L7_INTF_CLRMASKBIT(aclMacIndexInUseMask, ACL_MAC_INDEX_NORMALIZE(aclIndex));
      aclMacImpIndexNextUpdate();

      rc = L7_SUCCESS;
    }

    (void)osapiSemaGive(aclMacTree.semId);
  }
  
  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  
  return rc;
}


/*********************************************************************
*
* @purpose  To add the name to this MAC access list.
*
* @param    *acl_ptr    @b{(input)} access list element pointer
* @param    *name       @b{(input)} access list name
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list index, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Does not check for name validity (e.g. alphanumeric string).
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpNameAdd(void *acl_ptr, L7_uchar8 *name)
{
  aclMacStructure_t *ptr;
  L7_uint32         nameLen;

  if ((acl_ptr == L7_NULLPTR) || (name == L7_NULLPTR))
    return L7_FAILURE;

  nameLen = (L7_uint32)strlen((char *)name);

  if ((nameLen < L7_ACL_NAME_LEN_MIN) || (nameLen > L7_ACL_NAME_LEN_MAX))
    return L7_FAILURE;

  ptr = (aclMacStructure_t*)acl_ptr;

  strcpy((char *)ptr->aclName, (char *)name);

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To get the name of this MAC access list.
*
* @param    *acl_ptr    @b{(input)}  access list element pointer
* @param    *name       @b{(output)} access list name
*                                            
* @returns  void
*
* @comments Only outputs a value if the acl_ptr and name parms are both non-null.
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
void aclMacImpNameGet(void *acl_ptr, L7_uchar8 *name)
{
  aclMacStructure_t *ptr;

  if ((acl_ptr != L7_NULLPTR) && (name != L7_NULLPTR))
  {
    ptr = (aclMacStructure_t*)acl_ptr;
    strcpy((char *)name, (char *)ptr->aclName);
  }
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
L7_RC_t aclMacImpNameToIndexGet(L7_uchar8 *name, L7_uint32 *aclIndex)
{
  L7_RC_t           rc;
  aclMacStructure_t *ptr;
  L7_uint32         nameLen;
  L7_uint32         i;

  /* check inputs */
  if ((name == L7_NULLPTR) || (aclIndex == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  nameLen = (L7_uint32)strlen((char *)name);

  if ((nameLen < L7_ACL_NAME_LEN_MIN) || (nameLen > L7_ACL_NAME_LEN_MAX))
    return L7_FAILURE;

  /* search through all MAC access lists looking for this name */
  i = 0;
  rc = aclMacImpIndexGetNext(i, &i);

  while (rc == L7_SUCCESS)
  {
    ptr = (aclMacStructure_t *)aclMacFindACL(i);
    if (ptr == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    if (strcmp((char *)ptr->aclName, (char *)name) == 0)
    {
      /* found list name */
      *aclIndex = i;
      return L7_SUCCESS;
    }

    rc = aclMacImpIndexGetNext(i, &i);

  } /* endwhile */
  
  /* access list by this name does not exist */
  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To get the name of a MAC access list, given its index
*
* @param    *aclIndex   @b{(input)}  access list index
* @param    *name       @b{(output)} access list name
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpNameFromIndexGet(L7_uint32 aclIndex, L7_uchar8 *name)
{
  aclMacStructure_t *ptr;

  if ((aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS) ||
      (name == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  ptr = (aclMacStructure_t *)aclMacFindACL(aclIndex);
  if (ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  strcpy((char *)name, (char *)ptr->aclName);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To check if a rule number exists in a MAC access list.
*
* @param    *headPtr    @b{(input)}  pointer to first ACL rule
* @param    rulenum     @b{(input)}  rule number of interest
* @param    **rulePtr   @b{(output)} pointer to found ACL rule
*
* @returns  L7_TRUE     rule number exists
* @returns  L7_FALSE    rule number does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL aclMacRuleNumExist(aclMacRuleParms_t *headPtr, L7_uint32 rulenum, 
                           aclMacRuleParms_t **rulePtr)
{
  aclMacRuleParms_t *curr;

  curr = headPtr;
  while (curr->last == L7_FALSE)
  {
    if (rulenum == curr->ruleNum)
    {
      *rulePtr = curr;
      return L7_TRUE;
    }
    curr = curr->next;
  }

  return L7_FALSE;
}


/*********************************************************************
*
* @purpose  To add an access option to a rule in an access list.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    action      @b{(input)} access action (permit/deny)
*
* @returns  L7_SUCCESS  access option is added
* @returns  L7_FAILURE  invalid rule number
* @returns  L7_TABLE_IS_FULL, if storage unavailable for rule
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacActionAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 action)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr, *tmp;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  if (found == L7_TRUE)
  {
    curr->action = action;
  }
  else
  {
    /* need to create a new node */
    tmp = (aclMacRuleParms_t *)aclRuleNodeAllocate((L7_uint32)sizeof(aclMacRuleParms_t));
    if (tmp == L7_NULLPTR)
    {
      return L7_TABLE_IS_FULL;
    }
    memset(tmp, 0, sizeof(*tmp));
    
    tmp->configMask = 0;
    tmp->configMask |= (1 << ACL_MAC_ACTION);
    
    tmp->ruleNum = rulenum;
    tmp->action = action;
    tmp->every = L7_FALSE;
    tmp->last = L7_FALSE;
  
    /* if adding the first rule */
    if (ptr->head->last == L7_TRUE)
    {
      tmp->next = ptr->head;
      ptr->head = tmp;
    }
    
    /* OR if adding to the list of rules */
    else
    {
      curr = ptr->head;
      
      /* if 'rulenum' is less than the first rule number in the list */
      if (rulenum < curr->ruleNum)
      {
        tmp->next = curr;
        ptr->head = tmp;
      }
      else
      {
        while (curr->next->last == L7_FALSE)
        {
          if (rulenum > curr->next->ruleNum)
          {
            curr = curr->next;
          }
          else
          {
            break;
          }
        }
        tmp->next = curr->next;
        curr->next = tmp;
      }
    }

    ptr->ruleCount++;

  } /* else create new node */

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To remove a rule from an existing access list.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
*                                            
* @returns  L7_SUCCESS  rule is removed
* @returns  L7_FAILURE  invalid access list index or rule number
* @returns  L7_ERROR    access list index or rule number does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRemoveRule(void *acl_ptr, L7_uint32 rulenum)
{
  L7_RC_t           rc = L7_ERROR;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr, *prev;

  ptr = (aclMacStructure_t *)acl_ptr;

  prev = L7_NULLPTR;
  curr = ptr->head;

  while (curr->last == L7_FALSE)
  {
    if (rulenum == curr->ruleNum)
    {
      if (prev == L7_NULLPTR)
        ptr->head = curr->next;         /* remove first rule from list */
      else
        prev->next = curr->next;        /* remove rule from middle of list */

      aclRuleNodeFree((void *)curr);

      ptr->ruleCount--;

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;

      rc = L7_SUCCESS;
      break;
    }

    prev = curr;
    curr = curr->next;

  } /* endwhile */

  return rc;
}


/*********************************************************************
*
* @purpose  To add the assigned queue id for an access list rule.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    queueId     @b{(input)} assigned queue ID value
*
* @returns  L7_SUCCESS  assigned queue id added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacAssignQueueIdAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 queueId)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    curr->assignQueueId = queueId;
    curr->configMask |= (1 << ACL_MAC_ASSIGN_QUEUEID);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the redirect interface config id for an access list rule.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(input)} redirect interface config id
*
* @returns  L7_SUCCESS  redirect interface config id added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*                                            
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRedirectConfigIdAdd(void *acl_ptr, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    NIM_CONFIG_ID_COPY(&curr->redirectConfigId, configId);
    curr->configMask |= (1 << ACL_MAC_REDIRECT_INTF);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the mirror interface config id for an access list rule.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(input)} mirror interface config id
*
* @returns  L7_SUCCESS  mirror interface config id added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*                                            
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacMirrorConfigIdAdd(void *acl_ptr, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    NIM_CONFIG_ID_COPY(&curr->mirrorConfigId, configId);
    curr->configMask |= (1 << ACL_MAC_MIRROR_INTF);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To set the logging flag for a MAC access list rule.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    logFlag     @b{(input)} rule logging flag value
*
* @returns  L7_SUCCESS  logging flag is added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid flag value or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacLoggingAdd(void *acl_ptr, L7_uint32 rulenum, L7_BOOL logFlag)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  if ((logFlag != L7_FALSE) && (logFlag != L7_TRUE))
    return L7_FAILURE;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    curr->logging = logFlag;

    if (logFlag == L7_TRUE)
    {
      curr->configMask |= (1 << ACL_MAC_LOGGING);
    }
    else
    {
      curr->configMask &= ~(1 << ACL_MAC_LOGGING);
    }

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the class of service (cos) value.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    cos         @b{(input)} class of service (cos) value
*
* @returns  L7_SUCCESS  class of service value added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacCosAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 cos)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);

  if (found == L7_TRUE)
  {
    if (curr->every == L7_TRUE)
      return L7_FAILURE;

    curr->cos = cos;
    curr->configMask |= (1 << ACL_MAC_COS);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the secondary class of service (cos2) value.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    cos         @b{(input)} secondary class of service (cos2) value
*
* @returns  L7_SUCCESS  secondary class of service value added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacCos2Add(void *acl_ptr, L7_uint32 rulenum, L7_uint32 cos2)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    if (curr->every == L7_TRUE)
      return L7_FAILURE;

    curr->cos2 = cos2;
    curr->configMask |= (1 << ACL_MAC_COS2);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the destination MAC address and mask.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    *dstmac     @b{(input)} destination MAC address
* @param    *dstmask    @b{(input)} destination MAC mask
*
* @returns  L7_SUCCESS  destination MAC added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacDstMacAdd(void *acl_ptr, L7_uint32 rulenum,
                        L7_uchar8 *dstmac, L7_uchar8 *dstmask)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    if (curr->every == L7_TRUE)
      return L7_FAILURE;

    memcpy(curr->dstMac, dstmac, (size_t)L7_MAC_ADDR_LEN);
    curr->configMask |= (1 << ACL_MAC_DSTMAC);

    memcpy(curr->dstMacMask, dstmask, (size_t)L7_MAC_ADDR_LEN);
    curr->configMask |= (1 << ACL_MAC_DSTMAC_MASK);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the Ethertype keyword identifier.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    keyid       @b{(input)} Ethertype keyword identifier
* @param    value       @b{(input)} Ethertype custom value
*
* @returns  L7_SUCCESS  Ethertype keyword identifier added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments The value parameter is only meaningful when keyid is set to 
*           L7_QOS_ETYPE_KEYID_CUSTOM.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacEtypeKeyAdd(void *acl_ptr, L7_uint32 rulenum, 
                          L7_QOS_ETYPE_KEYID_t keyid, L7_uint32 value)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    if (curr->every == L7_TRUE)
      return L7_FAILURE;

    curr->etypeKeyId = (L7_ushort16)keyid;
    curr->configMask |= (1 << ACL_MAC_ETYPE_KEYID);

    /* if a custom keyword, set the etype value field
     *
     * NOTE:  Don't set this field if the 'unused' value is being
     *        passed in.  This means the value will be set in a 
     *        subsequent call.
     */
    if (keyid == L7_QOS_ETYPE_KEYID_CUSTOM)
    {
      if (value != L7_QOS_ETYPE_VALUE_UNUSED)
      {
        curr->etypeValue = (L7_ushort16)value;
        curr->configMask |= (1 << ACL_MAC_ETYPE_VALUE);
      }
    }
    else
    {
      curr->etypeValue = 0;
      curr->configMask &= ~(1 << ACL_MAC_ETYPE_VALUE);
    }

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the 'match every' match condition.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    every       @b{(input)} 'match every' flag value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS  'match every' condition added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacEveryAdd(void *acl_ptr, L7_uint32 rulenum, L7_BOOL every)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    curr->every = every;
    if (every == L7_TRUE)
    {
      /* clear all prior configured match-field flags when setting 'every'
       * to true
       */
      curr->configMask &= ~(ACL_MAC_RULE_CFG_MASK);

      curr->configMask |= (1 << ACL_MAC_EVERY);
    }
    else
    {
      curr->configMask &= ~(1 << ACL_MAC_EVERY);
    }

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the source MAC address and mask.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    *srcmac     @b{(input)} source MAC address
* @param    *srcmask    @b{(input)} source MAC mask
*
* @returns  L7_SUCCESS  source MAC added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacSrcMacAdd(void *acl_ptr, L7_uint32 rulenum,
                        L7_uchar8 *srcmac, L7_uchar8 *srcmask)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    if (curr->every == L7_TRUE)
      return L7_FAILURE;

    memcpy(curr->srcMac, srcmac, (size_t)L7_MAC_ADDR_LEN);
    curr->configMask |= (1 << ACL_MAC_SRCMAC);

    memcpy(curr->srcMacMask, srcmask, (size_t)L7_MAC_ADDR_LEN);
    curr->configMask |= (1 << ACL_MAC_SRCMAC_MASK);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add a single VLAN ID.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan        @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanIdAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 vlan)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    if (curr->every == L7_TRUE)
      return L7_FAILURE;

    /* clear out any existing range values */
    curr->vlanIdStart = 0;
    curr->vlanIdEnd = 0;
    curr->configMask &= ~(1 << ACL_MAC_VLANID_START);
    curr->configMask &= ~(1 << ACL_MAC_VLANID_END);

    curr->vlanId = (L7_ushort16)vlan;
    curr->configMask |= (1 << ACL_MAC_VLANID);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add a VLAN ID range.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    startvlan   @b{(input)} starting VLAN ID
* @param    endvlan     @b{(input)} ending VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID range added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanIdRangeAdd(void *acl_ptr, L7_uint32 rulenum, 
                             L7_uint32 startvlan, L7_uint32 endvlan)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    if (curr->every == L7_TRUE)
      return L7_FAILURE;

    /* clear out any existing single value */
    curr->vlanId = 0;
    curr->configMask &= ~(1 << ACL_MAC_VLANID);

    curr->vlanIdStart = (L7_ushort16)startvlan;
    curr->configMask |= (1 << ACL_MAC_VLANID_START);

    curr->vlanIdEnd = (L7_ushort16)endvlan;
    curr->configMask |= (1 << ACL_MAC_VLANID_END);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add a single Secondary VLAN ID.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan2       @b{(input)} Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanId2Add(void *acl_ptr, L7_uint32 rulenum, L7_uint32 vlan2)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    if (curr->every == L7_TRUE)
      return L7_FAILURE;

    /* clear out any existing range values */
    curr->vlanId2Start = 0;
    curr->vlanId2End = 0;
    curr->configMask &= ~(1 << ACL_MAC_VLANID2_START);
    curr->configMask &= ~(1 << ACL_MAC_VLANID2_END);

    curr->vlanId2 = (L7_ushort16)vlan2;
    curr->configMask |= (1 << ACL_MAC_VLANID2);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add a Secondary VLAN ID range.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    startvlan2  @b{(input)} starting Secondary VLAN ID
* @param    endvlan2    @b{(input)} ending Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID range added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanId2RangeAdd(void *acl_ptr, L7_uint32 rulenum, 
                              L7_uint32 startvlan2, L7_uint32 endvlan2)
{
  L7_BOOL           found;
  aclMacStructure_t *ptr;
  aclMacRuleParms_t *curr;

  ptr = (aclMacStructure_t *)acl_ptr;
  found = aclMacRuleNumExist(ptr->head, rulenum, &curr);
  
  if (found == L7_TRUE)
  {
    if (curr->every == L7_TRUE)
      return L7_FAILURE;

    /* clear out any existing single value */
    curr->vlanId2 = 0;
    curr->configMask &= ~(1 << ACL_MAC_VLANID2);

    curr->vlanId2Start = (L7_ushort16)startvlan2;
    curr->configMask |= (1 << ACL_MAC_VLANID2_START);

    curr->vlanId2End = (L7_ushort16)endvlan2;
    curr->configMask |= (1 << ACL_MAC_VLANID2_END);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  Get a aclStructure_t where the head is rule rulenum
*
* @param    aclIndex    @b{(input)}  access list index to begin search
* @param    rulenum     @b{(input)}  current rule number
* @param    *acl_rp     @b{(output)} rule parameter info pointer
*
* @returns  L7_SUCCESS  rulenum was found
* @returns  L7_ERROR    aclIndex or rulenum does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleGet(L7_uint32 aclIndex, L7_uint32 rulenum, aclMacRuleParms_t **acl_rp)
{
  aclMacStructure_t *acl_ptr;
  aclMacRuleParms_t *rp;

  if ((rulenum < L7_ACL_MIN_RULE_NUM) || (rulenum > L7_ACL_MAX_RULE_NUM))
    return L7_ERROR;

  acl_ptr = (aclMacStructure_t *)aclMacFindACL(aclIndex);
  
  if (acl_ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  
  rp = acl_ptr->head;

  while (rp != L7_NULLPTR)
  {
    if (rp->ruleNum == rulenum)
    {
      /* found it */
      *acl_rp = rp;
      return L7_SUCCESS;
    }

    rp = rp->next;
  } 
  
  /* rule not found */
  return L7_ERROR; 
}

/*********************************************************************
*
* @purpose  Sends a recently updated MAC ACL to the driver
*
* @param    aclIndex    @b{(input)} access list index
* @param    op          @b{(input)} operation (ACL_MODIFY)
*
* @returns  L7_SUCCESS  all interfaces updated
* @returns  L7_ERROR    TLV operation failed
* @returns  L7_FAILURE  other failure 
*
* @comments Will build a TLV and send the MAC ACL to the driver
*
* @end
*
*********************************************************************/
L7_RC_t aclMacUpdate(L7_uint32 aclIndex, L7_uint32 op)
{
  return aclMacUpdateExceptIntf(aclIndex, op, 0);
}

/*********************************************************************
*
* @purpose  Sends a recently updated MAC ACL to the driver, except for
*           the specified interface
*
* @param    aclIndex    @b{(input)} access list index
* @param    op          @b{(input)} operation (ACL_MODIFY)
* @param    intIfNum    @b{(input)} internal interface number, or 0
*
* @returns  L7_SUCCESS  all interfaces updated
* @returns  L7_ERROR    TLV operation failed
* @returns  L7_FAILURE  other failure 
*
* @comments Will build a TLV and send the MAC ACL to the driver for each
*           interface to which it is attached EXCEPT for the specified
*           intIfNum.  If the intIfNum is 0, all of the ACL's interfaces
*           are updated.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacUpdateExceptIntf(L7_uint32 aclIndex, L7_uint32 op, L7_uint32 intIfNum)
{
  L7_RC_t                     rc = L7_SUCCESS;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList;
  L7_uint32                   d, i;

  /* NOTE:  ACL_MODIFY affects all interfaces and directions to which
   *        this ACL is assigned.
   */

  switch (op)
  {
    case ACL_MODIFY:
      /* if not assoc with any intfs, noop */
      /* build and issue a create tlv for intf,dir with new info */
      for (d=0; d<ACL_INTF_DIR_MAX; d++)
      {
        if (aclMacImpAssignedIntfDirListGet(aclIndex, d, &intfList) == L7_SUCCESS)
        {
          for (i=0; i<intfList.count; i++)
          {
            if ((intIfNum != 0) && (intIfNum == intfList.intIfNum[i]))
            {
              /* caller does not want to update this interface */
              continue;
            }

            /* NOTE:  The following will call aclBuildTLVDelete() for an 
             *        ACL TLV that is currently in the device.
             */
            if (aclBuildTLVCreate(intfList.intIfNum[i], 0, d) != L7_SUCCESS)
            {
              return L7_ERROR;
            }
          }
        }
      } /* endfor d */

      /* if not assoc with any vlans, noop */
      /* build and issue a create tlv for vlan,dir with new info */
      for ( d = 0; d < ACL_INTF_DIR_MAX; d++ )
      {
        if ( L7_SUCCESS == aclMacImpAssignedVlanDirListGet( aclIndex, d, &vlanList ) )
        {
          for ( i = 0; i < vlanList.count; i++ )
          {
            /* NOTE:  The following will call aclBuildVlanTLVDelete() for an
             *        ACL TLV that is currently in the device.
             */
            if ( aclBuildTLVCreate( 0, vlanList.vlanNum[i], d ) != L7_SUCCESS )
            {
              return( L7_ERROR );
            }
          }
        }
      } /* endfor d */
      break;

    default:
      rc = L7_FAILURE;
      break;
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Builds a set of MAC ACL Rule TLVs
*
* @param    aclIndex            @b{(input)}  access list index
* @param    *ruleCount          @b{(output)} number of rules written to TLV
* @param    *matchEveryFlags    @b{(output)} set if a 'match every' rule used
* @param    *pDirInfo           @b{(output)} Ptr to intf,dir information
* @param    vlan                @b{(input)}  VLAN ID the ACL is going to be applied to
* @param    tlvHandle           @b{(input)}  TLV block handle to use
*
* @returns  L7_SUCCESS  this ACL successfully processed
* @returns  L7_ERROR    problem with TLV creation
* @returns  L7_FAILURE  any other failure
*
* @comments Does not append an implicit 'deny all' rule to the TLV.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacTlvRuleDefBuild(L7_uint32 aclIndex, L7_uint32 *ruleCount,
                              aclTlvMatchEvery_t *matchEveryFlags,
                              aclIntfDirInfo_t *pDirInfo,
                              L7_uint32 vlan, L7_tlvHandle_t tlvHandle)
{
  L7_BOOL                 needDenyAll = L7_TRUE;
  L7_uchar8               nullMac[L7_MAC_ADDR_LEN] = {0,0,0,0,0,0};
  L7_BOOL                 loggingValid;
  L7_RC_t                 rc;
  L7_uint32               rulenum;
  aclMacRuleParms_t       *p;
  nimUSP_t                nimUsp;
  L7_uint32               val;
  L7_ushort16             val16;
  L7_BOOL                 skipRule;
  L7_uint32               intIfNum;
  L7_INTF_STATES_t        intfState;

  aclTlvRuleDef_t         rule;
  aclTlvMatchCos_t        cos;
  aclTlvMatchCos2_t       cos2;
  aclTlvMatchDstMac_t     dstMac;
  aclTlvMatchEtype_t      etype;
  aclTlvMatchSrcMac_t     srcMac;
  aclTlvMatchVlanId_t     vlanId;
  aclTlvMatchVlanId2_t    vlanId2;

  aclTlvAttrAssignQueue_t assignQueue;
  aclTlvAttrRedirect_t    redirect; 
  aclTlvAttrMirror_t      mirror; 

  if (ruleCount == L7_NULLPTR)
    return L7_FAILURE;
  if (pDirInfo == L7_NULLPTR)
    return L7_FAILURE;

  *ruleCount = 0;

  /* do not process any MAC ACL rules if any MAC 'match every' rules were
   * previously seen
   */
  if ((*matchEveryFlags & ACL_TLV_RULE_BYPASS_MAC) != 0)
    return L7_SUCCESS;

  rc = aclMacImpRuleGetFirst(aclIndex, &rulenum);

  /* return successfully if ACL does not contain any rules (ruleCount is 0) */
  if (rc != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  while ((rc == L7_SUCCESS) && (needDenyAll == L7_TRUE))
  {
    rc = aclMacRuleGet(aclIndex, rulenum, &p);

    /* set a flag to bypass processing this rule if a 'match every' IP 
     * rule was previously seen, and this rule contains an explicit match
     * condition on an IPv4 Ethertype field
     */
    skipRule = L7_FALSE;
    if ((*matchEveryFlags & ACL_TLV_MATCH_EVERY_IP) != 0)
    {
      if (aclMacImpEtypeIsMatching(p, (L7_uint32)L7_QOS_ETYPE_ID_IPV4) == L7_TRUE)
      {
        skipRule = L7_TRUE;
      }
    }

    /* only allow logging if supported for the rule action type */
    if (p->logging == L7_TRUE)
      loggingValid = aclImpLoggingIsAllowed(p->action);
    else
      loggingValid = L7_FALSE;

    memset(&rule, 0, sizeof(rule));
    rule.denyFlag = (L7_uchar8)p->action;
    if (loggingValid == L7_TRUE)
      val = aclCorrEncode(L7_ACL_TYPE_MAC, aclIndex, rulenum, p->action);
    else
      val = L7_QOS_ACL_TLV_RULE_CORR_NULL;
    rule.logCorrelator = (L7_uint32)osapiHtonl((L7_ulong32)val);
    
    if (((p->configMask & ACL_MAC_RULE_CFG_MASK_EVERY) != 0) &&
        (skipRule == L7_FALSE))
    {
      if (loggingValid == L7_TRUE)
      {
        /* remember each non-null correlator used in TLV (needed for deletion)*/
        if (pDirInfo->tlvCorrCount < L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT)
          pDirInfo->tlvCorrList[pDirInfo->tlvCorrCount++] = val;
      }

      rc = tlvOpen(tlvHandle, L7_QOS_ACL_TLV_RULE_DEF, L7_QOS_ACL_TLV_RULE_DEF_LEN, (L7_uchar8*)&rule);

      /* write the assign queue, redirect intf, and mirror intf sub-TLVs
       * regardless of whether this is a 'match every' rule or not 
       * (only for 'permit'rules)
       */
      if (p->action == L7_ACL_PERMIT)
      {
        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_ASSIGN_QUEUEID) == L7_TRUE)
        { 
          memset(&assignQueue, 0, sizeof(assignQueue));
          assignQueue.qid = (L7_uchar8)p->assignQueueId;
          rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_TYPE, L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_LEN, (L7_uchar8*)&assignQueue);
        }

        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_REDIRECT_INTF) == L7_TRUE)
        { 
          /* only tell HAPI if target interface is in attaching or attached state */
          if (nimIntIfFromConfigIDGet(&p->redirectConfigId, &intIfNum) != L7_SUCCESS)
          {
            rc = L7_FAILURE;
            break;
          }
          if ((aclIsIntfAttached(intIfNum, &intfState) == L7_TRUE) &&
              ((intfState == L7_INTF_ATTACHING) || (intfState == L7_INTF_ATTACHED)))
          {
            /* translate configId into unit, slot, port */
            if (nimUspFromConfigIDGet(&p->redirectConfigId, &nimUsp) != L7_SUCCESS)
            {
              rc = L7_FAILURE;
              break;
            }
            val = nimUsp.unit;
            redirect.intfUnit = (L7_uint32)osapiHtonl(val);
            val = nimUsp.slot;
            redirect.intfSlot = (L7_uint32)osapiHtonl(val);
            val = nimUsp.port - 1;                    /* driver needs 0-based port */
            redirect.intfPort = (L7_uint32)osapiHtonl(val);
            rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_ATTR_REDIRECT_TYPE, L7_QOS_ACL_TLV_ATTR_REDIRECT_LEN, (L7_uchar8*)&redirect);
          }
        }

        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_MIRROR_INTF) == L7_TRUE)
        { 
          /* only tell HAPI if target interface is in attaching or attached state */
          if (nimIntIfFromConfigIDGet(&p->mirrorConfigId, &intIfNum) != L7_SUCCESS)
          {
            rc = L7_FAILURE;
            break;
          }
          if ((aclIsIntfAttached(intIfNum, &intfState) == L7_TRUE) &&
              ((intfState == L7_INTF_ATTACHING) || (intfState == L7_INTF_ATTACHED)))
          {
            /* translate configId into unit, slot, port */
            if (nimUspFromConfigIDGet(&p->mirrorConfigId, &nimUsp) != L7_SUCCESS)
            {
              rc = L7_FAILURE;
              break;
            }
            val = nimUsp.unit;
            mirror.intfUnit = (L7_uint32)osapiHtonl(val);
            val = nimUsp.slot;
            mirror.intfSlot = (L7_uint32)osapiHtonl(val);
            val = nimUsp.port - 1;                    /* driver needs 0-based port */
            mirror.intfPort = (L7_uint32)osapiHtonl(val);
            rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_ATTR_MIRROR_TYPE, L7_QOS_ACL_TLV_ATTR_MIRROR_LEN, (L7_uchar8*)&mirror);
          }
        }
      }

      if (0 != vlan)
      {
        /*
         * Force a VLAN ID rule when ACL is being associated with VLAN ID (not an interface).
         */
        vlanId.vidStart = osapiHtons(vlan);
        vlanId.vidEnd   = osapiHtons(vlan);
        rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID_LEN, (L7_uchar8*)&vlanId );
      }

      if ((p->every == L7_TRUE) || 
          (aclMacIsTlvMaskedRuleValid(aclIndex, rulenum, vlan) != L7_TRUE))
      { 
        /* only need to add this TLV entry if not applying ACL to vlan.  The vlan match criteria needed for that case
         * was already written above
         */
        if (0 == vlan)
        {
          rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_EVERY_TYPE, L7_QOS_ACL_TLV_MATCH_EVERY_LEN, L7_NULLPTR);
        }

        *matchEveryFlags |= ACL_TLV_MATCH_EVERY_MAC;

        /* Once a 'match every' rule is written to the TLV, no subsequent
         * rules for this ACL matter, including the implicit deny all.
         * Setting needDenyAll flag to false causes loop to terminate and the
         * final 'deny all' TLV rule to be skipped.
         */
        needDenyAll = L7_FALSE;
      }
      else
      {
        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_COS) == L7_TRUE)
        { 
          memset(&cos, 0, sizeof(cos));
          cos.cosValue = p->cos;
          rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_COS_TYPE, L7_QOS_ACL_TLV_MATCH_COS_LEN, (L7_uchar8*)&cos);
        }

        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_COS2) == L7_TRUE)
        { 
          memset(&cos2, 0, sizeof(cos2));
          cos2.cosValue = p->cos2;
          rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_COS2_TYPE, L7_QOS_ACL_TLV_MATCH_COS2_LEN, (L7_uchar8*)&cos2);
        }

        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_DSTMAC) == L7_TRUE)
        { 
          /* only include field if nonzero mask is specified */
          if (memcmp(p->dstMacMask, nullMac, (size_t)L7_MAC_ADDR_LEN) != 0)
          {
            memcpy(dstMac.macAddr, p->dstMac, (size_t)L7_MAC_ADDR_LEN);
            memcpy(dstMac.macMask, p->dstMacMask, (size_t)L7_MAC_ADDR_LEN);
            rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_DSTMAC_TYPE, L7_QOS_ACL_TLV_MATCH_DSTMAC_LEN, (L7_uchar8*)&dstMac);
          }
        }

        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_ETYPE_KEYID) == L7_TRUE)
        { 
          L7_uint32     value1, value2;

          /* NOTE:  Certain Ethertype keywords require two Ethertype values to
           *        cover packets belonging to the protocol.  The second value
           *        is also obtained here and presented to the driver to indicate
           *        a second hardware classifier is needed.
           */
          if (aclMacEtypeKeyIdToValue(p->etypeKeyId, p->etypeValue, 
                                      &value1, &value2) == L7_SUCCESS)
          {
            memset(&etype, 0, sizeof(etype));

            val16 = (L7_ushort16)value1;
            etype.etypeValue1 = osapiHtons(val16);

            val16 = (L7_ushort16)value2;
            etype.etypeValue2 = osapiHtons(val16);

            etype.checkStdHdrFlag = L7_FALSE;

            rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE, L7_QOS_ACL_TLV_MATCH_ETYPE_LEN, (L7_uchar8*)&etype);
          }
        }

        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_SRCMAC) == L7_TRUE)
        { 
          /* only include field if nonzero mask is specified */
          if (memcmp(p->srcMacMask, nullMac, (size_t)L7_MAC_ADDR_LEN) != 0)
          {
            memcpy(srcMac.macAddr, p->srcMac, (size_t)L7_MAC_ADDR_LEN);
            memcpy(srcMac.macMask, p->srcMacMask, (size_t)L7_MAC_ADDR_LEN);
            rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_SRCMAC_TYPE, L7_QOS_ACL_TLV_MATCH_SRCMAC_LEN, (L7_uchar8*)&srcMac);
          }
        }

        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_VLANID) == L7_TRUE)
        { 
          /* 
           * only add VLANID match rule from ACL if TLV is being built to apply to an interface... if it is going to
           * be applied to a VLAN ID, these fields are set above 
           */
          if (0 == vlan)
          {
            vlanId.vidStart = osapiHtons(p->vlanId);
            vlanId.vidEnd   = osapiHtons(p->vlanId);
            rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID_LEN, (L7_uchar8*)&vlanId);
          }
        }

        if ((aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_VLANID_START) == L7_TRUE) &&
            (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_VLANID_END) == L7_TRUE))
        { 
          /* 
           * only add VLANID match rule from ACL if TLV is being built to apply to an interface... if it is going to
           * be applied to a VLAN ID, these fields are set above 
           */
          if (0 == vlan)
          {
            vlanId.vidStart = osapiHtons(p->vlanIdStart);
            vlanId.vidEnd   = osapiHtons(p->vlanIdEnd);
            rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID_LEN, (L7_uchar8*)&vlanId);
          }
        }

        if (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_VLANID2) == L7_TRUE)
        { 
          vlanId2.vidStart = osapiHtons(p->vlanId2);
          vlanId2.vidEnd   = osapiHtons(p->vlanId2);
          rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID2_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID2_LEN, (L7_uchar8*)&vlanId2);
        }

        if ((aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_VLANID2_START) == L7_TRUE) &&
            (aclMacImpIsFieldConfigured(aclIndex, rulenum, ACL_MAC_VLANID2_END) == L7_TRUE))
        { 
          vlanId2.vidStart = osapiHtons(p->vlanId2Start);
          vlanId2.vidEnd   = osapiHtons(p->vlanId2End);
          rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID2_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID2_LEN, (L7_uchar8*)&vlanId2);
        }

      } /* endelse not 'match every' */

      rc = tlvClose(tlvHandle,L7_QOS_ACL_TLV_RULE_DEF);

      (*ruleCount)++;

    } /* endif non-empty rule */

    if (aclMacImpRuleGetNext(aclIndex, rulenum, &rulenum) != L7_SUCCESS)
      break;                            /* no more rules in this ACL */

  } /* endwhile */

  return rc;
}
  
/*********************************************************************
*
* @purpose  Checks if the MAC ACL rule is effectively non-empty after
*           factoring in a mask value, if any
*
* @param    *aclIndex   @b{(input)} access list index   
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan        @b{(input)} VLAN ID to which the ACL is
*                                   to be applied, if any 
*                                   (vlan == 0 if TLV is being built 
*                                   for an interface)
*
* @returns  L7_TRUE     TLV rule non-empty
* @returns  L7_FALSE    TLV rule is empty (i.e., masked value is 0)
*
* @comments The reason for this check is to prevent maskable fields
*           whose mask value is 0 from appearing in the TLV issued to the
*           device.  It also examines the rule set when the ACL is
*           being applied to a VLAN ID to see if that results in a "match every"
*           result.
*           
*
* @end
*
*********************************************************************/
L7_BOOL aclMacIsTlvMaskedRuleValid(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 vlan)
{
  L7_uchar8         nullMask[L7_MAC_ADDR_LEN] = {0,0,0,0,0,0};
  void              *acl_ptr;
  aclMacRuleParms_t *p;
  L7_uint32         configMask;

  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr == L7_NULLPTR)
  {
    return L7_FALSE;
  }
  
  if (aclMacRuleGet(aclIndex, rulenum, &p) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  configMask = p->configMask;

  /* check each rule field whose mask can be influenced by user configuration
   * and if the mask is 0, clear the corresponding field bits in the local copy
   * of the configMask
   */

  if (aclMacIsRuleFieldConfigured(acl_ptr, rulenum, ACL_MAC_DSTMAC) == L7_TRUE)
  {
    if (memcmp(p->dstMacMask, nullMask, L7_MAC_ADDR_LEN) == 0)
    {
      configMask &= ~(1 << ACL_MAC_DSTMAC);
      configMask &= ~(1 << ACL_MAC_DSTMAC_MASK);
    }
  }

  if (aclMacIsRuleFieldConfigured(acl_ptr, rulenum, ACL_MAC_SRCMAC) == L7_TRUE)
  {
    if (memcmp(p->srcMacMask, nullMask, L7_MAC_ADDR_LEN) == 0)
    {
      configMask &= ~(1 << ACL_MAC_SRCMAC);
      configMask &= ~(1 << ACL_MAC_SRCMAC_MASK);
    }
  }

  /* 
   * if we are applying to a VLAN, any VLAN tag match fields will be overridden by the 
   * the target VLAN ID.  If this results in matching all traffic on that VLAN, the rule
   * is equivalent to a 'match every' on that VLAN
   */
  if (vlan != 0)
  {
    configMask &= ~(1 << ACL_MAC_VLANID);
    configMask &= ~(1 << ACL_MAC_VLANID_START);
    configMask &= ~(1 << ACL_MAC_VLANID_END);
  }

  /* check for any remaining rule fields, including 'match every' */
  if ((configMask & ACL_MAC_RULE_CFG_MASK_EVERY) == 0)
    return L7_FALSE;

  return L7_TRUE;
}

/*********************************************************************
*
* @purpose  Translate the Ethertype keyid into a value  
*
* @param    keyId       @b{(input)}  Ethernet keyword identifier
* @param    custVal     @b{(input)}  Ethernet custom value
* @param    *value1     @b{(output)} first Ethertype value
* @param    *value2     @b{(output)} second Ethertype value (or 0 if unused)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Some Ethertype keywords translate into two Ethertype values. 
*           In this case, *value2 is output as a nonzero value.           
*
* @end
*
*********************************************************************/
L7_RC_t aclMacEtypeKeyIdToValue(L7_uint32 keyId, L7_uint32 custVal,
                                L7_uint32 *value1, L7_uint32 *value2)
{
  L7_uint32             val1, val2;

  if ((value1 == L7_NULLPTR) || (value2 == L7_NULLPTR))
    return L7_FAILURE;

  val2 = L7_QOS_ETYPE_VALUE_UNUSED;

  /* NOTE:  Only the 'custom' keyword uses the custom value configured
   *        by the user.  All other keywords use a well-known value.
   *        Some Ethertype keywords require two Ethertype values
   *        to represent the protocol.
   */

  switch (keyId)
  {
  case L7_QOS_ETYPE_KEYID_CUSTOM:
    /* a custom keyword with an 'unused' value means the value has not
     * been set yet
     */
    if (custVal == L7_QOS_ETYPE_VALUE_UNUSED)
      return L7_FAILURE;
    val1 = custVal;
    break;

  case L7_QOS_ETYPE_KEYID_APPLETALK:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_APPLETALK;
    break;

  case L7_QOS_ETYPE_KEYID_ARP:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_ARP;
    break;

  case L7_QOS_ETYPE_KEYID_IBMSNA:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_IBMSNA;
    break;

  case L7_QOS_ETYPE_KEYID_IPV4:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_IPV4;
    break;

  case L7_QOS_ETYPE_KEYID_IPV6:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_IPV6;
    break;

  case L7_QOS_ETYPE_KEYID_IPX:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_IPX;
    break;

  case L7_QOS_ETYPE_KEYID_MPLSMCAST:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_MPLSMCAST;
    break;

  case L7_QOS_ETYPE_KEYID_MPLSUCAST:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_MPLSUCAST;
    break;

  case L7_QOS_ETYPE_KEYID_NETBIOS:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_NETBIOS;
    break;

  case L7_QOS_ETYPE_KEYID_NOVELL:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_NOVELL_ID1;
    val2 = (L7_uint32)L7_QOS_ETYPE_ID_NOVELL_ID2;
    break;

  case L7_QOS_ETYPE_KEYID_PPPOE:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_PPPOE_ID1;
    val2 = (L7_uint32)L7_QOS_ETYPE_ID_PPPOE_ID2;
    break;

  case L7_QOS_ETYPE_KEYID_RARP:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_RARP;
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  /* set up the output pointers with the local values */
  *value1 = val1;
  *value2 = val2;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Translate the Ethertype value into a keyid
*
* @param    value       @b{(input)}  Ethernet value
* @param    *keyId      @b{(output)} Ethertype keyword identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An Ethertype value that is one of several values covered 
*           by a single keyid outputs that keyid.           
*
* @notes    Any value that does not match a specific keyid is output as   
*           the 'custom' keyid.           
*
* @end
*
*********************************************************************/
L7_RC_t aclMacEtypeValueToKeyId(L7_uint32 value, L7_uint32 *keyId)
{
  L7_uint32     id;

  if (keyId == L7_NULLPTR)
    return L7_FAILURE;

  switch (value)
  {
  case L7_QOS_ETYPE_ID_APPLETALK:
    id = L7_QOS_ETYPE_KEYID_APPLETALK;
    break;

  case L7_QOS_ETYPE_ID_ARP:
    id = L7_QOS_ETYPE_KEYID_ARP;
    break;

  case L7_QOS_ETYPE_ID_IBMSNA:
    id = L7_QOS_ETYPE_KEYID_IBMSNA;
    break; 

  case L7_QOS_ETYPE_ID_IPV4:
    id = L7_QOS_ETYPE_KEYID_IPV4;
    break; 

  case L7_QOS_ETYPE_ID_IPV6:
    id = L7_QOS_ETYPE_KEYID_IPV6;
    break; 

  case L7_QOS_ETYPE_ID_IPX:
    id = L7_QOS_ETYPE_KEYID_IPX;
    break; 

  case L7_QOS_ETYPE_ID_MPLSMCAST:
    id = L7_QOS_ETYPE_KEYID_MPLSMCAST;
    break; 

  case L7_QOS_ETYPE_ID_MPLSUCAST:
    id = L7_QOS_ETYPE_KEYID_MPLSUCAST;
    break; 

  case L7_QOS_ETYPE_ID_NETBIOS:
    id = L7_QOS_ETYPE_KEYID_NETBIOS;
    break; 

  case L7_QOS_ETYPE_ID_NOVELL_ID1:
  case L7_QOS_ETYPE_ID_NOVELL_ID2:
    id = L7_QOS_ETYPE_KEYID_NOVELL;
    break; 

  case L7_QOS_ETYPE_ID_PPPOE_ID1:
  case L7_QOS_ETYPE_ID_PPPOE_ID2:
    id = L7_QOS_ETYPE_KEYID_PPPOE;
    break; 

  case L7_QOS_ETYPE_ID_RARP:
    id = L7_QOS_ETYPE_KEYID_RARP;
    break; 

  default:
    id = L7_QOS_ETYPE_KEYID_CUSTOM;
    break; 

  } /* endswitch */

  /* set up the output pointer with the local value */
  *keyId = id;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Check if Ethertype field is set to match a specific value  
*
* @param    *p          @b{(input)}  ACL MAC rule parms ptr
* @param    etypeVal    @b{(input)}  Ethertype value of interest
*
* @returns  L7_TRUE     rule contains desired Ethertype match field
* @returns  L7_FALSE    reule does not contain desired Ethertype match field
*
* @notes    Considers a match on either Ethertype keyword or custom value.
*
* @end
*
*********************************************************************/
L7_BOOL aclMacImpEtypeIsMatching(aclMacRuleParms_t *p, L7_uint32 etypeVal)
{
  L7_BOOL       rc = L7_FALSE;
  L7_uint32     keyId;

  if (p == L7_NULLPTR)
    return L7_FALSE;

  if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_ETYPE_KEYID) == L7_TRUE)
  {
    /* check for matching Ethertype key id */
    if ((aclMacEtypeValueToKeyId(etypeVal, &keyId) == L7_SUCCESS) &&
        (p->etypeKeyId == (L7_ushort16)keyId))
    {
      rc = L7_TRUE;
    }
  }

  else if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MAC_ETYPE_VALUE) == L7_TRUE)
  {
    /* check for matching Ethertype custom value */
    if ((p->etypeKeyId == (L7_ushort16)L7_QOS_ETYPE_KEYID_CUSTOM) && 
        (p->etypeValue == (L7_ushort16)etypeVal))
    {
      rc = L7_TRUE;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Apply collateral config related to this interface 
*
* @param    intIfNum    @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is typically used to update the device when config
*           such as for redirect and mirror interfaces come and go.
*
* @comments The intIfNum parm represents the potential target interface
*           for redirect or mirroring, not the current interface(s) to 
*           which an ACL containing those attributes is applied.
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpCollateralConfigApply(L7_uint32 intIfNum)
{
  avlTree_t           *pTree = &aclMacTree;
  L7_uint32           aclIndex;
  aclMacStructure_t   *pAcl;
  aclMacRuleParms_t   *pRule;
  L7_RC_t             rc;
  L7_uint32           ruleIntIfNum;

  /* cycle through all existing ACLs looking for any that refer to 
   * the specified intIfNum as the target of a redirect or mirror
   * rule attribute
   */
  aclIndex = 0;                                 /* start with first entry */
  while ((pAcl = avlSearchLVL7(pTree, &aclIndex, AVL_NEXT)) != L7_NULLPTR)
  {
    aclIndex = pAcl->aclIndex;
    pRule = pAcl->head;

    while ((pRule != L7_NULLPTR) && (pRule->last != L7_TRUE))
    {
      /* the first occurrence of a matching redirect/mirror rule attribute
       * is sufficient for updating the interface(s) to which this ACL is 
       * attached
       *
       * check if the rule redirect/mirror interface matches the 
       * specified interface
       */
      if (ACL_RULE_FIELD_IS_SET(pRule->configMask, ACL_MAC_REDIRECT_INTF) == L7_TRUE)
      {
        rc = nimIntIfFromConfigIDGet(&pRule->redirectConfigId, &ruleIntIfNum);
      }
      else if (ACL_RULE_FIELD_IS_SET(pRule->configMask, ACL_MAC_MIRROR_INTF) == L7_TRUE)
      {
        rc = nimIntIfFromConfigIDGet(&pRule->mirrorConfigId, &ruleIntIfNum);
      }
      else
      {
        ruleIntIfNum = 0;
        rc = L7_ERROR;
      }

      if ((rc == L7_SUCCESS) && (ruleIntIfNum == intIfNum))
      {
        ACL_PRT(ACL_MSGLVL_LO_2,
                "ACL: Found collateral MAC ACL index %u mirr/redir to intf %u\n",
                aclIndex, intIfNum);

        /* update the MAC ACL in the device for all interfaces except the one
         * specified here (it is handled separately as part of the event 
         * processing for that interface)
         */
        if (aclMacUpdateExceptIntf(aclIndex, ACL_MODIFY, intIfNum) != L7_SUCCESS)
        {
          LOG_MSG("MAC ACL %s: Error updating device with latest configuration\n", 
                  pAcl->aclName);
        }

        break;                          /* done with this ACL */
      }

      pRule = pRule->next;

    } /* endwhile rule loop */

  } /* endwhile ACL loop */

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the first access list rule given a MAC ACL index
*
* @param    aclIndex    @b{(input)}  access list index
* @param    *rule       @b{(output)} first configured rule number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no rules exist for this ACL
* @returns  L7_FAILURE 
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpRuleGetFirst(L7_uint32 aclIndex, L7_uint32 *rule)
{
  void              *p;
  aclMacStructure_t *acl_ptr;

  p = aclMacFindACL(aclIndex);
  if (p == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  acl_ptr = (aclMacStructure_t *)p;
  
  if (acl_ptr->head->last == L7_TRUE)
  {
    return L7_ERROR;
  }
  
  *rule = acl_ptr->head->ruleNum;

  return L7_SUCCESS;
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
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpRuleGetNext(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *next)
{
  L7_RC_t           rc = L7_ERROR;
  void              *p;
  aclMacStructure_t *acl_ptr;
  aclMacRuleParms_t *acl_rp;

  if (rulenum >= L7_ACL_MAX_RULE_NUM)
    return L7_FAILURE;

  p = aclMacFindACL(aclIndex);
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  acl_ptr = (aclMacStructure_t *)p;

  acl_rp = acl_ptr->head; /* first (or only) rule */
  
  if (acl_rp->last == L7_TRUE)
  {
    return L7_ERROR; /* no rules */
  }

  /* last always exists and is rule #11 */
  while (acl_rp->last != L7_TRUE)
  {                      
    if (acl_rp->ruleNum > rulenum)
    {
      *next = acl_rp->ruleNum;
      rc = L7_SUCCESS;
      break;
    }

    acl_rp = acl_rp->next;

  } /* endwhile */
  
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
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The aclIndex parameter is required by this function in case there 
*           is ever a need to support separate rule number ranges based
*           on the type of ACL (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpRuleNumRangeCheck(L7_uint32 aclIndex, L7_uint32 rulenum)
{
  L7_RC_t       rc = L7_FAILURE;

  /* NOTE: Currently not using the aclIndex parameter. All ACL types are
   *       assumed to support the same number of rules.
   */

  /* sequentially check through the various rule number ranges that are supported */
  if ((rulenum >= L7_ACL_MIN_RULE_NUM) && (rulenum <= L7_ACL_MAX_RULE_NUM))
    rc = L7_SUCCESS;

  return rc;
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
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_BOOL aclMacImpIsFieldConfigured(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 field)
{
  void          *acl_ptr;

  /* check inputs */
  if ((aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS) ||
      (aclMacImpRuleNumRangeCheck(aclIndex, rulenum) != L7_SUCCESS))
  {
    return L7_FALSE;
  }

  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr == L7_NULLPTR)
  {
    return L7_FALSE;
  }
  
  return aclMacIsRuleFieldConfigured(acl_ptr, rulenum, field);
}

/*********************************************************************
*
* @purpose  Gets a list of interfaces to which a MAC ACL is assigned
*           in the specified direction
*
* @param    aclIndex    @b{(input)} access list index
* @param    aclDir      @b{(input)} interface direction (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    *intfList   @b{(output)} list of interfaces (L7_ACL_INTF_MAX_COUNT long)
*
* @returns  L7_SUCCESS  list of interfaces was built
* @returns  L7_FAILURE  invalid ACL index, or no interfaces exist
* @returns  L7_ERROR    access list is not assigned to any interface
*                       in this direction (no interface list was built)
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpAssignedIntfDirListGet(L7_uint32 aclIndex, L7_uint32 aclDir,
                                        L7_ACL_ASSIGNED_INTF_LIST_t *intfList)
{
  L7_RC_t           rc = L7_ERROR;
  L7_uint32         intIfNum, cfgIndex;
  aclIntfCfgData_t  *pCfg;
  nimConfigID_t     configIdNull;
  L7_uint32         count, i;
  aclIntfParms_t    *pList;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (ACL_IS_READY == L7_FALSE)
  {
    return L7_FAILURE;  /* no intf exist */
  }

  /* check validity of inputs */
  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (aclDir >= ACL_INTF_DIR_MAX)
    return L7_FAILURE;

  memset(intfList, 0, sizeof(*intfList));
  count = 0;

  for (cfgIndex = 1; cfgIndex < L7_ACL_INTF_MAX_COUNT; cfgIndex++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&aclCfgFileData->cfgParms.aclIntfCfgData[cfgIndex].configId, &configIdNull))
      continue;
    if (nimIntIfFromConfigIDGet(&aclCfgFileData->cfgParms.aclIntfCfgData[cfgIndex].configId, &intIfNum) != L7_SUCCESS)
      continue;
    if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
      continue;

    /* set up working ptr to assign list for this interface direction */
    pList = pCfg->intf_dir_Tb[aclDir].assignList;

    /* search entire assigned list for this interface */
    for (i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++, pList++)
    {
      if (pList->inUse != L7_TRUE)
        break;

      if (pList->id.aclType == L7_ACL_TYPE_MAC)
      {
        if (pList->id.aclId == aclIndex)
        {
          intfList->intIfNum[count++] = intIfNum;
          break;
        }
      }

    } /* endfor i */

  } /* endfor cfgIndex */

  intfList->count = count;

  if (count > 0)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  intf, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for intf, dir
* @returns  L7_REQUEST_DENIED Error applying ACL to hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpInterfaceDirectionAdd(L7_uint32 intIfNum, L7_uint32 aclDir, 
                                       L7_uint32 aclIndex, L7_uint32 seqNum)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclIntfCfgData_t  *pCfg;
  L7_ACL_TYPE_t     aclType;

  /* check inputs */
  if ( (nimCheckIfNumber(intIfNum) != L7_SUCCESS) ||
       (aclDir >= ACL_INTF_DIR_MAX) ||
       (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS) ||
       (seqNum > L7_ACL_MAX_INTF_SEQ_NUM) )
  {
    return L7_FAILURE;
  }

  /* check for supported interface type */
  if (aclIsValidIntf(intIfNum) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  /* Need to invoke this function before invoking aclImpIsInterfaceInUse().
   * aclMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE is a pre-condition
   * for aclImpIsInterfaceInUse()
   */
  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (aclMacImpAclTypeGet(aclIndex, &aclType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* make sure this ACL type is compatible with any other type already applied */
  if (aclImpIntfAclTypeCompatCheck(intIfNum, aclDir, aclType) != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  /* update intf,dir assign list with this ACL and notify driver of change */
  rc = aclImpIntfDirAdd(intIfNum, aclDir, aclType, aclIndex, seqNum);

  return rc;
}

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS  intIfNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or interface does not exist
* @returns  L7_ERROR    ACL does not exist for this index, or not assigned
*                         to the interface
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpInterfaceDirectionRemove(L7_uint32 intIfNum, L7_uint32 aclDir, 
                                          L7_uint32 aclIndex)
{
  L7_RC_t           rc;
  void              *acl_ptr;
  aclIntfCfgData_t  *pCfg;
  L7_ACL_TYPE_t     aclType;

  /* check inputs */
  if ((nimCheckIfNumber(intIfNum) != L7_SUCCESS) ||
      (aclDir >= ACL_INTF_DIR_MAX) ||
      (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  /* check for supported interface type */
  if (aclIsValidIntf(intIfNum) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  acl_ptr = aclMacFindACL(aclIndex);
  if (acl_ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  /* Need to invoke this function before invoking aclImpIsInterfaceInUse().
   * aclMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE is a pre-condition
   * for aclImpIsInterfaceInUse()
   */
  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (aclMacImpAclTypeGet(aclIndex, &aclType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* update app */
  rc = aclImpIntfDirRemove(intIfNum, aclDir, aclType, aclIndex);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets a list of VLANs to which a MAC ACL is assigned
*           in the specified direction
*
* @param    aclIndex    @b{(input)} access list index
* @param    aclDir      @b{(input)} VLAN direction (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    *intfList   @b{(output)} list of VLANs (L7_MAX_VLANS long)
*
* @returns  L7_SUCCESS  list of VLANs was built
* @returns  L7_FAILURE  invalid ACL index, or no VLANs exist
* @returns  L7_ERROR    access list is not assigned to any VLAN
*                       in this direction (no VLAN list was built)
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments An ACL can be shared/associated with multiple VLANs.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpAssignedVlanDirListGet( L7_uint32 aclIndex,
                                         L7_uint32 aclDir,
                                         L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList )
{
  L7_RC_t           rc = L7_ERROR;
  L7_uint32         cfgIndex;
  aclVlanCfgData_t  *pCfg;
  L7_uint32         count;
  L7_uint32         i;
  aclIntfParms_t    *pList;

  /* check validity of inputs */
  if ( aclMacImpIndexRangeCheck( aclIndex ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  if ( aclDir >= ACL_INTF_DIR_MAX )
  {
    return( L7_FAILURE );
  }

  memset( vlanList, 0, sizeof( *vlanList ) );
  count = 0;

  for ( cfgIndex = 1; cfgIndex <= L7_ACL_VLAN_MAX_COUNT; cfgIndex++ )
  {
    pCfg = &aclCfgFileData->cfgParms.aclVlanCfgData[cfgIndex];

    /* set up working ptr to assign list for this interface direction */
    pList = pCfg->vlan_dir_Tb[aclDir].assignList;

    /* search entire assigned list for this interface */
    for ( i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++, pList++ )
    {
      if ( pList->inUse != L7_TRUE )
      {
        break;
      }

      if ( pList->id.aclType == L7_ACL_TYPE_MAC )
      {
        if ( pList->id.aclId == aclIndex )
        {
          vlanList->vlanNum[count++] = pCfg->vlanId;
          break;
        }
      }
    } /* end for i */

  } /* end for cfgIndex */

  vlanList->count = count;

  if ( count > 0 )
  {
    rc = L7_SUCCESS;
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  To add an access list to the specified VLAN and direction.
*
* @param    intIfNum    @b{(input)} internal VLAN number
* @param    aclDir      @b{(input)} VLAN direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to VLAN, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  VLAN, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for VLAN, dir
* @returns  L7_REQUEST_DENIED Error applying ACL to hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpVlanDirectionAdd( L7_uint32 vlanNum,
                                   L7_uint32 aclDir,
                                   L7_uint32 aclIndex,
                                   L7_uint32 seqNum )
{
  aclMacStructure_t *acl_ptr;
  aclVlanCfgData_t  *pCfg;
  L7_ACL_TYPE_t     aclType;

  /* check inputs */
  if ( ( aclIsValidVlan( vlanNum ) != L7_TRUE ) ||
       ( aclDir >= ACL_INTF_DIR_MAX ) ||
       ( aclMacImpIndexRangeCheck( aclIndex ) != L7_SUCCESS ) ||
       ( seqNum > L7_ACL_MAX_VLAN_SEQ_NUM) )
  {
    return( L7_FAILURE );
  }

  acl_ptr = aclMacFindACL( aclIndex );
  if ( L7_NULLPTR == acl_ptr )
  {
    return( L7_ERROR );
  }

  if ( aclMapVlanConfigEntryGet( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  if ( aclMacImpAclTypeGet( aclIndex, &aclType ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  /* make sure this ACL type is compatible with any other type already applied */
  if (aclImpVlanAclTypeCompatCheck(vlanNum, aclDir, aclType) != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  /* update vlan,dir assign list with this ACL and notify driver of change */
  return( aclImpVlanDirAdd( vlanNum, aclDir, aclType, aclIndex, seqNum ) );
}

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified VLAN and direction.
*
* @param    intIfNum    @b{(input)} internal VLAN number
* @param    aclDir      @b{(input)} VLAN direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS  vlanNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or VLAN does not exist
* @returns  L7_ERROR    ACL does not exist for this index, or not assigned
*                         to the VLAN
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpVlanDirectionRemove( L7_uint32 vlanNum,
                                      L7_uint32 aclDir,
                                      L7_uint32 aclIndex )
{
  void              *acl_ptr;
  aclVlanCfgData_t  *pCfg;
  L7_ACL_TYPE_t     aclType;
  L7_RC_t           rc;

  /* check inputs */
  if ( ( aclDir >= ACL_INTF_DIR_MAX ) ||
       ( aclMacImpIndexRangeCheck( aclIndex ) != L7_SUCCESS ) )
  {
    return( L7_FAILURE );
  }

  /* check for supported interface type */
  if ( L7_FALSE == aclIsValidVlan( vlanNum ) )
  {
    return( L7_FAILURE );
  }

  acl_ptr = aclMacFindACL( aclIndex );
  if ( L7_NULLPTR == acl_ptr )
  {
    return( L7_ERROR );
  }

  /* Need to invoke this function before invoking aclImpIsVlanInUse().
   * aclMapVlanIsConfigurable( vlanNum, &pCfg ) == L7_TRUE is a pre-condition
   * for aclImpIsVlanInUse()
   */
  if ( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  if ( aclMacImpAclTypeGet( aclIndex, &aclType ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  /* update app */
  rc = aclImpVlanDirRemove(vlanNum, aclDir, aclType, aclIndex);

  if (L7_SUCCESS == rc)
  {
    /* if there are no ACLs configured on this VLAN ID after this removal, free up
     * configuration space
     */
    if (L7_TRUE != aclImpIsVlanInUse(vlanNum, ACL_INTF_DIR_IN) &&
        L7_TRUE != aclImpIsVlanInUse(vlanNum, ACL_INTF_DIR_OUT))
    {
      aclMapVlanConfigEntryRelease(vlanNum);
    }
  }
  return(rc);
}

/*************************************************************************
* @purpose  Get the current number of MAC ACLs configured
*
* @param    pCurr  @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_RC_t aclMacImpCurrNumGet(L7_uint32 *pCurr)
{
  *pCurr = avlTreeCount(&aclMacTree);
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Check if MAC ACL table is full
*
* @param    void
*
* @returns  L7_TRUE     MAC ACL table is full
* @returns  L7_FALSE    MAC ACL table not full
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_BOOL aclMacImpIsTableFull(void)
{
  L7_uint32     currCount;

  currCount = avlTreeCount(&aclMacTree);
  return (currCount >= acl_mac_max_entries) ? L7_TRUE : L7_FALSE;
}

/*********************************************************************
*
* @purpose  Outputs the MAC ACL type if it belongs to one of the supported 
*           index ranges
*
* @param    aclIndex    @b{(input)}  access list index to begin search
* @param    *acltype    @b{(output)} access list type
*
* @returns  L7_SUCCESS  access list index belongs to a supported MAC range
* @returns  L7_FAILURE  access list index is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpAclTypeGet(L7_uint32 aclIndex, L7_ACL_TYPE_t *acltype)
{
  L7_RC_t rc = L7_FAILURE;
  L7_ACL_TYPE_t localAclType;

  localAclType = aclImpTypeFromIndexGet(aclIndex);

  if (localAclType == L7_ACL_TYPE_MAC)
  {
    *acltype = localAclType;
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the current number of configured rules in a MAC ACL
*
* @param    aclIndex    @b{(input)}  access list index
* @param    *ruleCount  @b{(output)} number of rules in ACL
*
* @returns  L7_SUCCESS  ACL rule count retrieved
* @returns  L7_ERROR    access list does not exist
* @returns  L7_FAILURE  access list index is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The output ruleCount does not include the implicit 'deny all'
*           rule contained in every ACL definition.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpAclRuleCountGet(L7_uint32 aclIndex, L7_uint32 *ruleCount)
{
  aclMacStructure_t *ptr;

  if (aclMacImpIndexRangeCheck(aclIndex) != L7_SUCCESS)
    return L7_FAILURE;

  ptr = (aclMacStructure_t*)aclMacFindACL(aclIndex);
  if (ptr == L7_NULLPTR)
    return L7_ERROR;

  *ruleCount = ptr->ruleCount;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display the VLANs to which a MAC ACL is assigned.
*
* @param    index       @b{(input)} The index of the MAC ACL
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void aclMacShowVlans( L7_uint32 aclIndex )
{
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList;
  L7_uint32                   i, j;
  L7_uint32                   msgLvlReqd;
  L7_BOOL                     vlansFound = L7_FALSE;
  L7_RC_t                     rc;

  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_ON;

  ACL_PRT( msgLvlReqd, "VLANs assigned to MAC ACL index %d.\n\n", aclIndex );

  for ( i = 0; i < ACL_INTF_DIR_MAX; i++ )
  {
    rc = aclMacImpAssignedVlanDirListGet( aclIndex, i, &vlanList );
    if ( rc == L7_FAILURE )
    {
      ACL_PRT( msgLvlReqd, "aclMacImpAssignedVlanDirListGet failed.  rc = %d\n", rc );
      return;
    }

    if ( vlanList.count > 0 )
    {
      vlansFound = L7_TRUE;
      ACL_PRT( msgLvlReqd, "Direction - %s: %d", acl_direction_str[i], vlanList.vlanNum[0] );
      for ( j = 1; j < vlanList.count; j++ )
      {
        ACL_PRT( msgLvlReqd, ", %d", vlanList.vlanNum[j] );
      }
      ACL_PRT( msgLvlReqd, "\n" );
    }
  }

  if ( L7_FALSE == vlansFound )
  {
    ACL_PRT( msgLvlReqd, "No VLANs assigned to this ACL.\n" );
  }
}

/*********************************************************************
* @purpose  Display the current MAC ACL Table contents
*
* @param    showRules   @b{(input)} Indicates if ACL rules are displayed
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void aclMacShow(L7_BOOL showRules)
{
  L7_uint32           msgLvlReqd;
  avlTree_t           *pTree = &aclMacTree;
  L7_uint32           count;
  L7_uint32           aclIndex;
  aclMacStructure_t   *pAcl;

  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_ON;

  (void)osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    (void)osapiSemaGive(pTree->semId);
    ACL_PRT(msgLvlReqd, "MAC ACL table is empty\n\n");
    return;
  }

  ACL_PRT(msgLvlReqd, "\n\nMAC ACL Index Next:  %u\n\n", aclMacIndexNextFree);

  ACL_PRT(msgLvlReqd, "\nMAC ACL Table (contains %u entries):\n\n", count);

  ACL_PRT(msgLvlReqd, "Index  Rules               Name            \n");
  ACL_PRT(msgLvlReqd, "------ ----- ------------------------------\n");


  aclIndex = 0;                                 /* start with first entry */
  while ((pAcl = avlSearchLVL7(pTree, &aclIndex, AVL_NEXT)) != L7_NULLPTR)
  {
    ACL_PRT(msgLvlReqd, "%6u  %3u  %s \n", 
            pAcl->aclIndex, 
            pAcl->ruleCount, 
            pAcl->aclName); 

    /* update search keys for next pass */
    aclIndex = pAcl->aclIndex;
  }
  ACL_PRT(msgLvlReqd, "\n\n");

  /* optionally display the rules for each ACL */
  if (showRules == L7_TRUE)
  {
    aclIndex = 0;                               /* start with first entry */
    while ((pAcl = avlSearchLVL7(pTree, &aclIndex, AVL_NEXT)) != L7_NULLPTR)
    {
      aclMacRulesShow(pAcl->aclIndex);

      /* update search keys for next pass */
      aclIndex = pAcl->aclIndex;
    }
  }

  ACL_PRT(msgLvlReqd, "aclMacIndexNextFree = %u\n\n", aclMacIndexNextFree);

  (void)osapiSemaGive(pTree->semId);
}

/*********************************************************************
* @purpose  Display content of all rules for a specific MAC ACL
*
* @param    aclIndex    @b{(input)} MAC access list index
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclMacRulesShow(L7_uint32 aclIndex)
{
  L7_uint32           msgLvlReqd;
  avlTree_t           *pTree = &aclMacTree;
  aclMacStructure_t   *pAcl;
  aclMacRuleParms_t   *pRule;

  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_ON;

  pAcl = avlSearchLVL7(pTree, &aclIndex, AVL_EXACT);
  if (pAcl == L7_NULLPTR)
  {
    ACL_PRT(msgLvlReqd, "MAC ACL entry not found for index %u\n\n", aclIndex);
    return;
  }
  if (pAcl->aclIndex != aclIndex)
  {
    ACL_PRT(msgLvlReqd, "MAC ACL entry index %u does not match requested ACL index %u\n\n",
            pAcl->aclIndex, aclIndex);
    return;
  }

  pRule = pAcl->head;

  if (pRule->last == L7_TRUE)
    return;

  ACL_PRT(msgLvlReqd, "MAC ACL \"%s\" (Index %u)\n", (char *)pAcl->aclName, pAcl->aclIndex);

  while ((pRule != L7_NULLPTR) && (pRule->last != L7_TRUE))
  {
    aclMacRuleDisplay(pRule, msgLvlReqd);

    /* update search keys for next pass */
    pRule = pRule->next;
  }
  ACL_PRT(msgLvlReqd, "\n\n");
}

/*********************************************************************
* @purpose  Display contents of a MAC ACL rule
*
* @param    *pRule      @b{(input)} ACL rule pointer
* @param    msgLvlReqd  @b{(input)} Desired output message level
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclMacRuleDisplay(aclMacRuleParms_t *pRule, L7_uint32 msgLvlReqd)
{
  char                *pSpacer;

  pSpacer = " ";
  ACL_PRT(msgLvlReqd, "%s Rule %u\n", pSpacer, pRule->ruleNum);

  pSpacer = "   ";

  ACL_PRT(msgLvlReqd, "%s cfgMask=0x%8.8x ", pSpacer, pRule->configMask);
  if (pRule->configMask != 0)
  {
    ACL_PRT(msgLvlReqd, "(");
    aclRuleCfgMaskPrint(L7_ACL_TYPE_MAC, pRule->configMask, msgLvlReqd);
    ACL_PRT(msgLvlReqd, ")");
  }
  ACL_PRT(msgLvlReqd, "\n");

  ACL_PRT(msgLvlReqd, "%s act=%1s log=%1s every=%1s asgnQ=%1u ", 
          pSpacer, 
          (pRule->action == L7_ACL_DENY) ? "D" : "P",
          (pRule->logging == L7_FALSE) ? "F" : "T",
          (pRule->every == L7_FALSE) ? "F" : "T",
          pRule->assignQueueId);

  ACL_PRT(msgLvlReqd, "redir=%u/%u/%u mirror=%u/%u/%u\n", 
          pRule->redirectConfigId.configSpecifier.usp.unit, 
          pRule->redirectConfigId.configSpecifier.usp.slot,
          pRule->redirectConfigId.configSpecifier.usp.port,
          pRule->mirrorConfigId.configSpecifier.usp.unit, 
          pRule->mirrorConfigId.configSpecifier.usp.slot,
          pRule->mirrorConfigId.configSpecifier.usp.port);


  ACL_PRT(msgLvlReqd, "%s dmac=", pSpacer);
  aclMacAddrPrint(pRule->dstMac, msgLvlReqd);
  ACL_PRT(msgLvlReqd, " dmask=");
  aclMacAddrPrint(pRule->dstMacMask, msgLvlReqd);
  ACL_PRT(msgLvlReqd, " smac=");
  aclMacAddrPrint(pRule->srcMac, msgLvlReqd);
  ACL_PRT(msgLvlReqd, " smask=");
  aclMacAddrPrint(pRule->srcMacMask, msgLvlReqd);
  ACL_PRT(msgLvlReqd, "\n");

  ACL_PRT(msgLvlReqd, "%s etype: keyid=%hu(%s) value=0x%4.4x\n", 
          pSpacer, 
          pRule->etypeKeyId, aclMacEtypeKeyidStr[pRule->etypeKeyId],
          pRule->etypeValue);

  ACL_PRT(msgLvlReqd, "%s cos=%1u vid=%-4u vidstart=%-4u vidend=%-4u ", 
          pSpacer, pRule->cos, pRule->vlanId, 
          pRule->vlanIdStart, pRule->vlanIdEnd);
  ACL_PRT(msgLvlReqd, "cos2=%1u vid2=%-4u vid2start=%-4u vid2end=%-4u\n", 
          pRule->cos2, pRule->vlanId2, 
          pRule->vlanId2Start, pRule->vlanId2End);
}

/*********************************************************************
* @purpose  Display a MAC address or mask               
*
* @param    *p          @b{(input)} MAC address or mask pointer
* @param    msgLvlReqd  @b{(input)} Debug print message level
*
* @returns  void
*
* @comments Internal helper function.
*
* @end
*********************************************************************/
void aclMacAddrPrint(L7_uchar8 *p, L7_uint32 msgLvlReqd)
{
  L7_uint32     i;

  for (i = 0; i < (L7_MAC_ADDR_LEN-1); i++, p++)
  {
    ACL_PRT(msgLvlReqd, "%2.2x:", *p);
  }
  ACL_PRT(msgLvlReqd, "%2.2x", *p);
}

