/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/diffserv/cli_config_diffserv.c
 *
 * @purpose diffserv config commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  07/14/2003
 *
 * @author  rakeshk
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "cliapi.h"
#include "cliutil.h"
#include "clicommands_diffserv.h"
#include "cliutil_diffserv.h"
#include "usmdb_qos_diffserv_api.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "cli_web_exports.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_nim_api.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_acl_api.h"
#include "usmdb_util_diffserv_api.h"
#endif

#include "usmdb_util_api.h"
#include "ews.h"
#include "clicommands.h"
#include "cli_mode.h"
#include "clicommands_card.h"

/*********************************************************************
*
* @purpose   Sets the admin mode of DiffServ
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
* @cmdsyntax  config #> [no] diffserv
*
* @cmdhelp    Sets admin mode
*
* @cmddescript  This command sets the admin mode to enable/disable.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDiffserv(EwsContext ewsContext,
                                uintf argc,
                                const L7_char8 * * argv,
                                uintf index)
{

  L7_uint32 mode;
  L7_uint32 unit;

  L7_uint32 numArg;

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_Diffserv_2);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_Diffserv_2);
  }

  /* set admin mode */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServGenAdminModeSet(unit, mode) != L7_SUCCESS)
    {
      cliSyntaxNewLine(ewsContext);
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_common_SetAdminMode);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Core function for creating a DiffServ class
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes Called by command implementations for the 3 types of diffserv
*        classes that can be created.
*
*
* @end
*
*********************************************************************/
L7_RC_t cliDiffservClassCreate( EwsContext ewsContext, L7_char8 * strName,
                               L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType,
                               L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t classL3Protocol,
                               L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t aclType,
                               L7_uint32 aclNumber )
{
  L7_uint32 classId = 0;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strName) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_ClassNameMustBeAlnum);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* verify if this class already exists or not */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassAlreadyExists);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    /* find the free index in the table*/

    if (usmDbDiffServClassIndexNext(unit, &classId) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_DiffservClassIdxCreateFailed);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    if (0 == classId)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreClasses);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    /* create the row entry in the class table */
    if (usmDbDiffServClassCreate(unit, classId, L7_TRUE) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_DiffservClassEntryCreateFailed);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    /* set the name of the class */
    if (usmDbDiffServClassNameSet(unit, classId, strName) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_DiffservClassClassNameCreateFailed);
      /* delete the row entry just created*/
      usmDbDiffServClassDelete(unit, classId);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    /* set the class type*/
    if (usmDbDiffServClassTypeSet(unit, classId, classType) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_DiffservClassClassTypeCreateFailed);
      /* delete the row entry just created*/
      usmDbDiffServClassDelete(unit, classId);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    /* set the l3 protocol of the class */
    if (usmDbDiffServClassL3ProtoSet(unit, classId, classL3Protocol) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_DiffservClassClass);
      /* delete the row entry just created*/
      usmDbDiffServClassDelete(unit, classId);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    /* set the ACl number */
    if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
    {   /* Assumption: validation done by usmdb api */
      if (usmDbDiffServClassAclTypeSet(unit, classId, aclType) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_DiffservClassClassCreateFailedInvalidAclType);
        /* delete the row entry just created*/
        usmDbDiffServClassDelete(unit, classId);
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }
      if (usmDbDiffServClassAclNumSet(unit, classId, aclNumber) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_DiffservClassClassCreateFailedInvalidAcl);
        /* delete the row entry just created*/
        usmDbDiffServClassDelete(unit, classId);
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRowStatusGet(unit, classId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_DiffservClassClassCreateFailed);
      /* delete the row entry just created*/
      usmDbDiffServClassDelete(unit, classId);

      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;

    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose   Adds a match condition based on COS value
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
* @cmdsyntax  class-map config#> match cos <0-7>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on COS
*               (Class of Service) value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchCos(EwsContext ewsContext,
                                uintf argc,
                                const L7_char8 * * argv,
                                uintf index)
{
  L7_uint32 argCos = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 cos;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchCos);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argCos], &cos) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosVal);
  }

  /* verify if the specified value is in between 0 to 7 */
  if ((cos < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MIN)
      || (cos > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  /* reset the optional parameter 'exclude' */

    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  /* set the cos value*/
    if (usmDbDiffServClassRuleMatchCosSet(unit, classId, classRuleId, cos) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPrompt (ewsContext, "");

    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on COS value
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
* @cmdsyntax  class-map config#> match not cos <0-7>
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on COS
*               (Class of Service) value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotCos(EwsContext ewsContext,
                                   uintf argc,
                                   const L7_char8 * * argv,
                                   uintf index)
{
  L7_uint32 argCos = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 cos;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotCos);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argCos], &cos) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosVal);
  }

  /* verify if the specified value is in between 0 to 7 */
  if ((cos < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MIN)
      || (cos > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the optional parameter 'exclude' */
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the cos value*/
    if (usmDbDiffServClassRuleMatchCosSet(unit, classId, classRuleId,
                                          cos) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Secondary COS value
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
* @cmdsyntax  class-map config#> match cos <0-7>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on the Secondary COS
*               (Class of Service) value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchSecondaryCos(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index)
{
  L7_uint32 argCos = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 cos;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchSecondaryCos);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argCos], &cos) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidSecondaryCosVal);
  }

  /* verify if the specified value is in between 0 to 7 */
  if ((cos < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MIN)
      || (cos > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* reset the optional parameter 'exclude' */
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the cos value*/
    if (usmDbDiffServClassRuleMatchCos2Set(unit, classId, classRuleId,
                                           cos) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on Secondary COS value
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
* @cmdsyntax  class-map config#> match not secondary-cos <0-7>
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on COS
*               (Class of Service) value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotSecondaryCos(EwsContext ewsContext,
                                            uintf argc,
                                            const L7_char8 * * argv,
                                            uintf index)
{
  L7_uint32 argCos = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 cos;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotSecondaryCos);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argCos], &cos) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidSecondaryCosVal);
  }

  /* verify if the specified value is in between 0 to 7 */
  if ((cos < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MIN)
      || (cos > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the optional parameter 'exclude' */
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the cos value*/
    if (usmDbDiffServClassRuleMatchCos2Set(unit, classId, classRuleId,
                                           cos) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Destination IP Address
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
* @cmdsyntax  class-map config#> match dstip <ipaddr> <ipmask>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Destination IP
*               address.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchDstip(EwsContext ewsContext,
                                  uintf argc,
                                  const L7_char8 * * argv,
                                  uintf index)
{
  L7_uint32 argIpAddr = 1;
  L7_uint32 argIpMask = 2;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 ipAddr;
  L7_uint32 ipMask;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchDstIp);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argIpAddr]) >= sizeof(strIpAddr))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpAddr);
  }

  OSAPI_STRNCPY_SAFE(strIpAddr, argv[index + argIpAddr]);

  /* Verify if the specified ip address is valid */
  if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpAddr);
  }

  if (strlen(argv[index+argIpMask]) >= sizeof(strIpMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpMask);
  }

  OSAPI_STRNCPY_SAFE(strIpMask, argv[index + argIpMask]);

  /* Verify if the specified network mask is valid */
  if (usmDbInetAton(strIpMask, &ipMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpMask);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* reset the optional parameter 'exclude' */
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the IP Address */
    if (usmDbDiffServClassRuleMatchDstIpAddrSet(unit, classId, classRuleId,
                                                ipAddr) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the IP Mask */
    if (usmDbDiffServClassRuleMatchDstIpMaskSet(unit, classId, classRuleId,
                                                ipMask) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Delete the rule that was just created */
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId );
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on Destination IP Address
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
* @cmdsyntax  class-map config#> match not dstip <ipaddr> <ipmask>
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on Destination IP
*               address.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotDstip(EwsContext ewsContext,
                                     uintf argc,
                                     const L7_char8 * * argv,
                                     uintf index)
{
  L7_uint32 argIpAddr = 1;
  L7_uint32 argIpMask = 2;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 ipAddr;
  L7_uint32 ipMask;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotDstIp);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argIpAddr]) >= sizeof(strIpAddr))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpAddr);
  }
  OSAPI_STRNCPY_SAFE(strIpAddr, argv[index + argIpAddr]);

  /* Verify if the specified ip address is valid */
  if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpAddr);
  }

  if (strlen(argv[index+argIpMask]) >= sizeof(strIpMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpMask);
  }
  OSAPI_STRNCPY_SAFE(strIpMask, argv[index + argIpMask]);

  /* Verify if the specified network mask is valid */
  if (usmDbInetAton(strIpMask, &ipMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpMask);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the optional parameter 'exclude' */
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the IP Address */
    if (usmDbDiffServClassRuleMatchDstIpAddrSet(unit, classId, classRuleId,
                                                ipAddr) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the IP Mask */
    if (usmDbDiffServClassRuleMatchDstIpMaskSet(unit, classId, classRuleId,
                                                ipMask) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Delete the rule that was just created */
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId );
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Destination IPv6 Address
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
* @cmdsyntax  class-map config#> match dstipv6 <ipaddr> <plen>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Destination IPv6
*               address.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchDstip6(EwsContext ewsContext,
                                   uintf argc,
                                   const L7_char8 * * argv,
                                   uintf index)
{
  L7_uint32 argPrefix = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_in6_addr_t prefix;
  L7_uint32 prefixLen = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchDstIp_1);
  }

  if ((cliValidPrefixPrefixLenCheck(argv[index+argPrefix], &prefix, &prefixLen) != L7_SUCCESS) ||
      (prefixLen > 128))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_AclIpv6InvalidPrefix);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    strcpy(strName,EWSCLASSMAP(ewsContext));

    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* reset the optional parameter 'exclude' */
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the IP Address */
    if (usmDbDiffServClassRuleMatchDstIpv6AddrSet(unit, classId, classRuleId,
                                                  &prefix) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the IP prefix length */
    if (usmDbDiffServClassRuleMatchDstIpv6PrefLenSet(unit, classId, classRuleId,
                                                     prefixLen) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Delete the rule that was just created */
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId );
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Destination IPv6 Address
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
* @cmdsyntax  class-map config#> match dstipv6 <ipaddr> <plen>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Destination IPv6
*               address.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotDstip6(EwsContext ewsContext,
                                      uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index)
{
  L7_uint32 argPrefix = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_in6_addr_t prefix;
  L7_uint32 prefixLen = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotDstIp_1);
  }

  if ((cliValidPrefixPrefixLenCheck(argv[index+argPrefix], &prefix, &prefixLen) != L7_SUCCESS) ||
      (prefixLen > 128))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_AclIpv6InvalidPrefix);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    strcpy(strName,EWSCLASSMAP(ewsContext));

    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the optional parameter 'exclude' */
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the IP Address */
    if (usmDbDiffServClassRuleMatchDstIpv6AddrSet(unit, classId, classRuleId, &prefix) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the IP prefix length */
    if (usmDbDiffServClassRuleMatchDstIpv6PrefLenSet(unit, classId, classRuleId, prefixLen) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Delete the rule that was just created */
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId );
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }


  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Destination Layer 4 port
