/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_mcast.c
*
* Purpose: Implement Layer-2 Multicast Group Management (MGM).
*
* Component: hapi
*
* Comments:
*
* Created by: Andrey Tsigler 06/26/2002
*
*********************************************************************/

#include <string.h>
#include "sysapi.h"
#include "osapi.h"

#include "datatypes.h"
#include "l7_common.h"
#include "comm_mask.h"


#include "log.h"

#include "broad_common.h"
#include "broad_l2_mcast.h"

#include "bcm/mcast.h"
#include "bcm/vlan.h"

#include "bcmx/mcast.h"
#include "bcmx/vlan.h"
#include "bcmx/lplist.h"
#include "l7_usl_bcmx_l2.h"
#include "broad_l2_vlan.h"
#include "broad_l2_std.h"
#include "ibde.h"

#include "logger.h"

extern void hapiBroadLagCritSecEnter(void);
extern void hapiBroadLagCritSecExit(void);

/* Compute member index from USP.
*/

/* Determine if a port is a member of the multicast group.
*/
#define MGM_IS_PORT_MEMBER(mcastGroup, usp) \
			  hapiBroadMgmIsPortMember(mcastGroup, usp)

/* Set a bit indicating that the port is a member of the multicast group.
*/
#define MGM_PORT_MEMBER_SET(mcastGroup, usp) \
			  hapiBroadMgmPortMemberSet(mcastGroup, usp)

/* Clear a bit indicating that the port is not a member of the multicast group.
*/
#define MGM_PORT_MEMBER_CLEAR(mcastGroup, usp) \
			  hapiBroadMgmPortMemberClear(mcastGroup, usp)

typedef struct
{
  L7_BOOL               inUse;
  /* BCM return code for the entry. Any value other than
   * BCM_E_NONE indicates that the entry is not present
   * in the Hw.
   */
  L7_int32              hwStatus; 
  bcmx_mcast_addr_t     mcMacAddr;
  L7_uchar8             *if_member;
  L7_uint32             ipmc_index;
}MCAST_GROUP_LIST_t;


MCAST_GROUP_LIST_t *mcastGroupList = L7_NULLPTR;
void               *mcastGroupListSemaphore = L7_NULLPTR;

/* Buffer for handling multiple groups at a time */
/* Number 256 is derived based on max args for CUSTOMX */
#define HAPI_MGM_MAX_BCM_GROUPS  256  /* Max groups that can be handled in one-shot */
static int   mgmBcmxMcastGroups[HAPI_MGM_MAX_BCM_GROUPS]; 

static L7_uint32    bitmapSize = 0; /* number of bytes in the bitmap */
static L7_uint32    maxInterfaces = 0; /* the max number of interfaces being acted on by mgm */
static L7_uint32    numMcastGroups = 0; /* Number of mcast groups configured */
static L7_int32    numCurrentFailedMcastGroups = 0; /* Current number of failed groups */
static L7_uint32    numTotalMcastGroupFailures = 0; /* Total count of all the failure */

extern DAPI_t *dapi_g;

static L7_BOOL      hapiMcInit = L7_FALSE;
/* When this flag is L7_TRUE, retries of failed entries is attempted */
static L7_BOOL      hapiL2McFailedRetry = L7_FALSE;
static L7_uint32 debugL2mcEnable = 0; 

static L7_RC_t hapiBroadMgmNextPortGet(L7_uint32 index, L7_BOOL first, DAPI_USP_t *port);
static L7_int32 hapiBroadMgmPortIndexFromUsp(DAPI_USP_t *usp);
static L7_RC_t hapiBroadMgmUspFromPortIndexGet(L7_int32 portIndex, DAPI_USP_t *usp);
static L7_BOOL hapiBroadMgmIsPortMember(MCAST_GROUP_LIST_t *mcastGroup, DAPI_USP_t *port); 
static void hapiBroadMgmPortMemberSet(MCAST_GROUP_LIST_t *mcastGroup, DAPI_USP_t *port);
static void hapiBroadMgmPortMemberClear(MCAST_GROUP_LIST_t *mcastGroup, DAPI_USP_t *port);

/*********************************************************************
*
* @purpose Enters a critical section for L2MC.
*
* @returns 
*
* @end
*
*********************************************************************/
void hapiBroadL2McastCritSecEnter(void)
{
  osapiSemaTake(mcastGroupListSemaphore,L7_WAIT_FOREVER); 
}

/*********************************************************************
*
* @purpose Exits a critical section for L2MC.
*
* @returns 
*
* @end
*
*********************************************************************/
void hapiBroadL2McastCritSecExit(void)
{
  osapiSemaGive(mcastGroupListSemaphore);
}

