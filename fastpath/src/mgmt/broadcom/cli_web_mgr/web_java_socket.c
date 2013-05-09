/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename web_java_socket.c
*
* @purpose support for java socket connections
*
* @component
*
* @comments none
*
* @create  11/19/2001
*
* @author  jlshaw
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#include "cli_web_include.h"
#include "l7_common.h"
#include "poe_exports.h"
#include "cli_web_exports.h"
#include "usmdb_cardmgr_api.h"
#include "usmdb_cnfgr_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_registry_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_slotmapper.h"
#include "usmdb_spm_api.h"
#include "usmdb_status.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"

#ifdef L7_WIRELESS_PACKAGE
#include "usmdb_wdm_ap_api.h"
#include "usmdb_wdm_api.h"
#include "usmdb_wdm_dev_loc_api.h"
#endif

#include "usmdb_counters_api.h"

#include "osapi_support.h"
#include "l7_socket.h"
#include "web_java.h"
#include "usmdb_poe_api.h"
#include "session.h"
#include "l7utils_api.h"

#ifdef L7_WIRELESS_PACKAGE
#include "usmdb_wdm_nv_api.h"
#include "usmdb_wnv_api.h"
#include "usmdb_wdm_client_api.h"
#include "usmdb_wdm_client_security_api.h"
#include "wireless_commdefs.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#ifndef _L7_OS_LINUX_
  #include "vxWorks.h"
  #include <taskLib.h>
  #include <selectLib.h>
#else
#endif /* _L7_OS_LINUX_ */

#define USMWEB_NUM_SUPPORTED_SFP_INTERFACES   L7_MAX_PORT_COUNT
#define USMWEB_DATA_SIZE                  6   /* ChangeFlag, Datalength, SerialInterface, ServicePort, FanStatus1, FanStatus2 */
#define USMWEB_BUFFER_SIZE (32*1024)
/* #define USMWEB_BUFFER_SIZE   (L7_MAX_PORT_COUNT + USMWEB_NUM_SUPPORTED_SFP_INTERFACES + USMWEB_DATA_SIZE) */


#define USMWEB_RECV_RETRIES_MAX 10     /* Max number of recv() retries  */

#define JAVA_TCP_PORT     4242         /* JAVA listening port                 */
#define JAVA_MAX_CLIENTS  12           /* max number of client connections    */
#define LISTENQ           10           /* number of connections to queue      */

/* Commands */
#define WJS_ON        1          /* CMD Connect                               */
#define WJS_OFF       2          /* CMD Disconnect                            */

/* Map Applet commands */
#define WJS_APPLET    8          /* CMD Applet update                         */
#define WJS_SAMPLE    9          /* CMD Retrieve stat for specified interface */


/* NV Applet commands */
#define WJS_NV_GET_USER_DATA   10
#define WJS_NV_SET_USER_DATA   11
#define WJS_NV_GET_IMAGE_NAMES 12
#define WJS_NV_DEL_IMAGE       13
#define WJS_NV_GET_GC          14
#define WJS_NV_DEL_GC          15
#define WJS_NV_SET_GC          16
#define WJS_NV_GET_LOCAL_WS    17
#define WJS_NV_DEL_LOCAL_WS    18
#define WJS_NV_SET_LOCAL_WS    19
#define WJS_NV_GET_PEER_WS     20
#define WJS_NV_DEL_PEER_WS     21
#define WJS_NV_SET_PEER_WS     22
#define WJS_NV_GET_MAP         23
#define WJS_NV_GET_UAP         24
#define WJS_NV_DEL_AP          25
#define WJS_NV_SET_AP          26
#define WJS_NV_RESET_AP        27
#define WJS_NV_MANAGE_AP       28
#define WJS_NV_ACKNOWLEDGE_AP  29
#define WJS_NV_GET_CS          30
#define WJS_NV_DISASSOCIATE_CS 31
#define WJS_NV_POLL_RESET      32
#define WJS_NV_POLL            33
#define WJS_NV_GET_STATUS      34
#define WJS_NV_GET_DCS         35
#define WJS_NV_SET_DCS         36
#define WJS_NV_DEL_DCS         37
#define WJS_NV_GET_BLDG_FLR_DATA 38
#define WJS_NV_GET_OLT         39 /* On-Demand Location Trigger */
#define WJS_NV_SET_OLT         40
#define WJS_NV_START_SEARCH    41
#define WJS_NV_GET_OLT_STATUS  42
#define WJS_NV_GET_OLT_FLR_STATUS  43
#define WJS_NV_GET_SESSION_STATUS 99  /* Is the corresponding web session active */
#define WJS_NV_ON              101          /* CMD NV Connect     */
#define WJS_NV_OFF             102          /* CMD NV Disconnect  */


#define WJS_DEVSHELL  50

#define WJS_TX_TOTAL_FRAMES  1
#define WJS_RX_TOTAL_FRAMES  2

#define INTF_TYPE_DATA_RJ45     101
#define INTF_TYPE_DATA_GBIC     102
#define INTF_TYPE_DATA_XAUI     103
#define INTF_TYPE_DATA_MTRJ     104
#define INTF_TYPE_SERIAL_PORT   201
#define INTF_TYPE_SERVICE_PORT  301
#define INTF_TYPE_STACKING      401

#define JAVA_APPLET_SLOT_EMPTY      0
#define JAVA_APPLET_SLOT_POPULATED  1

#define WJS_POE_UP              0x8004
#define WJS_POE_DOWN            0x8005
#define WJS_POE_MODE_MASK       0x8000

/*****
 * if ipv6 package is defined, there are two choices for binding
 * sockets. with a linux kernel, bind once with the ipv6
 * unspecified address. v4 connections are then accepted as ipv6 v4-mapped.
 * a v4 bind cannot be performed as addr_in_use error is returned.
 * with other stacks, bind once for v4 inaddr_any and once for v6 unspecified.
 *
*****/
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
#undef L7_JAVA_V4_BIND
#else
#define L7_JAVA_V4_BIND
#endif

extern L7_int32 cliDevShell(L7_char8 *cmd);

static L7_BOOL bRemoteDevshellEnabled = L7_FALSE;

extern L7_BOOL cliConvertMac(L7_uchar8 *buf, L7_uchar8 *mac);
L7_BOOL usmWebChangeDetected(L7_uint32 unit, L7_char8 *data);


void tcpDevShellHelp(void)
{
  sysapiPrintf("\n tcpDevShell(flag)");
  sysapiPrintf("\n ");
  sysapiPrintf("\n Enables or disables remote (tcp) devshell capability using the java ");
  sysapiPrintf("\n tool tcpDevShell.jar where flag is :");
  sysapiPrintf("\n   0 - disable the capability");
  sysapiPrintf("\n   1 - enable the capability");
  sysapiPrintf("\n ");
  if (bRemoteDevshellEnabled == L7_TRUE)
    sysapiPrintf("\n tcpDevShell is currently ENABLED");
  else
    sysapiPrintf("\n tcpDevShell is currently DISABLED");
  sysapiPrintf("\n ");
}

void tcpDevShell(L7_uint32 flag)
{
  if (flag == 1)
  {
    bRemoteDevshellEnabled = L7_TRUE;
    sysapiPrintf("\n tcpDevShell has been ENABLED");
  }
  else
  {
    bRemoteDevshellEnabled = L7_FALSE;
    sysapiPrintf("\n tcpDevShell has been DISABLED");
  }
  sysapiPrintf("\n ");
}

struct WJS_Response_Packet
{
  int  rc;
  int  speed;
  int  val0;
  int  val1;
};

struct WJS_Response_Packet resp_packet;

int num_bytes_to_send;
int num_ints_to_send;
L7_uint32 javaAppletTraceOption = 0x00;
L7_uint32 javaAppletTraceCounter = 0;
L7_uint32 javaAppletTraceMode = 0;
L7_uint32 javaAppletBoxIDOverride = 0;
L7_uint32 javaAppletBoxID= 0;

void javaTraceHelp(void)
{
  sysapiPrintf("\njavaTrace(options, counter, appletTraceMode)");
  sysapiPrintf("\n ");
  sysapiPrintf("\n Enables trace on the java applet ");
  sysapiPrintf("\n where options are the server side trace options:");
  sysapiPrintf("\n   0 - no trace output for the server side");
  sysapiPrintf("\n   1 - trace output to the screen");
  sysapiPrintf("\n   2 - trace output is logged");
  sysapiPrintf("\n   3 - trace output to screen and log");
  sysapiPrintf("\n where counter is the number of requests to trace");
  sysapiPrintf("\n where appletTraceMode is the client side trace mode and ");
  sysapiPrintf("\n all trace output on the client goes to the java console");
  sysapiPrintf("\n   0 - no trace output for the client side");
  sysapiPrintf("\n   1 - traces the read data path");
  sysapiPrintf("\n ");
  sysapiPrintf("\njavaBoxID(boxID, boxMode)");
  sysapiPrintf("\n ");
  sysapiPrintf("\n Overrides the boxID ");
  sysapiPrintf("\n where boxID is the desired box id");
  sysapiPrintf("\n where boxMode is:");
  sysapiPrintf("\n   0 - clear the box id override");
  sysapiPrintf("\n   1 - override the box id");
  sysapiPrintf("\n ");
  sysapiPrintf("\njavaTraceServer()");
  sysapiPrintf("\n ");
  sysapiPrintf("\n Will simulate one client side data request ");
  sysapiPrintf("\n ");
}

void javaTrace(L7_uint32 jatOptions, L7_uint32 jatCounter, L7_uint32 jatMode)
{
  javaAppletTraceOption = jatOptions;
  javaAppletTraceCounter = jatCounter;
  javaAppletTraceMode = jatMode;
}

void javaAppletTraceMsg(L7_char8 *msg)
{
  if (javaAppletTraceOption & 0x01)
  {
    sysapiPrintf("%s", msg);
  }
  if (javaAppletTraceOption & 0x02)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "%s", msg);
  }
}

void javaBoxID(L7_uint32 boxID, L7_uint32 boxMode)
{
  javaAppletBoxIDOverride = boxMode;
  javaAppletBoxID = boxID;
}

void javaTraceServer(void)
{
  L7_char8  buffer[USMWEB_BUFFER_SIZE];  /* The size of buffer[] must match */

  usmWebChangeDetected(1, buffer);
}


L7_uint32 javaAppletCardPresentGet(L7_uint32 unit, L7_uint32 slot)
{
  L7_uint32  numPorts;

  if (usmDbPortsPerSlotGet(unit, slot, &numPorts) == L7_SUCCESS)
    return JAVA_APPLET_SLOT_POPULATED;

  return JAVA_APPLET_SLOT_EMPTY;
}

L7_uint32 javaAppletCardTypeGet(L7_uint32 unit, L7_uint32 slot)
{
  L7_uint32 cardType;

  cardType = JAVA_APPLET_SLOT_EMPTY;

  if (usmDbCardInsertedCardTypeGet(unit, slot, &cardType) == L7_SUCCESS)
    cardType = (cardType & 0x0000FFFF);

  return cardType;
}

L7_uint32 javaAppletSlotPortNumberFirstGet(L7_uint32 unit, L7_uint32 slot)
{
  L7_uint32 firstPort;

  firstPort = 1;

  return firstPort;
}

L7_BOOL usmWebSessionStatusGet(L7_char8 *cookie)
{
  L7_char8 session_id[256];
  L7_inet_addr_t inetAddr;
  L7_uint32 sessionType = L7_LOGIN_TYPE_UNKNWN;
  L7_char8 *pStr;

  /* printf("usmWebSessionStatusGet : cookie = %s\n", cookie);  TRGTRG */
  /* Parse the cookie */
  pStr = strchr(cookie, '=');
  if (pStr == NULL)
  {
    return L7_FALSE;
  }
  strncpy(session_id, pStr+1, sizeof(session_id)-1);
  *pStr = '\0'; /* Now cookie is only cookie name */

  if (strcmp(cookie, "SIDSSL") == 0)
  {
    sessionType = L7_LOGIN_TYPE_HTTPS;
  }
  else if (strcmp(cookie, "SID") == 0)
  {
    sessionType = L7_LOGIN_TYPE_HTTP;
  }
  else
  {
    return L7_FALSE;
  }

  /* printf("usmWebSessionStatusGet : calling EwaSessionLookup with cookie name = %s and val = %s\n", cookie, session_id);  TRGTRG */
  memset(&inetAddr, 0x00, sizeof(inetAddr)); /* This is not used in the following call */
  if (EwaSessionLookup(session_id, sessionType, inetAddr, L7_FALSE) == NULL)
  {
    return L7_FALSE;
  }
  else
  {
    return L7_TRUE;
  }
}

void usmWebPortsStatusGet(L7_uint32 unit, L7_uint32 slot, L7_uint32 *slotNumPorts, L7_uint32 *slotNumSFPs, L7_ulong32 *portStatus, L7_uint32 portStatusSize)
{
  L7_uint32 u, s, p, indx, sfpNum, intIfNum, admin_mode, linkState, poeLinkUp=0;
  L7_uint32 phyCapability;
  L7_uint32 spmIfNum;       /* FPS - front panel stacking interface number */
  L7_uint32 spmStackMode;   /* FPS - front panel stacking mode */
  L7_uint32 spmLinkState;
  L7_uint32 intIfFlags;
  L7_uint32 unitType;
  L7_BOOL   sfpLinkUp, sfpSupport, spmSupport, poeSupport;
  L7_char8 jMsg[256];
  L7_RC_t rcc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t spmPortEntry;

  u = unit;
  s = slot;
  p = 1;
  indx = 0;
  sfpNum = 0;

  memset(portStatus, 0x00, sizeof(L7_ulong32) * portStatusSize);

  while (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) == L7_SUCCESS)
  {
    sfpSupport = L7_FALSE;
    sfpLinkUp = L7_FALSE;
    admin_mode = L7_ENABLE;
    intIfFlags = 0x00;
    poeLinkUp = WJS_POE_DOWN;
    poeSupport = L7_FALSE;

    /* In the Enduro, there are no combo ports, but this function returns 2, which will cause the applet to fail.
     * Hence put a check for box type and skip the check for SFP ports.
     */
    rcc = usmDbUnitMgrUnitTypeGet(unit, &unitType);
    if ( ((unitType & 0xFFFF0000)/0x10000) != 0xb334 )
    { 
      if (usmDbIntfPhyCapabilityGet(intIfNum, &phyCapability) == L7_SUCCESS)
      {
        if (phyCapability & L7_PHY_CAP_PORTSPEED_SFP)
        {
          /* sprintf(jMsg, "usmWebPortsStatusGet() L7_PHY_CAP_PORTSPEED_SFP for port %d/%d/%d\n", u, s, p);
          javaAppletTraceMsg(jMsg); */
          sfpSupport = L7_TRUE;
          sfpNum++;
        }
      }
    }

    spmStackMode = L7_DISABLE;
    spmLinkState = L7_DOWN;
    memset(&spmPortEntry, 0x00, sizeof(SPM_STACK_PORT_ENTRY_t));

    /* ---------------------------------------------------------------------- */
    /* FPS - Does this interface support Front Panel Stacking and what is     */
    /*       it's current mode                                                */
    /* ---------------------------------------------------------------------- */
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID, L7_FPS_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbSpmFpsIndexFromUspGet (u, s, p, &spmIfNum) == L7_SUCCESS)
      {
        /* sprintf(jMsg, "This is a stacking Interface - port %d/%d/%d\n", u, s, p);
        javaAppletTraceMsg(jMsg); */
        spmSupport = L7_TRUE;
        if (usmdbSpmStackingCapablePortGet (spmIfNum, &spmPortEntry) == L7_SUCCESS)
        {
          /* sprintf(jMsg, "usmdbSpmStackingCapablePortGet() == L7_SUCCESS for port %d/%d/%d\n", u, s, p);
          javaAppletTraceMsg(jMsg);  */
          if (spmPortEntry.port_info.speed == 10)
          {
            /*sprintf(jMsg, "This is a HiGig / 10-Gig Interface - port %d/%d/%d\n", u, s, p);
            javaAppletTraceMsg(jMsg); */
            /* This is a HiGig / 10-Gig Interface */
            if (usmDbFeaturePresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID, L7_FPS_PORT_MODE_CFG_FEATURE_ID) == L7_TRUE)
            {
              intIfFlags |= APPLET_IFLAG_STACK_MODE_SUPPORT;
              spmStackMode = spmPortEntry.port_info.runtime_mode;
            }
            else
            {
              spmStackMode = L7_ENABLE;
            }

            if (spmStackMode == L7_ENABLE)
            {
              /* sprintf(jMsg, "Interface %d/%d/%d is in Stacking Mode\n", u, s, p);
              javaAppletTraceMsg(jMsg); */
              intIfFlags |= APPLET_IFLAG_STACK_MODE_HIGIG;
              if (spmPortEntry.port_info.link_status == 1)
                spmLinkState = L7_UP;
            }
            else
            {
              /* sprintf(jMsg, "Interface %d/%d/%d is in 10G Mode\n", u, s, p);
              javaAppletTraceMsg(jMsg); */
              intIfFlags |= APPLET_IFLAG_STACK_MODE_10G;
            }
          }
          else
          {
            intIfFlags |= APPLET_IFLAG_FPS_SUPPORT;

            if (usmDbFeaturePresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID, L7_FPS_PORT_MODE_CFG_FEATURE_ID) == L7_TRUE)
            {
              intIfFlags |= APPLET_IFLAG_FPS_MODE_SUPPORT;
              spmStackMode = spmPortEntry.port_info.runtime_mode;
            }
            else
            {
              spmStackMode = L7_ENABLE;
            }

            if (spmStackMode == L7_ENABLE)
            {
              intIfFlags |= APPLET_IFLAG_FPS_MODE;
              if (spmPortEntry.port_info.link_status == 1)
                spmLinkState = L7_UP;
            }
          }
        }
        /* sprintf(jMsg, "usmWebPortsStatusGet() FPS Mode for port %d.%d and SPM Interface %d is %d\n", s, p, spmIfNum, spmStackMode);
        javaAppletTraceMsg(jMsg); */
      }
    }


    if (usmDbIntfStatusGet(intIfNum, &linkState) == L7_SUCCESS)
    {
      if (linkState == L7_INTF_ATTACHED)
      {
        if (usmDbIfOperStatusGet(u, intIfNum, &linkState) != L7_SUCCESS)
          linkState = L7_DOWN;

        if (sfpSupport == L7_TRUE)
        {
          if (usmDbStatusSFPLinkUpGet(u, intIfNum, &sfpLinkUp) != L7_SUCCESS)
            sfpLinkUp = L7_FALSE;
        }

        if (usmDbIfAdminStateGet(u, intIfNum, &admin_mode) != L7_SUCCESS)
          admin_mode = L7_DISABLE;
#ifdef L7_POE_PACKAGE
        /* if this port supports POE, check POE status */
        if (usmDbPoeIsValidIntf(intIfNum) == L7_SUCCESS)
        {
          L7_uint32 val;
          poeSupport = L7_TRUE;
          if (usmDbPethPsePortDetectionStatusGet(0,intIfNum,&val) == L7_SUCCESS)
          {
            if (val == L7_POE_PORT_DETECTION_STATUS_DELIVERING_POWER)
            {
              poeLinkUp = WJS_POE_UP;
            }
          }
        }
#endif

      }
      else
      {
        linkState = L7_DOWN;
        sfpLinkUp = L7_FALSE;
      }

      if (spmStackMode == L7_DISABLE)
      {
        if (admin_mode == L7_DISABLE || admin_mode == L7_DIAG_DISABLE)
        {
          if (sfpSupport == L7_TRUE)
            portStatus[indx+1] = 2;
          portStatus[indx] = 2;
        }
        else
        {
          if (sfpLinkUp == L7_TRUE)
          {
            portStatus[indx+1] = 1;
          }
          else
          {
            if (linkState == L7_UP)
              portStatus[indx]   = 1;
          }
        }
        if (poeSupport == L7_TRUE)
        {
          if (poeLinkUp == WJS_POE_UP)
             portStatus[indx] = WJS_POE_UP; /* POE bit needs to match APPLET_IFLAG_POE_MODE in MapApplet.java */
          else
             portStatus[indx] |= WJS_POE_MODE_MASK;
        }
      }
      else /* Stack Mode is Enabled */
      {
        if (spmLinkState == L7_UP)  /* Not able to deal with sfp check on FPS interfaces TRGTRG */
          portStatus[indx]   = 1;
      }

      portStatus[indx] |= intIfFlags;
      if (sfpSupport == L7_TRUE)
          portStatus[indx+1] |= intIfFlags;
    }
    else
    {
      sprintf(jMsg, "usmDbIntfStatusGet() FAILED for Interface %d/%d/%d\n", u, s, p);
      javaAppletTraceMsg(jMsg);
    }

    /* sprintf(jMsg, "usmWebPortsStatusGet() link state for port %d.%d is %d\n", s, p, portStatus[indx]);
    javaAppletTraceMsg(jMsg); */

    if (sfpSupport == L7_TRUE)
    {
      indx++;
      /* sprintf(jMsg, "usmWebPortsStatusGet() link state for sfp %d.%d is %d\n", s, p, portStatus[indx]);
      javaAppletTraceMsg(jMsg); */
    }
    indx++;
    p++;

