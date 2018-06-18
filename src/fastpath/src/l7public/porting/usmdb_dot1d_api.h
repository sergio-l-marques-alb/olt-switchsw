/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\public\usmdb_dot1d_api.h
*
* @purpose externs for USMDB layer
*
* @component unitmgr
*
* @comments LVL7 extensions to MIB BRIDGE support
*
* @create 01/10/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_DOT1D_API_H
#define USMDB_DOT1D_API_H


/*********************************************************************
* @purpose  Check if an interface is valid for dot1d
*
* @param    UnitIndex      unit
* @param    intIfNum       internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*                                 
* @end
*********************************************************************/
extern L7_BOOL usmDbDot1dIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);

#endif