/*********************************************************************
*
* @purpose Indicates whether distribution over LAG members is
*          supported for L2MC entries. This essentially determines
*          whether we add all LAG members to the L2MC entry or 
*          just the first LAG member.
*
* @returns L7_BOOL
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadL2McastLagDistributionSupported()
{
  bcm_chip_family_t board_family                 = 0; 
  static L7_BOOL    first_time                   = L7_TRUE;
  static L7_BOOL    lag_distribution_supported   = L7_TRUE;

  if (first_time == L7_TRUE)
  {                        
    hapiBroadGetSystemBoardFamily(&board_family);
    first_time = L7_FALSE;
    if ((board_family == BCM_FAMILY_DRACO)   ||
        (board_family == BCM_FAMILY_DRACO15) || 
        (board_family == BCM_FAMILY_LYNX)    || 
        (board_family == BCM_FAMILY_TUCANA))
    {
      lag_distribution_supported = L7_FALSE;
    }
  }

  return lag_distribution_supported;
}

/*********************************************************************
*
* @purpose Indicates whether wlan virtual ports are supported
*
* @returns L7_BOOL
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadWlanHardwareFeatureSupport(DAPI_t *dapi_g)
{
  static L7_BOOL    first_time       = L7_TRUE;
  static L7_BOOL    wlan_supported   = L7_FALSE;
  int i;

  if (first_time == L7_TRUE)
  {                        
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      first_time = L7_FALSE;
      if (soc_feature(i, soc_feature_wlan)) 
      {
        wlan_supported = L7_TRUE;
      }
    }

  }

  return wlan_supported;
}

/*********************************************************************
*
* @purpose Re-Init L2 Mcast software
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadL2McastReInit(void)
{
  L7_uint32 i,maxEntries;

  if (hapiMcInit == L7_FALSE) 
    return;

  maxEntries = platMfdbTotalMaxEntriesGet();
  numMcastGroups = 0;

  for (i = 0; i < maxEntries; i++)
  {
    mcastGroupList[i].inUse = L7_FALSE;
    memset(&mcastGroupList[i].mcMacAddr,0,sizeof(bcmx_mcast_addr_t));     
    memset(mcastGroupList[i].if_member,0,bitmapSize);
  }
}

/*********************************************************************
*
* @purpose Add a USP (physical or LAG, or L2 tunnel) to a L2 mcast entry
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2McastUSPJoin(bcm_mac_t mac, bcm_vlan_t vid, 
                                L7_uint32 l2mc_index, 
                                DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  BROAD_PORT_t               *hapiLagMemberPortPtr, *hapiPortPtr;
  usl_bcm_mcast_addr_t        bcmMcastAddr;
  L7_uint32                   i;
  bcmx_lport_t                lport = 0;
  DAPI_PORT_t                *dapiPortPtr;
  int                         rv = BCM_E_NONE;
  DAPI_USP_t                  localUsp;
  int                         modid, modport;

  localUsp = *usp;

  memset(&bcmMcastAddr, 0, sizeof(bcmMcastAddr));

  memcpy(bcmMcastAddr.mac, mac, sizeof(bcm_mac_t));
  memcpy(&(bcmMcastAddr.vid), &vid, sizeof(vid));
  memcpy(&(bcmMcastAddr.l2mc_index), &l2mc_index, sizeof(l2mc_index));

  dapiPortPtr = DAPI_PORT_GET(&localUsp, dapi_g);

  if ((IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE) &&
      hapiBroadL2McastLagDistributionSupported())
  {

    hapiBroadLagCritSecEnter ();
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
      {
        hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

        lport = hapiLagMemberPortPtr->bcmx_lport;

        modid = BCM_GPORT_MODPORT_MODID_GET(lport);
        modport = BCM_GPORT_MODPORT_PORT_GET(lport);
        if (modid < 0 || modport < 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                  "Failed to get modid/port for lport %x: modid %d modport %d\n",
                   lport, modid, modport); 
          continue;
        }

        BCM_PBMP_PORT_ADD(bcmMcastAddr.mod_pbmp[modid], modport);
      }
    }
    hapiBroadLagCritSecExit ();

    rv = usl_bcmx_mcast_join_ports(&bcmMcastAddr);
  }
  else
  {
    if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
    {
      /* Always add first LAG member to the multicast group.
      */
      if (dapiPortPtr->modeparm.lag.memberSet[0].inUse == L7_TRUE)
      {
        localUsp = dapiPortPtr->modeparm.lag.memberSet[0].usp;
      } else
      {
        /* if there are no members in this LAG now, return */
        return L7_SUCCESS;
      }
    }

    hapiPortPtr = HAPI_PORT_GET(&localUsp, dapi_g);

    /* Get the logical port */
    lport = hapiPortPtr->bcmx_lport;


    if (!BCM_GPORT_IS_WLAN_PORT(lport))
    {
      modid = BCM_GPORT_MODPORT_MODID_GET(lport);
      modport = BCM_GPORT_MODPORT_PORT_GET(lport);
      if (modid < 0 || modport < 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
               "Failed to get modid/port for lport %x: modid %d modport %d\n",
               lport, modid, modport); 
      }
      else
      {
        BCM_PBMP_PORT_ADD(bcmMcastAddr.mod_pbmp[modid], modport);
         rv = usl_bcmx_mcast_join_ports(&bcmMcastAddr);
      }
    }
    else
    {
      BROAD_WLAN_PBMP_SET(bcmMcastAddr.wlan_pbmp, BROAD_WLAN_GPORT_TO_ID(lport));
      rv = usl_bcmx_mcast_join_ports(&bcmMcastAddr);
    }

  }

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Remove a USP (physical or LAG) from a L2 mcast entry
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2McastUSPLeave(bcm_mac_t mac, bcm_vlan_t vid, 
                                 L7_uint32 l2mc_index,
                                 DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  BROAD_PORT_t               *hapiLagMemberPortPtr, *hapiPortPtr;
  usl_bcm_mcast_addr_t         bcmMcastAddr;
  L7_uint32                   i;
  bcmx_lport_t                lport = 0;
  DAPI_PORT_t                *dapiPortPtr;
  int                         rv = BCM_E_NONE;
  DAPI_USP_t                  localUsp;
  int                         modid, modport;

  memset(&bcmMcastAddr, 0, sizeof(bcmMcastAddr));

  memcpy(&(bcmMcastAddr.mac), mac, sizeof(bcm_mac_t));
  memcpy(&(bcmMcastAddr.vid), &vid, sizeof(vid));
  memcpy(&(bcmMcastAddr.l2mc_index), &l2mc_index, sizeof(l2mc_index));

  localUsp = *usp;

  dapiPortPtr = DAPI_PORT_GET(&localUsp, dapi_g);

  if ((IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE) &&
      hapiBroadL2McastLagDistributionSupported())
  {
    /* This is a LAG on an XGS3 or later device. All member ports
       are in the mcast group. */
    hapiBroadLagCritSecEnter ();
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
      {
        hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

        lport = hapiLagMemberPortPtr->bcmx_lport;
        modid = BCM_GPORT_MODPORT_MODID_GET(lport);
        modport = BCM_GPORT_MODPORT_PORT_GET(lport);
        if (modid < 0 || modport < 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                  "Failed to get modid/port for lport %x: modid %d modport %d\n",
                   lport, modid, modport); 
          continue;
        }

        BCM_PBMP_PORT_ADD(bcmMcastAddr.mod_pbmp[modid], modport);
      }
    }
    hapiBroadLagCritSecExit ();

    rv = usl_bcmx_mcast_leave_ports(&bcmMcastAddr);
  }
  else
  {
    hapiPortPtr = HAPI_PORT_GET(&localUsp, dapi_g);
    /* Get the logical port */
    /* will not get here unless the hapPortPtr is valid */
    lport = hapiPortPtr->bcmx_lport;

    if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
    {
      /* For LAGs the first port in the list is always the port we use
      ** to send multicasts.
      */
      if (dapiPortPtr->modeparm.lag.memberSet[0].inUse == L7_TRUE)
      {
        hapiPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[0].usp,dapi_g);

        lport = hapiPortPtr->bcmx_lport;
      }
    }

    if (!BCM_GPORT_IS_WLAN_PORT(lport))
    {
      modid = BCM_GPORT_MODPORT_MODID_GET(lport);
      modport = BCM_GPORT_MODPORT_PORT_GET(lport);
      if (modid < 0 || modport < 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
               "Failed to get modid/port for lport %x: modid %d modport %d\n",
               lport, modid, modport); 
      }
      else
      {
        BCM_PBMP_PORT_ADD(bcmMcastAddr.mod_pbmp[modid], modport);
        rv = usl_bcmx_mcast_leave_ports(&bcmMcastAddr);
      }
    }
    else
    {
      BROAD_WLAN_PBMP_SET(bcmMcastAddr.wlan_pbmp, BROAD_WLAN_GPORT_TO_ID(lport));
      rv = usl_bcmx_mcast_leave_ports(&bcmMcastAddr);
    }

  }

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Init L2 Mcast software
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2McastInit(DAPI_t *dapi_g)
{
  L7_RC_t result = L7_FAILURE;
  L7_uint32 i,maxEntries;

  do
  {
    mcastGroupListSemaphore = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);

    if (mcastGroupListSemaphore == NULL) break;

    hapiBroadL2McastCritSecEnter();

    maxEntries = platMfdbTotalMaxEntriesGet();

    mcastGroupList = osapiMalloc(L7_DRIVER_COMPONENT_ID,maxEntries*sizeof(MCAST_GROUP_LIST_t));

    if (mcastGroupList == L7_NULLPTR) break;

    memset((void*)mcastGroupList,0,maxEntries*sizeof(MCAST_GROUP_LIST_t));

    /* For now, only Physical, LAGS, and L2 tunnels can be members of L2 MC groups */
    maxInterfaces = (platUnitMaxPhysicalPortsGet() * platUnitTotalMaxPerStackGet()) + platIntfLagIntfMaxCountGet() + platIntfL2TunnelIntfMaxCountGet();    
    bitmapSize = (maxInterfaces / 8) + 1;

    for (i = 0;i < maxEntries;i++)
    {
      mcastGroupList[i].if_member = osapiMalloc(L7_DRIVER_COMPONENT_ID,bitmapSize);
#if defined(L7_MCAST_PACKAGE) && defined(L7_WIRELESS_PACKAGE)
      if (hapiBroadWlanHardwareFeatureSupport(dapi_g)) 
      {
        mcastGroupList[i].ipmc_index = L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL + i; /* use l3_ipmc hardware table */
      }
      else
      {
        mcastGroupList[i].ipmc_index = i;
      }
#else
      mcastGroupList[i].ipmc_index = i;
#endif

      if (mcastGroupList[i].if_member == L7_NULLPTR) break;

      memset((void*)mcastGroupList[i].if_member,0,bitmapSize);
    }

    if (i < maxEntries) break; /* error allocating if_member for a mc group */

    result = L7_SUCCESS;

  } while (0);

  if (result == L7_SUCCESS)
  {
    hapiMcInit = L7_TRUE;
  }

  if (mcastGroupListSemaphore != L7_NULLPTR)
  {
    hapiBroadL2McastCritSecExit();
  }

  return result;
}

