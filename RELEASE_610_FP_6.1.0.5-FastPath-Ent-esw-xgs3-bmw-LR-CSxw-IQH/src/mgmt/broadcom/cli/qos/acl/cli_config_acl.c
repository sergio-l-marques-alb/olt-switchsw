/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/acl/cli_config_acl.c
 *
 * @purpose acl config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/07/2003
 *
 * @author  Jagdish
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "cliapi.h"
#include "cliutil.h"
#include "ews.h"
#include "cli_mode.h"
#include "clicommands_acl.h"
#include "usmdb_qos_acl_api.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_diffserv_api.h"
#include "usmdb_util_api.h"
#include "clicommands_card.h"
#include "cliutil.h"
#include "cliutil_acl.h"
#include "l7_packet.h"

/*********************************************************************
*
* @purpose  To print the acl command syntax
*
* @param EwsContext ewsContext
*
* @returntype void
*
* @notes
* @end
*
*********************************************************************/
void  aclClean(L7_BOOL isAclAdded, L7_uint32 aclId, L7_uint32 ruleNum)
{
  L7_uint32 unit;
  L7_RC_t rc;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if( isAclAdded == L7_TRUE)
  {
    rc = usmDbQosAclDelete(unit, aclId);
  }
  else
  {
    rc = usmDbQosAclRuleRemove(unit, aclId, ruleNum);
  }

  /*No need to check error case. This is garbage cleaner and doesn't
     throw any error message*/
}

/*********************************************************************
*
* @purpose  To print the acl command syntax
*
* @param EwsContext ewsContext
*
* @returntype void
*
* @notes
* @end
*
*********************************************************************/
void  aclSyntaxWrite(EwsContext ewsContext)
{
  L7_char8 cliSyntax[CLIACL_SYNTAXLENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 intfString[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 * cliCurr;
  L7_BOOL more = L7_FALSE;
  L7_BOOL bit = L7_FALSE;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if ( ewsContext->commType == CLI_NO_CMD)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgNoAclCreate);

  }
  else
  {
    memset (cliSyntax, 0, sizeof(cliSyntax));
    cliCurr = cliSyntax;
    strcpyAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, cliCurr, pStrErr_qos_AclPreCommonPreSyntax);
    cliCurr += strlen(cliCurr);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclSrcIp);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclSrcMask);
        cliCurr += strlen(cliCurr);
      }

    }

    strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
    cliCurr += strlen(cliCurr);

    strcpy(cliCurr, pStrErr_common_AclDelSyntax);
    cliCurr += strlen(cliCurr);

    strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclExtSyntax);
    cliCurr += strlen(cliCurr);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID) == L7_TRUE)
    {
      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      strcpy(cliCurr, pStrErr_qos_AclPortsSyntax);
      cliCurr += strlen(cliCurr);

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclSrcIpSyntax);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclSrcMask);
        cliCurr += strlen(cliCurr);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID) == L7_TRUE )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclPortValKeySyntax);
      cliCurr += strlen(cliCurr);

      if ( usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {

        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclRangeSyntax);
        cliCurr += strlen(cliCurr);
      }
      else
      {
        strcpy(cliCurr, pStrErr_qos_AclTosPostSyntax);
        cliCurr += strlen(cliCurr);
      }

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
    {

      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }

      strcpy(cliCurr, pStrErr_qos_AclBeginParenSyntax);
      cliCurr += strlen(cliCurr);

      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclDstIpSyntax);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) ==
          L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclDstMask);
        cliCurr += strlen(cliCurr);
      }

      strcpy(cliCurr, pStrErr_common_AclDelSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID) == L7_TRUE)
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclPortValKeySyntax);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclRangeSyntax);
        cliCurr += strlen(cliCurr);
      }
      else
      {
        strcpy(cliCurr, pStrErr_qos_AclTosPostSyntax);
        cliCurr += strlen(cliCurr);
      }

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID) == L7_TRUE)
    {
      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      bit = L7_TRUE;

      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclPrecedenceSyntax);
      cliCurr += strlen(cliCurr);

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID) == L7_TRUE)
    {

      if( bit == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 1, L7_NULLPTR, cliCurr, pStrErr_qos_AclMoreSyntax);
        cliCurr += strlen(cliCurr);
      }

      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclTosPreSyntax);
      cliCurr += strlen(cliCurr);
      bit = L7_TRUE;

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE )
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclTosBitsSyntax);
        cliCurr += strlen(cliCurr);
      }
      strcpy(cliCurr, pStrErr_qos_AclTosPostSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID) == L7_TRUE )
    {
      if( bit == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 1, L7_NULLPTR, cliCurr, pStrErr_qos_AclMoreSyntax);
        cliCurr += strlen(cliCurr);
      }
      bit = L7_TRUE;
      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      strcpy(cliCurr, pStrErr_qos_AclIpDscp);
      cliCurr += strlen(cliCurr);
    }

    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE) )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclLogsSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclAsSignQueueSyntax);
      cliCurr += strlen(cliCurr);
    }

#ifdef L7_STACKING_PACKAGE
    sprintf(intfString, pStrErr_common_AclIntfsStacking);
#else
    sprintf(intfString, pStrErr_common_AclIntfs);
#endif

    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE ) &&
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE ) )
    {
      sprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, pStrErr_qos_AclMirrorRedirectsSyntax, intfString );
      strcpy(cliCurr, buf);
      cliCurr += strlen(buf);
    }

    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE )
    {
      sprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, pStrErr_qos_AclMirrorSyntax, intfString );
      strcpy(cliCurr, buf);
      cliCurr += strlen(buf);
    }

    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE )
    {
      sprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, pStrErr_qos_AclRedirectsSyntax, intfString );
      strcpy(cliCurr, buf);
      cliCurr += strlen(buf);
    }

    if( bit ==  L7_TRUE)
    {
      strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
      cliCurr += strlen(cliCurr);
    }

    if( more == L7_TRUE)
    {
      strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
      cliCurr += strlen(cliCurr);

    }

    strcpy(cliCurr, pStrErr_qos_AclPort);
    cliCurr += strlen(cliCurr);
    ewsTelnetWrite( ewsContext, cliSyntax);
  }
}

/*********************************************************************
*
* @purpose  To print the acl command syntax
*
* @param EwsContext ewsContext
*
* @returntype void
*
* @notes
* @end
*
*********************************************************************/
void  ipv6AclSyntaxWrite(EwsContext ewsContext)
{
  L7_char8 cliSyntax[CLIACL_SYNTAXLENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 intfString[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 * cliCurr;
  L7_BOOL more = L7_FALSE;
  L7_BOOL bit = L7_FALSE;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if ( ewsContext->commType == CLI_NO_CMD)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgNoAclCreate);

  }
  else
  {
    memset (cliSyntax, 0, sizeof(cliSyntax));
    cliCurr = cliSyntax;
    strcpyAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, cliCurr, pStrErr_qos_AclPreCommonPreSyntax);
    cliCurr += strlen(cliCurr);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclSrcIp);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclSrcMask);
        cliCurr += strlen(cliCurr);
      }

    }

    strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
    cliCurr += strlen(cliCurr);

    strcpy(cliCurr, pStrErr_common_AclDelSyntax);
    cliCurr += strlen(cliCurr);

    strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclExtSyntax);
    cliCurr += strlen(cliCurr);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID) == L7_TRUE)
    {
      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      strcpy(cliCurr, pStrErr_qos_AclPortsSyntax);
      cliCurr += strlen(cliCurr);

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclSrcIpSyntax);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclSrcMask);
        cliCurr += strlen(cliCurr);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID) == L7_TRUE )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclPortValKeySyntax);
      cliCurr += strlen(cliCurr);

      if ( usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {

        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclRangeSyntax);
        cliCurr += strlen(cliCurr);
      }
      else
      {
        strcpy(cliCurr, pStrErr_qos_AclTosPostSyntax);
        cliCurr += strlen(cliCurr);
      }

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
    {

      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }

      strcpy(cliCurr, pStrErr_qos_AclBeginParenSyntax);
      cliCurr += strlen(cliCurr);

      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclDstIpSyntax);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) ==
          L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclDstMask);
        cliCurr += strlen(cliCurr);
      }

      strcpy(cliCurr, pStrErr_common_AclDelSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID) == L7_TRUE)
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclPortValKeySyntax);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclRangeSyntax);
        cliCurr += strlen(cliCurr);
      }
      else
      {
        strcpy(cliCurr, pStrErr_qos_AclTosPostSyntax);
        cliCurr += strlen(cliCurr);
      }

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID) == L7_TRUE)
    {
      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      bit = L7_TRUE;

      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclPrecedenceSyntax);
      cliCurr += strlen(cliCurr);

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID) == L7_TRUE)
    {

      if( bit == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 1, L7_NULLPTR, cliCurr, pStrErr_qos_AclMoreSyntax);
        cliCurr += strlen(cliCurr);
      }

      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclTosPreSyntax);
      cliCurr += strlen(cliCurr);
      bit = L7_TRUE;

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE )
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclTosBitsSyntax);
        cliCurr += strlen(cliCurr);
      }
      strcpy(cliCurr, pStrErr_qos_AclTosPostSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID) == L7_TRUE )
    {
      if( bit == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 1, L7_NULLPTR, cliCurr, pStrErr_qos_AclMoreSyntax);
        cliCurr += strlen(cliCurr);
      }
      bit = L7_TRUE;
      if( more == L7_FALSE)
      {
        strcpy (cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);
        more = L7_TRUE;
      }
      strcpy(cliCurr, pStrErr_qos_AclIpDscp);
      cliCurr += strlen(cliCurr);
    }

    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE) )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclLogsSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclAsSignQueueSyntax);
      cliCurr += strlen(cliCurr);
    }

#ifdef L7_STACKING_PACKAGE
    sprintf(intfString, pStrErr_common_AclIntfsStacking);
#else
    sprintf(intfString, pStrErr_common_AclIntfs);
#endif

    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE ) &&
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE ) )
    {
      sprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, pStrErr_qos_AclMirrorRedirectsSyntax, intfString );
      strcpy(cliCurr, buf);
      cliCurr += strlen(buf);
    }

    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE )
    {
      sprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, pStrErr_qos_AclMirrorSyntax, intfString );
      strcpy(cliCurr, buf);
      cliCurr += strlen(buf);
    }

    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE )
    {
      sprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, pStrErr_qos_AclRedirectsSyntax, intfString );
      strcpy(cliCurr, buf);
      cliCurr += strlen(buf);
    }

    if( bit ==  L7_TRUE)
    {
      strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
      cliCurr += strlen(cliCurr);
    }

    if( more == L7_TRUE)
    {
      strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
      cliCurr += strlen(cliCurr);

    }

    strcpy(cliCurr, pStrErr_qos_AclPort);
    cliCurr += strlen(cliCurr);
    ewsTelnetWrite( ewsContext, cliSyntax);
  }
}
/*********************************************************************
*
* @purpose  To print the acl command syntax
*
* @param EwsContext ewsContext
*
* @returntype void
*
* @notes
* @end
*
*********************************************************************/
void  macAclSyntaxWrite(EwsContext ewsContext)
{
  L7_char8 cliSyntax[CLIACL_SYNTAXLENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 intfString[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 * cliCurr;
  L7_uint32 unit;
  L7_uint32 ADD_BRACE1 = L7_TRUE;
  L7_uint32 ADD_BRACE2 = L7_TRUE;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if ( ewsContext->commType == CLI_NO_CMD)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgNoMacAclCreate);

  }
  else
  {
    memset (cliSyntax, 0, sizeof(cliSyntax));
    cliCurr = cliSyntax;
    strcpyAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, cliCurr, pStrErr_qos_MacAclPreCommonPreSyntax);
    cliCurr += strlen(cliCurr);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE)
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclSrcMacSyntax);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID) == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclSrcMaskNew);
        cliCurr += strlen(cliCurr);
        ADD_BRACE1 = L7_FALSE;
      }

      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr,pStrErr_qos_MacAclExtSyntaxAny);
      cliCurr += strlen(cliCurr);

      if(ADD_BRACE1 == L7_TRUE)
      {
        strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
        cliCurr += strlen(cliCurr);
      }

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE)
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclDstMacSyntax);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) == L7_TRUE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclDstMaskNew);
        cliCurr += strlen(cliCurr);
        ADD_BRACE2 = L7_FALSE;
      }

      if(ADD_BRACE2 == L7_FALSE)
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr,pStrErr_qos_MacAclExtSyntaxAnyNew);
        cliCurr += strlen(cliCurr);

        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclExtSyntaxBpdu);
        cliCurr += strlen(cliCurr);
      }
      else
      {
        strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr,pStrErr_qos_MacAclExtSyntaxAny);
        cliCurr += strlen(cliCurr);
      }

      if(ADD_BRACE2 == L7_TRUE)
      {
        strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
        cliCurr += strlen(cliCurr);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID) == L7_TRUE)
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclEtherTypesSyntax);
      cliCurr += strlen(cliCurr);

    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_VLANID_FEATURE_ID) == L7_TRUE)
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclVlansSyntax);
      cliCurr += strlen(cliCurr);

      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclVlansIn);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID) == L7_TRUE)
      {

        strcpy(cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);

        strcpy(cliCurr, pStrErr_qos_MacAclVlanRangeSyntax);
        cliCurr += strlen(cliCurr);
      }

      strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_COS_FEATURE_ID) == L7_TRUE )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclCosSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclSecondaryVlansSyntax);
      cliCurr += strlen(cliCurr);

      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclVlansIn);
      cliCurr += strlen(cliCurr);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID) == L7_TRUE)
      {

        strcpy(cliCurr, pStrErr_qos_AclOrSyntax);
        cliCurr += strlen(cliCurr);

        strcpy(cliCurr, pStrErr_qos_MacAclVlanRangeSyntax);
        cliCurr += strlen(cliCurr);
      }

      strcpy(cliCurr, pStrErr_qos_AclCloseSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_COS2_FEATURE_ID) == L7_TRUE )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_MacAclSecondaryCosSyntax);
      cliCurr += strlen(cliCurr);
    }

    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE) )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclLogsSyntax);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE )
    {
      strcpyAddBlanks (0, 0, 1, 0, L7_NULLPTR, cliCurr, pStrErr_qos_AclAsSignQueueSyntax);
      cliCurr += strlen(cliCurr);
    }

#ifdef L7_STACKING_PACKAGE
    sprintf(intfString, pStrErr_common_AclIntfsStacking);
#else
    sprintf(intfString, pStrErr_common_AclIntfs);
#endif

    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE ) &&
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE ) )
    {
      sprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, pStrErr_qos_AclMirrorRedirectsSyntax, intfString );
      strcpy(cliCurr, buf);
      cliCurr += strlen(buf);
    }

    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE )
    {
      sprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, pStrErr_qos_AclMirrorSyntax, intfString );
      strcpy(cliCurr, buf);
      cliCurr += strlen(buf);
    }

    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE )
    {
      sprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, pStrErr_qos_AclRedirectsSyntax, intfString );
      strcpy(cliCurr, buf);
      cliCurr += strlen(buf);
    }
  }

  ewsTelnetWrite( ewsContext, cliSyntax);

}

/*********************************************************************
*
* @purpose  To get the matching port value
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
* @param L7_uint32* relIndex
* @param L7_uint32* port
* @param L7_uint32* startPort
* @param L7_uint32* endPort
*
* @returntype L7_RC_t
*
* @returns L7_SUCCESS - For success cases
* @returns L7_FAILURE - For error case
* @end
*
*********************************************************************/
L7_RC_t  getPortRange(EwsContext ewsContext, const L7_char8 * * argv,
                      L7_uint32 index, L7_uint32 * relIndex,
                      L7_int32 * srcPortValue, L7_int32 * srcStartPort,
                      L7_int32 * srcEndPort, L7_BOOL * matchOther)

