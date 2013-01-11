/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/chassis/oem_chassis.c
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
#include "strlib_chassis_common.h"
#include "strlib_chassis_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebChassisGetPageHdr1(L7_int32 token);

static L7_BOOL local_call=L7_FALSE;
L7_char8 *usmWebChassisGetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebChassisGetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    local_call=L7_TRUE;
    errMsg = usmWebChassisGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebChassisGetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  if( local_call == L7_TRUE )
     return errMsg;
  else
     return usmWebPageHeader1stGet(errMsg);
}

L7_char8 *usmWebChassisGetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}
