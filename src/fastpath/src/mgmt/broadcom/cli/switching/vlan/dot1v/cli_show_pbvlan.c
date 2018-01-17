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
   L7_uint32 type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
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
    if ((usmDbPbVlanGroupProtocolGet(unit, groupID, protocol, type)) == L7_SUCCESS)
    {

      for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
      {
        switch (protocol[i])
        {
          case L7_USMDB_PROTOCOL_HEX_IP:
            OSAPI_STRNCAT(buf, pStrInfo_common_Ip);
            commaFlag = L7_TRUE;
            break;

          case L7_USMDB_PROTOCOL_HEX_ARP:
            if (commaFlag == L7_TRUE)
            {
              OSAPI_STRNCAT(buf, ",");
            }
            OSAPI_STRNCAT(buf, pStrInfo_common_Arp);
            commaFlag = L7_TRUE;
            break;

          case L7_USMDB_PROTOCOL_HEX_IPX:
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
const char *commandShowPortProtocol(EwsContext ewsContext, uintf argc, const char **argv, uintf index)
{
  L7_char8 group_name[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 groupid_str[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 intIfNum;
  L7_uint32 u, s, p;
  L7_uint32 vid;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 temp[3200]; /* 8*50*8 Max switches * Max ports * perportspace */

  L7_uint32 argGroupId = 1;
  L7_uint32 unit, i, numLines;
  
  L7_BOOL displayProto = L7_FALSE;

  static L7_BOOL all = L7_FALSE;
  static L7_char8   interfaceList[3200]; /* 8*50*8 Max switches * Max ports * perportspace */
  static L7_char8   intfDisplay[3200]; /* 8*50*8 Max switches * Max ports * perportspace */
  static L7_char8   protocolList[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8   protoDisplay[L7_CLI_MAX_STRING_LENGTH];
  static L7_RC_t  rc = L7_FAILURE;
  static L7_uint32 groupID = 0;
  static L7_uint32 nextGroupID = 0;
  static L7_BOOL groupFlag = L7_TRUE;
  static L7_BOOL printProto= L7_TRUE;
  static L7_BOOL printIntf= L7_TRUE;
  static L7_BOOL fullLine = L7_TRUE;

  L7_BOOL countFlag = L7_TRUE;
  L7_BOOL commaFlag = L7_TRUE;

  unit = cliGetUnitId();

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      groupID = 0;
      nextGroupID = 0;
      all = L7_FALSE;
      rc = L7_FAILURE;

      groupFlag = L7_TRUE;
      printProto= L7_TRUE;
      printIntf = L7_TRUE;
      fullLine = L7_TRUE;

      memset(intfDisplay, 0x00, sizeof(intfDisplay));
      memset(interfaceList, 0x00, sizeof(interfaceList));
      memset(protoDisplay, 0x00, sizeof(protoDisplay));
      memset(protocolList, 0x00, sizeof(protocolList));

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
      if ((cliConvertTo32BitUnsignedInteger(groupid_str, &groupID) 
           != L7_SUCCESS) ||
          (usmDbPbVlanGroupGetExact(unit, groupID) != L7_SUCCESS))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_VlanGrpDoesntExist);
      }
      rc = L7_SUCCESS;
    }
  }

  numLines = 0;

  /* Flags meaning:
     fullLine: This flag indicates full line should be diplayed or not.Since either protocols or interfaces
               will go beyond more than one line.So in that case other fields should not be displayed

     groupFlag: This flag indicates either protocols or interfaces needs to be grouped or not.Only if it is
                new group then only we need to group the protocols.

     countFlag: This flag indicates number of lines count should be incremented or not

     printProto: This flag indicates wheather protocol list should be diplayed or not

     printIntf:  This flag indicates wheather interface list should be diplayed or not

     displayProto: This flag indicates protocol information is displayed. So that other fields space
                   will be adjusted */

  /* Heading for the router rip interface summary */

  ewsTelnetWriteAddBlanks (1, 0, 18, 1, L7_NULLPTR, ewsContext,pStrInfo_common_Grp_1);
  ewsTelnetWriteAddBlanks (1, 0, 3, 1, L7_NULLPTR, ewsContext,pStrInfo_base_GrpNameProtoSIdVlanIntfS);

  ewsTelnetWrite(ewsContext, "\r\n----------------  ----  ----  ---------------------  ------------------------");

  while ((rc == L7_SUCCESS)&&(numLines < (CLI_MAX_SCROLL_LINES-6)))
  {
    countFlag = L7_TRUE;
    displayProto = L7_FALSE;

    if(fullLine == L7_TRUE)
    {
      memset(buf, 0x00, sizeof(buf));
      if ((usmDbPbVlanGroupNameGet(unit, groupID, group_name)) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%-18s", group_name);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%-18s", pStrInfo_common_Blank);
      }
      ewsTelnetWrite(ewsContext, buf);

      /* increment the line and set corresponding flag,
         so that others will not increment the count */
      countFlag = L7_FALSE;
      numLines++;


      memset(buf, 0x00, sizeof(buf));
      ewsTelnetPrintf (ewsContext, "%-6d", groupID);

      memset(buf, 0x00, sizeof(buf));
      if ((usmDbPbVlanGroupVIDGet(unit, groupID, &vid)) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%-6d", vid);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "%-6s", pStrInfo_common_Blank);
      }
      ewsTelnetWrite(ewsContext, buf);

      if(groupFlag == L7_TRUE)
      {
        memset(protocol, 0x00, L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS);
        memset(protocolList, 0x00, sizeof(protocolList));
        commaFlag = L7_FALSE;

        if ((usmDbPbVlanGroupProtocolGet(unit, groupID, protocol, type)) == L7_SUCCESS)
        {
          for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
          {
            memset(buf, 0x00, sizeof(buf));
            if(protocol[i] != L7_PBVLAN_INVALID_PROTOCOL)
            {
              if(type[i] == 0)
              {
                if(protocol[i] == L7_USMDB_PROTOCOL_HEX_IP)
                {
                  OSAPI_STRNCAT(buf, pStrInfo_common_Ip);
                }
                else if(protocol[i] == L7_USMDB_PROTOCOL_HEX_ARP)
                {
                  OSAPI_STRNCAT(buf, pStrInfo_common_Arp); 
                }
                else if(protocol[i] == L7_USMDB_PROTOCOL_HEX_IPX)
                {
                  OSAPI_STRNCAT(buf, pStrInfo_common_Ipx);
                }
              }
              else
              {
                osapiSnprintf(buf, sizeof(buf), "0x%4.4x", protocol[i]);
              }
              if(commaFlag == L7_TRUE)
              {
                OSAPI_STRNCAT(protocolList, ",");
                OSAPI_STRNCAT(protocolList, buf);
              }
              else
              {
                OSAPI_STRNCAT(protocolList, buf);
                commaFlag = L7_TRUE;
              }
            }
          }

          /*Protocol list is not zero, then go ahead and print the list */
          if(strlen(protocolList)!= 0)
          {
            memset(protoDisplay, 0x00, sizeof(protoDisplay));
            osapiSnprintf(protoDisplay, sizeof(protoDisplay), "%s", cliProcessStringOutput(protocolList, 21));
            printProto = L7_TRUE;
          }
          else
          {
            printProto = L7_FALSE;
          }
        }
        else
        {
          memset(protocolList, 0x0, sizeof(protocolList));
          printProto = L7_FALSE;
        }
      }

      if(groupFlag == L7_TRUE)
      {
        intIfNum = 0;
        memset(stat, 0x00, sizeof(stat));
        memset(interfaceList, 0x00, sizeof(interfaceList));
        commaFlag = L7_FALSE;
 
        while (usmDbPbVlanGroupPortGetNext(unit, groupID, intIfNum, &intIfNum) == L7_SUCCESS)
        {
          if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
          {
            osapiSnprintf(stat, sizeof(stat), "%s", cliDisplayInterfaceHelp(u, s, p));
            if (commaFlag == L7_TRUE)
              OSAPI_STRNCAT(interfaceList, ",");

            commaFlag = L7_TRUE;
            OSAPI_STRNCAT(interfaceList, stat);
          }
          memset(stat, 0x00, sizeof(stat));
        }

        if(strlen(interfaceList)!= 0)
        {
          memset(intfDisplay, 0x00, sizeof(intfDisplay));
          osapiSnprintf(intfDisplay, sizeof(intfDisplay), "%s", cliProcessStringSeparate(interfaceList, 25));
          printIntf= L7_TRUE;
        }
        else
        {
          memset(interfaceList, 0x0, sizeof(interfaceList));
          printIntf= L7_FALSE;
        }
      }

    }

    if(printProto == L7_TRUE)
    {
      if(fullLine == L7_FALSE)
      {
        memset(stat, 0x0, sizeof(stat));
        ewsTelnetPrintf (ewsContext, "\r\n%-30s", " ");
      }

      memset(stat, 0x0, sizeof(stat));
      ewsTelnetPrintf (ewsContext, "%-23s", protoDisplay);
      
      displayProto = L7_TRUE;

      /* increment the line and set corresponding flag,
         so that others will not increment the count */
      if(countFlag == L7_TRUE)
      {
        countFlag = L7_FALSE;
        numLines++;
      }

      memset(protoDisplay, 0x0, sizeof(protoDisplay));
      osapiSnprintf(protoDisplay, sizeof(protoDisplay), "%s", cliProcessStringOutput(protocolList, 21));
      if(strcmp(protocolList,protoDisplay) !=0)
      {
        /*Still there are protocol list display, so reset the corresponding lines */
        fullLine = L7_FALSE;
        groupFlag = L7_FALSE;
        printProto = L7_TRUE;
        rc = L7_SUCCESS;
      }
      else
      {
        groupFlag = L7_FALSE;
        printProto = L7_FALSE;
      }
    }

    if(printIntf== L7_TRUE)
    {
      if(displayProto == L7_TRUE)
      {
        memset(temp, 0x0, sizeof(temp));
        ewsTelnetPrintf (ewsContext, "%-25s", intfDisplay);
      }
      else
      {
        memset(stat, 0x0, sizeof(stat));

        if(fullLine == L7_FALSE)
        {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-53s", " ");
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-23s", " ");
        }
        ewsTelnetWrite(ewsContext, stat);

        memset(temp, 0x0, sizeof(temp));
        ewsTelnetPrintf (ewsContext, "%-25s", intfDisplay);
      }

      /* increment the line and set corresponding flag,
         so that others will not increment the count */
      if(countFlag == L7_TRUE)
      {
        countFlag = L7_FALSE;
        numLines++;
      }
      memset(intfDisplay, 0x0, sizeof(intfDisplay));
      osapiSnprintf(intfDisplay, sizeof(intfDisplay), "%s", cliProcessStringSeparate(interfaceList, 25));
      if(strcmp(interfaceList, intfDisplay) !=0)
      {
        /*There are some more interfaces to be displayed thats why resetting the corresponding flags*/
        fullLine = L7_FALSE;
        groupFlag = L7_FALSE;
        printIntf= L7_TRUE;
        rc = L7_SUCCESS;
        /*If there are interfaces to be displayed then we have to continue */
        continue;
      }
      else
      {
        groupFlag = L7_FALSE;
        printIntf= L7_FALSE;

        if(printProto == L7_TRUE)
        {
          continue;
        }
      }
    }

    /*If there are protocols to be displayed, then we havee to continue */
    if(printProto == L7_TRUE)
    {
      continue;
    }


    if (all == L7_TRUE)
    {
      rc = usmDbPbVlanGroupGetNext(unit, groupID, &nextGroupID);

      if(rc == L7_SUCCESS)
        groupID = nextGroupID;

      fullLine = L7_TRUE;
      groupFlag = L7_TRUE;
      printProto = L7_FALSE;
      printIntf = L7_FALSE;
    }
    else
    {
      groupID = 0;
      nextGroupID = 0;
      all = L7_FALSE;
      rc = L7_FAILURE;
      groupFlag = L7_TRUE;
      printProto = L7_TRUE;
      printIntf = L7_TRUE;
      fullLine = L7_TRUE;

      memset(intfDisplay, 0x00, sizeof(intfDisplay));
      memset(interfaceList, 0x00, sizeof(interfaceList));
      memset(protoDisplay, 0x00, sizeof(protoDisplay));
      memset(protocolList, 0x00, sizeof(protocolList));

      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  if ((rc == L7_SUCCESS)&&(numLines >= (CLI_MAX_SCROLL_LINES-6)))
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), "%s", argv[0]);
    for (i=1; i<argc; i++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[i]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }

  groupID = 0;
  nextGroupID = 0;
  all = L7_FALSE;
  rc = L7_FAILURE;
  groupFlag = L7_TRUE;
  printProto= L7_TRUE;
  printIntf = L7_TRUE;
  fullLine = L7_TRUE;

  memset(intfDisplay, 0x00, sizeof(intfDisplay));
  memset(interfaceList, 0x00, sizeof(interfaceList));
  memset(protoDisplay, 0x00, sizeof(protoDisplay));
  memset(protocolList, 0x00, sizeof(protocolList));

  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliSyntaxReturnPrompt (ewsContext, "");

}