/*********************************************************************
*
* @purpose Hook in the Mcast functionality and create any data
*
* @param   DAPI_PORT_t *dapiPortPtr - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2McastPortInit(DAPI_PORT_t *dapiPortPtr)
{
  L7_RC_t result = L7_SUCCESS;

  dapiPortPtr->cmdTable[DAPI_CMD_GARP_GMRP                         ] = (HAPICTLFUNCPTR_t)hapiBroadGarpGmrp;
  dapiPortPtr->cmdTable[DAPI_CMD_GVRP_GMRP_CONFIG                  ] = (HAPICTLFUNCPTR_t)hapiBroadGvrpGmrpConfig;

  dapiPortPtr->cmdTable[DAPI_CMD_GARP_GROUP_REG_MODIFY             ] = (HAPICTLFUNCPTR_t)hapiBroadGarpGroupRegModify;
  dapiPortPtr->cmdTable[DAPI_CMD_GARP_GROUP_REG_DELETE             ] = (HAPICTLFUNCPTR_t)hapiBroadGarpGroupRegDelete;

  return result;

}

/*********************************************************************
*
* @purpose Takes the dapiCmd info and enables GMRP on a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GARP_GMRP
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.portGmrp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGarpGmrp(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t          result      = L7_SUCCESS;
  DAPI_GARP_MGMT_CMD_t  *cmdGvrpGmrp = (DAPI_GARP_MGMT_CMD_t*)data;
  BROAD_PORT_t          *hapiPortPtr;

  if (cmdGvrpGmrp->cmdData.portGmrp.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    return result;
  }

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (cmdGvrpGmrp->cmdData.portGmrp.enable == L7_TRUE)
  {
    hapiPortPtr->gmrpEnabled = L7_TRUE;
  } else
  {
    hapiPortPtr->gmrpEnabled = L7_FALSE;
  }

  return result;

}

/*********************************************************************
*
* @purpose Enable GMRP on the system
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GVRP_GMRP_CONFIG
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.gvrpGmrpConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGvrpGmrpConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                    result       = L7_SUCCESS;
  DAPI_GARP_MGMT_CMD_t            *garpMgmt     = (DAPI_GARP_MGMT_CMD_t*)data;

  /* Determine if GVRP is enabled. */

  if (garpMgmt->cmdData.gvrpGmrpConfig.gvrp   == L7_TRUE)
  {
    if (garpMgmt->cmdData.gvrpGmrpConfig.enable == L7_TRUE)
      dapi_g->system->gvrpConfigEnabled = TRUE;
    else
      dapi_g->system->gvrpConfigEnabled = FALSE;
  }

  /* Determine if GMRP is enabled. */

  if (garpMgmt->cmdData.gvrpGmrpConfig.gmrp   == L7_TRUE)
  {
    if (garpMgmt->cmdData.gvrpGmrpConfig.enable == L7_TRUE)
      dapi_g->system->gmrpConfigEnabled = TRUE;
    else
      dapi_g->system->gmrpConfigEnabled = FALSE;
  }
  return result;
}

