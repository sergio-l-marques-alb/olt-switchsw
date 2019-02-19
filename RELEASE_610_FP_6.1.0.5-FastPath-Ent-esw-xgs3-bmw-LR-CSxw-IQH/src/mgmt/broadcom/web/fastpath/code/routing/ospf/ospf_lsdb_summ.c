/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/routing/ospf/ospf_lsdb_summ.c
 *
 * @purpose Code in support of the ospf_lsdb_summ.html page
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 06/28/2001
 *
 * @author betsyt
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_routing_common.h"
#include "strlib_routing_web.h"
#include "l7_common.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include <stdio.h>
#include <string.h>
#include "ew_proto.h"
#include "ewnet.h"
#include "util_pstring.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_ospf_api.h"
#include "l3_commdefs.h"
#include "log.h"
#include "web.h"
#include "l3_web.h"
#include "web_oem.h"

static L7_char8 usmWebBigBuf[1024];

/*********************************************************************
*
* @purpose Get the lsdb Table Info
*
* @param unit  Unit Number
*
* @returns usmWebBigBuf with the lsdb Table Info
*
* @end
*
*********************************************************************/
L7_char8 *usmWebLSDBTableInfoGet(L7_uint32 unit, usmWeb_LsdbInfo_t lsdb_entry)
{
  L7_RC_t rc;
  L7_char8 buf[128],tempBuff[128];

  L7_uint32 val;
  L7_uchar8 byteOptions;

  memset(usmWebBigBuf, 0, sizeof(usmWebBigBuf));
  memset(buf, 0, sizeof(buf));
  osapiSnprintf(usmWebBigBuf, sizeof(usmWebBigBuf),  "<tr>\n");

  memset(buf, 0, sizeof(buf));
  memset(tempBuff,0x00,sizeof(tempBuff));
  rc = usmDbInetNtoa(lsdb_entry.routerId, tempBuff);
  osapiSnprintf(buf, sizeof(buf), " <td class=\"tabledata\"> %s </td>",  tempBuff);
  strcat(usmWebBigBuf, buf);

  memset(buf, 0, sizeof(buf));
  memset(tempBuff,0x00,sizeof(tempBuff));
  rc = usmDbInetNtoa(lsdb_entry.areaId,tempBuff);
  osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>", tempBuff);
  strcat(usmWebBigBuf, buf);

  memset(buf, 0, sizeof(buf));
  memset(tempBuff,0x00,sizeof(tempBuff));
  rc = usmDbInetNtoa(lsdb_entry.Lsid, tempBuff);
  osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>", tempBuff);
  strcat(usmWebBigBuf, buf);
  
  memset(buf, 0, sizeof(buf));

  
  switch (lsdb_entry.type)
  {
  case L7_S_ILLEGAL_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_IllegalLsa);
    break;
  case L7_S_ROUTER_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_RtrLinks_1);
    break;
  case L7_S_NETWORK_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_NwLinks_1);
    break;
  case L7_S_IPNET_SUMMARY_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_NwSummary);
    break;
  case L7_S_ASBR_SUMMARY_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_SummaryAsbr);
    break;
  case L7_S_AS_EXTERNAL_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_AsExternal);
    osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>",tempBuff);
    strcat(usmWebBigBuf, buf);
    memset(buf, 0, sizeof(buf));
    memset(tempBuff,0x00,sizeof(tempBuff));
    lsdb_entry.areaId = 0;
    rc = usmDbInetNtoa(lsdb_entry.areaId,tempBuff);
    osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>", tempBuff);
    strcat(usmWebBigBuf, buf);

    memset(buf, 0, sizeof(buf));
    memset(tempBuff,0x00,sizeof(tempBuff));
    rc = usmDbOspfExtLsdbAgeGet(unit, lsdb_entry.type, lsdb_entry.Lsid, lsdb_entry.routerId, &val);
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%u", val);
    osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>", tempBuff);
    strcat(usmWebBigBuf, buf);

    memset(buf, 0, sizeof(buf));
    memset(tempBuff,0x00,sizeof(tempBuff));
    rc = usmDbOspfExtLsdbSequenceGet(unit, lsdb_entry.type, lsdb_entry.Lsid, lsdb_entry.routerId, &val);
    osapiSnprintf(tempBuff, sizeof(tempBuff), "%#x", val);
    osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>",  tempBuff);
    strcat(usmWebBigBuf, buf);

    memset(buf, 0, sizeof(buf));
    memset(tempBuff,0x00,sizeof(tempBuff));
    rc = usmDbOspfExtLsdbChecksumGet(unit, lsdb_entry.type, lsdb_entry.Lsid, lsdb_entry.routerId, &val);
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%#x", val);
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR,tempBuff, sizeof(buf), pStrInfo_common_Nbsp);
    osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>",  tempBuff);
    strcat(usmWebBigBuf, buf);
    break;
  case L7_S_GROUP_MEMBER_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_McastGrp);
    break;
  case L7_S_NSSA_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_NssaExternal);
    break;
  case L7_S_TMP2_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_TempLsa2);
    break;
  case L7_S_LINK_OPAQUE_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_LinkOpaque);
    break;
  case L7_S_AREA_OPAQUE_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_AreaOpaque);
    break;
  case L7_S_AS_OPAQUE_LSA:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_AsOpaque);
    break;
  case L7_S_LAST_LSA_TYPE:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_LastLsa);
    break;
  default:
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%s ", pStrInfo_common_NotApplicable);
    break;
  }
  if (lsdb_entry.type != L7_S_AS_EXTERNAL_LSA)
  {
  osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>", tempBuff);
  strcat(usmWebBigBuf, buf); 


    memset(buf, 0, sizeof(buf));
    memset(tempBuff,0x00,sizeof(tempBuff));
    rc = usmDbOspfLsdbAgeGet(unit, lsdb_entry.areaId, lsdb_entry.type, lsdb_entry.Lsid, lsdb_entry.routerId, &val);
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%u", val);
     osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>",tempBuff);
     strcat(usmWebBigBuf, buf);


    memset(buf, 0, sizeof(buf));
    memset(tempBuff,0x00,sizeof(tempBuff));
    rc = usmDbOspfLsdbSequenceGet(unit, lsdb_entry.areaId, lsdb_entry.type, lsdb_entry.Lsid, lsdb_entry.routerId, &val);
    osapiSnprintf(tempBuff, sizeof(tempBuff), "%#x", val);
    osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>",tempBuff);
    strcat(usmWebBigBuf, buf);

    memset(buf, 0, sizeof(buf));
    memset(tempBuff,0x00,sizeof(tempBuff));
    rc = usmDbOspfLsdbChecksumGet(unit, lsdb_entry.areaId, lsdb_entry.type, lsdb_entry.Lsid, lsdb_entry.routerId, &val);
    osapiSnprintf(tempBuff, sizeof(tempBuff),  "%#x", val);
    osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>",tempBuff);
    strcat(usmWebBigBuf, buf);

    memset(buf, 0, sizeof(buf));
    if (usmDbOspfAreaLSADbLSAOptionsGet(unit, lsdb_entry.areaId, lsdb_entry.type, lsdb_entry.Lsid, lsdb_entry.routerId, &byteOptions) == L7_SUCCESS)
    {
      if ((L7_OSPF_OPT_Q_BIT & byteOptions) != 0)
      {
        osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR,tempBuff, sizeof(buf), pStrInfo_common_Q);
      }
      else
      {
        osapiSnprintf(tempBuff, sizeof(tempBuff), pStrInfo_common_Dash);
      }

      if ((L7_OSPF_OPT_E_BIT & byteOptions) != 0)
      {
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, tempBuff, pStrInfo_common_E);
      }
      else
      {
        OSAPI_STRNCAT(tempBuff, pStrInfo_common_Dash);
      }

      if ((L7_OSPF_OPT_MC_BIT & byteOptions) != 0)
      {
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR,tempBuff, pStrInfo_common_Mc);
      }
      else
      {
        OSAPI_STRNCAT(tempBuff, pStrInfo_common_Dash);
      }

      if ((L7_OSPF_OPT_O_BIT & byteOptions) != 0)
      {
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, tempBuff, pStrInfo_common_Ipv6TypeOspfIntra);
      }
      else
      {
        OSAPI_STRNCAT(tempBuff, pStrInfo_common_Dash);
      }

      if ((L7_VPN_OPTION_BIT & byteOptions) != 0)
      {
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, tempBuff, pStrInfo_common_V_2);
      }
      else
      {
        OSAPI_STRNCAT(tempBuff, pStrInfo_common_Dash);
      }

    }
    else
    {
      osapiSnprintf(tempBuff, sizeof(buf), pStrErr_common_Err);
    }
   osapiSnprintf(buf, sizeof(buf),  " <td class=\"tabledata\"> %s </td>", tempBuff);
   strcat(usmWebBigBuf, buf);
  }

   strcat(usmWebBigBuf, "</tr>\n");
  
  return usmWebBigBuf;
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the ospf_lsdb_summ form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_ospf_lsdb_summ( EwsContext context, EwaForm_ospf_lsdb_summP form)
{

  form->value.Refresh = (char *) osapiStrDup( pStrInfo_common_Refresh );
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the ospf_lsdb_summ form
*
* @returns ospf_lsdb_summ.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_ospf_lsdb_summ(EwsContext context, EwaForm_ospf_lsdb_summP form)
{
  if (form->status.Refresh & EW_FORM_RETURNED)
  {
    ewsContextSendReply(context, pStrInfo_routing_HtmlFileOspfLsdbSumm);
    return NULL;
  }
  else
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "# (ospf_lsdb_summ.html) Form Submission Failed. "
				"No Action Taken. The form submission failed and no action is taken. # (ospf_lsdb_summ.html) "
				"indicates the file under consideration.\n");
    return NULL;
  }
}
