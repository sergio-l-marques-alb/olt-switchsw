/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_api.h
*
* @purpose RADIUS Client API's include file
*
* @component radius
*
* @comments
*
* @create 03/25/2003
*
* @author pmurthy
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_RADIUS_API_H
#define INCLUDE_RADIUS_API_H

#include "radius_exports.h"

/*
** Defines necessary for api use
*/

#define RADIUS_LVL7_VENDOR_ID            6132

#define RADIUS_REQUEST_TYPE_AUTH         1
#define RADIUS_REQUEST_TYPE_ACCT         2
#define RADIUS_REQUEST_TYPE_AUTH_VP      3

#define RADIUS_STATUS_SUCCESS            1
#define RADIUS_STATUS_CHALLENGED         2
#define RADIUS_STATUS_AUTHEN_FAILURE     3
#define RADIUS_STATUS_REQUEST_TIMED_OUT  4
#define RADIUS_STATUS_COMM_FAILURE       5

#define RADIUS_VALUE_LENGTH             253

#define RADIUS_ATTR_TYPE_USER_NAME                   1
#define RADIUS_ATTR_TYPE_USER_PASSWORD               2
#define RADIUS_ATTR_TYPE_CHAP_PASSWORD               3
#define RADIUS_ATTR_TYPE_NAS_IP_ADDRESS              4
#define RADIUS_ATTR_TYPE_NAS_PORT                    5
#define RADIUS_ATTR_TYPE_SERVICE_TYPE                6
#define RADIUS_ATTR_TYPE_FRAMED_PROTOCOL             7
#define RADIUS_ATTR_TYPE_FRAMED_IP_ADDRESS           8
#define RADIUS_ATTR_TYPE_FRAMED_IP_NETMASK           9
#define RADIUS_ATTR_TYPE_FRAMED_ROUTING             10
#define RADIUS_ATTR_TYPE_FILTER_ID                  11
#define RADIUS_ATTR_TYPE_FRAMED_MTU                 12
#define RADIUS_ATTR_TYPE_FRAMED_COMPRESSION         13
#define RADIUS_ATTR_TYPE_LOGIN_IP_HOST              14
#define RADIUS_ATTR_TYPE_LOGIN_SERVICE              15
#define RADIUS_ATTR_TYPE_LOGIN_TCP_PORT             16

#define RADIUS_ATTR_TYPE_REPLY_MESSAGE              18
#define RADIUS_ATTR_TYPE_CALLBACK_NUMBER            19
#define RADIUS_ATTR_TYPE_CALLBACK_ID                20

#define RADIUS_ATTR_TYPE_FRAMED_ROUTE               22
#define RADIUS_ATTR_TYPE_FRAMED_IPX_NETWORK         23
#define RADIUS_ATTR_TYPE_STATE                      24
#define RADIUS_ATTR_TYPE_CLASS                      25
#define RADIUS_ATTR_TYPE_VENDOR                     26
#define RADIUS_ATTR_TYPE_SESSION_TIMEOUT            27
#define RADIUS_ATTR_TYPE_IDLE_TIMEOUT               28
#define RADIUS_ATTR_TYPE_TERMINATION_ACTION         29
#define RADIUS_ATTR_TYPE_CALLED_STATION_ID          30
#define RADIUS_ATTR_TYPE_CALLING_STATION_ID         31
#define RADIUS_ATTR_TYPE_NAS_IDENTIFIER             32
#define RADIUS_ATTR_TYPE_PROXY_STATE                33
#define RADIUS_ATTR_TYPE_LOGIN_LAT_SERVICE          34
#define RADIUS_ATTR_TYPE_LOGIN_LAT_NODE             35
#define RADIUS_ATTR_TYPE_LOGIN_LAT_GROUP            36
#define RADIUS_ATTR_TYPE_FRAMED_APPLETALK_LINK      37
#define RADIUS_ATTR_TYPE_FRAMED_APPLETALK_NETWORK   38
#define RADIUS_ATTR_TYPE_FRAMED_APPLETALK_ZONE      39
#define RADIUS_ATTR_TYPE_ACCT_STATUS_TYPE           40
#define RADIUS_ATTR_TYPE_ACCT_DELAY_TIME            41
#define RADIUS_ATTR_TYPE_ACCT_INPUT_OCTETS          42
#define RADIUS_ATTR_TYPE_ACCT_OUTPUT_OCTETS         43
#define RADIUS_ATTR_TYPE_ACCT_SESSION_ID            44
#define RADIUS_ATTR_TYPE_ACCT_AUTHENTIC             45
#define RADIUS_ATTR_TYPE_ACCT_SESSION_TIME          46
#define RADIUS_ATTR_TYPE_ACCT_INPUT_PACKETS         47
#define RADIUS_ATTR_TYPE_ACCT_OUTPUT_PACKETS        48
#define RADIUS_ATTR_TYPE_ACCT_TERMINATE_CAUSE       49
#define RADIUS_ATTR_TYPE_ACCT_MULTI_SESSION_ID      50
#define RADIUS_ATTR_TYPE_ACCT_LINK_COUNT            51
#define RADIUS_ATTR_TYPE_ACCT_G_IBYTES              52
#define RADIUS_ATTR_TYPE_ACCT_G_OBYTES              53


