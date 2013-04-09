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


#include "cliapi.h"
#include "datatypes.h"
#include "commdefs.h"



#include "ews.h"
#include "clicommands_card.h"
#include "usmdb_pbvlan_api.h"
#include "cli_web_exports.h"
#include "filter_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "pbvlan_api.h"


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
  L7_uint32 argGroupId= 1;
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];   /* This is 1 + L7_PBVLAN_MAX_GROUP_NAME */
  L7_uint32 unit, rc;

  L7_uint32 numArg, groupId;

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

  if ( (numArg != 1) || (strlen(argv[index+argGroupId]) >= sizeof(strGroupId)) )
  {

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_VlanProtocol_group_id);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_VlanProtocol_no_group_id);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  memset(strGroupId, 0, sizeof(strGroupId));
  osapiStrncpySafe(strGroupId, argv[index + argGroupId], sizeof(strGroupId));

  if (convertTo32BitUnsignedInteger(strGroupId, &groupId) != L7_SUCCESS)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_VlanProtocol_group_id);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_VlanProtocol_no_group_id);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }


  /*******Check if the Flag is Set for Execution*************/

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc =  usmDbPbVlanGroupCreate(unit, groupId);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_ALREADY_CONFIGURED)
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
    
    if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpDoesntExist);
    }
      
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbPbVlanGroupDelete(unit, groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_base_VlanGrpNotDeld);
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
    if (convertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }
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
  L7_uint32 argGroupId = 1;
  L7_uint32 argProtocol = 3;
  L7_uint32 groupId, protocol, i, j;
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 *temp1 = NULL;
  L7_char8 strProtocol[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 protocolListPrev[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS], ptype[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
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
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_ethertype);
    }
    if ((strlen(argv[index+argGroupId]) >= sizeof(strGroupId)) ||
        (strlen(argv[index+argProtocol]) >= sizeof(strProtocol)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_ethertype);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
       OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

       if (convertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
       {
        if(usmDbPbVlanGroupIDGet(unit, strGroupId, &groupId) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
        }
       }

      if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }

      OSAPI_STRNCPY_SAFE(strProtocol,argv[index + argProtocol]);
      /*cliConvertToLowerCase(strProtocol);*/

      memset(protocolListPrev, 0x00, sizeof(protocolListPrev));
    
      /* get Ethertype hex value */
      for(temp1 = strtok(strProtocol,","), i = 0; (temp1 != NULL); temp1 = strtok(NULL,","))
      {
        cliConvertToLowerCase(temp1);
        if (strcmp(temp1, pStrInfo_common_IpOption) == 0)
        {
          protocol = L7_USMDB_PROTOCOL_HEX_IP;
          protocolListPrev[i] = protocol;
          ptype[i] = 0;
          i++;
          continue;
        }
        else if (strcmp(temp1, pStrInfo_common_Arp_2) == 0)
        {
          protocol = L7_USMDB_PROTOCOL_HEX_ARP;
          protocolListPrev[i] = protocol;
          ptype[i] = 0;
          i++;
          continue;
        }
        else if (strcmp(temp1, pStrInfo_base_Ipx_1) == 0)
        {
          protocol = L7_USMDB_PROTOCOL_HEX_IPX;
          protocolListPrev[i] = protocol;
          ptype[i] = 0;
          i++;
          continue;
        } 
        else if((temp1[0] != '0' ) && ((temp1[1] != 'x') || (temp1[1] != 'X')))
        {
          if (convertTo32BitUnsignedInteger(temp1, &protocol) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_ethtype);
          }
        }
        else if (cliConvertEtypeCustomValue(temp1, &protocol) != L7_TRUE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_ethtype);
        }
        /* verify custom ethertype value is within allowed range */
        if ((protocol< L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MIN) ||
          (protocol> L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MAX))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_ethtype);
        }
        protocolListPrev[i] = protocol;
        ptype[i] = 1;
        i++;
      }
    
      for(j=0; j < i; j++)
      {
        /* If the given protocol is already exist in that group, just skip */ 
        if(usmDbPbVlanGroupProtocolGetExact(unit, groupId, protocolListPrev[j]) == L7_SUCCESS)
        {
          found = L7_TRUE;
          break;
        }
      }
  
      if (found == L7_FALSE)
      {
  
         for(j=0; j < i; j++)
         {
           rc = usmDbPbVlanGroupProtocolAdd(unit, groupId, protocolListPrev[j], ptype[j]);

            if (rc == L7_ERROR )
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_VlanGrpAdd);
            }
            else if (rc == L7_ALREADY_CONFIGURED)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_GrpCfgured);
            }
            else if (rc == L7_TABLE_IS_FULL)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_GrpMaxProtocolsReached);
            }
        }
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_ProtoCfgured);
      }
    }

    cliSyntaxBottom(ewsContext);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 3)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_no_VlanProtoGrpAdd_ethertype);
    }
    if (strlen(argv[index+argGroupId]) >= sizeof(strGroupId))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_no_VlanProtoGrpAdd_ethertype);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

      if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
      {
        if(usmDbPbVlanGroupIDGet(unit, strGroupId, &groupId) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
        }
      }

      if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }

      OSAPI_STRNCPY_SAFE(strProtocol,argv[index + argProtocol]);
	
      memset(protocolListPrev, 0x00, sizeof(protocolListPrev));
    
      /* get Ethertype hex value */
      for(temp1 = strtok(strProtocol,","), i = 0; (temp1 != NULL); temp1 = strtok(NULL,","))
      {
        cliConvertToLowerCase(temp1);
        if (strcmp(temp1, pStrInfo_common_IpOption) == 0)
        {
          protocol = L7_USMDB_PROTOCOL_HEX_IP;
          protocolListPrev[i] = protocol;
          i++;
          continue;
        }
        else if (strcmp(temp1, pStrInfo_common_Arp_2) == 0)
        {
          protocol = L7_USMDB_PROTOCOL_HEX_ARP;
          protocolListPrev[i] = protocol;
          i++;
          continue;
        }
        else if (strcmp(temp1, pStrInfo_base_Ipx_1) == 0)
        {
          protocol = L7_USMDB_PROTOCOL_HEX_IPX;
          protocolListPrev[i] = protocol;
          i++;
          continue;
        }
        else if((temp1[0] != '0' ) && ((temp1[1] != 'x') || (temp1[1] != 'X')))
        {
          if (convertTo32BitUnsignedInteger(temp1, &protocol) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_no_VlanProtoGrpAdd_ethertype);
          }
        }
        else if (cliConvertEtypeCustomValue(temp1, &protocol) != L7_TRUE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_no_VlanProtoGrpAdd_ethertype);
        }
    
        /* verify custom ethertype value is within allowed range */
        if ((protocol< L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MIN) ||
          (protocol> L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MAX))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_ethtype);
        }
        protocolListPrev[i] = protocol;
        i++;
      }
  
      for(j=0; j < i; j++)
      {
        if (usmDbPbVlanGroupProtocolDelete(unit, groupId, protocolListPrev[j]) != L7_SUCCESS )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_ProtoNotRemoved);
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

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

      if (convertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
      {
        if(usmDbPbVlanGroupIDGet(unit, strGroupId, &groupId) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
        }
      }

      if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }

      OSAPI_STRNCPY_SAFE(strVlan,argv[index + argVlan]);
 
      if (convertTo32BitUnsignedInteger(strVlan, &vlan)!=L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

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

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);

      if (convertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
      {
        if(usmDbPbVlanGroupIDGet(unit, strGroupId, &groupId) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
        }
      }

      if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }

      OSAPI_STRNCPY_SAFE(strVlan,argv[index + argVlan]);

      if (convertTo32BitUnsignedInteger(strVlan, &vlan)!=L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

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
 * @cmdsyntax protocol vlan group  <group-list>
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
  L7_RC_t status = L7_SUCCESS;
  L7_uint32 groupCount;
  L7_uint32 groupList[L7_PBVLAN_MAX_NUM_GROUPS];

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

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

    if(cliParseRangeInput(strGroupId, &groupCount, groupList, L7_PBVLAN_MAX_NUM_GROUPS))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_ProtoVlanGrpListParse);    
    }
 

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        for(i=0; i<groupCount; i++)
        {
          groupId = groupList[i];
          
        /* Get the list of interfaces already configured with the group */
        intIfNum = 0;
        numIntfPrev = 0;
        while (usmDbPbVlanGroupPortGetNext(unit, groupId, intIfNum, &intIfNum) == L7_SUCCESS)
        {
          intfListPrev[numIntfPrev++] = intIfNum;
        }

        if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
        {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
        }

        rc = usmDbPbVlanGroupPortAdd(unit, groupId, iface);
        if (rc == L7_FAILURE )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_AddIntfToGrp);
        }
        else if (rc == L7_ALREADY_CONFIGURED)
        {
          found = L7_FALSE;
          for (i = 0; i < numIntfPrev; i++)
          {
            if (iface == intfListPrev[i])
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_IntfAlreadyCfguredWithGrp); /*Interface already configured with this group.*/
              found = L7_TRUE;
              break;
            }
          }
          if (found == L7_FALSE)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_IntfAlreadyCfguredWithSameProto); /*Interface already configured with another group using the same protocol.*/
          }
        }
      }
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

    if(cliParseRangeInput(strGroupId, &groupCount, groupList, L7_PBVLAN_MAX_NUM_GROUPS))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_ProtoVlanGrpListParse);    
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        for(i=0; i<groupCount; i++)
        {
          groupId = groupList[i];

          if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
          }
          if (usmDbPbVlanGroupPortDelete(unit, groupId, iface) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotIntfRemoved);
          }
        }
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

      if (convertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {

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
      if (alreadyConf == L7_TRUE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_GrpAlreadyCfgured); /*One or more interfaces already configured with this group.*/
      }
      else if (found == L7_FALSE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SameProtoAlreadyCfgured); /*One or more interfaces already configured with another group with same protocol.*/
      }
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
  
      if (convertTo32BitUnsignedInteger(strGroupId, &groupId)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
       if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }

      rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF, 0, &iface);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
      }
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
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  [no] vlan protocol group name <groupID> <groupname>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanProtocolGroupName(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 argGroupId= 1;
  L7_uint32 argGroupName= 2;
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];   /* This is 1 + L7_PBVLAN_MAX_GROUP_NAME */
  L7_char8 strGroupName[L7_CLI_MAX_STRING_LENGTH];   /* This is 1 + L7_PBVLAN_MAX_GROUP_NAME */
  L7_uint32 unit, rc;
  L7_uint32 numArg, groupId;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /*Macro to check that the command supports Normal and No modes*/

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( (numArg != 2) || (strlen(argv[index+argGroupId]) >= sizeof(strGroupId)) 
       || (strlen(argv[index+argGroupName]) >= sizeof(strGroupName)))
    {
	  return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrInfo_base_VlanProtocol_Name);
    }

    memset(strGroupId, 0, sizeof(strGroupId));
    osapiStrncpySafe(strGroupId, argv[index + argGroupId], sizeof(strGroupId));

    if (convertTo32BitUnsignedInteger(strGroupId, &groupId) != L7_SUCCESS)
    {
	  return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrInfo_base_VlanProtocol_Name);
    }

    memset(strGroupName, 0, sizeof(strGroupName));
    osapiStrncpySafe(strGroupName, argv[index + argGroupName], sizeof(strGroupName));

    if(strlen(strGroupName) > L7_PBVLAN_MAX_GROUP_NAME) 
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrInfo_base_CfgProtoGroupRange);
    }

    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }
      rc =  usmDbPbVlanGroupNameSet(unit, groupId, strGroupName);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_base_VlanGrpNameNotSet);
      } 
    }
  }
  else
  {
    if ((numArg != 1) || (strlen(argv[index+argGroupId]) >= sizeof(strGroupId)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrInfo_base_NoVlanProtocol_Name);
    }

    memset(strGroupId, 0, sizeof(strGroupId));
    osapiStrncpySafe(strGroupId, argv[index + argGroupId], sizeof(strGroupId));

    if (convertTo32BitUnsignedInteger(strGroupId, &groupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrInfo_base_NoVlanProtocol_Name);
    }

    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
       if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }

       rc =  usmDbPbVlanGroupNameSet(unit, groupId, L7_PBVLAN_DEFAULT_GROUP_NAME);
       if (rc != L7_SUCCESS)
       {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_base_VlanGrpNameDelete);
       } 
    }

  }
  
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

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
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  [no] vlan protocol group <groupname>
*
* @cmdhelp
*
* @cmddescript This command is deprecated, So we are migrating to new syntax of 
*              this command    
* @end
*
*********************************************************************/
const L7_char8 *commandMigrationVlanProtocolGroup(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 argGroupName = 1;
  L7_char8 strGroupName[L7_CLI_MAX_STRING_LENGTH];   /* This is 1 + L7_PBVLAN_MAX_GROUP_NAME */
  L7_uint32 unit, rc;
  L7_uint32 numArg, namegroupID;
  static L7_uint32 groupID =1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

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

  if (strlen(argv[index+argGroupName]) > L7_PBVLAN_MAX_GROUP_NAME)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
	{
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_VlanProtocol_group_id);
	}
    else if (ewsContext->commType == CLI_NO_CMD)
	{
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_VlanProtocol_no_group_id);
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
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc =  usmDbPbVlanGroupCreate(unit, groupID);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_ALREADY_CONFIGURED)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpExists);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_VlanGrpNotCreated);
        }

        return cliSyntaxReturnPrompt (ewsContext, "");
      }
      /* Increment the groupID, so that next group will get the furtherID */
      namegroupID = groupID;
      groupID++; 

      if (usmDbPbVlanGroupGetExact(unit, namegroupID) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }
   
      rc =  usmDbPbVlanGroupNameSet(unit, namegroupID, strGroupName);

      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpNameNotSet);
      }
    }
  }
  else
  {
    /*No command configuration will not appear in configuration file. 
      So we can ignore that case */
    return cliSyntaxReturnPrompt (ewsContext, "");
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
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  vlan protocol group add  <groupid> <protocol>
*
* @cmdhelp
*
* @cmddescript This command is deprecated. So we are migrating to new 
*              syntax of this command.
* @end
*
*********************************************************************/
const L7_char8 *commandMigrationVlanProtocolGroupAdd(EwsContext ewsContext, L7_uint32 argc,
                                            const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 argGroupId = 2;
  L7_uint32 argProtocol = 3;
  L7_uint32 groupId, protocol, i;
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strProtocol[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 protocolListPrev[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 ptype[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_BOOL found = L7_FALSE;
  L7_RC_t rc;
  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

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
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_ethertype);
    }
    if ((strlen(argv[index+argGroupId]) >= sizeof(strGroupId)) ||
        (strlen(argv[index+argProtocol]) >= sizeof(strProtocol)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_VlanProtoGrpAdd_ethertype);
    }
	
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
	OSAPI_STRNCPY_SAFE(strGroupId,argv[index + argGroupId]);
   
      if (cliConvertTo32BitUnsignedInteger(strGroupId, &groupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
      }

      OSAPI_STRNCPY_SAFE(strProtocol,argv[index + argProtocol]);
      cliConvertToLowerCase(strProtocol);

      memset(protocolListPrev, 0x00, sizeof(protocolListPrev));
      /* Get the list of Protocols currently configured with the Group */
      rc = usmDbPbVlanGroupProtocolGet(unit, groupId, protocolListPrev, ptype);

       if (strcmp(strProtocol, pStrInfo_common_IpOption) == 0)
      {
        protocol = L7_USMDB_PROTOCOL_HEX_IP;
      }
      else if (strcmp(strProtocol, pStrInfo_common_Arp_2) == 0)
      {
        protocol = L7_USMDB_PROTOCOL_HEX_ARP;
      }
      else if (strcmp(strProtocol, pStrInfo_base_Ipx_1) == 0)
      {
        protocol = L7_USMDB_PROTOCOL_HEX_IPX;
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
          if (usmDbPbVlanGroupGetExact(unit, groupId) != L7_SUCCESS)
          {
             return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpIdNotExist);
          }
          rc = usmDbPbVlanGroupProtocolAdd(unit, groupId, protocol, 0);
          if (rc == L7_FAILURE )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_VlanGrpAdd);
          }
          else if (rc == L7_ALREADY_CONFIGURED)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_GrpCfgured);
          }
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_ProtoCfgured);
      }
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* No command configuration will not appear in configuration file,
       we are not handling no commands */
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
