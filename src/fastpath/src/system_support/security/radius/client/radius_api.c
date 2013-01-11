/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_api.c
*
* @purpose RADIUS Client API's
*
* @component radius
*
* @comments
*
* @create 03/26/2003
*
* @author pmurthy
*
* @end
*
**********************************************************************/
#include "radius_include.h"

/* Mutex for shared data. */
extern void * radiusTaskSyncSema;
extern void * radiusServerDbSyncSema;
extern L7_uint32 radius_sm_bp_id;
/*********************************************************************
*
* @purpose Register a routine to be called when a RADIUS response is
*          received from a server for a previously submitted request.
*
* @param registrar_ID @b((input)) routine registrar id (See L7_COMPONENT_ID_t)
* @param *notify @b((input)) pointer to a routine to be invoked upon a respones.
*                            Each routine has the following parameters:
*                            (status, correlator, *attributes, attributesLen)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusResponseRegister(L7_COMPONENT_IDS_t registrar_ID,
                               L7_RC_t (*notify)(L7_uint32 status,
                                                   L7_uint32 correlator,
                                                   L7_uchar8 *attributes,
                                                   L7_uint32 attributesLen))
{
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc = cnfgrApiComponentNameGet(registrar_ID, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: Registrar ID %u greater then last component ID\n", registrar_ID);
    return L7_FAILURE;
  }

  if ((L7_uint32)radiusNotifyList[registrar_ID].notify_radius_resp != L7_NULL)
  {
    /* Already registered but allow it after the log */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: Registrar ID %u, %s already registered\n", registrar_ID, name);
  }

  radiusNotifyList[registrar_ID].registrar_ID = registrar_ID;
  radiusNotifyList[registrar_ID].notify_radius_resp = notify;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Deregister all routines to be called when a RADIUS response is
*          received from a server for a previously submitted request.
*
* @param   componentId  @b{(input)}  one of L7_COMPONENT_IDS_t
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusResponseDeregister( L7_COMPONENT_IDS_t componentId)
{
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc = cnfgrApiComponentNameGet(componentId, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if (componentId >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "radiusResponseDeregister registrar_ID %d greater than L7_LAST_COMPONENT_ID\n", componentId);
    return(L7_FAILURE);
  }

  if (radiusNotifyList[componentId].registrar_ID == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "radiusResponseDeregister registrar_ID %d, %s not registered\n", componentId, name);
    return(L7_FAILURE);
  }

  memset(&radiusNotifyList[componentId], L7_NULL, sizeof(radiusNotifyList_t));

  return L7_SUCCESS;
}

/*************************************************************************
*
* @purpose Initialize the attribute value-pair list
*
* @param vpList @b{(input)} list that needs to be initialized
*
* @returns void
*
* @comments
*
* @end
*
*************************************************************************/
void radiusAttrValuePairListInit(radiusValuePair_t **vpList)
{
  *vpList = L7_NULL;
}

/*************************************************************************
*
* @purpose Assign a given value to an attribute-value pair
*
* @param vp @b{(output)} pointer to ValuePair attribute
* @param value @b{(input)} the value that needs to be added to the ValuePair
*                          structure
* @param length @b{(input)} length of the attribute
*
* @returns void
*
* @comments
*
* @end
*
*************************************************************************/
void radiusAttrValueAssign(radiusValuePair_t *vp,
                           void *value,
                           L7_uint32 length)
{
  switch(vp->attrType)
  {
  case RADIUS_ATTR_VALUE_TYPE_STRING:
    if (length > L7_NULL && length <= RADIUS_VALUE_LENGTH)
    {
      bcopy(value, vp->strValue, length);
      vp->strValue[length] = '\0';
      vp->intValue = length;
    }
    break;

  case RADIUS_ATTR_VALUE_TYPE_INTEGER:
  case RADIUS_ATTR_VALUE_TYPE_IP_ADDR:
  case RADIUS_ATTR_VALUE_TYPE_DATE:
    vp->intValue = *(L7_uint32 *)value;
    break;
  }

  return;
}

/*************************************************************************
*
* @purpose Insert a ValuePair entry into the list of ValuePair entries
*
* @param vp @b{(input)} the structure with the value to be inserted
* @param vpList @b{(output)} list into which the value-pair is to be inserted
*
* @returns void
*
* @comments
*
* @end
*
*************************************************************************/
void radiusAttrValuePairInsert(radiusValuePair_t **vpList,
                               radiusValuePair_t *vp)
{
  radiusValuePair_t *vpTemp;

  if (*vpList == L7_NULL)
  {
    *vpList = vp;
    return;
  }

  for (vpTemp = *vpList; vpTemp->nextPair; vpTemp = vpTemp->nextPair)
    /* No action - walk to end of list */ ;
  vpTemp->nextPair = vp;
}

/*************************************************************************
*
* @purpose Add an attribute-value pair to the given list
*
* @param vpList @b{(input/output)} list into which the value-pair is to be added
* @param attrId @b{(input)} type of attribute to be added
* @param vsAttrId @b{(input)} type of vendor-specific attribute to be added
* @param value @b{(input)} the value of the attribute
* @param length @b((input)) length of the attribute
*
* @returns L7_SUCCESS if the valuepair is added successfully
* @returns L7_FAILURE in case of an error
*
* @comments The vpList must be freed by the RADIUS services user if it is
*           not successfully passed to the client code via an API. It can
*           be freed by calling radiusAttrValuePairListFree.
*
* @end
*
*************************************************************************/
L7_RC_t radiusAttrValuePairAdd(radiusValuePair_t **vpList,
                               L7_uint32 attrId,
                               L7_uint32 vsAttrId,
                               void *value,
                               L7_uint32 length)
{
  radiusValuePair_t *vp = L7_NULL;
  radiusDictAttr_t *attrDef;

  /* get buffer for vp */
  if (bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)(&vp)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: could not get buffer");
    return L7_FAILURE;
  }

 (void)memset((void *)vp, L7_NULL, RADIUS_SM_BUF_SIZE);

  if (((vsAttrId && (attrDef = radiusVSAttributeGet(attrId, vsAttrId)) != L7_NULL) ||
      (attrDef = radiusAttributeByIdGet(attrId)) != L7_NULL))
  {
    vp->attrId = (L7_uint32)attrDef->attrId;
    vp->vsAttrId = (L7_uint32)attrDef->vsAttrId;
    vp->attrType = (L7_uint32)attrDef->attrType;
    vp->vendorCode = (L7_uint32)attrDef->vendorCode;
    radiusAttrValueAssign(vp, value, length);
    radiusAttrValuePairInsert(vpList, vp);

    return L7_SUCCESS;
  }

  if (vp != L7_NULL)
  {
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)vp);
  }
  return L7_FAILURE;
}

/*************************************************************************
*
* @purpose Delete a given ValuePair from the ValuePair List
*
* @param vpList @b{(output)} list from which the value-pair is to be deleted
* @param attrId @b{(input)} type of attribute to be deleted
* @param vsAttrId @b{(input)} type of vendor-specific attribute to be deleted
*
* @returns void
*
* @comments
*
* @end
*
*************************************************************************/
void radiusAttrValuePairDel(radiusValuePair_t **vpList,
                            L7_uint32 attrId,
                            L7_uint32 vsAttrId)
{
  radiusValuePair_t *vp, *prevVp;

  for (vp = *vpList, prevVp = L7_NULL; vp; vp = vp->nextPair)
  {
    if (((vsAttrId && vp->vendorCode == attrId) && (vp->vsAttrId == vsAttrId)) ||
       (vp->attrId == attrId))
    {
      if (prevVp)
      {
        prevVp->nextPair = vp->nextPair;
      }
      else
      {
        *vpList = vp->nextPair;
      }

      bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)vp);

      break;
    }

    prevVp = vp;
  }

  return;
}

/*************************************************************************
*
* @purpose Free all ValuePair entries in the list
*
* @param vpList @b{(input)} list from which all entires are deleted
*
* @returns void
*
* @comments
*
* @end
*
*************************************************************************/
void radiusAttrValuePairListFree(radiusValuePair_t *vpList)
{
  radiusValuePair_t *vp, *nextVp;

  for (vp = vpList; vp; vp = nextVp)
  {
    nextVp = vp->nextPair;
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)vp);
  }

  return;
}

/*************************************************************************
*
* @purpose Authenticate a user with a RADIUS server
*
* @param userName @b{(input)} name of user to be authenticated
* @param password @b{(input)} password for the user
* @param state @b{(input)} State attribute for Challenge Access-Request
* @param correlator @b{(input)} application dependent correlator.
* @param componentID @b{(input)} FastPath Component ID of requesting function
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This is an easier API for RADIUS services users that do not
*           wish to supply a value pair list. It can be used in place of
*           the radiusAccessRequestSend API. If any attribute other than
*           the following are required to authenticate the user, this API
*           should not be used:
*
*           Attributes specified as function parameters - User-Name,
*             User-Password, State
*
*           Attributes included by RADIUS services - NAS-Identifier
*
*           The State attribute MUST be sent in a subsequent Access-Request
*           resulting from an Access-Challenge. It MUST be unmodified
*           from what was received in the Access-Challenge reply attributes,
*           if any. It is not required in an initial Access-Request.
*
*           This API will result in a callback to the registered function
*           for the specified FastPath Component ID.
*
* @end
*
*************************************************************************/
L7_RC_t radiusUserAuthenticate(L7_char8 *userName,
                               L7_char8 *password,
                               L7_char8 *state,
                               L7_uint32 correlator,
                               L7_COMPONENT_IDS_t componentID)
{
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc = cnfgrApiComponentNameGet(componentID, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if(L7_NULLPTR == userName || L7_NULLPTR == password)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusUserAuthenticate(): Invalid parameters.\n");
    return L7_FAILURE;
  }

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending the message to one of all possible servers \
                     ( via interface: %s IPAddr_type: %d componentID: %d, %s correlator: %d)",
                     __FUNCTION__,osapiInet_ntoa(L7_NULL),
                     L7_IP_ADDRESS_TYPE_UNKNOWN,componentID,name,correlator);


  return radiusUserAuthenticateMsg (userName,password,state,correlator,
                     componentID,L7_NULLPTR,L7_IP_ADDRESS_TYPE_UNKNOWN,L7_NULL);
}

/*************************************************************************
*
* @purpose Authenticate a user with a named RADIUS server
*
* @param userName @b{(input)} name of user to be authenticated
* @param password @b{(input)} password for the user
* @param state @b{(input)} State attribute for Challenge Access-Request
* @param correlator @b{(input)} application dependent correlator.
* @param componentID @b{(input)} FastPath Component ID of requesting function
* @param L7_char8 *serverName @b{(input)} Name of the server.
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} server address Type.
* @param L7_uchar8 *ipAddr @b{(input)} IP address of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This is an easier API for RADIUS services users that do not
*           wish to supply a value pair list. It can be used in place of
*           the radiusAccessRequestSend API. If any attribute other than
*           the following are required to authenticate the user, this API
*           should not be used:
*
*           Attributes specified as function parameters - User-Name,
*             User-Password, State
*
*           Attributes included by RADIUS services - NAS-Identifier
*
*           The State attribute MUST be sent in a subsequent Access-Request
*           resulting from an Access-Challenge. It MUST be unmodified
*           from what was received in the Access-Challenge reply attributes,
*           if any. It is not required in an initial Access-Request.
*
*           This API will result in a callback to the registered function
*           for the specified FastPath Component ID.
*
* @end
*
*************************************************************************/
L7_RC_t radiusNamedUserAuthenticate(L7_char8 *userName,
                               L7_char8 *password,
                               L7_char8 *state,
                               L7_uint32 correlator,
                               L7_COMPONENT_IDS_t componentID,
                               L7_char8 *serverName,
                               L7_IP_ADDRESS_TYPE_t type,
                               L7_uchar8 *ipAddr)
{

  if(L7_NULLPTR == userName || L7_NULLPTR == password ||
                     L7_NULLPTR == serverName)

  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Invalid parameters.\n",__FUNCTION__);
    return L7_FAILURE;
  }

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending the message to named Server: %s ( type: %d ipAddr %s",
                     __FUNCTION__,serverName,type,ipAddr);

  return radiusUserAuthenticateMsg  (userName,password,state,correlator,
                     componentID,serverName,type,ipAddr);
}

/*************************************************************************
*
* @purpose Authenticate a user with a named RADIUS server
*
* @param userName @b{(input)} name of user to be authenticated
* @param password @b{(input)} password for the user
* @param state @b{(input)} State attribute for Challenge Access-Request
* @param correlator @b{(input)} application dependent correlator.
* @param componentID @b{(input)} FastPath Component ID of requesting function
* @param L7_char8 *serverName @b{(input)} Name of the server.
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} server address Type.
* @param L7_uchar8 *ipAddr @b{(input)} IP address of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This is an easier API for RADIUS services users that do not
*           wish to supply a value pair list. It can be used in place of
*           the radiusAccessRequestSend API. If any attribute other than
*           the following are required to authenticate the user, this API
*           should not be used:
*
*           Attributes specified as function parameters - User-Name,
*             User-Password, State
*
*           Attributes included by RADIUS services - NAS-Identifier
*
*           The State attribute MUST be sent in a subsequent Access-Request
*           resulting from an Access-Challenge. It MUST be unmodified
*           from what was received in the Access-Challenge reply attributes,
*           if any. It is not required in an initial Access-Request.
*
*           This API will result in a callback to the registered function
*           for the specified FastPath Component ID.
*           Use serverName = L7_NULLPTR when request needs to be sent to
*           an active server out of all server names
*
* @end
*
*************************************************************************/
L7_RC_t radiusUserAuthenticateMsg(L7_char8 *userName,
                               L7_char8 *password,
                               L7_char8 *state,
                               L7_uint32 correlator,
                               L7_COMPONENT_IDS_t componentID,
                               L7_char8 *serverName,
                               L7_IP_ADDRESS_TYPE_t type,
                               L7_uchar8 *ipAddr)
{
  radiusRequestInfo_t *requestInfo;
  L7_uint32 length;
  radiusServerEntry_t *serverEntry = L7_NULL;
  L7_uint32 index=0;
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  L7_uint32 socket=0;
#endif
  L7_RADIUS_SERVER_USAGE_TYPE_t usageType;
  L7_BOOL tryAllServers = L7_FALSE;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_RC_t rc = L7_SUCCESS;

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Enetered the function..",__FUNCTION__);

  if(L7_NULLPTR == userName || L7_NULLPTR == password)
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
             "RADIUS: %s(): Invalid parameters\n",__FUNCTION__);
     return L7_FAILURE;
  }

  if( L7_IP_ADDRESS_TYPE_IPV4 != type && L7_IP_ADDRESS_TYPE_UNKNOWN != type)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS: %s(): Invalid parameters\n",__FUNCTION__);
      return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusUserAuthenticate(): Failed to get lock\n");
    return L7_FAILURE;
  }
  else
  {
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
    if( L7_IP_ADDRESS_TYPE_IPV4 == type && L7_NULLPTR != ipAddr)
    {
      /* return values: L7_SUCCESS/L7_ALREADY_CONFIGURED/L7_FAILURE*/
      if( radiusNwInterfaceSocketOpen((osapiInet_addr(ipAddr)),&socket)
                     == L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: %s(): Failed to open.",__FUNCTION__);
        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
        return L7_FAILURE;
      }
    }
#endif

    if (componentID == L7_DOT1X_COMPONENT_ID)
    {
      usageType = L7_RADIUS_SERVER_USAGE_DOT1X;
    }
    else if (componentID == L7_USER_MGR_COMPONENT_ID)
    {
      usageType = L7_RADIUS_SERVER_USAGE_LOGIN;
    }
    else
    {
      usageType = L7_RADIUS_SERVER_USAGE_ALL;
    }

    if(serverName != L7_NULLPTR &&
       (radiusServerNameEntryIndexGet(serverName, &index ) != L7_SUCCESS ||
        radiusServerAuthCurrentEntryGet(index, usageType, &serverEntry) != L7_SUCCESS ||
        L7_NULLPTR == serverEntry ))
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: radiusUserAuthenticateMsg(): Could not get the named \
                      %s  Server'\n",serverName);
        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
        return L7_FAILURE;
    }
    else if (serverName == L7_NULLPTR)
    {
      tryAllServers = L7_TRUE;
      if( radiusServerNameEntryIndexGet(L7_RADIUS_SERVER_DEFAULT_NAME_AUTH, &index ) != L7_SUCCESS ||
          radiusServerAuthCurrentEntryGet(index, usageType, &serverEntry) != L7_SUCCESS ||
          L7_NULLPTR == serverEntry )
      {
        /* Get first valid serverNameIndex */
        memset(name,0,sizeof(name));
        rc = radiusAuthServerNameEntryOrderlyGetNext(name, name, &serverEntry);
        while (rc == L7_SUCCESS)
        {
          if (radiusServerAuthCurrentEntryGet(serverEntry->serverNameIndex, usageType, &serverEntry) == L7_SUCCESS &&
              serverEntry != L7_NULLPTR)
          {
            index = serverEntry->serverNameIndex;
            break;
          }
          rc = radiusAuthServerNameEntryOrderlyGetNext(name, name, &serverEntry);
        }
        if (rc != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "RADIUS: radiusUserAuthenticateMsg(): Could not get at least one named Server'\n");
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return L7_FAILURE;
        }
      }
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  if(bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)(&requestInfo)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Could not allocate requestInfo. "
           "Resource issue with RADIUS Client service.\n" );
    return L7_FAILURE;
  }

  (void)memset((void *)requestInfo, L7_NULL, RADIUS_SM_BUF_SIZE);

  requestInfo->requestType = RADIUS_REQUEST_TYPE_AUTH;

  requestInfo->componentID = componentID;
  requestInfo->correlator = correlator;
  requestInfo->serverNameIndex = index;
  requestInfo->tryAllServers = tryAllServers;

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  if( L7_IP_ADDRESS_TYPE_IPV4 == type )
  {
    requestInfo->specificSocket = socket;
  }