{
  L7_int32 port = -1;
  L7_uint32 uport = 0;
  L7_uint32 totalArg = 0;
  L7_char8 strKeyword[L7_CLI_MAX_STRING_LENGTH];

  totalArg = cliNumFunctionArgsGet();
  if( strcmp( argv[index + *relIndex], pStrInfo_qos_AclEqstr) ==0)
  {
    *relIndex = *relIndex +1;
    if( totalArg < *relIndex)
    {
      aclSyntaxWrite(ewsContext);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }
    osapiStrncpySafe(strKeyword,argv[index+ *relIndex], sizeof(strKeyword));
    cliConvertToLowerCase(strKeyword);

    if ((cliConvertTo32BitUnsignedInteger(argv[index + *relIndex], &uport) == L7_SUCCESS))
    {
      if(uport < L7_ACL_MIN_L4PORT_NUM || uport > L7_ACL_MAX_L4PORT_NUM)
      {
        /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
        ewsTelnetWrite(ewsContext, pStrErr_qos_StartSrcL4Port);
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }
      else
      {
        port = uport;
      }
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Domain_1) == 0)       /* domain */
    {
      port =  L7_ACL_L4PORT_DOMAIN;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Echo) == 0)        /* echo */
    {
      port = L7_ACL_L4PORT_ECHO;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Ftp) == 0)        /* ftp */
    {
      port =  L7_ACL_L4PORT_FTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Ftpdata) == 0)        /* ftpdata */
    {
      port = L7_ACL_L4PORT_FTPDATA;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Http_1) == 0)        /* http */
    {
      port = L7_ACL_L4PORT_HTTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Smtp) == 0)        /* smtp */
    {
      port = L7_ACL_L4PORT_SMTP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Snmp_1) == 0)        /* snmp */
    {
      port = L7_ACL_L4PORT_SNMP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Telnet) == 0)        /* telnet */
    {
      port = L7_ACL_L4PORT_TELNET;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Tftp_1) == 0)        /* tftp */
    {
      port = L7_ACL_L4PORT_TFTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Www) == 0)        /* www */
    {
      port = L7_ACL_L4PORT_WWW;
    }
    else
    {
      /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_KeywordDomainetc);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }
    *srcPortValue = port;
    *matchOther = L7_TRUE;
    *relIndex = *relIndex +1;
  }
  else if( strcmp(argv[index+ *relIndex],pStrInfo_common_AclRangeStr) ==0)
  {
    *relIndex = *relIndex +1;
    if( totalArg < *relIndex)
    {
      aclSyntaxWrite(ewsContext);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    if ((cliConvertTo32BitUnsignedInteger(argv[index + *relIndex], &uport) != L7_SUCCESS)
        ||(uport < L7_ACL_MIN_L4PORT_NUM) || (uport > L7_ACL_MAX_L4PORT_NUM))
    {
      /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
      ewsTelnetWrite(ewsContext, pStrErr_qos_StartSrcL4Port);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }
    else
    {
      port = uport;
    }

    *srcStartPort = port;
    *relIndex = *relIndex +1;
    if( totalArg < *relIndex)
    {
      aclSyntaxWrite(ewsContext);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    if ((cliConvertTo32BitUnsignedInteger(argv[index + *relIndex], &uport) != L7_SUCCESS)
        ||(uport < L7_ACL_MIN_L4PORT_NUM) || (uport > L7_ACL_MAX_L4PORT_NUM))
    {
      /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
      ewsTelnetWrite(ewsContext, pStrErr_qos_StartSrcL4Port);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }
    else
    {
      port = uport;
    }

    *srcEndPort = port;
    *matchOther = L7_TRUE;
    *relIndex = *relIndex +1;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To check the ACL list type ( ACL standard or extended)
*
* @param L7_uint32 aclId
*
* @returntype L7_uint32
*
* @returns ACL_STANDARD - For standard acl list
* @returns ACL_EXTENDED - For extended acl list.
* @returns ACL_NOTDEFIND -For error cases.
* @end
*
*********************************************************************/
L7_uint32  checkAcltype(L7_uint32 aclId)
{
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return ACL_NOTDEFIND;
  }

  if(( aclId >= L7_ACL_MIN_STD1_ID) && (aclId <= L7_ACL_MAX_STD1_ID))
  {
    return ACL_STANDARD;
  }
  else if (((aclId >= L7_ACL_MIN_EXT1_ID) && (aclId <= L7_ACL_MAX_EXT1_ID)) ||
           (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) == L7_SUCCESS))
  {
    return ACL_EXTENDED;
  }
  else
  {
    return ACL_NOTDEFIND;
  }
}

/*********************************************************************
*
* @purpose  To get the ip address and Mask value
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
* @param L7_uint32* relIndex
* @param L7_uint32* ipAddr
* @param L7_uint32* ipMask
*
* @returntype L7_RC_t
*
* @returns NULL - For error cases
* @returns cliPrompt(ewsContext)- For success
* @end
*
*********************************************************************/
L7_char8 *  getIPAddressAndMask(EwsContext ewsContext,
                                const L7_char8 * * argv,
                                L7_uint32 index,L7_uint32 * relIndex,
                                L7_uint32 * ipAddr,
                                L7_uint32 * ipMask)
{
  L7_uint32 unit;
  L7_uint32 totalArg;
  L7_BOOL maskSupported = L7_FALSE;
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpMask[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
  {
    maskSupported = L7_TRUE;
  }
  totalArg = cliNumFunctionArgsGet();

  if( totalArg < *relIndex)
  {
    aclSyntaxWrite(ewsContext);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  /* Get the rcIp address*/
  if (strlen(argv[index+*relIndex]) >= sizeof(strIpAddr) )
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_TacacsInValIp);          /* Invalid IP address */
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  osapiStrncpySafe( strIpAddr, argv[index+*relIndex], sizeof(strIpAddr));
  *relIndex  = *relIndex+1;
  if (usmDbInetAton(strIpAddr, ipAddr) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_TacacsInValIp);        /* Invalid IP address */
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  /* Get the ip Mask*/
  if( totalArg < *relIndex && maskSupported == L7_TRUE)
  {
    aclSyntaxWrite(ewsContext);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  if( maskSupported == L7_TRUE)
  {
    if (strlen(argv[index+*relIndex]) >= sizeof(strIpMask) )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_IpMask_1);        /* Invalid IP mask */
      cliSyntaxBottom(ewsContext);
      return NULL;
    }

    osapiStrncpySafe( strIpMask, argv[index+*relIndex], sizeof(strIpMask));
    *relIndex  = *relIndex+1;

    if (usmDbInetAton(strIpMask, ipMask) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_IpMask_1);        /* Invalid IP mask */
      cliSyntaxBottom(ewsContext);
      return NULL;
    }
    *ipMask = (*ipMask)^(~(L7_uint32 )0);
  }
  else
  {
    *ipMask = L7_IP_SUBMASK_EXACT_MATCH_MASK;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To get the MAC and Mask value
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
* @param L7_uint32* relIndex
* @param L7_uint32* macAddr
* @param L7_uint32* ipMask
*
* @returntype L7_RC_t
*
* @returns NULL - For error cases
* @returns cliPrompt(ewsContext)- For success
* @end
*
*********************************************************************/
L7_char8 *  getMacAndMask(EwsContext ewsContext,
                          const L7_char8 * * argv,
                          L7_uint32 index,L7_uint32 * relIndex,
                          L7_uchar8 * macAddr,
                          L7_uchar8 * macMask,
                          L7_BOOL maskSupported)
{
  L7_uint32 unit;
  L7_uint32 totalArg;
  L7_char8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMacMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 x;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  totalArg = cliNumFunctionArgsGet();

  if( totalArg < *relIndex)
  {
    macAclSyntaxWrite(ewsContext);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  /* Get the rcIp address*/
  if (strlen(argv[index+*relIndex]) >= sizeof(strMacAddr) )
  {
    sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_common_NwMacInvalidParm);
    ewsTelnetWrite( ewsContext, buf);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  osapiStrncpySafe( strMacAddr, argv[index+*relIndex], sizeof(strMacAddr));
  memset (macAddr, 0,sizeof(macAddr));
  *relIndex  = *relIndex+1;

  if (cliConvertMac(strMacAddr, macAddr) != L7_TRUE)
  {
    sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf,pStrErr_qos_IncorrectMacAddrType);
    ewsTelnetWrite( ewsContext, buf);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  /* Get the mac Mask*/
  if( totalArg < *relIndex && maskSupported == L7_TRUE)
  {
    macAclSyntaxWrite(ewsContext);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  if( maskSupported == L7_TRUE)
  {
    if (strlen(argv[index+*relIndex]) >= sizeof(strMacMask) )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_IncorrectMacMaskType);           /* Invalid MAC mask */
      cliSyntaxBottom(ewsContext);
      return NULL;
    }

    osapiStrncpySafe( strMacMask, argv[index+*relIndex], sizeof(strMacMask));
    *relIndex  = *relIndex+1;

    /* verify if the specified mac mask is valid */
    if (cliConvertMac(strMacMask, macMask) != L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacMaskType);
      cliSyntaxBottom(ewsContext);
      return NULL;
    }

    for (x = 0; x < L7_MAC_ADDR_LEN; x++)
    {
      macMask[x] = ~(macMask[x]);
    }
  }
  else
  {
    memset(macMask, 0xff, (size_t)L7_MAC_ADDR_LEN);
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Creates/deletes ACL list.
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  access-list {(<1-99>  {deny|permit} <srcip> <srcmask>) |
*             ({<100-199> {deny|permit} {every | {{icmp | igmp| | ip | tcp
*             |udp | <number>} <srcip> <srcmask> [ {eq <portvalue> | range
*             <startport>  <endport> }]  <dstip> <dstmask> [ {eq <portvalue>
*             | range <startport> <endport> } ]  [precedence <precedence>]
*             [tos <tos> <tosmask>] [dscp <dscp>]} })} [log]
*             [assign-queue <queue-id>] [{mirror | redirect} <unit/slot/port>]
*
* @cmdhelp
*
* @cmddescript  Creates and removes acl list. Command also adds
*               acl rule to acl list.
*
* @end
*
*********************************************************************/
const L7_char8 *commandAccessList(EwsContext ewsContext, L7_uint32 argc,
                                  const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 aclId = 0;
  L7_char8 accessListName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 aclRuleNum;
  L7_uint32 aclruleNext;
  L7_uint32 relIndex;
  L7_uint32 unit, slot, port;
  L7_char8 strAction[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 aclType;
  L7_uint32 actionType;
  L7_BOOL maskSupported = L7_FALSE;
  L7_uint32 protVal = 0;
  L7_uint32 srcIpAddr = 0;
  L7_uint32 srcIpMask = 0;
  L7_uint32 dstIp = 0;
  L7_uint32 totalArg = 0;
  L7_uint32 dstIpMask = 0;
  L7_int32 srcPortValue = -1;
  L7_int32 srcStartPort = -1;
  L7_int32 srcEndPort = -1;
  L7_int32 dstPortValue = -1;
  L7_int32 dstStartPort = -1;
  L7_int32 dstEndPort = -1;
  L7_int32 precVal = -1;
  L7_int32 assignQueueVal = -1;
  L7_int32 mirrorVal = -1;
  L7_int32 redirectVal = -1;
  L7_uint32 argTosBits = 2;
  L7_uchar8 strTosBits[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 tosVal =-1;
  L7_uint32 utosVal =-1;
  L7_int32 dscpVal = -1;
  L7_uint32 udscpVal =0;
  L7_uint32 tosMask = 0;
  L7_uchar8 strConvertedTosBits[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strTosMask[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strConvertedTosMask[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL matchEvery= L7_FALSE;
  L7_BOOL matchSrc = L7_FALSE;
  L7_BOOL matchDst = L7_FALSE;
  L7_BOOL isAclAdded = L7_TRUE;
  L7_uint32 maxAcls;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strDscp[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL matchOther = L7_FALSE;
  L7_BOOL logSpecified = L7_FALSE;
  L7_BOOL mirrorOrRedirectSpecified = L7_FALSE;
  L7_BOOL configuringNamedIpAcl = L7_FALSE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
  {
    maskSupported = L7_TRUE;
  }

  totalArg = cliNumFunctionArgsGet();
  if (totalArg < 1)      /* parameter check */
  {
    aclSyntaxWrite(ewsContext);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* determine if we are in named IPv4 access-list config mode */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_IPV4_ACCESS_LIST_CONFIG_MODE))
  {
    /* we are in ipv4 named access list mode, retrieve ACL name for list being configured */
    osapiStrncpySafe(accessListName, EWSACCESSLISTNAME(ewsContext),sizeof(accessListName));

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosAclNameToIndex(unit, accessListName, &aclId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
      }
      aclType = checkAcltype(aclId);
      if (aclType != ACL_EXTENDED)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, "Specified access list is not a named IPv4 ACL.");
      }
    }
    else
    {
      /* if we are working on a named IP ACL for validation, assume the ACL type is extended */
      aclType = ACL_EXTENDED;
    }
    relIndex = 0;
    configuringNamedIpAcl = L7_TRUE;
  }
  else
  {
    /* we are configuring a numbered ACL */
    relIndex = 1;
    if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &aclId) != L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
      ewsTelnetWrite( ewsContext, argv[index+relIndex]);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }
    if (ewsContext->commType != CLI_NO_CMD)
    {
      if (totalArg < 3)
      {
        aclSyntaxWrite(ewsContext);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }
    aclType = checkAcltype(aclId);
    if (aclType == ACL_NOTDEFIND)
    {
      return cliSyntaxReturnPrompt (ewsContext,pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
    }
    relIndex++;
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbQosAclDelete(unit, aclId);

      if (rc == L7_FAILURE)
      {
        return cliSyntaxReturnPrompt (ewsContext,pStrErr_qos_AccessListDelFail);
      }

      if (rc == L7_ERROR)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AclIdNotExist);
      }
    }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  osapiStrncpySafe(strAction,argv[index+ relIndex], sizeof(strAction));
  relIndex++;
  cliConvertToLowerCase(strAction);

  if (strcmp(strAction, pStrInfo_common_Permit_1) == 0)
  {
    actionType = L7_ACL_PERMIT;
  }
  else if (strcmp(strAction, pStrInfo_common_Deny_2) == 0)
  {
    actionType = L7_ACL_DENY;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_Action_1);
  }
  if (strcmp(argv[index+ relIndex], pStrInfo_qos_AclEveryStr) == 0)
  {
    matchEvery = L7_TRUE;
    relIndex++;
  }

  if (matchEvery == L7_FALSE)
  {
    /* Check the protocol type*/
    if (aclType != ACL_STANDARD)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID) == L7_TRUE)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &protVal) == L7_SUCCESS)
        {
          if ((protVal < L7_ACL_MIN_PROTOCOL_NUM) || (protVal > L7_ACL_MAX_PROTOCOL_NUM))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectValProtoNum);
          }
        }
        /* icmp*/
        else if (strcmp(argv[index+relIndex], pStrInfo_common_Icmp_1) == 0)
        {
          protVal = L7_ACL_PROTOCOL_ICMP;
        }

        else if (strcmp(argv[index+relIndex], pStrInfo_common_Igmp_2) == 0)            /* igmp */
        {
          protVal = L7_ACL_PROTOCOL_IGMP;
        }
        /* ip */
        else if (strcmp(argv[index+relIndex], pStrInfo_common_IpOption) == 0)
        {
          protVal = L7_ACL_PROTOCOL_IP;
        }
        /* tcp */
        else if (strcmp(argv[index+relIndex], pStrInfo_qos_Tcp_1) == 0)
        {
          protVal = L7_ACL_PROTOCOL_TCP;
        }
        /* udp */
        else if (strcmp(argv[index+relIndex], pStrInfo_qos_Udp_1) == 0)
        {
          protVal = L7_ACL_PROTOCOL_UDP;
        }
        else
        {
          /* Invalid keyword <Currently supported values are \nigmp\ ip\ tcp\ udp\ icmp.> */
          return cliSyntaxReturnPrompt (ewsContext, pStrErr_qos_KeywordIgmpIpTcpUdpIcmp);
        }
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (protVal != L7_ACL_PROTOCOL_IP)
        {
          matchOther = L7_TRUE;
        }
        relIndex++;
      }
    }

    if (aclType != ACL_STANDARD && strcmp(argv[index+ relIndex], pStrInfo_qos_AclAnyStr) == 0)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
      }
      matchSrc = L7_TRUE;
      relIndex++;
    }

    if (matchSrc == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                         L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      if ((aclType != ACL_STANDARD) &&
          (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE))
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_FALSE)
        {
          if (totalArg == relIndex)
          {
            aclSyntaxWrite(ewsContext);
            cliSyntaxBottom(ewsContext);
            return NULL;
          }
        }
        else
        {
          if (totalArg == relIndex+1)
          {
            aclSyntaxWrite(ewsContext);
            cliSyntaxBottom(ewsContext);
            return NULL;
          }
        }
      }
      if (getIPAddressAndMask(ewsContext,argv,index,&relIndex, &srcIpAddr, &srcIpMask) == NULL)
      {
        return cliPrompt(ewsContext);
      }
    }
  }

  if (aclType == ACL_STANDARD)
  {
    while (totalArg >= relIndex)
    {
      /* Log */
      if (strcmp(pStrInfo_qos_AclLogsStr, argv[index+relIndex]) == 0)
      {
        /* check specific log feature per the rule action type */
        if (actionType == L7_ACL_DENY)
        {
          if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       L7_ACL_LOG_DENY_FEATURE_ID) != L7_TRUE )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclLogDenyNotSupported);
          }
        }
        else
        {
          if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       L7_ACL_LOG_PERMIT_FEATURE_ID) != L7_TRUE )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclLogPermitNotSupported);
          }
        }
        logSpecified = L7_TRUE;
      }

      /* Assign Queue */
      else if (strcmp(pStrInfo_qos_AclAsSignQueueStr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;
        if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &assignQueueVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidAsSignQueue);
        }
        /* verify if the specified value is in between min and max */
        if ((assignQueueVal < L7_QOS_COS_QUEUE_ID_MIN) || (assignQueueVal > L7_QOS_COS_QUEUE_ID_MAX))
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_IncorrectQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
        }
      }

      /* mirror */
      else if (strcmp(pStrInfo_qos_AclMirrorStr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        /* mirror and redirect rule attributes are mutually-exclusive */
        if (mirrorOrRedirectSpecified != L7_FALSE)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_CfgureAclDenyLog);
        }
        mirrorOrRedirectSpecified = L7_TRUE;
        relIndex++;
        if (cliIsStackingSupported() == L7_TRUE)
        {
          if ((cliValidSpecificUSPCheck(argv[index+relIndex], &unit, &slot, &port) != L7_SUCCESS) ||
              (usmDbIntIfNumFromUSPGet(unit, slot, port, &mirrorVal) != L7_SUCCESS))
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
          }
        }
        else
        {
          unit = cliGetUnitId();
          if (cliSlotPortToIntNum(ewsContext, argv[index+relIndex], &slot, &port, &mirrorVal) != L7_SUCCESS)
          {
            return cliPrompt(ewsContext);
          }
        }
      }

      /* redirect */
      else if (strcmp(pStrInfo_qos_AclRedirectsStr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        /* mirror and redirect rule attributes are mutually-exclusive */
        if (mirrorOrRedirectSpecified != L7_FALSE)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_CfgureAclDenyLog);
        }
        mirrorOrRedirectSpecified = L7_TRUE;
        relIndex++;
        if (cliIsStackingSupported() == L7_TRUE)
        {
          if ((cliValidSpecificUSPCheck(argv[index+relIndex], &unit, &slot, &port) != L7_SUCCESS) ||
              (usmDbIntIfNumFromUSPGet(unit, slot, port, &redirectVal) != L7_SUCCESS))
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
          }
        }
        else
        {
          unit = cliGetUnitId();
          if (cliSlotPortToIntNum(ewsContext, argv[index+relIndex], &slot, &port, &redirectVal) != L7_SUCCESS)
          {
            return cliPrompt(ewsContext);
          }
        }
      }

      relIndex++;
    }
  }
  else   /* ACL_EXTENDED */
  {
    if (totalArg >= relIndex)
    {

      if (getPortRange(ewsContext,argv,index,&relIndex, &srcPortValue, &srcStartPort, &srcEndPort, &matchOther) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

      if (totalArg < relIndex
          &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE))
      {
        aclSyntaxWrite(ewsContext);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      if (matchEvery == L7_FALSE && strcmp(argv[index+ relIndex], pStrInfo_qos_AclAnyStr) == 0)
      {
        matchDst = L7_TRUE;
        relIndex++;
      }

      if (matchEvery == L7_FALSE && matchDst == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                                                     L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
      {
        if (getIPAddressAndMask(ewsContext,argv,index,&relIndex, &dstIp, &dstIpMask) == NULL)
        {
          return cliPrompt(ewsContext);
        }
      }

      if (totalArg >= relIndex)
      {
        if (getPortRange(ewsContext,argv,index,&relIndex, &dstPortValue,
                         &dstStartPort, &dstEndPort, &matchOther) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
    }
    while (totalArg >= relIndex)
    {
      if (strcmp(pStrInfo_qos_AclPrecedence, argv[index+relIndex]) == 0)
      {
        L7_uint32 precedence;
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;
        if ((cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &precedence)
             != L7_SUCCESS) ||
            (precedence < L7_ACL_MIN_PRECEDENCE) || (precedence > L7_ACL_MAX_PRECEDENCE))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_IncorrectValPrecedence);
        }
        else
        {
          precVal = precedence;
          matchOther = L7_TRUE;
        }
      }

      else if (strcmp(pStrInfo_qos_AclTosStr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;
        if (strlen(argv[index+argTosBits]) >= sizeof(strTosBits))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrInfo_qos_IncorrectInputTosBits);
        }

        osapiStrncpySafe(strTosBits, argv[index + relIndex], sizeof(strTosBits));

        /* verify if the specified tos bits are valid */
        if (usmDbConvertTwoDigitHex(strTosBits, strConvertedTosBits) != L7_TRUE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrInfo_qos_IncorrectInputTosBits);
        }
        utosVal = (L7_uint32)*strConvertedTosBits;
        tosVal = utosVal;
        matchOther = L7_TRUE;
        if (maskSupported == TRUE && totalArg <=relIndex)
        {
          aclSyntaxWrite(ewsContext);
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
        else if (maskSupported == TRUE)
        {
          if (totalArg == relIndex)
          {
            aclSyntaxWrite(ewsContext);
            cliSyntaxBottom(ewsContext);
            return NULL;
          }
          relIndex++;
          if (strlen(argv[index+relIndex]) >= sizeof(strTosMask))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrInfo_qos_IncorrectInputTosMask);
          }

          osapiStrncpySafe(strTosMask, argv[index + relIndex], sizeof(strTosMask));

          /* verify if the specified tos mask is valid */
          if (usmDbConvertTwoDigitHex(strTosMask, strConvertedTosMask) != L7_TRUE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrInfo_qos_IncorrectInputTosMask);
          }
          else
          {
            tosMask = (L7_uint32)*strConvertedTosMask;
            matchOther = L7_TRUE;
          }
        }
      }
      else if (strcmp(pStrInfo_qos_AclDscpStr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;
        osapiStrncpySafe(strDscp, argv[index+relIndex], sizeof(strDscp));

        /* handle input dscp as keyword or value */
        if (cliAclConvertDSCPStringToVal(strDscp, &udscpVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDscpVal);
        }

        /* verify if the specified value is in between 0 to 63 */
        if ((udscpVal < L7_ACL_MIN_DSCP) ||
            (udscpVal > L7_ACL_MAX_DSCP))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectDscpVal_1);
        }
        else
        {
          dscpVal = udscpVal;
          matchOther = L7_TRUE;
        }
      }

      /* Log */
      else if (strcmp(pStrInfo_qos_AclLogsStr, argv[index+relIndex]) == 0)
      {
        /* check specific log feature per the rule action type */
        if (actionType == L7_ACL_DENY)
        {
          if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       L7_ACL_LOG_DENY_FEATURE_ID) != L7_TRUE )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclLogDenyNotSupported);
          }
        }
        else
        {
          if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       L7_ACL_LOG_PERMIT_FEATURE_ID) != L7_TRUE )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclLogPermitNotSupported);
          }
        }
        logSpecified = L7_TRUE;
      }

      /* Assign Queue */
      else if (strcmp(pStrInfo_qos_AclAsSignQueueStr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;
        if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &assignQueueVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidAsSignQueue);
        }
        /* verify if the specified value is in between 1 to 4094 */
        if ((assignQueueVal < L7_QOS_COS_QUEUE_ID_MIN) || (assignQueueVal > L7_QOS_COS_QUEUE_ID_MAX))
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_IncorrectQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
        }
      }

      /* mirror */
      else if (strcmp(pStrInfo_qos_AclMirrorStr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        /* mirror and redirect rule attributes are mutually-exclusive */
        if (mirrorOrRedirectSpecified != L7_FALSE)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_CfgureAclDenyLog);
        }
        mirrorOrRedirectSpecified = L7_TRUE;
        relIndex++;
        if (cliIsStackingSupported() == L7_TRUE)
        {
          if ((cliValidSpecificUSPCheck(argv[index+relIndex], &unit, &slot, &port) != L7_SUCCESS) ||
              (usmDbIntIfNumFromUSPGet(unit, slot, port, &mirrorVal) != L7_SUCCESS))
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
          }
        }
        else
        {
          unit = cliGetUnitId();
          if (cliSlotPortToIntNum(ewsContext, argv[index+relIndex], &slot, &port, &mirrorVal) != L7_SUCCESS)
          {
            return cliPrompt(ewsContext);
          }
        }
      }

      /* redirect */
      else if (strcmp(pStrInfo_qos_AclRedirectsStr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          aclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        /* mirror and redirect rule attributes are mutually-exclusive */
        if (mirrorOrRedirectSpecified != L7_FALSE)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_CfgureAclDenyLog);
        }
        mirrorOrRedirectSpecified = L7_TRUE;
        relIndex++;
        if (cliIsStackingSupported() == L7_TRUE)
        {
          if ((cliValidSpecificUSPCheck(argv[index+relIndex], &unit, &slot, &port) != L7_SUCCESS) ||
              (usmDbIntIfNumFromUSPGet(unit, slot, port, &redirectVal) != L7_SUCCESS))
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
          }
        }
        else
        {
          unit = cliGetUnitId();
          if (cliSlotPortToIntNum(ewsContext, argv[index+relIndex], &slot, &port, &redirectVal) != L7_SUCCESS)
          {
            return cliPrompt(ewsContext);
          }
        }
      }
      relIndex++;
    }
  }
  /**Check if the script execute Flag is set for execute then execute the usmdb***/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (configuringNamedIpAcl == L7_FALSE)
    {
      rc = usmDbQosAclCreate(unit, aclId);

      if (rc == L7_FAILURE)
      {
        return cliSyntaxReturnPrompt (ewsContext,pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
      }
      else if (rc == L7_TABLE_IS_FULL)
      {
        usmDbQosAclMaxNumGet(unit, &maxAcls);        /* This function always returns L7_SUCCESS */
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_qos_MaxOfAclsIsAlreadyCfgured, maxAcls);
      }
      else if (rc == L7_ERROR)
      {
        isAclAdded = L7_FALSE;
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_AclAlreadyExists);         /* ACL already exists */
      }
    }

    /* Get the new rule number*/
    if (usmDbQosAclRuleGetFirst(unit, aclId, &aclRuleNum)!= L7_SUCCESS)
    {
      aclRuleNum = L7_ACL_MIN_RULE_NUM;
    }
    else
    {
      while (usmDbQosAclRuleGetNext(unit, aclId, aclRuleNum, &aclruleNext)== L7_SUCCESS)
      {
        aclRuleNum = aclruleNext;
      }
      aclRuleNum = aclRuleNum+1;
    }
    if (aclRuleNum >L7_ACL_MAX_RULE_NUM)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_RuleError);          /* Invalid rule number <select rule number between 1 to 10> */

      /* it's a warning */
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

  /* Add the rule to acl*/
    rc = usmDbQosAclRuleActionAdd(unit, aclId, aclRuleNum,actionType );
    if (rc == L7_FAILURE)
    {
      return cliSyntaxReturnPrompt (ewsContext,pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
    }
    else if (rc == L7_ERROR)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AclIdNotExist);
    }
    else if (rc == L7_TABLE_IS_FULL)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, "Maximum number of ACL rules already configured.");
    }
    else if (rc != L7_SUCCESS)
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, buf, pStrErr_qos_ProcessingAcl);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    if ((matchEvery == L7_TRUE) ||
        (aclType == ACL_STANDARD && matchSrc == L7_TRUE) ||
        (aclType != ACL_STANDARD && matchSrc == L7_TRUE && matchDst == L7_TRUE && matchOther == L7_FALSE))
    {
      rc = usmDbQosAclRuleEveryAdd(unit, aclId, aclRuleNum, L7_TRUE);
      if (rc == L7_FAILURE)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPrompt (ewsContext,pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
      }

      if (rc == L7_ERROR)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AclIdNotExist);
      }

      if (rc == L7_REQUEST_DENIED)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }
    else
    {
      if (matchSrc == L7_FALSE)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
        {
          rc = usmDbQosAclRuleSrcIpMaskAdd(unit, aclId, aclRuleNum, srcIpAddr, srcIpMask);

          if (rc == L7_FAILURE)
          {
            aclClean(isAclAdded, aclId, aclRuleNum);
            return cliSyntaxReturnPrompt (ewsContext,pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
          }
          else if (rc == L7_REQUEST_DENIED)
          {
            aclClean(isAclAdded, aclId, aclRuleNum);
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
          }
        }
      }

      /* remaining match fields only meaningful for IP Extended ACLs */
      if (aclType != ACL_STANDARD)
      {
        /* Add the protocol value*/
        if (protVal != 0)
        {
          rc = usmDbQosAclRuleProtocolAdd(unit, aclId, aclRuleNum, protVal);
          if (rc != L7_SUCCESS)
          {
            aclClean(isAclAdded, aclId, aclRuleNum);
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
          }
        }
        if (srcPortValue > -1)
        {
          rc = usmDbQosAclRuleSrcL4PortAdd(unit, aclId, aclRuleNum, srcPortValue);
        }
        else if (srcStartPort > -1)
        {
          rc = usmDbQosAclRuleSrcL4PortRangeAdd(unit, aclId, aclRuleNum,srcStartPort, srcEndPort);
        }
        if (rc != L7_SUCCESS)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }

        if (matchDst == L7_FALSE)
        {
          if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
          {
            rc = usmDbQosAclRuleDstIpMaskAdd(unit, aclId, aclRuleNum,dstIp,dstIpMask);

            if (rc == L7_FAILURE)
            {
              aclClean(isAclAdded, aclId, aclRuleNum);
              return cliSyntaxReturnPrompt (ewsContext,pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
            }
            else if (rc == L7_REQUEST_DENIED)
            {
              aclClean(isAclAdded, aclId, aclRuleNum);
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
            }
          }
        }

        if (dstPortValue > -1)
        {
          rc = usmDbQosAclRuleDstL4PortAdd(unit, aclId, aclRuleNum, dstPortValue);
        }
        else if (dstStartPort > -1)
        {
          rc = usmDbQosAclRuleDstL4PortRangeAdd(unit, aclId, aclRuleNum,dstStartPort, dstEndPort);
        }
        if (rc != L7_SUCCESS)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }

        if (precVal > -1)
        {
          rc = usmDbQosAclRuleIPPrecedenceAdd(unit, aclId, aclRuleNum, precVal);
          if (rc != L7_SUCCESS)
          {
            aclClean(isAclAdded, aclId, aclRuleNum);
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
          }
        }

        if (tosVal > -1)
        {
          rc = usmDbQosAclRuleIPTosAdd(unit, aclId, aclRuleNum, tosVal, ~tosMask & 0x000000FF);
          if (rc != L7_SUCCESS)
          {
            aclClean(isAclAdded, aclId, aclRuleNum);
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
          }
        }

        if (dscpVal > -1)
        {
          rc = usmDbQosAclRuleIPDscpAdd(unit, aclId, aclRuleNum, dscpVal);
          if (rc != L7_SUCCESS)
          {
            aclClean(isAclAdded, aclId, aclRuleNum);
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
          }
        }
      }
    }

    /* the log, assign-queue, mirror and redirect attributes are used
     * for both standard and extended ACLs
     */

    /* Log */
    if (logSpecified == L7_TRUE)
    {
      rc = usmDbQosAclRuleLoggingAdd(unit, aclId, aclRuleNum, logSpecified);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    /* Assign-Queue */
    if (assignQueueVal > -1)
    {
      rc = usmDbQosAclRuleAssignQueueIdAdd(unit, aclId, aclRuleNum, assignQueueVal);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    /* Mirror */
    if (mirrorVal > -1)
    {
      rc = usmDbQosAclRuleMirrorIntfAdd(unit, aclId, aclRuleNum, mirrorVal);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    /* Redirect */
    if (redirectVal > -1)
    {
      rc = usmDbQosAclRuleRedirectIntfAdd(unit, aclId, aclRuleNum, redirectVal);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the IPv4 ACL mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliIpAccessListMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliIpAccessListPrompt(index+1, "(Config-ipv4-acl)#", pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the IPv4 ACL mode
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  #> [no] ip access-list <name>
*
* @cmdhelp
*
* @cmddescript  Enters the ip-access-list config mode.
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliIpAccessListPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt;
  L7_uint32 unit, index, maxAcls, rc;
  L7_char8 strName[L7_ACL_NAME_LEN_MAX+1];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType != CLI_NO_CMD)
  {
    prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext, cliGetMode(L7_GLOBAL_CONFIG_MODE));
    if (prompt != NULL)
    {
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return prompt;
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ((argc == depth + 1) &&
      (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_IPV4_ACCESS_LIST_CONFIG_MODE)))
  {

    if (strlen(argv[depth]) > L7_ACL_NAME_LEN_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    osapiStrncpySafe(strName, argv[depth], sizeof(strName));

    if (usmDbQosAclNameStringCheck(unit, strName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    /* Check to see if named IPv4 ACL exists */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Does not exist - create and add */
        if (usmDbQosAclNameToIndex(unit, strName, &index) != L7_SUCCESS)
        {
          rc = usmDbQosAclNamedIndexNextFree(unit, L7_ACL_TYPE_IP, &index);
          if (rc != L7_SUCCESS)
          {

            if (rc == L7_TABLE_IS_FULL)
            {
              (void)usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_MaxOfAclsIsAlreadyCfgured, maxAcls);
              return cliSyntaxReturnPrompt (ewsContext, buf);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToCreateAccessList);
            }
          }
          if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            rc = usmDbQosAclCreate(unit, index);
          }

          if (rc != L7_SUCCESS)
          {
            if (rc == L7_TABLE_IS_FULL)
            {
              usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrInfo_qos_MaxOfAclsIsAlreadyCfgured, maxAcls);
              return cliSyntaxReturnPrompt (ewsContext, buf);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToCreateAccessList);
            }
          }

          if (usmDbQosAclNameAdd(unit, index, strName) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToCreateAccessList);
          }
        }
        else
        {
          /* found an existing IP ACL under this name, check that it is and IPv4 ACL */
          if (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, index) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, "Specified access list is not a named IPv4 ACL.");
          }
        }
      }
      strcpy(EWSACCESSLISTNAME(ewsContext),strName);

      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_IPV4_ACCESS_LIST_CONFIG_MODE));
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ((usmDbQosAclNameToIndex(unit, strName, &index) == L7_SUCCESS) &&
            (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, index) == L7_SUCCESS))
        {
          if (usmDbQosAclDelete(unit, index) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDelFail);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
        }
      }
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    cliSyntaxBottom(ewsContext);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return NULL;
}


