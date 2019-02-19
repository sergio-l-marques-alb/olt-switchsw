/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/switching/mfdb/mfdb_igmp.c
 *
 * @purpose Code in support of the mfdb_igmp.html page
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 05/17/2002
 *
 * @author betsyt
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_switching_common.h"
#include "strlib_switching_web.h"
#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_util_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ew_proto.h"
#include "ewnet.h"

#include "usmdb_counters_api.h"
#include "web.h"
#include "log.h"
#include "web_oem.h"
#include "util_pstring.h"

static L7_char8 usmWebBigBuf[1024];

/*********************************************************************
*
* @purpose Get the Multicast Forwarding Datatbase Table Info
*
* @param unit  Unit Number
*
* @returns usmWebBigBuf with the MFDB table Info
*
* @end
*
*********************************************************************/
L7_char8 *usmWebMfdbIgmpTableInfoGet(L7_uint32 unit, usmWeb_MfdbEntry_t *entry)
{
  L7_RC_t rc;
  L7_char8 type[36];
  L7_uint32 listFor[256];
  L7_uint32 listFilt[256];
  L7_char8 stat[80];
  static L7_uchar8 prevMac[L7_MFDB_KEY_SIZE];
  L7_char8 interfaceListFor[USMWEB_BUFFER_SIZE_256];
  L7_char8 interfaceListFilt[USMWEB_BUFFER_SIZE_256];
  L7_BOOL commaFlag = L7_FALSE;
  L7_BOOL empty = L7_TRUE;
  L7_uint32 u, s, p, i, numFwd, numFlt;

  memset(usmWebBigBuf, 0, sizeof(usmWebBigBuf));
  memset(prevMac, 0, sizeof(prevMac));
  memset(type, 0, sizeof(type));
  memset(interfaceListFor, 0, sizeof(interfaceListFor));
  memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
  memset(stat, 0, sizeof(stat));
  memset(listFor, 0, sizeof(listFor));
  memset(listFilt, 0, sizeof(listFilt));
  switch (entry->userEntry.usmdbMfdbType)
  {
  case L7_MFDB_TYPE_STATIC:
    sprintf(type, pStrInfo_common_PimSmGrpRpMapStatic);
    break;
  case L7_MFDB_TYPE_DYNAMIC:
    sprintf(type, pStrInfo_common_Dyn_1);
    break;
  default:
    sprintf(type, pStrInfo_common_Unknown_1);
    break;
  }

  rc = usmDbConvertMaskToList(&entry->userEntry.usmdbMfdbFwdMask, listFor, &numFwd);
  rc = usmDbConvertMaskToList(&entry->userEntry.usmdbMfdbFltMask, listFilt, &numFlt);

  commaFlag = L7_FALSE;
  strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, interfaceListFor, pStrInfo_common_Fwd_2);

  for (i=1; i <= numFwd; i++)
  {
    if ( usmDbUnitSlotPortGet(listFor[i], &u, &s, &p) == L7_SUCCESS)
    {
      sprintf (stat, strUtilUspGet (unit, u, s, p, L7_NULLPTR, 0));

      if (commaFlag == L7_TRUE)
      {
        strcat(interfaceListFor, ", ");
      }
      commaFlag = L7_TRUE;
      strcat(interfaceListFor, stat);
    }
  }
  if (commaFlag == L7_FALSE)
  {
    empty = L7_TRUE;
  }
  else
  {empty = L7_FALSE;}

  /* filtering list*/
  commaFlag = L7_FALSE;
  strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, interfaceListFilt, pStrInfo_common_Flt);

  for (i=1; i <= numFlt; i++)
  {
    if ( usmDbUnitSlotPortGet(listFilt[i], &u, &s, &p) == L7_SUCCESS)
    {
      sprintf (stat, strUtilUspGet (unit, u, s, p, L7_NULLPTR, 0));

      if (commaFlag == L7_TRUE)
      {
        strcat(interfaceListFilt, ", ");
      }
      commaFlag = L7_TRUE;
      strcat(interfaceListFilt, stat);
    }
  }
  if (commaFlag == L7_FALSE)
  {
    sprintf(interfaceListFilt, " ");
  }
  else
  {
    if (empty == L7_FALSE)
    {
      if ((sizeof(interfaceListFor)-strlen(interfaceListFor)-1) >= strlen(pStrInfo_common_Br_4))
      {
        strcat(interfaceListFor, pStrInfo_common_Br_4);
      }
    }
    strcat(interfaceListFor, interfaceListFilt);
  }

  osapiSnprintf(stat, sizeof(stat), entry->userEntry.usmdbMfdbDescr);

  sprintf(usmWebBigBuf, "%s\n  %s%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X%s\n   %s%s %s \n%s %s%s\n%s%s %s \n%s",
          "<tr>",
          "<td CLASS=\"tabledata\">",
          entry->mfdbEntry.usmdbMfdbVidMac[0],
          entry->mfdbEntry.usmdbMfdbVidMac[1],
          entry->mfdbEntry.usmdbMfdbVidMac[2],
          entry->mfdbEntry.usmdbMfdbVidMac[3],
          entry->mfdbEntry.usmdbMfdbVidMac[4],
          entry->mfdbEntry.usmdbMfdbVidMac[5],
          entry->mfdbEntry.usmdbMfdbVidMac[6],
          entry->mfdbEntry.usmdbMfdbVidMac[7],
          "</td>",
          "<td CLASS=\"tabledata\">",type,"</td>",
          "<td CLASS=\"tabledata\">",stat,"</td>",
          "<td CLASS=\"tabledata\">",interfaceListFor,"</td>",
          "</tr>");

  return usmWebBigBuf;
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mfdb_igmp form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_mfdb_igmp(EwsContext context, EwaForm_mfdb_igmpP form)
{
  form->value.Refresh = (char *) osapiStrDup( pStrInfo_common_Refresh );
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.Clear = (char *) osapiStrDup( pStrInfo_switching_ClrEntries );
  form->status.Clear |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the route_table form
*
* @returns mfdb_igmp.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_mfdb_igmp ( EwsContext context, EwaForm_mfdb_igmpP form )
{
  EwaNetHandle net;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uchar8 family = L7_AF_INET;

  unit = usmDbThisUnitGet();

  if (form->status.Clear & EW_FORM_RETURNED)
  {
    rc = usmDbSnoopEntriesFlush(unit, family);
  }
  net = ewsContextNetHandle(context);
  ewsContextSendReply(context, pStrInfo_switching_HtmlFileMfdbIgmp);
  return NULL;
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mfdb_mld form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_mfdb_mld(EwsContext context, EwaForm_mfdb_mldP form)
{
  form->value.Refresh = (char *) osapiStrDup( pStrInfo_common_Refresh );
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.Clear = (char *) osapiStrDup( pStrInfo_switching_ClrEntries );
  form->status.Clear |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the route_table form
*
* @returns mfdb_mld.html
*
* @end
*
*********************************************************************/
char *ewaFormSubmit_mfdb_mld ( EwsContext context, EwaForm_mfdb_mldP form )
{
  EwaNetHandle net;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uchar8 family = L7_AF_INET6;

  unit = usmDbThisUnitGet();

  if (form->status.Clear & EW_FORM_RETURNED)
  {
    rc = usmDbSnoopEntriesFlush(unit, family);
  }
  net = ewsContextNetHandle(context);
  ewsContextSendReply(context, pStrInfo_switching_HtmlFileMfdbMld);
  return NULL;
}