*            keyword / number / range
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
* @notes    supported 'portkey' values are:
*           domain, echo, ftp, ftpdata, http, smtp, snmp, telnet, tftp,
*           www
*
* @cmdsyntax  class-map config#> match dstl4port { <portkey> | <0 - 65535> [ <0 - 65535> ] }
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Destination
*               Layer 4 port keyword / number / range.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchDstl4port(EwsContext ewsContext,
                                      uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index)
{
  L7_uint32 argKeyword = 1;
  L7_uint32 argNumber = 1;
  L7_uint32 argStartPort = 1;
  L7_uint32 argEndPort = 2;
  L7_char8 strKeyword[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 startPort;
  L7_uint32 endPort;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 port;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
  {
    if (numArg > 2 || numArg < 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchDstL4Port);
    }
  }
  else
  {
    /* check number of arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchDstL4PortNoRange);
    }
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* a keyword? */
  if ((numArg == 1) && (cliCheckIfInteger((L7_char8 *)argv[index+argKeyword]) != L7_SUCCESS))
  {
    if (strlen(argv[index+argKeyword]) >= sizeof(strKeyword))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidKeyword);
    }
    OSAPI_STRNCPY_SAFE(strKeyword, argv[index + argKeyword]);
    cliConvertToLowerCase(strKeyword);

    if (strcmp(strKeyword, pStrInfo_qos_Domain_1) == 0)
    {
      /* domain */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_DOMAIN;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Echo) == 0)
    {
      /* echo */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_ECHO;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Ftp) == 0)
    {
      /* ftp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Ftpdata) == 0)
    {
      /* ftpdata */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTPDATA;
    }
    else if ((strcmp(strKeyword, pStrInfo_common_Http_1) == 0) || (strcmp(strKeyword, pStrInfo_qos_Www) == 0))
    {
      /* http or www (both have same value) */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_HTTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Smtp) == 0)
    {
      /* smtp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SMTP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Snmp_1) == 0)
    {
      /* snmp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SNMP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Telnet) == 0)
    {
      /* telnet */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TELNET;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Tftp_1) == 0)
    {
      /* tftp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TFTP;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidKeyword);
    }

    startPort = port;
    endPort = port;
  }
  else
  {
    /* only a number? */
    if (numArg == 1)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+argNumber], &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPortNum);
      }

      if (port > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX) /* Convert routine already verifies non-negative number */
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPortNum);
      }

      startPort = port;
      endPort = port;
    }
    else  /* range */
    {
      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argStartPort], &startPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartPort);
      }

      /* verify if the specified value is in between 0 to 65535 */
      if ((startPort < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN)
          || (startPort > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPortVal);
      }

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argEndPort], &endPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndPort);
      }

      /* verify if the specified value is in between 0 to 65535 */
      if ((endPort < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN)
          || (endPort > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPortVal);
      }

      if (endPort < startPort)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndPortMustBeGreaterThanStartPort);
      }
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the start port */
    if (usmDbDiffServClassRuleMatchDstL4PortStartSet(unit, classId, classRuleId,
                                                     startPort) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the end port */
    if (usmDbDiffServClassRuleMatchDstL4PortEndSet(unit, classId, classRuleId,
                                                   endPort) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on Destination Layer 4 port
*            keyword / number / range
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
* @notes    supported 'portkey' values are:
*           domain, echo, ftp, ftpdata, http, smtp, snmp, telnet, tftp,
*           www
*
* @cmdsyntax  class-map config#> match not dstl4port { keyword <portkey> | number <0 - 65535>
*                                   | range <0 - 65535> <0 - 65535> }
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on Destination
*               Layer 4 port keyword / number / range.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotDstl4port(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index)
{
  L7_uint32 argKeyword = 1;
  L7_uint32 argNumber = 1;
  L7_uint32 argStartPort = 1;
  L7_uint32 argEndPort = 2;
  L7_char8 strKeyword[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 startPort;
  L7_uint32 endPort;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 port;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg > 2 || numArg < 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotDstL4Port);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* a keyword? */
  if ((numArg == 1) && (cliCheckIfInteger((L7_char8 *)argv[index+argKeyword]) != L7_SUCCESS))
  {
    if (strlen(argv[index+argKeyword]) >= sizeof(strKeyword))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidKeyword);
    }
    OSAPI_STRNCPY_SAFE(strKeyword, argv[index + argKeyword]);
    cliConvertToLowerCase(strKeyword);

    if (strcmp(strKeyword, pStrInfo_qos_Domain_1) == 0)
    {
      /* domain */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_DOMAIN;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Echo) == 0)
    {
      /* echo */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_ECHO;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Ftp) == 0)
    {
      /* ftp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Ftpdata) == 0)
    {
      /* ftpdata */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTPDATA;
    }
    else if ((strcmp(strKeyword, pStrInfo_common_Http_1) == 0) || (strcmp(strKeyword, pStrInfo_qos_Www) == 0))
    {
      /* http or www (both have same value) */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_HTTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Smtp) == 0)
    {
      /* smtp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SMTP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Snmp_1) == 0)
    {
      /* snmp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SNMP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Telnet) == 0)
    {
      /* telnet */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TELNET;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Tftp_1) == 0)
    {
      /* tftp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TFTP;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidKeyword);
    }

    startPort = port;
    endPort = port;

  }
  else
  {
    /* only a number? */
    if (numArg == 1)
    {

      if (cliConvertTo32BitUnsignedInteger(argv[index+argNumber], &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPortNum);
      }

      if (port < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN ||
          port > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX) /* Convert routine already verifies non-negative number */
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPortNum);
      }

      startPort = port;
      endPort = port;
    }
    else  /* range */
    {

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argStartPort], &startPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartPort);
      }

      /* verify if the specified value is in between 0 to 65535 */
      if ((startPort < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN)
          || (startPort > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPortVal);
      }

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argEndPort], &endPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndPort);
      }

      /* verify if the specified value is in between 0 to 65535 */
      if ((endPort < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN)
          || (endPort > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPortVal);
      }

      if (endPort < startPort)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndPortMustBeGreaterThanStartPort);
      }
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the start port */
    if (usmDbDiffServClassRuleMatchDstL4PortStartSet(unit, classId, classRuleId,
                                                     startPort) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the end port */
    if (usmDbDiffServClassRuleMatchDstL4PortEndSet(unit, classId, classRuleId,
                                                   endPort) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Destination MAC Address
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
* @cmdsyntax  class-map config#> match destination-address mac <address> <mac-mask>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Destination
*               MAC address
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchDestinationAddressMac(EwsContext ewsContext,
                                                  uintf argc,
                                                  const L7_char8 * * argv,
                                                  uintf index)
{

  L7_uint32 argMacAddr = 1;
  L7_uint32 argMacMask = 2;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMacMask[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 strConvertedMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strConvertedMacMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchDstAddrMac);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argMacAddr]) >= sizeof(strMacAddr))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacAddrType);
  }
  OSAPI_STRNCPY_SAFE(strMacAddr, argv[index + argMacAddr]);

  /* verify if the specified mac address is valid */
  if (cliConvertMac(strMacAddr, strConvertedMacAddr) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr );
  }

  if (strlen(argv[index+argMacMask]) >= sizeof(strMacMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacMaskType);
  }
  OSAPI_STRNCPY_SAFE(strMacMask, argv[index + argMacMask]);

  /* verify if the specified mac mask is valid */
  if (cliConvertMac(strMacMask, strConvertedMacMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacMaskType);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the MAC Address */
    if (usmDbDiffServClassRuleMatchDstMacAddrSet(unit, classId, classRuleId,
                                                 strConvertedMacAddr) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the MAC Mask */
    if (usmDbDiffServClassRuleMatchDstMacMaskSet(unit, classId, classRuleId,
                                                 strConvertedMacMask) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on Destination MAC Address
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
* @cmdsyntax  class-map config#> match not destination address mac <macaddr> <macmask>
*
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on Destination
*               MAC address
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotDestinationAddressMac(EwsContext ewsContext,
                                                     uintf argc,
                                                     const L7_char8 * * argv,
                                                     uintf index)
{
  L7_uint32 argMacAddr = 1;
  L7_uint32 argMacMask = 2;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMacMask[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 strConvertedMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strConvertedMacMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotDstAddrMac);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argMacAddr]) >= sizeof(strMacAddr))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacAddrType);
  }
  OSAPI_STRNCPY_SAFE(strMacAddr, argv[index + argMacAddr]);

  /* verify if the specified mac address is valid */
  if (cliConvertMac(strMacAddr, strConvertedMacAddr) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
  }

  if (strlen(argv[index+argMacMask]) >= sizeof(strMacMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacMaskType);
  }
  OSAPI_STRNCPY_SAFE(strMacMask, argv[index + argMacMask]);

  /* verify if the specified mac mask is valid */
  if (cliConvertMac(strMacMask, strConvertedMacMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacMaskType);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the MAC Address */
    if (usmDbDiffServClassRuleMatchDstMacAddrSet(unit, classId, classRuleId,
                                                 strConvertedMacAddr) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the MAC Mask */
    if (usmDbDiffServClassRuleMatchDstMacMaskSet(unit, classId, classRuleId,
                                                 strConvertedMacMask) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Adds a match condition whereby all packets are considered
*           to belong to the specified class
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
* @cmdsyntax  class-map config#> match any
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class whereby all packets are
*               considered to belong to this class, identified by the name
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchAny(EwsContext ewsContext,
                                uintf argc,
                                const L7_char8 * * argv,
                                uintf index)
{

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchAny);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }


  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Adds a not match condition whereby all packets are considered
*           to belong to the specified class
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
* @cmdsyntax  class-map config#> match not any
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class whereby all packets are
*               considered to belong to this class, identified by the name
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotAny(EwsContext ewsContext,
                                   uintf argc,
                                   const L7_char8 * * argv,
                                   uintf index)
{

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotAny);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on DSCP value
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
* @cmdsyntax  class-map config#> match ipdscp <dscpval>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on DSCP value
*               of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchIpDscp(EwsContext ewsContext,
                                   uintf argc,
                                   const L7_char8 * * argv,
                                   uintf index)
{
  L7_uint32 argDscp = 1;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 dscp;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strDscp[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchIpDscp_1);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argDscp]) >= sizeof(strDscp))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDscpVal);
  }
  OSAPI_STRNCPY_SAFE(strDscp,argv[index+ argDscp]);

  /* handle input dscp as keyword or value */
  if (cliDiffservConvertDSCPStringToValNoVerify(strDscp, &dscp) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDscpVal);
  }

  /* verify if the specified value is in between 0 to 63 */
  if ((dscp < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPDSCP_MIN) ||
      (dscp > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPDSCP_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectDscpVal);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the dscp value*/
    if (usmDbDiffServClassRuleMatchIpDscpSet(unit, classId, classRuleId,
                                             dscp) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on DSCP value
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
* @cmdsyntax  class-map config#> match not ipdscp <dscpval>
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on DSCP value
*               of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotIpDscp(EwsContext ewsContext,
                                      uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index)
{
  L7_uint32 argDscp = 1;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 dscp;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strDscp[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotIpDscp);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argDscp]) >= sizeof(strDscp))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDscpVal);
  }
  OSAPI_STRNCPY_SAFE(strDscp,argv[index+ argDscp]);

  /* handle input dscp as keyword or value */
  if (cliDiffservConvertDSCPStringToValNoVerify(strDscp, &dscp) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDscpVal);
  }

  /* verify if the specified value is in between 0 to 63 */
  if ((dscp < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPDSCP_MIN) ||
      (dscp > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPDSCP_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectDscpVal);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the dscp value*/
    if (usmDbDiffServClassRuleMatchIpDscpSet(unit, classId, classRuleId,
                                             dscp) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on ethertype
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
* @cmdsyntax  class-map config#> match ethertype <keyword> | <0x0600-0xFFFF>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on ethertype value
*               of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchEtherType(EwsContext ewsContext,
                                      uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index)
{
  L7_uint32 argKeyword = 1;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_QOS_ETYPE_KEYID_t keyId = L7_QOS_ETYPE_KEYID_NONE;
  L7_uint32 customValue = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strKeyword[L7_CLI_MAX_STRING_LENGTH];


  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchEtherType);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argKeyword]) >= sizeof(strKeyword))
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEtypeKeywordOrVal);
    return cliSyntaxReturnPrompt (ewsContext,"\r\n\r\n%s", pStrInfo_qos_MacAccessListEtherTypeKeywordOrVal);
  }
  OSAPI_STRNCPY_SAFE(strKeyword,argv[index+ argKeyword]);

  if (cliDiffservConvertEtherTypeStringToKeyId(strKeyword, &keyId) != L7_SUCCESS)
  {

    /* not a recognized keyword, so check for valid Ethertype hex value */
    if (cliConvertEtypeCustomValue(strKeyword, &customValue) != L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEtypeKeywordOrVal);
      return cliSyntaxReturnPrompt (ewsContext,"\r\n\r\n%s", pStrInfo_qos_MacAccessListEtherTypeKeywordOrVal);
    }

    /* verify custom ethertype value is within allowed range */
    if ((customValue < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MIN) ||
        (customValue > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_IncorrectEtypeCustomValRange,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MAX);
    }

    /* set up a 'custom' key id for use with application API */
    keyId = L7_QOS_ETYPE_KEYID_CUSTOM;
  }

  /* verify if the specified keyword is supported */
  if ((keyId < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPEKEYID_MIN) ||
      (keyId > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPEKEYID_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectEtypeKeyword);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE;

    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the Ethertype key id */
    if (usmDbDiffServClassRuleMatchEtypeKeySet(unit, classId, classRuleId,
                                               keyId) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* if 'custom' key id, set Ethertype value */
    if (keyId == L7_QOS_ETYPE_KEYID_CUSTOM)
    {
      if (usmDbDiffServClassRuleMatchEtypeValueSet(unit, classId, classRuleId,
                                                   customValue) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
      }
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on ethertype
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
* @cmdsyntax  class-map config#> match not ethertype <keyword> | <0x0600-0xFFFF>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on ethertype value
*               of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotEtherType(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index)
{
  L7_uint32 argKeyword = 1;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_QOS_ETYPE_KEYID_t keyId = L7_QOS_ETYPE_KEYID_NONE;
  L7_uint32 customValue = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strKeyword[L7_CLI_MAX_STRING_LENGTH];


  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotEtherType);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argKeyword]) >= sizeof(strKeyword))
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEtypeKeywordOrVal);
    return cliSyntaxReturnPrompt (ewsContext,"\r\n\r\n%s", pStrInfo_qos_MacAccessListEtherTypeKeywordOrVal);
  }
  OSAPI_STRNCPY_SAFE(strKeyword,argv[index+ argKeyword]);

  if (cliDiffservConvertEtherTypeStringToKeyId(strKeyword, &keyId) != L7_SUCCESS)
  {
    /* not a recognized keyword, so check for valid Ethertype hex value */
    if (cliConvertEtypeCustomValue(strKeyword, &customValue) != L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEtypeKeywordOrVal);
      return cliSyntaxReturnPrompt (ewsContext,"\r\n\r\n%s", pStrInfo_qos_MacAccessListEtherTypeKeywordOrVal);
    }

    /* verify custom ethertype value is within allowed range */
    if ((customValue < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MIN) ||
        (customValue > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_IncorrectEtypeCustomValRange,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MAX);
    }

    /* set up a 'custom' key id for use with application API */
    keyId = L7_QOS_ETYPE_KEYID_CUSTOM;
  }

  /* verify if the specified keyword is supported */
  if ((keyId < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPEKEYID_MIN) ||
      (keyId > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPEKEYID_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectEtypeKeyword);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    /* specify the entry type format */
    entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE;

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                  entryType) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
      }
    }

    /* set the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the Ethertype value*/
    if (usmDbDiffServClassRuleMatchEtypeKeySet(unit, classId, classRuleId,
                                               keyId) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* if 'custom' keyword, set Ethertype value */
    if (keyId == L7_QOS_ETYPE_KEYID_CUSTOM)
    {
      if (usmDbDiffServClassRuleMatchEtypeValueSet(unit, classId, classRuleId,
                                                   customValue) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
      }
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on IP Precedence value
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
* @cmdsyntax  class-map config#> match ipprecedence <0-7>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on IP Precedence
*               value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchIpPrecedence(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index)
{

  L7_uint32 argPrecedence = 1;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 precedence;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchIpPrecedence);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argPrecedence], &precedence) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidIpPrecedenceVal);
  }

  /* verify if the specified value is in between 0 to 7 */
  if ((precedence < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPPRECEDENCE_MIN) ||
      (precedence > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPPRECEDENCE_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the precedence value*/
    if (usmDbDiffServClassRuleMatchIpPrecedenceSet(unit, classId, classRuleId,
                                                   precedence ) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      }
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on IP Precedence value
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
* @cmdsyntax  class-map config#> match not ipprecedence <0-7>
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on IP Precedence
*               value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotIpPrecedence(EwsContext ewsContext,
                                            uintf argc,
                                            const L7_char8 * * argv,
                                            uintf index)
{
  L7_uint32 argPrecedence = 1;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 precedence;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotIpPrecedence);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argPrecedence], &precedence) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidIpPrecedenceVal);
  }

  /* verify if the specified value is in between 0 to 7 */
  if ((precedence < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPPRECEDENCE_MIN) ||
      (precedence > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPPRECEDENCE_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the 'exclude' flag*/
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the precedence value*/
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchIpPrecedenceSet(unit, classId, classRuleId,
                                                   precedence ) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      }
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on IP Tos value
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
* @cmdsyntax  class-map config#> match iptos <tosbits> <tosmask>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on IP Tos
*               value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchIpTos(EwsContext ewsContext,
                                  uintf argc,
                                  const L7_char8 * * argv,
                                  uintf index)
{
  L7_uint32 argTosBits = 1;
  L7_uint32 argTosMask = 2;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strTosBits[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strTosMask[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 strConvertedTosBits[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strConvertedTosMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchIpTos);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argTosBits]) >= sizeof(strTosBits))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectTosBits);
  }
  OSAPI_STRNCPY_SAFE(strTosBits, argv[index + argTosBits]);

  /* verify if the specified tos bits are valid */
  if (usmDbConvertTwoDigitHex(strTosBits, strConvertedTosBits) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectTosBits);
  }

  if (strlen(argv[index+argTosMask]) >= sizeof(strTosMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectTosMask);
  }
  OSAPI_STRNCPY_SAFE(strTosMask, argv[index + argTosMask]);

  /* verify if the specified tos mask is valid */
  if (usmDbConvertTwoDigitHex(strTosMask, strConvertedTosMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrInfo_qos_TosmaskMustBeTwoDigitHexValInRangeOf00AndFf);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the MAC Address */

    if (usmDbDiffServClassRuleMatchIpTosBitsSet(unit, classId, classRuleId,
                                                (L7_char8) (*strConvertedTosBits) ) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the MAC Mask */
    if (usmDbDiffServClassRuleMatchIpTosMaskSet(unit, classId, classRuleId,
                                                (L7_char8) (*strConvertedTosMask) ) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      }
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on IP Tos value
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
* @cmdsyntax  class-map config#> match not iptos <tosbits> <tosmask>
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on IP Tos
*               value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotIpTos(EwsContext ewsContext,
                                     uintf argc,
                                     const L7_char8 * * argv,
                                     uintf index)
{
  L7_uint32 argTosBits = 1;
  L7_uint32 argTosMask = 2;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strTosBits[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strTosMask[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 strConvertedTosBits[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strConvertedTosMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotIpTos);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argTosBits]) >= sizeof(strTosBits))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectTosBits);
  }
  OSAPI_STRNCPY_SAFE(strTosBits, argv[index + argTosBits]);

  /* verify if the specified tos bits are valid */
  if (usmDbConvertTwoDigitHex(strTosBits, strConvertedTosBits) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectTosBits);
  }

  if (strlen(argv[index+argTosMask]) >= sizeof(strTosMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectTosMask);
  }
  OSAPI_STRNCPY_SAFE(strTosMask, argv[index + argTosMask]);

  /* verify if the specified tos mask is valid */
  if (usmDbConvertTwoDigitHex(strTosMask, strConvertedTosMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrInfo_qos_TosmaskMustBeTwoDigitHexValInRangeOf00AndFf);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the 'exclude' flag*/
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the MAC Address */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchIpTosBitsSet(unit, classId, classRuleId,
                                                (L7_char8) (*strConvertedTosBits) ) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the MAC Mask */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchIpTosMaskSet(unit, classId, classRuleId,
                                                (L7_char8) (*strConvertedTosMask) ) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      }
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on IP Tos value
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
* @cmdsyntax  class-map config#> match ip6flowlbl <>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on IP Tos
*               value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchIp6FlowLbl(EwsContext ewsContext,
                                       uintf argc,
                                       const L7_char8 * * argv,
                                       uintf index)
{
  L7_uint32 argFlowLbl = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strFlowLbl[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 flowLbl;

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchIp);
  }

  osapiStrncpySafe(strName, EWSCLASSMAP(ewsContext), sizeof(strName));

  if (strlen(argv[index+argFlowLbl]) >= sizeof(strFlowLbl))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectFlow);
  }
  osapiStrncpySafe(strFlowLbl, argv[index + argFlowLbl], sizeof(strFlowLbl));

  /* verify if the specified tos bits are valid */
  if (cliConvertTo32BitUnsignedInteger(strFlowLbl, &flowLbl) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectFlow);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the MAC Address */

    if (usmDbDiffServClassRuleMatchIp6FlowLabelSet(unit, classId, classRuleId, flowLbl) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      }
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on IP Tos value
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
* @cmdsyntax  class-map config#> match not ip6flowlbl <label>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on IP Tos
*               value of the packet.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotIp6FlowLbl(EwsContext ewsContext,
                                          uintf argc,
                                          const L7_char8 * * argv,
                                          uintf index)
{
  L7_uint32 argFlowLbl = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strFlowLbl[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 flowLbl;

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotIp);
  }

  osapiStrncpySafe(strName, EWSCLASSMAP(ewsContext), sizeof(strName));

  if (strlen(argv[index+argFlowLbl]) >= sizeof(strFlowLbl))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectFlow);
  }
  osapiStrncpySafe(strFlowLbl, argv[index + argFlowLbl], sizeof(strFlowLbl));

  /* verify if the specified tos bits are valid */
  if (cliConvertTo32BitUnsignedInteger(strFlowLbl, &flowLbl) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectFlow);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the MAC Address */

    if (usmDbDiffServClassRuleMatchIp6FlowLabelSet(unit, classId, classRuleId, flowLbl) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      }
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose   Adds a match condition based on Protocol keyword / number
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
* @notes    supported protocol keyword are:
*           icmp, igmp, ip, tcp, udp
*
* @cmdsyntax  class-map config#> match protocol { keyword <protocolkey> | number <0-255> }
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Protocol
*               keyword / number.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchProtocol(EwsContext ewsContext,
                                     uintf argc,
                                     const L7_char8 * * argv,
                                     uintf index)
{
  L7_uint32 argKeyword = 1;
  L7_uint32 argProtocol = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 protocol;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strKeyword[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchProto);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* keyword ? */
  if (cliCheckIfInteger((L7_char8 *)argv[index+argKeyword]) != L7_SUCCESS)
  {
    if (strlen(argv[index+argKeyword]) >= sizeof(strKeyword))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidMatchKeyword);
    }
    OSAPI_STRNCPY_SAFE(strKeyword, argv[index + argKeyword]);
    cliConvertToLowerCase(strKeyword);

    if (strcmp(strKeyword, pStrInfo_common_Icmp_1) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_ICMP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Igmp_2) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IGMP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_IpOption) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Tcp_1) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_TCP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Udp_1) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_UDP;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidMatchKeyword);
    }
  }
  else /* its a number */
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argProtocol], &protocol) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidProtoNum);
    }

    /* verify if the specified value is in between 0 to 255 */
    if ((protocol < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_PROTOCOL_MIN) ||
        (protocol > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_PROTOCOL_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectProtoNum);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }
  /* set the protocol number */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchProtocolNumSet(unit, classId, classRuleId,
                                                  protocol) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      }
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on Protocol keyword / number
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
* @notes    supported protocol keyword are:
*           icmp, igmp, ip, tcp, udp
*
* @cmdsyntax  class-map config#> match not protocol { <protocolkey> | <0-255> }
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on Protocol
*               keyword / number.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotProtocol(EwsContext ewsContext,
                                        uintf argc,
                                        const L7_char8 * * argv,
                                        uintf index)
{
  L7_uint32 argKeyword = 1;
  L7_uint32 argProtocol = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_uint32 protocol;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strKeyword[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotProto);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* keyword ? */
  if (cliCheckIfInteger((L7_char8 *)argv[index+argKeyword]) != L7_SUCCESS)
  {
    if (strlen(argv[index+argKeyword]) >= sizeof(strKeyword))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidMatchKeyword);
    }
    OSAPI_STRNCPY_SAFE(strKeyword, argv[index + argKeyword]);
    cliConvertToLowerCase(strKeyword);

    if (strcmp(strKeyword, pStrInfo_common_Icmp_1) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_ICMP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Igmp_2) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IGMP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_IpOption) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Tcp_1) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_TCP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Udp_1) == 0)
    {
      protocol = L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_UDP;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidMatchKeyword);
    }
  }
  else /* its a number */
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argProtocol], &protocol) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidProtoNum);
    }

    /* verify if the specified value is in between 0 to 255 */
    if ((protocol < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_PROTOCOL_MIN) ||
        (protocol > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_PROTOCOL_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectProtoNum);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the 'exclude' flag*/
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the protocol number */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchProtocolNumSet(unit, classId, classRuleId,
                                                  protocol) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      }
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Adds/Removes a set of match conditions defined for another class
*           specified by reference class
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
* @cmdsyntax  class-map config#> [no] match class-map <refclassname>
*
* @cmdhelp
*
* @cmddescript  Adds a set of match conditions defined for another class
*               identified by the <refclassname>
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchClassMap(EwsContext ewsContext,
                                     uintf argc,
                                     const L7_char8 * * argv,
                                     uintf index)
{

  L7_uint32 argRefName = 1;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;
  L7_uint32 refClassId = 0;
  L7_uint32 tempClassId = 0;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strRefName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t refClassType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_NoMatchClassMap);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchClassMap);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* verify the length of <refclassname> argument */
  if (strlen(argv[index+ argRefName]) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_RefClassNameTooLong);
  }

  OSAPI_STRNCPY_SAFE(strRefName,argv[index+ argRefName]);

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strRefName) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_RefClassNameAlnum);
  }

  /* check if both name are identical */
  if (strcmp(strName, strRefName) == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_SelfReferencingClassNameNotAllowed);
  }


  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* get the type of the specified class */
    if (usmDbDiffServClassTypeGet(unit, classId, &classType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_qos_AddRefClass);
    }

    /* find the index of the specified reference class */
    if (usmDbDiffServClassNameToIndex(unit, strRefName, &refClassId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_RefClassDoesntExist);
    }

    /* get the type of the specified class */
    if (usmDbDiffServClassTypeGet(unit, refClassId, &refClassType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddRefClass);
    }

    if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantAddRefClassForAcl);
    }
    else if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL)
    {
      if (refClassType != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_BothClassesMustBeAll);
      }
    }
    else if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY)
    {
      if (refClassType != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_BothClassesMustBeAny);
      }
    }

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /* find the free index in the class rule table*/
      if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddRefClass);
      }
      if (0 == classRuleId)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
      }

      /* create the row entry in the class rule table */
      if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddRefClass);
      }

      /* specify the entry type format */
      entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS;

      if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                  entryType) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddRefClass);
      }

      /* set the optional parameter 'exclude' */
      if (usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                    L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddRefClass);
      }

      /* set the reference class */
      if (usmDbDiffServClassRuleMatchRefClassIndexSet(unit, classId, classRuleId,
                                                      refClassId) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddRefClass);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /* check if reference class index matches the one specified */
      if (( usmDbDiffServClassToRefClass( unit, classId, &tempClassId ) != L7_SUCCESS ) ||
          ( tempClassId != refClassId ))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_RefClassIsNotRefd);
      }

      if (usmDbDiffServClassRuleMatchRefClassRuleIndexFind( unit, classId, &classRuleId )
          != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_RefClassRuleNotExist);
      }

      if (usmDbDiffServClassRuleMatchRefClassIndexRemove(
            unit, classId, classRuleId ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_RemoveRefClass);
      }

      /* it is possible for the row to be deleted in certain cases (otherwise,
       * it remains active)
       */
      if (usmDbDiffServClassRuleGet(unit, classId, classRuleId) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Source IP Address
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
* @cmdsyntax  class-map config#> match srcip <ipaddr> <ipmask>
*
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Source IP
*               address.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchSrcip(EwsContext ewsContext,
                                  uintf argc,
                                  const L7_char8 * * argv,
                                  uintf index)
{

  L7_uint32 argIpAddr = 1;
  L7_uint32 argIpMask = 2;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;
  L7_uint32 ipAddr;
  L7_uint32 ipMask;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchSrcIp);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argIpAddr]) >= sizeof(strIpAddr))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpAddr);
  }
  OSAPI_STRNCPY_SAFE(strIpAddr, argv[index + argIpAddr]);

  /* Verify if the specified ip address is valid */
  if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpAddr);
  }

  if (strlen(argv[index+argIpMask]) >= sizeof(strIpMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpMask);
  }
  OSAPI_STRNCPY_SAFE(strIpMask, argv[index + argIpMask]);

  /* Verify if the specified network mask is valid */
  if (usmDbInetAton(strIpMask, &ipMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpMask);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  }
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the IP Address */
    if (usmDbDiffServClassRuleMatchSrcIpAddrSet(unit, classId, classRuleId,
                                                ipAddr) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the IP Mask */
    if (usmDbDiffServClassRuleMatchSrcIpMaskSet(unit, classId, classRuleId,
                                                ipMask) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on Source IP Address
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
* @cmdsyntax  class-map config#> match not srcip <ipaddr> <ipmask>
*
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on Source IP
*               address.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotSrcip(EwsContext ewsContext,
                                     uintf argc,
                                     const L7_char8 * * argv,
                                     uintf index)
{

  L7_uint32 argIpAddr = 1;
  L7_uint32 argIpMask = 2;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;
  L7_uint32 ipAddr;
  L7_uint32 ipMask;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotSrcIp);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argIpAddr]) >= sizeof(strIpAddr))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpAddr);
  }
  OSAPI_STRNCPY_SAFE(strIpAddr, argv[index + argIpAddr]);

  /* Verify if the specified ip address is valid */
  if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpAddr);
  }

  if (strlen(argv[index+argIpMask]) >= sizeof(strIpMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpMask);
  }
  OSAPI_STRNCPY_SAFE(strIpMask, argv[index + argIpMask]);

  /* Verify if the specified network mask is valid */
  if (usmDbInetAton(strIpMask, &ipMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgDiffservIpMask);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the 'exclude' flag*/

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the IP Address */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcIpAddrSet(unit, classId, classRuleId,
                                                ipAddr) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the IP Mask */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcIpMaskSet(unit, classId, classRuleId,
                                                ipMask) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Source IPv6 Address
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
* @cmdsyntax  class-map config#> match dstipv6 <ipaddr> <plen>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Destination IPv6
*               address.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchSrcip6(EwsContext ewsContext,
                                   uintf argc,
                                   const L7_char8 * * argv,
                                   uintf index)
{
  L7_uint32 argPrefix = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_in6_addr_t prefix;
  L7_uint32 prefixLen = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchSrcIp_1);
  }

  if ((cliValidPrefixPrefixLenCheck(argv[index+argPrefix], &prefix, &prefixLen) != L7_SUCCESS) ||
      (prefixLen > 128))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_AclIpv6InvalidPrefix);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    strcpy(strName,EWSCLASSMAP(ewsContext));

    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* reset the optional parameter 'exclude' */

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }
  /* set the IP Address */

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcIpv6AddrSet(unit, classId, classRuleId,
                                                  &prefix) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the IP prefix length */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcIpv6PrefLenSet(unit, classId, classRuleId,
                                                     prefixLen) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Delete the rule that was just created */
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId );
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Source IPv6 Address
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
* @cmdsyntax  class-map config#> match not srcip6 <ipaddr> <plen>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Destination IPv6
*               address.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotSrcip6(EwsContext ewsContext,
                                      uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index)
{
  L7_uint32 argPrefix = 1;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId;
  L7_in6_addr_t prefix;
  L7_uint32 prefixLen = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotSrcIp_1);
  }

  if ((cliValidPrefixPrefixLenCheck(argv[index+argPrefix], &prefix, &prefixLen) != L7_SUCCESS) ||
      (prefixLen > 128))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_AclIpv6InvalidPrefix);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    strcpy(strName,EWSCLASSMAP(ewsContext));

    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the optional parameter 'exclude' */

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }
  /* set the IP Address */

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcIpv6AddrSet(unit, classId, classRuleId,
                                                  &prefix) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the IP prefix length */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcIpv6PrefLenSet(unit, classId, classRuleId,
                                                     prefixLen) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Delete the rule that was just created */
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId );
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Source Layer 4 port
*            keyword / number / range
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
* @notes    supported 'portkey' values are:
*           domain, echo, ftp, ftpdata, http, smtp, snmp, telnet, tftp,
*           www
*
* @cmdsyntax  class-map config#> match srcl4port { <portkey> | <0 - 65535> [<0 - 65535>] }
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Source
*               Layer 4 port keyword / number / range.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchSrcl4port(EwsContext ewsContext,
                                      uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index)
{

  L7_uint32 argKeyword = 1;
  L7_uint32 argNumber = 1;
  L7_uint32 argStartPort = 1;
  L7_uint32 argEndPort = 2;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;
  L7_uint32 port = 0;
  L7_uint32 startPort = 0;
  L7_uint32 endPort = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strKeyword[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
  {
    if (numArg > 2 || numArg < 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchSrcL4Port);
    }
  }
  else
  {
    /* check number of arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchSrcL4PortNoRange);
    }
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  /* a keyword? */
  if ((numArg == 1) && (cliCheckIfInteger((L7_char8 *)argv[index+argKeyword]) != L7_SUCCESS))
  {
    if (strlen(argv[index+argKeyword]) >= sizeof(strKeyword))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidKeyword);
    }
    OSAPI_STRNCPY_SAFE(strKeyword, argv[index + argKeyword]);
    cliConvertToLowerCase(strKeyword);

    if (strcmp(strKeyword, pStrInfo_qos_Domain_1) == 0)
    {
      /* domain */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_DOMAIN;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Echo) == 0)
    {
      /* echo */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_ECHO;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Ftp) == 0)
    {
      /* ftp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Ftpdata) == 0)
    {
      /* ftpdata */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTPDATA;
    }
    else if ((strcmp(strKeyword, pStrInfo_common_Http_1) == 0) || (strcmp(strKeyword, pStrInfo_qos_Www) == 0))
    {
      /* http or www (both have same value) */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_HTTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Smtp) == 0)
    {
      /* smtp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SMTP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Snmp_1) == 0)
    {
      /* snmp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SNMP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Telnet) == 0)
    {
      /* telnet */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TELNET;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Tftp_1) == 0)
    {
      /* tftp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TFTP;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidKeyword);
    }

    startPort = port;
    endPort = port;
  }
  else
  {
    /* only a number? */
    if (numArg == 1)
    {

      if (cliConvertTo32BitUnsignedInteger(argv[index+argNumber], &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPortNum);
      }

      if (port > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX) /* Convert routine already verifies non-negative number */
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPortNum);
      }

      startPort = port;
      endPort = port;
    }
    else  /* range */
    {
      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argStartPort], &startPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartPort);
      }

      /* verify if the specified value is in between 0 to 65535 */
      if ((startPort < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN) ||
          (startPort > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPortVal);
      }

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argEndPort], &endPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndPort);
      }

      /* verify if the specified value is in between 0 to 65535 */
      if ((endPort < 0) || (endPort > 65535))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPortVal);
      }

      if (endPort < startPort)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndPortMustBeGreaterThanStartPort);
      }
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the start port */
    if (usmDbDiffServClassRuleMatchSrcL4PortStartSet(unit, classId, classRuleId,
                                                     startPort) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the end port */
    if (usmDbDiffServClassRuleMatchSrcL4PortEndSet(unit, classId, classRuleId,
                                                   endPort) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on Source Layer 4 port
*            keyword / number / range
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
* @notes    supported 'portkey' values are:
*           domain, echo, ftp, ftpdata, http, smtp, snmp, telnet, tftp,
*           www
*
* @cmdsyntax  class-map config#> match not srcl4port { <portkey> | <0 - 65535> [<0 - 65535>] }
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on Source
*               Layer 4 port keyword / number / range.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotSrcl4port(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index)
{

  L7_uint32 argKeyword = 1;
  L7_uint32 argNumber = 1;
  L7_uint32 argStartPort = 1;
  L7_uint32 argEndPort = 2;
  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;
  L7_uint32 port = 0;
  L7_uint32 startPort = 0;
  L7_uint32 endPort = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strKeyword[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg > 2 || numArg < 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotSrcL4Port);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if ((numArg == 1) && (cliCheckIfInteger((L7_char8 *)argv[index+argKeyword]) != L7_SUCCESS))
  {
    if (strlen(argv[index+argKeyword]) >= sizeof(strKeyword))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidKeyword);
    }
    OSAPI_STRNCPY_SAFE(strKeyword, argv[index + argKeyword]);
    cliConvertToLowerCase(strKeyword);

    if (strcmp(strKeyword, pStrInfo_qos_Domain_1) == 0)
    {
      /* domain */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_DOMAIN;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Echo) == 0)
    {
      /* echo */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_ECHO;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Ftp) == 0)
    {
      /* ftp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Ftpdata) == 0)
    {
      /* ftpdata */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTPDATA;
    }
    else if ((strcmp(strKeyword, pStrInfo_common_Http_1) == 0) || (strcmp(strKeyword, pStrInfo_qos_Www) == 0))
    {
      /* http or www (both have same value) */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_HTTP;
    }
    else if (strcmp(strKeyword, pStrInfo_qos_Smtp) == 0)
    {
      /* smtp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SMTP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Snmp_1) == 0)
    {
      /* snmp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SNMP;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Telnet) == 0)
    {
      /* telnet */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TELNET;
    }
    else if (strcmp(strKeyword, pStrInfo_common_Tftp_1) == 0)
    {
      /* tftp */
      port = L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TFTP;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_InvalidKeyword);
    }

    startPort = port;
    endPort = port;
  }
  else
  {
    /* only a number? */
    if (numArg == 1)
    {

      if (cliConvertTo32BitUnsignedInteger(argv[index+argNumber], &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPortNum);
      }

      if (port > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX) /* Convert routine already verifies non-negative number */
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPortNum);
      }

      startPort = port;
      endPort = port;
    }
    else  /* range */
    {
      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argStartPort], &startPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartPort);
      }

      /* verify if the specified value is in between 0 to 65535 */
      if ((startPort < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN) ||
          (startPort > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPortVal);
      }

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argEndPort], &endPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndPort);
      }

      /* verify if the specified value is in between 0 to 65535 */
      if ((endPort < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN) ||
          (endPort > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPortVal);
      }

      if (endPort < startPort)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndPortMustBeGreaterThanStartPort);
      }
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {

      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the 'exclude' flag*/

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the start port */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcL4PortStartSet(unit, classId, classRuleId,
                                                     startPort) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the end port */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcL4PortEndSet(unit, classId, classRuleId,
                                                   endPort) != L7_SUCCESS)
    {

      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Source MAC Address
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
* @cmdsyntax  class-map config#> match source-address mac <address> <macmask>
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Source
*               MAC address
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchSourceAddressMac(EwsContext ewsContext,
                                             uintf argc,
                                             const L7_char8 * * argv,
                                             uintf index)
{

  L7_uint32 argMacAddr = 1;
  L7_uint32 argMacMask = 2;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMacMask[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 strConvertedMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strConvertedMacMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchSrcAddrMac);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argMacAddr]) >= sizeof(strMacAddr))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacAddrType);
  }
  OSAPI_STRNCPY_SAFE(strMacAddr, argv[index + argMacAddr]);

  /* verify if the specified mac address is valid */
  if (cliConvertMac(strMacAddr, strConvertedMacAddr) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
  }

  if (strlen(argv[index+argMacMask]) >= sizeof(strMacMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacMaskType);
  }

  OSAPI_STRNCPY_SAFE(strMacMask, argv[index + argMacMask]);

  /* verify if the specified mac mask is valid */
  if (cliConvertMac(strMacMask, strConvertedMacMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacMaskType);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the MAC Address */
    if (usmDbDiffServClassRuleMatchSrcMacAddrSet(unit, classId, classRuleId,
                                                 strConvertedMacAddr) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the MAC Mask */
    if (usmDbDiffServClassRuleMatchSrcMacMaskSet(unit, classId, classRuleId,
                                                 strConvertedMacMask) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on Source MAC Address
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
* @cmdsyntax  class-map config#> match not source address mac <macaddr> <macmask>
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on Source
*               MAC address
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotSourceAddressMac(EwsContext ewsContext,
                                                uintf argc,
                                                const L7_char8 * * argv,
                                                uintf index)
{

  L7_uint32 argMacAddr = 1;
  L7_uint32 argMacMask = 2;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMacMask[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 strConvertedMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strConvertedMacMask[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_MatchNotSrcAddrMac);
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (strlen(argv[index+argMacAddr]) >= sizeof(strMacAddr))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacAddrType);
  }
  OSAPI_STRNCPY_SAFE(strMacAddr, argv[index + argMacAddr]);

  /* verify if the specified mac address is valid */
  if (cliConvertMac(strMacAddr, strConvertedMacAddr) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
  }

  if (strlen(argv[index+argMacMask]) >= sizeof(strMacMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacMaskType);
  }

  OSAPI_STRNCPY_SAFE(strMacMask, argv[index + argMacMask]);

  /* verify if the specified mac mask is valid */
  if (cliConvertMac(strMacMask, strConvertedMacMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_IncorrectMacMaskType);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the 'exclude' flag*/

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  }

  /* set the MAC Address */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcMacAddrSet(unit, classId, classRuleId,
                                                 strConvertedMacAddr) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* set the MAC Mask */
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchSrcMacMaskSet(unit, classId, classRuleId,
                                                 strConvertedMacMask) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on VLAN Id
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
* @cmdsyntax  class-map config#> match vlan <1-4094> [<1-4094>]
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on VLAN Id.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchVlan(EwsContext ewsContext,
                                 uintf argc,
                                 const L7_char8 * * argv,
                                 uintf index)
{

  L7_uint32 argVlan = 1;
  L7_uint32 argStartVlan = 1;
  L7_uint32 argEndVlan = 2;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;
  L7_uint32 vlan, startVlan, endVlan;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;


  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
  {
    if (numArg > 2 || numArg < 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_MatchVlan,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }
  }
  else
  {
    /* check number of arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_MatchVlanNoRange,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (numArg == 1)
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlan], &vlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlanId_1);
    }

    if ((vlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (vlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    startVlan = vlan;
    endVlan = vlan;
  }
  else  /* range */
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argStartVlan], &startVlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartVlanId);
    }

    if ((startVlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (startVlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValStartVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argEndVlan], &endVlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndVlanId);
    }

    if ((endVlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (endVlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValEndVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    if (endVlan < startVlan)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndVlanIdMustBeGreaterThanStartVlanId);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  }
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the start vlan id */
    if (usmDbDiffServClassRuleMatchVlanIdStartSet(unit, classId, classRuleId,
                                                  startVlan) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the end vlan id */
    if (usmDbDiffServClassRuleMatchVlanIdEndSet(unit, classId, classRuleId,
                                                endVlan) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on VLAN Id
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
* @cmdsyntax  class-map config#> match not vlan <1-4094> [<1-4094>]
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on VLAN Id.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotVlan(EwsContext ewsContext,
                                    uintf argc,
                                    const L7_char8 * * argv,
                                    uintf index)
{

  L7_uint32 argVlan = 1;
  L7_uint32 argStartVlan = 1;
  L7_uint32 argEndVlan = 2;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;
  L7_uint32 vlan, startVlan, endVlan;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;


  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
  {
    if (numArg > 2 || numArg < 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_MatchNotVlan,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }
  }
  else
  {
    /* check number of arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_MatchNotVlanNoRange,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (numArg == 1)
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlan], &vlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlanId_1);
    }

    if ((vlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (vlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    startVlan = vlan;
    endVlan = vlan;
  }
  else  /* range */
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argStartVlan], &startVlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartVlanId);
    }

    if ((startVlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (startVlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValStartVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argEndVlan], &endVlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndVlanId);
    }

    if ((endVlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (endVlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValEndVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    if (endVlan < startVlan)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndVlanIdMustBeGreaterThanStartVlanId);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  }
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the start vlan id */
    if (usmDbDiffServClassRuleMatchVlanIdStartSet(unit, classId, classRuleId,
                                                  startVlan) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the end vlan id */
    if (usmDbDiffServClassRuleMatchVlanIdEndSet(unit, classId, classRuleId,
                                                endVlan) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a match condition based on Secondary VLAN Id
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
* @cmdsyntax  class-map config#> match secondary-vlan <1-4094> [<1-4094>]
*
* @cmdhelp
*
* @cmddescript  Adds a match criteria to a class based on Secondary VLAN Id.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchSecondaryVlan(EwsContext ewsContext,
                                          uintf argc,
                                          const L7_char8 * * argv,
                                          uintf index)
{

  L7_uint32 argVlan = 1;
  L7_uint32 argStartVlan = 1;
  L7_uint32 argEndVlan = 2;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;
  L7_uint32 vlan, startVlan, endVlan;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;


  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
  {
    if (numArg > 2 || numArg < 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_MatchSecondaryVlan,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }
  }
  else
  {
    /* check number of arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_MatchSecondaryVlanNoRange,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (numArg == 1)
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlan], &vlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlan2Id);
    }

    if ((vlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (vlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    startVlan = vlan;
    endVlan = vlan;
  }
  else  /* range */
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argStartVlan], &startVlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartVlan2Id);
    }

    if ((startVlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (startVlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValStartVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argEndVlan], &endVlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndVlan2Id);
    }

    if ((endVlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (endVlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValEndVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    if (endVlan < startVlan)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndVlan2IdMustBeGreaterThanStartVlan2Id);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* reset the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  }
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the start vlan id */
    if (usmDbDiffServClassRuleMatchVlanId2StartSet(unit, classId, classRuleId,
                                                   startVlan) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* set the end vlan id */
    if (usmDbDiffServClassRuleMatchVlanId2EndSet(unit, classId, classRuleId,
                                                 endVlan) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Adds a not match condition based on VLAN Id
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
* @cmdsyntax  class-map config#> match not vlan <1-4094> [<1-4094>]
*
* @cmdhelp
*
* @cmddescript  Adds a not match criteria to a class based on VLAN Id.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMatchNotSecondaryVlan(EwsContext ewsContext,
                                             uintf argc,
                                             const L7_char8 * * argv,
                                             uintf index)
{

  L7_uint32 argVlan = 1;
  L7_uint32 argStartVlan = 1;
  L7_uint32 argEndVlan = 2;

  L7_uint32 classId = 0;
  L7_uint32 classRuleId = 0;
  L7_uint32 vlan, startVlan, endVlan;

  L7_RC_t check;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;


  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
  {
    if (numArg > 2 || numArg < 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_MatchNotSecondaryVlan,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }
  }
  else
  {
    /* check number of arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_MatchNotSecondaryVlanNoRange,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }
  }

  OSAPI_STRNCPY_SAFE(strName,EWSCLASSMAP(ewsContext));

  if (numArg == 1)
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlan], &vlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlan2Id);
    }

    if ((vlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (vlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    startVlan = vlan;
    endVlan = vlan;
  }
  else  /* range */
  {
    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argStartVlan], &startVlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidStartVlan2Id);
    }

    if ((startVlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (startVlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValStartVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argEndVlan], &endVlan) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidEndVlan2Id);
    }

    if ((endVlan < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) ||
        (endVlan > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_ValEndVlanIdMustBeFromTo, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
    }

    if (endVlan < startVlan)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_EndVlan2IdMustBeGreaterThanStartVlan2Id);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* find the index of the specified class */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* find the free index in the class rule table*/
    if (usmDbDiffServClassRuleIndexNext(unit, classId, &classRuleId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    if (0 == classRuleId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMoreCriteria);
    }

    /* create the row entry in the class rule table */
    if (usmDbDiffServClassRuleCreate(unit, classId, classRuleId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /* specify the entry type format */
  entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeSet(unit, classId, classRuleId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the 'exclude' flag*/
    check = usmDbDiffServClassRuleMatchExcludeFlagSet(unit, classId, classRuleId,
                                                      L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE);

    if (check != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

  }
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the start vlan id */
    if (usmDbDiffServClassRuleMatchVlanId2StartSet(unit, classId, classRuleId,
                                                   startVlan) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
    /* set the end vlan id */
    if (usmDbDiffServClassRuleMatchVlanId2EndSet(unit, classId, classRuleId,
                                                 endVlan) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }

    /* make sure row status becomes active */
    usmDbDiffServClassRuleRowStatusGet(unit, classId, classRuleId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        usmDbDiffServClassRuleDelete(unit, classId, classRuleId);
      }
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AddMatchCriteria_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Renames a DiffServ Class
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
* @cmdsyntax  config#> class-map rename <classname> <newclassname>
*
* @cmdhelp
*
* @cmddescript  Renames a DiffServ class, now onwards identified by the
*               new entered name.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandClassMapRename(EwsContext ewsContext,
                                      uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index)
{
  L7_uint32 argName = 1;
  L7_uint32 argNewName = 2;

  L7_uint32 classId = 0;
  L7_uint32 newClassId = 0;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strNewName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  L7_uint32 numArg;

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ClassMapRename_1);
  }

  /* verify the length of <classname> argument */
  if (strlen(argv[index+ argName]) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ClassNameTooLong);
  }

  OSAPI_STRNCPY_SAFE(strName,argv[index+ argName]);

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strName) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_ClassNameMustBeAlnum);
  }

  /* verify the length of <newclassname> argument */
  if (strlen(argv[index+ argNewName]) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NewClassNameLen);
  }

  OSAPI_STRNCPY_SAFE(strNewName,argv[index+ argNewName]);

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strNewName) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_NewClassNameMustBeAlnum);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* get the index of class from class name */
    if (usmDbDiffServClassNameToIndex(unit, strName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    }

    /* Verify if the new class name already exists or not */
    if (usmDbDiffServClassNameToIndex(unit, strNewName, &newClassId) == L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassExists);
    }

    /* set the new name of the class */
    if (usmDbDiffServClassNameSet(unit, classId, strNewName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_ClassRenameFailed);
    }

    /* save name */
    OSAPI_STRNCPY_SAFE(EWSCLASSMAP(ewsContext),strNewName);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the class of service / IP DSCP / IP Precedence mark value
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
* @notes   meaningful for a policy of type 'in'
*
* @cmdsyntax  mark { cos <0-7> | secondary-cos <0-7> | ip-dscp <dscpval> | ip-precedence <0-7> }
*
* @cmdhelp
*
* @cmddescript  Sets the class of service mark value for the specified
*               class instance with in the specified policy.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandMark(EwsContext ewsContext,
                            uintf argc,
                            const L7_char8 * * argv,
                            uintf index)
{

  L7_uint32 argPrecedence = 2;
  L7_uint32 precedence;
  L7_uint32 argDscp = 2;
  L7_uint32 dscp;
  L7_char8 strDscp[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argCos = 2;
  L7_uint32 argCos2 = 2;
  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 policyAttrId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;
  L7_uint32 cos = 0, cos2 = 0;

  L7_RC_t check;

  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t classL3Proto = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE;
  L7_uint32 unit;

  L7_uint32 numArg;

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (((strcmp(argv[index+1],pStrInfo_qos_MacAclCos) != 0) && 
                                     (strcmp(argv[index+1],pStrInfo_qos_MacAclCosAsCos2) != 0) && 
                                     (strcmp(argv[index+1],pStrInfo_qos_MacAclSecondaryCos) != 0) && 
                                     (strcmp(argv[index+1],pStrErr_qos_CfgCosTrustIpDscp) != 0) && 
                                     (strcmp(argv[index+1],pStrErr_qos_CfgCosTrustIpPrecedence) != 0)))
  {
    /* If only entered "mark cos", "mark secondary-cos", "mark cos-as-sec-cos", "mark ip-dscp", or "mark ip-precedence",
     * prompt with command specific syntax help
     */
    if ((strcmp(argv[index+1],pStrInfo_qos_MacAclCos) == 0) && (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,pStrErr_qos_MarkCos_1);
    }
    else if ((strcmp(argv[index+1],pStrInfo_qos_MacAclCosAsCos2) == 0) /*&& (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE)*/)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,pStrErr_qos_MarkCosAsInCtag_1);
    }
    else if ((strcmp(argv[index+1],pStrInfo_qos_MacAclSecondaryCos) == 0) && (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_TRUE))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,pStrErr_qos_MarkSecondaryCos_1);
    }
    else if ((strcmp(argv[index+1],pStrErr_qos_CfgCosTrustIpDscp) == 0) && (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_MarkIpDscp_1);
    }
    else if ((strcmp(argv[index+1],pStrErr_qos_CfgCosTrustIpPrecedence) == 0) && (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_MarkIpPrecedence_1);
    }
    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_FALSE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,pStrErr_qos_MarkCosNot);
    }
    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_FALSE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,pStrErr_qos_MarkCosAsInCtagNot);
    }
    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_FALSE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,pStrErr_qos_MarkSecondaryCosNot);
    }
    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_FALSE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_MarkIpDscpNot);
    }
    else if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_FALSE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_MarkIpPrecedenceNot);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_Mark);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    OSAPI_STRNCPY_SAFE(strPolicyName,EWSPOLICYNAME(ewsContext));
    OSAPI_STRNCPY_SAFE(strClassName,EWSSERVICENAME(ewsContext));

    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }

    if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservClassDoesntExist);
    }
  }

  if (strcmp(argv[index+1],pStrInfo_qos_MacAclCos) == 0)
  {

    /* second argument validation */

    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argCos], &cos) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosVal);
    }

    /* verify if the specified value is in between 0 to 7*/
    if ((cos < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
        (cos > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* get the policy type */
      if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,  pStrErr_qos_CfgureCosMarkVal);
      }

      /* verify if this class is associated with the specified policy or not */
      if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                                &policyInstId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
      }

      prevPolicyId = policyId;
      prevPolicyInstId = policyInstId;
      prevPolicyAttrId = 0; /* to start the search */

      check = usmDbDiffServPolicyAttrGetNext(unit,
                                             prevPolicyId,
                                             prevPolicyInstId,
                                             prevPolicyAttrId,
                                             &nextPolicyId,
                                             &nextPolicyInstId,
                                             &nextPolicyAttrId);

      /* traverse the policy Attribute table to find whether the given attribute
         type can be specified or not */
      while (check != L7_ERROR)
      {

        if (check == L7_SUCCESS)
        {
          if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
          {
            /* get the attribute type of that row*/
            if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                        nextPolicyInstId,
                                                        nextPolicyAttrId,
                                                        &entryType) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,  pStrErr_qos_CfgureCosMarkVal);
            }

            /* No Incompatabilities */

            /* check if cos value is already specified */
            if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL)
            {
              /*******Check if the Flag is Set for Execution*************/
              if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
              {
                /* modify the cos value */
                if (usmDbDiffServPolicyAttrStmtMarkCosValSet(unit,
                                                             nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                             cos) != L7_SUCCESS)
                {
                  return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
                }
              }

              return cliPrompt(ewsContext);
            }

            prevPolicyId = nextPolicyId; /* should be equal to policyId */
            prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

            prevPolicyAttrId = nextPolicyAttrId;

          }
          else
          {
            /* all the attributes had been scanned for the specified policy
               and class combination, this attribute is not yet specified so
               create a row entry in the table and configure this attribute */
            break;
          }

          /* determine next sequential row entry in the policy attribute table */
          check = usmDbDiffServPolicyAttrGetNext(unit,
                                                 prevPolicyId,
                                                 prevPolicyInstId,
                                                 prevPolicyAttrId,
                                                 &nextPolicyId,
                                                 &nextPolicyInstId,
                                                 &nextPolicyAttrId);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
        }
      }

      /* find the free index in the policy attribute table*/
      if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                           &policyAttrId ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }
      if (0 == policyAttrId)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }
    }

    /* specify the attribute type */
    entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL;

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                  policyInstId,
                                                  policyAttrId,
                                                  entryType) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }
    }

    /* set the cos value */
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServPolicyAttrStmtMarkCosValSet(unit,policyId,
                                                   policyInstId,
                                                   policyAttrId,
                                                   cos) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }

      /* make sure row status becomes active */
      usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                          policyAttrId, &status);
      if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }
    }
  }

  else if (strcmp(argv[index+1],pStrInfo_qos_MacAclCosAsCos2) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* get the policy type */
      if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }

      /* verify if this class is associated with the specified policy or not */
      if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                                &policyInstId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
      }

      prevPolicyId = policyId;
      prevPolicyInstId = policyInstId;
      prevPolicyAttrId = 0; /* to start the search */

      check = usmDbDiffServPolicyAttrGetNext(unit,
                                             prevPolicyId,
                                             prevPolicyInstId,
                                             prevPolicyAttrId,
                                             &nextPolicyId,
                                             &nextPolicyInstId,
                                             &nextPolicyAttrId);

      /* traverse the policy Attribute table to find whether the given attribute
         type can be specified or not */
      while (check != L7_ERROR)
      {
        if (check == L7_SUCCESS)
        {
          if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
          {
            /* get the attribute type of that row*/
            if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                        nextPolicyInstId,
                                                        nextPolicyAttrId,
                                                        &entryType) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,  pStrErr_qos_CfgureCosMarkVal);
            }

            /* No Incompatabilities */

            /* check if cos value (as secondary cos) is already specified */
            if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2)
            {
              /*******Check if the Flag is Set for Execution*************/
              if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
              {
                /* modify the cos value */
                if (usmDbDiffServPolicyAttrStmtMarkCosAsCos2Set(unit,
                                                             nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                             L7_TRUE) != L7_SUCCESS)
                {
                  return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
                }
              }

              return cliPrompt(ewsContext);
            }

            prevPolicyId = nextPolicyId; /* should be equal to policyId */
            prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

            prevPolicyAttrId = nextPolicyAttrId;

          }
          else
          {
            /* all the attributes had been scanned for the specified policy
               and class combination, this attribute is not yet specified so
               create a row entry in the table and configure this attribute */
            break;
          }

          /* determine next sequential row entry in the policy attribute table */
          check = usmDbDiffServPolicyAttrGetNext(unit,
                                                 prevPolicyId,
                                                 prevPolicyInstId,
                                                 prevPolicyAttrId,
                                                 &nextPolicyId,
                                                 &nextPolicyInstId,
                                                 &nextPolicyAttrId);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
        }
      }

      /* find the free index in the policy attribute table*/
      if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                           &policyAttrId ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }
      if (0 == policyAttrId)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }
    }

    /* specify the attribute type */
    entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2;

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                  policyInstId,
                                                  policyAttrId,
                                                  entryType) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }
    }

    /* set the cos (as secondary cos) value */
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServPolicyAttrStmtMarkCosAsCos2Set(unit,policyId,
                                                   policyInstId,
                                                   policyAttrId,
                                                   L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }

      /* make sure row status becomes active */
      usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                          policyAttrId, &status);
      if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_CfgureCosMarkVal);
      }
    }
  }

  else if (strcmp(argv[index+1],pStrInfo_qos_MacAclSecondaryCos) == 0)
  {

    /* second argument validation */

    /* verify if the specified  argument is an integer */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argCos2], &cos2) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidSecondaryCosVal);
    }

    /* verify if the specified value is in between 0 to 7*/
    if ((cos < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
        (cos > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* get the policy type */
      if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,  pStrErr_qos_CfgureSecondaryCosMarkVal);
      }

      /* verify if this class is associated with the specified policy or not */
      if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                                &policyInstId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
      }

      prevPolicyId = policyId;
      prevPolicyInstId = policyInstId;
      prevPolicyAttrId = 0; /* to start the search */

      check = usmDbDiffServPolicyAttrGetNext(unit,
                                             prevPolicyId,
                                             prevPolicyInstId,
                                             prevPolicyAttrId,
                                             &nextPolicyId,
                                             &nextPolicyInstId,
                                             &nextPolicyAttrId);

      /* traverse the policy Attribute table to find whether the given attribute
         type can be specified or not */
      while (check != L7_ERROR)
      {

        if (check == L7_SUCCESS)
        {
          if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
          {
            /* get the attribute type of that row*/
            if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                        nextPolicyInstId,
                                                        nextPolicyAttrId,
                                                        &entryType) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,  pStrErr_qos_CfgureSecondaryCosMarkVal);
            }

            /* No Incompatabilities */

            /* check if cos value is already specified */
            if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL)
            {
              /*******Check if the Flag is Set for Execution*************/
              if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
              {
                /* modify the cos value */
                if (usmDbDiffServPolicyAttrStmtMarkCos2ValSet(unit,
                                                              nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                              cos) != L7_SUCCESS)
                {
                  return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSecondaryCosMarkVal);
                }
              }

              return cliPrompt(ewsContext);
            }

            prevPolicyId = nextPolicyId; /* should be equal to policyId */
            prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

            prevPolicyAttrId = nextPolicyAttrId;

          }
          else
          {
            /* all the attributes had been scanned for the specified policy
               and class combination, this attribute is not yet specified so
               create a row entry in the table and configure this attribute */
            break;
          }

          /* determine next sequential row entry in the policy attribute table */
          check = usmDbDiffServPolicyAttrGetNext(unit,
                                                 prevPolicyId,
                                                 prevPolicyInstId,
                                                 prevPolicyAttrId,
                                                 &nextPolicyId,
                                                 &nextPolicyInstId,
                                                 &nextPolicyAttrId);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSecondaryCosMarkVal);
        }
      }

      /* find the free index in the policy attribute table*/
      if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                           &policyAttrId ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSecondaryCosMarkVal);
      }
      if (0 == policyAttrId)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSecondaryCosMarkVal);
      }
    }

    /* specify the attribute type */
    entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL;

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                  policyInstId,
                                                  policyAttrId,
                                                  entryType) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSecondaryCosMarkVal);
      }
    }

    /* set the cos value */
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServPolicyAttrStmtMarkCos2ValSet(unit,policyId,
                                                    policyInstId,
                                                    policyAttrId,
                                                    cos) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSecondaryCosMarkVal);
      }

      /* make sure row status becomes active */
      usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                          policyAttrId, &status);
      if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        }
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSecondaryCosMarkVal);
      }
    }
  }

  else if (strcmp(argv[index+1],pStrErr_qos_CfgCosTrustIpDscp) == 0)
  {
    /* second argument validation */
    if (strlen(argv[index+argDscp]) >= sizeof(strDscp))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDscpVal);
    }
    OSAPI_STRNCPY_SAFE(strDscp,argv[index+ argDscp]);

    /* handle dscp as keyword or integer and convert to integer */
    if (cliDiffservConvertDSCPStringToVal(strDscp, &dscp) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDscpVal);
    }

    /* verify if the specified value is in between 0 to 63*/
    if ((dscp < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
        (dscp > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectDscpVal);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* get the policy type */
      if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDscpMarkVal);
      }

      /* verify if this class is associated with the specified policy or not */
      if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                                &policyInstId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
      }

      prevPolicyId = policyId;
      prevPolicyInstId = policyInstId;
      prevPolicyAttrId = 0; /* to start the search */

      check = usmDbDiffServPolicyAttrGetNext(unit,
                                             prevPolicyId,
                                             prevPolicyInstId,
                                             prevPolicyAttrId,
                                             &nextPolicyId,
                                             &nextPolicyInstId,
                                             &nextPolicyAttrId);

      /* traverse the policy Attribute table to find whether the given attribute
         type can be specified or not */
      while (check != L7_ERROR)
      {
        if (check == L7_SUCCESS)
        {
          if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
          {
            /* get the attribute type of that row*/
            if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                        nextPolicyInstId,
                                                        nextPolicyAttrId,
                                                        &entryType) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDscpMarkVal);
            }

            /* check for Incompatabilities */

            if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
                (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) ||
                (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureDscpMarkWithPolice);
            }

            if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureDscpMarkWithIpPrec);
            }

            /* check if dscp value is already specified */
            if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
            {
              /*******Check if the Flag is Set for Execution*************/
              if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
              {
                /* modify the dscp value */
                if (usmDbDiffServPolicyAttrStmtMarkIpDscpValSet(unit,
                                                                nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                dscp) != L7_SUCCESS)
                {
                  return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDscpMarkVal);
                }
              }

              return cliPrompt(ewsContext);
            }

            prevPolicyId = nextPolicyId; /* should be equal to policyId */
            prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

            prevPolicyAttrId = nextPolicyAttrId;

          }
          else
          {
            /* all the attributes had been scanned for the specified policy
               and class combination, this attribute is not yet specified so
               create a row entry in the table and configure this attribute */
            break;
          }

          /* determine next sequential row entry in the policy attribute table */
          check = usmDbDiffServPolicyAttrGetNext(unit,
                                                 prevPolicyId,
                                                 prevPolicyInstId,
                                                 prevPolicyAttrId,
                                                 &nextPolicyId,
                                                 &nextPolicyInstId,
                                                 &nextPolicyAttrId);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDscpMarkVal);
        }
      }

      /* find the free index in the policy attribute table*/
      if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                           &policyAttrId ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDscpMarkVal);
      }
      if (0 == policyAttrId)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDscpMarkVal);
      }
    }

    /* specify the attribute type */
    entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL;

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                  policyInstId,
                                                  policyAttrId,
                                                  entryType) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDscpMarkVal);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* set the dscp value */
      if (usmDbDiffServPolicyAttrStmtMarkIpDscpValSet(unit,policyId,
                                                      policyInstId,
                                                      policyAttrId,
                                                      dscp) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDscpMarkVal);
      }
      /* make sure row status becomes active */
      usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                          policyAttrId, &status);
      if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        }
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_CfgureDscpMarkVal);
      }
    }
  }
  else if (strcmp(argv[index+1],pStrErr_qos_CfgCosTrustIpPrecedence) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* check if class is an IPv6 class, mark ip precedence is not appropriate for IPv6 traffic */
      if (usmDbDiffServClassL3ProtoGet(unit, classId, &classL3Proto) == L7_SUCCESS)
      {
        if (L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6 != classL3Proto)
        {
          /* second argument validation */
          if ((cliConvertTo32BitUnsignedInteger(argv[index+argPrecedence], &precedence ) != L7_SUCCESS ) ||
              (precedence > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPPRECEDENCE_MAX ))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPrecedenceVal);
          }

          /* get the policy type */
          if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePrecedenceMarkVal);
          }

          /* verify if this class is associated with the specified policy or not */
          if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                                    &policyInstId) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AttrNotMeaningfulForIPv6Class);
        }
      }

      prevPolicyId = policyId;
      prevPolicyInstId = policyInstId;
      prevPolicyAttrId = 0; /* to start the search */

      check = usmDbDiffServPolicyAttrGetNext(unit,
                                             prevPolicyId,
                                             prevPolicyInstId,
                                             prevPolicyAttrId,
                                             &nextPolicyId,
                                             &nextPolicyInstId,
                                             &nextPolicyAttrId);

      /* traverse the policy Attribute table to find whether the given attribute
         type can be specified or not */
      while (check != L7_ERROR)
      {

        if (check == L7_SUCCESS)
        {
          if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
          {
            /* get the attribute type of that row*/
            if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                        nextPolicyInstId,
                                                        nextPolicyAttrId,
                                                        &entryType) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePrecedenceMarkVal);
            }

            /* check for Incompatabilities */
            if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
                (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) ||
                (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgurePrecWithPolice);
            }

            if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgurePrecWithIpDscp);
            }

            /* check if precedence value is already specified */
            if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
            {
              /*******Check if the Flag is Set for Execution*************/
              if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
              {
                /* modify the precedence value */
                if (usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValSet(unit,
                                                                      nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                      precedence) != L7_SUCCESS)
                {

                  return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePrecedenceMarkVal);
                }
              }

              return cliPrompt(ewsContext);
            }

            prevPolicyId = nextPolicyId; /* should be equal to policyId */
            prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

            prevPolicyAttrId = nextPolicyAttrId;

          }
          else
          {
            /* all the attributes had been scanned for the specified policy
               and class combination, this attribute is not yet specified so
               create a row entry in the table and configure this attribute */
            break;
          }

          /* determine next sequential row entry in the policy attribute table */
          check = usmDbDiffServPolicyAttrGetNext(unit,
                                                 prevPolicyId,
                                                 prevPolicyInstId,
                                                 prevPolicyAttrId,
                                                 &nextPolicyId,
                                                 &nextPolicyInstId,
                                                 &nextPolicyAttrId);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePrecedenceMarkVal);
        }
      }

      /* find the free index in the policy attribute table*/
      if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                           &policyAttrId ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePrecedenceMarkVal);
      }
      if (0 == policyAttrId)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePrecedenceMarkVal);
      }
    }

    /* specify the attribute type */
    entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL;

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                  policyInstId,
                                                  policyAttrId,
                                                  entryType) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePrecedenceMarkVal);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* set the precedence value */
      if (usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValSet(unit,policyId,
                                                            policyInstId,
                                                            policyAttrId,
                                                            precedence) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePrecedenceMarkVal);
      }

      /* make sure row status becomes active */
      usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                          policyAttrId, &status);
      if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        }
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_CfgurePrecedenceMarkVal);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Base function for setting policy police conform
*
*
* @param EwsContext ewsContext
* @param strPolicyName
* @param strClassName
* @param actionType
* @param markVal (only necessary if action is markdscp or markprec)
*
* @returntype const L7_char8
*
* @returns void
*
* @notes
*********************************************************************/
void cliConfigDiffservPolicyPoliceConform(EwsContext ewsContext, L7_char8 * strPolicyName,
                                          L7_char8 * strClassName,
                                          L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t actionType,
                                          L7_uint32 markVal)
{
  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;

  L7_RC_t check;

  L7_uint32 unit;

  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if this policy already exists or not */
  if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservPolicyDoesntExist);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if this class already exists or not */
  if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if this class is associated with the specified policy or not */
  if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                            &policyInstId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    cliSyntaxBottom(ewsContext);
    return;
  }

  prevPolicyId = policyId;
  prevPolicyInstId = policyInstId;
  prevPolicyAttrId = 0; /* to start the search */

  check = usmDbDiffServPolicyAttrGetNext(unit,
                                         prevPolicyId,
                                         prevPolicyInstId,
                                         prevPolicyAttrId,
                                         &nextPolicyId,
                                         &nextPolicyInstId,
                                         &nextPolicyAttrId);

  /* traverse the policy Attribute table to find whether the given attribute
     type can be specified or not */
  while (check != L7_ERROR)
  {
    if (check == L7_SUCCESS)
    {
      if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
      {
        /* get the attribute type of that row*/
        if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                    nextPolicyInstId,
                                                    nextPolicyAttrId,
                                                    &entryType) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureConformingAction);
          cliSyntaxBottom(ewsContext);
          return;
        }

        /* check for necessary conditions*/

        if (( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
            (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) ||
            (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
        {

          /* modify the action */
          if (usmDbDiffServPolicyAttrStmtPoliceConformActSet(unit,
                                                             nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                             actionType) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureConformingAction);
            cliSyntaxBottom(ewsContext);
            return;
          }

          if (actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2 ||              
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
          {
            if (usmDbDiffServPolicyAttrStmtPoliceConformValSet(unit,
                                                               nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                               markVal) != L7_SUCCESS)
            {
              ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureConformingAction);
              cliSyntaxBottom(ewsContext);
              return;
            }
          }

          return;

        }

        prevPolicyId = nextPolicyId; /* should be equal to policyId */
        prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

        prevPolicyAttrId = nextPolicyAttrId;
      }
      else
      {
        /* all the attributes had been scanned for the specified policy
           and class combination, this attribute is not yet specified */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_SpecifyTrafficPolicyForConformAction);
        cliSyntaxBottom(ewsContext);
        return;
      }

      /* determine next sequential row entry in the policy attribute table */
      check = usmDbDiffServPolicyAttrGetNext(unit,
                                             prevPolicyId,
                                             prevPolicyInstId,
                                             prevPolicyAttrId,
                                             &nextPolicyId,
                                             &nextPolicyInstId,
                                             &nextPolicyAttrId);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureConformingAction);
      cliSyntaxBottom(ewsContext);
      return;
    }
  }

  /* if there is no entry in the table */
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_SpecifyTrafficPolicyForConformAction);
  cliSyntaxBottom(ewsContext);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return;
}