#define RADIUS_ATTR_TYPE_CHAP_CHALLENGE             60
#define RADIUS_ATTR_TYPE_NAS_PORT_TYPE              61
#define RADIUS_ATTR_TYPE_PORT_LIMIT                 62
#define RADIUS_ATTR_TYPE_LOGIN_LAST_PORT            63
#define RADIUS_ATTR_TYPE_TUNNEL_TYPE                64
#define RADIUS_ATTR_TYPE_TUNNEL_MEDIUM_TYPE         65

#define RADIUS_ATTR_TYPE_CONNECT_INFO               77

#define RADIUS_ATTR_TYPE_EAP_MESSAGE                79
#define RADIUS_ATTR_TYPE_MESSAGE_AUTHENTICATOR      80
#define RADIUS_ATTR_TYPE_TUNNEL_PRIVATE_GROUP_ID    81
#define RADIUS_ATTR_TYPE_VLAN_ID                    199

/*
** The Service-Type value codes
*/
#define RADIUS_SERVICE_TYPE_LOGIN                 1
#define RADIUS_SERVICE_TYPE_FRAMED                2
#define RADIUS_SERVICE_TYPE_CALLBACK_LOGIN        3
#define RADIUS_SERVICE_TYPE_CALLBACK_FRAMED       4
#define RADIUS_SERVICE_TYPE_OUTBOUND              5
#define RADIUS_SERVICE_TYPE_ADMIN                 6
#define RADIUS_SERVICE_TYPE_NAS_PROMPT            7
#define RADIUS_SERVICE_TYPE_AUTHEN_ONLY           8
#define RADIUS_SERVICE_TYPE_CALLBACK_NAS_PROMPT   9

/*
** The Framed Protocol value types
*/
#define RADIUS_FRAMED_PROTOCOL_PPP      1
#define RADIUS_FRAMED_PROTOCOL_SLIP     2
#define RADIUS_FRAMED_PROTOCOL_ARAP     3
#define RADIUS_FRAMED_PROTOCOL_SLP_MLP  4
#define RADIUS_FRAMED_PROTOCOL_IPX_SLIP 5

/*
** The Framed Routing value types
*/
#define RADIUS_FRAMED_ROUTING_NONE        0
#define RADIUS_FRAMED_ROUTING_SEND        1
#define RADIUS_FRAMED_ROUTING_LISTEN      2
#define RADIUS_FRAMED_ROUTING_SEND_LISTEN 3

/*
** The Framed Compression value types
*/
#define RADIUS_FRAMED_COMPRESSION_NONE        0
#define RADIUS_FRAMED_COMPRESSION_VJ_TCPIP    1
#define RADIUS_FRAMED_COMPRESSION_IPX_HDR     2

/*
** The Login Service value codes
*/
#define RADIUS_LOGIN_SERVICE_TELNET      1
#define RADIUS_LOGIN_SERVICE_RLOGIN      2
#define RADIUS_LOGIN_SERVICE_TCP_CLEAR   3
#define RADIUS_LOGIN_SERVICE_PORT_MASTER 4
#define RADIUS_LOGIN_SERVICE_LAST        5

/*
** The Termination Action value codes
*/
#define RADIUS_TERMINATION_ACTION_DEFAULT  0
#define RADIUS_TERMINATION_ACTION_RADIUS   1

