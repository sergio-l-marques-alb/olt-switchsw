/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  dhcps_map.c
* @purpose   DHCP Server Mapping system infrastructure
* @component DHCP Server Mapping Layer
* @comments  none
* @create    09/12/2003
* @author    athakur
* @end
*
**********************************************************************/

#include "l7_dhcpsinclude.h"
#ifdef L7_NSF_PACKAGE
#include "unitmgr_api.h"
#endif /* L7_NSF_PACKAGE */

/* external function prototypes */
extern void     dhcps_Task(void);
extern void     dhcpsPingTask(void);
extern void *   dhcpsProcQueue;             /* DHCPS Processing queue */

/* DHCP Server mapping component globals */
dhcpsMapCtrl_t            dhcpsMapCtrl_g;
L7_dhcpsMapCfg_t          *pDhcpsMapCfgData;
L7_dhcpsMapLeaseCfg_t     *pDhcpsMapLeaseCfgData;
dhcpsInfo_t               *pDhcpsInfo;
dhcpsAsyncMsg_t           *pDhcpsAsyncInfo;
void *dhcpsMap_Queue = L7_NULL;

/*********************************************************************
* @purpose  Initialize the DHCP Server layer application
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dhcpsMapAppsInit(void)
{

    /*--------------------------*/
    /* initialize the DHCP Server stack */
    /*--------------------------*/

    /* DHCPS_MAP_QUEUE */
    dhcpsMap_Queue = (void *)osapiMsgQueueCreate(DHCPSMAP_QUEUE,
                                                 DHCPSMAP_QUEUE_MAX_MSG_COUNT,
                                                 sizeof(dhcpsMapMsg_t) );

    if (dhcpsMap_Queue == L7_NULLPTR)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
               "Failed to create DHCP Server Map Queue");
        L7_assert(1);
    }

    /* create a semaphore to control mutual exclusion access to the DHCP Server
     * configuration and dyanamic data structure
     */
    dhcpsMapCtrl_g.dhcpsDataSemId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    if (dhcpsMapCtrl_g.dhcpsDataSemId == L7_NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
               "Failed to create DHCP Server data control semaphore");
        L7_assert(1);
        return L7_FAILURE;
    }

    if (dhcpsSockInit() != L7_SUCCESS)
    {
        L7_assert(1);
        return L7_FAILURE;
    }

    /*now start dhcps_task */
    if (dhcpsStartTasks() != L7_SUCCESS)
    {
        return L7_ERROR;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize DHCP Server task
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsStartTasks(void)
{
  /* clear task init flag prior to creating the dhcps_Task; it is used as
   * part of the control mechanism to wait for dhcps_Task to initialize
   */

  /* create DHCP Server task */
  dhcpsMapCtrl_g.dhcpsTaskId = osapiTaskCreate("tDhcpsTask", dhcps_Task, 0, 0,
                                        FD_CNFGR_DHCPS_DEFAULT_STACK_SIZE,
                                        FD_CNFGR_DHCPS_DEFAULT_TASK_PRI,
                                        FD_CNFGR_DHCPS_DEFAULT_TASK_SLICE);

  if (dhcpsMapCtrl_g.dhcpsTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to create DHCP Server task");
    return L7_FAILURE;
  }

  /* create dhcp ping receive task */
  dhcpsMapCtrl_g.dhcpsPingTaskId = osapiTaskCreate("dhcpsPingTask", dhcpsPingTask, 0, 0,
                                        FD_CNFGR_DHCPS_DEFAULT_STACK_SIZE,
                                        FD_CNFGR_DHCPS_DEFAULT_TASK_PRI,
                                        FD_CNFGR_DHCPS_DEFAULT_TASK_SLICE);

  if (dhcpsMapCtrl_g.dhcpsPingTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to create DHCP ping receive task");
    return L7_FAILURE;
  }

  /* must wait here until the dhcps_Task has reached a certain state
   * of initialization, or else things will fail when trying to
   * apply the configuration data for the DHCP Server interfaces (due to
   * uninitialized pointers, etc.)
   */
  if (osapiWaitForTaskInit(L7_DHCPS_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to synchronize with DHCP Server task");
    return L7_FAILURE;
  }

  dhcpsMapCtrl_g.dhcpsInitialized = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Save DHCP Server user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSave(void)
{
  if (pDhcpsMapCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    pDhcpsMapCfgData->cfgHdr.dataChanged = L7_FALSE;
    pDhcpsMapCfgData->checkSum =
    nvStoreCrc32((L7_char8 *)pDhcpsMapCfgData,
                 sizeof(L7_dhcpsMapCfg_t) - sizeof(pDhcpsMapCfgData->checkSum));

    if (sysapiCfgFileWrite(L7_DHCPS_MAP_COMPONENT_ID, L7_DHCPS_CFG_FILENAME,
                           (L7_char8 *)pDhcpsMapCfgData, sizeof(L7_dhcpsMapCfg_t)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
             "Failed to save the configuration file");
      return L7_FAILURE;
    }
  }

  /* save lease data as well */
  dhcpsSaveLeaseCfg(__FILE__, __LINE__);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Save DHCP Server user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSaveLeaseCfg(L7_uchar8 *file, L7_uint32 line)
{
  /*if(pDhcpsMapLeaseCfgData->cfgHdr.dataChanged == L7_TRUE)*/
  /* Do not check the data changed flag as this may have been reset due to run cfg logic
   * save it every time nvStoreSave is invoked
   */
  {
    pDhcpsMapLeaseCfgData->cfgHdr.dataChanged = L7_FALSE;
    pDhcpsMapLeaseCfgData->checkSum =
    nvStoreCrc32((L7_char8 *)pDhcpsMapLeaseCfgData,
                 sizeof(L7_dhcpsMapLeaseCfg_t) - sizeof(pDhcpsMapLeaseCfgData->checkSum));

    /* call config file save routine */
    if (sysapiBinaryCfgFileWrite(L7_DHCPS_MAP_COMPONENT_ID, L7_DHCPS_LEASE_CFG_FILENAME,
            (L7_char8 *)pDhcpsMapLeaseCfgData, sizeof(L7_dhcpsMapLeaseCfg_t)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
             "Failed to save the lease configuration file");
      return L7_FAILURE;
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_CFG,
                      "dhcpsSaveLeaseCfg: %s:%d Config saved\n", file, line);
    }
  }

  return L7_SUCCESS;
}

#ifdef L7_NSF_PACKAGE
/*********************************************************************
* @purpose  Tell unitmgr to save lease config and propagate to backup
*
* @param    file        @b{(input)} file call used in (debug)
* @param    line        @b{(input)} line call used in (debug)
* @param    immediate   @b{(input)} send config immediately or not
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsSaveLeaseCfgNSF(L7_uchar8 *file, L7_uint32 line, L7_BOOL immediate)
{
  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_CFG,
                "dhcpsSaveLeaseCfgNSF: %s:%d tell unitmgr to copy lease cfg to backup %s\n",
                file, line, ((immediate == L7_TRUE) ? "Now" : "Later"));
  unitMgrSendRunCfg(immediate);

  return;
}
#endif /* L7_NSF_PACKAGE */

/*********************************************************************
* @purpose  Check if DHCP Server user config or lease config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dhcpsHasDataChanged(void)
{
  return (pDhcpsMapCfgData->cfgHdr.dataChanged || pDhcpsMapLeaseCfgData->cfgHdr.dataChanged);
}
void dhcpsResetDataChanged(void)
{
  pDhcpsMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  pDhcpsMapLeaseCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Build default DHCP Server config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsBuildDefaultConfigData(L7_uint32 ver)
{

  L7_uint32 index;

  /*--------------------------*/
  /* build config file header */
  /*--------------------------*/

  memset(pDhcpsMapCfgData, 0, sizeof(L7_dhcpsMapCfg_t));
  strcpy(pDhcpsMapCfgData->cfgHdr.filename, L7_DHCPS_CFG_FILENAME);
  pDhcpsMapCfgData->cfgHdr.version       = ver;
  pDhcpsMapCfgData->cfgHdr.componentID   = L7_DHCPS_MAP_COMPONENT_ID;
  pDhcpsMapCfgData->cfgHdr.type          = L7_CFG_DATA;
  pDhcpsMapCfgData->cfgHdr.length        = sizeof(L7_dhcpsMapCfg_t);
  pDhcpsMapCfgData->cfgHdr.dataChanged   = L7_FALSE;

  /*---------------------------*/
  /* build default config data */
  /*---------------------------*/

  /* generic DHCP Server cfg */
  pDhcpsMapCfgData->dhcps.dhcpsAdminMode = FD_DHCPS_DEFAULT_ADMIN_MODE;
  pDhcpsMapCfgData->dhcps.dhcpsPingPktNo = FD_DHCPS_DEFAULT_PINGPKTNO;
  pDhcpsMapCfgData->dhcps.dhcpConflictLogging = FD_DHCPS_DEFAULT_CONFLICTLOGGING;
  pDhcpsMapCfgData->dhcps.dhcpBootpAuto = FD_DHCPS_DEFAULT_BOOTPAUTO;

  for (index = 0; index <  L7_DHCPS_MAX_POOL_NUMBER; index++)
  {
    pDhcpsMapCfgData->pool[index].leaseTime=FD_DHCPS_DEFAULT_POOLCFG_LEASETIME * 60; /* convert into seconds*/
    pDhcpsMapCfgData->pool[index].poolType=FD_DHCPS_DEFAULT_POOLCFG_TYPEOFBINDING;

    pDhcpsMapCfgData->pool[index].DNSServerAddrs[0] = FD_DHCPS_DEFAULT_DNS_SERVER;
    pDhcpsMapCfgData->pool[index].defaultrouterAddrs[0] = FD_DHCPS_DEFAULT_ROUTER;

    strcpy(pDhcpsMapCfgData->pool[index].hostName, FD_DHCPS_DEFAULT_CLIENT_NAME);

    strcpy(pDhcpsMapCfgData->pool[index].hostIdentifier, FD_DHCPS_DEFAULT_CLIENT_ID);
    pDhcpsMapCfgData->pool[index].hostIdLength = 0;

    strcpy(pDhcpsMapCfgData->pool[index].hostAddr, FD_DHCPS_DEFAULT_HARDWARE_ADDRESS);
    pDhcpsMapCfgData->pool[index].hostAddrLength = strlen(FD_DHCPS_DEFAULT_HARDWARE_ADDRESS);
    pDhcpsMapCfgData->pool[index].hostAddrtype = FD_DHCPS_DEFAULT_HARDWARE_TYPE;

    strcpy(pDhcpsMapCfgData->pool[index].domainName, FD_DHCPS_DEFAULT_DOMAIN_NAME);
    strcpy(pDhcpsMapCfgData->pool[index].clientBootFile, FD_DHCPS_DEFAULT_BOOTFILE_NAME);
    pDhcpsMapCfgData->pool[index].nextServer = FD_DHCPS_DEFAULT_NEXTSERVERIP_ADDR;
    pDhcpsMapCfgData->pool[index].netBiosNameServer[0] = FD_DHCPS_DEFAULT_NBNAMESERVERIP_ADDR;
    pDhcpsMapCfgData->pool[index].netBiosNodeType = FD_DHCPS_DEFAULT_NBNODETYPE;
  }
}

/*********************************************************************
* @purpose  Build default DHCP Server lease config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsBuildDefaultLeaseConfigData(L7_uint32 ver)
{
  /*--------------------------*/
  /* build config file header */
  /*--------------------------*/
  memset(pDhcpsMapLeaseCfgData, 0, sizeof(L7_dhcpsMapLeaseCfg_t));
  strcpy(pDhcpsMapLeaseCfgData->cfgHdr.filename, L7_DHCPS_LEASE_CFG_FILENAME);
  pDhcpsMapLeaseCfgData->cfgHdr.version       = ver;
  pDhcpsMapLeaseCfgData->cfgHdr.componentID   = L7_DHCPS_MAP_COMPONENT_ID;
  pDhcpsMapLeaseCfgData->cfgHdr.type          = L7_CFG_DATA;
  pDhcpsMapLeaseCfgData->cfgHdr.length        = sizeof(L7_dhcpsMapLeaseCfg_t);
  pDhcpsMapLeaseCfgData->cfgHdr.dataChanged   = L7_FALSE;
}



/*********************************************************************
* @purpose  Apply DHCP Server config data
*
* @param    void
*
* @returns  L7_SUCCESS  Config data applied
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsApplyConfigData(void)
{
  L7_uint32     poolIndex,leaseIndex;
  L7_uint32     secs;
  L7_RC_t       rc;
  dhcpsPoolNode_t  *pPool;
  dhcpsLeaseNode_t *pLease;
  L7_BOOL          leasesRestored = L7_FALSE;

  /* check validity of admin mode */
  if (! (pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_ENABLE ||
         pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_DISABLE) )
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
           "Invalid admin mode is specified.");
    return L7_FAILURE;
  }


  /*--------------------------------------------*/
  /* configure Dynamic struct DHCP Server parms */
  /*--------------------------------------------*/

  /* create client lease AVL table */
  if(dhcpsClientLeaseTableCreate() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to create client lease table");
    return L7_FAILURE;
  }

  /* Go through all pools one by one. Restore Pool and lease data */
  for(poolIndex=0; poolIndex < L7_DHCPS_MAX_POOL_NUMBER ; poolIndex++)
  {
    /* create pool */
    if(pDhcpsMapCfgData->pool[poolIndex].poolType == DYNAMIC_POOL)
    {
      rc = dhcpsCreatePool(&(pDhcpsMapCfgData->pool[poolIndex]),&pPool);
    }
    else if(pDhcpsMapCfgData->pool[poolIndex].poolType == MANUAL_POOL)
    {
      rc = dhcpsCreateManualPool(&(pDhcpsMapCfgData->pool[poolIndex]),&pPool);
    }
    else
    {
      continue;
    }

    if(L7_SUCCESS == rc)
    {
      /* create leases for pool */
      for(leaseIndex=0; leaseIndex < L7_DHCPS_MAX_LEASE_NUMBER ; leaseIndex++)
      {
        /* Looking for all leases except free ones */
        if((pDhcpsMapLeaseCfgData->lease[leaseIndex].state != FREE_LEASE ) &&
         (pDhcpsMapLeaseCfgData->lease[leaseIndex].poolName[0] != 0 ) &&
         (strncmp(pDhcpsMapLeaseCfgData->lease[leaseIndex].poolName,
                  pDhcpsMapCfgData->pool[poolIndex].poolName,
                             L7_DHCPS_POOL_NAME_MAXLEN) == 0) )
        {
          /* Add this lease */
           if(dhcpsCreateLeaseNode( leaseIndex , pPool, &pLease) != L7_SUCCESS)
           {
             L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
                    "Failed to apply DHCP Server configuration data");

             if (leasesRestored == L7_TRUE)
             {
               DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
                             "dhcpsApplyConfigData: Error applying DHCP Server config data 1\n");
               DHCPS_SET_LEASE_CONFIG_DATA_DIRTY_LATER;
             }
             return L7_FAILURE;
           }

           /* Add active and offered lease to scheduled leases */
          if( pDhcpsMapLeaseCfgData->lease[leaseIndex].IsBootp == L7_FALSE)
          {
            if(pDhcpsMapLeaseCfgData->lease[leaseIndex].state == ACTIVE_LEASE)
            {
#ifdef L7_NSF_PACKAGE
              L7_uint32 upTime = simSystemUpTimeGet();

              L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DHCPS_MAP_COMPONENT_ID,
                      "dhcpsApplyConfigData: leaseIndex %d, expiration %d, upTime %d\n",
                      leaseIndex, pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseExpiration, upTime);

              if (pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseExpiration > upTime)
              {
                if ((pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseExpiration - upTime) >
                    pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseTime)
                {
                  secs = pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseTime;
                }
                else
                {
                  secs = pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseExpiration - upTime;
                }
                L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DHCPS_MAP_COMPONENT_ID,
                        "dhcpsApplyConfigData: scheduling lease for %d seconds\n", secs);
                dhcpsScheduleLease(pLease, secs);
                leasesRestored = L7_TRUE;
              }
              else
              {
                /*
                dhcpsTransitionLeaseState(pExpiredLease, EXPIRED_LEASE);
                */
              }
#else
              secs = pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseTime ;
              dhcpsScheduleLease(pLease, secs);
              leasesRestored = L7_TRUE;
#endif /* L7_NSF_PACKAGE */
            }
            else if(pDhcpsMapLeaseCfgData->lease[leaseIndex].state == OFFERED_LEASE)
            {
              secs = DHCPS_OFFER_VALIDITY_DURATION_SECS;
              dhcpsScheduleLease(pLease, secs);
              leasesRestored = L7_TRUE;
            }
          }
        }
      } /* end of lease search for loop */
    } /* end if for pool add */
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
              "dhcpsApplyConfigData: Error applying DHCP Server config data 2\n");
      if (leasesRestored == L7_TRUE)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
                      "dhcpsApplyConfigData: Error applying DHCP Server config data 2\n");
        DHCPS_SET_LEASE_CONFIG_DATA_DIRTY_LATER;
      }
      return L7_FAILURE;
    }
  } /* end of pool search for loop */

  if (leasesRestored == L7_TRUE)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
                  "dhcpsApplyConfigData: All leases restored\n");
    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY_LATER;
  }
  return L7_SUCCESS;
}


