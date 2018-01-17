/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_admin_scope.c
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "l7sll_api.h"
#include "mcast_bitset.h"
#include "pimdm_defs.h"
#include "pimdm_admin_scope.h"
#include "pimdm_util.h"
#include "pimdm_mrt.h"
#include "pimdm_debug.h"

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/
static L7_int32
pimdmMrtAdminScopeEntryCompare (void* pData1,
                                void* pData2,
                                L7_uint32 size);

static pimdmASBNode_t*
pimdmAdminScopeNodeGet (pimdmCB_t *pimdmCB,
                        L7_inet_addr_t *grpAddr,
                        L7_inet_addr_t *grpMask);

static L7_RC_t
pimdmAdminScopeBoundaryNodeDelete (L7_sll_member_t *asbNode);

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose   Compare function for the Admin Scope List
*
* @param     pData1 @b{ (input) } Pointer to Admin Scope Entry
*            pData2 @b{ (input) } Pointer to Admin Scope Entry
*            size   @b{ (input) } Size for the comparision 
*                            
* @returns   > 0  if pData1 > pData2
*            = 0 if pData1 == pData2
*            < 0 if pData1 < pData2
*
* @comments  None
*
* @end
*********************************************************************/
static L7_int32
pimdmMrtAdminScopeEntryCompare (void* pData1,
                                void* pData2,
                                L7_uint32 size)
{
  L7_sll_member_t *pKey1 = (L7_sll_member_t *) pData1;
  L7_sll_member_t *pKey2 = (L7_sll_member_t *) pData2;
  register L7_int32 retVal = 0;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  if ((retVal = L7_INET_ADDR_COMPARE (&(((pimdmASBNode_t*)pKey1)->grpAddr),
                                      &(((pimdmASBNode_t*)pKey2)->grpAddr))) != 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_API, "retVal is %d",retVal);
    return retVal;
  }

  retVal = L7_INET_ADDR_COMPARE (&(((pimdmASBNode_t*)pKey1)->grpMask), 
                                 &(((pimdmASBNode_t*)pKey2)->grpMask));

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");*/
  return retVal;
}

