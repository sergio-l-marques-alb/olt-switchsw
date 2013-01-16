/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sdm_api.h
*
* @purpose API interface for the switch database management template 
*          manager.
*
* @component
*
* @create March 8, 2010
*
* @author rrice
* @end
*
*
*********************************************************************/


#ifndef SDM_API_H
#define SDM_API_H


#include "datatypes.h"
#include "l7_common.h"
#include "sdm_exports.h"

/*********************************************************************
* @purpose  Initialize the SDM template manager.
*
* @param    void
*
* @returns  L7_SUCCESS 
*           L7_NO_MEMORY    if templates could not be allocated
*
* @notes    This is done before p1 init.
*
* @end
*********************************************************************/
L7_RC_t sdmTemplateMgrInit(void);

/*********************************************************************
* @purpose  Get the default template ID for this build.
*
* @param    void
*
* @returns  default template ID
*
* @notes    
*
* @end
*********************************************************************/
sdmTemplateId_t sdmDefaultTemplateId(void);

/*********************************************************************
* @purpose  Get the template ID of the active SDM template.
*
* @param    void
*
* @returns  template ID
*
* @notes    
*
* @end
*********************************************************************/
sdmTemplateId_t sdmActiveTemplateGet(void);

/*********************************************************************
* @purpose  Retrieve the SDM template ID from persistent storage.
*
* @param    void
*
* @returns  template ID. If no template is saved, returns SDM_TEMPLATE_NONE.
*
* @notes    
*
* @end
*********************************************************************/
sdmTemplateId_t sdmSavedTemplateIdGet(void);

/*********************************************************************
* @purpose  Get the template ID that will be active if the system reboots.
*
* @param    void
*
* @returns  template ID
*
* @notes    If a template ID has been saved persistently, returns that value.
*           Otherwise, returns the template ID of the default template for 
*           the active image. 
*
* @end
*********************************************************************/
sdmTemplateId_t sdmNextActiveTemplateGet(void);

/*********************************************************************
* @purpose  Set the template ID of the next active SDM template.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  L7_SUCCESS
*           L7_FAILURE if template ID not stored persistently
*           L7_NOT_EXIST if the template ID does not match a valid template.
*
* @notes    This API is only called on the management unit. 
*           Immediately saves the value persistently on a local unit. If
*           stacking, push the template ID to other members of the stack,
*           so that they can also store persistently.
*
*           Intentionally do not check if next template ID is same as
*           existing next template ID. May need to reset next template ID
*           on stack manager to propagate the value to a stack unit.
*
*           If templateId is SDM_TEMPLATE_NONE, deletes the next active 
*           template ID from persistent storage.
*
* @end
*********************************************************************/
L7_RC_t sdmNextActiveTemplateSet(sdmTemplateId_t templateId);

/*********************************************************************
* @purpose  Register with message service so that stack units can 
*           receive messages from the management unit notifying them
*           of a change to the next active template. 
*
* @param    void
*
* @returns  void
*
* @notes    Called in p2 init. This is an API because SDM template manager is
*           not a component, and this is called from unit mgr.
*
* @end
*********************************************************************/
void sdmMessageServiceRegister(void);

/*********************************************************************
* @purpose  Push a next template change to a specific unit in a stack.
*
* @param    stackUnitId   @b{(input)}  unit ID of remote stack unit
* @param    templateId    @b{(input)}  template ID
*
* @returns  L7_SUCCESS if template ID successfully sent
*           L7_FAILURE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t sdmNextTemplateToStackMember(L7_uint32 stackUnitId,
                                     sdmTemplateId_t templateId);

/*********************************************************************
* @purpose  If a configuration change has occurred, save the next 
*           template ID persistently.
*
* @param    void
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    Called periodically on non-mgmt stack units.
*
* @end
*********************************************************************/
L7_RC_t sdmNextTemplateSave(void);

