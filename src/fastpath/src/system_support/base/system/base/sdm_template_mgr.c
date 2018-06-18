/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2010
*
**********************************************************************
*
* @filename sdm_template_mgr.c
*
* @purpose  Code to manage Switch Database Management (SDM) templates. 
*           An SDM template is a description of the maximum resources 
*           a switch or router can use for various features. Different 
*           SDM templates allow different combinations of scaling factors, 
*           enabling different allocations of resources depending on how 
*           the device is used.  
*
* @component N/A
*
* @comments 
*
* @created March 1, 2010
*
* @author rrice    
* @end
*
**********************************************************************/

#include <stdio.h>
#include "datatypes.h"
#include "sdm_exports.h"
#include "sdm_api.h"
#include "unitmgr_api.h"
#include "ms_api.h"
#include "simapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "defaultconfig.h"




/* Number of times to retry when sending the next template ID to 
 * a stack unit. */
#define SDM_MAX_TX_RETRIES 3

/* Number of milliseconds to wait for an ack from a stack unit when
 * propagating the next template ID. */
#define SDM_TX_ACK_TIMEOUT 500  

/* TLV to communicate the next active template ID. */
const L7_ushort16 SDM_TLV_TEMPLATE_ID = 1;

#define SDM_TRACE_LEN_MAX 512

typedef enum 
{
  SDM_TRACE_PERSISTENCE = 0x1

} sdmTraceFlags_t;

typedef struct sdmTemplate_s
{
  /* next element in template list */
  struct sdmTemplate_s *nextTemplate;

  /* Template identifier used in APIs. */
  sdmTemplateId_t templateId;

  L7_uchar8 templateName[SDM_TEMPLATE_NAME_LEN];

  /* Maximum number of entries in the IPv4 ARP cache. This does not include
   * ARP entries on the network or service ports. */
  L7_uint32 maxArpEntries;

  /* Maximum number of best routes in the IPv4 route table. */
  L7_uint32 maxIpv4UnicastRoutes;

  /* Maximum number of entries in the IPv6 neighbor cache. */
  L7_uint32 maxNdpEntries;

  /* Maximum number of best routes in the IPv6 route table. */
  L7_uint32 maxIpv6UnicastRoutes;

  /* Maximum number of next hops in an ECMP route. Applies to both 
   * IPv4 and IPv6. */
  L7_uint32 maxEcmpNextHops;

  /* Maximum number of IPv4 multicast routes */
  L7_uint32 maxIpv4McastRoutes;

  /* Maximum number of IPv6 multicast routes */
  L7_uint32 maxIpv6McastRoutes;

} sdmTemplate_t;

/* Fields are in network byte order */
typedef struct sdmMessage_s
{
  L7_ushort16 tlvType;
  L7_ushort16 tlvLen;
  L7_uint32   tlvVal;
} sdmMessage_t;


/* Template that is currently active and was used when system booted. */
static sdmTemplate_t *activeTemplate = NULL;

/* If user has changed the template since the system booted, then the 
 * template to become active after the next boot is identified here. This 
 * value is not guaranteed to reflect the value in persistent storage, since 
 * the user may change persistent storage without going through the 
 * SDM template manager. And we do not poll persistent storage to keep this
 * in sync with what's in persistent storage. */
static sdmTemplate_t *nextTemplate = NULL;

/* An singly linked list of SDM templates supported in this build. The list is 
 * populated before phase 1 init. Items are never removed from the list. The list
 * is expected to be short, less than a dozen or so entries. */
static sdmTemplate_t *templateList = NULL;

/* L7_TRUE when the next template has changed and needs to be saved persistently */
static L7_BOOL nextTemplateChanged = L7_FALSE;

static L7_uint32 sdmTraceFlags = 0;

static L7_RC_t sdmTemplateDefaultSet(void);
L7_RC_t sdmTemplateAdd(sdmTemplate_t *t);
static sdmTemplate_t *sdmTemplateFind(sdmTemplateId_t templateId);