/*********************************************************************
*
* @purpose  Get the Adminscope Boundary Node for a Group
*
* @param    pimdmCB        @b{ (input) } Pointer to the PIM-DM Ctrl Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           grpMask        @b{ (input) } Mask of the Multicast Group
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static pimdmASBNode_t*
pimdmAdminScopeNodeGet (pimdmCB_t *pimdmCB,
                        L7_inet_addr_t *grpAddr,
                        L7_inet_addr_t *grpMask)
{
  pimdmASBNode_t  asbCurrNode;
  
  memset(&asbCurrNode,0,sizeof(pimdmASBNode_t));
  inetCopy (&(asbCurrNode.grpAddr), grpAddr);
  inetCopy (&(asbCurrNode.grpMask), grpMask);

  return (pimdmASBNode_t *)SLLFind(&pimdmCB->asbList,
                                  (L7_sll_member_t *)&asbCurrNode);
}

/*********************************************************************
*
* @purpose  Delete a Admin Scope Boundary Node
*
* @param    pimdmCB        @b{ (input) } Pointer to the PIM-DM Ctrl Block
* @param    asbNode        @b{ (input) } Pointer to the Admin Scope node
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*
* @end
*********************************************************************/
static L7_RC_t
pimdmAdminScopeBoundaryNodeDelete (L7_sll_member_t *asbNode)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  L7_uchar8 addrFamily = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (asbNode == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid asbNode");
    return L7_FAILURE;
  }

  addrFamily = L7_INET_GET_FAMILY (&(((pimdmASBNode_t*)asbNode)->grpAddr));

  if ((pimdmCB = pimdmMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to get PIM-DM Ctrl Block");
    return L7_FAILURE;
  }

  PIMDM_FREE (pimdmCB->addrFamily, (void*) asbNode);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Initialize the AdminScope Boundary database
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmAdminScopeBoundaryInit (pimdmCB_t *pimdmCB)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Initialize the Admin Scope SLL */
  if (SLLCreate (L7_FLEX_PIMDM_MAP_COMPONENT_ID, L7_SLL_NO_ORDER,
                 L7_NULL ,pimdmMrtAdminScopeEntryCompare,
                 pimdmAdminScopeBoundaryNodeDelete, &(pimdmCB->asbList))
              != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Admin Scope List Creation Failure");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "AdminScope Database Creation Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-initialize the AdminScope Boundary Database
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmAdminScopeBoundaryDeInit (pimdmCB_t *pimdmCB)
{
  L7_RC_t retVal = L7_SUCCESS;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Destory the Admin Scope SLL */
  if (SLLDestroy (L7_FLEX_PIMDM_MAP_COMPONENT_ID, &(pimdmCB->asbList))
               != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Admin Scope List Destroy Failure");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "AdminScope Database Destroy Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Set the Adminscope Boundary for a Group on an Interface
*
* @param    pimdmCB        @b{ (input) } Pointer to the PIM-DM Ctrl Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           grpMask        @b{ (input) } Mask of the Multicast Group
*           rtrIfNum       @b{ (input) } Index of the Router Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmAdminScopeBoundarySet (pimdmCB_t *pimdmCB,
                            L7_inet_addr_t *grpAddr,
                            L7_inet_addr_t *grpMask,
                            L7_uint32 rtrIfNum)
{
  pimdmASBNode_t *asbNode = L7_NULLPTR;
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 groupMask[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  PIMDM_TRACE(PIMDM_DEBUG_EVENTS, "Grp Address is %s",inetAddrPrint(grpAddr,grp));
  PIMDM_TRACE(PIMDM_DEBUG_EVENTS, "Grp Mask is %s",inetAddrPrint(grpMask,groupMask));

  /* Get the ASB Node for the Group.
   */
  if ((asbNode = pimdmAdminScopeNodeGet (pimdmCB, grpAddr, grpMask))
                                      == L7_NULLPTR)
  {
    /* Node doesn't exist.  Create one.
     */
    if ((asbNode = PIMDM_ALLOC (pimdmCB->addrFamily, sizeof (pimdmASBNode_t)))
                             == L7_NULLPTR)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Admin Scope Node Alloc Failed ");
      return L7_FAILURE;
    }

    /* Update the Node members and Add to the List.
     */
    memset(asbNode,0,sizeof(pimdmASBNode_t));

    inetCopy (&(asbNode->grpAddr), grpAddr);
    inetCopy (&(asbNode->grpMask), grpMask);
    BITX_SET (&asbNode->intfMask, rtrIfNum);

    PIMDM_TRACE (PIMDM_DEBUG_API, "Adding Node in List %p",asbNode);
    if (SLLAdd (&(pimdmCB->asbList), (L7_sll_member_t *) asbNode) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Admin Scope Node Addition Failed for"
                   "Grp - %s, rtrIfNum - %d", inetAddrPrint(grpAddr,grp), rtrIfNum);
      return L7_FAILURE;
    }
  }
  else
  {
    /* Verify if the Interface is already SET for this Group.
     */
    if (BITX_TEST (&asbNode->intfMask, rtrIfNum) != 0)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Admin Scope Intf already SET for "
                   "Grp - %s Intf - %d", inetAddrPrint(grpAddr,grp), rtrIfNum);
      return L7_SUCCESS;
    }

    /* Set the Interface as Scoped for this Group.
     */
    BITX_SET (&asbNode->intfMask, rtrIfNum);
  }

  /* Compute the OIF List.
   */
  pimdmMrtEntryAdminScopeUpdate (pimdmCB,grpAddr,grpMask);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Reset the Adminscope Boundary for a Group on an Interface
*
* @param    pimdmCB        @b{ (input) } Pointer to the PIM-DM Ctrl Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           grpMask        @b{ (input) } Mask of the Multicast Group
*           rtrIfNum       @b{ (input) } Index of the Router Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmAdminScopeBoundaryReset (pimdmCB_t *pimdmCB,
                              L7_inet_addr_t *grpAddr,
                              L7_inet_addr_t *grpMask,
                              L7_uint32 rtrIfNum)
{
  pimdmASBNode_t *asbNode = L7_NULLPTR;
  L7_BOOL isEmpty = L7_FALSE;
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  /* Get the ASB Node for the Group.
   */
  if ((asbNode = pimdmAdminScopeNodeGet (pimdmCB, grpAddr, grpMask))
                                      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Admin Scope Node doesn't Exist for Grp - %s ",
                 inetAddrPrint(grpAddr,grp));
    return L7_FAILURE;
  }

  /* Verify if the Interface is SET for this Group.
   */
  if ((BITX_TEST (&asbNode->intfMask, rtrIfNum)) == 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Admin Scope Intf Not SET for Grp - %s Intf - %d ",
                 inetAddrPrint(grpAddr,grp), rtrIfNum);
    return L7_FAILURE;
  }

  /* Reset the Interface as Scoped for this Group.
   */
  BITX_RESET (&asbNode->intfMask, rtrIfNum);

  /* If there are no other Interfaces set for this Group,
   * Delete it.
   */
  BITX_IS_EMPTY (&asbNode->intfMask, isEmpty);
  if (isEmpty == L7_TRUE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "All admin scope interfaces are cleared "
                 "for Grp -%s", inetAddrPrint(grpAddr,grp));
    if (SLLDelete (&(pimdmCB->asbList), (L7_sll_member_t*) asbNode) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Deleting Admin Scope Node for Grp - %s Intf - %d ",
               inetAddrPrint(grpAddr,grp), rtrIfNum);
      return L7_FAILURE;
    }
  }

  /* Compute the OIF List.
   */
  pimdmMrtEntryAdminScopeUpdate (pimdmCB,grpAddr,grpMask);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Update Admin Scope Boundary Table when pimdm interface is down
