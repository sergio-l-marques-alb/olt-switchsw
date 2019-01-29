/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename ospf_mib_assist.c
*
* @purpose Functions to assist in OSPF MIB support
*
* @component ospf_map
*
* @comments none
*
* @create 07/14/2001
*
* @author wjacobs
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <string.h>
#include "l7_common.h"
#include "l7_common_l3.h"
#include "l3_commdefs.h"
#include "sysapi.h"
#include "osapi.h"
#include "avl_api.h"
#include "l7_ospfinclude.h"
/*
* m2Lib.h in VxWorks 5.4.2 has Mib II defines identical to the vendor code, so this include must
* be after all others.
*/

/* globals for area id avl tree */
avlTreeTables_t         ospmAreaIdTreeHeap[L7_OSPF_MAX_AREAS];
ospfmap_areaIdSearch_t  ospmAreaIdDataHeap[L7_OSPF_MAX_AREAS];
avlTree_t               ospfmAreaIdTreeData;

/* 2/18/2005. Similar AVL tree for area ranges removed. Having a single tree
 * for OSPF doesn't work because ranges are specific to an OSPF area. Generally
 * you want to walk ranges within a given area. Having a single tree doesn't 
 * easily allow this. Having a tree per area, allocated at system init time, 
 * would be a waste of memory. */



/*********************************************************************
* @purpose  insert area id into avl tree 
*
* @param    areaId      area ID
*
* @returns  L7_SUCCESS  if, successful
* @returns  L7_FAILURE  if, successful
*
* @notes    This function is provided for OSPF MIB support only.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaIdMibTblInsert(L7_IP_ADDR_t areaId)
{
  ospfmap_areaIdSearch_t data;
  ospfmap_areaIdSearch_t *pData;

  memset(( void * )&data, 0, sizeof( ospfmap_areaIdSearch_t)); 
  data.areaId = areaId;

  osapiSemaTake(ospfmAreaIdTreeData.semId, L7_WAIT_FOREVER);

  pData = avlInsertEntry(&ospfmAreaIdTreeData, &data);

  osapiSemaGive(ospfmAreaIdTreeData.semId);

  /* We have run out of heap space so return failure */  
  if (pData == &data)
    return L7_FAILURE;
  
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  delete area id into avl tree 
*
* @param    areaId      area ID
*
* @returns  L7_SUCCESS  if, successful
* @returns  L7_FAILURE  if, successful
*
* @notes    This function is provided for OSPF MIB support only.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaIdMibTblDelete(L7_IP_ADDR_t areaId)
{
  ospfmap_areaIdSearch_t data;
  ospfmap_areaIdSearch_t *pData;
  L7_RC_t rc;

  data.areaId = areaId;

  osapiSemaTake(ospfmAreaIdTreeData.semId, L7_WAIT_FOREVER);

  pData = avlDeleteEntry(&ospfmAreaIdTreeData, &data);

  osapiSemaGive(ospfmAreaIdTreeData.semId);

  if (!pData)
    rc = L7_FAILURE;
  else
    rc = L7_SUCCESS;

  return(rc);
}

/*********************************************************************
* @purpose  find OSPF Area ID in avl tree    
*
* @param    pAreaId     pointer to an area ID   
* @param    matchType   L7_MATCH_EXACT or L7_MATCH_GETNEXT
* @parm     pData       pointer to an ospfmap_areaIdSearch_t structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    This function is provided for OSPF MIB support only.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaIdMibTblFind(L7_IP_ADDR_t *pAreaId, L7_uint32 matchType, 
                                ospfmap_areaIdSearch_t *pData)
{
  ospfmap_areaIdSearch_t *pAreaData;

  osapiSemaTake(ospfmAreaIdTreeData.semId, L7_WAIT_FOREVER);

  pAreaData = avlSearchLVL7 (&ospfmAreaIdTreeData, pAreaId, matchType);

  if (pAreaData != L7_NULL)
    memcpy(pData, pAreaData, sizeof(ospfmap_areaIdSearch_t));       

  osapiSemaGive(ospfmAreaIdTreeData.semId);

  if (pAreaData == L7_NULL)
  {
    return(L7_FAILURE);
  }
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Initialize OSPF MAP Mib Support parameters
*
*
* @returns  L7_SUCCESS  If was successful
* @returns  L7_FAILURE  If was not successful
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapMibSupportInit()
{
  L7_RC_t rc = L7_SUCCESS; 

  /* Create avl tree for ospf areas */
  avlCreateAvlTree(&ospfmAreaIdTreeData, ospmAreaIdTreeHeap, ospmAreaIdDataHeap, 
                   L7_OSPF_MAX_AREAS,
                   sizeof(ospfmap_areaIdSearch_t), 0x10, sizeof(L7_IP_ADDR_t));
  (void)avlSetAvlTreeComparator(&ospfmAreaIdTreeData, avlCompareIPAddr);

  return rc; 
}

/*********************************************************************
* @purpose  Finis OSPF MAP Mib Support parameters
*
*
* @returns  L7_SUCCESS  If was successful
* @returns  L7_FAILURE  If was not successful
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapMibSupportFini()
{
  L7_RC_t rc = L7_SUCCESS; 

  /* Create avl tree for ospf areas */
  avlPurgeAvlTree(&ospfmAreaIdTreeData, L7_OSPF_MAX_AREAS);

  osapiSemaDelete(&ospfmAreaIdTreeData.semId);

  return rc; 
}



