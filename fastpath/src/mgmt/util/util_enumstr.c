/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/util/util_enumstr.c
 *
 * @purpose string utilites
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/01/2007
 *
 * @author  Rama Sasthri kristipati
 * @end
 *
 **********************************************************************/

#include <stdlib.h>

#include "datatypes.h"
#include "commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_web.h"
#include "strlib_switching_web.h"

#include "l7_common.h"
#include "acl_exports.h"
#include "cli_web_exports.h"
#include "cos_exports.h"
#include "dot1s_exports.h"
#include "fdb_exports.h"
#include "osapi.h"
#include "defaultconfig.h"
#include "l3_commdefs.h"
#include "wireless_commdefs.h"
#include "util_enumstr.h"

#include "usmdb_unitmgr_api.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_poe_api.h"
#include "l3_mcast_commdefs.h"
#include "l3_bgp_commdefs.h"
#include "l3_comm_structs.h"
#include "ipv6_commdefs.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "strlib_qos_web.h"
#include "strlib_base_cli.h"
#include "strlib_wireless_cli.h"
#include "strlib_bgp_cli.h"
#include "strlib_routingv6_common.h"
#include "strlib_routingv6_cli.h"
#include "strlib_routing_web.h"
#include "strlib_common_web.h"
#include "strlib_ip_mcast_common.h"
#include "compdefs.h"
#include "ews.h"
#include "cliutil.h"

#include "dot1q_exports.h"
#include "sntp_exports.h"

#include "clicommands.h"

#include "strlib_wireless_common.h"
#ifdef L7_MGMT_SECURITY_PACKAGE
#include "strlib_security_common.h"
#endif /* L7_MGMT_SECURITY_PACKAGE */

/*********************************************************************
*
* @purpose  To return either enable or disable
*
* @param L7_int32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @returns String "Enable" or "Disable"
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilEnableDisableGet(L7_int32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_ENABLE:
      return pStrInfo_common_Enbl_1;
      /* pass-through */
    case L7_DISABLE:
      return pStrInfo_common_Dsbl_1;
      /* pass-through */
    default:
      break;
  }
  return (L7_NULLPTR == def) ? pStrInfo_common_Dsbl_1 : def;
}

/*********************************************************************
*
* @purpose  To return either enabled or disabled
*
* @param L7_int32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @returns String "Enabled" or "Disabled"
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilEnabledDisabledGet(L7_int32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_ENABLE:
      return pStrInfo_common_Enbld;
      /* pass-through */
    case L7_DISABLE:
      return pStrInfo_common_Dsbld;
      /* pass-through */
    default:
      break;
  }
  return (L7_NULLPTR == def) ? pStrInfo_common_Dsbld : def;
}

/*********************************************************************
*
* @purpose  To return string "Active" or "Inactive"
*
* @param L7_int32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilActiveInactiveGet(L7_int32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_ACTIVE:
      return pStrInfo_common_WsActive;
      /* pass-through */
    case L7_INACTIVE:
      return pStrInfo_common_WsInactive;
      /* pass-through */
    default:
      break;
  }
  return (L7_NULLPTR == def) ? pStrInfo_common_WsInactive : def;
}

/*********************************************************************
*
* @purpose  To return string Encapsulation type
*
* @param L7_int32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilEncapTypeGet(L7_int32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_ENCAP_ETHERNET:
      return pStrInfo_common_EtherNet;
      /* pass-through */
    case L7_ENCAP_802:
      return pStrInfo_common_Snap;
      /* pass-through */
    default:
      break;
  }
  return (L7_NULLPTR == def) ? pStrInfo_common_EtherNet : def;
}

/*********************************************************************
*
* @purpose  To return string Ethertype
*
* @param L7_int32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilDvlanTagEtherTypeGet(L7_int32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_DVLANTAG_802_1Q_ETHERTYPE:
      return pStrInfo_base_Dot1qTag;
      /* pass-through */
    case L7_DVLANTAG_VMAN_ETHERTYPE:
      return pStrInfo_base_VmanTag;
      /* pass-through */
    default:
      break;
  }
  return (def == L7_NULLPTR) ? pStrInfo_base_CustomTag : def;
}

/*********************************************************************
*
* @purpose  Convert an OSPF network type to a string
*
* @param ospfNetworkType  (L7_OSPF_INTF_BROADCAST or L7_OSPF_INTF_PTP)
* @param L7_char8 *default
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_uchar8 *strUtilOspfNetworkTypeGet(L7_uint32 ospfNetworkType,
                                     L7_char8 *def)
{
  if (ospfNetworkType == L7_OSPF_INTF_BROADCAST)
    return pStrInfo_common_Bcast;
  else if (ospfNetworkType == L7_OSPF_INTF_PTP)
    return pStrInfo_common_P2P;

  return (def == L7_NULLPTR) ? pStrInfo_common_NotApplicable : def;
}

/*********************************************************************
*
* @purpose  To return string Authentication type
*
* @param L7_int32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilAuthTypeGet(L7_int32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_AUTH_TYPE_NONE:
      return pStrInfo_common_None_1;
      /* pass-through */
    case L7_AUTH_TYPE_SIMPLE_PASSWORD:
      return pStrInfo_common_Simple_1;
      /* pass-through */
    case L7_AUTH_TYPE_MD5:
      return pStrInfo_common_Encrypt_2;
      /* pass-through */
    default:
      break;
  }
  return (def == L7_NULLPTR) ? pStrInfo_common_NotApplicable : def;
}

/*********************************************************************
*
* @purpose  To return string representing rip2 config send type
*
* @param L7_uint32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilRip2ConfSendTypeGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_INTF_BROADCAST:
      return pStrInfo_common_None_1;
      /* pass-through */
    case L7_OSPF_INTF_NBMA:
      return pStrInfo_common_Rip_1;
      /* pass-through */
    case L7_OSPF_INTF_PTP:
      return pStrInfo_common_Rip_2;
      /* pass-through */
    case L7_OSPF_INTF_VIRTUAL:
      return pStrInfo_common_Both;
      /* pass-through */
    default:
      break;
  }
  return def ? def : pStrInfo_common_None_1;
}

/*********************************************************************
*
* @purpose  To return string representing rip2 config receive type
*
* @param L7_uint32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilRip2ConfReceiveTypeGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_RIP_CONF_RECV_DO_NOT_RECEIVE:
      return pStrInfo_common_None_1;
      /* pass-through */
    case L7_RIP_CONF_RECV_RIPV1:
      return pStrInfo_common_Rip_1;
      /* pass-through */
    case L7_RIP_CONF_RECV_RIPV2:
      return pStrInfo_common_Rip2;
      /* pass-through */
    case L7_RIP_CONF_RECV_RIPV1_RIPV2:
      return pStrInfo_common_Both;
      /* pass-through */
    default:
      break;
  }
  return def ? def : pStrInfo_common_None_1;
}

