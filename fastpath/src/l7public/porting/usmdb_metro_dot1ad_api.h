/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    usmdb_metro_dot1ad_api.h
* @purpose     usmdb service instance API functions
* @component   dot1ad
* @comments    none
* @create      04/16/2008
* @author      pmchakri
* @end
*
**********************************************************************/

#ifndef USMDB_METRO_DOT1AD_API_H
#define USMDB_METRO_DOT1AD_API_H

#include "dot1ad_api.h"
/*********************************************************************
*
* @purpose  To create a new dot1ad service.
*
* @param    svid @b{(input)} Service VlanID for the service
*
* @returns  L7_SUCCESS       If the service created
* @returns  L7_ERROR         If the service already exists
* @returns  L7_TABLE_IS_FULL If maximum number of services already created
* @returns  L7_FAILURE       All other failures
*
* @comments The service name must be set after the service is created.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceCreate(L7_uint32 svid);

/*********************************************************************
*
* @purpose  To delete an existing dot1ad service.
*
* @param    svid @b{(input)} Service VlanID for the service
*
* @returns  L7_SUCCESS       If the service is deleted
* @returns  L7_ERROR         If the service not exist
* @returns  L7_FAILURE       All other failures
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmdbDot1adServiceDelete(L7_uint32 svid);

/*********************************************************************
*
* @purpose  Get the First service index created in the system.
*
* @param   *serviceIndex @b{(output)} first service index value
*
* @returns  L7_SUCCESS   If the first service ID is found
* @returns  L7_ERROR     If no service have been created yet
* @returns  L7_FAILURE   All other failures
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceIndexGetFirst(L7_uint32 *serviceIndex);

/*********************************************************************
*
* @purpose  Given serviceIndex, get the next service index
*
* @param    serviceIndex  @b{(input)}  the current service index
* @param    next          @b{(output)} the next service index
*
* @returns  L7_SUCCESS    If the next service index was found
* @returns  L7_ERROR      If serviceIndex does not exist
* @returns  L7_FAILURE    All other failures
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceIndexGetNext(L7_uint32 serviceIndex, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if the service VlanId is within proper range
*
* @param    svid @b{(input)} Service VlanID
*
* @returns  L7_SUCCESS   If the service VlanId is in range
* @returns  L7_FAILURE   If the service VlanId is out of range
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSvidRangeCheck(L7_uint32 svid);


/*********************************************************************
*
* @purpose  To add the name to this service.
*
* @param    svid @b{(input)} Service VlanID
* @param    name @b{(input)} Name of the service for the specified
*                            Service VlanID
*
* @returns  L7_SUCCESS       If name is added
* @returns  L7_ERROR         If the service does not exist
* @returns  L7_FAILURE       If invalid name or other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceNameAdd(L7_uint32 svid, L7_char8 *name);

/*********************************************************************
*
* @purpose  To get the name of the service for a given service VlanID.
*
* @param    svid @b{(input)}  Service VlanID
* @param    name @b{(output)} Name of the service for the specified
*                             Service VlanID
*
* @returns  L7_SUCCESS        If service name is retrieved
* @returns  L7_ERROR          If the specified service VlanID does not exist
* @returns  L7_FAILURE        All other Failures
*
* @comments Caller must provide a name buffer of at least
*           (L7_DOT1AD_SERVICE_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceNameGet(L7_uint32 svid, L7_char8 *name);

/*********************************************************************
*
* @purpose  Checks if the service name is a valid string
*
* @param    *name @b{(input)} dot1ad service name
*
* @returns  L7_SUCCESS        If the service name is valid
* @returns  L7_FAILURE        If the service name is invalid
*
* @comments This function only checks the name string syntax for a service.
*           It does not check if a service currently contains this name.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceNameStringCheck(L7_char8 *name);

/*********************************************************************
*
* @purpose  To get the VlanID of a service, given its name.
*
* @param   *name @b{(input)}  dot1ad service name
* @param   svid  @b{(output)} service VlanID
*
* @returns  L7_SUCCESS       If service instance index is retrieved
* @returns  L7_ERROR         If service instance name does not exist
* @returns  L7_FAILURE       If invalid parms, or other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceNameToSvid(L7_char8 *name, L7_uint32 *svid);

/*********************************************************************
*
* @purpose  To get the information if specified VlanID is configured as
*           a dot1ad service Vlan.
*
* @param    vlanId   @b{(input)}   vlan id
* @param    *flag    @b{(output)}  flag 
*
* @returns  L7_SUCCESS     Information is retrieved
* @returns  L7_FAILURE     Invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceVidIsConfigured(L7_uint32 vlanId, L7_BOOL* flag) ;
/*********************************************************************
*
* @purpose  To get the information if specified VlanID is configured 
*           as isolateVlanId
*
* @param    vlanId  @b{(input)}   vlan id
* @param    *flag   @b{(output)}  flag 
*
* @returns  L7_SUCCESS     Information is retrieved
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceIsolateVidIsConfigured(L7_uint32 vlanId, L7_BOOL* flag) ;

/*********************************************************************
*
* @purpose  To set the service type for a given dot1ad service.
*
* @param    svid           @b{(input)}  service VlanID.
* @param    serviceType    @b{(input)}  Type of the dot1adService.
*                                       It can be either 
*                                       L7_DOT1AD_SVCTYPE_ELINE,
*                                       or L7_DOT1AD_SVCTYPE_ELAN,
*                                       or  L7_DOT1AD_SVCTYPE_ETREE,
*                                       or  L7_DOT1AD_SVCTYPE_TLS
*
* @returns  L7_SUCCESS      service Type is set sucessfully.
* @returns  L7_ERROR        service with specified service VlanID 
*                           does not exist.
* @returns  L7_FAILURE      invalid service VlanID, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceTypeSet(L7_uint32 svid,      L7_DOT1AD_SVCTYPE_t serviceType);

/*********************************************************************
*
* @purpose  To set the isolated VlanID for E-TREE service
*
* @param    svid        @b{(input)} service index
* @param    isoltaeVid  @b{(input)} isolated vlan id
*
* @returns  L7_SUCCESS  Isolate VlanID  is set.
* @returns  L7_ERROR    service does not exist.
* @returns  L7_FAILURE  service Type is not E-TREE, invalid vlanID, 
*                       or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmdbDot1adIsolateVidSet(L7_uint32 svid, L7_uint32 isolateVid);

/*********************************************************************
*
* @purpose  To get the isolated VLAN for E-TREE service
*
* @param    svid        @b{(input)}  service VlanID
* @param    isoltaeVid  @b{(output)} isolated VlanID
*
* @returns  L7_SUCCESS  Isolated VlanID is retrieved successfully.
* @returns  L7_ERROR    Service with specified service VlanID 
*                       does not exist.
* @returns  L7_FAILURE  Invalid service VlanID, or other failure

* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmdbDot1adIsolateVidGet(L7_uint32 svid, L7_uint32 *isolateVid);

/*********************************************************************
*
* @purpose  To get the service type for a given dot1ad service.
*
* @param    svid           @b{(input)}  service VlanID.
* @param    serviceType    @b{(output)}  Type of the dot1adService.
*
* @returns  L7_SUCCESS      service Type is retrieved sucessfully.
* @returns  L7_ERROR        service with specified service VlanID 
*                           does not exist.
* @returns  L7_FAILURE      invalid service VlanID, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceTypeGet(L7_uint32 svid,
                                  L7_DOT1AD_SVCTYPE_t *serviceType);

/*********************************************************************
*
* @purpose  To set the NNI Intf List for a given dot1ad service.
*
* @param    svid           @b{(input)}  service VlanID of the 
*                                       given dot1ad service.
* @param    nniIntfList    @b{(input)}  NNI Interface List.
* @param    nniIntfCnt     @b{(input)}  Number of NNI interfaces
*                                       specified in the nniIntfList.
*
* @returns  L7_SUCCESS            If NNI Intf List is added
* @returns  L7_ERROR              If the service does not exist
* @returns  L7_TABLE_IS_FULL      If number of NNI interfaces specified is 
*                                 greater than the allowed number.
* @returns  L7_DEPENDENCY_NOT_MET If any one of the Interface in the specified
*                                 nniIntfList is not NNI 
* @returns  L7_FAILURE            If specified service VlanID is 
*                                 out of range or other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adNniIntfListSet(L7_uint32 svid, L7_uint32 *nniIntfList, L7_uint32 nniIntfCnt);

/*********************************************************************
*
* @purpose  To get the NNI InterfaceList for a given dot1ad service.
*
* @param    svid          @b{(input)}  service VlanID of the 
*                                      given dot1ad service.
* @param    *nniInfList   @b{(output)} NNI Interface List
*                                      (1 to L7_CLI_MAX_STRING_LENGTH chars)
* @param    *nniIntfCnt   @b{(output)} Count of NNI Interfaces
*
* @returns  L7_SUCCESS    If  NNI InterfaceList is retrieved successfully.
* @returns  L7_ERROR      If Service with the specified service VlanID
*                         does not exist
* @returns  L7_FAILURE    If specified service VlanID is
*                         out of range or other failure
*
* @comments Caller must provide a nniIntfList buffer of at least
*           (L7_CLI_MAX_STRING_LENGTH+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adNniIntfListGet(L7_uint32 svid, L7_uint32 *nniIntfList, L7_uint32 *nniIntfCnt);

/*********************************************************************
*
* @purpose  To add a subscription to a given dot1ad service on a 
*           given interface.
*
* @param    svid          @b{(input)}  service VlanID of the
*                                      given dot1ad service.
* @param    intIfNum      @b{(input)}  Internal Interface number.
*
* @returns  L7_SUCCESS  If adding a subscription is successfull.
* @returns  L7_NOT_SUPPORTED If NNI interface list is specified at
*                            subscription level for service
*                            subscriptions whose service type is not
*                            E-LINE.
* @returns   L7_DEPENDENCY_NOT_MET If any one of the Interface in the 
*                                 specified nniIntfList is not NNI.
* @returns  L7_ERROR    If service with the specified service VlanID
*                       does not exist.
* @returns  L7_REQUEST_DENIED If hardware update failed
* @returns  L7_TABLE_IS_FULL If number of specified NNI interfaces is greater
*                            than the allowed number.
* @returns  L7_FAILURE  For all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionAdd(L7_uint32 intf, L7_uint32 svid);

/*********************************************************************
*
* @purpose  To delete a dot1ad service subscription on an 
*           given interface  
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} service VlanID of the
*                                  given dot1ad service.
* @param    subscrName @b{(input)} Name of the dot1ad service 
*                                  subscription.
*
* @returns  L7_SUCCESS  If the dot1ad service subscription is deleted. 
* @returns  L7_FAILURE  invalid service identifier, all other failures.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionDelete(L7_uint32 intf, L7_uint32 svid, L7_uchar8 *subscrName);

/*********************************************************************
*
* @purpose  Reserve current subscription structure for given internal 
*           interface number and dot1ad service VlanID.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
*
* @returns  L7_SUCCESS     If current subscription is reserved.
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adReserveCurrentSubscription(L7_uint32 intIfNum, L7_uint32 svid);

/*********************************************************************
*
* @purpose  Release current subscription structure from given 
*           internal interface number and dot1ad service VlanID.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
*
* @returns  L7_SUCCESS     If current subscription is released.
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adReleaseCurrentSubscription(L7_uint32 intIfNum, L7_uint32 svid);

/*********************************************************************
*
* @purpose  To set the name for a dot1ad service subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    name       @b{(input)} subscription  name
*
* @returns  L7_SUCCESS     If subscription name is set successfully
* @returns  L7_FAILURE     Subscription name already exists, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionNameSet(L7_uint32 intIfNum, L7_uint32 svid, L7_char8 *name);

/*********************************************************************
*
* @purpose  To get the name of a  dot1ad service subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    cvid       @b{(input)} customer VlanID
* @param    name       @b{(output)} subscription  name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE     
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionNameGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_char8 *name);

/*********************************************************************
*
* @purpose  To set the matching Packet type for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    pktType    @b{(input)} Matching Packet type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionPktTypeSet(L7_uint32 intIfNum, L7_uint32 svid, L7_DOT1AD_PKTTYPE_t pktType);

/*********************************************************************
*
* @purpose  To get the matching Packet type for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    cvid       @b{(input)} customer VlanID
* @param    pktType    @b{(output)} Matching Packet type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionPktTypeGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_DOT1AD_PKTTYPE_t *pktType);

/*********************************************************************
*
* @purpose  To set the Service VlanID for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionSvidSet(L7_uint32 intIfNum, L7_uint32 svid);

/*********************************************************************
*
* @purpose  To set the matching customer VlanID for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    matchCvid  @b{(input)} Matching customer VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionMatchCvidSet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 matchCvid);

/*********************************************************************
*
* @purpose  To get the matching customer VlanID for a dot1ad subscription.
*
* @param    subscrIndex @b{(input)} subscrIndex
* @param    intIfNum    @b{(input)} internal interface num
* @param    matchCvid   @b{(output)} Matching customer VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceIntfMatchCvidGet(L7_uint32 subscrIndex, L7_uint32 intIfNum, L7_uint32 *matchCvid);

/*********************************************************************
*
* @purpose  To set the matching Service VlanID for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    matchSvid  @b{(input)} Matching service VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionMatchSvidSet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 matchSvid);

/*********************************************************************
*
* @purpose  To get the matching service VlanID for a dot1ad subscription.
*
* @param    subscrIndex @b{(input)} subscrIndex
* @param    intIfNum    @b{(input)} internal interface num
* @param    matchSvid   @b{(output)} Matching service VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceIntfMatchSvidGet(L7_uint32 subscrIndex, L7_uint32 intIfNum, L7_uint32 *matchSvid);

/*********************************************************************
*
* @purpose  To set the matching dot1p priority for a dot1ad subscription.
*
* @param    intIfNum    @b{(input)} internal interface num
* @param    svid        @b{(input)} dot1ad service VlanID
* @param    priority    @b{(input)} Matching dot1p priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionMatchPrioritySet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 priority);

/*********************************************************************
*
* @purpose  To get the matching dot1p priority for a dot1ad subscription.
*
* @param    subscrIndex @b{(input)} subscrIndex
* @param    intIfNum    @b{(input)} internal interface num
* @param    priority    @b{(output)} Matching dot1p priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceIntfMatchPriorityGet(L7_uint32 subscrIndex, L7_uint32 intIfNum, L7_uint32 *priority);

/*********************************************************************
*
* @purpose  To set the resulting customer VlanID for a dot1ad subscription.
*
* @param    intIfNum    @b{(input)} internal interface num
* @param    svid        @b{(input)} dot1ad service VlanID
* @param    cvid        @b{(input)} resulting customer VlanID.
*                                   This is specified as part of 
*                                   assign-cvid or remark-cvid actions.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionCvidSet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid);

/*********************************************************************
*
* @purpose  To set the action remove CTAG  for a dot1ad subscription.
*
* @param    intIfNum    @b{(input)} internal interface num
* @param    svid        @b{(input)} dot1ad service VlanID
* @param    removeCtag  @b{(input)} removeCtag
*                                   (L7_TRUE or L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionRemoveCtagSet(L7_uint32 intIfNum, L7_uint32 svid, L7_BOOL removeCtag);

/*********************************************************************
*
* @purpose  To get the remove CTAG action for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)}  internal interface num
* @param    removeCtag @b{(output)} remove CTAG action.
*                                   (L7_TRUE or L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceRemoveCtagGet(L7_uint32 subscrIndex, L7_uint32 intIfNum, L7_BOOL *removeCtag);

/*********************************************************************
*
* @purpose  To set the NNI Interface List for a given dot1ad service
*           subscription.
*
* @param    intIfNum      @b{(input)} internal interface num
* @param    svid          @b{(input)} dot1ad service VlanID
* @param    cvid          @b{(input)} customer VlanID
* @param    nniIntfList   @b{(input)} NNI Interface List.
* @param    nniIntfCnt    @b{(input)} Number of NNI interfaces
*                                     specified in the nniIntfList.
*
* @returns  L7_SUCCESS    If NNI Intferface List is added
* @returns  L7_FAILURE    If any one of the Interface in the specified
*                         nniIntfList is not NNI other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adSubscriptionNNIIntfListSet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 *nniIntfList, L7_uint32 nniIntfCnt);

/*********************************************************************
*
* @purpose  To get the NNI InterfaceList for a given dot1ad service
*           subscription.
*
* @param    subscrIndex   @b{(input)}  subscrIndex
* @param    intIfNum      @b{(input)}  internal interface num
* @param    *nniInfList   @b{(output)} NNI Interface List
* @param    *nniIntfCnt   @b{(output)} Count of NNI Interfaces
*
* @returns  L7_SUCCESS    If  NNI InterfaceList is retrieved successfully.
* @returns  L7_FAILURE    
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adServiceIntfNniIntfListGet(L7_uint32 subscrIndex, L7_uint32 intIfNum, L7_uint32 *nniIntfList, L7_uint32 *nniIntfCnt);

/*********************************************************************
*
* @purpose  To check if a field is enabled in a given mask
*
* @param    mask      @b{(input)}  mask
* @param    field     @b{(input)}  field 
*
* @returns  L7_TRUE     
* @returns  L7_FALSE     
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL usmDbDot1adIsFieldConfigured(L7_uint32 mask, Dot1adRuleFields_t field );

/*********************************************************************
*
* @purpose  Gets interface mask of NNI interfaces for a given Service Vlan
*
* @param    L7_uint32   UnitIndex   @b((input))  The unit for this operation
* @param    L7_uint32   svid        @b((input))  serviceVlanId for  this operation
* @param    L7_uchar8   *mask       @b{(output)} Bit Mask of the interfaces
*                                   1  indicates this interface is part of NNI list
*                                   0  indicates this interface is NOT part of NNI list
* @param    L7_uint32   *maskLen    @b{(input/output)} Size of mask
* @param    L7_uint32   *nniIntfCnt @b{(output)} Number of NNI Interfaces
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if bit mask buffer is not big enough
*
* @notes    Mask is written into and is returned with bits set to represent interfaces that are
*           in the intf list, in the manner SNMP wants. MSb represents interface 1
*
* @end
*********************************************************************/
L7_RC_t usmDbNNIIntfMaskGet(L7_uint32 UnitIndex, 
                            L7_uint32 svid, 
                            L7_uchar8 *mask, 
                            L7_uint32 *maskLen, 
                            L7_uint32 *nniIntfCnt);