/*********************************************************************
*
* @purpose    Enable/Disable ACL trap mode.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] acl-trapflags
*
* @cmdhelp
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAclTrapflags(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs, flagVal;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    flagVal = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    flagVal = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_CmdMode);
  }

  if (numArgs != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclTrapFlags);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgAclTrapFlagsNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbQosAclTrapFlagSet(unit, flagVal) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_qos_SetTrapFlag);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Attach the access-list to an interface
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax ip access-group {<1-199> | <name>} {in | out} [<sequence>]
*
* @cmdhelp
*
* @cmddescript Attaches an ACL with interface
*
* @end
*
*********************************************************************/
const L7_char8 *commandIPAccessGroup(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 argAclIdOrName = 1;
  L7_uint32 argDir = 2;
  L7_uint32 argAclSequence = 3;
  L7_RC_t rc;
  L7_uint32 aclId,interface;
  L7_char8 strDir[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32 direction, feature, numArgs;
  L7_uint32 unit;
  L7_uint32 sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  L7_BOOL   isNamedAcl = L7_FALSE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArgs = cliNumFunctionArgsGet();

  usmDbIntIfNumFromUSPGet(unit, EWSSLOT(ewsContext), EWSPORT(ewsContext), &interface);

  if ((numArgs < 2) || (numArgs > 3))      /* parameter check */
  {
    if (ewsContext->commType != CLI_NO_CMD)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_FALSE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclIntfAddIn);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgAclIntfAdd);
      }
    }
    else
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_FALSE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoAclIntfAddIn);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgNoAclIntfAdd);
      }
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  if (cliConvertTo32BitUnsignedInteger(argv[index+argAclIdOrName], &aclId) != L7_SUCCESS)
  {
    /* if this parameter is not an integer, treat it as an ACL name */
    osapiStrncpySafe( aclName, argv[index+argAclIdOrName], sizeof(aclName));

    if (usmDbQosAclNameStringCheck(unit, aclName) == L7_SUCCESS)
    {
      /**Check if the script execute Flag is set for execute then execute the usmdb**/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ((usmDbQosAclNameToIndex(unit, aclName, &aclId) != L7_SUCCESS) ||
            (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) != L7_SUCCESS))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, "Invalid ACL name given.");
    }
    isNamedAcl = L7_TRUE;
  }

  if ((isNamedAcl == L7_FALSE) &&
      ((aclId < L7_MIN_ACL_ID ) || (aclId > L7_MAX_ACL_ID)))
  {
    return cliSyntaxReturnPrompt (ewsContext,pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbQosAclNumCheckValid(unit, aclId);
    if (rc == L7_FAILURE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclIdDoesntExist);
    }
  }

  if (strlen(argv[index+argDir]) >= sizeof(strDir))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  osapiStrncpySafe(strDir,argv[index+argDir], sizeof(strDir));

  if (strcmp(strDir, pStrInfo_common_AclInStr) == 0)        /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (strcmp(strDir, pStrInfo_common_AclOutStr) == 0)        /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  if (numArgs == 3 &&
      (cliConvertTo32BitUnsignedInteger(argv[index + argAclSequence], &sequence) != L7_SUCCESS || sequence <= 0))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_qos_AccessListGrpsSeq);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  if (usmDbQosAclIsDiffServIntfInUse( unit, interface, direction ) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIFFSERV_COEXIST_FEATURE_ID ) == L7_FALSE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_AclAndDiffservOnSameIntf);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      rc = usmDbQosAclInterfaceDirectionAdd(unit, interface, direction, aclId, sequence);
    }
    else
    {
      rc = usmDbQosAclInterfaceDirectionRemove(unit, interface, direction, aclId);
    }

    if (rc == L7_FAILURE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AclIdOrIntf);
    }
    else if (rc == L7_ERROR)
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_qos_AclSeqNumAtLimit);           /* ACL sequence number at limit for intf,dir  */
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_AclNotExistOrIntfNotCnfgdWithAcl);            /* ACL does not exist */
      }
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else if (rc == L7_ALREADY_CONFIGURED)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_IntfAndDirectionAreAlreadyInUse);
    }
    else if (rc == L7_REQUEST_DENIED)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
    }
    else if (rc == L7_TABLE_IS_FULL)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AclTblFull);
    }

    else if (rc == L7_NOT_SUPPORTED)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IpAclNotSupported);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AclNotAdded);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}
