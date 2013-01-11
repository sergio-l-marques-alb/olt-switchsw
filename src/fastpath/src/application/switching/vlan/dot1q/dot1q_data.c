/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q_data.c
*
* @purpose 802.1Q Data Management File
*
* @component dot1q
*
* @comments none
*
* @create 08/14/2000
*
* @author wjacobs
*
* @end
*             
**********************************************************************/

#include <string.h>
#include "flex.h"
#include "dot1q_common.h"
#include "l7_product.h"
#include "osapi.h"
#include "avl_api.h"
#include "dot1q_data.h"

/*****************************************************************
* @purpose  Searches for entry in the specified VLAN tree 
*
* @param    pTree       pointer to the specified tree    
* @param    vlanID      VLAN ID
*  
* @returns  void *      pointer to the item if matched
* @returns  NULL        if item does not exist in the tree
*
* @notes   
*
* @end
*****************************************************************/
void * vlanDataSearch(void *pTree, L7_uint32 vlanID)
{
  L7_RC_t    rc;
  void       *pEntry;
  vlanTree_t *p;

  p = pTree;

  rc = osapiSemaTake(p->semId, L7_WAIT_FOREVER);
  pEntry = avlSearchLVL7(pTree, &vlanID, AVL_EXACT);
  rc = osapiSemaGive(p->semId);

  return(pEntry);
}  /* vlanDataSearch */



/*****************************************************************
* @purpose  Searches for entry in the specified VLAN tree and copies
*			the contents of the entry into the pointer provided
*
* @param    @in   pTree       pointer to the specified tree    
* @param    @in   vlanID      VLAN ID
* @param    @out  pData       pointer to the item if matched
* 
* @returns	L7_BOOL  L7_TRUE if entry present, L7_FALSE if not.
*
* @notes   
*
* @end
*****************************************************************/
L7_BOOL	  vlanDataGet(void *pTree, L7_uint32 vlanID, dot1q_vlan_dataEntry_t *pData)
{
  L7_BOOL    entryFound = L7_FALSE;
  void       *pEntry;
  vlanTree_t *p;

  p = pTree;

  osapiSemaTake(p->semId, L7_WAIT_FOREVER);
  pEntry = avlSearchLVL7(pTree, &vlanID, AVL_EXACT);

  if (pEntry != L7_NULLPTR)
  {
	 memcpy(pData,pEntry,sizeof(dot1q_vlan_dataEntry_t));
	 entryFound = L7_TRUE;

  }
  osapiSemaGive(p->semId);

  return entryFound;
}  



/*****************************************************************
* @purpose  Add an entry into the specified tree           
*
* @param    pTree       pointer to the specified tree    
* @param    pData       pointer to the data to be added
*  
* @returns  rc          L7_SUCCESS or L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_RC_t vlanEntryAdd(vlanTree_t *pTree, void * pData)
{
  void *pEntry;
  L7_RC_t rc;

  rc = L7_SUCCESS;

  osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);

  pEntry = avlInsertEntry(pTree, pData);

  if (pEntry == pData)
    rc = L7_FAILURE;
  else
    if (pEntry)
  {   
    /* update the data entry, ensuring not to overlay the next pointer */
    /* Note that all vlan data structures are dependent upon offset_next
       being at the end of the data structure. */
    bcopy(pData, pEntry,  pTree->offset_next );
    rc = L7_SUCCESS;
  }

  osapiSemaGive(pTree->semId);

  return rc;

}  /* vlanEntryAdd */

/*****************************************************************
* @purpose  Delete an entry from the specified tree           
*
* @param    pTree       pointer to the specified tree    
* @param    vlanID      pointer to the data to be added
*  
* @returns  rc          L7_SUCCESS or L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_RC_t vlanEntryDelete(vlanTree_t *pTree, L7_uint32 vlanID)
{
  L7_RC_t rc;

  osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);

  rc = (L7_NULL == avlDeleteEntry(pTree, &vlanID)) ? L7_FAILURE : L7_SUCCESS;

  osapiSemaGive(pTree->semId);

  return rc;

}  /* vlanEntryDelete */

/*****************************************************************
* @purpose  Obtain count of VLANs in tree
*
* @param    pTree       pointer to the specified tree    
*  
* @returns  rc          L7_SUCCESS or L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_uint32 vlanDataCount(vlanTree_t *pTree)
{
  L7_uint32 count;

  osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);

  count = avlTreeCount(pTree);

  osapiSemaGive(pTree->semId);

  return count;

}  /* vlanDataCount */


/*****************************************************************
* @purpose  Obtain pointer to first VLAN item in the tree
*
* @param    pTree       pointer to the specified tree    
*  
* @returns  rc          L7_SUCCESS or L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
void* vlanFirstDataEntry(vlanTree_t *pTree)
{
  void *pData;
  L7_uint32 vlanID;

  vlanID =  L7_DOT1Q_NULL_VLAN_ID;

  osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);

  pData = avlSearchLVL7(pTree, &vlanID, AVL_NEXT);

  osapiSemaGive(pTree->semId);

  return(pData);

}  /* vlanFirstDataEntry */


/*****************************************************************
* @purpose  Obtain pointer to first VLAN item in the tree
*
* @param    pTree       pointer to the specified tree    
*  
* @returns  rc          L7_SUCCESS or L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
void* vlanNextDataEntry(vlanTree_t *pTree, L7_uint32 vlanID)
{
  void *pData;

  osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);

  pData = avlSearchLVL7(pTree, &vlanID, AVL_NEXT);

  osapiSemaGive(pTree->semId);

  return(pData);

}  /* vlanFirstDataEntry */

/*****************************************************************
* @purpose  Obtain a copy of the Next VLAN item in the tree
*			Note the get indicates that this functions assumes 
*			read only action
*
* @param    pTree       pointer to the specified tree    
*  
* @returns  rc          L7_SUCCESS or L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_RC_t vlanNextDataEntryGet(vlanTree_t *pTree, L7_uint32 vlanID, dot1q_vlan_dataEntry_t *pData)
{
  void *pEntry;
  L7_RC_t rc = L7_FAILURE;

  osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);

  pEntry = avlSearchLVL7(pTree, &vlanID, AVL_NEXT);

  if (pEntry != L7_NULLPTR)
  {
	  memcpy(pData,pEntry,sizeof(dot1q_vlan_dataEntry_t));
	  rc = L7_SUCCESS;
  }

  osapiSemaGive(pTree->semId);

  return rc;

} 

/*****************************************************************
* @purpose  Obtain a copy of the first VLAN item in the tree
*			Note the get indicates that this functions assumes 
*			read only action
*
* @param    pTree       pointer to the specified tree    
*  
* @returns  rc          L7_SUCCESS or L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_RC_t vlanFirstDataEntryGet(vlanTree_t *pTree, dot1q_vlan_dataEntry_t *pData)
{
  return vlanNextDataEntryGet(pTree,L7_DOT1Q_NULL_VLAN_ID, pData);
}  