/*********************************************************************
*
* @purpose Add Group Reg entry
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GARP_GROUP_REG_MODIFY
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.groupRegModify
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGarpGroupRegModify(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                     result = L7_SUCCESS;
  DAPI_GARP_MGMT_CMD_t       *garpMgmt = (DAPI_GARP_MGMT_CMD_t*)data;
  DAPI_USP_t                  localUsp;
  L7_int32                    rv = BCM_E_NONE;
  L7_ushort16                 port;
  L7_uint32                   index;
  L7_uint32                   maxEntries = platMfdbTotalMaxEntriesGet();
  BROAD_PORT_t               *hapiPortPtr;
  DAPI_PORT_t                *dapiPortPtr;
  L7_BOOL                     firstPort;
  usl_bcm_mcast_addr_t        bcmMcastAddr;

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  memset(&bcmMcastAddr, 0, sizeof(bcmMcastAddr));

  if (garpMgmt->cmdData.groupRegModify.getOrSet != DAPI_CMD_SET)
  {
    result =  L7_FAILURE;
    return result;
  }

  if (garpMgmt->cmdData.groupRegModify.isStatic == L7_TRUE)
  {
    result =  L7_FAILURE;
    return result;
  }

  hapiBroadL2McastCritSecEnter();

  /* 
   * Make sure that the vlan is created (using this in lieu of bcmx_vlan_port_get
   * because the bcmx routine is costly with a stack)
   * A slight cheat here is that the cpu is a member of all vlans.  If this changes
   * in the future, we will need to change this code
   */
  if (hapiBroadQvlanVlanExists(garpMgmt->cmdData.groupRegModify.vlanId) == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "\nVLAN [%d] does not exist\n",
            garpMgmt->cmdData.groupRegModify.vlanId);

    /* There is timing window between VLAN create and MFDB create operations.
     * When VLAN create is pending, the above call returns L7_FALSE. As the 
     * USL/SDK APIs do not require the VLAN to be present to create an
     * MFDB entry, do not cross check for VLAN. 
     */

    /* hapiBroadL2McastCritSecExit(); */
    /* return L7_FAILURE; */
  }

  /* See if the mcast group exist */
  for (index = 0; index < maxEntries; index++)
  {
    if (mcastGroupList[index].inUse == TRUE)
    {
      if ((memcmp(garpMgmt->cmdData.groupRegModify.grpMacAddr, mcastGroupList[index].mcMacAddr.mac, 6) == 0) &&
          (garpMgmt->cmdData.groupRegModify.vlanId == mcastGroupList[index].mcMacAddr.vid))
        break;
    }
  }

  if (index == maxEntries)
  {

    for (index = 0; index < maxEntries; index++)
    {
      if (mcastGroupList[index].inUse == FALSE)
        break;
    }

    if (index == maxEntries)
    {
      /* no more entries are available */
      hapiBroadL2McastCritSecExit();
      return L7_FAILURE;
    }

    /* allocate a new mcast entry */
    bcmx_mcast_addr_init(&mcastGroupList[index].mcMacAddr, 
                         garpMgmt->cmdData.groupRegModify.grpMacAddr, 
                         garpMgmt->cmdData.groupRegModify.vlanId);

    memset(mcastGroupList[index].if_member,0,bitmapSize);      /* WPJ */

    memcpy(bcmMcastAddr.mac, mcastGroupList[index].mcMacAddr.mac,
           sizeof(bcm_mac_t));
    memcpy(&(bcmMcastAddr.vid), &(mcastGroupList[index].mcMacAddr.vid),
           sizeof(bcm_vlan_t));

    /* add the mcast entry to the HW */
    rv = usl_bcmx_mcast_addr_add(&bcmMcastAddr);

    LOG_DEBUG(LOG_CTX_IGMP,"MFDB Vlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x added",
              bcmMcastAddr.vid,
              bcmMcastAddr.mac[0],bcmMcastAddr.mac[1],bcmMcastAddr.mac[2],bcmMcastAddr.mac[3],bcmMcastAddr.mac[4],bcmMcastAddr.mac[5]);

    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Failed to create Mcast entry %x:%x:%x:%x:%x:%x vid %d in Hardware, rv %d\n",
              bcmMcastAddr.mac[0], bcmMcastAddr.mac[1], bcmMcastAddr.mac[2],
              bcmMcastAddr.mac[3], bcmMcastAddr.mac[4], bcmMcastAddr.mac[5],
              bcmMcastAddr.vid, rv);
      numCurrentFailedMcastGroups++;
      numTotalMcastGroupFailures++;
    }

    mcastGroupList[index].inUse = TRUE;
    mcastGroupList[index].hwStatus = rv;
    mcastGroupList[index].mcMacAddr.l2mc_index = bcmMcastAddr.l2mc_index;
    numMcastGroups++;
  }

  memset(&localUsp, 0x00, sizeof(localUsp));
  firstPort = L7_TRUE;

  /* leave loop */
  while (hapiBroadMgmNextPortGet(index, firstPort, &localUsp) == L7_SUCCESS)
  {
    firstPort = L7_FALSE;

    for (port = 0; port < garpMgmt->cmdData.groupRegModify.numOfPorts; port++)
    {
      if (memcmp(&garpMgmt->cmdData.groupRegModify.ports[port],
                 &localUsp,
                 sizeof(DAPI_USP_t)) == 0)
      {
        /* this usp belongs in the mgm */
        break;
      }
    }

    if (port >= garpMgmt->cmdData.groupRegModify.numOfPorts)
    {
      /* we must not have found the entry, do a leave */
      MGM_PORT_MEMBER_CLEAR(&mcastGroupList[index],&localUsp);

      dapiPortPtr = DAPI_PORT_GET(&localUsp, dapi_g);
      hapiPortPtr = HAPI_PORT_GET(&localUsp,dapi_g);
      if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) &&
          (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE))
      {
        continue;
      }

      /* Add/Remove ports only if the entry was created successfully */
      if (mcastGroupList[index].hwStatus == BCM_E_NONE)
      {
      if (hapiBroadL2McastUSPLeave(mcastGroupList[index].mcMacAddr.mac, 
                                   mcastGroupList[index].mcMacAddr.vid,
                                   mcastGroupList[index].mcMacAddr.l2mc_index, 
                                   &localUsp, dapi_g) != L7_SUCCESS)
      {

          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
                  "Failed to add remove %d/%d/%d from Mcast entry %x:%x:%x:%x:%x:%x vid %d \n",
                  localUsp.unit, localUsp.slot, localUsp.port,
                  mcastGroupList[index].mcMacAddr.mac[0], 
                  mcastGroupList[index].mcMacAddr.mac[1], 
                  mcastGroupList[index].mcMacAddr.mac[2],
                  mcastGroupList[index].mcMacAddr.mac[3], 
                  mcastGroupList[index].mcMacAddr.mac[4], 
                  mcastGroupList[index].mcMacAddr.mac[5],
                  mcastGroupList[index].mcMacAddr.vid);
        }
      }
    }
  }

  /* join loop */
  for (port = 0; port < garpMgmt->cmdData.groupRegModify.numOfPorts; port++)
  {
    localUsp = garpMgmt->cmdData.groupRegModify.ports[port];

    if (MGM_IS_PORT_MEMBER(&mcastGroupList[index], &localUsp))           /* WPJ */
    {
      /* already in our group, check out the next one */
      continue;
    } else
    {
      /* not presently a member, add it */
      MGM_PORT_MEMBER_SET(&mcastGroupList[index], &localUsp);           /* WPJ */
    }

    dapiPortPtr = DAPI_PORT_GET(&localUsp, dapi_g);
    hapiPortPtr = HAPI_PORT_GET(&localUsp,dapi_g);
    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) &&
        (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE))
    {
      continue;
    }

    /* only add port to HW if it is a member of the VLAN */
    if (BROAD_IS_VLAN_MEMBER(&localUsp, mcastGroupList[index].mcMacAddr.vid, dapi_g))
    {


      /* Add/Remove ports only if the entry was created successfully */
      if (mcastGroupList[index].hwStatus == BCM_E_NONE)
      {
      if (hapiBroadL2McastUSPJoin(mcastGroupList[index].mcMacAddr.mac, 
                                  mcastGroupList[index].mcMacAddr.vid, 
                                  mcastGroupList[index].mcMacAddr.l2mc_index,
                                  &localUsp, dapi_g) != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
                  "Failed to add usp %d/%d/%d to Mcast entry %x:%x:%x:%x:%x:%x vid %d \n",
                  localUsp.unit, localUsp.slot, localUsp.port,
                  mcastGroupList[index].mcMacAddr.mac[0], 
                  mcastGroupList[index].mcMacAddr.mac[1], 
                  mcastGroupList[index].mcMacAddr.mac[2],
                  mcastGroupList[index].mcMacAddr.mac[3], 
                  mcastGroupList[index].mcMacAddr.mac[4], 
                  mcastGroupList[index].mcMacAddr.mac[5],
                  mcastGroupList[index].mcMacAddr.vid);
        }
      }
    }
  }

  hapiBroadL2McastCritSecExit();
  return result;
}

/*********************************************************************
*
* @purpose Delete Group Registration entry
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GARP_GROUP_REG_DELETE
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.groupRegDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGarpGroupRegDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t               result = L7_SUCCESS;
  DAPI_GARP_MGMT_CMD_t       *garpMgmt = (DAPI_GARP_MGMT_CMD_t*)data;
  L7_int32                    rv;
  L7_uint32                   index;
  usl_bcm_mcast_addr_t        bcmMcastAddr;
  L7_uint32 maxEntries = platMfdbTotalMaxEntriesGet();

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  memset(&bcmMcastAddr, 0, sizeof(bcmMcastAddr));

  if (garpMgmt->cmdData.groupRegDelete.getOrSet != DAPI_CMD_SET)
  {
    result =  L7_FAILURE;
    return result;
  }

  if (garpMgmt->cmdData.groupRegDelete.isStatic == L7_TRUE)
  {
    result =  L7_FAILURE;
    return result;
  }


  hapiBroadL2McastCritSecEnter();

  for (index = 0; index < maxEntries; index++)
  {
    if ((memcmp(mcastGroupList[index].mcMacAddr.mac, garpMgmt->cmdData.groupRegDelete.grpMacAddr, 6) == 0)
        && (mcastGroupList[index].mcMacAddr.vid == garpMgmt->cmdData.groupRegDelete.vlanId))
    {
      memcpy(bcmMcastAddr.mac, garpMgmt->cmdData.groupRegDelete.grpMacAddr,
            sizeof(bcm_mac_t));
      memcpy(&(bcmMcastAddr.vid), &(garpMgmt->cmdData.groupRegDelete.vlanId),
             sizeof(bcm_vlan_t));

      bcmMcastAddr.l2mc_index = mcastGroupList[index].mcMacAddr.l2mc_index;

      /* remove the entry from HW */
      rv = usl_bcmx_mcast_addr_remove(&bcmMcastAddr);

      LOG_DEBUG(LOG_CTX_IGMP,"MFDB Vlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x removed",
                bcmMcastAddr.vid,
                bcmMcastAddr.mac[0],bcmMcastAddr.mac[1],bcmMcastAddr.mac[2],bcmMcastAddr.mac[3],bcmMcastAddr.mac[4],bcmMcastAddr.mac[5]);

      if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_NOT_FOUND))
      {
        /* If we failed to delete, it is possible that the entry was never 
         * added successfully. Return failure only when an existing entry
         * in hardware could not be deleted.
         */
        if (mcastGroupList[index].hwStatus == BCM_E_NONE)
        {
          result =  L7_FAILURE;
        }
      }

      numMcastGroups--;
      mcastGroupList[index].inUse = FALSE;
      if (mcastGroupList[index].hwStatus != BCM_E_NONE)
      {
        numCurrentFailedMcastGroups--;  
      }

      mcastGroupList[index].hwStatus = BCM_E_NONE;
      bcmx_lplist_free(&(mcastGroupList[index].mcMacAddr.ports));
      bcmx_lplist_free(&(mcastGroupList[index].mcMacAddr.untag_ports));

      memset(&mcastGroupList[index].mcMacAddr, 0, sizeof(mcastGroupList[index].mcMacAddr));
      memset(mcastGroupList[index].if_member,0,bitmapSize);        /* WPJ */
      break;
    }
  }

  hapiBroadL2McastRetryFailures();

  hapiBroadL2McastCritSecExit();
  return result;
}