/*********************************************************************
*
* @purpose Attach the access-list to all interface
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax ip access-group {<1-199> | <name>} [vlan <vlan_id>] {in|out}
*
* @cmdhelp
*
* @cmddescript Attaches an acl to all interfaces or to a VLAN if
*              keyword vlan and a vlan_id are provided.
*
* @end
*
*********************************************************************/
const L7_char8 *commandIPAccessGroupAll(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAclIdOrName = 1;
  L7_uint32 argDir;
  L7_uint32 argAclSequence;
  L7_uint32 argAclVlanKeyword = 2;
  L7_uint32 argAclVlanId = 3;
  L7_BOOL aclVlanIdGiven = L7_FALSE;
  L7_RC_t rc;
  L7_uint32 aclId, interface, intListSize, numArgs, numArgsCheckLow, numArgsCheckHigh;
  L7_uint32 vlanId;
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_char8 strDir[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 direction, feature;
  L7_uint32 unit, unitNum, slot, port;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF;
  L7_uint32 exclIntfTypes = 0;
  L7_BOOL noAclId = L7_FALSE;
  L7_BOOL commaFlag = L7_FALSE;
  L7_BOOL crFlag = L7_FALSE;
  L7_char8 * interfaceList = L7_NULLPTR;
  L7_uint32 sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  static L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  static L7_uint32 count = 0;
  L7_BOOL   isNamedAcl = L7_FALSE;


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  unit = cliGetUnitId();

  numArgs = cliNumFunctionArgsGet();

  /* determine if ACL is being applied to a VLAN ID */
  if ((numArgs > 3) && (strcmp(argv[index+argAclVlanKeyword], pStrInfo_common_MacAclVlan_1) == 0))
  {
    aclVlanIdGiven = L7_TRUE;
    argDir = 4;
    argAclSequence = 5;
    numArgsCheckLow = 4;
    numArgsCheckHigh = 5;

    /* get the vlan id specified by user */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argAclVlanId], &vlanId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlanId_1);
    }
    /* verify if the specified value is in valid range */
    if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_FALSE)
    {
      sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_ValVlanIdMustBeFromTo, L7_ACL_MIN_VLAN_ID, L7_ACL_MAX_VLAN_ID);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }
  }
  else
  {
    argDir = 2;
    argAclSequence = 3;
    numArgsCheckLow = 2;
    numArgsCheckHigh = 3;
  }

  if (numArgs < numArgsCheckLow || numArgs > numArgsCheckHigh)     /* parameter check */
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_FALSE)
    {
      /* applying ACLs to VLANs _not_ supported */
      if (ewsContext->commType != CLI_NO_CMD)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_FALSE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclIntfAddIn);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgAclAllIntfAdd);
        }
      }
      else
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_FALSE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoAclIntfAddIn);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgNoAclIntfAdd);
        }
      }
    }
    else
    {
      /* applying ACLs to VLANs supported */
      if (ewsContext->commType != CLI_NO_CMD)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID) == L7_FALSE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclAllIntfVlanAddIn);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgAclAllIntfVlanAdd);
        }
      }
      else
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID) == L7_FALSE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoAclAllIntfVlanAddIn);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgNoAclAllIntfVlanAdd);
        }
      }
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index+argAclIdOrName], &aclId) != L7_SUCCESS)
  {
    /* if this parameter is not an integer, treat it as an ACL name */
    osapiStrncpySafe( aclName, argv[index+argAclIdOrName], sizeof(aclName));

    if (usmDbQosAclNameStringCheck(unit, aclName) == L7_SUCCESS)
    {
      /**Check if the script execute Flag is set for execute then execute the usmdb**/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ((usmDbQosAclNameToIndex(unit, aclName, &aclId) != L7_SUCCESS) ||
            (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) != L7_SUCCESS))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, "Invalid ACL name given.");
    }
    isNamedAcl = L7_TRUE;
  }

  if ((isNamedAcl == L7_FALSE) &&
      ((aclId < L7_MIN_ACL_ID ) || (aclId > L7_MAX_ACL_ID)))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbQosAclNumCheckValid(unit, aclId);
    if (rc == L7_FAILURE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclIdDoesntExist);
    }
  }

  if (strlen(argv[index+argDir]) >= sizeof(strDir))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  osapiStrncpySafe(strDir,argv[index+argDir], sizeof(strDir));

  if (strcmp(strDir, pStrInfo_common_AclInStr) == 0)       /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (strcmp(strDir, pStrInfo_common_AclOutStr) == 0)       /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  if ((numArgs == 3 || numArgs == 5) &&
      (cliConvertTo32BitUnsignedInteger(argv[index + argAclSequence], &sequence) != L7_SUCCESS || sequence <= 0))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_qos_AccessListGrpsSeq);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  if (aclVlanIdGiven == L7_FALSE)
  {
    /* applying ACL to all interfaces */

    /* If LAG is supported, include lags in interface types */
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID) == L7_TRUE)
    {
      inclIntfTypes = USM_PHYSICAL_INTF | USM_LAG_INTF;
    }

    /* interface = get first interface, if fail, exit */
    if (usmDbIntIfNumTypeFirstGet( unit, inclIntfTypes, exclIntfTypes,
                                  &interface ) != L7_SUCCESS)
    {
      sprintfAddBlanks (0, 0, 0, 0, pStrErr_common_Error, buf, pStrErr_common_ServiceBeAddedRetryUsingSlotPort, cliSyntaxInterfaceHelp());
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    if (ewsContext->commType == CLI_NO_CMD)
    {
      rc = usmDbQosAclAssignedIntfDirListGet(unit, aclId, direction, &intfList);
      if (rc ==  L7_ERROR || rc == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListNoIntfsAttached);
      }
      else
      {
        count = intfList.count;
      }
    }

    do
    {
      if (usmDbQosAclIsDiffServIntfInUse( unit, interface, direction ) == L7_TRUE &&
          usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_INTF_DIFFSERV_COEXIST_FEATURE_ID ) == L7_FALSE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_AclAndDiffservOnSameIntf);
      }

      rc = usmDbIntIfNumTypeNextGet( unit, inclIntfTypes, exclIntfTypes, interface, &interface );
      if (rc != L7_SUCCESS)
      {
        break;
      }

    } while (1);

    /* interface = get first interface, if fail, exit */
    if (usmDbIntIfNumTypeFirstGet( unit, inclIntfTypes, exclIntfTypes,
                                  &interface ) != L7_SUCCESS)
    {
      sprintfAddBlanks (0, 0, 0, 0, pStrErr_common_Error, buf, pStrErr_common_ServiceBeAddedRetryUsingSlotPort, cliSyntaxInterfaceHelp());
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    /* potential size of list of interfaces */
    if (cliIsStackingSupported() == L7_TRUE)
    {
      intListSize = L7_MAX_PORT_COUNT * 12;
    }
    else
    {
      intListSize = L7_MAX_PORT_COUNT * 9;
    }

    interfaceList = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, intListSize);
    memset (stat, 0, sizeof(stat));
    do
    {
      /**Check if the script execute Flag is set for execute then execute the usmdb**/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (ewsContext->commType == CLI_NORMAL_CMD)
        {
          rc = usmDbQosAclInterfaceDirectionAdd(unit, interface, direction, aclId, sequence);    /* TEMP */

        }
        else
        {
          if (count != 0)
          {
            rc = usmDbQosAclInterfaceDirectionRemove(unit, interface, direction, aclId);
            if (rc == L7_SUCCESS)
            {
              count--;
              noAclId = L7_FALSE;
            }
          }
        }

        if (rc != L7_SUCCESS)
        {
          /* Able to remove or add an interface, so set flag to FALSE */
          noAclId = L7_TRUE;
          if (usmDbUnitSlotPortGet(interface, &unitNum, &slot, &port) == L7_SUCCESS)
          {
            sprintf(buf, cliDisplayInterfaceHelp(unitNum, slot, port));
            if (commaFlag == L7_TRUE && crFlag == L7_FALSE)
            {
              strcat(stat, ",");
            }

            strcat(stat, buf);
            commaFlag = L7_TRUE;
            if (strlen(stat) > 72)
            {
              sprintf(buf, "%s,\r\n", stat);
              strcat(interfaceList, buf);
              memset (stat, 0, sizeof(stat));
              crFlag = L7_TRUE;
            }
            else
            {
              crFlag = L7_FALSE;
            }
          }
        }
      }

      rc = usmDbIntIfNumTypeNextGet( unit, inclIntfTypes, exclIntfTypes, interface, &interface );
      if (rc != L7_SUCCESS)
      {
        break;
      }

    } while (1);

    sprintf(buf, "%s.", stat);
    strcat(interfaceList, buf);

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (noAclId == L7_TRUE)
      {
        /* If not able to add or remove for even a single interface to the
           access-group,displaying proper error-message */
        if (ewsContext->commType == CLI_NORMAL_CMD)
        {
          if (direction == L7_INBOUND_ACL)
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListDirectionIn);
          }
          else if (direction == L7_OUTBOUND_ACL)
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListDirectionOut);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListDirection);
          }
          ewsTelnetWrite(ewsContext, interfaceList);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclIdDoesntExist);
        }
      }
    }

    if (interfaceList != L7_NULLPTR)
    {
      osapiFree(L7_CLI_WEB_COMPONENT_ID, interfaceList);
    }
  }
  else
  {
    /* applying ACL to a VLAN */

    /**Check if the script execute Flag is set for execute then execute the usmdb**/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        if ((rc = usmDbQosAclVlanDirectionAdd(unit, vlanId, direction, aclId, sequence)) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListVlanAdd);
        }
      }
      else
      {
        if ((rc = usmDbQosAclVlanDirectionRemove(unit, vlanId, direction, aclId)) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListVlanAddNo);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Creates/deletes ACL list.
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  {deny|permit} {every | {{icmp | igmp| | ip | tcp
*             |udp | <number>} {<source-ipv6-prefix/prefix-length> | any} [ {eq <portvalue> | range
*             <startport>  <endport> }] {<destination-ipv6-prefix/prefix-length> | any} [ {eq <portvalue>
*             | range <startport> <endport> } ]  [dscp <dscp>]} }}} [log]
*             [assign-queue <queue-id>] [{mirror | redirect} <unit/slot/port>]
*
* @cmdhelp
*
* @cmddescript  Command adds acl rule to acl list.
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpv6AccessList(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 aclId = 0;
  L7_uint32 aclRuleNum;
  L7_uint32 aclruleNext;
  L7_uint32 relIndex = 0;
  L7_uint32 unit, slot, port;
  L7_char8 strAction[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 accessListName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 actionType;
  L7_BOOL maskSupported = L7_FALSE;
  L7_uint32 protVal = 0;
  L7_in6_prefix_t srcIpv6Addr;
  L7_in6_prefix_t dstIpv6Addr;
  L7_uint32 totalArg = 0;
  L7_int32 srcPortValue = -1;
  L7_int32 srcStartPort = -1;
  L7_int32 srcEndPort = -1;
  L7_int32 dstPortValue = -1;
  L7_int32 dstStartPort = -1;
  L7_int32 dstEndPort = -1;
  L7_int32 assignQueueVal = -1;
  L7_int32 mirrorVal = -1;
  L7_int32 redirectVal = -1;
  L7_int32 dscpVal = -1;
  L7_uint32 udscpVal =0;
  L7_int32 flowLabelVal = -1;
  L7_uint32 uflowLabelVal =0;
  L7_BOOL matchEvery= L7_FALSE;
  L7_BOOL matchSrc = L7_FALSE;
  L7_BOOL matchDst = L7_FALSE;
  L7_BOOL isAclAdded = L7_TRUE;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strDscp[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strFlowLabel[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL matchOther = L7_FALSE;
  L7_BOOL logSpecified = L7_FALSE;
  L7_BOOL mirrorOrRedirectSpecified = L7_FALSE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
  {
    maskSupported = L7_TRUE;
  }

  totalArg = cliNumFunctionArgsGet();
  if (totalArg < 1)     /* parameter check */
  {
    ipv6AclSyntaxWrite(ewsContext);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  osapiStrncpySafe(accessListName, EWSACCESSLISTNAME(ewsContext),sizeof(accessListName));

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ((usmDbQosAclNameToIndex(unit, accessListName, &aclId) != L7_SUCCESS) ||
        (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IPV6, aclId) != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
    }
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbQosAclDelete(unit, aclId);

      if (rc != L7_SUCCESS)
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_qos_AccessListDelFail);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
    }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  osapiStrncpySafe(strAction, argv[index+relIndex], sizeof(strAction));
  relIndex++;
  cliConvertToLowerCase(strAction);

  if (strcmp(strAction, pStrInfo_common_Permit_1) == 0)
  {
    actionType = L7_ACL_PERMIT;
  }
  else if (strcmp(strAction, pStrInfo_common_Deny_2) == 0)
  {
    actionType = L7_ACL_DENY;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_Action_1);
  }
  if (strcmp(argv[index+ relIndex], pStrInfo_qos_AclEveryStr) == 0)
  {
    matchEvery = L7_TRUE;
    relIndex++;
  }

  if (matchEvery == L7_FALSE)
  {
    /* Check the protocol type*/
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID) == L7_TRUE)
    {
      if (totalArg == relIndex)
      {
        ipv6AclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &protVal) == L7_SUCCESS)
      {
        if ((protVal < L7_ACL_MIN_PROTOCOL_NUM) || (protVal > L7_ACL_MAX_PROTOCOL_NUM))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectValProtoNum);
        }
      }
      /* icmp*/
      else if (strcmp(argv[index+relIndex], pStrInfo_common_Icmp_1) == 0)
      {
        protVal = L7_ACL_PROTOCOL_ICMP;
      }
      /* igmp */
      else if (strcmp(argv[index+relIndex], pStrInfo_common_Igmp_2) == 0)
      {
        protVal = L7_ACL_PROTOCOL_IGMP;
      }
      /* ip */
      else if (strcmp(argv[index+relIndex], pStrInfo_common_Diffserv_5) == 0)
      {
        protVal = L7_ACL_PROTOCOL_IP;
      }
      /* tcp */
      else if (strcmp(argv[index+relIndex], pStrInfo_qos_Tcp_1) == 0)
      {
        protVal = L7_ACL_PROTOCOL_TCP;
      }
      /* udp */
      else if (strcmp(argv[index+relIndex], pStrInfo_qos_Udp_1) == 0)
      {
        protVal = L7_ACL_PROTOCOL_UDP;
      }
      else
      {
        /* Invalid keyword <Currently supported values are \nigmp\ ip\ tcp\ udp\ icmp.> */
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_qos_KeywordIgmpIpTcpUdpIcmp);
      }
      if (totalArg == relIndex)
      {
        ipv6AclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      if (protVal != L7_ACL_PROTOCOL_IP)
      {
        matchOther = L7_TRUE;
      }
      relIndex++;
    }

    if (strcmp(argv[index+ relIndex], pStrInfo_qos_AclAnyStr) == 0)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE)
      {
        if (totalArg == relIndex)
        {
          ipv6AclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
      }
      matchSrc = L7_TRUE;
      relIndex++;
    }

    if (matchSrc == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                         L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE)
      {
        if ((rc = cliValidPrefixPrefixLenCheck(argv[index+relIndex],
                                               &srcIpv6Addr.in6Addr, &srcIpv6Addr.in6PrefixLen)) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_AclIpv6InvalidPrefix);
        }
      }
      relIndex++;
    }
  }

  if (totalArg >= relIndex)
  {
    if (getPortRange(ewsContext,argv,index,&relIndex, &srcPortValue, &srcStartPort, &srcEndPort, &matchOther) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    if (totalArg < relIndex
        &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE))
    {
      ipv6AclSyntaxWrite(ewsContext);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    if (matchEvery == L7_FALSE && strcmp(argv[index+ relIndex], pStrInfo_qos_AclAnyStr) == 0)
    {
      matchDst = L7_TRUE;
      relIndex++;
    }

    if (matchEvery == L7_FALSE && matchDst == L7_FALSE &&
        usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE)
    {
      if ((rc = cliValidPrefixPrefixLenCheck(argv[index+relIndex],
                                             &dstIpv6Addr.in6Addr, &dstIpv6Addr.in6PrefixLen)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_AclIpv6InvalidPrefix);
      }
      relIndex++;
    }

    if (totalArg >= relIndex)
    {
      if (getPortRange(ewsContext,argv,index,&relIndex, &dstPortValue,
                       &dstStartPort, &dstEndPort, &matchOther) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
  }

  while (totalArg >= relIndex)
  {
    /* dscp */
    if (strcmp(pStrInfo_qos_AclDscpStr, argv[index+relIndex]) == 0)
    {
      if (totalArg == relIndex)
      {
        ipv6AclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      relIndex++;
      osapiStrncpySafe(strDscp, argv[index+relIndex], sizeof(strDscp));

      /* handle input dscp as keyword or value */
      if (cliAclConvertDSCPStringToVal(strDscp, &udscpVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDscpVal);
      }

      /* verify if the specified value is in between 0 to 63 */
      if ((udscpVal < L7_ACL_MIN_DSCP) ||
          (udscpVal > L7_ACL_MAX_DSCP))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectDscpVal_1);
      }
      else
      {
        dscpVal = udscpVal;
        matchOther = L7_TRUE;
      }
    }

    /* flow-label */
    else if (strcmp(pStrInfo_qos_AclFlowLabelsStr, argv[index+relIndex]) == 0)
    {
      if (totalArg == relIndex)
      {
        ipv6AclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      relIndex++;
      osapiStrncpySafe(strFlowLabel, argv[index+relIndex], sizeof(strFlowLabel));

      /* handle input dscp as keyword or value */
      if (cliConvertTo32BitUnsignedInteger(strFlowLabel, &uflowLabelVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidFlowLabelVal);
      }

      /* verify if the specified value is in between 0 to 1048575 */
      if ((uflowLabelVal < L7_ACL_MIN_FLOWLBL) ||
          (uflowLabelVal > L7_ACL_MAX_FLOWLBL))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectFlowLabelVal);
      }
      else
      {
        flowLabelVal = uflowLabelVal;
        matchOther = L7_TRUE;
      }
    }

    /* Log */
    else if (strcmp(pStrInfo_qos_AclLogsStr, argv[index+relIndex]) == 0)
    {
      /* check specific log feature per the rule action type */
      if (actionType == L7_ACL_DENY)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_LOG_DENY_FEATURE_ID) != L7_TRUE )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclLogDenyNotSupported);
        }
      }
      else
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_LOG_PERMIT_FEATURE_ID) != L7_TRUE )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclLogPermitNotSupported);
        }
      }
      logSpecified = L7_TRUE;
    }

    /* Assign Queue */
    else if (strcmp(pStrInfo_qos_AclAsSignQueueStr, argv[index+relIndex]) == 0)
    {
      if (totalArg == relIndex)
      {
        ipv6AclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      relIndex++;
      if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &assignQueueVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidAsSignQueue);
      }
      /* verify if the specified value is in between 1 to 4094 */
      if ((assignQueueVal < L7_QOS_COS_QUEUE_ID_MIN) || (assignQueueVal > L7_QOS_COS_QUEUE_ID_MAX))
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR,  buf, pStrErr_qos_IncorrectQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
    }

    /* mirror */
    else if (strcmp(pStrInfo_qos_AclMirrorStr, argv[index+relIndex]) == 0)
    {
      if (totalArg == relIndex)
      {
        ipv6AclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      /* mirror and redirect rule attributes are mutually-exclusive */
      if (mirrorOrRedirectSpecified != L7_FALSE)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_CfgureAclDenyLog);
      }
      mirrorOrRedirectSpecified = L7_TRUE;
      relIndex++;
      if (cliIsStackingSupported() == L7_TRUE)
      {
        if ((cliValidSpecificUSPCheck(argv[index+relIndex], &unit, &slot, &port) != L7_SUCCESS) ||
            (usmDbIntIfNumFromUSPGet(unit, slot, port, &mirrorVal) != L7_SUCCESS))
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
        }
      }
      else
      {
        unit = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[index+relIndex], &slot, &port, &mirrorVal) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
    }

    /* redirect */
    else if (strcmp(pStrInfo_qos_AclRedirectsStr, argv[index+relIndex]) == 0)
    {
      if (totalArg == relIndex)
      {
        ipv6AclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      /* mirror and redirect rule attributes are mutually-exclusive */
      if (mirrorOrRedirectSpecified != L7_FALSE)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_CfgureAclDenyLog);
      }
      mirrorOrRedirectSpecified = L7_TRUE;
      relIndex++;
      if (cliIsStackingSupported() == L7_TRUE)
      {
        if ((cliValidSpecificUSPCheck(argv[index+relIndex], &unit, &slot, &port) != L7_SUCCESS) ||
            (usmDbIntIfNumFromUSPGet(unit, slot, port, &redirectVal) != L7_SUCCESS))
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
        }
      }
      else
      {
        unit = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[index+relIndex], &slot, &port, &redirectVal) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
    }
    relIndex++;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Get the new rule number*/
    if (usmDbQosAclRuleGetFirst(unit, aclId, &aclRuleNum)!= L7_SUCCESS)
    {
      aclRuleNum = L7_ACL_MIN_RULE_NUM;
    }
    else
    {
      while (usmDbQosAclRuleGetNext(unit, aclId, aclRuleNum, &aclruleNext)== L7_SUCCESS)
      {
        aclRuleNum = aclruleNext;
      }
      aclRuleNum = aclRuleNum+1;
    }
    if (aclRuleNum > L7_ACL_MAX_RULE_NUM)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_RuleError);

      /* it's a warning */
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* Add the rule to acl*/

    rc = usmDbQosAclRuleActionAdd(unit, aclId, aclRuleNum, actionType );
    if (rc == L7_ERROR)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AclIdNotExist);
    }
    else if (rc == L7_TABLE_IS_FULL)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, "Maximum number of ACL rules already configured.");
    }
    else if (rc != L7_SUCCESS)
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, buf, pStrErr_qos_ProcessingAcl);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    if ((matchEvery == L7_TRUE) ||
        (matchSrc == L7_TRUE && matchDst == L7_TRUE && matchOther == L7_FALSE))
    {
      rc = usmDbQosAclRuleEveryAdd(unit, aclId, aclRuleNum, L7_TRUE);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
    }
    else
    {
      if (matchSrc == L7_FALSE)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID) == L7_TRUE)
        {
          rc = usmDbQosAclRuleSrcIpv6AddrAdd(unit, aclId, aclRuleNum, &srcIpv6Addr);

          if (rc != L7_SUCCESS)
          {
            aclClean(isAclAdded, aclId, aclRuleNum);
            sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, buf, pStrErr_qos_ProcessingAcl);
            return cliSyntaxReturnPrompt (ewsContext, buf);
          }
        }
      }

      /* Add the protocol value*/
      if (protVal != 0)
      {
        rc = usmDbQosAclRuleProtocolAdd(unit, aclId, aclRuleNum, protVal);
        if (rc != L7_SUCCESS)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
      }
      if (srcPortValue > -1)
      {
        rc = usmDbQosAclRuleSrcL4PortAdd(unit, aclId, aclRuleNum, srcPortValue);
      }
      else if (srcStartPort > -1)
      {
        rc = usmDbQosAclRuleSrcL4PortRangeAdd(unit, aclId, aclRuleNum,srcStartPort, srcEndPort);
      }
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }

      if (matchDst == L7_FALSE)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE)
        {
          rc = usmDbQosAclRuleDstIpv6AddrAdd(unit, aclId, aclRuleNum, &dstIpv6Addr);

          if (rc != L7_SUCCESS)
          {
            aclClean(isAclAdded, aclId, aclRuleNum);
            sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, buf, pStrErr_qos_ProcessingAcl);
            return cliSyntaxReturnPrompt (ewsContext, buf);
          }
        }
      }

      if (dstPortValue > -1)
      {
        rc = usmDbQosAclRuleDstL4PortAdd(unit, aclId, aclRuleNum, dstPortValue);
      }
      else if (dstStartPort > -1)
      {
        rc = usmDbQosAclRuleDstL4PortRangeAdd(unit, aclId, aclRuleNum,dstStartPort, dstEndPort);
      }
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }

      if (dscpVal > -1)
      {
        rc = usmDbQosAclRuleIPDscpAdd(unit, aclId, aclRuleNum, dscpVal);
        if (rc != L7_SUCCESS)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
      }

      if (flowLabelVal > -1)
      {
        rc = usmDbQosAclRuleIpv6FlowLabelAdd(unit, aclId, aclRuleNum, flowLabelVal);
        if (rc != L7_SUCCESS)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
      }
    }

    /* the log, assign-queue, mirror and redirect attributes are used
     * for both standard and extended ACLs
     */

    /* Log */
    if (logSpecified == L7_TRUE)
    {
      rc = usmDbQosAclRuleLoggingAdd(unit, aclId, aclRuleNum, logSpecified);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    /* Assign-Queue */
    if (assignQueueVal > -1)
    {
      rc = usmDbQosAclRuleAssignQueueIdAdd(unit, aclId, aclRuleNum, assignQueueVal);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    /* Mirror */
    if (mirrorVal > -1)
    {
      rc = usmDbQosAclRuleMirrorIntfAdd(unit, aclId, aclRuleNum, mirrorVal);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    /* Redirect */
    if (redirectVal > -1)
    {
      rc = usmDbQosAclRuleRedirectIntfAdd(unit, aclId, aclRuleNum, redirectVal);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the IPv6 ACL mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliIpv6AccessListMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliIpv6AccessListPrompt(index+1, pStrInfo_qos_Ipv6AccessListCfgModePrompt, pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the IPv6 ACL mode
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  #> [no] ipv6 access-list <name>
*
* @cmdhelp
*
* @cmddescript  Enters the ipv6-access-list config mode.
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliIpv6AccessListPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt;
  L7_uint32 unit, index, maxAcls, rc;
  L7_char8 strName[L7_ACL_NAME_LEN_MAX+1];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType != CLI_NO_CMD)
  {
    prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,cliGetMode(L7_GLOBAL_CONFIG_MODE));
    if (prompt != NULL)
    {
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return prompt;
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ((argc == depth + 1) &&
      (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_IPV6_ACCESS_LIST_CONFIG_MODE)))
  {

    if (strlen(argv[depth]) > L7_ACL_NAME_LEN_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    osapiStrncpySafe(strName, argv[depth], sizeof(strName));

    if (usmDbQosAclNameStringCheck(unit, strName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    /* Check to see if IPv6 ACL exists */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Does not exist - create and add */
        if (usmDbQosAclNameToIndex(unit, strName, &index) != L7_SUCCESS)
        {
          rc = usmDbQosAclNamedIndexNextFree(unit, L7_ACL_TYPE_IPV6, &index);
          if (rc != L7_SUCCESS)
          {

            if (rc == L7_TABLE_IS_FULL)
            {
              (void)usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_MaxOfAclsIsAlreadyCfgured, maxAcls);
              return cliSyntaxReturnPrompt (ewsContext, buf);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToCreateAccessList);
            }
          }
          if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            rc = usmDbQosAclCreate(unit, index);
          }

          if (rc != L7_SUCCESS)
          {
            if (rc == L7_TABLE_IS_FULL)
            {
              usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrInfo_qos_MaxOfAclsIsAlreadyCfgured, maxAcls);
              return cliSyntaxReturnPrompt (ewsContext, buf);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToCreateAccessList);
            }
          }

          if (usmDbQosAclNameAdd(unit, index, strName) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToCreateAccessList);
          }
        }
        else
        {
          /* found an existing IPv6 ACL under this name, check that it is an IPv6 ACL */
          if (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IPV6, index) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, "Specified access list is not an IPv6 ACL.");
          }
        }
      }
      strcpy(EWSACCESSLISTNAME(ewsContext),strName);

      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_IPV6_ACCESS_LIST_CONFIG_MODE));
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ((usmDbQosAclNameToIndex(unit, strName, &index) == L7_SUCCESS) &&
            (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IPV6, index) == L7_SUCCESS))
        {
          if (usmDbQosAclDelete(unit, index) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDelFail);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
        }
      }
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    cliSyntaxBottom(ewsContext);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return NULL;
}