/*
** NAS Port Types
*/
#define RADIUS_NAS_PORT_TYPE_ASYNC              0
#define RADIUS_NAS_PORT_TYPE_SYNC               1
#define RADIUS_NAS_PORT_TYPE_ISDN_SYNC          2
#define RADIUS_NAS_PORT_TYPE_ISDN_SYNC_V120     3
#define RADIUS_NAS_PORT_TYPE_ISDN_SYNC_V110     4
#define RADIUS_NAS_PORT_TYPE_VIRTUAL            5
#define RADIUS_NAS_PORT_TYPE_PIAFS              6
#define RADIUS_NAS_PORT_TYPE_HDLC_CLEAR_CHANNEL 7
#define RADIUS_NAS_PORT_TYPE_X25                8
#define RADIUS_NAS_PORT_TYPE_X75                9
#define RADIUS_NAS_PORT_TYPE_G3_FAX             10
#define RADIUS_NAS_PORT_TYPE_SDSL               11
#define RADIUS_NAS_PORT_TYPE_ADSL_CAP           12
#define RADIUS_NAS_PORT_TYPE_ADSL_DMT           13
#define RADIUS_NAS_PORT_TYPE_IDSL               14
#define RADIUS_NAS_PORT_TYPE_ETHERNET           15
#define RADIUS_NAS_PORT_TYPE_XDSL               16
#define RADIUS_NAS_PORT_TYPE_CABLE              17
#define RADIUS_NAS_PORT_TYPE_WIRELESS_OTHER     18
#define RADIUS_NAS_PORT_TYPE_WIRELESS_802_11    19

/*
** The Account-Status-Type value codes
*/
#define RADIUS_ACCT_STATUS_TYPE_START             1
#define RADIUS_ACCT_STATUS_TYPE_STOP              2

/*
** Radius Account Authentic value codes
*/
#define RADIUS_ACCT_AUTHENTIC_RADIUS    1
#define RADIUS_ACCT_AUTHENTIC_LOCAL     2
#define RADIUS_ACCT_AUTHENTIC_REMOTE    3

/*
** The Accounting Termination Cause value codes
*/
#define RADIUS_ACCT_TERM_CAUSE_USER_REQUEST                   1
#define RADIUS_ACCT_TERM_CAUSE_LOST_CARRIER                   2
#define RADIUS_ACCT_TERM_CAUSE_LOST_SERVICE                   3
#define RADIUS_ACCT_TERM_CAUSE_IDLE_TIMEOUT                   4
#define RADIUS_ACCT_TERM_CAUSE_SESSION_TIMEOUT                5
#define RADIUS_ACCT_TERM_CAUSE_ADMIN_RESET                    6
#define RADIUS_ACCT_TERM_CAUSE_ADMIN_REBOOT                   7
#define RADIUS_ACCT_TERM_CAUSE_PORT_ERROR                     8
#define RADIUS_ACCT_TERM_CAUSE_NAS_ERROR                      9
#define RADIUS_ACCT_TERM_CAUSE_NAS_REQUEST                    10
#define RADIUS_ACCT_TERM_CAUSE_NAS_REBOOT                     11
#define RADIUS_ACCT_TERM_CAUSE_PORT_UNNEEDED                  12
#define RADIUS_ACCT_TERM_CAUSE_PORT_PREEMPTED                 13
#define RADIUS_ACCT_TERM_CAUSE_PORT_SUSPENDED                 14
#define RADIUS_ACCT_TERM_CAUSE_SERVICE_UNAVAILABLE            15
#define RADIUS_ACCT_TERM_CAUSE_CALLBACK                       16
#define RADIUS_ACCT_TERM_CAUSE_USER_ERROR                     17
#define RADIUS_ACCT_TERM_CAUSE_HOST_REQUEST                   18
#define RADIUS_ACCT_TERM_CAUSE_SUPPLICANT_RESTART             19
#define RADIUS_ACCT_TERM_CAUSE_REAUTHENTICATION_FAILURE       20
#define RADIUS_ACCT_TERM_CAUSE_PORT_REINITIALIZED             21
#define RADIUS_ACCT_TERM_CAUSE_PORT_ADMINISTRATIVELY_DISABLED 22

/*
** The types of the attribute values in storage representation
*/
#define RADIUS_ATTR_VALUE_TYPE_STRING     1
#define RADIUS_ATTR_VALUE_TYPE_INTEGER    2
#define RADIUS_ATTR_VALUE_TYPE_IP_ADDR    3
#define RADIUS_ATTR_VALUE_TYPE_DATE       4

