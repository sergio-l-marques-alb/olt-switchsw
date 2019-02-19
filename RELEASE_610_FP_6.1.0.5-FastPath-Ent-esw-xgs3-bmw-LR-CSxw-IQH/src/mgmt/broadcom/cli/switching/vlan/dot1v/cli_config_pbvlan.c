/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/vlan/dot1v/cli_config_pbvlan.c
 *
 * @purpose config commands for protocol based vlan
 *
 * @component CLI
 *
 * @comments none
 *
 * @create  03/05/2007
 *
 * @author  Amit Kulkarni
 *
 * @end
 *
 **********************************************************************/



#include "datatypes.h"
#include "commdefs.h"



#include "ews.h"
#include "cliapi.h"
#include "clicommands_card.h"
#include "usmdb_pbvlan_api.h"
#include "cli_web_exports.h"
#include "filter_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"



/*********************************************************************
 *
 * @purpose  Creates or Deletes a new group.
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
 * @notes none
 *
 * @cmdsyntax  [no] vlan protocol group <groupname>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandVlanProtocolGroup(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argGroupName = 1;
  L7_char8 strGroupName[L7_CLI_MAX_STRING_LENGTH];   /* This is 1 + L7_PBVLAN_MAX_GROUP_NAME */
  L7_uint32 unit, rc;

  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 numArg, groupID;
  L7_uint32 nextGroupID;
  L7_BOOL found = L7_FALSE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /*Macro to check that the command supports Normal and No modes*/
  if (numArg != 1)
  {

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrp_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_NoVlanProtoGrp);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (strlen(argv[index+argGroupName]) >= sizeof(strGroupName))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrp_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_NoVlanProtoGrp);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  OSAPI_STRNCPY_SAFE(strGroupName, argv[index + argGroupName]);

  if (cliIsAlphaNum(strGroupName) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_VlanProtoGrpType);
  }

  /*******Check if the Flag is Set for Execution*************/

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc =  usmDbPbVlanGroupCreate(unit, strGroupName);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpExists);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_VlanGrpNotCreated);
        }

        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    groupID = 0;

    rc = usmDbPbVlanGroupGetNext(unit, groupID, &nextGroupID);
    groupID = nextGroupID;

    if (rc == L7_SUCCESS)
    {
      do
      {
        memset(buf, 0, sizeof(buf));
        rc = usmDbPbVlanGroupNameGet(unit, groupID, buf);

        if (rc == L7_SUCCESS && strncmp(buf, strGroupName, sizeof(strGroupName)) == 0)
        {
          found = L7_TRUE;
          break;
        }
        else
        {
          groupID = nextGroupID;
        }

      } while (usmDbPbVlanGroupGetNext(unit, groupID, &nextGroupID) == L7_SUCCESS);

      if (found)
      {
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbPbVlanGroupDelete(unit, groupID) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_base_VlanGrpNotDeld);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpDoesntExist);
      }
    }

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Deletes a specified group.
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
 * @notes none
 *
 * @cmdsyntax  vlan protocol group remove <groupid>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandVlanProtocolGroupRemove(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argGroupId = 1;
  L7_uint32 groupId;
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
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
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpRemove_1);
  }

  if (strlen(argv[index+argGroupId]) >= sizeof(strGroupId))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpRemove_1);
  }

  OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

  if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
  }

  if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbPbVlanGroupDelete(unit, groupId) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_VlanGrpNotDeld);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Adds / Removes a protocol to a group.
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
 * @notes none
 *
 * @cmdsyntax  vlan protocol group add  <groupid> <protocol>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandVlanProtocolGroupAdd(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argGroupId = 2;
  L7_uint32 argProtocol = 3;
  L7_uint32 groupId, protocol, i;
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strProtocol[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 protocolListPrev[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_BOOL found = L7_FALSE;
  L7_RC_t rc;
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

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 3)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_1);
    }
    if ((strlen(argv[index+argGroupId]) >= sizeof(strGroupId)) ||
        (strlen(argv[index+argProtocol]) >= sizeof(strProtocol)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_1);
    }

    OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

    if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    OSAPI_STRNCPY_SAFE(strProtocol,argv[index + argProtocol]);
    cliConvertToLowerCase(strProtocol);

    memset(protocolListPrev, 0, sizeof(protocolListPrev));
    /* Get the list of Protocols currently configured with the Group */
    rc = usmDbPbVlanGroupProtocolGet(unit, groupId, protocolListPrev);

    if (strcmp(strProtocol, pStrInfo_common_IpOption) == 0)
    {
      protocol = L7_USMDB_PROTOCOL_IP;
    }
    else if (strcmp(strProtocol, pStrInfo_common_Arp_2) == 0)
    {
      protocol = L7_USMDB_PROTOCOL_ARP;
    }
    else if (strcmp(strProtocol, pStrInfo_base_Ipx_1) == 0)
    {
      protocol = L7_USMDB_PROTOCOL_IPX;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_1);
    }

    found = L7_FALSE;
    for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
    {
      if (protocol == protocolListPrev[i])
      {
        found = L7_TRUE;
        break;
      }
    }
    if (found == L7_FALSE)
    {

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbPbVlanGroupProtocolAdd(unit, groupId, protocol);

        if (rc == L7_FAILURE )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_VlanGrpAdd);
        }
        else if (rc == L7_ALREADY_CONFIGURED)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_GrpCfgured);
        }
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_ProtoCfgured);
    }

    cliSyntaxBottom(ewsContext);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 3)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAddNo);
    }
    if (strlen(argv[index+argGroupId]) >= sizeof(strGroupId))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAddNo);
    }

    OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

    if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    OSAPI_STRNCPY_SAFE(strProtocol,argv[index + argProtocol]);
    cliConvertToLowerCase(strProtocol);

    if (strcmp(strProtocol, pStrInfo_common_IpOption) == 0)
    {
      protocol = L7_USMDB_PROTOCOL_IP;
    }
    else if (strcmp(strProtocol, pStrInfo_common_Arp_2) == 0)
    {
      protocol = L7_USMDB_PROTOCOL_ARP;
    }
    else if (strcmp(strProtocol, pStrInfo_base_Ipx_1) == 0)
    {
      protocol = L7_USMDB_PROTOCOL_IPX;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAddNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbPbVlanGroupProtocolDelete(unit, groupId, protocol) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_ProtoNotRemoved);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Adds / Removes a vlan id to a group.
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
 * @notes none
 *
 * @cmdsyntax  protocol group <groupid> <vlanid>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandProtocolGroupAdd(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argGroupId = 1;
  L7_uint32 argVlan = 2;
  L7_uint32 groupId, vlan;
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strVlan[L7_CLI_MAX_STRING_LENGTH];
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

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoGrpAdd);
    }
    if ((strlen(argv[index+argGroupId]) >= sizeof(strGroupId)) ||
        (strlen(argv[index+argVlan]) >= sizeof(strVlan)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoGrpAdd);
    }

    OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

    if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    OSAPI_STRNCPY_SAFE(strVlan,argv[index + argVlan]);

    if (cliConvertTo32BitUnsignedInteger(strVlan, &vlan)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbPbVlanGroupVlanIDAdd(unit, groupId, vlan) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotVlanAdded);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoGrpAddNo);
    }
    if ((strlen(argv[index+argGroupId]) >= sizeof(strGroupId)) ||
        (strlen(argv[index+argVlan]) >= sizeof(strVlan)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoGrpAddNo);
    }

    OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

    if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    OSAPI_STRNCPY_SAFE(strVlan,argv[index + argVlan]);

    if (cliConvertTo32BitUnsignedInteger(strVlan, &vlan)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbPbVlanGroupVlanIDDelete(unit, groupId, vlan) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotVlanRemoved);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Adds / Removes an interface to a group.
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
 * @notes none
 *
 * @cmdsyntax protocol vlan group  <groupid>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandProtocolVlanGroupAdd(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 argGroupId = 1;

  L7_uint32 groupId;
  L7_uint32 iface, s, p;
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL   found = L7_FALSE;
  L7_uint32 intfListPrev[L7_FILTER_MAX_INTF];
  L7_uint32 intIfNum, numIntfPrev, i;
  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  s = EWSSLOT(ewsContext);
  p =EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoVlanGrpAdd);
    }

    if (strlen(argv[index+argGroupId]) >= sizeof(strGroupId))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoVlanGrpAdd);
    }
    memset(intfListPrev, 0, sizeof(intfListPrev));
    OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

    if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    /* Get the list of interfaces already configured with the group */
    intIfNum = 0;
    numIntfPrev = 0;
    while (usmDbPbVlanGroupPortGetNext(unit, groupId, intIfNum, &intIfNum) == L7_SUCCESS)
    {
      intfListPrev[numIntfPrev++] = intIfNum;
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbPbVlanGroupPortAdd(unit, groupId, iface);
      if (rc == L7_FAILURE )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_AddIntfToGrp);
      }
    }
    else if (rc == L7_ALREADY_CONFIGURED)
    {
      found = L7_FALSE;
      for (i = 0; i < numIntfPrev; i++)
      {
        if (iface == intfListPrev[i])
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_IntfAlreadyCfguredWithGrp); /*Interface already configured with this group.*/
          cliSyntaxBottom(ewsContext);
          found = L7_TRUE;
          break;
        }
      }
      if (found == L7_FALSE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_IntfAlreadyCfguredWithSameProto); /*Interface already configured with another group using the same protocol.*/
        cliSyntaxBottom(ewsContext);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoVlanGrpAddNo);
    }

    if (strlen(argv[index+argGroupId]) >= sizeof(strGroupId))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoVlanGrpAddNo);
    }

    OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

    if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbPbVlanGroupPortDelete(unit, groupId, iface) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotIntfRemoved);
      }
    }

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Adds all interfaces to a group.
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
 * @notes none
 *
 * @cmdsyntax protocol vlan group all <groupid>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandProtocolVlanGroupAddAll(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 argGroupId = 1;

  L7_uint32 groupId;
  L7_uint32 iface;
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL   found = L7_FALSE;
  L7_BOOL   alreadyConf = L7_FALSE;
  L7_uint32 intfListPrev[L7_FILTER_MAX_INTF];
  L7_uint32 intIfNum, numIntfPrev, i;
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

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoVlanGrpAddAll);
    }

    if (strlen(argv[index+argGroupId]) >= sizeof(strGroupId))

    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoVlanGrpAddAll);
    }

    memset(intfListPrev, 0, sizeof(intfListPrev));
    OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

    if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    /* Get the list of interfaces already configured with the group */
    intIfNum = 0;
    numIntfPrev = 0;
    while (usmDbPbVlanGroupPortGetNext(unit, groupId, intIfNum, &intIfNum) == L7_SUCCESS)
    {
      intfListPrev[numIntfPrev++] = intIfNum;
    }

    rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF, 0, &iface);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
    }

    found = L7_TRUE;
    while (rc == L7_SUCCESS)
    {

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbPbVlanGroupPortAdd(unit, groupId, iface);
        if ((rc == L7_ALREADY_CONFIGURED) && (found))
        {
          for (i = 0; i <= numIntfPrev; i++)
          {
            if (iface == intfListPrev[i])
            {
              alreadyConf = L7_TRUE;
              break;
            }
            found = L7_FALSE;
          }
        }
      }
      rc = usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF, 0, iface, &iface);
    }
    if (alreadyConf == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_GrpAlreadyCfgured); /*One or more interfaces already configured with this group.*/
    }
    else if (found == L7_FALSE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SameProtoAlreadyCfgured); /*One or more interfaces already configured with another group with same protocol.*/
    }

    cliSyntaxBottom(ewsContext);

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoVlanGrpAddAllNo);
    }

    if (strlen(argv[index+argGroupId]) >= sizeof(strGroupId))

    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ProtoVlanGrpAddAllNo);
    }

    OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

    if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }

    rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF, 0, &iface);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      while (rc == L7_SUCCESS)
      {
        rc = usmDbPbVlanGroupPortDelete(unit, groupId, iface);
        rc = usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF, 0, iface, &iface);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