#ifdef FEAT_METRO_CPE_V1_0
    if(p > L7_MAX_PHYSICAL_PORTS_PER_UNIT)
    {
      break;
    }
#else
    if ( p * 2 > portStatusSize)
      break;
#endif
  }

  sprintf(jMsg, "usmWebPortsStatusGet() data count is %d\n", indx);
  javaAppletTraceMsg(jMsg);

  *slotNumPorts = p-1;
  *slotNumSFPs = sfpNum;
  return;

}

/*********************************************************************
*
* @purpose  Is the unit ready and available for applet operations
*
* @notes    none
* @returns  L7_TRUE     ready
*           L7_FALSE    not ready
*
* @end
*********************************************************************/
L7_BOOL usmWebUnitReady(L7_uint32 unitNum)
{
  L7_uint32 i;
  L7_uint32 numSlots;
  L7_uint32 slots[10];
  L7_uint32 cardType;
  L7_uint32 unitType;
  L7_uint32 unitStatus;
  L7_char8  jMsg[256];
  L7_BOOL isFull = L7_FALSE;

  if (usmDbUnitMgrUnitTypeGet(unitNum, &unitType) == L7_SUCCESS)
  {
    if (usmDbUnitMgrUnitStatusGet(unitNum, &unitStatus) == L7_SUCCESS)
    {
      if (unitStatus == L7_USMDB_UNITMGR_UNIT_NOT_PRESENT)
      {
        isFull = L7_TRUE;
        sprintf(jMsg, "usmWebUnitReady() Unit %d is preconfigured\n", unitNum);
        javaAppletTraceMsg(jMsg);
      }
      else if (unitStatus == L7_USMDB_UNITMGR_UNIT_OK)
      {
        if (usmDbUnitDbEntrySlotsGet(unitType, &numSlots, slots) == L7_SUCCESS)
        {
          for (i=0; i<numSlots; i++)
          {
            /* Need to deal with pluggable cards ... don't bother checking if no card present */
            if ((usmDbCardInsertedCardTypeGet(unitNum, i, &cardType) == L7_SUCCESS) && (cardType != 0))
            {
            if (usmDbSlotIsFullGet(unitNum, i, &isFull) == L7_SUCCESS)
            {
              if (isFull == L7_FALSE)
                {
                  sprintf(jMsg, "usmWebUnitReady() Unit/Slot %d/%d NOT full\n", unitNum, i);
                  javaAppletTraceMsg(jMsg);
                break;
            }
            else
            {
                  sprintf(jMsg, "usmWebUnitReady() Unit/Slot %d/%d full\n", unitNum, i);
                  javaAppletTraceMsg(jMsg);
                }
              }
              else
              {
                sprintf(jMsg, "usmWebUnitReady() Call to usmDbSlotIsFullGet() failed for Unit/Slot %d/%d\n", unitNum, i);
                javaAppletTraceMsg(jMsg);
              isFull = L7_FALSE;
              break;
            }
          }
        }
      }
      }
      else
      {
        isFull = L7_FALSE;
        sprintf(jMsg, "usmWebUnitReady() Unit %d state is %d\n", unitNum, unitStatus);
        javaAppletTraceMsg(jMsg);
      }
    }
  }
  if (isFull == L7_FALSE)
  {
    sprintf(jMsg, "usmWebUnitReady() Unit %d is not available\n", unitNum);
    javaAppletTraceMsg(jMsg);
  }
  return isFull;
}




