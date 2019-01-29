/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_config_vlan.c
 *
 * @purpose vlan config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  10/29/2000
 *
 * @author  djohnson/Forrest Samuels
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include <datatypes.h>                 /* for converting from IP to integer */
#include "ews.h"
#include "clicommands.h"
#include "clicommands_card.h"
#include "dot1q_api.h"
#include "usmdb_dot1q_api.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_mirror_api.h"
#include "usmdb_util_api.h"

static void printVlanWarning(EwsContext ewsContext)
{
  if ((L7_CONFIG_SCRIPT_NOT_RUNNING == ewsContext->configScriptStateFlag) &&
      (L7_EXECUTE_SCRIPT            == ewsContext->scriptActionFlag))
  {
    ewsTelnetWrite(ewsContext, pStrErr_base_DelayWarning);

  }
}





/*********************************************************************
*
* @purpose  Includes/Excludes a port in/from a VLAN (or autodetect)
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan participation {exclude|include|auto} <1-4094>
*
* @cmdhelp Configure how ports participate in a specific VLAN.
*
* @cmddescript
*   Determines the degree of participation of this port in this VLAN.
*   The values can be:
*         Include: Indicates this port is always a member of this VLAN.
*           This is equivalent to registration fixed.
*         Exclude: Indicates this port is never a member of this VLAN.
*           This is equivalent to registration forbidden.
*         Autodetect: Specifies the port is to be dynamically registered
*           in this VLAN by GVRP. The port will not participate in this
*           VLAN unless a join request is received on this port. This
*           is equivalent to registration normal.
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanParticipation(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;

  L7_uint32 vlanID = 0;
  L7_uint32 s, p, itype;
  L7_uint32 interface = 0;
  L7_uint32 argMode=1;
  L7_uint32 argVlanList=2;
  L7_uint32 unit;
  L7_uint32 numArg, i;         /* New variable Added */
  L7_VLAN_MASK_t cliVlanList;
  L7_char8 *strVlanList;
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uint32 vlanCount = 0;                 /* No. of Vlans to be created */
  L7_BOOL flag = L7_TRUE;
  L7_uint32 vlanList[L7_MAX_VLANS];        /* List of Vlan IDs to be created */
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgVlanParticipation_2);
  }
  /* Checks for the mode */
  if ((strcmp(argv[index+argMode], pStrInfo_common_Excl_1) == 0) ||
      (strcmp(argv[index+argMode], pStrInfo_common_Incl_1) == 0) ||
      (strcmp(argv[index+argMode], pStrInfo_common_Auto2) == 0))
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface) ||
          usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      /* 'participation <exclude/include/auto> <vlan id> <port/all>' */
      /* parse all range */
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbSwPortMonitorIsDestConfigured(unit,interface,L7_NULLPTR)
            == L7_TRUE)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (0, 1, 0, 0, pStrErr_common_Cant,  ewsContext, pStrErr_switching_CantCfgureProbePort);
          continue;
        }

        nimGetIntfType(interface, &itype);
        if (itype == L7_CPU_INTF)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_CantCfgureCpuIntf);
          continue;
        }
        else if (itype == L7_LOGICAL_VLAN_INTF)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CantCfgureVlanRoutingIntf);
          continue;
        }
        else if (usmDbDot1qIsValidIntf(unit, interface) != L7_TRUE)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          continue;
        }
      }

      strVlanList = (L7_char8 *) argv[index + argVlanList];

      if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
      {
         osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1, L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
         ewsTelnetWrite(ewsContext, tempBuf);
         return cliSyntaxReturnPrompt (ewsContext, "");
      }

      memset(&cliVlanList, 0, sizeof(L7_VLAN_MASK_t));

       /* No. of vlans to be created */
      for (i = 0; i < vlanCount; i++)
      {
        vlanID = vlanList[i];

        if ((vlanID < L7_DOT1Q_MIN_VLAN_ID) ||
            (vlanID > L7_DOT1Q_MAX_VLAN_ID))
        {
          flag =L7_FALSE;
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                  "\r\nCannot set vlan participation. VLAN ID %d is out of range\n", vlanID);
          continue;
        }

        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          NIM_INTF_SETMASKBIT(cliVlanList, vlanID);
        }
      }

      if (strcmp(argv[index+argMode], pStrInfo_common_Auto2) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbQportsVlanMembershipMaskSet(unit, interface, &cliVlanList, L7_DOT1Q_NORMAL_REGISTRATION, DOT1Q_SWPORT_MODE_GENERAL);
        }
      }

      else if (strcmp(argv[index+argMode], pStrInfo_common_Incl_1) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbQportsVlanMembershipMaskSet(unit, interface, &cliVlanList, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_GENERAL);
        }
      }
      else if (strcmp(argv[index+argMode], pStrInfo_common_Excl_1) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbQportsVlanMembershipMaskSet(unit, interface, &cliVlanList, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_GENERAL);
        }
      }
      if (flag != TRUE || rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, NULL, ewsContext, pStrErr_switching_FailedToSetVlanParticipation);
      }


    }
  }
  else
  { /* Error condition for invalid mode */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_common_IncorrectInputMode);
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Includes/Excludes a port in/from a VLAN (or autodetect)
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax   vlan participation all <exclude/include/auto> <1-4094>
*
* @cmdhelp Configure how ports participate in a specific VLAN.
*
* @cmddescript
*   Determines the degree of participation of this port in this VLAN.
*   The values can be:
*         Include: Indicates this port is always a member of this VLAN.
*           This is equivalent to registration fixed.
*         Exclude: Indicates this port is never a member of this VLAN.
*           This is equivalent to registration forbidden.
*         Autodetect: Specifies the port is to be dynamically registered
*           in this VLAN by GVRP. The port will not participate in this
*           VLAN unless a join request is received on this port. This
*           is equivalent to registration normal.
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanParticipationAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;

  L7_uint32 vlanID = 0;
  L7_uint32 argMode=1;
  L7_uint32 argVlanID=2;
  L7_uint32 unit;
  L7_uint32 numArg;         /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments */
  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgVlanParticipationAll, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
  }
  /* Checks for the mode */
  if ((strcmp(argv[index+argMode], pStrInfo_common_Excl_1) == 0) ||
      (strcmp(argv[index+argMode], pStrInfo_common_Incl_1) == 0) ||
      (strcmp(argv[index+argMode], pStrInfo_common_Auto2) == 0))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[index+argVlanID]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }
    /* 'participation <exclude/include/auto> <vlan id> <port/all>' */
    sscanf(argv[index+argVlanID], "%d", &vlanID);

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      rc = usmDbVlanIDGet(unit, vlanID);
      if (rc == L7_NOT_EXISTS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_VlanIdNotFound);
      }
      else if (rc == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_DuringLookup);
      }
    }

    /* apply to all interfaces */
    if (strcmp(argv[index+argMode], pStrInfo_common_Auto2) == 0)       /* dynamically detected, normal reg */
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
         printVlanWarning(ewsContext);
        rc = usmDbVlanMemberSet(unit, vlanID, L7_ALL_INTERFACES, L7_DOT1Q_NORMAL_REGISTRATION, DOT1Q_SWPORT_MODE_GENERAL);
      }
    }
    else if (strcmp(argv[index+argMode], pStrInfo_common_Incl_1) == 0)                         /* include, registration fixed */
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        printVlanWarning(ewsContext);
        rc = usmDbVlanMemberSet(unit, vlanID, L7_ALL_INTERFACES, L7_DOT1Q_FIXED, DOT1Q_SWPORT_MODE_GENERAL);
      }
    }
    else if (strcmp(argv[index+argMode], pStrInfo_common_Excl_1) == 0)                         /* exclude, registration fobidden */
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        printVlanWarning(ewsContext);
        rc = usmDbVlanMemberSet(unit, vlanID, L7_ALL_INTERFACES, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_GENERAL);
      }
    }
  }
  else
  { /* Error condition for invalid mode */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_common_IncorrectInputMode);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);


}