/*********************************************************************
* @purpose  Given a template ID, get the corresponding template name
*           that should be displayed in user-visible places.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  L7_SUCCESS
*
* @notes    The template name is no more than SDM_TEMPLATE_NAME_LEN,
*           including the NULL terminator.
*
* @end
*********************************************************************/
L7_uchar8 *sdmTemplateNameGet(sdmTemplateId_t templateId);

/*********************************************************************
* @purpose  Get the maximum number of ARP entries supported in the currently
*           active SDM template.
*
* @param    void
*
* @returns  max ARP entries
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmMaxArpEntriesGet(void);

/*********************************************************************
* @purpose  Get the maximum number of unicast IPv4 routes supported 
*           in the currently active SDM template.
*
* @param    void
*
* @returns  max IPv4 routes
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmMaxIpv4RoutesGet(void);

/*********************************************************************
* @purpose  Get the maximum number of NDP entries supported in the currently
*           active SDM template.
*
* @param    void
*
* @returns  max NDP entries
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmMaxNdpEntriesGet(void);

/*********************************************************************
* @purpose  Get the maximum number of IPv6 unicast routes supported 
*           in the currently active SDM template.
*
* @param    void
*
* @returns  max IPv6 routes
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmMaxIpv6RoutesGet(void);

/*********************************************************************
* @purpose  Get the maximum number of ECMP next hops supported 
*           in the currently active SDM template.
*
* @param    void
*
* @returns  max ECMP next hops
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmMaxEcmpNextHopsGet(void);

/*********************************************************************
* @purpose  Get the maximum number of IPv4 multicast routes supported 
*           in the currently active SDM template.
*
* @param    void
*
* @returns  max IPv4 multicast routes
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmMaxIpv4McastRoutesGet(void);

/*********************************************************************
* @purpose  Get the maximum number of IPv6 multicast routes supported 
*           in the currently active SDM template.
*
* @param    void
*
* @returns  max IPv6 multicast routes
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmMaxIpv6McastRoutesGet(void);

/*********************************************************************
* @purpose  Get the maximum number of ARP entries supported in a specific
*           SDM template.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  max ARP entries for this template
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmTemplateMaxArpEntriesGet(sdmTemplateId_t templateId);

/*********************************************************************
* @purpose  Get the maximum number of IPv4 routes supported in a specific
*           SDM template.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  max IPv4 routes for this template
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmTemplateMaxIpv4RoutesGet(sdmTemplateId_t templateId);

/*********************************************************************
* @purpose  Get the maximum number of NDP entries supported in a specific
*           SDM template.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  max NDP entries for this template
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmTemplateMaxNdpEntriesGet(sdmTemplateId_t templateId);

/*********************************************************************
* @purpose  Get the maximum number of IPv6 routes supported in a specific
*           SDM template.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  max IPv6 routes for this template
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmTemplateMaxIpv6RoutesGet(sdmTemplateId_t templateId);

/*********************************************************************
* @purpose  Get the maximum number of ECMP next hops supported in a specific
*           SDM template.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  max ECMP next hops for this template
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmTemplateMaxEcmpNextHopsGet(sdmTemplateId_t templateId);

/*********************************************************************
* @purpose  Get the maximum number of IPv4 multicast routes supported 
*           in a specific SDM template.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  max IPv4 multicast routes for this template
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmTemplateMaxIpv4McastRoutesGet(sdmTemplateId_t templateId);

/*********************************************************************
* @purpose  Get the maximum number of IPv6 multicast routes supported 
*           in a specific SDM template.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  max IPv6 multicast routes for this template
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 sdmTemplateMaxIpv6McastRoutesGet(sdmTemplateId_t templateId);
  

/*********************************************************************
* @purpose  Determine whether the active SDM template supports IPv6 routing.
*
* @param    void
*
* @returns  L7_TRUE if IPv6 routing is supported
*           L7_FALSE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL sdmTemplateSupportsIpv6(void);

/*********************************************************************
* @purpose  Determine whether a given template is supported in this build.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  L7_TRUE if template is supported
*           L7_FALSE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL sdmTemplateSupported(sdmTemplateId_t templateId);


#endif
