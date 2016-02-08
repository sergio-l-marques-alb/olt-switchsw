/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/acl/cli_tree_acl.c
 *
 * @purpose acl cli functions that allow users to move up the tree
 *
 * @component user interface
 *
 * @comments
 *
 * @create  08/23/2002
 *
 * @author  djohnson
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include <cliapi.h>

#include "clicommands_acl.h"

const char *cliConfigAcl(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDepth(CLICONFIGACL_DEPTH, CLICONFIGACL_PROMPT, CLICONFIGACL_UPPROMPT, argc, argv, ewsContext);
}

const char *cliConfigAclRule(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDepth(CLICONFIGACLRULE_DEPTH, CLICONFIGACLRULE_PROMPT, CLICONFIGACLRULE_UPPROMPT, argc, argv, ewsContext);
}

const char *cliConfigAclRuleMatch(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDepth(CLICONFIGACLRULEMATCH_DEPTH, CLICONFIGACLRULEMATCH_PROMPT, CLICONFIGACLRULEMATCH_UPPROMPT, argc, argv, ewsContext);
}

const char *cliConfigAclRuleMatchDstL4Port(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDepth(CLICONFIGACLRULEMATCHDSTL4_DEPTH, CLICONFIGACLRULEMATCHDSTL4_PROMPT, CLICONFIGACLRULEMATCHDSTL4_UPPROMPT, argc, argv, ewsContext);
}

const char *cliConfigAclRuleMatchSrcL4Port(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDepth(CLICONFIGACLRULEMATCHSRCL4_DEPTH, CLICONFIGACLRULEMATCHSRCL4_PROMPT, CLICONFIGACLRULEMATCHSRCL4_UPPROMPT, argc, argv, ewsContext);
}

const char *cliConfigAclRuleMatchProtocol(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDepth(CLICONFIGACLRULEMATCHPROT_DEPTH,CLICONFIGACLRULEMATCHPROT_PROMPT,CLICONFIGACLRULEMATCHPROT_UPPROMPT, argc, argv, ewsContext);
}

const char *cliConfigAclInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDepth(CLICONFIGACLINTF_DEPTH,CLICONFIGACLINTF_PROMPT,CLICONFIGACLINTF_UPPROMPT, argc, argv, ewsContext);
}

const char *cliShowAcl(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDepth(CLISHOWACL_DEPTH, CLISHOWACL_PROMPT, CLISHOWACL_UPPROMPT, argc, argv, ewsContext);
}