#ifdef L7_STACKING_PACKAGE
static L7_RC_t sdmNextTemplatePropagate(sdmTemplateId_t templateId);
void sdmMsReceive(L7_uchar8 *buffer, L7_uint32 msgLen);
#endif


/*********************************************************************
* @purpose  Enable or disable SDM debug tracing.
*
* @param    traceFlags  @b{(input)}  Bit string corresponding to sdmTraceFlags_t
*
* @notes    
*
* @end
*********************************************************************/
void sdmTraceFlagsSet(L7_uint32 traceFlags)
{
  sdmTraceFlags = traceFlags;
}

/*********************************************************************
* @purpose  Write a debug trace message.
*
* @param    traceMsg @b{(input)} A string to be displayed as a trace message.
*
* @notes    The input string must be no longer than SDM_TRACE_LEN_MAX
*           characters. This function will prepend the message with the 
*           current time and will insert a new line character at the 
*           beginning of the message.
*
* @end
*********************************************************************/
void sdmTraceWrite(L7_uchar8 *traceMsg)
{
    L7_uint32 stackUptime;
    L7_uint32 msecs;
    L7_uint32 secs;
    L7_uchar8 debugMsg[512];

    if (traceMsg == NULL)
        return;

    stackUptime = simSystemUpTimeMsecGet();
    secs = stackUptime / 1000;
    msecs = stackUptime % 1000;

    /* For now, just print the message with a timestamp. */
    memset(debugMsg, 0, 512);
    osapiSnprintf(debugMsg, 512, "\n%u.%03u:  ", secs, msecs);
    strncat(debugMsg, traceMsg, SDM_TRACE_LEN_MAX);
    sysapiPrintf("%s", debugMsg);
}


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
L7_RC_t sdmTemplateMgrInit(void)
{
#ifdef L7_ROUTING_PACKAGE
  sdmTemplate_t *t;
#endif
  sdmTemplateId_t savedTemplateId = SDM_TEMPLATE_NONE;
  sdmTemplate_t *savedTemplate = NULL;

  /* statement here solely to prevent compiler warning about this symbol
   * not being used in a switching-only build. */
  nextTemplateChanged = L7_FALSE;

  /* Create the template list */

#ifdef L7_ROUTING_PACKAGE

  /* default IPv4-only routing template */
  t = (sdmTemplate_t*) osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(sdmTemplate_t));
  if (t == NULL)
  {
    return L7_NO_MEMORY;
  }
  t->templateId = SDM_TEMPLATE_V4_DEFAULT;
  osapiStrncpySafe(t->templateName, "IPv4-routing Default", SDM_TEMPLATE_NAME_LEN);
  t->maxArpEntries = L7_L3_ARP_CACHE_SIZE_V4ONLY;
  t->maxIpv4UnicastRoutes = L7_IPV4_ROUTE_TBL_SIZE_V4ONLY;
  t->maxNdpEntries = 0;
  t->maxIpv6UnicastRoutes = 0;
  t->maxEcmpNextHops = L7_RT_MAX_EQUAL_COST_ROUTES_LOW;
#ifdef L7_MCAST_PACKAGE
  t->maxIpv4McastRoutes = L7_MULTICAST_IPV4_ROUTES_V4ONLY;  
  t->maxIpv6McastRoutes = 0;
#else
  t->maxIpv4McastRoutes = 0;  
  t->maxIpv6McastRoutes = 0;
#endif
  sdmTemplateAdd(t);

  /* data center IPv4-only template. Only if platform supports 
   * larger number of next hops. */
  if (L7_RT_MAX_EQUAL_COST_ROUTES_HIGH > L7_RT_MAX_EQUAL_COST_ROUTES_LOW)
  {
    t = (sdmTemplate_t*) osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(sdmTemplate_t));
    if (t == NULL)
    {
      return L7_NO_MEMORY;
    }
    t->templateId = SDM_TEMPLATE_V4_DATA_CENTER;
    osapiStrncpySafe(t->templateName, "IPv4 Data Center", SDM_TEMPLATE_NAME_LEN);
    t->maxArpEntries = L7_L3_ARP_CACHE_SIZE_DUAL;
    t->maxIpv4UnicastRoutes = L7_IPV4_ROUTE_TBL_SIZE_DUAL;
    t->maxNdpEntries = 0;
    t->maxIpv6UnicastRoutes = 0;
    t->maxEcmpNextHops = L7_RT_MAX_EQUAL_COST_ROUTES_HIGH;