/*********************************************************************
*
* @purpose  Base function for setting policy police exceed
*
*
* @param EwsContext ewsContext
* @param strPolicyName
* @param strClassName
* @param actionType
* @param markVal (only necessary if action is markdscp or markprec)
*
* @returntype const L7_char8
*
* @returns void
*
* @notes
*********************************************************************/
void cliConfigDiffservPolicyPoliceExceed(EwsContext ewsContext, L7_char8 * strPolicyName,
                                         L7_char8 * strClassName,
                                         L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t actionType,
                                         L7_uint32 markVal)
{
  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;

  L7_RC_t check;

  L7_uint32 unit;

  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strPolicyName) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_PolicyNameAlnum);
    cliSyntaxBottom(ewsContext);
    return;
  }
  /* verify if this policy already exists or not */
  if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservPolicyDoesntExist);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strClassName) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_ClassNameMustBeAlnum);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if this class already exists or not */
  if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if this class is associated with the specified policy or not */
  if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                            &policyInstId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    cliSyntaxBottom(ewsContext);
    return;
  }

  prevPolicyId = policyId;
  prevPolicyInstId = policyInstId;
  prevPolicyAttrId = 0; /* to start the search */

  check = usmDbDiffServPolicyAttrGetNext(unit,
                                         prevPolicyId,
                                         prevPolicyInstId,
                                         prevPolicyAttrId,
                                         &nextPolicyId,
                                         &nextPolicyInstId,
                                         &nextPolicyAttrId);

  /* traverse the policy Attribute table to find whether the given attribute
     type can be specified or not */
  while (check != L7_ERROR)
  {

    if (check == L7_SUCCESS)
    {
      if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
      {
        /* get the attribute type of that row*/
        if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                    nextPolicyInstId,
                                                    nextPolicyAttrId,
                                                    &entryType) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureExcessTrafficAction);
          cliSyntaxBottom(ewsContext);
          return;
        }

        /* check for necessary conditions*/

        if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) ||
            (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
        {

          /* modify the action */
          if (usmDbDiffServPolicyAttrStmtPoliceExceedActSet(unit,
                                                            nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                            actionType) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureExcessTrafficAction);
            cliSyntaxBottom(ewsContext);
            return;
          }

          if (actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2 ||              
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
          {
            /* set mark value */
            if (usmDbDiffServPolicyAttrStmtPoliceExceedValSet(unit,
                                                              nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                              markVal) != L7_SUCCESS)
            {
              ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureExcessTrafficAction);
              cliSyntaxBottom(ewsContext);
              return;
            }
          }

          return;

        }

        prevPolicyId = nextPolicyId; /* should be equal to policyId */
        prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

        prevPolicyAttrId = nextPolicyAttrId;
      }
      else
      {
        /* all the attributes had been scanned for the specified policy
           and class combination, this attribute is not yet specified */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_SpecifyTrafficPolicyForExceedAction);
        cliSyntaxBottom(ewsContext);
        return;
      }

      /* determine next sequential row entry in the policy attribute table */
      check = usmDbDiffServPolicyAttrGetNext(unit,
                                             prevPolicyId,
                                             prevPolicyInstId,
                                             prevPolicyAttrId,
                                             &nextPolicyId,
                                             &nextPolicyInstId,
                                             &nextPolicyAttrId);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureExcessTrafficAction);
      cliSyntaxBottom(ewsContext);
      return;
    }
  }

  /* if there is no entry in the table */
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_SpecifyTrafficPolicyForExceedAction);
  cliSyntaxBottom(ewsContext);
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return;

}

