/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename acl.c
*
* @purpose ACL main fns and utilities
*
* @component ACL
*
* @comments This file contains implementation functions that touch the
*           actual ACL data structures. Functions in this file should
*           not call back out to functions acl_api.c. Calling back to
*           acl_api.c functions risks creating deadlocks by attempting
*           to take the read/write lock more than once.
*
* @create 02/20/2002
*
* @author rjindal
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
#include "trap_qos_api.h"
#include "snmp_trap_api_qos.h"
#include "usmdb_qos_acl_api.h"
#include "acl.h"
#include "sysapi.h"
#include "avl_api.h"
#include "nvstoreapi.h"
#include "log.h"
#include "l7utils_api.h"

avlTree_t         aclTree;
avlTree_t         *pAclTree = &aclTree;
avlTreeTables_t   aclTreeHeap[L7_ACL_MAX_LISTS+1];
aclStructure_t    aclDataHeap[L7_ACL_MAX_LISTS+1];
L7_uint32         acl_max_entries = L7_ACL_MAX_LISTS;
L7_uint32         acl_curr_entries_g = 0;       /* total current list entries */
L7_uint32         aclMsgLvl_g = 0;
L7_uint32         aclRuleNodeCount = 0;

/* Number of bytes in mask */
#define ACL_IP_NAMED_INDEX_INDICES      (((L7_ACL_MAX_LISTS + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[ACL_IP_NAMED_INDEX_INDICES];
} ACL_IP_NAMED_INDEX_MASK_t;

/* NOTE: Index 0 is reserved and must not be used */
static ACL_IP_NAMED_INDEX_MASK_t  aclIpv6NamedIndexInUseMask;
static ACL_IP_NAMED_INDEX_MASK_t  aclIpv4NamedIndexInUseMask;

static L7_uint32  aclIpv6IndexNextFree = 0;         /* next available ACL index   */
static L7_uint32  aclNamedIpv4IndexNextFree = 0;    /* next available ACL index   */

avlTree_t         *pAclCorrTable_g = L7_NULLPTR;  /* might never be allocated */
avlTreeTables_t   *pAclCorrTableTreeHeap_g = L7_NULLPTR;
aclCorrTable_t    *pAclCorrTableDataHeap_g = L7_NULLPTR;

aclCallbackInfo_t *pAclCallbackInfo_g = L7_NULLPTR;

L7_int32          aclLogTaskSleepSecs_g = (L7_int32)(FD_QOS_ACL_LOGGING_INTERVAL * 60);

static char *aclIpRuleCfgMaskStr[] =
{
  "---", "Rnum", "Act", "Asgn", "Rdir", "Mirr", "Log", "Evry",
  "Prot", "Prmk", "Sip", "Sipm", "Sip6", "Spt", "Spts", "Spte",
  "Dip", "Dipm", "Dip6", "Dpt", "Dpts", "Dpte",
  "Tosm", "Prec", "Tos", "Dscp", "Flbl", "TRName"

};

static char *aclMacRuleCfgMaskStr[] =
{
  "---", "Rnum", "Act", "Asgn", "Rdir", "Mirr", "Log", "Evry",
  "Cos", "Cos2", "Dmac", "Dmsk", "Ekey", "Eval", "Smac", "Smsk",
  "Vid", "Vids", "Vide", "V2id", "V2ids", "V2ide", "TRName"

};

static void aclIpAddrPrint(L7_uint32 msgLvlReqd, L7_uint32 ip);
static void aclIpv6AddrPrint(L7_uint32 msgLvlReqd, L7_uchar8 *pAddr);

extern aclCfgFileData_t  *aclCfgFileData;
extern L7_tlvHandle_t    aclTlvHandle;
extern aclIntfInfo_t     *pAclIntfInfo;
extern osapiRWLock_t     aclRwLock;
extern aclCnfgrState_t   aclCnfgrState;

extern char              *acl_direction_str[];
extern char              *acl_type_str[];

extern aclVlanInfo_t     *pAclVlanInfo;
extern L7_uint32         *aclVlanMapTbl;
extern void            *aclEventQueue;
extern aclTRTableEntry_t *aclTimeRangeTable;
extern  aclTRCorrEntry_t *aclTRCorrEntryList;
/* Pointer to the start of free correlators list. */
static aclTRCorrEntry_t  *aclTRCorrEntryListFree = L7_NULLPTR;

/*********************************************************************
*
* @purpose  Reset correlator count variables
*
* @param    pDirInfo
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
static void aclCorrCountReset(aclIntfDirInfo_t *pDirInfo)
{
  if (pDirInfo != L7_NULLPTR)
  {
    pDirInfo->tlvCorrCount = 0;
    if(pDirInfo->tlvTRInfo != L7_NULLPTR)
    {
      pDirInfo->tlvTRInfo->tlvTimeRangeCorrCount = 0;
    }
  }
}

/*********************************************************************
*
* @purpose  To create an access list tree.
*
* @param    L7_uint32  maxListSize
*
* @returns  void
*
* @comments Creates a new instance of ACL, allocating space for up to
*           maxListSize ACLs.
*
* @end
*
*********************************************************************/
void aclCreateTree(L7_uint32 maxListSize)
{
  /* check validity of 'maxListSize' */
  if ((maxListSize == 0) || (maxListSize > L7_ACL_MAX_LISTS))
  {
    acl_max_entries = L7_ACL_MAX_LISTS;
  }
  else
  {
    acl_max_entries = maxListSize;
  }

  avlCreateAvlTree(pAclTree, aclTreeHeap, aclDataHeap, acl_max_entries,
                   (L7_uint32)sizeof(aclStructure_t), 0x10, (L7_uint32)sizeof(L7_uint32));
  (void)avlSetAvlTreeComparator(pAclTree, avlCompareULong32);

  /* initialize all named ACL index in-use tables */
  memset(&aclIpv4NamedIndexInUseMask, 0, sizeof(aclIpv4NamedIndexInUseMask));
  memset(&aclIpv6NamedIndexInUseMask, 0, sizeof(aclIpv6NamedIndexInUseMask));

  /* establish initial named ACL index next values */
  /* note that calling with any named ACL type will update all the values */
  aclImpNamedIndexNextUpdate(L7_ACL_TYPE_IPV6);
  aclImpNamedIndexNextUpdate(L7_ACL_TYPE_IP);
}


/*********************************************************************
*
* @purpose  To delete an access list tree.
*
* @param    void  *acl  pointer to DB
*
* @comments Destroys the instance of acl, releasing the previously
*           allocated database and control space.
*
* @end
*
*********************************************************************/
void aclDeleteTree(void)
{
  avlPurgeAvlTree(&aclTree, acl_max_entries);

  /* reset all named ACL index in-use table entries */
  memset(&aclIpv4NamedIndexInUseMask, 0, sizeof(aclIpv4NamedIndexInUseMask));
  memset(&aclIpv6NamedIndexInUseMask, 0, sizeof(aclIpv6NamedIndexInUseMask));

  /* reset all named ACL index next values */
  aclIpv6IndexNextFree = 0;
  aclNamedIpv4IndexNextFree = 0;
}


/*********************************************************************
*
* @purpose  To find an access list identified by an acl number.
*
* @param    void       *acl_ptr         pointer to database
* @param    L7_uint32  aclnum           the acl identifying number
* @param    L7_uint32  *found_at_index  the acl table index
*
* @returns  pointer to node if the acl is found
* @returns  L7_NULLPTR, if the acl is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
void* aclFindACL(L7_uint32 aclnum)
{
  void* acl_ptr;
  acl_ptr = avlSearchLVL7(&aclTree, &aclnum, L7_MATCH_EXACT);
  return acl_ptr;
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
L7_BOOL aclIsRuleFieldConfigured(void* acl_ptr, L7_uint32 rulenum, L7_uint32 field)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;
  L7_uint32 result;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    result = (curr->configMask) & (1 << field);
    if (result != 0)
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Provide the current value of the named ACL index next variable
*
* @param    aclType     @b{(input)}  access list type
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
void aclImpNamedIndexNextRead(L7_ACL_TYPE_t aclType, L7_uint32 *next)
{
  if (next != L7_NULLPTR)
  {
    if (aclType == L7_ACL_TYPE_IPV6)
      *next = aclIpv6IndexNextFree;
    else if (aclType == L7_ACL_TYPE_IP)
      *next = aclNamedIpv4IndexNextFree;
    else
      *next = 0;
  }
}

/*********************************************************************
*
* @purpose  Determine next available named acess list index value and
*           update the 'IndexNext' value
*
* @param    aclType     @b{(input)}  access list type
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
void aclImpNamedIndexNextUpdate(L7_ACL_TYPE_t aclType)
{
  L7_uint32     i;
  L7_BOOL       found;

  if ((aclType != L7_ACL_TYPE_IPV6) &&
      (aclType != L7_ACL_TYPE_IP))
    /* not a named IP ACL type */
    return;

  /* Use a value of 0 to indicate all indexes currently in use */
  if (acl_curr_entries_g >= acl_max_entries)
  {
    aclIpv6IndexNextFree = 0;
    aclNamedIpv4IndexNextFree = 0;
    return;
  }

    /* Look for first entry not in use.  Do not mark it as in use until
     * an ACL using this index value is successfully created.
     */
  for (i = 1, found = L7_FALSE; ((L7_FALSE == found) && (i <= acl_max_entries)); i++)
  {
      if (L7_INTF_ISMASKBITSET(aclIpv4NamedIndexInUseMask, i) == L7_FALSE)
        found = L7_TRUE;
  }
  if (L7_TRUE == found)
  {
    i--;
    aclNamedIpv4IndexNextFree = (i - 1) + L7_ACL_NAMED_IPV4_MIN_INDEX;
  }

  for (i = 1, found = L7_FALSE; ((L7_FALSE == found) && (i <= acl_max_entries)); i++)
  {
      if (L7_INTF_ISMASKBITSET(aclIpv6NamedIndexInUseMask, i) == L7_FALSE)
        found = L7_TRUE;
  }
  if (L7_TRUE == found)
  {
  i--;
    aclIpv6IndexNextFree = (i - 1) + L7_ACL_IPV6_MIN_INDEX;
  }
}

/*********************************************************************
*
* @purpose  Initialize in-use table entry for a named ACL
*
* @param    aclType     @b{(input)}  access list type
*
* @returns  void
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*
* @end
*
*********************************************************************/
void aclImpNamedIndexInUseInit(L7_ACL_TYPE_t aclType)
{
  if (L7_ACL_TYPE_IPV6 == aclType)
  {
    memset(&aclIpv6NamedIndexInUseMask, 0, sizeof(aclIpv6NamedIndexInUseMask));
  }
  else if (L7_ACL_TYPE_IP == aclType)
  {
    memset(&aclIpv4NamedIndexInUseMask, 0, sizeof(aclIpv4NamedIndexInUseMask));
  }
  else
  {
    return;
  }
  aclImpNamedIndexNextUpdate(aclType);
  return;
}

/*********************************************************************
*
* @purpose  Updates an in-use table entry for a named ACL
*
* @param    aclType     @b{(input)}  access list type
* @param    aclnum      @b{(input)}  access-list identifying number
* @param    val         @b{(input)}  value to set (L7_FALSE or L7_TRUE)
*
* @returns  void
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*
* @end
*
*********************************************************************/
void aclImpNamedIndexInUseUpdate(L7_ACL_TYPE_t aclType, L7_uint32 aclnum, L7_BOOL val)
{
  L7_uint32     index;

  /* only update in-use table for a supported named IP ACL types */
  if (aclType == L7_ACL_TYPE_IPV6)
  {
    index = ACL_IPV6_INDEX_NORMALIZE(aclnum);
    if (val == L7_TRUE)
    {
      L7_INTF_SETMASKBIT(aclIpv6NamedIndexInUseMask, index);
    }
    else
    {
      L7_INTF_CLRMASKBIT(aclIpv6NamedIndexInUseMask, index);
    }
  }
  else if ((aclType == L7_ACL_TYPE_IP) && (aclImpNamedIpv4IndexRangeCheck(aclnum) == L7_SUCCESS))
  {
    index = ACL_NAMED_IPV4_INDEX_NORMALIZE(aclnum);
    if (val == L7_TRUE)
    {
      L7_INTF_SETMASKBIT(aclIpv4NamedIndexInUseMask, index);
    }
    else
    {
      L7_INTF_CLRMASKBIT(aclIpv4NamedIndexInUseMask, index);
    }
  }
  else
  {
    /* not a named IP ACL type */
    return;
  }
}