/*********************************************************************
*
* @purpose  Detect differences in link status and update the map applet
*           if there are any
*
* @notes    none
* @returns  L7_TRUE     if there are differences
*           L7_FALSE    if there are no differences
*
* @end
*********************************************************************/
L7_BOOL usmWebChangeDetected(L7_uint32 unit, L7_char8 *data)
{
      L7_uint32 i, numUnits, indx, prev, slots[10];
  L7_uint32 slotNumPorts;
  L7_uint32 slotNumSFPs;
  L7_uint32 serviceLink, sfpIntfNum;
  L7_uint32 statusCode;
  L7_uint32 u, pU, numU;
  /* L7_uint32 ifIndex; */
  L7_uint32 unitNum, unitType, numSlots, numSFPs;
       /*L7_ulong32 portStatus[L7_MAX_PORTS_PER_SLOT*2];*/  /* Make sure space for SFP's */
       /*L7_RC_t rc, rcc;*/
       L7_char8  jMsg[256];
       /*L7_uint32 ledStatus;*/
       L7_BOOL flag = L7_FALSE;
       L7_uint32  firstUnitNum;
       L7_uint32  counter, numStackIntfIndex;
      SPM_STACK_PORT_ENTRY_t spmPortEntry;

   L7_ulong32 portStatus[L7_MAX_PORTS_PER_SLOT*2];
  L7_RC_t rc, rcc;
   L7_uint32  k;
   /*L7_uint32 ledStatus=0;*/

  sfpIntfNum = 0;
  indx = 0;
  numUnits = 0;
  firstUnitNum = 0;

  if (javaAppletTraceCounter > 0)
  {
    javaAppletTraceCounter--;
  }
  else
  {
    javaAppletTraceOption = 0;   /* No Trace */
    javaAppletTraceMode = 0;
  }

  l7utilsIntToBytes(&data[indx],javaAppletTraceMode); /* Trace Options */
  indx+=4;
  sprintf(jMsg, "\nClient side trace mode : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
  javaAppletTraceMsg(jMsg);

  /* XUI flag */
#ifdef L7_XUIBASE_PACKAGE
  l7utilsIntToBytes(&data[indx], 1); /* XUI Presence (1) Present, (0) Absent */
  indx+=4;
#else
  l7utilsIntToBytes(&data[indx], 0); /* XUI Presence (1) Present, (0) Absent */
  indx+=4;
#endif
  sprintf(jMsg, "\nXUI Present : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
  javaAppletTraceMsg(jMsg);

  if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
  {
    l7utilsIntToBytes(&data[indx],1); /* Stacking */
    indx+=4;
    rc = usmDbUnitMgrStackMemberGetFirst(&unitNum);
    while (rc == L7_SUCCESS)
    {
      if (usmWebUnitReady(unitNum) == L7_TRUE)
      {
        numUnits++;
        if (firstUnitNum == 0)
          firstUnitNum = unitNum;
      }
      prev = unitNum;
      rc = usmDbUnitMgrStackMemberGetNext(prev, &unitNum);
    }
  }
  else
  {
    l7utilsIntToBytes(&data[indx],0); /* Non-stacking */
    indx+=4;
    rc = usmDbUnitMgrMgrNumberGet(&unitNum);
    if (rc == L7_SUCCESS)
    {
      numUnits = 1;
      firstUnitNum = unitNum;
    }
  }
  sprintf(jMsg, "Stacking Support : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
  javaAppletTraceMsg(jMsg);

  l7utilsIntToBytes(&data[indx],numUnits); /* Number of units */
  indx+=4;
  sprintf(jMsg, "Number of units : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
  javaAppletTraceMsg(jMsg);

  if ((numUnits > 0) && (firstUnitNum != 0))
  {
    rc = L7_SUCCESS;
    unitNum = firstUnitNum;
  }

  while (rc == L7_SUCCESS)
  {
    rcc = usmDbUnitMgrUnitStatusGet(unitNum, &i);
    if ((rcc == L7_SUCCESS) && (i == L7_USMDB_UNITMGR_UNIT_OK))
      statusCode = 1;
    else
      statusCode = 0;

    rcc = usmDbUnitMgrUnitTypeGet(unitNum, &unitType);
    rcc = usmDbUnitDbEntrySlotsGet(unitType, &numSlots, slots);
    /* In the Enduro, there are no combo ports, but this function returns 2, which will cause the applet to fail.
     * Hence put a check for box type and skip the check for SFP ports.
     */
    if ( ((unitType & 0xFFFF0000)/0x10000) != 0xb334 )
    {
      rcc = usmWebNumSFPsGet(&numSFPs);
    }

    l7utilsIntToBytes(&data[indx],unitNum); /* Unit Number */
    indx+=4;
    sprintf(jMsg, "Unit Number : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    if (javaAppletBoxIDOverride == 1)
      l7utilsIntToBytes(data,(javaAppletBoxIDOverride & 0xFFFF)); /* Box Type  */
    else
      l7utilsIntToBytes(&data[indx],((unitType & 0xFFFF0000)/0x10000)); /* Box Type  */
    indx+=4;
    sprintf(jMsg, "Box Type : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    /* Unit Status TBD ... (1) mgmt unit, (2) mgmt capable, (3) member unit */
    l7utilsIntToBytes(&data[indx],statusCode); /* Unit Status */
    indx+=4;
    sprintf(jMsg, "Status Code : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    l7utilsIntToBytes(&data[indx],numSFPs); /* Number of SFPs */
    indx+=4;
    sprintf(jMsg, "Number of SFPs : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    l7utilsIntToBytes(&data[indx],numSlots); /* Number of slots */
    indx+=4;
    sprintf(jMsg, "Number of Slots : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    for (i=0; i<numSlots; i++)
    {
      l7utilsIntToBytes(&data[indx],slots[i]); /* Slot Number */
      indx+=4;
      sprintf(jMsg, "Slot Number : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      l7utilsIntToBytes(&data[indx],javaAppletCardPresentGet(unitNum, slots[i])); /* Status: (1) Populated, (0) Empty */
      indx+=4;
      sprintf(jMsg, "Slot Populated : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      l7utilsIntToBytes(&data[indx], javaAppletCardTypeGet(unitNum, slots[i]));
      indx+=4;
      sprintf(jMsg, "Card Type : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      usmWebPortsStatusGet(unitNum, slots[i], &slotNumPorts, &slotNumSFPs, portStatus, L7_MAX_PORTS_PER_SLOT*2);
      l7utilsIntToBytes(&data[indx], slotNumPorts); /* Number of interfaces */
      indx+=4;
      sprintf(jMsg, "Number of Ports : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      l7utilsIntToBytes(&data[indx], javaAppletSlotPortNumberFirstGet(unitNum, slots[i])); /* Number of first port */
      indx+=4;
      sprintf(jMsg, "First Port Number : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      for (k=0; k<(slotNumPorts+slotNumSFPs); k++)
      {
        l7utilsIntToBytes(&data[indx], portStatus[k]);
        indx+=4;
      }
      sprintf(jMsg, "Read %d integers for slot %d (%d-ports and %d sfps)\n", slotNumPorts+slotNumSFPs, slots[i], slotNumPorts, slotNumSFPs);
      javaAppletTraceMsg(jMsg);
    }

    /* Get the dedicated stack interfaces */
    if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
      l7utilsIntToBytes(&data[indx], 0); /* No dedicated stack interfaces on FPS systems */
      numStackIntfIndex = indx;
      indx+=4;

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID, L7_FPS_PORT_MODE_CFG_FEATURE_ID) != L7_TRUE)
      {
        /*  printf("\n Looking for dedicated stack interfaces"); */
        counter = 0;
        memset (&spmPortEntry, 0, sizeof(SPM_STACK_PORT_ENTRY_t));
        while (usmdbSpmStackingCapablePortNextGet (&spmPortEntry) == L7_SUCCESS)
        {
          if (spmPortEntry.unit == unitNum)  /* Isolate the stack interfaces to their corresponding unit */
          {
            counter++;
            l7utilsIntToBytes(&data[indx], spmPortEntry.port_info.link_status);
            indx+=4;
            sprintf(jMsg, "Link State for HiGig%d on Unit%d is %d\n", counter, unitNum, spmPortEntry.port_info.link_status);
            javaAppletTraceMsg(jMsg);
          }
        }
        l7utilsIntToBytes(&data[numStackIntfIndex], counter);
      }
      sprintf(jMsg, "Number of dedicated stack interfaces : %d\n", l7utilsBytesToInt(&data[numStackIntfIndex]));
      javaAppletTraceMsg(jMsg);
    }

    if (SERVICE_PORT_PRESENT != L7_FALSE)
    {
      l7utilsIntToBytes(&data[indx], 1); /* Service Port Presence (1) Present, (0) Absent */
      indx+=4;
      serviceLink = usmDbServicePortLinkStatusGet(unitNum);
      if (serviceLink == L7_UP)
        l7utilsIntToBytes(&data[indx], 1);
      else
        l7utilsIntToBytes(&data[indx], 0);
      indx+=4;
    }
    else
    {
      l7utilsIntToBytes(&data[indx], 0); /* Service Port Presence (1) Present, (0) Absent */
      indx+=4;
    }

    l7utilsIntToBytes(&data[indx], 1); /* Serial Port Presence (1) Present, (0) Absent */
    indx+=4;
    sprintf(jMsg, "Serial Port Presence : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    if (usmDbSerialStatusGet(unitNum) == L7_UP)
      l7utilsIntToBytes(&data[indx], 1); /* Serial Port Link State */
    else
      l7utilsIntToBytes(&data[indx], 0); /* Serial Port Link State */
    indx+=4;
    sprintf(jMsg, "Serial Port Link State : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
      numU = 0;
      rcc = usmDbUnitMgrStackMemberGetFirst(&u);
      while (rcc == L7_SUCCESS)
      {
        if (usmWebUnitReady(u) == L7_TRUE)
          numU++;
        pU = u;
        rcc = usmDbUnitMgrStackMemberGetNext(pU, &u);
      }

      if (numU == numUnits)
      {
        prev = unitNum;
        rc = usmDbUnitMgrStackMemberGetNext(prev, &unitNum);

        if (rc == L7_SUCCESS)
          sprintf(jMsg, "usmDbUnitMgrStackMemberGetNext( %d ) found unit %d \n", prev, unitNum);
        else
          sprintf(jMsg, "usmDbUnitMgrStackMemberGetNext( %d ) found no more units \n", prev);
        javaAppletTraceMsg(jMsg);
      }
      else
      {
        sprintf(jMsg, "Num units reported different from num units found \n");
        javaAppletTraceMsg(jMsg);
        /* Re-start the update if there was a unit added or removed */
        l7utilsIntToBytes(&data[indx], 0); /* Send zero as number of units - the applet will ignore this update */
        indx+=4;

        num_bytes_to_send = indx;
        return flag;
      }
    }
    else
    {
      rc = L7_FAILURE;  /* Exit the while loop */
      sprintf(jMsg, "\nExiting the loop for this data request \n");
      javaAppletTraceMsg(jMsg);

    }

    /* Fan or power status bytes would go here */
  }


  l7utilsIntToBytes(&data[indx], 0x11223344); /* End of data */
  indx+=4;
  sprintf(jMsg, "End of data marker : data[%d] = 0x%x\n", (indx-4)/4, l7utilsBytesToInt(&data[indx-4]));
  javaAppletTraceMsg(jMsg);

  num_bytes_to_send = indx;
  sprintf(jMsg, "Sending %d pieces (%d bytes) of data\n", num_bytes_to_send/4, num_bytes_to_send);
  javaAppletTraceMsg(jMsg);


  return flag;
}



  /* The LED support will need to be modified ... not currently supported ****
     Will need to be inserted before the end of data and also coded in the java

  if (usmDbStatusLedGet(unit, &ledStatus) == L7_SUCCESS)
  {
    switch (ledStatus)
    {
    case L7_LED_GREEN:
      statusCode = USMWEB_LED_STATUS_GREEN;
      break;
    case L7_LED_AMBER:
      statusCode = USMWEB_LED_STATUS_AMBER;
      break;
    case L7_LED_YELLOW:
      statusCode = USMWEB_LED_STATUS_YELLOW;
      break;
    case L7_LED_RED:
      statusCode = USMWEB_LED_STATUS_RED;
      break;
    case L7_LED_OFF:
      statusCode = USMWEB_LED_STATUS_OFF;
      break;
    default:
      statusCode = USMWEB_LED_STATUS_OFF;
    }
  }
  else
  {
    statusCode = USMWEB_LED_STATUS_OFF;
  }
  if (statusCode != data[L7_MAX_PORT_COUNT + USMWEB_NUM_SUPPORTED_SFP_INTERFACES + 4])
  {
    flag=L7_TRUE;
    data[L7_MAX_PORT_COUNT + USMWEB_NUM_SUPPORTED_SFP_INTERFACES + 4] = statusCode;
  }


  if (usmDbStatusFanLedGet(unit, &ledStatus) == L7_SUCCESS)
  {
    switch (ledStatus)
    {
    case L7_LED_GREEN:
      statusCode = USMWEB_LED_STATUS_GREEN;
      break;
    case L7_LED_AMBER:
      statusCode = USMWEB_LED_STATUS_AMBER;
      break;
    case L7_LED_YELLOW:
      statusCode = USMWEB_LED_STATUS_YELLOW;
      break;
    case L7_LED_RED:
      statusCode = USMWEB_LED_STATUS_RED;
      break;
    case L7_LED_OFF:
      statusCode = USMWEB_LED_STATUS_OFF;
      break;
    default:
      statusCode = USMWEB_LED_STATUS_OFF;
    }
  }
  else
  {
    statusCode = USMWEB_LED_STATUS_OFF;
  }
  if (statusCode != data[L7_MAX_PORT_COUNT + USMWEB_NUM_SUPPORTED_SFP_INTERFACES + 5])
  {
    flag=L7_TRUE;
    data[L7_MAX_PORT_COUNT + USMWEB_NUM_SUPPORTED_SFP_INTERFACES + 5] = statusCode;
  }
  **** The LED support will need to be modified ... not currently supported */

#ifdef L7_WIRELESS_PACKAGE /* BEGIN NETWORK VISUALIZATION FOR WIRELESS */

/*********************************************************************
*
* @purpose  Retrieve User Data (UD)
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvUserDataGet(L7_char8 *data)
{
  wdmNVUserData_t nvUD;
  L7_uint32 idx = 0;
  L7_uchar8 checkLocation = 0;
  L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t  status = L7_WDM_ONDEMAND_LOC_TRIG_STATUS_NOT_INPROGRESS;
  L7_BOOL flag = L7_FALSE;
  L7_uchar8   nvCfgMigrate = 0;

  if (L7_SUCCESS != usmDbWdmNVUserDataGet(&nvUD))
  {
    nvUD.showPowerDisplay = 0;
    nvUD.showTabView      = 0;
    nvUD.showMAPs         = 0;
    nvUD.showRAPs         = 0;
    nvUD.showMAPClients   = 0;
    nvUD.showDetectedClients   = 0;
    nvUD.showLocationFlag   = 0;
  }

   usmDbWdmNVCfgMigrateGet(&flag);

   if(flag == L7_TRUE)
   {
     nvCfgMigrate = 1;
   }
   else
   {
     nvCfgMigrate = 0;
   }
   /* Reset the flag after it is read */
   usmDbWdmNVCfgMigrateReset();

  data[idx] = nvUD.showPowerDisplay;
  idx+=1;
  data[idx] = nvUD.showTabView;
  idx+=1;
  data[idx] = nvUD.showMAPs;
  idx+=1;
  data[idx] = nvUD.showRAPs;
  idx+=1;
  data[idx] = nvUD.showMAPClients;
  idx+=1;
  data[idx] = nvUD.showDetectedClients;
  idx+=1;

  usmDbWdmNVCheckLocationGet(&status);

  if(status == L7_WDM_ONDEMAND_LOC_TRIG_STATUS_NOT_INPROGRESS)
    checkLocation = 0;
  else
    checkLocation = 1;

  data[idx] = checkLocation;
  idx+=1;

  data[idx] = nvUD.showLocationFlag;
  idx+=1;

  data[idx] = nvCfgMigrate;
  idx+=1;


  /* DEBUG */
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server retrieved user data p=%d t=%d m=%d r=%d c=%d dc=%d cl=%d.lf=%d cf=%d\n",
    nvUD.showPowerDisplay,nvUD.showTabView,nvUD.showMAPs,nvUD.showRAPs,nvUD.showMAPClients,nvUD.showDetectedClients,
    checkLocation, nvUD.showLocationFlag,nvCfgMigrate);

  return idx;
}

/*********************************************************************
*
* @purpose  Save User Data (UD)
*
* @notes    none
*
* @end
*********************************************************************/
void nvUserDataSet(wdmNVUserData_t nvUD)
{
  usmDbWdmNVUserDataSet(&nvUD);

  /* DEBUG */
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server set user data p=%d t=%d m=%d r=%d c=%d dc=%d.lf=%d\n",
    nvUD.showPowerDisplay,nvUD.showTabView,nvUD.showMAPs,nvUD.showRAPs,nvUD.showMAPClients,nvUD.showDetectedClients,
    nvUD.showLocationFlag);
}

/*********************************************************************
*
* @purpose  Retrieve image file names located in the NV ram directory
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvImageNameListGet(L7_char8 *data)
{
  L7_char8 fileName[L7_NV_MAX_FILE_NAME+1];
  L7_char8 tempName[L7_NV_MAX_FILE_NAME+1];
  L7_char8 *ext;
  L7_uint32 cnt = 0;
  L7_uint32 idx = 0;
  L7_uint32 len = 0;
  struct dirent *entry;
  DIR *dir;

  l7utilsIntToBytes(&data[idx],cnt);
  idx+=4;

  if ((dir = opendir(RAM_NV_PATH RAM_NV_NAME)) == NULL)
  {
    return(idx);
  }

  /* Retrieve file names, set an arbitrary name limit to prevent crazyness */
  while (((entry = readdir(dir)) != NULL) && ((L7_NV_MAX_GC_NAME*4) >= cnt))
  {
    memset(&fileName,0x00,sizeof(fileName));
    memset(&tempName,0x00,sizeof(tempName));

    strncpy(fileName, entry->d_name, L7_NV_MAX_FILE_NAME);
    strncpy(tempName,fileName,L7_NV_MAX_FILE_NAME);
    ext = strrchr(tempName,'.');
    if ((NULL!=ext) && (4==strlen(ext))) {
      osapiConvertToLowerCase(ext);
      if ((strcmp(ext,".gif")==0) || (strcmp(ext,".jpg")==0))
      {
        cnt++;
        len = strlen(fileName);
        l7utilsIntToBytes(&data[idx],len);
        idx+=4;
        strncpy(&data[idx],fileName,len);
        idx+=len;
      }
    }
  }
  l7utilsIntToBytes(&data[0],cnt);

  /* DEBUG */
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
          "NV: Server found %d image names in %s%s.\n",cnt,RAM_NV_PATH,RAM_NV_NAME);
  return idx;
}

/*********************************************************************
*
* @purpose  Delete image file from NV ram directory
*
* @notes    none
* @returns  none
*
* @end
*********************************************************************/
void nvDeleteImage(wdmNVGraphCfgData_t nvGC)
{
  L7_char8 fileName[L7_NV_MAX_FILE_NAME+1];
  L7_char8 tempName[L7_NV_MAX_FILE_NAME+1];
  L7_char8 fqf[256];
  struct dirent *entry;
  L7_uint32 cnt = 0;
  DIR *dir;

  if ((dir = opendir(RAM_NV_PATH RAM_NV_NAME)) == NULL)
  {
    return;
  }

  osapiConvertToLowerCase(nvGC.fileName);

  /* Search target file name for deletion */
  while (((entry = readdir(dir)) != NULL) && ((L7_NV_MAX_GC_NAME*4) >= cnt))
  {
    memset(&fileName,0x00,sizeof(fileName));
    memset(&tempName,0x00,sizeof(tempName));

    strncpy(fileName, entry->d_name, L7_NV_MAX_FILE_NAME);
    strncpy(tempName,fileName,L7_NV_MAX_FILE_NAME);
    osapiConvertToLowerCase(tempName);
    if (strcmp(tempName,nvGC.fileName)==0)
    {
      sprintf(fqf,"%s%s/%s",RAM_NV_PATH,RAM_NV_NAME,fileName);
      remove(fqf);
      usmDbWnvSetImageCfgChanged();

      /* DEBUG */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "NV: Server found and deleted %s.\n",fileName);
      break;
    }
  }
  return;
}

/*********************************************************************
*
* @purpose  Retrieve building floor Data (UD)
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvBldgFlrListGet(L7_char8 *data)
{
  L7_uint32 idx   = 0;
  L7_uint32 bldgMax = 0;
  L7_uint32 flrMax  = 0;


  bldgMax = L7_WDM_BUILDING_NUM_MAX;
  flrMax  = L7_WDM_FLOOR_NUM_MAX;

  l7utilsIntToBytes(&data[idx],bldgMax);
  idx+=4;

  l7utilsIntToBytes(&data[idx],flrMax);
  idx+=4;

  WIRELESS_DLOG(WD_LEVEL_NV_INFO,"Retrieved bldg Max = %d, floor Max =%d.\n",bldgMax,flrMax);
  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve Graph Configurations (GC) from USMDB
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvGCListGet(L7_char8 *data)
{
  wdmNVGraphCfgData_t nvGC;
  L7_uint32 cnt   = 0;
  L7_uint32 idx   = 0;
  L7_uint32 len   = 0;
  L7_uchar8 index = 0;

  cnt = 0;
  l7utilsIntToBytes(&data[idx],cnt); /* 1st int holds total #, init with zero */
  idx+=4;

  /* Retrieve Graphs, limit return count (a precautionary measure)  */
  while ((usmDbWdmNVGraphEntryNextGet(index,&index,&nvGC) == L7_SUCCESS) &&
        ((L7_NV_MAX_GC_NAME) >= cnt))
  {
    len = strlen(nvGC.graphName);
    l7utilsIntToBytes(&data[idx],len);
    idx+=4;
    strncpy(&data[idx],nvGC.graphName,len);
    idx+=len;
    len = strlen(nvGC.fileName);
    l7utilsIntToBytes(&data[idx],len);
    idx+=4;
    strncpy(&data[idx],nvGC.fileName,len);
    idx+=len;
    data[idx] = index;
    idx+=1;
    data[idx] = nvGC.imageMeasurement;
    idx+=1;
    l7utilsIntToBytes(&data[idx],nvGC.imageWidth);
    idx+=4;
    l7utilsIntToBytes(&data[idx],nvGC.imageHeight);
    idx+=4;
    l7utilsIntToBytes(&data[idx],nvGC.graphLoc.bldgNum);
    idx+=4;
    l7utilsIntToBytes(&data[idx],nvGC.graphLoc.flrNum);
    idx+=4;
    cnt++;

    WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server found graph id=%d, graph name=%d, bldg=%d and floor=%d.\n",
    nvGC.index,nvGC.graphName,nvGC.graphLoc.bldgNum,nvGC.graphLoc.flrNum);
  }

  /* DEBUG */
  l7utilsIntToBytes(&data[0],cnt);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server found %d graph configurations.\n",cnt);
  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve Local Wireless Switch (WS) Configuration from USMDB
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvLocalWSGet(L7_char8 *data)
{
  L7_char8 ips[L7_MAC_STRING_LENGTH+1];
  wnvWSData_t wsCfg;
  wdmNVCoordCfgData_t nvCfg;
  L7_uint32 cnt = 0;
  L7_uint32 idx = 0;

  cnt = 1;
  l7utilsIntToBytes(&data[idx],cnt);
  idx+=4;

  wsCfg.IP = 0;
  usmDbWdmWSIPAddrGet(&wsCfg.IP);
  if (L7_SUCCESS != usmDbWdmNVWSCoordGet(&nvCfg)) {
    nvCfg.graphIndex = 0;
    nvCfg.x = 0;
    nvCfg.y = 0;
  }

  l7utilsIntToBytes(&data[idx],wsCfg.IP);
  idx+=4;
  data[idx] = nvCfg.graphIndex;
  idx+=1;
  l7utilsIntToBytes(&data[idx],nvCfg.x);
  idx+=4;
  l7utilsIntToBytes(&data[idx],nvCfg.y);
  idx+=4;

  /* DEBUG */
  memset(&ips,0x00,sizeof(ips));
  usmDbInetNtoa(wsCfg.IP,ips);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server reported local switch. ip=%s id=%d x=%d x=%d.\n",
    ips,nvCfg.graphIndex,nvCfg.x,nvCfg.y);

  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve Peer Wireless Switch (WS) Configuration from USMDB
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvPeerWSListGet(L7_char8 *data)
{
  L7_char8 ips[L7_MAC_STRING_LENGTH+1];
  wnvWSData_t wsCfg;
  wdmNVCoordCfgData_t nvCfg;
  L7_uint32 cfgSize;
  L7_IP_ADDR_t ip;
  L7_uint32 cnt = 0;
  L7_uint32 idx = 0;

  l7utilsIntToBytes(&data[idx],cnt);  /* 1st int holds total #, init with zero */
  idx+=4;

  cfgSize = sizeof(wsCfg)+sizeof(nvCfg);
  wsCfg.IP = 0;
  ip = 0;

  while ((usmDbWdmPeerSwitchEntryNextGet(ip,&ip) == L7_SUCCESS)
     && ((idx+cfgSize)<USMWEB_BUFFER_SIZE))
  {
    wsCfg.IP = ip;
    if (usmDbWdmNVPeerSwitchEntryGet(ip,&nvCfg) != L7_SUCCESS)
    {
      nvCfg.graphIndex = 0;
      nvCfg.x = 0;
      nvCfg.y = 0;
    }

    /* Don't send if invalid IP */
    if (wsCfg.IP!=0)
    {
    l7utilsIntToBytes(&data[idx],wsCfg.IP);
    idx+=4;
    data[idx] = nvCfg.graphIndex;
    idx+=1;
    l7utilsIntToBytes(&data[idx],nvCfg.x);
    idx+=4;
    l7utilsIntToBytes(&data[idx],nvCfg.y);
    idx+=4;
    cnt++;
    }

    /* DEBUG */
    memset(&ips,0x00,sizeof(ips));
    usmDbInetNtoa(wsCfg.IP,ips);
    WIRELESS_DLOG(WD_LEVEL_NV_INFO,
      "NV: Server detected peer switch. ip=%s id=%d x=%d x=%d.\n",
      ips,nvCfg.graphIndex,nvCfg.x,nvCfg.y);
  }
  l7utilsIntToBytes(&data[0],cnt);
  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve Managed Access Points
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvMAPListGet(L7_char8 *data, L7_BOOL *fOverflow)
{
  wdmNVCoordCfgData_t nvCfg;
  wnvAPData_t apCfg;
  L7_enetMacAddr_t macAdd;
  L7_uint32 cfgSize;
  L7_uint32 cnt = 0;
  L7_uint32 idx = 0;
  L7_uint32 len = 0;
  L7_char8 mac[L7_MAC_STRING_LENGTH];
  L7_char8 location[L7_WDM_MAX_LOCATION_NAME+1];
  L7_char8 ips[32];
  L7_uint32 mode = 0;
  L7_uchar8 radio1Power;
  L7_uchar8 radio2Power;
  L7_uchar8 radio1Channel;
  L7_uchar8 radio2Channel;
  L7_uint32 radio1SentryMode;
  L7_uint32 radio2SentryMode;
  L7_WDM_RADIO_PHY_MODE_t radio1PhyMode;
  L7_WDM_RADIO_PHY_MODE_t radio2PhyMode;
  L7_WDM_MANAGED_AP_STATUS_t status;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 val;
  L7_WDM_ACTION_STATUS_t cfgStatus;
  L7_uint32 profileId;
  L7_uint32 mapSwitchIpAddr;

  *fOverflow = L7_FALSE;

  l7utilsIntToBytes(&data[idx],cnt);
  idx+=4;

  cfgSize = sizeof(wnvAPData_t)+4;
  memset(&macAdd,0x00,sizeof(L7_enetMacAddr_t));

  while (L7_SUCCESS == usmDbWdmManagedAPEntryNextGet(macAdd,&macAdd))
  {
    if ((idx+cfgSize)>USMWEB_BUFFER_SIZE)
    {
      *fOverflow = L7_TRUE;
      break;
    }

    if (L7_SUCCESS != usmDbWdmManagedAPStatusGet(macAdd,&status)) {
      status = 0;
    }
    val = 0;
    memset(buf,0x00,L7_CLI_MAX_STRING_LENGTH);
    if (L7_SUCCESS != usmDbWdmManagedAPCfgStatusGet(macAdd,&cfgStatus,&val,buf)) {
      cfgStatus = L7_WDM_ACTION_FAILURE;
    }
    memset(&location,0x00,sizeof(location));
    usmDbWdmManagedAPLocationGet(macAdd,location);
    if (L7_SUCCESS != usmDbWdmAPValidationGet(&mode)) {
      mode = 0;
    }
    if (L7_SUCCESS != usmDbWdmManagedAPProfileIdGet(macAdd,&profileId)) {
      profileId = 0;
    }
    if (L7_SUCCESS != usmDbWdmManagedAPRadioTxPwrGet(macAdd,1,&radio1Power)) {
      radio1Power = 0;
    }
    if (L7_SUCCESS != usmDbWdmManagedAPRadioChannelGet(macAdd,1,&radio1Channel)) {
      radio1Channel = 0;
    }
    if (L7_SUCCESS != usmDbWdmManagedAPRadioScanSentryModeGet(macAdd,1,&radio1SentryMode)) {
      radio1SentryMode = 0;
    }
    if (L7_SUCCESS != usmDbWdmManagedAPRadioPhyModeGet(macAdd,1,&radio1PhyMode)) {
      radio1PhyMode = 0;
    }
    if (L7_SUCCESS != usmDbWdmManagedAPRadioTxPwrGet(macAdd,2,&radio2Power)) {
      radio2Power = 0;
    }
    if (L7_SUCCESS != usmDbWdmManagedAPRadioChannelGet(macAdd,2,&radio2Channel)) {
      radio2Channel = 0;
    }
    if (L7_SUCCESS != usmDbWdmManagedAPRadioScanSentryModeGet(macAdd,2,&radio2SentryMode)) {
      radio2SentryMode = 0;
    }
    if (L7_SUCCESS != usmDbWdmManagedAPRadioPhyModeGet(macAdd,2,&radio2PhyMode)) {
      radio2PhyMode = 0;
    }
    if (L7_SUCCESS == usmDbWdmNVAPEntryGet(macAdd,&nvCfg)) {
      apCfg.GCId = nvCfg.graphIndex;
      apCfg.X = nvCfg.x;
      apCfg.Y = nvCfg.y;
    }
    else {
      apCfg.GCId = 0;
      apCfg.X    = 0;
      apCfg.Y    = 0;
    }
    /* Add support for ip address of managing switch */
    if (usmDbWdmManagedAPSwitchIPAddrGet(macAdd, &mapSwitchIpAddr) != L7_SUCCESS)
    {
      mapSwitchIpAddr = 0;
    }
    memset(&mac,0x00,sizeof(mac));
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                macAdd.addr[0], macAdd.addr[1],
                macAdd.addr[2], macAdd.addr[3],
                macAdd.addr[4], macAdd.addr[5]);

    len = sizeof(mac);
    strncpy(&data[idx],mac,len);
    idx+=len;
    len = strlen(location);
    l7utilsIntToBytes(&data[idx],len);
    idx+=4;
    strncpy(&data[idx],location,len);
    idx+=len;
    data[idx] = apCfg.GCId;
    idx+=1;
    l7utilsIntToBytes(&data[idx],apCfg.X);
    idx+=4;
    l7utilsIntToBytes(&data[idx],apCfg.Y);
    idx+=4;
    l7utilsIntToBytes(&data[idx],mode);
    idx+=4;
    data[idx] = radio1Power;
    idx+=1;
    data[idx] = radio1Channel;
    idx+=1;
    l7utilsIntToBytes(&data[idx],radio1SentryMode);
    idx+=4;
    l7utilsIntToBytes(&data[idx],radio1PhyMode);
    idx+=4;
    data[idx] = radio2Power;
    idx+=1;
    data[idx] = radio2Channel;
    idx+=1;
    l7utilsIntToBytes(&data[idx],radio2SentryMode);
    idx+=4;
    l7utilsIntToBytes(&data[idx],radio2PhyMode);
    idx+=4;
    l7utilsIntToBytes(&data[idx],cfgStatus);
    idx+=4;
    l7utilsIntToBytes(&data[idx],status);
    idx+=4;
    memset(&ips,0x00,sizeof(ips));  /* Managing switch ip address */
    if (mapSwitchIpAddr != 0)
    {
      usmDbInetNtoa(mapSwitchIpAddr,ips);
      len = strlen(ips);
    }
    else
    {
      len = 0;
    }
    l7utilsIntToBytes(&data[idx],len);
    idx+=4;
    if (len > 0)
    {
      strncpy(&data[idx],ips,len);
      idx+=len;
    }
    l7utilsIntToBytes(&data[idx],profileId);
    idx+=4;
    cnt++;

    /* DEBUG */
    WIRELESS_DLOG(WD_LEVEL_NV_INFO,
      "NV: Server detected managed AP. mac=%s id=%d x=%d y=%d m=%d r1p=%d r1c=%d r1s=%d r1m=%d r2p=%d r2c=%d r2s=%d r2m=%d s=%d.\n",
      mac,apCfg.GCId,apCfg.X,apCfg.Y,mode,
      radio1Power,radio1Channel,radio1SentryMode,radio1PhyMode,
      radio2Power,radio2Channel,radio2SentryMode,radio2PhyMode,status);
  }
  l7utilsIntToBytes(&data[0],cnt);
  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve Non-Managed Access Points
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvUAPListGet(L7_char8 *data, L7_BOOL *fOverflow)
{
  wdmNVCoordCfgData_t nvCfg;
  wnvAPData_t apCfg;
  L7_enetMacAddr_t macAdd;
  L7_enetMacAddr_t apMacAdd;
  L7_uint32 cfgSize;
  L7_uint32 cnt = 0;
  L7_uint32 idx = 0;
  L7_uint32 len = 0;
  L7_char8 mac[L7_MAC_STRING_LENGTH];
  L7_char8 location[L7_WDM_MAX_LOCATION_NAME+1];
  L7_uint32 mode = 0;
  L7_uchar8 radio1Power;
  L7_uchar8 radio2Power;
  L7_uchar8 radio1Channel;
  L7_uchar8 radio2Channel;
  L7_uint32 radio1SentryMode;
  L7_uint32 radio2SentryMode;
  L7_WDM_RADIO_PHY_MODE_t radio1PhyMode;
  L7_WDM_RADIO_PHY_MODE_t radio2PhyMode;
  L7_WDM_AP_STATUS_t  status;
  L7_WDM_MANAGED_AP_STATUS_t tmpStatus;
  L7_WDM_ACTION_STATUS_t cfgStatus;
  L7_uint32 profileId;

  *fOverflow = L7_FALSE;

  l7utilsIntToBytes(&data[idx],cnt);
  idx+=4;

  cfgSize = sizeof(wnvAPData_t)+4;
  memset(&macAdd,0x00,sizeof(L7_enetMacAddr_t));

  /* Get all non-managed APs */
  while ((L7_SUCCESS == usmDbWdmRFScanEntryNextGet(macAdd,&macAdd))
     && (L7_FALSE == *fOverflow))
  {
    if ((idx+cfgSize)>USMWEB_BUFFER_SIZE)
    {
      *fOverflow = L7_TRUE;
      break;
    }

    /*
    ** L7_WDM_AP_STATUS_MANAGED = 1, L7_WDM_AP_STATUS_UNKNOWN,
    ** L7_WDM_AP_STATUS_STANDALONE, L7_WDM_AP_STATUS_ROGUE
    */
    usmDbWdmRFScanStatusGet(macAdd,&status);
    if (L7_WDM_AP_STATUS_MANAGED!=status)
    {
      /*
      ** Check this MAC against the base AP MAC. If its not the base,
      ** determine if the AP (base) is managed, if so skip the update
      */
      memset(&apMacAdd,0x00,sizeof(apMacAdd));
      usmDbWdmRFScanAPMacAddrGet(macAdd,&apMacAdd);
      if (0 != memcmp(&macAdd,&apMacAdd,sizeof(L7_enetMacAddr_t)))
      {
        if (usmDbWdmManagedAPStatusGet(apMacAdd,&tmpStatus) == L7_SUCCESS)
        {
          continue;
        }
      }

      memset(&location,0x00,sizeof(location));
      usmDbWdmManagedAPLocationGet(macAdd,location);

      mode = 0; /* N/A */

      radio1Power   = 0;
      radio2Power   = 0;
      radio2Channel = 0;
      radio1Channel = 0;
      radio1PhyMode = 0;
      radio2PhyMode = 0;
      radio1SentryMode = 0;
      radio2SentryMode = 0;
      cfgStatus        = 0;
      profileId        = 0;
      /* Only a single radio channel is valid */
      usmDbWdmRFScanChannelGet(macAdd,&radio1Channel);

      if (L7_SUCCESS == usmDbWdmNVAPEntryGet(macAdd,&nvCfg)) {
        apCfg.GCId = nvCfg.graphIndex;
        apCfg.X    = nvCfg.x;
        apCfg.Y    = nvCfg.y;
      }
      else
      {
        apCfg.GCId = 0;
        apCfg.X    = 0;
        apCfg.Y    = 0;
      }
      memset(&mac,0x00,sizeof(mac));
      sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                macAdd.addr[0], macAdd.addr[1],
                macAdd.addr[2], macAdd.addr[3],
                macAdd.addr[4], macAdd.addr[5]);

      len = sizeof(mac);
      strncpy(&data[idx],mac,len);
      idx+=len;
      len = strlen(location);
      l7utilsIntToBytes(&data[idx],len);
      idx+=4;
      strncpy(&data[idx],location,len);
      idx+=len;
      data[idx] = apCfg.GCId;
      idx+=1;
      l7utilsIntToBytes(&data[idx],apCfg.X);
      idx+=4;
      l7utilsIntToBytes(&data[idx],apCfg.Y);
      idx+=4;
      l7utilsIntToBytes(&data[idx],mode);
      idx+=4;
      data[idx] = radio1Power;
      idx+=1;
      data[idx] = radio1Channel;
      idx+=1;
      l7utilsIntToBytes(&data[idx],radio1SentryMode);
      idx+=4;
      l7utilsIntToBytes(&data[idx],radio1PhyMode);
      idx+=4;
      data[idx] = radio2Power;
      idx+=1;
      data[idx] = radio2Channel;
      idx+=1;
      l7utilsIntToBytes(&data[idx],radio2SentryMode);
      idx+=4;
      l7utilsIntToBytes(&data[idx],radio2PhyMode);
      idx+=4;
      l7utilsIntToBytes(&data[idx],cfgStatus);
      idx+=4;
      l7utilsIntToBytes(&data[idx],status);
      idx+=4;
      len = 0;  /* Placeholder for managing switch ip address */
      l7utilsIntToBytes(&data[idx],len);
      idx+=4;
      l7utilsIntToBytes(&data[idx],profileId);
      idx+=4;
      cnt++;

    /* DEBUG */
    WIRELESS_DLOG(WD_LEVEL_NV_INFO,
      "NV: Server detected un-managed AP. mac=%s id=%d x=%d y=%d r1c=%d s=%d.\n",
      mac,apCfg.GCId,apCfg.X,apCfg.Y,radio1Channel,status);
    }
  }

  l7utilsIntToBytes(&data[0],cnt);
  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve Clients (CS) Configuration from USMDB
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvCSListGet(L7_enetMacAddr_t mac, L7_char8 *data, L7_BOOL *fOverflow)
{
  L7_uint32 cfgSize;
  L7_uint32 cnt = 0;
  L7_uint32 idx = 0;
  L7_uint32 len = 0;
  L7_enetMacAddr_t client;
  L7_enetMacAddr_t vapMac;
  L7_enetMacAddr_t nextVapMac;
  L7_int32 vap = 0;
  L7_IP_ADDR_t ip;
  L7_char8 clientMac[L7_MAC_STRING_LENGTH];
  L7_uchar8 radio;

  *fOverflow = L7_FALSE;

  l7utilsIntToBytes(&data[idx],cnt); /* 1st int holds total #, init with zero */
  idx+=4;

  cfgSize = sizeof(wnvCSData_t)+4;
  radio = 0;

  while (L7_SUCCESS == usmDbWdmManagedAPRadioEntryNextGet(mac,radio,&radio))
  {
    vap = -1;
    while (L7_SUCCESS == usmDbWdmManagedAPVAPEntryNextGet(mac,radio,vap,&vap))
    {
      memset(&vapMac,0x00,sizeof(vapMac));
      usmDbWdmManagedAPVAPMacAddrGet(mac,radio,vap,&vapMac);
      memset(&nextVapMac,0x00,sizeof(nextVapMac));
      memset(&client,0x00,sizeof(L7_enetMacAddr_t));
      while (((L7_SUCCESS == usmDbWdmVAPAssocClientEntryNextGet(vapMac,client,&nextVapMac,&client)) &&
            (0 == memcmp(&vapMac,&nextVapMac, sizeof(L7_enetMacAddr_t)))) &&
            (L7_FALSE == *fOverflow))
      {
        if ((idx+cfgSize)>USMWEB_BUFFER_SIZE)
        {
          *fOverflow = L7_TRUE;
          break;
        }
        usmDbWdmAssocClientTunnelIPAddrGet(client,&ip);
        memset(&clientMac,0x00,sizeof(clientMac));
        sprintf(clientMac, "%02X:%02X:%02X:%02X:%02X:%02X",
                client.addr[0], client.addr[1],
                client.addr[2], client.addr[3],
                client.addr[4], client.addr[5]);
        len = sizeof(clientMac);
        strncpy(&data[idx],clientMac,len);
        idx+=len;
        l7utilsIntToBytes(&data[idx],ip);
        idx+=4;
        data[idx] = radio;
        idx+=1;
        cnt++;

        /* DEBUG */
        WIRELESS_DLOG(WD_LEVEL_NV_INFO,
          "NV: Server detected client. mac=%s radio=%d.\n",clientMac,radio);
      }
      if (*fOverflow == L7_TRUE) break;
    }
    if (*fOverflow == L7_TRUE) break;
  }
  l7utilsIntToBytes(&data[0],cnt);
  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve Detected Clients (DCS) Configuration from USMDB
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvDCSListGet(L7_char8 *data, L7_BOOL *fOverflow)
{
  L7_uint32 cfgSize;
  L7_uint32 cnt = 0;
  L7_uint32 idx = 0;
  L7_uint32 len = 0;
   wdmNVCoordCfgData_t nvCfg;
  L7_enetMacAddr_t client;
  L7_char8 clientMac[L7_MAC_STRING_LENGTH];
  L7_WDM_DETECTED_CLIENT_STATUS_t clientStatus;
  L7_WDM_CLIENT_AUTHENTICATION_STATUS_t clientAuthenticated;
  L7_char8         clientName[L7_WDM_CLIENT_USERNAME_MAX+1];
  L7_uchar8        authRSSI = 0;
  L7_uchar8        channel = 0;
  L7_uint32        status = 0;
  L7_uint32        auth = 0;


  *fOverflow = L7_FALSE;

  l7utilsIntToBytes(&data[idx],cnt); /* 1st int holds total #, init with zero */
  idx+=4;

  /* cfgSize = sizeof(wnvCSData_t)+4;*/

  cfgSize = 69+4;

  memset(&client,0x00,sizeof(L7_enetMacAddr_t));
  while ((L7_SUCCESS == usmDbWdmDetectedClientEntryNextGet(client,&client)) &&
        (L7_FALSE == *fOverflow))
  {
    if ((idx+cfgSize)>USMWEB_BUFFER_SIZE)
    {
      *fOverflow = L7_TRUE;
      break;
    }

    if (L7_SUCCESS != usmDbWdmNVDCSEntryGet(client,&nvCfg)) {

      nvCfg.graphIndex = 0;
      nvCfg.x = 0;
      nvCfg.y = 0;
      WIRELESS_DLOG(WD_LEVEL_NV_INFO,"NV: DCS NV coordinates get returned failure.\n");
    }

    usmDbWdmDetectedClientStatusGet(client,&clientStatus);
    usmDbWdmDetectedClientAuthStatusGet(client,&clientAuthenticated);
    usmDbWdmDetectedClientNameGet(client,clientName);
    usmDbWdmDetectedClientAuthRSSIGet(client,&authRSSI);
    usmDbWdmDetectedClientChannelGet(client,&channel);

    memset(&clientMac,0x00,sizeof(clientMac));
    sprintf(clientMac, "%02X:%02X:%02X:%02X:%02X:%02X",
            client.addr[0], client.addr[1],
            client.addr[2], client.addr[3],
            client.addr[4], client.addr[5]);
    len = sizeof(clientMac);
    strncpy(&data[idx],clientMac,len);
    idx+=len;
    data[idx] = nvCfg.graphIndex;
    idx+=1;
    l7utilsIntToBytes(&data[idx],nvCfg.x);
    idx+=4;
    l7utilsIntToBytes(&data[idx],nvCfg.y);
    idx+=4;
    status = clientStatus;
    auth   = clientAuthenticated;
    l7utilsIntToBytes(&data[idx],clientStatus);
    idx+=4;
    l7utilsIntToBytes(&data[idx],clientAuthenticated);
    idx+=4;
    len = strlen(clientName);
    l7utilsIntToBytes(&data[idx],len);
    idx+=4;
    strncpy(&data[idx],clientName,len);
    idx+=len;
    data[idx] = authRSSI;
    idx+=1;
    data[idx] = channel;
    idx+=1;
    cnt++;

    /* DEBUG */
    WIRELESS_DLOG(WD_LEVEL_NV_INFO,"NV: Server detected client MAC=%s,d=%d x=%d y=%d status =%d, auth=%d,channel =%d.\n",
                  clientMac,nvCfg.graphIndex,nvCfg.x,nvCfg.y,clientStatus,clientAuthenticated,channel);


  }
  l7utilsIntToBytes(&data[0],cnt);
  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve On-Demand Location Trigger Data from USMDB
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvLocationTriggerDataGet(L7_char8 *data, L7_BOOL *fOverflow)
{
  L7_uint32 idx = 0, i, j;
  L7_uint32 len = 0;
  L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t  status = L7_WDM_ONDEMAND_LOC_TRIG_STATUS_NOT_INPROGRESS;
  L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t  device_type = L7_WDM_ONDEMAND_LOC_TARGET_TYPE_AP;
  L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t  radios = L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_SENTRY;
  L7_uint8 buildingNum = 0, floorNum = 0;
  L7_uint32 nextBuilding = 0, nextFloor = 0;
  L7_enetMacAddr_t mac;
  L7_uint32 numOfAps = 0;
  L7_char8 macAddr[L7_MAC_STRING_LENGTH];
  L7_uint8 buildings[L7_WIRELESS_DEV_LOC_MAX_BUILDINGS], floors[L7_WIRELESS_DEV_LOC_MAX_FLOORS_PER_BUILDING];
  L7_uint32 storeBuildingIdx, storeMACIdx;
  L7_uint32 numOfBuildings, numOfFloors, numOfMACs;
  L7_RC_t rc = L7_SUCCESS;

  *fOverflow = L7_FALSE;

  /*intToBytes(&data[idx],cnt);*/ /* 1st int holds total #, init with zero */
  /*idx+=4;*/

  /* Operational Radios */
  usmDbWdmOnDemandLocTrigStatusManagedApRadioTypeGet(&radios);
  l7utilsIntToBytes(&data[idx],radios);
  idx+=4;

  /* Number of APs */
  usmDbWdmOnDemandLocTrigStatusLocatorApsFindUpdate(&numOfAps);
  l7utilsIntToBytes(&data[idx],numOfAps);
  idx+=4;

  /* search-status */
  usmDbWdmOnDemandLocTrigStatusGet(&status);
  l7utilsIntToBytes(&data[idx],status);
  idx+=4;

  usmDbWdmOnDemandLocTrigStatusDevTypeGet(&device_type);
  l7utilsIntToBytes(&data[idx],device_type);
  idx+=4;

  usmDbWdmOnDemandLocTrigStatusBuildingNumGet(&buildingNum);
  data[idx] = buildingNum;
  idx+=1;

  usmDbWdmOnDemandLocTrigStatusFlrNumGet(&floorNum);
  data[idx] = floorNum;
  idx+=1;

  memset(&mac,0x00,sizeof(L7_enetMacAddr_t));
  usmDbWdmOnDemandLocTrigStatusMacGet(&mac);

  memset(&macAddr,0x00,sizeof(macAddr));
  sprintf(macAddr, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  len = sizeof(macAddr);
  strncpy(&data[idx],macAddr,len);
  idx+=len;

  /* Get Building List */
  numOfBuildings = 0;
  l7utilsIntToBytes(&data[idx],numOfBuildings);
  storeBuildingIdx = idx;
  idx+=4;

  buildings[numOfBuildings++] = 0;
  buildingNum = 0;
  floorNum = 0;

  while(rc == L7_SUCCESS)
  {
    floorNum = 0;
    do{
       rc = usmDbWdmDevLocBldngFlrEntryNextGet(buildingNum,
                                              floorNum,
                                              &nextBuilding,
                                              &nextFloor);
       floorNum = nextFloor;
    } while((buildingNum == nextBuilding) && ( rc  == L7_SUCCESS ));

   if(rc == L7_SUCCESS)
   {
     buildingNum = nextBuilding;
     buildings[numOfBuildings++] = buildingNum;
   }
  }

  for ( i = 0; i < numOfBuildings; i++)
  {
    data[idx] = buildings[i];
    idx+=1;
  }
  l7utilsIntToBytes(&data[storeBuildingIdx], numOfBuildings);

  /* Get Floor List */
  numOfFloors = 0;
  floors[numOfFloors++] = 0;

  for ( i = 0; i < numOfBuildings; i++)
  {
    floorNum = 0;
    rc = L7_SUCCESS;
    while(rc == L7_SUCCESS)
    {
      rc = usmDbWdmDevLocBldngFlrEntryNextGet(buildings[i],
                                              floorNum,
                                              &nextBuilding,
                                              &nextFloor);
      if((buildings[i] != nextBuilding) || (rc != L7_SUCCESS ))
      {
        l7utilsIntToBytes(&data[idx],numOfFloors);
        idx+=4;
        for ( j = 0; j < numOfFloors; j++)
        {
          data[idx] = floors[j];
          idx+=1;
        }
        numOfFloors = 1;
        break; /* end of list */
      }
      floorNum = nextFloor;
      floors[numOfFloors++] = floorNum;
    }
  }

  /* Get MAC List */
  numOfMACs = 0;
  l7utilsIntToBytes(&data[idx],numOfMACs);
  storeMACIdx = idx;
  idx+=4;

  memset(&mac,0x00,sizeof(L7_enetMacAddr_t));
  while(usmDbWdmTriangulationLocStatusEntryNextGet(mac, &mac) == L7_SUCCESS)
  {
    memset(&macAddr,0x00,sizeof(macAddr));
    sprintf(macAddr, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
    len = sizeof(macAddr);
    strncpy(&data[idx],macAddr,len);
    idx+=len;
    numOfMACs++;
  }
  l7utilsIntToBytes(&data[storeMACIdx],numOfMACs);

  /* DEBUG */
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,"NV: Location Trigger data MAC=%s,building=%d floor=%d device type=%d use radios =%d, number of aps=%d,status =%d.\n",
                macAddr,buildingNum,floorNum,device_type,radios,numOfAps,status);

  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve On-Demand Location Trigger Data from USMDB
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvLocationTriggerGlobalStatusGet(L7_char8 *data, L7_BOOL *fOverflow)
{
  L7_uint32 idx = 0;
  L7_uint32 len = 0;
  L7_WDM_ONDEMAND_LOC_STATUS_t  status = L7_WDM_ONDEMAND_LOC_STATUS_NOT_STARTED;
  L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t  device_type = L7_WDM_ONDEMAND_LOC_TARGET_TYPE_AP;
  L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t  radios = L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_SENTRY;
  L7_uint8 buildingNum = 0, floorNum = 0;
  L7_enetMacAddr_t mac;
  L7_uint32 numOfAps = 0, numOfDetAps = 0;
  L7_char8 macAddr[L7_MAC_STRING_LENGTH];
  L7_uint32 numOfDetBuildings, numOfDetFloors;
  L7_uint8 highestSigDetBuilding = 0, highestSigDetFloor = 0;

  *fOverflow = L7_FALSE;

  /*intToBytes(&data[idx],cnt);*/ /* 1st int holds total #, init with zero */
  /*idx+=4;*/

  /* Operational Radios */
  usmDbWdmOnDemandLocGlobalStatusManagedApRadioTypeGet(&radios);
  l7utilsIntToBytes(&data[idx],radios);
  idx+=4;

  /* Number of APs */
  usmDbWdmOnDemandLocGlobalStatusLocatorApsGet(&numOfAps);
  l7utilsIntToBytes(&data[idx],numOfAps);
  idx+=4;

  /* search-status */
  usmDbWdmOnDemandLocGlobalStatusGet(&status);
  l7utilsIntToBytes(&data[idx],status);
  idx+=4;

  usmDbWdmOnDemandLocGlobalStatusDevTypeGet(&device_type);
  l7utilsIntToBytes(&data[idx],device_type);
  idx+=4;

  usmDbWdmOnDemandLocGlobalStatusBuildingNumGet(&buildingNum);
  data[idx] = buildingNum;
  idx+=1;

  usmDbWdmOnDemandLocGlobalStatusFlrNumGet(&floorNum);
  data[idx] = floorNum;
  idx+=1;

  memset(&mac,0x00,sizeof(L7_enetMacAddr_t));
  usmDbWdmOnDemandLocGlobalStatusMacGet(&mac);

  memset(&macAddr,0x00,sizeof(macAddr));
  sprintf(macAddr, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  len = sizeof(macAddr);
  strncpy(&data[idx],macAddr,len);
  idx+=len;

  /* Number of Detecting APs */
  usmDbWdmOnDemandLocGlobalStatusDetectingApsGet(&numOfDetAps);
  l7utilsIntToBytes(&data[idx],numOfDetAps);
  idx+=4;

  /* Number of Detecting Buildings */
  usmDbWdmOnDemandLocGlobalStatusDetectBuildingsGet(&numOfDetBuildings);
  l7utilsIntToBytes(&data[idx],numOfDetBuildings);
  idx+=4;

  /* Number of Detecting Floors */
  usmDbWdmOnDemandLocGlobalStatusDetectFlrsGet(&numOfDetFloors);
  l7utilsIntToBytes(&data[idx],numOfDetFloors);
  idx+=4;

  usmDbWdmOnDemandLocGlobalStatusHighestDetectBuildingGet(&highestSigDetBuilding);
  data[idx] = highestSigDetBuilding;
  idx+=1;

  usmDbWdmOnDemandLocGlobalStatusHighestDetectFlrGet(&highestSigDetFloor);
  data[idx] = highestSigDetFloor;
  idx+=1;

  return idx;
}

/*********************************************************************
*
* @purpose  Retrieve On-Demand Location Trigger Data from USMDB
*
* @notes    none
* @returns  total number of bytes to send
*
* @end
*********************************************************************/
L7_uint32 nvLocationTriggerFloorStatusGet(L7_char8 *data, L7_BOOL *fOverflow)
{
  L7_uint32 idx = 0;
  L7_WDM_ONDEMAND_LOC_FLR_STATUS_t status = L7_WDM_ONDEMAND_LOC_FLR_STATUS_NOT_DETECTED;
  L7_WDM_ONDEMAND_LOC_SOLUTION_TYPE_t solType = L7_WDM_ONDEMAND_LOC_SOLUTION_TYPE_NO;
  L7_uint8 buildingNum = 0, floorNum = 0;
  L7_uint32 nextBuilding = 0, nextFloor = 0;
  L7_uint32 numOfAps = 0;
  devLocCoord_t coords;
  L7_uint32 radius = 0, sigma = 0;
  L7_uint32 numOfBuildings = 0, numOfFloors = 0;
  L7_uint32 storeBuildingIdx, storeFloorIdx;
  L7_RC_t rc = L7_SUCCESS;

  *fOverflow = L7_FALSE;

  /* Get Building List */
  numOfBuildings = 0;
  l7utilsIntToBytes(&data[idx],numOfBuildings);
  storeBuildingIdx = idx;
  idx+=4;

  buildingNum = 0;
  floorNum = 0;
  storeFloorIdx = 0;

  while(rc == L7_SUCCESS)
  {
    rc = usmDbWdmDevLocBldngFlrEntryNextGet(buildingNum,
                                           floorNum,
                                           &nextBuilding,
                                           &nextFloor);
    if(rc == L7_SUCCESS)
    {
      if(buildingNum != nextBuilding)
      {
        data[idx++] = nextBuilding;
        numOfFloors = 0;

        l7utilsIntToBytes(&data[idx],numOfFloors);
        storeFloorIdx = idx;
        idx+=4;
        numOfBuildings++;
      }

      data[idx++] = nextFloor;

      if (usmDbWdmOnDemandLocFlrStatusGet(nextBuilding, nextFloor, &status) == L7_SUCCESS)
      {
        l7utilsIntToBytes(&data[idx],status);
        idx+=4;
      }

      if (usmDbWdmOnDemandLocFlrStatusLocatorApsGet(nextBuilding, nextFloor, &numOfAps) == L7_SUCCESS)
      {
        l7utilsIntToBytes(&data[idx],numOfAps);
        idx+=4;
      }

      if (usmDbWdmOnDemandLocFlrStatusSolTypeGet(nextBuilding, nextFloor, &solType) == L7_SUCCESS)
      {
        l7utilsIntToBytes(&data[idx],solType);
        idx+=4;
      }

      if (usmDbWdmOnDemandLocFlrStatusCoordsGet(nextBuilding, nextFloor, &coords) == L7_SUCCESS)
      {
        l7utilsIntToBytes(&data[idx],coords.x);
        idx+=4;
        l7utilsIntToBytes(&data[idx],coords.y);
        idx+=4;
      }

      if(usmDbWdmOnDemandLocFlrStatusRadiusGet(nextBuilding, nextFloor, &radius) ==  L7_SUCCESS)
      {
        l7utilsIntToBytes(&data[idx],radius);
        idx+=4;
      }

      if (usmDbWdmOnDemandLocFlrStatusSigmaGet(nextBuilding, nextFloor, &sigma) == L7_SUCCESS)
      {
        l7utilsIntToBytes(&data[idx],sigma);
        idx+=4;
      }

      numOfFloors++;
    }

    buildingNum = nextBuilding;
    floorNum = nextFloor;

    if(storeFloorIdx != 0)
      l7utilsIntToBytes(&data[storeFloorIdx],numOfFloors);
  }

  l7utilsIntToBytes(&data[storeBuildingIdx],numOfBuildings);

  return idx;
}


/*********************************************************************
*
* @purpose  Update Location Trigger Data
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
int nvLocationTriggerDataSet(wdmNVDevLocTriggerData_s *nvOLT)
{
  int nBytes = 4;
  /* set building number */
  usmDbWdmOnDemandLocTrigStatusBuildingNumSet(nvOLT->building);
  usmDbWdmOnDemandLocTrigStatusFlrNumSet(nvOLT->floor);
  usmDbWdmOnDemandLocTrigStatusMacSet(nvOLT->mac);

   /* set device type */
   if(nvOLT->devType == 0)
   {
     usmDbWdmOnDemandLocTrigStatusDevTypeSet(L7_WDM_ONDEMAND_LOC_TARGET_TYPE_AP);
   }
   else
   {
     usmDbWdmOnDemandLocTrigStatusDevTypeSet(L7_WDM_ONDEMAND_LOC_TARGET_TYPE_CLIENT);
   }

   /* set operational radio usage */
   if(nvOLT->useRadios == 0)
   {
      usmDbWdmOnDemandLocTrigStatusManagedApRadioTypeSet(L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_SENTRY);
   }
   else
   {
     usmDbWdmOnDemandLocTrigStatusManagedApRadioTypeSet(L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_BOTH);
   }

  if(usmDbWdmOnDemandLocTrigStatusLocatorApsFindUpdate(&(nvOLT->numOfAPs)) != L7_SUCCESS)
  {
    nvOLT->numOfAPs = 0;
  }

  return nBytes;
}

/*********************************************************************
*
* @purpose  Start Location Search
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
int nvLocationTriggerStartSearch(wdmNVDevLocTriggerData_s *nvOLT)
{

  int nBytes = 4;

  usmDbWdmOnDemandLocTrigStatusGet(&(nvOLT->searchStatus));

  if(nvOLT->searchStatus == L7_WDM_ONDEMAND_LOC_TRIG_STATUS_NOT_INPROGRESS)
  {
    usmDbWdmOnDemandLocProcedureInitiate();
  }

  return nBytes;
}

/*********************************************************************
*
* @purpose  Update (New or Edit) NV graph component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvUpdateGC(L7_uchar8 index, wdmNVGraphCfgData_t nvGC)
{

  usmDbWdmNVGraphEntryAddUpdate(index,&nvGC);

  /* DEBUG */
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected graph update. bldgNum =%d flrNum=%d name=%s id=%d img=%s mea=%d .\n",
    nvGC.graphLoc.bldgNum,nvGC.graphLoc.flrNum,nvGC.graphName,index,nvGC.fileName,nvGC.imageMeasurement);
}

/*********************************************************************
*
* @purpose  Delete NV graph component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvDeleteGC(L7_uchar8 index)
{
  usmDbWdmNVGraphEntryDelete(index);

  /* DEBUG */
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected graph delete. id=%d.\n",index);
}

/*********************************************************************
*
* @purpose  Update local WS component coordinates
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvUpdateLocalWS(L7_IP_ADDR_t ip, wdmNVCoordCfgData_t nvCfg)
{
  L7_char8 ips[L7_MAC_STRING_LENGTH+1];

  usmDbWdmNVWSCoordSet(&nvCfg);

  /* DEBUG */
  memset(&ips,0x00,sizeof(ips));
  usmDbInetNtoa(ip,ips);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected local switch update. ip=%s id=%d x=%d y=%d.\n",
    ips,nvCfg.graphIndex,nvCfg.x,nvCfg.y);
}

/*********************************************************************
*
* @purpose  Update peer WS component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvUpdatePeerWS(L7_IP_ADDR_t ip, wdmNVCoordCfgData_t nvCfg)
{
  L7_char8 ips[L7_MAC_STRING_LENGTH+1];

  usmDbWdmNVPeerSwitchEntryAddUpdate(ip,&nvCfg);

  /* DEBUG */
  memset(&ips,0x00,sizeof(ips));
  usmDbInetNtoa(ip,ips);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected peer switch update. ip=%s id=%d x=%d y=%d.\n",
    ips,nvCfg.graphIndex,nvCfg.x,nvCfg.y);
}

/*********************************************************************
*
* @purpose  Delete peer WS component coordinates
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvDeletePeerWS(L7_IP_ADDR_t ip)
{
  L7_char8 ips[L7_MAC_STRING_LENGTH+1];

  usmDbWdmNVPeerSwitchEntryDelete(ip);

  /* DEBUG */
  memset(&ips,0x00,sizeof(ips));
  usmDbInetNtoa(ip,ips);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected peer switch delete. ip=%s.\n",ips);
}

/*********************************************************************
*
* @purpose  Update AP component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvUpdateAP(L7_enetMacAddr_t mac, wdmNVCoordCfgData_t nvCfg)
{
  L7_char8 sMac[L7_MACADDR_SIZE+1];

  usmDbWdmNVAPEntryAddUpdate(mac,&nvCfg);

  /* DEBUG */
  memset(&sMac,0x00,sizeof(sMac));
  sprintf(sMac, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected AP update. mac=%s id=%d x=%d y=%d.\n",
    sMac,nvCfg.graphIndex,nvCfg.x,nvCfg.y);
}

/*********************************************************************
*
* @purpose  Delete AP component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvDeleteAP(L7_enetMacAddr_t mac)
{
  L7_char8 sMac[L7_MACADDR_SIZE+1];

  usmDbWdmNVAPEntryDelete(mac);

  /* DEBUG */
  memset(&sMac,0x00,sizeof(sMac));
  sprintf(sMac, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected AP delete. mac=%s.\n",sMac);
}

/*********************************************************************
*
* @purpose  Reset AP component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvResetAP(L7_enetMacAddr_t mac)
{
  L7_char8 sMac[L7_MACADDR_SIZE+1];
  L7_RC_t rc;

  rc = usmDbWdmManagedAPReset(mac);

  /* DEBUG */
  memset(&sMac,0x00,sizeof(sMac));
  sprintf(sMac, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected Reset AP command. mac=%s rc=%d.\n",sMac,rc);
}

/*********************************************************************
*
* @purpose  Manage AP component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvManageAP(L7_enetMacAddr_t mac)
{
  L7_char8 sMac[L7_MACADDR_SIZE+1];
  L7_RC_t rc = L7_FAILURE;

  if (usmDbWdmAPEntryGet(mac) != L7_SUCCESS)
  {
    if (usmDbWdmAPEntryAdd(mac) == L7_SUCCESS)
    {
      rc = usmDbWdmAPModeSet(mac,L7_WDM_AP_WS_MANAGED);
    }
  }

  /* DEBUG */
  memset(&sMac,0x00,sizeof(sMac));
  sprintf(sMac, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected Manage AP command. mac=%s rc=%d.\n",sMac,rc);
}

/*********************************************************************
*
* @purpose  Acknowledge AP component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvAcknowledgeAP(L7_enetMacAddr_t mac)
{
  L7_char8 sMac[L7_MACADDR_SIZE+1];
  L7_RC_t rc = L7_FAILURE;

  rc = usmDbWdmRogueAPStateClear(mac);

  /* DEBUG */
  memset(&sMac,0x00,sizeof(sMac));
  sprintf(sMac, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected Acknowledge AP command. mac=%s rc=%d.\n",sMac,rc);
}

/*********************************************************************
*
* @purpose  Disassociate CS component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvDisassociateCS(L7_enetMacAddr_t mac)
{
  L7_char8 sMac[L7_MACADDR_SIZE+1];
  L7_RC_t rc;

  rc = usmDbWdmAssocClientForceDisassoc(mac);

  /* DEBUG */
  memset(&sMac,0x00,sizeof(sMac));
  sprintf(sMac, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server detected Disassociate CS command. mac=%s rc=%d.\n",sMac,rc);
}

/*********************************************************************
*
* @purpose  Update Detected client component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvUpdateDetectedClient(L7_enetMacAddr_t mac, wdmNVCoordCfgData_t nvCfg)
{
  L7_char8 macStr[L7_MACADDR_SIZE+1];

  usmDbWdmNVDCSEntryAddUpdate(mac,&nvCfg);

  /* DEBUG */
  memset(&macStr,0x00,sizeof(macStr));
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: detected client update from server. mac=%s id=%d x=%d y=%d.\n",
    macStr,nvCfg.graphIndex,nvCfg.x,nvCfg.y);
}