/*********************************************************************
*
* @purpose  change the name of a vlan based on the VLAN ID
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan name <2-4094> <name>
*
* @cmdhelp Configure an optional VLAN Name.
*
* @cmddescript
*   An alphanumeric character string of up to L7_MAX_VLAN_NAME characters
*   which identifies the VLAN. The default name is Vlan and the ID number.
*   The name for VLAN ID 1 is always Default.
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanName(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 buf[L7_MAX_VLAN_NAME];
  L7_uint32 argVlanId = 1;
  L7_uint32 argName = 2;
  L7_uint32 vlanID = 0;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if ((ewsContext->commType ==  CLI_NORMAL_CMD) && (numArg != 2))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_VlanName, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
  }
  else if ((ewsContext->commType ==  CLI_NO_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_VlanNameNo, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
  }

  if (cliCheckIfInteger((L7_char8 *)argv[index+argVlanId]) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }
  sscanf(argv[index+argVlanId], "%d", &vlanID);

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
      if (ewsContext->commType ==  CLI_NORMAL_CMD)
      {

        /* verify if the specified string contains all the alpha-numeric characters */
        if (cliIsAlphaNum((L7_char8 *)argv[index+argName]) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_NameMustBeAlnum_2);
        }

        if (strlen(argv[index+argName]) > (L7_MAX_VLAN_NAME-1))
        {
          return cliSyntaxReturnPrompt (ewsContext,pStrInfo_switching_NameMustBeCharsOrLess, (L7_MAX_VLAN_NAME-1));
        }
        OSAPI_STRNCPY_SAFE(buf, argv[index+argName]);
      }
      else
      {
        OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_EmptyString);       /*In case of NO_CMD, the Name is set to blank*/
      }

      if((rc = usmDbVlanNameSet(unit, vlanID, buf)) != L7_SUCCESS)
      {
        switch (rc)
        {
          case L7_ERROR:
                 /* Error! Can not configure the name %s as the VLAN %d is Dynamic */
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_DynamicVlan);
          case L7_NOT_SUPPORTED:
                 /* Error! Can not configure the name for a default VLAN */
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_DefaultVlan);
          case L7_NOT_EXIST:
                 /* Error! VLAN ID not Exists */
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_VlanIdNotFound);
          case L7_NO_VALUE:
                 /* Error! Invalid VLAN ID */
              return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_DuringLookup);
          default:
              break;
        }
      }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  change a dynamic VLAN to static
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan makestatic <2-4094>
*
* @cmdhelp Change the VLAN type from 'Dynamic' to 'Static'.
*
* @cmddescript
*   The type can be the Default VLAN, a static VLAN (one that is
*   permanently configured and defined), or a dynamic VLAN (one that
*   is created by GVRP registration). A VALN maked as "Dynamic" can
*   be made "Static" by toggling in the Type field when the VLAN is
*   being modified.
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanMakeStatic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 vlanID = 0;
  L7_uint32 argVlanId = 1;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_VlanMakeStatic_1, (L7_DOT1Q_MIN_VLAN_ID+1), L7_DOT1Q_MAX_VLAN_ID );
  }

  if (cliCheckIfInteger((L7_char8 *)argv[index+argVlanId]) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  sscanf(argv[index+argVlanId], "%d", &vlanID);

  if (vlanID == L7_DOT1Q_DEFAULT_VLAN)
  {
    return cliSyntaxReturnPrompt (ewsContext,pStrInfo_switching_VlanMakeStatic, L7_DOT1Q_DEFAULT_VLAN);
  }
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbVlanIDGet(unit, vlanID);
    if (rc == L7_NOT_EXISTS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_VlanIdNotFound);
    }
    else if (rc == L7_FAILURE)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_DuringLookup);
    }
    rc = usmDbVlanMakeStaticSet(unit, vlanID);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_DuringConVer);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure acceptable frame type for an interface
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan acceptframe {all|vlanonly|admituntaggedonly}
*
* @cmdhelp Configure acceptable frame type
*
* @cmddescript
*    none
* @end
*
*********************************************************************/
const L7_char8 *commandVlanAcceptFrame(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = 1;
  L7_uint32 s, p, interface;
  L7_uint32 argAdmit=1;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
  {
    if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface) ||
        usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
    {
      continue;
    }
    if (ewsContext->commType ==  CLI_NORMAL_CMD)
    {
      if (numArg != 1)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanAcceptFrame);
      }
      if (strcmp(argv[index+argAdmit], pStrInfo_common_All) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbQportsAcceptFrameTypeSet(unit, interface, L7_DOT1Q_ADMIT_ALL);
        }
      }
      else if (strcmp(argv[index+argAdmit], pStrInfo_common_VlanOnly_1) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbQportsAcceptFrameTypeSet(unit, interface, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED);
        }
      }
      else if (strcmp(argv[index+argAdmit], pStrInfo_common_AdmitUntaggedOnly_1) == 0)
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbQportsAcceptFrameTypeSet(unit, interface, L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanAcceptFrame);
      }
    }
    else if (ewsContext->commType ==  CLI_NO_CMD)
    {
      if (numArg != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanAcceptFrameNo);
      }
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbQportsAcceptFrameTypeSet(unit, interface, L7_DOT1Q_ADMIT_ALL);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        /* port selected cannot perform this function */
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      else if (rc != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_switching_CfgureAcceptableFrameType);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure acceptable frame type for all interfaces
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan port acceptframe all <all/vlanonly/admituntaggedonly>
*
* @cmdhelp Configure acceptable frame type
*
* @cmddescript
*    none
* @end
*
*********************************************************************/
const L7_char8 *commandVlanPortAcceptFrameAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = 1;
  L7_uint32 argAdmit=1;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanPortAcceptFrameAll_1);
    }

    if (strcmp(argv[index+argAdmit], pStrInfo_common_All) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
          printVlanWarning(ewsContext);
        rc = usmDbQportsAcceptFrameTypeSet(unit, L7_ALL_INTERFACES, L7_DOT1Q_ADMIT_ALL);
      }
    }
    else if (strcmp(argv[index+argAdmit], pStrInfo_common_VlanOnly_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
          printVlanWarning(ewsContext);
        rc = usmDbQportsAcceptFrameTypeSet(unit, L7_ALL_INTERFACES, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED);
      }
    }
    else if (strcmp(argv[index+argAdmit], pStrInfo_common_AdmitUntaggedOnly_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
          printVlanWarning(ewsContext);
        rc = usmDbQportsAcceptFrameTypeSet(unit, L7_ALL_INTERFACES, L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanPortAcceptFrameAll_1);
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanPortAcceptFrameAllNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      printVlanWarning(ewsContext);
      rc = usmDbQportsAcceptFrameTypeSet(unit, L7_ALL_INTERFACES, L7_DOT1Q_ADMIT_ALL);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc == L7_NOT_SUPPORTED)
    {
      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_switching_CfgureAcceptableFrameType);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);




}

