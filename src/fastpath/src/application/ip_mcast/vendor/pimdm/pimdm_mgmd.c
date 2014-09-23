/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_mgmd.c
*
* @purpose    Implementaion of the PIM-DM module's interaction with the MGMD module
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
#include "l3_addrdefs.h"
#include "heap_api.h"
#include "mcast_bitset.h"
#include "avl_api.h"
#include "pimdm_debug.h"
#include "pimdm_mgmd.h"
#include "pimdm_util.h"

/*******************************************************************************
**                             Defines                                        **
*******************************************************************************/
#define AVL_TREE_TYPE                  0x10
#define PIMDM_MAX_IP_STRING_LEN        75


/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/
static L7_int32
pimdmLocalMembEntryCompare (const void *pData1,
                            const void *pData2,
                            size_t size);

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Compare two local membership entries.
*
* @param    pData1   - pointer to the first entry
*           pData2   - pointer to the second entry
*           size     - size of the key in each entry (ignored as of now).
*
* @returns  >0, if pData1 is greater than pData2.
*           =0, if pData1 is equal to pData2.
*           <0, if pData1 is less than pData2.
*
* @notes    1. Entries are stored as <G,S> instead of the usual <S,G> to
*              ease out the searches for a group
*           2. <*,G> entry is stored as <G,0>. Since a source of <0> is
*              invalid, there would not be any clash between S & *.
*
* @end
*********************************************************************/
static L7_int32
pimdmLocalMembEntryCompare(const void *pData1,
                           const void *pData2,
                           size_t     size)
{
  pimdmLocalMemb_t *pKey1 = (pimdmLocalMemb_t *)pData1;
  pimdmLocalMemb_t *pKey2 = (pimdmLocalMemb_t *)pData2;
  register L7_int32 retVal = 0;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  retVal = L7_INET_ADDR_COMPARE(&(pKey1->group), &(pKey2->group));
  if (retVal != 0)
    return retVal;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");*/
  return (L7_INET_ADDR_COMPARE(&(pKey1->source), &(pKey2->source)));
}

/*********************************************************************
*
* @purpose  Add a <*,G> membership entry for the given group and interface
*
* @param    pimdmCB   - pointer to the PIM-DM control block to operate on
*           pGroup    - pointer to the Group address
*           rtrIfNum  - router interface number this information pertains to
*                            
* @returns  L7_TRUE, if there were changes to the database
* @returns  L7_FALSE, if entry is already there and no changes to the database
*
* @notes    
*
* @end
*********************************************************************/
static L7_BOOL
pimdmMgmdMembershipStarGAdd(pimdmCB_t      *pimdmCB,
                            L7_inet_addr_t *pGroup,
                            L7_uint32      rtrIfNum)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FALSE;

  /* Set-up the entry */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.group));

  inetCopy(&(entry.group), pGroup);
  BITX_SET(&entry.incl, rtrIfNum);

  /* Retrieve the entry if already existant */
  pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_EXACT);
  if(pEntry == L7_NULL)
  {
    /* Entry non existing; just add it */
    if(avlInsertEntry(&(pimdmCB->localMembTree), (void *)(&entry)) != L7_NULLPTR)
    {
      /* Error inserting a new entry : maybe duplicate exists in cache */
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Error adding a (*, %s) entry",
                  inetAddrPrint(pGroup, grp));
      return L7_FALSE;
    }
    PIMDM_TRACE (PIMDM_DEBUG_MGMD, "(*, %s) Entry Added", inetAddrPrint(pGroup, grp));
    return L7_TRUE; /* Indicate the change : new entry added */
  }

  /* Entry already on the tree; just make sure if the interface bit is set properly */
  PIMDM_TRACE (PIMDM_DEBUG_MGMD, "Entry (*, %s) alreday exists", inetAddrPrint(pGroup, grp));
  if(BITX_TEST(&pEntry->incl, rtrIfNum) != 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_MGMD, "rtrIfNum - %d is already Set for (*, %s) Entry",
                 rtrIfNum, inetAddrPrint(pGroup, grp));
    return L7_FALSE; /* Already set. No change */
  }
  else
  {
    PIMDM_TRACE (PIMDM_DEBUG_MGMD, "rtrIfNum - %d is already Set for (*, %s) Entry",
                 rtrIfNum, inetAddrPrint(pGroup, grp));
    BITX_SET(&pEntry->incl, rtrIfNum);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_TRUE;
}

