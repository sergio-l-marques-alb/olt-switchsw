#ifndef USMDB_PFC_H
#define USMDB_PFC_H

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
*
* @filename usmdb_qos_cos.c
*
* @purpose Provide interface to API's for PFC component
*
* @component pfc
*
* @comments none
*
* @create 6/30/2009
*
* @author 
* @end
*
**********************************************************************/
#include "datatypes.h"
#include "commdefs.h"
#include "nimapi.h"
#include "pfc_api.h"

/*********************************************************************
* @purpose  Get the active status on the interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    status      @b{(output)} The active status of the interface
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPfcStatusGet(L7_uint32 intIfNum, L7_uint32 *status);

/*********************************************************************
* @purpose  Get the pfc mode for the interfac3
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    mode        @b{(input)} enabled for pfc or not
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPfcModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
* @purpose  Enable an interface for pfc
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    mode        @b{(input)} enabled for pfc or not
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPfcModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the drop mode for the priority on an interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    priority    @b{(input)}  Priority to get
* @param    mode        @b{(output)} The drop mode of the interface
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPfcPriorityModeGet(L7_uint32 intIfNum, 
                                  L7_uint32 priority, 
                                  L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the drop mode for the priority on an interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    priority    @b{(input)}  Priority to get
* @param    cnt         @b{(input)} The drop mode of the interface
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPfcPriorityModeSet(L7_uint32 intIfNum, 
                                  L7_uint32 priority, 
                                  L7_uint32 mode);

/*********************************************************************
* @purpose  Get the PFC Rx Priority stat on an interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    priority    @b{(input)}  Priority to get
* @param    cnt         @b{(output)} The statistic
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPfcRxPriorityStatGet(L7_uint32 intIfNum, 
                                    L7_uint32 priority, 
                                    L7_uint32 *cnt);

/*********************************************************************
* @purpose  Get the PFC Rx stat on an interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    cnt         @b{(output)} The statistic
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPfcRxStatGet(L7_uint32 intIfNum, L7_uint32 *cnt);

/*********************************************************************
* @purpose  Get the PFC Tx stat on an interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    cnt         @b{(output)} The statistic
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPfcTxStatGet(L7_uint32 intIfNum, L7_uint32 *cnt);

/*********************************************************************
* @purpose  Clear the PFC stats on an interface
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPfcStatsClear(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Verify specified PFC interface index exists
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPfcIntfIndexGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential Priority mapping table interface index
*
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPfcIntfIndexGetNext(L7_uint32 intIfNum, L7_uint32 *pNext);

/*********************************************************************
* @purpose  Verify specified priority index exists
*
* @param    priority     @b{(input)}  Priority
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPfcPriorityIndexGet(L7_uint32 priority);

/*********************************************************************
* @purpose  Determine next sequential priority index
*
* @param    priority    @b{(input)} Priority
* @param    *pNext      @b{(output)} Ptr to next priority
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPfcPriorityIndexGetNext(L7_uint32 priority, L7_uint32 *pNext);
#endif
