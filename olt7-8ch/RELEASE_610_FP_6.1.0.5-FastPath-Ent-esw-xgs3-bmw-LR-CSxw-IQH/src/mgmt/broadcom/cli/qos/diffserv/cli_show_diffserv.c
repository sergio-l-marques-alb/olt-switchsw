/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/diffserv/cli_show_diffserv.c
 *
 * @purpose Differentiated Service show commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  07/17/2003
 *
 * @author  rakeshk
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "l7_common.h"
#include "diffserv_exports.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_util_api.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_util_diffserv_api.h"
#endif

#include "cliapi.h"

#include <datatypes.h>
#include "default_cnfgr.h"

#include "clicommands_diffserv.h"
#include "cliutil_diffserv.h"
#include "usmdb_qos_diffserv_api.h"
#include "ews.h"
#include "clicommands_card.h"

/* GLOBALS */

/*********************************************************************
*
* @purpose   Displays DiffServ General Information
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
* @cmdsyntax  #> show diffserv
*
*
* @cmdhelp
*
* @cmddescript  Displays general status information for Diffserv.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDiffserv(EwsContext ewsContext,
                                    uintf argc,
                                    const L7_char8 * * argv,
                                    uintf index)
{
  L7_uint32 max = 0;
  L7_uint32 current = 0;
  L7_uint32 unit;

  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_ShowDiffserv_1);
  }

  if (usmDbDiffServGenAdminModeGet(unit, &current) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_qos_DiffservAdminMode);
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(current,pStrInfo_common_NotApplicable));
  }

  if (usmDbDiffServGenClassTableSizeGet(unit, &current) == L7_SUCCESS)
  {
    if (usmDbDiffServGenClassTableMaxGet(unit, &max) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_ClassTblSizeCurrentMax);
      ewsTelnetPrintf (ewsContext, "%u / %u", current, max);
    }
  }

  if (usmDbDiffServGenClassRuleTableSizeGet(unit, &current) == L7_SUCCESS)
  {
    if (usmDbDiffServGenClassRuleTableMaxGet(unit, &max) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_ClassRuleTblSizeCurrentMax);
      ewsTelnetPrintf (ewsContext, "%u / %u", current, max);
    }
  }

  if (usmDbDiffServGenPolicyTableSizeGet(unit, &current) == L7_SUCCESS)
  {
    if (usmDbDiffServGenPolicyTableMaxGet(unit, &max) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_PolicyTblSizeCurrentMax);
      ewsTelnetPrintf (ewsContext, "%u / %u", current, max);
    }
  }

  if (usmDbDiffServGenPolicyInstTableSizeGet(unit, &current) == L7_SUCCESS)
  {
    if (usmDbDiffServGenPolicyInstTableMaxGet(unit, &max) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_PolicyInstTblSizeCurrentMax);
      ewsTelnetPrintf (ewsContext, "%u / %u", current, max);
    }
  }

  if (usmDbDiffServGenPolicyAttrTableSizeGet(unit, &current) == L7_SUCCESS)
  {
    if (usmDbDiffServGenPolicyAttrTableMaxGet(unit, &max) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_PolicyAttrTblSizeCurrentMax);
      ewsTelnetPrintf (ewsContext, "%u / %u", current, max);
    }
  }

  if (usmDbDiffServGenServiceTableSizeGet(unit, &current) == L7_SUCCESS)
  {
    if (usmDbDiffServGenServiceTableMaxGet(unit, &max) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_ServiceTblSizeCurrentMax);
      ewsTelnetPrintf (ewsContext, "%u / %u", current, max);
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   Displays DiffServ Class Detailed Information when invoked with parameter <classname>
   Displays DiffServ Class Summary Information when invoked without a parameter
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
* @cmdsyntax  exec> show class-map [<classname>]
*
*
* @cmdhelp
*
* @cmddescript  Displays all configured class information for a
*               Diffserv class when invoked with parameter <classname>.
*               Displays summary class information when invoked without a parameter.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowClassMap(EwsContext ewsContext,
                                    uintf argc,
                                    const L7_char8 * * argv,
                                    uintf index)
{

  L7_char8 className[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 refClassIndex;
  L7_uint32 classNameSize = L7_CLI_MAX_STRING_LENGTH;
  L7_char8 buf[L7_CLI_DIFFSERV_LARGE_STRING]; /* MAX STRING LENGTH NOT LARGE ENOUGH */
  L7_BOOL aclSupported = L7_FALSE;
  L7_BOOL needBlanks = L7_FALSE;
  L7_uint32 classIndex = 0;
  L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_NONE;
  L7_uchar8 aclName[L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX+1];  /* make room for trailing '\0' */
  L7_uint32 aclNum = 0;
  L7_uint32 argClassName = 1;
  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType;
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t classProto;
  L7_uint32 unit;
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if ( ( numArg != 0) && (numArg != 1) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_ShowClassMap_1);
  }

  if (  numArg == 0 )
  {
    /* printing header info */
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_TYPE_ACL_FEATURE_ID) == L7_TRUE)
    {
      aclSupported = L7_TRUE;
      ewsTelnetWriteAddBlanks (1, 0, 32, 0, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_ClassL3);
      ewsTelnetWriteAddBlanks (1, 0, 10, 0, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_ClassNameTypeProtoAclIdOrRefClassName);
      ewsTelnetWrite(ewsContext,
                     "\r\n------------------------------- ----- ----- --------------------------------------");
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 32, 0, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_ClassL3);
      ewsTelnetWriteAddBlanks (1, 0, 10, 0, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_ClassNameTypeProtoRefClassName);
      ewsTelnetWrite(ewsContext,
                     "\r\n------------------------------- ----- ----- -------------------------------");
    }

    while (usmDbDiffServClassGetNext(unit, classIndex, &classIndex) == L7_SUCCESS)
    {
      memset (buf, 0, sizeof(buf));
      classNameSize = sizeof(className);
      memset (className, 0, classNameSize);
      if (usmDbDiffServClassNameGet(unit, classIndex, className, &classNameSize) !=
          L7_SUCCESS)
      {
        continue;
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "\r\n%-31s ", className);
      }
      if (usmDbDiffServClassTypeGet(unit,classIndex, &classType) != L7_SUCCESS)
      {
        continue;
      }

      memset (buf, 0, sizeof(buf));
      switch (classType)
      {
      case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL:
        osapiSnprintf(buf, sizeof(buf), "%-5s ", pStrInfo_common_ApProfileRadioScanFrequencyAll);
        break;
      case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY:
        osapiSnprintf(buf, sizeof(buf), "%-5s ", pStrInfo_qos_Any);
        break;
      case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL:
        osapiSnprintf(buf, sizeof(buf), "%-5s ", pStrInfo_common_Acl);      /* acl */
        break;
      default:
        osapiSnprintf (buf, sizeof(buf), "%-5s ", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext,buf);

      if (usmDbDiffServClassL3ProtoGet(unit, classIndex, &classProto) != L7_SUCCESS)
      {
        continue;
      }

      memset (buf, 0, sizeof(buf));
      switch (classProto)
      {
      case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4:
        sprintf(buf,"%-5s ", pStrInfo_common_Diffserv_4);
        break;
      case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6:
        sprintf(buf,"%-5s ", pStrInfo_common_Diffserv_5);
        break;
      default:
        sprintf (buf,"%-5s ", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext,buf);

      needBlanks = L7_TRUE;                   /* to catch all exception paths */

      if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
      {
        /* display ACL number or name for class type 'acl'
         * NOTE: mutually-exclusive of reference class, so no display conflicts
         */
        if ((usmDbDiffServClassAclTypeGet(unit, classIndex, &aclType) == L7_SUCCESS) &&
            (usmDbDiffServClassAclNumGet(unit, classIndex, &aclNum) == L7_SUCCESS))
        {
          if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP)
          {
            if (L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4 == classProto)
            {
              if (usmDbDiffServIsAclNamedIpAcl(unit, aclNum) == L7_SUCCESS)
              {
                if (usmDbDiffServAclIpNameGet(unit, aclNum, aclName) == L7_SUCCESS)
                {
                  osapiSnprintf(buf, sizeof(buf),  "%-31s (%-3s) ", aclName, pStrInfo_common_Ip);            /* long format */
                }
              }
              else
              {
                osapiSnprintf(buf, sizeof(buf),  "%-31u (%-3s) ", aclNum, pStrInfo_common_Ip);            /* long format */
              }
              needBlanks = L7_FALSE;
            }
            else if (L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6 == classProto)
            {
              if (usmDbDiffServAclIpNameGet(unit, aclNum, aclName) == L7_SUCCESS)
              {
                osapiSnprintf(buf, sizeof(buf),  "%-31s (%-3s) ", aclName, pStrInfo_common_Ip);            /* long format */
                needBlanks = L7_FALSE;
              }
            }
          }
          else if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC)
          {
            if (usmDbDiffServAclMacNameGet(unit, aclNum, aclName) == L7_SUCCESS)
            {
              osapiSnprintf(buf, sizeof(buf),  "%-31s (%-3s) ", aclName, pStrInfo_qos_Mac_1);        /* long format */
              needBlanks = L7_FALSE;
            }
          }
        }
      }
      else
      {
        /* display reference class name, if class contains a reference to another class
         * NOTE: mutually-exclusive of class type 'acl', so no display conflicts
         */
        classNameSize = sizeof(className);
        memset ( className, 0, classNameSize);
        memset ( buf, 0, sizeof(buf) );
        if (usmDbDiffServClassToRefClass(unit, classIndex, &refClassIndex) == L7_SUCCESS &&
            usmDbDiffServClassNameGet(unit, refClassIndex, className, &classNameSize) == L7_SUCCESS )
        {
          if (aclSupported == L7_TRUE)
          {
            osapiSnprintf(buf, sizeof(buf),  "%-31s (%-3s) ", className, pStrInfo_qos_RefClassAbbr);
          }                                                                                  /* long format */
          else
          {
            osapiSnprintf(buf, sizeof(buf),  "%-31s", className);
          }                                                                              /* short format*/
          needBlanks = L7_FALSE;
        }
      }

      if (needBlanks == L7_TRUE)
      {
        if (aclSupported == L7_TRUE)
        {
          osapiSnprintf (buf, sizeof(buf), "%-38s", pStrInfo_common_EmptyString);
        }                                                                                /* long format */
        else
        {
          osapiSnprintf (buf, sizeof(buf), "%-31s", pStrInfo_common_EmptyString);
        }                                                                                /* short format*/
      }
      ewsTelnetWrite(ewsContext, buf);
    }

  }
  else if (  numArg == 1 )
  {

    /* verify the length of <classname> argument */
    if (strlen(argv[index+argClassName]) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ClassNameTooLong);
    }
    OSAPI_STRNCPY_SAFE(strClassName,argv[index+ argClassName]);

    /* verify if the specified string contains all the alpha-numeric characters */
    if (cliIsAlphaNum(strClassName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_ClassNameMustBeAlnum);
    }
    /* verify if this class already exists or not */
    else if (usmDbDiffServClassNameToIndex(unit, strClassName, &classIndex) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    cliFormat(ewsContext, pStrInfo_qos_ClassName_1);
    ewsTelnetWrite(ewsContext, strClassName);

    if (usmDbDiffServClassTypeGet(unit,classIndex, &classType) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_ClassType);
      switch (classType)
      {
      case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL:
        ewsTelnetWrite(ewsContext, pStrInfo_common_ApProfileRadioScanFrequencyAll);
        break;
      case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_Any);
        break;
      case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Acl);
        if ((usmDbDiffServClassAclTypeGet(unit, classIndex, &aclType) == L7_SUCCESS) &&
            (usmDbDiffServClassAclNumGet(unit, classIndex, &aclNum) == L7_SUCCESS))
        {
          if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP)
          {
            if (usmDbDiffServClassL3ProtoGet(unit, classIndex, &classProto) == L7_SUCCESS)
            {
              if (classProto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
              {
                if (usmDbDiffServAclIpNameGet(unit, aclNum, aclName) == L7_SUCCESS)
                {
                  cliFormat(ewsContext, "Reference IP ACL Name");
                  ewsTelnetPrintf (ewsContext, aclName);
                }
                else
                {
                  cliFormat(ewsContext, pStrInfo_qos_RefIpAclNum);
                  ewsTelnetPrintf (ewsContext, "%u", aclNum);
                }
              }
              else if (classProto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
              {
                if (usmDbDiffServAclIpNameGet(unit, aclNum, aclName) == L7_SUCCESS)
                {
                  cliFormat(ewsContext, pStrInfo_qos_RefIpv6AclName);
                  ewsTelnetPrintf (ewsContext, aclName);
                }
              }
            }
          }
          else if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC)
          {
            if (usmDbDiffServAclMacNameGet(unit, aclNum, aclName) == L7_SUCCESS)
            {
              cliFormat(ewsContext, pStrInfo_qos_RefMacAclName);
              ewsTelnetPrintf (ewsContext, aclName);
            }
          }
        }

        break;
      default:
        osapiSnprintf (buf, sizeof(buf), "%-12s ", pStrInfo_common_EmptyString);
        break;
      }
    }

    if (usmDbDiffServClassL3ProtoGet(unit,classIndex, &classProto) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_Class_1);
      switch (classProto)
      {
      case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Diffserv_4);
        break;
      case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Diffserv_5);
        break;
      default:
        sprintf (buf,"%-12s ", pStrInfo_common_EmptyString);
        break;
      }
    }

    /* display Match Criteria */
    /* display rules */
    cliDisplayClassRules(ewsContext, unit, classIndex);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   Displays DiffServ Policy Detailed Information when invoked with parameter <policyname>
