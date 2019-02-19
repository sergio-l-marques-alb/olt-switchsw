/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/dot1x/cli_config_dot1x.c
 *
 * @purpose Dot1x config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  3/04/2003
 *
 * @author  jflanagan
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "l7_common.h"
#include "dot1q_exports.h"
#include "dot1x_exports.h"
#include "user_manager_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_port_user.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_dot1x.h"
#include "clicommands_card.h"
#include "cliutil_dot1x.h"

#include "usmdb_user_mgmt_api.h"

/*********************************************************************
*
* @purpose  Enable/Disable dot1x support on the switch
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x system-auth-control
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xSystemAuthControl(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 * * argv,
                                              L7_uint32 index)
{
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliNumFunctionArgsGet() != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xAdminMode);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xNoAdminMode);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xAdminModeSet(unit, mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_security_Dot1xAdminMode_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Initialize the specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  dot1x initialize <slot/port>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xInitialize(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 slot, port, iface;
  L7_uint32 mode;
  L7_uint32 unit;
  L7_RC_t rc;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
 
  if ( cliNumFunctionArgsGet() != 1 )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortInitialize, cliSyntaxInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
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
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS ||
        cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index + argSlotPort], &slot, &port, &iface) != L7_SUCCESS ||
        cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

  }
  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                pStrErr_security_Dot1xPortInitialize_1);
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeAuth);
        return cliPrompt(ewsContext);
 }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xPortInitializeSet(unit, iface, L7_TRUE) != L7_SUCCESS )
    {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortInitialize_1);

      if (usmDbDot1xAdminModeGet(unit, &mode) == L7_SUCCESS &&
          mode == L7_DISABLE )
      {
        /* Adminmode must be enabled */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_Dot1xMustEnblAdminMode);
      }
      else if ( usmDbDot1xPortOperControlModeGet(unit, iface, &mode) == L7_SUCCESS
               && mode != L7_DOT1X_PORT_AUTO)
      {
        /* Port mode must be auto */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_Dot1xPortModeAutoOrMacBased);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Reauthenticate the specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  dot1x re-authenticate <slot/port>