/*********************************************************************
*
* @purpose  Delete a <*,G> membership entry for the given group and interface
*
* @param    pimdmCB   - pointer to the PIM-DM control block to operate on
*           pGroup    - pointer to the Group address
*           rtrIfNum  - router interface number this information pertains to
*                            
* @returns  L7_TRUE, if there were changes to the database
* @returns  L7_FALSE, if entry is already there and no changes to the database
*
* @notes    If the entry looses all the interfaces, the entry itself is removed
*
* @end
*********************************************************************/
L7_BOOL
pimdmMgmdMembershipStarGDelete(pimdmCB_t      *pimdmCB,
                               L7_inet_addr_t *pGroup,
                               L7_uint32      rtrIfNum)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FALSE;

  /* Set-up the entry */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.group));

  inetCopy(&(entry.group), pGroup);

  /* Retrieve the entry if existant */
  pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_EXACT);
  if(pEntry == L7_NULL)
  {
    /* Entry non existing; just return */
    PIMDM_TRACE (PIMDM_DEBUG_MGMD, "(*, %s) Entry doesn't Exist",
                 inetAddrPrint(pGroup, grp));
    return L7_FALSE;
  }

  /* Entry present on the tree; just make sure if the interface bit is cleared */
  if(BITX_TEST(&pEntry->incl, rtrIfNum) != 0)
  {
    L7_BOOL bEmpty;

    /* Bit set : Clear it */
    BITX_RESET(&pEntry->incl, rtrIfNum);

    PIMDM_TRACE (PIMDM_DEBUG_MGMD, "rtrIfNum - %d is Reset for (*, %s) Entry",
                 rtrIfNum, inetAddrPrint(pGroup, grp));

    /* Check if all the interfaces have been removed for this <*,G> entry; if so,
       remove the entry altogether */
    BITX_IS_EMPTY(&pEntry->incl, bEmpty);
    if(bEmpty == L7_TRUE)
    {
      PIMDM_TRACE (PIMDM_DEBUG_MGMD, "All intrfaces have been removed.So deleting the entry");
      avlDeleteEntry(&(pimdmCB->localMembTree), (void *)(&entry));
    }

    PIMDM_TRACE (PIMDM_DEBUG_MGMD, "Success");
    return L7_TRUE; /* Cleared the interface bit. Indicate change */
  }

  PIMDM_TRACE (PIMDM_DEBUG_MGMD, "rtrIfNum - %d is not Set for (*, %s) Entry",
               rtrIfNum, inetAddrPrint(pGroup, grp));

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_FALSE; /* No change; indicate so */
}

/*********************************************************************
*
* @purpose  Add a <S,G> membership entry for the given group and interface onto
*           the INCLUDE List
*
* @param    pimdmCB   - pointer to the PIM-DM control block to operate on
*           pGroup    - pointer to the Group address
*           pSource   - pointer to the Source address
*           rtrIfNum  - router interface number this information pertains to
*                            
* @returns  L7_TRUE, if there were changes to the database
* @returns  L7_FALSE, if entry is already there and no changes to the database
*
* @notes    If a corresponding entry exists in EXCLUDE mode, that would be cleared
*
* @end
*********************************************************************/
static L7_BOOL
pimdmMgmdMembershipSGInclAdd(pimdmCB_t      *pimdmCB,
                             L7_inet_addr_t *pGroup,
                             L7_inet_addr_t *pSource,
                             L7_uint32      rtrIfNum)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FALSE;

  /* Set-up the entry */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.group));

  inetCopy(&(entry.group), pGroup);
  inetCopy(&(entry.source), pSource);
  BITX_SET(&entry.incl, rtrIfNum);

  /* Retrieve the entry if already existant */
  pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_EXACT);
  if(pEntry == L7_NULL)
  {
    /* Entry non existing; just add it */
    if(avlInsertEntry(&(pimdmCB->localMembTree), (void *)(&entry)) != L7_NULLPTR)
    {
      /* Error inserting a new entry : maybe duplicate exists in cache */
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Error adding a <S,G> entry to the "
                  "local membership list");
      return L7_FALSE;
    }
    return L7_TRUE; /* Indicate the change : new entry added */
  }

  /* Entry already on the tree; just make sure if the interface bits are set properly */
  if(BITX_TEST(&pEntry->incl, rtrIfNum) != 0)
  {
    return L7_FALSE; /* Already set. No change */
  }
  else
  {
    BITX_SET(&pEntry->incl, rtrIfNum);
    BITX_RESET(&pEntry->excl, rtrIfNum);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_TRUE;
}

/*********************************************************************
*
* @purpose  Delete a <S,G> membership entry for the given group and interface
*           from the INCLUDE list
*
* @param    pimdmCB   - pointer to the PIM-DM control block to operate on
*           pGroup    - pointer to the Group address
*           pSource   - pointer to the Source address
*           rtrIfNum  - router interface number this information pertains to
*                            
* @returns  L7_TRUE, if there were changes to the database
* @returns  L7_FALSE, if entry is already there and no changes to the database
*
* @notes    If the entry looses all the interfaces in both the include and exclude
*           lists, the entry itself is removed
*
* @end
*********************************************************************/
L7_BOOL
pimdmMgmdMembershipSGInclDelete(pimdmCB_t      *pimdmCB,
                                L7_inet_addr_t *pGroup,
                                L7_inet_addr_t *pSource,
                                L7_uint32      rtrIfNum)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FALSE;

  /* Set-up the entry */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.group));

  inetCopy(&(entry.group), pGroup);
  inetCopy(&(entry.source), pSource);

  /* Retrieve the entry if existant */
  pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_EXACT);
  if(pEntry == L7_NULL)
  {
    /* Entry non existing; just return */
    return L7_FALSE;
  }

  /* Entry present on the tree; just make sure if the interface bit is cleared */
  if(BITX_TEST(&pEntry->incl, rtrIfNum) != 0)
  {
    L7_BOOL bEmpty;

    /* Bit set : Clear it */
    BITX_RESET(&pEntry->incl, rtrIfNum);

    /* Check if all the interfaces have been removed for this <S,G> entry; from
       both the include and exclude lists. If so, remove the entry altogether */
    BITX_IS_EMPTY(&pEntry->incl, bEmpty);
    if(bEmpty == L7_TRUE)
    {
      BITX_IS_EMPTY(&pEntry->excl, bEmpty);
      if(bEmpty == L7_TRUE)
      {
        avlDeleteEntry(&(pimdmCB->localMembTree), (void *)(&entry));
      }
    }
    return L7_TRUE; /* Cleared the interface bit. Indicate change */
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_FALSE; /* No change; indicate so */
}

