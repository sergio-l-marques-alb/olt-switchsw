/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ospf_vend_start.c
*
* @purpose   OSPF Routing vendor-specific startup routines
*
* @component OSPF Mapping Layer
*
* @comments  none
*
* @create    04/12/2001
*
* @author    gpaussa
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include <stdio.h>

#include "ospf_vend_ctrl.h"
#ifdef L7_NSF_PACKAGE
#include "ospf_ckpt.h"
#endif
#include "l7_ospf_api.h"
#include "ospf_util.h"
#include "rto_api.h"
#include "spcfg.h"
#include "l7_ip_api.h"
#include "l7_socket.h"
#include "l3end_api.h"
#include "osapi_support.h"
#include "osapi_trace.h"
#include "ipstk_api.h"
#include "nimapi.h"
#include "trap_layer3_api.h"

#include "buffer.ext"
#include "frame.h"
#include "frame.ext"

extern ospfIntfInfo_t     *pOspfIntfInfo;
extern L7_ospfMapCfg_t    *pOspfMapCfgData;
extern ospfVlinkInfo_t    *pOspfVlinkInfo;
extern ospfInfo_t         *pOspfInfo;
extern L7_uint32 tosToTosIndex[L7_RTR_MAX_TOS];


/* OSPF mapping control context (global) */
ospfMapCtrl_t   ospfMapCtrl_g;

/* OSPF mapping mib control context (global) */
ospfMapMibCtrl_t   ospfMapMibCtrl_g;

/* local manager array -- one per OSPF router interface */
static ospfMapLocalMgr_t  *localMgrPtr[OSPF_MAP_RTR_INTF_MAX+1];


/* error counters */
L7_uint32 ospfSockRxErrBadDest = 0;
L7_uint32 ospfSockRxErrTooShort = 0;
L7_uint32 ospfSockRxErrNoIfo = 0;
L7_uint32 ospfSockRxErrNoConn = 0;

/* OSPF Rx Stats Debug */
L7_uint32 ospfRxStats[255];
#define OSPF_RX_UNICAST 100
void ospfDebugRxStatsShow();
void ospfDebugRxStatsClear(L7_uint32 val);

/* debug message control */
/* 0x01 - Tx, 0x02 - Rx, 0x03 - both */
static L7_uint32 ospfTxRxTrace = 0;           /* turn off/on Tx and Rx messages */
static byte print_str[1024];

/* internal (private) function prototypes */
static void ospf_AreaCB(t_Handle ospfId, t_Handle area_Id,
                        t_Handle *p_areaUserId);
static void ospf_NeighborCB(t_Handle areaUserId, t_Handle objId,
                            t_Handle *p_nbUserId);
void ospf_NeighborFull(t_Handle NBO_Id);
void ospf_DrElected(t_Handle IFO_Id);
static void ospf_TrapCB(t_Handle ospfId, e_OspfTrapType TrapType,
                        u_OspfTrap *p_Trap);
static e_Err ospf_RTChanged(t_Handle RtoMngId, e_OspfDestType DestType,
                           e_RtChange ChangeType, SP_IPADR EntryAdr, SP_IPADR IpMask,
                           word PathNum, ulng PathCost,
                           t_OspfPathPrm *PathPrm, ulng PathType, Bool isRejectRoute);
static void ospf_GetLLOperStatusCB(t_Handle RtoMngId, t_LIH intIfNum,
                                   Bool *operStatus);

/* internal (private) function prototypes for ospf mib support */
static t_Handle ospf_IfLowLayerIdMCB(ulng ifIndex);
static e_Err ospf_IfParamsMCB(ulng ifIndex, t_ospfMibIfLowParams *ifLowParams);
static ulng ospf_IfMaskMCB(ulng ifIndex, ulng ifIpAddress);
static e_Err ospf_HostRouteMCB(ulng HostIpAddress, t_LIH *ifIndex);
static e_Err ospf_ifoCreateDelMCB(t_Handle rtrMng, t_Handle rtrObj,
                             ulng ifIpAddr, ulng ifIndex,
                             Bool ifoCreatDel, t_Handle IfObj);


static e_Err sockTransmit(t_Handle Id, void *p_Frame, SP_IPADR DstAdr, t_LIH tmp);
static e_Err sockReceive(t_Handle userId, void *p_Frame, ulng remoteAddress,
                         word remotePort, ulng intIfNum);
static ulng receiveQueue(t_Handle userId, void *p_Frame);
static void sockStatus( t_Handle userId, t_Handle connId, word status);
static void ospfMapCtrlCtxInit(void);
static void ospfMapLocalMgrSetup(ospfMapLocalMgr_t *lm, L7_uint32 index,
                                 L7_uint32 intIfNum);
static void ospfMapAuthKeyElemInit(t_OspfAuthKey *authKeyCfg);
static e_Err ospf_RtoUpdateDone(t_Handle RtoMngId);

/* Function to enqueue the creation of an IFO to the vendor ospf thread */
extern e_Err IFO_Config_Pack(t_Handle IFO_Id, t_IFO_Cfg *p_Cfg);
extern e_Err IFO_Delete_Pack(t_Handle IFO_Id, ulng flag, L7_uint32 intIfNum);
extern e_Err IFO_Up_Pack(t_Handle IFO_Id);
extern e_Err IFO_Down_Pack(t_Handle IFO_Id);
extern e_Err IFO_MetricConfig_Pack(t_Handle IFO_Id, t_S_IfMetric *p_Cfg);
extern e_Err IFO_AuthKeyConfig_Pack(t_Handle IFO_Id, t_OspfAuthKey *p_Cfg);
extern e_Err l7SocketSetOpt(IN t_Handle connId, int level, int optname, char *optval, int optlen);
extern e_Err ARO_Delete_Pack(t_Handle ARO_Id, ulng flag);
extern e_Err RTO_Config_Pack(t_Handle RTO_Id, t_S_RouterCfg *p_Cfg);

/* make sure auth key maximums are the same for internal and vendor code */
#if (L7_AUTH_MAX_KEY_OSPF_SIMPLE != OSPF_MAP_EXTEN_AUTH_MAX_KEY_SIMPLE)
#error OSPF auth max key simple definition mismatch!
#endif

#if (L7_AUTH_MAX_KEY_OSPF_MD5 != OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5)
#error OSPF auth max key md5 definition mismatch!
#endif

/* NSSA functions */
extern L7_RC_t ospfMapExtenNSSAConfigurationGet(L7_uint32 areaId, t_S_NssaCfg *cfg);

/* dont pull in vxworks for this */
#define IPTOS_PREC_INTERNETCONTROL 0xc0

/*********************************************************************
* @purpose  Initializes the OSPF task.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Causes Routing subsystem to be started if it is not already.
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfTaskCreate(void)
{
  e_Err             erc;
  static const char *routine_name = "ospfMapOspfTaskCreate()";
  L7_uint32 queueLengths[OSPF_NUM_QUEUES];
  L7_uint32 queueWeights[OSPF_NUM_QUEUES];
  L7_uint32 ackQLen;

  /* initialize globals */
  ospfMapCtrlCtxInit();

  /* Create the ospf user task */
  /* Limit the low priority data queue to 500 messages. LS Updates take about 10 ms
   * to process. So we can only do 500 Updates in the default retx time. We also
   * limit this queue length to limit the number of routing heap memory chunks that
   * can be taken by incoming messages. */
  queueLengths[OSPF_EVENT_QUEUE] = 1000;       /* don't want this to overflow */
  queueLengths[OSPF_DATA_QUEUE] = 500;
  /* Size of this queue driven by number of LS ACKs we might receive in a burst.
   * If we send max LSAs to a neighbor, we'd expect about (max LSAs / 72) LS ACK packets.
   * 72 LSA headers fit in a 1500 byte ACK packet.
   * Allow for several neighbors. If we overflow, we'll just retx, or process
   * LSA again. Some platforms don't support many routes; so set a min q length. */
  ackQLen = (OSPF_MAX_LSAS * OSPF_MAX_NBRS_EXCH_LOAD) / 72;
  if (ackQLen < 150)
  {
    ackQLen = 150;
  }
  queueLengths[OSPF_ACK_QUEUE] = ackQLen;
  queueLengths[OSPF_HELLO_QUEUE] = 500;

  /* Make sure we service hellos. ACKs take next priority. Config and timer
   * events next. Last of all, process updates, DDs, and LS reqs. */
  queueWeights[OSPF_EVENT_QUEUE] = 3;
  queueWeights[OSPF_DATA_QUEUE] = 1;
  queueWeights[OSPF_ACK_QUEUE] = 10;
  queueWeights[OSPF_HELLO_QUEUE] = 20;

  if (((erc = XX_CreateThreadMQueue(255, NULL, FALSE, &ospfMapCtrl_g.ospfThread,
                                    OSPF_NUM_QUEUES,
                                    queueLengths,
                                    queueWeights,
                                    "OSPF Proto")) != E_OK)
       || (ospfMapCtrl_g.ospfThread == NULL))
  {
    OSPFMAP_ERROR("%s %d: %s: Error initializing OSPF function, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc );
    return L7_FAILURE;
  }

  /*
  Note that here, unlike the usual case, this sync semaphore is
  being created as FULL rather than EMPTY.  This is done to support
  syncing that requires two takes of the sync semaphore for
  synchronization.  Previously, synchronization was only between
  the emWeb thread and the main OSPF thread.  Now that there is
  also an OSPF Mapping Thread, this method will allow all 3 threads
  to synch correctly.  Also, note that while 2 takes would not work
  for mutual-exclusion semaphores, it works well for synchronization.
  */

  ospfMapCtrl_g.ospfSyncSema = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ospfMapCtrl_g.ospfSyncSema == L7_NULLPTR)
  {
    OSPFMAP_ERROR("%s %d: %s: Error initializing OSPF synchronization semaphore\n",
                    __FILE__, __LINE__, routine_name);
    return L7_FAILURE;
  }

  /* clear OSPF debug stats */
  ospfDebugRxStatsClear(1);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deletes the Virata OSPF task.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The Virata ATIC subsystem cannot be stopped, but had it
*           failed the system would have crashed.
*
* @end
*********************************************************************/
void ospfMapOspfTaskDelete(void)
{
  /* Kill the ospf user task */
  XX_KillThread(ospfMapCtrl_g.ospfThread);
  osapiSemaDelete(&ospfMapCtrl_g.ospfSyncSema);
}