/*********************************************************************
*
* @purpose  Configure ingress filtering for an interface
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan ingressfilter
*
* @cmdhelp Configure ingress filtering
*
* @cmddescript
*    none
* @end
*
*********************************************************************/
const L7_char8 *commandVlanIngressFilter(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 interface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
  {
    if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface) ||
        usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
    {
      continue;
    }
    if (ewsContext->commType ==  CLI_NORMAL_CMD)
    {
      if (numArg != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanIngressFilter);
      }
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbQportsEnableIngressFilteringSet(unit, interface, L7_ENABLE);
      }
    }
    else if (ewsContext->commType ==  CLI_NO_CMD)
    {
      if (numArg != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanIngressFilterNo);
      }
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbQportsEnableIngressFilteringSet(unit, interface, L7_DISABLE);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanIngressFilter);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        /* port selected cannot perform this function */
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      else if (rc != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_switching_CfgureIngressFiltering);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure ingress filtering for all interfaces
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan port ingressfilter all
*
* @cmdhelp Configure ingress filtering
*
* @cmddescript
*    none
* @end
*
*********************************************************************/
const L7_char8 *commandVlanPortIngressFilterAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanPortIngressFilterAll_1);
    }
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
         printVlanWarning(ewsContext);
      rc = usmDbQportsEnableIngressFilteringSet(unit, L7_ALL_INTERFACES, L7_ENABLE);
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanPortIngressFilterAllNo);
    }
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        printVlanWarning(ewsContext);
      rc = usmDbQportsEnableIngressFilteringSet(unit, L7_ALL_INTERFACES, L7_DISABLE);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_VlanPortIngressFilterAll_1);
  }
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc == L7_NOT_SUPPORTED)
    {
      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_switching_CfgureIngressFiltering);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);



}

