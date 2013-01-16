/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2010
*
**********************************************************************
*
* @filename usmdb_sdm.c
*
* @purpose Provide interface to switch database management
*
* @component none
*
* @comments 
*
* @create March 5, 2010
*
* @author rrice
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "sdm_exports.h"
#include "sdm_api.h"

#define MAX_SDM_TEMPLATES 3
/*********************************************************************
*
* @purpose Determine if a given SDM template is supported in this build.
*          
* @param   templateId  @b{(input)}    template ID
* 
* @returns L7_TRUE or L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbSdmTemplateSupported(sdmTemplateId_t templateId)
{
  return sdmTemplateSupported(templateId);
}

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
sdmTemplateId_t usmDbSdmActiveTemplateGet(void)
{
  return sdmActiveTemplateGet();
}

/*********************************************************************
* @purpose  Get the template ID of the next active SDM template.
*
* @param    void
*
* @returns  template ID
*
* @notes    
*
* @end
*********************************************************************/
sdmTemplateId_t usmDbSdmNextActiveTemplateGet(void)
{
  return sdmNextActiveTemplateGet();
}

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
*           If templateId is SDM_TEMPLATE_NONE, deletes the next active 
*           template ID from persistent storage.
*
* @end
*********************************************************************/
L7_RC_t usmDbSdmNextActiveTemplateSet(sdmTemplateId_t templateId)
{
  return sdmNextActiveTemplateSet(templateId);
}

/*********************************************************************
* @purpose  Given a template ID, get the corresponding template name
*           that should be displayed in user-visible places.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  template name
*           If template ID doesn't match a template in this build, 
*           returns "Invalid."
*
* @notes    The template name is no more than SDM_TEMPLATE_NAME_LEN,
*           including the NULL terminator.
*
* @end
*********************************************************************/
L7_uchar8 *usmDbSdmTemplateNameGet(sdmTemplateId_t templateId)
{
  return sdmTemplateNameGet(templateId);
}

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
L7_uint32 usmDbSdmTemplateMaxArpEntriesGet(sdmTemplateId_t templateId)
{
  return sdmTemplateMaxArpEntriesGet(templateId);
}

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
L7_uint32 usmDbSdmTemplateMaxIpv4RoutesGet(sdmTemplateId_t templateId)
{
  return sdmTemplateMaxIpv4RoutesGet(templateId);
}

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
L7_uint32 usmDbSdmTemplateMaxNdpEntriesGet(sdmTemplateId_t templateId)
{
  return sdmTemplateMaxNdpEntriesGet(templateId);
}

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
L7_uint32 usmDbSdmTemplateMaxIpv6RoutesGet(sdmTemplateId_t templateId)
{
  return sdmTemplateMaxIpv6RoutesGet(templateId);
}

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
L7_uint32 usmDbSdmTemplateMaxEcmpNextHopsGet(sdmTemplateId_t templateId)
{
  return sdmTemplateMaxEcmpNextHopsGet(templateId);
}

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
L7_uint32 usmDbSdmTemplateMaxIpv4McastRoutesGet(sdmTemplateId_t templateId)
{
  return sdmTemplateMaxIpv4McastRoutesGet(templateId);
}

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
L7_uint32 usmDbSdmTemplateMaxIpv6McastRoutesGet(sdmTemplateId_t templateId)
{
  return sdmTemplateMaxIpv6McastRoutesGet(templateId);
}

/*********************************************************************
* @purpose  Get the next supported SDM template.
*
* @param    *templateId @b{(inout)}  pointer to next template ID
*
* @returns  L7_SUCCESS if next template ID exists.
*           L7_FAILURE otherwise.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbSdmTemplateIdNextGet(L7_uint32 *templateId)
{
  L7_uint32 i;
  for (i = *templateId + 1 ; i <= MAX_SDM_TEMPLATES; i++)
  {
    if(usmDbSdmTemplateSupported(i) == L7_TRUE)
    {
      *templateId = i;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

