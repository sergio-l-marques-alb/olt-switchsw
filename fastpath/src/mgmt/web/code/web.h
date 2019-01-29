/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/base/web.h
 *
 * @purpose Support of the EmWeb code
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 06/12/2000
 *
 * @author tgaunce
 * @end
 *
 **********************************************************************/

#ifndef WEB_H__
#define WEB_H__

#include "l7_product.h"
#include "usmdb_common.h"
#include "usmdb_lldp_api.h"
#include "usmdb_qos_voip_api.h"
#include "cli_web_mgr_api.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "ews_api.h"
#include "web_tabs.h"
#include "compdefs.h"
#include "usmdb_isdp_api.h"
#include "web_buffer.h"
#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_acl_api.h"
#include "usmdb_qos_cos_api.h"
#include "usmdb_qos_diffserv_api.h"
#include "usmdb_util_diffserv_api.h"
#endif

#define ENGLISH_LANG            101
#define FRENCH_LANG             102
#define GERMAN_LANG             103
#define ITALIAN_LANG            104
#define SPANISH_LANG            105
#define BRAZILIAN_LANG          106
#define PORTUGUESE_LANG         107
#define JAPANESE_LANG           108
#define KOREAN_LANG             109
#define SIMPLIFIED_CHINSE_LANG  110
#define TRADITIONAL_CHINSE_LANG 111
#define FURBISH_LANG            112

#define LANGUAGE_IN_USE         ENGLISH_LANG

#define USMWEB_READWRITE 1
#define USMWEB_READONLY  2
#define USMWEB_UNKNOWN   99

#define USMWEB_SUBMIT_SUBMIT   1
#define USMWEB_SUBMIT_CREATE   2
#define USMWEB_SUBMIT_REMOVE   3
#define USMWEB_SUBMIT_CANCEL   4
#define USMWEB_SUBMIT_UNKNOWN  5

#define USMWEB_ERROR_FAILED_TO_BLOCK_INTRFC  "Error: Failed to block interface\n"
#define USMWEB_ERROR_FAILED_TO_UNBLOCK_INTRFC "Error: Failed to unblock interface\n"
#define APPLET_IFLAG_STACKING_MASK      0x0000FF00
#define APPLET_IFLAG_FPS_SUPPORT        0x00000100
#define APPLET_IFLAG_FPS_MODE_SUPPORT   0x00000200
#define APPLET_IFLAG_FPS_MODE           0x00000400
#define APPLET_IFLAG_FPS_STACK_MODE     0x00000400
#define APPLET_IFLAG_STACK_MODE_ONLY    0x00000800
#define APPLET_IFLAG_STACK_MODE_SUPPORT 0x00001000
#define APPLET_IFLAG_STACK_MODE_HIGIG   0x00002000
#define APPLET_IFLAG_STACK_MODE_10G     0x00004000

#define USMWEB_PAGE_NUMBER_OF_ROWS      20
#define USMWEB_PAGES_PER_BLOCK          10

#define USMWEB_BUFFER_SIZE_64    64
#define USMWEB_BUFFER_SIZE_128   128
#define USMWEB_BUFFER_SIZE_256   256
#define USMWEB_BUFFER_SIZE_512   512
#define USMWEB_BUFFER_SIZE_1024  1024
#define USMWEB_BUFFER_SIZE_4096  4096
#define USMWEB_BUTTON_SIZE  32
#define USMWEB_IPADDR_SIZE  16     /*  111.222.333.444\0    */
#define USMWEB_MACADDR_SIZE 18     /*  aa:bb:cc:dd:ee:ff\0  */
#define USMWEB_VLAN_MACADDR_SIZE 24     /*  aa:bb:cc:dd:ee:ff\0  */

#define USMWEB_TRUNK_SLOT   0x100   /* Indicator that this slot represent the trunks */

#define USMWEB_IDX_DISABLE  0       /* Index of the select for any disables */
#define USMWEB_IDX_ENABLE   1       /* Index of the select for any enables */

#define USMWEB_IDX_ADMIT_ALL_FRAMES  0      /* Index of the select for admit all frames */
#define USMWEB_IDX_ADMIT_VLAN_ONLY   1      /* Index of the select for admit only vlan frames */

#define USMWEB_INPUT_TYPE_INTEGER    1
#define USMWEB_INPUT_TYPE_IPADDR     2

#define USMWEB_RADIUS_DELAY             1200    /* milliseconds */
#define USMWEB_RADIUS_DELAY_INCREMENT    200    /* milliseconds */

