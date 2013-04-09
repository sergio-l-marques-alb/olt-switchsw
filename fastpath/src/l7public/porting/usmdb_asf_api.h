/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  usmdb_asf_api.h
*
* @purpose   externs for Cut-Though (ASF) USMDB module
* @component asf
*
* @comments
*
* @create    17-Jan-2007
*
* @author    NarasimhaRaju
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef _USMDB_ASF_API_H_
#define _USMDB_ASF_API_H_

/*********************************************************************
* @purpose  Set (Enable/Disable) the ASF mode on a unit.
*          
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    mode        @b{(input)} mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAsfModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
* @purpose  Get (Enable/Disable) the ASF mode on a unit.
*          
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    currMode    @b{(output)} pointer to mode variable
* @param    configMode  @b{(output)} pointer to mode variable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAsfModeGet(L7_uint32 UnitIndex, L7_uint32 *currMode, L7_uint32 *configMode);

#endif /* _USMDB_ASF_API_H_ */