*
* @param    pimdmCB        @b{ (input) } Pointer to the PIM-DM Ctrl Block
* @param    rtrIfNum        @b{ (input) } Pointer to the Admin Scope node
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmAdminScopeIntfDownUpdate (pimdmCB_t *pimdmCB,
                              L7_uint32 rtrIfNum)
{
  pimdmASBNode_t  *asbNextNode = L7_NULLPTR;
  pimdmASBNode_t  *asbCurrNode = L7_NULLPTR;

  asbNextNode = (pimdmASBNode_t*)SLLFirstGet(&(pimdmCB->asbList));
  while (asbNextNode != L7_NULLPTR)
  {
    L7_BOOL isEmpty = L7_TRUE;

    asbCurrNode = asbNextNode;
    asbNextNode =  (pimdmASBNode_t*)SLLNextGet(&(pimdmCB->asbList),
                                               (L7_sll_member_t *) asbNextNode);

    BITX_RESET(&asbCurrNode->intfMask, rtrIfNum);
    BITX_IS_EMPTY(&asbCurrNode->intfMask, isEmpty);
    if (isEmpty == L7_TRUE)
    {
      if (SLLNodeDelete(&(pimdmCB->asbList), (L7_sll_member_t *) asbCurrNode) != L7_SUCCESS)
      {
        PIMDM_TRACE(PIMDM_DEBUG_FAILURE," Failed to delete admin-cope node");
        return L7_FAILURE;
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the Adminscope Boundary interface bitset for a Group
*
* @param    pimdmCB        @b{ (input) }   PIM-DM Control Block
*           grpAddr        @b{ (input) }   Address of the Multicast Group
*           rtrIfNum       @b{ (input) }   Router Interface Number
*           intfSet        @b{ (output) }  interface mask for the Group/mask. 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmAdminScopeIntfBitSetGet (pimdmCB_t *pimdmCB,
                              L7_inet_addr_t *grpAddr,
                              L7_uint32      rtrIfNum,
                              interface_bitset_t *oList)
{
  pimdmASBNode_t *curNode = L7_NULLPTR;
  L7_uchar8 maskLen;
  L7_uint32 rtrIfIndex;

  curNode = (pimdmASBNode_t*)SLLNextGet(&pimdmCB->asbList,L7_NULLPTR);

  BITX_RESET_ALL (oList);

  /*PIMDM_TRACE(PIMDM_DEBUG_API,"Entry");*/

  while (curNode != L7_NULLPTR)
  {
    inetMaskToMaskLen(&curNode->grpMask,&maskLen);
    PIMDM_TRACE(PIMDM_DEBUG_API,"maskLen is %d",maskLen);
    if (inetAddrCompareAddrWithMask(&curNode->grpAddr,maskLen,
                                    grpAddr, maskLen) == L7_SUCCESS)
    {
      if (BITX_TEST(&curNode->intfMask,rtrIfNum) != 0)
      {
        PIMDM_TRACE(PIMDM_DEBUG_API,"Upstrm Index(%d) is in Admin Scope List",rtrIfNum);
        for (rtrIfIndex =1;rtrIfIndex < PIMDM_MAX_INTERFACES;rtrIfIndex++)
        {
          BITX_SET (oList, rtrIfIndex);
        }
        return L7_SUCCESS;
      }
      else
      {
        /* Add the interface list to teh oList */
        PIMDM_TRACE(PIMDM_DEBUG_API,"Setting Admin List");
        BITX_ADD (oList, &curNode->intfMask, oList);
      }
    }
    curNode =  (pimdmASBNode_t*)SLLNextGet(&pimdmCB->asbList,
                                           (L7_sll_member_t*)curNode);
  }
  /*PIMDM_TRACE(PIMDM_DEBUG_API,"Exit");*/
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To display the info of the Admin Scope Boundary Database
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*
* @returns  void, if success
* @returns  void, if failure
*
* @comments    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmAdminScopeBoundaryInfoShow (pimdmCB_t *pimdmCB)
{
  pimdmASBNode_t *asbCurrNode = L7_NULLPTR;
  pimdmASBNode_t *asbNextNode = L7_NULLPTR;
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grpMask[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uint32 index = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Invalid Ctrl Block");
    return;
  }

  /* Display the Admin Scope Database Contents */
  if ((asbCurrNode = (pimdmASBNode_t*) SLLFirstGet (&(pimdmCB->asbList))) != L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Next Node Addr - 0x%x.\n", asbCurrNode->next);
    PIMDM_DEBUG_PRINTF ("Group Address  - %s.\n", inetAddrPrint (&(asbCurrNode->grpAddr), grp));
    PIMDM_DEBUG_PRINTF ("Group Mask  - %s.\n", inetAddrPrint (&(asbCurrNode->grpMask), grpMask));
    PIMDM_DEBUG_PRINTF ("Interfaces Set - ");
    for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
      if (BITX_TEST (&asbCurrNode->intfMask, index) != 0)
        PIMDM_DEBUG_PRINTF (" %02d ", index);
    PIMDM_DEBUG_PRINTF (".\n");
  }

  while ((asbNextNode = (pimdmASBNode_t*) SLLNextGet (&(pimdmCB->asbList),
                                                (L7_sll_member_t*) asbCurrNode))
                      != L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("----- ----- -----\n");
    PIMDM_DEBUG_PRINTF ("Next Node Addr - 0x%x.\n", asbNextNode->next);
    PIMDM_DEBUG_PRINTF ("Group Address  - %s.\n", inetAddrPrint (&(asbNextNode->grpAddr), grp));
    PIMDM_DEBUG_PRINTF ("Group Mask  - %s.\n", inetAddrPrint (&(asbNextNode->grpMask), grpMask));
    PIMDM_DEBUG_PRINTF ("Interfaces Set - ");
    for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
      if (BITX_TEST (&asbNextNode->intfMask, index) != 0)
        PIMDM_DEBUG_PRINTF (" %02d ", index);
    PIMDM_DEBUG_PRINTF (".\n");
    asbCurrNode = asbNextNode;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return;
}