*
* @cmdhelp This command begins the reauthentication sequence.
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xReauthenticate(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 slot, port, iface;
  L7_uint32 mode;
  L7_uint32 unit;
  L7_RC_t rc;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliNumFunctionArgsGet() != 1 )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortReAuth, cliSyntaxInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
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
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS ||
        cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index + argSlotPort], &slot, &port, &iface) != L7_SUCCESS ||
        cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                pStrErr_security_Dot1xPortReAuthPort);
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeAuth);
       return cliPrompt(ewsContext);
     
 }


  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xPortReauthenticateSet(unit, iface, L7_TRUE) != L7_SUCCESS )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortReAuthPort);

      if (usmDbDot1xAdminModeGet(unit, &mode) == L7_SUCCESS &&
          mode == L7_DISABLE )
      {
        /* Adminmode must be enabled */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_Dot1xMustEnblAdminMode);
      }
      else if ( usmDbDot1xPortOperControlModeGet(unit, iface, &mode) == L7_SUCCESS
               && mode != L7_DOT1X_PORT_AUTO)
      {
        /* Port mode must be auto */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_Dot1xPortModeAutoOrMacBased);
      }
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set the dot1x control direction on a specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  dot1x control-dir <slot/port | all> <both | in>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xControlDir(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 slot, port, iface;

  L7_uint32 argMode = 2;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 mode;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL allPorts = L7_FALSE;
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF;
  L7_uint32 interfaceCount = 0;
  L7_uint32 exclIntfTypes = 0;
  L7_uint32 unit;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliNumFunctionArgsGet() != 2 )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortCntrlDir, cliSyntaxInterfaceHelp());
  }

  if (strlen(argv[index+argSlotPort]) >= sizeof(strSlotPort))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortCntrlDir, cliSyntaxInterfaceHelp());
  }

  osapiStrncpySafe( strSlotPort, argv[index+argSlotPort], sizeof(strSlotPort));
  cliConvertToLowerCase(strSlotPort);
  if (strcmp(strSlotPort, pStrInfo_common_All) == 0 )  /* "all" */
  {
    allPorts = L7_TRUE;
  }
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
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
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS ||
          cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
      }
    }
    else
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, strSlotPort, &slot, &port, &iface) != L7_SUCCESS ||
          cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
  }

  if (strlen(argv[index+argMode]) >= sizeof(strMode))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortCntrlDir, cliSyntaxInterfaceHelp());
  }
  osapiStrncpySafe(strMode,argv[index + argMode], sizeof(strMode));
  cliConvertToLowerCase(strMode);

  if (strcmp(strMode, pStrInfo_common_Both_1) == 0) /* both */
  {
    mode = L7_DOT1X_PORT_DIRECTION_BOTH;
  }
  else if (strcmp(strMode, pStrInfo_common_AclInStr) == 0) /* in */
  {
    mode = L7_DOT1X_PORT_DIRECTION_IN;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortCntrlDir, cliSyntaxInterfaceHelp());
  }

  if (allPorts == L7_TRUE)
  {
    /* interface = get first interface, if fail, exit */
    if ( usmDbIntIfNumTypeFirstGet( unit, inclIntfTypes, exclIntfTypes,
                                   &iface ) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xNoPortAvailable);
    }
  }
  do
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xPortAdminControlledDirectionsSet(unit, iface, mode) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortCntrlDir);
      }
    }
    interfaceCount++;
    if (allPorts == L7_TRUE &&
        usmDbIntIfNumTypeNextGet( unit, inclIntfTypes, exclIntfTypes,
                                 iface, &iface ) != L7_SUCCESS )
    {
      return cliPrompt(ewsContext);
    }
  } while (allPorts == L7_TRUE);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set the dot1x control mode on a specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x port-control <auto | force-authorized | force-unauthorized | mac-based > 
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xPortControl(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 slot, port, iface;
  L7_uint32 numArgs;
  L7_uint32 argMode = 1;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 mode = L7_DOT1X_PORT_AUTO;
  L7_uint32 unit;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 1 )
    {
        if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)!=L7_TRUE) 
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode);
        }
        else
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode_MacBased);
        }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArgs != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xNoPortCntrlMode);
    }

  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }
  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                pStrErr_security_Dot1xPortCntrlMode);
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeAuth);
    return cliPrompt(ewsContext);

 }


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argMode]) >= sizeof(strMode))
    {
        if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)!=L7_TRUE) 
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode);
        }
        else
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode_MacBased);
        }
    }
    osapiStrncpySafe(strMode,argv[index + argMode], sizeof(strMode));
    cliConvertToLowerCase(strMode);

    if (strcmp(strMode, pStrInfo_common_Auto2) == 0) /* auto */
    {
      mode = L7_DOT1X_PORT_AUTO;
    }
    else if (strcmp(strMode, pStrInfo_security_ForceAuthorizedDot1x) == 0)
    {
      mode = L7_DOT1X_PORT_FORCE_AUTHORIZED;
    }
    else if (strcmp(strMode, pStrInfo_security_ForceUnAuthorizedDot1x) == 0)
    {
      mode = L7_DOT1X_PORT_FORCE_UNAUTHORIZED;
    }
    else if (strcmp(strMode, pStrInfo_security_MacBasedDot1x) == 0)
    {
       if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)==L7_TRUE)  
       {
            mode = L7_DOT1X_PORT_AUTO_MAC_BASED;
       }
       else
       {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode);
       }
    }
    else
    {
        if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)!=L7_TRUE) 
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode);
        }
        else
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode_MacBased);
        }
        
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DOT1X_PORT_AUTO;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xPortControlModeSet(unit, iface, mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortCntrlMode);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  Set the dot1x control mode for all port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x port-control all <auto | force-authorized | force-unauthorized>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xPortControlAll(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  L7_uint32 iface;
  L7_uint32 numArgs;
  L7_uint32 argMode = 1;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 mode = L7_DOT1X_PORT_AUTO;
  L7_BOOL allPorts = L7_FALSE;
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF;
  L7_uint32 interfaceCount = 0;
  L7_uint32 exclIntfTypes = 0;
  L7_uint32 unit;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 1 )
    {
      /*return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlModeAll);*/
      if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)!=L7_TRUE) 
      {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlModeAll);
      }
      else
      {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode_MacBased_All);
      }  
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArgs != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xNoPortCntrlModeAll);
    }

  }

  allPorts = L7_TRUE;    /* calling the commandDot1xPortControlAll function is always going to mean
                            all ports */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argMode]) >= sizeof(strMode))
    {
        if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)!=L7_TRUE) 
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlModeAll);
        }
        else
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode_MacBased_All);
        }
    }
    osapiStrncpySafe(strMode,argv[index + argMode], sizeof(strMode));
    cliConvertToLowerCase(strMode);

    if (strcmp(strMode, pStrInfo_common_Auto2) == 0) /* auto */
    {
      mode = L7_DOT1X_PORT_AUTO;
    }
    else if (strcmp(strMode, pStrInfo_security_ForceAuthorizedDot1x) == 0)
    {
      mode = L7_DOT1X_PORT_FORCE_AUTHORIZED;
    }
    else if (strcmp(strMode, pStrInfo_security_ForceUnAuthorizedDot1x) == 0)
    {
      mode = L7_DOT1X_PORT_FORCE_UNAUTHORIZED;
    }
    else if (strcmp(strMode, pStrInfo_security_MacBasedDot1x) == 0)
    {
        if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)==L7_TRUE) 
        {
            mode = L7_DOT1X_PORT_AUTO_MAC_BASED;
        }
        else
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlModeAll);
        }
    }
    else
    {
        /*return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode);*/
        if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)!=L7_TRUE) 
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlModeAll);
        }
        else
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode_MacBased_All);
        }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DOT1X_PORT_AUTO;
  }

  if ( usmDbIntIfNumTypeFirstGet( unit, inclIntfTypes, exclIntfTypes,
                                 &iface ) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xNoPortAvailable);
  }

  do
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
       if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
         (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))
       {
          if (allPorts == L7_TRUE &&
           usmDbIntIfNumTypeNextGet( unit, inclIntfTypes, exclIntfTypes,
                                 iface, &iface ) != L7_SUCCESS )
          {
            return cliPrompt(ewsContext);
          }
       }

      if ( usmDbDot1xPortControlModeSet(unit, iface, mode) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortCntrlMode);
      }
    }
    interfaceCount++;
    if (allPorts == L7_TRUE &&
        usmDbIntIfNumTypeNextGet( unit, inclIntfTypes, exclIntfTypes,
                                 iface, &iface ) != L7_SUCCESS )
    {
      return cliPrompt(ewsContext);
    }
  } while (allPorts == L7_TRUE);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set the timeout period for the specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x timeout {{reauth-period <seconds>} |
* {quiet-period <seconds>} | {tx-period <seconds>} |
* {supp-timeout <seconds>} | {server-timeout <seconds>}}
*
* @cmdhelp Set the quiet perioed on a per-port basis.
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xTimeout(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 * * argv,
                                    L7_uint32 index)
{
  L7_uint32 slot, port, iface, numArgs;
  L7_char8 strPeriod[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argToken = 1;
  L7_uint32 argPeriod = 2;
  L7_uint32 period;
  L7_uint32 unit;
  L7_uint32 tokenVal = 0;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* Check for valid number of arguments */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 2 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_CfgDot1xTimeout);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArgs != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_CfgDot1xNoTimeout);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Fetching Port information from ewsContext */
  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if ((usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)||
      (cliDot1xInterfaceValidate(ewsContext, unit, iface ) != L7_SUCCESS ))
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_security_Dot1xInvalidSlotPortIntf);
    return cliPrompt(ewsContext);
  }

  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))

  {
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeAuth);
            return cliPrompt(ewsContext);
 }


  /* Verify token value */
  if ( strcmp(argv[index+argToken], pStrInfo_security_Dot1xQuietPeriod) == 0 )
  {
    tokenVal = 1;
  }
  else if ( strcmp(argv[index+argToken], pStrInfo_security_Dot1xReauthPeriod) == 0 )
  {
    tokenVal = 2;
  }
  else if ( strcmp(argv[index+argToken], pStrInfo_security_Dot1xSrvrTimeout) == 0 )
  {
    tokenVal = 3;
  }
  else if ( strcmp(argv[index+argToken], pStrInfo_security_Dot1xSuppTimeout) == 0 )
  {
    tokenVal = 4;
  }
  else if ( strcmp(argv[index+argToken], pStrInfo_security_Dot1xTxPeriod) == 0 )
  {
    tokenVal = 5;
  }
  else if ( strcmp(argv[index+argToken], pStrInfo_security_Dot1xGuestPeriod) == 0 )
  {
    tokenVal = 6;
  }

  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_CfgDot1xTimeout);
  }

  /* Obtain period value or set to default */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    switch (tokenVal)
    {
    case 1:     /* Check for value */
      if (strlen(argv[index+argPeriod]) >= sizeof(strPeriod))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortQuietPeriod,
                                L7_DOT1X_PORT_MIN_QUIET_PERIOD, L7_DOT1X_PORT_MAX_QUIET_PERIOD );
      }
      osapiStrncpySafe(strPeriod, argv[index+argPeriod], sizeof(strPeriod));
      cliConvertToLowerCase(strPeriod);
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPeriod],
                                           &period) != L7_SUCCESS ||
          period < L7_DOT1X_PORT_MIN_QUIET_PERIOD ||
          period > L7_DOT1X_PORT_MAX_QUIET_PERIOD )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortQuietPeriod,
                                L7_DOT1X_PORT_MIN_QUIET_PERIOD, L7_DOT1X_PORT_MAX_QUIET_PERIOD );
      }
      break;

    case 2:     /* Check for value */
      if (strlen(argv[index+argPeriod]) >= sizeof(strPeriod))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortReAuthPeriod,
                                L7_DOT1X_PORT_MIN_REAUTH_PERIOD, L7_DOT1X_PORT_MAX_REAUTH_PERIOD );
      }
      osapiStrncpySafe(strPeriod, argv[index+argPeriod], sizeof(strPeriod));
      cliConvertToLowerCase(strPeriod);
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPeriod],
                                           &period) != L7_SUCCESS ||
          period > L7_DOT1X_PORT_MAX_REAUTH_PERIOD ||
          period < L7_DOT1X_PORT_MIN_REAUTH_PERIOD )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortReAuthPeriod,
                                L7_DOT1X_PORT_MIN_REAUTH_PERIOD, L7_DOT1X_PORT_MAX_REAUTH_PERIOD );
      }
      break;

    case 3:     /* Check for value */
      if (strlen(argv[index+argPeriod]) >= sizeof(strPeriod))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortSrvrTimeout,
                                L7_DOT1X_PORT_MIN_SERVER_TIMEOUT, L7_DOT1X_PORT_MAX_SERVER_TIMEOUT );
      }
      osapiStrncpySafe(strPeriod, argv[index+argPeriod], sizeof(strPeriod));
      cliConvertToLowerCase(strPeriod);
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPeriod],
                                           &period) != L7_SUCCESS ||
          period > L7_DOT1X_PORT_MAX_SERVER_TIMEOUT ||
          period < L7_DOT1X_PORT_MIN_SERVER_TIMEOUT )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortSrvrTimeout,
                                L7_DOT1X_PORT_MIN_SERVER_TIMEOUT, L7_DOT1X_PORT_MAX_SERVER_TIMEOUT );
      }
      break;

    case 4:     /* Check for value */
      if (strlen(argv[index+argPeriod]) >= sizeof(strPeriod))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortSupplicantTimeout,
                                L7_DOT1X_PORT_MIN_SUPP_TIMEOUT, L7_DOT1X_PORT_MAX_SUPP_TIMEOUT );
      }
      osapiStrncpySafe(strPeriod, argv[index+argPeriod], sizeof(strPeriod));
      cliConvertToLowerCase(strPeriod);
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPeriod],
                                           &period) != L7_SUCCESS ||
          period > L7_DOT1X_PORT_MAX_SUPP_TIMEOUT ||
          period < L7_DOT1X_PORT_MIN_SUPP_TIMEOUT )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortSupplicantTimeout,
                                L7_DOT1X_PORT_MIN_SUPP_TIMEOUT, L7_DOT1X_PORT_MAX_SUPP_TIMEOUT );
      }
      break;

    case 5:     /* Check for value */
      if (strlen(argv[index+argPeriod]) >= sizeof(strPeriod))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortTxPeriod,
                                L7_DOT1X_PORT_MIN_TX_PERIOD, L7_DOT1X_PORT_MAX_TX_PERIOD );
      }
      osapiStrncpySafe(strPeriod, argv[index+argPeriod], sizeof(strPeriod));
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPeriod],
                                           &period) != L7_SUCCESS ||
          period > L7_DOT1X_PORT_MAX_TX_PERIOD ||
          period < L7_DOT1X_PORT_MIN_TX_PERIOD )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortTxPeriod,
                                L7_DOT1X_PORT_MIN_TX_PERIOD, L7_DOT1X_PORT_MAX_TX_PERIOD );
      }
      break;
    case 6:    /*Check for value*/
      if (strlen(argv[index+argPeriod]) >= sizeof(strPeriod))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortGuestVlanPeriod,
                                L7_DOT1X_PORT_MIN_GUESTVLAN_PERIOD, L7_DOT1X_PORT_MAX_GUESTVLAN_PERIOD );
      }
      osapiStrncpySafe(strPeriod, argv[index+argPeriod], sizeof(strPeriod));
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPeriod],
                                           &period) != L7_SUCCESS ||
          period > L7_DOT1X_PORT_MAX_GUESTVLAN_PERIOD ||
          period < L7_DOT1X_PORT_MIN_GUESTVLAN_PERIOD )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortGuestVlanPeriod,
                                L7_DOT1X_PORT_MIN_GUESTVLAN_PERIOD, L7_DOT1X_PORT_MAX_GUESTVLAN_PERIOD );
      }
      break;

    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    switch (tokenVal)
    {
    case 1: period = FD_DOT1X_PORT_QUIET_PERIOD;
      break;

    case 2: period = FD_DOT1X_PORT_REAUTH_PERIOD;
      break;

    case 3: period = FD_DOT1X_PORT_SERVER_TIMEOUT;
      break;

    case 4: period = FD_DOT1X_PORT_SUPP_TIMEOUT;
      break;

    case 5: period = FD_DOT1X_PORT_TX_PERIOD;
      break;

    case 6:  period = FD_DOT1X_PORT_GUEST_PERIOD;
      break;
    }
  }

  switch (tokenVal)
  {
  case 1:     /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xPortQuietPeriodSet(unit, iface, period) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortQuietPeriod);
      }
    }
    break;

  case 2:    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xPortReAuthPeriodSet(unit, iface, period) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortReAuthPeriod);
      }
    }
    break;

  case 3:    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xPortServerTimeoutSet(unit, iface, period) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortReAuthSrvrTimeout);
      }
    }
    break;

  case 4:    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xPortSuppTimeoutSet(unit, iface, period) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortReAuthSuppTimeout);
      }
    }
    break;

  case 5:    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xPortTxPeriodSet(unit, iface, period) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortTxPeriod);
      }
    }
    break;

  case 6:    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xAdvancedPortGuestVlanPeriodSet(unit, iface, period) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortGuestVlanPeriod);
      }
    }
    break;

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set the maximum requests for the specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x max-req  <count>
*
* @cmdhelp Set the maximum requests on a per-port basis.
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xMaxRequests(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 slot, port, iface;

  L7_uint32 argValue = 1;
  L7_uint32 value;
  L7_uint32 unit;
  L7_uint32 numArgs;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortMaxReqsSyntax);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArgs != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortMaxReqsNoSyntax);
    }
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }
  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                pStrErr_security_Dot1xPortMaxReqs);
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeAuth);
     return cliPrompt(ewsContext);
 }


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                         &value) != L7_SUCCESS ||
        value > L7_DOT1X_PORT_MAX_MAX_REQ ||
        value < L7_DOT1X_PORT_MIN_MAX_REQ )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortMaxReqs,
                              L7_DOT1X_PORT_MIN_MAX_REQ, L7_DOT1X_PORT_MAX_MAX_REQ );
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    value = FD_DOT1X_PORT_MAX_REQ;
  }
  
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xPortMaxReqSet(unit, iface, value) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortMaxReqs);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Specify whether reauthentication is enabled on this port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] dot1x re-authentication
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xReauthentication(EwsContext ewsContext,
                                             L7_uint32 argc,
                                             const L7_char8 * * argv,
                                             L7_uint32 index)
{
  L7_uint32 slot, port, iface;
  L7_BOOL mode = L7_FALSE;
  L7_uint32 unit;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if ( cliNumFunctionArgsGet() != 0 )
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {

      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortReAuthEnbld);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortReAuthEnbldNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }
  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                pStrErr_security_Dot1xPortReAuthEnbl);
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeAuth);
      return cliPrompt(ewsContext);
 }


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xPortReAuthEnabledSet(unit, iface, mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortReAuthEnbl);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Config User Manager General Information
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
* @cmdsyntax  dot1x defaultlogin <listname>
*
*
* @cmdhelp
*
* @cmddescript  Displays general status information for User Management.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDot1xDefaultLogin(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index)
{
  L7_uint32 unit;
  L7_uint32 i;
  L7_char8 strAuthListName[L7_MAX_APL_NAME_SIZE+1];

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_security_CfgDot1xDeflLogin);
  }

  memset (strAuthListName, 0,sizeof(strAuthListName));
  if (strlen(argv[index+1]) > L7_MAX_APL_NAME_SIZE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);

  }
  osapiStrncpySafe(strAuthListName, argv[index+1], sizeof(strAuthListName));

  if (usmDbAuthenticationListIndexGetByName(ACCESS_LINE_DOT1X, 0, strAuthListName, &i) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbAPLNonConfiguredUserSet(unit, L7_DOT1X_COMPONENT_ID, strAuthListName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_security_AssigningAuthLoginListToNonCfguredUsrForPortSecurity);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Config User Manager General Information
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
* @cmdsyntax  dot1x login <user> <listname>
*
*
* @cmdhelp
*
* @cmddescript  Displays general status information for User Management.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDot1xLogin(EwsContext ewsContext,
                                  uintf argc,
                                  const L7_char8 * * argv,
                                  uintf index)
{
  L7_uint32 unit;
  L7_uint32 i;
  L7_char8 strAuthListName[L7_MAX_APL_NAME_SIZE+1];
  L7_char8 strUserName[L7_LOGIN_SIZE];

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliNumFunctionArgsGet() != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_security_CfgDot1xLogin);
  }

  osapiStrncpySafe(strUserName, argv[index+1], sizeof(strUserName));

  if (usmDbUserLoginIndexGet(unit, strUserName, &i) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_UsrName);
  }

  memset (strAuthListName, 0,sizeof(strAuthListName));
  if (strlen(argv[index+2]) > L7_MAX_APL_NAME_SIZE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
  }
  osapiStrncpySafe(strAuthListName, argv[index+2], sizeof(strAuthListName));
  if (usmDbAuthenticationListIndexGetByName(ACCESS_LINE_DOT1X, 0, strAuthListName, &i) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbAPLUserSet(unit, strUserName, L7_DOT1X_COMPONENT_ID, strAuthListName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_security_AssigningAuthLoginListToUsrForPortSecurity);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   configure dot1x users for a specific port
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
* @cmdsyntax  [no] dot1x user <user> <slot/port | all>
*
*
* @cmdhelp
*
* @cmddescript  Adds user access to specified dot1x port.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDot1xUser(EwsContext ewsContext,
                                 uintf argc,
                                 const L7_char8 * * argv,
                                 uintf index)
{

  L7_uint32 unit;
  L7_uint32 i;
  L7_uint32 slot, port, iface;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strUserName[L7_LOGIN_SIZE];
  L7_char8 suppUserName[L7_LOGIN_SIZE];
  L7_BOOL allPorts = L7_FALSE;
  L7_BOOL mode = L7_FALSE;
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF;
  L7_uint32 interfaceCount = 0;
  L7_uint32 exclIntfTypes = 0;
  L7_uchar8 cap;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliNumFunctionArgsGet() != 2 )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortUsrsAdd, cliSyntaxInterfaceHelp());
  }

  osapiStrncpySafe(strUserName, argv[index+1], sizeof(strUserName));
  if (usmDbUserLoginIndexGet(unit, strUserName, &i) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_UsrName);
  }

  osapiStrncpySafe( strSlotPort, argv[index+2], sizeof(strSlotPort));
  if (strlen(argv[index+2]) >= sizeof(strSlotPort))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortCntrlDir, cliSyntaxInterfaceHelp());
  }

  cliConvertToLowerCase(strSlotPort);
  if (strcmp(strSlotPort, pStrInfo_common_All) == 0 )  /* "all" */
  {
    allPorts = L7_TRUE;
  }
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if ((rc = cliValidSpecificUSPCheck(strSlotPort, &unit, &slot, &port)) != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
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
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS ||
          cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
      }
    }
    else
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, strSlotPort, &slot, &port, &iface) != L7_SUCCESS ||
          cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
  }

  if (allPorts == L7_TRUE)
  {
    /* interface = get first interface, if fail, exit */
    if ( usmDbIntIfNumTypeFirstGet( unit, inclIntfTypes, exclIntfTypes,
                                   &iface ) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xNoPortAvailable);
    }
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;
  }

  do
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbPortUserAccessSet(unit, iface, (L7_uchar8 *)strUserName, mode ) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_security_GivingUsrAccessTo8021xPort);
      }

      cap = 0x00;
      if ( usmDbDot1xPortPaeCapabilitiesGet(unit, iface, &cap) == L7_SUCCESS )
      {
        if ( cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
        {
          if ( mode == L7_TRUE)
          {
            usmDbDot1xSupplicantPortUserNameSet(iface,strUserName, strlen(strUserName));
          }
          else if ( mode == L7_FALSE)
          {
           if (usmDbDot1xSupplicantPortUserNameGet (iface,suppUserName) == L7_SUCCESS)
           {
             if (strncmp (suppUserName, strUserName, strlen(strUserName)) == 0)
             {
               usmDbDot1xSupplicantPortUserNameSet(iface,strUserName,0);
             }
           }
          }
        }
      }

    }

    interfaceCount++;

    if (allPorts == L7_TRUE &&
        usmDbIntIfNumTypeNextGet( unit, inclIntfTypes, exclIntfTypes,
                                 iface, &iface ) != L7_SUCCESS )
    {
      return cliPrompt(ewsContext);
    }
  } while (allPorts == L7_TRUE);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enable/Disable dot1x Vlan Assignment Support on the switch
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] authorization network radius
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xRadiusVlanAssignment(EwsContext ewsContext,
                                                 L7_uint32 argc,
                                                 const L7_char8 * * argv,
                                                 L7_uint32 index)
{
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliNumFunctionArgsGet() != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xVlanAsSign);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xVlanAssignmentModeSet(unit, mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_security_Dot1xVlanAsSign);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  Set the Guest Vlan Id for the specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x guest-vlan  <vlan-id>
*
* @cmdhelp Set the guest vlan id on a per-port basis.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDot1xGuestVlanId(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 slot, port, iface;

  L7_uint32 argValue = 1;
  L7_uint32 value;
  L7_uint32 unit;
  L7_uint32 numArgs;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortGuestVlanSyntax);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArgs != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xGuestVlanNoSyntax);
    }
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }
  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                               pStrErr_security_Dot1xPortGuestVlan );
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeAuth);
     return cliPrompt(ewsContext);
  }


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                         &value) != L7_SUCCESS ||
        value < L7_DOT1Q_MIN_VLAN_ID||
        value > L7_DOT1Q_MAX_VLAN_ID)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortGuestVlan,
                              L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    }
 
    /* Check if Vlan is Valid */
    if((usmDbVlanIsValid(unit, value) != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_Dot1xPortGuestVlan);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    value = FD_DOT1X_PORT_GUEST_VLAN;

  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xAdvancedGuestPortsCfgSet(unit, iface,value)!= L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortGuestVlan);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*************************************************************************