/*
** The LVL7 vendor-specific wireless attribute types
*/
#define WIRELESS_AP_LOCATION_VATTR                          101
#define WIRELESS_AP_MODE_VATTR                              102
#define WIRELESS_AP_PROFILE_ID_VATTR                        103
#define WIRELESS_AP_SWITCH_IP_VATTR                         104
#define WIRELESS_AP_RADIO_1_CHAN_VATTR                      105
#define WIRELESS_AP_RADIO_2_CHAN_VATTR                      106
#define WIRELESS_AP_RADIO_1_POWER_VATTR                     107
#define WIRELESS_AP_RADIO_2_POWER_VATTR                     108
#define WIRELESS_AP_EXPECTED_STANDALONE_SSID_VATTR          109
#define WIRELESS_AP_EXPECTED_STANDALONE_SECURITY_VATTR      110
#define WIRELESS_AP_EXPECTED_STANDALONE_WDS_VATTR           111
#define WIRELESS_AP_EXPECTED_STANDALONE_CHANNEL_VATTR       112
#define WIRELESS_AP_EXPECTED_STANDALONE_WIRED_VATTR         113
#define WIRELESS_MAC_ATUTHENTICATION_ACTION_VATTR           114
#define WIRELESS_CLINET_NICKNAME_VATTR                      115
#define WIRELESS_CLIENT_ACL_DN_VATTR                        120
#define WIRELESS_CLIENT_ACL_UP_VATTR                        121
#define WIRELESS_CLIENT_POLICY_DN_VATTR                     122
#define WIRELESS_CLIENT_POLICY_UP_VATTR                     123
#define LVL7_MAX_INPUT_OCTETS_VATTR                         124
#define LVL7_MAX_OUTPUT_OCTETS_VATTR                        125
#define LVL7_MAX_TOTAL_OCTETS_VATTR                         126
#define LVL7_CAPTIVE_PORTAL_GROUPS_VATTR                    127

/*
** The LVL7 vendor-specific wireless attribute values
*/
#define RADIUS_WIRELESS_AP_MODE_WS_MANAGED              1
#define RADIUS_WIRELESS_AP_MODE_STANDALONE              2
#define RADIUS_WIRELESS_AP_MODE_ROGUE                   3
#define RADIUS_WIRELESS_AP_RADIO_1_CHAN_AUTO            0
#define RADIUS_WIRELESS_AP_RADIO_2_CHAN_AUTO            0
#define RADIUS_WIRELESS_AP_RADIO_1_POWER_AUTO           0
#define RADIUS_WIRELESS_AP_RADIO_1_POWER_MINIMUM        1
#define RADIUS_WIRELESS_AP_RADIO_1_POWER_MAXIMUM        100
#define RADIUS_WIRELESS_AP_RADIO_2_POWER_AUTO           0
#define RADIUS_WIRELESS_AP_RADIO_2_POWER_MINIMUM        1
#define RADIUS_WIRELESS_AP_RADIO_2_POWER_MAXIMUM        100

/*
 ** Microsoft vendor specific attribute
*/
#define RADIUS_VENDOR_ID_MICROSOFT                      311

#define RADIUS_VENDOR_ATTR_MS_MPPE_SEND_KEY             16
#define RADIUS_VENDOR_ATTR_MS_MPPE_RECV_KEY             17

/*
 ** Wireless ISP - roaming (WISPr) vendor specific attributes
*/
#define RADIUS_VENDOR_ID_WISPR                          14122

#define RADIUS_VENDOR_ATTR_WISPR_BANDWIDTH_MAX_UP       7 
#define RADIUS_VENDOR_ATTR_WISPR_BANDWIDTH_MAX_DOWN     8


/* The type of attribute values for Tunnel Type attribute 
*/
#define RADIUS_TUNNEL_TYPE_VLAN     13


/* The type of attribute values for Tunnel Medium type attribute
*/
#define RADIUS_TUNNEL_MEDIUM_TYPE_802    6

#define RADIUS_TOKEN_LENGTH              32

#define RADIUS_AUTHENTICATOR_LENGTH      16

#define RADIUS_MS_KEY_SIZE               256

#define RADIUS_MS_KEY_SALT_LEN           2

#define RADIUS_VENDOR_ID_SIZE            4

/*
** Structures containing the definitions for attributes, value and vendor codes
** read from dictionary file.
*/
typedef struct radiusDictAttr_s
{
   struct radiusDictAttr_s *nextAttr;
   L7_char8                attrName[RADIUS_TOKEN_LENGTH + 1];
   L7_int32                attrId;
   L7_int32                attrType;
   L7_int32                vendorCode;
   L7_int32                vsAttrId;

}  radiusDictAttr_t;

/*
** Attribute-Value pair structure to contain the attribute definition
** as read from dictionary and value as given by Radius Client Services user.
*/
typedef struct radiusValuePair_s
{
   struct radiusValuePair_s *nextPair;
   L7_uint32                attrId;
   L7_uint32                attrType;
   L7_uint32                vendorCode;
   L7_uint32                vsAttrId;
   L7_uint32                intValue;
   L7_char8                 strValue[RADIUS_VALUE_LENGTH + 1];

} radiusValuePair_t;

