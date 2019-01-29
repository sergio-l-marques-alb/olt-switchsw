/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ipv6_provisioning_config.c
*
* @purpose Contains definitions to support the ipv6 provisioning configurations
*
*
* @component ipv6_provisioning
*
* @comments
*
* @create 09/02/2003
*
* @author nshrivastav
* @end
*
**********************************************************************/


#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "nimapi.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "log.h"
#include "dtlapi.h"
#include "ipv6_provisioning_config.h"
#include "ipv6_provisioning_api.h"
#include "platform_config.h"


ipv6ProvCfgData_t  *ipv6ProvCfgData;
L7_uint32 *ipv6ProvMapTbl = L7_NULLPTR;
L7_uint32 numIPv6Pkts = 0;
extern ipv6ProvCnfgrState_t ipv6ProvCnfgrState;


static void ipv6ProvBuildTestIntfConfigData(ipv6PortInfo_t *pCfg, L7_uint32 seed);

/*********************************************************************
* @purpose  Build default ipv6 config data
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipv6ProvBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId[L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT];

  /* Save the config IDs */
  memset((void *)&configId[0], 0, sizeof(nimConfigID_t) * L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT);
  for (cfgIndex = 1; cfgIndex < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &ipv6ProvCfgData->ipv6PortMappings[cfgIndex].configId);

  memset((void *)ipv6ProvCfgData, 0, sizeof(ipv6ProvCfgData_t));

  for (cfgIndex = 1; cfgIndex < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; cfgIndex++)
    ipv6ProvBuildDefaultIntfConfigData(&configId[cfgIndex], &ipv6ProvCfgData->ipv6PortMappings[cfgIndex]);

  strcpy((L7_char8 *)ipv6ProvCfgData->cfgHdr.filename, IPV6_PROVISIONING_CFG_FILENAME);

  ipv6ProvCfgData->cfgHdr.version = ver;

  ipv6ProvCfgData->cfgHdr.componentID = L7_IPV6_PROVISIONING_COMPONENT_ID;

  ipv6ProvCfgData->cfgHdr.type = L7_CFG_DATA;

  ipv6ProvCfgData->cfgHdr.length = (L7_uint32)sizeof(ipv6ProvCfgData_t);

  ipv6ProvCfgData->cfgHdr.dataChanged = L7_FALSE;

  return;
}

