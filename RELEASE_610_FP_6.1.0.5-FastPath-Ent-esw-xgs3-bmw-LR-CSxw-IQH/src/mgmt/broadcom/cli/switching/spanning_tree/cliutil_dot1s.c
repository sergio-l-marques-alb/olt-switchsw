/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/dot1s/cliutil_dot1s.c
 *
 * @purpose assorted functions for cli spantree
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   10/30/2002
 *
 * @author  Jill Flanagan
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include <errno.h>
#include "cliapi.h"
#include "clicommands_dot1s.h"
#include "usmdb_dot1s_api.h"
#include "dot1s_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "cliutil_dot1s.h"
#ifdef L7_DOT3AH_PACKAGE
#include "usmdb_dot3ah.h"
#endif
/*********************************************************************
* @purpose  Implements the set spanning tree instance logic that is
*           required by 2 cli commands.
*
* @param    ewsContext   context for error messages
* @param    instance     spanning tree instance
* @param    priority     priority for this instance
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/

void cliSpanTreeSetInstancePriority(EwsContext ewsContext, L7_uint32 instance,
                                    L7_uint32 priority)
{
  L7_uint32 unit;
  L7_uint32 userPriority = priority;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return;
  }

  if ( usmDbDot1sMstiBridgePrioritySet( unit,
                                       instance,
                                       &userPriority ) != L7_SUCCESS )
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_switching_Dot1sBridgePri);
    cliSyntaxBottom(ewsContext);
    return;
  }

  if (userPriority != priority)
  {
    memset ( buf, 0, sizeof(buf));
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetPrintf (ewsContext, pStrInfo_switching_Dot1sBridgePriInfo, userPriority );
    cliSyntaxBottom(ewsContext);
    return;
  }
}

/*********************************************************************
* @purpose  Query and display the list of FIDs and VIDs associated with the
*           specified mstid.  Display the count for each followed by the
*           list in a table.
*
* @param    ewsContext   context for error messages
* @param    mstid     spanning tree instance
* @param    displayLines max Lines to output
* @param    firstPass   required first time this is called
*
* @returns L7_SUCCESS
* @returns L7_FAILURE if paging is required
* @returns L7_ERROR  if not enough space to display headings
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t cliSpanTreeShowFidsAndVlans(EwsContext ewsContext, L7_uint32 mstid,
                                    L7_uint32 displayLines, L7_BOOL firstPass )
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 temp[L7_CLI_MAX_STRING_LENGTH];

  static L7_uint32 fidCount;
  static L7_uint32 vidCount;

  static L7_uint32 fid;
  static L7_uint32 vid;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    /* NOTE: Returning L7_NOT_EXIST below as L7_FAILURE and L7_ERROR
     *       imply different results to the calling function.
     */
    return L7_NOT_EXIST;
  }

  if ( firstPass == L7_TRUE )
  {
    fidCount = 0;
    vidCount = 0;

    if ( usmDbDot1sMstiFIDFirstGet(unit, mstid, &fid) == L7_SUCCESS )
    {
      fidCount++;
    }

    if ( usmDbDot1sMstiVIDFirstGet(unit, mstid, &vid) == L7_SUCCESS )
    {
      vidCount++;
    }

    /* FUTURE: Add call to get count for both FIDs and VIDs here */

    if (fidCount != 0 || vidCount != 0)
    {

      if (displayLines < 3 )        /* 3 lines of header required */
      {
        return L7_ERROR;
      }

      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (1, 0, 5, 0, L7_NULLPTR,  ewsContext,
                               pStrInfo_switching_AssociatedFidsAssociatedVlans);
      ewsTelnetWrite( ewsContext,
                     "\r\n     ---------------           ----------------");

      displayLines -= 3;
    }
    else
    {
      /* No FIDs or VIDs to display */
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_Dot1sShowFidVlanNone);
    }

  }

  while (fidCount != 0 && vidCount != 0 && displayLines > 0 )
  {
    displayLines--;

    memset (buf, 0, sizeof(buf));
    if (fidCount != 0)
    {
      osapiSnprintf( buf, sizeof(buf), "\r\n     %-4u           ", fid);
    }
    else
    {
      osapiSnprintf( buf, sizeof(buf), "                   ");
    }

    if (vidCount != 0)
    {
      osapiSnprintf( temp, sizeof(temp), "           %-4u", vid);
      strcat( buf, temp);
    }
    else
    {
      strcat( buf, "              ");
    }
    ewsTelnetWrite(ewsContext, buf);

    if (usmDbDot1sMstiFIDNextGet(unit, mstid, fid, &fid) == L7_SUCCESS &&
        fid != 0)
    {
      fidCount++;
    }
    else
    {
      fidCount = 0;
    }

    if (usmDbDot1sMstiVIDNextGet(unit, mstid, vid, &vid) == L7_SUCCESS &&
        vid != 0)
    {
      vidCount++;
    }
    else
    {
      vidCount = 0;
    }

  }   /* end of while  */

  if (displayLines == 0 && (fidCount!=0 || vidCount!=0))
  {
    /* paging required */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Query and display the list of MSTIDs
*
* @param    ewsContext   context for error messages
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes This function uses the ShortText version of cliFormat
*
* @end
*********************************************************************/

L7_RC_t cliSpanTreeShowMstList(EwsContext ewsContext )
{

  L7_char8 buf[ L7_CLI_MAX_STRING_LENGTH ];
  L7_char8 temp[ L7_CLI_MAX_STRING_LENGTH ];
  L7_uint32 mstid;
  L7_uint32 count = 0;
  L7_uint32 storedCount = 0;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  if ( usmDbDot1sInstanceFirstGet(unit, &mstid) == L7_SUCCESS )
  {
    /* The first instance is the cist.  Skip to the first mist */
    while (usmDbDot1sInstanceNextGet(unit, mstid, &mstid) == L7_SUCCESS )
    {
      count++;
      if (count == 1)
      {
        cliFormatShortText(ewsContext,
                           pStrInfo_switching_MstInsts);/* "MST Instances" */
        osapiSnprintf( buf, sizeof(buf), "%u", mstid );

      }
      else
      {
        osapiSnprintf( temp, sizeof(temp), ",%u", mstid );
        strcat( buf, temp);
      }

      storedCount++;

      if (storedCount >= 8)         /* Wrap the line */
      {
        ewsTelnetWrite(ewsContext, buf);
        osapiSnprintf( buf, sizeof(buf), "\r\n                                   ");
        storedCount = 0;
      }
    }

    if (storedCount != 0)
    {                                       /* don't redisplay the line */
      ewsTelnetWrite(ewsContext, buf);
    }
  }

  if (count == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_Dot1sShowMstNone);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format the provided char* into 802.1 bridge identifier format
*
* @param    stat output string
* @param    stat output string buffer size
* @param    idIn input string to be formatted
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/

void cliSpanTreeFormatBridgeIdentifier( L7_char8 * stat, L7_uint32 statSize, L7_uchar8 * idIn )
{
  osapiSnprintf(stat, statSize, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                idIn[0], idIn[1], idIn[2], idIn[3], idIn[4], idIn[5], idIn[6], idIn[7]);
  return;
}

/*********************************************************************
* @purpose  Format the provided ushort16 port identifier into a string
*           displayable in the format xx:xx.
*
* @param    stat output string. Must be at least 5 bytes long.
* @param    stat output string buffer size
* @param    val16 input val
*
* @returns void
*
* @notes
*
* @end
*********************************************************************/

void cliSpanTreeFormatPortIdentifier( L7_char8 * stat, L7_uint32 statSize, L7_ushort16 val16 )
{
  L7_char8 temp[5];

  memset (temp, 0,sizeof(temp));
  memset (stat, 0, statSize);
  osapiSnprintf(temp, sizeof(temp), "%04X",val16);
  osapiSnprintf(stat, statSize, "%c%c:%c%c", temp[0], temp[1], temp[2], temp[3]);

  return;
}

/*********************************************************************
* @purpose  Display the statistics for the specified port
*
* @param    ewsContext to write the output
* @param    interface associated with this port
*
* @returns void
*
* @notes
*
* @end
*********************************************************************/

void cliSpanTreeDisplayPortStats( EwsContext ewsContext, L7_uint32 iface )
{
  L7_uint32 val, unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return;
  }

  /* Port Stats */
  if ( usmDbDot1sPortStatsSTPBPDUsSentGet(unit, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_common_StpBpdusTxed);     /* "STP BPDUs Sent" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }
  if ( usmDbDot1sPortStatsSTPBPDUsReceivedGet(unit, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_common_StpBpdusRcvd);     /* "STP BPDUs Received" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  if ( usmDbDot1sPortStatsRSTPBPDUsSentGet(unit, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_common_RstpBpdusTxed);     /* "RSTP BPDUs Sent" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }
  if ( usmDbDot1sPortStatsRSTPBPDUsReceivedGet(unit, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_common_RstpBpdusRcvd);     /* "RSTP BPDUs Received" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }

  if ( usmDbDot1sPortStatsMSTPBPDUsSentGet(unit, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_common_MstpBpdusTxed);     /* "MSTP BPDUs Sent" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }
  if ( usmDbDot1sPortStatsMSTPBPDUsReceivedGet(unit, iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_common_MstpBpdusRcvd);     /* "MSTP BPDUs Received" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }
#ifdef L7_DOT3AH_PACKAGE
  if (usmDbDot3ahPortStatsOAMPDUsTxGet(iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_common_Dot3ahpdusTxed);     /* "Dot3ah PDUs Sent" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }
  if ( usmDbDot3ahPortStatsOAMPDUsRxGet(iface, &val) == L7_SUCCESS )
  {
    cliFormat(ewsContext,pStrInfo_common_Dot3ahpdusRcvd);     /* "Dot3ah PDUs Received" */
    ewsTelnetPrintf (ewsContext, "%u", val );
  }
#endif

}

/*********************************************************************
* @purpose  Get and return a string containing the role of the specified port
*
* @param    unit index
* @param    instance spanningtree instance to use
* @param    iface
* @param    role buffer to store the return string
* @param    size of buffer
*
* @returns void
*
* @notes The returned string will be null terminated.  The role
*        buffer must be at least L7_DOT1S_MAX_PORT_ROLE_LENGTH+1 in size;
*
* @end
*********************************************************************/

L7_RC_t cliSpanTreeGetPortRole( L7_uint32 unit, L7_uint32 instance,
                               L7_uint32 iface, L7_char8 * role,
                               L7_uint32 size )
{
  L7_uint32 val;

  if (size <= L7_DOT1S_MAX_PORT_ROLE_LENGTH )
  {
    return L7_ERROR;
  }

  if ( usmDbDot1sMstiPortRoleGet( unit, instance, iface, &val ) == L7_SUCCESS )
  {
    memset ( role, 0, size);
    switch (val)
    {
    case L7_DOT1S_ROLE_ROOT:
      sprintf( role, pStrInfo_common_RootPort);     /*"Root Port" */
      break;
    case L7_DOT1S_ROLE_DESIGNATED:
      sprintf( role, pStrInfo_common_DesignatedPort);     /* "Designated Port" */
      break;
    case L7_DOT1S_ROLE_ALTERNATE:
      sprintf( role, pStrInfo_common_AlternatePort);     /* "Alternate Port" */
      break;
    case L7_DOT1S_ROLE_BACKUP:
      sprintf( role, pStrInfo_common_Backup);     /* "Backup Port" */
      break;
    case L7_DOT1S_ROLE_MASTER:
      sprintf( role, pStrInfo_common_Master);     /* "Master Port" */
      break;
    case L7_DOT1S_ROLE_DISABLED:
      sprintf( role, pStrInfo_common_Dsbld);     /* "Disabled Port" */
      break;
    default:
      sprintf( role, pStrInfo_common_Blank);     /* "------" */
      break;
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get and return port forwarding state for the specified port
*           and instance.
*
* @param    unit index
* @param    instance spanningtree instance to use
* @param    iface
* @param    role buffer to store the return string
* @param    size of buffer
*
* @returns void
*
* @notes The returned string will be null terminated.  The role
*        buffer must be at least L7_DOT1S_MAX_PORTFWDSTATE_LENGTH+1 in size;
*
* @end
*********************************************************************/

L7_RC_t cliSpanTreeGetPortFwdState( L7_uint32 unit, L7_uint32 instance,
                                   L7_uint32 iface, L7_char8 * stat,
                                   L7_uint32 size )
{
  L7_uint32 val;

  if (size <= L7_DOT1S_MAX_PORTFWDSTATE_LENGTH )
  {
    return L7_ERROR;
  }

  if ( usmDbDot1sMstiPortForwardingStateGet(unit, instance,
                                            iface, &val) == L7_SUCCESS )
  {
    memset (stat, 0,sizeof(stat));
    switch (val)
    {
    case L7_DOT1S_DISCARDING:
      if ((usmDbDot1sMstiPortLoopInconsistentStateGet(unit, instance, iface, &val) == L7_SUCCESS) 
            && (val == L7_TRUE))
      {
        sprintf( stat, pStrInfo_common_DiscardingErr );        /* "Discarding*" */
      }
      else
      {
        sprintf( stat, pStrInfo_common_Discarding );        /* "Discarding" */
      }
      break;
    case L7_DOT1S_LEARNING:
      sprintf( stat, pStrInfo_common_Learning );        /* "Learning" */
      break;
    case L7_DOT1S_FORWARDING:
      sprintf( stat, pStrInfo_common_Fwd );        /* "Forwarding" */
      break;
    case L7_DOT1S_MANUAL_FWD:
      sprintf( stat, pStrInfo_common_ManualFwd );        /* "Manual forwarding" */
      break;
    case L7_DOT1S_NOT_PARTICIPATE:
      sprintf( stat, pStrErr_switching_NotParticipating );        /* "Not participating" */
      break;
    case L7_DOT1S_DISABLED:
      sprintf( stat, pStrInfo_common_Dsbld );        /* "Disabled" */
      break;
    default:
      return L7_FAILURE;
      /*PASSTHRU*/

    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose check user's input to determine if it is printable
 *
 *
 * @param *input         the argument entered by user
 * @param length         length of string to check
 *
 * @returns   L7_SUCCESS if all characters are printable
 * @returns   L7_FAILURE if there is one or more nonprintable characters
 *
 * @notes
 *
 *
 * @end
 *
 ********************************************************************/
L7_RC_t cliIsPrintable(L7_char8 * input, L7_uint32 length)
{
#define MAX_PRINTABLE_CHAR 0x7E
#define MIN_PRINTABLE_CHAR 0x20

  L7_uint32 i;

  for (i=0; i<length; i++)
  {
    if (input[i] > MAX_PRINTABLE_CHAR ||
        input[i] < MIN_PRINTABLE_CHAR)
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Determine if the specified port is valid for spanning tree
 *          and generate a failure message if not.
 *
 *
 * @param ewsContext   context for error messages
 * @param unit         the unit index (for a stacking environment)
 * @param iface        the interface to check
 *
 * @return   L7_SUCCESS the interface is valid
 * @return   L7_FAILURE the interface is not a valid type for dot1s.
 *
 * @note An interface is considered valid for dot1s if it is one of
 *       the following types:    USM_PHYSICAL_INTF or USM_LAG_INTF
 *       In case of failure, the failure message is written to the
 *       cli context.  The caller needs only to return.
 *
 *
 * @end
 *
 ********************************************************************/
L7_RC_t cliValidateDot1sInterface(EwsContext ewsContext,
                                  L7_uint32 unit, L7_uint32 iface)
{
  return cliValidateDot1sInterfaceMessage(ewsContext, unit, iface, L7_TRUE);
}

/*********************************************************************
 *
 * @purpose Determine if the specified port is valid for spanning tree
 *          and generate error message based on message parameter.
 *
 *
 * @param ewsContext   context for error messages
 * @param unit         the unit index (for a stacking environment)
 * @param iface        the interface to check
 * @param message      L7_BOOL generate error message or not
 *
 * @return   L7_SUCCESS the interface is valid
 * @return   L7_FAILURE the interface is not a valid type for dot1s.
 *
 * @note An interface is considered valid for dot1s if it is one of
 *       the following types:    USM_PHYSICAL_INTF or USM_LAG_INTF
 *       In case of failure, the failure message is written to the
 *       cli context.  The caller needs only to return.
 *
 *
 * @end
 *
 ********************************************************************/
L7_RC_t cliValidateDot1sInterfaceMessage(EwsContext ewsContext,
                                         L7_uint32 unit, L7_uint32 iface,
                                         L7_BOOL message )
{
  L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF + USM_LAG_INTF;
  L7_uint32 rc = L7_SUCCESS;
  L7_uint32 intIfType;

  /* Get the interface type always returns L7_SUCCESS*/
  (void)usmDbIntIfNumTypeMaskGet(unit, iface, &intIfType);

  /* Validate the interface type */
  if ( (intIfType & inclIntfTypes) == 0 )
  {
    /* Invalid port */
    if (message == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_Dot1sPortModeInvalid);
      cliSyntaxBottom(ewsContext);
    }
    rc = L7_FAILURE;
  }

  return rc;
}
