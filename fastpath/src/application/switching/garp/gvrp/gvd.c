/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gvd.c
* @purpose     GVRP data base
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#include "l7_common.h"
#include "osapi.h"
#include "avl_api.h"
#include "garp.h"
#include "gvrapi.h"
#include "gvd.h"
#include "garpcfg.h"
#include "garp_debug.h"

avlTreeTables_t   gvrpTreeHeap[L7_MAX_VLANS];
Gvd_structure     gvrpDataHeap[L7_MAX_VLANS];
avlTree_t         gvrpTreeData;

Vlan_id           gvrpIndexMap[L7_MAX_VLANS];
static L7_uint32  numberOfEntries;

/*********************************************************************
* @purpose  Initialize the index table    
*
* @param    none
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
static void initGvrpIndexMap()
{
  L7_uint32 i;
  
  for (i=0;i<L7_MAX_VLANS;i++)
  {
    gvrpIndexMap[i] = 0xffff;
  }
  
  numberOfEntries = 0;
}

/*********************************************************************
* @purpose  Insert an index
*
* @param    L7_uint32   key
*
* @returns  void
*
* @notes    Since the map is always packed, inserts are always at the end
*       
* @end
*********************************************************************/
static void InsertGvrpIndex(L7_uint32 key)
{
  gvrpIndexMap[numberOfEntries++] = (Vlan_id)key;
}

/*********************************************************************
* @purpose  Delete entry at index
*
* @param    L7_uint32  index
*
* @returns  void
*
* @notes    Slides the entries from at_index to numberOfEntries ahead
*       
* @end
*********************************************************************/
static void DeleteGvrpIndex(L7_uint32 at_index)
{
  L7_uint32 i;

  if (at_index <= numberOfEntries)
  {
    /* note: possible out of bounds read */
    for (i=at_index;i<numberOfEntries;i++)
    {
      gvrpIndexMap[i] = gvrpIndexMap[i+1];
    }
    
    numberOfEntries--;   
    gvrpIndexMap[numberOfEntries] = 0xffff;
  }
}

/*********************************************************************
* @purpose  Find Index   
*
* @param    L7_uint32  key
* @param    L7_uint32  *index
*
* @returns  L7_SUCCESS, if the key is found in the map table
* @returns  L7_FAILURE, if the key is not found
*
* @notes    Callers should check the return code to ensure *index is valid
*       
* @end
*********************************************************************/
static L7_RC_t FindGvrpIndex(L7_uint32 Key, L7_uint32 *index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  for (i=0;i<numberOfEntries;i++)
  {
    if (gvrpIndexMap[i] == (Vlan_id)Key)
    {
      *index = i;
      rc = L7_SUCCESS;
      break;
    }
  }
  
  return rc;
}

/*********************************************************************
* @purpose  create DB
*
* @PARAM    L7_uint32       vlans
* @param    void *          pointer to DB
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  Creates a new instance of gvd, allocating space for up to L7_MAX_VLANS
*         VLAN IDs.
*
*         Returns True if the creation suceeded together with a pointer to the
*         gvd information.
*       
* @end
*********************************************************************/
L7_BOOL gvd_create_gvd(void **gvd)
{
  L7_BOOL      returnVal = L7_TRUE;
  avlTree_t    *gvrpTreeDataPtr;

  initGvrpIndexMap();

  avlCreateAvlTree(&gvrpTreeData, gvrpTreeHeap, gvrpDataHeap, L7_MAX_VLANS,
                   (L7_uint32)sizeof(Gvd_structure), 0x10, (L7_uint32)sizeof(Vlan_id));
  (void)avlSetAvlTreeComparator(&gvrpTreeData, avlCompareUShort16);

  gvrpTreeDataPtr = &gvrpTreeData;
  
  *gvd = gvrpTreeDataPtr;
  
  return returnVal;
}

/*********************************************************************
* @purpose  destroy DB
*
* @param    none
*
* @returns  None
*
* @notes   Destroys the instance of gvd, releasing previously allocated database and
*          control space.
*       
* @end
*********************************************************************/
void gvd_destroy_gvd(void)
{
  avlPurgeAvlTree (&gvrpTreeData, L7_MAX_VLANS);
  initGvrpIndexMap();    
}

/*********************************************************************
* @purpose  find DB entry 
*
* @param    Vlan_id           key
* @param    L7_uint32         index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
L7_BOOL gvd_find_entry(Vlan_id key, L7_uint32 *found_at_index)
{
  void      *p;
  L7_RC_t   rc;
  L7_uint32 matchType = L7_MATCH_EXACT;
  L7_BOOL   returnVal = L7_FALSE;

  rc = osapiSemaTake(gvrpTreeData.semId, L7_WAIT_FOREVER);

  p = avlSearchLVL7 (&gvrpTreeData, &key, matchType);
  if (p != L7_NULL)
  {
    rc = FindGvrpIndex((L7_uint32)key, found_at_index);
    if (rc == L7_SUCCESS)
      returnVal = L7_TRUE;
  }

  osapiSemaGive(gvrpTreeData.semId);

  return returnVal;
}

/*********************************************************************
* @purpose  create DB entry 
*
* @param    Vlan_id           key
* @param    L7_uint32         index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
L7_BOOL gvd_create_entry(Vlan_id key, L7_uint32 *created_at_index)
{
    Gvd_structure *pData;
    L7_BOOL       returnVal = L7_TRUE;
    Gvd_structure dbEntry;
    L7_RC_t       rc;

    rc = osapiSemaTake(gvrpTreeData.semId, L7_WAIT_FOREVER);
    
    dbEntry.attribute_Value = key;
        
    pData = avlInsertEntry( &gvrpTreeData, &dbEntry);

    rc = osapiSemaGive(gvrpTreeData.semId);

    if (pData == L7_NULL)
    {
        *created_at_index = numberOfEntries;
        InsertGvrpIndex(key);
    }
    else
    {
        returnVal = L7_FALSE;
    }

    GARP_TRACE_INTERNAL_EVENTS("gvd_create_entry: vlan %d, created_at_index %d\n", 
                               key, created_at_index);
    return returnVal;

}

/*********************************************************************
* @purpose  delete DB entry 
*
* @param    delete_at_index    index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
L7_BOOL gvd_delete_entry(L7_uint32 delete_at_index)
{
    Gvd_structure *pData;
    L7_BOOL       returnVal = L7_TRUE;
    L7_RC_t       rc;

    GARP_TRACE_INTERNAL_EVENTS("gvd_delete_entry: delete_at_index %d\n", delete_at_index);

    rc = osapiSemaTake(gvrpTreeData.semId, L7_WAIT_FOREVER);

    pData = avlDeleteEntry (&gvrpTreeData, &gvrpIndexMap[delete_at_index]);

    rc = osapiSemaGive(gvrpTreeData.semId);
    
    if (pData == L7_NULL)
    {
       returnVal = L7_FALSE;
    }
    else
    {
        DeleteGvrpIndex(delete_at_index);
    }
    return returnVal;
}

/*********************************************************************
* @purpose  get vlan id 
*
* @param    L7_uint32         index
* @param    Vlan_id           key
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
L7_BOOL gvd_get_key(L7_uint32 index, Vlan_id *key)
{
    *key = gvrpIndexMap[index];
    return L7_TRUE;
}

/*********************************************************************
* @purpose  returns the number of entries in GVRP database 
*
* @returns  numberOfEntries
*
* @notes  
*       
* @end
*********************************************************************/
L7_uint32 getNumberOfGVDEntries() { return numberOfEntries; }