/*********************************************************************
*
* @purpose  Configures VLAN PVID
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan pvid <1-4094>
*
* @cmdhelp Configure VLAN PVID
*
* @cmddescript
*    none
* @end
*
*********************************************************************/
const L7_char8 *commandVlanPvid(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = 1;
  L7_uint32 interface = 0;
  L7_uint32 vlanID;
  L7_uint32 s, p;
  L7_uint32 argVlanID=1;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
  {
    if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface) ||
        usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
    {
      continue;
    }
    if (ewsContext->commType ==  CLI_NORMAL_CMD)
    {
      if (numArg != 1)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_VlanPvid, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
      }

      if (cliCheckIfInteger((L7_char8 *)argv[index+argVlanID]) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
      }
      else
      {
        sscanf(argv[index+argVlanID], "%d", &vlanID);
        /* port VLAN ID doesn't have to exist yet, just be in range */
        if ((vlanID < L7_DOT1Q_MIN_VLAN_ID) || (vlanID > L7_DOT1Q_MAX_VLAN_ID))
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrInfo_switching_VlanIdOutOfRange, L7_DOT1Q_MIN_VLAN_ID,L7_DOT1Q_MAX_VLAN_ID);
        }
      }
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbQportsPVIDSet(unit, interface, vlanID);
      }
    }
    else if (ewsContext->commType ==  CLI_NO_CMD)
    {
      if (numArg != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_switching_VlanPvidNo);
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbQportsPVIDSet(unit, interface,1);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        /* port selected cannot perform this function */
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      else if (rc != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_switching_CfgurePvid);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures VLAN PVID
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan port pvid all <1-4094>
*
* @cmdhelp Configure VLAN PVID
*
* @cmddescript
*    none
* @end
*
*********************************************************************/
const L7_char8 *commandVlanPortPvidAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = 1;
  L7_uint32 vlanID;
  L7_uint32 argVlanID=1;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_VlanPortPvidAll_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
    }

    if (cliCheckIfInteger((L7_char8 *)argv[index+argVlanID]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }
    else
    {
      sscanf(argv[index+argVlanID], "%d", &vlanID);
      /* port VLAN ID doesn't have to exist yet, just be in range */
      if ((vlanID < L7_DOT1Q_MIN_VLAN_ID) || (vlanID > L7_DOT1Q_MAX_VLAN_ID))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrInfo_switching_VlanIdOutOfRange, L7_DOT1Q_MIN_VLAN_ID,L7_DOT1Q_MAX_VLAN_ID);
      }
    }
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        printVlanWarning(ewsContext);
      rc = usmDbQportsPVIDSet(unit, L7_ALL_INTERFACES, vlanID);
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_switching_VlanPortPvidAllNo);
    }
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        printVlanWarning(ewsContext);
      rc = usmDbQportsPVIDSet(unit, L7_ALL_INTERFACES,1);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc == L7_NOT_SUPPORTED)
    {

      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_switching_CfgurePvid);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);


}