#ifdef L7_WIRELESS_PACKAGE
/*********************************************************************
*
* @purpose  To return string representing AP failure type
*
* @param L7_uint32 type
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilWdmAPFailureTypeGet(L7_uint32 type, L7_char8 *def)
{
  switch (type)
  {
    case L7_WDM_AP_FAILURE_NONE:
      return pStrInfo_common_None_1;
      /* pass-through */
    case L7_WDM_AP_FAILURE_NONE_AP_RELINK:
      return pStrErr_common_ApFailureApRelink;
      /* pass-through */
    case L7_WDM_AP_FAILURE_LOCAL_AUTH:
      return pStrErr_common_ApFailureLocalAuth;
      /* pass-through */
    case L7_WDM_AP_FAILURE_RADIUS_AUTH:
      return pStrErr_common_ApFailureRadiusAuth;
      /* pass-through */
    case L7_WDM_AP_FAILURE_RADIUS_UNREACH:
      return pStrErr_common_ApFailureRadiusUnreach;
      /* pass-through */
    case L7_WDM_AP_FAILURE_RADIUS_CHALLENGED:
      return pStrErr_common_ApFailureRadiusChallenge;
      /* pass-through */
    case L7_WDM_AP_FAILURE_RADIUS_RESP_INVALID:
      return pStrErr_common_ApFailureInvalidRadiusResp;
      /* pass-through */
    case L7_WDM_AP_FAILURE_NO_DB_ENTRY:
      return pStrErr_common_ApFailureNoDbEntry;
      /* pass-through */
    case L7_WDM_AP_FAILURE_NO_SET_MANAGED:
    case L7_WDM_AP_FAILURE_AP_NOT_MANAGED:
      return pStrErr_common_ApFailureNotManaged;
      /* pass-through */
    case L7_WDM_AP_FAILURE_PROFILE_ID_INVALID:
      return pStrErr_common_ApFailureInvalidProfileId;
      /* pass-through */
    case L7_WDM_AP_FAILURE_PROFILE_HW_TYPE_MISMATCH:
      return pStrErr_common_ApFailureProfileMismatch;
      /* pass-through */
    case L7_WDM_AP_FAILURE_RADIUS_MSG_SEND_FAILED:
      return pStrErr_common_ApFailureRadiusMsgSendFailed;
      /* pass-through */
    case L7_WDM_AP_FAILURE_CODE_IMAGE_NOT_AVAILABLE:
      return "AP Image Not Available";
      /* pass-through */
    case L7_WDM_AP_FAILURE_LAST:
      return pStrInfo_common_Last;
      /* pass-through */
    default:
      break;
  }
  return def ? def : pStrInfo_common_None_1;
}

/*********************************************************************
*
* @purpose  To return RF scan status string
*
* @param L7_uint32 status
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strWdmRFScanStatusGet(L7_uint32 status, L7_char8 *def)
{
  switch (status)
  {
    case L7_WDM_AP_STATUS_MANAGED:
      return pStrInfo_common_Managed;
      /* pass-through */
    case L7_WDM_AP_STATUS_UNKNOWN:
      return pStrInfo_common_Unknown;
      /* pass-through */
    case L7_WDM_AP_STATUS_STANDALONE:
      return pStrInfo_common_Standalone;
      /* pass-through */
    case L7_WDM_AP_STATUS_ROGUE:
      return pStrInfo_common_Rogue;
      /* pass-through */
    default:
      break;
  }
  return def ? def : pStrInfo_common_None_1;
}

/*********************************************************************
*
* @purpose  To return associated client status.
*
* @param L7_uint32 status
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strClientStatusEntryGet(L7_uint32 status, L7_char8 *def)
{
  switch (status)
  {
    case L7_WDM_CLIENT_ASSOC:
      return pStrInfo_common_ApProfileStateAssociated;
      /* pass-through */
    case L7_WDM_CLIENT_AUTH:
      return pStrInfo_common_ClientAuth;
      /* pass-through */
    case L7_WDM_CLIENT_DISASSOC:
      return pStrInfo_common_ClientDisassoc;
      /* pass-through */
    default:
      break;
  }
  return def ? def : pStrInfo_common_None_1;
}
#endif  /* L7_WIRELESS_PACKAGE */

L7_char8 *strUtilLinkStateGet(L7_int32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_UP:
      return pStrInfo_common_LinkUp;
      /* pass-through */
    case L7_DOWN:
      return pStrInfo_common_LinkDown;
      /* pass-through */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Blank;
}

L7_char8 *strUtilTransferModeGet(L7_TRANSFER_TYPES_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_TRANSFER_XMODEM:
      return pStrInfo_base_Xmodem;
      /* pass-through */
    case L7_TRANSFER_TFTP:
      return pStrInfo_base_Tftp;
      /* pass-through */
#ifdef L7_MGMT_SECURITY_PACKAGE
    case L7_TRANSFER_SFTP:
      return pStrInfo_security_Sftp_2;
      /* pass-through */
    case L7_TRANSFER_SCP:
      return pStrInfo_security_Scp_2;
      /* pass-through */
#endif /* L7_MGMT_SECURITY_PACKAGE */
    case L7_TRANSFER_FTP:
      return pStrInfo_base_Ftp;
      /* pass-through */
    case L7_TRANSFER_YMODEM:
      return pStrInfo_base_Ymodem;
      /* pass-through */
    case L7_TRANSFER_ZMODEM:
      return pStrInfo_base_Zmodem;
#if 0
    case L7_TRANSFER_LOCAL:
      return CLI_FLASHPREFIX;
#endif
      /* pass-through */
    default:
      break;
  }

  return (def == L7_NULLPTR) ? pStrInfo_common_Unknown_2 : def;
}