/*********************************************************************
* @purpose  Initializes the OSPF component.
*
* @param    p3InitTime - seconds since boot when OSPF last went through p3 init
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is never called before OSPF has received its configuration.
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVendorInit(L7_uint32 p3InitTime)
{
  L7_uint32         i;
  e_Err             erc;
  t_S_Callbacks     callback;
  t_ospfMibCallbacks mibCallback;
  t_S_RouterCfg     rtrCfg;
  L7_RC_t           cfgRc;
  L7_uint32         cfgData, trapFlags;
  t_SysLabel        ospfSysLabel;
  t_IFO             ifo;
  t_RTO             *p_RTO;

  /* Cleanup data structures */
  bzero((char *)&ifo, sizeof(t_IFO));

  /* Send the ospf user thread to the RTO_Init function */
  ospfSysLabel.threadHndle = ospfMapCtrl_g.ospfThread;

  /* init RTO */
  if (((erc = RTO_Init(&ospfSysLabel, &ospfSysLabel, TRUE, &ospfMapCtrl_g.RTO_Id))
       != E_OK) || (ospfMapCtrl_g.RTO_Id == NULL))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to initialize OSPFv2 protocol engine.");
    return L7_FAILURE;
  }
  p_RTO = (t_RTO*) ospfMapCtrl_g.RTO_Id;
  p_RTO->p3InitTime = p3InitTime;

  /* init local manager pointer array */
  /* NOTE: element 0 not used */
  for (i = 0; i <= OSPF_MAP_RTR_INTF_MAX; i++)
    localMgrPtr[i] = L7_NULL;

  /* enable all RTO traps (see spcfg.h for bit flag definitions) */
  /*RTO_TrapControl(ospfMapCtrl_g.RTO_Id, 0x2808a);*/
  if(trapMgrOspfTrapFlagsGet(&trapFlags) == L7_SUCCESS)
    RTO_TrapControl(ospfMapCtrl_g.RTO_Id, trapFlags);

  /* configure OSPF callbacks */
  memset(&callback, 0, sizeof(t_S_Callbacks));
  callback.f_AreaCreateInd = ospf_AreaCB;
  callback.f_NeighborCreateInd = ospf_NeighborCB;
  callback.f_NeighborFullInd = ospf_NeighborFull;
  callback.f_DrElected = ospf_DrElected;
  callback.f_IpForward = sockTransmit;
  callback.p_OspfTrap = ospf_TrapCB;
  callback.p_RoutingTableChanged = ospf_RTChanged;
  callback.f_InitialRtoUpdateDone = ospf_RtoUpdateDone;
  callback.f_GetLLOperStatus = ospf_GetLLOperStatusCB;
  if ((erc = RTO_SetCallbacks(ospfMapCtrl_g.RTO_Id, &callback)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to register OSPFv2 callbacks.");
    return L7_FAILURE;
  }

  /* bind local manager to the OSPF mapping control context */
  if ((erc = RTO_SetMngId(ospfMapCtrl_g.RTO_Id, &ospfMapCtrl_g)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to bind OSPFv2 local manager.");
    return L7_FAILURE;
  }

  /* configure the RTO object -- override certain defaults with our user cfg
   * NOTE:  must copy RTO->Cfg struct to initialize ours to preserve existing
   *        config info
   */
  cfgRc = L7_SUCCESS;
  memcpy(&rtrCfg, &((t_RTO *)ospfMapCtrl_g.RTO_Id)->Cfg,
         sizeof(t_S_RouterCfg));
  cfgRc |= ospfMapRouterIDGet(&cfgData);
  rtrCfg.RouterId = (SP_IPADR)cfgData;
  rtrCfg.AreaBdrRtrStatus = FALSE;
  rtrCfg.ASBdrRtrStatus = ospfMapIsAsbr();
  cfgRc |= ospfMapRfc1583CompatibilityGet(&cfgData);
  rtrCfg.RFC1583Compatibility = (cfgData == L7_ENABLE) ? TRUE : FALSE;
  cfgData |= ipMapRtrTosForwardingModeGet();
  rtrCfg.TOSSupport = (cfgData == L7_ENABLE) ? TRUE : FALSE;

  rtrCfg.RouterStatus = ROW_CREATE_AND_WAIT;

  if ((erc = RTO_Config(ospfMapCtrl_g.RTO_Id, &rtrCfg)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to configure OSPFv2 protocol engine");
    return L7_FAILURE;
  }

  /* OSPF MIB Registration */
  if (((erc = ospfCreateMibObj(ospfMapCtrl_g.RTO_Id, &ospfMapCtrl_g, &ospfMapMibCtrl_g.p_ospfMibObj)) != E_OK)
    || (ospfMapMibCtrl_g.p_ospfMibObj == NULL))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to create OSPFv2 MIB data structures.");
    return L7_FAILURE;
  }

#if L7_MIB_OSPF
  /* Bind OSPF MIB object to RTO object */
  if ((erc = RTO_BindOspfMibObject(ospfMapCtrl_g.RTO_Id, ospfMapMibCtrl_g.p_ospfMibObj)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to bind OSPFv2 MIB object.");
    return L7_FAILURE;
  }
#endif

  /* configure OSPF MIB callbacks */
  memset(&mibCallback, 0, sizeof(t_ospfMibCallbacks));
  mibCallback.f_Get_IfLowLayerId = ospf_IfLowLayerIdMCB;
  mibCallback.f_Get_IfParams = ospf_IfParamsMCB;
  mibCallback.f_Get_IfMask = ospf_IfMaskMCB;
  mibCallback.f_Get_HostRoute = ospf_HostRouteMCB;
  mibCallback.f_ifoCreateDelInd = ospf_ifoCreateDelMCB;
  if ((erc = ospfSetMibCallbacks(ospfMapMibCtrl_g.p_ospfMibObj, &mibCallback)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to register OSPFv2 MIB callbacks.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Uninitialize the OSPF protocol engine. Does the reverse of
*           actions taken in ospfMapOspfVendorInit().
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVendorUninit(void)
{
    t_S_Callbacks     callback;
    t_ospfMibCallbacks mibCallback;
    t_RTO *p_RTO = NULL;
    e_Err erc;
    t_S_RouterCfg rtrCfg;

    memset(&callback, 0, sizeof(t_S_Callbacks));
    memset(&mibCallback, 0, sizeof(t_ospfMibCallbacks));

    ospfSetMibCallbacks(ospfMapMibCtrl_g.p_ospfMibObj, &mibCallback);
    RTO_SetCallbacks(ospfMapCtrl_g.RTO_Id, &callback);
    RTO_TrapControl(ospfMapCtrl_g.RTO_Id, 0);
#if L7_MIB_OSPF
    RTO_BindOspfMibObject(ospfMapCtrl_g.RTO_Id, L7_NULL);
    ospfDestroyMibObj(ospfMapMibCtrl_g.p_ospfMibObj);
    ospfMapMibCtrl_g.p_ospfMibObj = 0;
#endif
    RTO_SetMngId(ospfMapCtrl_g.RTO_Id, L7_NULL);

    /* Delete the RTO object */
    p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
    memset(&rtrCfg, 0, sizeof(rtrCfg));
    memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
    rtrCfg.RouterStatus = ROW_DESTROY;
    if ((erc = RTO_Config_Pack(ospfMapCtrl_g.RTO_Id, &rtrCfg)) != E_OK)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
             "Failure deleting OSPF protocol RTO object.");
      return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Creates an OSPF router interface.
*
* @param    intIfNum    router internal interface number
* @param    ipAddr      IP address of router interface
* @param    netMask     network/subnet mask for router interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The first created interface causes some remaining OSPF init
*           to be completed.
*
* @notes    The function should only be called if OSPF is enabled on an
*           interface as it causes the interface to immediately be enabled
*           in the core OSPF stack.
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfIntfCreate(L7_uint32 intIfNum, L7_uint32 ipAddr,
                              L7_uint32 netMask)
{
  SP_IPADR          ip_addr, net_mask;
  L7_uint32         i;
  e_Err             erc;
  ospfMapLocalMgr_t *lm = NULL;
  L7_uchar8         authKey[L7_AUTH_MAX_KEY_OSPF+1];
  L7_RC_t           cfgRc;
  L7_uint32         cfgData, adminMode;
  t_IFO_Cfg         ifCfg;
  t_S_IfMetric      mCfg;
  L7_uint32         authType, keyLen, keyLenMax, keyId;
  t_OspfAuthKey     authKeyCfg;
  L7_RC_t           rc = L7_FAILURE;
  L7_uint32         metric;
  L7_uchar8         ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8         netMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32         ospfNetType;
  L7_uint32         nimIfType;
  L7_BOOL           passiveMode;
  static const char *routine_name = "ospfMapOspfIntfCreate()";
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  osapiInetNtoa(ipAddr, ipAddrStr);
  osapiInetNtoa(netMask, netMaskStr);
  OSPFMAP_TRACE("%s %d: %s : intf = %d, %s, ipAddr = %s, mask = %s\n",
                __FILE__, __LINE__, routine_name,
                intIfNum, ifName, ipAddrStr, netMaskStr);

  ip_addr = (SP_IPADR)ipAddr;
  net_mask = (SP_IPADR)netMask;

  /* use one-pass control loop for error exit control */
  do
  {
    /* allocate local manager instance */
    /* NOTE: allocate here so localMgrPtr can be updated under lock */
    lm = (ospfMapLocalMgr_t *)XX_Malloc(sizeof(ospfMapLocalMgr_t));
    if (lm == 0)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Unable to allocate local mgr instance for intf %d, %s\n",
                    __FILE__, __LINE__, routine_name, intIfNum, ifName);
      lm = NULL;                        /* set to known bad value */
      break;
    }

    /* scan the local manager array, looking for first unused entry */
    XX_Freeze();
    for (i = 1; i <= OSPF_MAP_RTR_INTF_MAX; i++)
      if (localMgrPtr[i] == L7_NULL)
        break;
    if (i >= OSPF_MAP_RTR_INTF_MAX)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      XX_Thaw();
      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Could not create OSPF intf %d, %s, at capacity\n",
                    __FILE__, __LINE__, routine_name, intIfNum, ifName);
      break;
    }
    localMgrPtr[i] = lm;                /* marks this entry "in use" */
    XX_Thaw();

    /* setup the local mgr element for use with this interface */
    ospfMapLocalMgrSetup(lm, i, intIfNum);

    if (nimGetIntfType(intIfNum, &nimIfType) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failed to get interface type for interface %s", ifName);
      return L7_FAILURE;
    }

    /* We need not add the loopback interface to the ospf multicast addresses,
       as there is no ospf packet sent or received over the loopback */
    if(nimIfType != L7_LOOPBACK_INTF)
    {
      if (dtlIpv4LocalMulticastAddrAdd(intIfNum,
                                       L7_IP_ALL_OSPF_RTRS_ADDR) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                "Failure adding AllSPFRouters multicast address to hardware for "
                "interface %s", ifName);
      }

      if (dtlIpv4LocalMulticastAddrAdd(intIfNum,
                                       L7_IP_ALL_OSPF_DESIG_RTRS_ADDR) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                "Failure adding AllDRouters multicast address to hardware for "
                "interface %s", ifName);
      }

      /* Set for Multicast to OSPFIGP All Routers RFC1700 */
      if ((erc = TcpUdpBind_AddMulticastAddr(ospfMapCtrl_g.ospfConn, intIfNum,
                                             (ulng)OSPF_MAP_MCAST_ALL_RTRS)) != E_OK)
      {
        OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Error adding all routers multicast addr, rc=%d\n",
                      __FILE__, __LINE__, routine_name, erc);
        break;
      }

      /* Set for Multicast to OSPFIGP Designated Routers RFC1700 "224.0.0.6" */
      if ((erc = TcpUdpBind_AddMulticastAddr(ospfMapCtrl_g.ospfConn, intIfNum,
                                             (ulng)OSPF_MAP_MCAST_DES_RTRS)) != E_OK)
      {
        OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Error adding designated router multicast addr, rc=%d\n",
                      __FILE__, __LINE__, routine_name, erc);
        break;
      }
    }

    /* init OSPF interface */
    if ((erc = IFO_Init(ospfMapCtrl_g.RTO_Id, lm, &lm->IFO_Id)) != E_OK)
    {
      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: IFO init error, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc);
      break;
    }
    pOspfIntfInfo[intIfNum].ospfIfExists = L7_TRUE;

    /* configure the IFO object -- override certain defaults with our user cfg
     * NOTE:  must zero out our config struct to properly initialize IFO->Cfg
     */
    cfgRc = L7_SUCCESS;
    memset(&ifCfg, 0, sizeof(ifCfg));
    ifCfg.IpAdr = ip_addr;
    ifCfg.IpMask = net_mask;
    ifCfg.IfIndex = lm->intIfNum;    /* NOTE: code differs from spec */

    cfgRc |= ospfMapIntfEffectiveAreaIdGet(intIfNum, &cfgData);
    ifCfg.AreaId = (SP_IPADR)cfgData;

    cfgRc |= ospfMapIntfVirtTransitAreaIdGet(intIfNum, &cfgData);
    ifCfg.VirtTransitAreaId = (SP_IPADR)cfgData;

    cfgRc |= ospfMapIntfVirtIntfNeighborGet(intIfNum, &cfgData);
    ifCfg.VirtIfNeighbor = (SP_IPADR)cfgData;

    ospfMapIntfTypeGet(intIfNum, &ospfNetType);
    ospfMapVendIfTypeGet(ospfNetType, &ifCfg.Type);

    if(ifCfg.Type == IFO_LOOPBACK)
    {
        ifCfg.DisableFlooding = TRUE; /* disable flooding on loopback i/f */
    }
    else
    {
        ifCfg.DisableFlooding = FALSE;
    }

    cfgRc |= ospfMapIntfPriorityGet(intIfNum, &cfgData);
    ifCfg.RouterPriority = (byte)cfgData;

    cfgRc |= ospfMapIntfTransitDelayGet(intIfNum, &cfgData);
    ifCfg.InfTransDelay = (word)cfgData;

    cfgRc |= ospfMapIntfLsaAckIntervalGet(intIfNum, &cfgData);
    ifCfg.AckInterval = (word)cfgData;

    cfgRc |= ospfMapIntfRxmtIntervalGet(intIfNum, &cfgData);
    ifCfg.RxmtInterval = (word)cfgData;

    cfgRc |= ospfMapIntfHelloIntervalGet(intIfNum, &cfgData);
    ifCfg.HelloInterval = (word)cfgData;

    cfgRc |= ospfMapIntfIpMtuGet(intIfNum, &cfgData);
    ifCfg.MaxIpMTUsize = (word)cfgData;

    cfgRc |= ospfMapIntfMtuIgnoreGet(intIfNum, &cfgData);
    ifCfg.MtuIgnore = (word)cfgData;

    cfgRc |= ospfMapIntfDeadIntervalGet(intIfNum, &cfgData);
    ifCfg.RouterDeadInterval = (word)cfgData;

    cfgRc |= ospfMapIntfNbmaPollIntervalGet(intIfNum, &cfgData);
    ifCfg.NbmaPollInterval = (word)cfgData;

    cfgRc |= ospfMapIntfEffectivePassiveModeGet(intIfNum, &passiveMode);
    ifCfg.passiveMode = passiveMode;

    ifCfg.IfDemand = FALSE;

    ifCfg.AckInterval = (word)FD_OSPF_INTF_DEFAULT_LSA_ACK_INTERVAL;

    ifCfg.MulticastForwarding = IF_MLTCST_BLOCKED;

    cfgRc |= ospfMapIntfAuthTypeGet(intIfNum, &authType);

    /* Note: key is returned as a string here and may be shorter than max length */
    cfgRc |= ospfMapIntfAuthKeyGet(intIfNum, authKey);

    cfgRc |= ospfMapIntfAuthKeyIdGet(intIfNum, &keyId);

    cfgRc |= ospfMapIntfEffectiveAdminModeGet(intIfNum, &adminMode);

    /* check for any errors in gathering config parms */
    if (cfgRc != L7_SUCCESS)
    {
      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Unable to gather router intf config parms\n",
                    __FILE__, __LINE__, routine_name);
      break;                            /* exit control loop */
    }

    /* translate system enum to vendor enum for auth type */
    if (authType == L7_AUTH_TYPE_MD5)
    {
      ifCfg.AuType = IFO_CRYPT;
      keyLenMax = L7_AUTH_MAX_KEY_OSPF_MD5;
    } else if (authType == L7_AUTH_TYPE_SIMPLE_PASSWORD)
    {
      ifCfg.AuType = IFO_PASSWORD;
      keyLenMax = L7_AUTH_MAX_KEY_OSPF_SIMPLE;
    } else
    {
      ifCfg.AuType = IFO_NULL_AUTH;
      keyLenMax = 0;
    }

    /* validate max key length per current auth type */
    keyLen = (L7_uint32)strlen(authKey);
    if (keyLen > keyLenMax)
    {
      OSPFMAP_ERROR("%s: OSPF_MAP: ERROR: Auth Key length=%u invalid\n",
                    routine_name, keyLen);
      break;
    }

    /* init the simple key field */
    memset(ifCfg.AuKey, 0, (size_t)OSPF_MAP_EXTEN_AUTH_MAX_KEY_SIMPLE);
    if (ifCfg.AuType == IFO_PASSWORD)
      memcpy(ifCfg.AuKey, authKey, keyLen);

    /* NOTE:  always set AdminStat=True so that ARO_Id is set up whether
     *          the interface is enabled or not; control the actual
     *          intf enablement through the IfStatus value used
     */
    ifCfg.AdminStat = TRUE;
    if (adminMode == L7_ENABLE)
      ifCfg.IfStatus = ROW_CREATE_AND_GO;
    else
      ifCfg.IfStatus = ROW_CREATE_AND_WAIT;

    if ((erc = IFO_Config_Pack(lm->IFO_Id, &ifCfg)) != E_OK)
    {
      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: IFO config error, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc);
      break;                            /* exit control loop */
    }

    /* configure interface metric */

    if((ospfMapIntfMetricGet(intIfNum, &metric) != L7_SUCCESS) ||
       (metric == L7_OSPF_INTF_METRIC_NOT_CONFIGURED))
    {
      /* If ospfMapIntfMetricGet fails a potential reason might be that at this time
      ** this function may be invoked for port-based routing interfaces
      ** when a link for the interface is down. In that scenario nimGetIntfSpeedDataRate
      ** returns an error. For now we will assume that the link is 100Full.
                                        OR
      ** If the stored value in the configuration indicates that the ospf interface
      ** metric has not been configured by the user then calculate the metric based
      ** on the speed of the interface
      */

      if (ospfMapIntfMetricCalculate(intIfNum, &metric) != L7_SUCCESS)
      {
        OSPFMAP_ERROR("%s %d: %s: ERROR: Unable to calculate ospf intf metric\n",
                      __FILE__, __LINE__, routine_name);
        break;                            /* exit control loop */
      }
    }

    memset(&mCfg, 0, sizeof(mCfg));
    mCfg.IfIndex = lm->intIfNum;     /* NOTE: code differs from spec */
    mCfg.MetricTOS = TOS_NORMAL_SERVICE;
    mCfg.Metric = metric;
    mCfg.MetricStatus = (adminMode == L7_ENABLE) ?
                        ROW_CREATE_AND_GO : ROW_CREATE_AND_WAIT;
    if ((erc = IFO_MetricConfig_Pack(lm->IFO_Id, &mCfg)) != E_OK)
    {
      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Error setting interface metrics, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc);
      break;                            /* exit control loop */
    }

    /* configure a single, permanent MD5 auth key element in the IFO AuthKeyHL */
    /* Note: key length was validated above */
    ospfMapAuthKeyElemInit(&authKeyCfg);
    authKeyCfg.KeyId = (byte)keyId;
    if ((ifCfg.AuType == IFO_CRYPT) && (keyLen > 0))
      memcpy(authKeyCfg.KeyValue, authKey, (size_t)keyLen);
    authKeyCfg.KeyStatus = ROW_CREATE_AND_GO;
    if ((erc = IFO_AuthKeyConfig_Pack(lm->IFO_Id, &authKeyCfg)) != E_OK)
    {
      OSPFMAP_ERROR("%s: ERROR: OSPF_MAP: Error setting intf MD5 auth key entry, rc=%d\n",
                    routine_name, erc);
      break;                            /* exit control loop */
    }

    rc = L7_SUCCESS;

  } while (0);

  /* clean up when operation is unsuccessful */
  if ((rc != L7_SUCCESS) && (lm != NULL))
    ospfMapOspfIntfDelete(intIfNum);

  return rc;
}