/*********************************************************************
* @purpose  Notify MGM that a port or LAG or L2 tunnel was added to VLAN in hardware.
*
* @param    *usp
* @param    vlan_id
* @param    *dapi_g
*
* @returns  none
*
* @notes    The function must NOT be called for LAG members.
*
*  @end
*********************************************************************/
void hapiBroadMgmPortVlanAddNotify (DAPI_USP_t *usp, L7_ushort16 vlan_id, DAPI_t *dapi_g)
{
  L7_uint32   index;
  L7_uint32   maxEntries = platMfdbTotalMaxEntriesGet();
  L7_BOOL     retryFailures = L7_FALSE; 

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  hapiBroadL2McastCritSecEnter();

  /* If no multicast groups are configured then return */
  if (numMcastGroups == 0) 
  {
    hapiBroadL2McastCritSecExit();
    return;
  }

  for (index = 0; index < maxEntries; index++)
  {
    if (mcastGroupList[index].inUse == L7_FALSE)
    {
      continue;
    }
    if (mcastGroupList[index].mcMacAddr.vid != vlan_id)
    {
      continue;
    }

    /* check whether this port is supposed to be in mcast table */
    if (MGM_IS_PORT_MEMBER(&mcastGroupList[index], usp))
    {
      
      if (mcastGroupList[index].hwStatus == BCM_E_NONE)
      {
      if (hapiBroadL2McastUSPJoin(mcastGroupList[index].mcMacAddr.mac,
                                  mcastGroupList[index].mcMacAddr.vid,
                                  mcastGroupList[index].mcMacAddr.l2mc_index,
                                  usp, dapi_g) != L7_SUCCESS)
      {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s!\n",
                       __FILE__, __LINE__, __FUNCTION__);
      }
    }
      else
      {
        retryFailures = L7_TRUE;
      }
    }
  }

  if (retryFailures == L7_TRUE)
  {
    hapiBroadL2McastRetryFailures();
  }

  hapiBroadL2McastCritSecExit();
  return;
}

/*********************************************************************
* @purpose  Notify MGM that a port or LAG or L2 tunnel was removed from VLAN hardware.
*
* @param    *usp
* @param    vlan_id
* @param    *dapi_g
*
* @returns  none
*
* @notes    The function must NOT be called for LAG members.
*
* @end
*********************************************************************/
void hapiBroadMgmPortVlanRemoveNotify (DAPI_USP_t *usp, L7_ushort16 vlan_id, DAPI_t *dapi_g)
{
  L7_uint32            index;
  L7_uint32 maxEntries = platMfdbTotalMaxEntriesGet();

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  hapiBroadL2McastCritSecEnter();

  /* If no multicast groups are configured then return */
  if (numMcastGroups == 0) 
  {
    hapiBroadL2McastCritSecExit();
    return;
  }

  for (index = 0; index < maxEntries; index++)
  {
    if (mcastGroupList[index].inUse == L7_FALSE)
    {
      continue;
    }
    if (mcastGroupList[index].mcMacAddr.vid != vlan_id)
    {
      continue;
    }

    /* check whether this port is supposed to be in mcast table */
    if ((MGM_IS_PORT_MEMBER(&mcastGroupList[index], usp)) && 
        (mcastGroupList[index].hwStatus == BCM_E_NONE))
    {
      if (hapiBroadL2McastUSPLeave(mcastGroupList[index].mcMacAddr.mac,
                                   mcastGroupList[index].mcMacAddr.vid,
                                   mcastGroupList[index].mcMacAddr.l2mc_index,
                                   usp, dapi_g) != L7_SUCCESS)
      {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s!\n",
                       __FILE__, __LINE__, __FUNCTION__);
      }
    }
  }
  hapiBroadL2McastCritSecExit();
  return;
}

/*********************************************************************
* @purpose  Notify MGM that a port was added to a LAG.
*
* @param    *memberUsp
* @param    *lagUsp
* @param    *dapi_g
*
* @returns  none
*
*  @end
*********************************************************************/
void hapiBroadMgmLagMemberAddNotify (DAPI_USP_t *memberUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_uint32            index, count=0;
  L7_uint32 maxEntries = platMfdbTotalMaxEntriesGet();
  L7_int32       rv;
  BROAD_PORT_t  *hapiPortPtr;
  bcm_gport_t port;

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  memset(mgmBcmxMcastGroups, 0, sizeof(mgmBcmxMcastGroups));

  hapiPortPtr = HAPI_PORT_GET(memberUsp, dapi_g);

  port = hapiPortPtr->bcmx_lport;

  if (hapiBroadL2McastLagDistributionSupported() == L7_TRUE)
  {

    for (index = 0; index < maxEntries; index++)
    {
      if (mcastGroupList[index].inUse == L7_FALSE)
      {
        continue;
      }

      /* check whether this port is supposed to be in mcast table */
      if ((MGM_IS_PORT_MEMBER(&mcastGroupList[index], lagUsp)) &&
          (mcastGroupList[index].hwStatus == BCM_E_NONE))
      {
        /* Instead of using hapiBroadL2McastUSPJoin, which handles one entry
         * at a time, use the multi-entry CUSTOMX call to improve performance
         */
        mgmBcmxMcastGroups[count] = mcastGroupList[index].mcMacAddr.l2mc_index;
        count++;

        if (count == HAPI_MGM_MAX_BCM_GROUPS)
        {
          /* time to commit */
          rv = usl_bcmx_mcast_port_join_groups(port, mgmBcmxMcastGroups, count);

          if (rv != L7_SUCCESS)
          {
            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s !\n",
                         __FILE__, __LINE__, __FUNCTION__);
          }  

          count = 0;
          memset(mgmBcmxMcastGroups, 0, sizeof(mgmBcmxMcastGroups));
        }
      }
    }

    if (count != 0)
    {
      rv = usl_bcmx_mcast_port_join_groups(port, mgmBcmxMcastGroups, count);
      if (rv != L7_SUCCESS)
      {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s !\n",
                      __FILE__, __LINE__, __FUNCTION__);
      }
    }
  }
  return;
}

