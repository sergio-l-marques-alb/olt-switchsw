/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/dot1x/cliutil_dot1x.c
 *
 * @purpose assorted functions for cli spantree
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   10/30/2002
 *
 * @author  jflanagan
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include <errno.h>
#include "clicommands_dot1x.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_util_api.h"
#include "cliutil_dot1x.h"

/*********************************************************************
 *
 * @purpose Determine if the specified port is valid for dot1x
 *          and generate a failure message if not.
 *
 *
 * @param ewsContext   context for error messages
 * @param unit         the unit index (for a stacking environment)
 * @param iface        the interface to check
 *
 * @return   L7_SUCCESS the interface is valid
 * @return   L7_FAILURE the interface is not a valid type for dot1x.
 *
 * @note An interface is considered valid for dot1x if it is one of
 *       the following types:    USM_PHYSICAL_INTF
 *       In case of failure, the failure message is written to the
 *       cli context.  The caller needs only to return.
 *
 *
 * @end
 *
 ********************************************************************/
L7_RC_t cliDot1xInterfaceValidate(EwsContext ewsContext,
                                  L7_uint32 unit, L7_uint32 iface)
{
  return cliDot1xInterfaceValidateWithMessage(ewsContext, unit, iface, L7_TRUE);
}

/*********************************************************************
 *
 * @purpose Determine if the specified port is valid for dot1x
 *          and generate error message based on message parameter.
 *
 *
 * @param ewsContext   context for error messages
 * @param unit         the unit index (for a stacking environment)
 * @param iface        the interface to check
 * @param message      L7_BOOL generate error message or not
 *
 * @return   L7_SUCCESS the interface is valid
 * @return   L7_FAILURE the interface is not a valid type for dot1s.
 *
 * @note An interface is considered valid for dot1s if it is one of
 *       the following types:    USM_PHYSICAL_INTF
 *       In case of failure, the failure message is written to the
 *       cli context.  The caller needs only to return.
 *
 *
 * @end
 *
 ********************************************************************/
L7_RC_t cliDot1xInterfaceValidateWithMessage(EwsContext ewsContext,
                                             L7_uint32 unit, L7_uint32 iface,
                                             L7_BOOL message )
{
  if ( usmDbDot1xInterfaceValidate( unit, iface ) != L7_SUCCESS )
  {
    /* Invalid port */
    if (message == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortModeInvalid);
      cliSyntaxBottom(ewsContext);
    }
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