* @purpose      Configure a limit to the amount of supplicants on a port
*
* @param        EwsContext ewsContext
* @param        L7_uint32 argc
* @param        const L7_char8 **argv
* @param        L7_uint32 index
*
* @returntype   const L7_char8  *
* @returns      cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    [no] dot1x max-user {user-number}
*
* @cmdview      interface ethernet port view
*
* @cmdhelp
*
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8  *commandDot1xMaxUserInterface(EwsContext ewsContext, L7_uint32 argc,
                                              const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 slot, port, iface;

  L7_uint32 argValue = 1;
  L7_uint32 value;
  L7_uint32 unit;
  L7_uint32 numArgs;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xMaxUser);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArgs != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgNoDot1xMaxUser);
    }
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }
  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                pStrErr_security_Dot1xPortMaxUsers);
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeAuth);
    return cliPrompt(ewsContext);
 }


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                         &value) != L7_SUCCESS ||
        value < L7_DOT1X_PORT_MIN_MAC_USERS ||
        value > L7_DOT1X_PORT_MAX_MAC_USERS )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortMaxUsers,
                              L7_DOT1X_PORT_MIN_MAC_USERS, L7_DOT1X_PORT_MAX_MAC_USERS );
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    value = FD_DOT1X_PORT_MAX_USERS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xPortMaxUsersSet(unit, iface, value) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortMaxUsers);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}