/*********************************************************************
*
* @purpose Apply an access list to interface(s) for a specified direction.
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  #> [no] ipv6 traffic-filter <name> {in | out} [<sequence>]
*
* @cmdhelp
*
* @cmddescript  Attaches an IPv6 ACL to interfaces.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpv6TrafficFilter(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 argAclName = 1;
  L7_uint32 argDir;
  L7_uint32 argAclSequence;
  L7_uint32 argAclVlanKeyword = 2;
  L7_uint32 argAclVlanId = 3;
  L7_BOOL aclVlanIdGiven = L7_FALSE;
  L7_RC_t rc;
  L7_BOOL allInterfaces = L7_FALSE;
  L7_uint32 interface, aclId, direction, tempDirection, feature, intListSize;
  L7_uint32 vlanId;
  L7_char8 strDir[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32 sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  L7_uint32 unit, numArgs, numArgsCheckLow, numArgsCheckHigh, unitNum, slot, port;
  L7_BOOL noAclName = L7_FALSE;
  L7_BOOL commaFlag = L7_FALSE;
  L7_BOOL crFlag = L7_FALSE;
  L7_char8 * interfaceList = L7_NULLPTR;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  L7_uint32 i = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArgs = cliNumFunctionArgsGet();

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    argDir = 2;
    argAclSequence = 3;
    numArgsCheckLow = 2;
    numArgsCheckHigh = 3;
  }
  else
  {
    /* determine if ACL is being applied to a VLAN ID */
    if ((numArgs > 3) && (strcmp(argv[index+argAclVlanKeyword], pStrInfo_common_MacAclVlan_1) == 0))
    {
      aclVlanIdGiven = L7_TRUE;
      argDir = 4;
      argAclSequence = 5;
      numArgsCheckLow = 4;
      numArgsCheckHigh = 5;

      /* get the vlan id specified by user */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argAclVlanId], &vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlanId_1);
      }
      /* verify if the specified value is in valid range */
      if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_FALSE)
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_ValVlanIdMustBeFromTo, L7_ACL_MIN_VLAN_ID, L7_ACL_MAX_VLAN_ID);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
    }
    else
    {
      argDir = 2;
      argAclSequence = 3;
      numArgsCheckLow = 2;
      numArgsCheckHigh = 3;
      allInterfaces = L7_TRUE;
    }
  }

  if ((numArgs < numArgsCheckLow) || (numArgs > numArgsCheckHigh))     /* parameter check */
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_FALSE)
    {
      /* applying ACLs to VLANs _not_ supported */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
        {
          /* both in and out supported */
          if (ewsContext->commType != CLI_NO_CMD)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgIpv6AclAllGrp);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoIpv6AclAllGrp);
          }
        }
        else
        {
          /* only in supported */
          if (ewsContext->commType != CLI_NO_CMD)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgIpv6AclAllGrpIn);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoIpv6AclAllGrpIn);
          }
        }
      }
      else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                        L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
      {
        /* only out supported */
        if (ewsContext->commType != CLI_NO_CMD)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgIpv6AclAllGrpOut);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoIpv6AclAllGrpOut);
        }
      }
      else
      {
        /* neither in nor out direction supported */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListGrpDirNotSupported);
      }
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else
    {
      /* applying ACLs to VLANs supported */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
        {
          /* both in and out supported */
          if (ewsContext->commType != CLI_NO_CMD)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgIpv6AclAllGrpVlan);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoIpv6AclAllGrpVlan);
          }
        }
        else
        {
          /* only in supported */
          if (ewsContext->commType != CLI_NO_CMD)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgIpv6AclAllGrpVlanIn);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoIpv6AclAllGrpVlanIn);
          }
        }
      }
      else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                        L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
      {
        /* only out supported */
        if (ewsContext->commType != CLI_NO_CMD)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgIpv6AclAllGrpVlanOut);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoIpv6AclAllGrpVlanOut);
        }
      }
      else
      {
        /* neither in nor out direction supported */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListGrpDirNotSupported);
      }
    }
  }

  if ((numArgs == 3 || numArgs == 5) &&
      cliConvertTo32BitUnsignedInteger(argv[index + argAclSequence], &sequence) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_qos_AccessListGrpsSeq);
  }

  osapiStrncpySafe( aclName, argv[index+argAclName], sizeof(aclName));

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ((usmDbQosAclNameToIndex(unit, aclName, &aclId) != L7_SUCCESS) ||
        (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IPV6, aclId) != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
    }
  }

  if (strlen(argv[index+argDir]) >= sizeof(strDir))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  osapiStrncpySafe(strDir,argv[index+argDir], sizeof(strDir));

  if (strcmp(strDir, pStrInfo_common_AclInStr) == 0)       /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (strcmp(strDir, pStrInfo_common_AclOutStr) == 0)       /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (aclVlanIdGiven == L7_FALSE)
    {
      /* applying ACL to interfaces */
      if (allInterfaces == L7_FALSE)
      {

        if (usmDbQosAclIsDiffServIntfInUse( unit, interface, direction ) == L7_TRUE &&
            usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_INTF_DIFFSERV_COEXIST_FEATURE_ID ) == L7_FALSE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_AclAndDiffservOnSameIntf);
        }

        if (ewsContext->commType == CLI_NORMAL_CMD)
        {
          rc = usmDbQosAclInterfaceDirectionAdd(unit, interface, direction, aclId, sequence);
        }
        else
        {
          rc = usmDbQosAclInterfaceDirectionRemove(unit, interface, direction, aclId);
        }

        if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AclTblFull);
        }
        else if (rc == L7_ALREADY_CONFIGURED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_IntfAndDirectionAreAlreadyInUse);
        }
        else if (rc == L7_REQUEST_DENIED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_IpAclNotSupported);
        }
        else if (rc != L7_SUCCESS)
        {
          /* Able to remove or add an interface, so set flag to FALSE */
          noAclName = L7_TRUE;
        }
      }/* end  if (allInterfaces == L7_FALSE) */
      else
      { /* allInterfaces != L7_FALSE */
        /* interface = get first interface, if fail, exit */
        interface = tempDirection = 0;
        if (usmDbQosAclIntfDirGetNext( unit, interface, tempDirection,
                                      &interface, &tempDirection ) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AccessListGrpNoIntfs);
        }

        /* loop through all ACL interfaces for this direction */
        do
        {
          if (direction == tempDirection)
          {
            if (usmDbQosAclIsDiffServIntfInUse( unit, interface, direction ) == L7_TRUE &&
                usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                         L7_ACL_INTF_DIFFSERV_COEXIST_FEATURE_ID ) == L7_FALSE)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_AclAndDiffservOnSameIntf);
            }
          }

          rc = usmDbQosAclIntfDirGetNext( unit, interface, tempDirection,
                                         &interface, &tempDirection );
          if (rc != L7_SUCCESS)
          {
            break;
          }

        } while (1);

        /* interface = get first interface, if fail, exit */
        interface = tempDirection = 0;
        if (usmDbQosAclIntfDirGetNext( unit, interface, tempDirection,
                                      &interface, &tempDirection ) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AccessListGrpNoIntfs);
        }

        /* potential size of list of interfaces */
        if (cliIsStackingSupported() == L7_TRUE)
        {
          intListSize = L7_MAX_PORT_COUNT * 12;
        }
        else
        {
          intListSize = L7_MAX_PORT_COUNT * 9;
        }
        interfaceList = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, intListSize);
        memset (stat, 0, sizeof(stat));
        /*If no command issued, get the list of assigned interfaces*/
        if (ewsContext->commType == CLI_NO_CMD)
        {
          rc = usmDbQosAclAssignedIntfDirListGet(unit, aclId, direction, &intfList);
          if (intfList.count == 0)
          {
             if (interfaceList != L7_NULLPTR)
             {
               osapiFree(L7_CLI_WEB_COMPONENT_ID, interfaceList);
             }
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListNoIntfsAttached);

          }
          else if (rc != L7_SUCCESS)
          {
            if (interfaceList != L7_NULLPTR)
            {
              osapiFree(L7_CLI_WEB_COMPONENT_ID, interfaceList);
            }
            return cliPrompt(ewsContext);
          }
        }

        do
        {
          if (direction == tempDirection)
          {
            /**Check if the script execute Flag is set for execute then execute the usmdb**/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              if (ewsContext->commType == CLI_NORMAL_CMD)
              {
                rc = usmDbQosAclInterfaceDirectionAdd(unit, interface, direction, aclId, sequence);
              }
              else
              {
                /*If no command, if the interface is an assigned interface, call remove
                 *otherwise, set rc to L7_SUCCESS*/
                for (i = 0; i <= intfList.count; i++)
                {
                  rc = L7_SUCCESS;
                  if (interface == intfList.intIfNum[i])
                  {
                    rc = usmDbQosAclInterfaceDirectionRemove(unit, interface, direction, aclId);
                    break;
                  }
                }
              }

              if (rc != L7_SUCCESS)
              {
                /* Able to remove or add an interface, so set flag to FALSE */
                noAclName = L7_TRUE;
                if (usmDbUnitSlotPortGet(interface, &unitNum, &slot, &port) == L7_SUCCESS)
                {
                  sprintf(buf, cliDisplayInterfaceHelp(unitNum, slot, port));
                  if (commaFlag == L7_TRUE && crFlag == L7_FALSE)
                  {
                    strcat(stat, ",");
                  }

                  strcat(stat, buf);
                  commaFlag = L7_TRUE;
                  if (strlen(stat) > 72)
                  {
                    sprintf(buf, "%s,\r\n", stat);
                    strcat(interfaceList, buf);
                    memset (stat, 0, sizeof(stat));
                    crFlag = L7_TRUE;
                  }
                  else
                  {
                    crFlag = L7_FALSE;
                  }
                }
              }
            }
          }

          rc = usmDbQosAclIntfDirGetNext( unit, interface, tempDirection,
                                         &interface, &tempDirection );
          if (rc != L7_SUCCESS)
          {
            break;
          }

        } while (1);

        sprintf(buf, "%s.", stat);
        strcat(interfaceList, buf);
      }/* end else (allInterfaces != L7_FALSE) */

      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (noAclName == L7_TRUE)
        {
          /* If not able to add or remove for even a single interface to the
           * access-group,displaying proper error-message
           */
          if (allInterfaces == L7_TRUE)
          {
            if (ewsContext->commType == CLI_NORMAL_CMD)
            {
              if (direction == L7_INBOUND_ACL)
              {
                sprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, pStrErr_qos_AccessListDirectionIn);
              }
              else
              {
                sprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, pStrErr_qos_AccessListDirectionOut);
              }
            }
            else if (ewsContext->commType == CLI_NO_CMD)
            {
              if (direction == L7_INBOUND_ACL)
              {
                sprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, pStrErr_qos_AccessListDirectionInNo);
              }
              else
              {
                sprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, pStrErr_qos_AccessListDirectionOutNo);
              }
            }
            ewsTelnetWrite(ewsContext, buf);
            ewsTelnetWrite(ewsContext, interfaceList);
            cliSyntaxBottom(ewsContext);
          }
          else
          {
            if (ewsContext->commType == CLI_NORMAL_CMD)
            {
              if (direction == L7_INBOUND_ACL)
              {
                ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListIntfDirIn);
              }
              else
              {
                ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListIntfDirOut);
              }
            }
            else if (ewsContext->commType == CLI_NO_CMD)
            {
              if (direction == L7_INBOUND_ACL)
              {
                ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListIntfDirInNo);
              }
              else
              {
                ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListIntfDirOutNo);
              }
            }
          }
        }
      }

      if (interfaceList != L7_NULLPTR)
      {
        osapiFree(L7_CLI_WEB_COMPONENT_ID, interfaceList);
      }
    }
    else
    {
      /* applying ACL to a VLAN */

      /**Check if the script execute Flag is set for execute then execute the usmdb**/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (ewsContext->commType == CLI_NORMAL_CMD)
        {
          if ((rc = usmDbQosAclVlanDirectionAdd(unit, vlanId, direction, aclId, sequence)) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListVlanAdd);
          }
        }
        else
        {
          if ((rc = usmDbQosAclVlanDirectionRemove(unit, vlanId, direction, aclId)) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListVlanAddNo);
          }
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Function to rename the MAC Access List
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  #> [no] ipv6 access-list rename <oldname> <newname>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpAccessListRename(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs, oldMacIndex, newMacIndex;
  L7_uint32 argOld = 1;
  L7_uint32 argNew = 2;
  L7_char8 newStrName[L7_ACL_NAME_LEN_MAX+1];
  L7_char8 oldStrName[L7_ACL_NAME_LEN_MAX+1];

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgIpv6AccessListRename);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (strlen(argv[index+argOld]) > L7_ACL_NAME_LEN_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
  }

  osapiStrncpySafe(oldStrName,argv[index+argOld], sizeof(oldStrName));

  /* Ensure old Acccess List name exists */
  if (usmDbQosAclNameToIndex(unit, oldStrName, &oldMacIndex) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
  }

  osapiStrncpySafe(newStrName,argv[index+argNew], sizeof(newStrName));

  /* Check to see if new name already exists */
  if (usmDbQosAclNameToIndex(unit, newStrName, &newMacIndex) == L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListAlreadyExist);
  }

  if (usmDbQosAclNameStringCheck(unit, newStrName) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
  }

  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbQosAclNameChange(unit, oldMacIndex, newStrName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToRenameAccessList);
    }
  }
  return cliPrompt(ewsContext);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return NULL;
}

