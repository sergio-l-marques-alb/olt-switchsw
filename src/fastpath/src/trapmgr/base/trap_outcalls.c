/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   trap_outcalls.c  
*
* @purpose    Trap manager outcall functions.
*
* @component  Trap manager
*
* @comments   none
*
* @create     1/26/2004
*
* @author     rrice
* @end
*
**********************************************************************/

#include "l7_common.h"

#ifdef L7_ROUTING_PACKAGE
    #include "l7_ospf_api.h"
#endif


/*********************************************************************
* @purpose  If it exists, tell the OSPF component which of the 
*           OSPF trap flags are set.
*
* @param    ospfTrapFlags {(input)} bitmap indicating the OSPF trap flags
*           that are set.
*
* @returns  L7_SUCCESS if trap flags are successfully communicated to OSPF
* @returns  L7_NOT_EXIST if the routing package is not present 
*
* @comments This function is needed during initialization. When the 
*           trap manager reads its configuration, it has to tell OSPF
*           if its trap flags are enabled. The call to OSPF must be
*           conditioned on whether routing is included in the build.
*           
* @comments When trapflags are configured at run time, the cli/web/snmp
*           checks whether routing is in the build, and if so calls
*           functions in the trapmgr/routing. 
*       
* @end
*********************************************************************/
L7_RC_t trapMgrOspfTrapFlagsApply(L7_uint32 ospfTrapFlags)
{
    L7_RC_t rc = L7_NOT_EXIST;

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_OSPF_PACKAGE
    rc = ospfMapTrapModeSet(ospfTrapFlags);
#endif
#endif

    return rc;
}

