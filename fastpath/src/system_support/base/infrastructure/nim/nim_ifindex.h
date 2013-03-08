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

#ifndef NIM_IFINDEX_H
#define NIM_IFINDEX_H

#include "l7_common.h"
#include "nim_data.h"

/* Shifts for creating unique IF_INDICES */
#define NIM_UNIT_MASK 0xff000000
#define NIM_SLOT_MASK 0x00ff0000
#define NIM_PORT_MASK 0x0000ffff

#define NIM_UNIT_SHIFT 24
#define NIM_SLOT_SHIFT 16
#define NIM_PORT_SHIFT  0


typedef struct {
  L7_uint32 ifIndex;
  L7_uint32 intIfNum;
  void *avlData;
} nimIfIndexTreeData_t;

/*********************************************************************
* @purpose  Get the unit id from ifIndex  
*
*
* @param    ifIndex
*
* @returns  unit id
*
* @notes   
*
*       
* @end
*
*********************************************************************/
L7_uint32 nimIfIndexGetUnit(L7_uint32 ifIndex);


/*********************************************************************
* @purpose  Get the slot id from ifIndex  
*
* @param    ifIndex
*
* @returns  slot id
*
* @notes   
*
*       
* @end
*
*********************************************************************/
L7_uint32 nimIfIndexGetSlot(L7_uint32 ifIndex);


/*********************************************************************
* @purpose  Get the port id from ifIndex  
*
* @param    ifIndex
*
* @returns  port id
*
* @notes   
*
*       
* @end
*
*********************************************************************/
L7_uint32 nimIfIndexGetPort(L7_uint32 ifIndex);

/*********************************************************************
* @purpose  Create an IfIndex
*
* @param    usp         @b{(input)}   The usp to encode in the ifIndex
* @param    type        @b{(input)}   The type of interface 
* @param    intIfNum    @b{(input)}   The intIfNum to marry the extIfNum to
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
void nimIfIndexCreate(nimUSP_t usp,L7_INTF_TYPES_t type,L7_uint32 *ifIndex, L7_uint32 intIfNum);

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
void nimIfIndexDelete(L7_uint32 ifIndex);

/*********************************************************************
* @purpose  Find the first external inferface number
*
* @param    pExtIfNum      @b{(input)}  The present extIfNum
* @param    pIntIfNum  @b{(output)} The internal interface number
*
* @returns  L7_ERROR if the extIfNum doesn't exist
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimIfIndexFirstGet(L7_uint32 *pExtIfNum, L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Find the next external inferface number
*
* @param    extIfNum      @b{(input)}  The present extIfNum
* @param    pNextExtIfNum  @b{(output)} The next extIfNum
* @param    pIntIfNum  @b{(output)} The internal interface number
*
* @returns  L7_ERROR if the extIfNum doesn't exist
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimIfIndexNextGet(L7_uint32 extIfNum, L7_uint32 *pNextExtIfNum, L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Find the intIfNum associated with the extIfNum 
*
* @param    extIfNum   @b{(input)}  The present extIfNum
* @param    pIntIfNum  @b{(output)} The internal interface number
*
* @returns  L7_ERROR if the extIfNum doesn't exist
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimIfIndexIntIfNumGet(L7_uint32 extIfNum, L7_uint32 *pIntIfNum);

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
L7_RC_t nimIfIndexPhaseOneInit();

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
void nimIfIndexDataClear(void);
#endif /* NIM_IFINDEX_H */
