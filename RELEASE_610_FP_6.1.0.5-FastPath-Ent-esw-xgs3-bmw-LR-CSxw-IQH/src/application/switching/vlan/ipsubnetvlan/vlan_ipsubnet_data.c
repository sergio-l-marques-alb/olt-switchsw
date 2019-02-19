/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_data.c
*
* @purpose   IP Subnet Vlan Data Management file
*
* @component vlanIpSubnet
*
* @comments 
*
* @create    5/5/2005
*
* @author    tsrikanth
*
* @end
*             
**********************************************************************/

#include <string.h>
#include "flex.h"
#include "l7_product.h"
#include "osapi.h"
#include "avl_api.h"
#include "vlan_ipsubnet_include.h"

/* Begin Function Declarations: vlan_ipsubet_data.h */

/*****************************************************************
* @purpose  Searches for entry in the specified VLAN tree 
*
* @param    pTree      @b{(input)}  pointer to the specified tree
* @param    searchKey  @b{(input)}  subnet address ANDed with netmask
*
* @returns  void *     @b{(output)}    pointer to the item if matched
* @returns  L7_NULL    @b{(output)}    if item does not exist in the tree
*
* @comments 
*
* @end
*****************************************************************/
void *vlanIpSubnetDataSearch(void *pTree, L7_uint32 subnet, L7_uint32 netmask)
{
    L7_RC_t rc;
    void *pEntry;
    avlTree_t *p;
    L7_uchar8 searchKey[VLAN_IPSUBNET_KEYSIZE];

    memcpy(&searchKey[0], &subnet, 4);
    memcpy(&searchKey[4], &netmask, 4);

    p = pTree;
    rc = osapiSemaTake(p->semId, L7_WAIT_FOREVER);
    pEntry = avlSearchLVL7(pTree, searchKey, AVL_EXACT);
    rc = osapiSemaGive(p->semId);

    return(pEntry);
}


/*****************************************************************
* @purpose  Add an entry into the specified VLAN tree           
*
* @param    pTree   @b{(input)}    pointer to the specified tree
* @param    pData   @b{(input)}    pointer to the data to be added
*  
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_RC_t vlanIpSubnetEntryAdd(avlTree_t *pTree, void *pData)
{
    void *pEntry;
    L7_RC_t rc = L7_FAILURE;

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
* @purpose  Delete an entry from the specified VLAN tree           
*
* @param    pTree   @b{(input)}    pointer to the specified tree
* @param    subnet  @b{(input)}    Subnet address
*  
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
L7_RC_t vlanIpSubnetEntryDelete(avlTree_t *pTree, L7_uint32 subnet, L7_uint32 netmask)
{
    L7_RC_t rc;
    L7_uchar8 searchKey[VLAN_IPSUBNET_KEYSIZE];

    memcpy(&searchKey[0], &subnet, 4);
    memcpy(&searchKey[4], &netmask, 4);

    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);
    rc = (L7_NULL == avlDeleteEntry(pTree, searchKey)) ? L7_FAILURE : L7_SUCCESS;
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
L7_uint32 vlanIpSubnetDataCount(avlTree_t *pTree)
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
void * vlanIpSubnetFirstDataEntry(avlTree_t *pTree)
{
    void *pData;
    L7_uchar8 searchKey[VLAN_IPSUBNET_KEYSIZE] = {0};

    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);
    pData = avlSearchLVL7(pTree, searchKey, AVL_NEXT);
    osapiSemaGive(pTree->semId);
    return(pData);

}
    


/*****************************************************************
* @purpose  Obtain pointer to first VLAN item in the tree
*
* @param    pTree      @b{(input)} pointer to the specified tree
* @param    subnet     @b{(input)} subnet address
*  
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
void *  vlanIpSubnetNextDataEntry(avlTree_t *pTree, L7_uint32 subnet, L7_uint32 netmask)
{
    void *pData;
    L7_uchar8 searchKey[VLAN_IPSUBNET_KEYSIZE];

    memcpy(&searchKey[0], &subnet, 4);
    memcpy(&searchKey[4], &netmask, 4);

    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);
    pData = avlSearchLVL7(pTree, searchKey, AVL_NEXT);
    osapiSemaGive(pTree->semId);
    return(pData);
}



/*****************************************************************
* @purpose  Check wether the entry exist or not
*
* @param    pTree      @b{(input)} pointer to the specified tree
* @param    subnet     @b{(output)} subnet address
*  
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*****************************************************************/
void *  vlanIpSubnetDataEntry(avlTree_t *pTree, L7_uint32 subnet, L7_uint32 netmask)
{
    void *pData;
    L7_uchar8 searchKey[VLAN_IPSUBNET_KEYSIZE];

    memcpy(&searchKey[0], &subnet, 4);
    memcpy(&searchKey[4], &netmask, 4);

    osapiSemaTake(pTree->semId, L7_WAIT_FOREVER);
    pData = avlSearchLVL7(pTree, searchKey, AVL_EXACT);
    osapiSemaGive(pTree->semId);
    return(pData);
}

/* End Function Declarations */