/*********************************************************************
*
* @purpose  Delete Detected client component
*
* @notes    none
* @returns
*
* @end
*********************************************************************/
void nvDeleteDetectedClient(L7_enetMacAddr_t mac)
{
  L7_char8 macStr[L7_MACADDR_SIZE+1];

  usmDbWdmNVDCSEntryDelete(mac);

  /* DEBUG */
  memset(&macStr,0x00,sizeof(macStr));
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac.addr[0], mac.addr[1],
          mac.addr[2], mac.addr[3],
          mac.addr[4], mac.addr[5]);
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: detected client delete from server. mac=%s.\n",macStr);
}

/*********************************************************************
*
* @purpose  Poll NVT for events
*
* @notes    none
* @returns  single event
*
* @end
*********************************************************************/
L7_uint32 nvPoll(L7_short16 que, L7_char8 *data)
{
  wnvEventData_t evt;
  wdmNVCoordCfgData_t nvCfg;
  L7_uint32 idx = 0;
  L7_uint32 len = 0;
  L7_char8 aMac[L7_MACADDR_SIZE+1];
  L7_char8 cMac[L7_MACADDR_SIZE+1];
  L7_char8 ips[L7_MAC_STRING_LENGTH+1];
  L7_IP_ADDR_t ip = 0;
  L7_enetMacAddr_t apMacAdd;
  L7_char8 mac[L7_MACADDR_SIZE+1];
  L7_char8 location[L7_WDM_MAX_LOCATION_NAME+1];
  L7_char8 GCId;
  L7_uint32 X;
  L7_uint32 Y;
  L7_uint32 mode = 0;
  L7_uchar8 radio1Power;
  L7_uchar8 radio2Power;
  L7_uchar8 radio1Channel;
  L7_uchar8 radio2Channel;
  L7_uint32 radio1SentryMode;
  L7_uint32 radio2SentryMode;
  L7_WDM_RADIO_PHY_MODE_t radio1PhyMode;
  L7_WDM_RADIO_PHY_MODE_t radio2PhyMode;
  L7_WDM_MANAGED_AP_STATUS_t status;
  L7_WDM_AP_STATUS_t ap_rfscan_status;
  L7_WDM_MANAGED_AP_STATUS_t tmpStatus;
  L7_uint32 mapSwitchIpAddr = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 val;
  L7_WDM_ACTION_STATUS_t cfgStatus;
  L7_uint32 profileId;
  L7_WDM_DETECTED_CLIENT_STATUS_t clientStatus;
  L7_WDM_CLIENT_AUTHENTICATION_STATUS_t clientAuthenticated;
  L7_char8         clientName[L7_WDM_CLIENT_USERNAME_MAX+1];
  L7_uchar8        authRSSI;
  L7_uchar8        channel;
  wdmNVUserData_t nvUD;
  L7_BOOL fOverflow;
  L7_uchar8 checkLocation = 0;
  L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t  trigStatus = L7_WDM_ONDEMAND_LOC_TRIG_STATUS_NOT_INPROGRESS;

  l7utilsIntToBytes(&data[0],-1); /* indicates non event */
  idx=4;
  if (L7_SUCCESS == usmDbWnvEventGet(que,&evt))
  {
    /* DEBUG */
    WIRELESS_DLOG(WD_LEVEL_NV_INFO,
      "NV: Server event notification detected. evt=%d.\n",evt.eventId);

    wdmNVUserDataGet(&nvUD);

    switch (evt.eventId)
    {
      case L7_WNV_EVENT_LOCAL_WS_UPDATE:
      case L7_WNV_EVENT_PEER_WS_ADD:
      case L7_WNV_EVENT_PEER_WS_UPDATE:
      case L7_WNV_EVENT_PEER_WS_DELETE:
        if (evt.eventId==L7_WNV_EVENT_LOCAL_WS_UPDATE)
        {
          usmDbWdmWSIPAddrGet(&ip);
        }
        else
        {
          ip = evt.wdmIndex.ip;
        }

        /* Don't send if invalid IP */
        if (ip==0)
        {
          l7utilsIntToBytes(&data[0],-1); /* indicates non event */
        }
        else
        {
          l7utilsIntToBytes(&data[0],evt.eventId);
          if (evt.eventId==L7_WNV_EVENT_LOCAL_WS_UPDATE)
          {
            if (L7_SUCCESS != wdmNVWSCoordGet(&nvCfg)) {
              nvCfg.graphIndex = 0;
              nvCfg.x = 0;
              nvCfg.y = 0;
            }
          }
          else
          {
            if (L7_SUCCESS != usmDbWdmNVPeerSwitchEntryGet(ip,&nvCfg))
            {
              nvCfg.graphIndex = 0;
              nvCfg.x = 0;
              nvCfg.y = 0;
            }
          }
          memset(&ips,0x00,sizeof(ips));
          usmDbInetNtoa(ip,ips);
          len = strlen(ips);
          l7utilsIntToBytes(&data[idx],len);
          idx+=4;
          strncpy(&data[idx],ips,len);
          idx+=len;
          data[idx] = nvCfg.graphIndex;
          idx+=1;
          l7utilsIntToBytes(&data[idx],nvCfg.x);
          idx+=4;
          l7utilsIntToBytes(&data[idx],nvCfg.y);
          idx+=4;

          /* DEBUG */
          WIRELESS_DLOG(WD_LEVEL_NV_INFO,
            "NV: Server event, switch. evt=%d ip=%s id=%d x=%d y=%d.\n",
            evt.eventId,ips,nvCfg.x,nvCfg.y);
        }

        break;
      case L7_WNV_EVENT_MANAGED_AP_ADD:
      case L7_WNV_EVENT_MANAGED_AP_UPDATE:
      case L7_WNV_EVENT_MANAGED_AP_DELETE:
        if (nvUD.showMAPs==0) /* Don't process if show Managed APs disabled */
        {
          l7utilsIntToBytes(&data[0],-1); /* indicates non event */
        }
        else
        {
          l7utilsIntToBytes(&data[0],evt.eventId);
          memset(&mac,0x00,sizeof(mac));
          sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                evt.wdmIndex.macAddr.addr[0], evt.wdmIndex.macAddr.addr[1],
                evt.wdmIndex.macAddr.addr[2], evt.wdmIndex.macAddr.addr[3],
                evt.wdmIndex.macAddr.addr[4], evt.wdmIndex.macAddr.addr[5]);
          if (L7_SUCCESS != usmDbWdmManagedAPStatusGet(evt.wdmIndex.macAddr,&status))
          {
            status = 0;
          }
          /* Add support for ip address of managing switch */
          if (usmDbWdmManagedAPSwitchIPAddrGet(evt.wdmIndex.macAddr, &mapSwitchIpAddr) != L7_SUCCESS)
          {
            mapSwitchIpAddr = 0;
          }

          val = 0;
          memset(buf,0x00,L7_CLI_MAX_STRING_LENGTH);
          if (L7_SUCCESS != usmDbWdmManagedAPCfgStatusGet(evt.wdmIndex.macAddr,&cfgStatus,&val,buf))
          {
            cfgStatus = L7_WDM_ACTION_FAILURE;
          }
          memset(&location,0x00,sizeof(location));
          usmDbWdmManagedAPLocationGet(evt.wdmIndex.macAddr,location);
          if (L7_SUCCESS != usmDbWdmAPValidationGet(&mode))
          {
            mode = 0;
          }
          if (L7_SUCCESS != usmDbWdmManagedAPProfileIdGet(evt.wdmIndex.macAddr,&profileId))
          {
            profileId = 0;
          }
          if (L7_SUCCESS != usmDbWdmManagedAPRadioTxPwrGet(evt.wdmIndex.macAddr,1,&radio1Power))
          {
            radio1Power = 0;
          }
          if (L7_SUCCESS != usmDbWdmManagedAPRadioChannelGet(evt.wdmIndex.macAddr,1,&radio1Channel))
          {
            radio1Channel = 0;
          }
          if (L7_SUCCESS != usmDbWdmManagedAPRadioScanSentryModeGet(evt.wdmIndex.macAddr,1,&radio1SentryMode))
          {
            radio1SentryMode = 0;
          }
          if (L7_SUCCESS != usmDbWdmManagedAPRadioPhyModeGet(evt.wdmIndex.macAddr,1,&radio1PhyMode))
          {
            radio1PhyMode = 0;
          }
          if (L7_SUCCESS != usmDbWdmManagedAPRadioTxPwrGet(evt.wdmIndex.macAddr,2,&radio2Power))
          {
            radio2Power = 0;
          }
          if (L7_SUCCESS != usmDbWdmManagedAPRadioChannelGet(evt.wdmIndex.macAddr,2,&radio2Channel))
          {
            radio2Channel = 0;
          }
          if (L7_SUCCESS != usmDbWdmManagedAPRadioScanSentryModeGet(evt.wdmIndex.macAddr,2,&radio2SentryMode))
          {
            radio2SentryMode = 0;
          }
          if (L7_SUCCESS != usmDbWdmManagedAPRadioPhyModeGet(evt.wdmIndex.macAddr,2,&radio2PhyMode))
          {
            radio2PhyMode = 0;
          }
          if (L7_SUCCESS == usmDbWdmNVAPEntryGet(evt.wdmIndex.macAddr,&nvCfg))
          {
            GCId = nvCfg.graphIndex;
            X    = nvCfg.x;
            Y    = nvCfg.y;
          }
          else
          {
            GCId = 0;
            X    = 0;
            Y    = 0;
          }
          len = strlen(mac);
          l7utilsIntToBytes(&data[idx],len);
          idx+=4;
          strncpy(&data[idx],mac,len);
          idx+=len;
          len = strlen(location);
          l7utilsIntToBytes(&data[idx],len);
          idx+=4;
          strncpy(&data[idx],location,len);
          idx+=len;
          data[idx] = GCId;
          idx+=1;
          l7utilsIntToBytes(&data[idx],X);
          idx+=4;
          l7utilsIntToBytes(&data[idx],Y);
          idx+=4;
          l7utilsIntToBytes(&data[idx],mode);
          idx+=4;
          data[idx] = radio1Power;
          idx+=1;
          data[idx] = radio1Channel;
          idx+=1;
          l7utilsIntToBytes(&data[idx],radio1SentryMode);
          idx+=4;
          l7utilsIntToBytes(&data[idx],radio1PhyMode);
          idx+=4;
          data[idx] = radio2Power;
          idx+=1;
          data[idx] = radio2Channel;
          idx+=1;
          l7utilsIntToBytes(&data[idx],radio2SentryMode);
          idx+=4;
          l7utilsIntToBytes(&data[idx],radio2PhyMode);
          idx+=4;
          l7utilsIntToBytes(&data[idx],cfgStatus);
          idx+=4;
          l7utilsIntToBytes(&data[idx],status);
          idx+=4;

          memset(&ips,0x00,sizeof(ips));  /* Managing switch ip address */
          if (mapSwitchIpAddr != 0)
          {
            usmDbInetNtoa(mapSwitchIpAddr,ips);
            len = strlen(ips);
          }
          else
          {
            len = 0;
          }
          l7utilsIntToBytes(&data[idx],len);
          idx+=4;
          if (len > 0)
          {
            strncpy(&data[idx],ips,len);
            idx+=len;
          }

          l7utilsIntToBytes(&data[idx],profileId);
          idx+=4;

          /* DEBUG */
          WIRELESS_DLOG(WD_LEVEL_NV_INFO,
            "NV: Server event, managed AP. evt=%d  mac=%s id=%d x=%d y=%d m=%d r1p=%d r1c=%d r1s=%d r1m=%d r2p=%d r2c=%d r2s=%d r2m=%d s=%d c=%d.\n",
            evt.eventId,mac,GCId,X,Y,mode,
            radio1Power,radio1Channel,radio1SentryMode,radio1PhyMode,
            radio2Power,radio2Channel,radio2SentryMode,radio2PhyMode,status,cfgStatus);
        }
        break;
      case L7_WNV_EVENT_RF_SCAN_AP_ADD:
      case L7_WNV_EVENT_RF_SCAN_AP_UPDATE:
      case L7_WNV_EVENT_RF_SCAN_AP_DELETE:/* aged out */
        if (nvUD.showRAPs==0) /* Don't process if show Rogue APs disabled */
        {
          l7utilsIntToBytes(&data[0],-1); /* indicates non event */
        }
        else
        {
          l7utilsIntToBytes(&data[0],evt.eventId);
          ap_rfscan_status           = 0;
          mode             = 0;
          radio1Power      = 0;
          radio2Power      = 0;
          radio2Channel    = 0;
          radio1Channel    = 0;
          radio1SentryMode = 0;
          radio2SentryMode = 0;
          radio1PhyMode    = 0;
          radio2PhyMode    = 0;
          cfgStatus        = 0;
          profileId        = 0;
          usmDbWdmRFScanStatusGet(evt.wdmIndex.macAddr,&ap_rfscan_status);
          if (L7_WDM_AP_STATUS_MANAGED!=ap_rfscan_status) /* filter out managed APs */
          {
            /*
            ** Check this MAC against the base AP MAC. If its not the base,
            ** determine if the AP (base) is managed, if so skip the update
            */
            if (evt.eventId!=L7_WNV_EVENT_RF_SCAN_AP_DELETE) /* Always allow delete */
            {
              memset(&apMacAdd,0x00,sizeof(apMacAdd));
              usmDbWdmRFScanAPMacAddrGet(evt.wdmIndex.macAddr,&apMacAdd);
              if (0 != memcmp(&(evt.wdmIndex.macAddr),&apMacAdd,sizeof(L7_enetMacAddr_t)))
              {
                if (usmDbWdmManagedAPStatusGet(apMacAdd,&tmpStatus) == L7_SUCCESS)
                {
                  l7utilsIntToBytes(&data[0],-1); /* indicates non event */
                  break;
                }
              }
            }

            memset(&location,0x00,sizeof(location));
            usmDbWdmManagedAPLocationGet(evt.wdmIndex.macAddr,location);

            /* For now, display 1 radio and its channel */
            usmDbWdmRFScanChannelGet(evt.wdmIndex.macAddr,&radio1Channel);

            if (L7_SUCCESS == usmDbWdmNVAPEntryGet(evt.wdmIndex.macAddr,&nvCfg))
            {
              GCId = nvCfg.graphIndex;
              X    = nvCfg.x;
              Y    = nvCfg.y;
            }
            else
            {
              GCId = 0;
              X    = 0;
              Y    = 0;
            }
            memset(&mac,0x00,sizeof(mac));
            sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                evt.wdmIndex.macAddr.addr[0], evt.wdmIndex.macAddr.addr[1],
                evt.wdmIndex.macAddr.addr[2], evt.wdmIndex.macAddr.addr[3],
                evt.wdmIndex.macAddr.addr[4], evt.wdmIndex.macAddr.addr[5]);

            len = strlen(mac);
            l7utilsIntToBytes(&data[idx],len);
            idx+=4;
            strncpy(&data[idx],mac,len);
            idx+=len;
            len = strlen(location);
            l7utilsIntToBytes(&data[idx],len);
            idx+=4;
            strncpy(&data[idx],location,len);
            idx+=len;
            data[idx] = GCId;
            idx+=1;
            l7utilsIntToBytes(&data[idx],X);
            idx+=4;
            l7utilsIntToBytes(&data[idx],Y);
            idx+=4;
            l7utilsIntToBytes(&data[idx],mode);
            idx+=4;
            data[idx] = radio1Power;
            idx+=1;
            data[idx] = radio1Channel;
            idx+=1;
            l7utilsIntToBytes(&data[idx],radio1SentryMode);
            idx+=4;
            l7utilsIntToBytes(&data[idx],radio1PhyMode);
            idx+=4;
            data[idx] = radio2Power;
            idx+=1;
            data[idx] = radio2Channel;
            idx+=1;
            l7utilsIntToBytes(&data[idx],radio2SentryMode);
            idx+=4;
            l7utilsIntToBytes(&data[idx],radio2PhyMode);
            idx+=4;
            l7utilsIntToBytes(&data[idx],cfgStatus);
            idx+=4;
            l7utilsIntToBytes(&data[idx],ap_rfscan_status);
            idx+=4;
            len = 0;  /* Placeholder for managing switch ip address */
            l7utilsIntToBytes(&data[idx],len);
            idx+=4;
            l7utilsIntToBytes(&data[idx],profileId);
            idx+=4;

            /* DEBUG */
            WIRELESS_DLOG(WD_LEVEL_NV_INFO,
              "NV: Server event, un-managed AP. evt=%d mac=%s id=%d x=%d y=%d r1c=%d s=%d.\n",
              evt.eventId,mac,GCId,X,Y,radio1Channel,ap_rfscan_status);
          }
          else
          {
            l7utilsIntToBytes(&data[0],-1);
            /* DEBUG */
            WIRELESS_DLOG(WD_LEVEL_NV_INFO,
              "NV: Server event discarded, RFScan managed AP.\n");
          }
        }
        break;
      case L7_WNV_EVENT_CLIENT_ADD:
      case L7_WNV_EVENT_CLIENT_UPDATE:
      case L7_WNV_EVENT_CLIENT_DELETE:
        if (nvUD.showMAPClients==0) /* Don't process if show Clients disabled */
        {
          l7utilsIntToBytes(&data[0],-1); /* indicates non event */
        }
        else
        {
          if (usmDbWdmNVAPEntryGet(evt.wdmIndex.client.apMacAddr,&nvCfg) != L7_SUCCESS)
          {
            nvCfg.graphIndex = 0;
            nvCfg.x          = 0;
            nvCfg.y          = 0;
          }
          if (0<nvCfg.graphIndex) /* Send Client only if the associated AP is graphed */
          {
            memset(&cMac,0x00,sizeof(cMac));
            sprintf(cMac, "%02X:%02X:%02X:%02X:%02X:%02X",
              evt.wdmIndex.client.macAddr.addr[0], evt.wdmIndex.client.macAddr.addr[1],
              evt.wdmIndex.client.macAddr.addr[2], evt.wdmIndex.client.macAddr.addr[3],
              evt.wdmIndex.client.macAddr.addr[4], evt.wdmIndex.client.macAddr.addr[5]);
            memset(&aMac,0x00,sizeof(aMac));
            sprintf(aMac, "%02X:%02X:%02X:%02X:%02X:%02X",
              evt.wdmIndex.client.apMacAddr.addr[0], evt.wdmIndex.client.apMacAddr.addr[1],
              evt.wdmIndex.client.apMacAddr.addr[2], evt.wdmIndex.client.apMacAddr.addr[3],
              evt.wdmIndex.client.apMacAddr.addr[4], evt.wdmIndex.client.apMacAddr.addr[5]);
            memset(&ips,0x00,sizeof(ips));
            if (usmDbWdmAssocClientTunnelIPAddrGet(evt.wdmIndex.client.macAddr,&ip) == L7_SUCCESS)
            {
              usmDbInetNtoa(ip,ips);
            }
            l7utilsIntToBytes(&data[0],evt.eventId);
            data[idx] = nvCfg.graphIndex;
            idx+=1;
            len = strlen(aMac);
            l7utilsIntToBytes(&data[idx],len);
            idx+=4;
            strncpy(&data[idx],aMac,len);
            idx+=len;
            len = strlen(cMac);
            l7utilsIntToBytes(&data[idx],len);
            idx+=4;
            strncpy(&data[idx],cMac,len);
            idx+=len;
            len = strlen(ips);
            l7utilsIntToBytes(&data[idx],len);
            idx+=4;
            strncpy(&data[idx],ips,len);
            idx+=len;
            data[idx] = evt.wdmIndex.client.apRadioIf;
            idx+=1;

            /* DEBUG */
            WIRELESS_DLOG(WD_LEVEL_NV_INFO,
              "NV: Server event, client. evt=%d id=%d AP mac=%s Client mac=%s Client ip=%s.\n",
              evt.eventId,nvCfg.graphIndex,aMac,cMac,ips);
          }
        }
        break;
      case L7_WNV_EVENT_DETECTED_CLIENT_ADD:
      case L7_WNV_EVENT_DETECTED_CLIENT_UPDATE:
      case L7_WNV_EVENT_DETECTED_CLIENT_DELETE:
          if (nvUD.showDetectedClients==0) /* Don't process if show detected clients disabled */
          {
            l7utilsIntToBytes(&data[0],-1); /* indicates non event */
            break;
          }

          clientStatus = 0;
          clientAuthenticated = 0;
          memset(clientName,0,(L7_WDM_CLIENT_USERNAME_MAX+1));
          authRSSI = 0;
          channel = 0;

          if (L7_SUCCESS == usmDbWdmNVDCSEntryGet(evt.wdmIndex.macAddr,&nvCfg))
          {
            GCId = nvCfg.graphIndex;
            X    = nvCfg.x;
            Y    = nvCfg.y;
          }
          else
          {
            GCId = 0;
            X    = 0;
            Y    = 0;
          }

          usmDbWdmDetectedClientStatusGet(evt.wdmIndex.macAddr,&clientStatus);
          usmDbWdmDetectedClientAuthStatusGet(evt.wdmIndex.macAddr,&clientAuthenticated);
          usmDbWdmDetectedClientNameGet(evt.wdmIndex.macAddr,clientName);
          usmDbWdmDetectedClientAuthRSSIGet(evt.wdmIndex.macAddr,&authRSSI);
          usmDbWdmDetectedClientChannelGet(evt.wdmIndex.macAddr,&channel);

          memset(&cMac,0x00,sizeof(cMac));
          sprintf(cMac, "%02X:%02X:%02X:%02X:%02X:%02X",
                  evt.wdmIndex.macAddr.addr[0], evt.wdmIndex.macAddr.addr[1],
                  evt.wdmIndex.macAddr.addr[2], evt.wdmIndex.macAddr.addr[3],
                  evt.wdmIndex.macAddr.addr[4], evt.wdmIndex.macAddr.addr[5]);

          l7utilsIntToBytes(&data[0],evt.eventId);
          len = strlen(cMac);
          l7utilsIntToBytes(&data[idx],len);
          idx+=4;
          strncpy(&data[idx],cMac,len);
          idx+=len;
          data[idx] = GCId;
          idx+=1;
          l7utilsIntToBytes(&data[idx],X);
          idx+=4;
          l7utilsIntToBytes(&data[idx],Y);
          idx+=4;
          l7utilsIntToBytes(&data[idx],clientStatus);
          idx+=4;
          l7utilsIntToBytes(&data[idx],clientAuthenticated);
          idx+=4;
          len = strlen(clientName);
          l7utilsIntToBytes(&data[idx],len);
          idx+=4;
          strncpy(&data[idx],clientName,len);
          idx+=len;
          data[idx] = authRSSI;
          idx+=1;
          data[idx] = channel;
          idx+=1;

          /* DEBUG */
          WIRELESS_DLOG(WD_LEVEL_NV_INFO,
            "NV: Server event, detected client. evt=%d id=%d detected Client mac=%s, client Name =%s, client status =%d, auth status =%d, rssi =%d, channel =%d.\n",
            evt.eventId,nvCfg.graphIndex,cMac,clientName,clientStatus,clientAuthenticated,authRSSI,channel);
          break;
      case L7_WNV_EVENT_BLDG_ADD:
      case L7_WNV_EVENT_BLDG_DELETE:
          l7utilsIntToBytes(&data[0],evt.eventId);
          l7utilsIntToBytes(&data[idx],evt.bldgNum);
          idx+=4;
          /* DEBUG */
            WIRELESS_DLOG(WD_LEVEL_NV_INFO,
              "NV: Server event, building. evt=%d building num=%d.\n",
              evt.eventId,evt.bldgNum);
          break;
      case L7_WNV_EVENT_BLDG_FLR_ADD:
      case L7_WNV_EVENT_BLDG_FLR_DELETE:
          l7utilsIntToBytes(&data[0],evt.eventId);
          l7utilsIntToBytes(&data[idx],evt.bldgNum);
          idx+=4;
          l7utilsIntToBytes(&data[idx],evt.flrNum);
          idx+=4;

          /* DEBUG */
            WIRELESS_DLOG(WD_LEVEL_NV_INFO,
              "NV: Server event, building floor. evt=%d building num=%d, floor num=%d.\n",
              evt.eventId,evt.bldgNum,evt.flrNum);
          break;
          case L7_WNV_EVENT_LOC_GLOBAL_STATUS_UPDATE:
            l7utilsIntToBytes(&data[0],evt.eventId);
            idx += nvLocationTriggerGlobalStatusGet(data+4, &fOverflow);

            WIRELESS_DLOG(WD_LEVEL_NV_INFO,
              "NV: Server event, Location Trigger evt=%d\n", evt.eventId);
            break;

          case L7_WNV_EVENT_LOC_TRIG_STATUS_UPDATE:
            l7utilsIntToBytes(&data[0],evt.eventId);

            usmDbWdmNVCheckLocationGet(&trigStatus);
            if(trigStatus == L7_WDM_ONDEMAND_LOC_TRIG_STATUS_NOT_INPROGRESS)
              checkLocation = 0;
            else
              checkLocation = 1;

            data[idx] = checkLocation;
            idx+=1;

            WIRELESS_DLOG(WD_LEVEL_NV_INFO,
              "NV: Server event, Location Trigger evt=%d trigger status =%d.\n",
              evt.eventId,checkLocation);
            break;

          case L7_WNV_EVENT_LOC_FLOOR_STATUS_UPDATE:
            l7utilsIntToBytes(&data[0],evt.eventId);
            idx += nvLocationTriggerFloorStatusGet(data+4, &fOverflow);

            WIRELESS_DLOG(WD_LEVEL_NV_INFO,
              "NV: Server event, Location Trigger evt=%d\n", evt.eventId);
            break;
    }
  }

 return idx;
}