/*********************************************************************
* @purpose  Creates an OSPF virtual interface.
*
* @param    index       index into the pOspfVlinkInfo needed to store
*                       the IFO handle returned by the vendor.
* @param    areaId      virtual transit areaId
* @param    neighbor    remote router id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The first created interface causes some remaining OSPF init
*           to be completed.
*
* @notes    The function should only be called if OSPF is enabled on an
*           interface as it causes the interface to immediately be enabled
*           in the core OSPF stack.
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVirtIntfCreate(L7_uint32 index, L7_uint32 areaId,
                                  L7_uint32 neighbor)
{
  e_Err             erc;
  L7_RC_t           cfgRc;
  t_IFO_Cfg         ifCfg;
  L7_uint32         cfgData;
  L7_uchar8         authKey[L7_AUTH_MAX_KEY_OSPF+1];
  L7_uint32         authType, keyLen, keyLenMax, keyId;
  t_OspfAuthKey     authKeyCfg;
  L7_RC_t           rc = L7_FAILURE;

  /* use one-pass control loop for error exit control */
  do
  {
    /* init OSPF interface */
    if ((erc = IFO_Init(ospfMapCtrl_g.RTO_Id, NULL, &pOspfVlinkInfo[index].IFO_Id)) != E_OK)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failed to create OSPFv2 virtual link interface.");
      return L7_FAILURE;
    }

    /* configure the IFO object -- override certain defaults with our user cfg
     * NOTE:  must zero out our config struct to properly initialize IFO->Cfg
     */
    cfgRc = L7_SUCCESS;
    memset(&ifCfg, 0, sizeof(ifCfg));
    ifCfg.IpAdr = 0;
    ifCfg.IpMask = 0;
    ifCfg.IfIndex = (L7_MAX_INTERFACE_COUNT + index);    /* unique ifindex for vlink */
    ifCfg.AreaId = 0;

    /*cfgRc |= ospfMapIntfVirtTransitAreaIdGet(intIfNum, &cfgData);*/
    ifCfg.VirtTransitAreaId = (SP_IPADR)areaId;

    /*cfgRc |= ospfMapIntfVirtIntfNeighborGet(intIfNum, &cfgData);*/
    ifCfg.VirtIfNeighbor = (SP_IPADR)neighbor;

    ifCfg.Type = IFO_VRTL;
    ifCfg.RouterPriority = 0;

    cfgRc |= ospfMapVirtIfTransitDelayGet(areaId, neighbor, &cfgData);
    ifCfg.InfTransDelay = (word)cfgData;

    ifCfg.AckInterval = (word)FD_OSPF_INTF_DEFAULT_LSA_ACK_INTERVAL;

    cfgRc |= ospfMapVirtIfRetransIntervalGet(areaId, neighbor, &cfgData);
    ifCfg.RxmtInterval = (word)cfgData;

    cfgRc |= ospfMapVirtIfHelloIntervalGet(areaId, neighbor, &cfgData);
    ifCfg.HelloInterval = (word)cfgData;

    cfgRc |= ospfMapVirtIfRtrDeadIntervalGet(areaId, neighbor, &cfgData);
    ifCfg.RouterDeadInterval = (word)cfgData;

    ifCfg.NbmaPollInterval = POLL_INTERVAL_DFLT;
    ifCfg.IfDemand = FALSE;

    cfgRc |= ospfMapVirtIfAuthTypeGet(areaId, neighbor, &authType);

    /* Note: key is returned as a string here and may be shorter than max length */
    cfgRc |= ospfMapVirtIfAuthKeyGet(areaId, neighbor, authKey);

    cfgRc |= ospfMapVirtIfAuthKeyIdGet(areaId, neighbor, &keyId);

    /* check for any errors in gathering config parms */
    if (cfgRc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Unable to gather router virtual interface config parameters");
      break;                            /* exit control loop */
    }

    /* translate system enum to vendor enum for auth type */
    if (authType == L7_AUTH_TYPE_MD5)
    {
      ifCfg.AuType = IFO_CRYPT;
      keyLenMax = L7_AUTH_MAX_KEY_OSPF_MD5;
    } else if (authType == L7_AUTH_TYPE_SIMPLE_PASSWORD)
    {
      ifCfg.AuType = IFO_PASSWORD;
      keyLenMax = L7_AUTH_MAX_KEY_OSPF_SIMPLE;
    } else
    {
      ifCfg.AuType = IFO_NULL_AUTH;
      keyLenMax = 0;
    }

    /* validate max key length per current auth type */
    keyLen = (L7_uint32)strlen(authKey);
    if (keyLen > keyLenMax)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Auth Key length=%u invalid", keyLen);
      break;                            /* exit control loop */
    }

    /* init the simple key field */
    memset(ifCfg.AuKey, 0, (size_t)OSPF_MAP_EXTEN_AUTH_MAX_KEY_SIMPLE);
    if (ifCfg.AuType == IFO_PASSWORD)
      memcpy(ifCfg.AuKey, authKey, keyLen);

    /* NOTE:  always set AdminStat=True so that ARO_Id is set up whether
     *          the interface is enabled or not; control the actual
     *          intf enablement through the IfStatus value used
     */
    ifCfg.AdminStat = TRUE;
    ifCfg.IfStatus  = ROW_CREATE_AND_GO;
    ifCfg.DrIpAddr  = 0;
    ifCfg.BackupIpAddr = 0;

    /* Proprietary VPN support is to be set to FALSE and VpnCos to 0 */
    ifCfg.IsVpnSupported = FALSE;
    ifCfg.VpnCos         = 0;

    if ((erc = IFO_Config_Pack(pOspfVlinkInfo[index].IFO_Id, &ifCfg)) != E_OK)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Virtual IFO config error, rc=%d", erc);
      break;                            /* exit control loop */
    }

    /* configure a single, permanent MD5 auth key element in the IFO AuthKeyHL */
    /* Note: key length was validated above */
    ospfMapAuthKeyElemInit(&authKeyCfg);
    authKeyCfg.KeyId = (byte)keyId;
    if ((ifCfg.AuType == IFO_CRYPT) && (keyLen > 0))
      memcpy(authKeyCfg.KeyValue, authKey, (size_t)keyLen);
    authKeyCfg.KeyStatus = ROW_CREATE_AND_GO;
    if ((erc = IFO_AuthKeyConfig_Pack(pOspfVlinkInfo[index].IFO_Id, &authKeyCfg)) != E_OK)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Error setting intf MD5 auth key entry, rc=%d", erc);
      break;                            /* exit control loop */
    }

    rc = L7_SUCCESS;

  } while (0);

  /* clean up when operation is unsuccessful (ignore 'delete' rc) */
  if (rc != L7_SUCCESS)
    (void)ospfMapOspfVirtIntfDelete(index, areaId, neighbor);

  return rc;
}

