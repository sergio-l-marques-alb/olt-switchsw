/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_ifindex.c
*
* @purpose    The functions that handle the external intf id or ifindex
*
* @component  NIM
*
* @comments   none
*
* @create     05/07/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "nim_ifindex.h"
#include "avl_api.h"
#include "nim_util.h" /* needed for NIM_LOG_MSG */
#include <string.h>

static avlTree_t              nimIfIndexTreeData  = { 0 };
static void                   *nimIfIndexSema     = L7_NULL;

/* Macros for protecting the AVL tree during operations */
#define NIM_IFINDEX_CRIT_SEC_ENTER()  \
{   \
    osapiSemaTake(nimIfIndexSema,L7_WAIT_FOREVER);  \
}

#define NIM_IFINDEX_CRIT_SEC_EXIT()  \
{   \
    osapiSemaGive(nimIfIndexSema);  \
}

/*********************************************************************
* @purpose  Create an IfIndex
*
* @param    usp         @b{(input)}   The usp to encode in the ifIndex
* @param    ifIndex     @b{(output)}  The ifIndex created
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes   
*
*       
* @end
*
*********************************************************************/
void nimIfIndexCreate(nimUSP_t usp,L7_INTF_TYPES_t type,L7_uint32 *ifIndex, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  nimIfIndexTreeData_t data;
  nimIfIndexTreeData_t *pData;

  if (ifIndex == L7_NULLPTR)
  {
    NIM_LOG_MSG("NIM: Attempted dereferencing of NULL\n");
  }
  else
  {
    switch (type)
    {
      case L7_PHYSICAL_INTF:
      case L7_STACK_INTF:
      case L7_CPU_INTF:
        break;
      case L7_LAG_INTF:
      case L7_LOGICAL_VLAN_INTF:
      case L7_LOOPBACK_INTF:
      case L7_TUNNEL_INTF:
      case L7_WIRELESS_INTF:
      case L7_CAPWAP_TUNNEL_INTF:
      case L7_VLAN_PORT_INTF:     /* PTin added: virtual ports */
        usp.unit = L7_LOGICAL_UNIT;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }

    if (rc == L7_SUCCESS)
    {
      /* *ifIndex = (usp.unit << NIM_UNIT_SHIFT) | (usp.slot << NIM_SLOT_SHIFT) | (usp.port << NIM_PORT_SHIFT); */
      *ifIndex = intIfNum;

      data.ifIndex = *ifIndex;
      data.intIfNum = intIfNum;

      NIM_IFINDEX_CRIT_SEC_ENTER();

      pData = avlInsertEntry(&nimIfIndexTreeData, &data);

      NIM_IFINDEX_CRIT_SEC_EXIT();

      if (pData != L7_NULL)
      {
        NIM_LOG_MSG("NIM: ifIndex not added to the AVL tree\n");
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Delete an IfIndex
*
* @param    ifIndex     @b{(output)}  The ifIndex to delete
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes   
*
*       
* @end
*
*********************************************************************/
void nimIfIndexDelete(L7_uint32 ifIndex)
{
  nimIfIndexTreeData_t *pData;
  nimIfIndexTreeData_t data;

  data.ifIndex = ifIndex;

  NIM_IFINDEX_CRIT_SEC_ENTER();

  pData = avlDeleteEntry(&nimIfIndexTreeData, &data);

  NIM_IFINDEX_CRIT_SEC_EXIT();

  if (pData == L7_NULL)
  {
    NIM_LOG_MSG("NIM: ifIndex %d not found, cannot delete it\n",ifIndex);
  }

  return;
}

/*********************************************************************
* @purpose  Allocate the memory for the ifIndex AVL tree
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimIfIndexPhaseOneInit()
{
  L7_RC_t   rc = L7_SUCCESS;

  do
  {

    if (avlAllocAndCreateAvlTree(&nimIfIndexTreeData,
                                 L7_NIM_COMPONENT_ID,
                                 platIntfTotalMaxCountGet(),
                                 sizeof(nimIfIndexTreeData_t),
                                 0x10, avlCompareULong32,
                                 sizeof(L7_uint32)) != L7_SUCCESS)
    {
      NIM_L7_LOG_ERROR("NIM: Unable to allocate resources\n");
      break; /* goto while */
    }

    nimIfIndexSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY); 

    if (nimIfIndexSema == L7_NULLPTR)
    {
      NIM_L7_LOG_ERROR("NIM: unable to create the ifIndex Sema\n");
    }

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Find the next external inferface number
*
* @param    extIfNum      @b{(input)}  The present extIfNum
* @param    pNextExtIfNum @b{(output)} The next extIfNum
* @param    pIntIfNum     @b{(output)} The internal interface number
*
* @returns  L7_ERROR if the extIfNum doesn't exist
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimIfIndexNextGet(L7_uint32 extIfNum, L7_uint32 *pNextExtIfNum,L7_uint32 *pIntIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  nimIfIndexTreeData_t *pData;

  NIM_IFINDEX_CRIT_SEC_ENTER();

  pData = avlSearchLVL7 (&nimIfIndexTreeData, &extIfNum, AVL_NEXT);

  NIM_IFINDEX_CRIT_SEC_EXIT();

  if (pData != L7_NULL)
  {
    *pNextExtIfNum = pData->ifIndex;
    *pIntIfNum = pData->intIfNum;
  }
  else
  {
    rc = L7_ERROR;
  }
  return rc;
}


/*********************************************************************
* @purpose  Find the first external inferface number
*
* @param    pExtIfNum      @b{(input)}  The present extIfNum
* @param    pIntIfNum      @b{(output)} The internal interface number
*
* @returns  L7_ERROR if the extIfNum doesn't exist
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimIfIndexFirstGet(L7_uint32 *pExtIfNum,L7_uint32 *pIntIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  nimIfIndexTreeData_t *pData;
  L7_uint32 ifIndex = 0;

  NIM_IFINDEX_CRIT_SEC_ENTER();

  pData = avlSearchLVL7 (&nimIfIndexTreeData, &ifIndex, AVL_NEXT);

  NIM_IFINDEX_CRIT_SEC_EXIT();

  if (pData != L7_NULL)
  {
    *pExtIfNum = pData->ifIndex;
    *pIntIfNum = pData->intIfNum;

  }
  else
  {
    rc = L7_ERROR;
  }
  return rc;
}

/*********************************************************************
* @purpose  Find the intIfNum associated with the extIfNum 
*
* @param    extIfNum      @b{(input)}  The present extIfNum
* @param    pNextExtIfNum  @b{(output)} The next extIfNum
*
* @returns  L7_ERROR if the extIfNum doesn't exist
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimIfIndexIntIfNumGet(L7_uint32 extIfNum, L7_uint32 *pIntIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  nimIfIndexTreeData_t *pData;

  NIM_IFINDEX_CRIT_SEC_ENTER();

  pData = avlSearchLVL7 (&nimIfIndexTreeData, &extIfNum, AVL_EXACT);

  NIM_IFINDEX_CRIT_SEC_EXIT();

  if (pData != L7_NULL)
  {
    *pIntIfNum= pData->intIfNum;
  }
  else
  {
    rc = L7_ERROR;
  }
  return rc;
}




/*********************************************************************
* @purpose  Find the intIfNum associated with the extIfNum 
*
* @param    extIfNum      @b{(input)}  The present extIfNum
* @param    pNextExtIfNum  @b{(output)} The next extIfNum
*
* @returns  L7_ERROR if the extIfNum doesn't exist
*
* @notes    none 
*
* @end
*********************************************************************/
void nimIfIndexDataClear(void)
{

  NIM_IFINDEX_CRIT_SEC_ENTER();

  avlPurgeAvlTree(&nimIfIndexTreeData, platIntfTotalMaxCountGet());

  NIM_IFINDEX_CRIT_SEC_EXIT();
}