#ifdef L7_MCAST_PACKAGE
    t->maxIpv4McastRoutes = L7_MULTICAST_IPV4_ROUTES_V4ONLY;  
    t->maxIpv6McastRoutes = 0;
#else
  t->maxIpv4McastRoutes = 0;  
  t->maxIpv6McastRoutes = 0;
#endif
    sdmTemplateAdd(t);
  }

#ifdef L7_IPV6_PACKAGE

  /* data center IPv4-only template */
  t = (sdmTemplate_t*) osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(sdmTemplate_t));
  if (t == NULL)
  {
    return L7_NO_MEMORY;
  }
  t->templateId = SDM_TEMPLATE_DUAL_DEFAULT;
  osapiStrncpySafe(t->templateName, "Dual IPv4 and IPv6", SDM_TEMPLATE_NAME_LEN);
  t->maxArpEntries = L7_L3_ARP_CACHE_SIZE_DUAL;
  t->maxIpv4UnicastRoutes = L7_IPV4_ROUTE_TBL_SIZE_DUAL;
  t->maxNdpEntries = L7_IPV6_NDP_CACHE_SIZE;
  t->maxIpv6UnicastRoutes = L7_IPV6_ROUTE_TBL_SIZE_TOTAL;
  t->maxEcmpNextHops = L7_RT_MAX_EQUAL_COST_ROUTES_LOW;
#ifdef L7_MCAST_PACKAGE
  t->maxIpv4McastRoutes = L7_MULTICAST_IPV4_ROUTES_DUAL;  
  t->maxIpv6McastRoutes = L7_MULTICAST_IPV6_ROUTES;
#else
  t->maxIpv4McastRoutes = 0;  
  t->maxIpv6McastRoutes = 0;
#endif
  sdmTemplateAdd(t);

#endif   /* L7_IPV6_PACKAGE */
#endif   /* L7_ROUTING_PACKAGE */

  /* Next template only set if user configures after boot. */
  nextTemplate = NULL;    

  /* Read the next active template ID from persistent storage and make it 
   * the current template. */
  savedTemplateId = sdmSavedTemplateIdGet();
  if ((savedTemplateId == SDM_TEMPLATE_NONE) ||
      ((savedTemplate = sdmTemplateFind(savedTemplateId)) == NULL))
  {
    /* No saved template or saved template ID no longer valid. 
     * Set the default template as the active template. */
    sdmTemplateDefaultSet();
    /* Not sure log server can log at this point, but should fail 
     * gracefully if it cannot. */
    if (activeTemplate)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
              "Booting with default SDM template %s.", 
              sdmTemplateNameGet(activeTemplate->templateId));
    }
    /* Normal to boot with no template on switching only build. So 
     * no need to write an error log message here. */
  }
  else
  {
    activeTemplate = savedTemplate;
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "Booting with saved SDM template %s.", 
            sdmTemplateNameGet(activeTemplate->templateId));
  }

  return L7_SUCCESS;
}

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
sdmTemplateId_t sdmDefaultTemplateId(void)
{
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  return FD_SDM_DEFAULT_DUAL;
#else
  return FD_SDM_DEFAULT_V4ONLY;
#endif
#else
  return SDM_TEMPLATE_NONE;
#endif
}