typedef struct
{
   L7_uchar8   code;
   L7_uchar8   id;
   L7_ushort16 msgLength;
   L7_uchar8   authenticator[RADIUS_AUTHENTICATOR_LENGTH];

} radiusHeader_t;

typedef struct
{
   L7_uchar8 type;
   L7_uchar8 length;
   L7_uchar8 value[1];

} radiusAttr_t;

typedef struct
{
   radiusHeader_t header;
   L7_uchar8    attributes[1];

} radiusPacket_t;


L7_RC_t radiusBufferPoolInit();

radiusDictAttr_t *radiusAttributeByIdGet(L7_uint32 attrId);


/*********************************************************************
*
* @purpose  Register a routine to be called when a RADIUS response is
*           received from a server for a previously submitted request.
*
* @param registrar_ID @b((input)) routine registrar id (See L7_COMPONENT_ID_t)
* @param *notify @b((input)) pointer to a routine to be invoked upon a respones.
*                            Each routine has the following parameters:
*                            (status, correlator, *attributes, attributesLen)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusResponseRegister( L7_COMPONENT_IDS_t  registrar_ID,
                               L7_RC_t (*notify)(L7_uint32 status,
                                                   L7_uint32 correlator,
                                                   L7_uchar8 *attributes,
                                                   L7_uint32 attributesLen));

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
L7_RC_t radiusResponseDeregister( L7_COMPONENT_IDS_t componentId);

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
                               L7_COMPONENT_IDS_t componentID);

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
                          L7_COMPONENT_IDS_t componentID);

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
                              radiusValuePair_t *vpList);

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
                             radiusValuePair_t *vpList);

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
void radiusAttrValuePairListInit(radiusValuePair_t **vp);

/*************************************************************************
*
* @purpose  Add an attribute-value pair to the given list
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
                               L7_uint32 length);

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
                           L7_uint32 length);

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
                               radiusValuePair_t *vp);

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
                            L7_uint32 vsAttrId);

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
void radiusAttrValuePairListFree(radiusValuePair_t *vpList);

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
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
                            L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

/*********************************************************************
*
* @purpose Get the value of the time-out duration parameter
*
* @returns the current value of the timeOutDuration parameter
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusTimeOutDurationGet(dnsHost_t                            *hostAddr,
                                 L7_uint32                            *timeOut,
                                 L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
                                 L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

/*********************************************************************
*
* @purpose  Get the value of the radius-accounting mode parameter
*
* @returns L7_TRUE - if accounting has been enabled
* @returns L7_FALSE - if accounting has been disabled
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL radiusAcctModeGet(void);

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
L7_RC_t radiusAcctModeSet(L7_BOOL acctMode);

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
                                         L7_IP_ADDRESS_TYPE_t type);

/*********************************************************************
*
* @purpose Verify a RADIUS Accounting server is a configured server.
*
* @param serverAddr @b((input)) IP Address of the server to verify
*
* @returns L7_TRUE for a verified server
* @returns L7_FALSE for an un-verified server
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL radiusAcctServerAddrVerify(L7_uint32 serverAddr);

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
                                       L7_IP_ADDRESS_TYPE_t type);

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
                                     L7_uint32 *serverAddr);

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
                                             L7_IP_ADDRESS_TYPE_t *pType); 

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
L7_uchar8 * radiusAcctServerIPHostNameGet(L7_IP_ADDRESS_TYPE_t *pType);

/*********************************************************************
*
* @purpose Set the HostName or IP address of the Accounting server being used 
*
* @param L7_uchar8 *serverAddr @b((input)) pointer to the server address.
* @param L7_IP_ADDRESS_TYPE_t type @b((input))  Address type DNS or IP address 
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
                     L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName);

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
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 *portNum);

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
                          L7_IP_ADDRESS_TYPE_t type, L7_uint32 portNum);

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
                         L7_IP_ADDRESS_TYPE_t type, L7_uchar8 *sharedSecret);

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
                           L7_IP_ADDRESS_TYPE_t type, L7_BOOL *pVal );

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
                           L7_IP_ADDRESS_TYPE_t type, L7_uchar8 *secret );

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
                                 L7_uint32 *serverAddr);

/*********************************************************************
*
* @purpose Get the Host Name or IP address of the radius server corresponding
*           to the index input
*
* @param index      @b((input)) Index of the Auth Server
* @param serverAddr @b((output)) Host Name or IP address of the Auth Server
* @param type       @b((output))Address type DNS or IP address
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
                                         L7_IP_ADDRESS_TYPE_t *type);

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
                                       L7_IP_ADDRESS_TYPE_t *type);

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
                                      L7_IP_ADDRESS_TYPE_t *pType);

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
                                      L7_IP_ADDRESS_TYPE_t *pType);

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
                     L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName) ;

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
L7_RC_t radiusServerRemoveAll(void);


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
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 *portNum);

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
                L7_IP_ADDRESS_TYPE_t type, L7_uint32 portNum);

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
                                            L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

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
                                                   L7_BOOL   *pVal );

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
                                            L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
         L7_IP_ADDRESS_TYPE_t type, L7_uint32 *entryType);

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
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 entryType);

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
             L7_IP_ADDRESS_TYPE_t type, L7_uint32 mode);

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
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 *mode);

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
L7_RC_t radiusPrimaryServerGet(L7_uint32 index, L7_uint32 *pIpAddr);

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
                     L7_uchar8 *pServerAddr);

/*********************************************************************
*
* @purpose Retrieve the HostName if configured and
*     if no Hostname retrieve the IP address of the current active auth server
*
* @param serverAddr @b((output)) HostName or IP address of the radius server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusCurrentAcctHostNameServerGet(L7_uchar8 *pServerAddr);

/*********************************************************************
*
* @purpose Get the round trip time
*
* @param serverAddr @b((input)) HostName or IP Address of the radius server
*
* @returns the round trip time value in hundredths of a second
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatRTTGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusServerStatAccessReqGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusServerStatAccessRetransGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusServerStatAccessAcceptGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusServerStatAccessRejectGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusServerStatAccessChallengeGet(dnsHost_t *hostAddr);

/*********************************************************************
*
* @purpose  Retrieve the number of malformed RADIUS Access-Response packets
*           received from this server. Malformed packets include packets
*           with an invalid length. Bad authenticators or signature attributes
*           or unknown types are not included
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
L7_uint32 radiusServerStatMalformedAccessResponseGet(dnsHost_t *hostAddr);

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Response packets
*          containing invalid authenticators or signature attributes received
*          from this server.
*
* @param hostAddr @b((input))HostName or  IP Address of the radius server
*
* @returns the number of packets received
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatBadAuthGet(dnsHost_t *hostAddr);

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Access-Request packets
*          destined for this server that have not yet timed out or received
*          a response from this server
*
* @param hostAddr @b((input)) IP Address of the radius server
*
* @returns the number of packets pending to be sent
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatPendingReqGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusServerStatTimeoutsGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusServerStatUnknownTypeGet(dnsHost_t *hostAddr);

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS packets received from this server on
*          the authentication port that were dropped for some other reason.
*
* @param hostAddr@b((input)) HostName or IP Address of the radius server
*
* @returns the number of packets dropped
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusServerStatPktsDroppedGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusServerStatInvalidAddressesGet(void);

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
L7_uint32 radiusAcctServerStatRTTGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusAcctServerStatReqGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusAcctServerStatRetransGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusAcctServerStatResponseGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusAcctServerStatMalformedResponseGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusAcctServerStatBadAuthGet(dnsHost_t *hostAddr);

/*********************************************************************
*
* @purpose Retrieve the number of RADIUS Accounting-Request packets
*          destined for this server that have not yet timed out or received
*          a response from this server
*
* @param hostAddr @b((input)) HostName or IP Address of the accounting server
*
* @returns the number of packets pending to be sent
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerStatPendingReqGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusAcctServerStatTimeoutsGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusAcctServerStatUnknownTypeGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusAcctServerStatPktsDroppedGet(dnsHost_t *hostAddr);

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
L7_uint32 radiusAcctServerStatInvalidAddressesGet(void);

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
                            L7_uint32 *nasIdSize);

/*********************************************************************
*
* @purpose Clear statistics for all radius servers
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusStatsClear(void);

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
L7_RC_t radiusAttribute4Set(L7_BOOL mode, L7_uint32 ipAddr);

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
L7_RC_t radiusAttribute4Get(L7_BOOL *mode, L7_uint32 *ipAddr);

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
                   L7_uchar8 *digest);

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
                           L7_uchar8 len, 
                           L7_uchar8 *reqAuth, 
                           L7_uchar8 *secret, 
                           L7_uchar8 secret_len, 
                           L7_uchar8 *key, 
                           L7_uchar8 *resLen);

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
                        L7_int32  *outlen);



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
                                       L7_IP_ADDRESS_TYPE_t *type);

/*********************************************************************
*
* @purpose Add a radius server with a specific Host Name or IP address
*          
* @param serverAddr @b((input)) the HostName or IP address of the server 
*                               being added
* @param type @b((input))     Address type either ipv4 or dns
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerIPHostNameSet(L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName);

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
               L7_IP_ADDRESS_TYPE_t type, L7_uint32 *index);

/*********************************************************************
*
* @purpose Get the server Name of specified radius server.
*
* @param L7_uint32 unitIndex   @b((input))  the unit for this operation
* @param L7_char8 *serverAddr  @b((input)) Host name or IP address of the next
*                                   configured server
* @param L7_IP_ADDRESS_TYPE_t *pType
*                              @b((input)) Address Type.

* @param L7_char8 *serverName  @b((output)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*       
* @end
*
*********************************************************************/
L7_RC_t radiusServerHostNameGet(L7_uchar8 *serverAddr, 
               L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName);

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
                     L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName);

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
                                       L7_IP_ADDRESS_TYPE_t type);

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
L7_RC_t radiusAcctHostNameServerRemove(L7_uchar8 *serverAddr, 
                     L7_IP_ADDRESS_TYPE_t type);