void dumpIpPkt(L7_ipHeader_t * ipHeader, sysnet_pdu_info_t *pduInfo)
{
  L7_udp_header_t *udpHeader;

    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
    "RecvInf/vlan (%x/%d), destInf/vlan (%x/%d), proto (%d), src/dest (%x/%x)",
    pduInfo->intIfNum,  pduInfo->vlanId,
    pduInfo->destIntIfNum,  pduInfo->destVlanId,
    ipHeader->iph_prot,
    osapiNtohl(ipHeader->iph_src), osapiNtohl(ipHeader->iph_dst));

  if(ipHeader->iph_prot == IP_PROT_UDP)
  {
    udpHeader = (L7_udp_header_t *)((L7_char8 *)ipHeader +
      sizeof(L7_ipHeader_t));

    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
    ", dest port(%d)\n", osapiNtohs(udpHeader->destPort));
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"\n");
  }
}

/*********************************************************************
* @purpose  Is the given packet destination valid?
*
* @param    intIfNum      Interface packet was received on
* @param    serverIpAddr  Address of DHCP server
* @param    ipHeader      IP packet
*
* @returns  L7_TRUE       If packet destination is valid
* @returns  L7_FALSE      Otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dhcpsIsValidDest(L7_uint32 intIfNum, L7_uint32 serverIpAddr,
                         L7_ipHeader_t *ipHeader)
{
#ifdef L7_ROUTING_PACKAGE
  L7_uint32 rtrIfNum = 0;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
#endif

  if( (osapiNtohl(ipHeader->iph_dst) == L7_IP_LTD_BCAST_ADDR) ||
      (osapiNtohl(ipHeader->iph_dst) == serverIpAddr))
  {
    return L7_TRUE;
  }
#ifdef L7_ROUTING_PACKAGE
  /* A packet is received on a routing interface (but the
    * dst addr doesnt match the ifaddr). Accept it if it is
    * destined to any valid routing interface addr.
    * This is required for cases where a relayed packet is
    * routed to the server.
    */
  if ((ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS) &&
      (ipMapRtrAdminModeGet() == L7_ENABLE))
  {
    /* Routing is enabled globally. Packet was received on a routing interface. */
    if (ipMapIpAddressToIntf(osapiNtohl(ipHeader->iph_dst), &intIfNum) == L7_SUCCESS)
    {
      /* destination IP address is a local address */
      return L7_TRUE;
    }
    if ((ipMapRouterIfResolve(osapiNtohl(ipHeader->iph_dst), &intIfNum) == L7_SUCCESS) &&
        (ipMapNetDirBcastMatchCheck(intIfNum, osapiNtohl(ipHeader->iph_dst),
                                    macAddr) == L7_SUCCESS))
    {
      /* Also accept packets to the network directed broadcast address of
       * one of our interfaces */
      return L7_TRUE;
    }
  }
#endif

  return L7_FALSE;
}

/*********************************************************************
 * @purpose  Map the Lease data structures to existing pools
 *
 *
 * @returns  L7_TRUE       If Success
 * @returns  L7_FALSE      Otherwise
 *
 * @notes    none
 *
 * @end
 *********************************************************************/