/*********************************************************************
*
* @purpose  Retrieve the min/max index values allowed for the named ACL tables
*
* @param    aclType     @b{(input)}  access list type
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
L7_RC_t aclImpNamedIndexMinMaxGet(L7_ACL_TYPE_t aclType, L7_uint32 *pMin, L7_uint32 *pMax)
{
  L7_RC_t       rc = L7_FAILURE;

  if ((pMin == L7_NULLPTR) || (pMax == L7_NULLPTR))
    return rc;

  if (aclType == L7_ACL_TYPE_IPV6)
  {
    *pMin = L7_ACL_IPV6_MIN_INDEX;
    *pMax = (L7_ACL_IPV6_MIN_INDEX + acl_max_entries - 1);
    rc = L7_SUCCESS;
  }
  else if (aclType == L7_ACL_TYPE_IP)
  {
    *pMin = L7_ACL_NAMED_IPV4_MIN_INDEX;
    *pMax = (L7_ACL_NAMED_IPV4_MIN_INDEX + acl_max_entries - 1);
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the next named access list index created in the system.
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  current ACL index value
* @param    *pAclIndex  @b{(output)} pointer to next ACL index value
*
* @returns  L7_SUCCESS        Next named access list index was found
* @returns  L7_ERROR          No subsequent ACL index was found
* @returns  L7_FAILURE        All other failures
*
* @comments This internal function expects the caller to check
*           for an appropriate aclType value if there are any
*           restrictions in place.
*
* @comments This function is also used for a 'getFirst' search by
*           specifying an aclIndex value of 0.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIndexGetNext(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex,
                                L7_uint32 *pAclIndex)
{
  L7_RC_t           rc = L7_ERROR;
  L7_uint32         acl;
  aclStructure_t    *p;

  /* establish initial search value to skip over any configured numbered IPv4 ACLs */
  if (aclIndex < L7_ACL_NAMED_IPV4_MIN_INDEX-1)
    acl = L7_ACL_NAMED_IPV4_MIN_INDEX-1;
  else
    acl = aclIndex;

  while ((p = (aclStructure_t*)avlSearchLVL7(&aclTree, &acl, L7_MATCH_GETNEXT))
         != L7_NULLPTR)
  {
    if (aclImpTypeFromIndexGet(p->aclNum) == aclType)
    {
      *pAclIndex = p->aclNum;
      rc = L7_SUCCESS;
      break;
    }

    /* found an entry, but not of the right ACL type; keep searching */
    acl = p->aclNum;
  }

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
* @returns  L7_FAILURE        Access list index not found
*
* @comments The only valid aclType values are L7_ACL_TYPE_IPV6 and
*           L7_ACL_TYPE_IP.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIndexCheckValid(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex)
{
  aclStructure_t    *p;
  L7_RC_t rc = L7_FAILURE;

  p = (aclStructure_t*)avlSearchLVL7(&aclTree, &aclIndex, L7_MATCH_EXACT);

  if (p != L7_NULLPTR)
  {
    switch (aclType)
    {
    case L7_ACL_TYPE_IPV6:
      if (aclImpNamedIpv6IndexRangeCheck(p->aclNum) == L7_SUCCESS)
        rc = L7_SUCCESS;
      break;
    case L7_ACL_TYPE_IP:
      if (aclImpNamedIpv4IndexRangeCheck(p->aclNum) == L7_SUCCESS)
        rc = L7_SUCCESS;
      break;
    default:
      break;
    }
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Checks if the aclIndex is within proper range
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  ACL index value
*
* @returns  L7_SUCCESS        Access list index is in range
* @returns  L7_FAILURE        Access list index is out of range
*
* @comments The only valid aclType value is L7_ACL_TYPE_IPV6.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIndexRangeCheck(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex)
{
  L7_RC_t       rc = L7_FAILURE;

  if (aclType == L7_ACL_TYPE_IPV6)
  {
    if (aclImpNamedIpv6IndexRangeCheck(aclIndex) == L7_SUCCESS)
      rc = L7_SUCCESS;
  }
  else if (aclType == L7_ACL_TYPE_IP)
  {
    if (aclImpNamedIpv4IndexRangeCheck(aclIndex) == L7_SUCCESS)
      rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to the named IPV6 ACL index range
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIpv6IndexRangeCheck(L7_uint32 aclnum)
{
  L7_RC_t rc = L7_FAILURE;

  if ((aclnum >= L7_ACL_IPV6_MIN_INDEX) && (aclnum <= L7_ACL_IPV6_MAX_INDEX))
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to the named IPV4 ACL index range
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIpv4IndexRangeCheck(L7_uint32 aclnum)
{
  L7_RC_t rc = L7_FAILURE;

  if ((aclnum >= L7_ACL_NAMED_IPV4_MIN_INDEX) && (aclnum <= L7_ACL_NAMED_IPV4_MAX_INDEX))
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  Check if specified named ACL type is supported
*
* @param    aclType     @b{(input)}  access list type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments No semaphore protection required.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedAclTypeSupported(L7_ACL_TYPE_t aclType)
{
  L7_RC_t       rc = L7_FAILURE;

  if ((aclType == L7_ACL_TYPE_IPV6) ||
      (aclType == L7_ACL_TYPE_IP))
    rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
*
* @purpose  Given aclnum, get the next IP access list ID
*
* @param    L7_uint32   aclnum   the current access-list ID
* @param    L7_uint32  *next     the next ACL ID
*
* @returns  L7_SUCCESS        Next named access list index was found
* @returns  L7_ERROR          No subsequent ACL index was found
* @returns  L7_FAILURE        All other failures
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments This function is also used for a 'getFirst' search by
*           specifying an aclIndex value of 0.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNumGetNext(L7_uint32 aclnum, L7_uint32 *next)
{
  L7_RC_t           rc = L7_ERROR;
  L7_uint32         acl = aclnum;
  aclStructure_t    *p;

  while ((p = (aclStructure_t*)avlSearchLVL7(&aclTree, &acl, L7_MATCH_GETNEXT))
         != L7_NULLPTR)
  {
    if ((p->aclNum <= L7_ACL_MAX_EXT1_ID) && (aclImpTypeFromIndexGet(p->aclNum) == L7_ACL_TYPE_IP))
    {
      *next = p->aclNum;
      rc = L7_SUCCESS;
      break;
    }
    else if (p->aclNum > L7_ACL_MAX_EXT1_ID)
    {
      /* we have searched past the numbered range of indexes without finding one, stop looking */
      break;
    }
    /* found an entry, but not of the right ACL type; keep searching */
    acl = p->aclNum;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Checks if aclnum is a valid, configured IP access list
*
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNumCheckValid(L7_uint32 aclnum)
{
  aclStructure_t    *p;

  p = (aclStructure_t*)avlSearchLVL7(&aclTree, &aclnum, L7_MATCH_EXACT);

  if (p == L7_NULLPTR)
    return L7_FAILURE;

  if (aclImpNumRangeCheck(p->aclNum) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the supported ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments This function does not distinguish between the various
*           IP ACL identifiers, but considers whether the ACL ID belongs
*           to any numbered or named IP ACL index range that is supported.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNumRangeCheck(L7_uint32 aclnum)
{
  /* check all supported ACL type ranges */
  if ((aclImpStdAclNumRangeCheck(aclnum) != L7_SUCCESS) &&
      (aclImpExtGroupRangeCheck(aclnum) != L7_SUCCESS) &&
      (aclImpNamedIpv6IndexRangeCheck(aclnum) != L7_SUCCESS) &&
      (aclImpNamedIpv4IndexRangeCheck(aclnum) != L7_SUCCESS))
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Allocates storage for a rule definition node in the AVL tree.
*           Maintains a counter of total number of rules being used in all
*           ACLs (of any type).
*
* @param    None.
*
* @returns  Pointer to new storage node, L7_NULLPTR if unsuccessful
*
* @end
*
*********************************************************************/
void *aclRuleNodeAllocate(L7_uint32 size)
{
  void *ruleNodePtr;

  /* check if we have already allocated the limit for total rules */
  if (L7_ACL_MAX_RULES <= aclRuleNodeCount)
  {
    return(L7_NULLPTR);
  }
  ruleNodePtr = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, size);
  if (ruleNodePtr != L7_NULLPTR)
  {
    aclRuleNodeCount++;
  }
  return(ruleNodePtr);
}

/*********************************************************************
*
* @purpose  Frees storage for a rule definition node in the AVL tree.
*           Maintains a counter of total number of rules being used in all
*           ACLs (of any type).
*
* @param    aclRuleParms_t *node  pointer to ACL rule node to be freed
*
* @returns  Nothing.
*
* @end
*
*********************************************************************/
void aclRuleNodeFree(void *node)
{
  osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, node);
  if (aclRuleNodeCount > 0)
  aclRuleNodeCount--;
  else
    ACL_PRT(ACL_MSGLVL_HI,
            "\n%s: Attempted to decrement total rule node counter (aclRuleNodeCount) below 0\n",
            __FUNCTION__);
}
/*********************************************************************
*
* @purpose  To create an access list identified by an acl number.
*
* @param    L7_uint32  aclnum             the acl identifying number
* @param    L7_uint32  *created_at_index  the acl table index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_TABLE_IS_FULL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclCreateACL(L7_uint32 aclnum)
{
  L7_RC_t rc;
  L7_ACL_TYPE_t  aclType;
  aclStructure_t acl;

  aclRuleParms_t *parms;
  aclStructure_t *ptr;

  if (aclImpAclTypeGet(aclnum, &aclType) != L7_SUCCESS)
    return L7_FAILURE;

  rc = ACL_SEMA_TAKE(aclTree.semId, L7_WAIT_FOREVER);
  if (rc == L7_SUCCESS)
  {
    /* check if overall number of access lists have already been created */
    if (aclImpIsTotalListsAtMax() == L7_TRUE)
    {
      (void)ACL_SEMA_GIVE(aclTree.semId);
      return L7_TABLE_IS_FULL;
    }

    parms = (aclRuleParms_t *)aclRuleNodeAllocate((L7_uint32)sizeof(aclRuleParms_t));
    if (parms == L7_NULLPTR)
    {
      (void)ACL_SEMA_GIVE(aclTree.semId);
      return L7_TABLE_IS_FULL;
    }

    memset(parms, 0, sizeof(*parms));
    memset(&acl, 0, sizeof(acl));

    /* NOTE: The aclnum uniquely identifies the type of IP or IPv6 ACL */
    acl.aclNum = aclnum;
    acl.ruleCount = 0;
    acl.head = parms;

    /* construct the implicit 'deny all' final rule */
    acl.head->ruleNum = L7_ACL_DEFAULT_RULE_NUM;
    acl.head->action = L7_ACL_DENY;
    acl.head->every = L7_TRUE;

    acl.head->last = L7_TRUE;
    acl.head->next = L7_NULLPTR;

    acl.nextacl = L7_NULLPTR;

    ptr = avlInsertEntry(&aclTree, &acl);
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
      /* update the 'indexNext' value now that a new ACL was created
       * (only meaningful for named ACL types)
       */
      aclImpNamedIndexInUseUpdate(aclType, aclnum, L7_TRUE);
      aclImpNamedIndexNextUpdate(aclType);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    }

    (void)ACL_SEMA_GIVE(aclTree.semId);
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To delete an access list identified by an acl number.
*
* @param    L7_uint32  aclnum             the acl identifying number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDeleteACL(void *acl_ptr)
{
  L7_ACL_TYPE_t aclType;
  L7_uint32     aclnum;
  L7_uint32 rule,d,i;
  L7_RC_t rc;
  aclStructure_t *ptr;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList;

  ptr = acl_ptr;
  aclnum = ptr->aclNum;

  if (aclImpAclTypeGet(aclnum, &aclType) != L7_SUCCESS)
    return L7_FAILURE;

  /* remove ACL from all interfaces in each direction */
  for (d=0; d<ACL_INTF_DIR_MAX; d++)
  {
    rc = aclImpAssignedIntfDirListGet(aclnum, d, &intfList);

    /* make sure ACL exists for an interface in order to remove it */
    if (rc != L7_SUCCESS)
      continue;

    for (i=0; i<intfList.count; i++)
    {
      rc = aclImpInterfaceDirectionRemove(intfList.intIfNum[i], d, aclnum);
    }
  }
  /* remove ACL from all VLANs in each direction */
  for ( d = 0; d < ACL_INTF_DIR_MAX; d++ )
  {
    rc = aclImpAssignedVlanDirListGet( ptr->aclNum, d, &vlanList );

    /* make sure ACL exists for a VLAN in order to remove it */
    if ( rc != L7_SUCCESS )
    {
      continue;
    }

    for ( i = 0; i < vlanList.count; i++ )
    {
      rc = aclImpVlanDirectionRemove( vlanList.vlanNum[i], d, ptr->aclNum );
    }
  }

  /* remove all rules */
  while (aclImpRuleGetFirst(aclnum, &rule) == L7_SUCCESS)
  {
    rc = aclRemoveRule(ptr,rule);
  }

  rc = ACL_SEMA_TAKE(aclTree.semId, L7_WAIT_FOREVER);
  if (rc == L7_SUCCESS)
  {
    aclRuleNodeFree((void *)ptr->head); /* free rule #11 */

    ptr = avlDeleteEntry(&aclTree, acl_ptr);
    if (ptr == L7_NULLPTR) /* item DNE */
    {
      rc = L7_FAILURE;
    }
    else /* deleted */
    {
      if (acl_curr_entries_g > 0)
      {
        acl_curr_entries_g--;
      }

      /* update the 'indexNext' value now that this ACL was deleted
       * (only meaningful for named ACL types)
       */
      aclImpNamedIndexInUseUpdate(aclType, aclnum, L7_FALSE);
      aclImpNamedIndexNextUpdate(aclType);

      rc = L7_SUCCESS;
    }

    (void)ACL_SEMA_GIVE(aclTree.semId);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To add the name to this named access list.
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
L7_RC_t aclImpNameAdd(void *acl_ptr, L7_uchar8 *name)
{
  aclStructure_t    *ptr;
  L7_uint32         nameLen;

  if ((acl_ptr == L7_NULLPTR) || (name == L7_NULLPTR))
    return L7_FAILURE;

  ptr = (aclStructure_t*)acl_ptr;

  if (aclImpIndexRangeCheck(ptr->aclNum) != L7_SUCCESS)
    return L7_FAILURE;

  nameLen = (L7_uint32)strlen((char *)name);

  if ((nameLen < L7_ACL_NAME_LEN_MIN) || (nameLen > L7_ACL_NAME_LEN_MAX))
    return L7_FAILURE;

  osapiStrncpySafe(ptr->aclName, name, sizeof(ptr->aclName));

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To get the name of this named access list.
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
void aclImpNameGet(void *acl_ptr, L7_uchar8 *name)
{
  aclStructure_t    *ptr;

  if ((acl_ptr != L7_NULLPTR) && (name != L7_NULLPTR))
  {
    ptr = (aclStructure_t*)acl_ptr;

    if (aclImpIndexRangeCheck(ptr->aclNum) == L7_SUCCESS)
    {
      osapiStrncpySafe(name, ptr->aclName, sizeof(ptr->aclName));
    }
    else
    {
      /* acl_ptr is not for a named access list -- output an empty string */
      name[0] = L7_EOS;
    }
  }
}


/*********************************************************************
*
* @purpose  Checks if the ACL name string is alpha-numeric
*
* @param    *name       @b{(input)} access-list name string
*
* @returns  L7_TRUE     ACL name is alpha-numeric
* @returns  L7_FALSE    ACL name string not alpha-numeric
*
* @comments This function only checks the name string syntax for a named ACL.
*           It does not check if any named ACL currently contains this name.
*
* @comments An alpha-numeric string is defined as consisting of only
*           alphabetic, numeric, dash, underscore, or space characters.
*
* @end
*
*********************************************************************/
L7_BOOL aclImpNameIsAlphanumeric(L7_uchar8 *name)
{
  L7_uint32     i, len;
  L7_uchar8     *p;

  if (name == L7_NULLPTR)
    return L7_FALSE;

  len = (L7_uint32)strlen((char *)name);

  /* cannot start with  a number */
  if (len == 0 || isalpha(*name) == 0)
  {
     return L7_FALSE;
  }

  /* the only allowed characters in a legal named ACL name are:
   * alphabetic, numeric, dash, underscore, or period
   */
  for (i=0, p=name; i<len; i++, p++)
  {
    if (!(isalnum(*p) || *p=='-' || *p=='_' || *p=='.'))
    {
      return L7_FALSE;
    }
  }

  return L7_TRUE;
}


/*********************************************************************
*
* @purpose  Find the index of a named access list, given its name.
*
* @param    aclType    @b{(input)}   named access list type
* @param    *name      @b{(input)}   access list name
* @param    *pIndex    @b{(output)}  ptr to access list index output location
*
* @returns  L7_SUCCESS  named access list index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNameByTypeIndexFind(L7_ACL_TYPE_t aclType, L7_uchar8 *name, L7_uint32 *pIndex)
{
  L7_RC_t           rc;
  aclStructure_t    *ptr;
  L7_uint32         i;

  /* check inputs */
  if ((aclType != L7_ACL_TYPE_IPV6) && (aclType != L7_ACL_TYPE_IP))
    return L7_FAILURE;
  if (name == L7_NULLPTR)
    return L7_FAILURE;
  if (pIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* search through all named IP access lists looking for this name */
  i = 0;
  rc = aclImpNamedIndexGetNext(aclType, i, &i);

  while (rc == L7_SUCCESS)
  {
    ptr = (aclStructure_t *)aclFindACL(i);
    if (ptr == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    if (aclImpIndexRangeCheck(ptr->aclNum) == L7_SUCCESS)
    {
      if (osapiStrncmp(ptr->aclName, name, sizeof(ptr->aclName)) == 0)
      {
        /* found list name */
        *pIndex = i;
        return L7_SUCCESS;
      }
    }

    rc = aclImpNamedIndexGetNext(aclType, i, &i);

  } /* endwhile */

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To get the index of a named access list, given its name.
*
* @param    *name      @b{(input)}  access list name
* @param    *aclnum    @b{(output)} access list identifying number
*
* @returns  L7_SUCCESS  named access list index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNameToIndexGet(L7_uchar8 *name, L7_uint32 *aclnum)
{
  L7_RC_t       rc;
  L7_uint32     nameLen;

  /* check inputs */
  if ((name == L7_NULLPTR) || (aclnum == L7_NULLPTR))
    return L7_FAILURE;

  nameLen = (L7_uint32)strlen((char *)name);

  if ((nameLen < L7_ACL_NAME_LEN_MIN) || (nameLen > L7_ACL_NAME_LEN_MAX))
    return L7_FAILURE;

  /* search through all named IP access lists looking for this name */
  if ((rc = aclImpNameByTypeIndexFind(L7_ACL_TYPE_IPV6, name, aclnum)) != L7_SUCCESS)
  {
    /* if not found amongst IPv6 ACLs, check named IPv4 ACLS; if invalid parms, return*/
    if (rc == L7_ERROR)
    {
      rc = aclImpNameByTypeIndexFind(L7_ACL_TYPE_IP, name, aclnum);
    }
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To get the name of a named access list, given its index
*
* @param    *aclnum   @b{(input)}  access list index
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
L7_RC_t aclImpNameFromIndexGet(L7_uint32 aclnum, L7_uchar8 *name)
{
  aclStructure_t    *ptr;

  if ((aclImpIndexRangeCheck(aclnum) != L7_SUCCESS) ||
      (name == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  ptr = (aclStructure_t *)aclFindACL(aclnum);
  if (ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  osapiStrncpySafe(name, ptr->aclName, sizeof(ptr->aclName));

  return L7_SUCCESS;
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
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
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
L7_RC_t aclImpCommonNameStringGet(L7_uint32 aclnum, L7_ACL_TYPE_t *aclType,
                                  L7_uchar8 *name)
{
  L7_RC_t           rc = L7_FAILURE;
  L7_ACL_TYPE_t     tempAclType;
  L7_uchar8         tempAclName[L7_ACL_NAME_LEN_MAX+1];

  if ((L7_NULLPTR == aclType) ||
      (L7_NULLPTR == name))
  {
    return L7_FAILURE;
  }

  tempAclType = aclImpTypeFromIndexGet(aclnum);

  switch (tempAclType)
  {
    case L7_ACL_TYPE_IP:
      /* can be either numbered or named IPV4 list */
      if ((aclImpStdAclNumRangeCheck(aclnum) == L7_SUCCESS) ||
          (aclImpExtAclNumRangeCheck(aclnum) == L7_SUCCESS))
      {
        /* convert ACL number to a string for event notifications */
        if (osapiSnprintf(tempAclName, sizeof(tempAclName), "%u", aclnum) > 0)
          rc = L7_SUCCESS;
      }
      else if (aclImpNamedIpv4IndexRangeCheck(aclnum) == L7_SUCCESS)
      {
        rc = aclImpNameFromIndexGet(aclnum, tempAclName);
      }
      break;

    case L7_ACL_TYPE_MAC:
      if (aclMacImpIndexRangeCheck(aclnum) == L7_SUCCESS)
      {
        rc = aclMacImpNameFromIndexGet(aclnum, tempAclName);
      }
      break;

    case L7_ACL_TYPE_IPV6:
      if (aclImpNamedIpv6IndexRangeCheck(aclnum) == L7_SUCCESS)
      {
        rc = aclImpNameFromIndexGet(aclnum, tempAclName);
      }
      break;

    case L7_ACL_TYPE_NONE:
    default:
      break;

  } /* endswitch */

  if (L7_SUCCESS == rc)
  {
    *aclType = tempAclType;
    osapiStrncpySafe(name, tempAclName, L7_ACL_NAME_LEN_MAX+1);
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To check if a rule number exists in an access list.
*
* @param    aclEntryParms_t  *headPtr  pointer to first rule in an acl
* @param    L7_uint32        rulenum   the acl rule number
*
* @returns  L7_TRUE, if the rule number exists
* @returns  L7_FALSE, if the rule number does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL aclRuleNumExist(aclRuleParms_t *headPtr, L7_uint32 rulenum)
{
  aclRuleParms_t *curr;

  curr = headPtr;
  while (curr->last == L7_FALSE)
  {
    if (rulenum == curr->ruleNum)
    {
      return L7_TRUE;
    }
    curr = curr->next;
  }

  return L7_FALSE;
}


/*********************************************************************
*
* @purpose  Gets the current number of configured rules in an ACL
*
* @param    aclType     @b{(input)}  access list type
* @param    aclId       @b{(input)}  access list identifier
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
L7_RC_t aclRuleCountGet(L7_ACL_TYPE_t aclType, L7_uint32 aclId, L7_uint32 *ruleCount)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     aclRuleCount;

  if ((aclType == L7_ACL_TYPE_IP) ||
      (aclType == L7_ACL_TYPE_IPV6))
  {
    rc = aclImpAclRuleCountGet(aclId, &aclRuleCount);
  }
  else if (aclType == L7_ACL_TYPE_MAC)
  {
    rc = aclMacImpAclRuleCountGet(aclId, &aclRuleCount);
  }

  if (rc != L7_SUCCESS)
  {
    aclRuleCount = 0;
  }

  *ruleCount = aclRuleCount;
  return rc;
}


/*********************************************************************
*
* @purpose  To add an access option to a rule in an access list.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  action   access action (permit/deny)
*
* @returns  L7_SUCCESS, if the access option is added
* @returns  L7_FAILURE, if invalid rule number
* @returns  L7_TABLE_IS_FULL, if storage unavailable for rule
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclActionAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 action)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr, *prev, *tmp;

  ptr = (aclStructure_t*)acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);
  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }
    curr->action = action;
  }
  else
  {
    /* need to create a new node */
    tmp = (aclRuleParms_t *)aclRuleNodeAllocate((L7_uint32)sizeof(aclRuleParms_t));
    if (tmp == L7_NULLPTR)
    {
      return L7_TABLE_IS_FULL;
    }
    memset(tmp, 0, sizeof(*tmp));

    tmp->configMask = 0;
    tmp->configMask |= (1 << ACL_ACTION);

    tmp->ruleNum = rulenum;
    tmp->action = action;
    tmp->every = L7_FALSE;
    tmp->last = L7_FALSE;

    /* insert the new rule node into the ordered rulenum list */
    prev = L7_NULLPTR;
    curr = ptr->head;

    while (curr->last == L7_FALSE)
    {
      /* NOTE: This assumes that the 'implicit' last rulenum is always
       *       greater than the highest user-configurable rule number.
       */
      if (rulenum < curr->ruleNum)
      {
        break;
      }

      prev = curr;
      curr = curr->next;

    } /* endwhile */

    tmp->next = curr;

    if (prev == L7_NULLPTR)
      ptr->head = tmp;                /* insert at front of list */
    else
      prev->next = tmp;               /* insert in middle of list */

    ptr->ruleCount++;

  } /* else */

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
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
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRemoveRule(void *acl_ptr, L7_uint32 rulenum)
{
  L7_RC_t rc = L7_ERROR;
  aclStructure_t *ptr;
  aclRuleParms_t *curr, *prev;
  aclTRCorrEntry_t *timeRangeEntry;
  L7_uint32 trId;

  ptr = acl_ptr;

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

      /* Free the time Range Entry if rule is time based rule*/
      if (ACL_RULE_FIELD_IS_SET(curr->configMask, ACL_TIME_RANGE_NAME) == L7_TRUE)
      {
        if (curr->timeRangeEntry != L7_NULLPTR)
        {          
          timeRangeEntry = curr->timeRangeEntry;
          trId = timeRangeEntry->trId;
        
          /* remove and free the correlator node from aclTimeRangeTable */
          timeRangeEntry->refCount =0;
          if (aclTimeRangeTableCorrEntryDelete(trId, timeRangeEntry) !=L7_SUCCESS)
          {
            return L7_FAILURE;
          }
                    
          /* free the node and add to free list */
          
          if (aclTRCorrEntryFree(timeRangeEntry) != L7_SUCCESS)
          {
            return L7_FAILURE;
          }

          /* reset the entry in aclTimeRangeTable */           
          if (aclTimeRangeTableEntryDelete(trId) != L7_SUCCESS)
          {
            return L7_FAILURE;
          }          
        }                 
      }

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
* @purpose  To set the assigned queue id for an access list rule.
*
* @param    L7_uint32  *acl_ptr  the access-list element pointer
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  queueId   the queue id value
*
* @returns  L7_SUCCESS, if assigned queue id is added
* @returns  L7_FAILURE, if invalid queue id or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclAssignQueueIdAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 queueId)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = (aclStructure_t*)acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    curr->assignQueueId = queueId;
    curr->configMask |= (1 << ACL_ASSIGN_QUEUEID);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}

/*********************************************************************
*
* @purpose  To set the time range name to an access list rule.
*
* @param    L7_uint32        *acl_ptr  the access-list element pointer
* @param    L7_uint32        rulenum   the acl rule number
* @param    aclTRCorrEntry_t *timeRangeEntry time range entry
*
* @returns  L7_SUCCESS, if assigned time range entry name is added
* @returns  L7_FAILURE, if invalid time range entry or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclTimeRangeEntryAdd(void *acl_ptr, L7_uint32 rulenum, aclTRCorrEntry_t *timeRangeEntry)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  if(timeRangeEntry == L7_NULLPTR)
  {
    return L7_FAILURE;
  } 
  
  ptr = (aclStructure_t*)acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    curr->timeRangeEntry = timeRangeEntry;
    curr->configMask |= (1 << ACL_TIME_RANGE_NAME);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}

/*********************************************************************
*
* @purpose  To set the redirect interface config id for an access list rule.
*
* @param    L7_uint32  *acl_ptr   the access-list element pointer
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *configId  the redirect interface config id
*
* @returns  L7_SUCCESS, if redirect interface is added
* @returns  L7_FAILURE, if invalid redirect interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRedirectConfigIdAdd(void *acl_ptr, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = (aclStructure_t*)acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    NIM_CONFIG_ID_COPY(&curr->redirectConfigId, configId);
    curr->configMask |= (1 << ACL_REDIRECT_INTF);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To set the mirror interface config id for an access list rule.
*
* @param    L7_uint32  *acl_ptr   the access-list element pointer
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *configId  the mirror interface config id
*
* @returns  L7_SUCCESS, if mirror interface is added
* @returns  L7_FAILURE, if invalid mirror interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclMirrorConfigIdAdd(void *acl_ptr, L7_uint32 rulenum, nimConfigID_t *configId)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = (aclStructure_t*)acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    NIM_CONFIG_ID_COPY(&curr->mirrorConfigId, configId);
    curr->configMask |= (1 << ACL_MIRROR_INTF);

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To set the logging flag for an access list rule.
*
* @param    L7_uint32  *acl_ptr  the access-list element pointer
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  logFlag   rule logging flag value
*
* @returns  L7_SUCCESS, if logging flag is added
* @returns  L7_FAILURE, if invalid flag value or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclLoggingAdd(void *acl_ptr, L7_uint32 rulenum, L7_BOOL logFlag)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  if ((logFlag != L7_FALSE) && (logFlag != L7_TRUE))
    return L7_FAILURE;

  ptr = (aclStructure_t*)acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    curr->logging = logFlag;

    if (logFlag == L7_TRUE)
    {
      curr->configMask |= (1 << ACL_LOGGING);
    }
    else
    {
      curr->configMask &= ~(1 << ACL_LOGGING);
    }

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add a protocol to a rule in an access list.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  protocol  the protocol number
*
* @returns  L7_SUCCESS, if the protocol is added
* @returns  L7_FAILURE, if the protocol is not added
* @returns  L7_ERROR, if the rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclProtocolAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 protocol)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = (aclStructure_t*)acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      curr->protocol = (L7_uchar8)protocol;
      if (protocol == L7_ACL_PROTOCOL_IP)
        curr->protmask = (L7_uchar8)L7_QOS_ACL_TLV_MATCH_PROTOCOL_IP_MASK;
      else
        curr->protmask = (L7_uchar8)L7_QOS_ACL_TLV_MATCH_PROTOCOL_MASK;
      curr->configMask |= (1 << ACL_PROTOCOL);
      curr->configMask |= (1 << ACL_PROTOCOL_MASK);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;

}


/*********************************************************************
*
* @purpose  To add the source ip address and net mask.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  ip       the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclSrcIPMaskAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 ipAddr, L7_uint32 mask)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      /* turn off V6 flag since this is a shared data field */
      curr->configMask &= ~(1 << ACL_SRCIPV6);

      /* clear entire field before setting V4 contents */
      memset(&curr->srcIp, 0, sizeof(curr->srcIp));

      curr->srcIp.v4.addr = ipAddr;
      curr->srcIp.v4.mask = mask;
      curr->configMask |= (1 << ACL_SRCIP);
      curr->configMask |= (1 << ACL_SRCIP_MASK);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
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
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclSrcIpv6AddrAdd(void *acl_ptr, L7_uint32 rulenum,
                          L7_in6_prefix_t *addr6)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      /* turn off V4 flags since this is a shared data field */
      curr->configMask &= ~(1 << ACL_SRCIP);
      curr->configMask &= ~(1 << ACL_SRCIP_MASK);

      /* clear entire field before setting V6 contents */
      memset(&curr->srcIp, 0, sizeof(curr->srcIp));

      memcpy(curr->srcIp.v6.in6Addr.in6.addr8, addr6->in6Addr.in6.addr8, L7_IP6_ADDR_LEN);
      curr->srcIp.v6.in6PrefixLen = addr6->in6PrefixLen;
      curr->configMask |= (1 << ACL_SRCIPV6);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the source port.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  port      the ip address
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclSrcPortAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 port)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      /* clear out any existing range values */
      curr->srcStartPort = 0;
      curr->srcEndPort = 0;
      curr->configMask &= ~(1 << ACL_SRCSTARTPORT);
      curr->configMask &= ~(1 << ACL_SRCENDPORT);

      curr->srcPort = (L7_ushort16)port;
      curr->configMask |= (1 << ACL_SRCPORT);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the range of the source layer 4 ports to a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the access-list entry id
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclSrcPortRangeAdd(void *acl_ptr, L7_uint32 rulenum,
                           L7_uint32 startport, L7_uint32 endport)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      /* clear out any existing single value */
      curr->srcPort = 0;
      curr->configMask &= ~(1 << ACL_SRCPORT);

      curr->srcStartPort = (L7_ushort16)startport;
      curr->srcEndPort = (L7_ushort16)endport;
      curr->configMask |= (1 << ACL_SRCSTARTPORT);
      curr->configMask |= (1 << ACL_SRCENDPORT);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the destination ip address and net mask.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  ip       the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if destination ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDstIPMaskAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 ipAddr, L7_uint32 mask)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      /* turn off V6 flag since this is a shared data field */
      curr->configMask &= ~(1 << ACL_DSTIPV6);

      /* clear entire field before setting V4 contents */
      memset(&curr->dstIp, 0, sizeof(curr->dstIp));

      curr->dstIp.v4.addr = ipAddr;
      curr->dstIp.v4.mask = mask;
      curr->configMask |= (1 << ACL_DSTIP);
      curr->configMask |= (1 << ACL_DSTIP_MASK);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
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
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDstIpv6AddrAdd(void *acl_ptr, L7_uint32 rulenum,
                          L7_in6_prefix_t *addr6)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      /* turn off V4 flags since this is a shared data field */
      curr->configMask &= ~(1 << ACL_DSTIP);
      curr->configMask &= ~(1 << ACL_DSTIP_MASK);

      /* clear entire field before setting V4 contents */
      memset(&curr->dstIp, 0, sizeof(curr->dstIp));

      memcpy(curr->dstIp.v6.in6Addr.in6.addr8, addr6->in6Addr.in6.addr8, L7_IP6_ADDR_LEN);
      curr->dstIp.v6.in6PrefixLen = addr6->in6PrefixLen;
      curr->configMask |= (1 << ACL_DSTIPV6);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the destination port.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  port      the ip address
*
* @returns  L7_SUCCESS, if destination ip address, net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDstPortAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 port)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      /* clear out any existing range values */
      curr->dstStartPort = 0;
      curr->dstEndPort = 0;
      curr->configMask &= ~(1 << ACL_DSTSTARTPORT);
      curr->configMask &= ~(1 << ACL_DSTENDPORT);

      curr->dstPort = (L7_ushort16)port;
      curr->configMask |= (1 << ACL_DSTPORT);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the range of the destination layer 4 ports to a
*           rule in an existing access list.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the access-list entry id
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDstPortRangeAdd(void *acl_ptr, L7_uint32 rulenum,
                           L7_uint32 startport, L7_uint32 endport)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      /* clear out any existing single value */
      curr->dstPort = 0;
      curr->configMask &= ~(1 << ACL_DSTPORT);

      curr->dstStartPort = (L7_ushort16)startport;
      curr->dstEndPort = (L7_ushort16)endport;
      curr->configMask |= (1 << ACL_DSTSTARTPORT);
      curr->configMask |= (1 << ACL_DSTENDPORT);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  Set the match condition whereby all packets match for an
*           ACL rule
*
* @param    void       acl_ptr  the pointer to the access-list
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_BOOL    match    the match condition (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list or rule number
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments This routine sets the range of all the filtering criteria
*           in a rule to the maximum, in an existing access list.
*
* @end
*
*********************************************************************/
L7_RC_t aclEveryAdd(void *acl_ptr, L7_uint32 rulenum, L7_BOOL match)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    curr->every = match;

    if (match == L7_TRUE)
    {
      /* clear all prior configured match-field flags when setting 'every'
       * to true
       */
      curr->configMask &= ~ACL_RULE_CFG_MASK;
      curr->configMask &= ~ACL_IPV6_RULE_CFG_MASK;

      curr->configMask |= (1 << ACL_EVERY);
    }
    else
    {
      curr->configMask &= ~(1 << ACL_EVERY);
    }

    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the differentiated services code point (dscp) value
*           to a rule in an existing access list.
*
* @param    L7_uint32  rulenum  the access-list entry id
* @param    L7_uint32  dscpval  the diffserv code point value
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
L7_RC_t aclIPDscpAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 dscpval)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      curr->tosbyte = (L7_uchar8) ((dscpval) << L7_QOS_ACL_TLV_MATCH_IPDSCP_SHIFT);
      curr->tosmask = (L7_uchar8)L7_QOS_ACL_TLV_MATCH_IPDSCP_MASK;

      curr->configMask |= (1 << ACL_IPDSCP);
      curr->configMask &= ~(1 << ACL_IPTOS);
      curr->configMask &= ~(1 << ACL_IPPREC);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the precedence value to a rule in an existing
*           access list.
*
* @param    L7_uint32  aclnum         access-list identifying number
* @param    L7_uint32  rulenum        the access-list entry id
* @param    L7_uint32  precedenceval  the precedence value
*
* @returns  L7_SUCCESS, if the precedence value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIPPrecedenceAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 precedenceval)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {
      curr->tosbyte = (L7_uchar8)(precedenceval << L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_SHIFT);
      curr->tosmask = (L7_uchar8)(L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_MASK);

      curr->configMask |= (1 << ACL_IPPREC);
      curr->configMask &= ~(1 << ACL_IPDSCP);
      curr->configMask &= ~(1 << ACL_IPTOS);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  To add the type of service bits and mask.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the access-list entry id
* @param    L7_uint32  tosbits  tos bits
* @param    L7_uint32  tosmask  tos mask
*
* @returns  L7_SUCCESS, if the service type parameters are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIPTosAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 tosbits, L7_uint32 tosmask)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {

      curr->tosbyte = (L7_uchar8)tosbits;
      curr->tosmask = (L7_uchar8)tosmask;

      curr->configMask |= (1 << ACL_IPTOS);
      curr->configMask &= ~(1 << ACL_IPPREC);
      curr->configMask &= ~(1 << ACL_IPDSCP);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;

      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}

/*********************************************************************
*
* @purpose  To add the IPv6 flow label.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the access-list entry id
* @param    L7_uint32  flowlbl  flow label value
*
* @returns  L7_SUCCESS, if the value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIpv6FlowLabelAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 flowlbl)
{
  L7_BOOL found;
  aclStructure_t *ptr;
  aclRuleParms_t *curr;

  ptr = acl_ptr;
  found = aclRuleNumExist(ptr->head, rulenum);

  if (found == L7_TRUE)
  {
    curr = ptr->head;
    while (rulenum != curr->ruleNum)
    {
      curr = curr->next;
    }

    if (curr->every == L7_FALSE)
    {

      curr->flowlbl = flowlbl;
      curr->configMask |= (1 << ACL_FLOWLBLV6);

      aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;

      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_ERROR;
}

/*************************************************************************
* @purpose  Translate external interface direction to internal value
*
* @param    direction   @b{(input)}  External direction value
*                                      (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *aclDir     @b{(output)} Internal direction value
*                                      (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Translates the ACL interface direction value used by the
*           public APIs into a value used internally by the ACL application.
*
* @end
*********************************************************************/
L7_RC_t aclDirExtToInt(L7_uint32 direction, L7_uint32 *aclDir)
{
  L7_RC_t   rc = L7_FAILURE;

  if (direction == L7_INBOUND_ACL)
  {
    *aclDir = ACL_INTF_DIR_IN;
    rc = L7_SUCCESS;
  }
  else if (direction == L7_OUTBOUND_ACL)
  {
    *aclDir = ACL_INTF_DIR_OUT;
    rc = L7_SUCCESS;
  }

  return rc;
}

/*************************************************************************
* @purpose  Translate internal interface direction to external value
*
* @param    aclDir      @b{(input)}  Internal direction value
*                                      (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    *direction  @b{(output)} External direction value
*                                      (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Translates the ACL interface direction value used internally
*           by the ACL application into the value used by the ACL public APIs.
*
* @end
*********************************************************************/
L7_RC_t aclDirIntToExt(L7_uint32 aclDir, L7_uint32 *direction)
{
  L7_RC_t   rc = L7_FAILURE;

  if (aclDir == ACL_INTF_DIR_IN)
  {
    *direction = L7_INBOUND_ACL;
    rc = L7_SUCCESS;
  }
  else if (aclDir == ACL_INTF_DIR_OUT)
  {
    *direction = L7_OUTBOUND_ACL;
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    aclType     @b{(input)}  access-list type
* @param    aclId       @b{(input)}  access-list identifying number
* @param    seqNum      @b{(input)}  ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  intf, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for intf, dir
*
* @comments This internal function is used for assigning all types of ACLs
*           to an interface.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @comments A seqnum of 0 causes the next highest sequence number to be used.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirListAdd(L7_uint32 intIfNum, L7_uint32 aclDir,
                          L7_ACL_TYPE_t aclType, L7_uint32 aclId,
                          L7_uint32 seqNum)
{
  aclIntfCfgData_t *pCfg;
  aclIntfParms_t   *pList;
  L7_BOOL          multiAclFeat;
  L7_BOOL          isSeqMatch;
  L7_uint32        listCount, insertIndex, highestSeqNum;
  L7_uint32        aclRuleCount, totalRuleCount;
  L7_uint32        i;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (aclDir >= ACL_INTF_DIR_MAX)
    return L7_FAILURE;

  multiAclFeat = cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       L7_ACL_INTF_MULTIPLE_ACL_FEATURE_ID);

  isSeqMatch = L7_FALSE;
  listCount = totalRuleCount = insertIndex = highestSeqNum = 0;

  pList = pCfg->intf_dir_Tb[aclDir].assignList;

  /* scan ACL assignment list for this interface and direction, looking for:
   *  1) the ACL insertion point (either matching seq num or next higher)
   *  2) the last entry currently in-use
   *
   * this list is maintained as densely-packed, sorted in ascending
   * sequence number order
   */
  for (i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++, pList++)
  {
    /* stop search when first unused entry is reached */
    if (pList->inUse != L7_TRUE)
      break;

    listCount++;
    highestSeqNum = pList->seqNum;

    /* NOTE:  An incoming seqNum of 0 will be handled below, but will cause
     *        the insertIndex to remain at 0 during this loop.
     */
    if (pList->seqNum < seqNum)
    {
      insertIndex = i + 1;
    }
    else if (pList->seqNum == seqNum)
    {
      insertIndex = i;
      isSeqMatch = L7_TRUE;
    }

    /* get the number of rules in this ACL and add it to the total for intf,dir */
    if (aclRuleCountGet(pList->id.aclType, pList->id.aclId, &aclRuleCount) != L7_SUCCESS)
      aclRuleCount = 0;

    /* add the ACL rule count to the total rule count, except if this is a
     * matching sequence number entry (since it will be replaced by the
     * incoming ACL)
     */
    if (pList->seqNum != seqNum)
      totalRuleCount += aclRuleCount;

  } /* endfor */

  /* restore list ptr to the beginning of the array so indexing can be used */
  pList = pCfg->intf_dir_Tb[aclDir].assignList;

  /* if there is already more than one ACL on this interface, or
   * there is exactly one ACL but with a different sequence number,
   * this add operation will result in multiple ACLs being assigned
   * to the interface
   *
   * in this case, need to check the feature support conditions for
   * multiple and mixed type ACLs
   */

  if ((listCount > 1) ||
      ((listCount == 1) && (isSeqMatch != L7_TRUE)))
  {
    /* fail if interface supports only one ACL per direction */
    if (multiAclFeat == L7_FALSE)
      return L7_ALREADY_CONFIGURED;
  }

  /* add the rule count for the incoming ACL to the total and check if it
   * exceeds the maximum allowed rule limit for this platform
   */
  if (aclRuleCountGet(aclType, aclId, &aclRuleCount) != L7_SUCCESS)
    aclRuleCount = 0;

  totalRuleCount += aclRuleCount;

  if (totalRuleCount > L7_ACL_MAX_RULES_PER_MULTILIST)
  {
    /* adding this ACL would exceed maximum number of rules per intf,dir */
    return L7_TABLE_IS_FULL;
  }

  /* at this point, the insertIndex indicates either:
   *  - the entry of the matching sequence number, or
   *  - the entry of the list insertion point, or
   *  - the first entry of an empty list, or
   *  - the first entry when the incoming seqNum is 0 (needs adjustment)
   */

  if (isSeqMatch == L7_TRUE)
  {
    /* nothing more to do here, except to replace the ACL entry (below)
     *
     * a matching sequence number entry can be updated whether or not
     * the table is full
     */
  }

  else if (listCount >= L7_ACL_MAX_LISTS_PER_INTF_DIR)
  {
    /* no more room in the table */
    return L7_TABLE_IS_FULL;
  }

  else if (seqNum == 0)
  {
    /* this entry goes at the end of the list using a sequence number
     * one greater than the highest sequence number currently in use,
     * provided not already using the maximum allowed sequence number
     *
     * NOTE:  The isSeqMatch can never be true if seqNum is 0.
     */
    if (highestSeqNum >= L7_ACL_MAX_INTF_SEQ_NUM)
      return L7_ERROR;

    seqNum = highestSeqNum + 1;

    /* set insertIndex to next available element (it is currently set to 0) */
    insertIndex = listCount;
  }

  else /* (isSeqMatch != L7_TRUE) */
  {
    /* if a matching sequence number entry was not found, shift all existing
     * entries following the insertIndex right one position to make room
     * for the new ACL entry, working backwards through the list of
     * existing entries
     *
     * an empty list or a list whose insertIndex is pointing beyond the
     * last existing element will not cause any entries to be shifted here
     */
    for (i = listCount; i > insertIndex; i--)
      pList[i] = pList[i-1];
  }

  /* the insertIndex denotes the location to set the new ACL entry in the list */
  pList[insertIndex].inUse = L7_TRUE;
  pList[insertIndex].seqNum = seqNum;

  pList[insertIndex].id.aclType = aclType;
  pList[insertIndex].id.aclId = aclId;

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To remove an access list from the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    aclType     @b{(input)}  access-list type
* @param    aclId       @b{(input)}  access-list identifying number
*
* @returns  L7_SUCCESS  ACL successfully removed from intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL not currently attached to intf, dir
*
* @comments This internal function is used for removing all types of ACLs
*           from an interface.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirListRemove(L7_uint32 intIfNum, L7_uint32 aclDir,
                             L7_ACL_TYPE_t aclType, L7_uint32 aclId)
{
  L7_BOOL          found = L7_FALSE;
  aclIntfCfgData_t *pCfg;
  aclIntfParms_t   *pList;
  L7_uint32        i;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (aclDir >= ACL_INTF_DIR_MAX)
    return L7_FAILURE;

  pList = pCfg->intf_dir_Tb[aclDir].assignList;

  /* search ACL assignment list for this interface and direction,
   * looking for the matching ACL type and id
   *
   * NOTE:  Using array notation for pList to allow easier shifting
   *        of entries using indexes
   */
  for (i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++)
  {
    if (pList[i].inUse != L7_TRUE)
      break;

    if ((pList[i].id.aclType == aclType) &&
        (pList[i].id.aclId == aclId))
    {
      found = L7_TRUE;
      break;
    }

  } /* endfor */

  if (found != L7_TRUE)
    return L7_ERROR;

  /* starting with the matching entry, shift each in use entry left one
   * position in the array
   *
   * always stop one element short of the end of the array to allow the
   * last entry to be cleared out of a previously full list
   */
  for (; i < (L7_ACL_MAX_LISTS_PER_INTF_DIR - 1); i++)
  {
    if (pList[i+1].inUse != L7_TRUE)
      break;

    pList[i] = pList[i+1];
  }

  /* clear out the last vacated entry */
  pList[i].inUse = L7_FALSE;
  pList[i].seqNum = 0;

  pList[i].id.aclType = L7_ACL_TYPE_NONE;
  pList[i].id.aclId = 0;

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To purge all access lists from the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
*
* @returns  L7_SUCCESS  all ACLs successfully purged from intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
*
* @comments This internal function is used for purging all types of ACLs
*           from an interface.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirListPurge(L7_uint32 intIfNum, L7_uint32 aclDir)
{
  aclIntfCfgData_t *pCfg;
  aclIntfParms_t   *pList;
  L7_uint32        i;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (aclDir >= ACL_INTF_DIR_MAX)
    return L7_FAILURE;

  pList = pCfg->intf_dir_Tb[aclDir].assignList;

  /* clear out entire assign list for this intf, dir */
  for (i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++, pList++)
  {
    /* NOTE:  Although the list is densely-packed, loop through entire
     *        list just in case some entry got orphaned.
     */
    pList->inUse = L7_FALSE;
    pList->seqNum = 0;

    pList->id.aclType = L7_ACL_TYPE_NONE;
    pList->id.aclId = 0;

  } /* endfor */

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Check if the specified interface and direction is valid
*           for use with ACL
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
*
* @returns  L7_SUCCESS  intf,dir is valid for ACL
* @returns  L7_ERROR    intf,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The interface type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the interface
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the interface
*           and direction (see aclImpIsInterfaceInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirGet(L7_uint32 intIfNum, L7_uint32 aclDir)
{
  L7_uint32     featureId;

  if (ACL_IS_READY == L7_FALSE)
    return L7_FAILURE;  /* no intf exist */

  /* check inputs */
  if ((nimCheckIfNumber(intIfNum) != L7_SUCCESS) ||
      (aclDir >= ACL_INTF_DIR_MAX))
  {
    return L7_FAILURE;
  }

  if (aclIsValidIntf(intIfNum) != L7_TRUE)
    return L7_ERROR;

  /* check the feature support to determine which ACL intf directions are allowed */
  if (aclDir == ACL_INTF_DIR_IN)
    featureId = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
  else
    featureId = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, featureId) != L7_TRUE)
    return L7_ERROR;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get next sequential interface and direction that is valid
*           for use with ACL
*
* @param    intIfNum        @b{(input)}  internal interface number
* @param    aclDir          @b{(input)}  interface direction (internal)
* @param    *nextIntIfNum   @b{(output)} next internal interface number
* @param    *nextAclDir     @b{(output)} next interface direction (internal)
*
* @returns  L7_SUCCESS  next intf,dir for ACL was obtained
* @returns  L7_ERROR    no more valid intf,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The interface type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an interface and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the interface
*           and direction (see aclImpIsInterfaceInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirGetNext(L7_uint32 intIfNum, L7_uint32 aclDir,
                             L7_uint32 *nextIntIfNum, L7_uint32 *nextAclDir)
{
  L7_RC_t       rc;
  L7_BOOL       aclInSupp, aclOutSupp, lagSupp;

  if (ACL_IS_READY == L7_FALSE)
    return L7_FAILURE;  /* no intf exist */

  /* check inputs */
  if ((nextIntIfNum == L7_NULLPTR) || (nextAclDir == L7_NULLPTR))
    return L7_FAILURE;

  /* determine which ACL directions are supported */
  aclInSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID);
  aclOutSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID);
  if ((aclInSupp == L7_FALSE) && (aclOutSupp == L7_FALSE))
    return L7_FAILURE;

  /* determine if LAG intf feature is supported for ACL */
  lagSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID);

  if (aclIsValidIntf(intIfNum) == L7_TRUE)
  {
    /* first try getting next direction for this valid ACL interface
     * (depends on feature support)
     */
    if ((aclDir < ACL_INTF_DIR_IN) && (aclInSupp == L7_TRUE))
    {
      *nextIntIfNum = intIfNum;
      *nextAclDir = ACL_INTF_DIR_IN;
      return L7_SUCCESS;
    }
    else if ((aclDir < ACL_INTF_DIR_OUT) && (aclOutSupp == L7_TRUE))
    {
      *nextIntIfNum = intIfNum;
      *nextAclDir = ACL_INTF_DIR_OUT;
      return L7_SUCCESS;
    }
    else
    {
      /* fall through to find next valid ACL interface */
    }
  }

  /* first check for the next physical intf, then check for the next LAG intf
   * (if feature is supported) when no more physical intfs are found
   */
  rc = nimNextValidIntfNumberByType(L7_PHYSICAL_INTF, intIfNum, nextIntIfNum);

  if ((rc != L7_SUCCESS) && (lagSupp == L7_TRUE))
  {
    rc = nimNextValidIntfNumberByType(L7_LAG_INTF, intIfNum, nextIntIfNum);
  }

  if (rc == L7_SUCCESS)
  {
    /* use first supported direction for this ACL interface */
    *nextAclDir = (aclInSupp == L7_TRUE) ? ACL_INTF_DIR_IN : ACL_INTF_DIR_OUT;
  }
  else
  {
    /* assume all NIM errors imply no more interfaces found */
    rc = L7_ERROR;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Check if the ACL sequence number is in use for the specified
*           interface and direction
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    seqNum      @b{(input)}  ACL interface sequence number
*
* @returns  L7_SUCCESS  sequence number is in use
* @returns  L7_ERROR    sequence number not in use
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments A sequence number value may be used only once for a given
*           interface and direction pair.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirSequenceGet(L7_uint32 intIfNum, L7_uint32 aclDir,
                                 L7_uint32 seqNum)
{
  L7_RC_t           rc = L7_ERROR;
  aclIntfCfgData_t  *pCfg;
  aclIntfParms_t    *pList;
  L7_uint32         i;

  if (ACL_IS_READY == L7_FALSE)
    return L7_FAILURE;  /* no intf exist */

  /* check inputs */
  if ((nimCheckIfNumber(intIfNum) != L7_SUCCESS) ||
      (aclDir >= ACL_INTF_DIR_MAX) ||
      ((seqNum < L7_ACL_MIN_INTF_SEQ_NUM) || (seqNum > L7_ACL_MAX_INTF_SEQ_NUM)))
  {
    return L7_FAILURE;
  }

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* set up working ptr to assign list for this interface direction */
  pList = pCfg->intf_dir_Tb[aclDir].assignList;

  for (i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++, pList++)
  {
    if (pList->inUse != L7_TRUE)
      break;

    if (pList->seqNum == seqNum)
    {
      rc = L7_SUCCESS;
      break;
    }

  } /* endfor */

  return rc;
}

/*********************************************************************
*
* @purpose  Get next sequential ACL interface sequence number in use
*           for the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    seqNum      @b{(input)}  ACL interface sequence number
* @param    *nextSeqNum @b{(output)} next ACL interface sequence number
*
* @returns  L7_SUCCESS  next sequence number found
* @returns  L7_ERROR    next sequence number not found
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Use a seqNum value of 0 to find the first ACL sequence
*           number for this interface and direction, if it exists.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirSequenceGetNext(L7_uint32 intIfNum, L7_uint32 aclDir,
                                     L7_uint32 seqNum, L7_uint32 *nextSeqNum)
{
  L7_RC_t           rc = L7_ERROR;
  aclIntfCfgData_t  *pCfg;
  aclIntfParms_t    *pList;
  L7_uint32         i;

  if (ACL_IS_READY == L7_FALSE)
    return L7_FAILURE;  /* no intf exist */

  /* check inputs (any seqNum is allowed for getNext function) */
  if ((nimCheckIfNumber(intIfNum) != L7_SUCCESS) ||
      (aclDir >= ACL_INTF_DIR_MAX))
  {
    return L7_FAILURE;
  }

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* set up working ptr to assign list for this interface direction */
  pList = pCfg->intf_dir_Tb[aclDir].assignList;

  for (i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++, pList++)
  {
    if (pList->inUse != L7_TRUE)
      break;

    /* the next highest sequence number can occur anywhere in the assign
     * list array since it is not sorted (must process entire list)
     *
     * although sequence number may be used only once per intf,dir, the
     * '<=' test condition is used for the case where the next legitimate
     * sequence number is the maximum allowed value
     */
    if (pList->seqNum > seqNum)
    {
      *nextSeqNum = pList->seqNum;
      rc = L7_SUCCESS;
      break;
    }

  } /* endfor */

  return rc;
}

/*********************************************************************
*
* @purpose  To add an access list to the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    aclType     @b{(input)}  access-list type
* @param    aclId       @b{(input)}  access-list identifying number
* @param    seqNum      @b{(input)}  ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  vlan, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for vlan, dir
*
* @comments This internal function is used for assigning all types of ACLs
*           to an vlan.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @comments A seqnum of 0 causes the next highest sequence number to be used.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirListAdd( L7_uint32     vlanNum,
                           L7_uint32     aclDir,
                           L7_ACL_TYPE_t aclType,
                           L7_uint32     aclId,
                           L7_uint32     seqNum )
{
  aclVlanCfgData_t *pCfg;
  aclIntfParms_t   *pList;
  L7_BOOL          multiAclFeat;
  L7_BOOL          isSeqMatch;
  L7_uint32        listCount;
  L7_uint32        insertIndex;
  L7_uint32        highestSeqNum;
  L7_uint32        aclRuleCount;
  L7_uint32        totalRuleCount;
  L7_uint32        i;

  if ( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  if ( aclDir >= ACL_INTF_DIR_MAX )
  {
    return( L7_FAILURE );
  }

  multiAclFeat = cnfgrIsFeaturePresent( L7_FLEX_QOS_ACL_COMPONENT_ID,
                                        L7_ACL_VLAN_MULTIPLE_ACL_FEATURE_ID );

  isSeqMatch = L7_FALSE;
  listCount = totalRuleCount = insertIndex = highestSeqNum = 0;

  pList = pCfg->vlan_dir_Tb[aclDir].assignList;

  /* scan ACL assignment list for this vlan and direction, looking for:
   *  1) the ACL insertion point (either matching seq num or next higher)
   *  2) the last entry currently in-use
   *
   * this list is maintained as densely-packed, sorted in ascending
   * sequence number order
   */
  for ( i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++, pList++ )
  {
    /* stop search when first unused entry is reached */
    if ( pList->inUse != L7_TRUE )
    {
      break;
    }

    listCount++;
    highestSeqNum = pList->seqNum;

    /* NOTE:  An incoming seqNum of 0 will be handled below, but will cause
     *        the insertIndex to remain at 0 during this loop.
     */
    if ( pList->seqNum < seqNum )
    {
      insertIndex = i + 1;
    }
    else if ( pList->seqNum == seqNum )
    {
      insertIndex = i;
      isSeqMatch  = L7_TRUE;
    }

    /* get the number of rules in this ACL and add it to the total for vlan,dir */
    if ( aclRuleCountGet( pList->id.aclType, pList->id.aclId, &aclRuleCount ) != L7_SUCCESS )
    {
      aclRuleCount = 0;
    }

    /* add the ACL rule count to the total rule count, except if this is a
     * matching sequence number entry (since it will be replaced by the
     * incoming ACL)
     */
    if ( pList->seqNum != seqNum )
    {
      totalRuleCount += aclRuleCount;
    }
  } /* endfor */

  /* restore list ptr to the beginning of the array so indexing can be used */
  pList = pCfg->vlan_dir_Tb[aclDir].assignList;

  /* if there is already more than one ACL on this vlan, or
   * there is exactly one ACL but with a different sequence number,
   * this add operation will result in multiple ACLs being assigned
   * to the vlan
   *
   * in this case, need to check the feature support conditions for
   * multiple and mixed type ACLs
   */

  if ( ( listCount > 1 ) ||
       ( ( 1 == listCount ) &&
         ( isSeqMatch != L7_TRUE ) ) )
  {
    /* fail if vlan supports only one ACL per direction */
    if ( L7_FALSE == multiAclFeat )
    {
      return( L7_ALREADY_CONFIGURED );
    }
  }

  /* add the rule count for the incoming ACL to the total and check if it
   * exceeds the maximum allowed rule limit for this platform
   */
  if ( aclRuleCountGet( aclType, aclId, &aclRuleCount ) != L7_SUCCESS )
  {
    aclRuleCount = 0;
  }

  totalRuleCount += aclRuleCount;

  if ( totalRuleCount > L7_ACL_MAX_RULES_PER_MULTILIST )
  {
    /* adding this ACL would exceed maximum number of rules per vlan,dir */
    return( L7_TABLE_IS_FULL );
  }

  /* at this point, the insertIndex indicates either:
   *  - the entry of the matching sequence number, or
   *  - the entry of the list insertion point, or
   *  - the first entry of an empty list, or
   *  - the first entry when the incoming seqNum is 0 (needs adjustment)
   */

  if ( L7_TRUE == isSeqMatch )
  {
    /* nothing more to do here, except to replace the ACL entry (below)
     *
     * a matching sequence number entry can be updated whether or not
     * the table is full
     */
  }
  else if ( listCount >= L7_ACL_MAX_LISTS_PER_VLAN_DIR )
  {
    /* no more room in the table */
    return( L7_TABLE_IS_FULL );
  }
  else if ( 0 == seqNum )
  {
    /* this entry goes at the end of the list using a sequence number
     * one greater than the highest sequence number currently in use,
     * provided not already using the maximum allowed sequence number
     *
     * NOTE:  The isSeqMatch can never be true if seqNum is 0.
     */
    if ( highestSeqNum >= L7_ACL_MAX_VLAN_SEQ_NUM )
    {
      return( L7_ERROR );
    }

    seqNum = highestSeqNum + 1;

    /* set insertIndex to next available element (it is currently set to 0) */
    insertIndex = listCount;
  }
  else /* (isSeqMatch != L7_TRUE) */
  {
    /* if a matching sequence number entry was not found, shift all existing
     * entries following the insertIndex right one position to make room
     * for the new ACL entry, working backwards through the list of
     * existing entries
     *
     * an empty list or a list whose insertIndex is pointing beyond the
     * last existing element will not cause any entries to be shifted here
     */
    for ( i = listCount; i > insertIndex; i-- )
    {
      pList[i] = pList[i-1];
    }
  }

  /* the insertIndex denotes the location to set the new ACL entry in the list */
  pList[insertIndex].inUse           = L7_TRUE;
  pList[insertIndex].seqNum          = seqNum;

  pList[insertIndex].id.aclType      = aclType;
  pList[insertIndex].id.aclId        = aclId;

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return( L7_SUCCESS );
}

/*********************************************************************
*
* @purpose  To remove an access list from the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    aclType     @b{(input)}  access-list type
* @param    aclId       @b{(input)}  access-list identifying number
*
* @returns  L7_SUCCESS  ACL successfully removed from vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL not currently attached to vlan, dir
*
* @comments This internal function is used for removing all types of ACLs
*           from an vlan.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirListRemove( L7_uint32     vlanNum,
                              L7_uint32     aclDir,
                              L7_ACL_TYPE_t aclType,
                              L7_uint32     aclId )
{
  L7_BOOL          found = L7_FALSE;
  aclVlanCfgData_t *pCfg;
  aclIntfParms_t   *pList;
  L7_uint32        i;

  if ( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  if ( aclDir >= ACL_INTF_DIR_MAX )
  {
    return( L7_FAILURE );
  }

  pList = pCfg->vlan_dir_Tb[aclDir].assignList;

  /* search ACL assignment list for this vlan and direction,
   * looking for the matching ACL type and id
   *
   * NOTE:  Using array notation for pList to allow easier shifting
   *        of entries using indexes
   */
  for ( i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++ )
  {
    if ( pList[i].inUse != L7_TRUE )
    {
      break;
    }

    if ( ( pList[i].id.aclType == aclType ) &&
         ( pList[i].id.aclId   == aclId ) )
    {
      found = L7_TRUE;
      break;
    }
  } /* endfor */

  if ( found != L7_TRUE )
  {
    return( L7_ERROR );
  }

  /* starting with the matching entry, shift each in use entry left one
   * position in the array
   *
   * always stop one element short of the end of the array to allow the
   * last entry to be cleared out of a previously full list
   */
  for ( ; i < (L7_ACL_MAX_LISTS_PER_VLAN_DIR - 1); i++ )
  {
    if ( pList[i+1].inUse != L7_TRUE )
    {
      break;
    }
    pList[i] = pList[i+1];
  }

  /* clear out the last vacated entry */
  pList[i].inUse                     = L7_FALSE;
  pList[i].seqNum                    = 0;

  pList[i].id.aclType                = L7_ACL_TYPE_NONE;
  pList[i].id.aclId                  = 0;

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return( L7_SUCCESS );
}

/*********************************************************************
*
* @purpose  To purge all access lists from the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
*
* @returns  L7_SUCCESS  all ACLs successfully purged from vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
*
* @comments This internal function is used for purging all types of ACLs
*           from an vlan.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirListPurge( L7_uint32 vlanNum, L7_uint32 aclDir )
{
  aclVlanCfgData_t *pCfg;
  aclIntfParms_t   *pList;
  L7_uint32        i;

  if ( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  if ( aclDir >= ACL_INTF_DIR_MAX )
  {
    return( L7_FAILURE );
  }

  pList = pCfg->vlan_dir_Tb[aclDir].assignList;

  /* clear out entire assign list for this vlan, dir */
  for ( i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++, pList++ )
  {
    /* NOTE:  Although the list is densely-packed, loop through entire
     *        list just in case some entry got orphaned.
     */
    pList->inUse      = L7_FALSE;
    pList->seqNum     = 0;

    pList->id.aclType = L7_ACL_TYPE_NONE;
    pList->id.aclId   = 0;

  } /* endfor */

  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;

  return( L7_SUCCESS );
}

/*********************************************************************
*
* @purpose  Check if the specified vlan and direction is valid
*           for use with ACL
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
*
* @returns  L7_SUCCESS  vlan,dir is valid for ACL
* @returns  L7_ERROR    vlan,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the vlan
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the vlan
*           and direction (see aclImpIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirGet( L7_uint32 vlanNum, L7_uint32 aclDir )
{
  L7_uint32 featureId;

  if ( L7_FALSE == ACL_IS_READY )
  {
    return( L7_FAILURE );  /* no vlan exist */
  }

  /* check inputs */
  if ( aclDir >= ACL_INTF_DIR_MAX )
  {
    return( L7_FAILURE );
  }

  if ( aclIsValidVlan( vlanNum ) != L7_TRUE )
  {
    return( L7_ERROR );
  }

  /* check the feature support to determine which ACL vlan directions are allowed */
  if ( ACL_INTF_DIR_IN == aclDir )
  {
    featureId = L7_ACL_VLAN_DIRECTION_INBOUND_FEATURE_ID;
  }
  else
  {
    featureId = L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID;
  }

  if ( cnfgrIsFeaturePresent( L7_FLEX_QOS_ACL_COMPONENT_ID, featureId ) != L7_TRUE )
  {
    return( L7_ERROR );
  }

  return( L7_SUCCESS );
}

/*********************************************************************
*
* @purpose  Get next sequential vlan and direction that is valid
*           for use with ACL
*
* @param    vlanNum         @b{(input)}  internal vlan number
* @param    aclDir          @b{(input)}  vlan direction (internal)
* @param    *nextVlanNum    @b{(output)} next internal vlan number
* @param    *nextAclDir     @b{(output)} next vlan direction (internal)
*
* @returns  L7_SUCCESS  next vlan,dir for ACL was obtained
* @returns  L7_ERROR    no more valid vlan,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an vlan and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the vlan
*           and direction (see aclImpIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirGetNext( L7_uint32 vlanNum,
                              L7_uint32 aclDir,
                              L7_uint32 *nextVlanNum,
                              L7_uint32 *nextAclDir )
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL aclInSupp;
  L7_BOOL aclOutSupp;

  if ( L7_FALSE == ACL_IS_READY )
  {
    return( L7_FAILURE );  /* no vlan exist */
  }

  /* check inputs */
  if ( ( L7_NULLPTR == nextVlanNum ) ||
       ( L7_NULLPTR == nextAclDir ) )
  {
    return( L7_FAILURE );
  }

  /* determine which ACL directions are supported */
  aclInSupp  = cnfgrIsFeaturePresent( L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_VLAN_DIRECTION_INBOUND_FEATURE_ID );
  aclOutSupp = cnfgrIsFeaturePresent( L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID );
  if ( ( L7_FALSE == aclInSupp ) &&
       ( L7_FALSE == aclOutSupp ) )
  {
    return( L7_FAILURE );
  }

  if ( L7_TRUE == aclIsValidVlan( vlanNum ) )
  {
    /* first try getting next direction for this valid ACL vlan
     * (depends on feature support)
     */
    if ( ( aclDir < ACL_INTF_DIR_IN ) &&
         ( L7_TRUE == aclInSupp ) )
    {
      *nextVlanNum = vlanNum;
      *nextAclDir  = ACL_INTF_DIR_IN;
      return( L7_SUCCESS );
    }
    else if ( ( aclDir < ACL_INTF_DIR_OUT ) &&
              ( L7_TRUE == aclOutSupp ) )
    {
      *nextVlanNum = vlanNum;
      *nextAclDir  = ACL_INTF_DIR_OUT;
      return( L7_SUCCESS );
    }
    else
    {
      /* fall through to find next valid ACL vlan */
    }
  }

  /*
   * Check for the next physical vlan
   */
  if ( L7_TRUE == aclIsValidVlan( vlanNum+1 ) )
  {
    *nextVlanNum = vlanNum+1;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  if ( L7_SUCCESS == rc )
  {
    /* use first supported direction for this ACL vlan */
    *nextAclDir = ( L7_TRUE == aclInSupp ) ? ACL_INTF_DIR_IN : ACL_INTF_DIR_OUT;
  }
  else
  {
    /* assume all errors imply no more vlans found */
    rc = L7_ERROR;
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  Check if the ACL sequence number is in use for the specified
*           vlan and direction
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
*
* @returns  L7_SUCCESS  sequence number is in use
* @returns  L7_ERROR    sequence number not in use
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments A sequence number value may be used only once for a given
*           vlan and direction pair.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirSequenceGet( L7_uint32 vlanNum,
                                  L7_uint32 aclDir,
                                  L7_uint32 seqNum )
{
  L7_RC_t           rc = L7_ERROR;
  aclVlanCfgData_t  *pCfg;
  aclIntfParms_t    *pList;
  L7_uint32         i;

  /* check inputs */
  if ( ( aclIsValidVlan( vlanNum ) != L7_TRUE ) ||
       ( aclDir >= ACL_INTF_DIR_MAX ) ||
       ( ( seqNum < L7_ACL_MIN_VLAN_SEQ_NUM ) ||
         ( seqNum > L7_ACL_MAX_VLAN_SEQ_NUM ) ) )
  {
    return( L7_FAILURE );
  }

  if ( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  /* set up working ptr to assign list for this vlan direction */
  pList = pCfg->vlan_dir_Tb[aclDir].assignList;

  for ( i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++, pList++ )
  {
    if ( pList->inUse != L7_TRUE )
    {
      break;
    }

    if ( pList->seqNum == seqNum )
    {
      rc = L7_SUCCESS;
      break;
    }

  } /* endfor */

  return( rc );
}

/*********************************************************************
*
* @purpose  Get next sequential ACL vlan sequence number in use
*           for the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
* @param    *nextSeqNum @b{(output)} next ACL vlan sequence number
*
* @returns  L7_SUCCESS  next sequence number found
* @returns  L7_ERROR    next sequence number not found
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Use a seqNum value of 0 to find the first ACL sequence
*           number for this vlan and direction, if it exists.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirSequenceGetNext( L7_uint32 vlanNum,
                                      L7_uint32 aclDir,
                                      L7_uint32 seqNum,
                                      L7_uint32 *nextSeqNum )
{
  L7_RC_t           rc = L7_ERROR;
  aclVlanCfgData_t  *pCfg;
  aclIntfParms_t    *pList;
  L7_uint32         i;

  /* check inputs (any seqNum is allowed for getNext function) */
  if ( ( aclIsValidVlan( vlanNum ) != L7_TRUE ) ||
       ( aclDir >= ACL_INTF_DIR_MAX ) )
  {
    return( L7_FAILURE );
  }

  if ( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  /* set up working ptr to assign list for this vlan direction */
  pList = pCfg->vlan_dir_Tb[aclDir].assignList;

  for ( i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++, pList++ )
  {
    if ( pList->inUse != L7_TRUE )
    {
      break;
    }

    /* the next highest sequence number can occur anywhere in the assign
     * list array since it is not sorted (must process entire list)
     *
     * although sequence number may be used only once per vlan,dir, the
     * '<=' test condition is used for the case where the next legitimate
     * sequence number is the maximum allowed value
     */
    if ( pList->seqNum > seqNum )
    {
      *nextSeqNum = pList->seqNum;
      rc          = L7_SUCCESS;
      break;
    }

  } /* endfor */

  return( rc );
}

/*********************************************************************
*
* @purpose  Get a aclStructure_t where the head is rule rulenum
*
* @param    L7_uint32   rulenum  the current rule ID
* @param    L7_uint32   *acl_ptr  reference to the rule
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleGet(L7_uint32 aclnum, L7_uint32 rulenum, aclRuleParms_t **acl_rp)
{
  aclStructure_t *acl_ptr;

  if ((rulenum < L7_ACL_MIN_RULE_NUM) || (rulenum > L7_ACL_MAX_RULE_NUM))
  {
    return L7_ERROR;
  }

  acl_ptr = (aclStructure_t*)aclFindACL(aclnum);

  if (acl_ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  (*acl_rp) = acl_ptr->head;

  while ((*acl_rp) != L7_NULLPTR)
  {

    if ( (*acl_rp)->ruleNum == rulenum )
    {
     return L7_SUCCESS;
    }

    else
    {
      (*acl_rp) = (*acl_rp)->next;
    }

  }

  return L7_ERROR; /* not found */
}

/*********************************************************************
*
* @purpose  Adds/removes an updated ACL interface assignment list
*           to/from the driver
*
* @param    L7_uint32   intf     the internal interface number
* @param    L7_uint32   aclDir   the interface direction (internal)
* @param    L7_BOOL     op       operation (ACL_INTF_ADD, ACL_INTF_REMOVE)
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments Will build a TLV and send the ACL to the driver
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfUpdate(L7_uint32 intf, L7_uint32 aclDir, L7_uint32 op)
{
  L7_RC_t       rc;

  /* NOTE:  ACL_INTF_ADD and ACL_INTF_REMOVE are always for a specific
   *        interface and direction, so the intIfNum and dir input parms
   *        are used.  An aclnum parameter is not needed, since the
   *        entire assignList is processed instead of just one ACL.
   */

  switch (op)
  {
    case ACL_INTF_ADD:
      /* build an acl create tlv for intf,dir */
      rc = aclBuildTLVCreate(intf, 0, aclDir);
      break;

    case ACL_INTF_REMOVE:
      /* build an acl delete tlv for intf,dir */
      rc = aclBuildTLVDelete(intf, 0, aclDir);
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Adds/removes an updated ACL VLAN assignment list
*           to/from the driver
*
* @param    L7_uint32   vlan     the internal VLAN number
* @param    L7_uint32   aclDir   the VLAN direction (internal)
* @param    L7_BOOL     op       operation (ACL_VLAN_ADD, ACL_VLAN_REMOVE)
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments Will build a TLV and send the ACL to the driver
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanUpdate( L7_uint32 vlanNum, L7_uint32 aclDir, L7_uint32 op )
{
  L7_RC_t rc = L7_SUCCESS;

  /* NOTE:  ACL_VLAN_ADD and ACL_VLAN_REMOVE are always for a specific
   *        interface and direction, so the vlanNum and dir input parms
   *        are used.  An aclnum parameter is not needed, since the
   *        entire assignList is processed instead of just one ACL.
   */

  switch ( op )
  {
    case ACL_VLAN_ADD:
      /* build an acl create tlv for vlan,dir */
      rc = aclBuildTLVCreate( 0, vlanNum, aclDir );
      break;

    case ACL_VLAN_REMOVE:
      /* build an acl delete tlv for vlan,dir */
      rc = aclBuildTLVDelete( 0, vlanNum, aclDir );
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Sends a recently updated ACL to the driver
*
* @param    L7_uint32   aclnum   the access-list identifying number
* @param    L7_BOOL     op       operation (ACL_MODIFY)
*
* @returns  L7_SUCCESS, if all interfaces updated
* @returns  L7_ERROR,   if TLV operation failed
* @returns  L7_FAILURE, if other failure
*
* @comments Will build a TLV and send the ACL to the driver
*
* @end
*
*********************************************************************/
L7_RC_t aclUpdate(L7_uint32 aclnum, L7_uint32 op)
{
  return aclUpdateExceptIntf(aclnum, op, 0);
}

/*********************************************************************
*
* @purpose  Sends a recently updated ACL to the driver, except for
*           the specified interface
*
* @param    L7_uint32   aclnum   the access-list identifying number
* @param    L7_BOOL     op       operation (ACL_MODIFY)
* @param    L7_uint32   intIfNum internal interface number, or 0
*
* @returns  L7_SUCCESS, if all interfaces updated
* @returns  L7_ERROR,   if TLV operation failed
* @returns  L7_FAILURE, if other failure
*
* @comments Will build a TLV and send the ACL to the driver for each
*           interface to which it is attached EXCEPT for the specified
*           intIfNum.  If the intIfNum is 0, all of the ACL's interfaces
*           are updated.
*
* @end
*
*********************************************************************/
L7_RC_t aclUpdateExceptIntf(L7_uint32 aclnum, L7_uint32 op, L7_uint32 intIfNum)
{
  L7_RC_t                     rc = L7_SUCCESS;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  L7_uint32                   d, i;
 L7_ACL_ASSIGNED_VLAN_LIST_t vlanList;

  /* NOTE:  ACL_MODIFY affects all interfaces and directions to which
   *        this ACL is assigned.
   */

  switch (op)
  {
    case ACL_MODIFY:
      /* if not assoc with any intfs, noop */
      /* build a delete tlv for intf,dir and delete */
      /* build a create tlv for intf,dir with new info and create */
      for (d=0; d<ACL_INTF_DIR_MAX; d++)
      {
        if (aclImpAssignedIntfDirListGet(aclnum, d, &intfList) == L7_SUCCESS)
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
      /* build a create tlv for vlan,dir with new info and create */
      for (d = 0; d < ACL_INTF_DIR_MAX; d++)
      {
        if (L7_SUCCESS == aclImpAssignedVlanDirListGet(aclnum, d, &vlanList))
        {
          for (i = 0; i < vlanList.count; i++)
          {
            /* NOTE:  The following will call aclBuildVlanTLVDelete() for an
             *        ACL TLV that is currently in the device.
             */
            if (aclBuildTLVCreate(0, vlanList.vlanNum[i], d) != L7_SUCCESS)
            {
              return(L7_ERROR);
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
* @purpose  Creates a single TLV containing active rules of the specified
*           ACL within it and return the data in TLV block storage
*           provided by the caller.
*
* @param    L7_ACL_TYPE_t     aclType   access-list type
* @param    L7_uint32         aclnum    access-list identifying number
* @param    L7_tlvHandle_t    tlvHandle TLV block handle (supplied by caller)
*
* @returns  L7_SUCCESS, if TLV successfully built
* @returns  L7_ERROR,   if problem with TLV creation
* @returns  L7_FAILURE  if any other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The information contained in the TLV represents a snapshot of
*           the data in the access list  database at time of invocation.
*           The tlvHandle parm designates the TLV block supplied by the caller.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpTlvGet(L7_ACL_TYPE_t aclType, L7_uint32 aclnum, L7_tlvHandle_t tlvHandle)
{
  L7_RC_t             rc;
  aclId_t             aclIdent;

  /* set up dummy values for parms expected by common TLV build function */
  aclIdent.aclType = aclType;
  aclIdent.aclId = aclnum;

  /* build the TLV (but don't send it to DTL) */
  rc = aclImpBuildTLVCreate(0, 0, ACL_INTF_DIR_IN, tlvHandle, &aclIdent);

  return rc;
}

/*********************************************************************
*
* @purpose  Builds an ACL Create TLV and sends it to DTL
*
* @param    L7_uint32   intf     interface associated with this ACL
* @param    L7_uint32   vlan     vlan ID associated with this ACL
* @param    L7_uint32   aclDir   interface direction (internal)
*
* @returns  L7_SUCCESS, if TLV successfully built and issued
* @returns  L7_ERROR,   if problem with TLV creation
* @returns  L7_FAILURE  if any other failure
*
* @comments Invoke multiple times for each interface, direction or
*           VLAN ID, direction
*
* @comments This function is used to build TLV data when either applying
*           ACLs to an interface or binding ACLs to a VLAN ID.
*           All rules from each ACL currently assigned to this interface/VLAN ID
*           and direction are combined into a single ACL List TLV for
*           use by the device.  There is one implicit 'deny all' rule
*           added to the end of the TLV (if no configured 'match every'
*           rule is used).
*
* @note     Only one of the two parameters intf or vlan shall be valid.
*           A zero value for vlan will cause the intf parameter to be the valid
*           input and the TLV will be constructed for that interface.  If vlan is non-zero, it will
*           be used to construct a VLAN ID associated TLV.  This is possible since 0 is not
*           a valid VLAN ID value.
*
* @end
*
*********************************************************************/
L7_RC_t aclBuildTLVCreate(L7_uint32 intf, L7_uint32 vlan, L7_uint32 aclDir)
{
  /* this is just a wrapper for the actual implementation function */
  return aclImpBuildTLVCreate(intf, vlan, aclDir, aclTlvHandle, L7_NULLPTR);
}

/*********************************************************************
*
* @purpose  Builds an ACL Create TLV and optionally sends it to DTL
*
* @param    L7_uint32       intf        interface associated with this ACL
* @param    L7_uint32       vlan        vlan ID associated with this ACL
* @param    L7_uint32       aclDir      interface direction (internal)
* @param    L7_tlvHandle_t  tlvHandle   TLV block handle to use
* @param    aclId_t         *pAclIdent  Single ACL identifier, or L7_NULLPTR
*
* @returns  L7_SUCCESS, if TLV successfully built and issued
* @returns  L7_ERROR,   if problem with TLV creation
* @returns  L7_FAILURE  if any other failure
*
* @comments Invoke multiple times for each interface, direction or
*           VLAN ID, direction
*
* @comments This function is used to build TLV data when either applying
*           ACLs to an interface or binding ACLs to a VLAN ID.
*           All rules from each ACL currently assigned to this interface/VLAN ID
*           and direction are combined into a single ACL List TLV for
*           use by the device.  There is one implicit 'deny all' rule
*           added to the end of the TLV (if no configured 'match every'
*           rule is used).
*
* @comments Only one of the two parameters intf or vlan shall be valid.
*           A zero value for vlan will cause the intf parameter to be the valid
*           input and the TLV will be constructed for that interface.  If vlan is non-zero, it will
*           be used to construct a VLAN ID associated TLV.  This is possible since 0 is not
*           a valid VLAN ID value.
*
* @comments The pAclIdent parm is normally set to L7_NULLPTR when issuing a TLV to
*           an interface or VLAN.  However, if this parm is non-null, then this
*           function will only produce the TLV data for the specified ACL and will
*           not perform any other activity related to interface or VLAN.  In this
*           case, the intf and vlan parms are both passed in as 0 by the caller
*           and are ignored by this routine.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpBuildTLVCreate(L7_uint32 intf, L7_uint32 vlan, L7_uint32 aclDir,
                             L7_tlvHandle_t tlvHandle, aclId_t *pAclIdent)
{
  L7_BOOL             macRuleSeen = L7_FALSE;
  L7_BOOL             ipv6RuleSeen = L7_FALSE;
  L7_BOOL             ipv4RuleSeen = L7_FALSE;
  aclTlvMatchEvery_t  matchEveryFlags = ACL_TLV_MATCH_EVERY_NONE;
  L7_RC_t             rc, rc2;
  aclIntfParms_t      *pList;
  aclIntfCfgData_t    *pIntfCfg;
  aclVlanCfgData_t    *pVlanCfg;
  L7_uint32           i;
  aclTlvRuleDef_t     rule;
  aclTlvMatchEtype_t  etypeRule;
  aclTlvMatchVlanId_t vlanIdRule;
  L7_uint32           totalListCount, totalRuleCount, aclRuleCount;
  L7_tlv_t            *pTLV;
  L7_uchar8           *pByteTLV;
  L7_uint32           tlvTotalSize;
  L7_uint32           direction;
  L7_uint32           list_loop_end;
  aclTlvListType_t    aclListTlv, *pAclListTlv;
  aclIntfDirInfo_t    *pDirInfo;
  L7_uint32           instanceKey;
  /* the following are used when just building (but not issuing) the TLV data */
  aclIntfParms_t      tempAssignList[L7_ACL_MAX_LISTS_PER_INTF_DIR];
  aclIntfDirInfo_t    tempDirInfo;
  aclIntfTRInfo_t     *trInfo = L7_NULLPTR;

  /* PTIn Added */
  L7_uint8 actionFlag = 0;

  rc2 = L7_SUCCESS;

  totalListCount = 0;
  totalRuleCount = 0;

  /* check ACL internal direction value */
  if (aclDir >= ACL_INTF_DIR_MAX)
    return L7_FAILURE;

  if (L7_NULLPTR == pAclIdent)
  {
    if (0 == vlan)
    {
      /* if the interface is not in one of the states where hw access is allowed
       * then we will simply return success
       */
      if (aclIsIntfAttached(intf, L7_NULLPTR) == L7_FALSE)
        return L7_SUCCESS;

      /* just return with success if the interface is currently in an acquired state */
      if (aclIsIntfAcquired(intf) == L7_TRUE)
        return L7_SUCCESS;

      if (aclMapIntfIsConfigurable(intf, &pIntfCfg) != L7_TRUE)
        return L7_FAILURE;

      pList = pIntfCfg->intf_dir_Tb[aclDir].assignList;

      /* set up ptr to intf directional information */
      pDirInfo = &pAclIntfInfo[intf].dir[aclDir];

      list_loop_end = L7_ACL_MAX_LISTS_PER_INTF_DIR;
    }
    else
    {
      /* if this vlan id not configured, return success (analogous to an interface not attached */
      if ( aclMapVlanIsConfigurable( vlan, &pVlanCfg ) != L7_TRUE )
      {
        return( L7_FAILURE );
      }

      pList = pVlanCfg->vlan_dir_Tb[aclDir].assignList;

      pDirInfo = &pAclVlanInfo[aclVlanMapTbl[vlan]].dir[aclDir];

      list_loop_end = L7_ACL_MAX_LISTS_PER_VLAN_DIR;
    }
    
    /* Time Range Information */
    trInfo = pDirInfo->tlvTRInfo;

    /* force a TLV deletion first if TLV is currently in device */
    if (pDirInfo->tlvInDevice == L7_TRUE)
    {
      if (aclBuildTLVDelete(intf, vlan, aclDir) != L7_SUCCESS)
        return L7_FAILURE;
    }
  }
  else /* (L7_NULLPTR != pAclIdent) */
  {
    /* only want to build TLV data for specified ACL, but not issue it to DTL */
    memset(&tempDirInfo, 0, sizeof(tempDirInfo));
    memset(tempAssignList, 0, sizeof(tempAssignList));
    tempAssignList[0].inUse = L7_TRUE;
    tempAssignList[0].seqNum = 1;
    tempAssignList[0].id = *pAclIdent;

    pList = tempAssignList;
    pDirInfo = &tempDirInfo;
    list_loop_end = 1;
  }

  rc = tlvCreate(tlvHandle);
  if (rc != L7_SUCCESS)
  {
    /* not enough resources */
    return rc;
  }

  /* clear out the TLV correlator list for this intf,dir to begin building TLV*/
  pDirInfo->tlvCorrCount = 0;
  if(sizeof(pDirInfo->tlvCorrList) > 0)
  {
    memset((L7_char8 *)pDirInfo->tlvCorrList, 0, sizeof(pDirInfo->tlvCorrList));
  }
  /* clear out the TLV correlator list for Time based ACLs 
   * if time ranges feature supported
   */
  
  if(trInfo != L7_NULLPTR)
  {
    trInfo->tlvTimeRangeCorrCount = 0;
    if(sizeof(trInfo->tlvTimeRangeCorrList) > 0)
    {
      memset((L7_char8 *)trInfo->tlvTimeRangeCorrList, 0, sizeof(trInfo->tlvTimeRangeCorrList));
    } 
  }

  /* NOTE:  The instance key field is zeroed out here, then set with the result
   *        of the TLV crc32 calculation below
   */
  memset(&aclListTlv, 0, sizeof(aclListTlv));
  aclListTlv.instanceKey = (L7_uint32)osapiHtonl(0);

  rc = tlvOpen(tlvHandle, L7_QOS_ACL_TLV_LIST_TYPE, L7_QOS_ACL_TLV_LIST_TYPE_LEN, (L7_uchar8*)&aclListTlv);
  if (rc != L7_SUCCESS)
  {
    (void)tlvDelete(tlvHandle);   /* this rc not meaningful */
    return rc;
  }

  /* walk through the assign list for this interface, direction to build
   * each set of rules for the assigned ACLs
   */

  for (i = 0; i < list_loop_end; i++, pList++)
  {
    /* the in-use elements are contiguous from the start of the list and
     * are arranged by increasing sequence number
     */
    if (pList->inUse != L7_TRUE)
      break;

    aclRuleCount = 0;                   /* re-init each pass */

    switch (pList->id.aclType)
    {
    case L7_ACL_TYPE_IP:
      rc = aclTlvRuleDefBuild(pList->id.aclId, &aclRuleCount, &matchEveryFlags,
                              pDirInfo, vlan, tlvHandle, &actionFlag);
      if (aclRuleCount > 0)
        ipv4RuleSeen = L7_TRUE;
      break;

    case L7_ACL_TYPE_IPV6:
      rc = aclTlvRuleDefBuild(pList->id.aclId, &aclRuleCount, &matchEveryFlags,
                              pDirInfo, vlan, tlvHandle, &actionFlag);
      if (aclRuleCount > 0)
        ipv6RuleSeen = L7_TRUE;         /* at least one IPv6 rule required for implicit deny all */
      break;

    case L7_ACL_TYPE_MAC:
      rc = aclMacTlvRuleDefBuild(pList->id.aclId, &aclRuleCount, &matchEveryFlags,
                                 pDirInfo, vlan, tlvHandle, &actionFlag);
      if (aclRuleCount > 0)
        macRuleSeen = L7_TRUE;          /* now need to deny all MAC instead */
      break;

    case L7_ACL_TYPE_NONE:
    default:
      /* ignore all other types */
      rc = L7_SUCCESS;
      break;

    } /* endswitch */

    if (rc != L7_SUCCESS)
    {
      aclCorrCountReset(pDirInfo);
      (void)tlvDelete(tlvHandle);   /* this rc not meaningful */
      return rc;
    }

    totalListCount++;
    totalRuleCount += aclRuleCount;

  } /* endfor */

  /* exit successfully if no ACLs were found in the assign list */
  if (totalListCount == 0)
  {
    aclCorrCountReset(pDirInfo);
    (void)tlvDelete(tlvHandle);   /* this rc not meaningful */
    return L7_SUCCESS;
  }

  if (L7_NULLPTR == pAclIdent)
  {
    /* if too many rules for this intf,dir, delete the TLV and build a new
     * one with just a 'deny all' rule to give the user a chance to correct
     * the configuration without leaving the interface unprotected
     *
     * NOTE: This assumes the L7_ACL_MAX_RULES_PER_MULTILIST is defined by
     *       the platform to account for any additional implicit
     *       'deny all' rule(s) that will be added for a given ACL type.
     */
    if (totalRuleCount > L7_ACL_MAX_RULES_PER_MULTILIST)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intf, L7_SYSNAME, ifName);
     
      aclCorrCountReset(pDirInfo);

      if ((tlvDelete(tlvHandle) != L7_SUCCESS) ||
          (tlvCreate(tlvHandle) != L7_SUCCESS))
      {
        return L7_ERROR;
      }

      if (tlvOpen(tlvHandle, L7_QOS_ACL_TLV_LIST_TYPE, L7_QOS_ACL_TLV_LIST_TYPE_LEN, (L7_uchar8*)&aclListTlv)
          != L7_SUCCESS)
      {
        (void)tlvDelete(tlvHandle);   /* this rc not meaningful */
        return L7_ERROR;
      }

      /* put a message in the log */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_ACL_COMPONENT_ID,
              "Total number of ACL rules (%u) exceeds max (%u) on intf %s %s. "
              "Applying \'deny all\' rule to device."
              " The combination of all ACLs applied to an interface has resulted"
              " in requiring more rules than the platform supports.",
              totalRuleCount, (L7_uint32)L7_ACL_MAX_RULES_PER_MULTILIST,
              ifName, acl_direction_str[aclDir]);

      /* save desired return code for later on */
      rc2 = L7_FAILURE;

      /* clear flags to allow 'deny all' rule to be built next */
      matchEveryFlags = ACL_TLV_MATCH_EVERY_NONE;
    }
  } /* endif pAclIdent is null */

  /* only write the implicit 'deny all' rule into TLV if a non time based 
   * 'match every' type of rule has not already been used for this set of 
   * ACLs if any MAC ACL was used, this is treated as a deny all MAC (i.e. all 
   * L2 packets), otherwise it is a deny all IPv6 (i.e. Ethertype 0x86DD)
   * or IPv4 (i.e. Ethertype 0x0800).
   *
   * NOTE:  A previously written deny all IPv4/IPv6 still needs the deny all MAC
   *        when any MAC ACL is used
   */
  if ( ((matchEveryFlags & ACL_TLV_MATCH_EVERY_MAC) == 0) && (actionFlag == 0) )
  {
    memset(&rule, 0, sizeof(rule));
    rule.denyFlag = L7_TRUE;
    if (macRuleSeen == L7_TRUE)
    {
      /* create a 'match every' deny rule if any MAC ACLs were used */
      rc=tlvOpen(tlvHandle, L7_QOS_ACL_TLV_RULE_DEF, L7_QOS_ACL_TLV_RULE_DEF_LEN, (L7_uchar8*)&rule);
      /* if building TLV for application to a VLAN, need to build the deny rule to only apply to traffic on target VLAN */
      if (vlan == 0)
      {
        rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_EVERY_TYPE, L7_QOS_ACL_TLV_MATCH_EVERY_LEN, L7_NULLPTR);
      }
      else
      {
        memset(&vlanIdRule, 0, sizeof(vlanIdRule));
        vlanIdRule.vidStart = osapiHtons(vlan);
        vlanIdRule.vidEnd = osapiHtons(vlan);
        rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID_LEN, (L7_uchar8*)&vlanIdRule);
      }
      rc=tlvClose(tlvHandle,L7_QOS_ACL_TLV_RULE_DEF);
    }
    else
    {
      if (ipv6RuleSeen == L7_TRUE)
      {
        if ((matchEveryFlags & ACL_TLV_MATCH_EVERY_IPV6) == 0)
        {
          /* create an Ethertype 0x86DD match condition if any IPv6 ACL rules
           * were processed and a match every IPv6 rule has not been written yet
           *
           * to allow IPv6 neighbor discovery to work, two ICMPv6 permit rules
           * must always preceed the deny all IPv6 rule (thus, three rules are
           * generated for the implicit IPv6 'deny all')
           */
            rc = aclTlvRuleIpv6DenyAllBuild(vlan, tlvHandle);
        }
      }

      /* totalRuleCount == 0 check is for degenerate case where IPv4 ACL
       * was created but no rules defined (possible via Web or SNMP)
       *
       * build an implicit deny-all rule for consistency with the way
       * this was handled in the past (it would have been better if an
       * empty list did not generate an implicit deny-all, as is the case
       * for named ACLs)
       */
      if ((ipv4RuleSeen == L7_TRUE) || (totalRuleCount == 0))
      {
        if ((matchEveryFlags & ACL_TLV_MATCH_EVERY_IP) == 0)
        {
          /* create an Ethertype 0x0800 deny rule if IPv4 ACLs were used
           * and a match every IPv4 rule has not been written yet
           */
          memset(&etypeRule, 0, sizeof(etypeRule));
          etypeRule.etypeValue1 = osapiHtons((L7_ushort16)L7_QOS_ETYPE_ID_IPV4);
          etypeRule.checkStdHdrFlag = L7_TRUE;
          rc=tlvOpen(tlvHandle, L7_QOS_ACL_TLV_RULE_DEF, L7_QOS_ACL_TLV_RULE_DEF_LEN, (L7_uchar8*)&rule);
          rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE, L7_QOS_ACL_TLV_MATCH_ETYPE_LEN,
                      (L7_uchar8 *)&etypeRule);
          /* if building TLV for application to a VLAN, need to build the deny rule to only apply to traffic on target VLAN */
          if (vlan != 0)
          {
            memset(&vlanIdRule, 0, sizeof(vlanIdRule));
            vlanIdRule.vidStart = osapiHtons(vlan);
            vlanIdRule.vidEnd = osapiHtons(vlan);
            rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID_LEN, (L7_uchar8*)&vlanIdRule);
          }
          rc=tlvClose(tlvHandle,L7_QOS_ACL_TLV_RULE_DEF);
        }
      }
    }
  }

  if (rc == L7_SUCCESS)
  {
    rc=tlvClose(tlvHandle,L7_QOS_ACL_TLV_LIST_TYPE);

    rc = tlvComplete(tlvHandle);
    if (rc != L7_SUCCESS)
    {
      /* open and closes dont match up */
      aclCorrCountReset(pDirInfo);
      (void)tlvDelete(tlvHandle);   /* this rc not meaningful */
      return rc;
    }

    rc = tlvQuery(tlvHandle, &pTLV, &tlvTotalSize);
    pByteTLV = (L7_uchar8*)pTLV;

    /* calculate crc32 over entire TLV and store it in the instance key field
     * (the instance key was previously initialized to 0 for this calculation)
     */
    pAclListTlv = (aclTlvListType_t *)pTLV->valueStart;
    instanceKey = nvStoreCrc32(pByteTLV, tlvTotalSize);
    pAclListTlv->instanceKey = (L7_uint32)osapiHtonl((L7_ulong32)instanceKey);

    /* translate ACL internal direction to API direction value for DTL */
    (void)aclDirIntToExt(aclDir, &direction);   /* ignore rc here */

    if (L7_NULLPTR == pAclIdent)
    {
      if (0 == vlan)
      {
        rc = dtlQosAclCreate(intf, direction, pByteTLV, tlvTotalSize);
      }
      else
      {
        rc = dtlQosSysAclCreate( direction, pByteTLV, tlvTotalSize );
      }

      if (rc == L7_SUCCESS)
      {
        pDirInfo->tlvInDevice = L7_TRUE;
        pDirInfo->tlvInstanceKey = instanceKey;

        /* update ACL correlator table for applied correlators */
        for (i = 0; i < pDirInfo->tlvCorrCount; i++)
        {
          if (aclCorrTableEntryInsert(pDirInfo->tlvCorrList[i]) != L7_SUCCESS)
          {
            L7_uint32       aclId, rulenum;
            L7_ACL_TYPE_t   aclType;
            L7_ACL_ACTION_t ruleAction;
            L7_uchar8       aclName[L7_ACL_NAME_LEN_MAX+1];

            ACL_PRT(ACL_MSGLVL_HI,
                    "\n%s: Table insert failed for correlator 0x%8.8x\n",
                    __FUNCTION__, pDirInfo->tlvCorrList[i]);

            if (aclCorrDecode(pDirInfo->tlvCorrList[i], &aclType, &aclId,
                              &rulenum, &ruleAction) == L7_SUCCESS)
            {
              if (aclType == L7_ACL_TYPE_MAC)
              {
                if (aclMacImpNameFromIndexGet(aclId, aclName) != L7_SUCCESS)
                {
                  sprintf(aclName, "MAC id %u", aclId);
                }
              }
              else
              {
                sprintf(aclName, "%u", aclId);
              }
            }
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_ACL_COMPONENT_ID,
                    "ACL \"%s\", rule %u:  This rule is not being logged."
                    " The ACL configuration has resulted in a requirement"
                    " for more logging rules than the platform supports."
                    " The specified rule is functioning normally except "
                    "for the logging action.",
                    aclName, rulenum);
          }
        }
        if(trInfo != L7_NULLPTR)
        {
          for(i = 0; i < trInfo->tlvTimeRangeCorrCount; i++)
          {
            if(aclTimeRangeTableCorrEntryAdd(trInfo->tlvTimeRangeCorrList[i]->trId,trInfo->tlvTimeRangeCorrList[i]) != L7_SUCCESS)
            {
              L7_uint32       aclId, rulenum;
              L7_ACL_TYPE_t   aclType;
              L7_ACL_ACTION_t ruleAction;
              L7_uchar8       aclName[L7_ACL_NAME_LEN_MAX+1];

              ACL_PRT(ACL_MSGLVL_HI,
                    "\n%s: Table insert failed for correlator 0x%8.8x\n",
                    __FUNCTION__, trInfo->tlvTimeRangeCorrList[i]->corrId);

              if (aclCorrDecode(trInfo->tlvTimeRangeCorrList[i]->corrId, &aclType, &aclId,
                              &rulenum, &ruleAction) == L7_SUCCESS)
              {
                if (aclType == L7_ACL_TYPE_MAC)
                {
                  if (aclMacImpNameFromIndexGet(aclId, aclName) != L7_SUCCESS)
                  {
                    sprintf(aclName, "MAC id %u", aclId);
                  }
                }
                else
                {
                  sprintf(aclName, "%u", aclId);
                }
              }
              L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_FLEX_QOS_ACL_COMPONENT_ID,
                    "ACL \"%s\", rule %u:  This rule is time based rule."
                    " The ACL configuration has resulted in a requirement"
                    " for more than maximum number of rules that the platform supports.",
                    aclName, rulenum);
           
           }
         }
       } /* end for check for Time Range */
      }
      else
      {
        aclCorrCountReset(pDirInfo);
      }

    } /* endif pAclIdent is null */

    (void)aclTlvParse(pTLV, intf, vlan, direction); /* useful for debugging */
  }

  if (L7_NULLPTR == pAclIdent)
  {
    (void)tlvDelete(tlvHandle);   /* this rc not meaningful */
  }

  /* an rc2 value other than L7_SUCCESS indicates a problem was
   * encountered earlier, so use it as the desired rc value
   */
  if (rc2 != L7_SUCCESS)
    rc = rc2;

  return rc;
}

/*********************************************************************
*
* @purpose  Builds an ACL Delete TLV and send it to DTL
*
* @param    L7_uint32   intf     interface associated with this ACL
* @param    L7_uint32   vlan     VLAN associated with this ACL
* @param    L7_uint32   aclDir   interface direction (internal)
*
* @returns  L7_SUCCESS, if the TLV was successfully build and sent
* @returns  L7_ERROR,   if TLV build error
* @returns  L7_FAILURE, if any other failure
*
* @comments Invoke multiple times for more than one interface/VLAN_ID, direction
*
* @note     Only one of the parameters intf or vlan can be valid at the same time.
*           If vlan is non-zero it is considered the valid parameter and the intf
*           parameter is ignored.
*
* @end
*
*********************************************************************/
L7_RC_t aclBuildTLVDelete(L7_uint32 intf, L7_uint32 vlan, L7_uint32 aclDir)
{
  L7_tlvHandle_t      tlvHandle = aclTlvHandle;
  L7_RC_t rc;
  aclTlvInstDelList_t aclDelList;
  L7_uint32           direction, tlvTotalSize;
  L7_tlv_t            *pTLV;
  L7_uchar8           *pByteTLV;
  aclIntfDirInfo_t    *pDirInfo;
  L7_uint32           i;
  aclIntfTRInfo_t     *trInfo = L7_NULLPTR;

  /* check ACL internal direction value */
  if (aclDir >= ACL_INTF_DIR_MAX)
    return L7_FAILURE;

  if (0 == vlan)
  {
    /* if the interface is not in one of the states where hw access is allowed
     * then we will simply return success
     */
    if (aclIsIntfAttached(intf, L7_NULLPTR) == L7_FALSE)
      return L7_SUCCESS;

    /* just return with success if the interface is currently in an acquired state */
    if (aclIsIntfAcquired(intf) == L7_TRUE)
      return L7_SUCCESS;

    /* set up ptr to intf directional information */
    pDirInfo = &pAclIntfInfo[intf].dir[aclDir];
  }
  else
  {
    if (0 == aclVlanMapTbl[vlan])
    {
      return L7_FAILURE;
    }

    pDirInfo = &pAclVlanInfo[aclVlanMapTbl[vlan]].dir[aclDir];
  }

  /* Time Range Info */
  trInfo = pDirInfo->tlvTRInfo;

  /* also return with success if flag indicates TLV not currently in device */
  if (pDirInfo->tlvInDevice != L7_TRUE)
    return L7_SUCCESS;

  /* translate ACL internal direction to API direction value for DTL */
  (void)aclDirIntToExt(aclDir, &direction);   /* ignore rc here */

  rc = tlvCreate(tlvHandle);
  if (rc == L7_ERROR)
  {
    /* not enough resources */
    return rc;
  }

  /* NOTE:  Must use the same instance key as was used for the ACL add */
  memset(&aclDelList, 0, sizeof(aclDelList));
  aclDelList.instanceKey = (L7_uint32)osapiHtonl((L7_ulong32)pDirInfo->tlvInstanceKey);
  aclDelList.keyCount = (L7_uint32)osapiHtonl(1);

  rc = tlvOpen(tlvHandle, L7_QOS_ACL_TLV_INST_DEL_LIST_TYPE, L7_QOS_ACL_TLV_INST_DEL_LIST_LEN, (L7_uchar8*)&aclDelList);

  rc = tlvClose(tlvHandle,L7_QOS_ACL_TLV_INST_DEL_LIST_TYPE);

  rc = tlvComplete(tlvHandle);
  if (rc != L7_SUCCESS)
  {
    /* open and closes dont match up */
    return rc;
  }

  rc = tlvQuery(tlvHandle, &pTLV, &tlvTotalSize);
  pByteTLV = (L7_uchar8*)pTLV;

  if (0 == vlan)
  {
    rc = dtlQosAclDelete(intf, direction, pByteTLV, tlvTotalSize);
  }
  else
  {
    rc = dtlQosSysAclDelete( direction, pByteTLV, tlvTotalSize );
  }

  if (rc == L7_SUCCESS)
  {
    pDirInfo->tlvInDevice = L7_FALSE;
    pDirInfo->tlvInstanceKey = 0;

    /* update ACL correlator table for withdrawn correlatorsi for logged ACL rules */
    for (i = 0; i < pDirInfo->tlvCorrCount; i++)
    {
      if (aclCorrTableEntryRemove(pDirInfo->tlvCorrList[i]) != L7_SUCCESS)
      {
        ACL_PRT(ACL_MSGLVL_HI,
                "\n%s: Table remove failed for correlator 0x%8.8x\n",
                __FUNCTION__, pDirInfo->tlvCorrList[i]);
      }
    }

    /* Time Based ACL - Remove the correlator node from the aclCorrEntryList 
     * trId entry in time range table
     */
    if (trInfo != L7_NULLPTR)
    {
      for (i = 0; i < trInfo->tlvTimeRangeCorrCount; i++)
      {            
        if(aclTimeRangeTableCorrEntryDelete(trInfo->tlvTimeRangeCorrList[i]->trId,trInfo->tlvTimeRangeCorrList[i]) != L7_SUCCESS)
        {
          ACL_PRT(ACL_MSGLVL_HI,
                "\n%s: Table remove failed for correlator 0x%8.8x\n",
                __FUNCTION__, trInfo->tlvTimeRangeCorrList[i]->corrId);
        }
      }
    }
    aclCorrCountReset(pDirInfo);
  }

  (void)aclTlvParse(pTLV, intf, vlan, direction); /* useful for debugging */

  (void)tlvDelete(tlvHandle); /* this rc not meaningful */

  return rc;
}

/*********************************************************************
*
* @purpose  Builds TLV entries for an IPv6 implicit deny all rule
*
* @param    L7_uint32       vlan       Specifies if TLV is being applied
*                                      to VLAN versus interface(s).  This
*                                      affects how the implicit deny rules
*                                      are created.  (vlan == 0 implies
*                                      application to interface(s); vlan != 0
*                                      implies application to vlan)
* @param    L7_tlvHandle_t  tlvHandle  TLV block handle to use
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    TLV build error
* @returns  L7_FAILURE
*
* @comments An IPv6 'deny all' rule consists of multiple classifier entries,
*           each of which contains multiple match fields.
*
* @end
*
*********************************************************************/
L7_RC_t aclTlvRuleIpv6DenyAllBuild(L7_uint32 vlan, L7_tlvHandle_t tlvHandle)
{
  L7_RC_t               rc;
  aclTlvRuleDef_t       rule;
  aclTlvMatchEtype_t    etypeRule;
  aclTlvMatchProtocol_t protRule;
  aclTlvMatchIcmpMsg_t  icmpMsgRule;
  aclTlvMatchVlanId_t   vlanIdRule;

  /* set up various data structures used to fill in TLV fields */

  memset(&rule, 0, sizeof(rule));
  rule.denyFlag = L7_FALSE;

  memset(&etypeRule, 0, sizeof(etypeRule));
  etypeRule.etypeValue1 = osapiHtons((L7_ushort16)L7_QOS_ETYPE_ID_IPV6);
  etypeRule.checkStdHdrFlag = L7_TRUE;

  memset(&protRule, 0, sizeof(protRule));
  protRule.protoNumValue = L7_ACL_PROTOCOL_ICMPV6;
  protRule.protoNumMask = L7_QOS_ACL_TLV_MATCH_PROTOCOL_MASK;

  memset(&icmpMsgRule, 0, sizeof(icmpMsgRule));
  icmpMsgRule.msgType = L7_ACL_ICMPV6_MSG_TYPE_ND_NA;
  icmpMsgRule.msgTypeMask = L7_QOS_ACL_TLV_MATCH_ICMPMSG_MASK_ON;
  icmpMsgRule.msgCode = L7_ACL_ICMPV6_MSG_CODE_ND_NA_NONE;
  icmpMsgRule.msgCodeMask = L7_QOS_ACL_TLV_MATCH_ICMPMSG_MASK_ON;


  /* if building TLV for application to a VLAN, need to build the deny rule to only apply to traffic on target VLAN */
  if (vlan != 0)
  {
    memset(&vlanIdRule, 0, sizeof(vlanIdRule));
    vlanIdRule.vidStart = osapiHtons(vlan);
    vlanIdRule.vidEnd = osapiHtons(vlan);
  }

  /* create an ICMPv6 nd-na (neighbor advertisement) permit rule */
  if ((rc=tlvOpen(tlvHandle, L7_QOS_ACL_TLV_RULE_DEF, L7_QOS_ACL_TLV_RULE_DEF_LEN,
                  (L7_uchar8*)&rule)) != L7_SUCCESS)
    return rc;
  if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE, L7_QOS_ACL_TLV_MATCH_ETYPE_LEN,
                   (L7_uchar8 *)&etypeRule)) != L7_SUCCESS)
    return rc;
  if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_PROTOCOL_TYPE, L7_QOS_ACL_TLV_MATCH_PROTOCOL_LEN,
                   (L7_uchar8 *)&protRule)) != L7_SUCCESS)
    return rc;
  if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_ICMPMSG_TYPE, L7_QOS_ACL_TLV_MATCH_ICMPMSG_LEN,
                   (L7_uchar8 *)&icmpMsgRule)) != L7_SUCCESS)
    return rc;
  if (vlan != 0)
  {
    if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID_LEN,
                     (L7_uchar8*)&vlanIdRule)) != L7_SUCCESS)
      return rc;
  }
  if ((rc=tlvClose(tlvHandle,L7_QOS_ACL_TLV_RULE_DEF)) != L7_SUCCESS)
    return rc;

  /* create an ICMPv6 nd-ns (neighbor solicitation) permit rule */
  icmpMsgRule.msgType = L7_ACL_ICMPV6_MSG_TYPE_ND_NS;
  icmpMsgRule.msgCode = L7_ACL_ICMPV6_MSG_CODE_ND_NS_NONE;
  if ((rc=tlvOpen(tlvHandle, L7_QOS_ACL_TLV_RULE_DEF, L7_QOS_ACL_TLV_RULE_DEF_LEN,
                  (L7_uchar8*)&rule)) != L7_SUCCESS)
    return rc;
  if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE, L7_QOS_ACL_TLV_MATCH_ETYPE_LEN,
                   (L7_uchar8 *)&etypeRule)) != L7_SUCCESS)
    return rc;
  if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_PROTOCOL_TYPE, L7_QOS_ACL_TLV_MATCH_PROTOCOL_LEN,
                   (L7_uchar8 *)&protRule)) != L7_SUCCESS)
    return rc;
  if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_ICMPMSG_TYPE, L7_QOS_ACL_TLV_MATCH_ICMPMSG_LEN,
                   (L7_uchar8 *)&icmpMsgRule)) != L7_SUCCESS)
    return rc;
  if (vlan != 0)
  {
    if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID_LEN, (L7_uchar8*)&vlanIdRule)) != L7_SUCCESS)
      return rc;
  }
  if ((rc=tlvClose(tlvHandle,L7_QOS_ACL_TLV_RULE_DEF)) != L7_SUCCESS)
    return rc;

  /* create an Ethertype 0x86DD deny rule */
  rule.denyFlag = L7_TRUE;
  if ((rc=tlvOpen(tlvHandle, L7_QOS_ACL_TLV_RULE_DEF, L7_QOS_ACL_TLV_RULE_DEF_LEN,
                  (L7_uchar8*)&rule)) != L7_SUCCESS)
    return rc;
  if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE, L7_QOS_ACL_TLV_MATCH_ETYPE_LEN,
                   (L7_uchar8 *)&etypeRule)) != L7_SUCCESS)
    return rc;
  if (vlan != 0)
  {
    if ((rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID_LEN, (L7_uchar8*)&vlanIdRule)) != L7_SUCCESS)
      return rc;
  }
  if ((rc=tlvClose(tlvHandle,L7_QOS_ACL_TLV_RULE_DEF)) != L7_SUCCESS)
    return rc;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Builds a set of IP ACL Rule TLVs
