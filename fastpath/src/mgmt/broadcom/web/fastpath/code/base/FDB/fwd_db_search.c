/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/base/system/fwd_db_search.c
 *
 * @purpose Code in support of the fwd_db_search.html page
 *
 * @component unitmgr
 *
 * @comments none
 *
 * @create 01/29/2001
 *
 * @author jlshaw
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_base_common.h"
#include "strlib_base_web.h"
#include <l7_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ew_proto.h>
#include <ewnet.h>
#include <ews_ctxt.h>

#include <web.h>
#include <log.h>
#include "web_oem.h"
#include "usmdb_mib_bridge_api.h"
#include "fdb_exports.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_nim_api.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"

static L7_char8 usmWebBigBuf[1024];

/*********************************************************************
*
* @purpose Get the Macaddress searching for
*
* @param usmWebBigBuf  Used to buffer data for mac address entry
*
* @returns L7_SUCCESS or L7_FAILURE with the Forwarding Database entry
*
* @end
*
*********************************************************************/

L7_RC_t  usmWebMacAddrGet(L7_BOOL search, L7_uchar8 * macBuf, usmdbFdbEntry_t * entry)
{
  L7_BOOL val;
  L7_FDB_TYPE_t type;
  L7_uint32 maclen;
  L7_uint32 unit;
  L7_RC_t rc;

  rc = usmDbUnitMgrMgrNumberGet(&unit);    /*Get the Management unit*/
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (search)
  {
    if (usmDbFDBTypeOfVLGet(unit, &type) == L7_SUCCESS)
    {
      if (type == L7_SVL)
      {
        maclen = 6;
      }
      else
      {
        maclen = 8;
      }
    }
    else
    {
      maclen = 8;
    }                     /* Default to IVL */

    val = usmWebConvertMac(maclen, macBuf, entry->usmdbFdbMac);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the Forwarding Database
*
* @param unit  Unit Number
*
* @param lvl7_flag  Flag for emweb iteration
*
* @param usmWebBigBuf  Used to buffer data for mac address entry
*
* @returns L7_SUCCESS or L7_FAILURE with the Forwarding Database entry
*
* @end
*
*********************************************************************/

L7_char8 *usmWebFDBSearch(L7_uint32 lvl7_flag, L7_uint32 unit, L7_uint32 filter, usmdbFdbEntry_t fdbEntry)
{
  L7_uint32 val, slot, port;
  L7_char8 * status;
  L7_FDB_TYPE_t type;
  L7_RC_t rc;

  memset(usmWebBigBuf, 0, 1024);
  if ((filter != L7_FALSE) && (fdbEntry.usmdbFdbEntryType != L7_FDB_ADDR_FLAG_LEARNED) )
  {
    return L7_NULLPTR;
  }
  /* First time through the iterate */
  else if (lvl7_flag == L7_TRUE)
  {
    /* Get data pertaining to mac address */
    rc = usmDbIfIndexGet(unit, fdbEntry.usmdbFdbIntIfNum, &val);
    rc = usmDbUnitSlotPortGet(fdbEntry.usmdbFdbIntIfNum, &unit, &slot, &port);
    /*checks if type is other than learned */
    if (filter != L7_TRUE)
    {
      switch (fdbEntry.usmdbFdbEntryType)
      {
      case L7_FDB_ADDR_FLAG_STATIC:
        status = pStrInfo_common_PimSmGrpRpMapStatic;         /* Static */
        break;
      case L7_FDB_ADDR_FLAG_LEARNED:
        status = pStrInfo_base_Learned;         /* Learned */
        break;
      case L7_FDB_ADDR_FLAG_SELF:
        status = pStrInfo_base_Self;         /* Self */
        break;
      case L7_FDB_ADDR_FLAG_MANAGEMENT:
        status = pStrInfo_base_Mgmt;         /* Management*/
        break;
      case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
        status = pStrInfo_base_GmrpLearned;         /* GMRP Learned */
        break;
      default:
        status = pStrInfo_common_Other;         /* Other */
        break;
      }
    }     /*if type is learned */
    else
    {status = pStrInfo_base_Learned;}            /* learned */

    rc = usmDbFDBTypeOfVLGet(unit, &type);
    if (type == L7_SVL)
    {
      if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
      {
        sprintf(usmWebBigBuf, "%s\n  %s%02X:%02X:%02X:%02X:%02X:%02X%s\n  %s%d/%d/%d%s\n  %s%d%s\n  %s%s%s\n%s",
                "<tr>", "<TD CLASS=\"tabledata\">",
                fdbEntry.usmdbFdbMac[0],
                fdbEntry.usmdbFdbMac[1],
                fdbEntry.usmdbFdbMac[2],
                fdbEntry.usmdbFdbMac[3],
                fdbEntry.usmdbFdbMac[4],
                fdbEntry.usmdbFdbMac[5],
                "</td>", "<TD CLASS=\"tabledata\">", unit, slot, port,
                "</td>", "<TD CLASS=\"tabledata\">", val,
                "</td>", "<TD CLASS=\"tabledata\">", status,
                "</td>", "</tr>");
      }
      else
      {
        sprintf(usmWebBigBuf, "%s\n  %s%02X:%02X:%02X:%02X:%02X:%02X%s\n  %s%d/%d%s\n  %s%d%s\n  %s%s%s\n%s",
                "<tr>", "<TD CLASS=\"tabledata\">",
                fdbEntry.usmdbFdbMac[0],
                fdbEntry.usmdbFdbMac[1],
                fdbEntry.usmdbFdbMac[2],
                fdbEntry.usmdbFdbMac[3],
                fdbEntry.usmdbFdbMac[4],
                fdbEntry.usmdbFdbMac[5],
                "</td>", "<TD CLASS=\"tabledata\">", slot, port,
                "</td>", "<TD CLASS=\"tabledata\">", val,
                "</td>", "<TD CLASS=\"tabledata\">", status,
                "</td>", "</tr>");
      }
    }
    else
    {
      if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
      {
        sprintf(usmWebBigBuf, "%s\n  %s%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X%s\n  %s%d/%d/%d%s\n  %s%d%s\n  %s%s%s\n%s",
                "<tr>", "<TD CLASS=\"tabledata\">",
                fdbEntry.usmdbFdbMac[0],
                fdbEntry.usmdbFdbMac[1],
                fdbEntry.usmdbFdbMac[2],
                fdbEntry.usmdbFdbMac[3],
                fdbEntry.usmdbFdbMac[4],
                fdbEntry.usmdbFdbMac[5],
                fdbEntry.usmdbFdbMac[6],
                fdbEntry.usmdbFdbMac[7],
                "</td>", "<TD CLASS=\"tabledata\">", unit, slot, port,
                "</td>", "<TD CLASS=\"tabledata\">", val,
                "</td>", "<TD CLASS=\"tabledata\">", status,
                "</td>", "</tr>");
      }
      else
      {
        sprintf(usmWebBigBuf, "%s\n  %s%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X%s\n  %s%d/%d%s\n  %s%d%s\n  %s%s%s\n%s",
                "<tr>", "<TD CLASS=\"tabledata\">",
                fdbEntry.usmdbFdbMac[0],
                fdbEntry.usmdbFdbMac[1],
                fdbEntry.usmdbFdbMac[2],
                fdbEntry.usmdbFdbMac[3],
                fdbEntry.usmdbFdbMac[4],
                fdbEntry.usmdbFdbMac[5],
                fdbEntry.usmdbFdbMac[6],
                fdbEntry.usmdbFdbMac[7],
                "</td>", "<TD CLASS=\"tabledata\">", slot, port,
                "</td>", "<TD CLASS=\"tabledata\">", val,
                "</td>", "<TD CLASS=\"tabledata\">", status,
                "</td>", "</tr>");
      }
    }
  }
  else
  {   /* After first time through */
    rc = usmDbIfIndexGet(unit, fdbEntry.usmdbFdbIntIfNum, &val);
    rc = usmDbUnitSlotPortGet(fdbEntry.usmdbFdbIntIfNum, &unit, &slot, &port);
    /*check if the type is not learned */
    if (filter != L7_TRUE)
    {
      switch (fdbEntry.usmdbFdbEntryType)
      {
      case L7_FDB_ADDR_FLAG_STATIC:
        status = pStrInfo_common_PimSmGrpRpMapStatic;         /* Static */
        break;
      case L7_FDB_ADDR_FLAG_LEARNED:
        status = pStrInfo_base_Learned;         /* Learned */
        break;
      case L7_FDB_ADDR_FLAG_SELF:
        status = pStrInfo_base_Self;         /* Self */
        break;
      case L7_FDB_ADDR_FLAG_MANAGEMENT:
        status = pStrInfo_base_Mgmt;         /* Management*/
        break;
      case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
        status = pStrInfo_base_GmrpLearned;         /* GMRP Learned */
        break;
      default:
        status = pStrInfo_common_Other;         /* Other */
        break;
      }
    }    /*if the type is learned */
    else
    {status = pStrInfo_base_Learned;}               /*learned */

    /* Copies all of the HTML data necessary to provide MAC address statistics into usmWebBigBuf */
    rc = usmDbFDBTypeOfVLGet(unit, &type);
    if (type == L7_SVL)
    {
      if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
      {
        sprintf(usmWebBigBuf, "%s\n  %s%02X:%02X:%02X:%02X:%02X:%02X%s\n  %s%d/%d/%d%s\n  %s%d%s\n  %s%s%s\n%s",
                "<tr>", "<TD CLASS=\"tabledata\">",
                fdbEntry.usmdbFdbMac[0],
                fdbEntry.usmdbFdbMac[1],
                fdbEntry.usmdbFdbMac[2],
                fdbEntry.usmdbFdbMac[3],
                fdbEntry.usmdbFdbMac[4],
                fdbEntry.usmdbFdbMac[5],
                "</td>", "<TD CLASS=\"tabledata\">", unit, slot, port,
                "</td>", "<TD CLASS=\"tabledata\">", val,
                "</td>", "<TD CLASS=\"tabledata\">", status,
                "</td>", "</tr>");
      }
      else
      {
        sprintf(usmWebBigBuf, "%s\n  %s%02X:%02X:%02X:%02X:%02X:%02X%s\n  %s%d/%d%s\n  %s%d%s\n  %s%s%s\n%s",
                "<tr>", "<TD CLASS=\"tabledata\">",
                fdbEntry.usmdbFdbMac[0],
                fdbEntry.usmdbFdbMac[1],
                fdbEntry.usmdbFdbMac[2],
                fdbEntry.usmdbFdbMac[3],
                fdbEntry.usmdbFdbMac[4],
                fdbEntry.usmdbFdbMac[5],
                "</td>", "<TD CLASS=\"tabledata\">", slot, port,
                "</td>", "<TD CLASS=\"tabledata\">", val,
                "</td>", "<TD CLASS=\"tabledata\">", status,
                "</td>", "</tr>");
      }
    }
    else
    {
      if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
      {
        sprintf(usmWebBigBuf, "%s\n  %s%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X%s\n  %s%d/%d/%d%s\n  %s%d%s\n  %s%s%s\n%s",
                "<tr>", "<TD CLASS=\"tabledata\">",
                fdbEntry.usmdbFdbMac[0],
                fdbEntry.usmdbFdbMac[1],
                fdbEntry.usmdbFdbMac[2],
                fdbEntry.usmdbFdbMac[3],
                fdbEntry.usmdbFdbMac[4],
                fdbEntry.usmdbFdbMac[5],
                fdbEntry.usmdbFdbMac[6],
                fdbEntry.usmdbFdbMac[7],
                "</td>", "<TD CLASS=\"tabledata\">", unit, slot, port,
                "</td>", "<TD CLASS=\"tabledata\">", val,
                "</td>", "<TD CLASS=\"tabledata\">", status,
                "</td>", "</tr>");
      }
      else
      {
        sprintf(usmWebBigBuf, "%s\n  %s%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X%s\n  %s%d/%d%s\n  %s%d%s\n  %s%s%s\n%s",
                "<tr>", "<TD CLASS=\"tabledata\">",
                fdbEntry.usmdbFdbMac[0],
                fdbEntry.usmdbFdbMac[1],
                fdbEntry.usmdbFdbMac[2],
                fdbEntry.usmdbFdbMac[3],
                fdbEntry.usmdbFdbMac[4],
                fdbEntry.usmdbFdbMac[5],
                fdbEntry.usmdbFdbMac[6],
                fdbEntry.usmdbFdbMac[7],
                "</td>", "<TD CLASS=\"tabledata\">", slot, port,
                "</td>", "<TD CLASS=\"tabledata\">", val,
                "</td>", "<TD CLASS=\"tabledata\">", status,
                "</td>", "</tr>");
      }
    }
  }
  return usmWebBigBuf;
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the fwd_db_search form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_fwd_db_search ( EwsContext context, EwaForm_fwd_db_searchP form )
{
  /* Sets initial form data */
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 mgmt_unit;
  L7_RC_t rc;

  form->status.filter |= (EW_FORM_INITIALIZED);

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    form->status.err_flag |= (EW_FORM_INITIALIZED);
    if (appInfo.err.err_flag == L7_TRUE)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = L7_NULL;
    }

    form->value.filter = appInfo.data[0];
    form->value.mactofind = osapiStrDup(appInfo.buf);
    form->status.mactofind |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->status.err_flag |= (EW_FORM_INITIALIZED);
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
    form->value.filter = all;
    form->value.mactofind = L7_NULL;
    form->status.mactofind |= (EW_FORM_INITIALIZED);
  }

  rc = usmDbUnitMgrMgrNumberGet(&mgmt_unit);    /*Get the Management unit*/
  if (rc == L7_SUCCESS)
  {
    form->value.mgmt_unitid = mgmt_unit;
    form->status.mgmt_unitid |= ( EW_FORM_INITIALIZED );
  }

  form->value.Reset = (char *) osapiStrDup( pStrInfo_common_Refresh );
  form->status.Reset |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.macSearch = (char *) osapiStrDup( pStrInfo_common_Search );
  form->status.macSearch |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  return;
}

/*********************************************************************
*
* @purpose Refreshes the data in the fdb table
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the fwd_db_search form
*
* @returns fwd_db_search.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_fwd_db_search ( EwsContext context, EwaForm_fwd_db_searchP form )
{

  L7_uchar8 mac_address[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 mac_hex[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL search = L7_FALSE;
  L7_BOOL rc;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  appInfo.data[0] = form->value.filter;

  if (form->status.Reset & EW_FORM_RETURNED)
  {
    search = L7_FALSE;
  }
  else if (form->status.macSearch & EW_FORM_RETURNED)
  {
    memset(mac_hex, 0, sizeof(mac_hex));
    memset(mac_address, 0, sizeof(mac_address));
    strncpy(mac_address, form->value.mactofind, USMWEB_VLAN_MACADDR_SIZE);
    rc = usmWebConvertMac(8, mac_address, mac_hex);
    if (rc != L7_TRUE)
    {
      usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error, pStrInfo_base_MacAddrSearchNotValid);
      search = L7_FALSE;
    }
    else
    {
      search = L7_TRUE;
      osapiSnprintf(appInfo.buf, USMWEB_APP_BUF_SIZE, form->value.mactofind);
    }
  }

  appInfo.data[1] = search;

  return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_base_HtmlFileFwdDbSearch);
}