/*********************************************************************
*
* @purpose  Base function for setting policy police nonconform
*
*
* @param EwsContext ewsContext
* @param strPolicyName
* @param strClassName
* @param actionType
* @param markVal (only necessary if action is markdscp or markprec)
*
* @returntype const L7_char8
*
* @returns void
*
* @notes
*********************************************************************/
void cliConfigDiffservPolicyPoliceNonconform(EwsContext ewsContext, L7_char8 * strPolicyName,
                                             L7_char8 * strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t actionType,
                                             L7_uint32 markVal)
{
  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;

  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;

  L7_uint32 check;

  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strPolicyName) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_PolicyNameAlnum);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if this policy already exists or not */
  if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservPolicyDoesntExist);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strClassName) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_ClassNameMustBeAlnum);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if this class already exists or not */
  if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassDoesntExist);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* verify if this class is associated with the specified policy or not */
  if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                            &policyInstId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    cliSyntaxBottom(ewsContext);
    return;
  }

  prevPolicyId = policyId;
  prevPolicyInstId = policyInstId;
  prevPolicyAttrId = 0; /* to start the search */

  check = usmDbDiffServPolicyAttrGetNext(unit,
                                         prevPolicyId,
                                         prevPolicyInstId,
                                         prevPolicyAttrId,
                                         &nextPolicyId,
                                         &nextPolicyInstId,
                                         &nextPolicyAttrId);

  /* traverse the policy Attribute table to find whether the given attribute
     type can be specified or not */
  while (check != L7_ERROR)
  {

    if (check == L7_SUCCESS)
    {
      if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
      {
        /* get the attribute type of that row*/
        if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                    nextPolicyInstId,
                                                    nextPolicyAttrId,
                                                    &entryType) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureNonConformingAction);
          cliSyntaxBottom(ewsContext);
          return;
        }

        /* check for necessary conditions*/

        if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
            (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) ||
            (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
        {

          /* modify the action */
          if (usmDbDiffServPolicyAttrStmtPoliceNonconformActSet(unit,
                                                                nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                actionType) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureNonConformingAction);
            cliSyntaxBottom(ewsContext);
            return;
          }

          /* set dscp value if action is markdscp */
          if (actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2 ||
              actionType == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
          {
            if (usmDbDiffServPolicyAttrStmtPoliceNonconformValSet(unit,
                                                                  nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                  markVal) != L7_SUCCESS)
            {
              ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureNonConformingAction);
              cliSyntaxBottom(ewsContext);
              return;
            }
          }

          return;

        }

        prevPolicyId = nextPolicyId; /* should be equal to policyId */
        prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

        prevPolicyAttrId = nextPolicyAttrId;
      }
      else
      {
        /* all the attributes had been scanned for the specified policy
           and class combination, this attribute is not yet specified */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_SpecifyTrafficPolicyForNonConformAction);
        cliSyntaxBottom(ewsContext);
        return;
      }

      /* determine next sequential row entry in the policy attribute table */
      check = usmDbDiffServPolicyAttrGetNext(unit,
                                             prevPolicyId,
                                             prevPolicyInstId,
                                             prevPolicyAttrId,
                                             &nextPolicyId,
                                             &nextPolicyInstId,
                                             &nextPolicyAttrId);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureNonConformingAction);
      cliSyntaxBottom(ewsContext);
      return;
    }
  }

  /* if there is no entry in the table */
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_SpecifyTrafficPolicyForNonConformAction);
  cliSyntaxBottom(ewsContext);
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return;
}

/* Color Conform */

