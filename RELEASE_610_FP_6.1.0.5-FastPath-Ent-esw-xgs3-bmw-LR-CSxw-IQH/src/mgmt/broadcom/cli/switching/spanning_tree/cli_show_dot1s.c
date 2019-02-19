 /*********************************************************************
 * 
 * (C) Copyright Broadcom Corporation 2002-2006
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/dot1s/cli_show_dot1s.c
 *
 * @purpose Spanning Tree show commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  10/11/2002
 *
 * @author  Jill Flanagan
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"
#include "dot1q_exports.h"
#include "dot1s_exports.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_nim_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_dot1s.h"
#include "clicommands_card.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_util_api.h"

#include "cliutil_dot1s.h"
#include "cliutil.h"
#include "ews.h"

/*********************************************************************
 *
 * @purpose  display information about a specific port in a mst
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
 * @notes add range checking
 *
 * @cmdsyntax  show spanning-tree mst port summary <mstid> <slot/port | all>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandShowSpanningTreeMstPortSummary(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argMstid = 1;
  L7_uint32 argSlotPort = 2;
  L7_uint32 mstid;
  L7_char8 commandString[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;        /* New variable Added */

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 2 )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowDot1sMstPortSummary_1, cliSyntaxInterfaceHelp());
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index+argMstid],&mstid) == L7_SUCCESS)
  {
    if (mstid == DOT1S_CIST_ID)
    {
      /* Invoke function for CST */
      OSAPI_STRNCPY_SAFE( commandString, pStrInfo_switching_ShowSpantreeMstPortSummary );
      return cliShowMSTPortSummary( ewsContext, argc, argv, index,
          L7_DOT1S_CIST_INSTANCE, argSlotPort,
          commandString, sizeof(commandString),
          argv[index+argMstid] /* mstid is DOT1S_CIST_ID */) ;
    }
    else
    {
      if (mstid > L7_DOT1S_MSTID_MAX || mstid < L7_DOT1S_MSTID_MIN)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
  }

  OSAPI_STRNCPY_SAFE( commandString, pStrInfo_switching_ShowSpantreeMstPortSummary );
  return cliShowMSTPortSummary( ewsContext, argc, argv, index,
      mstid, argSlotPort,
      commandString, sizeof(commandString),
      argv[index+argMstid] );

}

/*********************************************************************
 *
 * @purpose  display port information for the specified spanning tree instance
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 * @param L7_uint32 spanning tree instance id
 * @param L7_uint32 parameter number for <slot.port/all>
 * @param L7_char8* commandString if loop required
 * @param L7_uint32 commandStringSize is size of commandString buffer
 *
 * @returns const L7_char8  *
 *
 * @notes Invoked when the following commands are issued:
 *               show spanning-tree mst port summary <mstid> <slot.port/all>
 *
 *********************************************************************/
const L7_char8 *cliShowMSTPortSummary(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index,
    L7_uint32 instance, L7_uint32 argSlotPort,
    L7_char8 * commandString, L7_uint32 commandStringSize,
    const L7_char8 * cmdParm )
{
  L7_RC_t rc;
  L7_char8 stat[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 u = 1;
  L7_int32 s;
  L7_int32 p;
  L7_int32 slot;
  L7_int32 port;
  static L7_uint32 interface;
  L7_uint32 nextInterface;
  L7_uint32 val;
  L7_int32 retVal;
  static L7_BOOL all;
  L7_BOOL boolVal;
  L7_uint32 numWrites = 0;
  L7_uint32 unit;
  usmDbTimeSpec_t ts;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY ) /* Not first pass */
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (strlen(argv[index+argSlotPort]) >= sizeof(strSlotPort))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ShowDot1sPortSummary);
    }

    OSAPI_STRNCPY_SAFE(strSlotPort, argv[index+argSlotPort] );
    cliConvertToLowerCase(strSlotPort);

    /* Instance Displayed at top */
    if (usmDbDot1sMstiTimeSinceTopologyChangeGet(unit, instance,
          &ts) != L7_SUCCESS)
    {
      /* This routine only fails if the specified instance is not in use.  Use
         this to verify a valid instance ID was provided. */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_switching_Dot1sInvalidMstId);
    }
    if ( strcmp(strSlotPort, pStrInfo_common_All   /* "all" */)==0 )
    {
      all = L7_TRUE;
      interface = 1;
    }
    else
    {
      all = L7_FALSE;

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
        if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS ||
            cliValidateDot1sInterface(ewsContext, unit, interface) != L7_SUCCESS)
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
        if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &interface) != L7_SUCCESS ||
            cliValidateDot1sInterface(ewsContext, unit, interface ) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }

      memset (stat, 0,sizeof(stat));
      cliFormat(ewsContext,pStrInfo_switching_MstInstId);   /* "MST Instance ID" */
      if (instance == L7_DOT1S_CIST_INSTANCE)
      {
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Cst);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%d", instance );
      }

      ewsTelnetWrite(ewsContext,stat);
      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

    }

  } /* end first pass setup */

  /* Print headers on each page */

  ewsTelnetWriteAddBlanks (1, 0, 12, 3, L7_NULLPTR, ewsContext,
      pStrInfo_switching_StpStpPort);
  ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,
      pStrInfo_switching_IntfModeTypeStateRole);
  ewsTelnetWrite(ewsContext,
      "\r\n--------- -------- ------- ----------------- ---------- ----------");

  while ( interface != 0 )
  {
    if ( usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS &&
        usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS &&
        cliValidateDot1sInterfaceMessage(ewsContext, unit, interface, L7_FALSE ) == L7_SUCCESS )
    {
      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      osapiSnprintf( stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
      ewsTelnetPrintf (ewsContext, "\r\n%-10s", stat );

      /* STP Mode */
      if ( usmDbDot1sPortStateGet(unit, interface, &boolVal) == L7_SUCCESS )
      {
        ewsTelnetPrintf (ewsContext, "%-8s ",strUtilEnabledDisabledGet(boolVal,pStrInfo_common_Dsbld));
      }
      else
      {
        ewsTelnetWrite(ewsContext,"         ");
      }

      /* Type */
      memset (stat, 0, sizeof(stat));
      rc = usmDbIfSpecialPortTypeGet(unit, interface, &val);
      switch ( val )
      {
        case L7_PORT_NORMAL:
          osapiSnprintf (stat, sizeof(stat), "%-8s", pStrInfo_common_EmptyString);  /* "       \0"*/
          break;
        case L7_TRUNK_MEMBER:
          osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_LagMbr); /* "PC Mbr\0" */
          break;
        case L7_MIRRORED_PORT:
          osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_MirrorShow); /* "Mirror\0" */
          break;
        case L7_PROBE_PORT:
          osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_Probe); /* "Probe\0" */
          break;
        default:
          osapiSnprintf (stat, sizeof(stat), "%-8s", pStrInfo_common_EmptyString); /* "       \0"*/
      }
      ewsTelnetWrite(ewsContext,stat);

      /* STP State */
      if ( cliSpanTreeGetPortFwdState( unit, instance,
            interface, stat, sizeof(stat) ) == L7_SUCCESS )
      {
        osapiSnprintf( buf, sizeof(buf), "%-17s ", stat );
      }
      else
      {
        osapiSnprintf( buf, sizeof(buf), "%-17s ", pStrInfo_common_NotApplicable);
      }                                                       /* N/A */
      ewsTelnetWrite(ewsContext,buf);

      /* Port Role */
      memset ( buf, 0, sizeof(buf) );
      if ( cliSpanTreeGetPortRole( unit, instance,
            interface, stat, sizeof(stat) ) == L7_SUCCESS )
      {
        ewsTelnetPrintf (ewsContext, "%-10s ", stat );
      }
      else
      {
        ewsTelnetWrite(ewsContext,"                ");
      }

      /* Description */
      memset ( buf, 0, sizeof(buf) );
      if ((usmDbDot1sMstiPortLoopInconsistentStateGet(unit, instance, interface, &val) == L7_SUCCESS) 
          && (val == L7_TRUE))
      {
        ewsTelnetPrintf (ewsContext, "%-10s ", pStrInfo_switching_LoopInc);
      }
      else
      {
        ewsTelnetWrite(ewsContext,"                ");
      }

      numWrites++;

    } /* end if visible */

    if ( !all )
    {
      interface = 0;
    }
    else
    {
      if ( usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS )
      {
        interface = nextInterface;
      }
      else
      {
        interface = 0;
      }
    }

    if (interface != 0 /* there is another interface */ &&
        numWrites >= 15 )
    {
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      if (cmdParm != L7_NULL)
      {
        osapiSnprintf( stat, sizeof(stat), " %s", cmdParm );
        osapiStrncat( commandString, stat, commandStringSize-strlen(commandString)-1 );
      }
      osapiStrncatAddBlanks (0, 0, 1, 0, L7_NULLPTR,  commandString, pStrInfo_common_All, commandStringSize-strlen(commandString)-1);
      cliAlternateCommandSet(commandString);
      return pStrInfo_common_Name_2;     /* --More-- or (q)uit */
    }

  } /* end of while  */

  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliPrompt(ewsContext);

}