#endif

  if (state != L7_NULL)
  {
    length = (L7_uint32)min(RADIUS_VALUE_LENGTH - 1, strlen(state));
    strncpy((L7_char8 *)requestInfo->state, state, length);
    requestInfo->stateLength = length;
  }

  length = (L7_uint32)min(RADIUS_MAX_USERNAME_LENGTH - 1, strlen(userName));
  osapiStrncpySafe((L7_char8 *)requestInfo->userName, userName, length + 1);
  requestInfo->userNameLength = length;

  length = (L7_uint32)min(RADIUS_MAX_PASSWORD_LENGTH - 1, strlen(password));
  osapiStrncpySafe((L7_char8 *)requestInfo->userPassword, password, length + 1);
  requestInfo->userPasswordLength = length;

  if (radiusIssueCmd(RADIUS_REQUEST_INFO, L7_NULL, (void *)requestInfo) != L7_SUCCESS)
  {
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)requestInfo);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*************************************************************************
*
* @purpose Send an RADIUS Access-Request packet with the supplied VP list.
*
* @param vpList @b((output)) populated list with the Access-Request attributes.
* @param correlator @b{(input)} application dependent correlator
* @param componentID @b{(input)} FastPath Component ID of requesting function
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This is an more sophisticated API for RADIUS services users that
*           wish to specify all of the attributes by supplying a value pair
*           list. It can be used in place of the radiusUserAuthenticate API
*           when all required attributes are added to the value pair list.
*
*           Attributes included by RADIUS services - Message-Authenticator
*            (if an EAP-Message is included in the vp list).
*
*           This API will result in a callback to the registered function
*           for the specified FastPath Component ID.
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccessRequestSend(radiusValuePair_t *vpList,
                                L7_uint32 correlator,
                                 L7_COMPONENT_IDS_t componentID)
{
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc = cnfgrApiComponentNameGet(componentID, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending the message to one of all possible servers \
                     ( via interface: %s IPAddr_type: %d componentID: %d, %s correlator: %d)",
                     __FUNCTION__,osapiInet_ntoa(L7_NULL),
                     L7_IP_ADDRESS_TYPE_UNKNOWN,componentID,name,correlator);

  return radiusAccessRequestMsgSend(vpList,correlator,componentID,
                     L7_NULLPTR,L7_IP_ADDRESS_TYPE_UNKNOWN,L7_NULL);
}

/*************************************************************************
*
* @purpose Send an RADIUS Access-Request packet with the supplied VP list.
*
* @param vpList @b((output)) populated list with the Access-Request attributes.
* @param correlator @b{(input)} application dependent correlator
* @param componentID @b{(input)} FastPath Component ID of requesting function
* @param L7_char8 *serverName @b{(input)} Name of the server.
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} server address Type.
* @param L7_uchar8 *ipAddr @b{(input)} IP address of the server.
* @param L7_BOOL useIPAddr @b{(input)} Flag to indicate usage of IP addr.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This is an more sophisticated API for RADIUS services users that
*           wish to specify all of the attributes by supplying a value pair
*           list. It can be used in place of the radiusUserAuthenticate API
*           when all required attributes are added to the value pair list.
*
*           Attributes included by RADIUS services - Message-Authenticator
*            (if an EAP-Message is included in the vp list).
*
*           This API will result in a callback to the registered function
*           for the specified FastPath Component ID.
*           Use serverName = L7_NULLPTR when request needs to be sent to
*           an active server out of all server names
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccessRequestMsgSend(radiusValuePair_t *vpList,
                                L7_uint32 correlator,
                                L7_COMPONENT_IDS_t componentID,
                                L7_char8 *serverName,
                                L7_IP_ADDRESS_TYPE_t type,
                                L7_uchar8 *ipAddr)
{
  radiusRequestInfo_t *requestInfo;
  radiusServerEntry_t *serverEntry = L7_NULL;
  L7_uint32 index=0;
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  L7_uint32 socket=0;
#endif
  L7_RADIUS_SERVER_USAGE_TYPE_t usageType;
  L7_BOOL tryAllServers = L7_FALSE;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_RC_t  rc = L7_SUCCESS;


  if( L7_IP_ADDRESS_TYPE_IPV4 != type && L7_IP_ADDRESS_TYPE_UNKNOWN != type)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "%s(): Invalid parameters\n",__FUNCTION__);
    radiusAttrValuePairListFree(vpList);
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "%s(): Failed to get lock\n",__FUNCTION__);
    radiusAttrValuePairListFree(vpList);
    return L7_FAILURE;
  }
  else
  {

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED

    if( L7_IP_ADDRESS_TYPE_IPV4 == type && L7_NULL != ipAddr )
    {
      /* return values: L7_SUCCESS/L7_ALREADY_CONFIGURED/L7_FAILURE*/
      if( radiusNwInterfaceSocketOpen((osapiInet_addr(ipAddr)),&socket)
                     == L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "%s(): Failed to open.\n",__FUNCTION__);
        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
        radiusAttrValuePairListFree(vpList);
        return L7_FAILURE;
      }
    }
#endif
    if (componentID == L7_DOT1X_COMPONENT_ID)
    {
      usageType = L7_RADIUS_SERVER_USAGE_DOT1X;
    }
    else if (componentID == L7_USER_MGR_COMPONENT_ID)
    {
      usageType = L7_RADIUS_SERVER_USAGE_LOGIN;
    }
    else
    {
      usageType = L7_RADIUS_SERVER_USAGE_ALL;
    }

    if(serverName != L7_NULLPTR &&
       (radiusServerNameEntryIndexGet(serverName, &index ) != L7_SUCCESS ||
        radiusServerAuthCurrentEntryGet(index, usageType, &serverEntry) != L7_SUCCESS ||
        L7_NULLPTR == serverEntry))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "%s(): Named radius server:%s not found! \n",__FUNCTION__, serverName);
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      radiusAttrValuePairListFree(vpList);
      return L7_FAILURE;
    }
    else if (serverName == L7_NULLPTR)
    {
      tryAllServers = L7_TRUE;
      if( radiusServerNameEntryIndexGet(L7_RADIUS_SERVER_DEFAULT_NAME_AUTH, &index ) != L7_SUCCESS ||
          radiusServerAuthCurrentEntryGet(index, usageType, &serverEntry) != L7_SUCCESS ||
          L7_NULLPTR == serverEntry )
      {
        /* Get first valid serverNameIndex */
        memset(name,0,sizeof(name));
        rc = radiusAuthServerNameEntryOrderlyGetNext(name, name, &serverEntry);
        while (rc == L7_SUCCESS)
        {
          if (radiusServerAuthCurrentEntryGet(serverEntry->serverNameIndex, usageType, &serverEntry) == L7_SUCCESS &&
              serverEntry != L7_NULLPTR)
          {
            index = serverEntry->serverNameIndex;
            break;
          }
          rc = radiusAuthServerNameEntryOrderlyGetNext(name, name, &serverEntry);
        }
        if (rc != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "RADIUS: radiusUserAuthenticateMsg(): Could not get atleast one named Server'\n");
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return L7_FAILURE;
        }
      }
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }


  if(bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)(&requestInfo)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "%s(): Could not allocate requestInfo\n",__FUNCTION__);
    radiusAttrValuePairListFree(vpList);
    return L7_FAILURE;
  }

  (void)memset((void *)requestInfo, L7_NULL, RADIUS_SM_BUF_SIZE);

  requestInfo->requestType = RADIUS_REQUEST_TYPE_AUTH_VP;

  requestInfo->componentID = componentID;
  requestInfo->correlator = correlator;
  requestInfo->vpList = vpList;
  requestInfo->serverNameIndex = index;
  requestInfo->tryAllServers = tryAllServers;

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  if( L7_IP_ADDRESS_TYPE_IPV4 == type )
  {
    requestInfo->specificSocket = socket;
  }
#endif

  if (radiusIssueCmd(RADIUS_REQUEST_INFO, L7_NULL, (void *)requestInfo) != L7_SUCCESS)
  {
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)requestInfo);
    radiusAttrValuePairListFree(vpList);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*************************************************************************
*
* @purpose Send an RADIUS Access-Request packet with the supplied VP list.
*
* @param vpList @b((output)) populated list with the Access-Request attributes.
* @param correlator @b{(input)} application dependent correlator
* @param componentID @b{(input)} FastPath Component ID of requesting function
* @param L7_char8 *serverName @b{(input)} Name of the server.
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} server address Type.
* @param L7_uchar8 *ipAddr @b{(input)} IP address of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This is an more sophisticated API for RADIUS services users that
*           wish to specify all of the attributes by supplying a value pair
*           list. It can be used in place of the radiusUserAuthenticate API
*           when all required attributes are added to the value pair list.
*
*           Attributes included by RADIUS services - Message-Authenticator
*            (if an EAP-Message is included in the vp list).
*
*           This API will result in a callback to the registered function
*           for the specified FastPath Component ID.
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccessRequestNamedSend(radiusValuePair_t *vpList,
                                L7_uint32 correlator,
                                L7_COMPONENT_IDS_t componentID,
                                L7_char8 *serverName,
                                L7_IP_ADDRESS_TYPE_t type,
                                L7_uchar8 *ipAddr)
{
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc = cnfgrApiComponentNameGet(componentID, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if( L7_NULLPTR == vpList || L7_NULLPTR == serverName )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusAccessRequestNamedSend(): Invalid Parameters. \n");
    return L7_FAILURE;
  }

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending the message to named Server: %s \
                     ( via interface: %s IPAddr_type: %d componentID: %d, %s correlator: %d)",
                     __FUNCTION__,serverName,ipAddr,type,componentID,name,correlator);

  return radiusAccessRequestMsgSend(vpList,correlator,componentID,
                         serverName,type,ipAddr);
}

/*************************************************************************
*
* @purpose Start the Accounting for the session
*
* @param sessionId @b{(input)} session id string
* @param port @b((input)) external Interface number
* @param vpList @b((output)) populated list with accounting attributes
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccountingStart(L7_char8 *sessionId,
                              L7_uint32 port,
                              radiusValuePair_t *vpList)
{
  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending the message to one of all possible servers ( via interface: %s IPAddr_type: %d )",
                     __FUNCTION__,osapiInet_ntoa(L7_NULL),
                     L7_IP_ADDRESS_TYPE_UNKNOWN);

  return radiusAccountingMsgStart(sessionId, port, vpList,L7_NULLPTR,
                     L7_IP_ADDRESS_TYPE_UNKNOWN,L7_NULL);
}

/*************************************************************************
*
* @purpose Start the Accounting for the session
*
* @param sessionId @b{(input)} session id string
* @param port @b((input)) external Interface number
* @param vpList @b((output)) populated list with accounting attributes
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccountingNamedStart(L7_char8 *sessionId,
                              L7_uint32 port,
                              radiusValuePair_t *vpList,
                              L7_char8 *serverName,
                              L7_IP_ADDRESS_TYPE_t type,
                              L7_uchar8 *ipAddr)
{
  if( L7_NULLPTR == vpList || L7_NULLPTR == serverName )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusAccessRequestNamedSend(): Invalid Parameters. \n");
    return L7_FAILURE;
  }

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending the message to named Server: %s ( via interface: %s IPAddr_type: %d )",
                     __FUNCTION__,serverName,ipAddr,type);

    return radiusAccountingMsgStart(sessionId, port, vpList,serverName,
                     type,ipAddr);

}

/*************************************************************************
*
* @purpose Send the Accounting Start Message.
*
* @param sessionId @b{(input)} session id string
* @param port @b((input)) external Interface number
* @param vpList @b((output)) populated list with accounting attributes
* @param L7_char8 *serverName @b{(input)} Name of the Accounting server.
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} server address Type.
* @param L7_uchar8 *ipAddr @b{(input)} IP address of the server.
* @param L7_BOOL useIPAddr @b{(input)} Flag to indicate usage of IP addr.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*           Use serverName = L7_NULLPTR when request needs to be sent to
*           an active server out of all server names
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccountingMsgStart(L7_char8 *sessionId,
                              L7_uint32 port,
                              radiusValuePair_t *vpList,
                              L7_char8 *serverName,
                              L7_IP_ADDRESS_TYPE_t type,
                              L7_uchar8 *ipAddr)
{
  radiusRequestInfo_t *acctInfo;
  radiusServerEntry_t *serverEntry = L7_NULL;
    L7_uint32 index=0;
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  L7_uint32 socket=0;
#endif
  L7_BOOL tryAllServers = L7_FALSE;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_RC_t  rc = L7_SUCCESS;


  if( L7_IP_ADDRESS_TYPE_IPV4 != type && L7_IP_ADDRESS_TYPE_UNKNOWN != type)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusAccountingMsgStart(): Invalid parameters\n");
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusAccountingNamedStart(): Failed to get lock\n");
    return L7_FAILURE;
  }
  else
  {

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
    if( L7_IP_ADDRESS_TYPE_IPV4 == type && L7_NULLPTR != ipAddr)
    {
      /* return values: L7_SUCCESS/L7_ALREADY_CONFIGURED/L7_FAILURE*/
      if( radiusNwInterfaceSocketOpen((osapiInet_addr(ipAddr)),&socket) == L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: radiusAccountingMsgStart(): Failed to open.\n");
        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
        return L7_FAILURE;
      }
    }
#endif

    if(serverName != L7_NULLPTR &&
       (radiusAcctServerNameEntryIndexGet(serverName, &index ) != L7_SUCCESS ||
        radiusServerAcctCurrentEntryGet(index,&serverEntry) != L7_SUCCESS ||
        L7_NULLPTR == serverEntry ))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS: radiusAccountingNamedStart(): Could not get the '%s'\n",serverName);
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_FAILURE;
    }
    else if (serverName == L7_NULLPTR)
    {
      tryAllServers = L7_TRUE;
      if (radiusAcctServerNameEntryIndexGet(L7_RADIUS_SERVER_DEFAULT_NAME_ACCT, &index ) != L7_SUCCESS ||
          radiusServerAcctCurrentEntryGet(index,&serverEntry) != L7_SUCCESS ||
          L7_NULLPTR == serverEntry )
      {
        /* Get first valid serverNameIndex */
        rc = radiusAcctServerNameEntryOrderlyGetNext(name, name, &serverEntry);
        while (rc == L7_SUCCESS)
        {
          if (radiusServerAcctCurrentEntryGet(serverEntry->serverNameIndex, &serverEntry) == L7_SUCCESS &&
              serverEntry != L7_NULLPTR)
          {
            index = serverEntry->serverNameIndex;
            break;
          }
          rc = radiusAcctServerNameEntryOrderlyGetNext(name, name, &serverEntry);
        }

        if (rc != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "RADIUS: radiusAccountingNamedStart(): Could not get at least one named Server'\n");
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return L7_FAILURE;
        }
      }
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  if(bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)(&acctInfo)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Could not allocate"
           " accounting requestInfo. Resource issue with RADIUS Client service.\n");
    return L7_FAILURE;
  }

  (void)memset((void *)acctInfo, L7_NULL, RADIUS_SM_BUF_SIZE);

  acctInfo->requestType = RADIUS_REQUEST_TYPE_ACCT;
  acctInfo->serverNameIndex = index;
  acctInfo->tryAllServers = tryAllServers;

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  if( L7_IP_ADDRESS_TYPE_IPV4 == type )
  {
    acctInfo->specificSocket = socket;
  }