/*********************************************************************
*
* @purpose  Sets simple traffic policing action
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
* @notes   1. meaningful for a policy of type 'in'
*          2. Requires police-simple, police-single-rate, or police-two-rate
*               command to have already been issued for this policy-class
*               instance
*
* @cmdsyntax  conform-color <class-map-name> [exceed-color <class-map-name>]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandConformColor(EwsContext ewsContext,
                                    uintf argc,
                                    const L7_char8 * * argv,
                                    uintf index)
{

  L7_uint32 argConformClassMapName = 1;
  L7_uint32 argExceedClassMapName = 3;

  L7_uint32 conformClassId = 0;
  L7_uint32 exceedClassId = 0;
  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;
  L7_uint32 policingPolicyAttrId = 0;

  L7_RC_t rc, check;

  L7_char8 strConformClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strExceedClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;

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

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* check number of arguments */
    if (numArg < 1 || numArg > 3)
    {
      if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_POLICE_SINGLERATE_FEATURE_ID) == L7_TRUE) ||
          (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_POLICE_TWORATE_FEATURE_ID) == L7_TRUE))
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ConformColor);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ConformColorForPoliceSimple);
      }
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* verify the length of <classname> argument */
    if (strlen(argv[index+ argConformClassMapName]) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ClassNameTooLong);
    }

    OSAPI_STRNCPY_SAFE(strConformClassName,argv[index+ argConformClassMapName]);

    /* verify if the specified string contains all the alpha-numeric characters */
    if (cliIsAlphaNum(strConformClassName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_ClassNameMustBeAlnum);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServClassNameToIndex(unit, strConformClassName, &conformClassId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_ConformColorClassDoesntExist);
      }
    }

    if (numArg == 3)
    {
      if (strcmp(argv[index+2],pStrInfo_qos_ExceedColor) == 0)
      {
        /* verify the length of <classname> argument */
        if (strlen(argv[index+ argExceedClassMapName]) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ClassNameTooLong);
        }

        OSAPI_STRNCPY_SAFE(strExceedClassName,argv[index+ argExceedClassMapName]);

        /* verify if the specified string contains all the alpha-numeric characters */
        if (cliIsAlphaNum(strExceedClassName) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_ClassNameMustBeAlnum);
        }

        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbDiffServClassNameToIndex(unit, strExceedClassName, &exceedClassId) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_ExceedColorClassDoesntExist);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ConformColor);
      }
    }
  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* check number of arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_NoConformColor);
    }
  }  /* END OF NO COMMAND */

  OSAPI_STRNCPY_SAFE(strPolicyName,EWSPOLICYNAME(ewsContext));
  OSAPI_STRNCPY_SAFE(strClassName,EWSSERVICENAME(ewsContext));

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }

    if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservClassDoesntExist);
    }

    /* get the policy type */
    if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePolicingColorMode);
    }

    /* verify if this class is associated with the specified policy or not */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    policingPolicyAttrId = 0;

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    check = usmDbDiffServPolicyAttrGetNext(unit,
                                           prevPolicyId,
                                           prevPolicyInstId,
                                           prevPolicyAttrId,
                                           &nextPolicyId,
                                           &nextPolicyInstId,
                                           &nextPolicyAttrId);

    /* traverse the policy Attribute table to find a policing atttribute */
    while (check != L7_ERROR)
    {

      if (check == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId,
                                                      nextPolicyAttrId,
                                                      &entryType) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePolicingColorMode);
          }

          /* one of the policing types must exist for this command to be valid
           * (can assume the first policing attr entry found is the one to use,
           * since there should only be at most one per policy-class instance)
           */

          if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
              (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) ||
              (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
          {
            policingPolicyAttrId = nextPolicyAttrId;
            break;
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

          prevPolicyAttrId = nextPolicyAttrId;
        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination and no policing attribute was found */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        check = usmDbDiffServPolicyAttrGetNext(unit,
                                               prevPolicyId,
                                               prevPolicyInstId,
                                               prevPolicyAttrId,
                                               &nextPolicyId,
                                               &nextPolicyInstId,
                                               &nextPolicyAttrId);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgurePolicingColorMode);
      }
    }
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (policingPolicyAttrId == 0)
    {
      /* all the attributes had been scanned for the specified policy
         and class combination and no policing attribute was found, which
         is a required pre-condition, so display a command error msg */

      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_CfgureColorModeNeedsPolicing);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* validate the conform color class (only checking for L7_ERROR here,
       * since other problems will be caught by the 'set' operation)
       */
      if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(unit,
                                                                   policyId, policyInstId,
                                                                   conformClassId) == L7_ERROR)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ConformColorClassInvalid);
      }
    }

    if (numArg == 3)
    {
      if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_CfgureExceedColorPoliceSimple);
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* validate the exceed color class (only checking for L7_ERROR here,
         * since other problems will be caught by the 'set' operation)
         */
        if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(unit,
                                                                     policyId, policyInstId,
                                                                     exceedClassId) == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ExceedColorClassInvalid);
        }

        /* make sure the conform and exceed color classes do not define the
         * same mode and value (even if different class names are used).
         */
        if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassesAreEquivalent(unit,
                                                                            conformClassId, exceedClassId) == L7_TRUE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ExceedColorSameAsConformColor);
        }
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* before setting the actual color conform and exceed indexes, clear
       * out the existing values by setting the conform index to 0 (color-blind)
       * first -- this prevents false conflict check errors when the new
       * conform index is compared against the old exceed index
       *
       * NOTE:  USMDB automatically sets the exceed index to 0 when
       *        the conform index is set to 0
       */
      rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(unit,
                                                                 policyId, policyInstId, policingPolicyAttrId,
                                                                 0);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureConformColor);
      }

      /* set the conform color class index in the policing policy attr,
       * which will cause the corresponding color mode and value to be set
       * (we just set the index to 0, so no need to set it to 0 again)
       */
      if (conformClassId != 0)
      {
        rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(unit,
                                                                   policyId, policyInstId, policingPolicyAttrId,
                                                                   conformClassId);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureConformColor);
        }
      }

      /* set the exceed color class index in the policing policy attr,
       * which will cause the corresponding color mode and value to be set
       * (only valid for single-rate or two-rate policing)
       *
       * the exceedClassId will be 0 if the exceed color parameter was not
       * specified in the command
       */
      if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) ||
          (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
      {
        rc = usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexSet(unit,
                                                                  policyId, policyInstId, policingPolicyAttrId,
                                                                  exceedClassId);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureExceedColor);
        }
      }
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (policingPolicyAttrId == 0)
    {
      /* all the attributes had been scanned for the specified policy
         and class combination and no policing attribute was found, which
         is a required pre-condition, so display a command error msg */

      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_CfgureColorModeNeedsPolicing);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* set the conform index to 0 for color-blind mode
       *
       * NOTE:  USMDB automatically sets the exceed index to 0 when
       *        the conform index is set to 0
       */
      rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(unit,
                                                                 policyId, policyInstId, policingPolicyAttrId,
                                                                 0);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureColorBlind);
      }
    }
  }  /* END OF NO COMMAND */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets simple traffic policing action
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
* @notes   1. meaningful for a policy of type 'in'
*          2. Incompatible with mark IP DSCP
*
* @cmdsyntax  police simple {
*                              <1-4294967295> <1-128>
*                              |
*                              conform-action
*                                {drop |
*                                set-prec-transmit <0-7> |
*                                set-dscp-transmit <0-63> |
*                                set-cos-transmit <0-7> |
*                                set-sec-cos-transmit <0-7> |
*                                transmit }
*                                [violate-action
*                                  {drop |
*                                  set-prec-transmit <0-7> |
*                                  set-dscp-transmit <0-63> |
*                                  set-cos-transmit <0-7> |
*                                  set-sec-cos-transmit <0-7> |
*                                  transmit }
*                                ]
*                               }
*
*
* @cmdhelp
*
* @cmddescript  sets simple traffic policing action for the specified
*               class instance with in the specified policy.
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandPoliceSimple(EwsContext ewsContext,
                                    uintf argc,
                                    const L7_char8 * * argv,
                                    uintf index)
{
  L7_uint32 argPrecVal = 5;
  L7_uint32 argCosVal = 5;
  L7_uint32 argCos2Val = 5;
  L7_uint32 precVal = 0, cosVal = 0, cos2Val = 0;
  L7_uint32 argDscpVal = 5;
  L7_char8 strDscpVal[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 dscpVal = 0;

  L7_uint32 argViolatePrecVal = 0;
  L7_uint32 violatePrecVal = 0;
  L7_uint32 argViolateDscpVal = 0;
  L7_uint32 argViolateCosVal = 0;
  L7_uint32 argViolateCos2Val = 0;
  L7_char8 strViolateDscpVal[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 violateDscpVal = 0, violateCosVal = 0, violateCos2Val = 0;

  L7_uint32 argDataRate = 1;
  L7_uint32 argBurstSize = 2;

  L7_uint32 argViolateAction = 0;

  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 policyAttrId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;
  L7_uint32 dataRate = 0;
  L7_uint32 burstSize = 0;
  
  L7_RC_t check;

  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;
  L7_char8 bigBuf[600];
  L7_uint32 unit;

  L7_uint32 numArg;
 
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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg < 6)
  {
    cliConstructAndPrintPoliceSimpleSyntax(ewsContext, bigBuf, sizeof(bigBuf));
    return cliPrompt(ewsContext);
  }

  if ((cliCheckIfInteger((L7_char8 *)argv[index+argDataRate]) != L7_SUCCESS) ||
      (cliCheckIfInteger((L7_char8 *)argv[index+argBurstSize]) != L7_SUCCESS))
  {
    cliConstructAndPrintPoliceSimpleSyntax(ewsContext, bigBuf, sizeof(bigBuf));
    return cliPrompt(ewsContext);
  }

  OSAPI_STRNCPY_SAFE(strPolicyName,EWSPOLICYNAME(ewsContext));
  OSAPI_STRNCPY_SAFE(strClassName,EWSSERVICENAME(ewsContext));

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }

    if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservClassDoesntExist);
    }
  }

  /* verify if the specified  argument is an integer */

  if (cliConvertTo32BitUnsignedInteger(argv[index+argDataRate], &dataRate) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDataRate);
  }

  /* verify if the specified value is less than minimum */
  if (dataRate < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_MIN)
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectDataRate);

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argBurstSize], &burstSize) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidBurstSize);
  }

  /* verify if the specified value is in between 1 to 8000 */
  if ((burstSize < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MIN) ||
      (burstSize > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectBurstSizeRange);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* get the policy type */
    if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
    }

    /* verify if this class is associated with the specified policy or not */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    check = usmDbDiffServPolicyAttrGetNext(unit,
                                           prevPolicyId,
                                           prevPolicyInstId,
                                           prevPolicyAttrId,
                                           &nextPolicyId,
                                           &nextPolicyInstId,
                                           &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       type can be specified or not */
    while (check != L7_ERROR)
    {

      if (check == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId,
                                                      nextPolicyAttrId,
                                                      &entryType) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
          }

          /* check for incompatibilities */

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureSimpleTrafficWithDscp);
          }

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureSimpleTrafficWithPrec);
          }

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureSimpleTrafficWithCos);
          }

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureSimpleTrafficWithCos2);
          }

          if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) ||
              (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_SpecifyOnlyOneTypeOfTrafficPolicy);
          }

          /* check if simple policing is already specified */
          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {

              /* modify the data rate */
              if (usmDbDiffServPolicyAttrStmtPoliceSimpleCrateSet(unit,
                                                                  nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                  dataRate) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
              }
            }

            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              /* modify the burst size */
              if (usmDbDiffServPolicyAttrStmtPoliceSimpleCburstSet(unit,
                                                                   nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                   burstSize) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
              }
            }

            break;
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

          prevPolicyAttrId = nextPolicyAttrId;
        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          check = L7_ERROR;
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        check = usmDbDiffServPolicyAttrGetNext(unit,
                                               prevPolicyId,
                                               prevPolicyInstId,
                                               prevPolicyAttrId,
                                               &nextPolicyId,
                                               &nextPolicyInstId,
                                               &nextPolicyAttrId);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
      }
    }

    if (check != L7_SUCCESS)
    {
      /* find the free index in the policy attribute table*/
      if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                           &policyAttrId ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
      }
      if (0 == policyAttrId)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* create the row entry in the policy attribute table */
        if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                          policyAttrId, L7_TRUE) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
        }
      }

      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                    policyInstId,
                                                    policyAttrId,
                                                    entryType) != L7_SUCCESS)
        {
          /* delete the row entry just created*/
          usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
          return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* set the burst size */
        if (usmDbDiffServPolicyAttrStmtPoliceSimpleCburstSet(unit,
                                                             policyId, policyInstId, policyAttrId,
                                                             burstSize) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* set the data rate */
        if (usmDbDiffServPolicyAttrStmtPoliceSimpleCrateSet(unit,
                                                            policyId, policyInstId, policyAttrId,
                                                            dataRate) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
        }
      }

      /* make sure row status becomes active */
      usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                          policyAttrId, &status);
      if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        }
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_CfgureSimpleTrafficPolicy);
      }
    }
  }

  if (strcmp(argv[index+3],pStrInfo_qos_ConformAction_1) == 0)
  {
    /* check params of conform-action */
    if ((strcmp(argv[index+4],pStrInfo_qos_Drop_2) == 0) ||
        (strcmp(argv[index+4],pStrInfo_common_Tx_1) == 0) ||
        (strcmp(argv[index+4],pStrInfo_qos_SetCosAsCos2Tx) == 0))
    {
      argViolateAction = 5;
    }
    else if (((strcmp(argv[index+4],pStrInfo_qos_SetPrecTx) == 0) && numArg > 4) ||
             ((strcmp(argv[index+4],pStrInfo_qos_SetDscpTx) == 0) && numArg > 4) ||
             ((strcmp(argv[index+4],pStrInfo_qos_SetCosTx) == 0) && numArg > 4) ||
             ((strcmp(argv[index+4],pStrInfo_qos_SetSecCosTx) == 0) && numArg > 4))
    {
      argViolateAction = 6;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_PoliceSimple_1);
    }

    /* check for violate-action */
    if (argViolateAction == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName, strClassName,
                                                L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP, 0);
      }
    }
    else
    {
      if (strcmp(argv[index+argViolateAction],pStrInfo_qos_ViolateAction_1) == 0)
      {
        /* check params of violate-action */
        if (numArg == argViolateAction + 1)
        {
          if ((strcmp(argv[index+argViolateAction+1],pStrInfo_qos_Drop_2) != 0) &&
              (strcmp(argv[index+argViolateAction+1],pStrInfo_common_Tx_1) != 0) &&
              (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosAsCos2Tx) != 0))
          {
            cliConstructAndPrintPoliceSimpleSyntax(ewsContext, bigBuf, sizeof(bigBuf));
            return cliPrompt(ewsContext);
          }
        }
        else if (numArg == argViolateAction + 2)
        {
          if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetPrecTx) == 0)
          {
            argViolatePrecVal = argViolateAction + 2;
          }
          else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetDscpTx) == 0)
          {
            argViolateDscpVal = argViolateAction + 2;
          }

          else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosTx) == 0)
          {
            argViolateCosVal = argViolateAction + 2;
          }

          else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetSecCosTx) == 0)
          {
            argViolateCos2Val = argViolateAction + 2;
          }
          else
          {
            cliConstructAndPrintPoliceSimpleSyntax(ewsContext, bigBuf, sizeof(bigBuf));
            return cliPrompt(ewsContext);
          }
        }
        else
        {
          cliConstructAndPrintPoliceSimpleSyntax(ewsContext, bigBuf, sizeof(bigBuf));
          return cliPrompt(ewsContext);
        }
      }
      else
      {
        cliConstructAndPrintPoliceSimpleSyntax(ewsContext, bigBuf, sizeof(bigBuf));
        return cliPrompt(ewsContext);
      }
    }
  }
  else
  {
    cliConstructAndPrintPoliceSimpleSyntax(ewsContext, bigBuf, sizeof(bigBuf));
    return cliPrompt(ewsContext);
  }

  if (strcmp(argv[index+3],pStrInfo_qos_ConformAction_1) == 0)
  {
    if (strcmp(argv[index+4],pStrInfo_qos_SetDscpTx) == 0)
    {
      /* third argument validation */

      if (strlen(argv[index+argDscpVal]) >= sizeof(strDscpVal))
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      OSAPI_STRNCPY_SAFE(strDscpVal,argv[index+ argDscpVal]);

      /* convert DSCP string to integer */
      if (cliDiffservConvertDSCPStringToVal(strDscpVal, &dscpVal) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      /* check the range */
      if (dscpVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN ||
          dscpVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }
    }
    else if (strcmp(argv[index+4],pStrInfo_qos_SetPrecTx) == 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPrecVal], &precVal) != L7_SUCCESS ||
          precVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN ||
          precVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
      }
    }

    else if (strcmp(argv[index+4],pStrInfo_qos_SetCosTx) == 0)
    {
      /* third argument validation */
      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argCosVal], &cosVal) != L7_SUCCESS ||
          cosVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN ||
          cosVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
      }
    }

    else if (strcmp(argv[index+4],pStrInfo_qos_SetSecCosTx) == 0)
    {
      /* third argument validation */
      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argCos2Val], &cos2Val) != L7_SUCCESS ||
          cos2Val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN ||
          cos2Val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
      }
    }

    if (argViolateAction != 0)
    {
      if (argViolateDscpVal != 0)
      {
        /* third argument validation */
        if (strlen(argv[index+argViolateDscpVal]) >= sizeof(strViolateDscpVal))
        {
          ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
          return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
        }

        OSAPI_STRNCPY_SAFE(strViolateDscpVal,argv[index+argViolateDscpVal]);

        /* convert DSCP string to integer */
        if (cliDiffservConvertDSCPStringToVal(strViolateDscpVal, &violateDscpVal) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
          return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
        }

        /* check the range */
        if (violateDscpVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN ||
            violateDscpVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX)
        {
          ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
          return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
        }
      }
      else if (argViolatePrecVal != 0)
      {
        /* third argument validation */

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argViolatePrecVal], &violatePrecVal) != L7_SUCCESS ||
            violatePrecVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN ||
            violatePrecVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
        }
      }

      else if (argViolateCosVal != 0)
      {
        /* third argument validation */

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argViolateCosVal], &violateCosVal) != L7_SUCCESS ||
            violateCosVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN ||
            violateCosVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
        }
      }

      else if (argViolateCos2Val != 0)
      {
        /* third argument validation */

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argViolateCos2Val], &violateCos2Val) != L7_SUCCESS ||
            violateCos2Val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN ||
            violateCos2Val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
        }
      }
    }

    /* make calls for conform-action */
    if (strcmp(argv[index+4],pStrInfo_qos_Drop_2) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP,
                                             0 );
      }
    }
    else if (strcmp(argv[index+4],pStrInfo_common_Tx_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND,
                                             0 );
      }
    }
    else if (strcmp(argv[index+4],pStrInfo_qos_SetDscpTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP,
                                             dscpVal );
      }
    }
    else if (strcmp(argv[index+4],pStrInfo_qos_SetPrecTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC,
                                             precVal );
      }
    }
    else if (strcmp(argv[index+4],pStrInfo_qos_SetCosTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS,
                                             cosVal );
      }
    }
    else if (strcmp(argv[index+4],pStrInfo_qos_SetCosAsCos2Tx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2,
                                             L7_TRUE );
      }
    }
    else if (strcmp(argv[index+4],pStrInfo_qos_SetSecCosTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2,
                                             cos2Val );
      }
    }

    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidConformAction);
    }

    /* make calls for violate-action */
    if (argViolateAction != 0)
    {
      if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_Drop_2) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP,
                                                  0 );
        }
      }
      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_common_Tx_1) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND,
                                                  0 );
        }
      }
      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetDscpTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP,
                                                  violateDscpVal );
        }
      }
      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetPrecTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC,
                                                  violatePrecVal );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS,
                                                  violateCosVal );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosAsCos2Tx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2,
                                                  L7_TRUE );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetSecCosTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2,
                                                  violateCos2Val );
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidViolateAction);
      }
    }
  } /* conform-action */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets single rate traffic policing action
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
* @notes   1. meaningful for a policy of type 'in'
*          2. Incompatible with mark IP DSCP
*
* @cmdsyntax  police-single-rate {
*                        <1-4294967295> <1-128> <1-128> |
*                         conform-action { drop | set-prec-transmit <0-7> | set-dscp-transmit <0-63> | set-cos-transmit <0-7> | set-sec-cos-transmit <0-7> | transmit } |
*                         exceedaction { drop | set-prec-transmit <0-7> | set-dscp-transmit <0-63> | set-cos-transmit <0-7> | set-sec-cos-transmit <0-7>| transmit } |
*                         [violate-action { drop | set-prec-transmit <0-7> | set-dscp-transmit <0-63> | set-cos-transmit <0-7> | set-sec-cos-transmit <0-7>| transmit }]
*                    }
*
*
* @cmdhelp
*
* @cmddescript  sets single rate traffic policing action for the specified
*               class instance with in the specified policy.
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandPoliceSingleRate(EwsContext ewsContext,
                                        uintf argc,
                                        const L7_char8 * * argv,
                                        uintf index)
{
  L7_uint32 argPrecVal = 6;
  L7_uint32 precVal = 0;
  L7_uint32 argDscpVal = 6;
  L7_char8 strDscpVal[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 dscpVal = 0;
  L7_uint32 argCosVal = 6;
  L7_uint32 cosVal = 0;
  L7_uint32 argCos2Val = 6;
  L7_uint32 cos2Val = 0;

  L7_uint32 argExceedPrecVal = 0;
  L7_uint32 exceedPrecVal = 0;
  L7_uint32 argExceedDscpVal = 0;
  L7_char8 strExceedDscpVal[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 exceedDscpVal = 0;
  L7_uint32 argExceedCosVal = 0;
  L7_uint32 exceedCosVal = 0;
  L7_uint32 argExceedCos2Val = 0;
  L7_uint32 exceedCos2Val = 0;

  L7_uint32 argViolatePrecVal = 0;
  L7_uint32 violatePrecVal = 0;
  L7_uint32 argViolateDscpVal = 0;
  L7_char8 strViolateDscpVal[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 violateDscpVal = 0;
  L7_uint32 argViolateCosVal = 0;
  L7_uint32 violateCosVal = 0;
  L7_uint32 argViolateCos2Val = 0;
  L7_uint32 violateCos2Val = 0;

  L7_uint32 argExceedAction = 0;
  L7_uint32 argViolateAction = 0;

  L7_uint32 argDataRate = 1;
  L7_uint32 argBurstSize = 2;
  L7_uint32 argEBurstSize = 3;

  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 policyAttrId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;
  L7_uint32 dataRate = 0;
  L7_uint32 burstSize = 0;
  L7_uint32 eBurstSize = 0;

  L7_RC_t check;

  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;

  L7_uint32 unit;

  L7_uint32 numArg;
  L7_char8 bigBuf[600];

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg < 9)
  {
    cliConstructAndPrintPoliceSingleRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
    return cliPrompt(ewsContext);
  }

  if ((cliCheckIfInteger((L7_char8 *)argv[index+argDataRate]) != L7_SUCCESS) ||
      (cliCheckIfInteger((L7_char8 *)argv[index+argBurstSize]) != L7_SUCCESS) ||
      (cliCheckIfInteger((L7_char8 *)argv[index+argEBurstSize]) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_PoliceSingleRate_1);
  }
  if (strcmp(argv[index+4],pStrInfo_qos_ConformAction_1) == 0)
  {
    /* check params of conform-action */
    if ((strcmp(argv[index+5],pStrInfo_qos_Drop_2) == 0) ||
        (strcmp(argv[index+5],pStrInfo_common_Tx_1) == 0) ||
        (strcmp(argv[index+5],pStrInfo_qos_SetCosAsCos2Tx) == 0))
    {
      argExceedAction = 6;
    }
    else if (((strcmp(argv[index+5],pStrInfo_qos_SetPrecTx) == 0) && numArg > 3) ||
             ((strcmp(argv[index+5],pStrInfo_qos_SetDscpTx) == 0) && numArg > 3) ||
             ((strcmp(argv[index+5],pStrInfo_qos_SetCosTx) == 0) && numArg > 3) ||
             ((strcmp(argv[index+5],pStrInfo_qos_SetSecCosTx) == 0) && numArg > 3))
    {
      argExceedAction = 7;
    }
    else
    {
      cliConstructAndPrintPoliceSingleRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
      return cliPrompt(ewsContext);
    }

    if (strcmp(argv[index+argExceedAction],pStrInfo_qos_ExceedAction_1) == 0)
    {
      /* check params of exceed-action */
      if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_Drop_2) == 0) ||
         (strcmp(argv[index+argExceedAction+1],pStrInfo_common_Tx_1) == 0) ||
         (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetCosAsCos2Tx) == 0))
      {
        if (numArg > argExceedAction + 1)
        {
          argViolateAction = argExceedAction + 2;
        }
      }
      else if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetPrecTx) == 0) &&
               (numArg > argExceedAction+1))
      {
        argExceedPrecVal = argExceedAction + 2;

        if (numArg > argExceedAction + 2)
        {
          argViolateAction = argExceedAction + 3;
        }
      }
      else if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetDscpTx) == 0) &&
               (numArg > argExceedAction+1))
      {
        argExceedDscpVal = argExceedAction + 2;

        if (numArg > argExceedAction + 2)
        {
          argViolateAction = argExceedAction + 3;
        }
      }

      else if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetCosTx) == 0) &&
               (numArg > argExceedAction+1))
      {
        argExceedCosVal = argExceedAction + 2;

        if (numArg > argExceedAction + 2)
        {
          argViolateAction = argExceedAction + 3;
        }
      }

      else if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetSecCosTx) == 0) &&
               (numArg > argExceedAction+1))
      {
        argExceedCos2Val = argExceedAction + 2;

        if (numArg > argExceedAction + 2)
        {
          argViolateAction = argExceedAction + 3;
        }
      }
      else
      {
        cliConstructAndPrintPoliceSingleRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      cliConstructAndPrintPoliceSingleRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
      return cliPrompt(ewsContext);
    }

    /* check for violate-action */
    if (argViolateAction != 0)
    {
      if (strcmp(argv[index+argViolateAction],pStrInfo_qos_ViolateAction_1) == 0)
      {
        /* check params of violate-action */
        if (numArg == argViolateAction + 1)
        {
          if ((strcmp(argv[index+argViolateAction+1],pStrInfo_qos_Drop_2) != 0) &&
              (strcmp(argv[index+argViolateAction+1],pStrInfo_common_Tx_1) != 0) &&
              (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosAsCos2Tx) != 0))
          {
            cliConstructAndPrintPoliceSingleRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
            return cliPrompt(ewsContext);
          }
        }
        else if (numArg == argViolateAction + 2)
        {
          if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetPrecTx) == 0)
          {
            argViolatePrecVal = argViolateAction + 2;
          }
          else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetDscpTx) == 0)
          {
            argViolateDscpVal = argViolateAction + 2;
          }
          else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosTx) == 0)
          {
            argViolateCosVal = argViolateAction + 2;
          }
          else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetSecCosTx) == 0)
          {
            argViolateCos2Val = argViolateAction + 2;
          }
          else
          {
            cliConstructAndPrintPoliceSingleRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
            return cliPrompt(ewsContext);
          }
        }
        else
        {
          cliConstructAndPrintPoliceSingleRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
          return cliPrompt(ewsContext);
        }
      }
      else
      {
        cliConstructAndPrintPoliceSingleRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
        return cliPrompt(ewsContext);
      }
    } /* check for violate-action */
  } /* check for conform-action */
  else
  {
    cliConstructAndPrintPoliceSingleRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
    return cliPrompt(ewsContext);
  }

  OSAPI_STRNCPY_SAFE(strPolicyName,EWSPOLICYNAME(ewsContext));
  OSAPI_STRNCPY_SAFE(strClassName,EWSSERVICENAME(ewsContext));

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }

    if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservClassDoesntExist);
    }
  }

  /* third argument validation */

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argDataRate], &dataRate) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDataRate);
  }

  /* verify if the specified value is less than 1 */
  if (dataRate < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_MIN)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectDataRate);
  }

  /* fourth argument validation */

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argBurstSize], &burstSize) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidBurstSize);
  }

  /* verify if the specified value is in between 1 to 8000 */
  if ((burstSize < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MIN) ||
      (burstSize > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectBurstSizeRange);
  }

  /* fifth argument validation */

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argEBurstSize], &eBurstSize) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidExceedIn);
  }

  /* verify if the specified value is in between 1 to 64000 */
  if ((eBurstSize < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MIN) ||
      (eBurstSize > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectExceedIn);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* get the policy type */
    if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
    }

    /* verify if this class is associated with the specified policy or not */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    /* verify if exceeding burst size is smaller than conforming burst size*/
    if (eBurstSize < burstSize)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ExceedInBurstSizeMustExceedConforming);
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    check = usmDbDiffServPolicyAttrGetNext(unit,
                                           prevPolicyId,
                                           prevPolicyInstId,
                                           prevPolicyAttrId,
                                           &nextPolicyId,
                                           &nextPolicyInstId,
                                           &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       type can be specified or not */
    while (check != L7_ERROR)
    {

      if (check == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId,
                                                      nextPolicyAttrId,
                                                      &entryType) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
          }

          /* check for incompatibilities */

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureSingleTrafficWithDscp);
          }

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureSingleTrafficWithPrec);
          }

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureSingleTrafficWithCos);
          }

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureSingleTrafficWithCos2);
          }

          if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
              (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_SpecifyOnlyOneTypeOfTrafficPolicy);
          }

          /* check if single rate policy is already specified */
          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              /* modify the burst size */
              if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstSet(unit,
                                                                       nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                       burstSize) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
              }
            }

            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              /* modify the exceeding burst size */
              if (usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstSet(unit,
                                                                       nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                       eBurstSize) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
              }
            }

            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              /* modify the data rate */
              if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateSet(unit,
                                                                      nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                      dataRate) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
              }
            }

            break;
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

          prevPolicyAttrId = nextPolicyAttrId;
        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          check = L7_ERROR;
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        check = usmDbDiffServPolicyAttrGetNext(unit,
                                               prevPolicyId,
                                               prevPolicyInstId,
                                               prevPolicyAttrId,
                                               &nextPolicyId,
                                               &nextPolicyInstId,
                                               &nextPolicyAttrId);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
      }
    }

    if (check != L7_SUCCESS)
    {
      /* find the free index in the policy attribute table*/
      if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                           &policyAttrId ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
      }
      if (0 == policyAttrId)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* create the row entry in the policy attribute table */
        if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                          policyAttrId, L7_TRUE) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
        }
      }

      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                    policyInstId,
                                                    policyAttrId,
                                                    entryType) != L7_SUCCESS)
        {
          /* delete the row entry just created*/
          usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
          return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* set the burst size */
        if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstSet(unit,
                                                                 policyId, policyInstId, policyAttrId,
                                                                 burstSize) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* set the exceeding burst size */
        if (usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstSet(unit,
                                                                 policyId, policyInstId, policyAttrId,
                                                                 eBurstSize) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* set the data rate */
        if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateSet(unit,
                                                                policyId, policyInstId, policyAttrId,
                                                                dataRate) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
        }
      }

      /* make sure row status becomes active */
      usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                          policyAttrId, &status);
      if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        }
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_CfgureSingleRateTrafficPolicy);
      }
    }
  }

  if (strcmp(argv[index+4],pStrInfo_qos_ConformAction_1) == 0)
  {
    /* validate args of conform-action */
    if (strcmp(argv[index+5],pStrInfo_qos_SetDscpTx) == 0)
    {
      /* third argument validation */

      if (strlen(argv[index+argDscpVal]) >= sizeof(strDscpVal))
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      OSAPI_STRNCPY_SAFE(strDscpVal,argv[index+ argDscpVal]);

      /* convert DSCP string to integer */
      if (cliDiffservConvertDSCPStringToVal(strDscpVal, &dscpVal) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      /* check the range */
      if (dscpVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN ||
          dscpVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }
    } /* validate set-dscp-transmit */
    else if (strcmp(argv[index+5],pStrInfo_qos_SetPrecTx) == 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPrecVal], &precVal) != L7_SUCCESS ||
          precVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN ||
          precVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
      }
    }

    else if (strcmp(argv[index+5],pStrInfo_qos_SetCosTx) == 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argCosVal], &cosVal) != L7_SUCCESS ||
          cosVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN ||
          cosVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
      }
    }

    else if (strcmp(argv[index+5],pStrInfo_qos_SetSecCosTx) == 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argCos2Val], &cos2Val) != L7_SUCCESS ||
          cosVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN ||
          cosVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
      }
    }

    /* validate args of exceed-action */

    if (argExceedDscpVal != 0)
    {
      /* third argument validation */
      if (strlen(argv[index+argExceedDscpVal]) >= sizeof(strExceedDscpVal))
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      OSAPI_STRNCPY_SAFE(strExceedDscpVal,argv[index+argExceedDscpVal]);

      /* convert DSCP string to integer */
      if (cliDiffservConvertDSCPStringToVal(strExceedDscpVal, &exceedDscpVal) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      /* check the range */
      if (exceedDscpVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN ||
          exceedDscpVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }
    }
    else if (argExceedPrecVal != 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argExceedPrecVal], &exceedPrecVal) != L7_SUCCESS ||
          exceedPrecVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN ||
          exceedPrecVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
      }
    }

    else if (argExceedCosVal != 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argExceedCosVal], &exceedCosVal) != L7_SUCCESS ||
          exceedCosVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN ||
          exceedCosVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
      }
    }

    else if (argExceedCos2Val != 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argExceedCos2Val], &exceedCos2Val) != L7_SUCCESS ||
          exceedCos2Val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN ||
          exceedCos2Val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
      }
    }

    if (argViolateAction != 0)
    {
      if (argViolateDscpVal != 0)
      {
        /* third argument validation */
        if (strlen(argv[index+argViolateDscpVal]) >= sizeof(strViolateDscpVal))
        {
          ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
          return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
        }

        OSAPI_STRNCPY_SAFE(strViolateDscpVal,argv[index+argViolateDscpVal]);

        /* convert DSCP string to integer */
        if (cliDiffservConvertDSCPStringToVal(strViolateDscpVal, &violateDscpVal) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
          return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
        }

        /* check the range */
        if (violateDscpVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN ||
            violateDscpVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX)
        {
          ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
          return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
        }
      }
      else if (argViolatePrecVal != 0)
      {
        /* third argument validation */

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argViolatePrecVal], &violatePrecVal) != L7_SUCCESS ||
            violatePrecVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN ||
            violatePrecVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
        }
      }

      else if (argViolateCosVal != 0)
      {
        /* third argument validation */

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argViolateCosVal], &violateCosVal) != L7_SUCCESS ||
            violateCosVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN ||
            violateCosVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
        }
      }

      else if (argViolateCos2Val != 0)
      {
        /* third argument validation */

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argViolateCos2Val], &violateCos2Val) != L7_SUCCESS ||
            violateCos2Val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN ||
            violateCos2Val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
        }
      }

    } /* violate-action argument validation */

    /* calls for conform-action */
    if (strcmp(argv[index+5],pStrInfo_qos_Drop_2) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP,                                           0 );
      }
    }
    else if (strcmp(argv[index+5],pStrInfo_common_Tx_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND,
                                             0 );
      }
    }
    else if (strcmp(argv[index+5],pStrInfo_qos_SetDscpTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP,
                                             dscpVal );
      }
    }
    else if (strcmp(argv[index+5],pStrInfo_qos_SetPrecTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC,
                                             precVal );
      }
    }

    else if (strcmp(argv[index+5],pStrInfo_qos_SetCosTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS,
                                             cosVal );
      }
    }

    else if (strcmp(argv[index+5],pStrInfo_qos_SetCosAsCos2Tx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2,
                                             L7_TRUE);
      }
    }

    else if (strcmp(argv[index+5],pStrInfo_qos_SetSecCosTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2,
                                             cos2Val );
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidConformAction);
    }

    /* calls for exceed-action */
    if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_Drop_2) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP,
                                            0 );
      }
    }
    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_common_Tx_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND,
                                            0 );
      }
    }
    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetDscpTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP,
                                            exceedDscpVal );
      }

    }
    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetPrecTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC,
                                            exceedPrecVal );
      }
    }

    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetCosTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS,
                                            exceedCosVal );
      }
    }

    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetCosAsCos2Tx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2,
                                            L7_TRUE );
      }
    }

    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetSecCosTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2,
                                            exceedCos2Val );
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidExceedAction);
    }

    if (argViolateAction == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName, strClassName,
                                                L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP, 0);
      }
    }
    else
    {
      if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_Drop_2) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP,
                                                  0 );
        }
      }
      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_common_Tx_1) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND,
                                                  0 );
        }
      }
      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetDscpTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP,
                                                  violateDscpVal );
        }
      }
      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetPrecTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC,
                                                  violatePrecVal );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS,
                                                  violateCosVal );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosAsCos2Tx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2,
                                                  L7_TRUE );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetSecCosTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2,
                                                  violateCos2Val );
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidViolateAction);
      }

    } /* violate-action */
  } /* conform-action */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets tworate traffic policing action
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
* @notes   1. meaningful for a policy of type 'in'
*          2. Incompatible with mark IP DSCP
*
* @cmdsyntax  police tworate {
*                              <1-4294967295> <1-128> <1-4294967295> <1-128> |
*                               conformaction { drop | set-prec-transmit <0-7> | set-dscp-transmit <0-63> | set-cos-transmit <0-7> | set-sec-cos-transmit <0-7> | transmit } |
*                               exceedaction { drop | set-prec-transmit <0-7> | set-dscp-transmit <0-63> | set-cos-transmit <0-7> | set-sec-cos-transmit <0-7> | transmit } |
*                               [violate-action { drop | set-prec-transmit <0-7> | set-dscp-transmit <0-63> | set-cos-transmit <0-7> | set-sec-cos-transmit <0-7> | transmit } ]
*                             }
*
* @cmdhelp
*
* @cmddescript  sets two rate traffic policing action for the specified
*               class instance with in the specified policy.
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandPoliceTwoRate(EwsContext ewsContext,
                                     uintf argc,
                                     const L7_char8 * * argv,
                                     uintf index)
{

  L7_uint32 argPrecVal = 7;
  L7_uint32 precVal = 0;
  L7_uint32 argDscpVal = 7;
  L7_char8 strDscpVal[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 dscpVal = 0;
  L7_uint32 argCosVal = 7;
  L7_uint32 cosVal = 0;
  L7_uint32 argCos2Val = 7;
  L7_uint32 cos2Val = 0;

  L7_uint32 argExceedPrecVal = 0;
  L7_uint32 exceedPrecVal = 0;
  L7_uint32 argExceedDscpVal = 0;
  L7_char8 strExceedDscpVal[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 exceedDscpVal = 0;
  L7_uint32 argExceedCosVal = 0;
  L7_uint32 exceedCosVal = 0;
  L7_uint32 argExceedCos2Val = 0;
  L7_uint32 exceedCos2Val = 0;

  L7_uint32 argViolatePrecVal = 0;
  L7_uint32 violatePrecVal = 0;
  L7_uint32 argViolateDscpVal = 0;
  L7_char8 strViolateDscpVal[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 violateDscpVal = 0;
  L7_uint32 argViolateCosVal = 0;
  L7_uint32 violateCosVal = 0;
  L7_uint32 argViolateCos2Val = 0;
  L7_uint32 violateCos2Val = 0;

  L7_uint32 argExceedAction = 0;
  L7_uint32 argViolateAction = 0;

  L7_uint32 argDataRate = 1;
  L7_uint32 argBurstSize = 2;
  L7_uint32 argPDataRate = 3;
  L7_uint32 argPBurstSize = 4;

  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 policyAttrId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;
  L7_uint32 dataRate = 0;
  L7_uint32 burstSize = 0;
  L7_uint32 pDataRate = 0;
  L7_uint32 pBurstSize = 0;
  L7_uint32 unit;

  L7_uint32 numArg;

  L7_RC_t check;

  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;
  L7_char8 bigBuf[600];

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

  numArg = cliNumFunctionArgsGet();

  if (numArg < 10)
  {
    cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
    return cliPrompt(ewsContext);
  }

  /* check number of arguments */
  if ((cliCheckIfInteger((L7_char8 *)argv[index+argDataRate]) != L7_SUCCESS) ||
      (cliCheckIfInteger((L7_char8 *)argv[index+argBurstSize]) != L7_SUCCESS) ||
      (cliCheckIfInteger((L7_char8 *)argv[index+argPDataRate]) != L7_SUCCESS) ||
      (cliCheckIfInteger((L7_char8 *)argv[index+argPBurstSize]) != L7_SUCCESS))
  {
    cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
    return cliPrompt(ewsContext);
  }

  if (strcmp(argv[index+5],pStrInfo_qos_ConformAction_1) == 0)
  {
    /* check params of conform-action */
    if ((strcmp(argv[index+6],pStrInfo_qos_Drop_2) == 0) ||
        (strcmp(argv[index+6],pStrInfo_common_Tx_1) == 0) ||
        (strcmp(argv[index+6],pStrInfo_qos_SetCosAsCos2Tx) == 0))
    {
      argExceedAction = 7;
    }
    else if (((strcmp(argv[index+6],pStrInfo_qos_SetPrecTx) == 0) && numArg > 7) ||
             ((strcmp(argv[index+6],pStrInfo_qos_SetDscpTx) == 0) && numArg > 7) ||
             ((strcmp(argv[index+6],pStrInfo_qos_SetCosTx) == 0) && numArg > 7) ||
             ((strcmp(argv[index+6],pStrInfo_qos_SetSecCosTx) == 0) && numArg > 7))
    {
      argExceedAction = 8;
    }
    else
    {
      cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
      return cliPrompt(ewsContext);
    }

    if (strcmp(argv[index+argExceedAction],pStrInfo_qos_ExceedAction_1) == 0)
    {
      /* check params of exceed-action */
      if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_Drop_2) == 0) ||
          (strcmp(argv[index+argExceedAction+1],pStrInfo_common_Tx_1) == 0) ||
          (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetCosAsCos2Tx) == 0))
      {
        if (numArg > argExceedAction + 1)
        {
          argViolateAction = argExceedAction + 2;
        }
      }
      else if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetPrecTx) == 0) &&
               (numArg > argExceedAction+1))
      {
        argExceedPrecVal = argExceedAction + 2;

        if (numArg > argExceedAction + 2)
        {
          argViolateAction = argExceedAction + 3;
        }
      }
      else if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetDscpTx) == 0) &&
               (numArg > argExceedAction+1))
      {
        argExceedDscpVal = argExceedAction + 2;

        if (numArg > argExceedAction + 2)
        {
          argViolateAction = argExceedAction + 3;
        }
      }

      else if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetCosTx) == 0) &&
               (numArg > argExceedAction+1))
      {
        argExceedCosVal = argExceedAction + 2;

        if (numArg > argExceedAction + 2)
        {
          argViolateAction = argExceedAction + 3;
        }
      }

      else if ((strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetSecCosTx) == 0) &&
               (numArg > argExceedAction+1))
      {
        argExceedCos2Val = argExceedAction + 2;

        if (numArg > argExceedAction + 2)
        {
          argViolateAction = argExceedAction + 3;
        }
      }
      else
      {
        cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
      return cliPrompt(ewsContext);
    }

    /* check for violate-action */
    if (argViolateAction != 0)
    {
      if (strcmp(argv[index+argViolateAction],pStrInfo_qos_ViolateAction_1) == 0)
      {
        /* check params of violate-action */
        if (numArg == argViolateAction + 1)
        {
          if ((strcmp(argv[index+argViolateAction+1],pStrInfo_qos_Drop_2) != 0) &&
              (strcmp(argv[index+argViolateAction+1],pStrInfo_common_Tx_1) != 0) &&
              (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosAsCos2Tx) != 0))
          {
            cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
            return cliPrompt(ewsContext);
          }
        }
        else if (numArg == argViolateAction + 2)
        {
          if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetPrecTx) == 0)
          {
            argViolatePrecVal = argViolateAction + 2;
          }
          else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetDscpTx) == 0)
          {
            argViolateDscpVal = argViolateAction + 2;
          }

          else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosTx) == 0)
          {
            argViolateCosVal = argViolateAction + 2;
          }

          else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetSecCosTx) == 0)
          {
            argViolateCos2Val = argViolateAction + 2;
          }
          else
          {
            cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
            return cliPrompt(ewsContext);
          }
        }
        else
        {
          cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
          return cliPrompt(ewsContext);
        }
      }
      else
      {
        cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
        return cliPrompt(ewsContext);
      }
    } /* check for violate-action */
  } /* check for conform-action */
  else
  {
    cliConstructAndPrintPoliceTwoRateSyntax(ewsContext, bigBuf, sizeof(bigBuf));
    return cliPrompt(ewsContext);
  }

  OSAPI_STRNCPY_SAFE(strPolicyName,EWSPOLICYNAME(ewsContext));
  OSAPI_STRNCPY_SAFE(strClassName,EWSSERVICENAME(ewsContext));

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }

    if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservClassDoesntExist);
    }
  }

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argDataRate], &dataRate) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDataRate);
  }

  /* verify if the specified value is less than 1 */
  if (dataRate < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_MIN)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectDataRate);
  }

  /* fourth argument validation */

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argBurstSize], &burstSize) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidBurstSize);
  }

  /* verify if the specified value is in between 1 to 8000 */
  if ((burstSize < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MIN) ||
      (burstSize > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectBurstSizeRange);
  }

  /* fifth argument validation */

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argPDataRate], &pDataRate) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPeakDataRate);
  }

  /* verify if the specified value is less than 1 */
  if (pDataRate < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PRATE_KBPS_MIN)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPeakDataRate);
  }

  /* sixth argument validation */

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argPBurstSize], &pBurstSize) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPeakBurstSize);
  }

  /* verify if the specified value is in between 1 to 64000 */
  if ((pBurstSize < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MIN) ||
      (pBurstSize > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MAX))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectPeakBurstSize);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* get the policy type */
    if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
    }

    /* verify if this class is associated with the specified policy or not */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }
  }

  /* verify if peak data rate is smaller than conforming data rate*/
  if (pDataRate < dataRate)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_PeakDataRateMustExceedConforming);
  }

  /* verify if peak burst size is smaller than conforming burst size*/
  if (pBurstSize < burstSize)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_PeakBurstSizeMustExceedConformingBurstSize);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    check = usmDbDiffServPolicyAttrGetNext(unit,
                                           prevPolicyId,
                                           prevPolicyInstId,
                                           prevPolicyAttrId,
                                           &nextPolicyId,
                                           &nextPolicyInstId,
                                           &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       type can be specified or not */
    while (check != L7_ERROR)
    {
      if (check == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId,
                                                      nextPolicyAttrId,
                                                      &entryType) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
          }

          /* check for incompatibilities */

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureTwoRateWithDscp);
          }

          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantCfgureTwoRateWithPrec);
          }

          if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
              (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_SpecifyOnlyOneTypeOfTrafficPolicy);
          }

          /* check if two rate policing is already specified */
          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              /* modify the burst size */
              if (usmDbDiffServPolicyAttrStmtPoliceTworateCburstSet(unit,
                                                                    nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                    burstSize) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
              }
            }

            /* modify the data rate */
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              if (usmDbDiffServPolicyAttrStmtPoliceTworateCrateSet(unit,
                                                                   nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                   dataRate) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
              }
            }

            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              /* modify the peak burst size */
              if (usmDbDiffServPolicyAttrStmtPoliceTworatePburstSet(unit,
                                                                    nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                    pBurstSize) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
              }
            }

            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              /* modify the peak data rate */
              if (usmDbDiffServPolicyAttrStmtPoliceTworatePrateSet(unit,
                                                                   nextPolicyId, nextPolicyInstId, nextPolicyAttrId,
                                                                   pDataRate) != L7_SUCCESS)
              {

                return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
              }
            }

            break;
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

          prevPolicyAttrId = nextPolicyAttrId;
        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          check = L7_ERROR;
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        check = usmDbDiffServPolicyAttrGetNext(unit,
                                               prevPolicyId,
                                               prevPolicyInstId,
                                               prevPolicyAttrId,
                                               &nextPolicyId,
                                               &nextPolicyInstId,
                                               &nextPolicyAttrId);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
      }
    }

    if (check != L7_SUCCESS)
    {
      /* find the free index in the policy attribute table*/
      if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                           &policyAttrId ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
      }
      if (0 == policyAttrId)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* create the row entry in the policy attribute table */
        if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                          policyAttrId, L7_TRUE) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
        }
      }

      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                    policyInstId,
                                                    policyAttrId,
                                                    entryType) != L7_SUCCESS)
        {
          /* delete the row entry just created*/
          usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
          return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
        }
      }

  /* Changed location -samip */

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* set the burst size */
        if (usmDbDiffServPolicyAttrStmtPoliceTworateCburstSet(unit,
                                                              policyId, policyInstId, policyAttrId,
                                                              burstSize) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* set the data rate */
        if (usmDbDiffServPolicyAttrStmtPoliceTworateCrateSet(unit,
                                                             policyId, policyInstId, policyAttrId,
                                                             dataRate) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* set the peak burst size */
        if (usmDbDiffServPolicyAttrStmtPoliceTworatePburstSet(unit,
                                                              policyId, policyInstId, policyAttrId,
                                                              pBurstSize) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* set the peak data rate */
        if (usmDbDiffServPolicyAttrStmtPoliceTworatePrateSet(unit,
                                                             policyId, policyInstId, policyAttrId,
                                                             pDataRate) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
        }
      }

      /* make sure row status becomes active */
      usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                          policyAttrId, &status);
      if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_CfgureTwoRateTrafficPolicy);
      }
    }
  }

  if (strcmp(argv[index+5],pStrInfo_qos_ConformAction_1) == 0)
  {
    /* validate args of conform-action */
    if (strcmp(argv[index+6],pStrInfo_qos_SetDscpTx) == 0)
    {
      /* third argument validation */

      if (strlen(argv[index+argDscpVal]) >= sizeof(strDscpVal))
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      OSAPI_STRNCPY_SAFE(strDscpVal,argv[index+ argDscpVal]);

      /* convert DSCP string to integer */
      if (cliDiffservConvertDSCPStringToVal(strDscpVal, &dscpVal) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      /* check the range */
      if (dscpVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN ||
          dscpVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }
    } /* validate set-dscp-transmit */
    else if (strcmp(argv[index+6],pStrInfo_qos_SetPrecTx) == 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPrecVal], &precVal) != L7_SUCCESS ||
          precVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN ||
          precVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
      }
    }

    else if (strcmp(argv[index+6],pStrInfo_qos_SetCosTx) == 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argCosVal], &cosVal) != L7_SUCCESS ||
          cosVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN ||
          cosVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
      }
    }

    else if (strcmp(argv[index+6],pStrInfo_qos_SetSecCosTx) == 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argCos2Val], &cos2Val) != L7_SUCCESS ||
          cos2Val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN ||
          cos2Val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
      }
    }

    /* validate args of exceed-action */

    if (argExceedDscpVal != 0)
    {
      /* third argument validation */
      if (strlen(argv[index+argExceedDscpVal]) >= sizeof(strExceedDscpVal))
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      OSAPI_STRNCPY_SAFE(strExceedDscpVal,argv[index+argExceedDscpVal]);

      /* convert DSCP string to integer */
      if (cliDiffservConvertDSCPStringToVal(strExceedDscpVal, &exceedDscpVal) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }

      /* check the range */
      if (exceedDscpVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN ||
          exceedDscpVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX)
      {
        ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
        return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
      }
    }
    else if (argExceedPrecVal != 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argExceedPrecVal], &exceedPrecVal) != L7_SUCCESS ||
          exceedPrecVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN ||
          exceedPrecVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
      }
    }

    else if (argExceedCosVal != 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argExceedCosVal], &exceedCosVal) != L7_SUCCESS ||
          exceedCosVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN ||
          exceedCosVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
      }
    }

    else if (argExceedCos2Val != 0)
    {
      /* third argument validation */

      /* verify if the specified  argument is an integer */
      if (cliConvertTo32BitUnsignedInteger(argv[index+argExceedCos2Val], &exceedCos2Val) != L7_SUCCESS ||
          exceedCos2Val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN ||
          exceedCos2Val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
      }
    }

    if (argViolateAction != 0)
    {
      if (argViolateDscpVal != 0)
      {
        /* third argument validation */
        if (strlen(argv[index+argViolateDscpVal]) >= sizeof(strViolateDscpVal))
        {
          ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
          return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
        }

        OSAPI_STRNCPY_SAFE(strViolateDscpVal,argv[index+argViolateDscpVal]);

        /* convert DSCP string to integer */
        if (cliDiffservConvertDSCPStringToVal(strViolateDscpVal, &violateDscpVal) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
          return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
        }

        /* check the range */
        if (violateDscpVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN ||
            violateDscpVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX)
        {
          ewsTelnetWrite(ewsContext, pStrErr_qos_InvalidDscp);
          return cliSyntaxReturnPrompt (ewsContext, dscpHelp);
        }
      }
      else if (argViolatePrecVal != 0)
      {
        /* third argument validation */

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argViolatePrecVal], &violatePrecVal) != L7_SUCCESS ||
            violatePrecVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN ||
            violatePrecVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectIpPrecedence);
        }
      }

      else if (argViolateCosVal != 0)
      {
        /* third argument validation */

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argViolateCosVal], &violateCosVal) != L7_SUCCESS ||
            violateCosVal < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN ||
            violateCosVal > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectVlanVal);
        }
      }

      else if (argViolateCos2Val != 0)
      {
        /* third argument validation */

        /* verify if the specified  argument is an integer */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argViolateCos2Val], &violateCos2Val) != L7_SUCCESS ||
            violateCos2Val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN ||
            violateCos2Val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_IncorrectSecondaryCosVal);
        }
      }
    } /* violate-action argument validation */

    /* calls for conform-action */
    if (strcmp(argv[index+6],pStrInfo_qos_Drop_2) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP,                                           0 );
      }
    }
    else if (strcmp(argv[index+6],pStrInfo_common_Tx_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {

        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND,
                                             0 );
      }
    }
    else if (strcmp(argv[index+6],pStrInfo_qos_SetDscpTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP,
                                             dscpVal );
      }
    }
    else if (strcmp(argv[index+6],pStrInfo_qos_SetPrecTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC,
                                             precVal );
      }
    }
    else if (strcmp(argv[index+6],pStrInfo_qos_SetCosTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS,
                                             cosVal );
      }
    }

    else if (strcmp(argv[index+6],pStrInfo_qos_SetCosAsCos2Tx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2,
                                             L7_TRUE );
      }
    }

    else if (strcmp(argv[index+6],pStrInfo_qos_SetSecCosTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceConform( ewsContext, strPolicyName, strClassName,
                                             L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2,
                                             cos2Val );
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidConformAction);
    }

    /* calls for exceed-action */
    if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_Drop_2) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP,
                                            0 );
      }
    }
    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_common_Tx_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND,
                                            0 );
      }
    }
    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetDscpTx) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP,
                                            exceedDscpVal );
      }

    }
    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetPrecTx) == 0)
    {

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC,
                                            exceedPrecVal );
      }
    }

    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetCosTx) == 0)
    {

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS,
                                            exceedCosVal );
      }
    }

    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetCosAsCos2Tx) == 0)
    {

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2,
                                            L7_TRUE );
      }
    }

    else if (strcmp(argv[index+argExceedAction+1],pStrInfo_qos_SetSecCosTx) == 0)
    {

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceExceed( ewsContext, strPolicyName, strClassName,
                                            L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2,
                                            exceedCos2Val );
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidExceedAction);
    }

    if (argViolateAction == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName, strClassName,
                                                L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP, 0);
      }
    }
    else
    {
      if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_Drop_2) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP,
                                                  0 );
        }
      }
      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_common_Tx_1) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND,
                                                  0 );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetDscpTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP,
                                                  violateDscpVal );
        }
      }
      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetPrecTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC,
                                                  violatePrecVal );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS,
                                                  violateCosVal );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetCosAsCos2Tx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2,
                                                  L7_TRUE );
        }
      }

      else if (strcmp(argv[index+argViolateAction+1],pStrInfo_qos_SetSecCosTx) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          cliConfigDiffservPolicyPoliceNonconform(ewsContext, strPolicyName,
                                                  strClassName,
                                                  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2,
                                                  violateCos2Val );
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidViolateAction);
      }
    } /* violate-action */
  } /* conform-action */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Renames a DiffServ Policy
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
* @cmdsyntax  policy map rename <policyname> <newpolicyname>
*
* @cmdhelp
*
* @cmddescript  Renames a DiffServ policy, now onwards identified by the
*               new entered name.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandPolicyMapRename(EwsContext ewsContext,
                                       uintf argc,
                                       const L7_char8 * * argv,
                                       uintf index)
{
  L7_uint32 argName = 1;
  L7_uint32 argNewName = 2;

  L7_uint32 policyId = 0;
  L7_uint32 newPolicyId = 0;

  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strNewName[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 unit;

  L7_uint32 numArg;

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_PolicyMapRename);
  }

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

  /* verify the length of <newpolicyname> argument */
  if (strlen(argv[index+ argNewName]) > L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_PolicyNameTooLong);
  }

  OSAPI_STRNCPY_SAFE(strNewName,argv[index+ argNewName]);

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strNewName) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_qos_NewPolicyNameAlnum);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* get the index of policy from policy name */
    if (usmDbDiffServPolicyNameToIndex(unit, strName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservPolicyDoesntExist);
    }

    /* check if the new name of the policy already exists or not */
    if (usmDbDiffServPolicyNameToIndex(unit, strNewName, &newPolicyId) == L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservPolicyExists);
    }

    /* set the new name of the policy */
    if (usmDbDiffServPolicyNameSet(unit, policyId, strNewName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_PolicyRenameFailed);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Core routine to add / delete a policy to an interface in a
*           particular direction
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
* @param direction
*
* @returntype const L7_char8
*
* @returns L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t cliServicePolicy(EwsContext ewsContext,
                         uintf argc,
                         const L7_char8 * * argv,
                         uintf index,
                         L7_uint32 direction)
{
  L7_uint32 attachedPolicyId = 0;
  /* L7_BOOL deletedNothing = L7_TRUE; */
  char * pErrMsg = L7_NULLPTR;
  L7_uint32 argPolicyName = 1;
  L7_uint32 policyId = 0;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 interface;
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 validInterfaces[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL allSupported = L7_FALSE;
  L7_BOOL slotPortSupported = L7_FALSE;
  /* L7_uint32 rc; */
  L7_BOOL needCreate = L7_FALSE;
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF;
  L7_uint32 intfType;
  L7_uint32 unit;

  L7_uint32 numArg;

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  memset (validInterfaces, 0, sizeof(validInterfaces));
  cliDiffservBuildValidIntfTypes(direction,
                                 validInterfaces,
                                 sizeof(validInterfaces),
                                 &slotPortSupported,
                                 &allSupported );

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_SUPPORTS_LAG_INTF_FEATURE_ID) == L7_TRUE)
  {
    inclIntfTypes |= USM_LAG_INTF;
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    if (direction == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf),  pStrErr_qos_ServicePolicyIn);
      }
      else if (ewsContext->commType == CLI_NO_CMD)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf),  pStrErr_qos_NoServicePolicyIn);
      }
    }
    else
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf),  pStrErr_qos_ServicePolicyOut);
      }
      else if (ewsContext->commType == CLI_NO_CMD)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf),  pStrErr_qos_NoServicePolicyOut);
      }
    }
    ewsTelnetWrite( ewsContext, buf);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* second argument validation */

  /* verify the length of <policyname> argument */
  if (strlen(argv[index+ argPolicyName]) > L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_PolicyNameLen);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  OSAPI_STRNCPY_SAFE(strPolicyName,argv[index+ argPolicyName]);

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strPolicyName) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_PolicyNameAlnum);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* verify if this policy already exists or not */
    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservPolicyDoesntExist);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* get interface type as diffserv policy can ONLY be attached to physical interfaces
     * or LAG interfaces (if platform allows it)
     */
    if (usmDbIntfTypeGet(interface, &intfType) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }
    /* both lag and physical interface supports diff serve */
    if ( ((inclIntfTypes & USM_LAG_INTF) != 0) )
    {
      if ( (intfType != L7_PHYSICAL_INTF) &&
          (intfType != L7_LAG_INTF) )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_DiffservPortNotPhyNotPchannel,
                                cliDisplayInterfaceHelp(unit, slot, port));
        ewsTelnetWrite(ewsContext, buf);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_DiffservOnlyOnVlanIntfs);
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }
    }
    else if ( intfType != L7_PHYSICAL_INTF )
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_DiffservPortNotPhy,
                              cliDisplayInterfaceHelp(unit, slot, port));
      ewsTelnetWrite(ewsContext, buf);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_DiffservOnlyOnPhyPorts);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDiffServIsAclIntfInUse( unit, interface, direction ) == L7_TRUE &&
          usmDbFeaturePresentCheck( unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_SERVICE_ACL_COEXIST_FEATURE_ID ) == L7_FALSE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_AclAndDiffservOnSameIntf); /*Error:  ACL and diffServ can not be used on the same interface.*/
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }

      /* check if row is already created for this interface and direction */
      if (usmDbDiffServServiceGet(unit, interface, direction ) != L7_SUCCESS)
      {
        needCreate = L7_TRUE;
      }

      /* if necessary, create a new entry in the service table */
      if (needCreate == L7_TRUE)
      {
        /* create a new entry in the service table */
        if (usmDbDiffServServiceCreate(unit, interface, direction, L7_TRUE ) != L7_SUCCESS)
        {
          /* delete the row entry just created */
          usmDbDiffServServiceDelete(unit, interface, direction);
          ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AttachPolicy);
          cliSyntaxBottom(ewsContext);
          return L7_FAILURE;
        }
      }

      /* set the policy index */
      if (usmDbDiffServServicePolicyIndexSet(unit,interface, direction, policyId ) != L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AttachPolicy);
        /* delete the row entry just created*/
        if (needCreate == L7_TRUE)
        {
          usmDbDiffServServiceDelete(unit, interface, direction);
        }

        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }
      /* make sure row status is active */
      if (needCreate == L7_TRUE)
      {
        if (usmDbDiffServServiceRowStatusVerify(unit, interface, direction,
                                                L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE) != L7_SUCCESS)
        {
          /* delete the row entry just created*/
          usmDbDiffServServiceDelete(unit, interface, direction);
          ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AttachPolicy_1);
          cliSyntaxBottom(ewsContext);
          return L7_FAILURE;
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    pErrMsg = L7_NULLPTR;
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      do
      {
        /* get the attached policy index */
        if (usmDbDiffServServicePolicyIndexGet(unit,interface, direction,
                                               &attachedPolicyId ) != L7_SUCCESS)
        {
          pErrMsg = pStringAddCR(pStrInfo_qos_RemovePolicy, 1, 0, 0, 0, pStrErr_common_CouldNot);
          continue;
        }

        /* verify if specified policy is same or not */
        if (attachedPolicyId != policyId)
        {
          pErrMsg = pStrInfo_qos_PolicyIsNotAttachedToIntfInSpecifiedDirection;
          continue;
        }

        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          /* delete the specified row entry */
          if (usmDbDiffServServiceDelete(unit,interface, direction ) != L7_SUCCESS)
          {

            pErrMsg = pStringAddCR(pStrInfo_qos_RemovePolicy, 1, 0, 0, 0, pStrErr_common_CouldNot);
            continue;
          }
        }

        /* make sure row was deleted */
        if (usmDbDiffServServiceGet(unit, interface, direction) == L7_SUCCESS)
        {
          pErrMsg = pStringAddCR(pStrInfo_qos_SuccessfullyRemoveDiffservServiceIntf, 1, 0, 0, 0, pStrErr_common_CouldNot);
          continue;
        }
      } while (0);

      /* Log error message */
      if (pErrMsg != L7_NULLPTR)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
        ewsTelnetWrite( ewsContext, pErrMsg);
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Core routine to add / delete all policies to an interface in a
*           particular direction
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
* @param direction
*
* @returntype L7_RC_t
*
* @returns L7_RC_t
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t cliServicePolicyAll(EwsContext ewsContext,
                            uintf argc,
                            const L7_char8 * * argv,
                            uintf index,
                            L7_uint32 direction)
{

  L7_uint32 attachedPolicyId = 0;
  L7_uint32 argPolicyName = 1;

  L7_uint32 policyId = 0;
  /* L7_uint32 slot; */
  /* L7_uint32 port; */
  L7_uint32 interface;

  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 validInterfaces[L7_CLI_MAX_STRING_LENGTH];

  L7_BOOL allSupported = L7_FALSE;
  L7_BOOL slotPortSupported = L7_FALSE;
  L7_BOOL deleteError = L7_TRUE;
  L7_uint32 rc;
  /* L7_BOOL needCreate = L7_FALSE; */
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF;
  L7_uint32 exclIntfTypes = 0;

  char * pErrMsg = L7_NULLPTR;

  L7_uint32 unit;

  L7_uint32 numArg;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  memset (validInterfaces, 0, sizeof(validInterfaces));
  cliDiffservBuildValidIntfTypes(direction,
                                 validInterfaces,
                                 sizeof(validInterfaces),
                                 &slotPortSupported,
                                 &allSupported );

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_SUPPORTS_LAG_INTF_FEATURE_ID) == L7_TRUE)
  {
    inclIntfTypes |= USM_LAG_INTF;
  }

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    memset (buf, 0, sizeof(buf));
    if (direction == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf),  pStrErr_qos_ServicePolicyIn);
      }
      else if (ewsContext->commType == CLI_NO_CMD)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf),  pStrErr_qos_NoServicePolicyIn);
      }
    }
    else
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf),  pStrErr_qos_ServicePolicyOut);
      }
      else if (ewsContext->commType == CLI_NO_CMD)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  buf, sizeof(buf),  pStrErr_qos_NoServicePolicyOut);
      }
    }
    ewsTelnetWrite( ewsContext, buf);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  /* Verify support for all using configurator */
  if (allSupported != L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_AllOptionNotSupported);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  /* second argument validation */

  /* verify the length of <policyname> argument */
  if (strlen(argv[index+ argPolicyName]) > L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_PolicyNameLen);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  OSAPI_STRNCPY_SAFE(strPolicyName,argv[index+ argPolicyName]);

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strPolicyName) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_PolicyNameAlnum);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }
  /* verify if this policy already exists or not */
  else if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservPolicyDoesntExist);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* verify acl is not in use on the requested interface(s)*/
      /* interface = get first interface, if fail, exit */
      if (usmDbIntIfNumTypeFirstGet( unit, inclIntfTypes, exclIntfTypes,
                                    &interface ) != L7_SUCCESS)
      {
        osapiSnprintfAddBlanks (0, 0, 0, 0, pStrErr_common_Error, buf, sizeof(buf),  pStrErr_common_ServiceBeAddedRetryUsingSlotPort, cliSyntaxInterfaceHelp());
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }

      do
      {
        if (usmDbDiffServIsAclIntfInUse( unit, interface, direction ) == L7_TRUE &&
            usmDbFeaturePresentCheck( unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                     L7_DIFFSERV_SERVICE_ACL_COEXIST_FEATURE_ID ) == L7_FALSE)
        {
          ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_AclAndDiffservOnSameIntf); /*Error:  ACL and diffServ can not be used on the same interface.*/
          cliSyntaxBottom(ewsContext);
          return L7_FAILURE;
        }

        rc = usmDbIntIfNumTypeNextGet( unit, inclIntfTypes, exclIntfTypes, interface, &interface );
        if (rc != L7_SUCCESS)
        {
          break;
        }

      } while (1);   /*infinite loop (allInterfaces == L7_TRUE);*/

      interface = L7_ALL_INTERFACES;

      /* create a new entry in the service table */
      if (usmDbDiffServServiceCreate(unit, interface, direction, L7_TRUE ) != L7_SUCCESS)
      {
        /* delete the row entry just created */
        usmDbDiffServServiceDelete(unit, interface, direction);
        ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_qos_AttachPolicy);
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }

      /* set the policy index */
      if (usmDbDiffServServicePolicyIndexSet(unit,interface, direction, policyId ) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AttachPolicy);
        /* delete the row entry just created*/
        usmDbDiffServServiceDelete(unit, interface, direction);
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }

      /* make sure row status is active */
      if (usmDbDiffServServiceRowStatusVerify(unit, interface, direction,
                                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServServiceDelete(unit, interface, direction);
        ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_AttachPolicy_1);
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      pErrMsg = L7_NULLPTR;

      /* scan all attached policies */
      interface = 0;
      while (usmDbIntIfNumTypeNextGet(unit, inclIntfTypes, exclIntfTypes,
                                      interface, &interface ) == L7_SUCCESS)
      {
        /* reject if any interface contains policy other than the one specified */
        if (usmDbDiffServServicePolicyIndexGet(unit,interface, direction,
                                               &attachedPolicyId ) == L7_SUCCESS)
        {
          if (attachedPolicyId == policyId)
          {
            if (usmDbDiffServServiceDelete(unit, interface, direction ) != L7_SUCCESS)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ServiceCant);
              cliSyntaxBottom(ewsContext);
              return L7_FAILURE;
            }
            else
            {
              /* successfully deleted */
              deleteError=L7_FALSE;
            }
          }
        }
      } /* endwhile */

      if (deleteError == L7_TRUE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_PolicyNotAttached);
        cliSyntaxBottom(ewsContext);
        return L7_FAILURE;
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Adds a policy to an interface in the 'in' direction
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
* @cmdsyntax  config if-number #> service-policy in <policy-map-name>
*
* @cmdhelp
*
* @cmddescript  Attaches a policy to an interface in a particular direction
*
* @end
*
*********************************************************************/
const L7_char8 *commandServicePolicyIn(EwsContext ewsContext,
                                       uintf argc,
                                       const L7_char8 * * argv,
                                       uintf index)
{
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliServicePolicy( ewsContext, argc, argv, index,
                       L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN ) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Adds a policy to an interface in the 'out' direction
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
* @cmdsyntax  config if-number #> service-policy output <policy-map-name>
*
* @cmdhelp
*
* @cmddescript  Attaches a policy to an interface in a particular direction
*
* @end
*
*********************************************************************/
const L7_char8 *commandServicePolicyOut(EwsContext ewsContext,
                                        uintf argc,
                                        const L7_char8 * * argv,
                                        uintf index)
{

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliServicePolicy( ewsContext, argc, argv, index,
                       L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT ) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Adds a policy to all interfaces in the 'in' direction
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
*
* @cmdsyntax  config #> service-policy input <policy-map-name>
*
*
* @cmdhelp
*
* @cmddescript  Attaches a policy to an interface in a particular direction
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandServicePolicyInAll(EwsContext ewsContext,
                                          uintf argc,
                                          const L7_char8 * * argv,
                                          uintf index)
{
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliServicePolicyAll( ewsContext, argc, argv, index,
                          L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN ) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Adds a policy to all interfaces in the 'out' direction
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
*
* @cmdsyntax  config #> service-policy output <policy-map-name>
*
*
* @cmdhelp
*
* @cmddescript  Attaches a policy to an interface in a particular direction
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandServicePolicyOutAll(EwsContext ewsContext,
                                           uintf argc,
                                           const L7_char8 * * argv,
                                           uintf index)
{

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  if (cliServicePolicyAll( ewsContext, argc, argv, index,
                          L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT ) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enter the class-map mode
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
* @cmdsyntax  class-map match-any <class-map-name> [ipv4|ipv6]
* @cmdsyntax  class-map match-all <class-map-name> [ipv4|ipv6]
* @cmdsyntax  class-map match-access-group <class-map-name> <aclid>
* @cmdsyntax  class-map match-mac-access-group <class-map-name> <aclname> [ipv4|ipv6]
*
* @cmdsyntax  class-map <class-map-name>
*
* @cmdsyntax  no class-map <class-map-name>
*
* @cmdhelp
*
* @cmddescript  create/delete a class-map and enter the class-map mode.
*
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliClassMapMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 * prompt;
  L7_RC_t result;
  L7_uint32 classId = 0;
  L7_uint32 clIndx = 1;
  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strType[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1];
  L7_char8 strL3Protocol[L7_CLI_MAX_STRING_LENGTH];
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t diffservType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL;
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t l3protocol = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4;
  L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_NONE;
  L7_uint32 aclNumber = 0;
  L7_uint32 unit =0;

  /* set mode attrs*/
  L7_char8 * tmpPrompt = pStrInfo_qos_ClassMapModePrompt;
  L7_char8 * tmpUpPrompt = pStrInfo_common_GlobalCfgModePrompt;
  L7_uint32 depth = index + 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType != CLI_NO_CMD)
  {
    prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc, argv, ewsContext, cliGetMode(L7_GLOBAL_CONFIG_MODE));
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
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  if (argc >= depth + 1  && argc <=depth+4 && mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_CLASS_MAP_MODE)
      && mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_CLASS_MAP_IPV6_MODE))
  {

    if (argc>depth + 1)
    {
      OSAPI_STRNCPY_SAFE(strType,argv[depth]);

      if (strcmp(strType,pStrInfo_qos_DiffservMatchAccess) ==0)
      {
        diffservType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL;
        aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP;
      }
      else if (strcmp(strType,pStrInfo_qos_DiffservMatchMacAccess) ==0)
      {
        diffservType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL;
        aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC;
      }
      else if (strcmp(strType, pStrInfo_qos_DiffservMatchAnyType) ==0)
      {
        diffservType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY;
      }
      else if (strcmp(strType, pStrInfo_qos_DiffservMatchAllType) ==0)
      {
        diffservType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL;
      }
      else
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_InvalidPolicyClassType);
        ewsTelnetWrite( ewsContext,pStrErr_qos_DiffservMatchInvalidType);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      if (strlen(argv[depth +clIndx]) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX)
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_ClassNameTooLong);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      OSAPI_STRNCPY_SAFE(strName,argv[depth+clIndx]);
    }
    else
    {
      if (strlen(argv[depth]) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX)
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_ClassNameTooLong);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }

      OSAPI_STRNCPY_SAFE(strName,argv[depth]);
    }

    if (ewsContext->commType != CLI_NO_CMD && argc > depth + 1)
    {
      if (diffservType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
      {
        if (argc < depth + 3)
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWrite(ewsContext, pStrErr_qos_AclTo);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP)
        {
          /*
           * see if next parameter is numeric; that implies a numbered IPv4 ACL, otherwise treat parameter as
           * the name of an IPv6 or named IPv4 ACL
           */
          if (cliCheckIfInteger((L7_char8 *)argv[depth+2]) == L7_SUCCESS)
          {
            if (cliConvertTo32BitUnsignedInteger(argv[depth+2], &aclNumber) != L7_SUCCESS)
            {
              cliSyntaxTop(ewsContext);
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidAclNum);
              cliSyntaxBottom(ewsContext);
              return NULL;
            }
          }
          else
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              /* non-integer input given, check if this is a named IP ACL */
              osapiStrncpySafe(strType, argv[depth+2], sizeof(strType));
              if (usmDbQosAclNameToIndex(unit, strType, &aclNumber) != L7_SUCCESS)
              {
                cliSyntaxTop(ewsContext);
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidIp);
                cliSyntaxBottom(ewsContext);
                return NULL;
              }
              if (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IPV6, aclNumber) == L7_SUCCESS)
              {
                l3protocol = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6;
              }
            }
          }
        }
        else if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC)
        {
          /*******Check if the Flag is Set for Execution*************/
          if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            OSAPI_STRNCPY_SAFE(strType,argv[depth+2]);
            if (usmDbDiffServAclMacNameToIndex(unit, strType, &aclNumber) != L7_SUCCESS)
            {
              cliSyntaxTop(ewsContext);
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidMacAclName);
              cliSyntaxBottom(ewsContext);
              return NULL;
            }
          }
          if (argc > (depth+3))
          {
            osapiStrncpySafe(strL3Protocol, argv[depth+3], sizeof(strL3Protocol));
            if (strcmp(strL3Protocol, pStrInfo_common_Diffserv_5) == 0)
            {
              l3protocol = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6;
            }
            else if (strcmp(strL3Protocol, pStrInfo_common_Diffserv_4) == 0)
            {
              /*
               * this local variable is set to this value by default, this check is to assure
               * the provided l3 protocol keyword is decipherable
               */
              l3protocol = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4;
            }
            else
            {
              /* we were provided a l3 protocol keyword we don't know how to handle */
              cliSyntaxTop(ewsContext);
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDiffservClassSsl);
              cliSyntaxBottom(ewsContext);
              return NULL;
            }
          }
        }
        else
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidAclType);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }

        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (cliDiffservClassCreate( ewsContext, strName,
                                     L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL,
                                     l3protocol,
                                     aclType, aclNumber ) != L7_SUCCESS)
          {
            cliSyntaxBottom(ewsContext);
            return NULL;
          }
        }
        OSAPI_STRNCPY_SAFE(EWSCLASSMAP(ewsContext),strName);
        EWSCLASSMAPACLTYPE(ewsContext) = aclType;
        EWSCLASSMAPACLID(ewsContext) = aclNumber;

        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

        if (l3protocol == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
        {
          return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_CLASS_MAP_MODE));
        }
        else
        {
          return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_CLASS_MAP_IPV6_MODE));
        }
      }
      else
      {
        /* check if an optional l3 protocol keyword has been provided;
         * if none given, we default to ipv4
         */
        if (argc == depth + 3)
        {
          osapiStrncpySafe(strL3Protocol, argv[depth+2], sizeof(strL3Protocol));
          if (strcmp(strL3Protocol, pStrInfo_common_Diffserv_5) == 0)
          {
            l3protocol = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6;
          }
          else if (strcmp(strL3Protocol, pStrInfo_common_Diffserv_4) == 0)
          {
            /*
             * this local variable is set to this value by default, this check is to assure
             * the provided l3 protocol keyword is decipherable
             */
            l3protocol = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4;
          }
          else
          {
            /* we were provided a l3 protocol keyword we don't know how to handle */
            cliSyntaxTop(ewsContext);
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidDiffservClassSsl);
            cliSyntaxBottom(ewsContext);
            return NULL;
          }
        }
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (cliDiffservClassCreate( ewsContext, strName, diffservType, l3protocol, aclType, aclNumber ) != L7_SUCCESS)
          {
            cliSyntaxBottom(ewsContext);
            return NULL;
          }
        }
        OSAPI_STRNCPY_SAFE(EWSCLASSMAP(ewsContext),strName);

        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

        if (l3protocol == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
        {
          return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_CLASS_MAP_MODE));
        }
        else if (l3protocol == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
        {
          return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_CLASS_MAP_IPV6_MODE));
        }
      }
    }
    else if (argc== depth +1)
    {

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        result = usmDbDiffServClassNameToIndex(unit, strName, &classId);
        if (ewsContext->commType != CLI_NO_CMD && result == L7_SUCCESS)
        {
          OSAPI_STRNCPY_SAFE(EWSCLASSMAP(ewsContext),strName);

          /*************Set Flag for Script Success******/
          ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

          result = usmDbDiffServClassL3ProtoGet(unit, classId, &l3protocol);
          if (L7_SUCCESS == result)
          {
            if (l3protocol == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
            {
              return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_CLASS_MAP_MODE));
            }
            else if (l3protocol == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
            {
              return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_CLASS_MAP_IPV6_MODE));
            }
          }
        }
        else if (ewsContext->commType == CLI_NO_CMD && result == L7_SUCCESS)
        {
          /* delete the specified row entry in the class table */
          if (usmDbDiffServClassDelete(unit, classId ) != L7_SUCCESS)
          {
            cliSyntaxTop(ewsContext);
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext,pStrErr_qos_CantDelDiffServClass);
            cliSyntaxBottom(ewsContext);
            return NULL;
          }
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
        else
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_ClassName_2);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
      }
    }
  }
  else
  {
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    cliSyntaxBottom(ewsContext);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxBottom(ewsContext);
  return NULL;
}