/*********************************************************************
* @purpose  Notify MGM that a port was removed from a LAG
*
* @param    *memberUsp
* @param    *lagUsp
* @param    *dapi_g
*
* @returns  none
*
* @end
*********************************************************************/
void hapiBroadMgmLagMemberRemoveNotify (DAPI_USP_t *memberUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_int32       rv;
  L7_uint32      index, count=0;
  L7_uint32 maxEntries = platMfdbTotalMaxEntriesGet();
  BROAD_PORT_t  *hapiPortPtr;
  bcm_gport_t port;

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  memset(mgmBcmxMcastGroups, 0, sizeof(mgmBcmxMcastGroups));

  hapiPortPtr = HAPI_PORT_GET(memberUsp, dapi_g);

  port = hapiPortPtr->bcmx_lport;

  if (hapiBroadL2McastLagDistributionSupported() == L7_TRUE)
  {
  
    for (index = 0; index < maxEntries; index++)
    {
      if (mcastGroupList[index].inUse == L7_FALSE)
      {
        continue;
      }
  
      /* check whether this port is supposed to be in mcast table */
      if ((MGM_IS_PORT_MEMBER(&mcastGroupList[index], lagUsp)) &&
          (mcastGroupList[index].hwStatus == BCM_E_NONE))
      {
        /* Instead of using hapiBroadL2McastUSPLeave, which handles one entry
         * at a time, use the multi-entry USL call to improve performance
         */
        mgmBcmxMcastGroups[count] = mcastGroupList[index].mcMacAddr.l2mc_index;
        count++;

        if (count == HAPI_MGM_MAX_BCM_GROUPS)
        {
          /* time to commit */
          rv = usl_bcmx_mcast_port_leave_groups(port, mgmBcmxMcastGroups, count);

          if (rv != L7_SUCCESS)
          {
            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s !\n",
                         __FILE__, __LINE__, __FUNCTION__);
          }

          count = 0;
          memset(mgmBcmxMcastGroups, 0, sizeof(mgmBcmxMcastGroups));
        }
      }
    }

    if (count != 0)
    {
      rv = usl_bcmx_mcast_port_leave_groups(port, mgmBcmxMcastGroups, count);
      if (rv != L7_SUCCESS)
      {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s !\n",
                      __FILE__, __LINE__, __FUNCTION__);
      }
    }
  }
  return;
}

/*********************************************************************
* @purpose  Notify MGM that this port/vlan is tagging
*
* @param    *usp
* @param     vlanId
* @param    *dapi_g
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void hapiBroadMgmTaggingEnable(DAPI_USP_t *usp, L7_uint32 vlanId, DAPI_t *dapi_g)
{
  /* These updates are unnecessary for any chip after StrataII 
  ** (which we no longer support). 
  */

  return;
}

/*********************************************************************
* @purpose  Notify MGM that this port/vlan is not tagging
*
* @param    *usp
* @param     vlanId
* @param    *dapi_g
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void hapiBroadMgmTaggingDisable(DAPI_USP_t *usp, L7_uint32 vlanId, DAPI_t *dapi_g)
{
  /* These updates are unnecessary for any chip after StrataII 
  ** (which we no longer support). 
  */

  return;
}

void hapiBroadDebugL2mcDump()
{
  L7_uint32 i,maxEntries;
  maxEntries = platMfdbTotalMaxEntriesGet();

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  printf("Total number of groups = %d current failed inserts = %d Total failures = %d\n", 
         numMcastGroups, numCurrentFailedMcastGroups, numTotalMcastGroupFailures);
  for (i=0;i <maxEntries ;i++)
  {
    if (mcastGroupList[i].inUse)
    {
      printf("[%d] %2x:%2x:%2x:%2x:%2x:%2x vid %d hwRv %d\n",i,
             mcastGroupList[i].mcMacAddr.mac[0],
             mcastGroupList[i].mcMacAddr.mac[1],
             mcastGroupList[i].mcMacAddr.mac[2],
             mcastGroupList[i].mcMacAddr.mac[3],
             mcastGroupList[i].mcMacAddr.mac[4],
             mcastGroupList[i].mcMacAddr.mac[5],
             mcastGroupList[i].mcMacAddr.vid,
             mcastGroupList[i].hwStatus);
    }

  }
}

/*********************************************************************
* @purpose  Notify MGM to remove this wlan port
*
* @param    *usp
* @param     vlanId
* @param    *dapi_g
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void hapiBroadMgmL2TunnelRemove(DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g)
{
  L7_uint32                   index;
  L7_uint32                   maxEntries = platMfdbTotalMaxEntriesGet();

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);


  hapiBroadL2McastCritSecEnter();

  for (index = 0; index < maxEntries; index++)
  {
    if (mcastGroupList[index].inUse == L7_FALSE)
    {
      continue;
    }

    /* check whether this port is supposed to be in mcast table */
    if ((MGM_IS_PORT_MEMBER(&mcastGroupList[index], usp)) &&
        (mcastGroupList[index].hwStatus == BCM_E_NONE))
    {
      if (hapiBroadL2McastUSPLeave(mcastGroupList[index].mcMacAddr.mac,
                                   mcastGroupList[index].mcMacAddr.vid,
                                   mcastGroupList[index].mcMacAddr.l2mc_index,
                                   usp, dapi_g) != L7_SUCCESS)
      {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s!\n",
                       __FILE__, __LINE__, __FUNCTION__);
      }
    }
  }

  hapiBroadL2McastCritSecExit();
}

/*********************************************************************
* @purpose  Notify MGM to add this wlan port
*
* @param    *usp
* @param     vlanId
* @param    *dapi_g
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void hapiBroadMgmL2TunnelAdd(DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g)
{
  L7_uint32                   index;
  L7_uint32                   maxEntries = platMfdbTotalMaxEntriesGet();

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);


  hapiBroadL2McastCritSecEnter();

  for (index = 0; index < maxEntries; index++)
  {
    if (mcastGroupList[index].inUse == L7_FALSE)
    {
      continue;
    }

    /* check whether this port is supposed to be in mcast table */
    if ((MGM_IS_PORT_MEMBER(&mcastGroupList[index], usp)) &&
        (mcastGroupList[index].hwStatus == BCM_E_NONE))
 
    {
      if (hapiBroadL2McastUSPJoin(mcastGroupList[index].mcMacAddr.mac,
                                  mcastGroupList[index].mcMacAddr.vid,
                                  mcastGroupList[index].mcMacAddr.l2mc_index,
                                  usp, dapi_g) != L7_SUCCESS)
      {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s!\n",
                       __FILE__, __LINE__, __FUNCTION__);
      }
    }
  }

  hapiBroadL2McastCritSecExit();
}


/*********************************************************************
* @purpose  Notify L2 Async task to attempt retry of failed entries
*
* @param    None
*
* @returns  none
*
* @notes       
*
* @end
*********************************************************************/
void hapiBroadL2McastRetryFailures(void)
{

  if (numCurrentFailedMcastGroups > 0)
  {
    hapiL2McFailedRetry = L7_TRUE;

    hapiBroadL2AsyncTaskSignal();
  }

  return;
}