/*********************************************************************
* @purpose  Set the active template to the default for this build 
*           combination.
*
* @param    void
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t sdmTemplateDefaultSet(void)
{
  sdmTemplateId_t defaultTemplateId = sdmDefaultTemplateId();
  sdmTemplate_t *defaultTemplate = sdmTemplateFind(defaultTemplateId);
  activeTemplate = defaultTemplate;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a template to the template list.
*
* @param    t  @b{(input)}  an SDM template 
*
* @returns  L7_SUCCESS 
*
* @notes    Add to the head of the list.
*           Logically a static function, but no templates added in 
*           switching only build; so compiler complains if declared static.
*
* @end
*********************************************************************/
L7_RC_t sdmTemplateAdd(sdmTemplate_t *t)
{
  t->nextTemplate = templateList;
  templateList = t;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Find the template wtih the given template ID.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  pointer to the template, if found
*
* @notes    
*
* @end
*********************************************************************/
static sdmTemplate_t *sdmTemplateFind(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t = templateList;
  while (t)
  {
    if (t->templateId == templateId)
    {
      return t;
    }
    t = t->nextTemplate;
  }
  return NULL;
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
sdmTemplateId_t sdmActiveTemplateGet(void)
{
  if (activeTemplate)
    return activeTemplate->templateId;
  else
    return SDM_TEMPLATE_NONE;
}

/*********************************************************************
* @purpose  Get the template ID of the next active SDM template.
*
* @param    void
*
* @returns  template ID
*
* @notes    Need to read this from persistent storage, since persistent
*           value may have been cleared out from under us, for example 
*           if the user manually deletes the file. If no template ID is
*           saved persistently, then this returns the template ID of the
*           default template.
*
* @end
*********************************************************************/
sdmTemplateId_t sdmNextActiveTemplateGet(void)
{
  sdmTemplateId_t templateId = sdmSavedTemplateIdGet();
  if (templateId == SDM_TEMPLATE_NONE)
  {
    templateId = sdmDefaultTemplateId();
  }
  return templateId;
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
* @notes    This API is only called on the management unit, if stacking, or
*           on a standalone unit.
*           Immediately saves the value persistently on a local unit. If
*           stacking, push the template ID to other members of the stack,
*           so that they can also store persistently.
*
*           Intentionally do not check if next template ID is same as
*           existing next template ID. May need to reset next template ID
*           on stack manager to propagate the value to a stack unit in 
*           SDM Mismatch mode.
*
*           If templateId is SDM_TEMPLATE_NONE, deletes the next active 
*           template ID from persistent storage.
*
* @end
*********************************************************************/
L7_RC_t sdmNextActiveTemplateSet(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t = sdmTemplateFind(templateId);

  if ((templateId != SDM_TEMPLATE_NONE) && (t == NULL))
  {
    return L7_NOT_EXIST;
  }
  nextTemplate = t;

  if (templateId == SDM_TEMPLATE_NONE)
  {
    if (bspapiNextSdmTemplateClear() == L7_SUCCESS)
    {
      if (sdmTraceFlags & SDM_TRACE_PERSISTENCE) 
      {
        sdmTraceWrite("Cleared next template ID in persistent storage.");
      }
    }
    else if (SDM_TEMPLATE_NONE == sdmSavedTemplateIdGet())
    {
      return L7_SUCCESS;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LOG_COMPONENT_DEFAULT,
              "Failed to clear the next SDM template ID in persistent storage.");
      return L7_FAILURE;
    }
  }
  /* Save persistently on local unit */
  else if (bspapiNextSdmTemplateSave((L7_uint32) templateId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LOG_COMPONENT_DEFAULT,
              "Failed to save the next SDM template ID in persistent storage.");
    return L7_FAILURE;
  }

#ifdef L7_STACKING_PACKAGE
  /* Push the change to other units in the stack */
  sdmNextTemplatePropagate(templateId);
#endif

  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
          "Set the next active SDM template to %s.",
          sdmTemplateNameGet(templateId));

  return L7_SUCCESS;
}

#ifdef L7_STACKING_PACKAGE
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
                                     sdmTemplateId_t templateId)
{
  L7_BOOL msgSent = L7_FALSE;
  L7_uint32 numTries = 0;
  sdmMessage_t msg;

  msg.tlvType = osapiHtons(SDM_TLV_TEMPLATE_ID);
  msg.tlvLen = osapiHtons(sizeof(sdmMessage_t));
  msg.tlvVal = osapiHtonl((L7_uint32) templateId);

  while ((msgSent == L7_FALSE) && (numTries <= SDM_MAX_TX_RETRIES))
  {
    if (msMessageSend(L7_MS_SDM_REGISTRAR_ID, stackUnitId, sizeof(sdmMessage_t),
                      (L7_uchar8*) &msg, MS_RELIABLE, SDM_TX_ACK_TIMEOUT) == L7_SUCCESS)
    {
      if (sdmTraceFlags & SDM_TRACE_PERSISTENCE) 
      {
          L7_uchar8 traceBuf[SDM_TRACE_LEN_MAX];
          osapiSnprintf(traceBuf, SDM_TRACE_LEN_MAX, 
                        "Sent next template ID %u to stack unit %u.", 
                        templateId, stackUnitId);
          sdmTraceWrite(traceBuf);
      }
      msgSent = L7_TRUE;
    }
    numTries++;
  } 
  if (!msgSent)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "SDM template manager failed to propagate the next template ID to unit %d.",
            stackUnitId);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Push a next template change to other units in a stack.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t sdmNextTemplatePropagate(sdmTemplateId_t templateId)
{
  L7_uint32 myUnitId; 
  L7_uint32 stackUnitId;
  L7_RC_t rc;

  /* Get my unit ID */
  if (unitMgrNumberGet(&myUnitId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LOG_COMPONENT_DEFAULT,
              "Failed to send the next SDM template ID to other stack members. "
              "Failed to get unit number for local (management) unit.");
    return L7_FAILURE;
  }

  /* Iterate through units in stack, sending next template ID to each. */
  rc = unitMgrStackMemberGetFirst(&stackUnitId);
  while (rc == L7_SUCCESS)
  {
    if (stackUnitId != myUnitId)
    {
      (void) sdmNextTemplateToStackMember(stackUnitId, templateId);
    }
    rc = unitMgrStackMemberGetNext(stackUnitId, &stackUnitId);
  }
  return L7_SUCCESS;
}

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
*           not a component, and this piggyback's on unit mgr p2 init.
*
* @end
*********************************************************************/
void sdmMessageServiceRegister(void)
{
  if (msReceiveCallbackRegister(sdmMsReceive, L7_MS_SDM_REGISTRAR_ID) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LOG_COMPONENT_DEFAULT,
            "SDM template manager failed to register with stacking message service.");
  }
}

