/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv_util.c
*
* @purpose    DiffServ Component system infrastructure
*
* @component  DiffServ
*
* @comments   none
*
* @create     04/23/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#include "l7_flex_packages.h"
#include "l7_diffserv_include.h"
#include "registry.h"
#include "l7utils_api.h"

/*********************************************************************
* @purpose  Determine if the DiffServ component has been initialized
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Indicates diffServSysInit() has allocated and initialized
*           its component data structures, which is the minimum acceptable
*           condition for any DiffServ operation.
*
* @end
*********************************************************************/
L7_BOOL diffServIsInitialized(void)
{
  if (pDiffServInfo_g == L7_NULLPTR)
  {
    /* DiffServ component was not initialized during system initialization */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "WARNING:  Invalid build - QOS DiffServ component not initialized\n");
    L7_assert(1);
    return L7_FALSE;
  }

  return (pDiffServInfo_g->diffServInitialized);
}

/*********************************************************************
* @purpose  Determine if the DiffServ component is ready for operation
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Indicates all system conditions have been met for DiffServ
*           component operation.  This is the next sequential state of
*           readiness beyond component initialization, and covers any
*           dependencies on other parts of the system.
*
* @notes    To allow component preconfiguration, DO NOT check this
*           condition when doing configuration object 'get' and 'set'
*           functions.  This checking is needed, however, for the
*           various 'apply' functions.
*
* @end
*********************************************************************/
L7_BOOL diffServIsReady(void)
{
  if (diffServIsInitialized() != L7_TRUE)
    return L7_FALSE;

  return (pDiffServInfo_g->diffServReady);
}

/*********************************************************************
* @purpose  Get DiffServ component tracing mode
*
* @param    void
*
* @returns  mode        L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 diffServTraceModeGet(void)
{
  return pDiffServInfo_g->diffServTraceMode;
}

/*********************************************************************
* @purpose  Set DiffServ component tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void diffServTraceModeSet(L7_uint32 mode)
{
  pDiffServInfo_g->diffServTraceMode = mode;
  if (pDiffServCfgData_g != L7_NULLPTR)
    pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  if (dsmibSpecialConfigModeGet() != L7_TRUE)
    dsTraceModeApply(mode);
}

/*********************************************************************
* @purpose  Get DiffServ debug message level
*
* @param    msgLvl      @b{(input)} Message level (0 = off, >0 = on)
*
* @returns  none
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @end
*********************************************************************/
L7_uint32 diffServMsgLvlGet(void)
{
  return pDiffServInfo_g->diffServMsgLvl;
}

/*********************************************************************
* @purpose  Set DiffServ debug message level
*
* @param    msgLvl      @b{(input)} Message level (0 = off, >0 = on)
*
* @returns  none
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @end
*********************************************************************/
void diffServMsgLvlSet(L7_uint32 msgLvl)
{
  pDiffServInfo_g->diffServMsgLvl = msgLvl;
  if (pDiffServCfgData_g != L7_NULLPTR)
    pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;
}

/**************************************************************************
* @purpose  Take a semaphore
*
* @param    semId   @{(input)} Semaphore id
* @param    timeout @{(input)} Time to wait (in ticks), or L7_WAIT_FOREVER
*                                or L7_NO_WAIT
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  void
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiSemaTake() instead.
*
* @end
*************************************************************************/
void diffServSemaTake(void *semId, L7_int32 timeout, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  if ((rc = osapiSemaTake(semId, timeout)) != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "Semaphore take failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)semId);
  }
}

