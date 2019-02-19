/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/security/captive_portal/cp_web_utils.c
 *
 * @purpose captive portal utilites
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  02/26/2008
 *
 * @author  darsen
 * @end
 *
 **********************************************************************/

#include "l7_common.h"
#include "web.h"
#include "ew_proto.h"
#include "ew_config.h"
#include "ewnet.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "util_pstring.h"


/*********************************************************************
* @purpose  Set the specific form integer value after testing validity
*
* @param    L7_uchar8        formStatus @b{(input)} form status
* @param    L7_uint32        formValue  @b{(input)} form value
* @param    L7_char8         *pStrInfo  @b{(input)} info string 
* @param    L7_uint32        minValue   @b{(input)} min value
* @param    L7_uint32        maxValue   @b{(input)} max value
* @param    usmWeb_AppInfo_t *appInfo   @b{(input)} pointer to app
* @param    L7_RC_t          *funcPtr   @b{(input)} pointer to function
* @param    L7_ushort16      *id        @b{(input)} uId_t, cpId_t, or webId_t
*
* @returns  none
*
* @end
*********************************************************************/
void cpIntegerSetHelp(L7_uchar8 formStatus, L7_uint32 formValue, 
                                 L7_char8 *pStrInfo, L7_uint32 minValue, L7_uint32 maxValue, 
                                 usmWeb_AppInfo_t *appInfo, L7_RC_t (* funcPtr)(L7_ushort16, L7_uint32),
                                 L7_ushort16 id)
{
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];

  if (formStatus & EW_FORM_PARSE_ERROR)
  {
    osapiSnprintfAddBlanks(0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg),
                           pStrErr_common_FieldInvalidFmt, pStrInfo);
  }
  else if ((formValue < minValue) || (formValue > maxValue))
  {
    osapiSnprintfAddBlanks(0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg),
                           pStrErr_common_FieldOutOfRange, (L7_int32)formValue, pStrInfo, minValue, maxValue);
  }
  else
  {
    if ((*funcPtr)(id, formValue) == L7_SUCCESS)
    {
      return;
    }
    osapiSnprintfAddBlanks(0, 1, 0, 0, pStrErr_common_FailedToSet, errMsg, sizeof(errMsg),
                           pStrErr_common_FailedToSet_1, pStrInfo);
  }

  appInfo->err.err_flag = L7_TRUE;
  osapiStrncat(appInfo->err.msg, errMsg, (sizeof(appInfo->err.msg)-strlen(appInfo->err.msg)-1));
  return;
}