/*********************************************************************
* @purpose  Deletes an OSPF router interface.
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Cleans up Routing resources associated with the router
*           interface.
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfIntfDelete(L7_uint32 intIfNum)
{
  ospfMapLocalMgr_t *lm;
  e_Err erc;
  L7_uint32 nimIfType;

  static const char *routine_name = "ospfMapOspfIntfDelete()";
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  OSPFMAP_TRACE("%s %d: %s : intf = %d, %s\n",
                __FILE__, __LINE__, routine_name, intIfNum, ifName);

  if (ospfMapLmLookup(intIfNum, &lm) != L7_SUCCESS)
    return L7_FAILURE;

  if (nimGetIntfType(intIfNum, &nimIfType) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to get interface type for interface %s", ifName);
    return L7_FAILURE;
  }
  /* We need not add the loopback interface to the ospf multicast addresses,
     as there is no ospf packet sent or received over the loopback */
  if (nimIfType != L7_LOOPBACK_INTF)
  {
    if (dtlIpv4LocalMulticastAddrDelete(intIfNum,
                                        L7_IP_ALL_OSPF_RTRS_ADDR) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failure removing AllSPFRouters multicast address from hardware for "
              "interface %s", ifName);
    }

    if (dtlIpv4LocalMulticastAddrDelete(intIfNum,
                                        L7_IP_ALL_OSPF_DESIG_RTRS_ADDR) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failure removing AllDRouters multicast address from hardware for "
              "interface %s", ifName);
    }

    /* Set for Multicast to OSPFIGP All Routers RFC1700 */
    if ((erc = TcpUdpBind_DropMulticastAddr(ospfMapCtrl_g.ospfConn, intIfNum,
                                            (ulng)OSPF_MAP_MCAST_ALL_RTRS)) != E_OK)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failure removing AllSPFRouters multicast address from IP stack for "
              "interface %s", ifName);
    }

    /* Set for Multicast to OSPFIGP Designated Routers RFC1700 "224.0.0.6" */
    if ((erc = TcpUdpBind_DropMulticastAddr(ospfMapCtrl_g.ospfConn, intIfNum,
                                            (ulng)OSPF_MAP_MCAST_DES_RTRS)) != E_OK)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Failure removing AllDRouters multicast address from IP stack for "
              "interface %s", ifName);
    }
  }

  if (lm->IFO_Id != NULL)
  {
    IFO_Delete_Pack(lm->IFO_Id, TRUE, intIfNum);
    lm->IFO_Id = NULL;
  }

  /* deallocate the local manager instance and mark the pointer array
   * element as unused.
   */
  XX_Freeze();
  localMgrPtr[lm->index] = L7_NULL;
  XX_Thaw();

  XX_Free(lm);

  pOspfIntfInfo[intIfNum].ospfInitialized = L7_FALSE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deletes OSPF virtual interface.
*
* @param    index       index into the pOspfVlinkInfo needed to store
*                       the IFO handle returned by the vendor.
* @param    areaId      virtual transit areaId
* @param    neighbor    remote router id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Cleans up Routing resources associated with the router
*           interface.
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVirtIntfDelete(L7_uint32 index, L7_uint32 areaId, L7_uint32 neighbor)
{
  IFO_Delete_Pack((t_IFO *)pOspfVlinkInfo[index].IFO_Id, TRUE, 0);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deletes an OSPF area.
*
* @param    areaId      AreaId of the area to be deleted
*
* @returns  L7_SUCCESS  Area was successfully deleted
* @returns  L7_ERROR    Specified area has not been configured
* @returns  L7_FAILURE  Area contains active interface(s) (these
*                       need to be deleted before removing the area)
*
* @notes    Cleans up Routing resources associated with the area
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAreaDelete(L7_uint32 areaId)
{
  t_IFO *p_IFO = NULLP;
  t_ARO *p_ARO = NULLP;
  t_RTO *p_RTO = (t_RTO *)ospfMapCtrl_g.RTO_Id;

  /* If the specified area does not exist return with an error */
  if(HL_FindFirst(p_RTO->AroHl, (byte*)&areaId, (void**)&p_ARO) != E_OK)
    return L7_ERROR;

  /* The area has at least one active interface.  The user needs to delete
  ** the interface before deleting the area
  */
  if(HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO) == E_OK)
    return L7_FAILURE;

  return ARO_Delete_Pack((t_Handle)p_ARO, TRUE);
}