/*********************************************************************
*
* @purpose  Configures VLAN tagging
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan tagging <vlanid>
*
* @cmdhelp Configure VLAN tagging
*
* @cmddescript
*   Determines the tagging mode for an interface in a VLAN
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanTagging(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 vlanID = 0;
  L7_uint32 interface = 0;
  L7_uint32 s,p;
  L7_uint32 argVlanList = 1;
  L7_uint32 unit,i;
  L7_uint32 numArg;
  L7_VLAN_MASK_t cliVlanList;
  L7_char8 *strVlanList;
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uint32 vlanCount = 0;                 /* No. of Vlans to be created */
  L7_uint32 vlanList[L7_MAX_VLANS];        /* List of Vlan IDs to be created */
  L7_BOOL flag = L7_TRUE;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
     if (ewsContext->commType ==  CLI_NORMAL_CMD)
     {
       return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_VlanTagging_2);
     }
     else
     {
       return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_VlanTaggingNo_1);
     }
  }

  strVlanList = (L7_char8 *) argv[index + argVlanList];

  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1, L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, tempBuf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  memset(&cliVlanList, 0, sizeof(L7_VLAN_MASK_t));

  /* No. of vlans to be created */
  for (i = 0; i < vlanCount; i++)
  {
    vlanID = vlanList[i];

    if ((vlanID < L7_DOT1Q_MIN_VLAN_ID) ||
          (vlanID > L7_DOT1Q_MAX_VLAN_ID))
    {
      flag =L7_FALSE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "\r\nCannot set vlan tagging. VLAN ID %d is out of range\n", vlanID);
      continue;
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      NIM_INTF_SETMASKBIT(cliVlanList, vlanID);
    }
  }

  for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
  {
    if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface) ||
        usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
    {
      continue;
    }
    if (ewsContext->commType ==  CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbQportsVlanTagMaskSet(unit, interface, &cliVlanList, L7_DOT1Q_TAGGED);
      }
    }
    else if (ewsContext->commType ==  CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbQportsVlanTagMaskSet(unit, interface, &cliVlanList, L7_DOT1Q_UNTAGGED);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        /* port selected cannot perform this function */
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }
    }

    if (flag != TRUE || rc != L7_SUCCESS)
    {
      status = L7_FAILURE;
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
      ewsTelnetWriteAddBlanks (1, 1, 0, 0, NULL, ewsContext, pStrErr_switching_FailedToSetVlanTagging);
    }
  }
  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  Configures VLAN tagging for all interfaces
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan port tagging all <vlanid>
*
* @cmdhelp Configure VLAN tagging
*
* @cmddescript
*   Determines the tagging mode for all interfaces in a VLAN
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanPortTaggingAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = 1;
  L7_uint32 vlanID = 0;
  L7_uint32 argVlanID=1;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_VlanPortTaggingAllNo, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
    }

    if (cliCheckIfInteger((L7_char8 *)argv[index+argVlanID]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }
    /* 'tagging <enable/disable> <vlan id> <slot/port/all>' */
    sscanf(argv[index+argVlanID], "%d", &vlanID);
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        printVlanWarning(ewsContext);
      rc = usmDbVlanIDGet(unit, vlanID);
      if (rc == L7_NOT_EXISTS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_VlanIdNotFound);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_DuringLookup);
      }

      rc = usmDbVlanTaggedSet(unit, vlanID, L7_ALL_INTERFACES, L7_DOT1Q_UNTAGGED);
    }
  }
  else if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_VlanPortTaggingAll_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
    }

    if (cliCheckIfInteger((L7_char8 *)argv[index+argVlanID]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }
    /* 'tagging <enable/disable> <vlan id> <slot/port/all>' */
    sscanf(argv[index+argVlanID], "%d", &vlanID);
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        printVlanWarning(ewsContext);
      rc = usmDbVlanIDGet(unit, vlanID);
      if (rc == L7_NOT_EXISTS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_VlanIdNotFound);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_DuringLookup);
      }
      rc = usmDbVlanTaggedSet(unit, vlanID, L7_ALL_INTERFACES, L7_DOT1Q_TAGGED);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc == L7_NOT_SUPPORTED)
    {
      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_switching_CfgureVlanTagging);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);



}