/*********************************************************************
*
* @purpose Function to set or exit from the cli interface mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliMacAccessListMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliMacAccessListPrompt(index+3, pStrInfo_qos_MacAccessListCfgModePrompt,pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the bandwidth allocation mode.
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  #> [no] mac access-list extended <name>
*
* @cmdhelp
*
* @cmddescript  Enters the mac-access-list config mode.
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliMacAccessListPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt;
  L7_uint32 unit, index, maxAcls, rc;
  L7_char8 strName[L7_ACL_NAME_LEN_MAX+1];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType != CLI_NO_CMD)
  {
    prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,cliGetMode(L7_GLOBAL_CONFIG_MODE));
    if (prompt != NULL)
    {
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return prompt;
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ((argc == depth + 1) &&
      (mainMenu[cliCurrentHandleGet()] !=cliGetMode(L7_MAC_ACCESS_LIST_CONFIG_MODE)))
  {

    if (strlen(argv[depth]) > L7_ACL_NAME_LEN_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    osapiStrncpySafe(strName,argv[depth], sizeof(strName));

    if (usmDbQosAclMacNameStringCheck(unit, strName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    /* Check to see if MAC ACL exists */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Does not exist - create and add */
        if (usmDbQosAclMacNameToIndex(unit, strName, &index) != L7_SUCCESS)
        {
          rc = usmDbQosAclMacIndexNext(unit, &index);
          if (rc != L7_SUCCESS)
          {

            if (rc == L7_TABLE_IS_FULL)
            {
              usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_qos_MaxOfAclsIsAlreadyCfgured, maxAcls);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToCreateAccessList);
            }
          }

          rc = usmDbQosAclMacCreate(unit, index);

          if (rc != L7_SUCCESS)
          {
            if (rc == L7_TABLE_IS_FULL)
            {
              usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_qos_MaxOfAclsIsAlreadyCfgured, maxAcls);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToCreateAccessList);
            }
          }

          if (usmDbQosAclMacNameAdd(unit, index, strName) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToCreateAccessList);
          }
        }
      } /* End of if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT) */

      strcpy(EWSACCESSLISTNAME(ewsContext),strName);

      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_MAC_ACCESS_LIST_CONFIG_MODE));
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbQosAclMacNameToIndex(unit, strName, &index) == L7_SUCCESS)
        {
          if (usmDbQosAclMacDelete(unit, index) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDelFail);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
        }
      } /* End of if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT) */

      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    cliSyntaxBottom(ewsContext);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return NULL;
}