#endif

  (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_SESSION_ID, L7_NULL, sessionId, (L7_uint32)strlen(sessionId));
  (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_NAS_PORT, L7_NULL, &port, (L7_uint32)sizeof(port));

  acctInfo->acctStatusType = RADIUS_ACCT_STATUS_TYPE_START;
  acctInfo->vpList = vpList;

  if (radiusIssueCmd(RADIUS_REQUEST_INFO, L7_NULL, (void *)acctInfo) != L7_SUCCESS)
  {
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)acctInfo);
    radiusAttrValuePairListFree(vpList);

    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*************************************************************************
*
* @purpose Stop the Accounting for the session
*
* @param sessionId @b{(input)} session id string
* @param port @b((input)) external interface number
* @param vpList @b((output)) populated list with accounting attributes
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccountingStop(L7_char8 *sessionId,
                             L7_uint32 port,
                             radiusValuePair_t *vpList)
{

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending the message to one active server of all( via interface: %s IPAddr_type: %d )",
                     __FUNCTION__,osapiInet_ntoa(L7_NULL),
                     L7_IP_ADDRESS_TYPE_UNKNOWN);

  return radiusAccountingMsgStop(sessionId, port, vpList,L7_NULLPTR,
                     L7_IP_ADDRESS_TYPE_UNKNOWN,L7_NULL);
}

/*************************************************************************
*
* @purpose Stop the Accounting for the session
*
* @param sessionId @b{(input)} session id string
* @param port @b((input)) external interface number
* @param vpList @b((output)) populated list with accounting attributes
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccountingNamedStop(L7_char8 *sessionId,
                             L7_uint32 port,
                             radiusValuePair_t *vpList,
                             L7_char8 *serverName,
                             L7_IP_ADDRESS_TYPE_t type,
                             L7_uchar8 *ipAddr)
{
  if( L7_NULLPTR == vpList || L7_NULLPTR == serverName )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusAccessRequestNamedSend(): Invalid Parameters. \n");
    return L7_FAILURE;
  }

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending the message to named Server: %s ( via interface: %s IPAddr_type: %d )",
                     __FUNCTION__,serverName,ipAddr,type);

  return radiusAccountingMsgStop(sessionId, port, vpList,serverName,
                     type,ipAddr);

}

/*************************************************************************
*
* @purpose Stop the Accounting for the session
*
* @param sessionId @b{(input)} session id string
* @param port @b((input)) external interface number
* @param vpList @b((output)) populated list with accounting attributes
* @param L7_char8 *serverName @b{(input)} Name of the Accounting server.
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} server address Type.
* @param L7_uchar8 *ipAddr @b{(input)} IP address of the server.
* @param L7_BOOL useIPAddr @b{(input)} Flag to indicate usage of IP addr.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*           Use serverName = L7_NULLPTR when request needs to be sent to
*           an active server out of all server names
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccountingMsgStop(L7_char8 *sessionId,
                             L7_uint32 port,
                             radiusValuePair_t *vpList,
                             L7_char8 *serverName,
                             L7_IP_ADDRESS_TYPE_t type,
                             L7_uchar8 *ipAddr)
{
  radiusRequestInfo_t *acctInfo;
  radiusServerEntry_t *serverEntry = L7_NULL;
  L7_uint32 index=0;
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  L7_uint32 socket=0;
#endif
  L7_BOOL tryAllServers = L7_FALSE;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_RC_t  rc = L7_SUCCESS;

  if( L7_IP_ADDRESS_TYPE_IPV4 != type && L7_IP_ADDRESS_TYPE_UNKNOWN != type )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusAccountingMsgStop(): Invalid parameters\n");
    return L7_FAILURE;
  }


  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusAccountingMsgStop(): Failed to get lock\n");
    return L7_FAILURE;
  }
  else
  {
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
    if( L7_IP_ADDRESS_TYPE_IPV4 == type  && L7_NULLPTR != ipAddr)
    {
      /* return values: L7_SUCCESS/L7_ALREADY_CONFIGURED/L7_FAILURE*/
      if( radiusNwInterfaceSocketOpen((osapiInet_addr(ipAddr)),&socket) == L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: radiusAccessRequestMsgSend(): Failed to open.\n");
        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
        return L7_FAILURE;
      }
    }
#endif

    if(serverName != L7_NULLPTR &&
       (radiusAcctServerNameEntryIndexGet(serverName, &index ) != L7_SUCCESS ||
        radiusServerAcctCurrentEntryGet(index,&serverEntry) != L7_SUCCESS ||
        L7_NULLPTR == serverEntry))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS: radiusAccountingNamedStop(): Could not get the 'Default RADIUS Acct. Server'\n");
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_FAILURE;
    }
    else if (serverName == L7_NULLPTR)
    {
      tryAllServers = L7_TRUE;
      if (radiusAcctServerNameEntryIndexGet(L7_RADIUS_SERVER_DEFAULT_NAME_ACCT, &index ) != L7_SUCCESS ||
          radiusServerAcctCurrentEntryGet(index,&serverEntry) != L7_SUCCESS ||
          L7_NULLPTR == serverEntry )
      {
        /* Get first valid serverNameIndex */
        rc = radiusAcctServerNameEntryOrderlyGetNext(name, name, &serverEntry);
        while (rc == L7_SUCCESS)
        {
          if (radiusServerAcctCurrentEntryGet(serverEntry->serverNameIndex, &serverEntry) == L7_SUCCESS &&
              serverEntry != L7_NULLPTR)
          {
            index = serverEntry->serverNameIndex;
            break;
          }
          rc = radiusAcctServerNameEntryOrderlyGetNext(name, name, &serverEntry);
        }

        if (rc != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "RADIUS: radiusAccountingNamedStop(): Could not get at least one named Server'\n");
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return L7_FAILURE;
        }
      }
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  if(bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)(&acctInfo)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Could not allocate requestInfo. "
           "Resource issue with RADIUS Client service.\n" );
    return L7_FAILURE;
  }

  (void)memset((void *)acctInfo, L7_NULL, RADIUS_SM_BUF_SIZE);

  acctInfo->requestType = RADIUS_REQUEST_TYPE_ACCT;

  (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_SESSION_ID, L7_NULL, sessionId, (L7_uint32)strlen(sessionId));
  (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_NAS_PORT, L7_NULL, &port, (L7_uint32)sizeof(port));

  acctInfo->acctStatusType = RADIUS_ACCT_STATUS_TYPE_STOP;
  acctInfo->vpList = vpList;
  acctInfo->serverNameIndex = index;
  acctInfo->tryAllServers = tryAllServers;

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  if( L7_IP_ADDRESS_TYPE_IPV4 == type )
  {
    acctInfo->specificSocket = socket;
  }
#endif


  if (radiusIssueCmd(RADIUS_REQUEST_INFO, L7_NULL, (void *)acctInfo) != L7_SUCCESS)
  {
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)acctInfo);
    radiusAttrValuePairListFree(vpList);

    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Get the maximum number of retransmissions parameter
*
* @param serverIPAddr  @b((input)) server IP address (ALL_RADIUS_SERVERS=>global)
* @param maxRetransmit @b((output)) current value of MaxNoOfRetrans parameter
* @params paramStatus  @b((output)) if the parameter for a specific server is
*                                  global/local.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE

* @comments
*
* @end
*
********************************************************************************/
L7_RC_t radiusMaxNoOfRetransGet(dnsHost_t                            *hostAddr,
                                L7_uint32                            *maxRetransmit,
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  radiusServerEntry_t *serverEntry = L7_NULLPTR;
  radiusClient_t *radiusClient;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    radiusClient = radiusClientGet();

    if (radiusClient != L7_NULLPTR)
    {
      if(((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
      {  /* Global value is fetched */
        *maxRetransmit = radiusClient->maxNumRetrans;
      }
      else
      { /* Server specific value is fetched */
        if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
        }
        else
        {
          serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
        }
        if (serverEntry != L7_NULLPTR)
        {
          *paramStatus = (serverEntry->serverConfig.localConfigMask &
                          L7_RADIUS_SERVER_CONFIG_MAX_NUM_RETRANS) ? L7_RADIUSLOCAL
                                                          : L7_RADIUSGLOBAL;
          /* Irrespective of whether the server is using global value or a
          server-specific one, serverConfig always has the one in use. */
          *maxRetransmit = serverEntry->serverConfig.maxNumRetrans;
        }
        else
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "radiusMaxNoOfRetransGet(): Failed to find server");
          return L7_FAILURE;
        }
      }
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the maximum number of retransmissions parameter
*
* @param serverIPAddr  @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param maxRetransmit @b((input)) new value of maxRetransmit parameter
* @param paramStatus   @b((input)) if the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusMaxRetransSet(dnsHost_t                           *hostAddr,
                            L7_uint32                            maxRetransmit,
                            L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  radiusAuthServerInfo_t  serverInfo;
  radiusServerEntry_t *serverEntry = L7_NULLPTR;

  if ((maxRetransmit >= L7_RADIUS_MIN_RETRANS) &&
     (maxRetransmit <= L7_RADIUS_MAX_RETRANS))
  {
    if(!((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
    { /* Verify if the server entry exists */
      if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
      {
        if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
        }
        else
        {
          serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
        }
        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
        if (serverEntry == L7_NULLPTR)
        {
          /* No entry with serverIPAddr was found */
          return L7_FAILURE;
        }
      }
    }
    serverInfo.localGlobal = paramStatus;
    serverInfo.val.number = maxRetransmit;
    return radiusIssueCmd(RADIUS_MAX_RETRANS_SET, hostAddr, (void *)&serverInfo);
  }

  return L7_FAILURE;
}

/*******************************************************************************
*
* @purpose Get the server timeout parameter
*
* @param serverIPAddr @b((input)) server IP address (ALL_RADIUS_SERVERS=>global)
* @param timeOut      @b((output)) current value of timeout parameter
* @params paramStatus @b((output)) if the parameter for a specific server is
*                                  global/local.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE

* @comments
*
* @end
*
********************************************************************************/
L7_RC_t radiusTimeOutDurationGet(dnsHost_t                            *hostAddr,
                                 L7_uint32                            *timeOut,
                                 L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  radiusServerEntry_t *serverEntry = L7_NULLPTR;
  radiusClient_t *radiusClient;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    radiusClient = radiusClientGet();

    if (radiusClient != L7_NULLPTR)
    {
      if(((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
      {  /* Global value is fetched */
        *timeOut = radiusClient->timeOutDuration / RADIUS_TIMER_GRANULARITY;
      }
      else
      {  /* Server specific value is fetched */
        if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
        }
        else
        {
          serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
        }
        if (serverEntry != L7_NULLPTR)
        {
          *paramStatus = ( serverEntry->serverConfig.localConfigMask & L7_RADIUS_SERVER_CONFIG_TIMEOUT)
            ? L7_RADIUSLOCAL
            : L7_RADIUSGLOBAL;
          /* Irrespective of whether the server is using global value or a
          server-specific one, serverConfig always has the one in use. */
          *timeOut = serverEntry->serverConfig.timeOutDuration / RADIUS_TIMER_GRANULARITY;
        }
        else
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "radiusTimeOutDurationGet(): Failed to find server");
          return L7_FAILURE;
        }
      }
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the value of the time-out duration parameter
*
* @param serverIPAddr @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param timeOutVal   @b((input)) new value of timeout parameter
* @param paramStatus  @b((input)) if the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusTimeOutDurationSet(dnsHost_t                           *hostAddr,
                                 L7_uint32                            timeOutVal,
                                 L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  radiusAuthServerInfo_t  serverInfo;
  radiusServerEntry_t *serverEntry;

  if ((timeOutVal >= L7_RADIUS_MIN_TIMEOUTVAL) &&
     (timeOutVal <= L7_RADIUS_MAX_TIMEOUTVAL))
  {
    if(!((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
    {  /* Verify if the server entry exists */
      if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
      {
        if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
        }
        else
        {
          serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
        }
        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
        if (serverEntry == L7_NULLPTR)
        {
          /* No entry with serverIPAddr was found */
          return L7_FAILURE;
        }
      }
    }
    serverInfo.localGlobal = paramStatus;
    serverInfo.val.number = timeOutVal;
    return radiusIssueCmd(RADIUS_TIMEOUT_SET, hostAddr, (void *)&serverInfo);
  }

  return L7_FAILURE;
}

/*******************************************************************************
*
* @purpose Get the server deadtime parameter
*
* @param hostAddr     @b((input))  server IP address (ALL_RADIUS_SERVERS=>global)
* @param deadtime     @b((output)) current value of deadtime parameter
* @params paramStatus @b((output)) if the parameter for a specific server is
*                                  global/local.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE

* @comments
*
* @end
*
********************************************************************************/
L7_RC_t radiusServerDeadtimeGet(dnsHost_t                            *hostAddr,
                                L7_uint32                            *deadtime,
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  radiusServerEntry_t *serverEntry = L7_NULLPTR;
  radiusClient_t *radiusClient;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    radiusClient = radiusClientGet();

    if (radiusClient != L7_NULLPTR)
    {
      if(((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
      {  /* Global value is fetched */
        *deadtime = radiusClient->radiusServerDeadTime /
          RADIUS_TIMER_GRANULARITY / DEADTIME_EXTRA_GRANULARITY; /* in minutes */
      }
      else
      {  /* Server specific value is fetched */
        if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
        }
        else
        {
          serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
        }
        if (serverEntry != L7_NULLPTR)
        {
          *paramStatus = (serverEntry->serverConfig.localConfigMask &
                          L7_RADIUS_SERVER_CONFIG_DEAD_TIME) ? L7_RADIUSLOCAL
                                                          : L7_RADIUSGLOBAL;
          /* Irrespective of whether the server is using global value or a
          server-specific one, serverConfig always has the one in use. */
          *deadtime = serverEntry->serverConfig.radiusServerDeadTime /
            RADIUS_TIMER_GRANULARITY / DEADTIME_EXTRA_GRANULARITY; /* in minutes */
        }
        else
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "radiusServerDeadtimeGet(): Failed to find server");
          return L7_FAILURE;
        }
      }
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the value of the deadtime parameter
*
* @param hostAddr    @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param deadtime    @b((input)) the new value of the deadtime
* @param paramStatus @b((input)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments Deadtime is the time duration after a RADIUS sever is found
*           non-responsive or dead. During the deadtime the server should
*           not be accessed. Once the deadtime interval passes, the
*           server can be tried for access.
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerDeadtimeSet(dnsHost_t                           *hostAddr,
                                L7_uint32                            deadtime,
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  radiusAuthServerInfo_t  serverInfo;
  radiusServerEntry_t *serverEntry = L7_NULLPTR;

  if ((deadtime >= L7_RADIUS_MIN_DEADTIME) &&
     (deadtime <= L7_RADIUS_MAX_DEADTIME))
  {
    if(!((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
    {  /* Verify if the server entry exists */
      if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
      {
        if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
        }
        else
        {
          serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
        }
        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
        if (serverEntry == L7_NULLPTR)
        {
          /* No entry with serverIPAddr was found */
          return L7_FAILURE;
        }
      }
    }
    serverInfo.localGlobal = paramStatus;
    serverInfo.val.number = deadtime;
    return radiusIssueCmd(RADIUS_DEADTIME_SET, hostAddr, &serverInfo);
  }
  return L7_FAILURE;
}

/*******************************************************************************
*
* @purpose Get the source IP address parameter
*
* @param hostAddr     @b((input)) server IP address (ALL_RADIUS_SERVERS=>global)
* @param sourceIPAddr @b((output)) current value of source IP address parameter
* @params paramStatus @b((output)) if the parameter for a specific server is
*                                  global/local.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE

* @comments
*
* @end
*
********************************************************************************/
L7_RC_t radiusServerSourceIPGet(dnsHost_t                            *hostAddr,
                                L7_uint32                            *sourceIPAddr,
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  radiusServerEntry_t *serverEntry = L7_NULLPTR;
  radiusClient_t *radiusClient;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    radiusClient = radiusClientGet();

    if (radiusClient != L7_NULLPTR)
    {
      if(((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
      {  /* Global value is fetched */
        *sourceIPAddr = radiusClient->sourceIpAddress;
      }
      else
      {  /* Server specific value is fetched */
        if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
        }
        else
        {
          serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
        }
        if (serverEntry != L7_NULLPTR)
        {
          *paramStatus = (serverEntry->serverConfig.localConfigMask &
                          L7_RADIUS_SERVER_CONFIG_SOURCE_IP_ADDRESS) ? L7_RADIUSLOCAL
                                                          : L7_RADIUSGLOBAL;
          /* Irrespective of whether the server is using global value or a
          server-specific one, serverConfig always has the one in use. */
          *sourceIPAddr = serverEntry->serverConfig.sourceIpAddress;
        }
        else
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "radiusServerSourceIPGet(): Failed to find server");
          return L7_FAILURE;
        }
      }
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the value of the source IP address parameter
*
* @param hostAddr     @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param sourceIPAddr @b((input)) the new value of the source IP address
* @param paramStatus  @b((input)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerSourceIPSet(dnsHost_t                           *hostAddr,
                                L7_uint32                            sourceIPAddr,
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  radiusAuthServerInfo_t  serverInfo;
  radiusServerEntry_t *serverEntry = L7_NULLPTR;

  if(!((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
  {
    if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
    { /* Verify if the server entry exists */
      if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
      {
        serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
      }
      else
      {
        serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
      }
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      if (serverEntry == L7_NULLPTR)
      {
        /* No entry was found */
        return L7_FAILURE;
      }
    }
  }
  serverInfo.localGlobal = paramStatus;
  serverInfo.val.number = sourceIPAddr;
  return radiusIssueCmd(RADIUS_SOURCEIP_SET, hostAddr, (void *)&serverInfo);
}

/*********************************************************************
*
* @purpose Get the server usage type (Login, Dot1x, All) parameter
*
* @param hostAddr  @b((input))  server IP address
* @param usageType @b((output)) the value of server usage type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerUsageTypeGet(dnsHost_t                     *hostAddr,
                                 L7_RADIUS_SERVER_USAGE_TYPE_t *usageType)
{
  radiusServerEntry_t *serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found. Return failure */
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_FAILURE;
    }

    *usageType = serverEntry->serverConfig.usageType;

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the server usage type (Login, Dot1x, All) parameter
*
* @param hostAddr  @b((input)) server IP address
* @param usageType @b((input)) the new value of server usage type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerUsageTypeSet(dnsHost_t                    *hostAddr,
                                L7_RADIUS_SERVER_USAGE_TYPE_t  usageType)
{
  L7_RC_t rc = L7_FAILURE;
  radiusServerEntry_t *serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found */
      return L7_FAILURE;
    }
    rc = radiusIssueCmd(RADIUS_AUTH_USAGE_TYPE_SET, hostAddr, (void *)&usageType);

  }
  return rc;
}

/*********************************************************************
*
* @purpose  Get the number of configured RADIUS servers
*
* @param    pVal @b((output)) ptr to the count of configured servrs
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
********************************************************************/
L7_RC_t radiusServerCountGet(L7_uint32 *pVal)
{
  radiusClient_t *radiusClient;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    radiusClient = radiusClientGet();

    if (radiusClient != L7_NULLPTR)
    {
      *pVal = radiusClient->numAuthServers;
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the value of the RADIUS Accounting mode parameter
*
* @returns L7_TRUE - if accounting has been enabled
* @returns L7_FASLE - if accounting has been disabled
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL radiusAcctModeGet(void)
{
  radiusClient_t *radiusClient;
  L7_BOOL am;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    radiusClient = radiusClientGet();

    if (radiusClient != L7_NULLPTR)
    {
      am = ((radiusClient->acctAdminMode == L7_ENABLE) ? L7_TRUE : L7_FALSE);
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return am;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Set the value of the RADIUS Accounting mode parameter
*
* @param acctMode @b((input)) the new value of the radiusAccountingMode
*                             parameter L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctModeSet(L7_BOOL acctMode)
{
  if ((acctMode == L7_ENABLE) || (acctMode == L7_DISABLE))
  {
    return radiusIssueCmd(RADIUS_ACCT_ADMIN_MODE_SET, L7_NULL, (void *)&acctMode);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Verify a RADIUS Authentication server is a configured server.
*
* @param hostname @b((input)) Host Name or IP address of the server to verify
* @param type @b((input))     Address type either ipv4 or dns
*
* @returns L7_TRUE for a verified server
* @returns L7_FALSE for an un-verified server
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL radiusAuthServerIPHostNameVerify(L7_uchar8 *hostname,
                                         L7_IP_ADDRESS_TYPE_t type)
{
  radiusServerEntry_t * serverEntry = L7_NULLPTR;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet((osapiInet_addr(hostname)), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostname, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry != L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_TRUE;
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Verify a RADIUS Accounting server is a configured server.
*
* @param hostname @b((input)) Host Name or IP address of the server to verify
* @param type @b((input))     Address type either ipv4 or dns
*
* @returns L7_TRUE for a verified server
* @returns L7_FALSE for an un-verified server
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL radiusAcctServerIPHostNameVerify(L7_uchar8 *hostname,
                                       L7_IP_ADDRESS_TYPE_t type)
{
  radiusServerEntry_t *serverEntry = L7_NULLPTR;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet((osapiInet_addr(hostname)), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostname, RADIUS_SERVER_TYPE_ACCT);
    }

    if (serverEntry != L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_TRUE;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Get the IP address of the radius accounting server corresponding
*          to the index input
*
* @param index @b((input)) Index of the Accounting Server
* @param serverAddr @b((output)) IP Address of the server
*
* @returns L7_SUCCESS - if there is a server corresponding to the specified index
* @returns L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerIPByIndexGet(L7_uint32 index,
                                     L7_uint32 *serverAddr)
{

  radiusServerEntry_t * serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT)
      {
        if (serverEntry->radiusServerStats_t.acctStats.radiusAcctServerIndex == index)
        {
          *serverAddr = serverEntry->radiusServerConfigIpAddress;
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return L7_SUCCESS;
        }
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the Host Name or IP address of the first configured
*          radius acct server
*
* @param firstServAddr @b((output)) Host name or IP address of the first
*                                   configured server
* @param type          @b((output))Address type DNS or IP address
*
* @returns L7_SUCCESS - if there is a server configured
* @returns L7_FAILURE - if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerFirstIPHostNameGet(L7_uchar8 *firstServAddr,
                                       L7_IP_ADDRESS_TYPE_t *type)
{
  radiusServerEntry_t * serverEntry;
  L7_RC_t rc = L7_FAILURE;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT)
      {
        *type = serverEntry->radiusServerDnsHostName.hostAddrType;
        if(serverEntry->radiusServerDnsHostName.hostAddrType ==
                                           L7_IP_ADDRESS_TYPE_DNS)
        {
          strcpy(firstServAddr,
                 serverEntry->radiusServerDnsHostName.host.hostName);
        }
        else
        {
          strcpy(firstServAddr,
                 osapiInet_ntoa(serverEntry->radiusServerConfigIpAddress));
        }
        rc = L7_SUCCESS;
        break;
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Get the HostName or IP address of the radius accounting server
*          corresponding to the index input
*
* @param index      @b((input))  Index of the Accounting Server
* @param serverAddr @b((output)) Host Name  or IP Address of the server
* @param pType      @b((input))  Address type either ipv4 or dns
*
* @returns L7_SUCCESS - if there is a server corresponding to the specified index
* @returns L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerIPHostNameByIndexGet(L7_uint32 index,
                                             L7_uchar8 *serverAddr,
                                             L7_IP_ADDRESS_TYPE_t *pType)
{
  radiusServerEntry_t * serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT)
      {
        if (serverEntry->radiusServerStats_t.acctStats.radiusAcctServerIndex == index)
        {
          *pType = serverEntry->radiusServerDnsHostName.hostAddrType;
          if(serverEntry->radiusServerDnsHostName.hostAddrType ==
                                             L7_IP_ADDRESS_TYPE_DNS)
          {
            strcpy(serverAddr, serverEntry->radiusServerDnsHostName.host.hostName);
          }
          else
          {
            strcpy(serverAddr, osapiInet_ntoa(serverEntry->radiusServerConfigIpAddress));
          }
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return L7_SUCCESS;
        }
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the Host Name or IP address of the Accounting server being used
*
* @param pType @b((input))       Address type DNS or IP address
*
* @returns the host name or IP address of the Accounting server
*
* @comments
*
* @end
*
*********************************************************************/
L7_uchar8 * radiusAcctServerIPHostNameGet(L7_IP_ADDRESS_TYPE_t *pType)
{
  radiusServerEntry_t * serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT)
      {
        *pType = serverEntry->radiusServerDnsHostName.hostAddrType;
        if(serverEntry->radiusServerDnsHostName.hostAddrType ==
                                           L7_IP_ADDRESS_TYPE_DNS)
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return serverEntry->radiusServerDnsHostName.host.hostName;
        }
        else
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return (osapiInet_ntoa(serverEntry->radiusServerConfigIpAddress));
        }
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_NULL;
}

/*********************************************************************
*
* @purpose Set the HostName or IP address of the Accounting server being used
*
* @param L7_uchar8 *serverAddr @b((input)) pointer to the server address.
* @param L7_IP_ADDRESS_TYPE_t type @b((input)) Address type DNS or IP address
* @param L7_char8 *serverName @b((input)) pointer to server Name.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerIPHostNameAdd(L7_uchar8 *serverAddr,
                     L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{
  L7_uint32 numOfServers;
  radiusServerEntry_t *serverEntry = L7_NULL,*currentEntry=L7_NULL;
  dnsHost_t hostAddr;
  L7_uint32 rc = L7_FAILURE;
  L7_uint32 index = 0;

  if(L7_NULLPTR == serverAddr || L7_NULLPTR == serverName ||
        strlen(serverName) < L7_RADIUS_SERVER_NAME_MIN ||
        strlen(serverName) > L7_RADIUS_SERVER_NAME_MAX )

  {
    return L7_FAILURE;
  }
  /* Error checks: verify if entry already exists and verify if
  ** MAX servers have already been configured
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }

    numOfServers = radiusAcctServerCount();

    if( radiusAcctServerNameEntryIndexGet(serverName, &index ) == L7_SUCCESS )
    {
      radiusServerAcctCurrentEntryGet(index,&currentEntry);
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    if (serverEntry != L7_NULLPTR)
    {
      rc = L7_ALREADY_CONFIGURED;
      if( L7_NULL != currentEntry &&
            serverEntry != currentEntry )
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: %s(): Name already in use.!",__FUNCTION__);
        rc = L7_REQUEST_DENIED;
      }
    }
    else if (serverEntry == L7_NULLPTR && currentEntry != L7_NULLPTR)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: %s(): Name already in use.!",__FUNCTION__);
        rc = L7_REQUEST_DENIED;
    }
    else
    {

       if (numOfServers >= L7_RADIUS_MAX_ACCT_SERVERS)
      {
        rc = L7_ERROR;
      }
       else
       {
         if(type == L7_IP_ADDRESS_TYPE_IPV4)
          {
              hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
              hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
          }
        else
        {
          hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
          osapiStrncpySafe((L7_char8 *)hostAddr.host.hostName, (L7_char8 *)serverAddr, (L7_uint32)L7_DNS_HOST_NAME_LEN_MAX);
        }

        rc = radiusIssueCmd(RADIUS_ACCT_IPADDR_ADD, &hostAddr, serverName);
        (void)radiusSemaTake(radiusServerDbSyncSema, L7_WAIT_FOREVER, __FILE__, __LINE__);
      }
    }

    return rc;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Remove an Accounting server
*
* @param serverAddr @b((input)) the HostName or IP address to be removed
* @param type @b((input))       Address type DNS or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctHostNameServerRemove(L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type)
{
  radiusServerEntry_t * serverEntry;
  dnsHost_t hostAddr;

  /* Error checks: verify if entry exists */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    if (serverEntry == L7_NULLPTR)
    {
      /* Entry does not exist */
      return L7_FAILURE;
    }

    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
      hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
    }
    else
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
      osapiStrncpySafe((L7_char8 *)hostAddr.host.hostName, (L7_char8 *)serverAddr, (L7_uint32)L7_DNS_HOST_NAME_LEN_MAX);
    }
    return radiusIssueCmd(RADIUS_ACCT_IPADDR_DEL, &hostAddr, L7_NULL);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the port number of the radius server
*
* @param L7_uchar8 *serverAddr @b((input)) Pointer to Host Name or
*        IP Address of the radius server
* @param L7_IP_ADDRESS_TYPE_t type @b((input)) Address type DNS or IP address
* @param L7_uint32 *portNum @b((output)) Pointer to the Port Number for
*        the configured radius server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerPortNumGet(L7_uchar8 * serverAddr,
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 *portNum)
{
  radiusServerEntry_t *serverEntry;
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULL == serverAddr) || (L7_NULL == portNum))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            " RADIUS: radiusAcctServerPortNumGet(): Invalid parameters \n");

    return rc;
  }

  *portNum = L7_NULL;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }

    if (L7_NULL != serverEntry)
    {
      *portNum = serverEntry->radiusServerConfigUdpPort;

      rc = L7_SUCCESS;
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Set the port number of the accounting server
*
* @param serverAddr @b((input)) Host Name or IP address of the server
* @param portNum @b((input))    the port number to be set
* @param type @b((input))       Address type DNS or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctHostNameServerPortNumSet(L7_uchar8 * serverAddr,
                          L7_IP_ADDRESS_TYPE_t type, L7_uint32 portNum)
{
  radiusServerEntry_t * serverEntry;
  dnsHost_t hostAddr;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found. Return failure */
      return L7_FAILURE;
    }

    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
      hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
    }
    else
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
      osapiStrncpySafe((L7_char8 *)hostAddr.host.hostName, (L7_char8 *)serverAddr, (L7_uint32)L7_DNS_HOST_NAME_LEN_MAX);
    }
    return radiusIssueCmd(RADIUS_ACCT_PORT_SET, &hostAddr, (void *)&portNum);
  }
  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Set the shared secret being used between the radius client and
*          the accounting server
*
* @param serverAddr @b((input)) Server for which the secret is to be set
* @param sharedSecret @b((output)) the new string as the shared secret
* @param type @b((input))       Address type DNS or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctHostNameServerSharedSecretSet(L7_uchar8 *serverAddr,
                         L7_IP_ADDRESS_TYPE_t type, L7_uchar8 *sharedSecret)
{
  L7_RC_t rc = L7_FAILURE;
  radiusServerEntry_t *serverEntry;
  dnsHost_t hostAddr;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found */
      return L7_FAILURE;
    }
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
      hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
    }
    else
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
      osapiStrncpySafe((L7_char8 *)hostAddr.host.hostName, (L7_char8 *)serverAddr, (L7_uint32)L7_DNS_HOST_NAME_LEN_MAX);
    }
    rc = radiusIssueCmd(RADIUS_ACCT_SECRET_SET, &hostAddr, (void *)sharedSecret);

  }
  return rc;
}

/*********************************************************************
*
* @purpose Indicate if the shared secret is set for the accounting server
*
* @param serverAddr @b((input)) the HostName or IP address of the accounting server
* @param type @b((input))       Address type DNS or IP address
* @param pVal @b((output)) ptr to a boolean indicating if the secret is set
*
* @returns L7_FAILURE if the accounting server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctHostNameServerSharedSecretConfigured(L7_uchar8 *serverAddr,
                           L7_IP_ADDRESS_TYPE_t type, L7_BOOL *pVal )
{
  radiusServerEntry_t *serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found */
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_FAILURE;
    }

    *pVal = serverEntry->serverConfig.radiusServerIsSecretConfigured;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Returns the shared secret if set for the accounting server
*
* @param serverAddr @b((input)) the HostName or IP address of the accounting server
* @param type @b((input))       Address type DNS or IP address
* @param secret @b((output))    string to contain the secret if set
*
* @returns L7_FAILURE if the accounting server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctHostNameServerSharedSecretGet(L7_uchar8 *serverAddr,
                           L7_IP_ADDRESS_TYPE_t type, L7_uchar8 *secret )
{
  radiusServerEntry_t *serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found */
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_FAILURE;
    }

    if (serverEntry->serverConfig.radiusServerIsSecretConfigured == L7_TRUE)
    {
      strcpy(secret, serverEntry->serverConfig.radiusServerConfigSecret);
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose Get the IP address of the radius server corresponding to the
*          index input
*
* @param index @b((input)) Index of the Auth Server
* @param serverAddr @b((output)) IP address of the Auth Server
*
* @returns L7_SUCCESS - if there is a server corresponding to the specified index
* @returns L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerIPByIndexGet(L7_uint32 index,
                                 L7_uint32 *serverAddr)
{
  radiusServerEntry_t * serverEntry;
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
      {
        if (serverEntry->radiusServerStats_t.authStats.radiusAuthServerIndex == index)
        {
          *serverAddr = serverEntry->radiusServerConfigIpAddress;
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return L7_SUCCESS;
        }
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the Host Name or IP address of the radius server corresponding
*           to the index input
*
* @param index @b((input)) Index of the Auth Server
* @param serverAddr @b((output)) Host Name or IP address of the Auth Server
* @param type          @b((output))Address type DNS or IP address
*
* @returns L7_SUCCESS - if there is a server corresponding to the specified index
* @returns L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerIPHostNameByIndexGet(L7_uint32 index,
                                         L7_uchar8 *serverAddr,
                                         L7_IP_ADDRESS_TYPE_t *type)
{
  radiusServerEntry_t * serverEntry;
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
      {
        if (serverEntry->radiusServerStats_t.authStats.radiusAuthServerIndex == index)
        {
          *type = serverEntry->radiusServerDnsHostName.hostAddrType;
          if(serverEntry->radiusServerDnsHostName.hostAddrType ==
                                             L7_IP_ADDRESS_TYPE_DNS)
          {
            strcpy(serverAddr, serverEntry->radiusServerDnsHostName.host.hostName);
          }
          else
          {
            strcpy(serverAddr, osapiInet_ntoa(serverEntry->radiusServerConfigIpAddress));
          }
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return L7_SUCCESS;
        }
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the Host Name or IP address of the first configured
*          radius server
*
* @param firstServAddr @b((output)) Host name or IP address of the first
*                                   configured server
* @param type          @b((output))Address type DNS or IP address
*
* @returns L7_SUCCESS - if there is a server configured
* @returns L7_FAILURE - if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerFirstIPHostNameGet(L7_uchar8 *firstServAddr,
                                       L7_IP_ADDRESS_TYPE_t *type)
{
  radiusServerEntry_t * serverEntry;
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
      {
        *type = serverEntry->radiusServerDnsHostName.hostAddrType;
        if(serverEntry->radiusServerDnsHostName.hostAddrType ==
                                           L7_IP_ADDRESS_TYPE_DNS)
        {
          strcpy(firstServAddr,
                 serverEntry->radiusServerDnsHostName.host.hostName);
        }
        else
        {
          strcpy(firstServAddr,
                 osapiInet_ntoa(serverEntry->radiusServerConfigIpAddress));
        }

        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
        return L7_SUCCESS;
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the Hostname or IP address of the configured server next
*          in the list after the specified server
*
* @param serverAddr @b((input)) Host Name or IP Address of the "current" server
* @param pServAddr @b((output)) Host Name or IP address of the next configured server
* @param pType     @b((output))Address type DNS or IP address
*
* @returns L7_SUCCESS - if there is a "next-server" configured
* @returns L7_FAILURE - if no next server has been configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerNextIPHostNameGet(L7_uchar8 *serverAddr,
                                      L7_uchar8 *pServAddr,
                                      L7_IP_ADDRESS_TYPE_t *pType)
{
  radiusServerEntry_t * serverEntry;
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    /*
    ** Loop to find the specified server
    */
    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
      {
        if(serverEntry->radiusServerDnsHostName.hostAddrType ==
                                           L7_IP_ADDRESS_TYPE_DNS)
        {
          if (strcmp(serverEntry->radiusServerDnsHostName.host.hostName,
                     serverAddr) != 0)
          {
            serverEntry = serverEntry->nextEntry;
            continue;
          }
        }
        else
        {
          if(osapiInet_addr(serverAddr) != serverEntry->radiusServerConfigIpAddress)
          {
            serverEntry = serverEntry->nextEntry;
            continue;
          }
        }
        /*
        ** Loop to find the next server
        */
        while (serverEntry->nextEntry != L7_NULLPTR)
        {
          if (serverEntry->nextEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
          {
            *pType = serverEntry->nextEntry->radiusServerDnsHostName.hostAddrType;
            if(serverEntry->nextEntry->radiusServerDnsHostName.hostAddrType ==
                                           L7_IP_ADDRESS_TYPE_DNS)
            {
              strcpy(pServAddr,
                      serverEntry->nextEntry->radiusServerDnsHostName.host.hostName);
            }
            else
            {
              strcpy(pServAddr,
                      osapiInet_ntoa(serverEntry->nextEntry->radiusServerConfigIpAddress));
            }
            (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
            return L7_SUCCESS;
          }
          serverEntry = serverEntry->nextEntry;
        }
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Get the Hostname or IP address of the configured server next
*          in the list after the specified server
*
* @param L7_uchar8 *serverAddr @b((input)) Pointer to Host Name or
*        IP Address of the Accounting server
* @param L7_uchar8 *pServAddr @b((output)) Pointer to Host Name or
*        IP address of the next configured server
* @param L7_IP_ADDRESS_TYPE_t *pType @b((output)) pointer to the
*        Address type DNS or IP address
*
* @returns L7_SUCCESS - if there is a "next-server" configured
* @returns L7_FAILURE - if no next server has been configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerNextIPHostNameGet(L7_uchar8 *serverAddr,
                                      L7_uchar8 *pServAddr,
                                      L7_IP_ADDRESS_TYPE_t *pType)
{
  radiusServerEntry_t * serverEntry;
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    /*
    ** Loop to find the specified server
    */
    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT)
      {
        if(serverEntry->radiusServerDnsHostName.hostAddrType ==
                                           L7_IP_ADDRESS_TYPE_DNS)
        {
          if (strcmp(serverEntry->radiusServerDnsHostName.host.hostName,
                     serverAddr) != 0)
          {
            serverEntry = serverEntry->nextEntry;
            continue;
          }
        }
        else
        {
          if(osapiInet_addr(serverAddr) != serverEntry->radiusServerConfigIpAddress)
          {
            serverEntry = serverEntry->nextEntry;
            continue;
          }
        }
        /*
        ** Loop to find the next server
        */
        while (serverEntry->nextEntry != L7_NULLPTR)
        {
          if (serverEntry->nextEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT)
          {
            *pType = serverEntry->nextEntry->radiusServerDnsHostName.hostAddrType;
            if(serverEntry->nextEntry->radiusServerDnsHostName.hostAddrType ==
                                           L7_IP_ADDRESS_TYPE_DNS)
            {
              strcpy(pServAddr,
                      serverEntry->nextEntry->radiusServerDnsHostName.host.hostName);
            }
            else
            {
              strcpy(pServAddr,
                      osapiInet_ntoa(serverEntry->nextEntry->radiusServerConfigIpAddress));
            }
            (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
            return L7_SUCCESS;
          }
          serverEntry = serverEntry->nextEntry;
        }
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Add a radius server with a specific Host Name or IP address
*
* @param L7_uchar8 *serverAddr @b((input)) Pointer to Host Name or
*        IP Address of the Auth server
* @param L7_IP_ADDRESS_TYPE_t pType @b((input)) Address type DNS or IP address
* @param L7_char8 *serverName @b((input)) pointer to server Name.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerIPHostNameAdd(L7_uchar8 *serverAddr,
                     L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{
  L7_uint32 numOfServers;
  radiusServerEntry_t * serverEntry=L7_NULL;
  dnsHost_t hostAddr;
  L7_RC_t rc=L7_FAILURE;

  if(L7_NULLPTR == serverAddr || L7_NULLPTR == serverName ||
      strlen(serverName) < L7_RADIUS_SERVER_NAME_MIN ||
      strlen(serverName) > L7_RADIUS_SERVER_NAME_MAX )
  {
    return L7_FAILURE;
  }

  /* Error checks: verify if entry already exists and verify if
   * MAX servers have already been configured
   * */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry != L7_NULLPTR)
    {
      /* Entry already exists */
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_ALREADY_CONFIGURED ;
    }
    else
    {
      numOfServers = radiusAuthServerCount();
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);

      if (numOfServers >= L7_RADIUS_MAX_AUTH_SERVERS)
      {
        return L7_ERROR;
      }

      if(type == L7_IP_ADDRESS_TYPE_IPV4)
      {
        hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
        hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
      }
      else
      {
        hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
        osapiStrncpySafe((L7_char8 *)hostAddr.host.hostName, (L7_char8 *)serverAddr, (L7_uint32)L7_DNS_HOST_NAME_LEN_MAX);
      }
      rc = radiusIssueCmd(RADIUS_AUTH_IPADDR_ADD, &hostAddr, serverName);
      (void)radiusSemaTake(radiusServerDbSyncSema, L7_WAIT_FOREVER, __FILE__, __LINE__);
    }
    return rc;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Remove an auth server
*
* @param serverAddr @b((input)) the HostName or IP address of the
*                                server being removed
* @param type @b((input))       Address type DNS or IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAuthHostNameServerRemove(L7_uchar8 *serverAddr,
                                       L7_IP_ADDRESS_TYPE_t type)
{
  radiusServerEntry_t * serverEntry;
  dnsHost_t hostAddr;

  /* Error checks: verify if entry exists */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);

    if (serverEntry == L7_NULLPTR)
    {
      /* Entry does not exist */
      return L7_FAILURE;
    }

    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
      hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
    }
    else
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
      osapiStrncpySafe((L7_char8 *)hostAddr.host.hostName, (L7_char8 *)serverAddr, (L7_uint32)L7_DNS_HOST_NAME_LEN_MAX);
    }

    return radiusIssueCmd(RADIUS_AUTH_IPADDR_DEL, &hostAddr, L7_NULL);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Remove all RADIUS authentication and accounting servers
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerRemoveAll(void)
{
  return radiusIssueCmd(RADIUS_ALL_SERVER_DEL, L7_NULL, L7_NULLPTR);
}

/*********************************************************************
*
* @purpose Get the port number of the radius server
*
* @param serverAddr @b((input)) Host Name or IP Address of the radius server
* @param portNum @b((input))    the port number to be set
* @param type @b((input))       Address type DNS or IP address
* @param portNum @b((output)) Port Number for the configured radius server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerPortNumGet(L7_uchar8 * serverAddr,
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 *portNum)
{
  radiusServerEntry_t *serverEntry;
  L7_RC_t rc = L7_FAILURE;

  if(  L7_NULLPTR == serverAddr || L7_NULLPTR == portNum ||
       ((L7_IP_ADDRESS_TYPE_IPV4 != type) && (L7_IP_ADDRESS_TYPE_DNS != type))
    )
  {
     return L7_FAILURE;
  }

  *portNum = L7_NULL;
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
        if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr),
                     RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr,
                     RADIUS_SERVER_TYPE_AUTH);
    }


    if (serverEntry != L7_NULLPTR)
    {
      *portNum = serverEntry->radiusServerConfigUdpPort;
      rc = L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return rc;

}

/*********************************************************************
*
* @purpose Set the radius server port number
*
* @param serverAddr @b((input)) the HostName or IP address of the radius server
* @param portNum @b((input))    the port number to be set
* @param portNum @b((input)) the new value for the port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerPortNumSet(L7_uchar8 *serverAddr,
                L7_IP_ADDRESS_TYPE_t type, L7_uint32 portNum)
{
  radiusServerEntry_t *serverEntry;
  dnsHost_t hostAddr;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found */
      return L7_FAILURE;
    }
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
      hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
    }
    else
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
      osapiStrncpySafe((L7_char8 *)hostAddr.host.hostName, (L7_char8 *)serverAddr, (L7_uint32)L7_DNS_HOST_NAME_LEN_MAX);
    }
    return radiusIssueCmd(RADIUS_AUTH_PORT_SET, &hostAddr, (void *)&portNum);
  }
  return L7_FAILURE;

}

/*********************************************************************
*
* @purpose Set the shared secret being used between the client and
*          the server
*
* @param hostAddr     @b((input)) server to modify (ALL_RADIUS_SERVERS=>global)
* @param sharedSecret @b((input)) the new value for the shared secret
* @param paramStatus  @b((input)) the value is global or local
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerSharedSecretSet(dnsHost_t                           *hostAddr,
                                            L7_uchar8                           *sharedSecret,
                                            L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  radiusAuthServerInfo_t  serverInfo;
  radiusServerEntry_t *serverEntry = L7_NULLPTR;

  if (strlen(sharedSecret) > L7_RADIUS_MAX_SECRET)
  {
    return L7_FAILURE;
  }

  if(!((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
  {  /* Verify if the server entry exists */
    if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
    {
      if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
      {
        serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
      }
      else
      {
        serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
      }
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      if (serverEntry == L7_NULLPTR)
      {
        /* No entry with serverIPAddr was found */
        return L7_FAILURE;
      }
    }
  }

  serverInfo.localGlobal = paramStatus;
  osapiStrncpySafe((L7_char8 *)serverInfo.val.str, (L7_char8 *)sharedSecret, (L7_uint32)L7_RADIUS_MAX_SECRET+1);

  return radiusIssueCmd(RADIUS_AUTH_SECRET_SET, hostAddr, (void *)&serverInfo);
}

/*********************************************************************
*
* @purpose Indicate if the shared secret is set for the auth server
*
* @param hostAddr  @b((input))  server IP address
* @param pVal      @b((output)) ptr to a boolean indicating if the secret is set
*
* @returns L7_FAILURE if the auth server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerSharedSecretConfigured(dnsHost_t *hostAddr,
                                                   L7_BOOL   *pVal )
{
  radiusServerEntry_t *serverEntry;
  L7_RC_t rc = L7_FAILURE;

  if(  L7_NULLPTR == pVal ||
      (( L7_IP_ADDRESS_TYPE_IPV4 != hostAddr->hostAddrType) && (L7_IP_ADDRESS_TYPE_DNS != hostAddr->hostAddrType))
    )
  {
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry != L7_NULLPTR)
    {
      *pVal = serverEntry->serverConfig.radiusServerIsSecretConfigured;
      rc = L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return rc;
}

/*********************************************************************
*
* @purpose Returns the shared secret if set for the auth server
*
* @param hostAddr     @b((input))  server IP address (ALL_RADIUS_SERVERS=>global)
* @param secret       @b((output)) string to contain the secret if set
* @params paramStatus @b((output)) if the parameter for a specific server is
*                                  global/local.
*
* @returns L7_FAILURE if the auth server is not configured
* @returns L7_SUCCESS otherwise
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerSharedSecretGet(dnsHost_t                            *hostAddr,
                                            L7_uchar8                            *secret,
                                            L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus)
{
  radiusServerEntry_t *serverEntry = L7_NULLPTR;
  radiusClient_t *radiusClient;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    radiusClient = radiusClientGet();

    if (radiusClient != L7_NULLPTR)
    {
      if(((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS)))
      {  /* Global value is fetched */
        osapiStrncpySafe(secret, radiusClient->radiusServerSecret, (L7_RADIUS_MAX_SECRET+1));
      }
      else
      {  /* Server specific value is fetched */
        if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
        }
        else
        {
          serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
        }
        if (serverEntry != L7_NULLPTR)
        {
          if ( ( L7_FALSE == serverEntry->serverConfig.radiusServerIsSecretConfigured ) ||
               ( 0 == ( serverEntry->serverConfig.localConfigMask & L7_RADIUS_SERVER_CONFIG_SECRET ) ) )
          {
            osapiStrncpySafe(secret, radiusClient->radiusServerSecret, (L7_RADIUS_MAX_SECRET+1));
            *paramStatus = L7_RADIUSGLOBAL;
          }
          else
          {
            osapiStrncpySafe( secret, serverEntry->serverConfig.radiusServerConfigSecret,
                             (L7_RADIUS_MAX_SECRET+1));
            *paramStatus = L7_RADIUSLOCAL;
          }
        }
        else
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "radiusHostNameServerSharedSecretGet(): Failed to find server");
          return L7_FAILURE;
        }
      }
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the server entry type (PRIMARY or BACKUP)
*
* @param serverAddr @b((input)) the HostName or IP address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param entryType @b((output)) the server entry type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerEntryTypeGet(L7_uchar8 *serverAddr,
         L7_IP_ADDRESS_TYPE_t type, L7_uint32 *entryType)
{
  radiusServerEntry_t *serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      /* No entry with serverAddr was found. */
      *entryType = L7_NULL;
      return L7_FAILURE;
    }

    *entryType = serverEntry->serverConfig.radiusServerConfigServerEntryType;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the server entry type to PRIMARY or BACKUP
*
* @param serverAddr @b((input)) the HostName or IP address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param entryType @b((input)) the server entry type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerEntryTypeSet(L7_uchar8 *serverAddr,
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 entryType)
{
  radiusServerEntry_t *serverEntry;
  dnsHost_t hostAddr;

  if ((entryType != L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY) &&
     (entryType != L7_RADIUS_SERVER_ENTRY_TYPE_SECONDARY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS(): radiusHostNameServerEntryTypeSet() %s : Invalid parameters\n",__FUNCTION__);
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);

    if (serverEntry == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS(): radiusHostNameServerEntryTypeSet() : Specified Server Not found\n");
      return L7_FAILURE;
    }
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
      hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
    }
    else
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
      osapiStrncpySafe((L7_char8 *)hostAddr.host.hostName, (L7_char8 *)serverAddr, (L7_uint32)L7_DNS_HOST_NAME_LEN_MAX);
    }

    return radiusIssueCmd(RADIUS_SERVER_TYPE_SET, &hostAddr, (void *)&entryType);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Enable/Disable the inclusion of a Message-Authenticator attribute
*          in each Access-Request packet to a specified RADIUS server.
*
* @param serverAddr @b((input)) the HostName or IP address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param mode @b((input)) L7_ENABLE/L7_DISABLE to include the attribute or not
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerIncMsgAuthModeSet(L7_uchar8 *serverAddr,
             L7_IP_ADDRESS_TYPE_t type, L7_uint32 mode)
{
  radiusServerEntry_t *serverEntry;
  dnsHost_t hostAddr;

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
    /* Invalid input */
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);

    if (serverEntry == L7_NULLPTR)
    {
      /* Server not found */
      return L7_FAILURE;
    }
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
      hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
    }
    else
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
      osapiStrncpySafe((L7_char8 *)hostAddr.host.hostName, (L7_char8 *)serverAddr, (L7_uint32)L7_DNS_HOST_NAME_LEN_MAX);
    }

    return radiusIssueCmd(RADIUS_SERVER_INC_MSG_AUTH_MODE_SET, &hostAddr, (void *)&mode);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the value of a Message-Authenticator attribute mode
*          for the specified RADIUS server.
*
* @param serverAddr @b((input)) the IP address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param mode @b((output)) value of the mode, L7_ENABLE/L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerIncMsgAuthModeGet(L7_uchar8 *serverAddr,
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 *mode)
{
  radiusServerEntry_t *serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }


    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      /* Server not found */
      *mode = L7_NULL;
      return L7_FAILURE;
    }

    *mode = serverEntry->serverConfig.incMsgAuthMode;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;

}

/*********************************************************************
*
* @purpose Retrieve the IP address of the primary server
*
* @param L7_uint32 index@b((input)) Index to the array of server names.
* @param L7_uint32 *serverAddr @b((output)) Pointer to the IP address
*        of the radius server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusPrimaryServerGet(L7_uint32 index, L7_uint32 *pIpAddr)
{
  L7_RC_t rc = L7_FAILURE;
  dnsHost_t hostAddr;

  memset(&hostAddr, L7_NULL, sizeof(hostAddr));

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    rc = radiusPrimaryServerAddressGet(index,&hostAddr);
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    *pIpAddr = hostAddr.host.ipAddr;
    return rc;
  }
  *pIpAddr = L7_NULL;
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Retrieve the HostName if configured and if no Hostname
*          retrieve the IP address of the current active auth server
*
* @param L7_uint32 serverNameIndex@b((input)) Index to the array of
*        server names.
* @param L7_uchar8 *serverAddr @b((output)) Pointer to the HostName or
*        IP address of the radius server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusCurrentAuthHostNameServerGet(L7_uint32 serverNameIndex, L7_RADIUS_SERVER_USAGE_TYPE_t usageType,
                     L7_uchar8 *pServerAddr)
{
  radiusServerEntry_t *currentServerEntry=L7_NULL;
  L7_RC_t rc = L7_FAILURE;

  if(L7_NULLPTR == pServerAddr)
  {
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(radiusServerAuthCurrentEntryGet(serverNameIndex, usageType, &currentServerEntry)== L7_SUCCESS )
    {
      if(L7_NULLPTR != currentServerEntry)
      {
        if(currentServerEntry->radiusServerDnsHostName.hostAddrType ==
                         L7_IP_ADDRESS_TYPE_IPV4)
        {
          strcpy(pServerAddr, osapiInet_ntoa(currentServerEntry->radiusServerConfigIpAddress));
        }
        else
        {
          strcpy(pServerAddr,
                       currentServerEntry->radiusServerDnsHostName.host.hostName);
        }
        rc = L7_SUCCESS;
      }
      else
      {
        *pServerAddr = L7_NULL;
      }
    }
  }
  (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the round trip time
*
* @param hostAddr @b((input)) Host Name or IP Address of the radius server
*
* @returns the round trip time value in hundredths of a second
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatRTTGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32  rtt = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    rtt = serverEntry->radiusServerStats_t.authStats.radiusAuthClientRoundTripTime;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return rtt;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Request packets sent
*          to this server
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets sent
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatAccessReqGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 ar = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    ar = serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRequests;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return ar;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Request packets
*          retransmitted to this server
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets retransmitted
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatAccessRetransGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 ar = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    ar = serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRetransmissions;

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return ar;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Accept packets
*          received from this server
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatAccessAcceptGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 aa = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    aa = serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessAccepts;

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return aa;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Reject packets
*          received from this server
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatAccessRejectGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 ar = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    ar = serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRejects;

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return ar;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Challenge packets
*          received from this server
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatAccessChallengeGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 ac = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    ac = serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessChallenges;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return ac;
}

/*********************************************************************
*
* @purpose Retrieve the number of malformed RADIUS Access-Response packets
*          received from this server. Malformed packets include packets
*          with an invalid length. Bad authenticators or signature attributes
*          or unknown types are not included
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatMalformedAccessResponseGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 mar = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    mar = serverEntry->radiusServerStats_t.authStats.radiusAuthClientMalformedAccessResponses;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return mar;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Response packets
*          containing invalid authenticators or signature attributes received
*          from this server.
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatBadAuthGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 ba = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    ba = serverEntry->radiusServerStats_t.authStats.radiusAuthClientBadAuthenticators;

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return ba;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Request packets
*          destined for this server that have not yet timed out or received
*          a response from this server
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets pending to be sent
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatPendingReqGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 pr = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    pr = serverEntry->radiusServerStats_t.authStats.radiusAuthClientPendingRequests;

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return pr;
}

/*********************************************************************
*
* @purpose Retrieve the number of authentication time-outs to this server
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of time-outs
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatTimeoutsGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 st = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    st = serverEntry->radiusServerStats_t.authStats.radiusAuthClientTimeouts;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return st;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS packets of unknown type which
*          were received from this server on the authentication port
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatUnknownTypeGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 ut = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    ut = serverEntry->radiusServerStats_t.authStats.radiusAuthClientUnknownTypes;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return ut;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS packets received from this server on
*          the authentication port that were dropped for some other reason.
*
* @param hostAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets dropped
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatPktsDroppedGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 pd = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    pd = serverEntry->radiusServerStats_t.authStats.radiusAuthClientPacketsDropped;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return pd;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Response packets
*          received from unknown addresses.
*
* @returns the number of packets dropped
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatInvalidAddressesGet(void)
{
  return radiusGlobalData.radiusAuthClientInvalidServerAddresses;
}

/*********************************************************************
*
* @purpose Get the round trip time
*
* @param hostAddr @b((input)) HostName or IP Address of the radius accounting server
*
* @returns the round trip time value in hundredths of a second
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatRTTGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 rtt = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }
    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    rtt = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRoundTripTime;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return rtt;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Accounting-Request packets sent
*          to this server (without including retransmissions)
*
* @param hostAddr @b((input)) HostName or IP Address of the accounting server
*
* @returns the number of packets sent
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatReqGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 sr = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    sr = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRequests;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return sr;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Accounting-Request packets
*          retransmitted to this server
*
* @param hostAddr @b((input)) HostName or IP Address of the accounting server
*
* @returns the number of packets retransmitted
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatRetransGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 rt = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    rt = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRetransmissions;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return rt;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Accounting-Response packets
*          received from this server
*
* @param hostAddr @b((input)) HostName or IP Address of the accounting server
*
* @returns the number of packets retransmitted
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatResponseGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 sr = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    sr = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientResponses;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return sr;
}

/*********************************************************************
*
* @purpose Retrieve the number of malformed RADIUS Accounting-Response packets
*          received from this server. Malformed packets include packets
*          with an invalid length. Bad authenticators or signature attributes
*          or unknown types are not included
*
* @param hostAddr @b((input)) HostName or IP Address of the accounting server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatMalformedResponseGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 mr = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    mr = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientMalformedResponses;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return mr;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Accounting-Response packets
*          containing invalid authenticators or signature attributes received
*          from this server.
*
* @param hostAddr @b((input)) HostName or IP Address of the accounting server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatBadAuthGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 ba = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    ba = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientBadAuthenticators;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return ba;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Accounting-Request packets
*          destined for this server that have not yet timed out or received
*          a response from this server
*
* @param hostAddr @b((input)) HostName or IP Address of the radius accounting server
*
* @returns the number of packets pending to be sent
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatPendingReqGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 pr = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    pr = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPendingRequests;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return pr;
}

/*********************************************************************
*
* @purpose Retrieve the number of authentication time-outs to this server
*
* @param hostAddr @b((input)) HostName or IP Address of the accounting server
*
* @returns the number of time-outs
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatTimeoutsGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 st = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    st = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientTimeouts;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return st;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS packets of unknown type which
*          were received from this server on the accounting port
*
* @param hostAddr @b((input)) HostName or IP Address of the accounting server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatUnknownTypeGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 ut = L7_NULL;

  /* Retrieve the server entry corresponding to this IP address */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    ut = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientUnknownTypes;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return ut;
}

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS packets received from this server
*          on the accounting port that were dropped for some other reason.
*
* @param hostAddr @b((input)) HostName or IP Address of the accounting server
*
* @returns the number of packets dropped
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatPktsDroppedGet(dnsHost_t *hostAddr)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 pd = L7_NULL;

  /*
  ** Retrieve the server entry corresponding to this IP address
  */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
       serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
     }

    if (serverEntry == L7_NULLPTR)
    {
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_NULL;
    }

    pd = serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPacketsDropped;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return pd;
}

/*********************************************************************
*
* @purpose  Retrieve the number of RADIUS Accounting response packets
*           received from unknown addresses.
*
* @returns the number of packets dropped
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatInvalidAddressesGet(void)
{
  return radiusGlobalData.radiusAcctClientInvalidServerAddresses;
}

/*********************************************************************
*
* @purpose Get the NAS-Identifier attribute as is used by
*          the RADIUS client code.
*
* @params nasId @b((input/output)) location to write the NAS-Identifier
* @params nasIdSize @b((input/output)) location to write the
*         NAS-Identifier size
*
* @returns void
*
* @comments nasId must accomadate a string equal or greater than
*           L7_RADIUS_NAS_IDENTIFIER_SIZE + 1. nasIdSize will NOT include
*           the null termination character.
*
* @end
*
*********************************************************************/
void radiusNASIdentifierGet(L7_uchar8 *nasId,
                            L7_uint32 *nasIdSize)
{
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  /* The NAS-Identifier is the Base Mac Address of the switch. */
  simGetSystemMac(mac);

  sprintf((L7_char8 *)nasId, "%02X-%02X-%02X-%02X-%02X-%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  *nasIdSize = L7_RADIUS_NAS_IDENTIFIER_SIZE;

  return;
}

/*********************************************************************
*
* @purpose Clear statistics for all the radius servers
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusStatsClear(void)
{
  radiusServerEntry_t *serverEntry;

  /*
  ** Clear the global stats
  */
  radiusGlobalData.radiusAuthClientInvalidServerAddresses = L7_NULL;
  radiusGlobalData.radiusAcctClientInvalidServerAddresses = L7_NULL;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;
    while (serverEntry != L7_NULLPTR)
    {
      /*
      ** Clear the Auth stats
      */
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
      {
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessAccepts = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthServerAddress = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientServerPortNumber = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRequests = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessAccepts = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRejects = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessChallenges = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientPendingRequests = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientBadAuthenticators = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientMalformedAccessResponses = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientUnknownTypes = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientPacketsDropped = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientRoundTripTime = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientTimeouts = L7_NULL;
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRetransmissions = L7_NULL;
      }
      /*
      ** Clear the Acct stats
      */
      else if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT)
      {
        serverEntry->radiusServerStats_t.acctStats.radiusAcctServerAddress = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientServerPortNumber = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRequests = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPendingRequests = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientMalformedResponses = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientUnknownTypes = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRoundTripTime = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientResponses = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientTimeouts = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRetransmissions = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientBadAuthenticators = L7_NULL;
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPacketsDropped = L7_NULL;
      }

      serverEntry = serverEntry->nextEntry;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the value of the RADIUS NAS-IP Attributes
*
* @param    mode    @b{(input)} Radius NAS-IP Mode.
* @param    ipAddr  @b{(input)} Radius NAS-IP address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAttribute4Set(L7_BOOL mode, L7_uint32 ipAddr)
{
  dnsHost_t hostAddr;
  if ((mode == L7_TRUE) || (mode == L7_FALSE))
  {
    hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
    hostAddr.host.ipAddr = ipAddr;
    return radiusIssueCmd(RADIUS_ATTRIBUTE_4_SET, &hostAddr, (void *)&mode);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the value of the RADIUS NAS-IP Attributes
*
* @param    mode    @b{(input)} Radius NAS-IP Mode.
* @param    ipAddr  @b{(input)} Radius NAS-IP address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAttribute4Get(L7_BOOL *mode, L7_uint32 *ipAddr)
{
  radiusClient_t *radiusClient;
  L7_RC_t        rc= L7_FAILURE;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    radiusClient = radiusClientGet();
    if (radiusClient != L7_NULLPTR)
    {
      *mode = radiusClient->nasIpMode;
      *ipAddr = radiusClient->nasIpAddress;
      rc = L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return rc;
}
/*********************************************************************
*
* @purpose Encode the text using secret key and text into digest
*
* @param    text      @b{(input)} Pointer to plain text.
* @param    text_len  @b{(input)} Plain text len.
* @param    key       @b{(input)} Pointer to key text.
* @param    key_len   @b{(input)} Plain key len.
* @param    digest    @b{(output)} Pointer to digest after encoding plain text.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
void radiusHMACMD5(L7_uchar8 *text,
                   L7_uint32 text_len,
                   L7_uchar8 *key,
                   L7_uint32 key_len,
                   L7_uchar8 *digest)
{
  L7_hmac_md5 (text, text_len, key, key_len, digest);
  return;
}

/*****************************************************************************
*
* @purpose Decrypt the MS-MPPE-Send-Key/Recv-Key key from response attributes.
*
* @param encrKey    @b((input))  Pointer to buffer containing ecnrypted key
* @param encrkeyLen @b((input))  Encrypted key len
* @param reqAuth    @b((input))  Pointer to buffer containing request authenitcator
* @param secret     @b((input))  Secret string used to contact radius server
* @param secretLen  @b((input))  Secret string len
* @param decrKey    @b((output)) Pointer to buffer to store decrypted key
* @param decrkeyLen @b((output)) Pointer to store decrypted key len
*
* @returns
*
* @comments
*
* @end
*
******************************************************************************/
L7_RC_t radiusDecryptMSKey(L7_uchar8 *encrKey,
                           L7_uchar8 encrKeyLen,
                           L7_uchar8 *reqAuth,
                           L7_uchar8 *secret,
                           L7_uchar8 secretLen,
                           L7_uchar8 *decrKey,
                           L7_uchar8 *decrKeyLen)
{
    L7_uchar8       plainKey[RADIUS_MS_KEY_SIZE];
    L7_uchar8       *pos  = L7_NULLPTR;
    L7_uchar8       *ppos = L7_NULLPTR;
    L7_uchar8       left = 0;
    L7_uchar8       plen = 0;
    L7_uchar8       hash[RADIUS_AUTHENTICATOR_LENGTH];
    L7_MD5_CTX_t    context;
    L7_uint32       i;
    L7_uint32       first = 1;

    /*
    ** key: 16-bit salt followed by encrypted key info
    */

    if (encrKeyLen < RADIUS_MS_KEY_SALT_LEN + RADIUS_AUTHENTICATOR_LENGTH)
        return L7_FAILURE;

    /*
    ** Encrypted key packet dump
    */
    radiusDebugPktDump(RD_LEVEL_CLUSTER_RX_PKT, encrKey, encrKeyLen);

    pos =  encrKey + RADIUS_MS_KEY_SALT_LEN;
    left = encrKeyLen - RADIUS_MS_KEY_SALT_LEN;

    if (left % RADIUS_AUTHENTICATOR_LENGTH) {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: Invalid ms key len\n");
        return L7_FAILURE;
    }

    plen = left;
    ppos = plainKey;

    while (left > 0)
    {
        /*
        ** b(1) = MD5(Secret + Request-Authenticator + Salt)
        *  b(i) = MD5(Secret + c(i - 1)) for i > 1
        */

        l7_md5_init(&context);
        l7_md5_update(&context, secret, secretLen);
        if (first)
        {
            l7_md5_update(&context, reqAuth, RADIUS_AUTHENTICATOR_LENGTH);
            l7_md5_update(&context, encrKey, RADIUS_MS_KEY_SALT_LEN); /* Salt */
            first = 0;
        }
        else
        {
            l7_md5_update(&context, pos - RADIUS_AUTHENTICATOR_LENGTH, RADIUS_AUTHENTICATOR_LENGTH);
        }

        l7_md5_final(hash, &context);

        for (i = 0; i < RADIUS_AUTHENTICATOR_LENGTH; i++)
        {
           *ppos++ = *pos++ ^ hash[i];
        }
        left -= RADIUS_AUTHENTICATOR_LENGTH;
    }

    if (plainKey[0] > plen - 1)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: Failed to decrypt MPPE key\n");
        return L7_FAILURE;
    }

    memcpy(decrKey, plainKey + 1, plainKey[0]);

    if (decrKeyLen != L7_NULL)
    {
       *decrKeyLen = plainKey[0];

      if (decrKey != L7_NULL)
      {
    /*
    ** Decrypted key packet dump
    */
    radiusDebugPktDump(RD_LEVEL_CLUSTER_RX_PKT, decrKey, *(decrKeyLen));
      }
    }

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Encrypt the MS-MPPE key
*
* @param    input     @b{(input)} Pointer to plain text.
* @param    inlen     @b{(input)} Plain text len.
* @param    secret    @b{(input)} Pointer to secret string.
* @param    reqAuth   @b{(input)} Pointer to request authenticator.
* @param    output    @b{(input)} Pointer to store encrypted key.
* @param    outlen    @b{(input)} Pointer to store encrypted key len.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
void radiusEncryptMSKey(L7_uchar8 *input,
                        L7_int32  inlen,
                        L7_uchar8 *secret,
                        L7_uchar8 *reqAuth,
                        L7_uchar8 *output,
                        L7_int32  *outlen)
{
  L7_MD5_CTX_t context, old;
  L7_uchar8    digest[RADIUS_AUTHENTICATOR_LENGTH];
  L7_uchar8    msKey[254 + RADIUS_AUTHENTICATOR_LENGTH];
  L7_uint32    i;
  L7_uint32    n;
  L7_uint32    len;
  static L7_uint32 salt_offset =0;

  /*
  ** Length of the encrypted data is password length plus
  *  one byte for the length of the password.
  */
  len = inlen + 1;
  if ((len & 0x0f) != 0)
  {
    len += 0x0f;
    len &= ~0x0f;
  }

  *outlen = len + RADIUS_MS_KEY_SALT_LEN;   /* account for the salt */

  /*
  **  Copy the password over.
  */
  memcpy(msKey + 3, input, inlen);
  memset(msKey + 3 + inlen, 0, sizeof(msKey) - 3 - inlen);

  /*
   *      Generate salt.  As per RFC: The high bit of salt[0] must be set,
   *      each salt in a packet should be unique, and they should be random
   *      Set the high bit, add in a counter, and then add in some CSPRNG
   *      data.  should be OK..
   */
  msKey[0] = (0x80 | ( ((salt_offset++) & (0x0f)) << 3) |
             (((L7_uint32)L7_Random()) & 0x07));
  msKey[1] = L7_Random();
  msKey[2] = inlen;      /* length of the password string */

  l7_md5_init(&context);
  l7_md5_update(&context, secret, strlen(secret));
  old = context;

  l7_md5_update(&context, reqAuth, RADIUS_AUTHENTICATOR_LENGTH);
  l7_md5_update(&context, &msKey[0], RADIUS_MS_KEY_SALT_LEN);

  for (n = 0; n < len; n += RADIUS_AUTHENTICATOR_LENGTH)
  {
    if (n > 0)
    {
      context = old;
      l7_md5_update(&context,msKey + RADIUS_MS_KEY_SALT_LEN + n - RADIUS_AUTHENTICATOR_LENGTH,
                   RADIUS_AUTHENTICATOR_LENGTH);
    }

    l7_md5_final(digest, &context);
    for (i = 0; i < RADIUS_AUTHENTICATOR_LENGTH; i++)
    {
      msKey[i + RADIUS_MS_KEY_SALT_LEN + n] ^= digest[i];
    }
  }
  memcpy(output, msKey, (*outlen));

  /*
  ** Encrypted key hex dump
  */
  radiusDebugPktDump(RD_LEVEL_CLUSTER_TX_PKT, output, *(outlen));

}

/*********************************************************************
*
* @purpose Verify the named server exists.
*
* @param  L7_char8 *serverName @b((input)) Pointer to the Name of the
*         RADIUS auth server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t radiusNamedServerGet(L7_char8 *serverName)
{
  L7_uint32 index;/*dummy*/
  L7_RC_t rc = L7_FAILURE;

  if(L7_NULLPTR == serverName)
  {
    return L7_FAILURE;
  }

  if (osapiSemaTake(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    rc = radiusServerNameEntryIndexGet(serverName, &index);
  }
 (void)osapiSemaGive(radiusTaskSyncSema);
  return rc;
}

/*********************************************************************
*
* @purpose Verify the named server exists.
*
* @param   *serverName @b((input)) Name of the RADIUS auth server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t radiusNamedAcctServerGet(L7_char8 *serverName)
{
  L7_uint32 index;/*dummy*/
  L7_RC_t rc = L7_FAILURE;

  if(L7_NULLPTR == serverName)
  {
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    rc = radiusAcctServerNameEntryIndexGet(serverName, &index);
  }
 (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  return rc;
}

/*********************************************************************
*
* @purpose Set the serverName attribute to the Auth. server.
*
* @param serverAddr @b((input)) the HostName or IP address to be set
* @param L7_IP_ADDRESS_TYPE_t type @b((input)) Address type either ipv4 or dns
* @param L7_char8 *serverName  @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerAuthNameSet(L7_uchar8 *serverAddr,
                     L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{

  radiusServerEntry_t *serverEntry;
  dnsHost_t hostAddr;
  L7_uint32 rc= L7_FAILURE;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];

  if(L7_NULLPTR == serverAddr || L7_NULLPTR == serverName ||
       ( strlen(serverName) < L7_RADIUS_SERVER_NAME_MIN ) ||
       ( strlen(serverName) > L7_RADIUS_SERVER_NAME_MAX ) ||
       ( L7_IP_ADDRESS_TYPE_IPV4 != type && L7_IP_ADDRESS_TYPE_DNS != type )
    )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusServerAuthNameSet(): Invalid Parameters \n");
    return L7_FAILURE;
  }

  bzero(name,sizeof(name));


  /* Error checks: verify if entry already exists and verify if
   * MAX servers have already been configured
   * */
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr),
                     RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS: radiusServerAuthNameSet(): The specified server: %s of \
                     type: %d desn't exist\n",serverAddr,type);
      /* Entry doesn't exist */
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_FAILURE;
    }
    if(radiusServerNameEntryGet(serverEntry->serverNameIndex,name) == L7_SUCCESS &&
        osapiStrncmp(name,serverName,sizeof(name)) == 0  )
    {
      rc = L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);

    if(L7_SUCCESS != rc )
    {
      if(type == L7_IP_ADDRESS_TYPE_IPV4)
      {
        hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
        hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
      }
      else
      {
        hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
        osapiStrncpySafe(hostAddr.host.hostName,
                     serverAddr,L7_DNS_HOST_NAME_LEN_MAX);
      }
      rc = radiusIssueCmd(RADIUS_AUTH_SERVER_NAME_SET, &hostAddr, serverName);
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose Set the serverName attribute to the acct. server.
*
* @param serverAddr @b((input)) the HostName or IP address to be set
* @param L7_IP_ADDRESS_TYPE_t type @b((input)) Address type either ipv4 or dns
* @param L7_char8 *serverName  @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerAcctNameSet(L7_uchar8 *serverAddr,
                     L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{


  radiusServerEntry_t *serverEntry, *currentEntry = L7_NULL;
  dnsHost_t hostAddr;
  L7_uint32 index = 0;
  L7_RC_t rc = L7_FAILURE;

  if(L7_NULLPTR == serverAddr || L7_NULLPTR == serverName ||
       ( strlen(serverName) < L7_RADIUS_SERVER_NAME_MIN ) ||
       ( strlen(serverName) > L7_RADIUS_SERVER_NAME_MAX ) ||
       ( L7_IP_ADDRESS_TYPE_IPV4 != type && L7_IP_ADDRESS_TYPE_DNS != type )
    )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusServerAcctNameSet(): Invalid Parameters \n");
    return L7_FAILURE;
  }


  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }

    if (serverEntry == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS: radiusServerAuthNameSet(): The specified server: %s of \
                     type: %d desn't exist\n",serverAddr,type);
      /* Entry doesn't exist */
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_FAILURE;
    }
    rc = radiusAcctServerNameEntryIndexGet(serverName,&index);
    if(L7_SUCCESS == rc)
    {
      radiusServerAcctCurrentEntryGet(index,&currentEntry);
      if( L7_NULL != currentEntry &&
          serverEntry != currentEntry)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: radiusServerAcctNameSet(): Name already in use.\n");
        rc = L7_ALREADY_CONFIGURED;
      }
      else
      {
        rc = L7_SUCCESS;
      }
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    if(L7_FAILURE == rc )/* No server using this name, so apply to this server*/
    {
      if(type == L7_IP_ADDRESS_TYPE_IPV4)
      {
        hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
        hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
      }
      else
      {
        hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
        osapiStrncpySafe(hostAddr.host.hostName,
                     serverAddr,L7_DNS_HOST_NAME_LEN_MAX);
      }
      rc = radiusIssueCmd(RADIUS_ACCT_SERVER_NAME_SET, &hostAddr, serverName);
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose Get the serverNameIndex value of the specified RADIUS server.
*          This index points to an element in serverNameArray which
*          contains the alias name assigned to the specified server.
*
* @param serverAddr @b((input)) the IP address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param mode @b((output)) value of the mode, L7_ENABLE/L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This index value of the Auth. & Acct. servers points
*           to respective serverName arrays. And is different from
*           indexing given for each server entry in server entries List.
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerNameIndexGet(L7_uchar8 *serverAddr,
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 *index)
{

  radiusServerEntry_t *serverEntry;
  L7_RC_t rc = L7_FAILURE;

  if(L7_NULLPTR == serverAddr || L7_NULLPTR == index)
  {
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      /* Server not found */
      *index = L7_NULL;
    }
    else
    {
      *index = serverEntry->serverNameIndex;
      rc = L7_SUCCESS;
    }
  }
  (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  return rc;
}

/*********************************************************************
*
* @purpose Get the indexed server's name.
*
* @param serverEntry @b((input)) RADIUS server
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This can also be used to verify whether a specified serverName
*           entry exists.
*           Range for index of nameArray[], outside the API is 1-32.
*           And inside 0-31.
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerHostNameGet(L7_uchar8 *serverAddr,
               L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{

  radiusServerEntry_t *serverEntry;
  L7_RC_t rc = L7_FAILURE;

  if(L7_NULLPTR == serverAddr || L7_NULLPTR == serverName ||
     (L7_IP_ADDRESS_TYPE_IPV4 != type && L7_IP_ADDRESS_TYPE_DNS != type)
    )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusServerHostNameGet() Invalid Parameters\n");
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    if (L7_NULLPTR == serverEntry  )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS:radiusServerHostNameGet(): Not found the server entry for the host: %s\n", serverAddr);
    }
    else if( radiusServerNameEntryGet(serverEntry->serverNameIndex,
            serverName) == L7_SUCCESS )
    {
      rc = L7_SUCCESS;
    }

  }
  (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  return rc;
}

/*********************************************************************
*
* @purpose Get the Alias Name for an accounting server.
*
* @param serverEntry @b((input)) RADIUS server
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This can also be used to verify whether a specified serverName
*           entry exists.
*           Range for index of nameArray[], outside the API is 1-32.
*           And inside 0-31.
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerAcctHostNameGet(L7_uchar8 *serverAddr,
               L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName)
{

  radiusServerEntry_t *serverEntry;
  L7_RC_t rc = L7_FAILURE;

  if(L7_NULLPTR == serverAddr || L7_NULLPTR == serverName ||
     (L7_IP_ADDRESS_TYPE_IPV4 != type && L7_IP_ADDRESS_TYPE_DNS != type)
    )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s Invalid Parameters\n",__FUNCTION__);
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }

    if (L7_NULLPTR == serverEntry  )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS:%s Not found the server entry for the host: %s\n", __FUNCTION__,serverAddr);
    }
    else if( radiusAcctServerNameEntryGet(serverEntry->serverNameIndex,
            serverName) == L7_SUCCESS )
    {
      rc = L7_SUCCESS;
    }

  }
  (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  return rc;
}

/*********************************************************************
*
* @purpose Get the Server Name and Address of the next named auth server
*          from the servers' list.  The address could be DNS Host Name or IP
*          address.
*
* @param L7_char8 *name      @b((input)) Name of the configured Auth Server.
* @param L7_char8 *name      @b((input)) Name of the next configured Server.
* @param L7_char8 *nextAddr  @b((output)) Host name or IP address of the next
*                                   configured server
*
* @returns L7_SUCCESS - if there is a named server configured
* @returns L7_FAILURE - if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNamedServerOrderlyGetNext(L7_char8 *name, L7_char8 *nextName,
                                        L7_uchar8 *nextAddr ,L7_IP_ADDRESS_TYPE_t *pType)
{
  radiusServerEntry_t *nextEntry;
  L7_RC_t rc = L7_SUCCESS;

  if( L7_NULLPTR == name || L7_NULLPTR == nextAddr ||
      L7_NULLPTR == nextName || L7_NULLPTR == pType)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Invalid parameters \n",__FUNCTION__);
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    memset(nextAddr,0,L7_DNS_HOST_NAME_LEN_MAX);
    *pType = L7_IP_ADDRESS_TYPE_UNKNOWN;

    if ( radiusAuthServerNameEntryOrderlyGetNext(name,
                                                 nextName, &nextEntry) != L7_SUCCESS )
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to get the next Named Auth Server\n",__FUNCTION__);
      rc = L7_FAILURE;
    }
    /* it can be L7_NULL when a dns server is configured, that might
       had not been resolved yet. */
    else if(L7_NULL != nextEntry)
    {
      *pType = nextEntry->radiusServerDnsHostName.hostAddrType;
      if(L7_IP_ADDRESS_TYPE_DNS ==  nextEntry->serverConfig.
         radiusServerDnsHostName.hostAddrType)
      {
        osapiStrncpySafe(nextAddr,
                         nextEntry->serverConfig.radiusServerDnsHostName.host.
                         hostName,L7_DNS_HOST_NAME_LEN_MAX);
      }
      else
      {
        osapiStrncpySafe(nextAddr,
                         osapiInet_ntoa(nextEntry->radiusServerConfigIpAddress),
                         L7_DNS_HOST_NAME_LEN_MAX);
      }
    }
    else
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() No current server Found!\n",__FUNCTION__);
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Get the Server Name and Address of the next named auth server
*          from the servers' list.  The address could be DNS Host Name or IP
*          address.
*
* @param L7_char8 *name      @b((input)) Name of the configured Auth Server.
* @param L7_char8 *name      @b((input)) Name of the next configured Server.
* @param L7_char8 *nextAddr  @b((output)) Host name or IP address of the next
*                                   configured server
*
* @returns L7_SUCCESS - if there is a named server configured
* @returns L7_FAILURE - if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNamedAcctServerOrderlyGetNext(L7_char8 *name, L7_char8 *nextName,
                                            L7_char8 *nextAddr ,L7_IP_ADDRESS_TYPE_t *pType)
{
  radiusServerEntry_t *nextEntry=L7_NULL;
  L7_RC_t rc = L7_SUCCESS;

  if( L7_NULLPTR == name || L7_NULLPTR == nextAddr ||
      L7_NULLPTR == nextName || L7_NULLPTR == pType)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Invalid parameters \n",__FUNCTION__);
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    memset(nextAddr,0,L7_DNS_HOST_NAME_LEN_MAX);
    *pType = L7_IP_ADDRESS_TYPE_UNKNOWN;
    if ( radiusAcctServerNameEntryOrderlyGetNext(name,
                                                 nextName, &nextEntry) != L7_SUCCESS )
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to get the next Named Acct Server\n",__FUNCTION__);
      rc = L7_FAILURE;
    }
    /* it can be L7_NULL when a dns server is configured, that might
       not had been resolved yet. */
    else if(L7_NULL != nextEntry)
    {
      *pType = nextEntry->radiusServerDnsHostName.hostAddrType;
      if(L7_IP_ADDRESS_TYPE_DNS ==  nextEntry->serverConfig.
         radiusServerDnsHostName.hostAddrType)
      {
        osapiStrncpySafe(nextAddr,
                         nextEntry->serverConfig.
                         radiusServerDnsHostName.host.hostName,
                         L7_DNS_HOST_NAME_LEN_MAX);
      }
      else
      {
        osapiStrncpySafe(nextAddr,
                         osapiInet_ntoa(nextEntry->radiusServerConfigIpAddress),
                         L7_DNS_HOST_NAME_LEN_MAX);
      }
    }
    else
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() No current server found for server Name: %s!",__FUNCTION__,nextName);
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return rc;
}


/*********************************************************************
*
* @purpose Verify the named server exists.
*
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t radiusNamedServerAddrGet(L7_char8 *serverName, L7_RADIUS_SERVER_USAGE_TYPE_t usageType, L7_uchar8 *pServerAddr, L7_IP_ADDRESS_TYPE_t *pType)
{
  L7_uint32 index;
  L7_RC_t rc = L7_FAILURE;
  radiusServerEntry_t *currentEntry = L7_NULLPTR;

  if (L7_NULLPTR == serverName || L7_NULLPTR == pServerAddr || L7_NULLPTR == pType)
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s(): Invalid parameters! \n",__FUNCTION__);
    return L7_FAILURE;
  }
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    memset(pServerAddr,0,L7_DNS_HOST_NAME_LEN_MAX);
    *pType = L7_IP_ADDRESS_TYPE_UNKNOWN;

    rc = radiusServerNameEntryIndexGet(serverName, &index);
    if ( rc != L7_SUCCESS )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS: %s(): No server found with this name!\n",__FUNCTION__);
    }
    else
    {
      if (L7_RADIUS_SERVER_USAGE_ANY == usageType)
      {
        L7_uint32 i;

        for (i = L7_RADIUS_SERVER_USAGE_FIRST; i < L7_RADIUS_SERVER_USAGE_MAX; i++)
        {
          if ((L7_SUCCESS == radiusServerAuthCurrentEntryGet(index, i, &currentEntry)) &&
              (L7_NULLPTR != currentEntry))
          {
            break;
          }
        }
      }
      else
      {
        (void)radiusServerAuthCurrentEntryGet(index, usageType, &currentEntry);
      }
      if (L7_NULLPTR == currentEntry)
      {
        RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s(): Could not get current server!",
                    __FUNCTION__);
      }
      else
      {
        *pType = currentEntry->radiusServerDnsHostName.hostAddrType;
        if (currentEntry->radiusServerDnsHostName.hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          osapiStrncpySafe(pServerAddr,
                           osapiInet_ntoa(currentEntry->radiusServerConfigIpAddress),
                           L7_DNS_HOST_NAME_LEN_MAX);
        }
        else
        {
          osapiStrncpySafe(pServerAddr,
                           currentEntry->radiusServerDnsHostName.host.hostName,
                           L7_DNS_HOST_NAME_LEN_MAX);
        }
      }
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return rc;
}

/*********************************************************************
*
* @purpose Verify the named server exists.
*
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t radiusNamedAcctServerAddrGet(L7_char8 *serverName, L7_char8 *pServerAddr, L7_IP_ADDRESS_TYPE_t *pType)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 index;

  radiusServerEntry_t *currentEntry=L7_NULL;

  if(L7_NULLPTR == serverName || L7_NULLPTR == pServerAddr || L7_NULLPTR == pType)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Invalid parameters! \n",__FUNCTION__);
    return L7_FAILURE;
  }
  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    memset(pServerAddr,0,L7_DNS_HOST_NAME_LEN_MAX);
    *pType = L7_IP_ADDRESS_TYPE_UNKNOWN;

    rc = radiusAcctServerNameEntryIndexGet(serverName, &index);
    if( rc != L7_SUCCESS )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS: %s(): No server found with this name!\n",__FUNCTION__);
    }
    else
    {
      (void)radiusServerAcctCurrentEntryGet(index,&currentEntry);
       if(L7_NULLPTR == currentEntry)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                 "RADIUS: %s(): Could not get current server!\n",__FUNCTION__);
       }
       else
       {
         *pType = currentEntry->radiusServerDnsHostName.hostAddrType;
         if(currentEntry->radiusServerDnsHostName.hostAddrType ==
                     L7_IP_ADDRESS_TYPE_IPV4)
         {
           osapiStrncpySafe(pServerAddr,
                     osapiInet_ntoa(currentEntry->radiusServerConfigIpAddress),
                     L7_DNS_HOST_NAME_LEN_MAX);
         }
         else
         {
           osapiStrncpySafe(pServerAddr,
                     currentEntry->radiusServerDnsHostName.host.hostName,
                     L7_DNS_HOST_NAME_LEN_MAX);
         }
       }
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  return rc;

}

/*********************************************************************
*
* @purpose Get the priority of the radius server
*
* @param serverAddr @b((input))  Host Name or IP Address of the radius server
* @param type       @b((input))  Address type DNS or IP address
* @param priority   @b((output)) Priority for the configured radius server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerPriorityGet(L7_uchar8           *serverAddr,
                                        L7_IP_ADDRESS_TYPE_t type,
                                        L7_uint32           *priority)
{
  radiusServerEntry_t *serverEntry;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found. Return failure */
      *priority = L7_NULL;
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_FAILURE;
    }

    *priority = serverEntry->serverConfig.radiusServerConfigPriority;
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the priority of the radius server
*
* @param serverAddr @b((input)) Host Name or IP Address of the radius server
* @param type       @b((input)) Address type DNS or IP address
* @param priority   @b((input)) Priority for the configured radius server
*                               (lower values mean higher priority)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerPrioritySet(L7_uchar8           *serverAddr,
                                        L7_IP_ADDRESS_TYPE_t type,
                                        L7_uint32            priority)
{
  L7_RC_t rc = L7_FAILURE;
  radiusServerEntry_t *serverEntry;
  dnsHost_t hostAddr;

  if ((priority < L7_RADIUS_MIN_PRIORITY) || (priority > L7_RADIUS_MAX_PRIORITY))
  {
    return L7_FAILURE;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found */
      return L7_FAILURE;
    }
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
      hostAddr.host.ipAddr = (osapiInet_addr(serverAddr));
    }
    else
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
      osapiStrncpySafe(hostAddr.host.hostName, serverAddr, sizeof(hostAddr.host.hostName));
    }
    rc = radiusIssueCmd(RADIUS_AUTH_SERVER_PRIORITY_SET, &hostAddr, (void *)&priority);

  }
  return rc;
}

/*********************************************************************
*
* @purpose Get the priority of the accounting server
*
* @param serverAddr @b((input)) Host Name or IP Address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param priority @b((output)) Priority for thes configured accounting server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctHostNameServerPriorityGet(L7_uchar8 * serverAddr,
                                            L7_IP_ADDRESS_TYPE_t type,
                                            L7_uint32 *priority)
{
  radiusServerEntry_t *serverEntry;

  if (osapiSemaTake(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
        if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }


    if (serverEntry == L7_NULLPTR)
    {
      /* No entry with serverAddr was found. Return failure */
      *priority = L7_NULL;
      (void)osapiSemaGive(radiusTaskSyncSema);
      return L7_FAILURE;
    }

    *priority = serverEntry->serverConfig.radiusServerConfigPriority;
    (void)osapiSemaGive(radiusTaskSyncSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the server  type (Auth or Acct)
*
* @param serverAddr @b((input)) the HostName or IP address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param srvType @b((output))      the server type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusHostNameServerTypeGet(L7_uchar8 *serverAddr,
                                    L7_IP_ADDRESS_TYPE_t type,
                                    L7_uint32 *srvType)
{
  radiusServerEntry_t *serverEntry;

  if (osapiSemaTake(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)osapiSemaGive(radiusTaskSyncSema);
      /* No entry with serverAddr was found. */
      *srvType = L7_NULL;
      return L7_FAILURE;
    }

    *srvType = serverEntry->serverConfig.radiusServerConfigServerType;
    (void)osapiSemaGive(radiusTaskSyncSema);

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the server  type (Auth or Acct)
*
* @param serverAddr @b((input)) the HostName or IP address of the radius server
* @param type @b((input))       Address type DNS or IP address
* @param srvType @b((output))      the server type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctHostNameServerTypeGet(L7_uchar8 *serverAddr,
                                        L7_IP_ADDRESS_TYPE_t type,
                                        L7_uint32 *srvType)
{
  radiusServerEntry_t *serverEntry;

  if (osapiSemaTake(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_ACCT);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_ACCT);
    }

    if (serverEntry == L7_NULLPTR)
    {
      (void)osapiSemaGive(radiusTaskSyncSema);
      /* No entry with serverAddr was found. */
      *srvType = L7_NULL;
      return L7_FAILURE;
    }

    *srvType = serverEntry->serverConfig.radiusServerConfigServerType;
    (void)osapiSemaGive(radiusTaskSyncSema);

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the Number of configured servers of specified
*                    type (Auth or Acct).
*
* @param radiusServerType_t type @b((input)) Auth/Acct.
* @param L7_uint32 *count @b((input)) Pointer to the count.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusConfiguredServersCountGet(radiusServerType_t type,
                     L7_uint32 *count)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    if( L7_NULLPTR == count ||
        (RADIUS_SERVER_TYPE_AUTH  != type  && RADIUS_SERVER_TYPE_ACCT != type )
      )
    {
      break;
    }

    if (osapiSemaTake(radiusTaskSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      break;
    }

    *count = 0;
    if(RADIUS_SERVER_TYPE_AUTH  == type )
    {
      *count = radiusAuthServerCount();
    }
    else
    {
      *count = radiusAcctServerCount();
    }

    (void)osapiSemaGive(radiusTaskSyncSema);
    rc = L7_SUCCESS;

  }while(0);

  return rc;
}

/*********************************************************************
*
* @purpose Get the Number of configured Named server groups of specified
*                    type (Auth or Acct).
*
* @param radiusServerType_t type @b((input)) Auth/Acct.
* @param L7_uint32 *count @b((input)) Pointer to the count.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusConfiguredNamedServersGroupCountGet(radiusServerType_t type,
                                                  L7_uint32 *count)
{
  L7_RC_t rc = L7_FAILURE;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  radiusServerEntry_t *nextEntry;

  do
  {
    if( L7_NULLPTR == count ||
        (RADIUS_SERVER_TYPE_AUTH  != type  && RADIUS_SERVER_TYPE_ACCT != type )
      )
    {
      break;
    }

    if (osapiSemaTake(radiusTaskSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      break;
    }

    *count = 0;
    if(RADIUS_SERVER_TYPE_AUTH  == type )
    {
      bzero(name,sizeof(name));
      while ( radiusAuthServerNameEntryOrderlyGetNext(name,
                                                      name, &nextEntry) == L7_SUCCESS )
      {
        (*count)++;
      }
    }
    else
    {
      bzero(name,sizeof(name));
      while( radiusAcctServerNameEntryOrderlyGetNext(name,
                                                     name,&nextEntry) == L7_SUCCESS )
      {
        (*count)++;
      }
    }
    (void)osapiSemaGive(radiusTaskSyncSema);
    rc = L7_SUCCESS;

  }while(0);

  return rc;
}

/*********************************************************************
*
* @purpose Get the IP address of the specified DNS server.
*
* @param dnsAddr @b((input)) Pointer to the dns address string of
*                                   configured server
* @param radiusServerType_t type @b((input)) Auth/Acct.
* @param ipAddr @b((output)) Pointer to the IP address string.
*
* @returns L7_SUCCESS - if there is a server configured
* @returns L7_FAILURE - if no servers are configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerHostIPAddrGet(L7_uchar8 *dnsAddr,
                     radiusServerType_t type, L7_uint32 *ipAddr)
{
  radiusServerEntry_t * serverEntry;
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULL == dnsAddr) || (L7_NULL == ipAddr))
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Invalid parameters! ",__FUNCTION__);

    return rc;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    while (serverEntry != L7_NULLPTR)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == type &&
            serverEntry->serverConfig.radiusServerDnsHostName.hostAddrType ==
                     L7_IP_ADDRESS_TYPE_DNS &&
            (osapiStrncmp(dnsAddr,serverEntry->radiusServerDnsHostName.
                     host.hostName,strlen(dnsAddr)) == 0)
         )
      {
        *ipAddr = serverEntry->radiusServerConfigIpAddress;
        rc = L7_SUCCESS;
        break;
      }

      serverEntry = serverEntry->nextEntry;
    }

    if (L7_NULL == serverEntry)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "%s(): DNS server not found! ",__FUNCTION__);
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return rc;
}

/*******************************************************************************
*
* @purpose Get the config mask of server
*
* @param serverIPAddr  @b((input))  server IP address
* @param localMask     @b((output)) current value of localMask parameter
*
* @returns L7_SUCCESS
* @returns L7_FAILURE

* @comments
*
* @end
*
********************************************************************************/
L7_RC_t radiusLocalConfigMaskGet(dnsHost_t hostAddr,
                                L7_uint32  *localMask)
{
  radiusServerEntry_t *serverEntry = L7_NULLPTR;
  radiusClient_t *radiusClient;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    radiusClient = radiusClientGet();

    if (radiusClient != L7_NULLPTR)
    {
        if(hostAddr.hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          serverEntry = radiusServerEntryGet(hostAddr.host.ipAddr,
                                        RADIUS_SERVER_TYPE_AUTH);
        }
        else
        {
          serverEntry = radiusServerHostEntryGet(hostAddr.host.hostName,
                                        RADIUS_SERVER_TYPE_AUTH);
        }
        if (serverEntry != L7_NULLPTR)
        {
          *localMask = serverEntry->serverConfig.localConfigMask;
        }
        else
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "radiusLocalConfigMaskGet(): Failed to find server");
          return L7_FAILURE;
        }
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      return L7_SUCCESS;
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return L7_FAILURE;
}