/*********************************************************************
*
* @purpose  Sets the NNI interface List via a bit mask for a given Service Vlan
*
* @param    L7_uint32   UnitIndex   @b((input))  The unit for this operation
* @param    L7_uint32   svid        @b((input))  serviceVlanId for  this operation
* @param    L7_uchar8   *maskOctets @b{(input)}  Bit Mask of the interfaces
*                                   1  indicates this interface is part of NNI list
*                                   0  indicates this interface is NOT part of NNI list
* @param    L7_uint32   *maskLen    @b{(input/output)} Size of mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if mode could not be set or bit mask buffer is not big enough
*
* @notes    All set-able interfaces will be set.  If any one fails, a failure will
* @notes    be returned.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbNNIIntfMaskSet(L7_uint32 UnitIndex,
                            L7_uint32 svid, 
                            L7_uchar8 *maskOctets,
                            L7_uint32 *maskLen);
L7_RC_t usmDbdot1adSubscriptionEntryTreeSearch(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, 
                                                        L7_uint32 matchType, dot1adSubscriptionStruct_t **subscrEntry);

L7_RC_t usmDbDot1adSubscriptionFirstIntfEntryGet(L7_uint32 intfIfNum, dot1adSubscriptionStruct_t **subscrEntry);

L7_RC_t usmDbDot1adSubscriptionNextIntfEntryGet(dot1adSubscriptionStruct_t *subscrEntry, dot1adSubscriptionStruct_t **nextSubscrEntry);

L7_RC_t usmDbDot1adSubscriptionNextEntryGet(dot1adSubscriptionStruct_t *subscrEntry, dot1adSubscriptionStruct_t **nextSubscrEntry);

L7_RC_t usmDbDot1adSubEntrySubNameGet(L7_uint32 intIfNum, L7_uchar8 *subscrName, dot1adSubscriptionStruct_t **subscrEntry);

L7_RC_t usmDbDot1adSubEntryIntfSubNameGet(L7_uint32 intIfNum, L7_uchar8 *subscrName);

L7_RC_t usmDbDot1adServiceIntfSubscrIndexGetFirst(L7_uint32 intIfNum, L7_uint32 *subscrIndex);

L7_RC_t usmDbDot1adServiceIntfSubscrIndexGetNext(L7_uint32 intIfNum, L7_uint32 oldsusbcrIndex, L7_uint32 *subscrIndex);

L7_RC_t usmDbDot1adServiceIntfSubscrNameToIndex(L7_uint32 *subscrIndex, L7_uint32 intIfNum, L7_char8 *name);

L7_RC_t usmDbDot1adServiceIntfSvidGet(L7_uint32 subscrIndex, L7_uint32 intIfNum, L7_uint32 *svid);

L7_RC_t usmDbDot1adServiceIntfCvidGet(L7_uint32 subscrIndex,L7_uint32 intIfNum, L7_uint32 *cvid);

L7_RC_t usmDbDot1adServiceIntfRemove(L7_uint32 subscrIndex, L7_uint32 intIfNum);


#endif /* USMDB_METRO_DOT1AD_API_H */