#define USMWEB_TMOUT_SIZE     4
#define USMWEB_SERVER_ADDR_SIZE    40    /* aaaa:bbbb:cccc:dddd:eeee:ffff:gggg:hhhh\0 */
#define USMWEB_PATH_SIZE      33
#define USMWEB_FILENAME_SIZE  33
#define USMWEB_USERNAME_SIZE  9
#define USMWEB_PWD_SIZE       9
#define USMWEB_GARPJOIN_SIZE  4
#define USMWEB_GARPLEAVE_SIZE 4
#define USMWEB_GARPLVALL_SIZE 5
#define USMWEB_COMMNAME_SIZE  17
#define USMWEB_TRUNKNAME_SIZE 15
#define USMWEB_VLANNAME_SIZE  32
#define USMWEB_MEMBERS_SIZE   15
#define USMWEB_ERROR_MSG_SIZE 1024
#define USMWEB_APP_DATA_SIZE  32
#define USMWEB_APP_BUF_SIZE   80
#define USMWEB_APP_SMALL_DATA_SIZE  15

#define USMWEB_VLAN_TYPE_SIZE  12
#define USMWEB_VLAN_NAME_SIZE  16

#define USMWEB_VRRPVALUE_SIZE    16
#define USMWEB_VRRPAUTHDATA_SIZE 8
#define USMWEB_VRRP_VMAC_SIZE    6

#define USMWEB_SLOT_PORT_SIZE  6
#define USMWEB_UNIT_SLOT_PORT_SIZE  9

#define L7_MAX_TRAP_COMM      6

/* QOS */
#define USMWEB_NEW_POLICY_VALUE  0x0FFFFFFF

/* BGP */
#define USMWEB_MAX_ENTRIES_PER_PAGE 15

/* PIM-DM */
#define USMWEB_MIN_HELLO_INTERVAL       1
#define USMWEB_MAX_HELLO_INTERVAL       65535
#define L7_MAX_NEIGHBORS        255
#define USMWEB_TIME_SIZE 8

/* DVMRP */
#define USMWEB_MIN_INTERFACE_METRIC     1
#define USMWEB_MAX_INTERFACE_METRIC     31

/* Wireless */
#define USMWEB_AP_PROFILE_COPY_ID          (L7_WIRELESS_MAX_AP_PROFILES + 1)

#define USMWEB_BUFFER_SIZE_16   16

extern L7_uint32 usmWebUserAccessGet(EwsContext ewsContext);
extern L7_BOOL usmWebConvertMac(L7_uint32 numBytes, L7_uchar8 * buf, L7_uchar8 * mac);
#undef strdup
extern char *strdup(const char * srcString);
extern void  usmWebUtilCvtTime2String(L7_char8 * buffer, L7_uint32 timeticks);
extern L7_char8 *usmWebAgentCommClientCommunityGet(L7_uint32 unit, L7_uint32 index);
extern L7_char8 *usmWebAgentCommAccessStatusGet(L7_uint32 unit, L7_uint32 index);
extern L7_uint32 usmWebNumActiveDot1qInterfacesGet(L7_uint32 unit);
extern L7_char8 *usmWebPortOptionsGet(L7_uint32 unit);
extern L7_uint32 usmWebGarpErrorGet();
extern void usmWebGarpErrorSet(L7_uint32 error);
extern L7_RC_t usmWebToDot1sBridgeId(L7_uchar8 * buf, L7_uchar8 * temp, L7_uint32 val);
extern L7_RC_t usmWebToDot1sPortId(L7_uchar8 * buf, L7_uint32 val);
extern L7_uint32 usmWebDot1sMstiListFirstGet(L7_uint32 unit, L7_uint32 * buf, L7_uint32 * val);
extern L7_RC_t usmWebDot1sIfSpanningTreeModeSet(L7_uint32 unit, L7_uint32 intIfNum, L7_BOOL stp_mode);
extern L7_char8 *usmWebIGMPVlanInfoGet(L7_uint32 unit, L7_uchar8 family);
extern L7_RC_t usmWebNumSFPsGet(L7_uint32 * numSFPs);
extern void javaTraceHelp(void);
extern void javaTrace(L7_uint32 jatOptions, L7_uint32 jatCounter, L7_uint32 jatMode);
extern void javaAppletTraceMsg(L7_char8 * msg);
extern L7_char8 *usmWebVlanMcastRtrInfoGet(L7_uint32 unit,L7_uint32 intIfNum, L7_uint32 vid, L7_uchar8 family);
extern L7_char8 *usmWebImageStatusGet(L7_uint32 unit);
extern L7_char8 *usmWebAutoVoipModeGet(L7_uint32 intIfNum);
extern L7_char8 *usmWebAutoVoipBandwidthGet(L7_uint32 intIfNum);
extern L7_char8 *usmWebAutoVoipCosQueueGet(L7_uint32 intIfNum);
extern L7_char8 *usmWebAutoVoipInterfaceGet(L7_uint32 IntIfNum);
extern L7_BOOL usmWebIsValidLldpRemoteIntf(L7_uint32 intIfNum);
extern L7_RC_t usmWebLldpLocalIntfNextGet(L7_uint32 intIfNum, L7_uint32 * nextIntf);
extern L7_RC_t usmWebLldpLocalIntfFirstGet(L7_uint32 * firstIntf);
extern L7_BOOL usmWebIsValidLldpLocalIntf(L7_uint32 intIfNum);
extern L7_char8 *usmWebSourcePortsGet(L7_uint32 sessionId);
extern L7_char8 *usmWebPortDisplayHelp(L7_uint32 intIfNum);
extern L7_char8 *usmWebIntegerToString(L7_int32 val);
extern L7_char8 *usmWebIntegerRangeHelp(L7_int32 min, L7_int32 max);
extern L7_char8 *usmWebUnsignedIntegerRangeHelp(L7_uint32 min, L7_uint32 max);
extern L7_char8 *usmWebAlphanumericCharacterRangeHelp(L7_uint32 min, L7_uint32 max);
extern L7_char8 *usmWebCharacterRangeHelp(L7_uint32 min, L7_uint32 max);
extern L7_char8 *usmWebIPAddrHostnameRangeHelp(L7_uint32 min, L7_uint32 max);
extern L7_char8 *usmWebTimeIntervalRangeHelp(L7_uint32 min, L7_uint32 max, L7_uint32 count);

