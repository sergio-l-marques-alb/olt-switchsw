/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_api.c
*
* @purpose   ISDP API file
*
* @component isdp
*
* @comments
*
* @create    28/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/

#include "isdp_include.h"
#include "isdp_api.h"

extern isdpCfg_t      *isdpCfg;
extern void           *isdpSemaphore;
extern isdpStats_t    *isdpStats;

/*********************************************************************
 *                isdp Configuration APIs
 *********************************************************************/

/*********************************************************************
* @purpose  Given a deviceId, get the next one,
*           if it exists
*
* @param    intIfNum       @b((input))  Interface number
* @param    *deviceId      @b((input))  deviceId, start
* @param    *nextIntIfNum  @b((output))  Next interface number
* @param    *nextDeviceId  @b((output))  deviceId, next
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
* @notes   if the start is NULL_PTR, the first will be returned
*
* @end
*********************************************************************/
static L7_RC_t isdpNeighborGetNextImpl(L7_uint32 intIfNum, L7_uchar8 *deviceId,
                                   L7_uint32 *nextIntIfNum, L7_uchar8 *nextDeviceId)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  if(deviceId != L7_NULLPTR)
  {
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);
  }

  if(intIfNum != 0)
  {
    if (isdpIsValidIntf(intIfNum) == L7_TRUE)
    {
      key.intIfNum = intIfNum;
    }
    else
    {
      isdpDebugTrace(ISDP_DBG_FLAG_PduGroup,
                     "%s: LINE %d: interface %d isn't valid \n",
                   __FUNCTION__, __LINE__, intIfNum);
      rc = L7_FAILURE;
    }
  }

  if(rc == L7_SUCCESS)
  {
    pEntry = isdpEntryGetNext(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no more neighbors\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      *nextIntIfNum = pEntry->key.intIfNum;
      memset(nextDeviceId, 0, L7_ISDP_DEVICE_ID_LEN);
      osapiStrncpySafe(nextDeviceId, pEntry->key.deviceId, L7_ISDP_DEVICE_ID_LEN);
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Sets the isdp global mode
*
* @param    mode  @b((input)) Admin mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (mode != L7_ENABLE && mode != L7_DISABLE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Unsupported mode %d\n", mode);
    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Configuration isn't initialized\n");
    rc = L7_FAILURE;
  }
  else
  {
    if (isdpCfg->cfg.globalCfgData.mode != mode)
    {
      isdpCfg->cfg.globalCfgData.mode = mode;
      isdpCfg->hdr.dataChanged = L7_TRUE;
      rc = isdpModeApply(mode);
      if(rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
                "isdpModeApply failed\n");
      }
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the isdp global mode
*
* @param    *mode @b((output)) Admin mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpModeGet(L7_uint32 *mode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (mode == L7_NULLPTR)
  {
    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    rc = L7_FAILURE;
  }
  else
  {
    *mode = isdpCfg->cfg.globalCfgData.mode;
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Sets the isdp global timer
*
* @param    timer  @b((input)) timer in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpTimerSet(L7_uint32 timer)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (timer < L7_ISDP_TIMER_MIN ||
      timer > L7_ISDP_TIMER_MAX)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Unsupported timer %d\n", timer);
    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Configuration isn't initialized\n");
    rc = L7_FAILURE;
  }
  else if (isdpCfg->cfg.globalCfgData.timer != timer)
  {
    isdpCfg->cfg.globalCfgData.timer = timer;
    isdpCfg->hdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the isdp global timer
*
* @param    *timer @b((output)) timer in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpTimerGet(L7_uint32 *timer)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (timer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Parameter is equal to NULL\n");
    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Configuration isn't initialized\n");
    rc = L7_FAILURE;
  }
  else
  {
    *timer = isdpCfg->cfg.globalCfgData.timer;
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Sets the isdp global holdtime
*
* @param    holdtime  @b((input)) holdtime in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpHoldTimeSet(L7_uint32 holdTime)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (holdTime < L7_ISDP_HOLDTIME_MIN ||
      holdTime > L7_ISDP_HOLDTIME_MAX)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Unsupported holdTime %d\n", holdTime);
    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Configuration isn't initialized\n");
    rc = L7_FAILURE;
  }
  else if (isdpCfg->cfg.globalCfgData.holdTime != holdTime)
  {
    isdpCfg->cfg.globalCfgData.holdTime = holdTime;
    isdpCfg->hdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the isdp global holdtime
*
* @param    *holdtime @b((output)) holdtime in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpHoldTimeGet(L7_uint32 *holdTime)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (holdTime == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Parameter is equal to NULL\n");
    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Configuration isn't initialized\n");
    rc = L7_FAILURE;
  }
  else
  {
    *holdTime = isdpCfg->cfg.globalCfgData.holdTime;
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Sets the isdp global v2Mode
*
* @param    v2Mode  @b((input)) V2 advertisement mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpV2ModeSet(L7_uint32 v2Mode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (v2Mode != L7_ENABLE && v2Mode != L7_DISABLE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Unsupported v2Mode %d\n", v2Mode);
    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Configuration isn't initialized\n");
    rc = L7_FAILURE;
  }
  else if (isdpCfg->cfg.globalCfgData.v2Mode != v2Mode)
  {
    isdpCfg->cfg.globalCfgData.v2Mode = v2Mode;
    isdpCfg->hdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the isdp global v2Mode
*
* @param    *v2Mode @b((output)) V2 advertisement mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpV2ModeGet(L7_uint32 *v2Mode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (v2Mode == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Parameter is equal to NULL\n");
    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Configuration isn't initialized\n");
    rc = L7_FAILURE;
  }
  else
  {
    *v2Mode = isdpCfg->cfg.globalCfgData.v2Mode;
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Sets the isdp mode for the specified interface
*
* @param    intIfNum  @b((input)) internal interface number
* @param    mode      @b((input)) isdp intf mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  isdpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_RC_t           rc     = L7_SUCCESS;
  L7_uchar8 logBuf[256];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (mode != L7_ENABLE && mode != L7_DISABLE)
  {
    rc = L7_FAILURE;
  }
  else if (isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    osapiSnprintf(logBuf, sizeof(logBuf),"Unsupported interface %s\n", ifName);
    L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_ISDP_COMPONENT_ID,logBuf);
    rc = L7_FAILURE;
  }
  else if (isdpIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: can't map interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    rc = L7_FAILURE;
  }
  else
  {
    if ((mode == L7_ENABLE && pCfg->mode != L7_ENABLE) ||
        (mode == L7_DISABLE && pCfg->mode != L7_DISABLE))
    {
      pCfg->mode = mode;
      isdpCfg->hdr.dataChanged = L7_TRUE;

      rc = isdpIntfModeApply(intIfNum, mode);
      if(rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR ,L7_ISDP_COMPONENT_ID,"isdpIntfModeApply failed\n");
      }
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the isdp mode for the specified interface
*
* @param    intIfNum  @b((input))  internal interface number
* @param    *mode     @b((output)) isdp intf mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  isdpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_RC_t           rc     = L7_SUCCESS;
  L7_uchar8 logBuf[256];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    osapiSnprintf(logBuf, sizeof(logBuf),"Unsupported interface %s\n", ifName);
    L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_ISDP_COMPONENT_ID,logBuf);
    rc = L7_FAILURE;
  }
  else if (isdpIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: can't map interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    rc = L7_FAILURE;
  }
  else if(isdpCfg == L7_NULLPTR)
  {
    rc = L7_FAILURE;
  }
  else
  {
    *mode = pCfg->mode;
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Clears all isdp traffic counters
*
* @param    none
*
* @returns  L7_SUCCESS, if data cleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpTrafficCountersClear(void)
{
  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  isdpStatsClear();

  osapiSemaGive(isdpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Clears all isdp neighbor entries
*
* @param    none
*
* @returns  L7_SUCCESS, if data cleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpNeighborTableClear(void)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  rc = isdpAllEntryDelete();

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
* @purpose  Get next neighbor on specific interface
*
*
* @param    intIfNum      @b((input))   Interface number
* @param    *deviceIdId     @b((input))   deviceId, start
* @param    *nextDeviceId @b((output))  deviceId, next
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
* @notes   if the start is NULL_PTR, the first will be returned
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborGetNext(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uchar8 *nextDeviceId)
{
  L7_uint32 nextIntIfNum = 0;
  L7_RC_t   rc           = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Unsupported interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    rc = isdpNeighborGetNextImpl(intIfNum, deviceId, &nextIntIfNum, nextDeviceId);

    if (nextIntIfNum != intIfNum)
    {
      rc = L7_FAILURE;
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
* @purpose  Get next neighbor with specific deviceId and intIfNum
*
*
* @param    intIfNum      @b((input))   Interface number, start
* @param    *deviceId      @b((input))   deviceId, start
* @param    *nextIntIfNum @b((output))  Interface number, next
* @param    *nextDeviceId  @b((output))  deviceId, next
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
* @notes   if the start is NULL_PTR, the first will be returned
*
* @end
*********************************************************************/
L7_RC_t isdpDeviceIdNeighborGetNext(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uint32 *nextIntIfNum,
    L7_uchar8 *nextDeviceId)
{
  L7_RC_t rc = L7_SUCCESS;
  isdpEntry_t * pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(deviceId != L7_NULLPTR)
  {
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);
  }

  if(intIfNum != 0)
  {
    if (isdpIsValidIntf(intIfNum) == L7_TRUE)
    {
      key.intIfNum = intIfNum;
    }
  }

  pEntry = isdpEntryDeviceIdFind(key, deviceId);

  if(pEntry == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: no more neighbors\n",
                   __FUNCTION__, __LINE__);

    rc = L7_FAILURE;
  }
  else
  {
    *nextIntIfNum = pEntry->key.intIfNum;
    memset(nextDeviceId, 0, L7_ISDP_DEVICE_ID_LEN);
    osapiStrncpySafe(nextDeviceId, pEntry->key.deviceId, L7_ISDP_DEVICE_ID_LEN);
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
* @purpose  Given a deviceId, get the next one,
*           if it exists
*
* @param    intIfNum       @b((input))  Interface number
* @param    *deviceId      @b((input))  deviceId, start
* @param    *nextIntIfNum  @b((output))  Next interface number
* @param    *nextDeviceId  @b((output))  deviceId, next
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
* @notes   if the start is NULL_PTR, the first will be returned
*
* @end
*********************************************************************/
L7_RC_t isdpNeighborGetNext(L7_uint32 intIfNum, L7_uchar8 *deviceId,
                            L7_uint32 *nextIntIfNum, L7_uchar8 *nextDeviceId)
{
  L7_RC_t        rc      = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  rc = isdpNeighborGetNextImpl(intIfNum, deviceId, nextIntIfNum, nextDeviceId);

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the platform of neighbor
*
* @param    intIfNum   @b((input))  Interface number
* @param    *deviceId   @b((input))  device identifier
* @param    *platform  @b((output)) Platform string
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborPlatformGet(L7_uint32 intIfNum, L7_uchar8 *deviceId,
    L7_uchar8 *platform)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;


  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceId == L7_NULLPTR ||
      platform == L7_NULLPTR ||
      isdpIsValidIntf(intIfNum) == L7_FALSE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Bad parameters, interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);

    pEntry = isdpEntryFind(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no entry found\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      osapiStrncpy(platform, pEntry->platform, L7_ISDP_PLATFORM_LEN);
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the version of neighbor
*
* @param    intIfNum   @b((input))  Interface number
* @param    *deviceId   @b((input))  device identifier
* @param    *version   @b((output)) Version string
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborVersionGet(L7_uint32 intIfNum, L7_uchar8 *deviceId,
    L7_uchar8 *version)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceId == L7_NULLPTR ||
      version == L7_NULLPTR ||
      isdpIsValidIntf(intIfNum) == L7_FALSE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Bad parameters, interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);

    pEntry = isdpEntryFind(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no entry found\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      osapiStrncpy(version, pEntry->version, L7_ISDP_VERSION_LEN);
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the portId of neighbor
*
* @param    intIfNum   @b((input))  Interface number
* @param    *deviceId   @b((input))  device identifier
* @param    *portId    @b((output)) Port identifier string
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborPortIdGet(L7_uint32 intIfNum, L7_uchar8 *deviceId,
    L7_uchar8 *portId)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceId == L7_NULLPTR ||
      portId == L7_NULLPTR ||
      isdpIsValidIntf(intIfNum) == L7_FALSE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Bad parameters, interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);

    pEntry = isdpEntryFind(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no entry found\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      osapiStrncpy(portId, pEntry->portId, L7_ISDP_PORT_ID_LEN);
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Get the next ip adress index of the neighbor
*
* @param    intIfNum     @b((input))  Interface number
* @param    *deviceId     @b((input))  device identifier
* @param    index       @b((input))  Previous index
* @param    *nextIndex   @b((output)) Next index
*
*
* @returns  L7_SUCCESS if there is next address
* @returns  L7_FAILURE if there is no next address
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborAddressGetNext(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uint32 index,
    L7_uint32 *nextIndex)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceId == L7_NULLPTR ||
      isdpIsValidIntf(intIfNum) == L7_FALSE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Bad parameters, interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);

    pEntry = isdpEntryFind(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no entry found\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      if(index < 0 || index + 1 > pEntry->addressNumber)
      {
        isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                       "%s: LINE %d: index is out of addresses range %d\n",
                       __FUNCTION__, __LINE__, index);

        rc = L7_FAILURE;
      }
      else
      {
        *nextIndex = index + 1;
      }
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the ip address of neighbor
*
* @param    intIfNum      @b((input))  Interface number
* @param    *deviceId      @b((input))  device identifier
* @param    index         @b((input))  ip address index
* @param    *ipAddress    @b((output)) ip address
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborIpAddressGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uint32 index,
    L7_uint32 *ipAddress)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (index < 1 ||
      deviceId == L7_NULLPTR ||
      ipAddress == L7_NULLPTR ||
      isdpIsValidIntf(intIfNum) == L7_FALSE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: wrong params\n",
                   __FUNCTION__, __LINE__);

    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);

    pEntry = isdpEntryFind(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no entry found\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      rc = isdpIpAddrListIndexGet(pEntry->ipAddressList, index, ipAddress);
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the capabilities of neighbor
*
* @param    intIfNum      @b((input))  Interface number
* @param    *deviceId      @b((input))  device identifier
* @param    *capabilities @b((output)) Device capabilities
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborCapabilitiesGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uint32 *capabilities)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceId == L7_NULLPTR ||
      capabilities == L7_NULLPTR ||
      isdpIsValidIntf(intIfNum) == L7_FALSE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Bad parameters, interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);

    pEntry = isdpEntryFind(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no entry found\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      *capabilities = pEntry->capabilities;
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the holdTime of neighbor
*
* @param    intIfNum   @b((input))  Interface number
* @param    *deviceId   @b((input))  device identifier
* @param    *holdTime  @b((output)) Holdtime
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborHoldTimeGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uchar8 *holdTime)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceId == L7_NULLPTR ||
      holdTime == L7_NULLPTR ||
      isdpIsValidIntf(intIfNum) == L7_FALSE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Bad parameters, interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);

    pEntry = isdpEntryFind(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no entry found\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      *holdTime = pEntry->holdTime;
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the protocol version of neighbor
*
* @param    intIfNum      @b((input))  Interface number
* @param    *device_id    @b((input))  Device identifier
* @param    *protoVersion @b((output)) ISDP protocol version
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborProtocolVersionGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uchar8 *protoVersion)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceId == L7_NULLPTR ||
      protoVersion == L7_NULLPTR ||
      isdpIsValidIntf(intIfNum) == L7_FALSE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Bad parameters, interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);

    pEntry = isdpEntryFind(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no entry found\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      *protoVersion = pEntry->protoVersion;
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the time when entry last changed
*
* @param    intIfNum        @b((input))  Interface number
* @param    *device_id      @b((input))  Device identifier
* @param    *lastChangeTime @b((output)) time added/modified
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpIntfNeighborLastChangeTimeGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uint32 *lastChangeTime)
{
  L7_RC_t        rc      = L7_SUCCESS;
  isdpEntry_t    *pEntry = L7_NULLPTR;
  isdpEntryKey_t key;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceId == L7_NULLPTR ||
      lastChangeTime == L7_NULLPTR ||
      isdpIsValidIntf(intIfNum) == L7_FALSE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Bad parameters, interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    osapiStrncpySafe(key.deviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);

    pEntry = isdpEntryFind(key);

    if(pEntry == L7_NULLPTR)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                     "%s: LINE %d: no entry found\n",
                     __FUNCTION__, __LINE__);

      rc = L7_FAILURE;
    }
    else
    {
      *lastChangeTime = pEntry->lastChangeTime;
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
* @purpose  Get the number of all isdp pdu received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficPduReceivedGet(void)
{
  L7_uint32 pduRx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    pduRx = isdpStats->globalStats.pduRx;
  }

  osapiSemaGive(isdpSemaphore);

  return pduRx;
}

/*********************************************************************
* @purpose  Get the number of v1 isdp pdu received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficV1PduReceivedGet(void)
{
  L7_uint32 v1PduRx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    v1PduRx = isdpStats->globalStats.v1PduRx;
  }

  osapiSemaGive(isdpSemaphore);

  return v1PduRx;
}

/*********************************************************************
* @purpose  Get the number of v2 isdp pdu received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficV2PduReceivedGet(void)
{
  L7_uint32 v2PduRx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    v2PduRx = isdpStats->globalStats.v2PduRx;
  }

  osapiSemaGive(isdpSemaphore);

  return v2PduRx;
}

/*********************************************************************
* @purpose  Get the number of isdp pdu with bad header received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficBadHeaderPduReceivedGet(void)
{
  L7_uint32 badHeaderRx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    badHeaderRx = isdpStats->globalStats.badHeaderRx;
  }

  osapiSemaGive(isdpSemaphore);

  return badHeaderRx;
}

/*********************************************************************
* @purpose  Get the number of isdp pdu with chksum error received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficChkSumErrorPduReceivedGet(void)
{
  L7_uint32 chkSumErrorRx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    chkSumErrorRx = isdpStats->globalStats.chkSumErrorRx;
  }

  osapiSemaGive(isdpSemaphore);

  return chkSumErrorRx;
}

/*********************************************************************
* @purpose  Get the number of isdp pdu in invalid format received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficInvalidFormatPduReceivedGet(void)
{
  L7_uint32 invalidFormatRx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    invalidFormatRx = isdpStats->globalStats.invalidFormatRx;
  }

  osapiSemaGive(isdpSemaphore);

  return invalidFormatRx;
}

/*********************************************************************
* @purpose  Get the number of all isdp pdu transmitted
*
*
* @returns the current value of transmitted pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficPduTransmitGet(void)
{
  L7_uint32 pduTx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    pduTx = isdpStats->globalStats.pduTx;
  }

  osapiSemaGive(isdpSemaphore);

  return pduTx;
}

/*********************************************************************
* @purpose  Get the number of v1 isdp pdu transmitted
*
*
* @returns the current value of transmitted pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficV1PduTransmitGet(void)
{
  L7_uint32 v1PduTx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    v1PduTx = isdpStats->globalStats.v1PduTx;
  }

  osapiSemaGive(isdpSemaphore);

  return v1PduTx;
}

/*********************************************************************
* @purpose  Get the number of v2 isdp pdu transmitted
*
*
* @returns the current value of transmitted pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficV2PduTransmitGet(void)
{
  L7_uint32 v2PduTx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    v2PduTx = isdpStats->globalStats.v2PduTx;
  }

  osapiSemaGive(isdpSemaphore);

  return v2PduTx;
}

/*********************************************************************
* @purpose  Get the number of isdp pdu transmition failures
*
*
* @returns the current value of transmitted pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficFailurePduTransmitGet(void)
{
  L7_uint32 failureTx = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    failureTx = isdpStats->globalStats.failureTx;
  }

  osapiSemaGive(isdpSemaphore);

  return failureTx;
}

/*********************************************************************
* @purpose  Get the number of isdp entry table was full
*
*
* @returns the number
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficTableFullGet(void)
{
  L7_uint32 insertEntryDrops = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    insertEntryDrops = isdpStats->insertEntryDrops;
  }

  osapiSemaGive(isdpSemaphore);

  return insertEntryDrops;
}

/*********************************************************************
* @purpose  Get the number of ip address table was full
*
*
* @returns the number
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpTrafficIpAddressTableFullGet(void)
{
  L7_uint32 insertIpAddressDrops = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    insertIpAddressDrops = isdpStats->insertIpAddressDrops;
  }

  osapiSemaGive(isdpSemaphore);

  return insertIpAddressDrops;
}

/*********************************************************************
* @purpose  Get the time when isdp entry table was modified last time
*
*
* @returns the number
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 isdpNeighborsTableLastChangeTimeGet(void)
{
  L7_uint32 lastChangeTime = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(isdpStats == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ApiGroup,
                   "%s: LINE %d: statistics isn't initialized\n",
                   __FUNCTION__, __LINE__);
  }
  else
  {
    lastChangeTime = isdpStats->lastChangeTime;
  }

  osapiSemaGive(isdpSemaphore);

  return lastChangeTime;
}

/*********************************************************************
*
* @purpose  Set the tracing mode for tx packets
*
* @param    txTraceMode  @b((input)) Trace mode for tx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTracePacketTxModeSet(L7_uint32 txTraceMode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (txTraceMode != L7_ENABLE && txTraceMode != L7_DISABLE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Unsupported txTraceMode %d\n", txTraceMode);
    rc = L7_FAILURE;
  }
  else if (isdpDebugPacketTraceTxFlagGet() != txTraceMode)
  {
    isdpDebugPacketTraceTxFlagSet(txTraceMode);
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Get the tracing mode for tx packets
*
* @param    *txTraceMode @b((output)) Trace mode for tx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTracePacketTxModeGet(L7_uint32 *txTraceMode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (txTraceMode == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "There is null pointer as param\n");
    rc = L7_FAILURE;
  }
  else
  {
    *txTraceMode = isdpDebugPacketTraceTxFlagGet();
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Set the tracing mode for rx packets
*
* @param    rxTraceMode  @b((input)) Trace mode for rx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTracePacketRxModeSet(L7_uint32 rxTraceMode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (rxTraceMode != L7_ENABLE && rxTraceMode != L7_DISABLE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Unsupported rxTraceMode %d\n", rxTraceMode);
    rc = L7_FAILURE;
  }
  else if (isdpDebugPacketTraceRxFlagGet() != rxTraceMode)
  {
    isdpDebugPacketTraceRxFlagSet(rxTraceMode);
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Get the tracing mode for tx packets
*
* @param    *rxTraceMode @b((output)) Trace mode for rx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTracePacketRxModeGet(L7_uint32 *rxTraceMode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (rxTraceMode == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "There is null pointer as param\n");
    rc = L7_FAILURE;
  }
  else
  {
    *rxTraceMode = isdpDebugPacketTraceRxFlagGet();
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Set the tracing mode for events
*
* @param    eventsTraceMode  @b((input)) Trace mode for events
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTraceEventsModeSet(L7_uint32 eventsTraceMode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (eventsTraceMode != L7_ENABLE && eventsTraceMode != L7_DISABLE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Unsupported eventsTraceMode %d\n", eventsTraceMode);
    rc = L7_FAILURE;
  }
  else if (isdpDebugEventTraceFlagGet() != eventsTraceMode)
  {
    isdpDebugEventTraceFlagSet(eventsTraceMode);
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Get the tracing mode for events
*
* @param    *eventsTraceMode @b((output)) Trace mode for events
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTraceEventsModeGet(L7_uint32 *eventsTraceMode)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (eventsTraceMode == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "There is null pointer as param\n");
    rc = L7_FAILURE;
  }
  else
  {
    *eventsTraceMode = isdpDebugEventTraceFlagGet();
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the isdp deviceId name format capability
*
* @param    *deviceIdFormatCpb @b((output)) DeviceId format Capability
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDeviceIdFormatCapabilityGet(L7_uchar8 *deviceIdFormatCpb)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceIdFormatCpb == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "There is null pointer as param\n");
    rc = L7_FAILURE;
  }
  else
  {
    *deviceIdFormatCpb = ISDP_DEVICE_ID_FORMAT_CAPABILITY_CURRENT_BITMASK;
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the isdp deviceId name format
*
* @param    *deviceIdFormatCpb @b((output)) DeviceId format
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDeviceIdFormatGet(L7_uint32 *deviceIdFormat)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceIdFormat == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "There is null pointer as param\n");
    rc = L7_FAILURE;
  }
  else
  {
    *deviceIdFormat = isdpDevIdFmtGet();
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}
/**********************************************************************
* @purpose    Translates input integer capability code to its char
*             equivalent
*
* @param      L7_uint32 cap
* @param      L7_char8 *capCh
* @param      L7_uint32 size
*
* @returntype void
*
* @end
*
*********************************************************************/
void isdpCapabilityCharGet(L7_uint32 cap, L7_char8 *capCh, L7_uint32 size)
{
  L7_uint32 n = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(cap & ISDP_CAP_ROUTER_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%c ",ISDP_CAP_ROUTER_ASCII);

  if(cap & ISDP_CAP_TRANS_BRIDGE_BITMASK)
    n += osapiSnprintf(&capCh[n], size -n, "%c ", ISDP_CAP_TRANS_BRIDGE_ASCII);

  if(cap & ISDP_CAP_SOURCE_ROUTE_BRIDGE_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%c ", ISDP_CAP_SOURCE_ROUTE_BRIDGE_ASCII);

  if(cap & ISDP_CAP_SWITCH_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%c ", ISDP_CAP_SWITCH_ASCII);

  if(cap & ISDP_CAP_HOST_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%c ", ISDP_CAP_HOST_ASCII);

  if(cap & ISDP_CAP_IGMP_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%c ", ISDP_CAP_IGMP_ASCII);

  if(cap & ISDP_CAP_REPEATER_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%c ", ISDP_CAP_REPEATER_ASCII);

  osapiSemaGive(isdpSemaphore);
}

/*********************************************************************
*
* @purpose    Translates input integer capability code to its
*             string equivalent
*
* @param      L7_uint32 cap
* @param      L7_char8 *capCh
* @param      L7_uint32 size
*
* @returntype void
*
* @end
*
*********************************************************************/
void isdpCapabilityStringGet(L7_uint32 cap, L7_char8 *capCh, L7_uint32 size)
{
  L7_uint32 n = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(cap & ISDP_CAP_ROUTER_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%s", ISDP_CAP_ROUTER_STRING);

  if(cap & ISDP_CAP_TRANS_BRIDGE_BITMASK)
    n += osapiSnprintf(&capCh[n], size -n, "%s", ISDP_CAP_TRANS_BRIDGE_STRING);

  if(cap & ISDP_CAP_SOURCE_ROUTE_BRIDGE_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%s",
                                        ISDP_CAP_SOURCE_ROUTE_BRIDGE_STRING);

  if(cap & ISDP_CAP_SWITCH_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%s", ISDP_CAP_SWITCH_STRING);

  if(cap & ISDP_CAP_HOST_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%s", ISDP_CAP_HOST_STRING);

  if(cap & ISDP_CAP_IGMP_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%s", ISDP_CAP_IGMP_STRING);

  if(cap & ISDP_CAP_REPEATER_BITMASK)
    n += osapiSnprintf(&capCh[n], size - n, "%s", ISDP_CAP_REPEATER_STRING);

  osapiSemaGive(isdpSemaphore);
}

/*********************************************************************
*
* @purpose    Translates input integer device id capability code
*             to its string equivalent
*
* @param      L7_uint32 cap
* @param      L7_char8 *capCh
* @param      L7_uint32 size
*
* @returntype void
*
* @end
*
*********************************************************************/
void isdpDevIdFormatCapabilityStringGet(L7_char8 cap, L7_char8 *capCh, L7_uint32 size)
{
  L7_uint32 n = 0;
  L7_uchar8 commaSpted = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if(cap & L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_SERIAL_BITMASK)
  {
    commaSpted = 1;
    n += osapiSnprintf(&capCh[n], size - n, "%s", ISDP_ID_FORMAT_CAP_SN_STRING);
  }

  if(cap & L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_MAC_BITMASK)
  {
    if (commaSpted)
    {
      n += osapiSnprintf(&capCh[n], size -n, ", %s", ISDP_ID_FORMAT_CAP_MA_STRING);
    }
    else
    {
      n += osapiSnprintf(&capCh[n], size -n, "%s", ISDP_ID_FORMAT_CAP_MA_STRING);
    }
    commaSpted = 1;
  }

  if(cap & L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_HOSTNAME_BITMASK)
  {
    if (commaSpted)
    {
      n += osapiSnprintf(&capCh[n], size -n, ", %s", ISDP_ID_FORMAT_CAP_HN_STRING);
    }
    else
    {
      n += osapiSnprintf(&capCh[n], size -n, "%s", ISDP_ID_FORMAT_CAP_HN_STRING);
    }
    commaSpted = 1;
  }

  if(cap & L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_OTHER_BITMASK)
  {
    if (commaSpted)
    {
      n += osapiSnprintf(&capCh[n], size - n, ", %s", ISDP_ID_FORMAT_CAP_OTHER_STRING);
    }
    else
    {
      n += osapiSnprintf(&capCh[n], size - n, "%s", ISDP_ID_FORMAT_CAP_OTHER_STRING);
    }
  }

  osapiSemaGive(isdpSemaphore);
}

/*********************************************************************
*
* @purpose    Translates input integer device id code
*             to its string equivalent
*
* @param      L7_uint32 cap
* @param      L7_char8 *capCh
* @param      L7_uint32 size
*
* @returntype void
*
* @end
*
*********************************************************************/
void isdpDevIdFormatStringGet(L7_uint32 cap, L7_char8 *capCh, L7_uint32 size)
{
  L7_uint32 n = 0;

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  switch (cap)
  {
    case ISDP_DEVICE_ID_SERIAL :
      n = osapiSnprintf(&capCh[n], size - n, "%s", ISDP_ID_FORMAT_CAP_SN_STRING);
      break;

    case ISDP_DEVICE_ID_MAC :
      n = osapiSnprintf(&capCh[n], size -n, "%s", ISDP_ID_FORMAT_CAP_MA_STRING);
      break;

    case ISDP_DEVICE_ID_HOST :
      n = osapiSnprintf(&capCh[n], size -n, "%s", ISDP_ID_FORMAT_CAP_HN_STRING);
      break;

    case ISDP_DEVICE_ID_OTHER :
      n = osapiSnprintf(&capCh[n], size - n, "%s", ISDP_ID_FORMAT_CAP_OTHER_STRING);
      break;

    default:
      break;
  }

  osapiSemaGive(isdpSemaphore);
}

/*********************************************************************
* @purpose  Given an index and interface, get the next neighbor,
*           if it exists
*
* @param    intIfNum        @b((input))   Interface number
* @param    index           @b((input))   index, start
* @param    *nextIntIfNum   @b((output))  Next interface number
* @param    *nextIndex      @b((output))  index, next
* @param    *deviceId       @b((output))  device identifier
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
*
* @end
*********************************************************************/
L7_RC_t isdpNeighborIndexedGetNext(L7_uint32 intIfNum, L7_uint32 index,
                                   L7_uint32 *nextIntIfNum, L7_uint32 *nextIndex,
                                   L7_char8 *deviceId)

{
  isdpEntryKey_t key;
  L7_RC_t        rc               = L7_FAILURE;
  isdpEntry_t    *pEntry          = L7_NULLPTR;
  L7_uint32      num_intf_matches = 1;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (nextIntIfNum == L7_NULLPTR ||
      nextIndex == L7_NULLPTR ||
      deviceId == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "There is null pointer as param\n");
    rc = L7_FAILURE;
  }
  else if(index < 1 || index > isdpDataCountEntryTree())
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Index is out of range %d\n", index);
    rc = L7_FAILURE;
  }
  else
  {
    if(intIfNum != 0)
    {
      if(isdpIsValidIntf(intIfNum) == L7_TRUE)
      {
        key.intIfNum = intIfNum;
      }
    }
    pEntry = isdpEntryFind(key);
    if (pEntry != L7_NULLPTR)
    {
      num_intf_matches++;
    }

    pEntry = isdpEntryGetNext(key);
    while (pEntry != L7_NULLPTR)
    {
      if(num_intf_matches >= index)
      {
        if(pEntry->key.intIfNum == intIfNum)
        {
          *nextIndex = index;
        }
        else
        {
          *nextIndex = 1;
        }

        *nextIntIfNum = pEntry->key.intIfNum;
        memset(deviceId, 0, L7_ISDP_DEVICE_ID_LEN);
        osapiStrncpySafe(deviceId, pEntry->key.deviceId, L7_ISDP_DEVICE_ID_LEN);
        rc = L7_SUCCESS;
        break;
      }

      num_intf_matches++;
      pEntry = isdpEntryGetNext(pEntry->key);
    }
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*********************************************************************
* @purpose  Given an index and interface, get the neighbor device id,
*           if it exists
*
* @param    intIfNum        @b((input))   Interface number
* @param    index           @b((input))   index, start
* @param    *deviceId       @b((output))  device identifier
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
*
* @end
*********************************************************************/
L7_RC_t isdpNeighborIndexedGet(L7_uint32 intIfNum, L7_uint32 index,
                               L7_char8 *deviceId)

{
  isdpEntryKey_t key;
  L7_RC_t        rc               = L7_FAILURE;
  isdpEntry_t    *pEntry          = L7_NULLPTR;
  L7_uint32      num_intf_matches = 1;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  osapiSemaTake(isdpSemaphore, L7_WAIT_FOREVER);

  if (deviceId == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "There is null pointer as param\n");
    rc = L7_FAILURE;
  }
  else if(index < 1 || index > isdpDataCountEntryTree())
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Index is out of range %d\n", index);
    rc = L7_FAILURE;
  }
  else if(isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Unsupported interface %s\n", ifName);
    rc = L7_FAILURE;
  }
  else
  {
    key.intIfNum = intIfNum;
    pEntry = isdpEntryFind(key);
    if (pEntry != L7_NULLPTR)
    {
      if(num_intf_matches >= index)
      {
        if (pEntry->key.intIfNum == intIfNum)
        {
          memset(deviceId, 0, L7_ISDP_DEVICE_ID_LEN);
          osapiStrncpySafe(deviceId, pEntry->key.deviceId, L7_ISDP_DEVICE_ID_LEN);
          rc = L7_SUCCESS;
          osapiSemaGive(isdpSemaphore);
          return rc;
        }
      }
      num_intf_matches++;
    }

    pEntry = isdpEntryGetNext(key);
    while (pEntry != L7_NULLPTR)
    {
     if(num_intf_matches >= index)
     {
       if (pEntry->key.intIfNum == intIfNum)
       {
         memset(deviceId, 0, L7_ISDP_DEVICE_ID_LEN);
         osapiStrncpySafe(deviceId, pEntry->key.deviceId, L7_ISDP_DEVICE_ID_LEN);
         rc = L7_SUCCESS;
       }
       break;
     }
     num_intf_matches++;
     pEntry = isdpEntryGetNext(pEntry->key);
    } 
  }

  osapiSemaGive(isdpSemaphore);

  return rc;
}

/*****************************************************************
* @purpose  Determines the Device ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
****************************************************************/
L7_RC_t isdpDeviceIdGet(L7_uchar8 *isdp_device_id)
{
  if ( L7_NULLPTR == isdp_device_id)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_ISDP_COMPONENT_ID,
            "isdp_device_id param is not valid \n");
    return L7_FAILURE;
  }

  isdpDevIdGet(isdp_device_id);

  return L7_SUCCESS;

}