L7_RC_t dhcpsInitMappingLeaseConfigData()
{
  L7_uint32     poolIndex,leaseIndex;
  L7_uint32     secs;
  L7_RC_t       rc;
  dhcpsPoolNode_t  *pPool;
  dhcpsLeaseNode_t *pLease;

  dhcpsLink_t     *pLink;
  dhcpsPoolNode_t *pPoolTemp;
  L7_BOOL         leasesRestored = L7_FALSE;

  rc = L7_SUCCESS;

  /* check validity of admin mode */
  if (! (pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_ENABLE ||
    pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_DISABLE) )
  {

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
           "Invalid admin mode is specified");
    return L7_FAILURE;
  }



  /*--------------------------------------------*/
  /* configure Dynamic struct DHCP Server parms */
  /*--------------------------------------------*/

  /* create client lease AVL table */
  if(dhcpsClientLeaseTableCreate() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to create client lease table");
    return L7_FAILURE;
  }



  /* Go through all pools one by one. Restore Pool and Lease data */
  for(poolIndex=0; poolIndex < L7_DHCPS_MAX_POOL_NUMBER ; poolIndex++)
  {
    if( (pDhcpsMapCfgData->pool[poolIndex].poolType == DYNAMIC_POOL) ||
    (pDhcpsMapCfgData->pool[poolIndex].poolType == MANUAL_POOL) )
    {
      if(pDhcpsMapCfgData->pool[poolIndex].poolName[0] != 0)
      {
    /*get the POOL */
    pPool = L7_NULLPTR;
    pLink = (pDhcpsInfo->dhcpsPoolsHead).next;

    /* iterate pool leases */
    while(L7_NULLPTR != pLink)
    {
      pPoolTemp = (dhcpsPoolNode_t *)pLink->object;

      if(L7_NULLPTR != pPoolTemp)
      {
        if(pDhcpsMapCfgData->pool[poolIndex].ipAddr == pPoolTemp->poolData->ipAddr)
        {
          /* matches */
          pPool = pPoolTemp;
          break;
        }
        else if(pDhcpsMapCfgData->pool[poolIndex].ipAddr < pPoolTemp->poolData->ipAddr)
        {
          /* pools are in ascending order of ipaddr, so no match for this ipaddr! */
          break;
        }
      }
      else
      {
        break;
      }
      /* get next */
      pLink = (pPoolTemp->poolsLink).next;
    }

    if(L7_NULLPTR != pPool)
    {
      /* create leases for pool */
      for(leaseIndex=0; leaseIndex < L7_DHCPS_MAX_LEASE_NUMBER ; leaseIndex++)
      {
        /* Looking for all leases except free ones */
        if((pDhcpsMapLeaseCfgData->lease[leaseIndex].state != FREE_LEASE ) &&
        (pDhcpsMapLeaseCfgData->lease[leaseIndex].poolName[0] != 0 ) &&
        (strncmp(pDhcpsMapLeaseCfgData->lease[leaseIndex].poolName,
             pDhcpsMapCfgData->pool[poolIndex].poolName,
             L7_DHCPS_POOL_NAME_MAXLEN) == 0) )
        {
          /* Add this lease */
          if(dhcpsCreateLeaseNode( leaseIndex , pPool, &pLease) != L7_SUCCESS)
          {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
                       "Failed to apply DHCP Server configuration data");

        if (leasesRestored == L7_TRUE)
            {
              DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
                            "dhcpsInitMappingLeaseConfigData: Error applying DHCP Server config data\n");
              DHCPS_SET_LEASE_CONFIG_DATA_DIRTY_LATER;
            }
            return L7_FAILURE;
          }

          /* Add active and offered lease to scheduled leases */
          if( pDhcpsMapLeaseCfgData->lease[leaseIndex].IsBootp == L7_FALSE)
          {
            if(pDhcpsMapLeaseCfgData->lease[leaseIndex].state == ACTIVE_LEASE)
            {
#ifdef L7_NSF_PACKAGE
              L7_uint32 upTime = simSystemUpTimeGet();

              L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DHCPS_MAP_COMPONENT_ID,
                      "dhcpsInitMappingLeaseConfigData: leaseIndex %d, expiration %d, upTime %d\n",
                      leaseIndex, pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseExpiration, upTime);

              if (pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseExpiration > upTime)
              {
                if ((pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseExpiration - upTime) >
                    pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseTime)
                {
                    secs = pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseTime;
                }
                else
                {
                    secs = pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseExpiration - upTime;
                }
                L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DHCPS_MAP_COMPONENT_ID,
                        "dhcpsInitMappingLeaseConfigData: scheduling lease for %d seconds\n", secs);
                dhcpsScheduleLease(pLease, secs);
                leasesRestored = L7_TRUE;
              }
              else
              {
                /*
                dhcpsTransitionLeaseState(pExpiredLease, EXPIRED_LEASE);
                */
              }
#else
              secs = pDhcpsMapLeaseCfgData->lease[leaseIndex].leaseTime ;
              dhcpsScheduleLease(pLease, secs);
              leasesRestored = L7_TRUE;
#endif /* L7_NSF_PACKAGE */
            }
            else if(pDhcpsMapLeaseCfgData->lease[leaseIndex].state == OFFERED_LEASE)
            {
              secs = DHCPS_OFFER_VALIDITY_DURATION_SECS;
              dhcpsScheduleLease(pLease, secs);
              leasesRestored = L7_TRUE;
            }
          }
        }
      } /* end of lease search for loop */
    } /* end if for pool add */
      }
    }
  } /* end of pool search for loop */

  if (leasesRestored == L7_TRUE)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
                  "dhcpsInitMappingLeaseConfigData: All leases restored\n");
    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY_LATER;
  }

  return rc;
}

/*********************************************************************
* @purpose  Discard a unwanted packet not useful for DHCP Server
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED    if frame has been consumed
* @returns  SYSNET_PDU_RC_IGNORED     continue processing this frame
*
* @notes    Recall that DHCP Server packets are targeted to UDP port 67.
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t dhcpsMapFrameFilter(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc)
{
  L7_ipHeader_t *ipHeader;
  L7_udp_header_t *udpHeader;
  L7_uint32  serverIpAddr;
  L7_uint32  serverIpMask;
  L7_uchar8 *data;
  L7_uint32 ethHeaderSize, len, ipLen;

  if (pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_DISABLE)
  {
      return SYSNET_PDU_RC_IGNORED;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);

  ethHeaderSize = sysNetDataOffsetGet(data);

  ipHeader = (L7_ipHeader_t *)(data + ethHeaderSize);
  ipLen = len - ethHeaderSize;

  if(dhcpsMapCtrl_g.msgLvl > DHCPS_MAP_MSGLVL_HI)
    dumpIpPkt(ipHeader, pduInfo);

  if ((ipHeader->iph_versLen == ((L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN) )&&
      (ipHeader->iph_prot == IP_PROT_UDP))
  {
    udpHeader = (L7_udp_header_t *)((L7_char8 *)ipHeader + sizeof(L7_ipHeader_t));
    if (osapiNtohs(udpHeader->destPort) == DHCPS_SERVER_PORT)
    {
      if(dhcpsGetServerID(pduInfo->intIfNum, &serverIpAddr,  &serverIpMask) == L7_SUCCESS)
      {
        if (dhcpsIsValidDest(pduInfo->intIfNum, serverIpAddr, ipHeader) == L7_TRUE)
        {
          /* check that serverid is ok */
          if(serverIpAddr != 0)
          {
            processDhcpsFrames(ipHeader, ipLen, pduInfo->intIfNum);
            SYSAPI_NET_MBUF_FREE(bufHandle);
            return SYSNET_PDU_RC_CONSUMED;
          }
          else
          {
            DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Dropped pkt 0 server id returned");
          }
        }
        else
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"Dropped pkt destined to %x",
                        osapiNtohl(ipHeader->iph_dst));

          return SYSNET_PDU_RC_DISCARD;
        }
      }
      else
      {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"Dropped pkt failed to get server id");
      }
    }
  }

  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Process an incoming the Dhcp Packet
