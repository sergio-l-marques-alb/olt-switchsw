/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/vlan/dot1v/cli_show_pbvlan.c
 *
 * @purpose show commands for protocol based vlan
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



#include "ews.h"
#include "cliutil.h"
#include "cliapi.h"
#include "usmdb_pbvlan_api.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"


/*********************************************************************
*
* @purpose Displays Protocol Based VLAN summary information
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes     Here we only display the visible interfaces.
*
* @cmdsyntax    show protocol summary
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowProtocol(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
   L7_uint32 groupID, nextGroupID;
   L7_char8 group_name[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 groupid_str[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
   L7_uint32 intIfNum;
   L7_uint32 i, vid;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8   interfaceList[256];
   L7_BOOL all = L7_FALSE;
   L7_BOOL first = L7_TRUE;
   L7_BOOL first2 = L7_TRUE;
   L7_BOOL commaFlag = L7_FALSE;
   L7_uint32 u, s, p;
   L7_uint32 argGroupId = 1;
   L7_uint32 unit;

   /* validity check (arguments, Access */
   cliSyntaxTop(ewsContext);

   if (cliNumFunctionArgsGet() != 1)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowProto_1);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (strcmp(argv[index+argGroupId], pStrInfo_common_All) == 0)
  {
    all = L7_TRUE;
    groupID = 0;
    nextGroupID = 0;
    if (usmDbPbVlanGroupGetFirst(unit, &groupID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NoGrpsExist);
    }
  }
  else
  {
    if (strlen(argv[index+argGroupId]) >= sizeof(groupid_str))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpDoesntExist);
    }
    OSAPI_STRNCPY_SAFE(groupid_str,argv[index + argGroupId]);
    cliConvertTo32BitUnsignedInteger(groupid_str, &groupID);

    if (usmDbPbVlanGroupGetExact(unit, groupID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpDoesntExist);
    }
  }

  first = L7_TRUE;
  first2 = L7_TRUE;

  /* Heading for the router rip interface summary */
  ewsTelnetWriteAddBlanks (1, 0, 18, 1, L7_NULLPTR, ewsContext,pStrInfo_common_Grp_1);
  ewsTelnetWriteAddBlanks (1, 0, 3, 1, L7_NULLPTR, ewsContext,pStrInfo_base_GrpNameIdProtoSVlanIntfS);
   ewsTelnetWrite(ewsContext,"\r\n----------------  ------  -----------  ----  ------------------------ \r\n");

   do
   {
      if (first != L7_TRUE)
      {
         groupID = nextGroupID;
      }
    else
    {first = L7_FALSE;}

    memset (buf, 0,sizeof(buf));
    memset (group_name, 0,sizeof(group_name));
    if ((usmDbPbVlanGroupNameGet(unit, groupID, group_name)) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), "%-18s", group_name);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "%-18s", pStrInfo_common_Blank);
    }
    ewsTelnetWrite(ewsContext, buf);

    ewsTelnetPrintf (ewsContext, "%-8d", groupID);

    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    memset(protocol, 0, L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS);
    commaFlag = L7_FALSE;
    if ((usmDbPbVlanGroupProtocolGet(unit, groupID, protocol)) == L7_SUCCESS)
    {

      for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
      {
        switch (protocol[i])
        {
          case L7_USMDB_PROTOCOL_IP:
            OSAPI_STRNCAT(buf, pStrInfo_common_Ip);
            commaFlag = L7_TRUE;
            break;

          case L7_USMDB_PROTOCOL_ARP:
            if (commaFlag == L7_TRUE)
            {
              OSAPI_STRNCAT(buf, ",");
            }
            OSAPI_STRNCAT(buf, pStrInfo_common_Arp);
            commaFlag = L7_TRUE;
            break;

          case L7_USMDB_PROTOCOL_IPX:
            if (commaFlag == L7_TRUE)
            {
              OSAPI_STRNCAT(buf, ",");
            }
            OSAPI_STRNCAT(buf, pStrInfo_common_Ipx);
            break;

          default:
            break;
        }
      }
      osapiSnprintf(stat, sizeof(stat), "%-13s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (buf, 0, sizeof(buf));
    if ((usmDbPbVlanGroupVIDGet(unit, groupID, &vid)) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), "%-6d", vid);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "%-6s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, buf);

    intIfNum = 0;
    memset (stat, 0, sizeof(stat));
    memset (buf, 0, sizeof(buf));
    memset (interfaceList, 0, sizeof(interfaceList));
    commaFlag = L7_FALSE;
    while (usmDbPbVlanGroupPortGetNext(unit, groupID, intIfNum, &intIfNum) == L7_SUCCESS)
    {
      if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
        if (commaFlag == L7_TRUE)
        {
          OSAPI_STRNCAT(interfaceList, ",");
        }
        commaFlag = L7_TRUE;

        OSAPI_STRNCAT(interfaceList, stat);
      }
    }

    memset (stat, 0,sizeof(stat));
    osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceList, 24));
    do
    {
      if (first2 == L7_TRUE)
      {
        first2 = L7_FALSE;
        osapiSnprintf(stat, sizeof(stat), "%-24s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "\r\n%-45s", " ");
        osapiSnprintf(stat, sizeof(stat), "%-24s", buf);
      }

      ewsTelnetWrite(ewsContext,stat);

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceList, 24));
      }
      while (strcmp(buf, interfaceList) != 0);
      first2 = L7_TRUE;

      cliSyntaxBottom(ewsContext);
   }
   while (usmDbPbVlanGroupGetNext(unit, groupID, &nextGroupID) == L7_SUCCESS && (all == L7_TRUE));

  return cliSyntaxReturnPrompt (ewsContext, "");
}