/*********************************************************************
 *
 * @purpose  display port information for the bridge
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
 * @notes add range checking
 *
 * @cmdsyntax  show spanning-tree interface <slot/port>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandShowSpanningTreeInterface(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 slot, port, iface;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 unit;
  L7_RC_t rc;
  L7_uchar8 stat[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uint32 val;

  L7_BOOL boolVal;
  usmDbTimeSpec_t ts;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 1 )
  {
    return cliSyntaxReturnPromptAddBlanks (0, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
        pStrErr_switching_ShowSpanTreeIntf, cliSyntaxInterfaceHelp());
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
        cliValidateDot1sInterface(ewsContext, unit, iface) != L7_SUCCESS)
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
        cliValidateDot1sInterface(ewsContext, unit, iface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  /* Hello Time */
  if ( usmDbDot1sCistPortAdminHelloTimeGet(unit, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_HelloTime);   /* "Admin Hello Time" */
    memset (stat, 0, sizeof(stat));
    /*val==0 indicates that hello-time is not configured on that interface*/
    if(val!=0)
    {
      osapiSnprintf(stat, sizeof(stat), "%d", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_ApProfileNameNotCfgured);
    }
    ewsTelnetWrite(ewsContext, stat);
  }

  /* Port Mode */
  if ( usmDbDot1sPortStateGet(unit, iface, &boolVal) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_PortMode);   /* "Port Mode" */
    ewsTelnetWrite(ewsContext,strUtilEnabledDisabledGet(boolVal,pStrInfo_common_Dsbld));                                                  /* Disabled */
  }

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_BPDUGUARD) == L7_TRUE)
  {
      /* BPDU Guard Effect */
      if(usmDbDot1sBpduGuardEffectGet(unit,iface,&boolVal) == L7_SUCCESS )
      {
          cliFormat(ewsContext,pStrInfo_switching_PortBpduGuardEffect); /* "BPDU Guard Effect" */
          ewsTelnetWrite(ewsContext,strUtilEnabledDisabledGet(boolVal,pStrInfo_common_Dsbld));                                                /* Disabled */
      }
   }

  /* Root Guard status  */
  if ( usmDbDot1sPortRestrictedRoleGet(unit, iface, &boolVal) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_RootGuard); /* "Root Guard" */
    if (boolVal == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_True);
    }                                        /* True */
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_False);
    }                                         /* False */
  }

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_LOOPGUARD) == L7_TRUE)
  {
    /* Loop Guard status  */
    if ( usmDbDot1sPortLoopGuardGet(unit, iface, &boolVal) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_LoopGuard); /* "Loop Guard" */
      if (boolVal == L7_TRUE)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_True);
      }                                        /* True */
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_False);
      }                                         /* False */
    }
  }

  /* TCN Guard status  */
  if ( usmDbDot1sPortRestrictedTcnGet(unit, iface, &boolVal) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_TcnGuard); /* "TCN Guard" */
    if (boolVal == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_True);
    }                                        /* True */
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_False);
    }                                         /* False */
  }


  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_BPPDUFILTER) == L7_TRUE)
  {
      /* BPDU Filter Config */
      if(usmDbDot1sIntfBpduFilterGet(unit,iface,&boolVal) == L7_SUCCESS )
      {
          cliFormat(ewsContext,pStrInfo_switching_PortBpduFilterMode); /* "BPDU Filter Mode" */
          ewsTelnetWrite(ewsContext,strUtilEnabledDisabledGet(boolVal,pStrInfo_common_Dsbld));                                                /* Disabled */
      }
  }

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_BPPDUFILTER) == L7_TRUE)
  {
      /* BPDU Flood Config */
      if(usmDbDot1sIntfBpduFloodGet(unit,iface,&boolVal) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_PortBpdu); /* "BPDU Flood Mode" */
        ewsTelnetWrite(ewsContext,strUtilEnabledDisabledGet(boolVal,pStrInfo_common_Dsbld));                                                /* Disabled */
      }
  }

  /* Port Auto edge  */
  if ( usmDbDot1sPortAutoEdgeGet(unit, iface, &boolVal) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_AutoEdge); /* "Auto Edge" */
    if (boolVal == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_True);
    }                                        /* True */
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_False);
    }                                         /* False */
  }

  /* Port up time */
  if ( usmDbDot1sPortUpTimeGet(unit, iface, &ts) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_PortUpTimeSinceCountersLastClred);   /* "Port Up Time" */
    ewsTelnetPrintf (ewsContext, pStrErr_common_ShowTime,    /* "%d day %d hr %d min %d sec\0" */
        ts.days, ts.hours, ts.minutes, ts.seconds);
  }

  cliSpanTreeDisplayPortStats( ewsContext, iface );

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
 *
 * @purpose  display summary information for the Spanning Tree
 *
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
 * @notes add range checking
 *
 * @cmdsyntax  show spanning-tree
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandShowSpanningTree(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 val;
  L7_BOOL boolVal;
  L7_ushort16 val16;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 stat[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uint32 rc;
  L7_uint32 displayLines = 0;
  L7_uint32 unit;
  usmDbTimeSpec_t ts;
  L7_uint32 numArg;
  static L7_BOOL firstPassThisInstance;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  numArg = cliNumFunctionArgsGet();

  if(numArg != 0)
  {
    memset (stat, 0, sizeof(stat));
    if(numArg == 1 && strcmp(argv[index+1],pStrInfo_switching_Mst_1)==0)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowSpanTreeMst,
          cliSyntaxInterfaceHelp(), cliSyntaxInterfaceHelp());
    }
    else if(numArg == 2 && strcmp(argv[index+2],pStrInfo_common_Port_4)==0)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowSpanTreeMstPort,
          cliSyntaxInterfaceHelp(), cliSyntaxInterfaceHelp());
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ShowSpanTreeBrief);
    }
  }

  if ( cliGetCharInputID() == CLI_INPUT_EMPTY ) /* First pass */
  {
    firstPassThisInstance = L7_TRUE;

    /* Bridge Priority */
    if ( usmDbDot1sMstiBridgePriorityGet(unit,
          L7_DOT1S_CIST_INSTANCE,
          &val16) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_ShowDot1sDetailedCst);
    }

    cliFormat(ewsContext,pStrInfo_switching_BridgePri); /* "Bridge Priority */
    ewsTelnetPrintf (ewsContext, "%u", val16 );

    /* Bridge Identifier */
    val = sizeof(buf);
    memset ( buf, 0, val );
    if ( usmDbDot1sMstiBridgeIdentifierGet(unit,
          L7_DOT1S_CIST_INSTANCE,
          buf,
          &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_BridgeId);   /* "Bridge Identifier" */
      memset ( stat, 0, sizeof(stat));
      cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
      ewsTelnetWrite(ewsContext, stat);
    }

    /* Time since topology change */
    if ( usmDbDot1sMstiTimeSinceTopologyChangeGet(unit,
          L7_DOT1S_CIST_INSTANCE,
          &ts) == L7_SUCCESS )
    {
      cliFormat(ewsContext,
          pStrInfo_switching_TimeSinceTopologyChg);   /* "Time Since Topology Change" */
      ewsTelnetPrintf (ewsContext, pStrErr_common_ShowTime,   /* "%d day %d hr %d min %d sec\0" */
          ts.days, ts.hours, ts.minutes, ts.seconds);
    }

    /* Topology change count */
    if ( usmDbDot1sMstiTopologyChangeCountGet(unit, L7_DOT1S_CIST_INSTANCE, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,
          pStrInfo_switching_TopologyChgCount);   /* "Topology Change Count" */
      ewsTelnetPrintf (ewsContext, "%u", val );
      displayLines++;
    }

    /* Topology Change in progress */
    if ( usmDbDot1sMstiTopologyChangeParmGet(unit,
          L7_DOT1S_CIST_INSTANCE,
          &boolVal) == L7_SUCCESS )
    {
      cliFormat(ewsContext,
          pStrInfo_switching_TopologyChgInProgress);   /* "Topology Change in progress" */
      if (boolVal == L7_TRUE)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_True);
      }                                          /* "True" */
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_False);
      }                                           /* "False" */
      displayLines++;
    }

    /* Designated Root */
    val = sizeof(buf);
    memset (buf, 0, val);
    if ( usmDbDot1sMstiDesignatedRootIDGet(unit,
          L7_DOT1S_CIST_INSTANCE,
          buf,
          &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_DesignatedRoot);   /* "Designated Root" */
      memset ( stat, 0, sizeof(stat));
      cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
      ewsTelnetWrite(ewsContext, stat);
      displayLines++;
    }

    /* Root Path Cost */
    if ( usmDbDot1sMstiRootPathCostGet(unit,
          L7_DOT1S_CIST_INSTANCE,
          &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_DesignatedPortCost);   /* "Root Path Cost" */
      ewsTelnetPrintf (ewsContext, "%u", val );
      displayLines++;
    }

    /* Root Port ID  */
    if ( usmDbDot1sMstiRootPortIDGet(unit,
          L7_DOT1S_CIST_INSTANCE,
          &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,
          pStrInfo_switching_RootPortId);   /* "Root Port Identifier" */
      cliSpanTreeFormatPortIdentifier( buf, sizeof(buf), val );
      ewsTelnetWrite(ewsContext, buf);
      displayLines++;
    }

    /* Cist Root Max Age  */
    if ( usmDbDot1sCistMaxAgeGet(unit, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_BridgeMaxAge);   /* "Bridge Max Age" */
      ewsTelnetPrintf (ewsContext, "%u", val );
      displayLines++;
    }

    /* Max Hop Count */
    if ( usmDbDot1sBridgeMaxHopGet(unit, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext, pStrInfo_switching_CstMaxHop);   /* "Max Hop Count" */
      ewsTelnetPrintf (ewsContext, "%d", val );
      displayLines++;
    }

    /* Max Hop Count */
    if ( usmDbDot1sBridgeTxHoldCountGet(unit, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext, pStrInfo_switching_CstTxHoldCount);           /* "Max Hop Count" */
      ewsTelnetPrintf (ewsContext, "%d", val );
      displayLines++;
    }

    /* Root Port Bridge Forward Delay  */
    if ( usmDbDot1sCistRootFwdDelayGet(unit, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,
          pStrInfo_switching_BridgeForwardDelay_1);   /* "Bridge Forwarding Delay" */
      ewsTelnetPrintf (ewsContext, "%u", val );
      displayLines++;
    }

    /* CIST Hello time  */
    if ( usmDbDot1sCistHelloTimeGet(unit, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_HelloTime);   /* "Hello Time" */
      ewsTelnetPrintf (ewsContext, "%u", val );
      displayLines++;
    }

    /* CIST hold time  */
    if ( usmDbDot1sCistBridgeHoldTimeGet(unit, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_BridgeHoldTime);   /* "Bridge Hold Time" */
      ewsTelnetPrintf (ewsContext, "%u", val );
      displayLines++;
    }

    /* Regional Root id  */
    val = sizeof(buf);
    memset (buf, 0, val);
    if ( usmDbDot1sCistRegionalRootIDGet(unit, buf, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,
          pStrInfo_switching_CstRegionalRoot);   /* "CST Regional Root" */
      memset ( stat, 0, sizeof(stat));
      cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
      ewsTelnetWrite(ewsContext, stat);
      displayLines++;
    }

    /* Regional Root path cost  */
    if ( usmDbDot1sCistRegionalRootPathCostGet(unit, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,
          pStrInfo_switching_RegionalRootPathCost);   /* "Regional Root Path Cost" */
      ewsTelnetPrintf (ewsContext, "%u", val );
      displayLines++;
    }
  }
  else
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  rc = cliSpanTreeShowFidsAndVlans( ewsContext, L7_DOT1S_CIST_INSTANCE,
      (L7_CLI_MAX_LINES_PER_PAGE - displayLines),
      firstPassThisInstance );

  switch (rc)
  {
    case L7_FAILURE:
      /* Not enough lines to show heading.  Page and repeat */
      firstPassThisInstance = L7_FALSE;
      /* fallthrough */
    case L7_ERROR:
      /* paging required */
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliAlternateCommandSet(pStrInfo_switching_ShowSpantree);
      return pStrInfo_common_Name_2;        /* --More-- or (q)uit */
      /*PASSTHRU*/
    case L7_SUCCESS:
      /* All data for this instance displayed. Fallthrough */
    default:
      break;

  }
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  display summary information for the Spanning Tree Bridge properties
 *
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
 * @notes add range checking
 *
 * @cmdsyntax  show spanning-tree brief
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandShowSpanningTreeBrief(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 val;
  L7_ushort16 val16;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* Bridge Priority */
  if ( usmDbDot1sMstiBridgePriorityGet(unit,
        L7_DOT1S_CIST_INSTANCE,
        &val16) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_ShowDot1sSummaryBridge);
  }

  cliFormat(ewsContext, pStrInfo_switching_BridgePri); /* "Bridge Priority */
  ewsTelnetPrintf (ewsContext, "%u", val16 );

  /* Bridge Identifier */
  val = sizeof(buf);
  memset ( buf, 0, val );
  if ( usmDbDot1sMstiBridgeIdentifierGet(unit,
        L7_DOT1S_CIST_INSTANCE,
        buf,
        &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext, pStrInfo_switching_BridgeId);   /* "Bridge Identifier" */
    memset ( stat, 0, sizeof(stat));
    cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
    ewsTelnetWrite(ewsContext, stat);
  }

  /* Bridge Max Age */
  if ( usmDbDot1sCistBridgeMaxAgeGet(unit, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext, pStrInfo_switching_BridgeMaxAge);   /* "Bridge Max Age" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  /* Bridge Max Hop Count */
  if ( usmDbDot1sBridgeMaxHopGet(unit, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext, pStrInfo_switching_CstMaxHop);   /* "Bridge Max Hop" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  /* Bridge Hello Time */
  if ( usmDbDot1sCistBridgeHelloTimeGet(unit, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext, pStrErr_switching_BridgeHelloTime);   /* "Bridge Hello Time" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  /* Bridge Forward Delay  */
  if ( usmDbDot1sCistBridgeFwdDelayGet(unit, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext, pStrErr_switching_BridgeForwardDelay);   /* "Bridge Forward Delay" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  /* Bridge Hold Time  */
  if ( usmDbDot1sCistBridgeHoldTimeGet(unit, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext, pStrInfo_switching_BridgeHoldTime);   /* "Bridge Hold Time" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
 *
 * @purpose  display detailed information for the specified mstid
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
 * @notes add range checking
 *
 * @cmdsyntax  show spanningtree mst detailed <mstid>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandShowSpanningTreeMstDetailed(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argMstid = 1;
  static L7_uint32 mstid;
  L7_uint32 val;
  L7_BOOL boolVal;
  L7_ushort16 val16;
  L7_uint32 numArg;          /* New variable Added */

  L7_char8 buf[ L7_CLI_MAX_STRING_LENGTH ];
  L7_char8 stat[ L7_CLI_MAX_STRING_LENGTH ];
  L7_uint32 rc;
  L7_uint32 unit;
  usmDbTimeSpec_t ts;

  L7_uint32 displayLines = 0;

  static L7_BOOL firstPassThisInstance;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if ( cliGetCharInputID() == CLI_INPUT_EMPTY ) /* First pass */
  {
    firstPassThisInstance = L7_TRUE;
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowSpanTreeMstDetailed,
          L7_DOT1S_CIST_INSTANCE, L7_DOT1S_MSTID_MAX);
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argMstid],
          &mstid) != L7_SUCCESS ||
        mstid > L7_DOT1S_MSTID_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_CIST_INSTANCE, L7_DOT1S_MSTID_MAX);
    }

    /* Bridge Priority */
    if ( usmDbDot1sMstiBridgePriorityGet(unit, mstid, &val16) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_Dot1sInvalidMstId);
    }

    /* MST Instance ID */
    cliFormat(ewsContext,pStrInfo_switching_MstInstId);   /* "MST Instance ID" */
    ewsTelnetPrintf (ewsContext, "%u", mstid );
    displayLines++;

    cliFormat(ewsContext,pStrInfo_switching_MstBridgePri);   /* "MST Bridge Priority" */
    ewsTelnetPrintf (ewsContext, "%u", val16 );
    displayLines++;

    /* Bridge Identifer */
    val = sizeof(buf);
    memset (buf, 0, val);
    if ( usmDbDot1sMstiBridgeIdentifierGet(unit, mstid, buf, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_MstBridgeId);     /* "MST Bridge Identifier" */
      memset (stat, 0, sizeof(stat));
      cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
      ewsTelnetWrite(ewsContext, stat);
      displayLines++;
    }

    /* Time since topology change */
    if ( usmDbDot1sMstiTimeSinceTopologyChangeGet(unit, mstid,
          &ts) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_TimeSinceTopologyChg);   /* "Time Since Topology Change" */
      ewsTelnetPrintf (ewsContext, pStrErr_common_ShowTime,     /* "%d day %d hr %d min %d sec\0" */
          ts.days, ts.hours, ts.minutes, ts.seconds);
      displayLines++;
    }

    /* Topology change count */
    if ( usmDbDot1sMstiTopologyChangeCountGet(unit, mstid, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_TopologyChgCount);     /* "Topology Change Count" */
      ewsTelnetPrintf (ewsContext, "%u", val );
      displayLines++;
    }

    /* Topology change parm */
    if ( usmDbDot1sMstiTopologyChangeParmGet(unit, mstid, &boolVal) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_TopologyChgInProgress);     /* "Topology Change in progress" */
      if (boolVal == L7_TRUE)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_True);
      }                                            /* "True" */
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_False);
      }                                             /* "False" */
      displayLines++;
    }

    /* Designated Root */
    val = sizeof(buf);
    memset (buf, 0, val);
    if ( usmDbDot1sMstiDesignatedRootIDGet(unit, mstid, buf, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_DesignatedRoot);     /* "Designated Root" */
      memset (stat, 0, sizeof(stat) );
      cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
      ewsTelnetWrite(ewsContext, stat);
      displayLines++;
    }

    /* Root Path Cost */
    if ( usmDbDot1sMstiRootPathCostGet(unit, mstid, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_DesignatedPortCost);     /* "Root Path Cost" */
      ewsTelnetPrintf (ewsContext, "%u", val );
      displayLines++;
    }

    /* Root Port ID  */
    if ( usmDbDot1sMstiRootPortIDGet(unit, mstid, &val) == L7_SUCCESS )
    {
      cliFormat(ewsContext,pStrInfo_switching_RootPortId);     /* "Root Port Identifier" */
      cliSpanTreeFormatPortIdentifier( buf, sizeof(buf), val );
      ewsTelnetWrite(ewsContext, buf);
      displayLines++;
    }

  }
  else
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  rc = cliSpanTreeShowFidsAndVlans( ewsContext, mstid,
      (L7_CLI_MAX_LINES_PER_PAGE - displayLines),
      firstPassThisInstance );

  switch (rc)
  {
    case L7_FAILURE:
      /* Not enough lines to show heading.  Page and repeat */
      firstPassThisInstance = L7_FALSE;
      /* fallthrough */
    case L7_ERROR:
      /* paging required */
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      osapiSnprintf( buf, sizeof(buf), pStrInfo_switching_ShowSpantreeMstDetailed, argv[index+argMstid]);
      cliAlternateCommandSet(buf);
      return pStrInfo_common_Name_2;      /* --More-- or (q)uit */
      /*PASSTHRU*/
    case L7_SUCCESS:
      /* All data for this instance displayed. Fallthrough */
    default:
      break;

  }

  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliPrompt( ewsContext );
}