L7_char8 *strUtilFileTypeGet(L7_FILE_TYPES_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_FILE_TYPE_CODE:
      return pStrInfo_base_Code_2;
      /* pass-through */
    case L7_FILE_TYPE_CONFIG:
      return pStrInfo_base_Cfg_1;
      /* pass-through */
    case L7_FILE_TYPE_TXTCFG:
      return "Text Configuration";
      /* pass-through */
    case L7_FILE_TYPE_ERRORLOG:
      return pStrInfo_base_Log;
      /* pass-through */
    case L7_FILE_TYPE_VALGLOG:
      return pStrInfo_base_ValgrindLog;
      /* pass-through */
    case L7_FILE_TYPE_SYSTEM_MSGLOG:
      return pStrInfo_base_BufedLog;
      /* pass-through */
    case L7_FILE_TYPE_TRAP_LOG:
      return pStrInfo_base_TrapLog;
      /* pass-through */
    case L7_FILE_TYPE_TRACE_DUMP:
      return pStrInfo_base_TraceDump;
      /* pass-through */
    case L7_FILE_TYPE_SSHKEY_RSA1:
      return pStrInfo_base_SshRsa1Key;
      /* pass-through */
    case L7_FILE_TYPE_SSHKEY_RSA2:
      return pStrInfo_base_SshRsa2Key;
      /* pass-through */
    case L7_FILE_TYPE_SSHKEY_DSA:
      return pStrInfo_base_SshDsaKey;
      /* pass-through */
    case L7_FILE_TYPE_SSLPEM_ROOT:
      return pStrInfo_base_SslRootCert;
      /* pass-through */
    case L7_FILE_TYPE_SSLPEM_SERVER:
      return pStrInfo_base_SslSrvrCert;
      /* pass-through */
    case L7_FILE_TYPE_SSLPEM_DHWEAK:
      return pStrInfo_base_SslDhWeak;
      /* pass-through */
    case L7_FILE_TYPE_SSLPEM_DHSTRONG:
      return pStrInfo_base_SslDhStrong;
      /* pass-through */
    case L7_FILE_TYPE_STK:
      return pStrInfo_base_Stk;
      /* pass-through */
    case L7_FILE_TYPE_CONFIG_SCRIPT:
      return pStrInfo_base_CfgScript_1;
      /* pass-through */
    case L7_FILE_TYPE_CLI_BANNER:
      return pStrInfo_base_CliBanner;
      /* pass-through */
    case L7_FILE_TYPE_KERNEL:
      return pStrInfo_base_Kernel;
      /* pass-through */
    case L7_FILE_TYPE_ACL_XML:
      return "ACL Configuration";
      /* pass-through */
    case L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS:
      return "DHCP Snooping Bindings";
      /* pass-through */
    case L7_FILE_TYPE_STARTUP_LOG:
      return "Startup Log";
      /* pass-through */
    case L7_FILE_TYPE_OPERATIONAL_LOG:
      return "Operational Log";
      /* pass-through */
    case L7_FILE_TYPE_FWIMAGE:
      return "Firmware Image";
      /* pass-through */
    case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
      return pStrInfo_base_tr069_acs_SslRootCert;
      /* pass-through */
    case L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
      return pStrInfo_base_tr069_client_SslPrivateKey;
      /* pass-through */
    case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT:
      return pStrInfo_base_tr069_client_SslCert;
      /* pass-through */
    case L7_FILE_TYPE_AUTO_INSTALL_SCRIPT:
      return "Auto Install Script";
#if 0
    case L7_FILE_TYPE_FLASH:
      return CLI_FLASHFILE_INFO;
#endif
    case L7_FILE_TYPE_IAS_USERS:
      return "IAS Users";
      /* pass-through */
    case L7_FILE_TYPE_LAST:
    default:
      /* pass-through */
      break;
  }
  return (def == L7_NULLPTR) ? pStrInfo_common_Unknown_2 : def;
}

/*********************************************************************
*
* @purpose  To return either up  or down
*
* @param L7_int32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @returns String "Enabled" or "Disabled"
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilUpDownGet(L7_int32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_UP:
      return pStrInfo_common_Up;
      /* pass-through */
    case L7_DOWN:
      return pStrInfo_common_Down_1;
      /* pass-through */
    default:
      break;
  }

  return (L7_NULLPTR == def) ? pStrInfo_common_Down_1 : def;
}

L7_char8 *strUtilSntpStatusGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_SNTP_STATUS_OTHER:
      return pStrInfo_common_Other;
      /* pass-through */
    case L7_SNTP_STATUS_SUCCESS:
      return pStrInfo_common_ApActionSuccess;
      /* pass-through */
    case L7_SNTP_STATUS_REQUEST_TIMED_OUT:
      return pStrInfo_base_ReqTimedOut;
      /* pass-through */
    case L7_SNTP_STATUS_BAD_DATE_ENCODED:
      return pStrInfo_base_BadDateEncoded;
      /* pass-through */
    case L7_SNTP_STATUS_VERSION_NOT_SUPPORTED:
      return pStrErr_base_VerNotSupported;
      /* pass-through */
    case L7_SNTP_STATUS_SERVER_UNSYNCHRONIZED:
      return pStrInfo_base_SrvrUnsynchronized;
      /* pass-through */
    case L7_SNTP_STATUS_KISS_OF_DEATH:
      return pStrInfo_base_SrvrKissOfDeath;
      /* pass-through */
    default:
      break;
  }
  return (def == L7_NULLPTR) ? pStrInfo_common_Other : def;
}

L7_char8 *strUtilDot1qTypeGet(L7_int32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_DOT1Q_DEFAULT:
      return pStrInfo_common_Defl;
      /* pass-through */
    case L7_DOT1Q_ADMIN:
      return pStrInfo_common_PimSmGrpRpMapStatic;
      /* pass-through */
    case L7_DOT1Q_WS_DYNAMIC:
    case L7_DOT1Q_DYNAMIC:
    case L7_DOT1Q_IP_INTERNAL_VLAN:

      return pStrInfo_common_Dyn_1;
      /* pass-through */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Defl;
}

L7_char8 *strUtilUnitMgrMgmtPrefGet(usmDbUnitMgrMgmtPref_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_USMDB_UNITMGR_MGMTPREF_DISABLED:
      return pStrInfo_common_Dsbld;
      /*pass thru */

    case L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED:
      return pStrInfo_common_UnAssigned;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_common_UnAssigned;
}

L7_char8 *strUtilDiffservPolicyTypeGet(L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
      return pStrInfo_common_In;
      /*pass thru */
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
      return pStrInfo_qos_Out;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_common_EmptyString;
}

