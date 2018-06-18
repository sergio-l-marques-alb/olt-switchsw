/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/vlan/dot1v/cli_show_running_config_pbvlan.c
 *
 * @purpose show running config commands for protocol based vlan
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
#include "strlib_common_cli.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"



#include "ews.h"
#include "cli.h"
#include "cli_show_running_config.h"
#include "usmdb_pbvlan_api.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
      





/*********************************************************************
 * @purpose  To print the running configuration of protocol Info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningConfigProtocolInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 groupID, nextGroupID;
  L7_char8 group_name[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 i;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL first = L7_TRUE;
  L7_RC_t rc;

  groupID = 0;
  nextGroupID = 0;
  first = L7_TRUE;

  if (usmDbPbVlanGroupGetFirst(unit, &groupID) == L7_SUCCESS)
  {

    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Cfgure_1);
    groupID = 0;
    do
    {
      rc = usmDbPbVlanGroupGetFirst(unit, &groupID);

      if (first != L7_TRUE)
      {
        groupID = nextGroupID;
      }
      else
      {first = L7_FALSE;}

      memset (buf, 0,sizeof(buf));

      memset(buf, 0x00, sizeof(buf));
      osapiSnprintf(buf, sizeof(buf), "\r\nvlan protocol group %d", groupID);
      EWSWRITEBUFFER(ewsContext, buf);

      memset (group_name, 0,sizeof(group_name));
      if ( ((usmDbPbVlanGroupNameGet(unit, groupID, group_name)) == L7_SUCCESS) &&
           (strcmp(group_name,L7_PBVLAN_DEFAULT_GROUP_NAME) != 0) )
      {
        memset(buf, 0x00, sizeof(buf));
        osapiSnprintf(buf, sizeof(buf), "\r\nvlan protocol group name %d \"%s\"", groupID, group_name);
        EWSWRITEBUFFER(ewsContext, buf);
      }

      memset (buf, 0,sizeof(buf));
      memset(protocol, 0, L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS);
      memset (stat, 0,sizeof(stat));
      if ((usmDbPbVlanGroupProtocolGet(unit, groupID, protocol, type)) == L7_SUCCESS)
      {

        for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
        {
          if(protocol[i] != L7_PBVLAN_INVALID_PROTOCOL)   
          { 
            memset(stat, 0x00, sizeof(stat));
            if(type[i] == 0)
            {
              if(protocol[i] == L7_USMDB_PROTOCOL_HEX_IP)
              {
                OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_IpOption);
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_VlanProtoGrpAddProto, groupID, buf);
              }
              else if(protocol[i] == L7_USMDB_PROTOCOL_HEX_ARP)
              {
                OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_Arp_2);
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_VlanProtoGrpAddProto, groupID, buf); 
              }
              else if(protocol[i] == L7_USMDB_PROTOCOL_HEX_IPX)
              {
                OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Ipx_1);
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_VlanProtoGrpAddProto, groupID, buf); 
              }
            }
            else
            {
              osapiSnprintf(stat, sizeof(stat), "\r\nvlan protocol group add protocol %d ethertype 0x%4.4x", groupID, protocol[i]);
            }
          }
          EWSWRITEBUFFER(ewsContext, stat);
          memset (stat, 0,sizeof(stat));
          memset (buf, 0,sizeof(buf));
        }
      }
    }
    while (usmDbPbVlanGroupGetNext(unit, groupID, &nextGroupID) == L7_SUCCESS);

    EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_Exit);

  }

  return L7_SUCCESS;
}

L7_RC_t cliRunningPbVlanPhysicalInterfaceInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 groupID, nextGroupID;
  L7_uint32 groupCount = 0, groupList[L7_PBVLAN_MAX_NUM_GROUPS];

  if (usmDbPbVlanIsValidIntf(unit, interface) == L7_TRUE)
  {
    rc = usmDbPbVlanGroupGetFirst(unit, &nextGroupID);
    do
    {
      groupID = nextGroupID;
      if(usmDbPbVlanGroupPortGetExact(unit, groupID, interface) == L7_SUCCESS)
      {
        groupList[groupCount] = groupID;
        groupCount++;
      }
    } while (usmDbPbVlanGroupGetNext(unit, groupID, &nextGroupID) == L7_SUCCESS);
  }

  if (groupCount > 0)
  {
    displayRangeFormat(ewsContext,groupList,groupCount,pStrInfo_base_ProtoVlanGrp);
  }

  return rc;
}

/*********************************************************************
 * @purpose  To print the running configuration of protocol vlan Info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningConfigProtocolVlanInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 groupID, nextGroupID;
  L7_uint32 vid;
  L7_BOOL first = L7_TRUE;
  L7_RC_t rc;

  rc = usmDbPbVlanGroupGetFirst(unit, &groupID);
  if (rc == L7_SUCCESS)
  {
    do
    {
      if (first != L7_TRUE)
      {
        groupID = nextGroupID;
      }
      else
      {
        first = L7_FALSE;
      }
      if ((usmDbPbVlanGroupVIDGet(unit, groupID, &vid) == L7_SUCCESS ))
      {
        cliShowCmdIntAddInt (ewsContext, vid, FD_PBVLAN_DEFAULT_GROUP_VLAN_ID,
                             pStrInfo_switching_ProtoGrp, groupID);
      }

    }
    while (usmDbPbVlanGroupGetNext(unit, groupID, &nextGroupID) == L7_SUCCESS);
  }

  return rc;
}