/*********************************************************************
* @purpose  Dynamically changes the interface metric value.
*
* @param    intIfNum    router internal interface number
* @param    ifDataRate  current interface data rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Typically invoked when the interface link speed changes
*           so that the appropriate metric value can be advertised.
*           Assumes this metric can be changed dynamically while the
*           interface exists.
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfIntfMetricChange(L7_uint32 intIfNum, L7_uint32 ifDataRate)
{
  ospfMapLocalMgr_t *lm;
  t_IFO             *p_IFO;
  t_S_IfMetric      mCfg;
  e_Err             erc;

  static const char *routine_name = "ospfMapOspfIntfMetricChange()";

  if (ospfMapLmLookup(intIfNum, &lm) != L7_SUCCESS)
    return L7_SUCCESS;                  /* not an OSPF interface */

  p_IFO = lm->IFO_Id;

  /* NOTE:  If it turns out that a ROW_MODIFY is not the correct operation,
   *        then it is probably necessary to delete and re-create the
   *        IFO interface (which will pick up the latest metric value).
   */

  memset(&mCfg, 0, sizeof(mCfg));
  mCfg.IfIndex = lm->intIfNum;     /* NOTE: code differs from spec */
  mCfg.IpAdr = p_IFO->Cfg.IpAdr;
  mCfg.MetricTOS = TOS_NORMAL_SERVICE;
  mCfg.Metric = (ifDataRate >= 100) ? 1 : 10;
  mCfg.MetricStatus = ROW_MODIFY;
  if ((erc = IFO_MetricConfig_Pack(lm->IFO_Id, &mCfg)) != E_OK)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Error changing interface %d, %s metrics, rc=%d\n",
                  __FILE__, __LINE__, routine_name, intIfNum, ifName, erc);
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Signals router interface UP condition to Routing code.
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfIntfUp(L7_uint32 intIfNum)
{
  ospfMapLocalMgr_t *lm;

  if (ospfMapLmLookup(intIfNum, &lm) != L7_SUCCESS)
    return L7_FAILURE;

  IFO_Up_Pack(lm->IFO_Id);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Signals router interface DOWN condition to Routing code.
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfIntfDown(L7_uint32 intIfNum)
{
  ospfMapLocalMgr_t *lm;

  if (ospfMapLmLookup(intIfNum, &lm) != L7_SUCCESS)
    return L7_FAILURE;

  IFO_Down_Pack(lm->IFO_Id);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Signals virtual interface DOWN condition to Routing code.
*
* @param    index     virtual link configuration index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    A virtual link will get automatically activated and moved
*           to "up" state when the virtual neighbor is discovered
*           hence we do not need the corresponding ospfMapOspfVirtIntfUp
*           function.  However the ospfMapOspfVirtIntfDown function
*           is necessary for the case when a user wants to explicitly
*           deactivate/delete a preconfigured virtual link.
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVirtIntfDown(L7_uint32 index)
{
  if(pOspfVlinkInfo[index].IFO_Id != L7_NULLPTR)
    IFO_Down_Pack(pOspfVlinkInfo[index].IFO_Id);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determines the local manager element from an internal
*           interface number.
*
* @param    intIfNum    router internal interface number
* @param    ppLm        output area to store Local Mgr element pointer
*
* @returns  L7_SUCCESS  Local Mgr element found
* @returns  L7_FAILURE  could not find Local Mgr element for interface
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapLmLookup(L7_uint32 intIfNum, ospfMapLocalMgr_t **ppLm)
{
  L7_uint32         i;
  ospfMapLocalMgr_t *lm;

  for (i = 1; i < OSPF_MAP_RTR_INTF_MAX; i++)
  {
    lm = localMgrPtr[i];
    if ((lm != L7_NULL) && (lm->intIfNum == intIfNum))
    {
      *ppLm = lm;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determines the local manager element for an ip address
*
* @param    ipAddr      ip address
* @param    ppLm        output area to store Local Mgr element pointer
*
* @returns  L7_SUCCESS  Local Mgr element found
* @returns  L7_FAILURE  could not find Local Mgr element for interface
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapLmMatch(L7_uint32 ipAddr, ospfMapLocalMgr_t **ppLm)
{
  L7_uint32         i;
  ospfMapLocalMgr_t *lm;

  for (i = 1; i < OSPF_MAP_RTR_INTF_MAX; i++)
  {
    lm = localMgrPtr[i];
    if ((lm != L7_NULL) &&
        ((lm->ipAddr & lm->ipMask) == (ipAddr & lm->ipMask)))
    {
      *ppLm = lm;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determines the local manager index value from an internal
*           interface number.
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_uint32   Local Mgr index, or 0 if match not found.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ospfMapLmIndexGet(L7_uint32 intIfNum)
{
  ospfMapLocalMgr_t *lm;

  if (ospfMapLmLookup(intIfNum, &lm) == L7_SUCCESS)
    return lm->index;

  return 0;
}

/*********************************************************************
* @purpose  Determines the internal interface number from an local
*           index value
*
* @param    ifIndex     Local manager index value
*
* @returns  L7_uint32   Internal interface value, or 0 if match not found.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ospfMapLmIntIfNumGet(L7_uint32 ifIndex)
{
  L7_uint32 intIfNum;

  if (ifIndex >= OSPF_MAP_RTR_INTF_MAX + 1)
  {
    return 0;
  }
  intIfNum = (localMgrPtr[ifIndex])->intIfNum;
  return intIfNum;
}


/*********************************************************************
* @purpose  Get the internal interface number for a IFO handle.
*
* @param    IFO_Id      IFO object Id
*
* @returns  L7_uint32   Internal interface value, or 0 if match not found.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ospfMapLmIfoIntIfNumGet(t_Handle IFO_Id)
{
  L7_uint32         i;
  ospfMapLocalMgr_t *lm;

  for (i = 1; i < OSPF_MAP_RTR_INTF_MAX; i++)
  {
    lm = localMgrPtr[i];
    if ((lm != L7_NULL) && (lm->IFO_Id == IFO_Id))
    {
      return lm->intIfNum;
    }
  }
  return 0;
}

/* -------------------------------------------------------------------------- */
/* ---------------------------- OSPF Callbacks ------------------------------ */
/* -------------------------------------------------------------------------- */

/*********************************************************************
* @purpose  Callback for OSPF Area changes.
*
* @param    ospfId      OSPF object handle
* @param    area_Id     area stack level handle
* @param    p_areaUserId  output location to store area user ID
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void ospf_AreaCB(t_Handle ospfId, t_Handle area_Id,
                        t_Handle *p_areaUserId)
{
  L7_uint32 index, i;
  t_S_AreaCfg cfg;
  t_ARO *p_ARO = (t_ARO *)area_Id;
  t_Handle ospfStubAreaTablePtr = L7_NULLPTR;
  t_Handle ospfAreaAggrTablePtr = L7_NULLPTR;
  t_S_StubAreaEntry stubCfg;
  t_S_AreaAggregateEntry adrRange;
  t_S_NssaCfg nssaCfg;
  L7_uint32 tosIndex;

  L7_uchar8 areaAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 areaAddr = (L7_uint32) p_ARO->AreaId;
  (void) osapiInetNtoa(areaAddr, areaAddrStr);
  OSPFMAP_TRACE("New OSPF area %s added\n", areaAddrStr);

  if(ospfMapAreaIndexGet(p_ARO->AreaId, &index) != L7_SUCCESS)
  {
    /* If we got here we did not find the specified area id
       Find a free index to store the information associated
       with the new area */
    if(ospfMapAreaCreate(p_ARO->AreaId, &index) != L7_SUCCESS)
      return;
  }

  /* Cleanup variables */
  memset(&adrRange, 0, sizeof(t_S_AreaAggregateEntry));
  memset(&cfg, 0, sizeof(t_S_AreaCfg));
  memset(&stubCfg, 0, sizeof(t_S_StubAreaEntry));
  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* We only need to apply the configuration if this area is configured as
     a stub or NSSA, since by default an area comes up as a normal area */

  switch(pOspfMapCfgData->area[index].extRoutingCapability)
  {
  case L7_OSPF_AREA_IMPORT_NO_EXT:
    /* Stub Area */
    cfg.AreaStatus = ROW_READ;
    ARO_Config(p_ARO, &cfg);

    cfg.ExternalRoutingCapability = AREA_IMPORT_NO_EXTERNAL;
    if (pOspfMapCfgData->area[index].summary == L7_OSPF_AREA_SEND_SUMMARY)
    {
      cfg.AreaSummary = AREA_SEND_SUMMARY;
    }
    else
    {
      cfg.AreaSummary = AREA_NO_SUMMARY;
    }
    cfg.AreaStatus = ROW_CHANGE;
    ARO_Config(p_ARO, &cfg);

    tosIndex = tosToTosIndex[L7_TOS_NORMAL_SERVICE];

    stubCfg.AreaId        = p_ARO->AreaId;
    stubCfg.StubMetric    = pOspfMapCfgData->area[index].stubCfg[tosIndex].stubMetric;
    stubCfg.StubTOS       = L7_TOS_NORMAL_SERVICE;
    stubCfg.StubMetricType = pOspfMapCfgData->area[index].stubCfg[tosIndex].stubMetricType;

    ospfStubAreaTablePtr = ospfStubAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    p_ARO->AreaId, L7_TOS_NORMAL_SERVICE);

    stubCfg.StubStatus = (ospfStubAreaTablePtr == L7_NULLPTR)? ROW_CREATE_AND_GO: ROW_CHANGE;

    ARO_StubMetric(p_ARO, &stubCfg);
    break;

  case L7_OSPF_AREA_IMPORT_NSSA:
    /* NSSA */
    cfg.AreaStatus = ROW_READ;
    ARO_Config(p_ARO, &cfg);

    cfg.ExternalRoutingCapability = AREA_IMPORT_NSSA;
    cfg.AreaStatus = ROW_CHANGE;
    ARO_Config(p_ARO, &cfg);

    if (ospfMapExtenNSSAConfigurationGet(p_ARO->AreaId, &nssaCfg) == L7_SUCCESS)
    {
      nssaCfg.NSSAStatus = ROW_CREATE_AND_GO;
      NSSA_Config(p_ARO, &nssaCfg);
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_OSPF_MAP_COMPONENT_ID,
             "Failed to configure NSSA area");
    }
    break;
  default:
    break;
  }

  /* Apply all configured area ranges. */
  for (i = 0;  i < L7_OSPF_MAX_AREA_RANGES; i++)
  {
    if ((pOspfMapCfgData->areaRange[i].area == p_ARO->AreaId) &&
        (pOspfMapCfgData->areaRange[i].inUse == L7_TRUE))
    {

      adrRange.NetIpAdr = pOspfMapCfgData->areaRange[i].ipAddr;
      adrRange.SubnetMask = pOspfMapCfgData->areaRange[i].subnetMask;
      adrRange.AggregateEffect = (pOspfMapCfgData->areaRange[i].advertise == L7_TRUE) ? AGGREGATE_ADVERTISE : AGGREGATE_DO_NOT_ADVERTISE;
      adrRange.AreaId = pOspfMapCfgData->areaRange[i].area;
      adrRange.LsdbType = (pOspfMapCfgData->areaRange[i].lsdbType ==  L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK)?
        AGGREGATE_SUMMARY_LINK : AGGREGATE_NSSA_EXTERNAL_LINK;

      ospfAreaAggrTablePtr = ospfAreaAggrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                      adrRange.AreaId,
                                                      adrRange.LsdbType,
                                                      adrRange.NetIpAdr,
                                                      adrRange.SubnetMask);

      adrRange.AggregateStatus = (ospfAreaAggrTablePtr == L7_NULLPTR)? ROW_CREATE_AND_GO: ROW_CHANGE;

      ARO_AddressRange(p_ARO, &adrRange);

      memset(&adrRange, 0, sizeof(t_S_AreaAggregateEntry));
    }
  } /* i < L7_OSPF_MAX_AREA_RANGES */

  return;
}

/*********************************************************************
* @purpose  Callback for OSPF neighbor creation.
*
* @param    areaUserId  area object handle
* @param    objId       neighbor stack level handle
* @param    p_nbUserId  output location to store neighbor user ID
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void ospf_NeighborCB(t_Handle areaUserId, t_Handle objId,
                            t_Handle *p_nbUserId)
{
    t_NBO *nbr = (t_NBO*) objId;
    L7_uint32 nbrRouterId = (L7_uint32) nbr->RouterId;
    L7_uchar8 nbrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(nbrRouterId, nbrStr);
    OSPFMAP_TRACE("New OSPF neighbor %s\n", nbrStr);
}

/*********************************************************************
* @purpose  Callback invoked whenever a neighbor enters or leaves FULL state.
*
* @param    NBO_Id     neighbor object
*
* @returns  void
*
* @notes    If p_NBO->State == NBO_FULL, neighbor has just entered FULL state.
*           If p_NBO->State != NBO_FULL, neighbor has just left FULL state.
*
*           This function runs on the OSPF protocol thread.
*
*           The initial use of the callback is to trigger checkpoint of
*           fully adjacent neighbors. Checkpointing is only done when NSF is supported.
*
* @end
*********************************************************************/
void ospf_NeighborFull(t_Handle NBO_Id)
{
#ifdef L7_NSF_PACKAGE
  t_NBO *p_NBO = (t_NBO*) NBO_Id;
  t_IFO *p_IFO = (t_IFO*) p_NBO->IFO_Id;
  t_RTO *p_RTO = (t_RTO*) p_NBO->RTO_Id;
  o2CkptNeighbor_t neighbor;
  L7_RC_t rc;

  /* If this router is in GR, no checkpoint. Do a full ckpt
   * when we exit GR. */
  if (p_RTO->restartStatus != OSPF_GR_NOT_RESTARTING)
  {
    return;
  }

  /* If no backup, no checkpoint */
  if (!pOspfInfo->backupElected)
  {
    return;
  }

  /* Don't checkpoint anything if GR not enabled */
  if (pOspfMapCfgData->rtr.nsfSupport == OSPF_NSF_NONE)
  {
    return;
  }

  /* Don't checkpoint a neighbor that doesn't support opaque LSAs. He won't
   * process and ack our grace LSAs. Probably a misconfiguration if user has
   * enabled graceful restart, but one of our neighbors does not support
   * opaque LSAs. */
  if ((p_NBO->Options & OSPF_OPT_O_BIT) == 0)
  {
    L7_uchar8 nbrIdStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 nbrAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(p_NBO->RouterId, nbrIdStr);
    osapiInetNtoa(p_NBO->IpAdr, nbrAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_OSPF_MAP_COMPONENT_ID,
            "OSPFv2 graceful restart is enabled, but neighbor %s at %s does not "
            "support opaque LSAs.",
            nbrIdStr, nbrAddrStr);
    return;
  }

  memset(&neighbor, 0, sizeof(o2CkptNeighbor_t));
  if (p_IFO->Cfg.Type == IFO_VRTL)
  {
    neighbor.neighborType = O2_CKPT_VIRTUAL_NBR;
    neighbor.data.transitAreaId = p_IFO->TransitAreaId;
  }
  else
  {
    neighbor.neighborType = O2_CKPT_NORMAL_NBR;
    neighbor.data.intIfNum = p_IFO->Cfg.IfIndex;
  }

  neighbor.add = (p_NBO->State == NBO_FULL) ? 1 : 0;
  neighbor.routerId = p_NBO->RouterId;
  neighbor.neighborAddr = p_NBO->IpAdr;

  o2CkptNeighborAdd(&neighbor);
  rc = ckptMgrCheckpointStart(L7_OSPF_MAP_COMPONENT_ID);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Request for checkpoint message callback failed with error %d", rc);
  }
#endif
}

/*********************************************************************
* @purpose  Callback invoked whenever a DR or BDR changes.
*
* @param    IFO_Id     interface object
*
* @returns  void
*
* @notes    This function runs on the OSPF protocol thread.
*
*           The initial use of the callback is to trigger checkpoint of
*           DR and BDR. Checkpointing is only done when NSF is supported.
*
* @end
*********************************************************************/
void ospf_DrElected(t_Handle IFO_Id)
{
#ifdef L7_NSF_PACKAGE
  t_IFO *p_IFO = (t_IFO*) IFO_Id;
  t_RTO *p_RTO = (t_RTO*) p_IFO->RTO_Id;
  o2CkptIntf_t intf;
  L7_RC_t rc;

  /* If this routing is in GR, no checkpoint. Do a full ckpt
   * when we exit GR. */
  if (p_RTO->restartStatus != OSPF_GR_NOT_RESTARTING)
  {
    return;
  }

  /* If no backup, no checkpoint */
  if (!pOspfInfo->backupElected)
  {
    return;
  }

  /* Don't checkpoint anything if GR not enabled */
  if (pOspfMapCfgData->rtr.nsfSupport == OSPF_NSF_NONE)
  {
    return;
  }

  memset(&intf, 0, sizeof(o2CkptIntf_t));
  intf.intIfNum = p_IFO->Cfg.IfIndex;
  intf.drId = p_IFO->DrId;
  intf.drIpAddr = p_IFO->Cfg.DrIpAddr;
  intf.bdrId = p_IFO->BackupId;
  intf.bdrIpAddr = p_IFO->Cfg.BackupIpAddr;

  o2CkptInterfaceAdd(&intf);
  rc = ckptMgrCheckpointStart(L7_OSPF_MAP_COMPONENT_ID);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Request for checkpoint message callback failed with error %d", rc);
  }
#endif
}

/*********************************************************************
* @purpose  Callback for OSPF traps.
*
* @param    ospfId      OSPF object handle
* @param    TrapType    trap type
* @param    p_Trap      pointer to trap type information
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void ospf_TrapCB(t_Handle ospfId, e_OspfTrapType TrapType,
                        u_OspfTrap *p_Trap)
{
  L7_uint32 lvl7State = 0;

  if (ospfMapTraceModeGet() == L7_ENABLE)
  {
      RTO_TrapPrint(ospfMapCtrl_g.RTO_Id, TrapType, p_Trap);
  }

  switch(TrapType)
  {
  case VIRT_IF_STATE_CHANGE:

    switch(p_Trap->VirtIfStateChange.VirtIfState)
    {
    case IFO_DOWN:
      lvl7State = L7_OSPF_INTF_DOWN;
      break;
    case IFO_LOOP:
      lvl7State = L7_OSPF_INTF_LOOPBACK;
      break;
    case IFO_WAIT:
      lvl7State = L7_OSPF_INTF_WAITING;
      break;
    case IFO_PTPST:
      lvl7State = L7_OSPF_INTF_POINTTOPOINT;
      break;
    case IFO_DR:
      lvl7State = L7_OSPF_INTF_DESIGNATEDROUTER;
      break;
    case IFO_BACKUP:
      lvl7State = L7_OSPF_INTF_BACKUPDESIGNATEDROUTER;
      break;
    case IFO_DROTHER:
      lvl7State = L7_OSPF_INTF_OTHERDESIGNATEDROUTER;
      break;
    default:
      /* return without issuing a trap */
      return;
    }
    ospfMapTrapVirtIfStateChange(p_Trap->VirtIfStateChange.RouterId,
                                 p_Trap->VirtIfStateChange.VirtIfAreaId,
                                 p_Trap->VirtIfStateChange.VirtIfNeighbor,
                                 lvl7State);
    break;
  case NBR_STATE_CHANGE:
    ospfMapTrapNbrStateChange(p_Trap->NbrStateChange.RouterId,
                              p_Trap->NbrStateChange.NbrIpAddr,
                              p_Trap->NbrStateChange.NbrAddressLessIndex,
                              p_Trap->NbrStateChange.NbrRtrId,
                              p_Trap->NbrStateChange.NbrState);
    break;
  case VIRT_NBR_STATE_CHANGE:
    ospfMapTrapVirtNbrStateChange(p_Trap->VirtNbrStateChange.RouterId,
                                  p_Trap->VirtNbrStateChange.VirtNbrArea,
                                  p_Trap->VirtNbrStateChange.VirtNbrRtrId,
                                  p_Trap->VirtNbrStateChange.VirtNbrState);
    break;
  case IF_CONFIG_ERROR:
    ospfMapTrapIfConfigError(p_Trap->IfConfigError.RouterId,
                             p_Trap->IfConfigError.IfIpAddress,
                             p_Trap->IfConfigError.AddressLessIf,
                             p_Trap->IfConfigError.PacketSrc,
                             p_Trap->IfConfigError.ConfigErrorType,
                             p_Trap->IfConfigError.PacketType);
    break;
  case VIRT_IF_CONFIG_ERROR:
    ospfMapTrapVirtIfConfigError(p_Trap->VirtIfConfigError.RouterId,
                                 p_Trap->VirtIfConfigError.VirtIfAreaId,
                                 p_Trap->VirtIfConfigError.VirtIfNeighbor,
                                 p_Trap->VirtIfConfigError.ConfigErrorType,
                                 p_Trap->VirtIfConfigError.PacketType);

    break;
  case IF_AUTH_FAILURE:
    ospfMapTrapIfAuthFailure(p_Trap->IfConfigError.RouterId,
                             p_Trap->IfConfigError.IfIpAddress,
                             p_Trap->IfConfigError.AddressLessIf,
                             p_Trap->IfConfigError.PacketSrc,
                             p_Trap->IfConfigError.ConfigErrorType,
                             p_Trap->IfConfigError.PacketType);
    break;
  case VIRT_IF_AUTH_FAILURE:
    ospfMapTrapVirtIfAuthFailure(p_Trap->VirtIfConfigError.RouterId,
                                 p_Trap->VirtIfConfigError.VirtIfAreaId,
                                 p_Trap->VirtIfConfigError.VirtIfNeighbor,
                                 p_Trap->VirtIfConfigError.ConfigErrorType,
                                 p_Trap->VirtIfConfigError.PacketType);
    break;
  case RX_BAD_PACKET:
    ospfMapTrapIfRxBadPacket(p_Trap->IfRxBadPacket.RouterId,
                             p_Trap->IfRxBadPacket.IfIpAddress,
                             p_Trap->IfRxBadPacket.AddressLessIf,
                             p_Trap->IfRxBadPacket.PacketSrc,
                             p_Trap->IfRxBadPacket.PacketType);
    break;
  case VIRT_IF_RX_BAD_PACKET:
    ospfMapTrapVirtIfRxBadPacket(p_Trap->VirtIfRxBadPacket.RouterId,
                                 p_Trap->VirtIfRxBadPacket.VirtIfAreaId,
                                 p_Trap->VirtIfRxBadPacket.VirtIfNeighbor,
                                 p_Trap->VirtIfRxBadPacket.PacketType);
    break;
  case TX_RETRANSMIT:
    ospfMapTrapTxRetransmit(p_Trap->TxRetransmit.RouterId,
                            p_Trap->TxRetransmit.IfIpAddress,
                            p_Trap->TxRetransmit.AddressLessIf,
                            p_Trap->TxRetransmit.NbrRtrId,
                            p_Trap->TxRetransmit.PacketType,
                            p_Trap->TxRetransmit.LsdbType,
                            p_Trap->TxRetransmit.LsdbLsid,
                            p_Trap->TxRetransmit.LsdbRouterId);
    break;
  case VIRT_IF_TX_RETRANSMIT:
    ospfMapTrapVirtTxRetransmit(p_Trap->VirtTxRetransmit.RouterId,
                                p_Trap->VirtTxRetransmit.VirtIfAreaId,
                                p_Trap->VirtTxRetransmit.VirtIfNeighbor,
                                p_Trap->VirtTxRetransmit.PacketType,
                                p_Trap->VirtTxRetransmit.LsdbType,
                                p_Trap->VirtTxRetransmit.LsdbLsid,
                                p_Trap->VirtTxRetransmit.LsdbRouterId);
    break;
  case ORIGINATE_LSA:
    ospfMapTrapOriginateLsa(p_Trap->OriginateLsa.RouterId,
                            p_Trap->OriginateLsa.LsdbAreaId,
                            p_Trap->OriginateLsa.LsdbType,
                            p_Trap->OriginateLsa.LsdbLsid,
                            p_Trap->OriginateLsa.LsdbRouterId);
    break;
  case MAX_AGE_LSA:
    ospfMapTrapMaxAgeLsa(p_Trap->MaxAgeLsa.RouterId,
                         p_Trap->MaxAgeLsa.LsdbAreaId,
                         p_Trap->MaxAgeLsa.LsdbType,
                         p_Trap->MaxAgeLsa.LsdbLsid,
                         p_Trap->MaxAgeLsa.LsdbRouterId);
    break;
  case LS_DB_OVERFLOW:
    ospfMapTrapLsdbOverflow(p_Trap->LsdbOverflow.RouterId,
                            p_Trap->LsdbOverflow.ExtLsdbLimit);
    break;
  case LS_DB_APPROACHING_OVERFLOW:
    ospfMapTrapLsdbApproachingOverflow(p_Trap->LsdbApproachingOverflow.RouterId,
                                       p_Trap->LsdbApproachingOverflow.ExtLsdbLimit);
    break;
  case IF_STATE_CHANGE:
    /* Switch to lvl7 if state */
    switch(p_Trap->IfStateChange.IfState)
    {
    case IFO_DOWN:
      lvl7State = L7_OSPF_INTF_DOWN;
      break;
    case IFO_LOOP:
      lvl7State = L7_OSPF_INTF_LOOPBACK;
      break;
    case IFO_WAIT:
      lvl7State = L7_OSPF_INTF_WAITING;
      break;
    case IFO_PTPST:
      lvl7State = L7_OSPF_INTF_POINTTOPOINT;
      break;
    case IFO_DR:
      lvl7State = L7_OSPF_INTF_DESIGNATEDROUTER;
      break;
    case IFO_BACKUP:
      lvl7State = L7_OSPF_INTF_BACKUPDESIGNATEDROUTER;
      break;
    case IFO_DROTHER:
      lvl7State = L7_OSPF_INTF_OTHERDESIGNATEDROUTER;
      break;
    default:
      /* return without issuing a trap */
      return;
    }
    ospfMapTrapIfStateChange(p_Trap->IfStateChange.RouterId,
                             p_Trap->IfStateChange.IfIpAddress,
                             p_Trap->IfStateChange.AddressLessIf,
                             lvl7State);
    break;
  case IF_RX_PACKET:
    /* This trap does not need to be supported as per
       rfc 1850.
       ospfMapTrapIfRxPacket();
    */

    break;
  case RTB_ENTRY_INFO:
    /* This trap does not need to be supported as per
       rfc 1850.
       ospfMapTrapRtbEntryInfo();
    */
    break;
  default:
    break;
  }

  return;
}

