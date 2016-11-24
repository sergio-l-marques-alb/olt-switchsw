/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpcm_api.h
*
* @purpose      Captive Portal Client Credential Manager (CPCM) API
*               header.
*
* @component    CPCM
*
* @comments     APIs for usmDb layer. These APIs can be called via usmDb
*               and by the captive portal components.
*
* @create       08/08/2007
*
* @author       darsenault
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_CPCM_API_H
#define INCLUDE_CPCM_API_H

#include <time.h>
#include "datatypes.h"
#include "intf_cb_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"

/****************************************************************************/
/*                     RADIUS RELATED DATA STRUCTURES                       */
/****************************************************************************/

#define L7_CP_RAD_BASE_ATTR_SIZE 2

typedef struct CP_radiusAttr_s
{
	L7_uchar8  type;
	L7_uchar8  length;	/* lengh of entire attribute including the type and length fields */
} CP_radiusAttr_t;

#define CP_RAD_VEN_ATTR_SIZE 6
#include "radius_api.h"

typedef struct CP_vsRadiusAttr_s
{
    L7_uint32   vendor;  /* Vendor ID (network-byte order) */
    L7_uchar8   vType;   /* Vendor-specific type code */
    L7_uchar8   vLength; /* Length of vendor-specific attribute including the type and length fields */

} CP_vsRadiusAttr_t;

typedef struct CP_radiusEntry_s
{
  L7_uint32 sessionTimeout;
  L7_uint32 idleTimeout;
  L7_uint32 maxBandwidthUp;
  L7_uint32 maxBandwidthDown;
  L7_uint32 maxInputOctets;
  L7_uint32 maxOutputOctets;
  L7_uint32 maxTotalOctets;
  L7_char8  userGroups[CP_USER_RADIUS_USERGROUPS_MAX+1];
  /* userGroups are comma-delimited */
} CP_radiusEntry_t;

/* Captive Portal RADIUS users */

/*********************************************************************
*
* @purpose  Prepare to authenticate a new user on an interface. This
*           function essentially adds a new authentication in progress
*           entry into the AIPStatus table.
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpcmPrepareToAuthenticate(void);


/*********************************************************************
*
* @purpose  Prepare to authenticate a new user on an interface. This
*           teeny tiny function serves as a small wrapper for NIM.
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
* @param    L7_enetMacAddr_t  macAddr @b{(input)} MAC addr
* @param    L7_uint32         intfId  @b{(input)} internal I/F ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmPrepareToAuthenticate_NIMWrapper(L7_uint32          intfId,
                                             L7_enetMacAddr_t   mac,
                                             cpRedirIpAddr_t    ipAddr);
                                             
/*****************************************************************************
*
* @purpose  This function services UI authentication requests. It first
*           searches the client connection status table to determine if this
*           user has been authenticated. If so, it returns authentication
*           success. If not found in the client connection status table, it
*           should be in the aip table. If it is, and this function was called
*           as the initial request (indicated by the user->flag w/validate)
*           then the aip table entry is updated to indicate an authentication
*           request that includes the uid, pwd, connection time and wip status
*           flag. If its not an initial request (then its a wip refresh), so
*           simply return the latest status flag from the aip table.
*
* @param    cpUserAuth_t            user   @b{(input)} user related params
* @param    CP_AUTH_STATUS_FLAG_t   *flag  @b{(output)} current auth flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*****************************************************************************/
L7_RC_t cpcmUserAuthRequest(cpUserAuth_t *user, CP_AUTH_STATUS_FLAG_t *flag);


/*********************************************************************
*
* @purpose  Do the dirty work of authentication in the CP task
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmTryAuthentication(void);

/**************************************************************************
*
* @purpose   RADIUS callback
*
* @param     status         @b{(input)} status of RADIUS response (accept, reject, challenge, etc)
* @param     correlator     @b{(input)} correlates responses to requests; for captive portal, this is client mac
* @param     *attributes    @b{(input)} RADIUS attribute data
* @param     attributesLen  @b{(input)} length of RADIUS attribute data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  
*         
* @end
*************************************************************************/
L7_RC_t cpcmRadiusResponseCallback(L7_uint32 status,
                                   L7_uint32 correlator,
                                   L7_uchar8 *attributes,
                                   L7_uint32 attributesLen);


