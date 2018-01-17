/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2001-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename  usmdb_asf.c
*
* @purpose   Provide interface to Cut-Though (ASF) module
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

#include "l7_common.h"

#include <string.h>
#include "simapi.h"
#include "osapi.h"
#include "nimapi.h"
#include "defaultconfig.h"
#include "asf_api.h"
#include "usmdb_asf_api.h"

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
L7_RC_t usmDbAsfModeSet(L7_uint32 UnitIndex, L7_uint32 mode)
{
  return asfModeSet(mode); 
}

/*********************************************************************
* @purpose  Get (Enable/Disable) the ASF mode on a unit.
*          
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    currMode    @b{(output)} pointer to current mode variable
* @param    configMode  @b{(output)} pointer to configured mode variable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t usmDbAsfModeGet(L7_uint32 UnitIndex,
                        L7_uint32 *currMode,
                        L7_uint32 *configMode)
{
  return asfModeGet(currMode, configMode);
}