*
* @param    L7_uint32   aclnum              IP ACL number
* @param    L7_uint32   *ruleCount          number of rules written to TLV
* @param    aclTlvMatchEvery_t *matchEveryFlags set if a 'match every' rule used
* @param    aclIntfDirInfo_t   *pDirInfo    Ptr to intf,dir information
* @param    L7_uint32   *vlan    VLAN ID if ACL binding to VLAN
* @param    L7_tlvHandle_t  tlvHandle  TLV block handle to use
*
* @returns  L7_SUCCESS, if this ACL successfully processed
* @returns  L7_ERROR,   if problem with TLV creation
* @returns  L7_FAILURE  if any other failure
*
* @comments Does not append an implicit 'deny all' rule to the TLV.
*
* @note     If the vlan parameter is non-zero, an implied rule is generated
*           to match on that VLAN ID.
*
* @end
*
*********************************************************************/
L7_RC_t aclTlvRuleDefBuild(L7_uint32 aclnum, L7_uint32 *ruleCount,
                           aclTlvMatchEvery_t *matchEveryFlags,
                           aclIntfDirInfo_t *pDirInfo,
                           L7_uint32 vlan, L7_tlvHandle_t tlvHandle,
                           L7_uint8 *actionFlag)
{
  L7_BOOL                     needDenyAll = L7_TRUE;
  L7_BOOL                     loggingValid;
  L7_RC_t                     rc;
  L7_uint32                   rulenum;
  aclRuleParms_t              *p;
  nimUSP_t                    nimUsp;
  L7_uint32                   val;
  L7_uint32                   intIfNum;
  L7_INTF_STATES_t            intfState;
  L7_ACL_TYPE_t               aclType;
  aclTlvMatchEvery_t          bypassMask, matchEveryMask;
  L7_uint32                   cfgMask;
  L7_ushort16                 etypeId;


  aclTlvRuleDef_t             rule;
  aclTlvMatchDstIp_t          dstIp;
  aclTlvMatchDstIpv6_t        dstIpv6;
  aclTlvMatchDstL4Port_t      dstL4Port;
  aclTlvMatchIpTos_t          iptos;
  aclTlvMatchFlowlblv6_t      flowlblv6;
  aclTlvMatchProtocol_t       prot;
  aclTlvMatchSrcIp_t          srcIp;
  aclTlvMatchSrcIpv6_t        srcIpv6;
  aclTlvMatchSrcL4Port_t      srcL4Port;
  aclTlvAttrAssignQueue_t     assignQueue;
  aclTlvAttrRedirect_t        redirect;
  aclTlvAttrMirror_t          mirror;

  aclTlvMatchVlanId_t         vlanId;       /* used if ACL is being applied to VLAN ID */
  aclTlvMatchEtype_t          etypeRule;    /* used for IP 'match every' rule */

  /* Time Based ACLs variables*/
  L7_uint32                   val1;
  L7_BOOL                     timeBasedRule = L7_FALSE;               
  aclTlvRuleStatus_t          ruleStatus;
  L7_uint32                   status;
  aclTRCorrEntry_t            *timeRangeEntry;
  aclIntfTRInfo_t             *trInfo = L7_NULLPTR;

  if (ruleCount == L7_NULLPTR)
    return L7_FAILURE;
  if (pDirInfo == L7_NULLPTR)
    return L7_FAILURE;
  
  if (aclImpAclTypeGet(aclnum, &aclType) != L7_SUCCESS)
    return L7_FAILURE;
  
  trInfo = pDirInfo->tlvTRInfo;  

  /* set up some local values that differ among the IP ACL types */
  if (aclType == L7_ACL_TYPE_IPV6)
  {
    bypassMask = ACL_TLV_RULE_BYPASS_IPV6;
    cfgMask = ACL_IPV6_RULE_CFG_MASK_EVERY;
    etypeId = L7_QOS_ETYPE_ID_IPV6;
    matchEveryMask = ACL_TLV_MATCH_EVERY_IPV6;
  }
  else  /* default assumed to be numbered IP ACLs */
  {
    bypassMask = ACL_TLV_RULE_BYPASS_IP;
    cfgMask = ACL_RULE_CFG_MASK_EVERY;
    etypeId = L7_QOS_ETYPE_ID_IPV4;
    matchEveryMask = ACL_TLV_MATCH_EVERY_IP;
  }

  *ruleCount = 0;

  /* do not process any IP ACL rules if any IP or MAC 'match every' non time based rules 
   * were previously seen
   */
  if ((*matchEveryFlags & bypassMask) != 0)
    return L7_SUCCESS;

  rc = aclImpRuleGetFirst(aclnum, &rulenum);

  /* return successfully if ACL does not contain any rules (ruleCount is 0) */
  if (rc != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  while ((rc == L7_SUCCESS) && (needDenyAll == L7_TRUE))
  {
    rc=aclRuleGet(aclnum,rulenum,&p);

    /* only allow logging if supported for the rule action type */
    if (p->logging == L7_TRUE)
      loggingValid = aclImpLoggingIsAllowed(p->action);
    else
      loggingValid = L7_FALSE;

    memset((L7_char8 *)&rule, 0, sizeof(rule));
    rule.denyFlag = (L7_uchar8)p->action;

    /* PTIn Added */
    *actionFlag |= rule.denyFlag;

    if (loggingValid == L7_TRUE)
      val = aclCorrEncode(aclType, aclnum, rulenum, p->action);
    else
      val = L7_QOS_ACL_TLV_RULE_CORR_NULL;
    rule.logCorrelator = (L7_uint32)osapiHtonl((L7_ulong32)val);

    /* Time Based ACL Rule*/
    memset((L7_char8 *)&ruleStatus, 0, sizeof(ruleStatus));
    
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE) 
        && (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_TIME_RANGE_NAME) == L7_TRUE))
    { 
      timeBasedRule = L7_TRUE;
      /* if logging is valid use the same correaltor */
      if(loggingValid == L7_TRUE)          
      {
        val1 = val;        
      }
      else       
      {
        val1 = aclCorrEncode(aclType, aclnum, rulenum, p->action);       
      }       
    }
    else
    {
      /* Non Timed Based ACL Rule*/
      timeBasedRule = L7_FALSE;
      val1 = L7_QOS_ACL_TLV_RULE_CORR_NULL;      
    }
    ruleStatus.timeBasedCorrelator = (L7_uint32)osapiHtonl((L7_ulong32)val1);
    
    /* ignore empty rules that contain no classifier field */
    if ( (p->configMask & cfgMask) != 0 )
    {
      if (loggingValid == L7_TRUE)
      {
        /* remember each non-null correlator used in TLV (needed for deletion)*/
        if (pDirInfo->tlvCorrCount < L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT)
          pDirInfo->tlvCorrList[pDirInfo->tlvCorrCount++] = val;
      }

      rc=tlvOpen(tlvHandle, L7_QOS_ACL_TLV_RULE_DEF, L7_QOS_ACL_TLV_RULE_DEF_LEN, (L7_uchar8*)&rule);

      /* write the assign queue, redirect intf, and mirror intf sub-TLVs
       * regardless of whether this is a 'match every' rule or not
       * (only for 'permit' rules)
       */
      if (p->action == L7_ACL_PERMIT)
      {
        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_ASSIGN_QUEUEID) == L7_TRUE)
        {
          memset(&assignQueue, 0, sizeof(assignQueue));
          assignQueue.qid = (L7_uchar8)p->assignQueueId;
          rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_TYPE, L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_LEN, (L7_uchar8*)&assignQueue);
        }

        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_REDIRECT_INTF) == L7_TRUE)
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

        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_MIRROR_INTF) == L7_TRUE)
        {
          /* only tell HAPI if target interface is in attaching/attached state */
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
         * Force a VLAN ID rule.
         */
        vlanId.vidStart = osapiHtons(vlan);
        vlanId.vidEnd   = osapiHtons(vlan);
        rc = tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_VLANID_TYPE, L7_QOS_ACL_TLV_MATCH_VLANID_LEN, (L7_uchar8*)&vlanId );
      }

      /* always include an Ethertype 0x0800 match condition for each IP ACL
       * rule (so HAPI never needs to assume an IPv4 packet header match is
       * desired)
       */
      memset(&etypeRule, 0, sizeof(etypeRule));
      etypeRule.etypeValue1 = osapiHtons(etypeId);
      etypeRule.checkStdHdrFlag = L7_TRUE;
      rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE, L7_QOS_ACL_TLV_MATCH_ETYPE_LEN,
                  (L7_uchar8 *)&etypeRule);

      /* Timed Based ACL */
      if (timeBasedRule == L7_TRUE)
      {           
        /* get the rule status from the aclTimeRangeTable */
        if (p->timeRangeEntry != L7_NULLPTR)
        {
          timeRangeEntry = p->timeRangeEntry;
          if (aclTimeRangeTableEntryStatusGet(timeRangeEntry->trId, &status) == L7_SUCCESS)
          {
            ruleStatus.status = status;                
            timeRangeEntry->corrId = val1;
            if(trInfo != L7_NULLPTR)
            {
              if (trInfo->tlvTimeRangeCorrCount < L7_ACL_TIME_RANGE_RULE_PER_INTF_DIR_LIMIT)
              {
                trInfo->tlvTimeRangeCorrList[trInfo->tlvTimeRangeCorrCount++] = timeRangeEntry;
              }
            }
          }
          else
          {
            /*error*/
            rc = L7_FAILURE;
            break;
          }
        }
        else
        {
          /*error*/
          rc = L7_FAILURE;
          break;
        }
          
        /* Rule status is communicated to hapi for every Time Based ACL Rule and 
        * Non Time Based ACL Rule
        */
        rc= tlvWrite(tlvHandle, L7_QOS_ACL_TLV_RULE_STATUS, L7_QOS_ACL_TLV_RULE_STATUS_LEN, (L7_uchar8*)&ruleStatus);
      }/* end of timeBasedRule */
      else
      {
         /* If rule is not timed based then rule status is always considered to be active*/
         ruleStatus.status = L7_ACL_RULE_STATUS_ACTIVE;
         rc= tlvWrite(tlvHandle, L7_QOS_ACL_TLV_RULE_STATUS, L7_QOS_ACL_TLV_RULE_STATUS_LEN, (L7_uchar8*)&ruleStatus);
      } 

      if (((p->every == L7_TRUE) ||
           (aclIsTlvMaskedRuleValid(aclnum,rulenum) != L7_TRUE)) && (timeBasedRule != L7_TRUE))

      {
        /* this rule is defined to match every IP packet, regardless of
         * the contents of the other supported IP classifier fields
         */
        *matchEveryFlags |= matchEveryMask;

        /* Once a 'match every' Non Time Based Rule is written to the TLV, no subsequent
         * rules for this ACL matter, including the implicit deny all.
         * Setting needDenyAll flag to false causes loop to terminate
         * at the end of this iteration.
         */
        needDenyAll = L7_FALSE;
      }

      else
      {
        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTIP) == L7_TRUE)
        {
          /* only include field if nonzero mask is specified */
          if (p->dstIp.v4.mask != 0)
          {
            dstIp.ipAddr = (L7_uint32)osapiHtonl(p->dstIp.v4.addr);
            dstIp.ipMask = (L7_uint32)osapiHtonl(p->dstIp.v4.mask);
            rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_DSTIP_TYPE, L7_QOS_ACL_TLV_MATCH_DSTIP_LEN, (L7_uchar8*)&dstIp);
          }
        }

        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTIPV6) == L7_TRUE)
        {
          /* only include field if nonzero prefix length is specified */
          if (p->dstIp.v6.in6PrefixLen != 0)
          {
            /* NOTE: IPv6 addresses are always stored internally in network byte order */
            memcpy(dstIpv6.ipv6Addr, p->dstIp.v6.in6Addr.in6.addr8, sizeof(dstIpv6.ipv6Addr));
            (void)sysapiPrefixLenToNetMask(p->dstIp.v6.in6PrefixLen, sizeof(dstIpv6.ipv6Mask), dstIpv6.ipv6Mask);  /* ignore rc here */
            rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_DSTIPV6_TYPE, L7_QOS_ACL_TLV_MATCH_DSTIPV6_LEN, (L7_uchar8*)&dstIpv6);
          }
        }

        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTPORT) == L7_TRUE)
        {
          memset(&dstL4Port, 0, sizeof(dstL4Port));
          dstL4Port.portStart = osapiHtons(p->dstPort);
          dstL4Port.portEnd   = osapiHtons(p->dstPort);
          dstL4Port.portMask  = osapiHtons(L7_QOS_ACL_TLV_MATCH_DSTL4PORT_MASK);
          rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_DSTL4PORT_TYPE, L7_QOS_ACL_TLV_MATCH_DSTL4PORT_LEN, (L7_uchar8*)&dstL4Port);
        }

        if ( (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTSTARTPORT) == L7_TRUE) &&
             (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTENDPORT) == L7_TRUE) )
        {
          /* PTIN Added Support for ranges based on possible shared mask */
          L7_ushort16 start;
          L7_ushort16 end;
          L7_ushort16 new_start;
          L7_ushort16 new_end;
          L7_ushort16 mask;
          int i;

          memset(&dstL4Port, 0, sizeof(dstL4Port));
          dstL4Port.portStart = osapiHtons(p->dstStartPort);
          dstL4Port.portEnd   = osapiHtons(p->dstEndPort);

          start = p->dstStartPort;
          end = p->dstEndPort;

          for (i=0; i<32;i++)
          {
            if (start == end)
              break;

            start >>= 1;
            end >>= 1;
          }

          mask = L7_QOS_ACL_TLV_MATCH_DSTL4PORT_MASK & ~((1<<i)-1);

          new_start = p->dstStartPort & mask;
          new_end = new_start | (~mask);

          LOG_PT_DEBUG(LOG_CTX_MSG, "DSTL4 Range: [%d-%d] -> [%d->%d]", p->dstStartPort, p->dstEndPort, new_start, new_end);

          /* PTIN Changed: dstL4Port.portMask  = osapiHtons(L7_QOS_ACL_TLV_MATCH_DSTL4PORT_MASK); */
          dstL4Port.portMask  = osapiHtons(mask);
          rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_DSTL4PORT_TYPE, L7_QOS_ACL_TLV_MATCH_DSTL4PORT_LEN, (L7_uchar8*)&dstL4Port);
        }

        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_FLOWLBLV6) == L7_TRUE)
        {
          memset(&flowlblv6, 0, sizeof(flowlblv6));
          flowlblv6.flowLabel = (L7_uint32)osapiHtonl(p->flowlbl);
          flowlblv6.flowLabelMask  = (L7_uint32)osapiHtonl(L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_MASK);
          rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_TYPE, L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_LEN, (L7_uchar8*)&flowlblv6);
        }

        if ( (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_IPPREC) == L7_TRUE) ||
             (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_IPTOS) == L7_TRUE) ||
             (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_IPDSCP) == L7_TRUE) )
        {
          /* only include field if nonzero mask is specified */
          if (p->tosmask != 0)
          {
            memset(&iptos, 0, sizeof(iptos));
            iptos.tosValue = p->tosbyte;
            iptos.tosMask  = p->tosmask;
            rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_IPTOS_TYPE, L7_QOS_ACL_TLV_MATCH_IPTOS_LEN, (L7_uchar8*)&iptos);
          }
        }

        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_PROTOCOL) == L7_TRUE)
        {
          /* only include field if nonzero mask is specified */
          if (p->protmask != 0)
          {
            memset(&prot, 0, sizeof(prot));
            prot.protoNumValue = p->protocol;
            prot.protoNumMask  = p->protmask;
            rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_PROTOCOL_TYPE, L7_QOS_ACL_TLV_MATCH_PROTOCOL_LEN, (L7_uchar8*)&prot);
          }
        }

        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCIP) == L7_TRUE)
        {
          /* only include field if nonzero mask is specified */
          if (p->srcIp.v4.mask != 0)
          {
            srcIp.ipAddr = (L7_uint32)osapiHtonl(p->srcIp.v4.addr);
            srcIp.ipMask = (L7_uint32)osapiHtonl(p->srcIp.v4.mask);
            rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_SRCIP_TYPE, L7_QOS_ACL_TLV_MATCH_SRCIP_LEN, (L7_uchar8*)&srcIp);
          }
        }

        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCIPV6) == L7_TRUE)
        {
          /* only include field if nonzero prefix length is specified */
          if (p->srcIp.v6.in6PrefixLen != 0)
          {
            /* NOTE: IPv6 addresses are always stored internally in network byte order */
            memcpy(srcIpv6.ipv6Addr, p->srcIp.v6.in6Addr.in6.addr8, sizeof(srcIpv6.ipv6Addr));
            (void)sysapiPrefixLenToNetMask(p->srcIp.v6.in6PrefixLen, sizeof(srcIpv6.ipv6Mask), srcIpv6.ipv6Mask);  /* ignore rc here */
            rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_SRCIPV6_TYPE, L7_QOS_ACL_TLV_MATCH_SRCIPV6_LEN, (L7_uchar8*)&srcIpv6);
          }
        }

        if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCPORT) == L7_TRUE)
        {
          memset(&srcL4Port, 0, sizeof(srcL4Port));
          srcL4Port.portStart = osapiHtons(p->srcPort);
          srcL4Port.portEnd   = osapiHtons(p->srcPort);
          srcL4Port.portMask  = osapiHtons(L7_QOS_ACL_TLV_MATCH_SRCL4PORT_MASK);
          rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_SRCL4PORT_TYPE, L7_QOS_ACL_TLV_MATCH_SRCL4PORT_LEN, (L7_uchar8*)&srcL4Port);
        }

        if ( (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCSTARTPORT) == L7_TRUE) &&
             (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCENDPORT) == L7_TRUE) )
        {
          /* PTIN Added Support for ranges based on possible shared mask */
          L7_ushort16 start;
          L7_ushort16 end;
          L7_ushort16 new_start;
          L7_ushort16 new_end;
          L7_ushort16 mask;
          int i;

          memset(&srcL4Port, 0, sizeof(srcL4Port));
          srcL4Port.portStart = osapiHtons(p->srcStartPort);
          srcL4Port.portEnd   = osapiHtons(p->srcEndPort);

          start = p->srcStartPort;
          end = p->srcEndPort;

          for (i=0; i<32;i++)
          {
            if (start == end)
              break;

            start >>= 1;
            end >>= 1;
          }

          mask = L7_QOS_ACL_TLV_MATCH_SRCL4PORT_MASK & ~((1<<i)-1);

          new_start = p->srcStartPort & mask;
          new_end = new_start | (~mask);

          LOG_PT_DEBUG(LOG_CTX_MSG, "SRCL4 Range: [%d-%d] -> [%d->%d]", p->srcStartPort, p->srcEndPort, new_start, new_end);

          /* PTIN Changed: srcL4Port.portMask  = osapiHtons(L7_QOS_ACL_TLV_MATCH_SRCL4PORT_MASK); */
          srcL4Port.portMask  = osapiHtons(mask);
          rc=tlvWrite(tlvHandle, L7_QOS_ACL_TLV_MATCH_SRCL4PORT_TYPE, L7_QOS_ACL_TLV_MATCH_SRCL4PORT_LEN, (L7_uchar8*)&srcL4Port);
        }

      } /* endelse */

      rc=tlvClose(tlvHandle,L7_QOS_ACL_TLV_RULE_DEF);

      (*ruleCount)++;

    } /* endif non-empty rule */

    if (aclImpRuleGetNext(aclnum,rulenum,&rulenum) != L7_SUCCESS)
      break;                            /* no more rules for this ACL */

  } /* endwhile */

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the ACL rule is effectively non-empty after factoring
*           in a mask value, if any
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the ACL rule number
*
* @returns  L7_TRUE, if the TLV rule exists
* @returns  L7_FALSE, if the TLV rule is empty (i.e., masked value is 0)
*
* @comments The reason for this check is to prevent maskable fields
*           whose mask value is 0 from appearing in the TLV issued to the
*           device.
*
* @end
*
*********************************************************************/
L7_BOOL aclIsTlvMaskedRuleValid(L7_uint32 aclnum, L7_uint32 rulenum)
{
  void            *acl_ptr;
  aclRuleParms_t  *p;
  L7_uint32       configMask;

  if ((acl_ptr = aclFindACL(aclnum)) == L7_NULL)
    return L7_FALSE;

  if (aclRuleGet(aclnum,rulenum,&p) != L7_SUCCESS)
    return L7_FALSE;

  configMask = p->configMask;

  /* check each rule field whose mask can be influenced by user configuration
   * and if the mask is 0, clear the corresponding field bits in the local copy
   * of the configMask
   */

  if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTIP) == L7_TRUE)
  {
    if (p->dstIp.v4.mask == 0)
    {
      configMask &= ~(1 << ACL_DSTIP);
      configMask &= ~(1 << ACL_DSTIP_MASK);
    }
  }

  if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_DSTIPV6) == L7_TRUE)
  {
    if (p->dstIp.v6.in6PrefixLen == 0)
    {
      configMask &= ~(1 << ACL_DSTIPV6);
    }
  }

  if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_IPTOS) == L7_TRUE)
  {
    if (p->tosmask == 0)
    {
      configMask &= ~(1 << ACL_IPTOS);
    }
  }

  if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_PROTOCOL) == L7_TRUE)
  {
    if (p->protmask == 0)
    {
      configMask &= ~(1 << ACL_PROTOCOL);
      configMask &= ~(1 << ACL_PROTOCOL_MASK);
    }
  }

  if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCIP) == L7_TRUE)
  {
    if (p->srcIp.v4.mask == 0)
    {
      configMask &= ~(1 << ACL_SRCIP);
      configMask &= ~(1 << ACL_SRCIP_MASK);
    }
  }

  if (ACL_RULE_FIELD_IS_SET(p->configMask, ACL_SRCIPV6) == L7_TRUE)
  {
    if (p->srcIp.v6.in6PrefixLen == 0)
    {
      configMask &= ~(1 << ACL_SRCIPV6);
    }
  }

  /* check for any remaining rule fields, including 'match every' */
  if (((configMask & ACL_RULE_CFG_MASK_EVERY) == 0) &&
      ((configMask & ACL_IPV6_RULE_CFG_MASK_EVERY) == 0))
    return L7_FALSE;

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Parse and display TLV contents
*
* @param    pTlv        @{(input)} TLV start location
* @param    intIfNum    @{(input)} Internal interface number (or 0)
* @param    vlanId      @{(input)} VLAN ID (or 0)
* @param    direction   @{(input)} Interface.VLAN direction (external)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Specify the intIfNum parameter as 0 if this TLV is not
*           associated with a particular interface.  If TLV is associated
*           with a VLAN ID, this parameter is non-zero.
*
* @end
*********************************************************************/
L7_RC_t aclTlvParse(L7_tlv_t *pTlv, L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 direction)
{
  L7_uint32     msgLvlReqd;

  /* set minimum required message level for showing any output */
  msgLvlReqd = ACL_MSGLVL_MED;

  /* check for the desired message level setting up front before displaying */
  if (aclMsgLvlGet() < msgLvlReqd)
    return L7_SUCCESS;

  /* print a title line */
  if (intIfNum != 0)
  {
    ACL_PRT(msgLvlReqd,
            "\nParsing TLV at location 0x%8.8x (intf %u, dir %u):\n\n",
            (L7_uint32)pTlv, intIfNum, direction);
  }
  else if (vlanId != 0)
  {
    ACL_PRT(msgLvlReqd,
            "\nParsing TLV at location 0x%8.8x (vlan %u, dir %u):\n\n",
            (L7_uint32)pTlv, vlanId, direction);
  }
  else
  {
    ACL_PRT(msgLvlReqd,
            "\nParsing TLV at location 0x%8.8x:\n\n",
            (L7_uint32)pTlv);
  }

  /* use the TLV utility to traverse the TLV
   * (it uses our designated show function to display the TLV contents)
   */
  if (tlvParse(pTlv, aclTlvEntryDisplay) != L7_SUCCESS)
  {
    ACL_PRT(msgLvlReqd,
            "\n  >>> Error while parsing contents of TLV type=0x%8.8x\n\n",
            osapiNtohl(pTlv->type));
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display formatted content of a single TLV entry native info
*
* @param    pTlv        @{(input)}  TLV start location
* @param    nestLvl     @{(input)}  Nesting level of this TLV (0=top level)
* @param    pSize       @{(output)} Pointer to TLV entry size output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The nestLvl parameter indicates the nesting depth of this TLV
*           relative to the top-level (i.e., outermost) TLV.  A top-level
*           TLV is designated by a nestLvl of 0.
*
* @notes    The user function must provide the TOTAL size of the TLV, which
*           includes the size of the type and length fields, based on its
*           internal knowledge of the TLV type-based definition.
*
* @notes    This parser is specific to the ACL TLV definitions.  This function
*           name is passed to the TLV utility when parsing the TLV.
*
* @notes    Only displays the TLV type, length and native data.
*
* @end
*********************************************************************/
L7_RC_t aclTlvEntryDisplay(L7_tlv_t *pTlv, L7_uint32 nestLvl,
                           L7_uint32 *pEntrySize)
{
  void                    *pVal = (void *)pTlv->valueStart;
  L7_uint32               step = ACL_PARSE_MARGIN_STEP;
  L7_uchar8               spaceChar = ' ';
  L7_uint32               nativeLen;
  L7_uint32               msgLvlReqdHdr, msgLvlReqd;
  L7_uint32               indent;
  L7_uint32               tlvType, tlvLength;
  L7_uint32               val;
  L7_ushort16             val16;
  L7_uchar8               val8;
  L7_uint32               i;

  aclTlvListType_t        *acl;
  aclTlvInstDelList_t     *acldel;
  aclTlvRuleDef_t         *rule;
  aclTlvMatchCos_t        *cos;
  aclTlvMatchCos2_t       *cos2;
  aclTlvMatchDstIp_t      *dstIp;
  aclTlvMatchDstIpv6_t    *dstIpv6;
  aclTlvMatchDstL4Port_t  *dstL4Port;
  aclTlvMatchDstMac_t     *dstMac;
  aclTlvMatchEtype_t      *etype;
  aclTlvMatchFlowlblv6_t  *flowlblv6;
  aclTlvMatchIcmpMsg_t    *icmpMsg;
  aclTlvMatchIpTos_t      *iptos;
  aclTlvMatchProtocol_t   *prot;
  aclTlvMatchSrcIp_t      *srcIp;
  aclTlvMatchSrcIpv6_t    *srcIpv6;
  aclTlvMatchSrcL4Port_t  *srcL4Port;
  aclTlvMatchSrcMac_t     *srcMac;
  aclTlvMatchVlanId_t     *vlan;
  aclTlvMatchVlanId2_t    *vlan2;
  aclTlvAttrAssignQueue_t *assignQueue;
  aclTlvAttrRedirect_t    *redirect;
  aclTlvAttrMirror_t      *mirror;
  aclTlvRuleStatus_t      *ruleStatus;

  char                    *fmtStrHdr, *fmtStrVal;
  char                    *pBanner;
  char                    spaceHdr[ACL_PARSE_MARGIN_MAX+1];
  char                    spaceVal[ACL_PARSE_MARGIN_MAX+
                                   ACL_PARSE_MARGIN_STEP+1];

  /* initialize output value */
  *pEntrySize = 0;

  /* set minimum required message level for showing any output */
  msgLvlReqdHdr = ACL_MSGLVL_MED;
  msgLvlReqd = ACL_MSGLVL_LO;

  /* format string for common part of all value displays used here */
  fmtStrHdr = "%s0x%4.4x (L=%u) \'%s\'\n";
  fmtStrVal = "%s%-.30s: ";                     /* do not use '\n' here */

  /* set up the header line and value line spacer strings per current nest level
   *
   * NOTE: Filling each array with 'space' chars, then writing end-of-string
   *       char at appropriate termination position.
   */
  indent = (nestLvl+1) * step;          /* initial indent for this nest level */
  if (indent > ACL_PARSE_MARGIN_MAX)
    indent = ACL_PARSE_MARGIN_MAX;
  memset(spaceHdr, spaceChar, sizeof(spaceHdr));
  spaceHdr[indent] = '\0';
  memset(spaceVal, spaceChar, sizeof(spaceVal));
  spaceVal[indent+step] = '\0';

  tlvType = osapiNtohl(pTlv->type);
  tlvLength = osapiNtohl(pTlv->length);

  switch (tlvType)
  {

  /*----------------------------------*/
  /* QoS ACL Functional Category TLVs */
  /*----------------------------------*/

  case L7_QOS_ACL_TLV_LIST_TYPE:
    {
      acl = (aclTlvListType_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_LIST_TYPE_LEN;
      pBanner = "QoS ACL Instance List";
      ACL_PRT(msgLvlReqdHdr, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "instance key";
      val = (L7_uint32)osapiNtohl(acl->instanceKey);
      ACL_PRT(msgLvlReqdHdr, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqdHdr, "0x%8.8x\n", val);
    }
    break;

  case L7_QOS_ACL_TLV_INST_DEL_LIST_TYPE:
    {
      L7_uint32   keyCount;

      acldel = (aclTlvInstDelList_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_INST_DEL_LIST_LEN;
      pBanner = "QoS ACL Instance Deletion List";
      ACL_PRT(msgLvlReqdHdr, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "key count";
      keyCount = (L7_uint32)osapiNtohl(acldel->keyCount);
      ACL_PRT(msgLvlReqdHdr, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqdHdr, "%u\n", keyCount);

      pBanner = "instance key";
      val = (L7_uint32)osapiNtohl(acldel->instanceKey);
      ACL_PRT(msgLvlReqdHdr, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqdHdr, "0x%8.8x\n", val);
    }
    break;


  /*-----------------------------*/
  /* ACL Rule Specification TLVs */
  /*-----------------------------*/

  case L7_QOS_ACL_TLV_RULE_DEF:
    {
      rule = (aclTlvRuleDef_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_RULE_DEF_LEN;
      pBanner = "ACL Rule Definition";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "deny flag     ";
      val8 = rule->denyFlag;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)        ";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(rule->rsvd1); i++)
      {
        val8 = rule->rsvd1[i];
        ACL_PRT(msgLvlReqd, "%2.2x", val8);
      }
      ACL_PRT(msgLvlReqd, "\n");

      pBanner = "log correlator";
      val = (L7_uint32)osapiNtohl(rule->logCorrelator);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%8.8x\n", val);
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_COS_TYPE:
    {
      cos = (aclTlvMatchCos_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_COS_LEN;
      pBanner = "CoS match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "cos value";
      val8 = cos->cosValue;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)    ";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(cos->rsvd1); i++)
      {
        val8 = cos->rsvd1[i];
        ACL_PRT(msgLvlReqd, "%2.2x", val8);
      }
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_COS2_TYPE:
    {
      cos2 = (aclTlvMatchCos2_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_COS2_LEN;
      pBanner = "Secondary CoS match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "cos2 value";
      val8 = cos2->cosValue;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)    ";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(cos2->rsvd1); i++)
      {
        val8 = cos2->rsvd1[i];
        ACL_PRT(msgLvlReqd, "%2.2x", val8);
      }
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_DSTIP_TYPE:
    {
      dstIp = (aclTlvMatchDstIp_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_DSTIP_LEN;
      pBanner = "Dst IP Address match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "ip addr";
      val = (L7_uint32)osapiNtohl(dstIp->ipAddr);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%8.8x\n", val);

      pBanner = "ip mask";
      val = (L7_uint32)osapiNtohl(dstIp->ipMask);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%8.8x\n", val);
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_DSTIPV6_TYPE:
    {
      dstIpv6 = (aclTlvMatchDstIpv6_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_DSTIPV6_LEN;
      pBanner = "Dst IPv6 Address match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "ipv6 addr";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      aclIpv6AddrPrint(msgLvlReqd, dstIpv6->ipv6Addr);
      ACL_PRT(msgLvlReqd, "\n");

      pBanner = "ipv6 mask";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      aclIpv6AddrPrint(msgLvlReqd, dstIpv6->ipv6Mask);
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_DSTL4PORT_TYPE:
    {
      dstL4Port = (aclTlvMatchDstL4Port_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_DSTL4PORT_LEN;
      pBanner = "Dst L4 Port Range match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "port start";
      val16 = osapiNtohs(dstL4Port->portStart);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x (%u)\n", val16, val16);

      pBanner = "port end  ";
      val16 = osapiNtohs(dstL4Port->portEnd);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x (%u)\n", val16, val16);

      pBanner = "port mask ";
      val16 = osapiNtohs(dstL4Port->portMask);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x\n", val16);

      pBanner = "(rsvd)    ";
      val16 = osapiNtohs(dstL4Port->rsvd1);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x\n", val16);
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_DSTMAC_TYPE:
    {
      dstMac = (aclTlvMatchDstMac_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_DSTMAC_LEN;
      pBanner = "Destination MAC match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "mac addr";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      aclMacAddrPrint(dstMac->macAddr, msgLvlReqd);
      ACL_PRT(msgLvlReqd, "\n");

      pBanner = "mac mask";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      aclMacAddrPrint(dstMac->macMask, msgLvlReqd);
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE:
    {
      etype = (aclTlvMatchEtype_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_ETYPE_LEN;
      pBanner = "Ethertype match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "etype value1   ";
      val16 = osapiNtohs(etype->etypeValue1);
      ACL_PRT(msgLvlReqd,
                    fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x\n", val16);

      pBanner = "etype value2   ";
      val16 = osapiNtohs(etype->etypeValue2);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x\n", val16);

      pBanner = "checkStdHdrFlag";
      val8 = etype->checkStdHdrFlag;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)         ";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(etype->rsvd1); i++)
      {
        val8 = etype->rsvd1[i];
        ACL_PRT(msgLvlReqd, "%2.2x", val8);
      }
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_EVERY_TYPE:
    {
      nativeLen = L7_QOS_ACL_TLV_MATCH_EVERY_LEN;
      pBanner = "\"Match Every\" match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_TYPE:
    {
      flowlblv6 = (aclTlvMatchFlowlblv6_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_LEN;
      pBanner = "IPv6 Flow Label match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "flow label value";
      val = (L7_uint32)osapiNtohl(flowlblv6->flowLabel);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%8.8x (%u)\n", val, val);

      pBanner = "flow label mask ";
      val = (L7_uint32)osapiNtohl(flowlblv6->flowLabelMask);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%8.8x\n", val);
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_ICMPMSG_TYPE:
    {
      icmpMsg = (aclTlvMatchIcmpMsg_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_ICMPMSG_LEN;
      pBanner = "ICMP Message match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "msg type value";
      val8 = icmpMsg->msgType;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%2.2x (%u)\n", val8, val8);

      pBanner = "msg type mask ";
      val8 = icmpMsg->msgTypeMask;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%2.2x\n", val8);

      pBanner = "msg code value";
      val8 = icmpMsg->msgCode;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%2.2x (%u)\n", val8, val8);

      pBanner = "msg code mask ";
      val8 = icmpMsg->msgCodeMask;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%2.2x\n", val8);
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_IPTOS_TYPE:
    {
      iptos = (aclTlvMatchIpTos_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_IPTOS_LEN;
      pBanner = "IP Type of Service (ToS) match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "tos value";
      val8 = iptos->tosValue;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%2.2x", val8);
      /* check if the TOS mask is one of the recognized values and display a
       * decimal equivalent for the DSCP or Precedence value
       */
      if (iptos->tosMask == L7_QOS_ACL_TLV_MATCH_IPDSCP_MASK)
      {
        val = (L7_uint32)val8 >> L7_QOS_ACL_TLV_MATCH_IPDSCP_SHIFT;
        ACL_PRT(msgLvlReqd, " (dscp %u)", val);
      }
      else if (iptos->tosMask == L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_MASK)
      {
        val = (L7_uint32)val8 >> L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_SHIFT;
        ACL_PRT(msgLvlReqd, " (prec %u)", val);
      }
      ACL_PRT(msgLvlReqd, "\n");

      pBanner = "tos mask ";
      val8 = iptos->tosMask;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%2.2x\n", val8);

      pBanner = "(rsvd)   ";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(iptos->rsvd1); i++)
      {
        val8 = iptos->rsvd1[i];
        ACL_PRT(msgLvlReqd, "%2.2x", val8);
      }
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_PROTOCOL_TYPE:
    {
      prot = (aclTlvMatchProtocol_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_PROTOCOL_LEN;
      pBanner = "Protocol Number match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "proto num value";
      val8 = prot->protoNumValue;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%2.2x (%u)\n", val8, val8);

      pBanner = "proto num mask ";
      val8 = prot->protoNumMask;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%2.2x\n", val8);

      pBanner = "(rsvd)         ";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(prot->rsvd1); i++)
      {
        val8 = prot->rsvd1[i];
        ACL_PRT(msgLvlReqd, "%2.2x", val8);
      }
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_SRCIP_TYPE:
    {
      srcIp = (aclTlvMatchSrcIp_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_SRCIP_LEN;
      pBanner = "Src IP Address match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "ip addr";
      val = (L7_uint32)osapiNtohl(srcIp->ipAddr);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%8.8x\n", val);

      pBanner = "ip mask";
      val = (L7_uint32)osapiNtohl(srcIp->ipMask);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%8.8x\n", val);
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_SRCIPV6_TYPE:
    {
      srcIpv6 = (aclTlvMatchSrcIpv6_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_SRCIPV6_LEN;
      pBanner = "Src IPv6 Address match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "ipv6 addr";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      aclIpv6AddrPrint(msgLvlReqd, srcIpv6->ipv6Addr);
      ACL_PRT(msgLvlReqd, "\n");

      pBanner = "ipv6 mask";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      aclIpv6AddrPrint(msgLvlReqd, srcIpv6->ipv6Mask);
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_SRCL4PORT_TYPE:
    {
      srcL4Port = (aclTlvMatchSrcL4Port_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_SRCL4PORT_LEN;
      pBanner = "Src L4 Port Range match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "port start";
      val16 = osapiNtohs(srcL4Port->portStart);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x (%u)\n", val16, val16);

      pBanner = "port end  ";
      val16 = osapiNtohs(srcL4Port->portEnd);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x (%u)\n", val16, val16);

      pBanner = "port mask ";
      val16 = osapiNtohs(srcL4Port->portMask);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x\n", val16);

      pBanner = "(rsvd)    ";
      val16 = osapiNtohs(srcL4Port->rsvd1);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%4.4x\n", val16);
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_SRCMAC_TYPE:
    {
      srcMac = (aclTlvMatchSrcMac_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_SRCMAC_LEN;
      pBanner = "Source MAC match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "mac addr";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      aclMacAddrPrint(srcMac->macAddr, msgLvlReqd);
      ACL_PRT(msgLvlReqd, "\n");

      pBanner = "mac mask";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      aclMacAddrPrint(srcMac->macMask, msgLvlReqd);
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_VLANID_TYPE:
    {
      vlan = (aclTlvMatchVlanId_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_VLANID_LEN;
      pBanner = "VLAN ID match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "vid start";
      val16 = osapiNtohs(vlan->vidStart);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val16);

      pBanner = "vid end  ";
      val16 = osapiNtohs(vlan->vidEnd);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val16);
    }
    break;

  case L7_QOS_ACL_TLV_MATCH_VLANID2_TYPE:
    {
      vlan2 = (aclTlvMatchVlanId2_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_MATCH_VLANID2_LEN;
      pBanner = "Secondary VLAN ID match";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "vid2 start";
      val16 = osapiNtohs(vlan2->vidStart);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val16);

      pBanner = "vid2 end  ";
      val16 = osapiNtohs(vlan2->vidEnd);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val16);
    }
    break;

  case L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_TYPE:
    {
      assignQueue = (aclTlvAttrAssignQueue_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_LEN;
      pBanner = "Assign Queue attr";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "queue id";
      val8 = assignQueue->qid;
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)  ";
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(assignQueue->rsvd1); i++)
      {
        val8 = assignQueue->rsvd1[i];
        ACL_PRT(msgLvlReqd, "%2.2x", val8);
      }
      ACL_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_ACL_TLV_ATTR_REDIRECT_TYPE:
    {
      redirect = (aclTlvAttrRedirect_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_ATTR_REDIRECT_LEN;
      pBanner = "Redirect attr";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "intf unit";
      val = (L7_uint32)osapiHtonl(redirect->intfUnit);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val);

      pBanner = "intf slot";
      val = (L7_uint32)osapiHtonl(redirect->intfSlot);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val);

      pBanner = "intf port";
      val = (L7_uint32)osapiHtonl(redirect->intfPort);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val);
    }
    break;

  case L7_QOS_ACL_TLV_ATTR_MIRROR_TYPE:
    {
      mirror = (aclTlvAttrMirror_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_ATTR_MIRROR_LEN;
      pBanner = "Mirror attr";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);

      pBanner = "intf unit";
      val = (L7_uint32)osapiHtonl(mirror->intfUnit);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val);

      pBanner = "intf slot";
      val = (L7_uint32)osapiHtonl(mirror->intfSlot);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val);

      pBanner = "intf port";
      val = (L7_uint32)osapiHtonl(mirror->intfPort);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val);
    }
    break;
    case L7_QOS_ACL_TLV_RULE_STATUS:
    {
      ruleStatus = (aclTlvRuleStatus_t *)pVal;
      nativeLen = L7_QOS_ACL_TLV_RULE_STATUS_LEN;
      pBanner = "Time based ACL Rule Status ";
      ACL_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, pBanner);
      
      pBanner = "Status";
      val = (L7_uint32)osapiNtohl(ruleStatus->status);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "%u\n", val);

      pBanner = "Time Range correlator";
      val = (L7_uint32)osapiNtohl(ruleStatus->timeBasedCorrelator);
      ACL_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      ACL_PRT(msgLvlReqd, "0x%8.8x\n", val);
    }
    break;


  default:
    ACL_PRT(msgLvlReqd, "\n%s>>> Invalid TLV type: 0x%8.8x <<<\n\n", 
                  spaceHdr, tlvType);
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  *pEntrySize = (L7_uint32)L7_TLV_HEADER_SIZE + nativeLen;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Apply collateral config related to this interface
*
* @param    L7_uint32  intIfNum  internal interface number
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
L7_RC_t aclImpCollateralConfigApply(L7_uint32 intIfNum)
{
  avlTree_t           *pTree = &aclTree;
  L7_uint32           aclnum;
  aclStructure_t      *pAcl;
  aclRuleParms_t      *pRule;
  L7_RC_t             rc;
  L7_uint32           ruleIntIfNum;

  /* cycle through all existing ACLs looking for any that refer to
   * the specified intIfNum as the target of a redirect or mirror
   * rule attribute
   */
  aclnum = 0;                                   /* start with first entry */
  while ((pAcl = avlSearchLVL7(pTree, &aclnum, AVL_NEXT)) != L7_NULLPTR)
  {
    aclnum = pAcl->aclNum;
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
      if (ACL_RULE_FIELD_IS_SET(pRule->configMask, ACL_REDIRECT_INTF) == L7_TRUE)
      {
        rc = nimIntIfFromConfigIDGet(&pRule->redirectConfigId, &ruleIntIfNum);
      }
      else if (ACL_RULE_FIELD_IS_SET(pRule->configMask, ACL_MIRROR_INTF) == L7_TRUE)
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
                "ACL: Found collateral IP ACL index %u mirr/redir to intf %u\n",
                aclnum, intIfNum);

        /* update the ACL in the device for all interfaces except the one
         * specified here (it is handled separately as part of the event
         * processing for that interface)
         */
        if (aclUpdateExceptIntf(aclnum, ACL_MODIFY, intIfNum) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "ACL %u: Error updating device with latest configuration\n",
                  aclnum);
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
* @purpose  Get the first access list rule given an ACL ID
*
* @param    L7_uint32   aclnum   the access-list identifying number
* @param    L7_uint32   *rule    the next rule ID
*
* @returns  L7_SUCCESS, if the first rule for this ACL ID was found
* @returns  L7_FAILURE,
* @returns  L7_ERROR,   if no rules have been created for this ACL
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpRuleGetFirst(L7_uint32 aclnum, L7_uint32 *rule)
{
  void *p;
  aclStructure_t *acl_ptr;

  p = aclFindACL(aclnum);
  if (p == L7_NULL)
  {
    return L7_ERROR;
  }

  acl_ptr = (aclStructure_t*)p;

  if (acl_ptr->head->last == L7_TRUE)
  {
    return L7_ERROR;
  }

  *rule = acl_ptr->head->ruleNum;

  return L7_SUCCESS;
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
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpRuleGetNext(L7_uint32 aclnum, L7_uint32 rule, L7_uint32 *next)
{
  void *p;
  aclStructure_t *acl_ptr;
  aclRuleParms_t *acl_rp;
  L7_RC_t rc = L7_FAILURE;

  if (rule >= L7_ACL_MAX_RULE_NUM)
  {
    return L7_FAILURE;
  }

  p = aclFindACL(aclnum);
  if (p == L7_NULL)
  {
    return L7_ERROR;
  }

  acl_ptr = (aclStructure_t*)p;

  acl_rp = acl_ptr->head; /* rule 1 */

  if (acl_rp->last == L7_TRUE)
  {
    return L7_FAILURE; /* no rules */
  }

  /* last always exists and is rule #11 */
  while (acl_rp->last != L7_TRUE)
  {
    if (acl_rp->ruleNum > rule)
    {
      *next = acl_rp->ruleNum;
      rc = L7_SUCCESS;
      break;
    }

    acl_rp = acl_rp->next;
  }

  return rc;
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
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_BOOL aclImpIsFieldConfigured(L7_uint32 aclnum, L7_uint32 rulenum,
                                L7_uint32 field)
{
  L7_BOOL isConf;
  void *acl_ptr;

  /* check validity of aclnum, rulenum, and protocol type */
  if ( (aclImpNumRangeCheck(aclnum) != L7_SUCCESS) ||
       (aclImpRuleNumRangeCheck(aclnum, rulenum) != L7_SUCCESS) )
  {
    return L7_FALSE;
  }

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULL)
  {
    return L7_FALSE;
  }

  isConf = aclIsRuleFieldConfigured(acl_ptr,rulenum,field);
  return isConf;
}

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           interface and direction
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid interface, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the interface in this
*                       direction
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
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
L7_RC_t aclImpIntfDirAclListGet(L7_uint32 intIfNum, L7_uint32 aclDir,
                                L7_ACL_INTF_DIR_LIST_t *listInfo)
{
  L7_RC_t           rc = L7_ERROR;
  aclIntfCfgData_t  *pCfg;
  aclIntfParms_t    *pList;
  L7_uint32         count, i;

  if (ACL_IS_READY == L7_FALSE)
    return L7_FAILURE;  /* no intf exist */

  /* check validity of intIfNum and direction */
  if ((nimCheckIfNumber(intIfNum) != L7_SUCCESS) ||
      (aclDir >= ACL_INTF_DIR_MAX))
  {
    return L7_FAILURE;
  }

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* set up working ptr to assign list for this interface direction */
  pList = pCfg->intf_dir_Tb[aclDir].assignList;

  memset(listInfo, 0, sizeof(*listInfo));
  count = 0;

  for (i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++, pList++)
  {
    if (pList->inUse != L7_TRUE)
      break;

    listInfo->listEntry[count].seqNum = pList->seqNum;
    listInfo->listEntry[count].aclType = pList->id.aclType;
    listInfo->listEntry[count].aclId = pList->id.aclId;
    count++;

  } /* endfor */

  listInfo->count = count;

  if (count > 0)
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  Gets a list of interfaces to which an ACL is assigned in the
*           specified direction
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  aclDir      the interface direction (internal)
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
L7_RC_t aclImpAssignedIntfDirListGet(L7_uint32 aclnum, L7_uint32 aclDir,
                                     L7_ACL_ASSIGNED_INTF_LIST_t *intfList)
{
  L7_RC_t rc = L7_ERROR;
  L7_uint32 intIfNum, cfgIndex;
  aclIntfCfgData_t  *pCfg;
  nimConfigID_t configIdNull;
  L7_uint32 count, i;
  aclIntfParms_t *pList;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (!(ACL_IS_READY))
    return L7_FAILURE; /* no intf exist */

  /* check validity of aclnum */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;

  /* check ACL internal direction value */
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

      if ((pList->id.aclType == L7_ACL_TYPE_IP) ||
          (pList->id.aclType == L7_ACL_TYPE_IPV6))
      {
        if (pList->id.aclId == aclnum)
        {
          intfList->intIfNum[count++] = intIfNum;
          break;
        }
      }
    } /* endfor i */
  } /* endfor cfgIndex */

  intfList->count = count;

  if (count > 0)
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  aclDir     the interface direction (internal)
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
L7_RC_t aclImpInterfaceDirectionAdd(L7_uint32 intIfNum, L7_uint32 aclDir,
                                    L7_uint32 aclnum, L7_uint32 seqNum)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclIntfCfgData_t *pCfg;
  L7_ACL_TYPE_t aclType;

  /* check inputs */
  if ( (nimCheckIfNumber(intIfNum) != L7_SUCCESS) ||
       (aclDir >= ACL_INTF_DIR_MAX) ||
       (aclImpNumRangeCheck(aclnum) != L7_SUCCESS) ||
       (seqNum > L7_ACL_MAX_INTF_SEQ_NUM) )
  {
    return L7_FAILURE;
  }

  /* check for supported interface type */
  if (aclIsValidIntf(intIfNum) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  /* Need to invoke this function before invoking aclImpIsInterfaceInUse().
     aclMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE is a pre-condition
     for aclImpIsInterfaceInUse() */
  if(aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (aclImpAclTypeGet(aclnum, &aclType) != L7_SUCCESS)
    return L7_FAILURE;

  /* make sure this ACL type is compatible with any other type already applied */
  if (aclImpIntfAclTypeCompatCheck(intIfNum, aclDir, aclType) != L7_SUCCESS)
    return L7_NOT_SUPPORTED;

  /* update intf,dir assign list with this ACL and notify driver of change */
  rc = aclImpIntfDirAdd(intIfNum, aclDir, aclType, aclnum, seqNum);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Checks if ACL type is compatible for interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
*
* @returns  L7_SUCCESS        ACL type combination allowed
* @returns  L7_NOT_SUPPORTED  ACL type combination not allowed
* @returns  L7_FAILURE        invalid parameter, or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Function compares ACL types already assigned to this interface
*           and direction with this ACL against the feature support.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfAclTypeCompatCheck(L7_uint32 intIfNum, L7_uint32 aclDir,
                                     L7_ACL_TYPE_t aclType)
{
  L7_uint32               compId = L7_FLEX_QOS_ACL_COMPONENT_ID;
  L7_RC_t                 rc;
  L7_uint32               i;
  L7_BOOL                 hasMac, hasIpv4, hasIpv6;
  L7_BOOL                 suppInboundIpv4Mac, suppInboundIpv6Mac, suppInboundIpv6Ipv4;
  L7_BOOL                 suppOutboundIpv4Mac, suppOutboundIpv6Mac, suppOutboundIpv6Ipv4;
  L7_ACL_TYPE_t           localType;
  L7_ACL_INTF_DIR_LIST_t  listInfo;

  /* assumes inputs validated by caller */

  /* init all flags */
  hasMac = hasIpv4 = hasIpv6 = L7_FALSE;
  suppInboundIpv4Mac  = cnfgrIsFeaturePresent(compId, L7_ACL_INTF_MIXED_INBOUND_TYPE_ACL_FEATURE_ID);
  suppInboundIpv6Mac  = cnfgrIsFeaturePresent(compId, L7_ACL_INTF_MIXED_INBOUND_IPV6_MAC_FEATURE_ID);
  suppInboundIpv6Ipv4 = cnfgrIsFeaturePresent(compId, L7_ACL_INTF_MIXED_INBOUND_IPV6_IPV4_FEATURE_ID);
  suppOutboundIpv4Mac  = cnfgrIsFeaturePresent(compId, L7_ACL_INTF_MIXED_OUTBOUND_TYPE_ACL_FEATURE_ID);
  suppOutboundIpv6Mac  = cnfgrIsFeaturePresent(compId, L7_ACL_INTF_MIXED_OUTBOUND_IPV6_MAC_FEATURE_ID);
  suppOutboundIpv6Ipv4 = cnfgrIsFeaturePresent(compId, L7_ACL_INTF_MIXED_OUTBOUND_IPV6_IPV4_FEATURE_ID);

  rc = aclImpIntfDirAclListGet(intIfNum, aclDir, &listInfo);
  if (rc != L7_SUCCESS)
  {
    /* an L7_ERROR means intf,dir list currently empty, so return L7_SUCCESS */
    return (rc == L7_ERROR) ? L7_SUCCESS : L7_FAILURE;
  }

  /* scan the intf,dir list and make note of assigned ACL types
   *
   * loop one extra time to factor in requested type for comparisons below
   * (must not index listEntry array during this extra pass)
   */
  for (i = 0; i <= listInfo.count; i++)
  {
    if (i == listInfo.count)
      localType = aclType;              /* extra pass: use parm value */
    else
      localType = listInfo.listEntry[i].aclType;

    switch (localType)
    {
    case L7_ACL_TYPE_IP:
      hasIpv4 = L7_TRUE;
      break;
    case L7_ACL_TYPE_MAC:
      hasMac = L7_TRUE;
      break;
    case L7_ACL_TYPE_IPV6:
      hasIpv6 = L7_TRUE;
      break;
    default:
      break;
    }
  }

  /* check assigned ACL type combos against feature support */
  rc = L7_SUCCESS;
  switch (aclDir)
  {
  case L7_INBOUND_ACL:
    if (suppInboundIpv4Mac == L7_FALSE)
    {
      if ((hasIpv4 == L7_TRUE) && (hasMac == L7_TRUE))
        rc = L7_NOT_SUPPORTED;
    }
    if (suppInboundIpv6Mac == L7_FALSE)
    {
      if ((hasIpv6 == L7_TRUE) && (hasMac == L7_TRUE))
        rc = L7_NOT_SUPPORTED;
    }
    if (suppInboundIpv6Ipv4 == L7_FALSE)
    {
      if ((hasIpv6 == L7_TRUE) && (hasIpv4 == L7_TRUE))
        rc = L7_NOT_SUPPORTED;
    }
    break;
  case L7_OUTBOUND_ACL:
    if (suppOutboundIpv4Mac == L7_FALSE)
    {
    if ((hasIpv4 == L7_TRUE) && (hasMac == L7_TRUE))
      rc = L7_NOT_SUPPORTED;
  }
    if (suppOutboundIpv6Mac == L7_FALSE)
    {
    if ((hasIpv6 == L7_TRUE) && (hasMac == L7_TRUE))
      rc = L7_NOT_SUPPORTED;
  }
    if (suppOutboundIpv6Ipv4 == L7_FALSE)
    {
    if ((hasIpv6 == L7_TRUE) && (hasIpv4 == L7_TRUE))
      rc = L7_NOT_SUPPORTED;
  }
    break;
  default:
    rc = L7_NOT_SUPPORTED;
    break;
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
* @param    aclType     @b{(input)} access list type
* @param    aclId       @b{(input)} access list identifier
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
L7_RC_t aclImpIntfDirAdd(L7_uint32 intIfNum, L7_uint32 aclDir,
                         L7_ACL_TYPE_t aclType, L7_uint32 aclId,
                         L7_uint32 seqNum)
{
  L7_BOOL           aclExists = L7_FALSE;
  L7_RC_t           rc;
  aclIntfCfgData_t  *pCfg;
  L7_uint32         listIndex, listIndexNew;
  aclIntfParms_t    *pList;

  /* assumes inputs validated by caller */

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (aclImpIntfDirAclIdFind(intIfNum, aclDir, aclType, aclId,
                             &listIndex) == L7_SUCCESS)
  {
    aclExists = L7_TRUE;

    pList = pCfg->intf_dir_Tb[aclDir].assignList;

    /* ACL already assigned to this intf,dir.  If sequence number is
     * the same, there is no change to the ACL ordering so treat this
     * request as a no-op.  A different sequence number requires removing
     * the ACL from the assign list and re-adding it in the proper position.
     */
    if (pList[listIndex].seqNum == seqNum)
    {
      return L7_SUCCESS;
    }

    /* if new sequence number is 0 and this ACL is already the last (or only)
     * element in the list, this is treated as a no-op
     */
    if (seqNum == L7_ACL_AUTO_INCR_INTF_SEQ_NUM)
    {
      /* this ACL is last in the list if it is at the end of the
       * array, or if the next array element is not in use
       */
      if ((listIndex == (L7_ACL_MAX_LISTS_PER_INTF_DIR-1)) ||
          (pList[listIndex+1].inUse != L7_TRUE))
      {
        return L7_SUCCESS;
      }
    }

    /* update app */
    rc = aclIntfDirListRemove(intIfNum, aclDir, aclType, aclId);
    if (rc != L7_SUCCESS)
    {
      return rc;
    }

    /* NOTE: Driver update below will handle removing old ACL rules */
  }

  /* update app */
  rc = aclIntfDirListAdd(intIfNum, aclDir, aclType, aclId, seqNum);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  /* if ACL was already assigned to this intf,dir and its new list position
   * has not changed, there is no need to update the driver
   */
  if (aclExists == L7_TRUE)
  {
    if (aclImpIntfDirAclIdFind(intIfNum, aclDir, aclType, aclId,
                               &listIndexNew) == L7_SUCCESS)
    {
      if (listIndexNew == listIndex)
      {
        return L7_SUCCESS;
      }
    }
  }

  /* update driver
   *
   * NOTE:  Underlying code will delete existing rules from device before
   *        adding new set of rules.
   */
  rc = aclIntfUpdate(intIfNum, aclDir, ACL_INTF_ADD);

  /* if driver failed */
  if (rc != L7_SUCCESS)
  {
    /* rollback */
    (void)aclIntfDirListRemove(intIfNum, aclDir, aclType, aclId);   /* this rc is ignored */
    /* try to restore hardware to configuration before attempted add */
    /* check if any ACLs remain assigned to this intf,dir to issue new 
     * command to the device
     */
    if (aclImpIsInterfaceInUse(intIfNum, aclDir) == L7_TRUE)
    {
      (void)aclIntfUpdate(intIfNum, aclDir, ACL_INTF_ADD);
    }
    return L7_REQUEST_DENIED;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To remove an access list from the specified interface and direction.
*
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  aclDir     the interface direction (internal)
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if intIfNum is removed from the access-list
* @returns  L7_FAILURE, if invalid identifying number or
*                       internal interface number does not exist
* @returns  L7_ERROR, if access-list not found for this intf, dir
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpInterfaceDirectionRemove(L7_uint32 intIfNum, L7_uint32 aclDir,
                                       L7_uint32 aclnum)
{
  L7_RC_t rc;
  void *acl_ptr;
  aclIntfCfgData_t *pCfg;
  L7_ACL_TYPE_t aclType;

  /* check validity of the access-list identifying number, intIfNum, and direction */
  if ( (aclImpNumRangeCheck(aclnum) != L7_SUCCESS) ||
       (nimCheckIfNumber(intIfNum) != L7_SUCCESS) ||
       (aclDir >= ACL_INTF_DIR_MAX) )
  {
    return L7_FAILURE;
  }

  /* check for supported interface type */
  if (aclIsValidIntf(intIfNum) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  acl_ptr = aclFindACL(aclnum);
  if (acl_ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  /* Need to invoke this function before invoking aclImpIsInterfaceInUse().
     aclMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE is a pre-condition
     for aclImpIsInterfaceInUse() */
  if(aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

  if (aclImpAclTypeGet(aclnum, &aclType) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclImpIntfDirRemove(intIfNum, aclDir, aclType, aclnum);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To remove an access list from the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
* @param    aclId       @b{(input)} access list identifier
*
* @returns  L7_SUCCESS  ACL successfully removed from intf,dir
* @returns  L7_FAILURE  invalid ACL identifier, or interface does not exist
* @returns  L7_ERROR    ACL does not exist for this id, or not assigned
*                         to the intf,dir
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirRemove(L7_uint32 intIfNum, L7_uint32 aclDir,
                            L7_ACL_TYPE_t aclType, L7_uint32 aclId)
{
  L7_RC_t           rc;

  /* assumes inputs validated by caller */

  /* update app */
  rc = aclIntfDirListRemove(intIfNum, aclDir, aclType, aclId);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  /* update driver */
  rc = aclIntfUpdate(intIfNum, aclDir, ACL_INTF_REMOVE);
  if (rc != L7_SUCCESS)
  {
    return L7_REQUEST_DENIED;
  }

  /* check if any ACLs remain assigned to this intf,dir to issue new
   * command to the device
   */
  if (aclImpIsInterfaceInUse(intIfNum, aclDir) == L7_TRUE)
  {
    if (aclIntfUpdate(intIfNum, aclDir, ACL_INTF_ADD) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "Removed %s ACL %u from intf %s %s, but unable to "
              "apply remaining ACLs to device\n",
              acl_type_str[aclType], aclId, ifName,
              acl_direction_str[aclDir]);

      return L7_REQUEST_DENIED;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To check if an interface and direction is in use by any access list.
*
* @param    L7_uint32  intIfNum  the internal interface number
* @param    L7_uint32  aclDir    the interface direction (internal)
*
* @returns  L7_TRUE, if the intf,dir has at least one ACL attached
* @returns  L7_FALSE, if the intf,dir has no ACL attached
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The function assumes the interface, intIfNum is configurable
*           i.e. aclMapIntfIsConfigurable() returns L7_TRUE
* @end
*
*********************************************************************/
L7_BOOL aclImpIsInterfaceInUse(L7_uint32 intIfNum, L7_uint32 aclDir)
{
    L7_BOOL          rc = L7_FALSE;
    aclIntfCfgData_t *pCfg;
    aclIntfParms_t   *pList;

    if (aclDir >= ACL_INTF_DIR_MAX)
      return L7_FALSE;

    (void)aclMapIntfIsConfigurable(intIfNum, &pCfg);

    pList = pCfg->intf_dir_Tb[aclDir].assignList;

    /* only need to check the first entry, since the list is densely-packed
     * (can be any ACL type)
     */
    if (pList->inUse == L7_TRUE)
    {
      rc = L7_TRUE;
    }

    return rc;
}

/*********************************************************************
*
* @purpose  To check if an ACL is assigned to this interface and direction.
*
* @param    L7_uint32     intIfNum    the internal interface number
* @param    L7_uint32     aclDir      the interface direction (internal)
* @param    L7_ACL_TYPE_t aclType     the type of ACL
* @param    L7_uint32     aclId       the ACL identifier
* @param    L7_uint32*    listIndex   the assign list index for this ACL
*
* @returns  L7_SUCCESS, if the ACL id was found for intf,dir
* @returns  L7_FAILURE, if the ACL id not found for intf,dir
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The function assumes the interface, intIfNum is configurable
*           i.e. aclMapIntfIsConfigurable() returns L7_TRUE
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirAclIdFind(L7_uint32 intIfNum, L7_uint32 aclDir,
                               L7_ACL_TYPE_t aclType, L7_uint32 aclId,
                               L7_uint32 *listIndex)
{
    L7_RC_t          rc = L7_FAILURE;
    aclIntfCfgData_t *pCfg;
    aclIntfParms_t   *pList;
    L7_uint32        i;

    if (aclDir >= ACL_INTF_DIR_MAX)
      return L7_FAILURE;

    *listIndex = 0;

    (void)aclMapIntfIsConfigurable(intIfNum, &pCfg);

    pList = pCfg->intf_dir_Tb[aclDir].assignList;

    for (i = 0; i < L7_ACL_MAX_LISTS_PER_INTF_DIR; i++, pList++)
    {
      if (pList->inUse != L7_TRUE)
        break;

      /* can be any ACL type */
      if ((pList->id.aclType == aclType) &&
          (pList->id.aclId == aclId))
      {
        *listIndex = i;
        rc = L7_SUCCESS;
        break;
      }
    }

    return rc;
}

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           vlan and direction
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid vlan, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the vlan in this
*                       direction
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
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
L7_RC_t aclImpVlanDirAclListGet( L7_uint32               vlanNum,
                                 L7_uint32               aclDir,
                                 L7_ACL_VLAN_DIR_LIST_t *listInfo )
{
  L7_RC_t           rc = L7_ERROR;
  aclVlanCfgData_t  *pCfg;
  aclIntfParms_t    *pList;
  L7_uint32         count;
  L7_uint32         i;

  /* check validity of vlanNum and direction */
  if ( ( aclIsValidVlan( vlanNum ) != L7_TRUE ) ||
       ( aclDir >= ACL_INTF_DIR_MAX ) )
  {
    return( L7_FAILURE );
  }

  if ( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  /* set up working ptr to assign list for this vlan direction */
  pList = pCfg->vlan_dir_Tb[aclDir].assignList;

  memset( listInfo, 0, sizeof( *listInfo ) );
  count = 0;

  for ( i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++, pList++ )
  {
    if ( pList->inUse != L7_TRUE )
    {
      break;
    }

    listInfo->listEntry[count].seqNum  = pList->seqNum;
    listInfo->listEntry[count].aclType = pList->id.aclType;
    listInfo->listEntry[count].aclId   = pList->id.aclId;
    count++;

  } /* endfor */

  listInfo->count = count;

  if ( count > 0 )
  {
    rc = L7_SUCCESS;
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  Gets a list of vlans to which an ACL is assigned in the
*           specified direction
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  aclDir      the vlan direction (internal)
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
L7_RC_t aclImpAssignedVlanDirListGet(L7_uint32 aclnum, L7_uint32 aclDir, L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList)
{
  L7_RC_t          rc = L7_ERROR;
  L7_uint32        cfgIndex;
  aclVlanCfgData_t *pCfg;
  L7_uint32        count;
  L7_uint32        i;
  aclIntfParms_t   *pList;

  /* check validity of aclnum */
  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
  {
    return(L7_FAILURE);
  }

  /* check ACL internal direction value */
  if (aclDir >= ACL_INTF_DIR_MAX)
  {
    return(L7_FAILURE);
  }

  memset(vlanList, 0, sizeof(*vlanList));
  count = 0;


  for (cfgIndex = 1; cfgIndex <= L7_ACL_VLAN_MAX_COUNT; cfgIndex++)
  {
    pCfg = &aclCfgFileData->cfgParms.aclVlanCfgData[cfgIndex];

    /* set up working ptr to assign list for this vlan direction */
    pList = pCfg->vlan_dir_Tb[aclDir].assignList;

    /* search entire assigned list for this vlan */
    for (i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++, pList++)
    {
      if (pList->inUse != L7_TRUE)
      {
        break;
      }

      if ((pList->id.aclType == L7_ACL_TYPE_IP) ||
          (pList->id.aclType == L7_ACL_TYPE_IPV6))
      {
        if (pList->id.aclId == aclnum)
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
* @purpose  To add an access list to the specified vlan and direction.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  aclDir     the vlan direction (internal)
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
L7_RC_t aclImpVlanDirectionAdd( L7_uint32 vlanNum,
                                L7_uint32 aclDir,
                                L7_uint32 aclnum,
                                L7_uint32 seqNum )
{
  void             *acl_ptr;
  aclVlanCfgData_t *pCfg;
  L7_ACL_TYPE_t    aclType;
  L7_RC_t          rc;

  /* check inputs */
  if ( ( aclDir >= ACL_INTF_DIR_MAX ) ||
       ( aclImpNumRangeCheck( aclnum ) != L7_SUCCESS ) ||
       ( seqNum > L7_ACL_MAX_VLAN_SEQ_NUM ) )
  {
    return( L7_FAILURE );
  }

  /* check for supported vlan type */
  if ( L7_FALSE == aclIsValidVlan( vlanNum ) )
  {
    return( L7_FAILURE );
  }

  acl_ptr = aclFindACL( aclnum );
  if ( L7_NULLPTR == acl_ptr )
  {
    return( L7_ERROR );
  }

  if ( aclMapVlanConfigEntryGet( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  if ( aclImpAclTypeGet( aclnum, &aclType ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  /* make sure this ACL type is compatible with any other type already applied */
  if (aclImpVlanAclTypeCompatCheck(vlanNum, aclDir, aclType) != L7_SUCCESS)
    return L7_NOT_SUPPORTED;

  /* update vlan,dir assign list with this ACL and notify driver of change */
  rc = aclImpVlanDirAdd( vlanNum, aclDir, aclType, aclnum, seqNum );
  if (rc != L7_SUCCESS)
  {
    /* if there are no ACLs configured on this VLAN ID after this failure, free up
     * configuration space
     */
    if (L7_TRUE != aclImpIsVlanInUse(vlanNum, ACL_INTF_DIR_IN) &&
        L7_TRUE != aclImpIsVlanInUse(vlanNum, ACL_INTF_DIR_OUT))
    {
      aclMapVlanConfigEntryRelease(vlanNum);
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if ACL type is compatible for VLAN and direction.
*
* @param    blanNum     @b{(input)} VLAN ID
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
*
* @returns  L7_SUCCESS        ACL type combination allowed
* @returns  L7_NOT_SUPPORTED  ACL type combination not allowed
* @returns  L7_FAILURE        invalid parameter, or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Function compares ACL types already assigned to this VLAN ID
*           and direction with this ACL against the feature support.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanAclTypeCompatCheck(L7_uint32 vlanNum, L7_uint32 aclDir,
                                     L7_ACL_TYPE_t aclType)
{
  L7_uint32               compId = L7_FLEX_QOS_ACL_COMPONENT_ID;
  L7_RC_t                 rc;
  L7_uint32               i;
  L7_BOOL                 hasMac, hasIpv4, hasIpv6;
  L7_BOOL                 suppInboundIpv4Mac, suppInboundIpv6Mac, suppInboundIpv6Ipv4;
  L7_BOOL                 suppOutboundIpv4Mac, suppOutboundIpv6Mac, suppOutboundIpv6Ipv4;
  L7_ACL_TYPE_t           localType;
  L7_ACL_VLAN_DIR_LIST_t  listInfo;

  /* assumes inputs validated by caller */

  /* init all flags */
  hasMac = hasIpv4 = hasIpv6 = L7_FALSE;
  suppInboundIpv4Mac  = cnfgrIsFeaturePresent(compId, L7_ACL_VLAN_MIXED_INBOUND_TYPE_ACL_FEATURE_ID);
  suppInboundIpv6Mac  = cnfgrIsFeaturePresent(compId, L7_ACL_VLAN_MIXED_INBOUND_IPV6_MAC_FEATURE_ID);
  suppInboundIpv6Ipv4 = cnfgrIsFeaturePresent(compId, L7_ACL_VLAN_MIXED_INBOUND_IPV6_IPV4_FEATURE_ID);
  suppOutboundIpv4Mac  = cnfgrIsFeaturePresent(compId, L7_ACL_VLAN_MIXED_OUTBOUND_TYPE_ACL_FEATURE_ID);
  suppOutboundIpv6Mac  = cnfgrIsFeaturePresent(compId, L7_ACL_VLAN_MIXED_OUTBOUND_IPV6_MAC_FEATURE_ID);
  suppOutboundIpv6Ipv4 = cnfgrIsFeaturePresent(compId, L7_ACL_VLAN_MIXED_OUTBOUND_IPV6_IPV4_FEATURE_ID);

  rc = aclImpVlanDirAclListGet(vlanNum, aclDir, &listInfo);
  if (rc != L7_SUCCESS)
  {
    /* an L7_ERROR means vlan,dir list currently empty, so return L7_SUCCESS */
    return (rc == L7_ERROR) ? L7_SUCCESS : L7_FAILURE;
  }

  /* scan the vlan,dir list and make note of assigned ACL types
   *
   * loop one extra time to factor in requested type for comparisons below
   * (must not index listEntry array during this extra pass)
   */
  for (i = 0; i <= listInfo.count; i++)
  {
    if (i == listInfo.count)
      localType = aclType;              /* extra pass: use parm value */
    else
      localType = listInfo.listEntry[i].aclType;

    switch (localType)
    {
    case L7_ACL_TYPE_IP:
      hasIpv4 = L7_TRUE;
      break;
    case L7_ACL_TYPE_MAC:
      hasMac = L7_TRUE;
      break;
    case L7_ACL_TYPE_IPV6:
      hasIpv6 = L7_TRUE;
      break;
    default:
      break;
    }
  }

  /* check assigned ACL type combos against feature support */
  rc = L7_SUCCESS;
  switch (aclDir)
  {
  case L7_INBOUND_ACL:
    if (suppInboundIpv4Mac == L7_FALSE)
    {
      if ((hasIpv4 == L7_TRUE) && (hasMac == L7_TRUE))
        rc = L7_NOT_SUPPORTED;
    }
    if (suppInboundIpv6Mac == L7_FALSE)
    {
      if ((hasIpv6 == L7_TRUE) && (hasMac == L7_TRUE))
        rc = L7_NOT_SUPPORTED;
    }
    if (suppInboundIpv6Ipv4 == L7_FALSE)
    {
      if ((hasIpv6 == L7_TRUE) && (hasIpv4 == L7_TRUE))
        rc = L7_NOT_SUPPORTED;
    }
    break;
  case L7_OUTBOUND_ACL:
    if (suppOutboundIpv4Mac == L7_FALSE)
    {
      if ((hasIpv4 == L7_TRUE) && (hasMac == L7_TRUE))
        rc = L7_NOT_SUPPORTED;
    }
    if (suppOutboundIpv6Mac == L7_FALSE)
    {
      if ((hasIpv6 == L7_TRUE) && (hasMac == L7_TRUE))
        rc = L7_NOT_SUPPORTED;
    }
    if (suppOutboundIpv6Ipv4 == L7_FALSE)
    {
      if ((hasIpv6 == L7_TRUE) && (hasIpv4 == L7_TRUE))
        rc = L7_NOT_SUPPORTED;
    }
    break;
  default:
    rc = L7_NOT_SUPPORTED;
    break;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To add an access list to the specified vlan and direction.
*
* @param    vlanNum     @b{(input)} internal vlan number
* @param    aclDir      @b{(input)} vlan direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
* @param    aclId       @b{(input)} access list identifier
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  vlan, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for vlan, dir
* @returns  L7_REQUEST_DENIED Error applying ACL to hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirAdd( L7_uint32     vlanNum,
                          L7_uint32     aclDir,
                          L7_ACL_TYPE_t aclType,
                          L7_uint32     aclId,
                          L7_uint32     seqNum )
{
  L7_BOOL           aclExists = L7_FALSE;
  L7_RC_t           rc;
  aclVlanCfgData_t  *pCfg;
  L7_uint32         listIndex;
  L7_uint32         listIndexNew;
  aclIntfParms_t    *pList;

  /* assumes inputs validated by caller */

  if ( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  if ( L7_SUCCESS == aclImpVlanDirAclIdFind( vlanNum,
                                             aclDir,
                                             aclType,
                                             aclId,
                                             &listIndex) )
  {
    aclExists = L7_TRUE;

    pList = pCfg->vlan_dir_Tb[aclDir].assignList;

    /* ACL already assigned to this vlan,dir.  If sequence number is
     * the same, there is no change to the ACL ordering so treat this
     * request as a no-op.  A different sequence number requires removing
     * the ACL from the assign list and re-adding it in the proper position.
     */
    if ( pList[listIndex].seqNum == seqNum )
    {
      return( L7_SUCCESS );
    }

    /* if new sequence number is 0 and this ACL is already the last (or only)
     * element in the list, this is treated as a no-op
     */
    if ( L7_ACL_AUTO_INCR_INTF_SEQ_NUM == seqNum )
    {
      /* this ACL is last in the list if it is at the end of the
       * array, or if the next array element is not in use
       */
      if ( ( (L7_ACL_MAX_LISTS_PER_VLAN_DIR-1) == listIndex ) ||
           ( pList[listIndex+1].inUse != L7_TRUE ) )
      {
        return( L7_SUCCESS );
      }
    }

    /* update app */
    rc = aclVlanDirListRemove( vlanNum, aclDir, aclType, aclId );
    if ( rc != L7_SUCCESS )
    {
      return( rc );
    }

    /* NOTE: Driver update below will handle removing old ACL rules */
  }

  /* update app */
  rc = aclVlanDirListAdd( vlanNum, aclDir, aclType, aclId, seqNum );
  if ( rc != L7_SUCCESS )
  {
    return( rc );
  }

  /* if ACL was already assigned to this vlan,dir and its new list position
   * has not changed, there is no need to update the driver
   */
  if ( L7_TRUE == aclExists )
  {
    if ( L7_SUCCESS == aclImpVlanDirAclIdFind( vlanNum,
                                               aclDir,
                                               aclType,
                                               aclId,
                                               &listIndexNew) )
    {
      if ( listIndexNew == listIndex )
      {
        return( L7_SUCCESS );
      }
    }
  }

  /* update driver
   *
   * NOTE:  Underlying code will delete existing rules from device before
   *        adding new set of rules.
   */
  rc = aclVlanUpdate( vlanNum, aclDir, ACL_VLAN_ADD );

  /* if driver failed */
  if ( rc != L7_SUCCESS )
  {
    /* rollback */
    (void)aclVlanDirListRemove(vlanNum, aclDir, aclType, aclId);   /* this rc is ignored */
    /* try to restore hardware to configuration before attempted add */
    /* check if any ACLs remain assigned to this vlan,dir to issue new
     * command to the device
     */
    if (L7_TRUE == aclImpIsVlanInUse(vlanNum, aclDir))
    {
      (void)aclVlanUpdate(vlanNum, aclDir, ACL_VLAN_ADD);
    }
    return( L7_REQUEST_DENIED );
  }

  return( L7_SUCCESS );
}

/*********************************************************************
*
* @purpose  To remove an access list from the specified vlan and direction.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  aclDir     the vlan direction (internal)
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if vlanNum is removed from the access-list
* @returns  L7_FAILURE, if invalid identifying number or
*                       internal vlan number does not exist
* @returns  L7_ERROR, if access-list not found for this vlan, dir
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirectionRemove( L7_uint32 vlanNum,
                                   L7_uint32 aclDir,
                                   L7_uint32 aclnum )
{
  void             *acl_ptr;
  aclVlanCfgData_t *pCfg;
  L7_ACL_TYPE_t    aclType;
  L7_RC_t          rc;

  /* check validity of the access-list identifying number, vlanNum, and direction */
  if ( ( aclImpNumRangeCheck( aclnum ) != L7_SUCCESS ) ||
       ( aclDir >= ACL_INTF_DIR_MAX ) )
  {
    return( L7_FAILURE );
  }

  /* check for supported vlan type */
  if ( L7_FALSE == aclIsValidVlan( vlanNum ) )
  {
    return( L7_FAILURE );
  }

  acl_ptr = aclFindACL( aclnum );
  if ( L7_NULLPTR == acl_ptr )
  {
    return( L7_ERROR );
  }

  /* Need to invoke this function before invoking aclImpIsVlanInUse().
     aclMapVlanIsConfigurable(vlanNum, &pCfg) == L7_TRUE is a pre-condition
     for aclImpIsVlanInUse() */
  if( aclMapVlanIsConfigurable( vlanNum, &pCfg ) != L7_TRUE )
  {
    return( L7_FAILURE );
  }

  if ( aclImpAclTypeGet( aclnum, &aclType ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }

  rc = aclImpVlanDirRemove(vlanNum, aclDir, aclType, aclnum);

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

/*********************************************************************
*
* @purpose  To remove an access list from the specified vlan and direction.
*
* @param    vlanNum     @b{(input)} internal vlan number
* @param    aclDir      @b{(input)} vlan direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
* @param    aclId       @b{(input)} access list identifier
*
* @returns  L7_SUCCESS  ACL successfully removed from vlan,dir
* @returns  L7_FAILURE  invalid ACL identifier, or vlan does not exist
* @returns  L7_ERROR    ACL does not exist for this id, or not assigned
*                         to the vlan,dir
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirRemove( L7_uint32     vlanNum,
                             L7_uint32     aclDir,
                             L7_ACL_TYPE_t aclType,
                             L7_uint32     aclId )
{
  L7_RC_t rc;

  /* assumes inputs validated by caller */

  /* update app */
  rc = aclVlanDirListRemove( vlanNum, aclDir, aclType, aclId );
  if ( rc != L7_SUCCESS )
  {
    return( rc );
  }

  /* update driver */
  rc = aclVlanUpdate( vlanNum, aclDir, ACL_VLAN_REMOVE );
  if ( rc != L7_SUCCESS )
  {
    return( L7_REQUEST_DENIED );
  }

  /* check if any ACLs remain assigned to this vlan,dir to issue new
   * command to the device
   */
  if ( L7_TRUE == aclImpIsVlanInUse( vlanNum, aclDir ) )
  {
    if ( aclVlanUpdate( vlanNum, aclDir, ACL_VLAN_ADD ) != L7_SUCCESS )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "Removed %s ACL %u from vlan %u %s, but unable to "
              "apply remaining ACLs to device\n",
              acl_type_str[aclType],
              aclId,
              vlanNum,
              acl_direction_str[aclDir] );

      return( L7_REQUEST_DENIED );
    }
  }

  return( L7_SUCCESS );
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
L7_RC_t aclImpVlanIdGetNext(L7_uint32 vlanNum, L7_uint32 *nextVlanNum)
{
  L7_uint32 i;

  if (nextVlanNum == L7_NULLPTR)
  {
    return(L7_ERROR);
  }

  for (i = vlanNum+1; i <= L7_ACL_MAX_VLAN_ID; i++)
  {
    if (aclMapVlanIsConfigurable(i, L7_NULL) == L7_TRUE)
    {
      *nextVlanNum = i;
      return(L7_SUCCESS);
    }
  }
  /* no next vlan id found */
  return(L7_FAILURE);
}

/*********************************************************************
*
* @purpose  To check if an vlan and direction is in use by any access list.
*
* @param    L7_uint32  vlanNum   the internal vlan number
* @param    L7_uint32  aclDir    the vlan direction (internal)
*
* @returns  L7_TRUE, if the vlan,dir has at least one ACL attached
* @returns  L7_FALSE, if the vlan,dir has no ACL attached
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The function assumes the vlan, vlanNum is configurable
*           i.e. aclMapVlanIsConfigurable() returns L7_TRUE
* @end
*
*********************************************************************/
L7_BOOL aclImpIsVlanInUse( L7_uint32 vlanNum, L7_uint32 aclDir )
{
  L7_BOOL          rc = L7_FALSE;
  aclVlanCfgData_t *pCfg;
  aclIntfParms_t   *pList;

  if ( aclDir >= ACL_INTF_DIR_MAX )
  {
    return( L7_FALSE );
  }

  if (aclMapVlanIsConfigurable(vlanNum, &pCfg) == L7_TRUE)
  {
    pList = pCfg->vlan_dir_Tb[aclDir].assignList;

    /* only need to check the first entry, since the list is densely-packed
     * (can be any ACL type)
     */
    if ( L7_TRUE == pList->inUse )
    {
      rc = L7_TRUE;
    }
  }
  return( rc );
}

/*********************************************************************
*
* @purpose  To check if an ACL is assigned to this vlan and direction.
*
* @param    L7_uint32     vlanNum     the internal vlan number
* @param    L7_uint32     aclDir      the vlan direction (internal)
* @param    L7_ACL_TYPE_t aclType     the type of ACL
* @param    L7_uint32     aclId       the ACL identifier
* @param    L7_uint32*    listIndex   the assign list index for this ACL
*
* @returns  L7_SUCCESS, if the ACL id was found for vlan,dir
* @returns  L7_FAILURE, if the ACL id not found for vlan,dir
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The function assumes the vlan, vlanNum is configurable
*           i.e. aclMapVlanIsConfigurable() returns L7_TRUE
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirAclIdFind( L7_uint32     vlanNum,
                                L7_uint32     aclDir,
                                L7_ACL_TYPE_t aclType,
                                L7_uint32     aclId,
                                L7_uint32    *listIndex )
{
  L7_RC_t          rc = L7_FAILURE;
  aclVlanCfgData_t *pCfg;
  aclIntfParms_t   *pList;
  L7_uint32        i;

  if ( aclDir >= ACL_INTF_DIR_MAX )
  {
    return( L7_FAILURE );
  }

  *listIndex = 0;

  if (aclMapVlanIsConfigurable( vlanNum, &pCfg ) == L7_TRUE)
  {
    pList = pCfg->vlan_dir_Tb[aclDir].assignList;

    for ( i = 0; i < L7_ACL_MAX_LISTS_PER_VLAN_DIR; i++, pList++ )
    {
      if ( pList->inUse != L7_TRUE )
      {
        break;
      }

      /* can be any ACL type */
      if ( ( pList->id.aclType == aclType ) &&
           ( pList->id.aclId   == aclId ) )
      {
        *listIndex = i;
        rc         = L7_SUCCESS;
        break;
      }
    }
  }

  return( rc );
}

/*********************************************************************
*
* @purpose  Notify registered users of a change to an ACL.
*
* @param    event       Event indication
* @param    aclType     Access list type
* @param    aclnum      ACL associated with the event
* @param    aclOldName  Previous ACL name, or L7_NULLPTR
*
* @returns  void
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments It is important that the caller only hold a read lock.
*           The route filter callback functions are likely to call
*           aclRouteFilter(), so using a write lock here would
*           cause a deadlock during the callback.
*
* @comments The aclOldName parm is used for a rename or delete event
*           only, and is set to L7_NULLPTR otherwise.
*
* @end
*
*********************************************************************/
void aclImpNotify(aclEvent_t event, L7_ACL_TYPE_t aclType,
                  L7_uint32 aclnum, L7_uchar8 *aclOldName)
{
  L7_uint32             i, maxEnt;
  L7_ACL_TYPE_t         tempAclType;
  L7_char8              aclName[L7_ACL_NAME_LEN_MAX+1];
  aclNotifyEvent_t      aclEvent = 0;
  aclRouteFilter_t      rteEvent = 0;
  L7_BOOL               sendAclEvent = L7_TRUE;
  L7_BOOL               sendRteEvent = L7_TRUE;
  aclCallbackEntry_t    *pTableStart, *pCbEntry;
  L7_RC_t               rc = L7_FAILURE;

  /* set up a name string based on ACL type
   *
   * NOTE: A delete event uses the aclOldName that was passed in, since
   *       the ACL no longer exists from which to get its name.  However,
   *       the aclOldName is moved into aclName for passing the deleted
   *       ACL's name to the callback functions.
   */
  if (ACL_EVENT_DELETE == event)
  {
    memcpy(aclName, aclOldName, sizeof(aclName));
    aclOldName = L7_NULLPTR;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = aclImpCommonNameStringGet(aclnum, &tempAclType, aclName);
    if ((L7_SUCCESS == rc) && (tempAclType != aclType))
    {
      ACL_PRT(ACL_MSGLVL_HI,
              "\n%s: Unexpected ACL type for name \'%s\': expected=%d got%d\n",
              __FUNCTION__, aclName, aclType, tempAclType);
      rc = L7_FAILURE;
    }
  }

  if (L7_SUCCESS != rc)
    return;

  /* route filtering is only interested in IPV4 and IPV6 access lists
   * (this flag may also get turned off for certain event types below)
   */
  if ((L7_ACL_TYPE_IP != aclType) &&
      (L7_ACL_TYPE_IPV6 != aclType))
  {
    sendRteEvent = L7_FALSE;
  }

  /* translate internal event value to both route filter and ACL notify event values */
  switch (event)
  {
    case ACL_EVENT_CREATE:
      aclEvent = L7_ACL_NOTIFY_EVENT_CREATE;
      rteEvent = L7_ACL_ROUTE_FILTER_CREATE;
      break;

    case ACL_EVENT_MODIFY:
      aclEvent = L7_ACL_NOTIFY_EVENT_MODIFY;
      sendRteEvent = L7_FALSE;
      break;

    case ACL_EVENT_ATTR_ONLY_MODIFY:
      aclEvent = L7_ACL_NOTIFY_EVENT_ATTR_ONLY_MODIFY;
      sendRteEvent = L7_FALSE;
      break;

    case ACL_EVENT_ROUTE_FILTER_MODIFY:
      aclEvent = L7_ACL_NOTIFY_EVENT_MODIFY;
      rteEvent = L7_ACL_ROUTE_FILTER_MODIFY;
      break;

    case ACL_EVENT_DELETE:
      aclEvent = L7_ACL_NOTIFY_EVENT_DELETE;
      rteEvent = L7_ACL_ROUTE_FILTER_DELETE;
      break;

    case ACL_EVENT_RENAME:
      aclEvent = L7_ACL_NOTIFY_EVENT_RENAME;
      sendRteEvent = L7_FALSE;
      break;

    default:
      sendAclEvent = L7_FALSE;
      sendRteEvent = L7_FALSE;
      break;

  } /* endswitch */

  /* send route filter callback events */
  if (L7_TRUE == sendRteEvent)
  {
    pTableStart = pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_ROUTE_FILTER].pStart;
    maxEnt      = pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_ROUTE_FILTER].maxEntries;

    for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
    {
      if (pCbEntry->funcPtr != L7_NULLPTR)
      {
        /* This callback always uses the ACL number or index, not a name. */
        (pCbEntry->funcPtr)(aclnum, rteEvent);
      }
    } /* endfor */
  }

  /* send ACL notification events */
  if (L7_TRUE == sendAclEvent)
  {
    pTableStart = pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_NOTIFY_EVENT].pStart;
    maxEnt      = pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_NOTIFY_EVENT].maxEntries;

    for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
    {
      if (L7_NULLPTR != pCbEntry->funcPtr)
      {
        /* This callback always uses an ACL name string, even for IPv4 numbered lists.
         *
         * The aclOldName parm is L7_NULLPTR except for a name change event. If the
         * access list is being named for the first time, the aclOldName points to
         * the empty string "".
         */
        (pCbEntry->funcPtr)(aclEvent, aclType, aclnum, aclName, aclOldName);
      }
    } /* endfor */
  }
}

/*********************************************************************
*
* @purpose  Ask registered applications if the proposed ACL change is permissible.
*
* @param    aclChange   Change approval ID
* @param    aclnum      ACL associated with the change
* @param    pData       Ptr to request-specific data, or L7_NULLPTR
*
* @returns  L7_SUCCESS  Allow ACL change
* @returns  L7_FAILURE  Do not allow ACL change
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Gives registered apps the chance to block an ACL from
*           being changed by the user when it is being used elsewhere
*           in the switch configuration.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpChangeApprovalQuery(aclChangeApproval_t aclChange, L7_uint32 aclnum, void *pData)
{
  L7_uint32             i, maxEnt;
  aclCallbackEntry_t    *pTableStart, *pCbEntry;
  L7_FUNCPTR_t          funcPtr;
  L7_char8              aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_ACL_TYPE_t         aclType = L7_ACL_TYPE_NONE;
  L7_RC_t               rc = L7_FAILURE;

  /* set up a name string based on ACL type
   *
   * NOTE: If we cannot get the ACL name string then there is something
   *       strange about this ACL, so allow it to be changed without
   *       querying the registrants.
   */
  if (aclImpCommonNameStringGet(aclnum, &aclType, aclName) != L7_SUCCESS)
    return L7_SUCCESS;                  /* see note above! */

  /* initiate ACL change approval callbacks */
  pTableStart = pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_CHANGE_APPROVAL].pStart;
  maxEnt      = pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_CHANGE_APPROVAL].maxEntries;

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (L7_NULLPTR != pCbEntry->funcPtr)
    {
      /* This callback always uses an ACL name string, even for IPv4 numbered lists.
       *
       * Can stop after the first negative response since that is enough to
       * prevent the ACL from being changed.
       */
      funcPtr = (L7_FUNCPTR_t) pCbEntry->funcPtr;
      rc = (funcPtr)(aclChange, aclType, aclnum, aclName, pData);
      if (L7_SUCCESS != rc)
      {
        return L7_FAILURE;
      }
    }
  } /* endfor */

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the current number of IP ACLs configured
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
L7_RC_t aclImpCurrNumGet(L7_uint32 *pCurr)
{
  *pCurr = avlTreeCount(&aclTree);
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Check if IP ACL table is full
*
* @param    void
*
* @returns  L7_TRUE     ACL table is full
* @returns  L7_FALSE    ACL table not full
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_BOOL aclImpIsTableFull(void)
{
  L7_uint32     currCount;

  currCount = avlTreeCount(&aclTree);
  return (currCount >= acl_max_entries) ? L7_TRUE : L7_FALSE;
}

/*************************************************************************
* @purpose  Check if maximum number of access lists of any type have been created
*
* @param    void
*
* @returns  L7_TRUE     maximum number of ACLs exist
* @returns  L7_FALSE    ACLs below maximum number
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_BOOL aclImpIsTotalListsAtMax(void)
{
  return (acl_curr_entries_g >= L7_ACL_MAX_LISTS) ? L7_TRUE : L7_FALSE;
}

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the standard ACL number ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpStdAclNumRangeCheck(L7_uint32 aclnum)
{
  L7_RC_t rc = L7_FAILURE;

  /* sequentially check through the various ACL number ranges that are supported */
  if ((aclnum >= L7_ACL_MIN_STD1_ID) && (aclnum <= L7_ACL_MAX_STD1_ID))
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the extended ACL number ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpExtAclNumRangeCheck(L7_uint32 aclnum)
{
  L7_RC_t rc = L7_FAILURE;

  /* sequentially check through the various ACL number ranges that are supported */
  if ((aclnum >= L7_ACL_MIN_EXT1_ID) && (aclnum <= L7_ACL_MAX_EXT1_ID))
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to any extended IP ACL range
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Both numbered and named IP extended ACLs are considered here,
*           since these support the same ACL rule definitions.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpExtGroupRangeCheck(L7_uint32 aclnum)
{
  L7_RC_t       rc = L7_FAILURE;

  /* sequentially check each supported extended ACL type */
  if (aclImpExtAclNumRangeCheck(aclnum) == L7_SUCCESS)
    rc = L7_SUCCESS;
  else if (aclImpNamedIndexRangeCheck(L7_ACL_TYPE_IP, aclnum) == L7_SUCCESS)
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to any extended IP or IPV6 ACL range
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Both numbered and named IP extended ACLs are considered here,
*           as well as named IPV6 ACLs.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpExtGroupOrIpv6RangeCheck(L7_uint32 aclnum)
{
  L7_RC_t       rc = L7_FAILURE;

  /* sequentially check each supported extended ACL type */
  if (aclImpExtGroupRangeCheck(aclnum) == L7_SUCCESS)
    rc = L7_SUCCESS;
  else if (aclImpNamedIpv6IndexRangeCheck(aclnum) == L7_SUCCESS)
    rc = L7_SUCCESS;
  else if (aclImpNamedIpv4IndexRangeCheck(aclnum) == L7_SUCCESS)
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the ACL number belongs to any of the supported named
*           IP ACL index ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments This function only considers whether the aclnum belongs to
*           any supported named ACL index range.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIndexRangeCheck(L7_uint32 aclnum)
{
  L7_RC_t       rc = L7_FAILURE;

  /* sequentially check each named IP ACL type for an index range match */
  if ((aclImpNamedIpv6IndexRangeCheck(aclnum) == L7_SUCCESS) ||
      (aclImpNamedIpv4IndexRangeCheck(aclnum) == L7_SUCCESS))
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  Output the ACL type based on the specified ACL number
*
* @param    aclnum      @b{(input)}  access-list identifying number
*
* @returns  L7_ACL_TYPE_t  the access-list type
*
* @comments Outputs L7_ACL_TYPE_NONE if the ACL number is unknown
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_ACL_TYPE_t aclImpTypeFromIndexGet(L7_uint32 aclnum)
{
  L7_ACL_TYPE_t aclType;

  /* sequentially check all supported ACL type ranges */
  if ((aclImpStdAclNumRangeCheck(aclnum) == L7_SUCCESS) ||
      (aclImpExtAclNumRangeCheck(aclnum) == L7_SUCCESS))
  {
    aclType = L7_ACL_TYPE_IP;
  }
  else if (aclMacImpIndexRangeCheck(aclnum) == L7_SUCCESS)
  {
    aclType = L7_ACL_TYPE_MAC;
  }
  else if (aclImpNamedIpv6IndexRangeCheck(aclnum) == L7_SUCCESS)
  {
    aclType = L7_ACL_TYPE_IPV6;
  }
  else if (aclImpNamedIpv4IndexRangeCheck(aclnum) == L7_SUCCESS)
  {
    aclType = L7_ACL_TYPE_IP;
  }
  else
  {
    aclType = L7_ACL_TYPE_NONE;
  }

  return aclType;
}

/*********************************************************************
*
* @purpose  Outputs the IP ACL type if it belongs to one of the supported
*           number ranges
*
* @param    L7_uint32     aclnum    the access-list identifying number
* @param    L7_ACL_TYPE_t *acltype  the access-list type
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpAclTypeGet(L7_uint32 aclnum, L7_ACL_TYPE_t *acltype)
{
  L7_RC_t rc = L7_FAILURE;
  L7_ACL_TYPE_t localAclType;

  localAclType = aclImpTypeFromIndexGet(aclnum);

  if ((localAclType == L7_ACL_TYPE_IP) ||
      (localAclType == L7_ACL_TYPE_IPV6))
  {
    *acltype = localAclType;
    rc = L7_SUCCESS;
  }

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
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The aclnum parameter is required by this function in case there
*           is ever a need to support separate rule number ranges based
*           on the type of ACL (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpRuleNumRangeCheck(L7_uint32 aclnum, L7_uint32 rulenum)
{
  L7_RC_t rc = L7_FAILURE;

  if (aclImpTypeFromIndexGet(aclnum) != L7_ACL_TYPE_NONE)
  {
    /* sequentially check through the various rule number ranges that are supported */
    if ((rulenum >= L7_ACL_MIN_RULE_NUM) && (rulenum <= L7_ACL_MAX_RULE_NUM))
      rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the current number of configured rules in an IP ACL
*
* @param    aclnum      @b{(input)}  access-list identifying number
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
L7_RC_t aclImpAclRuleCountGet(L7_uint32 aclnum, L7_uint32 *ruleCount)
{
  aclStructure_t *ptr;

  if (aclImpNumRangeCheck(aclnum) != L7_SUCCESS)
    return L7_FAILURE;

  ptr = (aclStructure_t*)aclFindACL(aclnum);
  if (ptr == L7_NULLPTR)
    return L7_ERROR;

  *ruleCount = ptr->ruleCount;

  return L7_SUCCESS;
}

/*************************************************************************
*
* @purpose  Check if rule logging is allowed for a given rule action
*
* @param    action      @b{(input)}  access list rule action
*
* @returns  L7_TRUE     logging allowed for rule
* @returns  L7_FALSE    logging not allowed for rule
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_BOOL aclImpLoggingIsAllowed(L7_ACL_ACTION_t action)
{
  L7_uint32     featureId;

  if (action == L7_ACL_DENY)
    featureId = L7_ACL_LOG_DENY_FEATURE_ID;
  else
    featureId = L7_ACL_LOG_PERMIT_FEATURE_ID;

  return cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, featureId);
}

/*********************************************************************
* @purpose  Display the current IP ACL Table contents
*
* @param    showRules   @b{(input)} Indicates if ACL rules are displayed
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void aclIpShow(L7_BOOL showRules)
{
  L7_uint32         msgLvlReqd;
  avlTree_t         *pTree = &aclTree;
  L7_uint32         count;
  L7_uint32         aclNum;
  aclStructure_t    *pAcl;
  L7_ACL_TYPE_t     aclType;

  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_ON;

  if (ACL_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if ((count = avlTreeCount(pTree)) == 0)
    {
      (void)ACL_SEMA_GIVE(pTree->semId);
      ACL_PRT(msgLvlReqd, "IP ACL table is empty\n\n");
      return;
    }

    ACL_PRT(msgLvlReqd, "IP ACL Table (contains %u entries):\n\n", count);

    ACL_PRT(msgLvlReqd, "aclNum Rules               Name            \n");
    ACL_PRT(msgLvlReqd, "------ ----- ------------------------------\n");

    aclNum = 0;                                   /* start with first entry */
    while ((pAcl = avlSearchLVL7(pTree, &aclNum, AVL_NEXT)) != L7_NULLPTR)
    {
      ACL_PRT(msgLvlReqd, "%6u  %3u  %s \n",
              pAcl->aclNum, pAcl->ruleCount, pAcl->aclName);

      /* update search keys for next pass */
      aclNum = pAcl->aclNum;
    }
    ACL_PRT(msgLvlReqd, "\n\n");

    /* optionally display the rules for each ACL */
    if (showRules == L7_TRUE)
    {
      aclNum = 0;                                 /* start with first entry */
      while ((pAcl = avlSearchLVL7(pTree, &aclNum, AVL_NEXT)) != L7_NULLPTR)
      {
        aclType = aclImpTypeFromIndexGet(pAcl->aclNum);
        if (aclType == L7_ACL_TYPE_IP)
        {
          aclIpRulesShow(pAcl->aclNum);
        }
        else if (aclType == L7_ACL_TYPE_IPV6)
        {
          aclIpv6RulesShow(pAcl->aclNum);
        }

        /* update search keys for next pass */
        aclNum = pAcl->aclNum;
      }
    }

    (void)ACL_SEMA_GIVE(pTree->semId);
  }
}

/* common, internal helper function for displaying dotted IP addresses */
static void aclIpAddrPrint(L7_uint32 msgLvlReqd, L7_uint32 ip)
{
  ACL_PRT(msgLvlReqd, "%u.%u.%u.%u",
          (ip >> 24) & 0x000000FF,
          (ip >> 16) & 0x000000FF,
          (ip >> 8) & 0x000000FF,
          (ip >> 0) & 0x000000FF);
}

static void aclIpv6AddrPrint(L7_uint32 msgLvlReqd, L7_uchar8 *pAddr)
{
  L7_uint32     i;

  for (i = 0; i < (L7_IP6_ADDR_LEN-2); i += 2)
  {
    ACL_PRT(msgLvlReqd, "%2.2x%2.2x:", pAddr[i], pAddr[i+1]);
  }
  ACL_PRT(msgLvlReqd, "%2.2x%2.2x", pAddr[i], pAddr[i+1]);
}

/*********************************************************************
* @purpose  Display content of all rules for a specific IP ACL
*
* @param    aclNum      @b{(input)} IP access list index
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclIpRulesShow(L7_uint32 aclNum)
{
  L7_uint32           msgLvlReqd;
  avlTree_t           *pTree = &aclTree;
  aclStructure_t      *pAcl;
  aclRuleParms_t      *pRule;

  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_ON;

  pAcl = avlSearchLVL7(pTree, &aclNum, AVL_EXACT);
  if (pAcl == L7_NULLPTR)
  {
    ACL_PRT(msgLvlReqd, "IP ACL %u entry not found\n\n", aclNum);
    return;
  }
  if (pAcl->aclNum != aclNum)
  {
    ACL_PRT(msgLvlReqd, "IP ACL entry number %u does not match requested ACL number %u\n\n",
            pAcl->aclNum, aclNum);
    return;
  }

  pRule = pAcl->head;

  if (pRule->last == L7_TRUE)
    return;

  ACL_PRT(msgLvlReqd, "IP ACL \"%s\" (Index %u)\n", (char *)pAcl->aclName, pAcl->aclNum);

  while ((pRule != L7_NULLPTR) && (pRule->last != L7_TRUE))
  {
    aclIpRuleDisplay(L7_ACL_TYPE_IP, pRule, msgLvlReqd);

    /* update search keys for next pass */
    pRule = pRule->next;
  }
  ACL_PRT(msgLvlReqd, "\n\n");
}

/*********************************************************************
* @purpose  Display content of all rules for a specific IPV6 ACL
*
* @param    aclIndex    @b{(input)} IPV6 access list index
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclIpv6RulesShow(L7_uint32 aclIndex)
{
  L7_uint32           msgLvlReqd;
  avlTree_t           *pTree = &aclTree;
  aclStructure_t      *pAcl;
  aclRuleParms_t      *pRule;

  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_ON;

  pAcl = avlSearchLVL7(pTree, &aclIndex, AVL_EXACT);
  if (pAcl == L7_NULLPTR)
  {
    ACL_PRT(msgLvlReqd, "IPV6 ACL entry not found for index %u\n\n", aclIndex);
    return;
  }
  if (pAcl->aclNum != aclIndex)
  {
    ACL_PRT(msgLvlReqd, "IPV6 ACL entry index %u does not match requested ACL index %u\n\n",
            pAcl->aclNum, aclIndex);
    return;
  }

  pRule = pAcl->head;

  if (pRule->last == L7_TRUE)
    return;

  ACL_PRT(msgLvlReqd, "IPV6 ACL \"%s\" (Index %u)\n", (char *)pAcl->aclName, pAcl->aclNum);

  while ((pRule != L7_NULLPTR) && (pRule->last != L7_TRUE))
  {
    aclIpRuleDisplay(L7_ACL_TYPE_IPV6, pRule, msgLvlReqd);

    /* update search keys for next pass */
    pRule = pRule->next;
  }
  ACL_PRT(msgLvlReqd, "\n\n");
}

/*********************************************************************
* @purpose  Display contents of an IP ACL rule
*
* @param    aclType     @b{(input)} Access list type
* @param    *pRule      @b{(input)} ACL rule pointer
* @param    msgLvlReqd  @b{(input)} Desired output message level
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclIpRuleDisplay(L7_ACL_TYPE_t aclType, aclRuleParms_t *pRule, L7_uint32 msgLvlReqd)
{
  char                *pSpacer;
  aclTRTableEntry_t   trEntry;
  pSpacer = " ";
  ACL_PRT(msgLvlReqd, "%s Rule %u\n", pSpacer, pRule->ruleNum);

  pSpacer = "   ";

  ACL_PRT(msgLvlReqd, "%s cfgMask=0x%8.8x ", pSpacer, pRule->configMask);
  if (pRule->configMask != 0)
  {
    ACL_PRT(msgLvlReqd, "(");
    aclRuleCfgMaskPrint(L7_ACL_TYPE_IP, pRule->configMask, msgLvlReqd);
    ACL_PRT(msgLvlReqd, ")");
  }
  ACL_PRT(msgLvlReqd, "\n");

  ACL_PRT(msgLvlReqd, "%s act=%1s log=%1s every=%1s asgnQ=%1u ",
          pSpacer,
          (pRule->action == L7_ACL_DENY) ? "D" : "P",
          (pRule->logging == L7_FALSE) ? "F" : "T",
          (pRule->every == L7_FALSE) ? "F" : "T",
          pRule->assignQueueId);

  if(pRule->timeRangeEntry !=L7_NULLPTR)
  {
     trEntry = aclTimeRangeTable[pRule->timeRangeEntry->trId];
     ACL_PRT(msgLvlReqd, "%s time range name=%s status=%d",
     pSpacer,
     trEntry.timeRangeName,trEntry.status);
  }


  ACL_PRT(msgLvlReqd, "redir=%u/%u/%u mirror=%u/%u/%u\n",
          pRule->redirectConfigId.configSpecifier.usp.unit,
          pRule->redirectConfigId.configSpecifier.usp.slot,
          pRule->redirectConfigId.configSpecifier.usp.port,
          pRule->mirrorConfigId.configSpecifier.usp.unit,
          pRule->mirrorConfigId.configSpecifier.usp.slot,
          pRule->mirrorConfigId.configSpecifier.usp.port);

  if (aclType == L7_ACL_TYPE_IP)
  {
    ACL_PRT(msgLvlReqd, "%s dip=", pSpacer);
    aclIpAddrPrint(msgLvlReqd, pRule->dstIp.v4.addr);
    ACL_PRT(msgLvlReqd, " dmask=");
    aclIpAddrPrint(msgLvlReqd, pRule->dstIp.v4.mask);
    ACL_PRT(msgLvlReqd, " sip=");
    aclIpAddrPrint(msgLvlReqd, pRule->srcIp.v4.addr);
    ACL_PRT(msgLvlReqd, " smask=");
    aclIpAddrPrint(msgLvlReqd, pRule->srcIp.v4.mask);
    ACL_PRT(msgLvlReqd, "\n");
  }
  else if (aclType == L7_ACL_TYPE_IPV6)
  {
    ACL_PRT(msgLvlReqd, "%s dipv6=", pSpacer);
    aclIpv6AddrPrint(msgLvlReqd, pRule->dstIp.v6.in6Addr.in6.addr8);
    ACL_PRT(msgLvlReqd, "/%d", pRule->dstIp.v6.in6PrefixLen);
    ACL_PRT(msgLvlReqd, " sipv6=");
    aclIpv6AddrPrint(msgLvlReqd, pRule->srcIp.v6.in6Addr.in6.addr8);
    ACL_PRT(msgLvlReqd, "/%d", pRule->srcIp.v6.in6PrefixLen);
    ACL_PRT(msgLvlReqd, " flowlbl=%u", pRule->flowlbl);
    ACL_PRT(msgLvlReqd, "\n");
  }

  ACL_PRT(msgLvlReqd, "%s prot=%2u protmask=%2u tos=0x%2.2x tosmask=0x%2.2x\n",
          pSpacer, pRule->protocol, pRule->protmask,
          pRule->tosbyte, pRule->tosmask);

  ACL_PRT(msgLvlReqd, "%s dport=%-5u dpStart=%-5u dpEnd=%-5u ",
          pSpacer, pRule->dstPort, pRule->dstStartPort, pRule->dstEndPort);
  ACL_PRT(msgLvlReqd, "sport=%-5u spStart=%-5u spEnd=%-5u\n",
          pRule->srcPort, pRule->srcStartPort, pRule->srcEndPort);
}

/*********************************************************************
* @purpose  Display the ACL rule config mask contents
*
* @param    aclType     @b{(input)} Access list type
* @param    mask        @b{(input)} ACL rule config mask
* @param    msgLvlReqd  @b{(input)} Debug print message level
*
* @returns  void
*
* @comments This can be used for any ACL type.
*
* @end
*********************************************************************/
void aclRuleCfgMaskPrint(L7_ACL_TYPE_t aclType, L7_uint32 mask, L7_uint32 msgLvlReqd)
{
  L7_BOOL       isFirst = L7_TRUE;
  L7_uint32     i, imax;
  char          **cfgMaskStr;

  if ((aclType == L7_ACL_TYPE_IP) ||
      (aclType == L7_ACL_TYPE_IPV6))
  {
    imax = ACL_RULEFIELDS_TOTAL;
    cfgMaskStr = aclIpRuleCfgMaskStr;
  }
  else if (aclType == L7_ACL_TYPE_MAC)
  {
    imax = ACL_MAC_RULEFIELDS_TOTAL;
    cfgMaskStr = aclMacRuleCfgMaskStr;
  }
  else
  {
    return;
  }

  for (i = 1; i < imax; i++)
  {
    if ((mask & (1 << i)) != 0)
    {
      /* print a comma before each item except the first */
      if (isFirst == L7_TRUE)
      {
        isFirst = L7_FALSE;
      }
      else
      {
        ACL_PRT(msgLvlReqd, ",");
      }

      ACL_PRT(msgLvlReqd, "%s", cfgMaskStr[i]);
    }
  }
}

/*********************************************************************
*
* @purpose  Creates the ACL correlator table
*
* @param    maxEntries  @{(input)}  maximum number of entries in table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclCorrTableCreate(L7_uint32 maxEntries)
{
  pAclCorrTable_g = L7_NULLPTR;

  /* check input */
  if (maxEntries == 0)
  {
    /* don't allocate the table if ACL rule logging is not supported */
    return L7_SUCCESS;
  }
  else if (maxEntries > L7_ACL_LOG_RULE_LIMIT)
  {
    /* should never get a request for more than max log rules */
    return L7_FAILURE;
  }

  /* allocate storage for ACL correlator table AVL structures */
  pAclCorrTable_g = (avlTree_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                             (L7_uint32)sizeof(avlTree_t));

  pAclCorrTableTreeHeap_g = (avlTreeTables_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                           (L7_uint32)sizeof(avlTreeTables_t) * maxEntries);

  pAclCorrTableDataHeap_g = (aclCorrTable_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                         (L7_uint32)sizeof(aclCorrTable_t) * maxEntries);

  if ((pAclCorrTable_g == L7_NULLPTR) ||
      (pAclCorrTableTreeHeap_g == L7_NULLPTR) ||
      (pAclCorrTableDataHeap_g == L7_NULLPTR))
  {
    aclCorrTableDelete();
    return L7_FAILURE;
  }

  avlCreateAvlTree(pAclCorrTable_g, pAclCorrTableTreeHeap_g, pAclCorrTableDataHeap_g, maxEntries,
                   (L7_uint32)sizeof(aclCorrTable_t), 0x10, (L7_uint32)sizeof(L7_uint32));

  (void)avlSetAvlTreeComparator(pAclCorrTable_g, avlCompareULong32);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Deletes the ACL correlator table
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
void aclCorrTableDelete(void)
{
  if (pAclCorrTable_g != L7_NULLPTR)
  {
    /* using non-null semId as indication that AVL tree was created */
    if (pAclCorrTable_g->semId != L7_NULLPTR)
    {
      (void)avlDeleteAvlTree(pAclCorrTable_g);
    }
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pAclCorrTable_g);
    pAclCorrTable_g = L7_NULLPTR;
  }

  if (pAclCorrTableTreeHeap_g != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pAclCorrTableTreeHeap_g);
    pAclCorrTableTreeHeap_g = L7_NULLPTR;
  }

  if (pAclCorrTableDataHeap_g != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pAclCorrTableDataHeap_g);
    pAclCorrTableDataHeap_g = L7_NULLPTR;
  }
}


/*********************************************************************
*
* @purpose  Finds the specified correlator entry in ACL correlator table
*
* @param    correlator  @b{(input)}  correlator of entry to find
* @param    flags       @b{(input)}  search flags (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
*
* @returns  aclCorrTable_t *   Ptr to ACL correlator table entry, if found
* @returns  L7_NULLPTR, if entry not found
*
* @comments Use a correlator value of 0 to find first entry in table.
*
* @end
*
*********************************************************************/
aclCorrTable_t *aclCorrTableEntryFind(L7_uint32 correlator, L7_uint32 flags)
{
  aclCorrTable_t  *p = L7_NULLPTR;

  flags = (flags == L7_MATCH_GETNEXT) ? AVL_NEXT : AVL_EXACT;

  if (pAclCorrTable_g != L7_NULLPTR)
  {
    p = avlSearchLVL7(pAclCorrTable_g, &correlator, flags);
  }

  return (aclCorrTable_t *)p;
}

/*********************************************************************
*
* @purpose  Insert specified correlator entry in ACL correlator table
*
* @param    L7_uint32       correlator  rule correlator to be added
* @param    L7_uint32       searchFlags search flags
*
* @returns  L7_SUCCESS        correlator entry was inserted/updated
* @returns  L7_TABLE_IS_FULL  correlator table is full
* @returns  L7_FAILURE        all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclCorrTableEntryInsert(L7_uint32 correlator)
{
  aclCorrTable_t    *pEntry, newEntry;

  /* make sure ACL correlator table exists */
  if (pAclCorrTable_g == L7_NULLPTR)
    return L7_FAILURE;

  /* a correlator value of 0 means no correlator was used */
  if (correlator == 0)
    return L7_SUCCESS;

  memset(&newEntry, 0, sizeof(newEntry));
  newEntry.correlator = correlator;

  pEntry = avlInsertEntry(pAclCorrTable_g, &newEntry);

  if (pEntry == &newEntry)
  {
    /* no more room in table */
    return L7_TABLE_IS_FULL;
  }
  else if (pEntry == L7_NULLPTR)
  {
    /* new entry was successfully inserted -- retrieve its ptr */
    pEntry = aclCorrTableEntryFind(correlator, L7_MATCH_EXACT);
    if (pEntry == L7_NULLPTR)
      return L7_FAILURE;
  }
  else
  {
    /* found existing entry */
  }

  /* increment the table entry reference count */
  pEntry->refCt++;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Remove specified correlator entry from ACL correlator table
*
* @param    L7_uint32   correlator  rule correlator to be removed
*
* @returns  L7_SUCCESS  correlator entry was removed/updated
* @returns  L7_ERROR    correlator entry not found
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclCorrTableEntryRemove(L7_uint32 correlator)
{
  aclCorrTable_t    *pEntry;

  /* make sure ACL correlator table exists */
  if (pAclCorrTable_g == L7_NULLPTR)
    return L7_FAILURE;

  /* a correlator value of 0 means no correlator was used */
  if (correlator == 0)
    return L7_SUCCESS;

  /* find correlator entry in table */
  pEntry = aclCorrTableEntryFind(correlator, L7_MATCH_EXACT);
  if (pEntry == L7_NULLPTR)
    return L7_ERROR;

  /* decrement the table entry reference count */
  if (pEntry->refCt > 0)
    pEntry->refCt--;

  /* if the reference count is now zero, remove entry from table */
  if (pEntry->refCt == 0)
  {
    if (avlDeleteEntry(pAclCorrTable_g, pEntry) == L7_NULLPTR)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Displays contents of ACL correlator table
*
* @param    correlator  @b{(input)}  specific correlator, or 0 to show all
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void aclCorrTableShow(L7_uint32 correlator)
{
  L7_uint32         numEnt = 0;
  L7_uint32         msgLvlReqd;
  L7_uint32         flags, searchKey;
  aclCorrTable_t    *pEntry;

  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_ON;

  /* make sure ACL correlator table exists */
  if (pAclCorrTable_g == L7_NULLPTR)
  {
    ACL_PRT(msgLvlReqd, "\nACL correlator table does not exist\n\n");
    return;
  }

  /* a correlator value of 0 means display entire table */
  flags = (correlator == 0) ? L7_MATCH_GETNEXT : L7_MATCH_EXACT;

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    searchKey = correlator;                     /* to begin search */
    while ((pEntry = aclCorrTableEntryFind(searchKey, flags)) != L7_NULLPTR)
    {
      searchKey = pEntry->correlator;         /* update for next iteration */

      /* display entry contents */
      ACL_PRT(msgLvlReqd,
              "[%4u]  corr=0x%8.8x  refCt=0x%4.4x  queries=0x%4.4x\n",
              ++numEnt, pEntry->correlator, pEntry->refCt, pEntry->queries);

      /* quit if displaying a specific correlator entry */
      if (correlator != 0)
        break;

    } /* endwhile table search */

    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
  }
}

/*********************************************************************
*
* @purpose  Encodes an ACL rule correlator
*
* @param    aclType       @b{(input)}  access list type
* @param    aclId         @b{(input)}  access list identifier
* @param    rulenum       @b{(input)}  access list rule number
* @param    ruleAction    @b{(input)}  access list rule action
*
* @returns  L7_uint32     correlator value
*
* @comments The correlator value is treated as opaque by HAPI.
*
* @end
*
*********************************************************************/
L7_uint32 aclCorrEncode(L7_ACL_TYPE_t aclType, L7_uint32 aclId,
                        L7_uint32 rulenum, L7_ACL_ACTION_t ruleAction)
{
  L7_uint32     correlator, denyFlag;

  denyFlag = (L7_uint32)((ruleAction == L7_ACL_DENY) ? 1 : 0);

  /* create a correlator value from the inputs as follows:
   *   4 bits:  ACL type
   *  12 bits:  ACL id
   *  15 bits:  rule number
   *   1 bit :  rule action (0=permit, 1=deny)
   */
  correlator = ((L7_uint32)aclType << ACL_CORR_ACLTYPE_SHIFT) & ACL_CORR_ACLTYPE_MASK;
  switch (aclType)
  {
  case L7_ACL_TYPE_IP:
    {
      /* no adjustment needed */
      break;
    }
  case L7_ACL_TYPE_MAC:
    {
      aclId -= L7_ACL_MAC_MIN_INDEX;
      break;
    }
  case L7_ACL_TYPE_IPV6:
    {
      aclId -= L7_ACL_IPV6_MIN_INDEX;
      break;
    }
  default:
    break;
  }
  correlator |= ((aclId << ACL_CORR_ACLID_SHIFT) & ACL_CORR_ACLID_MASK);
  correlator |= ((rulenum << ACL_CORR_RULENUM_SHIFT) & ACL_CORR_RULENUM_MASK);
  correlator |= ((denyFlag << ACL_CORR_RULEACT_SHIFT) & ACL_CORR_RULEACT_MASK);

  return correlator;
}

/*********************************************************************
*
* @purpose  Decodes an ACL rule correlator
*
* @param    correlator    @b{(input)}  correlator value
* @param    *aclType      @b{(output)} access list type
* @param    *aclId        @b{(output)} access list identifier
* @param    *rulenum      @b{(output)} access list rule number
* @param    *ruleAction   @b{(output)} access list rule action
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      correlator value was null
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclCorrDecode(L7_uint32 correlator, L7_ACL_TYPE_t *aclType,
                      L7_uint32 *aclId, L7_uint32 *rulenum,
                      L7_ACL_ACTION_t *ruleAction)
{
  L7_uint32     denyFlag;

  /* check input ptrs */
  if ((aclType == L7_NULLPTR) || (aclId == L7_NULLPTR) ||
      (rulenum == L7_NULLPTR) || (ruleAction == L7_NULLPTR))
    return L7_FAILURE;

  /* this function should not be used for a null correlator value */
  if (correlator == L7_QOS_ACL_TLV_RULE_CORR_NULL)
    return L7_ERROR;

  /* create a correlator value from the inputs as follows:
   *   4 bits:  ACL type
   *  12 bits:  ACL id
   *  15 bits:  rule number
   *   1 bit :  rule action (0=permit, 1=deny)
   */
  *aclType = (L7_ACL_TYPE_t)((correlator & ACL_CORR_ACLTYPE_MASK) >> ACL_CORR_ACLTYPE_SHIFT);
  *aclId = (correlator & ACL_CORR_ACLID_MASK) >> ACL_CORR_ACLID_SHIFT;
  switch (*aclType)
  {
  case L7_ACL_TYPE_IP:
    {
      /* no adjustment needed */
      break;
    }
  case L7_ACL_TYPE_MAC:
    {
      *aclId += L7_ACL_MAC_MIN_INDEX;
      break;
    }
  case L7_ACL_TYPE_IPV6:
    {
      *aclId += L7_ACL_IPV6_MIN_INDEX;
      break;
    }
  default:
    break;
  }

  *rulenum = (correlator & ACL_CORR_RULENUM_MASK) >> ACL_CORR_RULENUM_SHIFT;
  denyFlag = (correlator & ACL_CORR_RULEACT_MASK) >> ACL_CORR_RULEACT_SHIFT;

  *ruleAction = (denyFlag == 0) ? L7_ACL_PERMIT : L7_ACL_DENY;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize ACL component callback support
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t aclCallbackInit(void)
{
  L7_uint32           maxEnt, memSize;
  void                *pMem;
  aclCallbackTable_t  *pTable;

  /* malloc space for ACL callback info structure  */
  pAclCallbackInfo_g =
    (aclCallbackInfo_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, sizeof(aclCallbackInfo_t));
  if (pAclCallbackInfo_g == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "%s: Unable to allocate ACL callback info structure\n", __FUNCTION__);
    return L7_FAILURE;
  }
  memset(pAclCallbackInfo_g, 0, sizeof(aclCallbackInfo_t));

  /* malloc space for the ACL Route Filter callback registration table */
  maxEnt = ACL_CALLBACK_REGISTRANTS_MAX_ROUTE_FILTER;
  memSize = maxEnt * sizeof(aclCallbackEntry_t);
  pMem = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, memSize);
  if (pMem == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "%s: Unable to allocate ACL Route Filter callback table\n", __FUNCTION__);
    return L7_FAILURE;
  }
  memset(pMem, 0, memSize);
  pTable = &pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_ROUTE_FILTER];
  pTable->pStart = (aclCallbackEntry_t *)pMem;
  pTable->maxEntries = maxEnt;

  /* malloc space for the ACL Notify Event callback registration table */
  maxEnt = ACL_CALLBACK_REGISTRANTS_MAX_NOTIFY_EVENT;
  memSize = maxEnt * sizeof(aclCallbackEntry_t);
  pMem = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, memSize);
  if (pMem == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "%s: Unable to allocate ACL Notify Event callback table\n", __FUNCTION__);
    return L7_FAILURE;
  }
  memset(pMem, 0, memSize);
  pTable = &pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_NOTIFY_EVENT];
  pTable->pStart = (aclCallbackEntry_t *)pMem;
  pTable->maxEntries = maxEnt;

  /* malloc space for the ACL Change Approval callback registration table */
  maxEnt = ACL_CALLBACK_REGISTRANTS_MAX_CHANGE_APPROVAL;
  memSize = maxEnt * sizeof(aclCallbackEntry_t);
  pMem = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, memSize);
  if (pMem == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "%s: Unable to allocate ACL Change Approval callback table\n", __FUNCTION__);
    return L7_FAILURE;
  }
  memset(pMem, 0, memSize);
  pTable = &pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_CHANGE_APPROVAL];
  pTable->pStart = (aclCallbackEntry_t *)pMem;
  pTable->maxEntries = maxEnt;

  /* set flag indicating ACL callback registration tables are ready for use */
  pAclCallbackInfo_g->isInitialized = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clean up ACL component callback support after a failed init
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void aclCallbackFini(void)
{
  aclCallbackTable_t  *pTable;

  if (pAclCallbackInfo_g != L7_NULLPTR)
  {
    pAclCallbackInfo_g->isInitialized = L7_FALSE;

    /* NOTE: None of the pStart ptrs are set to L7_NULLPTR since the
     *       entire callback info struct will be freed as well.
     */

    pTable = &pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_CHANGE_APPROVAL];
    if (pTable->pStart != L7_NULLPTR)
    {
      osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pTable->pStart);
    }

    pTable = &pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_NOTIFY_EVENT];
    if (pTable->pStart != L7_NULLPTR)
    {
      osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pTable->pStart);
    }

    pTable = &pAclCallbackInfo_g->table[ACL_CALLBACK_TABLE_ID_ROUTE_FILTER];
    if (pTable->pStart != L7_NULLPTR)
    {
      osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pTable->pStart);
    }

    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pAclCallbackInfo_g);
    pAclCallbackInfo_g = L7_NULLPTR;
  }
}

/*********************************************************************
*
* @purpose  ACL log task
*
* @param    void
*
* @returns  void
*
* @comments Collects ACL rule log counts from the hardware and presents
*           them via trap mechanism.
*
* @comments Not all platforms support ACL rule logging, so there are
*           cases where the ACL correlator table is not allocated and
*           this task is never started.
*
* @end
*
*********************************************************************/
void aclLogTask(void)
{
  L7_int32                            pollInterval, origPollInterval, sleepTime;
  aclCorrTable_t                      *pEntry;
  L7_uint32                           searchKey;
  L7_ulong64                          hitCount;
  SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t  trapData;
  L7_RC_t                             rc;

  /* sync up with initialization thread */
  (void)osapiTaskInitDone(L7_QOS_ACL_LOG_TASK_SYNC);


  /* main task body */
  while (1)
  {
    /* this task is designed to wake up frequently to check for changes
     * in the logging interval, although it only does real work each time
     * the logging interval expires
     */

    /* re-init these variables each pass */
    pollInterval = origPollInterval = aclLogTaskSleepSecs_g;
    sleepTime = FD_QOS_ACL_TASK_SLEEP_SECS;

    ACL_PRT(ACL_MSGLVL_LO_2,
            "\nACL Log Task idling for %d seconds (%d sec sleep interval)...\n",
            pollInterval, sleepTime);

    while (pollInterval > 0)
    {
      if (pollInterval < sleepTime)
        sleepTime = pollInterval;

      osapiSleep((L7_uint32)sleepTime);         /* seconds */

      pollInterval -= sleepTime;

      /* if logging interval has changed, do logging and start new interval */
      if (origPollInterval != aclLogTaskSleepSecs_g)
        pollInterval = 0;                       /* forces exit from sleep loop */
    }

    /*** it is now time to do the main task work ***/

    ACL_PRT(ACL_MSGLVL_LO_2, "\nACL Trap Task running...\n");

    /* make sure ACL correlator table exists...normally, this task is only
     * started if there is a table to process, but checking here just in case
     * the table was deallocated for some reason
     */
    if (pAclCorrTable_g == L7_NULLPTR)
      continue;

    if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
    {
      L7_BOOL   lockAcquired = L7_TRUE;

      searchKey = 0;                            /* use 0 to find first entry */
      while ((pEntry = aclCorrTableEntryFind(searchKey, L7_MATCH_GETNEXT)) != L7_NULLPTR)
      {
        searchKey = pEntry->correlator;         /* update for next iteration */

        /* skip this counter if cannot retrieve it from hardware */
        if (dtlQosAclRuleCountGet(pEntry->correlator, &hitCount) != L7_SUCCESS)
        {
          ACL_PRT(ACL_MSGLVL_LO_2,
                  "ACL Trap: corr=0x%8.8x  (DTL counter not available)\n",
                  pEntry->correlator);
          continue;
        }

        pEntry->queries++;

        /* hitCount is the delta since last interval, so issue a trap
         * if delta is non-zero
         */
        if ((hitCount.low != 0) || (hitCount.high != 0))
        {
          if (aclCorrDecode(pEntry->correlator,
                            &trapData.aclType,
                            &trapData.aclId,
                            &trapData.ruleNum,
                            &trapData.action) == L7_SUCCESS)
          {
            trapData.hitCount = hitCount;         /* this is a delta count */

            ACL_PRT(ACL_MSGLVL_LO_2,
                    "ACL Trap: corr=0x%8.8x  deltaCount=0x%8.8x-%8.8x\n",
                    pEntry->correlator, hitCount.high, hitCount.low);

            /* temporarily release the R/W lock for the trap processing to
             * avoid a possible deadlock condition
             */
            lockAcquired = L7_FALSE;
            (void)ACL_WRITE_LOCK_GIVE(aclRwLock);

            rc = trapMgrAclRuleLogEventTrap(&trapData);

            if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
              lockAcquired = L7_TRUE;

            /* check result of sending trap */
            if (rc != L7_SUCCESS)
            {
              L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_ACL_COMPONENT_ID,
                      "%s: error logging ACL rule trap for correlator 0x%8.8x."
                      " The system was unable to send an SNMP trap for this ACL "
                      "rule which contains a logging attribute.",
                      __FUNCTION__, pEntry->correlator);
            }

            /* if unable to reacquire the R/W lock, give up on this iteration */
            if (lockAcquired != L7_TRUE)
              break;
          }
        }
      } /* endwhile table search */

      if (lockAcquired == L7_TRUE)
      {
        (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "%s unable to obtain ACL write lock\n", __FUNCTION__);
    }

  } /* endwhile forever */
}

/*********************************************************************
*
* @purpose  Sets ACL message level value
*
* @param    sleepTime   @b{(input)}  task sleep time (in seconds)
*
* @returns  void
*
* @comments This is a debug function intended for ENGINEERING USE ONLY!
*
* @comments Setting a value of 0 restores the default sleep interval.
*
* @end
*
*********************************************************************/
void aclLogTaskSleepTimeSet(L7_int32 sleepSecs)
{
  /* an input value of 0 restores the default sleep interval */
  if (sleepSecs == 0)
    sleepSecs = (L7_int32)(FD_QOS_ACL_LOGGING_INTERVAL * 60);

  aclLogTaskSleepSecs_g = sleepSecs;
}


/**************************************************************************
* @purpose  Take an ACL semaphore
*
* @param    semId   @{(input)} Semaphore id
* @param    timeout @{(input)} Time to wait (in ticks), or L7_WAIT_FOREVER
*                                or L7_NO_WAIT
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiSemaTake() instead.
*
* @end
*************************************************************************/
L7_RC_t aclSemaTake(void *semId, L7_int32 timeout, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiSemaTake(semId, timeout);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "Semaphore take failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)semId);
  }
  return rc;
}

/**************************************************************************
* @purpose  Give an ACL semaphore
*
* @param    semId   @{(input)} Semaphore id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
L7_RC_t aclSemaGive(void *semId, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiSemaGive(semId);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "Semaphore give failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)semId);
  }
  return rc;
}

/**************************************************************************
* @purpose  Take an ACL read lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    timeout @{(input)} Time to wait (in ticks), or L7_WAIT_FOREVER
*                                or L7_NO_WAIT
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiReadLockTake() instead.
*
* @end
*************************************************************************/
L7_RC_t aclReadLockTake(osapiRWLock_t rwlock, L7_int32 timeout, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiReadLockTake(rwlock, timeout);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "Read lock take failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)rwlock.handle);
  }
  return rc;
}

/**************************************************************************
* @purpose  Give an ACL read lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
L7_RC_t aclReadLockGive(osapiRWLock_t rwlock, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiReadLockGive(rwlock);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "Read lock give failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)rwlock.handle);
  }
  return rc;
}

/**************************************************************************
* @purpose  Take an ACL write lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    timeout @{(input)} Time to wait (in ticks), or L7_WAIT_FOREVER
*                                or L7_NO_WAIT
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiWriteLockTake() instead.
*
* @end
*************************************************************************/
L7_RC_t aclWriteLockTake(osapiRWLock_t rwlock, L7_int32 timeout, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiWriteLockTake(rwlock, timeout);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "Write lock take failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)rwlock.handle);
  }
  return rc;
}

/**************************************************************************
* @purpose  Give an ACL write lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
L7_RC_t aclWriteLockGive(osapiRWLock_t rwlock, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiWriteLockGive(rwlock);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, "Write lock give failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)rwlock.handle);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Gets current ACL message level setting
*
* @param    void
*
* @returns  L7_uint32   message level value
*
* @comments The ACL message level does not persist across a system reset.
*
* @end
*
*********************************************************************/
L7_uint32 aclMsgLvlGet(void)
{
  return aclMsgLvl_g;
}

/*********************************************************************
*
* @purpose  Sets ACL message level value
*
* @param    L7_uint32   msgLvl
*
* @returns  void
*
* @comments The ACL message level does not persist across a system reset.
*
* @end
*
*********************************************************************/
void aclMsgLvlSet(L7_uint32 msgLvl)
{
  aclMsgLvl_g = msgLvl;

  /* the ACL msg lvl is saved in the config */
  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
}
 
/*********************************************************************
*
* @purpose  ACL Event Task
*
* @param    void
*
* @returns  void
*
* @comments Handles notifications form time range component and reinstalls a rule 
* as activated/deactivated. Generic enough to be extended for future enhancements
*
* @comments 
*
* @end
*
*********************************************************************/
void aclEventTask(void)
{
  L7_RC_t rc = L7_SUCCESS;
  aclEventMsg_t msg;
  L7_uint32 i=0;
  aclTRCorrEntry_t *aclTRCorrEntryTempNext = L7_NULLPTR;
  SNMP_QOS_ACL_RULE_TIMERANGE_EVENT_TRAP_t trapData;
  L7_uchar8 aclName[L7_ACL_NAME_LEN_MAX + 1];
  L7_ACL_TYPE_t aclType;
  /* sync up with initialization thread */
  (void)osapiTaskInitDone(L7_QOS_ACL_EVENT_TASK_SYNC);
   
   /* main task body */
  while (1)
  {
    /* this task currently handles notications posted by time ranges events, 
     * Reads notifcations posted for time range name and event information,
     * Searches aclTimeRangeTable to find if any of the rules referenced the time range,      
     * if match found update the status for the entry in table 
     * check the aclCorrEntryList for correlators associated with the Time range entry
     * Call DTL with correlator and status information, Reinstall the rule in hardware
     */
     ACL_PRT(ACL_MSGLVL_LO_2, "\nACL Event task running...\n");  
     
     /* Read the meesage from the aclTimeRangeQueue */

     if((osapiMessageReceive(aclEventQueue, &msg, (L7_uint32)ACL_EVENT_QUEUE_MSG_SIZE, L7_WAIT_FOREVER)) == L7_SUCCESS)
     {

       if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
       {
         L7_BOOL   lockAcquired = L7_TRUE;
          
         switch (msg.type)
         {
           case ACL_EVENT_TIME_RANGE_MSG:

                /* make sure that acl time range table exists */
                if(aclTimeRangeTable == L7_NULLPTR)
                continue;

                ACL_PRT(ACL_MSGLVL_LO_2, "\nACL Time range task,Received message from queue. "
                       " Start reading correlator table...\n");
  
                /* Read the time range name from the message */
               if (osapiStrnlen(msg.u.timeRangeMsg.timeRangeName, sizeof(msg.u.timeRangeMsg.timeRangeName))==0)
               {
                 continue; 
               }

               ACL_PRT(ACL_MSGLVL_LO_2,"\nACL Time range task, Message read, "
                       "timeRangeName %s length of timeRangeName %d",
                       msg.u.timeRangeMsg.timeRangeName,
                       strlen(msg.u.timeRangeMsg.timeRangeName));
               /* Read entries in aclTimeRangeTable table for  matching the time range name */
               for (i=0;i<L7_ACL_MAX_RULES;i++)
               {
                 if (osapiStrncmp(msg.u.timeRangeMsg.timeRangeName, aclTimeRangeTable[i].timeRangeName, 
                   sizeof(msg.u.timeRangeMsg.timeRangeName)) == 0)
                {
                  ACL_PRT(ACL_MSGLVL_LO_2,"\n ACL Time range task, "
                          "Match found timerange %s",
                           __FUNCTION__, __LINE__, 
                          aclTimeRangeTable[i].timeRangeName);
                 /*update the status for time range entry in table*/                
              
                 switch (msg.u.timeRangeMsg.event)
                 {
                   case L7_TIMERANGE_EVENT_ACTIVATE:
                   case L7_TIMERANGE_EVENT_DELETE:
                        aclTimeRangeTable[i].status = L7_ACL_RULE_STATUS_ACTIVE;
                        break;
                   case L7_TIMERANGE_EVENT_DEACTIVATE:  
                        aclTimeRangeTable[i].status = L7_ACL_RULE_STATUS_INACTIVE;
                        break;
                   default:
                        break;
                 }
                 /* Activate/Deactivate the rule */
                 if (aclTimeRangeTable[i].aclCorrEntryList != L7_NULLPTR)
                 {
                   aclTRCorrEntryTempNext = aclTimeRangeTable[i].aclCorrEntryList;
                   do 
                   {
                     aclCorrDecode(aclTRCorrEntryTempNext->corrId, 
                               &trapData.aclType, &trapData.aclId, 
                               &trapData.ruleNum, &trapData.action);                     
                   
                     osapiStrncpySafe(trapData.timeRangeName,aclTimeRangeTable[i].timeRangeName, sizeof(trapData.timeRangeName));
                     if (aclImpCommonNameStringGet(trapData.aclId, &aclType, aclName) != L7_SUCCESS)
                     {
                       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               "Failed to retrieve the ACL name for ACL ID %d "
                               "and ACL type %d", trapData.aclId, trapData.aclType);
                     }
                     /* update the status for each correlator entry in list */
                     switch (msg.u.timeRangeMsg.event)
                    {
                      case L7_TIMERANGE_EVENT_ACTIVATE:
                           trapData.notification = L7_TIMERANGE_EVENT_ACTIVATE;
                           if (dtlQosAclRuleStatusSet(aclTRCorrEntryTempNext->corrId, L7_ACL_RULE_STATUS_ACTIVE) != L7_SUCCESS)
                           {                            
                             L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      "%s: Unable to install ACL \"%s\", Rule %d as activated in hardware "
                                      "for time based ACL rule associated with time range \"%s\" "
                                      "on which there is a L7_TIMERANGE_EVENT_ACTIVATE notification",
                                      __FUNCTION__, aclName, trapData.ruleNum, 
                                      aclTimeRangeTable[i].timeRangeName);
                             trapData.status = L7_FAILURE;
                           }
                           else
                           {                                                      
                             L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      "Successful installation of ACL \"%s\", Rule %d as activated in hardware "
                                      "for time based ACL rule associated with time range \"%s\" "
                                      "on which there is a L7_TIMERANGE_EVENT_ACTIVATE notification",
                                      aclName, trapData.ruleNum, aclTimeRangeTable[i].timeRangeName);
                             trapData.status = L7_SUCCESS;
                           }
                           break;
                      case L7_TIMERANGE_EVENT_DEACTIVATE:
                           trapData.notification = L7_TIMERANGE_EVENT_DEACTIVATE;
                           if (dtlQosAclRuleStatusSet(aclTRCorrEntryTempNext->corrId, L7_ACL_RULE_STATUS_INACTIVE)!= L7_SUCCESS)                                                        
                           {
                             L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      "%s: Unable to install ACL \"%s\", Rule %d as deactivated in hardware " 
                                      "for time based ACL rule associated with time range \"%s\" "
                                      "on which there is a L7_TIMERANGE_EVENT_DEACTIVATE notification",
                                      __FUNCTION__, aclName, trapData.ruleNum, aclTimeRangeTable[i].timeRangeName);
                             trapData.status = L7_FAILURE;
                           }
                           else
                           {
                             L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      "Successful installation of ACL \"%s\", Rule %d as deactivated "
                                       "in hardware for time based ACL rule associated with time range \"%s\" "
                                      "on which there is a L7_TIMERANGE_EVENT_DEACTIVATE notification",
                                       aclName, trapData.ruleNum, aclTimeRangeTable[i].timeRangeName);
                             trapData.status = L7_SUCCESS;
                           }
                           break;
                      case L7_TIMERANGE_EVENT_DELETE:
                           trapData.notification = L7_TIMERANGE_EVENT_DELETE;
                           if (dtlQosAclRuleStatusSet(aclTRCorrEntryTempNext->corrId, L7_ACL_RULE_STATUS_ACTIVE)!= L7_SUCCESS)                                                       
                           {
                             L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      "%s: Unable to install ACL \"%s\", Rule %d as activated in hardware "
                                      "for time based ACL rule associated with time range \"%s\" "
                                      "on which there is a L7_TIMERANGE_EVENT_DELETE notification",
                                      __FUNCTION__, aclName, trapData.ruleNum,aclTimeRangeTable[i].timeRangeName);
                             trapData.status = L7_FAILURE;
                           }
                           else
                           {
                             L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     "Successful installation of ACL \"%s\", Rule %d as activated " 
                                     "in hardware for time based ACL rule associated with "
                                     "time range \"%s\" on which there is a L7_TIMERANGE_EVENT_DELETE notification",
                                      aclName, trapData.ruleNum, aclTimeRangeTable[i].timeRangeName);
                             trapData.status = L7_SUCCESS;
                            }                           
                      
                           break;
                       default:
                           break;
                     }
                     /* trap */
                     /* temporarily release the R/W lock for the trap processing to
                      * avoid a possible deadlock condition
                      */
                     lockAcquired = L7_FALSE;
                     (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
                   
                     rc = trapMgrAclRuleTimeRangeEventTrap(&trapData);                         
                   
                     if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
                     lockAcquired = L7_TRUE;

                     /* check result of sending trap */
                     if (rc != L7_SUCCESS)
                     {
                       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               "%s: error Time Based ACL rule trap for correlator 0x%8.8x."
                               " The system was unable to send an SNMP trap for this time based ACL rule",
                               __FUNCTION__, aclTRCorrEntryTempNext->corrId);
                     }

                     aclTRCorrEntryTempNext = aclTRCorrEntryTempNext->next;

                   }while(aclTRCorrEntryTempNext != L7_NULLPTR);

                 } /* End of check for aclCorrEntryList is L7_NULLPTR */              

                 /* once an entry matches there will be no more entries matching for same time range name 
                  * in the aclTimeRangeTable and hence break the loop for iterating all entries in table
                  */
                 break;

               } /* End of Time range name matches */
             }/* End of for loop */
             if (lockAcquired == L7_TRUE)
             {
                 (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
             }
             break;
          default:               
             break;
        } /*end of switch */
      } 
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                "%s unable to obtain ACL write lock\n", __FUNCTION__);
      }
    } /* end of  message receive*/    

  } /* endwhile */  
}

/****************************************************************************
* @purpose  call back routine that post the time range events to aclEventQueue
* @param    *name        @b{(input)} timeRangeName name of time range on
*                                    which there is a event
*
* @param    *event       @b{(input)} time range events  as listed in
*                                    timeRangeEvent_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t aclTimeRangeEventCallback(L7_uchar8 *timeRangeName, timeRangeEvent_t  event)
{
  aclEventMsg_t msg;  
  L7_RC_t rc;
  
  if(timeRangeName == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  
  memset(msg.u.timeRangeMsg.timeRangeName, 0, sizeof(msg.u.timeRangeMsg.timeRangeName));

  msg.type = ACL_EVENT_TIME_RANGE_MSG;
  osapiStrncpySafe(msg.u.timeRangeMsg.timeRangeName, timeRangeName, 
                   sizeof(msg.u.timeRangeMsg.timeRangeName));

  if(event == TIMERANGE_EVENT_START )
  {
     msg.u.timeRangeMsg.event = L7_TIMERANGE_EVENT_ACTIVATE;
  }
  else if(event == TIMERANGE_EVENT_END )
  {
     msg.u.timeRangeMsg.event = L7_TIMERANGE_EVENT_DEACTIVATE;
  }
  else if(event == TIMERANGE_EVENT_DELETE)
  {
     msg.u.timeRangeMsg.event = L7_TIMERANGE_EVENT_DELETE;
  }
  else
  {
     return L7_FAILURE;
  }

  /* post the message to message queue*/
  rc = osapiMessageSend(aclEventQueue, &msg, (L7_uint32)ACL_EVENT_QUEUE_MSG_SIZE,
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  return rc;
}

/*********************************************************************
*
* @purpose Initializes the Time Range Table to defaults
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*********************************************************************/
void aclTimeRangeTableInitialize()
{
  L7_uint32 i=0;

  for(i=0; i<L7_ACL_MAX_RULES; i++)
  {
    memset(aclTimeRangeTable[i].timeRangeName, 0, sizeof(aclTimeRangeTable[i].timeRangeName));
    aclTimeRangeTable[i].refCount =0;
    aclTimeRangeTable[i].status = L7_ACL_RULE_STATUS_NONE;
    aclTimeRangeTable[i].aclCorrEntryList = L7_NULLPTR;
  }
}

/*********************************************************************
*
* @purpose Updates or creates Time Range entry for a given
*          Time Range Id.
*
* @param   timeRangeName @b{ (input) } Time Range Name
* @param   TRStatus      @b{ (input) } Time Range Status from Timezone component.
* @param   TRId          @b{ (output) } Time Range Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This is called whenever a Time Based ACL rule is created
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableEntryUpdate(L7_uchar8 *timeRangeName,L7_uint32 status, L7_uint32 *TRId)
{
  L7_uint32 i=0;
  L7_uint32 freeSlot = 0;
  L7_BOOL freeSlotFound = L7_FALSE;

  /*Add the entry if there is no entry added for time range name*/
  for (i=0; i<L7_ACL_MAX_RULES; i++)
  {
    if (osapiStrncmp(aclTimeRangeTable[i].timeRangeName,timeRangeName, sizeof(aclTimeRangeTable[i].timeRangeName)) ==0)
    {
      /* entry already existing with the time range name
       * increment the refCount, update the status
       */
      aclTimeRangeTable[i].status = status;
      aclTimeRangeTable[i].refCount++;
      *TRId = i;      
      return L7_SUCCESS;     
    }
    else
    {
      if(aclTimeRangeTable[i].refCount == 0)
      {
        if (freeSlotFound != L7_TRUE) 
        {
           freeSlotFound = L7_TRUE;
           freeSlot = i; 
        }
      }
      continue;
    }
  }
  if (freeSlotFound !=L7_TRUE)
  {
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ACL_COMPONENT_ID,
            "aclTimeRangeTableEntryUpdate: Failed to insert the time range entry into aclTimeRangeTable."
              " Table is full with maximum entries %d",L7_ACL_MAX_RULES);
     ACL_PRT(ACL_MSGLVL_LO_2,
            "aclTimeRangeTableEntryUpdate: Failed to insert the time range entry into aclTimeRangeTable."
             " Table is full with maximum entries %d",L7_ACL_MAX_RULES);
     return L7_FAILURE;

  }
  else
  {     
    osapiStrncpy(aclTimeRangeTable[freeSlot].timeRangeName,
                 timeRangeName,
                 sizeof(aclTimeRangeTable[freeSlot].timeRangeName));
    aclTimeRangeTable[freeSlot].status = status;
    aclTimeRangeTable[freeSlot].refCount++;
    *TRId = freeSlot;
    return L7_SUCCESS;
  }
  
}

/*********************************************************************
*
* @purpose Get the status of Time Range Entry
*
* @param   TRId       @b{ (input) } Time Range Id
* @param   *status    @b{ (output) } status of the time range
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Creates or updates the existing entry in aclTimeRangeTable
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableEntryStatusGet(L7_uint32 TRId, L7_uint32 *status)
{
  if (TRId >=L7_ACL_MAX_RULES)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ACL_COMPONENT_ID,
            "aclTimeRangeTableEntryStatusGet: Time Range Index (%d) crossed the MAX ACL limit (%d)",
            TRId, L7_ACL_MAX_RULES);
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeRangeTableEntryStatusGet: trId (%d) crossed max limit(%d)", 
            TRId, L7_ACL_MAX_RULES);
    return L7_FAILURE;
  }
  if (aclTimeRangeTable[TRId].refCount == 0)
  {
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeRangeTableEntryStatusGet: trId (%d) entry is not valid ",
    TRId);
    return L7_FAILURE;
  }
  *status = aclTimeRangeTable[TRId].status;
  return L7_SUCCESS;  
}
/*********************************************************************
*
* @purpose Get the name of Time Range Entry
*
* @param   TRId              @b{ (input) } Time Range Id
* @param   *timeRangeName    @b{ (output) } Time Range Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Get the time range name for associated with TrId entry in table
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableEntryTRNameGet(L7_uint32 TRId, L7_uchar8 *timeRangeName)
{
  if (TRId >=L7_ACL_MAX_RULES)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ACL_COMPONENT_ID,
            "aclTimeRangeTableEntryTRNameGet: Time Range Index (%d) crossed the MAX ACL limit (%d)",
            TRId, L7_ACL_MAX_RULES);
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeRangeTableEntryTRNameGet: trId (%d) crossed max limit(%d)",
            TRId, L7_ACL_MAX_RULES);
    return L7_FAILURE;
  }

  if (aclTimeRangeTable[TRId].refCount == 0)
  {
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeRangeTableEntryTRNameGet: trId (%d) entry is not valid ",
    TRId);
    return L7_FAILURE;
  }

  osapiStrncpy(timeRangeName, aclTimeRangeTable[TRId].timeRangeName,L7_TIMERANGE_NAME_LEN_MAX+1);
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Updates or Delete Time Range information for a given
*          Time Range Id.
*
* @param   TRId       @b{ (input) } Time Range Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Reset the entry to defaults if refCount is zero 
*          otherwise decrement the refCount  
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableEntryDelete(L7_uint32 TRId)
{
  if (TRId >=L7_ACL_MAX_RULES)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ACL_COMPONENT_ID,
            "aclTimeRangeTableEntryDelete: Time Range Index (%d) crossed the MAX ACL limit (%d)",
            TRId, L7_ACL_MAX_RULES);
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeRangeTableEntryDelete: trId (%d) crossed max limit(%d)",
                      TRId, L7_ACL_MAX_RULES);
    return L7_FAILURE;
  }
  aclTimeRangeTable[TRId].refCount--;
  /* if refCount is non zero indicates there might be rules using same time range name
   * delete the entry from table only
   * when refCount reaches zero
   */
  if (aclTimeRangeTable[TRId].refCount ==0)
  {
    /* list should have been null by the time this function is executed */
    if (aclTimeRangeTable[TRId].aclCorrEntryList != L7_NULLPTR)
    {
       return L7_FAILURE;
    }

    memset(aclTimeRangeTable[TRId].timeRangeName,0,sizeof(aclTimeRangeTable[TRId].timeRangeName));
    aclTimeRangeTable[TRId].status =L7_ACL_RULE_STATUS_NONE;
    aclTimeRangeTable[TRId].aclCorrEntryList = L7_NULLPTR;
  }
  return L7_SUCCESS; 
}

/*********************************************************************
*
* @purpose Create a new Correlator node      
*
* @param   TRId              @b{ (input) } Time Range Id
* @param   corrId            @b{ (input) } Correlator Id
* @param   aclTRCorrEntryPtr @b{ (output) } New node created
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  Create a  new correlator note and initialize to defaults
*
* @end
*********************************************************************/
L7_RC_t aclTimeTangeTableCorrEntryCreate(L7_uint32 TRId,L7_uint32 corrId, aclTRCorrEntry_t **aclTRCorrEntryPtr)
{
  aclTRCorrEntry_t *aclTRCorrEntryTemp = L7_NULLPTR;
 
  /* Get a free correlator entry from the correlator list. */
  if (aclTRCorrEntryGet(&aclTRCorrEntryTemp) != L7_SUCCESS)
  {
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeTangeTableCorrEntryCreate trId (%d) corrId %d failed",
                       TRId,corrId);
    return L7_FAILURE;
  }
  aclTRCorrEntryTemp->corrId = corrId;
  aclTRCorrEntryTemp->trId = TRId;
  aclTRCorrEntryTemp->refCount =0;
  aclTRCorrEntryTemp->next = L7_NULLPTR;
  aclTRCorrEntryTemp->prev = L7_NULLPTR;
  *aclTRCorrEntryPtr = aclTRCorrEntryTemp;
  return L7_SUCCESS; 
}

/*********************************************************************
*
* @purpose Add the correaltor for trId list
*
* @param   TRId         @b{ (input) } Time Range Id
* @param   corrEntry    @b{ (input) } node to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  if node is already added to the list increments
*         the refCount maintained in correlator node
*         Adds new node to top of list
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableCorrEntryAdd(L7_uint32 TRId, aclTRCorrEntry_t *corrEntry)
{
  if (TRId >=L7_ACL_MAX_RULES)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ACL_COMPONENT_ID,
            "aclTimeRangeTableCorrEntryAdd: Time Range Index (%d) crossed the MAX ACL limit (%d)",
            TRId, L7_ACL_MAX_RULES);
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeRangeTableCorrEntryAdd: trId (%d) crossed max limit(%d)",
                       TRId, L7_ACL_MAX_RULES);
    return L7_FAILURE;
  }
  if (corrEntry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if(corrEntry->refCount ==0)
  {
    aclTRCorrEntryListAdd(TRId, corrEntry);
  }
  corrEntry->refCount++;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Add the correaltor for trId List
*
* @param   TRId         @b{ (input) } Time Range Id
* @param   corrEntry    @b{ (input) } node to be added
*
* @returns  
*
* @notes   Adds new node to top of list
*
* @end
*********************************************************************/
void aclTRCorrEntryListAdd(L7_uint32 TRId, aclTRCorrEntry_t *corrEntry)
{ 
  aclTRCorrEntry_t *aclTRCorrEntryTempNext = L7_NULLPTR;
  
  if (aclTimeRangeTable[TRId].aclCorrEntryList == L7_NULLPTR)  
  {
    /* no list exists*/
    aclTimeRangeTable[TRId].aclCorrEntryList = corrEntry;
  }
  else
  {
    /* more then one correlator present in the table */
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTRCorrEntryListAdd: More than one correlator present for TRId(%d)", TRId);
    aclTRCorrEntryTempNext = aclTimeRangeTable[TRId].aclCorrEntryList;
    corrEntry->next = aclTRCorrEntryTempNext;
    aclTRCorrEntryTempNext->prev = corrEntry;
    corrEntry->prev = L7_NULLPTR;
    aclTimeRangeTable[TRId].aclCorrEntryList = corrEntry;          
  }
}
/*********************************************************************
*
* @purpose Delete the correlator node from trId list
*
* @param   TRId         @b{ (input) } Time Range Id
* @param   corrEntry    @b{ (input) } node to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Decrement the refCount maintained in node, if refCount is
*          zero initialize the correlator to invalid value and remove from
*          the list
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableCorrEntryDelete(L7_uint32 TRId,aclTRCorrEntry_t *corrEntry)
{
  if (TRId >= L7_ACL_MAX_RULES)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ACL_COMPONENT_ID,
            "aclTimeRangeTableCorrEntryDelete: Time Range Index (%d) crossed the MAX ACL limit (%d)",
            TRId, L7_ACL_MAX_RULES);
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeRangeTableCorrEntryDelete: TRId (%d) crossed max limit(%d)",
                      TRId, L7_ACL_MAX_RULES);
    return L7_FAILURE;
  }

  if (corrEntry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (aclTimeRangeTable[TRId].aclCorrEntryList == L7_NULLPTR)
  {   
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeRangeTableCorrEntryDelete:TRId (%d) Corr List list is empty", 
                       TRId);
    return L7_SUCCESS;
  }
 
  /* stale entries */
  if (corrEntry->corrId == L7_QOS_ACL_TLV_RULE_CORR_NULL)
  {
     return L7_SUCCESS;
  }

  /* decrement the node refCount
   * if refCount is zero remove node from list 
   */
  if (corrEntry->refCount != 0)
  {
    corrEntry->refCount--;
  }  
  if(corrEntry->refCount == 0)
  {
    /* initialize the corrId to L7_QOS_ACL_TLV_RULE_CORR_NULL
     * Correlator is initialized only when rule is removed from interface/VLAN
     * but not incase of when correlatoe node is relocated
     */
    corrEntry->corrId  = L7_QOS_ACL_TLV_RULE_CORR_NULL;    
    aclTRCorrEntryListDelete(TRId, corrEntry);
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Delete the correaltor node from trId list
*
* @param   TRId         @b{ (input) } Time Range Id
* @param   corrEntry    @b{ (input) } node to be added
*
* @returns  
*
* @notes   Delete the node from list
*
* @end
*********************************************************************/
void aclTRCorrEntryListDelete(L7_uint32 TRId,aclTRCorrEntry_t *corrEntry)
{                
  aclTRCorrEntry_t *aclTRCorrEntryTempNext = L7_NULLPTR;
  
  /* Remove the node from list and add to free pool */
  if (aclTimeRangeTable[TRId].aclCorrEntryList->next == L7_NULLPTR)
  {
    /* if there is only one entry in the list*/
    aclTRCorrEntryTempNext = aclTimeRangeTable[TRId].aclCorrEntryList;    
    if (aclTRCorrEntryTempNext->corrId == corrEntry->corrId)
    {
         
      /* reset next and prev pointers of node*/
      corrEntry->prev = L7_NULLPTR;
      corrEntry->next = L7_NULLPTR; 
      aclTimeRangeTable[TRId].aclCorrEntryList = L7_NULLPTR;
      ACL_PRT(ACL_MSGLVL_LO_2,"aclTRCorrEntryListDelete: Deleted TRId %d corrId %d", 
                            TRId,corrEntry->corrId);
    }
      
  }    
  else
  {
    if (corrEntry->prev == L7_NULLPTR)
    {
      /* Special handling first node is removed from list 
      * move the aclCorrEntryList pointer 
      */
      aclTimeRangeTable[TRId].aclCorrEntryList = corrEntry->next;
    }
    else
    {
      corrEntry->prev->next = corrEntry->next;
    }      
    if (corrEntry->next !=L7_NULLPTR)
    {
        corrEntry->next->prev = corrEntry->prev;
    }     
    corrEntry->prev = L7_NULLPTR;
    corrEntry->next = L7_NULLPTR;
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTRCorrEntryListDelete: Deleted TRId (%d) corrId(%d)", 
                           TRId, corrEntry->corrId);
  }      
}
/*********************************************************************
*
* @purpose Detach and attach correlator node new trId
*
* @param   prevTRId     @b{ (input) } Previous Time Range Id
* @param   currTRId     @b{ (input) } Current Time Range Id
* @param   corrEntry    @b{ (input) } Correlator node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  Detach node from list of prevTRId and attach to currTRId
*
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableCorrEntryRelocate(L7_uint32 prevTRId, L7_uint32 currTRId, aclTRCorrEntry_t *corrEntry)
{
  if ((prevTRId >= L7_ACL_MAX_RULES) || (currTRId >=L7_ACL_MAX_RULES))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_ACL_COMPONENT_ID,
            "aclTimeRangeTableCorrEntryRelocate: Time Range Index prevTRId (%d) "
            " or currTRId %d crossed the MAX ACL limit (%d)",
            prevTRId, currTRId, L7_ACL_MAX_RULES);
    ACL_PRT(ACL_MSGLVL_LO_2,"aclTimeRangeTableCorrEntryAttachOrDetach: either of "
            "prevTRId (%d)  currTRId (%d)  crossed the MAX ACL limit (%d)",
                      prevTRId, currTRId, L7_ACL_MAX_RULES);
    return L7_FAILURE;
  }

  if(corrEntry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if(corrEntry->refCount ==0)
  {
     /* Correlator node is not attached to list*/
     return L7_SUCCESS;
  } 
  /* detach the node from current prevTRId and Attach to currTRId */
  aclTRCorrEntryListDelete(prevTRId, corrEntry);
  aclTRCorrEntryListAdd(currTRId, corrEntry);   
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Initializes the Time Range Correlator Entry List to defaults
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*********************************************************************/

void aclTRCorrEntryListInitialize()
{
  L7_uint32 temp = L7_NULL;

  memset (aclTRCorrEntryList, 0, (L7_uint32)(sizeof(aclTRCorrEntry_t)*(L7_ACL_MAX_RULES)));
  /* aclTRCorrEntryListFree always contains the first free location available. */
  aclTRCorrEntryListFree = aclTRCorrEntryList;

  /* Initialize all next pointer to the next entry.*/
  for (temp = 0; temp < (L7_ACL_MAX_RULES); temp++)
  {
    aclTRCorrEntryList[temp].next = &(aclTRCorrEntryList[temp+1]);
  }
}
/*********************************************************************
*
* @purpose Gets a free correlaotr entry from Free List.
*
* @param   aclTRCorrEntryPtr    @b{ (output) } correlator pointer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aclTRCorrEntryGet(aclTRCorrEntry_t **aclTRCorrEntryPtr)
{
  L7_uint32 msgLvlReqd;
  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_LO_2;
 
  aclTRCorrEntry_t *aclTRCorrEntryTemp = L7_NULLPTR;

  if (aclTRCorrEntryPtr == L7_NULLPTR ||
      aclTRCorrEntryListFree == L7_NULLPTR)
  {
    ACL_PRT(ACL_MSGLVL_LO_2,"Invalid input parameters.");
    return L7_FAILURE;
  }

  aclTRCorrEntryTemp = aclTRCorrEntryListFree;
  aclTRCorrEntryListFree = aclTRCorrEntryListFree->next;
  *aclTRCorrEntryPtr = aclTRCorrEntryTemp;
  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Sets back a used correlaotr entry to Free List.
*
* @param   aclTRCorrEntryPtr    @b{ (output) } correlator pointer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aclTRCorrEntryFree(aclTRCorrEntry_t *aclTRCorrEntry)
{
  L7_uint32 msgLvlReqd;
  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_LO_2;
  if (aclTRCorrEntry == L7_NULLPTR)
  {
    ACL_PRT(ACL_MSGLVL_LO_2,"Invalid input parameters");
    return L7_FAILURE;
  }
  /* Push the released entry to the top of the freed list. */
  memset(aclTRCorrEntry, 0 , sizeof(aclTRCorrEntry_t));
  aclTRCorrEntry->next = aclTRCorrEntryListFree;
  aclTRCorrEntryListFree = aclTRCorrEntry;

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Displays contents of ACL aclTimeRangeNameTable table
*
* @param    
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void aclTimeRangeTableShow()
{
  L7_uint32 i;
  L7_uint32 msgLvlReqd;
  L7_uint32 numEnt =0;
  L7_uint32 corrEnt =0;
  aclTRTableEntry_t entry;
  aclTRCorrEntry_t *aclTRCorrEntryNext = L7_NULLPTR;

  /* always display output for this function */
  msgLvlReqd = ACL_MSGLVL_ON;

  if(aclTimeRangeTable == L7_NULLPTR)
  {
    ACL_PRT(msgLvlReqd, "\nACL Time Range entry table does not exist\n\n");
    return;
  }

  if (ACL_WRITE_LOCK_TAKE(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
  {    
    for(i=0;i<L7_ACL_MAX_RULES;i++)
    {
     
      /* print each entry in the table */
      entry = aclTimeRangeTable[i];
      if((entry.refCount !=0) && (osapiStrnlen(entry.timeRangeName, sizeof(entry.timeRangeName)) !=0))
      {          
        ACL_PRT(msgLvlReqd,
               "[%4u] trId=%d timeRangeName=%s refCount=0x%4.4x status=%d\n",
               numEnt++,i,entry.timeRangeName,entry.refCount, entry.status);
        /* print each correlator node in the list*/
        if(entry.aclCorrEntryList !=L7_NULLPTR)
        {
           aclTRCorrEntryNext = entry.aclCorrEntryList;
           do
           {
              ACL_PRT(msgLvlReqd,
                      "[%4u] correlator=0x%8.8x trId=%d refCount=0x%4.4x\n",
                      corrEnt++, aclTRCorrEntryNext->corrId, aclTRCorrEntryNext->trId, 
                       aclTRCorrEntryNext->refCount); 
             aclTRCorrEntryNext = aclTRCorrEntryNext->next;
          }while(aclTRCorrEntryNext !=L7_NULLPTR);

        }/* End of Correlator node search*/

      } /*END of Valid entry search */

    }/*END of table search */
    (void)ACL_WRITE_LOCK_GIVE(aclRwLock);
  }
}