#define USMWEB_STATIC_BUFFER_SIZE       12000

/* One structure array per language known. */
typedef struct nls_strings
{
  L7_uint32 id;                                                                /* String ID */
  L7_char8 * string;                                                            /* string in english */
} usmNLSString_t;

typedef struct
{
  L7_uint32 index;
  usmDbTrapLogEntry_t entry;
} usmWebTrapLogEntry_t;

typedef struct
{
  L7_uint32 err_flag;
  L7_char8 msg[USMWEB_ERROR_MSG_SIZE];

} usmWeb_ErrMsg_t;

typedef struct
{
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
} usmWeb_USP_t;

typedef struct
{
  L7_uint32 data[USMWEB_APP_DATA_SIZE];
  L7_uint32 tabIndex[TAB_ROWS_MAX];
  L7_char8 buf[USMWEB_APP_BUF_SIZE];
  L7_char8 buf2[USMWEB_APP_BUF_SIZE];
  usmWeb_ErrMsg_t err;
} usmWeb_AppInfo_t;

typedef struct
{
  L7_uint32 vlanID;
  L7_uchar8 macaddr[6];
} usmWeb_FilterEntry_t;

typedef struct
{
  usmdbMfdbEntry_t mfdbEntry;
  usmdbMfdbUserInfo_t userEntry;
  L7_BOOL match_exact;
} usmWeb_MfdbEntry_t;

/* Used in getting PIM-DM Neighbor Information*/
typedef struct pimdmNeighborInfo_s
{
  L7_inet_addr_t neighborIp;
  L7_char8 neighborUptime[USMWEB_BUFFER_SIZE_256];
  L7_char8 neighborExpiryTime[USMWEB_BUFFER_SIZE_256];
} pimdmNeighborInfo_t;

/* Used in getting PIM-SM Neighbor Information*/
typedef struct pimsmNeighborInfo_s
{
  L7_inet_addr_t neighborIp;
  L7_char8 neighborUptime[16];
  L7_char8 neighborExpiryTime[16];
} pimsmNeighborInfo_t;

/* Used in getting DVMRP Tables Information */
typedef struct dvmrpRtInfo_s
{
  L7_uint32 rtSrc;
  L7_uint32 rtMask;
  L7_uint32 rtUpStreamNbr;
  L7_uint32 rtIndex;
  L7_ushort16 rtMetric;
  L7_ulong32 rtExpTime;
  L7_ulong32 rtUpTime;
} dvmrpRtInfo_t;

typedef struct dvmrpRtNextHop_s
{
  L7_uint32 nextHopSrc;
  L7_uint32 nextHopMask;
  L7_uint32 nextHopIndex;
  L7_uint32 nextHopType;
} dvmrpRtNextHop_t;

typedef struct dvmrpPrune_s
{
  L7_uint32 pruneGrp;
  L7_uint32 pruneSrc;
  L7_uint32 pruneSrcMask;
  L7_ulong32 pruneExpTime;
} dvmrpPrune_t;