/*********************************************************************
* @purpose  Callback for OSPF Route Table changes.
*
* @param    RtoMngId    OSPF station handle
* @param    DestType    destination type
* @param    ChangeType  nature of the change
* @param    EntryAdr    destination address of the entry that changed
* @param    IpMask      network/subnetwork address mask
* @param    PathNum     length of router chain to reach destination
* @param    PathPrm     next hop chain for this router
* @param    isRejectRoute if the route is a reject route
*
* @returns  E_OK
*           E_NO_ROOM if RTO is full
*
* @notes    none
*
* @end
*********************************************************************/
static e_Err ospf_RTChanged(t_Handle RtoMngId, e_OspfDestType DestType,
                           e_RtChange ChangeType, SP_IPADR EntryAdr, SP_IPADR IpMask,
                           word PathNum, ulng PathCost,
                           t_OspfPathPrm *PathPrm, ulng PathType, Bool isRejectRoute)
{
  t_RTO *p_RTO = (t_RTO *)ospfMapCtrl_g.RTO_Id;
  char  * dest_type, *change_type;
  L7_BOOL isRouter;
  L7_routeEntry_t routeEntry;
  L7_uint32 path_type, pref_type;
  t_IFO *p_IFO = L7_NULLPTR;
  t_OspfPathPrm *curr = PathPrm;
  L7_uint32 i, j, k;
  L7_uchar8 destAddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 dip = (L7_uint32)EntryAdr;
  L7_uint32 mask = (L7_uint32)IpMask;
  L7_uint32 intIfNum;
  static const char *routine_name = "ospf_RTChanged()";

  osapiInetNtoa(dip, destAddr);
  osapiInetNtoa(mask, maskStr);
  OSPFMAP_TRACE("%s %d: %s : ip addr = %s, mask = %s\n",
                __FILE__, __LINE__, routine_name, destAddr, maskStr);

  /* if in graceful restart, don't update RTO */
  if (o2GracefulRestartInProgress(p_RTO))
  {
    return E_OK;
  }

  bzero((L7_char8 *)&routeEntry, sizeof(L7_routeEntry_t));

  /* Filter out host routes to a local IP address. OSPF can report such routes
   * when the calculating router has a p2p link to one or more neighbors. */
  if ((IpMask == SPF_HOST_ROUTE) &&
      (ipMapIpAddressToIntf(EntryAdr, &intIfNum) == L7_SUCCESS))
  {
    return E_OK;
  }

  /* Suppress addition of OSPF route to local subnet */
  if (ChangeType != RTE_REMOVED)
  {
      if((PathPrm != L7_NULLPTR) && (PathPrm->NextHop != L7_NULLPTR))
      {
        if(EntryAdr != L7_NULL_IP_ADDR)
        {
          if(rtoPrefixFind((L7_uint32) EntryAdr, (L7_uint32) IpMask,
                           &routeEntry) == L7_SUCCESS)
          {
            if(routeEntry.protocol == RTO_LOCAL)
            {
              p_IFO = (t_IFO *)(PathPrm->NextHop);
              if (p_IFO->Cfg.Type == IFO_VRTL)
              {
                intIfNum = p_IFO->VirtIfIndex;
              }
              else
              {
                intIfNum = p_IFO->Cfg.IfIndex;
              }
              /* RTO has a local route to this destination */
              if (ChangeType == RTE_ADDED)
              {
                /* OSPF did not previously report this route. */
                /* If the next hop of the OSPF route is the local interface
                 * to the destination network, don't add the route to RTO.
                 * This prevents the OSPF route from being a temporary black
                 * hole if the interface goes down. */
                if (intIfNum == routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum)
                {
                  return E_OK;
                }
              }
              else if (ChangeType == RTE_MODIFIED)
              {
                /* OSPF previously reported this route. If current next hop
                 * is the local interface to the dest network, remove the
                 * route from RTO (in case it was previously added to RTO.
                 * Otherwise, do a modify. In either case, RTO may not have
                 * the route and report a bad delete, but it's difficult to
                 * ask RTO if it has any OSPF route to this destination. */
                if (intIfNum == routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum)
                {
                  ChangeType = RTE_REMOVED;
                }
              }
            }
          }
          else if(ChangeType == RTE_MODIFIED)
          {
             /* It comes here if there is no route to this destination in the RTO.
              * This could be the case when the earlier OSPF route was local network
              * and hence not added to RTO. When an indirect route to the local network
              * is active, it comes as RTE_MODIFIED change from OSPF. Since it is not
              * present in the RTO, we do ADD in the RTO */
             if (PathPrm->IpAdr != 0)
                ChangeType = RTE_ADDED;
          }
        }
      }
  }

  isRouter = L7_FALSE;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  /* check which bit is set in the following order of precedence
   * NOTE:  sometimes multiple bits are set (e.g., both border router and router)
   */
  if (DestType & DEST_TYPE_IS_STUB_NETWORK)
  {
    dest_type = "STUB_NETWORK";
  } else if (DestType & DEST_TYPE_IS_NETWORK)
  {
    dest_type = "NETWORK";
  } else if (DestType & DEST_TYPE_IS_AS_BOUNDARY_ROUTER)
  {
    dest_type = "BOUNDARY_ROUTER";
    isRouter = L7_TRUE;
  } else if (DestType & DEST_TYPE_IS_BORDER_ROUTER)
  {
    dest_type = "BORDER_ROUTER";
    isRouter = L7_TRUE;
  } else if (DestType & DEST_TYPE_IS_ROUTER)
  {
    dest_type = "ROUTER";
    isRouter = L7_TRUE;
  } else
  {
    dest_type = "Unknown";
  }

  switch (ChangeType)
  {
  case RTE_ADDED:
    change_type = "RTE_ADDED";
    break;
  case RTE_REMOVED:
    change_type = "RTE_REMOVED";
    break;
  case RTE_MODIFIED:
    change_type = "RTE_MODIFIED";
    break;
  case RTE_NOT_MODIFIED:
    change_type = "RTE_NOT_MODIFIED";
    break;
  default:
    change_type = "Unknown";
    break;
  }

  switch (PathType)
  {
  case OSPF_INTRA_AREA:
    path_type = RTO_OSPF_INTRA_AREA;
    pref_type = ROUTE_PREF_OSPF_INTRA_AREA;
    break;

  case OSPF_INTER_AREA:
    path_type = RTO_OSPF_INTER_AREA;
    pref_type = ROUTE_PREF_OSPF_INTER_AREA;
    break;

  case OSPF_TYPE_1_EXT:
    path_type = RTO_OSPF_TYPE1_EXT;
    pref_type = ROUTE_PREF_OSPF_EXTERNAL;
    break;

  case OSPF_TYPE_2_EXT:
    path_type = RTO_OSPF_TYPE2_EXT;
    pref_type = ROUTE_PREF_OSPF_EXTERNAL;
    break;

  case OSPF_NSSA_TYPE_1_EXT:
    path_type = RTO_OSPF_NSSA_TYPE1_EXT;
    pref_type = ROUTE_PREF_OSPF_EXTERNAL;
    break;

  case OSPF_NSSA_TYPE_2_EXT:
    path_type = RTO_OSPF_NSSA_TYPE2_EXT;
    pref_type = ROUTE_PREF_OSPF_EXTERNAL;
    break;

  default:
    path_type = RTO_OSPF_INTRA_AREA;
    pref_type = ROUTE_PREF_OSPF_INTRA_AREA;
    break;
  }

  OSPFMAP_TRACE("OSPF Route Change: Dest=%s, Dest Type=%s, Action=%s\n",
                destAddr, dest_type, change_type);

  routeEntry.ipAddr = EntryAdr;
  routeEntry.subnetMask = IpMask;
  routeEntry.protocol = path_type;
  routeEntry.pref = ipMapRouterPreferenceGet(pref_type);
  routeEntry.metric = PathCost;
  if(isRejectRoute == TRUE)
     routeEntry.flags |= L7_RTF_REJECT;

  /* Only do this for adds and modifies. A route may get deleted because
   * it's outgoing interface has been deleted. In this case, the IFO
   * has already been freed and we don't want to dereference it.
   * rto6RouteDelete() doesn't use the next hop information to find
   * its copy of the route.
   * Since p_IFO->NextHopToVrtNbr can be the same as one of the
   * nexthops in the path, be sure that if there are any
   * duplicates, they do not get added to the nexthops
   */
  i = 0;
  k = 0;
  if (ChangeType != RTE_REMOVED)
  {
    while ((k < PathNum) && (curr != NULL) && (curr->NextHop != NULL))
    {
      p_IFO = (t_IFO *)(curr->NextHop);
      if (p_IFO->Cfg.Type == IFO_VRTL)
      {
        routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr = p_IFO->NextHopToVrtNbr;
        routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.intIfNum = p_IFO->VirtIfIndex;
      }
      else
      {
        routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr = curr->IpAdr;
        routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.intIfNum = p_IFO->Cfg.IfIndex;
      }

      for (j = 0; j < i; j++)
      {
        if ((routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr ==
             routeEntry.ecmpRoutes.equalCostPath[j].arpEntry.ipAddr) &&
            (routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.intIfNum ==
             routeEntry.ecmpRoutes.equalCostPath[j].arpEntry.intIfNum))
        {
          routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr = 0;
          routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.intIfNum = 0;
          i--;
          break;
        }
      }
      curr = curr->next;
      i++;
      k++;
    }
  }

  routeEntry.ecmpRoutes.numOfRoutes = i;

  /* do not add router id entries to the routing table */
  if (isRouter == L7_FALSE)
  {
    switch(ChangeType)
    {
    case RTE_ADDED:
      if (rtoRouteAdd(&routeEntry) == L7_TABLE_IS_FULL)
        return E_NO_ROOM;
      break;

    case RTE_REMOVED:
      rtoRouteDelete(&routeEntry);
      break;

    case RTE_MODIFIED:
      rtoRouteModify(&routeEntry);
      break;

    default:
        OSPFMAP_TRACE("Unknown Route Table Entry Change Type (%d)\n",
                      (L7_uint32)ChangeType);
      break;
    }
  }
  return E_OK;
}