/*********************************************************************
*
* @purpose  Add a <S,G> membership entry for the given group and interface onto
*           the EXCLUDE List
*
* @param    pimdmCB   - pointer to the PIM-DM control block to operate on
*           pGroup    - pointer to the Group address
*           pSource   - pointer to the Source address
*           rtrIfNum  - router interface number this information pertains to
*                            
* @returns  L7_TRUE, if there were changes to the database
* @returns  L7_FALSE, if entry is already there and no changes to the database
*
* @notes    If a corresponding entry exists in INCLUDE mode, that would be cleared
*
* @end
*********************************************************************/
static L7_BOOL
pimdmMgmdMembershipSGExclAdd(pimdmCB_t      *pimdmCB,
                             L7_inet_addr_t *pGroup,
                             L7_inet_addr_t *pSource,
                             L7_uint32      rtrIfNum)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FALSE;

  /* Set-up the entry */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.group));

  inetCopy(&(entry.group), pGroup);
  inetCopy(&(entry.source), pSource);
  BITX_SET(&entry.excl, rtrIfNum);

  /* Retrieve the entry if already existant */
  pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_EXACT);
  if(pEntry == L7_NULL)
  {
    /* Entry non existing; just add it */
    if(avlInsertEntry(&(pimdmCB->localMembTree), (void *)(&entry)) != L7_NULLPTR)
    {
      /* Error inserting a new entry : maybe duplicate exists in cache */
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Error adding a <S,G> entry to the "
                  "local membership list");
      return L7_FALSE;
    }
    return L7_TRUE; /* Indicate the change : new entry added */
  }

  /* Entry already on the tree; just make sure if the interface bits are set properly */
  if(BITX_TEST(&pEntry->excl, rtrIfNum) != 0)
  {
    return L7_FALSE; /* Already set. No change */
  }
  else
  {
    BITX_SET(&pEntry->excl, rtrIfNum);
    BITX_RESET(&pEntry->incl, rtrIfNum);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_TRUE;
}

/*********************************************************************
*
* @purpose  Delete a <S,G> membership entry for the given group and interface
*           from the EXCLUDE list
*
* @param    pimdmCB   - pointer to the PIM-DM control block to operate on
*           pGroup    - pointer to the Group address
*           pSource   - pointer to the Source address
*           rtrIfNum  - router interface number this information pertains to
*                            
* @returns  L7_TRUE, if there were changes to the database
* @returns  L7_FALSE, if entry is already there and no changes to the database
*
* @notes    If the entry looses all the interfaces in both the include and exclude
*           lists, the entry itself is removed
*
* @end
*********************************************************************/
L7_BOOL
pimdmMgmdMembershipSGExclDelete(pimdmCB_t      *pimdmCB,
                                L7_inet_addr_t *pGroup,
                                L7_inet_addr_t *pSource,
                                L7_uint32      rtrIfNum)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FALSE;

  /* Set-up the entry */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.group));

  inetCopy(&(entry.group), pGroup);
  inetCopy(&(entry.source), pSource);

  /* Retrieve the entry if existant */
  pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_EXACT);
  if(pEntry == L7_NULL)
  {
    /* Entry non existing; just return */
    return L7_FALSE;
  }

  /* Entry present on the tree; just make sure if the interface bit is cleared */
  if(BITX_TEST(&pEntry->excl, rtrIfNum) != 0)
  {
    L7_BOOL bEmpty;

    /* Bit set : Clear it */
    BITX_RESET(&pEntry->excl, rtrIfNum);

    /* Check if all the interfaces have been removed for this <S,G> entry; from
       both the include and exclude lists. If so, remove the entry altogether */
    BITX_IS_EMPTY(&pEntry->excl, bEmpty);
    if(bEmpty == L7_TRUE)
    {
      BITX_IS_EMPTY(&pEntry->incl, bEmpty);
      if(bEmpty == L7_TRUE)
      {
        avlDeleteEntry(&(pimdmCB->localMembTree), (void *)(&entry));
      }
    }
    return L7_TRUE; /* Cleared the interface bit. Indicate change */
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_FALSE; /* No change; indicate so */
}