/*********************************************************************
*
* @purpose Displays Protocol Based VLAN summary information
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes     Here we only display the visible interfaces.
*
* @cmdsyntax    show port protocol {<groupid> | all}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowPortProtocol(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
   L7_char8 group_name[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 groupid_str[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
   L7_uint32 intIfNum;
   L7_uint32 vid;
   static L7_char8 buf[3200];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
   
   static L7_char8   interfaceList[3200]; /* 8*50*8 Max switches * Max ports * perportspace */

   L7_BOOL commaFlag = L7_FALSE;
   L7_uint32 u, s, p;
   L7_uint32 argGroupId = 1;
   L7_uint32 unit,i,numLines;

   static L7_RC_t  rc = L7_FAILURE;
   static L7_uint32 groupID = 0;
   static L7_uint32 nextGroupID = 0;
   static L7_BOOL first = L7_FALSE;
   static L7_BOOL fullLine = L7_TRUE;
   static L7_BOOL first2 = L7_TRUE;
   static L7_BOOL flag = L7_FALSE;
   static L7_BOOL all = L7_FALSE;
   static L7_BOOL emptySpace = L7_FALSE;
   static L7_BOOL groupUpdated = L7_TRUE;

   /* validity check (arguments, Access */
   cliSyntaxTop(ewsContext);
   cliCmdScrollSet( L7_FALSE);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      groupID = 0;
      nextGroupID = 0;
      groupUpdated = L7_TRUE;
      emptySpace = L7_FALSE;
      first = L7_FALSE;
      fullLine = L7_FALSE;
      first2 = L7_TRUE;
      flag = L7_FALSE;
      all = L7_FALSE;
      rc = L7_FAILURE;
    memset(interfaceList, 0, sizeof(interfaceList));
    memset(buf, 0, sizeof(buf));
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      groupID = 0;
      nextGroupID = 0;
      first = L7_FALSE;
      fullLine = L7_FALSE;
      groupUpdated = L7_TRUE;
      emptySpace = L7_FALSE;
      first2 = L7_TRUE;
      flag = L7_FALSE;
      all = L7_FALSE;
      rc = L7_FAILURE;
      memset(interfaceList, 0, sizeof(interfaceList));
      memset(buf, 0, sizeof(buf));
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowProto_1);
    }

    if (strcmp(argv[index+argGroupId], pStrInfo_common_All) == 0)
    {
      all = L7_TRUE;
      groupID = 0;
      nextGroupID = 0;
      if (usmDbPbVlanGroupGetFirst(unit, &groupID) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NoGrpsExist);
      }
      rc = L7_SUCCESS;
    }
    else
    {
      if (strlen(argv[index+argGroupId]) >= sizeof(groupid_str))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpDoesntExist);
      }
      OSAPI_STRNCPY_SAFE(groupid_str,argv[index + argGroupId]);
      cliConvertTo32BitUnsignedInteger(groupid_str, &groupID);

      if (usmDbPbVlanGroupGetExact(unit, groupID) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpDoesntExist);
       }
       rc = L7_SUCCESS;
     }
     first = L7_TRUE;
     fullLine = L7_TRUE;
   }

   numLines = 0;
   /* Heading for the router rip interface summary */
  ewsTelnetWriteAddBlanks (1, 0, 18, 1, L7_NULLPTR, ewsContext,pStrInfo_common_Grp_1);
  ewsTelnetWriteAddBlanks (1, 0, 3, 1, L7_NULLPTR, ewsContext,pStrInfo_base_GrpNameIdProtoSVlanIntfS);
   ewsTelnetWrite(ewsContext,"\r\n----------------  ------  -----------  ----  ------------------------ ");

   while( (rc == L7_SUCCESS)&&(numLines < (CLI_MAX_SCROLL_LINES-6)) )
   {
      if(groupUpdated == L7_TRUE)
      {
       if (first != L7_TRUE)
      {
        groupID = nextGroupID;
      }
      else
      {
        first = L7_FALSE;
      }
      }
      else
      {
         groupUpdated = L7_TRUE;
	 emptySpace = L7_TRUE;
      }

      if(fullLine == L7_TRUE)
      {
      memset(buf, 0, sizeof(buf));
      memset(group_name, 0, sizeof(group_name));
      if ((usmDbPbVlanGroupNameGet(unit, groupID, group_name)) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%-18s", group_name);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%-18s", pStrInfo_common_Blank);
      }

      ewsTelnetWrite(ewsContext, buf);

      ewsTelnetPrintf (ewsContext, "%-8d", groupID);

      memset(buf, 0, sizeof(buf));
      memset(stat, 0, sizeof(stat));
      memset(protocol, 0, L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS);
      commaFlag = L7_FALSE;
      if ((usmDbPbVlanGroupProtocolGet(unit, groupID, protocol)) == L7_SUCCESS)
      {
        for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
        {
          switch (protocol[i])
          {
            case L7_USMDB_PROTOCOL_IP:
              OSAPI_STRNCAT(buf, pStrInfo_common_Ip);
              commaFlag = L7_TRUE;
              break;

            case L7_USMDB_PROTOCOL_ARP:
              if (commaFlag == L7_TRUE)
              {
                OSAPI_STRNCAT(buf, ",");
              }
              OSAPI_STRNCAT(buf, pStrInfo_common_Arp);
              commaFlag = L7_TRUE;
              break;

            case L7_USMDB_PROTOCOL_IPX:
              if (commaFlag == L7_TRUE)
              {
                OSAPI_STRNCAT(buf, ",");
              }
              OSAPI_STRNCAT(buf, pStrInfo_common_Ipx);
              break;

            default:
              break;
          }
        }
        osapiSnprintf(stat, sizeof(stat), "%-13s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);
      memset(buf, 0, sizeof(buf));
      if ((usmDbPbVlanGroupVIDGet(unit, groupID, &vid)) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%-6d", vid);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "%-6s", pStrInfo_common_Line);
      }

      ewsTelnetWrite(ewsContext, buf);

      intIfNum = 0;
      memset(stat, 0, sizeof(stat));
      memset(interfaceList, 0, sizeof(interfaceList));
      commaFlag = L7_FALSE;
      while (usmDbPbVlanGroupPortGetNext(unit, groupID, intIfNum, &intIfNum) == L7_SUCCESS)
      {
        if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));

          if (commaFlag == L7_TRUE)
          {
            OSAPI_STRNCAT(interfaceList, ",");
          }

          commaFlag = L7_TRUE;

          OSAPI_STRNCAT(interfaceList, stat);
        }
        memset(stat, 0, sizeof(stat));
      }
      fullLine = L7_FALSE;
      numLines++;
      memset(stat, 0, sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceList, 24));
	if(numLines == (CLI_MAX_SCROLL_LINES-6))
	{
        ewsTelnetPrintf (ewsContext, "%-24s", buf);
        memset(stat, 0, sizeof(stat));
        osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceList, 24));
	  first2 = L7_FALSE;
	}

      } /* Full line if condition */

      do
      {
	 if((emptySpace == L7_TRUE)&& (strlen(buf) == 0))
	 {
	   emptySpace = L7_FALSE;
	   continue;
	  }

         if( numLines < (CLI_MAX_SCROLL_LINES-6) )
         {
     	   if(first2 == L7_FALSE)
        {
          numLines++;
        }

        if (first2 == L7_TRUE)
        {
          first2 = L7_FALSE;
          osapiSnprintf(stat, sizeof(stat), "%-24s", buf);
        }
        else
        {
          ewsTelnetPrintf (ewsContext, "\r\n%-45s", " ");
          osapiSnprintf(stat, sizeof(stat), "%-24s", buf);
        }
        ewsTelnetWrite(ewsContext,stat);
      }
      else
      {
        flag = L7_TRUE;
        break;
      }
      memset(buf, 0, sizeof(buf));
      memset(stat, 0, sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceList, 24));
    } while (strcmp(buf, interfaceList) != 0);

      if(flag == L7_TRUE)
      {
         groupUpdated = L7_FALSE;
	 flag = L7_FALSE;
         cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
         for (i=1; i<argc; i++)
         {
        OSAPI_STRNCAT(cmdBuf, " ");
        OSAPI_STRNCAT(cmdBuf, argv[i]);
	 }
	 cliAlternateCommandSet(cmdBuf);
      return pStrInfo_common_Name_2;      /* --More-- or (q)uit */
      }
      else
      {
	 fullLine = L7_TRUE;
      }

      if(all == L7_TRUE)
      {
	 rc = usmDbPbVlanGroupGetNext(unit, groupID, &nextGroupID);
      }
      else
      {
         groupID = 0; 
         nextGroupID = 0;
         first = L7_FALSE;
	 groupUpdated = L7_TRUE;
	 emptySpace = L7_FALSE;
         fullLine = L7_FALSE;
         first2 = L7_TRUE;
         flag = L7_FALSE;
         all = L7_FALSE;
         rc = L7_FAILURE;
      memset(interfaceList, 0, sizeof(interfaceList));
      memset(buf, 0, sizeof(buf));
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
	      
      }
      first2 = L7_TRUE;
   }

   if( (rc == L7_SUCCESS)&&(numLines >= (CLI_MAX_SCROLL_LINES-6)))
   {
       cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
       for (i=1; i<argc; i++)
       {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[i]);
       }
       cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;      /* --More-- or (q)uit */
   }

   groupID = 0;
   nextGroupID = 0;
   first = L7_FALSE;
   groupUpdated = L7_TRUE;
   emptySpace = L7_FALSE;
   fullLine = L7_FALSE;
   first2 = L7_TRUE;
   flag = L7_FALSE;
   all = L7_FALSE;
   rc = L7_FAILURE;
  memset(interfaceList, 0, sizeof(interfaceList));
  memset(buf, 0, sizeof(buf));
  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliSyntaxReturnPrompt (ewsContext, "");
}