/*********************************************************************
*
* @purpose Set the serverName attribute to the acct. server.
*          
* @param serverAddr @b((input)) the HostName or IP address to be set
* @param L7_IP_ADDRESS_TYPE_t type @b((input))     Address type either ipv4 or dns
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
L7_RC_t radiusServerAcctNameSet(L7_uchar8 *serverAddr, L7_IP_ADDRESS_TYPE_t type, 
                     L7_char8 *serverName) ;

/*********************************************************************
*
* @purpose Get the server Name of specified accounting server.
*
* @param L7_uint32 unitIndex   @b((input))  the unit for this operation
* @param L7_char8 *serverAddr  @b((input)) Host name or IP address of the next
*                                   configured server
* @param L7_IP_ADDRESS_TYPE_t *pType
*                              @b((input)) Address Type.

* @param L7_char8 *serverName  @b((output)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*       
* @end
*
*********************************************************************/
L7_RC_t radiusServerAcctHostNameGet(L7_uchar8 *serverAddr, 
               L7_IP_ADDRESS_TYPE_t type, L7_char8 *serverName);

/* TBD: ONly for debug purpose*
L7_RC_t radiusServerNameEntryDetailedGet(L7_uint32 ind,L7_char8 *serverName,L7_char8 *currentServer,L7_uint32 *count,void *currentEntry);
L7_RC_t radiusServerAcctNameEntryDetailedGet(L7_uint32 ind,L7_char8 *serverName,L7_uint32 *count);
*/