*                     DiffServ Policy Summary Information when invoked without any parameter
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
* @cmdsyntax  #> show policy-map [policyname]
*
*
* @cmdhelp
*
* @cmddescript  Displays all configured policy information for a
*               Diffserv policy when invoked with parameter <policyname>.
   Displays summary of policy information when invoked without any parameter.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowPolicyMap(EwsContext ewsContext,
                                     uintf argc,
                                     const L7_char8 * * argv,
                                     uintf index)
{
  L7_uint32 count = 0, argName = 1;
  static L7_uint32 policyIndex_det;
  L7_uint32 policyType_det;
  static L7_uint32 tempPolicyIndex_det, val;
  static L7_uint32 policyInstIndex_det ;
  static L7_uint32 classIndex_det  ;
  L7_uint32 classNameSize_det = L7_CLI_MAX_STRING_LENGTH;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 className_det[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf_det[L7_CLI_MAX_STRING_LENGTH];

  static L7_uint32 policyIndex_sum = 0, provPolicyIndex = 0, classDisplayBreak = L7_FALSE, tempInstance = 0;
  L7_uint32 tempPolicyIndex_sum = 0, tempPolIndex = 0, tempInstanceIndex = 0, lastPolicyEncountered = L7_FALSE;
  L7_uint32 policyInstIndex_sum = 0;
  L7_uint32 classIndex_sum = 0;
  L7_uint32 classNameSize_sum = L7_CLI_MAX_STRING_LENGTH;
  L7_uint32 policyNameSize = L7_CLI_MAX_STRING_LENGTH;
  L7_char8 policyName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 className_sum[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf_sum[L7_CLI_MAX_STRING_LENGTH];
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType_sum;
  L7_BOOL firstClass = L7_FALSE;
  static L7_uint32 lastClassEncountered = L7_FALSE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if  ((  numArg != 0) && (  numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ShowPolicyMap);
  }

  if ( numArg == 0 )
  {
    if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
    {                                                /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        policyIndex_sum = provPolicyIndex = classDisplayBreak = 0 ;
        tempInstance = lastClassEncountered = 0;
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      /* printing header info */
      ewsTelnetWriteAddBlanks (1, 0, 10, 9, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_PolicyNamePolicyTypeClassMbrs);
      ewsTelnetWrite(ewsContext,
                     "\r\n------------------------------- ----------- -------------------------------");

      count = count + 2;
      if (usmDbDiffServPolicyGetNext(unit, policyIndex_sum, &policyIndex_sum) != L7_SUCCESS)
      {
        policyIndex_sum = provPolicyIndex = classDisplayBreak = 0 ;
        tempInstance = lastClassEncountered = 0;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }

    do
    {
      memset (buf_sum, 0, sizeof(buf_sum));
      if((classDisplayBreak == L7_TRUE && lastClassEncountered == L7_TRUE) || classDisplayBreak == L7_FALSE)
      {

        firstClass = L7_FALSE;

        policyNameSize = sizeof(policyName);
        memset (policyName, 0, policyNameSize);
        if (usmDbDiffServPolicyNameGet(unit, policyIndex_sum,
                                       policyName, &policyNameSize) !=
            L7_SUCCESS)
        {
          continue;
        }
        else
        {
          ewsTelnetPrintf (ewsContext, "\r\n%-32s", policyName);
          memset (buf_sum, 0, sizeof(buf_sum));
        }
        if (usmDbDiffServPolicyTypeGet(unit,policyIndex_sum, &policyType_sum) != L7_SUCCESS)
        {
          continue;
        }

        switch (policyType_sum)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
          osapiSnprintf(buf_sum, sizeof(buf_sum), "%-11s ", pStrInfo_common_In);
          break;
        case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
          osapiSnprintf(buf_sum, sizeof(buf_sum), "%-11s ", pStrInfo_qos_Out);
          break;
        default:
          osapiSnprintf (buf_sum, sizeof(buf_sum), "%-11s ", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext,buf_sum);

        memset (buf_sum, 0, sizeof(buf_sum));
        tempPolicyIndex_sum = policyIndex_sum;
        policyInstIndex_sum = 0;
        if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex_sum, policyInstIndex_sum,
                                           &tempPolicyIndex_sum, &policyInstIndex_sum) != L7_SUCCESS)
        {
          continue;
        }
      }
      else
      {
        tempPolicyIndex_sum = provPolicyIndex;
        policyInstIndex_sum = tempInstance;
        firstClass = L7_TRUE;
      }

      while (tempPolicyIndex_sum == policyIndex_sum)
      {
        if (usmDbDiffServPolicyInstClassIndexGet(unit, policyIndex_sum, policyInstIndex_sum,
                                                 &classIndex_sum) == L7_SUCCESS)

        {
          classNameSize_sum = sizeof(className_sum);
          memset (className_sum, 0, classNameSize_sum);
          if (usmDbDiffServClassNameGet(unit, classIndex_sum,
                                        className_sum, &classNameSize_sum) == L7_SUCCESS)
          {
            memset (buf_sum, 0, sizeof(buf_sum));
            if (firstClass == L7_FALSE)
            {
              osapiSnprintf(buf_sum, sizeof(buf_sum), "%-31s ", className_sum);
            }
            else
            {
              osapiSnprintf (buf_sum, sizeof(buf_sum),  "\r\n                                            %-31s",
                             className_sum);
            }
            ewsTelnetWrite(ewsContext,buf_sum);
            count++;
            firstClass = L7_TRUE;
          }
        }
        if(count >= CLI_MAX_SCROLL_LINES - 6)
        {
          classDisplayBreak = L7_TRUE;
          provPolicyIndex = tempPolicyIndex_sum;
          tempPolIndex = tempPolicyIndex_sum;
          tempInstanceIndex = policyInstIndex_sum;
          if(usmDbDiffServPolicyInstGetNext(unit,tempPolIndex,tempInstanceIndex,
                                            &tempPolIndex, &tempInstanceIndex) == L7_SUCCESS)
          {
            if (provPolicyIndex != tempPolIndex)
            {
              provPolicyIndex = tempPolIndex;
              lastClassEncountered = L7_TRUE;
            }
            else
            {
              lastClassEncountered = L7_FALSE;
            }

            tempInstance = tempInstanceIndex;
          }
          else
          {
            lastPolicyEncountered = L7_TRUE;
          }

          break;
        }
        else
        {
          classDisplayBreak = L7_FALSE;
        }

        if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex_sum, policyInstIndex_sum,
                                           &tempPolicyIndex_sum, &policyInstIndex_sum) != L7_SUCCESS)
        {
          break;
        }
      }

      if (classDisplayBreak)
      {
        break;
      }

    } while (usmDbDiffServPolicyGetNext(unit, policyIndex_sum, &policyIndex_sum) == L7_SUCCESS);

    if(!lastPolicyEncountered)
    {
      tempPolIndex = policyIndex_sum;

      if(usmDbDiffServPolicyGetNext(unit, tempPolIndex, &tempPolIndex) == L7_SUCCESS)
      {
        if (lastClassEncountered)
        {
          policyIndex_sum = tempPolIndex;
        }

        cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
        cliSyntaxBottom(ewsContext);
        cliAlternateCommandSet(pStrInfo_qos_ShowPolicyMap_1);
        return pStrInfo_common_Name_2;
      }
      else if (classDisplayBreak)
      {
        cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
        cliSyntaxBottom(ewsContext);
        cliAlternateCommandSet(pStrInfo_qos_ShowPolicyMap_1);
        return pStrInfo_common_Name_2;
      }
    }

    policyIndex_sum = provPolicyIndex = classDisplayBreak = 0 ;
    tempInstance = lastClassEncountered = 0;
  }
  else if (  numArg == 1 )
  {
    if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
    {                                                                       /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      policyIndex_det = 0;
      /* verify the length of <policyname> argument */
      if (strlen(argv[index+ argName]) > L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_PolicyNameLen);
      }
      OSAPI_STRNCPY_SAFE(strName,argv[index+ argName]);

      /* verify if the specified string contains all the alpha-numeric characters */
      if (cliIsAlphaNum(strName) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_PolicyNameAlnum);
      }
      /* verify that this policy already exists */
      else if (usmDbDiffServPolicyNameToIndex(unit, strName, &policyIndex_det) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservPolicyDoesntExist);
      }

      cliFormat(ewsContext, pStrInfo_qos_PolicyName_1);
      ewsTelnetPrintf (ewsContext, "%-32s", strName);

      classIndex_det = 0;
      policyInstIndex_det = 0;
      tempPolicyIndex_det = 0;

      if (usmDbDiffServPolicyTypeGet(unit,policyIndex_det, &policyType_det) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_CantDispTypeOfDiffservPolicy);
      }
      cliFormat(ewsContext, pStrInfo_qos_PolicyType_1);
      memset (buf_det, 0, sizeof(buf_det));
      switch (policyType_det)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        osapiSnprintf(buf_det, sizeof(buf_det), "%-11s ", pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        osapiSnprintf(buf_det, sizeof(buf_det), "%-11s ", pStrInfo_qos_Out);
        break;
      default:
        osapiSnprintf (buf_det, sizeof(buf_det), "%-11s ", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext,buf_det);

      /* get the Class information */
      memset (buf_det, 0, sizeof(buf_det));
      tempPolicyIndex_det = policyIndex_det;
      policyInstIndex_det = 0;
      if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex_det, policyInstIndex_det,
                                         &tempPolicyIndex_det, &policyInstIndex_det) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
      val = tempPolicyIndex_det;
    }

    if(tempPolicyIndex_det == val)
    {
      for (count=0; count < 1; count++)
      {
        if (tempPolicyIndex_det == policyIndex_det)
        {
          if (usmDbDiffServPolicyInstClassIndexGet(unit, policyIndex_det, policyInstIndex_det,
                                                   &classIndex_det) == L7_SUCCESS)

          {
            classNameSize_det = sizeof(className_det);
            memset (className_det, 0, classNameSize_det);
            cliSyntaxNewLine(ewsContext);
            if (usmDbDiffServClassNameGet(unit, classIndex_det,
                                          className_det, &classNameSize_det) == L7_SUCCESS)
            {
              cliFormat(ewsContext, pStrInfo_qos_ClassName_1);
              ewsTelnetPrintf (ewsContext, "%-31s ", className_det);
            }
            cliDisplayPolicyClassAttributes(ewsContext, unit, policyIndex_det,
                                            policyInstIndex_det);
          }
        }
      }

      if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex_det, policyInstIndex_det,
                                         &tempPolicyIndex_det, &policyInstIndex_det) != L7_SUCCESS)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      else
      {
        if(tempPolicyIndex_det == val)
        {
          osapiSnprintf(buf_det, sizeof(buf_det),  "%s %s", pStrInfo_qos_ShowPolicyMap_1, argv[index+ argName]);
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          cliSyntaxBottom(ewsContext);
          cliAlternateCommandSet(buf_det);
          return pStrInfo_common_Name_2;     /* --More-- or (q)uit */
        }
      }
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   Displays DiffServ Service Detailed Information
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
* @cmdsyntax  #> show diffserv service <slot/port> {in | out}
*
*
* @cmdhelp
*
* @cmddescript  Displays all configured service information for a
*               specified Diffserv interface.
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowDiffservService(EwsContext ewsContext,
                                           uintf argc,
                                           const L7_char8 * * argv,
                                           uintf index)
{
  L7_uint32 argInterface = 1;
  L7_uint32 argDirection = 2;
  L7_uint32 intIfIndex = 0;
  L7_uint32 ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
  L7_uint32 ifOperStatus = L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_NONE;
  static L7_uint32 policyIndex = 0, temp;
  static L7_uint32 classIndex = 0;
  static L7_uint32 tempPolicyIndex = 0;
  static L7_uint32 policyInstIndex = 0;
  L7_uint32 policyNameSize = 0;
  L7_uint32 classNameSize = 0;
  L7_uint32 slot = 0, port = 0;
  L7_uint32 val = 0, count=0;
  L7_char8 strDirection[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strInterface[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 policyName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 className[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  static L7_BOOL inSupported = L7_FALSE;
  static L7_BOOL outSupported = L7_FALSE;
  static L7_BOOL firstPass = L7_TRUE;
  static L7_char8 dirOptions[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 unit;

  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (firstPass == L7_TRUE)
  {
    memset ( dirOptions, 0, sizeof(dirOptions));
    cliDiffservBuildValidDirs( dirOptions, sizeof(dirOptions), "{", "}",  &inSupported, &outSupported);
    firstPass = L7_FALSE;
  }

  if (inSupported == L7_FALSE && outSupported == L7_FALSE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_DiffservServicesNotSupported);
  }

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    memset ( buf, 0, sizeof(buf));
    if (numArg == 0)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf),  pStrErr_qos_ShowDiffservServiceAll, cliSyntaxInterfaceHelp());
      ewsTelnetWrite(ewsContext, buf);
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf),  pStrErr_qos_ShowDiffservService_1, cliSyntaxInterfaceHelp(), dirOptions);
      ewsTelnetWrite( ewsContext, buf);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
  {                                                                     /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }

  }
  else
  {
    temp = tempPolicyIndex;
    policyIndex = 0;
    /* second argument validation */
    if (strlen(argv[index+argDirection]) > 3 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowDiffservService_1, cliSyntaxInterfaceHelp(), dirOptions);
    }

    OSAPI_STRNCPY_SAFE(strDirection,argv[index+ argDirection]);

    cliConvertToLowerCase(strDirection);

    if ( strcmp(strDirection, pStrInfo_common_AclInStr) == 0 && inSupported == L7_TRUE)
    {
      ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
    }
    else if (strcmp(strDirection, pStrInfo_common_AclOutStr) == 0 && outSupported == L7_TRUE)
    {
      ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowDiffservService_1, cliSyntaxInterfaceHelp(), dirOptions);
    }

    /* first argument validation */
    if (strlen(argv[index+argInterface]) > sizeof(strInterface) )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_qos_IntfMustUse, cliSyntaxInterfaceHelp());
    }

    OSAPI_STRNCPY_SAFE(strInterface,argv[index+ argInterface]);

    cliConvertToLowerCase(strInterface);

    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(strInterface, &unit, &slot, &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfIndex) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      unit = cliGetUnitId();

      if (cliSlotPortToIntNum(ewsContext, strInterface, &slot, &port, &intIfIndex) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }

    /* display Diffserv Mode */
    if (usmDbDiffServGenAdminModeGet(unit, &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_DiffservAdminMode);
      ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val,pStrInfo_common_NotApplicable));
    }

    classIndex = 0;
    policyInstIndex = 0;
    tempPolicyIndex = 0;

    /* Display input interface and direction */
    cliFormat(ewsContext, pStrInfo_common_Intf);
    ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(unit, slot, port));

    cliFormat(ewsContext, pStrInfo_common_Direction);
    if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_In);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_qos_Out);
    }

    /* Display Operational Status */
    if (usmDbDiffServServiceIfOperStatusGet(unit, intIfIndex,
                                            ifDirection, &ifOperStatus) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_WsOperatingStatus);
      if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Up);
      }                                          /* Up */
      else if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_DOWN)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Down_1);
      }                                              /* Down */
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }                                            /* N/A */
    }

    /* Display policy */
    if ((usmDbDiffServServicePolicyIndexGet(unit, intIfIndex,
                                            ifDirection,
                                            &policyIndex) != L7_SUCCESS) ||
        (usmDbDiffServPolicyGet(unit, policyIndex) != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoPolicyIsAttachedToIntfInDirection);
    }

    policyNameSize = sizeof(policyName);
    memset (policyName, 0, policyNameSize);
    if (usmDbDiffServPolicyNameGet(unit, policyIndex,
                                   policyName, &policyNameSize) ==
        L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_PolicyName_1);
      ewsTelnetWrite(ewsContext, policyName);
    }

    /* get the Class information for the Policy */
    memset (buf, 0, sizeof(buf));
    tempPolicyIndex = policyIndex;
    policyInstIndex = 0;
    if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex, policyInstIndex,
                                       &tempPolicyIndex, &policyInstIndex) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    temp = tempPolicyIndex;
  }

  for(count = 0; count < 1; count++)
  {
    if (tempPolicyIndex == policyIndex)
    {
      if (usmDbDiffServPolicyInstClassIndexGet(unit, policyIndex, policyInstIndex,
                                               &classIndex) == L7_SUCCESS)

      {
        classNameSize = sizeof(className);
        memset (className, 0, classNameSize);
        cliSyntaxNewLine(ewsContext);
        if (usmDbDiffServClassNameGet(unit, classIndex,
                                      className, &classNameSize) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_ClassName_1);
          ewsTelnetPrintf (ewsContext, "%-31s ", className);
        }
        cliDisplayPolicyClassAttributes(ewsContext, unit, policyIndex,
                                        policyInstIndex);
      }
    }
  }

  if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex, policyInstIndex,
                                     &tempPolicyIndex, &policyInstIndex) != L7_SUCCESS)
  {
    ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    if(tempPolicyIndex == temp)
    {
      osapiSnprintf(buf, sizeof(buf),  "%s %s %s", pStrInfo_qos_ShowDiffservService_2,
                    argv[index+argInterface], argv[index+argDirection]);
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliSyntaxBottom(ewsContext);
      cliAlternateCommandSet(buf);
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   Displays DiffServ Service Summary Information
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
* @cmdsyntax  #> show diffserv service brief [in | out]
*
*
* @cmdhelp
*
* @cmddescript  Displays summary service information for
*               Diffserv interfaces.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDiffservServiceBrief(EwsContext ewsContext,
                                                uintf argc,
                                                const L7_char8 * * argv,
                                                uintf index)
{
  L7_uint32 count, val = 0;
  static L7_uint32 direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
  L7_uint32 argDirection = 1;
  static L7_uint32 intIfIndex = 0;
  static L7_uint32 ifDirection = 0;
  L7_uint32 ifOperStatus = L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_NONE;
  L7_uint32 policyIndex = 0;
  L7_uint32 policyNameSize = L7_CLI_MAX_STRING_LENGTH;
  L7_uint32 unitNum = 0, slot = 0, port = 0;
  L7_char8 strDirection[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 policyName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 ifOperStatusString[5];
  L7_BOOL emptyList = L7_FALSE;
  static L7_BOOL inSupported = L7_FALSE;
  static L7_BOOL outSupported = L7_FALSE;
  static L7_BOOL firstPass = L7_TRUE;
  static L7_char8 dirOptions[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 unit;

  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (firstPass == L7_TRUE)
  {
    memset ( dirOptions, 0, sizeof(dirOptions));
    cliDiffservBuildValidDirs( dirOptions, sizeof(dirOptions), "[", "]",  &inSupported, &outSupported);
    firstPass = L7_FALSE;
  }

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (  numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowDiffservServiceBrief_1, dirOptions);
  }

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
  {                                                                     /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (inSupported == L7_FALSE && outSupported == L7_FALSE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_DiffservServicesNotSupported);
    }

    if (cliNumFunctionArgsGet() == 1)
    {
      if (strlen(argv[index+argDirection]) > 3 )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowDiffservServiceBrief_1, dirOptions);
      }
      OSAPI_STRNCPY_SAFE(strDirection,argv[index+ argDirection]);

      cliConvertToLowerCase(strDirection);

      if ( strcmp(strDirection, pStrInfo_common_AclInStr) == 0 && inSupported == L7_TRUE)
      {
        direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
      }
      else if (strcmp(strDirection, pStrInfo_common_AclOutStr) == 0 && outSupported == L7_TRUE)
      {
        direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowDiffservServiceBrief_1, dirOptions);
      }
    }
    else
    {
      direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
    }

    intIfIndex = 0;
    ifDirection = 0;

    if (usmDbDiffServServiceGetNext(unit, intIfIndex,
                                    ifDirection,&intIfIndex, &ifDirection) != L7_SUCCESS )/* get the first entry after the zero parameter */
    {
      emptyList = L7_TRUE;
    }
    else
    {
      if (usmDbDiffServGenAdminModeGet(unit, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_qos_DiffservAdminMode);
        ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val,pStrInfo_common_NotApplicable));
      }
    }

  } /* end else input empty */

  /* printing header info */
  ewsTelnetWriteAddBlanks (2, 0, 1, 11, L7_NULLPTR, ewsContext,
                           pStrInfo_qos_IntfDirectionOperstatusPolicyName);
  ewsTelnetWrite(ewsContext,
                 "\r\n----------- ----------- ---------- -------------------------------");
  if (emptyList)
  {
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    return cliPrompt(ewsContext);
  }

  count = 0;
  do
  {
    if ((direction == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE) ||
        (direction == ifDirection ))
    {
      if (usmDbDiffServServicePolicyIndexGet(unit, intIfIndex, ifDirection,
                                             &policyIndex) != L7_SUCCESS)
      {
        continue;
      }

      /* verify that policy exists */
      if (usmDbDiffServPolicyGet(unit, policyIndex) != L7_SUCCESS)
      {
        continue;
      }

      if (usmDbUnitSlotPortGet(intIfIndex, &unitNum, &slot, &port) != L7_SUCCESS)
      {
        continue;
      }

      memset ( ifOperStatusString, 0, sizeof(ifOperStatusString) );
      if (usmDbDiffServServiceIfOperStatusGet(unit, intIfIndex,
                                              ifDirection, &ifOperStatus) == L7_SUCCESS)
      {
        if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP)
        {
          osapiSnprintf( ifOperStatusString, sizeof(ifOperStatusString), pStrInfo_common_Up);
        }                                                                                /* Up  */
        else if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_DOWN)
        {
          osapiSnprintf( ifOperStatusString, sizeof(ifOperStatusString), pStrInfo_common_Down_1);
        }                                                                                    /* Down*/
        else
        {
          osapiSnprintf( ifOperStatusString, sizeof(ifOperStatusString), pStrInfo_common_NotApplicable);
        }                                                                                  /* N/A */
      }

      policyNameSize = sizeof(policyName);
      memset (policyName, 0, policyNameSize);
      if (usmDbDiffServPolicyNameGet(unit, policyIndex,
                                     policyName, &policyNameSize) !=
          L7_SUCCESS)
      {
        continue;
      }

      if (count >= 15)
      {
        memset ( buf, 0, sizeof(buf));
        if (numArg == 1)
        {
          osapiSnprintf( buf, sizeof(buf),  "%s %s", pStrInfo_qos_ShowDiffservServiceBrief_2, argv[index+argDirection]);
        }
        else
        {
          osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR,  buf, sizeof(buf), pStrInfo_qos_ShowDiffservServiceBrief_2);
        }
        /* before displaying this entry, handle --more-- processing */
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
        cliAlternateCommandSet(buf);
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }

      memset (buf, 0, sizeof(buf));
      if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
      {
        osapiSnprintf(buf, sizeof(buf),  "\r\n%-11s %-11s %-11s %-32s", cliDisplayInterfaceHelp(unitNum, slot, port),
                      pStrInfo_common_In,
                      ifOperStatusString,
                      policyName);
      }
      else if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT)
      {
        osapiSnprintf(buf, sizeof(buf),  "\r\n%-11s %-11s %-11s %-32s", cliDisplayInterfaceHelp(unitNum, slot, port),
                      pStrInfo_qos_Out,
                      ifOperStatusString,
                      policyName);
      }

      ewsTelnetWrite(ewsContext, buf);
      count++;
    }

  } while (usmDbDiffServServiceGetNext(unit, intIfIndex, ifDirection,
                                       &intIfIndex, &ifDirection) == L7_SUCCESS);

  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   Displays DiffServ Service Stats Detailed Information
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
* @cmdsyntax  #> show policy map interface <slot/port> { in | out }
*
*
* @cmdhelp
*
* @cmddescript  Displays detailed statistical information for a
*               Diffserv interface.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowPolicyMapInterface(EwsContext ewsContext,
                                              uintf argc,
                                              const L7_char8 * * argv,
                                              uintf index)
{
  L7_uint32 argInterface = 1;
  L7_uint32 argDirection = 2;
  L7_uint32 intIfIndex = 0;
  L7_uint32 ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
  L7_uint32 policyIndex = 0;
  L7_uint32 classIndex = 0;
  L7_uint32 tempPolicyIndex = 0;
  L7_uint32 policyInstIndex = 0;
  L7_uint32 policyNameSize = 0;
  L7_uint32 classNameSize = 0;
  L7_uint32 packetsHi = 0;
  L7_uint32 packetsLo = 0;
  L7_uint32 octetsHi = 0;
  L7_uint32 octetsLo = 0;
  L7_uint32 slot = 0, port = 0;
  L7_char8 strDirection[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strInterface[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 policyName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 className[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 packetsbuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 octetsbuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ifOperStatus = L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_NONE;
  L7_uint32 offeredOctetsSupported, offeredPacketsSupported;
  L7_uint32 discardedOctetsSupported, discardedPacketsSupported;
  L7_uint32 unit;

  L7_uint32 numArg;

  static L7_BOOL inSupported = L7_FALSE;
  static L7_BOOL outSupported = L7_FALSE;
  static L7_char8 dirOptions[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  memset ( dirOptions, 0, sizeof(dirOptions));
  cliDiffservBuildValidDirs( dirOptions, sizeof(dirOptions), "{", "}", &inSupported, &outSupported);

  if (inSupported == L7_FALSE && outSupported == L7_FALSE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_DiffservServicesNotSupported);
  }

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (  numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowPolicyMapIntf_1, cliSyntaxInterfaceHelp(), dirOptions);
  }
  else
  {
    if (strlen( argv[index+argDirection] ) > 3 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowPolicyMapIntf_1, cliSyntaxInterfaceHelp(), dirOptions);
    }
    OSAPI_STRNCPY_SAFE(strDirection,argv[index+ argDirection]);

    cliConvertToLowerCase(strDirection);

    if ( strcmp(strDirection, pStrInfo_common_AclInStr) == 0 && inSupported == L7_TRUE)
    {
      ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
    }
    else if (strcmp(strDirection, pStrInfo_common_AclOutStr) == 0 && outSupported == L7_TRUE)
    {
      ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowPolicyMapIntf_1, cliSyntaxInterfaceHelp(), dirOptions);
    }
  }

  /* first argument validation */
  if (strlen(argv[index+argInterface]) >= sizeof(strInterface))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_qos_IntfMustUse, cliSyntaxInterfaceHelp());
  }
  OSAPI_STRNCPY_SAFE(strInterface,argv[index+ argInterface]);

  cliConvertToLowerCase(strInterface);

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(strInterface, &unit, &slot, &port) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfIndex) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();

    if (cliSlotPortToIntNum(ewsContext, strInterface, &slot, &port, &intIfIndex) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  cliFormat(ewsContext, pStrInfo_common_Intf);
  ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);

  if ((ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN ||
       ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE) &&
      inSupported == L7_TRUE )
  {
    cliFormat(ewsContext, pStrInfo_common_Direction);
    ewsTelnetWrite(ewsContext, pStrInfo_common_In);

    /* Display Operational Status */
    if (usmDbDiffServServiceIfOperStatusGet(unit, intIfIndex,
                                            ifDirection, &ifOperStatus) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_WsOperatingStatus);
      if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Up);
      }                                      /* Up */
      else if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_DOWN)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Down_1);
      }                                          /* Down */
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }                                        /* N/A */
    }

    /* Display Policy */
    if ((usmDbDiffServServicePolicyIndexGet(unit, intIfIndex,
                                            L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN,
                                            &policyIndex) != L7_SUCCESS) ||
        (usmDbDiffServPolicyGet(unit, policyIndex) != L7_SUCCESS))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoInBoundPolicyIsAttachedToIntf);
      cliSyntaxBottom(ewsContext);
    }
    else
    {
      policyNameSize = sizeof(policyName);
      memset (policyName, 0, policyNameSize);
      if (usmDbDiffServPolicyNameGet(unit, policyIndex,
                                     policyName, &policyNameSize) ==
          L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_qos_PolicyName_1);
        ewsTelnetWrite(ewsContext, policyName);
      }

      /* Interface Statistics */

      ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_IntfSummary);

      cliDiffservDisplayServiceStats(ewsContext, unit, intIfIndex,
                                     L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN);

      /* Class Statistics */
      memset (buf, 0, sizeof(buf));
      tempPolicyIndex = policyIndex;
      policyInstIndex = 0;
      if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex, policyInstIndex,
                                         &tempPolicyIndex, &policyInstIndex) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoClassesAreAttachedToPolicy);
        tempPolicyIndex++;   /* force tempPoliciyIndex != policyIndex */
      }

      while (tempPolicyIndex == policyIndex)
      {
        if (usmDbDiffServPolicyInstClassIndexGet(unit, policyIndex, policyInstIndex,
                                                 &classIndex) == L7_SUCCESS)

        {
          offeredPacketsSupported = L7_FALSE;
          offeredOctetsSupported = L7_FALSE;
          discardedPacketsSupported = L7_FALSE;
          discardedOctetsSupported = L7_FALSE;

          cliSyntaxNewLine(ewsContext);
          classNameSize = sizeof(className);
          memset (className, 0, classNameSize);
          if (usmDbDiffServClassNameGet(unit, classIndex,
                                        className, &classNameSize) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_ClassName_1);
            ewsTelnetWrite(ewsContext, className);
          }

          /* in offered octets/packets */
          if (usmDbDiffServPolicyPerfInOfferedOctetsGet(unit,
                                                        policyIndex,
                                                        policyInstIndex,
                                                        intIfIndex,
                                                        &octetsHi,
                                                        &octetsLo) == L7_SUCCESS)
          {
            offeredOctetsSupported = L7_TRUE;
            strUtil64toa (octetsHi, octetsLo, octetsbuf, sizeof (octetsbuf));
          }

          if (usmDbDiffServPolicyPerfInOfferedPacketsGet(unit,
                                                         policyIndex,
                                                         policyInstIndex,
                                                         intIfIndex,
                                                         &packetsHi,
                                                         &packetsLo) == L7_SUCCESS)
          {
            offeredPacketsSupported = L7_TRUE;
            strUtil64toa (packetsHi, packetsLo, packetsbuf, sizeof (packetsbuf));
          }

          if (offeredOctetsSupported == L7_TRUE &&
              offeredPacketsSupported == L7_TRUE )
          {
            cliFormat(ewsContext, pStrInfo_qos_InOfferedOctetsPkts);
            ewsTelnetPrintf (ewsContext, "%s/%s", octetsbuf, packetsbuf);
          }
          else if (offeredOctetsSupported == L7_TRUE)
          {
            cliFormat(ewsContext, pStrInfo_qos_InOfferedOctets);
            ewsTelnetPrintf (ewsContext, octetsbuf );
          }
          else if (offeredPacketsSupported == L7_TRUE)
          {
            cliFormat(ewsContext, pStrInfo_qos_InOfferedPkts);
            ewsTelnetPrintf (ewsContext, packetsbuf);
          }

          /* in discarded octets/packets */
          if (usmDbDiffServPolicyPerfInDiscardedOctetsGet(unit,
                                                          policyIndex,
                                                          policyInstIndex,
                                                          intIfIndex,
                                                          &octetsHi,
                                                          &octetsLo) == L7_SUCCESS)
          {
            discardedOctetsSupported = L7_TRUE;
            strUtil64toa (octetsHi, octetsLo, octetsbuf, sizeof (octetsbuf));
          }

          if (usmDbDiffServPolicyPerfInDiscardedPacketsGet(unit,
                                                           policyIndex,
                                                           policyInstIndex,
                                                           intIfIndex,
                                                           &packetsHi,
                                                           &packetsLo) == L7_SUCCESS)
          {
            discardedPacketsSupported = L7_TRUE;
            strUtil64toa (packetsHi, packetsLo, packetsbuf, sizeof (packetsbuf));
          }

          if (discardedOctetsSupported == L7_TRUE &&
              discardedPacketsSupported == L7_TRUE )
          {
            cliFormat(ewsContext, pStrInfo_qos_InDiscardedOctetsPkts);
            ewsTelnetPrintf (ewsContext, "%s/%s", octetsbuf, packetsbuf);
          }
          else if (discardedOctetsSupported == L7_TRUE)
          {
            cliFormat(ewsContext, pStrInfo_qos_InDiscardedOctets);
            ewsTelnetPrintf (ewsContext, octetsbuf);
          }
          else if (discardedPacketsSupported == L7_TRUE)
          {
            cliFormat(ewsContext, pStrInfo_qos_InDiscardedPkts);
            ewsTelnetPrintf (ewsContext, packetsbuf);
          }
        }
        if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex, policyInstIndex,
                                           &tempPolicyIndex, &policyInstIndex) != L7_SUCCESS)
        {
          break;
        }
      } /* end while */

    } /* end else there are policies attached */

  } /* end if 'in' or 'none' specified */

  /* Display the Out-bound Service Statistics */
  if ((ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT ||
       ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE) &&
      outSupported == L7_TRUE )
  {
    cliFormat(ewsContext, pStrInfo_common_Direction);
    ewsTelnetWrite(ewsContext, pStrInfo_qos_Out);

    /* Display Operational Status */
    if (usmDbDiffServServiceIfOperStatusGet(unit, intIfIndex,
                                            ifDirection, &ifOperStatus) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_WsOperatingStatus);
      if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Up);
      }                                      /* Up */
      else if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_DOWN)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Down_1);
      }                                          /* Down */
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }                                        /* N/A */
    }

    if ((usmDbDiffServServicePolicyIndexGet(unit, intIfIndex,
                                            L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT,
                                            &policyIndex) != L7_SUCCESS) ||
        (usmDbDiffServPolicyGet(unit, policyIndex) != L7_SUCCESS))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoOutBoundPolicyIsAttachedToIntf);
      cliSyntaxBottom(ewsContext);
    }
    else
    {
      policyNameSize = sizeof(policyName);
      memset (policyName, 0, policyNameSize);
      if (usmDbDiffServPolicyNameGet(unit, policyIndex,
                                     policyName, &policyNameSize) ==
          L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_qos_PolicyName_1);
        ewsTelnetWrite(ewsContext, policyName);
      }

      /* Interface Statistics */

      ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_IntfSummary);

      cliDiffservDisplayServiceStats(ewsContext, unit, intIfIndex,
                                     L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT);

      /* Class Statistics */
      memset (buf, 0, sizeof(buf));
      tempPolicyIndex = policyIndex;
      policyInstIndex = 0;
      if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex, policyInstIndex,
                                         &tempPolicyIndex, &policyInstIndex) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoClassesAreAttachedToPolicy);
      }

      while (tempPolicyIndex == policyIndex)
      {
        if (usmDbDiffServPolicyInstClassIndexGet(unit, policyIndex, policyInstIndex,
                                                 &classIndex) == L7_SUCCESS)

        {
          offeredPacketsSupported = L7_FALSE;
          offeredOctetsSupported = L7_FALSE;
          discardedPacketsSupported = L7_FALSE;
          discardedOctetsSupported = L7_FALSE;

          cliSyntaxNewLine(ewsContext);
          classNameSize = sizeof(className);
          memset (className, 0, classNameSize);
          if (usmDbDiffServClassNameGet(unit, classIndex,
                                        className, &classNameSize) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_ClassName_1);
            ewsTelnetWrite(ewsContext, className);
          }

          /* in offered octets/packets */
          if (usmDbDiffServPolicyPerfOutOfferedOctetsGet(unit,
                                                         policyIndex,
                                                         policyInstIndex,
                                                         intIfIndex,
                                                         &octetsHi,
                                                         &octetsLo) == L7_SUCCESS)
          {
            offeredOctetsSupported = L7_TRUE;
            strUtil64toa (octetsHi, octetsLo, octetsbuf, sizeof (octetsbuf));
          }

          if (usmDbDiffServPolicyPerfOutOfferedPacketsGet(unit,
                                                          policyIndex,
                                                          policyInstIndex,
                                                          intIfIndex,
                                                          &packetsHi,
                                                          &packetsLo) == L7_SUCCESS)
          {
            offeredPacketsSupported = L7_TRUE;
            strUtil64toa (packetsHi, packetsLo, packetsbuf, sizeof (packetsbuf));
          }

          if (offeredOctetsSupported == L7_TRUE &&
              offeredPacketsSupported == L7_TRUE )
          {
            cliFormat(ewsContext, pStrInfo_qos_OutOfferedOctetsPkts);
            ewsTelnetPrintf (ewsContext, "%s/%s", octetsbuf, packetsbuf);
          }
          else if (offeredOctetsSupported == L7_TRUE)
          {
            cliFormat(ewsContext, pStrInfo_qos_OutOfferedOctets);
            ewsTelnetPrintf (ewsContext, octetsbuf );
          }
          else if (offeredPacketsSupported == L7_TRUE)
          {
            cliFormat(ewsContext, pStrInfo_qos_OutOfferedPkts);
            ewsTelnetPrintf (ewsContext, packetsbuf);
          }

          /* in discarded octets/packets */
          if (usmDbDiffServPolicyPerfOutDiscardedOctetsGet(unit,
                                                           policyIndex,
                                                           policyInstIndex,
                                                           intIfIndex,
                                                           &octetsHi,
                                                           &octetsLo) == L7_SUCCESS)
          {
            discardedOctetsSupported = L7_TRUE;
            strUtil64toa (octetsHi, octetsLo, octetsbuf, sizeof (octetsbuf));
          }

          if (usmDbDiffServPolicyPerfOutDiscardedPacketsGet(unit,
                                                            policyIndex,
                                                            policyInstIndex,
                                                            intIfIndex,
                                                            &packetsHi,
                                                            &packetsLo) == L7_SUCCESS)
          {
            discardedPacketsSupported = L7_TRUE;
            strUtil64toa (packetsHi, packetsLo, packetsbuf, sizeof (packetsbuf));
          }

          if (discardedOctetsSupported == L7_TRUE &&
              discardedPacketsSupported == L7_TRUE )
          {
            cliFormat(ewsContext, pStrInfo_qos_OutDiscardedOctetsPkts);
            ewsTelnetPrintf (ewsContext, "%s/%s", octetsbuf, packetsbuf);
          }
          else if (discardedOctetsSupported == L7_TRUE)
          {
            cliFormat(ewsContext, pStrInfo_qos_OutDiscardedOctets);
            ewsTelnetPrintf (ewsContext, octetsbuf);
          }
          else if (discardedPacketsSupported == L7_TRUE)
          {
            cliFormat(ewsContext, pStrInfo_qos_OutDiscardedPkts);
            ewsTelnetPrintf (ewsContext, packetsbuf);
          }
        }
        if (usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex, policyInstIndex,
                                           &tempPolicyIndex, &policyInstIndex) != L7_SUCCESS)
        {
          break;
        }
      } /* end while */
    } /* end else 'out' or 'none' specified */
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   Displays DiffServ Service Stats Summary Information
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
* @cmdsyntax  #> show service-policy <in | out>
*
* @cmdhelp
*
* @cmddescript  Displays summary statistical information for
*               Diffserv interfaces.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowServicePolicy(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index)
{
  L7_uint32 direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
  L7_uint32 argDirection = 1;
  L7_uint32 intIfIndex = 0;
  L7_uint32 ifDirection = 0;
  L7_uint32 packetsHi = 0;
  L7_uint32 packetsLo = 0;
  L7_uint32 unitNum = 0, slot = 0, port = 0;
  L7_char8 strDirection[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tempbuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 valbuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ifOperStatus = L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_NONE;
  L7_char8 header1[L7_CLI_DIFFSERV_LARGE_STRING];
  L7_char8 header2[L7_CLI_DIFFSERV_LARGE_STRING];
  L7_char8 header3[L7_CLI_DIFFSERV_LARGE_STRING];
  static L7_BOOL inSupported = L7_FALSE;
  static L7_BOOL outSupported = L7_FALSE;
  L7_BOOL firstPass = L7_TRUE;
  static L7_char8 dirOptions[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 policyLen;
  L7_uint32 policyIndex;
  L7_char8 stringPtr[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX];

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  memset (dirOptions, 0, sizeof(dirOptions));
  cliDiffservBuildValidDirs(dirOptions, sizeof(dirOptions), "<", ">", &inSupported, &outSupported);

  if (inSupported == L7_FALSE && outSupported == L7_FALSE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_DiffservServicesNotSupported);
  }

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowServicePolicy_1, dirOptions);
  }

  if (strlen(argv[index+argDirection]) > 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowServicePolicy_1, dirOptions);
  }

  OSAPI_STRNCPY_SAFE(strDirection,argv[index+ argDirection]);
  cliConvertToLowerCase(strDirection);

  if (strcmp(strDirection, pStrInfo_common_AclInStr) == 0 && inSupported == L7_TRUE)
  {
    direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
  }
  else if (strcmp(strDirection, pStrInfo_common_AclOutStr) == 0 && outSupported == L7_TRUE)
  {
    direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_ShowServicePolicy_1, dirOptions);
  }

  memset ( header1, 0, sizeof( header1 ));
  memset ( header2, 0, sizeof( header2 ));
  memset ( header3, 0, sizeof( header3 ));
  firstPass = L7_TRUE;

  osapiSnprintf( header1, sizeof(header1),  "\r\n        ");
  osapiSnprintfAddBlanks (1, 0, 1, 3, L7_NULLPTR,  header2, sizeof(header2),  pStrInfo_common_Intf_3);
  osapiSnprintf( header3, sizeof(header3),  "\r\n------- ");

  while (usmDbDiffServServiceGetNext(unit, intIfIndex, ifDirection,
                                     &intIfIndex, &ifDirection) == L7_SUCCESS)
  {
    if (direction == ifDirection)
    {
      policyLen = L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1;
      memset ( stringPtr, 0, sizeof( stringPtr ));
      if( usmDbDiffServServicePolicyIndexGet(unit, intIfIndex,ifDirection, &policyIndex) ==L7_SUCCESS && usmDbDiffServPolicyNameGet(unit, policyIndex, stringPtr, &policyLen) == L7_SUCCESS)
      {
        memset (buf, 0, sizeof(buf));
        if (usmDbUnitSlotPortGet(intIfIndex, &unitNum, &slot, &port) != L7_SUCCESS)
        {
          continue;
        }

        if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
        {
          osapiSnprintf(buf, sizeof(buf),  "\r\n%-8s", cliDisplayInterfaceHelp(unitNum, slot, port));
        }
        else if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT)
        {
          osapiSnprintf(buf, sizeof(buf),  "\r\n%-8s", cliDisplayInterfaceHelp(unitNum, slot, port));
        }

        if (usmDbDiffServServiceIfOperStatusGet(unit, intIfIndex,
                                                ifDirection, &ifOperStatus) == L7_SUCCESS)
        {
          if (firstPass == L7_TRUE)
          {
            /* Build the header row based on stats supported */
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR,  header1, pStrInfo_qos_Oper);
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR,  header2, pStrInfo_qos_Stat);
            OSAPI_STRNCAT( header3, "---- ");
          }
          memset ( tempbuf, 0, sizeof(tempbuf) );
          if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP)
          {
            osapiSnprintf( tempbuf, sizeof(tempbuf),  "%-5s", pStrInfo_common_Up);
          }                                                            /* Up  */
          else if (ifOperStatus == L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_DOWN)
          {
            osapiSnprintf( tempbuf, sizeof(tempbuf),  "%-5s", pStrInfo_common_Down_1);
          }                                                                /* Down*/
          else
          {
            osapiSnprintf( tempbuf, sizeof(tempbuf),  "%-5s", pStrInfo_common_NotApplicable);
          }                                                              /* N/A */
          OSAPI_STRNCAT( buf, tempbuf );
        }

        if (usmDbDiffServServicePerfOfferedPacketsGet(unit, intIfIndex, ifDirection,
                                                      &packetsHi, &packetsLo) == L7_SUCCESS)
        {
          if (firstPass == L7_TRUE)
          {
            /* Build the header row based on stats supported */
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 2, 2, L7_NULLPTR,  header1, pStrInfo_qos_Offered);
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 2, 2, L7_NULLPTR,  header2, pStrInfo_qos_Pkts);
            OSAPI_STRNCAT( header3, "---------- ");
          }
          osapiSnprintf (tempbuf, sizeof (tempbuf), "%-11s", strUtil64toa (packetsHi, packetsLo, valbuf, sizeof (valbuf)));
          OSAPI_STRNCAT( buf, tempbuf );
        }

        if (usmDbDiffServServicePerfDiscardedPacketsGet(unit, intIfIndex, ifDirection,
                                                        &packetsHi, &packetsLo) == L7_SUCCESS)
        {
          if (firstPass == L7_TRUE)
          {
            /* Build the header row based on stats supported */
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 1, L7_NULLPTR,  header1, pStrInfo_qos_Discarded);
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 2, 2, L7_NULLPTR,  header2, pStrInfo_qos_Pkts);
            OSAPI_STRNCAT( header3, "---------- ");
          }
          osapiSnprintf (tempbuf, sizeof (tempbuf), "%-11s", strUtil64toa (packetsHi, packetsLo, valbuf, sizeof (valbuf)));
          OSAPI_STRNCAT( buf, tempbuf );
        }

        if (usmDbDiffServServicePerfSentPacketsGet(unit, intIfIndex, ifDirection,
                                                   &packetsHi, &packetsLo) == L7_SUCCESS)
        {
          if (firstPass == L7_TRUE)
          {
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 3, 4, L7_NULLPTR,  header1, pStrInfo_qos_Sent);
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 2, 2, L7_NULLPTR,  header2, pStrInfo_qos_Pkts);
            OSAPI_STRNCAT( header3, "---------- ");
          }
          osapiSnprintf (tempbuf, sizeof (tempbuf), "%-11s", strUtil64toa (packetsHi, packetsLo, valbuf, sizeof (valbuf)));
          OSAPI_STRNCAT(buf, tempbuf);
        }
        if (firstPass == L7_TRUE)
        {
          /* Build the header row based on stats supported */
          OSAPI_STRNCAT_ADD_BLANKS (0, 0, 10, 15, L7_NULLPTR,  header1, pStrInfo_qos_Policy);
          OSAPI_STRNCAT_ADD_BLANKS (0, 0, 11, 16, L7_NULLPTR,  header2, pStrInfo_common_Name_1);
          OSAPI_STRNCAT( header3, "-------------------------------");
        }
        osapiSnprintf(tempbuf, sizeof(tempbuf), "%-31s",stringPtr);
        OSAPI_STRNCAT(buf, tempbuf);

        if (firstPass == L7_TRUE)
        {
          /*ewsTelnetWrite(ewsContext, CLISYNTAX_BLANK_LINE);*/
          ewsTelnetWrite(ewsContext, header1);
          ewsTelnetWrite(ewsContext, header2);
          ewsTelnetWrite(ewsContext, header3);
          firstPass = L7_FALSE;
        }
        ewsTelnetWrite(ewsContext, buf);
        /*ewsTelnetWrite(ewsContext, CLISYNTAX_BLANK_LINE);*/
      }
    }
  }

  if (firstPass == L7_TRUE)
  {
    /* No services displayed */
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_DiffservServicesNoIntfToDisp);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}