/*********************************************************************
*
* @purpose  Set the Unauthenticated Vlan Id for the specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x unauthenticated-vlan  <vlan-id>
*
* @cmdhelp Set the guest vlan id on a per-port basis.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDot1xUnatuthenticatedVlanId(EwsContext ewsContext,
                                                   L7_uint32 argc,
                                                   const L7_char8 * * argv,
                                                   L7_uint32 index)
{
  L7_uint32 slot, port, iface;

  L7_uint32 argValue = 1;
  L7_uint32 value;
  L7_uint32 unit;
  L7_uint32 numArgs;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortUnauthenticatedVlanSyntax);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArgs != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xUnauthenticatedVlanNoSyntax);
    }
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }
  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_SUPP_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                pStrErr_security_Dot1xPortUnauthenticatedVlan);
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeAuth);
     return cliPrompt(ewsContext);
 }


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                         &value) != L7_SUCCESS ||
        value < L7_DOT1Q_MIN_VLAN_ID||
        value > L7_DOT1Q_MAX_VLAN_ID)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortUnauthenticatedVlan,
                              L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    value = FD_DOT1X_PORT_GUEST_VLAN;

  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xPortUnauthenticatedVlanSet(unit, iface,value)!= L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortUnauthenticatedVlan);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/* Supplicant Cli commands */