L7_char8 *strUtilYesNoGet(L7_BOOL val, L7_char8 *def)
{
  switch (val)
  {
    case L7_TRUE:
      return pStrInfo_common_Yes;
      /*pass thru */
    case L7_FALSE:
      return pStrInfo_common_No;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_No;
}

L7_char8 *strUtilTrueFalseGet(L7_BOOL val, L7_char8 *def)
{
  switch (val)
  {
    case L7_TRUE:
      return pStrInfo_common_True;
      /*pass thru */
    case L7_FALSE:
      return pStrInfo_common_False;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_common_False;
}

L7_char8 *strUtilQosCosQueueMgmtTypeGet(L7_QOS_COS_QUEUE_MGMT_TYPE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP:
      return pStrInfo_qos_NoRandomDetect_1;
      /*pass thru */
    case L7_QOS_COS_QUEUE_MGMT_TYPE_WRED:
      return pStrInfo_qos_RandomDetect_2;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_qos_NoRandomDetect_1;
}

L7_char8 *strUtilSpecialPortTypeGet(L7_SPECIAL_PORT_TYPE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_PORT_NORMAL:
      return pStrInfo_common_EmptyString;
      /*pass thru */
    case L7_TRUNK_MEMBER:
      return pStrInfo_common_LagMbr;
      /*pass thru */
    case L7_MIRRORED_PORT:
      return pStrInfo_common_MirrorShow;
      /*pass thru */
    case L7_PROBE_PORT:
      return pStrInfo_common_Probe;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_EmptyString;
}

L7_char8 *strUtilOspfVirtNbrTypeGet(L7_OSPF_VIRT_NBR_STATE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_VIRT_NBR_STATE_DOWN:
      return pStrInfo_common_Down_2;
      /*pass thru */
    case L7_OSPF_VIRT_NBR_STATE_ATTEMPT:
      return pStrInfo_common_Attempt;
      /*pass thru */
    case L7_OSPF_VIRT_NBR_STATE_INIT:
      return pStrInfo_common_Init;
      /*pass thru */
    case L7_OSPF_VIRT_NBR_STATE_TWOWAY:
      return pStrInfo_common_TwoWay;
      /*pass thru */
    case L7_OSPF_VIRT_NBR_STATE_EXCHANGE_START:
      return pStrInfo_common_ExchangeStart;
      /*pass thru */
    case L7_OSPF_VIRT_NBR_STATE_EXCHANGE:
      return pStrInfo_common_Exchange;
      /*pass thru */
    case L7_OSPF_VIRT_NBR_STATE_LOADING:
      return pStrInfo_common_Loading;
      /*pass thru */
    case L7_OSPF_VIRT_NBR_STATE_FULL:
      return pStrInfo_common_Full;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Down_2;
}

L7_char8 *strUtilOspfNssaTransStateGet(L7_OSPF_NSSA_TRANSLATOR_STATE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_NSSA_TRANS_STATE_ENABLED:
      return pStrInfo_common_Enbld;
      /*pass thru */
    case L7_OSPF_NSSA_TRANS_STATE_ELECTED:
      return pStrInfo_common_Elected;
      /*pass thru */
    case L7_OSPF_NSSA_TRANS_STATE_DISABLED:
      return pStrInfo_common_Dsbld;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_common_Dsbld;
}


L7_char8 *strUtilospfNssaTranslatorTypeGet(L7_OSPF_NSSA_TRANSLATOR_ROLE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_NSSA_TRANSLATOR_ALWAYS:
      return pStrInfo_common_Always;
      /*pass thru */
    case L7_OSPF_NSSA_TRANSLATOR_CANDIDATE:
      return pStrInfo_common_Candidate;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_common_Candidate;
}

L7_char8 *strUtilOspfMetricTypeGet(L7_OSPF_EXT_METRIC_TYPES_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_METRIC_EXT_TYPE1:
      return pStrInfo_common_OspfMetricExtType1;
      /*pass thru */
    case L7_OSPF_METRIC_EXT_TYPE2:
      return pStrInfo_common_OspfMetricExtType2;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_common_OspfMetricExtType1;
}

L7_char8 *strUtilOspfIntfTypeGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_INTF_DOWN:
      return pStrInfo_common_Down_2;
      /*pass thru */
      break;
    case L7_OSPF_INTF_LOOPBACK:
      return pStrInfo_common_LoopBack;
      /*pass thru */
      break;
    case L7_OSPF_INTF_WAITING:
      return pStrInfo_common_Waiting_2;
      /*pass thru */
      break;
    case L7_OSPF_INTF_POINTTOPOINT:
      return pStrInfo_common_PointToPoint_2;
      /*pass thru */
      break;
    case L7_OSPF_INTF_DESIGNATEDROUTER:
      return pStrInfo_common_DesignatedRtr_2;
      /*pass thru */
      break;
    case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
      return pStrInfo_common_BackupDesignatedRtr_2;
      /*pass thru */
      break;
    case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
      return pStrInfo_common_OtherDesignatedRtr_1;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_common_Blank;
}

L7_char8 *strUtilOspfIntfTypeStringGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_INTF_LOOPBACK:
      return pStrInfo_common_LoopBack;
      /*pass thru */
    case L7_OSPF_INTF_WAITING:
      return pStrInfo_common_Waiting;
      /*pass thru */
    case L7_OSPF_INTF_POINTTOPOINT:
      return pStrInfo_common_Ptp;
      /*pass thru */
    case L7_OSPF_INTF_DESIGNATEDROUTER:
      return pStrInfo_common_Dr;
      /*pass thru */
    case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
      return pStrInfo_common_BackupDr;
      /*pass thru */
    case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
      return pStrInfo_common_DrOther;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_common_Line;
}

L7_char8 *strUtilOspfMessageGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_DOWN:
      return pStrInfo_common_Down_1;
      /*pass thru */
    case L7_OSPF_ATTEMPT:
      return pStrInfo_common_Attempt;
      /*pass thru */
    case L7_OSPF_INIT:
      return pStrInfo_common_Init;
      /*pass thru */
    case L7_OSPF_TWOWAY:
      return pStrInfo_common_TwoWay;
      /*pass thru */
    case L7_OSPF_EXCHANGESTART:
      return pStrInfo_common_ExchangeStart;
      /*pass thru */
    case L7_OSPF_EXCHANGE:
      return pStrInfo_common_Exchange;
      /*pass thru */
    case L7_OSPF_LOADING:
      return pStrInfo_common_Loading;
      /*pass thru */
    case L7_OSPF_FULL:
      return pStrInfo_common_Full;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Down_1;
}

L7_char8 *strUtilOspfMessageTypeGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_DOWN:
      return pStrInfo_common_Down_1;
      /*pass thru */
    case L7_OSPF_ATTEMPT:
      return pStrInfo_common_Attempt;
      /*pass thru */
    case L7_OSPF_INIT:
      return pStrInfo_common_Init;
      /*pass thru */
    case L7_OSPF_TWOWAY:
      return pStrInfo_common_TwoWay;
      /*pass thru */
    case L7_OSPF_EXCHANGESTART:
      return pStrInfo_common_ExStart;
      /*pass thru */
    case L7_OSPF_EXCHANGE:
      return pStrInfo_common_Exchange;
      /*pass thru */
    case L7_OSPF_LOADING:
      return pStrInfo_common_Loading;
      /*pass thru */
    case L7_OSPF_FULL:
      return pStrInfo_common_Full;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Down_1;
}

L7_char8 *strUtilOspfAreaSummaryGet(L7_OSPF_AREA_SUMMARY_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_AREA_SEND_SUMMARY:
      return pStrInfo_common_Enbl_1;
      /*pass thru */
    case L7_OSPF_AREA_NO_SUMMARY:
      return pStrInfo_common_Dsbl_1;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Dsbl_1;
}