typedef struct pmlEntry_s
{
  L7_uint32 intIfNum;
  L7_enetMacAddr_t macAddr;
  L7_ushort16 vlanId;
} pmlEntry_t;

typedef struct
{
  L7_uint32 index;
  L7_enetMacAddr_t macAddr;
} macIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_uchar8 radioIndex;
  L7_enetMacAddr_t macAddr;
} radioIterator_t;

typedef struct
{
  L7_uint32 vlanId;
  L7_uchar8 macAddr[6];
} usmWeb_macBasedVlanEntry_t;

typedef struct lldp_mgmtAddrEntry_s
{
  lldpIANAAddrFamilyNumber_t family;
  L7_uchar8 address[LLDP_MGMT_ADDR_SIZE_MAX];
  L7_uchar8 addrlen;
} lldp_mgmtAddrEntry_t;

typedef struct
{
  L7_uint32 ipAddr;
  L7_uint32 subnet;
  L7_uint32 vlanId;
} usmWeb_ipSubnetBasedVlanEntry_t;

typedef struct 
{
  L7_uint32  intIfNum;
  L7_uint32  appType;
} usmWeb_lldpMedNetAppTypeEntry_t;

typedef struct 
{
  L7_uint32  intIfNum;
  L7_uint32  subType;
} usmWeb_lldpMedLocationSubTypeEntry_t;

typedef struct 
{
  L7_uint32  intIfNum;
  L7_uint32  remIndex;
  L7_uint32  timeStamp;
  L7_uint32  policyType;
} usmWeb_lldpMedRemNetPolicyAppTypeEntry_t;

typedef struct 
{
  L7_uint32  intIfNum;
  L7_uint32  remIndex;
  L7_uint32  timeStamp;
} usmWeb_lldpRemEntry_t;

typedef struct
{
  L7_char8 host[USMWEB_BUFFER_SIZE_256];
  L7_IP_ADDRESS_TYPE_t type;
} radiushostIterator_t;


typedef struct
{
  L7_uint32 index;
  L7_uint32 id;
} idIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_uint32 bldngNum;
} bldngIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_uint32 bldngNum;
  L7_uint32 flrNum;
} flrIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_uint32 bldngNum;
  L7_uint32 flrNum;
  L7_enetMacAddr_t macAddr;
} apIterator_t;

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
typedef enum
{
  CP_GET_ID_FOREGROUND_COLOR,
  CP_GET_ID_BACKGROUND_COLOR,
  CP_GET_ID_SEPARATOR_COLOR,
  CP_GET_ID_ACCEPT,
  CP_GET_ID_ACCOUNT,
  CP_GET_ID_BTITLE,
  CP_GET_ID_FONT_LIST,
  CP_GET_ID_TITLE,
  CP_GET_ID_USER,
  CP_GET_ID_PASSWORD,
  CP_GET_ID_INSTRUCT,
  CP_GET_ID_MSG,
  CP_GET_ID_WTITLE,
  CP_GET_ID_WELCOME,
  CP_GET_ID_REFRESH,
  CP_GET_ID_NOSEL_IMAGE,
  CP_GET_ID_IMAGES,
  CP_GET_ID_DEFAULT_ACCOUNT_IMAGE,
  CP_GET_ID_DEFAULT_BACKGROUND_IMAGE,
  CP_GET_ID_DEFAULT_BRANDING_IMAGE,
  CP_GET_ID_DEFAULT_LOGOUT_SUCCESS_BACKGROUND_IMAGE,
  CP_GET_ID_ACCOUNT_IMAGE,
  CP_GET_ID_BACKGROUND_IMAGE,
  CP_GET_ID_BRANDING_IMAGE,
  CP_GET_ID_SCRIPT,
  CP_GET_ID_POPUP,
  CP_GET_ID_LOGOUT_URL,
  CP_GET_ID_LOGOUT_BTITLE,
  CP_GET_ID_LOGOUT_TITLE,
  CP_GET_ID_LOGOUT_CONTENT,
  CP_GET_ID_LOGOUT_BUTTON,
  CP_GET_ID_LOGOUT_CONFIRM,
  CP_GET_ID_WELCOME_URL,
  CP_GET_ID_LOGOUT_SUCCESS_URL,
  CP_GET_ID_LOGOUT_SUCCESS_BTITLE,
  CP_GET_ID_LOGOUT_SUCCESS_TITLE,
  CP_GET_ID_LOGOUT_SUCCESS_CONTENT,
  CP_GET_ID_LOGOUT_SUCCESS_BACKGROUND_IMAGE
} usmWeb_cpGenericGetId_t;