/*********************************************************************
*
* @purpose  Set the dot1x pae capability for specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  dot1x pae <supplicant|authenticator>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xPaeCapability (EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  L7_uint32 iface;
  L7_uint32 numArgs;
  L7_uint32 argMode = 1;
  L7_char8  strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit,slot,port;
  L7_uchar8 paeCapability=0;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 1 )
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortPaeCapability);
    }
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argMode]) >= sizeof(strMode))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortPaeCapability);
      
    }
    osapiStrncpySafe(strMode,argv[index + argMode], sizeof(strMode));
    cliConvertToLowerCase(strMode);

    if (strcmp(strMode, pStrInfo_security_PaeSupplicant) == 0)
    {
      paeCapability = L7_DOT1X_PAE_PORT_SUPP_CAPABLE;
    }
    else if (strcmp(strMode, pStrInfo_security_PaeAuthenticator) == 0)
    {
      paeCapability = L7_DOT1X_PAE_PORT_AUTH_CAPABLE;
    }
    else
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortPaeCapability);
    }
  }
  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  do
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xPortPaeCapabilitiesSet(iface, paeCapability) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortPaeCapability);
      }
    }
  } while (0);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set the dot1x control mode on a specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x supplicant port-control <auto | force-authorized | force-unauthorized >
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDot1xSupplicantPortControl(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 slot, port, iface;
  L7_uint32 numArgs;
  L7_uint32 argMode = 1;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 mode = L7_DOT1X_PORT_AUTO;
  L7_uint32 unit;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xSupplicantPortCntrlMode);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArgs != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xNoSupplicantPortCntrlMode);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_AUTH_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                pStrErr_security_Dot1xSupplicantPortCntrlMode);
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeSupp);
  return cliPrompt(ewsContext);
 }


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argMode]) >= sizeof(strMode))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xSupplicantPortCntrlMode);
    }
    osapiStrncpySafe(strMode,argv[index + argMode], sizeof(strMode));
    cliConvertToLowerCase(strMode);

    if (strcmp(strMode, pStrInfo_common_Auto2) == 0) /* auto */
    {
      mode = L7_DOT1X_PORT_AUTO;
    }
    else if (strcmp(strMode, pStrInfo_security_ForceAuthorizedDot1x) == 0)
    {
      mode = L7_DOT1X_PORT_FORCE_AUTHORIZED;
    }
    else if (strcmp(strMode, pStrInfo_security_ForceUnAuthorizedDot1x) == 0)
    {
      mode = L7_DOT1X_PORT_FORCE_UNAUTHORIZED;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortCntrlMode);
  
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DOT1X_PORT_AUTO;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xSupplicantPortControlModeSet(iface, mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xSupplicantPortCntrlMode);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set the maximum starts for the specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x max-start  <count>
*
* @cmdhelp Set the maximum start EAPOL messages to be sent on a
*          per-port basis consequently in the absence of Authenticator.
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xMaxStarts(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{

  L7_uint32 slot, port, iface;

  L7_uint32 argValue = 1;
  L7_uint32 value = 0;
  L7_uint32 unit;
  L7_uint32 numArgs;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortMaxStartsSyntax);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArgs != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgDot1xPortMaxStartsNoSyntax);
    }
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_AUTH_CAPABLE))

  {
      /* Display generic failure.*/
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                pStrErr_security_Dot1xPortMaxStarts);
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeSupp);
  return cliPrompt(ewsContext);
 }


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                         &value) != L7_SUCCESS ||
        value > L7_DOT1X_PORT_MAX_MAX_START ||
        value < L7_DOT1X_PORT_MIN_MAX_START )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortMaxStarts,
                              L7_DOT1X_PORT_MIN_MAX_START, L7_DOT1X_PORT_MAX_MAX_START );
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    value = FD_DOT1X_SUPPLICANT_PORT_MAX_START;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbDot1xSupplicantPortMaxStartSet(iface, value) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortMaxStarts);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose  Set the timeout period for the specified port
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] dot1x supplicant timeout {{start-period <seconds>} |
* {held-period <seconds>} | {auth-period <seconds>}}
*
* @cmdhelp Set the start , held and Auth perioeds on a per-port basis.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDot1xSupplicantTimeout(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 * * argv,
                                    L7_uint32 index)
{
  L7_uint32 slot, port, iface, numArgs;
  L7_char8 strPeriod[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argToken = 1;
  L7_uint32 argPeriod = 2;
  L7_uint32 period;
  L7_uint32 unit;
  L7_uint32 tokenVal = 0;
  L7_uchar8 paeCap;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* Check for valid number of arguments */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArgs != 2 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_CfgDot1xSupplicantTimeout);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArgs != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_CfgDot1xSupplicantNoTimeout);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Fetching Port information from ewsContext */
  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if ((usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)||
      (cliDot1xInterfaceValidate(ewsContext, unit, iface ) != L7_SUCCESS ))
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_security_Dot1xInvalidSlotPortIntf);
    return cliPrompt(ewsContext);
  }

  /* Verify token value */
  if ( strcmp(argv[index+argToken], pStrInfo_security_Dot1xStartPeriod) == 0 )
  {
    tokenVal = 1;
  }
  else if ( strcmp(argv[index+argToken], pStrInfo_security_Dot1xHeldPeriod) == 0 )
  {
    tokenVal = 2;
  }
  else if ( strcmp(argv[index+argToken], pStrInfo_security_Dot1xAuthPeriod) == 0 )
  {
    tokenVal = 3;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_CfgDot1xSupplicantTimeout);
  }

  if ( (usmDbDot1xPortPaeCapabilitiesGet (unit,iface,&paeCap) == L7_SUCCESS) &&
     (paeCap == L7_DOT1X_PAE_PORT_AUTH_CAPABLE))

  {
      /* Should be Authenticator */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                pStrErr_security_Dot1xPortPaeSupp);
       return cliPrompt(ewsContext);
 }


  /* Obtain period value or set to default */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    switch (tokenVal)
    {
    case 1:     /* Check for value */
      if (strlen(argv[index+argPeriod]) >= sizeof(strPeriod))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortStartPeriod,
                                L7_DOT1X_PORT_MIN_START_PERIOD, L7_DOT1X_PORT_MAX_START_PERIOD );
      }
      osapiStrncpySafe(strPeriod, argv[index+argPeriod], sizeof(strPeriod));
      cliConvertToLowerCase(strPeriod);
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPeriod],
                                           &period) != L7_SUCCESS ||
          period < L7_DOT1X_PORT_MIN_START_PERIOD ||
          period > L7_DOT1X_PORT_MAX_START_PERIOD )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortQuietPeriod,
                                L7_DOT1X_PORT_MIN_QUIET_PERIOD, L7_DOT1X_PORT_MAX_QUIET_PERIOD );
      }
      break;

    case 2:     /* Check for value */
      if (strlen(argv[index+argPeriod]) >= sizeof(strPeriod))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortHeldPeriod,
                                L7_DOT1X_PORT_MIN_HELD_PERIOD, L7_DOT1X_PORT_MAX_HELD_PERIOD );
      }
      osapiStrncpySafe(strPeriod, argv[index+argPeriod], sizeof(strPeriod));
      cliConvertToLowerCase(strPeriod);
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPeriod],
                                           &period) != L7_SUCCESS ||
          period > L7_DOT1X_PORT_MAX_HELD_PERIOD ||
          period < L7_DOT1X_PORT_MIN_HELD_PERIOD )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortHeldPeriod,
                                L7_DOT1X_PORT_MIN_HELD_PERIOD, L7_DOT1X_PORT_MAX_HELD_PERIOD );
      }
      break;

    case 3:     /* Check for value */
      if (strlen(argv[index+argPeriod]) >= sizeof(strPeriod))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortAuthPeriod,
                                L7_DOT1X_PORT_MIN_AUTH_PERIOD, L7_DOT1X_PORT_MAX_AUTH_PERIOD);
      }
      osapiStrncpySafe(strPeriod, argv[index+argPeriod], sizeof(strPeriod));
      if (cliConvertTo32BitUnsignedInteger(argv[index+argPeriod],
                                           &period) != L7_SUCCESS ||
          period > L7_DOT1X_PORT_MAX_AUTH_PERIOD ||
          period < L7_DOT1X_PORT_MIN_AUTH_PERIOD )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xPortAuthPeriod,
                                L7_DOT1X_PORT_MIN_AUTH_PERIOD, L7_DOT1X_PORT_MAX_AUTH_PERIOD );
      }
      break;

    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    switch (tokenVal)
    {
    case 1: period = FD_DOT1X_SUPPLICANT_PORT_START_PERIOD;
      break;

    case 2: period = FD_DOT1X_SUPPLICANT_PORT_HELD_PERIOD;
      break;

    case 3: period = FD_DOT1X_SUPPLICANT_PORT_AUTH_PERIOD;
      break;
    }
  }
  else 
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                       pStrErr_security_CfgDot1xSupplicantTimeout);
  }

  switch (tokenVal)
  {
  case 1:     /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xSupplicantPortStartPeriodSet(iface, period) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortStartPeriod);
      }
    }
    break;
  case 2:    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xSupplicantPortHeldPeriodSet(iface, period) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortHeldPeriod);
      }
    }
    break;

  case 3:    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDot1xSupplicantPortAuthPeriodSet(iface, period) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xPortAuthPeriod);
      }
    }
    break;
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   configure dot1x Supplicant user for a specific port
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
* @cmdsyntax  [no] dot1x supplicant user <user> 
*
*
* @cmdhelp
*
*
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xSupplicantUser(EwsContext ewsContext,
                                 uintf argc,
                                 const L7_char8 * * argv,
                                 uintf index)
{

  L7_uint32 unit;
  L7_uint32 i;
  L7_uint32 slot, port, iface;
  L7_char8 strUserName[L7_LOGIN_SIZE];
  L7_BOOL mode = L7_FALSE;
  L7_uchar8 cap;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliNumFunctionArgsGet() != 1 )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_CfgDot1xSupplicantPortUsrAdd);
  }

  osapiStrncpySafe(strUserName, argv[index+1], sizeof(strUserName));
  if (usmDbUserLoginIndexGet(unit, strUserName, &i) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_UsrName);
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  /* Get interface and check its validity */
  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS) 
  {
     ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
     return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;
  }

  do
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      cap = 0x00;
      if ( usmDbDot1xPortPaeCapabilitiesGet(unit, iface, &cap) == L7_SUCCESS )
      {
        if ( cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
        {
          if ( mode == L7_TRUE)
          {
            usmDbDot1xSupplicantPortUserNameSet(iface,strUserName, strlen(strUserName));
          }
          else if ( mode == L7_FALSE)
          {
           usmDbDot1xSupplicantPortUserNameSet(iface,strUserName,0);
          }
        }
      }

    }

  } while (0);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}