L7_char8 *strUtilOspfAreaImportTypeGet(L7_OSPF_AREA_EXT_ROUTING_t val)
{
  switch (val)
  {
    case L7_OSPF_AREA_IMPORT_NSSA:
      return pStrInfo_common_DispOspfAreaNssa;
      /*pass thru */
    case L7_OSPF_AREA_IMPORT_NO_EXT:
      return pStrInfo_common_DispOspfAreaStub;
      /*pass thru */
    default:
      break;
  }
  return pStrInfo_common_DispOspfAreaStub;
}

L7_char8 *strUtilMcastProtocolTypeGet(L7_MCAST_IANA_PROTO_ID_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_MCAST_IANA_MROUTE_PIM_SM:
      return pStrInfo_common_Pimsm;
      /*pass thru */
    case L7_MCAST_IANA_MROUTE_PIM_DM:
      return pStrInfo_common_Pimdm;
      /*pass thru */
    case L7_MCAST_IANA_MROUTE_DVMRP:
      return pStrInfo_common_Dvmrp_1;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Pimsm;
}

L7_char8 *strUtilIntfStateGet(L7_PORT_EVENTS_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_DOWN:
      return pStrInfo_common_Down_2;
      /*pass thru */
    case L7_UP:
      return pStrInfo_common_Up;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Down_2;
}

L7_char8 *strUtilIntfStateGet1(L7_PORT_EVENTS_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_DOWN:
      return pStrInfo_common_Down_1;
      /*pass thru */
    case L7_UP:
      return pStrInfo_common_Up;
      /*pass thru */
    default:
      return (L7_NULLPTR == def) ? pStrInfo_common_Empty_1 : def;
  }
}

L7_char8 *strUtilDot1sPortRoleStateGet(L7_DOT1S_PORT_ROLE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_DOT1S_ROLE_DISABLED:
      return pStrInfo_common_Dsbld;
      /*pass thru */
    case L7_DOT1S_ROLE_ROOT:
      return pStrInfo_common_RootPort;
      /*pass thru */
    case L7_DOT1S_ROLE_DESIGNATED:
      return pStrInfo_common_DesignatedPort;
      /*pass thru */
    case L7_DOT1S_ROLE_ALTERNATE:
      return pStrInfo_common_AlternatePort;
      /*pass thru */
    case L7_DOT1S_ROLE_BACKUP:
      return pStrInfo_common_Backup;
      /*pass thru */
    case L7_DOT1S_ROLE_MASTER:
      return pStrInfo_common_Master;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Dsbld;
}

L7_char8 *strUtilDot1sPortRoleGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_DOT1S_DISCARDING:
      return pStrInfo_common_Discarding;
      /*pass thru */
    case L7_DOT1S_LEARNING:
      return pStrInfo_common_Learning;
      /*pass thru */
    case L7_DOT1S_FORWARDING:
      return pStrInfo_common_Fwd;
      /*pass thru */
    case L7_DOT1S_DISABLED:
      return pStrInfo_common_Dsbld;
      /*pass thru */
    case L7_DOT1S_MANUAL_FWD:
      return pStrInfo_common_ManualFwd;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Dsbld;
}

L7_char8 *strUtilEnableDisableGet1(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_ENABLE:
      return pStrInfo_common_Enbl_1;
      /*pass thru */

    case L7_DISABLE:
      return pStrInfo_common_Dsbl_1;
      /*pass thru */

    default:
      break;
  }
  return def ? def : pStrInfo_common_Dsbl_1;
}

L7_char8 *strUtilAclProtoGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_ACL_L4PORT_DOMAIN:
      return pStrInfo_qos_Domain_1;
      /*pass thru */
    case L7_ACL_L4PORT_ECHO:
      return pStrInfo_qos_Echo;
      /*pass thru */
    case L7_ACL_L4PORT_FTP:
      return pStrInfo_common_Ftp;
      /*pass thru */
    case L7_ACL_L4PORT_FTPDATA:
      return pStrInfo_qos_Ftpdata;
      /*pass thru */
    case L7_ACL_L4PORT_HTTP:
      return pStrInfo_common_Http_1;
      /*pass thru */
    case L7_ACL_L4PORT_SMTP:
      return pStrInfo_qos_Smtp;
      /*pass thru */
    case L7_ACL_L4PORT_SNMP:
      return pStrInfo_common_Snmp_1;
      /*pass thru */
    case L7_ACL_L4PORT_TELNET:
      return pStrInfo_common_Telnet;
      /*pass thru */
    case L7_ACL_L4PORT_TFTP:
      return pStrInfo_common_Tftp_1;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrInfo_common_Tftp_1;
}

L7_char8 *strUtilPortSpeedGet(L7_PORT_SPEEDS_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
      return pStrInfo_common_Auto;
      /*pass thru */
    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      return pStrInfo_base_Speed100Half;
      /*pass thru */
    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      return pStrInfo_base_Speed100Full;
      /*pass thru */
    case L7_PORTCTRL_PORTSPEED_HALF_10T:
      return pStrInfo_base_Speed10Half;
      /*pass thru */
    case L7_PORTCTRL_PORTSPEED_FULL_10T:
      return pStrInfo_base_Speed10Full;
      /*pass thru */
    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      return pStrInfo_base_Speed100FxFull;
      /*pass thru */
    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      return pStrInfo_base_Speed1000Full;
      /*pass thru */
    /* PTin added: Speed 2.5G */
    case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
      return pStrInfo_base_Speed2500Full;
      /*pass thru */
    /* PTin end */
    case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      return pStrInfo_base_Speed10GigFull;
      /*pass thru */
    /* PTin added: Speed 40G */
    case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
      return pStrInfo_base_Speed40GigFull;
      /*pass thru */
    /* PTin added: Speed 100G */
    case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
      return pStrInfo_base_Speed100GigFull;
      /*pass thru */
    /* PTin end */
    default:
      break;
  }
  return def ? def : pStrInfo_common_Auto;
}