/*********************************************************************
*
* @purpose Function to set or exit from the bandwidth allocation mode.
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  #> [no] mac access-group <name> {in | out} [<sequence>]
*
* @cmdhelp
*
* @cmddescript  Attaches MAC ACL to interfaces.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMacAccessGroup(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 argMacAclName = 1;
  L7_uint32 argDir;
  L7_uint32 argAclSequence;
  L7_uint32 argAclVlanKeyword = 2;
  L7_uint32 argAclVlanId = 3;
  L7_BOOL aclVlanIdGiven = L7_FALSE;
  L7_RC_t rc;
  L7_BOOL allInterfaces = L7_FALSE;
  L7_uint32 interface, aclId, direction, tempDirection, feature, intListSize;
  L7_uint32 vlanId;
  L7_char8 strDir[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 macAclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32 sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  L7_uint32 unit, numArgs, numArgsCheckLow, numArgsCheckHigh, unitNum, slot, port;
  L7_BOOL noMacAclName = L7_FALSE;
  L7_BOOL commaFlag = L7_FALSE;
  L7_BOOL crFlag = L7_FALSE;
  L7_char8 * interfaceList = L7_NULLPTR;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  L7_uint32 i = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArgs = cliNumFunctionArgsGet();

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    argDir = 2;
    argAclSequence = 3;
    numArgsCheckLow = 2;
    numArgsCheckHigh = 3;
  }
  else
  {
    /* determine if ACL is being applied to a VLAN ID */
    if ((numArgs > 3) && (strcmp(argv[index+argAclVlanKeyword], pStrInfo_common_MacAclVlan_1) == 0))
    {
      aclVlanIdGiven = L7_TRUE;
      argDir = 4;
      argAclSequence = 5;
      numArgsCheckLow = 4;
      numArgsCheckHigh = 5;

      /* get the vlan id specified by user */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argAclVlanId], &vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlanId_1);
      }
      /* verify if the specified value is in valid range */
      if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_FALSE)
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_ValVlanIdMustBeFromTo, L7_ACL_MIN_VLAN_ID, L7_ACL_MAX_VLAN_ID);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
    }
    else
    {
      argDir = 2;
      argAclSequence = 3;
      numArgsCheckLow = 2;
      numArgsCheckHigh = 3;
      allInterfaces = L7_TRUE;
    }
  }

  if ((numArgs < numArgsCheckLow) || (numArgs > numArgsCheckHigh))      /* parameter check */
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_FALSE)
    {
      /* applying ACLs to VLANs _not_ supported */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
        {
          /* both in and out supported */
          if (ewsContext->commType != CLI_NO_CMD)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclAllGrp);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoAclAllGrp);
          }
        }
        else
        {
          /* only in supported */
          if (ewsContext->commType != CLI_NO_CMD)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclAllGrpIn);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoAclAllGrpIn);
          }
        }
      }
      else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                        L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
      {
        /* only out supported */
        if (ewsContext->commType != CLI_NO_CMD)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclAllGrpOut);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoAclAllGrpOut);
        }
      }
      else
      {
        /* neither in nor out direction supported */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListGrpDirNotSupported);
      }
    }
    else
    {
      /* applying ACLs to VLANs supported */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
        {
          /* both in and out supported */
          if (ewsContext->commType != CLI_NO_CMD)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclAllGrpVlan);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoAclAllGrpVlan);
          }
        }
        else
        {
          /* only in supported */
          if (ewsContext->commType != CLI_NO_CMD)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclAllGrpVlanIn);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoAclAllGrpVlanIn);
          }
        }
      }
      else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                        L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
      {
        /* only out supported */
        if (ewsContext->commType != CLI_NO_CMD)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgAclAllGrpVlanOut);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_CfgNoAclAllGrpVlanOut);
        }
      }
      else
      {
        /* neither in nor out direction supported */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListGrpDirNotSupported);
      }
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if ((numArgs == 3 || numArgs == 5) &&
      (cliConvertTo32BitUnsignedInteger(argv[index + argAclSequence], &sequence) != L7_SUCCESS || sequence <= 0))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_qos_AccessListGrpsSeq);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    osapiStrncpySafe( macAclName, argv[index+argMacAclName], sizeof(macAclName));

    if (usmDbQosAclMacNameToIndex(unit, macAclName, &aclId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
    }

    if (usmDbQosAclMacIndexCheckValid(unit, aclId) == L7_ERROR)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_MacAclDoesntExist);
    }
  }

  if (strlen(argv[index+argDir]) >= sizeof(strDir))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  osapiStrncpySafe(strDir,argv[index+argDir], sizeof(strDir));

  if (strcmp(strDir, pStrInfo_common_AclInStr) == 0)        /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (strcmp(strDir, pStrInfo_common_AclOutStr) == 0)        /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Direction_1);
  }

  if (aclVlanIdGiven == L7_FALSE)
  {
    /* applying ACL to interfaces */
    if (allInterfaces == L7_FALSE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbQosAclIsDiffServIntfInUse( unit, interface, direction ) == L7_TRUE &&
            usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_INTF_DIFFSERV_COEXIST_FEATURE_ID ) == L7_FALSE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_AclAndDiffservOnSameIntf);
        }

        if (ewsContext->commType == CLI_NORMAL_CMD)
        {
          rc = usmDbQosAclMacInterfaceDirectionAdd(unit, interface, direction, aclId, sequence);
        }
        else
        {
          rc = usmDbQosAclMacInterfaceDirectionRemove(unit, interface, direction, aclId);
        }

        if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AclTblFull);
        }
        else if (rc == L7_ALREADY_CONFIGURED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_IntfAndDirectionAreAlreadyInUse);
        }
        else if (rc == L7_REQUEST_DENIED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_MacAclNotSupported);
        }
        else if (rc != L7_SUCCESS)
        {
          /* Able to remove or add an interface, so set flag to FALSE */
          noMacAclName = L7_TRUE;
        }
      } /* End of if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT) */
    }
    else
    {
      /* interface = get first interface, if fail, exit */
      interface = tempDirection = 0;
      if (usmDbQosAclIntfDirGetNext( unit, interface, tempDirection,
                                    &interface, &tempDirection ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AccessListGrpNoIntfs);
      }

      /* loop through all ACL interfaces for this direction */
      do
      {
        if (direction == tempDirection)
        {
          if (usmDbQosAclIsDiffServIntfInUse( unit, interface, direction ) == L7_TRUE &&
              usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                       L7_ACL_INTF_DIFFSERV_COEXIST_FEATURE_ID ) == L7_FALSE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_AclAndDiffservOnSameIntf);
          }
        }

        rc = usmDbQosAclIntfDirGetNext( unit, interface, tempDirection,
                                       &interface, &tempDirection );
        if (rc != L7_SUCCESS)
        {
          break;
        }

      } while (1);

      /* interface = get first interface, if fail, exit */
      interface = tempDirection = 0;
      if (usmDbQosAclIntfDirGetNext( unit, interface, tempDirection,
                                    &interface, &tempDirection ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AccessListGrpNoIntfs);
      }

      /* potential size of list of interfaces */
      if (cliIsStackingSupported() == L7_TRUE)
      {
        intListSize = L7_MAX_PORT_COUNT * 12;
      }
      else
      {
        intListSize = L7_MAX_PORT_COUNT * 9;
      }
      interfaceList = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, intListSize);
      memset (stat, 0, sizeof(stat));
      /*If no command issued, get the list of assigned interfaces*/
      if(ewsContext->commType == CLI_NO_CMD)
      {
        rc = usmDbQosAclMacAssignedIntfDirListGet(unit, aclId, direction, &intfList);
        if(intfList.count == 0)
        {
          if (interfaceList != L7_NULLPTR)
          {
            osapiFree(L7_CLI_WEB_COMPONENT_ID, interfaceList);
          }
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListNoIntfsAttached);

        }
        else if(rc != L7_SUCCESS)
        {
          if (interfaceList != L7_NULLPTR)
          {
            osapiFree(L7_CLI_WEB_COMPONENT_ID, interfaceList);
          }
          return cliPrompt(ewsContext);
        }
      }

      do
      {
        if (direction == tempDirection)
        {
          /**Check if the script execute Flag is set for execute then execute the usmdb**/
          if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (ewsContext->commType == CLI_NORMAL_CMD)
            {
              rc = usmDbQosAclMacInterfaceDirectionAdd(unit, interface, direction, aclId, sequence);
            }
            else
            {
              /*If no command, if the interface is an assigned interface, call remove
               *otherwise, set rc to L7_SUCCESS*/
              for(i = 0; i <= intfList.count; i++)
              {
                rc = L7_SUCCESS;
                if(interface == intfList.intIfNum[i])
                {
                  rc = usmDbQosAclMacInterfaceDirectionRemove(unit, interface, direction, aclId);
                  break;
                }
              }
            }

            if (rc != L7_SUCCESS)
            {
              /* Able to remove or add an interface, so set flag to FALSE */
              noMacAclName = L7_TRUE;
              if (usmDbUnitSlotPortGet(interface, &unitNum, &slot, &port) == L7_SUCCESS)
              {
                sprintf(buf, cliDisplayInterfaceHelp(unitNum, slot, port));
                if (commaFlag == L7_TRUE && crFlag == L7_FALSE)
                {
                  strcat(stat, ",");
                }

                strcat(stat, buf);
                commaFlag = L7_TRUE;
                if (strlen(stat) > 72)
                {
                  sprintf(buf, "%s,\r\n", stat);
                  strcat(interfaceList, buf);
                  memset (stat, 0, sizeof(stat));
                  crFlag = L7_TRUE;
                }
                else
                {
                  crFlag = L7_FALSE;
                }
              }
            }
          }
        }

        rc = usmDbQosAclIntfDirGetNext( unit, interface, tempDirection,
                                       &interface, &tempDirection );
        if (rc != L7_SUCCESS)
        {
          break;
        }

      } while (1);

      sprintf(buf, "%s.", stat);
      strcat(interfaceList, buf);
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (noMacAclName == L7_TRUE)
      {
        /* If not able to add or remove for even a single interface to the
         * access-group,displaying proper error-message
         */
        if (allInterfaces == L7_TRUE)
        {
          memset (buf, 0,sizeof(buf));
          if (ewsContext->commType == CLI_NORMAL_CMD)
          {
            if (direction == L7_INBOUND_ACL)
            {
              sprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, pStrErr_qos_AccessListDirectionIn);
            }
            else
            {
              sprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, pStrErr_qos_AccessListDirectionOut);
            }
          }
          else if (ewsContext->commType == CLI_NO_CMD)
          {
            if (direction == L7_INBOUND_ACL)
            {
              sprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, pStrErr_qos_AccessListDirectionInNo);
            }
            else
            {
              sprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, pStrErr_qos_AccessListDirectionOutNo);
            }
          }
          ewsTelnetWrite(ewsContext, buf);
          ewsTelnetWrite(ewsContext, interfaceList);
          cliSyntaxBottom(ewsContext);
        }
        else
        {

          if (ewsContext->commType == CLI_NORMAL_CMD)
          {
            if (direction == L7_INBOUND_ACL)
            {
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListIntfDirIn);
            }
            else
            {
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListIntfDirOut);
            }
          }
          else if (ewsContext->commType == CLI_NO_CMD)
          {
            if (direction == L7_INBOUND_ACL)
            {
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListIntfDirInNo);
            }
            else
            {
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListIntfDirOutNo);
            }
          }
        }
      }
    }

    if (interfaceList != L7_NULLPTR)
    {
      osapiFree(L7_CLI_WEB_COMPONENT_ID, interfaceList);
    }
  }
  else
  {
    /* applying ACL to a VLAN */

    /**Check if the script execute Flag is set for execute then execute the usmdb**/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        if ((rc = usmDbQosAclMacVlanDirectionAdd(unit, vlanId, direction, aclId, sequence)) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListVlanAdd);
        }
      }
      else
      {
        if ((rc = usmDbQosAclMacVlanDirectionRemove(unit, vlanId, direction, aclId)) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_AccessListVlanAddNo);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Function to rename the MAC Access List
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  #> [no] mac access-list extended rename <oldname> <newname>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandMacAccessListRename(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs, oldMacIndex, newMacIndex;
  L7_uint32 argOld = 1;
  L7_uint32 argNew = 2;
  L7_char8 newStrName[L7_ACL_NAME_LEN_MAX+1];
  L7_char8 oldStrName[L7_ACL_NAME_LEN_MAX+1];

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgMacAccessListRename);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (strlen(argv[index+argOld]) > L7_ACL_NAME_LEN_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
  }

  osapiStrncpySafe(newStrName,argv[index+argNew], sizeof(newStrName));

  if (usmDbQosAclMacNameStringCheck(unit, newStrName) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
  }

  /**Check if the script execute Flag is set for execute then execute the usmdb**/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    osapiStrncpySafe(oldStrName,argv[index+argOld], sizeof(oldStrName));

    /* Ensure old MAC Acccess List exists */
    if (usmDbQosAclMacNameToIndex(unit, oldStrName, &oldMacIndex) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
    }

    /* Check to see if new name already exists */
    if (usmDbQosAclMacNameToIndex(unit, newStrName, &newMacIndex) == L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListAlreadyExist);
    }

    if (usmDbQosAclMacNameChange(unit, oldMacIndex, newStrName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_qos_UnableToRenameAccessList);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Creates/deletes MAC ACL list.
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  {deny | permit } <srcmac> <srcmacmask> <dstmac> <dstmacmask>
*             [log] [assign-queue <queue-id>]
*             [{mirror | redirect} <unit/slot/port>]
*
* @cmdhelp
*
* @cmddescript  Creates rules for MAC acl lists.
*
* @end
*
*********************************************************************/
const L7_char8 *commandMacAccessListDenyPermit(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 relIndex = 1;
  L7_uint32 unit, aclId, aclRuleNum, aclruleNext, slot, port;
  L7_char8 strAction[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 actionType;
  L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN];
  L7_uchar8 srcMacMask[L7_MAC_ADDR_LEN];
  L7_uchar8 dstMacAddr[L7_MAC_ADDR_LEN];
  L7_uchar8 dstMacMask[L7_MAC_ADDR_LEN];
  L7_uchar8 strFirstToken[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 totalArg = 0;
  L7_int32 tempValue;
  L7_int32 eTypeKeyVal = -1;
  L7_int32 eTypeCustVal = -1;
  L7_int32 vlanVal = -1;
  L7_int32 startVlanVal = -1;
  L7_int32 endVlanVal = -1;
  L7_int32 secondaryVlanVal = -1;
  L7_int32 startSecondaryVlanVal = -1;
  L7_int32 endSecondaryVlanVal = -1;
  L7_int32 cosVal = -1;
  L7_int32 secondaryCosVal = -1;
  L7_int32 assignQueueVal = -1;
  L7_int32 mirrorVal = -1;
  L7_int32 redirectVal = -1;
  L7_char8 macAccessListName[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL matchSrc = L7_FALSE;
  L7_BOOL matchDst = L7_FALSE;
  L7_BOOL matchOther = L7_FALSE;
  L7_BOOL maskBpdu = L7_FALSE;
  L7_BOOL isAclAdded = L7_TRUE;
  L7_BOOL logSpecified = L7_FALSE;
  L7_BOOL mirrorOrRedirectSpecified = L7_FALSE;
  L7_BOOL maskSupported;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  totalArg = cliNumFunctionArgsGet();
  if (totalArg < 2)
  {
    macAclSyntaxWrite(ewsContext);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  osapiStrncpySafe(strAction,argv[0], sizeof(strAction));
  cliConvertToLowerCase(strAction);

  if (strcmp(strAction, pStrInfo_common_Permit_1) == 0)
  {
    actionType = L7_ACL_PERMIT;
  }
  else if (strcmp(strAction, pStrInfo_common_Deny_2) == 0)
  {
    actionType = L7_ACL_DENY;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_Action_1);
  }
  if (strcmp(argv[index+ relIndex], pStrInfo_qos_AclAnyStr) == 0)
  {
    matchSrc = L7_TRUE;
    relIndex++;
  }

  if (matchSrc == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                       L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE)
  {
    memset ( srcMacAddr, 0, sizeof(srcMacAddr));
    memset ( srcMacMask, 0, sizeof(srcMacMask));
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID) == L7_TRUE)
    {
      maskSupported = L7_TRUE;
    }
    else
    {
      maskSupported = L7_FALSE;
    }

    if (getMacAndMask(ewsContext,argv,index,&relIndex, srcMacAddr, srcMacMask, maskSupported) == NULL)
    {
      return cliPrompt(ewsContext);
    }
  }

  if (strcmp(argv[index+ relIndex], pStrInfo_qos_AclAnyStr) == 0)
  {
    matchDst = L7_TRUE;
    relIndex++;
  }
  else if (strcmp(argv[index+ relIndex], pStrInfo_common_MacAclBpdusStr) == 0)
  {
    maskBpdu = L7_TRUE;
    relIndex++;
  }

  if (totalArg < relIndex - 1
      &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE))
  {
    macAclSyntaxWrite(ewsContext);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  if (maskBpdu == L7_TRUE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                      L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) == L7_TRUE)
  {
    cliConvertMac(pStrInfo_qos_0180C2000000, dstMacAddr);
    cliConvertMac(pStrInfo_qos_FfFfFf000000_1, dstMacMask);
  }

  else if (matchDst == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                            L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE)
  {
    memset (dstMacAddr, 0, sizeof(dstMacAddr));
    memset (dstMacMask, 0, sizeof(dstMacMask));
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) == L7_TRUE)
    {
      maskSupported = L7_TRUE;
    }
    else
    {
      maskSupported = L7_FALSE;
    }

    if (getMacAndMask(ewsContext,argv,index,&relIndex, dstMacAddr, dstMacMask, maskSupported) == NULL)
    {
      return cliPrompt(ewsContext);
    }
  }

  while (totalArg > relIndex - 1)
  {
    if (strlen(argv[index+relIndex]) >= sizeof(strFirstToken))
    {
      macAclSyntaxWrite(ewsContext);
      cliSyntaxBottom(ewsContext);
      return NULL;
    }

    osapiStrncpySafe (strFirstToken, argv[index+relIndex], sizeof(strFirstToken));

    /* Ethertype */
    if (cliDiffservConvertEtherTypeStringToKeyId(strFirstToken, &tempValue) == L7_SUCCESS &&
        tempValue != L7_QOS_ETYPE_KEYID_NONE)
    {
      eTypeKeyVal = tempValue;
      matchOther = L7_TRUE;
    }
    else if (cliConvertEtypeCustomValue(strFirstToken, &eTypeCustVal) == L7_TRUE)
    {
      eTypeKeyVal = L7_QOS_ETYPE_KEYID_CUSTOM;
      /* verify custom ethertype value is within allowed range */
      if ((eTypeCustVal < L7_QOS_ETYPE_CUSTOM_VAL_MIN) ||
          (eTypeCustVal > L7_QOS_ETYPE_CUSTOM_VAL_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_IncorrectEtypeCustomValRange,
                                L7_QOS_ETYPE_CUSTOM_VAL_MIN,
                                L7_QOS_ETYPE_CUSTOM_VAL_MAX);
      }
      matchOther = L7_TRUE;
    }

    /* VLAN */
    else if (strcmp(pStrInfo_common_MacAclVlan_1, argv[index+relIndex]) == 0)
    {

      if (totalArg == relIndex)
      {
        macAclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      relIndex++;
      if (strcmp(pStrInfo_qos_AclEqstr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          macAclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;
        if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &vlanVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlanId_1);
        }
        /* verify if the specified value is in between 1 to 4094 */
        if ((vlanVal < L7_ACL_MIN_VLAN_ID) ||
            (vlanVal > L7_ACL_MAX_VLAN_ID))
        {
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_ValVlanIdMustBeFromTo, L7_ACL_MIN_VLAN_ID, L7_ACL_MAX_VLAN_ID);
          return cliSyntaxReturnPrompt (ewsContext, buf);
        }

      }
      else if (strcmp(pStrInfo_common_AclRangeStr, argv[index+relIndex]) == 0)
      {
        if (totalArg == relIndex)
        {
          macAclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;
        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &startVlanVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartVlanId);
        }

        /* verify if the specified value is in between 1 to 4094 */
        if ((startVlanVal < L7_ACL_MIN_VLAN_ID) ||
            (startVlanVal > L7_ACL_MAX_VLAN_ID))
        {
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_IncorrectStartVlanVal, L7_ACL_MIN_VLAN_ID, L7_ACL_MAX_VLAN_ID);
          return cliSyntaxReturnPrompt (ewsContext, buf);
        }

        if (totalArg == relIndex)
        {
          macAclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &endVlanVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndVlanId);
        }

        /* verify if the specified value is in between 1 to 4094 */
        if ((endVlanVal < L7_ACL_MIN_VLAN_ID) ||
            (endVlanVal > L7_ACL_MAX_VLAN_ID))
        {
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_IncorrectEndVlanVal, L7_ACL_MIN_VLAN_ID, L7_ACL_MAX_VLAN_ID);
          return cliSyntaxReturnPrompt (ewsContext, buf);
        }

        if (endVlanVal < startVlanVal)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndVlanIdMustBeGreaterThanStartVlanId);
        }
      }
      matchOther = L7_TRUE;
    }

    /* COS */
    else if (strcmp(pStrInfo_qos_MacAclCos, argv[index+relIndex]) == 0)
    {
      if (totalArg == relIndex)
      {
        macAclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }

      relIndex++;

      if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &cosVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosId);
      }
      if ((cosVal < L7_ACL_MIN_COS) ||
          (cosVal > L7_ACL_MAX_COS))
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_IncorrectVlanVal, L7_ACL_MIN_COS, L7_ACL_MAX_COS);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      matchOther = L7_TRUE;
    }

    /* Secondary-VLAN */
    else if (strcmp(pStrInfo_qos_MacAclSecondaryVlan, argv[index+relIndex]) == 0)
    {
      if (totalArg == relIndex)
      {
        macAclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      relIndex++;

      if (strcmp(pStrInfo_qos_AclEqstr, argv[index+relIndex]) == 0)
      {

        if (totalArg == relIndex)
        {
          macAclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;

        if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &secondaryVlanVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlanId_1);
        }
        /* verify if the specified value is in between 1 to 4094 */
        if ((secondaryVlanVal < L7_ACL_MIN_VLAN_ID) ||
            (secondaryVlanVal > L7_ACL_MAX_VLAN_ID))
        {
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_IncorrectVlanVal, L7_ACL_MIN_VLAN_ID, L7_ACL_MAX_VLAN_ID);
          return cliSyntaxReturnPrompt (ewsContext, buf);
        }

      }
      else if (strcmp(pStrInfo_common_AclRangeStr, argv[index+relIndex]) == 0)
      {

        if (totalArg == relIndex)
        {
          macAclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;
        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &startSecondaryVlanVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartVlanId);
        }

        /* verify if the specified value is in between 1 to 4094 */
        if ((startSecondaryVlanVal < L7_ACL_MIN_VLAN_ID) ||
            (startSecondaryVlanVal > L7_ACL_MAX_VLAN_ID))
        {
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_IncorrectStartVlanVal, L7_ACL_MIN_VLAN_ID, L7_ACL_MAX_VLAN_ID);
          return cliSyntaxReturnPrompt (ewsContext, buf);
        }
        if (totalArg == relIndex)
        {
          macAclSyntaxWrite(ewsContext);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        relIndex++;

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &endSecondaryVlanVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndVlanId);
        }

        /* verify if the specified value is in between 1 to 4094 */
        if ((endSecondaryVlanVal < L7_ACL_MIN_VLAN_ID) ||
            (endSecondaryVlanVal > L7_ACL_MAX_VLAN_ID))
        {
          sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_IncorrectEndVlanVal, L7_ACL_MIN_VLAN_ID, L7_ACL_MAX_VLAN_ID);
          return cliSyntaxReturnPrompt (ewsContext, buf);
        }

        if (endSecondaryVlanVal < startSecondaryVlanVal)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndVlanIdMustBeGreaterThanStartVlanId);
        }
      }
      matchOther = L7_TRUE;
    }

    /* secondary-COS */
    else if (strcmp(pStrInfo_qos_MacAclSecondaryCos, argv[index+relIndex]) == 0)
    {

      if (totalArg == relIndex)
      {
        macAclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      relIndex++;

      if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &secondaryCosVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosId);
      }
      /* verify if the specified value is in between 1 to 4094 */
      if ((secondaryCosVal < L7_ACL_MIN_COS) ||
          (secondaryCosVal > L7_ACL_MAX_COS))
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf,pStrErr_qos_IncorrectVlanVal, L7_ACL_MIN_COS, L7_ACL_MAX_COS);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      matchOther = L7_TRUE;
    }

    /* log */
    else if (strcmp(pStrInfo_qos_AclLogsStr, argv[index+relIndex]) == 0)
    {
      /* check specific log feature per the rule action type */
      if (actionType == L7_ACL_DENY)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_LOG_DENY_FEATURE_ID) != L7_TRUE )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclLogDenyNotSupported);
        }
      }
      else
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_LOG_PERMIT_FEATURE_ID) != L7_TRUE )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclLogPermitNotSupported);
        }
      }
      logSpecified = L7_TRUE;
    }

    /* assign-queue */
    else if (strcmp(pStrInfo_qos_AclAsSignQueueStr, argv[index+relIndex]) == 0)
    {

      if (totalArg == relIndex)
      {
        macAclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      relIndex++;
      if (cliConvertTo32BitUnsignedInteger(argv[index+relIndex], &assignQueueVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidAsSignQueue);
      }
      /* verify if the specified value is in between 1 to 4094 */
      if ((assignQueueVal < L7_QOS_COS_QUEUE_ID_MIN) || (assignQueueVal > L7_QOS_COS_QUEUE_ID_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_IncorrectQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
      }
    }

    /* mirror */
    else if (strcmp(pStrInfo_qos_AclMirrorStr, argv[index+relIndex]) == 0)
    {

      if (totalArg == relIndex)
      {
        macAclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      /* mirror and redirect rule attributes are mutually-exclusive */
      if (mirrorOrRedirectSpecified != L7_FALSE)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_CfgureAclDenyLog);
      }
      mirrorOrRedirectSpecified = L7_TRUE;
      relIndex++;
      if (cliIsStackingSupported() == L7_TRUE)
      {
        rc = cliValidSpecificUSPCheck(argv[index+relIndex], &unit, &slot, &port);
        if (rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 1, pStrErr_common_Error,ewsContext, pStrErr_common_InvalidIntfNonSpecific, cliSyntaxInterfaceHelp());
        }
        else if (rc != L7_SUCCESS || usmDbIntIfNumFromUSPGet(unit, slot, port, &mirrorVal) != L7_SUCCESS)
        {
          memset(buf, 0, (L7_int32)sizeof(buf));
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
        }
      }
      else
      {
        unit = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[index+relIndex], &slot, &port, &mirrorVal) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
    }

    /* redirect */
    else if (strcmp(pStrInfo_qos_AclRedirectsStr, argv[index+relIndex]) == 0)
    {

      if (totalArg == relIndex)
      {
        macAclSyntaxWrite(ewsContext);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      /* mirror and redirect rule attributes are mutually-exclusive */
      if (mirrorOrRedirectSpecified != L7_FALSE)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_CfgureAclDenyLog);
      }
      mirrorOrRedirectSpecified = L7_TRUE;
      relIndex++;
      if (cliIsStackingSupported() == L7_TRUE)
      {
        rc = cliValidSpecificUSPCheck(argv[index+relIndex], &unit, &slot, &port);
        if (rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 1, pStrErr_common_Error,ewsContext, pStrErr_common_InvalidIntfNonSpecific, cliSyntaxInterfaceHelp());
        }
        else if (rc != L7_SUCCESS || usmDbIntIfNumFromUSPGet(unit, slot, port, &redirectVal) != L7_SUCCESS)
        {
          memset(buf, 0, (L7_int32)sizeof(buf));
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
        }
      }
      else
      {
        unit = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[index+relIndex], &slot, &port, &redirectVal) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
    }
    else
    {
      macAclSyntaxWrite(ewsContext);
      cliSyntaxBottom(ewsContext);
      return NULL;
    }
    relIndex++;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    strcpy( macAccessListName, EWSACCESSLISTNAME(ewsContext));

    if (usmDbQosAclMacNameToIndex(unit, macAccessListName, &aclId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
    }

    /* Get the new rule number*/
    if (usmDbQosAclMacRuleGetFirst(unit, aclId, &aclRuleNum)!= L7_SUCCESS)
    {
      aclRuleNum = L7_ACL_MIN_RULE_NUM;
    }
    else
    {
      while (usmDbQosAclMacRuleGetNext(unit, aclId, aclRuleNum, &aclruleNext)== L7_SUCCESS)
      {
        aclRuleNum = aclruleNext;
      }
      aclRuleNum++;
    }
    if (aclRuleNum >L7_ACL_MAX_RULE_NUM)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_RuleError);          /* Invalid rule number <select rule number between 1 to 10> */

      /* it's a warning */
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    rc = usmDbQosAclMacRuleActionAdd(unit, aclId, aclRuleNum,actionType );
    if (rc == L7_ERROR)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_MacAclDoesntExist);
    }
    else if (rc == L7_TABLE_IS_FULL)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, "Maximum number of ACL rules already configured.");
    }
    else if (rc != L7_SUCCESS)
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, buf, pStrErr_qos_ProcessingAcl);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    /* Match Every */
    if (matchOther == L7_FALSE && matchSrc == L7_TRUE && matchDst == L7_TRUE)
    {
      rc = usmDbQosAclMacRuleEveryAdd(unit, aclId, aclRuleNum, matchSrc);
      if (rc == L7_FAILURE)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPrompt (ewsContext,pStrInfo_qos_AddingSrcMacAddrAndMaskFailed);
      }

      if (rc == L7_ERROR)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_MacAclDoesntExist);
      }

      if (rc == L7_REQUEST_DENIED)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    else
    {
      if (matchSrc == L7_FALSE)
      {
        rc = usmDbQosAclMacRuleSrcMacAdd(unit, aclId, aclRuleNum, srcMacAddr, srcMacMask);

        if (rc == L7_FAILURE)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPrompt (ewsContext,pStrInfo_qos_AddingSrcMacAddrAndMaskFailed);
        }
        else if (rc == L7_REQUEST_DENIED)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
      }

      if (matchDst == L7_FALSE)
      /*  && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE) */
      {

        rc = usmDbQosAclMacRuleDstMacAdd(unit, aclId, aclRuleNum, dstMacAddr, dstMacMask);

        if (rc == L7_FAILURE)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPrompt (ewsContext,pStrInfo_qos_AddingDstMacAddrAndMaskFailed);
        }
        else if (rc == L7_REQUEST_DENIED)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
      }

      /* EtherType */
      if (eTypeKeyVal > -1 && eTypeKeyVal != 0)
      {
        rc = usmDbQosAclMacRuleEtypeKeyAdd(unit, aclId, aclRuleNum, eTypeKeyVal, eTypeCustVal);
        if (rc != L7_SUCCESS)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
      }

      /* VLAN Value */
      if (vlanVal > -1)
      {
        rc = usmDbQosAclMacRuleVlanIdAdd(unit, aclId, aclRuleNum, vlanVal);
      }

      /* VLAN Range*/
      else if (startVlanVal > -1)
      {
        rc = usmDbQosAclMacRuleVlanIdRangeAdd(unit, aclId, aclRuleNum, startVlanVal, endVlanVal);
      }
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }

      /* COS */
      if (cosVal > -1)
      {
        rc = usmDbQosAclMacRuleCosAdd(unit, aclId, aclRuleNum, cosVal);
        if (rc != L7_SUCCESS)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
      }

      /* Secondary-VLAN Value*/
      if (secondaryVlanVal > -1)
      {
        rc = usmDbQosAclMacRuleVlanId2Add(unit, aclId, aclRuleNum, secondaryVlanVal);
      }

      /* Secondary-VLAN Range*/
      else if (startSecondaryVlanVal > -1)
      {
        rc = usmDbQosAclMacRuleVlanId2RangeAdd(unit, aclId, aclRuleNum, startSecondaryVlanVal, endSecondaryVlanVal);
      }
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }

      /* Secondary-COS */
      if (secondaryCosVal > -1)
      {
        rc = usmDbQosAclMacRuleCos2Add(unit, aclId, aclRuleNum, secondaryCosVal);
        if (rc != L7_SUCCESS)
        {
          aclClean(isAclAdded, aclId, aclRuleNum);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
        }
      }
    }

    /* Log */
    if (logSpecified == L7_TRUE)
    {
      rc = usmDbQosAclMacRuleLoggingAdd(unit, aclId, aclRuleNum, logSpecified);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    /* Assign-Queue */
    if (assignQueueVal > -1)
    {
      rc = usmDbQosAclMacRuleAssignQueueIdAdd(unit, aclId, aclRuleNum, assignQueueVal);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    /* Mirror */
    if (mirrorVal > -1)
    {
      rc = usmDbQosAclMacRuleMirrorIntfAdd(unit, aclId, aclRuleNum, mirrorVal);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }

    /* Redirect */
    if (redirectVal > -1)
    {
      rc = usmDbQosAclMacRuleRedirectIntfAdd(unit, aclId, aclRuleNum, redirectVal);
      if (rc != L7_SUCCESS)
      {
        aclClean(isAclAdded, aclId, aclRuleNum);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_qos_ProcessingAcl);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