/*********************************************************************
*
* @purpose  Add IP/MAC address pair to  authorization in 
*           progress table
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
* @param    L7_enetMacAddr_t* macAddr @b{(input)} MAC addr
* @param    L7_uint32         intfId  @b{(input)} internal I/F ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusAdd(L7_IP_ADDR_t       ipAddr, 
                         L7_enetMacAddr_t * mac,
                         L7_uint32          intfId);


/*********************************************************************
*
* @purpose  Delete  authorization in progress table entry
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusDelete(L7_IP_ADDR_t  ipAddr);

/*********************************************************************
*
* @purpose  Retieves the tryAuth flag which is used to signal the CP
*           task to look for client authorization candidates
*
* @param    L7_IP_ADDR_t  ipAddr    @b{(input)} IP addr
* @param    L7_BOOL       *tryAuth  @b{(output)} indicates wakeup
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusTryAuthenGet(L7_IP_ADDR_t ipAddr, L7_BOOL *tryAuth);

/*********************************************************************
*
* @purpose  Check for existing IP address with authorization in 
*           progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusGet(L7_IP_ADDR_t ipAddr);

/*********************************************************************
*
* @purpose  Get description for existing IP address with authorization 
*           in  progress, if it exists
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    cpcmAuthInProgressDesc_t *  @b{(output)} description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusDescGet(L7_IP_ADDR_t ipAddr,
			     cpcmAuthInProgressStatusDesc_t * pDesc);


/*********************************************************************
*
* @purpose  Get limits for existing IP address with authorization 
*           in progress, if it exists
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    cpcmAuthInProgressStatusLimits_t *  @b{(output)} limits
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusLimitsGet(L7_IP_ADDR_t ipAddr,
			       cpConnectionLimits_t * pLimits);


/*********************************************************************
*
* @purpose  Check for NEXT IP address with authorization in 
*           progress, given a current IP address
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_IP_ADDR_t  *nextIpAddr  @b{(output)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusNextGet(L7_IP_ADDR_t ipAddr, L7_IP_ADDR_t * nextIpAddr);

/*********************************************************************
*
* @purpose  Retrieve MAC address for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr   @b{(input)} IP addr
* @param    L7_enetMacAddr_t *mac  @b{(output)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMacAddrGet(L7_IP_ADDR_t ipAddr, L7_enetMacAddr_t * mac);

/*********************************************************************
*
* @purpose  Retrieve interface ID for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_enetMacAddr_t * intfId  @b{(output)} I/F ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusIntfIdGet(L7_IP_ADDR_t ipAddr, L7_uint32 * pIntfId);

/*********************************************************************
*
* @purpose  Retrieve client connection port for existing IP address
*           with authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     * port  @b{(output)} client port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusPortGet(L7_IP_ADDR_t ipAddr, L7_uint32 * port);

/*********************************************************************
*
* @purpose  Retrieve session timeout for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   *  timeout  @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusSessionTimeoutGet(L7_IP_ADDR_t ipAddr, L7_uint32 * timeout);

/*********************************************************************
*
* @purpose  Retrieve idle timeout for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   *  timeout  @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusIdleTimeoutGet(L7_IP_ADDR_t ipAddr, L7_uint32 * timeout);

/*********************************************************************
*
* @purpose  Retrieve max bandwidth up rate from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxBandwidthUp  @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxBandwidthUpGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxBandwidthUp);

/*********************************************************************
*
* @purpose  Retrieve max bandwidth down rate from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxBandwidthDown @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxBandwidthDownGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxBandwidthDown);

/*********************************************************************
*
* @purpose  Retrieve max input octets from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxInputOctets @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxInputOctetsGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxInputOctets);

/*********************************************************************
*
* @purpose  Retrieve max output octets from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxOutputOctets @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxOutputOctetsGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxOutputOctets);

/*********************************************************************
*
* @purpose  Retrieve max total octets from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxTotalOctets @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxTotalOctetsGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxTotalOctets);

/*********************************************************************
*
* @purpose  Retrieve connection time for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    time_t   *  connTime  @b{(output)} connection time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusConnTimeGet(L7_IP_ADDR_t ipAddr, time_t * connTime);

/*********************************************************************
*
* @purpose  Retrieve uid for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uchar8  *  uid  @b{(output)} user ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusUIDGet(L7_IP_ADDR_t ipAddr, L7_uchar8 * uid);

/*********************************************************************
*
* @purpose  Retrieve password for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uchar8  *  pwd  @b{(output)} password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusPwdGet(L7_IP_ADDR_t ipAddr, L7_uchar8 * pwd);


/*********************************************************************
*
* @purpose  Retrieve authorization flag for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    CP_AUTH_STATUS_FLAG_t * flag  @b{(output)} auth flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusFlagGet(L7_IP_ADDR_t ipAddr, 
			     CP_AUTH_STATUS_FLAG_t * flag);

/*********************************************************************
*
* @purpose  Set boolean used to wakeup the CP task to look for client
*           authorization candidates
*
* @param    L7_IP_ADDR_t  ipAddr   @b{(input)} IP addr
* @param    L7_BOOL tryAuth        @b{(input)} indicates wakeup
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusTryAuthenSet(L7_IP_ADDR_t ipAddr, L7_BOOL tryAuth);

/*********************************************************************
*
* @purpose  Set MAC address for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr   @b{(input)} IP addr
* @param    L7_enetMacAddr_t *mac  @b{(input)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMacAddrSet(L7_IP_ADDR_t ipAddr, L7_enetMacAddr_t * mac);

/*********************************************************************
*
* @purpose  Set client connection port for existing IP address
*           with authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     port    @b{(output)} client port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusPortSet(L7_IP_ADDR_t ipAddr, L7_uint32 port);

/*********************************************************************
*
* @purpose  Set interface ID for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     intfId  @b{(input)} I/F ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusIntfIdSet(L7_IP_ADDR_t ipAddr, L7_uint32 intfId);

/*********************************************************************
*
* @purpose  Set session timeout for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     timeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusSessionTimeoutSet(L7_IP_ADDR_t ipAddr, L7_uint32 timeout);

/*********************************************************************
*
* @purpose  Set idle timeout for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     timeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusIdleTimeoutSet(L7_IP_ADDR_t ipAddr, L7_uint32 timeout);


/*********************************************************************
*
* @purpose  Set max bandwidth up rate for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxBandwidthUp @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxBandwidthUpSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxBandwidthUp);

/*********************************************************************
*
* @purpose  Set max bandwidth down rate for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxBandwidthDown @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxBandwidthDownSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxBandwidthDown);

/*********************************************************************
*
* @purpose  Set max input octets for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxInputOctets @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxInputOctetsSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxInputOctets);

/*********************************************************************
*
* @purpose  Set max output octets for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxOutputOctets @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxOutputOctetsSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxOutputOctets);

/*********************************************************************
*
* @purpose  Set max total octets for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxTotalOctets @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxTotalOctetsSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxTotalOctets);


/*********************************************************************
*
* @purpose  Set connection time for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    time_t   *  connTime  @b{(input)} connection time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusConnTimeSet(L7_IP_ADDR_t ipAddr, time_t * connTime);


/*********************************************************************
*
* @purpose  Set uid for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr @b{(input)} IP addr
* @param    L7_uchar8  *  uid    @b{(input)} user ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusUIDSet(L7_IP_ADDR_t ipAddr, L7_uchar8 * uid);


/*********************************************************************
*
* @purpose  Set password for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr @b{(input)} IP addr
* @param    L7_uchar8  *  pwd    @b{(input)} password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusPwdSet(L7_IP_ADDR_t ipAddr, L7_uchar8 * pwd);


/*********************************************************************
*
* @purpose  Set authorization flag for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t          ipAddr @b{(input)} IP addr
* @param    CP_AUTH_STATUS_FLAG_t flag   @b{(input)} auth flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusFlagSet(L7_IP_ADDR_t ipAddr, 
			     CP_AUTH_STATUS_FLAG_t flag);

#endif /* INCLUDE_CPCM_API_H */