/*********************************************************************
*
* @purpose  Update the internal database with the latest local membership information
*           from the MGMD module for the given Group address and interface
*
* @param    pimdmCB         - pointer to the PIM-DM control block to operate on
*           mgmdUpdateEntry - pointer to the update info from the MGMD module
*                            
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t
pimdmMgmdMembershipUpdate(pimdmCB_t *pimdmCB,
                          mgmdMrpEventInfo_t *mgmdUpdateEntry)
{
  L7_uint32      numSrcs, rtrIfNum, i;
  L7_uchar8      change[MCAST_BITX_NUM_BITS(MGMD_MAX_QUERY_SOURCES + 1)];
  L7_inet_addr_t *pGroup;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if(pimdmCB == L7_NULL)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid PIM-DM control block : NULL");
    return L7_FAILURE;
  }
  if(mgmdUpdateEntry == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid MGMD update information : NULL");
    return L7_FAILURE;
  }

  /* Setup some local variables for ease of use */
  numSrcs = mgmdUpdateEntry->numSrcs;
  rtrIfNum = mgmdUpdateEntry->rtrIfNum;
  pGroup = &(mgmdUpdateEntry->group);
  MCAST_BITX_RESET_ALL (change);

  /* Process the given update */
  switch(mgmdUpdateEntry->mode)
  {
    case MGMD_FILTER_MODE_INCLUDE:
      if(pimdmMgmdMembershipStarGDelete(pimdmCB, pGroup, rtrIfNum) == L7_TRUE)
      {
        /* Set Zero'th index for the (*,G) Entry */
        MCAST_BITX_SET(change, 0);
      }
      for(i = 0; i < numSrcs; i++)
      {
        if (mgmdUpdateEntry->sourceList[i] == L7_NULLPTR)
        {
          PIMDM_TRACE (PIMDM_DEBUG_MGMD, "MGMD Update Entry Source List Buffer Ptr is NULL");
          continue;
        }
        if(mgmdUpdateEntry->sourceList[i]->action == MGMD_SOURCE_ADD)
        {
          if(pimdmMgmdMembershipSGInclAdd(pimdmCB, pGroup, &(mgmdUpdateEntry->sourceList[i]->sourceAddr), rtrIfNum) == L7_TRUE)
          {
            MCAST_BITX_SET(change, i + 1);
          }
        }
        else if(mgmdUpdateEntry->sourceList[i]->action == MGMD_SOURCE_DELETE)
        {
          if(pimdmMgmdMembershipSGInclDelete(pimdmCB, pGroup, &(mgmdUpdateEntry->sourceList[i]->sourceAddr), rtrIfNum) == L7_TRUE)
          {
            MCAST_BITX_SET(change, i + 1);
          }
        }
      }
      break;
    case MGMD_FILTER_MODE_EXCLUDE:
      if(pimdmMgmdMembershipStarGAdd(pimdmCB, pGroup, rtrIfNum) == L7_TRUE)
      {
        /* Set Zero'th index for the (*,G) Entry */
        MCAST_BITX_SET(change, 0);
      }
      for(i = 0; i < numSrcs; i++)
      {
        if (mgmdUpdateEntry->sourceList[i] == L7_NULLPTR)
        {
          PIMDM_TRACE (PIMDM_DEBUG_MGMD, "MGMD Update Entry Source List Buffer Ptr is NULL");
          continue;
        }
        if(mgmdUpdateEntry->sourceList[i]->filterMode == MGMD_FILTER_MODE_INCLUDE)
        {
          if(mgmdUpdateEntry->sourceList[i]->action == MGMD_SOURCE_ADD)
          {
            if(pimdmMgmdMembershipSGInclAdd(pimdmCB, pGroup, &(mgmdUpdateEntry->sourceList[i]->sourceAddr), rtrIfNum) == L7_TRUE)
            {
              MCAST_BITX_SET(change, i + 1);
            }
          }
          else if(mgmdUpdateEntry->sourceList[i]->action == MGMD_SOURCE_DELETE)
          {
            if(pimdmMgmdMembershipSGInclDelete(pimdmCB, pGroup, &(mgmdUpdateEntry->sourceList[i]->sourceAddr), rtrIfNum) == L7_TRUE)
            {
              MCAST_BITX_SET(change, i + 1);
            }
          }
        }
        else if(mgmdUpdateEntry->sourceList[i]->filterMode == MGMD_FILTER_MODE_EXCLUDE)
        {
          if(mgmdUpdateEntry->sourceList[i]->action == MGMD_SOURCE_ADD)
          {
            if(pimdmMgmdMembershipSGExclAdd(pimdmCB, pGroup, &(mgmdUpdateEntry->sourceList[i]->sourceAddr), rtrIfNum) == L7_TRUE)
            {
              MCAST_BITX_SET(change, i + 1);
            }
          }
          else if(mgmdUpdateEntry->sourceList[i]->action == MGMD_SOURCE_DELETE)
          {
            if(pimdmMgmdMembershipSGExclDelete(pimdmCB, pGroup, &(mgmdUpdateEntry->sourceList[i]->sourceAddr), rtrIfNum) == L7_TRUE)
            {
              MCAST_BITX_SET(change, i + 1);
            }
          }
        }
      }
      break;
    default:
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Group Filter Mode %d",
                   mgmdUpdateEntry->mode);
      return L7_FAILURE;
  }

  /* Process any changes */
  for(i = 0; i < MGMD_MAX_QUERY_SOURCES; i++)
  {
    if(MCAST_BITX_TEST(change, i) != 0) /* Zero'th index is for the (*,G) Entry */
    {
      if(i == 0)
      {
        pimdmMrtOifListCompute(pimdmCB, L7_NULLPTR, pGroup, PIMDM_MRT_CHANGE_LOCAL_RCV);
      }
      else
      {
        if (mgmdUpdateEntry->sourceList[i - 1] == L7_NULLPTR)
        {
          PIMDM_TRACE (PIMDM_DEBUG_MGMD, "MGMD Update Entry Source List Buffer Ptr is NULL");
          continue;
        }
        pimdmMrtOifListCompute(pimdmCB, &(mgmdUpdateEntry->sourceList[i - 1]->sourceAddr), pGroup, PIMDM_MRT_CHANGE_LOCAL_RCV);
      }
    }
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Process messages from the MGMD module
*
* @param
*                            
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t
pimdmMgmdMsgProcess(pimdmCB_t       *pimdmCB,
                    L7_uint32       mgmdEventType,
                    mgmdMrpEventInfo_t *mgmdUpdateEntry)
{
  L7_RC_t retVal = L7_FAILURE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  switch (mgmdEventType)
  {
    case MCAST_EVENT_MGMD_GROUP_UPDATE:
      retVal = pimdmMgmdMembershipUpdate(pimdmCB, mgmdUpdateEntry);
      break;

    default:
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid message %d from MGMD",
                   mgmdEventType);
      return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Initialize the local membership data structures
*
* @param    pimdmCB - pointer to PIM-DM control block
*                            
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine MUST be called only from Phase 1 init of PIM-DM
*
* @end
*********************************************************************/

L7_RC_t
pimdmMgmdInit (pimdmCB_t* pimdmCB)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic sanity checks & validations */
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Control Block");
    return L7_FAILURE;
  }

  /* Initialize memory for the tree that would hold all the local memberships */
  pimdmCB->pLocalMembTreeHeap = PIMDM_ALLOC (pimdmCB->addrFamily,
                     (sizeof (avlTreeTables_t) * PIMDM_MAX_LOCAL_MEMB_ENTRIES));
  pimdmCB->pLocalMembDataHeap = PIMDM_ALLOC (pimdmCB->addrFamily,
                    (sizeof (pimdmLocalMemb_t) * PIMDM_MAX_LOCAL_MEMB_ENTRIES));

  if ((pimdmCB->pLocalMembDataHeap == L7_NULLPTR) ||
      (pimdmCB->pLocalMembTreeHeap == L7_NULLPTR))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Local Mem'ship Heap/Data Alloc Failed");

    if (pimdmCB->pLocalMembTreeHeap != L7_NULLPTR)
    {
      PIMDM_FREE (pimdmCB->addrFamily, (void*) pimdmCB->pLocalMembTreeHeap);
    }

    pimdmCB->pLocalMembTreeHeap = L7_NULLPTR;
    pimdmCB->pLocalMembDataHeap = L7_NULLPTR;
    return L7_FAILURE;
  }

  /* Initialize the Cache Table on an AVL tree */
  memset(&(pimdmCB->localMembTree), 0, sizeof(avlTree_t));
  avlCreateAvlTree(&(pimdmCB->localMembTree),         /* Pointer to the AVL Tree object */
                   pimdmCB->pLocalMembTreeHeap,       /* Pointer to a AVL Tree Node/Entry heap */
                   pimdmCB->pLocalMembDataHeap,       /* Pointer to a data heap */
                   PIMDM_MAX_LOCAL_MEMB_ENTRIES,      /* Max. number of entries supported */
                   sizeof(pimdmLocalMemb_t),          /* Size of the data portion of each entry */
                   AVL_TREE_TYPE,                     /* Generic value for AVL Tree type */
                   sizeof(L7_inet_addr_t) * 2);       /* Length of the key - <G,S> */

  /* Check AVL semaphore ID to see if create succeeded */
  if (pimdmCB->localMembTree.semId == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Local Mem'ship Storage object Creation Failed");
    PIMDM_FREE (pimdmCB->addrFamily, (void*) pimdmCB->pLocalMembTreeHeap);
    PIMDM_FREE (pimdmCB->addrFamily, (void*) pimdmCB->pLocalMembDataHeap);
    pimdmCB->pLocalMembTreeHeap = L7_NULLPTR;
    pimdmCB->pLocalMembDataHeap = L7_NULLPTR;
    return L7_FAILURE;
  }
  avlSetAvlTreeComparator(&(pimdmCB->localMembTree), pimdmLocalMembEntryCompare);

  /* All done; return SUCCESS */
  PIMDM_TRACE (PIMDM_DEBUG_INIT, "Local Mem'ship Tree Creation Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  De-initialize the local membership data structures
*
* @param    pimdmCB - pointer to PIM-DM control block
*                            
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t
pimdmMgmdDeInit (pimdmCB_t* pimdmCB)
{
  L7_RC_t retVal = L7_SUCCESS;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic sanity checks and validations */
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Control Block");
    return L7_FAILURE;
  }

  /* Do the clean-up in the reverse order of initialization */
  if (avlDeleteAvlTree (&(pimdmCB->localMembTree)) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Local Mem'ship Storage object Destroy Failed");
    retVal = L7_FAILURE;
  }

  PIMDM_FREE (pimdmCB->addrFamily, (void*) pimdmCB->pLocalMembTreeHeap);
  PIMDM_FREE (pimdmCB->addrFamily, (void*) pimdmCB->pLocalMembDataHeap);

  pimdmCB->pLocalMembTreeHeap = L7_NULLPTR;
  pimdmCB->pLocalMembDataHeap = L7_NULLPTR;

  /* All done */
  PIMDM_TRACE (PIMDM_DEBUG_INIT, "Local Mem'ship Tree Destroy Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Update a PIM-DM Interface Bitmap with the local Include(*,G) membership details
*
* @param    pimdmCB     @b{(input)} Pointer to PIM-DM control block
* @param    pGroup      @b{(input)} Pointer to the Group address
* @param    pBitmap     @b{(inout)} Pointer to the Bitmap
*                            
* @returns  L7_TRUE, if the membership bitmap was changed
* @returns  L7_FALSE, if the membership bitmap was not changed
*
* @comments The given bitmap MUST point to a buffer of size PIMDM_INTF_BIT_SIZE.
* @comments The given bitmap will be updated with the new values upon return.
*
* @end
*********************************************************************/
L7_BOOL
pimdmMgmdStarGInclCompute(pimdmCB_t      *pimdmCB,
                          L7_inet_addr_t *pGroup,
                          interface_bitset_t *pBitmap)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;
  L7_BOOL          bIsEmpty;
  interface_bitset_t bitmap;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  /* Some sanity Checks */
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Control Block pointer is NULL");
    return L7_FALSE;
  }
  if(pGroup == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Group Address pointer is NULL");
    return L7_FALSE;
  }
  if(pBitmap == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"BitMap Pointer is NULL");
    return L7_FALSE;
  }

  BITX_COPY (pBitmap, &bitmap);

  /* Retrieve the local <*,G> membership entry */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.group));

  inetCopy(&(entry.group), pGroup);

  /* Retrieve the entry if existant */
  pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_EXACT);
  if(pEntry == L7_NULL)
  {
    /* Entry non existing; check if any group memberships were there before and
    update the bitmap and return appropriate return code*/
    BITX_IS_EMPTY(&bitmap, bIsEmpty);
    if(bIsEmpty == L7_TRUE)
      return L7_FALSE;
    BITX_RESET_ALL (pBitmap);
    return L7_TRUE;
  }
  else
  {
    interface_bitset_t testBitmap;

    /* Entry exists in the tree. Diff the bitmaps, then update the given entry with the correct bitmap
       and return an appropriate return value */
    BITX_XOR(&bitmap, &pEntry->incl, &testBitmap);
    BITX_IS_EMPTY(&testBitmap, bIsEmpty);
    if(bIsEmpty == L7_TRUE)
    {
      /* No change */
      return L7_FALSE;
    }
    BITX_COPY (&pEntry->incl, pBitmap);
    return L7_TRUE;
  }
}