/*********************************************************************
* @purpose  Retry failure of failed Mcast groups
*
* @param    None
*
* @returns  none
*
* @notes   Invoked by L2 Async task. Retries are done only if there
*          is some chance of entries being reinserted successfully
*          in the hardware. The following events trigger retries:
*            - A mcast group deletion
*            - Warm start complete: There could be some transient 
*              failures due to USL db being full during warm start.
*          When these events happen, hapiL2McFailedRetry is set to
*          L7_TRUE.
*              
*
* @end
*********************************************************************/
void hapiBroadL2AsyncMcastRetryFailures(void)
{
  L7_int32               rv;
  L7_uint32              index;
  L7_uint32 maxEntries = platMfdbTotalMaxEntriesGet();
  L7_BOOL                firstPort;
  DAPI_USP_t             localUsp;
  usl_bcm_mcast_addr_t   bcmMcastAddr;

  /* If no failures or nothing has changed, then return */
  if ((numCurrentFailedMcastGroups == 0) ||
      (hapiL2McFailedRetry == L7_FALSE))
  {
    return;    
  }

  hapiBroadL2McastCritSecEnter();

  /* If no multicast groups are configured then return */
  if (numMcastGroups == 0) 
  {
    hapiBroadL2McastCritSecExit();
    return;
  }

  for (index = 0; index < maxEntries; index++)
  {
    if (numCurrentFailedMcastGroups <= 0)
    {
      numCurrentFailedMcastGroups = 0;
      break;         
    }

    if (mcastGroupList[index].inUse == L7_FALSE) 
    {
      continue;
    }

    if (mcastGroupList[index].hwStatus == BCM_E_NONE) 
    {
      continue;
    }

    if (hapiBroadQvlanVlanExists(mcastGroupList[index].mcMacAddr.vid) == L7_FALSE)
    {
      continue;
    }

    memset(&bcmMcastAddr, 0, sizeof(bcmMcastAddr));

    /* Try to create the entry in the Hw */
    memcpy(bcmMcastAddr.mac, mcastGroupList[index].mcMacAddr.mac,
           sizeof(bcm_mac_t));
    memcpy(&(bcmMcastAddr.vid), &(mcastGroupList[index].mcMacAddr.vid),
           sizeof(bcm_vlan_t));

    /* add the mcast entry to the HW */
    rv = usl_bcmx_mcast_addr_add(&bcmMcastAddr);

    LOG_DEBUG(LOG_CTX_IGMP,"MFDB Vlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x added",
              bcmMcastAddr.vid,
              bcmMcastAddr.mac[0],bcmMcastAddr.mac[1],bcmMcastAddr.mac[2],bcmMcastAddr.mac[3],bcmMcastAddr.mac[4],bcmMcastAddr.mac[5]);

    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Retry of Mcast entry %x:%x:%x:%x:%x:%x vid %d insert in Hardware failed, rv %d\n",
              bcmMcastAddr.mac[0], bcmMcastAddr.mac[1], bcmMcastAddr.mac[2],
              bcmMcastAddr.mac[3], bcmMcastAddr.mac[4], bcmMcastAddr.mac[5],
              bcmMcastAddr.vid, rv);
      continue;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
              "Retry of Mcast entry %x:%x:%x:%x:%x:%x vid %d succeeded, rv %d\n",
              bcmMcastAddr.mac[0], bcmMcastAddr.mac[1], bcmMcastAddr.mac[2],
              bcmMcastAddr.mac[3], bcmMcastAddr.mac[4], bcmMcastAddr.mac[5],
              bcmMcastAddr.vid, rv);
      numCurrentFailedMcastGroups--;
      mcastGroupList[index].hwStatus = rv;
      mcastGroupList[index].mcMacAddr.l2mc_index = bcmMcastAddr.l2mc_index;
    }

    /* Add the ports in the hardware */
    memset(&localUsp, 0x00, sizeof(localUsp));
    firstPort = L7_TRUE;

    /* leave loop */
    while (hapiBroadMgmNextPortGet(index, firstPort, &localUsp) == L7_SUCCESS)
    {
      firstPort = L7_FALSE;

      if (hapiBroadL2McastUSPJoin(mcastGroupList[index].mcMacAddr.mac, 
                                  mcastGroupList[index].mcMacAddr.vid, 
                                  mcastGroupList[index].mcMacAddr.l2mc_index,
                                  &localUsp, dapi_g) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
                "Failed to add usp %d/%d/%d to Mcast entry %x:%x:%x:%x:%x:%x vid %d \n",
                localUsp.unit, localUsp.slot, localUsp.port,
                mcastGroupList[index].mcMacAddr.mac[0], 
                mcastGroupList[index].mcMacAddr.mac[1], 
                mcastGroupList[index].mcMacAddr.mac[2],
                mcastGroupList[index].mcMacAddr.mac[3], 
                mcastGroupList[index].mcMacAddr.mac[4], 
                mcastGroupList[index].mcMacAddr.mac[5],
                mcastGroupList[index].mcMacAddr.vid);
      }
    }
  }

  hapiL2McFailedRetry = L7_FALSE;
  hapiBroadL2McastCritSecExit();

  return;
}

void hapiBroadDebugL2mcDebug(L7_uint32 enable)
{
  debugL2mcEnable = enable;
}

/*====================================================================================*/
/*
 
  The port numbers in the if_member mask are mapped as follows:

        --------------------------------------------------------------------
        | Physical interfaces | add'l phys ports    | logical interfaces    |
        |     for 1 box       | for stacked boxes   | for all boxes         |
         -------------------------------------------------------------------           
                              A                    B                       C


                                                  Port Offset
               -----------------------------
              |  physical ports in slot 0   |         0
               -----------------------------
              |  physical ports in slot 1   |
               -----------------------------
              |      .......                |
               -----------------------------
              |   physical ports in slot n  |
    Unit 0  -------------------------------------     M
               -----------------------------        M + 1
              |  physical ports in slot 0   |
               -----------------------------
              |  physical ports in slot 1   |
               -----------------------------
              |      .......                |
               -----------------------------
              |   physical ports in slot n  |        N
    Unit 1  -------------------------------------  
                      ......                        N+1 
               -----------------------------
              |  physical ports in slot 0   |
               -----------------------------
              |  physical ports in slot 1   |
               -----------------------------
              |      .......                |
               -----------------------------
              |   physical ports in slot n  |        P
    Unit N  -------------------------------------      
               -------------------------------    P + 1
              | LAG ports in slot LAG_SLOT_NUM |
               -------------------------------     Logical Unit 0?????

         

 */
/*====================================================================================*/