/*********************************************************************
*
* @purpose  Post LostMessage event so client can notify UI via poll
*
* @notes    This event is very unlikely if the USMWEB_BUFFER_SIZE
*           is a large number as it is intended to be. It is expected
*           to be defined in the area of (32*1024) bytes. This is
*           enough for ~ 320 Access Points and 1280 Clients
* @returns  single event
*
* @end
*********************************************************************/
void nvLostMessageEventAdd()
{
  wnvEventData_t wnvEvent;
  wnvEvent.eventId = L7_WNV_EVENT_LOST_MESSAGES;
  usmDbWnvEventAdd(&wnvEvent);
}

#endif /* L7_WIRELESS_PACKAGE*/

/*********************************************************************
*
* @purpose  web java task
*
* @notes    none
*
* @end
*********************************************************************/
void L7_web_java_task ()
{
  L7_BOOL    bTimeZero = L7_TRUE,flagCMD=L7_FALSE;
  L7_int32   num_bytes_sent = 0;
  L7_int32   num_bytes_recv;
  L7_int32   connfd;
  L7_int32   sockfd = 0;
  L7_int32   reuse_addr  = 1;
  L7_int32   maxfd = 0, maxi = 0, i;
  L7_int32   nready;
  L7_int32   client[JAVA_MAX_CLIENTS];
  L7_uint32  java_mode_current;
  L7_uint32  java_mode_previous;
  L7_uint32  unit;        /* Unit ID */
  L7_uint32  cmd=0;
  L7_uint32  datalength;
  L7_uint32  len=0;
  L7_uint32  idx=0;
  L7_char8   jMsg[256];
  L7_char8  *cmdPtr;
#ifdef L7_WIRELESS_PACKAGE /* BEGIN NETWORK VISUALIZATION FOR WIRELESS */
  L7_char8 ips[L7_MAC_STRING_LENGTH+1];
  L7_char8 mac[L7_MAC_STRING_LENGTH+1];
  L7_enetMacAddr_t    macAdd;
  L7_IP_ADDR_t        ip;
  L7_uchar8           index;
  wdmNVUserData_t     nvUD;
  wdmNVGraphCfgData_t nvGC;
  wdmNVCoordCfgData_t nvCfg;
  wdmNVDevLocTriggerData_s nvOLT;
  L7_int32            nvEvents = 0;
  L7_BOOL             fOverflow;
  L7_BOOL             clusterController = L7_FALSE;
  L7_char8            cookie[256];
  L7_int32            sVal;
  L7_uint32           bldgNum = 0;
  L7_uint32           flrNum = 0;
#endif /* L7_WIRELESS_PACKAGE*/
/*if(usmDbIsAChassisPlatform()==L7_TRUE)
{*/
 L7_uint32 buffer_chassis[L7_MAX_PORT_COUNT + USMWEB_NUM_SUPPORTED_SFP_INTERFACES + USMWEB_DATA_SIZE];  /* The size of buffer[] must match */
  /* inputData[] in MapApplet.java   */
L7_char8  *pDat;
/*}*/
L7_char8 buffer[USMWEB_BUFFER_SIZE];  /* The size of buffer[] must match */

 /* inputData[] in MapApplet.java   */

#ifdef L7_JAVA_V4_BIND
  struct     sockaddr_in addr;       /* an Internet endpoint address */
  L7_int32  srvsock = 0;             /* HTTP listening socket */
#endif
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  struct sockaddr_in6 addr6;     /* an Internet endpoint address */
  L7_int32  srvsock6 = 0;        /* HTTP listening socket */
#endif

#ifndef _L7_OS_LINUX_
  struct fd_set allset;
  struct fd_set readset;
#else
  fd_set allset;
  fd_set readset;
#endif /* _L7_OS_LINUX_ */

  /* Initialize the java mode variables */

  if (usmDbUnitMgrMgrNumberGet(&unit) != L7_SUCCESS)
  {
    java_mode_current = L7_DISABLE;
  }
  else
  {
    if (usmDbWebJavaModeGet(unit, &java_mode_current) != L7_SUCCESS)
      java_mode_current = L7_DISABLE;
  }
  java_mode_previous = java_mode_current;

  sprintf(jMsg, "Unit Manager Unit is 0x%x\n", unit);
  javaAppletTraceMsg(jMsg);

  javaTrace(0, 0, 0);

  for (; ;)                         /* Forever ...                            */
  {

    if (java_mode_current == L7_ENABLE)
    {
      if ((java_mode_previous != java_mode_current) || (bTimeZero == L7_TRUE))
      {
         #ifdef L7_JAVA_V4_BIND
        /* ------------------------------------------------------------------ */
        /* |||              set up the java socket                        ||| */
        /* ------------------------------------------------------------------ */
        /* ------------------------------------------------------------------ */
        /* Obtain a file descriptor for the java "listening" socket           */
        /* ------------------------------------------------------------------ */
        srvsock = socket( AF_INET, SOCK_STREAM, 0 );
        if (srvsock < 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                  "Failed to create the Java Socket\n");
          osapiSocketClose(srvsock);
          return;
        }
        /* ------------------------------------------------------------------ */
        /* enable reuse of addresses to avoid errors                          */
        /* ------------------------------------------------------------------ */
        setsockopt(srvsock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse_addr,
                   sizeof(reuse_addr));

        /* ------------------------------------------------------------------ */
        /* associate a local network transport address with the socket        */
        /* ------------------------------------------------------------------ */
        memset((char *)&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;                    /* address family, inet */
        addr.sin_port =osapiHtons(JAVA_TCP_PORT);
        addr.sin_addr.s_addr=osapiHtonl(INADDR_ANY);
        if (bind(srvsock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                  "Failed to bind to the Java port\n");
          osapiSocketClose(srvsock);
          return;
        }
        /* ------------------------------------------------------------------ */
        /* Prepare to accept MAX_CLIENTS incoming connections                 */
        /* ------------------------------------------------------------------ */
        if (listen(srvsock, LISTENQ) < 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                  "Listen failure on the Java port\n");
          osapiSocketClose(srvsock);
          return;
        }
        maxfd = srvsock;              /* set maxfd                            */
        maxi = -1;                    /* index into client[] array            */

        for (i = 0; i < JAVA_MAX_CLIENTS; i++)
          client[i] = -1;             /* -1 signifies an available entry      */

        FD_ZERO(&allset);             /* clear all the bits in the allset     */
        FD_SET(srvsock, &allset);     /* add srvsock (java listening socket)  */
        #endif
        #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
        /* ------------------------------------------------------------------ */
        /* |||              set up the java socket                        ||| */
        /* ------------------------------------------------------------------ */
        /* ------------------------------------------------------------------ */
        /* Obtain a file descriptor for the java "listening" socket           */
        /* ------------------------------------------------------------------ */
         srvsock6 = socket( AF_INET6, SOCK_STREAM, 0 );
         if (srvsock6 < 0)
         {
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                   "Failed to create the Java Socket\n");
           return;
         }
        /* ------------------------------------------------------------------ */
        /* enable reuse of addresses to avoid errors                          */
        /* ------------------------------------------------------------------ */
        setsockopt(srvsock6, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse_addr,
                   sizeof(reuse_addr));

        /* ------------------------------------------------------------------ */
        /* associate a local network transport address with the socket        */
        /* ------------------------------------------------------------------ */
         memset( &addr6, 0, sizeof( addr6 ));
         addr6.sin6_family = AF_INET6;
         addr6.sin6_port =osapiHtons(JAVA_TCP_PORT);
        if (bind(srvsock6, (struct sockaddr *) &addr6, sizeof(addr6)) < 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                  "Failed to bind to the Java port\n");
          osapiSocketClose(srvsock6);
          return;
        }

        /* ------------------------------------------------------------------ */
        /* Prepare to accept MAX_CLIENTS incoming connections                 */
        /* ------------------------------------------------------------------ */
        if (listen(srvsock6, LISTENQ) < 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                  "Listen failure on the Java port\n");
          osapiSocketClose(srvsock6);
          return;
        }
        maxfd = srvsock6;              /* set maxfd                            */
        maxi = -1;                    /* index into client[] array            */

        for (i = 0; i < JAVA_MAX_CLIENTS; i++)
          client[i] = -1;             /* -1 signifies an available entry      */

        FD_ZERO(&allset);             /* clear all the bits in the allset     */
        FD_SET(srvsock6, &allset);     /* add srvsock (java listening socket)  */
      #endif
      }
    }
    else    /* java_mode_current == L7_DISABLE */
    {
      if (java_mode_previous != java_mode_current)
      {
        /* ------------------------------------------------------------------ */
        /* |||                Tear down the socket                        ||| */
        /* ------------------------------------------------------------------ */
        #ifdef L7_JAVA_V4_BIND
        shutdown(srvsock, 2);
        osapiSocketClose(srvsock);
        #endif
        #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
        shutdown(srvsock6, 2);
        osapiSocketClose(srvsock6);
        #endif
      }

    }
    bTimeZero = L7_FALSE;
    java_mode_previous = java_mode_current;

    if (usmDbWebJavaModeGet(unit, &java_mode_current) != L7_SUCCESS)
      java_mode_current = L7_DISABLE;

    if (java_mode_previous == L7_DISABLE)
    {
      osapiSleep (1); /* Wait one second before checking JAVA mode again */
      continue;
    }

    memcpy(&readset, &allset, sizeof(allset));  /* Copy allset into readset   */

    nready = select(maxfd+1, &readset, NULL, NULL, NULL);
    if (nready <= 0)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "Error on select.\n");
      continue;
    }

    /* ---------------------------------------------------------------------- */
    /* Process new client connections                                         */
    /* ---------------------------------------------------------------------- */
   #ifdef L7_JAVA_V4_BIND
    if (FD_ISSET(srvsock, &readset))
    {
      len = sizeof(addr);
      connfd = accept(srvsock, (struct sockaddr *)&addr, &len);
      if (connfd < 0 )
        continue;
      for (i = 0; i < JAVA_MAX_CLIENTS; i++)
      {
        if (client[i] < 0)
        {
          client[i] = connfd;          /* save descriptior                    */
          break;
        }
      }
      if (i == JAVA_MAX_CLIENTS)
      {
        shutdown(sockfd, 2);
        osapiSocketClose(connfd);
        FD_CLR(connfd, &allset);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "Max clients exceeded. This message is shown when "
                          "the maximum allowed java client connections to the switch is exceeded.\n");
        continue;
      }

      FD_SET(connfd, &allset);         /* add the new client descriptor       */
      if (connfd > maxfd)
        maxfd = connfd;
      if (i > maxi)
        maxi = i;

      FD_CLR(srvsock, &readset);       /* Don't want to read from the server  */

      if (--nready <= 0)
        continue;                      /* no more readable descriptors        */
    }