/*********************************************************************
*
* @purpose  create/delete a vlan based on the VLAN ID
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes not well tested
*
* @cmdsyntax  vlan <2-4094>
*
* @cmdhelp Create/Delete a VLAN.
*
* @cmddescript
*   Create/Delete a VLAN based on a VLAN ID. VLAN identifier. It can be
*   any number from 2 to 4094 (ID 1 is reserved for the default VLAN).
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanSwDev(EwsContext ewsContext, L7_uint32 argc,
                                 const L7_char8 ** argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_int32 vlanID = 0;
  L7_uint32 argVlanList = 1;
  L7_char8 *strVlanList;
  L7_uint32 unit;
  L7_uint32 numArg, i;
  L7_uint32 vlanCount = 0;                 /* No. of Vlans to be created */
  L7_uint32 vlanList[L7_MAX_VLANS];        /* List of Vlan IDs to be created */
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_BOOL flag = L7_TRUE;
  L7_VLAN_MASK_t cliVlanList;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg =  cliNumFunctionArgsGet();

  if (numArg != 1)
  {
     if (ewsContext->commType ==  CLI_NORMAL_CMD)
     {
       osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_switching_Vlan_2);
       ewsTelnetWrite( ewsContext, buf);
     }
     else
     {
       osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_switching_VlanNo_1);
       ewsTelnetWrite( ewsContext, buf);
     }
     return cliSyntaxReturnPrompt (ewsContext, "");
  }

  strVlanList = (L7_char8 *) argv[index + argVlanList];

  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1, L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, tempBuf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  memset(&cliVlanList, 0, sizeof(L7_VLAN_MASK_t));

  /* No. of vlans to be created */
  for (i = 0; i < vlanCount; i++)
  {
    vlanID = vlanList[i];
    if ((vlanID < L7_DOT1Q_MIN_VLAN_ID) ||
        (vlanID > L7_DOT1Q_MAX_VLAN_ID))
    {
      flag =L7_FALSE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "\r\nCannot set vlan participation. VLAN ID %d is out of range\n", vlanID);
      continue;
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      L7_VLAN_SETMASKBIT(cliVlanList, vlanID);
    }
  } 
 
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbVlanCreateMaskSet(unit, &cliVlanList);
    }
    /* For script validation set rc to L7_SUCCESS */
    if (ewsContext->scriptActionFlag == L7_VALIDATE_SCRIPT)
    {
      if (flag == L7_TRUE)
      {
        /* The only time we will return a L7_ACTION_FUNC_SUCCESS in the script validate case
         * is when the vlan id is in the right range and the only reason why rc is set to failure
         * is because that valid vlan id does not exist in the system.
         */ 
        rc = L7_SUCCESS;
      }
    }

    if (flag == L7_TRUE && rc == L7_SUCCESS)
    {
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, NULL, ewsContext, pStrErr_switching_FailedToCreateVlan_1);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if(L7_VLAN_ISMASKBITSET(cliVlanList, L7_DOT1Q_DEFAULT_VLAN) == L7_TRUE)
    {
      /* Set the flag to L7_FALSE to indicate error in deleting vlans if
       * default vlan is being specified as one of the input paramters */
      flag = L7_FALSE;
    }
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
       rc = usmDbVlanDeleteMaskSet(unit, &cliVlanList);
    }
    if (flag == TRUE && rc == L7_SUCCESS)
    {
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, NULL, ewsContext, pStrErr_switching_FailedToDeleteVlan_1);
    }
  }  /* End of if (ewsContext->commType == CLI_NO_CMD) */

  return cliPrompt(ewsContext);
}



/**************** Code Addition TM2 ********************/
