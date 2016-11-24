/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_client_status.c
*
* @purpose    Code in support of the cp_client_status.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     7/9/2007
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "web_oem.h"
#include "strlib_security_web.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "usmdb_cpim_api.h"
#include "usmdb_cpdm_api.h"
#include "captive_portal_commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"


/*********************************************************************
*
* @purpose  Get the CP client session time
*
* @param    context     @b{(input)} EmWeb/Server request context handle
*
* @returns  CP client session time in usmWebBuffer
*
* @end
*
*********************************************************************/
L7_char8 *usmWebCPClientSessionTimeGet(EwsContext context, L7_BOOL sessionTime)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_enetMacAddr_t macAddr;
  L7_uint32 val = 0;
  L7_char8 tmpBuf[USMWEB_APP_DATA_SIZE];
  static L7_char8 buf[USMWEB_APP_DATA_SIZE];
  L7_RC_t rc = L7_FAILURE;

  memset(buf, 0, sizeof(buf));
  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    memcpy(macAddr.addr, appInfo.buf, L7_ENET_MAC_ADDR_LEN);
  }
  else
  {
    (void)usmDbCpdmClientConnStatusNextGet(&macAddr, &macAddr);
  }

  if (sessionTime == L7_TRUE)
  {
    rc = usmDbCpdmClientConnStatusSessionTimeGet(&macAddr, &val);
  }

  if (rc == L7_SUCCESS)
  {
    (void)usmDbTimeToStringConvert(val, tmpBuf, sizeof(tmpBuf));
    osapiSnprintf(buf, sizeof(buf), tmpBuf);
  }

  return buf;
}

