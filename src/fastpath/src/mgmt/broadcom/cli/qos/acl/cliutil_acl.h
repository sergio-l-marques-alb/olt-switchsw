/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/acl/cliutil_acl.h
 *
 * @purpose  Header for cliutil_acl.c
 *
 * @component  IS-CLI
 *
 * @comments  None
 *
 * @create  03/24/2004
 *
 * @author  rjindal
 *
 * @end
 *
 **********************************************************************/

#ifndef CLIUTIL_ACL_H
#define CLIUTIL_ACL_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include <commdefs.h>
#include <datatypes.h>

L7_char8 *cliTreeAccessListDscpNodeHelp(void);
L7_RC_t  cliAclConvertDSCPValToString(L7_uint32 dscpVal, L7_char8 * dscpString);
L7_RC_t  cliAclConvertDSCPStringToVal(L7_char8 * dscpString, L7_uint32 * dscpVal);
L7_char8 *cliAclMacAssignedIntfDirListGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 direction);

#endif /* CLIUTIL_ACL_H */