/*********************************************************************
*
* @purpose Get the Server Name and Address of the next named auth server
*          from the servers' list.  The address could be DNS Host Name or IP 
*          address.
*                    
* @param L7_char8 *name      @b((input)) Name of the configured Auth Server.
* @param L7_char8 *nextName      @b((input)) Name of the next configured Server.
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
L7_RC_t radiusNamedServerGetNext(L7_char8 *name, L7_char8 *nextName, L7_char8 *nextAddr ,L7_IP_ADDRESS_TYPE_t *pType);

/*********************************************************************
*
* @purpose Get the Server Name and Address of the next named acct server
*          from the servers' list.  The address could be DNS Host Name or IP 
*          address.
*                    
* @param L7_char8 *name      @b((input)) Name of the configured Auth Server.
* @param L7_char8 *nextName      @b((input)) Name of the next configured Server.
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
L7_RC_t radiusNamedAcctServerGetNext(L7_char8 *name, L7_char8 *nextName, L7_char8 *nextAddr,L7_IP_ADDRESS_TYPE_t *pType);

/*********************************************************************
*
* @purpose Get the server Name of specified radius server.
*
* @param L7_uint32 unitIndex   @b((input))  the unit for this operation
* @param L7_char8 *serverName  @b((input)) Name of the server.
* @param L7_char8 *pServerAddr @b((output)) Host name or IP address of the next
*                                   configured server
* @param L7_IP_ADDRESS_TYPE_t *pType
*                              @b((output)) Address Type.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*       
* @end
*
*********************************************************************/
L7_RC_t radiusNamedServerAddrGet(L7_char8 *serverName, L7_RADIUS_SERVER_USAGE_TYPE_t usageType, L7_uchar8 *pServerAddr, L7_IP_ADDRESS_TYPE_t *pType);