/*********************************************************************
* @purpose  Take actions required on first RTO update after GR completes.
*
* @param    RtoMngId    OSPF station handle
* @param    intIfNum    lower layer object id
* @param    operStatus  interface state:
*                       TRUE: Interface is active
*                       FALSE: Interface is inactive
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static e_Err ospf_RtoUpdateDone(t_Handle RtoMngId)
{
  ipMapInitialRoutesDone(NSF_PROTOCOL_OSPF);

  OSAPI_TRACE_EVENT(L7_TRACE_EVENT_OSPF2_RESTART_DONE,
                    (L7_uchar8 *)L7_NULLPTR, 0);

  return E_OK;
}


/*********************************************************************
* @purpose  This callback returns the physical status for low layer
*           object specified by ifIndex.
*
* @param    RtoMngId    OSPF station handle
* @param    intIfNum    lower layer object id
* @param    operStatus  interface state:
*                       TRUE: Interface is active
*                       FALSE: Interface is inactive
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void ospf_GetLLOperStatusCB(t_Handle RtoMngId, t_LIH intIfNum,
                                   Bool *operStatus)
{
  L7_uint32 activeState;

  *operStatus = FALSE;

  if (((L7_uint32)intIfNum < 1 ) || (intIfNum > L7_MAX_INTERFACE_COUNT))
    return;

  if((nimGetIntfActiveState((L7_uint32)intIfNum, &activeState) == L7_SUCCESS) &&
     (activeState == L7_ACTIVE))
  {
    *operStatus = TRUE;
  }
}

/*********************************************************************
* @purpose  This callback returns the low layer object handle
*           for the interface specified by ifIndex.
*
* @param    ulng        ifIndex
*
* @returns  t_Handle
*
* @notes    none
*
* @end
*********************************************************************/
static t_Handle ospf_IfLowLayerIdMCB(ulng ifIndex)
{
  return localMgrPtr[ifIndex];
}

/*********************************************************************
* @purpose  This callback returns the low layer parameters of the
*           interface: speed (bandwidth in bits per second), type
*           (Ethernet (1), frame relay (1) or ATM (2), maximal MTU
*           size.
*
* @param    ulng                    ifIndex
* @param    t_ospfMibIfLowParams    ifLowParams
*
* @returns  E_OK      success
* @returns  not E_OK  failure
*
* @notes    none
*
* @end
*********************************************************************/
static e_Err ospf_IfParamsMCB(ulng intIfNum, t_ospfMibIfLowParams *ifLowParams)
{
  L7_uint32 ifSpeed, mtu, ifType;

  if (nimGetIntfSpeedDataRate(intIfNum, &ifSpeed) == L7_SUCCESS)
  {
    if (nimGetIntfMaxFrameSize (intIfNum, &mtu) == L7_SUCCESS)
    {
      if (nimGetIfPhyType(intIfNum, &ifType) == L7_SUCCESS)
      {
        switch (ifType)
        {
        case L7_ETHERNET:
          ifLowParams->ifPhyType = OSPF_ETHERNET;
          break;
        case L7_FRAME_RELAY:
          ifLowParams->ifPhyType = OSPF_FRAME_RELAY;
          break;
        case L7_ATM:
          ifLowParams->ifPhyType = OSPF_ATM;
          break;
        default:
          return E_FAILED;
          /*passthru*/
        }

        ifLowParams->ifSpeed = ifSpeed;
        ifLowParams->ifMTUsize = mtu;

        return E_OK;
      }
    }
  }

  return E_FAILED;
}

/*********************************************************************
* @purpose  This callback returns the interface mask.
*
* @param    ulng    ifIndex
* @param    ulng    ifIpAddress
*
* @returns  ulng    ifMask
*
* @notes    none
*
* @end
*********************************************************************/
static ulng ospf_IfMaskMCB(ulng intIfNum, ulng ifIpAddress)
{
  L7_uint32 ipAddr, ipMask;

  if (ipMapRtrIntfCfgIpAddressGet(intIfNum, &ipAddr, &ipMask) == L7_SUCCESS &&
      ipAddr != L7_NULL_IP_ADDR)
  {
    if (ifIpAddress == ipAddr)
    {
      return ipMask;
    }
  }

  return L7_NULL_IP_MASK;
}

/*********************************************************************
* @purpose  This callback returns ifIndex of an interface connecting
*           to the specified host.
*
* @param    ulng        HostIpAddress
* @param    t_LIH       *ifIndex
*
* @returns  E_OK      success
* @returns  not E_OK  failure
*
* @notes    none
*
* @end
*********************************************************************/
static e_Err ospf_HostRouteMCB(ulng HostIpAddress, t_LIH *intIfNum)
{
  L7_routeEntry_t bestRoute;

  if (rtoBestRouteLookup(HostIpAddress, &bestRoute, L7_FALSE) == L7_SUCCESS)
  {
      /* Just use first next hop. Assuming the host is on a local subnet,
       * the best route should be the local route, with only one next hop. */
      *intIfNum = bestRoute.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
  }

  return E_FAILED;
}

/*********************************************************************
* @purpose  This callback procedure is used for indication of IFO
*           object creation or deletion of an IFO object.
*
* @param    t_Handle      rtrMng
* @param    t_Handle      rtrObj
* @param    ulng          ifIpAddr
* @param    ulng          ifIndex
* @param    Bool          ifoCreatDel
* @param    t_Handle      IfObj
*
* @returns  E_OK      success
* @returns  not E_OK  failure
*
* @notes    if ifoCreatDel is TRUE the interface has been activated
*           and IFO object was created successfully,
*           if ifCreatDel is FALSE, the IFO object has been deleted
*
* @end
*********************************************************************/
static e_Err ospf_ifoCreateDelMCB(t_Handle rtrMng, t_Handle rtrObj,
                             ulng ifIpAddr, ulng ifIndex,
                             Bool ifoCreatDel, t_Handle IfObj)
{
  OSPFMAP_TRACE("f_ifoCreateDelInd\n");
  return E_OK;
}


/* -------------------------------------------------------------------------- */
/* ---------------------------- Socket Support ------------------------------ */
/* -------------------------------------------------------------------------- */

void ospfTxRxTraceSet(L7_uint32 val)
{
  ospfTxRxTrace = val;
}

static e_Err sockTransmit(t_Handle Id, void *p_Frame, SP_IPADR DstAdr, t_LIH tmp)
{
  t_lm  *p_lm = (t_lm *)Id;
  word  length;
  word  sentLen;
  e_Err e = E_OK;

  if (p_Frame == NULLP)
  {
    ASSERT(FALSE);
    return E_FAILED;
  }

  if (p_lm == NULLP)
  {
    F_Delete(p_Frame);
    return E_FAILED;
  }

  if(ospfMapCtrl_g.ospfConn == NULLP)
  {
    F_Delete(p_Frame);
    return E_FAILED;
  }

  /* point socket in the right direction for multicast output */
  if ((DstAdr & 0xE0000000) == 0xE0000000)
  {
    /* ulng tgt_mcast = osapiHtonl(p_lm->ipAddr); */
    /* Because we support unnumbered interfaces, we cannot identify the
     * interface by IP address. Must use interface index. */
    struct ip_mreqn mreq;
    L7_uchar8 ifName[IFNAMSIZ];
    L7_uint32 ifIndex;

    if (osapiIfNameStringGet(p_lm->intIfNum, ifName, IFNAMSIZ) != L7_SUCCESS)
    {
      F_Delete(p_Frame);
      return E_FAILED;
    }
    if (ipstkStackIfIndexGet(ifName, &ifIndex) != L7_SUCCESS)
    {
      F_Delete(p_Frame);
      return E_FAILED;
    }
    mreq.imr_multiaddr.s_addr = 0;
#ifdef _L7_OS_LINUX_
    mreq.imr_address.s_addr = 0;
#endif
#ifdef _L7_OS_VXWORKS_
    mreq.imr_interface.s_addr = 0;
#endif
mreq.imr_ifindex = (int) ifIndex;

    if(l7SocketSetOpt(ospfMapCtrl_g.ospfConn, IPPROTO_IP, IP_MULTICAST_IF,
                      (char*)&mreq, sizeof(mreq)) != E_OK)
    {
      F_Delete(p_Frame);
      return E_FAILED;
    }
  }

  length = F_GetLength(p_Frame);

  if(ospfTxRxTrace & 0x01)
  {
  sprintf(print_str,"Sent packet length %d", length);
    XX_Print(print_str);

    {
    word i;
    byte data;
    char *p=print_str;
    for (i=0;i<length;i++)
    {
      F_Peek( &data, p_Frame, i);
      sprintf(p,"%2.2X ",data);
      p = p + strlen(p);
    }
    XX_Print(print_str);
  }
  }

  /* send packet */
  sentLen = TcpUdpBind_Send(ospfMapCtrl_g.ospfConn, p_Frame, DstAdr, tmp, 0);
  if (length != sentLen)
    e = E_FAILED;

  return e;
}