/*********************************************************************
* @purpose  Get all CP client MAC addresses in the options drop down 
*           box with the first client as the first option.
*
* @param    context   @b{(input)} EmWeb/Server request context handle
* @param    optionp   @b{(output)} Dynamic Select Support
* @param    iterator  @b{(input)} Pointer to the current iteration
*
* @returns  Option
*
* @end
*********************************************************************/
void *ewaFormSelect_cp_clientMac(EwsContext context, EwsFormSelectOptionP optionp, void *iterator)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 * bufChoice;
  static L7_enetMacAddr_t selected_clientMac;
  static L7_enetMacAddr_t macAddr;
  static L7_BOOL flag;
  L7_RC_t rc = L7_FAILURE;

  if (iterator == L7_NULL)
  {
    flag = L7_FALSE;
    memset(&selected_clientMac, 0, sizeof(L7_enetMacAddr_t));
    memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
    rc = usmDbCpdmClientConnStatusNextGet(&macAddr, &macAddr);

    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      memcpy(selected_clientMac.addr, appInfo.buf, L7_ENET_MAC_ADDR_LEN);
    }
    else
    {
      if (rc == L7_SUCCESS)
      {
        memcpy(selected_clientMac.addr, macAddr.addr, L7_ENET_MAC_ADDR_LEN);
      }
    }
  }

  if (flag == L7_TRUE)
  {
    rc = usmDbCpdmClientConnStatusNextGet(&macAddr, &macAddr);
  }
  else
  {
    flag = L7_TRUE;
  }

  if (rc == L7_SUCCESS)
  {
    optionp->valuep = (void *)macAddr.addr;
    bufChoice = ewsContextNetHandle(context)->buffer;
    osapiSnprintf(bufChoice, APP_BUFFER_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x", 
                  macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], 
                  macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
    optionp->choice = bufChoice;

    if (memcmp(macAddr.addr, selected_clientMac.addr, L7_ENET_MAC_ADDR_LEN) == 0)
    {
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }

    return (void *) optionp;
  }

  return L7_NULL;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpClientStatusP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpClientStatus(EwsContext context, EwaForm_cpClientStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_enetMacAddr_t selected_clientMac;
  L7_IP_ADDR_t ipAddr;
  L7_uchar8 userName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_LOGIN_TYPE_t protocol;
  CP_VERIFY_MODE_t verifyMode;
  L7_uint32 intfId;
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 name[USMWEB_APP_DATA_SIZE+1];
  L7_char8 tmpBuf[USMWEB_APP_DATA_SIZE+1];
  cpId_t cpId;
  static L7_uchar8 null_mac[] = {0,0,0,0,0,0};

  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  memset(&selected_clientMac, 0, sizeof(L7_enetMacAddr_t));

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    memcpy(&selected_clientMac, appInfo.buf, sizeof(L7_enetMacAddr_t));
    memcpy(form->value.mac, appInfo.buf, sizeof(L7_enetMacAddr_t));
    form->value.err_flag = appInfo.err.err_flag;
    form->value.err_msg = osapiStrDup(appInfo.err.msg);
    form->status.err_msg |= (EW_FORM_DYNAMIC);
  }
  else
  {
    if (usmDbCpdmClientConnStatusNextGet(&selected_clientMac, &selected_clientMac) == L7_SUCCESS)
    {
      memcpy(form->value.mac, selected_clientMac.addr, sizeof(L7_enetMacAddr_t));
    }
  }

  if (memcmp(&selected_clientMac, null_mac, sizeof(L7_enetMacAddr_t)) != 0)
  {
    /* client ip address */
    memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
    if (usmDbCpdmClientConnStatusIpGet(&selected_clientMac, &ipAddr) == L7_SUCCESS)
    {
      form->value.clientIp = ipAddr;
      form->status.clientIp |= (EW_FORM_INITIALIZED);
    }

    if (usmDbCpdmClusterSupportGet() == L7_SUCCESS)
    {
      L7_enetMacAddr_t macAddr;
      cpdmAuthWhichSwitch_t type;

      /* switch mac address */
      memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
      if (usmDbCpdmClientConnStatusSwitchMacAddrGet(&selected_clientMac, &macAddr) == L7_SUCCESS)
      {
        memcpy(form->value.swMac, macAddr.addr, L7_ENET_MAC_ADDR_LEN);
        form->status.swMac |= (EW_FORM_INITIALIZED);
      }

      /* switch ip address */
      memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
      if (usmDbCpdmClientConnStatusSwitchIpGet(&selected_clientMac, &ipAddr) == L7_SUCCESS)
      {
        form->value.swIp = ipAddr;
        form->status.swIp |= (EW_FORM_INITIALIZED);
      }

      /* switch type */
      if (usmDbCpdmClientConnStatusWhichSwitchGet(&selected_clientMac, &type) == L7_SUCCESS)
      {
        if (type == CP_AUTH_ON_LOCAL_SWITCH)
        {
          form->value.swType = osapiStrDup(pStrInfo_security_Local);
        }
        else
        {
          form->value.swType = osapiStrDup(pStrInfo_security_Peer);
        }
        form->status.swType |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
      }
    }

    /* user name */
    if (usmDbCpdmClientConnStatusUserNameGet(&selected_clientMac, userName) == L7_SUCCESS)
    {
      form->value.userName = osapiStrDup(userName);
      form->status.userName |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    /* protocol */
    if (usmDbCpdmClientConnStatusProtocolModeGet(&selected_clientMac, &protocol) == L7_SUCCESS)
    {
      if (protocol == L7_LOGIN_TYPE_HTTP)
      {
        form->value.protocol = osapiStrDup(pStrInfo_security_ProtocolHTTP);
      }
      else
      {
        form->value.protocol = osapiStrDup(pStrInfo_security_ProtocolHTTPS);
      }
      form->status.protocol |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    /* verification */
    if (usmDbCpdmClientConnStatusVerifyModeGet(&selected_clientMac, &verifyMode) == L7_SUCCESS)
    {
      if (verifyMode == CP_VERIFY_MODE_LOCAL)
      {
        form->value.verify = osapiStrDup(pStrInfo_security_VerificationModeLocal);
      }
      else if (verifyMode == CP_VERIFY_MODE_RADIUS)
      {
        form->value.verify = osapiStrDup(pStrInfo_security_VerificationModeRadius);
      }
      else
      {
        form->value.verify = osapiStrDup(pStrInfo_security_VerificationModeGuest);
      }
      form->status.verify |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    /* interface */
    if (usmDbCpdmClientConnStatusIntfIdGet(&selected_clientMac, &intfId) == L7_SUCCESS)
    {
      if (usmDbCpimIntfCapabilityDescriptionGet(intfId, ifDescription) == L7_SUCCESS)
      {
        form->value.intf_id_maxlength = strlen(ifDescription);
        form->value.intf_id_size = form->value.intf_id_maxlength+1;
        form->value.intf_id = osapiStrDup(ifDescription);
        form->status.intf_id |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
      }
    }

    /* cp configuration */
    if (usmDbCpdmClientConnStatusCpIdGet(&selected_clientMac, &cpId) == L7_SUCCESS)
    {
	  memset(name, 0, sizeof(name));
	  if (usmDbCpdmCPConfigNameGet(cpId, name) == L7_SUCCESS)
      {
        osapiSnprintf(tmpBuf, sizeof(tmpBuf), "%u-%s", cpId, name);
        form->value.cpCfg_id = osapiStrDup(tmpBuf);
        form->status.cpCfg_id |= (EW_FORM_INITIALIZED);
      }
    }
  }

  form->value.Refresh = osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->status.mac |= (EW_FORM_INITIALIZED);
  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext              context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpClientStatusP form    @b{(input)} pointer to the form
*
* @returns  cp_client_status.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpClientStatus(EwsContext context, EwaForm_cpClientStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0x00, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  if (form->value.refresh_key == 1)
  {
    memcpy(appInfo.buf, form->value.cp_clientMac, L7_ENET_MAC_ADDR_LEN);
  }
  else
  {
    memcpy(appInfo.buf, form->value.mac, L7_ENET_MAC_ADDR_LEN);
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPClientStatus);
}

