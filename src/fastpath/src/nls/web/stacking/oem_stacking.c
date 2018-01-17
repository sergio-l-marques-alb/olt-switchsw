/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/stacking/oem_stacking.c
*
* @purpose    OEM String Helper Functions
*
* @component  WEB
*
* @comments   none
*
* @create     01/10/2007
*
* @author     Rama Sasthri, Kristipati
*
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_stacking_common.h"
#include "strlib_stacking_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebStackingGetPageHdr1(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebStackingGetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 16272:
    errMsg = pStrInfo_stacking_DstImage;
    break;
  case 16273:
    errMsg = pStrInfo_stacking_DstUnit;
    break;
  case 16274:
    errMsg = pStrInfo_stacking_SrcImage;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebStackingGetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    local_call=L7_TRUE;
    errMsg = usmWebStackingGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebStackingGetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1450:
    errMsg = pStrInfo_stacking_UnitCfg;
    break;
  case 1454:
    errMsg = pStrInfo_stacking_FirmwareUpdate;
    break;
  case 1455:
    errMsg = pStrInfo_stacking_StackSummary;
    break;
  case 1459:
    errMsg = pStrInfo_stacking_SupportedSwitches;
    break;
  case 1460:
    errMsg = pStrInfo_stacking_StackPortCounters;
    break;
  case 1461:
    errMsg = pStrInfo_stacking_StackPortCfg;
    break;
  case 1462:
    errMsg = pStrInfo_stacking_StackPortDiagnostics;
    break;
  case 1463:
    errMsg = pStrInfo_stacking_StackPortSummary;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  if( local_call == L7_TRUE )
     return errMsg;
  else
     return usmWebPageHeader1stGet(errMsg);
}

L7_char8 *usmWebStackingGetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2450:
    errMsg = pStrInfo_stacking_HtmlLinkStackingHelpStackingUnitcfg;
    break;
  case 2453:
    errMsg = pStrInfo_stacking_HtmlLinkStackingHelpStackingFirmupdt;
    break;
  case 2454:
    errMsg = pStrInfo_stacking_HtmlLinkStackingHelpStackingStacksumm;
    break;
  case 2459:
    errMsg = pStrInfo_stacking_HtmlLinkStackingHelpStackingSuppunit;
    break;
  case 2460:
    errMsg = pStrInfo_stacking_HtmlLinkStackingHelpStackingStackportconfig;
    break;
  case 2461:
    errMsg = pStrInfo_stacking_HtmlLinkStackingHelpStackingStackport;
    break;
  case 2462:
    errMsg = pStrInfo_stacking_HtmlLinkStackingHelpStackingStackportcounters;
    break;
  case 2463:
    errMsg = pStrInfo_stacking_HtmlLinkStackingHelpStackingStackportdiag;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