/*********************************************************************
* @purpose  This callback returns the Queue Id to be used for a packet
*
* @param    userId
* @param    p_Frame
*
* @returns  Queue Id
*
* @notes    Hello and Ack packets are received on the priority queue.
*
* @end
*********************************************************************/
static ulng receiveQueue(t_Handle userId, void *p_Frame)
{
  byte       *pRecvBuf;
  t_S_PckHeader *p_Hdr;
  ulng recvQueue = OSPF_DATA_QUEUE;
  ulng pktType = S_ILLEGAL;
  ulng dstAddr = 0;
  L7_ipHeader_t *ipHdr;

  pRecvBuf = B_GetData(F_GetFirstBuf(p_Frame));
  ipHdr = (L7_ipHeader_t*) pRecvBuf;
  p_Hdr = (t_S_PckHeader *)(pRecvBuf + sizeof(L7_ipHeader_t));

  /* determine which queue to use */
  switch(p_Hdr->Type)
  {
  case S_HELLO:
    recvQueue = OSPF_HELLO_QUEUE;
    break;
  case S_LS_ACK:
    recvQueue = OSPF_ACK_QUEUE;
    break;
  default:   /* all other packets */
    recvQueue = OSPF_DATA_QUEUE;
  }

  /* collect RX stats for debug */
  if(p_Hdr->Type > S_ILLEGAL && p_Hdr->Type <= S_IGMP_REPORT)
    pktType = p_Hdr->Type;


  dstAddr = osapiNtohl(ipHdr->iph_dst);
  if((dstAddr & 0xE0000000) != 0xE0000000)
    pktType += OSPF_RX_UNICAST;

  ospfRxStats[pktType] += 1;

  return recvQueue;
}

/*********************************************************************
* @purpose  This callback returns the Queue Id to be used for a packet
*
* @param    userId
* @param    p_Frame
*
* @returns  Queue Id
*
* @notes    Hello and Ack packets are received on the priority queue.
*
* @end
*********************************************************************/
static e_Err sockReceive(t_Handle userId,    /* User Id of connection */
                         void *p_Frame,      /* Data frame */
                         ulng remoteAddress, /* Remote IP address for UDP connection */
                         word remotePort,    /* Remote logical port for UDP connection */
                         ulng intIfNum)      /* receive interface */
{
  t_lm  *p_lm = 0;
  t_IFO *p_IFO = 0;
  int        rlen;
  SP_IPADR   SrcAdr, DstAdr;
  byte *pRecvBuf;
  L7_ipHeader_t *ipHdr;

  if (ospfMapCtrl_g.ospfConn == NULLP)
  {
    ospfSockRxErrNoConn++;
    F_Delete(p_Frame);
    return E_FAILED;
  }

  rlen = F_GetLength(p_Frame);
  if (rlen <= sizeof(L7_ipHeader_t))
  {
    ospfSockRxErrTooShort++;
    F_Delete(p_Frame);
    return E_FAILED;
  }

  ipHdr = (L7_ipHeader_t*) B_GetData(F_GetFirstBuf(p_Frame));

  SrcAdr = osapiNtohl(ipHdr->iph_src);
  DstAdr = osapiNtohl(ipHdr->iph_dst);

  /* Get receive interface object based on rx internal interface num */
  if (ospfMapLmLookup(intIfNum, &p_lm) != L7_SUCCESS)
  {
    ospfSockRxErrNoIfo++;
    F_Delete(p_Frame);
    return E_FAILED;
  }

  p_IFO = (t_IFO *)p_lm->IFO_Id;

  /* drop loopback multicast frames because
   * IP_MULTICAST_LOOP doesn't work on vxworks
   */
  if (SrcAdr == p_lm->ipAddr || (p_IFO == L7_NULLPTR))
  {
    F_Delete(p_Frame);
    return E_OK;
  }

  if (ospfTxRxTrace & 0x02)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    sprintf(print_str, "Received packet length %d on interface %s",
            rlen, ifName);
    XX_Print(print_str);
    pRecvBuf = (byte*) ipHdr;
    {
    int i;
    char *p=print_str;
    for (i=0;i<rlen;i++)
    {
      sprintf(p,"%2.2X ",pRecvBuf[i]);
      p = p + strlen(p);
    }
    XX_Print(print_str);
  }
  }

  if ( sizeof(L7_ipHeader_t) != F_CutFromStart(p_Frame, sizeof(L7_ipHeader_t)))
  {
    if(ospfTxRxTrace & 0x02)
    {
    sprintf(print_str,"Received error");
    XX_Print(print_str);
    }
    ospfSockRxErrTooShort++;
    F_Delete(p_Frame);
    return E_FAILED;
  }

  /* Protect interfaces from receiving packets not destined to them,
   * so that unnecessary traplog entries can be avoided
   */
  if ((DstAdr != AllDRouters) && (DstAdr != AllSpfRouters) &&
      (DstAdr != p_IFO->Cfg.IpAdr))
  {
    ospfSockRxErrBadDest++;
    F_Delete(p_Frame);
    return E_FAILED;
  }

  IFO_Received(p_lm->IFO_Id, p_Frame, SrcAdr, DstAdr);
  return E_OK;
}


static void sockStatus( t_Handle userId,     /* User Id of connection */
                        t_Handle connId,     /* Connection Idreturned by TcpUdpBind_Open */
                        word status)         /* Connection status flags */
{
  OSPFMAP_TRACE("Socket 0x%8.8X status is %ld\n", (unsigned int)connId, (ulng)status);
}


/*********************************************************************
* @purpose  Creates the OSPF socket
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    A single socket is used for OSPF tx rx. This is to avoid
*           loopback and cross socket delivery. It does mean that we
*           have to match a local manager (lm) to every incoming packet
*           and run IP_MULTICAST_IF for every outgoing packet.
*
*           An OSPF sysnet hook is used to ensure that the source address
*           of incoming multicast packets matches the interface subnet.
*
* @end
*********************************************************************/
L7_RC_t ospfMapRawSockInit(void)
{
  t_TcpUdpParam   param;
  e_Err           erc;
  L7_uint32       ipTosVal;
  int option;


  static const char *routine_name = "rawSockInit()";

  /* already initialized? */
  if(ospfMapCtrl_g.ospfTcpUdpObject != NULLP)
    return L7_SUCCESS;

  if ((erc = TcpUdpBind_Init(&ospfMapCtrl_g.ospfTcpUdpObject, NULLP,
                             "OSPF Receive")) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to create OSPF receive socket. Error code %u", erc);
    return L7_FAILURE;
  }

  memset(&param, 0, sizeof(t_TcpUdpParam));
  param.localAddress   = INADDR_ANY;
  param.netMask        = 0;
  param.connType       = ipConnType;
  param.protocol       = OSPF_IP_PROTOCOL;
  param.dontRoute      = FALSE;
  param.f_Rx           = sockReceive;
  param.f_RxQueue      = receiveQueue;
  param.f_Stat         = sockStatus;
  param.client         = TRUE;
  param.threadId       = ospfMapCtrl_g.ospfThread;

  /* Need pretty large receive buffer for OSPF. A single socket
   * serves all neighbors on all interfaces. With a single neighbor, a full
   * set of ACKs for 8000 routes would require 120 LS ACK messages, each
   * 1500 bytes long. That's 180,000 bytes. 8000 incoming LSAs would
   * take even more space. On the other hand, we don't want OSPFv2 to
   * be able to queue all the IP stack's receive buffers on its queue.  */
  param.rxBufSize = 100000;
  param.txBufSize = 100000;

  /* Create the RAW Socket for OSPF Protocol */
  if ((erc = TcpUdpBind_Open(ospfMapCtrl_g.ospfTcpUdpObject, 0, &param,
                             &ospfMapCtrl_g.ospfConn)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to open OSPF receive socket. Error code %u", erc);
    return L7_FAILURE;
  }

  /* Set the local address valid since we already have a valid address & net mask */
  TcpUdpBind_SetLocalAddressValid(ospfMapCtrl_g.ospfConn);

  /* make sure that the TOS bits are set correctly for packets sent to this socket */
  ipTosVal = IPTOS_PREC_INTERNETCONTROL;

  if (l7SocketSetOpt(ospfMapCtrl_g.ospfConn, IPPROTO_IP, IP_TOS,
                 (char *)&ipTosVal, sizeof (ipTosVal)) != E_OK)
  {
    OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Raw socket l7SocketSetOpt error\n",
                    __FILE__, __LINE__, routine_name);
    return L7_FAILURE;
  }

  option = 1;
  erc = l7SocketSetOpt(ospfMapCtrl_g.ospfConn, IPPROTO_IP, L7_IP_PKTINFO,
                     (L7_uchar8 *) &option, sizeof (option));
  if (erc != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
           "Failed to set packet info option on OSPF socket. Error %d.",
            erc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Deletes the OSPF socket
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapRawSockDelete(void)
{
  e_Err           erc;

  static const char *routine_name = "rawSockDelete()";


  /* Close the RAW Socket for OSPF Protocol on this interface */
  if (ospfMapCtrl_g.ospfConn != NULLP)
  {
    if ((erc = TcpUdpBind_Close(ospfMapCtrl_g.ospfConn)) != E_OK)
    {
      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Raw socket Close error, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc);
      return L7_FAILURE;
    }
    ospfMapCtrl_g.ospfConn = NULLP;
  }

  if (ospfMapCtrl_g.ospfTcpUdpObject != NULLP)
  {
    if ((erc = TcpUdpBind_Destroy(&ospfMapCtrl_g.ospfTcpUdpObject)) != E_OK)
    {
      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Raw socket Destroy error, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc);
      return L7_FAILURE;
    }
    ospfMapCtrl_g.ospfTcpUdpObject = NULLP;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Initializes OSPF control context structure
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void ospfMapCtrlCtxInit(void)
{
  ospfMapCtrl_g.RTO_Id = NULLP;
  ospfMapCtrl_g.ospfThread = NULLP;
  ospfMapCtrl_g.ospfSyncSema = NULLP;
  ospfMapCtrl_g.ospfTcpUdpObject = NULLP;
  ospfMapCtrl_g.ospfConn = NULLP;
  ospfMapMibCtrl_g.p_ospfMibObj = NULLP;
}


/*********************************************************************
* @purpose  Sets up a Local Manager array element in preparation for use.
*
* @param    lm          pointer to local manager element
* @param    index       local manager pointer array index number
* @param    intIfNum    internal interface number assigned to this local mgr
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void ospfMapLocalMgrSetup(ospfMapLocalMgr_t *lm, L7_uint32 index,
                                 L7_uint32 intIfNum)
{
  lm->index = index;
  lm->intIfNum = intIfNum;
  lm->IFO_Id = NULLP;

  if (ipMapIntfIsUnnumbered(intIfNum))
  {
    lm->ipAddr = 0;
    lm->ipMask = 0;
  }
  else
  {
    if (ipMapRtrIntfIpAddressGet(intIfNum, &lm->ipAddr, &lm->ipMask) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Interface address error, intf=%d, %s\n",
                    __FILE__, __LINE__, __FUNCTION__, intIfNum, ifName);
    }
  }
}


/*********************************************************************
* @purpose  Initializes a generic MD5 authentication key element with
*           full lifetime
*
* @param    *pAuth      @b{(input)} pointer to auth key config element
*
* @returns  void
*
* @notes    The caller is expected to fill in a valid key id, key, etc.
*           after this function returns.
*
* @end
*********************************************************************/
static void ospfMapAuthKeyElemInit(t_OspfAuthKey *pAuth)
{
  memset(pAuth, 0, (size_t)sizeof(t_OspfAuthKey));
  pAuth->KeyStartAccept = 0;
  pAuth->KeyStopAccept = (ulng)-1;
  pAuth->KeyStartGenerate = 0;
  pAuth->KeyStopGenerate = (ulng)-1;
}

void ospfDebugRxStatsClear(L7_uint32 val)
{
  memset(ospfRxStats, 0, sizeof(ospfRxStats));
  ospfSockRxErrBadDest = 0;
  ospfSockRxErrTooShort = 0;
  ospfSockRxErrNoIfo = 0;
  ospfSockRxErrNoConn = 0;
  if(!val)
    sysapiPrintf("\r\n OSPF stats cleared");
}

void ospfDebugRxStatsShow()
{
  L7_uint32 i = 0;

  sysapiPrintf("\n\nSeconds since boot:  %u", osapiUpTimeRaw());

  sysapiPrintf("\r\nInvalid destination IP address: %u", ospfSockRxErrBadDest);
  sysapiPrintf("\r\nPacket too short: %u", ospfSockRxErrTooShort);
  sysapiPrintf("\r\nUnable to determine receive interface: %u", ospfSockRxErrNoIfo);
  sysapiPrintf("\r\nNo connection info: %u", ospfSockRxErrNoConn);

  sysapiPrintf("\n\r\nOSPF stats   mcast      unicast ");

  while(i <= S_LS_ACK)
  {
    sysapiPrintf("\r\n%-13s %-10d %-10d",
      ospfPckTypeNames[i],
      ospfRxStats[i],
      ospfRxStats[i+OSPF_RX_UNICAST]);
    i++;
  }
}