L7_char8 *strUtilWdmClientStateGet(L7_WDM_CLIENT_STATUS_t status, L7_char8 *def)
{
  switch (status)
  {
    case L7_WDM_CLIENT_ASSOC:
      return pStrInfo_common_ApProfileStateAssociated;
      /*pass thru */
    case L7_WDM_CLIENT_AUTH:
      return pStrInfo_common_ClientAuth;
      /*pass thru */
    case L7_WDM_CLIENT_DISASSOC:
      return pStrInfo_common_ClientDisassoc;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_EmptyString;
}

L7_char8 *strUtilApStatusGet(L7_WDM_AP_STATUS_t status, L7_char8 *def)
{
  switch (status)
  {
    case L7_WDM_AP_STATUS_MANAGED:
      return pStrInfo_common_Managed;
      /*pass thru */
    case L7_WDM_AP_STATUS_STANDALONE:
      return pStrInfo_common_Standalone;
      /*pass thru */
    case L7_WDM_AP_STATUS_UNKNOWN:
      return pStrInfo_common_Unknown;
      /*pass thru */
    case L7_WDM_AP_STATUS_ROGUE:
      return pStrInfo_common_Rogue;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_common_None_1;
}

L7_char8 *strUtilWdmActionStatusGet(L7_WDM_ACTION_STATUS_t status, L7_char8 *def)
{
  switch (status)
  {
    case L7_WDM_ACTION_NOT_STARTED:
      return pStrErr_wireless_ApActionNotStarted;
      /*pass thru */
    case L7_WDM_ACTION_REQUESTED:
      return pStrInfo_wireless_ApActionReq;
      /*pass thru */
    case L7_WDM_ACTION_IN_PROGRESS:
      return pStrInfo_wireless_ApActionInProgress;
      /*pass thru */
    case L7_WDM_ACTION_SUCCESS:
      return pStrInfo_common_ApActionSuccess;
      /*pass thru */
    case L7_WDM_ACTION_PARTIAL_SUCCESS:
      return pStrInfo_wireless_ApActionPartSuccess;
      /*pass thru */
    case L7_WDM_ACTION_FAILURE:
      return pStrErr_wireless_ApActionFailure;
      /*pass thru */
    default:
      break;
  }

  return def ? def : pStrErr_wireless_ApActionFailure;
}

L7_char8 *strUtilBgpStatusGet(L7_BGP_PEER_STATE_t status, L7_char8 *def)
{
  switch (status)
  {
    case L7_BGP_START:
      return pStrInfo_bgp_Start;
      /*pass thru */
    case L7_BGP_STOP:
      return pStrInfo_bgp_Stop;
      /*pass thru */
    default:
      break;
  }
  return def ? def : pStrInfo_bgp_Stop;

}

L7_char8 *strUtilAlarmStatusGet(L7_MODULE_ALARM_STATUS_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_MODULE_ALARM_PRESENT:
      return pStrInfo_common_Yes;
    case L7_MODULE_ALARM_NOT_PRESENT:
      return pStrInfo_common_No;
    default:
      break;
  }
  return def ? def : pStrInfo_common_No;
}

L7_char8 *strUtilMfdbTypeGet(L7_MFDB_ENTRY_TYPE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_MFDB_TYPE_STATIC:
      return pStrInfo_common_PimSmGrpRpMapStatic;
    case L7_MFDB_TYPE_DYNAMIC:
      return pStrInfo_common_Dyn_1;
    default:
      break;
  }
  return def ? def : pStrInfo_common_Unknown_1;
}

L7_char8 *strUtilAclDirectionGet(L7_ACL_DIRECTION_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_INBOUND_ACL:
      return pStrInfo_qos_Inbound_1;
    case L7_OUTBOUND_ACL:
      return pStrInfo_qos_Outbound_1;
    default:
      break;
  }
  return def ? def : "  ";
}

L7_char8 *strUtilMgmdErrCodeGet(L7_MGMD_ERROR_CODE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_MGMD_ERR_INVALID_INPUT:
      return pStrErr_common_Input_4;
    case L7_MGMD_ERR_REQUEST_FAILED:
      return pStrInfo_common_OperationFailed_1;
    case L7_MGMD_ERR_PROXY_INTF:
      return pStrErr_common_IgmpProxyCfguredOnIntf;
    default:
      break;
  }
  return def ? def : pStrInfo_common_OperationFailed_1;
}

L7_char8 *strUtilOspfVirtualLinkGet(L7_OSPF_AREA_EXT_ROUTING_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_AREA_IMPORT_NSSA:
      return pStrErr_common_CreateVirtualLinkNssa;
    case L7_OSPF_AREA_IMPORT_NO_EXT:
      return pStrErr_common_CreateVirtualLinkStubArea;
    default:
      break;
  }
  return def ? def : pStrErr_common_CreateVirtualLinkStubArea;
}

L7_char8 *strUtilOspfAreaImportGet(L7_OSPF_AREA_EXT_ROUTING_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_AREA_IMPORT_EXT:
      return pStrInfo_common_ImportExternalLsas;
    case L7_OSPF_AREA_IMPORT_NO_EXT:
      return pStrInfo_common_ImportNoLsas;
    case L7_OSPF_AREA_IMPORT_NSSA:
      return pStrInfo_common_ImportNssas;
    default:
      break;
  }
  return def ? def : pStrInfo_common_ImportNoLsas;
}

L7_char8 *strUtilFdbAddrEntryTypeGet(L7_FDB_ADDR_FLAG_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_FDB_ADDR_FLAG_STATIC:
      return pStrInfo_common_PimSmGrpRpMapStatic;
    case L7_FDB_ADDR_FLAG_LEARNED:
      return pStrInfo_base_Learned;
    case L7_FDB_ADDR_FLAG_SELF:
      return pStrInfo_base_Self;
    case L7_FDB_ADDR_FLAG_MANAGEMENT:
    case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
      return pStrInfo_base_Mgmt;
    case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
      return pStrInfo_base_GmrpLearned;
    default:
      break;
  }
  return def ? def : pStrInfo_base_Learned;
}

L7_char8 *strUtilMcastPimsmTypeGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_MCAST_PIMSM_FLAG_RPT:
      return pStrInfo_ipmcast_Rpt;
    case L7_MCAST_PIMSM_FLAG_SPT:
      return pStrInfo_ipmcast_Spt;
    default:
      break;
  }
  return def ? def : pStrInfo_ipmcast_Spt;
}

L7_char8 *strUtilApHealthStatusGet(L7_MODULE_HEALTH_STATUS_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_MODULE_HEALTHY:
      return pStrInfo_common_Ok;
    case L7_MODULE_FAILED:
      return pStrErr_common_ApStatusFailed;
    case L7_MODULE_NOT_PRESENT:
      return pStrErr_common_NotPresent;
    default:
      break;
  }
  return def ? def : pStrErr_common_NotPresent;
}