typedef enum
{
  CP_FORM_OBJ_UID,
  CP_FORM_OBJ_PWD,
  CP_FORM_OBJ_MSG,
  CP_FORM_OBJ_AUP,
  CP_FORM_OBJ_USER_LOGOUT
} usmWeb_cpFormObjectId_t;

typedef struct
{
  L7_uint32 index;
  L7_char8 user[USMWEB_APP_DATA_SIZE+1];
} userIterator_t;

#endif

extern void usmWebInputFieldSetFailedErrMsg(L7_char8 * tokMsg, usmWeb_AppInfo_t * appInfo);
extern void usmWebInputFieldGetFailedErrMsg(L7_char8 * tokMsg, usmWeb_AppInfo_t * appInfo);
extern void usmWebInputFieldGenericErrMsg(L7_char8 * tokMsg, usmWeb_AppInfo_t * appInfo);
extern L7_RC_t usmWebInputFieldCheck(EwsContext context, L7_char8 * fieldName, L7_char8 * tokMsg,
                                     usmWeb_AppInfo_t * appInfo, L7_uint32 lowerLimit, L7_uint32 upperLimit);

extern L7_BOOL usmWebIsRestrictedFilterMac(L7_uchar8 * macaddr);
extern L7_char8 *usmWebLldpMedRemoteInfo(usmWeb_lldpRemEntry_t lldpRemEntry_itr);

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
L7_char8 *usmWebCPGenericGet(EwsContext context, usmWeb_cpGenericGetId_t id);
L7_BOOL usmWebCPFormObjectEnabled(EwsContext context, usmWeb_cpFormObjectId_t id);
L7_char8 *usmWebCPLangLinksGet(EwsContext context);
#endif

/* web_utils.c */
L7_uint32 usmWebL7EncapToEncapTypeGet (L7_int32 val, L7_int32 def);
L7_uint32 usmWebDvlanTagEtherTypeGet (L7_uint32 val, L7_uint32 def);
L7_uint32 usmWebRip2ConfReceiveTypeGet (L7_uint32 val, L7_uint32 def);
L7_uint32 usmWebRip2ConfSendTypeGet (L7_uint32 val, L7_uint32 def);
L7_uint32 usmWebL7EnDisToEnDisGet (L7_int32 val, L7_uint32 def);
L7_uint32  usmWebL7TrueFalseToEnDisGet (L7_int32 val, L7_uint32 def);
L7_uint32 usmWebL7TrueFalseToTrueFalseGet (L7_int32 val, L7_uint32 def);
L7_uint32  usmWebL7AdminModeToEnDisGet (L7_int32 val, L7_uint32 def);
L7_int32 usmWebEnDisToL7EnDisGet (L7_uint32 val, L7_int32 def);
L7_int32 usmWebEnStartToL7EnBoolGet (L7_uint32 val, L7_int32 def);
L7_uint32 usmWebL7BoolToEnStartGet (L7_int32 val, L7_uint32 def);
void usmWebFormIntSet (L7_uint32 * valPtr, L7_uint8 * statPtr, L7_uint32 value);
void usmWebFormStringSet (L7_char8 * * valPtr, L7_uint8 * statPtr, L7_char8 * value);
void usmWebFormIpAddrSet (L7_char8 * * valPtr, L7_uint8 * statPtr, L7_uint32 value);
L7_uint32 web2DefLogSeverity (L7_uchar8 val, L7_uint32 def);
L7_uint32 web2DefEtherType (L7_ushort16 val, L7_uint32 def);
void usmWebAppInfoCatErrMsg (usmWeb_AppInfo_t * appInfop, char * prefix, char * fmt, ...);
L7_char8 *usmWebEwsContextSendReply (EwsContext context, L7_BOOL create, usmWeb_AppInfo_t * appInfop, char * file);
L7_uint32 usmWebMaskLength(L7_uint32 mask);
L7_RC_t webCheckIfInteger(L7_char8 * buf);
L7_ushort16 usmWebPasswdMinLenGet();

L7_RC_t usmWebConvertHexToUnicode(L7_char8 *in, L7_char8 *out);
L7_RC_t usmWebConvertAsciiToUnicode(L7_char8 *in, L7_char8 *out, L7_uint32 inLen, L7_uint32 outLen);
L7_char8 *usmWebGetDefaultVLANID();
L7_char8 *usmWebSubmitDescriptionHelp();
L7_char8 *usmWebRefreshDescriptionHelp();
L7_char8 *usmWebCancelDescriptionHelp();


#endif