/*********************************************************************
* @purpose  Build default IPV6 Provisioning interface config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void ipv6ProvBuildDefaultIntfConfigData(nimConfigID_t *configId, ipv6PortInfo_t *pCfg)
{
  nimConfigID_t configIdNull;

  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  pCfg->ipv6Mode = FD_IPV6_PROVISIONING_DEFAULT_MODE;
  pCfg->ipv6copyToCpu = FD_IPV6_PROVISIONING_DEFAULT_MODE;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));
  NIM_CONFIG_ID_COPY(&pCfg->ipv6DstId, &configIdNull);

  return;
}


/*********************************************************************
* @purpose  Applies ipv6 config data
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipv6ProvApplyConfigData(void)
{
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;

  /* Now check for ports that are valid for IPv6 provisioning.  All non-zero values in the
   * config structure contain a port config ID that is valid for IPv6 provisioning.
   * Convert this to internal interface number and apply the port config.
   * The internal interface number is used as an index into the operational
   * interface data.
   */
  if (IPV6_PROVISIONING_IS_READY)
  {
    memset(&configIdNull, 0, sizeof(nimConfigID_t));

    for (cfgIndex = 1; cfgIndex < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex].configId,
                                 &configIdNull))
        continue;
      if (nimIntIfFromConfigIDGet(&(ipv6ProvCfgData->ipv6PortMappings[cfgIndex].configId),
                                  &intIfNum) != L7_SUCCESS)
        continue;
      if (ipv6ProvApplyIntfConfigData(intIfNum) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IPV6_PROVISIONING_COMPONENT_ID,
                "ipv6ProvApplyConfigData: Failure in applying config data for interface %s\n", ifName);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies the configured saved configuration for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ipv6ProvApplyIntfConfigData(L7_uint32 intIfNum)
{
  ipv6PortInfo_t *pCfg;
  L7_uint32 srcIfNum, dstIfNum, cfgIndex;
  nimConfigID_t configIdNull;

  /* First check to see if this interface is a valid src interface */
  if (ipv6ProvIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (nimIntIfFromConfigIDGet(&(pCfg->ipv6DstId), &dstIfNum) == L7_SUCCESS)
    {
      (void)ipv6ProvBridgeSrcIfNumDstIfNum(intIfNum, dstIfNum, pCfg->ipv6Mode, pCfg->ipv6copyToCpu);
    }
  }

  /* Now check to see if this interface is configured as a destination interface */
  memset(&configIdNull, 0, sizeof(nimConfigID_t));
  for (cfgIndex = 1; cfgIndex < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; cfgIndex++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex].configId,
                               &configIdNull))
      continue;

    if (NIM_CONFIG_ID_IS_EQUAL(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6DstId,
                               &configIdNull))
      continue;

    if (nimIntIfFromConfigIDGet(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6DstId,
                                &dstIfNum) != L7_SUCCESS)
      continue;

    if (intIfNum == dstIfNum)
    {
      if (nimIntIfFromConfigIDGet(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex].configId,
                                  &srcIfNum) == L7_SUCCESS)
      {
        (void)ipv6ProvBridgeSrcIfNumDstIfNum(srcIfNum, dstIfNum,
                                             ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6Mode,
                                             ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6copyToCpu);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if ipv6 user config data has changed
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
L7_BOOL ipv6ProvHasDataChanged(void)
{
  return ipv6ProvCfgData->cfgHdr.dataChanged;
}
void ipv6ProvResetDataChanged(void)
{
  ipv6ProvCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Saves ipv6 user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipv6ProvSave(void)
{

  L7_uint32 rc = L7_SUCCESS;

  if (ipv6ProvCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    ipv6ProvCfgData->cfgHdr.dataChanged = L7_FALSE;
    ipv6ProvCfgData->checkSum = nvStoreCrc32((L7_char8 *)ipv6ProvCfgData,
                                        sizeof(ipv6ProvCfgData_t) - sizeof(ipv6ProvCfgData->checkSum));

    if ( (rc = sysapiCfgFileWrite(L7_IPV6_PROVISIONING_COMPONENT_ID, IPV6_PROVISIONING_CFG_FILENAME,
                                  (L7_char8 *)ipv6ProvCfgData,
                                  (L7_int32)sizeof(ipv6ProvCfgData_t))) != L7_SUCCESS )
    {
      rc = L7_ERROR;
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IPV6_PROVISIONING_COMPONENT_ID,
              "ipv6ProvSave: error on call to sysapiCfgFileWrite routine on config file %s\n",
              IPV6_PROVISIONING_CFG_FILENAME);
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose Receives IPv6 packet from the DTL and passes it to User Application
*
*
* @param    bufHandle    @b{(input)} handle to the network buffer the IPv6
*                                    packet is stored in
* @param    pduInfo      @b{(input)} pointer to sysnet structure which contains
*                                    the internal  interface number that the
*                                    IPv6 packet was received on
*
*
* @returns  L7_SUCCESS
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipv6ProvRecvPkt(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  numIPv6Pkts++;
  SYSAPI_NET_MBUF_FREE(bufHandle);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  NIM interface change callback processing
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    intfEvent    @b{(input)} Event to be processes
* @param    correlator   @b{(input)} NIM correlator for the event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ipv6ProvIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 intfEvent, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_FAILURE;
  NIM_EVENT_COMPLETE_INFO_t status;
  ipv6PortInfo_t *pCfg;

  status.intIfNum     = intIfNum;
  status.component    = L7_IPV6_PROVISIONING_COMPONENT_ID;
  status.event        = intfEvent;
  status.correlator   = correlator;

  if (!(IPV6_PROVISIONING_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IPV6_PROVISIONING_COMPONENT_ID,
            "ipv6ProvIntfChangeCallback: Received an interface change callback while not ready to receive it");
    rc = L7_FAILURE;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if ((intfEvent != L7_CREATE) && (intfEvent != L7_DELETE))
  {
    /*check to see if IPv6 Provisioning is configurable for this interface*/
    if (ipv6ProvIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    {
      rc = L7_SUCCESS;
      status.response.rc = rc;
      nimEventStatusCallback(status);
      return rc;
    }
  }

  if (ipv6ProvIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  switch (intfEvent)
  {
  case L7_CREATE:
    rc = ipv6ProvIntfCreate(intIfNum);
    break;

  case L7_ATTACH:
    rc = ipv6ProvApplyIntfConfigData(intIfNum);
    break;

  case L7_DETACH:
    rc = ipv6ProvIntfDetach(intIfNum);
    break;

  case L7_DELETE:
    rc = ipv6ProvIntfDelete(intIfNum);
    break;

    default:
      rc = L7_SUCCESS;

  }

  status.response.rc = rc;
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to ipv6Prov port config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL ipv6ProvIntfIsConfigurable(L7_uint32 intIfNum, ipv6PortInfo_t **pCfg)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId;

  if (!(IPV6_PROVISIONING_IS_READY))
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  cfgIndex = ipv6ProvMapTbl[intIfNum];

  if (cfgIndex == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId,
                               &(ipv6ProvCfgData->ipv6PortMappings[cfgIndex].configId)) == L7_FALSE)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* if we get here, either we have a table management error between ipv6ProvCfgData and ipv6ProvMapTbl
      ** or there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_IPV6_PROVISIONING_COMPONENT_ID,
              "ipv6ProvIntfIsConfigurable: Error accessing IPv6 Provisioning config data for interface %s."
              " A default configuration does not exist for this interface. Typically a case when a new "
              "interface is created and has no pre-configuration.", ifName);
      return L7_FALSE;
    }
  }

  *pCfg = &ipv6ProvCfgData->ipv6PortMappings[cfgIndex];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to IPv6 Provisioning port config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @end
*********************************************************************/
L7_BOOL ipv6ProvIntfConfigEntryGet(L7_uint32 intIfNum, ipv6PortInfo_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  L7_RC_t rc;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (!(IPV6_PROVISIONING_IS_READY))
    return L7_FALSE;

  if ((rc = nimConfigIdGet(intIfNum, &configId)) == L7_SUCCESS)
  {
    for (i = 1; i < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&ipv6ProvCfgData->ipv6PortMappings[i].configId, &configIdNull))
      {
        ipv6ProvMapTbl[intIfNum] = i;
        *pCfg = &ipv6ProvCfgData->ipv6PortMappings[i];
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t ipv6ProvIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  ipv6PortInfo_t *pCfg;
  L7_uint32 i;

  if (ipv6ProvIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&ipv6ProvCfgData->ipv6PortMappings[i].configId, &configId))
    {
      ipv6ProvMapTbl[intIfNum] = i;
      break;
    }
  }

  pCfg = L7_NULL;

  /* If an interface configuration entry is not already assigned to the interface, assign one */
  if (ipv6ProvIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if (ipv6ProvIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id */
    if (pCfg != L7_NULL)
      ipv6ProvBuildDefaultIntfConfigData(&configId, pCfg);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t ipv6ProvIntfDetach(L7_uint32 intIfNum)
{
  ipv6PortInfo_t *pCfg;
  L7_uint32 srcIfNum, dstIfNum, cfgIndex;
  nimConfigID_t configIdNull;

  /* First check to see if this interface has been enabled as a source interface.
   * If so, disable it.
   */
  if (ipv6ProvIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (pCfg->ipv6Mode == L7_ENABLE)
    {
      if (nimIntIfFromConfigIDGet(&(pCfg->ipv6DstId), &dstIfNum) == L7_SUCCESS)
        (void)ipv6ProvBridgeSrcIfNumDstIfNum(intIfNum, dstIfNum, L7_DISABLE, pCfg->ipv6copyToCpu);
    }

    ipv6ProvBuildDefaultIntfConfigData(&pCfg->configId, pCfg);
    (void)ipv6ProvApplyIntfConfigData(intIfNum);
  }

  /* Now check to see if this interface is configured as a destination interface.
   * If so, disable it.
   */
  memset(&configIdNull, 0, sizeof(nimConfigID_t));
  for (cfgIndex = 1; cfgIndex < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; cfgIndex++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex].configId,
                               &configIdNull))
      continue;

    if (NIM_CONFIG_ID_IS_EQUAL(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6DstId,
                               &configIdNull))
      continue;

    if (nimIntIfFromConfigIDGet(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6DstId,
                                &dstIfNum) != L7_SUCCESS)
      continue;

    if (intIfNum == dstIfNum)
    {
      if (nimIntIfFromConfigIDGet(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex].configId,
                                  &srcIfNum) == L7_SUCCESS)
      {
        (void)ipv6ProvBridgeSrcIfNumDstIfNum(srcIfNum, dstIfNum, L7_DISABLE,
                                             ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6copyToCpu);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t ipv6ProvIntfDelete(L7_uint32 intIfNum)
{
  ipv6PortInfo_t *pCfg;

  if (ipv6ProvIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset((void *)&ipv6ProvMapTbl[intIfNum], 0, sizeof(L7_uint32));
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current IPv6 Provisioning config values
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipv6ProvCfgDump(void)
{
  L7_char8 buf[32];
  nimUSP_t srcUsp, dstUsp;
  L7_uint32 i, intIfNum;
  ipv6PortInfo_t *pCfg;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));
  memset(&srcUsp, 0, sizeof(nimUSP_t));
  memset(&dstUsp, 0, sizeof(nimUSP_t));

  printf("\n");
  printf("IPv6 Provisioning\n");
  printf("=================\n");

  for (i = 1; i < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&ipv6ProvCfgData->ipv6PortMappings[i].configId, &configIdNull))
      continue;
    if (nimIntIfFromConfigIDGet(&(ipv6ProvCfgData->ipv6PortMappings[i].configId), &intIfNum) != L7_SUCCESS)
      continue;
    if (ipv6ProvIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if (nimUspFromConfigIDGet(&(ipv6ProvCfgData->ipv6PortMappings[i].configId), &srcUsp) != L7_SUCCESS)
        continue;

      if (nimUspFromConfigIDGet(&(ipv6ProvCfgData->ipv6PortMappings[i].ipv6DstId), &dstUsp) != L7_SUCCESS)
      {
        dstUsp.unit = 0;
        dstUsp.slot = 0;
        dstUsp.port = 0;
      }

      sprintf(buf, "%u/%u/%u", srcUsp.unit, srcUsp.slot, srcUsp.port);
      printf("SrcUSP   : %s\n", buf);

      sprintf(buf, "%s", pCfg->ipv6Mode == L7_ENABLE ? "Enabled" : "Disabled");
      printf("IPv6Mode : %s\n", buf);

      sprintf(buf, "%u/%u/%u", dstUsp.unit, dstUsp.slot, dstUsp.port);
      printf("DstUSP   : %s\n", buf);

      sprintf(buf, "%s", pCfg->ipv6copyToCpu == L7_ENABLE ? "Enabled" : "Disabled");
      printf("CopyToCPU: %s\n", buf);

      printf("\n");
    }
  }

  printf("=================\n");

  return L7_SUCCESS;
}





/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



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
void ipv6ProvBuildTestConfigData(void)
{
    L7_uint32 cfgIndex;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

   for (cfgIndex = 1; cfgIndex < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; cfgIndex++)
     ipv6ProvBuildTestIntfConfigData(&ipv6ProvCfgData->ipv6PortMappings[cfgIndex], cfgIndex);



 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   ipv6ProvCfgData->cfgHdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}



/*********************************************************************
* @purpose  Build test config data for an intf
*
* @param    configId     configID associated with an interface
* @param    *pCfg        pointer to interface configuration structure
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipv6ProvBuildTestIntfConfigData(ipv6PortInfo_t *pCfg, L7_uint32 seed)
{
  nimConfigID_t configIdTest;
  L7_uint32     lowest;

  pCfg->ipv6Mode = L7_ENABLE;
  pCfg->ipv6copyToCpu = L7_ENABLE;

  memset(&configIdTest, 0, sizeof(nimConfigID_t));

  /*  Get lowest interface number to save */

  if (nimNextValidIntfNumber(0, &lowest) == L7_SUCCESS)
  {
      if ( nimConfigIdGet( (1 + seed), &configIdTest) == L7_SUCCESS)
      {
          NIM_CONFIG_ID_COPY(&pCfg->ipv6DstId, &configIdTest);
      }

  }

  return;
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
void ipv6ProvConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(ipv6ProvCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    ipv6ProvCfgDump();


    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT - %d\n",
                  L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("ipv6ProvCfgData->checkSum : %u\n", ipv6ProvCfgData->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/