L7_char8 *strUtilOspfV3EntryTypeGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_V3_ILLEGAL_LSA:
      return pStrInfo_common_IllegalLsa;
    case L7_V3_ROUTER_LSA:
      return pStrInfo_common_RtrLinks;
    case L7_V3_NETWORK_LSA:
      return pStrInfo_common_NwLinks;
    case L7_V3_INTER_PREFIX_LSA:
      return pStrInfo_routingv6_RoutingInterNw;
    case L7_V3_INTER_ROUTER_LSA:
      return pStrInfo_routingv6_RoutingInterRtr;
    case L7_V3_AS_EXTERNAL_LSA:
      return pStrInfo_common_AsExternal;
    case L7_V3_GROUP_MEMBER_LSA:
      return pStrInfo_common_McastGrp;
    case L7_V3_NSSA_LSA:
      return pStrInfo_common_NssaLsa;
    case L7_V3_LINK_LSA:
      return pStrInfo_routingv6_RoutingLink;
    case L7_V3_INTRA_PREFIX_LSA:
      return pStrInfo_routingv6_RoutingIntraPrefix;
    case L7_V3_LINK_UNKNOWN_LSA:
      return pStrInfo_routingv6_RoutingUnknownLink;
    case L7_V3_AREA_UNKNOWN_LSA:
      return pStrInfo_routingv6_RoutingUnknownArea;
    case L7_V3_AS_UNKNOWN_LSA:
      return pStrInfo_routingv6_RoutingUnknownAs;
    default:
      break;
  }
  return def ? def : pStrInfo_routingv6_RoutingUnknownAs;
}

L7_char8 *strUtilOspfAreaAggLsdbTypeGet(L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK:
      return pStrInfo_common_SummaryLink_1;
    case L7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK:
      return pStrInfo_common_NssaExternalLink;
    default:
      break;
  }
  return def ? def : pStrInfo_common_NssaExternalLink;
}

L7_char8 *strUtilIpv6NbrStateGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_IP6_NBR_STATE_REACHABLE:
      return pStrInfo_common_Ipv6Reachable;
    case L7_IP6_NBR_STATE_STALE:
      return pStrInfo_common_Stale;
    case L7_IP6_NBR_STATE_DELAY:
      return pStrInfo_common_Delay;
    case L7_IP6_NBR_STATE_PROBE:
      return pStrInfo_common_Probe;
    case L7_IP6_NBR_STATE_UNKNOWN:
      return pStrInfo_common_Blank;
    default:
      break;
  }
  return def ? def : pStrInfo_common_Blank;
}

L7_char8 *strUtilDhcp6PrefixTypeGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case L7_DHCP6_PREFIX_IAPD:
      return pStrInfo_routingv6_Ipv6DhcpIapdAbbrev;
    case L7_DHCP6_PREFIX_IANA:
      return pStrInfo_routingv6_Ipv6DhcpIanaAbbrev;
    case L7_DHCP6_PREFIX_IATA:
      return pStrInfo_routingv6_Ipv6DhcpIataAbbrev;
    default:
      break;
  }
  return def ? def : pStrInfo_routingv6_Ipv6DhcpIanaAbbrev;
}

L7_char8 *strUtilReDistRouteTypeGet(L7_REDIST_RT_INDICES_t val, L7_char8 *def)
{
  switch (val)
  {
    case REDIST_RT_BGP:
      return pStrInfo_common_Bgp_1;
    case REDIST_RT_RIP:
      return pStrInfo_common_Rip3;
    case REDIST_RT_STATIC:
      return pStrInfo_common_Static2;
    case REDIST_RT_LOCAL:
      return pStrInfo_common_Connected;
    default:
      break;
  }
  return def ? def : pStrInfo_common_Connected;
}

L7_char8 *strUtilWdmQosQueueTypeGet(L7_WDM_QOS_QUEUES_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_WDM_QUEUE_VOICE:
      return pStrInfo_wireless_Voice;
    case L7_WDM_QUEUE_VIDEO:
      return pStrInfo_wireless_Video;
    case L7_WDM_QUEUE_BEST_EFFORT:
      return pStrInfo_wireless_Best_Effort;
    case L7_WDM_QUEUE_BACKGROUND:
      return pStrInfo_wireless_Background;
    default:
      break;
  }
  return def ? def : pStrInfo_wireless_Best_Effort;
}


L7_char8 *strUtilDelAreaError(L7_RC_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_ERROR:
      return pStrErr_common_CantDelAreaNotCfgured;
    case L7_FAILURE:
      return pStrErr_common_CantDelAreaActiveIntf;
    default:
      break;
  }
  return def ? def : pStrErr_common_CantDelAreaActiveIntf;
}

L7_char8 *strUtilRtoEntryTypeGet(L7_RTO_PROTOCOL_INDICES_t val, L7_char8 *def)
{
  switch (val)
  {
    case RTO_LOCAL:
      return pStrInfo_common_WsNwLocal;
    case RTO_STATIC:
      return pStrInfo_common_PimSmGrpRpMapStatic;
    case RTO_MPLS:
      return pStrInfo_common_Mpls;
    case RTO_OSPF_INTRA_AREA:
      return pStrInfo_common_OspfIntra;
    case RTO_OSPF_INTER_AREA:
      return pStrInfo_common_OspfInter;
    case RTO_OSPF_TYPE1_EXT:
      return pStrInfo_common_OspfExternal;
    case RTO_OSPF_TYPE2_EXT:
      return pStrInfo_common_OspfExternal;
    case RTO_OSPF_NSSA_TYPE1_EXT:
      return pStrInfo_routing_OspfNssaType1;
    case RTO_OSPF_NSSA_TYPE2_EXT:
      return pStrInfo_routing_OspfNssaType2;
    case RTO_RIP:
      return pStrInfo_common_Rip;
    case RTO_DEFAULT:
      return pStrInfo_common_Defl;
    case RTO_IBGP:
    case RTO_EBGP:
      return pStrInfo_common_Bgp4;
    default:
      break;
  }
  return def ? def : pStrInfo_common_Defl;
}

L7_char8 *strUtilOspfPathTypeGet(L7_OSPF_PATH_TYPE_t val, L7_char8 *def)
{
  switch (val)
  {
    case L7_OSPF_INTRA_AREA:
      return pStrInfo_common_IntraArea;
    case L7_OSPF_INTER_AREA:
      return pStrInfo_common_InterArea;
    default:
      break;
  }

  return def ? def : pStrInfo_common_InterArea;
}

L7_char8 *strUtilRouteTypeInfoGet(L7_uint32 val, L7_char8 *def)
{
  switch (val)
  {
    case 1:
      return pStrInfo_common_Connected;
    case 2:
      return pStrInfo_common_PimSmGrpRpMapStatic;
    case 3:
      return pStrInfo_common_Rip;
    case 4:
      return pStrInfo_common_Ospf;
    case 5:
      return pStrInfo_common_Bgp;
    default:
      break;
  }
  return def ? def : pStrInfo_common_Connected;
}