/*********************************************************************
*
* @purpose Get the server Name of specified acct server.
*
* @param L7_uint32 unitIndex   @b((input))  the unit for this operation
* @param L7_char8 *serverName  @b((input)) Name of the server.
* @param L7_char8 *pServerAddr @b((output)) Host name or IP address of the next
*                                   configured server
* @param L7_IP_ADDRESS_TYPE_t *pType
*                              @b((output)) Address Type.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*       
* @end
*
*********************************************************************/
L7_RC_t radiusNamedAcctServerAddrGet(L7_char8 *serverName, L7_char8 *pServerAddr, L7_IP_ADDRESS_TYPE_t *pType);

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
L7_RC_t radiusAccessRequestNamedSend(radiusValuePair_t *vpList,
                                L7_uint32 correlator,
                                L7_COMPONENT_IDS_t componentID,
                                L7_char8 *serverName,
                                L7_IP_ADDRESS_TYPE_t type,
                                L7_uchar8 *ipAddr);

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
L7_RC_t radiusAccountingNamedStart(L7_char8 *sessionId, 
                             L7_uint32 port, 
                             radiusValuePair_t *vpList,
                             L7_char8 *serverName,
                             L7_IP_ADDRESS_TYPE_t type,
                             L7_uchar8 *ipAddr);
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
                             L7_uchar8 *ipAddr);


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
                               L7_uchar8 *ipAddr);

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
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccessRequestMsgSend(radiusValuePair_t *vpList,
                                L7_uint32 correlator,
                                L7_COMPONENT_IDS_t componentID,
                                L7_char8 *serverName,
                                L7_IP_ADDRESS_TYPE_t type,
                                L7_uchar8 *ipAddr);

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
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccountingMsgStart(L7_char8 *sessionId, 
                              L7_uint32 port, 
                              radiusValuePair_t *vpList,
                              L7_char8 *serverName,
                              L7_IP_ADDRESS_TYPE_t type,
                              L7_uchar8 *ipAddr);

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
*
* @end
*
*************************************************************************/
L7_RC_t radiusAccountingMsgStop(L7_char8 *sessionId, 
                             L7_uint32 port, 
                             radiusValuePair_t *vpList,
                             L7_char8 *serverName,
                             L7_IP_ADDRESS_TYPE_t type,
                             L7_uchar8 *ipAddr);

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
L7_RC_t radiusUserAuthenticateMsg(L7_char8 *userName, 
                               L7_char8 *password, 
                               L7_char8 *state,
                               L7_uint32 correlator,
                               L7_COMPONENT_IDS_t componentID,
                               L7_char8 *serverName,
                               L7_IP_ADDRESS_TYPE_t type,
                               L7_uchar8 *ipAddr);

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
                     L7_uchar8 *nextAddr ,L7_IP_ADDRESS_TYPE_t *pType);

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
                     L7_char8 *nextAddr ,L7_IP_ADDRESS_TYPE_t *pType);

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
* @end
*
*********************************************************************/
L7_RC_t radiusNamedServerGet(L7_char8 *serverName);

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
L7_RC_t radiusNamedAcctServerGet(L7_char8 *serverName);

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
                                        L7_uint32           *priority);

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
L7_RC_t radiusHostNameServerPrioritySet(L7_uchar8            *serverAddr,
                                        L7_IP_ADDRESS_TYPE_t type, 
                                        L7_uint32            priority);

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
                                            L7_uint32 *priority);

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
                                    L7_uint32 *srvType);

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
                                        L7_uint32 *srvType);

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
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

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
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t *paramStatus);

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
                                L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

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
                                 L7_RADIUS_SERVER_USAGE_TYPE_t *usageType);

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
                                L7_RADIUS_SERVER_USAGE_TYPE_t  usageType);

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
                     L7_uint32 *count);

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
                     L7_uint32 *count);

/*********************************************************************
*
* @purpose Get the IP address of the specified DNS server.
*          
* @param dnsAddr @b((output)) Pointer to the dns address string of
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
                     radiusServerType_t type, L7_uint32 *ipAddr);

/*********************************************************************
*
* @purpose Get the local mask parameter of server
*          
* @param hostAddr    @b((input))  server IP address
* @param localmask   @b((output)) the value of localmask 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments localmask is a bitmap which tells whether parameters for 
*           specific server are global/local
*
* @end
*
*********************************************************************/

L7_RC_t radiusLocalConfigMaskGet(dnsHost_t hostAddr,
                                L7_uint32  *localMask);

#endif /* INCLUDE_RADIUS_API_H */

