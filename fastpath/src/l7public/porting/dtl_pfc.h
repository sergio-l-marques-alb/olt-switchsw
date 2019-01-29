#ifndef DTL_PFC_H
#define DTL_PFC_H

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2009
*
**********************************************************************
*
* @filename dtl_pfc.c
*
* @purpose Priority Flow Control definitions
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
#include "nimapi.h"
#include "datatypes.h"
#include "dapi.h"

#define  PFC_DISABLE ((L7_uchar8)0)
#define  PFC_ENABLE  ((L7_uchar8)1)

/*********************************************************************
* @purpose  Set the PFC participation mode for an interface
*
* @param  intIfNum @b((input)) The internal interface
* @param  enable   @b((input)) The particpation mode 
* @param  nodrop_pri_bmp @b{(input)}  priority bitmap to set a nodrop
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcIntfConfig( L7_uint32 intIfNum, L7_BOOL enable, L7_uchar8 nodrop_pri_bmp);

/*********************************************************************
* @purpose  Get the count of pfc receives for an interface/priority
*
* @param  intIfNum  @b((input)) The internal interface
* @param  priority  @b((input)) The particpation mode 
* @param  stat      @b{(output)}  stat
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcRxPriorityStatGet(L7_uint32 intIfNum, 
                                L7_uint32 priority, 
                                L7_uint32 *stat);

/*********************************************************************
* @purpose  Get the count of pfc receives for an interface
*
* @param  intIfNum  @b((input)) The internal interface
* @param  stat      @b{(output)} stat
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcRxStatGet(L7_uint32 intIfNum, L7_uint32 *stat);

/*********************************************************************
* @purpose  Get the count of pfc transmits for an interface
*
* @param  intIfNum  @b((input)) The internal interface
* @param  stat      @b{(output)} stat
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcTxStatGet(L7_uint32 intIfNum, L7_uint32 *stat);

/*********************************************************************
* @purpose Clear all PFC stats on an interface 
*
* @param  intIfNum  @b((input))  The internal interface
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcStatsClear(L7_uint32 intIfNum);  

#endif