/*********************************************************************
 *
 * @purpose  display information about a specific port for a specific msti
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
 * @notes add range checking
 *
 * @cmdsyntax  show spanning-tree mst port detailed <mstid> <slot/port>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandShowSpanningTreeMstPortDetailed(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argMstid = 1;

  L7_uint32 argSlotPort = 2;
  L7_uint32 slot, port, iface;
  L7_uint32 unit;
  L7_RC_t rc;

  L7_uint32 mstid;
  L7_uint32 val;
  L7_ushort16 val16;
  L7_BOOL boolVal;
  L7_uint32 numArg;          /* New variable Added */

  L7_uchar8 buf[ L7_CLI_MAX_STRING_LENGTH ];
  L7_uchar8 stat[ L7_CLI_MAX_STRING_LENGTH ];

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 2 )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowDot1sMstPortDetailed_1, cliSyntaxInterfaceHelp());
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index+argMstid],&mstid) == L7_SUCCESS)
  {
    if (mstid == DOT1S_CIST_ID)
    {
      /* Functionality for CST */
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
            cliValidateDot1sInterface(ewsContext, unit, iface) != L7_SUCCESS)
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
            cliValidateDot1sInterface(ewsContext, unit, iface) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }

      /* Port ID */
      if ( usmDbDot1sMstiPortIDGet(unit,
            L7_DOT1S_CIST_INSTANCE,
            iface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_PortId_3); /* "Port Identifier" */
        cliSpanTreeFormatPortIdentifier( buf, sizeof(buf), val );
        ewsTelnetWrite(ewsContext, buf);
      }

      /* Port Priority */
      if ( usmDbDot1sMstiPortPriorityGet(unit,
            L7_DOT1S_CIST_INSTANCE,
            iface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_PortPri); /* "Port Priority" */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }

      /* Port forwarding state */
      memset ( buf, 0, sizeof(buf) );
      if ( cliSpanTreeGetPortFwdState( unit, L7_DOT1S_CIST_INSTANCE,
            iface, buf, sizeof(buf) ) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_PortFwdState);/* "Port Forwading State" */
        ewsTelnetWrite(ewsContext,buf);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }                                      /* N/A */

      /* Port Role */
      memset ( buf, 0, sizeof(buf) );
      if ( cliSpanTreeGetPortRole( unit, L7_DOT1S_CIST_INSTANCE,
            iface, buf, sizeof(buf) ) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_common_PortRole); /* "Port Role" */
        ewsTelnetWrite(ewsContext,buf);
      }

      /* Auto Port path cost enabled */
      if ( usmDbDot1sMstiPortPathCostModeGet(unit, L7_DOT1S_CIST_INSTANCE, iface, &boolVal) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_AutoCalculatePortPathCost); /* "Auto-calculate Port Path Cost" */
        ewsTelnetPrintf (ewsContext, "%-8s ",strUtilEnabledDisabledGet(boolVal,pStrInfo_common_Dsbld));
      }

      /* Port path cost */
      if ( usmDbDot1sMstiPortPathCostGet(unit,
            L7_DOT1S_CIST_INSTANCE,
            iface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_PortPathCost); /* "Port Path Cost" */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }
      /* Auto-Calculate External Port path cost */
      if ( usmDbDot1sCistExtPortPathCostModeGet(unit, iface) == L7_TRUE)
      {
        cliFormat(ewsContext,pStrInfo_switching_PortPathExtCostMode); /* "Auto-Calculate External Port Path Cost" */
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbld );
      }
      else
      {
        cliFormat(ewsContext,pStrInfo_switching_PortPathExtCostMode); /* "Auto-Calculate External Port Path Cost" */
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbld );
      }

      /* External Port path cost */
      if ( usmDbDot1sCistPortExternalPathCostGet(unit, iface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_PortPathExtCost); /* "External Port Path Cost" */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }

      /* Designated Root  */
      val = sizeof(buf);
      memset (buf, 0, val);
      if ( usmDbDot1sMstiPortDesignatedRootIDGet(unit,
            L7_DOT1S_CIST_INSTANCE,
            iface, buf, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_DesignatedRoot); /* "Designated Root" */
        memset ( stat, 0, sizeof(stat));
        cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
        ewsTelnetWrite(ewsContext, stat);
      }

      /* Designated port cost */
      if ( usmDbDot1sMstiDesignatedCostGet(unit,
            L7_DOT1S_CIST_INSTANCE,
            iface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_DesignatedPortCost); /* "Designated Port Cost" */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }

      /* Designated bridge  */
      val = sizeof(buf);
      memset (buf, 0, val);
      if ( usmDbDot1sMstiDesignatedBridgeIDGet(unit,
            L7_DOT1S_CIST_INSTANCE,
            iface, buf, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_DesignatedBridge); /* "Designated Bridge" */
        memset ( stat, 0, sizeof(stat));
        cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
        ewsTelnetWrite(ewsContext, stat);
      }

      /* Designated port ID */
      if ( usmDbDot1sMstiDesignatedPortIDGet(unit,
            L7_DOT1S_CIST_INSTANCE,
            iface, &val16) == L7_SUCCESS )
      {
        cliFormat(ewsContext,
            pStrInfo_switching_DesignatedPortId); /* "Designated Port Identifier" */
        cliSpanTreeFormatPortIdentifier( buf, sizeof(buf), val16 );
        ewsTelnetWrite(ewsContext, buf);
      }

      /* Topology change acknowledge  */
      if ( usmDbDot1sCistPortTopologyChangeAckGet(unit, iface, &boolVal) == L7_SUCCESS )
      {
        cliFormat(ewsContext,
            pStrInfo_switching_TopologyChgAck); /* "Topology Change Acknowledge" */
        if (boolVal == L7_TRUE)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_True);
        }                                        /* True */
        else
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_False);
        }                                         /* False */
      }
      /* Port Admin Hello Time */
      if ( usmDbDot1sCistPortAdminHelloTimeGet(unit,
            iface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_HelloTime); /* "Admin Hello Time" */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }

      /* Port edge  */
      if ( usmDbDot1sCistPortEdgeGet(unit, iface, &boolVal) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_EdgePort); /* "Edge Port" */
        if (boolVal == L7_TRUE)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_True);
        }                                        /* True */
        else
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_False);
        }                                         /* False */
      }

      /* Port operational edge  */
      if ( usmDbDot1sCistPortOperEdgeGet(unit, iface, &boolVal) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_EdgePortStatus); /* "Edge Port Status" */
        if (boolVal == L7_TRUE)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_True);
        }                                        /* True */
        else
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_False);
        }                                         /* False */
      }

      /* Point to point mac status  */
      if ( usmDbDot1sCistPortOperPointToPointGet(unit, iface, &boolVal) == L7_SUCCESS )
      {
        cliFormat(ewsContext,
            pStrInfo_switching_PointToPointMacStatus); /* "Point to Point MAC Status" */
        if (boolVal == L7_TRUE)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_True);
        }                                        /* True */
        else
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_False);
        }                                         /* False */
      }
      /* CIST Regional Root Identifier*/
      val = sizeof(buf);
      memset (buf, 0, val);
      if ( usmDbDot1sCistPortRegionalRootIDGet(unit,
            iface, buf, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_CstRegionalRoot); /* "CST Regional Root" */
        memset ( stat, 0, sizeof(stat));
        cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
        ewsTelnetWrite(ewsContext, stat);
      }
      /* CIST port cost */
      if ( usmDbDot1sCistPortPathCostGet(unit,
            iface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_CstPathCost); /* "CST Port Cost" */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }
      /* Port Loop Inconsistent State */
      if ( usmDbDot1sMstiPortLoopInconsistentStateGet(unit, L7_DOT1S_CIST_INSTANCE, 
            iface, &boolVal) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_LoopInconsistentState); /* "Port Loop Inconsistent State" */
        if (boolVal == L7_TRUE)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_True);
        }                                        /* True */
        else
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_False);
        }                                         /* False */
      }
      /* Port Loop Inconsistent State Start Counter */
      if ( usmDbDot1sPortStatsTransitionsIntoLoopInconsistentStateGet(unit, L7_DOT1S_CIST_INSTANCE,
            iface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_TransitionsIntoLoopInconsistentState); /* "Port Loop Inconsistent State Start Counter" */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }
      /* Port Loop Inconsistent State End Counter */
      if ( usmDbDot1sPortStatsTransitionsOutOfLoopInconsistentStateGet(unit, L7_DOT1S_CIST_INSTANCE,
            iface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext,pStrInfo_switching_TransitionsOutOfLoopInconsistentState); /* "Port Loop Inconsistent State End Counter" */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }
     
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else
    {
      if (mstid > L7_DOT1S_MSTID_MAX || mstid < L7_DOT1S_MSTID_MIN)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sMstId, L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
  }

  /* Functionality for the MST Instance */
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
        cliValidateDot1sInterface(ewsContext, unit, iface) != L7_SUCCESS)
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
        cliValidateDot1sInterface(ewsContext, unit, iface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  if ( usmDbDot1sMstiPortIDGet(unit, mstid, iface, &val) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_Dot1sInvalidMstId);
  }

  /* MST Instance ID */
  cliFormat(ewsContext,pStrInfo_switching_MstInstId); /* "MST Instance ID" */
  ewsTelnetPrintf (ewsContext, "%u", mstid );

  /* Port ID */
  cliFormat(ewsContext,pStrInfo_switching_PortId_3); /* "Port Identifier" */
  cliSpanTreeFormatPortIdentifier( buf, sizeof(buf), val );
  ewsTelnetWrite(ewsContext, buf);

  /* Port Priority */
  if ( usmDbDot1sMstiPortPriorityGet(unit, mstid, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_PortPri);   /* "Port Priority" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  /* Port forwarding state */
  memset ( buf, 0, sizeof(buf) );
  if ( cliSpanTreeGetPortFwdState( unit, mstid,
        iface, buf, sizeof(buf) ) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_PortFwdState);  /* "Port Forwading State" */
    ewsTelnetWrite(ewsContext,buf);
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
  }                                        /* N/A */

  /* Port Role */
  memset ( buf, 0, sizeof(buf) );
  if ( cliSpanTreeGetPortRole( unit, mstid, iface,
        buf, sizeof(buf) ) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_common_PortRole);  /* "Port Role" */
    ewsTelnetWrite(ewsContext,buf);
  }

  /* Auto Port path cost enabled */
  if ( usmDbDot1sMstiPortPathCostModeGet(unit, mstid, iface, &boolVal) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_AutoCalculatePortPathCost);   /* "Auto-calculate Port Path Cost" */
    ewsTelnetPrintf (ewsContext, "%-8s ",strUtilEnabledDisabledGet(boolVal,pStrInfo_common_Dsbld));
  }

  /* Port path cost */
  if ( usmDbDot1sMstiPortPathCostGet(unit, mstid, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_PortPathCost);   /* "Port Path Cost" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }
  /* Designated Root  */
  val = sizeof(buf);
  memset (buf, 0, val);
  if ( usmDbDot1sMstiPortDesignatedRootIDGet(unit,
        mstid,
        iface, buf, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_DesignatedRoot);   /* "Designated Root" */
    memset ( stat, 0, sizeof(stat));
    cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
    ewsTelnetWrite(ewsContext, stat);
  }
  /* Designated port cost */
  if ( usmDbDot1sMstiDesignatedCostGet(unit, mstid, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_DesignatedPortCost);   /* "Designated Port Cost" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  /* Designated bridge  */
  val  = sizeof(buf);
  memset (buf, 0, val);
  if ( usmDbDot1sMstiDesignatedBridgeIDGet(unit, mstid, iface,
        buf, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_DesignatedBridge);   /* "Designated Bridge" */
    memset ( stat, 0, sizeof(stat));
    cliSpanTreeFormatBridgeIdentifier(stat, sizeof(stat), buf);
    ewsTelnetWrite(ewsContext, stat);
  }

  /* Designated port ID */
  if ( usmDbDot1sMstiDesignatedPortIDGet(unit, mstid, iface, &val16) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_DesignatedPortId);   /* "Designated Port Identifier" */
    cliSpanTreeFormatPortIdentifier( buf, sizeof(buf), val16 );
    ewsTelnetWrite(ewsContext, buf);
  }

  /* Port Loop Inconsistent State */
  if ( usmDbDot1sMstiPortLoopInconsistentStateGet(unit, mstid, 
        iface, &boolVal) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_LoopInconsistentState); /* "Port Loop Inconsistent State" */
    if (boolVal == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_True);
    }                                        /* True */
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_False);
    }                                         /* False */
  }
  
  /* Port Loop Inconsistent State Start Counter */
  if ( usmDbDot1sPortStatsTransitionsIntoLoopInconsistentStateGet(unit, mstid,
        iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_TransitionsIntoLoopInconsistentState); /* "Port Loop Inconsistent State Start Counter" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }
  
  /* Port Loop Inconsistent State End Counter */
  if ( usmDbDot1sPortStatsTransitionsOutOfLoopInconsistentStateGet(unit, mstid,
        iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_switching_TransitionsOutOfLoopInconsistentState); /* "Port Loop Inconsistent State End Counter" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_CrLf);

}

/*********************************************************************
 *
 * @purpose  display summary information for all mst instances in the switch
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
 * @notes add range checking
 *
 * @cmdsyntax  show spanningtree mst summary
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandShowSpanningTreeMstSummary(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  static L7_uint32 mstid;
  static L7_BOOL firstPassThisInstance;
  static L7_BOOL instanceComplete;
  L7_uint32 count = 0;

  L7_uint32 displayLines;
  L7_uint32 rc;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY ) /* Not first pass */
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else  /* First pass setup */
  {
    firstPassThisInstance = L7_TRUE;
    instanceComplete = L7_TRUE;

    if ( numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_ShowSpanTreeMstSummary);
    }

    if ( usmDbDot1sInstanceFirstGet(unit, &mstid) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_Dot1sShowMstSummary);
    }
  }

  displayLines = 0;     /* counts number of lines displayed ...used for paging */

  /* The first instance is the cist.  Skip to the first msti */
  while (instanceComplete == L7_FALSE ||  /* output interrupted for paging */
      usmDbDot1sInstanceNextGet(unit, mstid, &mstid) == L7_SUCCESS )
  {
    if (instanceComplete == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      cliFormat(ewsContext,pStrInfo_switching_MstInstId);     /* "MST Instance ID" */
      ewsTelnetPrintf (ewsContext, "%u", mstid );
      displayLines+=2;
    }

    rc = cliSpanTreeShowFidsAndVlans( ewsContext, mstid,
        (L7_CLI_MAX_LINES_PER_PAGE - displayLines),
        firstPassThisInstance );

    switch (rc)
    {
      case L7_FAILURE:
        /* paging required */
        firstPassThisInstance = L7_FALSE;
        /* fallthrough */
      case L7_ERROR:
        /* Not enough lines to show heading.  Page and repeat */
        instanceComplete = L7_FALSE;     /* instance id already displayed */
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliAlternateCommandSet(pStrInfo_switching_ShowSpantreeMstSummary);
        return pStrInfo_common_Name_2;       /* --More-- or (q)uit */
        /*PASSTHRU*/
      case L7_SUCCESS:
        /* All data for this instance displayed. Fallthrough */
        instanceComplete = L7_TRUE;
        count++;       /* counts number of MSTs displayed */
        firstPassThisInstance = L7_TRUE;
      default:
        break;

    }

  }

  if (count == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_Dot1sShowMstNone);
  }

  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  display spanningtree summary information
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
 * @notes add range checking
 *
 * @cmdsyntax  show spanningtree summary
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandShowSpanningTreeSummary(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_BOOL mode;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 digest[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 key[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 version;
  L7_uint32 format;
  L7_uint32 revision;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_BOOL boolVal;

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
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_ShowDot1sSummary_2);
  }

  /* Spanning Tree Mode */
  if ( usmDbDot1sModeGet(unit, &mode) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_ShowDot1sSummary_1);
  }

  memset ( buf, 0, sizeof(buf) );
  if (mode == L7_ENABLE)
  {
    osapiSnprintf( buf, sizeof(buf), pStrInfo_common_Enbld);
  }                                                   /* Enabled */
  else if (mode == L7_DISABLE)
  {
    osapiSnprintf( buf, sizeof(buf), pStrInfo_common_Dsbld);
  }                                                      /* Disabled */
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_ShowDot1sSummary_1);
  }

  cliFormatShortText(ewsContext,pStrInfo_switching_SpanTreeAdminMode_1);    /* "Spanning Tree Mode\0" */
  ewsTelnetWrite(ewsContext, buf);

  /* Spanning Tree Specification/Version */
  if ( usmDbDot1sForceVersionGet(unit, &version) == L7_SUCCESS )
  {
    memset ( buf, 0, sizeof(buf) );
    if (version == DOT1S_FORCE_VERSION_DOT1D )
    {
      osapiSnprintf( buf, sizeof(buf), pStrInfo_switching_Ieee802Dot1D);
    }                                                       /* "IEEE 802.1d" */
    else if (version == DOT1S_FORCE_VERSION_DOT1W)
    {
      osapiSnprintf( buf, sizeof(buf), pStrInfo_switching_Ieee802Dot1W);
    }                                                       /* "IEEE 802.1w" */
    else if (version == DOT1S_FORCE_VERSION_DOT1S )
    {
      osapiSnprintf( buf, sizeof(buf), pStrInfo_switching_Ieee802Dot1S);
    }                                                       /* "IEEE 802.1s" */
    cliFormatShortText(ewsContext,pStrInfo_switching_SpanTreeVer);   /* "Spanning Tree Version" */
    ewsTelnetWrite(ewsContext, buf);
  }

  /* Spanning Tree BPDU guard */
  osapiSnprintf(buf,sizeof(buf), pStrInfo_common_Dsbld);   /* Disabled */
  if (usmDbDot1sBpduGuardGet(unit, &boolVal) == L7_SUCCESS )
  {
    if (boolVal == L7_TRUE)
    {
      osapiSnprintf( buf,sizeof(buf), pStrInfo_common_Enbld);
    }                                                /* Enabled */
  }
  cliFormatShortText(ewsContext,pStrInfo_switching_SpanTreeBpduGuardMode); /*"BPDU Guard mode"*/
  ewsTelnetWrite(ewsContext, buf);

  /* Spanning Tree BPDU Filter */
  osapiSnprintf( buf,sizeof(buf), pStrInfo_common_Dsbld);   /* Disabled */
  if (usmDbDot1sBpduFilterGet(unit, &boolVal) == L7_SUCCESS )
  {
    if (boolVal == L7_TRUE)
    {
      osapiSnprintf( buf,sizeof(buf), pStrInfo_common_Enbld);
    }                                                /* Enabled */
  }
  cliFormatShortText(ewsContext,pStrInfo_switching_PortBpduFilterMode); /*"BPDU Filter mode"*/
  ewsTelnetWrite(ewsContext, buf);

  /* Configuration Name */
  memset ( buf, 0, sizeof(buf) );
  if ( usmDbDot1sConfigNameGet(unit, buf) == L7_SUCCESS )
  {
    cliFormatShortText(ewsContext,pStrInfo_switching_CfgName);   /* "Configuration Name" */
    if(mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_PRIVILEGE_USER_MODE))
    {
      ewsTelnetWrite(ewsContext, buf);
    }
    else
    {
      ewsTelnetWrite(ewsContext, "****" );
    }
  }

  /* Configuration Revision Level */
  if ( usmDbDot1sConfigRevisionGet(unit, &revision) == L7_SUCCESS )
  {
    cliFormatShortText(ewsContext,
        pStrInfo_switching_CfgRevisionLvl); /* "Configuration Revision Level" */
    memset ( buf, 0, sizeof(buf) );
    if(mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_PRIVILEGE_USER_MODE))
    {
      osapiSnprintf( buf, sizeof(buf), "%u", revision );
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "****" );
    }
    ewsTelnetWrite(ewsContext, buf);
  }

  /* Configuration Digest Key */
  memset ( digest, 0, sizeof(digest) );
  if ( usmDbDot1sConfigDigestKeyGet(unit, digest) == L7_SUCCESS )
  {
    cliFormatShortText(ewsContext,
        pStrInfo_switching_CfgDigestKey); /* "Configuration Digest Key" */
    memset (key, 0, sizeof(key) );
    if(mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_PRIVILEGE_USER_MODE))
    {
      osapiSnprintf(key, sizeof(key),
          "0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
          digest[0],digest[1],digest[2],digest[3],
          digest[4],digest[5],digest[6],digest[7],
          digest[8],digest[9],digest[10],digest[11],
          digest[12],digest[13],digest[14],digest[15]);
    }
    else
    {
      osapiSnprintf(key, sizeof(key), "****" );
    }
    ewsTelnetWrite(ewsContext, key);
  }

  /* Configuration Format Selector */
  if ( usmDbDot1sConfigFormatSelectorGet(unit, &format) == L7_SUCCESS )
  {
    cliFormatShortText(ewsContext,
        pStrInfo_switching_CfgFmtSelectOr); /* "Configuration Format Selector" */
    ewsTelnetPrintf (ewsContext, "%u", format);
  }

  /* MST Instances */
  (void)cliSpanTreeShowMstList( ewsContext );

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
 *
 * @purpose  display information about a specific vlan
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
 * @notes add range checking
 *
 * @cmdsyntax  show spanning-tree vlan <vlan>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandShowSpanningTreeVlan(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 argVid = 1;
  L7_uint32 vid;
  L7_uint32 mstid;
  L7_uint32 numArg;          /* New variable Added */
  L7_uint32 unit;

  L7_uchar8 buf[ L7_CLI_MAX_STRING_LENGTH ];

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if ( numArg != 1 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_ShowDot1sVlan_1);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index+argVid],&vid) != L7_SUCCESS ||
      vid > L7_DOT1Q_MAX_VLAN_ID ||
      vid < L7_DOT1Q_MIN_VLAN_ID )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_switching_Dot1sVlan, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
  }

  if ( usmDbDot1sVlanToMstiGet(unit, vid, &mstid) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_Dot1sInvalidVlan);
  }

  cliFormat(ewsContext,pStrInfo_common_VlanId_3); /* "VLAN Identifier" */
  ewsTelnetPrintf (ewsContext, "%u", vid );

  memset ( buf, 0, sizeof(buf) );
  if (mstid == L7_DOT1S_CIST_INSTANCE )
  {
    cliFormat(ewsContext,pStrInfo_switching_AssociatedInst);   /* "Associated Instance" */
    osapiSnprintf( buf, sizeof(buf), pStrInfo_common_Cst );         /* "CST" */
  }
  else
  {
    cliFormat(ewsContext,pStrInfo_switching_AssociatedMstInst);   /* "Associated MST Instance" */
    osapiSnprintf( buf, sizeof(buf), "%u", mstid );
  }
  return cliSyntaxReturnPrompt (ewsContext, buf);
}
