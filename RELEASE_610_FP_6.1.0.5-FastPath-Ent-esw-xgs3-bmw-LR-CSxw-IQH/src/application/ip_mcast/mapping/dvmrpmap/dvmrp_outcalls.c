/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_outcalls.c
*
* @purpose Functions providing policy services to the DVMRP component.
*
* @component 
*
* @comments 
*
* @create 06/25/2003
*
* @author jeffr
* @end
*
**********************************************************************/
#include "l7_common_l3.h"
#include "l3_mcast_commdefs.h"
#include "l7_ip_api.h"                   
#include "l3_mcast_default_cnfgr.h"
#include "dvmrp_cnfgr.h" 
#include "dvmrp_config.h" 
#include "dvmrp_logging.h" 
/*********************************************************************
* @purpose  Determine if the interface type is valid in DVMRP
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL dvmrpIntfIsValidType(L7_uint32 sysIntfType)
{
  if ((sysIntfType != L7_PHYSICAL_INTF)  &&
      (sysIntfType != L7_LAG_INTF)       &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF))
  {
   DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Invalid interface in DVMRP\n", 
                  __FUNCTION__, __LINE__);    
    return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Determine if the interface is valid in DVMRP
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL dvmrpIntfIsValid(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
      "%s:%d Failed to return the internal interface type\n", 
                  __FUNCTION__, __LINE__);    
    return L7_FALSE;
  }
  return dvmrpIntfIsValidType(sysIntfType);
}

