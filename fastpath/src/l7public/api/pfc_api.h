#ifndef PFC_API_H
#define PFC_API_H
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2009
*
**********************************************************************
*
* @filename pfc_api.h
*
* @purpose Priority Flow Control API definitions
*
* @component pfc
*
* @comments none
*
* @create 06/29/2009
*
* @author 
* @end
*
**********************************************************************/
#include "datatypes.h"

/*********************************************************************
* @purpose  Set the PFC participation mode for an interface
*
* @param    intIfNum   @b((input)) The internal interface
* @param    mode       @b((input)) PFC_MODE_DISABLE or PFC_MODE_ENABLE
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t PfcModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Set the drop mode for a priority on an interface
*
* @param    intIfNum   @b((input)) The internal interface
* @param    priority   @b((input)) The priority
* @param    mode       @b((input)) PFC_PRI_DROP_MODE for best effort (lossy) or 
                                   PFC_PRI_NODROP_MODE for lossless 
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @return   L7_NO_VALUE       - bad arguments
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t PfcPriorityModeSet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 mode);

/*********************************************************************
* @purpose  Gets the number of PFC packets received per interface/priority
*
* @param    intIfNum   @b((input)) The internal interface
* @param    priority   @b((input)) The priority
* @param    *cnt       @b((output)) The number of pfc packets rx on this priority
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t PfcRxPriorityStatGet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 *cnt);

/*********************************************************************
* @purpose  Get the number of pfc packets received on an interface
*
* @param    intIfNum    @b((input)) The internal interface
* @param    *cnt        @b((output)) The number of received pfc packets
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t PfcRxStatGet(L7_uint32 intIfNum, L7_uint32 *cnt);

/*********************************************************************
* @purpose  Get the number of pfc packets transmitted on an interface
*
* @param    intIfNum  @b((input)) The internal interface
* @param    *cnt      @b((output)) The number of transmitted pfc packets
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t PfcTxStatGet(L7_uint32 intIfNum, L7_uint32 *cnt);

/*********************************************************************
* @purpose  Clear the PFC statistics for an interface
*
* @param    intIfNum @b((input))  The internal interface
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t PfcStatsClear(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the PFC participation mode for the interface
*
* @param    intIfNum  @b((input))  The internal interface
* @param    *mode     @b((output)) The particapation mode
*
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t PfcModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets the PFC Priority participation mode for the interface
*
* @param    intIfNum  @b((input))  The internal interface
* @param    priority  @b((input))  The priority
* @param    *mode     @b((output)) The drop mode of the priority
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t PfcPriorityModeGet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 *mode);

/*********************************************************************
* @purpose  Determine interface type validity for use with PFC component
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments At present, only physical interfaces are supported.
*
* @end
*********************************************************************/
L7_BOOL pfcIntfTypeIsValid(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Verify specified config interface index is valid
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pfcIntfIndexGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential queue config interface index
*
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pfcIntfIndexGetNext(L7_uint32 intIfNum, L7_uint32 *pNext);

/*********************************************************************
* @purpose  Verify specified priority index exists
*
* @param    queueId     @b{(input)}  Queue id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pfcPriorityIndexGet(L7_uint32 priority);

/*********************************************************************
* @purpose  Determine next sequential priority index
*
* @param    priority    @b{(input)}  Priority
* @param    *pNext      @b{(output)} Ptr to next priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pfcPriorityIndexGetNext(L7_uint32 priority, L7_uint32 *pNext);

#endif
