/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mc_data.c
*
* @purpose   Mac Vlan file
*
* @component vlanMac
*
* @comments
*
* @create    5/20/2005
*
* @author   tsrikanth 
*
* @end
*
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include "flex.h"
#include "l7_product.h"
#include "osapi.h"
#include "avl_api.h"


/* Begin Functinal Declarations: vlan_mac_data.h */

/*****************************************************************
* @purpose  Searches for entry in the specified VLAN tree 
*
* @param    mac         @b{(input)}    Mac Address
* @param    pTree       @b{(input)}    pointer to the specified tree    
*  
* @returns  void *      pointer to the item if matched
* @returns  NULL        if item does not exist in the tree
*
* @notes   
*
* @end
*****************************************************************/
void * vlanMacDataSearch(void *pTree, L7_enetMacAddr_t mac)
{
    L7_RC_t rc;
    void *pEntry;
    avlTree_t *p;

    p = pTree;
    rc = osapiSemaTake(p->semId, L7_WAIT_FOREVER);
    pEntry = avlSearchLVL7(pTree, &mac, AVL_EXACT);
    rc = osapiSemaGive(p->semId);

    return(pEntry);
}

/*****************************************************************
* @purpose  Add an entry into the specified tree           
*
* @param    pTree       @b{(input)}    pointer to the specified tree    
* @param    pData       @b{(input)}    pointer to the data to be added
*  
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_RC_t vlanMacEntryAdd(avlTree_t *pTree, void *pData)
{
    void *pEntry;
    L7_RC_t rc;

    rc = L7_SUCCESS;
    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);

    pEntry = avlInsertEntry(pTree, pData);
    if (pEntry == pData)
        rc = L7_FAILURE;
    else if (pEntry)
    {
        /* update the data entry, ensuring not to overlay the next pointer */
        /* Note that all vlan data structures are dependent upon offset_next
           being at the end of the data structure. */
        bcopy(pData, pEntry, pTree->offset_next);
        rc = L7_SUCCESS;
    }

    osapiSemaGive(pTree->semId);
    return rc;
}

/*****************************************************************
* @purpose  Delete an entry from the specified tree           
*
* @param    pTree       @b{(input)}     pointer to the specified tree    
* @param    vlanID      @b{(input)}     pointer to the data to be added
*  
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_RC_t vlanMacEntryDelete(avlTree_t *pTree, L7_enetMacAddr_t mac)
{
    L7_RC_t rc;

    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);
    rc = (L7_NULL == avlDeleteEntry(pTree, &mac)) ? L7_FAILURE : L7_SUCCESS;
    osapiSemaGive(pTree->semId);
    return(rc);
}

/*****************************************************************
* @purpose  Obtain count of VLANs in tree
*
* @param    pTree     @b{(input)}   pointer to the specified tree    
*  
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_uint32 vlanMacDataCount(avlTree_t * pTree)
{
    L7_uint32 count;

    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);
    count = avlTreeCount(pTree);
    osapiSemaGive(pTree->semId);
    return(count);
}

/*****************************************************************
* @purpose  Obtain pointer to first VLAN item in the tree
*
* @param    pTree      @b{(input)} pointer to the specified tree    
*  
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
void * vlanMacFirstDataEntry(avlTree_t *pTree)
{
    void *pData;
    L7_enetMacAddr_t mac;

    memset(mac.addr, 0x00, sizeof mac);
    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);
    pData = avlSearchLVL7(pTree, &mac, AVL_NEXT);
    osapiSemaGive(pTree->semId);
    return(pData);
}

/*****************************************************************
* @purpose  Obtain pointer to first VLAN item in the tree
*
* @param    pTree   @b{(input)}    pointer to the specified tree    
* @param    mac        @b{(output)} mac address
*  
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
void * vlanMacNextDataEntry(avlTree_t *pTree, L7_enetMacAddr_t mac)
{
    void *pData;

    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);
    pData = avlSearchLVL7(pTree, &mac, AVL_NEXT);
    osapiSemaGive(pTree->semId);
    return(pData);
}

/*****************************************************************
* @purpose  Check wether the entry exist or not
*
* @param    pTree      @b{(input)} pointer to the specified tree
* @param    mac        @b{(output)} mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************/
void *  vlanMacDataEntry(avlTree_t *pTree, L7_enetMacAddr_t mac)
{
    void *pData;

    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);
    pData = avlSearchLVL7(pTree, &mac, AVL_EXACT);
    osapiSemaGive(pTree->semId);
    return(pData);
}

/* End Function Declarations */