*
*
* @param    ipHeader     @b{(input)} IP Packet data
* @param    ipLen        @b{(input)} IP Packet length
* @param    intIfNum     @b{(input)} Interface num. through which packet arrived
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t processDhcpsFrames(L7_ipHeader_t *ipHeader, L7_uint32 ipLen,
                           L7_uint32 intIfNum)
{
  dhcpsMapMsg_t dhcpMessage;
  L7_udp_header_t *udp_header;
  dhcpsPacket_t *pDhcpPacket;
  L7_uint32 dhcpLen;

  if ( ((osapiNtohl(ipHeader->iph_src) & L7_CLASS_D_ADDR_NETWORK) == L7_CLASS_D_ADDR_NETWORK) ||
       ((osapiNtohl(ipHeader->iph_src) & L7_CLASS_E_ADDR_NETWORK) == L7_CLASS_E_ADDR_NETWORK) )
  {
    /* Illegal src IP in BOOTP/DHCP frame... tell the caller we've processed it */
    pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
    return L7_FAILURE;
  }

  udp_header = (L7_udp_header_t *)((L7_char8 *)ipHeader + sizeof(L7_ipHeader_t));

  pDhcpPacket = (dhcpsPacket_t *)((L7_char8 *)udp_header + sizeof(L7_udp_header_t));
  dhcpLen = ipLen - (sizeof(L7_ipHeader_t) + sizeof(L7_udp_header_t));

  if(dhcpLen > L7_DHCP_PACKET_LEN)
  {
    /* truncate message length if it exceeds the buffer size.
     * DHCP validates the packet down the line, so this works
     * well when the client sends large packets with a lot of
     * padding e.g. the Cisco 3750! Fix for defect 23708
     */
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Message length %u exceeded max length, truncating to %u bytes",
      dhcpLen, L7_DHCP_PACKET_LEN);

    dhcpLen = L7_DHCP_PACKET_LEN;
  }

  memset(&dhcpMessage, 0, sizeof(dhcpMessage));

  dhcpMessage.intf = osapiHtonl(intIfNum);
  dhcpMessage.destIp = ipHeader->iph_dst;
  dhcpMessage.length = osapiHtonl(dhcpLen);
  memcpy(dhcpMessage.dhcpPacketData, pDhcpPacket, dhcpLen);

  /* send on dhcps queue for processing */
  if(osapiMessageSend(dhcpsMap_Queue, &dhcpMessage, sizeof(dhcpsMapMsg_t),
        L7_NO_WAIT, L7_MSG_PRIORITY_NORM ) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Dhcps Packet Enqueue failed.\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display internal DHCP Server information for debugging
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsMapDebugShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"sysapi dhcpsMapDebugShow\n");
  if(pDhcpsInfo != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "DHCP Server Statistics\n----------------------\n");

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "DISCOVER recd        :%u\n", pDhcpsInfo->dhcpsStats.numOfDhcpsDiscoverReceived);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "REQUEST  recd        :%u\n", pDhcpsInfo->dhcpsStats.numOfDhcpsRequestReceived);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "DECLINE  recd        :%u\n", pDhcpsInfo->dhcpsStats.numOfDhcpsDeclineReceived);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "RELEASE  recd        :%u\n", pDhcpsInfo->dhcpsStats.numOfDhcpsReleaseReceived);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "INFORM   recd        :%u\n", pDhcpsInfo->dhcpsStats.numOfDhcpsInformReceived);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "BOOTP only   recd        :%u\n", pDhcpsInfo->dhcpsStats.numOfBootpOnlyReceived);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "Malformed Msgs       :%u\n", pDhcpsInfo->dhcpsStats.numOfMalformedMessages);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "Failed to copy opt 82  :%u\n", pDhcpsInfo->dhcpsStats.opt82CopyFailed);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "Bootp/Dhcp discarded :%u\n", pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "TOTAL BOOTP/DHCP RECD:%u\n", pDhcpsInfo->dhcpsStats.numOfDhcpsDiscoverReceived + pDhcpsInfo->dhcpsStats.numOfDhcpsRequestReceived + pDhcpsInfo->dhcpsStats.numOfDhcpsDeclineReceived + pDhcpsInfo->dhcpsStats.numOfDhcpsReleaseReceived + pDhcpsInfo->dhcpsStats.numOfDhcpsInformReceived + pDhcpsInfo->dhcpsStats.numOfBootpOnlyReceived);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "OFFER    sent        :%u\n", pDhcpsInfo->dhcpsStats.numOfDhcpsOfferSent);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "ACK      sent        :%u\n", pDhcpsInfo->dhcpsStats.numOfDhcpsAckSent);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "NAK      sent        :%u\n", pDhcpsInfo->dhcpsStats.numOfDhcpsNackSent);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "BOOTP only      sent        :%u\n", pDhcpsInfo->dhcpsStats.numOfBootpOnlySent);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "TOTAL BOOTP/DHCP SENT:%u\n\n", pDhcpsInfo->dhcpsStats.numOfDhcpsOfferSent + pDhcpsInfo->dhcpsStats.numOfDhcpsAckSent + pDhcpsInfo->dhcpsStats.numOfDhcpsNackSent + pDhcpsInfo->dhcpsStats.numOfBootpOnlySent);
  }
}

/*********************************************************************
* @purpose  Display Async message list for the DISCOVERs currently
*           being processed
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsMapAsyncMsgShow(void)
{
  L7_uint32 i = 0, j = 0;
  L7_uchar8 ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_BOOL          isDhcpPacket = L7_FALSE;
  L7_uint32        subnetMask = 0, requestedIPAddr = 0, leaseTime = 0, serverID = 0;
  L7_uint32        messageType = 0, optionOverload;
  L7_uchar8        clientIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN];
  L7_uchar8        clientIdLen = 0;
  dhcpsOption_t    *pParameterList= L7_NULLPTR;
  dhcpsOption_t    *option82 = NULL;
  L7_uint32        count = 0;

  if(pDhcpsAsyncInfo != L7_NULLPTR)
  {
    for(i=0; i<L7_DHCPS_MAX_ASYNC_MSGS; i++)
    {
      if(pDhcpsAsyncInfo[i].ipAddr != 0)
      {
        count++;
      }
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "The number of DHCP DISCOVER packets currently processed: %d\n", count);

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
      "The recent DHCP DISCOVER packets processed asynchronously\n---------------------------------------------------------\n");

    for(i=0; i<L7_DHCPS_MAX_ASYNC_MSGS; i++)
    {
      if(pDhcpsAsyncInfo[i].ipAddrShadow != 0)
      {
        osapiInetNtoa(pDhcpsAsyncInfo[i].ipAddrShadow, ipStr);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
          "\n(%2d)   IP Address : %s\n", i, ipStr);
        nimGetIntfName(pDhcpsAsyncInfo[i].intIfNum, L7_SYSNAME, ifName);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
          "       Incoming interface : %s\n", ifName);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
          "       Pings Issued for this ip : %d\n", pDhcpsAsyncInfo[i].pingsIssued);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
          "       Ping TimeOut left for this IP : %d msecs\n",
                  (DHCPS_PING_SELECT_TIMEOUT * pDhcpsAsyncInfo[i].pingTimeoutCnt));
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
          "       Get Next Free IP flag Ping TimeOut left for this IP : %d\n",
                  pDhcpsAsyncInfo[i].getNextFreeIp);

        isDhcpPacket = L7_FALSE;
        subnetMask = requestedIPAddr = leaseTime = serverID = 0;
        messageType = optionOverload = 0;
        memset(clientIdentifier, 0, sizeof(clientIdentifier));
        clientIdLen = 0;
        pParameterList = L7_NULLPTR;
        if(dhcpsParseDhcpPacket((dhcpsPacket_t *)pDhcpsAsyncInfo[i].dhcpPacketData,
                                 pDhcpsAsyncInfo[i].dhcpPacketLength,
                                 &isDhcpPacket, clientIdentifier, &clientIdLen, &messageType,
                                 &optionOverload, &subnetMask, &requestedIPAddr,
                                 &leaseTime, &serverID, &pParameterList, &option82) == L7_SUCCESS)
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "       DHCP Packet Contents:-\n");
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "         isDhcpPacket : %d\n",isDhcpPacket);
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "         clientIdentifier : ");
          for(j=0; j<clientIdLen; j++)
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
              "%2x%s",clientIdentifier[j],((j == (clientIdLen-1)) ? "" : ":"));
          }
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "         clientIdLen : %d\n",clientIdLen);
          osapiInetNtoa(subnetMask, ipStr);
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "         subnetMask : %s\n",ipStr);
          osapiInetNtoa(requestedIPAddr, ipStr);
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "         requestedIPAddr : %s\n",ipStr);
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "         leaseTime : %d\n",leaseTime);
        }
        else
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "DHCP Packet Contents parsing failed!!!\n");
        }
      }
    }
  }
}

/*********************************************************************
* @purpose  Sets the level of DHCP Server Mapping layer debugging messages to display
*
* @param    msgLvl      message level (0 = off, >0 on)
*
* @returns  void
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @end
*********************************************************************/
void dhcpsMapMsgLvlSet(L7_uint32 msgLvl)
{
  dhcpsMapCtrl_g.msgLvl = msgLvl;
}