/*********************************************************************
*
* @purpose Obtain DAPI_USP from port index
*
* @param    index       port index in mask
* @param    first       TRUE to get first port in the given index,
*                       FALSE to get next port after the given port
* @param    *usp        DAPI_USP
*
* @returns  L7_SUCCESS or L7_FAILURE 
*
* @notes   none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadMgmNextPortGet(L7_uint32 index, L7_BOOL first, DAPI_USP_t *port)
{

  L7_BOOL     nextPortFound;
  L7_int32    i;
  L7_int32    portOffset = 0;
  L7_int32    startOffset = 0;

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  /* Convert given DAPI_USP to an port bit offset */

  if (first == L7_TRUE)
  {
    startOffset = 0;
  }
  else
  {
    portOffset = hapiBroadMgmPortIndexFromUsp(port);
    startOffset = portOffset + 1;
  }
  


  /* Find next bit offset which is set */

  nextPortFound = L7_FALSE;
  for (i = (startOffset); i < maxInterfaces; i++)
  {
    if (mcastGroupList[index].if_member[i / 8] & (1 << (i % 8)))
    {
      nextPortFound = L7_TRUE;
      break;
    }
  }

  /* Convert that next port bit offset to a DAPI_USP */
  if (nextPortFound == L7_TRUE)
  {
    return(hapiBroadMgmUspFromPortIndexGet(i, port));
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Given a DAPI USP, return portindex
*
* @param    *usp        DAPI_USP
*
* @returns  index       port index in mask
*
* @notes   Routine assumes only physical ports and LAGs are members
*
* @end
*
*********************************************************************/
static L7_int32 hapiBroadMgmPortIndexFromUsp(DAPI_USP_t *usp)
{
  L7_uint32 portOffset;
  L7_uint32 i;

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  if (usp->slot == L7_LAG_SLOT_NUM)
  {
    /* Dependent upon design point that physical slot numbers 
     *  are always below LAG slot numbers.   
     */
    portOffset = platUnitMaxPhysicalPortsGet() * platUnitTotalMaxPerStackGet() + usp->port;
  } 
  else if (usp->slot == L7_CAPWAP_TUNNEL_SLOT_NUM)
  {
    /* Dependent upon design point that LAG slot numbers 
     *  are always below L2 tunnel slot numbers.   
     */
    portOffset = (platUnitMaxPhysicalPortsGet() * platUnitTotalMaxPerStackGet()) + platIntfLagIntfMaxCountGet() + usp->port;
  } 
  else if (usp->slot >= platSlotMaxPhysicalSlotsPerUnitGet())
  {
    portOffset = maxInterfaces;
  } 
  else
  {
    if (usp->port >= sysapiHpcPhysPortsInSlotGet(usp->slot))
    {
      portOffset = maxInterfaces;
    } 
    else
    {
      /* slot is a valid physical slot */
      /* get offset based on unit */
      if (usp->unit == 0 || usp->unit == 1)
      {
        portOffset = 0;
      }
      else
      {
        portOffset  =  (usp->unit - 1) * (platUnitMaxPhysicalPortsGet());
      }
                                                                         
      /* get the offset based on the slot */
      for (i=0; i < usp->slot; i++)
      {
        portOffset += sysapiHpcPhysPortsInSlotGet(i);
      }

      /* add the port */
      portOffset += usp->port;
    }    
  }    

  if (portOffset > maxInterfaces)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%d/%d/%d  on cannot be processed\n", usp->unit,usp->slot,usp->port);
    portOffset = maxInterfaces;
  }

  return(portOffset);
}

/*********************************************************************
*
* @purpose Obtain DAPI_USP from port index
*
* @param    index       port index in mask
* @param    *usp        DAPI_USP
*
* @returns  L7_SUCCESS or L7_FAILURE 
*
* @notes   Routine assumes only physical ports and LAGs are members
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadMgmUspFromPortIndexGet(L7_int32 portIndex, DAPI_USP_t *usp)
{

  L7_uint32 unit = 0;
  L7_uint32 slot = 0;
  L7_uint32 port = 0;
  L7_uint32 slotAndPortOffset;
  L7_uint32 firstOffsetInSlot = 0;
  L7_uint32 lastOffsetInSlot = sysapiHpcPhysPortsInSlotGet(0);
  L7_uint32 lastSlot         = platSlotMaxPhysicalSlotsPerUnitGet();
  L7_uint32 i;
  L7_uint32 physPortMaxIndex = 0, lagPortMaxIndex = 0;
  L7_BOOL   slotAndPortFound;


  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  if (portIndex >= maxInterfaces) return L7_FAILURE;

  /* Find the highest numbered physical port offset */

  physPortMaxIndex = platUnitMaxPhysicalPortsGet() * platUnitTotalMaxPerStackGet();
  lagPortMaxIndex = (platUnitMaxPhysicalPortsGet() * platUnitTotalMaxPerStackGet()) + platIntfLagIntfMaxCountGet();

  /* Check if port offset belongs to a physical or logical interface */
  if (portIndex >= physPortMaxIndex)
  {
    if (portIndex >= maxInterfaces)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure to convert portIndex value%d\n",
                     portIndex);

      return L7_FAILURE;
    } 

    if (portIndex >= lagPortMaxIndex)
    {
      usp->unit = L7_LOGICAL_UNIT;
      usp->slot = L7_CAPWAP_TUNNEL_SLOT_NUM;
      usp->port = portIndex - lagPortMaxIndex;    /* DAPI USP ports are 0-based */
      return L7_SUCCESS;
    }
    else
    {
      usp->unit = L7_LOGICAL_UNIT;
      usp->slot = L7_LAG_SLOT_NUM;
      usp->port = portIndex - physPortMaxIndex;    /* DAPI USP ports are 0-based */
      return L7_SUCCESS;
    }
  } 
  else /* Convert physical port */
  {
    /* Initialize */
    slotAndPortFound = L7_FALSE;

    /* Find the unit - units are 1 based */
    unit = (portIndex / platUnitMaxPhysicalPortsGet()) + 1;

    /* Find the slot and port for physical ports */
    slotAndPortOffset = portIndex % platUnitMaxPhysicalPortsGet();

    lastSlot            = platSlotMaxPhysicalSlotsPerUnitGet();
    lastOffsetInSlot    = 0;

    for (i=0; i < lastSlot; i++)
    {
      firstOffsetInSlot = lastOffsetInSlot;
      lastOffsetInSlot    += sysapiHpcPhysPortsInSlotGet(i);

      /* if the slot and port offset is within this slot's range, slot is found */
      if (slotAndPortOffset <  lastOffsetInSlot)
      {
        slotAndPortFound = L7_TRUE;
        slot             = i;
        port             = slotAndPortOffset - firstOffsetInSlot;
        break;
      }
    }    /* i < lastSlot */

    if (slotAndPortFound == L7_TRUE)
    {
      usp->unit = unit;
      usp->slot = slot;
      usp->port = port;
      return L7_SUCCESS;
    }
  } /* end physical port */

  return L7_FAILURE;
}



/*********************************************************************
*
* @purpose Determine if port is a member of the list
*
* @param    *mcastGroup  pointer to MCAST_GROUP_LIST_t
* @param    *usp        DAPI_USP
*
* @returns  L7_TRUE or L7_FALSE 
*
* @notes   none
*
* @end
*
*********************************************************************/
static L7_BOOL hapiBroadMgmIsPortMember(MCAST_GROUP_LIST_t *mcastGroup, DAPI_USP_t *port)
{
  L7_int32 portOffset;

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  /* Convert given DAPI_USP to an port bit offset */
  portOffset = hapiBroadMgmPortIndexFromUsp(port);

  if (mcastGroup->if_member[portOffset / 8] & (1 << (portOffset % 8)))
    return L7_TRUE;

  return L7_FALSE;

}


/*********************************************************************
*
* @purpose Set the port in the member list
*
* @param    *mcastGroup  pointer to MCAST_GROUP_LIST_t
* @param    *usp        DAPI_USP
*
* @returns  void
*
* @notes   none
*
* @end
*
*********************************************************************/
static void hapiBroadMgmPortMemberSet(MCAST_GROUP_LIST_t *mcastGroup, DAPI_USP_t *port)
{

  L7_int32 portOffset;

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  /* Convert given DAPI_USP to an port bit offset */

  portOffset = hapiBroadMgmPortIndexFromUsp(port);

  mcastGroup->if_member[portOffset / 8] |= (1 << (portOffset % 8));
}


/*********************************************************************
*
* @purpose Clear the port from the member list
*
* @param    *mcastGroup  pointer to MCAST_GROUP_LIST_t
* @param    *usp        DAPI_USP
*
* @returns  void
*
* @notes   none
*
* @end
*
*********************************************************************/
static void hapiBroadMgmPortMemberClear(MCAST_GROUP_LIST_t *mcastGroup, DAPI_USP_t *port)
{

  L7_int32 portOffset;

  if (hapiMcInit == L7_FALSE) LOG_ERROR(hapiMcInit);

  /* Convert given DAPI_USP to an port bit offset */

  portOffset = hapiBroadMgmPortIndexFromUsp(port);

  mcastGroup->if_member[portOffset / 8] &= ~(1 << (portOffset % 8));

}
