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
#endif

#include "usmdb_counters_api.h"

#include "osapi_support.h"
#include "l7_socket.h"
#include "web_java.h"
#include "usmdb_poe_api.h"
#include "session.h"

#ifdef L7_WIRELESS_PACKAGE
#include "usmdb_wdm_nv_api.h"
#include "usmdb_wnv_api.h"
#include "usmdb_wdm_client_api.h"
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

int bytesToInt(L7_char8 *b)
{
  return *b<<24 | ((*++b)&0xff)<<16 | ((*++b)&0xff)<<8 | ((*++b)&0xff);
}

void intToBytes(L7_char8 *b, L7_uint32 i)
{
  *b   = (L7_char8)(i>>24);
  *++b = (L7_char8)(i>>16);
  *++b = (L7_char8)(i>>8);
  *++b = (L7_char8)i;
}

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
    LOG_MSG("%s", msg);
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
  L7_BOOL   sfpLinkUp, sfpSupport, spmSupport, poeSupport;
  L7_char8 jMsg[256];
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
  /*L7_uint32 i; /
  L7_uint32 numSlots;
  L7_uint32 slots[10];
  L7_uint32 cardType;*/
  L7_uint32 unitType;
  L7_uint32 unitStatus;
  L7_char8  jMsg[256];
  L7_BOOL isFull = L7_FALSE;

  if(usmDbIsAChassisPlatform() == L7_TRUE)
  {
   L7_uint32 phySlot;
   L7_RC_t rcSlot, rc;

  if (usmDbUnitMgrUnitTypeGet(unitNum, &unitType) == L7_SUCCESS)
  {
    if (usmDbUnitMgrUnitStatusGet(unitNum, &unitStatus) == L7_SUCCESS)
    {
      if (unitStatus == L7_USMDB_UNITMGR_UNIT_NOT_PRESENT)
      {
        isFull = L7_FALSE;
      }
       if ( (unitStatus == L7_USMDB_UNITMGR_UNIT_OK) ||
                (unitStatus == L7_USMDB_UNITMGR_UNIT_UNSUPPORTED) ||
                (unitStatus == L7_USMDB_UNITMGR_CODE_MISMATCH) ||
                (unitStatus == L7_USMDB_UNITMGR_CFG_MISMATCH))
      {
            rc = usmDbSlotMapUSPhysicalSlotGet(unitNum,&phySlot);
            rcSlot = usmDbSlotIsFullGet(unitNum, phySlot, &isFull);

            if (rcSlot == L7_SUCCESS || rcSlot == L7_ERROR)
            {
              isFull = L7_TRUE;
            }
            else
            {
              isFull = L7_FALSE;
            }
     }
     else
     {
       isFull = L7_FALSE;
     }
    }
   }
  }
  else
  {
  L7_uint32 i;
  L7_uint32 numSlots;
  L7_uint32 slots[10];
  L7_uint32 cardType;
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


  intToBytes(&data[indx],javaAppletTraceMode); /* Trace Options */
  indx+=4;
  sprintf(jMsg, "\nClient side trace mode : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
  javaAppletTraceMsg(jMsg);

  /* XUI flag */
#ifdef L7_XUIBASE_PACKAGE
  intToBytes(&data[indx], 1); /* XUI Presence (1) Present, (0) Absent */
  indx+=4;
#else
  intToBytes(&data[indx], 0); /* XUI Presence (1) Present, (0) Absent */
  indx+=4;
#endif
  sprintf(jMsg, "\nXUI Present : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
  javaAppletTraceMsg(jMsg);

  if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
  {
    intToBytes(&data[indx],1); /* Stacking */
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
    intToBytes(&data[indx],0); /* Non-stacking */
    indx+=4;
    rc = usmDbUnitMgrMgrNumberGet(&unitNum);
    if (rc == L7_SUCCESS)
    {
      numUnits = 1;
      firstUnitNum = unitNum;
    }
  }
  sprintf(jMsg, "Stacking Support : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
  javaAppletTraceMsg(jMsg);

  intToBytes(&data[indx],numUnits); /* Number of units */
  indx+=4;
  sprintf(jMsg, "Number of units : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
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
    rcc = usmWebNumSFPsGet(&numSFPs);

    intToBytes(&data[indx],unitNum); /* Unit Number */
    indx+=4;
    sprintf(jMsg, "Unit Number : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    if (javaAppletBoxIDOverride == 1)
      intToBytes(data,(javaAppletBoxIDOverride & 0xFFFF)); /* Box Type  */
    else
      intToBytes(&data[indx],((unitType & 0xFFFF0000)/0x10000)); /* Box Type  */
    indx+=4;
    sprintf(jMsg, "Box Type : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    /* Unit Status TBD ... (1) mgmt unit, (2) mgmt capable, (3) member unit */
    intToBytes(&data[indx],statusCode); /* Unit Status */
    indx+=4;
    sprintf(jMsg, "Status Code : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    intToBytes(&data[indx],numSFPs); /* Number of SFPs */
    indx+=4;
    sprintf(jMsg, "Number of SFPs : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    intToBytes(&data[indx],numSlots); /* Number of slots */
    indx+=4;
    sprintf(jMsg, "Number of Slots : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    for (i=0; i<numSlots; i++)
    {
      intToBytes(&data[indx],slots[i]); /* Slot Number */
      indx+=4;
      sprintf(jMsg, "Slot Number : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      intToBytes(&data[indx],javaAppletCardPresentGet(unitNum, slots[i])); /* Status: (1) Populated, (0) Empty */
      indx+=4;
      sprintf(jMsg, "Slot Populated : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      intToBytes(&data[indx], javaAppletCardTypeGet(unitNum, slots[i]));
      indx+=4;
      sprintf(jMsg, "Card Type : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      usmWebPortsStatusGet(unitNum, slots[i], &slotNumPorts, &slotNumSFPs, portStatus, L7_MAX_PORTS_PER_SLOT*2);
      intToBytes(&data[indx], slotNumPorts); /* Number of interfaces */
      indx+=4;
      sprintf(jMsg, "Number of Ports : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      intToBytes(&data[indx], javaAppletSlotPortNumberFirstGet(unitNum, slots[i])); /* Number of first port */
      indx+=4;
      sprintf(jMsg, "First Port Number : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
      javaAppletTraceMsg(jMsg);

      for (k=0; k<(slotNumPorts+slotNumSFPs); k++)
      {
        intToBytes(&data[indx], portStatus[k]);
        indx+=4;
      }
      sprintf(jMsg, "Read %d integers for slot %d (%d-ports and %d sfps)\n", slotNumPorts+slotNumSFPs, slots[i], slotNumPorts, slotNumSFPs);
      javaAppletTraceMsg(jMsg);
    }

    /* Get the dedicated stack interfaces */
    if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
      intToBytes(&data[indx], 0); /* No dedicated stack interfaces on FPS systems */
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
            intToBytes(&data[indx], spmPortEntry.port_info.link_status);
            indx+=4;
            sprintf(jMsg, "Link State for HiGig%d on Unit%d is %d\n", counter, unitNum, spmPortEntry.port_info.link_status);
            javaAppletTraceMsg(jMsg);
          }
        }
        intToBytes(&data[numStackIntfIndex], counter);
      }
      sprintf(jMsg, "Number of dedicated stack interfaces : %d\n", bytesToInt(&data[numStackIntfIndex]));
      javaAppletTraceMsg(jMsg);
    }

    if (SERVICE_PORT_PRESENT != L7_FALSE)
    {
      intToBytes(&data[indx], 1); /* Service Port Presence (1) Present, (0) Absent */
      indx+=4;
      serviceLink = usmDbServicePortLinkStatusGet(unitNum);
      if (serviceLink == L7_UP)
        intToBytes(&data[indx], 1);
      else
        intToBytes(&data[indx], 0);
      indx+=4;
    }
    else
    {
      intToBytes(&data[indx], 0); /* Service Port Presence (1) Present, (0) Absent */
      indx+=4;
    }

    intToBytes(&data[indx], 1); /* Serial Port Presence (1) Present, (0) Absent */
    indx+=4;
    sprintf(jMsg, "Serial Port Presence : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
    javaAppletTraceMsg(jMsg);

    if (usmDbSerialStatusGet(unitNum) == L7_UP)
      intToBytes(&data[indx], 1); /* Serial Port Link State */
    else
      intToBytes(&data[indx], 0); /* Serial Port Link State */
    indx+=4;
    sprintf(jMsg, "Serial Port Link State : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
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
        intToBytes(&data[indx], 0); /* Send zero as number of units - the applet will ignore this update */
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


  intToBytes(&data[indx], 0x11223344); /* End of data */
  indx+=4;
  sprintf(jMsg, "End of data marker : data[%d] = 0x%x\n", (indx-4)/4, bytesToInt(&data[indx-4]));
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

  if (L7_SUCCESS != usmDbWdmNVUserDataGet(&nvUD))
  {
    nvUD.showPowerDisplay = 0;
    nvUD.showTabView      = 0;
    nvUD.showMAPs         = 0;
    nvUD.showRAPs         = 0;
    nvUD.showMAPClients   = 0;
  }
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

  /* DEBUG */
  WIRELESS_DLOG(WD_LEVEL_NV_INFO,
    "NV: Server retrieved user data p=%d t=%d m=%d r=%d c=%d.\n",
    nvUD.showPowerDisplay,nvUD.showTabView,nvUD.showMAPs,nvUD.showRAPs,nvUD.showMAPClients);

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
    "NV: Server set user data p=%d t=%d m=%d r=%d c=%d.\n",
    nvUD.showPowerDisplay,nvUD.showTabView,nvUD.showMAPs,nvUD.showRAPs,nvUD.showMAPClients);
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

  intToBytes(&data[idx],cnt);
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
        intToBytes(&data[idx],len);
        idx+=4;
        strncpy(&data[idx],fileName,len);
        idx+=len;
      }
    }
  }
  intToBytes(&data[0],cnt);

  /* DEBUG */
  LOG_MSG("NV: Server found %d image names in %s%s.\n",cnt,RAM_NV_PATH,RAM_NV_NAME);
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
      LOG_MSG("NV: Server found and deleted %s.\n",fileName);
      break;
    }
  }
  return;
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
  intToBytes(&data[idx],cnt); /* 1st int holds total #, init with zero */
  idx+=4;

  /* Retrieve Graphs, limit return count (a precautionary measure)  */
  while ((usmDbWdmNVGraphEntryNextGet(index,&index,&nvGC) == L7_SUCCESS) &&
        ((L7_NV_MAX_GC_NAME) >= cnt))
  {
    len = strlen(nvGC.graphName);
    intToBytes(&data[idx],len);
    idx+=4;
    strncpy(&data[idx],nvGC.graphName,len);
    idx+=len;
    len = strlen(nvGC.fileName);
    intToBytes(&data[idx],len);
    idx+=4;
    strncpy(&data[idx],nvGC.fileName,len);
    idx+=len;
    data[idx] = index;
    idx+=1;
    data[idx] = nvGC.imageMeasurement;
    idx+=1;
    data[idx] = nvGC.imageDimension;
    idx+=1;
    intToBytes(&data[idx],nvGC.imageLength);
    idx+=4;
    cnt++;
  }

  /* DEBUG */
  intToBytes(&data[0],cnt);
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
  intToBytes(&data[idx],cnt);
  idx+=4;

  wsCfg.IP = 0;
  usmDbWdmWSIPAddrGet(&wsCfg.IP);
  if (L7_SUCCESS != usmDbWdmNVWSCoordGet(&nvCfg)) {
    nvCfg.graphIndex = 0;
    nvCfg.x = 0;
    nvCfg.y = 0;
  }

  intToBytes(&data[idx],wsCfg.IP);
  idx+=4;
  data[idx] = nvCfg.graphIndex;
  idx+=1;
  intToBytes(&data[idx],nvCfg.x);
  idx+=4;
  intToBytes(&data[idx],nvCfg.y);
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

  intToBytes(&data[idx],cnt);  /* 1st int holds total #, init with zero */
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
    intToBytes(&data[idx],wsCfg.IP);
    idx+=4;
    data[idx] = nvCfg.graphIndex;
    idx+=1;
    intToBytes(&data[idx],nvCfg.x);
    idx+=4;
    intToBytes(&data[idx],nvCfg.y);
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
  intToBytes(&data[0],cnt);
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

  intToBytes(&data[idx],cnt);
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
    intToBytes(&data[idx],len);
    idx+=4;
    strncpy(&data[idx],location,len);
    idx+=len;
    data[idx] = apCfg.GCId;
    idx+=1;
    intToBytes(&data[idx],apCfg.X);
    idx+=4;
    intToBytes(&data[idx],apCfg.Y);
    idx+=4;
    intToBytes(&data[idx],mode);
    idx+=4;
    data[idx] = radio1Power;
    idx+=1;
    data[idx] = radio1Channel;
    idx+=1;
    intToBytes(&data[idx],radio1SentryMode);
    idx+=4;
    intToBytes(&data[idx],radio1PhyMode);
    idx+=4;
    data[idx] = radio2Power;
    idx+=1;
    data[idx] = radio2Channel;
    idx+=1;
    intToBytes(&data[idx],radio2SentryMode);
    idx+=4;
    intToBytes(&data[idx],radio2PhyMode);
    idx+=4;
    intToBytes(&data[idx],cfgStatus);
    idx+=4;
    intToBytes(&data[idx],status);
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
    intToBytes(&data[idx],len);
    idx+=4;
    if (len > 0)
    {
      strncpy(&data[idx],ips,len);
      idx+=len;
    }
    intToBytes(&data[idx],profileId);
    idx+=4;
    cnt++;

    /* DEBUG */
    WIRELESS_DLOG(WD_LEVEL_NV_INFO,
      "NV: Server detected managed AP. mac=%s id=%d x=%d y=%d m=%d r1p=%d r1c=%d r1s=%d r1m=%d r2p=%d r2c=%d r2s=%d r2m=%d s=%d.\n",
      mac,apCfg.GCId,apCfg.X,apCfg.Y,mode,
      radio1Power,radio1Channel,radio1SentryMode,radio1PhyMode,
      radio2Power,radio2Channel,radio2SentryMode,radio2PhyMode,status);
  }
  intToBytes(&data[0],cnt);
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

  intToBytes(&data[idx],cnt);
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
      intToBytes(&data[idx],len);
      idx+=4;
      strncpy(&data[idx],location,len);
      idx+=len;
      data[idx] = apCfg.GCId;
      idx+=1;
      intToBytes(&data[idx],apCfg.X);
      idx+=4;
      intToBytes(&data[idx],apCfg.Y);
      idx+=4;
      intToBytes(&data[idx],mode);
      idx+=4;
      data[idx] = radio1Power;
      idx+=1;
      data[idx] = radio1Channel;
      idx+=1;
      intToBytes(&data[idx],radio1SentryMode);
      idx+=4;
      intToBytes(&data[idx],radio1PhyMode);
      idx+=4;
      data[idx] = radio2Power;
      idx+=1;
      data[idx] = radio2Channel;
      idx+=1;
      intToBytes(&data[idx],radio2SentryMode);
      idx+=4;
      intToBytes(&data[idx],radio2PhyMode);
      idx+=4;
      intToBytes(&data[idx],cfgStatus);
      idx+=4;
      intToBytes(&data[idx],status);
      idx+=4;
      len = 0;  /* Placeholder for managing switch ip address */
      intToBytes(&data[idx],len);
      idx+=4;
      intToBytes(&data[idx],profileId);
      idx+=4;
      cnt++;

    /* DEBUG */
    WIRELESS_DLOG(WD_LEVEL_NV_INFO,
      "NV: Server detected un-managed AP. mac=%s id=%d x=%d y=%d r1c=%d s=%d.\n",
      mac,apCfg.GCId,apCfg.X,apCfg.Y,radio1Channel,status);
    }
  }

  intToBytes(&data[0],cnt);
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

  intToBytes(&data[idx],cnt); /* 1st int holds total #, init with zero */
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
        intToBytes(&data[idx],ip);
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
  intToBytes(&data[0],cnt);
  return idx;
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
    "NV: Server detected graph update. name=%s id=%d img=%s mea=%d dim=%d len=%d.\n",
    nvGC.graphName,index,nvGC.fileName,nvGC.imageMeasurement,nvGC.imageDimension,nvGC.imageLength);
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

  if (usmDbWdmAPEntryGet(mac) != L7_SUCCESS)
  {
    rc = usmDbWdmRogueAPStateClear(mac);
  }

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
  L7_WDM_MANAGED_AP_STATUS_t tmpStatus;
  L7_uint32 mapSwitchIpAddr = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 val;
  L7_WDM_ACTION_STATUS_t cfgStatus;
  L7_uint32 profileId;
  wdmNVUserData_t nvUD;

  intToBytes(&data[0],-1); /* indicates non event */
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
          intToBytes(&data[0],-1); /* indicates non event */
        }
        else
        {
          intToBytes(&data[0],evt.eventId);
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
          intToBytes(&data[idx],len);
          idx+=4;
          strncpy(&data[idx],ips,len);
          idx+=len;
          data[idx] = nvCfg.graphIndex;
          idx+=1;
          intToBytes(&data[idx],nvCfg.x);
          idx+=4;
          intToBytes(&data[idx],nvCfg.y);
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
          intToBytes(&data[0],-1); /* indicates non event */
        }
        else
        {
          intToBytes(&data[0],evt.eventId);
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
          intToBytes(&data[idx],len);
          idx+=4;
          strncpy(&data[idx],mac,len);
          idx+=len;
          len = strlen(location);
          intToBytes(&data[idx],len);
          idx+=4;
          strncpy(&data[idx],location,len);
          idx+=len;
          data[idx] = GCId;
          idx+=1;
          intToBytes(&data[idx],X);
          idx+=4;
          intToBytes(&data[idx],Y);
          idx+=4;
          intToBytes(&data[idx],mode);
          idx+=4;
          data[idx] = radio1Power;
          idx+=1;
          data[idx] = radio1Channel;
          idx+=1;
          intToBytes(&data[idx],radio1SentryMode);
          idx+=4;
          intToBytes(&data[idx],radio1PhyMode);
          idx+=4;
          data[idx] = radio2Power;
          idx+=1;
          data[idx] = radio2Channel;
          idx+=1;
          intToBytes(&data[idx],radio2SentryMode);
          idx+=4;
          intToBytes(&data[idx],radio2PhyMode);
          idx+=4;
          intToBytes(&data[idx],cfgStatus);
          idx+=4;
          intToBytes(&data[idx],status);
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
          intToBytes(&data[idx],len);
          idx+=4;
          if (len > 0)
          {
            strncpy(&data[idx],ips,len);
            idx+=len;
          }

          intToBytes(&data[idx],profileId);
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
          intToBytes(&data[0],-1); /* indicates non event */
        }
        else
        {
          intToBytes(&data[0],evt.eventId);
          status           = 0;
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
          usmDbWdmRFScanStatusGet(evt.wdmIndex.macAddr,(L7_WDM_AP_STATUS_t *)&status);
          if (L7_WDM_AP_STATUS_MANAGED!=status) /* filter out managed APs */
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
                  intToBytes(&data[0],-1); /* indicates non event */
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
            intToBytes(&data[idx],len);
            idx+=4;
            strncpy(&data[idx],mac,len);
            idx+=len;
            len = strlen(location);
            intToBytes(&data[idx],len);
            idx+=4;
            strncpy(&data[idx],location,len);
            idx+=len;
            data[idx] = GCId;
            idx+=1;
            intToBytes(&data[idx],X);
            idx+=4;
            intToBytes(&data[idx],Y);
            idx+=4;
            intToBytes(&data[idx],mode);
            idx+=4;
            data[idx] = radio1Power;
            idx+=1;
            data[idx] = radio1Channel;
            idx+=1;
            intToBytes(&data[idx],radio1SentryMode);
            idx+=4;
            intToBytes(&data[idx],radio1PhyMode);
            idx+=4;
            data[idx] = radio2Power;
            idx+=1;
            data[idx] = radio2Channel;
            idx+=1;
            intToBytes(&data[idx],radio2SentryMode);
            idx+=4;
            intToBytes(&data[idx],radio2PhyMode);
            idx+=4;
            intToBytes(&data[idx],cfgStatus);
            idx+=4;
            intToBytes(&data[idx],status);
            idx+=4;
            len = 0;  /* Placeholder for managing switch ip address */
            intToBytes(&data[idx],len);
            idx+=4;
            intToBytes(&data[idx],profileId);
            idx+=4;

            /* DEBUG */
            WIRELESS_DLOG(WD_LEVEL_NV_INFO,
              "NV: Server event, un-managed AP. evt=%d mac=%s id=%d x=%d y=%d r1c=%d s=%d.\n",
              evt.eventId,mac,GCId,X,Y,radio1Channel,status);
          }
          else
          {
            intToBytes(&data[0],-1);
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
          intToBytes(&data[0],-1); /* indicates non event */
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
            intToBytes(&data[0],evt.eventId);
            data[idx] = nvCfg.graphIndex;
            idx+=1;
            len = strlen(aMac);
            intToBytes(&data[idx],len);
            idx+=4;
            strncpy(&data[idx],aMac,len);
            idx+=len;
            len = strlen(cMac);
            intToBytes(&data[idx],len);
            idx+=4;
            strncpy(&data[idx],cMac,len);
            idx+=len;
            len = strlen(ips);
            intToBytes(&data[idx],len);
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
  L7_int32            nvEvents = 0;
  L7_BOOL             fOverflow;
  L7_BOOL             clusterController = L7_FALSE;
  L7_char8            cookie[256];
  L7_int32            sVal;
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
          LOG_MSG("Failed to create the Java Socket\n");
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
          LOG_MSG("Failed to bind to the Java port\n");
          osapiSocketClose(srvsock);
          return;
        }
        /* ------------------------------------------------------------------ */
        /* Prepare to accept MAX_CLIENTS incoming connections                 */
        /* ------------------------------------------------------------------ */
        if (listen(srvsock, LISTENQ) < 0)
        {
          LOG_MSG("Listen failure on the Java port\n");
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
           LOG_MSG("Failed to create the Java Socket\n");
           osapiSocketClose(srvsock6);
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
          LOG_MSG("Failed to bind to the Java port\n");
          osapiSocketClose(srvsock6);
          return;
        }

        /* ------------------------------------------------------------------ */
        /* Prepare to accept MAX_CLIENTS incoming connections                 */
        /* ------------------------------------------------------------------ */
        if (listen(srvsock6, LISTENQ) < 0)
        {
          LOG_MSG("Listen failure on the Java port\n");
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
      LOG_MSG("Error on select.\n");
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
		L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "Max clients exceeded. This message is shown when "
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
		L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "Max clients exceeded. This message is shown when "
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
        cmd = bytesToInt(&buffer[idx]);
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
          LOG_MSG("Invalid recv datalength.\n");
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
                intToBytes(&buffer[0],L7_WNV_EVENT_CC_MOVED); /* Cluster controller moved */
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
              case WJS_NV_SET_GC:
              case WJS_NV_DEL_GC:
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
                LOG_MSG("Refusing NV Applet connection.  Switch is not the cluster controller.");
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
            len = bytesToInt(&buffer[idx+=4]);
            strncpy(nvGC.fileName,&buffer[idx+=4],len);
            nvGC.fileName[len] = '\0';
            nvGC.imageDimension = 0;
            nvGC.imageLength = 0;
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
            len = bytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            num_bytes_to_send = nvCSListGet(macAdd,buffer,&fOverflow);
            if (0<num_bytes_to_send)
              num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

        case WJS_NV_SET_GC:
            index = buffer[idx+=4];
            len = bytesToInt(&buffer[idx+=1]);
            strncpy(nvGC.graphName,&buffer[idx+=4],len);
            nvGC.graphName[len] = '\0';
            len = bytesToInt(&buffer[idx+=len]);
            strncpy(nvGC.fileName,&buffer[idx+=4],len);
            nvGC.fileName[len] = '\0';
            nvGC.imageMeasurement = buffer[idx+=len];
            nvGC.imageDimension = buffer[idx+=1];
            nvGC.imageLength = bytesToInt(&buffer[idx+=1]);
            nvUpdateGC(index,nvGC);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_DEL_GC:
            index = buffer[idx+=4];
            nvDeleteGC(index);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_SET_LOCAL_WS:
        case WJS_NV_DEL_LOCAL_WS:
            len = bytesToInt(&buffer[idx+=4]);
            strncpy(ips,&buffer[idx+=4],len);
            ips[len] = '\0';
            usmDbInetAton(ips,&ip);
            nvCfg.graphIndex = buffer[idx+=len];
            if (WJS_NV_DEL_LOCAL_WS==cmd) {
              nvCfg.graphIndex = 0;
            }
            nvCfg.x = bytesToInt(&buffer[idx+=1]);
            nvCfg.y = bytesToInt(&buffer[idx+=4]);
            nvUpdateLocalWS(ip,nvCfg);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_SET_PEER_WS:
        case WJS_NV_DEL_PEER_WS:
            len = bytesToInt(&buffer[idx+=4]);
            strncpy(ips,&buffer[idx+=4],len);
            ips[len] = '\0';
            usmDbInetAton(ips,&ip);
            nvCfg.graphIndex = buffer[idx+=len];
            nvCfg.x = bytesToInt(&buffer[idx+=1]);
            nvCfg.y = bytesToInt(&buffer[idx+=4]);
            if (WJS_NV_SET_PEER_WS==cmd) nvUpdatePeerWS(ip,nvCfg);
              else nvDeletePeerWS(ip);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_SET_AP:
        case WJS_NV_DEL_AP:
            len = bytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            if (WJS_NV_SET_AP==cmd) {
              nvCfg.graphIndex = buffer[idx+=len];
              nvCfg.x = bytesToInt(&buffer[idx+=1]);
              nvCfg.y = bytesToInt(&buffer[idx+=4]);
              nvUpdateAP(macAdd,nvCfg);
            }
            else {
             nvDeleteAP(macAdd);
            }
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_RESET_AP:
            len = bytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvResetAP(macAdd);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_MANAGE_AP:
            len = bytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvManageAP(macAdd);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_ACKNOWLEDGE_AP:
            len = bytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvAcknowledgeAP(macAdd);
            num_bytes_to_send = 0;
            num_bytes_sent = 0;
            break;

        case WJS_NV_DISASSOCIATE_CS:
            len = bytesToInt(&buffer[idx+=4]);
            strncpy(mac,&buffer[idx+=4],len);
            mac[len] = '\0';
            memset(&macAdd,0x00,sizeof(macAdd));
            cliConvertMac(mac,macAdd.addr);
            nvDisassociateCS(macAdd);
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
            intToBytes(&buffer[0],nvEvents);
            num_bytes_to_send = 4;
            num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;


        case WJS_NV_GET_SESSION_STATUS:
            len = bytesToInt(&buffer[idx+=4]);
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
            intToBytes(&buffer[0],sVal);
            num_bytes_to_send = 4;
            num_bytes_sent = send(sockfd, buffer, num_bytes_to_send, 0);
            break;

#endif /* L7_WIRELESS_PACKAGE*/

          default:
            LOG_MSG("Unknown command %d (0x%08x)\n", cmd, cmd);
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