/*********************************************************************
*
* @purpose  Update a PIM-DM Interface Bitmap with the local Include(S,G) membership details
*
* @param    pimdmCB     @b{(input)} Pointer to PIM-DM control block
* @param    pSource     @b{(input)} Pointer to the Source address
* @param    pGroup      @b{(input)} Pointer to the Group address
* @param    pBitmap     @b{(inout)} Pointer to the Bitmap
*                            
* @returns  L7_TRUE, if the membership bitmap was changed
* @returns  L7_FALSE, if the membership bitmap was not changed
*
* @comments The given bitmap MUST point to a buffer of size PIMDM_INTF_BIT_SIZE.
* @comments The given bitmap will be updated with the new values upon return.
*
* @end
*********************************************************************/
L7_BOOL
pimdmMgmdSGInclCompute(pimdmCB_t      *pimdmCB,
                       L7_inet_addr_t *pSource,
                       L7_inet_addr_t *pGroup,
                       interface_bitset_t *pBitmap)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;
  L7_BOOL          bIsEmpty;
  interface_bitset_t bitmap;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  /* Some sanity Checks */
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Control Block pointer is NULL");
    return L7_FALSE;
  }
  if(pSource == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Source Address pointer is NULL");
    return L7_FALSE;
  }
  if(pGroup == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Group Address pointer is NULL");
    return L7_FALSE;
  }
  if(pBitmap == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"BitMap pointer is NULL");
    return L7_FALSE;
  }

  BITX_COPY (pBitmap, &bitmap);

  /* Retrieve the local <*,G> membership entry */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.group));

  inetCopy(&(entry.group), pGroup);
  inetCopy(&(entry.source), pSource);

  /* Retrieve the entry if existant */
  pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_EXACT);
  if(pEntry == L7_NULL)
  {
    /* Entry non existing; check if any memberships were there before and
    update the bitmap and return appropriate return code*/
    BITX_IS_EMPTY(&bitmap, bIsEmpty);
    if(bIsEmpty == L7_TRUE)
      return L7_FALSE;
    BITX_RESET_ALL (pBitmap);
    return L7_TRUE;
  }
  else
  {
    interface_bitset_t testBitmap;

    /* Entry exists in the tree. Diff the bitmaps, then update the given entry with the correct bitmap
       and return an appropriate return value */
    BITX_XOR(&bitmap, &pEntry->incl, &testBitmap);
    BITX_IS_EMPTY(&testBitmap, bIsEmpty);
    if(bIsEmpty == L7_TRUE)
    {
      /* No change */
      return L7_FALSE;
    }
    BITX_COPY (&pEntry->incl, pBitmap);
    return L7_TRUE;
  }
}