/*********************************************************************
*
* @purpose  To return either enabled or disabled
*
* @param L7_int32 val
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @returns String "Enabled" or "Disabled"
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilLLdpMedCapabilityGet(lldpXMedCapabilities_t medCap, L7_char8 *buf)
{
  L7_uint32 len=0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  memset(stat, 0, sizeof(stat));
  if (medCap.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
  {
    OSAPI_STRNCAT(stat, " capabilities,");
  }
  if (medCap.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
  {
    OSAPI_STRNCAT(stat, " networkpolicy,");
  }
  if (medCap.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
  {
    OSAPI_STRNCAT(stat, " location,");
  }
  if (medCap.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
  {
    OSAPI_STRNCAT(stat, " extended-pse,");
  }
  if (medCap.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
  {
    OSAPI_STRNCAT(stat, " extended-pd,");
  }
  if (medCap.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
  {
    OSAPI_STRNCAT(stat, " inventory,");
  }
  len = strlen(stat);
  if (len > 0)
  {
    stat[len-1] = L7_EOS;              /* remove the trailing comma */
  }

  osapiStrncpy(buf, stat, sizeof(stat));
  return buf;
}

/*********************************************************************
*
* @purpose  To return detected client status string
*
* @param L7_uint32 status
* @param L7_char8 * def
*
* @returntype const L7_char8
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strWdmDetectedClientStatusGet(L7_uint32 status, L7_char8 *def)
{
  switch (status)
  {
    case L7_WDM_DETECTED_CLIENT_AUTH:
      return pStrInfo_common_ClientAuth;
      /* pass-through */
    case L7_WDM_DETECTED_CLIENT_DETECTED:
      return pStrInfo_common_Detected;
      /* pass-through */
    case L7_WDM_DETECTED_CLIENT_KNOWN:
      return pStrInfo_common_Known;
      /* pass-through */
    case L7_WDM_DETECTED_CLIENT_BLACKLISTED:
      return pStrInfo_common_BlackListed;
      /* pass-through */
    case L7_WDM_DETECTED_CLIENT_ROGUE:
      return pStrInfo_common_Rogue;
      /* pass-through */
    default:
      break;
  }
  return def ? def : pStrInfo_common_None_1;
}

/*********************************************************************
*
* @purpose  To return hash mode name
*
* @param L7_int32 hashMode     Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returntype const L7_char8
*
* @returns String hash mode name
*
* @notes
*
* @end
*
*********************************************************************/
L7_char8 *strUtilHashModeGet(L7_int32 hashMode)
{
  switch (hashMode)
  {
    case L7_DOT3AD_HASHING_MODE_SA_VLAN:
      return pStrInfo_switching_HashModeSaVlan;
    case L7_DOT3AD_HASHING_MODE_DA_VLAN:
      return pStrInfo_switching_HashModeDaVlan;
    case L7_DOT3AD_HASHING_MODE_SDA_VLAN:
      return pStrInfo_switching_HashModeSaDaVlan;
    case L7_DOT3AD_HASHING_MODE_SIP_SPORT:
      return pStrInfo_switching_HashModeSrcIP;
    case L7_DOT3AD_HASHING_MODE_DIP_DPORT:
      return pStrInfo_switching_HashModeDestIP;
    case L7_DOT3AD_HASHING_MODE_SDIP_DPORT:
      return pStrInfo_switching_HashModeSrcDestIP;
    case L7_DOT3AD_HASHING_MODE_ENHANCED:
      return pStrInfo_switching_HashModeEnhanced;
    default:
      return pStrErr_base_LagHashModeReturnErr;
  }
}

/*********************************************************************
*
* @purpose  Convert a UDP port number to a string
*
* @param    udpPort     a UDP port number
* @param    portString  buffer to store result. At least IH_UDP_PORT_NAME_LEN.
*
* @returns  L7_SUCCESS
*
* @comments Certain well-known ports are given names. Others are written
*           as numbers.
*
* @end
*
*********************************************************************/
L7_RC_t strUtilUdpPortToString(L7_ushort16 udpPort, L7_uchar8 *portString)
{
  switch (udpPort)
  {
    case IH_UDP_PORT_DEFAULT:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "Default");
      break;
    case UDP_PORT_DHCP_SERV:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "dhcp");
      break;
    case UDP_PORT_TIME:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "time");
      break;
    case UDP_PORT_NAMESERVER:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "nameserver");
      break;
    case UDP_PORT_TACACS:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "tacacs");
      break;
    case UDP_PORT_DNS:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "domain");
      break;
    case UDP_PORT_TFTP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "tftp");
      break;
    case UDP_PORT_NTP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "ntp");
      break;
    case UDP_PORT_NETBIOS_NS:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "netbios-ns");
      break;
    case UDP_PORT_NETBIOS_DGM:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "netbios-dgm");
      break;
    case UDP_PORT_MOBILE_IP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "mobile-ip");
      break;
    case UDP_PORT_PIM_AUTO_RP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "pim-auto-rp");
      break;
    case UDP_PORT_ISAKMP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "isakmp");
      break;
    case UDP_PORT_RIP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "rip");
      break;

    default:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%u", udpPort);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Convert a UDP port name to a number
*
* @param    portName
* @param    udpPort     a UDP port number. IH_UDP_PORT_DEFAULT if name not recognized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Certain well-known ports are given names. Others are written
*           as numbers. Implemented here rather than in app so UI can
*           determine which ports to show as names and which as numbers.
*
* @end
*
*********************************************************************/
L7_ushort16 strUtilUdpPortNameToNumber(const L7_char8 *portString)
{
  L7_uint32 portNum = 0;

  if (strcmp(portString, "dhcp") == 0)
    return UDP_PORT_DHCP_SERV;
  if (strcmp(portString, "time") == 0)
    return UDP_PORT_TIME;
  if (strcmp(portString, "nameserver") == 0)
    return UDP_PORT_NAMESERVER;
  if (strcmp(portString, "tacacs") == 0)
    return UDP_PORT_TACACS;
  if (strcmp(portString, "domain") == 0)
    return UDP_PORT_DNS;
  if (strcmp(portString, "tftp") == 0)
    return UDP_PORT_TFTP;
  if (strcmp(portString, "ntp") == 0)
    return UDP_PORT_NTP;
  if (strcmp(portString, "netbios-ns") == 0)
    return UDP_PORT_NETBIOS_NS;
  if (strcmp(portString, "netbios-dgm") == 0)
    return UDP_PORT_NETBIOS_DGM;
  if (strcmp(portString, "mobile-ip") == 0)
    return UDP_PORT_MOBILE_IP;
  if (strcmp(portString, "pim-auto-rp") == 0)
    return UDP_PORT_PIM_AUTO_RP;
  if (strcmp(portString, "isakmp") == 0)
    return UDP_PORT_ISAKMP;
  if (strcmp(portString, "rip") == 0)
    return UDP_PORT_RIP;

  /* Assume it's a number written as a character string */
  if ((cliConvertTo32BitUnsignedInteger(portString, &portNum)!=L7_SUCCESS)||(portNum < L7_IP_HELPER_MIN_PORT) || (portNum > L7_IP_HELPER_MAX_PORT))
    return IH_UDP_PORT_DEFAULT;
  return (L7_ushort16) portNum;
}