#endif
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
if (FD_ISSET(srvsock6, &readset))
    {
      len = sizeof(addr6);
      connfd = accept(srvsock6, (struct sockaddr *)&addr6, &len);
      if (connfd < 0 )
        continue;

      for (i = 0; i < JAVA_MAX_CLIENTS; i++)
      {
        if (client[i] < 0)
        {
          client[i] = connfd;          /* save descriptior                    */
          break;
        }
      }
      if (i == JAVA_MAX_CLIENTS)
      {
        shutdown(sockfd, 2);
        osapiSocketClose(connfd);
        FD_CLR(connfd, &allset);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "Max clients exceeded. This message is shown when "
                          "the maximum allowed java client connections to the switch is exceeded.\n");
        continue;
      }

      FD_SET(connfd, &allset);         /* add the new client descriptor       */
      if (connfd > maxfd)
        maxfd = connfd;
      if (i > maxi)
        maxi = i;

      FD_CLR(srvsock6, &readset);       /* Don't want to read from the server  */

      if (--nready <= 0)
        continue;                      /* no more readable descriptors        */
    }
#endif
    for (i = 0; i <= maxi; i++)        /* Check all client sockets for data   */
    {
      sockfd = client[i];
      if (sockfd < 0)
        continue;
      if (FD_ISSET(sockfd, &readset))  /* Process data from sockfd            */
      {
        if(usmDbIsAChassisPlatform()!=L7_TRUE)
        {
        bzero(buffer,sizeof(buffer));
        num_bytes_recv = recv(sockfd,buffer,sizeof(buffer),0);
        sprintf(jMsg, "num_bytes_recv = %d bytes\n", num_bytes_recv);
        javaAppletTraceMsg(jMsg);

        idx = 0;
        cmd = l7utilsBytesToInt(&buffer[idx]);
        sprintf(jMsg, "cmd = %d (WJS_APPLET[%d] WJS_ON[%d] WJS_OFF[%d])\n", cmd, WJS_APPLET, WJS_ON, WJS_OFF);
        javaAppletTraceMsg(jMsg);
        num_bytes_to_send = 0;
        num_bytes_sent = 0;
        flagCMD=L7_FALSE;
        }
        else if(usmDbIsAChassisPlatform()==L7_TRUE)
        {
           memset(buffer_chassis, 0x00, sizeof(buffer_chassis));
        pDat = (char *) &buffer_chassis;
        num_bytes_recv = recv(sockfd, pDat, 2*sizeof(L7_uint32), 0);
        cmd = buffer_chassis[0];
        if (cmd == WJS_APPLET)
          datalength = (sizeof(buffer_chassis)-2*sizeof(L7_uint32));
        else
          datalength = buffer_chassis[1]*sizeof(L7_uint32);
        if (datalength > (sizeof(buffer_chassis)-2*sizeof(L7_uint32)))
        {
          flagCMD=L7_TRUE;
          shutdown(sockfd, 2);
          osapiSocketClose(sockfd);
          FD_CLR(sockfd, &allset);
          client[i] = -1;
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                  "Invalid recv datalength.\n");
        }



        }

#ifdef L7_WIRELESS_PACKAGE /* BEGIN NETWORK VISUALIZATION FOR WIRELESS */

        /* ---------------------------------------------------------------------- */
        /* Only allow NV to run on the Cluster Controller                         */
        /* If controller changes, freeze the NV applet                            */
        /* ---------------------------------------------------------------------- */
        if (usmDbWdmWidsControllerIndGet(&clusterController) != L7_SUCCESS)
        {
          clusterController = L7_FALSE;
        }
        if (clusterController == L7_FALSE)
        {
            switch (cmd)
            {
              case WJS_NV_ON:
                num_bytes_to_send = 1;
                buffer[0] = 0;  /* Not the cluster controller */
                num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
                break;
              case WJS_NV_POLL:
                num_bytes_to_send = 4;
                l7utilsIntToBytes(&buffer[0],L7_WNV_EVENT_CC_MOVED); /* Cluster controller moved */
                num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
                break;
              case WJS_NV_OFF:
              case WJS_NV_GET_USER_DATA:
              case WJS_NV_SET_USER_DATA:
              case WJS_NV_GET_IMAGE_NAMES:
              case WJS_NV_DEL_IMAGE:
              case WJS_NV_GET_GC:
              case WJS_NV_GET_LOCAL_WS:
              case WJS_NV_GET_PEER_WS:
              case WJS_NV_GET_MAP:
              case WJS_NV_GET_UAP:
              case WJS_NV_GET_CS:
              case WJS_NV_GET_DCS:
              case WJS_NV_SET_DCS:
              case WJS_NV_DEL_DCS:
              case WJS_NV_SET_GC:
              case WJS_NV_DEL_GC:
              case WJS_NV_GET_BLDG_FLR_DATA:
              case WJS_NV_SET_LOCAL_WS:
              case WJS_NV_DEL_LOCAL_WS:
              case WJS_NV_SET_PEER_WS:
              case WJS_NV_DEL_PEER_WS:
              case WJS_NV_SET_AP:
              case WJS_NV_DEL_AP:
              case WJS_NV_RESET_AP:
              case WJS_NV_MANAGE_AP:
              case WJS_NV_ACKNOWLEDGE_AP:
              case WJS_NV_DISASSOCIATE_CS:
              case WJS_NV_POLL_RESET:
              case WJS_NV_GET_STATUS:
              case WJS_NV_GET_OLT:
              case WJS_NV_SET_OLT:
              case WJS_NV_START_SEARCH:
              case WJS_NV_GET_OLT_STATUS:
              case WJS_NV_GET_OLT_FLR_STATUS:
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                        "Refusing NV Applet connection.  Switch is not the cluster controller.");
                shutdown(sockfd, 2);
                osapiSocketClose(sockfd);
                FD_CLR(sockfd, &allset);
                client[i] = -1;
                num_bytes_to_send = 0;
                num_bytes_sent = 0;
                break;
            }
        }
