/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
 * @filename src/mgmt/cli/qos/iscsi/cli_show_iscsi.c
*
* @purpose iscsi show commands for the cli
*
* @component user interface
*
* @comments
*
* @create  11/24/2008
*
* @author  Rajakrishna
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "l7_common.h"
#include "cos_exports.h"
#include "iscsi_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "cliapi.h"

#include <datatypes.h>
#include "default_cnfgr.h"
#include "clicommands_card.h"
#include "clicommands_iscsi.h"
#include "cliutil.h"
#include "ews.h"
#include "usmdb_qos_iscsi.h"
#include "strlib_qos_cli.h"
#include "l7_productconfig.h"
extern void iscsiDebugBuffToStr(L7_uchar8 *text, L7_uchar8 *buffer, L7_uint32 bufferLength);
extern const L7_uchar8 *monthsOfYear[];
/*********************************************************************
*
* @purpose  Display Iscsi Parameters.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking, replace help string
*
* @cmdsyntax  show iscsi
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIscsi(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 **argv,
                                    L7_uint32 index)
{
  L7_uint32 numArgs = 0,count = 0;
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 mode;
  L7_char8  strAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 cosVal;
  L7_QOS_COS_MAP_INTF_MODE_t cosType;
  L7_uint32 remark;
  L7_uint32 agingTime;
  L7_uint32 maxSessions=0;
  L7_uint32 tcpPort;
  L7_inet_addr_t targetIpAddress;
  L7_char8 targetName[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32 entryId=0;
  L7_char8 checkChar;
  L7_uint32 charInputId=0;
 
  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);
  numArgs = cliNumFunctionArgsGet();
  
  if( numArgs != 0 )
  {
    osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiShow);
    ewsTelnetWrite(ewsContext, buf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  charInputId = cliGetCharInputID();
  if (charInputId != CLI_INPUT_EMPTY ) /* Not first pass */
  {
    checkChar = tolower(cliGetCharInput());
    if ((checkChar == 'q' ) ||
        (checkChar == 26 )) /*ctrl-z*/
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  if (charInputId == CLI_INPUT_EMPTY ) /* First pass */
  {
    ewsTelnetWrite(ewsContext,pStrInfo_qos_show_Iscsi);
    if(usmDbIscsiAdminModeGet(&mode)!=L7_SUCCESS)
    {
      osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiAdminModeGet);
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if(mode == L7_ENABLE)
    {
      ewsTelnetWrite(ewsContext,pStrInfo_qos_show_Iscsi_enable);
    }
    else
    {
      ewsTelnetWrite(ewsContext,pStrInfo_qos_show_Iscsi_disable);
    }
    count++;
  }
  
    /* cos info */
    if (charInputId == CLI_INPUT_EMPTY ) /* First pass */
    {
      ewsTelnetWrite(ewsContext,pStrInfo_qos_show_Iscsi_1);
      
      if(usmDbIscsiTagFieldGet(&cosType)!=L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiPRECValueGet);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext); 
      }
      if(cosType == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiVpt);
        if(usmDbIscsiVlanPriorityGet(&cosVal)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiVlanPriorityGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        ewsTelnetPrintf (ewsContext, "%d",cosVal);
      }
      else if(cosType == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiDscp);
        if(usmDbIscsiDscpGet(&cosVal)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiDscpGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        ewsTelnetPrintf (ewsContext, "%d",cosVal);
      }
      if(usmDbIscsiMarkingModeGet(&remark)!=L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiRemarkModeGet);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext); 
      }
      if(remark == L7_ENABLE)
      {
        ewsTelnetWrite(ewsContext,pStrInfo_qos_show_IscsiRemark);
      }
      if(usmDbIscsiTimeOutIntervalGet(&agingTime)!=L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiAgingOutTimeGet);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext); 
      }
      ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiAgingTime);
      ewsTelnetPrintf (ewsContext, "%d min",(agingTime/60));
  
      /* Maximum Number of Sessions Supported */
      ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiSessions);
      maxSessions = L7_ISCSI_MAX_SESSIONS; 
      ewsTelnetPrintf (ewsContext, "%d",maxSessions);
      /* iSCSI targets and TCP ports */
      ewsTelnetWrite(ewsContext, "\r\n------------------------------------------------");
      ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiTargetInfo);
      ewsTelnetWrite(ewsContext, "\r\n------------------------------------------------");
      ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiTargetInfo_1);
      count +=7;
      rc = usmDbIscsiTargetTableFirstGet(&entryId, L7_FALSE);
    }
    while(rc == L7_SUCCESS)
    {
      if(usmDbIscsiTargetTableEntryInUseStatusGet(entryId) == L7_TRUE)
      {
        if(usmDbIscsiTargetTableEntryTcpPortGet(entryId,&tcpPort)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiTargetTcpPortGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        ewsTelnetPrintf (ewsContext, "\r\n%-12d",tcpPort);
        if(usmDbIscsiTargetTableEntryIpAddressGet(entryId,&targetIpAddress)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiTargetIpAddressGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        if((targetIpAddress.family == L7_AF_INET) && (!targetIpAddress.addr.ipv4.s_addr))
        {
          ewsTelnetPrintf (ewsContext, "%-21s",pStrInfo_qos_show_IscsiNotConfig);
        }
        else
        {
          inetAddrHtop(&targetIpAddress, strAddr);
          ewsTelnetPrintf (ewsContext, "%-21s",strAddr);
        }
        if(usmDbIscsiTargetTableEntryTargetNameGet(entryId,targetName)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiTargetNameGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        if(*targetName == '\0')
        {
          ewsTelnetPrintf (ewsContext, "%s",pStrInfo_qos_show_IscsiNotConfig);
        }
        else
        {
          ewsTelnetPrintf (ewsContext, "%s",targetName);
        }

      }
      count++;
      rc = usmDbIscsiTargetTableNextGet(entryId, &entryId, L7_FALSE);
      if (rc != L7_SUCCESS)
      {
        break;
      }
      else if(count >= CLI_MAX_SCROLL_LINES-4)
      {
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
        cliAlternateCommandSet(pStrInfo_qos_IscsiShow);
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }
    }
    if(rc == L7_ERROR)
    {
      osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiTargetTableEntryIdGet);
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
  
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Display Iscsi sesssion Parameters.
*           Also Dispaly in Detailed Form.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking, replace help string
*
* @cmdsyntax  show iscsi sessions [detailed]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIscsiSessions(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 **argv,
                                    L7_uint32 index)
{
  static L7_uint32 numArgs=0;
  L7_uint32 count=0;
  L7_RC_t   rc = L7_SUCCESS;
  L7_char8  strAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 agingTime;
  L7_uint32 targetTcpPort;
  L7_uint32 initiatorTcpPort;
  L7_inet_addr_t targetIpAddr;
  L7_inet_addr_t initiatorIpAddr;
  L7_char8 targetName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 initiatorName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 iSID[6];
  L7_uchar8 isid_string[(sizeof(iSID)+1)];
  static L7_uint32 sessionId=0;
  static L7_uint32 connectionId=0;
  L7_clocktime startTime, timeVal;
  L7_uint32 silentTime;
  L7_timespec ts;
  L7_char8 checkChar;
  L7_uint32 charInputId = 0;
  static L7_char8 sessionFlag=L7_TRUE;

  cliSyntaxTop(ewsContext);

  cliCmdScrollSet( L7_FALSE);
  charInputId = cliGetCharInputID();
  if (charInputId == CLI_INPUT_EMPTY ) /* First pass */
  {
    numArgs = cliNumFunctionArgsGet();
    count = 0;
    sessionFlag=L7_TRUE;
    sessionId = 0;
    connectionId = 0;
  }
  
  if( numArgs < 0 )
  {
    osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiSessionShow);
    ewsTelnetWrite(ewsContext, buf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (charInputId != CLI_INPUT_EMPTY ) /* Not first pass */
  {
    checkChar = tolower(cliGetCharInput());
    if ((checkChar == 'q' ) ||
        (checkChar == 26 )) /*ctrl-z*/
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  /* Getting First Session ID */
  if (charInputId == CLI_INPUT_EMPTY ) /* First pass */
  {
    rc = usmDbIscsiSessionFirstGet(&sessionId);
  }

  while(rc == L7_SUCCESS)
  {
    if(sessionFlag  == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext, "\r\n\n");
      ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiSession_1);
      ewsTelnetPrintf(ewsContext, "%d:",sessionId);
      ewsTelnetWrite(ewsContext, "\r\n-----------------------------------------------------------------------------");
      if(usmDbIscsiSessionTargetNameGet(sessionId,targetName)!=L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiSessionTargetNameGet);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext); 
      }
      ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiTarget);
      ewsTelnetPrintf(ewsContext, "%s",targetName);  
      if(usmDbIscsiSessionInitiatorNameGet(sessionId,initiatorName)!=L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiSessionInitiatorNameGet);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext); 
      }
      ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiInitiator);
      ewsTelnetPrintf(ewsContext, "%s",initiatorName);
      count += 6;
      if(numArgs == 1)
      {
        if(usmDbIscsiSessionStartTimeGet(sessionId,&startTime)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiSessionStartTimeGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        osapiClockTimeRaw(&timeVal);
        timeVal.seconds = timeVal.seconds - startTime.seconds;
        osapiConvertRawUpTime(timeVal.seconds, &ts);
        ewsTelnetWrite(ewsContext, pStrInfo_qos_show_Iscsitime);
        memset(buf, 0x00, sizeof(buf));
        sprintf(buf, "%02d:%02d:%02d:%02d (DD:HH:MM:SS)",
                           ts.days, ts.hours, ts.minutes, ts.seconds);
        ewsTelnetWrite(ewsContext, buf);
        memset(buf, 0x00, sizeof(buf));

        if(usmDbIscsiSessionSilentTimeGet(sessionId,&silentTime)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiSessionSilentTimeGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        if(usmDbIscsiTimeOutIntervalGet((L7_uint32 *)&agingTime)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiSessionAgingOutTimeGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsitimeAgingOut);
        ewsTelnetPrintf(ewsContext, "%d secs",(agingTime - silentTime));   
        count +=2;
      }
      memset(iSID,0,sizeof(iSID));
      memset(isid_string,0,sizeof(isid_string));
      if(usmDbIscsiSessionIsidGet(sessionId,iSID)!=L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiSessionISIDGet);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext); 
      }
      iscsiDebugBuffToStr(isid_string, iSID, sizeof(iSID));
      ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsitIsid);
      ewsTelnetPrintf(ewsContext, "%s",isid_string);
      count += 1;
    }
    if(numArgs == 1)
    {
      if(sessionFlag == L7_TRUE)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiSessionDetail_1);
        ewsTelnetWrite(ewsContext, pStrInfo_qos_show_IscsiSessionDetail_2);
        count += 2;
        connectionId = 0;
        rc = usmDbIscsiConnectionFirstGet(sessionId,&connectionId);
      }
      while(rc == L7_SUCCESS)
      {
        if(usmDbIscsiConnectionInitiatorIpAddressGet(connectionId,&initiatorIpAddr)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiConnectionInitiatorIpAddressGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        memset(strAddr,0,sizeof(strAddr));
        inetAddrHtop(&initiatorIpAddr, strAddr);
        ewsTelnetPrintf (ewsContext, "\r\n%-17s",strAddr);
        if(usmDbIscsiConnectionInitiatorTcpPortGet(connectionId,&initiatorTcpPort)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiConnectionInitiatorTcpPortGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        ewsTelnetPrintf (ewsContext, "%-11u",initiatorTcpPort);
        if(usmDbIscsiConnectionTargetIpAddressGet(connectionId,&targetIpAddr)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiConnectionTargetIpAddressGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        memset(strAddr,0,sizeof(strAddr));
        inetAddrHtop(&targetIpAddr, strAddr);
        ewsTelnetPrintf (ewsContext, "%-17s",strAddr);
        if(usmDbIscsiConnectionTargetTcpPortGet(connectionId,&targetTcpPort)!=L7_SUCCESS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiConnectionTargetTcpPortGet);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext); 
        }
        ewsTelnetPrintf (ewsContext, "%-11u",targetTcpPort);
        count += 1;
        rc = usmDbIscsiConnectionNextGet(sessionId,connectionId, &connectionId);
        if (rc != L7_SUCCESS)
        {
          break;
        }
        else if(count >= CLI_MAX_SCROLL_LINES-4)
        {
          sessionFlag = L7_FALSE;
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          cliSyntaxBottom(ewsContext);
          cliAlternateCommandSet(pStrInfo_qos_IscsiSessionShow);
          return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
        }
      }/*End-of-Inner-While-Loop*/
      if(rc == L7_ERROR)
      {
        osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiConnectionIdGet);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext); 
      }
    }    
    rc = usmDbIscsiSessionNextGet(sessionId, &sessionId);
    sessionFlag = L7_TRUE;
    if (rc != L7_SUCCESS)
    {
      break;
    } 
    else if(count >= CLI_MAX_SCROLL_LINES-4)
    {
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliSyntaxBottom(ewsContext);
      cliAlternateCommandSet(pStrInfo_qos_IscsiSessionShow);
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
    }
  }
  if(rc == L7_ERROR)
  {
    osapiSnprintf(buf,sizeof(buf),pStrErr_qos_IscsiSessionIdGet);
    ewsTelnetWrite(ewsContext, buf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext); 
  }
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}