/*********************************************************************
* @purpose  Receive callback registered with the message service.
*
* @param    buffer   @b{(input)}  start of message
* @param    msgLen   @b{(input)}  length of message (bytes)
*
* @returns  void
*
* @notes    Parse the message to extract the next template ID. Note that
*           the next template has changed and needs to be stored.
*
* @end
*********************************************************************/
void sdmMsReceive(L7_uchar8 *buffer, L7_uint32 msgLen)
{
  sdmMessage_t *msg = (sdmMessage_t*) buffer;
  sdmTemplate_t *t = NULL;
  sdmTemplateId_t templateId;
  L7_ushort16 tlvType;
  L7_uint32 mgmtUnitId = 0;

  if (unitMgrMgrNumberGet(&mgmtUnitId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LOG_COMPONENT_DEFAULT,
              "SDM template manager failed to get unit number for management unit.");
    return;
  }

  if (msg)
  {
    tlvType = osapiNtohs(msg->tlvType);
    if (tlvType == SDM_TLV_TEMPLATE_ID)
    {
      templateId = (sdmTemplateId_t) osapiNtohl(msg->tlvVal);
      if (sdmTraceFlags & SDM_TRACE_PERSISTENCE) 
      {
        L7_uchar8 traceBuf[SDM_TRACE_LEN_MAX];
        osapiSnprintf(traceBuf, SDM_TRACE_LEN_MAX, 
                      "Received next template ID %u from stack manager.", 
                      templateId);
        sdmTraceWrite(traceBuf);
      }
      if (templateId == SDM_TEMPLATE_NONE)
      {
        nextTemplate = NULL;
        nextTemplateChanged = L7_TRUE;
      }
      else 
      {
        t = sdmTemplateFind(templateId);
        if (t)
        {
          nextTemplate = t;
          nextTemplateChanged = L7_TRUE;
        }
      }
    }
    else
    {
      if (sdmTraceFlags & SDM_TRACE_PERSISTENCE) 
      {
        L7_uchar8 traceBuf[SDM_TRACE_LEN_MAX];
        osapiSnprintf(traceBuf, SDM_TRACE_LEN_MAX, 
                      "Received message from message service with unknown TLV type %u.", 
                      tlvType);
        sdmTraceWrite(traceBuf);
      }
    }
    msMessageAck(L7_MS_SDM_REGISTRAR_ID, mgmtUnitId, 0, L7_NULLPTR, MS_RELIABLE);
  }
}

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
L7_RC_t sdmNextTemplateSave(void)
{
  if (!nextTemplateChanged)
  {
    return L7_SUCCESS;
  }
  /* Will only try to save the change once. If attempt fails, then 
   * let automatic resolution of SDM mismatch take care of it later, 
   * rather than retrying. Retrying risks failing every few seconds
   * forever and printing a log msg for each failure. */
  nextTemplateChanged = L7_FALSE;
  if (nextTemplate)
  {
    if (bspapiNextSdmTemplateSave((L7_uint32) nextTemplate->templateId) == L7_SUCCESS)
    {
      if (sdmTraceFlags & SDM_TRACE_PERSISTENCE) 
      {
        L7_uchar8 traceBuf[SDM_TRACE_LEN_MAX];
        osapiSnprintf(traceBuf, SDM_TRACE_LEN_MAX, 
                      "Wrote next template ID %u to persistent storage.", 
                      nextTemplate->templateId);
        sdmTraceWrite(traceBuf);
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LOG_COMPONENT_DEFAULT,
              "Failed to save next SDM template ID persistently.");
      return L7_FAILURE;
    }
  }
  else
  {
    if (sdmSavedTemplateIdGet() != SDM_TEMPLATE_NONE)
    {
      if (bspapiNextSdmTemplateClear() == L7_SUCCESS)
      {
        if (sdmTraceFlags & SDM_TRACE_PERSISTENCE) 
        {
          L7_uchar8 traceBuf[SDM_TRACE_LEN_MAX];
          osapiSnprintf(traceBuf, SDM_TRACE_LEN_MAX, 
                        "Cleared next template ID in persistent storage.");
          sdmTraceWrite(traceBuf);
        }
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LOG_COMPONENT_DEFAULT,
                "Failed to clear the next SDM template ID persistently.");
        return L7_FAILURE;
      }
    }
  }
  return L7_SUCCESS;
}