/*********************************************************************
*
* @purpose  Enter the policy-map mode
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
* @cmdsyntax  policy-map <policy-name> {in | out}
*
* @cmdsyntax  policy-map <policy-name>
*
* @cmdsyntax  no policy-map <policy-name>
*
* @cmdhelp
*
* @cmddescript  create/delete a policy-map and enter the policy-map mode.
*
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliPolicyMapMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit =0;
  L7_char8 * prompt = NULL;
  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t result;
  L7_uint32 policyId = 0;
  L7_char8 strType[L7_CLI_MAX_STRING_LENGTH];
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  /* set mode attrs*/
  L7_char8 * tmpPrompt = pStrInfo_qos_PolicyMapModePrompt;
  L7_char8 * tmpUpPrompt = pStrInfo_common_GlobalCfgModePrompt;
  L7_uint32 depth = index + 1;

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
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }
  strType[0] = L7_EOS;

  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_POLICY_MAP_MODE))
  {
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }
  else if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (argc == (depth+1))
    {
      if ((strcmp(strType, pStrInfo_common_AclInStr) == 0) ||
          (strcmp(strType, pStrInfo_common_AclOutStr) == 0))
      {
        if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                      L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE) ||
            (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                      L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE ))
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_PolicyMap);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
      }
    }
    else if (argc != (depth+2))
    {
      if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                    L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE) ||
          (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                    L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE ))
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_PolicyMap);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
    }
  }
  else if ((argc != (depth+1)) &&  ewsContext->commType == CLI_NO_CMD)
  {
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_qos_NoPolicyMap);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  /* verify the length of <policyname> argument */
  if (strlen(argv[depth]) > L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX)
  {
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_PolicyNameLen);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  OSAPI_STRNCPY_SAFE(strName,argv[depth]);

  /* verify if the specified string contains all the alpha-numeric characters */
  if (cliIsAlphaNum(strName) != L7_SUCCESS)
  {
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_PolicyNameAlnum);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    result = usmDbDiffServPolicyNameToIndex(unit, strName, &policyId);
    if (result != L7_SUCCESS && ewsContext->commType != CLI_NO_CMD)
    {
      if (argc == depth+2)
      {
        OSAPI_STRNCPY_SAFE(strType,argv[depth+1]);
        cliConvertToLowerCase(strType);

        if (strcmp(strType, pStrInfo_common_AclInStr) == 0)
        {
          policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN;
        }
        else if (strcmp(strType,pStrInfo_common_AclOutStr) == 0)
        {
          policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT;
        }
        else
        {
          cliSyntaxTop(ewsContext);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_PolicyType);
        }
      }
      else
      {
        cliSyntaxTop(ewsContext);
        /* trying to enter policy-map mode but policy doesn't exist*/
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
    }

    if (ewsContext->commType == CLI_NO_CMD && result == L7_SUCCESS)
    {
      /* delete the specified row entry in the policy table */
      if (usmDbDiffServPolicyDelete(unit, policyId ) != L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_DiffservPolicyRemoveFailed);
      }

      /* make sure row was deleted */
      if (usmDbDiffServPolicyGet(unit, policyId) == L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,pStrErr_qos_DiffservPolicyRemoveFailed);
      }

    }
    else if (ewsContext->commType == CLI_NO_CMD && result != L7_SUCCESS)
    {
      cliSyntaxTop(ewsContext);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }
    else if (ewsContext->commType != CLI_NO_CMD && result != L7_SUCCESS)
    {
      /* find the free index in the table*/
      if (usmDbDiffServPolicyIndexNext(unit, &policyId) != L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (1, 0, 1, 0, pStrErr_common_CouldNot,  ewsContext,pStrErr_qos_CreateDiffservPolicy_1);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      if (0 == policyId)
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoMorePolicyCanbeCreated);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }

      if (usmDbDiffServPolicyCreate(unit, policyId, L7_TRUE) != L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (1, 0, 1, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CreateDiffservPolicy_1);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      /* set the name of the policy */
      else if (usmDbDiffServPolicyNameSet(unit, policyId, strName) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServPolicyDelete(unit, policyId);

        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (1, 0, 1, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CreateDiffservPolicy_1);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }

      /* store policy name in context for mode commands */
      OSAPI_STRNCPY_SAFE(EWSPOLICYNAME(ewsContext), strName);

      /* set the policy type*/
      if (usmDbDiffServPolicyTypeSet(unit, policyId, policyType) != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServPolicyDelete(unit, policyId);
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CreateDiffservPolicy);
      }

      /* make sure row status becomes active */
      usmDbDiffServPolicyRowStatusGet(unit, policyId, &status);
      if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        usmDbDiffServPolicyDelete(unit, policyId);
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_qos_CreateDiffservPolicy_1);
      }

      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

      /* enter Policy Map mode */
      return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_POLICY_MAP_MODE));
    }
    else if (ewsContext->commType != CLI_NO_CMD && result == L7_SUCCESS)
    {
      /* policy name exists and trying to create a new one with that name */
      if (argc == depth+2)
      {
        cliSyntaxTop(ewsContext);
        /* don't let user try to create policy with existing name */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservPolicyExists);
      }
      else
      {
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

        /* store policy name in context for mode commands */
        OSAPI_STRNCPY_SAFE(EWSPOLICYNAME(ewsContext), strName);
        /* enter Policy Map mode */
        return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_POLICY_MAP_MODE));
      }
    }
    else
    {
      cliSyntaxTop(ewsContext);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidPolicyMapName);
    }
  }
  else
  {

    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    /* enter Policy Map mode during validation*/
    return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_POLICY_MAP_MODE));
  }

  /*************Set Flag for Script Success******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Enter the policy-class-map mode
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
* @cmdsyntax  class <class-name>
*
* @cmdsyntax  no class <class-name>
*
* @cmdhelp
*
* @cmddescript  attach/detach a class-map with a policy-map and enter the policy-class-map mode.
*
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliPolicyClassMapMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 * prompt;
  L7_uint32 policyId =0;
  L7_uint32 classId = 0;
  L7_uint32 policyInstId = 0;
  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_uint32 unit =0;
  L7_RC_t result;

  /* set mode attrs*/
  L7_char8 * tmpPrompt = pStrInfo_qos_PolicyClassMapModePrompt;
  L7_char8 * tmpUpPrompt = pStrInfo_qos_PolicyMapModePrompt;
  L7_uint32 depth = index + 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType != CLI_NO_CMD)
  {
    prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,cliGetMode(L7_POLICY_MAP_MODE));
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
    cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (argc ==  depth+1 && mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_POLICY_CLASS_MODE))
  {
    /* verify the length of <classname> argument */
    if (strlen(argv[depth]) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX)
    {
      cliSyntaxTop(ewsContext);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_ClassNameTooLong);
    }

    OSAPI_STRNCPY_SAFE(strClassName,argv[depth]);

    /* verify if the specified string contains all the alpha-numeric characters */
    if (cliIsAlphaNum(strClassName) != L7_SUCCESS)
    {
      cliSyntaxTop(ewsContext);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_ClassNameMustBeAlnum);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* verify if this class already exists or not */
      if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_DiffservClassDoesntExist);
      }

      result = usmDbDiffServPolicyNameToIndex(unit, EWSPOLICYNAME(ewsContext), &policyId);
      if (result != L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
      }

      /* verify if class is already associated with the specified policy or not */
      result = usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                                     &policyInstId);

      if (ewsContext->commType == CLI_NORMAL_CMD && result!= L7_SUCCESS)
      {

        /* find the free index in the policy instance table*/
        if (usmDbDiffServPolicyInstIndexNext(unit, policyId, &policyInstId) != L7_SUCCESS)
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext,pStrErr_qos_CantAddDiffservClass);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (0 == policyInstId)
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_NoMoreClassCanbeAdded);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }

        /* create the row entry in the policy instance table */
        if (usmDbDiffServPolicyInstCreate(unit, policyId, policyInstId, L7_TRUE) != L7_SUCCESS)
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantAddDiffservClass);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }

        /* set the class index for the specified policy */
        if (usmDbDiffServPolicyInstClassIndexSet(unit, policyId, policyInstId,classId) != L7_SUCCESS)
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantAddDiffservClass);
          usmDbDiffServPolicyInstDelete(unit, policyId, policyInstId);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }

        /* make sure row status becomes active */
        usmDbDiffServPolicyInstRowStatusGet(unit, policyId, policyInstId, &status);
        if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
        {
          cliSyntaxTop(ewsContext);
          usmDbDiffServPolicyInstDelete(unit, policyId, policyInstId);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CantAddDiffservClass);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }

        OSAPI_STRNCPY_SAFE(EWSSERVICENAME(ewsContext), strClassName);

        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

        return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_POLICY_CLASS_MODE));
      }
      else if (ewsContext->commType == CLI_NORMAL_CMD && result== L7_SUCCESS)
      {
        OSAPI_STRNCPY_SAFE(EWSSERVICENAME(ewsContext), strClassName);

        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

        return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_POLICY_CLASS_MODE));

      }
      else if (ewsContext->commType != CLI_NORMAL_CMD && result== L7_SUCCESS)
      {
        /* delete the row entry in the policy instance table */
        if (usmDbDiffServPolicyInstDelete(unit, policyId, policyInstId) != L7_SUCCESS)
        {
          cliSyntaxTop(ewsContext);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_qos_DiffservCantRemove);
        }

        /* make sure row status becomes destruy */
        usmDbDiffServPolicyInstRowStatusGet(unit, policyId, policyInstId, &status);
      }
      else if (ewsContext->commType != CLI_NORMAL_CMD && result != L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
      }
    }
    else
    {
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_POLICY_CLASS_MODE));
    }
  }
  else
  {
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /*************Set Flag for Script Success******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   Drop for Traffic Class
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
* @cmdsyntax  drop
*
* @cmdhelp
*
* @cmddescript  Specifies that all packets are to be dropped at ingress.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDrop(EwsContext ewsContext,
                            uintf argc,
                            const L7_char8 * * argv,
                            uintf index)
{

  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 policyAttrId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;
  L7_uint32 unit;

  L7_uint32 numArg;

  L7_RC_t check;

  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_Drop_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    OSAPI_STRNCPY_SAFE(strPolicyName,EWSPOLICYNAME(ewsContext));
    OSAPI_STRNCPY_SAFE(strClassName,EWSSERVICENAME(ewsContext));

    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }

    if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservClassDoesntExist);
    }

    /* get the policy type */
    if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,  pStrErr_qos_CfgureDrop);
    }

    /* verify if this class is associated with the specified policy or not */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    check = usmDbDiffServPolicyAttrGetNext(unit,
                                           prevPolicyId,
                                           prevPolicyInstId,
                                           prevPolicyAttrId,
                                           &nextPolicyId,
                                           &nextPolicyInstId,
                                           &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       type can be specified or not */
    while (check != L7_ERROR)
    {
      if (check == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId,
                                                      nextPolicyAttrId,
                                                      &entryType) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDrop);
          }

          /* check if drop is already specified */
          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP)
          {

            return cliPrompt(ewsContext);
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

          prevPolicyAttrId = nextPolicyAttrId;
        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        check = usmDbDiffServPolicyAttrGetNext(unit,
                                               prevPolicyId,
                                               prevPolicyInstId,
                                               prevPolicyAttrId,
                                               &nextPolicyId,
                                               &nextPolicyInstId,
                                               &nextPolicyAttrId);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDrop);
      }
    }

    /* find the free index in the policy attribute table*/
    if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                         &policyAttrId ) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDrop);
    }
    if (0 == policyAttrId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
    }

    /* create the row entry in the policy attribute table */
    if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                      policyAttrId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDrop);
    }
  }

  /* specify the attribute type */
  entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                policyInstId,
                                                policyAttrId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureDrop);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the optional parameter burst size */

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Assign Traffic Class to a queue
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
* @cmdsyntax  assign-queue <queue-id>
*
* @cmdhelp
*
* @cmddescript  Specifies that all packets are to be handled using the specified queue.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAssignQueue(EwsContext ewsContext,
                                   uintf argc,
                                   const L7_char8 * * argv,
                                   uintf index)
{

  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 policyAttrId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0;
  L7_uint32 unit;
  L7_uint32 argQueueId = 1;

  L7_uint32 numArg, queueId;

  L7_RC_t check;

  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_AsSignQueue_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    OSAPI_STRNCPY_SAFE(strPolicyName,EWSPOLICYNAME(ewsContext));
    OSAPI_STRNCPY_SAFE(strClassName,EWSSERVICENAME(ewsContext));

    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }

    if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservClassDoesntExist);
    }
  }

  /* verify if the specified  argument is an integer */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argQueueId], &queueId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_Invalid_3);
  }

  if (queueId < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MIN ||
      queueId > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_qos_IncorrectQueueId,
                            L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MIN, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MAX);

  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* get the policy type */
    if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,  pStrErr_qos_CfgureDrop);
    }

    /* verify if policy type is 'in' for specifying Assign Queue */
    if (policyType != L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AttrNotMeaningfulOtherThanIn);
    }

    /* verify if this class is associated with the specified policy or not */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    check = usmDbDiffServPolicyAttrGetNext(unit,
                                           prevPolicyId,
                                           prevPolicyInstId,
                                           prevPolicyAttrId,
                                           &nextPolicyId,
                                           &nextPolicyInstId,
                                           &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       type can be specified or not */
    while (check != L7_ERROR)
    {
      if (check == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId,
                                                      nextPolicyAttrId,
                                                      &entryType) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureAsSignQueue);
          }

          /* check if assign queue is already specified */
          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE)
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              /* set the new name of the policy */
              if (usmDbDiffServPolicyAttrStmtAssignQueueIdSet(unit, nextPolicyId,
                                                              nextPolicyInstId,
                                                              nextPolicyAttrId,
                                                              queueId) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AsSignQueueFailed);
              }
            }
            return cliPrompt(ewsContext);
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

          prevPolicyAttrId = nextPolicyAttrId;
        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        check = usmDbDiffServPolicyAttrGetNext(unit,
                                               prevPolicyId,
                                               prevPolicyInstId,
                                               prevPolicyAttrId,
                                               &nextPolicyId,
                                               &nextPolicyInstId,
                                               &nextPolicyAttrId);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureAsSignQueue);
      }
    }

    /* find the free index in the policy attribute table*/
    if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                         &policyAttrId ) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureAsSignQueue);
    }
    if (0 == policyAttrId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
    }

    /* create the row entry in the policy attribute table */
    if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                      policyAttrId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureAsSignQueue);
    }
  }

  /* specify the attribute type */
  entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                policyInstId,
                                                policyAttrId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureAsSignQueue);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* set the optional parameter burst size */
    if (usmDbDiffServPolicyAttrStmtAssignQueueIdSet(unit, policyId, policyInstId, policyAttrId, queueId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_AsSignQueueFailed);
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Redirect for Traffic Class
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
* @cmdsyntax  redirect <unit/slot/port>
*
* @cmdhelp
*
* @cmddescript  Redirects all incoming packets to a specific egress interface.
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandRedirect(EwsContext ewsContext,
                                uintf argc,
                                const L7_char8 * * argv,
                                uintf index)
{

  L7_uint32 argSlotPort = 1;
  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 policyAttrId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0, intIfNum;
  L7_uint32 rc;

  L7_uint32 numArg, unit, slot, port;

  L7_RC_t check;

  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_Redirect_1, cliSyntaxInterfaceHelp());
  }
  if (cliIsStackingSupported() == L7_TRUE)
  {

    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%d/%d/%d", unit, slot, port);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *  ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    /* This function also print message on screen, so no need to print it here */
    if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &intIfNum) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    OSAPI_STRNCPY_SAFE(strPolicyName,EWSPOLICYNAME(ewsContext));
    OSAPI_STRNCPY_SAFE(strClassName,EWSSERVICENAME(ewsContext));

    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }

    if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservClassDoesntExist);
    }

    /* get the policy type */
    if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,  pStrErr_qos_CfgureRedirect);
    }

    /* verify if policy type is 'in' for specifying Redirect */
    if (policyType != L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AttrNotMeaningfulOtherThanIn);
    }

    /* verify if this class is associated with the specified policy or not */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    check = usmDbDiffServPolicyAttrGetNext(unit,
                                           prevPolicyId,
                                           prevPolicyInstId,
                                           prevPolicyAttrId,
                                           &nextPolicyId,
                                           &nextPolicyInstId,
                                           &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       type can be specified or not */
    while (check != L7_ERROR)
    {
      if (check == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId,
                                                      nextPolicyAttrId,
                                                      &entryType) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureRedirect);
          }

          /* if redirect is already specified, just update its intf setting */
          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT)
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              if (usmDbDiffServPolicyAttrStmtRedirectIntfSet(unit, nextPolicyId,
                                                             nextPolicyInstId,
                                                             nextPolicyAttrId,
                                                             intIfNum) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_RedirectFailed);
              }
            }
            return cliPrompt(ewsContext);
          }

          /* do not allow redirect if mirror attribute is already specified */
          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_CfgureMirrorRedirectPolicyIn);
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

          prevPolicyAttrId = nextPolicyAttrId;
        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        check = usmDbDiffServPolicyAttrGetNext(unit,
                                               prevPolicyId,
                                               prevPolicyInstId,
                                               prevPolicyAttrId,
                                               &nextPolicyId,
                                               &nextPolicyInstId,
                                               &nextPolicyAttrId);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureRedirect);
      }
    }

    /* find the free index in the policy attribute table*/
    if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                         &policyAttrId ) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureRedirect);
    }
    if (0 == policyAttrId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
    }

    /* create the row entry in the policy attribute table */
    if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                      policyAttrId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureRedirect);
    }
  }

  /* specify the attribute type */
  entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                policyInstId,
                                                policyAttrId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureRedirect);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyAttrStmtRedirectIntfSet(unit, policyId, policyInstId, policyAttrId, intIfNum) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_RedirectFailed);
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Mirror for Traffic Class
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
* @cmdsyntax  mirror <unit/slot/port>
*
* @cmdhelp
*
* @cmddescript  Mirrors all incoming packets to a specific interface.
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandMirror(EwsContext ewsContext,
                              uintf argc,
                              const L7_char8 * * argv,
                              uintf index)
{

  L7_uint32 argSlotPort = 1;
  L7_uint32 classId = 0;
  L7_uint32 policyId = 0;
  L7_uint32 policyInstId = 0;
  L7_uint32 policyAttrId = 0;
  L7_uint32 prevPolicyId = 0;
  L7_uint32 prevPolicyInstId = 0;
  L7_uint32 prevPolicyAttrId = 0;
  L7_uint32 nextPolicyId = 0;
  L7_uint32 nextPolicyInstId = 0;
  L7_uint32 nextPolicyAttrId = 0, intIfNum;
  L7_uint32 rc;

  L7_uint32 numArg, unit, slot, port;

  L7_RC_t check;

  L7_char8 strClassName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPolicyName[L7_CLI_MAX_STRING_LENGTH];

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;

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

  numArg = cliNumFunctionArgsGet();

  /* check number of arguments */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_qos_Mirror, cliSyntaxInterfaceHelp());
  }
  if (cliIsStackingSupported() == L7_TRUE)
  {

    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%d/%d/%d", unit, slot, port);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *  ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    /* This function also print message on screen, so no need to print it here */
    if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &intIfNum) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    OSAPI_STRNCPY_SAFE(strPolicyName,EWSPOLICYNAME(ewsContext));
    OSAPI_STRNCPY_SAFE(strClassName,EWSSERVICENAME(ewsContext));

    if (usmDbDiffServPolicyNameToIndex(unit, strPolicyName, &policyId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservPolicyDoesntExist);
    }

    if (usmDbDiffServClassNameToIndex(unit, strClassName, &classId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_DiffservClassDoesntExist);
    }

    /* get the policy type */
    if (usmDbDiffServPolicyTypeGet(unit, policyId, &policyType) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,  pStrErr_qos_CfgureMirror);
    }

    /* verify if policy type is 'in' for specifying Mirror */
    if (policyType != L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AttrNotMeaningfulOtherThanIn);
    }

    /* verify if this class is associated with the specified policy or not */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    check = usmDbDiffServPolicyAttrGetNext(unit,
                                           prevPolicyId,
                                           prevPolicyInstId,
                                           prevPolicyAttrId,
                                           &nextPolicyId,
                                           &nextPolicyInstId,
                                           &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       type can be specified or not */
    while (check != L7_ERROR)
    {
      if (check == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId,
                                                      nextPolicyAttrId,
                                                      &entryType) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureMirror);
          }

          /* if mirror is already specified, just update its intf setting */
          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR)
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              if (usmDbDiffServPolicyAttrStmtMirrorIntfSet(unit, nextPolicyId,
                                                           nextPolicyInstId,
                                                           nextPolicyAttrId,
                                                           intIfNum) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_MirrorFailed);
              }
            }
            return cliPrompt(ewsContext);
          }

          /* do not allow mirror if redirect attribute is already specified */
          if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_CfgureMirrorRedirectPolicyIn);
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */

          prevPolicyAttrId = nextPolicyAttrId;
        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        check = usmDbDiffServPolicyAttrGetNext(unit,
                                               prevPolicyId,
                                               prevPolicyInstId,
                                               prevPolicyAttrId,
                                               &nextPolicyId,
                                               &nextPolicyInstId,
                                               &nextPolicyAttrId);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureMirror);
      }
    }

    /* find the free index in the policy attribute table*/
    if (usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                         &policyAttrId ) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureMirror);
    }
    if (0 == policyAttrId)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
    }

    /* create the row entry in the policy attribute table */
    if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                      policyAttrId, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureMirror);
    }
  }

  /* specify the attribute type */
  entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                policyInstId,
                                                policyAttrId,
                                                entryType) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      return cliSyntaxReturnPromptAddBlanks(0,1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_CfgureMirror);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDiffServPolicyAttrStmtMirrorIntfSet(unit, policyId, policyInstId, policyAttrId, intIfNum) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_MirrorFailed);
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