/*********************************************************************
*
* @purpose  Update a PIM-DM Interface Bitmap with the local Exclude(S,G) membership details
*
* @param    pimdmCB     @b{(input)} Pointer to PIM-DM control block
* @param    pSource     @b{(input)} Pointer to the Source address
* @param    pGroup      @b{(input)} Pointer to the Group address
* @param    pBitmap     @b{(inout)} Pointer to the Bitmap
*                            
* @returns  L7_TRUE, if the membership bitmap was changed
* @returns  L7_FALSE, if the membership bitmap was not changed
*
* @comments The given bitmap MUST point to a buffer of size PIMDM_INTF_BIT_SIZE.
* @comments The given bitmap will be updated with the new values upon return.
*
* @end
*********************************************************************/
L7_BOOL
pimdmMgmdSGExclCompute(pimdmCB_t      *pimdmCB,
                       L7_inet_addr_t *pSource,
                       L7_inet_addr_t *pGroup,
                       interface_bitset_t *pBitmap)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;
  L7_BOOL          bIsEmpty;
  interface_bitset_t bitmap;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  /* Some sanity Checks */
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"CB pointer is NULL");
    return L7_FALSE;
  }
  if(pSource == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Source Address pointer is NULL");
    return L7_FALSE;
  }
  if(pGroup == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Group Address pointer is NULL");
    return L7_FALSE;
  }
  if(pBitmap == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"BitMap pointer is NULL");
    return L7_FALSE;
  }

  BITX_COPY (pBitmap, &bitmap);

  /* Retrieve the local <*,G> membership entry */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(entry.group));

  inetCopy(&(entry.group), pGroup);
  inetCopy(&(entry.source), pSource);

  /* Retrieve the entry if existant */
  pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_EXACT);
  if(pEntry == L7_NULL)
  {
    /* Entry non existing; check if any memberships were there before and
    update the bitmap and return appropriate return code*/
    BITX_IS_EMPTY(&bitmap, bIsEmpty);
    if(bIsEmpty == L7_TRUE)
      return L7_FALSE;
    BITX_RESET_ALL (pBitmap);
    return L7_TRUE;
  }
  else
  {
    interface_bitset_t testBitmap;

    /* Entry exists in the tree. Diff the bitmaps, then update the given entry with the correct bitmap
       and return an appropriate return value */
    BITX_XOR(&bitmap, &pEntry->excl, &testBitmap);
    BITX_IS_EMPTY(&testBitmap, bIsEmpty);
    if(bIsEmpty == L7_TRUE)
    {
      /* No change */
      return L7_FALSE;
    }
    BITX_COPY (&pEntry->excl, pBitmap);
    return L7_TRUE;
  }
}