/*********************************************************************
* @purpose purge DHCP Server config  and pool data
*
* @param    void
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @end
*********************************************************************/
L7_RC_t dhcpsPurgeData(void)
{
  L7_uint32 i;
  dhcpsPoolNode_t *pPool=L7_NULL;

  if(pDhcpsMapCfgData == L7_NULL || dhcpsMapCtrl_g.dhcpsInitialized != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

   for(i=0; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
   {
    if( (pDhcpsMapCfgData->pool[i].poolName[0] != 0) &&
        (pDhcpsMapCfgData->pool[i].poolType == DYNAMIC_POOL ||
         pDhcpsMapCfgData->pool[i].poolType == MANUAL_POOL)   )
     {

       if(dhcpsFindPoolByIp(pDhcpsMapCfgData->pool[i].ipAddr, &pPool )
                                                         == L7_SUCCESS)

       {
           /* Now delete pool entries from dynamic data struct */
           dhcpsDestroyPool(&pPool);
       }
     }
   } /* End of for loop*/

   /* delete the AVL tree */
   dhcpsClientLeaseTableDelete();

   memset(pDhcpsMapCfgData, 0, sizeof(L7_dhcpsMapCfg_t));
   memset(pDhcpsMapLeaseCfgData, 0, sizeof(L7_dhcpsMapLeaseCfg_t));
   memset(pDhcpsInfo, 0, sizeof(dhcpsInfo_t));
   memset(pDhcpsAsyncInfo, 0, L7_DHCPS_MAX_ASYNC_MSGS * sizeof(dhcpsAsyncMsg_t));

   osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The DHCPS processing task uses this function to
*           process messages from the message queue, performing the tasks
*           specified in each message. The initial motivation for this
*           task is to offload initialization processing from the
*           configurator.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dhcpsProcTask( void )
{
    L7_uint32 status;
    dhcpsProcMsg_t message;


    osapiTaskInitDone(L7_DHCPS_PROC_TASK_SYNC);

    /* Loop forever, processing incoming messages */
    while (L7_TRUE)
    {
        memset(&message, 0x00, sizeof(dhcpsProcMsg_t));
        status = osapiMessageReceive(dhcpsProcQueue, &message,
                                     sizeof(dhcpsProcMsg_t), L7_WAIT_FOREVER);

        if (status == L7_SUCCESS)
        {
            switch (message.msgType)
            {
            case DHCPS_CNFGR_CMD:
                dhcpsCnfgrHandle(&message.type.cmdData);
                break;

            case DHCPS_RESTORE_EVENT:
                dhcpsRestoreProcess();
                break;

            default:
                break;
            }
        }
        else
        {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
                    "Bad status on receive message from proc queue %08x",
                     status);
        }
    } /* endwhile */

    return;
}

/*********************************************************************
* @purpose  Restores DHCPS configuration to factory defaults.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    This function clears the configuration data but does not
*           apply config data. When config data is applied, DHCPS creates
*           the ClientLeaseTable AVL tree. Doing this from restore
*           results in a leak on "clear config" because both uconfig phase 2
*           and init phase 3 are called (both of which do an apply config,
*           resulting in the leak of the AVL semaphore).
*
*
* @end
*********************************************************************/
L7_RC_t dhcpsRestoreProcess(void)
{
    dhcpsMapAdminModeSet(L7_DISABLE);

    dhcpsPurgeData();

    dhcpsBuildDefaultConfigData(L7_DHCPS_CFG_VER_CURRENT);
    pDhcpsMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    dhcpsBuildDefaultLeaseConfigData(L7_DHCPS_LEASE_CFG_VER_CURRENT);
    pDhcpsMapLeaseCfgData->cfgHdr.dataChanged = L7_TRUE;

    dhcpsInitializeOptionValidationData(pDhcpsInfo->dhcpsOptionInfo);

    return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Initialize the OptionInfo data structure
*
* @param    pOptionInfo      @b{(input)}  DHCP option data ptr
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dhcpsInitializeOptionValidationData(dhcpsOptionInfo_t * pOptionInfo)
{
  /* Pad Option */
  pOptionInfo[0].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[0].fixedLength = 1;
  pOptionInfo[0].multipleOf  = -1;
  pOptionInfo[0].minLength   = -1;

  /* End Option */
  pOptionInfo[255].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[255].fixedLength = 1;
  pOptionInfo[255].multipleOf  = -1;
  pOptionInfo[255].minLength   = -1;

  /* Subnet Mask */
  pOptionInfo[1].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[1].fixedLength = 4;
  pOptionInfo[1].multipleOf  = -1;
  pOptionInfo[1].minLength   = -1;

  /* Time Offset */
  pOptionInfo[2].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[2].fixedLength = 4;
  pOptionInfo[2].multipleOf  = -1;
  pOptionInfo[2].minLength   = -1;

  /* Router Option */
  pOptionInfo[3].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[3].fixedLength = -1;
  pOptionInfo[3].multipleOf  = 4;
  pOptionInfo[3].minLength   = 4;

  /* Time Server Option */
  pOptionInfo[4].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[4].fixedLength = -1;
  pOptionInfo[4].multipleOf  = 4;
  pOptionInfo[4].minLength   = 4;

  /* Name Server Option */
  pOptionInfo[5].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[5].fixedLength = -1;
  pOptionInfo[5].multipleOf  = 4;
  pOptionInfo[5].minLength   = 4;

  /* Domain Name Server Option */
  pOptionInfo[6].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[6].fixedLength = -1;
  pOptionInfo[6].multipleOf  = 4;
  pOptionInfo[6].minLength   = 4;

  /* Log Server Option */
  pOptionInfo[7].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[7].fixedLength = -1;
  pOptionInfo[7].multipleOf  = 4;
  pOptionInfo[7].minLength   = 4;

  /* Cookie Server Option */
  pOptionInfo[8].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[8].fixedLength = -1;
  pOptionInfo[8].multipleOf  = 4;
  pOptionInfo[8].minLength   = 4;

  /* LPR Server Option */
  pOptionInfo[9].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[9].fixedLength = -1;
  pOptionInfo[9].multipleOf  = 4;
  pOptionInfo[9].minLength   = 4;

  /* Impress Server Option */
  pOptionInfo[10].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[10].fixedLength = -1;
  pOptionInfo[10].multipleOf  = 4;
  pOptionInfo[10].minLength   = 4;

  /* Resource Location Server Option */
  pOptionInfo[11].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[11].fixedLength = -1;
  pOptionInfo[11].multipleOf  = 4;
  pOptionInfo[11].minLength   = 4;

  /* Host Name Option */
  pOptionInfo[12].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[12].fixedLength = -1;
  pOptionInfo[12].multipleOf  = -1;
  pOptionInfo[12].minLength   = 1;

  /* Boot File Size Option */
  pOptionInfo[13].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[13].fixedLength = 2;
  pOptionInfo[13].multipleOf  = -1;
  pOptionInfo[13].minLength   = -1;

  /* Merit Dump File */
  pOptionInfo[14].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[14].fixedLength = -1;
  pOptionInfo[14].multipleOf  = -1;
  pOptionInfo[14].minLength   = 1;

  /* Domain Name */
  pOptionInfo[15].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[15].fixedLength = -1;
  pOptionInfo[15].multipleOf  = -1;
  pOptionInfo[15].minLength   = 1;

  /* Swap Server */
  pOptionInfo[16].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[16].fixedLength = 4;
  pOptionInfo[16].multipleOf  = -1;
  pOptionInfo[16].minLength   = -1;

  /* Root Path */
  pOptionInfo[17].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[17].fixedLength = -1;
  pOptionInfo[17].multipleOf  = -1;
  pOptionInfo[17].minLength   = 1;

  /* Extensions Path */
  pOptionInfo[18].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[18].fixedLength = -1;
  pOptionInfo[18].multipleOf  = -1;
  pOptionInfo[18].minLength   = 1;

  /* IP Forwarding Enable/Disable Option */
  pOptionInfo[19].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[19].fixedLength = 1;
  pOptionInfo[19].multipleOf  = -1;
  pOptionInfo[19].minLength   = -1;

  /* Non-Local Source Routing Enable/Disable Option */
  pOptionInfo[20].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[20].fixedLength = 1;
  pOptionInfo[20].multipleOf  = -1;
  pOptionInfo[20].minLength   = -1;

  /* Policy Filter Option */
  pOptionInfo[21].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[21].fixedLength = -1;
  pOptionInfo[21].multipleOf  = 8;
  pOptionInfo[21].minLength   = 8;

  /* Maximum Datagram Reassembly Size */
  pOptionInfo[22].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[22].fixedLength = 2;
  pOptionInfo[22].multipleOf  = -1;
  pOptionInfo[22].minLength   = -1;

  /* Default IP Time-to-live */
  pOptionInfo[23].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[23].fixedLength = 1;
  pOptionInfo[23].multipleOf  = -1;
  pOptionInfo[23].minLength   = -1;

  /* Path MTU Aging Timeout Option */
  pOptionInfo[24].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[24].fixedLength = 4;
  pOptionInfo[24].multipleOf  = -1;
  pOptionInfo[24].minLength   = -1;

  /* Path MTU Plateau Table Option */
  pOptionInfo[25].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[25].fixedLength = -1;
  pOptionInfo[25].multipleOf  = 2;
  pOptionInfo[25].minLength   = 2;

  /* Interface MTU Option */
  pOptionInfo[26].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[26].fixedLength = 2;
  pOptionInfo[26].multipleOf  = -1;
  pOptionInfo[26].minLength   = -1;

  /* All Subnets are Local Option */
  pOptionInfo[27].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[27].fixedLength = 1;
  pOptionInfo[27].multipleOf  = -1;
  pOptionInfo[27].minLength   = -1;

  /* Broadcast Address Option */
  pOptionInfo[28].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[28].fixedLength = 4;
  pOptionInfo[28].multipleOf  = -1;
  pOptionInfo[28].minLength   = -1;

  /* Perform Mask Discovery Option */
  pOptionInfo[29].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[29].fixedLength = 1;
  pOptionInfo[29].multipleOf  = -1;
  pOptionInfo[29].minLength   = -1;

  /* Mask Supplier Option */
  pOptionInfo[30].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[30].fixedLength = 1;
  pOptionInfo[30].multipleOf  = -1;
  pOptionInfo[30].minLength   = -1;

  /* Perform Router Discovery Option */
  pOptionInfo[31].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[31].fixedLength = 1;
  pOptionInfo[31].multipleOf  = -1;
  pOptionInfo[31].minLength   = -1;

  /* Router Solicitation Address Option */
  pOptionInfo[32].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[32].fixedLength = 4;
  pOptionInfo[32].multipleOf  = -1;
  pOptionInfo[32].minLength   = -1;

  /* Static Route Option */
  pOptionInfo[33].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[33].fixedLength = -1;
  pOptionInfo[33].multipleOf  = 8;
  pOptionInfo[33].minLength   = 8;

  /* Trailer Encapsulation Option */
  pOptionInfo[34].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[34].fixedLength = 1;
  pOptionInfo[34].multipleOf  = -1;
  pOptionInfo[34].minLength   = -1;

  /* ARP Cache Timeout Option */
  pOptionInfo[35].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[35].fixedLength = 4;
  pOptionInfo[35].multipleOf  = -1;
  pOptionInfo[35].minLength   = -1;

  /* Ethernet Encapsulation Option */
  pOptionInfo[36].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[36].fixedLength = 1;
  pOptionInfo[36].multipleOf  = -1;
  pOptionInfo[36].minLength   = -1;

  /* TCP Default TTL Option */
  pOptionInfo[37].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[37].fixedLength = 1;
  pOptionInfo[37].multipleOf  = -1;
  pOptionInfo[37].minLength   = -1;

  /* TCP Keepalive Interval Option */
  pOptionInfo[38].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[38].fixedLength = 4;
  pOptionInfo[38].multipleOf  = -1;
  pOptionInfo[38].minLength   = -1;

  /* TCP Keepalive Garbage Option */
  pOptionInfo[39].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[39].fixedLength = 1;
  pOptionInfo[39].multipleOf  = -1;
  pOptionInfo[39].minLength   = -1;

  /* Network Information Service Domain Option */
  pOptionInfo[40].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[40].fixedLength = -1;
  pOptionInfo[40].multipleOf  = -1;
  pOptionInfo[40].minLength   = 1;

  /* Network Information Servers Option */
  pOptionInfo[41].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[41].fixedLength = -1;
  pOptionInfo[41].multipleOf  = 4;
  pOptionInfo[41].minLength   = 4;

  /* Network Time Protocol Servers Option */
  pOptionInfo[42].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[42].fixedLength = -1;
  pOptionInfo[42].multipleOf  = 4;
  pOptionInfo[42].minLength   = 4;

  /* Vendor Specific Information */
  pOptionInfo[43].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[43].fixedLength = -1;
  pOptionInfo[43].multipleOf  = -1;
  pOptionInfo[43].minLength   = 1;

  /* NetBIOS over TCP/IP Name Server Option */
  pOptionInfo[44].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[44].fixedLength = -1;
  pOptionInfo[44].multipleOf  = 4;
  pOptionInfo[44].minLength   = 4;

  /* NetBIOS over TCP/IP Datagram Distribution Server Option */
  pOptionInfo[45].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[45].fixedLength = -1;
  pOptionInfo[45].multipleOf  = 4;
  pOptionInfo[45].minLength   = 4;

  /* NetBIOS over TCP/IP Node Type Option */
  pOptionInfo[46].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[46].fixedLength = 1;
  pOptionInfo[46].multipleOf  = -1;
  pOptionInfo[46].minLength   = -1;

  /* NetBIOS over TCP/IP Scope Option */
  pOptionInfo[47].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[47].fixedLength = -1;
  pOptionInfo[47].multipleOf  = -1;
  pOptionInfo[47].minLength   = 1;

  /* X Window System Font Server Option */
  pOptionInfo[48].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[48].fixedLength = -1;
  pOptionInfo[48].multipleOf  = 4;
  pOptionInfo[48].minLength   = 4;

  /* X Window System Display Manager Option */
  pOptionInfo[49].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[49].fixedLength = -1;
  pOptionInfo[49].multipleOf  = 4;
  pOptionInfo[49].minLength   = 4;

  /* Network Information Service+ Domain Option */
  pOptionInfo[64].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[64].fixedLength = -1;
  pOptionInfo[64].multipleOf  = -1;
  pOptionInfo[64].minLength   = 1;

  /* Network Information Service+ Servers Option */
  pOptionInfo[65].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[65].fixedLength = -1;
  pOptionInfo[65].multipleOf  = 4;
  pOptionInfo[65].minLength   = 4;

  /* Mobile IP Home Agent option */
  pOptionInfo[68].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[68].fixedLength = -1;
  pOptionInfo[68].multipleOf  = 4;
  pOptionInfo[68].minLength   = 0;

  /* Simple Mail Transport Protocol (SMTP) Server Option */
  pOptionInfo[69].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[69].fixedLength = -1;
  pOptionInfo[69].multipleOf  = 4;
  pOptionInfo[69].minLength   = 4;

  /* Post Office Protocol (POP3) Server Option */
  pOptionInfo[70].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[70].fixedLength = -1;
  pOptionInfo[70].multipleOf  = 4;
  pOptionInfo[70].minLength   = 4;

  /* Network News Transport Protocol (NNTP) Server Option */
  pOptionInfo[71].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[71].fixedLength = -1;
  pOptionInfo[71].multipleOf  = 4;
  pOptionInfo[71].minLength   = 4;

  /* Default World Wide Web (WWW) Server Option */
  pOptionInfo[72].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[72].fixedLength = -1;
  pOptionInfo[72].multipleOf  = 4;
  pOptionInfo[72].minLength   = 4;

  /* Default Finger Server Option */
  pOptionInfo[73].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[73].fixedLength = -1;
  pOptionInfo[73].multipleOf  = 4;
  pOptionInfo[73].minLength   = 4;

  /* Default Internet Relay Chat (IRC) Server Option */
  pOptionInfo[74].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[74].fixedLength = -1;
  pOptionInfo[74].multipleOf  = 4;
  pOptionInfo[74].minLength   = 4;

  /* StreetTalk Server Option */
  pOptionInfo[75].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[75].fixedLength = -1;
  pOptionInfo[75].multipleOf  = 4;
  pOptionInfo[75].minLength   = 4;

  /* StreetTalk Directory Assistance (STDA) Server Option */
  pOptionInfo[76].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[76].fixedLength = -1;
  pOptionInfo[76].multipleOf  = 4;
  pOptionInfo[76].minLength   = 4;

  /* Requested IP Address */
  pOptionInfo[50].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[50].fixedLength = 4;
  pOptionInfo[50].multipleOf  = -1;
  pOptionInfo[50].minLength   = -1;

  /* IP Address Lease Time */
  pOptionInfo[51].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[51].fixedLength = 4;
  pOptionInfo[51].multipleOf  = -1;
  pOptionInfo[51].minLength   = -1;

  /* Option Overload */
  pOptionInfo[52].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[52].fixedLength = 1;
  pOptionInfo[52].multipleOf  = -1;
  pOptionInfo[52].minLength   = -1;

  /* TFTP server name */
  pOptionInfo[66].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[66].fixedLength = -1;
  pOptionInfo[66].multipleOf  = -1;
  pOptionInfo[66].minLength   = 1;

  /* Bootfile name */
  pOptionInfo[67].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[67].fixedLength = -1;
  pOptionInfo[67].multipleOf  = -1;
  pOptionInfo[67].minLength   = 1;

  /* DHCP Message Type */
  pOptionInfo[53].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[53].fixedLength = -1;
  pOptionInfo[53].multipleOf  = -1;
  pOptionInfo[53].minLength   = 1;

  /* Server Identifier */
  pOptionInfo[54].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[54].fixedLength = 4;
  pOptionInfo[54].multipleOf  = -1;
  pOptionInfo[54].minLength   = -1;

  /* Parameter Request List */
  pOptionInfo[55].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[55].fixedLength = -1;
  pOptionInfo[55].multipleOf  = -1;
  pOptionInfo[55].minLength   = 1;

  /* Message */
  pOptionInfo[56].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[56].fixedLength = -1;
  pOptionInfo[56].multipleOf  = -1;
  pOptionInfo[56].minLength   = 1;

  /* Maximum DHCP Message Size */
  pOptionInfo[57].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[57].fixedLength = 2;
  pOptionInfo[57].multipleOf  = -1;
  pOptionInfo[57].minLength   = -1;

  /* Renewal (T1) Time Value */
  pOptionInfo[58].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[58].fixedLength = 4;
  pOptionInfo[58].multipleOf  = -1;
  pOptionInfo[58].minLength   = -1;

  /* Rebinding (T2) Time Value */
  pOptionInfo[59].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[59].fixedLength = 4;
  pOptionInfo[59].multipleOf  = -1;
  pOptionInfo[59].minLength   = -1;

  /* Vendor class identifier */
  pOptionInfo[60].isAllowed   = DHCPS_OPTION_RFC2132;
  pOptionInfo[60].fixedLength = -1;
  pOptionInfo[60].multipleOf  = -1;
  pOptionInfo[60].minLength   = 1;

  /* Client-identifier */
  pOptionInfo[61].isAllowed   = DHCPS_OPTION_NOT_ALLOWED;
  pOptionInfo[61].fixedLength = -1;
  pOptionInfo[61].multipleOf  = -1;
  pOptionInfo[61].minLength   = 2;

  return L7_SUCCESS;
}



/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/


void dhcpsBuildTestConfigData(L7_uint32 ver);
static void dhcpsBuildTestLeaseConfigData(L7_uint32 ver);


void dhcpsConfigDataTestShow(int showAll);
void dhcpsCfgDataShow(int showAll);

void dhcpsLeaseConfigDataTestShow(int showAll);
void dhcpsLeaseCfgDataShow(int showAll);

/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void dhcpsBuildTestConfigData(L7_uint32 ver)
{
  L7_uint32 index, nudge, i;
  L7_char8 strVal[] = "dummy";

  memset(pDhcpsMapCfgData, 0, sizeof(L7_dhcpsMapCfg_t));

  /*---------------------------*/
  /* build non-default config data */
  /*---------------------------*/

  /* generic DHCP Server cfg */
  pDhcpsMapCfgData->dhcps.dhcpsAdminMode = L7_ENABLE;
  pDhcpsMapCfgData->dhcps.dhcpsPingPktNo = FD_DHCPS_DEFAULT_PINGPKTNO + 1;
  pDhcpsMapCfgData->dhcps.dhcpConflictLogging = L7_FALSE;
  pDhcpsMapCfgData->dhcps.dhcpBootpAuto = L7_TRUE;

  /* DHCP Server pool cfg */
  for (index = 0; index <  L7_DHCPS_MAX_POOL_NUMBER; index++)
  {
    nudge = index + 1;

    pDhcpsMapCfgData->pool[index].leaseTime= FD_DHCPS_DEFAULT_POOLCFG_LEASETIME + nudge; /* convert into seconds*/
    pDhcpsMapCfgData->pool[index].poolType= DYNAMIC_POOL;

    for(i = 0; i < L7_DHCPS_DNS_SERVER_MAX; i++)
      pDhcpsMapCfgData->pool[index].DNSServerAddrs[i] = FD_DHCPS_DEFAULT_DNS_SERVER + i + 1;

    for(i = 0; i < L7_DHCPS_DEFAULT_ROUTER_MAX; i++)
      pDhcpsMapCfgData->pool[index].defaultrouterAddrs[i] = FD_DHCPS_DEFAULT_ROUTER + i + 1;

    strcpy(pDhcpsMapCfgData->pool[index].hostName, strVal);

    strcpy(pDhcpsMapCfgData->pool[index].hostIdentifier, strVal);
    pDhcpsMapCfgData->pool[index].hostIdLength = nudge;

    strcpy(pDhcpsMapCfgData->pool[index].hostAddr, strVal);
    pDhcpsMapCfgData->pool[index].hostAddrLength = nudge;
    pDhcpsMapCfgData->pool[index].hostAddrtype = L7_DHCPS_IEEE_802;

    strcpy(pDhcpsMapCfgData->pool[index].domainName, strVal);
    strcpy(pDhcpsMapCfgData->pool[index].clientBootFile, strVal);
    pDhcpsMapCfgData->pool[index].nextServer = FD_DHCPS_DEFAULT_NEXTSERVERIP_ADDR + nudge;

    for(i = 0; i < L7_DHCPS_NETBIOS_NAME_SERVER_MAX; i++)
      pDhcpsMapCfgData->pool[index].netBiosNameServer[i] = FD_DHCPS_DEFAULT_NBNAMESERVERIP_ADDR + i + 1;

    pDhcpsMapCfgData->pool[index].netBiosNodeType = FD_DHCPS_DEFAULT_NBNODETYPE + nudge;
    memset(pDhcpsMapCfgData->pool[index].options, 0xab, L7_DHCPS_POOL_OPTION_BUFFER);
  }

  /* DHCP Server excluded addresses cfg */
  for(i = 0; i < L7_DHCPS_MAX_EXCLUDE_NUMBER; i++)
  {
    nudge = i + 1;
    pDhcpsMapCfgData->excludedAddrs[i].fromAddr = nudge;
    pDhcpsMapCfgData->excludedAddrs[i].toAddr = nudge + 1;
  }

  /* DHCP Server Lease cfg */
  dhcpsBuildTestLeaseConfigData(ver);
}

/*********************************************************************
* @purpose  Build test DHCP Server lease config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void dhcpsBuildTestLeaseConfigData(L7_uint32 ver)
{
  leaseCfgData_t *pLease;
  L7_uint32 index, nudge;
  L7_char8 strVal[] = "dummy";


  /*--------------------------*/
  /* build config file header */
  /*--------------------------*/

  memset(pDhcpsMapLeaseCfgData, 0, sizeof(L7_dhcpsMapLeaseCfg_t));

  pLease = &(pDhcpsMapLeaseCfgData->lease[0]);

  for( index = 0; index < L7_DHCPS_MAX_LEASE_NUMBER; index++)
  {
    nudge = index + 1;

    /* read-only (after lease allocation) */
    strcpy(pLease->poolName, strVal);
    memset(pLease->clientIdentifier, 0xab, L7_DHCPS_CLIENT_ID_MAXLEN);
    memset(pLease->hwAddr, 0xab, L7_DHCPS_HARDWARE_ADDR_MAXLEN);

    pLease->clientIdLength          = L7_DHCPS_CLIENT_ID_MAXLEN;
    pLease->hAddrtype               = nudge;
    pLease->hwAddrLength            = L7_DHCPS_HARDWARE_ADDR_MAXLEN;
    pLease->ipAddr                  = nudge;
    pLease->ipMask                  = nudge;
    pLease->leaseTime               = nudge;
    pLease->state                   = L7_DHCPS_EXPIRED_LEASE;
    pLease->IsBootp                 = L7_TRUE;
    pLease->conflictDetectionMethod = L7_DHCPS_GRATUITOUS_ARP;

    /* next lease */
    pLease++;
  }
}

/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void dhcpsConfigDataTestShow(int showAll)
{
    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    sysapiPrintf( "DHCPS MAP CONFIG DATA\n");
    sysapiPrintf( "---------------------\n");

    pFileHdr = &(pDhcpsMapCfgData->cfgHdr);
    sysapiCfgFileHeaderDump (pFileHdr);

   /*--------------------------------*/
   /* cfgParms                       */
   /*--------------------------------*/

    dhcpsCfgDataShow(showAll);

    /*-------------------------------*/
    /* Scaling Constants             */
    /*                               */
    /*                               */
    /* NOTE:                         */
    /* Print any limits of arrays    */
    /* or anything else in the       */
    /* confi files which might       */
    /* affect the size of the config */
    /* file.                         */
    /*                               */
    /*-------------------------------*/

    sysapiPrintf( "\n\n");
    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");

    sysapiPrintf( "L7_DHCPS_MAX_POOL_NUMBER         - %d\n", L7_DHCPS_MAX_POOL_NUMBER);
    sysapiPrintf( "L7_DHCPS_POOL_NAME_MAXLEN        - %d\n", L7_DHCPS_POOL_NAME_MAXLEN);
    sysapiPrintf( "L7_DHCPS_DEFAULT_ROUTER_MAX      - %d\n", L7_DHCPS_DEFAULT_ROUTER_MAX);
    sysapiPrintf( "L7_DHCPS_DNS_SERVER_MAX          - %d\n", L7_DHCPS_DNS_SERVER_MAX);
    sysapiPrintf( "L7_DHCPS_CLIENT_ID_MAXLEN        - %d\n", L7_DHCPS_CLIENT_ID_MAXLEN);
    sysapiPrintf( "L7_DHCPS_HARDWARE_ADDR_MAXLEN    - %d\n", L7_DHCPS_HARDWARE_ADDR_MAXLEN);
    sysapiPrintf( "L7_DHCPS_HOST_NAME_MAXLEN        - %d\n", L7_DHCPS_HOST_NAME_MAXLEN);
    sysapiPrintf( "L7_DHCPS_NETBIOS_NAME_SERVER_MAX - %d\n", L7_DHCPS_NETBIOS_NAME_SERVER_MAX);
    sysapiPrintf( "L7_DHCPS_DOMAIN_NAME_MAXLEN      - %d\n", L7_DHCPS_DOMAIN_NAME_MAXLEN);
    sysapiPrintf( "L7_DHCPS_BOOT_FILE_NAME_MAXLEN   - %d\n", L7_DHCPS_BOOT_FILE_NAME_MAXLEN);
    sysapiPrintf( "L7_DHCPS_POOL_OPTION_BUFFER      - %d\n", L7_DHCPS_POOL_OPTION_BUFFER);
    sysapiPrintf( "L7_DHCPS_MAX_EXCLUDE_NUMBER      - %d\n", L7_DHCPS_MAX_EXCLUDE_NUMBER);

    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("\n\npDhcpsMapCfgData->checkSum : %u\n", pDhcpsMapCfgData->checkSum);

    dhcpsLeaseConfigDataTestShow(showAll);
}

void dhcpsCfgDataShow(int showAll)
{
  L7_uint32 index, i, bytesDone;
  L7_char8 buf[80];
  L7_char8 lineStr[60];

  sysapiPrintf("\n+---------------------------------------------------------+");
  sysapiPrintf("\n|                 DHCPS CONFIGURATION DATA                |");
  sysapiPrintf("\n+---------------------------------------------------------+");

  /* generic DHCP Server cfg */
  sysapiPrintf("\ndhcpsAdminMode.........................%s (%d)",
               ((pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_ENABLE) ? "L7_ENABLE" : "L7_DISABLE"),
               (L7_uint32)pDhcpsMapCfgData->dhcps.dhcpsAdminMode);
  sysapiPrintf("\ndhcpsPingPktNo.........................%d", pDhcpsMapCfgData->dhcps.dhcpsPingPktNo);
  sysapiPrintf("\ndhcpConflictLogging....................%s (%d)",
               ((pDhcpsMapCfgData->dhcps.dhcpConflictLogging == L7_TRUE) ? "L7_TRUE" : "L7_FALSE"),
               (L7_uint32)pDhcpsMapCfgData->dhcps.dhcpConflictLogging);
  sysapiPrintf("\ndhcpBootpAuto..........................%s (%d)",
               ((pDhcpsMapCfgData->dhcps.dhcpBootpAuto == L7_TRUE) ? "L7_TRUE" : "L7_FALSE"),
               (L7_uint32)pDhcpsMapCfgData->dhcps.dhcpBootpAuto);
  sysapiPrintf("\n");

  /* DHCP Server pool cfg */
  for (index = 0; index <  L7_DHCPS_MAX_POOL_NUMBER; index++)
  {
    if ((showAll == 0) && (*pDhcpsMapCfgData->pool[index].poolName == L7_NULL))
    {
      continue;
    }

    sysapiPrintf("\nPool #%d", index);
    sysapiPrintf("\n-----------------------------------------------------------");

    switch(pDhcpsMapCfgData->pool[index].poolType)
    {
    case INACTIVE_POOL:
      sysapiPrintf("\npoolType..........................INACTIVE_POOL");
      break;
    case DYNAMIC_POOL:
      sysapiPrintf("\npoolType..........................DYNAMIC_POOL");
      break;
    case MANUAL_POOL:
      sysapiPrintf("\npoolType..........................MANUAL_POOL");
      break;
    default:
      sysapiPrintf("\npoolType..........................UNKNOWN (%d)",
                   pDhcpsMapCfgData->pool[index].poolType);
      break;
    }

    sysapiPrintf("\npoolName..........................%s", (L7_uint32)pDhcpsMapCfgData->pool[index].poolName);
    sysapiPrintf("\nleaseTime.........................%d", (L7_uint32)pDhcpsMapCfgData->pool[index].leaseTime);

    for(i = 0; i < L7_DHCPS_DNS_SERVER_MAX; i++)
    {
      if ((showAll == 0) && (pDhcpsMapCfgData->pool[index].DNSServerAddrs[i] == 0))
      {
        continue;
      }

      osapiInetNtoa(pDhcpsMapCfgData->pool[index].DNSServerAddrs[i], buf);
      sysapiPrintf("\nDNSServerAddrs[%d].................%s", i, buf);
    }

    for(i = 0; i < L7_DHCPS_DEFAULT_ROUTER_MAX; i++)
    {
      if ((showAll == 0) && (pDhcpsMapCfgData->pool[index].defaultrouterAddrs[i] == 0))
      {
        continue;
      }

      osapiInetNtoa(pDhcpsMapCfgData->pool[index].defaultrouterAddrs[i], buf);
      sysapiPrintf("\ndefaultrouterAddrs[%d].............%s", i, buf);
    }

    sysapiPrintf("\nhostName..........................%s", pDhcpsMapCfgData->pool[index].hostName);

    sysapiPrintf("\nhostIdLength......................%d", (L7_uint32)pDhcpsMapCfgData->pool[index].hostIdLength);
    sysapiPrintf("\nhostIdentifier....................");
    bytesDone = 0;
    while(bytesDone < L7_DHCPS_CLIENT_ID_MAXLEN)
    {
      bytesDone += dhcpsBin2Hex(&(pDhcpsMapCfgData->pool[index].hostIdentifier[bytesDone]), /* start here */
                                L7_DHCPS_CLIENT_ID_MAXLEN - bytesDone, /* bytes left */
                                sizeof(lineStr),  /* output buffer size */
                                lineStr);         /* output buffer */

      sysapiPrintf("\n %s", lineStr);
    }

    switch(pDhcpsMapCfgData->pool[index].hostAddrtype)
    {
    case L7_DHCPS_IEEE_802:
      sysapiPrintf("\nhostAddrtype......................L7_DHCPS_IEEE_802");
      break;
    case L7_DHCPS_ETHERNET:
      sysapiPrintf("\nhostAddrtype......................L7_DHCPS_ETHERNET");
      break;
    default:
      sysapiPrintf("\nhostAddrtype......................UNKNOWN (%d)",
                   pDhcpsMapCfgData->pool[index].hostAddrtype);
      break;
    }
    sysapiPrintf("\nhostAddrLength....................%d", (L7_uint32)pDhcpsMapCfgData->pool[index].hostAddrLength);
    sysapiPrintf("\nhostAddr..........................");
    bytesDone = 0;
    while(bytesDone < L7_DHCPS_HARDWARE_ADDR_MAXLEN)
    {
      bytesDone += dhcpsBin2Hex(&(pDhcpsMapCfgData->pool[index].hostAddr[bytesDone]), /* start here */
                                L7_DHCPS_HARDWARE_ADDR_MAXLEN - bytesDone, /* bytes left */
                                sizeof(lineStr),  /* output buffer size */
                                lineStr);         /* output buffer */

      sysapiPrintf("\n %s", lineStr);
    }

    sysapiPrintf("\ndomainName........................%s", pDhcpsMapCfgData->pool[index].domainName);
    sysapiPrintf("\nclientBootFile....................%s", pDhcpsMapCfgData->pool[index].clientBootFile);

    osapiInetNtoa(pDhcpsMapCfgData->pool[index].nextServer, buf);
    sysapiPrintf("\nnextServer........................%s", buf);

    for(i = 0; i < L7_DHCPS_NETBIOS_NAME_SERVER_MAX; i++)
    {
      if ((showAll == 0) && (pDhcpsMapCfgData->pool[index].netBiosNameServer[i] == 0))
      {
        continue;
      }

      osapiInetNtoa(pDhcpsMapCfgData->pool[index].netBiosNameServer[i], buf);
      sysapiPrintf("\nnetBiosNameServer[%d]..............%s", i, buf);
    }

    sysapiPrintf("\nnetBiosNodeType...................%d", pDhcpsMapCfgData->pool[index].netBiosNodeType);

    sysapiPrintf("\noptions...........................");
    bytesDone = 0;
    while(bytesDone < L7_DHCPS_POOL_OPTION_BUFFER)
    {
      bytesDone += dhcpsBin2Hex(&(pDhcpsMapCfgData->pool[index].options[bytesDone]), /* start here */
                                L7_DHCPS_POOL_OPTION_BUFFER - bytesDone, /* bytes left */
                                sizeof(lineStr),  /* output buffer size */
                                lineStr);         /* output buffer */

      sysapiPrintf("\n %s", lineStr);
    }
  }

  /* DHCP Server excluded addresses cfg */
  for(i = 0; i < L7_DHCPS_MAX_EXCLUDE_NUMBER; i++)
  {
    if ((showAll == 0) && (pDhcpsMapCfgData->excludedAddrs[i].fromAddr == 0) &&
        (pDhcpsMapCfgData->excludedAddrs[i].toAddr == 0))
    {
      continue;
    }

    sysapiPrintf("\nexcludedAddrs #%d", i);
    osapiInetNtoa(pDhcpsMapCfgData->excludedAddrs[i].fromAddr, buf);
    sysapiPrintf("\nfrom..............................%s", buf);
    osapiInetNtoa(pDhcpsMapCfgData->excludedAddrs[i].toAddr, buf);
    sysapiPrintf("\nto................................%s", buf);
  }
}

/*********************************************************************
*
* @purpose  Dump the contents of the lease config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void dhcpsLeaseConfigDataTestShow(int showAll)
{
    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------------*/
    /* Lease Config File Header Contents */
    /*-----------------------------------*/
    sysapiPrintf( "DHCPS MAP LEASE CONFIG DATA\n");
    sysapiPrintf( "---------------------------\n");

    pFileHdr = &(pDhcpsMapLeaseCfgData->cfgHdr);
    sysapiCfgFileHeaderDump (pFileHdr);

   /*--------------------------------------*/
   /* Lease cfgParms                       */
   /*--------------------------------------*/

    dhcpsLeaseCfgDataShow(showAll);


    /*-------------------------------*/
    /* Scaling Constants             */
    /*                               */
    /*                               */
    /* NOTE:                         */
    /* Print any limits of arrays    */
    /* or anything else in the       */
    /* confi files which might       */
    /* affect the size of the config */
    /* file.                         */
    /*                               */
    /*-------------------------------*/

    sysapiPrintf( "\n\n");
    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");

    sysapiPrintf( "L7_DHCPS_MAX_LEASE_NUMBER        - %d\n", L7_DHCPS_MAX_LEASE_NUMBER);
    sysapiPrintf( "L7_DHCPS_POOL_NAME_MAXLEN        - %d\n", L7_DHCPS_POOL_NAME_MAXLEN);
    sysapiPrintf( "L7_DHCPS_CLIENT_ID_MAXLEN        - %d\n", L7_DHCPS_CLIENT_ID_MAXLEN);
    sysapiPrintf( "L7_DHCPS_HARDWARE_ADDR_MAXLEN    - %d\n", L7_DHCPS_HARDWARE_ADDR_MAXLEN);

    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("\n\npDhcpsMapLeaseCfgData->checkSum : %u\n", pDhcpsMapLeaseCfgData->checkSum);
}

void dhcpsLeaseCfgDataShow(int showAll)
{
  leaseCfgData_t *pLease;
  L7_uint32 index, bytesDone;
  L7_char8 buf[80];
  L7_char8 lineStr[40];
  L7_uint32 dhcpsOsapiUpTime, dhcpsSystemUpTime;

  sysapiPrintf("\n\n");
  sysapiPrintf("\n+---------------------------------------------------------------+");
  sysapiPrintf("\n|                 DHCPS LEASE CONFIGURATION DATA                |");
  sysapiPrintf("\n+---------------------------------------------------------------+");

  /* generic DHCP Server cfg */

  dhcpsSystemUpTime = simSystemUpTimeGet();
  dhcpsOsapiUpTime = osapiUpTimeRaw();

  sysapiPrintf("\nsimSystemUpTimeGet.....................%d", dhcpsSystemUpTime);

  sysapiPrintf("\nosapiUpTimeRaw.........................%d", dhcpsOsapiUpTime);

  sysapiPrintf("\nsystemUpTimeDelta......................%d", dhcpsSystemUpTime - dhcpsOsapiUpTime);

  pLease = &(pDhcpsMapLeaseCfgData->lease[0]);
  for( index = 0; index < L7_DHCPS_MAX_LEASE_NUMBER; index++)
  {
    if ((showAll == 0) &&
        ((pDhcpsMapLeaseCfgData->lease[index].state == FREE_LEASE) ||
        (*pDhcpsMapLeaseCfgData->lease[index].poolName == L7_NULL)))
    {
        pLease++;
        continue;
    }

    sysapiPrintf("\n-----------------------------------------------------------");
    sysapiPrintf("\nLease #%d", index);
    sysapiPrintf("\n-----------------------------------------------------------");

    sysapiPrintf("\npoolName...............................%s", pLease->poolName);

    osapiInetNtoa(pLease->ipAddr, buf);
    sysapiPrintf("\nipAddr.................................%s", buf);

    osapiInetNtoa(pLease->ipMask, buf);
    sysapiPrintf("\nipMask.................................%s", buf);

    sysapiPrintf("\nleaseTime..............................%d", pLease->leaseTime);

    sysapiPrintf("\nleaseExpiration........................%d", pLease->leaseExpiration);

    switch(pLease->state)
    {
    case FREE_LEASE:
      sysapiPrintf("\nstate..................................FREE_LEASE");
      break;
    case ACTIVE_LEASE:
      sysapiPrintf("\nstate..................................ACTIVE_LEASE");
      break;
    case OFFERED_LEASE:
      sysapiPrintf("\nstate..................................OFFERED_LEASE");
      break;
    case EXPIRED_LEASE:
      sysapiPrintf("\nstate..................................EXPIRED_LEASE");
      break;
    case ABANDONED_LEASE:
      sysapiPrintf("\nstate..................................ABANDONED_LEASE");
      break;
    default:
      sysapiPrintf("\nstate..................................UNKOWN(%d)",
                   (L7_uint32)pLease->state);
      break;
    }

    sysapiPrintf("\nIsBootp................................%s (%d)",
                 ((pLease->IsBootp == L7_TRUE) ? "L7_TRUE" : "L7_FALSE"),
                 (L7_uint32)pLease->IsBootp);

    switch(pLease->conflictDetectionMethod)
    {
    case L7_DHCPS_PING:
      sysapiPrintf("\nstate..................................L7_DHCPS_PING");
      break;
    case L7_DHCPS_GRATUITOUS_ARP:
      sysapiPrintf("\nstate..................................L7_DHCPS_GRATUITOUS_ARP");
      break;
    default:
      sysapiPrintf("\nstate..................................UNKOWN(%d)",
                   (L7_uint32)pLease->conflictDetectionMethod);
      break;
    }

    sysapiPrintf("\nclientIdLength.........................%d", (L7_uint32)pLease->clientIdLength);
    sysapiPrintf("\nclientIdentifier.......................");
    bytesDone = 0;
    while(bytesDone < L7_DHCPS_CLIENT_ID_MAXLEN)
    {
      bytesDone += dhcpsBin2Hex(&(pLease->clientIdentifier[bytesDone]), /* start here */
                                L7_DHCPS_CLIENT_ID_MAXLEN - bytesDone, /* bytes left */
                                sizeof(lineStr),  /* output buffer size */
                                lineStr);         /* output buffer */

      sysapiPrintf("\n %s", lineStr);
    }

    switch(pLease->hAddrtype)
    {
    case L7_DHCPS_IEEE_802:
      sysapiPrintf("\nhostAddrtype...........................L7_DHCPS_IEEE_802");
      break;
    case L7_DHCPS_ETHERNET:
      sysapiPrintf("\nhostAddrtype...........................L7_DHCPS_ETHERNET");
      break;
    default:
      sysapiPrintf("\nhostAddrtype...........................UNKNOWN (%d)", \
                   (L7_uint32)pLease->hAddrtype);
      break;
    }
    sysapiPrintf("\nhwAddrLength...........................%d", (L7_uint32)pLease->hwAddrLength);
    sysapiPrintf("\nhwAddr.................................");
    bytesDone = 0;
    while(bytesDone < L7_DHCPS_HARDWARE_ADDR_MAXLEN)
    {
      bytesDone += dhcpsBin2Hex(&(pLease->hwAddr[bytesDone]), /* start here */
                                L7_DHCPS_HARDWARE_ADDR_MAXLEN - bytesDone, /* bytes left */
                                sizeof(lineStr),  /* output buffer size */
                                lineStr);         /* output buffer */

      sysapiPrintf("\n %s", lineStr);
    }

    /* next lease */
    pLease++;
  }
}