/**************************************************************************
* @purpose  Give a semaphore
*
* @param    semId   @{(input)} Semaphore id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  void
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
void diffServSemaGive(void *semId, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  if ((rc = osapiSemaGive(semId)) != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "Semaphore give failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)semId);
  }
}

/*********************************************************************
* @purpose  Show the DiffServ feature support list for this platform
*
* @param    none
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/

static char *diffServFeatureString[L7_DIFFSERV_FEATURE_ID_TOTAL] =
{
  "",                                           /* not used */

  "Class Type All................... %s\n",
  "Class Type Any................... %s\n",
  "Class Type Acl................... %s\n",

  "Class Layer 3 Protocol IPv4...... %s\n",
  "Class Layer 3 Protocol IPv6...... %s\n",

  "Match COS........................ %s\n",
  "Match COS2....................... %s\n",
  "Match Dst IP..................... %s\n",
  "Match Dst L4 Port................ %s\n",
  "Match Dst MAC.................... %s\n",
  "Match Ethertype.................. %s\n",
  "Match Every...................... %s\n",
  "Match IPv6 Dst IPv6.............. %s\n",
  "Match IPv6 Dst L4 Port........... %s\n",
  "Match IPv6 Dst L4 Port Range..... %s\n",
  "Match IPv6 Flow Label............ %s\n",
  "Match IPv6 DSCP.................. %s\n",
  "Match IPv6 Protocol.............. %s\n",
  "Match IPv6 Src IPv6.............. %s\n",
  "Match IPv6 Src L4 Port........... %s\n",
  "Match IPv6 Src L4 Port Range..... %s\n",
  "Match IPv6 & MAC in Same Class... %s\n",
  "Match IP DSCP.................... %s\n",
  "Match IP Precedence.............. %s\n",
  "Match IP TOS..................... %s\n",
  "Match Protocol................... %s\n",
  "Match Reference Class............ %s\n",
  "Match Src IP..................... %s\n",
  "Match Src L4 Port................ %s\n",
  "Match Src MAC.................... %s\n",
  "Match VLAN ID.................... %s\n",
  "Match VLAN ID2................... %s\n",
  "Match Exclude.................... %s\n",
  "Supports Masking................. %s\n",
  "Supports Ranges.................. %s\n",

  "Policy Out Class Unrestricted.... %s\n",
  "Policy with IPv4 & IPv6 Classes.. %s\n",

  "Policy Attr Assign Queue......... %s\n",
  "Policy Attr Drop................. %s\n",
  "Policy Attr Mark COS............. %s\n",
  "Policy Attr Mark COS2............ %s\n",
  "Policy Attr Mark COS as COS2..... %s\n",
  "Policy Attr Mark IP DSCP......... %s\n",
  "Policy Attr Mark IP Precedence... %s\n",
  "Policy Attr Mirror............... %s\n",
  "Policy Attr Police Simple........ %s\n",
  "Policy Attr Police Single Rate... %s\n",
  "Policy Attr Police Two Rate...... %s\n",
  "Policy Attr Police Color Aware... %s\n",
  "Policy Attr Redirect............. %s\n",
  "Policy Attr Mark COS&Assgn Queue. %s\n",
  "Policy Attr Mark COS by Policer.. %s\n",

  "Service In Slot/Port............. %s\n",
  "Service In All Ports............. %s\n",
  "Service Out Slot/Port............ %s\n",
  "Service Out All Ports............ %s\n",
  "Service Supports LAG Intf........ %s\n",
  "Service Supports Tunnel Intf..... %s\n",
  "Service ACL Coexist.............. %s\n",

  "PHB EF........................... %s\n",
  "PHB AF4X......................... %s\n",
  "PHB AF3X......................... %s\n",
  "PHB AF2X......................... %s\n",
  "PHB AF1X......................... %s\n",
  "PHB CS........................... %s\n",
  "PHB other........................ %s\n"
};

#define DIFFSERV_PLATFORM_NAME_LEN      128

void diffServFeatureShow(void)
{
  L7_uint32     compId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
  L7_uint32     msgLvlReqd;
  char          platName[DIFFSERV_PLATFORM_NAME_LEN+1];
  L7_uint32     i;
  char          *pYes = "Yes", *pNo = "No";

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  if (sysapiRegistryGet(NPD_TYPE_STRING, STR_ENTRY, platName) != L7_SUCCESS)
    strcpy(platName, "Unknown");

  if (cnfgrFlexIsFeaturePresent(compId, L7_DIFFSERV_FEATURE_SUPPORTED)
      == L7_FALSE)
  {
    DIFFSERV_PRT(msgLvlReqd,
                 "\nDiffServ not supported on platform: %s\n\n", platName);
    return;
  }

  DIFFSERV_PRT(msgLvlReqd,
               "\nDIFFSERV FEATURE SUPPORT LIST (platform: %s)\n\n", platName);

  for (i = 1; i < L7_DIFFSERV_FEATURE_ID_TOTAL; i++)
  {
    DIFFSERV_PRT(msgLvlReqd, diffServFeatureString[i],
            (cnfgrFlexIsFeaturePresent(compId, i) == L7_TRUE) ? pYes : pNo);
  }

  DIFFSERV_PRT(msgLvlReqd, "\n");
}