/*********************************************************************
*
* @purpose  Get the MGMD Group Membership details for an Interface
*
* @param    pimdmCB     @b{(input)} Pointer to PIM-DM control block
* @param    rtrIfNum    @b{(input)} Router Interface Number
*                            
* @returns  L7_SUCCESS,  if the membership get event post is success
* @returns  L7_FFAILURE, if the membership get event post fails
*
* @comments None.
*
* @end
*********************************************************************/
L7_RC_t
pimdmMgmdIntfGroupInfoGet (pimdmCB_t *pimdmCB,
                           L7_uint32 rtrIfNum)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Post an event to MGMD to get the Group membership details for
   * this interface.
   */
  if (mgmdMapMRPGroupInfoGet (pimdmCB->addrFamily, rtrIfNum) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failure in Posting the MGMD Info Get "
                 "Event for rtrIfNum - %d", rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Show the current local membership details
*
* @param    pimdmCB  - pointer to PIM-DM control block
*                            
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void
pimdmMgmdShow(pimdmCB_t *pimdmCB,L7_int32 count)
{
  pimdmLocalMemb_t *pEntry;
  pimdmLocalMemb_t entry;
  L7_uchar8        printBuf[PIMDM_MAX_IP_STRING_LEN];
  L7_uint32        i = 0;
  L7_int32         localcount=L7_NULL;  

  /* Some sanity Checks */
  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("%s, %d : Control Block pointer is NULL\n",__FUNCTION__, __LINE__);
    return;
  }
  if(pimdmCB->localMembTree.semId == L7_NULL)
  {
    PIMDM_DEBUG_PRINTF ("%s, %d : Local Membership object not yet initialized\n",__FUNCTION__, __LINE__);
    return;
  }

  /* Retrieve entries and print */
  memset(&entry, 0, sizeof(pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily, &entry.group);
  inetAddressZeroSet(pimdmCB->addrFamily, &entry.source);
  for (pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_NEXT);
       pEntry != L7_NULLPTR;
       pEntry = (pimdmLocalMemb_t *)avlSearchLVL7(&(pimdmCB->localMembTree), (void *)(&entry), AVL_NEXT))
  {

    if((count != L7_NULL)&&(localcount >= count))
    {
      break;
    }
    
    localcount++;

    inetCopy(&(entry.group), &(pEntry->group));
    inetCopy(&(entry.source), &(pEntry->source));
    if(inetIsAddressZero(&(pEntry->source)) == L7_TRUE)
    {
      /* <*,G> entry */
      PIMDM_DEBUG_PRINTF ("\n");
      PIMDM_DEBUG_PRINTF ("    <*,G> entry :\n");
      osapiSnprintf(printBuf, PIMDM_MAX_IP_STRING_LEN, "Error");
      inetAddrHtop(&(pEntry->group), printBuf);
      PIMDM_DEBUG_PRINTF ("       Group Address : %s\n", printBuf);
      PIMDM_DEBUG_PRINTF ("       Include Bitmap :");
      for(i = 1; i <= PIMDM_INTF_BIT_SIZE; i++)
      {
        if (BITX_TEST (&pEntry->incl, i) != 0)
          PIMDM_DEBUG_PRINTF (" %02d", i);
      }
      PIMDM_DEBUG_PRINTF ("\n");
    }
    else
    {
      /* <S,G> entry */
      PIMDM_DEBUG_PRINTF ("\n");
      PIMDM_DEBUG_PRINTF ("    <S,G> entry :\n");
      osapiSnprintf(printBuf, PIMDM_MAX_IP_STRING_LEN, "Error");
      inetAddrHtop(&(pEntry->source), printBuf);
      PIMDM_DEBUG_PRINTF ("       Source Address : %s\n", printBuf);
      osapiSnprintf(printBuf, PIMDM_MAX_IP_STRING_LEN, "Error");
      inetAddrHtop(&(pEntry->group), printBuf);
      PIMDM_DEBUG_PRINTF ("       Group Address : %s\n", printBuf);
      PIMDM_DEBUG_PRINTF ("       Include Bitmap :");
      for(i = 1; i <= PIMDM_INTF_BIT_SIZE; i++)
      {
        if (BITX_TEST (&pEntry->incl, i) != 0)
          PIMDM_DEBUG_PRINTF (" %02d", i);
      }
      PIMDM_DEBUG_PRINTF ("\n");
      PIMDM_DEBUG_PRINTF ("       Exclude Bitmap :");
      for(i = 1; i <= PIMDM_INTF_BIT_SIZE; i++)
      {
        if (BITX_TEST (&pEntry->excl, i) != 0)
          PIMDM_DEBUG_PRINTF (" %02d", i);
      }
      PIMDM_DEBUG_PRINTF ("\n");
    }
  }
}