#endif   /* L7_STACKING_PACKAGE */



/*********************************************************************
* @purpose  Retrieve the SDM template ID from persistent storage.
*
* @param    void
*
* @returns  template ID
*
* @notes    
*
* @end
*********************************************************************/
sdmTemplateId_t sdmSavedTemplateIdGet(void)
{
  L7_uint32 templateId;

  if (bspapiNextSdmTemplateGet(&templateId) == L7_SUCCESS)
  {
    return (sdmTemplateId_t) templateId;
  }
  return SDM_TEMPLATE_NONE;
}

/*********************************************************************
* @purpose  Debug routine to show the name of the saved template ID.
*
* @param    void
*
* @returns  template ID
*
* @notes    Called periodically on non-mgmt stack units.
*
* @end
*********************************************************************/
void sdmSavedTemplateShow(void)
{
  sdmTemplateId_t savedTemplateId = sdmSavedTemplateIdGet();
  
  printf("\nSaved SDM template is the %s template.",
         sdmTemplateNameGet(savedTemplateId));
}

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
L7_uchar8 *sdmTemplateNameGet(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t;
  static L7_uchar8 *noneStr = "None";
  static L7_uchar8 *invalidStr = "Invalid";

  if (templateId == SDM_TEMPLATE_NONE)
    return noneStr;

  t = sdmTemplateFind(templateId);
  if (t)
    return t->templateName;

  return invalidStr;
}

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
L7_uint32 sdmMaxArpEntriesGet(void)
{
  if (activeTemplate)
    return sdmTemplateMaxArpEntriesGet(activeTemplate->templateId);
  else
    return 0;
}

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
L7_uint32 sdmMaxIpv4RoutesGet(void)
{
  if (activeTemplate)
    return sdmTemplateMaxIpv4RoutesGet(activeTemplate->templateId);
  else
    return 0;
}

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
L7_uint32 sdmMaxNdpEntriesGet(void)
{
  if (activeTemplate)
    return sdmTemplateMaxNdpEntriesGet(activeTemplate->templateId);
  else
    return 0;
}

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
L7_uint32 sdmMaxIpv6RoutesGet(void)
{
  if (activeTemplate)
    return sdmTemplateMaxIpv6RoutesGet(activeTemplate->templateId);
  else
    return 0;
}

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
L7_uint32 sdmMaxEcmpNextHopsGet(void)
{
  if (activeTemplate)
    return sdmTemplateMaxEcmpNextHopsGet(activeTemplate->templateId);
  else
    return 0;
}

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
L7_uint32 sdmMaxIpv4McastRoutesGet(void)
{
  if (activeTemplate)
    return sdmTemplateMaxIpv4McastRoutesGet(activeTemplate->templateId);
  else
    return 0;
}

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
L7_uint32 sdmMaxIpv6McastRoutesGet(void)
{
  if (activeTemplate)
    return sdmTemplateMaxIpv6McastRoutesGet(activeTemplate->templateId);
  else
    return 0;
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
L7_uint32 sdmTemplateMaxArpEntriesGet(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t = sdmTemplateFind(templateId);
  if (!t)
  {
    return 0;
  }
  return t->maxArpEntries;
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
L7_uint32 sdmTemplateMaxIpv4RoutesGet(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t = sdmTemplateFind(templateId);
  if (!t)
  {
    return 0;
  }
  return t->maxIpv4UnicastRoutes;
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
L7_uint32 sdmTemplateMaxNdpEntriesGet(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t = sdmTemplateFind(templateId);
  if (!t)
  {
    return 0;
  }
  return t->maxNdpEntries;
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
L7_uint32 sdmTemplateMaxIpv6RoutesGet(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t = sdmTemplateFind(templateId);
  if (!t)
  {
    return 0;
  }
  return t->maxIpv6UnicastRoutes;
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
L7_uint32 sdmTemplateMaxEcmpNextHopsGet(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t = sdmTemplateFind(templateId);
  if (!t)
  {
    return 0;
  }
  return t->maxEcmpNextHops;
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
L7_uint32 sdmTemplateMaxIpv4McastRoutesGet(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t = sdmTemplateFind(templateId);
  if (!t)
  {
    return 0;
  }
  return t->maxIpv4McastRoutes;
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
L7_uint32 sdmTemplateMaxIpv6McastRoutesGet(sdmTemplateId_t templateId)
{
  sdmTemplate_t *t = sdmTemplateFind(templateId);
  if (!t)
  {
    return 0;
  }
  return t->maxIpv6McastRoutes;
}

/*********************************************************************
* @purpose  Determine whether active SDM template supports IPv6 routing.
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
L7_BOOL sdmTemplateSupportsIpv6(void)
{
  if (activeTemplate && (activeTemplate->templateId == SDM_TEMPLATE_DUAL_DEFAULT))
    return L7_TRUE;
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine whether a given template is supported in this build.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  L7_TRUE if template is supported
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL sdmTemplateSupported(sdmTemplateId_t templateId)
{
  if (sdmTemplateFind(templateId) != NULL)
    return L7_TRUE;
  return L7_FALSE;
}

