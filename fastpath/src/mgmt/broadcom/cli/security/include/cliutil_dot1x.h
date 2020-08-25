/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/dot1x/cliutil_dot1x.h
 *
 * @purpose header for cliutil_dot1x.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   3/04/2003
 *
 * @author  jlanagan
 * @end
 *
 **********************************************************************/

#ifndef CLIUTIL_DOT1X_H
#define CLIUTIL_DOT1X_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "commdefs.h"
#include "datatypes.h"

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
                                  L7_uint32 unit, L7_uint32 iface);

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
                                             L7_BOOL message );

#endif