/*********************************************************************
*
* @purpose  Search the PIMDM Mgmd tree to get next  entry 
*
* @param    pimdmCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr @b{ (input) } Pointer Source Address
*           grpAddr @b{ (input) } Pointer Group Address
*                            
* @returns  Pointer to the PIMDM Mgmd Entry if found else return NULL.
*
* @comments None
*
* @end
*********************************************************************/
pimdmLocalMemb_t*
pimdmMgmdEntryNextGet (pimdmCB_t *pimdmCB,
                      L7_inet_addr_t *srcAddr,
                      L7_inet_addr_t *grpAddr)
{
  pimdmLocalMemb_t *mgmdEntry = L7_NULLPTR;
  pimdmLocalMemb_t tempMgmdEntry;

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");*/

  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Invalid Control Block");
    return mgmdEntry;
  }

  /* Update the srcAddr and grpAddr information */
  memset (&tempMgmdEntry, 0, sizeof (pimdmLocalMemb_t));
  inetAddressZeroSet(pimdmCB->addrFamily,&(tempMgmdEntry.source));
  inetAddressZeroSet(pimdmCB->addrFamily,&(tempMgmdEntry.group));

  if (srcAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMgmdEntry.source), srcAddr);
  }

  if (grpAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMgmdEntry.group), grpAddr);
  }

  /* Lookup for the matching srcAddr and grpAddr entry in the Tree */
  mgmdEntry = (pimdmLocalMemb_t*)
              avlSearchLVL7 (&pimdmCB->localMembTree, (void*) (&tempMgmdEntry),
              AVL_NEXT);

  /*PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");*/
  return mgmdEntry;
}

/*********************************************************************
*
* @purpose  Search the PIMDM Mgmd tree to get next  entry 
*
* @param    pimdmCB  @b{ (input) } Pointer PIMDM Control Block
*           rtrIfNum @b{ (input) } Router interface number
*                            
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t
pimdmMgmdIntfDownUpdate (pimdmCB_t *pimdmCB,
                         L7_uint32 rtrIfNum)
{
  pimdmLocalMemb_t *mgmdEntry = L7_NULLPTR;
  pimdmLocalMemb_t *nextMgmdEntry = L7_NULLPTR;
  L7_inet_addr_t *srcAddr = L7_NULLPTR;
  L7_inet_addr_t *grpAddr = L7_NULLPTR;
  L7_RC_t retVal = L7_SUCCESS;
  L7_BOOL lclMemshipChange = L7_FALSE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Clear the respective entries from the MGMD database */
  mgmdEntry = pimdmMgmdEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR); 
  while (mgmdEntry != L7_NULLPTR)
  {
    srcAddr = &mgmdEntry->source;
    grpAddr = &mgmdEntry->group;

    nextMgmdEntry = pimdmMgmdEntryNextGet (pimdmCB, srcAddr, grpAddr);

    if ((inetIsAddressZero (srcAddr) == L7_TRUE) && /* Check for (*,G) */
        (inetIsAddressZero (grpAddr) != L7_TRUE))
    {
      /* Clear the interface in the (*,G) Include Bitmap and Update the respective
       * MRT Entry in case of any change.
       */
      if (pimdmMgmdMembershipStarGDelete (pimdmCB, grpAddr, rtrIfNum)
                                       == L7_TRUE)
      {
        PIMDM_TRACE (PIMDM_DEBUG_MGMD, "Change in (*,G, Inc) for rtrIfNum - %d",
                     rtrIfNum);
        lclMemshipChange = L7_TRUE;
      }
    }
    else if ((inetIsAddressZero (srcAddr) != L7_TRUE) && /* Check for (S,G) */
             (inetIsAddressZero (grpAddr) != L7_TRUE))
    {
      /* Clear the interface in the (S,G) Include Bitmap and Update the respective
       * MRT Entry in case of any change.
       */
      if (pimdmMgmdMembershipSGInclDelete (pimdmCB, grpAddr, srcAddr, rtrIfNum)
                                        == L7_TRUE)
      {
        PIMDM_TRACE (PIMDM_DEBUG_MGMD, "Change in (S,G, Incl) for rtrIfNum - %d",
                     rtrIfNum);
        lclMemshipChange = L7_TRUE;
      }

      /* Clear the interface in the (S,G) Exclude Bitmap and Update the respective
       * MRT Entry in case of any change.
       */
      if (pimdmMgmdMembershipSGExclDelete (pimdmCB, grpAddr, srcAddr, rtrIfNum)
                                        == L7_TRUE)
      {
        PIMDM_TRACE (PIMDM_DEBUG_MGMD, "Change in (S,G, Excl) for rtrIfNum - %d",
                     rtrIfNum);
        lclMemshipChange = L7_TRUE;
      }
    }
    else
    {
      /* Nothing to be done here */
    }

    if (lclMemshipChange == L7_TRUE)
    {
      PIMDM_TRACE (PIMDM_DEBUG_MGMD, "Change in Local Membership Change for "
                   "rtrIfNum - %d", rtrIfNum);
      /* TBD: To check if we need to compute the OIF List again. */
    }

    mgmdEntry = nextMgmdEntry;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