#endif

        switch (cmd)                 /* process the command                 */
        {
          case WJS_ON:
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

          case WJS_DEVSHELL:
            if (bRemoteDevshellEnabled == L7_TRUE)
            {
              cmdPtr = &buffer[4];
              printf("\n-----------------------------------------------------------------------");
              printf("\nremote devShell command :  %s", cmdPtr);
              printf("\n-----------------------------------------------------------------------\n");
              cliDevShell(cmdPtr);
            }
            else
            {
              shutdown(sockfd, 2);
              osapiSocketClose(sockfd);
              FD_CLR(sockfd, &allset);
              client[i] = -1;
              break;
            }
            break;

          case WJS_OFF:
            shutdown(sockfd, 2);
            osapiSocketClose(sockfd);
            FD_CLR(sockfd, &allset);
            client[i] = -1;
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_APPLET:
            usmWebChangeDetected(1, buffer);
            num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);

            break;

#ifdef L7_WIRELESS_PACKAGE /* BEGIN NETWORK VISUALIZATION FOR WIRELESS */

          case WJS_NV_ON:
            if (clusterController == L7_TRUE)
            {
              num_bytes_to_send = 1;
              buffer[0] = 1;  /* This is the cluster controller */
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
              usmDbWnvConnectionActiveSet();
            }
            break;


          case WJS_NV_OFF:
            shutdown(sockfd, 2);
            osapiSocketClose(sockfd);
            FD_CLR(sockfd, &allset);
            client[i] = -1;
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            usmDbWnvConnectionActiveClear();
            break;

          case WJS_NV_GET_USER_DATA:
            num_bytes_to_send = nvUserDataGet(buffer);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

          case WJS_NV_SET_USER_DATA:
            nvUD.showPowerDisplay = buffer[idx+=4];
            nvUD.showTabView      = buffer[idx+=1];
            nvUD.showMAPs         = buffer[idx+=1];
            nvUD.showRAPs         = buffer[idx+=1];
            nvUD.showMAPClients   = buffer[idx+=1];
            nvUD.showDetectedClients   = buffer[idx+=1];
            nvUD.showLocationFlag   = buffer[idx+=1];
            nvUserDataSet(nvUD);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

          case WJS_NV_GET_IMAGE_NAMES:
            num_bytes_to_send = nvImageNameListGet(buffer);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

        case WJS_NV_DEL_IMAGE:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(nvGC.fileName,&buffer[idx+=4],len);
            nvGC.fileName[len] = '\0';
            nvGC.imageWidth = 0;
            nvGC.imageHeight = 0;
            nvGC.imageMeasurement = 0;
            nvDeleteImage(nvGC);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

          case WJS_NV_GET_GC:
            num_bytes_to_send = nvGCListGet(buffer);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

          case WJS_NV_GET_LOCAL_WS:
            num_bytes_to_send = nvLocalWSGet(buffer);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

          case WJS_NV_GET_PEER_WS:
            num_bytes_to_send = nvPeerWSListGet(buffer);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

          case WJS_NV_GET_MAP:
            fOverflow = L7_FALSE;
            num_bytes_to_send = nvMAPListGet(buffer,&fOverflow);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            if (fOverflow == L7_TRUE)
            {
              /* Notify event queue of situation */
              nvLostMessageEventAdd();
            }
            break;

          case WJS_NV_GET_UAP:
            fOverflow = L7_FALSE;
            num_bytes_to_send = nvUAPListGet(buffer,&fOverflow);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            if (fOverflow == L7_TRUE)
            {
              /* Notify event queue of situation */
              nvLostMessageEventAdd();
            }
            break;

          case WJS_NV_GET_CS:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            num_bytes_to_send = nvCSListGet(macAdd,buffer,&fOverflow);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

        case WJS_NV_GET_DCS:
            num_bytes_to_send = nvDCSListGet(buffer,&fOverflow);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

        case WJS_NV_GET_OLT:
            num_bytes_to_send = nvLocationTriggerDataGet(buffer, &fOverflow);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

        case WJS_NV_SET_OLT:
            nvOLT.building = buffer[idx+=4];
            nvOLT.floor = buffer[idx+=1];
            len = l7utilsBytesToInt(&buffer[idx+=1]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&nvOLT.mac,0x00,sizeof(nvOLT.mac));
            cliConvertMac(mac,nvOLT.mac.addr);
            nvOLT.devType= l7utilsBytesToInt(&buffer[idx+=len]);

            nvOLT.useRadios= l7utilsBytesToInt(&buffer[idx+=4]);

            num_bytes_to_send = nvLocationTriggerDataSet(&nvOLT);
            bzero(buffer,sizeof(buffer));
            l7utilsIntToBytes(&buffer[0],nvOLT.numOfAPs);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
          break;

        case WJS_NV_START_SEARCH:
            num_bytes_to_send = nvLocationTriggerStartSearch(&nvOLT);
            bzero(buffer,sizeof(buffer));
            l7utilsIntToBytes(&buffer[0],nvOLT.searchStatus);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

        case WJS_NV_GET_OLT_STATUS:
            num_bytes_to_send = nvLocationTriggerGlobalStatusGet(buffer, &fOverflow);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

        case WJS_NV_GET_OLT_FLR_STATUS:
            num_bytes_to_send = nvLocationTriggerFloorStatusGet(buffer, &fOverflow);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

        case WJS_NV_SET_GC:
            index = buffer[idx+=4];
            len = l7utilsBytesToInt(&buffer[idx+=1]);
            strncpy(nvGC.graphName,&buffer[idx+=4],len);
            nvGC.graphName[len] = '\0';
            len = l7utilsBytesToInt(&buffer[idx+=len]);
            strncpy(nvGC.fileName,&buffer[idx+=4],len);
            nvGC.fileName[len] = '\0';
            nvGC.imageMeasurement = buffer[idx+=len];
            nvGC.imageWidth = l7utilsBytesToInt(&buffer[idx +=1]);
            nvGC.imageHeight = l7utilsBytesToInt(&buffer[idx +=4]);
            bldgNum = l7utilsBytesToInt(&buffer[idx +=4]);
            flrNum = l7utilsBytesToInt(&buffer[idx +=4]);
            nvGC.graphLoc.bldgNum = bldgNum;
            nvGC.graphLoc.flrNum  = flrNum;
            nvGC.inUse = L7_TRUE;
            nvUpdateGC(index,nvGC);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_DEL_GC:
            index = buffer[idx+=4];
            bldgNum = l7utilsBytesToInt(&buffer[idx +=1]);
            flrNum  = l7utilsBytesToInt(&buffer[idx +=4]);
            nvDeleteGC(index);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

         case WJS_NV_GET_BLDG_FLR_DATA:
            num_bytes_to_send = nvBldgFlrListGet(buffer);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;
        case WJS_NV_SET_LOCAL_WS:
        case WJS_NV_DEL_LOCAL_WS:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(ips,&buffer[idx+=4],len);
            ips[len] = '\0';
            usmDbInetAton(ips,&ip);
            nvCfg.graphIndex = buffer[idx+=len];
            if (WJS_NV_DEL_LOCAL_WS==cmd) {
              nvCfg.graphIndex = 0;
            }
            nvCfg.x = l7utilsBytesToInt(&buffer[idx+=1]);
            nvCfg.y = l7utilsBytesToInt(&buffer[idx+=4]);
            nvUpdateLocalWS(ip,nvCfg);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_SET_PEER_WS:
        case WJS_NV_DEL_PEER_WS:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(ips,&buffer[idx+=4],len);
            ips[len] = '\0';
            usmDbInetAton(ips,&ip);
            nvCfg.graphIndex = buffer[idx+=len];
            nvCfg.x = l7utilsBytesToInt(&buffer[idx+=1]);
            nvCfg.y = l7utilsBytesToInt(&buffer[idx+=4]);
            if (WJS_NV_SET_PEER_WS==cmd) nvUpdatePeerWS(ip,nvCfg);
              else nvDeletePeerWS(ip);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_SET_AP:
        case WJS_NV_DEL_AP:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            if (WJS_NV_SET_AP==cmd) {
              nvCfg.graphIndex = buffer[idx+=len];
              nvCfg.x = l7utilsBytesToInt(&buffer[idx+=1]);
              nvCfg.y = l7utilsBytesToInt(&buffer[idx+=4]);
              nvUpdateAP(macAdd,nvCfg);
            }
            else {
             nvDeleteAP(macAdd);
            }
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_RESET_AP:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvResetAP(macAdd);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_MANAGE_AP:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvManageAP(macAdd);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_ACKNOWLEDGE_AP:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvAcknowledgeAP(macAdd);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_DISASSOCIATE_CS:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvDisassociateCS(macAdd);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_SET_DCS:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvCfg.graphIndex = buffer[idx+=len];
            nvCfg.x = l7utilsBytesToInt(&buffer[idx+=1]);
            nvCfg.y = l7utilsBytesToInt(&buffer[idx+=4]);
            nvUpdateDetectedClient(macAdd,nvCfg);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_DEL_DCS:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvDeleteDetectedClient(macAdd);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_POLL_RESET:
            usmDbWnvEventsReset(i-1);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_POLL:
            num_bytes_to_send = nvPoll(i-1,buffer);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

        case WJS_NV_GET_STATUS:
            if (L7_SUCCESS != usmDbWnvEventsLostGet(&nvEvents)) {
              nvEvents = 0;
            }
            l7utilsIntToBytes(&buffer[0],nvEvents);
            num_bytes_to_send = 4;
            num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;


        case WJS_NV_GET_SESSION_STATUS:
            len = l7utilsBytesToInt(&buffer[idx+=4]);
            /* printf("WJS_NV_GET_SESSION_STATUS : len = %d\n", len); */
            strncpy(cookie,&buffer[idx+=4],sizeof(cookie)-1); /* Check for too long here */
            cookie[len] = '\0';
            /* printf("WJS_NV_GET_SESSION_STATUS : %s\n", cookie); */
            if (usmWebSessionStatusGet(cookie) == L7_TRUE)
            {
              sVal = 1;
            }
            else
            {
              sVal = 0;
            }
            l7utilsIntToBytes(&buffer[0],sVal);
            num_bytes_to_send = 4;
            num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

#endif /* L7_WIRELESS_PACKAGE*/

          default:
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                    "Unknown command %d (0x%08x)\n", cmd, cmd);
            /*
            */
            shutdown(sockfd, 2);
            osapiSocketClose(sockfd);
            FD_CLR(sockfd, &allset);
            client[i] = -1;
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;
        }

        /* Generic trap for socket sends */
        if (num_bytes_sent != num_bytes_to_send)
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "Error on send to sockfd %d, closing connection. "
                                   "Failed to send data to the java clients through the socket.\n", sockfd);
          shutdown(sockfd, 2);
          osapiSocketClose(sockfd);
          FD_CLR(sockfd, &allset);
          client[i] = -1;
        }

        if (--nready <= 0)
          continue;                      /* no more readable descriptors    */

      } /* Process data from sockfd */
    } /* End checking all client sockets for data */
  } /* Done processing forever loop */
}



